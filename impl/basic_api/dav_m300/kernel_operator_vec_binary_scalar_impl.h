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
    "impl/basic_api/dav_m300/kernel_operator_vec_binary_scalar_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_vec_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_SCALAR_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_BINARY_SCALAR_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_BINARY_SCALAR_IMPL_H
#include "../kernel_utils.h"
#include "../../../include/basic_api/kernel_common.h"
#include "kernel_operator_common_impl.h"
#include "../../../include/basic_api/kernel_struct_unary.h"

// Expand shared register stages in place to preserve the compiler input and instruction order.
#define ASC_M300_STAGE_SCALAR_BITMASK_AND_STRIDES(VectorType, MaskIntrinsic, repeatParams) \
    VectorType vreg0;                                                                      \
    VectorType vreg1;                                                                      \
    vector_bool preg;                                                                      \
    preg = MaskIntrinsic();                                                                \
    uint32_t strideConfig0 = ((static_cast<uint32_t>(repeatParams.srcBlkStride)) << 16);   \
    uint32_t strideConfig1 = ((static_cast<uint32_t>(repeatParams.dstBlkStride)) << 16)

#define ASC_M300_STAGE_SCALAR_COUNT_MASK_AND_STRIDES(VectorType, mask, MaskIntrinsic, repeatParams) \
    VectorType vreg0;                                                                               \
    VectorType vreg1;                                                                               \
    uint32_t sreg = static_cast<uint32_t>(mask);                                                    \
    vector_bool preg = MaskIntrinsic(sreg, POST_UPDATE);                                            \
    uint32_t strideConfig0 = ((static_cast<uint32_t>(repeatParams.srcBlkStride)) << 16);            \
    uint32_t strideConfig1 = ((static_cast<uint32_t>(repeatParams.dstBlkStride)) << 16)

#define ASC_M300_STAGE_SCALAR_CONTIGUOUS_INIT_REGS(VectorType, count, ElementCount) \
    VectorType vreg0;                                                               \
    VectorType vreg1;                                                               \
    uint32_t sreg = static_cast<uint32_t>(count);                                   \
    vector_bool preg;                                                               \
    uint32_t sregLower = static_cast<uint32_t>(ElementCount);                       \
    uint16_t repeatTime = CeilDivision(count, ElementCount)

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
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

// Emit the original overloads with their existing vector scope and intrinsic order.
#define ASC_M300_DEFINE_SCALAR_1(DataType, Function, VectorType, MaskIntrinsic, ElementCount, VectorIntrinsic) \
    template <typename T = DataType, bool isSetMask = true>                                                    \
    __aicore__ inline void Function(                                                                           \
        __ubuf__ DataType* dst, __ubuf__ DataType* src, DataType scalarValue, const uint64_t mask[],           \
        const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)                                       \
    {                                                                                                          \
        if constexpr (isSetMask) {                                                                             \
            SetVectorMask<T>(mask[1], mask[0]);                                                                \
        }                                                                                                      \
        __VEC_SCOPE__                                                                                          \
        {                                                                                                      \
            ASC_M300_STAGE_SCALAR_BITMASK_AND_STRIDES(VectorType, MaskIntrinsic, repeatParams);                \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                                 \
                vsldb(vreg0, src + i * repeatParams.srcRepStride * ElementCount, strideConfig0, preg);         \
                VectorIntrinsic(vreg1, vreg0, scalarValue, preg, MODE_ZEROING);                                \
                vsstb(vreg1, dst + i * repeatParams.dstRepStride * ElementCount, strideConfig1, preg);         \
            }                                                                                                  \
        }                                                                                                      \
    }

ASC_M300_DEFINE_SCALAR_1(half, AddsImpl, vector_f16, movp_b16, 16, vadds)
ASC_M300_DEFINE_SCALAR_1(float, AddsImpl, vector_f32, movp_b32, 8, vadds)
ASC_M300_DEFINE_SCALAR_1(int32_t, AddsImpl, vector_s32, movp_b32, 8, vadds)
ASC_M300_DEFINE_SCALAR_1(int16_t, AddsImpl, vector_s16, movp_b16, 16, vadds)

template <typename T, bool isSetMask = true>
__aicore__ inline void AddsImpl(
    __ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_SCALAR_2(DataType, Function, VectorType, MaskIntrinsic, ElementCount, VectorIntrinsic) \
    template <typename T = DataType, bool isSetMask = true>                                                    \
    __aicore__ inline void Function(                                                                           \
        __ubuf__ DataType* dst, __ubuf__ DataType* src, DataType scalarValue, const uint64_t mask,             \
        const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)                                       \
    {                                                                                                          \
        __VEC_SCOPE__                                                                                          \
        {                                                                                                      \
            ASC_M300_STAGE_SCALAR_COUNT_MASK_AND_STRIDES(VectorType, mask, MaskIntrinsic, repeatParams);       \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                                 \
                vsldb(vreg0, src + i * repeatParams.srcRepStride * ElementCount, strideConfig0, preg);         \
                VectorIntrinsic(vreg1, vreg0, scalarValue, preg, MODE_ZEROING);                                \
                vsstb(vreg1, dst + i * repeatParams.dstRepStride * ElementCount, strideConfig1, preg);         \
            }                                                                                                  \
        }                                                                                                      \
    }

ASC_M300_DEFINE_SCALAR_2(half, AddsImpl, vector_f16, plt_b16, 16, vadds)
ASC_M300_DEFINE_SCALAR_2(int16_t, AddsImpl, vector_s16, plt_b16, 16, vadds)
ASC_M300_DEFINE_SCALAR_2(int32_t, AddsImpl, vector_s32, plt_b32, 8, vadds)
ASC_M300_DEFINE_SCALAR_2(float, AddsImpl, vector_f32, plt_b32, 8, vadds)

// Adds::Level 2
template <typename T, bool isSetMask = true>
__aicore__ inline void AddsImpl(__ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_SCALAR_3(                                                                   \
    DataType, Function, VectorType, ElementCount, MaskIntrinsic, VectorIntrinsic, NORM_B16Arg)      \
    template <typename T = DataType, bool isSetMask = true>                                         \
    __aicore__ inline void Function(                                                                \
        __ubuf__ DataType* dst, __ubuf__ DataType* src, DataType scalarValue, const int32_t& count) \
    {                                                                                               \
        __VEC_SCOPE__                                                                               \
        {                                                                                           \
            ASC_M300_STAGE_SCALAR_CONTIGUOUS_INIT_REGS(VectorType, count, ElementCount);            \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                      \
                preg = MaskIntrinsic(sreg, POST_UPDATE);                                            \
                vlds(vreg0, src, i* sregLower, NORM);                                               \
                VectorIntrinsic(vreg1, vreg0, scalarValue, preg, MODE_ZEROING);                     \
                vsts(vreg1, dst, i* sregLower, NORM_B16Arg, preg);                                  \
            }                                                                                       \
        }                                                                                           \
    }

ASC_M300_DEFINE_SCALAR_3(half, AddsImpl, vector_f16, 128, plt_b16, vadds, NORM_B16)
ASC_M300_DEFINE_SCALAR_3(float, AddsImpl, vector_f32, 64, plt_b32, vadds, NORM_B32)
ASC_M300_DEFINE_SCALAR_3(int16_t, AddsImpl, vector_s16, 128, plt_b16, vadds, NORM_B16)
ASC_M300_DEFINE_SCALAR_3(int32_t, AddsImpl, vector_s32, 64, plt_b32, vadds, NORM_B32)

/* **************************************************************************************************
 * Muls                                             *
 * ************************************************************************************************* */
// Muls::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void MulsImpl(
    __ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_SCALAR_1(half, MulsImpl, vector_f16, movp_b16, 16, vmuls)

#define ASC_M300_DEFINE_SCALAR_4(DataType, VectorType)                                               \
    template <typename T = DataType, bool isSetMask = true>                                          \
    __aicore__ inline void MulsImpl(                                                                 \
        __ubuf__ DataType* dst, __ubuf__ DataType* src, DataType scalarValue, const uint64_t mask[], \
        const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)                             \
    {                                                                                                \
        if constexpr (isSetMask) {                                                                   \
            SetVectorMask<T>(mask[1], mask[0]);                                                      \
        }                                                                                            \
        __VEC_SCOPE__                                                                                \
        {                                                                                            \
            VectorType vreg0;                                                                        \
            VectorType vreg1;                                                                        \
            vector_bool preg;                                                                        \
            vector_bool preg1;                                                                       \
            preg = movp_b32();                                                                       \
            uint32_t strideConfig0 = ((static_cast<uint32_t>(repeatParams.srcBlkStride)) << 16);     \
            uint32_t strideConfig1 = ((static_cast<uint32_t>(repeatParams.dstBlkStride)) << 16);     \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                       \
                vsldb(vreg0, src + i * repeatParams.srcRepStride * 8, strideConfig0, preg);          \
                vmuls(vreg1, vreg0, scalarValue, preg, MODE_ZEROING);                                \
                vsstb(vreg1, dst + i * repeatParams.dstRepStride * 8, strideConfig1, preg);          \
            }                                                                                        \
        }                                                                                            \
    }

ASC_M300_DEFINE_SCALAR_4(float, vector_f32)
ASC_M300_DEFINE_SCALAR_4(int32_t, vector_s32)

#undef ASC_M300_DEFINE_SCALAR_4

ASC_M300_DEFINE_SCALAR_1(int16_t, MulsImpl, vector_s16, movp_b16, 16, vmuls)

template <typename T, bool isSetMask = true>
__aicore__ inline void MulsImpl(
    __ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_SCALAR_2(half, MulsImpl, vector_f16, plt_b16, 16, vmuls)
ASC_M300_DEFINE_SCALAR_2(int16_t, MulsImpl, vector_s16, plt_b16, 16, vmuls)
ASC_M300_DEFINE_SCALAR_2(int32_t, MulsImpl, vector_s32, plt_b32, 8, vmuls)
ASC_M300_DEFINE_SCALAR_2(float, MulsImpl, vector_f32, plt_b32, 8, vmuls)

// Muls::Level 2
template <typename T, bool isSetMask = true>
__aicore__ inline void MulsImpl(__ubuf__ T* dst, __ubuf__ T* src, const T scalarValue, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_SCALAR_3(half, MulsImpl, vector_f16, 128, plt_b16, vmuls, NORM_B16)
ASC_M300_DEFINE_SCALAR_3(float, MulsImpl, vector_f32, 64, plt_b32, vmuls, NORM_B32)
ASC_M300_DEFINE_SCALAR_3(int16_t, MulsImpl, vector_s16, 128, plt_b16, vmuls, NORM_B16)
ASC_M300_DEFINE_SCALAR_3(int32_t, MulsImpl, vector_s32, 64, plt_b32, vmuls, NORM_B32)

/* **************************************************************************************************
 * Maxs                                             *
 * ************************************************************************************************* */
// Maxs::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void MaxsImpl(
    __ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_SCALAR_1(half, MaxsImpl, vector_f16, movp_b16, 16, vmaxs)
ASC_M300_DEFINE_SCALAR_1(float, MaxsImpl, vector_f32, movp_b32, 8, vmaxs)
ASC_M300_DEFINE_SCALAR_1(int32_t, MaxsImpl, vector_s32, movp_b32, 8, vmaxs)
ASC_M300_DEFINE_SCALAR_1(int16_t, MaxsImpl, vector_s16, movp_b16, 16, vmaxs)

template <typename T, bool isSetMask = true>
__aicore__ inline void MaxsImpl(
    __ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_SCALAR_2(half, MaxsImpl, vector_f16, plt_b16, 16, vmaxs)
ASC_M300_DEFINE_SCALAR_2(int16_t, MaxsImpl, vector_s16, plt_b16, 16, vmaxs)
ASC_M300_DEFINE_SCALAR_2(int32_t, MaxsImpl, vector_s32, plt_b32, 8, vmaxs)
ASC_M300_DEFINE_SCALAR_2(float, MaxsImpl, vector_f32, plt_b32, 8, vmaxs)

// Maxs::Level 2
template <typename T, bool isSetMask = true>
__aicore__ inline void MaxsImpl(__ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_SCALAR_3(half, MaxsImpl, vector_f16, 128, plt_b16, vmaxs, NORM_B16)
ASC_M300_DEFINE_SCALAR_3(float, MaxsImpl, vector_f32, 64, plt_b32, vmaxs, NORM_B32)
ASC_M300_DEFINE_SCALAR_3(int16_t, MaxsImpl, vector_s16, 128, plt_b16, vmaxs, NORM_B16)
ASC_M300_DEFINE_SCALAR_3(int32_t, MaxsImpl, vector_s32, 64, plt_b32, vmaxs, NORM_B32)

/* **************************************************************************************************
 * Mins                                             *
 * ************************************************************************************************* */
// Mins::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void MinsImpl(
    __ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_SCALAR_1(half, MinsImpl, vector_f16, movp_b16, 16, vmins)
ASC_M300_DEFINE_SCALAR_1(float, MinsImpl, vector_f32, movp_b32, 8, vmins)
ASC_M300_DEFINE_SCALAR_1(int32_t, MinsImpl, vector_s32, movp_b32, 8, vmins)
ASC_M300_DEFINE_SCALAR_1(int16_t, MinsImpl, vector_s16, movp_b16, 16, vmins)

template <typename T, bool isSetMask = true>
__aicore__ inline void MinsImpl(
    __ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_SCALAR_2(half, MinsImpl, vector_f16, plt_b16, 16, vmins)
ASC_M300_DEFINE_SCALAR_2(int16_t, MinsImpl, vector_s16, plt_b16, 16, vmins)
ASC_M300_DEFINE_SCALAR_2(int32_t, MinsImpl, vector_s32, plt_b32, 8, vmins)
ASC_M300_DEFINE_SCALAR_2(float, MinsImpl, vector_f32, plt_b32, 8, vmins)

// Mins::Level 2
template <typename T, bool isSetMask = true>
__aicore__ inline void MinsImpl(__ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_SCALAR_3(half, MinsImpl, vector_f16, 128, plt_b16, vmins, NORM_B16)
ASC_M300_DEFINE_SCALAR_3(float, MinsImpl, vector_f32, 64, plt_b32, vmins, NORM_B32)
ASC_M300_DEFINE_SCALAR_3(int16_t, MinsImpl, vector_s16, 128, plt_b16, vmins, NORM_B16)
ASC_M300_DEFINE_SCALAR_3(int32_t, MinsImpl, vector_s32, 64, plt_b32, vmins, NORM_B32)

/* **************************************************************************************************
 * ShiftLeft                                             *
 * ************************************************************************************************* */
// ShiftLeft::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void ShiftLeftImpl(
    __ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_SCALAR_5(DataType, VectorType, MaskIntrinsic, ElementCount)                    \
    template <typename T = DataType, bool isSetMask = true>                                            \
    __aicore__ inline void ShiftLeftImpl(                                                              \
        __ubuf__ DataType* dst, __ubuf__ DataType* src, DataType scalarValue, const uint64_t mask[],   \
        const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)                               \
    {                                                                                                  \
        if constexpr (isSetMask) {                                                                     \
            SetVectorMask<T>(mask[1], mask[0]);                                                        \
        }                                                                                              \
        __VEC_SCOPE__                                                                                  \
        {                                                                                              \
            ASC_M300_STAGE_SCALAR_BITMASK_AND_STRIDES(VectorType, MaskIntrinsic, repeatParams);        \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                         \
                vsldb(vreg0, src + i * repeatParams.srcRepStride * ElementCount, strideConfig0, preg); \
                vshls(vreg1, vreg0, (int16_t)scalarValue, preg, MODE_ZEROING);                         \
                vsstb(vreg1, dst + i * repeatParams.dstRepStride * ElementCount, strideConfig1, preg); \
            }                                                                                          \
        }                                                                                              \
    }

ASC_M300_DEFINE_SCALAR_5(uint16_t, vector_u16, movp_b16, 16)
ASC_M300_DEFINE_SCALAR_5(uint32_t, vector_u32, movp_b32, 8)
ASC_M300_DEFINE_SCALAR_5(int32_t, vector_s32, movp_b32, 8)

#undef ASC_M300_DEFINE_SCALAR_5

ASC_M300_DEFINE_SCALAR_1(int16_t, ShiftLeftImpl, vector_s16, movp_b16, 16, vshls)

template <typename T, bool isSetMask = true>
__aicore__ inline void ShiftLeftImpl(
    __ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_SCALAR_6(DataType, VectorType, MaskIntrinsic, ElementCount)                      \
    template <typename T = DataType, bool isSetMask = true>                                              \
    __aicore__ inline void ShiftLeftImpl(                                                                \
        __ubuf__ DataType* dst, __ubuf__ DataType* src, DataType scalarValue, const uint64_t mask,       \
        const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)                                 \
    {                                                                                                    \
        __VEC_SCOPE__                                                                                    \
        {                                                                                                \
            ASC_M300_STAGE_SCALAR_COUNT_MASK_AND_STRIDES(VectorType, mask, MaskIntrinsic, repeatParams); \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                           \
                vsldb(vreg0, src + i * repeatParams.srcRepStride * ElementCount, strideConfig0, preg);   \
                vshls(vreg1, vreg0, (int16_t)scalarValue, preg, MODE_ZEROING);                           \
                vsstb(vreg1, dst + i * repeatParams.dstRepStride * ElementCount, strideConfig1, preg);   \
            }                                                                                            \
        }                                                                                                \
    }

ASC_M300_DEFINE_SCALAR_6(uint16_t, vector_u16, plt_b16, 16)
ASC_M300_DEFINE_SCALAR_2(int16_t, ShiftLeftImpl, vector_s16, plt_b16, 16, vshls)
ASC_M300_DEFINE_SCALAR_6(int32_t, vector_s32, plt_b32, 8)
ASC_M300_DEFINE_SCALAR_6(uint32_t, vector_u32, plt_b32, 8)

#undef ASC_M300_DEFINE_SCALAR_6

// ShiftLeft::Level 2
template <typename T, bool isSetMask = true>
__aicore__ inline void ShiftLeftImpl(__ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_SCALAR_7(                                                                   \
    DataType, Function, VectorType, ElementCount, MaskIntrinsic, VectorIntrinsic, NORM_B16Arg)      \
    template <typename T = DataType, bool isSetMask = true>                                         \
    __aicore__ inline void Function(                                                                \
        __ubuf__ DataType* dst, __ubuf__ DataType* src, DataType scalarValue, const int32_t& count) \
    {                                                                                               \
        __VEC_SCOPE__                                                                               \
        {                                                                                           \
            ASC_M300_STAGE_SCALAR_CONTIGUOUS_INIT_REGS(VectorType, count, ElementCount);            \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                      \
                preg = MaskIntrinsic(sreg, POST_UPDATE);                                            \
                vlds(vreg0, src, i* sregLower, NORM);                                               \
                VectorIntrinsic(vreg1, vreg0, (int16_t)scalarValue, preg, MODE_ZEROING);            \
                vsts(vreg1, dst, i* sregLower, NORM_B16Arg, preg);                                  \
            }                                                                                       \
        }                                                                                           \
    }

ASC_M300_DEFINE_SCALAR_7(uint16_t, ShiftLeftImpl, vector_u16, 128, plt_b16, vshls, NORM_B16)
ASC_M300_DEFINE_SCALAR_7(uint32_t, ShiftLeftImpl, vector_u32, 64, plt_b32, vshls, NORM_B32)
ASC_M300_DEFINE_SCALAR_3(int16_t, ShiftLeftImpl, vector_s16, 128, plt_b16, vshls, NORM_B16)
ASC_M300_DEFINE_SCALAR_7(int32_t, ShiftLeftImpl, vector_s32, 64, plt_b32, vshls, NORM_B32)

/* **************************************************************************************************
 * ShiftRight                                             *
 * ************************************************************************************************* */
// ShiftRight::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void ShiftRightImpl(
    __ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams, bool roundEn)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_SCALAR_8(DataType, VectorType, MaskIntrinsic, ElementCount)                    \
    template <typename T = DataType, bool isSetMask = true>                                            \
    __aicore__ inline void ShiftRightImpl(                                                             \
        __ubuf__ DataType* dst, __ubuf__ DataType* src, DataType scalarValue, const uint64_t mask[],   \
        const uint8_t repeatTime, const UnaryRepeatParams& repeatParams, bool roundEn)                 \
    {                                                                                                  \
        if constexpr (isSetMask) {                                                                     \
            SetVectorMask<T>(mask[1], mask[0]);                                                        \
        }                                                                                              \
        __VEC_SCOPE__                                                                                  \
        {                                                                                              \
            ASC_M300_STAGE_SCALAR_BITMASK_AND_STRIDES(VectorType, MaskIntrinsic, repeatParams);        \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                         \
                vsldb(vreg0, src + i * repeatParams.srcRepStride * ElementCount, strideConfig0, preg); \
                vshrs(vreg1, vreg0, (int16_t)scalarValue, preg, MODE_ZEROING);                         \
                vsstb(vreg1, dst + i * repeatParams.dstRepStride * ElementCount, strideConfig1, preg); \
            }                                                                                          \
        }                                                                                              \
    }

ASC_M300_DEFINE_SCALAR_8(uint16_t, vector_u16, movp_b16, 16)
ASC_M300_DEFINE_SCALAR_8(uint32_t, vector_u32, movp_b32, 8)
ASC_M300_DEFINE_SCALAR_8(int32_t, vector_s32, movp_b32, 8)

#undef ASC_M300_DEFINE_SCALAR_8

template <typename T = int16_t, bool isSetMask = true>
__aicore__ inline void ShiftRightImpl(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t scalarValue, const uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams, bool roundEn)
{
    if constexpr (isSetMask) {
        SetVectorMask<T>(mask[1], mask[0]);
    }

    __VEC_SCOPE__
    {
        vector_s16 vreg0;
        vector_s16 vreg1;
        vector_bool preg;
        preg = movp_b16();
        uint32_t strideConfig0 = ((static_cast<uint32_t>(repeatParams.srcBlkStride)) << 16);
        uint32_t strideConfig1 = ((static_cast<uint32_t>(repeatParams.dstBlkStride)) << 16);
        for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {
            vsldb(vreg0, src + i * repeatParams.srcRepStride * 16, strideConfig0, preg);
            vshrs(vreg1, vreg0, scalarValue, preg, MODE_ZEROING);
            vsstb(vreg1, dst + i * repeatParams.dstRepStride * 16, strideConfig1, preg);
        }
    }
}

template <typename T, bool isSetMask = true>
__aicore__ inline void ShiftRightImpl(
    __ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams, bool roundEn)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_SCALAR_9(DataType, VectorType, MaskIntrinsic, ElementCount)                      \
    template <typename T = DataType, bool isSetMask = true>                                              \
    __aicore__ inline void ShiftRightImpl(                                                               \
        __ubuf__ DataType* dst, __ubuf__ DataType* src, DataType scalarValue, const uint64_t mask,       \
        const uint8_t repeatTime, const UnaryRepeatParams& repeatParams, bool roundEn)                   \
    {                                                                                                    \
        __VEC_SCOPE__                                                                                    \
        {                                                                                                \
            ASC_M300_STAGE_SCALAR_COUNT_MASK_AND_STRIDES(VectorType, mask, MaskIntrinsic, repeatParams); \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                           \
                vsldb(vreg0, src + i * repeatParams.srcRepStride * ElementCount, strideConfig0, preg);   \
                vshrs(vreg1, vreg0, (int16_t)scalarValue, preg, MODE_ZEROING);                           \
                vsstb(vreg1, dst + i * repeatParams.dstRepStride * ElementCount, strideConfig1, preg);   \
            }                                                                                            \
        }                                                                                                \
    }

ASC_M300_DEFINE_SCALAR_9(uint16_t, vector_u16, plt_b16, 16)

template <typename T = int16_t, bool isSetMask = true>
__aicore__ inline void ShiftRightImpl(
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

ASC_M300_DEFINE_SCALAR_9(int32_t, vector_s32, plt_b32, 8)
ASC_M300_DEFINE_SCALAR_9(uint32_t, vector_u32, plt_b32, 8)

#undef ASC_M300_DEFINE_SCALAR_9

// ShiftLeft::Level 2
template <typename T, bool isSetMask = true>
__aicore__ inline void ShiftRightImpl(__ubuf__ T* dst, __ubuf__ T* src, const T scalarValue, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_SCALAR_7(uint16_t, ShiftRightImpl, vector_u16, 128, plt_b16, vshrs, NORM_B16)
ASC_M300_DEFINE_SCALAR_7(uint32_t, ShiftRightImpl, vector_u32, 64, plt_b32, vshrs, NORM_B32)
ASC_M300_DEFINE_SCALAR_3(int16_t, ShiftRightImpl, vector_s16, 128, plt_b16, vshrs, NORM_B16)
ASC_M300_DEFINE_SCALAR_7(int32_t, ShiftRightImpl, vector_s32, 64, plt_b32, vshrs, NORM_B32)

#undef ASC_M300_DEFINE_SCALAR_7

/* **************************************************************************************************
 * LeakyRelu                                             *
 * ************************************************************************************************* */
// LeakyRelu::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void LeakyReluImpl(
    __ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const uint64_t mask[], uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_SCALAR_1(float, LeakyReluImpl, vector_f32, movp_b32, 8, vlrelu)
ASC_M300_DEFINE_SCALAR_1(half, LeakyReluImpl, vector_f16, movp_b16, 16, vlrelu)

#undef ASC_M300_DEFINE_SCALAR_1

template <typename T, bool isSetMask = true>
__aicore__ inline void LeakyReluImpl(
    __ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const uint64_t mask, uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_SCALAR_2(float, LeakyReluImpl, vector_f32, plt_b32, 8, vlrelu)
ASC_M300_DEFINE_SCALAR_2(half, LeakyReluImpl, vector_f16, plt_b16, 16, vlrelu)

#undef ASC_M300_DEFINE_SCALAR_2

// LeakyRelu::Level 2
template <typename T, bool isSetMask = true>
__aicore__ inline void LeakyReluImpl(__ubuf__ T* dst, __ubuf__ T* src, T scalarValue, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_SCALAR_3(float, LeakyReluImpl, vector_f32, 64, plt_b32, vlrelu, NORM_B32)
ASC_M300_DEFINE_SCALAR_3(half, LeakyReluImpl, vector_f16, 128, plt_b16, vlrelu, NORM_B16)

#undef ASC_M300_DEFINE_SCALAR_3
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_BINARY_SCALAR_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_SCALAR_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_SCALAR_IMPL_H__
#endif

#undef ASC_M300_STAGE_SCALAR_CONTIGUOUS_INIT_REGS
#undef ASC_M300_STAGE_SCALAR_COUNT_MASK_AND_STRIDES
#undef ASC_M300_STAGE_SCALAR_BITMASK_AND_STRIDES
