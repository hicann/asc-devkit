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

#include "adaptive_max_pool3d_grad_common.h"
#include "c_api/asc_simd.h"
#include "kernel_operator.h"

namespace scalar_impl {

using pool3d_common::fill_float_ub;
using pool3d_common::min;
using pool3d_common::split_nc;
using pool3d_common::wait_mte2_to_scalar;
using pool3d_common::wait_mte3_to_scalar;
using pool3d_common::wait_v_to_mte3;

constexpr uint32_t GRAD_TILE_COUNT = 4096;
constexpr uint32_t ZERO_TILE_COUNT = 4096;

// Build a block of zeros in UB with SIMD, then copy it contiguously over the whole output slice this core owns.
__aicore__ inline void zero_output(__gm__ float* output_data, uint64_t count)
{
    __ubuf__ float zero_ub[ZERO_TILE_COUNT];
    asc_vf_call<fill_float_ub>(zero_ub, 0.0F, ZERO_TILE_COUNT);
    wait_v_to_mte3();

    for (uint64_t offset = 0; offset < count; offset += ZERO_TILE_COUNT) {
        const uint32_t copy_count = static_cast<uint32_t>(min<uint64_t>(ZERO_TILE_COUNT, count - offset));
        const uint32_t copy_bytes = copy_count * sizeof(float);
        asc_copy_ub2gm(output_data + offset, zero_ub, copy_bytes);
    }
    wait_mte3_to_scalar();
}

__aicore__ inline void process(
    __gm__ float* grad, __gm__ int32_t* argmax, __gm__ float* output, uint32_t nc_count, uint32_t output_spatial,
    uint32_t grad_spatial)
{
    __ubuf__ float grad_ub[GRAD_TILE_COUNT];
    __ubuf__ int32_t argmax_ub[GRAD_TILE_COUNT];

    const uint32_t block_grad_count = nc_count * grad_spatial;
    zero_output(output, static_cast<uint64_t>(nc_count) * output_spatial);
    asc_dcci_entire_all();

    for (uint32_t grad_offset = 0; grad_offset < block_grad_count; grad_offset += GRAD_TILE_COUNT) {
        const uint32_t tile_grad_count = min(GRAD_TILE_COUNT, block_grad_count - grad_offset);

        asc_copy_gm2ub(grad_ub, grad + grad_offset, tile_grad_count * sizeof(float));
        asc_copy_gm2ub(argmax_ub, argmax + grad_offset, tile_grad_count * sizeof(int32_t));
        wait_mte2_to_scalar();

        for (uint32_t i = 0; i < tile_grad_count; ++i) {
            const uint64_t local_grad_index = static_cast<uint64_t>(grad_offset + i);
            const uint64_t local_nc_index = local_grad_index / grad_spatial;
            const uint64_t target = local_nc_index * output_spatial + static_cast<uint64_t>(argmax_ub[i]);
            output[target] = grad_ub[i];
        }
    }

    asc_dcci_entire_all();
}

} // namespace scalar_impl

__global__ __vector__ void adaptive_max_pool3d_grad_scalar_kernel(
    __gm__ uint8_t* grad, __gm__ uint8_t* argmax, __gm__ uint8_t* output, uint32_t nc, uint32_t output_spatial,
    uint32_t grad_spatial)
{
    asc_init();

    // Contiguous split: zero_output and the scalar scatter both run over this block's whole NC range in one pass.
    uint32_t nc_start = 0;
    uint32_t nc_count = 0;
    scalar_impl::split_nc(nc, nc_start, nc_count);

    __gm__ float* grad_data = reinterpret_cast<__gm__ float*>(grad);
    __gm__ int32_t* argmax_data = reinterpret_cast<__gm__ int32_t*>(argmax);
    __gm__ float* output_data = reinterpret_cast<__gm__ float*>(output);

    __gm__ float* grad_slice = grad_data + static_cast<uint64_t>(nc_start) * grad_spatial;
    __gm__ int32_t* argmax_slice = argmax_data + static_cast<uint64_t>(nc_start) * grad_spatial;
    __gm__ float* output_slice = output_data + static_cast<uint64_t>(nc_start) * output_spatial;

    scalar_impl::process(grad_slice, argmax_slice, output_slice, nc_count, output_spatial, grad_spatial);
}
