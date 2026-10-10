/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/reduce/reduce_common_ar_reuse_align_less_than_vl_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/reduce/reduce.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_COMMON_AR_REUSE_ALIGN_LESS_THAN_VL_C310_IMPL_H__
#endif

#ifndef IMPL_REDUCE_REDUCE_COMMON_AR_REUSE_ALIGN_LESS_THAN_VL_C310_IMPL_H
#define IMPL_REDUCE_REDUCE_COMMON_AR_REUSE_ALIGN_LESS_THAN_VL_C310_IMPL_H

#include "../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../include/basic_api/kernel_tensor.h"
#include "reduce_common_util_impl.h"
#include "reduce_common_util_3510_impl.h"

namespace AscendC {
template <
    class T, class U, const Reg::RegTrait& Trait, const Reg::CastTrait& CastTraitUppper,
    const Reg::CastTrait& CastTraitLower, const uint16_t vlSize, auto Binaryfunc, auto Reducefunc>
__simd_vf__ inline void ReduceARCastLessThanVL(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t dimA, uint32_t dimR)
{
    if (dimR <= (vlSize / 2)) {
        Reg::RegTensor<T, Trait> vreg0;
        Reg::RegTensor<T, Trait> vreg1;
        Reg::RegTensor<U, Trait> vreg0CastUpper;
        Reg::RegTensor<U, Trait> vreg1CastUpper;
        Reg::UnalignReg uDst;
        uint32_t sreg1 = dimR;
        Reg::MaskReg mask = Reg::UpdateMask<U>(sreg1);
        for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {
            if constexpr (IsSameType<T, bfloat16_t>::value) {
                Reg::LoadAlign<T, Reg::LoadDist::DIST_US_B16>(vreg0, srcAddr + loopA * dimR);
            } else {
                Reg::LoadAlign<T, Reg::LoadDist::DIST_US_B8>(vreg0, srcAddr + loopA * dimR);
            }
            Reg::Cast<U, T, CastTraitUppper>(vreg0CastUpper, vreg0, mask);
            Reducefunc(vreg1CastUpper, vreg0CastUpper, mask);
            Reg::Cast<T, U, CastTraitLower>(vreg1, vreg1CastUpper, mask);
            Reg::StoreUnAlign((__ubuf__ T*&)dstAddr, vreg1, uDst, 1);
        }
        Reg::StoreUnAlignPost((__ubuf__ T*&)dstAddr, uDst, 0);
    } else {
        Reg::RegTensor<T, Trait> vreg0;
        Reg::RegTensor<T, Trait> vreg1;
        Reg::RegTensor<T, Trait> vreg2;
        Reg::RegTensor<U, Trait> vreg0CastB32;
        Reg::RegTensor<U, Trait> vreg1CastB32;
        Reg::UnalignReg uDst;
        Reg::UnalignRegForLoad ureg0;
        uint32_t sreg1 = dimR;
        Reg::MaskReg fullMask = Reg::CreateMask<T, Reg::MaskPattern::ALL, Trait>();
        Reg::MaskReg mask = Reg::UpdateMask<U>(sreg1);
        mask = Reg::UpdateMask<U>(sreg1);
        Reg::MaskPack(mask, mask);
        for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {
            Reg::LoadAlign(vreg0, srcAddr + loopA * dimR);
            if constexpr (vlSize > ONE_BLK_SIZE) {
                Reg::LoadAlign(vreg1, srcAddr + vlSize / 2 + loopA * dimR);
            } else {
                Reg::LoadUnAlignPre(ureg0, srcAddr + vlSize / 2 + loopA * dimR);
                Reg::LoadUnAlign(vreg1, ureg0, srcAddr + vlSize / 2 + loopA * dimR);
            }
            Binaryfunc(vreg2, vreg0, vreg1, mask);
            Select(vreg2, vreg2, vreg0, mask);
            if constexpr (IsSameType<T, bfloat16_t>::value) {
                Reg::UnPack((Reg::RegTensor<uint32_t, Trait>&)vreg2, (Reg::RegTensor<uint16_t, Trait>&)vreg2);
            } else {
                Reg::UnPack((Reg::RegTensor<uint16_t, Trait>&)vreg2, (Reg::RegTensor<uint8_t, Trait>&)vreg2);
            }
            Reg::Cast<U, T, CastTraitUppper>(vreg0CastB32, vreg2, fullMask);
            Reducefunc(vreg1CastB32, vreg0CastB32, fullMask);
            Reg::Cast<T, U, CastTraitLower>(vreg1, vreg1CastB32, fullMask);
            Reg::StoreUnAlign((__ubuf__ T*&)dstAddr, vreg1, uDst, 1);
        }
        Reg::StoreUnAlignPost((__ubuf__ T*&)dstAddr, uDst, 0);
    }
}

template <class T, const Reg::RegTrait& Trait, auto Reducefunc>
__simd_vf__ inline void ReduceARLessThanVL(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t dimA, uint32_t dimR)
{
    Reg::RegTensor<T, Trait> vreg0;
    Reg::RegTensor<T, Trait> vreg1;
    Reg::UnalignReg uDst;
    uint32_t sreg1 = dimR;
    Reg::MaskReg mask = Reg::UpdateMask<T, Trait>(sreg1);
    for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {
        Reg::LoadAlign(vreg0, srcAddr + loopA * dimR);
        Reducefunc(vreg1, vreg0, mask);
        Reg::StoreUnAlign((__ubuf__ T*&)dstAddr, vreg1, uDst, 1);
    }
    Reg::StoreUnAlignPost((__ubuf__ T*&)dstAddr, uDst, 0);
}

template <class T, const Reg::RegTrait& Trait, ReduceType reduceType>
__simd_callee__ inline void GroupReduce(
    Reg::RegTensor<T, Trait>& dst, Reg::RegTensor<T, Trait>& src, Reg::MaskReg& mask)
{
    if constexpr (reduceType == ReduceType::SUM) {
        Reg::ReduceSumWithDataBlock(dst, src, mask);
    } else if constexpr (reduceType == ReduceType::MAX) {
        Reg::ReduceMaxWithDataBlock(dst, src, mask);
    } else if constexpr (reduceType == ReduceType::MIN) {
        Reg::ReduceMinWithDataBlock(dst, src, mask);
    }
}

template <
    class T, const Reg::RegTrait& Trait, const uint16_t vlSize, auto Binaryfunc, auto GroupReduceType,
    bool NeedFoldR = true>
__simd_vf__ inline void GroupReduceVf(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t innerFoldNum, uint16_t fusedA, uint16_t strideA,
    uint32_t outerRepElement)
{
    constexpr uint16_t blockDataLen = GetDataBlockSizeInBytes() / sizeof(T);
    constexpr uint16_t blockNumInVl = vlSize / blockDataLen;
    Reg::RegTensor<T, Trait> vreg0;
    Reg::RegTensor<T, Trait> vreg1;
    Reg::UnalignReg uDst;
    Reg::MaskReg mask;
    const uint16_t innerFoldBinaryNum = innerFoldNum - 1;

    for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(fusedA); ++loopA) {
        mask = Reg::UpdateMask<T, Trait>(outerRepElement);
        if constexpr (NeedFoldR) {
            auto srcAddrFold = srcAddr + blockDataLen;
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
                vreg0, srcAddr, innerFoldNum, strideA, mask);
            for (uint16_t loopR = 0; loopR < innerFoldBinaryNum; ++loopR) {
                Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
                    vreg1, srcAddrFold, innerFoldNum, 1, mask);
                Binaryfunc(vreg0, vreg1, vreg0, mask);
            }
        } else {
            Reg::LoadAlign(vreg0, srcAddr + vlSize * loopA);
        }
        GroupReduce<T, Trait, GroupReduceType>(vreg1, vreg0, mask);
        Reg::StoreUnAlign((__ubuf__ T*&)dstAddr, vreg1, uDst, blockNumInVl);
    }
    Reg::StoreUnAlignPost((__ubuf__ T*&)dstAddr, uDst, blockNumInVl);
}

template <
    class T, const Reg::RegTrait& Trait, const uint16_t vlSize, auto Binaryfunc, auto groupReduceType,
    bool NeedFoldR = true>
__aicore__ inline void GroupReduceARLessThanVL(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t dimA, uint32_t dimR)
{
    constexpr uint16_t blockDataLen = GetDataBlockSizeInBytes() / sizeof(T);
    constexpr uint16_t blockNumInVl = vlSize / blockDataLen;
    const auto innerFoldNum = dimR / blockDataLen;
    const auto fusedA = CeilDivision(dimA, blockNumInVl);
    const auto strideA = (blockNumInVl * dimR) / blockDataLen;
    uint32_t outerRepElement = dimA * blockDataLen;
    GroupReduceVf<T, Trait, vlSize, Binaryfunc, groupReduceType, NeedFoldR>(
        dstAddr, srcAddr, innerFoldNum, fusedA, strideA, outerRepElement);
}

template <
    class T, const Reg::RegTrait& Trait, const uint16_t vlSize, auto Binaryfunc, auto Reducefunc,
    ReduceType groupReduceType = ReduceType::NONE>
__aicore__ inline void ReduceARReuseSourceLessThanVL(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t dimA, uint32_t dimR)
{
    if (dimR == 1) {
        ReduceOpInternal::ReduceCopyOutImpl<T>(dstAddr, srcAddr, dimA);
    } else if constexpr (IsSameType<T, bfloat16_t>::value) {
        ReduceARCastLessThanVL<
            T, float, Trait, ReduceOpInternal::CastTraitBF16F32, ReduceOpInternal::CastTraitF32BF16, vlSize, Binaryfunc,
            Reducefunc>(dstAddr, srcAddr, dimA, dimR);
    } else if constexpr (SupportBytes<T, 1>()) {
        ReduceARCastLessThanVL<
            T, half, Trait, ReduceOpInternal::CastTraitB8F16, ReduceOpInternal::CastTraitF16B8, vlSize, Binaryfunc,
            Reducefunc>(dstAddr, srcAddr, dimA, dimR);
    } else if constexpr (groupReduceType != ReduceType::NONE) {
        if (dimR == GetDataBlockSizeInBytes() / sizeof(T)) {
            GroupReduceARLessThanVL<T, Trait, vlSize, Binaryfunc, groupReduceType, false>(dstAddr, srcAddr, dimA, dimR);
        } else if (dimR < vlSize / 2) {
            GroupReduceARLessThanVL<T, Trait, vlSize, Binaryfunc, groupReduceType, true>(dstAddr, srcAddr, dimA, dimR);
        } else {
            ReduceARLessThanVL<T, Trait, Reducefunc>(dstAddr, srcAddr, dimA, dimR);
        }
    } else {
        ReduceARLessThanVL<T, Trait, Reducefunc>(dstAddr, srcAddr, dimA, dimR);
    }
}
} // namespace AscendC
#endif // IMPL_REDUCE_REDUCE_COMMON_AR_REUSE_ALIGN_LESS_THAN_VL_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_COMMON_AR_REUSE_ALIGN_LESS_THAN_VL_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_COMMON_AR_REUSE_ALIGN_LESS_THAN_VL_C310_IMPL_H__
#endif
