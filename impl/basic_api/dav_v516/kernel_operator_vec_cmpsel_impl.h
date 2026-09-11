/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file kernel_operator_vec_cmpsel_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_v516/kernel_operator_vec_cmpsel_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_CMPSEL_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_CMPSEL_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_CMPSEL_IMPL_H

#include "../kernel_utils.h"
#include "../../../include/basic_api/kernel_tpipe.h"
#include "kernel_operator_vec_template_impl.h"
#include "../../../include/basic_api/reg_compute/kernel_reg_compute_intf.h"

namespace AscendC {
namespace CmpSelInternal {
constexpr uint32_t maskBitToByte = 8;
}
/* ***************************************************************************************
 * ************************************** Compare ****************************************
 * ************************************************************************************** */
// Compare written to CMPMASK
template <typename T, bool isSetMask, CMPMODE cmpMode>
__aicore__ inline void CompareWithoutDstCounterModeImplVF(
    __ubuf__ T* src0, __ubuf__ T* src1, __ubuf__ uint64_t* tempBuf, const uint64_t mask,
    const BinaryRepeatParams& repeatParams)
{
    Reg::RegTensor<T> srcReg0, srcReg1;
    Reg::MaskReg maskReg, dstReg;
    Reg::UnalignReg uReg;
    uint32_t sreg = static_cast<uint32_t>(mask);
    if constexpr (!isSetMask) {
        maskReg = Reg::MoveMask<uint16_t>();
        Reg::StoreAlign<uint64_t, Reg::MaskDist::DIST_PACK>(tempBuf, maskReg);
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::SCALAR_LOAD>();
        sreg = static_cast<uint32_t>(tempBuf[0]);
    }
    maskReg = Reg::UpdateMask<T>(sreg);
    Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(srcReg0, src0, (uint32_t)repeatParams.src0BlkStride, maskReg);
    Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(srcReg1, src1, (uint32_t)repeatParams.src1BlkStride, maskReg);
    Reg::Compare<T, cmpMode>(dstReg, srcReg0, srcReg1, maskReg);
    Reg::StoreUnAlign((__ubuf__ T*&)tempBuf, dstReg, uReg);
    Reg::StoreUnAlignPost<uint64_t, Reg::PostLiteral::POST_MODE_NORMAL>(tempBuf, uReg, 0);
}

template <typename T, bool isSetMask, bool isBitMap, CMPMODE cmpMode>
__aicore__ inline void CompareWithoutDstNormalModeImplVF(
    __ubuf__ T* src0, __ubuf__ T* src1, __ubuf__ uint64_t* tempBuf, const uint64_t mask,
    const BinaryRepeatParams& repeatParams)
{
    Reg::RegTensor<T> srcReg0, srcReg1;
    Reg::MaskReg maskReg, dstReg;
    Reg::UnalignReg uReg;
    uint32_t sreg = static_cast<uint32_t>(mask);
    if constexpr (isBitMap) {
        maskReg = Reg::MoveMask<T>();
    } else {
        if constexpr (isSetMask) {
            maskReg = Reg::UpdateMask<T>(sreg);
        } else {
            maskReg = Reg::MoveMask<T>();
        }
    }
    Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(srcReg0, src0, (uint32_t)repeatParams.src0BlkStride, maskReg);
    Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(srcReg1, src1, (uint32_t)repeatParams.src1BlkStride, maskReg);
    Reg::Compare<T, cmpMode>(dstReg, srcReg0, srcReg1, maskReg);
    Reg::StoreUnAlign((__ubuf__ T*&)tempBuf, dstReg, uReg);
    Reg::StoreUnAlignPost<uint64_t, Reg::PostLiteral::POST_MODE_NORMAL>(tempBuf, uReg, 0);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void VcmpImpl(
    __ubuf__ T* src0, __ubuf__ T* src1, CMPMODE cmpMode, const uint64_t mask[], const BinaryRepeatParams& repeatParams)
{
    static_assert(SupportType<T, half, float>(), "current data type is not supported!");
    bool isCounterMode = Internal::IsCounterMode();
    event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
    SetFlag<HardEvent::S_V>(eventIdSToV);
    WaitFlag<HardEvent::S_V>(eventIdSToV);
    __ubuf__ uint64_t* tempBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(GetRuntimeUBSize(), 2);
    if (isCounterMode) {
        switch (cmpMode) {
            case CMPMODE::LT: {
                VF_CALL<CompareWithoutDstCounterModeImplVF<T, isSetMask, CMPMODE::LT>>(
                    src0, src1, tempBuf, mask[0], repeatParams);
                break;
            }
            case CMPMODE::GT: {
                VF_CALL<CompareWithoutDstCounterModeImplVF<T, isSetMask, CMPMODE::GT>>(
                    src0, src1, tempBuf, mask[0], repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                VF_CALL<CompareWithoutDstCounterModeImplVF<T, isSetMask, CMPMODE::EQ>>(
                    src0, src1, tempBuf, mask[0], repeatParams);
                break;
            }
            case CMPMODE::LE: {
                VF_CALL<CompareWithoutDstCounterModeImplVF<T, isSetMask, CMPMODE::LE>>(
                    src0, src1, tempBuf, mask[0], repeatParams);
                break;
            }
            case CMPMODE::GE: {
                VF_CALL<CompareWithoutDstCounterModeImplVF<T, isSetMask, CMPMODE::GE>>(
                    src0, src1, tempBuf, mask[0], repeatParams);
                break;
            }
            case CMPMODE::NE: {
                VF_CALL<CompareWithoutDstCounterModeImplVF<T, isSetMask, CMPMODE::NE>>(
                    src0, src1, tempBuf, mask[0], repeatParams);
                break;
            }
            default:
                break;
        }
    } else {
        if constexpr (isSetMask) {
            SetVectorMask<T>(mask[1], mask[0]);
        }
        switch (cmpMode) {
            case CMPMODE::LT: {
                VF_CALL<CompareWithoutDstNormalModeImplVF<T, isSetMask, true, CMPMODE::LT>>(
                    src0, src1, tempBuf, 0, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                VF_CALL<CompareWithoutDstNormalModeImplVF<T, isSetMask, true, CMPMODE::GT>>(
                    src0, src1, tempBuf, 0, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                VF_CALL<CompareWithoutDstNormalModeImplVF<T, isSetMask, true, CMPMODE::EQ>>(
                    src0, src1, tempBuf, 0, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                VF_CALL<CompareWithoutDstNormalModeImplVF<T, isSetMask, true, CMPMODE::LE>>(
                    src0, src1, tempBuf, 0, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                VF_CALL<CompareWithoutDstNormalModeImplVF<T, isSetMask, true, CMPMODE::GE>>(
                    src0, src1, tempBuf, 0, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                VF_CALL<CompareWithoutDstNormalModeImplVF<T, isSetMask, true, CMPMODE::NE>>(
                    src0, src1, tempBuf, 0, repeatParams);
                break;
            }
            default:
                break;
        }
    }
    event_t eventIdVToS = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_S));
    SetFlag<HardEvent::V_S>(eventIdVToS);
    WaitFlag<HardEvent::V_S>(eventIdVToS);
    if constexpr (sizeof(T) == 2) {
        Internal::g_cmpMaskLow = static_cast<uint64_t>(tempBuf[0]);
        Internal::g_cmpMaskHigh = static_cast<uint64_t>(tempBuf[1]);
    } else if constexpr (sizeof(T) == 4) {
        Internal::g_cmpMaskLow = static_cast<uint64_t>(tempBuf[0]);
        Internal::g_cmpMaskHigh = static_cast<uint64_t>(0);
    }
    AscendCUtils::FreeTemporaryBuffer<uint64_t>(tempBuf);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void VcmpImpl(
    __ubuf__ T* src0, __ubuf__ T* src1, CMPMODE cmpMode, const uint64_t mask, const BinaryRepeatParams& repeatParams)
{
    static_assert(SupportType<T, half, float>(), "current data type is not supported!");
    bool isCounterMode = Internal::IsCounterMode();
    event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
    SetFlag<HardEvent::S_V>(eventIdSToV);
    WaitFlag<HardEvent::S_V>(eventIdSToV);
    __ubuf__ uint64_t* tempBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(GetRuntimeUBSize(), 2);
    if (isCounterMode) {
        switch (cmpMode) {
            case CMPMODE::LT: {
                VF_CALL<CompareWithoutDstCounterModeImplVF<T, isSetMask, CMPMODE::LT>>(
                    src0, src1, tempBuf, mask, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                VF_CALL<CompareWithoutDstCounterModeImplVF<T, isSetMask, CMPMODE::GT>>(
                    src0, src1, tempBuf, mask, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                VF_CALL<CompareWithoutDstCounterModeImplVF<T, isSetMask, CMPMODE::EQ>>(
                    src0, src1, tempBuf, mask, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                VF_CALL<CompareWithoutDstCounterModeImplVF<T, isSetMask, CMPMODE::LE>>(
                    src0, src1, tempBuf, mask, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                VF_CALL<CompareWithoutDstCounterModeImplVF<T, isSetMask, CMPMODE::GE>>(
                    src0, src1, tempBuf, mask, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                VF_CALL<CompareWithoutDstCounterModeImplVF<T, isSetMask, CMPMODE::NE>>(
                    src0, src1, tempBuf, mask, repeatParams);
                break;
            }
            default:
                break;
        }
    } else {
        switch (cmpMode) {
            case CMPMODE::LT: {
                VF_CALL<CompareWithoutDstNormalModeImplVF<T, isSetMask, false, CMPMODE::LT>>(
                    src0, src1, tempBuf, mask, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                VF_CALL<CompareWithoutDstNormalModeImplVF<T, isSetMask, false, CMPMODE::GT>>(
                    src0, src1, tempBuf, mask, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                VF_CALL<CompareWithoutDstNormalModeImplVF<T, isSetMask, false, CMPMODE::EQ>>(
                    src0, src1, tempBuf, mask, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                VF_CALL<CompareWithoutDstNormalModeImplVF<T, isSetMask, false, CMPMODE::LE>>(
                    src0, src1, tempBuf, mask, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                VF_CALL<CompareWithoutDstNormalModeImplVF<T, isSetMask, false, CMPMODE::GE>>(
                    src0, src1, tempBuf, mask, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                VF_CALL<CompareWithoutDstNormalModeImplVF<T, isSetMask, false, CMPMODE::NE>>(
                    src0, src1, tempBuf, mask, repeatParams);
                break;
            }
            default:
                break;
        }
    }
    event_t eventIdVToS = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_S));
    SetFlag<HardEvent::V_S>(eventIdVToS);
    WaitFlag<HardEvent::V_S>(eventIdVToS);
    if constexpr (sizeof(T) == 2) {
        Internal::g_cmpMaskLow = static_cast<uint64_t>(tempBuf[0]);
        Internal::g_cmpMaskHigh = static_cast<uint64_t>(tempBuf[1]);
    } else if constexpr (sizeof(T) == 4) {
        Internal::g_cmpMaskLow = static_cast<uint64_t>(tempBuf[0]);
        Internal::g_cmpMaskHigh = static_cast<uint64_t>(0);
    }
    AscendCUtils::FreeTemporaryBuffer<uint64_t>(tempBuf);
}

// Compare::Level 0 - bit mode / Continuous mode support b16/b32
template <typename T, typename U, CMPMODE cmpMode>
__aicore__ inline void CompareLevel0CounterMode(
    __ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const BinaryRepeatParams& repeatParams)
{
    Reg::MaskReg maskReg;
    Reg::RegTensor<T> src0Reg, src1Reg;
    Reg::MaskReg dstReg;
    Reg::UnalignReg uReg;
    uint32_t sreg = static_cast<uint32_t>(mask);
    constexpr uint16_t oneRepSize = GetVecLen() / sizeof(T);
    uint16_t newRepeatTimes = CeilDivision(sreg, oneRepSize);
    for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeatTimes); ++i) {
        maskReg = Reg::UpdateMask<T>(sreg);
        Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
            src0Reg, src0, (uint32_t)repeatParams.src0BlkStride, (uint32_t)repeatParams.src0RepStride, maskReg);
        Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
            src1Reg, src1, (uint32_t)repeatParams.src1BlkStride, (uint32_t)repeatParams.src1RepStride, maskReg);
        Reg::Compare<T, cmpMode>(dstReg, src0Reg, src1Reg, maskReg);
        Reg::StoreUnAlign((__ubuf__ T*&)dst, dstReg, uReg);
    }
    Reg::StoreUnAlignPost<U, Reg::PostLiteral::POST_MODE_NORMAL>(dst, uReg, 0);
}

// Compare::Level 0 - bit mode / Continuous mode support b8/b16/b32
template <typename T, typename U, CMPMODE cmpMode, bool isBitMapMode>
__aicore__ inline void CompareLevel0NormalMode(
    __ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    Reg::MaskReg maskReg;
    Reg::RegTensor<T> src0Reg, src1Reg;
    Reg::MaskReg dstReg;
    Reg::UnalignReg uReg;
    if constexpr (isBitMapMode) {
        maskReg = Reg::MoveMask<T>();
    } else {
        uint32_t sreg = static_cast<uint32_t>(mask);
        maskReg = Reg::UpdateMask<T>(sreg);
    }
    for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {
        Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
            src0Reg, src0, (uint32_t)repeatParams.src0BlkStride, (uint32_t)repeatParams.src0RepStride, maskReg);
        Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
            src1Reg, src1, (uint32_t)repeatParams.src1BlkStride, (uint32_t)repeatParams.src1RepStride, maskReg);
        Reg::Compare<T, cmpMode>(dstReg, src0Reg, src1Reg, maskReg);
        Reg::StoreUnAlign((__ubuf__ T*&)dst, dstReg, uReg);
    }
    Reg::StoreUnAlignPost<U, Reg::PostLiteral::POST_MODE_NORMAL>(dst, uReg, 0);
}

template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void VcmpvImpl(
    __ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, CMPMODE cmpMode, const uint64_t mask[], uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, int16_t, uint16_t, bfloat16_t, int32_t, uint32_t, float>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t, int8_t>(), "current data type is not supported!");
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        switch (cmpMode) {
            case CMPMODE::LT: {
                VF_CALL<CompareLevel0CounterMode<T, U, CMPMODE::LT>>(dst, src0, src1, mask[0], repeatParams);
                break;
            }
            case CMPMODE::GT: {
                VF_CALL<CompareLevel0CounterMode<T, U, CMPMODE::GT>>(dst, src0, src1, mask[0], repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                VF_CALL<CompareLevel0CounterMode<T, U, CMPMODE::EQ>>(dst, src0, src1, mask[0], repeatParams);
                break;
            }
            case CMPMODE::LE: {
                VF_CALL<CompareLevel0CounterMode<T, U, CMPMODE::LE>>(dst, src0, src1, mask[0], repeatParams);
                break;
            }
            case CMPMODE::GE: {
                VF_CALL<CompareLevel0CounterMode<T, U, CMPMODE::GE>>(dst, src0, src1, mask[0], repeatParams);
                break;
            }
            case CMPMODE::NE: {
                VF_CALL<CompareLevel0CounterMode<T, U, CMPMODE::NE>>(dst, src0, src1, mask[0], repeatParams);
                break;
            }
            default:
                break;
        }
    } else {
        if constexpr (isSetMask) {
            SetVectorMask<T>(mask[1], mask[0]);
        }
        switch (cmpMode) {
            case CMPMODE::LT: {
                VF_CALL<CompareLevel0NormalMode<T, U, CMPMODE::LT, true>>(dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                VF_CALL<CompareLevel0NormalMode<T, U, CMPMODE::GT, true>>(dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                VF_CALL<CompareLevel0NormalMode<T, U, CMPMODE::EQ, true>>(dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                VF_CALL<CompareLevel0NormalMode<T, U, CMPMODE::LE, true>>(dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                VF_CALL<CompareLevel0NormalMode<T, U, CMPMODE::GE, true>>(dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                VF_CALL<CompareLevel0NormalMode<T, U, CMPMODE::NE, true>>(dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            default:
                break;
        }
    }
}

template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void VcmpvImpl(
    __ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, CMPMODE cmpMode, const uint64_t mask, uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, bfloat16_t, half, int16_t, uint16_t, int32_t, uint32_t, float>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t, int8_t>(), "current data type is not supported!");
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        switch (cmpMode) {
            case CMPMODE::LT: {
                VF_CALL<CompareLevel0CounterMode<T, U, CMPMODE::LT>>(dst, src0, src1, mask, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                VF_CALL<CompareLevel0CounterMode<T, U, CMPMODE::GT>>(dst, src0, src1, mask, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                VF_CALL<CompareLevel0CounterMode<T, U, CMPMODE::EQ>>(dst, src0, src1, mask, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                VF_CALL<CompareLevel0CounterMode<T, U, CMPMODE::LE>>(dst, src0, src1, mask, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                VF_CALL<CompareLevel0CounterMode<T, U, CMPMODE::GE>>(dst, src0, src1, mask, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                VF_CALL<CompareLevel0CounterMode<T, U, CMPMODE::NE>>(dst, src0, src1, mask, repeatParams);
                break;
            }
            default:
                break;
        }
    } else {
        switch (cmpMode) {
            case CMPMODE::LT: {
                VF_CALL<CompareLevel0NormalMode<T, U, CMPMODE::LT, false>>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                VF_CALL<CompareLevel0NormalMode<T, U, CMPMODE::GT, false>>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                VF_CALL<CompareLevel0NormalMode<T, U, CMPMODE::EQ, false>>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                VF_CALL<CompareLevel0NormalMode<T, U, CMPMODE::LE, false>>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                VF_CALL<CompareLevel0NormalMode<T, U, CMPMODE::GE, false>>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                VF_CALL<CompareLevel0NormalMode<T, U, CMPMODE::NE, false>>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            default:
                break;
        }
    }
}

/* ***************************************************************************************
 * ************************************** CompareScalar ****************************************
 * ************************************************************************************** */
// CompareScalar::Level 0 - bit mode / continuous mode
template <typename T, typename U, CMPMODE cmpMode, bool isSetMask>
__aicore__ inline void CompareScalarLevel0CounterMode(
    __ubuf__ U* dst, __ubuf__ T* src0, const T src1, const uint64_t mask, __ubuf__ uint64_t* tempBuf,
    const UnaryRepeatParams& repeatParams)
{
    Reg::MaskReg maskReg;
    Reg::RegTensor<T> src0Reg;
    Reg::MaskReg dstReg;
    Reg::UnalignReg uReg;
    uint32_t sreg = static_cast<uint32_t>(mask);
    constexpr uint16_t oneRepSize = GetVecLen() / sizeof(T);
    if constexpr (!isSetMask) {
        maskReg = Reg::MoveMask<uint16_t>();
        Reg::StoreAlign<uint64_t, Reg::MaskDist::DIST_PACK>(tempBuf, maskReg);
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::SCALAR_LOAD>();
        sreg = static_cast<uint32_t>(tempBuf[0]);
    }
    uint16_t newRepeatTimes = CeilDivision(sreg, oneRepSize);
    for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeatTimes); ++i) {
        maskReg = Reg::UpdateMask<T>(sreg);
        Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
            src0Reg, src0, (uint32_t)repeatParams.srcBlkStride, (uint32_t)repeatParams.srcRepStride, maskReg);
        Reg::CompareScalar<T, cmpMode>(dstReg, src0Reg, src1, maskReg);
        Reg::StoreUnAlign((__ubuf__ T*&)dst, dstReg, uReg);
    }
    Reg::StoreUnAlignPost<U, Reg::PostLiteral::POST_MODE_NORMAL>(dst, uReg, 0);
}

template <typename T, typename U, CMPMODE cmpMode, bool isBitMapMode, bool isSetMask>
__aicore__ inline void CompareScalarLevel0NormalMode(
    __ubuf__ U* dst, __ubuf__ T* src0, const T src1, const uint64_t mask, uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    Reg::MaskReg maskReg;
    Reg::RegTensor<T> src0Reg;
    Reg::MaskReg dstReg;
    Reg::UnalignReg uReg;
    if constexpr (isBitMapMode) {
        maskReg = Reg::MoveMask<T>();
    } else {
        if constexpr (isSetMask) {
            uint32_t sreg = static_cast<uint32_t>(mask);
            maskReg = Reg::UpdateMask<T>(sreg);
        } else {
            maskReg = Reg::MoveMask<T>();
        }
    }
    for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {
        Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
            src0Reg, src0, (uint32_t)repeatParams.srcBlkStride, (uint32_t)repeatParams.srcRepStride, maskReg);
        Reg::CompareScalar<T, cmpMode>(dstReg, src0Reg, src1, maskReg);
        Reg::StoreUnAlign((__ubuf__ T*&)dst, dstReg, uReg);
    }
    Reg::StoreUnAlignPost<U, Reg::PostLiteral::POST_MODE_NORMAL>(dst, uReg, 0);
}

template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void VcmpvsImpl(
    __ubuf__ U* dst, __ubuf__ T* src0, const T src1, CMPMODE cmpMode, const uint64_t mask[], uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, int16_t, uint16_t, bfloat16_t, int32_t, uint32_t, float>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t>(), "current data type is not supported!");
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        __ubuf__ uint64_t* tempBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(GetRuntimeUBSize(), 2);
        switch (cmpMode) {
            case CMPMODE::LT: {
                VF_CALL<CompareScalarLevel0CounterMode<T, U, CMPMODE::LT, isSetMask>>(
                    dst, src0, src1, mask[0], tempBuf, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                VF_CALL<CompareScalarLevel0CounterMode<T, U, CMPMODE::GT, isSetMask>>(
                    dst, src0, src1, mask[0], tempBuf, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                VF_CALL<CompareScalarLevel0CounterMode<T, U, CMPMODE::EQ, isSetMask>>(
                    dst, src0, src1, mask[0], tempBuf, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                VF_CALL<CompareScalarLevel0CounterMode<T, U, CMPMODE::LE, isSetMask>>(
                    dst, src0, src1, mask[0], tempBuf, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                VF_CALL<CompareScalarLevel0CounterMode<T, U, CMPMODE::GE, isSetMask>>(
                    dst, src0, src1, mask[0], tempBuf, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                VF_CALL<CompareScalarLevel0CounterMode<T, U, CMPMODE::NE, isSetMask>>(
                    dst, src0, src1, mask[0], tempBuf, repeatParams);
                break;
            }
            default:
                break;
        }
        AscendCUtils::FreeTemporaryBuffer<uint64_t>(tempBuf);
    } else {
        if constexpr (isSetMask) {
            SetVectorMask<T>(mask[1], mask[0]);
        }
        switch (cmpMode) {
            case CMPMODE::LT: {
                VF_CALL<CompareScalarLevel0NormalMode<T, U, CMPMODE::LT, true, isSetMask>>(
                    dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                VF_CALL<CompareScalarLevel0NormalMode<T, U, CMPMODE::GT, true, isSetMask>>(
                    dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                VF_CALL<CompareScalarLevel0NormalMode<T, U, CMPMODE::EQ, true, isSetMask>>(
                    dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                VF_CALL<CompareScalarLevel0NormalMode<T, U, CMPMODE::LE, true, isSetMask>>(
                    dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                VF_CALL<CompareScalarLevel0NormalMode<T, U, CMPMODE::GE, true, isSetMask>>(
                    dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                VF_CALL<CompareScalarLevel0NormalMode<T, U, CMPMODE::NE, true, isSetMask>>(
                    dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            default:
                break;
        }
    }
}

template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void VcmpvsImpl(
    __ubuf__ U* dst, __ubuf__ T* src0, const T src1, CMPMODE cmpMode, const uint64_t mask, uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, bfloat16_t, half, int16_t, uint16_t, int32_t, uint32_t, float>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t>(), "current data type is not supported!");
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        __ubuf__ uint64_t* tempBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(GetRuntimeUBSize(), 2);
        switch (cmpMode) {
            case CMPMODE::LT: {
                VF_CALL<CompareScalarLevel0CounterMode<T, U, CMPMODE::LT, isSetMask>>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                VF_CALL<CompareScalarLevel0CounterMode<T, U, CMPMODE::GT, isSetMask>>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                VF_CALL<CompareScalarLevel0CounterMode<T, U, CMPMODE::EQ, isSetMask>>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                VF_CALL<CompareScalarLevel0CounterMode<T, U, CMPMODE::LE, isSetMask>>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                VF_CALL<CompareScalarLevel0CounterMode<T, U, CMPMODE::GE, isSetMask>>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                VF_CALL<CompareScalarLevel0CounterMode<T, U, CMPMODE::NE, isSetMask>>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            default:
                break;
        }
        AscendCUtils::FreeTemporaryBuffer<uint64_t>(tempBuf);
    } else {
        switch (cmpMode) {
            case CMPMODE::LT: {
                VF_CALL<CompareScalarLevel0NormalMode<T, U, CMPMODE::LT, false, isSetMask>>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                VF_CALL<CompareScalarLevel0NormalMode<T, U, CMPMODE::GT, false, isSetMask>>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                VF_CALL<CompareScalarLevel0NormalMode<T, U, CMPMODE::EQ, false, isSetMask>>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                VF_CALL<CompareScalarLevel0NormalMode<T, U, CMPMODE::LE, false, isSetMask>>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                VF_CALL<CompareScalarLevel0NormalMode<T, U, CMPMODE::GE, false, isSetMask>>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                VF_CALL<CompareScalarLevel0NormalMode<T, U, CMPMODE::NE, false, isSetMask>>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            default:
                break;
        }
    }
}

// CompareScalar::Level 2 - counter mode
template <typename T, typename U, CMPMODE cmpMode>
__aicore__ inline void CompareScalarLevel2(__ubuf__ U* dst, __ubuf__ T* src0, const T src1, const uint32_t calCount)
{
    Reg::MaskReg dstReg, maskReg;
    Reg::UnalignReg uReg;
    uint32_t repeatElm = GetVecLen() / sizeof(T);
    uint16_t repeatTime = CeilDivision(calCount, repeatElm);
    uint32_t sreg = static_cast<uint32_t>(calCount);
    if constexpr (sizeof(T) == 8) {
        repeatElm = repeatElm * 2;
        repeatTime = CeilDivision(calCount, repeatElm);
        Reg::RegTensor<T, Reg::RegTraitNumTwo> src0Reg;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            maskReg = Reg::UpdateMask<T, Reg::RegTraitNumTwo>(sreg);
            Reg::LoadAlign(src0Reg, src0 + i * repeatElm);
            Reg::CompareScalar<T, cmpMode>(dstReg, src0Reg, src1, maskReg);
            Reg::StoreUnAlign((__ubuf__ uint32_t*&)dst, dstReg, uReg);
        }
        Reg::StoreUnAlignPost<U, Reg::PostLiteral::POST_MODE_NORMAL>(dst, uReg, 0);
    } else {
        Reg::RegTensor<T> src0Reg;
        constexpr uint32_t offset = GetVecLen() / sizeof(T) / CmpSelInternal::maskBitToByte;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign(src0Reg, src0 + i * repeatElm);
            Reg::CompareScalar<T, cmpMode>(dstReg, src0Reg, src1, maskReg);
            if constexpr (sizeof(T) == 1) {
                Reg::StoreAlign(dst + i * offset, dstReg);
            } else {
                Reg::StoreUnAlign((__ubuf__ T*&)dst, dstReg, uReg);
            }
        }
        if constexpr (sizeof(T) > 1) {
            Reg::StoreUnAlignPost<U, Reg::PostLiteral::POST_MODE_NORMAL>(dst, uReg, 0);
        }
    }
}

template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void VcmpvsImpl(
    __ubuf__ U* dst, __ubuf__ T* src0, const T src1, CMPMODE cmpMode, const uint32_t calCount)
{
    static_assert(
        SupportType<
            T, uint8_t, int8_t, bfloat16_t, half, int16_t, uint16_t, int32_t, uint32_t, float, uint64_t, int64_t>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t>(), "current data type is not supported!");
    switch (cmpMode) {
        case CMPMODE::LT: {
            VF_CALL<CompareScalarLevel2<T, U, CMPMODE::LT>>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::GT: {
            VF_CALL<CompareScalarLevel2<T, U, CMPMODE::GT>>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::EQ: {
            VF_CALL<CompareScalarLevel2<T, U, CMPMODE::EQ>>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::LE: {
            VF_CALL<CompareScalarLevel2<T, U, CMPMODE::LE>>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::GE: {
            VF_CALL<CompareScalarLevel2<T, U, CMPMODE::GE>>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::NE: {
            VF_CALL<CompareScalarLevel2<T, U, CMPMODE::NE>>(dst, src0, src1, calCount);
            break;
        }
        default:
            break;
    }
}

/* ***************************************************************************************
 * ************************************** CompareScalar src0 scalar****************************************
 * ************************************************************************************** */
// CompareScalar::Level 0 - bit mode / continuous mode
template <typename T, typename U, CMPMODE cmpMode, bool isSetMask>
__aicore__ inline void CompareSrc0ScalarLevel0CounterMode(
    __ubuf__ U* dst, const T src0, __ubuf__ T* src1, const uint64_t mask, __ubuf__ uint64_t* tempBuf,
    const UnaryRepeatParams& repeatParams)
{
    Reg::MaskReg maskReg;
    uint32_t sreg = static_cast<uint32_t>(mask);
    constexpr uint16_t oneRepSize = GetVecLen() / sizeof(T);
    Reg::RegTensor<T> src0Reg, src1Reg;
    Reg::MaskReg dstReg;
    Reg::UnalignReg uReg;
    Reg::Duplicate(src0Reg, src0);
    if constexpr (!isSetMask) {
        maskReg = Reg::MoveMask<uint16_t>();
        Reg::StoreAlign<uint64_t, Reg::MaskDist::DIST_PACK>(tempBuf, maskReg);
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::SCALAR_LOAD>();
        sreg = static_cast<uint32_t>(tempBuf[0]);
    }
    uint16_t newRepeatTimes = CeilDivision(sreg, oneRepSize);
    for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeatTimes); ++i) {
        maskReg = Reg::UpdateMask<T>(sreg);
        Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
            src1Reg, src1, (uint32_t)repeatParams.srcBlkStride, (uint32_t)repeatParams.srcRepStride, maskReg);
        Reg::Compare<T, cmpMode>(dstReg, src0Reg, src1Reg, maskReg);
        Reg::StoreUnAlign((__ubuf__ T*&)dst, dstReg, uReg);
    }
    Reg::StoreUnAlignPost<U, Reg::PostLiteral::POST_MODE_NORMAL>(dst, uReg, 0);
}

template <typename T, typename U, CMPMODE cmpMode, bool isBitMapMode, bool isSetMask>
__aicore__ inline void CompareSrc0ScalarLevel0NormalMode(
    __ubuf__ U* dst, const T src0, __ubuf__ T* src1, const uint64_t mask, uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    Reg::MaskReg maskReg;
    Reg::RegTensor<T> src0Reg, src1Reg;
    Reg::MaskReg dstReg;
    Reg::UnalignReg uReg;
    if constexpr (isBitMapMode) {
        maskReg = Reg::MoveMask<T>();
    } else {
        if constexpr (isSetMask) {
            uint32_t sreg = static_cast<uint32_t>(mask);
            maskReg = Reg::UpdateMask<T>(sreg);
        } else {
            maskReg = Reg::MoveMask<T>();
        }
    }
    Reg::Duplicate(src0Reg, src0);
    for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {
        Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
            src1Reg, src1, (uint32_t)repeatParams.srcBlkStride, (uint32_t)repeatParams.srcRepStride, maskReg);
        Reg::Compare<T, cmpMode>(dstReg, src0Reg, src1Reg, maskReg);
        Reg::StoreUnAlign((__ubuf__ T*&)dst, dstReg, uReg);
    }
    Reg::StoreUnAlignPost<U, Reg::PostLiteral::POST_MODE_NORMAL>(dst, uReg, 0);
}

template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void VcmpvsImpl(
    __ubuf__ U* dst, const T src0, __ubuf__ T* src1, CMPMODE cmpMode, const uint64_t mask[], uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, int16_t, uint16_t, bfloat16_t, int32_t, uint32_t, float>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t>(), "current data type is not supported!");
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        __ubuf__ uint64_t* tempBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(GetRuntimeUBSize(), 2);
        switch (cmpMode) {
            case CMPMODE::LT: {
                VF_CALL<CompareSrc0ScalarLevel0CounterMode<T, U, CMPMODE::LT, isSetMask>>(
                    dst, src0, src1, mask[0], tempBuf, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                VF_CALL<CompareSrc0ScalarLevel0CounterMode<T, U, CMPMODE::GT, isSetMask>>(
                    dst, src0, src1, mask[0], tempBuf, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                VF_CALL<CompareSrc0ScalarLevel0CounterMode<T, U, CMPMODE::EQ, isSetMask>>(
                    dst, src0, src1, mask[0], tempBuf, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                VF_CALL<CompareSrc0ScalarLevel0CounterMode<T, U, CMPMODE::LE, isSetMask>>(
                    dst, src0, src1, mask[0], tempBuf, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                VF_CALL<CompareSrc0ScalarLevel0CounterMode<T, U, CMPMODE::GE, isSetMask>>(
                    dst, src0, src1, mask[0], tempBuf, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                VF_CALL<CompareSrc0ScalarLevel0CounterMode<T, U, CMPMODE::NE, isSetMask>>(
                    dst, src0, src1, mask[0], tempBuf, repeatParams);
                break;
            }
            default:
                break;
        }
        AscendCUtils::FreeTemporaryBuffer<uint64_t>(tempBuf);
    } else {
        if constexpr (isSetMask) {
            SetVectorMask<T>(mask[1], mask[0]);
        }
        switch (cmpMode) {
            case CMPMODE::LT: {
                VF_CALL<CompareSrc0ScalarLevel0NormalMode<T, U, CMPMODE::LT, true, isSetMask>>(
                    dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                VF_CALL<CompareSrc0ScalarLevel0NormalMode<T, U, CMPMODE::GT, true, isSetMask>>(
                    dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                VF_CALL<CompareSrc0ScalarLevel0NormalMode<T, U, CMPMODE::EQ, true, isSetMask>>(
                    dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                VF_CALL<CompareSrc0ScalarLevel0NormalMode<T, U, CMPMODE::LE, true, isSetMask>>(
                    dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                VF_CALL<CompareSrc0ScalarLevel0NormalMode<T, U, CMPMODE::GE, true, isSetMask>>(
                    dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                VF_CALL<CompareSrc0ScalarLevel0NormalMode<T, U, CMPMODE::NE, true, isSetMask>>(
                    dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            default:
                break;
        }
    }
}

template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void VcmpvsImpl(
    __ubuf__ U* dst, const T src0, __ubuf__ T* src1, CMPMODE cmpMode, const uint64_t mask, uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, bfloat16_t, half, int16_t, uint16_t, int32_t, uint32_t, float>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t>(), "current data type is not supported!");
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        __ubuf__ uint64_t* tempBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(GetRuntimeUBSize(), 2);
        switch (cmpMode) {
            case CMPMODE::LT: {
                VF_CALL<CompareSrc0ScalarLevel0CounterMode<T, U, CMPMODE::LT, isSetMask>>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                VF_CALL<CompareSrc0ScalarLevel0CounterMode<T, U, CMPMODE::GT, isSetMask>>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                VF_CALL<CompareSrc0ScalarLevel0CounterMode<T, U, CMPMODE::EQ, isSetMask>>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                VF_CALL<CompareSrc0ScalarLevel0CounterMode<T, U, CMPMODE::LE, isSetMask>>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                VF_CALL<CompareSrc0ScalarLevel0CounterMode<T, U, CMPMODE::GE, isSetMask>>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                VF_CALL<CompareSrc0ScalarLevel0CounterMode<T, U, CMPMODE::NE, isSetMask>>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            default:
                break;
        }
        AscendCUtils::FreeTemporaryBuffer<uint64_t>(tempBuf);
    } else {
        switch (cmpMode) {
            case CMPMODE::LT: {
                VF_CALL<CompareSrc0ScalarLevel0NormalMode<T, U, CMPMODE::LT, false, isSetMask>>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                VF_CALL<CompareSrc0ScalarLevel0NormalMode<T, U, CMPMODE::GT, false, isSetMask>>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                VF_CALL<CompareSrc0ScalarLevel0NormalMode<T, U, CMPMODE::EQ, false, isSetMask>>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                VF_CALL<CompareSrc0ScalarLevel0NormalMode<T, U, CMPMODE::LE, false, isSetMask>>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                VF_CALL<CompareSrc0ScalarLevel0NormalMode<T, U, CMPMODE::GE, false, isSetMask>>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                VF_CALL<CompareSrc0ScalarLevel0NormalMode<T, U, CMPMODE::NE, false, isSetMask>>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            default:
                break;
        }
    }
}

// CompareScalar::Level 2 - counter mode

template <typename T, typename U, CMPMODE cmpMode>
__aicore__ inline void CompareSrc0ScalarLevel2(__ubuf__ U* dst, const T src0, __ubuf__ T* src1, const uint32_t calCount)
{
    uint32_t repeatElm = GetVecLen() / sizeof(T);
    uint16_t repeatTime = CeilDivision(calCount, repeatElm);
    uint32_t sreg = static_cast<uint32_t>(calCount);
    Reg::MaskReg dstReg, maskReg;
    Reg::UnalignReg uReg;
    if constexpr (sizeof(T) == 8) {
        repeatElm = repeatElm * 2;
        repeatTime = CeilDivision(calCount, repeatElm);
        Reg::RegTensor<T, Reg::RegTraitNumTwo> src0Reg, src1Reg;
        Reg::Duplicate(src0Reg, src0);
        for (uint16_t i = 0; i < repeatTime; ++i) {
            maskReg = Reg::UpdateMask<T, Reg::RegTraitNumTwo>(sreg);
            Reg::LoadAlign(src1Reg, src1 + i * repeatElm);
            Reg::Compare<T, cmpMode>(dstReg, src0Reg, src1Reg, maskReg);
            Reg::StoreUnAlign((__ubuf__ uint32_t*&)dst, dstReg, uReg);
        }
        Reg::StoreUnAlignPost<U, Reg::PostLiteral::POST_MODE_NORMAL>(dst, uReg, 0);
    } else {
        Reg::RegTensor<T> src0Reg, src1Reg;
        constexpr uint32_t offset = GetVecLen() / sizeof(T) / CmpSelInternal::maskBitToByte;
        Reg::Duplicate(src0Reg, src0);
        for (uint16_t i = 0; i < repeatTime; ++i) {
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign(src1Reg, src1 + i * repeatElm);
            Reg::Compare<T, cmpMode>(dstReg, src0Reg, src1Reg, maskReg);
            if constexpr (sizeof(T) == 1) {
                Reg::StoreAlign(dst + i * offset, dstReg);
            } else {
                Reg::StoreUnAlign((__ubuf__ T*&)dst, dstReg, uReg);
            }
        }
        if constexpr (sizeof(T) > 1) {
            Reg::StoreUnAlignPost<U, Reg::PostLiteral::POST_MODE_NORMAL>(dst, uReg, 0);
        }
    }
}

template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void VcmpvsImpl(
    __ubuf__ U* dst, const T src0, __ubuf__ T* src1, CMPMODE cmpMode, const uint32_t calCount)
{
    static_assert(
        SupportType<
            T, uint8_t, int8_t, bfloat16_t, half, int16_t, uint16_t, int32_t, uint32_t, float, uint64_t, int64_t>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t>(), "current data type is not supported!");
    switch (cmpMode) {
        case CMPMODE::LT: {
            VF_CALL<CompareSrc0ScalarLevel2<T, U, CMPMODE::LT>>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::GT: {
            VF_CALL<CompareSrc0ScalarLevel2<T, U, CMPMODE::GT>>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::EQ: {
            VF_CALL<CompareSrc0ScalarLevel2<T, U, CMPMODE::EQ>>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::LE: {
            VF_CALL<CompareSrc0ScalarLevel2<T, U, CMPMODE::LE>>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::GE: {
            VF_CALL<CompareSrc0ScalarLevel2<T, U, CMPMODE::GE>>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::NE: {
            VF_CALL<CompareSrc0ScalarLevel2<T, U, CMPMODE::NE>>(dst, src0, src1, calCount);
            break;
        }
        default:
            break;
    }
}
/* ***************************************************************************************
 * ************************************** CompareScalar Both Tensor****************************************
 * ************************************************************************************** */
// CompareScalar::Level 0 - bit mode / continuous mode

template <typename T, typename U, CMPMODE cmpMode, uint8_t scalarIdx, Reg::LoadDist pattern>
__aicore__ inline void CompareScalarBothTensorLevel2(
    __ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint32_t calCount)
{
    constexpr uint32_t repeatElm = GetVecLen() / sizeof(T);
    uint16_t repeatTime = CeilDivision(calCount, repeatElm);
    uint32_t sreg = static_cast<uint32_t>(calCount);
    Reg::MaskReg dstReg, maskReg;
    Reg::UnalignReg uReg;
    Reg::RegTensor<T> src0Reg, src1Reg;
    constexpr uint32_t offset = GetVecLen() / sizeof(T) / CmpSelInternal::maskBitToByte;
    if constexpr (scalarIdx == 0) {
        Reg::LoadAlign<T, pattern>(src0Reg, src0);
    } else {
        Reg::LoadAlign<T, pattern>(src1Reg, src1);
    }
    for (uint16_t i = 0; i < repeatTime; ++i) {
        maskReg = Reg::UpdateMask<T>(sreg);
        if constexpr (scalarIdx == 0) {
            Reg::LoadAlign(src1Reg, src1 + i * repeatElm);
        } else {
            Reg::LoadAlign(src0Reg, src0 + i * repeatElm);
        }
        Reg::Compare<T, cmpMode>(dstReg, src0Reg, src1Reg, maskReg);
        if constexpr (sizeof(T) == 1) {
            Reg::StoreAlign(dst + i * offset, dstReg);
        } else {
            Reg::StoreUnAlign((__ubuf__ T*&)dst, dstReg, uReg);
        }
    }
    if constexpr (sizeof(T) > 1) {
        Reg::StoreUnAlignPost<U, Reg::PostLiteral::POST_MODE_NORMAL>(dst, uReg, 0);
    }
}

template <typename T, typename U, bool isSetMask = true, uint8_t scalarIdx = 1, Reg::LoadDist pattern>
__aicore__ inline void VcmpvsImpl(
    __ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, CMPMODE cmpMode, const uint32_t calCount)
{
    switch (cmpMode) {
        case CMPMODE::LT: {
            VF_CALL<CompareScalarBothTensorLevel2<T, U, CMPMODE::LT, scalarIdx, pattern>>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::GT: {
            VF_CALL<CompareScalarBothTensorLevel2<T, U, CMPMODE::GT, scalarIdx, pattern>>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::EQ: {
            VF_CALL<CompareScalarBothTensorLevel2<T, U, CMPMODE::EQ, scalarIdx, pattern>>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::LE: {
            VF_CALL<CompareScalarBothTensorLevel2<T, U, CMPMODE::LE, scalarIdx, pattern>>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::GE: {
            VF_CALL<CompareScalarBothTensorLevel2<T, U, CMPMODE::GE, scalarIdx, pattern>>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::NE: {
            VF_CALL<CompareScalarBothTensorLevel2<T, U, CMPMODE::NE, scalarIdx, pattern>>(dst, src0, src1, calCount);
            break;
        }
        default:
            break;
    }
}

template <typename T, typename U, CMPMODE cmpMode, uint8_t scalarIdx>
__aicore__ inline void CompareScalarLevel2B64(
    __ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint32_t calCount)
{
    constexpr uint32_t repeatElm = GetVecLen() / sizeof(T) * 2;
    uint16_t repeatTime = CeilDivision(calCount, repeatElm);
    uint32_t sreg = static_cast<uint32_t>(calCount);
    Reg::MaskReg dstReg, maskReg;
    Reg::UnalignReg uReg, dupuReg;
    repeatTime = CeilDivision(calCount, repeatElm);
    Reg::RegTensor<T, Reg::RegTraitNumTwo> src0Reg, src1Reg, dupReg;
    Reg::RegTensor<T, Reg::RegTraitNumOne> preReg;

    Reg::RegTensor<uint32_t> zeroReg;
    Reg::MaskReg maskFull = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    Reg::Duplicate(zeroReg, 0, maskFull);

    if constexpr (scalarIdx == 0) {
        Reg::LoadUnAlignPre(dupuReg, (__ubuf__ T*)src0);
        Reg::LoadUnAlign(preReg, dupuReg, (__ubuf__ T*)src0);
        Reg::DeInterleave(
            (Reg::RegTensor<uint32_t>&)dupReg.reg[0], (Reg::RegTensor<uint32_t>&)dupReg.reg[1],
            (Reg::RegTensor<uint32_t>&)preReg, zeroReg);
        Reg::Duplicate(src0Reg, dupReg, maskFull);
    } else {
        Reg::LoadUnAlignPre(dupuReg, (__ubuf__ T*)src1);
        Reg::LoadUnAlign(preReg, dupuReg, (__ubuf__ T*)src1);
        Reg::DeInterleave(
            (Reg::RegTensor<uint32_t>&)dupReg.reg[0], (Reg::RegTensor<uint32_t>&)dupReg.reg[1],
            (Reg::RegTensor<uint32_t>&)preReg, zeroReg);
        Reg::Duplicate(src1Reg, dupReg, maskFull);
    }

    for (uint16_t i = 0; i < repeatTime; ++i) {
        maskReg = Reg::UpdateMask<T, Reg::RegTraitNumTwo>(sreg);
        if constexpr (scalarIdx == 0) {
            Reg::LoadAlign(src1Reg, src1 + i * repeatElm);
        } else {
            Reg::LoadAlign(src0Reg, src0 + i * repeatElm);
        }
        Reg::Compare<T, cmpMode>(dstReg, src0Reg, src1Reg, maskReg);
        Reg::StoreUnAlign((__ubuf__ uint32_t*&)dst, dstReg, uReg);
    }
    Reg::StoreUnAlignPost<U, Reg::PostLiteral::POST_MODE_NORMAL>(dst, uReg, 0);
}

template <typename T, typename U, bool isSetMask = true, uint8_t scalarIdx = 1>
__aicore__ inline void VcmpvsImplB64(
    __ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, CMPMODE cmpMode, const uint32_t calCount)
{
    switch (cmpMode) {
        case CMPMODE::LT: {
            VF_CALL<CompareScalarLevel2B64<T, U, CMPMODE::LT, scalarIdx>>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::GT: {
            VF_CALL<CompareScalarLevel2B64<T, U, CMPMODE::GT, scalarIdx>>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::EQ: {
            VF_CALL<CompareScalarLevel2B64<T, U, CMPMODE::EQ, scalarIdx>>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::LE: {
            VF_CALL<CompareScalarLevel2B64<T, U, CMPMODE::LE, scalarIdx>>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::GE: {
            VF_CALL<CompareScalarLevel2B64<T, U, CMPMODE::GE, scalarIdx>>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::NE: {
            VF_CALL<CompareScalarLevel2B64<T, U, CMPMODE::NE, scalarIdx>>(dst, src0, src1, calCount);
            break;
        }
        default:
            break;
    }
}

template <typename T, typename U, bool isSetMask = true, uint8_t scalarIdx = 1>
__aicore__ inline void VcmpvsImpl(
    __ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, CMPMODE cmpMode, const uint32_t calCount)
{
    static_assert(
        SupportType<
            T, uint8_t, int8_t, bfloat16_t, half, int16_t, uint16_t, int32_t, uint32_t, float, uint64_t, int64_t>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t>(), "current data type is not supported!");

    if constexpr (sizeof(T) == 1) {
        VcmpvsImpl<T, U, isSetMask, scalarIdx, Reg::LoadDist::DIST_BRC_B8>(dst, src0, src1, cmpMode, calCount);
    } else if constexpr (sizeof(T) == 2) {
        VcmpvsImpl<T, U, isSetMask, scalarIdx, Reg::LoadDist::DIST_BRC_B16>(dst, src0, src1, cmpMode, calCount);
    } else if constexpr (sizeof(T) == 4) {
        VcmpvsImpl<T, U, isSetMask, scalarIdx, Reg::LoadDist::DIST_BRC_B32>(dst, src0, src1, cmpMode, calCount);
    } else if constexpr (sizeof(T) == 8) {
        VcmpvsImplB64<T, U, isSetMask, scalarIdx>(dst, src0, src1, cmpMode, calCount);
    }
}

template <
    typename T, typename U, CMPMODE cmpMode, bool isBitMapMode, uint8_t scalarIdx, Reg::LoadDist pattern,
    bool isSetMask>
__aicore__ inline void CompareScalarBothTensorLevel0CounterMode(
    __ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, __ubuf__ uint64_t* tempBuf,
    const UnaryRepeatParams& repeatParams)
{
    Reg::MaskReg maskReg;
    uint32_t countSreg = static_cast<uint32_t>(mask);
    if constexpr (!isSetMask) {
        maskReg = Reg::MoveMask<uint16_t>();
        Reg::StoreAlign<uint64_t, Reg::MaskDist::DIST_PACK>(tempBuf, maskReg);
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::SCALAR_LOAD>();
        countSreg = static_cast<uint32_t>(tempBuf[0]);
    }
    constexpr uint16_t oneRepSize = GetVecLen() / sizeof(T);
    uint16_t newRepeatTimes = CeilDivision(countSreg, oneRepSize);
    Reg::RegTensor<T> src0Reg, src1Reg;
    Reg::MaskReg dstReg;
    Reg::UnalignReg uReg;
    if constexpr (scalarIdx == 0) {
        Reg::LoadAlign<T, pattern>(src0Reg, src0);
    } else {
        Reg::LoadAlign<T, pattern>(src1Reg, src1);
    }
    for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeatTimes); ++i) {
        maskReg = Reg::UpdateMask<T>(countSreg);
        if constexpr (scalarIdx == 0) {
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
                src1Reg, src1, (uint32_t)repeatParams.srcBlkStride, (uint32_t)repeatParams.srcRepStride, maskReg);
        } else {
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
                src0Reg, src0, (uint32_t)repeatParams.srcBlkStride, (uint32_t)repeatParams.srcRepStride, maskReg);
        }
        Reg::Compare<T, cmpMode>(dstReg, src0Reg, src1Reg, maskReg);
        Reg::StoreUnAlign((__ubuf__ T*&)dst, dstReg, uReg);
    }
    Reg::StoreUnAlignPost<U, Reg::PostLiteral::POST_MODE_NORMAL>(dst, uReg, 0);
}

template <
    typename T, typename U, CMPMODE cmpMode, bool isBitMapMode, uint8_t scalarIdx, Reg::LoadDist pattern,
    bool isSetMask>
__aicore__ inline void CompareScalarBothTensorLevel0NormalMode(
    __ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    Reg::MaskReg maskReg;
    Reg::RegTensor<T> src0Reg, src1Reg;
    Reg::MaskReg dstReg;
    Reg::UnalignReg uReg;
    if constexpr (isBitMapMode) {
        maskReg = Reg::MoveMask<T>();
    } else {
        if constexpr (isSetMask) {
            uint32_t sreg = static_cast<uint32_t>(mask);
            maskReg = Reg::UpdateMask<T>(sreg);
        } else {
            maskReg = Reg::MoveMask<T>();
        }
    }
    if constexpr (scalarIdx == 0) {
        Reg::LoadAlign<T, pattern>(src0Reg, src0);
    } else {
        Reg::LoadAlign<T, pattern>(src1Reg, src1);
    }
    for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {
        if constexpr (scalarIdx == 0) {
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
                src1Reg, src1, (uint32_t)repeatParams.srcBlkStride, (uint32_t)repeatParams.srcRepStride, maskReg);
        } else {
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
                src0Reg, src0, (uint32_t)repeatParams.srcBlkStride, (uint32_t)repeatParams.srcRepStride, maskReg);
        }
        Reg::Compare<T, cmpMode>(dstReg, src0Reg, src1Reg, maskReg);
        Reg::StoreUnAlign((__ubuf__ T*&)dst, dstReg, uReg);
    }
    Reg::StoreUnAlignPost<U, Reg::PostLiteral::POST_MODE_NORMAL>(dst, uReg, 0);
}

template <typename T, typename U, bool isSetMask = true, bool isBitMapMode, uint8_t scalarIdx, Reg::LoadDist pattern>
__aicore__ inline void VcmpvsImpl(
    __ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, CMPMODE cmpMode, const uint64_t mask, uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        __ubuf__ uint64_t* tempBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(GetRuntimeUBSize(), 2);
        switch (cmpMode) {
            case CMPMODE::LT: {
                VF_CALL<CompareScalarBothTensorLevel0CounterMode<
                    T, U, CMPMODE::LT, isBitMapMode, scalarIdx, pattern, isSetMask>>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                VF_CALL<CompareScalarBothTensorLevel0CounterMode<
                    T, U, CMPMODE::GT, isBitMapMode, scalarIdx, pattern, isSetMask>>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                VF_CALL<CompareScalarBothTensorLevel0CounterMode<
                    T, U, CMPMODE::EQ, isBitMapMode, scalarIdx, pattern, isSetMask>>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                VF_CALL<CompareScalarBothTensorLevel0CounterMode<
                    T, U, CMPMODE::LE, isBitMapMode, scalarIdx, pattern, isSetMask>>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                VF_CALL<CompareScalarBothTensorLevel0CounterMode<
                    T, U, CMPMODE::GE, isBitMapMode, scalarIdx, pattern, isSetMask>>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                VF_CALL<CompareScalarBothTensorLevel0CounterMode<
                    T, U, CMPMODE::NE, isBitMapMode, scalarIdx, pattern, isSetMask>>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            default:
                break;
        }
        AscendCUtils::FreeTemporaryBuffer<uint64_t>(tempBuf);
    } else {
        switch (cmpMode) {
            case CMPMODE::LT: {
                VF_CALL<CompareScalarBothTensorLevel0NormalMode<
                    T, U, CMPMODE::LT, isBitMapMode, scalarIdx, pattern, isSetMask>>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                VF_CALL<CompareScalarBothTensorLevel0NormalMode<
                    T, U, CMPMODE::GT, isBitMapMode, scalarIdx, pattern, isSetMask>>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                VF_CALL<CompareScalarBothTensorLevel0NormalMode<
                    T, U, CMPMODE::EQ, isBitMapMode, scalarIdx, pattern, isSetMask>>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                VF_CALL<CompareScalarBothTensorLevel0NormalMode<
                    T, U, CMPMODE::LE, isBitMapMode, scalarIdx, pattern, isSetMask>>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                VF_CALL<CompareScalarBothTensorLevel0NormalMode<
                    T, U, CMPMODE::GE, isBitMapMode, scalarIdx, pattern, isSetMask>>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                VF_CALL<CompareScalarBothTensorLevel0NormalMode<
                    T, U, CMPMODE::NE, isBitMapMode, scalarIdx, pattern, isSetMask>>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            default:
                break;
        }
    }
}

template <typename T, typename U, bool isSetMask = true, uint8_t scalarIdx = 1>
__aicore__ inline void VcmpvsImpl(
    __ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, CMPMODE cmpMode, const uint64_t mask[], uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, uint8_t, int8_t, half, int16_t, uint16_t, bfloat16_t, int32_t, uint32_t, float>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t>(), "current data type is not supported!");
    bool isCounterMode = Internal::IsCounterMode();
    if (isSetMask) {
        SetVectorMask<T>(mask[1], mask[0]);
    }
    if constexpr (sizeof(T) == 2) {
        VcmpvsImpl<T, U, isSetMask, true, scalarIdx, Reg::LoadDist::DIST_BRC_B16>(
            dst, src0, src1, cmpMode, mask[0], repeatTime, repeatParams);
    } else if constexpr (sizeof(T) == 4) {
        VcmpvsImpl<T, U, isSetMask, true, scalarIdx, Reg::LoadDist::DIST_BRC_B32>(
            dst, src0, src1, cmpMode, mask[0], repeatTime, repeatParams);
    }
}

template <typename T, typename U, bool isSetMask = true, uint8_t scalarIdx = 1>
__aicore__ inline void VcmpvsImpl(
    __ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, CMPMODE cmpMode, const uint64_t mask, uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, bfloat16_t, half, int16_t, uint16_t, int32_t, uint32_t, float>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t>(), "current data type is not supported!");
    if constexpr (sizeof(T) == 2) {
        VcmpvsImpl<T, U, isSetMask, false, scalarIdx, Reg::LoadDist::DIST_BRC_B16>(
            dst, src0, src1, cmpMode, mask, repeatTime, repeatParams);
    } else if constexpr (sizeof(T) == 4) {
        VcmpvsImpl<T, U, isSetMask, false, scalarIdx, Reg::LoadDist::DIST_BRC_B32>(
            dst, src0, src1, cmpMode, mask, repeatTime, repeatParams);
    }
}

/* ***************************************************************************************
 * *************************************** Select ****************************************
 * ************************************************************************************** */

template <typename T, bool isCounterMode>
__aicore__ inline void SelectWithoutMaskMode0ImplVF(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, __ubuf__ uint64_t* tempBuf, int32_t repeat,
    const BinaryRepeatParams& repeatParams)
{
    Reg::RegTensor<T> srcReg0, srcReg1, dstReg;
    Reg::MaskReg maskReg, selMask;
    Reg::RegTensor<uint32_t> selReg;
    Reg::UnalignReg ureg;
    uint16_t newRepeatTimes = repeat;
    uint32_t sreg;
    constexpr uint32_t oneRepSize = GetVecLen() / sizeof(T);
    constexpr uint32_t blockElm = GetDataBlockSizeInBytes() / sizeof(T);
    if constexpr (sizeof(T) == 2) {
        Reg::LoadAlign<uint32_t, Reg::MaskDist::DIST_US>(selMask, (__ubuf__ uint32_t*)tempBuf);
    } else if constexpr (sizeof(T) == 4) {
        Reg::LoadUnAlignPre(ureg, (__ubuf__ uint32_t*)tempBuf);
        Reg::LoadUnAlign(selReg, ureg, (__ubuf__ uint32_t*)tempBuf);
        Reg::MaskGenWithRegTensor<uint32_t, 0>(selMask, selReg);
    }
    if constexpr (isCounterMode) {
        maskReg = Reg::MoveMask<uint16_t>();
        Reg::StoreAlign<uint64_t, Reg::MaskDist::DIST_PACK>(tempBuf, maskReg);
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::SCALAR_LOAD>();
        sreg = static_cast<uint32_t>(tempBuf[0]);
        newRepeatTimes = CeilDivision(sreg, oneRepSize);
    } else {
        maskReg = Reg::MoveMask<T>();
    }
    for (uint16_t i = 0; i < newRepeatTimes; ++i) {
        if constexpr (isCounterMode) {
            maskReg = Reg::UpdateMask<T>(sreg);
        }
        Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            srcReg0, src0 + i * blockElm * repeatParams.src0RepStride,
            static_cast<uint32_t>(repeatParams.src0BlkStride), maskReg);
        Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            srcReg1, src1 + i * blockElm * repeatParams.src1RepStride,
            static_cast<uint32_t>(repeatParams.src1BlkStride), maskReg);
        Reg::Select(dstReg, srcReg0, srcReg1, selMask);
        Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            dst + i * blockElm * repeatParams.dstRepStride, dstReg, static_cast<uint32_t>(repeatParams.dstBlkStride),
            maskReg);
    }
}

template <typename T, bool isCounterMode>
__aicore__ inline void SelectWithoutMaskMode2ImplVF(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, __ubuf__ uint64_t* tempBuf, uint64_t selAddr, int32_t repeat,
    const BinaryRepeatParams& repeatParams)
{
    Reg::RegTensor<T> srcReg0, srcReg1, dstReg;
    Reg::MaskReg maskReg, selMask;
    Reg::RegTensor<uint8_t> selReg;
    Reg::UnalignReg ureg;
    uint16_t newRepeatTimes = repeat;
    constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T);
    constexpr uint32_t oneRepSize = GetVecLen() / sizeof(T);
    constexpr uint32_t blockElm = GetDataBlockSizeInBytes() / sizeof(T);
    uint32_t sreg;
    if constexpr (isCounterMode) {
        maskReg = Reg::MoveMask<uint16_t>();
        Reg::StoreAlign<uint64_t, Reg::MaskDist::DIST_PACK>(tempBuf, maskReg);
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::SCALAR_LOAD>();
        sreg = static_cast<uint32_t>(tempBuf[0]);
        newRepeatTimes = CeilDivision(sreg, oneRepSize);
    } else {
        maskReg = Reg::MoveMask<T>();
    }
    for (uint16_t i = 0; i < newRepeatTimes; ++i) {
        if constexpr (isCounterMode) {
            maskReg = Reg::UpdateMask<T>(sreg);
        }
        if constexpr (sizeof(T) == 2) {
            Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask, (__ubuf__ uint8_t*)selAddr + i * selOffset);
        } else if constexpr (sizeof(T) == 4) {
            Reg::LoadUnAlignPre(ureg, (__ubuf__ uint8_t*)selAddr + i * selOffset);
            Reg::LoadUnAlign(selReg, ureg, (__ubuf__ uint8_t*)selAddr + i * selOffset);
            Reg::MaskGenWithRegTensor<uint32_t, 0>(selMask, (Reg::RegTensor<uint32_t>&)selReg);
        }
        Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            srcReg0, src0 + i * blockElm * repeatParams.src0RepStride,
            static_cast<uint32_t>(repeatParams.src0BlkStride), maskReg);
        Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            srcReg1, src1 + i * blockElm * repeatParams.src1RepStride,
            static_cast<uint32_t>(repeatParams.src1BlkStride), maskReg);
        Reg::Select(dstReg, srcReg0, srcReg1, selMask);
        Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            dst + i * blockElm * repeatParams.dstRepStride, dstReg, static_cast<uint32_t>(repeatParams.dstBlkStride),
            maskReg);
    }
}

template <typename T, SELMODE selMode>
__aicore__ inline void SelectCal(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, int32_t repeat, const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, int16_t, uint16_t, int32_t, uint32_t, float, bfloat16_t>(),
        "current data type is not supported!");
    bool isCounterMode = Internal::IsCounterMode();
    __ubuf__ uint64_t* tempBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(GetRuntimeUBSize(), 2);
    event_t eventIdVToS = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_S));
    SetFlag<HardEvent::V_S>(eventIdVToS);
    WaitFlag<HardEvent::V_S>(eventIdVToS);
    if constexpr (selMode == SELMODE::VSEL_CMPMASK_SPR) {
        if constexpr (sizeof(T) == 2) {
            (*(__ubuf__ uint64_t*)((__ubuf__ uint64_t*)tempBuf)) = Internal::g_cmpMaskLow;
            (*(__ubuf__ uint64_t*)((__ubuf__ uint64_t*)tempBuf + 1)) = Internal::g_cmpMaskHigh;
        } else {
            (*(__ubuf__ uint64_t*)((__ubuf__ uint64_t*)tempBuf)) = Internal::g_cmpMaskLow;
        }
        event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
        SetFlag<HardEvent::S_V>(eventIdSToV);
        WaitFlag<HardEvent::S_V>(eventIdSToV);
        if (isCounterMode) {
            VF_CALL<SelectWithoutMaskMode0ImplVF<T, true>>(dst, src0, src1, tempBuf, repeat, repeatParams);
        } else {
            VF_CALL<SelectWithoutMaskMode0ImplVF<T, false>>(dst, src0, src1, tempBuf, repeat, repeatParams);
        }
    } else if constexpr (selMode == SELMODE::VSEL_TENSOR_TENSOR_MODE) {
        uint64_t selAddr = Internal::g_cmpMaskLow;
        event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
        SetFlag<HardEvent::S_V>(eventIdSToV);
        WaitFlag<HardEvent::S_V>(eventIdSToV);
        if (isCounterMode) {
            VF_CALL<SelectWithoutMaskMode2ImplVF<T, true>>(dst, src0, src1, tempBuf, selAddr, repeat, repeatParams);
        } else {
            VF_CALL<SelectWithoutMaskMode2ImplVF<T, false>>(dst, src0, src1, tempBuf, selAddr, repeat, repeatParams);
        }
    }
    AscendCUtils::FreeTemporaryBuffer<uint64_t>(tempBuf);
}

template <typename T, typename U, bool isCounterMode>
__aicore__ inline void SelectWithoutMaskMode1ImplVF(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, T scalar, __ubuf__ uint64_t* tempBuf, int32_t repeat,
    const BinaryRepeatParams& repeatParams)
{
    Reg::RegTensor<T> srcReg0, srcReg1, dstReg;
    Reg::MaskReg maskReg, selMask;
    Reg::RegTensor<uint8_t> selReg;
    Reg::UnalignReg ureg;
    uint16_t newRepeatTimes = repeat;
    uint32_t sreg;
    constexpr uint32_t oneRepSize = GetVecLen() / sizeof(T);
    constexpr uint32_t blockElm = GetDataBlockSizeInBytes() / sizeof(T);
    constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T);
    if constexpr (isCounterMode) {
        maskReg = Reg::MoveMask<uint16_t>();
        Reg::StoreAlign<uint64_t, Reg::MaskDist::DIST_PACK>(tempBuf, maskReg);
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::SCALAR_LOAD>();
        sreg = static_cast<uint32_t>(tempBuf[0]);
        newRepeatTimes = CeilDivision(sreg, oneRepSize);
    } else {
        maskReg = Reg::MoveMask<T>();
    }
    Reg::Duplicate(srcReg1, scalar);
    for (uint16_t i = 0; i < newRepeatTimes; ++i) {
        if constexpr (isCounterMode) {
            maskReg = Reg::UpdateMask<T>(sreg);
        }
        if constexpr (sizeof(T) == 2) {
            Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
        } else if constexpr (sizeof(T) == 4) {
            Reg::LoadUnAlignPre(ureg, (__ubuf__ uint8_t*)sel + i * selOffset);
            Reg::LoadUnAlign(selReg, ureg, (__ubuf__ uint8_t*)sel + i * selOffset);
            Reg::MaskGenWithRegTensor<uint32_t, 0>(selMask, (Reg::RegTensor<uint32_t>&)selReg);
        }
        Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            srcReg0, src0 + i * blockElm * repeatParams.src0RepStride,
            static_cast<uint32_t>(repeatParams.src0BlkStride), maskReg);
        Reg::Select(dstReg, srcReg0, srcReg1, selMask);
        Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            dst + i * blockElm * repeatParams.dstRepStride, dstReg, static_cast<uint32_t>(repeatParams.dstBlkStride),
            maskReg);
    }
}

template <typename T, typename U>
__aicore__ inline void SelectCal(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, int32_t repeat, const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, int16_t, uint16_t, int32_t, uint32_t, float, bfloat16_t>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t, uint16_t, uint32_t, uint64_t>(), "current data type is not supported!");
    bool isCounterMode = Internal::IsCounterMode();
    T scalar = *reinterpret_cast<T*>(&Internal::g_cmpMaskLow);
    event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
    SetFlag<HardEvent::S_V>(eventIdSToV);
    WaitFlag<HardEvent::S_V>(eventIdSToV);
    if (isCounterMode) {
        __ubuf__ uint64_t* tempBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(GetRuntimeUBSize(), 2);
        VF_CALL<SelectWithoutMaskMode1ImplVF<T, U, true>>(dst, sel, src0, scalar, tempBuf, repeat, repeatParams);
        AscendCUtils::FreeTemporaryBuffer<uint64_t>(tempBuf);
    } else {
        VF_CALL<SelectWithoutMaskMode1ImplVF<T, U, false>>(dst, sel, src0, scalar, nullptr, repeat, repeatParams);
    }
}

// ============ select mode: 0/2 ============
// ================Level2====================
template <typename T, typename U, bool isBitMap, bool isCounterMode>
__aicore__ inline void SelectMode0Level0(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    constexpr uint32_t blockElm = GetDataBlockSizeInBytes() / sizeof(T);
    constexpr uint16_t oneRepSize = GetVecLen() / sizeof(T);
    Reg::RegTensor<T> src0Reg, src1Reg, dstReg;
    Reg::MaskReg maskReg;
    uint32_t sreg;
    uint16_t newRepeatTimes = repeatTime;
    if constexpr (isCounterMode) {
        sreg = static_cast<uint32_t>(mask);
        newRepeatTimes = CeilDivision(sreg, oneRepSize);
    } else {
        if constexpr (isBitMap) {
            maskReg = Reg::MoveMask<T>();
        } else {
            sreg = static_cast<uint32_t>(mask);
            maskReg = Reg::UpdateMask<T>(sreg);
        }
    }
    Reg::MaskReg selMask;
    Reg::LoadAlign<U, Reg::MaskDist::DIST_US>(selMask, sel);
    if constexpr (sizeof(T) == 4) {
        Reg::MaskUnPack(selMask, selMask);
    }
    for (uint16_t i = 0; i < (uint16_t)newRepeatTimes; ++i) {
        if constexpr (isCounterMode) {
            maskReg = Reg::UpdateMask<T>(sreg);
        }
        Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            src0Reg, src0 + i * blockElm * repeatParams.src0RepStride, (uint32_t)repeatParams.src0BlkStride, maskReg);
        Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            src1Reg, src1 + i * blockElm * repeatParams.src1RepStride, (uint32_t)repeatParams.src1BlkStride, maskReg);
        Reg::Select(dstReg, src0Reg, src1Reg, selMask);
        Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            dst + i * blockElm * repeatParams.dstRepStride, dstReg, (uint32_t)repeatParams.dstBlkStride, maskReg);
    }
}

template <typename T, typename U, bool isBitMap, bool isCounterMode>
__aicore__ inline void SelectMode2Level0(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    constexpr uint32_t blockElm = GetDataBlockSizeInBytes() / sizeof(T);
    constexpr uint16_t oneRepSize = GetVecLen() / sizeof(T);
    uint16_t newRepeatTimes = repeatTime;
    uint32_t sreg;
    if constexpr (sizeof(T) == 4) {
        constexpr uint32_t unRollConstant = 2;
        constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T) * unRollConstant;
        Reg::RegTensor<T> src0Reg, src1Reg, src2Reg, src3Reg, dst0Reg, dst1Reg;
        Reg::MaskReg maskReg;
        if constexpr (isCounterMode) {
            sreg = static_cast<uint32_t>(mask);
            newRepeatTimes = CeilDivision(sreg, oneRepSize);
        } else {
            if constexpr (isBitMap) {
                maskReg = Reg::MoveMask<T>();
            } else {
                sreg = static_cast<uint32_t>(mask);
                maskReg = Reg::UpdateMask<T>(sreg);
            }
        }

        Reg::MaskReg selMask0, selMask1, tmpMask0;
        Reg::MaskReg tmpMask1 = Reg::CreateMask<uint8_t, Reg::MaskPattern::ALL>();
        uint16_t tail = newRepeatTimes % unRollConstant;
        newRepeatTimes = newRepeatTimes / unRollConstant;
        for (uint16_t i = 0; i < (uint16_t)newRepeatTimes; ++i) {
            if constexpr (isCounterMode) {
                maskReg = Reg::UpdateMask<T>(sreg);
            }
            Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(tmpMask0, (__ubuf__ uint8_t*)sel + i * selOffset);
            Reg::MaskInterleave<uint16_t>(selMask0, selMask1, tmpMask0, tmpMask1);
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src0Reg, src0 + i * unRollConstant * blockElm * repeatParams.src0RepStride,
                (uint32_t)repeatParams.src0BlkStride, maskReg);
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src1Reg, src1 + i * unRollConstant * blockElm * repeatParams.src1RepStride,
                (uint32_t)repeatParams.src1BlkStride, maskReg);
            Reg::Select(dst0Reg, src0Reg, src1Reg, selMask0);
            Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                dst + i * unRollConstant * blockElm * repeatParams.dstRepStride, dst0Reg,
                (uint32_t)repeatParams.dstBlkStride, maskReg);
            if constexpr (isCounterMode) {
                maskReg = Reg::UpdateMask<T>(sreg);
            }
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src2Reg, src0 + (i * unRollConstant + 1) * blockElm * repeatParams.src0RepStride,
                (uint32_t)repeatParams.src0BlkStride, maskReg);
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src3Reg, src1 + (i * unRollConstant + 1) * blockElm * repeatParams.src1RepStride,
                (uint32_t)repeatParams.src1BlkStride, maskReg);
            Reg::Select(dst1Reg, src2Reg, src3Reg, selMask1);
            Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                dst + (i * unRollConstant + 1) * blockElm * repeatParams.dstRepStride, dst1Reg,
                (uint32_t)repeatParams.dstBlkStride, maskReg);
        }
        Reg::RegTensor<T> src4Reg, src5Reg, dst2Reg;
        Reg::MaskReg selMask2;
        uint32_t offset0 = newRepeatTimes * unRollConstant * repeatParams.src0RepStride * blockElm;
        uint32_t offset1 = newRepeatTimes * unRollConstant * repeatParams.src1RepStride * blockElm;
        uint32_t offset2 = newRepeatTimes * unRollConstant * repeatParams.dstRepStride * blockElm;
        uint32_t newSelOffset = newRepeatTimes * selOffset;
        uint32_t tailSreg = sreg - unRollConstant * newRepeatTimes * oneRepSize;
        for (uint16_t i = 0; i < (uint16_t)tail; ++i) {
            if constexpr (isCounterMode) {
                maskReg = Reg::UpdateMask<T>(tailSreg);
            }
            Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask2, (__ubuf__ uint8_t*)sel + newSelOffset);
            Reg::MaskUnPack(selMask2, selMask2);
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src4Reg, src0 + offset0, (uint32_t)repeatParams.src0BlkStride, maskReg);
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src5Reg, src1 + offset1, (uint32_t)repeatParams.src1BlkStride, maskReg);
            Reg::Select(dst2Reg, src4Reg, src5Reg, selMask2);
            Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                dst + offset2, dst2Reg, (uint32_t)repeatParams.dstBlkStride, maskReg);
        }
    } else {
        constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T);
        Reg::RegTensor<T> src0Reg, src1Reg, dstReg;
        Reg::MaskReg maskReg;
        if constexpr (isCounterMode) {
            sreg = static_cast<uint32_t>(mask);
            newRepeatTimes = CeilDivision(sreg, oneRepSize);
        } else {
            if constexpr (isBitMap) {
                maskReg = Reg::MoveMask<T>();
            } else {
                sreg = static_cast<uint32_t>(mask);
                maskReg = Reg::UpdateMask<T>(sreg);
            }
        }
        Reg::MaskReg selMask;
        for (uint16_t i = 0; i < (uint16_t)newRepeatTimes; ++i) {
            if constexpr (isCounterMode) {
                maskReg = Reg::UpdateMask<T>(sreg);
            }
            Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src0Reg, src0 + i * blockElm * repeatParams.src0RepStride, (uint32_t)repeatParams.src0BlkStride,
                maskReg);
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src1Reg, src1 + i * blockElm * repeatParams.src1RepStride, (uint32_t)repeatParams.src1BlkStride,
                maskReg);
            Reg::Select(dstReg, src0Reg, src1Reg, selMask);
            Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                dst + i * blockElm * repeatParams.dstRepStride, dstReg, (uint32_t)repeatParams.dstBlkStride, maskReg);
        }
    }
}

template <typename T, typename U>
__aicore__ inline void VselImpl(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, __ubuf__ T* src1, SELMODE selMode, const uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, int16_t, uint16_t, int32_t, uint32_t, float, bfloat16_t>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t, uint16_t, uint32_t, uint64_t>(), "current data type is not supported!");
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        if (selMode == SELMODE::VSEL_CMPMASK_SPR) {
            VF_CALL<SelectMode0Level0<T, U, false, true>>(dst, sel, src0, src1, mask, repeatTime, repeatParams);
        } else if (selMode == SELMODE::VSEL_TENSOR_TENSOR_MODE) {
            VF_CALL<SelectMode2Level0<T, U, false, true>>(dst, sel, src0, src1, mask, repeatTime, repeatParams);
        }
    } else {
        if (selMode == SELMODE::VSEL_CMPMASK_SPR) {
            VF_CALL<SelectMode0Level0<T, U, false, false>>(dst, sel, src0, src1, mask, repeatTime, repeatParams);
        } else if (selMode == SELMODE::VSEL_TENSOR_TENSOR_MODE) {
            VF_CALL<SelectMode2Level0<T, U, false, false>>(dst, sel, src0, src1, mask, repeatTime, repeatParams);
        }
    }
}

template <typename T, typename U>
__aicore__ inline void VselImpl(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, __ubuf__ T* src1, SELMODE selMode, const uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, int16_t, uint16_t, int32_t, uint32_t, float, bfloat16_t>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t, uint16_t, uint32_t, uint64_t>(), "current data type is not supported!");
    SetVectorMask<T>(mask[1], mask[0]);
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        if (selMode == SELMODE::VSEL_CMPMASK_SPR) {
            VF_CALL<SelectMode0Level0<T, U, true, true>>(dst, sel, src0, src1, mask[0], repeatTime, repeatParams);
        } else if (selMode == SELMODE::VSEL_TENSOR_TENSOR_MODE) {
            VF_CALL<SelectMode2Level0<T, U, true, true>>(dst, sel, src0, src1, mask[0], repeatTime, repeatParams);
        }
    } else {
        if (selMode == SELMODE::VSEL_CMPMASK_SPR) {
            VF_CALL<SelectMode0Level0<T, U, true, false>>(dst, sel, src0, src1, mask[0], repeatTime, repeatParams);
        } else if (selMode == SELMODE::VSEL_TENSOR_TENSOR_MODE) {
            VF_CALL<SelectMode2Level0<T, U, true, false>>(dst, sel, src0, src1, mask[0], repeatTime, repeatParams);
        }
    }
}
// ============ select mode: 1 ============
// ================Level0====================

template <typename T, typename U, bool isBitMap, bool isCounterMode>
__aicore__ inline void SelectMode1Level0(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, T src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    constexpr uint32_t blockElm = GetDataBlockSizeInBytes() / sizeof(T);
    constexpr uint16_t oneRepSize = GetVecLen() / sizeof(T);
    uint16_t newRepeatTimes = repeatTime;
    uint32_t sreg;
    if constexpr (sizeof(T) == 2) {
        Reg::RegTensor<T> src0Reg, src1Reg, dstReg;
        Reg::Duplicate(src1Reg, (const T&)src1);
        Reg::MaskReg maskReg;
        constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T);
        if constexpr (isCounterMode) {
            sreg = static_cast<uint32_t>(mask);
            newRepeatTimes = CeilDivision(sreg, oneRepSize);
        } else {
            if constexpr (isBitMap) {
                maskReg = Reg::MoveMask<T>();
            } else {
                sreg = static_cast<uint32_t>(mask);
                maskReg = Reg::UpdateMask<T>(sreg);
            }
        }
        Reg::MaskReg selMask;
        for (uint16_t i = 0; i < (uint16_t)newRepeatTimes; ++i) {
            if constexpr (isCounterMode) {
                maskReg = Reg::UpdateMask<T>(sreg);
            }
            Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src0Reg, src0 + i * blockElm * repeatParams.src0RepStride, (uint32_t)repeatParams.src0BlkStride,
                maskReg);
            Reg::Select(dstReg, src0Reg, src1Reg, selMask);
            Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                dst + i * blockElm * repeatParams.dstRepStride, dstReg, (uint32_t)repeatParams.dstBlkStride, maskReg);
        }
    } else {
        Reg::RegTensor<T> scalarReg, src0Reg, src1Reg, dst0Reg, dst1Reg;
        Reg::MaskReg maskReg;
        constexpr uint32_t unRollConstant = 2;
        constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T) * unRollConstant;
        if constexpr (isCounterMode) {
            sreg = static_cast<uint32_t>(mask);
            newRepeatTimes = CeilDivision(sreg, oneRepSize);
        } else {
            if constexpr (isBitMap) {
                maskReg = Reg::MoveMask<T>();
            } else {
                sreg = static_cast<uint32_t>(mask);
                maskReg = Reg::UpdateMask<T>(sreg);
            }
        }
        Reg::MaskReg selMask0, selMask1, tmpMask0;
        Reg::MaskReg tmpMask1 = Reg::CreateMask<uint8_t, Reg::MaskPattern::ALL>();
        uint16_t tail = newRepeatTimes % unRollConstant;
        newRepeatTimes = newRepeatTimes / unRollConstant;
        Reg::Duplicate(scalarReg, (const T&)src1);
        for (uint16_t i = 0; i < (uint16_t)newRepeatTimes; ++i) {
            if constexpr (isCounterMode) {
                maskReg = Reg::UpdateMask<T>(sreg);
            }
            Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(tmpMask0, (__ubuf__ uint8_t*)sel + i * selOffset);
            Reg::MaskInterleave<uint16_t>(selMask0, selMask1, tmpMask0, tmpMask1);
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src0Reg, src0 + i * unRollConstant * blockElm * repeatParams.src0RepStride,
                (uint32_t)repeatParams.src0BlkStride, maskReg);
            Reg::Select(dst0Reg, src0Reg, scalarReg, selMask0);
            Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                dst + i * unRollConstant * blockElm * repeatParams.dstRepStride, dst0Reg,
                (uint32_t)repeatParams.dstBlkStride, maskReg);
            if constexpr (isCounterMode) {
                maskReg = Reg::UpdateMask<T>(sreg);
            }
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src1Reg, src0 + (i * unRollConstant + 1) * blockElm * repeatParams.src0RepStride,
                (uint32_t)repeatParams.src0BlkStride, maskReg);
            Reg::Select(dst1Reg, src1Reg, scalarReg, selMask1);
            Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                dst + (i * unRollConstant + 1) * blockElm * repeatParams.dstRepStride, dst1Reg,
                (uint32_t)repeatParams.dstBlkStride, maskReg);
        }
        Reg::RegTensor<T> src2Reg, dst2Reg;
        Reg::MaskReg selMask2;
        uint32_t offset0 = newRepeatTimes * unRollConstant * repeatParams.src0RepStride * blockElm;
        uint32_t offset1 = newRepeatTimes * unRollConstant * repeatParams.dstRepStride * blockElm;
        uint32_t newSelOffset = newRepeatTimes * selOffset;
        uint32_t tailSreg = sreg - unRollConstant * newRepeatTimes * oneRepSize;
        for (uint16_t i = 0; i < (uint16_t)tail; ++i) {
            if constexpr (isCounterMode) {
                maskReg = Reg::UpdateMask<T>(tailSreg);
            }
            Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask2, (__ubuf__ uint8_t*)sel + newSelOffset);
            Reg::MaskUnPack(selMask2, selMask2);
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src2Reg, src0 + offset0, (uint32_t)repeatParams.src0BlkStride, maskReg);
            Reg::Select(dst2Reg, src2Reg, scalarReg, selMask2);
            Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                dst + offset1, dst2Reg, (uint32_t)repeatParams.dstBlkStride, maskReg);
        }
    }
}

template <typename T, typename U>
__aicore__ inline void VselImpl(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, T src1, SELMODE selMode, const uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, int16_t, uint16_t, int32_t, uint32_t, float, bfloat16_t>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t, uint16_t, uint32_t, uint64_t>(), "current data type is not supported!");
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        VF_CALL<SelectMode1Level0<T, U, false, true>>(dst, sel, src0, src1, mask, repeatTime, repeatParams);
    } else {
        VF_CALL<SelectMode1Level0<T, U, false, false>>(dst, sel, src0, src1, mask, repeatTime, repeatParams);
    }
}

template <typename T, typename U>
__aicore__ inline void VselImpl(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, T src1, SELMODE selMode, const uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, int16_t, uint16_t, int32_t, uint32_t, float, bfloat16_t>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t, uint16_t, uint32_t, uint64_t>(), "current data type is not supported!");
    SetVectorMask<T>(mask[1], mask[0]);
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        VF_CALL<SelectMode1Level0<T, U, true, true>>(dst, sel, src0, src1, mask[0], repeatTime, repeatParams);
    } else {
        VF_CALL<SelectMode1Level0<T, U, true, false>>(dst, sel, src0, src1, mask[0], repeatTime, repeatParams);
    }
}
// ===============  Src0 Scalar =====================
template <typename T, typename U, bool isBitMap, bool isCounterMode>
__aicore__ inline void SelectSrc0ScalarMode1Level0(
    __ubuf__ T* dst, __ubuf__ U* sel, T src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    constexpr uint32_t blockElm = GetDataBlockSizeInBytes() / sizeof(T);
    uint32_t sreg;
    constexpr uint16_t oneRepSize = GetVecLen() / sizeof(T);
    uint16_t newRepeatTimes = repeatTime;
    Reg::MaskReg maskReg;
    if constexpr (isCounterMode) {
        sreg = static_cast<uint32_t>(mask);
        newRepeatTimes = CeilDivision(sreg, oneRepSize);
    } else {
        if constexpr (isBitMap) {
            maskReg = Reg::MoveMask<T>();
        } else {
            sreg = static_cast<uint32_t>(mask);
            maskReg = Reg::UpdateMask<T>(sreg);
        }
    }
    if constexpr (sizeof(T) == 2) {
        constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T);
        Reg::RegTensor<T> src0Reg, src1Reg, dstReg;
        Reg::Duplicate(src0Reg, (const T&)src0);
        Reg::MaskReg selMask;
        for (uint16_t i = 0; i < (uint16_t)newRepeatTimes; ++i) {
            if constexpr (isCounterMode) {
                maskReg = Reg::UpdateMask<T>(sreg);
            }
            Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src1Reg, src1 + i * blockElm * repeatParams.src1RepStride, (uint32_t)repeatParams.src1BlkStride,
                maskReg);
            Reg::Select(dstReg, src0Reg, src1Reg, selMask);
            Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                dst + i * blockElm * repeatParams.dstRepStride, dstReg, (uint32_t)repeatParams.dstBlkStride, maskReg);
        }
    } else {
        constexpr uint32_t unRollConstant = 2;
        constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T) * unRollConstant;
        Reg::RegTensor<T> scalarReg, src0Reg, src1Reg, dst0Reg, dst1Reg;
        Reg::MaskReg selMask0, selMask1, tmpMask0;
        Reg::MaskReg tmpMask1 = Reg::CreateMask<uint8_t, Reg::MaskPattern::ALL>();
        uint16_t tail = newRepeatTimes % unRollConstant;
        newRepeatTimes = newRepeatTimes / unRollConstant;
        Reg::Duplicate(scalarReg, (const T&)src0);
        for (uint16_t i = 0; i < (uint16_t)newRepeatTimes; ++i) {
            if constexpr (isCounterMode) {
                maskReg = Reg::UpdateMask<T>(sreg);
            }
            Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(tmpMask0, (__ubuf__ uint8_t*)sel + i * selOffset);
            Reg::MaskInterleave<uint16_t>(selMask0, selMask1, tmpMask0, tmpMask1);
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src0Reg, src1 + i * unRollConstant * blockElm * repeatParams.src1RepStride,
                (uint32_t)repeatParams.src1BlkStride, maskReg);
            Reg::Select(dst0Reg, scalarReg, src0Reg, selMask0);
            Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                dst + i * unRollConstant * blockElm * repeatParams.dstRepStride, dst0Reg,
                (uint32_t)repeatParams.dstBlkStride, maskReg);
            if constexpr (isCounterMode) {
                maskReg = Reg::UpdateMask<T>(sreg);
            }
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src1Reg, src1 + (i * unRollConstant + 1) * blockElm * repeatParams.src1RepStride,
                (uint32_t)repeatParams.src1BlkStride, maskReg);
            Reg::Select(dst1Reg, scalarReg, src1Reg, selMask1);
            Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                dst + (i * unRollConstant + 1) * blockElm * repeatParams.dstRepStride, dst1Reg,
                (uint32_t)repeatParams.dstBlkStride, maskReg);
        }
        Reg::RegTensor<T> src2Reg, dst2Reg;
        Reg::MaskReg selMask2;
        uint32_t offset0 = newRepeatTimes * unRollConstant * repeatParams.src1RepStride * blockElm;
        uint32_t offset1 = newRepeatTimes * unRollConstant * repeatParams.dstRepStride * blockElm;
        uint32_t newSelOffset = newRepeatTimes * selOffset;
        uint32_t tailSreg = sreg - unRollConstant * newRepeatTimes * oneRepSize;
        for (uint16_t i = 0; i < (uint16_t)tail; ++i) {
            if constexpr (isCounterMode) {
                maskReg = Reg::UpdateMask<T>(tailSreg);
            }
            Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask2, (__ubuf__ uint8_t*)sel + newSelOffset);
            Reg::MaskUnPack(selMask2, selMask2);
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src2Reg, src1 + offset0, (uint32_t)repeatParams.src1BlkStride, maskReg);
            Reg::Select(dst2Reg, scalarReg, src2Reg, selMask2);
            Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                dst + offset1, dst2Reg, (uint32_t)repeatParams.dstBlkStride, maskReg);
        }
    }
}

template <typename T, typename U>
__aicore__ inline void VselImpl(
    __ubuf__ T* dst, __ubuf__ U* sel, T src0, __ubuf__ T* src1, SELMODE selMode, const uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, int16_t, uint16_t, int32_t, uint32_t, float, bfloat16_t>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t, uint16_t, uint32_t, uint64_t>(), "current data type is not supported!");
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        VF_CALL<SelectSrc0ScalarMode1Level0<T, U, false, true>>(dst, sel, src0, src1, mask, repeatTime, repeatParams);
    } else {
        VF_CALL<SelectSrc0ScalarMode1Level0<T, U, false, false>>(dst, sel, src0, src1, mask, repeatTime, repeatParams);
    }
}

template <typename T, typename U>
__aicore__ inline void VselImpl(
    __ubuf__ T* dst, __ubuf__ U* sel, T src0, __ubuf__ T* src1, SELMODE selMode, const uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, int16_t, uint16_t, int32_t, uint32_t, float, bfloat16_t>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t, uint16_t, uint32_t, uint64_t>(), "current data type is not supported!");
    SetVectorMask<T>(mask[1], mask[0]);
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        VF_CALL<SelectSrc0ScalarMode1Level0<T, U, true, true>>(dst, sel, src0, src1, mask[0], repeatTime, repeatParams);
    } else {
        VF_CALL<SelectSrc0ScalarMode1Level0<T, U, true, false>>(
            dst, sel, src0, src1, mask[0], repeatTime, repeatParams);
    }
}

// both src0 / src1 are tensor
template <typename T, typename U, bool isBitMap, uint8_t scalarIdx, Reg::LoadDist pattern, bool isCounterMode>
__aicore__ inline void SelectBothTensorMode1Level0(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    constexpr uint32_t blockElm = GetDataBlockSizeInBytes() / sizeof(T);
    uint16_t newRepeatTimes = repeatTime;
    constexpr uint16_t oneRepSize = GetVecLen() / sizeof(T);
    uint32_t sreg;
    Reg::MaskReg maskReg, selMask;
    if constexpr (isCounterMode) {
        sreg = static_cast<uint32_t>(mask);
        newRepeatTimes = CeilDivision(sreg, oneRepSize);
    } else {
        if constexpr (isBitMap) {
            maskReg = Reg::MoveMask<T>();
        } else {
            sreg = static_cast<uint32_t>(mask);
            maskReg = Reg::UpdateMask<T>(sreg);
        }
    }
    if constexpr (scalarIdx == 0) {
        if constexpr (sizeof(T) == 2) {
            constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T);
            Reg::RegTensor<T> src0Reg, src1Reg, dstReg;
            Reg::LoadAlign<T, pattern>(src0Reg, src0);
            for (uint16_t i = 0; i < (uint16_t)newRepeatTimes; ++i) {
                if constexpr (isCounterMode) {
                    maskReg = Reg::UpdateMask<T>(sreg);
                }
                Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
                Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    src1Reg, src1 + i * blockElm * repeatParams.src1RepStride, (uint32_t)repeatParams.src1BlkStride,
                    maskReg);
                Reg::Select(dstReg, src0Reg, src1Reg, selMask);
                Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    dst + i * blockElm * repeatParams.dstRepStride, dstReg, (uint32_t)repeatParams.dstBlkStride,
                    maskReg);
            }
        } else {
            constexpr uint32_t unRollConstant = 2;
            constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T) * unRollConstant;
            Reg::RegTensor<T> scalarReg, src0Reg, src1Reg, dst0Reg, dst1Reg;
            Reg::MaskReg selMask0, selMask1, tmpMask0;
            Reg::MaskReg tmpMask1 = Reg::CreateMask<uint8_t, Reg::MaskPattern::ALL>();
            uint16_t tail = newRepeatTimes % unRollConstant;
            newRepeatTimes = newRepeatTimes / unRollConstant;
            Reg::LoadAlign<T, pattern>(scalarReg, src0);
            for (uint16_t i = 0; i < (uint16_t)newRepeatTimes; ++i) {
                if constexpr (isCounterMode) {
                    maskReg = Reg::UpdateMask<T>(sreg);
                }
                Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(tmpMask0, (__ubuf__ uint8_t*)sel + i * selOffset);
                Reg::MaskInterleave<uint16_t>(selMask0, selMask1, tmpMask0, tmpMask1);
                Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    src0Reg, src1 + i * unRollConstant * blockElm * repeatParams.src1RepStride,
                    (uint32_t)repeatParams.src1BlkStride, maskReg);
                Reg::Select(dst0Reg, scalarReg, src0Reg, selMask0);
                Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    dst + i * unRollConstant * blockElm * repeatParams.dstRepStride, dst0Reg,
                    (uint32_t)repeatParams.dstBlkStride, maskReg);
                if constexpr (isCounterMode) {
                    maskReg = Reg::UpdateMask<T>(sreg);
                }
                Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    src1Reg, src1 + (i * unRollConstant + 1) * blockElm * repeatParams.src1RepStride,
                    (uint32_t)repeatParams.src1BlkStride, maskReg);
                Reg::Select(dst1Reg, scalarReg, src1Reg, selMask1);
                Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    dst + (i * unRollConstant + 1) * blockElm * repeatParams.dstRepStride, dst1Reg,
                    (uint32_t)repeatParams.dstBlkStride, maskReg);
            }
            Reg::RegTensor<T> src2Reg, dst2Reg;
            Reg::MaskReg selMask2;
            uint32_t offset0 = newRepeatTimes * unRollConstant * repeatParams.src0RepStride * blockElm;
            uint32_t offset1 = newRepeatTimes * unRollConstant * repeatParams.dstRepStride * blockElm;
            uint32_t newSelOffset = newRepeatTimes * selOffset;
            uint32_t tailSreg = sreg - unRollConstant * newRepeatTimes * oneRepSize;
            for (uint16_t i = 0; i < (uint16_t)tail; ++i) {
                if constexpr (isCounterMode) {
                    maskReg = Reg::UpdateMask<T>(tailSreg);
                }
                Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask2, (__ubuf__ uint8_t*)sel + newSelOffset);
                Reg::MaskUnPack(selMask2, selMask2);
                Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    src2Reg, src1 + offset0, (uint32_t)repeatParams.src1BlkStride, maskReg);
                Reg::Select(dst2Reg, scalarReg, src2Reg, selMask2);
                Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    dst + offset1, dst2Reg, (uint32_t)repeatParams.dstBlkStride, maskReg);
            }
        }
    } else if constexpr (scalarIdx == 1) {
        if constexpr (sizeof(T) == 2) {
            constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T);
            Reg::RegTensor<T> src0Reg, src1Reg, dstReg;
            Reg::LoadAlign<T, pattern>(src1Reg, src1);
            for (uint16_t i = 0; i < (uint16_t)newRepeatTimes; ++i) {
                if constexpr (isCounterMode) {
                    maskReg = Reg::UpdateMask<T>(sreg);
                }
                Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
                Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    src0Reg, src0 + i * blockElm * repeatParams.src0RepStride, (uint32_t)repeatParams.src0BlkStride,
                    maskReg);
                Reg::Select(dstReg, src0Reg, src1Reg, selMask);
                Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    dst + i * blockElm * repeatParams.dstRepStride, dstReg, (uint32_t)repeatParams.dstBlkStride,
                    maskReg);
            }
        } else {
            constexpr uint32_t unRollConstant = 2;
            constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T) * unRollConstant;
            Reg::RegTensor<T> scalarReg, src0Reg, src1Reg, dst0Reg, dst1Reg;
            Reg::MaskReg selMask0, selMask1, tmpMask0;
            Reg::MaskReg tmpMask1 = Reg::CreateMask<uint8_t, Reg::MaskPattern::ALL>();
            uint16_t tail = newRepeatTimes % unRollConstant;
            newRepeatTimes = newRepeatTimes / unRollConstant;
            Reg::LoadAlign<T, pattern>(scalarReg, src1);
            for (uint16_t i = 0; i < (uint16_t)newRepeatTimes; ++i) {
                if constexpr (isCounterMode) {
                    maskReg = Reg::UpdateMask<T>(sreg);
                }
                Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(tmpMask0, (__ubuf__ uint8_t*)sel + i * selOffset);
                Reg::MaskInterleave<uint16_t>(selMask0, selMask1, tmpMask0, tmpMask1);
                Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    src0Reg, src0 + i * unRollConstant * blockElm * repeatParams.src0RepStride,
                    (uint32_t)repeatParams.src0BlkStride, maskReg);
                Reg::Select(dst0Reg, src0Reg, scalarReg, selMask0);
                Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    dst + i * unRollConstant * blockElm * repeatParams.dstRepStride, dst0Reg,
                    (uint32_t)repeatParams.dstBlkStride, maskReg);
                if constexpr (isCounterMode) {
                    maskReg = Reg::UpdateMask<T>(sreg);
                }
                Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    src1Reg, src0 + (i * unRollConstant + 1) * blockElm * repeatParams.src0RepStride,
                    (uint32_t)repeatParams.src0BlkStride, maskReg);
                Reg::Select(dst1Reg, src1Reg, scalarReg, selMask1);
                Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    dst + (i * unRollConstant + 1) * blockElm * repeatParams.dstRepStride, dst1Reg,
                    (uint32_t)repeatParams.dstBlkStride, maskReg);
            }
            Reg::RegTensor<T> src2Reg, dst2Reg;
            Reg::MaskReg selMask2;
            uint32_t offset0 = newRepeatTimes * unRollConstant * repeatParams.src0RepStride * blockElm;
            uint32_t offset1 = newRepeatTimes * unRollConstant * repeatParams.dstRepStride * blockElm;
            uint32_t newSelOffset = newRepeatTimes * selOffset;
            uint32_t tailSreg = sreg - unRollConstant * newRepeatTimes * oneRepSize;
            for (uint16_t i = 0; i < (uint16_t)tail; ++i) {
                if constexpr (isCounterMode) {
                    maskReg = Reg::UpdateMask<T>(tailSreg);
                }
                Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask2, (__ubuf__ uint8_t*)sel + newSelOffset);
                Reg::MaskUnPack(selMask2, selMask2);
                Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    src2Reg, src0 + offset0, (uint32_t)repeatParams.src0BlkStride, maskReg);
                Reg::Select(dst2Reg, src2Reg, scalarReg, selMask2);
                Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    dst + offset1, dst2Reg, (uint32_t)repeatParams.dstBlkStride, maskReg);
            }
        }
    }
}

template <typename T, typename U, uint8_t scalarIdx>
__aicore__ inline void VselImpl(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, __ubuf__ T* src1, SELMODE selMode, const uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, int16_t, uint16_t, int32_t, uint32_t, float, bfloat16_t>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t, uint16_t, uint32_t, uint64_t>(), "current data type is not supported!");
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        if constexpr (sizeof(T) == 2) {
            VF_CALL<SelectBothTensorMode1Level0<T, U, false, scalarIdx, Reg::LoadDist::DIST_BRC_B16, true>>(
                dst, sel, src0, src1, mask, repeatTime, repeatParams);
        } else if constexpr (sizeof(T) == 4) {
            VF_CALL<SelectBothTensorMode1Level0<T, U, false, scalarIdx, Reg::LoadDist::DIST_BRC_B32, true>>(
                dst, sel, src0, src1, mask, repeatTime, repeatParams);
        }
    } else {
        if constexpr (sizeof(T) == 2) {
            VF_CALL<SelectBothTensorMode1Level0<T, U, false, scalarIdx, Reg::LoadDist::DIST_BRC_B16, false>>(
                dst, sel, src0, src1, mask, repeatTime, repeatParams);
        } else if constexpr (sizeof(T) == 4) {
            VF_CALL<SelectBothTensorMode1Level0<T, U, false, scalarIdx, Reg::LoadDist::DIST_BRC_B32, false>>(
                dst, sel, src0, src1, mask, repeatTime, repeatParams);
        }
    }
}

template <typename T, typename U, uint8_t scalarIdx>
__aicore__ inline void VselImpl(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, __ubuf__ T* src1, SELMODE selMode, const uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, int16_t, uint16_t, int32_t, uint32_t, float, bfloat16_t>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t, uint16_t, uint32_t, uint64_t>(), "current data type is not supported!");
    SetVectorMask<T>(mask[1], mask[0]);
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        if constexpr (sizeof(T) == 2) {
            VF_CALL<SelectBothTensorMode1Level0<T, U, true, scalarIdx, Reg::LoadDist::DIST_BRC_B16, true>>(
                dst, sel, src0, src1, mask[0], repeatTime, repeatParams);
        } else if constexpr (sizeof(T) == 4) {
            VF_CALL<SelectBothTensorMode1Level0<T, U, true, scalarIdx, Reg::LoadDist::DIST_BRC_B32, true>>(
                dst, sel, src0, src1, mask[0], repeatTime, repeatParams);
        }
    } else {
        if constexpr (sizeof(T) == 2) {
            VF_CALL<SelectBothTensorMode1Level0<T, U, true, scalarIdx, Reg::LoadDist::DIST_BRC_B16, false>>(
                dst, sel, src0, src1, mask[0], repeatTime, repeatParams);
        } else if constexpr (sizeof(T) == 4) {
            VF_CALL<SelectBothTensorMode1Level0<T, U, true, scalarIdx, Reg::LoadDist::DIST_BRC_B32, false>>(
                dst, sel, src0, src1, mask[0], repeatTime, repeatParams);
        }
    }
}

// ============ select mode: 0/2 ============
// =============== LEVEL2 ===================
template <typename T, typename U>
__aicore__ inline void SelectMode0Level2(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, __ubuf__ T* src1, uint32_t calCount)
{
    constexpr uint32_t repeatElm = GetVecLen() / sizeof(T);
    uint16_t repeatTime = CeilDivision(calCount, repeatElm);
    uint32_t sreg = static_cast<uint32_t>(calCount);
    if constexpr (sizeof(T) == 8) {
        Reg::RegTensor<T> src0Reg, src1Reg, dstReg;
        Reg::MaskReg selMask, maskReg;
        Reg::RegTensor<uint32_t> selReg;
        Reg::UnalignReg ureg;
        Reg::LoadUnAlignPre(ureg, (__ubuf__ uint32_t*)sel);
        Reg::LoadUnAlign(selReg, ureg, (__ubuf__ uint32_t*)sel);
        Reg::MaskGenWithRegTensor<uint32_t, 0>(selMask, selReg);
        Reg::MaskUnPack(selMask, selMask);
        for (uint16_t i = 0; i < (uint16_t)repeatTime; ++i) {
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign<T>(src0Reg, src0 + i * repeatElm);
            Reg::LoadAlign<T>(src1Reg, src1 + i * repeatElm);
            Reg::Select(dstReg, src0Reg, src1Reg, selMask);
            Reg::StoreAlign<T>(dst + i * repeatElm, dstReg, maskReg);
        }
    } else {
        Reg::RegTensor<T> src0Reg, src1Reg, dstReg;
        Reg::MaskReg maskReg, selMask;
        if constexpr (sizeof(T) == 1) {
            Reg::LoadAlign<U>(selMask, sel);
        } else {
            Reg::LoadAlign<U, Reg::MaskDist::DIST_US>(selMask, sel);
            if constexpr (sizeof(T) == 4) {
                Reg::MaskUnPack(selMask, selMask);
            }
        }
        for (uint16_t i = 0; i < (uint16_t)repeatTime; ++i) {
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign<T>(src0Reg, src0 + i * repeatElm);
            Reg::LoadAlign<T>(src1Reg, src1 + i * repeatElm);
            Reg::Select(dstReg, src0Reg, src1Reg, selMask);
            Reg::StoreAlign<T>(dst + i * repeatElm, dstReg, maskReg);
        }
    }
}

template <typename T, typename U>
__aicore__ inline void SelectMode2Level2(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, __ubuf__ T* src1, uint32_t calCount)
{
    constexpr uint32_t repeatElm = GetVecLen() / sizeof(T);
    uint32_t repeatTime = CeilDivision(calCount, repeatElm);
    uint32_t sreg = static_cast<uint32_t>(calCount);
    if constexpr (sizeof(T) == 8) {
        constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T);
        Reg::RegTensor<T> src0Reg, src1Reg, dstReg;
        Reg::MaskReg selMask, maskReg;
        Reg::RegTensor<uint8_t> selReg;
        Reg::UnalignReg ureg;
        for (uint16_t i = 0; i < (uint16_t)repeatTime; ++i) {
            Reg::LoadUnAlignPre(ureg, (__ubuf__ uint8_t*)sel + i * selOffset);
            Reg::LoadUnAlign(selReg, ureg, (__ubuf__ uint8_t*)sel + i * selOffset);
            Reg::MaskGenWithRegTensor<uint32_t, 0>(selMask, (Reg::RegTensor<uint32_t>&)selReg);
            Reg::MaskUnPack(selMask, selMask);
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign<T>(src0Reg, src0 + i * repeatElm);
            Reg::LoadAlign<T>(src1Reg, src1 + i * repeatElm);
            Reg::Select(dstReg, src0Reg, src1Reg, selMask);
            Reg::StoreAlign<T>(dst + i * repeatElm, dstReg, maskReg);
        }
    } else if constexpr (sizeof(T) == 4) {
        constexpr uint32_t unRollConstant = 2;
        constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T) * unRollConstant;
        Reg::RegTensor<T> src0Reg, src1Reg, src2Reg, src3Reg, dst0Reg, dst1Reg;
        Reg::MaskReg maskReg;
        Reg::MaskReg selMask0, selMask1, tmpMask0;
        Reg::MaskReg tmpMask1 = Reg::CreateMask<uint8_t, Reg::MaskPattern::ALL>();
        uint16_t tail = repeatTime % unRollConstant;
        uint16_t newRepeatTimes = repeatTime / unRollConstant;
        for (uint16_t i = 0; i < (uint16_t)newRepeatTimes; ++i) {
            Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(tmpMask0, (__ubuf__ uint8_t*)sel + i * selOffset);
            Reg::MaskInterleave<uint16_t>(selMask0, selMask1, tmpMask0, tmpMask1);
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign<T>(src0Reg, src0 + i * unRollConstant * repeatElm);
            Reg::LoadAlign<T>(src1Reg, src1 + i * unRollConstant * repeatElm);
            Reg::Select(dst0Reg, src0Reg, src1Reg, selMask0);
            Reg::StoreAlign<T>(dst + i * unRollConstant * repeatElm, dst0Reg, maskReg);
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign<T>(src2Reg, src0 + (i * unRollConstant + 1) * repeatElm);
            Reg::LoadAlign<T>(src3Reg, src1 + (i * unRollConstant + 1) * repeatElm);
            Reg::Select(dst1Reg, src2Reg, src3Reg, selMask1);
            Reg::StoreAlign<T>(dst + (i * unRollConstant + 1) * repeatElm, dst1Reg, maskReg);
        }
        Reg::RegTensor<T> src4Reg, src5Reg, dst2Reg;
        Reg::MaskReg selMask2;
        uint32_t offset = newRepeatTimes * unRollConstant * repeatElm;
        uint32_t newSelOffset = newRepeatTimes * selOffset;
        for (uint16_t i = 0; i < (uint16_t)tail; ++i) {
            Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask2, (__ubuf__ uint8_t*)sel + newSelOffset);
            Reg::MaskUnPack(selMask2, selMask2);
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign<T>(src4Reg, src0 + offset);
            Reg::LoadAlign<T>(src5Reg, src1 + offset);
            Reg::Select(dst2Reg, src4Reg, src5Reg, selMask2);
            Reg::StoreAlign<T>(dst + offset, dst2Reg, maskReg);
        }
    } else {
        constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T);
        Reg::RegTensor<T> src0Reg, src1Reg, dstReg;
        uint32_t sreg = (uint32_t)calCount;
        Reg::MaskReg maskReg, selMask;
        for (uint16_t i = 0; i < (uint16_t)repeatTime; ++i) {
            if constexpr (sizeof(T) == 2) {
                Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
            } else {
                Reg::LoadAlign<uint8_t>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
            }
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign<T>(src0Reg, src0 + i * repeatElm);
            Reg::LoadAlign<T>(src1Reg, src1 + i * repeatElm);
            Reg::Select(dstReg, src0Reg, src1Reg, selMask);
            Reg::StoreAlign<T>(dst + i * repeatElm, dstReg, maskReg);
        }
    }
}

template <typename T, typename U>
__aicore__ inline void VselImpl(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, __ubuf__ T* src1, SELMODE selMode, uint32_t calCount)
{
    static_assert(
        SupportType<
            T, uint8_t, int8_t, bfloat16_t, half, int16_t, uint16_t, int32_t, uint32_t, float, uint64_t, int64_t,
            complex32, complex64>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t, uint16_t, uint32_t, uint64_t>(), "current data type is not supported!");
    if (selMode == SELMODE::VSEL_CMPMASK_SPR) {
        VF_CALL<SelectMode0Level2<T, U>>(dst, sel, src0, src1, calCount);
    } else if (selMode == SELMODE::VSEL_TENSOR_TENSOR_MODE) {
        VF_CALL<SelectMode2Level2<T, U>>(dst, sel, src0, src1, calCount);
    }
}

// ============ select mode: 1 ============
// =============== LEVEL2 ===================
template <typename T, typename U>
__aicore__ inline void SelectMode1Level2(__ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, T src1, uint32_t calCount)
{
    constexpr uint32_t repeatElm = GetVecLen() / sizeof(T);
    uint32_t repeatTime = CeilDivision(calCount, repeatElm);
    uint32_t sreg = static_cast<uint32_t>(calCount);
    if constexpr (sizeof(T) == 8) {
        constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T);
        Reg::RegTensor<T> src0Reg, src1Reg, dstReg;
        Reg::MaskReg selMask, maskReg;
        Reg::RegTensor<uint8_t> selReg;
        Reg::Duplicate(src1Reg, (const T&)src1);
        Reg::UnalignReg ureg;
        for (uint16_t i = 0; i < (uint16_t)repeatTime; ++i) {
            Reg::LoadUnAlignPre(ureg, (__ubuf__ uint8_t*)sel + i * selOffset);
            Reg::LoadUnAlign(selReg, ureg, (__ubuf__ uint8_t*)sel + i * selOffset);
            Reg::MaskGenWithRegTensor<uint32_t, 0>(selMask, (Reg::RegTensor<uint32_t>&)selReg);
            Reg::MaskUnPack(selMask, selMask);
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign<T>(src0Reg, src0 + i * repeatElm);
            Reg::Select(dstReg, src0Reg, src1Reg, selMask);
            Reg::StoreAlign<T>(dst + i * repeatElm, dstReg, maskReg);
        }
    } else if constexpr (sizeof(T) == 4) {
        constexpr uint32_t unRollConstant = 2;
        constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T) * unRollConstant;
        Reg::RegTensor<T> src0Reg, src1Reg, scalarReg, dst0Reg, dst1Reg;
        Reg::MaskReg maskReg;
        Reg::MaskReg selMask0, selMask1, tmpMask0;
        Reg::MaskReg tmpMask1 = Reg::CreateMask<uint8_t, Reg::MaskPattern::ALL>();
        uint16_t tail = repeatTime % unRollConstant;
        uint16_t newRepeatTimes = repeatTime / unRollConstant;
        Reg::Duplicate(scalarReg, (const T&)src1);
        for (uint16_t i = 0; i < (uint16_t)newRepeatTimes; ++i) {
            Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(tmpMask0, (__ubuf__ uint8_t*)sel + i * selOffset);
            Reg::MaskInterleave<uint16_t>(selMask0, selMask1, tmpMask0, tmpMask1);
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign<T>(src0Reg, src0 + i * unRollConstant * repeatElm);
            Reg::Select(dst0Reg, src0Reg, scalarReg, selMask0);
            Reg::StoreAlign<T>(dst + i * unRollConstant * repeatElm, dst0Reg, maskReg);
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign<T>(src1Reg, src0 + (i * unRollConstant + 1) * repeatElm);
            Reg::Select(dst1Reg, src1Reg, scalarReg, selMask1);
            Reg::StoreAlign<T>(dst + (i * unRollConstant + 1) * repeatElm, dst1Reg, maskReg);
        }
        Reg::RegTensor<T> src2Reg, dst2Reg;
        Reg::MaskReg selMask2;
        uint32_t offset = newRepeatTimes * unRollConstant * repeatElm;
        uint32_t newSelOffset = newRepeatTimes * selOffset;
        for (uint16_t i = 0; i < (uint16_t)tail; ++i) {
            Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask2, (__ubuf__ uint8_t*)sel + newSelOffset);
            Reg::MaskUnPack(selMask2, selMask2);
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign<T>(src2Reg, src0 + offset);
            Reg::Select(dst2Reg, src2Reg, scalarReg, selMask2);
            Reg::StoreAlign<T>(dst + offset, dst2Reg, maskReg);
        }
    } else {
        constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T);
        Reg::RegTensor<T> src0Reg, src1Reg, dstReg;
        uint32_t sreg = (uint32_t)calCount;
        Reg::MaskReg maskReg, selMask;
        Reg::Duplicate(src1Reg, (const T&)src1);
        for (uint16_t i = 0; i < (uint16_t)repeatTime; ++i) {
            if constexpr (sizeof(T) == 2) {
                Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
            } else {
                Reg::LoadAlign<uint8_t>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
            }
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign<T>(src0Reg, src0 + i * repeatElm);
            Reg::Select(dstReg, src0Reg, src1Reg, selMask);
            Reg::StoreAlign<T>(dst + i * repeatElm, dstReg, maskReg);
        }
    }
}

template <typename T, typename U>
__aicore__ inline void VselImpl(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, T src1, SELMODE selMode, uint32_t calCount)
{
    static_assert(
        SupportType<
            T, uint8_t, int8_t, bfloat16_t, half, int16_t, uint16_t, int32_t, uint32_t, float, uint64_t, int64_t,
            complex32, complex64>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t, uint16_t, uint32_t, uint64_t>(), "current data type is not supported!");
    VF_CALL<SelectMode1Level2<T, U>>(dst, sel, src0, src1, calCount);
}
// Src0Scalar
template <typename T, typename U>
__aicore__ inline void SelectSrc0ScalarMode1Level2(
    __ubuf__ T* dst, __ubuf__ U* sel, T src0, __ubuf__ T* src1, uint32_t calCount)
{
    constexpr uint32_t repeatElm = GetVecLen() / sizeof(T);
    uint32_t repeatTime = CeilDivision(calCount, repeatElm);
    uint32_t sreg = static_cast<uint32_t>(calCount);
    if constexpr (sizeof(T) == 8) {
        constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T);
        Reg::RegTensor<T> src0Reg, src1Reg, dstReg;
        Reg::MaskReg selMask, maskReg;
        Reg::RegTensor<uint8_t> selReg;
        Reg::Duplicate(src0Reg, (const T&)src0);
        Reg::UnalignReg ureg;
        for (uint16_t i = 0; i < (uint16_t)repeatTime; ++i) {
            Reg::LoadUnAlignPre(ureg, (__ubuf__ uint8_t*)sel + i * selOffset);
            Reg::LoadUnAlign(selReg, ureg, (__ubuf__ uint8_t*)sel + i * selOffset);
            Reg::MaskGenWithRegTensor<uint32_t, 0>(selMask, (Reg::RegTensor<uint32_t>&)selReg);
            Reg::MaskUnPack(selMask, selMask);
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign<T>(src1Reg, src1 + i * repeatElm);
            Reg::Select(dstReg, src0Reg, src1Reg, selMask);
            Reg::StoreAlign<T>(dst + i * repeatElm, dstReg, maskReg);
        }
    } else if constexpr (sizeof(T) == 4) {
        constexpr uint32_t unRollConstant = 2;
        constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T) * unRollConstant;
        Reg::RegTensor<T> src0Reg, src1Reg, scalarReg, dst0Reg, dst1Reg;
        Reg::MaskReg maskReg;
        Reg::MaskReg selMask0, selMask1, tmpMask0;
        Reg::MaskReg tmpMask1 = Reg::CreateMask<uint8_t, Reg::MaskPattern::ALL>();
        uint16_t tail = repeatTime % unRollConstant;
        uint16_t newRepeatTimes = repeatTime / unRollConstant;
        Reg::Duplicate(scalarReg, (const T&)src0);
        for (uint16_t i = 0; i < (uint16_t)newRepeatTimes; ++i) {
            Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(tmpMask0, (__ubuf__ uint8_t*)sel + i * selOffset);
            Reg::MaskInterleave<uint16_t>(selMask0, selMask1, tmpMask0, tmpMask1);
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign<T>(src0Reg, src1 + i * unRollConstant * repeatElm);
            Reg::Select(dst0Reg, scalarReg, src0Reg, selMask0);
            Reg::StoreAlign<T>(dst + i * unRollConstant * repeatElm, dst0Reg, maskReg);
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign<T>(src1Reg, src1 + (i * unRollConstant + 1) * repeatElm);
            Reg::Select(dst1Reg, scalarReg, src1Reg, selMask1);
            Reg::StoreAlign<T>(dst + (i * unRollConstant + 1) * repeatElm, dst1Reg, maskReg);
        }
        Reg::RegTensor<T> src2Reg, dst2Reg;
        Reg::MaskReg selMask2;
        uint32_t offset = newRepeatTimes * unRollConstant * repeatElm;
        uint32_t newSelOffset = newRepeatTimes * selOffset;
        for (uint16_t i = 0; i < (uint16_t)tail; ++i) {
            Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask2, (__ubuf__ uint8_t*)sel + newSelOffset);
            Reg::MaskUnPack(selMask2, selMask2);
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign<T>(src2Reg, src1 + offset);
            Reg::Select(dst2Reg, scalarReg, src2Reg, selMask2);
            Reg::StoreAlign<T>(dst + offset, dst2Reg, maskReg);
        }
    } else {
        constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T);
        Reg::RegTensor<T> src0Reg, src1Reg, dstReg;
        Reg::MaskReg maskReg, selMask;
        Reg::Duplicate(src0Reg, (const T&)src0);
        for (uint16_t i = 0; i < (uint16_t)repeatTime; ++i) {
            if constexpr (sizeof(T) == 2) {
                Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
            } else {
                Reg::LoadAlign<uint8_t>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
            }
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign<T>(src1Reg, src1 + i * repeatElm);
            Reg::Select(dstReg, src0Reg, src1Reg, selMask);
            Reg::StoreAlign<T>(dst + i * repeatElm, dstReg, maskReg);
        }
    }
}

template <typename T, typename U>
__aicore__ inline void VselImpl(
    __ubuf__ T* dst, __ubuf__ U* sel, T src0, __ubuf__ T* src1, SELMODE selMode, uint32_t calCount)
{
    static_assert(
        SupportType<
            T, uint8_t, int8_t, bfloat16_t, half, int16_t, uint16_t, int32_t, uint32_t, float, uint64_t, int64_t,
            complex32, complex64>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t, uint16_t, uint32_t, uint64_t>(), "current data type is not supported!");
    VF_CALL<SelectSrc0ScalarMode1Level2<T, U>>(dst, sel, src0, src1, calCount);
}
// both src0 / src1 Tensor
template <typename T, typename U, uint8_t scalarIdx, Reg::LoadDist pattern = Reg::LoadDist::DIST_BRC_B32>
__aicore__ inline void SelectBothTensorMode1Level2(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, __ubuf__ T* src1, uint32_t calCount)
{
    constexpr uint32_t repeatElm = GetVecLen() / sizeof(T);
    uint32_t repeatTime = CeilDivision(calCount, repeatElm);
    uint32_t sreg = static_cast<uint32_t>(calCount);
    if constexpr (scalarIdx == 0) {
        if constexpr (sizeof(T) == 8) {
            constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T);
            Reg::RegTensor<T> src0Reg, src1Reg, dstReg, tmpReg;
            Reg::MaskReg selMask, maskReg;
            Reg::RegTensor<uint8_t> selReg;
            Reg::UnalignReg ureg, uregDup;
            Reg::LoadUnAlignPre(uregDup, (__ubuf__ T*)src0);
            Reg::LoadUnAlign(tmpReg, uregDup, (__ubuf__ T*)src0);
            Reg::MaskReg maskFull = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
            Reg::Duplicate(src0Reg, tmpReg, maskFull);
            for (uint16_t i = 0; i < (uint16_t)repeatTime; ++i) {
                Reg::LoadUnAlignPre(ureg, (__ubuf__ uint8_t*)sel + i * selOffset);
                Reg::LoadUnAlign(selReg, ureg, (__ubuf__ uint8_t*)sel + i * selOffset);
                Reg::MaskGenWithRegTensor<uint32_t, 0>(selMask, (Reg::RegTensor<uint32_t>&)selReg);
                Reg::MaskUnPack(selMask, selMask);
                maskReg = Reg::UpdateMask<T>(sreg);
                Reg::LoadAlign<T>(src1Reg, src1 + i * repeatElm);
                Reg::Select(dstReg, src0Reg, src1Reg, selMask);
                Reg::StoreAlign<T>(dst + i * repeatElm, dstReg, maskReg);
            }
        } else if constexpr (sizeof(T) == 4) {
            constexpr uint32_t unRollConstant = 2;
            constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T) * unRollConstant;
            Reg::RegTensor<T> src0Reg, src1Reg, scalarReg, dst0Reg, dst1Reg;
            Reg::MaskReg maskReg;
            Reg::MaskReg selMask0, selMask1, tmpMask0;
            Reg::MaskReg tmpMask1 = Reg::CreateMask<uint8_t, Reg::MaskPattern::ALL>();
            uint16_t tail = repeatTime % unRollConstant;
            uint16_t newRepeatTimes = repeatTime / unRollConstant;
            Reg::LoadAlign<T, pattern>(scalarReg, src0);
            for (uint16_t i = 0; i < (uint16_t)newRepeatTimes; ++i) {
                Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(tmpMask0, (__ubuf__ uint8_t*)sel + i * selOffset);
                Reg::MaskInterleave<uint16_t>(selMask0, selMask1, tmpMask0, tmpMask1);
                maskReg = Reg::UpdateMask<T>(sreg);
                Reg::LoadAlign<T>(src0Reg, src1 + i * unRollConstant * repeatElm);
                Reg::Select(dst0Reg, scalarReg, src0Reg, selMask0);
                Reg::StoreAlign<T>(dst + i * unRollConstant * repeatElm, dst0Reg, maskReg);
                maskReg = Reg::UpdateMask<T>(sreg);
                Reg::LoadAlign<T>(src1Reg, src1 + (i * unRollConstant + 1) * repeatElm);
                Reg::Select(dst1Reg, scalarReg, src1Reg, selMask1);
                Reg::StoreAlign<T>(dst + (i * unRollConstant + 1) * repeatElm, dst1Reg, maskReg);
            }
            Reg::RegTensor<T> src2Reg, dst2Reg;
            Reg::MaskReg selMask2;
            uint32_t offset = newRepeatTimes * unRollConstant * repeatElm;
            uint32_t newSelOffset = newRepeatTimes * selOffset;
            for (uint16_t i = 0; i < (uint16_t)tail; ++i) {
                Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask2, (__ubuf__ uint8_t*)sel + newSelOffset);
                Reg::MaskUnPack(selMask2, selMask2);
                maskReg = Reg::UpdateMask<T>(sreg);
                Reg::LoadAlign<T>(src2Reg, src1 + offset);
                Reg::Select(dst2Reg, scalarReg, src2Reg, selMask2);
                Reg::StoreAlign<T>(dst + offset, dst2Reg, maskReg);
            }
        } else {
            constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T);
            Reg::RegTensor<T> src0Reg, src1Reg, dstReg;
            Reg::MaskReg maskReg, selMask;
            Reg::LoadAlign<T, pattern>(src0Reg, src0);
            for (uint16_t i = 0; i < (uint16_t)repeatTime; ++i) {
                if constexpr (sizeof(T) == 2) {
                    Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
                } else {
                    Reg::LoadAlign<uint8_t>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
                }
                maskReg = Reg::UpdateMask<T>(sreg);
                Reg::LoadAlign<T>(src1Reg, src1 + i * repeatElm);
                Reg::Select(dstReg, src0Reg, src1Reg, selMask);
                Reg::StoreAlign<T>(dst + i * repeatElm, dstReg, maskReg);
            }
        }
    } else if constexpr (scalarIdx == 1) {
        if constexpr (sizeof(T) == 8) {
            constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T);
            Reg::RegTensor<T> src0Reg, src1Reg, dstReg, tmpReg;
            Reg::MaskReg selMask, maskReg;
            Reg::RegTensor<uint8_t> selReg;
            Reg::UnalignReg ureg, uregDup;
            Reg::LoadUnAlignPre(uregDup, (__ubuf__ T*)src1);
            Reg::LoadUnAlign(tmpReg, uregDup, (__ubuf__ T*)src1);
            Reg::MaskReg maskFull = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
            Reg::Duplicate(src1Reg, tmpReg, maskFull);
            for (uint16_t i = 0; i < (uint16_t)repeatTime; ++i) {
                Reg::LoadUnAlignPre(ureg, (__ubuf__ uint8_t*)sel + i * selOffset);
                Reg::LoadUnAlign(selReg, ureg, (__ubuf__ uint8_t*)sel + i * selOffset);
                Reg::MaskGenWithRegTensor<uint32_t, 0>(selMask, (Reg::RegTensor<uint32_t>&)selReg);
                Reg::MaskUnPack(selMask, selMask);
                maskReg = Reg::UpdateMask<T>(sreg);
                Reg::LoadAlign<T>(src0Reg, src0 + i * repeatElm);
                Reg::Select(dstReg, src0Reg, src1Reg, selMask);
                Reg::StoreAlign<T>(dst + i * repeatElm, dstReg, maskReg);
            }
        } else if constexpr (sizeof(T) == 4) {
            constexpr uint32_t unRollConstant = 2;
            constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T) * unRollConstant;
            Reg::RegTensor<T> src0Reg, src1Reg, scalarReg, dst0Reg, dst1Reg;
            Reg::MaskReg maskReg;
            Reg::MaskReg selMask0, selMask1, tmpMask0;
            Reg::MaskReg tmpMask1 = Reg::CreateMask<uint8_t, Reg::MaskPattern::ALL>();
            uint16_t tail = repeatTime % unRollConstant;
            uint16_t newRepeatTimes = repeatTime / unRollConstant;
            Reg::LoadAlign<T, pattern>(scalarReg, src1);
            for (uint16_t i = 0; i < (uint16_t)newRepeatTimes; ++i) {
                Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(tmpMask0, (__ubuf__ uint8_t*)sel + i * selOffset);
                Reg::MaskInterleave<uint16_t>(selMask0, selMask1, tmpMask0, tmpMask1);
                maskReg = Reg::UpdateMask<T>(sreg);
                Reg::LoadAlign<T>(src0Reg, src0 + i * unRollConstant * repeatElm);
                Reg::Select(dst0Reg, src0Reg, scalarReg, selMask0);
                Reg::StoreAlign<T>(dst + i * unRollConstant * repeatElm, dst0Reg, maskReg);
                maskReg = Reg::UpdateMask<T>(sreg);
                Reg::LoadAlign<T>(src1Reg, src0 + (i * unRollConstant + 1) * repeatElm);
                Reg::Select(dst1Reg, src1Reg, scalarReg, selMask1);
                Reg::StoreAlign<T>(dst + (i * unRollConstant + 1) * repeatElm, dst1Reg, maskReg);
            }
            Reg::RegTensor<T> src2Reg, dst2Reg;
            Reg::MaskReg selMask2;
            uint32_t offset = newRepeatTimes * unRollConstant * repeatElm;
            uint32_t newSelOffset = newRepeatTimes * selOffset;
            for (uint16_t i = 0; i < (uint16_t)tail; ++i) {
                Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask2, (__ubuf__ uint8_t*)sel + newSelOffset);
                Reg::MaskUnPack(selMask2, selMask2);
                maskReg = Reg::UpdateMask<T>(sreg);
                Reg::LoadAlign<T>(src2Reg, src0 + offset);
                Reg::Select(dst2Reg, src2Reg, scalarReg, selMask2);
                Reg::StoreAlign<T>(dst + offset, dst2Reg, maskReg);
            }
        } else {
            constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T);
            Reg::RegTensor<T> src0Reg, src1Reg, dstReg;
            Reg::MaskReg maskReg, selMask;
            Reg::LoadAlign<T, pattern>(src1Reg, src1);
            for (uint16_t i = 0; i < (uint16_t)repeatTime; ++i) {
                if constexpr (sizeof(T) == 2) {
                    Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
                } else {
                    Reg::LoadAlign<uint8_t>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
                }
                maskReg = Reg::UpdateMask<T>(sreg);
                Reg::LoadAlign<T>(src0Reg, src0 + i * repeatElm);
                Reg::Select(dstReg, src0Reg, src1Reg, selMask);
                Reg::StoreAlign<T>(dst + i * repeatElm, dstReg, maskReg);
            }
        }
    }
}

template <typename T, typename U, uint8_t scalarIdx>
__aicore__ inline void VselImpl(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, __ubuf__ T* src1, SELMODE selMode, uint32_t calCount)
{
    static_assert(
        SupportType<
            T, uint8_t, int8_t, bfloat16_t, half, int16_t, uint16_t, int32_t, uint32_t, float, uint64_t, int64_t,
            complex32, complex64>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t, uint16_t, uint32_t, uint64_t>(), "current data type is not supported!");
    if constexpr (sizeof(T) == 1) {
        VF_CALL<SelectBothTensorMode1Level2<T, U, scalarIdx, Reg::LoadDist::DIST_BRC_B8>>(
            dst, sel, src0, src1, calCount);
    } else if constexpr (sizeof(T) == 2) {
        VF_CALL<SelectBothTensorMode1Level2<T, U, scalarIdx, Reg::LoadDist::DIST_BRC_B16>>(
            dst, sel, src0, src1, calCount);
    } else if constexpr (sizeof(T) == 4) {
        VF_CALL<SelectBothTensorMode1Level2<T, U, scalarIdx, Reg::LoadDist::DIST_BRC_B32>>(
            dst, sel, src0, src1, calCount);
    } else {
        VF_CALL<SelectBothTensorMode1Level2<T, U, scalarIdx>>(dst, sel, src0, src1, calCount);
    }
}

template <typename T>
__aicore__ inline void GetCmpMaskImpl(__ubuf__ T* dst)
{
    pipe_barrier(PIPE_ALL);
    (*(__ubuf__ uint64_t*)((__ubuf__ uint64_t*)dst)) = Internal::g_cmpMaskLow;
    (*(__ubuf__ uint64_t*)((__ubuf__ uint64_t*)dst + 1)) = Internal::g_cmpMaskHigh;
    pipe_barrier(PIPE_ALL);
}

template <typename T>
__aicore__ inline void SetCmpMaskImpl(__ubuf__ T* src)
{
    pipe_barrier(PIPE_ALL);
    Internal::g_cmpMaskLow = reinterpret_cast<uint64_t>(((__ubuf__ uint64_t*)src)[0]);
    Internal::g_cmpMaskHigh = reinterpret_cast<uint64_t>(((__ubuf__ uint64_t*)src)[1]);
    pipe_barrier(PIPE_ALL);
}

} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_CMPSEL_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_CMPSEL_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_CMPSEL_IMPL_H__
#endif
