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
    "impl/adv_api/detail/reduce/reduce_common_ar_ra_reuse_unalign_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/reduce/reduce.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_COMMON_AR_RA_REUSE_UNALIGN_C310_IMPL_H__
#endif

#ifndef IMPL_REDUCE_REDUCE_COMMON_AR_RA_REUSE_UNALIGN_C310_IMPL_H
#define IMPL_REDUCE_REDUCE_COMMON_AR_RA_REUSE_UNALIGN_C310_IMPL_H

#include "../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../include/basic_api/kernel_tensor.h"
#include "reduce_common_util_impl.h"
#include "reduce_common_util_3510_impl.h"

// Expand shared register stages in place to preserve the compiler input and instruction order.
#define ASC_3510_RA_FOLD_SETUP_AND_MERGE_TAIL(                                                                \
    T, dimR, mainR, folds, avgFolds, dimA, postUpdateStrideTail, Trait, srcAddr, Binaryfunc)                  \
    constexpr uint16_t vlSize = SupportBytes<T, 8>() ? GetVecLen() / sizeof(float) : GetVecLen() / sizeof(T); \
    uint32_t tailR = dimR - mainR;                                                                            \
    uint16_t needInplaceAdd = tailR > 0 ? 1 : 0;                                                              \
    uint16_t base = mainR;                                                                                    \
    uint16_t mainTimes = folds / avgFolds;                                                                    \
    /* Process vlSize axisA each time */                                                                      \
    uint16_t loopANum = (dimA + vlSize - 1) / vlSize;                                                         \
    uint32_t dtypeSize = sizeof(T);                                                                           \
    uint32_t postUpdateStrideMain = vlSize;                                                                   \
    uint32_t tailA = postUpdateStrideTail;                                                                    \
    Reg::RegTensor<T, Trait> vregMain;                                                                        \
    Reg::RegTensor<T, Trait> vregTail;                                                                        \
    Reg::UnalignReg vUregMain;                                                                                \
    Reg::UnalignReg vUregTail;                                                                                \
    Reg::UnalignReg uDst;                                                                                     \
    Reg::MaskReg maskMain = Reg::CreateMask<T, Reg::MaskPattern::ALL, Trait>();                               \
    Reg::MaskReg maskTail = Reg::UpdateMask<T, Trait>(tailA);                                                 \
    for (uint16_t i = 0; i < needInplaceAdd; i++) {                                                           \
        for (uint16_t loopR = 0; loopR < static_cast<uint16_t>(tailR); loopR++) {                             \
            uint64_t hoistMainAddr = (uint64_t)srcAddr + static_cast<uint64_t>(loopR * dimA * dtypeSize);     \
            uint64_t hoistTailAddr = hoistMainAddr + static_cast<uint64_t>(mainR * dimA * dtypeSize);         \
            uint64_t hoistMainReuseAddr = hoistMainAddr;                                                      \
            Reg::LoadUnAlignPre(vUregMain, ((__ubuf__ T*&)hoistMainAddr));                                    \
            Reg::LoadUnAlignPre(vUregTail, ((__ubuf__ T*&)hoistTailAddr));                                    \
            for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(loopANum - 1); loopA++) {                  \
                Reg::LoadUnAlign(vregMain, vUregMain, ((__ubuf__ T*&)hoistMainAddr), postUpdateStrideMain);   \
                Reg::LoadUnAlign(vregTail, vUregTail, ((__ubuf__ T*&)hoistTailAddr), postUpdateStrideMain);   \
                Binaryfunc(vregMain, vregMain, vregTail, maskMain);                                           \
                Reg::StoreUnAlign(((__ubuf__ T*&)hoistMainReuseAddr), vregMain, uDst, postUpdateStrideMain);  \
            }                                                                                                 \
            Reg::LoadUnAlign(vregMain, vUregMain, ((__ubuf__ T*&)hoistMainAddr), postUpdateStrideTail);       \
            Reg::LoadUnAlign(vregTail, vUregTail, ((__ubuf__ T*&)hoistTailAddr), postUpdateStrideTail);       \
            Binaryfunc(vregMain, vregMain, vregTail, maskTail);                                               \
            Reg::StoreUnAlign(((__ubuf__ T*&)hoistMainReuseAddr), vregMain, uDst, postUpdateStrideTail);      \
            Reg::StoreUnAlignPost(((__ubuf__ T*&)hoistMainReuseAddr), uDst, 0);                               \
        }                                                                                                     \
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();                                  \
    }                                                                                                         \
    /* MainFolds need 8*2 register */                                                                         \
    Reg::RegTensor<T, Trait> vreg0;                                                                           \
    Reg::RegTensor<T, Trait> vreg1;                                                                           \
    Reg::RegTensor<T, Trait> vreg2;                                                                           \
    Reg::RegTensor<T, Trait> vreg3;                                                                           \
    Reg::RegTensor<T, Trait> vreg4;                                                                           \
    Reg::RegTensor<T, Trait> vreg5;                                                                           \
    Reg::RegTensor<T, Trait> vreg6;                                                                           \
    Reg::RegTensor<T, Trait> vreg7

#define ASC_3510_REDUCE_FOLD_EIGHT_AND_STORE(                                                                         \
    Binaryfunc, vreg0, vreg4, maskMain, vreg1, vreg5, vreg2, vreg6, vreg3, vreg7, T, dst, uDst, postUpdateStrideMain) \
    Binaryfunc(vreg0, vreg0, vreg4, maskMain);                                                                        \
    Binaryfunc(vreg1, vreg1, vreg5, maskMain);                                                                        \
    Binaryfunc(vreg2, vreg2, vreg6, maskMain);                                                                        \
    Binaryfunc(vreg3, vreg3, vreg7, maskMain);                                                                        \
    /* L2 */                                                                                                          \
    Binaryfunc(vreg0, vreg0, vreg2, maskMain);                                                                        \
    Binaryfunc(vreg1, vreg1, vreg3, maskMain);                                                                        \
    /* L3 */                                                                                                          \
    Binaryfunc(vreg0, vreg0, vreg1, maskMain);                                                                        \
    StoreUnAlign((__ubuf__ T*&)dst, vreg0, uDst, postUpdateStrideMain)

#define ASC_3510_REDUCE_RA_FOLD_REMAINDER(                                                                       \
    src, srcAddr, foldOne, T, Trait, Binaryfunc, dstAddr, loopANum, dimA, vlSize, dtypeSize, maskMain, maskTail, \
    postUpdateStrideMain, postUpdateStrideTail, foldTwo)                                                         \
    src = (uint64_t)srcAddr;                                                                                     \
    for (uint16_t i = 0; i < foldOne; i++) {                                                                     \
        ReduceRAReuseSourceUnAlignedFoldOne<T, Trait, Binaryfunc>(                                               \
            dstAddr, src, loopANum, dimA, vlSize, dtypeSize, maskMain, maskTail, postUpdateStrideMain,           \
            postUpdateStrideTail);                                                                               \
    }                                                                                                            \
    for (uint16_t i = 0; i < foldTwo; i++) {                                                                     \
        ReduceRAReuseSourceUnAlignedFoldTwo<T, Trait, Binaryfunc>(                                               \
            dstAddr, src, loopANum, dimA, vlSize, dtypeSize, maskMain, maskTail, postUpdateStrideMain,           \
            postUpdateStrideTail);                                                                               \
    }

#define ASC_3510_REDUCE_DECLARE_HIGH_REGS(T, Trait, base) \
    Reg::RegTensor<T, Trait> vreg8;                       \
    Reg::RegTensor<T, Trait> vreg9;                       \
    Reg::RegTensor<T, Trait> vreg10;                      \
    Reg::RegTensor<T, Trait> vreg11;                      \
    Reg::RegTensor<T, Trait> vreg12;                      \
    Reg::RegTensor<T, Trait> vreg13;                      \
    Reg::RegTensor<T, Trait> vreg14;                      \
    Reg::RegTensor<T, Trait> vreg15;                      \
    Reg::UnalignReg uSrc;                                 \
    /* Process main folds */                              \
    uint16_t loopRNum = base;                             \
    uint64_t src = 0;                                     \
    uint64_t dst = 0;                                     \
    uint64_t newSrc = 0

#define ASC_3510_AR_FOLD_SETUP_AND_MERGE_TAIL(                                                                \
    T, tailR, folds, avgFolds, postUpdateStrideTail, Trait, dimA, srcAddr, dimR, mainR, Binaryfunc)           \
    constexpr uint16_t vlSize = SupportBytes<T, 8>() ? GetVecLen() / sizeof(float) : GetVecLen() / sizeof(T); \
    uint16_t needInplaceAdd = tailR > 0 ? 1 : 0;                                                              \
    uint16_t inplaceRepeats = (tailR + vlSize - 1) / vlSize;                                                  \
    uint32_t dtypeSize = sizeof(T);                                                                           \
    uint16_t mainTimes = folds / avgFolds;                                                                    \
    uint32_t postUpdateStrideMain = vlSize;                                                                   \
    uint32_t tailA = postUpdateStrideTail;                                                                    \
    Reg::RegTensor<T, Trait> vregMain;                                                                        \
    Reg::RegTensor<T, Trait> vregTail;                                                                        \
    Reg::UnalignReg vUregMain;                                                                                \
    Reg::UnalignReg vUregTail;                                                                                \
    Reg::UnalignReg uDst;                                                                                     \
    Reg::MaskReg fullMask = Reg::CreateMask<T, Reg::MaskPattern::ALL, Trait>();                               \
    Reg::MaskReg maskTail = Reg::UpdateMask<T, Trait>(tailA);                                                 \
    for (uint16_t i = 0; i < needInplaceAdd; i++) {                                                           \
        for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {                              \
            uint64_t hoistMainAddr = (uint64_t)srcAddr + static_cast<uint64_t>(loopA * dimR * dtypeSize);     \
            uint64_t hoistTailAddr = hoistMainAddr + static_cast<uint64_t>(mainR * dtypeSize);                \
            uint64_t hoistMainReuseAddr = hoistMainAddr;                                                      \
            Reg::LoadUnAlignPre(vUregMain, ((__ubuf__ T*&)hoistMainAddr));                                    \
            Reg::LoadUnAlignPre(vUregTail, ((__ubuf__ T*&)hoistTailAddr));                                    \
            for (uint16_t loopR = 0; loopR < static_cast<uint16_t>(inplaceRepeats - 1); loopR++) {            \
                Reg::LoadUnAlign(vregMain, vUregMain, ((__ubuf__ T*&)hoistMainAddr), vlSize);                 \
                Reg::LoadUnAlign(vregTail, vUregTail, ((__ubuf__ T*&)hoistTailAddr), vlSize);                 \
                Binaryfunc(vregMain, vregMain, vregTail, fullMask);                                           \
                Reg::StoreUnAlign(((__ubuf__ T*&)hoistMainReuseAddr), vregMain, uDst, postUpdateStrideMain);  \
                Reg::StoreUnAlignPost(((__ubuf__ T*&)hoistMainReuseAddr), uDst, 0);                           \
            }                                                                                                 \
            Reg::LoadUnAlign(vregMain, vUregMain, ((__ubuf__ T*&)hoistMainAddr), vlSize);                     \
            Reg::LoadUnAlign(vregTail, vUregTail, ((__ubuf__ T*&)hoistTailAddr), vlSize);                     \
            Binaryfunc(vregMain, vregMain, vregTail, maskTail);                                               \
            Reg::StoreUnAlign(((__ubuf__ T*&)hoistMainReuseAddr), vregMain, uDst, postUpdateStrideTail);      \
            Reg::StoreUnAlignPost(((__ubuf__ T*&)hoistMainReuseAddr), uDst, 0);                               \
        }                                                                                                     \
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();                                  \
    }                                                                                                         \
    /* MainFolds need 16 register */                                                                          \
    Reg::RegTensor<T, Trait> vreg0;                                                                           \
    Reg::RegTensor<T, Trait> vreg1;                                                                           \
    Reg::RegTensor<T, Trait> vreg2;                                                                           \
    Reg::RegTensor<T, Trait> vreg3;                                                                           \
    Reg::RegTensor<T, Trait> vreg4;                                                                           \
    Reg::RegTensor<T, Trait> vreg5;                                                                           \
    Reg::RegTensor<T, Trait> vreg6;                                                                           \
    Reg::RegTensor<T, Trait> vreg7

#define ASC_3510_FOLD_TAIL_COLUMNS(                                                                                   \
    src, srcAddr, foldOne, dimA, newSrc, dimR, dtypeSize, vreg0, uSrc, T, vlSize, vreg1, Binaryfunc, fullMask, Trait, \
    Reducefunc, vreg2, SupportBytes, dstAddr, uDst, foldTwo, vreg3)                                                   \
    src = (uint64_t)srcAddr;                                                                                          \
    for (uint16_t i = 0; i < foldOne; i++) {                                                                          \
        for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {                                      \
            newSrc = src + static_cast<uint64_t>(loopA * dimR * dtypeSize);                                           \
            /* L0 */                                                                                                  \
            ASC_REDUCE_3510_LOAD_ONE(vreg0, uSrc, ((__ubuf__ T*&)newSrc), vlSize);                                    \
            Reg::LoadUnAlign(vreg1, uSrc, ((__ubuf__ T*&)newSrc), vlSize);                                            \
            /* L1 */                                                                                                  \
            Binaryfunc(vreg0, vreg0, vreg1, fullMask);                                                                \
            if constexpr (IsSameType<T, bfloat16_t>::value) {                                                         \
                ReduceOpInternal::ReduceARCastfoldOneToThree<                                                         \
                    T, float, Trait, ReduceOpInternal::CastTraitBF16F32, ReduceOpInternal::CastTraitF32BF16,          \
                    Binaryfunc, Reducefunc>(vreg0, vreg2, fullMask);                                                  \
            } else if constexpr (SupportBytes<T, 1>()) {                                                              \
                ReduceOpInternal::ReduceARCastfoldOneToThree<                                                         \
                    T, half, Trait, ReduceOpInternal::CastTraitB8F16, ReduceOpInternal::CastTraitF16B8, Binaryfunc,   \
                    Reducefunc>(vreg0, vreg2, fullMask);                                                              \
            } else {                                                                                                  \
                Reducefunc(vreg2, vreg0, fullMask);                                                                   \
            }                                                                                                         \
            StoreUnAlign((__ubuf__ T*&)dstAddr, vreg2, uDst, 1);                                                      \
        }                                                                                                             \
        Reg::StoreUnAlignPost((__ubuf__ T*&)dstAddr, uDst, 0);                                                        \
    }                                                                                                                 \
    for (uint16_t i = 0; i < foldTwo; i++) {                                                                          \
        for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {                                      \
            newSrc = src + static_cast<uint64_t>(loopA * dimR * dtypeSize);                                           \
            /* L0 */                                                                                                  \
            ASC_REDUCE_3510_LOAD_ONE(vreg0, uSrc, ((__ubuf__ T*&)newSrc), vlSize);                                    \
            Reg::LoadUnAlign(vreg1, uSrc, ((__ubuf__ T*&)newSrc), vlSize);                                            \
            Reg::LoadUnAlign(vreg2, uSrc, ((__ubuf__ T*&)newSrc), vlSize);                                            \
            Reg::LoadUnAlign(vreg3, uSrc, ((__ubuf__ T*&)newSrc), vlSize);                                            \
            /* L1 */                                                                                                  \
            Binaryfunc(vreg0, vreg0, vreg2, fullMask);                                                                \
            Binaryfunc(vreg1, vreg1, vreg3, fullMask);                                                                \
            /* L2 */                                                                                                  \
            Binaryfunc(vreg0, vreg0, vreg1, fullMask);                                                                \
            if constexpr (IsSameType<T, bfloat16_t>::value) {                                                         \
                ReduceOpInternal::ReduceARCastfoldOneToThree<                                                         \
                    T, float, Trait, ReduceOpInternal::CastTraitBF16F32, ReduceOpInternal::CastTraitF32BF16,          \
                    Binaryfunc, Reducefunc>(vreg0, vreg2, fullMask);                                                  \
            } else if constexpr (SupportBytes<T, 1>()) {                                                              \
                ReduceOpInternal::ReduceARCastfoldOneToThree<                                                         \
                    T, half, Trait, ReduceOpInternal::CastTraitB8F16, ReduceOpInternal::CastTraitF16B8, Binaryfunc,   \
                    Reducefunc>(vreg0, vreg2, fullMask);                                                              \
            } else {                                                                                                  \
                Reducefunc(vreg2, vreg0, fullMask);                                                                   \
            }                                                                                                         \
            StoreUnAlign((__ubuf__ T*&)dstAddr, vreg2, uDst, 1);                                                      \
        }                                                                                                             \
        Reg::StoreUnAlignPost((__ubuf__ T*&)dstAddr, uDst, 0);                                                        \
    }

#define ASC_3510_REDUCE_CAST_AND_ACCUMULATE(                                                                         \
    T, Trait, U, vreg0CastB32, vreg1CastB32, dimR, srcAddr, vlSize, dtypeSize, dimA, Binaryfunc, uint32_tVariant,    \
    uint16_tVariant, CastTraitUppper, CastTraitLower, dstAddr)                                                       \
    Reg::RegTensor<T, Trait> vreg0;                                                                                  \
    Reg::RegTensor<T, Trait> vreg1;                                                                                  \
    Reg::RegTensor<T, Trait> vreg2;                                                                                  \
    Reg::RegTensor<U, Trait> vreg0CastB32;                                                                           \
    Reg::RegTensor<U, Trait> vreg1CastB32;                                                                           \
    Reg::UnalignReg uSrc0;                                                                                           \
    Reg::UnalignReg uSrc1;                                                                                           \
    Reg::UnalignReg uDst;                                                                                            \
    uint32_t sreg1 = dimR;                                                                                           \
    Reg::MaskReg fullMask = Reg::CreateMask<T, Reg::MaskPattern::ALL, Trait>();                                      \
    Reg::MaskReg mask = Reg::UpdateMask<U>(sreg1);                                                                   \
    mask = Reg::UpdateMask<U>(sreg1);                                                                                \
    Reg::MaskPack(mask, mask);                                                                                       \
    uint64_t newSrc = (uint64_t)srcAddr + vlSize / 2 * dtypeSize;                                                    \
    for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {                                         \
        Reg::LoadUnAlignPre(uSrc0, ((__ubuf__ T*&)srcAddr));                                                         \
        Reg::LoadUnAlignPre(uSrc1, ((__ubuf__ T*&)newSrc));                                                          \
        Reg::LoadUnAlign(vreg0, uSrc0, ((__ubuf__ T*&)srcAddr), dimR);                                               \
        Reg::LoadUnAlign(vreg1, uSrc1, ((__ubuf__ T*&)newSrc), dimR);                                                \
        Binaryfunc(vreg2, vreg0, vreg1, mask);                                                                       \
        Select(vreg2, vreg2, vreg0, mask);                                                                           \
        Reg::UnPack((Reg::RegTensor<uint32_tVariant, Trait>&)vreg2, (Reg::RegTensor<uint16_tVariant, Trait>&)vreg2); \
        Reg::Cast<U, T, CastTraitUppper>(vreg0CastB32, vreg2, fullMask);                                             \
        Reducefunc(vreg1CastB32, vreg0CastB32, fullMask);                                                            \
        Reg::Cast<T, U, CastTraitLower>(vreg1, vreg1CastB32, fullMask);                                              \
        StoreUnAlign((__ubuf__ T*&)dstAddr, vreg1, uDst, 1);                                                         \
    }                                                                                                                \
    Reg::StoreUnAlignPost((__ubuf__ T*&)dstAddr, uDst, 0)

namespace AscendC {

// Keep each load paired with its unaligned state initialization and preserve the register order.
#define ASC_REDUCE_3510_LOAD_ONE(dst, state, src, stride) \
    Reg::LoadUnAlignPre(state, src);                      \
    Reg::LoadUnAlign(dst, state, src, stride)

#define ASC_REDUCE_3510_LOAD_FOUR(dst0, dst1, dst2, dst3, state, src, stride) \
    ASC_REDUCE_3510_LOAD_ONE(dst0, state, src, stride);                       \
    ASC_REDUCE_3510_LOAD_ONE(dst1, state, src, stride);                       \
    ASC_REDUCE_3510_LOAD_ONE(dst2, state, src, stride);                       \
    ASC_REDUCE_3510_LOAD_ONE(dst3, state, src, stride)

template <class T, const Reg::RegTrait& Trait, auto Binaryfunc>
__simd_callee__ inline void ReduceRAReuseSourceUnAlignedFoldOne(
    __ubuf__ T* dstAddr, uint64_t src, uint16_t loopANum, uint32_t dimA, uint16_t vlSize, uint32_t dtypeSize,
    Reg::MaskReg maskMain, Reg::MaskReg maskTail, uint32_t postUpdateStrideMain, uint32_t postUpdateStrideTail)
{
    uint64_t newSrc;
    Reg::UnalignReg uSrc;
    Reg::UnalignReg uDst;
    Reg::RegTensor<T, Trait> vreg0;
    Reg::RegTensor<T, Trait> vreg1;
    for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(loopANum - 1); loopA++) {
        newSrc = src + static_cast<uint64_t>(loopA * vlSize * dtypeSize);
        // L0
        ASC_REDUCE_3510_LOAD_ONE(vreg0, uSrc, ((__ubuf__ T*&)newSrc), dimA);
        ASC_REDUCE_3510_LOAD_ONE(vreg1, uSrc, ((__ubuf__ T*&)newSrc), dimA);
        // L1
        Binaryfunc(vreg0, vreg0, vreg1, maskMain);
        StoreUnAlign((__ubuf__ T*&)dstAddr, vreg0, uDst, postUpdateStrideMain);
    }
    newSrc = src + static_cast<uint64_t>((loopANum - 1) * vlSize * dtypeSize);
    // L0
    ASC_REDUCE_3510_LOAD_ONE(vreg0, uSrc, ((__ubuf__ T*&)newSrc), dimA);
    ASC_REDUCE_3510_LOAD_ONE(vreg1, uSrc, ((__ubuf__ T*&)newSrc), dimA);
    // L1
    Binaryfunc(vreg0, vreg0, vreg1, maskTail);
    StoreUnAlign((__ubuf__ T*&)dstAddr, vreg0, uDst, postUpdateStrideTail);
    Reg::StoreUnAlignPost((__ubuf__ T*&)dstAddr, uDst, 0);
}

template <class T, const Reg::RegTrait& Trait, auto Binaryfunc>
__simd_callee__ inline void ReduceRAReuseSourceUnAlignedFoldTwo(
    __ubuf__ T* dstAddr, uint64_t src, uint16_t loopANum, uint32_t dimA, uint16_t vlSize, uint32_t dtypeSize,
    Reg::MaskReg maskMain, Reg::MaskReg maskTail, uint32_t postUpdateStrideMain, uint32_t postUpdateStrideTail)
{
    uint64_t newSrc;
    Reg::UnalignReg uSrc;
    Reg::UnalignReg uDst;
    Reg::RegTensor<T, Trait> vreg0;
    Reg::RegTensor<T, Trait> vreg1;
    Reg::RegTensor<T, Trait> vreg2;
    Reg::RegTensor<T, Trait> vreg3;
    for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(loopANum - 1); loopA++) {
        newSrc = src + static_cast<uint64_t>(loopA * vlSize * dtypeSize);
        // L0
        ASC_REDUCE_3510_LOAD_FOUR(vreg0, vreg1, vreg2, vreg3, uSrc, ((__ubuf__ T*&)newSrc), dimA);
        // L1
        Binaryfunc(vreg0, vreg0, vreg2, maskMain);
        Binaryfunc(vreg1, vreg1, vreg3, maskMain);
        // L2
        Binaryfunc(vreg0, vreg0, vreg1, maskMain);
        StoreUnAlign((__ubuf__ T*&)dstAddr, vreg0, uDst, postUpdateStrideMain);
    }
    newSrc = src + static_cast<uint64_t>((loopANum - 1) * vlSize * dtypeSize);
    // L0
    ASC_REDUCE_3510_LOAD_FOUR(vreg0, vreg1, vreg2, vreg3, uSrc, ((__ubuf__ T*&)newSrc), dimA);
    // L1
    Binaryfunc(vreg0, vreg0, vreg2, maskTail);
    Binaryfunc(vreg1, vreg1, vreg3, maskTail);
    // L2
    Binaryfunc(vreg0, vreg0, vreg1, maskTail);
    StoreUnAlign((__ubuf__ T*&)dstAddr, vreg0, uDst, postUpdateStrideTail);
    Reg::StoreUnAlignPost((__ubuf__ T*&)dstAddr, uDst, 0);
}
template <class T, const Reg::RegTrait& Trait, auto Binaryfunc>
__simd_vf__ inline void ReduceRAReuseSourceUnAlignedB64VF(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t dimA, uint32_t dimR, uint32_t mainR, uint16_t folds,
    uint16_t avgFolds, uint16_t foldZero, uint16_t foldOne, uint16_t foldTwo, uint32_t postUpdateStrideTail)
{
    ASC_3510_RA_FOLD_SETUP_AND_MERGE_TAIL(
        T, dimR, mainR, folds, avgFolds, dimA, postUpdateStrideTail, Trait, srcAddr, Binaryfunc);
    Reg::UnalignReg uSrc;

    // Process main folds
    uint16_t loopRNum = base;
    uint64_t src = 0;
    uint64_t dst = 0;
    uint64_t newSrc = 0;
    uint64_t newSrcTail = 0;
    for (uint16_t loopMain = 0; loopMain < mainTimes; loopMain++) {
        loopRNum = loopRNum >> avgFolds;
        uint16_t offsetR = loopRNum * dimA;
        for (uint16_t loopR = 0; loopR < loopRNum; loopR++) {
            src = (uint64_t)srcAddr + static_cast<uint64_t>(loopR * dimA * dtypeSize);
            newSrcTail = src + static_cast<uint64_t>((loopANum - 1) * vlSize * dtypeSize);
            for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(loopANum - 1); loopA++) {
                newSrc = src + static_cast<uint64_t>(loopA * vlSize * dtypeSize);
                dst = newSrc;
                // L0
                ASC_REDUCE_3510_LOAD_FOUR(vreg0, vreg1, vreg2, vreg3, uSrc, ((__ubuf__ T*&)newSrc), offsetR);
                ASC_REDUCE_3510_LOAD_FOUR(vreg4, vreg5, vreg6, vreg7, uSrc, ((__ubuf__ T*&)newSrc), offsetR);
                // L1
                ASC_3510_REDUCE_FOLD_EIGHT_AND_STORE(
                    Binaryfunc, vreg0, vreg4, maskMain, vreg1, vreg5, vreg2, vreg6, vreg3, vreg7, T, dst, uDst,
                    postUpdateStrideMain);
            }
            dst = newSrcTail;
            // L0
            ASC_REDUCE_3510_LOAD_FOUR(vreg0, vreg1, vreg2, vreg3, uSrc, ((__ubuf__ T*&)newSrcTail), offsetR);
            ASC_REDUCE_3510_LOAD_FOUR(vreg4, vreg5, vreg6, vreg7, uSrc, ((__ubuf__ T*&)newSrcTail), offsetR);
            // L1
            ASC_3510_REDUCE_FOLD_EIGHT_AND_STORE(
                Binaryfunc, vreg0, vreg4, maskTail, vreg1, vreg5, vreg2, vreg6, vreg3, vreg7, T, dst, uDst,
                postUpdateStrideTail);
            Reg::StoreUnAlignPost((__ubuf__ T*&)dst, uDst, 0);
        }
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
    }

    // Process tail folds
    ASC_3510_REDUCE_RA_FOLD_REMAINDER(
        src, srcAddr, foldOne, T, Trait, Binaryfunc, dstAddr, loopANum, dimA, vlSize, dtypeSize, maskMain, maskTail,
        postUpdateStrideMain, postUpdateStrideTail, foldTwo)

    // Reduce to 1
    for (uint16_t i = 0; i < foldZero; ++i) {
        for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(loopANum - 1); loopA++) {
            ASC_REDUCE_3510_LOAD_ONE(vreg0, uSrc, ((__ubuf__ T*&)srcAddr), postUpdateStrideMain);
            Reg::StoreUnAlign((__ubuf__ T*&)dstAddr, vreg0, uDst, postUpdateStrideMain);
        }
        ASC_REDUCE_3510_LOAD_ONE(vreg0, uSrc, ((__ubuf__ T*&)srcAddr), postUpdateStrideTail);
        Reg::StoreUnAlign((__ubuf__ T*&)dstAddr, vreg0, uDst, postUpdateStrideTail);
        Reg::StoreUnAlignPost((__ubuf__ T*&)dstAddr, uDst, 0);
    }
}

template <class T, const Reg::RegTrait& Trait, auto Binaryfunc>
__aicore__ inline void ReduceRAReuseSourceUnAlignedB64(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t dimA, uint32_t dimR)
{
    constexpr uint16_t vlSize = SupportBytes<T, 8>() ? GetVecLen() / sizeof(float) : GetVecLen() / sizeof(T);
    uint32_t mainR = ReduceOpInternal::CalculateMainR(dimR, false, vlSize);

    uint16_t folds = ReduceOpInternal::CalculateFolds(mainR);
    uint16_t avgFolds = ReduceOpInternal::BASE_FOLD_B64;
    uint16_t tailFolds = folds % avgFolds;
    uint16_t foldZero = (tailFolds == 0) ? 1 : 0;
    uint16_t foldOne = (tailFolds == ReduceOpInternal::FOLD_ONE) ? 1 : 0;
    uint16_t foldTwo = (tailFolds == ReduceOpInternal::FOLD_TWO) ? 1 : 0;

    // Process vlSize axisA each time
    uint32_t postUpdateStrideTail = dimA % vlSize;

    ReduceRAReuseSourceUnAlignedB64VF<T, Trait, Binaryfunc>(
        dstAddr, srcAddr, dimA, dimR, mainR, folds, avgFolds, foldZero, foldOne, foldTwo, postUpdateStrideTail);
}

template <class T, const Reg::RegTrait& Trait, auto Binaryfunc>
__simd_vf__ inline void ReduceRAReuseSourceUnAlignedVF(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t dimA, uint32_t dimR, uint32_t mainR, uint16_t folds,
    uint16_t avgFolds, uint16_t foldZero, uint16_t foldOne, uint16_t foldTwo, uint16_t foldThree,
    uint32_t postUpdateStrideTail)
{
    // MainFolds need 16 register
    ASC_3510_RA_FOLD_SETUP_AND_MERGE_TAIL(
        T, dimR, mainR, folds, avgFolds, dimA, postUpdateStrideTail, Trait, srcAddr, Binaryfunc);
    ASC_3510_REDUCE_DECLARE_HIGH_REGS(T, Trait, base);
    uint64_t newSrcTail = 0;
    for (uint16_t loopMain = 0; loopMain < mainTimes; loopMain++) {
        loopRNum = loopRNum >> avgFolds;
        uint16_t offsetR = loopRNum * dimA;
        for (uint16_t loopR = 0; loopR < loopRNum; loopR++) {
            src = (uint64_t)srcAddr + static_cast<uint64_t>(loopR * dimA * dtypeSize);
            newSrcTail = src + static_cast<uint64_t>((loopANum - 1) * vlSize * dtypeSize);
            for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(loopANum - 1); loopA++) {
                newSrc = src + static_cast<uint64_t>(loopA * vlSize * dtypeSize);
                dst = newSrc;
                // L0
                ASC_REDUCE_3510_LOAD_FOUR(vreg0, vreg1, vreg2, vreg3, uSrc, ((__ubuf__ T*&)newSrc), offsetR);
                ASC_REDUCE_3510_LOAD_FOUR(vreg4, vreg5, vreg6, vreg7, uSrc, ((__ubuf__ T*&)newSrc), offsetR);
                ASC_REDUCE_3510_LOAD_FOUR(vreg8, vreg9, vreg10, vreg11, uSrc, ((__ubuf__ T*&)newSrc), offsetR);
                ASC_REDUCE_3510_LOAD_FOUR(vreg12, vreg13, vreg14, vreg15, uSrc, ((__ubuf__ T*&)newSrc), offsetR);
                // L1
                Binaryfunc(vreg0, vreg0, vreg8, maskMain);
                Binaryfunc(vreg1, vreg1, vreg9, maskMain);
                Binaryfunc(vreg2, vreg2, vreg10, maskMain);
                Binaryfunc(vreg3, vreg3, vreg11, maskMain);
                Binaryfunc(vreg4, vreg4, vreg12, maskMain);
                Binaryfunc(vreg5, vreg5, vreg13, maskMain);
                Binaryfunc(vreg6, vreg6, vreg14, maskMain);
                Binaryfunc(vreg7, vreg7, vreg15, maskMain);
                // L2
                // L4
                ASC_3510_REDUCE_FOLD_EIGHT_AND_STORE(
                    Binaryfunc, vreg0, vreg4, maskMain, vreg1, vreg5, vreg2, vreg6, vreg3, vreg7, T, dst, uDst,
                    postUpdateStrideMain);
            }
            dst = newSrcTail;
            // L0
            ASC_REDUCE_3510_LOAD_FOUR(vreg0, vreg1, vreg2, vreg3, uSrc, ((__ubuf__ T*&)newSrcTail), offsetR);
            ASC_REDUCE_3510_LOAD_FOUR(vreg4, vreg5, vreg6, vreg7, uSrc, ((__ubuf__ T*&)newSrcTail), offsetR);
            ASC_REDUCE_3510_LOAD_FOUR(vreg8, vreg9, vreg10, vreg11, uSrc, ((__ubuf__ T*&)newSrcTail), offsetR);
            ASC_REDUCE_3510_LOAD_FOUR(vreg12, vreg13, vreg14, vreg15, uSrc, ((__ubuf__ T*&)newSrcTail), offsetR);
            // L1
            Binaryfunc(vreg0, vreg0, vreg8, maskTail);
            Binaryfunc(vreg1, vreg1, vreg9, maskTail);
            Binaryfunc(vreg2, vreg2, vreg10, maskTail);
            Binaryfunc(vreg3, vreg3, vreg11, maskTail);
            Binaryfunc(vreg4, vreg4, vreg12, maskTail);
            Binaryfunc(vreg5, vreg5, vreg13, maskTail);
            Binaryfunc(vreg6, vreg6, vreg14, maskTail);
            Binaryfunc(vreg7, vreg7, vreg15, maskTail);
            // L2
            // L4
            ASC_3510_REDUCE_FOLD_EIGHT_AND_STORE(
                Binaryfunc, vreg0, vreg4, maskTail, vreg1, vreg5, vreg2, vreg6, vreg3, vreg7, T, dst, uDst,
                postUpdateStrideTail);
            Reg::StoreUnAlignPost((__ubuf__ T*&)dst, uDst, 0);
        }
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
    }

    // Process tail folds
    ASC_3510_REDUCE_RA_FOLD_REMAINDER(
        src, srcAddr, foldOne, T, Trait, Binaryfunc, dstAddr, loopANum, dimA, vlSize, dtypeSize, maskMain, maskTail,
        postUpdateStrideMain, postUpdateStrideTail, foldTwo)

    for (uint16_t i = 0; i < foldThree; i++) {
        for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(loopANum - 1); loopA++) {
            newSrc = src + static_cast<uint64_t>(loopA * vlSize * dtypeSize);
            // L0
            ASC_REDUCE_3510_LOAD_FOUR(vreg0, vreg1, vreg2, vreg3, uSrc, ((__ubuf__ T*&)newSrc), dimA);
            ASC_REDUCE_3510_LOAD_FOUR(vreg4, vreg5, vreg6, vreg7, uSrc, ((__ubuf__ T*&)newSrc), dimA);
            // L1
            ASC_3510_REDUCE_FOLD_EIGHT_AND_STORE(
                Binaryfunc, vreg0, vreg4, maskMain, vreg1, vreg5, vreg2, vreg6, vreg3, vreg7, T, dstAddr, uDst,
                postUpdateStrideMain);
        }
        newSrc = src + static_cast<uint64_t>((loopANum - 1) * vlSize * dtypeSize);
        // L0
        ASC_REDUCE_3510_LOAD_FOUR(vreg0, vreg1, vreg2, vreg3, uSrc, ((__ubuf__ T*&)newSrc), dimA);
        ASC_REDUCE_3510_LOAD_FOUR(vreg4, vreg5, vreg6, vreg7, uSrc, ((__ubuf__ T*&)newSrc), dimA);
        // L1
        ASC_3510_REDUCE_FOLD_EIGHT_AND_STORE(
            Binaryfunc, vreg0, vreg4, maskTail, vreg1, vreg5, vreg2, vreg6, vreg3, vreg7, T, dstAddr, uDst,
            postUpdateStrideTail);
        Reg::StoreUnAlignPost((__ubuf__ T*&)dstAddr, uDst, 0);
    }

    // Reduce to 1
    for (uint16_t i = 0; i < foldZero; ++i) {
        for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(loopANum - 1); loopA++) {
            ASC_REDUCE_3510_LOAD_ONE(vreg0, uSrc, ((__ubuf__ T*&)srcAddr), postUpdateStrideMain);
            Reg::StoreUnAlign((__ubuf__ T*&)dstAddr, vreg0, uDst, postUpdateStrideMain);
        }
        ASC_REDUCE_3510_LOAD_ONE(vreg0, uSrc, ((__ubuf__ T*&)srcAddr), postUpdateStrideTail);
        Reg::StoreUnAlign((__ubuf__ T*&)dstAddr, vreg0, uDst, postUpdateStrideTail);
        Reg::StoreUnAlignPost((__ubuf__ T*&)dstAddr, uDst, 0);
    }
}

template <class T, const Reg::RegTrait& Trait, auto Binaryfunc>
__aicore__ inline void ReduceRAReuseSourceUnAligned(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t dimA, uint32_t dimR)
{
    constexpr uint16_t vlSize = SupportBytes<T, 8>() ? GetVecLen() / sizeof(float) : GetVecLen() / sizeof(T);
    uint32_t mainR = ReduceOpInternal::CalculateMainR(dimR, false, vlSize);
    uint16_t folds = ReduceOpInternal::CalculateFolds(mainR);
    uint16_t avgFolds = ReduceOpInternal::BASE_FOLD;
    uint16_t tailFolds = folds % avgFolds;
    uint16_t foldZero = (tailFolds == 0) ? 1 : 0;
    uint16_t foldOne = (tailFolds == ReduceOpInternal::FOLD_ONE) ? 1 : 0;
    uint16_t foldTwo = (tailFolds == ReduceOpInternal::FOLD_TWO) ? 1 : 0;
    uint16_t foldThree = (tailFolds == ReduceOpInternal::FOLD_THREE) ? 1 : 0;
    // Process vlSize axisA each time
    uint32_t postUpdateStrideTail = dimA % vlSize;
    ReduceRAReuseSourceUnAlignedVF<T, Trait, Binaryfunc>(
        dstAddr, srcAddr, dimA, dimR, mainR, folds, avgFolds, foldZero, foldOne, foldTwo, foldThree,
        postUpdateStrideTail);
}

template <
    class T, class U, const Reg::RegTrait& Trait, const Reg::CastTrait& CastTraitUppper,
    const Reg::CastTrait& CastTraitLower, const uint16_t vlSize, auto Binaryfunc, auto Reducefunc>
__simd_callee__ inline void ReduceARCastfoldZeroUnAligned(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t dimA, uint32_t dimR, Reg::MaskReg& fullMask)
{
    Reg::RegTensor<U, Trait> vreg0Cast;
    Reg::RegTensor<U, Trait> vreg1Cast;
    Reg::RegTensor<T, Trait> vreg0;
    Reg::RegTensor<T, Trait> vreg1;
    Reg::UnalignReg uSrc0;
    Reg::UnalignReg uSrc1;
    Reg::UnalignReg uDst;
    for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {
        uint64_t newSrc = (uint64_t)srcAddr + vlSize / 2 * sizeof(T);
        Reg::LoadUnAlignPre(uSrc0, ((__ubuf__ T*&)srcAddr));
        Reg::LoadUnAlignPre(uSrc1, ((__ubuf__ T*&)newSrc));
        Reg::LoadUnAlign(vreg0, uSrc0, ((__ubuf__ T*&)srcAddr), dimR);
        Reg::LoadUnAlign(vreg1, uSrc1, ((__ubuf__ T*&)newSrc), dimR);
        Binaryfunc(vreg0, vreg0, vreg1, fullMask);
        if constexpr (IsSameType<T, bfloat16_t>::value) {
            Reg::UnPack((Reg::RegTensor<uint32_t, Trait>&)vreg0, (Reg::RegTensor<uint16_t, Trait>&)vreg0);
        } else {
            Reg::UnPack((Reg::RegTensor<uint16_t, Trait>&)vreg0, (Reg::RegTensor<uint8_t, Trait>&)vreg0);
        }
        if constexpr (IsSameType<T, bfloat16_t>::value) {
            Reg::Cast<U, T, ReduceOpInternal::CastTraitBF16F32>(vreg0Cast, vreg0, fullMask);
            Reducefunc(vreg1Cast, vreg0Cast, fullMask);
            Reg::Cast<T, U, ReduceOpInternal::CastTraitF32BF16>(vreg1, vreg1Cast, fullMask);
        } else {
            Reg::Cast<U, T, ReduceOpInternal::CastTraitB8F16>(vreg0Cast, vreg0, fullMask);
            Reducefunc(vreg1Cast, vreg0Cast, fullMask);
            Reg::Cast<T, U, ReduceOpInternal::CastTraitF16B8>(vreg1, vreg1Cast, fullMask);
        }
        StoreUnAlign((__ubuf__ T*&)dstAddr, vreg1, uDst, 1);
    }
    Reg::StoreUnAlignPost((__ubuf__ T*&)dstAddr, uDst, 0);
}

template <class T, const Reg::RegTrait& Trait, auto Binaryfunc, auto Reducefunc>
__simd_vf__ inline void ReduceAROverVLReuseSourceUnAlignedB64VF(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t dimA, uint32_t dimR, uint32_t mainR, uint32_t tailR,
    uint16_t base, uint16_t folds, uint16_t avgFolds, uint16_t foldZero, uint16_t foldOne, uint16_t foldTwo,
    uint32_t postUpdateStrideTail)
{
    ASC_3510_AR_FOLD_SETUP_AND_MERGE_TAIL(
        T, tailR, folds, avgFolds, postUpdateStrideTail, Trait, dimA, srcAddr, dimR, mainR, Binaryfunc);
    Reg::UnalignReg uSrc;

    // Process main folds
    uint16_t loopRNum = base;
    uint64_t src = 0;
    uint64_t dst = 0;
    uint64_t newSrc = 0;
    for (uint16_t loopMain = 0; loopMain < mainTimes; loopMain++) {
        loopRNum = loopRNum >> avgFolds;
        uint16_t offsetR = loopRNum * vlSize;

        for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {
            src = (uint64_t)srcAddr + static_cast<uint64_t>(loopA * dimR * dtypeSize);
            dst = src;
            for (uint16_t loopR = 0; loopR < loopRNum; loopR++) {
                newSrc = src + static_cast<uint64_t>(loopR * vlSize * dtypeSize);
                // L0
                ASC_REDUCE_3510_LOAD_FOUR(vreg0, vreg1, vreg2, vreg3, uSrc, ((__ubuf__ T*&)newSrc), offsetR);
                ASC_REDUCE_3510_LOAD_FOUR(vreg4, vreg5, vreg6, vreg7, uSrc, ((__ubuf__ T*&)newSrc), offsetR);
                // L1
                ASC_3510_REDUCE_FOLD_EIGHT_AND_STORE(
                    Binaryfunc, vreg0, vreg4, fullMask, vreg1, vreg5, vreg2, vreg6, vreg3, vreg7, T, dst, uDst, vlSize);
            }
            Reg::StoreUnAlignPost((__ubuf__ T*&)dst, uDst, 0);
        }
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
    }

    // Process tail folds
    ASC_3510_FOLD_TAIL_COLUMNS(
        src, srcAddr, foldOne, dimA, newSrc, dimR, dtypeSize, vreg0, uSrc, T, vlSize, vreg1, Binaryfunc, fullMask,
        Trait, Reducefunc, vreg2, SupportBytes, dstAddr, uDst, foldTwo, vreg3)

    // Reduce to 1
    uint32_t sreg1 = mainR;
    for (uint16_t i = 0; i < foldZero; i++) {
        if constexpr (IsSameType<T, bfloat16_t>::value) {
            ReduceARCastfoldZeroUnAligned<
                T, float, Trait, ReduceOpInternal::CastTraitBF16F32, ReduceOpInternal::CastTraitF32BF16, vlSize,
                Binaryfunc, Reducefunc>(dstAddr, srcAddr, dimA, dimR, fullMask);
        } else if constexpr (SupportBytes<T, 1>()) {
            ReduceARCastfoldZeroUnAligned<
                T, half, Trait, ReduceOpInternal::CastTraitB8F16, ReduceOpInternal::CastTraitF16B8, vlSize, Binaryfunc,
                Reducefunc>(dstAddr, srcAddr, dimA, dimR, fullMask);
        } else {
            Reg::MaskReg mask = Reg::UpdateMask<T, Trait>(sreg1);
            for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {
                newSrc = src + static_cast<uint64_t>(loopA * dimR * dtypeSize);
                ASC_REDUCE_3510_LOAD_ONE(vreg0, uSrc, ((__ubuf__ T*&)newSrc), vlSize);
                Reducefunc(vreg1, vreg0, mask);
                Reg::StoreUnAlign((__ubuf__ T*&)dstAddr, vreg1, uDst, 1);
            }
            Reg::StoreUnAlignPost((__ubuf__ T*&)dstAddr, uDst, 0);
        }
    }
}

template <class T, const Reg::RegTrait& Trait, auto Binaryfunc, auto Reducefunc>
__aicore__ inline void ReduceAROverVLReuseSourceUnAlignedB64(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t dimA, uint32_t dimR)
{
    constexpr uint16_t vlSize = SupportBytes<T, 8>() ? GetVecLen() / sizeof(float) : GetVecLen() / sizeof(T);
    uint32_t mainR = ReduceOpInternal::CalculateMainR(dimR, true, vlSize);
    uint32_t tailR = dimR - mainR;
    uint16_t base = mainR / vlSize;
    uint16_t folds = ReduceOpInternal::CalculateFolds(base);
    uint16_t avgFolds = ReduceOpInternal::BASE_FOLD_B64;
    uint16_t tailFolds = folds % avgFolds;
    uint16_t foldZero = (tailFolds == 0) ? 1 : 0;
    uint16_t foldOne = (tailFolds == ReduceOpInternal::FOLD_ONE) ? 1 : 0;
    uint16_t foldTwo = (tailFolds == ReduceOpInternal::FOLD_TWO) ? 1 : 0;
    uint32_t postUpdateStrideTail = tailR % vlSize;
    ReduceAROverVLReuseSourceUnAlignedB64VF<T, Trait, Binaryfunc, Reducefunc>(
        dstAddr, srcAddr, dimA, dimR, mainR, tailR, base, folds, avgFolds, foldZero, foldOne, foldTwo,
        postUpdateStrideTail);
}

template <class T, const Reg::RegTrait& Trait, auto Binaryfunc, auto Reducefunc>
__simd_vf__ inline void ReduceAROverVLReuseSourceUnAlignedVF(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t dimA, uint32_t dimR, uint32_t mainR, uint32_t tailR,
    uint16_t base, uint16_t folds, uint16_t avgFolds, uint16_t foldZero, uint16_t foldOne, uint16_t foldTwo,
    uint16_t foldThree, uint32_t postUpdateStrideTail)
{
    ASC_3510_AR_FOLD_SETUP_AND_MERGE_TAIL(
        T, tailR, folds, avgFolds, postUpdateStrideTail, Trait, dimA, srcAddr, dimR, mainR, Binaryfunc);
    ASC_3510_REDUCE_DECLARE_HIGH_REGS(T, Trait, base);
    for (uint16_t loopMain = 0; loopMain < mainTimes; loopMain++) {
        loopRNum = loopRNum >> avgFolds;
        uint16_t offsetR = loopRNum * vlSize;

        for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {
            src = (uint64_t)srcAddr + static_cast<uint64_t>(loopA * dimR * dtypeSize);
            dst = src;
            for (uint16_t loopR = 0; loopR < loopRNum; loopR++) {
                newSrc = src + static_cast<uint64_t>(loopR * vlSize * dtypeSize);
                // L0
                ASC_REDUCE_3510_LOAD_FOUR(vreg0, vreg1, vreg2, vreg3, uSrc, ((__ubuf__ T*&)newSrc), offsetR);
                ASC_REDUCE_3510_LOAD_FOUR(vreg4, vreg5, vreg6, vreg7, uSrc, ((__ubuf__ T*&)newSrc), offsetR);
                ASC_REDUCE_3510_LOAD_FOUR(vreg8, vreg9, vreg10, vreg11, uSrc, ((__ubuf__ T*&)newSrc), offsetR);
                ASC_REDUCE_3510_LOAD_FOUR(vreg12, vreg13, vreg14, vreg15, uSrc, ((__ubuf__ T*&)newSrc), offsetR);
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
                ASC_3510_REDUCE_FOLD_EIGHT_AND_STORE(
                    Binaryfunc, vreg0, vreg4, fullMask, vreg1, vreg5, vreg2, vreg6, vreg3, vreg7, T, dst, uDst, vlSize);
            }
            Reg::StoreUnAlignPost((__ubuf__ T*&)dst, uDst, 0);
        }
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
    }

    // Process tail folds
    ASC_3510_FOLD_TAIL_COLUMNS(
        src, srcAddr, foldOne, dimA, newSrc, dimR, dtypeSize, vreg0, uSrc, T, vlSize, vreg1, Binaryfunc, fullMask,
        Trait, Reducefunc, vreg2, SupportBytes, dstAddr, uDst, foldTwo, vreg3)

    for (uint16_t i = 0; i < foldThree; i++) {
        for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {
            newSrc = src + static_cast<uint64_t>(loopA * dimR * dtypeSize);
            // L0
            ASC_REDUCE_3510_LOAD_ONE(vreg0, uSrc, ((__ubuf__ T*&)newSrc), vlSize);
            Reg::LoadUnAlign(vreg1, uSrc, ((__ubuf__ T*&)newSrc), vlSize);
            Reg::LoadUnAlign(vreg2, uSrc, ((__ubuf__ T*&)newSrc), vlSize);
            Reg::LoadUnAlign(vreg3, uSrc, ((__ubuf__ T*&)newSrc), vlSize);
            Reg::LoadUnAlign(vreg4, uSrc, ((__ubuf__ T*&)newSrc), vlSize);
            Reg::LoadUnAlign(vreg5, uSrc, ((__ubuf__ T*&)newSrc), vlSize);
            Reg::LoadUnAlign(vreg6, uSrc, ((__ubuf__ T*&)newSrc), vlSize);
            Reg::LoadUnAlign(vreg7, uSrc, ((__ubuf__ T*&)newSrc), vlSize);
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
            if constexpr (IsSameType<T, bfloat16_t>::value) {
                ReduceOpInternal::ReduceARCastfoldOneToThree<
                    T, float, Trait, ReduceOpInternal::CastTraitBF16F32, ReduceOpInternal::CastTraitF32BF16, Binaryfunc,
                    Reducefunc>(vreg0, vreg2, fullMask);
            } else if constexpr (SupportBytes<T, 1>()) {
                ReduceOpInternal::ReduceARCastfoldOneToThree<
                    T, half, Trait, ReduceOpInternal::CastTraitB8F16, ReduceOpInternal::CastTraitF16B8, Binaryfunc,
                    Reducefunc>(vreg0, vreg2, fullMask);
            } else {
                Reducefunc(vreg2, vreg0, fullMask);
            }
            StoreUnAlign((__ubuf__ T*&)dstAddr, vreg2, uDst, 1);
        }
        Reg::StoreUnAlignPost((__ubuf__ T*&)dstAddr, uDst, 0);
    }

    // Reduce to 1
    for (uint16_t i = 0; i < foldZero; i++) {
        if constexpr (IsSameType<T, bfloat16_t>::value) {
            ReduceARCastfoldZeroUnAligned<
                T, float, Trait, ReduceOpInternal::CastTraitBF16F32, ReduceOpInternal::CastTraitF32BF16, vlSize,
                Binaryfunc, Reducefunc>(dstAddr, srcAddr, dimA, dimR, fullMask);
        } else if constexpr (SupportBytes<T, 1>()) {
            ReduceARCastfoldZeroUnAligned<
                T, half, Trait, ReduceOpInternal::CastTraitB8F16, ReduceOpInternal::CastTraitF16B8, vlSize, Binaryfunc,
                Reducefunc>(dstAddr, srcAddr, dimA, dimR, fullMask);
        } else {
            for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {
                newSrc = src + static_cast<uint64_t>(loopA * dimR * dtypeSize);
                ASC_REDUCE_3510_LOAD_ONE(vreg0, uSrc, ((__ubuf__ T*&)newSrc), vlSize);
                Reducefunc(vreg1, vreg0, fullMask);
                Reg::StoreUnAlign((__ubuf__ T*&)dstAddr, vreg1, uDst, 1);
            }
            Reg::StoreUnAlignPost((__ubuf__ T*&)dstAddr, uDst, 0);
        }
    }
}

#undef ASC_REDUCE_3510_LOAD_FOUR
#undef ASC_REDUCE_3510_LOAD_ONE

template <class T, const Reg::RegTrait& Trait, auto Binaryfunc, auto Reducefunc>
__aicore__ inline void ReduceAROverVLReuseSourceUnAligned(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t dimA, uint32_t dimR)
{
    constexpr uint16_t vlSize = SupportBytes<T, 8>() ? GetVecLen() / sizeof(float) : GetVecLen() / sizeof(T);
    uint32_t mainR = ReduceOpInternal::CalculateMainR(dimR, true, vlSize);
    uint32_t tailR = dimR - mainR;
    uint16_t base = mainR / vlSize;
    uint16_t folds = ReduceOpInternal::CalculateFolds(base);
    uint16_t avgFolds = ReduceOpInternal::BASE_FOLD;
    uint16_t tailFolds = folds % avgFolds;
    uint16_t foldZero = (tailFolds == 0) ? 1 : 0;
    uint16_t foldOne = (tailFolds == ReduceOpInternal::FOLD_ONE) ? 1 : 0;
    uint16_t foldTwo = (tailFolds == ReduceOpInternal::FOLD_TWO) ? 1 : 0;
    uint16_t foldThree = (tailFolds == ReduceOpInternal::FOLD_THREE) ? 1 : 0;
    uint32_t postUpdateStrideTail = tailR % vlSize;
    ReduceAROverVLReuseSourceUnAlignedVF<T, Trait, Binaryfunc, Reducefunc>(
        dstAddr, srcAddr, dimA, dimR, mainR, tailR, base, folds, avgFolds, foldZero, foldOne, foldTwo, foldThree,
        postUpdateStrideTail);
}

template <
    class T, class U, const Reg::RegTrait& Trait, const Reg::CastTrait& CastTraitUppper,
    const Reg::CastTrait& CastTraitLower, const uint16_t vlSize, auto Binaryfunc, auto Reducefunc>
__simd_vf__ inline void CastReduceARUnAligned(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t dimA, uint32_t dimR)
{
    uint32_t dtypeSize = sizeof(T);
    if (dimR <= (vlSize / 2)) {
        if constexpr (SupportBytes<T, 2>()) {
            Reg::RegTensor<T, Trait> vreg0;
            Reg::RegTensor<T, Trait> vreg1;
            Reg::RegTensor<U, Trait> vreg0CastB32;
            Reg::RegTensor<U, Trait> vreg1CastB32;
            Reg::UnalignReg uSrc;
            Reg::UnalignReg uDst;
            uint32_t sreg1 = dimR;
            Reg::MaskReg mask = Reg::UpdateMask<U>(sreg1);
            Reg::LoadUnAlignPre(uSrc, ((__ubuf__ T*&)srcAddr));
            for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {
                Reg::LoadUnAlign(vreg0, uSrc, ((__ubuf__ T*&)srcAddr), dimR);
                Reg::UnPack((Reg::RegTensor<uint32_t, Trait>&)vreg0, (Reg::RegTensor<uint16_t, Trait>&)vreg0);
                Reg::Cast<U, T, CastTraitUppper>(vreg0CastB32, vreg0, mask);
                Reducefunc(vreg1CastB32, vreg0CastB32, mask);
                Reg::Cast<T, U, CastTraitLower>(vreg1, vreg1CastB32, mask);
                StoreUnAlign((__ubuf__ T*&)dstAddr, vreg1, uDst, 1);
            }
            Reg::StoreUnAlignPost((__ubuf__ T*&)dstAddr, uDst, 0);
        } else if constexpr (SupportBytes<T, 1>()) {
            Reg::RegTensor<T, Trait> vreg0;
            Reg::RegTensor<T, Trait> vreg1;
            Reg::RegTensor<U, Trait> vreg0CastB16;
            Reg::RegTensor<U, Trait> vreg1CastB16;
            Reg::UnalignReg uSrc;
            Reg::UnalignReg uDst;
            uint32_t sreg1 = dimR;
            Reg::MaskReg mask = Reg::UpdateMask<U>(sreg1);
            Reg::LoadUnAlignPre(uSrc, ((__ubuf__ T*&)srcAddr));
            for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {
                Reg::LoadUnAlign(vreg0, uSrc, ((__ubuf__ T*&)srcAddr), dimR);
                Reg::UnPack((Reg::RegTensor<uint16_t, Trait>&)vreg0, (Reg::RegTensor<uint8_t, Trait>&)vreg0);
                Reg::Cast<U, T, CastTraitUppper>(vreg0CastB16, vreg0, mask);
                Reducefunc(vreg1CastB16, vreg0CastB16, mask);
                Reg::Cast<T, U, CastTraitLower>(vreg1, vreg1CastB16, mask);
                StoreUnAlign((__ubuf__ T*&)dstAddr, vreg1, uDst, 1);
            }
            Reg::StoreUnAlignPost((__ubuf__ T*&)dstAddr, uDst, 0);
        }
    } else {
        if constexpr (SupportBytes<T, 2>()) {
            ASC_3510_REDUCE_CAST_AND_ACCUMULATE(
                T, Trait, U, vreg0CastB32, vreg1CastB32, dimR, srcAddr, vlSize, dtypeSize, dimA, Binaryfunc, uint32_t,
                uint16_t, CastTraitUppper, CastTraitLower, dstAddr);
        } else if constexpr (SupportBytes<T, 1>()) {
            ASC_3510_REDUCE_CAST_AND_ACCUMULATE(
                T, Trait, U, vreg0CastB16, vreg1CastB16, dimR, srcAddr, vlSize, dtypeSize, dimA, Binaryfunc, uint16_t,
                uint8_t, CastTraitUppper, CastTraitLower, dstAddr);
        }
    }
}

template <class T, const Reg::RegTrait& Trait, auto Binaryfunc, auto Reducefunc>
__simd_vf__ inline void ReduceARReuseSourceUnAlignedVF(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t dimA, uint32_t dimR)
{
    Reg::RegTensor<T, Trait> vreg0;
    Reg::RegTensor<T, Trait> vreg1;
    Reg::UnalignReg uSrc;
    Reg::UnalignReg uDst;
    uint32_t sreg1 = dimR;
    Reg::MaskReg mask = Reg::UpdateMask<T, Trait>(sreg1);
    Reg::LoadUnAlignPre(uSrc, ((__ubuf__ T*&)srcAddr));
    for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {
        Reg::LoadUnAlign(vreg0, uSrc, ((__ubuf__ T*&)srcAddr), dimR);
        Reducefunc(vreg1, vreg0, mask);
        StoreUnAlign((__ubuf__ T*&)dstAddr, vreg1, uDst, 1);
    }
    Reg::StoreUnAlignPost((__ubuf__ T*&)dstAddr, uDst, 0);
}

template <class T, const Reg::RegTrait& Trait, auto Binaryfunc, auto Reducefunc>
__aicore__ inline void ReduceARReuseSourceUnAligned(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t dimA, uint32_t dimR)
{
    constexpr uint16_t vlSize = SupportBytes<T, 8>() ? GetVecLen() / sizeof(float) : GetVecLen() / sizeof(T);
    if (dimR < vlSize) {
        if constexpr (IsSameType<T, bfloat16_t>::value) {
            CastReduceARUnAligned<
                T, float, Trait, ReduceOpInternal::CastTraitBF16F32, ReduceOpInternal::CastTraitF32BF16, vlSize,
                Binaryfunc, Reducefunc>(dstAddr, srcAddr, dimA, dimR);
        } else if constexpr (SupportBytes<T, 1>()) {
            CastReduceARUnAligned<
                T, half, Trait, ReduceOpInternal::CastTraitB8F16, ReduceOpInternal::CastTraitF16B8, vlSize, Binaryfunc,
                Reducefunc>(dstAddr, srcAddr, dimA, dimR);
        } else {
            ReduceARReuseSourceUnAlignedVF<T, Trait, Binaryfunc, Reducefunc>(dstAddr, srcAddr, dimA, dimR);
        }
    } else {
        if constexpr (SupportBytes<T, 8>()) {
            ReduceAROverVLReuseSourceUnAlignedB64<T, Trait, Binaryfunc, Reducefunc>(dstAddr, srcAddr, dimA, dimR);
        } else {
            ReduceAROverVLReuseSourceUnAligned<T, Trait, Binaryfunc, Reducefunc>(dstAddr, srcAddr, dimA, dimR);
        }
    }
}
} // namespace AscendC
#endif // IMPL_REDUCE_REDUCE_COMMON_AR_RA_REUSE_UNALIGN_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_COMMON_AR_RA_REUSE_UNALIGN_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_COMMON_AR_RA_REUSE_UNALIGN_C310_IMPL_H__
#endif

#undef ASC_3510_REDUCE_CAST_AND_ACCUMULATE
#undef ASC_3510_FOLD_TAIL_COLUMNS
#undef ASC_3510_AR_FOLD_SETUP_AND_MERGE_TAIL
#undef ASC_3510_REDUCE_DECLARE_HIGH_REGS
#undef ASC_3510_REDUCE_RA_FOLD_REMAINDER
#undef ASC_3510_REDUCE_FOLD_EIGHT_AND_STORE
#undef ASC_3510_RA_FOLD_SETUP_AND_MERGE_TAIL
