/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file hccl_common.h
 * \brief
 */
#ifndef IMPL_HCCL_COMMON_H
#define IMPL_HCCL_COMMON_H

namespace AscendC {
__aicore__ inline void FlushDataCache(GlobalTensor<int64_t>& globalHcclMsgArea, __gm__ void* gmAddr)
{
    AscendC::Barrier();
    globalHcclMsgArea.SetGlobalBuffer((__gm__ int64_t*)gmAddr);
    __asm__("NOP");
    DataCacheCleanAndInvalid<int64_t, CacheLine::SINGLE_CACHE_LINE, DcciDst::CACHELINE_OUT>(globalHcclMsgArea);
    DataSyncBarrier<MemDsbT::ALL>();
}

__aicore__ inline void FlushDataCache(__gm__ void* gmAddr)
{
    GlobalTensor<int64_t> globalHcclMsgArea;
    FlushDataCache(globalHcclMsgArea, gmAddr);
}

__aicore__ inline void FlushDataCacheAicpu(GlobalTensor<int64_t>& globalHcclMsgArea, __gm__ void* gmAddr)
{
    AscendC::Barrier();
    globalHcclMsgArea.SetGlobalBuffer((__gm__ int64_t*)gmAddr);
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    // Throttle A5 AICPU polling reads to avoid too many read requests per unit time causing a bus hang.
    constexpr uint32_t a5AicpuPollingReadThrottleNopCycles = 1000U;
    AscendC::Nop<a5AicpuPollingReadThrottleNopCycles>();
#else
    __asm__("NOP");
#endif
    DataCacheCleanAndInvalid<int64_t, CacheLine::SINGLE_CACHE_LINE, DcciDst::CACHELINE_OUT>(globalHcclMsgArea);
    DataSyncBarrier<MemDsbT::ALL>();
}

__aicore__ inline void FlushDataCacheAicpu(__gm__ void* gmAddr)
{
    GlobalTensor<int64_t> globalHcclMsgArea;
    FlushDataCacheAicpu(globalHcclMsgArea, gmAddr);
}

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510

template <typename T>
__aicore__ inline T ReadHBMData(__gm__ T* addr)
{
    Barrier();
    Nop<800>();

    if constexpr (SupportBytes<T, 8>()) {
        return ReadGmBypassDCache(reinterpret_cast<__gm__ uint64_t*>(addr));
    } else if constexpr (SupportBytes<T, 4>()) {
        return ReadGmBypassDCache(reinterpret_cast<__gm__ uint32_t*>(addr));
    } else if constexpr (SupportBytes<T, 2>()) {
        return ReadGmBypassDCache(reinterpret_cast<__gm__ uint16_t*>(addr));
    } else {
        return ReadGmBypassDCache(reinterpret_cast<__gm__ uint8_t*>(addr));
    }

    DataSyncBarrier<MemDsbT::ALL>();
}

template <typename T>
__aicore__ inline void WriteHBMData(__gm__ T* addr, T value)
{
    Barrier();
    Nop();

    if constexpr (SupportBytes<T, 8>()) {
        WriteGmBypassDCache(reinterpret_cast<__gm__ uint64_t*>(addr), (uint64_t)value);
    } else if constexpr (SupportBytes<T, 4>()) {
        WriteGmBypassDCache(reinterpret_cast<__gm__ uint32_t*>(addr), (uint32_t)value);
    } else if constexpr (SupportBytes<T, 2>()) {
        WriteGmBypassDCache(reinterpret_cast<__gm__ uint16_t*>(addr), (uint16_t)value);
    } else {
        WriteGmBypassDCache(reinterpret_cast<__gm__ uint8_t*>(addr), (uint8_t)value);
    }

    DataSyncBarrier<MemDsbT::ALL>();
}

#endif

} // namespace AscendC

#endif
