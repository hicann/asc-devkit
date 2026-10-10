/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file kernel_operator_vec_cmp_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_9201/kernel_operator_vec_cmp_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_CMP_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_CMP_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_CMP_IMPL_H

#include "../kernel_utils.h"
#include "../../../include/basic_api/kernel_tpipe.h"
#include "kernel_operator_vec_template_impl.h"
#include "../../../include/basic_api/reg_compute/kernel_reg_compute_intf.h"

namespace AscendC {
namespace CmpInternal {
constexpr uint32_t maskBitToByte = 8;
}
/* ***************************************************************************************
 * ************************************** Compare ****************************************
 * ************************************************************************************** */
// Compare written to CMPMASK
template <typename T, bool isSetMask, CMPMODE cmpMode>
__simd_vf__ inline void CompareWithoutDstCounterModeImplVF(
    __ubuf__ T* src0, __ubuf__ T* src1, __ubuf__ uint64_t* tempBuf, const uint64_t mask,
    const BinaryRepeatParams repeatParams)
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
    Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
        srcReg0, src0, static_cast<uint32_t>(repeatParams.src0BlkStride), maskReg);
    Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
        srcReg1, src1, static_cast<uint32_t>(repeatParams.src1BlkStride), maskReg);
    Reg::Compare<T, cmpMode>(dstReg, srcReg0, srcReg1, maskReg);
    Reg::StoreUnAlign((__ubuf__ T*&)tempBuf, dstReg, uReg);
    Reg::StoreUnAlignPost<uint64_t, Reg::PostLiteral::POST_MODE_NORMAL>(tempBuf, uReg, 0);
}

template <typename T, bool isSetMask, bool isBitMap, CMPMODE cmpMode>
__simd_vf__ inline void CompareWithoutDstNormalModeImplVF(
    __ubuf__ T* src0, __ubuf__ T* src1, __ubuf__ uint64_t* tempBuf, const uint64_t mask,
    const BinaryRepeatParams repeatParams)
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
    Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
        srcReg0, src0, static_cast<uint32_t>(repeatParams.src0BlkStride), maskReg);
    Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
        srcReg1, src1, static_cast<uint32_t>(repeatParams.src1BlkStride), maskReg);
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
                CompareWithoutDstCounterModeImplVF<T, isSetMask, CMPMODE::LT>(
                    src0, src1, tempBuf, mask[0], repeatParams);
                break;
            }
            case CMPMODE::GT: {
                CompareWithoutDstCounterModeImplVF<T, isSetMask, CMPMODE::GT>(
                    src0, src1, tempBuf, mask[0], repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                CompareWithoutDstCounterModeImplVF<T, isSetMask, CMPMODE::EQ>(
                    src0, src1, tempBuf, mask[0], repeatParams);
                break;
            }
            case CMPMODE::LE: {
                CompareWithoutDstCounterModeImplVF<T, isSetMask, CMPMODE::LE>(
                    src0, src1, tempBuf, mask[0], repeatParams);
                break;
            }
            case CMPMODE::GE: {
                CompareWithoutDstCounterModeImplVF<T, isSetMask, CMPMODE::GE>(
                    src0, src1, tempBuf, mask[0], repeatParams);
                break;
            }
            case CMPMODE::NE: {
                CompareWithoutDstCounterModeImplVF<T, isSetMask, CMPMODE::NE>(
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
                CompareWithoutDstNormalModeImplVF<T, isSetMask, true, CMPMODE::LT>(
                    src0, src1, tempBuf, 0, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                CompareWithoutDstNormalModeImplVF<T, isSetMask, true, CMPMODE::GT>(
                    src0, src1, tempBuf, 0, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                CompareWithoutDstNormalModeImplVF<T, isSetMask, true, CMPMODE::EQ>(
                    src0, src1, tempBuf, 0, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                CompareWithoutDstNormalModeImplVF<T, isSetMask, true, CMPMODE::LE>(
                    src0, src1, tempBuf, 0, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                CompareWithoutDstNormalModeImplVF<T, isSetMask, true, CMPMODE::GE>(
                    src0, src1, tempBuf, 0, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                CompareWithoutDstNormalModeImplVF<T, isSetMask, true, CMPMODE::NE>(
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
        Internal::g_cmpMaskLow = (uint64_t)tempBuf[0];
        Internal::g_cmpMaskHigh = (uint64_t)tempBuf[1];
    } else if constexpr (sizeof(T) == 4) {
        Internal::g_cmpMaskLow = (uint64_t)tempBuf[0];
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
                CompareWithoutDstCounterModeImplVF<T, isSetMask, CMPMODE::LT>(src0, src1, tempBuf, mask, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                CompareWithoutDstCounterModeImplVF<T, isSetMask, CMPMODE::GT>(src0, src1, tempBuf, mask, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                CompareWithoutDstCounterModeImplVF<T, isSetMask, CMPMODE::EQ>(src0, src1, tempBuf, mask, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                CompareWithoutDstCounterModeImplVF<T, isSetMask, CMPMODE::LE>(src0, src1, tempBuf, mask, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                CompareWithoutDstCounterModeImplVF<T, isSetMask, CMPMODE::GE>(src0, src1, tempBuf, mask, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                CompareWithoutDstCounterModeImplVF<T, isSetMask, CMPMODE::NE>(src0, src1, tempBuf, mask, repeatParams);
                break;
            }
            default:
                break;
        }
    } else {
        switch (cmpMode) {
            case CMPMODE::LT: {
                CompareWithoutDstNormalModeImplVF<T, isSetMask, false, CMPMODE::LT>(
                    src0, src1, tempBuf, mask, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                CompareWithoutDstNormalModeImplVF<T, isSetMask, false, CMPMODE::GT>(
                    src0, src1, tempBuf, mask, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                CompareWithoutDstNormalModeImplVF<T, isSetMask, false, CMPMODE::EQ>(
                    src0, src1, tempBuf, mask, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                CompareWithoutDstNormalModeImplVF<T, isSetMask, false, CMPMODE::LE>(
                    src0, src1, tempBuf, mask, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                CompareWithoutDstNormalModeImplVF<T, isSetMask, false, CMPMODE::GE>(
                    src0, src1, tempBuf, mask, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                CompareWithoutDstNormalModeImplVF<T, isSetMask, false, CMPMODE::NE>(
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
        Internal::g_cmpMaskLow = (uint64_t)tempBuf[0];
        Internal::g_cmpMaskHigh = (uint64_t)tempBuf[1];
    } else if constexpr (sizeof(T) == 4) {
        Internal::g_cmpMaskLow = (uint64_t)tempBuf[0];
        Internal::g_cmpMaskHigh = static_cast<uint64_t>(0);
    }
    AscendCUtils::FreeTemporaryBuffer<uint64_t>(tempBuf);
}

// Compare::Level 0 - bit mode / Continuous mode support b16/b32
template <typename T, typename U, CMPMODE cmpMode>
__simd_vf__ inline void CompareLevel0CounterMode(
    __ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const BinaryRepeatParams repeatParams)
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
            src0Reg, src0, static_cast<uint32_t>(repeatParams.src0BlkStride),
            static_cast<uint32_t>(repeatParams.src0RepStride), maskReg);
        Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
            src1Reg, src1, static_cast<uint32_t>(repeatParams.src1BlkStride),
            static_cast<uint32_t>(repeatParams.src1RepStride), maskReg);
        Reg::Compare<T, cmpMode>(dstReg, src0Reg, src1Reg, maskReg);
        Reg::StoreUnAlign((__ubuf__ T*&)dst, dstReg, uReg);
    }
    Reg::StoreUnAlignPost<U, Reg::PostLiteral::POST_MODE_NORMAL>(dst, uReg, 0);
}

// Compare::Level 0 - bit mode / Continuous mode support b8/b16/b32
template <typename T, typename U, CMPMODE cmpMode, bool isBitMapMode>
__simd_vf__ inline void CompareLevel0NormalMode(
    __ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams repeatParams)
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
            src0Reg, src0, static_cast<uint32_t>(repeatParams.src0BlkStride),
            static_cast<uint32_t>(repeatParams.src0RepStride), maskReg);
        Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
            src1Reg, src1, static_cast<uint32_t>(repeatParams.src1BlkStride),
            static_cast<uint32_t>(repeatParams.src1RepStride), maskReg);
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
                CompareLevel0CounterMode<T, U, CMPMODE::LT>(dst, src0, src1, mask[0], repeatParams);
                break;
            }
            case CMPMODE::GT: {
                CompareLevel0CounterMode<T, U, CMPMODE::GT>(dst, src0, src1, mask[0], repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                CompareLevel0CounterMode<T, U, CMPMODE::EQ>(dst, src0, src1, mask[0], repeatParams);
                break;
            }
            case CMPMODE::LE: {
                CompareLevel0CounterMode<T, U, CMPMODE::LE>(dst, src0, src1, mask[0], repeatParams);
                break;
            }
            case CMPMODE::GE: {
                CompareLevel0CounterMode<T, U, CMPMODE::GE>(dst, src0, src1, mask[0], repeatParams);
                break;
            }
            case CMPMODE::NE: {
                CompareLevel0CounterMode<T, U, CMPMODE::NE>(dst, src0, src1, mask[0], repeatParams);
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
                CompareLevel0NormalMode<T, U, CMPMODE::LT, true>(dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                CompareLevel0NormalMode<T, U, CMPMODE::GT, true>(dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                CompareLevel0NormalMode<T, U, CMPMODE::EQ, true>(dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                CompareLevel0NormalMode<T, U, CMPMODE::LE, true>(dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                CompareLevel0NormalMode<T, U, CMPMODE::GE, true>(dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                CompareLevel0NormalMode<T, U, CMPMODE::NE, true>(dst, src0, src1, 0, repeatTime, repeatParams);
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
                CompareLevel0CounterMode<T, U, CMPMODE::LT>(dst, src0, src1, mask, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                CompareLevel0CounterMode<T, U, CMPMODE::GT>(dst, src0, src1, mask, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                CompareLevel0CounterMode<T, U, CMPMODE::EQ>(dst, src0, src1, mask, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                CompareLevel0CounterMode<T, U, CMPMODE::LE>(dst, src0, src1, mask, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                CompareLevel0CounterMode<T, U, CMPMODE::GE>(dst, src0, src1, mask, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                CompareLevel0CounterMode<T, U, CMPMODE::NE>(dst, src0, src1, mask, repeatParams);
                break;
            }
            default:
                break;
        }
    } else {
        switch (cmpMode) {
            case CMPMODE::LT: {
                CompareLevel0NormalMode<T, U, CMPMODE::LT, false>(dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                CompareLevel0NormalMode<T, U, CMPMODE::GT, false>(dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                CompareLevel0NormalMode<T, U, CMPMODE::EQ, false>(dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                CompareLevel0NormalMode<T, U, CMPMODE::LE, false>(dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                CompareLevel0NormalMode<T, U, CMPMODE::GE, false>(dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                CompareLevel0NormalMode<T, U, CMPMODE::NE, false>(dst, src0, src1, mask, repeatTime, repeatParams);
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
__simd_vf__ inline void CompareScalarLevel0CounterMode(
    __ubuf__ U* dst, __ubuf__ T* src0, const T src1, const uint64_t mask, __ubuf__ uint64_t* tempBuf,
    const UnaryRepeatParams repeatParams)
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
            src0Reg, src0, static_cast<uint32_t>(repeatParams.srcBlkStride),
            static_cast<uint32_t>(repeatParams.srcRepStride), maskReg);
        Reg::CompareScalar<T, cmpMode>(dstReg, src0Reg, src1, maskReg);
        Reg::StoreUnAlign((__ubuf__ T*&)dst, dstReg, uReg);
    }
    Reg::StoreUnAlignPost<U, Reg::PostLiteral::POST_MODE_NORMAL>(dst, uReg, 0);
}

template <typename T, typename U, CMPMODE cmpMode, bool isBitMapMode, bool isSetMask>
__simd_vf__ inline void CompareScalarLevel0NormalMode(
    __ubuf__ U* dst, __ubuf__ T* src0, const T src1, const uint64_t mask, uint8_t repeatTime,
    const UnaryRepeatParams repeatParams)
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
            src0Reg, src0, static_cast<uint32_t>(repeatParams.srcBlkStride),
            static_cast<uint32_t>(repeatParams.srcRepStride), maskReg);
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
                CompareScalarLevel0CounterMode<T, U, CMPMODE::LT, isSetMask>(
                    dst, src0, src1, mask[0], tempBuf, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                CompareScalarLevel0CounterMode<T, U, CMPMODE::GT, isSetMask>(
                    dst, src0, src1, mask[0], tempBuf, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                CompareScalarLevel0CounterMode<T, U, CMPMODE::EQ, isSetMask>(
                    dst, src0, src1, mask[0], tempBuf, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                CompareScalarLevel0CounterMode<T, U, CMPMODE::LE, isSetMask>(
                    dst, src0, src1, mask[0], tempBuf, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                CompareScalarLevel0CounterMode<T, U, CMPMODE::GE, isSetMask>(
                    dst, src0, src1, mask[0], tempBuf, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                CompareScalarLevel0CounterMode<T, U, CMPMODE::NE, isSetMask>(
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
                CompareScalarLevel0NormalMode<T, U, CMPMODE::LT, true, isSetMask>(
                    dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                CompareScalarLevel0NormalMode<T, U, CMPMODE::GT, true, isSetMask>(
                    dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                CompareScalarLevel0NormalMode<T, U, CMPMODE::EQ, true, isSetMask>(
                    dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                CompareScalarLevel0NormalMode<T, U, CMPMODE::LE, true, isSetMask>(
                    dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                CompareScalarLevel0NormalMode<T, U, CMPMODE::GE, true, isSetMask>(
                    dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                CompareScalarLevel0NormalMode<T, U, CMPMODE::NE, true, isSetMask>(
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
                CompareScalarLevel0CounterMode<T, U, CMPMODE::LT, isSetMask>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                CompareScalarLevel0CounterMode<T, U, CMPMODE::GT, isSetMask>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                CompareScalarLevel0CounterMode<T, U, CMPMODE::EQ, isSetMask>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                CompareScalarLevel0CounterMode<T, U, CMPMODE::LE, isSetMask>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                CompareScalarLevel0CounterMode<T, U, CMPMODE::GE, isSetMask>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                CompareScalarLevel0CounterMode<T, U, CMPMODE::NE, isSetMask>(
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
                CompareScalarLevel0NormalMode<T, U, CMPMODE::LT, false, isSetMask>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                CompareScalarLevel0NormalMode<T, U, CMPMODE::GT, false, isSetMask>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                CompareScalarLevel0NormalMode<T, U, CMPMODE::EQ, false, isSetMask>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                CompareScalarLevel0NormalMode<T, U, CMPMODE::LE, false, isSetMask>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                CompareScalarLevel0NormalMode<T, U, CMPMODE::GE, false, isSetMask>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                CompareScalarLevel0NormalMode<T, U, CMPMODE::NE, false, isSetMask>(
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
__simd_vf__ inline void CompareScalarLevel2(__ubuf__ U* dst, __ubuf__ T* src0, const T src1, const uint32_t calCount)
{
    Reg::MaskReg dstReg, maskReg;
    Reg::UnalignReg uReg;
    uint32_t repeatElm = GetVecLen() / sizeof(T);
    uint16_t repeatTime = CeilDivision(calCount, repeatElm);
    uint32_t sreg = static_cast<uint32_t>(calCount);
    if constexpr (sizeof(T) == 8) {
        repeatElm = repeatElm * 2;
        repeatTime = CeilDivision(calCount, repeatElm);
        if constexpr (Std::is_same_v<T, double>) {
            Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> src1Reg;
            Reg::RegTensor<double, Reg::RegTraitNumTwo> src0Reg;
            Reg::Duplicate(src1Reg, GetScalarBitcodeValue<double, uint64_t>(src1));
            for (uint16_t i = 0; i < repeatTime; ++i) {
                maskReg = Reg::UpdateMask<T, Reg::RegTraitNumTwo>(sreg);
                Reg::LoadAlign(src0Reg, src0 + i * repeatElm);
                CompareEqualDouble<uint64_t>(
                    dstReg, (Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo>&)src0Reg, src1Reg, maskReg);
                Reg::StoreUnAlign((__ubuf__ uint32_t*&)dst, dstReg, uReg);
            }
        } else {
            Reg::RegTensor<T, Reg::RegTraitNumTwo> src0Reg;
            for (uint16_t i = 0; i < repeatTime; ++i) {
                maskReg = Reg::UpdateMask<T, Reg::RegTraitNumTwo>(sreg);
                Reg::LoadAlign(src0Reg, src0 + i * repeatElm);
                Reg::CompareScalar<T, cmpMode>(dstReg, src0Reg, src1, maskReg);
                Reg::StoreUnAlign((__ubuf__ uint32_t*&)dst, dstReg, uReg);
            }
        }
        Reg::StoreUnAlignPost<U, Reg::PostLiteral::POST_MODE_NORMAL>(dst, uReg, 0);
    } else {
        Reg::RegTensor<T> src0Reg;
        constexpr uint32_t offset = GetVecLen() / sizeof(T) / CmpInternal::maskBitToByte;
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
            T, uint8_t, int8_t, bfloat16_t, half, int16_t, uint16_t, int32_t, uint32_t, float, uint64_t, int64_t,
            double>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t>(), "current data type is not supported!");
    switch (cmpMode) {
        case CMPMODE::LT: {
            CompareScalarLevel2<T, U, CMPMODE::LT>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::GT: {
            CompareScalarLevel2<T, U, CMPMODE::GT>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::EQ: {
            CompareScalarLevel2<T, U, CMPMODE::EQ>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::LE: {
            CompareScalarLevel2<T, U, CMPMODE::LE>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::GE: {
            CompareScalarLevel2<T, U, CMPMODE::GE>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::NE: {
            CompareScalarLevel2<T, U, CMPMODE::NE>(dst, src0, src1, calCount);
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
__simd_vf__ inline void CompareSrc0ScalarLevel0CounterMode(
    __ubuf__ U* dst, const T src0, __ubuf__ T* src1, const uint64_t mask, __ubuf__ uint64_t* tempBuf,
    const UnaryRepeatParams repeatParams)
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
            src1Reg, src1, static_cast<uint32_t>(repeatParams.srcBlkStride),
            static_cast<uint32_t>(repeatParams.srcRepStride), maskReg);
        Reg::Compare<T, cmpMode>(dstReg, src0Reg, src1Reg, maskReg);
        Reg::StoreUnAlign((__ubuf__ T*&)dst, dstReg, uReg);
    }
    Reg::StoreUnAlignPost<U, Reg::PostLiteral::POST_MODE_NORMAL>(dst, uReg, 0);
}

template <typename T, typename U, CMPMODE cmpMode, bool isBitMapMode, bool isSetMask>
__simd_vf__ inline void CompareSrc0ScalarLevel0NormalMode(
    __ubuf__ U* dst, const T src0, __ubuf__ T* src1, const uint64_t mask, uint8_t repeatTime,
    const UnaryRepeatParams repeatParams)
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
            src1Reg, src1, static_cast<uint32_t>(repeatParams.srcBlkStride),
            static_cast<uint32_t>(repeatParams.srcRepStride), maskReg);
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
                CompareSrc0ScalarLevel0CounterMode<T, U, CMPMODE::LT, isSetMask>(
                    dst, src0, src1, mask[0], tempBuf, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                CompareSrc0ScalarLevel0CounterMode<T, U, CMPMODE::GT, isSetMask>(
                    dst, src0, src1, mask[0], tempBuf, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                CompareSrc0ScalarLevel0CounterMode<T, U, CMPMODE::EQ, isSetMask>(
                    dst, src0, src1, mask[0], tempBuf, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                CompareSrc0ScalarLevel0CounterMode<T, U, CMPMODE::LE, isSetMask>(
                    dst, src0, src1, mask[0], tempBuf, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                CompareSrc0ScalarLevel0CounterMode<T, U, CMPMODE::GE, isSetMask>(
                    dst, src0, src1, mask[0], tempBuf, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                CompareSrc0ScalarLevel0CounterMode<T, U, CMPMODE::NE, isSetMask>(
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
                CompareSrc0ScalarLevel0NormalMode<T, U, CMPMODE::LT, true, isSetMask>(
                    dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                CompareSrc0ScalarLevel0NormalMode<T, U, CMPMODE::GT, true, isSetMask>(
                    dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                CompareSrc0ScalarLevel0NormalMode<T, U, CMPMODE::EQ, true, isSetMask>(
                    dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                CompareSrc0ScalarLevel0NormalMode<T, U, CMPMODE::LE, true, isSetMask>(
                    dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                CompareSrc0ScalarLevel0NormalMode<T, U, CMPMODE::GE, true, isSetMask>(
                    dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                CompareSrc0ScalarLevel0NormalMode<T, U, CMPMODE::NE, true, isSetMask>(
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
                CompareSrc0ScalarLevel0CounterMode<T, U, CMPMODE::LT, isSetMask>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                CompareSrc0ScalarLevel0CounterMode<T, U, CMPMODE::GT, isSetMask>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                CompareSrc0ScalarLevel0CounterMode<T, U, CMPMODE::EQ, isSetMask>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                CompareSrc0ScalarLevel0CounterMode<T, U, CMPMODE::LE, isSetMask>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                CompareSrc0ScalarLevel0CounterMode<T, U, CMPMODE::GE, isSetMask>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                CompareSrc0ScalarLevel0CounterMode<T, U, CMPMODE::NE, isSetMask>(
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
                CompareSrc0ScalarLevel0NormalMode<T, U, CMPMODE::LT, false, isSetMask>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                CompareSrc0ScalarLevel0NormalMode<T, U, CMPMODE::GT, false, isSetMask>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                CompareSrc0ScalarLevel0NormalMode<T, U, CMPMODE::EQ, false, isSetMask>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                CompareSrc0ScalarLevel0NormalMode<T, U, CMPMODE::LE, false, isSetMask>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                CompareSrc0ScalarLevel0NormalMode<T, U, CMPMODE::GE, false, isSetMask>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                CompareSrc0ScalarLevel0NormalMode<T, U, CMPMODE::NE, false, isSetMask>(
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
__simd_vf__ inline void CompareSrc0ScalarLevel2(
    __ubuf__ U* dst, const T src0, __ubuf__ T* src1, const uint32_t calCount)
{
    uint32_t repeatElm = GetVecLen() / sizeof(T);
    uint16_t repeatTime = CeilDivision(calCount, repeatElm);
    uint32_t sreg = static_cast<uint32_t>(calCount);
    Reg::MaskReg dstReg, maskReg;
    Reg::UnalignReg uReg;
    if constexpr (sizeof(T) == 8) {
        repeatElm = repeatElm * 2;
        repeatTime = CeilDivision(calCount, repeatElm);
        if constexpr (Std::is_same_v<T, double>) {
            Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> src0Reg;
            Reg::RegTensor<double, Reg::RegTraitNumTwo> src1Reg;
            Reg::Duplicate(src0Reg, GetScalarBitcodeValue<double, uint64_t>(src0));
            for (uint16_t i = 0; i < repeatTime; ++i) {
                maskReg = Reg::UpdateMask<T, Reg::RegTraitNumTwo>(sreg);
                Reg::LoadAlign(src1Reg, src1 + i * repeatElm);
                CompareEqualDouble<uint64_t>(
                    dstReg, (Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo>&)src1Reg, src0Reg, maskReg);
                Reg::StoreUnAlign((__ubuf__ uint32_t*&)dst, dstReg, uReg);
            }
        } else {
            Reg::RegTensor<T, Reg::RegTraitNumTwo> src0Reg, src1Reg;
            Reg::Duplicate(src0Reg, src0);
            for (uint16_t i = 0; i < repeatTime; ++i) {
                maskReg = Reg::UpdateMask<T, Reg::RegTraitNumTwo>(sreg);
                Reg::LoadAlign(src1Reg, src1 + i * repeatElm);
                Reg::Compare<T, cmpMode>(dstReg, src0Reg, src1Reg, maskReg);
                Reg::StoreUnAlign((__ubuf__ uint32_t*&)dst, dstReg, uReg);
            }
        }
        Reg::StoreUnAlignPost<U, Reg::PostLiteral::POST_MODE_NORMAL>(dst, uReg, 0);
    } else {
        Reg::RegTensor<T> src0Reg, src1Reg;
        constexpr uint32_t offset = GetVecLen() / sizeof(T) / CmpInternal::maskBitToByte;
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
            T, uint8_t, int8_t, bfloat16_t, half, int16_t, uint16_t, int32_t, uint32_t, float, uint64_t, int64_t,
            double>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t>(), "current data type is not supported!");
    switch (cmpMode) {
        case CMPMODE::LT: {
            CompareSrc0ScalarLevel2<T, U, CMPMODE::LT>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::GT: {
            CompareSrc0ScalarLevel2<T, U, CMPMODE::GT>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::EQ: {
            CompareSrc0ScalarLevel2<T, U, CMPMODE::EQ>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::LE: {
            CompareSrc0ScalarLevel2<T, U, CMPMODE::LE>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::GE: {
            CompareSrc0ScalarLevel2<T, U, CMPMODE::GE>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::NE: {
            CompareSrc0ScalarLevel2<T, U, CMPMODE::NE>(dst, src0, src1, calCount);
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
__simd_vf__ inline void CompareScalarBothTensorLevel2(
    __ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint32_t calCount)
{
    constexpr uint32_t repeatElm = GetVecLen() / sizeof(T);
    uint16_t repeatTime = CeilDivision(calCount, repeatElm);
    uint32_t sreg = static_cast<uint32_t>(calCount);
    Reg::MaskReg dstReg, maskReg;
    Reg::UnalignReg uReg;
    Reg::RegTensor<T> src0Reg, src1Reg;
    constexpr uint32_t offset = GetVecLen() / sizeof(T) / CmpInternal::maskBitToByte;
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
            CompareScalarBothTensorLevel2<T, U, CMPMODE::LT, scalarIdx, pattern>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::GT: {
            CompareScalarBothTensorLevel2<T, U, CMPMODE::GT, scalarIdx, pattern>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::EQ: {
            CompareScalarBothTensorLevel2<T, U, CMPMODE::EQ, scalarIdx, pattern>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::LE: {
            CompareScalarBothTensorLevel2<T, U, CMPMODE::LE, scalarIdx, pattern>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::GE: {
            CompareScalarBothTensorLevel2<T, U, CMPMODE::GE, scalarIdx, pattern>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::NE: {
            CompareScalarBothTensorLevel2<T, U, CMPMODE::NE, scalarIdx, pattern>(dst, src0, src1, calCount);
            break;
        }
        default:
            break;
    }
}

template <
    typename T, typename U, CMPMODE cmpMode, uint8_t scalarIdx,
    typename Std::enable_if<!Std::is_same<PrimT<T>, double>::value, bool>::type = true>
__simd_vf__ inline void CompareScalarLevel2B64(
    __ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint32_t calCount)
{
    constexpr uint32_t repeatElm = GetVecLen() / sizeof(T) * 2;
    uint16_t repeatTime = CeilDivision(calCount, repeatElm);
    uint32_t sreg = static_cast<uint32_t>(calCount);
    Reg::MaskReg dstReg, maskReg;
    Reg::UnalignReg uReg, dupuReg;
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

template <
    typename T, typename U, CMPMODE cmpMode, uint8_t scalarIdx,
    typename Std::enable_if<Std::is_same<PrimT<T>, double>::value, bool>::type = true>
__simd_vf__ inline void CompareScalarLevel2B64(
    __ubuf__ U* dst, __ubuf__ double* src0, __ubuf__ double* src1, const uint32_t calCount)
{
    constexpr uint32_t repeatElm = GetVecLen() / sizeof(double) * 2;
    uint16_t repeatTime = CeilDivision(calCount, repeatElm);
    uint32_t sreg = static_cast<uint32_t>(calCount);
    Reg::MaskReg dstReg, maskReg;
    Reg::UnalignReg uReg, dupuReg;
    Reg::RegTensor<double, Reg::RegTraitNumOne> preReg;
    Reg::RegTensor<uint32_t> zeroReg;
    Reg::MaskReg maskFull = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    Reg::Duplicate(zeroReg, 0, maskFull);
    if constexpr (scalarIdx == 0) {
        Reg::RegTensor<double, Reg::RegTraitNumTwo> src1Reg, dupReg;
        Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> src0Reg;
        Reg::LoadUnAlignPre(dupuReg, (__ubuf__ double*)src0);
        Reg::LoadUnAlign(preReg, dupuReg, (__ubuf__ double*)src0);
        Reg::DeInterleave(
            (Reg::RegTensor<uint32_t>&)dupReg.reg[0], (Reg::RegTensor<uint32_t>&)dupReg.reg[1],
            (Reg::RegTensor<uint32_t>&)preReg, zeroReg);
        Reg::Duplicate(src0Reg, (Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo>&)dupReg, maskFull);
        for (uint16_t i = 0; i < repeatTime; ++i) {
            maskReg = Reg::UpdateMask<double, Reg::RegTraitNumTwo>(sreg);
            Reg::LoadAlign(src1Reg, src1 + i * repeatElm);
            CompareEqualDouble<uint64_t>(
                dstReg, src0Reg, (Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo>&)src1Reg, maskReg);
            Reg::StoreUnAlign((__ubuf__ uint32_t*&)dst, dstReg, uReg);
        }
    } else {
        Reg::RegTensor<double, Reg::RegTraitNumTwo> src0Reg, dupReg;
        Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> src1Reg;
        Reg::LoadUnAlignPre(dupuReg, (__ubuf__ double*)src1);
        Reg::LoadUnAlign(preReg, dupuReg, (__ubuf__ double*)src1);
        Reg::DeInterleave(
            (Reg::RegTensor<uint32_t>&)dupReg.reg[0], (Reg::RegTensor<uint32_t>&)dupReg.reg[1],
            (Reg::RegTensor<uint32_t>&)preReg, zeroReg);
        Reg::Duplicate(src1Reg, (Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo>&)dupReg, maskFull);
        for (uint16_t i = 0; i < repeatTime; ++i) {
            maskReg = Reg::UpdateMask<double, Reg::RegTraitNumTwo>(sreg);
            Reg::LoadAlign(src0Reg, src0 + i * repeatElm);
            CompareEqualDouble<uint64_t>(
                dstReg, (Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo>&)src0Reg, src1Reg, maskReg);
            Reg::StoreUnAlign((__ubuf__ uint32_t*&)dst, dstReg, uReg);
        }
    }
    Reg::StoreUnAlignPost<U, Reg::PostLiteral::POST_MODE_NORMAL>(dst, uReg, 0);
}

template <typename T, typename U, bool isSetMask = true, uint8_t scalarIdx = 1>
__aicore__ inline void VcmpvsImplB64(
    __ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, CMPMODE cmpMode, const uint32_t calCount)
{
    switch (cmpMode) {
        case CMPMODE::LT: {
            CompareScalarLevel2B64<T, U, CMPMODE::LT, scalarIdx>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::GT: {
            CompareScalarLevel2B64<T, U, CMPMODE::GT, scalarIdx>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::EQ: {
            CompareScalarLevel2B64<T, U, CMPMODE::EQ, scalarIdx>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::LE: {
            CompareScalarLevel2B64<T, U, CMPMODE::LE, scalarIdx>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::GE: {
            CompareScalarLevel2B64<T, U, CMPMODE::GE, scalarIdx>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::NE: {
            CompareScalarLevel2B64<T, U, CMPMODE::NE, scalarIdx>(dst, src0, src1, calCount);
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
            T, uint8_t, int8_t, bfloat16_t, half, int16_t, uint16_t, int32_t, uint32_t, float, uint64_t, int64_t,
            double>(),
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
__simd_vf__ inline void CompareScalarBothTensorLevel0CounterMode(
    __ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, __ubuf__ uint64_t* tempBuf,
    const UnaryRepeatParams repeatParams)
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
                src1Reg, src1, static_cast<uint32_t>(repeatParams.srcBlkStride),
                static_cast<uint32_t>(repeatParams.srcRepStride), maskReg);
        } else {
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
                src0Reg, src0, static_cast<uint32_t>(repeatParams.srcBlkStride),
                static_cast<uint32_t>(repeatParams.srcRepStride), maskReg);
        }
        Reg::Compare<T, cmpMode>(dstReg, src0Reg, src1Reg, maskReg);
        Reg::StoreUnAlign((__ubuf__ T*&)dst, dstReg, uReg);
    }
    Reg::StoreUnAlignPost<U, Reg::PostLiteral::POST_MODE_NORMAL>(dst, uReg, 0);
}

template <
    typename T, typename U, CMPMODE cmpMode, bool isBitMapMode, uint8_t scalarIdx, Reg::LoadDist pattern,
    bool isSetMask>
__simd_vf__ inline void CompareScalarBothTensorLevel0NormalMode(
    __ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, uint8_t repeatTime,
    const UnaryRepeatParams repeatParams)
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
                src1Reg, src1, static_cast<uint32_t>(repeatParams.srcBlkStride),
                static_cast<uint32_t>(repeatParams.srcRepStride), maskReg);
        } else {
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
                src0Reg, src0, static_cast<uint32_t>(repeatParams.srcBlkStride),
                static_cast<uint32_t>(repeatParams.srcRepStride), maskReg);
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
                CompareScalarBothTensorLevel0CounterMode<
                    T, U, CMPMODE::LT, isBitMapMode, scalarIdx, pattern, isSetMask>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                CompareScalarBothTensorLevel0CounterMode<
                    T, U, CMPMODE::GT, isBitMapMode, scalarIdx, pattern, isSetMask>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                CompareScalarBothTensorLevel0CounterMode<
                    T, U, CMPMODE::EQ, isBitMapMode, scalarIdx, pattern, isSetMask>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                CompareScalarBothTensorLevel0CounterMode<
                    T, U, CMPMODE::LE, isBitMapMode, scalarIdx, pattern, isSetMask>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                CompareScalarBothTensorLevel0CounterMode<
                    T, U, CMPMODE::GE, isBitMapMode, scalarIdx, pattern, isSetMask>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                CompareScalarBothTensorLevel0CounterMode<
                    T, U, CMPMODE::NE, isBitMapMode, scalarIdx, pattern, isSetMask>(
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
                CompareScalarBothTensorLevel0NormalMode<T, U, CMPMODE::LT, isBitMapMode, scalarIdx, pattern, isSetMask>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                CompareScalarBothTensorLevel0NormalMode<T, U, CMPMODE::GT, isBitMapMode, scalarIdx, pattern, isSetMask>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                CompareScalarBothTensorLevel0NormalMode<T, U, CMPMODE::EQ, isBitMapMode, scalarIdx, pattern, isSetMask>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                CompareScalarBothTensorLevel0NormalMode<T, U, CMPMODE::LE, isBitMapMode, scalarIdx, pattern, isSetMask>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                CompareScalarBothTensorLevel0NormalMode<T, U, CMPMODE::GE, isBitMapMode, scalarIdx, pattern, isSetMask>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                CompareScalarBothTensorLevel0NormalMode<T, U, CMPMODE::NE, isBitMapMode, scalarIdx, pattern, isSetMask>(
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
#endif // ASCENDC_MODULE_OPERATOR_VEC_CMP_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_CMP_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_CMP_IMPL_H__
#endif
