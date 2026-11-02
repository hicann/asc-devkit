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
 * \file kernel_operator_vec_bilinearinterpolation_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_c220/kernel_operator_vec_bilinearinterpolation_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BILINEARINTERPOLATION_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_BILINEARINTERPOLATION_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_BILINEARINTERPOLATION_IMPL_H
#include "../../../include/basic_api/kernel_tensor.h"
#include "../../../include/basic_api/kernel_struct_brcb.h"
#include "../../../include/basic_api/kernel_struct_gather.h"
#include "../../../include/basic_api/kernel_struct_binary.h"
#include "../../../include/basic_api/kernel_struct_unary.h"
#include "../../../include/basic_api/kernel_operator_vec_binary_intf.h"
#include "../../../include/basic_api/kernel_operator_block_sync_intf.h"
#include "kernel_operator_vec_gather_impl.h"
#include "../../../include/basic_api/kernel_operator_vec_brcb_intf.h"
#include "../../../include/basic_api/kernel_operator_vec_binary_scalar_intf.h"
#if ASCENDC_CPU_DEBUG
#include "../kernel_check.h"
#endif

#pragma begin_pipe(V)
namespace AscendC {
constexpr uint32_t brcbEleNum = 8;

template <typename T, typename MaskT>
__aicore__ inline void BilinearInterpolationCalcImpl(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<uint32_t>& src0Offset,
    const LocalTensor<T>& src1, MaskT& mask, uint8_t hRepeat, bool repeatMode, uint16_t dstBlkStride, uint16_t vROffset,
    uint8_t vRepeat, const LocalTensor<uint8_t>& sharedTmpBuffer)
{
    using PrimType = PrimT<T>;
    auto sharedTmpBufferT = sharedTmpBuffer.ReinterpretCast<T>();
    GatherRepeatParams gatherbRepeatParams;
    uint8_t innerRepeatTimes = hRepeat * vRepeat;
    constexpr uint32_t eleCntOfOneRep = DEFAULT_REPEAT_STRIDE * ONE_BLK_SIZE / sizeof(PrimType);
    // gatherb
    GatherbImpl(
        (__ubuf__ uint16_t*)sharedTmpBufferT.GetPhyAddr(), (__ubuf__ uint16_t*)src0.GetPhyAddr(),
        (__ubuf__ uint32_t*)src0Offset.GetPhyAddr(), src0.GetSize(), innerRepeatTimes, gatherbRepeatParams);
    uint32_t posSrc1Brcb = hRepeat * vRepeat * DEFAULT_REPEAT_STRIDE * ONE_BLK_SIZE / sizeof(PrimType);
    BrcbRepeatParams brcbRepeatParams;
    Brcb(sharedTmpBufferT[posSrc1Brcb], src1, src1.GetSize() / brcbEleNum, brcbRepeatParams);
    PipeBarrier<PIPE_V>();
    // mul
    BinaryRepeatParams mulRepeatParams;
    if (repeatMode == false) {
        mulRepeatParams.src0RepStride = 1;
        mulRepeatParams.src0BlkStride = 0;
    }

    SetMaskCount();
    SetVectorMask<PrimType, MaskMode::COUNTER>(0, innerRepeatTimes * eleCntOfOneRep);
    Mul<T, false>(
        sharedTmpBufferT, sharedTmpBufferT[posSrc1Brcb], sharedTmpBufferT, mask, innerRepeatTimes, mulRepeatParams);
    SetMaskNorm();
    ResetMask();
    PipeBarrier<PIPE_V>();

    BinaryRepeatParams addRepeatParams;
    addRepeatParams.dstRepStride = 0;
    addRepeatParams.src1RepStride = 0;
    for (int i = 0; i < vRepeat; i++) {
        if (hRepeat > 1) {
            Add(sharedTmpBufferT[i * hRepeat * eleCntOfOneRep], sharedTmpBufferT[(i * hRepeat + 1) * eleCntOfOneRep],
                sharedTmpBufferT[i * hRepeat * eleCntOfOneRep], mask, hRepeat - 1, addRepeatParams);
        }
    }
    PipeBarrier<PIPE_V>();
    // copy out
    UnaryRepeatParams addsRepeatParams;
    addsRepeatParams.srcRepStride = hRepeat * DEFAULT_REPEAT_STRIDE;
    addsRepeatParams.dstBlkStride = dstBlkStride;
    addsRepeatParams.dstRepStride = vROffset * sizeof(PrimType) / ONE_BLK_SIZE;
    Adds(dst, sharedTmpBufferT, (PrimType)0, mask, vRepeat, addsRepeatParams);
}

template <typename T>
__aicore__ inline void BilinearInterpolationCalc(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<uint32_t>& src0Offset,
    const LocalTensor<T>& src1, uint64_t mask, uint8_t hRepeat, bool repeatMode, uint16_t dstBlkStride,
    uint16_t vROffset, uint8_t vRepeat, const LocalTensor<uint8_t>& sharedTmpBuffer)
{
    BilinearInterpolationCalcImpl(
        dst, src0, src0Offset, src1, mask, hRepeat, repeatMode, dstBlkStride, vROffset, vRepeat, sharedTmpBuffer);
}

template <typename T>
__aicore__ inline void BilinearInterpolationCalc(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<uint32_t>& src0Offset,
    const LocalTensor<T>& src1, uint64_t mask[], uint8_t hRepeat, bool repeatMode, uint16_t dstBlkStride,
    uint16_t vROffset, uint8_t vRepeat, const LocalTensor<uint8_t>& sharedTmpBuffer)
{
    BilinearInterpolationCalcImpl(
        dst, src0, src0Offset, src1, mask, hRepeat, repeatMode, dstBlkStride, vROffset, vRepeat, sharedTmpBuffer);
}
} // namespace AscendC
#pragma end_pipe
#endif // ASCENDC_MODULE_OPERATOR_VEC_BILINEARINTERPOLATION_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BILINEARINTERPOLATION_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BILINEARINTERPOLATION_IMPL_H__
#endif
