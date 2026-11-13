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
 * \file kernel_operator_vec_binary_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_m300/kernel_operator_vec_binary_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_vec_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_BINARY_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_BINARY_IMPL_H
#include "../kernel_utils.h"
#include "../../../include/basic_api/kernel_common.h"
#include "../../../include/basic_api/kernel_struct_binary.h"
#include "kernel_operator_common_impl.h"
#include "kernel_operator_vec_binary_continuous_impl.h"

// Expand shared register stages in place to preserve the compiler input and instruction order.
#define ASC_M300_STAGE_BINARY_LOAD_PAIR_WITH_BARRIER(                                                    \
    repeatParams, repeatTime, vreg0, src0, ElementCount, preg, vreg1, src1, VectorIntrinsic, vreg2, dst) \
    uint32_t strideConfig0 = ((static_cast<uint32_t>(repeatParams.src0BlkStride)) << 16);                \
    uint32_t strideConfig1 = ((static_cast<uint32_t>(repeatParams.src1BlkStride)) << 16);                \
    uint32_t strideConfig2 = ((static_cast<uint32_t>(repeatParams.dstBlkStride)) << 16);                 \
    for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                                   \
        mem_bar(VST_VLD);                                                                                \
        vsldb(vreg0, src0 + i * repeatParams.src0RepStride * ElementCount, strideConfig0, preg);         \
        vsldb(vreg1, src1 + i * repeatParams.src1RepStride * ElementCount, strideConfig1, preg);         \
        VectorIntrinsic(vreg2, vreg0, vreg1, preg, MODE_ZEROING);                                        \
        vsstb(vreg2, dst + i * repeatParams.dstRepStride * ElementCount, strideConfig2, preg);           \
    }

#define ASC_M300_STAGE_BINARY_BITMASK_AND_STRIDES(VectorType, MaskIntrinsic, repeatParams) \
    VectorType vreg0;                                                                      \
    VectorType vreg1;                                                                      \
    VectorType vreg2;                                                                      \
    VectorType vreg3;                                                                      \
    vector_bool preg;                                                                      \
    preg = MaskIntrinsic();                                                                \
    uint32_t strideConfig0 = (static_cast<uint32_t>(repeatParams.src0BlkStride) << 16);    \
    uint32_t strideConfig1 = (static_cast<uint32_t>(repeatParams.src1BlkStride) << 16);    \
    uint32_t strideConfig2 = (static_cast<uint32_t>(repeatParams.dstBlkStride) << 16)

#define ASC_M300_STAGE_BINARY_REPEAT_WITH_RESULT_LOAD(                                                           \
    repeatTime, vreg0, src0, repeatParams, ElementCount, strideConfig0, preg, vreg1, src1, strideConfig1, vreg2, \
    vreg3, dst, strideConfig2)                                                                                   \
    for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                                           \
        mem_bar(VST_VLD);                                                                                        \
        vsldb(vreg0, src0 + i * repeatParams.src0RepStride * ElementCount, strideConfig0, preg);                 \
        vsldb(vreg1, src1 + i * repeatParams.src1RepStride * ElementCount, strideConfig1, preg);                 \
        vadd(vreg2, vreg0, vreg1, preg, MODE_ZEROING);                                                           \
        vrelu(vreg3, vreg2, preg, MODE_ZEROING);                                                                 \
        vsstb(vreg3, dst + i * repeatParams.dstRepStride * ElementCount, strideConfig2, preg);                   \
    }

#define ASC_M300_STAGE_BINARY_COUNT_MASK_AND_STRIDES(                                   \
    VectorType, vreg1, vreg2, vreg3, mask, MaskIntrinsic, repeatParams)                 \
    VectorType vreg1;                                                                   \
    VectorType vreg2;                                                                   \
    VectorType vreg3;                                                                   \
    uint32_t sreg = static_cast<uint32_t>(mask);                                        \
    vector_bool preg = MaskIntrinsic(sreg, POST_UPDATE);                                \
    uint32_t strideConfig0 = (static_cast<uint32_t>(repeatParams.src0BlkStride) << 16); \
    uint32_t strideConfig1 = (static_cast<uint32_t>(repeatParams.src1BlkStride) << 16); \
    uint32_t strideConfig2 = (static_cast<uint32_t>(repeatParams.dstBlkStride) << 16)

#define ASC_M300_STAGE_BINARY_REPEAT_WITH_CONVERSION(                                                                 \
    repeatTime, vreg0, src0, repeatParams, ElementCount, strideConfig0, preg, vreg1, src1, strideConfig1, vreg2, dst, \
    strideConfig2)                                                                                                    \
    for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                                                \
        mem_bar(VST_VLD);                                                                                             \
        vsldb(vreg0, src0 + i * repeatParams.src0RepStride * ElementCount, strideConfig0, preg);                      \
        vsldb(vreg1, src1 + i * repeatParams.src1RepStride * ElementCount, strideConfig1, preg);                      \
        vsldb(vreg2, dst + i * repeatParams.dstRepStride * ElementCount, strideConfig2, preg);                        \
        vmadd(vreg2, vreg0, vreg1, preg, MODE_ZEROING);                                                               \
        vsstb(vreg2, dst + i * repeatParams.dstRepStride * ElementCount, strideConfig2, preg);                        \
    }

#define ASC_M300_STAGE_BINARY_REPEAT_FUSED_PAIR(                                                                      \
    repeatTime, vreg0, src0, repeatParams, ElementCount, strideConfig0, preg, vreg1, src1, strideConfig1, vreg2, dst, \
    strideConfig2, vreg3)                                                                                             \
    for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                                                \
        mem_bar(VST_VLD);                                                                                             \
        vsldb(vreg0, src0 + i * repeatParams.src0RepStride * ElementCount, strideConfig0, preg);                      \
        vsldb(vreg1, src1 + i * repeatParams.src1RepStride * ElementCount, strideConfig1, preg);                      \
        vsldb(vreg2, dst + i * repeatParams.dstRepStride * ElementCount, strideConfig2, preg);                        \
        vmadd(vreg2, vreg0, vreg1, preg, MODE_ZEROING);                                                               \
        vrelu(vreg3, vreg2, preg, MODE_ZEROING);                                                                      \
        vsstb(vreg3, dst + i * repeatParams.dstRepStride * ElementCount, strideConfig2, preg);                        \
    }

#define ASC_M300_STAGE_BINARY_REPEAT_WITH_DEST_INPUT(                                                            \
    repeatTime, vreg0, src0, repeatParams, ElementCount, strideConfig0, preg, vreg1, src1, strideConfig1, vreg2, \
    vreg3, dst, strideConfig2)                                                                                   \
    for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                                           \
        mem_bar(VST_VLD);                                                                                        \
        vsldb(vreg0, src0 + i * repeatParams.src0RepStride * ElementCount, strideConfig0, preg);                 \
        vsldb(vreg1, src1 + i * repeatParams.src1RepStride * ElementCount, strideConfig1, preg);                 \
        vsub(vreg2, vreg0, vreg1, preg, MODE_ZEROING);                                                           \
        vrelu(vreg3, vreg2, preg, MODE_ZEROING);                                                                 \
        vsstb(vreg3, dst + i * repeatParams.dstRepStride * ElementCount, strideConfig2, preg);                   \
    }

namespace AscendC {
/* **************************************************************************************************
 * Add                                             *
 * ************************************************************************************************* */
// Add::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void AddImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

// Emit the original overloads with their existing vector scope and intrinsic order.
#define ASC_M300_DEFINE_BINARY_1(DataType, Function, VectorType, MaskIntrinsic, ElementCount, VectorIntrinsic)       \
    template <typename T = DataType, bool isSetMask = true>                                                          \
    __aicore__ inline void Function(                                                                                 \
        __ubuf__ DataType* dst, __ubuf__ DataType* src0, __ubuf__ DataType* src1, const uint64_t mask[],             \
        const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)                                            \
    {                                                                                                                \
        if constexpr (isSetMask) {                                                                                   \
            SetVectorMask<T>(mask[1], mask[0]);                                                                      \
        }                                                                                                            \
        __VEC_SCOPE__                                                                                                \
        {                                                                                                            \
            VectorType vreg0;                                                                                        \
            VectorType vreg1;                                                                                        \
            VectorType vreg2;                                                                                        \
            vector_bool preg;                                                                                        \
            preg = MaskIntrinsic();                                                                                  \
            ASC_M300_STAGE_BINARY_LOAD_PAIR_WITH_BARRIER(                                                            \
                repeatParams, repeatTime, vreg0, src0, ElementCount, preg, vreg1, src1, VectorIntrinsic, vreg2, dst) \
        }                                                                                                            \
    }

ASC_M300_DEFINE_BINARY_1(half, AddImpl, vector_f16, movp_b16, 16, vadd)
ASC_M300_DEFINE_BINARY_1(int16_t, AddImpl, vector_s16, movp_b16, 16, vadd)
ASC_M300_DEFINE_BINARY_1(int32_t, AddImpl, vector_s32, movp_b32, 8, vadd)
ASC_M300_DEFINE_BINARY_1(float, AddImpl, vector_f32, movp_b32, 8, vadd)

template <typename T, bool isSetMask = true>
__aicore__ inline void AddImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_BINARY_2(DataType, Function, VectorType, MaskIntrinsic, ElementCount, VectorIntrinsic)       \
    template <typename T = DataType, bool isSetMask = true>                                                          \
    __aicore__ inline void Function(                                                                                 \
        __ubuf__ DataType* dst, __ubuf__ DataType* src0, __ubuf__ DataType* src1, const uint64_t mask,               \
        const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)                                            \
    {                                                                                                                \
        __VEC_SCOPE__                                                                                                \
        {                                                                                                            \
            VectorType vreg0;                                                                                        \
            VectorType vreg1;                                                                                        \
            VectorType vreg2;                                                                                        \
            uint32_t sreg = static_cast<uint32_t>(mask);                                                             \
            vector_bool preg = MaskIntrinsic(sreg, POST_UPDATE);                                                     \
            ASC_M300_STAGE_BINARY_LOAD_PAIR_WITH_BARRIER(                                                            \
                repeatParams, repeatTime, vreg0, src0, ElementCount, preg, vreg1, src1, VectorIntrinsic, vreg2, dst) \
        }                                                                                                            \
    }

ASC_M300_DEFINE_BINARY_2(half, AddImpl, vector_f16, plt_b16, 16, vadd)
ASC_M300_DEFINE_BINARY_2(int16_t, AddImpl, vector_s16, plt_b16, 16, vadd)
ASC_M300_DEFINE_BINARY_2(int32_t, AddImpl, vector_s32, plt_b32, 8, vadd)
ASC_M300_DEFINE_BINARY_2(float, AddImpl, vector_f32, plt_b32, 8, vadd)

/* **************************************************************************************************
 * Sub                                             *
 * ************************************************************************************************* */
// Sub::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void SubImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_BINARY_1(half, SubImpl, vector_f16, movp_b16, 16, vsub)
ASC_M300_DEFINE_BINARY_1(int16_t, SubImpl, vector_s16, movp_b16, 16, vsub)
ASC_M300_DEFINE_BINARY_1(int32_t, SubImpl, vector_s32, movp_b32, 8, vsub)
ASC_M300_DEFINE_BINARY_1(float, SubImpl, vector_f32, movp_b32, 8, vsub)

template <typename T, bool isSetMask = true>
__aicore__ inline void SubImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_BINARY_2(half, SubImpl, vector_f16, plt_b16, 16, vsub)
ASC_M300_DEFINE_BINARY_2(int16_t, SubImpl, vector_s16, plt_b16, 16, vsub)
ASC_M300_DEFINE_BINARY_2(int32_t, SubImpl, vector_s32, plt_b32, 8, vsub)
ASC_M300_DEFINE_BINARY_2(float, SubImpl, vector_f32, plt_b32, 8, vsub)

/* **************************************************************************************************
 * Mul                                             *
 * ************************************************************************************************* */
// Mul::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void MulImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_BINARY_1(half, MulImpl, vector_f16, movp_b16, 16, vmul)
ASC_M300_DEFINE_BINARY_1(int16_t, MulImpl, vector_s16, movp_b16, 16, vmul)
ASC_M300_DEFINE_BINARY_1(int32_t, MulImpl, vector_s32, movp_b32, 8, vmul)
ASC_M300_DEFINE_BINARY_1(float, MulImpl, vector_f32, movp_b32, 8, vmul)

template <typename T, bool isSetMask = true>
__aicore__ inline void MulImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_BINARY_2(half, MulImpl, vector_f16, plt_b16, 16, vmul)
ASC_M300_DEFINE_BINARY_2(int16_t, MulImpl, vector_s16, plt_b16, 16, vmul)
ASC_M300_DEFINE_BINARY_2(int32_t, MulImpl, vector_s32, plt_b32, 8, vmul)
ASC_M300_DEFINE_BINARY_2(float, MulImpl, vector_f32, plt_b32, 8, vmul)

/* **************************************************************************************************
 * Div                                             *
 * ************************************************************************************************* */
// Div::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void DivImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_BINARY_1(half, DivImpl, vector_f16, movp_b16, 16, vdiv)
ASC_M300_DEFINE_BINARY_1(float, DivImpl, vector_f32, movp_b32, 8, vdiv)

template <typename T, bool isSetMask = true>
__aicore__ inline void DivImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_BINARY_2(half, DivImpl, vector_f16, plt_b16, 16, vdiv)

template <typename T = float, bool isSetMask = true>
__aicore__ inline void DivImpl(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    __VEC_SCOPE__
    {
        vector_f32 vreg0;
        vector_f32 vreg1;
        vector_f32 vreg2;
        vector_bool preg;
        uint32_t sreg = static_cast<uint32_t>(mask);
        preg = plt_b32(sreg, POST_UPDATE);
        uint32_t strideConfig0 = ((static_cast<uint32_t>(repeatParams.src0BlkStride)) << 16);
        uint32_t strideConfig1 = ((static_cast<uint32_t>(repeatParams.src1BlkStride)) << 16);
        uint32_t strideConfig2 = ((static_cast<uint32_t>(repeatParams.dstBlkStride)) << 16);
        for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {
            mem_bar(VST_VLD);
            vsldb(vreg0, src0 + i * repeatParams.src0RepStride * 8, strideConfig0, preg);
            vsldb(vreg1, src1 + i * repeatParams.src1RepStride * 8, strideConfig1, preg);
            vdiv(vreg2, vreg0, vreg1, preg, MODE_ZEROING);
            vsstb(vreg2, dst + i * repeatParams.dstRepStride * 8, strideConfig2, preg);
        }
    }
}

/* **************************************************************************************************
 * Max                                             *
 * ************************************************************************************************* */
// Max::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void MaxImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

template <typename T, bool isSetMask = true>
__aicore__ inline void MaxImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_BINARY_1(half, MaxImpl, vector_f16, movp_b16, 16, vmax)
ASC_M300_DEFINE_BINARY_1(int16_t, MaxImpl, vector_s16, movp_b16, 16, vmax)
ASC_M300_DEFINE_BINARY_1(int32_t, MaxImpl, vector_s32, movp_b32, 8, vmax)
ASC_M300_DEFINE_BINARY_1(float, MaxImpl, vector_f32, movp_b32, 8, vmax)
ASC_M300_DEFINE_BINARY_2(half, MaxImpl, vector_f16, plt_b16, 16, vmax)
ASC_M300_DEFINE_BINARY_2(int16_t, MaxImpl, vector_s16, plt_b16, 16, vmax)
ASC_M300_DEFINE_BINARY_2(int32_t, MaxImpl, vector_s32, plt_b32, 8, vmax)
ASC_M300_DEFINE_BINARY_2(float, MaxImpl, vector_f32, plt_b32, 8, vmax)

/* **************************************************************************************************
 * Min                                             *
 * ************************************************************************************************* */
// Min::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void MinImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_BINARY_1(half, MinImpl, vector_f16, movp_b16, 16, vmin)
ASC_M300_DEFINE_BINARY_1(int16_t, MinImpl, vector_s16, movp_b16, 16, vmin)
ASC_M300_DEFINE_BINARY_1(int32_t, MinImpl, vector_s32, movp_b32, 8, vmin)
ASC_M300_DEFINE_BINARY_1(float, MinImpl, vector_f32, movp_b32, 8, vmin)

template <typename T, bool isSetMask = true>
__aicore__ inline void MinImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_BINARY_2(half, MinImpl, vector_f16, plt_b16, 16, vmin)
ASC_M300_DEFINE_BINARY_2(int16_t, MinImpl, vector_s16, plt_b16, 16, vmin)
ASC_M300_DEFINE_BINARY_2(int32_t, MinImpl, vector_s32, plt_b32, 8, vmin)
ASC_M300_DEFINE_BINARY_2(float, MinImpl, vector_f32, plt_b32, 8, vmin)

/* **************************************************************************************************
 * And                                             *
 * ************************************************************************************************* */
// And::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void AndImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_BINARY_1(int16_t, AndImpl, vector_s16, movp_b16, 16, vand)
ASC_M300_DEFINE_BINARY_1(uint16_t, AndImpl, vector_u16, movp_b16, 16, vand)

template <typename T, bool isSetMask = true>
__aicore__ inline void AndImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_BINARY_2(int16_t, AndImpl, vector_s16, plt_b16, 16, vand)
ASC_M300_DEFINE_BINARY_2(uint16_t, AndImpl, vector_u16, plt_b16, 16, vand)

/* **************************************************************************************************
 * Or                                             *
 * ************************************************************************************************* */
// Or::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void OrImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_BINARY_1(int16_t, OrImpl, vector_s16, movp_b16, 16, vor)
ASC_M300_DEFINE_BINARY_1(uint16_t, OrImpl, vector_u16, movp_b16, 16, vor)

#undef ASC_M300_DEFINE_BINARY_1

template <typename T, bool isSetMask = true>
__aicore__ inline void OrImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_BINARY_2(int16_t, OrImpl, vector_s16, plt_b16, 16, vor)
ASC_M300_DEFINE_BINARY_2(uint16_t, OrImpl, vector_u16, plt_b16, 16, vor)

#undef ASC_M300_DEFINE_BINARY_2

/* **************************************************************************************************
 * AddRelu                                             *
 * ************************************************************************************************* */
// AddRelu::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void AddReluImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_BINARY_3(DataType, VectorType, MaskIntrinsic, ElementCount)                                   \
    template <typename T = DataType, bool isSetMask = true>                                                           \
    __aicore__ inline void AddReluImpl(                                                                               \
        __ubuf__ DataType* dst, __ubuf__ DataType* src0, __ubuf__ DataType* src1, const uint64_t mask[],              \
        const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)                                             \
    {                                                                                                                 \
        static_assert(SupportType<T, DataType>(), "AddRelu level-0 api only support half/float on current device");   \
        if constexpr (isSetMask) {                                                                                    \
            SetVectorMask<T>(mask[1], mask[0]);                                                                       \
        }                                                                                                             \
        __VEC_SCOPE__                                                                                                 \
        {                                                                                                             \
            ASC_M300_STAGE_BINARY_BITMASK_AND_STRIDES(VectorType, MaskIntrinsic, repeatParams);                       \
            ASC_M300_STAGE_BINARY_REPEAT_WITH_RESULT_LOAD(                                                            \
                repeatTime, vreg0, src0, repeatParams, ElementCount, strideConfig0, preg, vreg1, src1, strideConfig1, \
                vreg2, vreg3, dst, strideConfig2)                                                                     \
        }                                                                                                             \
    }

ASC_M300_DEFINE_BINARY_3(half, vector_f16, movp_b16, 16)
ASC_M300_DEFINE_BINARY_3(float, vector_f32, movp_b32, 8)

#undef ASC_M300_DEFINE_BINARY_3

template <typename T, bool isSetMask = true>
__aicore__ inline void AddReluImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_BINARY_4(DataType, VectorType, MaskIntrinsic, ElementCount)                                   \
    template <typename T = DataType, bool isSetMask = true>                                                           \
    __aicore__ inline void AddReluImpl(                                                                               \
        __ubuf__ DataType* dst, __ubuf__ DataType* src0, __ubuf__ DataType* src1, const uint64_t mask,                \
        const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)                                             \
    {                                                                                                                 \
        static_assert(SupportType<T, DataType>(), "AddRelu level-0 api only support half/float on current device");   \
        __VEC_SCOPE__                                                                                                 \
        {                                                                                                             \
            VectorType vreg0;                                                                                         \
            ASC_M300_STAGE_BINARY_COUNT_MASK_AND_STRIDES(                                                             \
                VectorType, vreg1, vreg2, vreg3, mask, MaskIntrinsic, repeatParams);                                  \
            ASC_M300_STAGE_BINARY_REPEAT_WITH_RESULT_LOAD(                                                            \
                repeatTime, vreg0, src0, repeatParams, ElementCount, strideConfig0, preg, vreg1, src1, strideConfig1, \
                vreg2, vreg3, dst, strideConfig2)                                                                     \
        }                                                                                                             \
    }

ASC_M300_DEFINE_BINARY_4(half, vector_f16, plt_b16, 16)
ASC_M300_DEFINE_BINARY_4(float, vector_f32, plt_b32, 8)

#undef ASC_M300_DEFINE_BINARY_4

/* **************************************************************************************************
 * FusedMulAdd                                             *
 * ************************************************************************************************* */
// FusedMulAdd::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void FusedMulAddImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_BINARY_5(DataType, VectorType, MaskIntrinsic, ElementCount)                                   \
    template <typename T = DataType, bool isSetMask = true>                                                           \
    __aicore__ inline void FusedMulAddImpl(                                                                           \
        __ubuf__ DataType* dst, __ubuf__ DataType* src0, __ubuf__ DataType* src1, const uint64_t mask[],              \
        const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)                                             \
    {                                                                                                                 \
        static_assert(                                                                                                \
            SupportType<T, DataType>(), "FusedMulAdd level-0 api only support half/float on current device");         \
        if constexpr (isSetMask) {                                                                                    \
            SetVectorMask<T>(mask[1], mask[0]);                                                                       \
        }                                                                                                             \
        __VEC_SCOPE__                                                                                                 \
        {                                                                                                             \
            VectorType vreg0;                                                                                         \
            VectorType vreg1;                                                                                         \
            VectorType vreg2;                                                                                         \
            vector_bool preg;                                                                                         \
            preg = MaskIntrinsic();                                                                                   \
            uint32_t strideConfig0 = (static_cast<uint32_t>(repeatParams.src0BlkStride) << 16);                       \
            uint32_t strideConfig1 = (static_cast<uint32_t>(repeatParams.src1BlkStride) << 16);                       \
            uint32_t strideConfig2 = (static_cast<uint32_t>(repeatParams.dstBlkStride) << 16);                        \
            ASC_M300_STAGE_BINARY_REPEAT_WITH_CONVERSION(                                                             \
                repeatTime, vreg0, src0, repeatParams, ElementCount, strideConfig0, preg, vreg1, src1, strideConfig1, \
                vreg2, dst, strideConfig2)                                                                            \
        }                                                                                                             \
    }

ASC_M300_DEFINE_BINARY_5(half, vector_f16, movp_b16, 16)
ASC_M300_DEFINE_BINARY_5(float, vector_f32, movp_b32, 8)

#undef ASC_M300_DEFINE_BINARY_5

template <typename T, bool isSetMask = true>
__aicore__ inline void FusedMulAddImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_BINARY_6(DataType, VectorType, MaskIntrinsic, ElementCount)                                   \
    template <typename T = DataType, bool isSetMask = true>                                                           \
    __aicore__ inline void FusedMulAddImpl(                                                                           \
        __ubuf__ DataType* dst, __ubuf__ DataType* src0, __ubuf__ DataType* src1, const uint64_t mask,                \
        const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)                                             \
    {                                                                                                                 \
        static_assert(                                                                                                \
            SupportType<T, DataType>(), "FusedMulAdd level-0 api only support half/float on current device");         \
        __VEC_SCOPE__                                                                                                 \
        {                                                                                                             \
            ASC_M300_STAGE_BINARY_COUNT_MASK_AND_STRIDES(                                                             \
                VectorType, vreg0, vreg1, vreg2, mask, MaskIntrinsic, repeatParams);                                  \
            ASC_M300_STAGE_BINARY_REPEAT_WITH_CONVERSION(                                                             \
                repeatTime, vreg0, src0, repeatParams, ElementCount, strideConfig0, preg, vreg1, src1, strideConfig1, \
                vreg2, dst, strideConfig2)                                                                            \
        }                                                                                                             \
    }

ASC_M300_DEFINE_BINARY_6(half, vector_f16, plt_b16, 16)
ASC_M300_DEFINE_BINARY_6(float, vector_f32, plt_b32, 8)

#undef ASC_M300_DEFINE_BINARY_6

/* **************************************************************************************************
 * FusedMulAddRelu                                             *
 * ************************************************************************************************* */
// FusedMulAddRelu::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void FusedMulAddReluImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_BINARY_7(DataType, VectorType, MaskIntrinsic, ElementCount)                                   \
    template <typename T = DataType, bool isSetMask = true>                                                           \
    __aicore__ inline void FusedMulAddReluImpl(                                                                       \
        __ubuf__ DataType* dst, __ubuf__ DataType* src0, __ubuf__ DataType* src1, const uint64_t mask[],              \
        const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)                                             \
    {                                                                                                                 \
        static_assert(                                                                                                \
            SupportType<T, DataType>(), "FusedMulAddRelu level-0 api only support half/float on current device");     \
        if constexpr (isSetMask) {                                                                                    \
            SetVectorMask<T>(mask[1], mask[0]);                                                                       \
        }                                                                                                             \
        __VEC_SCOPE__                                                                                                 \
        {                                                                                                             \
            ASC_M300_STAGE_BINARY_BITMASK_AND_STRIDES(VectorType, MaskIntrinsic, repeatParams);                       \
            ASC_M300_STAGE_BINARY_REPEAT_FUSED_PAIR(                                                                  \
                repeatTime, vreg0, src0, repeatParams, ElementCount, strideConfig0, preg, vreg1, src1, strideConfig1, \
                vreg2, dst, strideConfig2, vreg3)                                                                     \
        }                                                                                                             \
    }

ASC_M300_DEFINE_BINARY_7(half, vector_f16, movp_b16, 16)
ASC_M300_DEFINE_BINARY_7(float, vector_f32, movp_b32, 8)

#undef ASC_M300_DEFINE_BINARY_7

template <typename T, bool isSetMask = true>
__aicore__ inline void FusedMulAddReluImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_BINARY_8(DataType, VectorType, MaskIntrinsic, ElementCount)                                   \
    template <typename T = DataType, bool isSetMask = true>                                                           \
    __aicore__ inline void FusedMulAddReluImpl(                                                                       \
        __ubuf__ DataType* dst, __ubuf__ DataType* src0, __ubuf__ DataType* src1, const uint64_t mask,                \
        const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)                                             \
    {                                                                                                                 \
        static_assert(                                                                                                \
            SupportType<T, DataType>(), "FusedMulAddRelu level-0 api only support half/float on current device");     \
        __VEC_SCOPE__                                                                                                 \
        {                                                                                                             \
            VectorType vreg0;                                                                                         \
            VectorType vreg1;                                                                                         \
            VectorType vreg2;                                                                                         \
            VectorType vreg3;                                                                                         \
            uint32_t sreg = static_cast<uint32_t>(mask);                                                              \
            vector_bool preg = MaskIntrinsic(sreg, POST_UPDATE);                                                      \
            uint32_t strideConfig0 = (static_cast<uint32_t>(repeatParams.src0BlkStride) << 16);                       \
            uint32_t strideConfig1 = (static_cast<uint32_t>(repeatParams.src1BlkStride) << 16);                       \
            uint32_t strideConfig2 = (static_cast<uint32_t>(repeatParams.dstBlkStride) << 16);                        \
            ASC_M300_STAGE_BINARY_REPEAT_FUSED_PAIR(                                                                  \
                repeatTime, vreg0, src0, repeatParams, ElementCount, strideConfig0, preg, vreg1, src1, strideConfig1, \
                vreg2, dst, strideConfig2, vreg3)                                                                     \
        }                                                                                                             \
    }

ASC_M300_DEFINE_BINARY_8(half, vector_f16, plt_b16, 16)
ASC_M300_DEFINE_BINARY_8(float, vector_f32, plt_b32, 8)

#undef ASC_M300_DEFINE_BINARY_8

/* **************************************************************************************************
 * MulAddDst                                             *
 * ************************************************************************************************* */
// MulAddDst::Level 0
template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void MulAddDstImpl(
    __ubuf__ T* dst, __ubuf__ U* src0, __ubuf__ U* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_BINARY_9(DataType, VectorType)                                                             \
    template <typename T = DataType, typename U = DataType, bool isSetMask = true>                                 \
    __aicore__ inline void MulAddDstImpl(                                                                          \
        __ubuf__ DataType* dst, __ubuf__ DataType* src0, __ubuf__ DataType* src1, const uint64_t mask[],           \
        const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)                                          \
    {                                                                                                              \
        static_assert(SupportType<U, DataType>(), "MulAddDst level-0 api only support b16/b32 on current device"); \
        static_assert(SupportType<T, DataType>(), "MulAddDst level-0 api only support b16/b32 on current device"); \
        if constexpr (isSetMask) {                                                                                 \
            SetVectorMask<T>(mask[1], mask[0]);                                                                    \
        }                                                                                                          \
        __VEC_SCOPE__                                                                                              \
        {                                                                                                          \
            VectorType vreg0;                                                                                      \
            VectorType vreg1;                                                                                      \
            VectorType vreg2;                                                                                      \
            vector_bool preg;                                                                                      \
            preg = movp_b16();                                                                                     \
            uint32_t strideConfig0 = (static_cast<uint32_t>(repeatParams.src0BlkStride) << 16);                    \
            uint32_t strideConfig1 = (static_cast<uint32_t>(repeatParams.src1BlkStride) << 16);                    \
            uint32_t strideConfig2 = (static_cast<uint32_t>(repeatParams.dstBlkStride) << 16);                     \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                                     \
                mem_bar(VST_VLD);                                                                                  \
                vsldb(vreg0, src0 + i * repeatParams.src0RepStride * 16, strideConfig0, preg);                     \
                vsldb(vreg1, src1 + i * repeatParams.src1RepStride * 16, strideConfig1, preg);                     \
                vsldb(vreg2, dst + i * repeatParams.dstRepStride * 16, strideConfig2, preg);                       \
                vmula(vreg2, vreg0, vreg1, preg, MODE_ZEROING);                                                    \
                vsstb(vreg2, dst + i * repeatParams.dstRepStride * 16, strideConfig2, preg);                       \
            }                                                                                                      \
        }                                                                                                          \
    }

ASC_M300_DEFINE_BINARY_9(half, vector_f16)
ASC_M300_DEFINE_BINARY_9(int16_t, vector_s16)
ASC_M300_DEFINE_BINARY_9(uint16_t, vector_u16)

#undef ASC_M300_DEFINE_BINARY_9

#define ASC_M300_DEFINE_BINARY_10(DataType, VectorType)                                                             \
    template <typename T = DataType, typename U = DataType, bool isSetMask = true>                                  \
    __aicore__ inline void MulAddDstImpl(                                                                           \
        __ubuf__ DataType* dst, __ubuf__ DataType* src0, __ubuf__ DataType* src1, const uint64_t mask[],            \
        const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)                                           \
    {                                                                                                               \
        static_assert(SupportType<T, DataType>(), "MulAddDst level-0 api only support b16/b32  on current device"); \
        static_assert(SupportType<U, DataType>(), "MulAddDst level-0 api only support b16/b32  on current device"); \
        if constexpr (isSetMask) {                                                                                  \
            SetVectorMask<T>(mask[1], mask[0]);                                                                     \
        }                                                                                                           \
        __VEC_SCOPE__                                                                                               \
        {                                                                                                           \
            VectorType vreg0;                                                                                       \
            VectorType vreg1;                                                                                       \
            VectorType vreg2;                                                                                       \
            vector_bool preg;                                                                                       \
            preg = movp_b32();                                                                                      \
            uint32_t strideConfig0 = (static_cast<uint32_t>(repeatParams.src0BlkStride) << 16);                     \
            uint32_t strideConfig1 = (static_cast<uint32_t>(repeatParams.src1BlkStride) << 16);                     \
            uint32_t strideConfig2 = (static_cast<uint32_t>(repeatParams.dstBlkStride) << 16);                      \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                                      \
                mem_bar(VST_VLD);                                                                                   \
                vsldb(vreg0, src0 + i * repeatParams.src0RepStride * 8, strideConfig0, preg);                       \
                vsldb(vreg1, src1 + i * repeatParams.src1RepStride * 8, strideConfig1, preg);                       \
                vsldb(vreg2, dst + i * repeatParams.dstRepStride * 8, strideConfig2, preg);                         \
                vmula(vreg2, vreg0, vreg1, preg, MODE_ZEROING);                                                     \
                vsstb(vreg2, dst + i * repeatParams.dstRepStride * 8, strideConfig2, preg);                         \
            }                                                                                                       \
        }                                                                                                           \
    }

ASC_M300_DEFINE_BINARY_10(float, vector_f32)
ASC_M300_DEFINE_BINARY_10(int32_t, vector_s32)
ASC_M300_DEFINE_BINARY_10(uint32_t, vector_u32)

#undef ASC_M300_DEFINE_BINARY_10

template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void MulAddDstImpl(
    __ubuf__ T* dst, __ubuf__ U* src0, __ubuf__ U* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_BINARY_11(DataType, VectorType, MaskIntrinsic, ElementCount)                               \
    template <typename T = DataType, typename U = DataType, bool isSetMask = true>                                 \
    __aicore__ inline void MulAddDstImpl(                                                                          \
        __ubuf__ DataType* dst, __ubuf__ DataType* src0, __ubuf__ DataType* src1, const uint64_t mask,             \
        const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)                                          \
    {                                                                                                              \
        static_assert(SupportType<T, DataType>(), "MulAddDst level-0 api only support b16/b32 on current device"); \
        static_assert(SupportType<U, DataType>(), "MulAddDst level-0 api only support b16/b32 on current device"); \
        __VEC_SCOPE__                                                                                              \
        {                                                                                                          \
            ASC_M300_STAGE_BINARY_COUNT_MASK_AND_STRIDES(                                                          \
                VectorType, vreg0, vreg1, vreg2, mask, MaskIntrinsic, repeatParams);                               \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                                     \
                mem_bar(VST_VLD);                                                                                  \
                vsldb(vreg0, src0 + i * repeatParams.src0RepStride * ElementCount, strideConfig0, preg);           \
                vsldb(vreg1, src1 + i * repeatParams.src1RepStride * ElementCount, strideConfig1, preg);           \
                vsldb(vreg2, dst + i * repeatParams.dstRepStride * ElementCount, strideConfig2, preg);             \
                vmula(vreg2, vreg0, vreg1, preg, MODE_ZEROING);                                                    \
                vsstb(vreg2, dst + i * repeatParams.dstRepStride * ElementCount, strideConfig2, preg);             \
            }                                                                                                      \
        }                                                                                                          \
    }

ASC_M300_DEFINE_BINARY_11(half, vector_f16, plt_b16, 16)
ASC_M300_DEFINE_BINARY_11(int16_t, vector_s16, plt_b16, 16)
ASC_M300_DEFINE_BINARY_11(uint16_t, vector_u16, plt_b16, 16)
ASC_M300_DEFINE_BINARY_11(float, vector_f32, plt_b32, 8)
ASC_M300_DEFINE_BINARY_11(int32_t, vector_s32, plt_b32, 8)
ASC_M300_DEFINE_BINARY_11(uint32_t, vector_u32, plt_b32, 8)

#undef ASC_M300_DEFINE_BINARY_11

/* **************************************************************************************************
 * SubRelu                                             *
 * ************************************************************************************************* */
// SubRelu::Level 2
template <typename T>
__aicore__ inline void SubReluImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_BINARY_12(DataType, VectorType, ElementCount, MaskIntrinsic, NORM_B16Arg)                   \
    template <typename T = DataType>                                                                                \
    __aicore__ inline void SubReluImpl(                                                                             \
        __ubuf__ DataType* dst, __ubuf__ DataType* src0, __ubuf__ DataType* src1, const int32_t& count)             \
    {                                                                                                               \
        static_assert(SupportType<T, DataType>(), "SubRelu level-2 api only support half/float on current device"); \
        __VEC_SCOPE__                                                                                               \
        {                                                                                                           \
            VectorType vreg0;                                                                                       \
            VectorType vreg1;                                                                                       \
            VectorType vreg2;                                                                                       \
            VectorType vreg3;                                                                                       \
            uint32_t sreg = static_cast<uint32_t>(count);                                                           \
            vector_bool preg;                                                                                       \
            uint32_t sregLower = static_cast<uint32_t>(ElementCount);                                               \
            uint16_t repeatTime = CeilDivision(count, ElementCount);                                                \
            for (uint16_t i = 0; i < repeatTime; ++i) {                                                             \
                preg = MaskIntrinsic(sreg, POST_UPDATE);                                                            \
                vlds(vreg0, src0, i* sregLower, NORM);                                                              \
                vlds(vreg1, src1, i* sregLower, NORM);                                                              \
                vsub(vreg2, vreg0, vreg1, preg, MODE_ZEROING);                                                      \
                vrelu(vreg3, vreg2, preg, MODE_ZEROING);                                                            \
                vsts(vreg3, dst, i* sregLower, NORM_B16Arg, preg);                                                  \
            }                                                                                                       \
        }                                                                                                           \
    }

ASC_M300_DEFINE_BINARY_12(half, vector_f16, 128, plt_b16, NORM_B16)
ASC_M300_DEFINE_BINARY_12(float, vector_f32, 64, plt_b32, NORM_B32)

#undef ASC_M300_DEFINE_BINARY_12

// SubRelu::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void SubReluImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_BINARY_13(DataType, VectorType, MaskIntrinsic, ElementCount)                                  \
    template <typename T = DataType, bool isSetMask = true>                                                           \
    __aicore__ inline void SubReluImpl(                                                                               \
        __ubuf__ DataType* dst, __ubuf__ DataType* src0, __ubuf__ DataType* src1, const uint64_t mask[],              \
        const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)                                             \
    {                                                                                                                 \
        static_assert(SupportType<T, DataType>(), "SubRelu level-0 api only support half/float on current device");   \
        if constexpr (isSetMask) {                                                                                    \
            SetVectorMask<T>(mask[1], mask[0]);                                                                       \
        }                                                                                                             \
        __VEC_SCOPE__                                                                                                 \
        {                                                                                                             \
            ASC_M300_STAGE_BINARY_BITMASK_AND_STRIDES(VectorType, MaskIntrinsic, repeatParams);                       \
            ASC_M300_STAGE_BINARY_REPEAT_WITH_DEST_INPUT(                                                             \
                repeatTime, vreg0, src0, repeatParams, ElementCount, strideConfig0, preg, vreg1, src1, strideConfig1, \
                vreg2, vreg3, dst, strideConfig2)                                                                     \
        }                                                                                                             \
    }

ASC_M300_DEFINE_BINARY_13(half, vector_f16, movp_b16, 16)
ASC_M300_DEFINE_BINARY_13(float, vector_f32, movp_b32, 8)

#undef ASC_M300_DEFINE_BINARY_13

template <typename T, bool isSetMask = true>
__aicore__ inline void SubReluImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_BINARY_14(DataType, VectorType, MaskIntrinsic, ElementCount)                                  \
    template <typename T = DataType, bool isSetMask = true>                                                           \
    __aicore__ inline void SubReluImpl(                                                                               \
        __ubuf__ DataType* dst, __ubuf__ DataType* src0, __ubuf__ DataType* src1, const uint64_t mask,                \
        const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)                                             \
    {                                                                                                                 \
        static_assert(SupportType<T, DataType>(), "SubRelu level-0 api only support half/float on current device");   \
        __VEC_SCOPE__                                                                                                 \
        {                                                                                                             \
            VectorType vreg0;                                                                                         \
            ASC_M300_STAGE_BINARY_COUNT_MASK_AND_STRIDES(                                                             \
                VectorType, vreg1, vreg2, vreg3, mask, MaskIntrinsic, repeatParams);                                  \
            ASC_M300_STAGE_BINARY_REPEAT_WITH_DEST_INPUT(                                                             \
                repeatTime, vreg0, src0, repeatParams, ElementCount, strideConfig0, preg, vreg1, src1, strideConfig1, \
                vreg2, vreg3, dst, strideConfig2)                                                                     \
        }                                                                                                             \
    }

ASC_M300_DEFINE_BINARY_14(half, vector_f16, plt_b16, 16)
ASC_M300_DEFINE_BINARY_14(float, vector_f32, plt_b32, 8)

#undef ASC_M300_DEFINE_BINARY_14

/* **************************************************************************************************
 * AddDeqRelu                                             *
 * ************************************************************************************************* */
__aicore__ inline void AddDeqReluImpl(
    __ubuf__ half* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, const int32_t& count)
{
    (void)dst;
    (void)src0;
    (void)src1;
    (void)count;
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "unsupported AddDeqRelu on current device"); });
}

// AddDeqRelu::Level 0
template <bool isSetMask = true>
__aicore__ inline void AddDeqReluImpl(
    __ubuf__ half* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    (void)dst;
    (void)src0;
    (void)src1;
    (void)mask;
    (void)repeatTime;
    (void)repeatParams;
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "unsupported AddDeqRelu on current device"); });
}

template <bool isSetMask = true>
__aicore__ inline void AddDeqReluImpl(
    __ubuf__ half* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    (void)dst;
    (void)src0;
    (void)src1;
    (void)mask;
    (void)repeatTime;
    (void)repeatParams;
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "unsupported AddDeqRelu on current device"); });
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_BINARY_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_IMPL_H__
#endif

#undef ASC_M300_STAGE_BINARY_REPEAT_WITH_DEST_INPUT
#undef ASC_M300_STAGE_BINARY_REPEAT_FUSED_PAIR
#undef ASC_M300_STAGE_BINARY_REPEAT_WITH_CONVERSION
#undef ASC_M300_STAGE_BINARY_COUNT_MASK_AND_STRIDES
#undef ASC_M300_STAGE_BINARY_REPEAT_WITH_RESULT_LOAD
#undef ASC_M300_STAGE_BINARY_BITMASK_AND_STRIDES
#undef ASC_M300_STAGE_BINARY_LOAD_PAIR_WITH_BARRIER
