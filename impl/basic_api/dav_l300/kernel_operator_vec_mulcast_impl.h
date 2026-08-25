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
 * \file kernel_operator_vec_mulcast_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_l300/kernel_operator_vec_mulcast_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_MULCAST_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_VEC_MULCAST_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_MULCAST_IMPL_H
#include "../../../include/basic_api/kernel_tensor.h"
#if ASCENDC_CPU_DEBUG
#include "../kernel_check.h"
#endif

namespace AscendC {
template <typename T, typename U>
constexpr __aicore__ inline void CheckMulCastSupportType()
{
    static_assert(
        SupportType<Tuple<T, U>, Tuple<int8_t, half>, Tuple<uint8_t, half>>(),
        "Failed to check dtype in "
        "MulCast, current api support dtype combination is src: half, dst: int8_t, uint8_t.");
}
namespace CastParam {
constexpr Reg::CastTrait MulCastTrait = {
    Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};
}

namespace RegMulCast {
template <typename T, typename U, typename RegT, typename RegU>
__aicore__ inline void MulCast(RegT& dstReg, RegU& src0Reg, RegU& src1Reg, Reg::MaskReg& mask)
{
    Reg::Mul<U>(src0Reg, src0Reg, src1Reg, mask);
    Reg::Cast<T, U, CastParam::MulCastTrait>(dstReg, src0Reg, mask);
    Reg::Pack<uint8_t, uint16_t, Reg::HighLowPart::LOWEST>(
        (Reg::RegTensor<uint8_t>&)dstReg, (Reg::RegTensor<uint16_t>&)dstReg);
}
} // namespace RegMulCast

template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void MulCastImpl(
    __ubuf__ T* dst, __ubuf__ U* src0, __ubuf__ U* src1, uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    CheckMulCastSupportType<T, U>();
    constexpr auto func = RegMulCast::MulCast<T, U, Reg::RegTensor<T>, Reg::RegTensor<U>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, false>(dst, src0, src1, nullptr, mask, repeatTime, repeatParams);
}

template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void MulCastImpl(
    __ubuf__ T* dst, __ubuf__ U* src0, __ubuf__ U* src1, uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    CheckMulCastSupportType<T, U>();
    constexpr auto func = RegMulCast::MulCast<T, U, Reg::RegTensor<T>, Reg::RegTensor<U>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, true>(dst, src0, src1, mask, 0, repeatTime, repeatParams);
}

template <typename T, typename U>
__simd_vf__ inline void MulCastImpl(__ubuf__ T* dst, __ubuf__ U* src0, __ubuf__ U* src1, uint32_t calCount)
{
    static_assert(
        SupportType<
            Tuple<T, U>, Tuple<int8_t, half>, Tuple<uint8_t, half>, Tuple<int32_t, int64_t>, Tuple<float, int64_t>>(),
        "Failed to check dtype in MulCast, current api support dtype combination is src: "
        "half, dst: int8_t, uint8_t; src: int64_t, dst: int32_t / float.");
    uint32_t sreg = static_cast<uint32_t>(calCount);
    if constexpr (sizeof(U) == 8) {
        const uint32_t sregLower = static_cast<uint32_t>(B64_DATA_NUM_PER_REPEAT * 2);
        const uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, sregLower));
        Reg::RegTensor<T> vDstReg0;
        Reg::RegTensor<U, Reg::RegTraitNumTwo> vDstReg1;
        Reg::RegTensor<U, Reg::RegTraitNumTwo> vSrcReg0;
        Reg::RegTensor<U, Reg::RegTraitNumTwo> vSrcReg1;
        Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            mask = Reg::UpdateMask<U, Reg::RegTraitNumTwo>(sreg);
            Reg::LoadAlign(vSrcReg0, src0 + i * sregLower);
            Reg::LoadAlign(vSrcReg1, src1 + i * sregLower);
            Reg::Mul(vDstReg1, vSrcReg0, vSrcReg1, mask);
            Reg::Cast<T, U, CastParam::MulCastTrait>(vDstReg0, vDstReg1, mask);
            Reg::StoreAlign(dst + i * sregLower, vDstReg0, mask);
        }
    } else {
        const uint32_t sregLower = static_cast<uint32_t>(GetVecLen() / sizeof(U));
        const uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, sregLower));
        Reg::RegTensor<T> dst0Reg;
        Reg::RegTensor<U> dst1Reg;
        Reg::RegTensor<U> src0Reg;
        Reg::RegTensor<U> src1Reg;
        Reg::MaskReg preg;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            preg = Reg::UpdateMask<U>(calCount);
            Reg::LoadAlign<U>(src0Reg, src0 + i * sregLower);
            Reg::LoadAlign<U>(src1Reg, src1 + i * sregLower);
            Reg::Mul<U>(dst1Reg, src0Reg, src1Reg, preg);
            Reg::Cast<T, U, CastParam::MulCastTrait>(dst0Reg, dst1Reg, preg);
            Reg::StoreAlign<T, Reg::StoreDist::DIST_PACK_B16>(dst + i * sregLower, dst0Reg, preg);
        }
    }
}

template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void MulCastCalc(
    const LocalTensor<T>& dstLocal, const LocalTensor<U>& src0Local, const LocalTensor<U>& src1Local, uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using DstPrimType = PrimT<T>;
    using SrcPrimType = PrimT<U>;
    MulCastImpl<DstPrimType, SrcPrimType, isSetMask>(
        (__ubuf__ DstPrimType*)dstLocal.GetPhyAddr(), (__ubuf__ SrcPrimType*)src0Local.GetPhyAddr(),
        (__ubuf__ SrcPrimType*)src1Local.GetPhyAddr(), mask, repeatTime, repeatParams);
}

template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void MulCastCalc(
    const LocalTensor<T>& dstLocal, const LocalTensor<U>& src0Local, const LocalTensor<U>& src1Local, uint64_t mask[2],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using DstPrimType = PrimT<T>;
    using SrcPrimType = PrimT<U>;
    MulCastImpl<DstPrimType, SrcPrimType, isSetMask>(
        (__ubuf__ DstPrimType*)dstLocal.GetPhyAddr(), (__ubuf__ SrcPrimType*)src0Local.GetPhyAddr(),
        (__ubuf__ SrcPrimType*)src1Local.GetPhyAddr(), mask, repeatTime, repeatParams);
}

template <typename T, typename U>
__aicore__ inline void MulCastCalc(
    const LocalTensor<T>& dstLocal, const LocalTensor<U>& src0Local, const LocalTensor<U>& src1Local, uint32_t calCount)
{
    using DstPrimType = PrimT<T>;
    using SrcPrimType = PrimT<U>;
    MulCastImpl<DstPrimType, SrcPrimType>(
        (__ubuf__ DstPrimType*)dstLocal.GetPhyAddr(), (__ubuf__ SrcPrimType*)src0Local.GetPhyAddr(),
        (__ubuf__ SrcPrimType*)src1Local.GetPhyAddr(), calCount);
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_MULCAST_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_MULCAST_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_MULCAST_IMPL_H__
#endif
