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
 * \file ascend_dequant_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/quantization/dequant/ascend_dequant_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/quantization/ascend_dequant.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_DEQUANT_ASCEND_DEQUANT_C310_IMPL_H__
#endif

#ifndef LIB_ASCEND_DEQUANT_ASCEND_DEQUANT_C310_IMPL_H
#define LIB_ASCEND_DEQUANT_ASCEND_DEQUANT_C310_IMPL_H
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "kernel_tiling/kernel_tiling.h"
#include "ascend_dequant_common.h"
#include "../../quantization/quant/ascend_quant_3510_impl.h"
#include "../../common/check.h"

namespace AscendC {
constexpr uint32_t ASCENDC_DEQUANT_B32_VF_LEN = GetVecLen() / sizeof(uint32_t);
template <typename dstT, typename scaleT, DeQuantMode mode>
__simd_vf__ inline void DequantPerchannelVFImpl(
    __ubuf__ half* dstUb, __ubuf__ int32_t* srcUb, __ubuf__ float* scaleUb, DequantParams params)
{
    uint32_t rowNum = params.m;
    uint32_t N = params.n;
    uint32_t calCount = params.calCount;
    uint32_t oneBlockNum = ONE_BLK_SIZE / sizeof(dstT);
    uint32_t dstInner = CeilDivision(N, oneBlockNum) * oneBlockNum;

    Reg::MaskReg preg;
    Reg::RegTensor<int32_t> s32vreg;
    Reg::RegTensor<float> f32vreg;
    Reg::RegTensor<half> b16vreg;
    Reg::RegTensor<float> scaleB32Vreg0;
    Reg::RegTensor<float> scaleB32Vreg1;

    uint32_t sregLower = ASCENDC_DEQUANT_B32_VF_LEN;
    uint16_t repeat = static_cast<uint16_t>(CeilDivision(calCount, sregLower));

    for (uint16_t i = 0; i < static_cast<uint16_t>(rowNum); ++i) {
        uint32_t sreg = calCount;
        for (uint16_t j = 0; j < repeat; ++j) {
            preg = Reg::UpdateMask<uint32_t>(sreg);
            Reg::LoadAlign<int32_t, Reg::LoadDist::DIST_NORM>(s32vreg, srcUb + i * N + j * sregLower);
            Reg::Cast<float, int32_t, MrgZRndA>(f32vreg, s32vreg, preg);

            Reg::LoadAlign<float, Reg::LoadDist::DIST_DINTLV_B32>(
                scaleB32Vreg0, scaleB32Vreg1,
                scaleUb + 2 * j * sregLower); // only half of uint64_t is used

            Reg::Mul(f32vreg, f32vreg, scaleB32Vreg0, preg);

            Reg::Cast<dstT, float, LayoutZMrgZRndRSatS>(b16vreg, f32vreg, preg);
            Reg::StoreAlign<dstT, Reg::StoreDist::DIST_PACK_B32>(dstUb + i * dstInner + j * sregLower, b16vreg, preg);
        }
    }
}
template <typename dstT, typename scaleT, DeQuantMode mode>
__aicore__ inline void DequantPerchannelImpl(
    const LocalTensor<half>& dstTensor, const LocalTensor<int32_t>& srcTensor, const LocalTensor<uint64_t>& deqScale,
    DequantParams& params)
{
    CheckTensorPosition(dstTensor, "dstTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcTensor, "srcTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(deqScale, "deqScale", "VECIN/VECOUT/VECCALC");
    __ubuf__ half* dstUb = (__ubuf__ half*)dstTensor.GetPhyAddr();
    __ubuf__ int32_t* srcUb = (__ubuf__ int32_t*)srcTensor.GetPhyAddr();
    __ubuf__ float* scaleUb = reinterpret_cast<__ubuf__ float*>(deqScale.GetPhyAddr());

    DequantPerchannelVFImpl<dstT, scaleT, mode>(dstUb, srcUb, scaleUb, params);
}

template <typename dstT, typename scaleT, DeQuantMode mode>
__simd_vf__ inline void DequantPerchannelVFImpl(
    __ubuf__ dstT* dstUb, __ubuf__ int32_t* srcUb, __ubuf__ scaleT* scaleUb, DequantParams params)
{
    uint32_t rowNum = params.m;
    uint32_t N = params.n;
    uint32_t calCount = params.calCount;
    uint32_t oneBlockNum = ONE_BLK_SIZE / sizeof(dstT);
    uint32_t dstInner = CeilDivision(N, oneBlockNum) * oneBlockNum;

    Reg::MaskReg preg;
    Reg::RegTensor<int32_t> s32vreg;
    Reg::RegTensor<float> f32vreg;
    Reg::RegTensor<dstT> b16vreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<float> scaleB32Vreg;

    uint32_t sregLower = ASCENDC_DEQUANT_B32_VF_LEN;
    uint16_t repeat = static_cast<uint16_t>(CeilDivision(calCount, sregLower));

    for (uint16_t i = 0; i < static_cast<uint16_t>(rowNum); ++i) {
        uint32_t sreg = calCount;
        for (uint16_t j = 0; j < repeat; ++j) {
            preg = Reg::UpdateMask<uint32_t>(sreg);
            Reg::LoadAlign<int32_t, Reg::LoadDist::DIST_NORM>(s32vreg, srcUb + i * N + j * sregLower);

            Reg::Cast<float, int32_t, MrgZRndA>(f32vreg, s32vreg, preg);
            if constexpr (SupportType<scaleT, bfloat16_t>()) {
                Reg::LoadAlign<bfloat16_t, Reg::LoadDist::DIST_UNPACK_B16>(scaleVreg, scaleUb + j * sregLower);
                Reg::Cast<float, bfloat16_t, layoutZMrgZ>(scaleB32Vreg, scaleVreg, preg); // bf16->fp32
            } else {
                Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(scaleB32Vreg, scaleUb + j * sregLower);
            }

            Reg::Mul(f32vreg, f32vreg, scaleB32Vreg, preg);

            if constexpr (SupportType<dstT, bfloat16_t, half>()) {
                Reg::Cast<dstT, float, LayoutZMrgZRndRSatS>(b16vreg, f32vreg, preg);
                Reg::StoreAlign<dstT, Reg::StoreDist::DIST_PACK_B32>(
                    dstUb + i * dstInner + j * sregLower, b16vreg, preg);
            } else { // out is fp32
                Reg::StoreAlign<float, Reg::StoreDist::DIST_NORM_B32>(
                    dstUb + i * dstInner + j * sregLower, f32vreg, preg);
            }
        }
    }
}

template <typename dstT, typename scaleT, DeQuantMode mode>
__aicore__ inline void DequantPerchannelImpl(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const LocalTensor<scaleT>& deqScale,
    DequantParams& params)
{
    CheckTensorPosition(dstTensor, "dstTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcTensor, "srcTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(deqScale, "deqScale", "VECIN/VECOUT/VECCALC");
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ int32_t* srcUb = (__ubuf__ int32_t*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)deqScale.GetPhyAddr();

    DequantPerchannelVFImpl<dstT, scaleT, mode>(dstUb, srcUb, scaleUb, params);
}

template <typename dstT, typename scaleT, DeQuantMode mode>
__simd_vf__ inline void DequantPertensorVFImpl(
    __ubuf__ dstT* dstUb, __ubuf__ int32_t* srcUb, const scaleT deqScale, DequantParams params)
{
    uint32_t rowNum = params.m;
    uint32_t N = params.n;
    uint32_t calCount = params.calCount;
    uint32_t oneBlockNum = ONE_BLK_SIZE / sizeof(dstT);
    uint32_t dstInner = CeilDivision(N, oneBlockNum) * oneBlockNum;

    Reg::MaskReg preg;
    Reg::RegTensor<int32_t> s32vreg;
    Reg::RegTensor<float> f32vreg;
    Reg::RegTensor<bfloat16_t> b16vreg;

    uint32_t sregLower = ASCENDC_DEQUANT_B32_VF_LEN;
    uint16_t repeat = static_cast<uint16_t>(CeilDivision(calCount, sregLower));

    for (uint16_t i = 0; i < static_cast<uint16_t>(rowNum); ++i) {
        uint32_t sreg = calCount;
        for (uint16_t j = 0; j < repeat; ++j) {
            preg = Reg::UpdateMask<uint32_t>(sreg);
            Reg::LoadAlign<int32_t, Reg::LoadDist::DIST_NORM>(s32vreg, srcUb + i * N + j * sregLower);
            Reg::Cast<float, int32_t, MrgZRndA>(f32vreg, s32vreg, preg);
            if constexpr (SupportType<scaleT, bfloat16_t>()) {
                Reg::Muls(f32vreg, f32vreg, ToFloat(deqScale), preg);
            } else {
                Reg::Muls(f32vreg, f32vreg, deqScale, preg);
            }

            if constexpr (SupportType<dstT, bfloat16_t>()) {
                Reg::Cast<bfloat16_t, float, LayoutZMrgZRndRSatS>(b16vreg, f32vreg, preg);
                Reg::StoreAlign<bfloat16_t, Reg::StoreDist::DIST_PACK_B32>(
                    dstUb + i * dstInner + j * sregLower, b16vreg, preg);
            } else { // out is fp32
                Reg::StoreAlign<float, Reg::StoreDist::DIST_NORM_B32>(
                    dstUb + i * dstInner + j * sregLower, f32vreg, preg);
            }
        }
    }
}

template <typename dstT, typename scaleT, DeQuantMode mode>
__aicore__ inline void DequantPertensorImpl(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const scaleT deqScale,
    DequantParams& params)
{
    CheckTensorPosition(dstTensor, "dstTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcTensor, "srcTensor", "VECIN/VECOUT/VECCALC");
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ int32_t* srcUb = (__ubuf__ int32_t*)srcTensor.GetPhyAddr();
    DequantPertensorVFImpl<dstT, scaleT, mode>(dstUb, srcUb, deqScale, params);
}

template <typename scaleT>
__simd_callee__ inline void LoadPerTokenScale(__ubuf__ scaleT* addr, Reg::RegTensor<scaleT>& vreg)
{
    if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
        Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_BRC_B16>(vreg, addr);
    } else {
        Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_BRC_B32>(vreg, addr);
    }
}

template <typename dstT>
__simd_callee__ inline void StoreRes(__ubuf__ dstT* dstAddr, Reg::RegTensor<float>& vreg, Reg::MaskReg& preg)
{
    if constexpr (SupportType<dstT, half, bfloat16_t>()) {
        Reg::RegTensor<dstT> tempVreg;
        Reg::Cast<dstT, float, LayoutZMrgZRndRSatS>(tempVreg, vreg, preg);
        Reg::StoreAlign<dstT, Reg::StoreDist::DIST_PACK_B32>(dstAddr, tempVreg, preg);
    } else {
        Reg::StoreAlign<dstT, Reg::StoreDist::DIST_NORM_B32>(dstAddr, vreg, preg);
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendDeQuantConfig& config>
__simd_vf__ inline void DeQuantPerTokenForS32VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, const AscendDeQuantParam para)
{
    uint16_t rowNum = para.calCount / para.n;
    uint32_t vecLen = ASCENDC_QUANT_B32_VF_LEN;
    uint16_t repeat = static_cast<uint16_t>(CeilDivision(para.n, vecLen));

    Reg::MaskReg preg;
    Reg::RegTensor<int32_t> srcVreg;
    Reg::RegTensor<float> f32Vreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<float> scaleF32Vreg;
    for (uint16_t i = 0; i < static_cast<uint16_t>(rowNum); ++i) {
        LoadPerTokenScale<scaleT>(scaleUb + i, scaleVreg);
        uint32_t sreg = para.n;
        for (uint16_t j = 0; j < repeat; ++j) {
            preg = Reg::UpdateMask<uint32_t>(sreg);
            Reg::LoadAlign<int32_t, Reg::LoadDist::DIST_NORM>(srcVreg, srcUb + i * para.n + j * vecLen);
            Reg::Cast<float, int32_t, MrgZRndA>(f32Vreg, srcVreg, preg);
            if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
                Reg::Cast<float, scaleT, layoutZMrgZ>(scaleF32Vreg, scaleVreg, preg);
                Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32Vreg, f32Vreg, scaleF32Vreg, preg);
            } else {
                Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32Vreg, f32Vreg, scaleVreg, preg);
            }
            StoreRes<dstT>(dstUb + i * para.n + j * vecLen, f32Vreg, preg);
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendDeQuantConfig& config>
__aicore__ inline void DeQuantPerTokenForS32(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const LocalTensor<scaleT>& offsetTensor, const AscendDeQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    DeQuantPerTokenForS32VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, para);
}

template <typename dstT, typename srcT, typename scaleT, const AscendDeQuantConfig& config>
__simd_vf__ inline void DeQuantPerTokenForF32VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, const AscendDeQuantParam para)
{
    uint16_t rowNum = para.calCount / para.n;
    uint32_t vecLen = ASCENDC_QUANT_B32_VF_LEN;
    uint16_t repeat = static_cast<uint16_t>(CeilDivision(para.n, vecLen));

    Reg::MaskReg preg;
    Reg::RegTensor<float> srcVreg;
    Reg::RegTensor<float> f32Vreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<float> scaleF32Vreg;
    for (uint16_t i = 0; i < static_cast<uint16_t>(rowNum); ++i) {
        LoadPerTokenScale<scaleT>(scaleUb + i, scaleVreg);
        uint32_t sreg = para.n;
        for (uint16_t j = 0; j < repeat; ++j) {
            preg = Reg::UpdateMask<uint32_t>(sreg);
            Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(srcVreg, srcUb + i * para.n + j * vecLen);
            if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
                Reg::Cast<float, scaleT, layoutZMrgZ>(scaleF32Vreg, scaleVreg, preg);
                Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32Vreg, srcVreg, scaleF32Vreg, preg);
            } else {
                Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32Vreg, srcVreg, scaleVreg, preg);
            }
            StoreRes<dstT>(dstUb + i * para.n + j * vecLen, f32Vreg, preg);
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendDeQuantConfig& config>
__aicore__ inline void DeQuantPerTokenForF32(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const LocalTensor<scaleT>& offsetTensor, const AscendDeQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    DeQuantPerTokenForF32VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, para);
}

template <typename T>
__simd_callee__ inline void GetPerGroupScale(
    __ubuf__ T* scaleUb, const int32_t start, const AscendDeQuantParam& para, const AscendDeQuantConfig& config,
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

template <typename dstT, typename srcT, typename scaleT, const AscendDeQuantConfig& config>
__simd_vf__ inline void DeQuantPerGroupForColS32VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, const AscendDeQuantParam para)
{
    uint16_t rowNum = para.calCount / para.n;
    uint32_t vecLen = ASCENDC_QUANT_B32_VF_LEN;
    uint16_t repeat = static_cast<uint16_t>(CeilDivision(para.n, vecLen));
    uint32_t sreg = para.n;
    uint16_t scaleK = static_cast<uint16_t>(CeilDivision(para.n, para.groupSize));

    Reg::MaskReg preg;
    Reg::RegTensor<int32_t> srcVreg;
    Reg::RegTensor<float> f32Vreg;
    Reg::RegTensor<scaleT> oriScaleVreg;
    Reg::RegTensor<scaleT> tempVreg;
    Reg::RegTensor<int32_t> offsetVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<float> scaleF32Vreg;
    Reg::RegTensor<scaleT> zeroVreg;
    if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
        Reg::MaskReg b16FullPreg = Reg::CreateMask<uint16_t, Reg::MaskPattern::ALL>();
        Reg::Duplicate(zeroVreg, static_cast<scaleT>(0), b16FullPreg);
    } else {
        Reg::MaskReg b32FullPreg = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
        Reg::Duplicate(zeroVreg, static_cast<scaleT>(0), b32FullPreg);
    }
    for (uint16_t i = 0; i < static_cast<uint16_t>(rowNum); ++i) {
        sreg = para.n;
        for (uint16_t j = 0; j < repeat; ++j) {
            preg = Reg::UpdateMask<uint32_t>(sreg);
            if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
                GetPerGroupScale<scaleT>(scaleUb + i * scaleK, j * vecLen, para, config, oriScaleVreg);
                Reg::Interleave(scaleVreg, tempVreg, oriScaleVreg, zeroVreg);
                Reg::Cast<float, scaleT, layoutZMrgZ>(scaleF32Vreg, scaleVreg, preg);
            } else {
                GetPerGroupScale<scaleT>(scaleUb + i * scaleK, j * vecLen, para, config, scaleF32Vreg);
            }
            Reg::LoadAlign<int32_t, Reg::LoadDist::DIST_NORM>(srcVreg, srcUb + i * para.n + j * vecLen);
            Reg::Cast<float, int32_t, MrgZRndA>(f32Vreg, srcVreg, preg);
            Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32Vreg, f32Vreg, scaleF32Vreg, preg);
            StoreRes<dstT>(dstUb + i * para.n + j * vecLen, f32Vreg, preg);
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendDeQuantConfig& config>
__aicore__ inline void DeQuantPerGroupForColS32(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const LocalTensor<scaleT>& offsetTensor, const AscendDeQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    DeQuantPerGroupForColS32VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, para);
}

template <typename dstT, typename srcT, typename scaleT, const AscendDeQuantConfig& config>
__simd_vf__ inline void DeQuantPerGroupForColF32VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, const AscendDeQuantParam para)
{
    uint16_t rowNum = para.calCount / para.n;
    uint32_t vecLen = ASCENDC_QUANT_B32_VF_LEN;
    uint16_t repeat = static_cast<uint16_t>(CeilDivision(para.n, vecLen));
    uint16_t scaleK = static_cast<uint16_t>(CeilDivision(para.n, para.groupSize));

    Reg::MaskReg preg;
    Reg::RegTensor<float> srcVreg;
    Reg::RegTensor<float> f32Vreg;
    Reg::RegTensor<scaleT> oriScaleVreg;
    Reg::RegTensor<scaleT> tempVreg;
    Reg::RegTensor<float> offsetVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<float> scaleF32Vreg;
    Reg::RegTensor<dstT> dstVreg;
    Reg::RegTensor<scaleT> zeroVreg;
    if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
        Reg::MaskReg b16FullPreg = Reg::CreateMask<uint16_t, Reg::MaskPattern::ALL>();
        Reg::Duplicate(zeroVreg, static_cast<scaleT>(0), b16FullPreg);
    } else {
        Reg::MaskReg b32FullPreg = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
        Reg::Duplicate(zeroVreg, static_cast<scaleT>(0), b32FullPreg);
    }
    for (uint16_t i = 0; i < static_cast<uint16_t>(rowNum); ++i) {
        uint32_t sreg = para.n;
        for (uint16_t j = 0; j < repeat; ++j) {
            preg = Reg::UpdateMask<uint32_t>(sreg);
            if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
                GetPerGroupScale<scaleT>(scaleUb + i * scaleK, j * vecLen, para, config, oriScaleVreg);
                Reg::Interleave(scaleVreg, tempVreg, oriScaleVreg, zeroVreg);
                Reg::Cast<float, scaleT, layoutZMrgZ>(scaleF32Vreg, scaleVreg, preg);
            } else {
                GetPerGroupScale<scaleT>(scaleUb + i * scaleK, j * vecLen, para, config, scaleF32Vreg);
            }
            Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(srcVreg, srcUb + i * para.n + j * vecLen);
            Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32Vreg, srcVreg, scaleF32Vreg, preg);
            StoreRes<dstT>(dstUb + i * para.n + j * vecLen, f32Vreg, preg);
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendDeQuantConfig& config>
__aicore__ inline void DeQuantPerGroupForColF32(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const LocalTensor<scaleT>& offsetTensor, const AscendDeQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    DeQuantPerGroupForColF32VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, para);
}

template <typename dstT, typename srcT, typename scaleT, const AscendDeQuantConfig& config>
__simd_callee__ inline void DeQuantPerGroupForRowTailBlock(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, uint16_t repeat, uint16_t tailRow, uint32_t n,
    uint32_t vecLen)
{
    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<float> f32ScaleVreg;
    Reg::RegTensor<srcT> srcVreg;
    Reg::RegTensor<float> f32Vreg;
    Reg::MaskReg b32FullPreg = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    for (uint16_t i = 0; i < tailRow; ++i) {
        uint32_t sreg = n;
        for (uint16_t j = 0; j < repeat; ++j) {
            if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
                Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_UNPACK_B16>(scaleVreg, scaleUb + j * vecLen);
                Reg::Cast<float, scaleT, layoutZMrgZ>(f32ScaleVreg, scaleVreg, b32FullPreg);
            } else {
                Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_NORM>(f32ScaleVreg, scaleUb + j * vecLen);
            }
            preg = Reg::UpdateMask<uint32_t>(sreg);
            Reg::LoadAlign<srcT, Reg::LoadDist::DIST_NORM>(srcVreg, srcUb + i * n + j * vecLen);
            if constexpr (SupportType<srcT, int32_t>()) {
                Reg::Cast<float, int32_t, MrgZRndA>(f32Vreg, srcVreg, preg);
                Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32Vreg, f32Vreg, f32ScaleVreg, preg);
            } else {
                Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32Vreg, srcVreg, f32ScaleVreg, preg);
            }
            StoreRes<dstT>(dstUb + i * n + j * vecLen, f32Vreg, preg);
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendDeQuantConfig& config>
__simd_vf__ inline void DeQuantPerGroupForRowVF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, const AscendDeQuantParam para,
    uint16_t rowNum, uint16_t tailRow)
{
    uint16_t mainRowGroup = rowNum / para.groupSize;
    uint32_t vecLen = ASCENDC_QUANT_B32_VF_LEN;
    uint16_t repeat = static_cast<uint16_t>(CeilDivision(para.n, vecLen));

    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<float> f32ScaleVreg;
    Reg::RegTensor<srcT> srcVreg;
    Reg::RegTensor<float> f32Vreg;
    Reg::MaskReg b32FullPreg = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    for (uint16_t i = 0; i < mainRowGroup; ++i) {
        for (uint16_t j = 0; j < static_cast<uint16_t>(para.groupSize); ++j) {
            uint32_t sreg = para.n;
            for (uint16_t k = 0; k < repeat; ++k) {
                if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
                    Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_UNPACK_B16>(
                        scaleVreg, scaleUb + i * para.n + k * vecLen);
                    Reg::Cast<float, scaleT, layoutZMrgZ>(f32ScaleVreg, scaleVreg, b32FullPreg);
                } else {
                    Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_NORM>(f32ScaleVreg, scaleUb + i * para.n + k * vecLen);
                }
                preg = Reg::UpdateMask<uint32_t>(sreg);
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_NORM>(
                    srcVreg, srcUb + (i * para.groupSize + j) * para.n + k * vecLen);
                if constexpr (SupportType<srcT, int32_t>()) {
                    Reg::Cast<float, int32_t, MrgZRndA>(f32Vreg, srcVreg, preg);
                    Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32Vreg, f32Vreg, f32ScaleVreg, preg);
                } else {
                    Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32Vreg, srcVreg, f32ScaleVreg, preg);
                }
                StoreRes<dstT>(dstUb + (i * para.groupSize + j) * para.n + k * vecLen, f32Vreg, preg);
            }
        }
    }
    DeQuantPerGroupForRowTailBlock<dstT, srcT, scaleT, config>(
        dstUb + mainRowGroup * para.groupSize * para.n, srcUb + mainRowGroup * para.groupSize * para.n,
        scaleUb + mainRowGroup * para.n, repeat, tailRow, para.n, vecLen);
}

template <typename dstT, typename srcT, typename scaleT, const AscendDeQuantConfig& config>
__aicore__ inline void DeQuantPerGroupForRow(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const LocalTensor<scaleT>& offsetTensor, const AscendDeQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    uint16_t rowNum = para.calCount / para.n;
    uint16_t tailRow = rowNum % para.groupSize;
    DeQuantPerGroupForRowVF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, para, rowNum, tailRow);
}

template <typename dstT, typename srcT, typename scaleT, const AscendDeQuantConfig& config>
__aicore__ inline void AscendDeQuantPerToken(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const LocalTensor<scaleT>& scaleTensor, const LocalTensor<scaleT>& offsetTensor, const AscendDeQuantParam& para)
{
    if constexpr (SupportType<srcT, int32_t>()) {
        DeQuantPerTokenForS32<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, offsetTensor, para);
    } else if constexpr (SupportType<srcT, float>()) {
        DeQuantPerTokenForF32<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, offsetTensor, para);
    } else {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupport srcT for AscendDeQuant!"); });
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendDeQuantConfig& config>
__aicore__ inline void AscendDeQuantPerGroupForCol(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const LocalTensor<scaleT>& scaleTensor, const LocalTensor<scaleT>& offsetTensor, const AscendDeQuantParam& para)
{
    if constexpr (SupportType<srcT, int32_t>()) {
        DeQuantPerGroupForColS32<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, offsetTensor, para);
    } else if constexpr (SupportType<srcT, float>()) {
        DeQuantPerGroupForColF32<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, offsetTensor, para);
    } else {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupport srcT for AscendDeQuant!"); });
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendDeQuantConfig& config>
__aicore__ inline void AscendDeQuantPerGroupForRow(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const LocalTensor<scaleT>& scaleTensor, const LocalTensor<scaleT>& offsetTensor, const AscendDeQuantParam& para)
{
    if constexpr (SupportType<srcT, int32_t, float>()) {
        DeQuantPerGroupForRow<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, offsetTensor, para);
    } else {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupport srcT for AscendDeQuant!"); });
    }
}

template <
    typename dstT, typename srcT, typename scaleT, const AscendDeQuantConfig& config, const AscendDeQuantPolicy& policy>
__aicore__ inline void AscendDequantImpl(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const LocalTensor<scaleT>& scaleTensor, const LocalTensor<scaleT>& offsetTensor, const AscendDeQuantParam& para)
{
    if ASCEND_IS_AIC {
        return;
    }
    CheckTensorPosition(dstTensor, "dstTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcTensor, "srcTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(scaleTensor, "scaleTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(offsetTensor, "offsetTensor", "VECIN/VECOUT/VECCALC");
    static_assert(SupportType<srcT, int32_t, float>(), "AscendDequant only support int32_t/float input dtype");
    static_assert(
        SupportType<dstT, bfloat16_t, half, float>(), "AscendDequant only support bfloat16_t/half/float output dtype");
    static_assert(
        SupportType<scaleT, bfloat16_t, half, float>(),
        "AscendDequant only support bfloat16_t/half/float scaleT dtype");
    static_assert(
        ((policy == AscendDeQuantPolicy::PER_TOKEN) || (policy == AscendDeQuantPolicy::PER_GROUP)),
        "unsupported policy for AscendDequant in current device!");
    ASCENDC_ASSERT(
        (para.calCount <= srcTensor.GetSize() && para.calCount <= dstTensor.GetSize() && para.calCount >= 0), {
            KERNEL_LOG(
                KERNEL_ERROR, "calCount is %u, which should be in [0, min(%u, %u)]", para.calCount, srcTensor.GetSize(),
                dstTensor.GetSize());
        });
    if constexpr (policy == AscendDeQuantPolicy::PER_TOKEN) {
        AscendDeQuantPerToken<dstT, srcT, scaleT, config>(
            dstTensor, srcTensor, sharedTmpBuffer, scaleTensor, offsetTensor, para);
    } else if constexpr (policy == AscendDeQuantPolicy::PER_GROUP) {
        static_assert(
            ((config.kDim == 0) || (config.kDim == 1)), "AscendDequant PerGroup only support kDim is axis 0/1!");
        ASCENDC_ASSERT((para.groupSize > 0 && para.groupSize % 32 == 0), {
            KERNEL_LOG(KERNEL_ERROR, "groupSize must be an integer multiple of 32 and greater than 0 !");
        });
        if constexpr (config.kDim == 1) {
            AscendDeQuantPerGroupForCol<dstT, srcT, scaleT, config>(
                dstTensor, srcTensor, sharedTmpBuffer, scaleTensor, offsetTensor, para);
        } else {
            AscendDeQuantPerGroupForRow<dstT, srcT, scaleT, config>(
                dstTensor, srcTensor, sharedTmpBuffer, scaleTensor, offsetTensor, para);
        }
    }
}
} //  namespace AscendC
#endif // LIB_ASCEND_DEQUANT_ASCEND_DEQUANT_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_DEQUANT_ASCEND_DEQUANT_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_DEQUANT_ASCEND_DEQUANT_C310_IMPL_H__
#endif
