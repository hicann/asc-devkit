/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file kernel_operator_vec_gather_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_v516/kernel_operator_vec_gather_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_vec_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_GATHER_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_VEC_GATHER_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_GATHER_IMPL_H
#include "../kernel_utils.h"
#include "kernel_operator_common_impl.h"
#include "kernel_operator_vec_template_impl.h"

namespace AscendC {
/* **************************************************************************************************
 * Gather                                             *
 * ************************************************************************************************* */
template <typename T, bool isNormalMode>
__aicore__ inline void VfGatherApi0B16(
    __ubuf__ T* dst, __ubuf__ T* src, __ubuf__ uint32_t* srcOffsetLocal, const uint32_t srcBaseIndex,
    const uint8_t repeatTime, const uint16_t& dstRepStride, uint32_t dstRepeatCount, uint32_t u32OffsetRepeatCount,
    uint32_t blkCount, const uint64_t maskCount)
{
    Reg::RegTensor<uint32_t> offsetReg0;
    Reg::RegTensor<uint32_t> offsetReg1;
    Reg::RegTensor<uint16_t> indexReg;
    Reg::RegTensor<uint16_t> dstReg;
    uint32_t sregPlt = static_cast<uint32_t>(maskCount);
    Reg::MaskReg indexMask = Reg::CreateMask<uint32_t>();
    Reg::MaskReg selectMask = Reg::CreateMask<uint16_t, Reg::MaskPattern::H>();
    Reg::MaskReg dstMask;
    if constexpr (isNormalMode) {
        dstMask = Reg::MoveMask<T>();
    }
    for (uint16_t i = 0; i < (uint16_t)repeatTime; ++i) {
        if constexpr (!isNormalMode) {
            dstMask = Reg::UpdateMask<T>(sregPlt);
        }
        Reg::LoadAlign(offsetReg0, srcOffsetLocal + (2 * i) * u32OffsetRepeatCount);
        Reg::LoadAlign(offsetReg1, srcOffsetLocal + (2 * i + 1) * u32OffsetRepeatCount);
        // convert addr offset into B16 element index: divide by 2 (implemented by ShiftRight 1 bit)
        ShiftRights(offsetReg0, offsetReg0, (int16_t)1, indexMask);
        ShiftRights(offsetReg1, offsetReg1, (int16_t)1, indexMask);
        // extract the lower 16-bit of uint32_t offset data into uint16_t index data:
        // for offsetReg0，pack every lower 16-bit into the lower half of the vreg：
        // 0x00FF00FE00FD... ->0xFFFEFD...000000...
        // for offsetReg1, pack every higher 16-bit into the higher half of the vreg:
        // 0x001100120013... -> 0x000000...111213...
        Reg::Pack<uint16_t, uint32_t, Reg::HighLowPart::LOWEST>((Reg::RegTensor<uint16_t>&)offsetReg0, offsetReg0);
        Reg::Pack<uint16_t, uint32_t, Reg::HighLowPart::HIGHEST>((Reg::RegTensor<uint16_t>&)offsetReg1, offsetReg1);
        // Select the effective data in offsetReg0 and offsetReg1 and joint them into a complete uint16_t type
        // indexReg：0xFFFEFD...111213...
        Select(indexReg, (Reg::RegTensor<uint16_t>&)offsetReg0, (Reg::RegTensor<uint16_t>&)offsetReg1, selectMask);
        Reg::Gather(dstReg, (__ubuf__ uint16_t*)src + srcBaseIndex, indexReg, dstMask);
        Reg::StoreAlign((__ubuf__ uint16_t*)dst + i * dstRepStride * blkCount, dstReg, dstMask);
    }
}

template <typename T, bool isNormalMode>
__aicore__ inline void GatherApi0B16Impl(
    __ubuf__ T* dst, __ubuf__ T* src, __ubuf__ uint32_t* srcOffsetLocal, const uint32_t srcBaseIndex,
    const uint8_t repeatTime, const uint16_t& dstRepStride, const uint64_t maskCount)
{
    uint32_t dstRepeatCount = (uint32_t)(VECTOR_REG_WIDTH / sizeof(T));
    uint32_t u32OffsetRepeatCount = (uint32_t)(VECTOR_REG_WIDTH / sizeof(uint32_t));
    uint32_t blkCount = (uint32_t)(ONE_BLK_SIZE / sizeof(T));
    VF_CALL<VfGatherApi0B16<T, isNormalMode>>(
        dst, src, srcOffsetLocal, srcBaseIndex, repeatTime, dstRepStride, dstRepeatCount, u32OffsetRepeatCount,
        blkCount, maskCount);
}

template <typename T, bool isNormalMode>
__aicore__ inline void VfGatherApi0B32(
    __ubuf__ T* dst, __ubuf__ T* src, __ubuf__ uint32_t* srcOffsetLocal, const uint32_t srcBaseIndex,
    const uint8_t repeatTime, const uint16_t& dstRepStride, uint32_t dstRepeatCount, uint32_t u32OffsetRepeatCount,
    uint32_t blkCount, const uint64_t maskCount)
{
    Reg::RegTensor<uint32_t> offsetReg;
    Reg::RegTensor<uint32_t> indexReg;
    Reg::RegTensor<uint32_t> dstReg;
    uint32_t sregPlt = static_cast<uint32_t>(maskCount);
    Reg::MaskReg indexMask = Reg::CreateMask<T>();
    Reg::MaskReg dstMask;
    Reg::MaskReg offsetMask = Reg::CreateMask<uint32_t>();
    if constexpr (isNormalMode) {
        dstMask = Reg::MoveMask<T>();
    }
    for (uint16_t i = 0; i < (uint16_t)repeatTime; ++i) {
        if constexpr (!isNormalMode) {
            dstMask = Reg::UpdateMask<T>(sregPlt);
        }
        Reg::LoadAlign(offsetReg, srcOffsetLocal + i * u32OffsetRepeatCount);
        // convert addr offset into B32 element index: divide by 4 (implemented by ShiftRight 2 bit)
        ShiftRights(indexReg, offsetReg, (int16_t)2, indexMask);
        Reg::Gather(dstReg, (__ubuf__ uint32_t*)src + srcBaseIndex, indexReg, dstMask);
        Reg::StoreAlign((__ubuf__ uint32_t*)dst + i * dstRepStride * blkCount, dstReg, dstMask);
    }
}

template <typename T, bool isNormalMode>
__aicore__ inline void GatherApi0B32Impl(
    __ubuf__ T* dst, __ubuf__ T* src, __ubuf__ uint32_t* srcOffsetLocal, const uint32_t srcBaseIndex,
    const uint8_t repeatTime, const uint16_t& dstRepStride, const uint64_t maskCount)
{
    uint32_t dstRepeatCount = (uint32_t)(VECTOR_REG_WIDTH / sizeof(T));
    uint32_t u32OffsetRepeatCount = (uint32_t)(VECTOR_REG_WIDTH / sizeof(uint32_t));
    uint32_t blkCount = (uint32_t)(ONE_BLK_SIZE / sizeof(T));
    VF_CALL<VfGatherApi0B32<T, isNormalMode>>(
        dst, src, srcOffsetLocal, srcBaseIndex, repeatTime, dstRepStride, dstRepeatCount, u32OffsetRepeatCount,
        blkCount, maskCount);
}

template <typename T, bool isNormalMode>
__aicore__ inline void VfGatherApi0B64Normal(
    __ubuf__ T* dst, __ubuf__ T* src, __ubuf__ uint32_t* srcOffsetLocal, const uint32_t srcBaseIndex,
    const uint8_t repeatTime, const uint16_t& dstRepStride, uint32_t u64OffsetRepeatCount, uint32_t u32BlkCount,
    const uint64_t maskCount)
{
    Reg::RegTensor<uint32_t> offsetReg;
    Reg::RegTensor<uint32_t> indexReg;
    Reg::RegTensor<uint32_t> oddIndexReg;
    Reg::RegTensor<uint32_t> tmpReg;
    Reg::RegTensor<uint32_t> indexU32Reg;
    Reg::RegTensor<uint32_t> dstReg;
    uint32_t sregPlt = static_cast<uint32_t>(maskCount);
    Reg::MaskReg indexMask = Reg::CreateMask<uint32_t, Reg::MaskPattern::H>();
    Reg::MaskReg dstMask;
    if constexpr (isNormalMode) {
        dstMask = Reg::UpdateMask<uint32_t>(sregPlt);
    }
    for (uint16_t i = 0; i < (uint16_t)repeatTime; ++i) {
        if constexpr (!isNormalMode) {
            dstMask = Reg::UpdateMask<uint32_t>(sregPlt);
        }
        Reg::LoadAlign(offsetReg, srcOffsetLocal + i * u64OffsetRepeatCount);
        // convert addr offset into B64 element index: divide by 8 (implemented by ShiftRight 3 bit)
        ShiftRights(indexReg, offsetReg, (int16_t)3, indexMask);
        // Consider every B64 element as two B32 elements
        // the index of lower 32-bit is 2 * i, the index of higher 32-bit is 2 * i + 1
        Muls(indexReg, indexReg, 2, indexMask);
        Adds(oddIndexReg, indexReg, 1, indexMask);
        // Interleave the seperately calculated indices of the lower and higher 32-bit of every B64 element
        Interleave(indexU32Reg, tmpReg, indexReg, oddIndexReg);
        Reg::Gather(dstReg, (__ubuf__ uint32_t*)src + srcBaseIndex, indexU32Reg, dstMask);
        Reg::StoreAlign((__ubuf__ uint32_t*)dst + i * dstRepStride * u32BlkCount, dstReg, dstMask);
    }
}

template <typename T, bool isNormalMode>
__aicore__ inline void GatherApi0B64NormalImpl(
    __ubuf__ T* dst, __ubuf__ T* src, __ubuf__ uint32_t* srcOffsetLocal, const uint32_t srcBaseIndex,
    const uint8_t repeatTime, const uint16_t& dstRepStride, const uint64_t maskCount)
{
    uint64_t dstMaskValue = maskCount * 2;
    uint32_t u64OffsetRepeatCount = (uint32_t)(VECTOR_REG_WIDTH / sizeof(T));
    uint32_t u32BlkCount = (uint32_t)(ONE_BLK_SIZE / sizeof(uint32_t));
    VF_CALL<VfGatherApi0B64Normal<T, isNormalMode>>(
        dst, src, srcOffsetLocal, srcBaseIndex, repeatTime, dstRepStride, u64OffsetRepeatCount, u32BlkCount,
        dstMaskValue);
}

template <typename T, bool isNormalMode>
__aicore__ inline void VfGatherApi0B64Bits(
    __ubuf__ T* dst, __ubuf__ T* src, __ubuf__ uint32_t* srcOffsetLocal, const uint32_t srcBaseIndex,
    const uint8_t repeatTime, const uint16_t& dstRepStride, uint32_t u64OffsetRepeatCount, uint32_t u32BlkCount,
    const uint64_t maskCount)
{
    Reg::RegTensor<uint32_t> offsetReg;
    Reg::RegTensor<uint32_t> bitsIndexReg;
    Reg::RegTensor<uint32_t> bitsOddIndexReg;
    Reg::RegTensor<uint32_t> tmpReg;
    Reg::RegTensor<uint32_t> indexU32Reg;
    Reg::RegTensor<uint32_t> dstReg;
    uint32_t sregPlt = static_cast<uint32_t>(maskCount);
    Reg::MaskReg dstMask;
    Reg::MaskReg indexMask = Reg::CreateMask<uint32_t, Reg::MaskPattern::H>();
    if constexpr (isNormalMode) {
        Reg::MaskReg tmpMask0;
        Reg::MaskReg tmpMask1 = Reg::MoveMask<uint16_t>();
        Reg::MaskPack(tmpMask1, tmpMask1);
        Reg::MaskInterleave<uint8_t>(dstMask, tmpMask0, tmpMask1, tmpMask1);
        Reg::MaskUnPack(dstMask, dstMask);
        Reg::MaskUnPack(dstMask, dstMask);
    }

    for (uint16_t i = 0; i < (uint16_t)repeatTime; ++i) {
        if constexpr (!isNormalMode) {
            dstMask = Reg::UpdateMask<uint32_t>(sregPlt);
        }
        Reg::LoadAlign(offsetReg, srcOffsetLocal + i * u64OffsetRepeatCount);
        // convert addr offset into B64 element index: divide by 8 (implemented by ShiftRight 3 bit)
        ShiftRights(bitsIndexReg, offsetReg, (int16_t)3, indexMask);
        // Consider every B64 element as two B32 elements
        // the index of lower 32-bit is 2 * i, the index of higher 32-bit is 2 * i + 1
        Muls(bitsIndexReg, bitsIndexReg, 2, indexMask);
        Adds(bitsOddIndexReg, bitsIndexReg, 1, indexMask);
        // Interleave the seperately calculated indices of the lower and higher 32-bit of every B64 element
        Interleave(indexU32Reg, tmpReg, bitsIndexReg, bitsOddIndexReg);
        Reg::Gather(dstReg, (__ubuf__ uint32_t*)src + srcBaseIndex, indexU32Reg, dstMask);
        Reg::StoreAlign((__ubuf__ uint32_t*)dst + i * dstRepStride * u32BlkCount, dstReg, dstMask);
    }
}

template <typename T, bool isNormalMode>
__aicore__ inline void GatherApi0B64BitsImpl(
    __ubuf__ T* dst, __ubuf__ T* src, __ubuf__ uint32_t* srcOffsetLocal, const uint32_t srcBaseIndex,
    const uint8_t repeatTime, const uint16_t& dstRepStride, const uint64_t maskCount)
{
    uint32_t u64OffsetRepeatCount = (uint32_t)(VECTOR_REG_WIDTH / sizeof(T));
    uint32_t u32BlkCount = (uint32_t)(ONE_BLK_SIZE / sizeof(uint32_t));
    uint64_t dstMaskValue = maskCount * 2;
    VF_CALL<VfGatherApi0B64Bits<T, isNormalMode>>(
        dst, src, srcOffsetLocal, srcBaseIndex, repeatTime, dstRepStride, u64OffsetRepeatCount, u32BlkCount,
        dstMaskValue);
}

template <typename T>
__aicore__ inline void VfGatherApi2B8(
    __ubuf__ T* dst, __ubuf__ T* src, __ubuf__ uint32_t* srcOffsetLocal, const uint32_t srcBaseIndex,
    const uint32_t count, uint32_t u32OffsetRepeatCount, uint32_t u8GatherRepeatCount)
{
    Reg::RegTensor<uint32_t> offsetReg0;
    Reg::RegTensor<uint32_t> offsetReg1;
    Reg::RegTensor<uint16_t> indexReg;
    Reg::RegTensor<uint16_t> dstReg;
    uint32_t sreg = (uint32_t)count;
    Reg::MaskReg indexMask = Reg::CreateMask<uint16_t>();
    Reg::MaskReg dstMask;
    uint16_t repeatTime = CeilDivision(count, u8GatherRepeatCount);
    Reg::MaskReg selectMask = Reg::CreateMask<uint16_t, Reg::MaskPattern::H>();
    for (uint16_t i = 0; i < (uint16_t)repeatTime; ++i) {
        dstMask = Reg::UpdateMask<uint16_t>(sreg);
        Reg::LoadAlign(offsetReg0, srcOffsetLocal + (2 * i) * u32OffsetRepeatCount);
        Reg::LoadAlign(offsetReg1, srcOffsetLocal + (2 * i + 1) * u32OffsetRepeatCount);
        // extract the lower 16-bit of uint32_t offset data into uint16_t index data:
        // for offsetReg0，pack every lower 16-bit into the lower half of the vreg：
        // 0x00FF00FE00FD... -> 0xFFFEFD...000000...
        // for offsetReg1, pack every higher 16-bit into the higher half of the vreg:
        // 0x001100120013... -> 0x000000...111213...
        Reg::Pack<uint16_t, uint32_t, Reg::HighLowPart::LOWEST>((Reg::RegTensor<uint16_t>&)offsetReg0, offsetReg0);
        Reg::Pack<uint16_t, uint32_t, Reg::HighLowPart::HIGHEST>((Reg::RegTensor<uint16_t>&)offsetReg1, offsetReg1);
        // Select the effective data in offsetReg0 and offsetReg1 and joint them into a complete uint16_t type indexReg:
        // 0xFFFEFD...111213...
        Select(indexReg, (Reg::RegTensor<uint16_t>&)offsetReg0, (Reg::RegTensor<uint16_t>&)offsetReg1, selectMask);
        Reg::Gather(dstReg, (__ubuf__ uint8_t*)src + srcBaseIndex, indexReg, dstMask);
        // remove the higher zeros of the uint16_t data gathered by the Micro Gather instr, and pack into continuous B8
        // data: 0x010203... -> 0x123...000... (only the lower 128 elements are effective)
        Reg::Pack((Reg::RegTensor<uint8_t>&)dstReg, dstReg);
        // convert uint16_t type preg to B8 type preg：0b010101... -> 0b111...000... (lower 128-bit effective)
        Reg::MaskPack(dstMask, dstMask);
        Reg::StoreAlign((__ubuf__ uint8_t*)dst + i * u8GatherRepeatCount, (Reg::RegTensor<uint8_t>&)dstReg, dstMask);
    }
}

template <typename T>
__aicore__ inline void GatherApi2B8Impl(
    __ubuf__ T* dst, __ubuf__ T* src, __ubuf__ uint32_t* srcOffsetLocal, const uint32_t srcBaseIndex,
    const uint32_t count)
{
    uint32_t u8GatherRepeatCount = (uint32_t)(VECTOR_REG_WIDTH / sizeof(uint16_t));
    uint32_t u32OffsetRepeatCount = (uint32_t)(VECTOR_REG_WIDTH / sizeof(uint32_t));
    VF_CALL<VfGatherApi2B8<T>>(
        dst, src, srcOffsetLocal, srcBaseIndex, count, u32OffsetRepeatCount, u8GatherRepeatCount);
}

template <typename T>
__aicore__ inline void VfGatherApi2B16(
    __ubuf__ T* dst, __ubuf__ T* src, __ubuf__ uint32_t* srcOffsetLocal, const uint32_t srcBaseIndex,
    const uint32_t count, uint32_t dstRepeatCount, uint32_t u32OffsetRepeatCount, uint16_t repeatTime)
{
    Reg::RegTensor<uint32_t> api2OffsetReg0;
    Reg::RegTensor<uint32_t> api2OffsetReg1;
    Reg::RegTensor<uint16_t> indexReg;
    Reg::RegTensor<uint16_t> dstReg;
    Reg::MaskReg indexMask = Reg::CreateMask<uint16_t>();
    Reg::MaskReg dstMask;
    Reg::MaskReg selectMask = Reg::CreateMask<uint16_t, Reg::MaskPattern::H>();
    uint32_t sreg = (uint32_t)count;
    for (uint16_t i = 0; i < (uint16_t)repeatTime; ++i) {
        dstMask = Reg::UpdateMask<uint16_t>(sreg);
        Reg::LoadAlign(api2OffsetReg0, srcOffsetLocal + (2 * i) * u32OffsetRepeatCount);
        Reg::LoadAlign(api2OffsetReg1, srcOffsetLocal + (2 * i + 1) * u32OffsetRepeatCount);
        // convert addr offset into B32 element index: divide by 4 (implemented by ShiftRight 2 bit)
        ShiftRights(api2OffsetReg0, api2OffsetReg0, (int16_t)1, indexMask);
        ShiftRights(api2OffsetReg1, api2OffsetReg1, (int16_t)1, indexMask);
        // extract the lower 16-bit of uint32_t offset data into uint16_t index data:
        // for api2OffsetReg0，pack every lower 16-bit into the lower half of the vreg：
        // 0x00FF00FE00FD... -> 0xFFFEFD...000000...
        // for api2OffsetReg1, pack every higher 16-bit into the higher half of the vreg:
        // 0x001100120013... -> 0x000000...111213...
        Reg::Pack<uint16_t, uint32_t, Reg::HighLowPart::LOWEST>(
            (Reg::RegTensor<uint16_t>&)api2OffsetReg0, api2OffsetReg0);
        Reg::Pack<uint16_t, uint32_t, Reg::HighLowPart::HIGHEST>(
            (Reg::RegTensor<uint16_t>&)api2OffsetReg1, api2OffsetReg1);
        // Select the effective data in api2OffsetReg0 and api2OffsetReg1 and joint them into a complete uint16_t type
        // indexReg：0xFFFEFD...111213...
        Select(
            indexReg, (Reg::RegTensor<uint16_t>&)api2OffsetReg0, (Reg::RegTensor<uint16_t>&)api2OffsetReg1, selectMask);
        Reg::Gather(dstReg, (__ubuf__ uint16_t*)src + srcBaseIndex, indexReg, dstMask);
        Reg::StoreAlign((__ubuf__ uint16_t*)dst + i * dstRepeatCount, dstReg, dstMask);
    }
}

template <typename T>
__aicore__ inline void GatherApi2B16Impl(
    __ubuf__ T* dst, __ubuf__ T* src, __ubuf__ uint32_t* srcOffsetLocal, const uint32_t srcBaseIndex,
    const uint32_t count)
{
    uint32_t dstRepeatCount = (uint32_t)(VECTOR_REG_WIDTH / sizeof(T));
    uint32_t u32OffsetRepeatCount = (uint32_t)(VECTOR_REG_WIDTH / sizeof(uint32_t));
    uint16_t repeatTime = CeilDivision(count, dstRepeatCount);
    VF_CALL<VfGatherApi2B16<T>>(
        dst, src, srcOffsetLocal, srcBaseIndex, count, dstRepeatCount, u32OffsetRepeatCount, repeatTime);
}

template <typename T>
__aicore__ inline void VfGatherApi2B32(
    __ubuf__ T* dst, __ubuf__ T* src, __ubuf__ uint32_t* srcOffsetLocal, const uint32_t srcBaseIndex,
    const uint32_t count, uint32_t dstRepeatCount, uint32_t u32OffsetRepeatCount, uint16_t repeatTime)
{
    Reg::RegTensor<uint32_t> offsetReg;
    Reg::RegTensor<uint32_t> indexReg;
    Reg::RegTensor<uint32_t> dstReg;
    Reg::MaskReg indexMask = Reg::CreateMask<T>();
    Reg::MaskReg dstMask;
    uint32_t sreg = (uint32_t)count;
    for (uint16_t i = 0; i < (uint16_t)repeatTime; ++i) {
        dstMask = Reg::UpdateMask<uint32_t>(sreg);
        Reg::LoadAlign(offsetReg, srcOffsetLocal + i * u32OffsetRepeatCount);
        // convert addr offset into B32 element index: divide by 4 (implemented by ShiftRight 2 bit)
        ShiftRights(indexReg, offsetReg, (int16_t)2, indexMask);
        Reg::Gather(dstReg, (__ubuf__ uint32_t*)src + srcBaseIndex, indexReg, dstMask);
        Reg::StoreAlign((__ubuf__ uint32_t*)dst + i * dstRepeatCount, dstReg, dstMask);
    }
}

template <typename T>
__aicore__ inline void GatherApi2B32Impl(
    __ubuf__ T* dst, __ubuf__ T* src, __ubuf__ uint32_t* srcOffsetLocal, const uint32_t srcBaseIndex,
    const uint32_t count)
{
    uint32_t dstRepeatCount = (uint32_t)(VECTOR_REG_WIDTH / sizeof(T));
    uint32_t u32OffsetRepeatCount = (uint32_t)(VECTOR_REG_WIDTH / sizeof(uint32_t));
    uint16_t repeatTime = CeilDivision(count, dstRepeatCount);
    VF_CALL<VfGatherApi2B32<T>>(
        dst, src, srcOffsetLocal, srcBaseIndex, count, dstRepeatCount, u32OffsetRepeatCount, repeatTime);
}

template <typename T>
__aicore__ inline void VfGatherApi2B64(
    __ubuf__ T* dst, __ubuf__ T* src, __ubuf__ uint32_t* srcOffsetLocal, const uint32_t srcBaseIndex,
    const uint32_t count, uint32_t u32Count, uint32_t u64OffsetRepeatCount, uint32_t u32RepeatCount,
    uint16_t repeatTime)
{
    Reg::RegTensor<uint32_t> offsetReg;
    Reg::RegTensor<uint32_t> indexReg;
    Reg::RegTensor<uint32_t> oddIndexReg;
    Reg::RegTensor<uint32_t> tmpReg;
    Reg::RegTensor<uint32_t> indexU32Reg;
    Reg::RegTensor<uint32_t> dstReg;
    Reg::MaskReg indexMask = Reg::CreateMask<uint32_t, Reg::MaskPattern::H>();
    Reg::MaskReg dstMask;
    uint32_t sreg = (uint32_t)u32Count;
    for (uint16_t i = 0; i < (uint16_t)repeatTime; ++i) {
        dstMask = Reg::UpdateMask<uint32_t>(sreg);
        Reg::LoadAlign(offsetReg, srcOffsetLocal + i * u64OffsetRepeatCount);
        // convert addr offset into B64 element index: divide by 8 (implemented by ShiftRight 3 bit)
        ShiftRights(indexReg, offsetReg, (int16_t)3, indexMask);
        // Consider every B64 element as two B32 elements
        // the index of lower 32-bit is 2*i, the index of higher 32-bit is 2*i+1
        Muls(indexReg, indexReg, 2, indexMask);
        Adds(oddIndexReg, indexReg, 1, indexMask);
        // Interleave the seperately calculated indices of the lower and higher 32-bit of every B64 element
        Interleave(indexU32Reg, tmpReg, indexReg, oddIndexReg);
        Reg::Gather(dstReg, (__ubuf__ uint32_t*)src + srcBaseIndex, indexU32Reg, dstMask);
        Reg::StoreAlign((__ubuf__ uint32_t*)dst + i * u32RepeatCount, dstReg, dstMask);
    }
}

template <typename T>
__aicore__ inline void GatherApi2B64Impl(
    __ubuf__ T* dst, __ubuf__ T* src, __ubuf__ uint32_t* srcOffsetLocal, const uint32_t srcBaseIndex,
    const uint32_t count)
{
    uint32_t u32Count = (uint32_t)count * 2;
    uint32_t u64OffsetRepeatCount = (uint32_t)(VECTOR_REG_WIDTH / sizeof(T));
    uint32_t u32RepeatCount = (uint32_t)(VECTOR_REG_WIDTH / sizeof(uint32_t));
    uint16_t repeatTime = CeilDivision(u32Count, u32RepeatCount);
    VF_CALL<VfGatherApi2B64<T>>(
        dst, src, srcOffsetLocal, srcBaseIndex, count, u32Count, u64OffsetRepeatCount, u32RepeatCount, repeatTime);
}

// Gather::Level 0 Normal mode
template <typename T>
__aicore__ inline void GatherImpl(
    __ubuf__ T* dst, __ubuf__ T* src, __ubuf__ uint32_t* srcOffsetLocal, const uint32_t srcLength,
    const uint32_t srcBaseOffset, const uint64_t mask, const uint8_t repeatTime, const uint16_t& dstRepStride)
{
    static_assert(SupportBytes<T, 2, 4, 8>(), "Gather only support type b16/b32/b64 on current device");

    uint8_t newRepeatTimes = repeatTime;
    bool isNormalMode = !Internal::IsCounterMode();
    if (isNormalMode) {
        if constexpr (sizeof(T) == 2) {
            SetVectorMask<uint16_t>(mask);
        } else {
            SetVectorMask<uint32_t>(mask);
        }
    } else {
        newRepeatTimes = static_cast<uint8_t>(Internal::VecMicroGetRepeatTimes<T, false>(mask, repeatTime));
    }

    uint32_t srcBaseIndex;
    if constexpr (sizeof(T) == 2) {
        srcBaseIndex = srcBaseOffset / sizeof(T);
        if (isNormalMode) {
            GatherApi0B16Impl<T, true>(dst, src, srcOffsetLocal, srcBaseIndex, newRepeatTimes, dstRepStride, mask);
        } else {
            GatherApi0B16Impl<T, false>(dst, src, srcOffsetLocal, srcBaseIndex, newRepeatTimes, dstRepStride, mask);
        }
    } else if constexpr (sizeof(T) == 4) {
        srcBaseIndex = srcBaseOffset / sizeof(T);
        if (isNormalMode) {
            GatherApi0B32Impl<T, true>(dst, src, srcOffsetLocal, srcBaseIndex, newRepeatTimes, dstRepStride, mask);
        } else {
            GatherApi0B32Impl<T, false>(dst, src, srcOffsetLocal, srcBaseIndex, newRepeatTimes, dstRepStride, mask);
        }
    } else {
        srcBaseIndex = srcBaseOffset / sizeof(uint32_t);
        if (isNormalMode) {
            GatherApi0B64NormalImpl<T, true>(
                dst, src, srcOffsetLocal, srcBaseIndex, newRepeatTimes, dstRepStride, mask);
        } else {
            GatherApi0B64NormalImpl<T, false>(
                dst, src, srcOffsetLocal, srcBaseIndex, newRepeatTimes, dstRepStride, mask);
        }
    }
}

/* **************************************************************************************************
 * Gather                                             *
 * ************************************************************************************************* */
// Gather::Level 0 Bit-wise mode
template <typename T>
__aicore__ inline void GatherImpl(
    __ubuf__ T* dst, __ubuf__ T* src, __ubuf__ uint32_t* srcOffsetLocal, const uint32_t srcLength,
    const uint32_t srcBaseOffset, const uint64_t mask[], const uint8_t repeatTime, const uint16_t& dstRepStride)
{
    static_assert(SupportBytes<T, 2, 4, 8>(), "Gather only support type b16/b32/b64 on current device");

    uint8_t newRepeatTimes = repeatTime;
    bool isNormalMode = !Internal::IsCounterMode();
    if (isNormalMode) {
        if constexpr (sizeof(T) == 2) {
            SetVectorMask<uint16_t>(mask[1], mask[0]);
        } else {
            SetVectorMask<uint32_t>(mask[1], mask[0]);
        }
    } else {
        newRepeatTimes = static_cast<uint8_t>(Internal::VecMicroGetRepeatTimes<T, false>(mask[0], repeatTime));
    }

    uint32_t srcBaseIndex;
    if constexpr (sizeof(T) == 2) {
        srcBaseIndex = srcBaseOffset / sizeof(T);
        if (isNormalMode) {
            GatherApi0B16Impl<T, true>(dst, src, srcOffsetLocal, srcBaseIndex, newRepeatTimes, dstRepStride, mask[0]);
        } else {
            GatherApi0B16Impl<T, false>(dst, src, srcOffsetLocal, srcBaseIndex, newRepeatTimes, dstRepStride, mask[0]);
        }
    } else if constexpr (sizeof(T) == 4) {
        srcBaseIndex = srcBaseOffset / sizeof(T);
        if (isNormalMode) {
            GatherApi0B32Impl<T, true>(dst, src, srcOffsetLocal, srcBaseIndex, newRepeatTimes, dstRepStride, mask[0]);
        } else {
            GatherApi0B32Impl<T, false>(dst, src, srcOffsetLocal, srcBaseIndex, newRepeatTimes, dstRepStride, mask[0]);
        }
    } else {
        srcBaseIndex = srcBaseOffset / sizeof(uint32_t);
        if (isNormalMode) {
            GatherApi0B64BitsImpl<T, true>(
                dst, src, srcOffsetLocal, srcBaseIndex, newRepeatTimes, dstRepStride, mask[0]);
        } else {
            GatherApi0B64BitsImpl<T, false>(
                dst, src, srcOffsetLocal, srcBaseIndex, newRepeatTimes, dstRepStride, mask[0]);
        }
    }
}

/* **************************************************************************************************
 * Gather                                             *
 * ************************************************************************************************* */
// Gather::Level 2 Count mode
template <typename T, bool isSetMask = true>
__aicore__ inline void GatherImpl(
    __ubuf__ T* dst, __ubuf__ T* src, __ubuf__ uint32_t* srcOffsetLocal, const uint32_t srcBaseOffset,
    const uint32_t count)
{
    static_assert(SupportBytes<T, 1, 2, 4, 8>(), "Gather only support type b8/b16/b32/b64 on current device");

    uint32_t srcBaseIndex;
    if constexpr (sizeof(T) == 1) {
        srcBaseIndex = srcBaseOffset / sizeof(T);
        GatherApi2B8Impl(dst, src, srcOffsetLocal, srcBaseIndex, count);
    } else if constexpr (sizeof(T) == 2) {
        srcBaseIndex = srcBaseOffset / sizeof(T);
        GatherApi2B16Impl(dst, src, srcOffsetLocal, srcBaseIndex, count);
    } else if constexpr (sizeof(T) == 4) {
        srcBaseIndex = srcBaseOffset / sizeof(T);
        GatherApi2B32Impl(dst, src, srcOffsetLocal, srcBaseIndex, count);
    } else {
        srcBaseIndex = srcBaseOffset / sizeof(uint32_t);
        GatherApi2B64Impl(dst, src, srcOffsetLocal, srcBaseIndex, count);
    }
}

/* **************************************************************************************************
 * Gatherb                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void GatherbImpl(
    __ubuf__ T* dst, __ubuf__ T* src, __ubuf__ uint32_t* srcOffsetLocal, const uint8_t repeatTime,
    const GatherRepeatParams& repeatParams)
{
    constexpr uint32_t oneBlkNum = static_cast<uint32_t>(ONE_BLK_SIZE / sizeof(T));
    __VEC_SCOPE__
    {
        Reg::RegTensor<T> srcReg;
        Reg::RegTensor<T> dstReg;
        Reg::RegTensor<uint32_t> indexReg;
        Reg::MaskReg fullPreg = Reg::CreateMask<uint8_t>();

        constexpr uint32_t sregLower = static_cast<uint32_t>(VECTOR_REG_WIDTH / sizeof(T));
        for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {
            Reg::LoadAlign(indexReg, srcOffsetLocal + i * DEFAULT_BLK_NUM);
            if constexpr (sizeof(T) == 8) {
                // pg is the predicate for block index, and regarded as B32 format.
                // For convenience, we use fullPreg format to represent it.
                Reg::GatherB(dstReg, src, indexReg, fullPreg);
                Reg::StoreAlign<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
                    (__ubuf__ uint32_t*&)dst, (Reg::RegTensor<uint32_t>&)dstReg,
                    static_cast<uint32_t>(repeatParams.dstBlkStride), static_cast<uint32_t>(repeatParams.dstRepStride),
                    fullPreg);
            } else {
                Reg::GatherB(dstReg, src, indexReg, fullPreg);
                Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    dst + i * repeatParams.dstRepStride * oneBlkNum, dstReg, repeatParams.dstBlkStride, fullPreg);
            }
        }
    }
}

template <typename T>
__aicore__ inline void GatherbImpl(
    __ubuf__ T* dst, __ubuf__ T* src, __ubuf__ uint32_t* srcOffsetLocal, const uint32_t srcLength,
    const uint8_t repeatTime, const GatherRepeatParams& repeatParams)
{
    (void)srcLength;
    static_assert(
        SupportBytes<T, 1, 2, 4, 8>(),
        "Failed to check dtype in Gatherb, current api support dtype"
        "combination is src and dst both: uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, "
        "bfloat16_t, uint64_t, int64_t.");
    GatherbImpl(dst, src, srcOffsetLocal, repeatTime, repeatParams);
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_GATHER_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_GATHER_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_GATHER_IMPL_H__
#endif
