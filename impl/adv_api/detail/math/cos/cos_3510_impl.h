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
 * \file cos_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/cos/cos_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/cos.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_COS_COS_C310_IMPL_H__
#endif
#ifndef LIB_MATH_COS_C310_IMPL_H
#define LIB_MATH_COS_C310_IMPL_H
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../common/common.h"
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#if __NPU_ARCH__ != 3003 && __NPU_ARCH__ != 3113
#include "../sincos/sincos_3510_impl.h"
#endif
#include "cos_common_utils.h"
#endif

namespace AscendC {
namespace Internal {
// define the number of x div pi
constexpr float COS_PI_FOR_X_TODIV = 0.3183098733425140380859375;
// define the PI for compute
constexpr float PI_0 = 3.140625;
constexpr float COS_KPI_FIRS_PI_MULS = 0.0009670257568359375;
constexpr float COS_KPI_TWI_PI_MULS = 6.2771141529083251953125e-7;
constexpr float COS_KPI_THIR_PI_MULS = 1.21644916362129151821136474609375e-10;
constexpr float COS_KPI_FOR_PI_MULS = -1.0290623200529979163359041220560e-13;
// define the number of down of pi_div
constexpr float COS_PI_DOWN = 1.57079637050628662109375;
// kpi_2
constexpr float COS_PI_RESDOWN_ADDS_NEG = -0.00000004371139000189375;
// define the number of cos compute
constexpr float COS_RES_MULTI_SCA = 2.604926501e-6;
constexpr float COS_RES_ADDICT_UP = -0.0001980894471;
constexpr float COS_2ADDS = 0.008333049340;
constexpr float COS_3ADDS = -0.1666665792;
constexpr float COS_POINT_FIVE = 0.5;
constexpr float COS_M4_SCA = 4.0;
constexpr float COS_K2_SCA = -2.0;
constexpr float SCALAR_ONE = 1.0;

__simd_callee__ inline void CosPolynomialApproximation(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg, Reg::RegTensor<float>& x,
    Reg::RegTensor<float>& round, Reg::RegTensor<float>& kpi, Reg::MaskReg mask)
{
    // k = round(x * invpi + 1/2)
    Reg::Muls(round, srcReg, COS_PI_FOR_X_TODIV, mask);
    Reg::Adds(round, round, COS_POINT_FIVE, mask);
    // tie to even
    Reg::Truncate<float, RoundMode::CAST_RINT, Reg::MaskMergeMode::ZEROING>(round, round, mask);
    // x -= k * pi_0
    Reg::Muls(kpi, round, PI_0, mask);
    Reg::Sub(x, srcReg, kpi, mask);

    // x -= k * pi_1
    Reg::Muls(kpi, round, COS_KPI_FIRS_PI_MULS, mask);
    Reg::Sub(x, x, kpi, mask);

    // x = x + COS_PI_DOWN
    Reg::Adds(x, x, COS_PI_DOWN, mask);

    // x -= k * pi_2
    Reg::Muls(kpi, round, COS_KPI_TWI_PI_MULS, mask);
    Reg::Sub(x, x, kpi, mask);

    // x -= k * pi_3
    Reg::Muls(kpi, round, COS_KPI_THIR_PI_MULS, mask);
    Reg::Sub(x, x, kpi, mask);

    // x -= k * pi_4
    Reg::Muls(kpi, round, COS_KPI_FOR_PI_MULS, mask);
    Reg::Sub(x, x, kpi, mask);

    // x = x + COS_PI_RESDOWN_ADDS_NEG
    Reg::Adds(x, x, COS_PI_RESDOWN_ADDS_NEG, mask);

    // x^2 = mul(input_x, input_x)
    Reg::Mul(kpi, x, x, mask);
    // kover2
    Reg::Muls(dstReg, round, COS_POINT_FIVE, mask);
    Reg::Truncate<float, RoundMode::CAST_FLOOR, Reg::MaskMergeMode::ZEROING>(dstReg, dstReg, mask);

    // kover2floorm4
    Reg::Muls(dstReg, dstReg, COS_M4_SCA, mask);
    // k2
    Reg::Muls(round, round, COS_K2_SCA, mask);
    // sign
    Reg::Add(dstReg, dstReg, round, mask);
    Reg::Adds(dstReg, dstReg, SCALAR_ONE, mask);

    // res_up = mul(x^2, 2.604926501e-6)
    Reg::Muls(round, kpi, COS_RES_MULTI_SCA, mask);
    Reg::Adds(round, round, COS_RES_ADDICT_UP, mask);
    // res_up = mul(res_up, x^2)
    Reg::Mul(round, round, kpi, mask);
    Reg::Adds(round, round, COS_2ADDS, mask);
    // res_up = mul(res_up, x^2)
    Reg::Mul(round, round, kpi, mask);
    Reg::Adds(round, round, COS_3ADDS, mask);
    // res_up = mul(res_up, x^2)
    Reg::Mul(round, round, kpi, mask);
    Reg::Adds(round, round, SCALAR_ONE, mask);
    // sin(x) = xP(x)
    Reg::Mul(round, round, x, mask);
    Reg::Mul(dstReg, round, dstReg, mask);
    Reg::Mins(dstReg, dstReg, SCALAR_ONE, mask);
    Reg::Maxs(dstReg, dstReg, -SCALAR_ONE, mask);
}

template <typename T>
__simd_vf__ inline void CosPolynomial(__ubuf__ T* dst, __ubuf__ T* src, uint32_t calCount, uint16_t repeat)
{
    Reg::RegTensor<T> x;
    Reg::RegTensor<float> xTmp;
    Reg::RegTensor<float> round;
    Reg::RegTensor<float> kpi;
    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<float> srcTmp;
    Reg::RegTensor<T> dstReg;
    Reg::RegTensor<float> dstTmp;
    constexpr uint32_t oneRepSize = GetVecLen() / sizeof(float);
    Reg::MaskReg mask;
    Reg::MaskReg maskAll = Reg::CreateMask<uint8_t>();
    for (uint16_t i = 0; i < repeat; i++) {
        mask = Reg::UpdateMask<float>(calCount);
        if constexpr (std::is_same<T, half>::value) {
            Reg::LoadAlign<T, Reg::LoadDist::DIST_UNPACK_B16>(srcReg, src + i * oneRepSize);
            Reg::Cast<float, half, castTraitB16ToB32>(srcTmp, srcReg, mask);
            CosPolynomialApproximation(dstTmp, srcTmp, xTmp, round, kpi, mask);
            Reg::Cast<half, float, castTraitB32ToB16>(dstReg, dstTmp, mask);
            Reg::StoreAlign<T, Reg::StoreDist::DIST_PACK_B32>(dst + i * oneRepSize, dstReg, mask);
        } else {
            Reg::LoadAlign(srcReg, src + i * oneRepSize);
            CosPolynomialApproximation(dstReg, srcReg, xTmp, round, kpi, mask);
            Reg::StoreAlign(dst + i * oneRepSize, dstReg, mask);
        }
    }
}
} // namespace Internal

template <typename T>
__aicore__ inline void CosPolynomialImpl(__ubuf__ T* dst, __ubuf__ T* src, uint32_t calCount)
{
    constexpr uint32_t oneRepSize = GetVecLen() / sizeof(float);
    uint16_t repeat = CeilDivision(calCount, oneRepSize);
    Internal::CosPolynomial<T>(dst, src, calCount, repeat);
}

__aicore__ inline constexpr uint32_t GetCosTmpBufferLiveNode()
{
    constexpr uint32_t tmpBufferLiveNode = sizeof(float) * 2;
    return tmpBufferLiveNode;
}

template <typename T>
__aicore__ inline uint32_t GetCosTmpBufferSize(const LocalTensor<uint8_t>& sharedTmpBuffer)
{
    uint32_t sharedTmpBufferSize = sharedTmpBuffer.GetSize() / GetCosTmpBufferLiveNode();
    return AlignUp(sharedTmpBufferSize, GetDataBlockSizeInBytes()) / sizeof(T);
}

template <typename T, bool isReuseSource = false, const CosConfig& config = defaultCosConfig>
__aicore__ inline void CosImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }

    static_assert(SupportType<T, half, float>(), "current data type is not supported on current device!");
    CheckTensorPos<T>(dstTensor, Hardware::UB, "dstTensor", "VECIN/VECCALC/VECOUT", "Cos");
    CheckTensorPos<T>(srcTensor, Hardware::UB, "srcTensor", "VECIN/VECCALC/VECOUT", "Cos");
    CheckTensorPos<uint8_t>(sharedTmpBuffer, Hardware::UB, "sharedTmpBuffer", "VECIN/VECCALC/VECOUT", "Cos");
    ASCENDC_ASSERT((calCount <= srcTensor.GetSize()), {
        KERNEL_LOG(
            KERNEL_ERROR, "calCount is %u, which should not be larger than srcTensor length %u", calCount,
            srcTensor.GetSize());
    });
    ASCENDC_ASSERT((calCount <= dstTensor.GetSize()), {
        KERNEL_LOG(
            KERNEL_ERROR, "calCount is %u, which should not be larger than dstTensor length %u", calCount,
            dstTensor.GetSize());
    });

    if constexpr (config.algo == CosAlgo::POLYNOMIAL_APPROXIMATION) {
        CosPolynomialImpl((__ubuf__ T*)dstTensor.GetPhyAddr(), (__ubuf__ T*)srcTensor.GetPhyAddr(), calCount);
    } else if constexpr (config.algo == CosAlgo::RADIAN_REDUCTION) {
        uint32_t sharedTmpBufferSize = GetCosTmpBufferSize<T>(sharedTmpBuffer);
        uint32_t count = calCount;
        uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(calCount, sharedTmpBufferSize));
        for (uint16_t i = 0; i < repeatTimes; i++) {
            uint32_t remainCount = count - sharedTmpBufferSize * i;
            uint32_t oneRepSize = remainCount < sharedTmpBufferSize ? remainCount : sharedTmpBufferSize;
            CosRadianReductionImpl(
                (__ubuf__ T*)dstTensor.GetPhyAddr() + i * sharedTmpBufferSize,
                (__ubuf__ T*)srcTensor.GetPhyAddr() + i * sharedTmpBufferSize,
                (__ubuf__ uint32_t*)sharedTmpBuffer.GetPhyAddr(), oneRepSize);
        }
    }
}

template <typename T, bool isReuseSource = false, const CosConfig& config = defaultCosConfig>
__aicore__ inline void CosImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }

    // Using the Stack Space to Allocate tmpBuffer
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    CosImpl<T, isReuseSource, config>(dstTensor, srcTensor, sharedTmpBuffer, calCount);
}

__ASC_USE_RESERVED_UBUF__(3510, "Cos is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void CosCastFullMask(
    const LocalTensor<float>& dstTensor, const LocalTensor<float>& srcTensor, RoundMode castType)
{
    uint64_t newMask = 64;
    Cast<float, float, false>(
        dstTensor, srcTensor, castType, newMask, 1, {1, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE});
    PipeBarrier<PIPE_V>();
}
} // namespace AscendC
#endif // LIB_MATH_COS_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_COS_COS_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_COS_COS_C310_IMPL_H__
#endif
