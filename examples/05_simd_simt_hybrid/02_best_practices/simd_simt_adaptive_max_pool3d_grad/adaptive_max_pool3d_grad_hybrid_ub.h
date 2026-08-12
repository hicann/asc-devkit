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

namespace hybrid_ub_impl {

using pool3d_common::fill_float_ub;
using pool3d_common::split_nc;
using pool3d_common::wait_mte3_to_scalar;
using pool3d_common::wait_v_to_mte3;

constexpr uint32_t THREAD_COUNT = 1024;
constexpr uint32_t OUTPUT_UB_SIZE = 32U * 32U * 32U;

template <typename T, typename IndexT>
__simt_vf__ __launch_bounds__(THREAD_COUNT) inline void scatter_grad(
    const __gm__ T* grad, const __gm__ IndexT* argmax, __ubuf__ T* output, uint32_t output_spatial,
    uint32_t grad_spatial)
{
    for (uint32_t local_grad_index = threadIdx.x; local_grad_index < grad_spatial; local_grad_index += blockDim.x) {
        const uint32_t output_index = static_cast<uint32_t>(argmax[local_grad_index]);
        if (output_index < output_spatial) {
            output[output_index] = grad[local_grad_index];
        }
    }
}

} // namespace hybrid_ub_impl

__global__ __vector__ void adaptive_max_pool3d_grad_hybrid_ub_kernel(
    __gm__ uint8_t* grad, __gm__ uint8_t* argmax, __gm__ uint8_t* output, uint32_t nc, uint32_t output_spatial,
    uint32_t grad_spatial)
{
    asc_init();

    __gm__ float* grad_data = reinterpret_cast<__gm__ float*>(grad);
    __gm__ int32_t* argmax_data = reinterpret_cast<__gm__ int32_t*>(argmax);
    __gm__ float* output_data = reinterpret_cast<__gm__ float*>(output);

    __ubuf__ float output_ub[hybrid_ub_impl::OUTPUT_UB_SIZE];

    // Contiguous split, matching the scalar and hybrid implementations.
    uint32_t nc_start = 0;
    uint32_t nc_count = 0;
    hybrid_ub_impl::split_nc(nc, nc_start, nc_count);

    const uint32_t nc_end = nc_start + nc_count;
    for (uint32_t nc_index = nc_start; nc_index < nc_end; ++nc_index) {
        asc_vf_call<hybrid_ub_impl::fill_float_ub>(output_ub, 0.0F, output_spatial);
        asc_sync();

        const uint64_t grad_base_offset = static_cast<uint64_t>(nc_index) * grad_spatial;
        asc_vf_call<hybrid_ub_impl::scatter_grad<float, int32_t>>(
            dim3(hybrid_ub_impl::THREAD_COUNT), grad_data + grad_base_offset, argmax_data + grad_base_offset, output_ub,
            output_spatial, grad_spatial);
        asc_sync_data_barrier(mem_dsb_t::DSB_UB);
        hybrid_ub_impl::wait_v_to_mte3();

        const uint32_t active_output_bytes = output_spatial * sizeof(float);
        const uint64_t output_base_offset = static_cast<uint64_t>(nc_index) * output_spatial;
        asc_copy_ub2gm_align(output_data + output_base_offset, output_ub, active_output_bytes);
        hybrid_ub_impl::wait_mte3_to_scalar();
    }
}
