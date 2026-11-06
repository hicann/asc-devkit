/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning \
    "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/copy_common.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_COPY_COMMON_H
#endif

/*!
 * \file copy_common.h
 * \brief Shared helpers for the gm_to_l1 copy implementations.
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_COPY_COMMON_H
#define IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_COPY_COMMON_H

#include "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/instruction.h"

namespace asc {
namespace te {

__aicore__ constexpr gm_to_l1_trait::gm_to_l1_trait(bool pre_allocation) : pre_allocation(pre_allocation) {}

// Shared GM->L1 batch dispatch for the four copy ops (ND2Nz/DN2Nz/ND2Zn/DN2Zn). The op-specific
// single-matrix parameter extraction lives in CopyOp::EmitCopy; this routine handles the parts that
// are identical across ops: depth-based batch detection, stripping the leading B axis, and reading
// the per-batch counts/strides. CopyOp must provide static check_template<trait,DstTensor,SrcTensor>() and
// emit_copy(dst, src, src_layout, dst_layout, matrix_num, src_matrix_stride, dst_matrix_stride).
//   - Non-batch (depth 2/4): pass the full layouts, matrix_num=1, strides=0.
//   - Batch (depth 3/5): strip B with remove_batch_dim (keeps pattern/trait), matrix_num/strides from
//     the B axis. The GM stride comes straight from the layout, so both bmk-contiguous and
//     mbk-non-contiguous memory are covered.
template <const gm_to_l1_trait& trait, typename CopyOp, typename DstTensor, typename SrcTensor>
__aicore__ inline void run_gm_to_l1_batched(const DstTensor& dst, const SrcTensor& src)
{
    CopyOp::template check_template<trait, DstTensor, SrcTensor>();
    constexpr auto src_depth = nesting_depth_v<decltype(src.layout().shape())>;
    if constexpr (src_depth == three_dim_data || src_depth == five_dim_data) {
        auto src_layout = src.layout();
        auto dst_layout = dst.layout();
        uint16_t matrix_num = get<0>(src_layout.shape());
        uint64_t src_matrix_stride = get<0>(src_layout.stride());
        uint32_t dst_matrix_stride = get<0>(dst_layout.stride());
        CopyOp::template emit_copy<trait>(
            dst, src, remove_batch_dim(src_layout), remove_batch_dim(dst_layout), matrix_num, src_matrix_stride,
            dst_matrix_stride);
    } else {
        CopyOp::template emit_copy<trait>(dst, src, src.layout(), dst.layout(), 1, 0, 0);
    }
}

template <
    const gm_to_l1_trait& trait, typename CopyOp, typename DstTensor, typename SrcTensor, typename DstCoord,
    typename SrcCoord, typename CopyShape>
__aicore__ inline void run_gm_to_l1_batched(
    const DstTensor& dst, const SrcTensor& src, const DstCoord& dst_coord, const SrcCoord& src_coord,
    const CopyShape& copy_shape)
{
    CopyOp::template check_template<trait, DstTensor, SrcTensor>();
    auto src_shape = make_slice_shape(src_coord, src.layout(), copy_shape);
    auto dst_offset = dst.layout()(dst_coord);
    auto src_offset = src.layout()(src_coord);
    uint16_t matrix_num = get_shape_batch_size(src_shape);
    uint64_t src_matrix_stride = 0;
    uint32_t dst_matrix_stride = 0;
    constexpr auto src_depth = nesting_depth_v<decltype(src.layout().shape())>;
    if constexpr (src_depth == three_dim_data || src_depth == five_dim_data) {
        src_matrix_stride = get<0>(src.layout().stride());
        dst_matrix_stride = get<0>(dst.layout().stride());
    }
    // emit_copy receives matrix strides in element units and performs instruction-specific conversion.
    CopyOp::template emit_copy<trait>(
        dst, src, src_shape, matrix_num, src_matrix_stride, dst_matrix_stride, dst_offset, src_offset);
}

// ---- c0_element_half helpers for scale copy ----
// When the ZZ/NN layout uses c0_element_half, layout stride/shape values are in different
// units than the old c0Ele approach:
//   - Outer dims: in hif4_scale units (1 hif4_scale = sizeof(type) e8m0 bytes)
//   - Inner c0_element_half dims: in half-element units (1 half-elem = sizeof(type)/2 e8m0 bytes)
// The old c0Ele approach keeps everything in e8m0 units (1 e8m0 = sizeof(fp8_e8m0_t) bytes).
// These helpers convert layout values to e8m0 units so the existing instruction formulas
// (written for e8m0) produce correct results with c0_element_half layouts.

// Byte size of one layout-element unit. For c0_element_half layouts, outer-dim values are in
// hif4_scale units (sizeof(type) bytes); for old c0Ele layouts, they are in e8m0 units
// (sizeof(fp8_e8m0_t) bytes).
template <typename LayoutType, typename Type>
constexpr size_t layout_stride_bytes =
    is_c0_element_half_v<decltype(get_layout_trait<LayoutType>::c0_element)> ? sizeof(Type) : sizeof(fp8_e8m0_t);

// Number of e8m0 elements covered by an inner-dim (within-fractal) stride/shape value.
// c0_element_half(1) covers sizeof(ElemType)/2 e8m0; a plain integer already counts e8m0
// (old c0Ele approach).
template <typename ElemType, typename Val>
__aicore__ inline constexpr auto inner_dim_e8m0_num(const Val& val)
{
    if constexpr (is_c0_element_half_v<Val>) {
        return static_cast<size_t>(val) * sizeof(ElemType) / 2;
    } else {
        return static_cast<size_t>(val);
    }
}

// Number of e8m0 elements covered by an outer-dim (fractal-level) stride/shape value.
// For c0_element_half layouts, each scale element covers sizeof(ElemType) e8m0; otherwise the
// value already counts e8m0.
template <typename LayoutType, typename ElemType, typename Val>
__aicore__ inline constexpr auto outer_dim_e8m0_num(const Val& val)
{
    if constexpr (is_c0_element_half_v<decltype(get_layout_trait<LayoutType>::c0_element)>) {
        return static_cast<size_t>(val) * sizeof(ElemType);
    } else {
        return static_cast<size_t>(val);
    }
}

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_COPY_COMMON_H

#if defined( \
    UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_COPY_COMMON_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_COPY_COMMON_H
#endif
