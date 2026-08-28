/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <gtest/gtest.h>

#include "tensor_api_runtime_validation_include.h"

namespace {

using namespace asc::te;
using tensor_api_runtime_validation::TrapException;

class TensorApiCubeOutAdditionalRuntimeValidation : public testing::Test {
protected:
    void SetUp() override { AscendC::SetGCoreType(1); }

    void TearDown() override { AscendC::SetGCoreType(0); }
};

enum class QuantVariant { NONE, SCALAR, TENSOR };

enum class BatchQuantVariant { NONE, SCALAR, SHARED_TENSOR, BATCHED_TENSOR };

template <typename CopyOperation>
struct copy_params;

template <>
struct copy_params<copy_l0c_to_gm> {
    using type = l0c_to_gm_params;
};

template <>
struct copy_params<copy_l0c_to_ub> {
    using type = l0c_to_ub_params;
};

template <>
struct copy_params<copy_l0c_to_l1> {
    using type = l0c_to_l1_params;
};

template <typename CopyOperation>
using copy_params_t = typename copy_params<CopyOperation>::type;

template <typename LocationType, typename PointerType, typename LayoutType>
auto make_tensor_at(PointerType pointer, const LayoutType& layout)
{
    return make_tensor(make_mem_ptr<LocationType>(pointer), layout);
}

template <typename LocationType, typename PointerType, typename LayoutType>
auto make_tensor_directly(PointerType pointer, const LayoutType& layout)
{
    auto iterator = make_mem_ptr<LocationType>(pointer);
    using TensorType = decltype(make_tensor(iterator, layout));
    using EngineType = typename TensorType::engine_type;
    return TensorType{EngineType{iterator}, layout};
}

enum class InvalidLayoutKind { ZERO_SHAPE, NEGATIVE_SHAPE, STRIDE };

enum class InvalidTensorRole { DST, SRC, QUANT, FM, FILTER, BIAS };

template <typename ElementType, InvalidLayoutKind kind>
auto make_invalid_nd_layout()
{
    constexpr int M = 16;
    constexpr int N = 16;
    if constexpr (kind == InvalidLayoutKind::ZERO_SHAPE) {
        int invalid_n = 0;
        return make_frame_layout<nd_layout_ptn, ElementType>(M, invalid_n);
    } else if constexpr (kind == InvalidLayoutKind::NEGATIVE_SHAPE) {
        int invalid_n = -1;
        auto invalid_shape_layout = make_frame_layout<nd_layout_ptn, ElementType>(M, invalid_n);
        auto valid_layout = make_frame_layout<nd_layout_ptn, ElementType>(M, N);
        using LayoutTraitType = get_layout_trait<decltype(valid_layout)>;
        return make_pattern_layout<nd_layout_ptn, LayoutTraitType>(invalid_shape_layout.shape(), valid_layout.stride());
    } else {
        int invalid_stride = -1;
        auto valid_layout = make_frame_layout<nd_layout_ptn, ElementType>(M, N);
        using LayoutTraitType = get_layout_trait<decltype(valid_layout)>;
        return make_pattern_layout<nd_layout_ptn, LayoutTraitType>(
            valid_layout.shape(), make_stride(invalid_stride, _1{}));
    }
}

template <typename ElementType, InvalidLayoutKind kind>
auto make_invalid_ndext_layout()
{
    constexpr int M = 16;
    constexpr int N = 16;
    if constexpr (kind == InvalidLayoutKind::ZERO_SHAPE) {
        int invalid_n = 0;
        return make_frame_layout<nd_ext_layout_ptn, ElementType>(M, invalid_n);
    } else if constexpr (kind == InvalidLayoutKind::NEGATIVE_SHAPE) {
        int invalid_n = -1;
        auto invalid_shape_layout = make_frame_layout<nd_ext_layout_ptn, ElementType>(M, invalid_n);
        auto valid_layout = make_frame_layout<nd_ext_layout_ptn, ElementType>(M, N);
        using LayoutTraitType = get_layout_trait<decltype(valid_layout)>;
        return make_pattern_layout<nd_ext_layout_ptn, LayoutTraitType>(
            invalid_shape_layout.shape(), valid_layout.stride());
    } else {
        int invalid_stride = -1;
        auto valid_layout = make_frame_layout<nd_ext_layout_ptn, ElementType>(M, N);
        using LayoutTraitType = get_layout_trait<decltype(valid_layout)>;
        auto stride = make_stride(make_stride(_0{}, invalid_stride), make_stride(_0{}, _1{}));
        return make_pattern_layout<nd_ext_layout_ptn, LayoutTraitType>(valid_layout.shape(), stride);
    }
}

template <typename ElementType, InvalidLayoutKind kind, typename LayoutTraitType = layout_trait_default<ElementType>>
auto make_invalid_nz_layout()
{
    constexpr int M = 16;
    constexpr int N = 16;
    if constexpr (kind == InvalidLayoutKind::ZERO_SHAPE) {
        int invalid_n = 0;
        return make_frame_layout<nz_layout_ptn, LayoutTraitType>(M, invalid_n);
    } else if constexpr (kind == InvalidLayoutKind::NEGATIVE_SHAPE) {
        int invalid_outer_extent = -1;
        auto valid_layout = make_frame_layout<nz_layout_ptn, LayoutTraitType>(M, N);
        using ActualLayoutTraitType = get_layout_trait<decltype(valid_layout)>;
        auto shape = make_shape(
            make_shape(get<0>(get<0>(valid_layout.shape())), invalid_outer_extent), get<1>(valid_layout.shape()));
        return make_pattern_layout<nz_layout_ptn, ActualLayoutTraitType>(shape, valid_layout.stride());
    } else {
        int invalid_stride = -1;
        auto valid_layout = make_frame_layout<nz_layout_ptn, LayoutTraitType>(M, N);
        using ActualLayoutTraitType = get_layout_trait<decltype(valid_layout)>;
        auto stride = make_stride(get<0>(valid_layout.stride()), make_stride(_1{}, invalid_stride));
        return make_pattern_layout<nz_layout_ptn, ActualLayoutTraitType>(valid_layout.shape(), stride);
    }
}

template <typename ElementType, InvalidLayoutKind kind>
auto make_invalid_zn_layout()
{
    constexpr int M = 16;
    constexpr int N = 16;
    if constexpr (kind == InvalidLayoutKind::ZERO_SHAPE) {
        int invalid_n = 0;
        return make_frame_layout<zn_layout_ptn, ElementType>(M, invalid_n);
    } else if constexpr (kind == InvalidLayoutKind::NEGATIVE_SHAPE) {
        int invalid_outer_extent = -1;
        auto valid_layout = make_frame_layout<zn_layout_ptn, ElementType>(M, N);
        using LayoutTraitType = get_layout_trait<decltype(valid_layout)>;
        auto shape = make_shape(
            make_shape(get<0>(get<0>(valid_layout.shape())), invalid_outer_extent), get<1>(valid_layout.shape()));
        return make_pattern_layout<zn_layout_ptn, LayoutTraitType>(shape, valid_layout.stride());
    } else {
        int invalid_stride = -1;
        auto valid_layout = make_frame_layout<zn_layout_ptn, ElementType>(M, N);
        using LayoutTraitType = get_layout_trait<decltype(valid_layout)>;
        auto stride = make_stride(make_stride(_1{}, invalid_stride), get<1>(valid_layout.stride()));
        return make_pattern_layout<zn_layout_ptn, LayoutTraitType>(valid_layout.shape(), stride);
    }
}

template <typename LayoutPattern>
auto make_zero_row_stride_layout()
{
    constexpr int M = 16;
    constexpr int N = 16;
    if constexpr (asc::te::Std::is_same_v<LayoutPattern, nd_layout_ptn>) {
        auto valid_layout = make_frame_layout<nd_layout_ptn, float>(M, N);
        using LayoutTraitType = get_layout_trait<decltype(valid_layout)>;
        return make_pattern_layout<nd_layout_ptn, LayoutTraitType>(valid_layout.shape(), make_stride(0, _1{}));
    } else {
        auto valid_layout = make_frame_layout<nd_ext_layout_ptn, float>(M, N);
        using LayoutTraitType = get_layout_trait<decltype(valid_layout)>;
        auto stride = make_stride(make_stride(_0{}, 0), make_stride(_0{}, _1{}));
        return make_pattern_layout<nd_ext_layout_ptn, LayoutTraitType>(valid_layout.shape(), stride);
    }
}

template <typename ElementType, typename LayoutTraitType = layout_trait_default<ElementType>>
auto make_zero_nz_column_stride_layout(int m, int n)
{
    auto valid_layout = make_frame_layout<nz_layout_ptn, LayoutTraitType>(m, n);
    using ActualLayoutTraitType = get_layout_trait<decltype(valid_layout)>;
    auto stride = make_stride(get<0>(valid_layout.stride()), make_stride(_1{}, 0));
    return make_pattern_layout<nz_layout_ptn, ActualLayoutTraitType>(valid_layout.shape(), stride);
}

template <
    QuantVariant variant, typename CopyOperationType, typename CopyTraitType, typename DstTensorType,
    typename SrcTensorType, typename QuantTensorType, typename ParamsType = copy_params_t<CopyOperationType>>
void copy_with_quant_variant(
    const DstTensorType& dst, const SrcTensorType& src, const QuantTensorType& quant,
    const ParamsType& params = ParamsType{})
{
    auto atom = make_copy(CopyOperationType{}, CopyTraitType{}).with(params);
    if constexpr (variant == QuantVariant::NONE) {
        copy(atom, dst, src);
    } else if constexpr (variant == QuantVariant::SCALAR) {
        uint64_t scalar_quant = 1;
        copy(atom, dst, src, scalar_quant);
    } else {
        copy(atom, dst, src, quant);
    }
}

template <
    QuantVariant variant, typename CopyOperationType, typename CopyTraitType, typename DstTensorType,
    typename SrcTensorType, typename QuantTensorType, typename ParamsType = copy_params_t<CopyOperationType>>
void expect_copy_trap(
    const DstTensorType& dst, const SrcTensorType& src, const QuantTensorType& quant,
    const ParamsType& params = ParamsType{})
{
    EXPECT_THROW(
        (copy_with_quant_variant<variant, CopyOperationType, CopyTraitType>(dst, src, quant, params)), TrapException);
}

template <
    BatchQuantVariant variant, typename CopyOperationType, typename CopyTraitType, typename DstTensorType,
    typename SrcTensorType, typename SharedQuantTensorType, typename BatchedQuantTensorType>
void expect_batch_copy_trap(
    const DstTensorType& dst, const SrcTensorType& src, const SharedQuantTensorType& shared_quant,
    const BatchedQuantTensorType& batched_quant)
{
    auto atom = make_copy(CopyOperationType{}, CopyTraitType{}).with(copy_params_t<CopyOperationType>{});
    if constexpr (variant == BatchQuantVariant::NONE) {
        EXPECT_THROW(copy(atom, dst, src), TrapException);
    } else if constexpr (variant == BatchQuantVariant::SCALAR) {
        uint64_t scalar_quant = 1;
        EXPECT_THROW(copy(atom, dst, src, scalar_quant), TrapException);
    } else if constexpr (variant == BatchQuantVariant::SHARED_TENSOR) {
        EXPECT_THROW(copy(atom, dst, src, shared_quant), TrapException);
    } else {
        EXPECT_THROW(copy(atom, dst, src, batched_quant), TrapException);
    }
}

template <typename DstLayoutPattern, BatchQuantVariant variant>
void expect_gm_batch_capacity_trap()
{
    constexpr uint32_t SRC_BATCH = 2;
    constexpr uint32_t DST_BATCH = 1;
    constexpr uint32_t M = 32;
    constexpr uint32_t N = 32;
    alignas(512) static __cc__ float src_data[SRC_BATCH * M * N] = {};
    alignas(512) static __gm__ float dst_data[DST_BATCH * M * N] = {};
    alignas(512) static __cbuf__ uint64_t shared_quant_data[N] = {};
    alignas(512) static __cbuf__ uint64_t batched_quant_data[SRC_BATCH * N] = {};

    auto src = make_tensor_at<location::l0c>(
        src_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(SRC_BATCH, M, N));
    auto dst = make_tensor_at<location::gm>(dst_data, make_frame_layout<DstLayoutPattern, float>(DST_BATCH, M, N));
    auto shared_quant =
        make_tensor_at<location::l1>(shared_quant_data, make_frame_layout<nd_ext_layout_ptn, uint64_t>(_1{}, N));
    auto batched_quant = make_tensor_at<location::l1>(
        batched_quant_data, make_frame_layout<nd_ext_layout_ptn, uint64_t>(SRC_BATCH, _1{}, N));

    expect_batch_copy_trap<variant, copy_l0c_to_gm, l0c_to_gm_trait_default>(dst, src, shared_quant, batched_quant);
}

template <typename DstLayoutPattern, BatchQuantVariant variant>
void expect_ub_batch_capacity_trap()
{
    constexpr uint32_t SRC_BATCH = 2;
    constexpr uint32_t DST_BATCH = 1;
    constexpr uint32_t M = 32;
    constexpr uint32_t N = 32;
    alignas(512) static __cc__ float src_data[SRC_BATCH * M * N] = {};
    alignas(512) static __ubuf__ float dst_data[DST_BATCH * M * N] = {};
    alignas(512) static __cbuf__ uint64_t shared_quant_data[N] = {};
    alignas(512) static __cbuf__ uint64_t batched_quant_data[SRC_BATCH * N] = {};

    auto src = make_tensor_at<location::l0c>(
        src_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(SRC_BATCH, M, N));
    auto dst = make_tensor_at<location::ub>(dst_data, make_frame_layout<DstLayoutPattern, float>(DST_BATCH, M, N));
    auto shared_quant =
        make_tensor_at<location::l1>(shared_quant_data, make_frame_layout<nd_ext_layout_ptn, uint64_t>(_1{}, N));
    auto batched_quant = make_tensor_at<location::l1>(
        batched_quant_data, make_frame_layout<nd_ext_layout_ptn, uint64_t>(SRC_BATCH, _1{}, N));

    expect_batch_copy_trap<variant, copy_l0c_to_ub, l0c_to_ub_trait_default>(dst, src, shared_quant, batched_quant);
}

template <
    typename DstLocationType, typename CopyOperationType, typename CopyTraitType, QuantVariant variant,
    InvalidTensorRole role, InvalidLayoutKind kind, typename DstPointerType, typename SrcPointerType,
    typename QuantPointerType>
void expect_l0c_out_invalid_role_trap(DstPointerType dst_data, SrcPointerType src_data, QuantPointerType quant_data)
{
    constexpr int M = 16;
    constexpr int N = 16;
    using SrcLayoutTraitType = layout_trait_default<float, _16>;
    auto valid_dst_layout = make_frame_layout<nd_ext_layout_ptn, float>(M, N);
    auto valid_src_layout = make_frame_layout<nz_layout_ptn, SrcLayoutTraitType>(M, N);
    auto valid_quant_layout = make_frame_layout<nd_ext_layout_ptn, uint64_t>(_1{}, N);

    if constexpr (role == InvalidTensorRole::DST) {
        auto dst = make_tensor_directly<DstLocationType>(dst_data, make_invalid_ndext_layout<float, kind>());
        auto src = make_tensor_at<location::l0c>(src_data, valid_src_layout);
        auto quant = make_tensor_at<location::l1>(quant_data, valid_quant_layout);
        expect_copy_trap<variant, CopyOperationType, CopyTraitType>(dst, src, quant);
    } else if constexpr (role == InvalidTensorRole::SRC) {
        auto dst = make_tensor_at<DstLocationType>(dst_data, valid_dst_layout);
        auto src =
            make_tensor_directly<location::l0c>(src_data, make_invalid_nz_layout<float, kind, SrcLayoutTraitType>());
        auto quant = make_tensor_at<location::l1>(quant_data, valid_quant_layout);
        expect_copy_trap<variant, CopyOperationType, CopyTraitType>(dst, src, quant);
    } else {
        static_assert(role == InvalidTensorRole::QUANT && variant == QuantVariant::TENSOR);
        auto dst = make_tensor_at<DstLocationType>(dst_data, valid_dst_layout);
        auto src = make_tensor_at<location::l0c>(src_data, valid_src_layout);
        auto quant = make_tensor_directly<location::l1>(quant_data, make_invalid_ndext_layout<uint64_t, kind>());
        expect_copy_trap<variant, CopyOperationType, CopyTraitType>(dst, src, quant);
    }
}

template <QuantVariant variant, InvalidTensorRole role, InvalidLayoutKind kind>
void expect_gm_invalid_role_trap()
{
    constexpr int M = 16;
    constexpr int N = 16;
    alignas(512) static __gm__ float dst_data[M * N] = {};
    alignas(512) static __cc__ float src_data[M * N] = {};
    alignas(512) static __cbuf__ uint64_t quant_data[N] = {};
    expect_l0c_out_invalid_role_trap<location::gm, copy_l0c_to_gm, l0c_to_gm_trait_default, variant, role, kind>(
        dst_data, src_data, quant_data);
}

template <QuantVariant variant, InvalidTensorRole role, InvalidLayoutKind kind>
void expect_ub_invalid_role_trap()
{
    constexpr int M = 16;
    constexpr int N = 16;
    alignas(512) static __ubuf__ float dst_data[M * N] = {};
    alignas(512) static __cc__ float src_data[M * N] = {};
    alignas(512) static __cbuf__ uint64_t quant_data[N] = {};
    expect_l0c_out_invalid_role_trap<location::ub, copy_l0c_to_ub, l0c_to_ub_trait_default, variant, role, kind>(
        dst_data, src_data, quant_data);
}

template <QuantVariant variant>
void expect_gm_invalid_roles_for_overload()
{
    expect_gm_invalid_role_trap<variant, InvalidTensorRole::DST, InvalidLayoutKind::ZERO_SHAPE>();
    expect_gm_invalid_role_trap<variant, InvalidTensorRole::DST, InvalidLayoutKind::NEGATIVE_SHAPE>();
    expect_gm_invalid_role_trap<variant, InvalidTensorRole::DST, InvalidLayoutKind::STRIDE>();
    expect_gm_invalid_role_trap<variant, InvalidTensorRole::SRC, InvalidLayoutKind::ZERO_SHAPE>();
    expect_gm_invalid_role_trap<variant, InvalidTensorRole::SRC, InvalidLayoutKind::NEGATIVE_SHAPE>();
    expect_gm_invalid_role_trap<variant, InvalidTensorRole::SRC, InvalidLayoutKind::STRIDE>();
    if constexpr (variant == QuantVariant::TENSOR) {
        expect_gm_invalid_role_trap<variant, InvalidTensorRole::QUANT, InvalidLayoutKind::ZERO_SHAPE>();
        expect_gm_invalid_role_trap<variant, InvalidTensorRole::QUANT, InvalidLayoutKind::NEGATIVE_SHAPE>();
        expect_gm_invalid_role_trap<variant, InvalidTensorRole::QUANT, InvalidLayoutKind::STRIDE>();
    }
}

template <QuantVariant variant>
void expect_ub_invalid_roles_for_overload()
{
    expect_ub_invalid_role_trap<variant, InvalidTensorRole::DST, InvalidLayoutKind::ZERO_SHAPE>();
    expect_ub_invalid_role_trap<variant, InvalidTensorRole::DST, InvalidLayoutKind::NEGATIVE_SHAPE>();
    expect_ub_invalid_role_trap<variant, InvalidTensorRole::DST, InvalidLayoutKind::STRIDE>();
    expect_ub_invalid_role_trap<variant, InvalidTensorRole::SRC, InvalidLayoutKind::ZERO_SHAPE>();
    expect_ub_invalid_role_trap<variant, InvalidTensorRole::SRC, InvalidLayoutKind::NEGATIVE_SHAPE>();
    expect_ub_invalid_role_trap<variant, InvalidTensorRole::SRC, InvalidLayoutKind::STRIDE>();
    if constexpr (variant == QuantVariant::TENSOR) {
        expect_ub_invalid_role_trap<variant, InvalidTensorRole::QUANT, InvalidLayoutKind::ZERO_SHAPE>();
        expect_ub_invalid_role_trap<variant, InvalidTensorRole::QUANT, InvalidLayoutKind::NEGATIVE_SHAPE>();
        expect_ub_invalid_role_trap<variant, InvalidTensorRole::QUANT, InvalidLayoutKind::STRIDE>();
    }
}

template <typename DstLayoutPattern>
void expect_gm_zero_dst_stride_trap()
{
    constexpr int M = 16;
    constexpr int N = 16;
    alignas(512) static __cc__ float src_data[M * N] = {};
    alignas(512) static __gm__ float dst_data[M * N] = {};
    alignas(512) static __cbuf__ uint64_t quant_data[N] = {};

    auto src = make_tensor_at<location::l0c>(
        src_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(M, N));
    auto dst = make_tensor_at<location::gm>(dst_data, make_zero_row_stride_layout<DstLayoutPattern>());
    auto quant = make_tensor_at<location::l1>(quant_data, make_frame_layout<nd_ext_layout_ptn, uint64_t>(_1{}, N));

    expect_copy_trap<QuantVariant::NONE, copy_l0c_to_gm, l0c_to_gm_trait_default>(dst, src, quant);
}

template <typename DstLayoutPattern>
void expect_ub_zero_dst_stride_trap()
{
    constexpr int M = 16;
    constexpr int N = 16;
    alignas(512) static __cc__ float src_data[M * N] = {};
    alignas(512) static __ubuf__ float dst_data[M * N] = {};
    alignas(512) static __cbuf__ uint64_t quant_data[N] = {};

    auto src = make_tensor_at<location::l0c>(
        src_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(M, N));
    auto dst = make_tensor_at<location::ub>(dst_data, make_zero_row_stride_layout<DstLayoutPattern>());
    auto quant = make_tensor_at<location::l1>(quant_data, make_frame_layout<nd_ext_layout_ptn, uint64_t>(_1{}, N));

    expect_copy_trap<QuantVariant::NONE, copy_l0c_to_ub, l0c_to_ub_trait_default>(dst, src, quant);
}

template <QuantVariant variant>
void expect_l1_matrix_capacity_trap()
{
    constexpr int SRC_M = 32;
    constexpr int DST_M = 16;
    constexpr int N = 32;
    using LayoutTraitType = layout_trait_default<float, _16>;
    alignas(512) static __cc__ float src_data[SRC_M * N] = {};
    alignas(512) static __cbuf__ float dst_data[DST_M * N] = {};
    alignas(512) static __cbuf__ uint64_t quant_data[N] = {};

    auto src = make_tensor_at<location::l0c>(src_data, make_frame_layout<nz_layout_ptn, LayoutTraitType>(SRC_M, N));
    auto dst = make_tensor_at<location::l1>(dst_data, make_frame_layout<nz_layout_ptn, LayoutTraitType>(DST_M, N));
    auto quant = make_tensor_at<location::l1>(quant_data, make_frame_layout<nd_ext_layout_ptn, uint64_t>(_1{}, N));

    expect_copy_trap<variant, copy_l0c_to_l1, l0c_to_l1_trait_default>(dst, src, quant);
}

template <QuantVariant variant, InvalidTensorRole role, InvalidLayoutKind kind>
void expect_l1_invalid_role_trap()
{
    constexpr int M = 16;
    constexpr int N = 16;
    using LayoutTraitType = layout_trait_default<float, _16>;
    alignas(512) static __cc__ float src_data[M * N] = {};
    alignas(512) static __cbuf__ float dst_data[M * N] = {};
    alignas(512) static __cbuf__ uint64_t quant_data[N] = {};
    auto valid_src_layout = make_frame_layout<nz_layout_ptn, LayoutTraitType>(M, N);
    auto valid_dst_layout = make_frame_layout<nz_layout_ptn, LayoutTraitType>(M, N);
    auto valid_quant_layout = make_frame_layout<nd_ext_layout_ptn, uint64_t>(_1{}, N);

    if constexpr (role == InvalidTensorRole::DST) {
        auto dst = make_tensor_directly<location::l1>(dst_data, make_invalid_nz_layout<float, kind, LayoutTraitType>());
        auto src = make_tensor_at<location::l0c>(src_data, valid_src_layout);
        auto quant = make_tensor_at<location::l1>(quant_data, valid_quant_layout);
        expect_copy_trap<variant, copy_l0c_to_l1, l0c_to_l1_trait_default>(dst, src, quant);
    } else if constexpr (role == InvalidTensorRole::SRC) {
        auto dst = make_tensor_at<location::l1>(dst_data, valid_dst_layout);
        auto src =
            make_tensor_directly<location::l0c>(src_data, make_invalid_nz_layout<float, kind, LayoutTraitType>());
        auto quant = make_tensor_at<location::l1>(quant_data, valid_quant_layout);
        expect_copy_trap<variant, copy_l0c_to_l1, l0c_to_l1_trait_default>(dst, src, quant);
    } else {
        static_assert(role == InvalidTensorRole::QUANT && variant == QuantVariant::TENSOR);
        auto dst = make_tensor_at<location::l1>(dst_data, valid_dst_layout);
        auto src = make_tensor_at<location::l0c>(src_data, valid_src_layout);
        auto quant = make_tensor_directly<location::l1>(quant_data, make_invalid_ndext_layout<uint64_t, kind>());
        expect_copy_trap<variant, copy_l0c_to_l1, l0c_to_l1_trait_default>(dst, src, quant);
    }
}

template <QuantVariant variant>
void expect_l1_invalid_roles_for_overload()
{
    expect_l1_invalid_role_trap<variant, InvalidTensorRole::DST, InvalidLayoutKind::ZERO_SHAPE>();
    expect_l1_invalid_role_trap<variant, InvalidTensorRole::DST, InvalidLayoutKind::NEGATIVE_SHAPE>();
    expect_l1_invalid_role_trap<variant, InvalidTensorRole::DST, InvalidLayoutKind::STRIDE>();
    expect_l1_invalid_role_trap<variant, InvalidTensorRole::SRC, InvalidLayoutKind::ZERO_SHAPE>();
    expect_l1_invalid_role_trap<variant, InvalidTensorRole::SRC, InvalidLayoutKind::NEGATIVE_SHAPE>();
    expect_l1_invalid_role_trap<variant, InvalidTensorRole::SRC, InvalidLayoutKind::STRIDE>();
    if constexpr (variant == QuantVariant::TENSOR) {
        expect_l1_invalid_role_trap<variant, InvalidTensorRole::QUANT, InvalidLayoutKind::ZERO_SHAPE>();
        expect_l1_invalid_role_trap<variant, InvalidTensorRole::QUANT, InvalidLayoutKind::NEGATIVE_SHAPE>();
        expect_l1_invalid_role_trap<variant, InvalidTensorRole::QUANT, InvalidLayoutKind::STRIDE>();
    }
}

void expect_l1_n_limit_trap()
{
    constexpr int M = 16;
    constexpr int N = 4096;
    using LayoutTraitType = layout_trait_default<float, _16>;
    alignas(512) static __cc__ float src_data[M * N] = {};
    alignas(512) static __cbuf__ float dst_data[M * N] = {};
    alignas(512) static __cbuf__ uint64_t quant_data[N] = {};
    auto src = make_tensor_at<location::l0c>(src_data, make_frame_layout<nz_layout_ptn, LayoutTraitType>(M, N));
    auto dst = make_tensor_at<location::l1>(dst_data, make_frame_layout<nz_layout_ptn, LayoutTraitType>(M, N));
    auto quant = make_tensor_at<location::l1>(quant_data, make_frame_layout<nd_ext_layout_ptn, uint64_t>(_1{}, N));

    expect_copy_trap<QuantVariant::NONE, copy_l0c_to_l1, l0c_to_l1_trait_default>(dst, src, quant);
}

void expect_l1_m_limit_trap()
{
    constexpr int M = 65536;
    constexpr int N = 16;
    using LayoutTraitType = layout_trait_default<float, _16>;
    alignas(512) static __cc__ float src_data[M * N] = {};
    alignas(512) static __cbuf__ float dst_data[M * N] = {};
    alignas(512) static __cbuf__ uint64_t quant_data[N] = {};
    auto src = make_tensor_at<location::l0c>(src_data, make_frame_layout<nz_layout_ptn, LayoutTraitType>(M, N));
    auto dst = make_tensor_at<location::l1>(dst_data, make_frame_layout<nz_layout_ptn, LayoutTraitType>(M, N));
    auto quant = make_tensor_at<location::l1>(quant_data, make_frame_layout<nd_ext_layout_ptn, uint64_t>(_1{}, N));

    expect_copy_trap<QuantVariant::SCALAR, copy_l0c_to_l1, l0c_to_l1_trait_default>(dst, src, quant);
}

void expect_l1_src_stride_limit_trap()
{
    constexpr int BASE_M = 65536;
    constexpr int M = 16;
    constexpr int N = 16;
    using LayoutTraitType = layout_trait_default<float, _16>;
    alignas(512) static __cc__ float src_data[BASE_M * N] = {};
    alignas(512) static __cbuf__ float dst_data[M * N] = {};
    alignas(512) static __cbuf__ uint64_t quant_data[N] = {};
    auto src_base =
        make_tensor_at<location::l0c>(src_data, make_frame_layout<nz_layout_ptn, LayoutTraitType>(BASE_M, N));
    auto src = slice(src_base, make_coord(_0{}, _0{}), make_shape(M, N));
    auto dst = make_tensor_at<location::l1>(dst_data, make_frame_layout<nz_layout_ptn, LayoutTraitType>(M, N));
    auto quant = make_tensor_at<location::l1>(quant_data, make_frame_layout<nd_ext_layout_ptn, uint64_t>(_1{}, N));

    expect_copy_trap<QuantVariant::TENSOR, copy_l0c_to_l1, l0c_to_l1_trait_default>(dst, src, quant);
}

void expect_l1_zero_dst_stride_trap()
{
    constexpr int M = 16;
    constexpr int N = 16;
    using LayoutTraitType = layout_trait_default<float, _16>;
    alignas(512) static __cc__ float src_data[M * N] = {};
    alignas(512) static __cbuf__ float dst_data[M * N] = {};
    alignas(512) static __cbuf__ uint64_t quant_data[N] = {};
    auto src = make_tensor_at<location::l0c>(src_data, make_frame_layout<nz_layout_ptn, LayoutTraitType>(M, N));
    auto dst = make_tensor_at<location::l1>(dst_data, make_zero_nz_column_stride_layout<float, LayoutTraitType>(M, N));
    auto quant = make_tensor_at<location::l1>(quant_data, make_frame_layout<nd_ext_layout_ptn, uint64_t>(_1{}, N));

    expect_copy_trap<QuantVariant::NONE, copy_l0c_to_l1, l0c_to_l1_trait_default>(dst, src, quant);
}

void expect_l1_invalid_unit_flag_trap()
{
    constexpr int M = 16;
    constexpr int N = 16;
    using LayoutTraitType = layout_trait_default<float, _16>;
    alignas(512) static __cc__ float src_data[M * N] = {};
    alignas(512) static __cbuf__ float dst_data[M * N] = {};
    alignas(512) static __cbuf__ uint64_t quant_data[N] = {};
    auto src = make_tensor_at<location::l0c>(src_data, make_frame_layout<nz_layout_ptn, LayoutTraitType>(M, N));
    auto dst = make_tensor_at<location::l1>(dst_data, make_frame_layout<nz_layout_ptn, LayoutTraitType>(M, N));
    auto quant = make_tensor_at<location::l1>(quant_data, make_frame_layout<nd_ext_layout_ptn, uint64_t>(_1{}, N));
    l0c_to_l1_params params{static_cast<unit_flag_mode>(1)};

    expect_copy_trap<QuantVariant::NONE, copy_l0c_to_l1, l0c_to_l1_trait_default>(dst, src, quant, params);
}

template <typename DstLayoutPattern, QuantVariant variant>
void expect_gm_matrix_capacity_trap()
{
    constexpr uint32_t SRC_M = 32;
    constexpr uint32_t SRC_N = 32;
    constexpr uint32_t DST_M = 16;
    constexpr uint32_t DST_N = 32;
    alignas(512) static __cc__ float src_data[SRC_M * SRC_N] = {};
    alignas(512) static __gm__ float dst_data[DST_M * DST_N] = {};
    alignas(512) static __cbuf__ uint64_t quant_data[SRC_N] = {};

    auto src = make_tensor_at<location::l0c>(
        src_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(SRC_M, SRC_N));
    auto dst = make_tensor_at<location::gm>(dst_data, make_frame_layout<DstLayoutPattern, float>(DST_M, DST_N));
    auto quant = make_tensor_at<location::l1>(quant_data, make_frame_layout<nd_ext_layout_ptn, uint64_t>(_1{}, SRC_N));

    expect_copy_trap<variant, copy_l0c_to_gm, l0c_to_gm_trait_default>(dst, src, quant);
}

template <typename DstLayoutPattern, QuantVariant variant>
void expect_ub_matrix_capacity_trap()
{
    constexpr uint32_t SRC_M = 32;
    constexpr uint32_t SRC_N = 32;
    constexpr uint32_t DST_M = 16;
    constexpr uint32_t DST_N = 32;
    alignas(512) static __cc__ float src_data[SRC_M * SRC_N] = {};
    alignas(512) static __ubuf__ float dst_data[DST_M * DST_N] = {};
    alignas(512) static __cbuf__ uint64_t quant_data[SRC_N] = {};

    auto src = make_tensor_at<location::l0c>(
        src_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(SRC_M, SRC_N));
    auto dst = make_tensor_at<location::ub>(dst_data, make_frame_layout<DstLayoutPattern, float>(DST_M, DST_N));
    auto quant = make_tensor_at<location::l1>(quant_data, make_frame_layout<nd_ext_layout_ptn, uint64_t>(_1{}, SRC_N));

    expect_copy_trap<variant, copy_l0c_to_ub, l0c_to_ub_trait_default>(dst, src, quant);
}

template <QuantVariant variant>
void expect_gm_nc1hwc0_capacity_trap()
{
    constexpr uint32_t SRC_M = 32;
    constexpr uint32_t SRC_N = 32;
    constexpr uint32_t DST_C0 = 16;
    constexpr uint32_t DST_SIZE = 16 * DST_C0;
    alignas(512) static __cc__ float src_data[SRC_M * SRC_N] = {};
    alignas(512) static __gm__ float dst_data[DST_SIZE] = {};
    alignas(512) static __cbuf__ uint64_t quant_data[SRC_N] = {};

    auto src = make_tensor_at<location::l0c>(
        src_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(SRC_M, SRC_N));
    auto dst = make_tensor_at<location::gm>(
        dst_data, make_frame_layout<nc1hwc0_layout_ptn, float>(_1{}, _1{}, _1{}, _16{}, DST_C0));
    auto quant = make_tensor_at<location::l1>(quant_data, make_frame_layout<nd_ext_layout_ptn, uint64_t>(_1{}, SRC_N));

    expect_copy_trap<variant, copy_l0c_to_gm, l0c_to_gm_trait_default>(dst, src, quant);
}

template <typename DstLayoutPattern>
void expect_gm_conv_capacity_trap()
{
    constexpr uint32_t SRC_M = 32;
    constexpr uint32_t SRC_N = 32;
    constexpr uint32_t DST_SIZE = 16 * 16;
    alignas(512) static __cc__ float src_data[SRC_M * SRC_N] = {};
    alignas(512) static __gm__ float dst_data[DST_SIZE] = {};
    alignas(512) static __cbuf__ uint64_t quant_data[SRC_N] = {};

    auto src = make_tensor_at<location::l0c>(
        src_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(SRC_M, SRC_N));
    auto dst =
        make_tensor_at<location::gm>(dst_data, make_frame_layout<DstLayoutPattern, float>(_1{}, _1{}, _16{}, _16{}));
    auto quant = make_tensor_at<location::l1>(quant_data, make_frame_layout<nd_ext_layout_ptn, uint64_t>(_1{}, SRC_N));

    expect_copy_trap<QuantVariant::NONE, copy_l0c_to_gm, l0c_to_gm_trait_default>(dst, src, quant);
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2GMNDRejectsInsufficientCapacityForAllQuantVariants)
{
    expect_gm_matrix_capacity_trap<nd_layout_ptn, QuantVariant::NONE>();
    expect_gm_matrix_capacity_trap<nd_layout_ptn, QuantVariant::SCALAR>();
    expect_gm_matrix_capacity_trap<nd_layout_ptn, QuantVariant::TENSOR>();
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2GMNDExtRejectsInsufficientCapacityForAllQuantVariants)
{
    expect_gm_matrix_capacity_trap<nd_ext_layout_ptn, QuantVariant::NONE>();
    expect_gm_matrix_capacity_trap<nd_ext_layout_ptn, QuantVariant::SCALAR>();
    expect_gm_matrix_capacity_trap<nd_ext_layout_ptn, QuantVariant::TENSOR>();
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2GMDNRejectsInsufficientCapacityForAllQuantVariants)
{
    expect_gm_matrix_capacity_trap<dn_layout_ptn, QuantVariant::NONE>();
    expect_gm_matrix_capacity_trap<dn_layout_ptn, QuantVariant::SCALAR>();
    expect_gm_matrix_capacity_trap<dn_layout_ptn, QuantVariant::TENSOR>();
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2GMDNExtRejectsInsufficientCapacityForAllQuantVariants)
{
    expect_gm_matrix_capacity_trap<dn_ext_layout_ptn, QuantVariant::NONE>();
    expect_gm_matrix_capacity_trap<dn_ext_layout_ptn, QuantVariant::SCALAR>();
    expect_gm_matrix_capacity_trap<dn_ext_layout_ptn, QuantVariant::TENSOR>();
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2GMNZRejectsInsufficientCapacityForAllQuantVariants)
{
    expect_gm_matrix_capacity_trap<nz_layout_ptn, QuantVariant::NONE>();
    expect_gm_matrix_capacity_trap<nz_layout_ptn, QuantVariant::SCALAR>();
    expect_gm_matrix_capacity_trap<nz_layout_ptn, QuantVariant::TENSOR>();
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2GMNC1HWC0RejectsInsufficientCapacityForSupportedVariants)
{
    expect_gm_nc1hwc0_capacity_trap<QuantVariant::NONE>();
    expect_gm_nc1hwc0_capacity_trap<QuantVariant::SCALAR>();
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2GMNHWCRejectsInsufficientCapacity)
{
    expect_gm_conv_capacity_trap<nhwc_layout_ptn>();
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2GMNCHWRejectsInsufficientCapacity)
{
    expect_gm_conv_capacity_trap<nchw_layout_ptn>();
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2UBNDRejectsInsufficientCapacityForAllQuantVariants)
{
    expect_ub_matrix_capacity_trap<nd_layout_ptn, QuantVariant::NONE>();
    expect_ub_matrix_capacity_trap<nd_layout_ptn, QuantVariant::SCALAR>();
    expect_ub_matrix_capacity_trap<nd_layout_ptn, QuantVariant::TENSOR>();
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2UBNDExtRejectsInsufficientCapacityForAllQuantVariants)
{
    expect_ub_matrix_capacity_trap<nd_ext_layout_ptn, QuantVariant::NONE>();
    expect_ub_matrix_capacity_trap<nd_ext_layout_ptn, QuantVariant::SCALAR>();
    expect_ub_matrix_capacity_trap<nd_ext_layout_ptn, QuantVariant::TENSOR>();
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2UBDNRejectsInsufficientCapacityForAllQuantVariants)
{
    expect_ub_matrix_capacity_trap<dn_layout_ptn, QuantVariant::NONE>();
    expect_ub_matrix_capacity_trap<dn_layout_ptn, QuantVariant::SCALAR>();
    expect_ub_matrix_capacity_trap<dn_layout_ptn, QuantVariant::TENSOR>();
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2UBDNExtRejectsInsufficientCapacityForAllQuantVariants)
{
    expect_ub_matrix_capacity_trap<dn_ext_layout_ptn, QuantVariant::NONE>();
    expect_ub_matrix_capacity_trap<dn_ext_layout_ptn, QuantVariant::SCALAR>();
    expect_ub_matrix_capacity_trap<dn_ext_layout_ptn, QuantVariant::TENSOR>();
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2UBNZRejectsInsufficientCapacityForAllQuantVariants)
{
    expect_ub_matrix_capacity_trap<nz_layout_ptn, QuantVariant::NONE>();
    expect_ub_matrix_capacity_trap<nz_layout_ptn, QuantVariant::SCALAR>();
    expect_ub_matrix_capacity_trap<nz_layout_ptn, QuantVariant::TENSOR>();
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2GMBatchRejectsInsufficientCapacityForEveryFormat)
{
    expect_gm_batch_capacity_trap<nd_layout_ptn, BatchQuantVariant::NONE>();
    expect_gm_batch_capacity_trap<nd_layout_ptn, BatchQuantVariant::SCALAR>();
    expect_gm_batch_capacity_trap<nd_layout_ptn, BatchQuantVariant::SHARED_TENSOR>();
    expect_gm_batch_capacity_trap<nd_layout_ptn, BatchQuantVariant::BATCHED_TENSOR>();
    expect_gm_batch_capacity_trap<nd_ext_layout_ptn, BatchQuantVariant::NONE>();
    expect_gm_batch_capacity_trap<nd_ext_layout_ptn, BatchQuantVariant::SCALAR>();
    expect_gm_batch_capacity_trap<nd_ext_layout_ptn, BatchQuantVariant::SHARED_TENSOR>();
    expect_gm_batch_capacity_trap<nd_ext_layout_ptn, BatchQuantVariant::BATCHED_TENSOR>();
    expect_gm_batch_capacity_trap<dn_layout_ptn, BatchQuantVariant::NONE>();
    expect_gm_batch_capacity_trap<dn_layout_ptn, BatchQuantVariant::SCALAR>();
    expect_gm_batch_capacity_trap<dn_layout_ptn, BatchQuantVariant::SHARED_TENSOR>();
    expect_gm_batch_capacity_trap<dn_layout_ptn, BatchQuantVariant::BATCHED_TENSOR>();
    expect_gm_batch_capacity_trap<dn_ext_layout_ptn, BatchQuantVariant::NONE>();
    expect_gm_batch_capacity_trap<dn_ext_layout_ptn, BatchQuantVariant::SCALAR>();
    expect_gm_batch_capacity_trap<dn_ext_layout_ptn, BatchQuantVariant::SHARED_TENSOR>();
    expect_gm_batch_capacity_trap<dn_ext_layout_ptn, BatchQuantVariant::BATCHED_TENSOR>();
    expect_gm_batch_capacity_trap<nz_layout_ptn, BatchQuantVariant::NONE>();
    expect_gm_batch_capacity_trap<nz_layout_ptn, BatchQuantVariant::SCALAR>();
    expect_gm_batch_capacity_trap<nz_layout_ptn, BatchQuantVariant::SHARED_TENSOR>();
    expect_gm_batch_capacity_trap<nz_layout_ptn, BatchQuantVariant::BATCHED_TENSOR>();
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2UBBatchRejectsInsufficientCapacityForEveryFormat)
{
    expect_ub_batch_capacity_trap<nd_layout_ptn, BatchQuantVariant::NONE>();
    expect_ub_batch_capacity_trap<nd_layout_ptn, BatchQuantVariant::SCALAR>();
    expect_ub_batch_capacity_trap<nd_layout_ptn, BatchQuantVariant::SHARED_TENSOR>();
    expect_ub_batch_capacity_trap<nd_layout_ptn, BatchQuantVariant::BATCHED_TENSOR>();
    expect_ub_batch_capacity_trap<nd_ext_layout_ptn, BatchQuantVariant::NONE>();
    expect_ub_batch_capacity_trap<nd_ext_layout_ptn, BatchQuantVariant::SCALAR>();
    expect_ub_batch_capacity_trap<nd_ext_layout_ptn, BatchQuantVariant::SHARED_TENSOR>();
    expect_ub_batch_capacity_trap<nd_ext_layout_ptn, BatchQuantVariant::BATCHED_TENSOR>();
    expect_ub_batch_capacity_trap<dn_layout_ptn, BatchQuantVariant::NONE>();
    expect_ub_batch_capacity_trap<dn_layout_ptn, BatchQuantVariant::SCALAR>();
    expect_ub_batch_capacity_trap<dn_layout_ptn, BatchQuantVariant::SHARED_TENSOR>();
    expect_ub_batch_capacity_trap<dn_layout_ptn, BatchQuantVariant::BATCHED_TENSOR>();
    expect_ub_batch_capacity_trap<dn_ext_layout_ptn, BatchQuantVariant::NONE>();
    expect_ub_batch_capacity_trap<dn_ext_layout_ptn, BatchQuantVariant::SCALAR>();
    expect_ub_batch_capacity_trap<dn_ext_layout_ptn, BatchQuantVariant::SHARED_TENSOR>();
    expect_ub_batch_capacity_trap<dn_ext_layout_ptn, BatchQuantVariant::BATCHED_TENSOR>();
    expect_ub_batch_capacity_trap<nz_layout_ptn, BatchQuantVariant::NONE>();
    expect_ub_batch_capacity_trap<nz_layout_ptn, BatchQuantVariant::SCALAR>();
    expect_ub_batch_capacity_trap<nz_layout_ptn, BatchQuantVariant::SHARED_TENSOR>();
    expect_ub_batch_capacity_trap<nz_layout_ptn, BatchQuantVariant::BATCHED_TENSOR>();
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2GMOverloadsRejectInvalidTensorRoles)
{
    expect_gm_invalid_roles_for_overload<QuantVariant::NONE>();
    expect_gm_invalid_roles_for_overload<QuantVariant::SCALAR>();
    expect_gm_invalid_roles_for_overload<QuantVariant::TENSOR>();
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2UBOverloadsRejectInvalidTensorRoles)
{
    expect_ub_invalid_roles_for_overload<QuantVariant::NONE>();
    expect_ub_invalid_roles_for_overload<QuantVariant::SCALAR>();
    expect_ub_invalid_roles_for_overload<QuantVariant::TENSOR>();
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2GMRejectsZeroDestinationStrideForPublicNDLayouts)
{
    expect_gm_zero_dst_stride_trap<nd_layout_ptn>();
    expect_gm_zero_dst_stride_trap<nd_ext_layout_ptn>();
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2UBRejectsZeroDestinationStrideForPublicNDLayouts)
{
    expect_ub_zero_dst_stride_trap<nd_layout_ptn>();
    expect_ub_zero_dst_stride_trap<nd_ext_layout_ptn>();
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2L1OverloadsRejectInsufficientCapacity)
{
    expect_l1_matrix_capacity_trap<QuantVariant::NONE>();
    expect_l1_matrix_capacity_trap<QuantVariant::SCALAR>();
    expect_l1_matrix_capacity_trap<QuantVariant::TENSOR>();
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2L1OverloadsRejectInvalidTensorRoles)
{
    expect_l1_invalid_roles_for_overload<QuantVariant::NONE>();
    expect_l1_invalid_roles_for_overload<QuantVariant::SCALAR>();
    expect_l1_invalid_roles_for_overload<QuantVariant::TENSOR>();
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2L1RejectsPublicNumericBoundaries)
{
    expect_l1_n_limit_trap();
    expect_l1_m_limit_trap();
    expect_l1_src_stride_limit_trap();
    expect_l1_zero_dst_stride_trap();
    expect_l1_invalid_unit_flag_trap();
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2GMRejectsNAboveInstructionLimitForDirectQuantForms)
{
    constexpr uint32_t M = 16;
    constexpr uint32_t N = 4096;
    alignas(512) static __cc__ float src_data[M * N] = {};
    alignas(512) static __gm__ float dst_data[M * N] = {};
    alignas(512) static __cbuf__ uint64_t quant_data[N] = {};

    auto src = make_tensor_at<location::l0c>(
        src_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(M, N));
    auto dst = make_tensor_at<location::gm>(dst_data, make_frame_layout<nd_ext_layout_ptn, float>(M, N));
    auto quant = make_tensor_at<location::l1>(quant_data, make_frame_layout<nd_ext_layout_ptn, uint64_t>(_1{}, N));

    expect_copy_trap<QuantVariant::NONE, copy_l0c_to_gm, l0c_to_gm_trait_default>(dst, src, quant);
    expect_copy_trap<QuantVariant::SCALAR, copy_l0c_to_gm, l0c_to_gm_trait_default>(dst, src, quant);
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2UBRejectsNAboveInstructionLimitForDirectQuantForms)
{
    constexpr uint32_t M = 16;
    constexpr uint32_t N = 4096;
    alignas(512) static __cc__ float src_data[M * N] = {};
    alignas(512) static __ubuf__ float dst_data[M * N] = {};
    alignas(512) static __cbuf__ uint64_t quant_data[N] = {};

    auto src = make_tensor_at<location::l0c>(
        src_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(M, N));
    auto dst = make_tensor_at<location::ub>(dst_data, make_frame_layout<dn_ext_layout_ptn, float>(M, N));
    auto quant = make_tensor_at<location::l1>(quant_data, make_frame_layout<nd_ext_layout_ptn, uint64_t>(_1{}, N));

    expect_copy_trap<QuantVariant::NONE, copy_l0c_to_ub, l0c_to_ub_trait_default>(dst, src, quant);
    expect_copy_trap<QuantVariant::SCALAR, copy_l0c_to_ub, l0c_to_ub_trait_default>(dst, src, quant);
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2GMRejectsNDMAboveLimitWithTensorQuant)
{
    constexpr uint32_t M = 8208;
    constexpr uint32_t N = 16;
    alignas(512) static __cc__ float src_data[M * N] = {};
    alignas(512) static __gm__ float dst_data[M * N] = {};
    alignas(512) static __cbuf__ uint64_t quant_data[N] = {};

    auto src = make_tensor_at<location::l0c>(
        src_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(M, N));
    auto dst = make_tensor_at<location::gm>(dst_data, make_frame_layout<nd_ext_layout_ptn, float>(M, N));
    auto quant = make_tensor_at<location::l1>(quant_data, make_frame_layout<nd_ext_layout_ptn, uint64_t>(_1{}, N));

    expect_copy_trap<QuantVariant::TENSOR, copy_l0c_to_gm, l0c_to_gm_trait_default>(dst, src, quant);
    expect_copy_trap<QuantVariant::SCALAR, copy_l0c_to_gm, l0c_to_gm_trait_default>(dst, src, quant);
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2UBRejectsNDMAboveLimitWithoutQuant)
{
    constexpr uint32_t M = 8208;
    constexpr uint32_t N = 16;
    alignas(512) static __cc__ float src_data[M * N] = {};
    alignas(512) static __ubuf__ float dst_data[M * N] = {};
    alignas(512) static __cbuf__ uint64_t quant_data[N] = {};

    auto src = make_tensor_at<location::l0c>(
        src_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(M, N));
    auto dst = make_tensor_at<location::ub>(dst_data, make_frame_layout<nd_layout_ptn, float>(M, N));
    auto quant = make_tensor_at<location::l1>(quant_data, make_frame_layout<nd_ext_layout_ptn, uint64_t>(_1{}, N));

    expect_copy_trap<QuantVariant::NONE, copy_l0c_to_ub, l0c_to_ub_trait_default>(dst, src, quant);
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2GMRejectsNZMAboveLimitWithScalarQuant)
{
    constexpr uint32_t M = 65536;
    constexpr uint32_t N = 16;
    alignas(512) static __cc__ float src_data[M * N] = {};
    alignas(512) static __gm__ float dst_data[M * N] = {};
    alignas(512) static __cbuf__ uint64_t quant_data[N] = {};

    auto src = make_tensor_at<location::l0c>(
        src_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(M, N));
    auto dst = make_tensor_at<location::gm>(
        dst_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(M, N));
    auto quant = make_tensor_at<location::l1>(quant_data, make_frame_layout<nd_ext_layout_ptn, uint64_t>(_1{}, N));

    expect_copy_trap<QuantVariant::SCALAR, copy_l0c_to_gm, l0c_to_gm_trait_default>(dst, src, quant);
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2UBRejectsNZMAboveLimitWithTensorQuant)
{
    constexpr uint32_t M = 65536;
    constexpr uint32_t N = 16;
    alignas(512) static __cc__ float src_data[M * N] = {};
    alignas(512) static __ubuf__ float dst_data[M * N] = {};
    alignas(512) static __cbuf__ uint64_t quant_data[N] = {};

    auto src = make_tensor_at<location::l0c>(
        src_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(M, N));
    auto dst = make_tensor_at<location::ub>(
        dst_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(M, N));
    auto quant = make_tensor_at<location::l1>(quant_data, make_frame_layout<nd_ext_layout_ptn, uint64_t>(_1{}, N));

    expect_copy_trap<QuantVariant::TENSOR, copy_l0c_to_ub, l0c_to_ub_trait_default>(dst, src, quant);
    expect_copy_trap<QuantVariant::SCALAR, copy_l0c_to_ub, l0c_to_ub_trait_default>(dst, src, quant);
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2GMRejectsNZNNotDivisibleBy16)
{
    constexpr uint32_t SRC_M = 16;
    constexpr uint32_t SRC_N = 16;
    constexpr uint32_t DST_M = 32;
    constexpr uint32_t DST_N = 8;
    alignas(512) __cc__ float src_data[SRC_M * SRC_N] = {};
    alignas(512) __gm__ float dst_data[DST_M * DST_N] = {};
    alignas(512) __cbuf__ uint64_t quant_data[DST_N] = {};

    auto src = make_tensor_at<location::l0c>(
        src_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(SRC_M, SRC_N));
    auto dst = make_tensor_at<location::gm>(dst_data, make_frame_layout<nz_layout_ptn, float>(DST_M, DST_N));
    auto quant = make_tensor_at<location::l1>(quant_data, make_frame_layout<nd_ext_layout_ptn, uint64_t>(_1{}, DST_N));

    expect_copy_trap<QuantVariant::NONE, copy_l0c_to_gm, l0c_to_gm_trait_default>(dst, src, quant);
    expect_copy_trap<QuantVariant::SCALAR, copy_l0c_to_gm, l0c_to_gm_trait_default>(dst, src, quant);
    expect_copy_trap<QuantVariant::TENSOR, copy_l0c_to_gm, l0c_to_gm_trait_default>(dst, src, quant);
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2UBRejectsNZNNotDivisibleBy16)
{
    constexpr uint32_t SRC_M = 16;
    constexpr uint32_t SRC_N = 16;
    constexpr uint32_t DST_M = 32;
    constexpr uint32_t DST_N = 8;
    alignas(512) __cc__ float src_data[SRC_M * SRC_N] = {};
    alignas(512) __ubuf__ float dst_data[DST_M * DST_N] = {};
    alignas(512) __cbuf__ uint64_t quant_data[DST_N] = {};

    auto src = make_tensor_at<location::l0c>(
        src_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(SRC_M, SRC_N));
    auto dst = make_tensor_at<location::ub>(dst_data, make_frame_layout<nz_layout_ptn, float>(DST_M, DST_N));
    auto quant = make_tensor_at<location::l1>(quant_data, make_frame_layout<nd_ext_layout_ptn, uint64_t>(_1{}, DST_N));

    expect_copy_trap<QuantVariant::NONE, copy_l0c_to_ub, l0c_to_ub_trait_default>(dst, src, quant);
    expect_copy_trap<QuantVariant::SCALAR, copy_l0c_to_ub, l0c_to_ub_trait_default>(dst, src, quant);
    expect_copy_trap<QuantVariant::TENSOR, copy_l0c_to_ub, l0c_to_ub_trait_default>(dst, src, quant);
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2GMRejectsSourceStrideAboveLimit)
{
    constexpr uint32_t BASE_M = 65536;
    constexpr uint32_t M = 16;
    constexpr uint32_t N = 16;
    alignas(512) static __cc__ float src_data[BASE_M * N] = {};
    alignas(512) static __gm__ float dst_data[M * N] = {};
    alignas(512) static __cbuf__ uint64_t quant_data[N] = {};

    auto src_base = make_tensor_at<location::l0c>(
        src_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(BASE_M, N));
    auto src = slice(src_base, make_coord(_0{}, _0{}), make_shape(M, N));
    auto dst = make_tensor_at<location::gm>(dst_data, make_frame_layout<nd_ext_layout_ptn, float>(M, N));
    auto quant = make_tensor_at<location::l1>(quant_data, make_frame_layout<nd_ext_layout_ptn, uint64_t>(_1{}, N));

    expect_copy_trap<QuantVariant::SCALAR, copy_l0c_to_gm, l0c_to_gm_trait_default>(dst, src, quant);
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2UBRejectsSourceStrideAboveLimit)
{
    constexpr uint32_t BASE_M = 65536;
    constexpr uint32_t M = 16;
    constexpr uint32_t N = 16;
    alignas(512) static __cc__ float src_data[BASE_M * N] = {};
    alignas(512) static __ubuf__ float dst_data[M * N] = {};
    alignas(512) static __cbuf__ uint64_t quant_data[N] = {};

    auto src_base = make_tensor_at<location::l0c>(
        src_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(BASE_M, N));
    auto src = slice(src_base, make_coord(_0{}, _0{}), make_shape(M, N));
    auto dst = make_tensor_at<location::ub>(dst_data, make_frame_layout<nd_ext_layout_ptn, float>(M, N));
    auto quant = make_tensor_at<location::l1>(quant_data, make_frame_layout<nd_ext_layout_ptn, uint64_t>(_1{}, N));

    expect_copy_trap<QuantVariant::TENSOR, copy_l0c_to_ub, l0c_to_ub_trait_default>(dst, src, quant);
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2GMQuantOverloadsRejectInvalidUnitFlag)
{
    constexpr uint32_t M = 16;
    constexpr uint32_t N = 16;
    alignas(512) __cc__ float src_data[M * N] = {};
    alignas(512) __gm__ float dst_data[M * N] = {};
    alignas(512) __cbuf__ uint64_t quant_data[N] = {};
    l0c_to_gm_params params{static_cast<unit_flag_mode>(1)};

    auto src = make_tensor_at<location::l0c>(
        src_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(M, N));
    auto dst = make_tensor_at<location::gm>(dst_data, make_frame_layout<nd_ext_layout_ptn, float>(M, N));
    auto quant = make_tensor_at<location::l1>(quant_data, make_frame_layout<nd_ext_layout_ptn, uint64_t>(_1{}, N));

    expect_copy_trap<QuantVariant::SCALAR, copy_l0c_to_gm, l0c_to_gm_trait_default>(dst, src, quant, params);
    expect_copy_trap<QuantVariant::TENSOR, copy_l0c_to_gm, l0c_to_gm_trait_default>(dst, src, quant, params);
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, CopyL0C2UBQuantOverloadsRejectInvalidUnitFlag)
{
    constexpr uint32_t M = 16;
    constexpr uint32_t N = 16;
    alignas(512) __cc__ float src_data[M * N] = {};
    alignas(512) __ubuf__ float dst_data[M * N] = {};
    alignas(512) __cbuf__ uint64_t quant_data[N] = {};
    l0c_to_ub_params params{static_cast<unit_flag_mode>(1)};

    auto src = make_tensor_at<location::l0c>(
        src_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(M, N));
    auto dst = make_tensor_at<location::ub>(dst_data, make_frame_layout<nd_ext_layout_ptn, float>(M, N));
    auto quant = make_tensor_at<location::l1>(quant_data, make_frame_layout<nd_ext_layout_ptn, uint64_t>(_1{}, N));

    expect_copy_trap<QuantVariant::SCALAR, copy_l0c_to_ub, l0c_to_ub_trait_default>(dst, src, quant, params);
    expect_copy_trap<QuantVariant::TENSOR, copy_l0c_to_ub, l0c_to_ub_trait_default>(dst, src, quant, params);
}

constexpr mmad_trait GEMV_ENABLED_MMAD_TRAIT = {0, false, false, false, mmad_type::normal};

constexpr mmad_trait MX_MMAD_TRAIT = {0, false, false, true, mmad_type::mx};

struct GemvEnabledMmadTrait {
    using trait_type = mmad_trait;
    static constexpr const trait_type value = GEMV_ENABLED_MMAD_TRAIT;
};

struct MxMmadTrait {
    using trait_type = mmad_trait;
    static constexpr const trait_type value = MX_MMAD_TRAIT;
};

template <typename BiasLocationType, typename L0CPointerType, typename BiasPointerType>
auto make_valid_mmad_bias(L0CPointerType l0c_pointer, BiasPointerType bias_pointer, int m, int n)
{
    if constexpr (asc::te::Std::is_same_v<BiasLocationType, location::l0c>) {
        return make_tensor_at<location::l0c>(
            l0c_pointer, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(m, n));
    } else {
        return make_tensor_at<location::bias>(bias_pointer, make_frame_layout<nd_layout_ptn, float>(m, n));
    }
}

template <typename BiasLocationType, InvalidLayoutKind kind, typename L0CPointerType, typename BiasPointerType>
auto make_invalid_mmad_bias(L0CPointerType l0c_pointer, BiasPointerType bias_pointer)
{
    if constexpr (asc::te::Std::is_same_v<BiasLocationType, location::l0c>) {
        return make_tensor_directly<location::l0c>(
            l0c_pointer, make_invalid_nz_layout<float, kind, layout_trait_default<float, _16>>());
    } else {
        return make_tensor_directly<location::bias>(bias_pointer, make_invalid_nd_layout<float, kind>());
    }
}

template <typename BiasLocationType, typename MmadTraitType>
void run_mmad_with_bias_at(const mmad_params& params, const MmadTraitType& trait)
{
    constexpr size_t BUFFER_ELEMENTS = 4096 * 16;
    alignas(512) static __ca__ half fm_data[BUFFER_ELEMENTS] = {};
    alignas(512) static __cb__ half filter_data[BUFFER_ELEMENTS] = {};
    alignas(512) static __cc__ float dst_data[BUFFER_ELEMENTS] = {};
    alignas(512) static __cc__ float l0c_bias_data[BUFFER_ELEMENTS] = {};
    alignas(512) static __biasbuf__ float bias_data[BUFFER_ELEMENTS] = {};

    auto fm = make_tensor_at<location::l0a>(fm_data, make_frame_layout<nz_layout_ptn, half>(params.m, params.k));
    auto filter =
        make_tensor_at<location::l0b>(filter_data, make_frame_layout<zn_layout_ptn, half>(params.k, params.n));
    auto dst = make_tensor_at<location::l0c>(
        dst_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(params.m, params.n));
    auto bias = make_valid_mmad_bias<BiasLocationType>(l0c_bias_data, bias_data, params.m, params.n);
    auto atom = make_mmad(mmad_operation{}, trait).with(params);

    mmad(atom, dst, fm, filter, bias);
}

void run_normal_mmad_with_default_atom(const mmad_params& params)
{
    constexpr size_t BUFFER_ELEMENTS = 16 * 16;
    alignas(512) static __ca__ half fm_data[BUFFER_ELEMENTS] = {};
    alignas(512) static __cb__ half filter_data[BUFFER_ELEMENTS] = {};
    alignas(512) static __cc__ float dst_data[BUFFER_ELEMENTS] = {};

    auto fm = make_tensor_at<location::l0a>(fm_data, make_frame_layout<nz_layout_ptn, half>(16, 16));
    auto filter = make_tensor_at<location::l0b>(filter_data, make_frame_layout<zn_layout_ptn, half>(16, 16));
    auto dst = make_tensor_at<location::l0c>(
        dst_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(16, 16));
    auto atom = make_mmad(mmad_operation{}).with(params);

    mmad(atom, dst, fm, filter);
}

void run_mx_mmad(const mmad_params& params)
{
    constexpr size_t BUFFER_ELEMENTS = 4096 * 16;
    alignas(512) static __ca__ fp8_e4m3fn_t fm_data[BUFFER_ELEMENTS] = {};
    alignas(512) static __cb__ fp8_e4m3fn_t filter_data[BUFFER_ELEMENTS] = {};
    alignas(512) static __cc__ float dst_data[BUFFER_ELEMENTS] = {};

    auto fm =
        make_tensor_at<location::l0a>(fm_data, make_frame_layout<nz_layout_ptn, fp8_e4m3fn_t>(params.m, params.k));
    auto filter =
        make_tensor_at<location::l0b>(filter_data, make_frame_layout<zn_layout_ptn, fp8_e4m3fn_t>(params.k, params.n));
    auto dst = make_tensor_at<location::l0c>(
        dst_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(params.m, params.n));
    auto atom = make_mmad(mmad_operation{}, MxMmadTrait{}).with(params);

    mmad(atom, dst, fm, filter);
}

template <typename InputType, typename MmadTraitType, InvalidTensorRole role, InvalidLayoutKind kind>
void expect_mmad_no_bias_invalid_role_trap(const MmadTraitType& trait)
{
    constexpr int M = 16;
    constexpr int N = 16;
    constexpr int K = 16;
    alignas(512) static __ca__ InputType fm_data[M * K] = {};
    alignas(512) static __cb__ InputType filter_data[K * N] = {};
    alignas(512) static __cc__ float dst_data[M * N] = {};
    mmad_params params{M, N, K, unit_flag_mode::disable, true};
    auto atom = make_mmad(mmad_operation{}, trait).with(params);

    if constexpr (role == InvalidTensorRole::DST) {
        auto dst = make_tensor_directly<location::l0c>(
            dst_data, make_invalid_nz_layout<float, kind, layout_trait_default<float, _16>>());
        auto fm = make_tensor_at<location::l0a>(fm_data, make_frame_layout<nz_layout_ptn, InputType>(M, K));
        auto filter = make_tensor_at<location::l0b>(filter_data, make_frame_layout<zn_layout_ptn, InputType>(K, N));
        EXPECT_THROW((mmad(atom, dst, fm, filter)), TrapException);
    } else if constexpr (role == InvalidTensorRole::FM) {
        auto dst = make_tensor_at<location::l0c>(
            dst_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(M, N));
        auto fm = make_tensor_directly<location::l0a>(fm_data, make_invalid_nz_layout<InputType, kind>());
        auto filter = make_tensor_at<location::l0b>(filter_data, make_frame_layout<zn_layout_ptn, InputType>(K, N));
        EXPECT_THROW((mmad(atom, dst, fm, filter)), TrapException);
    } else {
        static_assert(role == InvalidTensorRole::FILTER);
        auto dst = make_tensor_at<location::l0c>(
            dst_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(M, N));
        auto fm = make_tensor_at<location::l0a>(fm_data, make_frame_layout<nz_layout_ptn, InputType>(M, K));
        auto filter = make_tensor_directly<location::l0b>(filter_data, make_invalid_zn_layout<InputType, kind>());
        EXPECT_THROW((mmad(atom, dst, fm, filter)), TrapException);
    }
}

template <typename InputType, typename MmadTraitType>
void expect_mmad_no_bias_invalid_roles(const MmadTraitType& trait)
{
    expect_mmad_no_bias_invalid_role_trap<
        InputType, MmadTraitType, InvalidTensorRole::DST, InvalidLayoutKind::ZERO_SHAPE>(trait);
    expect_mmad_no_bias_invalid_role_trap<
        InputType, MmadTraitType, InvalidTensorRole::DST, InvalidLayoutKind::NEGATIVE_SHAPE>(trait);
    expect_mmad_no_bias_invalid_role_trap<InputType, MmadTraitType, InvalidTensorRole::DST, InvalidLayoutKind::STRIDE>(
        trait);
    expect_mmad_no_bias_invalid_role_trap<
        InputType, MmadTraitType, InvalidTensorRole::FM, InvalidLayoutKind::ZERO_SHAPE>(trait);
    expect_mmad_no_bias_invalid_role_trap<
        InputType, MmadTraitType, InvalidTensorRole::FM, InvalidLayoutKind::NEGATIVE_SHAPE>(trait);
    expect_mmad_no_bias_invalid_role_trap<InputType, MmadTraitType, InvalidTensorRole::FM, InvalidLayoutKind::STRIDE>(
        trait);
    expect_mmad_no_bias_invalid_role_trap<
        InputType, MmadTraitType, InvalidTensorRole::FILTER, InvalidLayoutKind::ZERO_SHAPE>(trait);
    expect_mmad_no_bias_invalid_role_trap<
        InputType, MmadTraitType, InvalidTensorRole::FILTER, InvalidLayoutKind::NEGATIVE_SHAPE>(trait);
    expect_mmad_no_bias_invalid_role_trap<
        InputType, MmadTraitType, InvalidTensorRole::FILTER, InvalidLayoutKind::STRIDE>(trait);
}

template <typename BiasLocationType, InvalidTensorRole role, InvalidLayoutKind kind>
void expect_mmad_with_bias_invalid_role_trap()
{
    constexpr int M = 16;
    constexpr int N = 16;
    constexpr int K = 16;
    alignas(512) static __ca__ half fm_data[M * K] = {};
    alignas(512) static __cb__ half filter_data[K * N] = {};
    alignas(512) static __cc__ float dst_data[M * N] = {};
    alignas(512) static __cc__ float l0c_bias_data[M * N] = {};
    alignas(512) static __biasbuf__ float bias_data[M * N] = {};
    mmad_params params{M, N, K, unit_flag_mode::disable, false};
    auto atom = make_mmad(mmad_operation{}, mmad_trait_default{}).with(params);

    if constexpr (role == InvalidTensorRole::DST) {
        auto dst = make_tensor_directly<location::l0c>(
            dst_data, make_invalid_nz_layout<float, kind, layout_trait_default<float, _16>>());
        auto fm = make_tensor_at<location::l0a>(fm_data, make_frame_layout<nz_layout_ptn, half>(M, K));
        auto filter = make_tensor_at<location::l0b>(filter_data, make_frame_layout<zn_layout_ptn, half>(K, N));
        auto bias = make_valid_mmad_bias<BiasLocationType>(l0c_bias_data, bias_data, M, N);
        EXPECT_THROW((mmad(atom, dst, fm, filter, bias)), TrapException);
    } else if constexpr (role == InvalidTensorRole::FM) {
        auto dst = make_tensor_at<location::l0c>(
            dst_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(M, N));
        auto fm = make_tensor_directly<location::l0a>(fm_data, make_invalid_nz_layout<half, kind>());
        auto filter = make_tensor_at<location::l0b>(filter_data, make_frame_layout<zn_layout_ptn, half>(K, N));
        auto bias = make_valid_mmad_bias<BiasLocationType>(l0c_bias_data, bias_data, M, N);
        EXPECT_THROW((mmad(atom, dst, fm, filter, bias)), TrapException);
    } else if constexpr (role == InvalidTensorRole::FILTER) {
        auto dst = make_tensor_at<location::l0c>(
            dst_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(M, N));
        auto fm = make_tensor_at<location::l0a>(fm_data, make_frame_layout<nz_layout_ptn, half>(M, K));
        auto filter = make_tensor_directly<location::l0b>(filter_data, make_invalid_zn_layout<half, kind>());
        auto bias = make_valid_mmad_bias<BiasLocationType>(l0c_bias_data, bias_data, M, N);
        EXPECT_THROW((mmad(atom, dst, fm, filter, bias)), TrapException);
    } else {
        static_assert(role == InvalidTensorRole::BIAS);
        auto dst = make_tensor_at<location::l0c>(
            dst_data, make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(M, N));
        auto fm = make_tensor_at<location::l0a>(fm_data, make_frame_layout<nz_layout_ptn, half>(M, K));
        auto filter = make_tensor_at<location::l0b>(filter_data, make_frame_layout<zn_layout_ptn, half>(K, N));
        auto bias = make_invalid_mmad_bias<BiasLocationType, kind>(l0c_bias_data, bias_data);
        EXPECT_THROW((mmad(atom, dst, fm, filter, bias)), TrapException);
    }
}

template <typename BiasLocationType>
void expect_mmad_with_bias_invalid_roles()
{
    expect_mmad_with_bias_invalid_role_trap<BiasLocationType, InvalidTensorRole::DST, InvalidLayoutKind::ZERO_SHAPE>();
    expect_mmad_with_bias_invalid_role_trap<
        BiasLocationType, InvalidTensorRole::DST, InvalidLayoutKind::NEGATIVE_SHAPE>();
    expect_mmad_with_bias_invalid_role_trap<BiasLocationType, InvalidTensorRole::DST, InvalidLayoutKind::STRIDE>();
    expect_mmad_with_bias_invalid_role_trap<BiasLocationType, InvalidTensorRole::FM, InvalidLayoutKind::ZERO_SHAPE>();
    expect_mmad_with_bias_invalid_role_trap<
        BiasLocationType, InvalidTensorRole::FM, InvalidLayoutKind::NEGATIVE_SHAPE>();
    expect_mmad_with_bias_invalid_role_trap<BiasLocationType, InvalidTensorRole::FM, InvalidLayoutKind::STRIDE>();
    expect_mmad_with_bias_invalid_role_trap<
        BiasLocationType, InvalidTensorRole::FILTER, InvalidLayoutKind::ZERO_SHAPE>();
    expect_mmad_with_bias_invalid_role_trap<
        BiasLocationType, InvalidTensorRole::FILTER, InvalidLayoutKind::NEGATIVE_SHAPE>();
    expect_mmad_with_bias_invalid_role_trap<BiasLocationType, InvalidTensorRole::FILTER, InvalidLayoutKind::STRIDE>();
    expect_mmad_with_bias_invalid_role_trap<BiasLocationType, InvalidTensorRole::BIAS, InvalidLayoutKind::ZERO_SHAPE>();
    expect_mmad_with_bias_invalid_role_trap<
        BiasLocationType, InvalidTensorRole::BIAS, InvalidLayoutKind::NEGATIVE_SHAPE>();
    expect_mmad_with_bias_invalid_role_trap<BiasLocationType, InvalidTensorRole::BIAS, InvalidLayoutKind::STRIDE>();
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, MmadNormalDefaultAtomRejectsInvalidUnitFlag)
{
    mmad_params params{16, 16, 16, static_cast<unit_flag_mode>(1), true};

    EXPECT_THROW((run_normal_mmad_with_default_atom(params)), TrapException);
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, MmadMxRejectsDimensionsAboveLimit)
{
    mmad_params invalid_m{4096, 16, 16, unit_flag_mode::disable, true};
    mmad_params invalid_n{16, 4096, 16, unit_flag_mode::disable, true};
    mmad_params invalid_k{16, 16, 4096, unit_flag_mode::disable, true};

    EXPECT_THROW((run_mx_mmad(invalid_m)), TrapException);
    EXPECT_THROW((run_mx_mmad(invalid_n)), TrapException);
    EXPECT_THROW((run_mx_mmad(invalid_k)), TrapException);
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, MmadMxRejectsInvalidUnitFlag)
{
    mmad_params params{16, 16, 16, static_cast<unit_flag_mode>(1), true};

    EXPECT_THROW((run_mx_mmad(params)), TrapException);
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, MmadNormalAndMxRejectInvalidOperandLayouts)
{
    expect_mmad_no_bias_invalid_roles<half>(mmad_trait_default{});
    expect_mmad_no_bias_invalid_roles<fp8_e4m3fn_t>(MxMmadTrait{});
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, MmadWithBiasRejectsInvalidOperandLayouts)
{
    expect_mmad_with_bias_invalid_roles<location::bias>();
    expect_mmad_with_bias_invalid_roles<location::l0c>();
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, MmadWithBiasRejectsDimensionAboveLimit)
{
    mmad_params invalid_m{4096, 16, 16, unit_flag_mode::disable, false};
    mmad_params invalid_n{16, 4096, 16, unit_flag_mode::disable, false};
    mmad_params invalid_k{16, 16, 4096, unit_flag_mode::disable, false};

    EXPECT_THROW((run_mmad_with_bias_at<location::bias>(invalid_m, mmad_trait_default{})), TrapException);
    EXPECT_THROW((run_mmad_with_bias_at<location::bias>(invalid_n, mmad_trait_default{})), TrapException);
    EXPECT_THROW((run_mmad_with_bias_at<location::bias>(invalid_k, mmad_trait_default{})), TrapException);
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, MmadWithBiasRejectsInvalidUnitFlag)
{
    mmad_params params{16, 16, 16, static_cast<unit_flag_mode>(1), false};

    EXPECT_THROW((run_mmad_with_bias_at<location::bias>(params, mmad_trait_default{})), TrapException);
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, MmadWithBiasRejectsMatrixMAboveOneInGemvMode)
{
    mmad_params params{16, 16, 16, unit_flag_mode::disable, false};

    EXPECT_THROW((run_mmad_with_bias_at<location::bias>(params, GemvEnabledMmadTrait{})), TrapException);
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, MmadWithL0CBiasRejectsDimensionAboveLimit)
{
    mmad_params invalid_m{4096, 16, 16, unit_flag_mode::disable, false};
    mmad_params invalid_n{16, 4096, 16, unit_flag_mode::disable, false};
    mmad_params invalid_k{16, 16, 4096, unit_flag_mode::disable, false};

    EXPECT_THROW((run_mmad_with_bias_at<location::l0c>(invalid_m, mmad_trait_default{})), TrapException);
    EXPECT_THROW((run_mmad_with_bias_at<location::l0c>(invalid_n, mmad_trait_default{})), TrapException);
    EXPECT_THROW((run_mmad_with_bias_at<location::l0c>(invalid_k, mmad_trait_default{})), TrapException);
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, MmadWithL0CBiasRejectsInvalidUnitFlag)
{
    mmad_params params{16, 16, 16, static_cast<unit_flag_mode>(1), false};

    EXPECT_THROW((run_mmad_with_bias_at<location::l0c>(params, mmad_trait_default{})), TrapException);
}

TEST_F(TensorApiCubeOutAdditionalRuntimeValidation, MmadWithL0CBiasRejectsMatrixMAboveOneInGemvMode)
{
    mmad_params params{16, 16, 16, unit_flag_mode::disable, false};

    EXPECT_THROW((run_mmad_with_bias_at<location::l0c>(params, GemvEnabledMmadTrait{})), TrapException);
}

} // namespace
