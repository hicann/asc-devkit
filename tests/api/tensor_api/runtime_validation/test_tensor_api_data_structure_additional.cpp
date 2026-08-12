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

class TensorApiDataStructureAdditionalValidation : public testing::Test {
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

TEST_F(TensorApiDataStructureAdditionalValidation, MakeTensorLayoutRejectsNegativeNestedShape)
{
    using namespace asc::te;

    alignas(512) __gm__ int32_t data[32] = {};
    auto shape = make_shape(make_shape(16, -1), make_shape(8, 2));
    auto stride = make_stride(make_stride(8, 128), make_stride(1, 256));
    auto layout = make_layout(shape, stride);

    EXPECT_THROW((void)make_tensor(make_mem_ptr<location::gm>(data), layout), TrapException);
}

TEST_F(TensorApiDataStructureAdditionalValidation, MakeTensorLayoutRejectsNegativeNestedStride)
{
    using namespace asc::te;

    alignas(512) __gm__ int32_t data[32] = {};
    auto shape = make_shape(make_shape(16, 2), make_shape(8, 2));
    auto stride = make_stride(make_stride(8, 128), make_stride(1, -1));
    auto layout = make_layout(shape, stride);

    EXPECT_THROW((void)make_tensor(make_mem_ptr<location::gm>(data), layout), TrapException);
}

TEST_F(TensorApiDataStructureAdditionalValidation, MakeTensorShapeStrideRejectsZeroNestedShape)
{
    using namespace asc::te;

    alignas(512) __gm__ int32_t data[32] = {};
    auto shape = make_shape(make_shape(16, 2), make_shape(8, 0));
    auto stride = make_stride(make_stride(8, 128), make_stride(1, 256));

    EXPECT_THROW((void)make_tensor(make_mem_ptr<location::gm>(data), shape, stride), TrapException);
}

TEST_F(TensorApiDataStructureAdditionalValidation, MakeTensorLocalRejectsZeroBatchShape)
{
    using namespace asc::te;

    alignas(512) __ubuf__ int32_t data[32] = {};
    auto layout = make_frame_layout<nd_ext_layout_ptn, int32_t>(0, 4, 4);

    EXPECT_THROW((void)make_tensor(make_mem_ptr<location::ub>(data), layout), TrapException);
}

TEST_F(TensorApiDataStructureAdditionalValidation, MakeTensorShapeStrideRejectsNegativeBatchShape)
{
    using namespace asc::te;

    alignas(512) __gm__ int32_t data[32] = {};
    auto matrix_shape = make_shape(make_shape(16, 2), make_shape(8, 2));
    auto matrix_stride = make_stride(make_stride(8, 128), make_stride(1, 256));
    auto shape = make_shape(-1, matrix_shape);
    auto stride = make_stride(512, matrix_stride);

    EXPECT_THROW((void)make_tensor(make_mem_ptr<location::gm>(data), shape, stride), TrapException);
}

TEST_F(TensorApiDataStructureAdditionalValidation, MakeTensorLayoutRejectsNegativeBatchStride)
{
    using namespace asc::te;

    alignas(512) __gm__ int32_t data[32] = {};
    auto matrix_shape = make_shape(make_shape(16, 2), make_shape(8, 2));
    auto matrix_stride = make_stride(make_stride(8, 128), make_stride(1, 256));
    auto layout = make_layout(make_shape(2, matrix_shape), make_stride(-512, matrix_stride));

    EXPECT_THROW((void)make_tensor(make_mem_ptr<location::gm>(data), layout), TrapException);
}

TEST_F(TensorApiDataStructureAdditionalValidation, FreeSliceShapeRejectsNegativeSliceShape)
{
    using namespace asc::te;

    alignas(512) __gm__ int32_t data[32] = {};
    auto tensor = make_tensor_at<location::gm>(data, make_frame_layout<nd_layout_ptn, int32_t>(4, 4));

    EXPECT_THROW((void)slice(tensor, make_coord(0, 0), make_shape(1, -1)), TrapException);
}

TEST_F(TensorApiDataStructureAdditionalValidation, FreeSliceLayoutRejectsNegativeSliceShape)
{
    using namespace asc::te;

    alignas(512) __gm__ int32_t data[32] = {};
    auto tensor = make_tensor_at<location::gm>(data, make_frame_layout<nd_layout_ptn, int32_t>(4, 4));
    auto slice_layout = make_layout(make_shape(-1, 1), make_stride(4, 1));

    EXPECT_THROW((void)slice(tensor, make_coord(0, 0), slice_layout), TrapException);
}

TEST_F(TensorApiDataStructureAdditionalValidation, FreeSliceRejectsInvalidSourceShape)
{
    using namespace asc::te;

    alignas(512) __gm__ int32_t data[32] = {};
    auto layout = make_layout(make_shape(4, 0), make_stride(4, 1));
    auto tensor = make_tensor_directly<location::gm>(data, layout);

    EXPECT_THROW((void)slice(tensor, make_coord(0, 0), make_shape(1, 1)), TrapException);
}

TEST_F(TensorApiDataStructureAdditionalValidation, FreeSliceRejectsInvalidSourceStride)
{
    using namespace asc::te;

    alignas(512) __gm__ int32_t data[32] = {};
    auto layout = make_layout(make_shape(4, 4), make_stride(-4, 1));
    auto tensor = make_tensor_directly<location::gm>(data, layout);

    EXPECT_THROW((void)slice(tensor, make_coord(0, 0), make_shape(1, 1)), TrapException);
}

TEST_F(TensorApiDataStructureAdditionalValidation, TensorMemberSliceLayoutRejectsZeroSliceShape)
{
    using namespace asc::te;

    alignas(512) __gm__ int32_t data[32] = {};
    auto tensor = make_tensor_at<location::gm>(data, make_frame_layout<nd_layout_ptn, int32_t>(4, 4));
    auto slice_layout = make_layout(make_shape(1, 0), make_stride(4, 1));

    EXPECT_THROW((void)tensor.slice(make_coord(0, 0), slice_layout), TrapException);
}

TEST_F(TensorApiDataStructureAdditionalValidation, ConstTensorMemberSliceLayoutRejectsNegativeSliceShape)
{
    using namespace asc::te;

    alignas(512) __gm__ int32_t data[32] = {};
    const auto tensor = make_tensor_at<location::gm>(data, make_frame_layout<nd_layout_ptn, int32_t>(4, 4));
    auto slice_layout = make_layout(make_shape(1, -1), make_stride(4, 1));

    EXPECT_THROW((void)tensor.slice(make_coord(0, 0), slice_layout), TrapException);
}

TEST_F(TensorApiDataStructureAdditionalValidation, FreeSliceConstTensorRejectsNegativeCoordinate)
{
    using namespace asc::te;

    alignas(512) __gm__ int32_t data[32] = {};
    const auto tensor = make_tensor_at<location::gm>(data, make_frame_layout<nd_layout_ptn, int32_t>(4, 4));

    EXPECT_THROW((void)slice(tensor, make_coord(0, -1), make_shape(1, 1)), TrapException);
}

TEST_F(TensorApiDataStructureAdditionalValidation, TensorMemberSliceRejectsLogicalCoordinateOutsideNzShape)
{
    using namespace asc::te;

    alignas(512) __cbuf__ int32_t data[32 * 32] = {};
    auto tensor = make_tensor_at<location::l1>(data, make_frame_layout<nz_layout_ptn, int32_t>(32, 32));

    auto invalid_coord = make_coord(32, 0);
    EXPECT_THROW((void)tensor.slice(invalid_coord, make_shape(1, 1)), TrapException);
}

TEST_F(TensorApiDataStructureAdditionalValidation, TensorMemberSliceRejectsBatchCoordinateOutsideNzShape)
{
    using namespace asc::te;

    alignas(512) __cbuf__ int32_t data[2 * 32 * 32] = {};
    auto tensor = make_tensor_at<location::l1>(data, make_frame_layout<nz_layout_ptn, int32_t>(2, 32, 32));
    auto slice_shape = make_shape(1, make_shape(16, 16));

    auto matrix_origin = make_coord(0, 0);
    EXPECT_THROW((void)tensor.slice(make_coord(2, matrix_origin), slice_shape), TrapException);
}

TEST_F(TensorApiDataStructureAdditionalValidation, Crd2IdxShapeStrideRejectsCoordinateAtUpperBound)
{
    using namespace asc::te;

    auto shape = make_shape(4, 8);
    auto stride = make_stride(8, 1);

    EXPECT_THROW((void)crd2idx(make_coord(4, 0), shape, stride), TrapException);
}

TEST_F(TensorApiDataStructureAdditionalValidation, Crd2IdxLayoutRejectsNegativeCoordinate)
{
    using namespace asc::te;

    auto layout = make_layout(make_shape(4, 8), make_stride(8, 1));

    EXPECT_THROW((void)crd2idx(make_coord(0, -1), layout), TrapException);
}

TEST_F(TensorApiDataStructureAdditionalValidation, Crd2IdxShapeStrideRejectsLinearCoordinateAtUpperBound)
{
    using namespace asc::te;

    auto shape = make_shape(4, 8);
    auto stride = make_stride(8, 1);

    EXPECT_THROW((void)crd2idx(32, shape, stride), TrapException);
}

TEST_F(TensorApiDataStructureAdditionalValidation, Crd2IdxLayoutRejectsNestedCoordinateOutsideShape)
{
    using namespace asc::te;

    auto shape = make_shape(make_shape(16, 2), make_shape(8, 4));
    auto stride = make_stride(make_stride(8, 128), make_stride(1, 256));
    auto layout = make_layout(shape, stride);
    auto coord = make_coord(make_coord(16, 0), make_coord(0, 0));

    EXPECT_THROW((void)crd2idx(coord, layout), TrapException);
}

TEST_F(TensorApiDataStructureAdditionalValidation, Crd2IdxShapeStrideRejectsLogicalCoordinateOutsideNestedShape)
{
    using namespace asc::te;

    auto shape = make_shape(make_shape(16, 2), make_shape(8, 4));
    auto stride = make_stride(make_stride(8, 128), make_stride(1, 256));

    EXPECT_THROW((void)crd2idx(make_coord(32, 0), shape, stride), TrapException);
}

TEST_F(TensorApiDataStructureAdditionalValidation, Crd2IdxLayoutRejectsBatchCoordinateOutsideShape)
{
    using namespace asc::te;

    auto matrix_shape = make_shape(make_shape(16, 2), make_shape(8, 4));
    auto matrix_stride = make_stride(make_stride(8, 128), make_stride(1, 256));
    auto layout = make_layout(make_shape(2, matrix_shape), make_stride(1024, matrix_stride));

    EXPECT_THROW((void)crd2idx(make_coord(2, make_coord(0, 0)), layout), TrapException);
}

TEST_F(TensorApiDataStructureAdditionalValidation, LayoutIndexRejectsNegativeCoordinate)
{
    using namespace asc::te;

    auto layout = make_layout(make_shape(4, 8), make_stride(8, 1));

    EXPECT_THROW((void)layout(make_coord(-1, 0)), TrapException);
}

TEST_F(TensorApiDataStructureAdditionalValidation, LayoutIndexRejectsLinearCoordinateAtUpperBound)
{
    using namespace asc::te;

    auto layout = make_layout(make_shape(4, 8), make_stride(8, 1));

    EXPECT_THROW((void)layout(32), TrapException);
}

TEST_F(TensorApiDataStructureAdditionalValidation, LayoutIndexRejectsBatchCoordinateOutsideNestedShape)
{
    using namespace asc::te;

    auto matrix_shape = make_shape(make_shape(16, 2), make_shape(8, 4));
    auto matrix_stride = make_stride(make_stride(8, 128), make_stride(1, 256));
    auto layout = make_layout(make_shape(2, matrix_shape), make_stride(1024, matrix_stride));

    EXPECT_THROW((void)layout(make_coord(2, make_coord(0, 0))), TrapException);
}

TEST_F(TensorApiDataStructureAdditionalValidation, TensorElementAccessRejectsCoordinateOutsideNzShapeInUb)
{
    using namespace asc::te;

    alignas(512) __ubuf__ int32_t data[32 * 32] = {};
    auto tensor = make_tensor_at<location::ub>(data, make_frame_layout<nz_layout_ptn, int32_t>(32, 32));

    auto invalid_coord = make_coord(make_coord(0, 0), make_coord(8, 0));
    EXPECT_THROW((void)tensor[invalid_coord], TrapException);
}

TEST_F(TensorApiDataStructureAdditionalValidation, TensorElementAccessRejectsLinearCoordinateAtUpperBound)
{
    using namespace asc::te;

    alignas(512) __gm__ int32_t data[8] = {};
    auto tensor = make_tensor_at<location::gm>(data, make_layout(make_shape(8), make_stride(1)));

    EXPECT_THROW((void)tensor[8], TrapException);
}

TEST_F(TensorApiDataStructureAdditionalValidation, TensorSubTensorRejectsCoordinateOutsideNzShapeInL1)
{
    using namespace asc::te;

    alignas(512) __cbuf__ int32_t data[32 * 32] = {};
    auto tensor = make_tensor_at<location::l1>(data, make_frame_layout<nz_layout_ptn, int32_t>(32, 32));
    auto coord = make_coord(make_coord(0, 0), make_coord(8, 0));

    EXPECT_THROW((void)tensor(coord), TrapException);
}

TEST_F(TensorApiDataStructureAdditionalValidation, TensorVariadicSubTensorRejectsBatchCoordinateOutsideNzShape)
{
    using namespace asc::te;

    alignas(512) __cbuf__ int32_t data[2 * 32 * 32] = {};
    auto tensor = make_tensor_at<location::l1>(data, make_frame_layout<nz_layout_ptn, int32_t>(2, 32, 32));
    auto matrix_coord = make_coord(make_coord(0, 0), make_coord(0, 0));

    EXPECT_THROW((void)tensor(2, matrix_coord), TrapException);
}

} // namespace
