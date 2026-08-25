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

class TensorApiDataStructureValidationMatrix : public testing::Test {
protected:
    void SetUp() override { AscendC::SetGCoreType(2); }

    void TearDown() override { AscendC::SetGCoreType(0); }
};

template <typename LocationType, typename PointerType, typename LayoutType>
auto make_tensor_at(PointerType pointer, const LayoutType& layout)
{
    return asc::te::make_tensor(asc::te::make_mem_ptr<LocationType>(pointer), layout);
}

TEST_F(TensorApiDataStructureValidationMatrix, MakeTensorLayoutRejectsRemainingInvalidShapeAndStrideTopologies)
{
    using namespace asc::te;

    alignas(512) __gm__ int32_t data[1024] = {};
    auto pointer = make_mem_ptr<location::gm>(data);

    EXPECT_THROW((void)make_tensor(pointer, make_layout(make_shape(4, -1), make_stride(4, 1))), TrapException);
    EXPECT_THROW((void)make_tensor(pointer, make_layout(make_shape(4, 4), make_stride(-4, 1))), TrapException);

    auto nested_shape_with_zero = make_shape(make_shape(16, 2), make_shape(8, 0));
    auto nested_stride = make_stride(make_stride(8, 128), make_stride(1, 256));
    EXPECT_THROW((void)make_tensor(pointer, make_layout(nested_shape_with_zero, nested_stride)), TrapException);

    auto batch_shape_with_negative = make_shape(-1, make_shape(4, 8));
    auto batch_stride = make_stride(32, make_stride(8, 1));
    EXPECT_THROW((void)make_tensor(pointer, make_layout(batch_shape_with_negative, batch_stride)), TrapException);
}

TEST_F(TensorApiDataStructureValidationMatrix, MakeTensorShapeStrideRejectsRemainingInvalidTopologies)
{
    using namespace asc::te;

    alignas(512) __gm__ int32_t data[1024] = {};
    auto pointer = make_mem_ptr<location::gm>(data);

    EXPECT_THROW((void)make_tensor(pointer, make_shape(4, 0), make_stride(4, 1)), TrapException);

    auto nested_shape_with_negative = make_shape(make_shape(16, 2), make_shape(8, -1));
    auto nested_shape = make_shape(make_shape(16, 2), make_shape(8, 4));
    auto nested_stride = make_stride(make_stride(8, 128), make_stride(1, 256));
    auto nested_stride_with_negative = make_stride(make_stride(8, 128), make_stride(1, -1));
    EXPECT_THROW((void)make_tensor(pointer, nested_shape_with_negative, nested_stride), TrapException);
    EXPECT_THROW((void)make_tensor(pointer, nested_shape, nested_stride_with_negative), TrapException);

    auto batch_shape_with_zero = make_shape(0, make_shape(4, 8));
    auto batch_shape = make_shape(2, make_shape(4, 8));
    auto batch_stride = make_stride(32, make_stride(8, 1));
    auto batch_stride_with_negative = make_stride(-32, make_stride(8, 1));
    EXPECT_THROW((void)make_tensor(pointer, batch_shape_with_zero, batch_stride), TrapException);
    EXPECT_THROW((void)make_tensor(pointer, batch_shape, batch_stride_with_negative), TrapException);
}

TEST_F(TensorApiDataStructureValidationMatrix, SliceRejectsInvalidCoordinatesAcrossInfoAndConstOverloads)
{
    using namespace asc::te;

    alignas(512) __gm__ int32_t data[64] = {};
    auto tensor = make_tensor_at<location::gm>(data, make_frame_layout<nd_layout_ptn, int32_t>(4, 8));
    const auto const_tensor = tensor;
    auto slice_layout = make_layout(make_shape(1, 1), make_stride(8, 1));

    EXPECT_THROW((void)slice(tensor, make_coord(4, 0), slice_layout), TrapException);
    EXPECT_THROW((void)slice(const_tensor, make_coord(-1, 0), slice_layout), TrapException);
    EXPECT_THROW((void)tensor.slice(make_coord(4, 0), slice_layout), TrapException);
    EXPECT_THROW((void)const_tensor.slice(make_coord(4, 0), slice_layout), TrapException);
}

TEST_F(TensorApiDataStructureValidationMatrix, MemberSliceRejectsInvalidShapeInfoAcrossConstOverloads)
{
    using namespace asc::te;

    alignas(512) __gm__ int32_t data[64] = {};
    auto tensor = make_tensor_at<location::gm>(data, make_frame_layout<nd_layout_ptn, int32_t>(4, 8));
    const auto const_tensor = tensor;

    EXPECT_THROW((void)tensor.slice(make_coord(0, 0), make_shape(1, -1)), TrapException);
    EXPECT_THROW((void)const_tensor.slice(make_coord(0, 0), make_shape(1, 0)), TrapException);
}

TEST_F(TensorApiDataStructureValidationMatrix, SliceRejectsInvalidBatchCoordinateWithLayoutInfo)
{
    using namespace asc::te;

    alignas(512) __cbuf__ int32_t data[2 * 32 * 32] = {};
    const auto tensor = make_tensor_at<location::l1>(data, make_frame_layout<nz_layout_ptn, int32_t>(2, 32, 32));
    auto slice_layout = make_frame_layout<nz_layout_ptn, int32_t>(1, 16, 16);
    auto matrix_coord = make_coord(make_coord(0, 0), make_coord(0, 0));

    EXPECT_THROW((void)tensor.slice(make_coord(2, matrix_coord), slice_layout), TrapException);
}

TEST_F(TensorApiDataStructureValidationMatrix, Crd2IdxShapeStrideRejectsAllCoordinateTopologies)
{
    using namespace asc::te;

    auto flat_shape = make_shape(4, 8);
    auto flat_stride = make_stride(8, 1);
    EXPECT_THROW((void)crd2idx(-1, flat_shape, flat_stride), TrapException);

    auto nested_shape = make_shape(make_shape(16, 2), make_shape(8, 4));
    auto nested_stride = make_stride(make_stride(8, 128), make_stride(1, 256));
    EXPECT_THROW(
        (void)crd2idx(make_coord(make_coord(-1, 0), make_coord(0, 0)), nested_shape, nested_stride), TrapException);
    EXPECT_THROW(
        (void)crd2idx(make_coord(make_coord(16, 0), make_coord(0, 0)), nested_shape, nested_stride), TrapException);
    EXPECT_THROW((void)crd2idx(make_coord(-1, 0), nested_shape, nested_stride), TrapException);

    auto flat_batch_shape = make_shape(2, 4, 8);
    auto flat_batch_stride = make_stride(32, 8, 1);
    EXPECT_THROW((void)crd2idx(make_coord(2, 0, 0), flat_batch_shape, flat_batch_stride), TrapException);

    auto nested_batch_shape = make_shape(2, nested_shape);
    auto nested_batch_stride = make_stride(1024, nested_stride);
    EXPECT_THROW(
        (void)crd2idx(make_coord(2, make_coord(0, 0)), nested_batch_shape, nested_batch_stride), TrapException);
}

TEST_F(TensorApiDataStructureValidationMatrix, Crd2IdxLayoutRejectsAllCoordinateTopologies)
{
    using namespace asc::te;

    auto flat_layout = make_layout(make_shape(4, 8), make_stride(8, 1));
    EXPECT_THROW((void)crd2idx(-1, flat_layout), TrapException);
    EXPECT_THROW((void)crd2idx(32, flat_layout), TrapException);

    auto nested_shape = make_shape(make_shape(16, 2), make_shape(8, 4));
    auto nested_stride = make_stride(make_stride(8, 128), make_stride(1, 256));
    auto nested_layout = make_layout(nested_shape, nested_stride);
    EXPECT_THROW((void)crd2idx(make_coord(make_coord(-1, 0), make_coord(0, 0)), nested_layout), TrapException);
    EXPECT_THROW((void)crd2idx(make_coord(32, 0), nested_layout), TrapException);

    auto flat_batch_layout = make_layout(make_shape(2, 4, 8), make_stride(32, 8, 1));
    EXPECT_THROW((void)crd2idx(make_coord(2, 0, 0), flat_batch_layout), TrapException);
}

TEST_F(TensorApiDataStructureValidationMatrix, LayoutIndexRejectsAllCoordinateTopologies)
{
    using namespace asc::te;

    auto flat_layout = make_layout(make_shape(4, 8), make_stride(8, 1));
    EXPECT_THROW((void)flat_layout(-1), TrapException);

    auto nested_shape = make_shape(make_shape(16, 2), make_shape(8, 4));
    auto nested_stride = make_stride(make_stride(8, 128), make_stride(1, 256));
    auto nested_layout = make_layout(nested_shape, nested_stride);
    EXPECT_THROW((void)nested_layout(make_coord(make_coord(-1, 0), make_coord(0, 0))), TrapException);
    EXPECT_THROW((void)nested_layout(make_coord(make_coord(16, 0), make_coord(0, 0))), TrapException);
    EXPECT_THROW((void)nested_layout(make_coord(-1, 0)), TrapException);
    EXPECT_THROW((void)nested_layout(make_coord(32, 0)), TrapException);

    auto flat_batch_layout = make_layout(make_shape(2, 4, 8), make_stride(32, 8, 1));
    EXPECT_THROW((void)flat_batch_layout(make_coord(2, 0, 0)), TrapException);
}

TEST_F(TensorApiDataStructureValidationMatrix, TensorElementAccessRejectsConstNestedAndBatchCoordinates)
{
    using namespace asc::te;

    alignas(512) __ubuf__ int32_t nested_data[32 * 32] = {};
    const auto nested_tensor =
        make_tensor_at<location::ub>(nested_data, make_frame_layout<nz_layout_ptn, int32_t>(32, 32));
    auto invalid_nested_coord = make_coord(make_coord(0, 0), make_coord(8, 0));
    EXPECT_THROW((void)nested_tensor[invalid_nested_coord], TrapException);

    alignas(512) __gm__ int32_t batch_data[2 * 4 * 8] = {};
    auto batch_tensor =
        make_tensor_at<location::gm>(batch_data, make_frame_layout<nd_ext_layout_ptn, int32_t>(2, 4, 8));
    auto matrix_origin = make_coord(make_coord(0, 0), make_coord(0, 0));
    EXPECT_THROW((void)batch_tensor[make_coord(2, matrix_origin)], TrapException);
}

TEST_F(TensorApiDataStructureValidationMatrix, TensorSubTensorRejectsConstNestedAndBatchCoordinates)
{
    using namespace asc::te;

    alignas(512) __cbuf__ int32_t nested_data[32 * 32] = {};
    const auto nested_tensor =
        make_tensor_at<location::l1>(nested_data, make_frame_layout<nz_layout_ptn, int32_t>(32, 32));
    EXPECT_THROW((void)nested_tensor(make_coord(make_coord(0, 0), make_coord(8, 0))), TrapException);

    alignas(512) __cbuf__ int32_t batch_data[2 * 32 * 32] = {};
    const auto batch_tensor =
        make_tensor_at<location::l1>(batch_data, make_frame_layout<nz_layout_ptn, int32_t>(2, 32, 32));
    auto matrix_coord = make_coord(make_coord(0, 0), make_coord(0, 0));
    EXPECT_THROW((void)batch_tensor(make_coord(2, matrix_coord)), TrapException);
    EXPECT_THROW((void)batch_tensor(2, matrix_coord), TrapException);
}

} // namespace
