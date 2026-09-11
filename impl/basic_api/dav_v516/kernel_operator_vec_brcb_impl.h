/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file kernel_operator_vec_brcb_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_v516/kernel_operator_vec_brcb_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_vec_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BRCB_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_BRCB_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_BRCB_IMPL_H

namespace AscendC {
/* **************************************************************************************************
 * Brcb                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void BrcbB64Impl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint8_t repeatTime, const BrcbRepeatParams& repeatParams,
    int32_t dstBlkStride)
{
    int32_t dstRepOffset = 0;
    int32_t sreg0 = 0;
    uint32_t sreg1 = ONE_BLK_FLOAT_NUM;
    Reg::RegTensor<uint32_t> srcRegLow;
    Reg::RegTensor<uint32_t> srcRegHigh;
    Reg::RegTensor<uint32_t> dstReg;
    Reg::RegTensor<uint32_t> tmpReg;
    Reg::MaskReg preg = Reg::UpdateMask<uint32_t>(sreg1);
    for (uint16_t i = 0; i < (uint16_t)repeatTime; ++i) {
        sreg0 = (int32_t)repeatParams.dstRepStride * i * ONE_BLK_FLOAT_NUM;
        for (uint16_t j = 0; j < (uint16_t)BRCB_BROADCAST_NUMBER; ++j) {
            dstRepOffset = sreg0 + dstBlkStride * j * ONE_BLK_FLOAT_NUM;
            Reg::LoadAlign<uint32_t, Reg::LoadDist::DIST_BRC_B32>(
                srcRegLow, (__ubuf__ uint32_t*)src + (i * BRCB_BROADCAST_NUMBER + j) * 2);
            Reg::LoadAlign<uint32_t, Reg::LoadDist::DIST_BRC_B32>(
                srcRegHigh, (__ubuf__ uint32_t*)src + (i * BRCB_BROADCAST_NUMBER + j) * 2 + 1);
            Reg::Duplicate(srcRegLow, srcRegLow, preg);
            Reg::Duplicate(srcRegHigh, srcRegHigh, preg);
            Reg::Interleave(dstReg, tmpReg, srcRegLow, srcRegHigh);
            Reg::StoreAlign((__ubuf__ uint32_t*)dst + dstRepOffset, dstReg, preg);
        }
    }
}

template <typename T>
__aicore__ inline void BrcbB8Impl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint8_t repeatTime, const BrcbRepeatParams& repeatParams,
    int32_t dstBlkStride)
{
    int32_t dstRepOffset = 0;
    constexpr uint32_t oneBlkSize = ONE_BLK_SIZE / sizeof(T);
    int32_t sreg0 = 0;
    uint32_t sreg1 = oneBlkSize;
    Reg::RegTensor<T> srcReg;
    Reg::MaskReg preg = Reg::UpdateMask<T>(sreg1);
    for (uint16_t i = 0; i < (uint16_t)repeatTime; ++i) {
        sreg0 = (int32_t)repeatParams.dstRepStride * i * oneBlkSize;
        for (uint16_t j = 0; j < (uint16_t)BRCB_BROADCAST_NUMBER; ++j) {
            dstRepOffset = sreg0 + dstBlkStride * j * oneBlkSize;
            Reg::LoadAlign<T, Reg::LoadDist::DIST_BRC_B8>(srcReg, src + i * BRCB_BROADCAST_NUMBER + j);
            Reg::Duplicate(srcReg, srcReg, preg);
            Reg::StoreAlign(dst + dstRepOffset, srcReg, preg);
        }
    }
}

template <typename T>
__aicore__ inline void BrcbCommonImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint8_t repeatTime, const BrcbRepeatParams& repeatParams,
    int32_t dstBlkStride)
{
    int32_t dstRepOffset = 0;
    Reg::RegTensor<T> srcReg;
    constexpr uint32_t oneBlkSize = ONE_BLK_SIZE / sizeof(T);
    Reg::MaskReg fullMask = Reg::CreateMask<T, Reg::MaskPattern::ALL>();
    for (uint16_t i = 0; i < (uint16_t)repeatTime; ++i) {
        if constexpr (sizeof(T) == 2) {
            Reg::LoadAlign<T, Reg::LoadDist::DIST_E2B_B16>(srcReg, src + i * BRCB_BROADCAST_NUMBER);
        } else {
            Reg::LoadAlign<T, Reg::LoadDist::DIST_E2B_B32>(srcReg, src + i * BRCB_BROADCAST_NUMBER);
        }
        Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            dst + i * repeatParams.dstRepStride * oneBlkSize, srcReg, dstBlkStride, fullMask);
    }
}

template <typename T>
__aicore__ inline void BrcbImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint8_t repeatTime, const BrcbRepeatParams& repeatParams)
{
    static_assert(
        SupportBytes<T, 1, 2, 4, 8>(),
        "Failed to check dtype in Brcb, current api support dtype"
        "combination is src and dst both: uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, "
        "bfloat16_t, uint64_t, int64_t.");

    int32_t dstBlkStride = repeatParams.dstBlkStride ? repeatParams.dstBlkStride : 1;
    if constexpr (sizeof(T) == 8) {
        VF_CALL<BrcbB64Impl<T>>(dst, src, repeatTime, repeatParams, dstBlkStride);
    } else if constexpr (sizeof(T) == 1) {
        VF_CALL<BrcbB8Impl<T>>(dst, src, repeatTime, repeatParams, dstBlkStride);
    } else {
        VF_CALL<BrcbCommonImpl<T>>(dst, src, repeatTime, repeatParams, dstBlkStride);
    }
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_BRCB_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BRCB_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BRCB_IMPL_H__
#endif
