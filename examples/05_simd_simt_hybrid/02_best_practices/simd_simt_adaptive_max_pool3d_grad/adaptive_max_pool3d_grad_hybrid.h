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
#include "simt_api/asc_simt.h"

namespace hybrid_impl {

using pool3d_common::fill_float_ub;
using pool3d_common::min;
using pool3d_common::split_nc;
using pool3d_common::wait_mte3_to_scalar;
using pool3d_common::wait_v_to_mte3;

constexpr uint32_t THREAD_COUNT = 1024;
constexpr uint32_t ZERO_TILE_COUNT = 4096;

template <typename T, typename IndexT>
__simt_vf__ __launch_bounds__(THREAD_COUNT) inline void scatter_grad(
    const __gm__ T* grad, const __gm__ IndexT* argmax, __gm__ T* output, uint32_t nc_count, uint32_t output_spatial,
    uint32_t grad_spatial)
{
    const uint64_t block_grad_count = static_cast<uint64_t>(nc_count) * grad_spatial;
    for (uint64_t local_grad_index = static_cast<uint64_t>(threadIdx.x); local_grad_index < block_grad_count;
         local_grad_index += static_cast<uint64_t>(blockDim.x)) {
        const uint64_t local_nc_index = static_cast<uint32_t>(local_grad_index) / grad_spatial;
        const uint64_t output_index = local_nc_index * output_spatial + static_cast<uint64_t>(argmax[local_grad_index]);
        output[output_index] = grad[local_grad_index];
    }
}

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

} // namespace hybrid_impl

__global__ __vector__ void adaptive_max_pool3d_grad_hybrid_kernel(
    __gm__ uint8_t* grad, __gm__ uint8_t* argmax, __gm__ uint8_t* output, uint32_t nc, uint32_t output_spatial,
    uint32_t grad_spatial)
{
    asc_init();

    __gm__ float* grad_data = reinterpret_cast<__gm__ float*>(grad);
    __gm__ int32_t* argmax_data = reinterpret_cast<__gm__ int32_t*>(argmax);
    __gm__ float* output_data = reinterpret_cast<__gm__ float*>(output);

    // Contiguous split: zero_output copies one long run, and scatter_grad treats this block's grad as a flat array.
    uint32_t nc_start = 0;
    uint32_t nc_count = 0;
    hybrid_impl::split_nc(nc, nc_start, nc_count);

    __gm__ float* grad_slice = grad_data + static_cast<uint64_t>(nc_start) * grad_spatial;
    __gm__ int32_t* argmax_slice = argmax_data + static_cast<uint64_t>(nc_start) * grad_spatial;
    __gm__ float* output_slice = output_data + static_cast<uint64_t>(nc_start) * output_spatial;

    hybrid_impl::zero_output(output_slice, static_cast<uint64_t>(nc_count) * output_spatial);
    asc_vf_call<hybrid_impl::scatter_grad<float, int32_t>>(
        dim3(hybrid_impl::THREAD_COUNT), grad_slice, argmax_slice, output_slice, nc_count, output_spatial,
        grad_spatial);
}
