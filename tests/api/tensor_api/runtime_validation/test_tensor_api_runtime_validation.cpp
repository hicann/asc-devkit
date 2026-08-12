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

class TensorApiRuntimeValidation : public testing::Test {
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

TEST_F(TensorApiRuntimeValidation, MakeTensorRejectsZeroShape)
{
    using namespace asc::te;

    int rows = 4;
    int columns = 0;
    __gm__ int32_t data[16] = {};
    auto layout = make_layout(make_shape(rows, columns), make_stride(4, 1));

    EXPECT_THROW((void)make_tensor(make_mem_ptr<location::gm>(data), layout), TrapException);
}

TEST_F(TensorApiRuntimeValidation, MakeTensorRejectsNegativeShape)
{
    using namespace asc::te;

    int rows = 4;
    int columns = -1;
    __gm__ int32_t data[16] = {};
    auto shape = make_shape(rows, columns);
    auto stride = make_stride(4, 1);

    EXPECT_THROW((void)make_tensor(make_mem_ptr<location::gm>(data), shape, stride), TrapException);
}

TEST_F(TensorApiRuntimeValidation, MakeTensorRejectsNegativeStride)
{
    using namespace asc::te;

    int row_stride = -4;
    __gm__ int32_t data[16] = {};
    auto shape = make_shape(4, 4);
    auto stride = make_stride(row_stride, 1);

    EXPECT_THROW((void)make_tensor(make_mem_ptr<location::gm>(data), shape, stride), TrapException);
}

TEST_F(TensorApiRuntimeValidation, SliceRejectsOutOfRangeCoordinate)
{
    using namespace asc::te;

    __gm__ int32_t data[32] = {};
    auto tensor = make_tensor_at<location::gm>(data, make_frame_layout<nd_layout_ptn, int32_t>(4, 4));
    auto coord = make_coord(4, 0);
    auto slice_shape = make_shape(1, 1);

    EXPECT_THROW((void)slice(tensor, coord, slice_shape), TrapException);
}

TEST_F(TensorApiRuntimeValidation, SliceRejectsZeroShape)
{
    using namespace asc::te;

    int slice_columns = 0;
    __gm__ int32_t data[32] = {};
    auto tensor = make_tensor_at<location::gm>(data, make_frame_layout<nd_layout_ptn, int32_t>(4, 4));
    auto coord = make_coord(0, 0);
    auto slice_shape = make_shape(1, slice_columns);

    EXPECT_THROW((void)slice(tensor, coord, slice_shape), TrapException);
}

TEST_F(TensorApiRuntimeValidation, SliceRejectsNegativeCoordinate)
{
    using namespace asc::te;

    __gm__ int32_t data[32] = {};
    auto tensor = make_tensor_at<location::gm>(data, make_frame_layout<nd_layout_ptn, int32_t>(4, 4));
    auto coord = make_coord(-1, 0);
    auto slice_shape = make_shape(1, 1);

    EXPECT_THROW((void)slice(tensor, coord, slice_shape), TrapException);
}

TEST_F(TensorApiRuntimeValidation, SliceRejectsZeroShapeInLayout)
{
    using namespace asc::te;

    int slice_columns = 0;
    __gm__ int32_t data[32] = {};
    auto tensor = make_tensor_at<location::gm>(data, make_frame_layout<nd_layout_ptn, int32_t>(4, 4));
    auto coord = make_coord(0, 0);
    auto slice_layout = make_layout(make_shape(1, slice_columns), make_stride(1, 1));

    EXPECT_THROW((void)slice(tensor, coord, slice_layout), TrapException);
}

TEST_F(TensorApiRuntimeValidation, TensorMemberSliceRejectsOutOfRangeCoordinate)
{
    using namespace asc::te;

    __gm__ int32_t data[32] = {};
    auto tensor = make_tensor_at<location::gm>(data, make_frame_layout<nd_layout_ptn, int32_t>(4, 4));
    auto coord = make_coord(4, 0);

    EXPECT_THROW((void)tensor.slice(coord, make_shape(1, 1)), TrapException);
}

TEST_F(TensorApiRuntimeValidation, ConstTensorMemberSliceRejectsOutOfRangeCoordinate)
{
    using namespace asc::te;

    __gm__ int32_t data[32] = {};
    const auto tensor = make_tensor_at<location::gm>(data, make_frame_layout<nd_layout_ptn, int32_t>(4, 4));
    auto coord = make_coord(4, 0);

    EXPECT_THROW((void)tensor.slice(coord, make_shape(1, 1)), TrapException);
}

TEST_F(TensorApiRuntimeValidation, Crd2IdxRejectsNegativeCoordinate)
{
    using namespace asc::te;

    auto coord = make_coord(-1, 0);
    auto shape = make_shape(4, 8);
    auto stride = make_stride(8, 1);

    EXPECT_THROW((void)crd2idx(coord, shape, stride), TrapException);
}

TEST_F(TensorApiRuntimeValidation, Crd2IdxRejectsCoordinateOutsideLayout)
{
    using namespace asc::te;

    auto coord = make_coord(4, 0);
    auto layout = make_layout(make_shape(4, 8), make_stride(8, 1));

    EXPECT_THROW((void)crd2idx(coord, layout), TrapException);
}

TEST_F(TensorApiRuntimeValidation, LayoutIndexRejectsOutOfRangeCoordinate)
{
    using namespace asc::te;

    auto coord = make_coord(4, 0);
    auto layout = make_layout(make_shape(4, 8), make_stride(8, 1));

    EXPECT_THROW((void)layout(coord), TrapException);
}

TEST_F(TensorApiRuntimeValidation, TensorElementAccessRejectsOutOfRangeCoordinate)
{
    using namespace asc::te;

    __gm__ int32_t data[64] = {};
    auto tensor = make_tensor_at<location::gm>(data, make_frame_layout<nd_layout_ptn, int32_t>(4, 8));
    auto coord = make_coord(4, 0);

    EXPECT_THROW((void)tensor[coord], TrapException);
}

TEST_F(TensorApiRuntimeValidation, ConstTensorElementAccessRejectsNegativeCoordinate)
{
    using namespace asc::te;

    __gm__ int32_t data[64] = {};
    const auto tensor = make_tensor_at<location::gm>(data, make_frame_layout<nd_layout_ptn, int32_t>(4, 8));
    auto coord = make_coord(-1, 0);

    EXPECT_THROW((void)tensor[coord], TrapException);
}

TEST_F(TensorApiRuntimeValidation, TensorTupleSubTensorRejectsOutOfRangeCoordinate)
{
    using namespace asc::te;

    __gm__ int32_t data[64] = {};
    auto tensor = make_tensor_at<location::gm>(data, make_frame_layout<nd_layout_ptn, int32_t>(4, 8));
    auto coord = make_coord(4, 0);

    EXPECT_THROW((void)tensor(coord), TrapException);
}

TEST_F(TensorApiRuntimeValidation, ConstTensorTupleSubTensorRejectsNegativeCoordinate)
{
    using namespace asc::te;

    __gm__ int32_t data[64] = {};
    const auto tensor = make_tensor_at<location::gm>(data, make_frame_layout<nd_layout_ptn, int32_t>(4, 8));
    auto coord = make_coord(-1, 0);

    EXPECT_THROW((void)tensor(coord), TrapException);
}

TEST_F(TensorApiRuntimeValidation, TensorSubTensorRejectsOutOfRangeCoordinate)
{
    using namespace asc::te;

    __gm__ int32_t data[64] = {};
    auto tensor = make_tensor_at<location::gm>(data, make_frame_layout<nd_layout_ptn, int32_t>(4, 8));

    EXPECT_THROW((void)tensor(4, 0), TrapException);
}

TEST_F(TensorApiRuntimeValidation, ConstTensorSubTensorRejectsNegativeCoordinate)
{
    using namespace asc::te;

    __gm__ int32_t data[64] = {};
    const auto tensor = make_tensor_at<location::gm>(data, make_frame_layout<nd_layout_ptn, int32_t>(4, 8));

    EXPECT_THROW((void)tensor(-1, 0), TrapException);
}

} // namespace
