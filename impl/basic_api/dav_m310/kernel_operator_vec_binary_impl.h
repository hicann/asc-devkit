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
    "impl/basic_api/dav_m310/kernel_operator_vec_binary_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_BINARY_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_BINARY_IMPL_H
#include "../kernel_utils.h"
#include "kernel_operator_common_impl.h"
#include "../../../include/basic_api/kernel_struct_binary.h"
#include "../../../include/basic_api/kernel_tpipe.h"
#include "../../../include/basic_api/kernel_common.h"

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
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

// Emit the original overloads with their existing vector scope and intrinsic order.
#define ASC_M310_DEFINE_BINARY_BITMASK_B16(DataType, Function, VectorType, VectorIntrinsic)              \
    template <typename T = DataType, bool isSetMask = true>                                              \
    __aicore__ inline void Function(                                                                     \
        __ubuf__ DataType* dst, __ubuf__ DataType* src0, __ubuf__ DataType* src1, const uint64_t mask[], \
        const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)                                \
    {                                                                                                    \
        __ubuf__ uint8_t* tmpBuf = AscendCUtils::GetTemporaryBufferAddr<uint8_t>(TMP_UB_OFFSET, 16);     \
        *((__ubuf__ uint64_t*)tmpBuf) = mask[0];                                                         \
        *((__ubuf__ uint64_t*)tmpBuf + 1) = mask[1];                                                     \
        event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));         \
        SetFlag<HardEvent::S_V>(eventIdSToV);                                                            \
        WaitFlag<HardEvent::S_V>(eventIdSToV);                                                           \
        __VEC_SCOPE__                                                                                    \
        {                                                                                                \
            VectorType vreg0;                                                                            \
            VectorType vreg1;                                                                            \
            VectorType vreg2;                                                                            \
            vector_bool preg;                                                                            \
            plds(preg, ((__ubuf__ uint32_t*)tmpBuf), 0, US);                                             \
            uint32_t strideConfig0 = ((static_cast<uint32_t>(repeatParams.src0BlkStride)) << 16);        \
            uint32_t strideConfig1 = ((static_cast<uint32_t>(repeatParams.src1BlkStride)) << 16);        \
            uint32_t strideConfig2 = ((static_cast<uint32_t>(repeatParams.dstBlkStride)) << 16);         \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                           \
                vsldb(vreg0, src0 + i * repeatParams.src0RepStride * 16, strideConfig0, preg);           \
                vsldb(vreg1, src1 + i * repeatParams.src1RepStride * 16, strideConfig1, preg);           \
                VectorIntrinsic(vreg2, vreg0, vreg1, preg, MODE_ZEROING);                                \
                vsstb(vreg2, dst + i * repeatParams.dstRepStride * 16, strideConfig2, preg);             \
            }                                                                                            \
        }                                                                                                \
        AscendCUtils::FreeTemporaryBuffer<uint8_t>(tmpBuf);                                              \
    }

ASC_M310_DEFINE_BINARY_BITMASK_B16(half, AddImpl, vector_f16, vadd)
ASC_M310_DEFINE_BINARY_BITMASK_B16(int16_t, AddImpl, vector_s16, vadd)

#define ASC_M310_DEFINE_BINARY_BITMASK_B32(DataType, Function, VectorType, VectorIntrinsic)              \
    template <typename T = DataType, bool isSetMask = true>                                              \
    __aicore__ inline void Function(                                                                     \
        __ubuf__ DataType* dst, __ubuf__ DataType* src0, __ubuf__ DataType* src1, const uint64_t mask[], \
        const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)                                \
    {                                                                                                    \
        __ubuf__ uint8_t* tmpBuf = AscendCUtils::GetTemporaryBufferAddr<uint8_t>(TMP_UB_OFFSET, 16);     \
        *((__ubuf__ uint64_t*)tmpBuf) = mask[0];                                                         \
        *((__ubuf__ uint64_t*)tmpBuf + 1) = mask[1];                                                     \
        event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));         \
        SetFlag<HardEvent::S_V>(eventIdSToV);                                                            \
        WaitFlag<HardEvent::S_V>(eventIdSToV);                                                           \
        __VEC_SCOPE__                                                                                    \
        {                                                                                                \
            VectorType vreg0;                                                                            \
            VectorType vreg1;                                                                            \
            VectorType vreg2;                                                                            \
            vector_bool preg;                                                                            \
            vector_bool preg1;                                                                           \
            plds(preg, ((__ubuf__ uint32_t*)tmpBuf), 0, US);                                             \
            punpack(preg1, preg, LOWER);                                                                 \
            uint32_t strideConfig0 = ((static_cast<uint32_t>(repeatParams.src0BlkStride)) << 16);        \
            uint32_t strideConfig1 = ((static_cast<uint32_t>(repeatParams.src1BlkStride)) << 16);        \
            uint32_t strideConfig2 = ((static_cast<uint32_t>(repeatParams.dstBlkStride)) << 16);         \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                           \
                vsldb(vreg0, src0 + i * repeatParams.src0RepStride * 8, strideConfig0, preg1);           \
                vsldb(vreg1, src1 + i * repeatParams.src1RepStride * 8, strideConfig1, preg1);           \
                VectorIntrinsic(vreg2, vreg0, vreg1, preg1, MODE_ZEROING);                               \
                vsstb(vreg2, dst + i * repeatParams.dstRepStride * 8, strideConfig2, preg1);             \
            }                                                                                            \
        }                                                                                                \
        AscendCUtils::FreeTemporaryBuffer<uint8_t>(tmpBuf);                                              \
    }

ASC_M310_DEFINE_BINARY_BITMASK_B32(int32_t, AddImpl, vector_s32, vadd)
ASC_M310_DEFINE_BINARY_BITMASK_B32(float, AddImpl, vector_f32, vadd)

template <typename T, bool isSetMask = true>
__aicore__ inline void AddImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

#define ASC_M310_DEFINE_BINARY_CONTINUOUS_MASK(                                                          \
    DataType, Function, VectorType, MaskIntrinsic, ElementCount, VectorIntrinsic)                        \
    template <typename T = DataType, bool isSetMask = true>                                              \
    __aicore__ inline void Function(                                                                     \
        __ubuf__ DataType* dst, __ubuf__ DataType* src0, __ubuf__ DataType* src1, const uint64_t mask,   \
        const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)                                \
    {                                                                                                    \
        __VEC_SCOPE__                                                                                    \
        {                                                                                                \
            VectorType vreg0;                                                                            \
            VectorType vreg1;                                                                            \
            VectorType vreg2;                                                                            \
            uint32_t sreg = static_cast<uint32_t>(mask);                                                 \
            vector_bool preg = MaskIntrinsic(sreg, POST_UPDATE);                                         \
            uint32_t strideConfig0 = ((static_cast<uint32_t>(repeatParams.src0BlkStride)) << 16);        \
            uint32_t strideConfig1 = ((static_cast<uint32_t>(repeatParams.src1BlkStride)) << 16);        \
            uint32_t strideConfig2 = ((static_cast<uint32_t>(repeatParams.dstBlkStride)) << 16);         \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                           \
                vsldb(vreg0, src0 + i * repeatParams.src0RepStride * ElementCount, strideConfig0, preg); \
                vsldb(vreg1, src1 + i * repeatParams.src1RepStride * ElementCount, strideConfig1, preg); \
                VectorIntrinsic(vreg2, vreg0, vreg1, preg, MODE_ZEROING);                                \
                vsstb(vreg2, dst + i * repeatParams.dstRepStride * ElementCount, strideConfig2, preg);   \
            }                                                                                            \
        }                                                                                                \
    }

ASC_M310_DEFINE_BINARY_CONTINUOUS_MASK(half, AddImpl, vector_f16, plt_b16, 16, vadd)
ASC_M310_DEFINE_BINARY_CONTINUOUS_MASK(int16_t, AddImpl, vector_s16, plt_b16, 16, vadd)
ASC_M310_DEFINE_BINARY_CONTINUOUS_MASK(int32_t, AddImpl, vector_s32, plt_b32, 8, vadd)
ASC_M310_DEFINE_BINARY_CONTINUOUS_MASK(float, AddImpl, vector_f32, plt_b32, 8, vadd)
/* **************************************************************************************************
 * Sub                                             *
 * ************************************************************************************************* */
// Sub::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void SubImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_BINARY_BITMASK_B16(half, SubImpl, vector_f16, vsub)
ASC_M310_DEFINE_BINARY_BITMASK_B16(int16_t, SubImpl, vector_s16, vsub)
ASC_M310_DEFINE_BINARY_BITMASK_B32(int32_t, SubImpl, vector_s32, vsub)
ASC_M310_DEFINE_BINARY_BITMASK_B32(float, SubImpl, vector_f32, vsub)

template <typename T, bool isSetMask = true>
__aicore__ inline void SubImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_BINARY_CONTINUOUS_MASK(half, SubImpl, vector_f16, plt_b16, 16, vsub)
ASC_M310_DEFINE_BINARY_CONTINUOUS_MASK(int16_t, SubImpl, vector_s16, plt_b16, 16, vsub)
ASC_M310_DEFINE_BINARY_CONTINUOUS_MASK(int32_t, SubImpl, vector_s32, plt_b32, 8, vsub)
ASC_M310_DEFINE_BINARY_CONTINUOUS_MASK(float, SubImpl, vector_f32, plt_b32, 8, vsub)

/* **************************************************************************************************
 * Mul                                             *
 * ************************************************************************************************* */
// Mul::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void MulImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_BINARY_BITMASK_B16(half, MulImpl, vector_f16, vmul)
ASC_M310_DEFINE_BINARY_BITMASK_B16(int16_t, MulImpl, vector_s16, vmul)
ASC_M310_DEFINE_BINARY_BITMASK_B32(int32_t, MulImpl, vector_s32, vmul)
ASC_M310_DEFINE_BINARY_BITMASK_B32(float, MulImpl, vector_f32, vmul)

template <typename T, bool isSetMask = true>
__aicore__ inline void MulImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_BINARY_CONTINUOUS_MASK(half, MulImpl, vector_f16, plt_b16, 16, vmul)
ASC_M310_DEFINE_BINARY_CONTINUOUS_MASK(int16_t, MulImpl, vector_s16, plt_b16, 16, vmul)
ASC_M310_DEFINE_BINARY_CONTINUOUS_MASK(int32_t, MulImpl, vector_s32, plt_b32, 8, vmul)
ASC_M310_DEFINE_BINARY_CONTINUOUS_MASK(float, MulImpl, vector_f32, plt_b32, 8, vmul)

/* **************************************************************************************************
 * Div                                             *
 * ************************************************************************************************* */
// Div::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void DivImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_BINARY_BITMASK_B16(half, DivImpl, vector_f16, vdiv)
ASC_M310_DEFINE_BINARY_BITMASK_B32(float, DivImpl, vector_f32, vdiv)

template <typename T, bool isSetMask = true>
__aicore__ inline void DivImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_BINARY_CONTINUOUS_MASK(half, DivImpl, vector_f16, plt_b16, 16, vdiv)

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
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

template <typename T, bool isSetMask = true>
__aicore__ inline void MaxImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_BINARY_BITMASK_B16(half, MaxImpl, vector_f16, vmax)
ASC_M310_DEFINE_BINARY_BITMASK_B16(int16_t, MaxImpl, vector_s16, vmax)
ASC_M310_DEFINE_BINARY_BITMASK_B32(int32_t, MaxImpl, vector_s32, vmax)
ASC_M310_DEFINE_BINARY_BITMASK_B32(float, MaxImpl, vector_f32, vmax)
ASC_M310_DEFINE_BINARY_CONTINUOUS_MASK(half, MaxImpl, vector_f16, plt_b16, 16, vmax)
ASC_M310_DEFINE_BINARY_CONTINUOUS_MASK(int16_t, MaxImpl, vector_s16, plt_b16, 16, vmax)
ASC_M310_DEFINE_BINARY_CONTINUOUS_MASK(int32_t, MaxImpl, vector_s32, plt_b32, 8, vmax)
ASC_M310_DEFINE_BINARY_CONTINUOUS_MASK(float, MaxImpl, vector_f32, plt_b32, 8, vmax)

/* **************************************************************************************************
 * Min                                             *
 * ************************************************************************************************* */
// Min::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void MinImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_BINARY_BITMASK_B16(half, MinImpl, vector_f16, vmin)
ASC_M310_DEFINE_BINARY_BITMASK_B16(int16_t, MinImpl, vector_s16, vmin)
ASC_M310_DEFINE_BINARY_BITMASK_B32(int32_t, MinImpl, vector_s32, vmin)
ASC_M310_DEFINE_BINARY_BITMASK_B32(float, MinImpl, vector_f32, vmin)

#undef ASC_M310_DEFINE_BINARY_BITMASK_B32

template <typename T, bool isSetMask = true>
__aicore__ inline void MinImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_BINARY_CONTINUOUS_MASK(half, MinImpl, vector_f16, plt_b16, 16, vmin)
ASC_M310_DEFINE_BINARY_CONTINUOUS_MASK(int16_t, MinImpl, vector_s16, plt_b16, 16, vmin)
ASC_M310_DEFINE_BINARY_CONTINUOUS_MASK(int32_t, MinImpl, vector_s32, plt_b32, 8, vmin)
ASC_M310_DEFINE_BINARY_CONTINUOUS_MASK(float, MinImpl, vector_f32, plt_b32, 8, vmin)

/* **************************************************************************************************
 * And                                             *
 * ************************************************************************************************* */
// And::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void AndImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_BINARY_BITMASK_B16(int16_t, AndImpl, vector_s16, vand)
ASC_M310_DEFINE_BINARY_BITMASK_B16(uint16_t, AndImpl, vector_u16, vand)

template <typename T, bool isSetMask = true>
__aicore__ inline void AndImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_BINARY_CONTINUOUS_MASK(int16_t, AndImpl, vector_s16, plt_b16, 16, vand)
ASC_M310_DEFINE_BINARY_CONTINUOUS_MASK(uint16_t, AndImpl, vector_u16, plt_b16, 16, vand)

/* **************************************************************************************************
 * Or                                             *
 * ************************************************************************************************* */
// Or::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void OrImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_BINARY_BITMASK_B16(int16_t, OrImpl, vector_s16, vor)
ASC_M310_DEFINE_BINARY_BITMASK_B16(uint16_t, OrImpl, vector_u16, vor)

#undef ASC_M310_DEFINE_BINARY_BITMASK_B16

template <typename T, bool isSetMask = true>
__aicore__ inline void OrImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_BINARY_CONTINUOUS_MASK(int16_t, OrImpl, vector_s16, plt_b16, 16, vor)
ASC_M310_DEFINE_BINARY_CONTINUOUS_MASK(uint16_t, OrImpl, vector_u16, plt_b16, 16, vor)

#undef ASC_M310_DEFINE_BINARY_CONTINUOUS_MASK

/* **************************************************************************************************
 * AddRelu                                             *
 * ************************************************************************************************* */
// AddRelu::Level 2
template <typename T, bool isSetMask = true>
__aicore__ inline void AddReluImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

template <typename T, bool isSetMask = true>
__aicore__ inline void AddReluImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

/* **************************************************************************************************
 * FusedMulAdd                                             *
 * ************************************************************************************************* */
// FusedMulAdd::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void FusedMulAddImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "FusedMulAdd is not supported on current device!"); });
}

template <typename T, bool isSetMask = true>
__aicore__ inline void FusedMulAddImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "FusedMulAdd is not supported on current device!"); });
}

/* **************************************************************************************************
 * FusedMulAddRelu                                             *
 * ************************************************************************************************* */
// FusedMulAddRelu::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void FusedMulAddReluImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "FusedMulAddRelu is not supported on current device!"); });
}

template <typename T, bool isSetMask = true>
__aicore__ inline void FusedMulAddReluImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "FusedMulAddRelu is not supported on current device!"); });
}
/* **************************************************************************************************
 * MulAddDst                                             *
 * ************************************************************************************************* */
// MulAddDst::Level 0
template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void MulAddDstImpl(
    __ubuf__ T* dst, __ubuf__ U* src0, __ubuf__ U* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "MulAddDst is not supported on current device!"); });
}

template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void MulAddDstImpl(
    __ubuf__ T* dst, __ubuf__ U* src0, __ubuf__ U* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "MulAddDst is not supported on current device!"); });
}

/* **************************************************************************************************
 * SubRelu                                             *
 * ************************************************************************************************* */
// SubRelu::Level 2
template <typename T>
__aicore__ inline void SubReluImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SubRelu is not supported on current device"); });
}

// SubRelu::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void SubReluImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SubRelu is not supported on current device"); });
}

template <typename T, bool isSetMask = true>
__aicore__ inline void SubReluImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SubRelu is not supported on current device"); });
}

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
