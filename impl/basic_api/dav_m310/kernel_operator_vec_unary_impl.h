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
 * \file kernel_operator_vec_unary_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_m310/kernel_operator_vec_unary_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_UNARY_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_UNARY_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_UNARY_IMPL_H
#include "../kernel_utils.h"
#include "kernel_operator_common_impl.h"
#include "../../../include/basic_api/kernel_struct_unary.h"
#include "../../../include/basic_api/kernel_tpipe.h"
#include "../../../include/basic_api/kernel_common.h"

namespace AscendC {
// Macros for block & repeat size
#define B8_BLOCK_SIZE 32
#define B16_BLOCK_SIZE 16
#define B32_BLOCK_SIZE 8

#define B8_REPEAT_SIZE 256
#define B16_REPEAT_SIZE 128
#define B32_REPEAT_SIZE 64

// Macros for level-0 api with type not support
#define UNARY_VEC_NORMAL_NOT_SUPPORT(FUNC_NAME)                                                                       \
    template <typename T, bool isSetMask = true>                                                                      \
    __aicore__ inline void FUNC_NAME(                                                                                 \
        __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask, const uint8_t repeatTime,                              \
        const UnaryRepeatParams& repeatParams)                                                                        \
    {                                                                                                                 \
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); }); \
    }

#define UNARY_VEC_BITWISE_NOT_SUPPORT(FUNC_NAME)                                                                      \
    template <typename T, bool isSetMask = true>                                                                      \
    __aicore__ inline void FUNC_NAME(                                                                                 \
        __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask[], const uint8_t repeatTime,                            \
        const UnaryRepeatParams& repeatParams)                                                                        \
    {                                                                                                                 \
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); }); \
    }

// Macros for level-2 api with type not support
#define UNARY_VEC_COUNTER_NOT_SUPPORT(FUNC_NAME)                                                                      \
    template <typename T>                                                                                             \
    __aicore__ inline void FUNC_NAME(__ubuf__ T* dst, __ubuf__ T* src, const int32_t& count)                          \
    {                                                                                                                 \
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); }); \
    }

// Macros for level-0 api
#define ASC_M310_DEFINE_UNARY_NORMAL(FUNC_NAME, OP_NAME, DATA_TYPE, REG_TYPE, UPDATE_MASK, BLOCK_ELEMS)  \
    template <typename T = DATA_TYPE, bool isSetMask = true>                                             \
    __aicore__ inline void FUNC_NAME(                                                                    \
        __ubuf__ DATA_TYPE* dst, __ubuf__ DATA_TYPE* src, const uint64_t mask, const uint8_t repeatTime, \
        const UnaryRepeatParams& repeatParams)                                                           \
    {                                                                                                    \
        __VEC_SCOPE__                                                                                    \
        {                                                                                                \
            REG_TYPE vreg0;                                                                              \
            REG_TYPE vreg1;                                                                              \
            uint32_t sreg = static_cast<uint32_t>(mask);                                                 \
            vector_bool preg = UPDATE_MASK(sreg, POST_UPDATE);                                           \
            uint32_t strideConfig0 = ((static_cast<uint32_t>(repeatParams.srcBlkStride)) << 16);         \
            uint32_t strideConfig1 = ((static_cast<uint32_t>(repeatParams.dstBlkStride)) << 16);         \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                           \
                vsldb(vreg0, src + i * repeatParams.srcRepStride * BLOCK_ELEMS, strideConfig0, preg);    \
                OP_NAME(vreg1, vreg0, preg, MODE_ZEROING);                                               \
                vsstb(vreg1, dst + i * repeatParams.dstRepStride * BLOCK_ELEMS, strideConfig1, preg);    \
            }                                                                                            \
        }                                                                                                \
    }

// for bit-wise 8-bit types: u8, s8. 4个mask，plds用normal连续存。preg长度256.
#define UNARY_VEC_BITWISE_T8(FUNC_NAME, OP_NAME, DATA_TYPE, REG_TYPE)                                       \
    template <typename T = DATA_TYPE, bool isSetMask = true>                                                \
    __aicore__ inline void FUNC_NAME(                                                                       \
        __ubuf__ DATA_TYPE* dst, __ubuf__ DATA_TYPE* src, const uint64_t mask[4], const uint8_t repeatTime, \
        const UnaryRepeatParams& repeatParams)                                                              \
    {                                                                                                       \
        __ubuf__ uint64_t* maskBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(TMP_UB_OFFSET, 4);      \
        maskBuf[0] = mask[0];                                                                               \
        maskBuf[1] = mask[1];                                                                               \
        maskBuf[2] = mask[2];                                                                               \
        maskBuf[3] = mask[3];                                                                               \
        event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));            \
        SetFlag<HardEvent::S_V>(eventIdSToV);                                                               \
        WaitFlag<HardEvent::S_V>(eventIdSToV);                                                              \
        __VEC_SCOPE__                                                                                       \
        {                                                                                                   \
            REG_TYPE vreg0;                                                                                 \
            REG_TYPE vreg1;                                                                                 \
            vector_bool preg;                                                                               \
            plds(preg, ((__ubuf__ uint32_t*)maskBuf), 0, NORM);                                             \
            uint32_t strideConfig0 = ((static_cast<uint32_t>(repeatParams.srcBlkStride)) << 16);            \
            uint32_t strideConfig1 = ((static_cast<uint32_t>(repeatParams.dstBlkStride)) << 16);            \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                              \
                vsldb(vreg0, src + i * repeatParams.srcRepStride * B8_BLOCK_SIZE, strideConfig0, preg);     \
                OP_NAME(vreg1, vreg0, preg, MODE_ZEROING);                                                  \
                vsstb(vreg1, dst + i * repeatParams.dstRepStride * B8_BLOCK_SIZE, strideConfig1, preg);     \
            }                                                                                               \
        }                                                                                                   \
        AscendCUtils::FreeTemporaryBuffer<uint64_t>(maskBuf);                                               \
    }

// For 32-bit elements, expand the loaded predicate before using it.
#define ASC_M310_LOAD_UNARY_MASK_B16(maskReg, maskBuf) \
    vector_bool maskReg;                               \
    plds(maskReg, ((__ubuf__ uint32_t*)maskBuf), 0, US)

#define ASC_M310_LOAD_UNARY_MASK_B32(maskReg, maskBuf) \
    vector_bool preg;                                  \
    vector_bool maskReg;                               \
    plds(preg, ((__ubuf__ uint32_t*)maskBuf), 0, US);  \
    punpack(maskReg, preg, LOWER)

#define ASC_M310_DEFINE_UNARY_BITWISE(FUNC_NAME, OP_NAME, DATA_TYPE, REG_TYPE, LOAD_MASK, MASK_REG, BLOCK_ELEMS) \
    template <typename T = DATA_TYPE, bool isSetMask = true>                                                     \
    __aicore__ inline void FUNC_NAME(                                                                            \
        __ubuf__ DATA_TYPE* dst, __ubuf__ DATA_TYPE* src, const uint64_t mask[], const uint8_t repeatTime,       \
        const UnaryRepeatParams& repeatParams)                                                                   \
    {                                                                                                            \
        __ubuf__ uint64_t* maskBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(TMP_UB_OFFSET, 2);           \
        maskBuf[0] = mask[0];                                                                                    \
        maskBuf[1] = mask[1];                                                                                    \
        event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));                 \
        SetFlag<HardEvent::S_V>(eventIdSToV);                                                                    \
        WaitFlag<HardEvent::S_V>(eventIdSToV);                                                                   \
        __VEC_SCOPE__                                                                                            \
        {                                                                                                        \
            REG_TYPE vreg0;                                                                                      \
            REG_TYPE vreg1;                                                                                      \
            LOAD_MASK(MASK_REG, maskBuf);                                                                        \
            uint32_t strideConfig0 = ((static_cast<uint32_t>(repeatParams.srcBlkStride)) << 16);                 \
            uint32_t strideConfig1 = ((static_cast<uint32_t>(repeatParams.dstBlkStride)) << 16);                 \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                                   \
                vsldb(vreg0, src + i * repeatParams.srcRepStride * BLOCK_ELEMS, strideConfig0, MASK_REG);        \
                OP_NAME(vreg1, vreg0, MASK_REG, MODE_ZEROING);                                                   \
                vsstb(vreg1, dst + i * repeatParams.dstRepStride * BLOCK_ELEMS, strideConfig1, MASK_REG);        \
            }                                                                                                    \
        }                                                                                                        \
        AscendCUtils::FreeTemporaryBuffer<uint64_t>(maskBuf);                                                    \
    }

#define ASC_M310_DEFINE_UNARY_COUNTER(FUNC_NAME, OP_NAME, DATA_TYPE, REG_TYPE, UPDATE_MASK, REPEAT_ELEMS, STORE_MODE) \
    __aicore__ inline void FUNC_NAME(__ubuf__ DATA_TYPE* dst, __ubuf__ DATA_TYPE* src, const int32_t& count)          \
    {                                                                                                                 \
        __VEC_SCOPE__                                                                                                 \
        {                                                                                                             \
            REG_TYPE vreg0;                                                                                           \
            REG_TYPE vreg1;                                                                                           \
            uint32_t sreg = static_cast<uint32_t>(count);                                                             \
            vector_bool preg;                                                                                         \
            uint32_t sregLower = static_cast<uint32_t>(REPEAT_ELEMS);                                                 \
            uint16_t repeatTime = CeilDivision(count, REPEAT_ELEMS);                                                  \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                                        \
                preg = UPDATE_MASK(sreg, POST_UPDATE);                                                                \
                vlds(vreg0, src, i* sregLower, NORM);                                                                 \
                OP_NAME(vreg1, vreg0, preg, MODE_ZEROING);                                                            \
                vsts(vreg1, dst, i* sregLower, STORE_MODE, preg);                                                     \
            }                                                                                                         \
        }                                                                                                             \
    }

/* **************************************************************************************************
 * Exp                                             *
 * ************************************************************************************************* */
// Exp::Level 0
UNARY_VEC_NORMAL_NOT_SUPPORT(ExpImpl);
UNARY_VEC_BITWISE_NOT_SUPPORT(ExpImpl);

ASC_M310_DEFINE_UNARY_NORMAL(ExpImpl, vexp, half, vector_f16, plt_b16, B16_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_NORMAL(ExpImpl, vexp, float, vector_f32, plt_b32, B32_BLOCK_SIZE);

ASC_M310_DEFINE_UNARY_BITWISE(ExpImpl, vexp, half, vector_f16, ASC_M310_LOAD_UNARY_MASK_B16, preg, B16_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_BITWISE(ExpImpl, vexp, float, vector_f32, ASC_M310_LOAD_UNARY_MASK_B32, preg1, B32_BLOCK_SIZE);

// // Exp::Level 2
UNARY_VEC_COUNTER_NOT_SUPPORT(ExpImpl);

ASC_M310_DEFINE_UNARY_COUNTER(ExpImpl, vexp, half, vector_f16, plt_b16, B16_REPEAT_SIZE, NORM_B16);
ASC_M310_DEFINE_UNARY_COUNTER(ExpImpl, vexp, float, vector_f32, plt_b32, B32_REPEAT_SIZE, NORM_B32);

// /* **************************************************************************************************
//  * Ln                                             *
//  * ************************************************************************************************* */
// // Ln::Level 0
UNARY_VEC_NORMAL_NOT_SUPPORT(LnImpl);
UNARY_VEC_BITWISE_NOT_SUPPORT(LnImpl);

ASC_M310_DEFINE_UNARY_NORMAL(LnImpl, vln, half, vector_f16, plt_b16, B16_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_NORMAL(LnImpl, vln, float, vector_f32, plt_b32, B32_BLOCK_SIZE);

ASC_M310_DEFINE_UNARY_BITWISE(LnImpl, vln, half, vector_f16, ASC_M310_LOAD_UNARY_MASK_B16, preg, B16_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_BITWISE(LnImpl, vln, float, vector_f32, ASC_M310_LOAD_UNARY_MASK_B32, preg1, B32_BLOCK_SIZE);

// // Ln::Level 2
UNARY_VEC_COUNTER_NOT_SUPPORT(LnImpl);

ASC_M310_DEFINE_UNARY_COUNTER(LnImpl, vln, half, vector_f16, plt_b16, B16_REPEAT_SIZE, NORM_B16);
ASC_M310_DEFINE_UNARY_COUNTER(LnImpl, vln, float, vector_f32, plt_b32, B32_REPEAT_SIZE, NORM_B32);

// /* **************************************************************************************************
//  * Abs                                             *
//  * ************************************************************************************************* */
// // Abs::Level 0
UNARY_VEC_NORMAL_NOT_SUPPORT(AbsImpl);
UNARY_VEC_BITWISE_NOT_SUPPORT(AbsImpl);

ASC_M310_DEFINE_UNARY_NORMAL(AbsImpl, vabs, int8_t, vector_s8, plt_b8, B8_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_NORMAL(AbsImpl, vabs, half, vector_f16, plt_b16, B16_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_NORMAL(AbsImpl, vabs, float, vector_f32, plt_b32, B32_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_NORMAL(AbsImpl, vabs, int16_t, vector_s16, plt_b16, B16_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_NORMAL(AbsImpl, vabs, int32_t, vector_s32, plt_b32, B32_BLOCK_SIZE);

UNARY_VEC_BITWISE_T8(AbsImpl, vabs, int8_t, vector_s8);
ASC_M310_DEFINE_UNARY_BITWISE(AbsImpl, vabs, half, vector_f16, ASC_M310_LOAD_UNARY_MASK_B16, preg, B16_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_BITWISE(AbsImpl, vabs, float, vector_f32, ASC_M310_LOAD_UNARY_MASK_B32, preg1, B32_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_BITWISE(AbsImpl, vabs, int16_t, vector_s16, ASC_M310_LOAD_UNARY_MASK_B16, preg, B16_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_BITWISE(AbsImpl, vabs, int32_t, vector_s32, ASC_M310_LOAD_UNARY_MASK_B32, preg1, B32_BLOCK_SIZE);

// // Abs::Level 2
UNARY_VEC_COUNTER_NOT_SUPPORT(AbsImpl);

ASC_M310_DEFINE_UNARY_COUNTER(AbsImpl, vabs, int8_t, vector_s8, plt_b8, B8_REPEAT_SIZE, NORM_B8);
ASC_M310_DEFINE_UNARY_COUNTER(AbsImpl, vabs, half, vector_f16, plt_b16, B16_REPEAT_SIZE, NORM_B16);
ASC_M310_DEFINE_UNARY_COUNTER(AbsImpl, vabs, float, vector_f32, plt_b32, B32_REPEAT_SIZE, NORM_B32);
ASC_M310_DEFINE_UNARY_COUNTER(AbsImpl, vabs, int16_t, vector_s16, plt_b16, B16_REPEAT_SIZE, NORM_B16);
ASC_M310_DEFINE_UNARY_COUNTER(AbsImpl, vabs, int32_t, vector_s32, plt_b32, B32_REPEAT_SIZE, NORM_B32);

// /* **************************************************************************************************
//  * Rec                                             *
//  * ************************************************************************************************* */
// // Rec::Level 0
UNARY_VEC_NORMAL_NOT_SUPPORT(ReciprocalImpl);
UNARY_VEC_BITWISE_NOT_SUPPORT(ReciprocalImpl);

ASC_M310_DEFINE_UNARY_NORMAL(ReciprocalImpl, vrec, half, vector_f16, plt_b16, B16_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_NORMAL(ReciprocalImpl, vrec, float, vector_f32, plt_b32, B32_BLOCK_SIZE);

ASC_M310_DEFINE_UNARY_BITWISE(
    ReciprocalImpl, vrec, half, vector_f16, ASC_M310_LOAD_UNARY_MASK_B16, preg, B16_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_BITWISE(
    ReciprocalImpl, vrec, float, vector_f32, ASC_M310_LOAD_UNARY_MASK_B32, preg1, B32_BLOCK_SIZE);

// // Rec::Level 2
UNARY_VEC_COUNTER_NOT_SUPPORT(ReciprocalImpl);

ASC_M310_DEFINE_UNARY_COUNTER(ReciprocalImpl, vrec, half, vector_f16, plt_b16, B16_REPEAT_SIZE, NORM_B16);
ASC_M310_DEFINE_UNARY_COUNTER(ReciprocalImpl, vrec, float, vector_f32, plt_b32, B32_REPEAT_SIZE, NORM_B32);

// /* **************************************************************************************************
//  * Sqrt                                             *
//  * ************************************************************************************************* */
// // Sqrt::Level 0
UNARY_VEC_NORMAL_NOT_SUPPORT(SqrtImpl);
UNARY_VEC_BITWISE_NOT_SUPPORT(SqrtImpl);

ASC_M310_DEFINE_UNARY_NORMAL(SqrtImpl, vsqrt, half, vector_f16, plt_b16, B16_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_NORMAL(SqrtImpl, vsqrt, float, vector_f32, plt_b32, B32_BLOCK_SIZE);

ASC_M310_DEFINE_UNARY_BITWISE(SqrtImpl, vsqrt, half, vector_f16, ASC_M310_LOAD_UNARY_MASK_B16, preg, B16_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_BITWISE(SqrtImpl, vsqrt, float, vector_f32, ASC_M310_LOAD_UNARY_MASK_B32, preg1, B32_BLOCK_SIZE);

// // Sqrt::Level 2
UNARY_VEC_COUNTER_NOT_SUPPORT(SqrtImpl);

ASC_M310_DEFINE_UNARY_COUNTER(SqrtImpl, vsqrt, half, vector_f16, plt_b16, B16_REPEAT_SIZE, NORM_B16);
ASC_M310_DEFINE_UNARY_COUNTER(SqrtImpl, vsqrt, float, vector_f32, plt_b32, B32_REPEAT_SIZE, NORM_B32);

// // /* **************************************************************************************************
// //  * Rsqrt                                             *
// //  * ************************************************************************************************* */
// // // Rsqrt::Level 0
UNARY_VEC_NORMAL_NOT_SUPPORT(RsqrtImpl);
UNARY_VEC_BITWISE_NOT_SUPPORT(RsqrtImpl);

ASC_M310_DEFINE_UNARY_NORMAL(RsqrtImpl, vrsqrt, half, vector_f16, plt_b16, B16_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_NORMAL(RsqrtImpl, vrsqrt, float, vector_f32, plt_b32, B32_BLOCK_SIZE);

ASC_M310_DEFINE_UNARY_BITWISE(RsqrtImpl, vrsqrt, half, vector_f16, ASC_M310_LOAD_UNARY_MASK_B16, preg, B16_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_BITWISE(
    RsqrtImpl, vrsqrt, float, vector_f32, ASC_M310_LOAD_UNARY_MASK_B32, preg1, B32_BLOCK_SIZE);

// // Rec::Level 2
UNARY_VEC_COUNTER_NOT_SUPPORT(RsqrtImpl);

ASC_M310_DEFINE_UNARY_COUNTER(RsqrtImpl, vrsqrt, half, vector_f16, plt_b16, B16_REPEAT_SIZE, NORM_B16);
ASC_M310_DEFINE_UNARY_COUNTER(RsqrtImpl, vrsqrt, float, vector_f32, plt_b32, B32_REPEAT_SIZE, NORM_B32);

// // /* **************************************************************************************************
// //  * Not                                             *
// //  * ************************************************************************************************* */
// // // Not::Level 0
UNARY_VEC_NORMAL_NOT_SUPPORT(NotImpl);
UNARY_VEC_BITWISE_NOT_SUPPORT(NotImpl);

ASC_M310_DEFINE_UNARY_NORMAL(NotImpl, vnot, uint8_t, vector_u8, plt_b8, B8_BLOCK_SIZE)
ASC_M310_DEFINE_UNARY_NORMAL(NotImpl, vnot, int8_t, vector_s8, plt_b8, B8_BLOCK_SIZE)
ASC_M310_DEFINE_UNARY_NORMAL(NotImpl, vnot, uint16_t, vector_u16, plt_b16, B16_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_NORMAL(NotImpl, vnot, int16_t, vector_s16, plt_b16, B16_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_NORMAL(NotImpl, vnot, half, vector_f16, plt_b16, B16_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_NORMAL(NotImpl, vnot, float, vector_f32, plt_b32, B32_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_NORMAL(NotImpl, vnot, uint32_t, vector_u32, plt_b32, B32_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_NORMAL(NotImpl, vnot, int32_t, vector_s32, plt_b32, B32_BLOCK_SIZE);

UNARY_VEC_BITWISE_T8(NotImpl, vnot, uint8_t, vector_u8)
UNARY_VEC_BITWISE_T8(NotImpl, vnot, int8_t, vector_s8)
ASC_M310_DEFINE_UNARY_BITWISE(NotImpl, vnot, uint16_t, vector_u16, ASC_M310_LOAD_UNARY_MASK_B16, preg, B16_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_BITWISE(NotImpl, vnot, int16_t, vector_s16, ASC_M310_LOAD_UNARY_MASK_B16, preg, B16_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_BITWISE(NotImpl, vnot, half, vector_f16, ASC_M310_LOAD_UNARY_MASK_B16, preg, B16_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_BITWISE(NotImpl, vnot, float, vector_f32, ASC_M310_LOAD_UNARY_MASK_B32, preg1, B32_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_BITWISE(NotImpl, vnot, uint32_t, vector_u32, ASC_M310_LOAD_UNARY_MASK_B32, preg1, B32_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_BITWISE(NotImpl, vnot, int32_t, vector_s32, ASC_M310_LOAD_UNARY_MASK_B32, preg1, B32_BLOCK_SIZE);

// // // Not::Level 2
UNARY_VEC_COUNTER_NOT_SUPPORT(NotImpl);

ASC_M310_DEFINE_UNARY_COUNTER(NotImpl, vnot, uint8_t, vector_u8, plt_b8, B8_REPEAT_SIZE, NORM_B8);
ASC_M310_DEFINE_UNARY_COUNTER(NotImpl, vnot, int8_t, vector_s8, plt_b8, B8_REPEAT_SIZE, NORM_B8);
ASC_M310_DEFINE_UNARY_COUNTER(NotImpl, vnot, uint16_t, vector_u16, plt_b16, B16_REPEAT_SIZE, NORM_B16);
ASC_M310_DEFINE_UNARY_COUNTER(NotImpl, vnot, int16_t, vector_s16, plt_b16, B16_REPEAT_SIZE, NORM_B16);
ASC_M310_DEFINE_UNARY_COUNTER(NotImpl, vnot, half, vector_f16, plt_b16, B16_REPEAT_SIZE, NORM_B16);
ASC_M310_DEFINE_UNARY_COUNTER(NotImpl, vnot, float, vector_f32, plt_b32, B32_REPEAT_SIZE, NORM_B32);
ASC_M310_DEFINE_UNARY_COUNTER(NotImpl, vnot, uint32_t, vector_u32, plt_b32, B32_REPEAT_SIZE, NORM_B32);
ASC_M310_DEFINE_UNARY_COUNTER(NotImpl, vnot, int32_t, vector_s32, plt_b32, B32_REPEAT_SIZE, NORM_B32);

// /* **************************************************************************************************
//  * Relu                                             *
//  * ************************************************************************************************* */
// // Relu::Level 0
// // bit mode
UNARY_VEC_NORMAL_NOT_SUPPORT(ReluImpl);
UNARY_VEC_BITWISE_NOT_SUPPORT(ReluImpl);

ASC_M310_DEFINE_UNARY_NORMAL(ReluImpl, vrelu, half, vector_f16, plt_b16, B16_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_NORMAL(ReluImpl, vrelu, float, vector_f32, plt_b32, B32_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_NORMAL(ReluImpl, vrelu, int32_t, vector_s32, plt_b32, B32_BLOCK_SIZE);

ASC_M310_DEFINE_UNARY_BITWISE(ReluImpl, vrelu, half, vector_f16, ASC_M310_LOAD_UNARY_MASK_B16, preg, B16_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_BITWISE(ReluImpl, vrelu, float, vector_f32, ASC_M310_LOAD_UNARY_MASK_B32, preg1, B32_BLOCK_SIZE);
ASC_M310_DEFINE_UNARY_BITWISE(
    ReluImpl, vrelu, int32_t, vector_s32, ASC_M310_LOAD_UNARY_MASK_B32, preg1, B32_BLOCK_SIZE);

// // Relu::Level 2
UNARY_VEC_COUNTER_NOT_SUPPORT(ReluImpl);

ASC_M310_DEFINE_UNARY_COUNTER(ReluImpl, vrelu, half, vector_f16, plt_b16, B16_REPEAT_SIZE, NORM_B16);
ASC_M310_DEFINE_UNARY_COUNTER(ReluImpl, vrelu, float, vector_f32, plt_b32, B32_REPEAT_SIZE, NORM_B32);
ASC_M310_DEFINE_UNARY_COUNTER(ReluImpl, vrelu, int32_t, vector_s32, plt_b32, B32_REPEAT_SIZE, NORM_B32);
#undef ASC_M310_DEFINE_UNARY_NORMAL

#undef ASC_M310_DEFINE_UNARY_COUNTER

#undef ASC_M310_DEFINE_UNARY_BITWISE
#undef ASC_M310_LOAD_UNARY_MASK_B32
#undef ASC_M310_LOAD_UNARY_MASK_B16

} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_UNARY_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_UNARY_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_UNARY_IMPL_H__
#endif
