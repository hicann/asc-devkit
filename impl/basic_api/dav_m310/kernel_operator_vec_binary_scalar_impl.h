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
 * \file kernel_operator_vec_binary_scalar_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_m310/kernel_operator_vec_binary_scalar_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_SCALAR_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_BINARY_SCALAR_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_BINARY_SCALAR_IMPL_H
#include "../kernel_utils.h"
#include "kernel_operator_common_impl.h"
#include "../../../include/basic_api/kernel_struct_unary.h"
#include "../../../include/basic_api/kernel_tpipe.h"
#include "../../../include/basic_api/kernel_common.h"

// Expand shared register stages in place to preserve the compiler input and instruction order.
#define ASC_M310_STAGE_SCALAR_INIT_REGS(VectorType, count, ElementCount) \
    VectorType vreg0;                                                    \
    VectorType vreg1;                                                    \
    uint32_t sreg = static_cast<uint32_t>(count);                        \
    vector_bool preg;                                                    \
    uint32_t sregLower = static_cast<uint32_t>(ElementCount);            \
    uint16_t repeatTime = CeilDivision(count, ElementCount)

#define ASC_M310_STAGE_SCALAR_BITMASK_AND_STRIDES(VectorType, tmpBuf, repeatParams)      \
    VectorType vreg0;                                                                    \
    VectorType vreg1;                                                                    \
    vector_bool preg;                                                                    \
    plds(preg, ((__ubuf__ uint32_t*)tmpBuf), 0, US);                                     \
    uint32_t strideConfig0 = ((static_cast<uint32_t>(repeatParams.srcBlkStride)) << 16); \
    uint32_t strideConfig1 = ((static_cast<uint32_t>(repeatParams.dstBlkStride)) << 16)

#define ASC_M310_STAGE_SCALAR_COUNT_MASK_AND_STRIDES(                                    \
    VectorType, vreg0, vreg1, vector_bool, preg, tmpBuf, repeatParams)                   \
    VectorType vreg0;                                                                    \
    VectorType vreg1;                                                                    \
    vector_bool preg;                                                                    \
    vector_bool preg1;                                                                   \
    plds(preg, ((__ubuf__ uint32_t*)tmpBuf), 0, US);                                     \
    punpack(preg1, preg, LOWER);                                                         \
    uint32_t strideConfig0 = ((static_cast<uint32_t>(repeatParams.srcBlkStride)) << 16); \
    uint32_t strideConfig1 = ((static_cast<uint32_t>(repeatParams.dstBlkStride)) << 16)

#define ASC_M310_STAGE_SCALAR_CONTIGUOUS_INIT_REGS(VectorType, mask, MaskIntrinsic, repeatParams) \
    VectorType vreg0;                                                                             \
    VectorType vreg1;                                                                             \
    uint32_t sreg = static_cast<uint32_t>(mask);                                                  \
    vector_bool preg = MaskIntrinsic(sreg, POST_UPDATE);                                          \
    uint32_t strideConfig0 = ((static_cast<uint32_t>(repeatParams.srcBlkStride)) << 16);          \
    uint32_t strideConfig1 = ((static_cast<uint32_t>(repeatParams.dstBlkStride)) << 16)

namespace AscendC {
/* **************************************************************************************************
 * Adds                                             *
 * ************************************************************************************************* */
// Adds::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void AddsImpl(
    __ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

// Emit the original overloads with their existing vector scope and intrinsic order.
#define ASC_M310_DEFINE_SCALAR_BITMASK_B16(Function, DataType, VectorType, VectorIntrinsic)            \
    template <>                                                                                        \
    __aicore__ inline void Function<DataType, true>(                                                   \
        __ubuf__ DataType * dst, __ubuf__ DataType * src, DataType scalarValue, const uint64_t mask[], \
        const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)                               \
    {                                                                                                  \
        __ubuf__ uint8_t* tmpBuf = AscendCUtils::GetTemporaryBufferAddr<uint8_t>(TMP_UB_OFFSET, 16);   \
        *((__ubuf__ uint64_t*)tmpBuf) = mask[0];                                                       \
        *((__ubuf__ uint64_t*)tmpBuf + 1) = mask[1];                                                   \
        event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));       \
        SetFlag<HardEvent::S_V>(eventIdSToV);                                                          \
        WaitFlag<HardEvent::S_V>(eventIdSToV);                                                         \
        __VEC_SCOPE__                                                                                  \
        {                                                                                              \
            ASC_M310_STAGE_SCALAR_BITMASK_AND_STRIDES(VectorType, tmpBuf, repeatParams);               \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                         \
                vsldb(vreg0, src + i * repeatParams.srcRepStride * 16, strideConfig0, preg);           \
                VectorIntrinsic(vreg1, vreg0, scalarValue, preg, MODE_ZEROING);                        \
                vsstb(vreg1, dst + i * repeatParams.dstRepStride * 16, strideConfig1, preg);           \
            }                                                                                          \
        }                                                                                              \
        AscendCUtils::FreeTemporaryBuffer<uint8_t>(tmpBuf);                                            \
    }

ASC_M310_DEFINE_SCALAR_BITMASK_B16(AddsImpl, half, vector_f16, vadds)

#define ASC_M310_DEFINE_SCALAR_BITMASK_B32(Function, DataType, VectorType, VectorIntrinsic)            \
    template <>                                                                                        \
    __aicore__ inline void Function<DataType, true>(                                                   \
        __ubuf__ DataType * dst, __ubuf__ DataType * src, DataType scalarValue, const uint64_t mask[], \
        const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)                               \
    {                                                                                                  \
        __ubuf__ uint8_t* tmpBuf = AscendCUtils::GetTemporaryBufferAddr<uint8_t>(TMP_UB_OFFSET, 16);   \
        *((__ubuf__ uint64_t*)tmpBuf) = mask[0];                                                       \
        *((__ubuf__ uint64_t*)tmpBuf + 1) = mask[1];                                                   \
        event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));       \
        SetFlag<HardEvent::S_V>(eventIdSToV);                                                          \
        WaitFlag<HardEvent::S_V>(eventIdSToV);                                                         \
        __VEC_SCOPE__                                                                                  \
        {                                                                                              \
            ASC_M310_STAGE_SCALAR_COUNT_MASK_AND_STRIDES(                                              \
                VectorType, vreg0, vreg1, vector_bool, preg, tmpBuf, repeatParams);                    \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                         \
                vsldb(vreg0, src + i * repeatParams.srcRepStride * 8, strideConfig0, preg1);           \
                VectorIntrinsic(vreg1, vreg0, scalarValue, preg1, MODE_ZEROING);                       \
                vsstb(vreg1, dst + i * repeatParams.dstRepStride * 8, strideConfig1, preg1);           \
            }                                                                                          \
        }                                                                                              \
        AscendCUtils::FreeTemporaryBuffer<uint8_t>(tmpBuf);                                            \
    }

ASC_M310_DEFINE_SCALAR_BITMASK_B32(AddsImpl, float, vector_f32, vadds)
ASC_M310_DEFINE_SCALAR_BITMASK_B32(AddsImpl, int32_t, vector_s32, vadds)
ASC_M310_DEFINE_SCALAR_BITMASK_B16(AddsImpl, int16_t, vector_s16, vadds)

template <typename T, bool isSetMask = true>
__aicore__ inline void AddsImpl(
    __ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

#define ASC_M310_DEFINE_SCALAR_CONTINUOUS_MASK(                                                        \
    Function, DataType, VectorType, MaskIntrinsic, ElementCount, VectorIntrinsic)                      \
    template <>                                                                                        \
    __aicore__ inline void Function<DataType, true>(                                                   \
        __ubuf__ DataType * dst, __ubuf__ DataType * src, DataType scalarValue, const uint64_t mask,   \
        const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)                               \
    {                                                                                                  \
        __VEC_SCOPE__                                                                                  \
        {                                                                                              \
            ASC_M310_STAGE_SCALAR_CONTIGUOUS_INIT_REGS(VectorType, mask, MaskIntrinsic, repeatParams); \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                         \
                vsldb(vreg0, src + i * repeatParams.srcRepStride * ElementCount, strideConfig0, preg); \
                VectorIntrinsic(vreg1, vreg0, scalarValue, preg, MODE_ZEROING);                        \
                vsstb(vreg1, dst + i * repeatParams.dstRepStride * ElementCount, strideConfig1, preg); \
            }                                                                                          \
        }                                                                                              \
    }

ASC_M310_DEFINE_SCALAR_CONTINUOUS_MASK(AddsImpl, half, vector_f16, plt_b16, 16, vadds)
ASC_M310_DEFINE_SCALAR_CONTINUOUS_MASK(AddsImpl, int16_t, vector_s16, plt_b16, 16, vadds)
ASC_M310_DEFINE_SCALAR_CONTINUOUS_MASK(AddsImpl, int32_t, vector_s32, plt_b32, 8, vadds)
ASC_M310_DEFINE_SCALAR_CONTINUOUS_MASK(AddsImpl, float, vector_f32, plt_b32, 8, vadds)

// Adds::Level 2
template <typename T, bool isSetMask = true>
__aicore__ inline void AddsImpl(__ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

#define ASC_M310_DEFINE_SCALAR_COUNT(                                                                 \
    Function, DataType, VectorType, ElementCount, MaskIntrinsic, VectorIntrinsic, NORM_B16Arg)        \
    template <>                                                                                       \
    __aicore__ inline void Function<DataType, true>(                                                  \
        __ubuf__ DataType * dst, __ubuf__ DataType * src, DataType scalarValue, const int32_t& count) \
    {                                                                                                 \
        __VEC_SCOPE__                                                                                 \
        {                                                                                             \
            ASC_M310_STAGE_SCALAR_INIT_REGS(VectorType, count, ElementCount);                         \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                        \
                preg = MaskIntrinsic(sreg, POST_UPDATE);                                              \
                vlds(vreg0, src, i* sregLower, NORM);                                                 \
                VectorIntrinsic(vreg1, vreg0, scalarValue, preg, MODE_ZEROING);                       \
                vsts(vreg1, dst, i* sregLower, NORM_B16Arg, preg);                                    \
            }                                                                                         \
        }                                                                                             \
    }

ASC_M310_DEFINE_SCALAR_COUNT(AddsImpl, half, vector_f16, 128, plt_b16, vadds, NORM_B16)
ASC_M310_DEFINE_SCALAR_COUNT(AddsImpl, float, vector_f32, 64, plt_b32, vadds, NORM_B32)
ASC_M310_DEFINE_SCALAR_COUNT(AddsImpl, int16_t, vector_s16, 128, plt_b16, vadds, NORM_B16)
ASC_M310_DEFINE_SCALAR_COUNT(AddsImpl, int32_t, vector_s32, 64, plt_b32, vadds, NORM_B32)

/* **************************************************************************************************
 * Muls                                             *
 * ************************************************************************************************* */
// Muls::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void MulsImpl(
    __ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_SCALAR_BITMASK_B16(MulsImpl, half, vector_f16, vmuls)
ASC_M310_DEFINE_SCALAR_BITMASK_B32(MulsImpl, float, vector_f32, vmuls)
ASC_M310_DEFINE_SCALAR_BITMASK_B32(MulsImpl, int32_t, vector_s32, vmuls)
ASC_M310_DEFINE_SCALAR_BITMASK_B16(MulsImpl, int16_t, vector_s16, vmuls)

template <typename T, bool isSetMask = true>
__aicore__ inline void MulsImpl(
    __ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_SCALAR_CONTINUOUS_MASK(MulsImpl, half, vector_f16, plt_b16, 16, vmuls)
ASC_M310_DEFINE_SCALAR_CONTINUOUS_MASK(MulsImpl, int16_t, vector_s16, plt_b16, 16, vmuls)
ASC_M310_DEFINE_SCALAR_CONTINUOUS_MASK(MulsImpl, int32_t, vector_s32, plt_b32, 8, vmuls)
ASC_M310_DEFINE_SCALAR_CONTINUOUS_MASK(MulsImpl, float, vector_f32, plt_b32, 8, vmuls)

// Muls::Level 2
template <typename T, bool isSetMask = true>
__aicore__ inline void MulsImpl(__ubuf__ T* dst, __ubuf__ T* src, const T scalarValue, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_SCALAR_COUNT(MulsImpl, half, vector_f16, 128, plt_b16, vmuls, NORM_B16)
ASC_M310_DEFINE_SCALAR_COUNT(MulsImpl, float, vector_f32, 64, plt_b32, vmuls, NORM_B32)
ASC_M310_DEFINE_SCALAR_COUNT(MulsImpl, int16_t, vector_s16, 128, plt_b16, vmuls, NORM_B16)
ASC_M310_DEFINE_SCALAR_COUNT(MulsImpl, int32_t, vector_s32, 64, plt_b32, vmuls, NORM_B32)

/* **************************************************************************************************
 * Maxs                                             *
 * ************************************************************************************************* */
// Maxs::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void MaxsImpl(
    __ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_SCALAR_BITMASK_B16(MaxsImpl, half, vector_f16, vmaxs)
ASC_M310_DEFINE_SCALAR_BITMASK_B32(MaxsImpl, float, vector_f32, vmaxs)
ASC_M310_DEFINE_SCALAR_BITMASK_B32(MaxsImpl, int32_t, vector_s32, vmaxs)
ASC_M310_DEFINE_SCALAR_BITMASK_B16(MaxsImpl, int16_t, vector_s16, vmaxs)

template <typename T, bool isSetMask = true>
__aicore__ inline void MaxsImpl(
    __ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_SCALAR_CONTINUOUS_MASK(MaxsImpl, half, vector_f16, plt_b16, 16, vmaxs)
ASC_M310_DEFINE_SCALAR_CONTINUOUS_MASK(MaxsImpl, int16_t, vector_s16, plt_b16, 16, vmaxs)
ASC_M310_DEFINE_SCALAR_CONTINUOUS_MASK(MaxsImpl, int32_t, vector_s32, plt_b32, 8, vmaxs)
ASC_M310_DEFINE_SCALAR_CONTINUOUS_MASK(MaxsImpl, float, vector_f32, plt_b32, 8, vmaxs)

// Maxs::Level 2
template <typename T, bool isSetMask = true>
__aicore__ inline void MaxsImpl(__ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_SCALAR_COUNT(MaxsImpl, half, vector_f16, 128, plt_b16, vmaxs, NORM_B16)
ASC_M310_DEFINE_SCALAR_COUNT(MaxsImpl, float, vector_f32, 64, plt_b32, vmaxs, NORM_B32)
ASC_M310_DEFINE_SCALAR_COUNT(MaxsImpl, int16_t, vector_s16, 128, plt_b16, vmaxs, NORM_B16)
ASC_M310_DEFINE_SCALAR_COUNT(MaxsImpl, int32_t, vector_s32, 64, plt_b32, vmaxs, NORM_B32)

/* **************************************************************************************************
 * Mins                                             *
 * ************************************************************************************************* */
// Mins::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void MinsImpl(
    __ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_SCALAR_BITMASK_B16(MinsImpl, half, vector_f16, vmins)
ASC_M310_DEFINE_SCALAR_BITMASK_B32(MinsImpl, float, vector_f32, vmins)
ASC_M310_DEFINE_SCALAR_BITMASK_B32(MinsImpl, int32_t, vector_s32, vmins)
ASC_M310_DEFINE_SCALAR_BITMASK_B16(MinsImpl, int16_t, vector_s16, vmins)

template <typename T, bool isSetMask = true>
__aicore__ inline void MinsImpl(
    __ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_SCALAR_CONTINUOUS_MASK(MinsImpl, half, vector_f16, plt_b16, 16, vmins)
ASC_M310_DEFINE_SCALAR_CONTINUOUS_MASK(MinsImpl, int16_t, vector_s16, plt_b16, 16, vmins)
ASC_M310_DEFINE_SCALAR_CONTINUOUS_MASK(MinsImpl, int32_t, vector_s32, plt_b32, 8, vmins)
ASC_M310_DEFINE_SCALAR_CONTINUOUS_MASK(MinsImpl, float, vector_f32, plt_b32, 8, vmins)

// Mins::Level 2
template <typename T, bool isSetMask = true>
__aicore__ inline void MinsImpl(__ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_SCALAR_COUNT(MinsImpl, half, vector_f16, 128, plt_b16, vmins, NORM_B16)
ASC_M310_DEFINE_SCALAR_COUNT(MinsImpl, float, vector_f32, 64, plt_b32, vmins, NORM_B32)
ASC_M310_DEFINE_SCALAR_COUNT(MinsImpl, int16_t, vector_s16, 128, plt_b16, vmins, NORM_B16)
ASC_M310_DEFINE_SCALAR_COUNT(MinsImpl, int32_t, vector_s32, 64, plt_b32, vmins, NORM_B32)

/* **************************************************************************************************
 * ShiftLeft                                             *
 * ************************************************************************************************* */
// ShiftLeft::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void ShiftLeftImpl(
    __ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

template <>
__aicore__ inline void ShiftLeftImpl<uint16_t, true>(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint16_t scalarValue, const uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
    __ubuf__ uint8_t* tmpBuf = AscendCUtils::GetTemporaryBufferAddr<uint8_t>(TMP_UB_OFFSET, 16);
    *((__ubuf__ uint64_t*)tmpBuf) = mask[0];
    *((__ubuf__ uint64_t*)tmpBuf + 1) = mask[1];
    event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
    SetFlag<HardEvent::S_V>(eventIdSToV);
    WaitFlag<HardEvent::S_V>(eventIdSToV);
    __VEC_SCOPE__
    {
        ASC_M310_STAGE_SCALAR_BITMASK_AND_STRIDES(vector_u16, tmpBuf, repeatParams);
        for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {
            vsldb(vreg0, src + i * repeatParams.srcRepStride * 16, strideConfig0, preg);
            vshls(vreg1, vreg0, (int16_t)scalarValue, preg, MODE_ZEROING);
            vsstb(vreg1, dst + i * repeatParams.dstRepStride * 16, strideConfig1, preg);
        }
    }
    AscendCUtils::FreeTemporaryBuffer<uint8_t>(tmpBuf);
}

#define ASC_M310_DEFINE_SHIFT_LEFT_BITMASK_B32(DataType, VectorType)                                   \
    template <>                                                                                        \
    __aicore__ inline void ShiftLeftImpl<DataType, true>(                                              \
        __ubuf__ DataType * dst, __ubuf__ DataType * src, DataType scalarValue, const uint64_t mask[], \
        const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)                               \
    {                                                                                                  \
        __ubuf__ uint8_t* tmpBuf = AscendCUtils::GetTemporaryBufferAddr<uint8_t>(TMP_UB_OFFSET, 16);   \
        *((__ubuf__ uint64_t*)tmpBuf) = mask[0];                                                       \
        *((__ubuf__ uint64_t*)tmpBuf + 1) = mask[1];                                                   \
        event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));       \
        SetFlag<HardEvent::S_V>(eventIdSToV);                                                          \
        WaitFlag<HardEvent::S_V>(eventIdSToV);                                                         \
        __VEC_SCOPE__                                                                                  \
        {                                                                                              \
            ASC_M310_STAGE_SCALAR_COUNT_MASK_AND_STRIDES(                                              \
                VectorType, vreg0, vreg1, vector_bool, preg, tmpBuf, repeatParams);                    \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                         \
                vsldb(vreg0, src + i * repeatParams.srcRepStride * 8, strideConfig0, preg1);           \
                vshls(vreg1, vreg0, (int16_t)scalarValue, preg1, MODE_ZEROING);                        \
                vsstb(vreg1, dst + i * repeatParams.dstRepStride * 8, strideConfig1, preg1);           \
            }                                                                                          \
        }                                                                                              \
        AscendCUtils::FreeTemporaryBuffer<uint8_t>(tmpBuf);                                            \
    }

ASC_M310_DEFINE_SHIFT_LEFT_BITMASK_B32(uint32_t, vector_u32)
ASC_M310_DEFINE_SHIFT_LEFT_BITMASK_B32(int32_t, vector_s32)

#undef ASC_M310_DEFINE_SHIFT_LEFT_BITMASK_B32

ASC_M310_DEFINE_SCALAR_BITMASK_B16(ShiftLeftImpl, int16_t, vector_s16, vshls)

template <typename T, bool isSetMask = true>
__aicore__ inline void ShiftLeftImpl(
    __ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

#define ASC_M310_DEFINE_SHIFT_LEFT_CONTINUOUS_MASK(DataType, VectorType, MaskIntrinsic, ElementCount)  \
    template <>                                                                                        \
    __aicore__ inline void ShiftLeftImpl<DataType, true>(                                              \
        __ubuf__ DataType * dst, __ubuf__ DataType * src, DataType scalarValue, const uint64_t mask,   \
        const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)                               \
    {                                                                                                  \
        __VEC_SCOPE__                                                                                  \
        {                                                                                              \
            ASC_M310_STAGE_SCALAR_CONTIGUOUS_INIT_REGS(VectorType, mask, MaskIntrinsic, repeatParams); \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                         \
                vsldb(vreg0, src + i * repeatParams.srcRepStride * ElementCount, strideConfig0, preg); \
                vshls(vreg1, vreg0, (int16_t)scalarValue, preg, MODE_ZEROING);                         \
                vsstb(vreg1, dst + i * repeatParams.dstRepStride * ElementCount, strideConfig1, preg); \
            }                                                                                          \
        }                                                                                              \
    }

ASC_M310_DEFINE_SHIFT_LEFT_CONTINUOUS_MASK(uint16_t, vector_u16, plt_b16, 16)
ASC_M310_DEFINE_SCALAR_CONTINUOUS_MASK(ShiftLeftImpl, int16_t, vector_s16, plt_b16, 16, vshls)
ASC_M310_DEFINE_SHIFT_LEFT_CONTINUOUS_MASK(int32_t, vector_s32, plt_b32, 8)
ASC_M310_DEFINE_SHIFT_LEFT_CONTINUOUS_MASK(uint32_t, vector_u32, plt_b32, 8)

#undef ASC_M310_DEFINE_SHIFT_LEFT_CONTINUOUS_MASK

// ShiftLeft::Level 2
template <typename T, bool isSetMask = true>
__aicore__ inline void ShiftLeftImpl(__ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

#define ASC_M310_DEFINE_SHIFT_COUNT(                                                                  \
    Function, DataType, VectorType, ElementCount, MaskIntrinsic, VectorIntrinsic, NORM_B16Arg)        \
    template <>                                                                                       \
    __aicore__ inline void Function<DataType, true>(                                                  \
        __ubuf__ DataType * dst, __ubuf__ DataType * src, DataType scalarValue, const int32_t& count) \
    {                                                                                                 \
        __VEC_SCOPE__                                                                                 \
        {                                                                                             \
            ASC_M310_STAGE_SCALAR_INIT_REGS(VectorType, count, ElementCount);                         \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                        \
                preg = MaskIntrinsic(sreg, POST_UPDATE);                                              \
                vlds(vreg0, src, i* sregLower, NORM);                                                 \
                VectorIntrinsic(vreg1, vreg0, (int16_t)scalarValue, preg, MODE_ZEROING);              \
                vsts(vreg1, dst, i* sregLower, NORM_B16Arg, preg);                                    \
            }                                                                                         \
        }                                                                                             \
    }

ASC_M310_DEFINE_SHIFT_COUNT(ShiftLeftImpl, uint16_t, vector_u16, 128, plt_b16, vshls, NORM_B16)
ASC_M310_DEFINE_SHIFT_COUNT(ShiftLeftImpl, uint32_t, vector_u32, 64, plt_b32, vshls, NORM_B32)
ASC_M310_DEFINE_SCALAR_COUNT(ShiftLeftImpl, int16_t, vector_s16, 128, plt_b16, vshls, NORM_B16)
ASC_M310_DEFINE_SHIFT_COUNT(ShiftLeftImpl, int32_t, vector_s32, 64, plt_b32, vshls, NORM_B32)

/* **************************************************************************************************
 * ShiftRight                                             *
 * ************************************************************************************************* */
// ShiftRight::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void ShiftRightImpl(
    __ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams, bool roundEn)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

template <>
__aicore__ inline void ShiftRightImpl<uint16_t, true>(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint16_t scalarValue, const uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams, bool roundEn)
{
    __ubuf__ uint8_t* tmpBuf = AscendCUtils::GetTemporaryBufferAddr<uint8_t>(TMP_UB_OFFSET, 16);
    *((__ubuf__ uint64_t*)tmpBuf) = mask[0];
    *((__ubuf__ uint64_t*)tmpBuf + 1) = mask[1];
    event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
    SetFlag<HardEvent::S_V>(eventIdSToV);
    WaitFlag<HardEvent::S_V>(eventIdSToV);
    __VEC_SCOPE__
    {
        ASC_M310_STAGE_SCALAR_BITMASK_AND_STRIDES(vector_u16, tmpBuf, repeatParams);
        for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {
            vsldb(vreg0, src + i * repeatParams.srcRepStride * 16, strideConfig0, preg);
            vshrs(vreg1, vreg0, (int16_t)scalarValue, preg, MODE_ZEROING);
            vsstb(vreg1, dst + i * repeatParams.dstRepStride * 16, strideConfig1, preg);
        }
    }
    AscendCUtils::FreeTemporaryBuffer<uint8_t>(tmpBuf);
}

#define ASC_M310_DEFINE_SHIFT_RIGHT_BITMASK_B32(DataType, VectorType)                                  \
    template <>                                                                                        \
    __aicore__ inline void ShiftRightImpl<DataType, true>(                                             \
        __ubuf__ DataType * dst, __ubuf__ DataType * src, DataType scalarValue, const uint64_t mask[], \
        const uint8_t repeatTime, const UnaryRepeatParams& repeatParams, bool roundEn)                 \
    {                                                                                                  \
        __ubuf__ uint8_t* tmpBuf = AscendCUtils::GetTemporaryBufferAddr<uint8_t>(TMP_UB_OFFSET, 16);   \
        *((__ubuf__ uint64_t*)tmpBuf) = mask[0];                                                       \
        *((__ubuf__ uint64_t*)tmpBuf + 1) = mask[1];                                                   \
        event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));       \
        SetFlag<HardEvent::S_V>(eventIdSToV);                                                          \
        WaitFlag<HardEvent::S_V>(eventIdSToV);                                                         \
        __VEC_SCOPE__                                                                                  \
        {                                                                                              \
            ASC_M310_STAGE_SCALAR_COUNT_MASK_AND_STRIDES(                                              \
                VectorType, vreg0, vreg1, vector_bool, preg, tmpBuf, repeatParams);                    \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                         \
                vsldb(vreg0, src + i * repeatParams.srcRepStride * 8, strideConfig0, preg1);           \
                vshrs(vreg1, vreg0, (int16_t)scalarValue, preg1, MODE_ZEROING);                        \
                vsstb(vreg1, dst + i * repeatParams.dstRepStride * 8, strideConfig1, preg1);           \
            }                                                                                          \
        }                                                                                              \
        AscendCUtils::FreeTemporaryBuffer<uint8_t>(tmpBuf);                                            \
    }

ASC_M310_DEFINE_SHIFT_RIGHT_BITMASK_B32(uint32_t, vector_u32)
ASC_M310_DEFINE_SHIFT_RIGHT_BITMASK_B32(int32_t, vector_s32)

#undef ASC_M310_DEFINE_SHIFT_RIGHT_BITMASK_B32

template <>
__aicore__ inline void ShiftRightImpl<int16_t, true>(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t scalarValue, const uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams, bool roundEn)
{
    __ubuf__ uint8_t* tmpBuf = AscendCUtils::GetTemporaryBufferAddr<uint8_t>(TMP_UB_OFFSET, 16);
    *((__ubuf__ uint64_t*)tmpBuf) = mask[0];
    *((__ubuf__ uint64_t*)tmpBuf + 1) = mask[1];
    event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
    SetFlag<HardEvent::S_V>(eventIdSToV);
    WaitFlag<HardEvent::S_V>(eventIdSToV);
    __VEC_SCOPE__
    {
        ASC_M310_STAGE_SCALAR_BITMASK_AND_STRIDES(vector_s16, tmpBuf, repeatParams);
        for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {
            vsldb(vreg0, src + i * repeatParams.srcRepStride * 16, strideConfig0, preg);
            vshrs(vreg1, vreg0, scalarValue, preg, MODE_ZEROING);
            vsstb(vreg1, dst + i * repeatParams.dstRepStride * 16, strideConfig1, preg);
        }
    }
    AscendCUtils::FreeTemporaryBuffer<uint8_t>(tmpBuf);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void ShiftRightImpl(
    __ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams, bool roundEn)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

#define ASC_M310_DEFINE_SHIFT_RIGHT_CONTINUOUS_MASK(DataType, VectorType, MaskIntrinsic, ElementCount) \
    template <>                                                                                        \
    __aicore__ inline void ShiftRightImpl<DataType, true>(                                             \
        __ubuf__ DataType * dst, __ubuf__ DataType * src, DataType scalarValue, const uint64_t mask,   \
        const uint8_t repeatTime, const UnaryRepeatParams& repeatParams, bool roundEn)                 \
    {                                                                                                  \
        __VEC_SCOPE__                                                                                  \
        {                                                                                              \
            ASC_M310_STAGE_SCALAR_CONTIGUOUS_INIT_REGS(VectorType, mask, MaskIntrinsic, repeatParams); \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                         \
                vsldb(vreg0, src + i * repeatParams.srcRepStride * ElementCount, strideConfig0, preg); \
                vshrs(vreg1, vreg0, (int16_t)scalarValue, preg, MODE_ZEROING);                         \
                vsstb(vreg1, dst + i * repeatParams.dstRepStride * ElementCount, strideConfig1, preg); \
            }                                                                                          \
        }                                                                                              \
    }

ASC_M310_DEFINE_SHIFT_RIGHT_CONTINUOUS_MASK(uint16_t, vector_u16, plt_b16, 16)

template <>
__aicore__ inline void ShiftRightImpl<int16_t, true>(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t scalarValue, const uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams, bool roundEn)
{
    __VEC_SCOPE__
    {
        vector_s16 vreg0;
        vector_s16 vreg1;
        uint32_t sreg = static_cast<uint32_t>(mask);
        vector_bool preg = plt_b16(sreg, POST_UPDATE);
        uint32_t strideConfig0 = ((static_cast<uint32_t>(repeatParams.srcBlkStride)) << 16);
        uint32_t strideConfig1 = ((static_cast<uint32_t>(repeatParams.dstBlkStride)) << 16);
        for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {
            vsldb(vreg0, src + i * repeatParams.srcRepStride * 16, strideConfig0, preg);
            vshrs(vreg1, vreg0, scalarValue, preg, MODE_ZEROING);
            vsstb(vreg1, dst + i * repeatParams.dstRepStride * 16, strideConfig1, preg);
        }
    }
}

ASC_M310_DEFINE_SHIFT_RIGHT_CONTINUOUS_MASK(int32_t, vector_s32, plt_b32, 8)
ASC_M310_DEFINE_SHIFT_RIGHT_CONTINUOUS_MASK(uint32_t, vector_u32, plt_b32, 8)

#undef ASC_M310_DEFINE_SHIFT_RIGHT_CONTINUOUS_MASK

// ShiftLeft::Level 2
template <typename T, bool isSetMask = true>
__aicore__ inline void ShiftRightImpl(__ubuf__ T* dst, __ubuf__ T* src, const T scalarValue, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_SHIFT_COUNT(ShiftRightImpl, uint16_t, vector_u16, 128, plt_b16, vshrs, NORM_B16)
ASC_M310_DEFINE_SHIFT_COUNT(ShiftRightImpl, uint32_t, vector_u32, 64, plt_b32, vshrs, NORM_B32)
ASC_M310_DEFINE_SCALAR_COUNT(ShiftRightImpl, int16_t, vector_s16, 128, plt_b16, vshrs, NORM_B16)
ASC_M310_DEFINE_SHIFT_COUNT(ShiftRightImpl, int32_t, vector_s32, 64, plt_b32, vshrs, NORM_B32)

#undef ASC_M310_DEFINE_SHIFT_COUNT

/* **************************************************************************************************
 * LeakyRelu                                             *
 * ************************************************************************************************* */
// LeakyRelu::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void LeakyReluImpl(
    __ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const uint64_t mask[], uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_SCALAR_BITMASK_B32(LeakyReluImpl, float, vector_f32, vlrelu)

#undef ASC_M310_DEFINE_SCALAR_BITMASK_B32

ASC_M310_DEFINE_SCALAR_BITMASK_B16(LeakyReluImpl, half, vector_f16, vlrelu)

#undef ASC_M310_DEFINE_SCALAR_BITMASK_B16

template <typename T, bool isSetMask = true>
__aicore__ inline void LeakyReluImpl(
    __ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const uint64_t mask, uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_SCALAR_CONTINUOUS_MASK(LeakyReluImpl, float, vector_f32, plt_b32, 8, vlrelu)
ASC_M310_DEFINE_SCALAR_CONTINUOUS_MASK(LeakyReluImpl, half, vector_f16, plt_b16, 16, vlrelu)

#undef ASC_M310_DEFINE_SCALAR_CONTINUOUS_MASK

// LeakyRelu::Level 2
template <typename T, bool isSetMask = true>
__aicore__ inline void LeakyReluImpl(__ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_SCALAR_COUNT(LeakyReluImpl, float, vector_f32, 64, plt_b32, vlrelu, NORM_B32)
ASC_M310_DEFINE_SCALAR_COUNT(LeakyReluImpl, half, vector_f16, 128, plt_b16, vlrelu, NORM_B16)

#undef ASC_M310_DEFINE_SCALAR_COUNT
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_BINARY_SCALAR_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_SCALAR_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_SCALAR_IMPL_H__
#endif

#undef ASC_M310_STAGE_SCALAR_CONTIGUOUS_INIT_REGS
#undef ASC_M310_STAGE_SCALAR_COUNT_MASK_AND_STRIDES
#undef ASC_M310_STAGE_SCALAR_BITMASK_AND_STRIDES

#undef ASC_M310_STAGE_SCALAR_INIT_REGS
