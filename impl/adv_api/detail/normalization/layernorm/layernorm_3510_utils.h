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
 * \file layernorm_3510_utils.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/normalization/layernorm/layernorm_3510_utils.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/normalization/layernorm.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_LAYERNORM_LAYERNORM_C310_UTILS_H__
#endif
#ifndef IMPL_NORMALIZATION_LAYERNORM_C310_UTILS_H
#define IMPL_NORMALIZATION_LAYERNORM_C310_UTILS_H

#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../basic_api/kernel_pop_stack_buffer.h"
#include "kernel_tiling/kernel_tiling.h"

namespace AscendC {
namespace Internal {
const uint16_t kLayernormFoldNum = 2;
constexpr uint32_t LAYERNORM_B16_VF_LEN = GetVecLen() / sizeof(uint16_t);
constexpr uint32_t LAYERNORM_B32_VF_LEN = GetVecLen() / sizeof(uint32_t);
} // namespace Internal

template <typename T>
__simd_callee__ inline void LoadDataWithT(
    __ubuf__ T* src0, __ubuf__ T* src1, Reg::RegTensor<float>& dstReg0, Reg::RegTensor<float>& dstReg1,
    Reg::MaskReg& dst0Preg, Reg::MaskReg& dst1Preg, uint32_t src0Offset, uint32_t src1Offset)
{
    if constexpr (IsSameType<T, half>::value || IsSameType<T, bfloat16_t>::value) {
        Reg::RegTensor<T> src0Origin;
        Reg::RegTensor<T> src1Origin;
        Reg::LoadAlign<T, Reg::LoadDist::DIST_UNPACK_B16>(src0Origin, src0 + src0Offset);
        Reg::LoadAlign<T, Reg::LoadDist::DIST_UNPACK_B16>(src1Origin, src1 + src1Offset);
        Cast<float, T, layoutZMrgZ>(dstReg0, src0Origin, dst0Preg);
        Cast<float, T, layoutZMrgZ>(dstReg1, src1Origin, dst1Preg);
    } else { // this branch: only support float
        Reg::LoadAlign(dstReg0, src0 + src0Offset);
        Reg::LoadAlign(dstReg1, src1 + src1Offset);
    }
}

template <typename T>
__simd_callee__ inline void LoadDataWithT(
    __ubuf__ T* src, Reg::RegTensor<float>& dstReg, Reg::MaskReg& dstPreg, uint32_t srcOffset)
{
    if constexpr (IsSameType<T, half>::value || IsSameType<T, bfloat16_t>::value) {
        Reg::RegTensor<T> srcOrigin;
        Reg::LoadAlign<T, Reg::LoadDist::DIST_UNPACK_B16>(srcOrigin, src + srcOffset);
        Cast<float, T, layoutZMrgZ>(dstReg, srcOrigin, dstPreg);
    } else { // this branch: only support float
        Reg::LoadAlign(dstReg, src + srcOffset);
    }
}

__aicore__ inline uint16_t CalculateHalfAddRepeatTimes(uint32_t halfAddTimes)
{
    constexpr uint16_t kMaxOffset = 16;
    uint16_t fold = 0;
    for (uint16_t i = 1; i < kMaxOffset; i++) {
        if ((halfAddTimes >> i) == 0) {
            break;
        }
        fold++;
    }
    return fold;
}

// Helper function for the first loop in ComputeMeanUseY
template <typename T>
__simd_callee__ inline void ComputeMeanLoop1(
    __ubuf__ T* const srcUb, __ubuf__ T* const workUbYOrigin, Reg::MaskReg& pregFull, Reg::MaskReg& pregOne,
    const uint32_t aLength, const uint32_t rLengthWithPadding, const uint32_t rHeadLength, const uint32_t m,
    const uint16_t repeatTimes1, const float k2Rec, const uint16_t sregLower, Reg::RegTensor<float>& src0Reg0,
    Reg::RegTensor<float>& src1Reg0, Reg::RegTensor<float>& src0Reg1, Reg::RegTensor<float>& src1Reg1,
    Reg::RegTensor<float>& dstReg0, Reg::RegTensor<float>& dstReg1, Reg::RegTensor<float>& dstReg)
{
    for (uint16_t j = 0; j < static_cast<uint16_t>(aLength); j++) {
        uint32_t mTmp = m;
        __ubuf__ float* workUbOrigin = (__ubuf__ float*)(workUbYOrigin + j * rLengthWithPadding);
        // tail block add to main block
        for (uint16_t i = 0; i < repeatTimes1; i++) {
            Reg::MaskReg preg = Reg::UpdateMask<float>(mTmp);
            LoadDataWithT<T>(
                srcUb, srcUb, src0Reg0, src1Reg0, pregFull, preg, j * rLengthWithPadding + (2 * i) * sregLower,
                j * rLengthWithPadding + rHeadLength + (2 * i) * sregLower);
            Muls(dstReg0, src1Reg0, k2Rec, preg);
            Axpy(dstReg0, src0Reg0, k2Rec, pregFull);
            preg = Reg::UpdateMask<float>(mTmp);
            LoadDataWithT<T>(
                srcUb, srcUb, src0Reg1, src1Reg1, pregFull, preg, j * rLengthWithPadding + (2 * i + 1) * sregLower,
                j * rLengthWithPadding + rHeadLength + (2 * i + 1) * sregLower);
            Muls(dstReg1, src1Reg1, k2Rec, preg);
            Axpy(dstReg1, src0Reg1, k2Rec, pregFull);
            Add(dstReg, dstReg0, dstReg1, pregFull);
            Reg::Reduce<ReduceType::SUM>(dstReg, dstReg, pregFull);
            Reg::StoreAlign<float, Reg::StoreDist::DIST_FIRST_ELEMENT_B32>((workUbOrigin + i), dstReg, pregOne);
        }
    }
}

// Helper function for the second loop in ComputeMeanUseY
template <typename T>
__simd_callee__ inline void ComputeMeanLoop2(
    __ubuf__ T* const srcUb, __ubuf__ T* const workUbYOrigin, Reg::MaskReg& pregFull, Reg::MaskReg& pregOne,
    Reg::MaskReg& preg2, const uint32_t aLength, const uint32_t rLengthWithPadding, const uint32_t rHeadLength,
    const uint16_t repeatTimes1, const uint16_t repeatTimes2, const float k2Rec, const uint16_t sregLower,
    Reg::RegTensor<float>& src0Reg0, Reg::RegTensor<float>& src1Reg0, Reg::RegTensor<float>& src0Reg1,
    Reg::RegTensor<float>& dstReg0, Reg::RegTensor<float>& dstReg1, Reg::RegTensor<float>& dstReg)
{
    for (uint16_t j = 0; j < static_cast<uint16_t>(aLength); j++) {
        __ubuf__ float* workUbOrigin = (__ubuf__ float*)(workUbYOrigin + j * rLengthWithPadding);
        for (uint16_t i = 0; i < repeatTimes2; i++) {
            LoadDataWithT<T>(
                srcUb, srcUb, src0Reg0, src1Reg0, pregFull, preg2,
                j * rLengthWithPadding + repeatTimes1 * 2 * sregLower,
                j * rLengthWithPadding + rHeadLength + repeatTimes1 * 2 * sregLower);
            Muls(dstReg0, src1Reg0, k2Rec, preg2);
            Axpy(dstReg0, src0Reg0, k2Rec, pregFull);

            LoadDataWithT<T>(
                srcUb, src0Reg1, pregFull, j * rLengthWithPadding + repeatTimes1 * 2 * sregLower + sregLower);
            Muls(dstReg1, src0Reg1, k2Rec, pregFull);

            Add(dstReg, dstReg0, dstReg1, pregFull);
            Reg::Reduce<ReduceType::SUM>(dstReg, dstReg, pregFull);
            Reg::StoreAlign<float, Reg::StoreDist::DIST_FIRST_ELEMENT_B32>(
                (workUbOrigin + repeatTimes1 + i), dstReg, pregOne);
        }
    }
}

// Helper function for the third loop in ComputeMeanUseY
template <typename T>
__simd_callee__ inline void ComputeMeanLoop3(
    __ubuf__ T* const srcUb, __ubuf__ T* const workUbYOrigin, Reg::MaskReg& pregFull, Reg::MaskReg& pregOne,
    const uint32_t aLength, const uint32_t rLengthWithPadding, const uint16_t repeatTimes1, const uint16_t repeatTimes2,
    const uint16_t repeatTimes3, const uint32_t mVL, const float k2Rec, const uint16_t sregLower,
    Reg::RegTensor<float>& src0Reg0, Reg::RegTensor<float>& src0Reg1, Reg::RegTensor<float>& dstReg0,
    Reg::RegTensor<float>& dstReg1, Reg::RegTensor<float>& dstReg)
{
    for (uint16_t j = 0; j < static_cast<uint16_t>(aLength); j++) {
        __ubuf__ float* workUbOrigin = (__ubuf__ float*)(workUbYOrigin + j * rLengthWithPadding);
        // Processes the remaining data of the entire block.
        for (uint16_t i = 0; i < repeatTimes3; i++) {
            LoadDataWithT<T>(srcUb, src0Reg0, pregFull, j * rLengthWithPadding + mVL + 2 * i * sregLower);
            LoadDataWithT<T>(srcUb, src0Reg1, pregFull, j * rLengthWithPadding + mVL + (2 * i + 1) * sregLower);
            Muls(dstReg0, src0Reg0, k2Rec, pregFull);
            Muls(dstReg1, src0Reg1, k2Rec, pregFull);
            Add(dstReg, dstReg0, dstReg1, pregFull);
            Reg::Reduce<ReduceType::SUM>(dstReg, dstReg, pregFull);
            Reg::StoreAlign<float, Reg::StoreDist::DIST_FIRST_ELEMENT_B32>(
                (workUbOrigin + repeatTimes1 + repeatTimes2 + i), dstReg, pregOne);
        }
    }
}

template <typename T>
__simd_callee__ inline void ComputeMeanUseY(
    __ubuf__ T* const srcUb, __ubuf__ T* const workUbYOrigin, Reg::MaskReg& pregFull, Reg::MaskReg& pregOne,
    Reg::MaskReg& pregLastCount, Reg::MaskReg& preg2, const uint32_t aLength, const uint32_t rLengthWithPadding,
    const uint32_t rHeadLength, const uint32_t m, const uint16_t repeatTimes1, const uint16_t repeatTimes2,
    const uint16_t repeatTimes3, const uint32_t mVL, const float k2Rec, const uint16_t sregLower)
{
    Reg::RegTensor<float> src0Reg0;
    Reg::RegTensor<float> src1Reg0;
    Reg::RegTensor<float> src0Reg1;
    Reg::RegTensor<float> src1Reg1;
    Reg::RegTensor<float> dstReg0;
    Reg::RegTensor<float> dstReg1;
    Reg::RegTensor<float> dstReg;

    ComputeMeanLoop1<T>(
        srcUb, workUbYOrigin, pregFull, pregOne, aLength, rLengthWithPadding, rHeadLength, m, repeatTimes1, k2Rec,
        sregLower, src0Reg0, src1Reg0, src0Reg1, src1Reg1, dstReg0, dstReg1, dstReg);
    ComputeMeanLoop2<T>(
        srcUb, workUbYOrigin, pregFull, pregOne, preg2, aLength, rLengthWithPadding, rHeadLength, repeatTimes1,
        repeatTimes2, k2Rec, sregLower, src0Reg0, src1Reg0, src0Reg1, dstReg0, dstReg1, dstReg);
    ComputeMeanLoop3<T>(
        srcUb, workUbYOrigin, pregFull, pregOne, aLength, rLengthWithPadding, repeatTimes1, repeatTimes2, repeatTimes3,
        mVL, k2Rec, sregLower, src0Reg0, src0Reg1, dstReg0, dstReg1, dstReg);
}

// Helper function for the first loop in ComputeVarianceUseY
template <typename T>
__simd_callee__ inline void ComputeVarianceLoop1(
    __ubuf__ T* const srcUb, __ubuf__ T* const workUbYOrigin, __ubuf__ float* const meanUb, Reg::MaskReg& pregFull,
    Reg::MaskReg& pregOne, const uint32_t aLength, const uint32_t rLengthWithPadding, const uint32_t rHeadLength,
    const uint32_t m, const uint16_t repeatTimes1, const float k2Rec, const uint16_t sregLower,
    Reg::RegTensor<float>& meanReg, Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& src0Reg0,
    Reg::RegTensor<float>& src1Reg0, Reg::RegTensor<float>& src0Reg1, Reg::RegTensor<float>& src1Reg1,
    Reg::RegTensor<float>& dstReg0, Reg::RegTensor<float>& dstReg1)
{
    for (uint16_t j = 0; j < static_cast<uint16_t>(aLength); j++) {
        uint32_t mTmp = m;
        __ubuf__ float* workUbOrigin = (__ubuf__ float*)(workUbYOrigin + j * rLengthWithPadding);
        Reg::LoadAlign<float, Reg::LoadDist::DIST_BRC_B32>(meanReg, meanUb + j);
        // tail block add to main block
        for (uint16_t i = 0; i < repeatTimes1; i++) {
            Reg::MaskReg preg = Reg::UpdateMask<float>(mTmp);
            LoadDataWithT<T>(
                srcUb, srcUb, src0Reg0, src1Reg0, pregFull, preg, j * rLengthWithPadding + (2 * i) * sregLower,
                j * rLengthWithPadding + rHeadLength + (2 * i) * sregLower);

            Sub(src0Reg0, src0Reg0, meanReg, pregFull);
            Sub(src1Reg0, src1Reg0, meanReg, preg);

            Mul(src0Reg0, src0Reg0, src0Reg0, pregFull);
            Mul(src1Reg0, src1Reg0, src1Reg0, pregFull);
            Muls(dstReg0, src1Reg0, k2Rec, pregFull);
            Axpy(dstReg0, src0Reg0, k2Rec, pregFull);

            preg = Reg::UpdateMask<float>(mTmp);
            LoadDataWithT<T>(
                srcUb, srcUb, src0Reg1, src1Reg1, pregFull, preg, j * rLengthWithPadding + (2 * i + 1) * sregLower,
                j * rLengthWithPadding + rHeadLength + (2 * i + 1) * sregLower);

            Sub(src0Reg1, src0Reg1, meanReg, pregFull);
            Sub(src1Reg1, src1Reg1, meanReg, preg);

            Mul(src0Reg1, src0Reg1, src0Reg1, pregFull);
            Mul(src1Reg1, src1Reg1, src1Reg1, pregFull);
            Muls(dstReg1, src1Reg1, k2Rec, pregFull);
            Axpy(dstReg1, src0Reg1, k2Rec, pregFull);

            Add(dstReg, dstReg0, dstReg1, pregFull);
            Reg::Reduce<ReduceType::SUM>(dstReg, dstReg, pregFull);
            Reg::StoreAlign<float, Reg::StoreDist::DIST_FIRST_ELEMENT_B32>((workUbOrigin + i), dstReg, pregOne);
        }
    }
}

// Helper function for the second loop in ComputeVarianceUseY
template <typename T>
__simd_callee__ inline void ComputeVarianceLoop2(
    __ubuf__ T* const srcUb, __ubuf__ T* const workUbYOrigin, __ubuf__ float* const meanUb, Reg::MaskReg& pregFull,
    Reg::MaskReg& pregOne, Reg::MaskReg& preg2, const uint32_t aLength, const uint32_t rLengthWithPadding,
    const uint32_t rHeadLength, const uint16_t repeatTimes1, const uint16_t repeatTimes2, const float k2Rec,
    const uint16_t sregLower, Reg::RegTensor<float>& meanReg, Reg::RegTensor<float>& src0Reg0,
    Reg::RegTensor<float>& src1Reg0, Reg::RegTensor<float>& src0Reg1, Reg::RegTensor<float>& dstReg0,
    Reg::RegTensor<float>& dstReg1, Reg::RegTensor<float>& dstReg)
{
    for (uint16_t j = 0; j < static_cast<uint16_t>(aLength); j++) {
        __ubuf__ float* workUbOrigin = (__ubuf__ float*)(workUbYOrigin + j * rLengthWithPadding);
        Reg::LoadAlign<float, Reg::LoadDist::DIST_BRC_B32>(meanReg, meanUb + j);
        for (uint16_t i = 0; i < repeatTimes2; i++) {
            LoadDataWithT<T>(
                srcUb, srcUb, src0Reg0, src1Reg0, pregFull, preg2,
                j * rLengthWithPadding + repeatTimes1 * 2 * sregLower,
                j * rLengthWithPadding + rHeadLength + repeatTimes1 * 2 * sregLower);

            Sub(src0Reg0, src0Reg0, meanReg, pregFull);
            Sub(src1Reg0, src1Reg0, meanReg, pregFull);

            Mul(src0Reg0, src0Reg0, src0Reg0, pregFull);
            Mul(src1Reg0, src1Reg0, src1Reg0, pregFull);
            Muls(dstReg0, src1Reg0, k2Rec, preg2);
            Axpy(dstReg0, src0Reg0, k2Rec, pregFull);

            LoadDataWithT<T>(
                srcUb, src0Reg1, pregFull, j * rLengthWithPadding + repeatTimes1 * 2 * sregLower + sregLower);

            Sub(src0Reg1, src0Reg1, meanReg, pregFull);

            Mul(src0Reg1, src0Reg1, src0Reg1, pregFull);

            Muls(dstReg1, src0Reg1, k2Rec, pregFull);

            Add(dstReg, dstReg0, dstReg1, pregFull);
            Reg::Reduce<ReduceType::SUM>(dstReg, dstReg, pregFull);
            Reg::StoreAlign<float, Reg::StoreDist::DIST_FIRST_ELEMENT_B32>(
                (workUbOrigin + repeatTimes1 + i), dstReg, pregOne);
        }
    }
}

// Helper function for the third loop in ComputeVarianceUseY
template <typename T>
__simd_callee__ inline void ComputeVarianceLoop3(
    __ubuf__ T* const srcUb, __ubuf__ T* const workUbYOrigin, __ubuf__ float* const meanUb, Reg::MaskReg& pregFull,
    Reg::MaskReg& pregOne, const uint32_t aLength, const uint32_t rLengthWithPadding, const uint16_t repeatTimes1,
    const uint16_t repeatTimes2, const uint16_t repeatTimes3, const uint32_t mVL, const float k2Rec,
    const uint16_t sregLower, Reg::RegTensor<float>& meanReg, Reg::RegTensor<float>& src0Reg0,
    Reg::RegTensor<float>& src0Reg1, Reg::RegTensor<float>& dstReg0, Reg::RegTensor<float>& dstReg1,
    Reg::RegTensor<float>& dstReg)
{
    for (uint16_t j = 0; j < static_cast<uint16_t>(aLength); j++) {
        __ubuf__ float* workUbOrigin = (__ubuf__ float*)(workUbYOrigin + j * rLengthWithPadding);
        Reg::LoadAlign<float, Reg::LoadDist::DIST_BRC_B32>(meanReg, meanUb + j);
        // Processes the remaining data of the entire block.
        for (uint16_t i = 0; i < repeatTimes3; i++) {
            LoadDataWithT<T>(srcUb, src0Reg0, pregFull, j * rLengthWithPadding + mVL + (2 * i) * sregLower);
            LoadDataWithT<T>(srcUb, src0Reg1, pregFull, j * rLengthWithPadding + mVL + (2 * i + 1) * sregLower);

            Sub(src0Reg0, src0Reg0, meanReg, pregFull);
            Mul(src0Reg0, src0Reg0, src0Reg0, pregFull);
            Muls(dstReg0, src0Reg0, k2Rec, pregFull);
            Sub(src0Reg1, src0Reg1, meanReg, pregFull);
            Mul(src0Reg1, src0Reg1, src0Reg1, pregFull);
            Muls(dstReg1, src0Reg1, k2Rec, pregFull);

            Add(dstReg, dstReg0, dstReg1, pregFull);

            Reg::Reduce<ReduceType::SUM>(dstReg, dstReg, pregFull);
            Reg::StoreAlign<float, Reg::StoreDist::DIST_FIRST_ELEMENT_B32>(
                (workUbOrigin + repeatTimes1 + repeatTimes2 + i), dstReg, pregOne);
        }
    }
}

template <typename T>
__simd_callee__ inline void ComputeVarianceUseY(
    __ubuf__ T* const srcUb, __ubuf__ T* const workUbYOrigin, __ubuf__ float* const meanUb, Reg::MaskReg& pregFull,
    Reg::MaskReg& pregOne, Reg::MaskReg& pregLastCount, Reg::MaskReg& preg2, const uint32_t aLength,
    const uint32_t rLengthWithPadding, const uint32_t rHeadLength, const uint32_t m, const uint16_t repeatTimes1,
    const uint16_t repeatTimes2, const uint16_t repeatTimes3, const uint32_t mVL, const float k2Rec,
    const uint16_t sregLower)
{
    Reg::RegTensor<float> meanReg;
    Reg::RegTensor<float> dstReg;
    Reg::RegTensor<float> src0Reg0;
    Reg::RegTensor<float> src1Reg0;
    Reg::RegTensor<float> src0Reg1;
    Reg::RegTensor<float> src1Reg1;
    Reg::RegTensor<float> dstReg0;
    Reg::RegTensor<float> dstReg1;

    ComputeVarianceLoop1<T>(
        srcUb, workUbYOrigin, meanUb, pregFull, pregOne, aLength, rLengthWithPadding, rHeadLength, m, repeatTimes1,
        k2Rec, sregLower, meanReg, dstReg, src0Reg0, src1Reg0, src0Reg1, src1Reg1, dstReg0, dstReg1);
    ComputeVarianceLoop2<T>(
        srcUb, workUbYOrigin, meanUb, pregFull, pregOne, preg2, aLength, rLengthWithPadding, rHeadLength, repeatTimes1,
        repeatTimes2, k2Rec, sregLower, meanReg, src0Reg0, src1Reg0, src0Reg1, dstReg0, dstReg1, dstReg);
    ComputeVarianceLoop3<T>(
        srcUb, workUbYOrigin, meanUb, pregFull, pregOne, aLength, rLengthWithPadding, repeatTimes1, repeatTimes2,
        repeatTimes3, mVL, k2Rec, sregLower, meanReg, src0Reg0, src0Reg1, dstReg0, dstReg1, dstReg);
}

// Helper: reduce temporary work buffer into a scalar per row and store to dstUb
template <typename T, uint16_t HalfAddTimes>
__simd_callee__ inline void ReduceWorkBufferAndStore(
    __ubuf__ T* const workUbYOrigin, __ubuf__ float* const dstUb, Reg::MaskReg& pregFull, Reg::MaskReg& pregOne,
    Reg::MaskReg& pregLastCount, const uint32_t aLength, const uint32_t rLengthWithPadding,
    const uint16_t halfAddRepeatTimes, const uint32_t lastCount, const float k2RRec, const uint16_t sregLower,
    const uint16_t dynamicHalfAddTimes = 0)
{
    Reg::RegTensor<float> v0, v1, v2, v3;
    Reg::RegTensor<float> s0, s1;
    Reg::RegTensor<float> tmp, outReg;

    for (uint16_t j = 0; j < static_cast<uint16_t>(aLength); ++j) {
        __ubuf__ float* workUb = (__ubuf__ float*)(workUbYOrigin + j * rLengthWithPadding);

        if constexpr (HalfAddTimes == 1) {
            Reg::LoadAlign(v0, workUb);
            Reg::Reduce<ReduceType::SUM>(tmp, v0, pregLastCount);
        } else if constexpr (HalfAddTimes == 2) {
            Reg::LoadAlign(v0, workUb);
            Reg::LoadAlign(v1, workUb + sregLower);
            Add(v0, v0, v1, pregFull);
            Reg::Reduce<ReduceType::SUM>(tmp, v0, pregLastCount);
        } else if constexpr (HalfAddTimes == 4) {
            Reg::LoadAlign(v0, workUb);
            Reg::LoadAlign(v1, workUb + sregLower * 1);
            Reg::LoadAlign(v2, workUb + sregLower * 2);
            Reg::LoadAlign(v3, workUb + sregLower * 3);
            Add(v0, v0, v2, pregFull);
            Add(v1, v1, v3, pregFull);
            Add(v0, v0, v1, pregFull);
            Reg::Reduce<ReduceType::SUM>(tmp, v0, pregLastCount);
        } else { // general case
            uint16_t cur = dynamicHalfAddTimes;
            for (uint16_t k = 0; k < halfAddRepeatTimes; ++k) {
                cur = cur / Internal::kLayernormFoldNum; // Fold
                for (uint16_t i = 0; i < cur; ++i) {
                    Reg::LoadAlign(s0, workUb + i * sregLower);
                    Reg::LoadAlign(s1, workUb + (cur + i) * sregLower);
                    Add(tmp, s0, s1, pregFull);
                    Reg::StoreAlign(workUb + i * sregLower, tmp, pregFull);
                }
                Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
            }
            Reg::LoadAlign(s0, workUb);
            Reg::Reduce<ReduceType::SUM>(tmp, s0, pregLastCount);
        }

        Muls(outReg, tmp, k2RRec, pregOne);
        Reg::StoreAlign<float, Reg::StoreDist::DIST_FIRST_ELEMENT_B32>((dstUb + j), outReg, pregOne);
    }
}

} // namespace AscendC
#endif // IMPL_NORMALIZATION_LAYERNORM_C310_UTILS_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_LAYERNORM_LAYERNORM_C310_UTILS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_LAYERNORM_LAYERNORM_C310_UTILS_H__
#endif
