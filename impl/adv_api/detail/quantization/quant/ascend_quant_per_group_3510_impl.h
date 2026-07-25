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
    "impl/adv_api/detail/quantization/quant/ascend_quant_per_group_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/quantization/ascend_quant.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_QUANT_ASCEND_QUANT_PER_GROUP_C310_IMPL_H__
#endif

#ifndef LIB_ASCEND_QUANT_ASCEND_QUANT_PER_GROUP_C310_IMPL_H
#define LIB_ASCEND_QUANT_ASCEND_QUANT_PER_GROUP_C310_IMPL_H
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "kernel_tiling/kernel_tiling.h"
#include "../../../../../include/adv_api/quantization/ascend_quant_utils.h"
#include "../../common/check.h"

namespace AscendC {
constexpr uint32_t ASCENDC_QUANT_PER_GROUP_B32_VF_LEN = GetVecLen() / sizeof(uint32_t);

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__simd_vf__ inline void QuantPerTokenForFp8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, __ubuf__ scaleT* offsetUb,
    const AscendQuantParam para)
{
    uint16_t rowNum = para.calCount / para.n;
    uint32_t vecLen = ASCENDC_QUANT_PER_GROUP_B32_VF_LEN;
    uint16_t repeat = CeilDivision(para.n, vecLen);
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, config.roundMode};

    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> offsetVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<float> f32ScaleVreg;
    Reg::RegTensor<float> f32OffsetVreg;
    Reg::RegTensor<srcT> srcVreg;
    Reg::RegTensor<float> f32Vreg;
    Reg::RegTensor<dstT> b8Vreg;
    Reg::MaskReg b16FullPreg = Reg::CreateMask<uint16_t, Reg::MaskPattern::ALL>();
    for (uint16_t i = 0; i < rowNum; ++i) {
        if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
            GetPerTokenScaleAndOffset<scaleT, config>(scaleUb + i, offsetUb + i, scaleVreg, offsetVreg);
            Reg::Cast<float, scaleT, layoutZMrgZ>(f32ScaleVreg, scaleVreg, b16FullPreg);
            if constexpr (config.hasOffset) {
                Reg::Cast<float, scaleT, layoutZMrgZ>(f32OffsetVreg, offsetVreg, b16FullPreg);
            }
        } else {
            GetPerTokenScaleAndOffset<scaleT, config>(scaleUb + i, offsetUb + i, f32ScaleVreg, f32OffsetVreg);
        }
        uint32_t sreg = para.n;
        for (uint16_t j = 0; j < repeat; ++j) {
            preg = Reg::UpdateMask<uint32_t>(sreg);
            if constexpr (SupportType<srcT, half, bfloat16_t>()) {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(srcVreg, srcUb + i * para.n + j * vecLen);
                Reg::Cast<float, srcT, layoutZMrgZ>(f32Vreg, srcVreg, preg);
            } else {
                Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(f32Vreg, srcUb + i * para.n + j * vecLen);
            }
            Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32Vreg, f32Vreg, f32ScaleVreg, preg);
            AddQuantOffsetIfExist<float, config>(f32Vreg, f32OffsetVreg, preg);
            TransRegForFp8<dstT, float, castTrait>(f32Vreg, b8Vreg, preg);
            Reg::StoreAlign<dstT, Reg::StoreDist::DIST_PACK4_B32>(dstUb + i * para.n + j * vecLen, b8Vreg, preg);
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__aicore__ inline void QuantPerTokenForFp8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const LocalTensor<scaleT>& offsetTensor, const AscendQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    __ubuf__ scaleT* offsetUb = (__ubuf__ scaleT*)offsetTensor.GetPhyAddr();
    QuantPerTokenForFp8VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, offsetUb, para);
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__simd_vf__ inline void QuantPerTokenForHif8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, __ubuf__ scaleT* offsetUb,
    const AscendQuantParam para)
{
    uint16_t rowNum = para.calCount / para.n;
    uint32_t vecLen = GetVecLen() / sizeof(scaleT);
    uint16_t repeat = CeilDivision(para.n, vecLen);
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, config.roundMode};

    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> srcVreg;
    Reg::RegTensor<dstT> dstVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<scaleT> offsetVreg;
    Reg::RegTensor<srcT> tempVreg;
    for (uint16_t i = 0; i < rowNum; ++i) {
        GetPerTokenScaleAndOffset<scaleT, config>(scaleUb + i, offsetUb + i, scaleVreg, offsetVreg);
        uint32_t sreg = para.n;
        for (uint16_t j = 0; j < repeat; ++j) {
            preg = Reg::UpdateMask<scaleT>(sreg);
            if constexpr (SupportType<srcT, half, bfloat16_t>() && SupportType<scaleT, float>()) {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(tempVreg, srcUb + i * para.n + j * vecLen);
                Reg::Cast<float, srcT, layoutZMrgZ>(srcVreg, tempVreg, preg);
            } else {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_NORM>(srcVreg, srcUb + i * para.n + j * vecLen);
            }
            Reg::Mul<scaleT, Reg::MaskMergeMode::ZEROING>(srcVreg, srcVreg, scaleVreg, preg);
            if constexpr (config.hasOffset) {
                Reg::Add<scaleT, Reg::MaskMergeMode::ZEROING>(srcVreg, srcVreg, offsetVreg, preg);
            }
            TransRegForHif8<dstT, scaleT, castTrait>(srcVreg, dstVreg, preg);
            StoreRes<dstT, scaleT>(dstUb + i * para.n + j * vecLen, dstVreg, preg);
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__aicore__ inline void QuantPerTokenForHif8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const LocalTensor<scaleT>& offsetTensor, const AscendQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    __ubuf__ scaleT* offsetUb = (__ubuf__ scaleT*)offsetTensor.GetPhyAddr();
    QuantPerTokenForHif8VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, offsetUb, para);
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__simd_vf__ inline void QuantPerTokenForS8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, __ubuf__ scaleT* offsetUb,
    const AscendQuantParam para)
{
    uint16_t rowNum = para.calCount / para.n;
    uint32_t vecLen = GetVecLen() / sizeof(scaleT);
    uint16_t repeat = CeilDivision(para.n, vecLen);
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, config.roundMode};

    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> srcVreg;
    Reg::RegTensor<dstT> dstVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<scaleT> offsetVreg;
    Reg::RegTensor<srcT> tempVreg;
    for (uint16_t i = 0; i < rowNum; ++i) {
        GetPerTokenScaleAndOffset<scaleT, config>(scaleUb + i, offsetUb + i, scaleVreg, offsetVreg);
        uint32_t sreg = para.n;
        for (uint16_t j = 0; j < repeat; ++j) {
            preg = Reg::UpdateMask<scaleT>(sreg);
            if constexpr (SupportType<srcT, half, bfloat16_t>() && SupportType<scaleT, float>()) {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(tempVreg, srcUb + i * para.n + j * vecLen);
                Reg::Cast<float, srcT, layoutZMrgZ>(srcVreg, tempVreg, preg);
            } else {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_NORM>(srcVreg, srcUb + i * para.n + j * vecLen);
            }
            Reg::Mul<scaleT, Reg::MaskMergeMode::ZEROING>(srcVreg, srcVreg, scaleVreg, preg);
            if constexpr (config.hasOffset) {
                Reg::Add<scaleT, Reg::MaskMergeMode::ZEROING>(srcVreg, srcVreg, offsetVreg, preg);
            }
            TransRegForS8<dstT, scaleT, castTrait>(srcVreg, dstVreg, preg);
            StoreRes<dstT, scaleT>(dstUb + i * para.n + j * vecLen, dstVreg, preg);
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__aicore__ inline void QuantPerTokenForS8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const LocalTensor<scaleT>& offsetTensor, const AscendQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    __ubuf__ scaleT* offsetUb = (__ubuf__ scaleT*)offsetTensor.GetPhyAddr();
    QuantPerTokenForS8VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, offsetUb, para);
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__simd_vf__ inline void QuantPerTokenForFp8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, const scaleT offset,
    const AscendQuantParam para)
{
    uint16_t rowNum = para.calCount / para.n;
    uint32_t vecLen = ASCENDC_QUANT_PER_GROUP_B32_VF_LEN;
    uint16_t repeat = CeilDivision(para.n, vecLen);
    float fp32_offset = ConvertToFloat<scaleT>(offset);
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, config.roundMode};

    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<float> f32ScaleVreg;
    Reg::RegTensor<srcT> srcVreg;
    Reg::RegTensor<float> f32Vreg;
    Reg::RegTensor<dstT> b8Vreg;
    Reg::MaskReg b16FullPreg = Reg::CreateMask<uint16_t, Reg::MaskPattern::ALL>();
    for (uint16_t i = 0; i < rowNum; ++i) {
        if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
            Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_BRC_B16>(scaleVreg, scaleUb + i);
            Reg::Cast<float, scaleT, layoutZMrgZ>(f32ScaleVreg, scaleVreg, b16FullPreg);
        } else {
            Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_BRC_B32>(f32ScaleVreg, scaleUb + i);
        }
        uint32_t sreg = para.n;
        for (uint16_t j = 0; j < repeat; ++j) {
            preg = Reg::UpdateMask<uint32_t>(sreg);
            if constexpr (SupportType<srcT, half, bfloat16_t>()) {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(srcVreg, srcUb + i * para.n + j * vecLen);
                Reg::Cast<float, srcT, layoutZMrgZ>(f32Vreg, srcVreg, preg);
            } else {
                Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(f32Vreg, srcUb + i * para.n + j * vecLen);
            }
            Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32Vreg, f32Vreg, f32ScaleVreg, preg);
            if constexpr (config.hasOffset) {
                Reg::Adds<float, float, Reg::MaskMergeMode::ZEROING>(f32Vreg, f32Vreg, fp32_offset, preg);
            }
            TransRegForFp8<dstT, float, castTrait>(f32Vreg, b8Vreg, preg);
            Reg::StoreAlign<dstT, Reg::StoreDist::DIST_PACK4_B32>(dstUb + i * para.n + j * vecLen, b8Vreg, preg);
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__aicore__ inline void QuantPerTokenForFp8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const scaleT& offset, const AscendQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    QuantPerTokenForFp8VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, offset, para);
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__simd_vf__ inline void QuantPerTokenForHif8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, const scaleT offset,
    const AscendQuantParam para)
{
    uint16_t rowNum = para.calCount / para.n;
    uint32_t vecLen = GetVecLen() / sizeof(scaleT);
    uint16_t repeat = CeilDivision(para.n, vecLen);
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, config.roundMode};

    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> srcVreg;
    Reg::RegTensor<dstT> dstVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<srcT> tempVreg;
    for (uint16_t i = 0; i < rowNum; ++i) {
        GetPerTokenScale<scaleT>(scaleUb + i, scaleVreg);
        uint32_t sreg = para.n;
        for (uint16_t j = 0; j < repeat; ++j) {
            preg = Reg::UpdateMask<scaleT>(sreg);
            if constexpr (SupportType<srcT, half, bfloat16_t>() && SupportType<scaleT, float>()) {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(tempVreg, srcUb + i * para.n + j * vecLen);
                Reg::Cast<float, srcT, layoutZMrgZ>(srcVreg, tempVreg, preg);
            } else {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_NORM>(srcVreg, srcUb + i * para.n + j * vecLen);
            }
            Reg::Mul<scaleT, Reg::MaskMergeMode::ZEROING>(srcVreg, srcVreg, scaleVreg, preg);
            if constexpr (config.hasOffset) {
                Reg::Adds<scaleT, scaleT, Reg::MaskMergeMode::ZEROING>(
                    srcVreg, srcVreg, static_cast<scaleT>(offset), preg);
            }
            TransRegForHif8<dstT, scaleT, castTrait>(srcVreg, dstVreg, preg);
            StoreRes<dstT, scaleT>(dstUb + i * para.n + j * vecLen, dstVreg, preg);
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__aicore__ inline void QuantPerTokenForHif8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const scaleT& offset, const AscendQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    QuantPerTokenForHif8VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, offset, para);
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__simd_vf__ inline void QuantPerTokenForS8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, const scaleT offset,
    const AscendQuantParam para)
{
    uint16_t rowNum = para.calCount / para.n;
    uint32_t vecLen = GetVecLen() / sizeof(scaleT);
    uint16_t repeat = CeilDivision(para.n, vecLen);
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, config.roundMode};

    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> srcVreg;
    Reg::RegTensor<dstT> dstVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<srcT> tempVreg;
    for (uint16_t i = 0; i < rowNum; ++i) {
        GetPerTokenScale<scaleT>(scaleUb + i, scaleVreg);
        uint32_t sreg = para.n;
        for (uint16_t j = 0; j < repeat; ++j) {
            preg = Reg::UpdateMask<scaleT>(sreg);
            if constexpr (SupportType<srcT, half, bfloat16_t>() && SupportType<scaleT, float>()) {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(tempVreg, srcUb + i * para.n + j * vecLen);
                Reg::Cast<float, srcT, layoutZMrgZ>(srcVreg, tempVreg, preg);
            } else {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_NORM>(srcVreg, srcUb + i * para.n + j * vecLen);
            }
            Reg::Mul<scaleT, Reg::MaskMergeMode::ZEROING>(srcVreg, srcVreg, scaleVreg, preg);
            if constexpr (config.hasOffset) {
                Reg::Adds<scaleT, scaleT, Reg::MaskMergeMode::ZEROING>(
                    srcVreg, srcVreg, static_cast<scaleT>(offset), preg);
            }
            TransRegForS8<dstT, scaleT, castTrait>(srcVreg, dstVreg, preg);
            StoreRes<dstT, scaleT>(dstUb + i * para.n + j * vecLen, dstVreg, preg);
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__aicore__ inline void QuantPerTokenForS8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const scaleT offset, const AscendQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    QuantPerTokenForS8VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, offset, para);
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__simd_vf__ inline void QuantPerGroupForKColFp4VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, const AscendQuantParam para)
{
    uint16_t rowNum = para.calCount / para.n;
    uint32_t vecLen = GetVecLen() / sizeof(scaleT);
    uint16_t repeat = CeilDivision(para.n, vecLen);
    uint16_t scaleK = CeilDivision(para.n, para.groupSize);
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, config.roundMode};

    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> srcVreg;
    Reg::RegTensor<dstT> dstVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<srcT> tempVreg;
    for (uint16_t i = 0; i < rowNum; ++i) {
        uint32_t sreg = para.n;
        for (uint16_t j = 0; j < repeat; ++j) {
            preg = Reg::UpdateMask<scaleT>(sreg);
            GetPerGroupScale(scaleUb + i * scaleK, j * vecLen, para, config, scaleVreg);
            if constexpr (SupportType<srcT, half, bfloat16_t>() && SupportType<scaleT, float>()) {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(tempVreg, srcUb + i * para.n + j * vecLen);
                Reg::Cast<float, srcT, layoutZMrgZ>(srcVreg, tempVreg, preg);
            } else {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_NORM>(srcVreg, srcUb + i * para.n + j * vecLen);
            }
            Reg::Mul<scaleT, Reg::MaskMergeMode::ZEROING>(srcVreg, srcVreg, scaleVreg, preg);
            TransRegForFp4<dstT, scaleT, castTrait>(srcVreg, dstVreg, preg);
            Reg::StoreAlign<uint8_t, Reg::StoreDist::DIST_PACK4_B32>(
                (__ubuf__ uint8_t*)dstUb + (i * para.n + j * vecLen) / 2, (Reg::RegTensor<uint8_t>&)dstVreg, preg);
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__aicore__ inline void QuantPerGroupForKColFp4(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const AscendQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    QuantPerGroupForKColFp4VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, para);
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__simd_vf__ inline void QuantPerGroupForKColFp8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, __ubuf__ scaleT* offsetUb,
    const AscendQuantParam para)
{
    uint16_t rowNum = para.calCount / para.n;
    uint32_t vecLen = ASCENDC_QUANT_PER_GROUP_B32_VF_LEN;
    uint16_t repeat = CeilDivision(para.n, vecLen);
    uint16_t scaleK = CeilDivision(para.n, para.groupSize);
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, config.roundMode};

    Reg::MaskReg preg;
    Reg::RegTensor<float> f32ScaleVreg;
    Reg::RegTensor<float> f32OffsetVreg;
    Reg::RegTensor<srcT> srcVreg;
    Reg::RegTensor<float> f32Vreg;
    Reg::RegTensor<dstT> b8Vreg;
    for (uint16_t i = 0; i < rowNum; ++i) {
        uint32_t sreg = para.n;
        for (uint16_t j = 0; j < repeat; ++j) {
            preg = Reg::UpdateMask<uint32_t>(sreg);
            GetPerGroupScaleEntry<scaleT, config>(scaleUb + i * scaleK, para, j * vecLen, preg, f32ScaleVreg);
            GetPerGroupScaleEntry<scaleT, config>(offsetUb + i * scaleK, para, j * vecLen, preg, f32OffsetVreg);
            if constexpr (SupportType<srcT, half, bfloat16_t>()) {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(srcVreg, srcUb + i * para.n + j * vecLen);
                Reg::Cast<float, srcT, layoutZMrgZ>(f32Vreg, srcVreg, preg);
            } else {
                Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(f32Vreg, srcUb + i * para.n + j * vecLen);
            }
            Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32Vreg, f32Vreg, f32ScaleVreg, preg);
            if constexpr (config.hasOffset) {
                Reg::Add<float, Reg::MaskMergeMode::ZEROING>(f32Vreg, f32Vreg, f32OffsetVreg, preg);
            }
            TransRegForFp8<dstT, float, castTrait>(f32Vreg, b8Vreg, preg);
            Reg::StoreAlign<dstT, Reg::StoreDist::DIST_PACK4_B32>(dstUb + i * para.n + j * vecLen, b8Vreg, preg);
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__aicore__ inline void QuantPerGroupForKColFp8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const LocalTensor<scaleT>& offsetTensor, const AscendQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    __ubuf__ scaleT* offsetUb = (__ubuf__ scaleT*)offsetTensor.GetPhyAddr();
    QuantPerGroupForKColFp8VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, offsetUb, para);
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__simd_vf__ inline void QuantPerGroupForKColHif8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, __ubuf__ scaleT* offsetUb,
    const AscendQuantParam para)
{
    uint16_t rowNum = para.calCount / para.n;
    uint32_t vecLen = GetVecLen() / sizeof(scaleT);
    uint16_t repeat = CeilDivision(para.n, vecLen);
    uint16_t scaleK = CeilDivision(para.n, para.groupSize);
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, config.roundMode};

    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> srcVreg;
    Reg::RegTensor<dstT> dstVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<scaleT> offsetVreg;
    Reg::RegTensor<srcT> tempSrcVreg;
    for (uint16_t i = 0; i < rowNum; ++i) {
        uint32_t sreg = para.n;
        for (uint16_t j = 0; j < repeat; ++j) {
            preg = Reg::UpdateMask<scaleT>(sreg);
            GetPerGroupScale(scaleUb + i * scaleK, j * vecLen, para, config, scaleVreg);
            if constexpr (config.hasOffset) {
                GetPerGroupOffset(offsetUb + i * scaleK, j * vecLen, para, config, offsetVreg);
            }
            if constexpr (SupportType<srcT, half, bfloat16_t>() && SupportType<scaleT, float>()) {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(tempSrcVreg, srcUb + i * para.n + j * vecLen);
                Reg::Cast<float, srcT, layoutZMrgZ>(srcVreg, tempSrcVreg, preg);
            } else {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_NORM>(srcVreg, srcUb + i * para.n + j * vecLen);
            }
            Reg::Mul<scaleT, Reg::MaskMergeMode::ZEROING>(srcVreg, srcVreg, scaleVreg, preg);
            if constexpr (config.hasOffset) {
                Reg::Add<scaleT, Reg::MaskMergeMode::ZEROING>(srcVreg, srcVreg, offsetVreg, preg);
            }
            TransRegForHif8<dstT, scaleT, castTrait>(srcVreg, dstVreg, preg);
            StoreRes<dstT, scaleT>(dstUb + i * para.n + j * vecLen, dstVreg, preg);
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__aicore__ inline void QuantPerGroupForKColHif8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const LocalTensor<scaleT>& offsetTensor, const AscendQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    __ubuf__ scaleT* offsetUb = (__ubuf__ scaleT*)offsetTensor.GetPhyAddr();
    QuantPerGroupForKColHif8VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, offsetUb, para);
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__simd_vf__ inline void QuantPerGroupForKColS8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, __ubuf__ scaleT* offsetUb,
    const AscendQuantParam para)
{
    uint16_t rowNum = para.calCount / para.n;
    uint32_t vecLen = GetVecLen() / sizeof(scaleT);
    uint16_t repeat = CeilDivision(para.n, vecLen);
    uint16_t scaleK = CeilDivision(para.n, para.groupSize);
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, config.roundMode};

    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> srcVreg;
    Reg::RegTensor<dstT> dstVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<scaleT> offsetVreg;
    Reg::RegTensor<srcT> tempVreg;
    for (uint16_t i = 0; i < rowNum; ++i) {
        uint32_t sreg = para.n;
        for (uint16_t j = 0; j < repeat; ++j) {
            preg = Reg::UpdateMask<scaleT>(sreg);
            GetPerGroupScale(scaleUb + i * scaleK, j * vecLen, para, config, scaleVreg);
            if constexpr (config.hasOffset) {
                GetPerGroupOffset(offsetUb + i * scaleK, j * vecLen, para, config, offsetVreg);
            }
            if constexpr (SupportType<srcT, half, bfloat16_t>() && SupportType<scaleT, float>()) {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(tempVreg, srcUb + i * para.n + j * vecLen);
                Reg::Cast<float, srcT, layoutZMrgZ>(srcVreg, tempVreg, preg);
            } else {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_NORM>(srcVreg, srcUb + i * para.n + j * vecLen);
            }
            Reg::Mul<scaleT, Reg::MaskMergeMode::ZEROING>(srcVreg, srcVreg, scaleVreg, preg);
            if constexpr (config.hasOffset) {
                Reg::Add<scaleT, Reg::MaskMergeMode::ZEROING>(srcVreg, srcVreg, offsetVreg, preg);
            }
            TransRegForS8<dstT, scaleT, castTrait>(srcVreg, dstVreg, preg);
            StoreRes<dstT, scaleT>(dstUb + i * para.n + j * vecLen, dstVreg, preg);
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__aicore__ inline void QuantPerGroupForKColS8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const LocalTensor<scaleT>& offsetTensor, const AscendQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    __ubuf__ scaleT* offsetUb = (__ubuf__ scaleT*)offsetTensor.GetPhyAddr();
    QuantPerGroupForKColS8VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, offsetUb, para);
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__simd_vf__ inline void QuantPerGroupForKColFp8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, const scaleT offset,
    const AscendQuantParam para)
{
    uint16_t rowNum = para.calCount / para.n;
    uint32_t vecLen = ASCENDC_QUANT_PER_GROUP_B32_VF_LEN;
    uint16_t repeat = CeilDivision(para.n, vecLen);
    uint32_t sreg = para.n;
    uint16_t scaleK = CeilDivision(para.n, para.groupSize);
    float fp32_offset = ConvertToFloat<scaleT>(offset);
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, config.roundMode};

    Reg::MaskReg preg;
    Reg::RegTensor<float> f32ScaleVreg;
    Reg::RegTensor<float> f32OffsetVreg;
    Reg::RegTensor<srcT> srcVreg;
    Reg::RegTensor<float> f32Vreg;
    Reg::RegTensor<dstT> b8Vreg;
    for (uint16_t i = 0; i < rowNum; ++i) {
        uint32_t sreg = para.n;
        for (uint16_t j = 0; j < repeat; ++j) {
            preg = Reg::UpdateMask<uint32_t>(sreg);
            GetPerGroupScaleEntry<scaleT, config>(scaleUb + i * scaleK, para, j * vecLen, preg, f32ScaleVreg);
            if constexpr (SupportType<srcT, half, bfloat16_t>()) {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(srcVreg, srcUb + i * para.n + j * vecLen);
                Reg::Cast<float, srcT, layoutZMrgZ>(f32Vreg, srcVreg, preg);
            } else {
                Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(f32Vreg, srcUb + i * para.n + j * vecLen);
            }
            Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32Vreg, f32Vreg, f32ScaleVreg, preg);
            if constexpr (config.hasOffset) {
                Reg::Adds<float, float, Reg::MaskMergeMode::ZEROING>(f32Vreg, f32Vreg, fp32_offset, preg);
            }
            TransRegForFp8<dstT, float, castTrait>(f32Vreg, b8Vreg, preg);
            Reg::StoreAlign<dstT, Reg::StoreDist::DIST_PACK4_B32>(dstUb + i * para.n + j * vecLen, b8Vreg, preg);
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__aicore__ inline void QuantPerGroupForKColFp8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const scaleT& offset, const AscendQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    QuantPerGroupForKColFp8VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, offset, para);
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__simd_vf__ inline void QuantPerGroupForKColHif8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, const scaleT offset,
    const AscendQuantParam para)
{
    uint16_t rowNum = para.calCount / para.n;
    uint32_t vecLen = GetVecLen() / sizeof(scaleT);
    uint16_t repeat = CeilDivision(para.n, vecLen);
    uint16_t scaleK = CeilDivision(para.n, para.groupSize);
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, config.roundMode};

    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> srcVreg;
    Reg::RegTensor<dstT> dstVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<srcT> tempSrcVreg;
    for (uint16_t i = 0; i < rowNum; ++i) {
        uint32_t sreg = para.n;
        for (uint16_t j = 0; j < repeat; ++j) {
            preg = Reg::UpdateMask<scaleT>(sreg);
            GetPerGroupScale(scaleUb + i * scaleK, j * vecLen, para, config, scaleVreg);
            if constexpr (SupportType<srcT, half, bfloat16_t>() && SupportType<scaleT, float>()) {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(tempSrcVreg, srcUb + i * para.n + j * vecLen);
                Reg::Cast<float, srcT, layoutZMrgZ>(srcVreg, tempSrcVreg, preg);
            } else {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_NORM>(srcVreg, srcUb + i * para.n + j * vecLen);
            }
            Reg::Mul<scaleT, Reg::MaskMergeMode::ZEROING>(srcVreg, srcVreg, scaleVreg, preg);
            if constexpr (config.hasOffset) {
                Reg::Adds<scaleT, scaleT, Reg::MaskMergeMode::ZEROING>(
                    srcVreg, srcVreg, static_cast<scaleT>(offset), preg);
            }
            TransRegForHif8<dstT, scaleT, castTrait>(srcVreg, dstVreg, preg);
            StoreRes<dstT, scaleT>(dstUb + i * para.n + j * vecLen, dstVreg, preg);
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__aicore__ inline void QuantPerGroupForKColHif8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const scaleT& offset, const AscendQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    QuantPerGroupForKColHif8VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, offset, para);
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__simd_vf__ inline void QuantPerGroupForKColS8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, const scaleT offset,
    const AscendQuantParam para)
{
    uint16_t rowNum = para.calCount / para.n;
    uint32_t vecLen = GetVecLen() / sizeof(scaleT);
    uint16_t repeat = CeilDivision(para.n, vecLen);
    uint16_t scaleK = CeilDivision(para.n, para.groupSize);
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, config.roundMode};

    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> srcVreg;
    Reg::RegTensor<dstT> dstVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<srcT> tempVreg;
    for (uint16_t i = 0; i < rowNum; ++i) {
        uint32_t sreg = para.n;
        for (uint16_t j = 0; j < repeat; ++j) {
            preg = Reg::UpdateMask<scaleT>(sreg);
            GetPerGroupScale(scaleUb + i * scaleK, j * vecLen, para, config, scaleVreg);
            if constexpr (SupportType<srcT, half, bfloat16_t>() && SupportType<scaleT, float>()) {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(tempVreg, srcUb + i * para.n + j * vecLen);
                Reg::Cast<float, srcT, layoutZMrgZ>(srcVreg, tempVreg, preg);
            } else {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_NORM>(srcVreg, srcUb + i * para.n + j * vecLen);
            }
            Reg::Mul<scaleT, Reg::MaskMergeMode::ZEROING>(srcVreg, srcVreg, scaleVreg, preg);
            if constexpr (config.hasOffset) {
                Reg::Adds<scaleT, scaleT, Reg::MaskMergeMode::ZEROING>(
                    srcVreg, srcVreg, static_cast<scaleT>(offset), preg);
            }
            TransRegForS8<dstT, scaleT, castTrait>(srcVreg, dstVreg, preg);
            StoreRes<dstT, scaleT>(dstUb + i * para.n + j * vecLen, dstVreg, preg);
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__aicore__ inline void QuantPerGroupForKColS8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const scaleT offset, const AscendQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    QuantPerGroupForKColS8VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, offset, para);
}

template <typename dstT, typename srcT, typename scaleT, const Reg::CastTrait& castTrait>
__simd_callee__ inline void QuantPerGroupForKRowFp4OneRow(
    __ubuf__ dstT* dstAddr, __ubuf__ srcT* srcAddr, __ubuf__ scaleT* scaleAddr, Reg::RegTensor<dstT>& dstVreg,
    Reg::RegTensor<scaleT>& srcVreg, Reg::RegTensor<scaleT>& scaleVreg, Reg::RegTensor<srcT>& tempVreg,
    Reg::MaskReg& preg, uint16_t repeat, uint32_t n, uint32_t vecLen)
{
    for (uint16_t j = 0; j < repeat; ++j) {
        Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_NORM>(scaleVreg, scaleAddr + j * vecLen);
        preg = Reg::UpdateMask<scaleT>(n);
        if constexpr (SupportType<srcT, half, bfloat16_t>() && SupportType<scaleT, float>()) {
            Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(tempVreg, srcAddr + j * vecLen);
            Reg::Cast<float, srcT, layoutZMrgZ>(srcVreg, tempVreg, preg);
        } else {
            Reg::LoadAlign<srcT, Reg::LoadDist::DIST_NORM>(srcVreg, srcAddr + j * vecLen);
        }
        Reg::Mul<scaleT, Reg::MaskMergeMode::ZEROING>(srcVreg, srcVreg, scaleVreg, preg);
        TransRegForFp4<dstT, scaleT, castTrait>(srcVreg, dstVreg, preg);
        Reg::StoreAlign<uint8_t, Reg::StoreDist::DIST_PACK4_B32>(
            (__ubuf__ uint8_t*)dstAddr + (j * vecLen) / 2, (Reg::RegTensor<uint8_t>&)dstVreg, preg);
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__simd_vf__ inline void QuantPerGroupForKRowFp4VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, const AscendQuantParam para, uint16_t rowNum,
    uint16_t tailRow)
{
    uint16_t mainRowGroup = rowNum / para.groupSize;
    uint32_t vecLen = GetVecLen() / sizeof(scaleT);
    uint16_t repeat = CeilDivision(para.n, vecLen);
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, config.roundMode};

    Reg::MaskReg preg;
    Reg::RegTensor<dstT> dstVreg;
    Reg::RegTensor<scaleT> srcVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<srcT> tempVreg;
    for (uint16_t i0 = 0; i0 < mainRowGroup; ++i0) {
        for (uint16_t i1 = 0; i1 < static_cast<uint16_t>(para.groupSize); ++i1) {
            QuantPerGroupForKRowFp4OneRow<dstT, srcT, scaleT, castTrait>(
                dstUb + ((i0 * para.groupSize + i1) * para.n) / 2, srcUb + (i0 * para.groupSize + i1) * para.n,
                scaleUb + i0 * para.n, dstVreg, srcVreg, scaleVreg, tempVreg, preg, repeat, para.n, vecLen);
        }
    }
    for (uint16_t i = 0; i < tailRow; ++i) {
        QuantPerGroupForKRowFp4OneRow<dstT, srcT, scaleT, castTrait>(
            dstUb + ((mainRowGroup * para.groupSize + i) * para.n) / 2,
            srcUb + (mainRowGroup * para.groupSize + i) * para.n, scaleUb + mainRowGroup * para.n, dstVreg, srcVreg,
            scaleVreg, tempVreg, preg, repeat, para.n, vecLen);
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__aicore__ inline void QuantPerGroupForKRowFp4(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const AscendQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    uint16_t rowNum = para.calCount / para.n;
    uint16_t tailRow = rowNum % para.groupSize;
    QuantPerGroupForKRowFp4VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, para, rowNum, tailRow);
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__simd_callee__ inline void QuantPerGroupForKRowFp8TailBlock(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, __ubuf__ scaleT* offsetUb, uint16_t repeat,
    uint16_t tailRow, uint32_t n, uint32_t vecLen)
{
    Reg::MaskReg preg;
    Reg::RegTensor<float> f32ScaleVreg;
    Reg::RegTensor<float> f32OffsetVreg;
    Reg::RegTensor<srcT> srcVreg;
    Reg::RegTensor<float> f32Vreg;
    Reg::RegTensor<dstT> b8Vreg;
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, config.roundMode};
    for (uint16_t i = 0; i < tailRow; ++i) {
        uint32_t sreg = n;
        for (uint16_t j = 0; j < repeat; ++j) {
            GetPerGroupKRowScaleEntry<scaleT>(scaleUb + j * vecLen, f32ScaleVreg);
            GetPerGroupKRowOffsetEntry<scaleT, config>(offsetUb + j * vecLen, f32OffsetVreg);
            preg = Reg::UpdateMask<uint32_t>(sreg);
            if constexpr (SupportType<srcT, half, bfloat16_t>()) {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(srcVreg, srcUb + i * n + j * vecLen);
                Reg::Cast<float, srcT, layoutZMrgZ>(f32Vreg, srcVreg, preg);
            } else {
                Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(f32Vreg, srcUb + i * n + j * vecLen);
            }
            Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32Vreg, f32Vreg, f32ScaleVreg, preg);
            if constexpr (config.hasOffset) {
                Reg::Add<float, Reg::MaskMergeMode::ZEROING>(f32Vreg, f32Vreg, f32OffsetVreg, preg);
            }
            TransRegForFp8<dstT, float, castTrait>(f32Vreg, b8Vreg, preg);
            Reg::StoreAlign<dstT, Reg::StoreDist::DIST_PACK4_B32>(dstUb + i * n + j * vecLen, b8Vreg, preg);
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__simd_vf__ inline void QuantPerGroupForKRowFp8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, __ubuf__ scaleT* offsetUb,
    const AscendQuantParam para, uint16_t rowNum, uint16_t tailRow)
{
    uint16_t mainRowGroup = rowNum / para.groupSize;
    uint32_t vecLen = ASCENDC_QUANT_PER_GROUP_B32_VF_LEN;
    uint16_t repeat = CeilDivision(para.n, vecLen);
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, config.roundMode};

    Reg::MaskReg preg;
    Reg::RegTensor<float> f32ScaleVreg;
    Reg::RegTensor<float> f32OffsetVreg;
    Reg::RegTensor<srcT> srcVreg;
    Reg::RegTensor<float> f32Vreg;
    Reg::RegTensor<dstT> b8Vreg;
    for (uint16_t i = 0; i < mainRowGroup; ++i) {
        for (uint16_t j = 0; j < static_cast<uint16_t>(para.groupSize); ++j) {
            uint32_t sreg = para.n;
            for (uint16_t k = 0; k < repeat; ++k) {
                GetPerGroupKRowScaleEntry<scaleT>(scaleUb + i * para.n + k * vecLen, f32ScaleVreg);
                GetPerGroupKRowOffsetEntry<scaleT, config>(offsetUb + i * para.n + k * vecLen, f32OffsetVreg);
                preg = Reg::UpdateMask<uint32_t>(sreg);
                if constexpr (SupportType<srcT, half, bfloat16_t>()) {
                    Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(
                        srcVreg, srcUb + (i * para.groupSize + j) * para.n + k * vecLen);
                    Reg::Cast<float, srcT, layoutZMrgZ>(f32Vreg, srcVreg, preg);
                } else {
                    Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(
                        f32Vreg, srcUb + (i * para.groupSize + j) * para.n + k * vecLen);
                }
                Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32Vreg, f32Vreg, f32ScaleVreg, preg);
                if constexpr (config.hasOffset) {
                    Reg::Add<float, Reg::MaskMergeMode::ZEROING>(f32Vreg, f32Vreg, f32OffsetVreg, preg);
                }
                TransRegForFp8<dstT, float, castTrait>(f32Vreg, b8Vreg, preg);
                Reg::StoreAlign<dstT, Reg::StoreDist::DIST_PACK4_B32>(
                    dstUb + (i * para.groupSize + j) * para.n + k * vecLen, b8Vreg, preg);
            }
        }
    }
    QuantPerGroupForKRowFp8TailBlock<dstT, srcT, scaleT, config>(
        dstUb + mainRowGroup * para.groupSize * para.n, srcUb + mainRowGroup * para.groupSize * para.n,
        scaleUb + mainRowGroup * para.n, offsetUb + mainRowGroup * para.n, repeat, tailRow, para.n, vecLen);
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__aicore__ inline void QuantPerGroupForKRowFp8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const LocalTensor<scaleT>& offsetTensor, const AscendQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    __ubuf__ scaleT* offsetUb = (__ubuf__ scaleT*)offsetTensor.GetPhyAddr();
    uint16_t rowNum = para.calCount / para.n;
    uint16_t tailRow = rowNum % para.groupSize;
    QuantPerGroupForKRowFp8VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, offsetUb, para, rowNum, tailRow);
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__simd_callee__ inline void QuantPerGroupForKRowHif8TailBlock(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, __ubuf__ scaleT* offsetUb, uint16_t repeat,
    uint16_t tailRow, uint32_t n, uint32_t vecLen)
{
    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> offsetVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<dstT> dstVreg;
    Reg::RegTensor<scaleT> srcVreg;
    Reg::RegTensor<srcT> tempSrcVreg;
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, config.roundMode};
    for (uint16_t i = 0; i < tailRow; ++i) {
        uint32_t sreg = n;
        for (uint16_t j = 0; j < repeat; ++j) {
            Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_NORM>(scaleVreg, scaleUb + j * vecLen);
            if constexpr (config.hasOffset) {
                Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_NORM>(offsetVreg, offsetUb + j * vecLen);
            }
            preg = Reg::UpdateMask<scaleT>(sreg);
            if constexpr (SupportType<srcT, half, bfloat16_t>() && SupportType<scaleT, float>()) {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(tempSrcVreg, srcUb + i * n + j * vecLen);
                Reg::Cast<float, srcT, layoutZMrgZ>(srcVreg, tempSrcVreg, preg);
            } else {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_NORM>(srcVreg, srcUb + i * n + j * vecLen);
            }
            Reg::Mul<scaleT, Reg::MaskMergeMode::ZEROING>(srcVreg, srcVreg, scaleVreg, preg);
            if constexpr (config.hasOffset) {
                Reg::Add<scaleT, Reg::MaskMergeMode::ZEROING>(srcVreg, srcVreg, offsetVreg, preg);
            }
            TransRegForHif8<dstT, scaleT, castTrait>(srcVreg, dstVreg, preg);
            StoreRes<dstT, scaleT>(dstUb + i * n + j * vecLen, dstVreg, preg);
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__simd_vf__ inline void QuantPerGroupForKRowHif8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, __ubuf__ scaleT* offsetUb,
    const AscendQuantParam para, uint16_t rowNum, uint16_t tailRow)
{
    uint16_t mainRowGroup = rowNum / para.groupSize;
    uint32_t vecLen = GetVecLen() / sizeof(scaleT);
    uint16_t repeat = CeilDivision(para.n, vecLen);
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, config.roundMode};

    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> offsetVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<dstT> dstVreg;
    Reg::RegTensor<scaleT> srcVreg;
    Reg::RegTensor<srcT> tempSrcVreg;
    for (uint16_t i = 0; i < mainRowGroup; ++i) {
        for (uint16_t j = 0; j < static_cast<uint16_t>(para.groupSize); ++j) {
            uint32_t sreg = para.n;
            for (uint16_t k = 0; k < repeat; ++k) {
                LoadContinuousScaleAndOffset<scaleT, config>(
                    scaleUb + i * para.n + k * vecLen, offsetUb + i * para.n + k * vecLen, scaleVreg, offsetVreg);
                preg = Reg::UpdateMask<scaleT>(sreg);
                if constexpr (SupportType<srcT, half, bfloat16_t>() && SupportType<scaleT, float>()) {
                    Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(
                        tempSrcVreg, srcUb + (i * para.groupSize + j) * para.n + k * vecLen);
                    Reg::Cast<float, srcT, layoutZMrgZ>(srcVreg, tempSrcVreg, preg);
                } else {
                    Reg::LoadAlign<srcT, Reg::LoadDist::DIST_NORM>(
                        srcVreg, srcUb + (i * para.groupSize + j) * para.n + k * vecLen);
                }
                Reg::Mul<scaleT, Reg::MaskMergeMode::ZEROING>(srcVreg, srcVreg, scaleVreg, preg);
                if constexpr (config.hasOffset) {
                    Reg::Add<scaleT, Reg::MaskMergeMode::ZEROING>(srcVreg, srcVreg, offsetVreg, preg);
                }
                TransRegForHif8<dstT, scaleT, castTrait>(srcVreg, dstVreg, preg);
                StoreRes<dstT, scaleT>(dstUb + (i * para.groupSize + j) * para.n + k * vecLen, dstVreg, preg);
            }
        }
    }
    QuantPerGroupForKRowHif8TailBlock<dstT, srcT, scaleT, config>(
        dstUb + mainRowGroup * para.groupSize * para.n, srcUb + mainRowGroup * para.groupSize * para.n,
        scaleUb + mainRowGroup * para.n, offsetUb + mainRowGroup * para.n, repeat, tailRow, para.n, vecLen);
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__aicore__ inline void QuantPerGroupForKRowHif8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const LocalTensor<scaleT>& offsetTensor, const AscendQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    __ubuf__ scaleT* offsetUb = (__ubuf__ scaleT*)offsetTensor.GetPhyAddr();
    uint16_t rowNum = para.calCount / para.n;
    uint16_t tailRow = rowNum % para.groupSize;
    QuantPerGroupForKRowHif8VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, offsetUb, para, rowNum, tailRow);
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__simd_callee__ inline void QuantPerGroupForKRowS8TailBlock(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, __ubuf__ scaleT* offsetUb, uint16_t repeat,
    uint16_t tailRow, uint32_t n, uint32_t vecLen)
{
    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> offsetVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<srcT> tempVreg;
    Reg::RegTensor<dstT> dstVreg;
    Reg::RegTensor<scaleT> srcVreg;
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, config.roundMode};
    for (uint16_t i = 0; i < tailRow; ++i) {
        uint32_t sreg = n;
        for (uint16_t j = 0; j < repeat; ++j) {
            Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_NORM>(scaleVreg, scaleUb + j * vecLen);
            if constexpr (config.hasOffset) {
                Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_NORM>(offsetVreg, offsetUb + j * vecLen);
            }
            preg = Reg::UpdateMask<scaleT>(sreg);
            if constexpr (SupportType<srcT, half, bfloat16_t>() && SupportType<scaleT, float>()) {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(tempVreg, srcUb + i * n + j * vecLen);
                Reg::Cast<float, srcT, layoutZMrgZ>(srcVreg, tempVreg, preg);
            } else {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_NORM>(srcVreg, srcUb + i * n + j * vecLen);
            }
            Reg::Mul<scaleT, Reg::MaskMergeMode::ZEROING>(srcVreg, srcVreg, scaleVreg, preg);
            if constexpr (config.hasOffset) {
                Reg::Add<scaleT, Reg::MaskMergeMode::ZEROING>(srcVreg, srcVreg, offsetVreg, preg);
            }
            TransRegForS8<dstT, scaleT, castTrait>(srcVreg, dstVreg, preg);
            StoreRes<dstT, scaleT>(dstUb + i * n + j * vecLen, dstVreg, preg);
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__simd_vf__ inline void QuantPerGroupForKRowS8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, __ubuf__ scaleT* offsetUb,
    const AscendQuantParam para, uint16_t rowNum, uint16_t tailRow)
{
    uint16_t mainRowGroup = rowNum / para.groupSize;
    uint32_t vecLen = GetVecLen() / sizeof(scaleT);
    uint16_t repeat = CeilDivision(para.n, vecLen);
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, config.roundMode};

    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> offsetVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<srcT> tempVreg;
    Reg::RegTensor<dstT> dstVreg;
    Reg::RegTensor<scaleT> srcVreg;
    for (uint16_t i = 0; i < mainRowGroup; ++i) {
        for (uint16_t j = 0; j < static_cast<uint16_t>(para.groupSize); ++j) {
            uint32_t sreg = para.n;
            for (uint16_t k = 0; k < repeat; ++k) {
                LoadContinuousScaleAndOffset<scaleT, config>(
                    scaleUb + i * para.n + k * vecLen, offsetUb + i * para.n + k * vecLen, scaleVreg, offsetVreg);
                preg = Reg::UpdateMask<scaleT>(sreg);
                if constexpr (SupportType<srcT, half, bfloat16_t>() && SupportType<scaleT, float>()) {
                    Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(
                        tempVreg, srcUb + (i * para.groupSize + j) * para.n + k * vecLen);
                    Reg::Cast<float, srcT, layoutZMrgZ>(srcVreg, tempVreg, preg);
                } else {
                    Reg::LoadAlign<srcT, Reg::LoadDist::DIST_NORM>(
                        srcVreg, srcUb + (i * para.groupSize + j) * para.n + k * vecLen);
                }
                Reg::Mul<scaleT, Reg::MaskMergeMode::ZEROING>(srcVreg, srcVreg, scaleVreg, preg);
                if constexpr (config.hasOffset) {
                    Reg::Add<scaleT, Reg::MaskMergeMode::ZEROING>(srcVreg, srcVreg, offsetVreg, preg);
                }
                TransRegForS8<dstT, scaleT, castTrait>(srcVreg, dstVreg, preg);
                StoreRes<dstT, scaleT>(dstUb + (i * para.groupSize + j) * para.n + k * vecLen, dstVreg, preg);
            }
        }
    }
    QuantPerGroupForKRowS8TailBlock<dstT, srcT, scaleT, config>(
        dstUb + mainRowGroup * para.groupSize * para.n, srcUb + mainRowGroup * para.groupSize * para.n,
        scaleUb + mainRowGroup * para.n, offsetUb + mainRowGroup * para.n, repeat, tailRow, para.n, vecLen);
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__aicore__ inline void QuantPerGroupForKRowS8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const LocalTensor<scaleT>& offsetTensor, const AscendQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    __ubuf__ scaleT* offsetUb = (__ubuf__ scaleT*)offsetTensor.GetPhyAddr();
    uint16_t rowNum = para.calCount / para.n;
    uint16_t tailRow = rowNum % para.groupSize;
    QuantPerGroupForKRowS8VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, offsetUb, para, rowNum, tailRow);
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__simd_callee__ inline void QuantPerGroupForKRowFp8TailBlock(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, const scaleT& offset, uint16_t repeat,
    uint16_t tailRow, uint32_t n, uint32_t vecLen)
{
    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> oriScaleVreg;
    Reg::RegTensor<float> f32ScaleVreg;
    Reg::RegTensor<srcT> srcVreg;
    Reg::RegTensor<float> f32Vreg;
    Reg::RegTensor<dstT> b8Vreg;
    Reg::MaskReg b32FullPreg = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    float fp32_offset = ConvertToFloat<scaleT>(offset);
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, config.roundMode};
    for (uint16_t i = 0; i < tailRow; ++i) {
        uint32_t sreg = n;
        for (uint16_t j = 0; j < repeat; ++j) {
            if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
                Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_UNPACK_B16>(oriScaleVreg, scaleUb + j * vecLen);
                Reg::Cast<float, scaleT, layoutZMrgZ>(f32ScaleVreg, oriScaleVreg, b32FullPreg);
            } else {
                Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_NORM>(f32ScaleVreg, scaleUb + j * vecLen);
            }
            preg = Reg::UpdateMask<uint32_t>(sreg);
            LoadSrc<srcT>(srcUb + i * n + j * vecLen, preg, f32Vreg);
            Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32Vreg, f32Vreg, f32ScaleVreg, preg);
            if constexpr (config.hasOffset) {
                Reg::Adds<float, float, Reg::MaskMergeMode::ZEROING>(f32Vreg, f32Vreg, fp32_offset, preg);
            }
            TransRegForFp8<dstT, float, castTrait>(f32Vreg, b8Vreg, preg);
            Reg::StoreAlign<dstT, Reg::StoreDist::DIST_PACK4_B32>(dstUb + i * n + j * vecLen, b8Vreg, preg);
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__simd_vf__ inline void QuantPerGroupForKRowFp8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, const scaleT offset,
    const AscendQuantParam para, uint16_t rowNum, uint16_t tailRow)
{
    uint16_t mainRowGroup = rowNum / para.groupSize;
    uint32_t vecLen = ASCENDC_QUANT_PER_GROUP_B32_VF_LEN;
    uint16_t repeat = CeilDivision(para.n, vecLen);
    uint32_t sreg = para.n;
    float fp32_offset = ConvertToFloat<scaleT>(offset);
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, config.roundMode};

    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> oriScaleVreg;
    Reg::RegTensor<float> f32ScaleVreg;
    Reg::RegTensor<float> f32Vreg;
    Reg::RegTensor<dstT> b8Vreg;
    Reg::MaskReg b32FullPreg = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    for (uint16_t i = 0; i < mainRowGroup; ++i) {
        for (uint16_t j = 0; j < static_cast<uint16_t>(para.groupSize); ++j) {
            sreg = para.n;
            for (uint16_t k = 0; k < repeat; ++k) {
                if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
                    Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_UNPACK_B16>(
                        oriScaleVreg, scaleUb + i * para.n + k * vecLen);
                    Reg::Cast<float, scaleT, layoutZMrgZ>(f32ScaleVreg, oriScaleVreg, b32FullPreg);
                } else {
                    Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_NORM>(f32ScaleVreg, scaleUb + i * para.n + k * vecLen);
                }
                preg = Reg::UpdateMask<uint32_t>(sreg);
                LoadSrc<srcT>(srcUb + (i * para.groupSize + j) * para.n + k * vecLen, preg, f32Vreg);
                Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32Vreg, f32Vreg, f32ScaleVreg, preg);
                if constexpr (config.hasOffset) {
                    Reg::Adds<float, float, Reg::MaskMergeMode::ZEROING>(f32Vreg, f32Vreg, fp32_offset, preg);
                }
                TransRegForFp8<dstT, float, castTrait>(f32Vreg, b8Vreg, preg);
                Reg::StoreAlign<dstT, Reg::StoreDist::DIST_PACK4_B32>(
                    dstUb + (i * para.groupSize + j) * para.n + k * vecLen, b8Vreg, preg);
            }
        }
    }
    QuantPerGroupForKRowFp8TailBlock<dstT, srcT, scaleT, config>(
        dstUb + mainRowGroup * para.groupSize * para.n, srcUb + mainRowGroup * para.groupSize * para.n,
        scaleUb + mainRowGroup * para.n, offset, repeat, tailRow, para.n, vecLen);
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__aicore__ inline void QuantPerGroupForKRowFp8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const scaleT& offset, const AscendQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    uint16_t rowNum = para.calCount / para.n;
    uint16_t tailRow = rowNum % para.groupSize;
    QuantPerGroupForKRowFp8VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, offset, para, rowNum, tailRow);
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__simd_callee__ inline void QuantPerGroupForKRowHif8TailBlock(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, const scaleT& offset, uint16_t repeat,
    uint16_t tailRow, uint32_t n, uint32_t vecLen)
{
    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<dstT> dstVreg;
    Reg::RegTensor<scaleT> srcVreg;
    Reg::RegTensor<srcT> tempVreg;
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, config.roundMode};
    for (uint16_t i = 0; i < tailRow; ++i) {
        uint32_t sreg = n;
        for (uint16_t j = 0; j < repeat; ++j) {
            Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_NORM>(scaleVreg, scaleUb + j * vecLen);
            preg = Reg::UpdateMask<scaleT>(sreg);
            if constexpr (SupportType<srcT, half, bfloat16_t>() && SupportType<scaleT, float>()) {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(tempVreg, srcUb + i * n + j * vecLen);
                Reg::Cast<float, srcT, layoutZMrgZ>(srcVreg, tempVreg, preg);
            } else {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_NORM>(srcVreg, srcUb + i * n + j * vecLen);
            }
            Reg::Mul<scaleT, Reg::MaskMergeMode::ZEROING>(srcVreg, srcVreg, scaleVreg, preg);
            if constexpr (config.hasOffset) {
                Reg::Adds<scaleT, scaleT, Reg::MaskMergeMode::ZEROING>(
                    srcVreg, srcVreg, static_cast<scaleT>(offset), preg);
            }
            TransRegForHif8<dstT, scaleT, castTrait>(srcVreg, dstVreg, preg);
            StoreRes<dstT, scaleT>(dstUb + i * n + j * vecLen, dstVreg, preg);
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__simd_vf__ inline void QuantPerGroupForKRowHif8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, const scaleT offset,
    const AscendQuantParam para, uint16_t rowNum, uint16_t tailRow)
{
    uint16_t mainRowGroup = rowNum / para.groupSize;
    uint32_t vecLen = GetVecLen() / sizeof(scaleT);
    uint16_t repeat = CeilDivision(para.n, vecLen);
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, config.roundMode};

    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<dstT> dstVreg;
    Reg::RegTensor<scaleT> srcVreg;
    Reg::RegTensor<srcT> tempVreg;
    for (uint16_t i = 0; i < mainRowGroup; ++i) {
        for (uint16_t j = 0; j < static_cast<uint16_t>(para.groupSize); ++j) {
            uint32_t sreg = para.n;
            for (uint16_t k = 0; k < repeat; ++k) {
                Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_NORM>(scaleVreg, scaleUb + i * para.n + k * vecLen);
                preg = Reg::UpdateMask<scaleT>(sreg);
                if constexpr (SupportType<srcT, half, bfloat16_t>() && SupportType<scaleT, float>()) {
                    Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(
                        tempVreg, srcUb + (i * para.groupSize + j) * para.n + k * vecLen);
                    Reg::Cast<float, srcT, layoutZMrgZ>(srcVreg, tempVreg, preg);
                } else {
                    Reg::LoadAlign<srcT, Reg::LoadDist::DIST_NORM>(
                        srcVreg, srcUb + (i * para.groupSize + j) * para.n + k * vecLen);
                }
                Reg::Mul<scaleT, Reg::MaskMergeMode::ZEROING>(srcVreg, srcVreg, scaleVreg, preg);
                if constexpr (config.hasOffset) {
                    Reg::Adds<scaleT, scaleT, Reg::MaskMergeMode::ZEROING>(
                        srcVreg, srcVreg, static_cast<scaleT>(offset), preg);
                }
                TransRegForHif8<dstT, scaleT, castTrait>(srcVreg, dstVreg, preg);
                StoreRes<dstT, scaleT>(dstUb + (i * para.groupSize + j) * para.n + k * vecLen, dstVreg, preg);
            }
        }
    }
    QuantPerGroupForKRowHif8TailBlock<dstT, srcT, scaleT, config>(
        dstUb + mainRowGroup * para.groupSize * para.n, srcUb + mainRowGroup * para.groupSize * para.n,
        scaleUb + mainRowGroup * para.n, offset, repeat, tailRow, para.n, vecLen);
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__aicore__ inline void QuantPerGroupForKRowHif8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const scaleT& offset, const AscendQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    uint16_t rowNum = para.calCount / para.n;
    uint16_t tailRow = rowNum % para.groupSize;
    QuantPerGroupForKRowHif8VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, offset, para, rowNum, tailRow);
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__simd_callee__ inline void QuantPerGroupForKRowS8TailBlock(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, const scaleT& offset, uint16_t repeat,
    uint16_t tailRow, uint32_t n, uint32_t vecLen)
{
    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<srcT> tempVreg;
    Reg::RegTensor<dstT> dstVreg;
    Reg::RegTensor<scaleT> srcVreg;
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, config.roundMode};
    for (uint16_t i = 0; i < tailRow; ++i) {
        uint32_t sreg = n;
        for (uint16_t j = 0; j < repeat; ++j) {
            Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_NORM>(scaleVreg, scaleUb + j * vecLen);
            preg = Reg::UpdateMask<scaleT>(sreg);
            if constexpr (SupportType<srcT, half, bfloat16_t>() && SupportType<scaleT, float>()) {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(tempVreg, srcUb + i * n + j * vecLen);
                Reg::Cast<float, srcT, layoutZMrgZ>(srcVreg, tempVreg, preg);
            } else {
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_NORM>(srcVreg, srcUb + i * n + j * vecLen);
            }
            Reg::Mul<scaleT, Reg::MaskMergeMode::ZEROING>(srcVreg, srcVreg, scaleVreg, preg);
            if constexpr (config.hasOffset) {
                Reg::Adds<scaleT, scaleT, Reg::MaskMergeMode::ZEROING>(
                    srcVreg, srcVreg, static_cast<scaleT>(offset), preg);
            }
            TransRegForS8<dstT, scaleT, castTrait>(srcVreg, dstVreg, preg);
            StoreRes<dstT, scaleT>(dstUb + i * n + j * vecLen, dstVreg, preg);
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__simd_vf__ inline void QuantPerGroupForKRowS8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, const scaleT offset,
    const AscendQuantParam para, uint16_t rowNum, uint16_t tailRow)
{
    uint16_t mainRowGroup = rowNum / para.groupSize;
    uint32_t vecLen = GetVecLen() / sizeof(scaleT);
    uint16_t repeat = CeilDivision(para.n, vecLen);
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, config.roundMode};

    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<srcT> tempVreg;
    Reg::RegTensor<dstT> dstVreg;
    Reg::RegTensor<scaleT> srcVreg;
    for (uint16_t i = 0; i < mainRowGroup; ++i) {
        for (uint16_t j = 0; j < static_cast<uint16_t>(para.groupSize); ++j) {
            uint32_t sreg = para.n;
            for (uint16_t k = 0; k < repeat; ++k) {
                Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_NORM>(scaleVreg, scaleUb + i * para.n + k * vecLen);
                preg = Reg::UpdateMask<scaleT>(sreg);
                if constexpr (SupportType<srcT, half, bfloat16_t>() && SupportType<scaleT, float>()) {
                    Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B16>(
                        tempVreg, srcUb + (i * para.groupSize + j) * para.n + k * vecLen);
                    Reg::Cast<float, srcT, layoutZMrgZ>(srcVreg, tempVreg, preg);
                } else {
                    Reg::LoadAlign<srcT, Reg::LoadDist::DIST_NORM>(
                        srcVreg, srcUb + (i * para.groupSize + j) * para.n + k * vecLen);
                }
                Reg::Mul<scaleT, Reg::MaskMergeMode::ZEROING>(srcVreg, srcVreg, scaleVreg, preg);
                if constexpr (config.hasOffset) {
                    Reg::Adds<scaleT, scaleT, Reg::MaskMergeMode::ZEROING>(
                        srcVreg, srcVreg, static_cast<scaleT>(offset), preg);
                }
                TransRegForS8<dstT, scaleT, castTrait>(srcVreg, dstVreg, preg);
                StoreRes<dstT, scaleT>(dstUb + (i * para.groupSize + j) * para.n + k * vecLen, dstVreg, preg);
            }
        }
    }
    QuantPerGroupForKRowS8TailBlock<dstT, srcT, scaleT, config>(
        dstUb + mainRowGroup * para.groupSize * para.n, srcUb + mainRowGroup * para.groupSize * para.n,
        scaleUb + mainRowGroup * para.n, offset, repeat, tailRow, para.n, vecLen);
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__aicore__ inline void QuantPerGroupForKRowS8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const scaleT offset, const AscendQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    uint16_t rowNum = para.calCount / para.n;
    uint16_t tailRow = rowNum % para.groupSize;
    QuantPerGroupForKRowS8VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, offset, para, rowNum, tailRow);
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__aicore__ inline void AscendQuantPerToken(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const LocalTensor<scaleT>& scaleTensor, const LocalTensor<scaleT>& offsetTensor, const AscendQuantParam& para)
{
    if constexpr (SupportType<dstT, fp8_e4m3fn_t, fp8_e5m2_t>()) {
        QuantPerTokenForFp8<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, offsetTensor, para);
    } else if constexpr (SupportType<dstT, hifloat8_t>()) {
        QuantPerTokenForHif8<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, offsetTensor, para);
    } else if constexpr (SupportType<dstT, int8_t>()) {
        QuantPerTokenForS8<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, offsetTensor, para);
    } else {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupport dstT for AscendQuant!"); });
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendQuantConfig& config>
__aicore__ inline void AscendQuantPerToken(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const LocalTensor<scaleT>& scaleTensor, const scaleT offset, const AscendQuantParam& para)
{
    if constexpr (SupportType<dstT, fp8_e4m3fn_t, fp8_e5m2_t>()) {
        QuantPerTokenForFp8<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, offset, para);
    } else if constexpr (SupportType<dstT, hifloat8_t>()) {
        QuantPerTokenForHif8<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, offset, para);
    } else if constexpr (SupportType<dstT, int8_t>()) {
        QuantPerTokenForS8<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, offset, para);
    } else {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupport dstT for AscendQuant!"); });
    }
}

template <typename dstT, typename srcT, typename scaleT, bool isReuseSource = false, const AscendQuantConfig& config>
__aicore__ inline void AscendQuantPerGroupForKCol(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const LocalTensor<scaleT>& scaleTensor, const LocalTensor<scaleT>& offsetTensor, const AscendQuantParam& para)
{
    if constexpr (SupportType<dstT, fp8_e4m3fn_t, fp8_e5m2_t>()) {
        QuantPerGroupForKColFp8<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, offsetTensor, para);
    } else if constexpr (SupportType<dstT, hifloat8_t>()) {
        QuantPerGroupForKColHif8<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, offsetTensor, para);
    } else if constexpr (SupportType<dstT, int8_t>()) {
        QuantPerGroupForKColS8<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, offsetTensor, para);
    } else if constexpr (SupportType<dstT, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
        // fp4 doesn't count offset
        QuantPerGroupForKColFp4<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, para);
    } else {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupport dstT for AscendQuant!"); });
    }
}

template <typename dstT, typename srcT, typename scaleT, bool isReuseSource = false, const AscendQuantConfig& config>
__aicore__ inline void AscendQuantPerGroupForKCol(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const LocalTensor<scaleT>& scaleTensor, const scaleT offset, const AscendQuantParam& para)
{
    if constexpr (SupportType<dstT, fp8_e4m3fn_t, fp8_e5m2_t>()) {
        QuantPerGroupForKColFp8<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, offset, para);
    } else if constexpr (SupportType<dstT, hifloat8_t>()) {
        QuantPerGroupForKColHif8<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, offset, para);
    } else if constexpr (SupportType<dstT, int8_t>()) {
        QuantPerGroupForKColS8<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, offset, para);
    } else if constexpr (SupportType<dstT, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
        // fp4 doesn't count offset
        QuantPerGroupForKColFp4<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, para);
    } else {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupport dstT for AscendQuant!"); });
    }
}

template <typename dstT, typename srcT, typename scaleT, bool isReuseSource = false, const AscendQuantConfig& config>
__aicore__ inline void AscendQuantPerGroupForKRow(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const LocalTensor<scaleT>& scaleTensor, const LocalTensor<scaleT>& offsetTensor, const AscendQuantParam& para)
{
    if constexpr (SupportType<dstT, fp8_e4m3fn_t, fp8_e5m2_t>()) {
        QuantPerGroupForKRowFp8<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, offsetTensor, para);
    } else if constexpr (SupportType<dstT, hifloat8_t>()) {
        QuantPerGroupForKRowHif8<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, offsetTensor, para);
    } else if constexpr (SupportType<dstT, int8_t>()) {
        QuantPerGroupForKRowS8<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, offsetTensor, para);
    } else if constexpr (SupportType<dstT, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
        // fp4 doesn't count offset
        QuantPerGroupForKRowFp4<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, para);
    } else {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupport dstT for AscendQuant!"); });
    }
}

template <typename dstT, typename srcT, typename scaleT, bool isReuseSource = false, const AscendQuantConfig& config>
__aicore__ inline void AscendQuantPerGroupForKRow(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const LocalTensor<scaleT>& scaleTensor, const scaleT offset, const AscendQuantParam& para)
{
    if constexpr (SupportType<dstT, fp8_e4m3fn_t, fp8_e5m2_t>()) {
        QuantPerGroupForKRowFp8<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, offset, para);
    } else if constexpr (SupportType<dstT, hifloat8_t>()) {
        QuantPerGroupForKRowHif8<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, offset, para);
    } else if constexpr (SupportType<dstT, int8_t>()) {
        QuantPerGroupForKRowS8<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, offset, para);
    } else if constexpr (SupportType<dstT, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
        // fp4 doesn't count offset
        QuantPerGroupForKRowFp4<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, para);
    } else {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupport dstT for AscendQuant!"); });
    }
}

template <
    typename dstT, typename srcT, typename scaleT, bool isReuseSource = false, const AscendQuantConfig& config,
    const AscendQuantPolicy& policy>
__aicore__ inline void AscendQuantImpl(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const LocalTensor<scaleT>& scaleTensor, const LocalTensor<scaleT>& offsetTensor, const AscendQuantParam& para)
{
    if ASCEND_IS_AIC {
        return;
    }
    CheckTensorPosition(dstTensor, "dstTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcTensor, "srcTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(scaleTensor, "scaleTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(offsetTensor, "offsetTensor", "VECIN/VECOUT/VECCALC");
    static_assert(
        SupportType<srcT, half, float, bfloat16_t>(), "AscendQuant only support half/float/bfloat16_t input dtype");
    static_assert(
        SupportType<scaleT, half, float, bfloat16_t>(), "AscendQuant only support half/float/bfloat16_t scale dtype");
    static_assert(
        (policy == AscendQuantPolicy::PER_TOKEN || policy == AscendQuantPolicy::PER_GROUP),
        "unsupported policy for AscendQuant in current device!");
    ASCENDC_ASSERT(
        (para.calCount <= srcTensor.GetSize() && para.calCount <= dstTensor.GetSize() && para.calCount >= 0), {
            KERNEL_LOG(
                KERNEL_ERROR, "calCount is %u, which should be in [0, min(%u, %u)]", para.calCount, srcTensor.GetSize(),
                dstTensor.GetSize());
        });
    ASCENDC_ASSERT(
        (para.calCount % para.n == 0), { KERNEL_LOG(KERNEL_ERROR, "calCount must be an integer multiple of n!"); });
    if constexpr (policy == AscendQuantPolicy::PER_TOKEN) {
        static_assert(
            SupportType<dstT, int8_t, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t>(),
            "AscendQuant PerToken only support int8_t/fp8_e4m3fn_t/fp8_e5m2_t/hifloat8_t output dtype");
        AscendQuantPerToken<dstT, srcT, scaleT, config>(
            dstTensor, srcTensor, sharedTmpBuffer, scaleTensor, offsetTensor, para);
    } else if constexpr (policy == AscendQuantPolicy::PER_GROUP) {
        static_assert(
            SupportType<dstT, int8_t, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t, fp4x2_e2m1_t, fp4x2_e1m2_t>(),
            "AscendQuant PerGroup only support "
            "int8_t/fp8_e4m3fn_t/fp8_e5m2_t/hifloat8_t/fp4x2_e2m1_t/fp4x2_e1m2_t output dtype");
        static_assert(
            ((config.kDim == 1) || (config.kDim == 0)), "AscendAntiQuant PerGroup only support K is axis 0/1!");
        ASCENDC_ASSERT((para.groupSize > 0 && para.groupSize % 32 == 0), {
            KERNEL_LOG(KERNEL_ERROR, "groupSize must be an integer multiple of 32 and greater than 0 !");
        });
        if constexpr (config.kDim == 1) {
            AscendQuantPerGroupForKCol<dstT, srcT, scaleT, isReuseSource, config>(
                dstTensor, srcTensor, sharedTmpBuffer, scaleTensor, offsetTensor, para);
        } else {
            AscendQuantPerGroupForKRow<dstT, srcT, scaleT, isReuseSource, config>(
                dstTensor, srcTensor, sharedTmpBuffer, scaleTensor, offsetTensor, para);
        }
    }
}

template <
    typename dstT, typename srcT, typename scaleT, bool isReuseSource = false, const AscendQuantConfig& config,
    const AscendQuantPolicy& policy>
__aicore__ inline void AscendQuantImpl(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const LocalTensor<scaleT>& scaleTensor, const scaleT offset, const AscendQuantParam& para)
{
    if ASCEND_IS_AIC {
        return;
    }
    CheckTensorPosition(dstTensor, "dstTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcTensor, "srcTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(scaleTensor, "scaleTensor", "VECIN/VECOUT/VECCALC");
    static_assert(
        SupportType<srcT, half, float, bfloat16_t>(), "AscendQuant only support half/float/bfloat16_t input dtype");
    static_assert(
        SupportType<scaleT, half, float, bfloat16_t>(), "AscendQuant only support half/float/bfloat16_t scale dtype");
    static_assert(
        (policy == AscendQuantPolicy::PER_TOKEN || policy == AscendQuantPolicy::PER_GROUP),
        "unsupported policy for AscendQuant in current device!");
    ASCENDC_ASSERT(
        (para.calCount <= srcTensor.GetSize() && para.calCount <= dstTensor.GetSize() && para.calCount >= 0), {
            KERNEL_LOG(
                KERNEL_ERROR, "calCount is %u, which should be in [0, min(%u, %u)]", para.calCount, srcTensor.GetSize(),
                dstTensor.GetSize());
        });
    ASCENDC_ASSERT(
        (para.calCount % para.n == 0), { KERNEL_LOG(KERNEL_ERROR, "calCount must be an integer multiple of n!"); });
    if constexpr (policy == AscendQuantPolicy::PER_TOKEN) {
        static_assert(
            SupportType<dstT, int8_t, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t>(),
            "AscendQuant PerToken only support int8_t/fp8_e4m3fn_t/fp8_e5m2_t/hifloat8_t output dtype");
        AscendQuantPerToken<dstT, srcT, scaleT, config>(
            dstTensor, srcTensor, sharedTmpBuffer, scaleTensor, offset, para);
    } else if constexpr (policy == AscendQuantPolicy::PER_GROUP) {
        static_assert(
            SupportType<dstT, int8_t, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t, fp4x2_e2m1_t, fp4x2_e1m2_t>(),
            "AscendQuant PerGroup only support "
            "int8_t/fp8_e4m3fn_t/fp8_e5m2_t/hifloat8_t/fp4x2_e2m1_t/fp4x2_e1m2_t output dtype");
        static_assert(
            ((config.kDim == 1) || (config.kDim == 0)), "AscendAntiQuant PerGroup only support K is axis 0/1!");
        ASCENDC_ASSERT((para.groupSize > 0 && para.groupSize % 32 == 0), {
            KERNEL_LOG(KERNEL_ERROR, "groupSize must be an integer multiple of 32 and greater than 0 !");
        });
        if constexpr (config.kDim == 1) {
            AscendQuantPerGroupForKCol<dstT, srcT, scaleT, isReuseSource, config>(
                dstTensor, srcTensor, sharedTmpBuffer, scaleTensor, offset, para);
        } else {
            AscendQuantPerGroupForKRow<dstT, srcT, scaleT, isReuseSource, config>(
                dstTensor, srcTensor, sharedTmpBuffer, scaleTensor, offset, para);
        }
    }
}
} //  namespace AscendC
#endif // LIB_ASCEND_QUANT_ASCEND_QUANT_PER_GROUP_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_QUANT_ASCEND_QUANT_PER_GROUP_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_QUANT_ASCEND_QUANT_PER_GROUP_C310_IMPL_H__
#endif
