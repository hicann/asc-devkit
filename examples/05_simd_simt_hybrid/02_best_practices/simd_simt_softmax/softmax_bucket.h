/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#pragma once

#include <cstdint>

#include "c_api/asc_simd.h"
#include "simt_api/asc_simt.h"
#include "simt_api/device_warp_functions.h"
#include "simt_api/math_constants.h"

namespace softmax_bucket_impl {

constexpr uint32_t BUCKET_THREAD_COUNT_SMALL = 512;
constexpr uint32_t BUCKET_THREAD_COUNT_MID = 1024;
constexpr uint32_t BUCKET_THREAD_COUNT_LARGE = 2048;
constexpr uint32_t BUCKET_DYNAMIC_MAX_COLS = 256;

template <typename T>
__aicore__ inline __gm__ T* mutable_gm_ptr(__gm__ const T* ptr)
{
    return const_cast<__gm__ T*>(ptr);
}

template <uint32_t kThreadCount, uint32_t kBucketCols>
__simt_vf__ __launch_bounds__(kThreadCount) inline void softmax_bucket_vf(
    __ubuf__ half* output_ub, __ubuf__ const half* input_ub, uint32_t row_count, uint32_t cols)
{
    // 每个线程分到的元素个数，为编译期常量。
    constexpr uint32_t kWarpIterations = (kBucketCols + warpSize - 1) / warpSize;
    const uint32_t row_in_tile = threadIdx.x / warpSize;
    const uint32_t thread_in_warp = threadIdx.x % warpSize;
    if (row_in_tile >= row_count) {
        return;
    }

    // tile内每行按kBucketCols的步长存放，真实数据之外的余量空置。
    const uint32_t row_offset = row_in_tile * kBucketCols;
    // 遍历数组的循环边界与访问下标均为编译期常量，数组可驻留寄存器。
    float elements[kWarpIterations];

    // 一轮读入线程分到的元素，尾部无效列用-inf屏蔽，不参与后续计算。
    for (uint32_t iter = 0; iter < kWarpIterations; ++iter) {
        const uint32_t col = thread_in_warp + iter * warpSize;
        elements[iter] = col < cols ? static_cast<float>(input_ub[row_offset + col]) : -ASCRT_INF_F;
    }

    // 对已读入的数组元素求max，再经Warp归约得到行最大值。
    float max_value = elements[0];
    for (uint32_t iter = 1; iter < kWarpIterations; ++iter) {
        max_value = max(max_value, elements[iter]);
    }
    max_value = asc_reduce_max(max_value);

    // exp只计算一次，结果写回数组复用，并经Warp归约得到行指数和。
    float sum_value = 0.0F;
    for (uint32_t iter = 0; iter < kWarpIterations; ++iter) {
        elements[iter] = expf(elements[iter] - max_value);
        sum_value += elements[iter];
    }
    sum_value = asc_reduce_add(sum_value);

    // 归一化写回padded UB，尾部无效列写0；MTE3仅搬回真实cols内的数据。
    for (uint32_t iter = 0; iter < kWarpIterations; ++iter) {
        const uint32_t col = thread_in_warp + iter * warpSize;
        output_ub[row_offset + col] = static_cast<half>(elements[iter] / sum_value);
    }
}

template <uint32_t kThreadCount, uint32_t kBucketCols>
__global__ __vector__ void softmax_bucket_kernel(
    __gm__ half* output, __gm__ const half* input, uint32_t rows, uint32_t cols)
{
    asc_init();

    constexpr uint32_t kRowsPerTile = kThreadCount / warpSize;
    constexpr uint32_t kTileElements = kRowsPerTile * kBucketCols;
    __ubuf__ half input_tile_ub[2][kTileElements];
    __ubuf__ half output_tile_ub[2][kTileElements];

    const uint32_t tile_stride = static_cast<uint32_t>(block_num) * kRowsPerTile;
    const uint32_t first_row_base = static_cast<uint32_t>(block_idx) * kRowsPerTile;

    asc_sync_notify(PIPE_V, PIPE_MTE2, EVENT_ID0);
    asc_sync_notify(PIPE_V, PIPE_MTE2, EVENT_ID1);
    asc_sync_notify(PIPE_MTE3, PIPE_V, EVENT_ID0);
    asc_sync_notify(PIPE_MTE3, PIPE_V, EVENT_ID1);

    uint32_t tile_idx = 0;
    for (uint32_t row_base = first_row_base; row_base < rows; row_base += tile_stride) {
        const uint32_t slot = tile_idx & 1;
        const event_t event_id = slot == 0 ? EVENT_ID0 : EVENT_ID1;
        const uint32_t rows_remaining = rows - row_base;
        const uint32_t rows_in_tile = rows_remaining >= kRowsPerTile ? kRowsPerTile : rows_remaining;
        const uint32_t row_bytes = cols * sizeof(half);
        const uint32_t padded_row_bytes = kBucketCols * sizeof(half);

        asc_sync_wait(PIPE_V, PIPE_MTE2, event_id);
        // 每行按真实cols搬入，行尾由硬件自动补齐到32字节，padding区域不参与计算。
        asc_copy_gm2ub_align(
            input_tile_ub[slot], mutable_gm_ptr(input + row_base * cols), static_cast<uint16_t>(rows_in_tile),
            row_bytes, 0, 0, false, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, row_bytes, padded_row_bytes);
        asc_sync_notify(PIPE_MTE2, PIPE_V, event_id);

        asc_sync_wait(PIPE_MTE2, PIPE_V, event_id);
        asc_sync_wait(PIPE_MTE3, PIPE_V, event_id);
        asc_vf_call<softmax_bucket_vf<kThreadCount, kBucketCols>>(
            dim3(kThreadCount), output_tile_ub[slot], input_tile_ub[slot], rows_in_tile, cols);
        asc_sync_notify(PIPE_V, PIPE_MTE2, event_id);

        asc_sync_notify(PIPE_V, PIPE_MTE3, event_id);
        asc_sync_wait(PIPE_V, PIPE_MTE3, event_id);
        asc_copy_ub2gm_align(
            output + row_base * cols, output_tile_ub[slot], static_cast<uint16_t>(rows_in_tile), row_bytes,
            asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, row_bytes, padded_row_bytes);
        asc_sync_notify(PIPE_MTE3, PIPE_V, event_id);
        ++tile_idx;
    }

    asc_sync_wait(PIPE_V, PIPE_MTE2, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE2, EVENT_ID1);
    asc_sync_wait(PIPE_MTE3, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE3, PIPE_V, EVENT_ID1);
}

} // namespace softmax_bucket_impl
