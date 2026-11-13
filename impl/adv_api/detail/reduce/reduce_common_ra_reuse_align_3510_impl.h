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
    "impl/adv_api/detail/reduce/reduce_common_ra_reuse_align_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/reduce/reduce.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_COMMON_RA_REUSE_ALIGN_C310_IMPL_H__
#endif

#ifndef IMPL_REDUCE_REDUCE_COMMON_RA_REUSE_ALIGN_C310_IMPL_H
#define IMPL_REDUCE_REDUCE_COMMON_RA_REUSE_ALIGN_C310_IMPL_H

#include "../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../include/basic_api/kernel_tensor.h"
#include "reduce_common_util_impl.h"
#include "reduce_common_util_3510_impl.h"

// Expand shared register stages in place to preserve the compiler input and instruction order.
#define ASC_3510_REDUCE_HANDLE_REUSE_AND_TAIL(                                                                   \
    mainR, dimA, T, isReuseSource, Trait, loopANum, copyA, tailR, srcAddr, vlSize, tmpAddr, vregMain, vregTail)  \
    uint32_t aTailOffset = mainR * dimA;                                                                         \
    __ubuf__ T* addr;                                                                                            \
    if constexpr (!isReuseSource) {                                                                              \
        Reg::RegTensor<T, Trait> vregTmp;                                                                        \
        Reg::MaskReg mask;                                                                                       \
        for (uint16_t loopA = 0; loopA < loopANum; loopA++) {                                                    \
            mask = Reg::UpdateMask<T, Trait>(copyA);                                                             \
            /* 0 to tailR will be merge later, no need to move */                                                \
            for (uint16_t loopR = static_cast<uint16_t>(tailR); loopR < static_cast<uint16_t>(mainR); loopR++) { \
                Reg::LoadAlign(vregTmp, srcAddr + loopA * vlSize + loopR * dimA);                                \
                Reg::StoreAlign(tmpAddr + loopA * vlSize + loopR * dimA, vregTmp, mask);                         \
            }                                                                                                    \
        }                                                                                                        \
        addr = tmpAddr;                                                                                          \
    } else {                                                                                                     \
        addr = srcAddr;                                                                                          \
    }                                                                                                            \
    Reg::RegTensor<T, Trait> vregMain;                                                                           \
    Reg::RegTensor<T, Trait> vregTail;                                                                           \
    Reg::MaskReg mask

#define ASC_3510_REDUCE_DECLARE_SIXTEEN_REGS(T, Trait) \
    Reg::RegTensor<T, Trait> vreg0;                    \
    Reg::RegTensor<T, Trait> vreg1;                    \
    Reg::RegTensor<T, Trait> vreg2;                    \
    Reg::RegTensor<T, Trait> vreg3;                    \
    Reg::RegTensor<T, Trait> vreg4;                    \
    Reg::RegTensor<T, Trait> vreg5;                    \
    Reg::RegTensor<T, Trait> vreg6;                    \
    Reg::RegTensor<T, Trait> vreg7;                    \
    Reg::RegTensor<T, Trait> vreg8;                    \
    Reg::RegTensor<T, Trait> vreg9;                    \
    Reg::RegTensor<T, Trait> vreg10;                   \
    Reg::RegTensor<T, Trait> vreg11;                   \
    Reg::RegTensor<T, Trait> vreg12;                   \
    Reg::RegTensor<T, Trait> vreg13;                   \
    Reg::RegTensor<T, Trait> vreg14;                   \
    Reg::RegTensor<T, Trait> vreg15

#define ASC_3510_REDUCE_FOLD_SIXTEEN(                                                                                \
    Binaryfunc, vreg0, vreg8, mask, vreg1, vreg9, vreg2, vreg10, vreg3, vreg11, vreg4, vreg12, vreg5, vreg13, vreg6, \
    vreg14, vreg7, vreg15)                                                                                           \
    Binaryfunc(vreg0, vreg0, vreg8, mask);                                                                           \
    Binaryfunc(vreg1, vreg1, vreg9, mask);                                                                           \
    Binaryfunc(vreg2, vreg2, vreg10, mask);                                                                          \
    Binaryfunc(vreg3, vreg3, vreg11, mask);                                                                          \
    Binaryfunc(vreg4, vreg4, vreg12, mask);                                                                          \
    Binaryfunc(vreg5, vreg5, vreg13, mask);                                                                          \
    Binaryfunc(vreg6, vreg6, vreg14, mask);                                                                          \
    Binaryfunc(vreg7, vreg7, vreg15, mask);                                                                          \
    /* L2 */                                                                                                         \
    Binaryfunc(vreg0, vreg0, vreg4, mask);                                                                           \
    Binaryfunc(vreg1, vreg1, vreg5, mask);                                                                           \
    Binaryfunc(vreg2, vreg2, vreg6, mask);                                                                           \
    Binaryfunc(vreg3, vreg3, vreg7, mask);                                                                           \
    /* L3 */                                                                                                         \
    Binaryfunc(vreg0, vreg0, vreg2, mask);                                                                           \
    Binaryfunc(vreg1, vreg1, vreg3, mask);                                                                           \
    /* L4 */                                                                                                         \
    Binaryfunc(vreg0, vreg0, vreg1, mask)

#define ASC_3510_REDUCE_COPY_OR_REUSE_INPUT(                                                                \
    isReuseSource, T, Trait, copyNum, vlSize, srcAddr, tailNum, tmpAddr, addr, needInplaceAdd, aTailOffset, \
    Binaryfunc)                                                                                             \
    Reg::MaskReg counterMask;                                                                               \
    if constexpr (!isReuseSource) {                                                                         \
        Reg::RegTensor<T, Trait> vregTmp;                                                                   \
        uint16_t mainRepeat = CeilDivision(copyNum, vlSize);                                                \
        /* 0 to tailR will be merge later, no need to move */                                               \
        for (uint16_t loopMain = 0; loopMain < mainRepeat; loopMain++) {                                    \
            counterMask = Reg::UpdateMask<T, Trait>(copyNum);                                               \
            Reg::LoadAlign(vregTmp, srcAddr + tailNum + loopMain * vlSize);                                 \
            Reg::StoreAlign(tmpAddr + tailNum + loopMain * vlSize, vregTmp, counterMask);                   \
        }                                                                                                   \
        addr = tmpAddr;                                                                                     \
    } else {                                                                                                \
        addr = srcAddr;                                                                                     \
    }                                                                                                       \
    Reg::RegTensor<T, Trait> vregMain;                                                                      \
    Reg::RegTensor<T, Trait> vregTail;                                                                      \
    /* Process mainR and tailR */                                                                           \
    for (uint16_t i = 0; i < needInplaceAdd; i++) {                                                         \
        uint16_t tailRepeat = CeilDivision(tailNum, vlSize);                                                \
        for (uint16_t loopTail = 0; loopTail < tailRepeat; loopTail++) {                                    \
            counterMask = Reg::UpdateMask<T, Trait>(tailNum);                                               \
            Reg::LoadAlign(vregMain, srcAddr + loopTail * vlSize);                                          \
            Reg::LoadAlign(vregTail, srcAddr + aTailOffset + loopTail * vlSize);                            \
            Binaryfunc(vregMain, vregMain, vregTail, counterMask);                                          \
            Reg::StoreAlign(addr + loopTail * vlSize, vregMain, counterMask);                               \
        }                                                                                                   \
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();                                \
    }

namespace AscendC {
template <class T, const Reg::RegTrait& Trait, auto Binaryfunc, bool isReuseSource>
__simd_vf__ inline void ReduceRAOverVLVFImpl(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ T* tmpAddr, uint16_t dimA, uint32_t dimR, uint32_t mainR,
    uint32_t tailR, uint16_t loopANum, uint16_t loopANumFinal, uint16_t folds, uint16_t avgFolds, uint16_t foldZero,
    uint16_t foldOne, uint16_t foldTwo, uint16_t foldThree)
{
    constexpr uint16_t vlSize = GetVecLen() / sizeof(T);
    uint16_t needInplaceAdd = tailR > 0 ? 1 : 0;
    uint16_t noNeedCopy = mainR > 1 ? 0 : 1;
    uint16_t mainTimes = folds / avgFolds;
    // Process vlSize axisA each time
    uint32_t inplaceA = dimA;
    uint32_t processA = dimA;
    uint32_t tailA = dimA;
    uint32_t copyA = dimA;
    uint32_t dtypeSize = sizeof(T);
    ASC_3510_REDUCE_HANDLE_REUSE_AND_TAIL(
        mainR, dimA, T, isReuseSource, Trait, loopANum, copyA, tailR, srcAddr, vlSize, tmpAddr, vregMain, vregTail);
    for (uint16_t i = 0; i < noNeedCopy; i++) {
        for (uint16_t loopA = 0; loopA < loopANum; loopA++) {
            mask = Reg::UpdateMask<T, Trait>(inplaceA);
            Reg::LoadAlign(vregMain, srcAddr + loopA * vlSize);
            Reg::LoadAlign(vregTail, srcAddr + loopA * vlSize + aTailOffset);
            Binaryfunc(vregMain, vregMain, vregTail, mask);
            Reg::StoreAlign(dstAddr + loopA * vlSize, vregMain, mask);
        }
        return;
    }
    // Process mainR and tailR
    for (uint16_t i = 0; i < needInplaceAdd; i++) {
        for (uint16_t loopA = 0; loopA < loopANum; loopA++) {
            mask = Reg::UpdateMask<T, Trait>(inplaceA);
            for (uint16_t loopR = 0; loopR < static_cast<uint16_t>(tailR); loopR++) {
                Reg::LoadAlign(vregMain, srcAddr + loopA * vlSize + loopR * dimA);
                Reg::LoadAlign(vregTail, srcAddr + loopA * vlSize + aTailOffset + loopR * dimA);
                Binaryfunc(vregMain, vregMain, vregTail, mask);
                Reg::StoreAlign(addr + loopA * vlSize + loopR * dimA, vregMain, mask);
            }
        }
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
    }

    // MainFolds need 16 register
    ASC_3510_REDUCE_DECLARE_SIXTEEN_REGS(T, Trait);

    // Process main folds
    uint16_t loopRNum = mainR;
    for (uint16_t loopMain = 0; loopMain < mainTimes; loopMain++) {
        loopRNum = loopRNum >> avgFolds;
        uint16_t offsetR = loopRNum * dimA;
        uint32_t mainA = dimA;
        for (uint16_t loopA = 0; loopA < loopANum; loopA++) {
            mask = Reg::UpdateMask<T, Trait>(mainA);
            for (uint16_t loopR = 0; loopR < loopRNum; loopR++) {
                // L0
                Reg::LoadAlign(vreg0, addr + loopA * vlSize + loopR * dimA);
                Reg::LoadAlign(vreg1, addr + offsetR + loopA * vlSize + loopR * dimA);
                Reg::LoadAlign(vreg2, addr + 2 * offsetR + loopA * vlSize + loopR * dimA);
                Reg::LoadAlign(vreg3, addr + 3 * offsetR + loopA * vlSize + loopR * dimA);
                Reg::LoadAlign(vreg4, addr + 4 * offsetR + loopA * vlSize + loopR * dimA);
                Reg::LoadAlign(vreg5, addr + 5 * offsetR + loopA * vlSize + loopR * dimA);
                Reg::LoadAlign(vreg6, addr + 6 * offsetR + loopA * vlSize + loopR * dimA);
                Reg::LoadAlign(vreg7, addr + 7 * offsetR + loopA * vlSize + loopR * dimA);
                Reg::LoadAlign(vreg8, addr + 8 * offsetR + loopA * vlSize + loopR * dimA);
                Reg::LoadAlign(vreg9, addr + 9 * offsetR + loopA * vlSize + loopR * dimA);
                Reg::LoadAlign(vreg10, addr + 10 * offsetR + loopA * vlSize + loopR * dimA);
                Reg::LoadAlign(vreg11, addr + 11 * offsetR + loopA * vlSize + loopR * dimA);
                Reg::LoadAlign(vreg12, addr + 12 * offsetR + loopA * vlSize + loopR * dimA);
                Reg::LoadAlign(vreg13, addr + 13 * offsetR + loopA * vlSize + loopR * dimA);
                Reg::LoadAlign(vreg14, addr + 14 * offsetR + loopA * vlSize + loopR * dimA);
                Reg::LoadAlign(vreg15, addr + 15 * offsetR + loopA * vlSize + loopR * dimA);
                // L1
                ASC_3510_REDUCE_FOLD_SIXTEEN(
                    Binaryfunc, vreg0, vreg8, mask, vreg1, vreg9, vreg2, vreg10, vreg3, vreg11, vreg4, vreg12, vreg5,
                    vreg13, vreg6, vreg14, vreg7, vreg15);
                Reg::StoreAlign(addr + loopA * vlSize + loopR * dimA, vreg0, mask);
            }
        }
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
    }

    // Process tail folds
    for (uint16_t i = 0; i < foldOne; i++) {
        for (uint16_t loopA = 0; loopA < loopANum; loopA++) {
            mask = Reg::UpdateMask<T, Trait>(tailA);
            // L0
            Reg::LoadAlign(vreg0, addr + loopA * vlSize);
            Reg::LoadAlign(vreg1, addr + dimA + loopA * vlSize);
            // L1
            Binaryfunc(vreg0, vreg0, vreg1, mask);
            Reg::StoreAlign(dstAddr + loopA * vlSize, vreg0, mask);
        }
    }

    for (uint16_t i = 0; i < foldTwo; i++) {
        for (uint16_t loopA = 0; loopA < loopANum; loopA++) {
            mask = Reg::UpdateMask<T, Trait>(tailA);
            // L0
            Reg::LoadAlign(vreg0, addr + loopA * vlSize);
            Reg::LoadAlign(vreg1, addr + dimA + loopA * vlSize);
            Reg::LoadAlign(vreg2, addr + 2 * dimA + loopA * vlSize);
            Reg::LoadAlign(vreg3, addr + 3 * dimA + loopA * vlSize);
            // L1
            Binaryfunc(vreg0, vreg0, vreg2, mask);
            Binaryfunc(vreg1, vreg1, vreg3, mask);
            // L2
            Binaryfunc(vreg0, vreg0, vreg1, mask);
            Reg::StoreAlign(dstAddr + loopA * vlSize, vreg0, mask);
        }
    }

    for (uint16_t i = 0; i < foldThree; i++) {
        for (uint16_t loopA = 0; loopA < loopANum; loopA++) {
            mask = Reg::UpdateMask<T, Trait>(tailA);
            // L0
            Reg::LoadAlign(vreg0, addr + loopA * vlSize);
            Reg::LoadAlign(vreg1, addr + dimA + loopA * vlSize);
            Reg::LoadAlign(vreg2, addr + 2 * dimA + loopA * vlSize);
            Reg::LoadAlign(vreg3, addr + 3 * dimA + loopA * vlSize);
            Reg::LoadAlign(vreg4, addr + 4 * dimA + loopA * vlSize);
            Reg::LoadAlign(vreg5, addr + 5 * dimA + loopA * vlSize);
            Reg::LoadAlign(vreg6, addr + 6 * dimA + loopA * vlSize);
            Reg::LoadAlign(vreg7, addr + 7 * dimA + loopA * vlSize);
            // L1
            Binaryfunc(vreg0, vreg0, vreg4, mask);
            Binaryfunc(vreg1, vreg1, vreg5, mask);
            Binaryfunc(vreg2, vreg2, vreg6, mask);
            Binaryfunc(vreg3, vreg3, vreg7, mask);
            // L2
            Binaryfunc(vreg0, vreg0, vreg2, mask);
            Binaryfunc(vreg1, vreg1, vreg3, mask);
            // L3
            Binaryfunc(vreg0, vreg0, vreg1, mask);
            Reg::StoreAlign(dstAddr + loopA * vlSize, vreg0, mask);
        }
    }

    // Reduce to 1
    for (uint16_t i = 0; i < foldZero; i++) {
        for (uint16_t loopA = 0; loopA < loopANumFinal; loopA++) {
            mask = Reg::UpdateMask<T, Trait>(processA);
            Reg::LoadAlign(vreg0, addr + loopA * vlSize);
            Reg::StoreAlign(dstAddr + loopA * vlSize, vreg0, mask);
        }
    }
}

template <class T, const Reg::RegTrait& Trait, auto Binaryfunc, bool isReuseSource>
__aicore__ inline void ReduceRAOverVLImpl(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ T* tmpAddr, uint16_t dimA, uint32_t dimR)
{
    constexpr uint16_t vlSize = GetVecLen() / sizeof(T);
    uint32_t mainR = ReduceOpInternal::CalculateMainR(dimR, false, vlSize);
    uint32_t tailR = dimR - mainR;

    uint16_t loopANum = CeilDivision(dimA, vlSize);
    // move by fold zero only if R axis is 1
    uint16_t loopANumFinal = loopANum;
    if (mainR == 1) {
        ReduceOpInternal::ReduceCopyOutImpl<T>(dstAddr, srcAddr, dimA);
        return;
    }

    if constexpr (!isReuseSource) {
        if (tailR == 0 && mainR > 1) {
            mainR = mainR / 2;
            tailR = mainR;
        }
    }

    uint16_t folds = ReduceOpInternal::CalculateFolds(mainR);
    uint16_t avgFolds = ReduceOpInternal::BASE_FOLD;
    uint16_t tailFolds = folds % avgFolds;
    uint16_t foldZero = (tailFolds == 0) ? 1 : 0;
    uint16_t foldOne = (tailFolds == ReduceOpInternal::FOLD_ONE) ? 1 : 0;
    uint16_t foldTwo = (tailFolds == ReduceOpInternal::FOLD_TWO) ? 1 : 0;
    uint16_t foldThree = (tailFolds == ReduceOpInternal::FOLD_THREE) ? 1 : 0;

    ReduceRAOverVLVFImpl<T, Trait, Binaryfunc, isReuseSource>(
        dstAddr, srcAddr, tmpAddr, dimA, dimR, mainR, tailR, loopANum, loopANumFinal, folds, avgFolds, foldZero,
        foldOne, foldTwo, foldThree);
}

template <class T, const Reg::RegTrait& Trait, auto Binaryfunc, bool isReuseSource>
__simd_vf__ inline void ReduceRALessThanVLDimR1VFImpl(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ T* tmpAddr, uint32_t dimA, uint32_t dimR)
{
    Reg::RegTensor<T, Trait> vregTmp;
    Reg::MaskReg mask = Reg::UpdateMask<T, Trait>(dimA);
    Reg::LoadAlign(vregTmp, srcAddr);
    Reg::StoreAlign(dstAddr, vregTmp, mask);
}

template <class T, const Reg::RegTrait& Trait, auto Binaryfunc, bool isReuseSource>
__simd_vf__ inline void ReduceRALessThanVLVFImpl(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ T* tmpAddr, uint32_t dimA, uint32_t dimR, uint32_t mainR,
    uint32_t tailR, uint16_t folds, uint16_t avgFolds, uint16_t foldZero, uint16_t foldOne, uint16_t foldTwo,
    uint16_t foldThree)
{
    constexpr uint16_t vlSize = GetVecLen() / sizeof(T);
    uint16_t needInplaceAdd = tailR > 0 ? 1 : 0;
    uint16_t mainTimes = folds / avgFolds;
    // Process vlSize axisA each time
    uint32_t processA = dimA;
    uint32_t dtypeSize = sizeof(T);
    uint32_t aTailOffset = mainR * dimA;
    uint32_t copyNum = (mainR - tailR) * dimA;
    uint32_t tailNum = tailR * dimA;
    uint16_t loopRNum = mainR;

    __ubuf__ T* addr;
    Reg::MaskReg mask;
    mask = Reg::UpdateMask<T, Trait>(processA);
    ASC_3510_REDUCE_COPY_OR_REUSE_INPUT(
        isReuseSource, T, Trait, copyNum, vlSize, srcAddr, tailNum, tmpAddr, addr, needInplaceAdd, aTailOffset,
        Binaryfunc)

    // MainFolds need 16 register
    ASC_3510_REDUCE_DECLARE_SIXTEEN_REGS(T, Trait);

    // Process main folds
    for (uint16_t loopMain = 0; loopMain < mainTimes; loopMain++) {
        loopRNum = loopRNum >> avgFolds;
        auto tmpSrcAddr = addr;
        for (uint16_t loopR = 0; loopR < loopRNum; loopR++) {
            // L0
            Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg0, tmpSrcAddr, dimA);
            Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg1, tmpSrcAddr, dimA);
            Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg2, tmpSrcAddr, dimA);
            Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg3, tmpSrcAddr, dimA);
            Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg4, tmpSrcAddr, dimA);
            Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg5, tmpSrcAddr, dimA);
            Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg6, tmpSrcAddr, dimA);
            Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg7, tmpSrcAddr, dimA);
            Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg8, tmpSrcAddr, dimA);
            Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg9, tmpSrcAddr, dimA);
            Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg10, tmpSrcAddr, dimA);
            Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg11, tmpSrcAddr, dimA);
            Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg12, tmpSrcAddr, dimA);
            Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg13, tmpSrcAddr, dimA);
            Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg14, tmpSrcAddr, dimA);
            Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg15, tmpSrcAddr, dimA);
            // L1
            ASC_3510_REDUCE_FOLD_SIXTEEN(
                Binaryfunc, vreg0, vreg8, mask, vreg1, vreg9, vreg2, vreg10, vreg3, vreg11, vreg4, vreg12, vreg5,
                vreg13, vreg6, vreg14, vreg7, vreg15);
            Reg::StoreAlign(addr + loopR * dimA, vreg0, mask);
        }
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
    }

    // Process tail folds
    for (uint16_t i = 0; i < foldOne; i++) {
        // L0
        Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg0, addr, dimA);
        Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg1, addr, dimA);
        // L1
        Binaryfunc(vreg0, vreg0, vreg1, mask);
        Reg::StoreAlign(dstAddr, vreg0, mask);
    }

    for (uint16_t i = 0; i < foldTwo; i++) {
        // L0
        Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg0, addr, dimA);
        Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg1, addr, dimA);
        Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg2, addr, dimA);
        Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg3, addr, dimA);
        // L1
        Binaryfunc(vreg0, vreg0, vreg2, mask);
        Binaryfunc(vreg1, vreg1, vreg3, mask);
        // L2
        Binaryfunc(vreg0, vreg0, vreg1, mask);
        Reg::StoreAlign(dstAddr, vreg0, mask);
    }

    for (uint16_t i = 0; i < foldThree; i++) {
        // L0
        Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg0, addr, dimA);
        Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg1, addr, dimA);
        Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg2, addr, dimA);
        Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg3, addr, dimA);
        Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg4, addr, dimA);
        Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg5, addr, dimA);
        Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg6, addr, dimA);
        Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(vreg7, addr, dimA);
        // L1
        Binaryfunc(vreg0, vreg0, vreg4, mask);
        Binaryfunc(vreg1, vreg1, vreg5, mask);
        Binaryfunc(vreg2, vreg2, vreg6, mask);
        Binaryfunc(vreg3, vreg3, vreg7, mask);
        // L2
        Binaryfunc(vreg0, vreg0, vreg2, mask);
        Binaryfunc(vreg1, vreg1, vreg3, mask);
        // L3
        Binaryfunc(vreg0, vreg0, vreg1, mask);
        Reg::StoreAlign(dstAddr, vreg0, mask);
    }

    // Reduce to 1
    for (uint16_t i = 0; i < foldZero; i++) {
        Reg::LoadAlign(vreg0, addr);
        Reg::StoreAlign(dstAddr, vreg0, mask);
    }
}

template <class T, const Reg::RegTrait& Trait, auto Binaryfunc, bool isReuseSource>
__aicore__ inline void ReduceRALessThanVLImpl(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ T* tmpAddr, uint32_t dimA, uint32_t dimR)
{
    constexpr uint16_t vlSize = GetVecLen() / sizeof(T);
    uint32_t mainR = ReduceOpInternal::CalculateMainR(dimR, false, vlSize);
    uint32_t tailR = dimR - mainR;
    if constexpr (!isReuseSource) {
        if (tailR == 0) {
            mainR = mainR / 2;
            tailR = mainR;
        }
    }
    if (dimR == 1) {
        ReduceRALessThanVLDimR1VFImpl<T, Trait, Binaryfunc, isReuseSource>(dstAddr, srcAddr, tmpAddr, dimA, dimR);
        return;
    }

    uint16_t folds = ReduceOpInternal::CalculateFolds(mainR);
    uint16_t avgFolds = ReduceOpInternal::BASE_FOLD;
    uint16_t tailFolds = folds % avgFolds;
    uint16_t foldZero = (tailFolds == 0) ? 1 : 0;
    uint16_t foldOne = (tailFolds == ReduceOpInternal::FOLD_ONE) ? 1 : 0;
    uint16_t foldTwo = (tailFolds == ReduceOpInternal::FOLD_TWO) ? 1 : 0;
    uint16_t foldThree = (tailFolds == ReduceOpInternal::FOLD_THREE) ? 1 : 0;

    ReduceRALessThanVLVFImpl<T, Trait, Binaryfunc, isReuseSource>(
        dstAddr, srcAddr, tmpAddr, dimA, dimR, mainR, tailR, folds, avgFolds, foldZero, foldOne, foldTwo, foldThree);
}

template <class T, const Reg::RegTrait& Trait, auto Binaryfunc, bool isReuseSource>
__simd_vf__ inline void ReduceRAConcatDimR1VFImpl(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ T* tmpAddr, uint32_t dimA, uint32_t dimR)
{
    Reg::RegTensor<T, Trait> vregTmp;
    Reg::MaskReg mask = Reg::UpdateMask<T, Trait>(dimA);
    Reg::LoadAlign(vregTmp, srcAddr);
    Reg::StoreAlign(dstAddr, vregTmp, mask);
}

template <class T, const Reg::RegTrait& Trait, auto Binaryfunc, bool isReuseSource>
__simd_vf__ inline void ReduceRAConcatDimR2VFImpl(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ T* tmpAddr, uint32_t dimA, uint32_t dimR)
{
    Reg::RegTensor<T, Trait> vregMain;
    Reg::RegTensor<T, Trait> vregTail;
    uint32_t maskScalar = dimA;
    Reg::MaskReg counterMask = Reg::UpdateMask<T, Trait>(maskScalar);
    Reg::LoadAlign(vregMain, srcAddr);
    Reg::LoadAlign(vregTail, srcAddr + dimA);
    Binaryfunc(vregMain, vregMain, vregTail, counterMask);
    Reg::StoreAlign(dstAddr, vregMain, counterMask);
}

template <class T, const Reg::RegTrait& Trait, auto Binaryfunc, bool isReuseSource>
__simd_vf__ inline void ReduceRAConcatVFImpl(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ T* tmpAddr, uint32_t dimA, uint32_t dimR, uint16_t foldTime,
    uint32_t mainR, uint32_t tailR)
{
    // Process RegTraitNumTwo
    constexpr uint16_t vlSize = SupportBytes<T, 8>() ? (GetVecLen() / sizeof(float)) : (GetVecLen() / sizeof(T));
    uint16_t needInplaceAdd = tailR > 0 ? 1 : 0;
    // Process vlSize axisA each time
    uint32_t processA = dimA;
    uint32_t dtypeSize = sizeof(T);
    uint32_t aTailOffset = mainR * dimA;
    // do mainR-tailR copy, do tailR binary op
    uint32_t copyNum = (mainR - tailR) * dimA;
    uint32_t tailNum = tailR * dimA;
    uint16_t loopDataNum = mainR * dimA;

    __ubuf__ T* addr;
    Reg::MaskReg fullMask;
    fullMask = Reg::CreateMask<T, Reg::MaskPattern::ALL, Trait>();
    ASC_3510_REDUCE_COPY_OR_REUSE_INPUT(
        isReuseSource, T, Trait, copyNum, vlSize, srcAddr, tailNum, tmpAddr, addr, needInplaceAdd, aTailOffset,
        Binaryfunc)

    for (uint16_t fold = 0; fold < foldTime; fold++) {
        mainR = mainR >> 1;
        uint32_t foldDataNum = mainR * dimA;
        uint16_t foldRepeat = CeilDivision(foldDataNum, vlSize);
        for (uint16_t i = 0; i < foldRepeat; i++) {
            Reg::LoadAlign(vregMain, addr + i * vlSize);
            Reg::LoadAlign(vregTail, addr + foldDataNum + i * vlSize);
            Binaryfunc(vregMain, vregMain, vregTail, fullMask);
            Reg::StoreAlign(addr + i * vlSize, vregMain, fullMask);
        }
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
    }

    // final fold is less than vl, no repeat
    uint32_t maskScalar = dimA;
    counterMask = Reg::UpdateMask<T, Trait>(maskScalar);
    Reg::LoadAlign(vregMain, addr);
    Reg::LoadAlign(vregTail, addr + dimA);
    Binaryfunc(vregMain, vregMain, vregTail, counterMask);
    Reg::StoreAlign(dstAddr, vregMain, counterMask);
}

template <class T, const Reg::RegTrait& Trait, auto Binaryfunc, bool isReuseSource>
__aicore__ inline void ReduceRAConcatImpl(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ T* tmpAddr, uint32_t dimA, uint32_t dimR)
{
    constexpr uint16_t vlSize = GetVecLen() / sizeof(T);
    uint16_t foldTime = Internal::FindClosestPowerOfTwo(dimR);
    uint32_t mainR = 1 << foldTime;
    // last fold not in main loop, main R == 1 will not enter main loop
    foldTime = foldTime - 1;
    uint32_t tailR = dimR - mainR;

    if constexpr (!isReuseSource) {
        if (tailR == 0) {
            mainR = mainR / 2;
            tailR = mainR;
            foldTime = foldTime - 1;
        }
    }
    if (dimR == 1) {
        ReduceRAConcatDimR1VFImpl<T, Trait, Binaryfunc, isReuseSource>(dstAddr, srcAddr, tmpAddr, dimA, dimR);
        return;
    } else if (dimR == 2) {
        ReduceRAConcatDimR2VFImpl<T, Trait, Binaryfunc, isReuseSource>(dstAddr, srcAddr, tmpAddr, dimA, dimR);
        return;
    }

    ReduceRAConcatVFImpl<T, Trait, Binaryfunc, isReuseSource>(
        dstAddr, srcAddr, tmpAddr, dimA, dimR, foldTime, mainR, tailR);
}

template <class T, const Reg::RegTrait& Trait, auto Binaryfunc, bool isReuseSource>
__aicore__ inline void ReduceRAImpl(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ T* tmpAddr, uint32_t dimA, uint32_t dimR)
{
    constexpr uint16_t vlSize = GetVecLen() / sizeof(T);
    if (dimA <= vlSize / ReduceOpInternal::REGULAR_FOLD_NUM || dimA > ReduceOpInternal::U16_STRIDE) {
        ReduceRAConcatImpl<T, Trait, Binaryfunc, isReuseSource>(dstAddr, srcAddr, tmpAddr, dimA, dimR);
    } else if (dimA <= vlSize) {
        ReduceRALessThanVLImpl<T, Trait, Binaryfunc, isReuseSource>(dstAddr, srcAddr, tmpAddr, dimA, dimR);
    } else {
        ReduceRAOverVLImpl<T, Trait, Binaryfunc, isReuseSource>(
            dstAddr, srcAddr, tmpAddr, static_cast<uint16_t>(dimA), dimR);
    }
}

template <class T, const Reg::RegTrait& Trait, auto Binaryfunc, bool isReuseSource>
__simd_vf__ inline void ReduceRAB64ReuseSourceVF(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ T* tmpAddr, uint32_t dimA, uint32_t dimR, uint32_t mainR,
    uint32_t tailR, uint16_t loopANum, uint16_t loopANumFinal, uint16_t folds, uint16_t avgFolds, uint16_t foldZero,
    uint16_t foldOne, uint16_t foldTwo)
{
    constexpr uint16_t vlSize = GetVecLen() / sizeof(float);
    uint16_t needInplaceAdd = tailR > 0 ? 1 : 0;
    uint16_t mainTimes = folds / avgFolds;
    // Process vlSize axisA each time
    uint32_t inplaceA = dimA;
    uint32_t processA = dimA;
    uint32_t tailA = dimA;
    uint32_t copyA = dimA;
    ASC_3510_REDUCE_HANDLE_REUSE_AND_TAIL(
        mainR, dimA, T, isReuseSource, Trait, loopANum, copyA, tailR, srcAddr, vlSize, tmpAddr, b64VregMain,
        b64VregTail);
    // Add mainR and tailR
    for (uint16_t i = 0; i < needInplaceAdd; i++) {
        for (uint16_t loopA = 0; loopA < loopANum; loopA++) {
            mask = Reg::UpdateMask<T, Trait>(inplaceA);
            for (uint16_t loopR = 0; loopR < static_cast<uint16_t>(tailR); loopR++) {
                Reg::LoadAlign(b64VregMain, srcAddr + loopA * vlSize + loopR * dimA);
                Reg::LoadAlign(b64VregTail, srcAddr + loopA * vlSize + aTailOffset + loopR * dimA);
                Binaryfunc(b64VregMain, b64VregMain, b64VregTail, mask);
                Reg::StoreAlign(addr + loopA * vlSize + loopR * dimA, b64VregMain, mask);
            }
        }
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

    // Process main folds
    uint16_t loopRNum = mainR;
    for (uint16_t loopMain = 0; loopMain < mainTimes; loopMain++) {
        loopRNum = loopRNum >> avgFolds;
        uint16_t offsetR = loopRNum * dimA;
        uint32_t mainA = dimA;
        for (uint16_t loopA = 0; loopA < loopANum; loopA++) {
            mask = Reg::UpdateMask<T, Trait>(mainA);
            for (uint16_t loopR = 0; loopR < loopRNum; loopR++) {
                // L0
                Reg::LoadAlign(b64Vreg0, addr + loopA * vlSize + loopR * dimA);
                Reg::LoadAlign(b64Vreg1, addr + offsetR + loopA * vlSize + loopR * dimA);
                Reg::LoadAlign(b64Vreg2, addr + 2 * offsetR + loopA * vlSize + loopR * dimA);
                Reg::LoadAlign(b64Vreg3, addr + 3 * offsetR + loopA * vlSize + loopR * dimA);
                Reg::LoadAlign(b64Vreg4, addr + 4 * offsetR + loopA * vlSize + loopR * dimA);
                Reg::LoadAlign(b64Vreg5, addr + 5 * offsetR + loopA * vlSize + loopR * dimA);
                Reg::LoadAlign(b64Vreg6, addr + 6 * offsetR + loopA * vlSize + loopR * dimA);
                Reg::LoadAlign(b64Vreg7, addr + 7 * offsetR + loopA * vlSize + loopR * dimA);
                // L1
                Binaryfunc(b64Vreg0, b64Vreg0, b64Vreg4, mask);
                Binaryfunc(b64Vreg1, b64Vreg1, b64Vreg5, mask);
                Binaryfunc(b64Vreg2, b64Vreg2, b64Vreg6, mask);
                Binaryfunc(b64Vreg3, b64Vreg3, b64Vreg7, mask);
                // L2
                Binaryfunc(b64Vreg0, b64Vreg0, b64Vreg2, mask);
                Binaryfunc(b64Vreg1, b64Vreg1, b64Vreg3, mask);
                // L3
                Binaryfunc(b64Vreg0, b64Vreg0, b64Vreg1, mask);
                Reg::StoreAlign(addr + loopA * vlSize + loopR * dimA, b64Vreg0, mask);
            }
        }
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
    }

    // Process tail folds
    for (uint16_t i = 0; i < foldOne; i++) {
        for (uint16_t loopA = 0; loopA < loopANum; loopA++) {
            mask = Reg::UpdateMask<T, Trait>(tailA);
            // L0
            Reg::LoadAlign(b64Vreg0, addr + loopA * vlSize);
            Reg::LoadAlign(b64Vreg1, addr + dimA + loopA * vlSize);
            // L1
            Binaryfunc(b64Vreg0, b64Vreg0, b64Vreg1, mask);
            Reg::StoreAlign(dstAddr + loopA * vlSize, b64Vreg0, mask);
        }
    }

    for (uint16_t i = 0; i < foldTwo; i++) {
        for (uint16_t loopA = 0; loopA < loopANum; loopA++) {
            mask = Reg::UpdateMask<T, Trait>(tailA);
            // L0
            Reg::LoadAlign(b64Vreg0, addr + loopA * vlSize);
            Reg::LoadAlign(b64Vreg1, addr + dimA + loopA * vlSize);
            Reg::LoadAlign(b64Vreg2, addr + 2 * dimA + loopA * vlSize);
            Reg::LoadAlign(b64Vreg3, addr + 3 * dimA + loopA * vlSize);
            // L1
            Binaryfunc(b64Vreg0, b64Vreg0, b64Vreg2, mask);
            Binaryfunc(b64Vreg1, b64Vreg1, b64Vreg3, mask);
            // L2
            Binaryfunc(b64Vreg0, b64Vreg0, b64Vreg1, mask);
            Reg::StoreAlign(dstAddr + loopA * vlSize, b64Vreg0, mask);
        }
    }

    // Reduce to 1
    for (uint16_t i = 0; i < foldZero; i++) {
        for (uint16_t loopA = 0; loopA < loopANumFinal; loopA++) {
            mask = Reg::UpdateMask<T, Trait>(processA);
            Reg::LoadAlign(b64Vreg0, addr + loopA * vlSize);
            Reg::StoreAlign(dstAddr + loopA * vlSize, b64Vreg0, mask);
        }
    }
}

template <class T, const Reg::RegTrait& Trait, auto Binaryfunc, bool isReuseSource>
__aicore__ inline void ReduceRAB64ReuseSource(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ T* tmpAddr, uint32_t dimA, uint32_t dimR)
{
    constexpr uint16_t vlSize = GetVecLen() / sizeof(float);
    uint32_t mainR = ReduceOpInternal::CalculateMainR(dimR, false, vlSize);
    uint32_t tailR = dimR - mainR;

    uint16_t loopANum = (dimA + vlSize - 1) / vlSize;
    // move by fold zero only if R axis is 1
    uint16_t loopANumFinal = loopANum;
    if (mainR == 1) {
        ReduceOpInternal::ReduceCopyOutImpl<T>(dstAddr, srcAddr, dimA);
        return;
    }

    if constexpr (!isReuseSource) {
        if (tailR == 0 && mainR > 1) {
            mainR = mainR / 2;
            tailR = mainR;
        }
    }
    uint16_t needInplaceAdd = tailR > 0 ? 1 : 0;

    uint16_t folds = ReduceOpInternal::CalculateFolds(mainR);
    uint16_t avgFolds = ReduceOpInternal::BASE_FOLD_B64;
    uint16_t tailFolds = folds % avgFolds;
    uint16_t foldZero = (tailFolds == 0) ? 1 : 0;
    uint16_t foldOne = (tailFolds == ReduceOpInternal::FOLD_ONE) ? 1 : 0;
    uint16_t foldTwo = (tailFolds == ReduceOpInternal::FOLD_TWO) ? 1 : 0;

    ReduceRAB64ReuseSourceVF<T, Trait, Binaryfunc, isReuseSource>(
        dstAddr, srcAddr, tmpAddr, dimA, dimR, mainR, tailR, loopANum, loopANumFinal, folds, avgFolds, foldZero,
        foldOne, foldTwo);
}
} // namespace AscendC
#endif // IMPL_REDUCE_REDUCE_COMMON_RA_REUSE_ALIGN_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_COMMON_RA_REUSE_ALIGN_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_COMMON_RA_REUSE_ALIGN_C310_IMPL_H__
#endif

#undef ASC_3510_REDUCE_COPY_OR_REUSE_INPUT
#undef ASC_3510_REDUCE_FOLD_SIXTEEN
#undef ASC_3510_REDUCE_DECLARE_SIXTEEN_REGS
#undef ASC_3510_REDUCE_HANDLE_REUSE_AND_TAIL
