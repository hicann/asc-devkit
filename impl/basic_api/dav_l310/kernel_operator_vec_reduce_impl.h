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
 * \file kernel_operator_vec_reduce_impl.h
 * \brief AscendC l310 support reduce api.
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_l310/kernel_operator_vec_reduce_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_REDUCE_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_REDUCE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_REDUCE_IMPL_H

#include "../../../include/basic_api/reg_compute/kernel_reg_compute_intf.h"
#include "kernel_operator_vec_template_impl.h"

namespace AscendC {
__BLOCK_LOCAL__ __inline__ float accValFloat;
__BLOCK_LOCAL__ __inline__ half accValHalf;

/* **************************************** Pair Reduce Impl ****************************************** */
template <typename T, bool isSetMask = true>
__aicore__ inline void PairReduceSumImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const int32_t repeat, const int32_t mask, const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "PairReduceSum is not supported on current device!"); });
}

template <typename T, bool isSetMask = true>
__aicore__ inline void PairReduceSumImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const int32_t repeat, const uint64_t mask[], const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "PairReduceSum is not supported on current device!"); });
}

/* **************************************** Block Reduce Impl ****************************************** */
template <typename T, bool isSetMask = true>
__aicore__ inline void BlockReduceSumImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const int32_t repeat, const uint64_t mask[], const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "BlockReduceSum is not supported on current device!"); });
}

template <typename T, bool isSetMask = true>
__aicore__ inline void BlockReduceSumImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const int32_t repeat, const int32_t mask, const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "BlockReduceSum is not supported on current device!"); });
}

template <typename T, bool isSetMask = true>
__aicore__ inline void BlockReduceMaxImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const int32_t repeat, const uint64_t mask[], const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "BlockReduceMax is not supported on current device!"); });
}

template <typename T, bool isSetMask = true>
__aicore__ inline void BlockReduceMaxImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const int32_t repeat, const int32_t mask, const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "BlockReduceMax is not supported on current device!"); });
}

template <typename T, bool isSetMask = true>
__aicore__ inline void BlockReduceMinImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const int32_t repeat, const uint64_t mask[], const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "BlockReduceMin is not supported on current device!"); });
}

template <typename T, bool isSetMask = true>
__aicore__ inline void BlockReduceMinImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const int32_t repeat, const int32_t mask, const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "BlockReduceMin is not supported on current device!"); });
}

template <typename T, bool isSetMask = true, typename U = T>
__aicore__ inline void RepeatReduceSumImpl(
    __ubuf__ U* dstLocal, __ubuf__ T* srcLocal, const int32_t repeat, const int32_t elemsInOneRepeat,
    const int32_t dstBlkStride, const int32_t srcBlkStride, const int32_t dstRepStride, const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "RepeatReduceSum is not supported on current device!"); });
}

template <typename T, bool isSetMask = true>
__aicore__ inline void WholeReduceMaxImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, const uint64_t mask[], const int32_t repeat, const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride, const ReduceOrder order)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "WholeReduceMax is not supported on current device!"); });
}

template <typename T, bool isSetMask = true>
__aicore__ inline void WholeReduceMaxImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, const int32_t mask, const int32_t repeat, const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride, const ReduceOrder order)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "WholeReduceMax is not supported on current device!"); });
}

template <typename T, bool isSetMask = true>
__aicore__ inline void WholeReduceMinImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, const uint64_t mask[], const int32_t repeat, const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride, const ReduceOrder order)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "WholeReduceMin is not supported on current device!"); });
}

template <typename T, bool isSetMask = true>
__aicore__ inline void WholeReduceMinImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, const int32_t mask, const int32_t repeat, const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride, const ReduceOrder order)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "WholeReduceMin is not supported on current device!"); });
}

// WholeReduceSum mask����ģʽ
template <typename T, bool isSetMask = true, typename U = T>
__aicore__ inline void WholeReduceSumImpl(
    __ubuf__ U* dstLocal, __ubuf__ T* srcLocal, const uint64_t mask[], const int32_t repeat, const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "WholeReduceSum is not supported on current device!"); });
}

template <typename T, bool isSetMask = true, typename U = T>
__aicore__ inline void WholeReduceSumImpl(
    __ubuf__ U* dstLocal, __ubuf__ T* srcLocal, const int32_t mask, const int32_t repeat, const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "WholeReduceSum is not supported on current device!"); });
}

template <typename T>
__aicore__ inline void ReduceSumImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* workLocal, const uint64_t mask[], const int32_t repeat,
    const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "ReduceSum is not supported on current device!"); });
}

template <typename T>
__aicore__ inline void ReduceSumImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* workLocal, const int32_t mask, const int32_t repeat,
    const int32_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "ReduceSum is not supported on current device!"); });
}

template <typename T>
__aicore__ inline void ReduceSumImpl(__ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* workLocal, uint32_t count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "ReduceSum is not supported on current device!"); });
}

/***************************** Reduce Max & Min ******************/
template <typename T>
__aicore__ inline void ReduceMaxImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* workLocal, uint32_t count, bool calIndex)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "ReduceMax is not supported on current device!"); });
}

template <typename T>
__aicore__ inline void ReduceMaxImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* workLocal, const uint64_t mask[], const int32_t repeat,
    const int32_t srcRepStride, bool calIndex)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "ReduceMax is not supported on current device!"); });
}

template <typename T>
__aicore__ inline void ReduceMaxImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* workLocal, const int32_t mask, const int32_t repeat,
    const int32_t srcRepStride, bool calIndex)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "ReduceMax is not supported on current device!"); });
}

template <typename T>
__aicore__ inline void ReduceMinImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* workLocal, uint32_t count, bool calIndex)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "ReduceMin is not supported on current device!"); });
}

template <typename T>
__aicore__ inline void ReduceMinImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* workLocal, const uint64_t mask[], const int32_t repeat,
    const int32_t srcRepStride, bool calIndex)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "ReduceMin is not supported on current device!"); });
}

template <typename T>
__aicore__ inline void ReduceMinImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* workLocal, const int32_t mask, const int32_t repeat,
    const int32_t srcRepStride, bool calIndex)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "ReduceMin is not supported on current device!"); });
}

template <typename T>
__aicore__ inline void GetReduceMaxMinCountImpl(T& maxMinValue, T& maxMinIndex)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "GetReduceMaxMinCount is not supported on current device!"); });
}

template <typename T>
__aicore__ inline void GetReduceMaxMinCountImpl(T& maxMinValue)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "GetReduceMaxMinCount is not supported on current device!"); });
}

template <typename T>
__aicore__ inline T GetAccValImpl()
{
    if constexpr (sizeof(T) == 2) {
        return accValHalf;
    } else {
        return accValFloat;
    }
}

} // end namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_REDUCE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_REDUCE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_REDUCE_IMPL_H__
#endif
