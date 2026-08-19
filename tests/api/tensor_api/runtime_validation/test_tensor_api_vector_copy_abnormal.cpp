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

using tensor_api_runtime_validation::TrapException;

constexpr int VALID_ROWS = 16;
constexpr int VALID_COLUMNS = 16;
constexpr int SMALL_ROWS = 16;
constexpr int LARGE_ROWS = 64;
constexpr int ROUTE_COLUMNS = 32;
constexpr int BLOCK_COUNT_LIMIT_PLUS_ONE = 4096;
constexpr int FRACTAL_BLOCK_COUNT_LIMIT_PLUS_ONE = BLOCK_COUNT_LIMIT_PLUS_ONE * 16;
constexpr int HALF_ELEMENTS_FOR_BLOCK_LEN_LIMIT_PLUS_ONE = 1048576;
constexpr int FRACTAL_ELEMENTS_FOR_BLOCK_LEN_LIMIT_PLUS_ONE = 65536;
constexpr int MISALIGNED_STRIDE_ELEMENTS = 17;

class TensorApiVectorCopyAbnormalValidation : public testing::Test {
protected:
    void SetUp() override
    {
        AscendC::SetGCoreType(2);
    }

    void TearDown() override
    {
        AscendC::SetGCoreType(0);
    }
};

template <typename LocationType, typename PointerType, typename LayoutType>
auto make_tensor_at(PointerType pointer, const LayoutType& layout)
{
    return asc::te::make_tensor(asc::te::make_mem_ptr<LocationType>(pointer), layout);
}

template <typename LocationType, typename PointerType, typename LayoutType>
auto make_tensor_directly(PointerType pointer, const LayoutType& layout)
{
    using namespace asc::te;

    auto iterator = make_mem_ptr<LocationType>(pointer);
    using TensorType = decltype(make_tensor(iterator, layout));
    using EngineType = typename TensorType::engine_type;
    return TensorType{EngineType{iterator}, layout};
}

template <typename CopyOperationType, typename DstTensorType, typename SrcTensorType>
void copy_tensor(const DstTensorType& dst, const SrcTensorType& src)
{
    using namespace asc::te;
    copy(make_copy(CopyOperationType{}), dst, src);
}

template <typename ElementType>
void copy_gm2ub_with_padding(int src_columns, int dst_columns, const asc::te::gm_to_ub_params& params)
{
    using namespace asc::te;

    alignas(512) ElementType src_data[128] = {};
    alignas(512) ElementType dst_data[128] = {};
    auto src =
        make_tensor_at<location::gm>(src_data, make_frame_layout<nd_ext_layout_ptn, ElementType>(2, src_columns));
    auto dst =
        make_tensor_at<location::ub>(dst_data, make_frame_layout<nd_ext_layout_ptn, ElementType>(2, dst_columns));

    copy(make_copy(copy_gm_to_ub{}).with(params), dst, src);
}

template <typename CopyOperationType, typename SrcLocationType, typename DstLocationType, typename SrcLayoutType,
          typename DstLayoutType>
void expect_copy_trap(const SrcLayoutType& src_layout, const DstLayoutType& dst_layout)
{
    alignas(512) half src_data[1] = {};
    alignas(512) half dst_data[1] = {};
    auto src = make_tensor_at<SrcLocationType>(src_data, src_layout);
    auto dst = make_tensor_at<DstLocationType>(dst_data, dst_layout);

    EXPECT_THROW((copy_tensor<CopyOperationType>(dst, src)), TrapException);
}

template <typename CopyOperationType, typename SrcLocationType, typename DstLocationType>
void expect_operand_layout_traps()
{
    using namespace asc::te;

    alignas(512) half src_data[1] = {};
    alignas(512) half dst_data[1] = {};
    auto valid_layout = make_frame_layout<nd_ext_layout_ptn, half>(VALID_ROWS, VALID_COLUMNS);
    auto zero_shape_layout = make_frame_layout<nd_ext_layout_ptn, half>(VALID_ROWS, 0);
    using LayoutTraitType = get_layout_trait<decltype(valid_layout)>;
    auto negative_shape_frame = make_frame_layout<nd_ext_layout_ptn, half>(VALID_ROWS, -1);
    auto negative_shape_layout =
        make_pattern_layout<nd_ext_layout_ptn, LayoutTraitType>(negative_shape_frame.shape(), valid_layout.stride());
    auto negative_stride = make_stride(make_stride(_0{}, -1), make_stride(_0{}, _1{}));
    auto negative_stride_layout =
        make_pattern_layout<nd_ext_layout_ptn, LayoutTraitType>(valid_layout.shape(), negative_stride);

    auto valid_src = make_tensor_at<SrcLocationType>(src_data, valid_layout);
    auto valid_dst = make_tensor_at<DstLocationType>(dst_data, valid_layout);
    auto zero_shape_src = make_tensor_directly<SrcLocationType>(src_data, zero_shape_layout);
    auto negative_shape_src = make_tensor_directly<SrcLocationType>(src_data, negative_shape_layout);
    auto negative_stride_src = make_tensor_directly<SrcLocationType>(src_data, negative_stride_layout);
    auto zero_shape_dst = make_tensor_directly<DstLocationType>(dst_data, zero_shape_layout);
    auto negative_shape_dst = make_tensor_directly<DstLocationType>(dst_data, negative_shape_layout);
    auto negative_stride_dst = make_tensor_directly<DstLocationType>(dst_data, negative_stride_layout);

    EXPECT_THROW((copy_tensor<CopyOperationType>(valid_dst, zero_shape_src)), TrapException);
    EXPECT_THROW((copy_tensor<CopyOperationType>(valid_dst, negative_shape_src)), TrapException);
    EXPECT_THROW((copy_tensor<CopyOperationType>(valid_dst, negative_stride_src)), TrapException);
    EXPECT_THROW((copy_tensor<CopyOperationType>(zero_shape_dst, valid_src)), TrapException);
    EXPECT_THROW((copy_tensor<CopyOperationType>(negative_shape_dst, valid_src)), TrapException);
    EXPECT_THROW((copy_tensor<CopyOperationType>(negative_stride_dst, valid_src)), TrapException);
}

template <typename LayoutPattern>
auto make_block_len_boundary_layout()
{
    using namespace asc::te;

    if constexpr (asc::te::Std::is_same_v<LayoutPattern, nd_ext_layout_ptn>) {
        return make_frame_layout<nd_ext_layout_ptn, half>(1, HALF_ELEMENTS_FOR_BLOCK_LEN_LIMIT_PLUS_ONE);
    } else if constexpr (asc::te::Std::is_same_v<LayoutPattern, dn_ext_layout_ptn>) {
        return make_frame_layout<dn_ext_layout_ptn, half>(HALF_ELEMENTS_FOR_BLOCK_LEN_LIMIT_PLUS_ONE, 1);
    } else if constexpr (asc::te::Std::is_same_v<LayoutPattern, nz_layout_ptn>) {
        return make_frame_layout<nz_layout_ptn, half>(FRACTAL_ELEMENTS_FOR_BLOCK_LEN_LIMIT_PLUS_ONE, 16);
    } else {
        static_assert(asc::te::Std::is_same_v<LayoutPattern, zn_layout_ptn>);
        return make_frame_layout<zn_layout_ptn, half>(16, FRACTAL_ELEMENTS_FOR_BLOCK_LEN_LIMIT_PLUS_ONE);
    }
}

template <typename LayoutPattern>
auto make_stride_test_layout()
{
    using namespace asc::te;

    if constexpr (asc::te::Std::is_same_v<LayoutPattern, nd_ext_layout_ptn>) {
        return make_frame_layout<nd_ext_layout_ptn, half>(2, VALID_COLUMNS);
    } else if constexpr (asc::te::Std::is_same_v<LayoutPattern, dn_ext_layout_ptn>) {
        return make_frame_layout<dn_ext_layout_ptn, half>(VALID_ROWS, 2);
    } else if constexpr (asc::te::Std::is_same_v<LayoutPattern, nz_layout_ptn>) {
        return make_frame_layout<nz_layout_ptn, half>(VALID_ROWS, 32);
    } else {
        static_assert(asc::te::Std::is_same_v<LayoutPattern, zn_layout_ptn>);
        return make_frame_layout<zn_layout_ptn, half>(32, VALID_COLUMNS);
    }
}

template <typename LayoutPattern>
auto make_stride_test_layout(int selected_stride)
{
    using namespace asc::te;

    auto valid_layout = make_stride_test_layout<LayoutPattern>();
    using LayoutTraitType = get_layout_trait<decltype(valid_layout)>;
    if constexpr (asc::te::Std::is_same_v<LayoutPattern, nd_ext_layout_ptn>) {
        auto stride = make_stride(make_stride(_0{}, selected_stride), make_stride(_0{}, _1{}));
        return make_pattern_layout<nd_ext_layout_ptn, LayoutTraitType>(valid_layout.shape(), stride);
    } else if constexpr (asc::te::Std::is_same_v<LayoutPattern, dn_ext_layout_ptn>) {
        auto stride = make_stride(make_stride(_0{}, _1{}), make_stride(_0{}, selected_stride));
        return make_pattern_layout<dn_ext_layout_ptn, LayoutTraitType>(valid_layout.shape(), stride);
    } else if constexpr (asc::te::Std::is_same_v<LayoutPattern, nz_layout_ptn>) {
        auto stride = make_stride(get<0>(valid_layout.stride()), make_stride(_1{}, selected_stride));
        return make_pattern_layout<nz_layout_ptn, LayoutTraitType>(valid_layout.shape(), stride);
    } else {
        static_assert(asc::te::Std::is_same_v<LayoutPattern, zn_layout_ptn>);
        auto stride = make_stride(make_stride(_1{}, selected_stride), get<1>(valid_layout.stride()));
        return make_pattern_layout<zn_layout_ptn, LayoutTraitType>(valid_layout.shape(), stride);
    }
}

TEST_F(TensorApiVectorCopyAbnormalValidation, AllPublicFormatRoutesRejectSmallerDestination)
{
    using namespace asc::te;

    expect_copy_trap<copy_gm_to_ub, location::gm, location::ub>(
        make_frame_layout<nd_ext_layout_ptn, half>(LARGE_ROWS, ROUTE_COLUMNS),
        make_frame_layout<nd_ext_layout_ptn, half>(SMALL_ROWS, ROUTE_COLUMNS));
    expect_copy_trap<copy_gm_to_ub, location::gm, location::ub>(
        make_frame_layout<dn_ext_layout_ptn, half>(LARGE_ROWS, ROUTE_COLUMNS),
        make_frame_layout<dn_ext_layout_ptn, half>(SMALL_ROWS, ROUTE_COLUMNS));
    expect_copy_trap<copy_gm_to_ub, location::gm, location::ub>(
        make_frame_layout<nz_layout_ptn, half>(LARGE_ROWS, ROUTE_COLUMNS),
        make_frame_layout<nz_layout_ptn, half>(SMALL_ROWS, ROUTE_COLUMNS));
    expect_copy_trap<copy_gm_to_ub, location::gm, location::ub>(
        make_frame_layout<zn_layout_ptn, half>(LARGE_ROWS, ROUTE_COLUMNS),
        make_frame_layout<zn_layout_ptn, half>(SMALL_ROWS, ROUTE_COLUMNS));

    expect_copy_trap<copy_ub_to_gm, location::ub, location::gm>(
        make_frame_layout<nd_ext_layout_ptn, half>(LARGE_ROWS, ROUTE_COLUMNS),
        make_frame_layout<nd_ext_layout_ptn, half>(SMALL_ROWS, ROUTE_COLUMNS));
    expect_copy_trap<copy_ub_to_gm, location::ub, location::gm>(
        make_frame_layout<dn_ext_layout_ptn, half>(LARGE_ROWS, ROUTE_COLUMNS),
        make_frame_layout<dn_ext_layout_ptn, half>(SMALL_ROWS, ROUTE_COLUMNS));
    expect_copy_trap<copy_ub_to_gm, location::ub, location::gm>(
        make_frame_layout<nz_layout_ptn, half>(LARGE_ROWS, ROUTE_COLUMNS),
        make_frame_layout<nz_layout_ptn, half>(SMALL_ROWS, ROUTE_COLUMNS));

    expect_copy_trap<copy_ub_to_l1, location::ub, location::l1>(
        make_frame_layout<nd_ext_layout_ptn, half>(LARGE_ROWS, ROUTE_COLUMNS),
        make_frame_layout<nd_ext_layout_ptn, half>(SMALL_ROWS, ROUTE_COLUMNS));
    expect_copy_trap<copy_ub_to_l1, location::ub, location::l1>(
        make_frame_layout<dn_ext_layout_ptn, half>(LARGE_ROWS, ROUTE_COLUMNS),
        make_frame_layout<dn_ext_layout_ptn, half>(SMALL_ROWS, ROUTE_COLUMNS));
    expect_copy_trap<copy_ub_to_l1, location::ub, location::l1>(
        make_frame_layout<nz_layout_ptn, half>(LARGE_ROWS, ROUTE_COLUMNS),
        make_frame_layout<nz_layout_ptn, half>(SMALL_ROWS, ROUTE_COLUMNS));
    expect_copy_trap<copy_ub_to_l1, location::ub, location::l1>(
        make_frame_layout<zn_layout_ptn, half>(LARGE_ROWS, ROUTE_COLUMNS),
        make_frame_layout<zn_layout_ptn, half>(SMALL_ROWS, ROUTE_COLUMNS));

    expect_copy_trap<copy_ub_to_ub, location::ub, location::ub>(
        make_frame_layout<nd_ext_layout_ptn, half>(LARGE_ROWS, ROUTE_COLUMNS),
        make_frame_layout<nd_ext_layout_ptn, half>(SMALL_ROWS, ROUTE_COLUMNS));
    expect_copy_trap<copy_ub_to_ub, location::ub, location::ub>(
        make_frame_layout<dn_ext_layout_ptn, half>(LARGE_ROWS, ROUTE_COLUMNS),
        make_frame_layout<dn_ext_layout_ptn, half>(SMALL_ROWS, ROUTE_COLUMNS));
    expect_copy_trap<copy_ub_to_ub, location::ub, location::ub>(
        make_frame_layout<nz_layout_ptn, half>(LARGE_ROWS, ROUTE_COLUMNS),
        make_frame_layout<nz_layout_ptn, half>(SMALL_ROWS, ROUTE_COLUMNS));
    expect_copy_trap<copy_ub_to_ub, location::ub, location::ub>(
        make_frame_layout<zn_layout_ptn, half>(LARGE_ROWS, ROUTE_COLUMNS),
        make_frame_layout<zn_layout_ptn, half>(SMALL_ROWS, ROUTE_COLUMNS));
}

TEST_F(TensorApiVectorCopyAbnormalValidation, AllPublicFormatRoutesRejectBlockCountAboveLimit)
{
    using namespace asc::te;

    auto nd_layout = make_frame_layout<nd_ext_layout_ptn, half>(BLOCK_COUNT_LIMIT_PLUS_ONE, VALID_COLUMNS);
    auto dn_layout = make_frame_layout<dn_ext_layout_ptn, half>(VALID_ROWS, BLOCK_COUNT_LIMIT_PLUS_ONE);
    auto nz_layout = make_frame_layout<nz_layout_ptn, half>(VALID_ROWS, FRACTAL_BLOCK_COUNT_LIMIT_PLUS_ONE);
    auto zn_layout = make_frame_layout<zn_layout_ptn, half>(FRACTAL_BLOCK_COUNT_LIMIT_PLUS_ONE, VALID_COLUMNS);

    expect_copy_trap<copy_gm_to_ub, location::gm, location::ub>(nd_layout, nd_layout);
    expect_copy_trap<copy_gm_to_ub, location::gm, location::ub>(dn_layout, dn_layout);
    expect_copy_trap<copy_gm_to_ub, location::gm, location::ub>(nz_layout, nz_layout);
    expect_copy_trap<copy_gm_to_ub, location::gm, location::ub>(zn_layout, zn_layout);

    expect_copy_trap<copy_ub_to_gm, location::ub, location::gm>(nd_layout, nd_layout);
    expect_copy_trap<copy_ub_to_gm, location::ub, location::gm>(dn_layout, dn_layout);
    expect_copy_trap<copy_ub_to_gm, location::ub, location::gm>(nz_layout, nz_layout);

    expect_copy_trap<copy_ub_to_l1, location::ub, location::l1>(nd_layout, nd_layout);
    expect_copy_trap<copy_ub_to_l1, location::ub, location::l1>(dn_layout, dn_layout);
    expect_copy_trap<copy_ub_to_l1, location::ub, location::l1>(nz_layout, nz_layout);
    expect_copy_trap<copy_ub_to_l1, location::ub, location::l1>(zn_layout, zn_layout);

    expect_copy_trap<copy_ub_to_ub, location::ub, location::ub>(nd_layout, nd_layout);
    expect_copy_trap<copy_ub_to_ub, location::ub, location::ub>(dn_layout, dn_layout);
    expect_copy_trap<copy_ub_to_ub, location::ub, location::ub>(nz_layout, nz_layout);
    expect_copy_trap<copy_ub_to_ub, location::ub, location::ub>(zn_layout, zn_layout);
}

TEST_F(TensorApiVectorCopyAbnormalValidation, CopyGM2UBRejectsAllInvalidSrcAndDstLayouts)
{
    expect_operand_layout_traps<asc::te::copy_gm_to_ub, asc::te::location::gm, asc::te::location::ub>();
}

TEST_F(TensorApiVectorCopyAbnormalValidation, CopyGM2UBRejectsPaddingOver32Bytes)
{
    using namespace asc::te;

    EXPECT_THROW((copy_gm2ub_with_padding<uint8_t>(32, 64, gm_to_ub_params{33, 0, true})), TrapException);
    EXPECT_THROW((copy_gm2ub_with_padding<half>(16, 32, gm_to_ub_params{0, 17, true})), TrapException);
    EXPECT_THROW((copy_gm2ub_with_padding<uint64_t>(4, 8, gm_to_ub_params{9, 0, true})), TrapException);
}

TEST_F(TensorApiVectorCopyAbnormalValidation, CopyGM2UBUsesUint32PaddingGranularityForEightByteTypes)
{
    using namespace asc::te;

    EXPECT_NO_THROW((copy_gm2ub_with_padding<uint64_t>(4, 8, gm_to_ub_params{8, 8, true})));
}

TEST_F(TensorApiVectorCopyAbnormalValidation, CopyGM2UBRejectsNonzeroPaddingInCompactMode)
{
    using namespace asc::te;

    EXPECT_THROW((copy_gm2ub_with_padding<uint8_t>(32, 32, gm_to_ub_params{1, 0, true})), TrapException);
    EXPECT_THROW((copy_gm2ub_with_padding<half>(16, 16, gm_to_ub_params{0, 1, true})), TrapException);
}

TEST_F(TensorApiVectorCopyAbnormalValidation, CopyUB2GMRejectsAllInvalidSrcAndDstLayouts)
{
    expect_operand_layout_traps<asc::te::copy_ub_to_gm, asc::te::location::ub, asc::te::location::gm>();
}

TEST_F(TensorApiVectorCopyAbnormalValidation, CopyUB2L1RejectsAllInvalidSrcAndDstLayouts)
{
    expect_operand_layout_traps<asc::te::copy_ub_to_l1, asc::te::location::ub, asc::te::location::l1>();
}

TEST_F(TensorApiVectorCopyAbnormalValidation, CopyUB2UBRejectsAllInvalidSrcAndDstLayouts)
{
    expect_operand_layout_traps<asc::te::copy_ub_to_ub, asc::te::location::ub, asc::te::location::ub>();
}

TEST_F(TensorApiVectorCopyAbnormalValidation, EveryPublicFormatRouteRejectsItsBlockLenUpperBoundary)
{
    using namespace asc::te;

    auto nd_layout = make_block_len_boundary_layout<nd_ext_layout_ptn>();
    auto dn_layout = make_block_len_boundary_layout<dn_ext_layout_ptn>();
    auto nz_layout = make_block_len_boundary_layout<nz_layout_ptn>();
    auto zn_layout = make_block_len_boundary_layout<zn_layout_ptn>();

    expect_copy_trap<copy_gm_to_ub, location::gm, location::ub>(nd_layout, nd_layout);
    expect_copy_trap<copy_gm_to_ub, location::gm, location::ub>(dn_layout, dn_layout);
    expect_copy_trap<copy_gm_to_ub, location::gm, location::ub>(nz_layout, nz_layout);
    expect_copy_trap<copy_gm_to_ub, location::gm, location::ub>(zn_layout, zn_layout);

    expect_copy_trap<copy_ub_to_gm, location::ub, location::gm>(nd_layout, nd_layout);
    expect_copy_trap<copy_ub_to_gm, location::ub, location::gm>(dn_layout, dn_layout);
    expect_copy_trap<copy_ub_to_gm, location::ub, location::gm>(nz_layout, nz_layout);

    expect_copy_trap<copy_ub_to_l1, location::ub, location::l1>(nd_layout, nd_layout);
    expect_copy_trap<copy_ub_to_l1, location::ub, location::l1>(dn_layout, dn_layout);
    expect_copy_trap<copy_ub_to_l1, location::ub, location::l1>(nz_layout, nz_layout);
    expect_copy_trap<copy_ub_to_l1, location::ub, location::l1>(zn_layout, zn_layout);

    expect_copy_trap<copy_ub_to_ub, location::ub, location::ub>(nd_layout, nd_layout);
    expect_copy_trap<copy_ub_to_ub, location::ub, location::ub>(dn_layout, dn_layout);
    expect_copy_trap<copy_ub_to_ub, location::ub, location::ub>(nz_layout, nz_layout);
    expect_copy_trap<copy_ub_to_ub, location::ub, location::ub>(zn_layout, zn_layout);
}

TEST_F(TensorApiVectorCopyAbnormalValidation, CopyUB2UBRejectsPublicBlockLenBelowOneDataBlock)
{
    using namespace asc::te;

    auto layout = make_frame_layout<nd_ext_layout_ptn, half>(1, 1);
    expect_copy_trap<copy_ub_to_ub, location::ub, location::ub>(layout, layout);
}

TEST_F(TensorApiVectorCopyAbnormalValidation, DirectionalStrideRestrictionsCoverEveryPublicFormatRoute)
{
    using namespace asc::te;

    auto nd_layout = make_stride_test_layout<nd_ext_layout_ptn>();
    auto dn_layout = make_stride_test_layout<dn_ext_layout_ptn>();
    auto nz_layout = make_stride_test_layout<nz_layout_ptn>();
    auto zn_layout = make_stride_test_layout<zn_layout_ptn>();
    auto nd_zero_stride = make_stride_test_layout<nd_ext_layout_ptn>(0);
    auto dn_zero_stride = make_stride_test_layout<dn_ext_layout_ptn>(0);
    auto nz_zero_stride = make_stride_test_layout<nz_layout_ptn>(0);
    auto zn_zero_stride = make_stride_test_layout<zn_layout_ptn>(0);
    auto nd_misaligned_stride = make_stride_test_layout<nd_ext_layout_ptn>(MISALIGNED_STRIDE_ELEMENTS);
    auto dn_misaligned_stride = make_stride_test_layout<dn_ext_layout_ptn>(MISALIGNED_STRIDE_ELEMENTS);
    auto nz_misaligned_stride = make_stride_test_layout<nz_layout_ptn>(MISALIGNED_STRIDE_ELEMENTS);
    auto zn_misaligned_stride = make_stride_test_layout<zn_layout_ptn>(MISALIGNED_STRIDE_ELEMENTS);

    expect_copy_trap<copy_gm_to_ub, location::gm, location::ub>(nd_layout, nd_zero_stride);
    expect_copy_trap<copy_gm_to_ub, location::gm, location::ub>(dn_layout, dn_zero_stride);
    expect_copy_trap<copy_gm_to_ub, location::gm, location::ub>(nz_layout, nz_zero_stride);
    expect_copy_trap<copy_gm_to_ub, location::gm, location::ub>(zn_layout, zn_zero_stride);
    expect_copy_trap<copy_gm_to_ub, location::gm, location::ub>(nd_layout, nd_misaligned_stride);
    expect_copy_trap<copy_gm_to_ub, location::gm, location::ub>(dn_layout, dn_misaligned_stride);
    expect_copy_trap<copy_gm_to_ub, location::gm, location::ub>(nz_layout, nz_misaligned_stride);
    expect_copy_trap<copy_gm_to_ub, location::gm, location::ub>(zn_layout, zn_misaligned_stride);

    expect_copy_trap<copy_ub_to_gm, location::ub, location::gm>(nd_zero_stride, nd_layout);
    expect_copy_trap<copy_ub_to_gm, location::ub, location::gm>(dn_zero_stride, dn_layout);
    expect_copy_trap<copy_ub_to_gm, location::ub, location::gm>(nz_zero_stride, nz_layout);
    expect_copy_trap<copy_ub_to_gm, location::ub, location::gm>(nd_misaligned_stride, nd_layout);
    expect_copy_trap<copy_ub_to_gm, location::ub, location::gm>(dn_misaligned_stride, dn_layout);
    expect_copy_trap<copy_ub_to_gm, location::ub, location::gm>(nz_misaligned_stride, nz_layout);
}

} // namespace
