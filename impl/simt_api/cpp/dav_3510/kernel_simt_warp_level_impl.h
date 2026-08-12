/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef IMPL_SIMT_API_CPP_DAV_C310_KERNEL_SIMT_WARP_LEVEL_IMPL_H
#define IMPL_SIMT_API_CPP_DAV_C310_KERNEL_SIMT_WARP_LEVEL_IMPL_H

#if defined(ASCENDC_CPU_DEBUG)
#include <cmath>

#include "../../../basic_api/kernel_utils.h"
#include "simt_api/cpp/kernel_simt_utils.h"
#include "stub_def.h"
#endif
#include "impl/simt_api/cpp/dav_3510/kernel_simt_common_impl.h"
namespace AscendC {
namespace Simt {

#if defined(ASCENDC_CPU_DEBUG)
constexpr int32_t MAX_SHLF_OFFSET = 31;
constexpr int32_t WARP_SIZE = 32;
#endif

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t AllSyncImpl(int32_t predicate)
{
#if defined(ASCENDC_CPU_DEBUG)
    uint32_t warpId = GetWarpId();
    Warp& warp = ThreadBlock::GetBlockInstance().warps_[warpId];
    auto customFunc = [](int32_t a, int32_t b) -> int32_t { return (a != 0) && (b != 0); };
    return warp.WarpOp(predicate, customFunc);
#else
    return __all(predicate);
#endif
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t AnySyncImpl(int32_t predicate)
{
#if defined(ASCENDC_CPU_DEBUG)
    uint32_t warpId = GetWarpId();
    Warp& warp = ThreadBlock::GetBlockInstance().warps_[warpId];
    auto customFunc = [](int32_t a, int32_t b) -> int32_t { return (a != 0) || (b != 0); };
    return warp.WarpOp(predicate, customFunc);
#else
    return __any(predicate);
#endif
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int32_t UniImpl(int32_t predicate)
{
#if defined(ASCENDC_CPU_DEBUG)
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "Uni CPU: not supported yet"); });
    return 0;
#else
    return __uni(predicate);
#endif
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t BallotSyncImpl(int32_t predicate)
{
#if defined(ASCENDC_CPU_DEBUG)
    uint32_t warpId = GetWarpId();
    uint32_t laneId = GetLaneId();
    uint32_t one = 1;
    uint32_t bitNum = 0;
    if (predicate != 0) {
        bitNum = one << laneId;
    }
    Warp& warp = ThreadBlock::GetBlockInstance().warps_[warpId];
    auto customFunc = [](uint32_t a, uint32_t b) -> uint32_t { return a + b; };
    return warp.WarpOp(bitNum, customFunc);
#else
    return __ballot(predicate);
#endif
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t ActiveMaskImpl()
{
#if defined(ASCENDC_CPU_DEBUG)
    uint32_t warpId = GetWarpId();
    uint32_t laneId = GetLaneId();
    uint32_t one = 1;
    uint32_t bitNum = one << laneId;
    Warp& warp = ThreadBlock::GetBlockInstance().warps_[warpId];
    auto customFunc = [](uint32_t a, uint32_t b) -> uint32_t { return a + b; };
    return warp.WarpOp(bitNum, customFunc);
#else
    return __activemask();
#endif
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T ShflSyncImpl(T var, int32_t srcLane, int32_t width = warpSize)
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, uint32_t, int64_t, uint64_t, half, half2, float, bfloat16_t, bfloat16x2_t>,
        "Input type of var only supports int32_t, uint32_t, int64_t, uint64_t, half, half2, float, bfloat16_t, "
        "bfloat16x2_t.");
#if defined(ASCENDC_CPU_DEBUG)
    ASCENDC_ASSERT((width <= WARP_SIZE && width > 0 && WARP_SIZE % width == 0), {
        KERNEL_LOG(KERNEL_ERROR, "width must be a power of 2 and in (0, 32]");
    });
    ASCENDC_ASSERT((srcLane < WARP_SIZE && srcLane >= 0), { KERNEL_LOG(KERNEL_ERROR, "srcLane must be in [0, 32)"); });

    uint32_t warpId = GetWarpId();
    uint32_t laneId = GetLaneId();
    Warp& warp = ThreadBlock::GetBlockInstance().warps_[warpId];
    int32_t minLane = laneId / width * width;
    srcLane = minLane + srcLane % width;

    return warp.WarpShuffleOp(var, laneId, srcLane);
#else
    return __shfl(var, srcLane, width);
#endif
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T ShflUpSyncImpl(T var, uint32_t delta, int32_t width = warpSize)
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, uint32_t, int64_t, uint64_t, half, half2, float, bfloat16_t, bfloat16x2_t>,
        "Input type of var only supports int32_t, uint32_t, int64_t, uint64_t, half, half2, float, bfloat16_t, "
        "bfloat16x2_t.");
#if defined(ASCENDC_CPU_DEBUG)
    ASCENDC_ASSERT((width <= WARP_SIZE && width > 0 && WARP_SIZE % width == 0), {
        KERNEL_LOG(KERNEL_ERROR, "width must be a power of 2 and in (0, 32]");
    });
    ASCENDC_ASSERT((delta < WARP_SIZE && delta >= 0), { KERNEL_LOG(KERNEL_ERROR, "delta must be in [0, 32)"); });

    uint32_t warpId = GetWarpId();
    uint32_t laneId = GetLaneId();
    Warp& warp = ThreadBlock::GetBlockInstance().warps_[warpId];
    int32_t minLane = laneId / width * width;
    int32_t srcLane = laneId - delta;

    if (srcLane < minLane) {
        srcLane = laneId;
    }

    return warp.WarpShuffleOp(var, laneId, srcLane);
#else
    return __shfl_up(var, delta, width);
#endif
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T ShflDownSyncImpl(T var, uint32_t delta, int32_t width = warpSize)
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, uint32_t, int64_t, uint64_t, half, half2, float, bfloat16_t, bfloat16x2_t>,
        "Input type of var only supports int32_t, uint32_t, int64_t, uint64_t, half, half2, float, bfloat16_t, "
        "bfloat16x2_t.");
#if defined(ASCENDC_CPU_DEBUG)
    ASCENDC_ASSERT((width <= WARP_SIZE && width > 0 && WARP_SIZE % width == 0), {
        KERNEL_LOG(KERNEL_ERROR, "width must be a power of 2 and in (0, 32]");
    });
    ASCENDC_ASSERT((delta < WARP_SIZE && delta >= 0), { KERNEL_LOG(KERNEL_ERROR, "delta must be in [0, 32)"); });

    uint32_t warpId = GetWarpId();
    uint32_t laneId = GetLaneId();
    Warp& warp = ThreadBlock::GetBlockInstance().warps_[warpId];
    int32_t minLane = laneId / width * width;
    int32_t maxLane = minLane + MAX_SHLF_OFFSET % width;
    int32_t srcLane = laneId + delta;

    if (srcLane > maxLane) {
        srcLane = laneId;
    }

    return warp.WarpShuffleOp(var, laneId, srcLane);
#else
    return __shfl_down(var, delta, width);
#endif
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T ShflXorSyncImpl(T var, int32_t laneMask, int32_t width = warpSize)
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, uint32_t, int64_t, uint64_t, half, half2, float, bfloat16_t, bfloat16x2_t>,
        "Input type of var only supports int32_t, uint32_t, int64_t, uint64_t, half, half2, float, bfloat16_t, "
        "bfloat16x2_t.");
#if defined(ASCENDC_CPU_DEBUG)
    ASCENDC_ASSERT((width <= WARP_SIZE && width > 0 && WARP_SIZE % width == 0), {
        KERNEL_LOG(KERNEL_ERROR, "width must be a power of 2 and in (0, 32]");
    });
    ASCENDC_ASSERT(
        (laneMask < WARP_SIZE && laneMask >= 0), { KERNEL_LOG(KERNEL_ERROR, "laneMask must be in [0, 32)"); });

    uint32_t warpId = GetWarpId();
    uint32_t laneId = GetLaneId();
    Warp& warp = ThreadBlock::GetBlockInstance().warps_[warpId];
    int32_t minLane = laneId / width * width;
    int32_t maxLane = minLane + MAX_SHLF_OFFSET % width;
    int32_t srcLane = laneId ^ laneMask;

    if (srcLane < 0 || srcLane > maxLane) {
        srcLane = laneId;
    }

    return warp.WarpShuffleOp(var, laneId, srcLane);
#else
    return __shfl_xor(var, laneMask, width);
#endif
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T ReduceAddSyncImpl(T val)
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, uint32_t, half, float>,
        "Input type of val only supports int32_t, uint32_t, half, float.");
#if defined(ASCENDC_CPU_DEBUG)
    uint32_t warpId = GetWarpId();
    Warp& warp = ThreadBlock::GetBlockInstance().warps_[warpId];
    auto customFunc = [](T a, T b) -> T { return a + b; };
    return warp.WarpOp(val, customFunc);
#else
    return __reduce_add(val);
#endif
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T ReduceMaxSyncImpl(T val)
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, uint32_t, half, float>,
        "Input type of val only supports int32_t, uint32_t, half, float.");
#if defined(ASCENDC_CPU_DEBUG)
    uint32_t warpId = GetWarpId();
    Warp& warp = ThreadBlock::GetBlockInstance().warps_[warpId];
    auto customFunc = [](T a, T b) -> T { return a > b ? a : b; };
    return warp.WarpOp(val, customFunc);
#else
    return __reduce_max(val);
#endif
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T ReduceMinSyncImpl(T val)
{
    static_assert(
        SupportTypeSimtInternel<T, int32_t, uint32_t, half, float>,
        "Input type of val only supports int32_t, uint32_t, half, float.");
#if defined(ASCENDC_CPU_DEBUG)
    uint32_t warpId = GetWarpId();
    Warp& warp = ThreadBlock::GetBlockInstance().warps_[warpId];
    auto customFunc = [](T a, T b) -> T { return a < b ? a : b; };
    return warp.WarpOp(val, customFunc);
#else
    return __reduce_min(val);
#endif
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void ThreadBarrierImpl()
{
#if defined(ASCENDC_CPU_DEBUG)
    Sync();
#else
    __sync_workitems();
#endif
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void ThreadFenceImpl()
{
#if defined(ASCENDC_CPU_DEBUG)
#else
    __threadfence();
#endif
}
} // namespace Simt
} // namespace AscendC
#endif // IMPL_SIMT_API_CPP_DAV_C310_KERNEL_SIMT_WARP_LEVEL_IMPL_H
