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
 * \file fmod_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/fmod/fmod_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/fmod.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_FMOD_FMOD_C310_IMPL_H__
#endif
#ifndef IMPL_MATH_FMOD_FMOD_C310_IMPL_H
#define IMPL_MATH_FMOD_FMOD_C310_IMPL_H
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../common/check.h"

namespace AscendC {
namespace FmodInternal {
union FloatS32Union {
    constexpr __aicore__ FloatS32Union() : f(0.0f) {}
    constexpr __aicore__ FloatS32Union(int32_t val) : i(val) {}
    float f;
    int32_t i;
};
union FloatU32Union {
    constexpr __aicore__ FloatU32Union() : f(0.0f) {}
    constexpr __aicore__ FloatU32Union(uint32_t val) : i(val) {}
    float f;
    uint32_t i;
};
constexpr FloatU32Union inf(F32_INF);
constexpr FloatU32Union negInf(F32_NEG_INF);
constexpr FloatU32Union nan(F32_NAN);
constexpr uint16_t oneRepSize = static_cast<uint16_t>(GetVecLen() / sizeof(float));

constexpr Reg::CastTrait castTraitS322F32 = {
    Reg::RegLayout::UNKNOWN, Reg::SatMode::UNKNOWN, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};

constexpr FloatS32Union scaleList1[FMOD_ITERATION_NUM_MAX] = {
    FloatS32Union(0x4b800000), FloatS32Union(0x4b800000), FloatS32Union(0x57800000), FloatS32Union(0x63800000),
    FloatS32Union(0x6f800000), FloatS32Union(0x7b800000), FloatS32Union(0x7b800000), FloatS32Union(0x7b800000),
    FloatS32Union(0x7b800000), FloatS32Union(0x7b800000), FloatS32Union(0x7b800000)};
constexpr FloatS32Union scaleList2[FMOD_ITERATION_NUM_MAX] = {
    FloatS32Union(0x3f800000), FloatS32Union(0x3f800000), FloatS32Union(0x3f800000), FloatS32Union(0x3f800000),
    FloatS32Union(0x3f800000), FloatS32Union(0x3f800000), FloatS32Union(0x4b800000), FloatS32Union(0x57800000),
    FloatS32Union(0x63800000), FloatS32Union(0x6f800000), FloatS32Union(0x7b800000)};

template <typename T>
__simd_callee__ inline void LoadDataWithT(
    __ubuf__ T* src, Reg::RegTensor<float>& dstReg, Reg::MaskReg& mask, uint32_t offset)
{
    if constexpr (IsSameType<T, half>::value) {
        Reg::RegTensor<T> srcOrigin;
        Reg::LoadAlign<T, Reg::LoadDist::DIST_UNPACK_B16>(srcOrigin, src + offset);
        Cast<float, T, layoutZMrgZ>(dstReg, srcOrigin, mask);
    } else {
        Reg::LoadAlign(dstReg, src + offset);
    }
}

template <typename T>
__simd_callee__ inline void SaveDataWithT(
    __ubuf__ T* dst, Reg::RegTensor<float>& srcReg, Reg::MaskReg& mask, uint32_t offset)
{
    if constexpr (IsSameType<T, half>::value) {
        Reg::RegTensor<T> regT;
        Reg::Cast<T, float, LayoutZMrgZRndRSatNS>(regT, srcReg, mask);
        Reg::StoreAlign<T, Reg::StoreDist::DIST_PACK_B32>(dst + offset, regT, mask);
    } else {
        Reg::StoreAlign(dst + offset, srcReg, mask);
    }
}

__simd_callee__ inline void GetSignBit(Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg, Reg::MaskReg& mask)
{
    constexpr int16_t signRightNum = 31;
    Reg::RegTensor<uint32_t> oneReg;
    Reg::RegTensor<uint32_t> tmpReg;
    Reg::Duplicate(oneReg, 1, mask);
    Reg::ShiftRights(tmpReg, (Reg::RegTensor<uint32_t>&)srcReg, signRightNum, mask);
    Reg::And(tmpReg, tmpReg, oneReg, mask);
    Reg::Cast<float, int32_t, FmodInternal::castTraitS322F32>(dstReg, (Reg::RegTensor<int32_t>&)tmpReg, mask);
}

template <int32_t iterationNum>
__simd_callee__ inline void SolveScale(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& src1Reg, const float scale1, const float scale2,
    Reg::MaskReg& mask)
{
    constexpr float maxValue = 3.4028235e38;
    constexpr float subnormal = 1.1754944e-38;

    Reg::MaskReg subnormalMask;
    Reg::RegTensor<float> bTmpReg;
    Reg::RegTensor<float> tmpReg;
    Reg::RegTensor<float> kReg;
    Reg::RegTensor<float> signReg;

    if constexpr (iterationNum == 1) { // iter 1 (last iteration) handles subnormal case
        Reg::CompareScalar<float, CMPMODE::LE>(subnormalMask, src1Reg, subnormal, mask);
        Reg::Muls(tmpReg, src1Reg, scale1, subnormalMask);
        Reg::Select(bTmpReg, tmpReg, src1Reg, subnormalMask);
        Reg::Muls(tmpReg, dstReg, scale1, subnormalMask);
        Reg::Select(dstReg, tmpReg, dstReg, subnormalMask);

        Reg::Div(kReg, dstReg, bTmpReg, mask);
        Reg::Truncate<float, RoundMode::CAST_RINT>(kReg, kReg, mask);
    } else {
        Reg::Muls(bTmpReg, src1Reg, scale1, mask);
        if constexpr (iterationNum > 5) { // last 5 iterations do not need extra scaling
            Reg::Muls(bTmpReg, bTmpReg, scale2, mask);
        }
        Reg::Div(kReg, dstReg, bTmpReg, mask);
        Reg::Truncate<float, RoundMode::CAST_ROUND>(kReg, kReg, mask);
    }

    // not necessary to check for inf in the final iteration
    if constexpr (iterationNum != 1) {
        Reg::Mins(bTmpReg, bTmpReg, maxValue, mask);
    }
    Reg::Neg(kReg, kReg, mask);
    // res = -k * bTmp + y
    Reg::MulAddDst(dstReg, kReg, bTmpReg, mask);

    if constexpr (iterationNum == 1) { // iter 1 handles subnormal case
        // r = r + np.float32(np.signbit(r)) * btmp
        GetSignBit(signReg, dstReg, mask);
        Reg::Mul(signReg, signReg, bTmpReg, mask);
        Reg::Add(dstReg, dstReg, signReg, mask);
    }
}

// recurse from itermax to 1
template <int32_t iterationNum>
__simd_callee__ inline void SolveScaleIter(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& src1Reg, Reg::MaskReg& mask)
{
    SolveScale<iterationNum>(dstReg, src1Reg, scaleList1[iterationNum - 1].f, scaleList2[iterationNum - 1].f, mask);

    if constexpr (iterationNum > 1) {
        SolveScaleIter<iterationNum - 1>(dstReg, src1Reg, mask);
    }
}

template <int32_t iterationNum>
__simd_callee__ inline void SolveScale(
    __ubuf__ float* dst, __ubuf__ float* src, const uint16_t unitRepTimes, const float scale1, const float scale2,
    Reg::MaskReg& mask)
{
    Reg::RegTensor<float> src1OriginReg;
    Reg::RegTensor<float> src1Reg;
    Reg::RegTensor<float> dstReg;
    for (uint16_t i = 0; i < unitRepTimes; i++) {
        Reg::LoadAlign(src1OriginReg, src + i * FmodInternal::oneRepSize);
        Reg::LoadAlign(dstReg, dst + i * FmodInternal::oneRepSize);
        Reg::Abs(src1Reg, src1OriginReg, mask);
        SolveScale<iterationNum>(dstReg, src1Reg, scale1, scale2, mask);
        Reg::StoreAlign(dst + i * FmodInternal::oneRepSize, dstReg, mask);
    }
}

template <int32_t iterationNum>
__simd_callee__ inline void SolveScaleInit(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, const uint16_t unitRepTimes, const float scale1,
    const float scale2, Reg::MaskReg& mask)
{
    Reg::RegTensor<float> src0OriginReg;
    Reg::RegTensor<float> src1OriginReg;
    Reg::RegTensor<float> src1Reg;
    Reg::RegTensor<float> dstReg;
    for (uint16_t i = 0; i < unitRepTimes; i++) {
        Reg::LoadAlign(src0OriginReg, src0 + i * FmodInternal::oneRepSize);
        Reg::LoadAlign(src1OriginReg, src1 + i * FmodInternal::oneRepSize);
        Reg::Abs(dstReg, src0OriginReg, mask);
        Reg::Abs(src1Reg, src1OriginReg, mask);
        SolveScale<iterationNum>(dstReg, src1Reg, scale1, scale2, mask);
        Reg::StoreAlign(dst + i * FmodInternal::oneRepSize, dstReg, mask);
    }
}

template <int32_t iterationNum, int32_t totalIterationNum>
__simd_callee__ inline void SolveScaleIterImpl(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, const uint16_t unitRepTimes, Reg::MaskReg& mask)
{
    if (iterationNum == totalIterationNum) { // first iteration, initialization
        SolveScaleInit<iterationNum>(
            dst, src0, src1, unitRepTimes, scaleList1[iterationNum - 1].f, scaleList2[iterationNum - 1].f, mask);
    } else {
        SolveScale<iterationNum>(
            dst, src1, unitRepTimes, scaleList1[iterationNum - 1].f, scaleList2[iterationNum - 1].f, mask);
    }

    if constexpr (iterationNum > 1) {
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
        SolveScaleIterImpl<iterationNum - 1, totalIterationNum>(dst, src0, src1, unitRepTimes, mask);
    }
}

template <int32_t iterationNum>
__simd_callee__ inline void SolveScaleIter(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, const uint16_t unitRepTimes, Reg::MaskReg& mask)
{
    SolveScaleIterImpl<iterationNum, iterationNum>(dst, src0, src1, unitRepTimes, mask);
}

__simd_callee__ inline void SolveExceptionScenarios(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& src0Reg, Reg::RegTensor<float>& src1Reg,
    Reg::RegTensor<float> nanReg, Reg::MaskReg& mask)
{
    Reg::MaskReg src0Is0CmpReg;
    Reg::MaskReg src0IsNeg0CmpReg;
    Reg::MaskReg src0InfCmpReg;
    Reg::MaskReg src0NegInfCmpReg;
    Reg::MaskReg src1InfCmpReg;
    Reg::MaskReg src1NegInfCmpReg;
    Reg::MaskReg srcBothInfCmpReg;
    Reg::MaskReg src1Not0CmpReg;
    Reg::MaskReg src1NotNeg0CmpReg;
    Reg::MaskReg src1NotNanCmpReg;
    // if src1Tensor is inf return src0Reg
    Reg::CompareScalar(src1InfCmpReg, src1Reg, inf.f, mask);
    Reg::Select(dstReg, src0Reg, dstReg, src1InfCmpReg);
    // if src1Tensor is -inf return src0Reg
    Reg::CompareScalar(src1NegInfCmpReg, src1Reg, negInf.f, mask);
    Reg::Select(dstReg, src0Reg, dstReg, src1NegInfCmpReg);
    // if src0Tensor is inf
    Reg::CompareScalar(src0InfCmpReg, src0Reg, inf.f, mask);
    // if src0Tensor is -inf
    Reg::CompareScalar(src0NegInfCmpReg, src0Reg, negInf.f, mask);
    // if src0Tensor and src1Tensor both inf return inf
    Reg::MaskOr(src0InfCmpReg, src0InfCmpReg, src0NegInfCmpReg, mask);
    Reg::MaskOr(src1InfCmpReg, src1InfCmpReg, src1NegInfCmpReg, mask);
    Reg::MaskAnd(srcBothInfCmpReg, src0InfCmpReg, src1InfCmpReg, mask);
    Reg::Select(dstReg, nanReg, dstReg, srcBothInfCmpReg);
    // if src0Tensor is ±0 and src1Tensor is not ±0 and not nan, return src0Tensor
    Reg::CompareScalar(src0Is0CmpReg, src0Reg, static_cast<float>(0), mask);
    Reg::CompareScalar(src0IsNeg0CmpReg, src0Reg, static_cast<float>(-0), mask);
    Reg::MaskOr(src0Is0CmpReg, src0Is0CmpReg, src0IsNeg0CmpReg, mask);

    Reg::CompareScalar<float, CMPMODE::NE>(src1Not0CmpReg, src1Reg, static_cast<float>(0), mask);
    Reg::CompareScalar<float, CMPMODE::NE>(src1NotNeg0CmpReg, src1Reg, static_cast<float>(-0), mask);
    Reg::Compare<float, CMPMODE::NE>(src1NotNanCmpReg, src1Reg, src1Reg, mask);
    Reg::MaskNot(src1NotNanCmpReg, src1NotNanCmpReg, mask);

    Reg::MaskOr(src1Not0CmpReg, src1Not0CmpReg, src1NotNeg0CmpReg, mask);
    Reg::MaskAnd(src1Not0CmpReg, src1Not0CmpReg, src1NotNanCmpReg, mask);

    Reg::MaskAnd(src0Is0CmpReg, src0Is0CmpReg, src1Not0CmpReg, mask);
    Reg::Select(dstReg, src0Reg, dstReg, src0Is0CmpReg);
}
} // namespace FmodInternal

template <int32_t iterationNum>
__simd_vf__ inline void FmodComputeIterationF32(
    __ubuf__ float* dstTensor, __ubuf__ float* src0Tensor, __ubuf__ float* src1Tensor, const uint16_t mainRepeatTimes,
    const uint16_t mainBlockLen, const uint16_t tailRepeatTimes, uint32_t tailCount)
{
    constexpr FmodInternal::FloatU32Union scale1(0x4B800000); // 2**24
    constexpr FmodInternal::FloatU32Union scale2(0x33800000); // 2**-24
    constexpr float subnormal = 1.1754944e-38;
    Reg::RegTensor<float> src0OriginReg;
    Reg::RegTensor<float> src1OriginReg;
    Reg::RegTensor<float> src1Reg;
    Reg::RegTensor<float> dstReg;
    Reg::RegTensor<float> nanReg;
    Reg::RegTensor<float> zeroReg;
    Reg::RegTensor<float> n2Reg;
    Reg::RegTensor<float> oneReg;
    Reg::RegTensor<float> src0SignBitReg;
    Reg::RegTensor<float> src0SignBitTmpReg;
    Reg::RegTensor<float> dstSignBitReg;
    Reg::RegTensor<float> bTmpReg;
    Reg::RegTensor<float> tmpReg;
    Reg::MaskReg maskReg;
    Reg::MaskReg subnormalMask;
    Reg::MaskReg maskFull = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::Duplicate(nanReg, FmodInternal::nan.f, maskFull);
    Reg::Duplicate(zeroReg, static_cast<float>(0.0), maskFull);
    Reg::Duplicate(n2Reg, static_cast<float>(-2.0), maskFull);
    Reg::Duplicate(oneReg, static_cast<float>(1), maskFull);

    FmodInternal::SolveScaleIter<iterationNum>(dstTensor, src0Tensor, src1Tensor, mainRepeatTimes, maskFull);

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    for (uint16_t i = 0; i < mainRepeatTimes; i++) {
        FmodInternal::LoadDataWithT(src0Tensor, src0OriginReg, maskFull, i * FmodInternal::oneRepSize);
        FmodInternal::LoadDataWithT(src1Tensor, src1OriginReg, maskFull, i * FmodInternal::oneRepSize);
        Reg::Abs(src1Reg, src1OriginReg, maskFull);
        Reg::LoadAlign(dstReg, dstTensor + i * FmodInternal::oneRepSize);

        // res = res*(np.float32(signbit)*np.float32(-2.0) + np.float32(1))
        FmodInternal::GetSignBit(src0SignBitReg, src0OriginReg, maskFull);
        Reg::Mul(src0SignBitTmpReg, src0SignBitReg, n2Reg, maskFull);
        Reg::Add(src0SignBitTmpReg, src0SignBitTmpReg, oneReg, maskFull);
        Reg::Mul(dstReg, dstReg, src0SignBitTmpReg, maskFull);

        Reg::CompareScalar<float, CMPMODE::LE>(subnormalMask, src1Reg, subnormal, maskFull);
        Reg::Muls(tmpReg, src1Reg, scale1.f, subnormalMask);
        Reg::Select(bTmpReg, tmpReg, src1Reg, subnormalMask);

        Reg::Muls(tmpReg, dstReg, scale2.f, subnormalMask);
        Reg::Select(dstReg, tmpReg, dstReg, subnormalMask);

        FmodInternal::SolveExceptionScenarios(dstReg, src0OriginReg, src1OriginReg, nanReg, maskFull);
        FmodInternal::SaveDataWithT(dstTensor, dstReg, maskFull, i * FmodInternal::oneRepSize);
    }

    for (uint16_t i = 0; i < tailRepeatTimes; i++) {
        maskReg = Reg::UpdateMask<float>(tailCount);
        FmodInternal::LoadDataWithT(src0Tensor, src0OriginReg, maskReg, mainBlockLen + i * FmodInternal::oneRepSize);
        FmodInternal::LoadDataWithT(src1Tensor, src1OriginReg, maskReg, mainBlockLen + i * FmodInternal::oneRepSize);

        Reg::Abs(dstReg, src0OriginReg, maskReg);
        Reg::Abs(src1Reg, src1OriginReg, maskReg);
        FmodInternal::SolveScaleIter<iterationNum>(dstReg, src1Reg, maskReg);

        // res = res*(np.float32(signbit)*np.float32(-2.0) + np.float32(1))
        FmodInternal::GetSignBit(src0SignBitReg, src0OriginReg, maskReg);
        Reg::Mul(src0SignBitTmpReg, src0SignBitReg, n2Reg, maskReg);
        Reg::Add(src0SignBitTmpReg, src0SignBitTmpReg, oneReg, maskReg);
        Reg::Mul(dstReg, dstReg, src0SignBitTmpReg, maskReg);

        Reg::CompareScalar<float, CMPMODE::LE>(subnormalMask, src1Reg, subnormal, maskReg);
        Reg::Muls(tmpReg, src1Reg, scale1.f, subnormalMask);
        Reg::Select(bTmpReg, tmpReg, src1Reg, subnormalMask);
        Reg::Muls(tmpReg, dstReg, scale2.f, subnormalMask);
        Reg::Select(dstReg, tmpReg, dstReg, subnormalMask);

        FmodInternal::SolveExceptionScenarios(dstReg, src0OriginReg, src1OriginReg, nanReg, maskReg);
        FmodInternal::SaveDataWithT(dstTensor, dstReg, maskReg, mainBlockLen + i * FmodInternal::oneRepSize);
    }
}

template <int32_t iterationNum>
__aicore__ inline void FmodComputeIteration(
    const LocalTensor<float>& dstTensor, const LocalTensor<float>& src0Tensor, const LocalTensor<float>& src1Tensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t count)
{
    __ubuf__ float* src0 = (__ubuf__ float*)src0Tensor.GetPhyAddr();
    __ubuf__ float* src1 = (__ubuf__ float*)src1Tensor.GetPhyAddr();
    __ubuf__ float* dst = (__ubuf__ float*)dstTensor.GetPhyAddr();

    const uint16_t mainRepeatTimes = static_cast<uint16_t>(count / FmodInternal::oneRepSize);
    const uint16_t mainBlockLen = mainRepeatTimes * FmodInternal::oneRepSize;

    uint32_t tailCount = count - mainBlockLen;
    const uint16_t tailRepeatTimes = static_cast<uint16_t>(CeilDivision(tailCount, FmodInternal::oneRepSize));

    FmodComputeIterationF32<iterationNum>(dst, src0, src1, mainRepeatTimes, mainBlockLen, tailRepeatTimes, tailCount);
}

template <typename T>
__simd_vf__ inline void FmodComputeVF(
    __ubuf__ T* dstTensor, __ubuf__ T* src0Tensor, __ubuf__ T* src1Tensor, const uint16_t repeatTimes, uint32_t count)
{
    Reg::RegTensor<float> src0Reg;
    Reg::RegTensor<float> src1Reg;
    Reg::RegTensor<float> negReg;
    Reg::RegTensor<float> divReg;
    Reg::RegTensor<float> dstReg;
    Reg::RegTensor<float> nanReg;
    Reg::MaskReg maskReg;
    Reg::MaskReg maskFull = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::Duplicate(nanReg, FmodInternal::nan.f, maskFull);

    for (uint16_t i = 0; i < repeatTimes; i++) {
        maskReg = Reg::UpdateMask<float>(count);
        FmodInternal::LoadDataWithT<T>(src0Tensor, src0Reg, maskReg, i * FmodInternal::oneRepSize);
        FmodInternal::LoadDataWithT<T>(src1Tensor, src1Reg, maskReg, i * FmodInternal::oneRepSize);
        Reg::Div(divReg, src0Reg, src1Reg, maskReg);
        Reg::Truncate<float, RoundMode::CAST_TRUNC, Reg::MaskMergeMode::ZEROING>(dstReg, divReg, maskReg);
        Reg::Neg(negReg, src1Reg, maskReg);
        Reg::FusedMulDstAdd(dstReg, negReg, src0Reg, maskReg);
        FmodInternal::SolveExceptionScenarios(dstReg, src0Reg, src1Reg, nanReg, maskReg);
        FmodInternal::SaveDataWithT(dstTensor, dstReg, maskReg, i * FmodInternal::oneRepSize);
    }
}

__aicore__ inline void FmodCompute(
    const LocalTensor<float>& dstTensor, const LocalTensor<float>& src0Tensor, const LocalTensor<float>& src1Tensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t count)
{
    __ubuf__ float* src0 = (__ubuf__ float*)src0Tensor.GetPhyAddr();
    __ubuf__ float* src1 = (__ubuf__ float*)src1Tensor.GetPhyAddr();
    __ubuf__ float* dst = (__ubuf__ float*)dstTensor.GetPhyAddr();
    uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(count, FmodInternal::oneRepSize));

    FmodComputeVF<float>(dst, src0, src1, repeatTimes, count);
}

__aicore__ inline void FmodCompute(
    const LocalTensor<half>& dstTensor, const LocalTensor<half>& src0Tensor, const LocalTensor<half>& src1Tensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t count)
{
    __ubuf__ half* src0 = (__ubuf__ half*)src0Tensor.GetPhyAddr();
    __ubuf__ half* src1 = (__ubuf__ half*)src1Tensor.GetPhyAddr();
    __ubuf__ half* dst = (__ubuf__ half*)dstTensor.GetPhyAddr();
    uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(count, FmodInternal::oneRepSize));

    FmodComputeVF<half>(dst, src0, src1, repeatTimes, count);
}

template <typename T, bool isReuseSource = false, const FmodConfig& config = DEFAULT_FMOD_CONFIG>
__aicore__ inline void FmodImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
{
    if ASCEND_IS_AIC {
        return;
    }

    CheckTensorPos(dstTensor, Hardware::UB, "dstTensor", "VECIN/VECOUT/VECCALC", "Fmod");
    CheckTensorPos(src0Tensor, Hardware::UB, "src0Tensor", "VECIN/VECOUT/VECCALC", "Fmod");
    CheckTensorPos(src1Tensor, Hardware::UB, "src1Tensor", "VECIN/VECOUT/VECCALC", "Fmod");
    CheckTensorPos(sharedTmpBuffer, Hardware::UB, "sharedTmpBuffer", "VECIN/VECOUT/VECCALC", "Fmod");

    CheckCalCount(calCount, "calCount", src0Tensor, "src0Tensor", "Fmod");
    CheckCalCount(calCount, "calCount", src1Tensor, "src1Tensor", "Fmod");
    CheckCalCount(calCount, "calCount", dstTensor, "dstTensor", "Fmod");

    ASCENDC_ASSERT((src0Tensor.GetSize() == src1Tensor.GetSize()), {
        KERNEL_LOG(KERNEL_ERROR, "Input params.GetSize must be equal with each other!");
    });

    if constexpr (config.algo == FmodAlgo::ITERATION_COMPENSATION) {
        static_assert(
            config.iterationNum >= 1 && config.iterationNum <= FMOD_ITERATION_NUM_MAX,
            "Iteration number must be in the range [1, 11].");
        static_assert(SupportType<T, float>(), "current data type is not supported on current device!");
        FmodComputeIteration<config.iterationNum>(dstTensor, src0Tensor, src1Tensor, sharedTmpBuffer, calCount);
    } else {
        static_assert(SupportType<T, half, float>(), "current data type is not supported on current device!");
        FmodCompute(dstTensor, src0Tensor, src1Tensor, sharedTmpBuffer, calCount);
    }
}
} // namespace AscendC
#endif // IMPL_MATH_FMOD_FMOD_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_FMOD_FMOD_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_FMOD_FMOD_C310_IMPL_H__
#endif
