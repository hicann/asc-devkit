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

namespace simd_impl {

using pool3d_common::min;
using pool3d_common::wait_mte2_to_v;
using pool3d_common::wait_mte3_to_v;
using pool3d_common::wait_scalar_to_v;
using pool3d_common::wait_v_to_mte3;
using pool3d_common::wait_v_to_scalar;

constexpr uint32_t NC_TILE = 64;
constexpr uint32_t GM_BLOCK_BYTES = 32;
constexpr uint32_t W_COMPARE_TILE = 32;
constexpr uint32_t B32_ELEM_COUNT = GM_BLOCK_BYTES / sizeof(float);

// Global memory tensors used by the SIMD implementation.
struct GlobalTensors {
    __gm__ float* grad;
    __gm__ int32_t* argmax;
    __gm__ float* output;
};

// Runtime shape parameters in [NC, D, H, W] order.
struct Pool3DShape {
    uint32_t nc;
    uint32_t output_d;
    uint32_t output_h;
    uint32_t output_w;
    uint32_t grad_d;
    uint32_t grad_h;
    uint32_t grad_w;
};

// UB buffers allocated by the global kernel entry and passed down to the helper functions.
struct UbBuffers {
    __ubuf__ float* grad;
    __ubuf__ int32_t* argmax;
    __ubuf__ uint32_t* scatter_offsets;
    __ubuf__ float* selected_tile;
    __ubuf__ float* expanded;
};

// Per-core execution context shared by the scalar control code and the SIMD helper functions.
struct SimdContext {
    GlobalTensors tensors;
    Pool3DShape shape;
    UbBuffers ub;
    uint32_t output_spatial;
    uint32_t grad_spatial;
    uint32_t nc_tile_count;
    uint32_t nc_start;
    uint32_t nc_count;
};

// For one gradient, compare argmax against a W tile of output and keep the grad lanes that hit.
__simd_vf__ inline void select_w_tile(
    __ubuf__ float* grad, __ubuf__ int32_t* argmax, __ubuf__ float* selected_tile, uint32_t base_output_index,
    uint32_t tile_count, uint32_t nc_count)
{
    vector_bool active_mask = asc_update_mask_b32(nc_count);
    vector_bool cmp_mask;
    vector_float grad_reg;
    vector_float zero_reg;
    vector_float selected_reg;
    vector_int32_t argmax_reg;
    vector_int32_t output_index_reg;

    asc_duplicate_scalar(zero_reg, 0.0F, active_mask);
    asc_loadalign(grad_reg, grad);
    asc_loadalign(argmax_reg, argmax);

    for (uint32_t tile_index = 0; tile_index < tile_count; ++tile_index) {
        asc_duplicate_scalar(output_index_reg, static_cast<int32_t>(base_output_index + tile_index), active_mask);
        asc_eq(cmp_mask, argmax_reg, output_index_reg, active_mask);
        asc_select(selected_reg, grad_reg, zero_reg, cmp_mask);
        asc_storealign(selected_tile + tile_index * NC_TILE, selected_reg, active_mask);
    }
}

// Scatter the NC-contiguous selected values into a UB layout with 32B spacing, ready for the strided write back to GM.
__simd_vf__ inline void scatter_selected_w(
    __ubuf__ float* selected, __ubuf__ uint32_t* scatter_offsets, __ubuf__ float* expanded, uint32_t nc_count)
{
    vector_bool active_mask = asc_update_mask_b32(nc_count);
    vector_float selected_reg;
    vector_uint32_t scatter_index_reg;

    asc_loadalign(selected_reg, selected);
    asc_loadalign(scatter_index_reg, scatter_offsets);
    asc_scatter(expanded, selected_reg, scatter_index_reg, active_mask);
}

// Initialize the derived dimensions and the context information reused by this kernel.
__aicore__ inline void init_context(
    SimdContext& ctx, const GlobalTensors& tensors, const Pool3DShape& shape, const UbBuffers& ub)
{
    ctx.tensors = tensors;
    ctx.shape = shape;
    ctx.ub = ub;
    ctx.output_spatial = shape.output_d * shape.output_h * shape.output_w;
    ctx.grad_spatial = shape.grad_d * shape.grad_h * shape.grad_w;
    ctx.nc_tile_count = (shape.nc + NC_TILE - 1U) / NC_TILE;
}

// Prepare the scatter offsets so that every NC lane lands at the start of its own 32B UB slot.
__aicore__ inline void init_scatter_offsets(SimdContext& ctx)
{
    for (uint32_t i = 0; i < NC_TILE; ++i) {
        ctx.ub.scatter_offsets[i] = i * B32_ELEM_COUNT;
    }
    wait_scalar_to_v();
}

// Copy the grad and argmax of one gradient coordinate across all NC lanes into UB, laid out contiguously so that a
// vector load can read them directly.
__aicore__ inline void copy_in_grad_argmax(SimdContext& ctx, uint32_t grad_index)
{
    const uint64_t gm_offset = static_cast<uint64_t>(ctx.nc_start) * ctx.grad_spatial + grad_index;
    // One element per NC lane, striding one grad plane on the GM side. dst_stride == len_burst selects Compact mode,
    // which packs the lanes contiguously in UB.
    const uint64_t gm_stride = static_cast<uint64_t>(ctx.grad_spatial) * sizeof(float);
    asc_copy_gm2ub_align(
        ctx.ub.grad, ctx.tensors.grad + gm_offset, static_cast<uint16_t>(ctx.nc_count), sizeof(float), 0, 0, false,
        asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, gm_stride, sizeof(float));
    asc_copy_gm2ub_align(
        ctx.ub.argmax, ctx.tensors.argmax + gm_offset, static_cast<uint16_t>(ctx.nc_count), sizeof(int32_t), 0, 0,
        false, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, gm_stride, sizeof(int32_t));
}

// Write the grad values selected for one output spatial position back to GM, which is laid out as [NC, output_spatial].
__aicore__ inline void store_selected_output(SimdContext& ctx, uint32_t output_index, uint32_t tile_index)
{
    asc_vf_call<scatter_selected_w>(
        ctx.ub.selected_tile + tile_index * NC_TILE, ctx.ub.scatter_offsets, ctx.ub.expanded, ctx.nc_count);
    wait_v_to_mte3();
    const uint64_t output_gm_offset = static_cast<uint64_t>(ctx.nc_start) * ctx.output_spatial + output_index;
    // Each 32B slot of expanded holds one selected value, matching the stride of the write back across NC.
    asc_copy_ub2gm_align(
        ctx.tensors.output + output_gm_offset, ctx.ub.expanded, static_cast<uint16_t>(ctx.nc_count), sizeof(float),
        asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, static_cast<uint64_t>(ctx.output_spatial) * sizeof(float),
        GM_BLOCK_BYTES);
    wait_mte3_to_v();
}

// Process one output row segment covered by the region that the current gradient occupies.
__aicore__ inline void process_output_row(SimdContext& ctx, uint32_t d, uint32_t h, uint32_t w_start, uint32_t w_end)
{
    const uint32_t row_base_output_index = (d * ctx.shape.output_h + h) * ctx.shape.output_w;
    for (uint32_t w = w_start; w < w_end; w += W_COMPARE_TILE) {
        const uint32_t tile_count = min(W_COMPARE_TILE, w_end - w);
        // Compute the output data of the current w tile into ctx.ub.selected_tile.
        asc_vf_call<select_w_tile>(
            ctx.ub.grad, ctx.ub.argmax, ctx.ub.selected_tile, row_base_output_index + w, tile_count, ctx.nc_count);

        for (uint32_t tile_index = 0; tile_index < tile_count; ++tile_index) {
            // Move the selected_tile data from UB to its corresponding position in GM.
            store_selected_output(ctx, row_base_output_index + w + tile_index, tile_index);
        }
    }
}

// Convert a gradient coordinate into the region it occupies in output, and process that region.
__aicore__ inline void process_grad_window(SimdContext& ctx, uint32_t gd_index, uint32_t gh_index, uint32_t gw_index)
{
    const uint32_t d_start = gd_index * ctx.shape.output_d / ctx.shape.grad_d;
    const uint32_t d_end = ((gd_index + 1U) * ctx.shape.output_d + ctx.shape.grad_d - 1U) / ctx.shape.grad_d;
    const uint32_t h_start = gh_index * ctx.shape.output_h / ctx.shape.grad_h;
    const uint32_t h_end = ((gh_index + 1U) * ctx.shape.output_h + ctx.shape.grad_h - 1U) / ctx.shape.grad_h;
    const uint32_t w_start = gw_index * ctx.shape.output_w / ctx.shape.grad_w;
    const uint32_t w_end = ((gw_index + 1U) * ctx.shape.output_w + ctx.shape.grad_w - 1U) / ctx.shape.grad_w;

    for (uint32_t d = d_start; d < d_end; ++d) {
        for (uint32_t h = h_start; h < h_end; ++h) {
            process_output_row(ctx, d, h, w_start, w_end);
        }
    }
}

// Process one gradient coordinate: copy in the data of all NC lanes at that coordinate, then fill the window it
// occupies in output.
__aicore__ inline void process_grad(SimdContext& ctx, uint32_t gd_index, uint32_t gh_index, uint32_t gw_index)
{
    const uint32_t grad_index = (gd_index * ctx.shape.grad_h + gh_index) * ctx.shape.grad_w + gw_index;
    // MTE2 may only overwrite these two UB buffers once the previous gradient's vector computation has finished
    // reading grad/argmax.
    wait_v_to_scalar();
    copy_in_grad_argmax(ctx, grad_index);
    wait_mte2_to_v();
    process_grad_window(ctx, gd_index, gh_index, gw_index);
}

__aicore__ inline void process(SimdContext& ctx)
{
    init_scatter_offsets(ctx);
    // Process NC in tiles of NC_TILE = 64.
    for (uint32_t nc_tile_index = block_idx; nc_tile_index < ctx.nc_tile_count; nc_tile_index += block_num) {
        ctx.nc_start = nc_tile_index * NC_TILE;
        ctx.nc_count = min(NC_TILE, ctx.shape.nc - ctx.nc_start);
        asc_dcci_entire_all();
        // Iterate over every gradient coordinate of this NC tile; their windows tile output exactly.
        for (uint32_t gd = 0; gd < ctx.shape.grad_d; ++gd) {
            for (uint32_t gh = 0; gh < ctx.shape.grad_h; ++gh) {
                for (uint32_t gw = 0; gw < ctx.shape.grad_w; ++gw) {
                    process_grad(ctx, gd, gh, gw);
                }
            }
        }
    }
    asc_dcci_entire_all();
}

} // namespace simd_impl

// SIMD global kernel entry: allocates the UB buffers, initializes the context, and runs this core's work.
__global__ __vector__ void adaptive_max_pool3d_grad_simd_kernel(
    __gm__ uint8_t* grad, __gm__ uint8_t* argmax, __gm__ uint8_t* output, uint32_t nc, uint32_t output_d,
    uint32_t output_h, uint32_t output_w, uint32_t grad_d, uint32_t grad_h, uint32_t grad_w)
{
    asc_init();

    __gm__ float* grad_data = reinterpret_cast<__gm__ float*>(grad);
    __gm__ int32_t* argmax_data = reinterpret_cast<__gm__ int32_t*>(argmax);
    __gm__ float* output_data = reinterpret_cast<__gm__ float*>(output);
    const simd_impl::GlobalTensors tensors{grad_data, argmax_data, output_data};
    const simd_impl::Pool3DShape shape{nc, output_d, output_h, output_w, grad_d, grad_h, grad_w};

    __ubuf__ float grad_ub[simd_impl::NC_TILE];
    __ubuf__ int32_t argmax_ub[simd_impl::NC_TILE];
    __ubuf__ uint32_t scatter_offsets[simd_impl::NC_TILE];
    __ubuf__ float selected_tile[simd_impl::W_COMPARE_TILE * simd_impl::NC_TILE];
    __ubuf__ float expanded[simd_impl::NC_TILE * simd_impl::B32_ELEM_COUNT];
    const simd_impl::UbBuffers ub{grad_ub, argmax_ub, scatter_offsets, selected_tile, expanded};

    simd_impl::SimdContext ctx;
    simd_impl::init_context(ctx, tensors, shape, ub);
    simd_impl::process(ctx);
}
