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
 * \file ascend_quant_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/quantization/quant/ascend_quant_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/quantization/ascend_quant.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_QUANT_ASCEND_QUANT_C310_IMPL_H__
#endif

#ifndef LIB_ASCEND_QUANT_ASCEND_QUANT_C310_IMPL_H
#define LIB_ASCEND_QUANT_ASCEND_QUANT_C310_IMPL_H
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "kernel_tiling/kernel_tiling.h"
#include "../../common/check.h"
#include "../../../../../include/adv_api/quantization/ascend_quant_utils.h"

namespace AscendC {
constexpr uint32_t ASCENDC_QUANT_B16_VF_LEN = GetVecLen() / sizeof(uint16_t);
constexpr uint32_t ASCENDC_QUANT_B32_VF_LEN = GetVecLen() / sizeof(uint32_t);

template <typename dstT, typename srcT>
__simd_vf__ inline void QuantPertensorForB8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, const float scale, const float offset, const uint32_t calCount)
{
    Reg::MaskReg preg;
    Reg::RegTensor<bfloat16_t> b16vreg;
    Reg::RegTensor<half> f16Vreg;
    Reg::RegTensor<dstT> s8vreg;

    uint32_t sregLower = (uint32_t)ASCENDC_QUANT_B16_VF_LEN;
    uint32_t sreg = (uint32_t)calCount;
    uint16_t repeat = CeilDivision(calCount, sregLower);

    for (uint16_t i = 0; i < (uint16_t)repeat; ++i) {
        preg = Reg::UpdateMask<uint16_t>(sreg);

        if constexpr (SupportType<srcT, bfloat16_t>()) {
            Reg::LoadAlign<bfloat16_t, Reg::LoadDist::DIST_NORM>(b16vreg, srcUb + i * sregLower);
            Reg::Cast<half, bfloat16_t, MrgZRndRSatS>(f16Vreg, b16vreg, preg);
        } else {
            Reg::LoadAlign<half, Reg::LoadDist::DIST_NORM>(f16Vreg, srcUb + i * sregLower);
        }

        Reg::Muls<half, half, Reg::MaskMergeMode::ZEROING>(f16Vreg, f16Vreg, static_cast<half>(scale), preg);
        Reg::Adds<half, half, Reg::MaskMergeMode::ZEROING>(f16Vreg, f16Vreg, static_cast<half>(offset), preg);
        if constexpr (SupportType<dstT, int8_t>()) {
            Reg::Cast<dstT, half, LayoutZMrgZRndRSatS>(s8vreg, f16Vreg, preg);
        } else {
            Reg::Cast<dstT, half, LayoutZMrgZRndASatS>(s8vreg, f16Vreg, preg);
        }
        Reg::StoreAlign<dstT, Reg::StoreDist::DIST_PACK_B16>(dstUb + i * sregLower, s8vreg, preg);
    }
}

/* **************************************************************************************************
 * pertensor process for int8/hif8 output                                             *
 * ************************************************************************************************* */
template <typename dstT, typename srcT>
__aicore__ inline void QuantPertensorForB8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const float scale, const float offset,
    const uint32_t calCount)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    QuantPertensorForB8VF<dstT, srcT>(dstUb, srcUb, scale, offset, calCount);
}

template <typename dstT, typename srcT>
__simd_vf__ inline void QuantPertensorForB8VF(
    __ubuf__ dstT* dstUb, __ubuf__ float* srcUb, const float scale, const float offset, const uint32_t calCount)
{
    Reg::MaskReg preg;
    Reg::RegTensor<float> f32vreg;
    Reg::RegTensor<half> f16Vreg;
    Reg::RegTensor<dstT> s8vreg;

    uint32_t sregLower = (uint32_t)ASCENDC_QUANT_B32_VF_LEN;
    uint32_t sreg = (uint32_t)calCount;
    uint16_t repeat = CeilDivision(calCount, sregLower);

    for (uint16_t i = 0; i < (uint16_t)repeat; ++i) {
        preg = Reg::UpdateMask<uint32_t>(sreg);
        Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(f32vreg, srcUb + i * sregLower);
        Reg::Cast<half, float, LayoutZMrgZRndRSatS>(f16Vreg, f32vreg, preg);

        Reg::Muls<half, half, Reg::MaskMergeMode::ZEROING>(f16Vreg, f16Vreg, static_cast<half>(scale), preg);
        Reg::Adds<half, half, Reg::MaskMergeMode::ZEROING>(f16Vreg, f16Vreg, static_cast<half>(offset), preg);

        if constexpr (SupportType<dstT, int8_t>()) {
            Reg::Cast<dstT, half, LayoutZMrgZRndRSatS>(s8vreg, f16Vreg, preg);
        } else {
            Reg::Cast<dstT, half, LayoutZMrgZRndASatS>(s8vreg, f16Vreg, preg);
        }
        Reg::StoreAlign<dstT, Reg::StoreDist::DIST_PACK4_B32>(dstUb + i * sregLower, s8vreg, preg);
    }
}

template <typename dstT, typename srcT>
__aicore__ inline void QuantPertensorForB8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<float>& srcTensor, const float scale, const float offset,
    const uint32_t calCount)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ float* srcUb = (__ubuf__ float*)srcTensor.GetPhyAddr();
    QuantPertensorForB8VF<dstT, srcT>(dstUb, srcUb, scale, offset, calCount);
}

template <typename T, bool isReuseSource = false, const AscendQuantConfig& config>
__aicore__ inline void AscendQuantImpl(
    const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const float scale, const float offset, const uint32_t calCount)
{
    if ASCEND_IS_AIC {
        return;
    }
    CheckTensorPosition(dstTensor, "dstTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcTensor, "srcTensor", "VECIN/VECOUT/VECCALC");
    static_assert(
        SupportType<T, half, float, bfloat16_t>(), "This AscendQuant only support half/float/bfloat16_t input dtype");

    const uint32_t calCountReal = config.calcCount != 0 ? config.calcCount : calCount;
    ASCENDC_ASSERT((calCountReal <= srcTensor.GetSize() && calCountReal <= dstTensor.GetSize() && calCountReal >= 0), {
        KERNEL_LOG(
            KERNEL_ERROR, "calCount is %u, which should be in [0, min(%u, %u)]", calCountReal, srcTensor.GetSize(),
            dstTensor.GetSize());
    });
    QuantPertensorForB8<int8_t, T>(dstTensor, srcTensor, scale, offset, calCountReal);
}

template <typename dstT, typename srcT>
__simd_vf__ inline void QuantPertensorForFp8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, const float scale, const float offset, const uint32_t calCount)
{
    Reg::MaskReg preg;
    Reg::RegTensor<float> f32vreg;
    Reg::RegTensor<srcT> b16vreg;
    Reg::RegTensor<dstT> b8vreg;

    uint32_t sregLower = (uint32_t)ASCENDC_QUANT_B32_VF_LEN;
    uint32_t sreg = (uint32_t)calCount;
    uint16_t repeat = CeilDivision(calCount, sregLower);

    for (uint16_t i = 0; i < (uint16_t)repeat; ++i) {
        preg = Reg::UpdateMask<uint32_t>(sreg);
        if constexpr (SupportType<srcT, half, bfloat16_t>()) {
            Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(b16vreg, srcUb + i * sregLower);
            Reg::Cast<float, srcT, layoutZMrgZ>(f32vreg, b16vreg, preg);
        } else {
            Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(f32vreg, srcUb + i * sregLower);
        }

        Reg::Muls<float, float, Reg::MaskMergeMode::ZEROING>(f32vreg, f32vreg, static_cast<float>(scale), preg);
        Reg::Adds<float, float, Reg::MaskMergeMode::ZEROING>(f32vreg, f32vreg, static_cast<float>(offset), preg);

        Reg::Cast<dstT, float, LayoutZMrgZRndRSatS>(b8vreg, f32vreg, preg);
        Reg::StoreAlign<dstT, Reg::StoreDist::DIST_PACK4_B32>(dstUb + i * sregLower, b8vreg, preg);
    }
}

/* **************************************************************************************************
 * pertensor process for fp8 output                                             *
 * ************************************************************************************************* */
template <typename dstT, typename srcT>
__aicore__ inline void QuantPertensorForFp8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const float scale, const float offset,
    const uint32_t calCount)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    QuantPertensorForFp8VF<dstT, srcT>(dstUb, srcUb, scale, offset, calCount);
}

template <typename dstT, typename srcT, bool isReuseSource = false>
__aicore__ inline void AscendQuantImpl(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const float scale, const float offset, const uint32_t calCount)
{
    if ASCEND_IS_AIC {
        return;
    }
    CheckTensorPosition(dstTensor, "dstTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcTensor, "srcTensor", "VECIN/VECOUT/VECCALC");
    static_assert(
        SupportType<srcT, half, float, bfloat16_t>(),
        "This AscendQuant only support half/float/bfloat16_t input dtype");
    static_assert(
        SupportType<dstT, int8_t, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t>(),
        "This AscendQuant only support int8_t/fp8_e4m3fn_t/fp8_e5m2_t/hifloat8_t output dtype");
    ASCENDC_ASSERT((calCount <= srcTensor.GetSize() && calCount <= dstTensor.GetSize() && calCount >= 0), {
        KERNEL_LOG(
            KERNEL_ERROR, "calCount is %u, which should be in [0, min(%u, %u)]", calCount, srcTensor.GetSize(),
            dstTensor.GetSize());
    });
    if constexpr (SupportType<dstT, fp8_e4m3fn_t, fp8_e5m2_t>()) {
        QuantPertensorForFp8<dstT, srcT>(dstTensor, srcTensor, scale, offset, calCount);
    } else {
        QuantPertensorForB8<dstT, srcT>(dstTensor, srcTensor, scale, offset, calCount); // for int8/hif8 output
    }
}

template <typename dstT, typename srcT>
__simd_vf__ inline void QuantPerchannelForFp8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ srcT* scaleUb, __ubuf__ srcT* offsetUb,
    const uint32_t scaleCount, const uint32_t rowNum)
{
    Reg::MaskReg preg;
    Reg::RegTensor<float> f32vreg;
    Reg::RegTensor<float> offsetf32vreg;
    Reg::RegTensor<float> scalef32vreg;

    Reg::RegTensor<srcT> b16vreg;
    Reg::RegTensor<srcT> offsetB16Vreg;
    Reg::RegTensor<srcT> scaleB16Vreg;
    Reg::RegTensor<dstT> b8vreg;

    uint32_t sregLower = (uint32_t)ASCENDC_QUANT_B32_VF_LEN;

    for (uint16_t i = 0; i < (uint16_t)rowNum; ++i) {
        uint32_t sreg = (uint32_t)scaleCount;
        uint16_t repeat = CeilDivision(scaleCount, sregLower);
        for (uint16_t j = 0; j < (uint16_t)repeat; ++j) {
            preg = Reg::UpdateMask<uint32_t>(sreg);
            if constexpr (SupportType<srcT, half, bfloat16_t>()) {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(b16vreg, srcUb + i * scaleCount + j * sregLower);
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(scaleB16Vreg, scaleUb + j * sregLower);
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(offsetB16Vreg, offsetUb + j * sregLower);
                Reg::Cast<float, srcT, layoutZMrgZ>(f32vreg, b16vreg, preg);
                Reg::Cast<float, srcT, layoutZMrgZ>(scalef32vreg, scaleB16Vreg, preg);
                Reg::Cast<float, srcT, layoutZMrgZ>(offsetf32vreg, offsetB16Vreg, preg);
            } else {
                Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(f32vreg, srcUb + i * scaleCount + j * sregLower);
                Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(scalef32vreg, scaleUb + j * sregLower);
                Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(offsetf32vreg, offsetUb + j * sregLower);
            }

            Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32vreg, f32vreg, scalef32vreg, preg);
            Reg::Add<float, Reg::MaskMergeMode::ZEROING>(f32vreg, f32vreg, offsetf32vreg, preg);

            Reg::Cast<dstT, float, LayoutZMrgZRndRSatS>(b8vreg, f32vreg, preg);
            Reg::StoreAlign<dstT, Reg::StoreDist::DIST_PACK4_B32>(dstUb + i * scaleCount + j * sregLower, b8vreg, preg);
        }
    }
}

/* **************************************************************************************************
 * perchannel process                                              *
 * ************************************************************************************************* */
template <typename dstT, typename srcT>
__aicore__ inline void QuantPerchannelForFp8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<srcT>& scaleTensor,
    const LocalTensor<srcT>& offsetTensor, const uint32_t scaleCount, const uint32_t rowNum)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ srcT* scaleUb = (__ubuf__ srcT*)scaleTensor.GetPhyAddr();
    __ubuf__ srcT* offsetUb = (__ubuf__ srcT*)offsetTensor.GetPhyAddr();
    QuantPerchannelForFp8VF<dstT, srcT>(dstUb, srcUb, scaleUb, offsetUb, scaleCount, rowNum);
}

template <typename dstT, typename srcT>
__simd_vf__ inline void QuantPerchannelForB8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ srcT* scaleUb, __ubuf__ srcT* offsetUb,
    const uint32_t scaleCount, const uint32_t rowNum)
{
    Reg::MaskReg preg;
    Reg::RegTensor<bfloat16_t> b16vreg;
    Reg::RegTensor<half> f16Vreg;
    Reg::RegTensor<dstT> s8vreg;
    Reg::RegTensor<half> scaleVreg;
    Reg::RegTensor<half> offsetVreg;
    Reg::RegTensor<bfloat16_t> scaleBfVreg;
    Reg::RegTensor<bfloat16_t> offsetB16Vreg;

    uint32_t sregLower = (uint32_t)ASCENDC_QUANT_B16_VF_LEN;

    for (uint16_t i = 0; i < (uint16_t)rowNum; ++i) {
        uint32_t sreg = (uint32_t)scaleCount;
        uint16_t repeat = CeilDivision(scaleCount, sregLower);
        for (uint16_t j = 0; j < (uint16_t)repeat; ++j) {
            preg = Reg::UpdateMask<uint16_t>(sreg);
            uint32_t srcOffset = i * scaleCount + j * sregLower;

            if constexpr (SupportType<srcT, bfloat16_t>()) {
                Reg::LoadAlign<bfloat16_t, Reg::LoadDist::DIST_NORM>(b16vreg, srcUb + srcOffset);
                Reg::LoadAlign<bfloat16_t, Reg::LoadDist::DIST_NORM>(offsetB16Vreg, offsetUb + j * sregLower);
                Reg::LoadAlign<bfloat16_t, Reg::LoadDist::DIST_NORM>(scaleBfVreg, scaleUb + j * sregLower);
                Reg::Cast<half, bfloat16_t, MrgZRndRSatS>(f16Vreg, b16vreg, preg);
                Reg::Cast<half, bfloat16_t, MrgZRndRSatS>(offsetVreg, offsetB16Vreg, preg);
                Reg::Cast<half, bfloat16_t, MrgZRndRSatS>(scaleVreg, scaleBfVreg, preg);
            } else { // half
                Reg::LoadAlign<half, Reg::LoadDist::DIST_NORM>(f16Vreg, srcUb + srcOffset);
                Reg::LoadAlign<half, Reg::LoadDist::DIST_NORM>(offsetVreg, offsetUb + j * sregLower);
                Reg::LoadAlign<half, Reg::LoadDist::DIST_NORM>(scaleVreg, scaleUb + j * sregLower);
            }

            Reg::Mul<half, Reg::MaskMergeMode::ZEROING>(f16Vreg, f16Vreg, scaleVreg, preg);
            Reg::Add<half, Reg::MaskMergeMode::ZEROING>(f16Vreg, f16Vreg, offsetVreg, preg);

            if constexpr (SupportType<dstT, int8_t>()) {
                Reg::Cast<dstT, half, LayoutZMrgZRndRSatS>(s8vreg, f16Vreg, preg);
            } else {
                Reg::Cast<dstT, half, LayoutZMrgZRndASatS>(s8vreg, f16Vreg, preg);
            }

            Reg::StoreAlign<dstT, Reg::StoreDist::DIST_PACK_B16>(dstUb + srcOffset, s8vreg, preg);
        }
    }
}

template <typename dstT, typename srcT>
__aicore__ inline void QuantPerchannelForB8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<srcT>& scaleTensor,
    const LocalTensor<srcT>& offsetTensor, const uint32_t scaleCount, const uint32_t rowNum)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ srcT* scaleUb = (__ubuf__ srcT*)scaleTensor.GetPhyAddr();
    __ubuf__ srcT* offsetUb = (__ubuf__ srcT*)offsetTensor.GetPhyAddr();
    QuantPerchannelForB8VF<dstT, srcT>(dstUb, srcUb, scaleUb, offsetUb, scaleCount, rowNum);
}

template <typename dstT, typename srcT>
__simd_vf__ inline void QuantPerchannelForB8VF(
    __ubuf__ dstT* dstUb, __ubuf__ float* srcUb, __ubuf__ float* scaleUb, __ubuf__ float* offsetUb,
    const uint32_t scaleCount, const uint32_t rowNum)
{
    Reg::MaskReg preg;
    Reg::RegTensor<float> f32vreg;
    Reg::RegTensor<half> f16vreg;
    Reg::RegTensor<dstT> b8vreg;
    Reg::RegTensor<half> scalevreg;
    Reg::RegTensor<half> offsetvreg;
    Reg::RegTensor<float> scaleB32Vreg;
    Reg::RegTensor<float> offsetB32Vreg;

    uint32_t sregLower = (uint32_t)ASCENDC_QUANT_B32_VF_LEN;

    for (uint16_t i = 0; i < (uint16_t)rowNum; ++i) {
        uint32_t sreg = (uint32_t)scaleCount;
        uint16_t repeat = CeilDivision(scaleCount, sregLower);
        for (uint16_t j = 0; j < (uint16_t)repeat; ++j) {
            preg = Reg::UpdateMask<uint32_t>(sreg);

            Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(f32vreg, srcUb + i * scaleCount + j * sregLower);
            Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(offsetB32Vreg, offsetUb + j * sregLower);
            Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(scaleB32Vreg, scaleUb + j * sregLower);

            Reg::Cast<half, float, LayoutZMrgZRndRSatS>(f16vreg, f32vreg, preg);
            Reg::Cast<half, float, LayoutZMrgZRndRSatS>(offsetvreg, offsetB32Vreg, preg);
            Reg::Cast<half, float, LayoutZMrgZRndRSatS>(scalevreg, scaleB32Vreg, preg);

            Reg::Mul<half, Reg::MaskMergeMode::ZEROING>(f16vreg, f16vreg, scalevreg, preg);
            Reg::Add<half, Reg::MaskMergeMode::ZEROING>(f16vreg, f16vreg, offsetvreg, preg);

            if constexpr (SupportType<dstT, int8_t>()) {
                Reg::Cast<dstT, half, LayoutZMrgZRndRSatS>(b8vreg, f16vreg, preg);
            } else {
                Reg::Cast<dstT, half, LayoutZMrgZRndASatS>(b8vreg, f16vreg, preg);
            }
            Reg::StoreAlign<dstT, Reg::StoreDist::DIST_PACK4_B32>(dstUb + i * scaleCount + j * sregLower, b8vreg, preg);
        }
    }
}

template <typename dstT, typename srcT>
__aicore__ inline void QuantPerchannelForB8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<float>& srcTensor, const LocalTensor<float>& scaleTensor,
    const LocalTensor<float>& offsetTensor, const uint32_t scaleCount, const uint32_t rowNum)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ float* srcUb = (__ubuf__ float*)srcTensor.GetPhyAddr();
    __ubuf__ float* scaleUb = (__ubuf__ float*)scaleTensor.GetPhyAddr();
    __ubuf__ float* offsetUb = (__ubuf__ float*)offsetTensor.GetPhyAddr();
    QuantPerchannelForB8VF<dstT, srcT>(dstUb, srcUb, scaleUb, offsetUb, scaleCount, rowNum);
}

template <typename dstT, typename srcT>
__simd_vf__ inline void QuantPerchannelForB8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ srcT* scaleUb, const srcT offset, const uint32_t scaleCount,
    const uint32_t rowNum)
{
    Reg::MaskReg preg;
    Reg::RegTensor<bfloat16_t> b16vreg;
    Reg::RegTensor<half> f16Vreg;
    Reg::RegTensor<dstT> s8vreg;
    Reg::RegTensor<half> scaleVreg;
    Reg::RegTensor<bfloat16_t> scaleB16Vreg;
    uint32_t sregLower = (uint32_t)ASCENDC_QUANT_B16_VF_LEN;

    for (uint16_t i = 0; i < (uint16_t)rowNum; ++i) {
        uint32_t sreg = (uint32_t)scaleCount;
        uint16_t repeat = CeilDivision(scaleCount, sregLower);
        for (uint16_t j = 0; j < (uint16_t)repeat; ++j) {
            preg = Reg::UpdateMask<uint16_t>(sreg);

            if constexpr (SupportType<srcT, bfloat16_t>()) {
                Reg::LoadAlign<bfloat16_t, Reg::LoadDist::DIST_NORM>(b16vreg, srcUb + i * scaleCount + j * sregLower);
                Reg::LoadAlign<bfloat16_t, Reg::LoadDist::DIST_NORM>(scaleB16Vreg, scaleUb + j * sregLower);
                Reg::Cast<half, bfloat16_t, MrgZRndRSatS>(f16Vreg, b16vreg, preg);
                Reg::Cast<half, bfloat16_t, MrgZRndRSatS>(scaleVreg, scaleB16Vreg, preg);
                Reg::Mul<half, Reg::MaskMergeMode::ZEROING>(f16Vreg, f16Vreg, scaleVreg, preg);
                Reg::Adds<half, half, Reg::MaskMergeMode::ZEROING>(
                    f16Vreg, f16Vreg, static_cast<half>(ToFloat(offset)), preg);
            } else { // half
                Reg::LoadAlign<half, Reg::LoadDist::DIST_NORM>(f16Vreg, srcUb + i * scaleCount + j * sregLower);
                Reg::LoadAlign<half, Reg::LoadDist::DIST_NORM>(scaleVreg, scaleUb + j * sregLower);
                Reg::Mul<half, Reg::MaskMergeMode::ZEROING>(f16Vreg, f16Vreg, scaleVreg, preg);
                Reg::Adds<half, half, Reg::MaskMergeMode::ZEROING>(f16Vreg, f16Vreg, offset, preg);
            }

            if constexpr (SupportType<dstT, int8_t>()) {
                Reg::Cast<dstT, half, LayoutZMrgZRndRSatS>(s8vreg, f16Vreg, preg);
            } else {
                Reg::Cast<dstT, half, LayoutZMrgZRndASatS>(s8vreg, f16Vreg, preg);
            }

            Reg::StoreAlign<dstT, Reg::StoreDist::DIST_PACK_B16>(dstUb + i * scaleCount + j * sregLower, s8vreg, preg);
        }
    }
}

template <typename dstT, typename srcT>
__aicore__ inline void QuantPerchannelForB8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<srcT>& scaleTensor,
    const srcT offset, const uint32_t scaleCount, const uint32_t rowNum)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ srcT* scaleUb = (__ubuf__ srcT*)scaleTensor.GetPhyAddr();
    QuantPerchannelForB8VF<dstT, srcT>(dstUb, srcUb, scaleUb, offset, scaleCount, rowNum);
}

template <typename dstT, typename srcT>
__simd_vf__ inline void QuantPerchannelForB8VF(
    __ubuf__ dstT* dstUb, __ubuf__ float* srcUb, __ubuf__ float* scaleUb, const float offset, const uint32_t scaleCount,
    const uint32_t rowNum)
{
    Reg::MaskReg preg;
    Reg::RegTensor<float> f32vreg;
    Reg::RegTensor<half> f16Vreg;
    Reg::RegTensor<dstT> b8vreg;
    Reg::RegTensor<half> scaleVreg;
    Reg::RegTensor<float> scaleB32Vreg;

    uint32_t sregLower = (uint32_t)ASCENDC_QUANT_B32_VF_LEN;

    for (uint16_t i = 0; i < (uint16_t)rowNum; ++i) {
        uint32_t sreg = (uint32_t)scaleCount;
        uint16_t repeat = CeilDivision(scaleCount, sregLower);
        for (uint16_t j = 0; j < (uint16_t)repeat; ++j) {
            preg = Reg::UpdateMask<uint32_t>(sreg);

            Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(f32vreg, srcUb + i * scaleCount + j * sregLower);
            Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(scaleB32Vreg, scaleUb + j * sregLower);

            Reg::Cast<half, float, LayoutZMrgZRndRSatS>(f16Vreg, f32vreg, preg);
            Reg::Cast<half, float, LayoutZMrgZRndRSatS>(scaleVreg, scaleB32Vreg, preg);

            Reg::Mul<half, Reg::MaskMergeMode::ZEROING>(f16Vreg, f16Vreg, scaleVreg, preg);
            Reg::Adds<half, half, Reg::MaskMergeMode::ZEROING>(f16Vreg, f16Vreg, static_cast<half>(offset), preg);

            if constexpr (SupportType<dstT, int8_t>()) {
                Reg::Cast<dstT, half, LayoutZMrgZRndRSatS>(b8vreg, f16Vreg, preg);
            } else {
                Reg::Cast<dstT, half, LayoutZMrgZRndASatS>(b8vreg, f16Vreg, preg);
            }

            Reg::StoreAlign<dstT, Reg::StoreDist::DIST_PACK4_B32>(dstUb + i * scaleCount + j * sregLower, b8vreg, preg);
        }
    }
}

template <typename dstT, typename srcT>
__aicore__ inline void QuantPerchannelForB8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<float>& srcTensor, const LocalTensor<float>& scaleTensor,
    const float offset, const uint32_t scaleCount, const uint32_t rowNum)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ float* srcUb = (__ubuf__ float*)srcTensor.GetPhyAddr();
    __ubuf__ float* scaleUb = (__ubuf__ float*)scaleTensor.GetPhyAddr();
    QuantPerchannelForB8VF<dstT, srcT>(dstUb, srcUb, scaleUb, offset, scaleCount, rowNum);
}

template <typename dstT, typename srcT>
__simd_vf__ inline void QuantPerchannelForFp8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ srcT* scaleUb, const srcT offset, const uint32_t scaleCount,
    const uint32_t rowNum)
{
    Reg::MaskReg preg;
    Reg::RegTensor<float> f32vreg;
    Reg::RegTensor<float> scalef32vreg;
    Reg::RegTensor<srcT> b16vreg;
    Reg::RegTensor<srcT> scaleB16Vreg;
    Reg::RegTensor<dstT> b8vreg;

    uint32_t sregLower = (uint32_t)ASCENDC_QUANT_B32_VF_LEN;

    for (uint16_t i = 0; i < (uint16_t)rowNum; ++i) {
        uint32_t sreg = (uint32_t)scaleCount;
        uint16_t repeat = CeilDivision(scaleCount, sregLower);
        for (uint16_t j = 0; j < (uint16_t)repeat; ++j) {
            preg = Reg::UpdateMask<uint32_t>(sreg);
            if constexpr (SupportType<srcT, half, bfloat16_t>()) {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(b16vreg, srcUb + i * scaleCount + j * sregLower);
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(scaleB16Vreg, scaleUb + j * sregLower);
                Reg::Cast<float, srcT, layoutZMrgZ>(f32vreg, b16vreg, preg);
                Reg::Cast<float, srcT, layoutZMrgZ>(scalef32vreg, scaleB16Vreg, preg);
            } else {
                Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(f32vreg, srcUb + i * scaleCount + j * sregLower);
                Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(scalef32vreg, scaleUb + j * sregLower);
            }

            Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32vreg, f32vreg, scalef32vreg, preg);
            if constexpr (SupportType<srcT, bfloat16_t>()) {
                Reg::Adds<float, float, Reg::MaskMergeMode::ZEROING>(f32vreg, f32vreg, ToFloat(offset), preg);
            } else {
                Reg::Adds<float, float, Reg::MaskMergeMode::ZEROING>(
                    f32vreg, f32vreg, static_cast<float>(offset), preg);
            }

            Reg::Cast<dstT, float, LayoutZMrgZRndRSatS>(b8vreg, f32vreg, preg);
            Reg::StoreAlign<dstT, Reg::StoreDist::DIST_PACK4_B32>(dstUb + i * scaleCount + j * sregLower, b8vreg, preg);
        }
    }
}

template <typename dstT, typename srcT>
__aicore__ inline void QuantPerchannelForFp8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<srcT>& scaleTensor,
    const srcT offset, const uint32_t scaleCount, const uint32_t rowNum)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ srcT* scaleUb = (__ubuf__ srcT*)scaleTensor.GetPhyAddr();
    QuantPerchannelForFp8VF<dstT, srcT>(dstUb, srcUb, scaleUb, offset, scaleCount, rowNum);
}

template <typename dstT, typename srcT, bool isReuseSource = false>
__aicore__ inline void AscendQuantImpl(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const LocalTensor<srcT>& scaleTensor, const LocalTensor<srcT>& offsetTensor, const uint32_t scaleCount,
    const uint32_t offsetCount, const uint32_t calCount)
{
    if ASCEND_IS_AIC {
        return;
    }
    CheckTensorPosition(dstTensor, "dstTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcTensor, "srcTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(scaleTensor, "scaleTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(offsetTensor, "offsetTensor", "VECIN/VECOUT/VECCALC");
    static_assert(
        SupportType<srcT, half, float, bfloat16_t>(),
        "This AscendQuant only support half/float/bfloat16_t input dtype");
    static_assert(
        SupportType<dstT, int8_t, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t>(),
        "This AscendQuant only support int8_t/fp8_e4m3fn_t/fp8_e5m2_t/hifloat8_t output dtype");
    ASCENDC_ASSERT((calCount <= srcTensor.GetSize() && calCount <= dstTensor.GetSize() && calCount >= 0), {
        KERNEL_LOG(
            KERNEL_ERROR, "calCount is %u, which should be in [0, min(%u, %u)]", calCount, srcTensor.GetSize(),
            dstTensor.GetSize());
    });
    ASCENDC_ASSERT((scaleCount > 0 && scaleCount == offsetCount), {
        KERNEL_LOG(KERNEL_ERROR, "scaleCount must be greater than 0 and equal to offsetCount!");
    });
    ASCENDC_ASSERT((calCount % 32 == 0 && calCount % scaleCount == 0), {
        KERNEL_LOG(KERNEL_ERROR, "calCount must be an integer multiple of 32 and scaleCount!");
    });
    ASCENDC_ASSERT((scaleCount == offsetCount), { KERNEL_LOG(KERNEL_ERROR, "scaleCount equal to offsetCount!"); });
    const uint32_t rowNum = calCount / scaleCount;
    if constexpr (SupportType<dstT, fp8_e4m3fn_t, fp8_e5m2_t>()) {
        QuantPerchannelForFp8<dstT, srcT>(dstTensor, srcTensor, scaleTensor, offsetTensor, scaleCount, rowNum);
    } else {
        QuantPerchannelForB8<dstT, srcT>(
            dstTensor, srcTensor, scaleTensor, offsetTensor, scaleCount,
            rowNum); // for int8/hif8 output
    }
}

template <typename dstT, typename srcT, bool isReuseSource = false>
__aicore__ inline void AscendQuantImpl(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const LocalTensor<srcT>& scaleTensor, const srcT offset, const uint32_t scaleCount, const uint32_t calCount)
{
    if ASCEND_IS_AIC {
        return;
    }
    CheckTensorPosition(dstTensor, "dstTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcTensor, "srcTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(scaleTensor, "scaleTensor", "VECIN/VECOUT/VECCALC");
    static_assert(
        SupportType<srcT, half, float, bfloat16_t>(),
        "This AscendQuant only support half/float/bfloat16_t input dtype");
    static_assert(
        SupportType<dstT, int8_t, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t>(),
        "This AscendQuant only support int8_t/fp8_e4m3fn_t/fp8_e5m2_t/hifloat8_t output dtype");
    ASCENDC_ASSERT((calCount <= srcTensor.GetSize() && calCount <= dstTensor.GetSize() && calCount >= 0), {
        KERNEL_LOG(
            KERNEL_ERROR, "calCount is %u, which should be in [0, min(%u, %u)]", calCount, srcTensor.GetSize(),
            dstTensor.GetSize());
    });
    ASCENDC_ASSERT((scaleCount > 0), { KERNEL_LOG(KERNEL_ERROR, "scaleCount must be greater than 0"); });
    ASCENDC_ASSERT((calCount % 32 == 0 && calCount % scaleCount == 0), {
        KERNEL_LOG(KERNEL_ERROR, "calCount must be an integer multiple of 32 and scaleCount!");
    });
    const uint32_t rowNum = calCount / scaleCount;
    if constexpr (SupportType<dstT, fp8_e4m3fn_t, fp8_e5m2_t>()) {
        QuantPerchannelForFp8<dstT, srcT>(dstTensor, srcTensor, scaleTensor, offset, scaleCount, rowNum);
    } else {
        QuantPerchannelForB8<dstT, srcT>(
            dstTensor, srcTensor, scaleTensor, offset, scaleCount,
            rowNum); // for int8/hif8 output
    }
}

template <typename T, bool isReuseSource = false, const AscendQuantConfig& config>
__aicore__ inline void AscendQuantImpl(
    const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const LocalTensor<T>& scaleTensor, const T offset, const uint32_t scaleCount, const uint32_t calCount)
{
    if ASCEND_IS_AIC {
        return;
    }
    CheckTensorPosition(dstTensor, "dstTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcTensor, "srcTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(scaleTensor, "scaleTensor", "VECIN/VECOUT/VECCALC");
    static_assert(
        SupportType<T, half, float, bfloat16_t>(), "This AscendQuant only support half/float/bfloat16_t input dtype");

    constexpr bool enableConfig = config.calcCount != 0 && config.scaleCount != 0;
    const uint32_t calCountReal = enableConfig ? config.calcCount : calCount;
    const uint32_t scaleCountReal = enableConfig ? config.scaleCount : scaleCount;

    ASCENDC_ASSERT((calCountReal <= srcTensor.GetSize() && calCountReal <= dstTensor.GetSize() && calCountReal >= 0), {
        KERNEL_LOG(
            KERNEL_ERROR, "calCount is %u, which should be in [0, min(%u, %u)]", calCountReal, srcTensor.GetSize(),
            dstTensor.GetSize());
    });
    ASCENDC_ASSERT((scaleCountReal > 0), { KERNEL_LOG(KERNEL_ERROR, "scaleCount must be greater than 0"); });
    ASCENDC_ASSERT((calCountReal % 32 == 0 && calCountReal % scaleCountReal == 0), {
        KERNEL_LOG(KERNEL_ERROR, "calCount must be an integer multiple of 32 and scaleCount!");
    });
    const uint32_t rowNum = calCountReal / scaleCountReal;
    QuantPerchannelForB8<int8_t, T>(
        dstTensor, srcTensor, scaleTensor, offset, scaleCountReal,
        rowNum); // for int8/hif8 output
}

template <typename T, bool isReuseSource = false, const AscendQuantConfig& config>
__aicore__ inline void AscendQuantImpl(
    const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const LocalTensor<T>& scaleTensor, const LocalTensor<T>& offsetTensor, const uint32_t scaleCount,
    const uint32_t offsetCount, const uint32_t calCount)
{
    if ASCEND_IS_AIC {
        return;
    }
    CheckTensorPosition(dstTensor, "dstTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcTensor, "srcTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(scaleTensor, "scaleTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(offsetTensor, "offsetTensor", "VECIN/VECOUT/VECCALC");
    static_assert(
        SupportType<T, half, float, bfloat16_t>(), "This AscendQuant only support half/float/bfloat16_t input dtype");

    constexpr bool enableConfig = config.calcCount != 0 && config.scaleCount != 0 && config.offsetCount != 0;
    const uint32_t calCountReal = enableConfig ? config.calcCount : calCount;
    const uint32_t scaleCountReal = enableConfig ? config.scaleCount : scaleCount;
    const uint32_t offsetCountReal = enableConfig ? config.offsetCount : offsetCount;

    ASCENDC_ASSERT((calCountReal <= srcTensor.GetSize() && calCountReal <= dstTensor.GetSize() && calCountReal >= 0), {
        KERNEL_LOG(
            KERNEL_ERROR, "calCount is %u, which should be in [0, min(%u, %u)]", calCountReal, srcTensor.GetSize(),
            dstTensor.GetSize());
    });
    ASCENDC_ASSERT((scaleCountReal > 0 && scaleCountReal == offsetCountReal), {
        KERNEL_LOG(KERNEL_ERROR, "scaleCount must be greater than 0 and equal to offsetCount!");
    });
    ASCENDC_ASSERT((calCountReal % 32 == 0 && calCountReal % scaleCountReal == 0), {
        KERNEL_LOG(KERNEL_ERROR, "calCount must be an integer multiple of 32 and scaleCount!");
    });
    const uint32_t rowNum = calCountReal / scaleCountReal;
    QuantPerchannelForB8<int8_t, T>(
        dstTensor, srcTensor, scaleTensor, offsetTensor, scaleCountReal,
        rowNum); // for int8/hif8 output
}

template <typename scaleT>
__aicore__ constexpr inline float ConvertToFloat(const scaleT& offset)
{
    if constexpr (SupportType<scaleT, bfloat16_t>()) {
        return ToFloat(offset);
    }
    return static_cast<float>(offset);
}

template <typename scaleT, const AscendQuantConfig& config>
__simd_callee__ inline void GetPerTokenScaleAndOffset(
    __ubuf__ scaleT* scaleAddr, __ubuf__ scaleT* offsetAddr, Reg::RegTensor<scaleT>& scaleVreg,
    Reg::RegTensor<scaleT>& offsetVreg)
{
    if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
        Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_BRC_B16>(scaleVreg, scaleAddr);
        if constexpr (config.hasOffset) {
            Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_BRC_B16>(offsetVreg, offsetAddr);
        }
    } else {
        Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_BRC_B32>(scaleVreg, scaleAddr);
        if constexpr (config.hasOffset) {
            Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_BRC_B32>(offsetVreg, offsetAddr);
        }
    }
}

template <typename scaleT>
__simd_callee__ inline void GetPerTokenScale(__ubuf__ scaleT* scaleAddr, Reg::RegTensor<scaleT>& scaleVreg)
{
    if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
        Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_BRC_B16>(scaleVreg, scaleAddr);
    } else {
        Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_BRC_B32>(scaleVreg, scaleAddr);
    }
}

template <typename dstT, typename scaleT>
__simd_callee__ inline void StoreRes(__ubuf__ dstT* dstAddr, Reg::RegTensor<dstT>& vreg, Reg::MaskReg& preg)
{
    if (SupportType<scaleT, float>()) {
        Reg::StoreAlign<dstT, Reg::StoreDist::DIST_PACK4_B32>(dstAddr, vreg, preg);
    } else {
        Reg::StoreAlign<dstT, Reg::StoreDist::DIST_PACK_B16>(dstAddr, vreg, preg);
    }
}

template <typename T>
__simd_callee__ inline void GetPerGroupScale(
    __ubuf__ T* scaleUb, const int32_t start, const AscendQuantParam& para, const AscendQuantConfig& config,
    Reg::RegTensor<T>& scaleReg)
{
    // use vgather to get perGroup scale/offset
    uint32_t groupSize = para.groupSize;
    if constexpr (SupportType<T, half, bfloat16_t>()) {
        Reg::MaskReg preg = Reg::CreateMask<uint16_t, Reg::MaskPattern::ALL>();
        Reg::RegTensor<int16_t> vci_vreg;
        Reg::RegTensor<uint16_t> index_vreg;
        Reg::RegTensor<uint16_t> gsize_vreg;
        Reg::Duplicate(gsize_vreg, static_cast<uint16_t>(groupSize));
        Reg::Arange(vci_vreg, static_cast<int16_t>(start));
        Reg::Div(index_vreg, (Reg::RegTensor<uint16_t>&)vci_vreg, gsize_vreg, preg);
        Reg::Gather(scaleReg, scaleUb, index_vreg, preg);
    } else {
        Reg::MaskReg preg = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
        Reg::RegTensor<int32_t> vci_vreg;
        Reg::RegTensor<uint32_t> index_vreg;
        Reg::RegTensor<uint32_t> gsize_vreg;
        Reg::Duplicate(gsize_vreg, static_cast<uint32_t>(groupSize));
        Reg::Arange(vci_vreg, static_cast<int32_t>(start));
        Reg::Div(index_vreg, (Reg::RegTensor<uint32_t>&)vci_vreg, gsize_vreg, preg);
        Reg::Gather(scaleReg, scaleUb, index_vreg, preg);
    }
}

template <typename T>
__simd_callee__ inline void GetPerGroupOffset(
    __ubuf__ T* offsetUb, const int32_t start, const AscendQuantParam& para, const AscendQuantConfig& config,
    Reg::RegTensor<T>& offsetReg)
{
    // use vgather to get perGroup scale/offset
    uint32_t groupSize = para.groupSize;
    if constexpr (SupportType<T, half, bfloat16_t>()) {
        Reg::MaskReg preg = Reg::CreateMask<uint16_t, Reg::MaskPattern::ALL>();
        Reg::RegTensor<int16_t> vci_vreg;
        Reg::RegTensor<uint16_t> index_vreg;
        Reg::RegTensor<uint16_t> gsize_vreg;
        Reg::Duplicate(gsize_vreg, static_cast<uint16_t>(groupSize));
        Reg::Arange(vci_vreg, static_cast<int16_t>(start));
        Reg::Div(index_vreg, (Reg::RegTensor<uint16_t>&)vci_vreg, gsize_vreg, preg);
        Reg::Gather(offsetReg, offsetUb, index_vreg, preg);
    } else {
        Reg::MaskReg preg = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
        Reg::RegTensor<int32_t> vci_vreg;
        Reg::RegTensor<uint32_t> index_vreg;
        Reg::RegTensor<uint32_t> gsize_vreg;
        Reg::Duplicate(gsize_vreg, static_cast<uint32_t>(groupSize));
        Reg::Arange(vci_vreg, static_cast<int32_t>(start));
        Reg::Div(index_vreg, (Reg::RegTensor<uint32_t>&)vci_vreg, gsize_vreg, preg);
        Reg::Gather(offsetReg, offsetUb, index_vreg, preg);
    }
}

template <typename scaleT>
__simd_callee__ inline void GenerateZeroVreg(Reg::RegTensor<scaleT>& zeroVreg)
{
    if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
        Reg::MaskReg b16FullPreg = Reg::CreateMask<uint16_t, Reg::MaskPattern::ALL>();
        Reg::Duplicate(zeroVreg, static_cast<scaleT>(0), b16FullPreg);
    } else {
        Reg::MaskReg b32FullPreg = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
        Reg::Duplicate(zeroVreg, static_cast<scaleT>(0), b32FullPreg);
    }
}

template <typename scaleT, const AscendQuantConfig& config>
__simd_callee__ inline void GetPerGroupScaleEntry(
    __ubuf__ scaleT* scaleAddr, const AscendQuantParam& para, int32_t start, Reg::MaskReg& preg,
    Reg::RegTensor<float>& f32ScaleVreg)
{
    Reg::RegTensor<scaleT> zeroVreg;
    GenerateZeroVreg<scaleT>(zeroVreg);
    if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
        Reg::RegTensor<scaleT> oriScaleVreg;
        Reg::RegTensor<scaleT> tempVreg;
        Reg::RegTensor<scaleT> scaleVreg;
        GetPerGroupScale(scaleAddr, start, para, config, oriScaleVreg);
        Reg::Interleave(scaleVreg, tempVreg, oriScaleVreg, zeroVreg);
        Reg::Cast<float, scaleT, layoutZMrgZ>(f32ScaleVreg, scaleVreg, preg);
    } else {
        GetPerGroupScale(scaleAddr, start, para, config, f32ScaleVreg);
    }
}

template <typename scaleT, const AscendQuantConfig& config>
__aicore__ inline void GetPerGroupOffsetEntry(
    __ubuf__ scaleT* offsetAddr, const AscendQuantParam& para, int32_t start, Reg::MaskReg& preg,
    Reg::RegTensor<float>& f32OffsetVreg)
{
    Reg::RegTensor<scaleT> zeroVreg;
    GenerateZeroVreg<scaleT>(zeroVreg);
    if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
        Reg::RegTensor<scaleT> oriOffsetVreg;
        Reg::RegTensor<scaleT> tempVreg;
        Reg::RegTensor<scaleT> offsetVreg;
        if constexpr (config.hasOffset) {
            GetPerGroupOffset(offsetAddr, start, para, config, oriOffsetVreg);
            Reg::Interleave(offsetVreg, tempVreg, oriOffsetVreg, zeroVreg);
            Reg::Cast<float, scaleT, layoutZMrgZ>(f32OffsetVreg, offsetVreg, preg);
        }
    } else {
        if constexpr (config.hasOffset) {
            GetPerGroupOffset(offsetAddr, start, para, config, f32OffsetVreg);
        }
    }
}

template <typename scaleT>
__simd_callee__ inline void GetPerGroupKRowScaleEntry(__ubuf__ scaleT* scaleAddr, Reg::RegTensor<float>& f32ScaleVreg)
{
    Reg::MaskReg b32FullPreg = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    Reg::RegTensor<scaleT> tempVreg;
    if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
        Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_UNPACK_B16>(tempVreg, scaleAddr);
        Reg::Cast<float, scaleT, layoutZMrgZ>(f32ScaleVreg, tempVreg, b32FullPreg);
    } else {
        Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_NORM>(f32ScaleVreg, scaleAddr);
    }
}

template <typename scaleT, const AscendQuantConfig& config>
__simd_callee__ inline void GetPerGroupKRowOffsetEntry(
    __ubuf__ scaleT* offsetAddr, Reg::RegTensor<float>& f32OffsetVreg)
{
    Reg::MaskReg b32FullPreg = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    Reg::RegTensor<scaleT> tempVreg;
    if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
        if constexpr (config.hasOffset) {
            Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_UNPACK_B16>(tempVreg, offsetAddr);
            Reg::Cast<float, scaleT, layoutZMrgZ>(f32OffsetVreg, tempVreg, b32FullPreg);
        }
    } else {
        if constexpr (config.hasOffset) {
            Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_NORM>(f32OffsetVreg, offsetAddr);
        }
    }
}

template <typename dstT, typename scaleT, const Reg::CastTrait& castTrait>
__simd_callee__ inline void TransRegForFp8(
    Reg::RegTensor<scaleT>& srcVreg, Reg::RegTensor<dstT>& dstVreg, Reg::MaskReg& preg)
{
    if constexpr (castTrait.roundMode == RoundMode::CAST_RINT) {
        Reg::Cast<dstT, scaleT, castTrait>(dstVreg, srcVreg, preg);
    } else {
        Reg::Cast<dstT, scaleT, LayoutZMrgZRndRSatS>(dstVreg, srcVreg, preg);
    }
}

template <typename dstT, typename scaleT, const Reg::CastTrait& castTrait>
__simd_callee__ inline void TransRegForHif8(
    Reg::RegTensor<scaleT>& srcVreg, Reg::RegTensor<dstT>& dstVreg, Reg::MaskReg& preg)
{
    if constexpr (SupportType<scaleT, bfloat16_t>()) {
        // bf16->fp32->hif8
        Reg::MaskReg preg1;
        Reg::MaskReg preg2 = Reg::CreateMask<scaleT, Reg::MaskPattern::ALLF>();
        Reg::RegTensor<float> f32Vreg;
        Reg::RegTensor<float> f32Vreg2;
        Reg::RegTensor<scaleT> srcVreg2;
        Reg::RegTensor<dstT> dstVreg2;
        Reg::MaskInterleave<scaleT>(preg1, preg2, preg, preg2);
        Reg::Interleave(srcVreg, srcVreg2, srcVreg, srcVreg2);
        Reg::Cast<float, scaleT, layoutZMrgZ>(f32Vreg, srcVreg, preg1);
        Reg::Cast<float, scaleT, layoutZMrgZ>(f32Vreg2, srcVreg2, preg2);
        if constexpr (castTrait.roundMode == RoundMode::CAST_ROUND || castTrait.roundMode == RoundMode::CAST_HYBRID) {
            Reg::Cast<dstT, float, castTrait>(dstVreg, f32Vreg, preg1);
            Reg::Cast<dstT, float, castTrait>(dstVreg2, f32Vreg2, preg2);
        } else {
            Reg::Cast<dstT, float, LayoutZMrgZRndASatS>(dstVreg, f32Vreg, preg1);
            Reg::Cast<dstT, float, LayoutZMrgZRndASatS>(dstVreg2, f32Vreg2, preg2);
        }
        Reg::DeInterleave(
            (Reg::RegTensor<scaleT>&)dstVreg, (Reg::RegTensor<scaleT>&)dstVreg2, (Reg::RegTensor<scaleT>&)dstVreg,
            (Reg::RegTensor<scaleT>&)dstVreg2);
    } else if constexpr (SupportType<scaleT, half, float>()) {
        if constexpr (castTrait.roundMode == RoundMode::CAST_ROUND || castTrait.roundMode == RoundMode::CAST_HYBRID) {
            Reg::Cast<dstT, scaleT, castTrait>(dstVreg, srcVreg, preg);
        } else {
            Reg::Cast<dstT, scaleT, LayoutZMrgZRndASatS>(dstVreg, srcVreg, preg);
        }
    }
}

template <typename dstT, typename scaleT, const Reg::CastTrait& castTrait>
__simd_callee__ inline void TransRegForS8(
    Reg::RegTensor<scaleT>& srcVreg, Reg::RegTensor<dstT>& dstVreg, Reg::MaskReg& preg)
{
    if constexpr (SupportType<scaleT, bfloat16_t>()) {
        // bf16->fp32->s16->fp16->s8
        Reg::MaskReg preg1;
        Reg::MaskReg preg2 = Reg::CreateMask<scaleT, Reg::MaskPattern::ALLF>();
        Reg::RegTensor<float> f32Vreg;
        Reg::RegTensor<float> f32Vreg2;
        Reg::RegTensor<scaleT> srcVreg2;
        Reg::RegTensor<dstT> dstVreg2;
        Reg::MaskInterleave<scaleT>(preg1, preg2, preg, preg2);
        Reg::Interleave(srcVreg, srcVreg2, srcVreg, srcVreg2);
        Reg::Cast<float, scaleT, layoutZMrgZ>(f32Vreg, srcVreg, preg1);
        Reg::Cast<float, scaleT, layoutZMrgZ>(f32Vreg2, srcVreg2, preg2);
        if constexpr (
            castTrait.roundMode == RoundMode::CAST_RINT || castTrait.roundMode == RoundMode::CAST_ROUND ||
            castTrait.roundMode == RoundMode::CAST_CEIL || castTrait.roundMode == RoundMode::CAST_FLOOR ||
            castTrait.roundMode == RoundMode::CAST_TRUNC) {
            Reg::Cast<int16_t, float, castTrait>((Reg::RegTensor<int16_t>&)f32Vreg, f32Vreg, preg1);
            Reg::Cast<int16_t, float, castTrait>((Reg::RegTensor<int16_t>&)f32Vreg2, f32Vreg2, preg2);
        } else {
            Reg::Cast<int16_t, float, LayoutZMrgZRndRSatS>((Reg::RegTensor<int16_t>&)f32Vreg, f32Vreg, preg1);
            Reg::Cast<int16_t, float, LayoutZMrgZRndRSatS>((Reg::RegTensor<int16_t>&)f32Vreg2, f32Vreg2, preg2);
        }
        Reg::Cast<half, int16_t, LayoutZMrgZRndRSatS>(
            (Reg::RegTensor<half>&)f32Vreg, (Reg::RegTensor<int16_t>&)f32Vreg, preg1);
        Reg::Cast<half, int16_t, LayoutZMrgZRndRSatS>(
            (Reg::RegTensor<half>&)f32Vreg2, (Reg::RegTensor<int16_t>&)f32Vreg2, preg2);
        Reg::Cast<dstT, half, LayoutZMrgZRndRSatS>(dstVreg, (Reg::RegTensor<half>&)f32Vreg, preg1);
        Reg::Cast<dstT, half, LayoutZMrgZRndRSatS>(dstVreg2, (Reg::RegTensor<half>&)f32Vreg2, preg2);
        Reg::DeInterleave(
            (Reg::RegTensor<scaleT>&)dstVreg, (Reg::RegTensor<scaleT>&)dstVreg2, (Reg::RegTensor<scaleT>&)dstVreg,
            (Reg::RegTensor<scaleT>&)dstVreg2);
    } else if constexpr (SupportType<scaleT, float>()) {
        // fp32->s16->fp16->s8
        Reg::RegTensor<half> f16Vreg;
        if constexpr (
            castTrait.roundMode == RoundMode::CAST_RINT || castTrait.roundMode == RoundMode::CAST_ROUND ||
            castTrait.roundMode == RoundMode::CAST_CEIL || castTrait.roundMode == RoundMode::CAST_FLOOR ||
            castTrait.roundMode == RoundMode::CAST_TRUNC) {
            Reg::Cast<int16_t, scaleT, castTrait>((Reg::RegTensor<int16_t>&)f16Vreg, srcVreg, preg);
        } else {
            Reg::Cast<int16_t, scaleT, LayoutZMrgZRndRSatS>((Reg::RegTensor<int16_t>&)f16Vreg, srcVreg, preg);
        }
        Reg::Cast<half, int16_t, LayoutZMrgZRndRSatS>(f16Vreg, (Reg::RegTensor<int16_t>&)f16Vreg, preg);
        Reg::Cast<dstT, half, LayoutZMrgZRndRSatS>(dstVreg, f16Vreg, preg);
    } else if constexpr (SupportType<scaleT, half>()) {
        if constexpr (
            castTrait.roundMode == RoundMode::CAST_RINT || castTrait.roundMode == RoundMode::CAST_ROUND ||
            castTrait.roundMode == RoundMode::CAST_CEIL || castTrait.roundMode == RoundMode::CAST_FLOOR ||
            castTrait.roundMode == RoundMode::CAST_TRUNC) {
            Reg::Cast<dstT, scaleT, castTrait>(dstVreg, srcVreg, preg);
        } else {
            Reg::Cast<dstT, scaleT, LayoutZMrgZRndRSatS>(dstVreg, srcVreg, preg);
        }
    }
}

template <typename dstT, typename scaleT, const Reg::CastTrait& castTrait>
__simd_callee__ inline void TransRegForFp4(
    Reg::RegTensor<scaleT>& vreg, Reg::RegTensor<dstT>& dstVreg, Reg::MaskReg& preg)
{
    Reg::RegTensor<bfloat16_t> bf16Vreg;
    if constexpr (SupportType<scaleT, float>()) {
        Reg::Cast<bfloat16_t, scaleT, LayoutZMrgZRndRSatS>(bf16Vreg, vreg, preg);
        Reg::Pack<uint16_t, uint32_t, Reg::HighLowPart::LOWEST>(
            (Reg::RegTensor<uint16_t>&)bf16Vreg, (Reg::RegTensor<uint32_t>&)bf16Vreg);
        Reg::MaskPack(preg, preg);
        if constexpr (
            castTrait.roundMode == RoundMode::CAST_RINT || castTrait.roundMode == RoundMode::CAST_ROUND ||
            castTrait.roundMode == RoundMode::CAST_CEIL || castTrait.roundMode == RoundMode::CAST_FLOOR ||
            castTrait.roundMode == RoundMode::CAST_TRUNC) {
            Reg::Cast<dstT, bfloat16_t, castTrait>(dstVreg, bf16Vreg, preg);
        } else {
            Reg::Cast<dstT, bfloat16_t, LayoutZMrgZRndRSatS>(dstVreg, bf16Vreg, preg);
        }
    } else if constexpr (SupportType<scaleT, half>()) {
        Reg::Cast<bfloat16_t, scaleT, LayoutZMrgZRndRSatS>(bf16Vreg, vreg, preg);
        if constexpr (
            castTrait.roundMode == RoundMode::CAST_RINT || castTrait.roundMode == RoundMode::CAST_ROUND ||
            castTrait.roundMode == RoundMode::CAST_CEIL || castTrait.roundMode == RoundMode::CAST_FLOOR ||
            castTrait.roundMode == RoundMode::CAST_TRUNC) {
            Reg::Cast<dstT, bfloat16_t, castTrait>(dstVreg, bf16Vreg, preg);
        } else {
            Reg::Cast<dstT, bfloat16_t, LayoutZMrgZRndRSatS>(dstVreg, bf16Vreg, preg);
        }
    } else if constexpr (SupportType<scaleT, bfloat16_t>()) {
        if constexpr (
            castTrait.roundMode == RoundMode::CAST_RINT || castTrait.roundMode == RoundMode::CAST_ROUND ||
            castTrait.roundMode == RoundMode::CAST_CEIL || castTrait.roundMode == RoundMode::CAST_FLOOR ||
            castTrait.roundMode == RoundMode::CAST_TRUNC) {
            Reg::Cast<dstT, bfloat16_t, castTrait>(dstVreg, vreg, preg);
        } else {
            Reg::Cast<dstT, bfloat16_t, LayoutZMrgZRndRSatS>(dstVreg, vreg, preg);
        }
    }
}

template <typename scaleT, const AscendQuantConfig& config>
__simd_callee__ inline void LoadContinuousScaleAndOffset(
    __ubuf__ scaleT* scaleAddr, __ubuf__ scaleT* offsetAddr, Reg::RegTensor<scaleT>& scaleVreg,
    Reg::RegTensor<scaleT>& offsetVreg)
{
    Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_NORM>(scaleVreg, scaleAddr);
    if constexpr (config.hasOffset) {
        Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_NORM>(offsetVreg, offsetAddr);
    }
}

template <typename srcT>
__simd_callee__ inline void LoadSrc(__ubuf__ srcT* srcAddr, Reg::MaskReg& preg, Reg::RegTensor<float>& vreg)
{
    if constexpr (SupportType<srcT, half, bfloat16_t>()) {
        Reg::RegTensor<srcT> srcVreg;
        Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(srcVreg, srcAddr);
        Reg::Cast<float, srcT, layoutZMrgZ>(vreg, srcVreg, preg);
    } else {
        Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(vreg, srcAddr);
    }
}

template <typename scaleT, const AscendQuantConfig& config>
__simd_callee__ inline void AddQuantOffsetIfExist(
    Reg::RegTensor<float>& vreg, Reg::RegTensor<float>& offsetVreg, Reg::MaskReg& preg)
{
    if constexpr (config.hasOffset) {
        Reg::Add<scaleT, Reg::MaskMergeMode::ZEROING>(vreg, vreg, offsetVreg, preg);
    }
}
} //  namespace AscendC
#endif // LIB_ASCEND_QUANT_ASCEND_QUANT_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_QUANT_ASCEND_QUANT_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_QUANT_ASCEND_QUANT_C310_IMPL_H__
#endif
