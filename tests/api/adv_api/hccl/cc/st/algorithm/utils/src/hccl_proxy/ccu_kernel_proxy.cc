/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_kernel_proxy.h"
#include "ccu_primitives_stub.h"

#include <algorithm>
#include <exception>

namespace HcclSim {
namespace CcuSt {

RegisterManager& RegisterManager::Global()
{
    static RegisterManager manager;
    return manager;
}

void RegisterManager::Reset()
{
    std::lock_guard<std::mutex> lock(mutex_);
    nextKernelHandle_ = 1;
    kernels_.clear();
    instances_.clear();
    lastCompiledGroup_.clear();
    threadLocalCompiledGroup_.clear();
    CompilerContext::SetCurrent(nullptr);
    std::lock_guard<std::mutex> channelLock(channelMapMutex_);
    channelMap_.clear();
}

Result RegisterManager::RegisterStart(InstanceHandle instance)
{
    std::lock_guard<std::mutex> lock(mutex_);
    InstanceState& state = instances_[instance];
    if (state.registering) {
        return Result::STATE_ERROR;
    }
    state.registering = true;
    state.currentGroup.clear();
    return Result::SUCCESS;
}

Result RegisterManager::Register(
    InstanceHandle instance, const std::string& name, const KernelFunction& function, void* kernelArg,
    KernelHandle& handle)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto stateIterator = instances_.find(instance);
        if (stateIterator == instances_.end() || !stateIterator->second.registering || !function) {
            return Result::STATE_ERROR;
        }
    }

    KernelRecord record;
    record.name = name;
    CompilerContext compiler(record.program);
    CompilerContext::SetCurrent(&compiler);
    Result result = Result::SUCCESS;
    try {
        result = function(kernelArg);
    } catch (const std::exception&) {
        result = Result::STATE_ERROR;
    } catch (...) {
        result = Result::STATE_ERROR;
    }
    CompilerContext::SetCurrent(nullptr);
    if (result != Result::SUCCESS) {
        return result;
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        handle = nextKernelHandle_++;
        kernels_.emplace(handle, std::move(record));
        auto stateIterator = instances_.find(instance);
        if (stateIterator != instances_.end()) {
            stateIterator->second.currentGroup.push_back(handle);
        }
    }
    return Result::SUCCESS;
}

Result RegisterManager::RegisterEnd(InstanceHandle instance)
{
    std::vector<KernelHandle> currentGroup;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto stateIterator = instances_.find(instance);
        if (stateIterator == instances_.end() || !stateIterator->second.registering) {
            return Result::STATE_ERROR;
        }
        stateIterator->second.registering = false;
        currentGroup = stateIterator->second.currentGroup;
    }
    for (KernelHandle handle : currentGroup) {
        Result result = Result::SUCCESS;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            auto kernelIterator = kernels_.find(handle);
            if (kernelIterator == kernels_.end()) {
                return Result::NOT_FOUND;
            }
            result = CompilerContext(kernelIterator->second.program).Finalize();
        }
        if (result != Result::SUCCESS) {
            return result;
        }
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (KernelHandle handle : currentGroup) {
            auto kernelIterator = kernels_.find(handle);
            if (kernelIterator != kernels_.end()) {
                kernelIterator->second.compiled = true;
            }
        }
    }
    threadLocalCompiledGroup_ = currentGroup;
    return Result::SUCCESS;
}

const Program* RegisterManager::GetProgram(KernelHandle handle) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iterator = kernels_.find(handle);
    if (iterator == kernels_.end() || !iterator->second.compiled) {
        return nullptr;
    }
    return &iterator->second.program;
}

const std::vector<KernelHandle>& RegisterManager::GetLastCompiledGroup() const { return threadLocalCompiledGroup_; }

bool RegisterManager::IsRegistering(InstanceHandle instance) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iterator = instances_.find(instance);
    return iterator != instances_.end() && iterator->second.registering;
}

std::map<ChannelHandle, std::pair<uint32_t, uint32_t>> RegisterManager::channelMap_;
std::mutex RegisterManager::channelMapMutex_;
thread_local std::vector<KernelHandle> RegisterManager::threadLocalCompiledGroup_;

void RegisterManager::RegisterChannelMap(ChannelHandle handle, uint32_t localRank, uint32_t remoteRank)
{
    std::lock_guard<std::mutex> lock(channelMapMutex_);
    channelMap_.emplace(handle, std::make_pair(localRank, remoteRank));
}

bool RegisterManager::GetChannelRanks(ChannelHandle handle, uint32_t& localRank, uint32_t& remoteRank)
{
    std::lock_guard<std::mutex> lock(channelMapMutex_);
    auto it = channelMap_.find(handle);
    if (it != channelMap_.end()) {
        localRank = it->second.first;
        remoteRank = it->second.second;
        return true;
    }
    return false;
}

std::vector<TaskParam> PackTaskArgs(const std::vector<uint64_t>& taskArgs)
{
    std::vector<TaskParam> taskParams;
    for (size_t offset = 0; offset < taskArgs.size(); offset += TASK_PARAM_SLOT_COUNT) {
        TaskParam taskParam;
        taskParam.copiedArgCount =
            static_cast<uint32_t>(std::min<size_t>(TASK_PARAM_SLOT_COUNT, taskArgs.size() - offset));
        for (uint32_t index = 0; index < taskParam.copiedArgCount; ++index) {
            taskParam.args[index] = taskArgs[offset + index];
        }
        taskParams.push_back(taskParam);
    }
    return taskParams;
}

ChannelHandle MakeChannel(uint32_t localRank, uint32_t remoteRank)
{
    return (static_cast<uint64_t>(localRank) << 32U) | remoteRank;
}

uint32_t ChannelLocalRank(ChannelHandle channel)
{
    uint32_t localRank = 0;
    uint32_t remoteRank = 0;
    if (RegisterManager::GetChannelRanks(channel, localRank, remoteRank)) {
        return localRank;
    }
    return static_cast<uint32_t>(channel >> 32U);
}

uint32_t ChannelRemoteRank(ChannelHandle channel)
{
    uint32_t localRank = 0;
    uint32_t remoteRank = 0;
    if (RegisterManager::GetChannelRanks(channel, localRank, remoteRank)) {
        return remoteRank;
    }
    return static_cast<uint32_t>(channel & 0xffffffffULL);
}

} // namespace CcuSt
} // namespace HcclSim
