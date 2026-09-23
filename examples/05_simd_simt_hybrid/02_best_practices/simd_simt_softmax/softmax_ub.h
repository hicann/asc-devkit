/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#pragma once

#include <cstdint>

#include "c_api/asc_simd.h"
#include "simt_api/asc_simt.h"
#include "simt_api/device_warp_functions.h"
#include "simt_api/math_constants.h"

namespace softmax_ub_impl {

constexpr uint32_t THREADS_PER_BLOCK = 2048;
constexpr uint32_t MAX_WARPS_PER_BLOCK = THREADS_PER_BLOCK / 32; // Warp内32个线程，设备侧内置变量为warpSize
constexpr uint32_t DOUBLE_BUFFER = 2;
constexpr uint32_t UB_TILE_BUFFER_COUNT = DOUBLE_BUFFER * 2;
constexpr uint32_t BLOCK_SIZE_BYTES = 32;
// 上板实测的动态UB申请上限：224KB UB中约8KB为框架/栈预留，超过216KB的动态UB启动会失败（ret 107000）。
constexpr uint32_t UB_DYNAMIC_BUDGET_BYTES = 216 * 1024;

inline uint32_t ceil_div_host(uint32_t numerator, uint32_t denominator)
{
    return (numerator + denominator - 1) / denominator;
}

inline uint32_t ceil_align_host(uint32_t value, uint32_t alignment)
{
    return ceil_div_host(value, alignment) * alignment;
}

__aicore__ inline uint32_t ceil_div(uint32_t numerator, uint32_t denominator)
{
    return (numerator + denominator - 1) / denominator;
}

__aicore__ inline uint32_t ceil_align(uint32_t value, uint32_t alignment)
{
    return ceil_div(value, alignment) * alignment;
}

template <typename T>
__aicore__ inline __gm__ T* mutable_gm_ptr(__gm__ const T* ptr)
{
    return const_cast<__gm__ T*>(ptr);
}

inline uint32_t calc_ub_size(uint32_t rows, uint32_t cols, uint32_t block_count)
{
    if (rows == 0 || cols == 0 || block_count == 0) {
        return 0;
    }

    const uint32_t ub_stride = ceil_align_host(cols, BLOCK_SIZE_BYTES / sizeof(uint16_t));
    const uint32_t bytes_per_row = UB_TILE_BUFFER_COUNT * ub_stride * sizeof(uint16_t);
    const uint32_t max_rows_by_ub = UB_DYNAMIC_BUDGET_BYTES / bytes_per_row;
    if (max_rows_by_ub == 0) {
        return 0;
    }

    const uint32_t rows_per_block = ceil_div_host(rows, block_count);
    const uint32_t max_warps = max_rows_by_ub < MAX_WARPS_PER_BLOCK ? max_rows_by_ub : MAX_WARPS_PER_BLOCK;
    const uint32_t rows_per_tile = rows_per_block < max_warps ? rows_per_block : max_warps;
    const uint32_t tile_elements = rows_per_tile * ub_stride;
    const uint32_t tile_bytes = ceil_align_host(tile_elements * sizeof(uint16_t), BLOCK_SIZE_BYTES);
    return UB_TILE_BUFFER_COUNT * tile_bytes;
}

__simt_vf__ __launch_bounds__(THREADS_PER_BLOCK) inline void softmax_ub_vf(
    __ubuf__ half* output_ub, __ubuf__ const half* input_ub, uint32_t rows_in_tile, uint32_t cols, uint32_t ub_stride)
{
    const uint32_t thread_in_warp = threadIdx.x;
    const uint32_t row = threadIdx.y;
    if (row >= rows_in_tile) {
        return;
    }

    const uint32_t row_offset = row * ub_stride;
    float max_value = -ASCRT_INF_F;
    for (uint32_t col = thread_in_warp; col < cols; col += warpSize) {
        const float value = static_cast<float>(input_ub[row_offset + col]);
        max_value = max_value > value ? max_value : value;
    }
    max_value = asc_reduce_max(max_value);

    float sum_value = 0.0F;
    for (uint32_t col = thread_in_warp; col < cols; col += warpSize) {
        const float value = static_cast<float>(input_ub[row_offset + col]);
        sum_value += expf(value - max_value);
    }
    sum_value = asc_reduce_add(sum_value);

    for (uint32_t col = thread_in_warp; col < cols; col += warpSize) {
        const float value = static_cast<float>(input_ub[row_offset + col]);
        output_ub[row_offset + col] = static_cast<half>(expf(value - max_value) / sum_value);
    }
}

__global__ __vector__ void softmax_ub_kernel(
    __gm__ half* output, __gm__ const half* input, uint32_t rows, uint32_t cols)
{
    asc_init();

    if (rows == 0 || cols == 0) {
        return;
    }

    const uint32_t ub_stride = ceil_align(cols, BLOCK_SIZE_BYTES / sizeof(uint16_t));
    const uint32_t bytes_per_row = UB_TILE_BUFFER_COUNT * ub_stride * sizeof(uint16_t);
    const uint32_t max_rows_by_ub = UB_DYNAMIC_BUDGET_BYTES / bytes_per_row;
    if (max_rows_by_ub == 0) {
        return;
    }

    const uint32_t block_count = static_cast<uint32_t>(block_num);
    const uint32_t rows_per_block = ceil_div(rows, block_count);
    const uint32_t max_warps = max_rows_by_ub < MAX_WARPS_PER_BLOCK ? max_rows_by_ub : MAX_WARPS_PER_BLOCK;
    const uint32_t rows_per_tile = rows_per_block < max_warps ? rows_per_block : max_warps;
    const uint32_t tile_elements = rows_per_tile * ub_stride;
    const uint32_t tile_bytes = ceil_align(tile_elements * sizeof(uint16_t), BLOCK_SIZE_BYTES);

    extern __ubuf__ char dynamic_buf[];
    uint32_t ub_offset = 0;
    __ubuf__ half* input_tile_ub0 = reinterpret_cast<__ubuf__ half*>(dynamic_buf + ub_offset);
    ub_offset += tile_bytes;
    __ubuf__ half* input_tile_ub1 = reinterpret_cast<__ubuf__ half*>(dynamic_buf + ub_offset);
    ub_offset += tile_bytes;
    __ubuf__ half* output_tile_ub0 = reinterpret_cast<__ubuf__ half*>(dynamic_buf + ub_offset);
    ub_offset += tile_bytes;
    __ubuf__ half* output_tile_ub1 = reinterpret_cast<__ubuf__ half*>(dynamic_buf + ub_offset);

    const uint32_t tile_stride = block_count * rows_per_tile;
    const uint32_t first_row_base = static_cast<uint32_t>(block_idx) * rows_per_tile;
    if (first_row_base >= rows) {
        return;
    }

    const uint32_t row_bytes = cols * sizeof(uint16_t);
    const uint32_t padded_row_bytes = ub_stride * sizeof(uint16_t);

    asc_sync_notify(PIPE_V, PIPE_MTE2, EVENT_ID0);
    asc_sync_notify(PIPE_V, PIPE_MTE2, EVENT_ID1);
    asc_sync_notify(PIPE_MTE3, PIPE_V, EVENT_ID0);
    asc_sync_notify(PIPE_MTE3, PIPE_V, EVENT_ID1);

    const uint32_t first_rows_remaining = rows - first_row_base;
    const uint32_t first_rows_in_tile = first_rows_remaining < rows_per_tile ? first_rows_remaining : rows_per_tile;
    asc_sync_wait(PIPE_V, PIPE_MTE2, EVENT_ID0);
    asc_copy_gm2ub_align(
        input_tile_ub0, mutable_gm_ptr(input + first_row_base * cols), static_cast<uint16_t>(first_rows_in_tile),
        row_bytes, 0, 0, false, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, row_bytes, padded_row_bytes);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);

    uint32_t tile_idx = 0;
    for (uint32_t row_base = first_row_base; row_base < rows; row_base += tile_stride) {
        const uint32_t slot = tile_idx & 1;
        const event_t event_id = slot == 0 ? EVENT_ID0 : EVENT_ID1;
        const uint32_t rows_remaining = rows - row_base;
        const uint32_t rows_in_tile = rows_remaining < rows_per_tile ? rows_remaining : rows_per_tile;
        __ubuf__ half* input_tile_ub = slot == 0 ? input_tile_ub0 : input_tile_ub1;
        __ubuf__ half* output_tile_ub = slot == 0 ? output_tile_ub0 : output_tile_ub1;

        asc_sync_wait(PIPE_MTE2, PIPE_V, event_id);
        const uint32_t next_row_base = row_base + tile_stride;
        if (next_row_base < rows) {
            const uint32_t next_slot = slot ^ 1;
            const event_t next_event_id = next_slot == 0 ? EVENT_ID0 : EVENT_ID1;
            const uint32_t next_rows_remaining = rows - next_row_base;
            const uint32_t next_rows_in_tile =
                next_rows_remaining < rows_per_tile ? next_rows_remaining : rows_per_tile;
            __ubuf__ half* next_input_tile_ub = next_slot == 0 ? input_tile_ub0 : input_tile_ub1;

            asc_sync_wait(PIPE_V, PIPE_MTE2, next_event_id);
            asc_copy_gm2ub_align(
                next_input_tile_ub, mutable_gm_ptr(input + next_row_base * cols),
                static_cast<uint16_t>(next_rows_in_tile), row_bytes, 0, 0, false,
                asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, row_bytes, padded_row_bytes);
            asc_sync_notify(PIPE_MTE2, PIPE_V, next_event_id);
        }

        asc_sync_wait(PIPE_MTE3, PIPE_V, event_id);
        asc_vf_call<softmax_ub_vf>(
            dim3(warpSize, rows_in_tile), output_tile_ub, input_tile_ub, rows_in_tile, cols, ub_stride);
        asc_sync_notify(PIPE_V, PIPE_MTE2, event_id);

        asc_sync_notify(PIPE_V, PIPE_MTE3, event_id);
        asc_sync_wait(PIPE_V, PIPE_MTE3, event_id);
        asc_copy_ub2gm_align(
            output + row_base * cols, output_tile_ub, static_cast<uint16_t>(rows_in_tile), row_bytes,
            asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, row_bytes, padded_row_bytes);
        asc_sync_notify(PIPE_MTE3, PIPE_V, event_id);
        ++tile_idx;
    }

    asc_sync_wait(PIPE_V, PIPE_MTE2, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE2, EVENT_ID1);
    asc_sync_wait(PIPE_MTE3, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE3, PIPE_V, EVENT_ID1);
}

} // namespace softmax_ub_impl
