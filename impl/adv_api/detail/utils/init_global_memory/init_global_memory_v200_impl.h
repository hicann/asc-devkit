/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file init_global_memory_v200_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/utils/init_global_memory/init_global_memory_v200_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/utils/init_global_memory.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_UTILS_INIT_GLOBAL_MEMORY_INIT_GLOBAL_MEMORY_V200_IMPL_H__
#endif

#ifndef IMPL_UTILS_INIT_GLOBAL_MEMORY_INIT_GLOBAL_MEMORY_V200_IMPL_H
#define IMPL_UTILS_INIT_GLOBAL_MEMORY_INIT_GLOBAL_MEMORY_V200_IMPL_H

#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/basic_api/kernel_tensor.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/utils/init_global_memory/init_global_memory_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
template <typename T>
__aicore__ inline void InitGlobalMemoryImpl(GlobalTensor<T>& gmWorkspaceAddr, const uint64_t size, const T value)
{
    if ASCEND_IS_AIC {
        return;
    }
    CHECK_FUNC_HIGHLEVEL_API(InitGlobalMemory, (T), (gmWorkspaceAddr, size, value));
    LocalTensor<T> popBuffer;
    constexpr uint32_t MAX_REPEAT_LEN = 256;
    bool ret = PopStackBuffer<T, TPosition::LCM>(popBuffer);
    ASCENDC_ASSERT(ret, { KERNEL_LOG(KERNEL_ERROR, "No space left to allocate in Unified Buffer"); });
    constexpr uint32_t maxBurstSize = (MAX_REPEAT_TIMES * MAX_REPEAT_LEN) / sizeof(T);
    const uint32_t popSize = popBuffer.GetSize() >= maxBurstSize ? maxBurstSize : popBuffer.GetSize();
    ASCENDC_ASSERT((popSize > 0), { KERNEL_LOG(KERNEL_ERROR, "popSize must be larger than 0"); });
    const uint32_t round = size / popSize;
    const uint32_t tail = size % popSize;
    const uint32_t roundSize = round != 0 ? popSize : 0;
    Duplicate<T>(popBuffer, value, popSize);
    event_t eventIDVToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE3));
    SetFlag<HardEvent::V_MTE3>(eventIDVToMTE3);
    WaitFlag<HardEvent::V_MTE3>(eventIDVToMTE3);
    struct DataCopyParams repeatParams;
    repeatParams.blockCount = 1;
    uint32_t comOffset = 0;
    // compute the main block
    if ((roundSize * sizeof(T)) % ONE_BLK_SIZE == 0) {
        repeatParams.blockLen = static_cast<uint32_t>(roundSize * sizeof(T)) / ONE_BLK_SIZE;
        for (uint32_t index = 0; index < round; ++index) {
            DataCopy(gmWorkspaceAddr[comOffset], popBuffer, repeatParams);
            comOffset += roundSize;
        }
    } else {
        const uint32_t roundSizeExtra = roundSize * sizeof(T) % ONE_BLK_SIZE;
        const uint32_t roundSizeAlign = roundSize * sizeof(T) - roundSizeExtra;
        repeatParams.blockLen = static_cast<uint16_t>(roundSizeAlign) / ONE_BLK_SIZE;
        for (uint32_t index = 0; index < round; ++index) {
            DataCopy(gmWorkspaceAddr[comOffset], popBuffer, repeatParams);
            comOffset += roundSize;
            for (uint64_t i = comOffset - roundSizeExtra / sizeof(T); i < comOffset; ++i) {
                gmWorkspaceAddr.SetValue(i, value);
            }
        }
    }
    // compute the tail block
    if (tail != 0) {
        if ((tail * sizeof(T)) % ONE_BLK_SIZE == 0) {
            repeatParams.blockLen = static_cast<uint32_t>(tail * sizeof(T)) / ONE_BLK_SIZE;
            comOffset = round * roundSize;
            DataCopy(gmWorkspaceAddr[comOffset], popBuffer, repeatParams);
        } else {
            const uint32_t tailExtra = tail * sizeof(T) % ONE_BLK_SIZE;
            const uint32_t tailAlign = tail * sizeof(T) - tailExtra;
            repeatParams.blockLen = static_cast<uint16_t>(tailAlign) / ONE_BLK_SIZE;
            comOffset = round * roundSize;
            DataCopy(gmWorkspaceAddr[comOffset], popBuffer, repeatParams);
            for (uint64_t i = comOffset + tailAlign / sizeof(T); i < size; ++i) {
                gmWorkspaceAddr.SetValue(i, value);
            }
        }
    }
    PipeBarrier<PIPE_MTE3>();
}

template <typename T>
__aicore__ inline __in_pipe__(V)
    __out_pipe__(MTE3, S) void InitGlobalMemory(GlobalTensor<T>& gmWorkspaceAddr, const uint64_t size, const T value)
{
    InitGlobalMemoryImpl<T>(gmWorkspaceAddr, size, value);
}
} // namespace AscendC
#endif // IMPL_UTILS_INIT_GLOBAL_MEMORY_INIT_GLOBAL_MEMORY_V200_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_UTILS_INIT_GLOBAL_MEMORY_INIT_GLOBAL_MEMORY_V200_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_UTILS_INIT_GLOBAL_MEMORY_INIT_GLOBAL_MEMORY_V200_IMPL_H__
#endif
