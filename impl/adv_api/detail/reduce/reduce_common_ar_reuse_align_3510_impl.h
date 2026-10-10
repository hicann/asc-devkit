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
    "impl/adv_api/detail/reduce/reduce_common_ar_reuse_align_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/reduce/reduce.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_COMMON_AR_REUSE_ALIGN_C310_IMPL_H__
#endif

#ifndef IMPL_REDUCE_REDUCE_COMMON_AR_REUSE_ALIGN_C310_IMPL_H
#define IMPL_REDUCE_REDUCE_COMMON_AR_REUSE_ALIGN_C310_IMPL_H

#include "../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../include/basic_api/kernel_tensor.h"
#include "reduce_common_util_impl.h"
#include "reduce_common_util_3510_impl.h"
#include "reduce_common_ar_reuse_align_less_than_vl_3510_impl.h"

namespace AscendC {
template <
    class T, class U, const Reg::RegTrait& Trait, const Reg::CastTrait& CastTraitUppper,
    const Reg::CastTrait& CastTraitLower, const uint16_t vlSize, auto Binaryfunc, auto Reducefunc>
__simd_callee__ inline void ReduceARCastfoldZero(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t dimA, uint32_t dimR, Reg::MaskReg& fullMask)
{
    using UnpackSrcT = typename ReduceOpInternal::ExtractUnsignedTypeBySize<sizeof(T)>::T;
    using UnpackDstT = typename ReduceOpInternal::ExtractUnsignedTypeBySize<sizeof(U)>::T;
    Reg::RegTensor<U, Trait> vreg0CastB32;
    Reg::RegTensor<U, Trait> vreg1CastB32;
    Reg::RegTensor<T, Trait> vreg0;
    Reg::RegTensor<T, Trait> vreg1;
    Reg::UnalignReg uDst;
    Reg::UnalignRegForLoad ureg0;
    for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {
        Reg::LoadAlign(vreg0, srcAddr + loopA * dimR);
        if constexpr (vlSize > ONE_BLK_SIZE) {
            Reg::LoadAlign(vreg1, srcAddr + vlSize / 2 + loopA * dimR);
        } else {
            Reg::LoadUnAlignPre(ureg0, srcAddr + vlSize / 2 + loopA * dimR);
            Reg::LoadUnAlign(vreg1, ureg0, srcAddr + vlSize / 2 + loopA * dimR);
        }
        Binaryfunc(vreg0, vreg0, vreg1, fullMask);
        Reg::UnPack((Reg::RegTensor<UnpackDstT, Trait>&)vreg0, (Reg::RegTensor<UnpackSrcT, Trait>&)vreg0);
        Reg::Cast<U, T, ReduceOpInternal::CastTraitBF16F32>(vreg0CastB32, vreg0, fullMask);
        Reducefunc(vreg1CastB32, vreg0CastB32, fullMask);
        Reg::Cast<T, U, ReduceOpInternal::CastTraitF32BF16>(vreg1, vreg1CastB32, fullMask);
        Reg::StoreUnAlign((__ubuf__ T*&)dstAddr, vreg1, uDst, 1);
    }
    Reg::StoreUnAlignPost((__ubuf__ T*&)dstAddr, uDst, 0);
}

template <class T, const Reg::RegTrait& Trait, auto Binaryfunc, auto Reducefunc>
__simd_callee__ inline void ReduceARfoldOneToThree(
    Reg::RegTensor<T, Trait>& vreg0, Reg::RegTensor<T, Trait>& vreg2, Reg::MaskReg& fullMask)
{
    if constexpr (IsSameType<T, bfloat16_t>::value) {
        ReduceOpInternal::ReduceARCastfoldOneToThree<
            T, float, Trait, ReduceOpInternal::CastTraitBF16F32, ReduceOpInternal::CastTraitF32BF16, Binaryfunc,
            Reducefunc>(vreg0, vreg2, fullMask);
    } else if constexpr (SupportBytes<T, 1>()) {
        ReduceOpInternal::ReduceARCastfoldOneToThree<
            T, half, Trait, ReduceOpInternal::CastTraitB8F16, ReduceOpInternal::CastTraitF16B8, Binaryfunc, Reducefunc>(
            vreg0, vreg2, fullMask);
    } else {
        Reducefunc(vreg2, vreg0, fullMask);
    }
}

template <
    class T, const Reg::RegTrait& Trait, const uint16_t vlSize, auto Binaryfunc, auto Reducefunc, bool isReuseSource>
__simd_vf__ inline void ReduceARB64OverVLVF(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ T* tmpAddr, uint32_t dimA, uint32_t dimR, uint32_t mainR,
    uint32_t tailR, uint16_t dimRAxis, uint16_t inplaceRepeats, uint32_t inplaceTail, uint16_t needInplaceAdd,
    uint16_t copyRepeats, uint16_t base, uint16_t folds, uint16_t avgFolds, uint16_t mainTimes, uint16_t foldZero,
    uint16_t foldOne, uint16_t foldTwo)
{
    __ubuf__ T* addr;
    Reg::MaskReg mask;
    Reg::MaskReg fullMask;
    fullMask = Reg::CreateMask<T, Reg::MaskPattern::ALL, Trait>();
    // Process mainR and tailR
    if constexpr (isReuseSource) {
        addr = srcAddr;
        Reg::RegTensor<T, Trait> b64VregMain;
        Reg::RegTensor<T, Trait> b64VregTail;
        for (uint16_t i = 0; i < needInplaceAdd; i++) {
            uint32_t sreg0 = tailR;
            for (uint16_t loopR = 0; loopR < inplaceRepeats; loopR++) {
                mask = Reg::UpdateMask<T, Trait>(sreg0);
                for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {
                    Reg::LoadAlign(b64VregMain, srcAddr + loopA * dimRAxis + loopR * vlSize);
                    Reg::LoadAlign(b64VregTail, srcAddr + loopA * dimRAxis + mainR + loopR * vlSize);
                    Binaryfunc(b64VregMain, b64VregMain, b64VregTail, mask);
                    Reg::StoreAlign(addr + loopA * dimRAxis + loopR * vlSize, b64VregMain, mask);
                }
            }
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
        }
    } else {
        addr = tmpAddr;
        uint16_t dimRTmp = mainR;
        Reg::RegTensor<T, Trait> b64VregMain;
        Reg::RegTensor<T, Trait> b64VregTail;
        for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {
            // do inplace add tail with main
            for (uint16_t loopR = 0; loopR < inplaceRepeats; loopR++) {
                Reg::LoadAlign(b64VregMain, srcAddr + loopA * dimRAxis + loopR * vlSize);
                Reg::LoadAlign(b64VregTail, srcAddr + loopA * dimRAxis + mainR + loopR * vlSize);
                Binaryfunc(b64VregMain, b64VregMain, b64VregTail, fullMask);
                Reg::StoreAlign(addr + loopA * dimRTmp + loopR * vlSize, b64VregMain, fullMask);
            }
            // do copy main if tail < main
            for (uint16_t loopR = inplaceRepeats; loopR < copyRepeats; loopR++) {
                Reg::LoadAlign(b64VregMain, srcAddr + loopA * dimRAxis + loopR * vlSize);
                Reg::StoreAlign(addr + loopA * dimRTmp + loopR * vlSize, b64VregMain, fullMask);
            }
        }
        // add remainder in tail with first element for each R axis
        // tail process is necessary as mode merging is not supported
        for (uint16_t i = 0; i < needInplaceAdd; i++) {
            mask = Reg::UpdateMask<T, Trait>(inplaceTail);
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
            for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {
                Reg::LoadAlign(b64VregTail, srcAddr + loopA * dimRAxis + mainR + inplaceRepeats * vlSize);
                Reg::LoadAlign(b64VregMain, addr + loopA * dimRTmp);
                Binaryfunc(b64VregMain, b64VregMain, b64VregTail, mask);
                Reg::StoreAlign(addr + loopA * dimRTmp, b64VregMain, mask);
            }
        }
        dimR = mainR;
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
    }

    // MainFolds need 8*2 register
    Reg::RegTensor<T, Trait> b64Vreg0;
    Reg::RegTensor<T, Trait> b64Vreg1;
    Reg::RegTensor<T, Trait> b64Vreg2;
    Reg::RegTensor<T, Trait> b64Vreg3;
    Reg::RegTensor<T, Trait> b64Vreg4;
    Reg::RegTensor<T, Trait> b64Vreg5;
    Reg::RegTensor<T, Trait> b64Vreg6;
    Reg::RegTensor<T, Trait> b64Vreg7;
    Reg::UnalignReg uDst;

    // Process main folds
    uint16_t loopRNum = base;
    for (uint16_t loopMain = 0; loopMain < mainTimes; loopMain++) {
        loopRNum = loopRNum >> avgFolds;
        uint16_t offsetR = loopRNum * vlSize;
        for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {
            for (uint16_t loopR = 0; loopR < loopRNum; loopR++) {
                // L0
                Reg::LoadAlign(b64Vreg0, addr + loopA * dimR + loopR * vlSize);
                Reg::LoadAlign(b64Vreg1, addr + offsetR + loopA * dimR + loopR * vlSize);
                Reg::LoadAlign(b64Vreg2, addr + 2 * offsetR + loopA * dimR + loopR * vlSize);
                Reg::LoadAlign(b64Vreg3, addr + 3 * offsetR + loopA * dimR + loopR * vlSize);
                Reg::LoadAlign(b64Vreg4, addr + 4 * offsetR + loopA * dimR + loopR * vlSize);
                Reg::LoadAlign(b64Vreg5, addr + 5 * offsetR + loopA * dimR + loopR * vlSize);
                Reg::LoadAlign(b64Vreg6, addr + 6 * offsetR + loopA * dimR + loopR * vlSize);
                Reg::LoadAlign(b64Vreg7, addr + 7 * offsetR + loopA * dimR + loopR * vlSize);
                // L1
                Binaryfunc(b64Vreg0, b64Vreg0, b64Vreg4, fullMask);
                Binaryfunc(b64Vreg1, b64Vreg1, b64Vreg5, fullMask);
                Binaryfunc(b64Vreg2, b64Vreg2, b64Vreg6, fullMask);
                Binaryfunc(b64Vreg3, b64Vreg3, b64Vreg7, fullMask);
                // L2
                Binaryfunc(b64Vreg0, b64Vreg0, b64Vreg2, fullMask);
                Binaryfunc(b64Vreg1, b64Vreg1, b64Vreg3, fullMask);
                // L3
                Binaryfunc(b64Vreg0, b64Vreg0, b64Vreg1, fullMask);
                Reg::StoreAlign(addr + loopA * dimR + loopR * vlSize, b64Vreg0, fullMask);
            }
        }
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
    }

    // Process tail folds
    for (uint16_t i = 0; i < foldOne; i++) {
        for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {
            Reg::LoadAlign(b64Vreg0, addr + loopA * dimR);
            Reg::LoadAlign(b64Vreg1, addr + vlSize + loopA * dimR);
            Binaryfunc(b64Vreg0, b64Vreg0, b64Vreg1, fullMask);
            Reducefunc(b64Vreg2, b64Vreg0, fullMask);
            Reg::StoreUnAlign((__ubuf__ T*&)dstAddr, b64Vreg2, uDst, 1);
        }
        Reg::StoreUnAlignPost((__ubuf__ T*&)dstAddr, uDst, 0);
    }

    for (uint16_t i = 0; i < foldTwo; i++) {
        for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {
            // L0
            Reg::LoadAlign(b64Vreg0, addr + loopA * dimR);
            Reg::LoadAlign(b64Vreg1, addr + vlSize + loopA * dimR);
            Reg::LoadAlign(b64Vreg2, addr + 2 * vlSize + loopA * dimR);
            Reg::LoadAlign(b64Vreg3, addr + 3 * vlSize + loopA * dimR);
            // L1
            Binaryfunc(b64Vreg0, b64Vreg0, b64Vreg2, fullMask);
            Binaryfunc(b64Vreg1, b64Vreg1, b64Vreg3, fullMask);
            // L2
            Binaryfunc(b64Vreg0, b64Vreg0, b64Vreg1, fullMask);
            Reducefunc(b64Vreg2, b64Vreg0, fullMask);
            Reg::StoreUnAlign((__ubuf__ T*&)dstAddr, b64Vreg2, uDst, 1);
        }
        Reg::StoreUnAlignPost((__ubuf__ T*&)dstAddr, uDst, 0);
    }

    // Reduce to 1
    uint32_t sreg = mainR;
    for (uint16_t i = 0; i < foldZero; i++) {
        mask = Reg::UpdateMask<T, Trait>(sreg);
        for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {
            Reg::LoadAlign(b64Vreg0, addr + loopA * dimR);
            Reducefunc(b64Vreg1, b64Vreg0, mask);
            Reg::StoreUnAlign((__ubuf__ T*&)dstAddr, b64Vreg1, uDst, 1);
        }
        Reg::StoreUnAlignPost((__ubuf__ T*&)dstAddr, uDst, 0);
    }
}
template <
    class T, const Reg::RegTrait& Trait, const uint16_t vlSize, auto Binaryfunc, auto Reducefunc, bool isReuseSource>
__aicore__ inline void ReduceARB64OverVL(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ T* tmpAddr, uint32_t dimA, uint32_t dimR)
{
    uint32_t mainR = ReduceOpInternal::CalculateMainR(dimR, true, vlSize);
    uint32_t tailR = dimR - mainR;
    // A is at least 8 Bytes, dimRAxis will not over b16 max
    // use b16 dimR to accelerate load store address
    uint16_t dimRAxis = static_cast<uint16_t>(dimR);
    if constexpr (!isReuseSource) {
        mainR = tailR > 0 ? mainR : mainR / 2;
        tailR = tailR > 0 ? tailR : mainR;
    }

    uint16_t inplaceRepeats = 0;
    uint32_t inplaceTail = 0;
    uint16_t needInplaceAdd = 0;
    if constexpr (isReuseSource) {
        inplaceRepeats = CeilDivision(tailR, vlSize);
        needInplaceAdd = tailR > 0 ? 1 : 0;
    } else {
        inplaceRepeats = tailR / vlSize;
        inplaceTail = tailR % vlSize;
        needInplaceAdd = inplaceTail == 0 ? 0 : 1;
    }

    uint32_t dtypeSize = sizeof(T);
    uint16_t copyRepeats = CeilDivision(mainR, vlSize);

    uint16_t base = mainR / vlSize;
    uint16_t folds = ReduceOpInternal::CalculateFolds(base);
    uint16_t avgFolds = ReduceOpInternal::BASE_FOLD_B64;
    uint16_t mainTimes = folds / avgFolds;
    uint16_t tailFolds = folds % avgFolds;
    uint16_t foldZero = (tailFolds == 0) ? 1 : 0;
    uint16_t foldOne = (tailFolds == ReduceOpInternal::FOLD_ONE) ? 1 : 0;
    uint16_t foldTwo = (tailFolds == ReduceOpInternal::FOLD_TWO) ? 1 : 0;

    ReduceARB64OverVLVF<T, Trait, vlSize, Binaryfunc, Reducefunc, isReuseSource>(
        dstAddr, srcAddr, tmpAddr, dimA, dimR, mainR, tailR, dimRAxis, inplaceRepeats, inplaceTail, needInplaceAdd,
        copyRepeats, base, folds, avgFolds, mainTimes, foldZero, foldOne, foldTwo);
}

template <
    class T, const Reg::RegTrait& Trait, const uint16_t vlSize, auto Binaryfunc, auto Reducefunc, bool isReuseSource>
__simd_vf__ inline void ReduceAROverVLVFImpl(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ T* tmpAddr, uint32_t dimA, uint32_t dimR, uint32_t mainR,
    uint32_t tailR, uint16_t dimRAxis, uint16_t inplaceRepeats, uint32_t inplaceTail, uint16_t needInplaceAdd,
    uint16_t copyRepeats, uint16_t base, uint16_t folds, uint16_t avgFolds, uint16_t mainTimes, uint16_t foldZero,
    uint16_t foldOne, uint16_t foldTwo, uint16_t foldThree)
{
    __ubuf__ T* addr;
    Reg::MaskReg mask;
    Reg::MaskReg fullMask;
    Reg::UnalignReg uDst;
    fullMask = Reg::CreateMask<T, Reg::MaskPattern::ALL, Trait>();
    // Process mainR and tailR
    if constexpr (isReuseSource) {
        addr = srcAddr;
        Reg::RegTensor<T, Trait> vregMain;
        Reg::RegTensor<T, Trait> vregTail;
        for (uint16_t i = 0; i < needInplaceAdd; i++) {
            uint32_t sreg0 = tailR;
            for (uint16_t loopR = 0; loopR < inplaceRepeats; loopR++) {
                mask = Reg::UpdateMask<T, Trait>(sreg0);
                for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {
                    Reg::LoadAlign(vregMain, srcAddr + loopA * dimRAxis + loopR * vlSize);
                    Reg::LoadAlign(vregTail, srcAddr + loopA * dimRAxis + mainR + loopR * vlSize);
                    Binaryfunc(vregMain, vregMain, vregTail, mask);
                    Reg::StoreAlign(addr + loopA * dimRAxis + loopR * vlSize, vregMain, mask);
                }
            }
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
        }
    } else {
        addr = tmpAddr;
        uint16_t dimRTmp = mainR;
        Reg::RegTensor<T, Trait> vregMain;
        Reg::RegTensor<T, Trait> vregTail;
        for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {
            // do inplace add tail with main
            for (uint16_t loopR = 0; loopR < inplaceRepeats; loopR++) {
                Reg::LoadAlign(vregMain, srcAddr + loopA * dimRAxis + loopR * vlSize);
                Reg::LoadAlign(vregTail, srcAddr + loopA * dimRAxis + mainR + loopR * vlSize);
                Binaryfunc(vregMain, vregMain, vregTail, fullMask);
                Reg::StoreAlign(addr + loopA * dimRTmp + loopR * vlSize, vregMain, fullMask);
            }
            // do copy main if tail < main
            for (uint16_t loopR = inplaceRepeats; loopR < copyRepeats; loopR++) {
                Reg::LoadAlign(vregMain, srcAddr + loopA * dimRAxis + loopR * vlSize);
                Reg::StoreAlign(addr + loopA * dimRTmp + loopR * vlSize, vregMain, fullMask);
            }
        }
        // add remainder in tail with first element for each R axis
        // tail process is necessary as mode merging is not supported
        for (uint16_t i = 0; i < needInplaceAdd; i++) {
            mask = Reg::UpdateMask<T, Trait>(inplaceTail);
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
            for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {
                Reg::LoadAlign(vregTail, srcAddr + loopA * dimRAxis + mainR + inplaceRepeats * vlSize);
                Reg::LoadAlign(vregMain, addr + loopA * dimRTmp);
                Binaryfunc(vregMain, vregMain, vregTail, mask);
                Reg::StoreAlign(addr + loopA * dimRTmp, vregMain, mask);
            }
        }
        dimR = mainR;
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
    }

    // MainFolds need 16 register
    Reg::RegTensor<T, Trait> vreg0;
    Reg::RegTensor<T, Trait> vreg1;
    Reg::RegTensor<T, Trait> vreg2;
    Reg::RegTensor<T, Trait> vreg3;
    Reg::RegTensor<T, Trait> vreg4;
    Reg::RegTensor<T, Trait> vreg5;
    Reg::RegTensor<T, Trait> vreg6;
    Reg::RegTensor<T, Trait> vreg7;
    Reg::RegTensor<T, Trait> vreg8;
    Reg::RegTensor<T, Trait> vreg9;
    Reg::RegTensor<T, Trait> vreg10;
    Reg::RegTensor<T, Trait> vreg11;
    Reg::RegTensor<T, Trait> vreg12;
    Reg::RegTensor<T, Trait> vreg13;
    Reg::RegTensor<T, Trait> vreg14;
    Reg::RegTensor<T, Trait> vreg15;

    // Process main folds
    uint16_t loopRNum = base;
    for (uint16_t loopMain = 0; loopMain < mainTimes; loopMain++) {
        loopRNum = loopRNum >> avgFolds;
        for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {
            auto tmpSrcAddr = addr + loopA * dimR;
            for (uint16_t loopR = 0; loopR < loopRNum; loopR++) {
                // L0
                Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg0, tmpSrcAddr, vlSize);
                Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg1, tmpSrcAddr, vlSize);
                Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg2, tmpSrcAddr, vlSize);
                Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg3, tmpSrcAddr, vlSize);
                Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg4, tmpSrcAddr, vlSize);
                Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg5, tmpSrcAddr, vlSize);
                Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg6, tmpSrcAddr, vlSize);
                Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg7, tmpSrcAddr, vlSize);
                Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg8, tmpSrcAddr, vlSize);
                Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg9, tmpSrcAddr, vlSize);
                Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg10, tmpSrcAddr, vlSize);
                Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg11, tmpSrcAddr, vlSize);
                Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg12, tmpSrcAddr, vlSize);
                Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg13, tmpSrcAddr, vlSize);
                Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg14, tmpSrcAddr, vlSize);
                Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg15, tmpSrcAddr, vlSize);
                // L1
                Binaryfunc(vreg0, vreg0, vreg8, fullMask);
                Binaryfunc(vreg1, vreg1, vreg9, fullMask);
                Binaryfunc(vreg2, vreg2, vreg10, fullMask);
                Binaryfunc(vreg3, vreg3, vreg11, fullMask);
                Binaryfunc(vreg4, vreg4, vreg12, fullMask);
                Binaryfunc(vreg5, vreg5, vreg13, fullMask);
                Binaryfunc(vreg6, vreg6, vreg14, fullMask);
                Binaryfunc(vreg7, vreg7, vreg15, fullMask);
                // L2
                Binaryfunc(vreg0, vreg0, vreg4, fullMask);
                Binaryfunc(vreg1, vreg1, vreg5, fullMask);
                Binaryfunc(vreg2, vreg2, vreg6, fullMask);
                Binaryfunc(vreg3, vreg3, vreg7, fullMask);
                // L3
                Binaryfunc(vreg0, vreg0, vreg2, fullMask);
                Binaryfunc(vreg1, vreg1, vreg3, fullMask);
                // L4
                Binaryfunc(vreg0, vreg0, vreg1, fullMask);
                Reg::StoreAlign(addr + loopA * dimR + loopR * vlSize, vreg0, fullMask);
            }
        }
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
    }

    // Process tail folds
    for (uint16_t i = 0; i < foldOne; i++) {
        for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {
            Reg::LoadAlign(vreg0, addr + loopA * dimR);
            Reg::LoadAlign(vreg1, addr + vlSize + loopA * dimR);
            Binaryfunc(vreg0, vreg0, vreg1, fullMask);
            ReduceARfoldOneToThree<T, Trait, Binaryfunc, Reducefunc>(vreg0, vreg2, fullMask);
            Reg::StoreUnAlign((__ubuf__ T*&)dstAddr, vreg2, uDst, 1);
        }
        Reg::StoreUnAlignPost((__ubuf__ T*&)dstAddr, uDst, 0);
    }

    for (uint16_t i = 0; i < foldTwo; i++) {
        for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {
            // L0
            Reg::LoadAlign(vreg0, addr + loopA * dimR);
            Reg::LoadAlign(vreg1, addr + vlSize + loopA * dimR);
            Reg::LoadAlign(vreg2, addr + 2 * vlSize + loopA * dimR);
            Reg::LoadAlign(vreg3, addr + 3 * vlSize + loopA * dimR);
            // L1
            Binaryfunc(vreg0, vreg0, vreg2, fullMask);
            Binaryfunc(vreg1, vreg1, vreg3, fullMask);
            // L2
            Binaryfunc(vreg0, vreg0, vreg1, fullMask);
            ReduceARfoldOneToThree<T, Trait, Binaryfunc, Reducefunc>(vreg0, vreg2, fullMask);
            Reg::StoreUnAlign((__ubuf__ T*&)dstAddr, vreg2, uDst, 1);
        }
        Reg::StoreUnAlignPost((__ubuf__ T*&)dstAddr, uDst, 0);
    }

    for (uint16_t i = 0; i < foldThree; i++) {
        for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {
            // L0
            Reg::LoadAlign(vreg0, addr + loopA * dimR);
            Reg::LoadAlign(vreg1, addr + vlSize + loopA * dimR);
            Reg::LoadAlign(vreg2, addr + 2 * vlSize + loopA * dimR);
            Reg::LoadAlign(vreg3, addr + 3 * vlSize + loopA * dimR);
            Reg::LoadAlign(vreg4, addr + 4 * vlSize + loopA * dimR);
            Reg::LoadAlign(vreg5, addr + 5 * vlSize + loopA * dimR);
            Reg::LoadAlign(vreg6, addr + 6 * vlSize + loopA * dimR);
            Reg::LoadAlign(vreg7, addr + 7 * vlSize + loopA * dimR);
            // L1
            Binaryfunc(vreg0, vreg0, vreg4, fullMask);
            Binaryfunc(vreg1, vreg1, vreg5, fullMask);
            Binaryfunc(vreg2, vreg2, vreg6, fullMask);
            Binaryfunc(vreg3, vreg3, vreg7, fullMask);
            // L2
            Binaryfunc(vreg0, vreg0, vreg2, fullMask);
            Binaryfunc(vreg1, vreg1, vreg3, fullMask);
            // L3
            Binaryfunc(vreg0, vreg0, vreg1, fullMask);
            ReduceARfoldOneToThree<T, Trait, Binaryfunc, Reducefunc>(vreg0, vreg2, fullMask);
            Reg::StoreUnAlign((__ubuf__ T*&)dstAddr, vreg2, uDst, 1);
        }
        Reg::StoreUnAlignPost((__ubuf__ T*&)dstAddr, uDst, 0);
    }

    // Reduce to 1
    for (uint16_t i = 0; i < foldZero; i++) {
        if constexpr (IsSameType<T, bfloat16_t>::value) {
            ReduceARCastfoldZero<
                T, float, Trait, ReduceOpInternal::CastTraitBF16F32, ReduceOpInternal::CastTraitF32BF16, vlSize,
                Binaryfunc, Reducefunc>(dstAddr, addr, dimA, dimR, fullMask);
        } else if constexpr (SupportBytes<T, 1>()) {
            ReduceARCastfoldZero<
                T, half, Trait, ReduceOpInternal::CastTraitB8F16, ReduceOpInternal::CastTraitF16B8, vlSize, Binaryfunc,
                Reducefunc>(dstAddr, addr, dimA, dimR, fullMask);
        } else {
            uint32_t sreg = mainR;
            mask = Reg::UpdateMask<T, Trait>(sreg);
            for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {
                Reg::LoadAlign(vreg0, addr + loopA * dimR);
                Reducefunc(vreg1, vreg0, mask);
                Reg::StoreUnAlign((__ubuf__ T*&)dstAddr, vreg1, uDst, 1);
            }
            Reg::StoreUnAlignPost((__ubuf__ T*&)dstAddr, uDst, 0);
        }
    }
}

template <
    class T, const Reg::RegTrait& Trait, const uint16_t vlSize, auto Binaryfunc, auto Reducefunc, bool isReuseSource>
__aicore__ inline void ReduceAROverVLImpl(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ T* tmpAddr, uint32_t dimA, uint32_t dimR)
{
    uint32_t mainR = ReduceOpInternal::CalculateMainR(dimR, true, vlSize);
    uint32_t tailR = dimR - mainR;
    // A is at least 8 Bytes, dimRAxis will not over b16 max
    // use b16 dimR to accelerate load store address
    uint16_t dimRAxis = static_cast<uint16_t>(dimR);
    if constexpr (!isReuseSource) {
        mainR = tailR > 0 ? mainR : mainR / 2;
        tailR = tailR > 0 ? tailR : mainR;
    }

    uint16_t inplaceRepeats = 0;
    uint32_t inplaceTail = 0;
    uint16_t needInplaceAdd = 0;
    if constexpr (isReuseSource) {
        inplaceRepeats = CeilDivision(tailR, vlSize);
        needInplaceAdd = tailR > 0 ? 1 : 0;
    } else {
        inplaceRepeats = tailR / vlSize;
        inplaceTail = tailR % vlSize;
        needInplaceAdd = inplaceTail == 0 ? 0 : 1;
    }

    uint32_t dtypeSize = sizeof(T);
    uint16_t copyRepeats = CeilDivision(mainR, vlSize);

    uint16_t base = mainR / vlSize;
    uint16_t folds = ReduceOpInternal::CalculateFolds(base);
    uint16_t avgFolds = ReduceOpInternal::BASE_FOLD;
    uint16_t mainTimes = folds / avgFolds;
    uint16_t tailFolds = folds % avgFolds;
    uint16_t foldZero = (tailFolds == 0) ? 1 : 0;
    uint16_t foldOne = (tailFolds == ReduceOpInternal::FOLD_ONE) ? 1 : 0;
    uint16_t foldTwo = (tailFolds == ReduceOpInternal::FOLD_TWO) ? 1 : 0;
    uint16_t foldThree = (tailFolds == ReduceOpInternal::FOLD_THREE) ? 1 : 0;

    ReduceAROverVLVFImpl<T, Trait, vlSize, Binaryfunc, Reducefunc, isReuseSource>(
        dstAddr, srcAddr, tmpAddr, dimA, dimR, mainR, tailR, dimRAxis, inplaceRepeats, inplaceTail, needInplaceAdd,
        copyRepeats, base, folds, avgFolds, mainTimes, foldZero, foldOne, foldTwo, foldThree);
}

template <
    class T, const Reg::RegTrait& Trait, auto Binaryfunc, auto Reducefunc, bool isReuseSource = false,
    ReduceType groupReduceType = ReduceType::NONE>
__aicore__ inline void ReduceARImpl(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ T* tmpAddr, uint32_t dimA, uint32_t dimR)
{
    constexpr uint16_t vlSize = SupportBytes<T, 8>() ? GetVecLen() / sizeof(float) : GetVecLen() / sizeof(T);
    if (dimR <= vlSize) {
        ReduceARReuseSourceLessThanVL<T, Trait, vlSize, Binaryfunc, Reducefunc, groupReduceType>(
            dstAddr, srcAddr, dimA, dimR);
    } else {
        if constexpr (SupportBytes<T, 8>()) {
            ReduceARB64OverVL<T, Trait, vlSize, Binaryfunc, Reducefunc, isReuseSource>(
                dstAddr, srcAddr, tmpAddr, dimA, dimR);
        } else {
            ReduceAROverVLImpl<T, Trait, vlSize, Binaryfunc, Reducefunc, isReuseSource>(
                dstAddr, srcAddr, tmpAddr, dimA, dimR);
        }
    }
}
} // namespace AscendC
#endif // IMPL_REDUCE_REDUCE_COMMON_AR_REUSE_ALIGN_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_COMMON_AR_REUSE_ALIGN_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_COMMON_AR_REUSE_ALIGN_C310_IMPL_H__
#endif
