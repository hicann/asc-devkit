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
 * \file gelu_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/activation/gelu/gelu_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/gelu.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_GELU_C310_IMPL_H__
#endif

#ifndef IMPL_ACTIVATION_GELU_GELU_IMPL_C310_H
#define IMPL_ACTIVATION_GELU_GELU_IMPL_C310_H

#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../common/check.h"
#include "../../common/common.h"

namespace AscendC {
namespace Internal {

template <typename T, bool highPrecision>
__simd_vf__ inline void GeluImplVF(__ubuf__ T* dst, __ubuf__ T* src, uint32_t count, const uint16_t repeatTimes)
{
    constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(T));
    constexpr float coefficientsA = 0.044715;
    constexpr float coefficientsB = 1.5957691216057308;
    Reg::RegTensor<T> srcVreg;
    Reg::RegTensor<T> dstVreg;
    Reg::RegTensor<T> tmpReg0;
    Reg::RegTensor<T> tmpReg1;
    Reg::RegTensor<T> tmpReg2;
    Reg::RegTensor<T> tmpReg3;
    Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = Reg::UpdateMask<T>(count);
        if constexpr (highPrecision) {
            Reg::LoadAlign<half, Reg::LoadDist::DIST_UNPACK_B16>(
                (Reg::RegTensor<half>&)srcVreg, (__ubuf__ half*)src + i * oneRepElm);
            Reg::Cast<float, half, castTraitB16ToB32>(srcVreg, (Reg::RegTensor<half>&)srcVreg, mask);
        } else {
            Reg::LoadAlign(srcVreg, src + i * oneRepElm);
        }
        // y = (input_x + 0.044715 * input_x ^ 3) * 1.5957691
        Reg::Mul(tmpReg0, srcVreg, srcVreg, mask);
        Reg::Mul(tmpReg0, tmpReg0, srcVreg, mask);
        Reg::Muls(tmpReg0, tmpReg0, coefficientsA, mask);
        Reg::Add(tmpReg0, tmpReg0, srcVreg, mask);
        Reg::Muls(tmpReg0, tmpReg0, coefficientsB, mask);
        // exp(min(y, 0))
        Reg::Mins(tmpReg1, tmpReg0, 0.0f, mask);
        Reg::Exp(tmpReg1, tmpReg1, mask);
        // x / (exp^(-abs(y)) + 1)
        Reg::Abs(tmpReg2, tmpReg0, mask);
        Reg::Muls(tmpReg2, tmpReg2, -1.0f, mask);
        Reg::Exp(tmpReg3, tmpReg2, mask);
        Reg::Adds(tmpReg3, tmpReg3, 1.0f, mask);
        Reg::Div(tmpReg3, srcVreg, tmpReg3, mask);
        // x / (exp^(-abs(y)) + 1) * exp(min(y, 0))
        Reg::Mul(dstVreg, tmpReg1, tmpReg3, mask);
        if constexpr (highPrecision) {
            Reg::Cast<half, float, castTraitB32ToB16>((Reg::RegTensor<half>&)dstVreg, dstVreg, mask);
            Reg::StoreAlign<half, Reg::StoreDist::DIST_PACK_B32>(
                (__ubuf__ half*)dst + i * oneRepElm, (Reg::RegTensor<half>&)dstVreg, mask);
        } else {
            Reg::StoreAlign(dst + i * oneRepElm, dstVreg, mask);
        }
    }
}

template <typename T>
__simd_callee__ inline void FastGeluCoreAlg(
    Reg::RegTensor<T>& dstVreg, Reg::RegTensor<T>& srcVreg, Reg::MaskReg& mask, Reg::RegTensor<T>& stackVreg)
{
    constexpr float coefficients = -1.702f;
    constexpr float oneFloatScalar = 1.0f;
    Reg::Muls(stackVreg, srcVreg, coefficients, mask);
    Reg::Exp(stackVreg, stackVreg, mask);
    Reg::Adds(stackVreg, stackVreg, oneFloatScalar, mask);
    Reg::Div(dstVreg, srcVreg, stackVreg, mask);
}

template <typename T = half>
__simd_vf__ inline void FastGeluHighPrecisionAlgVF(__ubuf__ T* dst, __ubuf__ T* src, const uint32_t dataSize)
{
    Reg::RegTensor<T> srcVreg;
    Reg::RegTensor<float> srcVregFloat;
    Reg::RegTensor<T> dstVreg;
    Reg::RegTensor<float> dstVregFloat;

    constexpr uint32_t stackSize = GetVecLen() / sizeof(float);
    uint32_t sreg = dataSize;

    Reg::RegTensor<float> stackVregFloat;

    Reg::MaskReg mask;

    uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(dataSize, stackSize));
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = Reg::UpdateMask<float>(sreg);
        Reg::LoadAlign<T, Reg::LoadDist::DIST_UNPACK_B16>(srcVreg, src + i * stackSize);
        Reg::Cast<float, half, castTraitB16ToB32>(srcVregFloat, srcVreg, mask);

        FastGeluCoreAlg<float>(dstVregFloat, srcVregFloat, mask, stackVregFloat);

        Reg::Cast<half, float, castTraitB32ToB16>(dstVreg, dstVregFloat, mask);
        Reg::StoreAlign<T, Reg::StoreDist::DIST_PACK_B32>(dst + i * stackSize, dstVreg, mask);
    }
}

template <typename T = half>
__aicore__ inline void FastGeluHighPrecisionAlg(
    const LocalTensor<half>& dstLocal, const LocalTensor<half>& srcLocal, const uint32_t dataSize)
{
    __ubuf__ T* src = (__ubuf__ T*)srcLocal.GetPhyAddr();
    __ubuf__ T* dst = (__ubuf__ T*)dstLocal.GetPhyAddr();

    FastGeluHighPrecisionAlgVF<T>(dst, src, dataSize);
}

template <typename T>
__simd_vf__ inline void FastGeluAlgVF(__ubuf__ T* dst, __ubuf__ T* src, const uint32_t dataSize)
{
    Reg::RegTensor<T> srcVreg;
    Reg::RegTensor<T> dstVreg;
    constexpr uint32_t stackSize = GetVecLen() / sizeof(T);
    uint32_t sreg = dataSize;
    Reg::RegTensor<T> stackVreg;
    Reg::MaskReg mask;
    uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(dataSize, stackSize));
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = Reg::UpdateMask<T>(sreg);
        Reg::LoadAlign<T>(srcVreg, src + i * stackSize);
        FastGeluCoreAlg<T>(dstVreg, srcVreg, mask, stackVreg);
        Reg::StoreAlign<T>(dst + i * stackSize, dstVreg, mask);
    }
}

template <typename T>
__aicore__ inline void FastGeluAlg(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dataSize)
{
    __ubuf__ T* src = (__ubuf__ T*)srcLocal.GetPhyAddr();
    __ubuf__ T* dst = (__ubuf__ T*)dstLocal.GetPhyAddr();

    FastGeluAlgVF<T>(dst, src, dataSize);
}

template <typename T>
__simd_callee__ inline void FastGeluV2CoreAlg(
    Reg::RegTensor<T>& dstVreg, Reg::RegTensor<T>& srcVreg, Reg::MaskReg& mask, Reg::RegTensor<T>& stackVregA,
    Reg::RegTensor<T>& stackVregB, Reg::RegTensor<T>& stackVregC)
{
    constexpr float coefficients = 0.000000000001;
    constexpr float coefficientsHalf = 0.5;
    constexpr float coefficientsA = -0.1444;
    constexpr float coefficientsB = -1.769;
    constexpr float coefficientsBInv = 1.769;
    constexpr float coefficientsC = 0.7071;
    constexpr float coefficientsD = 0.5;
    Reg::Muls(stackVregA, srcVreg, coefficientsC, mask);
    Reg::Abs(stackVregA, stackVregA, mask);
    Reg::Mins(stackVregA, stackVregA, coefficientsBInv, mask);
    Reg::Adds(stackVregA, stackVregA, coefficientsB, mask);
    Reg::Mul(stackVregA, stackVregA, stackVregA, mask);
    Reg::Muls(stackVregA, stackVregA, coefficientsA, mask);
    Reg::Adds(stackVregA, stackVregA, coefficientsD, mask);

    Reg::Adds(stackVregB, srcVreg, coefficients, mask);
    Reg::Abs(stackVregC, stackVregB, mask);
    Reg::Div(stackVregB, stackVregB, stackVregC, mask);

    Reg::Mul(stackVregA, stackVregA, stackVregB, mask);
    Reg::Adds(stackVregA, stackVregA, coefficientsHalf, mask);

    Reg::Mul(dstVreg, srcVreg, stackVregA, mask);
}

template <typename T = half>
__simd_vf__ inline void FastGeluV2HighPrecisionAlgVF(__ubuf__ T* dst, __ubuf__ T* src, const uint32_t dataSize)
{
    Reg::RegTensor<T> srcVreg;
    Reg::RegTensor<float> srcVregFloat;
    Reg::RegTensor<T> dstVreg;
    Reg::RegTensor<float> dstVregFloat;

    constexpr uint32_t stackSize = GetVecLen() / sizeof(float);
    uint32_t sreg = dataSize;

    Reg::RegTensor<float> stackVregFloat;

    Reg::MaskReg mask;

    Reg::RegTensor<float> stackVregA;
    Reg::RegTensor<float> stackVregB;
    Reg::RegTensor<float> stackVregC;

    uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(dataSize, stackSize));
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = Reg::UpdateMask<float>(sreg);
        Reg::LoadAlign<T, Reg::LoadDist::DIST_UNPACK_B16>(srcVreg, src + i * stackSize);
        Reg::Cast<float, half, castTraitB16ToB32>(srcVregFloat, srcVreg, mask);

        FastGeluV2CoreAlg<float>(dstVregFloat, srcVregFloat, mask, stackVregA, stackVregB, stackVregC);

        Reg::Cast<half, float, castTraitB32ToB16>(dstVreg, dstVregFloat, mask);
        Reg::StoreAlign<T, Reg::StoreDist::DIST_PACK_B32>(dst + i * stackSize, dstVreg, mask);
    }
}

template <typename T = half>
__aicore__ inline void FastGeluV2HighPrecisionAlg(
    const LocalTensor<half>& dstLocal, const LocalTensor<half>& srcLocal, const uint32_t dataSize)
{
    __ubuf__ T* src = (__ubuf__ T*)srcLocal.GetPhyAddr();
    __ubuf__ T* dst = (__ubuf__ T*)dstLocal.GetPhyAddr();

    FastGeluV2HighPrecisionAlgVF<T>(dst, src, dataSize);
}

template <typename T>
__simd_vf__ inline void FastGeluV2AlgVF(__ubuf__ T* dst, __ubuf__ T* src, const uint32_t dataSize)
{
    Reg::RegTensor<T> srcVreg;
    Reg::RegTensor<T> dstVreg;
    constexpr uint32_t stackSize = GetVecLen() / sizeof(T);
    uint32_t sreg = dataSize;

    Reg::RegTensor<T> stackVregA;
    Reg::RegTensor<T> stackVregB;
    Reg::RegTensor<T> stackVregC;
    Reg::MaskReg mask;
    uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(dataSize, stackSize));
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = Reg::UpdateMask<T>(sreg);
        Reg::LoadAlign<T>(srcVreg, src + i * stackSize);
        FastGeluV2CoreAlg<T>(dstVreg, srcVreg, mask, stackVregA, stackVregB, stackVregC);
        Reg::StoreAlign<T>(dst + i * stackSize, dstVreg, mask);
    }
}

template <typename T>
__aicore__ inline void FastGeluV2Alg(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dataSize)
{
    __ubuf__ T* src = (__ubuf__ T*)srcLocal.GetPhyAddr();
    __ubuf__ T* dst = (__ubuf__ T*)dstLocal.GetPhyAddr();

    FastGeluV2AlgVF<T>(dst, src, dataSize);
}
} // namespace Internal

template <typename T, bool highPrecision = false, bool highPerformance = false>
__aicore__ inline void GeluImpl(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t count)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    (void)highPerformance;
    static_assert(SupportType<T, half, float>(), "Gelu only support half/float data type on current device!");
    CheckTensorPosition(dstLocal, "dstLocal", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcLocal, "srcLocal", "VECIN/VECOUT/VECCALC");
    CheckCalCount(count, "calCount", dstLocal, "dstLocal", "Gelu");
    CheckCalCount(count, "calCount", srcLocal, "srcLocal", "Gelu");
    if constexpr (highPrecision && sizeof(T) == sizeof(half)) {
        constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(float));
        uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(count, oneRepElm));
        Internal::GeluImplVF<float, true>(
            (__ubuf__ float*)dstLocal.GetPhyAddr(), (__ubuf__ float*)srcLocal.GetPhyAddr(), count, repeatTimes);
    } else {
        constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(T));
        uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(count, oneRepElm));
        Internal::GeluImplVF<T, false>(
            (__ubuf__ T*)dstLocal.GetPhyAddr(), (__ubuf__ T*)srcLocal.GetPhyAddr(), count, repeatTimes);
    }
}

template <typename T, bool highPrecision = false, bool highPerformance = false>
__aicore__ inline void GeluImpl(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t count)
{
    CheckTensorPosition(sharedTmpBuffer, "sharedTmpBuffer", "VECIN/VECOUT/VECCALC");
    GeluImpl<T, highPrecision, highPerformance>(dstLocal, srcLocal, count);
}

template <typename T, bool highPrecision = false, bool highPerformance = false>
__aicore__ inline void FasterGeluImpl(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t dataSize)
{
    (void)sharedTmpBuffer;
    (void)highPerformance;
    static_assert((SupportType<T, half, float>()), "current data type is not supported on current device!");
#if ASCENDC_CPU_DEBUG
    bool ret = (dataSize <= srcLocal.GetSize()) && (dataSize <= dstLocal.GetSize()) && (dataSize > 0);
    ASCENDC_ASSERT(
        ret, { KERNEL_LOG(KERNEL_ERROR, "DataSize must bigger than 0 and smaller than or equal to src&dst tensor."); });
#endif

    if constexpr (highPrecision && (IsSameType<T, half>::value)) {
        Internal::FastGeluHighPrecisionAlg(dstLocal, srcLocal, dataSize);
    } else {
        Internal::FastGeluAlg(dstLocal, srcLocal, dataSize);
    }
}

template <typename T, bool highPrecision = false, bool highPerformance = false>
__aicore__ inline void FasterGeluImpl(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dataSize)
{
    LocalTensor<uint8_t> sharedTmpBuffer;
    FasterGeluImpl<T, highPrecision, highPerformance>(dstLocal, srcLocal, sharedTmpBuffer, dataSize);
}

template <typename T, bool highPrecision = false, bool highPerformance = false>
__aicore__ inline void FasterGeluV2Impl(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t dataSize)
{
    (void)sharedTmpBuffer;
    (void)highPerformance;
    static_assert((SupportType<T, half, float>()), "current data type is not supported on current device!");
#if ASCENDC_CPU_DEBUG
    bool ret = (dataSize <= srcLocal.GetSize()) && (dataSize <= dstLocal.GetSize()) && (dataSize > 0);
    ASCENDC_ASSERT(
        ret, { KERNEL_LOG(KERNEL_ERROR, "DataSize must bigger than 0 and smaller than or equal to src&dst tensor."); });
#endif

    if constexpr (highPrecision && (IsSameType<T, half>::value)) {
        Internal::FastGeluV2HighPrecisionAlg(dstLocal, srcLocal, dataSize);
    } else {
        Internal::FastGeluV2Alg(dstLocal, srcLocal, dataSize);
    }
}

template <typename T, bool highPrecision = false, bool highPerformance = false>
__aicore__ inline void FasterGeluV2Impl(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dataSize)
{
    LocalTensor<uint8_t> sharedTmpBuffer;
    FasterGeluV2Impl<T, highPrecision, highPerformance>(dstLocal, srcLocal, sharedTmpBuffer, dataSize);
}
#pragma end_pipe
} // namespace AscendC
#endif // IMPL_ACTIVATION_GELU_GELU_IMPL_C310_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_GELU_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_GELU_C310_IMPL_H__
#endif
