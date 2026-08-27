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
#include "tensor_api/experimental/vector_compute.h"

namespace {

using tensor_api_runtime_validation::TrapException;

class TensorApiDataStructureStrictValidation : public testing::Test {
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

template <typename TensorType>
void expect_all_slice_source_entry_traps(TensorType& tensor)
{
    using namespace asc::te;

    const auto const_tensor = tensor;
    auto slice_shape = make_shape(1, 1);
    auto slice_layout = make_frame_layout<nd_layout_ptn, int32_t>(1, 1);
    auto coord = make_coord(0, 0);

    EXPECT_THROW((void)slice(tensor, coord, slice_shape), TrapException);
    EXPECT_THROW((void)slice(tensor, coord, slice_layout), TrapException);
    EXPECT_THROW((void)slice(const_tensor, coord, slice_shape), TrapException);
    EXPECT_THROW((void)slice(const_tensor, coord, slice_layout), TrapException);
    EXPECT_THROW((void)tensor.slice(coord, slice_shape), TrapException);
    EXPECT_THROW((void)tensor.slice(coord, slice_layout), TrapException);
    EXPECT_THROW((void)const_tensor.slice(coord, slice_shape), TrapException);
    EXPECT_THROW((void)const_tensor.slice(coord, slice_layout), TrapException);
}

template <typename TensorType>
void expect_flat_tensor_coordinate_traps(TensorType& tensor)
{
    using namespace asc::te;

    const auto const_tensor = tensor;
    auto invalid_coord = make_coord(4, 0);

    EXPECT_THROW((void)tensor[invalid_coord], TrapException);
    EXPECT_THROW((void)const_tensor[invalid_coord], TrapException);
    EXPECT_THROW((void)tensor[32], TrapException);
    EXPECT_THROW((void)const_tensor[32], TrapException);
    EXPECT_THROW((void)tensor(invalid_coord), TrapException);
    EXPECT_THROW((void)const_tensor(invalid_coord), TrapException);
    EXPECT_THROW((void)tensor(4, 0), TrapException);
    EXPECT_THROW((void)const_tensor(4, 0), TrapException);
}

template <typename TensorType>
void expect_nested_tensor_coordinate_traps(TensorType& tensor)
{
    using namespace asc::te;

    const auto const_tensor = tensor;
    auto valid_row_coord = make_coord(0, 0);
    auto invalid_column_coord = make_coord(8, 0);
    auto invalid_coord = make_coord(valid_row_coord, invalid_column_coord);
    auto invalid_logical_coord = make_coord(0, 32);

    EXPECT_THROW((void)tensor[invalid_coord], TrapException);
    EXPECT_THROW((void)const_tensor[invalid_coord], TrapException);
    EXPECT_THROW((void)tensor[invalid_logical_coord], TrapException);
    EXPECT_THROW((void)const_tensor[invalid_logical_coord], TrapException);
    EXPECT_THROW((void)tensor(invalid_coord), TrapException);
    EXPECT_THROW((void)const_tensor(invalid_coord), TrapException);
    EXPECT_THROW((void)tensor(valid_row_coord, invalid_column_coord), TrapException);
    EXPECT_THROW((void)const_tensor(valid_row_coord, invalid_column_coord), TrapException);
}

template <typename TensorType>
void expect_batch_tensor_coordinate_traps(TensorType& tensor)
{
    using namespace asc::te;

    const auto const_tensor = tensor;
    auto matrix_origin = make_coord(make_coord(0, 0), make_coord(0, 0));
    auto negative_batch_coord = make_coord(-1, matrix_origin);
    auto invalid_matrix_coord = make_coord(make_coord(0, 0), make_coord(8, 0));
    auto invalid_inner_coord = make_coord(0, invalid_matrix_coord);

    EXPECT_THROW((void)tensor[negative_batch_coord], TrapException);
    EXPECT_THROW((void)tensor[invalid_inner_coord], TrapException);
    EXPECT_THROW((void)const_tensor[negative_batch_coord], TrapException);
    EXPECT_THROW((void)const_tensor[invalid_inner_coord], TrapException);
    EXPECT_THROW((void)tensor(negative_batch_coord), TrapException);
    EXPECT_THROW((void)tensor(invalid_inner_coord), TrapException);
    EXPECT_THROW((void)const_tensor(negative_batch_coord), TrapException);
    EXPECT_THROW((void)const_tensor(invalid_inner_coord), TrapException);
    EXPECT_THROW((void)tensor(-1, matrix_origin), TrapException);
    EXPECT_THROW((void)tensor(0, invalid_matrix_coord), TrapException);
    EXPECT_THROW((void)const_tensor(-1, matrix_origin), TrapException);
    EXPECT_THROW((void)const_tensor(0, invalid_matrix_coord), TrapException);
}

TEST_F(TensorApiDataStructureStrictValidation, MakeTensorLocalOverloadsRejectNegativeShapeAndStride)
{
    using namespace asc::te;

    alignas(512) __ubuf__ int32_t data[64] = {};
    auto pointer = make_mem_ptr<location::ub>(data);

    auto negative_shape_layout = make_pattern_layout<nd_layout_ptn, layout_trait<asc::te::Std::ignore_t, _1>>(
        make_shape(4, -1), make_stride(4, _1{}));
    auto negative_stride_layout = make_pattern_layout<nd_layout_ptn, layout_trait<asc::te::Std::ignore_t, _1>>(
        make_shape(4, 4), make_stride(-4, _1{}));

    EXPECT_THROW((void)make_tensor(pointer, negative_shape_layout), TrapException);
    EXPECT_THROW((void)make_tensor(pointer, negative_stride_layout), TrapException);
    EXPECT_THROW((void)make_tensor(pointer, make_shape(4, -1), make_stride(4, _1{})), TrapException);
    EXPECT_THROW((void)make_tensor(pointer, make_shape(4, 4), make_stride(-4, _1{})), TrapException);
}

TEST_F(TensorApiDataStructureStrictValidation, SliceSourceValidationCoversAllPublicEntryForms)
{
    using namespace asc::te;

    alignas(512) __gm__ int32_t data[64] = {};
    auto zero_shape_layout = make_pattern_layout<nd_layout_ptn, layout_trait<asc::te::Std::ignore_t, _1>>(
        make_shape(4, 0), make_stride(4, _1{}));
    auto negative_shape_layout = make_pattern_layout<nd_layout_ptn, layout_trait<asc::te::Std::ignore_t, _1>>(
        make_shape(4, -1), make_stride(4, _1{}));
    auto negative_stride_layout = make_pattern_layout<nd_layout_ptn, layout_trait<asc::te::Std::ignore_t, _1>>(
        make_shape(4, 4), make_stride(-4, _1{}));

    auto zero_shape_tensor = make_tensor_directly<location::gm>(data, zero_shape_layout);
    auto negative_shape_tensor = make_tensor_directly<location::gm>(data, negative_shape_layout);
    auto negative_stride_tensor = make_tensor_directly<location::gm>(data, negative_stride_layout);

    expect_all_slice_source_entry_traps(zero_shape_tensor);
    expect_all_slice_source_entry_traps(negative_shape_tensor);
    expect_all_slice_source_entry_traps(negative_stride_tensor);
}

TEST_F(TensorApiDataStructureStrictValidation, SliceRejectsInvalidBatchCoordinatesForNDAndNZ)
{
    using namespace asc::te;

    alignas(512) __gm__ int32_t nd_data[2 * 4 * 8] = {};
    auto nd_tensor = make_tensor_at<location::gm>(nd_data, make_frame_layout<nd_layout_ptn, int32_t>(2, 4, 8));
    const auto const_nd_tensor = nd_tensor;
    auto nd_slice_shape = make_shape(1, make_shape(1, 1));
    auto nd_slice_layout = make_frame_layout<nd_layout_ptn, int32_t>(1, 1, 1);

    EXPECT_THROW((void)slice(nd_tensor, make_coord(-1, make_coord(0, 0)), nd_slice_shape), TrapException);
    EXPECT_THROW((void)const_nd_tensor.slice(make_coord(0, make_coord(4, 0)), nd_slice_layout), TrapException);

    alignas(512) __cbuf__ int32_t nz_data[2 * 32 * 32] = {};
    auto nz_tensor = make_tensor_at<location::l1>(nz_data, make_frame_layout<nz_layout_ptn, int32_t>(2, 32, 32));
    const auto const_nz_tensor = nz_tensor;
    auto nz_slice_shape = make_shape(1, make_shape(16, 16));
    auto nz_slice_layout = make_frame_layout<nz_layout_ptn, int32_t>(1, 16, 16);

    EXPECT_THROW((void)nz_tensor.slice(make_coord(-1, make_coord(0, 0)), nz_slice_shape), TrapException);
    auto invalid_nz_coord = make_coord(0, make_coord(make_coord(0, 0), make_coord(8, 0)));
    EXPECT_THROW((void)slice(const_nz_tensor, invalid_nz_coord, nz_slice_layout), TrapException);
}

TEST_F(TensorApiDataStructureStrictValidation, SliceRejectsInvalidLogicalAndNestedNzCoordinates)
{
    using namespace asc::te;

    alignas(512) __cbuf__ int32_t data[32 * 32] = {};
    auto tensor = make_tensor_at<location::l1>(data, make_frame_layout<nz_layout_ptn, int32_t>(32, 32));
    const auto const_tensor = tensor;
    auto slice_layout = make_frame_layout<nz_layout_ptn, int32_t>(16, 16);

    EXPECT_THROW((void)const_tensor.slice(make_coord(-1, 0), make_shape(1, 1)), TrapException);
    EXPECT_THROW((void)slice(tensor, make_coord(make_coord(-1, 0), make_coord(0, 0)), slice_layout), TrapException);
    EXPECT_THROW((void)tensor.slice(make_coord(make_coord(0, 2), make_coord(0, 0)), slice_layout), TrapException);
}

TEST_F(TensorApiDataStructureStrictValidation, Crd2IdxAndLayoutRejectInvalidBatchCoordinates)
{
    using namespace asc::te;

    auto flat_shape = make_shape(2, 4, 8);
    auto flat_stride = make_stride(32, 8, 1);
    auto flat_layout = make_layout(flat_shape, flat_stride);
    auto flat_negative_batch = make_coord(-1, 0, 0);
    auto flat_invalid_inner = make_coord(0, 4, 0);

    EXPECT_THROW((void)crd2idx(flat_negative_batch, flat_shape, flat_stride), TrapException);
    EXPECT_THROW((void)crd2idx(flat_invalid_inner, flat_shape, flat_stride), TrapException);
    EXPECT_THROW((void)crd2idx(flat_negative_batch, flat_layout), TrapException);
    EXPECT_THROW((void)crd2idx(flat_invalid_inner, flat_layout), TrapException);
    EXPECT_THROW((void)flat_layout(flat_negative_batch), TrapException);
    EXPECT_THROW((void)flat_layout(flat_invalid_inner), TrapException);

    auto matrix_shape = make_shape(make_shape(16, 2), make_shape(8, 4));
    auto matrix_stride = make_stride(make_stride(8, 128), make_stride(1, 256));
    auto nested_batch_shape = make_shape(2, matrix_shape);
    auto nested_batch_stride = make_stride(1024, matrix_stride);
    auto nested_batch_layout = make_layout(nested_batch_shape, nested_batch_stride);
    auto nested_negative_batch = make_coord(-1, make_coord(0, 0));
    auto nested_invalid_inner = make_coord(0, make_coord(32, 0));

    EXPECT_THROW((void)crd2idx(nested_negative_batch, nested_batch_shape, nested_batch_stride), TrapException);
    EXPECT_THROW((void)crd2idx(nested_invalid_inner, nested_batch_shape, nested_batch_stride), TrapException);
    EXPECT_THROW((void)crd2idx(nested_negative_batch, nested_batch_layout), TrapException);
    EXPECT_THROW((void)crd2idx(nested_invalid_inner, nested_batch_layout), TrapException);
    EXPECT_THROW((void)nested_batch_layout(nested_negative_batch), TrapException);
    EXPECT_THROW((void)nested_batch_layout(nested_invalid_inner), TrapException);
}

TEST_F(TensorApiDataStructureStrictValidation, TensorCoordinateEntriesCoverConstnessAndTopologies)
{
    using namespace asc::te;

    alignas(512) __gm__ int32_t flat_data[4 * 8] = {};
    auto flat_tensor = make_tensor_at<location::gm>(flat_data, make_frame_layout<nd_layout_ptn, int32_t>(4, 8));
    expect_flat_tensor_coordinate_traps(flat_tensor);

    alignas(512) __ubuf__ int32_t nested_data[32 * 32] = {};
    auto nested_tensor = make_tensor_at<location::ub>(nested_data, make_frame_layout<nz_layout_ptn, int32_t>(32, 32));
    expect_nested_tensor_coordinate_traps(nested_tensor);

    alignas(512) __ubuf__ int32_t batch_data[2 * 32 * 32] = {};
    auto batch_tensor = make_tensor_at<location::ub>(batch_data, make_frame_layout<nz_layout_ptn, int32_t>(2, 32, 32));
    expect_batch_tensor_coordinate_traps(batch_tensor);
}

TEST_F(TensorApiDataStructureStrictValidation, TensorRegisterLoadStoreAndBroadcastRejectInvalidCoordinates)
{
    using namespace asc::te;

    alignas(512) __ubuf__ int32_t data[4 * 8] = {};
    auto tensor = make_tensor_at<location::ub>(data, make_frame_layout<nd_layout_ptn, int32_t>(4, 8));
    experimental::reg_tensor<int32_t> value0{};
    experimental::reg_tensor<int32_t> value1{};

    EXPECT_THROW((void)experimental::load(tensor, make_coord(-1, 0)), TrapException);
    EXPECT_THROW((void)experimental::load(tensor, make_coord(0, 8)), TrapException);
    EXPECT_THROW(experimental::load(tensor, make_coord(-1, 0), value0, value1), TrapException);
    EXPECT_THROW(experimental::load(tensor, make_coord(0, 8), value0, value1), TrapException);
    EXPECT_THROW((void)experimental::load_broadcast(tensor, make_coord(-1, 0)), TrapException);
    EXPECT_THROW((void)experimental::load_broadcast(tensor, make_coord(0, 8)), TrapException);
    EXPECT_THROW(experimental::store(tensor, make_coord(-1, 0), value0), TrapException);
    EXPECT_THROW(experimental::store(tensor, make_coord(4, 0), value0), TrapException);
    EXPECT_THROW(experimental::store(tensor, make_coord(-1, 0), value0, value1), TrapException);
    EXPECT_THROW(experimental::store(tensor, make_coord(4, 0), value0, value1), TrapException);
}

} // namespace
