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
 * \file antiquantize_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/quantization/antiquantize/antiquantize_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/quantization/antiquantize.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_ANTIQUANTIZE_ANTIQUANTIZE_C310_IMPL_H__
#endif

#ifndef IMPL_QUANTIZATION_ANTIQUANTIZE_C310_IMPL_H
#define IMPL_QUANTIZATION_ANTIQUANTIZE_C310_IMPL_H

#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../basic_api/kernel_pop_stack_buffer.h"
#include "antiquantize_common.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102)
#include "../antiquant/ascend_antiquant_3510_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../antiquant/ascend_antiquant_l300_impl.h"
#endif

namespace AscendC {
namespace AntiQuantizeUtils {
template <typename SrcT>
__simd_callee__ inline void CastAntiQuantizeSrcToFp32(
    Reg::RegTensor<float>& f32SrcVreg, Reg::RegTensor<SrcT>& srcVreg, Reg::RegTensor<half>& f16Vreg, Reg::MaskReg& preg)
{
    if constexpr (SupportType<SrcT, fp8_e4m3fn_t, fp8_e5m2_t>()) {
        Reg::Cast<float, SrcT, layoutZMrgZ>(f32SrcVreg, srcVreg, preg);
    } else {
        Reg::Cast<half, SrcT, layoutZMrgZ>(f16Vreg, srcVreg, preg);
        Reg::Cast<float, half, layoutZMrgZ>(f32SrcVreg, f16Vreg, preg);
    }
}
} // namespace AntiQuantizeUtils

template <typename dstT, typename SrcT, typename scaleT, const AscendAntiQuantConfig& config>
__simd_vf__ inline void AntiQuantizePerGroupForColCommonVF(
    __ubuf__ dstT* dstUb, __ubuf__ SrcT* srcUb, __ubuf__ scaleT* scaleUb, __ubuf__ scaleT* offsetUb,
    const AscendAntiQuantParam para)
{
    uint16_t rowNum = para.calCount / para.n;
    uint32_t vecLen = ASCENDC_QUANT_B32_VF_LEN;
    uint16_t repeat = CeilDivision(para.n, vecLen);
    uint32_t sreg = para.n;
    uint16_t scaleK = CeilDivision(para.n, para.groupSize);

    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> offsetVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<SrcT> srcVreg;
    Reg::RegTensor<half> f16Vreg;
    Reg::RegTensor<float> f32SrcVreg;
    Reg::RegTensor<float> f32ScaleVreg;
    Reg::RegTensor<float> f32OffsetVreg;
    for (uint16_t i = 0; i < rowNum; ++i) {
        sreg = para.n;
        for (uint16_t j = 0; j < repeat; ++j) {
            preg = Reg::UpdateMask<uint32_t>(sreg);
            GetPerGroupScaleAndOffset<scaleT, config>(
                scaleUb + i * scaleK, offsetUb + i * scaleK, j * vecLen, para, scaleVreg, offsetVreg);
            Reg::LoadAlign<SrcT, Reg::LoadDist::DIST_UNPACK4_B8>(srcVreg, srcUb + i * para.n + j * vecLen);
            AntiQuantizeUtils::CastAntiQuantizeSrcToFp32<SrcT>(f32SrcVreg, srcVreg, f16Vreg, preg);
            ConvertToF32ScaleAndOffset<scaleT, config>(scaleVreg, offsetVreg, preg, f32ScaleVreg, f32OffsetVreg);
            if constexpr (SupportType<scaleT, float>()) {
                AddOffsetIfExist<float, config>(f32SrcVreg, offsetVreg, preg);
                Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32SrcVreg, f32SrcVreg, scaleVreg, preg);
            } else {
                ConvertToF32ScaleAndOffset<scaleT, config>(scaleVreg, offsetVreg, preg, f32ScaleVreg, f32OffsetVreg);
                AddOffsetIfExist<float, config>(f32SrcVreg, f32OffsetVreg, preg);
                Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32SrcVreg, f32SrcVreg, f32ScaleVreg, preg);
            }
            StoreF32Res<dstT>((dstUb + i * para.n + j * vecLen), f32SrcVreg, preg);
        }
    }
}
/* **************************************************************************************************
 * PER_GROUP for B8                                             *
 * ************************************************************************************************* */
template <typename dstT, typename SrcT, typename scaleT, const AscendAntiQuantConfig& config>
__aicore__ inline void AntiQuantizePerGroupForColCommon(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const LocalTensor<scaleT>& offsetTensor, const AscendAntiQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ SrcT* srcUb = (__ubuf__ SrcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    __ubuf__ scaleT* offsetUb = (__ubuf__ scaleT*)offsetTensor.GetPhyAddr();
    AntiQuantizePerGroupForColCommonVF<dstT, SrcT, scaleT, config>(dstUb, srcUb, scaleUb, offsetUb, para);
}

template <typename dstT, typename SrcT, typename scaleT, const AscendAntiQuantConfig& config>
__simd_callee__ inline void AntiQuantizePerGroupForRowCommonTailBlock(
    __ubuf__ dstT* dstUb, __ubuf__ SrcT* srcUb, __ubuf__ scaleT* scaleUb, __ubuf__ scaleT* offsetUb, uint16_t repeat,
    uint16_t tailRow, uint32_t n, uint32_t vecLen)
{
    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> offsetVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<SrcT> srcVreg;
    Reg::RegTensor<float> f32ScaleVreg;
    Reg::RegTensor<float> f32OffsetVreg;
    Reg::RegTensor<float> f32SrcVreg;
    Reg::RegTensor<half> f16Vreg;
    for (uint16_t i = 0; i < tailRow; ++i) {
        uint32_t sreg = n;
        for (uint16_t j = 0; j < repeat; ++j) {
            LoadNormScaleAndOffset<scaleT, config>(
                (scaleUb + j * vecLen), (offsetUb + j * vecLen), scaleVreg, offsetVreg);
            preg = Reg::UpdateMask<uint32_t>(sreg);
            Reg::LoadAlign<SrcT, Reg::LoadDist::DIST_UNPACK4_B8>(srcVreg, srcUb + i * n + j * vecLen);
            AntiQuantizeUtils::CastAntiQuantizeSrcToFp32<SrcT>(f32SrcVreg, srcVreg, f16Vreg, preg);
            if constexpr (SupportType<scaleT, float>()) {
                AddOffsetIfExist<float, config>(f32SrcVreg, offsetVreg, preg);
                Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32SrcVreg, f32SrcVreg, scaleVreg, preg);
            } else {
                ConvertToF32ScaleAndOffset<scaleT, config>(scaleVreg, offsetVreg, preg, f32ScaleVreg, f32OffsetVreg);
                AddOffsetIfExist<float, config>(f32SrcVreg, f32OffsetVreg, preg);
                Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32SrcVreg, f32SrcVreg, f32ScaleVreg, preg);
            }
            StoreF32Res<dstT>((dstUb + i * n + j * vecLen), f32SrcVreg, preg);
        }
    }
}

template <typename dstT, typename SrcT, typename scaleT, const AscendAntiQuantConfig& config>
__simd_vf__ inline void AntiQuantizePerGroupForRowCommonVF(
    __ubuf__ dstT* dstUb, __ubuf__ SrcT* srcUb, __ubuf__ scaleT* scaleUb, __ubuf__ scaleT* offsetUb,
    const AscendAntiQuantParam para, uint16_t rowNum, uint16_t tailRow)
{
    uint16_t mainRowGroup = rowNum / para.groupSize;
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    uint32_t vecLen = ANTIQUANT_B32_VF_LEN;
#else
    uint32_t vecLen = ASCENDC_QUANT_B32_VF_LEN;
#endif
    uint16_t repeat = CeilDivision(para.n, vecLen);

    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> offsetVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<SrcT> srcVreg;
    Reg::RegTensor<float> f32Svreg;
    Reg::RegTensor<float> f32Ovreg;
    Reg::RegTensor<float> f32SrcVreg;
    Reg::RegTensor<half> f16Vreg;
    for (uint16_t i = 0; i < mainRowGroup; ++i) {
        for (uint16_t j = 0; j < static_cast<uint16_t>(para.groupSize); ++j) {
            uint32_t sreg = para.n;
            for (uint16_t k = 0; k < repeat; ++k) {
                preg = Reg::UpdateMask<uint32_t>(sreg);
                LoadNormScaleAndOffset<scaleT, config>(
                    (scaleUb + i * para.n + k * vecLen), (offsetUb + i * para.n + k * vecLen), scaleVreg, offsetVreg);
                Reg::LoadAlign<SrcT, Reg::LoadDist::DIST_UNPACK4_B8>(
                    srcVreg, (srcUb + (i * para.groupSize + j) * para.n + k * vecLen));
                AntiQuantizeUtils::CastAntiQuantizeSrcToFp32<SrcT>(f32SrcVreg, srcVreg, f16Vreg, preg);
                if constexpr (SupportType<scaleT, float>()) {
                    AddOffsetIfExist<float, config>(f32SrcVreg, offsetVreg, preg);
                    Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32SrcVreg, f32SrcVreg, scaleVreg, preg);
                } else {
                    ConvertToF32ScaleAndOffset<scaleT, config>(scaleVreg, offsetVreg, preg, f32Svreg, f32Ovreg);
                    AddOffsetIfExist<float, config>(f32SrcVreg, f32Ovreg, preg);
                    Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32SrcVreg, f32SrcVreg, f32Svreg, preg);
                }
                StoreF32Res<dstT>((dstUb + (i * para.groupSize + j) * para.n + k * vecLen), f32SrcVreg, preg);
            }
        }
    }
    AntiQuantizePerGroupForRowCommonTailBlock<dstT, SrcT, scaleT, config>(
        dstUb + mainRowGroup * para.groupSize * para.n, srcUb + mainRowGroup * para.groupSize * para.n,
        scaleUb + mainRowGroup * para.n, offsetUb + mainRowGroup * para.n, repeat, tailRow, para.n, vecLen);
}

template <typename dstT, typename SrcT, typename scaleT, const AscendAntiQuantConfig& config>
__aicore__ inline void AntiQuantizePerGroupForRowCommon(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const LocalTensor<scaleT>& offsetTensor, const AscendAntiQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ SrcT* srcUb = (__ubuf__ SrcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    __ubuf__ scaleT* offsetUb = (__ubuf__ scaleT*)offsetTensor.GetPhyAddr();
    uint16_t rowNum = para.calCount / para.n;
    uint16_t tailRow = rowNum % para.groupSize;
    AntiQuantizePerGroupForRowCommonVF<dstT, SrcT, scaleT, config>(
        dstUb, srcUb, scaleUb, offsetUb, para, rowNum, tailRow);
}

template <typename dstT, typename SrcT, typename scaleT, const AscendAntiQuantConfig& config>
__simd_vf__ inline void AntiQuantizePerTokenCommonVF(
    __ubuf__ dstT* dstUb, __ubuf__ SrcT* srcUb, __ubuf__ scaleT* scaleUb, __ubuf__ scaleT* offsetUb,
    const AscendAntiQuantParam para)
{
    uint16_t rowNum = para.calCount / para.n;
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    uint32_t vecLen = ANTIQUANT_B32_VF_LEN;
#else
    uint32_t vecLen = ASCENDC_QUANT_B32_VF_LEN;
#endif
    uint16_t repeat = CeilDivision(para.n, vecLen);
    uint32_t sreg = para.n;

    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> offsetVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<SrcT> srcVreg;
    Reg::RegTensor<float> f32Vreg;
    Reg::RegTensor<float> f32ScaleVreg;
    Reg::RegTensor<float> f32OffsetVreg;
    Reg::RegTensor<half> f16Vreg;
    for (uint16_t i = 0; i < rowNum; ++i) {
        LoadPerTokenScaleAndOffset<scaleT, config>(scaleUb + i, offsetUb + i, scaleVreg, offsetVreg);
        sreg = para.n;
        for (uint16_t j = 0; j < repeat; ++j) {
            preg = Reg::UpdateMask<uint32_t>(sreg);
            Reg::LoadAlign<SrcT, Reg::LoadDist::DIST_UNPACK4_B8>(srcVreg, srcUb + i * para.n + j * vecLen);
            AntiQuantizeUtils::CastAntiQuantizeSrcToFp32<SrcT>(f32Vreg, srcVreg, f16Vreg, preg);

            if constexpr (SupportType<scaleT, float>()) {
                AddOffsetIfExist<float, config>(f32Vreg, offsetVreg, preg);
                Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32Vreg, f32Vreg, scaleVreg, preg);
            } else {
                ConvertToF32ScaleAndOffset<scaleT, config>(scaleVreg, offsetVreg, preg, f32ScaleVreg, f32OffsetVreg);
                AddOffsetIfExist<float, config>(f32Vreg, f32OffsetVreg, preg);
                Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32Vreg, f32Vreg, f32ScaleVreg, preg);
            }
            StoreF32Res<dstT>((dstUb + i * para.n + j * vecLen), f32Vreg, preg);
        }
    }
}

/* **************************************************************************************************
 * PER_TOKEN for B8                                             *
 * ************************************************************************************************* */
template <typename dstT, typename SrcT, typename scaleT, const AscendAntiQuantConfig& config>
__aicore__ inline void AntiQuantizePerTokenCommon(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const LocalTensor<scaleT>& offsetTensor, const AscendAntiQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ SrcT* srcUb = (__ubuf__ SrcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    __ubuf__ scaleT* offsetUb = (__ubuf__ scaleT*)offsetTensor.GetPhyAddr();
    AntiQuantizePerTokenCommonVF<dstT, SrcT, scaleT, config>(dstUb, srcUb, scaleUb, offsetUb, para);
}

/* **************************************************************************************************
 * PER_TENSOR for B8                                             *
 * ************************************************************************************************* */
template <bool hasOffset, typename SrcT, typename DstT>
__simd_vf__ inline void PerTensorProcessCommon(
    __ubuf__ DstT* dst, __ubuf__ SrcT* src, const DstT offset, const DstT scale, const uint32_t srcCalCount)
{
    Reg::MaskReg preg;
    Reg::RegTensor<SrcT> vreg;
    Reg::RegTensor<float> f32Vreg;
    Reg::RegTensor<DstT> outReg;
    Reg::RegTensor<half> f16Vreg;

    uint32_t sregLower = (uint32_t)ANTIQUANT_B32_VF_LEN;
    uint32_t sreg = (uint32_t)srcCalCount;
    uint16_t repeat = CeilDivision(srcCalCount, sregLower);

    for (uint16_t i = 0; i < (uint16_t)repeat; ++i) {
        preg = Reg::UpdateMask<uint32_t>(sreg);
        Reg::LoadAlign<SrcT, Reg::LoadDist::DIST_UNPACK4_B8>(vreg, src + i * sregLower);
        AntiQuantizeUtils::CastAntiQuantizeSrcToFp32<SrcT>(f32Vreg, vreg, f16Vreg, preg);
        if constexpr (SupportType<DstT, bfloat16_t>()) {
            if constexpr (hasOffset) {
                Reg::Adds<float, float, Reg::MaskMergeMode::ZEROING>(f32Vreg, f32Vreg, ToFloat(offset), preg);
            }
            Reg::Muls<float, float, Reg::MaskMergeMode::ZEROING>(f32Vreg, f32Vreg, ToFloat(scale), preg);
        } else {
            if constexpr (hasOffset) {
                Reg::Adds<float, float, Reg::MaskMergeMode::ZEROING>(
                    f32Vreg, f32Vreg, static_cast<float>(offset), preg);
            }
            Reg::Muls<float, float, Reg::MaskMergeMode::ZEROING>(f32Vreg, f32Vreg, static_cast<float>(scale), preg);
        }

        Reg::Cast<DstT, float, LayoutZMrgZRndRSatS>(outReg, f32Vreg, preg);
        Reg::StoreAlign<DstT, Reg::StoreDist::DIST_PACK_B32>(dst + i * sregLower, outReg, preg);
    }
}

template <const AntiQuantizeConfig& config, typename DstT, typename SrcT>
__aicore__ inline void AntiQuantizePerTensor(
    const LocalTensor<DstT>& dst, const LocalTensor<SrcT>& src, const DstT offset, const DstT scale,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const AntiQuantizeParams& params)
{
    static_assert(
        SupportType<
            SrcT, fp8_e4m3fn_t, fp8_e5m2_t, int8_t
#if !(defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113))
            ,
            hifloat8_t
#endif
            >(),
        "This AntiQuantize only support fp8_e4m3fn_t/fp8_e5m2_t/hifloat8_t/int8_t input dtype");
    static_assert(SupportType<DstT, half, bfloat16_t>(), "This AntiQuantize only support half/bfloat16_t output dtype");
    __ubuf__ DstT* dstUb = (__ubuf__ DstT*)dst.GetPhyAddr();
    __ubuf__ SrcT* srcUb = (__ubuf__ SrcT*)src.GetPhyAddr();
    auto tmpbuffer = sharedTmpBuffer.ReinterpretCast<DstT>();
    __ubuf__ DstT* tmpbufferUb = (__ubuf__ DstT*)tmpbuffer.GetPhyAddr();

    PerTensorProcessCommon<config.hasOffset, SrcT, DstT>(dstUb, srcUb, offset, scale, params.m * params.n);
}

/* **************************************************************************************************
 * PER_CHANNEL for B8                                             *
 * ************************************************************************************************* */
template <bool hasOffset, typename SrcT, typename DstT>
__simd_vf__ inline void PerChannelNoTransposeCommon(
    __ubuf__ DstT* dst, __ubuf__ SrcT* src, __ubuf__ DstT* offset, __ubuf__ DstT* scale, const uint32_t M,
    const uint32_t N)
{
    Reg::MaskReg preg;
    Reg::RegTensor<SrcT> vreg;
    Reg::RegTensor<float> f32Vreg;
    Reg::RegTensor<DstT> outReg;
    Reg::RegTensor<DstT> scaleB16Vreg;
    Reg::RegTensor<DstT> offsetB16Vreg;
    Reg::RegTensor<float> scaleB32Vreg;
    Reg::RegTensor<float> offsetB32Vreg;
    Reg::RegTensor<half> f16Vreg;
    uint32_t sregLower = ANTIQUANT_B32_VF_LEN;
    uint32_t sreg = N;
    uint16_t repeat = CeilDivision(N, sregLower);

    for (uint16_t i = 0; i < repeat; ++i) {
        preg = Reg::UpdateMask<uint32_t>(sreg);
        // load offset and scale ,then cast to float to add &&mul
        if constexpr (hasOffset) {
            Reg::LoadAlign<DstT, Reg::LoadDist::DIST_UNPACK_B16>(offsetB16Vreg, offset + i * sregLower);
            Reg::Cast<float, DstT, layoutZMrgZ>(offsetB32Vreg, offsetB16Vreg, preg); // b16->fp32
        }
        Reg::LoadAlign<DstT, Reg::LoadDist::DIST_UNPACK_B16>(scaleB16Vreg, scale + i * sregLower);
        Reg::Cast<float, DstT, layoutZMrgZ>(scaleB32Vreg, scaleB16Vreg, preg); // b16->fp32

        for (uint16_t j = 0; j < (uint16_t)M; ++j) {
            Reg::LoadAlign<SrcT, Reg::LoadDist::DIST_UNPACK4_B8>(vreg, src + j * N + i * sregLower);
            AntiQuantizeUtils::CastAntiQuantizeSrcToFp32<SrcT>(f32Vreg, vreg, f16Vreg, preg);
            if constexpr (hasOffset) {
                Reg::Add(f32Vreg, f32Vreg, offsetB32Vreg, preg);
            }
            Reg::Mul(f32Vreg, f32Vreg, scaleB32Vreg, preg);

            Reg::Cast<DstT, float, LayoutZMrgZRndRSatS>(outReg, f32Vreg, preg);
            Reg::StoreAlign<DstT, Reg::StoreDist::DIST_PACK_B32>(dst + j * N + i * sregLower, outReg, preg);
        }
    }
}

template <bool hasOffset, typename SrcT, typename DstT>
__aicore__ inline void AntiQuantPerChannelNoTranspose(
    const LocalTensor<DstT>& dst, const LocalTensor<SrcT>& src, const LocalTensor<DstT>& offset,
    const LocalTensor<DstT>& scale, const uint32_t M, const uint32_t N)
{
    __ubuf__ DstT* scaleUb = (__ubuf__ DstT*)scale.GetPhyAddr();
    __ubuf__ DstT* offsetUb = (__ubuf__ DstT*)offset.GetPhyAddr();
    __ubuf__ DstT* dstUb = (__ubuf__ DstT*)dst.GetPhyAddr();
    __ubuf__ SrcT* srcUb = (__ubuf__ SrcT*)src.GetPhyAddr();
    PerChannelNoTransposeCommon<hasOffset, SrcT, DstT>(dstUb, srcUb, offsetUb, scaleUb, M, N);
}

/* **************************************************************************************************
 * PER_CHANNEL for B8                                             *
 * ************************************************************************************************* */
template <const AntiQuantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
__aicore__ inline void AntiQuantizePerChannel(
    const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const AntiQuantizeParams& params)
{
    static_assert(SupportType<DstT, half, bfloat16_t>(), "This AntiQuantize only support half/bfloat16_t output dtype");

    AntiQuantPerChannelNoTranspose<config.hasOffset, SrcT, DstT>(
        dstTensor, srcTensor, offset, scale, params.m, params.n);
}
} // namespace AscendC
#endif // IMPL_QUANTIZATION_ANTIQUANTIZE_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_ANTIQUANTIZE_ANTIQUANTIZE_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_ANTIQUANTIZE_ANTIQUANTIZE_C310_IMPL_H__
#endif
