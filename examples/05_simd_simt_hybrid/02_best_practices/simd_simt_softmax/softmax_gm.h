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

namespace softmax_gm_impl {

constexpr uint32_t THREADS_PER_BLOCK = 2048;
constexpr uint32_t MAX_WARPS_PER_BLOCK = THREADS_PER_BLOCK / 32; // Warp内32个线程，设备侧内置变量为warpSize

__simt_vf__ __launch_bounds__(THREADS_PER_BLOCK) inline void softmax_gm_vf(
    __gm__ half* output, __gm__ const half* input, uint32_t rows, uint32_t cols)
{
    const uint32_t thread_in_warp = threadIdx.x;
    const uint32_t warp_in_block = threadIdx.y;
    const uint32_t warps_per_block = blockDim.y;
    const uint32_t row_stride = gridDim.x * warps_per_block;

    // 一个Warp处理一行：行内max/sum归约通过Warp归约原语在Warp内32个线程之间
    // 交换数据完成，只使用Warp内部资源，不需要共享内存；行与行之间彼此独立，
    // 各Warp各自处理不同的行，也不需要跨Warp同步。行方向按grid-stride循环覆盖全部行。
    for (uint32_t row = blockIdx.x * warps_per_block + warp_in_block; row < rows; row += row_stride) {
        const uint32_t row_offset = row * cols;

        // 第一轮：从GM读取整行求最大值，asc_reduce_max在Warp内32个线程间归约。
        float max_value = -ASCRT_INF_F;
        for (uint32_t col = thread_in_warp; col < cols; col += warpSize) {
            const float value = static_cast<float>(input[row_offset + col]);
            max_value = max_value > value ? max_value : value;
        }
        max_value = asc_reduce_max(max_value);

        // 第二轮：重新读取GM，计算expf(x - max)并用asc_reduce_add在Warp内归约求和。
        float sum_value = 0.0F;
        for (uint32_t col = thread_in_warp; col < cols; col += warpSize) {
            const float value = static_cast<float>(input[row_offset + col]);
            sum_value += expf(value - max_value);
        }
        sum_value = asc_reduce_add(sum_value);

        // 第三轮：再次读取GM完成归一化写回。三轮循环读取整行三次、计算exp两次。
        for (uint32_t col = thread_in_warp; col < cols; col += warpSize) {
            const float value = static_cast<float>(input[row_offset + col]);
            output[row_offset + col] = static_cast<half>(expf(value - max_value) / sum_value);
        }
    }
}

__global__ __vector__ void softmax_gm_kernel(
    __gm__ half* output, __gm__ const half* input, uint32_t rows, uint32_t cols)
{
    asc_init();

    if (rows == 0 || cols == 0) {
        return;
    }

    const uint32_t block_count = static_cast<uint32_t>(block_num);
    const uint32_t rows_per_block = (rows + block_count - 1) / block_count;
    const uint32_t warps_per_block = rows_per_block < MAX_WARPS_PER_BLOCK ? rows_per_block : MAX_WARPS_PER_BLOCK;
    // warps_per_block随rows自适应，rows不足时每个Thread Block只处理一行；
    // asc_vf_call以dim3(32, warps_per_block)启动SIMT VF：x维为Warp内32个线程，
    // y维为Warp数，每个Warp对应一行。
    asc_vf_call<softmax_gm_vf>(dim3(warpSize, warps_per_block), output, input, rows, cols);
}

} // namespace softmax_gm_impl
