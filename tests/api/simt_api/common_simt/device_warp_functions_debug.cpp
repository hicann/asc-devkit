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
 * \file device_warp_functions_debug.cpp
 * \brief
 */

#if defined(ASCENDC_CPU_DEBUG)
#include "stub_def.h"
#include "kernel_simt_cpu.h"
#include "kernel_process_lock.h"

constexpr int32_t MAX_SHLF_OFFSET = 31;
constexpr int32_t WARP_SIZE = 32;

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
int32_t __all(int32_t predicate)
{
    uint32_t warp_id = AscendC::Simt::GetWarpId();
    AscendC::Simt::Warp& warp = AscendC::Simt::ThreadBlock::GetBlockInstance().warps_[warp_id];
    auto custom_func = [](int32_t a, int32_t b) -> int32_t { return (a != 0) && (b != 0); };
    return warp.WarpOp(predicate, custom_func);
}

int32_t __any(int32_t predicate)
{
    uint32_t warp_id = AscendC::Simt::GetWarpId();
    AscendC::Simt::Warp& warp = AscendC::Simt::ThreadBlock::GetBlockInstance().warps_[warp_id];
    auto custom_func = [](int32_t a, int32_t b) -> int32_t { return (a != 0) || (b != 0); };
    return warp.WarpOp(predicate, custom_func);
}

uint32_t __ballot(int32_t predicate)
{
    uint32_t warp_id = AscendC::Simt::GetWarpId();
    uint32_t lane_id = AscendC::Simt::GetLaneId();
    uint32_t one = 1;
    uint32_t bit_num = 0;
    if (predicate != 0) {
        bit_num = one << lane_id;
    }
    AscendC::Simt::Warp& warp = AscendC::Simt::ThreadBlock::GetBlockInstance().warps_[warp_id];
    auto custom_func = [](uint32_t a, uint32_t b) -> uint32_t { return a + b; };
    return warp.WarpOp(bit_num, custom_func);
}

uint32_t __activemask()
{
    uint32_t warp_id = AscendC::Simt::GetWarpId();
    uint32_t lane_id = AscendC::Simt::GetLaneId();
    uint32_t one = 1;
    uint32_t bit_num = one << lane_id;
    AscendC::Simt::Warp& warp = AscendC::Simt::ThreadBlock::GetBlockInstance().warps_[warp_id];
    auto custom_func = [](uint32_t a, uint32_t b) -> uint32_t { return a + b; };
    return warp.WarpOp(bit_num, custom_func);
}
#endif

template <typename T>
T AscShflCPU(T var, int32_t src_lane, int32_t width)
{
    uint32_t warp_id = AscendC::Simt::GetWarpId();
    uint32_t lane_id = AscendC::Simt::GetLaneId();
    AscendC::Simt::Warp& warp = AscendC::Simt::ThreadBlock::GetBlockInstance().warps_[warp_id];
    int32_t min_lane = lane_id / width * width;
    int32_t actual_src_lane = min_lane + src_lane % width;

    return warp.WarpShuffleOp(var, lane_id, actual_src_lane);
}

template <typename T>
T AscShflUpCPU(T var, int32_t delta, int32_t width)
{
    uint32_t warp_id = AscendC::Simt::GetWarpId();
    uint32_t lane_id = AscendC::Simt::GetLaneId();
    AscendC::Simt::Warp& warp = AscendC::Simt::ThreadBlock::GetBlockInstance().warps_[warp_id];
    int32_t min_lane = lane_id / width * width;
    int32_t src_lane = lane_id - delta;

    if (src_lane < min_lane) {
        src_lane = lane_id;
    }

    return warp.WarpShuffleOp(var, lane_id, src_lane);
}

template <typename T>
T AscShflDownCPU(T var, int32_t delta, int32_t width)
{
    uint32_t warp_id = AscendC::Simt::GetWarpId();
    uint32_t lane_id = AscendC::Simt::GetLaneId();
    AscendC::Simt::Warp& warp = AscendC::Simt::ThreadBlock::GetBlockInstance().warps_[warp_id];
    int32_t min_lane = lane_id / width * width;
    int32_t max_lane = min_lane + width - 1;
    int32_t src_lane = lane_id + delta;

    if (src_lane > max_lane) {
        src_lane = lane_id;
    }

    return warp.WarpShuffleOp(var, lane_id, src_lane);
}

template <typename T>
T AscShflXorCPU(T var, int32_t lane_mask, int32_t width)
{
    uint32_t warp_id = AscendC::Simt::GetWarpId();
    uint32_t lane_id = AscendC::Simt::GetLaneId();
    AscendC::Simt::Warp& warp = AscendC::Simt::ThreadBlock::GetBlockInstance().warps_[warp_id];
    int32_t min_lane = lane_id / width * width;
    int32_t max_lane = min_lane + width - 1;
    int32_t src_lane = lane_id ^ lane_mask;

    if (src_lane < min_lane || src_lane > max_lane) {
        src_lane = lane_id;
    }

    return warp.WarpShuffleOp(var, lane_id, src_lane);
}

#define REGISTER_SIMT_ASC_SHFL(INTRI, TYPE) \
    TYPE INTRI(TYPE var, int32_t src_lane, int32_t width) { return AscShflCPU<TYPE>(var, src_lane, width); }

#define REGISTER_SIMT_ASC_SHFL_UP(INTRI, TYPE) \
    TYPE INTRI(TYPE var, int32_t delta, int32_t width) { return AscShflUpCPU<TYPE>(var, delta, width); }

#define REGISTER_SIMT_ASC_SHFL_DOWN(INTRI, TYPE) \
    TYPE INTRI(TYPE var, int32_t delta, int32_t width) { return AscShflDownCPU<TYPE>(var, delta, width); }

#define REGISTER_SIMT_ASC_SHFL_XOR(INTRI, TYPE) \
    TYPE INTRI(TYPE var, int32_t lane_mask, int32_t width) { return AscShflXorCPU<TYPE>(var, lane_mask, width); }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
REGISTER_SIMT_ASC_SHFL(__shfl, uint32_t);
REGISTER_SIMT_ASC_SHFL(__shfl, int32_t);
REGISTER_SIMT_ASC_SHFL(__shfl, uint64_t);
REGISTER_SIMT_ASC_SHFL(__shfl, int64_t);
REGISTER_SIMT_ASC_SHFL(__shfl, float);
REGISTER_SIMT_ASC_SHFL(__shfl, half);
REGISTER_SIMT_ASC_SHFL(__shfl, half2);
REGISTER_SIMT_ASC_SHFL(__shfl, bfloat16_t);
REGISTER_SIMT_ASC_SHFL(__shfl, bfloat16x2_t);

REGISTER_SIMT_ASC_SHFL_UP(__shfl_up, uint32_t);
REGISTER_SIMT_ASC_SHFL_UP(__shfl_up, int32_t);
REGISTER_SIMT_ASC_SHFL_UP(__shfl_up, uint64_t);
REGISTER_SIMT_ASC_SHFL_UP(__shfl_up, int64_t);
REGISTER_SIMT_ASC_SHFL_UP(__shfl_up, float);
REGISTER_SIMT_ASC_SHFL_UP(__shfl_up, half);
REGISTER_SIMT_ASC_SHFL_UP(__shfl_up, half2);
REGISTER_SIMT_ASC_SHFL_UP(__shfl_up, bfloat16_t);
REGISTER_SIMT_ASC_SHFL_UP(__shfl_up, bfloat16x2_t);

REGISTER_SIMT_ASC_SHFL_DOWN(__shfl_down, uint32_t);
REGISTER_SIMT_ASC_SHFL_DOWN(__shfl_down, int32_t);
REGISTER_SIMT_ASC_SHFL_DOWN(__shfl_down, uint64_t);
REGISTER_SIMT_ASC_SHFL_DOWN(__shfl_down, int64_t);
REGISTER_SIMT_ASC_SHFL_DOWN(__shfl_down, float);
REGISTER_SIMT_ASC_SHFL_DOWN(__shfl_down, half);
REGISTER_SIMT_ASC_SHFL_DOWN(__shfl_down, half2);
REGISTER_SIMT_ASC_SHFL_DOWN(__shfl_down, bfloat16_t);
REGISTER_SIMT_ASC_SHFL_DOWN(__shfl_down, bfloat16x2_t);

REGISTER_SIMT_ASC_SHFL_XOR(__shfl_xor, uint32_t);
REGISTER_SIMT_ASC_SHFL_XOR(__shfl_xor, int32_t);
REGISTER_SIMT_ASC_SHFL_XOR(__shfl_xor, uint64_t);
REGISTER_SIMT_ASC_SHFL_XOR(__shfl_xor, int64_t);
REGISTER_SIMT_ASC_SHFL_XOR(__shfl_xor, float);
REGISTER_SIMT_ASC_SHFL_XOR(__shfl_xor, half);
REGISTER_SIMT_ASC_SHFL_XOR(__shfl_xor, half2);
REGISTER_SIMT_ASC_SHFL_XOR(__shfl_xor, bfloat16_t);
REGISTER_SIMT_ASC_SHFL_XOR(__shfl_xor, bfloat16x2_t);
#endif

template <typename T>
T ReduceAddCPU(T val)
{
    uint32_t warp_id = AscendC::Simt::GetWarpId();
    AscendC::Simt::Warp& warp = AscendC::Simt::ThreadBlock::GetBlockInstance().warps_[warp_id];
    auto custom_func = [](T a, T b) -> T { return a + b; };
    return warp.WarpOp(val, custom_func);
}

#define REGISTER_SIMT_REDUCE_ADD(INTRI, TYPE) \
    TYPE INTRI(TYPE val) { return ReduceAddCPU<TYPE>(val); }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
REGISTER_SIMT_REDUCE_ADD(__reduce_add, uint32_t);
REGISTER_SIMT_REDUCE_ADD(__reduce_add, int32_t);
REGISTER_SIMT_REDUCE_ADD(__reduce_add, float);
REGISTER_SIMT_REDUCE_ADD(__reduce_add, half);
#endif

template <typename T>
T ReduceMaxCPU(T val)
{
    uint32_t warp_id = AscendC::Simt::GetWarpId();
    AscendC::Simt::Warp& warp = AscendC::Simt::ThreadBlock::GetBlockInstance().warps_[warp_id];
    auto custom_func = [](T a, T b) -> T { return a > b ? a : b; };
    return warp.WarpOp(val, custom_func);
}

#define REGISTER_SIMT_REDUCE_MAX(INTRI, TYPE) \
    TYPE INTRI(TYPE val) { return ReduceMaxCPU<TYPE>(val); }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
REGISTER_SIMT_REDUCE_MAX(__reduce_max, uint32_t);
REGISTER_SIMT_REDUCE_MAX(__reduce_max, int32_t);
REGISTER_SIMT_REDUCE_MAX(__reduce_max, float);
REGISTER_SIMT_REDUCE_MAX(__reduce_max, half);
#endif

template <typename T>
T ReduceMinCPU(T val)
{
    uint32_t warp_id = AscendC::Simt::GetWarpId();
    AscendC::Simt::Warp& warp = AscendC::Simt::ThreadBlock::GetBlockInstance().warps_[warp_id];
    auto custom_func = [](T a, T b) -> T { return a < b ? a : b; };
    return warp.WarpOp(val, custom_func);
}

#define REGISTER_SIMT_REDUCE_MIN(INTRI, TYPE) \
    TYPE INTRI(TYPE val) { return ReduceMinCPU<TYPE>(val); }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
REGISTER_SIMT_REDUCE_MIN(__reduce_min, uint32_t);
REGISTER_SIMT_REDUCE_MIN(__reduce_min, int32_t);
REGISTER_SIMT_REDUCE_MIN(__reduce_min, float);
REGISTER_SIMT_REDUCE_MIN(__reduce_min, half);
#endif

#endif
