/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <iostream>
#include "sim_npu.h"
#include "sim_world.h"
#include "log.h"
#include "exception_util.h"
#include "invalid_params_exception.h"

namespace HcclSim {
uint32_t notifyIdGen = 0;
uint32_t streamIdGen = 0;
std::map<BufferType, uint32_t> g_BufferType2Index = {
    {BufferType::INPUT, 0}, {BufferType::OUTPUT, 1}, {BufferType::CCL, 2}};

void SimNpu::InitSimNpuRes(const NpuPos& pos)
{
    npuPos_ = pos;
    InitMemLayOut();
    InitStreamRes();
    InitNotifyRes();
}

void SimNpu::InitMemLayOut()
{
    memLayout_.clear();
    uint64_t addr = SIM_MEM_BLOCK_SIZE;
    uint64_t startAddr = (uint64_t)addr & SIM_MEM_MASKER;
    MemBlock inMemBlock{BufferType::INPUT, addr, 0};
    memLayout_.push_back(inMemBlock);
    addr += SIM_MEM_BLOCK_SIZE;

    startAddr = (uint64_t)addr & SIM_MEM_MASKER;
    MemBlock outMemBlock{BufferType::OUTPUT, addr, 0};
    memLayout_.push_back(outMemBlock);
    addr += SIM_MEM_BLOCK_SIZE;

    startAddr = (uint64_t)addr & SIM_MEM_MASKER;
    MemBlock cclMemBlock{BufferType::CCL, addr, (uint64_t)SIZE_200MB};
    memLayout_.push_back(cclMemBlock);
}

void SimNpu::InitStreamRes()
{
    streamIdGen = 0;
    streams_.clear();
    streams_.reserve(MAX_STREAM_COUNT);
    for (int i = 0; i < MAX_STREAM_COUNT; i++) {
        streams_.push_back(SimStream(streamIdGen++));
    }
}

void SimNpu::InitNotifyRes()
{
    notifys_.clear();
    notifys_.reserve(MAX_NOTIFY_COUNT);
    for (int i = 0; i < MAX_NOTIFY_COUNT; i++) {
        notifys_.push_back(SimNotify(notifyIdGen++));
    }
}

uint64_t SimNpu::AllocMemory(BufferType bufferType, uint64_t len)
{
    if (g_BufferType2Index.count(bufferType) == 0 || len > SIM_MEM_BLOCK_SIZE) {
        Hccl::THROW<Hccl::InvalidParamsException>(
            "[SimNpu::AllocMemory] failed, bufferType[%s], len[%llu]", bufferType.Describe().c_str(), len);
    }
    auto index = g_BufferType2Index[bufferType];
    MemBlock& memBlock = memLayout_[index];
    memBlock.size = len;
    return memBlock.startAddr;
}

MemBlock SimNpu::GetMemBlock(BufferType bufferType)
{
    if (g_BufferType2Index.count(bufferType) == 0) {
        Hccl::THROW<Hccl::InvalidParamsException>(
            "[SimNpu::GetMemBlock] failed, bufferType[%s]", bufferType.Describe().c_str());
    }
    auto index = g_BufferType2Index[bufferType];
    return memLayout_[index];
}

void* SimNpu::AllocMainStream()
{
    if (streams_.size() <= 0) {
        HCCL_ERROR("[SimNpu::AllocMainStream] streams_ is empty");
        return nullptr;
    }

    SimStream& stream = streams_[0];
    if (stream.IsAllocated()) {
        HCCL_ERROR("[SimNpu::AllocSlaveStream] main stream is already allocated");
        return nullptr;
    }

    stream.Allocate();
    return reinterpret_cast<void*>(&stream);
}

void* SimNpu::AllocSlaveStream()
{
    for (int i = 0; i < MAX_STREAM_COUNT; i++) {
        SimStream& stream = streams_[i];
        if (stream.IsAllocated()) {
            continue;
        }
        stream.Allocate();
        return reinterpret_cast<void*>(&stream);
    }

    HCCL_ERROR("[SimNpu::AllocSlaveStream] alloc slave stream failed");
    return nullptr;
}

void* SimNpu::AllocNotify()
{
    for (int i = 0; i < MAX_NOTIFY_COUNT; i++) {
        SimNotify& notify = notifys_[i];
        if (notify.IsAllocated()) {
            continue;
        }
        notify.Allocate();
        return reinterpret_cast<void*>(&notify);
    }

    HCCL_ERROR("[SimNpu::AllocNotify] alloc notify failed");
    return nullptr;
}

void SimNpu::ReleaseStream(void* stream)
{
    if (stream == nullptr) {
        Hccl::THROW<Hccl::InvalidParamsException>("[SimNpu::ReleaseStream] stream is nullptr");
    }
    SimStream* simStream = reinterpret_cast<SimStream*>(stream);
    simStream->Release();
}

void SimNpu::ReleaseNotify(void* notify)
{
    if (notify == nullptr) {
        Hccl::THROW<Hccl::InvalidParamsException>("[SimNpu::ReleaseNotify] notify is nullptr");
    }
    SimNotify* simNotify = reinterpret_cast<SimNotify*>(notify);
    simNotify->Release();
}

void SimNpu::SetDevType(DevType devType) { devType_ = devType; }

DevType SimNpu::GetDevType() { return devType_; }

HcclResult SimNpu::GetSlice(uint64_t addr, uint64_t size, DataSlice& dataSlice)
{
    for (uint32_t index = 0; index < memLayout_.size(); index++) {
        if (addr < memLayout_[index].startAddr) {
            continue;
        }

        uint64_t endAddr = memLayout_[index].startAddr + memLayout_[index].size;
        if (endAddr < addr) {
            continue;
        }

        if (addr + size > endAddr) {
            HCCL_ERROR("[SimNpu::GetSlice] addr[%#llx] + size[%x] > endAddr[%#llx]", addr, size, endAddr);
            return HcclResult::HCCL_E_MEMORY;
        }

        dataSlice.SetBufferType(memLayout_[index].bufferType);
        dataSlice.SetOffset(addr - memLayout_[index].startAddr);
        dataSlice.SetSize(size);
        return HcclResult::HCCL_SUCCESS;
    }

    HCCL_ERROR("[SimNpu::GetSlice] GetSlice failed, addr[%#llx], size[%x]", addr, size);
    return HcclResult::HCCL_E_MEMORY;
}

HcclResult SimNpu::GetSlice(uint64_t addr, uint64_t dataCount, const HcclDataType dataType, DataSlice& dataSlice)
{
    uint64_t size = dataCount * SIZE_TABLE[dataType];
    CHK_RET(GetSlice(addr, size, dataSlice));

    return HcclResult::HCCL_SUCCESS;
}
} // namespace HcclSim
