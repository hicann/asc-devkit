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
 * \file sincos_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/sincos/sincos_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/sincos.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_SINCOS_SINCOS_C310_IMPL_H__
#endif
#ifndef LIB_MATH_SINCOS_SINCOS_C310_IMPL_H
#define LIB_MATH_SINCOS_SINCOS_C310_IMPL_H
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/adv_api/math/sincos_utils.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/math/sincos/sincos_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
namespace SinCosImpl {

constexpr Reg::CastTrait castTraitF16F32 = {
    Reg::RegLayout::ZERO, Reg::SatMode::UNKNOWN, Reg::MaskMergeMode::ZEROING, RoundMode::UNKNOWN};
constexpr Reg::CastTrait castTraitF32F16 = {
    Reg::RegLayout::ZERO, Reg::SatMode::NO_SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};
constexpr Reg::CastTrait castTraitI64F32 = {
    Reg::RegLayout::ZERO, Reg::SatMode::NO_SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_ROUND};
constexpr Reg::CastTrait castTraitF32I64 = {
    Reg::RegLayout::UNKNOWN, Reg::SatMode::NO_SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_ROUND};
constexpr Reg::CastTrait castTraitI32F32 = {
    Reg::RegLayout::UNKNOWN, Reg::SatMode::NO_SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_ROUND};

template <typename T, typename U>
__simd_callee__ inline void AndScalar(Reg::RegTensor<T>& dstReg, Reg::RegTensor<U>& srcReg, T val, Reg::MaskReg& mask)
{
    Reg::RegTensor<T> tmpReg;
    Reg::Duplicate(tmpReg, val, mask);
    Reg::And(dstReg, (Reg::RegTensor<T>&)srcReg, tmpReg, mask);
}

__simd_callee__ inline void FMaf(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg, float scalarValue, Reg::MaskReg& mask)
{
    // dst = dst * src + scalarValue
    Reg::RegTensor<float> tmpReg;
    Reg::Duplicate(tmpReg, scalarValue);
    Reg::FusedMulDstAdd(dstReg, srcReg, tmpReg, mask);
}

__simd_callee__ inline void FMaf(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg1, Reg::RegTensor<float>& srcReg2,
    Reg::RegTensor<float>& srcReg3, Reg::MaskReg& mask)
{
    // dst = src1 * src2 + src3
    Reg::RegTensor<float> tmpReg = srcReg1;
    Reg::FusedMulDstAdd(tmpReg, srcReg2, srcReg3, mask);
    dstReg = tmpReg;
}

__simd_callee__ inline void FMaf(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg1, Reg::RegTensor<float>& srcReg2, float scalarValue,
    Reg::MaskReg& mask)
{
    // dst = src1 * src2 + scalarValue
    Reg::RegTensor<float> tmpReg;
    Reg::Duplicate(tmpReg, scalarValue, mask);
    FMaf(dstReg, srcReg1, srcReg2, tmpReg, mask);
}

__simd_callee__ inline void FMaf(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg1, float scalarValue, Reg::RegTensor<float>& srcReg2,
    Reg::MaskReg& mask)
{
    // dst = src1 * scalarValue + src2
    Reg::RegTensor<float> tmpReg;
    Reg::Duplicate(tmpReg, scalarValue, mask);
    FMaf(dstReg, srcReg1, tmpReg, srcReg2, mask);
}

__simd_callee__ inline void FMaf(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg1, float scalarValue, float scalarValue2,
    Reg::MaskReg& mask)
{
    // dst = src1 * scalarValue + scalarValue2
    Reg::RegTensor<float> tmpReg, tmpReg2;
    Reg::Duplicate(tmpReg, scalarValue, mask);
    Reg::Duplicate(tmpReg2, scalarValue2, mask);
    FMaf(dstReg, srcReg1, tmpReg, tmpReg2, mask);
}

__simd_callee__ inline void BitShiftCombine(
    Reg::RegTensor<uint32_t>& dstReg, Reg::RegTensor<uint32_t>& srcReg1, Reg::RegTensor<uint32_t>& srcReg2,
    Reg::RegTensor<int32_t>& srcRegE, Reg::MaskReg& mask)
{
    // dst = (src1  << e) | (src2 >> (32 - e));
    constexpr uint32_t BITSHIFTS = 32;

    Reg::RegTensor<uint32_t> tmpU32Reg1, tmpU32Reg2;
    Reg::ShiftLeft(tmpU32Reg1, srcReg1, (Reg::RegTensor<int32_t>&)srcRegE, mask);
    Reg::Duplicate(tmpU32Reg2, BITSHIFTS, mask);
    Reg::Sub(tmpU32Reg2, tmpU32Reg2, (Reg::RegTensor<uint32_t>&)srcRegE, mask);
    Reg::ShiftRight(tmpU32Reg2, srcReg2, (Reg::RegTensor<int32_t>&)tmpU32Reg2, mask);
    Reg::Or(dstReg, tmpU32Reg1, tmpU32Reg2, mask);
}

__aicore__ inline void GenerateZeroVreg(Reg::RegTensor<uint32_t>& zeroReg)
{
    Reg::MaskReg b32FullMask = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    Reg::Duplicate(zeroReg, 0, b32FullMask);
}

__simd_callee__ inline void ReinterpretedU32ToFloatAndCastToU32(
    Reg::RegTensor<uint32_t>& dstReg, Reg::RegTensor<uint32_t>& srcReg, Reg::MaskReg& mask)
{
    // dst = (unsigned int) reinterpret_cast<float &>(src);
    Reg::RegTensor<float> tmpF32Reg;
    Reg::RegTensor<int64_t, Reg::RegTraitNumTwo> tmpI64Reg;

    tmpF32Reg = (Reg::RegTensor<float>&)srcReg;
    Reg::Cast<int64_t, float, castTraitF32I64>(tmpI64Reg, tmpF32Reg, mask);
    dstReg = (Reg::RegTensor<uint32_t>&)tmpI64Reg.reg[0];
}

__simd_callee__ inline void TrigComputeP(
    Reg::RegTensor<uint32_t>& regPHigh, Reg::RegTensor<uint32_t>& regPLow, Reg::RegTensor<uint32_t>& regIa,
    Reg::RegTensor<uint32_t>& regMid, Reg::RegTensor<uint32_t>& regLo, Reg::RegTensor<uint32_t>& regHi,
    Reg::MaskReg& mask)
{
    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> tmpU64Reg;
    Reg::RegTensor<uint32_t> tmpU32Reg, zeroReg;
    Reg::MaskReg carrypMask;

    // step 12: p = (unsigned long long int)ia * lo;
    Reg::Mull((Reg::RegTensor<uint32_t>&)regPLow, (Reg::RegTensor<uint32_t>&)regPHigh, regIa, regLo, mask);

    // step 13: p = (unsigned long long int)ia * mid + (p >> 32);
    Reg::Mull(
        (Reg::RegTensor<uint32_t>&)tmpU64Reg.reg[0], (Reg::RegTensor<uint32_t>&)tmpU64Reg.reg[1], regIa, regMid, mask);

    Reg::AddCarryOut(
        carrypMask, (Reg::RegTensor<uint32_t>&)regPLow, (Reg::RegTensor<uint32_t>&)tmpU64Reg.reg[0],
        (Reg::RegTensor<uint32_t>&)regPHigh, mask);
    Reg::Duplicate(zeroReg, 0, mask);
    Reg::AddCarryOuts(
        carrypMask, (Reg::RegTensor<uint32_t>&)regPHigh, (Reg::RegTensor<uint32_t>&)tmpU64Reg.reg[1], zeroReg,
        carrypMask, mask);

    // step 14: p = ((unsigned long long int)(ia * hi) << 32) + p;
    Reg::Mul(tmpU32Reg, regIa, regHi, mask);
    Reg::AddCarryOut(
        carrypMask, (Reg::RegTensor<uint32_t>&)regPHigh, (Reg::RegTensor<uint32_t>&)regPHigh, tmpU32Reg, mask);
}

__simd_callee__ inline void TrigComputeHLQ(
    Reg::RegTensor<float>& regDh, Reg::RegTensor<float>& regDl, Reg::RegTensor<int32_t>& regQ,
    Reg::RegTensor<uint32_t>& regPHigh, Reg::RegTensor<uint32_t>& regPLow, Reg::MaskReg& mask)
{
    constexpr int16_t Q_SHIFT_BITS = 62;
    constexpr int16_t B32_BITS = 32;
    constexpr uint64_t P_AND_COEFF1 = 0x3fffffffffffffffULL;
    constexpr uint64_t P_AND_COEFF2 = 0x2000000000000000ULL;
    constexpr uint64_t P_SUBS_COEFF = 0x4000000000000000ULL;
    constexpr float P_MULS = 2.0f;
#if (defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5162))
    constexpr float B64_SHIFT_BITS = static_cast<float>(1ULL << 32);
#else
    constexpr uint64_t B64_SHIFT_BITS = 1ULL << 32;
#endif

    Reg::RegTensor<int64_t, Reg::RegTraitNumTwo> tmpI64Reg;
    Reg::RegTensor<int32_t> tmpRegQ;
    Reg::RegTensor<uint32_t> tmpU32Reg;
    Reg::MaskReg tmpMask;

    // step 15: q = (int)(p >> 62);
    Reg::ShiftRights(
        (Reg::RegTensor<uint32_t>&)regQ, (Reg::RegTensor<uint32_t>&)regPHigh, (int16_t)(Q_SHIFT_BITS - B32_BITS), mask);

    // step 16: p = p & 0x3fffffffffffffffULL;
    Reg::Duplicate(tmpU32Reg, P_AND_COEFF1 >> B32_BITS, mask);
    Reg::And((Reg::RegTensor<uint32_t>&)regPHigh, (Reg::RegTensor<uint32_t>&)regPHigh, tmpU32Reg, mask);

    /* step 17:
     * if (p & 0x2000000000000000ULL) {   // fraction >= 0.5
     *    p = p - 0x4000000000000000ULL; // fraction - 1.0
     *    q = q + 1;
     * }
     */
    Reg::Duplicate(tmpU32Reg, P_AND_COEFF2 >> B32_BITS, mask);
    Reg::And(tmpU32Reg, (Reg::RegTensor<uint32_t>&)regPHigh, tmpU32Reg, mask);
    Reg::CompareScalar<uint32_t, CMPMODE::GT>(tmpMask, tmpU32Reg, 0, mask);
    Reg::Duplicate(tmpU32Reg, P_SUBS_COEFF >> B32_BITS, mask);
    Reg::Sub(tmpU32Reg, (Reg::RegTensor<uint32_t>&)regPHigh, tmpU32Reg, mask);
    Reg::Select((Reg::RegTensor<uint32_t>&)regPHigh, tmpU32Reg, (Reg::RegTensor<uint32_t>&)regPHigh, tmpMask);
    Reg::Adds(tmpRegQ, regQ, 1, mask);
    Reg::Select(regQ, tmpRegQ, regQ, tmpMask);

    /* compute remainder of x / (pi/2) */
    // step 18: float d_h, d_l;
    // step 19: long long int P = (long long int)p;
    Reg::RegTensor<float> tmpRegDH, tmpRegDL, tmpF32Reg;
    Reg::RegTensor<int32_t> tmpI32Reg, tmpRegPHigh, tmpRegPLow;
    Reg::Copy((Reg::RegTensor<uint32_t>&)tmpRegPHigh, regPHigh);
    /*
     * d_h' = (float)P_high;
     * d_l' = (float)(P_low >> 1) * 2;
     */
    Reg::Cast<float, int32_t, castTraitI32F32>(tmpRegDH, tmpRegPHigh, mask);
    Reg::ShiftRights(tmpU32Reg, regPLow, (int16_t)1, mask);
    Reg::Cast<float, int32_t, castTraitI32F32>(tmpRegDL, (Reg::RegTensor<int32_t>&)tmpU32Reg, mask);
    Reg::Muls(tmpRegDL, tmpRegDL, P_MULS, mask);
    // next: d_l = (float)(P_high - (int)d_h') *(2**32) + (float)P_low
    Reg::Cast<int32_t, float, castTraitI32F32>(tmpI32Reg, tmpRegDH, mask);
    Reg::Sub(tmpI32Reg, tmpRegPHigh, tmpI32Reg, mask);
    Reg::Cast<float, int32_t, castTraitI32F32>(tmpF32Reg, tmpI32Reg, mask);
    Reg::Muls(tmpF32Reg, tmpF32Reg, B64_SHIFT_BITS, mask);
    Reg::Add(regDl, tmpF32Reg, tmpRegDL, mask);
    // then: d_h = d_h' * (2**32)
    Reg::Muls(regDh, tmpRegDH, B64_SHIFT_BITS, mask);
}

__simd_callee__ inline void TrigRedSlowpathFComputeP(
    Reg::RegTensor<uint32_t>& regPHigh, Reg::RegTensor<uint32_t>& regPLow, Reg::RegTensor<float>& srcReg,
    Reg::RegTensor<uint32_t>& oneOverPiFReg, Reg::MaskReg& mask)
{
    constexpr uint32_t TA_AND_COEFF = 0x007fffff;
    constexpr uint32_t IA_ADD_COEFF = 0x4f000000;
    constexpr int16_t TA_SHIFT_BITS = 23;
    constexpr int32_t TA_SHIFT_AND_COEFF = 0x000000ff;
    constexpr int16_t I_SHIFT_BITS = 5;
    constexpr int32_t E_SUB_COEFF = 126;
    constexpr int32_t E_AND_COEFF = 31;
    constexpr uint32_t LO_SELECT = 1;
    constexpr uint32_t TMP_SELECT = 2;

    Reg::RegTensor<uint32_t> regIa, regHi, regMid, regLo, regTmp, regI;
    Reg::RegTensor<int32_t> regE;
    Reg::RegTensor<int32_t> tmpI32Reg;
    Reg::RegTensor<uint32_t> tmpU32Reg;

    // step 1: unsigned int ta = reinterpret_cast<unsigned int &>(a);
    // ta can be obtained by (RegTensor<uint32_t>&)srcReg;
    // step 2: ia = (ta&0x007fffff) + 0x4f000000;
    AndScalar(regIa, (Reg::RegTensor<uint32_t>&)srcReg, TA_AND_COEFF, mask);
    Reg::Adds(regIa, regIa, IA_ADD_COEFF, mask);

    // step 3: ia = (unsigned int) reinterpret_cast<float &>(ia);
    ReinterpretedU32ToFloatAndCastToU32(regIa, regIa, mask);

    // step 4: e = ((ta >> 23) & 0x000000ff) - 127;
    Reg::ShiftRights(regE, (Reg::RegTensor<int32_t>&)srcReg, TA_SHIFT_BITS, mask);
    AndScalar(regE, regE, TA_SHIFT_AND_COEFF, mask);
    Reg::Adds(regE, regE, -E_SUB_COEFF, mask);

    // step 5: i = (unsigned int)e >> 5;
    Reg::ShiftRights(regI, (Reg::RegTensor<uint32_t>&)regE, I_SHIFT_BITS, mask);
    // step 6: e = (unsigned int)e & 31;
    Reg::Duplicate(tmpI32Reg, E_AND_COEFF, mask);
    Reg::And(regE, (Reg::RegTensor<int32_t>&)regE, tmpI32Reg, mask);

    // step 7:hi  = i ? one_over_pi_f [i-1] : 0;
    Reg::MaskReg tmpMask;
    Reg::RegTensor<uint32_t> tmpRegSelect;
    Reg::CompareScalar<uint32_t, CMPMODE::GT>(tmpMask, regI, 0, mask);
    Reg::Adds(tmpU32Reg, regI, -1, mask);
    Reg::Gather(tmpRegSelect, oneOverPiFReg, tmpU32Reg);
    Reg::Duplicate(regHi, 0, mask);
    Reg::Select(regHi, tmpRegSelect, regHi, tmpMask);

    // step 8: mid = one_over_pi_f [i+0];
    Reg::Gather(regMid, oneOverPiFReg, regI);
    // // step 9: lo  = one_over_pi_f [i+1];
    Reg::Adds(tmpU32Reg, regI, LO_SELECT, mask);
    Reg::Gather(regLo, oneOverPiFReg, tmpU32Reg);
    // step 10: tmp = one_over_pi_f [i+2];
    Reg::Adds(tmpU32Reg, regI, TMP_SELECT, mask);
    Reg::Gather(regTmp, oneOverPiFReg, tmpU32Reg);

    /* step 11:
     * if(e) {
     *    hi  = (hi  << e) | (mid >> (32 - e));
     *    mid = (mid << e) | (lo  >> (32 - e));
     *    lo  = (lo  << e) | (tmp >> (32 - e));
     * }
     */
    Reg::CompareScalar<int32_t, CMPMODE::GT>(tmpMask, regE, 0, mask);
    BitShiftCombine(tmpRegSelect, regHi, regMid, regE, mask);
    Reg::Select(regHi, tmpRegSelect, regHi, tmpMask);
    BitShiftCombine(tmpRegSelect, regMid, regLo, regE, mask);
    Reg::Select(regMid, tmpRegSelect, regMid, tmpMask);
    BitShiftCombine(tmpRegSelect, regLo, regTmp, regE, mask);
    Reg::Select(regLo, tmpRegSelect, regLo, tmpMask);

    TrigComputeP(regPHigh, regPLow, regIa, regMid, regLo, regHi, mask);
}

__simd_callee__ inline void TrigRedSlowpathFComputeRI(
    Reg::RegTensor<float>& dstRegR, Reg::RegTensor<int32_t>& dstRegI, Reg::RegTensor<uint32_t>& regPHigh,
    Reg::RegTensor<uint32_t>& regPLow, Reg::RegTensor<float>& srcReg, Reg::MaskReg& mask)
{
    constexpr float R_MUL_COEFF = 3.4061215800865545e-19;

    Reg::RegTensor<int32_t> regQ;
    Reg::RegTensor<float> regR, regDh, regDl;
    Reg::RegTensor<float> tmpF32Reg;
    Reg::RegTensor<int32_t> tmpRegQ;
    Reg::MaskReg tmpMask;

    TrigComputeHLQ(regDh, regDl, regQ, regPHigh, regPLow, mask);

    // step 23: r = d_l*3.4061215800865545e-19;
    Reg::Muls(regR, regDl, R_MUL_COEFF, mask);
    // step 24: r = r + d_h*3.4061215800865545e-19;
    Reg::Duplicate(tmpF32Reg, R_MUL_COEFF, mask);
    Reg::MulAddDst(regR, regDh, tmpF32Reg, mask);

    /* step 25:
     * if (a < 0.0f) {
     *    r = -r;
     *    q = -q;
     * }
     */
    Reg::CompareScalar<float, CMPMODE::LT>(tmpMask, srcReg, 0.0f, mask);
    Reg::Neg(tmpF32Reg, regR, mask);
    Reg::Select(regR, tmpF32Reg, regR, tmpMask);
    Reg::Neg(tmpRegQ, regQ, mask);
    Reg::Select(regQ, tmpRegQ, regQ, tmpMask);

    // step 26: *quadrant = q;
    dstRegR = regR;
    dstRegI = regQ;
}

__simd_callee__ inline void SinfPoly(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcRegA, Reg::RegTensor<float>& srcRegS, Reg::MaskReg& mask)
{
    constexpr float SIN_POLY_COEFF0 = 2.86567956e-6f;
    constexpr float SIN_POLY_COEFF1 = -1.98559923e-4f;
    constexpr float SIN_POLY_COEFF2 = 8.33338592e-3f;
    constexpr float SIN_POLY_COEFF3 = -1.66666672e-1f;
    constexpr float SIN_POLY_COEFF5 = 0.0f;

    Reg::RegTensor<float> tmpRegT;
    // step 1: r = 2.86567956e-6f;
    Reg::Duplicate(dstReg, SIN_POLY_COEFF0, mask);
    // step 2: r = r* s+ -1.98559923e-4f;
    FMaf(dstReg, srcRegS, SIN_POLY_COEFF1, mask);
    // step 3: r = r* s+  8.33338592e-3f;
    FMaf(dstReg, srcRegS, SIN_POLY_COEFF2, mask);
    // step 4: r = r* s+ -1.66666672e-1f;
    FMaf(dstReg, srcRegS, SIN_POLY_COEFF3, mask);
    // step 5: t = a* s+ 0.0f;
    FMaf(tmpRegT, srcRegA, srcRegS, SIN_POLY_COEFF5, mask);
    // step 6: r = r* t+ a;
    Reg::FusedMulDstAdd(dstReg, tmpRegT, srcRegA, mask);
}

__simd_callee__ inline void CosfPoly(Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcRegS, Reg::MaskReg& mask)
{
    constexpr float COS_POLY_COEFF0 = 2.44677067e-5f;
    constexpr float COS_POLY_COEFF1 = -1.38877297e-3f;
    constexpr float COS_POLY_COEFF2 = 4.16666567e-2f;
    constexpr float COS_POLY_COEFF3 = -5.00000000e-1f;
    constexpr float COS_POLY_COEFF4 = 1.00000000e+0f;

    // step 1: r = 2.44677067e-5f;
    Reg::Duplicate(dstReg, COS_POLY_COEFF0, mask);
    // step 2: r = r* s+ -1.38877297e-3f;
    FMaf(dstReg, srcRegS, COS_POLY_COEFF1, mask);
    // step 3: r = r* s+  4.16666567e-2f;
    FMaf(dstReg, srcRegS, COS_POLY_COEFF2, mask);
    // step 4: r = r* s+ -5.00000000e-1f;
    FMaf(dstReg, srcRegS, COS_POLY_COEFF3, mask);
    // step 5: r = r* s+  1.00000000e+0f;
    FMaf(dstReg, srcRegS, COS_POLY_COEFF4, mask);
}

__simd_callee__ inline void TrigRedFPreprocessForHalf(
    Reg::RegTensor<float>& regR, Reg::RegTensor<int32_t>& regI, Reg::RegTensor<float>& srcRegA, Reg::MaskReg& mask)
{
    constexpr float J_MUL_COEFF = 0.636619747f;
    constexpr float J_ADD_COEFF = 12582912.0f;
    constexpr float J_MUL_COEFF1 = -1.57079601e+00f;
    constexpr float J_MUL_COEFF2 = -3.13916473e-07f;
    constexpr float J_MUL_COEFF3 = -5.39030253e-15f;

    Reg::RegTensor<float> regJ;
    Reg::RegTensor<float> tmpF32Reg;
    Reg::RegTensor<int32_t> tmpI32Reg;

    // step 1: a = a * 0.0f + a; convert inf to NAN
    Reg::Duplicate(tmpF32Reg, 0.0f, mask);
    Reg::FusedMulDstAdd(srcRegA, tmpF32Reg, srcRegA, mask);

    // step 2: j = a*0.636619747f + 12582912.0f;
    FMaf(regJ, srcRegA, J_MUL_COEFF, J_ADD_COEFF, mask);
    // step 3: i = reinterpret_cast<int&> (j);
    regI = (Reg::RegTensor<int32_t>&)regJ;
    // step 4: j = j - 12582912.0f;
    Reg::Adds(regJ, regJ, -J_ADD_COEFF, mask);
    // step 5: r = j* -1.57079601e+00f+ a; // -0x1.921fb0p+00 // pio2_high
    FMaf(regR, regJ, J_MUL_COEFF1, srcRegA, mask);
    // step 6: r = j* -3.13916473e-07f+ r; // -0x1.5110b4p-22 // pio2_mid
    Reg::Duplicate(tmpF32Reg, J_MUL_COEFF2, mask);
    Reg::MulAddDst(regR, regJ, tmpF32Reg, mask);
    // step 7: r = j* -5.39030253e-15f+ r; // -0x1.846988p-48 // pio2_low
    Reg::Duplicate(tmpF32Reg, J_MUL_COEFF3, mask);
    Reg::MulAddDst(regR, regJ, tmpF32Reg, mask);
}

__simd_callee__ inline void TrigRedFComputeP(
    Reg::MaskReg& tmpMask, Reg::RegTensor<uint32_t>& regPHigh, Reg::RegTensor<uint32_t>& regPLow,
    Reg::RegTensor<float>& srcRegA, Reg::RegTensor<uint32_t>& oneOverPiFReg, Reg::MaskReg& mask)
{
    constexpr float A_ABS_COEFF = 3.1415926535f * 0.25f;
    Reg::RegTensor<float> tmpF32Reg;
    Reg::RegTensor<int32_t> tmpI32Reg;
    /* step 8:
     * if (std::abs(a) > 3.1415926535f*0.25f) {
     *     r = trig_red_slowpath_f (a, &i);
     * }
     */
    Reg::Abs(tmpF32Reg, srcRegA, mask);
    Reg::CompareScalar<float, CMPMODE::GT>(tmpMask, tmpF32Reg, A_ABS_COEFF, mask);
    TrigRedSlowpathFComputeP(regPHigh, regPLow, srcRegA, oneOverPiFReg, mask);
}

__simd_callee__ inline void TrigRedFComputeRI(
    Reg::MaskReg& tmpMask, Reg::RegTensor<float>& dstRegR, Reg::RegTensor<int32_t>& dstRegI,
    Reg::RegTensor<uint32_t>& regPHigh, Reg::RegTensor<uint32_t>& regPLow, Reg::RegTensor<float>& srcRegA,
    Reg::MaskReg& mask)
{
    constexpr float A_ABS_COEFF = 3.1415926535f * 0.25f;
    Reg::RegTensor<float> tmpF32Reg;
    Reg::RegTensor<int32_t> tmpI32Reg;
    /* step 8:
     * if (std::abs(a) > 3.1415926535f*0.25f) {
     *     r = trig_red_slowpath_f (a, &i);
     * }
     */
    Reg::Abs(tmpF32Reg, srcRegA, mask);
    Reg::CompareScalar<float, CMPMODE::GT>(tmpMask, tmpF32Reg, A_ABS_COEFF, mask);
    TrigRedSlowpathFComputeRI(dstRegR, dstRegI, regPHigh, regPLow, srcRegA, mask);
}

/* Compute sine and cosine simultaneously, based on quadrant */
__simd_callee__ inline void SCFCore(
    Reg::RegTensor<float>& dstRegSin, Reg::RegTensor<float>& dstRegCos, Reg::RegTensor<int32_t>& regI,
    Reg::RegTensor<float>& regR, Reg::MaskReg& mask)
{
    constexpr int32_t I_AND_CONDITION = 2;

    // step 9: float c, s, t;
    Reg::RegTensor<float> regC, regS, regT;
    Reg::RegTensor<float> tmpF32Reg, tmpF32Reg1;
    Reg::RegTensor<int32_t> tmpI32Reg;
    // step 10: s = r * r;
    Reg::Mul(regS, regR, regR, mask);
    // step 11: c = cosf_poly (s);
    CosfPoly(regC, regS, mask);
    // step 12: s = sinf_poly (r, s);
    SinfPoly(tmpF32Reg, regR, regS, mask);
    regS = tmpF32Reg;

    /* step 13:
     * if (i & 2) {
     *    s = 0.0f - s; // don't change "sign" of NaNs or create negative zeros
     *    c = 0.0f - c; // don't change "sign" of NaNs or create negative zeros
     * }
     */
    Reg::MaskReg tmpMask;
    Reg::Duplicate(tmpI32Reg, I_AND_CONDITION, mask);
    Reg::And(tmpI32Reg, regI, tmpI32Reg, mask);
    Reg::CompareScalar<int32_t, CMPMODE::GT>(tmpMask, tmpI32Reg, 0, mask);
    Reg::Duplicate(tmpF32Reg1, 0.0f, mask);
    Reg::Sub(tmpF32Reg, tmpF32Reg1, regS, mask);
    Reg::Select(regS, tmpF32Reg, regS, tmpMask);
    Reg::Sub(tmpF32Reg, tmpF32Reg1, regC, mask);
    Reg::Select(regC, tmpF32Reg, regC, tmpMask);

    /* step 14:
     * if (i & 1) {
     *    t = 0.0f - s; // don't change "sign" of NaNs or create negative zeros
     *    s = c;
     *    c = t;
     * }
     */
    Reg::Duplicate(tmpI32Reg, 1, mask);
    Reg::And(tmpI32Reg, regI, tmpI32Reg, mask);
    Reg::CompareScalar<int32_t, CMPMODE::GT>(tmpMask, tmpI32Reg, 0, mask);
    Reg::Duplicate(tmpF32Reg, 0.0f, mask);
    Reg::Sub(tmpF32Reg, tmpF32Reg, regS, mask);
    Reg::Select(regT, tmpF32Reg, regT, tmpMask);
    Reg::Select(regS, regC, regS, tmpMask);
    Reg::Select(regC, regT, regC, tmpMask);

    // step 15: *sp = s;  //sp is the sin result
    dstRegSin = regS;
    // step 16: *cp = c;  //cp is the cos result
    dstRegCos = regC;
}

__aicore__ inline void InitializeFloatTempBuffer(
    __ubuf__ uint32_t*& tmpBuffer, __ubuf__ float*& tmpBufferR, __ubuf__ int32_t*& tmpBufferI,
    const uint32_t alignCount)
{
    constexpr uint32_t oneOverPiFAlignedLength = 8;
    static unsigned int oneOverPiF[6] = {0x28be60db, 0x9391054a, 0x7f09d5f4, 0x7d4d3770, 0x36d8a566, 0x4f10e410};

    for (uint16_t i = 0; i < 6; ++i) {
        tmpBuffer[i] = oneOverPiF[i];
    }

    tmpBufferR = (__ubuf__ float*)((__ubuf__ uint8_t*)tmpBuffer + sizeof(uint32_t) * oneOverPiFAlignedLength);
    tmpBufferI = (__ubuf__ int32_t*)((__ubuf__ uint8_t*)tmpBufferR + sizeof(float) * alignCount);
}

__aicore__ inline void InitializeHalfTempBuffer(
    __ubuf__ uint32_t*& tmpBuffer, __ubuf__ float*& tmpBufferR, __ubuf__ int32_t*& tmpBufferI,
    const uint32_t alignCount)
{
    tmpBufferR = (__ubuf__ float*)((__ubuf__ uint8_t*)tmpBuffer);
    tmpBufferI = (__ubuf__ int32_t*)((__ubuf__ uint8_t*)tmpBufferR + sizeof(float) * alignCount);
}

template <typename T>
__simd_vf__ inline void TrigRedFPreProcessImpl(
    __ubuf__ float* tmpBufferR, __ubuf__ int32_t* tmpBufferI, __ubuf__ T* src, uint32_t calCount, uint16_t repeatTimes)
{
    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<int32_t> regI;
    Reg::RegTensor<float> castReg, regR;

    for (uint16_t i = 0; i < repeatTimes; i++) {
        Reg::MaskReg mask = Reg::UpdateMask<float>(calCount);
        Reg::LoadAlign<T, Reg::LoadDist::DIST_UNPACK_B16>(srcReg, src + i * B32_DATA_NUM_PER_REPEAT);
        Reg::Cast<float, T, SinCosImpl::castTraitF16F32>(castReg, srcReg, mask);
        TrigRedFPreprocessForHalf(regR, regI, castReg, mask);

        Reg::StoreAlign(tmpBufferR + i * B32_DATA_NUM_PER_REPEAT, regR, mask);
        Reg::StoreAlign(tmpBufferI + i * B32_DATA_NUM_PER_REPEAT, regI, mask);
    }
}

template <typename T>
__simd_vf__ inline void TrigRedFComputePImpl(
    __ubuf__ uint32_t* tmpBufferRegPHigh, __ubuf__ uint32_t* tmpBufferRegPLow, __ubuf__ T* src,
    __ubuf__ uint32_t* tmpBuffer, uint32_t calCount, uint16_t repeatTimes)
{
    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<uint32_t> oneOverPiFReg, regPHigh, regPLow, regI;
    Reg::RegTensor<float> castReg, regR, tmpF32Reg;
    Reg::MaskReg selectMask;

    // Load the array of one_over_pi_f
    Reg::LoadAlign(oneOverPiFReg, tmpBuffer);

    for (uint16_t i = 0; i < repeatTimes; i++) {
        Reg::MaskReg mask = Reg::UpdateMask<float>(calCount);
        Reg::LoadAlign(castReg, src + i * B32_DATA_NUM_PER_REPEAT);
        // a = a * 0.0f + a
        Reg::Duplicate(tmpF32Reg, 0.0f, mask);
        Reg::FusedMulDstAdd(castReg, tmpF32Reg, castReg, mask);
        // initialize q and r: *q = 0; r = a;
        Reg::Duplicate(regI, 0, mask);
        regR = castReg;
        // store the origin q and r in ub
        Reg::StoreAlign((__ubuf__ float*)tmpBufferRegPHigh + i * B32_DATA_NUM_PER_REPEAT, regR, mask);
        Reg::StoreAlign(tmpBufferRegPLow + i * B32_DATA_NUM_PER_REPEAT, regI, mask);

        TrigRedFComputeP(selectMask, regPHigh, regPLow, castReg, oneOverPiFReg, mask);

        Reg::StoreAlign(tmpBufferRegPHigh + i * B32_DATA_NUM_PER_REPEAT, regPHigh, selectMask);
        Reg::StoreAlign(tmpBufferRegPLow + i * B32_DATA_NUM_PER_REPEAT, regPLow, selectMask);
    }
}

template <typename T>
__simd_vf__ inline void TrigRedFComputeRIImpl(
    __ubuf__ uint32_t* tmpBufferRegPHigh, __ubuf__ uint32_t* tmpBufferRegPLow, __ubuf__ T* src,
    __ubuf__ uint32_t* tmpBuffer, uint32_t calCount, uint16_t repeatTimes)
{
    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<uint32_t> regPHigh, regPLow;
    Reg::RegTensor<int32_t> regI;
    Reg::RegTensor<float> castReg, regR, tmpF32Reg;
    Reg::MaskReg selectMask;

    for (uint16_t i = 0; i < repeatTimes; i++) {
        Reg::MaskReg mask = Reg::UpdateMask<float>(calCount);
        Reg::LoadAlign(castReg, src + i * B32_DATA_NUM_PER_REPEAT);
        Reg::LoadAlign(regPHigh, tmpBufferRegPHigh + i * B32_DATA_NUM_PER_REPEAT);
        Reg::LoadAlign(regPLow, tmpBufferRegPLow + i * B32_DATA_NUM_PER_REPEAT);
        // a = a * 0.0f + a
        Reg::Duplicate(tmpF32Reg, 0.0f, mask);
        Reg::FusedMulDstAdd(castReg, tmpF32Reg, castReg, mask);

        TrigRedFComputeRI(selectMask, regR, regI, regPHigh, regPLow, castReg, mask);

        Reg::StoreAlign((__ubuf__ float*)tmpBufferRegPHigh + i * B32_DATA_NUM_PER_REPEAT, regR, selectMask);
        Reg::StoreAlign((__ubuf__ int32_t*)tmpBufferRegPLow + i * B32_DATA_NUM_PER_REPEAT, regI, selectMask);
    }
}

template <typename T, int mode = 0>
__simd_vf__ inline void SCFCoreImpl(
    __ubuf__ T* dst, __ubuf__ float* tmpBufferR, __ubuf__ int32_t* tmpBufferI, uint32_t calCount, uint16_t repeatTimes)
{
    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<int32_t> regI;
    Reg::RegTensor<float> regR;
    Reg::RegTensor<float> dstRegCos, dstRegSin, dstReg;

    for (uint16_t i = 0; i < repeatTimes; i++) {
        Reg::MaskReg mask = Reg::UpdateMask<float>(calCount);
        Reg::LoadAlign(regR, tmpBufferR + i * B32_DATA_NUM_PER_REPEAT);
        Reg::LoadAlign(regI, tmpBufferI + i * B32_DATA_NUM_PER_REPEAT);

        SCFCore(dstRegSin, dstRegCos, regI, regR, mask);

        if constexpr (mode == 0) {
            dstReg = dstRegSin;
        } else {
            dstReg = dstRegCos;
        }

        if constexpr (sizeof(T) == sizeof(half)) {
            Reg::Cast<T, float, SinCosImpl::castTraitF32F16>(srcReg, dstReg, mask);
            Reg::StoreAlign<T, Reg::StoreDist::DIST_PACK_B32>(dst + i * B32_DATA_NUM_PER_REPEAT, srcReg, mask);
        } else {
            Reg::StoreAlign(dst + i * B32_DATA_NUM_PER_REPEAT, dstReg, mask);
        }
    }
}

template <typename T>
__simd_vf__ inline void BSCFCoreImpl(
    __ubuf__ T* dstSin, __ubuf__ T* dstCos, __ubuf__ float* tmpBufferR, __ubuf__ int32_t* tmpBufferI, uint32_t calCount,
    uint16_t repeatTimes)
{
    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<int32_t> regI;
    Reg::RegTensor<float> regR;
    Reg::RegTensor<float> dstRegCos, dstRegSin;

    for (uint16_t i = 0; i < repeatTimes; i++) {
        Reg::MaskReg mask = Reg::UpdateMask<float>(calCount);
        Reg::LoadAlign(regR, tmpBufferR + i * B32_DATA_NUM_PER_REPEAT);
        Reg::LoadAlign(regI, tmpBufferI + i * B32_DATA_NUM_PER_REPEAT);

        SCFCore(dstRegSin, dstRegCos, regI, regR, mask);

        if constexpr (sizeof(T) == sizeof(half)) {
            Reg::Cast<T, float, SinCosImpl::castTraitF32F16>(srcReg, dstRegSin, mask);
            Reg::StoreAlign<T, Reg::StoreDist::DIST_PACK_B32>(dstSin + i * B32_DATA_NUM_PER_REPEAT, srcReg, mask);
            Reg::Cast<T, float, SinCosImpl::castTraitF32F16>(srcReg, dstRegCos, mask);
            Reg::StoreAlign<T, Reg::StoreDist::DIST_PACK_B32>(dstCos + i * B32_DATA_NUM_PER_REPEAT, srcReg, mask);
        } else {
            Reg::StoreAlign(dstSin + i * B32_DATA_NUM_PER_REPEAT, dstRegSin, mask);
            Reg::StoreAlign(dstCos + i * B32_DATA_NUM_PER_REPEAT, dstRegCos, mask);
        }
    }
}
} // namespace SinCosImpl

template <typename T>
__aicore__ inline void SinRadianReductionImpl(
    __ubuf__ T* dst, __ubuf__ T* src, __ubuf__ uint32_t* tmpBuffer, uint32_t calCount)
{
    static_assert(
        (std::is_same_v<T, half> || std::is_same_v<T, float>), "current data type is not supported on current device!");
    constexpr uint32_t oneRepSize = GetVecLen() / sizeof(float);
    uint16_t repeatTimes = CeilDivision(calCount, oneRepSize);
    __ubuf__ float* tmpBufferR;
    __ubuf__ int32_t* tmpBufferI;
    uint32_t alignCount = (calCount + 31) / 32 * 32;

    if constexpr (std::is_same_v<T, float>) {
        SinCosImpl::InitializeFloatTempBuffer(tmpBuffer, tmpBufferR, tmpBufferI, alignCount);
        SinCosImpl::TrigRedFComputePImpl<T>(
            (__ubuf__ uint32_t*)tmpBufferR, (__ubuf__ uint32_t*)tmpBufferI, src, tmpBuffer, calCount, repeatTimes);
        SinCosImpl::TrigRedFComputeRIImpl<T>(
            (__ubuf__ uint32_t*)tmpBufferR, (__ubuf__ uint32_t*)tmpBufferI, src, tmpBuffer, calCount, repeatTimes);
    } else if constexpr (std::is_same_v<T, half>) {
        SinCosImpl::InitializeHalfTempBuffer(tmpBuffer, tmpBufferR, tmpBufferI, alignCount);
        SinCosImpl::TrigRedFPreProcessImpl<T>(tmpBufferR, tmpBufferI, src, calCount, repeatTimes);
    }
    SinCosImpl::SCFCoreImpl<T, 0>(dst, tmpBufferR, tmpBufferI, calCount, repeatTimes);
}

template <typename T>
__aicore__ inline void CosRadianReductionImpl(
    __ubuf__ T* dst, __ubuf__ T* src, __ubuf__ uint32_t* tmpBuffer, uint32_t calCount)
{
    static_assert(
        (std::is_same_v<T, half> || std::is_same_v<T, float>), "current data type is not supported on current device!");
    constexpr uint32_t oneRepSize = GetVecLen() / sizeof(float);
    uint16_t repeatTimes = CeilDivision(calCount, oneRepSize);
    __ubuf__ float* tmpBufferR;
    __ubuf__ int32_t* tmpBufferI;
    uint32_t alignCount = (calCount + 31) / 32 * 32;

    if constexpr (std::is_same_v<T, float>) {
        SinCosImpl::InitializeFloatTempBuffer(tmpBuffer, tmpBufferR, tmpBufferI, alignCount);
        SinCosImpl::TrigRedFComputePImpl<T>(
            (__ubuf__ uint32_t*)tmpBufferR, (__ubuf__ uint32_t*)tmpBufferI, src, tmpBuffer, calCount, repeatTimes);
        SinCosImpl::TrigRedFComputeRIImpl<T>(
            (__ubuf__ uint32_t*)tmpBufferR, (__ubuf__ uint32_t*)tmpBufferI, src, tmpBuffer, calCount, repeatTimes);
    } else if constexpr (std::is_same_v<T, half>) {
        SinCosImpl::InitializeHalfTempBuffer(tmpBuffer, tmpBufferR, tmpBufferI, alignCount);
        SinCosImpl::TrigRedFPreProcessImpl<T>(tmpBufferR, tmpBufferI, src, calCount, repeatTimes);
    }
    SinCosImpl::SCFCoreImpl<T, 1>(dst, tmpBufferR, tmpBufferI, calCount, repeatTimes);
}

template <const SinCosConfig& config, typename T>
__aicore__ inline void SinCosRadianReductionImpl(
    const LocalTensor<T>& dst0, const LocalTensor<T>& dst1, const LocalTensor<T>& src,
    const LocalTensor<uint8_t>& sharedTmpBuffer, uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    CHECK_FUNC_HIGHLEVEL_API(SinCos, (T, config.isReuseSource), (dst0, dst1, src, sharedTmpBuffer, calCount));
    __ubuf__ T* dstSinAddr = (__ubuf__ T*)dst0.GetPhyAddr();
    __ubuf__ T* dstCosAddr = (__ubuf__ T*)dst1.GetPhyAddr();
    __ubuf__ T* srcAddr = (__ubuf__ T*)src.GetPhyAddr();
    __ubuf__ uint32_t* tmpBuffer = (__ubuf__ uint32_t*)sharedTmpBuffer.GetPhyAddr();
    static_assert(
        (std::is_same_v<T, half> || std::is_same_v<T, float>), "current data type is not supported on current device!");
    constexpr uint32_t oneRepSize = GetVecLen() / sizeof(float);
    uint16_t repeatTimes = CeilDivision(calCount, oneRepSize);
    __ubuf__ float* tmpBufferR;
    __ubuf__ int32_t* tmpBufferI;
    uint32_t alignCount = (calCount + 31) / 32 * 32;

    if constexpr (std::is_same_v<T, float>) {
        SinCosImpl::InitializeFloatTempBuffer(tmpBuffer, tmpBufferR, tmpBufferI, alignCount);
        SinCosImpl::TrigRedFComputePImpl<T>(
            (__ubuf__ uint32_t*)tmpBufferR, (__ubuf__ uint32_t*)tmpBufferI, srcAddr, tmpBuffer, calCount, repeatTimes);
        SinCosImpl::TrigRedFComputeRIImpl<T>(
            (__ubuf__ uint32_t*)tmpBufferR, (__ubuf__ uint32_t*)tmpBufferI, srcAddr, tmpBuffer, calCount, repeatTimes);
    } else if constexpr (std::is_same_v<T, half>) {
        SinCosImpl::InitializeHalfTempBuffer(tmpBuffer, tmpBufferR, tmpBufferI, alignCount);
        SinCosImpl::TrigRedFPreProcessImpl<T>(tmpBufferR, tmpBufferI, srcAddr, calCount, repeatTimes);
    }
    SinCosImpl::BSCFCoreImpl<T>(dstSinAddr, dstCosAddr, tmpBufferR, tmpBufferI, calCount, repeatTimes);
}

template <const SinCosConfig& config, typename T>
__aicore__ inline void SinCosRadianReductionImpl(
    const LocalTensor<T>& dst0, const LocalTensor<T>& dst1, const LocalTensor<T>& src, uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    SinCosRadianReductionImpl<config, T>(dst0, dst1, src, sharedTmpBuffer, calCount);
}
} // namespace AscendC

#endif // LIB_MATH_SINCOS_SINCOS_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_SINCOS_SINCOS_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_SINCOS_SINCOS_C310_IMPL_H__
#endif
