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
 * \file softmax_common_broadcast.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/activation/softmax/softmax_common/softmax_common_broadcast.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/softmax.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_COMMON_BROADCAST_H__
#endif
#ifndef IMPL_ACTIVATION_SOFTMAX_SOFTMAX_COMMON_BROADCAST_H
#define IMPL_ACTIVATION_SOFTMAX_SOFTMAX_COMMON_BROADCAST_H

#include "../../../../../../include/basic_api/kernel_basic_intf.h"
#include "softmax_common_utils.h"

namespace AscendC {

template <typename T>
__ASC_USE_RESERVED_UBUF__(3510, "SoftMax is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void AlignedBrcbImpl(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t brcbCount)
{
    T scalarList[SCALAR_STACK_DEPTH] = {0};

    SetVectorMask<T>(brcbCount);
    for (uint32_t j = 0; j < SCALAR_STACK_DEPTH; j++) {
        scalarList[j] = srcLocal.GetValue(j);
    }
    for (uint32_t k = 0; k < SCALAR_STACK_DEPTH; k++) {
        Duplicate<T, false>(
            dstLocal[k * brcbCount], scalarList[k], MASK_PLACEHOLDER, DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE,
            DEFAULT_REPEAT_STRIDE);
    }
}

__ASC_USE_RESERVED_UBUF__(3510, "SoftMax is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void ContinuousColumnBrcbImpl(
    const LocalTensor<float>& dstLocal, const LocalTensor<float>& srcLocal, const uint32_t& repeat,
    const uint32_t& brcbCount)
{
    float scalarList[SCALAR_STACK_DEPTH] = {0};
    SetVectorMask<float>(brcbCount);
    const uint32_t rangeM = repeat / SCALAR_STACK_DEPTH;
    const uint32_t tailM = repeat % SCALAR_STACK_DEPTH;
    uint32_t offset = 0;

    for (uint32_t i = 0; i < rangeM; i++) {
        offset = i * brcbCount * SCALAR_STACK_DEPTH;
        for (uint32_t j = 0; j < SCALAR_STACK_DEPTH; j++) {
            scalarList[j] = srcLocal.GetValue(offset + j);
        }
        for (uint32_t k = 0; k < SCALAR_STACK_DEPTH; k++) {
            Duplicate<float, false>(
                dstLocal[offset + k * brcbCount], scalarList[k], MASK_PLACEHOLDER, DEFAULT_BLK_STRIDE,
                DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
        }
    }
    if (tailM != 0) {
        offset = rangeM * brcbCount * SCALAR_STACK_DEPTH;
        for (uint32_t j = 0; j < tailM; j++) {
            scalarList[j] = srcLocal.GetValue(offset + j);
        }
        for (uint32_t k = 0; k < tailM; k++) {
            Duplicate<float, false>(
                dstLocal[offset + k * brcbCount], scalarList[k], MASK_PLACEHOLDER, DEFAULT_BLK_STRIDE,
                DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
        }
    }
}

__ASC_USE_RESERVED_UBUF__(3510, "SoftMax is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void AlignedColumnBrcbImpl(
    const LocalTensor<float>& dstLocal, const LocalTensor<float>& srcLocal, const uint32_t& repeat,
    const uint32_t& brcbCount)
{
    float scalarList[SCALAR_STACK_DEPTH] = {0};
    SetVectorMask<float>(brcbCount);
    const uint32_t rangeM = repeat / SCALAR_STACK_DEPTH;
    const uint32_t tailM = repeat % SCALAR_STACK_DEPTH;
    uint32_t offset = 0;

    for (uint32_t i = 0; i < rangeM; i++) {
        offset = i * brcbCount * SCALAR_STACK_DEPTH;
        for (uint32_t j = 0; j < SCALAR_STACK_DEPTH; j++) {
            scalarList[j] = srcLocal.GetValue(offset + j * brcbCount);
        }
        for (uint32_t k = 0; k < SCALAR_STACK_DEPTH; k++) {
            Duplicate<float, false>(
                dstLocal[offset + k * brcbCount], scalarList[k], MASK_PLACEHOLDER, DEFAULT_BLK_STRIDE,
                DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
        }
    }
    if (tailM != 0) {
        offset = rangeM * brcbCount * SCALAR_STACK_DEPTH;
        for (uint32_t j = 0; j < tailM; j++) {
            scalarList[j] = srcLocal.GetValue(offset + j * brcbCount);
        }
        for (uint32_t k = 0; k < tailM; k++) {
            Duplicate<float, false>(
                dstLocal[offset + k * brcbCount], scalarList[k], MASK_PLACEHOLDER, DEFAULT_BLK_STRIDE,
                DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
        }
    }
}

__ASC_USE_RESERVED_UBUF__(3510, "SoftMax is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void BroadCastNZImpl(
    const LocalTensor<float>& dst, const LocalTensor<float>& src, const uint32_t srcM)
{
    uint8_t repeat = srcM / DEFAULT_REPEAT_STRIDE;
    for (uint8_t i = 0; i < repeat; i++) {
        Muls<float, false>(
            dst[i * B16_BYTE_SIZE * FLOAT_REPEAT_SIZE], src[i * B16_BYTE_SIZE * FLOAT_REPEAT_SIZE], 1.0f,
            MASK_PLACEHOLDER, B16_BYTE_SIZE, {1, 0, DEFAULT_REPEAT_STRIDE, 0});
    }
    PipeBarrier<PIPE_V>();

    uint64_t dstList[NCHW_CONV_ADDR_LIST_SIZE];
    uint64_t srcList[NCHW_CONV_ADDR_LIST_SIZE];
    for (int32_t i = 0; i < NCHW_CONV_ADDR_LIST_SIZE; i++) {
        dstList[i] = (uint64_t)dst[i * FLOAT_NUM_PER_BLK].GetPhyAddr();
        srcList[i] = (uint64_t)src[i * FLOAT_NUM_PER_BLK].GetPhyAddr();
    }
    TransDataTo5HDParams transDataParams;
    transDataParams.repeatTimes = repeat;
    if (transDataParams.repeatTimes > 1) {
        transDataParams.dstRepStride = B16_BYTE_SIZE * DEFAULT_REPEAT_STRIDE;
        transDataParams.srcRepStride = B16_BYTE_SIZE * DEFAULT_REPEAT_STRIDE;
    }
    TransDataTo5HD<float>(dstList, srcList, transDataParams);
}

template <typename T>
__ASC_USE_RESERVED_UBUF__(3510, "SoftMax is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void BroadCastLastCompute(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const BroadCastLastND& brcParam,
    const uint32_t scalarStackDepth, const uint32_t index)
{
    const uint32_t elementNumPerRep = ONE_REPEAT_BYTE_SIZE / sizeof(T);
    const uint32_t rangeK = brcParam.dstK / elementNumPerRep;
    const uint32_t tailK = brcParam.dstK % elementNumPerRep;
    T scalarList[SCALAR_STACK_DEPTH] = {0};

    for (uint32_t j = 0; j < scalarStackDepth; j++) {
        scalarList[j] = src[(index * SCALAR_STACK_DEPTH + j) * brcParam.srcK].GetValue(0);
    }
    for (uint32_t j = 0; j < rangeK; j++) {
        for (uint32_t k = 0; k < scalarStackDepth; k++) {
            Duplicate(
                dst[j * elementNumPerRep + (index * SCALAR_STACK_DEPTH + k) * brcParam.dstK], scalarList[k],
                elementNumPerRep, DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
        }
    }
    if (tailK != 0) {
        for (uint32_t k = 0; k < scalarStackDepth; k++) {
            Duplicate(
                dst[rangeK * elementNumPerRep + (index * SCALAR_STACK_DEPTH + k) * brcParam.dstK], scalarList[k], tailK,
                DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
        }
    }
}

}; // namespace AscendC
#endif // IMPL_ACTIVATION_SOFTMAX_SOFTMAX_COMMON_BROADCAST_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_COMMON_BROADCAST_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_COMMON_BROADCAST_H__
#endif
