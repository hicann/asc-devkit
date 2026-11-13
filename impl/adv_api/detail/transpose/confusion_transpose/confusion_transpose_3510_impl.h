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
 * \file confusion_transpose_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/transpose/confusion_transpose/confusion_transpose_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/transpose/transdata.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TRANSPOSE_CONFUSION_TRANSPOSE_CONFUSION_TRANSPOSE_C310_IMPL_H__
#endif

#ifndef IMPL_TRANSPOSE_CONFUSION_TRANSPOSE_CONFUSION_TRANSPOSE_C310_IMPL_H
#define IMPL_TRANSPOSE_CONFUSION_TRANSPOSE_CONFUSION_TRANSPOSE_C310_IMPL_H

namespace AscendC {
template <typename T, typename X, typename U, const Reg::RegTrait& Trait, const uint16_t vlSize>
__simd_vf__ inline void ConfusionTransposeCommonGatherVF(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t forLoop0, uint32_t forLoop1, uint32_t forLoop2,
    uint32_t srcStride0, uint32_t srcStride1, uint32_t srcStride2, uint32_t tail, uint32_t count, uint16_t mainLoop,
    uint32_t dtypeSize, uint32_t tailLoop)
{
    Reg::RegTensor<U, Trait> indexReg;
    Reg::RegTensor<T, Trait> MainVreg;
    Reg::MaskReg indexFullMask = Reg::CreateMask<U, Reg::MaskPattern::ALL, Trait>();
    Reg::MaskReg mainMask = Reg::CreateMask<T, Reg::MaskPattern::ALL, Trait>();
    Reg::MaskReg tailMask = Reg::UpdateMask<T, Trait>(count);
    Reg::UnalignReg ureg0;
    Arange((Reg::RegTensor<X, Trait>&)indexReg, static_cast<X>(0));
    Muls(indexReg, indexReg, static_cast<U>(srcStride2), indexFullMask);
    for (uint16_t i = 0; i < static_cast<uint16_t>(forLoop0); i++) {
        for (uint16_t j = 0; j < static_cast<uint16_t>(forLoop1); j++) {
            uint64_t hoistDstAddr = (uint64_t)dstAddr + (uint64_t)((i * forLoop1 + j) * forLoop2 * dtypeSize);
            for (uint16_t k = 0; k < static_cast<uint16_t>(mainLoop); k++) {
                Reg::Gather(
                    MainVreg, srcAddr + i * srcStride0 + j * srcStride1 + k * vlSize * srcStride2, indexReg, mainMask);
                Reg::StoreUnAlign(((__ubuf__ T*&)hoistDstAddr), MainVreg, ureg0, vlSize);
            }
            for (uint16_t k = 0; k < static_cast<uint16_t>(tailLoop); k++) {
                Reg::Gather(
                    MainVreg, srcAddr + i * srcStride0 + j * srcStride1 + mainLoop * vlSize * srcStride2, indexReg,
                    tailMask);
                Reg::StoreUnAlign(((__ubuf__ T*&)hoistDstAddr), MainVreg, ureg0, tail);
            }
            Reg::StoreUnAlignPost(((__ubuf__ T*&)hoistDstAddr), ureg0, 0);
        }
    }
}

template <typename T, typename X, typename U, const Reg::RegTrait& Trait, const uint16_t vlSize>
__aicore__ inline void ConfusionTransposeCommonGather(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t forLoop0, uint32_t forLoop1, uint32_t forLoop2,
    uint32_t srcStride0, uint32_t srcStride1, uint32_t srcStride2)
{
    uint32_t tail = forLoop2 % vlSize;
    uint32_t count = tail;
    uint16_t mainLoop = forLoop2 / vlSize;
    uint32_t dtypeSize = sizeof(T);
    uint32_t tailLoop = tail > 0 ? 1 : 0;
    ConfusionTransposeCommonGatherVF<T, X, U, Trait, vlSize>(
        dstAddr, srcAddr, forLoop0, forLoop1, forLoop2, srcStride0, srcStride1, srcStride2, tail, count, mainLoop,
        dtypeSize, tailLoop);
}

template <typename T, typename X, typename U, const Reg::RegTrait& Trait, const uint16_t vlSize>
__simd_vf__ inline void ConfusionTransposeCommonGatherB8VF(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t forLoop0, uint32_t forLoop1, uint32_t forLoop2,
    uint32_t srcStride0, uint32_t srcStride1, uint32_t srcStride2, uint32_t tail, uint32_t count, uint16_t mainLoop,
    uint32_t dtypeSize, uint32_t tailLoop)
{
    Reg::RegTensor<U, Trait> indexReg;
    Reg::RegTensor<T, Trait> vreg0;
    Reg::RegTensor<T, Trait> vreg1;
    Reg::RegTensor<T, Trait> vreg2;
    Reg::RegTensor<T, Trait> vreg3;
    Reg::MaskReg indexFullMask = Reg::CreateMask<U, Reg::MaskPattern::ALL, Trait>();
    Reg::MaskReg mainMask = Reg::CreateMask<T, Reg::MaskPattern::ALL, Trait>();
    Reg::MaskReg tailMask0 = Reg::UpdateMask<U, Trait>(count);
    Reg::MaskReg tailMask1 = Reg::UpdateMask<U, Trait>(count);
    Reg::UnalignReg ureg0;
    Arange((Reg::RegTensor<X, Trait>&)indexReg, static_cast<X>(0));
    Muls(indexReg, indexReg, static_cast<U>(srcStride2), indexFullMask);
    for (uint16_t i = 0; i < static_cast<uint16_t>(forLoop0); i++) {
        for (uint16_t j = 0; j < static_cast<uint16_t>(forLoop1); j++) {
            uint64_t hoistDstAddr = (uint64_t)dstAddr + (uint64_t)((i * forLoop1 + j) * forLoop2 * dtypeSize);
            for (uint16_t k = 0; k < static_cast<uint16_t>(mainLoop); k++) {
                Reg::Gather(
                    (Reg::RegTensor<U, Trait>&)vreg0,
                    srcAddr + i * srcStride0 + j * srcStride1 + k * vlSize * srcStride2, indexReg, mainMask);
                Reg::Gather(
                    (Reg::RegTensor<U, Trait>&)vreg1,
                    srcAddr + i * srcStride0 + j * srcStride1 + k * vlSize * srcStride2 + vlSize / 2 * srcStride2,
                    indexReg, mainMask);
                DeInterleave(vreg2, vreg3, vreg0, vreg1);
                Reg::StoreUnAlign(((__ubuf__ T*&)hoistDstAddr), vreg2, ureg0, vlSize);
            }
            for (uint16_t k = 0; k < static_cast<uint16_t>(tailLoop); k++) {
                Reg::Gather(
                    (Reg::RegTensor<U, Trait>&)vreg0,
                    srcAddr + i * srcStride0 + j * srcStride1 + mainLoop * vlSize * srcStride2, indexReg, tailMask0);
                Reg::Gather(
                    (Reg::RegTensor<U, Trait>&)vreg1,
                    srcAddr + i * srcStride0 + j * srcStride1 + mainLoop * vlSize * srcStride2 +
                        vlSize / 2 * srcStride2,
                    indexReg, tailMask1);
                DeInterleave(vreg2, vreg3, vreg0, vreg1);
                Reg::StoreUnAlign(((__ubuf__ T*&)hoistDstAddr), vreg2, ureg0, tail);
            }
            Reg::StoreUnAlignPost(((__ubuf__ T*&)hoistDstAddr), ureg0, 0);
        }
    }
}

template <typename T, typename X, typename U, const Reg::RegTrait& Trait, const uint16_t vlSize>
__aicore__ inline void ConfusionTransposeCommonGatherB8(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t forLoop0, uint32_t forLoop1, uint32_t forLoop2,
    uint32_t srcStride0, uint32_t srcStride1, uint32_t srcStride2)
{
    uint32_t tail = forLoop2 % vlSize;
    uint32_t count = tail;
    uint16_t mainLoop = forLoop2 / vlSize;
    uint32_t dtypeSize = sizeof(T);
    uint32_t tailLoop = tail > 0 ? 1 : 0;
    ConfusionTransposeCommonGatherB8VF<T, X, U, Trait, vlSize>(
        dstAddr, srcAddr, forLoop0, forLoop1, forLoop2, srcStride0, srcStride1, srcStride2, tail, count, mainLoop,
        dtypeSize, tailLoop);
}

template <typename T, typename X, typename U, const Reg::RegTrait& Trait, const uint16_t vlSize>
__simd_vf__ inline void ConfusionTransposeComplexGatherVF(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t forLoop0, uint32_t forLoop1, uint32_t forLoop2,
    uint32_t srcStride0, uint32_t srcStride1, uint32_t srcStride2, uint32_t factor, uint32_t mainSize,
    uint16_t mainLoop, uint32_t tail, uint32_t tailLoop, uint32_t mainCount, uint32_t tailCount, uint32_t count,
    uint32_t dtypeSize)
{
    Reg::RegTensor<U, Trait> indexReg0;
    Reg::RegTensor<U, Trait> indexReg1;
    Reg::RegTensor<U, Trait> indexReg2;
    Reg::RegTensor<U, Trait> indexReg3;
    Reg::RegTensor<U, Trait> indexReg4;
    Reg::RegTensor<U, Trait> indexReg5;
    Reg::RegTensor<T, Trait> MainVreg;
    Reg::MaskReg indexFullMask = Reg::CreateMask<U, Reg::MaskPattern::ALL, Trait>();
    Reg::MaskReg mainMask = Reg::UpdateMask<T, Trait>(mainCount);
    Reg::MaskReg tailMask = Reg::UpdateMask<T, Trait>(tailCount);
    Reg::UnalignReg ureg0;
    Arange((Reg::RegTensor<X, Trait>&)indexReg0, static_cast<X>(0));
    Duplicate(indexReg1, static_cast<U>(forLoop2));
    Div(indexReg2, indexReg0, indexReg1, indexFullMask);
    Muls(indexReg3, indexReg2, static_cast<U>(srcStride1), indexFullMask);
    // k%c: k - k/c*c
    Mul(indexReg4, indexReg2, indexReg1, indexFullMask);
    Sub(indexReg4, indexReg0, indexReg4, indexFullMask);
    Muls(indexReg4, indexReg4, static_cast<U>(srcStride2), indexFullMask);
    Add(indexReg5, indexReg3, indexReg4, indexFullMask);
    for (uint16_t i = 0; i < static_cast<uint16_t>(forLoop0); i++) {
        uint64_t hoistDstAddr = (uint64_t)dstAddr + (uint64_t)(i * forLoop1 * forLoop2 * dtypeSize);
        for (uint16_t j = 0; j < static_cast<uint16_t>(mainLoop); j++) {
            Reg::Gather(MainVreg, srcAddr + i * srcStride0 + j * srcStride1 * factor, indexReg5, mainMask);
            Reg::StoreUnAlign(((__ubuf__ T*&)hoistDstAddr), MainVreg, ureg0, mainSize);
        }
        for (uint16_t k = 0; k < static_cast<uint16_t>(tailLoop); k++) {
            Reg::Gather(MainVreg, srcAddr + i * srcStride0 + mainLoop * srcStride1 * factor, indexReg5, tailMask);
            Reg::StoreUnAlign(((__ubuf__ T*&)hoistDstAddr), MainVreg, ureg0, tail);
        }
        Reg::StoreUnAlignPost(((__ubuf__ T*&)hoistDstAddr), ureg0, 0);
    }
}

template <typename T, typename X, typename U, const Reg::RegTrait& Trait, const uint16_t vlSize>
__aicore__ inline void ConfusionTransposeComplexGather(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t forLoop0, uint32_t forLoop1, uint32_t forLoop2,
    uint32_t srcStride0, uint32_t srcStride1, uint32_t srcStride2)
{
    uint32_t factor = vlSize / forLoop2;
    uint32_t mainSize = factor * forLoop2;
    uint16_t mainLoop = forLoop1 / factor;
    uint32_t tail = forLoop1 % factor * forLoop2;
    uint32_t tailLoop = tail > 0 ? 1 : 0;
    uint32_t mainCount = mainSize;
    uint32_t tailCount = tail;

    uint32_t count = tail;
    uint32_t dtypeSize = sizeof(T);
    ConfusionTransposeComplexGatherVF<T, X, U, Trait, vlSize>(
        dstAddr, srcAddr, forLoop0, forLoop1, forLoop2, srcStride0, srcStride1, srcStride2, factor, mainSize, mainLoop,
        tail, tailLoop, mainCount, tailCount, count, dtypeSize);
}

template <typename T, typename X, typename U, const Reg::RegTrait& Trait, const uint16_t vlSize>
__simd_vf__ inline void ConfusionTransposeComplexGatherB8VF(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t forLoop0, uint32_t forLoop1, uint32_t forLoop2,
    uint32_t srcStride0, uint32_t srcStride1, uint32_t srcStride2, uint32_t factor, uint32_t mainSize,
    uint16_t mainLoop, uint32_t tail, uint32_t tailLoop, uint32_t mainCount, uint32_t tailCount, uint32_t dtypeSize,
    uint32_t halfVlSize)
{
    Reg::RegTensor<U, Trait> indexReg0;
    Reg::RegTensor<U, Trait> indexReg1;
    Reg::RegTensor<U, Trait> indexReg2;
    Reg::RegTensor<U, Trait> indexReg3;
    Reg::RegTensor<U, Trait> indexReg4;
    Reg::RegTensor<U, Trait> indexReg5;
    Reg::RegTensor<U, Trait> indexReg6;
    Reg::RegTensor<U, Trait> indexReg7;
    Reg::RegTensor<U, Trait> indexReg8;
    Reg::RegTensor<U, Trait> indexReg9;
    Reg::RegTensor<U, Trait> indexReg10;
    Reg::RegTensor<T, Trait> vreg0;
    Reg::RegTensor<T, Trait> vreg1;
    Reg::RegTensor<T, Trait> vreg2;
    Reg::RegTensor<T, Trait> vreg3;
    Reg::MaskReg indexFullMask = Reg::CreateMask<U, Reg::MaskPattern::ALL, Trait>();
    Reg::MaskReg mainMask0 = Reg::UpdateMask<U, Trait>(mainCount);
    Reg::MaskReg mainMask1 = Reg::UpdateMask<U, Trait>(mainCount);
    Reg::MaskReg tailMask0 = Reg::UpdateMask<U, Trait>(tailCount);
    Reg::MaskReg tailMask1 = Reg::UpdateMask<U, Trait>(tailCount);
    Reg::UnalignReg ureg0;
    Arange((Reg::RegTensor<X, Trait>&)indexReg0, static_cast<X>(0));
    Duplicate(indexReg1, static_cast<U>(forLoop2));
    Div(indexReg2, indexReg0, indexReg1, indexFullMask);
    Muls(indexReg3, indexReg2, static_cast<U>(srcStride1), indexFullMask);
    // k%c: k - k/c*c
    Mul(indexReg4, indexReg2, indexReg1, indexFullMask);
    Sub(indexReg4, indexReg0, indexReg4, indexFullMask);
    Muls(indexReg4, indexReg4, static_cast<U>(srcStride2), indexFullMask);
    Add(indexReg5, indexReg3, indexReg4, indexFullMask);

    // the other half vl index
    Arange((Reg::RegTensor<X, Trait>&)indexReg6, static_cast<X>(halfVlSize));
    Div(indexReg7, indexReg6, indexReg1, indexFullMask);
    Muls(indexReg8, indexReg7, static_cast<U>(srcStride1), indexFullMask);
    // k%c: k - k/c*c
    Mul(indexReg9, indexReg7, indexReg1, indexFullMask);
    Sub(indexReg9, indexReg6, indexReg9, indexFullMask);
    Muls(indexReg9, indexReg9, static_cast<U>(srcStride2), indexFullMask);
    Add(indexReg10, indexReg8, indexReg9, indexFullMask);
    for (uint16_t i = 0; i < static_cast<uint16_t>(forLoop0); i++) {
        uint64_t hoistDstAddr = (uint64_t)dstAddr + (uint64_t)(i * forLoop1 * forLoop2 * dtypeSize);
        for (uint16_t j = 0; j < static_cast<uint16_t>(mainLoop); j++) {
            Reg::Gather(
                (Reg::RegTensor<U, Trait>&)vreg0, srcAddr + i * srcStride0 + j * srcStride1 * factor, indexReg5,
                mainMask0);
            Reg::Gather(
                (Reg::RegTensor<U, Trait>&)vreg1, srcAddr + i * srcStride0 + j * srcStride1 * factor, indexReg10,
                mainMask1);
            DeInterleave(vreg2, vreg3, vreg0, vreg1);
            Reg::StoreUnAlign(((__ubuf__ T*&)hoistDstAddr), vreg2, ureg0, mainSize);
        }
        for (uint16_t k = 0; k < static_cast<uint16_t>(tailLoop); k++) {
            Reg::Gather(
                (Reg::RegTensor<U, Trait>&)vreg0, srcAddr + i * srcStride0 + mainLoop * srcStride1 * factor, indexReg5,
                tailMask0);
            Reg::Gather(
                (Reg::RegTensor<U, Trait>&)vreg1, srcAddr + i * srcStride0 + mainLoop * srcStride1 * factor, indexReg10,
                tailMask1);
            DeInterleave(vreg2, vreg3, vreg0, vreg1);
            Reg::StoreUnAlign(((__ubuf__ T*&)hoistDstAddr), vreg2, ureg0, tail);
        }
        Reg::StoreUnAlignPost(((__ubuf__ T*&)hoistDstAddr), ureg0, 0);
    }
}

template <typename T, typename X, typename U, const Reg::RegTrait& Trait, const uint16_t vlSize>
__aicore__ inline void ConfusionTransposeComplexGatherB8(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t forLoop0, uint32_t forLoop1, uint32_t forLoop2,
    uint32_t srcStride0, uint32_t srcStride1, uint32_t srcStride2)
{
    uint32_t factor = vlSize / forLoop2;
    uint32_t mainSize = factor * forLoop2;
    uint16_t mainLoop = forLoop1 / factor;
    uint32_t tail = forLoop1 % factor * forLoop2;
    uint32_t tailLoop = tail > 0 ? 1 : 0;
    uint32_t mainCount = mainSize;
    uint32_t tailCount = tail;
    uint32_t dtypeSize = sizeof(T);
    uint32_t halfVlSize = vlSize / 2;

    ConfusionTransposeComplexGatherB8VF<T, X, U, Trait, vlSize>(
        dstAddr, srcAddr, forLoop0, forLoop1, forLoop2, srcStride0, srcStride1, srcStride2, factor, mainSize, mainLoop,
        tail, tailLoop, mainCount, tailCount, dtypeSize, halfVlSize);
}

template <typename T, const Reg::RegTrait& Trait, const uint16_t vlSize>
__simd_vf__ inline void ConfusionTransposeCopySrcToDst(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t totalCount)
{
    uint32_t count = totalCount;
    uint32_t forLoop = (totalCount + vlSize - 1) / vlSize;
    Reg::RegTensor<T, Trait> vreg0;
    Reg::MaskReg mainMask;
    for (uint16_t i = 0; i < static_cast<uint16_t>(forLoop); i++) {
        mainMask = Reg::UpdateMask<T, Trait>(count);
        Reg::LoadAlign(vreg0, srcAddr + i * vlSize);
        Reg::StoreAlign(dstAddr + i * vlSize, vreg0, mainMask);
    }
}

template <typename T, const Reg::RegTrait& Trait, const uint16_t vlSize>
__simd_vf__ inline void ConfusionTransposeCommonDataCopyVF(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t forLoop0, uint32_t forLoop1, uint32_t forLoop2,
    uint32_t srcStride0, uint32_t srcStride1, uint32_t tail, uint32_t count, uint16_t mainLoop, uint32_t dtypeSize,
    uint32_t tailLoop)
{
    Reg::RegTensor<T, Trait> MainVreg;
    Reg::UnalignReg DstUreg;
    Reg::UnalignReg SrcUreg;
    for (uint16_t i = 0; i < static_cast<uint16_t>(forLoop0); i++) {
        for (uint16_t j = 0; j < static_cast<uint16_t>(forLoop1); j++) {
            uint64_t hoistSrcAddr = (uint64_t)srcAddr + (uint64_t)((i * srcStride0 + j * srcStride1) * dtypeSize);
            uint64_t hoistDstAddr = (uint64_t)dstAddr + (uint64_t)((i * forLoop1 + j) * forLoop2 * dtypeSize);
            for (uint16_t k = 0; k < static_cast<uint16_t>(mainLoop); k++) {
                Reg::LoadUnAlignPre(SrcUreg, ((__ubuf__ T*&)hoistSrcAddr));
                Reg::LoadUnAlign(MainVreg, SrcUreg, ((__ubuf__ T*&)hoistSrcAddr), vlSize);
                Reg::StoreUnAlign(((__ubuf__ T*&)hoistDstAddr), MainVreg, DstUreg, vlSize);
            }
            for (uint16_t k = 0; k < static_cast<uint16_t>(tailLoop); k++) {
                Reg::LoadUnAlignPre(SrcUreg, ((__ubuf__ T*&)hoistSrcAddr));
                Reg::LoadUnAlign(MainVreg, SrcUreg, ((__ubuf__ T*&)hoistSrcAddr), tail);
                Reg::StoreUnAlign(((__ubuf__ T*&)hoistDstAddr), MainVreg, DstUreg, tail);
            }
            Reg::StoreUnAlignPost(((__ubuf__ T*&)hoistDstAddr), DstUreg, 0);
        }
    }
}

template <typename T, const Reg::RegTrait& Trait, const uint16_t vlSize>
__aicore__ inline void ConfusionTransposeCommonDataCopy(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t forLoop0, uint32_t forLoop1, uint32_t forLoop2,
    uint32_t srcStride0, uint32_t srcStride1)
{
    uint32_t tail = forLoop2 % vlSize;
    uint32_t count = tail;
    uint16_t mainLoop = forLoop2 / vlSize;
    uint32_t dtypeSize = sizeof(T);
    uint32_t tailLoop = tail > 0 ? 1 : 0;
    ConfusionTransposeCommonDataCopyVF<T, Trait, vlSize>(
        dstAddr, srcAddr, forLoop0, forLoop1, forLoop2, srcStride0, srcStride1, tail, count, mainLoop, dtypeSize,
        tailLoop);
}

/*
scene 13：{ shape:[H, W], format:"ND"} -->{ shape:[W, H], format:"ND"};
          { shape:[N, H, W], format:"ND"} -->{ shape:[N, W, H], format:"ND"};
*/
template <typename T>
__aicore__ inline void ConfusionTranspose021(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, ConfusionTranspose021Tiling& tiling)
{
    constexpr uint16_t vlSize = IsSameType<T, int64_t>::value ? GetVecLen() / sizeof(float) : GetVecLen() / sizeof(T);
    if ((tiling.dim1 == 1) || (tiling.dim2 == 1)) {
        uint32_t totalCount = tiling.dim0 * tiling.dim1 * tiling.dim2;
        ConfusionTransposeCopySrcToDst<T, Reg::RegTraitNumOne, vlSize>(
            (__ubuf__ T*)dstTensor.GetPhyAddr(), (__ubuf__ T*)srcTensor.GetPhyAddr(), totalCount);
    } else {
        uint32_t srcStride0 = tiling.dim1 * tiling.dim2;
        uint32_t srcStride1 = 1;
        uint32_t srcStride2 = tiling.dim2;
        if (tiling.dim1 > vlSize / 2) {
            if constexpr (SupportBytes<T, 4>()) {
                ConfusionTransposeCommonGather<T, int32_t, uint32_t, Reg::RegTraitNumOne, vlSize>(
                    (__ubuf__ T*)dstTensor.GetPhyAddr(), (__ubuf__ T*)srcTensor.GetPhyAddr(), tiling.dim0, tiling.dim2,
                    tiling.dim1, srcStride0, srcStride1, srcStride2);
            } else if constexpr (SupportBytes<T, 2>()) {
                ConfusionTransposeCommonGather<T, int16_t, uint16_t, Reg::RegTraitNumOne, vlSize>(
                    (__ubuf__ T*)dstTensor.GetPhyAddr(), (__ubuf__ T*)srcTensor.GetPhyAddr(), tiling.dim0, tiling.dim2,
                    tiling.dim1, srcStride0, srcStride1, srcStride2);
            } else if constexpr (SupportBytes<T, 1>()) {
                ConfusionTransposeCommonGatherB8<T, int16_t, uint16_t, Reg::RegTraitNumOne, vlSize>(
                    (__ubuf__ T*)dstTensor.GetPhyAddr(), (__ubuf__ T*)srcTensor.GetPhyAddr(), tiling.dim0, tiling.dim2,
                    tiling.dim1, srcStride0, srcStride1, srcStride2);
            }
        } else {
            if constexpr (SupportBytes<T, 4>()) {
                ConfusionTransposeComplexGather<T, int32_t, uint32_t, Reg::RegTraitNumOne, vlSize>(
                    (__ubuf__ T*)dstTensor.GetPhyAddr(), (__ubuf__ T*)srcTensor.GetPhyAddr(), tiling.dim0, tiling.dim2,
                    tiling.dim1, srcStride0, srcStride1, srcStride2);
            } else if constexpr (SupportBytes<T, 2>()) {
                ConfusionTransposeComplexGather<T, int16_t, uint16_t, Reg::RegTraitNumOne, vlSize>(
                    (__ubuf__ T*)dstTensor.GetPhyAddr(), (__ubuf__ T*)srcTensor.GetPhyAddr(), tiling.dim0, tiling.dim2,
                    tiling.dim1, srcStride0, srcStride1, srcStride2);
            } else if constexpr (SupportBytes<T, 1>()) {
                ConfusionTransposeComplexGatherB8<T, int16_t, uint16_t, Reg::RegTraitNumOne, vlSize>(
                    (__ubuf__ T*)dstTensor.GetPhyAddr(), (__ubuf__ T*)srcTensor.GetPhyAddr(), tiling.dim0, tiling.dim2,
                    tiling.dim1, srcStride0, srcStride1, srcStride2);
            }
        }
    }
}

/*
scene 14:{ shape:[N, H, W], format:"ND"} -->{ shape:[H, N, W], format:"ND"};
*/
template <typename T>
__aicore__ inline void ConfusionTranspose102(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, ConfusionTranspose102Tiling& tiling)
{
    constexpr uint16_t vlSize = IsSameType<T, int64_t>::value ? GetVecLen() / sizeof(float) : GetVecLen() / sizeof(T);
    if ((tiling.dim0 == 1) || (tiling.dim1 == 1)) {
        uint32_t totalCount = tiling.dim0 * tiling.dim1 * tiling.dim2;
        ConfusionTransposeCopySrcToDst<T, Reg::RegTraitNumOne, vlSize>(
            (__ubuf__ T*)dstTensor.GetPhyAddr(), (__ubuf__ T*)srcTensor.GetPhyAddr(), totalCount);
    } else {
        uint32_t srcStride0 = tiling.dim2;
        uint32_t srcStride1 = tiling.dim1 * tiling.dim2;
        uint32_t srcStride2 = 1;
        if (tiling.dim2 > vlSize / 2) {
            ConfusionTransposeCommonDataCopy<T, Reg::RegTraitNumOne, vlSize>(
                (__ubuf__ T*)dstTensor.GetPhyAddr(), (__ubuf__ T*)srcTensor.GetPhyAddr(), tiling.dim1, tiling.dim0,
                tiling.dim2, srcStride0, srcStride1);
        } else {
            if constexpr (SupportBytes<T, 4>()) {
                ConfusionTransposeComplexGather<T, int32_t, uint32_t, Reg::RegTraitNumOne, vlSize>(
                    (__ubuf__ T*)dstTensor.GetPhyAddr(), (__ubuf__ T*)srcTensor.GetPhyAddr(), tiling.dim1, tiling.dim0,
                    tiling.dim2, srcStride0, srcStride1, srcStride2);
            } else if constexpr (SupportBytes<T, 2>()) {
                ConfusionTransposeComplexGather<T, int16_t, uint16_t, Reg::RegTraitNumOne, vlSize>(
                    (__ubuf__ T*)dstTensor.GetPhyAddr(), (__ubuf__ T*)srcTensor.GetPhyAddr(), tiling.dim1, tiling.dim0,
                    tiling.dim2, srcStride0, srcStride1, srcStride2);
            } else if constexpr (SupportBytes<T, 1>()) {
                ConfusionTransposeComplexGatherB8<T, int16_t, uint16_t, Reg::RegTraitNumOne, vlSize>(
                    (__ubuf__ T*)dstTensor.GetPhyAddr(), (__ubuf__ T*)srcTensor.GetPhyAddr(), tiling.dim1, tiling.dim0,
                    tiling.dim2, srcStride0, srcStride1, srcStride2);
            }
        }
    }
}

/*
scene 15:{ shape:[N, H, W], format:"ND"} -->{ shape:[W, H, N], format:"ND"};
*/
template <typename T>
__aicore__ inline void ConfusionTranspose210(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, ConfusionTranspose210Tiling& tiling)
{
    constexpr uint16_t vlSize = IsSameType<T, int64_t>::value ? GetVecLen() / sizeof(float) : GetVecLen() / sizeof(T);
    ConfusionTranspose021Tiling tiling021 = {1, tiling.dim1, tiling.dim2};
    if (tiling.dim0 == 1 || tiling.dim1 == 1 || tiling.dim2 == 1) {
        ConfusionTranspose021Tiling tiling021 = {1, tiling.dim1, tiling.dim2};
        if (tiling.dim1 == 1) {
            tiling021.dim1 = tiling.dim0;
        } else if (tiling.dim2 == 1) {
            tiling021.dim1 = tiling.dim0;
            tiling021.dim2 = tiling.dim1;
        }
        ConfusionTranspose021(dstTensor, srcTensor, tiling021);
    } else {
        uint32_t srcStride0 = 1;
        uint32_t srcStride1 = tiling.dim2;
        uint32_t srcStride2 = tiling.dim1 * tiling.dim2;
        if (tiling.dim0 > vlSize / 2) {
            if constexpr (SupportBytes<T, 4>()) {
                ConfusionTransposeCommonGather<T, int32_t, uint32_t, Reg::RegTraitNumOne, vlSize>(
                    (__ubuf__ T*)dstTensor.GetPhyAddr(), (__ubuf__ T*)srcTensor.GetPhyAddr(), tiling.dim2, tiling.dim1,
                    tiling.dim0, srcStride0, srcStride1, srcStride2);
            } else if constexpr (SupportBytes<T, 2>()) {
                ConfusionTransposeCommonGather<T, int16_t, uint16_t, Reg::RegTraitNumOne, vlSize>(
                    (__ubuf__ T*)dstTensor.GetPhyAddr(), (__ubuf__ T*)srcTensor.GetPhyAddr(), tiling.dim2, tiling.dim1,
                    tiling.dim0, srcStride0, srcStride1, srcStride2);
            } else if constexpr (SupportBytes<T, 1>()) {
                ConfusionTransposeCommonGatherB8<T, int16_t, uint16_t, Reg::RegTraitNumOne, vlSize>(
                    (__ubuf__ T*)dstTensor.GetPhyAddr(), (__ubuf__ T*)srcTensor.GetPhyAddr(), tiling.dim2, tiling.dim1,
                    tiling.dim0, srcStride0, srcStride1, srcStride2);
            }
        } else {
            if constexpr (SupportBytes<T, 4>()) {
                ConfusionTransposeComplexGather<T, int32_t, uint32_t, Reg::RegTraitNumOne, vlSize>(
                    (__ubuf__ T*)dstTensor.GetPhyAddr(), (__ubuf__ T*)srcTensor.GetPhyAddr(), tiling.dim2, tiling.dim1,
                    tiling.dim0, srcStride0, srcStride1, srcStride2);
            } else if constexpr (SupportBytes<T, 2>()) {
                ConfusionTransposeComplexGather<T, int16_t, uint16_t, Reg::RegTraitNumOne, vlSize>(
                    (__ubuf__ T*)dstTensor.GetPhyAddr(), (__ubuf__ T*)srcTensor.GetPhyAddr(), tiling.dim2, tiling.dim1,
                    tiling.dim0, srcStride0, srcStride1, srcStride2);
            } else if constexpr (SupportBytes<T, 1>()) {
                ConfusionTransposeComplexGatherB8<T, int16_t, uint16_t, Reg::RegTraitNumOne, vlSize>(
                    (__ubuf__ T*)dstTensor.GetPhyAddr(), (__ubuf__ T*)srcTensor.GetPhyAddr(), tiling.dim2, tiling.dim1,
                    tiling.dim0, srcStride0, srcStride1, srcStride2);
            }
        }
    }
}

template <typename T>
__aicore__ inline void ConfusionTransposeND2NZWithInlv(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, ConfusionTranspose210Tiling& tiling)
{
    ConfusionTransposeND2NZWithInlvImpl(
        (__ubuf__ T*)dstTensor.GetPhyAddr(), (__ubuf__ T*)srcTensor.GetPhyAddr(), tiling);
}

template <typename T>
__simd_vf__ inline void ConfusionTransposeND2NZWithInlvVFImpl(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t regWidth, uint32_t c0Size, uint32_t factor, uint32_t height,
    uint32_t width, uint32_t tailWidth, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> vSrcReg0;
    AscendC::Reg::RegTensor<T> vSrcReg1;
    AscendC::Reg::RegTensor<T> vDstReg0;
    AscendC::Reg::RegTensor<T> vDstReg1;
    AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T, Reg::MaskPattern::ALL>();
    uint32_t srcOffset = 0;
    uint32_t dstOffset = 0;
    const uint16_t heightLoopNum = height / factor;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        srcOffset = i * regWidth;
        dstOffset = i * regWidth * heightLoopNum * factor;
        for (uint16_t j = 0; j < heightLoopNum; j++) {
            uint64_t rowStrideAddr = uint64_t(srcAddr + srcOffset + factor * j * width);
            uint64_t neighborStrideAddr = uint64_t(srcAddr + srcOffset + (factor * j + 1) * width);

            AscendC::Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_NORMAL>(
                vSrcReg0, (__ubuf__ T*&)rowStrideAddr, 1, 0, mask);
            AscendC::Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_NORMAL>(
                vSrcReg1, (__ubuf__ T*&)neighborStrideAddr, 1, 0, mask);
            AscendC::Reg::Interleave<T>(vDstReg0, vDstReg1, vSrcReg0, vSrcReg1);

            uint64_t dstRowStrideAddr = uint64_t(dstAddr + dstOffset + j * c0Size);
            AscendC::Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_NORMAL>(
                (__ubuf__ T*&)dstRowStrideAddr, vDstReg0, heightLoopNum, 0, mask);

            uint64_t dstNerborStrideAddr = uint64_t(dstAddr + dstOffset + regWidth * heightLoopNum + j * c0Size);
            AscendC::Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_NORMAL>(
                (__ubuf__ T*&)dstNerborStrideAddr, vDstReg1, heightLoopNum, 0, mask);
        }
    }

    if (tailWidth) {
        if (repeatTimes != 0) {
            srcOffset += regWidth;
            dstOffset += regWidth * heightLoopNum * factor;
        }
        const uint32_t regWidthHalf = GetVecLen() / 2;
        for (uint16_t j = 0; j < heightLoopNum; j++) {
            uint64_t rowStrideAddr = uint64_t(srcAddr + srcOffset + factor * j * width);
            uint64_t neighborStrideAddr = uint64_t(srcAddr + srcOffset + (factor * j + 1) * width);
            uint32_t mask_size = tailWidth * factor;
            if (tailWidth > regWidthHalf) {
                mask_size = regWidth;
            }
            AscendC::Reg::MaskReg mask = AscendC::Reg::UpdateMask<T>(mask_size);
            AscendC::Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_NORMAL>(
                vSrcReg0, (__ubuf__ T*&)rowStrideAddr, 1, 0, mask);
            AscendC::Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_NORMAL>(
                vSrcReg1, (__ubuf__ T*&)neighborStrideAddr, 1, 0, mask);
            AscendC::Reg::Interleave<T>(vDstReg0, vDstReg1, vSrcReg0, vSrcReg1);

            uint64_t dstRowStrideAddr = uint64_t(dstAddr + dstOffset + j * c0Size);
            AscendC::Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_NORMAL>(
                (__ubuf__ T*&)dstRowStrideAddr, vDstReg0, heightLoopNum, 0, mask);

            if (tailWidth > regWidthHalf) {
                mask_size = tailWidth * factor - regWidth;
                AscendC::Reg::MaskReg mask1 = AscendC::Reg::UpdateMask<T>(mask_size);
                uint64_t dstNerborStrideAddr = uint64_t(dstAddr + dstOffset + regWidth * heightLoopNum + j * c0Size);
                AscendC::Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_NORMAL>(
                    (__ubuf__ T*&)dstNerborStrideAddr, vDstReg1, heightLoopNum, 0, mask1);
            }
        }
    }
}

template <typename T>
__aicore__ inline void ConfusionTransposeND2NZWithInlvImpl(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, ConfusionTranspose210Tiling& tiling)
{
    constexpr uint32_t regWidth = GetVecLen(); // vf reg length (VL-general: 3510=256, 5162=32)
    constexpr uint32_t c0Size = 32;            // fractal size
    constexpr uint32_t factor = 2;             // row factor
    uint32_t height = tiling.dim1;
    uint32_t width = tiling.dim2;
    uint32_t tailWidth = width % regWidth;
    uint16_t repeatTimes = width / regWidth;
    ConfusionTransposeND2NZWithInlvVFImpl<T>(
        dstAddr, srcAddr, regWidth, c0Size, factor, height, width, tailWidth, repeatTimes);
}
} // namespace AscendC
#endif // IMPL_TRANSPOSE_CONFUSION_TRANSPOSE_CONFUSION_TRANSPOSE_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TRANSPOSE_CONFUSION_TRANSPOSE_CONFUSION_TRANSPOSE_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TRANSPOSE_CONFUSION_TRANSPOSE_CONFUSION_TRANSPOSE_C310_IMPL_H__
#endif
