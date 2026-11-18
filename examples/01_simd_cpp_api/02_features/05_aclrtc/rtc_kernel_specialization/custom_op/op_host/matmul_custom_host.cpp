/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "../op_kernel/matmul_custom_tiling.h"
#include "register/op_def_registry.h"
#include "tiling/tiling_api.h"

namespace {
bool IsExpectedShape(const gert::StorageShape* shape, int32_t rows, int32_t cols)
{
    if (shape == nullptr) {
        return false;
    }
    const auto& storage = shape->GetStorageShape();
    return storage.GetDimNum() == 2 && storage.GetDim(0) == rows && storage.GetDim(1) == cols;
}
} // namespace

namespace optiling {
static ge::graphStatus MatmulTiling(gert::TilingContext* context)
{
    if (context == nullptr || context->GetInputShape(0) == nullptr || context->GetInputShape(1) == nullptr ||
        context->GetOutputShape(0) == nullptr || context->GetRawTilingData() == nullptr ||
        context->GetWorkspaceSizes(1) == nullptr || context->GetPlatformInfo() == nullptr) {
        return ge::GRAPH_FAILED;
    }
    if (!IsExpectedShape(context->GetInputShape(0), kMatmulM, kMatmulK) ||
        !IsExpectedShape(context->GetInputShape(1), kMatmulK, kMatmulN) ||
        !IsExpectedShape(context->GetOutputShape(0), kMatmulM, kMatmulN)) {
        return ge::GRAPH_FAILED;
    }

    optiling::TCubeTiling tiling;
    platform_ascendc::PlatformAscendC platform(context->GetPlatformInfo());
    const auto coreNum = platform.GetCoreNumAic();
    if (coreNum == 0) {
        return ge::GRAPH_FAILED;
    }
    matmul_tiling::MultiCoreMatmulTiling tilingApi(platform);
    tilingApi.SetDim(coreNum);
    tilingApi.SetAType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16, false);
    tilingApi.SetBType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16, false);
    tilingApi.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tilingApi.SetBiasType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tilingApi.SetShape(kMatmulM, kMatmulN, kMatmulK);
    tilingApi.SetOrgShape(kMatmulM, kMatmulN, kMatmulK);
    tilingApi.SetBias(false);
    tilingApi.SetBufferSpace(-1, -1, -1);
    if (tilingApi.GetTiling(tiling) != 0) {
        return ge::GRAPH_FAILED;
    }

    auto* rawTiling = context->GetRawTilingData();
    if (tiling.get_usedCoreNum() <= 0 || tiling.GetDataSize() > rawTiling->GetCapacity() ||
        rawTiling->GetData() == nullptr) {
        return ge::GRAPH_FAILED;
    }
    tiling.SaveToBuffer(rawTiling->GetData(), rawTiling->GetCapacity());
    rawTiling->SetDataSize(tiling.GetDataSize());
    context->SetBlockDim(static_cast<uint32_t>(tiling.get_usedCoreNum()));
    context->SetTilingKey(0);
    context->GetWorkspaceSizes(1)[0] = 0;
    return ge::GRAPH_SUCCESS;
}
} // namespace optiling

namespace ge {
static graphStatus MatmulInferShape(gert::InferShapeContext* context)
{
    if (context == nullptr || context->GetInputShape(0) == nullptr || context->GetOutputShape(0) == nullptr) {
        return GRAPH_FAILED;
    }
    *context->GetOutputShape(0) = *context->GetInputShape(0);
    return GRAPH_SUCCESS;
}

static graphStatus MatmulInferDataType(gert::InferDataTypeContext* context)
{
    if (context == nullptr) {
        return GRAPH_FAILED;
    }
    context->SetOutputDataType(0, DT_FLOAT);
    return GRAPH_SUCCESS;
}
} // namespace ge

namespace ops {
class MatmulCustom : public OpDef {
public:
    explicit MatmulCustom(const char* name) : OpDef(name)
    {
        this->Input("a")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16})
            .Format({ge::FORMAT_ND})
            .UnknownShapeFormat({ge::FORMAT_ND});
        this->Input("b")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16})
            .Format({ge::FORMAT_ND})
            .UnknownShapeFormat({ge::FORMAT_ND});
        this->Output("c")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT})
            .Format({ge::FORMAT_ND})
            .UnknownShapeFormat({ge::FORMAT_ND});
        this->SetInferShape(ge::MatmulInferShape).SetInferDataType(ge::MatmulInferDataType);
        this->AICore()
            .SetTiling(optiling::MatmulTiling)
            .AddConfig("ascend910b")
            .AddConfig("ascend910_93")
            .AddConfig("ascend950");
    }
};

OP_ADD(MatmulCustom);
} // namespace ops
