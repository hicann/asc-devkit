/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "adv_api/utils/types.h"
#include "adv_api/normalization/layernorm_grad_tiling.h"
#include "adv_api/normalization/layernorm_grad_tilingdata.h"
#include "../../detail/host_log.h"
#include "utils/tiling/platform/platform_ascendc.h"
namespace optiling {
REGISTER_TILING_DATA_CLASS(LayerNormGradTilingOpApi, LayerNormGradTiling);
}
namespace AscendC {
namespace {
constexpr uint32_t LAYERNORM_GRAD_HALF_SIZE = 2;
constexpr uint32_t LAYERNORM_GRAD_FLOAT_SIZE = 4;
constexpr uint32_t LAYERNORM_GRAD_SRC_DIM_NUM = 4;
union LastDimValue {
    float floatValue;
    uint32_t uint32Value;
};
void CheckSrcShape(std::vector<int64_t> shapeDims)
{
    constexpr uint32_t LAYERNORM_GRAD_SHAPE_SIZE = 4;
    ASCENDC_HOST_ASSERT(
        shapeDims.size() >= LAYERNORM_GRAD_SHAPE_SIZE, return, "srcShape dims must not be less than 4.");
    ASCENDC_HOST_ASSERT(shapeDims[0] > 0, return, "srcShape[0] must be greater than 0.");
    ASCENDC_HOST_ASSERT(shapeDims[1] > 0, return, "srcShape[1] must be greater than 0.");
    ASCENDC_HOST_ASSERT(shapeDims[2] > 0, return, "srcShape[2] must be greater than 0.");
    ASCENDC_HOST_ASSERT(shapeDims[3] > 0, return, "srcShape[3] must be greater than 0.");
}

void CheckLayerNormGradHostCommon(
    const char* apiName, const char* hostFuncName, const AscendC::TensorShape& srcShape, const uint32_t typeSize)
{
    ASCENDC_HOST_ASSERT(
        typeSize == LAYERNORM_GRAD_HALF_SIZE || typeSize == LAYERNORM_GRAD_FLOAT_SIZE, return,
        "[%s][%s] Type size %u is unsupported!", apiName, hostFuncName, typeSize);
    ASCENDC_HOST_ASSERT(
        srcShape.GetShapeSize() > 0, return, "[%s][%s] Input Shape size must be greater than 0.", apiName,
        hostFuncName);
    ASCENDC_HOST_ASSERT(
        srcShape.GetDimNum() == LAYERNORM_GRAD_SRC_DIM_NUM, return,
        "[%s][%s] The dims of srcShape is %zu, should be 4 (e.g. [B, S, storageHLength, originHLength])!", apiName,
        hostFuncName, srcShape.GetDimNum());
    return;
}

void SetTensorInfo(optiling::LayerNormGradTiling& tiling, uint32_t oneCalSize, uint32_t typeSize, bool isReuseSource)
{
    uint32_t tmpTensorBSHPos = 0;
    uint32_t pdVarTensorPos = tmpTensorBSHPos + oneCalSize;
    uint32_t pdMeanTensorPos = pdVarTensorPos + oneCalSize;
    uint32_t x1TensorPos = 0;
    uint32_t x2TensorPos = 0;
    uint32_t x3TensorPos = pdMeanTensorPos + oneCalSize;
    if (!(isReuseSource && typeSize == LAYERNORM_GRAD_B32_BYTE_SIZE)) {
        x1TensorPos = pdMeanTensorPos + oneCalSize;
        x2TensorPos = x1TensorPos + oneCalSize;
        x3TensorPos = x2TensorPos + oneCalSize;
    }
    uint32_t tmpTensorPos = 0;
    uint32_t tmpTensor1Pos = 0;
    uint32_t tmpTensor2Pos = 0;
    if (typeSize == LAYERNORM_GRAD_B16_BYTE_SIZE) {
        tmpTensorPos = x3TensorPos + oneCalSize;
        tmpTensor1Pos = tmpTensorPos + oneCalSize;
        tmpTensor2Pos = tmpTensor1Pos + oneCalSize;
    }
    tiling.set_tmpTensorBSHPos(tmpTensorBSHPos);
    tiling.set_pdVarTensorPos(pdVarTensorPos);
    tiling.set_pdMeanTensorPos(pdMeanTensorPos);
    tiling.set_x1TensorPos(x1TensorPos);
    tiling.set_x2TensorPos(x2TensorPos);
    tiling.set_x3TensorPos(x3TensorPos);
    tiling.set_tmpTensorPos(tmpTensorPos);
    tiling.set_tmpTensor1Pos(tmpTensor1Pos);
    tiling.set_tmpTensor2Pos(tmpTensor2Pos);
}

void SetTilingData(
    optiling::LayerNormGradTiling& tiling, const uint32_t oneCalSize, const uint32_t typeSize, const bool isReuseSource)
{
    SetTensorInfo(tiling, oneCalSize, typeSize, isReuseSource);
    tiling.set_tmpTensorBSHSize(oneCalSize);
    tiling.set_pdVarTensorSize(oneCalSize);
    tiling.set_pdMeanTensorSize(oneCalSize);
    tiling.set_x1TensorSize(oneCalSize);
    tiling.set_x2TensorSize(oneCalSize);
    tiling.set_x3TensorSize(oneCalSize);
    tiling.set_tmpTensorSize(oneCalSize);
    tiling.set_tmpTensor1Size(oneCalSize);
    tiling.set_tmpTensor2Size(oneCalSize);
}
} // namespace

void GetLayerNormGradMaxMinTmpSize(
    const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue,
    uint32_t& minValue)
{
    CheckLayerNormGradHostCommon("LayerNormGrad", "GetLayerNormGradMaxMinTmpSize", srcShape, typeSize);
    std::vector<int64_t> shapeDims = srcShape.GetDims();
    CheckSrcShape(shapeDims);
    uint32_t bLength = shapeDims[0];
    uint32_t sLength = shapeDims[1];
    uint32_t hLength = shapeDims[2];
    platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
    ASCENDC_HOST_ASSERT((platform != nullptr), return, "Failed to get PlatformAscendC.");
    const auto npuArch = platform->GetCurNpuArch();
    if (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201) {
        // all tmp data stored as float
        if (isReuseSource && typeSize == LAYERNORM_GRAD_B32_BYTE_SIZE) {
            // no tmp buffer required
            maxValue = 0;
            minValue = 0;
        } else {
            // two tmp buffer for x1 and x2
            uint32_t bufferSize = (hLength + 7) / 8 * 8;
            maxValue = LAYERNORM_GRAD_DAVID_BUF_NUM * bufferSize * sizeof(float);
            minValue = maxValue;
        }
    } else {
        uint32_t inputSize = bLength * sLength * hLength;
        uint32_t maxBaseSize = (inputSize > (hLength * hLength)) ? inputSize : (hLength * hLength);
        uint32_t minBaseSize = (inputSize < (hLength * hLength)) ? inputSize : (hLength * hLength);
        if (typeSize == LAYERNORM_GRAD_B16_BYTE_SIZE) {
            maxValue = LAYERNORM_GRAD_HALF_BUF_NUM * maxBaseSize * typeSize;
            minValue = minBaseSize * LAYERNORM_GRAD_HALF_BUF_NUM * typeSize;
            return;
        }
        if (isReuseSource) {
            maxValue = LAYERNORM_GRAD_REUSE_FLOAT_BUF_NUM * maxBaseSize * typeSize;
            minValue = minBaseSize * LAYERNORM_GRAD_REUSE_FLOAT_BUF_NUM * typeSize;
        } else {
            maxValue = LAYERNORM_GRAD_FLOAT_BUF_NUM * maxBaseSize * typeSize;
            minValue = minBaseSize * LAYERNORM_GRAD_FLOAT_BUF_NUM * typeSize;
        }
    }
}

void GetLayerNormGradNDTilingInfo(
    const AscendC::TensorShape srcShape, const uint32_t stackBufferSize, const uint32_t typeSize,
    const bool isReuseSource, optiling::LayerNormGradTiling& tiling)
{
    CheckLayerNormGradHostCommon("LayerNormGrad", "GetLayerNormGradNDTilingInfo", srcShape, typeSize);
    std::vector<int64_t> shapeDims = srcShape.GetDims();
    CheckSrcShape(shapeDims);
    uint32_t bLength = shapeDims[0];
    uint32_t sLength = shapeDims[1];
    uint32_t hLength = shapeDims[2];
    uint32_t originalHLength = shapeDims[3];
    uint32_t inputXSize = bLength * sLength * hLength;
    uint32_t needBufferBlock = (typeSize == LAYERNORM_GRAD_B16_BYTE_SIZE) ?
                                   LAYERNORM_GRAD_HALF_BUF_NUM :
                                   (isReuseSource ? LAYERNORM_GRAD_REUSE_FLOAT_BUF_NUM : LAYERNORM_GRAD_FLOAT_BUF_NUM);
    platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
    ASCENDC_HOST_ASSERT((platform != nullptr), return, "Failed to get PlatformAscendC.");
    const auto npuArch = platform->GetCurNpuArch();
    uint32_t oneCalSize = 0;
    uint32_t nohCalSize = 0;
    if (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201) {
        // needless to calculate oneCalSize and nohCalSize
        oneCalSize = static_cast<uint32_t>(1);
        nohCalSize = static_cast<uint32_t>(1);
    } else {
        oneCalSize = stackBufferSize * static_cast<uint32_t>(sizeof(uint8_t)) / static_cast<uint32_t>(sizeof(float)) /
                     needBufferBlock;
        oneCalSize = oneCalSize / hLength * hLength;
        ASCENDC_HOST_ASSERT(oneCalSize > static_cast<uint32_t>(0), return, "stackBufferSize is not enough.");
        nohCalSize = oneCalSize / hLength;
        uint32_t dataNumPerBlock = (typeSize == LAYERNORM_GRAD_B32_BYTE_SIZE) ? LAYERNORM_GRAD_B32_DATA_NUM_PER_BLOCK :
                                                                                LAYERNORM_GRAD_B16_DATA_NUM_PER_BLOCK;
        nohCalSize = (nohCalSize + dataNumPerBlock - static_cast<uint32_t>(1)) / dataNumPerBlock * dataNumPerBlock;
        oneCalSize = nohCalSize * hLength;
    }
    SetTilingData(tiling, oneCalSize, typeSize, isReuseSource);
    LastDimValue lastDimValueBack;
    lastDimValueBack.floatValue = 1.0f / static_cast<float>(originalHLength);
    LastDimValue lastDimValueBackMulTwo;
    lastDimValueBackMulTwo.floatValue = 2.0f / static_cast<float>(originalHLength);
    tiling.set_stackBufferSize(stackBufferSize);
    tiling.set_bLength(bLength);
    tiling.set_sLength(sLength);
    tiling.set_hLength(hLength);
    tiling.set_originalHLength(originalHLength);
    tiling.set_oneCalSize(oneCalSize);
    tiling.set_nohCalSize(nohCalSize);
    tiling.set_loopNum(inputXSize / oneCalSize);
    tiling.set_tailSize(inputXSize % oneCalSize);
    tiling.set_nohTailSize((inputXSize % oneCalSize) / hLength);
    tiling.set_lastDimValueBack(lastDimValueBack.uint32Value);
    tiling.set_lastDimValueBackMulTwo(lastDimValueBackMulTwo.uint32Value);
}

void GetLayerNormGradNDTilingInfo(
    const AscendC::TensorShape srcShape, const uint32_t stackBufferSize, const uint32_t typeSize,
    const bool isReuseSource, AscendC::tiling::LayerNormGradTiling& tiling)
{
    optiling::LayerNormGradTiling tilingData;
    GetLayerNormGradNDTilingInfo(srcShape, stackBufferSize, typeSize, isReuseSource, tilingData);
    tilingData.SaveToBuffer(&tiling, sizeof(LayerNormGradTiling));
}
} // namespace AscendC
