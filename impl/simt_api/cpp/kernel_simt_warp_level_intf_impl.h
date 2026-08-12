/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef IMPL_SIMT_API_CPP_KERNEL_SIMT_WARP_LEVEL_INTERFACE_IMPL_H
#define IMPL_SIMT_API_CPP_KERNEL_SIMT_WARP_LEVEL_INTERFACE_IMPL_H

#include "impl/simt_api/cpp/dav_3510/kernel_simt_warp_level_impl.h"

namespace AscendC {
namespace Simt {
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t WarpAllSync(int32_t predicate) { return AllSyncImpl(predicate); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t WarpAnySync(int32_t predicate) { return AnySyncImpl(predicate); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t Uni(int32_t predicate) { return UniImpl(predicate); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t WarpBallotSync(int32_t predicate) { return BallotSyncImpl(predicate); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t WarpActiveMask() { return ActiveMaskImpl(); }

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T WarpShflSync(T var, int32_t srcLane, int32_t width)
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, uint32_t, int64_t, uint64_t, half, half2, float, bfloat16_t, bfloat16x2_t>,
        "Input type of var only supports int32_t, uint32_t, int64_t, uint64_t, half, half2, float, bfloat16_t, "
        "bfloat16x2_t.");
    return ShflSyncImpl(var, srcLane, width);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T WarpShflUpSync(T var, uint32_t delta, int32_t width)
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, uint32_t, int64_t, uint64_t, half, half2, float, bfloat16_t, bfloat16x2_t>,
        "Input type of var only supports int32_t, uint32_t, int64_t, uint64_t, half, half2, float, bfloat16_t, "
        "bfloat16x2_t.");
    return ShflUpSyncImpl(var, delta, width);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T WarpShflDownSync(T var, uint32_t delta, int32_t width)
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, uint32_t, int64_t, uint64_t, half, half2, float, bfloat16_t, bfloat16x2_t>,
        "Input type of var only supports int32_t, uint32_t, int64_t, uint64_t, half, half2, float, bfloat16_t, "
        "bfloat16x2_t.");
    return ShflDownSyncImpl(var, delta, width);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T WarpShflXorSync(T var, int32_t laneMask, int32_t width)
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, uint32_t, int64_t, uint64_t, half, half2, float, bfloat16_t, bfloat16x2_t>,
        "Input type of var only supports int32_t, uint32_t, int64_t, uint64_t, half, half2, float, bfloat16_t, "
        "bfloat16x2_t.");
    return ShflXorSyncImpl(var, laneMask, width);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T WarpReduceAddSync(T val)
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, uint32_t, half, float>,
        "Input type of val only supports int32_t, uint32_t, half, float.");
    return ReduceAddSyncImpl(val);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T WarpReduceMaxSync(T val)
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, uint32_t, half, float>,
        "Input type of val only supports int32_t, uint32_t, half, float.");
    return ReduceMaxSyncImpl(val);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T WarpReduceMinSync(T val)
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, uint32_t, half, float>,
        "Input type of val only supports int32_t, uint32_t, half, float.");
    return ReduceMinSyncImpl(val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void ThreadBarrier() { ThreadBarrierImpl(); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void ThreadFence() { ThreadFenceImpl(); }
} // namespace Simt
} // namespace AscendC
#endif // IMPL_SIMT_API_CPP_KERNEL_SIMT_WARP_LEVEL_INTERFACE_IMPL_H
