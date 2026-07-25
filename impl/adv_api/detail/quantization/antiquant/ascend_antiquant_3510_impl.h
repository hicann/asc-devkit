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
 * \file ascend_antiquant_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/quantization/antiquant/ascend_antiquant_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/quantization/ascend_antiquant.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_ANTIQUANT_ASCEND_ANTIQUANT_C310_IMPL_H__
#endif

#ifndef IMPL_QUANTIZATION_ANTIQUANT_ASCEND_ANTIQUANT_C310_IMPL_H
#define IMPL_QUANTIZATION_ANTIQUANT_ASCEND_ANTIQUANT_C310_IMPL_H

#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../basic_api/kernel_pop_stack_buffer.h"
#include "ascend_antiquant_common.h"
#include "../../quantization/quant/ascend_quant_3510_impl.h"
#include "../../common/check.h"

namespace AscendC {
constexpr uint32_t ANTIQUANT_B16_VF_LEN = GetVecLen() / sizeof(uint16_t);
constexpr uint32_t ANTIQUANT_B32_VF_LEN = GetVecLen() / sizeof(uint32_t);

template <typename SrcType, typename OutType>
__aicore__ inline void CheckApiDtypeValid()
{
    constexpr bool inputValid = (IsSameType<SrcType, int8_t>::value) || (IsSameType<SrcType, int4b_t>::value);
    constexpr bool outputValid = (IsSameType<OutType, half>::value) || (IsSameType<OutType, bfloat16_t>::value);
    ASCENDC_ASSERT((inputValid && outputValid), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in AscendAntiQuant, "
                          "current api support dtype combination is src: int8_t / int4b_t, dst: half / bfloat16_t.");
    });
}

__simd_callee__ inline void SelectZeroNan(
    Reg::RegTensor<bfloat16_t>& b16vreg, Reg::RegTensor<uint16_t>& bf16Zero, Reg::RegTensor<uint16_t>& bf16Nan,
    Reg::RegTensor<uint16_t>& e8m0Zero, Reg::RegTensor<uint16_t>& e8m0Nan, Reg::MaskReg& selPreg, Reg::MaskReg& preg)
{
    Compare<uint16_t, CMPMODE::NE>(selPreg, (Reg::RegTensor<uint16_t>&)b16vreg, e8m0Zero, preg);
    Select<uint16_t>((Reg::RegTensor<uint16_t>&)b16vreg, (Reg::RegTensor<uint16_t>&)b16vreg, bf16Zero, selPreg);
    Compare<uint16_t, CMPMODE::NE>(selPreg, (Reg::RegTensor<uint16_t>&)b16vreg, e8m0Nan, preg);
    Select<uint16_t>((Reg::RegTensor<uint16_t>&)b16vreg, (Reg::RegTensor<uint16_t>&)b16vreg, bf16Nan, selPreg);
}

template <typename OutputDataType>
__simd_vf__ inline void CastScale(__ubuf__ OutputDataType* dst, __ubuf__ fp8_e8m0_t* scale, const uint32_t srcCalCount)
{
    Reg::MaskReg preg;
    Reg::RegTensor<fp8_e8m0_t> vreg;
    Reg::RegTensor<bfloat16_t> b16vreg;
    Reg::RegTensor<half> halfvreg;
    Reg::MaskReg selPreg;
    Reg::RegTensor<uint16_t> bf16Zero;
    Reg::RegTensor<uint16_t> bf16Nan;
    Reg::RegTensor<uint16_t> e8m0Zero;
    Reg::RegTensor<uint16_t> e8m0Nan;
    Duplicate(bf16Zero, (uint16_t)0x0040); // if e8m0 = 0b00000000, bf16 is 0x0040
    Duplicate(bf16Nan, (uint16_t)0x7fff);  // if e8m0 = 0b11111111, use 0x7fff as bf16 nan
    Duplicate(e8m0Zero, 0);
    Duplicate(e8m0Nan, (uint16_t)0x7f80); // if e8m0 = 0b11111111, after << 7 is 0x7f80
    uint32_t sregLower = static_cast<uint32_t>(ANTIQUANT_B16_VF_LEN);
    uint32_t scaleCalCount = srcCalCount / ANTIQUANT_FP4_PERGROUP_SIZE; // perGroupSize = 32 default
    uint32_t sreg = static_cast<uint32_t>(scaleCalCount);
    uint16_t repeatScale = CeilDivision(scaleCalCount, sregLower);

    for (uint16_t i = 0; i < static_cast<uint16_t>(repeatScale); ++i) {
        preg = Reg::UpdateMask<uint16_t>(sreg);
        Reg::LoadAlign<fp8_e8m0_t, Reg::LoadDist::DIST_UNPACK_B8>(vreg, scale + i * sregLower);
        Reg::ShiftLefts<uint16_t, int16_t>(
            (Reg::RegTensor<uint16_t>&)b16vreg, (Reg::RegTensor<uint16_t>&)vreg, ANTIQUANT_BF16_MAN_LEN, preg);

        // 00000000 and 11111111 need special process
        SelectZeroNan(b16vreg, bf16Zero, bf16Nan, e8m0Zero, e8m0Nan, selPreg, preg);

        if constexpr (SupportType<OutputDataType, half>()) {
            Reg::Cast<half, bfloat16_t, MrgZRndRSatS>(halfvreg, b16vreg, preg);
            Reg::StoreAlign<half, Reg::StoreDist::DIST_NORM_B16>(dst + i * sregLower, halfvreg, preg);
        } else {
            Reg::StoreAlign<bfloat16_t, Reg::StoreDist::DIST_NORM_B16>(dst + i * sregLower, b16vreg, preg);
        }
    }
}

template <typename SrcType, typename OutputDataType>
__simd_callee__ inline void AntiQuantProcessByLine(
    __ubuf__ OutputDataType* dstUb, __ubuf__ SrcType* srcUb, __ubuf__ fp8_e8m0_t* scaleUb, const uint32_t calCount,
    Reg::RegTensor<uint16_t>& bf16Zero, Reg::RegTensor<uint16_t>& bf16Nan, Reg::RegTensor<uint16_t>& e8m0Zero,
    Reg::RegTensor<uint16_t>& e8m0Nan)
{
    Reg::RegTensor<fp8_e8m0_t> vreg1;
    Reg::RegTensor<bfloat16_t> b16vreg1;
    Reg::RegTensor<bfloat16_t> b16vreg2;
    Reg::RegTensor<SrcType> fp4vreg0;
    Reg::RegTensor<half> halfvreg1;
    Reg::RegTensor<half> halfvreg2;
    Reg::MaskReg selPreg;

    uint32_t sreg = static_cast<uint32_t>(calCount);
    Reg::MaskReg preg;
    uint32_t sregLower = static_cast<uint32_t>(ANTIQUANT_B16_VF_LEN);
    uint16_t repeatTimes = CeilDivision(calCount, sregLower);
    for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTimes); ++i) {
        preg = Reg::UpdateMask<uint16_t>(sreg);
        Reg::LoadAlign<fp8_e8m0_t, Reg::LoadDist::DIST_UNPACK_B8>(vreg1, scaleUb + i * sregLower);
        Reg::ShiftLefts<uint16_t, int16_t>(
            (Reg::RegTensor<uint16_t>&)b16vreg1, (Reg::RegTensor<uint16_t>&)vreg1, ANTIQUANT_BF16_MAN_LEN, preg);

        // 00000000 and 11111111 need special process
        SelectZeroNan(b16vreg1, bf16Zero, bf16Nan, e8m0Zero, e8m0Nan, selPreg, preg);

        Reg::LoadAlign<uint8_t, Reg::LoadDist::DIST_UNPACK4_B8>(
            (Reg::RegTensor<uint8_t>&)fp4vreg0, (__ubuf__ uint8_t*)srcUb + (i * sregLower / HALF_FACTOR));
        Reg::Cast<bfloat16_t, SrcType, layoutZMrgZ>(b16vreg2, fp4vreg0, preg);
        if constexpr (SupportType<OutputDataType, half>()) {
            Reg::Cast<half, bfloat16_t, MrgZRndRSatS>(halfvreg2, b16vreg2, preg);
            Reg::Cast<half, bfloat16_t, MrgZRndRSatS>(halfvreg1, b16vreg1, preg);
            Reg::Mul<half>(halfvreg2, halfvreg1, halfvreg2, preg);
            Reg::StoreAlign<half, Reg::StoreDist::DIST_NORM_B16>(dstUb + i * sregLower, halfvreg2, preg);
        } else {
            Reg::Mul<bfloat16_t>(b16vreg2, b16vreg1, b16vreg2, preg);
            Reg::StoreAlign<bfloat16_t, Reg::StoreDist::DIST_NORM_B16>(dstUb + i * sregLower, b16vreg2, preg);
        }
    }
}

template <typename SrcType, typename OutputDataType>
__simd_vf__ inline void AntiQuantProcessByNum(
    __ubuf__ OutputDataType* dst, __ubuf__ SrcType* src, __ubuf__ OutputDataType* scale, const uint32_t srcCalCount,
    const uint16_t newRepeatTime)
{
    uint32_t sregLower = static_cast<uint32_t>(ANTIQUANT_B16_VF_LEN);
    Reg::MaskReg preg1;
    Reg::RegTensor<bfloat16_t> b16vreg2;
    Reg::RegTensor<OutputDataType> scaleReg1;
    Reg::RegTensor<OutputDataType> scaleReg2;
    Reg::RegTensor<OutputDataType> tmpReg;
    Reg::RegTensor<SrcType> fp4vreg0;
    Reg::RegTensor<half> halfvreg2;
    uint32_t sreg1 = static_cast<uint32_t>(srcCalCount);
    for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeatTime); i++) {
        preg1 = Reg::UpdateMask<uint16_t>(sreg1);
        Reg::LoadAlign<OutputDataType, Reg::LoadDist::DIST_E2B_B16>(tmpReg, scale + i * DEFAULT_BLK_NUM);
        Reg::Interleave<uint16_t>(
            (Reg::RegTensor<uint16_t>&)scaleReg1, (Reg::RegTensor<uint16_t>&)scaleReg2,
            (Reg::RegTensor<uint16_t>&)tmpReg, (Reg::RegTensor<uint16_t>&)tmpReg);

        Reg::LoadAlign<uint8_t, Reg::LoadDist::DIST_UNPACK4_B8>(
            (Reg::RegTensor<uint8_t>&)fp4vreg0,
            (__ubuf__ uint8_t*)src + (2 * i) * sregLower / HALF_FACTOR); // once process half of sregLower num
        Reg::Cast<bfloat16_t, SrcType, layoutZMrgZ>(b16vreg2, fp4vreg0, preg1);
        if constexpr (SupportType<OutputDataType, half>()) {
            Reg::Cast<half, bfloat16_t, MrgZRndRSatS>(halfvreg2, b16vreg2, preg1);
            Reg::Mul<half>(halfvreg2, scaleReg1, halfvreg2, preg1);
            Reg::StoreAlign<half, Reg::StoreDist::DIST_NORM_B16>(dst + (2 * i) * sregLower, halfvreg2, preg1);
        } else {
            Reg::Mul<bfloat16_t>(b16vreg2, scaleReg1, b16vreg2, preg1);
            Reg::StoreAlign<bfloat16_t, Reg::StoreDist::DIST_NORM_B16>(dst + (2 * i) * sregLower, b16vreg2, preg1);
        }

        preg1 = Reg::UpdateMask<uint16_t>(sreg1);
        Reg::LoadAlign<uint8_t, Reg::LoadDist::DIST_UNPACK4_B8>(
            (Reg::RegTensor<uint8_t>&)fp4vreg0,
            (__ubuf__ uint8_t*)src + (2 * i + 1) * sregLower / HALF_FACTOR); // once process half of sregLower num
        Reg::Cast<bfloat16_t, SrcType, layoutZMrgZ>(b16vreg2, fp4vreg0, preg1);

        if constexpr (SupportType<OutputDataType, half>()) {
            Reg::Cast<half, bfloat16_t, MrgZRndRSatS>(halfvreg2, b16vreg2, preg1);
            Reg::Mul<half>(halfvreg2, scaleReg2, halfvreg2, preg1);
            Reg::StoreAlign<half, Reg::StoreDist::DIST_NORM_B16>(dst + (2 * i + 1) * sregLower, halfvreg2, preg1);
        } else {
            Reg::Mul<bfloat16_t>(b16vreg2, scaleReg2, b16vreg2, preg1);
            Reg::StoreAlign<bfloat16_t, Reg::StoreDist::DIST_NORM_B16>(dst + (2 * i + 1) * sregLower, b16vreg2, preg1);
        }
    }
}

template <typename SrcType, typename OutputDataType>
__simd_vf__ inline void AscendAntiQuantNoTranspose(
    __ubuf__ OutputDataType* dstUb, __ubuf__ SrcType* srcUb, __ubuf__ fp8_e8m0_t* scaleUb, const uint32_t k,
    const uint32_t n)
{
    static_assert(
        SupportType<SrcType, fp4x2_e2m1_t, fp4x2_e1m2_t>(), "This AscendAntiQuant only support fp4 input dtype");
    Reg::RegTensor<uint16_t> bf16Zero;
    Reg::RegTensor<uint16_t> bf16Nan;
    Reg::RegTensor<uint16_t> e8m0Zero;
    Reg::RegTensor<uint16_t> e8m0Nan;
    Duplicate(bf16Zero, (uint16_t)0x0040); // if e8m0 = 0b00000000, bf16 is 0x0040
    Duplicate(bf16Nan, (uint16_t)0x7fff);  // if e8m0 = 0b11111111, use 0x7fff as bf16 nan
    Duplicate(e8m0Zero, 0);
    Duplicate(e8m0Nan, (uint16_t)0x7f80); // if e8m0 = 0b11111111, after << 7 is 0x7f80
    uint16_t repeatTimes = CeilDivision(k, ANTIQUANT_FP4_PERGROUP_SIZE);
    for (uint16_t i = 0; i < repeatTimes; i++) {
        for (uint16_t j = 0; j < ANTIQUANT_FP4_PERGROUP_SIZE; j++) {
            uint16_t srcAddrCount = i * ANTIQUANT_FP4_PERGROUP_SIZE * n + j * n;
            if constexpr (SupportType<SrcType, fp4x2_e1m2_t, fp4x2_e2m1_t>) {
                srcAddrCount = srcAddrCount / 2;
            }
            AntiQuantProcessByLine<SrcType, OutputDataType>(
                dstUb + i * ANTIQUANT_FP4_PERGROUP_SIZE * n + j * n, srcUb + srcAddrCount, scaleUb + i * n, n, bf16Zero,
                bf16Nan, e8m0Zero, e8m0Nan);
        }
    }
}

__aicore__ inline constexpr uint32_t GetAscendAntiQuantTmpBufferLiveNode()
{
    constexpr uint32_t tmpBufferLiveNode = 1;
    return tmpBufferLiveNode;
}

template <typename SrcType>
__aicore__ inline uint32_t GetAscendAntiQuantTmpBufferSize(const LocalTensor<uint8_t>& sharedTmpBuffer)
{
    uint32_t sharedTmpBufferSize = sharedTmpBuffer.GetSize() / GetAscendAntiQuantTmpBufferLiveNode();
    return AlignUp(sharedTmpBufferSize, GetDataBlockSizeInBytes()) / sizeof(SrcType);
}

template <typename SrcType, typename OutputDataType>
__aicore__ inline void AscendAntiQuantTranspose(
    const LocalTensor<OutputDataType>& dst, const LocalTensor<SrcType>& src, const LocalTensor<fp8_e8m0_t>& scale,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t k, const uint32_t n)
{
    __ubuf__ OutputDataType* dstUb = (__ubuf__ OutputDataType*)dst.GetPhyAddr();
    __ubuf__ SrcType* srcUb = (__ubuf__ SrcType*)src.GetPhyAddr();
    __ubuf__ fp8_e8m0_t* scaleUb = (__ubuf__ fp8_e8m0_t*)scale.GetPhyAddr();
    auto tmpbuffer = sharedTmpBuffer.ReinterpretCast<OutputDataType>();
    __ubuf__ OutputDataType* tmpbufferUb = (__ubuf__ OutputDataType*)tmpbuffer.GetPhyAddr();

    uint32_t srcCalCount = n * k;
    if (scale.GetSize() == 1) {
        uint32_t sharedTmpBufferSize = GetAscendAntiQuantTmpBufferSize<SrcType>(sharedTmpBuffer);
        uint32_t count = srcCalCount;
        uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(srcCalCount, sharedTmpBufferSize));
        for (uint16_t i = 0; i < repeatTimes; i++) {
            uint32_t remainCount = count - sharedTmpBufferSize * i;
            uint32_t oneRepSize = remainCount < sharedTmpBufferSize ? remainCount : sharedTmpBufferSize;
            CastScale<OutputDataType>(tmpbufferUb, scaleUb + i * sharedTmpBufferSize, oneRepSize);
            uint16_t againRepeatTimes = CeilDivision(oneRepSize, ANTIQUANT_B16_VF_LEN);
            uint16_t newRepeatTime =
                (againRepeatTimes == 1) ? 1 : (againRepeatTimes / HALF_FACTOR); // if calcount <=128 need repeat once
            AntiQuantProcessByNum<SrcType, OutputDataType>(
                dstUb + i * sharedTmpBufferSize, srcUb + i * sharedTmpBufferSize, tmpbufferUb, oneRepSize,
                newRepeatTime);
        }
    } else {
        CastScale<OutputDataType>(tmpbufferUb, scaleUb, srcCalCount);
        uint16_t repeatTimes = CeilDivision(srcCalCount, ANTIQUANT_B16_VF_LEN);
        uint16_t newRepeatTime =
            (repeatTimes == 1) ? 1 : (repeatTimes / HALF_FACTOR); // if calcount <=128 need repeat once
        AntiQuantProcessByNum<SrcType, OutputDataType>(dstUb, srcUb, tmpbufferUb, srcCalCount, newRepeatTime);
    }
}

template <typename SrcType, typename OutputDataType, bool isTranspose>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendAntiQuant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void AscendAntiQuantImpl(
    const LocalTensor<OutputDataType>& dst, const LocalTensor<SrcType>& src, const LocalTensor<fp8_e8m0_t>& scale,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t k, const AntiQuantShapeInfo& shapeInfo = {})
{
    CheckTensorPosition(dst, "dst", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(src, "src", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(scale, "scale", "VECIN/VECOUT/VECCALC");
    static_assert(
        SupportType<SrcType, fp4x2_e2m1_t, fp4x2_e1m2_t>(), "This AscendAntiQuant only support fp4 input dtype");
    static_assert(
        SupportType<OutputDataType, half, bfloat16_t>(), "This AscendAntiQuant only support half/bf16 output dtype");
    __ubuf__ OutputDataType* dstUb = (__ubuf__ OutputDataType*)dst.GetPhyAddr();
    __ubuf__ SrcType* srcUb = (__ubuf__ SrcType*)src.GetPhyAddr();
    __ubuf__ fp8_e8m0_t* scaleUb = (__ubuf__ fp8_e8m0_t*)scale.GetPhyAddr();
    if constexpr (isTranspose) {
        ASCENDC_ASSERT((k != 0 && (k / HALF_FACTOR) % ONE_BLK_SIZE == 0), {
            KERNEL_LOG(KERNEL_ERROR, "K should be larger than 0 && should be 32B aligned!");
        });
        uint32_t n = (shapeInfo.scaleHeight == 0 ? scale.GetShapeInfo().shape[0] : shapeInfo.scaleHeight);
        AscendAntiQuantTranspose(dst, src, scale, sharedTmpBuffer, k, n);
    } else {
        uint32_t n1 = (shapeInfo.scaleWidth == 0 ? scale.GetShapeInfo().shape[1] : shapeInfo.scaleWidth);
        ASCENDC_ASSERT((n1 != 0 && (n1 / HALF_FACTOR) % ONE_BLK_SIZE == 0), {
            KERNEL_LOG(KERNEL_ERROR, "k should be larger than 0 && should be 32B aligned!");
        });
        AscendAntiQuantNoTranspose<SrcType, OutputDataType>(dstUb, srcUb, scaleUb, k, n1);
    }
}

/* **************************************************************************************************
 * perTensor for B8                                             *
 * ************************************************************************************************* */
template <typename SrcType, typename OutputDataType>
__simd_vf__ inline void PerTensorProcessForFp8(
    __ubuf__ OutputDataType* dst, __ubuf__ SrcType* src, const OutputDataType offset, const OutputDataType scale,
    const uint32_t srcCalCount)
{
    Reg::MaskReg preg;
    Reg::RegTensor<SrcType> vreg;
    Reg::RegTensor<float> f32vreg;
    Reg::RegTensor<OutputDataType> outReg;

    uint32_t sregLower = static_cast<uint32_t>(ANTIQUANT_B32_VF_LEN);
    uint32_t sreg = static_cast<uint32_t>(srcCalCount);
    uint16_t repeat = CeilDivision(srcCalCount, sregLower);

    for (uint16_t i = 0; i < static_cast<uint16_t>(repeat); ++i) {
        preg = Reg::UpdateMask<uint32_t>(sreg);
        Reg::LoadAlign<SrcType, Reg::LoadDist::DIST_UNPACK4_B8>(vreg, src + i * sregLower);
        Reg::Cast<float, SrcType, layoutZMrgZ>(f32vreg, vreg, preg);

        if constexpr (SupportType<OutputDataType, bfloat16_t>()) {
            Reg::Adds<float, float, Reg::MaskMergeMode::ZEROING>(f32vreg, f32vreg, ToFloat(offset), preg);
            Reg::Muls<float, float, Reg::MaskMergeMode::ZEROING>(f32vreg, f32vreg, ToFloat(scale), preg);
        } else {
            Reg::Adds<float, float, Reg::MaskMergeMode::ZEROING>(f32vreg, f32vreg, static_cast<float>(offset), preg);
            Reg::Muls<float, float, Reg::MaskMergeMode::ZEROING>(f32vreg, f32vreg, static_cast<float>(scale), preg);
        }

        Reg::Cast<OutputDataType, float, LayoutZMrgZRndRSatS>(outReg, f32vreg, preg);
        Reg::StoreAlign<OutputDataType, Reg::StoreDist::DIST_PACK_B32>(dst + i * sregLower, outReg, preg);
    }
}
template <typename SrcType>
__simd_vf__ inline void PerTensorProcessForB8(
    __ubuf__ half* dst, __ubuf__ SrcType* src, const half offset, const half scale, const uint32_t srcCalCount)
{
    Reg::MaskReg preg;
    Reg::RegTensor<SrcType> vreg;
    Reg::RegTensor<bfloat16_t> b16vreg;
    Reg::RegTensor<half> f16vreg;

    uint32_t sregLower = static_cast<uint32_t>(ANTIQUANT_B16_VF_LEN);
    uint32_t sreg = static_cast<uint32_t>(srcCalCount);
    uint16_t repeat = CeilDivision(srcCalCount, sregLower);

    for (uint16_t i = 0; i < static_cast<uint16_t>(repeat); ++i) {
        preg = Reg::UpdateMask<uint16_t>(sreg);
        Reg::LoadAlign<SrcType, Reg::LoadDist::DIST_UNPACK_B8>(vreg, src + i * sregLower);
        Reg::Cast<half, SrcType, layoutZMrgZ>(f16vreg, vreg, preg); // hif8->f16 or int8->f16

        Reg::Adds<half, half, Reg::MaskMergeMode::ZEROING>(f16vreg, f16vreg, offset, preg);
        Reg::Muls<half, half, Reg::MaskMergeMode::ZEROING>(f16vreg, f16vreg, scale, preg);
        Reg::StoreAlign<half, Reg::StoreDist::DIST_NORM_B16>(dst + i * sregLower, f16vreg, preg);
    }
}

template <typename SrcType>
__simd_vf__ inline void PerTensorProcessForB8(
    __ubuf__ bfloat16_t* dst, __ubuf__ SrcType* src, const bfloat16_t offset, const bfloat16_t scale,
    const uint32_t srcCalCount)
{
    Reg::MaskReg preg;
    Reg::RegTensor<SrcType> vreg;
    Reg::RegTensor<bfloat16_t> b16vreg;
    Reg::RegTensor<half> f16vreg;
    Reg::RegTensor<float> f32vreg;
    Reg::RegTensor<half> vregTmp;

    uint32_t sregLower = static_cast<uint32_t>(ANTIQUANT_B32_VF_LEN);
    uint32_t sreg = static_cast<uint32_t>(srcCalCount);
    uint16_t repeat = CeilDivision(srcCalCount, sregLower);

    uint32_t f16sreg = static_cast<uint32_t>(ANTIQUANT_B16_VF_LEN);
    Reg::MaskReg f16preg = Reg::UpdateMask<uint16_t>(f16sreg);

    for (uint16_t i = 0; i < static_cast<uint16_t>(repeat); ++i) {
        preg = Reg::UpdateMask<uint32_t>(sreg);
        Reg::LoadAlign<SrcType, Reg::LoadDist::DIST_UNPACK_B8>(vreg, src + i * sregLower);
        Reg::Cast<half, SrcType, layoutZMrgZ>(f16vreg, vreg, f16preg); // hif8->f16 or int8->f16

        Reg::Interleave(f16vreg, vregTmp, f16vreg, vregTmp);

        Reg::Cast<float, half, layoutZMrgZ>(f32vreg, f16vreg, preg); // f16->f32
        Reg::Adds<float, float, Reg::MaskMergeMode::ZEROING>(f32vreg, f32vreg, ToFloat(offset), preg);
        Reg::Muls<float, float, Reg::MaskMergeMode::ZEROING>(f32vreg, f32vreg, ToFloat(scale), preg);
        Reg::Cast<bfloat16_t, float, LayoutZMrgZRndRSatS>(b16vreg, f32vreg, preg);
        Reg::StoreAlign<bfloat16_t, Reg::StoreDist::DIST_PACK_B32>(dst + i * sregLower, b16vreg, preg);
    }
}

template <typename SrcType, typename OutputDataType>
__aicore__ inline void AntiQuantPertensorImpl(
    const LocalTensor<OutputDataType>& dst, const LocalTensor<SrcType>& src, const OutputDataType offset,
    const OutputDataType scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t K,
    const AntiQuantShapeInfo& shapeInfo = {})
{
    static_assert(
        SupportType<SrcType, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t, int8_t>(),
        "This AscendAntiQuant only support fp8/hif8/int8 input dtype");
    static_assert(
        SupportType<OutputDataType, half, bfloat16_t>(), "This AscendAntiQuant only support f16/bf16 output dtype");
    __ubuf__ OutputDataType* dstUb = (__ubuf__ OutputDataType*)dst.GetPhyAddr();
    __ubuf__ SrcType* srcUb = (__ubuf__ SrcType*)src.GetPhyAddr();
    auto tmpbuffer = sharedTmpBuffer.ReinterpretCast<OutputDataType>();
    __ubuf__ OutputDataType* tmpbufferUb = (__ubuf__ OutputDataType*)tmpbuffer.GetPhyAddr();

    uint32_t srcCalCount = src.GetSize();
    if constexpr (SupportType<SrcType, fp8_e4m3fn_t, fp8_e5m2_t>()) {
        PerTensorProcessForFp8<SrcType, OutputDataType>(dstUb, srcUb, offset, scale, srcCalCount);
    } else {
        // vfcall not support overload function
        PerTensorProcessForB8<SrcType>(dstUb, srcUb, offset, scale, srcCalCount);
    }
}

/* **************************************************************************************************
 * perChannel for B8                                             *
 * ************************************************************************************************* */
template <typename SrcType, typename OutputDataType>
__simd_vf__ inline void PerchannelNoTransposeForFp8(
    __ubuf__ OutputDataType* dst, __ubuf__ SrcType* src, __ubuf__ OutputDataType* offset,
    __ubuf__ OutputDataType* scale, const uint32_t K, const uint32_t N)
{
    Reg::MaskReg preg;
    Reg::RegTensor<SrcType> vreg;
    Reg::RegTensor<float> f32vreg;
    Reg::RegTensor<OutputDataType> outReg;
    Reg::RegTensor<OutputDataType> scaleB16Vreg;
    Reg::RegTensor<OutputDataType> offsetB16Vreg;
    Reg::RegTensor<float> scaleB32Vreg;
    Reg::RegTensor<float> offsetB32Vreg;

    uint32_t sregLower = ANTIQUANT_B32_VF_LEN;
    uint32_t sreg = N;
    uint16_t repeat = CeilDivision(N, sregLower);

    for (uint16_t i = 0; i < repeat; ++i) {
        preg = Reg::UpdateMask<uint32_t>(sreg);
        // load offset and scale ,then cast to float to add &&mul
        Reg::LoadAlign<OutputDataType, Reg::LoadDist::DIST_UNPACK_B16>(offsetB16Vreg, offset + i * sregLower);
        Reg::LoadAlign<OutputDataType, Reg::LoadDist::DIST_UNPACK_B16>(scaleB16Vreg, scale + i * sregLower);
        Reg::Cast<float, OutputDataType, layoutZMrgZ>(offsetB32Vreg, offsetB16Vreg, preg); // b16->fp32
        Reg::Cast<float, OutputDataType, layoutZMrgZ>(scaleB32Vreg, scaleB16Vreg, preg);   // b16->fp32

        for (uint16_t j = 0; j < static_cast<uint16_t>(K); ++j) {
            Reg::LoadAlign<SrcType, Reg::LoadDist::DIST_UNPACK4_B8>(vreg, src + j * N + i * sregLower);
            Reg::Cast<float, SrcType, layoutZMrgZ>(f32vreg, vreg, preg);

            Reg::Add(f32vreg, f32vreg, offsetB32Vreg, preg);
            Reg::Mul(f32vreg, f32vreg, scaleB32Vreg, preg);

            Reg::Cast<OutputDataType, float, LayoutZMrgZRndRSatS>(outReg, f32vreg, preg);
            Reg::StoreAlign<OutputDataType, Reg::StoreDist::DIST_PACK_B32>(dst + j * N + i * sregLower, outReg, preg);
        }
    }
}

template <typename SrcType, typename OutputDataType>
__simd_vf__ inline void PerchannelNoTransposeForB8(
    __ubuf__ OutputDataType* dst, __ubuf__ SrcType* src, __ubuf__ OutputDataType* offset,
    __ubuf__ OutputDataType* scale, const uint32_t K, const uint32_t N)
{
    Reg::MaskReg preg;
    Reg::RegTensor<SrcType> vreg;
    Reg::RegTensor<OutputDataType> scaleB16Vreg;
    Reg::RegTensor<OutputDataType> offsetB16Vreg;
    Reg::RegTensor<half> f16vreg;
    Reg::RegTensor<OutputDataType> b16vreg;

    uint32_t sregLower = ANTIQUANT_B16_VF_LEN;
    uint32_t sreg = N;
    uint16_t repeat = CeilDivision(N, sregLower);

    for (uint16_t i = 0; i < repeat; ++i) {
        preg = Reg::UpdateMask<uint16_t>(sreg);
        Reg::LoadAlign<OutputDataType, Reg::LoadDist::DIST_NORM>(offsetB16Vreg, offset + i * sregLower);
        Reg::LoadAlign<OutputDataType, Reg::LoadDist::DIST_NORM>(scaleB16Vreg, scale + i * sregLower);

        for (uint16_t j = 0; j < static_cast<uint16_t>(K); ++j) {
            Reg::LoadAlign<SrcType, Reg::LoadDist::DIST_UNPACK_B8>(vreg, src + j * N + i * sregLower);
            if constexpr (SupportType<OutputDataType, bfloat16_t>()) {
                Reg::Cast<half, SrcType, layoutZMrgZ>(f16vreg, vreg, preg);    // hif8->f16 or int8->f16
                Reg::Cast<bfloat16_t, half, MrgZRndR>(b16vreg, f16vreg, preg); // f16->bf16
            } else {
                Reg::Cast<OutputDataType, SrcType, layoutZMrgZ>(b16vreg, vreg, preg);
            }

            Reg::Add(b16vreg, b16vreg, offsetB16Vreg, preg);
            Reg::Mul(b16vreg, b16vreg, scaleB16Vreg, preg);
            Reg::StoreAlign<OutputDataType, Reg::StoreDist::DIST_NORM_B16>(dst + j * N + i * sregLower, b16vreg, preg);
        }
    }
}

template <typename SrcType, typename OutputDataType>
__aicore__ inline void AntiQuantPerchannelNoTranspose(
    const LocalTensor<OutputDataType>& dst, const LocalTensor<SrcType>& src, const LocalTensor<OutputDataType>& offset,
    const LocalTensor<OutputDataType>& scale, const uint32_t K, const uint32_t N)
{
    __ubuf__ OutputDataType* scaleUb = (__ubuf__ OutputDataType*)scale.GetPhyAddr();
    __ubuf__ OutputDataType* offsetUb = (__ubuf__ OutputDataType*)offset.GetPhyAddr();
    __ubuf__ OutputDataType* dstUb = (__ubuf__ OutputDataType*)dst.GetPhyAddr();
    __ubuf__ SrcType* srcUb = (__ubuf__ SrcType*)src.GetPhyAddr();

    if constexpr (SupportType<SrcType, fp8_e4m3fn_t, fp8_e5m2_t>()) {
        PerchannelNoTransposeForFp8<SrcType, OutputDataType>(dstUb, srcUb, offsetUb, scaleUb, K, N);
    } else {
        PerchannelNoTransposeForB8<SrcType, OutputDataType>(dstUb, srcUb, offsetUb, scaleUb, K, N);
    }
}

template <typename SrcType, typename OutputDataType>
__simd_vf__ inline void PerchannelUnalignedForFp8(
    __ubuf__ OutputDataType* dst, __ubuf__ SrcType* src, __ubuf__ OutputDataType* offset,
    __ubuf__ OutputDataType* scale, const uint32_t srcCalCount)
{
    Reg::MaskReg preg;
    Reg::RegTensor<SrcType> vreg;
    Reg::RegTensor<float> f32vreg;
    Reg::RegTensor<OutputDataType> outReg;
    Reg::RegTensor<OutputDataType> scaleB16Vreg;
    Reg::RegTensor<OutputDataType> offsetB16Vreg;
    Reg::RegTensor<OutputDataType> scaleB16Vreg1;
    Reg::RegTensor<OutputDataType> offsetB16Vreg1;
    Reg::RegTensor<OutputDataType> scaleB16Vreg2;
    Reg::RegTensor<OutputDataType> offsetB16Vreg2;
    Reg::RegTensor<float> scaleB32Vreg;
    Reg::RegTensor<float> offsetB32Vreg;

    uint32_t sregLower = static_cast<uint32_t>(ANTIQUANT_B32_VF_LEN);
    uint32_t sreg = static_cast<uint32_t>(srcCalCount);
    uint16_t repeat = CeilDivision(srcCalCount, sregLower);

    uint32_t scaleLen = static_cast<uint32_t>(ANTIQUANT_B32_VF_LEN);
    Reg::MaskReg b16Preg = Reg::UpdateMask<uint32_t>(scaleLen);
    Reg::LoadAlign<OutputDataType, Reg::LoadDist::DIST_BLK>(offsetB16Vreg, offset);
    Reg::LoadAlign<OutputDataType, Reg::LoadDist::DIST_BLK>(scaleB16Vreg, scale);
    Reg::Interleave<half>(
        (Reg::RegTensor<half>&)offsetB16Vreg1, (Reg::RegTensor<half>&)offsetB16Vreg2,
        (Reg::RegTensor<half>&)offsetB16Vreg, (Reg::RegTensor<half>&)offsetB16Vreg);
    Reg::Interleave<half>(
        (Reg::RegTensor<half>&)scaleB16Vreg1, (Reg::RegTensor<half>&)scaleB16Vreg2, (Reg::RegTensor<half>&)scaleB16Vreg,
        (Reg::RegTensor<half>&)scaleB16Vreg);
    Reg::Cast<float, OutputDataType, layoutZMrgZ>(offsetB32Vreg, offsetB16Vreg1, b16Preg);
    Reg::Cast<float, OutputDataType, layoutZMrgZ>(scaleB32Vreg, scaleB16Vreg1, b16Preg);

    for (uint16_t i = 0; i < static_cast<uint16_t>(repeat); ++i) {
        preg = Reg::UpdateMask<uint32_t>(sreg);
        Reg::LoadAlign<SrcType, Reg::LoadDist::DIST_UNPACK4_B8>(vreg, src + i * sregLower);
        Reg::Cast<float, SrcType, layoutZMrgZ>(f32vreg, vreg, preg);

        Reg::Add<float, Reg::MaskMergeMode::ZEROING>(f32vreg, f32vreg, offsetB32Vreg, preg);
        Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32vreg, f32vreg, scaleB32Vreg, preg);

        Reg::Cast<OutputDataType, float, LayoutZMrgZRndRSatS>(outReg, f32vreg, preg);
        Reg::StoreAlign<OutputDataType, Reg::StoreDist::DIST_PACK_B32>(dst + i * sregLower, outReg, preg);
    }
}

template <typename SrcType, typename OutputDataType>
__simd_vf__ inline void PerchannelUnalignedForB8(
    __ubuf__ OutputDataType* dst, __ubuf__ SrcType* src, __ubuf__ OutputDataType* offset,
    __ubuf__ OutputDataType* scale, const uint32_t srcCalCount)
{
    Reg::MaskReg preg;
    Reg::RegTensor<SrcType> vreg;
    Reg::RegTensor<OutputDataType> b16vreg;
    Reg::RegTensor<OutputDataType> scaleB16Vreg;
    Reg::RegTensor<OutputDataType> offsetB16Vreg;
    Reg::RegTensor<half> f16vreg;

    uint32_t sregLower = static_cast<uint32_t>(ANTIQUANT_B16_VF_LEN);
    uint32_t sreg = static_cast<uint32_t>(srcCalCount);
    uint16_t repeat = CeilDivision(srcCalCount, sregLower);

    Reg::LoadAlign<OutputDataType, Reg::LoadDist::DIST_BLK>(offsetB16Vreg, offset);
    Reg::LoadAlign<OutputDataType, Reg::LoadDist::DIST_BLK>(scaleB16Vreg, scale);

    for (uint16_t i = 0; i < static_cast<uint16_t>(repeat); ++i) {
        preg = Reg::UpdateMask<uint16_t>(sreg);
        Reg::LoadAlign<SrcType, Reg::LoadDist::DIST_UNPACK_B8>(vreg, src + i * sregLower);

        if constexpr (SupportType<OutputDataType, bfloat16_t>()) {
            Reg::Cast<half, SrcType, layoutZMrgZ>(f16vreg, vreg, preg); // hif8->f16 or int8->f16
            Reg::Cast<bfloat16_t, half, MrgZRndA>(b16vreg, f16vreg, preg);
        } else {
            Reg::Cast<OutputDataType, SrcType, layoutZMrgZ>(b16vreg, vreg, preg);
        }
        Reg::Add<OutputDataType, Reg::MaskMergeMode::ZEROING>(b16vreg, b16vreg, offsetB16Vreg, preg);
        Reg::Mul<OutputDataType, Reg::MaskMergeMode::ZEROING>(b16vreg, b16vreg, scaleB16Vreg, preg);
        Reg::StoreAlign<OutputDataType, Reg::StoreDist::DIST_NORM_B16>(dst + i * sregLower, b16vreg, preg);
    }
}

template <typename SrcType, typename OutputDataType>
__aicore__ inline void AntiQuantUnalignedProcess(
    const LocalTensor<OutputDataType>& dst, const LocalTensor<SrcType>& src, const LocalTensor<OutputDataType>& offset,
    const LocalTensor<OutputDataType>& scale, const uint32_t K, const uint32_t N)
{
    __ubuf__ OutputDataType* scaleUb = (__ubuf__ OutputDataType*)scale.GetPhyAddr();
    __ubuf__ OutputDataType* offsetUb = (__ubuf__ OutputDataType*)offset.GetPhyAddr();
    __ubuf__ OutputDataType* dstUb = (__ubuf__ OutputDataType*)dst.GetPhyAddr();
    __ubuf__ SrcType* srcUb = (__ubuf__ SrcType*)src.GetPhyAddr();

    if constexpr (SupportType<SrcType, fp8_e4m3fn_t, fp8_e5m2_t>()) {
        PerchannelUnalignedForFp8<SrcType, OutputDataType>(dstUb, srcUb, offsetUb, scaleUb, N * K);
    } else { // now only support hifloat8 and int8
        PerchannelUnalignedForB8<SrcType, OutputDataType>(dstUb, srcUb, offsetUb, scaleUb, N * K);
    }
}

template <typename SrcType>
__simd_vf__ inline void PerchannelTransposeForB8(
    __ubuf__ half* dst, __ubuf__ SrcType* src, __ubuf__ half* offset, __ubuf__ half* scale, const uint32_t K,
    const uint32_t N)
{
    Reg::MaskReg preg;

    Reg::RegTensor<SrcType> vreg;
    Reg::RegTensor<half> scaleB16Vreg;
    Reg::RegTensor<half> offsetB16Vreg;
    Reg::RegTensor<half> f16vreg;

    uint32_t sregLower = static_cast<uint32_t>(ANTIQUANT_B16_VF_LEN);
    uint16_t repeat = CeilDivision(K, sregLower);

    for (uint16_t i = 0; i < static_cast<uint16_t>(N); i++) {
        Reg::LoadAlign<half, Reg::LoadDist::DIST_BRC_B16>(scaleB16Vreg, scale + i);
        Reg::LoadAlign<half, Reg::LoadDist::DIST_BRC_B16>(offsetB16Vreg, offset + i);

        uint32_t sreg = static_cast<uint32_t>(K);
        for (uint16_t j = 0; j < static_cast<uint16_t>(repeat); ++j) {
            preg = Reg::UpdateMask<uint16_t>(sreg);
            Reg::LoadAlign<SrcType, Reg::LoadDist::DIST_UNPACK_B8>(vreg, src + i * K + j * sregLower);
            Reg::Cast<half, SrcType, layoutZMrgZ>(f16vreg, vreg, preg); // hif8->f16 or int8->f16

            Reg::Add<half, Reg::MaskMergeMode::ZEROING>(f16vreg, f16vreg, offsetB16Vreg, preg);
            Reg::Mul<half, Reg::MaskMergeMode::ZEROING>(f16vreg, f16vreg, scaleB16Vreg, preg);

            Reg::StoreAlign<half, Reg::StoreDist::DIST_NORM_B16>(dst + i * K + j * sregLower, f16vreg, preg);
        }
    }
}

template <typename SrcType>
__simd_vf__ inline void PerchannelTransposeForB8(
    __ubuf__ bfloat16_t* dst, __ubuf__ SrcType* src, __ubuf__ bfloat16_t* offset, __ubuf__ bfloat16_t* scale,
    const uint32_t K, const uint32_t N)
{
    Reg::MaskReg preg;
    Reg::RegTensor<SrcType> vreg;
    uint32_t sregLower = static_cast<uint32_t>(ANTIQUANT_B32_VF_LEN);
    uint16_t repeat = CeilDivision(K, sregLower);

    uint32_t f16sreg = static_cast<uint32_t>(ANTIQUANT_B16_VF_LEN);
    Reg::MaskReg f16preg = Reg::UpdateMask<uint16_t>(f16sreg);

    Reg::RegTensor<bfloat16_t> b16vreg;
    Reg::RegTensor<bfloat16_t> scaleB16Vreg;
    Reg::RegTensor<bfloat16_t> offsetB16Vreg;
    Reg::RegTensor<float> scaleB32Vreg;
    Reg::RegTensor<float> offsetB32Vreg;
    Reg::RegTensor<half> f16vreg;
    Reg::RegTensor<float> f32vreg;
    Reg::RegTensor<half> vregTmp;

    for (uint16_t i = 0; i < static_cast<uint16_t>(N); i++) {
        Reg::LoadAlign<bfloat16_t, Reg::LoadDist::DIST_BRC_B16>(scaleB16Vreg, scale + i);
        Reg::LoadAlign<bfloat16_t, Reg::LoadDist::DIST_BRC_B16>(offsetB16Vreg, offset + i);

        Reg::Cast<float, bfloat16_t, layoutZMrgZ>(offsetB32Vreg, offsetB16Vreg, f16preg);
        Reg::Cast<float, bfloat16_t, layoutZMrgZ>(scaleB32Vreg, scaleB16Vreg, f16preg);

        uint32_t sreg = static_cast<uint32_t>(K);
        for (uint16_t j = 0; j < repeat; ++j) { // process single line
            preg = Reg::UpdateMask<uint32_t>(sreg);
            Reg::LoadAlign<SrcType, Reg::LoadDist::DIST_UNPACK_B8>(vreg, src + i * K + j * sregLower);
            Reg::Cast<half, SrcType, layoutZMrgZ>(f16vreg, vreg,
                                                  f16preg); // hif8->f16 or int8->f16
            Reg::Interleave(f16vreg, vregTmp, f16vreg, vregTmp);
            Reg::Cast<float, half, layoutZMrgZ>(f32vreg, f16vreg, preg); // f16->f32

            Reg::Add<float, Reg::MaskMergeMode::ZEROING>(f32vreg, f32vreg, offsetB32Vreg, preg);
            Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32vreg, f32vreg, scaleB32Vreg, preg);
            Reg::Cast<bfloat16_t, float, LayoutZMrgZRndRSatS>(b16vreg, f32vreg, preg);
            Reg::StoreAlign<bfloat16_t, Reg::StoreDist::DIST_PACK_B32>(dst + i * K + j * sregLower, b16vreg, preg);
        }
    }
}

template <typename SrcType, typename OutputDataType>
__simd_vf__ inline void PerchannelTransposeForFp8(
    __ubuf__ OutputDataType* dst, __ubuf__ SrcType* src, __ubuf__ OutputDataType* offset,
    __ubuf__ OutputDataType* scale, const uint32_t K, const uint32_t N)
{
    Reg::MaskReg preg;
    Reg::RegTensor<SrcType> vreg;
    Reg::RegTensor<float> f32vreg;
    Reg::RegTensor<OutputDataType> outReg;
    Reg::RegTensor<OutputDataType> scaleB16Vreg;
    Reg::RegTensor<OutputDataType> offsetB16Vreg;
    Reg::RegTensor<float> scaleB32Vreg;
    Reg::RegTensor<float> offsetB32Vreg;
    uint32_t f16sreg = static_cast<uint32_t>(ANTIQUANT_B16_VF_LEN);
    Reg::MaskReg f16preg = Reg::UpdateMask<uint16_t>(f16sreg);
    uint32_t sregLower = static_cast<uint32_t>(ANTIQUANT_B32_VF_LEN);
    uint16_t repeat = CeilDivision(K, sregLower);

    for (uint16_t i = 0; i < static_cast<uint16_t>(N); i++) {
        Reg::LoadAlign<OutputDataType, Reg::LoadDist::DIST_BRC_B16>(scaleB16Vreg, scale + i);
        Reg::LoadAlign<OutputDataType, Reg::LoadDist::DIST_BRC_B16>(offsetB16Vreg, offset + i);

        Reg::Cast<float, OutputDataType, layoutZMrgZ>(offsetB32Vreg, offsetB16Vreg, f16preg);
        Reg::Cast<float, OutputDataType, layoutZMrgZ>(scaleB32Vreg, scaleB16Vreg, f16preg);

        uint32_t sreg = static_cast<uint32_t>(K);
        for (uint16_t j = 0; j < static_cast<uint16_t>(repeat); ++j) {
            preg = Reg::UpdateMask<uint32_t>(sreg);
            Reg::LoadAlign<SrcType, Reg::LoadDist::DIST_UNPACK4_B8>(vreg, src + i * K + j * sregLower);
            Reg::Cast<float, SrcType, layoutZMrgZ>(f32vreg, vreg, preg);

            Reg::Add<float, Reg::MaskMergeMode::ZEROING>(f32vreg, f32vreg, offsetB32Vreg, preg);
            Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32vreg, f32vreg, scaleB32Vreg, preg);

            Reg::Cast<OutputDataType, float, LayoutZMrgZRndRSatS>(outReg, f32vreg, preg);
            Reg::StoreAlign<OutputDataType, Reg::StoreDist::DIST_PACK_B32>(dst + i * K + j * sregLower, outReg, preg);
        }
    }
}

template <typename SrcType, typename OutputDataType>
__aicore__ inline void AntiQuantPerchannelTranspose(
    const LocalTensor<OutputDataType>& dst, const LocalTensor<SrcType>& src, const LocalTensor<OutputDataType>& offset,
    const LocalTensor<OutputDataType>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t K,
    const uint32_t N)
{
    __ubuf__ OutputDataType* dstUb = (__ubuf__ OutputDataType*)dst.GetPhyAddr();
    __ubuf__ SrcType* srcUb = (__ubuf__ SrcType*)src.GetPhyAddr();
    __ubuf__ OutputDataType* scaleUb = (__ubuf__ OutputDataType*)scale.GetPhyAddr();
    __ubuf__ OutputDataType* offsetUb = (__ubuf__ OutputDataType*)offset.GetPhyAddr();

    if constexpr (SupportType<SrcType, fp8_e4m3fn_t, fp8_e5m2_t>()) {
        PerchannelTransposeForFp8<SrcType, OutputDataType>(dstUb, srcUb, offsetUb, scaleUb, K, N);
    } else { // now only support hifloat8 and int8
        PerchannelTransposeForB8<SrcType>(dstUb, srcUb, offsetUb, scaleUb, K, N);
    }
}

template <typename SrcType, typename OutputDataType, bool isTranspose>
__aicore__ inline void AntiQuantPerchannelImpl(
    const LocalTensor<OutputDataType>& dst, const LocalTensor<SrcType>& src, const LocalTensor<OutputDataType>& offset,
    const LocalTensor<OutputDataType>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t k,
    const AntiQuantShapeInfo& shapeInfo = {})
{
    static_assert(
        SupportType<SrcType, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t, int8_t>(),
        "This AscendAntiQuant only support fp8/hif8/int8 input dtype");
    static_assert(
        SupportType<OutputDataType, half, bfloat16_t>(), "This AscendAntiQuant only support f16/bf16 output dtype");

    if constexpr (isTranspose) { // src [n,k] offset [n,1]
        uint32_t n = (shapeInfo.offsetWidth == 0 ? offset.GetShapeInfo().shape[0] : shapeInfo.offsetWidth);
        AntiQuantPerchannelTranspose(dst, src, offset, scale, sharedTmpBuffer, k, n);
    } else { // src [k,n] offset [1,n]
        uint32_t n = (shapeInfo.offsetWidth == 0 ? offset.GetShapeInfo().shape[1] : shapeInfo.offsetWidth);
        if (n < 32) { // b8 input single line is not 32B aligned such as input n == 16
            ASCENDC_ASSERT((k % 2 == 0), { KERNEL_LOG(KERNEL_ERROR, "input calculate size must be 32B aligned!"); });
            AntiQuantUnalignedProcess<SrcType, OutputDataType>(dst, src, offset, scale, k, n);
        } else {
            AntiQuantPerchannelNoTranspose<SrcType, OutputDataType>(dst, src, offset, scale, k, n);
        }
    }
}

template <typename SrcType, bool withOffset = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendAntiQuant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void AntiQuantInnerLoop(
    const LocalTensor<bfloat16_t>& dst, const LocalTensor<SrcType>& src, const LocalTensor<bfloat16_t>& offset,
    const LocalTensor<bfloat16_t>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const UnaryRepeatParams& unaryParamsCastSrc, const UnaryRepeatParams& unaryParamsToFP32,
    const UnaryRepeatParams& unaryParamsFP32ToDst, const BinaryRepeatParams& binaryParams, const uint32_t calCount)
{
    uint32_t srcFp16Pos = calCount * sizeof(bfloat16_t);
    uint32_t offsetFp32Pos = calCount * sizeof(float);
    auto fp16TmpBuffer = sharedTmpBuffer[srcFp16Pos].ReinterpretCast<half>();
    auto offsetBuffer = sharedTmpBuffer[offsetFp32Pos].ReinterpretCast<float>();
    auto resultBuffer = sharedTmpBuffer.ReinterpretCast<float>();

    UnaryRepeatParams src2f16unaryParams;
    if constexpr (IsSameType<SrcType, int8_t>::value) {
        src2f16unaryParams.srcRepStride = HALF_DEFAULT_REPEAT_STRIDE;
    } else {
        src2f16unaryParams.srcRepStride = ONE_FOURTH_DEFAULT_REPEAT_STRIDE;
    }
    src2f16unaryParams.srcRepStride = ONE_FOURTH_DEFAULT_REPEAT_STRIDE;
    UnaryRepeatParams unaryParams;
    unaryParams.srcRepStride = HALF_DEFAULT_REPEAT_STRIDE;
    UnaryRepeatParams f322f16Params;
    f322f16Params.dstRepStride = HALF_DEFAULT_REPEAT_STRIDE;

    SetVectorMask<float, MaskMode::COUNTER>(0, calCount);
    Cast<half, SrcType>(fp16TmpBuffer, src, RoundMode::CAST_NONE, calCount);
    PipeBarrier<PIPE_V>();
    Cast<float, half>(resultBuffer, fp16TmpBuffer, RoundMode::CAST_NONE, calCount);
    PipeBarrier<PIPE_V>();
    if constexpr (withOffset) {
        Cast<float, bfloat16_t>(offsetBuffer, offset, RoundMode::CAST_NONE, calCount);
        PipeBarrier<PIPE_V>();
        Add<float>(resultBuffer, resultBuffer, offsetBuffer, calCount);
        PipeBarrier<PIPE_V>();
    }
    Cast<float, bfloat16_t>(offsetBuffer, scale, RoundMode::CAST_NONE, calCount);
    PipeBarrier<PIPE_V>();
    Mul<float>(resultBuffer, resultBuffer, offsetBuffer, calCount);
    PipeBarrier<PIPE_V>();
    Cast<bfloat16_t, float>(dst, resultBuffer, RoundMode::CAST_RINT, calCount);
    PipeBarrier<PIPE_V>();
}

template <typename SrcType, bool withOffset = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendAntiQuant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void AntiQuantInnerLoop(
    const LocalTensor<bfloat16_t>& dst, const LocalTensor<SrcType>& src, const bfloat16_t offset,
    const bfloat16_t scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const UnaryRepeatParams& unaryParamsCastSrc,
    const UnaryRepeatParams& unaryParamsToFP32, const UnaryRepeatParams& unaryParamsFP32ToDst,
    const UnaryRepeatParams& unaryParamsScalar, const uint32_t calCount)
{
    uint32_t srcFp16Pos = calCount * sizeof(bfloat16_t);
    auto fp16TmpBuffer = sharedTmpBuffer[srcFp16Pos].ReinterpretCast<half>();
    auto resultBuffer = sharedTmpBuffer.ReinterpretCast<float>();

    UnaryRepeatParams src2f16unaryParams;
    if constexpr (IsSameType<SrcType, int8_t>::value) {
        src2f16unaryParams.srcRepStride = HALF_DEFAULT_REPEAT_STRIDE;
    } else {
        src2f16unaryParams.srcRepStride = ONE_FOURTH_DEFAULT_REPEAT_STRIDE;
    }
    UnaryRepeatParams unaryParams;
    unaryParams.srcRepStride = HALF_DEFAULT_REPEAT_STRIDE;
    UnaryRepeatParams f322f16Params;
    f322f16Params.dstRepStride = HALF_DEFAULT_REPEAT_STRIDE;

    SetVectorMask<float, MaskMode::COUNTER>(0, calCount);
    Cast<half, SrcType>(fp16TmpBuffer, src, RoundMode::CAST_NONE, calCount);
    PipeBarrier<PIPE_V>();
    Cast<float, half>(resultBuffer, fp16TmpBuffer, RoundMode::CAST_NONE, calCount);
    PipeBarrier<PIPE_V>();
    if constexpr (withOffset) {
        Adds<float>(resultBuffer, resultBuffer, ToFloat(offset), calCount);
        PipeBarrier<PIPE_V>();
    }
    Muls<float>(resultBuffer, resultBuffer, ToFloat(scale), calCount);
    PipeBarrier<PIPE_V>();
    Cast<bfloat16_t, float>(dst, resultBuffer, RoundMode::CAST_RINT, calCount);
    PipeBarrier<PIPE_V>();
}

template <typename SrcType>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendAntiQuant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void AscendAntiQuantNoTransposePerformance(
    const LocalTensor<bfloat16_t>& dst, const LocalTensor<SrcType>& src, const LocalTensor<bfloat16_t>& offset,
    const LocalTensor<bfloat16_t>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t K,
    const uint32_t N)
{
    uint32_t posOffsetScale = N * sizeof(float) * ANTIQUANT_TWO;
    uint32_t posCast = posOffsetScale + ANTIQUANT_SINGLE_N_SIZE_BF16 * K * sizeof(half);
    auto fp16TmpBuffer = sharedTmpBuffer[posCast].ReinterpretCast<half>();
    auto resultBuffer = sharedTmpBuffer[posOffsetScale].ReinterpretCast<float>();

    UnaryRepeatParams s42f16unaryParams;
    s42f16unaryParams.srcRepStride = N / ANTIQUANT_TWO / ONE_BLK_SIZE;
    s42f16unaryParams.dstRepStride = HALF_DEFAULT_REPEAT_STRIDE;
    UnaryRepeatParams s82f16unaryParams;
    s82f16unaryParams.srcRepStride = N * sizeof(int8_t) / ONE_BLK_SIZE;
    s82f16unaryParams.dstRepStride = HALF_DEFAULT_REPEAT_STRIDE;
    UnaryRepeatParams f162f32unaryParams;
    f162f32unaryParams.srcRepStride = HALF_DEFAULT_REPEAT_STRIDE;
    BinaryRepeatParams binaryParams;
    binaryParams.src1RepStride = 0;
    UnaryRepeatParams f322f16Params;
    f322f16Params.dstRepStride = N * sizeof(half) / ONE_BLK_SIZE;

    for (uint32_t i = 0; i < N / ANTIQUANT_SINGLE_N_SIZE_BF16; i++) {
        if constexpr (IsSameType<SrcType, int4b_t>::value) {
            // 1.cast 64K to fp16, use norm mode
            Cast<half, int4b_t>(
                fp16TmpBuffer, src[ANTIQUANT_SINGLE_N_SIZE_BF16 * i], RoundMode::CAST_NONE,
                ANTIQUANT_SINGLE_N_SIZE_BF16, K, s42f16unaryParams);
        } else {
            // 1.cast 64K to fp16, use norm mode
            Cast<half, int8_t>(
                fp16TmpBuffer, src[ANTIQUANT_SINGLE_N_SIZE_BF16 * i], RoundMode::CAST_NONE,
                ANTIQUANT_SINGLE_N_SIZE_BF16, K, s82f16unaryParams);
        }

        Cast<float, half>(
            resultBuffer, fp16TmpBuffer, RoundMode::CAST_NONE, ANTIQUANT_SINGLE_N_SIZE_BF16, K, f162f32unaryParams);
        // 2.add offset
        auto offsetBuffer = sharedTmpBuffer[ANTIQUANT_SINGLE_N_SIZE_BF16 * i * sizeof(float)].ReinterpretCast<float>();

        Add<float>(resultBuffer, resultBuffer, offsetBuffer, ANTIQUANT_SINGLE_N_SIZE_BF16, K, binaryParams);

        // 3.mul scale
        auto scaleBuffer = sharedTmpBuffer[N * sizeof(float) + ANTIQUANT_SINGLE_N_SIZE_BF16 * i * sizeof(float)]
                               .ReinterpretCast<float>();
        Mul<float>(resultBuffer, resultBuffer, scaleBuffer, ANTIQUANT_SINGLE_N_SIZE_BF16, K, binaryParams);
        // 4.cast back to bf16
        Cast<bfloat16_t, float>(
            dst[ANTIQUANT_SINGLE_N_SIZE_BF16 * i], resultBuffer, RoundMode::CAST_RINT, ANTIQUANT_SINGLE_N_SIZE_BF16, K,
            f322f16Params);
    }
}

template <typename SrcType>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendAntiQuant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void AscendAntiQuantNoTransposePerformanceTail(
    const LocalTensor<bfloat16_t>& dst, const LocalTensor<SrcType>& src, const LocalTensor<bfloat16_t>& offset,
    const LocalTensor<bfloat16_t>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t K,
    const uint32_t N, const uint32_t mask)
{
    uint32_t index = N / ANTIQUANT_SINGLE_N_SIZE_BF16 * ANTIQUANT_SINGLE_N_SIZE_BF16;
    uint32_t posOffset = N * sizeof(float);
    uint32_t posOffsetScale = posOffset * ANTIQUANT_TWO;
    uint32_t posCast = posOffsetScale + ANTIQUANT_SINGLE_N_SIZE_BF16 * K * sizeof(half);
    auto fp16TmpBuffer = sharedTmpBuffer[posCast].ReinterpretCast<half>();
    auto resultBuffer = sharedTmpBuffer[posOffsetScale].ReinterpretCast<float>();
    auto offsetBuffer = sharedTmpBuffer[index * sizeof(float)].ReinterpretCast<float>();
    auto scaleBuffer = sharedTmpBuffer[posOffset + index * sizeof(float)].ReinterpretCast<float>();

    UnaryRepeatParams s42f16unaryParams;
    s42f16unaryParams.srcRepStride = N / ANTIQUANT_TWO / ONE_BLK_SIZE;
    UnaryRepeatParams s82f16unaryParams;
    s82f16unaryParams.srcRepStride = N * sizeof(int8_t) / ONE_BLK_SIZE;
    s82f16unaryParams.dstRepStride = HALF_DEFAULT_REPEAT_STRIDE;
    UnaryRepeatParams f162f32unaryParams;
    f162f32unaryParams.srcRepStride = HALF_DEFAULT_REPEAT_STRIDE;
    BinaryRepeatParams binaryParams;
    binaryParams.src1RepStride = 0;
    UnaryRepeatParams f322f16Params;
    f322f16Params.dstRepStride = N * sizeof(bfloat16_t) / ONE_BLK_SIZE;

    if constexpr (IsSameType<SrcType, int4b_t>::value) {
        Cast<half, int4b_t>(fp16TmpBuffer, src, RoundMode::CAST_NONE, mask, K, s42f16unaryParams);
    } else {
        Cast<half, int8_t>(fp16TmpBuffer, src, RoundMode::CAST_NONE, mask, K, s82f16unaryParams);
    }

    // cast 64K to fp32, use count mode
    Cast<float, half>(resultBuffer, fp16TmpBuffer, RoundMode::CAST_NONE, mask, K, f162f32unaryParams);
    // 2.add offset
    Add<float>(resultBuffer, resultBuffer, offsetBuffer, mask, K, binaryParams);

    // 3.mul scale
    Mul<float>(resultBuffer, resultBuffer, scaleBuffer, mask, K, binaryParams);

    // 4.cast back to bf16
    Cast<bfloat16_t, float>(dst, resultBuffer, RoundMode::CAST_RINT, mask, K, f322f16Params);
}

template <typename SrcType>
__aicore__ inline void PreCast(
    const LocalTensor<bfloat16_t>& dst, const LocalTensor<SrcType>& src, const LocalTensor<bfloat16_t>& offset,
    const LocalTensor<bfloat16_t>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t K)
{
    uint32_t posOffset = offset.GetSize() * sizeof(float);
    uint32_t repeatEle = ONE_REPEAT_BYTE_SIZE / sizeof(bfloat16_t);
    uint32_t repeatTimes =
        offset.GetSize() % repeatEle == 0 ? offset.GetSize() / repeatEle : offset.GetSize() / repeatEle + 1;
    auto offsetBuffer = sharedTmpBuffer.ReinterpretCast<float>();
    auto scaleBuffer = sharedTmpBuffer[posOffset].ReinterpretCast<float>();

    UnaryRepeatParams unaryParams;
    unaryParams.srcRepStride = HALF_DEFAULT_REPEAT_STRIDE;

    Cast<float, bfloat16_t>(offsetBuffer, offset, RoundMode::CAST_NONE, offset.GetSize());
    PipeBarrier<PIPE_V>();
    Cast<float, bfloat16_t>(scaleBuffer, scale, RoundMode::CAST_NONE, offset.GetSize());
    PipeBarrier<PIPE_V>();
}

template <typename OutputDataType>
__aicore__ inline bool AntiQuantCheckPerformanceMode(
    const LocalTensor<OutputDataType>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t K)
{
    if constexpr (IsSameType<OutputDataType, bfloat16_t>::value) {
        uint32_t maxTmpBufferSize =
            scale.GetSize() * ANTIQUANT_TWO * sizeof(float) + ANTIQUANT_SINGLE_N_SIZE_BF16 * K * sizeof(float);
        return sharedTmpBuffer.GetSize() >= maxTmpBufferSize;
    }
    return true;
}

// scale * (src + offset)   src: N * K, scale: N, offset: N  NOffset: offset used for tmpTensorOffset, tmpTensorScale
// For now, calCount must equal to N * K then can use brcb
template <typename SrcType, typename OutputDataType, bool isOffset>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendAntiQuant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void CalculationMax(
    const LocalTensor<SrcType>& src, const LocalTensor<OutputDataType>& dst, AntiquantParams<float>& params,
    const uint32_t calCount, const uint32_t N, const uint32_t K, const uint32_t NOffset)
{
    // store FP16 result in second half of FP32 tmpTensor to avoid input FP16 being replaced
    uint32_t srcFp16Pos = calCount / ANTIQUANT_TWO; // therefore start from (calCount / 2)th FP32 tmpTensor
    auto fp16TmpBuffer = params.tempTensorInput[srcFp16Pos].ReinterpretCast<half>();

    UnaryRepeatParams unaryParams;
    unaryParams.srcRepStride = HALF_DEFAULT_REPEAT_STRIDE;
    UnaryRepeatParams f322f16Params;
    f322f16Params.dstRepStride = HALF_DEFAULT_REPEAT_STRIDE;
    uint32_t count = K / ANTIQUANT_SINGLE_N_SIZE; // times of for loop   K = n * 64
    // src1BlkStride = 0: need same line for add and mul
    // src1RepStride = 1: 1 block for 64 num calculation
    // dst, src0RepStride = count * 8: one repeat calculate 64 num, need to jump n * 8 block
    BinaryRepeatParams binaryParams(1, 1, 0, count * DEFAULT_REPEAT_STRIDE, count * DEFAULT_REPEAT_STRIDE, 1);

    SetVectorMask<half, MaskMode::COUNTER>(0, calCount);
    // INT8 -> FP16
    Cast<half, int8_t>(fp16TmpBuffer, src, RoundMode::CAST_NONE, calCount);
    PipeBarrier<PIPE_V>();
    // FP16 -> FP32
    Cast<float, half>(params.tempTensorInput, fp16TmpBuffer, RoundMode::CAST_NONE, calCount);
    PipeBarrier<PIPE_V>();

    SetVectorMask<float, MaskMode::COUNTER>(0, ANTIQUANT_SINGLE_N_SIZE * N); // brcb  src1 has N line, 1 line has 64 num
    for (uint32_t i = 0; i < count; i++) {
        // scale * (src + offset)
        uint32_t curOffset = i * ANTIQUANT_SINGLE_N_SIZE;
        // calculate the first group (0 ~ 64) in first loop, second group (64 ~ 128) in second loop
        if constexpr (isOffset) {
            Add<float>(
                params.tempTensorInput[curOffset], params.tempTensorInput[curOffset], params.tempTensorOffset[NOffset],
                ANTIQUANT_SINGLE_N_SIZE * N);
            PipeBarrier<PIPE_V>();
        }
        Mul<float>(
            params.tempTensorInput[curOffset], params.tempTensorInput[curOffset], params.tempTensorScale[NOffset],
            ANTIQUANT_SINGLE_N_SIZE * N);
        PipeBarrier<PIPE_V>();
    }

    // FP32 -> BF16
    SetVectorMask<float, MaskMode::COUNTER>(0, calCount);
    Cast<bfloat16_t, float>(dst, params.tempTensorInput, RoundMode::CAST_RINT, calCount);
    PipeBarrier<PIPE_V>();
}

// Brcb version
// allocate tmp buffer
template <typename OutputDataType>
__aicore__ inline void GetAntiquantTensorInfo(
    const LocalTensor<OutputDataType>& scale, const LocalTensor<float>& stackBuffer, AntiquantParams<float>& params)
{
    uint32_t N = scale.GetSize();                                  // scale and offset are shape [N]
    params.tempTensorOffset = stackBuffer[0];                      // store 8 * N * FP32    N -> brcb -> 8 * N
    params.tempTensorScale = stackBuffer[ANTIQUANT_BRCB_BASE * N]; // store 8 * N * FP32    N -> brcb -> 8 * N
    params.tempTensorInput = stackBuffer[ANTIQUANT_BRCB_BASE * ANTIQUANT_TWO * N]; // need [N * 64 * FP32, N * K * FP32]
}

// 1. BF16 / FP16 -> cast -> FP32      2. N -> brcb -> 8 * N
// nLength means shape [N] for offset and scale
template <typename OutputDataType, bool withOffset = true>
__aicore__ inline void CastAndBrcb(
    const LocalTensor<OutputDataType>& offset, const LocalTensor<OutputDataType>& scale, AntiquantParams<float>& params,
    const uint32_t nLength)
{
    UnaryRepeatParams unaryParams;
    unaryParams.srcRepStride = HALF_DEFAULT_REPEAT_STRIDE;
    uint32_t N = offset.GetSize();

    // shape [N]  BF16/ FP16 offset, scale -> cast -> FP32
    SetVectorMask<half, MaskMode::COUNTER>(0, nLength);
    if constexpr (withOffset) {
        Cast<float, OutputDataType>(
            params.tempTensorOffset[ANTIQUANT_BRCB_BASE * N - nLength], offset, RoundMode::CAST_NONE, nLength);
    }
    Cast<float, OutputDataType>(
        params.tempTensorScale[ANTIQUANT_BRCB_BASE * N - nLength], scale, RoundMode::CAST_NONE, nLength);
    PipeBarrier<PIPE_V>();

    constexpr uint16_t brcbDstBlkStride = 1;                   // 1 num -> 8 num(1 block)
    constexpr uint16_t brcbDstRepStride = ANTIQUANT_BRCB_BASE; // 1 brcb: 8 num -> 64 num
    const uint8_t repeatTimes = nLength / ANTIQUANT_BRCB_BASE; // 1 brcb cmd needs 8 input num
    BrcbRepeatParams brcbParams(brcbDstBlkStride, brcbDstRepStride);

    SetMaskNorm();
    ResetMask();
    // brcb: 1 FP32 A -> 1 block contains 8 FP32 A, after 1 block, do the same to the next FP32 B
    if constexpr (withOffset) {
        Brcb(
            params.tempTensorOffset, params.tempTensorOffset[ANTIQUANT_BRCB_BASE * N - nLength], repeatTimes,
            brcbParams);
        PipeBarrier<PIPE_V>();
    }
    Brcb(params.tempTensorScale, params.tempTensorScale[ANTIQUANT_BRCB_BASE * N - nLength], repeatTimes, brcbParams);
    PipeBarrier<PIPE_V>();
    SetMaskCount();
}

// scale * (src + offset)   src: N * K, scale: N, offset: N  NOffset: offset used for tmpTensorOffset, tmpTensorScale
// For now, calCount must equal to N * K then can use brcb   calCount: 64 * N
template <typename SrcType, typename OutputDataType, bool withOffset>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendAntiQuant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void CalculationMin(
    const LocalTensor<SrcType>& src, const LocalTensor<OutputDataType>& dst, AntiquantParams<float>& params,
    const uint32_t calCount, const uint32_t n, const uint32_t srcN, const uint32_t k)
{
    // store FP16 result in second half of FP32 tmpTensor to avoid input FP16 being replaced
    uint32_t srcFp16Pos = calCount / ANTIQUANT_TWO; // therefore start from (calCount / 2)th FP32 tmpTensor
    uint32_t n1 = k / ANTIQUANT_SINGLE_N_SIZE;      // K = 64 * n1
    UnaryRepeatParams unaryParamsInt8Fp16;
    unaryParamsInt8Fp16.srcRepStride = ANTIQUANT_TWO * n1; // K(num) / 32(num per block)
    // one repeat calculate 64 int8 -> 64 fp16, 4 block
    unaryParamsInt8Fp16.dstRepStride = ANTIQUANT_SINGLE_N_SIZE / (ONE_BLK_SIZE / sizeof(half));
    UnaryRepeatParams unaryParamsFp16Fp32;
    unaryParamsFp16Fp32.srcRepStride = HALF_DEFAULT_REPEAT_STRIDE;

    // Must use NORM for calculation instead of counter
    SetMaskNorm();
    SetVectorMask<half, MaskMode::NORMAL>(0, FULL_MASK); // the first 64 num for calculation
    // INT8 -> FP16
    auto fp16TmpBuffer = params.tempTensorInput[srcFp16Pos].ReinterpretCast<half>();
    Cast<half, int8_t, false>(fp16TmpBuffer, src, RoundMode::CAST_NONE, MASK_PLACEHOLDER, n, unaryParamsInt8Fp16);
    PipeBarrier<PIPE_V>();
    // FP16 -> FP32
    Cast<float, half, false>(
        params.tempTensorInput, fp16TmpBuffer, RoundMode::CAST_NONE, MASK_PLACEHOLDER, n, unaryParamsFp16Fp32);
    PipeBarrier<PIPE_V>();

    SetMaskCount();
    BinaryRepeatParams binaryParams;
    binaryParams.src1BlkStride = 0; // same line for add and mul
    binaryParams.src1RepStride = 1; // one line for 64 num calculation

    SetVectorMask<float, MaskMode::COUNTER>(0, ANTIQUANT_SINGLE_N_SIZE * n);
    // scale * (src + offset)
    if constexpr (withOffset) {
        Add<float>(
            params.tempTensorInput, params.tempTensorInput, params.tempTensorOffset, ANTIQUANT_SINGLE_N_SIZE * n);
        PipeBarrier<PIPE_V>();
    }
    Mul<float>(params.tempTensorInput, params.tempTensorInput, params.tempTensorScale, ANTIQUANT_SINGLE_N_SIZE * n);
    PipeBarrier<PIPE_V>();

    // FP32 -> BF16
    SetMaskNorm();
    SetVectorMask<float, MaskMode::NORMAL>(0, FULL_MASK);
    UnaryRepeatParams f322f16Params;
    f322f16Params.dstRepStride = ANTIQUANT_SINGLE_N_SIZE * n1 / (ONE_BLK_SIZE / sizeof(half));
    Cast<OutputDataType, float, false>(
        dst, params.tempTensorInput, RoundMode::CAST_RINT, MASK_PLACEHOLDER, srcN, f322f16Params);
    PipeBarrier<PIPE_V>();
}

// Method2: min: N * 64
template <typename SrcType, typename OutputDataType>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendAntiQuant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void CalculateByBrcbMin(
    const LocalTensor<OutputDataType>& dst, const LocalTensor<SrcType>& src, const LocalTensor<OutputDataType>& offset,
    const LocalTensor<OutputDataType>& scale, const LocalTensor<float>& stackBuffer, const uint32_t calCount,
    const uint32_t n, const uint32_t k)
{
    AntiquantParams<float> antiquantParams;
    GetAntiquantTensorInfo<OutputDataType>(scale, stackBuffer, antiquantParams);

    SetMaskCount();
    CastAndBrcb<OutputDataType, true>(offset, scale, antiquantParams, n); // store FP32 offset and scale into params

    uint32_t curNKOffset = 0;
    uint32_t loopNum = k / ANTIQUANT_SINGLE_N_SIZE;
    uint32_t srcN = src.GetSize() / k;
    // calculate  N * 64
    for (uint32_t i = 0; i < loopNum; i++) {
        curNKOffset = ANTIQUANT_SINGLE_N_SIZE * i;
        CalculationMin<SrcType, OutputDataType, true>(
            src[curNKOffset], dst[curNKOffset], antiquantParams, ANTIQUANT_SINGLE_N_SIZE * n, n, srcN, k);
    }
}

template <typename SrcType, typename OutputDataType>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendAntiQuant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void CalculateByBrcbMin(
    const LocalTensor<OutputDataType>& dst, const LocalTensor<SrcType>& src, const LocalTensor<OutputDataType>& scale,
    const LocalTensor<float>& stackBuffer, const uint32_t calCount, const uint32_t n, const uint32_t k)
{
    AntiquantParams<float> antiquantParams;
    GetAntiquantTensorInfo<OutputDataType>(scale, stackBuffer, antiquantParams);

    SetMaskCount();
    CastAndBrcb<OutputDataType, false>(scale, scale, antiquantParams, n); // store FP32 offset and scale into params

    uint32_t curNKOffset = 0;
    uint32_t loopNum = k / ANTIQUANT_SINGLE_N_SIZE;
    uint32_t srcN = src.GetSize() / k;
    // calculate  N * 64
    for (uint32_t i = 0; i < loopNum; i++) {
        curNKOffset = ANTIQUANT_SINGLE_N_SIZE * i;
        CalculationMin<SrcType, OutputDataType, false>(
            src[curNKOffset], dst[curNKOffset], antiquantParams, ANTIQUANT_SINGLE_N_SIZE * n, n, srcN, k);
    }
}

template <typename OutputDataType>
__aicore__ inline void CalculateByBrcbMin(
    const LocalTensor<OutputDataType>& dst, const LocalTensor<int4b_t>& src, const LocalTensor<OutputDataType>& scale,
    const LocalTensor<float>& stackBuffer, const uint32_t calCount, const uint32_t n, const uint32_t k)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "unsupported type: int4b_t for AntiQuant"); });
}

template <typename OutputDataType>
__aicore__ inline void CalculateByBrcbMin(
    const LocalTensor<OutputDataType>& dst, const LocalTensor<int4b_t>& src, const LocalTensor<OutputDataType>& offset,
    const LocalTensor<OutputDataType>& scale, const LocalTensor<float>& stackBuffer, const uint32_t calCount,
    const uint32_t n, const uint32_t k)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "unsupported type: int4b_t for AntiQuant"); });
}

template <bool withOffset = true>
__aicore__ inline void AntiQuantFp16Brcb(
    const LocalTensor<half>& scale, const LocalTensor<half>& offset, AntiquantParams<half>& params,
    const uint32_t scaleN)
{
    // step 1: do brcb for scale and offset
    const uint8_t repeatTimes = scaleN / BRCB_BROADCAST_NUMBER;
    BrcbRepeatParams brcbParams(DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    SetMaskNorm();
    ResetMask();
    Brcb(params.tempTensorScale, scale, repeatTimes, brcbParams);
    PipeBarrier<PIPE_V>();
    if constexpr (withOffset) {
        Brcb(params.tempTensorOffset, offset, repeatTimes, brcbParams);
        PipeBarrier<PIPE_V>();
    }
}

template <typename SrcType, typename OutputDataType>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendAntiQuant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void AscendAntiQuantTranspose(
    const LocalTensor<OutputDataType>& dst, const LocalTensor<SrcType>& src, const LocalTensor<OutputDataType>& offset,
    const LocalTensor<OutputDataType>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t K,
    const AntiQuantShapeInfo& shapeInfo = {})
{
    uint32_t calCount = src.GetSize();
    uint32_t N = offset.GetSize();
    if constexpr (IsSameType<OutputDataType, half>::value || IsSameType<SrcType, int4b_t>::value) {
        return AntiQuantImplScalar(dst, src, offset, scale, sharedTmpBuffer, calCount, K, shapeInfo);
    }
    if (K > ANTIQUANT_MAX_K * ANTIQUANT_BRCB_BASE || (K % ANTIQUANT_SINGLE_N_SIZE != 0)) {
        return AntiQuantImplScalar(dst, src, offset, scale, sharedTmpBuffer, calCount, K, shapeInfo);
    }

    auto stackBuffer = sharedTmpBuffer.ReinterpretCast<float>();
    // input and scale & offset
    uint32_t stackBufferSize = N * ANTIQUANT_SINGLE_N_SIZE + N * ANTIQUANT_BRCB_BASE * ANTIQUANT_TWO;
    stackBuffer.SetSize(stackBufferSize);
    CalculateByBrcbMin(dst, src, offset, scale, stackBuffer, calCount, N, K);
}

template <typename SrcType, typename OutputDataType>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendAntiQuant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void AscendAntiQuantTranspose(
    const LocalTensor<OutputDataType>& dst, const LocalTensor<SrcType>& src, const LocalTensor<OutputDataType>& scale,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t K, const AntiQuantShapeInfo& shapeInfo = {})
{
    uint32_t calCount = src.GetSize();
    uint32_t N = scale.GetSize();
    if constexpr (IsSameType<OutputDataType, half>::value || IsSameType<SrcType, int4b_t>::value) {
        return AntiQuantImplScalar(dst, src, scale, sharedTmpBuffer, calCount, K, shapeInfo);
    }
    if (K > ANTIQUANT_MAX_K * ANTIQUANT_BRCB_BASE || (K % ANTIQUANT_SINGLE_N_SIZE != 0) ||
        IsSameType<SrcType, int4b_t>::value) {
        return AntiQuantImplScalar(dst, src, scale, sharedTmpBuffer, calCount, K, shapeInfo);
    }

    auto stackBuffer = sharedTmpBuffer.ReinterpretCast<float>();
    uint32_t stackBufferSize = N * ANTIQUANT_SINGLE_N_SIZE + N * ANTIQUANT_BRCB_BASE * ANTIQUANT_TWO;
    stackBuffer.SetSize(stackBufferSize);
    CalculateByBrcbMin(dst, src, scale, stackBuffer, calCount, N, K);
}

template <typename scaleT, const AscendAntiQuantConfig& config>
__simd_callee__ inline void LoadPerTokenScaleAndOffset(
    __ubuf__ scaleT* scaleUb, __ubuf__ scaleT* offsetUb, Reg::RegTensor<scaleT>& scaleVreg,
    Reg::RegTensor<scaleT>& offsetVreg)
{
    if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
        Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_BRC_B16>(scaleVreg, scaleUb);
        if constexpr (config.hasOffset) {
            Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_BRC_B16>(offsetVreg, offsetUb);
        }
    } else {
        Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_BRC_B32>(scaleVreg, scaleUb);
        if constexpr (config.hasOffset) {
            Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_BRC_B32>(offsetVreg, offsetUb);
        }
    }
}

template <typename scaleT, const AscendAntiQuantConfig& config>
__simd_callee__ inline void LoadPerTokenTransposeScaleAndOffset(
    __ubuf__ scaleT* scaleUb, __ubuf__ scaleT* offsetUb, Reg::RegTensor<scaleT>& scaleVreg,
    Reg::RegTensor<scaleT>& offsetVreg)
{
    if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
        Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_NORM>(scaleVreg, scaleUb);
        if constexpr (config.hasOffset) {
            Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_NORM>(offsetVreg, offsetUb);
        }
    } else {
        Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_NORM>(scaleVreg, scaleUb);
        if constexpr (config.hasOffset) {
            Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_NORM>(offsetVreg, offsetUb);
        }
    }
}

template <typename T, const AscendAntiQuantConfig& config>
__simd_callee__ inline void GetPerGroupScaleAndOffset(
    __ubuf__ T* scaleUb, __ubuf__ T* offsetUb, const int32_t start, const AscendAntiQuantParam& para,
    Reg::RegTensor<T>& scaleReg, Reg::RegTensor<T>& offsetReg)
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
        if constexpr (config.hasOffset) {
            Reg::Gather(offsetReg, offsetUb, index_vreg, preg);
        }
    } else {
        Reg::MaskReg preg = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
        Reg::RegTensor<int32_t> vci_vreg;
        Reg::RegTensor<uint32_t> index_vreg;
        Reg::RegTensor<uint32_t> gsize_vreg;
        Reg::Duplicate(gsize_vreg, static_cast<uint32_t>(groupSize));
        Reg::Arange(vci_vreg, static_cast<int32_t>(start));
        Reg::Div(index_vreg, (Reg::RegTensor<uint32_t>&)vci_vreg, gsize_vreg, preg);
        Reg::Gather(scaleReg, scaleUb, index_vreg, preg);
        if constexpr (config.hasOffset) {
            Reg::Gather(offsetReg, offsetUb, index_vreg, preg);
        }
    }
}

template <typename dstT>
__simd_callee__ inline void StoreF32Res(__ubuf__ dstT* dstAddr, Reg::RegTensor<float>& vreg, Reg::MaskReg& preg)
{
    if constexpr (SupportType<dstT, float>()) {
        Reg::StoreAlign<float, Reg::StoreDist::DIST_NORM_B32>(dstAddr, vreg, preg);
    } else {
        Reg::RegTensor<dstT> tempVreg;
        Reg::Cast<dstT, float, LayoutZMrgZRndRSatS>(tempVreg, vreg, preg);
        Reg::StoreAlign<dstT, Reg::StoreDist::DIST_PACK_B32>(dstAddr, tempVreg, preg);
    }
}

template <typename scaleT, typename srcT>
__simd_callee__ inline void LoadSrc(__ubuf__ srcT* srcAddr, Reg::RegTensor<srcT>& srcVreg)
{
    if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
        Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK_B8>(srcVreg, srcAddr);
    } else {
        Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK4_B8>(srcVreg, srcAddr);
    }
}

template <typename scaleT, typename srcT>
__simd_callee__ inline void ConvertSrc(Reg::RegTensor<scaleT>& vreg, Reg::RegTensor<srcT>& srcVreg, Reg::MaskReg& preg)
{
    if constexpr (SupportType<scaleT, half>()) {
        Reg::Cast<scaleT, srcT, layoutZMrgZ>(vreg, srcVreg, preg);
    } else if constexpr (SupportType<scaleT, bfloat16_t>()) {
        Reg::RegTensor<half> f16Vreg;
        Reg::Cast<half, srcT, layoutZMrgZ>(f16Vreg, srcVreg, preg);
        Reg::Cast<bfloat16_t, half, MrgZRndR>(vreg, f16Vreg, preg);
    } else {
        Reg::RegTensor<int32_t> s32Vreg;
        if constexpr (SupportType<srcT, int8_t>()) {
            Reg::Cast<int32_t, srcT, layoutZMrgZ>(s32Vreg, srcVreg, preg);
            Reg::Cast<float, int32_t, MrgZRndA>(vreg, s32Vreg, preg);
        } else {
            Reg::Cast<float, srcT, layoutZMrgZ>(vreg, srcVreg, preg);
        }
    }
}

template <typename scaleT, const AscendAntiQuantConfig& config>
__simd_callee__ inline void AddOffsetIfExist(
    Reg::RegTensor<scaleT>& vreg, Reg::RegTensor<scaleT>& offsetVreg, Reg::MaskReg& preg)
{
    if constexpr (config.hasOffset) {
        Reg::Add<scaleT, Reg::MaskMergeMode::ZEROING>(vreg, vreg, offsetVreg, preg);
    }
}

template <typename scaleT>
__simd_callee__ inline void GenZeroVreg(Reg::RegTensor<scaleT>& vreg)
{
    if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
        Reg::MaskReg b16FullPreg = Reg::CreateMask<uint16_t, Reg::MaskPattern::ALL>();
        Reg::Duplicate(vreg, static_cast<scaleT>(0), b16FullPreg);
    } else {
        Reg::MaskReg b32FullPreg = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
        Reg::Duplicate(vreg, static_cast<scaleT>(0), b32FullPreg);
    }
}

template <typename scaleT, const AscendAntiQuantConfig& config>
__simd_callee__ inline void ConvertToF32ScaleAndOffset(
    Reg::RegTensor<scaleT>& scaleVreg, Reg::RegTensor<scaleT>& offsetVreg, Reg::MaskReg& preg,
    Reg::RegTensor<float>& f32ScaleVreg, Reg::RegTensor<float>& f32OffsetVreg)
{
    if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
        Reg::RegTensor<scaleT> zeroVreg;
        GenZeroVreg<scaleT>(zeroVreg);
        Reg::RegTensor<scaleT> tempOffsetVreg;
        Reg::RegTensor<scaleT> tempScaleVreg;
        Reg::RegTensor<scaleT> tempVreg;
        Reg::Interleave(tempScaleVreg, tempVreg, scaleVreg, zeroVreg);
        Reg::Cast<float, scaleT, layoutZMrgZ>(f32ScaleVreg, tempScaleVreg, preg);
        if constexpr (config.hasOffset) {
            Reg::Interleave(tempOffsetVreg, tempVreg, offsetVreg, zeroVreg);
            Reg::Cast<float, scaleT, layoutZMrgZ>(f32OffsetVreg, tempOffsetVreg, preg);
        }
    }
}

template <typename scaleT, const AscendAntiQuantConfig& config>
__simd_callee__ inline void LoadNormScaleAndOffset(
    __ubuf__ scaleT* scaleAddr, __ubuf__ scaleT* offsetAddr, Reg::RegTensor<scaleT>& scaleVreg,
    Reg::RegTensor<scaleT>& offsetVreg)
{
    Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_NORM>(scaleVreg, scaleAddr);
    if constexpr (config.hasOffset) {
        Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_NORM>(offsetVreg, offsetAddr);
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config>
__simd_vf__ inline void AntiQuantPerTokenForB8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, __ubuf__ scaleT* offsetUb,
    const AscendAntiQuantParam para)
{
    uint16_t rowNum = para.calCount / para.n;
    uint32_t vecLen = GetVecLen() / sizeof(scaleT);
    uint16_t repeat = CeilDivision(para.n, vecLen);
    uint32_t sreg = para.n;

    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> offsetVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<srcT> srcVreg;
    Reg::RegTensor<scaleT> vreg;
    for (uint16_t i = 0; i < rowNum; ++i) {
        LoadPerTokenScaleAndOffset<scaleT, config>(scaleUb + i, offsetUb + i, scaleVreg, offsetVreg);
        sreg = para.n;
        for (uint16_t j = 0; j < repeat; ++j) {
            preg = Reg::UpdateMask<scaleT>(sreg);
            LoadSrc<scaleT, srcT>((srcUb + i * para.n + j * vecLen), srcVreg);
            ConvertSrc<scaleT, srcT>(vreg, srcVreg, preg);
            AddOffsetIfExist<scaleT, config>(vreg, offsetVreg, preg);
            Reg::Mul<scaleT, Reg::MaskMergeMode::ZEROING>(vreg, vreg, scaleVreg, preg);
            if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
                Reg::StoreAlign<dstT, Reg::StoreDist::DIST_NORM_B16>(dstUb + i * para.n + j * vecLen, vreg, preg);
            } else {
                if constexpr (SupportType<dstT, float>()) {
                    Reg::StoreAlign<float, Reg::StoreDist::DIST_NORM_B32>(dstUb + i * para.n + j * vecLen, vreg, preg);
                } else {
                    Reg::RegTensor<dstT> tempVreg;
                    Reg::Cast<dstT, float, LayoutZMrgZRndRSatS>(tempVreg, vreg, preg);
                    Reg::StoreAlign<dstT, Reg::StoreDist::DIST_PACK_B32>(
                        dstUb + i * para.n + j * vecLen, tempVreg, preg);
                }
            }
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config>
__aicore__ inline void AntiQuantPerTokenForB8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const LocalTensor<scaleT>& offsetTensor, const AscendAntiQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    __ubuf__ scaleT* offsetUb = (__ubuf__ scaleT*)offsetTensor.GetPhyAddr();
    AntiQuantPerTokenForB8VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, offsetUb, para);
}

template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config>
__simd_vf__ inline void AntiQuantPerTokenTransposeForB8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, __ubuf__ scaleT* offsetUb,
    const AscendAntiQuantParam para)
{
    uint16_t rowNum = para.calCount / para.n;
    uint32_t vecLen = GetVecLen() / sizeof(scaleT);
    uint16_t repeat = CeilDivision(para.n, vecLen);
    uint32_t sreg = para.n;

    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> offsetVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<srcT> srcVreg;
    Reg::RegTensor<scaleT> vreg;
    for (uint16_t i = 0; i < rowNum; ++i) {
        sreg = para.n;
        for (uint16_t j = 0; j < repeat; ++j) {
            preg = Reg::UpdateMask<scaleT>(sreg);
            LoadPerTokenTransposeScaleAndOffset<scaleT, config>(
                scaleUb + j * vecLen, offsetUb + j * vecLen, scaleVreg, offsetVreg);
            LoadSrc<scaleT, srcT>((srcUb + i * para.n + j * vecLen), srcVreg);
            ConvertSrc<scaleT, srcT>(vreg, srcVreg, preg);
            AddOffsetIfExist<scaleT, config>(vreg, offsetVreg, preg);
            Reg::Mul<scaleT, Reg::MaskMergeMode::ZEROING>(vreg, vreg, scaleVreg, preg);
            if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
                Reg::StoreAlign<dstT, Reg::StoreDist::DIST_NORM_B16>(dstUb + i * para.n + j * vecLen, vreg, preg);
            } else {
                if constexpr (SupportType<dstT, float>()) {
                    Reg::StoreAlign<float, Reg::StoreDist::DIST_NORM_B32>(dstUb + i * para.n + j * vecLen, vreg, preg);
                } else {
                    Reg::RegTensor<dstT> tempVreg;
                    Reg::Cast<dstT, float, LayoutZMrgZRndRSatS>(tempVreg, vreg, preg);
                    Reg::StoreAlign<dstT, Reg::StoreDist::DIST_PACK_B32>(
                        dstUb + i * para.n + j * vecLen, tempVreg, preg);
                }
            }
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config>
__aicore__ inline void AntiQuantPerTokenTransposeForB8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const LocalTensor<scaleT>& offsetTensor, const AscendAntiQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    __ubuf__ scaleT* offsetUb = (__ubuf__ scaleT*)offsetTensor.GetPhyAddr();
    AntiQuantPerTokenTransposeForB8VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, offsetUb, para);
}

template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config>
__simd_vf__ inline void AntiQuantPerTokenForFp8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, __ubuf__ scaleT* offsetUb,
    const AscendAntiQuantParam para)
{
    uint16_t rowNum = para.calCount / para.n;
    uint32_t vecLen = ASCENDC_QUANT_B32_VF_LEN;
    uint16_t repeat = CeilDivision(para.n, vecLen);
    uint32_t sreg = para.n;

    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> offsetVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<srcT> srcVreg;
    Reg::RegTensor<float> f32Vreg;
    Reg::RegTensor<float> f32ScaleVreg;
    Reg::RegTensor<float> f32OffsetVreg;
    for (uint16_t i = 0; i < rowNum; ++i) {
        LoadPerTokenScaleAndOffset<scaleT, config>(scaleUb + i, offsetUb + i, scaleVreg, offsetVreg);
        sreg = para.n;
        for (uint16_t j = 0; j < repeat; ++j) {
            preg = Reg::UpdateMask<uint32_t>(sreg);
            Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK4_B8>(srcVreg, srcUb + i * para.n + j * vecLen);
            Reg::Cast<float, srcT, layoutZMrgZ>(f32Vreg, srcVreg, preg);
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

template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config>
__aicore__ inline void AntiQuantPerTokenForFp8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const LocalTensor<scaleT>& offsetTensor, const AscendAntiQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    __ubuf__ scaleT* offsetUb = (__ubuf__ scaleT*)offsetTensor.GetPhyAddr();
    AntiQuantPerTokenForFp8VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, offsetUb, para);
}

template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config>
__simd_vf__ inline void AntiQuantPerTokenTransposeForFp8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, __ubuf__ scaleT* offsetUb,
    const AscendAntiQuantParam para)
{
    uint16_t rowNum = para.calCount / para.n;
    uint32_t vecLen = ASCENDC_QUANT_B32_VF_LEN;
    uint16_t repeat = CeilDivision(para.n, vecLen);
    uint32_t sreg = para.n;

    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> offsetVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<srcT> srcVreg;
    Reg::RegTensor<float> f32Vreg;
    Reg::RegTensor<float> f32ScaleVreg;
    Reg::RegTensor<float> f32OffsetVreg;
    for (uint16_t i = 0; i < rowNum; ++i) {
        sreg = para.n;
        for (uint16_t j = 0; j < repeat; ++j) {
            preg = Reg::UpdateMask<uint32_t>(sreg);
            LoadPerTokenTransposeScaleAndOffset<scaleT, config>(
                scaleUb + j * vecLen, offsetUb + j * vecLen, scaleVreg, offsetVreg);
            Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK4_B8>(srcVreg, srcUb + i * para.n + j * vecLen);
            Reg::Cast<float, srcT, layoutZMrgZ>(f32Vreg, srcVreg, preg);
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

template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config>
__aicore__ inline void AntiQuantPerTokenTransposeForFp8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const LocalTensor<scaleT>& offsetTensor, const AscendAntiQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    __ubuf__ scaleT* offsetUb = (__ubuf__ scaleT*)offsetTensor.GetPhyAddr();
    AntiQuantPerTokenTransposeForFp8VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, offsetUb, para);
}

__aicore__ inline void ReplaceBf16VmulsWithVmul(
    Reg::RegTensor<bfloat16_t>& vreg, const bfloat16_t scale, Reg::MaskReg& preg)
{
    Reg::RegTensor<bfloat16_t> bf16ScaleVreg;
    Reg::Duplicate(bf16ScaleVreg, static_cast<bfloat16_t>(scale), preg);
    Reg::Mul<bfloat16_t, Reg::MaskMergeMode::ZEROING>(vreg, vreg, bf16ScaleVreg, preg);
}

template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config>
__simd_vf__ inline void AntiQuantPerGroupForColB8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, __ubuf__ scaleT* offsetUb,
    const AscendAntiQuantParam para)
{
    uint16_t rowNum = para.calCount / para.n;
    uint32_t vecLen = GetVecLen() / sizeof(scaleT);
    uint16_t repeat = CeilDivision(para.n, vecLen);
    uint32_t sreg = para.n;
    uint16_t scaleK = CeilDivision(para.n, para.groupSize);

    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> offsetVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<scaleT> vreg;
    Reg::RegTensor<srcT> srcVreg;
    for (uint16_t i = 0; i < rowNum; ++i) {
        sreg = para.n;
        for (uint16_t j = 0; j < repeat; ++j) {
            preg = Reg::UpdateMask<scaleT>(sreg);
            GetPerGroupScaleAndOffset<scaleT, config>(
                scaleUb + i * scaleK, offsetUb + i * scaleK, j * vecLen, para, scaleVreg, offsetVreg);
            LoadSrc<scaleT, srcT>((srcUb + i * para.n + j * vecLen), srcVreg);
            ConvertSrc<scaleT, srcT>(vreg, srcVreg, preg);
            AddOffsetIfExist<scaleT, config>(vreg, offsetVreg, preg);
            Reg::Mul<scaleT, Reg::MaskMergeMode::ZEROING>(vreg, vreg, scaleVreg, preg);
            if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
                Reg::StoreAlign<dstT, Reg::StoreDist::DIST_NORM_B16>(dstUb + i * para.n + j * vecLen, vreg, preg);
            } else {
                if constexpr (SupportType<dstT, float>()) {
                    Reg::StoreAlign<float, Reg::StoreDist::DIST_NORM_B32>(dstUb + i * para.n + j * vecLen, vreg, preg);
                } else {
                    Reg::RegTensor<dstT> tempVreg;
                    Reg::Cast<dstT, float, LayoutZMrgZRndRSatS>(tempVreg, vreg, preg);
                    Reg::StoreAlign<dstT, Reg::StoreDist::DIST_PACK_B32>(
                        dstUb + i * para.n + j * vecLen, tempVreg, preg);
                }
            }
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config>
__aicore__ inline void AntiQuantPerGroupForColB8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const LocalTensor<scaleT>& offsetTensor, const AscendAntiQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    __ubuf__ scaleT* offsetUb = (__ubuf__ scaleT*)offsetTensor.GetPhyAddr();
    AntiQuantPerGroupForColB8VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, offsetUb, para);
}

template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config>
__simd_vf__ inline void AntiQuantPerGroupForColFp4VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, const AscendAntiQuantParam para)
{
    uint16_t rowNum = para.calCount / para.n;
    uint32_t vecLen = GetVecLen() / sizeof(dstT);
    uint16_t repeat = CeilDivision(para.n, vecLen);
    uint32_t sreg = para.n;
    uint16_t scaleK = CeilDivision(para.n, para.groupSize);

    Reg::MaskReg preg, selPreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<srcT> srcVreg;
    Reg::RegTensor<dstT> dstVreg, dstSrcVreg, dstScaleVreg;
    Reg::MaskReg pregFull = Reg::CreateMask<scaleT, Reg::MaskPattern::ALL>();
    Reg::RegTensor<uint16_t> indexVreg, gsizeVreg, bf16Zero, bf16Nan, e8m0Zero, e8m0Nan;
    Duplicate(bf16Zero, (uint16_t)0x0040); // if e8m0 = 0b00000000, bf16 is 0x0040
    Duplicate(bf16Nan, (uint16_t)0x7fff);  // if e8m0 = 0b11111111, use 0x7fff as bf16 nan
    Duplicate(e8m0Zero, 0);
    Duplicate(e8m0Nan, (uint16_t)0x7f80); // if e8m0 = 0b11111111, after << 7 is 0x7f80
    for (uint16_t i = 0; i < rowNum; ++i) {
        sreg = para.n;
        for (uint16_t j = 0; j < repeat; ++j) {
            preg = Reg::UpdateMask<dstT>(sreg);
            // step1 load scale procedure
            // gather fp8_e8m0x128 scale data, 8-bit elements is zero-extented to 16-bit
            Reg::Duplicate(gsizeVreg, static_cast<uint16_t>(para.groupSize));
            Reg::Arange((Reg::RegTensor<int16_t>&)indexVreg, static_cast<int16_t>(j * vecLen));
            Reg::Div(indexVreg, indexVreg, gsizeVreg, pregFull);
            Reg::Gather<uint16_t, uint8_t, uint16_t>(
                (Reg::RegTensor<uint16_t>&)scaleVreg, (__ubuf__ uint8_t*)scaleUb + i * scaleK, indexVreg, pregFull);
            // fp8_e8m0x128 -> bf16x128
            Reg::ShiftLefts<uint16_t, int16_t>(
                (Reg::RegTensor<uint16_t>&)dstScaleVreg, (Reg::RegTensor<uint16_t>&)scaleVreg, ANTIQUANT_BF16_MAN_LEN,
                preg);
            // 00000000 and 11111111 need special process
            SelectZeroNan(
                (Reg::RegTensor<bfloat16_t>&)dstScaleVreg, bf16Zero, bf16Nan, e8m0Zero, e8m0Nan, selPreg, preg);
            // step2. load src procedure
            // 1. using UNPACK4_B8 to load 64xb8 and store as 64xb32
            // 2. using cast fp4->bf16, to cast 64xb32 to 128xbf16
            Reg::LoadAlign<uint8_t, Reg::LoadDist::DIST_UNPACK4_B8>(
                (Reg::RegTensor<uint8_t>&)srcVreg, (__ubuf__ uint8_t*)srcUb + (i * para.n + j * vecLen) / 2);
            Reg::Cast<bfloat16_t, srcT, layoutZMrgZ>((Reg::RegTensor<bfloat16_t>&)dstSrcVreg, srcVreg, preg);
            // step3. dst = src * scale
            Reg::Mul<bfloat16_t, Reg::MaskMergeMode::ZEROING>(
                (Reg::RegTensor<bfloat16_t>&)dstVreg, (Reg::RegTensor<bfloat16_t>&)dstSrcVreg,
                (Reg::RegTensor<bfloat16_t>&)dstScaleVreg, preg);
            // step4. cast dst to half if DstT is half
            if constexpr (SupportType<dstT, half>()) {
                Reg::Cast<dstT, bfloat16_t, LayoutZMrgZRndRSatS>(dstVreg, (Reg::RegTensor<bfloat16_t>&)dstVreg, preg);
            }
            // step5. store dst->ub
            Reg::StoreAlign<dstT, Reg::StoreDist::DIST_NORM_B16>(dstUb + i * para.n + j * vecLen, dstVreg, preg);
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config>
__aicore__ inline void AntiQuantPerGroupForColFp4(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const AscendAntiQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    AntiQuantPerGroupForColFp4VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, para);
}

template <typename dstT, typename srcT, typename scaleT>
__simd_callee__ inline void AntiQuantPerGroupForRowFp4OneRow(
    __ubuf__ dstT* dstAddr, __ubuf__ srcT* srcAddr, __ubuf__ scaleT* scaleAddr, Reg::RegTensor<dstT>& dstVreg,
    Reg::RegTensor<srcT>& srcVreg, Reg::RegTensor<scaleT>& scaleVreg, Reg::RegTensor<dstT>& dstSrcVreg,
    Reg::RegTensor<dstT>& dstScaleVreg, Reg::MaskReg& preg, uint16_t repeat, uint32_t n, uint32_t vecLen)
{
    Reg::MaskReg selPreg;
    Reg::RegTensor<uint16_t> bf16Zero, bf16Nan, e8m0Zero, e8m0Nan;
    Duplicate(bf16Zero, (uint16_t)0x0040); // if e8m0 = 0b00000000, bf16 is 0x0040
    Duplicate(bf16Nan, (uint16_t)0x7fff);  // if e8m0 = 0b11111111, use 0x7fff as bf16 nan
    Duplicate(e8m0Zero, 0);
    Duplicate(e8m0Nan, (uint16_t)0x7f80); // if e8m0 = 0b11111111, after << 7 is 0x7f80
    for (uint16_t j = 0; j < repeat; ++j) {
        preg = Reg::UpdateMask<dstT>(n);
        // step1: load scale procedure
        Reg::LoadAlign<scaleT, Reg::LoadDist::DIST_UNPACK_B8>(scaleVreg, scaleAddr + j * vecLen);
        Reg::ShiftLefts<uint16_t, int16_t>(
            (Reg::RegTensor<uint16_t>&)dstScaleVreg, (Reg::RegTensor<uint16_t>&)scaleVreg, ANTIQUANT_BF16_MAN_LEN,
            preg);
        // 00000000 and 11111111 need special process
        SelectZeroNan((Reg::RegTensor<bfloat16_t>&)dstScaleVreg, bf16Zero, bf16Nan, e8m0Zero, e8m0Nan, selPreg, preg);
        // step2: load src
        Reg::LoadAlign<uint8_t, Reg::LoadDist::DIST_UNPACK4_B8>(
            (Reg::RegTensor<uint8_t>&)srcVreg, (__ubuf__ uint8_t*)srcAddr + (j * vecLen) / 2);
        Reg::Cast<bfloat16_t, srcT, layoutZMrgZ>((Reg::RegTensor<bfloat16_t>&)dstSrcVreg, srcVreg, preg);
        // step3. dst = src * scale
        Reg::Mul<bfloat16_t, Reg::MaskMergeMode::ZEROING>(
            (Reg::RegTensor<bfloat16_t>&)dstVreg, (Reg::RegTensor<bfloat16_t>&)dstSrcVreg,
            (Reg::RegTensor<bfloat16_t>&)dstScaleVreg, preg);
        // step4. cast dst to half if DstT is half
        if constexpr (SupportType<dstT, half>()) {
            Reg::Cast<dstT, bfloat16_t, LayoutZMrgZRndRSatS>(dstVreg, (Reg::RegTensor<bfloat16_t>&)dstVreg, preg);
        }
        // step5. store dst->ub
        Reg::StoreAlign<dstT, Reg::StoreDist::DIST_NORM_B16>(dstAddr + j * vecLen, dstVreg, preg);
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config>
__simd_vf__ inline void AntiQuantPerGroupForRowFp4VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, const AscendAntiQuantParam para,
    uint16_t rowNum, uint16_t tailRow)
{
    uint16_t mainRowGroup = rowNum / para.groupSize;
    uint32_t vecLen = GetVecLen() / sizeof(dstT);
    uint16_t repeat = CeilDivision(para.n, vecLen);

    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<srcT> srcVreg;
    Reg::RegTensor<dstT> dstVreg, dstSrcVreg, dstScaleVreg;
    for (uint16_t i0 = 0; i0 < mainRowGroup; ++i0) {
        for (uint16_t i1 = 0; i1 < static_cast<uint16_t>(para.groupSize); ++i1) {
            AntiQuantPerGroupForRowFp4OneRow<dstT, srcT, scaleT>(
                dstUb + (i0 * para.groupSize + i1) * para.n, srcUb + ((i0 * para.groupSize + i1) * para.n) / 2,
                scaleUb + i0 * para.n, dstVreg, srcVreg, scaleVreg, dstSrcVreg, dstScaleVreg, preg, repeat, para.n,
                vecLen);
        }
    }
    for (uint16_t i = 0; i < tailRow; ++i) {
        AntiQuantPerGroupForRowFp4OneRow<dstT, srcT, scaleT>(
            dstUb + (mainRowGroup * para.groupSize + i) * para.n,
            srcUb + ((mainRowGroup * para.groupSize + i) * para.n) / 2, scaleUb + mainRowGroup * para.n, dstVreg,
            srcVreg, scaleVreg, dstSrcVreg, dstScaleVreg, preg, repeat, para.n, vecLen);
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config>
__aicore__ inline void AntiQuantPerGroupForRowFp4(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const AscendAntiQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    uint16_t rowNum = para.calCount / para.n;
    uint16_t tailRow = rowNum % para.groupSize;

    AntiQuantPerGroupForRowFp4VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, para, rowNum, tailRow);
}

template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config>
__simd_vf__ inline void AntiQuantPerGroupForColFp8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, __ubuf__ scaleT* offsetUb,
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
    Reg::RegTensor<srcT> srcVreg;
    Reg::RegTensor<float> f32SrcVreg;
    Reg::RegTensor<float> f32ScaleVreg;
    Reg::RegTensor<float> f32OffsetVreg;
    for (uint16_t i = 0; i < rowNum; ++i) {
        sreg = para.n;
        for (uint16_t j = 0; j < repeat; ++j) {
            preg = Reg::UpdateMask<uint32_t>(sreg);
            GetPerGroupScaleAndOffset<scaleT, config>(
                scaleUb + i * scaleK, offsetUb + i * scaleK, j * vecLen, para, scaleVreg, offsetVreg);
            Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK4_B8>(srcVreg, srcUb + i * para.n + j * vecLen);
            Reg::Cast<float, srcT, layoutZMrgZ>(f32SrcVreg, srcVreg, preg);
            ConvertToF32ScaleAndOffset<scaleT, config>(scaleVreg, offsetVreg, preg, f32ScaleVreg, f32OffsetVreg);
            if constexpr (config.hasOffset) {
                if constexpr (SupportType<scaleT, float>()) {
                    Reg::Add<float, Reg::MaskMergeMode::ZEROING>(f32SrcVreg, f32SrcVreg, offsetVreg, preg);
                } else {
                    Reg::Add<float, Reg::MaskMergeMode::ZEROING>(f32SrcVreg, f32SrcVreg, f32OffsetVreg, preg);
                }
            }
            if constexpr (SupportType<scaleT, float>()) {
                Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32SrcVreg, f32SrcVreg, scaleVreg, preg);
            } else {
                Reg::Mul<float, Reg::MaskMergeMode::ZEROING>(f32SrcVreg, f32SrcVreg, f32ScaleVreg, preg);
            }
            StoreF32Res<dstT>((dstUb + i * para.n + j * vecLen), f32SrcVreg, preg);
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config>
__aicore__ inline void AntiQuantPerGroupForColFp8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const LocalTensor<scaleT>& offsetTensor, const AscendAntiQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    __ubuf__ scaleT* offsetUb = (__ubuf__ scaleT*)offsetTensor.GetPhyAddr();
    AntiQuantPerGroupForColFp8VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, offsetUb, para);
}

template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config>
__simd_callee__ inline void AntiQuantPerGroupForRowB8TailBlock(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, __ubuf__ scaleT* offsetUb, uint16_t repeat,
    uint16_t tailRow, uint32_t n, uint32_t vecLen)
{
    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> offsetVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<scaleT> vreg;
    Reg::RegTensor<srcT> srcVreg;
    for (uint16_t i = 0; i < tailRow; ++i) {
        uint32_t sreg = n;
        for (uint16_t j = 0; j < repeat; ++j) {
            LoadNormScaleAndOffset<scaleT, config>(
                (scaleUb + j * vecLen), (offsetUb + j * vecLen), scaleVreg, offsetVreg);
            preg = Reg::UpdateMask<scaleT>(sreg);
            LoadSrc<scaleT, srcT>(srcUb + i * n + j * vecLen, srcVreg);
            ConvertSrc<scaleT, srcT>(vreg, srcVreg, preg);
            AddOffsetIfExist<scaleT, config>(vreg, offsetVreg, preg);
            Reg::Mul<scaleT, Reg::MaskMergeMode::ZEROING>(vreg, vreg, scaleVreg, preg);
            if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
                Reg::StoreAlign<dstT, Reg::StoreDist::DIST_NORM_B16>(dstUb + i * n + j * vecLen, vreg, preg);
            } else {
                if constexpr (SupportType<dstT, float>()) {
                    Reg::StoreAlign<float, Reg::StoreDist::DIST_NORM_B32>(dstUb + i * n + j * vecLen, vreg, preg);
                } else {
                    Reg::RegTensor<dstT> tempVreg;
                    Reg::Cast<dstT, float, LayoutZMrgZRndRSatS>(tempVreg, vreg, preg);
                    Reg::StoreAlign<dstT, Reg::StoreDist::DIST_PACK_B32>(dstUb + i * n + j * vecLen, tempVreg, preg);
                }
            }
        }
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config>
__simd_vf__ inline void AntiQuantPerGroupForRowB8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, __ubuf__ scaleT* offsetUb,
    const AscendAntiQuantParam para, uint16_t rowNum, uint16_t tailRow)
{
    uint16_t mainRowGroup = rowNum / para.groupSize;
    uint32_t vecLen = GetVecLen() / sizeof(scaleT);
    uint16_t repeat = CeilDivision(para.n, vecLen);

    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> offsetVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<scaleT> vreg;
    Reg::RegTensor<srcT> srcVreg;
    for (uint16_t i = 0; i < mainRowGroup; ++i) {
        for (uint16_t j = 0; j < static_cast<uint16_t>(para.groupSize); ++j) {
            uint32_t sreg = para.n;
            for (uint16_t k = 0; k < repeat; ++k) {
                LoadNormScaleAndOffset<scaleT, config>(
                    (scaleUb + i * para.n + k * vecLen), (offsetUb + i * para.n + k * vecLen), scaleVreg, offsetVreg);
                preg = Reg::UpdateMask<scaleT>(sreg);
                LoadSrc<scaleT, srcT>(srcUb + (i * para.groupSize + j) * para.n + k * vecLen, srcVreg);
                ConvertSrc<scaleT, srcT>(vreg, srcVreg, preg);
                AddOffsetIfExist<scaleT, config>(vreg, offsetVreg, preg);
                Reg::Mul<scaleT, Reg::MaskMergeMode::ZEROING>(vreg, vreg, scaleVreg, preg);
                if constexpr (SupportType<scaleT, half, bfloat16_t>()) {
                    Reg::StoreAlign<dstT, Reg::StoreDist::DIST_NORM_B16>(
                        dstUb + (i * para.groupSize + j) * para.n + k * vecLen, vreg, preg);
                } else {
                    if constexpr (SupportType<dstT, float>()) {
                        Reg::StoreAlign<float, Reg::StoreDist::DIST_NORM_B32>(
                            dstUb + (i * para.groupSize + j) * para.n + k * vecLen, vreg, preg);
                    } else {
                        Reg::RegTensor<dstT> tempVreg;
                        Reg::Cast<dstT, float, LayoutZMrgZRndRSatS>(tempVreg, vreg, preg);
                        Reg::StoreAlign<dstT, Reg::StoreDist::DIST_PACK_B32>(
                            dstUb + (i * para.groupSize + j) * para.n + k * vecLen, tempVreg, preg);
                    }
                }
            }
        }
    }
    AntiQuantPerGroupForRowB8TailBlock<dstT, srcT, scaleT, config>(
        dstUb + mainRowGroup * para.groupSize * para.n, srcUb + mainRowGroup * para.groupSize * para.n,
        scaleUb + mainRowGroup * para.n, offsetUb + mainRowGroup * para.n, repeat, tailRow, para.n, vecLen);
}

template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config>
__aicore__ inline void AntiQuantPerGroupForRowB8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const LocalTensor<scaleT>& offsetTensor, const AscendAntiQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    __ubuf__ scaleT* offsetUb = (__ubuf__ scaleT*)offsetTensor.GetPhyAddr();
    uint16_t rowNum = para.calCount / para.n;
    uint16_t tailRow = rowNum % para.groupSize;
    AntiQuantPerGroupForRowB8VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, offsetUb, para, rowNum, tailRow);
}

template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config>
__simd_callee__ inline void AntiQuantPerGroupForRowFp8TailBlock(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, __ubuf__ scaleT* offsetUb, uint16_t repeat,
    uint16_t tailRow, uint32_t n, uint32_t vecLen)
{
    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> offsetVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<srcT> srcVreg;
    Reg::RegTensor<float> f32ScaleVreg;
    Reg::RegTensor<float> f32OffsetVreg;
    Reg::RegTensor<float> f32SrcVreg;
    for (uint16_t i = 0; i < tailRow; ++i) {
        uint32_t sreg = n;
        for (uint16_t j = 0; j < repeat; ++j) {
            LoadNormScaleAndOffset<scaleT, config>(
                (scaleUb + j * vecLen), (offsetUb + j * vecLen), scaleVreg, offsetVreg);
            preg = Reg::UpdateMask<uint32_t>(sreg);
            Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK4_B8>(srcVreg, srcUb + i * n + j * vecLen);
            Reg::Cast<float, srcT, layoutZMrgZ>(f32SrcVreg, srcVreg, preg);
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

template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config>
__simd_vf__ inline void AntiQuantPerGroupForRowFp8VF(
    __ubuf__ dstT* dstUb, __ubuf__ srcT* srcUb, __ubuf__ scaleT* scaleUb, __ubuf__ scaleT* offsetUb,
    const AscendAntiQuantParam para, uint16_t rowNum, uint16_t tailRow)
{
    uint16_t mainRowGroup = rowNum / para.groupSize;
    uint32_t vecLen = ASCENDC_QUANT_B32_VF_LEN;
    uint16_t repeat = CeilDivision(para.n, vecLen);

    Reg::MaskReg preg;
    Reg::RegTensor<scaleT> offsetVreg;
    Reg::RegTensor<scaleT> scaleVreg;
    Reg::RegTensor<srcT> srcVreg;
    Reg::RegTensor<float> f32Svreg;
    Reg::RegTensor<float> f32Ovreg;
    Reg::RegTensor<float> f32SrcVreg;
    for (uint16_t i = 0; i < mainRowGroup; ++i) {
        for (uint16_t j = 0; j < static_cast<uint16_t>(para.groupSize); ++j) {
            uint32_t sreg = para.n;
            for (uint16_t k = 0; k < repeat; ++k) {
                preg = Reg::UpdateMask<uint32_t>(sreg);
                LoadNormScaleAndOffset<scaleT, config>(
                    (scaleUb + i * para.n + k * vecLen), (offsetUb + i * para.n + k * vecLen), scaleVreg, offsetVreg);
                Reg::LoadAlign<srcT, Reg::LoadDist::DIST_UNPACK4_B8>(
                    srcVreg, (srcUb + (i * para.groupSize + j) * para.n + k * vecLen));
                Reg::Cast<float, srcT, layoutZMrgZ>(f32SrcVreg, srcVreg, preg);
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
    AntiQuantPerGroupForRowFp8TailBlock<dstT, srcT, scaleT, config>(
        dstUb + mainRowGroup * para.groupSize * para.n, srcUb + mainRowGroup * para.groupSize * para.n,
        scaleUb + mainRowGroup * para.n, offsetUb + mainRowGroup * para.n, repeat, tailRow, para.n, vecLen);
}

template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config>
__aicore__ inline void AntiQuantPerGroupForRowFp8(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const LocalTensor<scaleT>& offsetTensor, const AscendAntiQuantParam& para)
{
    __ubuf__ dstT* dstUb = (__ubuf__ dstT*)dstTensor.GetPhyAddr();
    __ubuf__ srcT* srcUb = (__ubuf__ srcT*)srcTensor.GetPhyAddr();
    __ubuf__ scaleT* scaleUb = (__ubuf__ scaleT*)scaleTensor.GetPhyAddr();
    __ubuf__ scaleT* offsetUb = (__ubuf__ scaleT*)offsetTensor.GetPhyAddr();
    uint16_t rowNum = para.calCount / para.n;
    uint16_t tailRow = rowNum % para.groupSize;
    AntiQuantPerGroupForRowFp8VF<dstT, srcT, scaleT, config>(dstUb, srcUb, scaleUb, offsetUb, para, rowNum, tailRow);
}

template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config>
__aicore__ inline void AscendAntiQuantPerToken(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const LocalTensor<scaleT>& scaleTensor, const LocalTensor<scaleT>& offsetTensor, const AscendAntiQuantParam& para)
{
    if constexpr (config.isTranspose) {
        if constexpr (SupportType<srcT, fp8_e4m3fn_t, fp8_e5m2_t>()) {
            AntiQuantPerTokenTransposeForFp8<dstT, srcT, scaleT, config>(
                dstTensor, srcTensor, scaleTensor, offsetTensor, para);
        } else {
            AntiQuantPerTokenTransposeForB8<dstT, srcT, scaleT, config>(
                dstTensor, srcTensor, scaleTensor, offsetTensor, para);
        }
        return;
    }
    if constexpr (SupportType<srcT, fp8_e4m3fn_t, fp8_e5m2_t>()) {
        AntiQuantPerTokenForFp8<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, offsetTensor, para);
    } else {
        AntiQuantPerTokenForB8<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, offsetTensor, para);
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config>
__aicore__ inline void AscendAntiQuantPerGroupForCol(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const LocalTensor<scaleT>& scaleTensor, const LocalTensor<scaleT>& offsetTensor, const AscendAntiQuantParam& para)
{
    if constexpr (SupportType<srcT, fp8_e4m3fn_t, fp8_e5m2_t>()) {
        AntiQuantPerGroupForColFp8<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, offsetTensor, para);
    } else if constexpr (SupportType<srcT, fp4x2_e1m2_t, fp4x2_e2m1_t>()) {
        // fp4 doesn't count offset
        AntiQuantPerGroupForColFp4<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, para);
    } else {
        AntiQuantPerGroupForColB8<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, offsetTensor, para);
    }
}

template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config>
__aicore__ inline void AscendAntiQuantPerGroupForRow(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const LocalTensor<scaleT>& scaleTensor, const LocalTensor<scaleT>& offsetTensor, const AscendAntiQuantParam& para)
{
    if constexpr (SupportType<srcT, fp8_e4m3fn_t, fp8_e5m2_t>()) {
        AntiQuantPerGroupForRowFp8<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, offsetTensor, para);
    } else if constexpr (SupportType<srcT, fp4x2_e1m2_t, fp4x2_e2m1_t>()) {
        // fp4 doesn't count offset
        AntiQuantPerGroupForRowFp4<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, para);
    } else {
        AntiQuantPerGroupForRowB8<dstT, srcT, scaleT, config>(dstTensor, srcTensor, scaleTensor, offsetTensor, para);
    }
}

template <
    typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config,
    const AscendAntiQuantPolicy& policy>
__aicore__ inline void AscendAntiQuantImpl(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const LocalTensor<scaleT>& scaleTensor, const LocalTensor<scaleT>& offsetTensor, const AscendAntiQuantParam& para)
{
    if ASCEND_IS_AIC {
        return;
    }
    CheckTensorPosition(dstTensor, "dstTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcTensor, "srcTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(scaleTensor, "scaleTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(offsetTensor, "offsetTensor", "VECIN/VECOUT/VECCALC");
    static_assert(
        SupportType<dstT, bfloat16_t, half, float>(),
        "AscendAntiQuant only support bfloat16_t/half/float output dtype");
    static_assert(
        SupportType<scaleT, bfloat16_t, half, float, fp8_e8m0_t>(),
        "AscendAntiQuant only support bfloat16_t/half/float scale/offset dtype");
    static_assert(
        (policy == AscendAntiQuantPolicy::PER_TOKEN || policy == AscendAntiQuantPolicy::PER_GROUP),
        "unsupported policy for AscendAntiQuant in current device!");
    ASCENDC_ASSERT(
        (para.calCount <= srcTensor.GetSize() && para.calCount <= dstTensor.GetSize() && para.calCount >= 0), {
            KERNEL_LOG(
                KERNEL_ERROR, "calCount is %u, which should be in [0, min(%u, %u)]", para.calCount, srcTensor.GetSize(),
                dstTensor.GetSize());
        });
    ASCENDC_ASSERT(
        (para.calCount % para.n == 0), { KERNEL_LOG(KERNEL_ERROR, "calCount must be an integer multiple of n!"); });
    if constexpr (policy == AscendAntiQuantPolicy::PER_TOKEN) {
        static_assert(
            SupportType<srcT, int8_t, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t>(),
            "AscendAntiQuant PerToken only support int8_t/fp8_e4m3fn_t/fp8_e5m2_t/hifloat8_t input dtype");
        AscendAntiQuantPerToken<dstT, srcT, scaleT, config>(
            dstTensor, srcTensor, sharedTmpBuffer, scaleTensor, offsetTensor, para);
    } else if constexpr (policy == AscendAntiQuantPolicy::PER_GROUP) {
        static_assert(
            SupportType<srcT, int8_t, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t, fp4x2_e1m2_t, fp4x2_e2m1_t>(),
            "AscendAntiQuant PerGroup only support "
            "int8_t/fp8_e4m3fn_t/fp8_e5m2_t/hifloat8_t/fp4x2_e1m2_t/fp4x2_e2m1_t input dtype");
        static_assert(
            ((config.kDim == 1) || (config.kDim == 0)), "AscendAntiQuant PerGroup only support K is axis 0/1!");
        ASCENDC_ASSERT((para.groupSize > 0 && para.groupSize % 32 == 0), {
            KERNEL_LOG(KERNEL_ERROR, "groupSize must be an integer multiple of 32 and greater than 0 !");
        });
        if constexpr ((config.kDim == 1 && !config.isTranspose) || (config.kDim == 0 && config.isTranspose)) {
            AscendAntiQuantPerGroupForCol<dstT, srcT, scaleT, config>(
                dstTensor, srcTensor, sharedTmpBuffer, scaleTensor, offsetTensor, para);
        } else {
            AscendAntiQuantPerGroupForRow<dstT, srcT, scaleT, config>(
                dstTensor, srcTensor, sharedTmpBuffer, scaleTensor, offsetTensor, para);
        }
    }
}

template <typename SrcType, typename DstType, bool isTranspose>
__aicore__ inline void AscendAntiQuantImplCommon(
    const LocalTensor<DstType>& dst, const LocalTensor<SrcType>& src, const LocalTensor<DstType>& offset,
    const LocalTensor<DstType>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t k,
    const AntiQuantShapeInfo& shapeInfo = {})
{
    CheckTensorPosition(dst, "dst", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(src, "src", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(offset, "offset", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(scale, "scale", "VECIN/VECOUT/VECCALC");
    AntiQuantPerchannelImpl<SrcType, DstType, isTranspose>(dst, src, offset, scale, sharedTmpBuffer, k, shapeInfo);
}

template <typename SrcType, typename DstType, bool isTranspose>
__aicore__ inline void AscendAntiQuantImplCommon(
    const LocalTensor<DstType>& dst, const LocalTensor<SrcType>& src, const DstType offset, const DstType scale,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t k, const AntiQuantShapeInfo& shapeInfo = {})
{
    CheckTensorPosition(dst, "dst", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(src, "src", "VECIN/VECOUT/VECCALC");
    AntiQuantPertensorImpl<SrcType, DstType>(dst, src, offset, scale, sharedTmpBuffer, k, shapeInfo);
}
} // namespace AscendC
#endif // IMPL_QUANTIZATION_ANTIQUANT_ASCEND_ANTIQUANT_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_ANTIQUANT_ASCEND_ANTIQUANT_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_ANTIQUANT_ASCEND_ANTIQUANT_C310_IMPL_H__
#endif
