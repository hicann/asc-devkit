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
#include "adv_api/normalization/layernorm_tiling.h"
#include <cmath>
#include "adv_api/normalization/normalize_tiling.h"
#include "utils/tiling/platform/platform_ascendc.h"
#include "../../detail/host_log.h"

namespace optiling {
REGISTER_TILING_DATA_CLASS(LayerNormTilingOpApi, LayerNormTiling);
REGISTER_TILING_DATA_CLASS(LayerNormSeparateTilingOpApi, LayerNormSeparateTiling);
} // namespace optiling
namespace AscendC {
namespace {
constexpr uint32_t LAYERNORM_SIZEOF_FLOAT = 4;
constexpr uint32_t LAYERNORM_SIZEOF_HALF = 2;
constexpr uint32_t LAYERNORM_THREE_TIMES = 3;
constexpr uint32_t LAYERNORM_TWO_TIMES = 2;
constexpr uint32_t LAYERNORM_ONE_BLK_SIZE = 32;
constexpr uint32_t LAYERNORM_ONE_BLK_SHIFT_AMOUNT = 5;
constexpr uint32_t LAYERNORM_ONE_NUMBER = 1;
constexpr uint32_t LAYERNORM_ZERO_NUMBER = 0;
constexpr float LAYERNOR_LAST_DIM_INIT_VALUE = 1.0;
constexpr uint32_t WEL_UP_REP_SIZE = 256;
constexpr uint32_t WEL_UP_FLOAT_SIZE = 256 / sizeof(float);
constexpr uint32_t SHAPE_DIM = 2;
constexpr uint32_t LAYERNORM_FOLD_NUM = 2;
constexpr uint32_t LAYERNORM_SRC_DIM_NUM = 4;

void CheckLayerNormHostCommon(
    const char* apiName, const char* hostFuncName, const AscendC::TensorShape& srcShape, const uint32_t typeSize)
{
    ASCENDC_HOST_ASSERT(
        srcShape.GetShapeSize() > 0, return, "[%s][%s] Input Shape size must be greater than 0.", apiName,
        hostFuncName);
    ASCENDC_HOST_ASSERT(
        srcShape.GetDimNum() == LAYERNORM_SRC_DIM_NUM, return,
        "[%s][%s] The dims of srcShape is %zu, should be 4 (e.g. [B, S, storageHLength, originHLength])!", apiName,
        hostFuncName, srcShape.GetDimNum());
    ASCENDC_HOST_ASSERT(
        typeSize == LAYERNORM_SIZEOF_HALF || typeSize == LAYERNORM_SIZEOF_FLOAT, return,
        "[%s][%s] Type size %u is unsupported!", apiName, hostFuncName, typeSize);
    return;
}

uint32_t GetLayerNormMaxTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource)
{
    std::vector<int64_t> shapeDims = srcShape.GetDims();
    const uint32_t bLength = static_cast<uint32_t>(shapeDims[0]);
    const uint32_t sLength = static_cast<uint32_t>(shapeDims[1]);
    const uint32_t hLength = static_cast<uint32_t>(shapeDims[2]);

    uint32_t mvTmpLen = bLength * sLength * sizeof(float);
    uint32_t inputLen = bLength * sLength * hLength * sizeof(float);

    mvTmpLen = ((mvTmpLen + LAYERNORM_ONE_BLK_SIZE - LAYERNORM_ONE_NUMBER) >> LAYERNORM_ONE_BLK_SHIFT_AMOUNT)
               << LAYERNORM_ONE_BLK_SHIFT_AMOUNT;
    inputLen = ((inputLen + LAYERNORM_ONE_BLK_SIZE - LAYERNORM_ONE_NUMBER) >> LAYERNORM_ONE_BLK_SHIFT_AMOUNT)
               << LAYERNORM_ONE_BLK_SHIFT_AMOUNT;

    if (isReuseSource && (typeSize == LAYERNORM_SIZEOF_FLOAT)) {
        return LAYERNORM_TWO_TIMES * inputLen + LAYERNORM_TWO_TIMES * mvTmpLen;
    }
    return LAYERNORM_THREE_TIMES * inputLen + LAYERNORM_TWO_TIMES * mvTmpLen;
}

uint32_t GetLayerNormMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource)
{
    std::vector<int64_t> shapeDims = srcShape.GetDims();
    const uint32_t bLength = static_cast<uint32_t>(shapeDims[0]);
    const uint32_t sLength = static_cast<uint32_t>(shapeDims[1]);
    const uint32_t hLength = static_cast<uint32_t>(shapeDims[2]);

    uint32_t mvTmpLen = bLength * sLength * sizeof(float);
    uint32_t hLengthDiv = hLength * sizeof(float);

    mvTmpLen =
        (mvTmpLen + LAYERNORM_ONE_BLK_SIZE - LAYERNORM_ONE_NUMBER) / LAYERNORM_ONE_BLK_SIZE * LAYERNORM_ONE_BLK_SIZE;
    hLengthDiv =
        (hLengthDiv + LAYERNORM_ONE_BLK_SIZE - LAYERNORM_ONE_NUMBER) / LAYERNORM_ONE_BLK_SIZE * LAYERNORM_ONE_BLK_SIZE;

    if (isReuseSource && (typeSize == LAYERNORM_SIZEOF_FLOAT)) {
        return LAYERNORM_TWO_TIMES * hLengthDiv + LAYERNORM_TWO_TIMES * mvTmpLen;
    }
    return LAYERNORM_THREE_TIMES * hLengthDiv + LAYERNORM_TWO_TIMES * mvTmpLen;
}

void GetLayerNormNDTilingInfoImpl(
    const AscendC::TensorShape& srcShape, const uint32_t stackBufferSize, const uint32_t typeSize,
    const bool isReuseSource, optiling::LayerNormTiling& tiling)
{
    std::vector<int64_t> shapeDims = srcShape.GetDims();

    const uint32_t bLength = static_cast<uint32_t>(shapeDims[0]);
    const uint32_t sLength = static_cast<uint32_t>(shapeDims[1]);
    const uint32_t hLength = static_cast<uint32_t>(shapeDims[2]);
    const uint32_t originalHLength = shapeDims[3];

    const uint32_t inputXSize = bLength * sLength * hLength;
    const uint32_t meanVarSize = bLength * sLength;

    uint32_t numberOfTmpBuf = LAYERNORM_THREE_TIMES;
    if (isReuseSource && (typeSize == LAYERNORM_SIZEOF_FLOAT)) {
        numberOfTmpBuf = LAYERNORM_TWO_TIMES;
    }

    constexpr uint32_t oneBlockNum = LAYERNORM_ONE_BLK_SIZE / LAYERNORM_SIZEOF_FLOAT;
    constexpr uint32_t meanTmpTensorPos = LAYERNORM_ZERO_NUMBER;
    const uint32_t meanTmpTensorSize = (meanVarSize + oneBlockNum - LAYERNORM_ONE_NUMBER) / oneBlockNum * oneBlockNum;
    const uint32_t varianceTmpTensorPos = meanTmpTensorSize;
    const uint32_t varianceTmpTensorSize = meanTmpTensorSize;

    uint32_t meanVarTotalSize = meanTmpTensorSize + varianceTmpTensorSize;
    if (typeSize == LAYERNORM_SIZEOF_FLOAT) {
        meanVarTotalSize = LAYERNORM_ZERO_NUMBER;
    }

    const uint32_t tmpBufSize = stackBufferSize / LAYERNORM_SIZEOF_FLOAT;

    uint32_t oneTmpSize = (tmpBufSize - meanVarTotalSize) / numberOfTmpBuf;
    ASCENDC_HOST_ASSERT(hLength != 0, return, "the value of hLength should not be zero.");
    oneTmpSize = oneTmpSize / hLength * hLength;

    if (oneTmpSize > inputXSize) {
        oneTmpSize = inputXSize;
    }

    if (oneTmpSize == LAYERNORM_ZERO_NUMBER) {
        return;
    }

    const uint32_t firstTmpStartPos = meanVarTotalSize;
    const uint32_t secondTmpStartPos = firstTmpStartPos + oneTmpSize;
    const uint32_t thirdTmpStartPos = secondTmpStartPos + oneTmpSize;

    const uint32_t loopRound = inputXSize / oneTmpSize;

    const uint32_t inputRoundSize = oneTmpSize;
    const uint32_t inputTailSize = inputXSize % oneTmpSize;

    const uint32_t inputTailPos = inputXSize - inputTailSize;

    const uint32_t meanVarRoundSize = inputRoundSize / hLength;
    const uint32_t meanVarTailSize = inputTailSize / hLength;

    const uint32_t meanVarTailPos = meanVarSize - meanVarTailSize;

    const uint32_t bshCurLength = inputRoundSize;
    const uint32_t bsCurLength = meanVarRoundSize;

    const float lastDimValueBack = LAYERNOR_LAST_DIM_INIT_VALUE / static_cast<float>(originalHLength);

    tiling.set_bLength(bLength);
    tiling.set_sLength(sLength);
    tiling.set_hLength(hLength);
    tiling.set_originalHLength(originalHLength);
    tiling.set_inputXSize(inputXSize);
    tiling.set_meanVarSize(meanVarSize);
    tiling.set_numberOfTmpBuf(numberOfTmpBuf);
    tiling.set_meanTmpTensorPos(meanTmpTensorPos);
    tiling.set_meanTmpTensorSize(meanTmpTensorSize);
    tiling.set_varianceTmpTensorPos(varianceTmpTensorPos);
    tiling.set_varianceTmpTensorSize(varianceTmpTensorSize);
    tiling.set_tmpBufSize(tmpBufSize);
    tiling.set_oneTmpSize(oneTmpSize);
    tiling.set_firstTmpStartPos(firstTmpStartPos);
    tiling.set_secondTmpStartPos(secondTmpStartPos);
    tiling.set_thirdTmpStartPos(thirdTmpStartPos);
    tiling.set_loopRound(loopRound);
    tiling.set_inputRoundSize(inputRoundSize);
    tiling.set_inputTailSize(inputTailSize);
    tiling.set_inputTailPos(inputTailPos);
    tiling.set_meanVarRoundSize(meanVarRoundSize);
    tiling.set_meanVarTailSize(meanVarTailSize);
    tiling.set_meanVarTailPos(meanVarTailPos);
    tiling.set_bshCurLength(bshCurLength);
    tiling.set_bsCurLength(bsCurLength);
    tiling.set_lastDimValueBack(lastDimValueBack);
}

void GetLayerNormNDTilingInfoImpl(
    const AscendC::TensorShape& srcShape, const uint32_t stackBufferSize, const uint32_t typeSize,
    const bool isReuseSource, AscendC::tiling::LayerNormTiling& tiling)
{
    optiling::LayerNormTiling tilingData;
    GetLayerNormNDTilingInfoImpl(srcShape, stackBufferSize, typeSize, isReuseSource, tilingData);
    tilingData.SaveToBuffer(&tiling, sizeof(LayerNormTiling));
}
} // namespace

void GetLayerNormMaxMinTmpSize(
    const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue,
    uint32_t& minValue)
{
    CheckLayerNormHostCommon("LayerNorm", "GetLayerNormMaxMinTmpSize", srcShape, typeSize);
    maxValue = GetLayerNormMaxTmpSize(srcShape, typeSize, isReuseSource);
    minValue = GetLayerNormMinTmpSize(srcShape, typeSize, isReuseSource);
}

void GetLayerNormNDTillingInfo(
    const AscendC::TensorShape& srcShape, const uint32_t stackBufferSize, const uint32_t typeSize,
    const bool isReuseSource, optiling::LayerNormTiling& tilling)
{
    CheckLayerNormHostCommon("LayerNorm", "GetLayerNormNDTillingInfo", srcShape, typeSize);
    GetLayerNormNDTilingInfoImpl(srcShape, stackBufferSize, typeSize, isReuseSource, tilling);
}

void GetLayerNormNDTilingInfo(
    const AscendC::TensorShape& srcShape, const uint32_t stackBufferSize, const uint32_t typeSize,
    const bool isReuseSource, optiling::LayerNormTiling& tiling)
{
    CheckLayerNormHostCommon("LayerNorm", "GetLayerNormNDTilingInfo", srcShape, typeSize);
    GetLayerNormNDTilingInfoImpl(srcShape, stackBufferSize, typeSize, isReuseSource, tiling);
}

void GetLayerNormNDTilingInfo(
    const AscendC::TensorShape& srcShape, const uint32_t stackBufferSize, const uint32_t typeSize,
    const bool isReuseSource, AscendC::tiling::LayerNormTiling& tiling)
{
    CheckLayerNormHostCommon("LayerNorm", "GetLayerNormNDTilingInfo", srcShape, typeSize);
    GetLayerNormNDTilingInfoImpl(srcShape, stackBufferSize, typeSize, isReuseSource, tiling);
}

void GetWelfordUpdateMaxMinTmpSize(
    const AscendC::TensorShape& srcShape, const uint32_t typeSizeT, const uint32_t typeSizeU, const bool isReuseSource,
    const bool isInplace, uint32_t& maxValue, uint32_t& minValue)
{
    (void)isInplace;
    (void)typeSizeU;
    platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
    ASCENDC_HOST_ASSERT((platform != nullptr), return, "Failed to get PlatformAscendC.");
    const auto npuArch = platform->GetCurNpuArch();
    if (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201 || npuArch == NpuArch::DAV_5102) {
        (void)typeSizeT;
        (void)isReuseSource;
        minValue = 0;
        maxValue = 0;
        return;
    } else {
        std::vector<int64_t> shapeDims = srcShape.GetDims();
        ASCENDC_HOST_ASSERT(shapeDims.size() == SHAPE_DIM, return, "srcShape dims must be 2.");

        const uint32_t rnLength = static_cast<uint32_t>(shapeDims[0]);
        const uint32_t abLength = static_cast<uint32_t>(shapeDims[1]);

        if (typeSizeT == sizeof(uint16_t)) {
            minValue = 0x3 * WEL_UP_REP_SIZE; // dispense 3 buffers
        } else if (isReuseSource) {
            minValue = 1 * WEL_UP_REP_SIZE; // dispense 1 buffer
        } else {
            minValue = 0x2 * WEL_UP_REP_SIZE; // dispense 2 buffers
        }
        maxValue = (rnLength * abLength + WEL_UP_FLOAT_SIZE - 1) / WEL_UP_FLOAT_SIZE * minValue;
    }
}

void GetLayerNormMaxMinTmpSize(
    const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource, const bool isComputeRstd,
    const bool isOnlyOutput, uint32_t& maxValue, uint32_t& minValue)
{
    ASCENDC_HOST_ASSERT(typeSize != 0, return, "typeSize can not be 0!");
    ASCENDC_HOST_ASSERT(isOnlyOutput == false, return, "isOnlyOutput current only support false.");
    platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
    ASCENDC_HOST_ASSERT((platform != nullptr), return, "Failed to get PlatformAscendC.");
    const auto npuArch = platform->GetCurNpuArch();
    if (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201 || npuArch == NpuArch::DAV_5102) {
        (void)isReuseSource;
        std::vector<int64_t> shapeDims = srcShape.GetDims();
        const uint32_t vecLenB32 = platform->GetVecRegLen() / LAYERNORM_SIZEOF_FLOAT;
        const uint32_t vecLenB16 = platform->GetVecRegLen() / LAYERNORM_SIZEOF_HALF;
        ASCENDC_HOST_ASSERT(vecLenB32 != 0, return, "vecLenB32 can not be 0!");
        ASCENDC_HOST_ASSERT(vecLenB16 != 0, return, "vecLenB16 can not be 0!");
        const uint32_t rLength = static_cast<uint32_t>(shapeDims.back());
        uint32_t rLengthWithPadding = (rLength + vecLenB32 - 1) / vecLenB32 * vecLenB32;
        const uint32_t varianceLen = static_cast<uint32_t>(shapeDims.front());
        uint32_t len = (rLengthWithPadding / vecLenB32 + vecLenB16 - 1) / vecLenB16 * vecLenB16 + varianceLen;
        if (!isComputeRstd) {
            len += varianceLen;
        }
        minValue = len * sizeof(float);
        maxValue = minValue;
        return;
    } else {
        CheckLayerNormHostCommon("LayerNorm", "GetLayerNormMaxMinTmpSize", srcShape, typeSize);
        std::vector<int64_t> shapeDims = srcShape.GetDims();
        const uint32_t aLength = static_cast<uint32_t>(shapeDims[0]);
        const uint32_t rLength = static_cast<uint32_t>(shapeDims[1]);
        int32_t typeAignSize = 32 / typeSize;
        uint32_t rLengthWithPadding = (rLength + typeAignSize - 1) / typeAignSize * typeAignSize;
        uint32_t mvTmpLen = aLength * sizeof(float);
        uint32_t inputLen = aLength * rLengthWithPadding * sizeof(float);
        uint32_t rLengthDiv = rLengthWithPadding * sizeof(float);
        mvTmpLen = (mvTmpLen + LAYERNORM_ONE_BLK_SIZE - LAYERNORM_ONE_NUMBER) / LAYERNORM_ONE_BLK_SIZE *
                   LAYERNORM_ONE_BLK_SIZE;
        inputLen = (inputLen + LAYERNORM_ONE_BLK_SIZE - LAYERNORM_ONE_NUMBER) / LAYERNORM_ONE_BLK_SIZE *
                   LAYERNORM_ONE_BLK_SIZE;
        rLengthDiv = (rLengthDiv + LAYERNORM_ONE_BLK_SIZE - LAYERNORM_ONE_NUMBER) / LAYERNORM_ONE_BLK_SIZE *
                     LAYERNORM_ONE_BLK_SIZE;
        maxValue = LAYERNORM_TWO_TIMES * inputLen + LAYERNORM_ONE_NUMBER * mvTmpLen;
        minValue = LAYERNORM_TWO_TIMES * rLengthDiv + LAYERNORM_ONE_NUMBER * mvTmpLen;
        uint32_t maxNormalizeValue;
        uint32_t minNormalizeValue;
        GetNormalizeMaxMinTmpSize(
            srcShape, typeSize, typeSize, isReuseSource, isComputeRstd, isOnlyOutput, maxNormalizeValue,
            minNormalizeValue);
        if (minValue - mvTmpLen <= minNormalizeValue) {
            minValue = minNormalizeValue + mvTmpLen;
        }
        if (maxValue - mvTmpLen <= maxNormalizeValue) {
            maxValue = maxNormalizeValue + mvTmpLen;
        }
    }
}

void GetLayerNormNDTilingInfo(
    const AscendC::TensorShape& srcShape, const uint32_t stackBufferSize, const uint32_t typeSize,
    const bool isReuseSource, const bool isComputeRstd, optiling::LayerNormSeparateTiling& tiling)
{
    (void)isReuseSource;
    (void)isComputeRstd;
    ASCENDC_HOST_ASSERT(typeSize != 0, return, "typeSize can not be 0!");
    std::vector<int64_t> shapeDims = srcShape.GetDims();
    platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
    ASCENDC_HOST_ASSERT((platform != nullptr), return, "Failed to get PlatformAscendC.");
    const auto npuArch = platform->GetCurNpuArch();
    if (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201 || npuArch == NpuArch::DAV_5102) {
        (void)stackBufferSize;
        const uint32_t rLength = static_cast<uint32_t>(shapeDims.back());
        int32_t typeAignSize = 32 / typeSize;
        uint32_t rLengthWithPadding = (rLength + typeAignSize - 1) / typeAignSize * typeAignSize;
        uint32_t rHeadLength = platform->GetVecRegLen() / sizeof(float);
        ASCENDC_HOST_ASSERT(rHeadLength != 0, return, "rHeadLength can not be 0");
        uint32_t k = static_cast<uint32_t>(log2(rHeadLength));
        for (uint32_t i = 0; i < rLengthWithPadding; i++) {
            if (rHeadLength * LAYERNORM_FOLD_NUM > rLength) {
                k += i;
                break;
            }
            rHeadLength *= LAYERNORM_FOLD_NUM;
        }
        tiling.set_rLength(rLength);
        tiling.set_oneTmpSize(k);
        tiling.set_rHeadLength(rHeadLength);
        uint32_t kOverflow = k;
        if (pow(LAYERNORM_FOLD_NUM, kOverflow) < rLength) {
            kOverflow += 1;
        }
        uint32_t rLengthOverflow = static_cast<uint32_t>(pow(LAYERNORM_FOLD_NUM, kOverflow));
        float k2Rec = static_cast<float>(1) / static_cast<float>(rLengthOverflow);
        float k2RRec = static_cast<float>(rLengthOverflow) / static_cast<float>(rLength);
        tiling.set_k2Rec(k2Rec);
        tiling.set_k2RRec(k2RRec);
        return;
    } else {
        CheckLayerNormHostCommon("LayerNorm", "GetLayerNormNDTilingInfo", srcShape, typeSize);
        const uint32_t aLength = static_cast<uint32_t>(shapeDims[0]);
        const uint32_t rLength = static_cast<uint32_t>(shapeDims[1]);
        int32_t typeAignSize = 32 / typeSize;
        uint32_t rLengthWithPadding = (rLength + typeAignSize - 1) / typeAignSize * typeAignSize;

        const uint32_t inputXSize = aLength * rLengthWithPadding;
        const uint32_t meanVarSize = aLength;

        uint32_t numberOfTmpBuf = LAYERNORM_TWO_TIMES;

        constexpr uint32_t oneBlockNum = LAYERNORM_ONE_BLK_SIZE / LAYERNORM_SIZEOF_FLOAT;
        constexpr uint32_t varianceTmpTensorPos = LAYERNORM_ZERO_NUMBER;
        const uint32_t varianceTmpTensorSize =
            (meanVarSize + oneBlockNum - LAYERNORM_ONE_NUMBER) / oneBlockNum * oneBlockNum;

        const uint32_t tmpBufSize = stackBufferSize / LAYERNORM_SIZEOF_FLOAT;

        uint32_t oneTmpSize = (tmpBufSize - varianceTmpTensorSize) / numberOfTmpBuf;
        oneTmpSize = oneTmpSize / rLengthWithPadding * rLengthWithPadding;

        if (oneTmpSize > inputXSize) {
            oneTmpSize = inputXSize;
        }

        if (oneTmpSize == LAYERNORM_ZERO_NUMBER) {
            return;
        }

        const uint32_t firstTmpStartPos = varianceTmpTensorSize;
        const uint32_t secondTmpStartPos = firstTmpStartPos + oneTmpSize;

        const uint32_t loopRound = inputXSize / oneTmpSize;

        const uint32_t inputRoundSize = oneTmpSize;
        const uint32_t inputTailSize = inputXSize % oneTmpSize;

        const uint32_t inputTailPos = inputXSize - inputTailSize;

        const uint32_t meanVarRoundSize = inputRoundSize / rLengthWithPadding;
        const uint32_t meanVarTailSize = inputTailSize / rLengthWithPadding;

        const uint32_t meanVarTailPos = meanVarSize - meanVarTailSize;

        const uint32_t arCurLength = inputRoundSize;
        const uint32_t aCurLength = meanVarRoundSize;

        const float rValueBack = float(1) / static_cast<float>(rLength);

        tiling.set_aLength(aLength);
        tiling.set_rLength(rLength);
        tiling.set_inputXSize(inputXSize);
        tiling.set_meanVarSize(meanVarSize);
        tiling.set_numberOfTmpBuf(numberOfTmpBuf);
        tiling.set_varianceTmpTensorPos(varianceTmpTensorPos);
        tiling.set_varianceTmpTensorSize(varianceTmpTensorSize);
        tiling.set_tmpBufSize(tmpBufSize);
        tiling.set_oneTmpSize(oneTmpSize);
        tiling.set_firstTmpStartPos(firstTmpStartPos);
        tiling.set_secondTmpStartPos(secondTmpStartPos);
        tiling.set_loopRound(loopRound);
        tiling.set_inputRoundSize(inputRoundSize);
        tiling.set_inputTailSize(inputTailSize);
        tiling.set_inputTailPos(inputTailPos);
        tiling.set_meanVarRoundSize(meanVarRoundSize);
        tiling.set_meanVarTailSize(meanVarTailSize);
        tiling.set_meanVarTailPos(meanVarTailPos);
        tiling.set_arCurLength(arCurLength);
        tiling.set_aCurLength(aCurLength);
        tiling.set_rValueBack(rValueBack);
    }
}

void GetLayerNormNDTilingInfo(
    const AscendC::TensorShape& srcShape, const uint32_t stackBufferSize, const uint32_t typeSize,
    const bool isReuseSource, const bool isComputeRstd, AscendC::tiling::LayerNormSeparateTiling& tiling)
{
    optiling::LayerNormSeparateTiling tilingData;
    GetLayerNormNDTilingInfo(srcShape, stackBufferSize, typeSize, isReuseSource, isComputeRstd, tilingData);
    tilingData.SaveToBuffer(&tiling, sizeof(LayerNormSeparateTiling));
}
} // namespace AscendC
