/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_CCU_KERNEL_PROXY_H
#define HCCL_CCU_KERNEL_PROXY_H

#include "ccu_st_types.h"

#include <functional>
#include <mutex>
#include <string>

namespace HcclSim {
namespace CcuSt {

class RegisterManager {
public:
    using KernelFunction = std::function<Result(void*)>;

    static RegisterManager& Global();
    void Reset();

    Result RegisterStart(InstanceHandle instance);
    Result Register(
        InstanceHandle instance, const std::string& name, const KernelFunction& function, void* kernelArg,
        KernelHandle& handle);
    Result RegisterEnd(InstanceHandle instance);

    const Program* GetProgram(KernelHandle handle) const;
    const std::vector<KernelHandle>& GetLastCompiledGroup() const;
    bool IsRegistering(InstanceHandle instance) const;

    static void RegisterChannelMap(ChannelHandle handle, uint32_t localRank, uint32_t remoteRank);
    static bool GetChannelRanks(ChannelHandle handle, uint32_t& localRank, uint32_t& remoteRank);

private:
    struct KernelRecord {
        std::string name;
        Program program;
        bool compiled{false};
    };
    struct InstanceState {
        bool registering{false};
        std::vector<KernelHandle> currentGroup;
    };

    KernelHandle nextKernelHandle_{1};
    std::map<KernelHandle, KernelRecord> kernels_;
    std::map<InstanceHandle, InstanceState> instances_;
    std::vector<KernelHandle> lastCompiledGroup_;
    mutable std::mutex mutex_;
    static std::map<ChannelHandle, std::pair<uint32_t, uint32_t>> channelMap_;
    static std::mutex channelMapMutex_;
    static thread_local std::vector<KernelHandle> threadLocalCompiledGroup_;
};

ChannelHandle MakeChannel(uint32_t localRank, uint32_t remoteRank);
uint32_t ChannelLocalRank(ChannelHandle channel);
uint32_t ChannelRemoteRank(ChannelHandle channel);

using ArgConverter = std::function<void*(const void*, uint32_t)>;
using ArgDeleter = std::function<void(void*)>;

void RegisterCaptureFunction(
    const std::string& name, const RegisterManager::KernelFunction& capture, const ArgConverter& convert,
    const ArgDeleter& deleter);

} // namespace CcuSt
} // namespace HcclSim

#endif // HCCL_CCU_KERNEL_PROXY_H
