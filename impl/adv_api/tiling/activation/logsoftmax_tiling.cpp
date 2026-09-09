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
 * \file softmax_tiling.cpp
 * \brief
 */

#include "adv_api/utils/types.h"
#include "adv_api/activation/logsoftmax_tiling.h"

#include <set>

#include "adv_api/activation/logsoftmax_tilingdata.h"
#include "utils/tiling/platform/platform_ascendc.h"
#include "../../detail/api_check/host_apicheck.h"

namespace optiling {
REGISTER_TILING_DATA_CLASS(LogSoftMaxTilingOpApi, LogSoftMaxTiling)
}
namespace AscendC {
constexpr uint32_t SOFTMAX_DEFAULT_BLK_SIZE = 32;
constexpr uint32_t SOFTMAX_HALF_SIZE = 2;
constexpr uint32_t SOFTMAX_FLOAT_SIZE = 4;
constexpr uint32_t BASIC_TILE_NUM = SOFTMAX_DEFAULT_BLK_SIZE / SOFTMAX_FLOAT_SIZE;
constexpr uint32_t SOFTMAX_BASICBLOCK_MIN_SIZE = 256;
constexpr uint32_t SOFTMAX_BASICBLOCK_UNIT = 64;
#define UNUSED __attribute__((unused))
static const std::set<uint32_t> SUPPORT_TYPESIZE = {SOFTMAX_HALF_SIZE, SOFTMAX_FLOAT_SIZE};
static constexpr const char LOG_SOFTMAX_GET_MAX[] = "GetLogSoftMaxMaxTmpSize";
static constexpr const char LOG_SOFTMAX_GET_MIN[] = "GetLogSoftMaxMinTmpSize";
static constexpr const char LOG_SOFTMAX_TILING[] = "LogSoftMaxTilingFunc";

inline std::vector<uint32_t> GetLastAxisShapeND(const AscendC::TensorShape srcShape)
{
    std::vector<uint32_t> ret;
    std::vector<int64_t> shapeDims = srcShape.GetDims();
    uint32_t calculateSize = 1;
    for (uint32_t i = 0; i < shapeDims.size(); i++) {
        calculateSize *= shapeDims[i];
    }

    if (shapeDims.size() >= 1) {
        const uint32_t srcK = shapeDims[shapeDims.size() - 1];
        const uint32_t srcM = calculateSize / srcK;
        ret = {srcM, srcK};
    }
    return ret;
}

inline void AdjustToBasicBlockBaseM(uint32_t& baseM, const uint32_t srcM, const uint32_t srcK)
{
    if (baseM > BASIC_TILE_NUM && srcM % BASIC_TILE_NUM == 0 && srcK % SOFTMAX_BASICBLOCK_UNIT == 0) { // basicblock
        baseM = baseM / BASIC_TILE_NUM * BASIC_TILE_NUM;
        while (srcM % baseM != 0) {
            baseM -= BASIC_TILE_NUM;
        }
        // max repeat only support 255
        while (baseM * srcK >= SOFTMAX_BASICBLOCK_UNIT * SOFTMAX_BASICBLOCK_MIN_SIZE) {
            baseM = baseM / SOFTMAX_HALF_SIZE;
        }
    }
}

uint32_t GetLogSoftMaxMaxTmpSize(
    const AscendC::TensorShape srcShape, const uint32_t dataTypeSize, UNUSED const bool isReuseSource)
{
    HighLevelApiCheck::SrcShapeSizeVerifyingParameters<LOG_SOFTMAX_GET_MAX>(srcShape.GetShapeSize(), dataTypeSize);
    HighLevelApiCheck::ShapeLastAxisAlignVerifyingParameters<LOG_SOFTMAX_GET_MAX>(
        srcShape, dataTypeSize, SOFTMAX_DEFAULT_BLK_SIZE);
    HighLevelApiCheck::TypeSizeVerifyingParameters<LOG_SOFTMAX_GET_MAX>(dataTypeSize, SUPPORT_TYPESIZE);
    HighLevelApiCheck::IsReuseSourceVerifyingParameters<LOG_SOFTMAX_GET_MAX>(isReuseSource);
    std::vector<uint32_t> retVec = GetLastAxisShapeND(srcShape);
    // the softmax shape size must be 2
    if (retVec.size() <= 1 || dataTypeSize == 0) {
        return 0;
    }
    const uint32_t srcM = retVec[0];
    const uint32_t srcK = retVec[1];
    const uint32_t elementNumPerBlk = SOFTMAX_DEFAULT_BLK_SIZE / dataTypeSize;

    platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
    ASCENDC_HOST_ASSERT((platform != nullptr), return 0, "Failed to get PlatformAscendC.");
    const auto npuArch = platform->GetCurNpuArch();

    uint32_t needSize;
    if (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201 || npuArch == NpuArch::DAV_5102) {
        uint32_t needSize1 = srcM * (BASIC_TILE_NUM + srcK) + SOFTMAX_BASICBLOCK_UNIT * SOFTMAX_FLOAT_SIZE +
                             (srcM + BASIC_TILE_NUM - 1) / BASIC_TILE_NUM * BASIC_TILE_NUM;
        uint32_t needSize2 = srcM * (elementNumPerBlk + srcK + SOFTMAX_BASICBLOCK_UNIT);
        needSize = std::max(needSize1, needSize2);
    } else {
        needSize = srcM * (elementNumPerBlk + srcK + SOFTMAX_BASICBLOCK_UNIT);
    }
    return needSize * SOFTMAX_FLOAT_SIZE;
}

uint32_t GetLogSoftMaxMinTmpSize(
    const AscendC::TensorShape srcShape, const uint32_t dataTypeSize, UNUSED const bool isReuseSource)
{
    HighLevelApiCheck::SrcShapeSizeVerifyingParameters<LOG_SOFTMAX_GET_MIN>(srcShape.GetShapeSize(), dataTypeSize);
    HighLevelApiCheck::ShapeLastAxisAlignVerifyingParameters<LOG_SOFTMAX_GET_MIN>(
        srcShape, dataTypeSize, SOFTMAX_DEFAULT_BLK_SIZE);
    HighLevelApiCheck::TypeSizeVerifyingParameters<LOG_SOFTMAX_GET_MIN>(dataTypeSize, SUPPORT_TYPESIZE);
    HighLevelApiCheck::IsReuseSourceVerifyingParameters<LOG_SOFTMAX_GET_MIN>(isReuseSource);
    std::vector<uint32_t> retVec = GetLastAxisShapeND(srcShape);
    // the softmax shape size must be 2
    if (retVec.size() <= 1 || dataTypeSize == 0) {
        return 0;
    }
    const uint32_t srcM = retVec[0];
    const uint32_t srcK = retVec[1];
    const uint32_t elementNumPerBlk = SOFTMAX_DEFAULT_BLK_SIZE / dataTypeSize;

    platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
    ASCENDC_HOST_ASSERT((platform != nullptr), return 0, "Failed to get PlatformAscendC.");
    const auto npuArch = platform->GetCurNpuArch();

    uint32_t needSize;
    if (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201 || npuArch == NpuArch::DAV_5102) {
        uint32_t needSize1 = srcM * (BASIC_TILE_NUM + srcK) + SOFTMAX_BASICBLOCK_UNIT * SOFTMAX_FLOAT_SIZE +
                             (srcM + BASIC_TILE_NUM - 1) / BASIC_TILE_NUM * BASIC_TILE_NUM;
        uint32_t needSize2 = srcM * (elementNumPerBlk + srcK);
        needSize = std::max(needSize1, needSize2);
    } else {
        needSize = elementNumPerBlk + srcK + SOFTMAX_BASICBLOCK_UNIT;
    }
    return needSize * SOFTMAX_FLOAT_SIZE;
}

void LogSoftMaxTilingFunc(
    const AscendC::TensorShape srcShape, const uint32_t dataTypeSize, const uint32_t localWorkSpaceSize,
    optiling::LogSoftMaxTiling& softmaxTiling)
{
    HighLevelApiCheck::SrcShapeSizeVerifyingParameters<LOG_SOFTMAX_TILING>(srcShape.GetShapeSize(), dataTypeSize);
    HighLevelApiCheck::ShapeLastAxisAlignVerifyingParameters<LOG_SOFTMAX_TILING>(
        srcShape, dataTypeSize, SOFTMAX_DEFAULT_BLK_SIZE);
    HighLevelApiCheck::TypeSizeVerifyingParameters<LOG_SOFTMAX_TILING>(dataTypeSize, SUPPORT_TYPESIZE);
    HighLevelApiCheck::LocalWorkSpaceSizeVerifyingParameters<LOG_SOFTMAX_TILING>(localWorkSpaceSize);
    std::vector<uint32_t> retVec = GetLastAxisShapeND(srcShape);
    if (retVec.size() <= 1 || dataTypeSize == 0) {
        return;
    }
    const uint32_t elementNumPerBlk = SOFTMAX_DEFAULT_BLK_SIZE / dataTypeSize;
    const uint32_t workLocalSize = localWorkSpaceSize / SOFTMAX_FLOAT_SIZE;
    const uint32_t srcK = retVec[1];
    const uint32_t srcM = retVec[0];
    uint32_t baseM = std::min(workLocalSize / (elementNumPerBlk + srcK + SOFTMAX_BASICBLOCK_UNIT), srcM);
    if (baseM < srcM && baseM > BASIC_TILE_NUM) {
        baseM = baseM / BASIC_TILE_NUM * BASIC_TILE_NUM;
    }

    AdjustToBasicBlockBaseM(baseM, srcM, srcK);

    softmaxTiling.set_srcM(srcM);
    softmaxTiling.set_srcK(srcK);
    softmaxTiling.set_srcSize(srcM * srcK);

    softmaxTiling.set_outMaxM(srcM);             // output dstMax
    softmaxTiling.set_outMaxK(elementNumPerBlk); // output dstMax
    softmaxTiling.set_outMaxSize(srcM * elementNumPerBlk);

    softmaxTiling.set_splitM(baseM);
    softmaxTiling.set_splitK(srcK);
    softmaxTiling.set_splitSize(baseM * srcK);

    softmaxTiling.set_reduceM(baseM);
    softmaxTiling.set_reduceK(elementNumPerBlk);
    softmaxTiling.set_reduceSize(baseM * elementNumPerBlk);

    const uint32_t range = srcM / baseM;
    const uint32_t tail = srcM % baseM;
    softmaxTiling.set_rangeM(range);
    softmaxTiling.set_tailM(tail);

    softmaxTiling.set_tailSplitSize(tail * srcK);
    softmaxTiling.set_tailReduceSize(tail * elementNumPerBlk);
}

void LogSoftMaxTilingFunc(
    const AscendC::TensorShape srcShape, const uint32_t dataTypeSize, const uint32_t localWorkSpaceSize,
    AscendC::tiling::LogSoftMaxTiling& softmaxTiling)
{
    optiling::LogSoftMaxTiling tiling;
    LogSoftMaxTilingFunc(srcShape, dataTypeSize, localWorkSpaceSize, tiling);
    tiling.SaveToBuffer(&softmaxTiling, sizeof(LogSoftMaxTiling));
}
} // namespace AscendC
