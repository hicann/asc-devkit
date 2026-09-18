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
 * \file kernel_pop_stack_buffer.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_pop_stack_buffer.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_POP_STACK_BUFFER_H__
#endif
#ifndef TIKCFW_IMPL_KERNEL_POP_STACK_BUFFER_H
#define TIKCFW_IMPL_KERNEL_POP_STACK_BUFFER_H

#include "kernel_tpipe_impl.h"
namespace AscendC {
template <TPosition pos>
__aicore__ inline uint64_t GetEndAddress()
{
    Hardware hardType = GetPhyType(pos);
    ASCENDC_ASSERT((hardType == Hardware::UB), { KERNEL_LOG(KERNEL_ERROR, "hardType should be UB"); });
    // the last 64B reserved for ub kfc msg send
#if __NPU_ARCH__ == 2201
    return TOTAL_UB_SIZE - sizeof(KfcMsg);
#elif (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162)
    return TMP_UB_OFFSET;
#else
    return TOTAL_UB_SIZE;
#endif
}

template <typename T, TPosition pos>
__aicore__ inline bool PopStackBuffer(LocalTensor<T>& pop)
{
    TBuffAddr addr;
    addr.logicPos = (int8_t)pos;
    ASCENDC_ASSERT((GetTPipePtr() != nullptr), { KERNEL_LOG(KERNEL_ERROR, "GetTPipePtr is nullptr"); });
    uint64_t endAddress = GetEndAddress<pos>();
    uint64_t queEndAddress = GetTPipePtr()->GetQueueEndAddress<pos>();
    ASCENDC_ASSERT((queEndAddress % ONE_BLK_SIZE == 0), {
        KERNEL_LOG(KERNEL_ERROR, "queEndAddress is %lu, which must be 32B aligned", queEndAddress);
    });
    ASCENDC_ASSERT((endAddress > queEndAddress), {
        KERNEL_LOG(
            KERNEL_ERROR, "PopStackBuffer failed, local memory has used %lu bytes, there is no enough memory to pop",
            queEndAddress);
    });
    addr.dataLen = static_cast<uint32_t>(endAddress - queEndAddress);
    addr.bufferAddr = queEndAddress;
#ifdef ASCENDC_CPU_DEBUG
    auto absAddr = GetTPipePtr()->GetBaseAddr(static_cast<int8_t>(pos));
    addr.absAddr = absAddr + addr.bufferAddr;
    AscendCBufInit(
        static_cast<uint8_t>(pos), static_cast<uint8_t>(1), static_cast<uint8_t>(1), (uint64_t)addr.absAddr,
        static_cast<uint64_t>(addr.dataLen));
    AscendCBufInit(
        static_cast<uint8_t>(pos), static_cast<uint8_t>(1), static_cast<uint8_t>(1), (uint64_t)(absAddr + endAddress),
        static_cast<uint64_t>(ONE_BLK_SIZE << 1));
#endif // ASCENDC_CPU_DEBUG
    pop.SetAddr(addr);
    return true;
}

template <TPosition pos>
__aicore__ inline bool PopStackBuffer(TBuf<pos>& popBuffer, TBufType& bufStart)
{
    uint64_t endAddress = GetEndAddress<pos>();
    uint64_t queEndAddress = GetTPipePtr()->GetQueueEndAddress<pos>();
    ASCENDC_ASSERT((queEndAddress % ONE_BLK_SIZE == 0), {
        KERNEL_LOG(KERNEL_ERROR, "queEndAddress is %lu, which must be 32B aligned", queEndAddress);
    });
    ASCENDC_ASSERT((endAddress > queEndAddress), {
        KERNEL_LOG(
            KERNEL_ERROR, "PopStackBuffer failed, local memory has used %lu bytes, there is no enough memory to pop",
            queEndAddress);
    });
    uint32_t dataLen = static_cast<uint32_t>(endAddress - queEndAddress);
    bufStart.address = queEndAddress;
    bufStart.dataLen = dataLen;
    popBuffer.SetTpipeBuf(&bufStart, dataLen);
#ifdef ASCENDC_CPU_DEBUG
    auto absAddr = GetTPipePtr()->GetBaseAddr(static_cast<int8_t>(pos));
    AscendCBufInit(
        static_cast<uint8_t>(pos), static_cast<uint8_t>(1), static_cast<uint8_t>(1),
        (uint64_t)(absAddr + queEndAddress), static_cast<uint64_t>(dataLen));
    AscendCBufInit(
        static_cast<uint8_t>(pos), static_cast<uint8_t>(1), static_cast<uint8_t>(1), (uint64_t)(absAddr + endAddress),
        static_cast<uint64_t>(ONE_BLK_SIZE << 1));
#endif
    return true;
}
} // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_POP_STACK_BUFFER_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_POP_STACK_BUFFER_H__
#endif
