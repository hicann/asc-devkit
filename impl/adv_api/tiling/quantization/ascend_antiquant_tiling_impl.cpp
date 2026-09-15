/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file ascend_antiquant_tiling_impl.cpp
 * \brief
 */
#include "adv_api/utils/types.h"
#include "adv_api/quantization/ascend_antiquant_tiling.h"
#include "../../detail/host_log.h"
#include "utils/tiling/platform/platform_ascendc.h"

namespace AscendC {
namespace {
constexpr uint32_t ANTI_QUANT_MIN_TMP_SIZE = 1024;
constexpr uint32_t ASCEND_ANTIQUANT_TWO = 2;
constexpr uint32_t ASCEND_ANTIQUANT_SINGLE_N_SIZE = 64;
constexpr uint32_t ASCEND_ANTIQUANT_TPM_N_SIZE = 80;
constexpr uint32_t ASCEND_ANTIQUANT_ALIGN_K_SIZE = 32;

inline uint32_t GetAscendAntiQuantTmpSizeOfFp4(const AscendC::TensorShape& scaleShape, bool isTranspose)
{
    auto shapeDims = scaleShape.GetDims();
    uint32_t scaleSize = 1;
    for (uint32_t i = 0; i < shapeDims.size(); i++) {
        scaleSize *= shapeDims[i];
    }
    if (isTranspose) {
        uint32_t alignedScaleSize = (scaleSize + 31) / 32 * 32; // need 32B aligned
        return alignedScaleSize * ASCEND_ANTIQUANT_TWO;
    } else {
        return 0;
    }
}

uint32_t GetScaleSize(const AscendC::TensorShape& scaleShape)
{
    auto shapeDims = scaleShape.GetDims();
    uint32_t scaleSize = 1;
    for (uint32_t i = 0; i < shapeDims.size(); i++) {
        scaleSize *= shapeDims[i];
    }
    return scaleSize;
}

void CheckAntiQuantHostCommon(
    const char* apiName, const char* hostFuncName, const AscendC::TensorShape& srcShape, bool isTranspose,
    AscendC::TensorDataType inputDataType)
{
    ASCENDC_HOST_ASSERT(
        srcShape.GetShapeSize() > 0, return, "[%s][%s] Input Shape size must be greater than 0.", apiName,
        hostFuncName);
    uint32_t k = static_cast<uint32_t>(srcShape.GetDims()[1]);
    if (isTranspose) {
        ASCENDC_HOST_ASSERT(
            static_cast<int32_t>(inputDataType) * k % ASCEND_ANTIQUANT_ALIGN_K_SIZE == 0, continue,
            "[%s][%s] When isTranspose is true, srcShape is [N, K] where the dimension K must be 32B aligned!", apiName,
            hostFuncName);
    }
    return;
}
} // namespace

uint32_t GetAscendAntiQuantMaxTmpSize(
    const AscendC::TensorShape& srcShape, const AscendC::TensorShape& scaleShape, bool isTranspose,
    AscendC::TensorDataType inputDataType, AscendC::TensorDataType outputDataType)
{
    CheckAntiQuantHostCommon("AscendAntiQuant", "GetAscendAntiQuantMaxTmpSize", srcShape, isTranspose, inputDataType);
    if (inputDataType == ge::DT_FLOAT4_E2M1 || inputDataType == ge::DT_FLOAT4_E1M2) {
        return GetAscendAntiQuantTmpSizeOfFp4(scaleShape, isTranspose);
    }
    if (outputDataType == ge::DT_FLOAT16) {
        return 0;
    }

    uint32_t scaleSize = GetScaleSize(scaleShape);
    auto shapeDims = srcShape.GetDims();
    uint32_t srcSize = 1;
    for (uint32_t i = 0; i < shapeDims.size(); i++) {
        srcSize *= shapeDims[i];
    }
    bool isPerChannel = (scaleSize == 1) ? false : true;

    if (isTranspose && isPerChannel) {
        uint32_t tmpTensorScaleSize = 8 * scaleSize * sizeof(float);  // 8  * N FP32
        uint32_t tmpTensorOffsetSize = 8 * scaleSize * sizeof(float); // 8  * N FP32
        uint32_t tmpTensorInputSize = 64 * scaleSize * sizeof(float); // 64 * N FP32
        return tmpTensorScaleSize + tmpTensorOffsetSize + tmpTensorInputSize;
    } else if (isTranspose && (!isPerChannel)) {
        return srcSize * sizeof(float);
    } else if ((!isTranspose) && isPerChannel) {
        uint32_t k = srcShape.GetDims()[0];
        return scaleSize * ASCEND_ANTIQUANT_TWO * sizeof(float) + ASCEND_ANTIQUANT_SINGLE_N_SIZE * k * sizeof(float);
    } else {
        return srcSize * sizeof(float);
    }
}

uint32_t GetAscendAntiQuantMinTmpSize(
    const AscendC::TensorShape& srcShape, const AscendC::TensorShape& scaleShape, bool isTranspose,
    AscendC::TensorDataType inputDataType, AscendC::TensorDataType outputDataType)
{
    CheckAntiQuantHostCommon("AscendAntiQuant", "GetAscendAntiQuantMinTmpSize", srcShape, isTranspose, inputDataType);
    if (inputDataType == ge::DT_FLOAT4_E2M1 || inputDataType == ge::DT_FLOAT4_E1M2) {
        return GetAscendAntiQuantTmpSizeOfFp4(scaleShape, isTranspose);
    }
    if (outputDataType == ge::DT_FLOAT16) {
        return 0;
    }

    uint32_t scaleSize = GetScaleSize(scaleShape);
    bool isPerChannel = (scaleSize == 1) ? false : true;
    if (!isPerChannel) {
        return ANTI_QUANT_MIN_TMP_SIZE;
    }

    auto shapeDims = srcShape.GetDims();
    uint32_t srcSize = 1;
    for (uint32_t i = 0; i < shapeDims.size(); i++) {
        srcSize *= shapeDims[i];
    }

    if (isTranspose) {
        uint32_t tmpTensorScaleSize = 8 * scaleSize * sizeof(float);  // 8  * N FP32
        uint32_t tmpTensorOffsetSize = 8 * scaleSize * sizeof(float); // 8  * N FP32
        uint32_t tmpTensorInputSize = 64 * scaleSize * sizeof(float); // 64 * N FP32
        return tmpTensorScaleSize + tmpTensorOffsetSize + tmpTensorInputSize;
    } else {
        uint32_t k = srcShape.GetDims()[0];
        return scaleSize * ASCEND_ANTIQUANT_TWO * sizeof(float) + ASCEND_ANTIQUANT_SINGLE_N_SIZE * k * sizeof(float);
    }
}

void GetAscendAntiQuantTmpBufferFactorSize(
    const AscendC::TensorShape& srcShape, const AscendC::TensorShape& scaleShape, bool isTranspose,
    AscendC::TensorDataType inputDataType, AscendC::TensorDataType outputDataType, uint32_t& maxLiveNodeCount,
    uint32_t& extraBuf)
{
    extraBuf = 0;
    maxLiveNodeCount = 0;
    uint32_t scaleSize = GetScaleSize(scaleShape);
    bool isPerChannel = (scaleSize == 1) ? false : true;
    if (inputDataType == ge::DT_FLOAT4_E2M1 || inputDataType == ge::DT_FLOAT4_E1M2) {
        if (isTranspose) {
            uint32_t alignedScaleSize = (scaleSize + 31) / 32 * 32;
            extraBuf = alignedScaleSize * ASCEND_ANTIQUANT_TWO;
        }
    } else if (outputDataType == ge::DT_FLOAT16) {
        maxLiveNodeCount = 0;
    } else if (isTranspose && isPerChannel) {
        extraBuf = ASCEND_ANTIQUANT_TPM_N_SIZE * scaleSize * sizeof(float);
    } else if ((isTranspose && (!isPerChannel)) || ((!isTranspose) && (!isPerChannel))) {
        maxLiveNodeCount = 1;
    } else if ((!isTranspose) && isPerChannel) {
        extraBuf = scaleSize * ASCEND_ANTIQUANT_TWO * sizeof(float) +
                   ASCEND_ANTIQUANT_SINGLE_N_SIZE * srcShape.GetDims()[0] * sizeof(float);
    }
}

void GetAscendAntiQuantMaxMinTmpSize(
    const AscendC::TensorShape& srcShape, const AscendC::TensorShape& scaleShape, bool isTranspose,
    AscendC::TensorDataType inputDataType, AscendC::TensorDataType outputDataType, uint32_t& maxValue,
    uint32_t& minValue)
{
    CheckAntiQuantHostCommon(
        "AscendAntiQuant", "GetAscendAntiQuantMaxMinTmpSize", srcShape, isTranspose, inputDataType);
    maxValue = GetAscendAntiQuantMaxTmpSize(srcShape, scaleShape, isTranspose, inputDataType, outputDataType);
    minValue = GetAscendAntiQuantMinTmpSize(srcShape, scaleShape, isTranspose, inputDataType, outputDataType);
}
} // namespace AscendC
