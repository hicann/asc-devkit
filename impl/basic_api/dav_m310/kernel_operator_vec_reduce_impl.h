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
 * \file kernel_operator_vec_reduce_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_m310/kernel_operator_vec_reduce_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_REDUCE_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_REDUCE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_REDUCE_IMPL_H

#include "../../../include/basic_api/kernel_tpipe.h"
#include "../../../include/basic_api/kernel_common.h"

// Expand shared register stages in place to preserve the compiler input and instruction order.
#define ASC_M310_STAGE_REDUCE_INIT_F16_REGISTERS(tempBuf, srcBlkStride) \
    vector_f16 vreg0;                                                   \
    vector_f16 vreg1;                                                   \
    vector_align ureg;                                                  \
    vector_bool preg;                                                   \
    plds(preg, ((__ubuf__ uint32_t*)tempBuf), 0, US);                   \
    uint32_t strideConfig = ((static_cast<uint32_t>(srcBlkStride)) << 16)

#define ASC_M310_STAGE_REDUCE_INIT_F32_REGISTERS(vreg0, vreg1, ureg, preg, tempBuf, srcBlkStride) \
    vector_f32 vreg0;                                                                             \
    vector_f32 vreg1;                                                                             \
    vector_align ureg;                                                                            \
    vector_bool preg;                                                                             \
    vector_bool preg1;                                                                            \
    plds(preg1, ((__ubuf__ uint32_t*)tempBuf), 0, US);                                            \
    punpack(preg, preg1, LOWER);                                                                  \
    uint32_t strideConfig = ((static_cast<uint32_t>(srcBlkStride)) << 16)

#define ASC_M310_STAGE_REDUCE_STAGE_BITMASK(mask)                                                 \
    __ubuf__ uint8_t* tempBuf = AscendCUtils::GetTemporaryBufferAddr<uint8_t>(TMP_UB_OFFSET, 16); \
    (*(__ubuf__ uint64_t*)((__ubuf__ uint64_t*)tempBuf)) = (static_cast<uint64_t>(mask[0]));      \
    (*(__ubuf__ uint64_t*)((__ubuf__ uint64_t*)tempBuf + 1)) = (static_cast<uint64_t>(mask[1]));  \
    event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));      \
    SetFlag<HardEvent::S_V>(eventIdSToV);                                                         \
    WaitFlag<HardEvent::S_V>(eventIdSToV)

#define ASC_M310_STAGE_REDUCE_INIT_BITMASK_REPEAT(                               \
    repeatTime, srcRepStride, ONE_BLK_HALF_NUMArg, dstRepStride, DataType, src)  \
    int32_t newRepeat = repeatTime;                                              \
    uint32_t srcStrideOffset = srcRepStride * ONE_BLK_HALF_NUMArg;               \
    uint32_t newDstRepStride = dstRepStride;                                     \
    __ubuf__ DataType* newSrc = src;                                             \
    if (dstRepStride == 0 && repeatTime > 0) {                                   \
        newRepeat = 1;                                                           \
        srcStrideOffset = srcRepStride * ONE_BLK_HALF_NUMArg * (repeatTime - 1); \
        newSrc += srcStrideOffset;                                               \
        newDstRepStride = 1;                                                     \
    }

#define ASC_M310_STAGE_REDUCE_ADJUST_LAST_REPEAT(                                                                 \
    dstRepStride, repeatTime, newRepeat, srcStrideOffset, srcRepStride, newSrc, newDstRepStride, dstStrideOffset) \
    if (dstRepStride == 0 && repeatTime > 0) {                                                                    \
        newRepeat = 1;                                                                                            \
        srcStrideOffset = srcRepStride * ONE_BLK_HALF_NUM * (repeatTime - 1);                                     \
        newSrc += srcStrideOffset;                                                                                \
        newDstRepStride = 1;                                                                                      \
        dstStrideOffset = 1;                                                                                      \
    }

#define ASC_M310_STAGE_REDUCE_INIT_WORK_REGISTERS(VectorType, mask, MaskIntrinsic) \
    VectorType vreg0;                                                              \
    VectorType vreg1;                                                              \
    vector_align ureg;                                                             \
    uint32_t sreg = static_cast<uint32_t>(mask);                                   \
    vector_bool preg = MaskIntrinsic(sreg, POST_UPDATE);                           \
    uint32_t strideConfig0 = ((static_cast<uint32_t>(1)) << 16);                   \
    uint32_t strideConfig1 = ((static_cast<uint32_t>(0)) << 16)

namespace AscendC {
#define VCPADD_FUNC() vcpadd(vreg1, vreg0, preg, MODE_ZEROING)
#define VCGADD_FUNC() vcgadd(vreg1, vreg0, preg, MODE_ZEROING)
#define VCGMAX_FUNC() vcgmax(vreg1, vreg0, preg, MODE_ZEROING)
#define VCGMIN_FUNC() vcgmin(vreg1, vreg0, preg, MODE_ZEROING)
#define VCMAX_FUNC() vcmax(vreg1, vreg0, preg, MODE_ZEROING)
#define VCMIN_FUNC() vcmin(vreg1, vreg0, preg, MODE_ZEROING)
#define VCADD_FUNC() vcadd(vreg1, vreg0, preg, MODE_ZEROING)

#define CONTINUOUS_MODE_REDUCE_VF(reduceFunc, vregType, pltType, dstStrideOffset) \
    __VEC_SCOPE__                                                                 \
    {                                                                             \
        vector_##vregType vreg0;                                                  \
        vector_##vregType vreg1;                                                  \
        vector_align ureg;                                                        \
        uint32_t sreg = static_cast<uint32_t>(mask);                              \
        vector_bool preg = plt_##pltType(sreg, POST_UPDATE);                      \
        uint32_t strideConfig = ((static_cast<uint32_t>(srcBlkStride)) << 16);    \
        for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeat); ++i) {         \
            vsldb(vreg0, newSrc + i * srcStrideOffset, strideConfig, preg);       \
            reduceFunc();                                                         \
            vstus(ureg, dstStrideOffset, vreg1, dst, POST_UPDATE);                \
            vstas(ureg, dst, dstStrideOffset*(newDstRepStride - 1), POST_UPDATE); \
        }                                                                         \
    }

#define BITBYBIT_MODE_HALF_REDUCE_VF(reduceFunc, dstStrideOffset)                 \
    __VEC_SCOPE__                                                                 \
    {                                                                             \
        vector_f16 vreg0;                                                         \
        vector_f16 vreg1;                                                         \
        vector_align ureg;                                                        \
        vector_bool preg;                                                         \
        plds(preg, ((__ubuf__ uint32_t*)tempBuf), 0, US);                         \
        uint32_t strideConfig = ((static_cast<uint32_t>(srcBlkStride)) << 16);    \
        for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeat); ++i) {         \
            vsldb(vreg0, newSrc + i * srcStrideOffset, strideConfig, preg);       \
            reduceFunc();                                                         \
            vstus(ureg, dstStrideOffset, vreg1, dst, POST_UPDATE);                \
            vstas(ureg, dst, dstStrideOffset*(newDstRepStride - 1), POST_UPDATE); \
        }                                                                         \
    }

#define BITBYBIT_MODE_FLOAT_REDUCE_VF(reduceFunc, dstStrideOffset)                                 \
    __VEC_SCOPE__                                                                                  \
    {                                                                                              \
        ASC_M310_STAGE_REDUCE_INIT_F32_REGISTERS(vreg0, vreg1, ureg, preg, tempBuf, srcBlkStride); \
        for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeat); ++i) {                          \
            vsldb(vreg0, newSrc + i * srcStrideOffset, strideConfig, preg);                        \
            reduceFunc();                                                                          \
            vstus(ureg, dstStrideOffset, vreg1, dst, POST_UPDATE);                                 \
            vstas(ureg, dst, dstStrideOffset*(newDstRepStride - 1), POST_UPDATE);                  \
        }                                                                                          \
    }

/* **************************************** Pair Reduce Impl ****************************************** */
template <typename T, bool isSetMask = true>
__aicore__ inline void PairReduceSumImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const int32_t repeatTime, const int32_t mask, const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

template <typename T, bool isSetMask = true>
__aicore__ inline void PairReduceSumImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const int32_t repeatTime, const uint64_t mask[], const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

// Emit the original overloads with their existing vector scope and intrinsic order.
#define ASC_M310_DEFINE_PAIR_REDUCE_BITMASK(                                                             \
    DataType, ONE_BLK_HALF_NUMArg, BITBYBIT_MODE_HALF_REDUCE_VFArg, FULL_MASK_LENArg)                    \
    template <typename T = DataType, bool isSetMask = true>                                              \
    __aicore__ inline void PairReduceSumImpl(                                                            \
        __ubuf__ DataType* dst, __ubuf__ DataType* src, const int32_t repeatTime, const uint64_t mask[], \
        const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)              \
    {                                                                                                    \
        ASC_M310_STAGE_REDUCE_STAGE_BITMASK(mask);                                                       \
        ASC_M310_STAGE_REDUCE_INIT_BITMASK_REPEAT(                                                       \
            repeatTime, srcRepStride, ONE_BLK_HALF_NUMArg, dstRepStride, DataType, src)                  \
        BITBYBIT_MODE_HALF_REDUCE_VFArg(VCPADD_FUNC, FULL_MASK_LENArg / HALF_FACTOR);                    \
        AscendCUtils::FreeTemporaryBuffer<uint8_t>(tempBuf);                                             \
    }

ASC_M310_DEFINE_PAIR_REDUCE_BITMASK(half, ONE_BLK_HALF_NUM, BITBYBIT_MODE_HALF_REDUCE_VF, FULL_MASK_LEN)
ASC_M310_DEFINE_PAIR_REDUCE_BITMASK(float, ONE_BLK_FLOAT_NUM, BITBYBIT_MODE_FLOAT_REDUCE_VF, HALF_MASK_LEN)

#undef ASC_M310_DEFINE_PAIR_REDUCE_BITMASK

#define ASC_M310_DEFINE_PAIR_REDUCE_CONTINUOUS_MASK(DataType, ONE_BLK_HALF_NUMArg, f16Arg, b16Arg, FULL_MASK_LENArg) \
    template <typename T = DataType, bool isSetMask = true>                                                          \
    __aicore__ inline void PairReduceSumImpl(                                                                        \
        __ubuf__ DataType* dst, __ubuf__ DataType* src, const int32_t repeatTime, const int32_t mask,                \
        const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)                          \
    {                                                                                                                \
        ASC_M310_STAGE_REDUCE_INIT_BITMASK_REPEAT(                                                                   \
            repeatTime, srcRepStride, ONE_BLK_HALF_NUMArg, dstRepStride, DataType, src)                              \
        CONTINUOUS_MODE_REDUCE_VF(VCPADD_FUNC, f16Arg, b16Arg, FULL_MASK_LENArg / HALF_FACTOR);                      \
    }

ASC_M310_DEFINE_PAIR_REDUCE_CONTINUOUS_MASK(half, ONE_BLK_HALF_NUM, f16, b16, FULL_MASK_LEN)
ASC_M310_DEFINE_PAIR_REDUCE_CONTINUOUS_MASK(float, ONE_BLK_FLOAT_NUM, f32, b32, HALF_MASK_LEN)

#undef ASC_M310_DEFINE_PAIR_REDUCE_CONTINUOUS_MASK

/* **************************************** Block Reduce Impl ****************************************** */
template <typename T, bool isSetMask = true>
__aicore__ inline void BlockReduceSumImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const int32_t repeatTime, const uint64_t mask[], const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}
template <typename T, bool isSetMask = true>
__aicore__ inline void BlockReduceSumImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const int32_t repeatTime, const int32_t mask, const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

#define ASC_M310_DEFINE_BLOCK_REDUCE_BITMASK(                                                            \
    DataType, Function, ONE_BLK_HALF_NUMArg, BITBYBIT_MODE_HALF_REDUCE_VFArg, VCGADD_FUNCArg)            \
    template <typename T = DataType, bool isSetMask = true>                                              \
    __aicore__ inline void Function(                                                                     \
        __ubuf__ DataType* dst, __ubuf__ DataType* src, const int32_t repeatTime, const uint64_t mask[], \
        const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)              \
    {                                                                                                    \
        ASC_M310_STAGE_REDUCE_STAGE_BITMASK(mask);                                                       \
        ASC_M310_STAGE_REDUCE_INIT_BITMASK_REPEAT(                                                       \
            repeatTime, srcRepStride, ONE_BLK_HALF_NUMArg, dstRepStride, DataType, src)                  \
        BITBYBIT_MODE_HALF_REDUCE_VFArg(VCGADD_FUNCArg, DEFAULT_BLK_NUM);                                \
        AscendCUtils::FreeTemporaryBuffer<uint8_t>(tempBuf);                                             \
    }

ASC_M310_DEFINE_BLOCK_REDUCE_BITMASK(
    half, BlockReduceSumImpl, ONE_BLK_HALF_NUM, BITBYBIT_MODE_HALF_REDUCE_VF, VCGADD_FUNC)
ASC_M310_DEFINE_BLOCK_REDUCE_BITMASK(
    float, BlockReduceSumImpl, ONE_BLK_FLOAT_NUM, BITBYBIT_MODE_FLOAT_REDUCE_VF, VCGADD_FUNC)

#define ASC_M310_DEFINE_BLOCK_REDUCE_CONTINUOUS_MASK(                                                 \
    DataType, Function, ONE_BLK_HALF_NUMArg, VCGADD_FUNCArg, f16Arg, b16Arg)                          \
    template <typename T = DataType, bool isSetMask = true>                                           \
    __aicore__ inline void Function(                                                                  \
        __ubuf__ DataType* dst, __ubuf__ DataType* src, const int32_t repeatTime, const int32_t mask, \
        const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)           \
    {                                                                                                 \
        ASC_M310_STAGE_REDUCE_INIT_BITMASK_REPEAT(                                                    \
            repeatTime, srcRepStride, ONE_BLK_HALF_NUMArg, dstRepStride, DataType, src)               \
        CONTINUOUS_MODE_REDUCE_VF(VCGADD_FUNCArg, f16Arg, b16Arg, DEFAULT_BLK_NUM);                   \
    }

ASC_M310_DEFINE_BLOCK_REDUCE_CONTINUOUS_MASK(half, BlockReduceSumImpl, ONE_BLK_HALF_NUM, VCGADD_FUNC, f16, b16)
ASC_M310_DEFINE_BLOCK_REDUCE_CONTINUOUS_MASK(float, BlockReduceSumImpl, ONE_BLK_FLOAT_NUM, VCGADD_FUNC, f32, b32)

template <typename T, bool isSetMask = true>
__aicore__ inline void BlockReduceMaxImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const int32_t repeatTime, const uint64_t mask[], const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

template <typename T, bool isSetMask = true>
__aicore__ inline void BlockReduceMaxImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const int32_t repeatTime, const int32_t mask, const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_BLOCK_REDUCE_BITMASK(
    half, BlockReduceMaxImpl, ONE_BLK_HALF_NUM, BITBYBIT_MODE_HALF_REDUCE_VF, VCGMAX_FUNC)
ASC_M310_DEFINE_BLOCK_REDUCE_CONTINUOUS_MASK(half, BlockReduceMaxImpl, ONE_BLK_HALF_NUM, VCGMAX_FUNC, f16, b16)
ASC_M310_DEFINE_BLOCK_REDUCE_BITMASK(
    float, BlockReduceMaxImpl, ONE_BLK_FLOAT_NUM, BITBYBIT_MODE_FLOAT_REDUCE_VF, VCGMAX_FUNC)
ASC_M310_DEFINE_BLOCK_REDUCE_CONTINUOUS_MASK(float, BlockReduceMaxImpl, ONE_BLK_FLOAT_NUM, VCGMAX_FUNC, f32, b32)

template <typename T, bool isSetMask = true>
__aicore__ inline void BlockReduceMinImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const int32_t repeatTime, const uint64_t mask[], const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

template <typename T, bool isSetMask = true>
__aicore__ inline void BlockReduceMinImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const int32_t repeatTime, const int32_t mask, const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_BLOCK_REDUCE_BITMASK(
    half, BlockReduceMinImpl, ONE_BLK_HALF_NUM, BITBYBIT_MODE_HALF_REDUCE_VF, VCGMIN_FUNC)
ASC_M310_DEFINE_BLOCK_REDUCE_CONTINUOUS_MASK(half, BlockReduceMinImpl, ONE_BLK_HALF_NUM, VCGMIN_FUNC, f16, b16)
ASC_M310_DEFINE_BLOCK_REDUCE_BITMASK(
    float, BlockReduceMinImpl, ONE_BLK_FLOAT_NUM, BITBYBIT_MODE_FLOAT_REDUCE_VF, VCGMIN_FUNC)

#undef ASC_M310_DEFINE_BLOCK_REDUCE_BITMASK

ASC_M310_DEFINE_BLOCK_REDUCE_CONTINUOUS_MASK(float, BlockReduceMinImpl, ONE_BLK_FLOAT_NUM, VCGMIN_FUNC, f32, b32)

#undef ASC_M310_DEFINE_BLOCK_REDUCE_CONTINUOUS_MASK

template <typename T, bool isSetMask = true>
__aicore__ inline void RepeatReduceSumImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, const int32_t repeatTime, const int32_t elemsInOneRepeat,
    const int32_t dstBlkStride, const int32_t srcBlkStride, const int32_t dstRepStride, const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

/* **************************************** Whole Reduce Interface ****************************************** */
template <typename T, bool isSetMask = true>
__aicore__ inline void WholeReduceMaxImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, const uint64_t mask[], const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, const ReduceOrder order)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

template <typename T, bool isSetMask = true>
__aicore__ inline void WholeReduceMaxImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, const int32_t mask, const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, const ReduceOrder order)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

#define ASC_M310_DEFINE_WHOLE_MINMAX_CONTINUOUS_MASK(                                                                \
    DataType, Function, ONE_BLK_HALF_NUMArg, VectorType, MaskIntrinsic, VectorIntrinsic)                             \
    template <typename T = DataType, bool isSetMask = true>                                                          \
    __aicore__ inline void Function(                                                                                 \
        __ubuf__ DataType* dst, __ubuf__ DataType* src, const int32_t mask, const int32_t repeatTime,                \
        const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, const ReduceOrder order) \
    {                                                                                                                \
        int32_t newRepeat = repeatTime;                                                                              \
        uint32_t srcStrideOffset = srcRepStride * ONE_BLK_HALF_NUMArg;                                               \
        uint32_t newDstRepStride = dstRepStride;                                                                     \
        uint32_t dstStrideOffset = (dstRepStride > 1) ? 2 : 1;                                                       \
        __ubuf__ DataType* newSrc = src;                                                                             \
        if (dstRepStride == 0 && repeatTime > 0) {                                                                   \
            newRepeat = 1;                                                                                           \
            srcStrideOffset = srcRepStride * ONE_BLK_HALF_NUMArg * (repeatTime - 1);                                 \
            newSrc += srcStrideOffset;                                                                               \
            newDstRepStride = 1;                                                                                     \
            dstStrideOffset = 1;                                                                                     \
        }                                                                                                            \
        __VEC_SCOPE__                                                                                                \
        {                                                                                                            \
            VectorType vreg0;                                                                                        \
            VectorType vreg1;                                                                                        \
            vector_align ureg;                                                                                       \
            uint32_t sreg = static_cast<uint32_t>(mask);                                                             \
            vector_bool preg = MaskIntrinsic(sreg, POST_UPDATE);                                                     \
            uint32_t strideConfig = ((static_cast<uint32_t>(srcBlkStride)) << 16);                                   \
            for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeat); ++i) {                                        \
                vsldb(vreg0, newSrc + i * srcStrideOffset, strideConfig, preg);                                      \
                VectorIntrinsic(vreg1, vreg0, preg, MODE_ZEROING);                                                   \
                vstus(ureg, dstStrideOffset, vreg1, dst, POST_UPDATE);                                               \
                vstas(ureg, dst, newDstRepStride - dstStrideOffset, POST_UPDATE);                                    \
            }                                                                                                        \
        }                                                                                                            \
    }

ASC_M310_DEFINE_WHOLE_MINMAX_CONTINUOUS_MASK(half, WholeReduceMaxImpl, ONE_BLK_HALF_NUM, vector_f16, plt_b16, vcmax)
ASC_M310_DEFINE_WHOLE_MINMAX_CONTINUOUS_MASK(float, WholeReduceMaxImpl, ONE_BLK_FLOAT_NUM, vector_f32, plt_b32, vcmax)

template <typename T = half, bool isSetMask = true>
__aicore__ inline void WholeReduceMaxImpl(
    __ubuf__ half* dst, __ubuf__ half* src, const uint64_t mask[], const int32_t repeatTime, const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride, const ReduceOrder order)
{
    ASC_M310_STAGE_REDUCE_STAGE_BITMASK(mask);

    int32_t newRepeat = repeatTime;
    uint32_t newDstRepStride = dstRepStride;
    uint32_t dstStrideOffset = (dstRepStride > 1) ? 2 : 1;
    __ubuf__ half* newSrc = src;
    uint32_t srcStrideOffset = srcRepStride * ONE_BLK_HALF_NUM;
    ASC_M310_STAGE_REDUCE_ADJUST_LAST_REPEAT(
        dstRepStride, repeatTime, newRepeat, srcStrideOffset, srcRepStride, newSrc, newDstRepStride, dstStrideOffset)
    __VEC_SCOPE__
    {
        ASC_M310_STAGE_REDUCE_INIT_F16_REGISTERS(tempBuf, srcBlkStride);
        for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeat); ++i) {
            vsldb(vreg0, newSrc + i * srcStrideOffset, strideConfig, preg);
            vcmax(vreg1, vreg0, preg, MODE_ZEROING);
            vstus(ureg, dstStrideOffset, vreg1, dst, POST_UPDATE);
            vstas(ureg, dst, newDstRepStride - dstStrideOffset, POST_UPDATE);
        }
    }
    AscendCUtils::FreeTemporaryBuffer<uint8_t>(tempBuf);
}

#define ASC_M310_DEFINE_WHOLE_MINMAX_BITMASK_B32(Function, VectorIntrinsic)                                          \
    template <typename T = float, bool isSetMask = true>                                                             \
    __aicore__ inline void Function(                                                                                 \
        __ubuf__ float* dst, __ubuf__ float* src, const uint64_t mask[], const int32_t repeatTime,                   \
        const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, const ReduceOrder order) \
    {                                                                                                                \
        ASC_M310_STAGE_REDUCE_STAGE_BITMASK(mask);                                                                   \
        int32_t newRepeat = repeatTime;                                                                              \
        uint32_t srcStrideOffset = srcRepStride * ONE_BLK_FLOAT_NUM;                                                 \
        uint32_t newDstRepStride = dstRepStride;                                                                     \
        uint32_t dstStrideOffset = (dstRepStride > 1) ? 2 : 1;                                                       \
        __ubuf__ float* newSrc = src;                                                                                \
        if (dstRepStride == 0 && repeatTime > 0) {                                                                   \
            newRepeat = 1;                                                                                           \
            srcStrideOffset = srcRepStride * ONE_BLK_FLOAT_NUM * (repeatTime - 1);                                   \
            newSrc += srcStrideOffset;                                                                               \
            newDstRepStride = 1;                                                                                     \
            dstStrideOffset = 1;                                                                                     \
        }                                                                                                            \
        __VEC_SCOPE__                                                                                                \
        {                                                                                                            \
            ASC_M310_STAGE_REDUCE_INIT_F32_REGISTERS(vreg0, vreg1, ureg, preg, tempBuf, srcBlkStride);               \
            for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeat); ++i) {                                        \
                vsldb(vreg0, newSrc + i * srcStrideOffset, strideConfig, preg);                                      \
                VectorIntrinsic(vreg1, vreg0, preg, MODE_ZEROING);                                                   \
                vstus(ureg, dstStrideOffset, vreg1, dst, POST_UPDATE);                                               \
                vstas(ureg, dst, newDstRepStride - dstStrideOffset, POST_UPDATE);                                    \
            }                                                                                                        \
        }                                                                                                            \
        AscendCUtils::FreeTemporaryBuffer<uint8_t>(tempBuf);                                                         \
    }

ASC_M310_DEFINE_WHOLE_MINMAX_BITMASK_B32(WholeReduceMaxImpl, vcmax)

template <typename T, bool isSetMask = true>
__aicore__ inline void WholeReduceMinImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, const uint64_t mask[], const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, const ReduceOrder order)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

template <typename T, bool isSetMask = true>
__aicore__ inline void WholeReduceMinImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, const int32_t mask, const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, const ReduceOrder order)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_WHOLE_MINMAX_CONTINUOUS_MASK(half, WholeReduceMinImpl, ONE_BLK_HALF_NUM, vector_f16, plt_b16, vcmin)
ASC_M310_DEFINE_WHOLE_MINMAX_CONTINUOUS_MASK(float, WholeReduceMinImpl, ONE_BLK_FLOAT_NUM, vector_f32, plt_b32, vcmin)

#undef ASC_M310_DEFINE_WHOLE_MINMAX_CONTINUOUS_MASK

template <typename T = half, bool isSetMask = true>
__aicore__ inline void WholeReduceMinImpl(
    __ubuf__ half* dst, __ubuf__ half* src, const uint64_t mask[], const int32_t repeatTime, const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride, const ReduceOrder order)
{
    ASC_M310_STAGE_REDUCE_STAGE_BITMASK(mask);

    int32_t newRepeat = repeatTime;
    uint32_t srcStrideOffset = srcRepStride * ONE_BLK_HALF_NUM;
    uint32_t newDstRepStride = dstRepStride;
    uint32_t dstStrideOffset = (dstRepStride > 1) ? 2 : 1;
    __ubuf__ half* newSrc = src;
    ASC_M310_STAGE_REDUCE_ADJUST_LAST_REPEAT(
        dstRepStride, repeatTime, newRepeat, srcStrideOffset, srcRepStride, newSrc, newDstRepStride, dstStrideOffset)

    __VEC_SCOPE__
    {
        ASC_M310_STAGE_REDUCE_INIT_F16_REGISTERS(tempBuf, srcBlkStride);
        for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeat); ++i) {
            vsldb(vreg0, newSrc + i * srcStrideOffset, strideConfig, preg);
            vcmin(vreg1, vreg0, preg, MODE_ZEROING);
            vstus(ureg, dstStrideOffset, vreg1, dst, POST_UPDATE);
            vstas(ureg, dst, newDstRepStride - dstStrideOffset, POST_UPDATE);
        }
    }
    AscendCUtils::FreeTemporaryBuffer<uint8_t>(tempBuf);
}

ASC_M310_DEFINE_WHOLE_MINMAX_BITMASK_B32(WholeReduceMinImpl, vcmin)

#undef ASC_M310_DEFINE_WHOLE_MINMAX_BITMASK_B32

template <typename T, bool isSetMask = true>
__aicore__ inline void WholeReduceSumImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, const uint64_t mask[], const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

template <typename T, bool isSetMask = true>
__aicore__ inline void WholeReduceSumImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, const int32_t mask, const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

#define ASC_M310_DEFINE_WHOLE_SUM_CONTINUOUS_MASK(DataType, ONE_BLK_HALF_NUMArg, f16Arg, b16Arg)      \
    template <typename T = DataType, bool isSetMask = true>                                           \
    __aicore__ inline void WholeReduceSumImpl(                                                        \
        __ubuf__ DataType* dst, __ubuf__ DataType* src, const int32_t mask, const int32_t repeatTime, \
        const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)           \
    {                                                                                                 \
        ASC_M310_STAGE_REDUCE_INIT_BITMASK_REPEAT(                                                    \
            repeatTime, srcRepStride, ONE_BLK_HALF_NUMArg, dstRepStride, DataType, src)               \
        CONTINUOUS_MODE_REDUCE_VF(VCADD_FUNC, f16Arg, b16Arg, 1);                                     \
    }

ASC_M310_DEFINE_WHOLE_SUM_CONTINUOUS_MASK(half, ONE_BLK_HALF_NUM, f16, b16)
ASC_M310_DEFINE_WHOLE_SUM_CONTINUOUS_MASK(float, ONE_BLK_FLOAT_NUM, f32, b32)

#undef ASC_M310_DEFINE_WHOLE_SUM_CONTINUOUS_MASK

#define ASC_M310_DEFINE_WHOLE_SUM_BITMASK(DataType, ONE_BLK_HALF_NUMArg, BITBYBIT_MODE_HALF_REDUCE_VFArg) \
    template <typename T = DataType, bool isSetMask = true>                                               \
    __aicore__ inline void WholeReduceSumImpl(                                                            \
        __ubuf__ DataType* dst, __ubuf__ DataType* src, const uint64_t mask[], const int32_t repeatTime,  \
        const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)               \
    {                                                                                                     \
        ASC_M310_STAGE_REDUCE_STAGE_BITMASK(mask);                                                        \
        ASC_M310_STAGE_REDUCE_INIT_BITMASK_REPEAT(                                                        \
            repeatTime, srcRepStride, ONE_BLK_HALF_NUMArg, dstRepStride, DataType, src)                   \
        BITBYBIT_MODE_HALF_REDUCE_VFArg(VCADD_FUNC, 1);                                                   \
        AscendCUtils::FreeTemporaryBuffer<uint8_t>(tempBuf);                                              \
    }

ASC_M310_DEFINE_WHOLE_SUM_BITMASK(half, ONE_BLK_HALF_NUM, BITBYBIT_MODE_HALF_REDUCE_VF)
ASC_M310_DEFINE_WHOLE_SUM_BITMASK(float, ONE_BLK_FLOAT_NUM, BITBYBIT_MODE_FLOAT_REDUCE_VF)

#undef ASC_M310_DEFINE_WHOLE_SUM_BITMASK

/* **************************************** Reduce Interface ****************************************** */
template <typename T>
__aicore__ inline void ReduceMaxIntrinsicsImpl(
    __ubuf__ T* sharedTmpBuffer, __ubuf__ T* srcLocal, const int32_t mask, const int32_t repeatTime,
    const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "ReduceMaxIntrinsicsImpl is not supported!"); });
}

template <typename T>
__aicore__ inline void ReduceMaxIntrinsicsImpl(
    __ubuf__ T* sharedTmpBuffer, __ubuf__ T* srcLocal, const uint64_t mask[], const int32_t repeatTime,
    const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "ReduceMaxIntrinsicsImpl is not supported!"); });
}

#define ASC_M310_DEFINE_REDUCE_MINMAX_WORK_CONTINUOUS_MASK(                                                            \
    Function, DataType, VectorType, MaskIntrinsic, ElementCount, VectorIntrinsic)                                      \
    __aicore__ inline void Function(                                                                                   \
        __ubuf__ DataType* sharedTmpBuffer, __ubuf__ DataType* srcLocal, const int32_t mask, const int32_t repeatTime, \
        const int32_t srcRepStride)                                                                                    \
    {                                                                                                                  \
        __VEC_SCOPE__                                                                                                  \
        {                                                                                                              \
            ASC_M310_STAGE_REDUCE_INIT_WORK_REGISTERS(VectorType, mask, MaskIntrinsic);                                \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                                         \
                vsldb(vreg0, srcLocal + i * srcRepStride * ElementCount, strideConfig0, preg);                         \
                VectorIntrinsic(vreg1, vreg0, preg, MODE_ZEROING);                                                     \
                vstus(ureg, 2, vreg1, sharedTmpBuffer, POST_UPDATE);                                                   \
            }                                                                                                          \
            vstas(ureg, sharedTmpBuffer, 0, POST_UPDATE);                                                              \
        }                                                                                                              \
    }

ASC_M310_DEFINE_REDUCE_MINMAX_WORK_CONTINUOUS_MASK(ReduceMaxIntrinsicsImpl, half, vector_f16, plt_b16, 16, vcmax)

#define ASC_M310_DEFINE_REDUCE_MINMAX_WORK_BITMASK_B16(Function, VectorIntrinsic)                                 \
    __aicore__ inline void Function(                                                                              \
        __ubuf__ half* sharedTmpBuffer, __ubuf__ half* srcLocal, const uint64_t mask[], const int32_t repeatTime, \
        const int32_t srcRepStride)                                                                               \
    {                                                                                                             \
        __ubuf__ uint8_t* tempBuf = AscendCUtils::GetTemporaryBufferAddr<uint8_t>(TMP_UB_OFFSET, 16);             \
        *((__ubuf__ uint64_t*)tempBuf) = mask[0];                                                                 \
        *((__ubuf__ uint64_t*)tempBuf + 1) = mask[1];                                                             \
        event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));                  \
        SetFlag<HardEvent::S_V>(eventIdSToV);                                                                     \
        WaitFlag<HardEvent::S_V>(eventIdSToV);                                                                    \
        __VEC_SCOPE__                                                                                             \
        {                                                                                                         \
            vector_f16 vreg0;                                                                                     \
            vector_f16 vreg1;                                                                                     \
            vector_align ureg;                                                                                    \
            vector_bool preg;                                                                                     \
            plds(preg, ((__ubuf__ uint32_t*)tempBuf), 0, US);                                                     \
            uint32_t strideConfig0 = ((static_cast<uint32_t>(1)) << 16);                                          \
            uint32_t strideConfig1 = ((static_cast<uint32_t>(0)) << 16);                                          \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                                    \
                vsldb(vreg0, srcLocal + i * srcRepStride * 16, strideConfig0, preg);                              \
                VectorIntrinsic(vreg1, vreg0, preg, MODE_ZEROING);                                                \
                vstus(ureg, 2, vreg1, sharedTmpBuffer, POST_UPDATE);                                              \
            }                                                                                                     \
            vstas(ureg, sharedTmpBuffer, 0, POST_UPDATE);                                                         \
        }                                                                                                         \
        AscendCUtils::FreeTemporaryBuffer<uint8_t>(tempBuf);                                                      \
    }

ASC_M310_DEFINE_REDUCE_MINMAX_WORK_BITMASK_B16(ReduceMaxIntrinsicsImpl, vcmax)
ASC_M310_DEFINE_REDUCE_MINMAX_WORK_CONTINUOUS_MASK(ReduceMaxIntrinsicsImpl, float, vector_f32, plt_b32, 8, vcmax)

#define ASC_M310_DEFINE_REDUCE_MINMAX_WORK_BITMASK_B32(Function, VectorIntrinsic)                                   \
    __aicore__ inline void Function(                                                                                \
        __ubuf__ float* sharedTmpBuffer, __ubuf__ float* srcLocal, const uint64_t mask[], const int32_t repeatTime, \
        const int32_t srcRepStride)                                                                                 \
    {                                                                                                               \
        __ubuf__ uint8_t* tempBuf = AscendCUtils::GetTemporaryBufferAddr<uint8_t>(TMP_UB_OFFSET, 16);               \
        *((__ubuf__ uint64_t*)tempBuf) = mask[0];                                                                   \
        *((__ubuf__ uint64_t*)tempBuf + 1) = mask[1];                                                               \
        event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));                    \
        SetFlag<HardEvent::S_V>(eventIdSToV);                                                                       \
        WaitFlag<HardEvent::S_V>(eventIdSToV);                                                                      \
        __VEC_SCOPE__                                                                                               \
        {                                                                                                           \
            vector_f32 vreg0;                                                                                       \
            vector_f32 vreg1;                                                                                       \
            vector_align ureg;                                                                                      \
            vector_bool preg;                                                                                       \
            vector_bool preg1;                                                                                      \
            plds(preg, ((__ubuf__ uint32_t*)tempBuf), 0, US);                                                       \
            punpack(preg1, preg, LOWER);                                                                            \
            uint32_t strideConfig0 = ((static_cast<uint32_t>(1)) << 16);                                            \
            uint32_t strideConfig1 = ((static_cast<uint32_t>(0)) << 16);                                            \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                                      \
                vsldb(vreg0, srcLocal + i * srcRepStride * 8, strideConfig0, preg1);                                \
                VectorIntrinsic(vreg1, vreg0, preg1, MODE_ZEROING);                                                 \
                vstus(ureg, 2, vreg1, sharedTmpBuffer, POST_UPDATE);                                                \
            }                                                                                                       \
            vstas(ureg, sharedTmpBuffer, 0, POST_UPDATE);                                                           \
        }                                                                                                           \
        AscendCUtils::FreeTemporaryBuffer<uint8_t>(tempBuf);                                                        \
    }

ASC_M310_DEFINE_REDUCE_MINMAX_WORK_BITMASK_B32(ReduceMaxIntrinsicsImpl, vcmax)

template <typename T>
__aicore__ inline void ReduceMinIntrinsicsImpl(
    __ubuf__ T* sharedTmpBuffer, __ubuf__ T* srcLocal, const int32_t mask, const int32_t repeatTime,
    const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "ReduceMinIntrinsicsImpl is not supported!"); });
}

template <typename T>
__aicore__ inline void ReduceMinIntrinsicsImpl(
    __ubuf__ T* sharedTmpBuffer, __ubuf__ T* srcLocal, const uint64_t mask[], const int32_t repeatTime,
    const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "ReduceMinIntrinsicsImpl is not supported!"); });
}

ASC_M310_DEFINE_REDUCE_MINMAX_WORK_CONTINUOUS_MASK(ReduceMinIntrinsicsImpl, half, vector_f16, plt_b16, 16, vcmin)
ASC_M310_DEFINE_REDUCE_MINMAX_WORK_BITMASK_B16(ReduceMinIntrinsicsImpl, vcmin)

#undef ASC_M310_DEFINE_REDUCE_MINMAX_WORK_BITMASK_B16

ASC_M310_DEFINE_REDUCE_MINMAX_WORK_CONTINUOUS_MASK(ReduceMinIntrinsicsImpl, float, vector_f32, plt_b32, 8, vcmin)

#undef ASC_M310_DEFINE_REDUCE_MINMAX_WORK_CONTINUOUS_MASK

ASC_M310_DEFINE_REDUCE_MINMAX_WORK_BITMASK_B32(ReduceMinIntrinsicsImpl, vcmin)

#undef ASC_M310_DEFINE_REDUCE_MINMAX_WORK_BITMASK_B32

template <typename T>
__aicore__ inline void ReduceSumIntrinsicsImpl(
    __ubuf__ T* sharedTmpBuffer, __ubuf__ T* srcLocal, const int32_t mask, const int32_t repeatTime,
    const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "ReduceSumIntrinsicsImpl is not supported!"); });
}

template <typename T>
__aicore__ inline void ReduceSumIntrinsicsImpl(
    __ubuf__ T* sharedTmpBuffer, __ubuf__ T* srcLocal, const uint64_t mask[], const int32_t repeatTime,
    const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "ReduceSumIntrinsicsImpl is not supported!"); });
}

#define ASC_M310_DEFINE_REDUCE_SUM_WORK_CONTINUOUS_MASK(DataType, VectorType, MaskIntrinsic, ElementCount)             \
    __aicore__ inline void ReduceSumIntrinsicsImpl(                                                                    \
        __ubuf__ DataType* sharedTmpBuffer, __ubuf__ DataType* srcLocal, const int32_t mask, const int32_t repeatTime, \
        const int32_t srcRepStride)                                                                                    \
    {                                                                                                                  \
        __VEC_SCOPE__                                                                                                  \
        {                                                                                                              \
            ASC_M310_STAGE_REDUCE_INIT_WORK_REGISTERS(VectorType, mask, MaskIntrinsic);                                \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                                         \
                vsldb(vreg0, srcLocal + i * srcRepStride * ElementCount, strideConfig0, preg);                         \
                vcadd(vreg1, vreg0, preg, MODE_ZEROING);                                                               \
                vstus(ureg, 1, vreg1, sharedTmpBuffer, POST_UPDATE);                                                   \
            }                                                                                                          \
            vstas(ureg, sharedTmpBuffer, 0, POST_UPDATE);                                                              \
        }                                                                                                              \
    }

ASC_M310_DEFINE_REDUCE_SUM_WORK_CONTINUOUS_MASK(half, vector_f16, plt_b16, 16)

__aicore__ inline void ReduceSumIntrinsicsImpl(
    __ubuf__ half* sharedTmpBuffer, __ubuf__ half* srcLocal, const uint64_t mask[], const int32_t repeatTime,
    const int32_t srcRepStride)
{
    __ubuf__ uint8_t* tempBuf = AscendCUtils::GetTemporaryBufferAddr<uint8_t>(TMP_UB_OFFSET, 16);
    *((__ubuf__ uint64_t*)tempBuf) = mask[0];
    *((__ubuf__ uint64_t*)tempBuf + 1) = mask[1];

    event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
    SetFlag<HardEvent::S_V>(eventIdSToV);
    WaitFlag<HardEvent::S_V>(eventIdSToV);
    __VEC_SCOPE__
    {
        vector_f16 vreg0;
        vector_f16 vreg1;
        vector_align ureg;
        vector_bool preg;
        plds(preg, ((__ubuf__ uint32_t*)tempBuf), 0, US);
        uint32_t strideConfig0 = ((static_cast<uint32_t>(1)) << 16);
        uint32_t strideConfig1 = ((static_cast<uint32_t>(0)) << 16);
        for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {
            vsldb(vreg0, srcLocal + i * srcRepStride * 16, strideConfig0, preg);
            vcadd(vreg1, vreg0, preg, MODE_ZEROING);
            vstus(ureg, 1, vreg1, sharedTmpBuffer, POST_UPDATE);
        }
        vstas(ureg, sharedTmpBuffer, 0, POST_UPDATE);
    }
    AscendCUtils::FreeTemporaryBuffer<uint8_t>(tempBuf);
}

ASC_M310_DEFINE_REDUCE_SUM_WORK_CONTINUOUS_MASK(float, vector_f32, plt_b32, 8)

#undef ASC_M310_DEFINE_REDUCE_SUM_WORK_CONTINUOUS_MASK

__aicore__ inline void ReduceSumIntrinsicsImpl(
    __ubuf__ float* sharedTmpBuffer, __ubuf__ float* srcLocal, const uint64_t mask[], const int32_t repeatTime,
    const int32_t srcRepStride)
{
    __ubuf__ uint8_t* tempBuf = AscendCUtils::GetTemporaryBufferAddr<uint8_t>(TMP_UB_OFFSET, 16);
    *((__ubuf__ uint64_t*)tempBuf) = mask[0];
    *((__ubuf__ uint64_t*)tempBuf + 1) = mask[1];

    event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
    SetFlag<HardEvent::S_V>(eventIdSToV);
    WaitFlag<HardEvent::S_V>(eventIdSToV);
    __VEC_SCOPE__
    {
        vector_f32 vreg0;
        vector_f32 vreg1;
        vector_align ureg;
        vector_bool preg;
        vector_bool preg1;
        plds(preg, ((__ubuf__ uint32_t*)tempBuf), 0, US);
        punpack(preg1, preg, LOWER);
        uint32_t strideConfig0 = ((static_cast<uint32_t>(1)) << 16);
        uint32_t strideConfig1 = ((static_cast<uint32_t>(0)) << 16);
        for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {
            vsldb(vreg0, srcLocal + i * srcRepStride * 8, strideConfig0, preg1);
            vcadd(vreg1, vreg0, preg1, MODE_ZEROING);
            vstus(ureg, 1, vreg1, sharedTmpBuffer, POST_UPDATE);
        }
        vstas(ureg, sharedTmpBuffer, 0, POST_UPDATE);
    }
    AscendCUtils::FreeTemporaryBuffer<uint8_t>(tempBuf);
}

template <typename T>
__aicore__ inline void ReduceSumSecondStep(
    __ubuf__ T* dstLocal, __ubuf__ T* sharedTmpBuffer, struct ReduceRepeatParams& params)
{
    int32_t dstOffset = 0;
    int32_t srcOffset = 0;
    int32_t elementNumPerRep = ONE_REPEAT_BYTE_SIZE / sizeof(T); // fp16=128 , fp32=64
    int32_t newRepeatTimes = params.repeatTimes / elementNumPerRep;
    int32_t leftData = params.repeatTimes % elementNumPerRep;

    if (newRepeatTimes != 0) {
        ReduceSumIntrinsicsImpl(
            sharedTmpBuffer, sharedTmpBuffer, elementNumPerRep, newRepeatTimes, DEFAULT_REPEAT_STRIDE);
    }

    if (leftData > 0) { // has_tail
        srcOffset = elementNumPerRep * newRepeatTimes;
        ReduceSumIntrinsicsImpl(dstLocal, sharedTmpBuffer + srcOffset, leftData, 1, DEFAULT_REPEAT_STRIDE);
        event_t eventIdVToS = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_S));
        SetFlag<HardEvent::V_S>(eventIdVToS);
        WaitFlag<HardEvent::V_S>(eventIdVToS);
        *(sharedTmpBuffer + newRepeatTimes) = *dstLocal;
        if (newRepeatTimes != 0) {
            event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
            SetFlag<HardEvent::S_V>(eventIdSToV);
            WaitFlag<HardEvent::S_V>(eventIdSToV);
        }
    }
}

template <typename T>
__aicore__ inline void CreateSpecialFormatMask(const int32_t& maskLen, uint64_t& highMask, uint64_t& lowMask)
{
    // create mask in the "0101010101" format
    int32_t halfLen = HALF_MASK_LEN / 2;
    for (int32_t i = 0; i < maskLen - halfLen; i++) {
        highMask = highMask << 2;
        highMask = highMask | 1;
    }
    int32_t lowMaskRange = maskLen >= halfLen ? halfLen : maskLen;
    for (int32_t i = 0; i < lowMaskRange; i++) {
        lowMask = lowMask << 2;
        lowMask = lowMask | 1;
    }
}

template <typename T>
__aicore__ inline void ReduceOperation(
    __ubuf__ T* sharedTmpBuffer, __ubuf__ T* srcLocal, struct ReduceRepeatParams& params, const ReduceMode& mode)
{
    if (params.maskMode == 1) {
        switch (mode) {
            case ReduceMode::REDUCE_MAX:
                ReduceMaxIntrinsicsImpl(
                    sharedTmpBuffer, srcLocal, params.normalMask, params.repeatTimes, params.srcRepStride);
                break;
            case ReduceMode::REDUCE_MIN:
                ReduceMinIntrinsicsImpl(
                    sharedTmpBuffer, srcLocal, params.normalMask, params.repeatTimes, params.srcRepStride);
                break;
            case ReduceMode::REDUCE_SUM:
                ReduceSumIntrinsicsImpl(
                    sharedTmpBuffer, srcLocal, params.normalMask, params.repeatTimes, params.srcRepStride);
                break;
            default:
                break;
        }
    } else {
        switch (mode) {
            case ReduceMode::REDUCE_MAX:
                ReduceMaxIntrinsicsImpl(
                    sharedTmpBuffer, srcLocal, params.bitMask, params.repeatTimes, params.srcRepStride);
                break;
            case ReduceMode::REDUCE_MIN:
                ReduceMinIntrinsicsImpl(
                    sharedTmpBuffer, srcLocal, params.bitMask, params.repeatTimes, params.srcRepStride);
                break;
            case ReduceMode::REDUCE_SUM:
                ReduceSumIntrinsicsImpl(
                    sharedTmpBuffer, srcLocal, params.bitMask, params.repeatTimes, params.srcRepStride);
                break;
            default:
                break;
        }
    }
}

template <typename T>
__aicore__ inline void ReduceImplFirstStep(
    __ubuf__ T* sharedTmpBuffer, __ubuf__ T* srcLocal, struct ReduceRepeatParams& params, const ReduceMode& mode,
    int32_t& curData)
{
    int32_t dstOffset = 0;
    int32_t srcOffset = 0;
    int32_t range = params.repeatTimes / MAX_REPEAT_TIMES;

    for (int32_t index = 0; index < range; index++) {
        dstOffset = index * MAX_REPEAT_TIMES * VREDUCE_PER_REP_OUTPUT;
        srcOffset = index * MAX_REPEAT_TIMES * params.srcRepStride * ONE_BLK_SIZE / sizeof(T);
        struct ReduceRepeatParams newParams = params;
        newParams.repeatTimes = MAX_REPEAT_TIMES;
        ReduceOperation<T>(sharedTmpBuffer + dstOffset, srcLocal + srcOffset, newParams, mode);
    }
    int32_t leftRepeatTimes = params.repeatTimes % MAX_REPEAT_TIMES;
    if (leftRepeatTimes > 0) {
        dstOffset = range * MAX_REPEAT_TIMES * VREDUCE_PER_REP_OUTPUT;
        srcOffset = range * MAX_REPEAT_TIMES * params.srcRepStride * ONE_BLK_SIZE / sizeof(T);
        struct ReduceRepeatParams leftParams = params;
        leftParams.repeatTimes = leftRepeatTimes;
        ReduceOperation<T>(sharedTmpBuffer + dstOffset, srcLocal + srcOffset, leftParams, mode);
    }
    curData = VREDUCE_PER_REP_OUTPUT * params.repeatTimes;
}

template <typename T>
__aicore__ inline void ReduceImplSecondStep(
    __ubuf__ T* sharedTmpBuffer, const ReduceMode& mode, int32_t& curData, int32_t preStartPos, int32_t secondStartPos)
{
    int32_t dstOffset = 0;
    int32_t srcOffset = 0;
    int32_t newMaskLen = 0;
    int32_t elementNumPerRep = ONE_REPEAT_BYTE_SIZE / sizeof(T);
    int32_t newRepeatTimes = curData / elementNumPerRep;
    int32_t leftData = curData % elementNumPerRep;
    uint64_t highMask = 0, lowMask = 0;
    uint64_t newMask[2];
    int32_t bodyOutputCount = 0;
    int32_t tailOutputCount = 0;

    if (newRepeatTimes >= 1) {
        highMask = (sizeof(T) == sizeof(half)) ? 0x5555555555555555 : 0;
        lowMask = 0x5555555555555555;
        newMask[0] = lowMask;
        newMask[1] = highMask;
        struct ReduceRepeatParams newParams(
            newMask, newRepeatTimes, DEFAULT_REDUCE_DST_REP_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);

        ReduceOperation<T>(sharedTmpBuffer + secondStartPos, sharedTmpBuffer + preStartPos, newParams, mode);
        bodyOutputCount = newRepeatTimes * VREDUCE_PER_REP_OUTPUT;
    }
    highMask = 0;
    lowMask = 0;

    if (leftData > 0) {
        newMaskLen = leftData / VREDUCE_PER_REP_OUTPUT;
        // create mask in the "0101010101" format
        CreateSpecialFormatMask<T>(newMaskLen, highMask, lowMask);
        newMask[0] = lowMask;
        newMask[1] = highMask;
        struct ReduceRepeatParams leftParams(
            newMask, 1, DEFAULT_REDUCE_DST_REP_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);

        dstOffset = secondStartPos + bodyOutputCount;
        srcOffset = preStartPos + newRepeatTimes * elementNumPerRep;
        ReduceOperation<T>(sharedTmpBuffer + dstOffset, sharedTmpBuffer + srcOffset, leftParams, mode);
        tailOutputCount = VREDUCE_PER_REP_OUTPUT;
    }

    curData = bodyOutputCount + tailOutputCount;
}

template <typename T>
__aicore__ inline void GetIndex(
    __ubuf__ T* sharedTmpBuffer, int32_t secondStartPos, int32_t& secondIndex, int32_t& thirdIndex)
{
    int32_t elementNumPerRep = ONE_REPEAT_BYTE_SIZE / sizeof(T);
    if (sizeof(T) == sizeof(half)) {
        thirdIndex = *reinterpret_cast<__ubuf__ uint16_t*>(sharedTmpBuffer + secondStartPos + 1);
        ASSERT(thirdIndex >= 0);
        ASSERT(thirdIndex < elementNumPerRep);
        secondIndex = *reinterpret_cast<__ubuf__ uint16_t*>(sharedTmpBuffer + thirdIndex + 1);
        ASSERT(secondIndex >= 0);
        ASSERT(secondIndex < elementNumPerRep);
    } else {
        thirdIndex = *reinterpret_cast<__ubuf__ uint32_t*>(sharedTmpBuffer + secondStartPos + 1);
        ASSERT(thirdIndex >= 0);
        ASSERT(thirdIndex < elementNumPerRep);
        secondIndex = *reinterpret_cast<__ubuf__ uint32_t*>(sharedTmpBuffer + thirdIndex + 1);
        ASSERT(secondIndex >= 0);
        ASSERT(secondIndex < elementNumPerRep);
    }
}

template <typename T>
__aicore__ inline void GetIndex(
    __ubuf__ T* sharedTmpBuffer, int32_t secondStartPos, int32_t thirdStartPos, int32_t& firstIndex,
    int32_t& secondIndex, int32_t& thirdIndex)
{
    int32_t elementNumPerRep = ONE_REPEAT_BYTE_SIZE / sizeof(T);
    if (sizeof(T) == sizeof(half)) {
        thirdIndex = *reinterpret_cast<__ubuf__ uint16_t*>(sharedTmpBuffer + thirdStartPos + 1);
        ASSERT(thirdIndex >= 0);
        ASSERT(thirdIndex < elementNumPerRep);
        secondIndex = *reinterpret_cast<__ubuf__ uint16_t*>(sharedTmpBuffer + secondStartPos + thirdIndex + 1);
        ASSERT(secondIndex >= 0);
        ASSERT(secondIndex < elementNumPerRep);
        firstIndex = *reinterpret_cast<__ubuf__ uint16_t*>(
            sharedTmpBuffer + elementNumPerRep * (thirdIndex / VREDUCE_PER_REP_OUTPUT) + secondIndex + 1);
        ASSERT(firstIndex >= 0);
        ASSERT(firstIndex < elementNumPerRep);
    } else {
        thirdIndex = *reinterpret_cast<__ubuf__ uint32_t*>(sharedTmpBuffer + thirdStartPos + 1);
        ASSERT(thirdIndex >= 0);
        ASSERT(thirdIndex < elementNumPerRep);
        secondIndex = *reinterpret_cast<__ubuf__ uint32_t*>(sharedTmpBuffer + secondStartPos + thirdIndex + 1);
        ASSERT(secondIndex >= 0);
        ASSERT(secondIndex < elementNumPerRep);
        firstIndex = *reinterpret_cast<__ubuf__ uint32_t*>(
            sharedTmpBuffer + elementNumPerRep * (thirdIndex / VREDUCE_PER_REP_OUTPUT) + secondIndex + 1);
        ASSERT(firstIndex >= 0);
        ASSERT(firstIndex < elementNumPerRep);
    }
}

template <typename T>
__aicore__ inline void GetIndex(
    __ubuf__ T* sharedTmpBuffer, int32_t secondStartPos, int32_t thirdStartPos, int32_t fourthStartPos,
    int32_t& firstIndex, int32_t& secondIndex, int32_t& thirdIndex, int32_t& fourthIndex)
{
    int32_t elementNumPerRep = ONE_REPEAT_BYTE_SIZE / sizeof(T);
    if (sizeof(T) == sizeof(half)) {
        fourthIndex = *reinterpret_cast<__ubuf__ uint16_t*>(sharedTmpBuffer + fourthStartPos + 1);
        ASSERT(fourthIndex >= 0);
        ASSERT(fourthIndex < elementNumPerRep);
        thirdIndex = *reinterpret_cast<__ubuf__ uint16_t*>(sharedTmpBuffer + thirdStartPos + fourthIndex + 1);
        ASSERT(thirdIndex >= 0);
        ASSERT(thirdIndex < elementNumPerRep);
        secondIndex = *reinterpret_cast<__ubuf__ uint16_t*>(
            sharedTmpBuffer + secondStartPos + elementNumPerRep * (fourthIndex / VREDUCE_PER_REP_OUTPUT) + thirdIndex +
            1);
        ASSERT(secondIndex >= 0);
        ASSERT(secondIndex < elementNumPerRep);
        firstIndex = *reinterpret_cast<__ubuf__ uint16_t*>(
            sharedTmpBuffer +
            elementNumPerRep * (elementNumPerRep * (fourthIndex / VREDUCE_PER_REP_OUTPUT) + thirdIndex) /
                VREDUCE_PER_REP_OUTPUT +
            secondIndex + 1);
        ASSERT(firstIndex >= 0);
        ASSERT(firstIndex < elementNumPerRep);
    } else {
        fourthIndex = *reinterpret_cast<__ubuf__ uint32_t*>(sharedTmpBuffer + fourthStartPos + 1);
        ASSERT(fourthIndex >= 0);
        ASSERT(fourthIndex < elementNumPerRep);
        thirdIndex = *reinterpret_cast<__ubuf__ uint32_t*>(sharedTmpBuffer + thirdStartPos + fourthIndex + 1);
        ASSERT(thirdIndex >= 0);
        ASSERT(thirdIndex < elementNumPerRep);
        secondIndex = *reinterpret_cast<__ubuf__ uint32_t*>(
            sharedTmpBuffer + secondStartPos + elementNumPerRep * (fourthIndex / VREDUCE_PER_REP_OUTPUT) + thirdIndex +
            1);
        ASSERT(secondIndex >= 0);
        ASSERT(secondIndex < elementNumPerRep);
        firstIndex = *reinterpret_cast<__ubuf__ uint32_t*>(
            sharedTmpBuffer +
            elementNumPerRep * (elementNumPerRep * (fourthIndex / VREDUCE_PER_REP_OUTPUT) + thirdIndex) /
                VREDUCE_PER_REP_OUTPUT +
            secondIndex + 1);
        ASSERT(firstIndex >= 0);
        ASSERT(firstIndex < elementNumPerRep);
    }
}

template <typename T>
__aicore__ inline void ReduceImplThirdStep(
    __ubuf__ T* dstLocal, __ubuf__ T* sharedTmpBuffer, const int32_t srcRepStride, const ReduceMode& mode,
    int32_t& curData, int32_t& secondStartPos, int32_t& thirdStartPos)
{
    int32_t preNum = 0;
    int32_t firstIndex = 0;
    int32_t secondIndex = 0;
    int32_t thirdIndex = 0;
    int32_t fourthIndex = 0;
    int32_t dstOffset = 0;
    int32_t srcOffset = 0;
    uint64_t highMask = 0;
    uint64_t lowMask = 0;
    uint64_t newMask[2];
    int32_t offsetNumPerRep = ONE_BLK_SIZE / sizeof(T) * srcRepStride;
    int32_t elementNumPerRep = ONE_REPEAT_BYTE_SIZE / sizeof(T);
    event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
    event_t eventIdVToS = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_S));
    event_t eventIdSToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_MTE3));
    if (curData == VREDUCE_PER_REP_OUTPUT) {
        SetFlag<HardEvent::V_S>(eventIdVToS);
        WaitFlag<HardEvent::V_S>(eventIdVToS);
        GetIndex<T>(sharedTmpBuffer, secondStartPos, secondIndex, thirdIndex);
        preNum = offsetNumPerRep * (thirdIndex / VREDUCE_PER_REP_OUTPUT);
        int32_t resultIndex = secondIndex + preNum;
        *dstLocal = *(sharedTmpBuffer + secondStartPos);
        *(dstLocal + 1) = *reinterpret_cast<T*>(&resultIndex);
        SetFlag<HardEvent::S_V>(eventIdSToV);
        WaitFlag<HardEvent::S_V>(eventIdSToV);
        SetFlag<HardEvent::S_MTE3>(eventIdSToMTE3);
        WaitFlag<HardEvent::S_MTE3>(eventIdSToMTE3);
        return;
    }

    int32_t newMaskLen = curData / VREDUCE_PER_REP_OUTPUT;
    CreateSpecialFormatMask<T>(newMaskLen, highMask, lowMask);
    newMask[0] = lowMask;
    newMask[1] = highMask;
    if (curData > elementNumPerRep) {
        ReduceImplSecondStep<T>(sharedTmpBuffer, mode, curData, secondStartPos, thirdStartPos);

        int32_t fourthStartPos =
            (((thirdStartPos + curData) * sizeof(T) + ONE_BLK_SIZE - 1) / ONE_BLK_SIZE) * ONE_BLK_SIZE / sizeof(T);
        dstOffset = fourthStartPos;
        srcOffset = thirdStartPos;
        struct ReduceRepeatParams newParams(
            newMask, 1, DEFAULT_REDUCE_DST_REP_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);

        ReduceOperation<T>(sharedTmpBuffer + dstOffset, sharedTmpBuffer + srcOffset, newParams, mode);
        SetFlag<HardEvent::V_S>(eventIdVToS);
        WaitFlag<HardEvent::V_S>(eventIdVToS);
        *dstLocal = *(sharedTmpBuffer + dstOffset);

        GetIndex<T>(
            sharedTmpBuffer, secondStartPos, thirdStartPos, fourthStartPos, firstIndex, secondIndex, thirdIndex,
            fourthIndex);
        preNum = offsetNumPerRep *
                 (elementNumPerRep * (elementNumPerRep * (fourthIndex / VREDUCE_PER_REP_OUTPUT) + thirdIndex) /
                      VREDUCE_PER_REP_OUTPUT +
                  secondIndex) /
                 VREDUCE_PER_REP_OUTPUT;
    } else {
        dstOffset = thirdStartPos;
        srcOffset = secondStartPos;
        struct ReduceRepeatParams newParams(
            newMask, 1, DEFAULT_REDUCE_DST_REP_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
        ReduceOperation<T>(sharedTmpBuffer + dstOffset, sharedTmpBuffer + srcOffset, newParams, mode);
        SetFlag<HardEvent::V_S>(eventIdVToS);
        WaitFlag<HardEvent::V_S>(eventIdVToS);
        *dstLocal = *(sharedTmpBuffer + thirdStartPos);

        GetIndex<T>(sharedTmpBuffer, secondStartPos, thirdStartPos, firstIndex, secondIndex, thirdIndex);
        preNum = offsetNumPerRep * (elementNumPerRep * (thirdIndex / VREDUCE_PER_REP_OUTPUT) + secondIndex) /
                 VREDUCE_PER_REP_OUTPUT;
    }

    int32_t resultIndex = firstIndex + preNum;
    *(dstLocal + 1) = *reinterpret_cast<T*>(&resultIndex);
    SetFlag<HardEvent::S_V>(eventIdSToV);
    WaitFlag<HardEvent::S_V>(eventIdSToV);
    SetFlag<HardEvent::S_MTE3>(eventIdSToMTE3);
    WaitFlag<HardEvent::S_MTE3>(eventIdSToMTE3);
}

template <typename T>
__aicore__ inline void ReduceSumFirstStep(
    __ubuf__ T* sharedTmpBuffer, __ubuf__ T* srcLocal, struct ReduceRepeatParams& params)
{
    int32_t dstOffset = 0;
    int32_t srcOffset = 0;
    int32_t maxRepeatTimes = (MAX_REPEAT_TIMES - ONE_BLK_SIZE / sizeof(T) + 1);
    int32_t range = params.repeatTimes / maxRepeatTimes;

    for (int32_t index = 0; index < range; index++) {
        dstOffset = index * maxRepeatTimes;
        srcOffset = index * maxRepeatTimes * (params.srcRepStride * ONE_BLK_SIZE / sizeof(T));
        struct ReduceRepeatParams newParams = params;
        newParams.repeatTimes = maxRepeatTimes;
        ReduceOperation<T>(sharedTmpBuffer + dstOffset, srcLocal + srcOffset, newParams, ReduceMode::REDUCE_SUM);
    }

    int32_t leftRepeatTimes = params.repeatTimes % maxRepeatTimes;
    if (leftRepeatTimes > 0) {
        dstOffset = range * maxRepeatTimes;
        srcOffset = range * maxRepeatTimes * (params.srcRepStride * ONE_BLK_SIZE / sizeof(T));
        struct ReduceRepeatParams leftParams = params;
        leftParams.repeatTimes = leftRepeatTimes;
        ReduceOperation<T>(sharedTmpBuffer + dstOffset, srcLocal + srcOffset, leftParams, ReduceMode::REDUCE_SUM);
    }
}

template <typename T>
__aicore__ inline void ReduceSumFinalStep(__ubuf__ T* dstLocal, __ubuf__ T* sharedTmpBuffer, int32_t& secondResultNum)
{
    if (secondResultNum == 1) {
        event_t eventIdVToS = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_S));
        SetFlag<HardEvent::V_S>(eventIdVToS);
        WaitFlag<HardEvent::V_S>(eventIdVToS);
        *(dstLocal) = *(sharedTmpBuffer);
        event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
        SetFlag<HardEvent::S_V>(eventIdSToV);
        WaitFlag<HardEvent::S_V>(eventIdSToV);
        event_t eventIdSToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_MTE3));
        SetFlag<HardEvent::S_MTE3>(eventIdSToMTE3);
        WaitFlag<HardEvent::S_MTE3>(eventIdSToMTE3);
    } else {
        struct ReduceRepeatParams newParams(
            secondResultNum, 1, DEFAULT_REDUCE_DST_REP_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
        ReduceOperation<T>(dstLocal, sharedTmpBuffer, newParams, ReduceMode::REDUCE_SUM);
    }
}

template <typename T>
__aicore__ inline void ReduceSumImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* sharedTmpBuffer, struct ReduceRepeatParams& params)
{
    ReduceSumFirstStep<T>(sharedTmpBuffer, srcLocal, params);
    ReduceSumSecondStep<T>(dstLocal, sharedTmpBuffer, params);
    int32_t secondResultNum = DivCeil(params.repeatTimes, ONE_REPEAT_BYTE_SIZE / sizeof(T));
    ReduceSumFinalStep<T>(dstLocal, sharedTmpBuffer, secondResultNum);
}

template <typename T>
__aicore__ inline void ReduceImplSecondStepNoIndex(
    __ubuf__ T* sharedTmpBuffer, const ReduceMode& mode, int32_t& curData)
{
    int32_t elementNumPerRep = ONE_REPEAT_BYTE_SIZE / sizeof(T); // fp16=128,fp32=64
    int32_t newRepeatTimes = curData / elementNumPerRep;
    int32_t leftData = curData % elementNumPerRep;
    uint64_t highMask = 0;
    uint64_t lowMask = 0;
    uint64_t newMask[2];
    if (newRepeatTimes != 0) {
        CreateSpecialFormatMask<T>(elementNumPerRep / VREDUCE_PER_REP_OUTPUT, highMask, lowMask);
        newMask[0] = lowMask;
        newMask[1] = highMask;
        struct ReduceRepeatParams newParams(
            newMask, newRepeatTimes, DEFAULT_REDUCE_DST_REP_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
        ReduceOperation<T>(sharedTmpBuffer, sharedTmpBuffer, newParams, mode);
    }
    highMask = 0;
    lowMask = 0;
    if (leftData > 0) {
        CreateSpecialFormatMask<T>(leftData / VREDUCE_PER_REP_OUTPUT, highMask, lowMask);
        newMask[0] = lowMask;
        newMask[1] = highMask;
        struct ReduceRepeatParams leftParams(
            newMask, 1, DEFAULT_REDUCE_DST_REP_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
        ReduceOperation<T>(
            sharedTmpBuffer + newRepeatTimes * VREDUCE_PER_REP_OUTPUT,
            sharedTmpBuffer + newRepeatTimes * elementNumPerRep, leftParams, mode);
        newRepeatTimes += 1;
    }
    curData = newRepeatTimes * VREDUCE_PER_REP_OUTPUT;
}

template <typename T>
__aicore__ inline void ReduceImplThirdStepNoIndex(
    __ubuf__ T* dstLocal, __ubuf__ T* sharedTmpBuffer, const ReduceMode& mode, int32_t& curData)
{
    uint64_t highMask = 0;
    uint64_t lowMask = 0;
    uint64_t newMask[2];
    CreateSpecialFormatMask<T>(curData / VREDUCE_PER_REP_OUTPUT, highMask, lowMask);
    newMask[0] = lowMask;
    newMask[1] = highMask;
    struct ReduceRepeatParams newParams(
        newMask, 1, DEFAULT_REDUCE_DST_REP_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    ReduceOperation<T>(sharedTmpBuffer, sharedTmpBuffer, newParams, mode);
    event_t eventIdVToS = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_S));
    SetFlag<HardEvent::V_S>(eventIdVToS);
    WaitFlag<HardEvent::V_S>(eventIdVToS);
    *dstLocal = *sharedTmpBuffer;
    event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
    SetFlag<HardEvent::S_V>(eventIdSToV);
    WaitFlag<HardEvent::S_V>(eventIdSToV);
    event_t eventIdSToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_MTE3));
    SetFlag<HardEvent::S_MTE3>(eventIdSToMTE3);
    WaitFlag<HardEvent::S_MTE3>(eventIdSToMTE3);
}

template <typename T>
__aicore__ inline void ReduceImplWithIndex(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* sharedTmpBuffer, struct ReduceRepeatParams& params,
    const ReduceMode& mode)
{
    if (params.repeatTimes == 1) {
        ReduceOperation<T>(dstLocal, srcLocal, params, mode);
    } else {
        int32_t curData = 0;
        event_t eventIdVToS = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_S));
        ReduceImplFirstStep<T>(sharedTmpBuffer, srcLocal, params, mode, curData);
        SetFlag<HardEvent::V_S>(eventIdVToS);
        WaitFlag<HardEvent::V_S>(eventIdVToS);

        int32_t secondStartPos = ((curData * sizeof(T) + ONE_BLK_SIZE - 1) / ONE_BLK_SIZE) * ONE_BLK_SIZE / sizeof(T);
        ReduceImplSecondStep<T>(sharedTmpBuffer, mode, curData, 0, secondStartPos);
        SetFlag<HardEvent::V_S>(eventIdVToS);
        WaitFlag<HardEvent::V_S>(eventIdVToS);

        int32_t thirdStartPos =
            (((secondStartPos + curData) * sizeof(T) + ONE_BLK_SIZE - 1) / ONE_BLK_SIZE) * ONE_BLK_SIZE / sizeof(T);
        ReduceImplThirdStep<T>(
            dstLocal, sharedTmpBuffer, params.srcRepStride, mode, curData, secondStartPos, thirdStartPos);
    }
}

template <typename T>
__aicore__ inline void ReduceImplNoIndex(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* sharedTmpBuffer, struct ReduceRepeatParams& params,
    const ReduceMode& mode)
{
    event_t eventIdVToS = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_S));
    event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
    if (params.repeatTimes == 1) {
        ReduceOperation<T>(sharedTmpBuffer, srcLocal, params, mode);
        SetFlag<HardEvent::V_S>(eventIdVToS);
        WaitFlag<HardEvent::V_S>(eventIdVToS);
        *dstLocal = *sharedTmpBuffer;
        SetFlag<HardEvent::S_V>(eventIdSToV);
        WaitFlag<HardEvent::S_V>(eventIdSToV);
        event_t eventIdSToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_MTE3));
        SetFlag<HardEvent::S_MTE3>(eventIdSToMTE3);
        WaitFlag<HardEvent::S_MTE3>(eventIdSToMTE3);
    } else {
        if (mode == ReduceMode::REDUCE_SUM) {
            ReduceSumImpl<T>(dstLocal, srcLocal, sharedTmpBuffer, params);
        } else {
            int32_t curData = 0;
            ReduceImplFirstStep<T>(sharedTmpBuffer, srcLocal, params, mode, curData);
            SetFlag<HardEvent::V_S>(eventIdVToS);
            WaitFlag<HardEvent::V_S>(eventIdVToS);

            ReduceImplSecondStepNoIndex<T>(sharedTmpBuffer, mode, curData);

            int32_t elementNumPerRep = ONE_REPEAT_BYTE_SIZE / sizeof(T); // fp16=128,fp32=64
            if (curData <= elementNumPerRep) {
                SetFlag<HardEvent::V_S>(eventIdVToS);
                WaitFlag<HardEvent::V_S>(eventIdVToS);
                ReduceImplThirdStepNoIndex<T>(dstLocal, sharedTmpBuffer, mode, curData);
                return;
            }
            SetFlag<HardEvent::V_S>(eventIdVToS);
            WaitFlag<HardEvent::V_S>(eventIdVToS);
            ReduceImplSecondStepNoIndex<T>(sharedTmpBuffer, mode, curData);
            if (curData <= elementNumPerRep) {
                SetFlag<HardEvent::V_S>(eventIdVToS);
                WaitFlag<HardEvent::V_S>(eventIdVToS);
                ReduceImplThirdStepNoIndex<T>(dstLocal, sharedTmpBuffer, mode, curData);
            }
        }
    }
}

template <typename T>
__aicore__ inline void ReduceImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* sharedTmpBuffer, struct ReduceRepeatParams& params,
    bool calIndex, const ReduceMode& mode)
{
    if (calIndex) {
        ReduceImplWithIndex<T>(dstLocal, srcLocal, sharedTmpBuffer, params, mode);
    } else {
        ReduceImplNoIndex<T>(dstLocal, srcLocal, sharedTmpBuffer, params, mode);
    }
}

template <typename T>
__aicore__ inline void ReduceTailCompute(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& work, const int32_t count,
    bool calIndex, const ReduceMode& mode)
{
    using PrimType = PrimT<T>;
    int32_t elementNumPerRep = ONE_REPEAT_BYTE_SIZE / sizeof(PrimType); // fp16=128 , fp32=64
    int32_t repeatTime = count / elementNumPerRep;
    int32_t tailCount = count % elementNumPerRep; // tailCount  <= 128/64 repeatTime=1
    event_t eventIdVToS = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_S));
    SetFlag<HardEvent::V_S>(eventIdVToS);
    WaitFlag<HardEvent::V_S>(eventIdVToS);
    PrimType bodyValue = dst.GetValue(0);
    PrimType bodyIndex = dst.GetValue(1);

    struct ReduceRepeatParams tailParams(
        tailCount, 1, DEFAULT_REDUCE_DST_REP_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);

    ReduceImpl<PrimType>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), // 复用dst
        (__ubuf__ PrimType*)src.GetPhyAddr(elementNumPerRep * repeatTime), (__ubuf__ PrimType*)work.GetPhyAddr(),
        tailParams, calIndex, mode);
    SetFlag<HardEvent::V_S>(eventIdVToS);
    WaitFlag<HardEvent::V_S>(eventIdVToS);
    PrimType tailValue = dst.GetValue(0);
    PrimType tailIndex = dst.GetValue(1);

    // bodyresult tailresult need vcmin/vcmax again
    struct ReduceRepeatParams lastParams(
        2, 1, DEFAULT_REDUCE_DST_REP_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    work.SetValue(0, bodyValue);
    work.SetValue(1, tailValue);
    event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
    SetFlag<HardEvent::S_V>(eventIdSToV);
    WaitFlag<HardEvent::S_V>(eventIdSToV);

    ReduceImpl<PrimType>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)work.GetPhyAddr(),
        (__ubuf__ PrimType*)work.GetPhyAddr(), lastParams, calIndex, mode);
    if (calIndex) {
        SetFlag<HardEvent::V_S>(eventIdVToS);
        WaitFlag<HardEvent::V_S>(eventIdVToS);
        PrimType lastIndexVal = dst.GetValue(1);
        uint32_t newIndex = 0;
        uint32_t lastIndex = 0;
        if constexpr (sizeof(PrimType) == sizeof(half)) {
            lastIndex = *reinterpret_cast<uint16_t*>(&lastIndexVal);
            newIndex = elementNumPerRep * repeatTime + *reinterpret_cast<uint16_t*>(&tailIndex);
        } else {
            lastIndex = *reinterpret_cast<uint32_t*>(&lastIndexVal);
            newIndex = elementNumPerRep * repeatTime + *reinterpret_cast<uint32_t*>(&tailIndex);
        }
        if (lastIndex == 1) {
            dst.SetValue(1, *reinterpret_cast<PrimType*>(&newIndex));
        } else {
            dst.SetValue(1, bodyIndex);
        }
        SetFlag<HardEvent::S_V>(eventIdSToV);
        WaitFlag<HardEvent::S_V>(eventIdSToV);
        event_t eventIdSToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_MTE3));
        SetFlag<HardEvent::S_MTE3>(eventIdSToMTE3);
        WaitFlag<HardEvent::S_MTE3>(eventIdSToMTE3);
    }
}

template <typename T>
__aicore__ inline void GetReduceMaxMinCountImpl(uint32_t& maxMinValue, uint32_t& maxMinIndex)
{
    ASCENDC_ASSERT((false), "GetReduceMaxMinCount is not supported on current device");
}

template <typename T>
__aicore__ inline void GetReduceMaxMinCountImpl(uint32_t& maxMinValue)
{
    ASCENDC_ASSERT((false), "GetReduceMaxMinCount is not supported on current device");
}

template <typename T>
__aicore__ inline void GetReduceMaxMinCountImpl(T& maxMinValue, T& maxMinIndex)
{
    ASCENDC_ASSERT((false), "GetReduceMaxMinCount is not supported on current device");
}

template <typename T>
__aicore__ inline void GetReduceMaxMinCountImpl(T& maxMinValue)
{
    ASCENDC_ASSERT((false), "GetReduceMaxMinCount is not supported on current device");
}

template <typename T>
__aicore__ inline T GetAccValImpl()
{
    ASCENDC_ASSERT((false), "GetAccVal is not supported on current device");
    return 0;
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_REDUCE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_REDUCE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_REDUCE_IMPL_H__
#endif

#undef ASC_M310_STAGE_REDUCE_INIT_WORK_REGISTERS
#undef ASC_M310_STAGE_REDUCE_ADJUST_LAST_REPEAT
#undef ASC_M310_STAGE_REDUCE_INIT_BITMASK_REPEAT
#undef ASC_M310_STAGE_REDUCE_STAGE_BITMASK
#undef ASC_M310_STAGE_REDUCE_INIT_F32_REGISTERS

#undef ASC_M310_STAGE_REDUCE_F16_STRIDE
