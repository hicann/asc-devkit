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

#include <set>

#include "adv_api/utils/types.h"
#include "adv_api/activation/softmax_tilingdata.h"
#include "adv_api/activation/softmax_tiling.h"
#include "utils/tiling/platform/platform_ascendc.h"
#include "../../detail/host_log.h"
#include "../../detail/api_check/host_apicheck.h"

namespace optiling {
REGISTER_TILING_DATA_CLASS(SoftMaxTilingOpApi, SoftMaxTiling)
}
namespace AscendC {
constexpr uint32_t SOFTMAX_DEFAULT_BLK_SIZE = 32;
constexpr uint32_t SOFTMAX_TMPBUFFER_COUNT = 2;
constexpr uint32_t SOFTMAX_TMPFLASHUPDATE_COUNT = 4;
constexpr uint32_t SOFTMAX_HALF_SIZE = 2;
constexpr uint32_t SOFTMAX_FLOAT_SIZE = 4;
constexpr uint32_t SOFTMAXGRAD_TMPBUFFER_COUNT = 3;
constexpr uint32_t BASIC_TILE_NUM = SOFTMAX_DEFAULT_BLK_SIZE / SOFTMAX_FLOAT_SIZE;
constexpr uint32_t SOFTMAX_BASICBLOCK_MIN_SIZE = 256;
constexpr uint32_t SOFTMAX_BASICBLOCK_UNIT = 64;
constexpr uint32_t SOFTMAX_SPECIAL_BASICBLOCK_LEN = SOFTMAX_BASICBLOCK_MIN_SIZE * SOFTMAX_FLOAT_SIZE;
constexpr uint32_t SOFTMAXV3_TMPBUFFER_COUNT = 5;
static const std::set<uint32_t> SUPPORT_TYPESIZE = {SOFTMAX_HALF_SIZE, SOFTMAX_FLOAT_SIZE};
static constexpr const char SOFTMAX_GET_MAX[] = "GetSoftMaxMaxTmpSize";
static constexpr const char SOFTMAX_GET_MIN[] = "GetSoftMaxMinTmpSize";
static constexpr const char SOFTMAX_TILING[] = "SoftMaxTilingFunc";

static constexpr const char SOFTMAX_FLASH_GET_MAX[] = "GetSoftMaxFlashMaxTmpSize";
static constexpr const char SOFTMAX_FLASH_GET_MIN[] = "GetSoftMaxFlashMinTmpSize";
static constexpr const char SOFTMAX_FLASH_TILING[] = "SoftMaxFlashTilingFunc";

static constexpr const char SOFTMAX_GRAD_GET_MAX[] = "GetSoftMaxGradMaxTmpSize";
static constexpr const char SOFTMAX_GRAD_GET_MIN[] = "GetSoftMaxGradMinTmpSize";
static constexpr const char SOFTMAX_GRAD_TILING[] = "SoftMaxGradTilingFunc";

static constexpr const char SOFTMAX_FLASH_V2_GET_MAX[] = "GetSoftMaxFlashV2MaxTmpSize";
static constexpr const char SOFTMAX_FLASH_V2_GET_MIN[] = "GetSoftMaxFlashV2MinTmpSize";
static constexpr const char SOFTMAX_FLASH_V2_TILING[] = "SoftMaxFlashV2TilingFunc";

static constexpr const char SOFTMAX_FLASH_V3_GET_MAX_MIN[] = "GetSoftMaxFlashV3MaxMinTmpSize";
static constexpr const char SOFTMAX_FLASH_V3_TILING[] = "SoftMaxFlashV3TilingFunc";

#define UNUSED __attribute__((unused))

inline std::vector<uint32_t> GetLastAxisShapeND(const AscendC::TensorShape& srcShape)
{
    std::vector<uint32_t> ret;
    std::vector<int64_t> shapeDims = srcShape.GetDims();
    uint32_t calculateSize = 1;
    for (uint32_t i = 0; i < shapeDims.size(); i++) {
        calculateSize *= shapeDims[i];
    }

    const uint32_t srcK = shapeDims.back();
    uint32_t srcM = calculateSize / srcK;
    ret = {srcM, srcK};
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

uint32_t GetSoftMaxMaxTmpSize(
    const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize, UNUSED const bool isReuseSource)
{
    HighLevelApiCheck::SrcShapeSizeVerifyingParameters<SOFTMAX_GET_MAX>(srcShape.GetShapeSize(), dataTypeSize);
    HighLevelApiCheck::ShapeLastAxisAlignVerifyingParameters<SOFTMAX_GET_MAX>(
        srcShape, dataTypeSize, SOFTMAX_DEFAULT_BLK_SIZE);
    HighLevelApiCheck::TypeSizeVerifyingParameters<SOFTMAX_GET_MAX>(dataTypeSize, SUPPORT_TYPESIZE);
    HighLevelApiCheck::IsReuseSourceVerifyingParameters<SOFTMAX_GET_MAX>(isReuseSource);
    const uint32_t inputSize = srcShape.GetShapeSize();
    ASCENDC_HOST_ASSERT(inputSize > 0, return 0, "input srcShape size must be greater than 0.");
    ASCENDC_HOST_ASSERT(
        (dataTypeSize != 0) && (dataTypeSize == SOFTMAX_HALF_SIZE || dataTypeSize == SOFTMAX_FLOAT_SIZE), return 0,
        "dataTypeSize input error, dataType now only support half or float.");

    std::vector<uint32_t> retVec = GetLastAxisShapeND(srcShape);
    const uint32_t srcM = retVec[0];
    const uint32_t srcK = retVec[1];
    const uint32_t elementNumPerBlk = SOFTMAX_DEFAULT_BLK_SIZE / dataTypeSize;
    platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
    ASCENDC_HOST_ASSERT((platform != nullptr), return 0, "Failed to get PlatformAscendC.");
    const auto npuArch = platform->GetCurNpuArch();

    uint32_t needSize;
    if (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_3003 || npuArch == NpuArch::DAV_5102) {
        uint32_t needSize1 = srcM * (BASIC_TILE_NUM + srcK) + SOFTMAX_BASICBLOCK_UNIT * SOFTMAX_TMPFLASHUPDATE_COUNT +
                             (srcM + BASIC_TILE_NUM - 1) / BASIC_TILE_NUM * BASIC_TILE_NUM;
        uint32_t needSize2 = srcM * (elementNumPerBlk + srcK + SOFTMAX_BASICBLOCK_UNIT);
        needSize = std::max(needSize1, needSize2);
    } else {
        needSize = srcM * (elementNumPerBlk + srcK + SOFTMAX_BASICBLOCK_UNIT);
    }
    return needSize * SOFTMAX_FLOAT_SIZE;
}

uint32_t GetSoftMaxMinTmpSize(
    const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize, UNUSED const bool isReuseSource)
{
    HighLevelApiCheck::SrcShapeSizeVerifyingParameters<SOFTMAX_GET_MIN>(srcShape.GetShapeSize(), dataTypeSize);
    HighLevelApiCheck::ShapeLastAxisAlignVerifyingParameters<SOFTMAX_GET_MIN>(
        srcShape, dataTypeSize, SOFTMAX_DEFAULT_BLK_SIZE);
    HighLevelApiCheck::TypeSizeVerifyingParameters<SOFTMAX_GET_MIN>(dataTypeSize, SUPPORT_TYPESIZE);
    HighLevelApiCheck::IsReuseSourceVerifyingParameters<SOFTMAX_GET_MIN>(isReuseSource);
    const uint32_t inputSize = srcShape.GetShapeSize();
    ASCENDC_HOST_ASSERT(inputSize > 0, return 0, "input srcShape size must be greater than 0.");
    ASCENDC_HOST_ASSERT(
        (dataTypeSize != 0) && (dataTypeSize == SOFTMAX_HALF_SIZE || dataTypeSize == SOFTMAX_FLOAT_SIZE), return 0,
        "dataTypeSize input error, dataType now only support half or float.");

    std::vector<uint32_t> retVec = GetLastAxisShapeND(srcShape);
    const uint32_t srcM = retVec[0];
    const uint32_t srcK = retVec[1];
    const uint32_t elementNumPerBlk = SOFTMAX_DEFAULT_BLK_SIZE / dataTypeSize;
    platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
    ASCENDC_HOST_ASSERT((platform != nullptr), return 0, "Failed to get PlatformAscendC.");
    const auto npuArch = platform->GetCurNpuArch();

    uint32_t needSize;
    if (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_3003 || npuArch == NpuArch::DAV_5102) {
        uint32_t needSize1 = srcM * (BASIC_TILE_NUM + srcK) + SOFTMAX_BASICBLOCK_UNIT * SOFTMAX_TMPFLASHUPDATE_COUNT +
                             (srcM + BASIC_TILE_NUM - 1) / BASIC_TILE_NUM * BASIC_TILE_NUM;
        uint32_t needSize2 = srcM * (elementNumPerBlk + srcK);
        needSize = std::max(needSize1, needSize2);
    } else {
        needSize = elementNumPerBlk + srcK + SOFTMAX_BASICBLOCK_UNIT;
    }
    return needSize * SOFTMAX_FLOAT_SIZE;
}

void SoftMaxTilingFunc(
    const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize, const uint32_t localWorkSpaceSize,
    optiling::SoftMaxTiling& softmaxTiling)
{
    HighLevelApiCheck::SrcShapeSizeVerifyingParameters<SOFTMAX_TILING>(srcShape.GetShapeSize(), dataTypeSize);
    HighLevelApiCheck::ShapeLastAxisAlignVerifyingParameters<SOFTMAX_TILING>(
        srcShape, dataTypeSize, SOFTMAX_DEFAULT_BLK_SIZE);
    HighLevelApiCheck::TypeSizeVerifyingParameters<SOFTMAX_TILING>(dataTypeSize, SUPPORT_TYPESIZE);
    HighLevelApiCheck::LocalWorkSpaceSizeVerifyingParameters<SOFTMAX_TILING>(localWorkSpaceSize);
    const uint32_t inputSize = srcShape.GetShapeSize();
    ASCENDC_HOST_ASSERT(inputSize > 0, return, "input srcShape size must be greater than 0.");
    ASCENDC_HOST_ASSERT(
        (dataTypeSize != 0) && (dataTypeSize == SOFTMAX_HALF_SIZE || dataTypeSize == SOFTMAX_FLOAT_SIZE), return,
        "dataTypeSize input error, dataType now only support half or float.");

    std::vector<uint32_t> retVec = GetLastAxisShapeND(srcShape);
    const uint32_t srcM = retVec[0];
    const uint32_t srcK = retVec[1];
    const uint32_t elementNumPerBlk = SOFTMAX_DEFAULT_BLK_SIZE / dataTypeSize;
    const uint32_t workLocalSize = localWorkSpaceSize / SOFTMAX_FLOAT_SIZE;
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

    uint32_t tail = 0;
    if (baseM != 0) {
        tail = srcM % baseM;
        softmaxTiling.set_rangeM(srcM / baseM);
    }
    softmaxTiling.set_tailM(tail);

    softmaxTiling.set_tailSplitSize(tail * srcK);
    softmaxTiling.set_tailReduceSize(tail * elementNumPerBlk);
}

void SoftMaxTilingFunc(
    const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize, const uint32_t localWorkSpaceSize,
    AscendC::tiling::SoftMaxTiling& softmaxTiling)
{
    optiling::SoftMaxTiling tiling;
    SoftMaxTilingFunc(srcShape, dataTypeSize, localWorkSpaceSize, tiling);
    tiling.SaveToBuffer(&softmaxTiling, sizeof(SoftMaxTiling));
}

uint32_t GetSoftMaxFlashMaxTmpSize(
    const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize, const bool isUpdate,
    UNUSED const bool isReuseSource)
{
    HighLevelApiCheck::SrcShapeSizeVerifyingParameters<SOFTMAX_FLASH_GET_MAX>(srcShape.GetShapeSize(), dataTypeSize);
    HighLevelApiCheck::ShapeLastAxisAlignVerifyingParameters<SOFTMAX_FLASH_GET_MAX>(
        srcShape, dataTypeSize, SOFTMAX_DEFAULT_BLK_SIZE);
    HighLevelApiCheck::TypeSizeVerifyingParameters<SOFTMAX_FLASH_GET_MAX>(dataTypeSize, SUPPORT_TYPESIZE);
    HighLevelApiCheck::IsReuseSourceVerifyingParameters<SOFTMAX_FLASH_GET_MAX>(isReuseSource);
    const uint32_t inputSize = srcShape.GetShapeSize();
    ASCENDC_HOST_ASSERT(inputSize > 0, return 0, "input srcShape size must be greater than 0.");
    ASCENDC_HOST_ASSERT(
        (dataTypeSize != 0) && (dataTypeSize == SOFTMAX_HALF_SIZE || dataTypeSize == SOFTMAX_FLOAT_SIZE), return 0,
        "dataTypeSize input error, dataType now only support half or float.");

    std::vector<uint32_t> retVec = GetLastAxisShapeND(srcShape);
    const uint32_t srcK = retVec[1];
    const uint32_t srcM = retVec[0];
    const uint32_t elementNumPerBlk = SOFTMAX_DEFAULT_BLK_SIZE / dataTypeSize;

    platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
    ASCENDC_HOST_ASSERT((platform != nullptr), return 0, "Failed to get PlatformAscendC.");
    const auto npuArch = platform->GetCurNpuArch();

    uint32_t needSize;
    if (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_5102) {
        uint32_t needSize2 = srcM * (elementNumPerBlk + srcK + SOFTMAX_BASICBLOCK_UNIT);
        if (!isUpdate) {
            uint32_t needSize1 = srcM * (BASIC_TILE_NUM + srcK) + SOFTMAX_BASICBLOCK_UNIT * SOFTMAX_FLOAT_SIZE +
                                 (srcM + BASIC_TILE_NUM - 1) / BASIC_TILE_NUM * BASIC_TILE_NUM;
            needSize = std::max(needSize1, needSize2);
        } else {
            needSize = needSize2;
        }
    } else {
        needSize = !isUpdate ?
                       srcM * (elementNumPerBlk + srcK + SOFTMAX_BASICBLOCK_UNIT) :
                       srcM * (elementNumPerBlk * SOFTMAX_TMPFLASHUPDATE_COUNT + srcK * SOFTMAX_TMPBUFFER_COUNT);
    }

    return needSize * SOFTMAX_FLOAT_SIZE;
}

uint32_t GetSoftMaxFlashMinTmpSize(
    const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize, const bool isUpdate,
    UNUSED const bool isReuseSource)
{
    HighLevelApiCheck::SrcShapeSizeVerifyingParameters<SOFTMAX_FLASH_GET_MIN>(srcShape.GetShapeSize(), dataTypeSize);
    HighLevelApiCheck::ShapeLastAxisAlignVerifyingParameters<SOFTMAX_FLASH_GET_MIN>(
        srcShape, dataTypeSize, SOFTMAX_DEFAULT_BLK_SIZE);
    HighLevelApiCheck::TypeSizeVerifyingParameters<SOFTMAX_FLASH_GET_MIN>(dataTypeSize, SUPPORT_TYPESIZE);
    HighLevelApiCheck::IsReuseSourceVerifyingParameters<SOFTMAX_FLASH_GET_MIN>(isReuseSource);
    const uint32_t inputSize = srcShape.GetShapeSize();
    ASCENDC_HOST_ASSERT(inputSize > 0, return 0, "input srcShape size must be greater than 0.");
    ASCENDC_HOST_ASSERT(
        (dataTypeSize != 0) && (dataTypeSize == SOFTMAX_HALF_SIZE || dataTypeSize == SOFTMAX_FLOAT_SIZE), return 0,
        "dataTypeSize input error, dataType now only support half or float.");

    std::vector<uint32_t> retVec = GetLastAxisShapeND(srcShape);
    const uint32_t elementNumPerBlk = SOFTMAX_DEFAULT_BLK_SIZE / dataTypeSize;
    const uint32_t srcM = retVec[0];
    const uint32_t srcK = retVec[1];

    platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
    ASCENDC_HOST_ASSERT((platform != nullptr), return 0, "Failed to get PlatformAscendC.");
    const auto npuArch = platform->GetCurNpuArch();

    uint32_t needSize;
    if (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_5102) {
        uint32_t needSize2 = srcM * (elementNumPerBlk + srcK);
        if (!isUpdate) {
            uint32_t needSize1 = srcM * (BASIC_TILE_NUM + srcK) + SOFTMAX_BASICBLOCK_UNIT * SOFTMAX_FLOAT_SIZE +
                                 (srcM + BASIC_TILE_NUM - 1) / BASIC_TILE_NUM * BASIC_TILE_NUM;
            needSize = std::max(needSize1, needSize2);
        } else {
            needSize = needSize2;
        }
    } else {
        needSize = !isUpdate ? elementNumPerBlk + srcK + SOFTMAX_BASICBLOCK_UNIT :
                               elementNumPerBlk * SOFTMAX_TMPFLASHUPDATE_COUNT + srcK * SOFTMAX_TMPBUFFER_COUNT;
    }

    return needSize * SOFTMAX_FLOAT_SIZE;
}

void SoftMaxFlashTilingFunc(
    const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize, const uint32_t localWorkSpaceSize,
    optiling::SoftMaxTiling& softmaxFlashTiling, const bool isUpdate)
{
    HighLevelApiCheck::SrcShapeSizeVerifyingParameters<SOFTMAX_FLASH_TILING>(srcShape.GetShapeSize(), dataTypeSize);
    HighLevelApiCheck::ShapeLastAxisAlignVerifyingParameters<SOFTMAX_FLASH_TILING>(
        srcShape, dataTypeSize, SOFTMAX_DEFAULT_BLK_SIZE);
    HighLevelApiCheck::TypeSizeVerifyingParameters<SOFTMAX_FLASH_TILING>(dataTypeSize, SUPPORT_TYPESIZE);
    HighLevelApiCheck::LocalWorkSpaceSizeVerifyingParameters<SOFTMAX_FLASH_TILING>(localWorkSpaceSize);
    const uint32_t inputSize = srcShape.GetShapeSize();
    ASCENDC_HOST_ASSERT(inputSize > 0, return, "input srcShape size must be greater than 0.");
    ASCENDC_HOST_ASSERT(
        (dataTypeSize != 0) && (dataTypeSize == SOFTMAX_HALF_SIZE || dataTypeSize == SOFTMAX_FLOAT_SIZE), return,
        "dataTypeSize input error, dataType now only support half or float.");

    std::vector<uint32_t> retVec = GetLastAxisShapeND(srcShape);
    const uint32_t elementNumPerBlk = SOFTMAX_DEFAULT_BLK_SIZE / dataTypeSize;
    const uint32_t workLocalSize = localWorkSpaceSize / SOFTMAX_FLOAT_SIZE;
    const uint32_t srcM = retVec[0];
    const uint32_t srcK = retVec[1];
    uint32_t baseM = 0;
    baseM = !isUpdate ?
                workLocalSize / (elementNumPerBlk + srcK + SOFTMAX_BASICBLOCK_UNIT) :
                workLocalSize / (elementNumPerBlk * SOFTMAX_TMPFLASHUPDATE_COUNT + srcK * SOFTMAX_TMPBUFFER_COUNT);
    baseM = std::min(baseM, srcM);
    if (baseM < srcM && baseM > BASIC_TILE_NUM) {
        baseM = baseM / BASIC_TILE_NUM * BASIC_TILE_NUM;
    }

    AdjustToBasicBlockBaseM(baseM, srcM, srcK);

    softmaxFlashTiling.set_srcM(srcM);
    softmaxFlashTiling.set_srcK(srcK);
    softmaxFlashTiling.set_srcSize(srcM * srcK);

    softmaxFlashTiling.set_outMaxM(srcM);             // output dstMax
    softmaxFlashTiling.set_outMaxK(elementNumPerBlk); // output dstMax
    softmaxFlashTiling.set_outMaxSize(srcM * elementNumPerBlk);

    softmaxFlashTiling.set_splitM(baseM);
    softmaxFlashTiling.set_splitK(srcK);
    softmaxFlashTiling.set_splitSize(baseM * srcK);

    softmaxFlashTiling.set_reduceM(baseM);
    softmaxFlashTiling.set_reduceK(elementNumPerBlk);
    softmaxFlashTiling.set_reduceSize(baseM * elementNumPerBlk);

    uint32_t tail = 0;
    if (baseM != 0) {
        softmaxFlashTiling.set_rangeM(srcM / baseM);
        tail = srcM % baseM;
    }
    softmaxFlashTiling.set_tailM(tail);
    softmaxFlashTiling.set_tailSplitSize(tail * srcK);
    softmaxFlashTiling.set_tailReduceSize(tail * elementNumPerBlk);
}

void SoftMaxFlashTilingFunc(
    const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize, const uint32_t localWorkSpaceSize,
    AscendC::tiling::SoftMaxTiling& softmaxFlashTiling, const bool isUpdate)
{
    optiling::SoftMaxTiling tiling;
    SoftMaxFlashTilingFunc(srcShape, dataTypeSize, localWorkSpaceSize, tiling, isUpdate);
    tiling.SaveToBuffer(&softmaxFlashTiling, sizeof(SoftMaxTiling));
}

uint32_t GetSoftMaxGradMaxTmpSize(
    const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize, const bool isFront,
    UNUSED const bool isReuseSource)
{
    HighLevelApiCheck::SrcShapeSizeVerifyingParameters<SOFTMAX_GRAD_GET_MAX>(srcShape.GetShapeSize(), dataTypeSize);
    HighLevelApiCheck::ShapeLastAxisAlignVerifyingParameters<SOFTMAX_GRAD_GET_MAX>(
        srcShape, dataTypeSize, SOFTMAX_DEFAULT_BLK_SIZE);
    HighLevelApiCheck::TypeSizeVerifyingParameters<SOFTMAX_GRAD_GET_MAX>(dataTypeSize, SUPPORT_TYPESIZE);
    HighLevelApiCheck::IsReuseSourceVerifyingParameters<SOFTMAX_GRAD_GET_MAX>(isReuseSource);
    const uint32_t inputSize = srcShape.GetShapeSize();
    ASCENDC_HOST_ASSERT(inputSize > 0, return 0, "input srcShape size must be greater than 0.");
    ASCENDC_HOST_ASSERT(
        (dataTypeSize != 0) && (dataTypeSize == SOFTMAX_HALF_SIZE || dataTypeSize == SOFTMAX_FLOAT_SIZE), return 0,
        "dataTypeSize input error, dataType now only support half or float.");

    std::vector<uint32_t> retVec = GetLastAxisShapeND(srcShape);
    const uint32_t srcM = retVec[0];
    const uint32_t srcK = retVec[1];
    const uint32_t elementNumPerBlk = SOFTMAX_DEFAULT_BLK_SIZE / dataTypeSize;
    uint32_t needSize = 0;
    if (dataTypeSize == SOFTMAX_HALF_SIZE) {
        needSize = srcM * (elementNumPerBlk * SOFTMAX_TMPBUFFER_COUNT + srcK * SOFTMAXGRAD_TMPBUFFER_COUNT +
                           SOFTMAX_BASICBLOCK_UNIT);
    } else {
        needSize = isFront ? srcM * (elementNumPerBlk + srcK + SOFTMAX_BASICBLOCK_UNIT) :
                             srcM * (elementNumPerBlk * SOFTMAX_TMPBUFFER_COUNT + srcK + SOFTMAX_BASICBLOCK_UNIT);
    }
    return needSize * SOFTMAX_FLOAT_SIZE;
}

uint32_t GetSoftMaxGradMinTmpSize(
    const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize, const bool isFront,
    UNUSED const bool isReuseSource)
{
    HighLevelApiCheck::SrcShapeSizeVerifyingParameters<SOFTMAX_GRAD_GET_MIN>(srcShape.GetShapeSize(), dataTypeSize);
    HighLevelApiCheck::ShapeLastAxisAlignVerifyingParameters<SOFTMAX_GRAD_GET_MIN>(
        srcShape, dataTypeSize, SOFTMAX_DEFAULT_BLK_SIZE);
    HighLevelApiCheck::TypeSizeVerifyingParameters<SOFTMAX_GRAD_GET_MIN>(dataTypeSize, SUPPORT_TYPESIZE);
    HighLevelApiCheck::IsReuseSourceVerifyingParameters<SOFTMAX_GRAD_GET_MIN>(isReuseSource);
    const uint32_t inputSize = srcShape.GetShapeSize();
    ASCENDC_HOST_ASSERT(inputSize > 0, return 0, "input srcShape size must be greater than 0.");
    ASCENDC_HOST_ASSERT(
        (dataTypeSize != 0) && (dataTypeSize == SOFTMAX_HALF_SIZE || dataTypeSize == SOFTMAX_FLOAT_SIZE), return 0,
        "dataTypeSize input error, dataType now only support half or float.");

    std::vector<uint32_t> retVec = GetLastAxisShapeND(srcShape);
    const uint32_t srcM = retVec[0];
    const uint32_t srcK = retVec[1];
    const uint32_t elementNumPerBlk = SOFTMAX_DEFAULT_BLK_SIZE / dataTypeSize;
    uint32_t needSize = 0;

    platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
    ASCENDC_HOST_ASSERT((platform != nullptr), return 0, "Failed to get PlatformAscendC.");
    const auto npuArch = platform->GetCurNpuArch();
    if (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_5102) {
        uint32_t needSize1 = (srcM * SOFTMAX_TMPBUFFER_COUNT + SOFTMAX_BASICBLOCK_UNIT * SOFTMAX_TMPBUFFER_COUNT - 1) /
                             (SOFTMAX_BASICBLOCK_UNIT * SOFTMAX_TMPBUFFER_COUNT) *
                             (SOFTMAX_BASICBLOCK_UNIT * SOFTMAX_TMPBUFFER_COUNT);
        uint32_t needSize2 = (srcM + SOFTMAX_BASICBLOCK_UNIT - 1) / SOFTMAX_BASICBLOCK_UNIT * SOFTMAX_BASICBLOCK_UNIT;
        needSize = isFront ? needSize2 : needSize1;
    } else {
        if (dataTypeSize == SOFTMAX_HALF_SIZE) {
            needSize = elementNumPerBlk * SOFTMAX_TMPBUFFER_COUNT + srcK * SOFTMAXGRAD_TMPBUFFER_COUNT +
                       SOFTMAX_BASICBLOCK_UNIT;
        } else {
            needSize = isFront ? elementNumPerBlk + srcK + SOFTMAX_BASICBLOCK_UNIT :
                                 elementNumPerBlk * SOFTMAX_TMPBUFFER_COUNT + srcK + SOFTMAX_BASICBLOCK_UNIT;
        }
    }
    return needSize * SOFTMAX_FLOAT_SIZE;
}

void SoftMaxGradTilingFunc(
    const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize, const uint32_t localWorkSpaceSize,
    optiling::SoftMaxTiling& softmaxGradTiling, const bool isFront)
{
    HighLevelApiCheck::SrcShapeSizeVerifyingParameters<SOFTMAX_GRAD_TILING>(srcShape.GetShapeSize(), dataTypeSize);
    HighLevelApiCheck::ShapeLastAxisAlignVerifyingParameters<SOFTMAX_GRAD_TILING>(
        srcShape, dataTypeSize, SOFTMAX_DEFAULT_BLK_SIZE);
    HighLevelApiCheck::TypeSizeVerifyingParameters<SOFTMAX_GRAD_TILING>(dataTypeSize, SUPPORT_TYPESIZE);
    HighLevelApiCheck::LocalWorkSpaceSizeVerifyingParameters<SOFTMAX_GRAD_TILING>(localWorkSpaceSize);
    const uint32_t inputSize = srcShape.GetShapeSize();
    ASCENDC_HOST_ASSERT(inputSize > 0, return, "input srcShape size must be greater than 0.");
    ASCENDC_HOST_ASSERT(
        (dataTypeSize != 0) && (dataTypeSize == SOFTMAX_HALF_SIZE || dataTypeSize == SOFTMAX_FLOAT_SIZE), return,
        "dataTypeSize input error, dataType now only support half or float.");

    std::vector<uint32_t> retVec = GetLastAxisShapeND(srcShape);
    const uint32_t elementNumPerBlk = SOFTMAX_DEFAULT_BLK_SIZE / dataTypeSize;
    const uint32_t workLocalSize = localWorkSpaceSize / SOFTMAX_FLOAT_SIZE;
    const uint32_t srcK = retVec[1];
    const uint32_t srcM = retVec[0];
    uint32_t baseM = 0;
    if (dataTypeSize == SOFTMAX_HALF_SIZE) {
        baseM = workLocalSize / (elementNumPerBlk * SOFTMAX_TMPBUFFER_COUNT + srcK * SOFTMAXGRAD_TMPBUFFER_COUNT +
                                 SOFTMAX_BASICBLOCK_UNIT);
    } else {
        baseM = isFront ? workLocalSize / (elementNumPerBlk + srcK + SOFTMAX_BASICBLOCK_UNIT) :
                          workLocalSize / (elementNumPerBlk * SOFTMAX_TMPBUFFER_COUNT + srcK + SOFTMAX_BASICBLOCK_UNIT);
    }

    baseM = std::min(baseM, srcM);
    if (baseM < srcM && baseM > BASIC_TILE_NUM) {
        baseM = baseM / BASIC_TILE_NUM * BASIC_TILE_NUM;
    }

    AdjustToBasicBlockBaseM(baseM, srcM, srcK);

    softmaxGradTiling.set_srcM(srcM);
    softmaxGradTiling.set_srcK(srcK);
    softmaxGradTiling.set_srcSize(srcM * srcK);

    softmaxGradTiling.set_outMaxM(srcM);
    softmaxGradTiling.set_outMaxK(elementNumPerBlk);
    softmaxGradTiling.set_outMaxSize(srcM * elementNumPerBlk);

    softmaxGradTiling.set_splitM(baseM);
    softmaxGradTiling.set_splitK(srcK);
    softmaxGradTiling.set_splitSize(baseM * srcK);

    softmaxGradTiling.set_reduceM(baseM);
    softmaxGradTiling.set_reduceK(elementNumPerBlk);
    softmaxGradTiling.set_reduceSize(baseM * elementNumPerBlk);

    uint32_t tail = 0;
    if (baseM != 0) {
        softmaxGradTiling.set_rangeM(srcM / baseM);
        tail = srcM % baseM;
    }
    softmaxGradTiling.set_tailM(tail);

    softmaxGradTiling.set_tailSplitSize(tail * srcK);
    softmaxGradTiling.set_tailReduceSize(tail * elementNumPerBlk);
}

void SoftMaxGradTilingFunc(
    const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize, const uint32_t localWorkSpaceSize,
    AscendC::tiling::SoftMaxTiling& softmaxGradTiling, const bool isFront)
{
    optiling::SoftMaxTiling tiling;
    SoftMaxGradTilingFunc(srcShape, dataTypeSize, localWorkSpaceSize, tiling, isFront);
    tiling.SaveToBuffer(&softmaxGradTiling, sizeof(SoftMaxTiling));
}

bool IsBasicBlockInSoftMax(optiling::SoftMaxTiling& tiling, const uint32_t dataTypeSize)
{
    constexpr uint32_t SOFTMAX_BASICBLOCK_MAX_SIZE = 2048;
    if (dataTypeSize == 0) {
        return false;
    }
    const uint32_t srcK = tiling.get_srcK();
    const uint32_t tailM = tiling.get_tailM();
    if (tailM == 0 && srcK >= (SOFTMAX_BASICBLOCK_MIN_SIZE / dataTypeSize) && srcK < SOFTMAX_BASICBLOCK_MAX_SIZE &&
        srcK % SOFTMAX_BASICBLOCK_UNIT == 0) {
        return true;
    } else {
        return false;
    }
}

bool IsBasicBlockInSoftMax(AscendC::tiling::SoftMaxTiling& tiling, const uint32_t dataTypeSize)
{
    optiling::SoftMaxTiling opTiling;
    opTiling.set_srcK(tiling.srcK);
    opTiling.set_tailM(tiling.tailM);
    return IsBasicBlockInSoftMax(opTiling, dataTypeSize);
}

uint32_t GetSoftMaxFlashV2MaxTmpSize(
    const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize1, const uint32_t dataTypeSize2,
    const bool isUpdate, const bool isBasicBlock, UNUSED const bool isFlashOutputBrc)
{
    HighLevelApiCheck::SrcShapeSizeVerifyingParameters<SOFTMAX_FLASH_V2_GET_MAX>(
        srcShape.GetShapeSize(), dataTypeSize1);
    HighLevelApiCheck::ShapeLastAxisAlignVerifyingParameters<SOFTMAX_FLASH_V2_GET_MAX>(
        srcShape, dataTypeSize1, SOFTMAX_DEFAULT_BLK_SIZE);
    HighLevelApiCheck::TypeSizeVerifyingParameters<SOFTMAX_FLASH_V2_GET_MAX>(dataTypeSize1, SUPPORT_TYPESIZE);
    HighLevelApiCheck::TypeSizeVerifyingParameters<SOFTMAX_FLASH_V2_GET_MAX>(dataTypeSize2, SUPPORT_TYPESIZE);
    const uint32_t inputSize = srcShape.GetShapeSize();
    ASCENDC_HOST_ASSERT(inputSize > 0, return 0, "input srcShape size must be greater than 0.");
    ASCENDC_HOST_ASSERT(
        (dataTypeSize1 != 0) && (dataTypeSize1 == SOFTMAX_HALF_SIZE || dataTypeSize1 == SOFTMAX_FLOAT_SIZE), return 0,
        "dataTypeSize1 input error, dataType now only support half or float.");
    ASCENDC_HOST_ASSERT(
        (dataTypeSize2 != 0) && (dataTypeSize2 == SOFTMAX_HALF_SIZE || dataTypeSize2 == SOFTMAX_FLOAT_SIZE), return 0,
        "dataTypeSize2 input error, dataType now only support half or float.");

    std::vector<uint32_t> retVec = GetLastAxisShapeND(srcShape);
    uint32_t srcM = retVec[0];
    const uint32_t srcK = retVec[1];
    const uint32_t elementNumPerBlk = SOFTMAX_DEFAULT_BLK_SIZE / dataTypeSize2;
    platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
    ASCENDC_HOST_ASSERT((platform != nullptr), return 0, "Failed to get PlatformAscendC.");
    const auto npuArch = platform->GetCurNpuArch();

    uint32_t needMaxSize = 0;
    if (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_5102) {
        if (isUpdate) {
            if (srcM <= SOFTMAX_TMPBUFFER_COUNT) {
                if (dataTypeSize1 == SOFTMAX_HALF_SIZE) {
                    uint32_t size1 =
                        (srcM * SOFTMAX_TMPBUFFER_COUNT + SOFTMAX_BASICBLOCK_UNIT * SOFTMAX_TMPBUFFER_COUNT - 1) /
                        (SOFTMAX_BASICBLOCK_UNIT * SOFTMAX_TMPBUFFER_COUNT) *
                        (SOFTMAX_BASICBLOCK_UNIT * SOFTMAX_TMPBUFFER_COUNT);
                    if (dataTypeSize2 == SOFTMAX_HALF_SIZE) {
                        needMaxSize = size1 + srcM * (elementNumPerBlk * SOFTMAX_TMPBUFFER_COUNT + srcK);
                    } else {
                        needMaxSize =
                            size1 + srcM * (elementNumPerBlk * SOFTMAX_TMPBUFFER_COUNT + srcK + elementNumPerBlk);
                    }
                } else {
                    needMaxSize =
                        (srcM + SOFTMAX_BASICBLOCK_UNIT - 1) / SOFTMAX_BASICBLOCK_UNIT * SOFTMAX_BASICBLOCK_UNIT +
                        srcM * (elementNumPerBlk * SOFTMAX_TMPBUFFER_COUNT);
                }
            } else {
                if (isBasicBlock && (srcK % SOFTMAX_BASICBLOCK_UNIT == 0 && srcM % BASIC_TILE_NUM == 0)) {
                    if (dataTypeSize1 == SOFTMAX_HALF_SIZE) {
                        needMaxSize = srcM * (elementNumPerBlk + srcK + SOFTMAX_BASICBLOCK_UNIT);
                    } else {
                        needMaxSize = srcM * (elementNumPerBlk + SOFTMAX_BASICBLOCK_UNIT);
                    }
                } else {
                    needMaxSize =
                        (dataTypeSize1 == SOFTMAX_HALF_SIZE) ?
                            srcM * (elementNumPerBlk * SOFTMAX_TMPBUFFER_COUNT + srcK + SOFTMAX_BASICBLOCK_UNIT) :
                            srcM * (elementNumPerBlk + SOFTMAX_BASICBLOCK_UNIT);
                }
            }
        } else {
            uint32_t needSize1 = srcM * (BASIC_TILE_NUM + srcK) +
                                 SOFTMAX_BASICBLOCK_UNIT * SOFTMAX_TMPFLASHUPDATE_COUNT +
                                 (srcM + BASIC_TILE_NUM - 1) / BASIC_TILE_NUM * BASIC_TILE_NUM;
            uint32_t needSize2 = srcM * (elementNumPerBlk + srcK + SOFTMAX_BASICBLOCK_UNIT);
            needMaxSize = std::max(needSize1, needSize2);
        }
    } else {
        if (isBasicBlock && srcK % SOFTMAX_BASICBLOCK_UNIT == 0 && srcM % BASIC_TILE_NUM == 0) {
            // max repeat only support 255
            while (srcM * srcK >= SOFTMAX_BASICBLOCK_UNIT * SOFTMAX_BASICBLOCK_MIN_SIZE) {
                srcM = srcM / SOFTMAX_HALF_SIZE;
            }
            if (dataTypeSize1 == SOFTMAX_HALF_SIZE) {
                needMaxSize = (srcK == SOFTMAX_SPECIAL_BASICBLOCK_LEN) ?
                                  srcM * (elementNumPerBlk + srcK + SOFTMAX_BASICBLOCK_UNIT * SOFTMAX_HALF_SIZE) :
                                  srcM * (elementNumPerBlk + srcK + SOFTMAX_BASICBLOCK_UNIT);
            } else {
                needMaxSize = (srcK == SOFTMAX_SPECIAL_BASICBLOCK_LEN) ?
                                  srcM * (elementNumPerBlk + SOFTMAX_BASICBLOCK_UNIT * SOFTMAX_HALF_SIZE) :
                                  srcM * (elementNumPerBlk + SOFTMAX_BASICBLOCK_UNIT);
            }
        } else {
            needMaxSize = (dataTypeSize1 == SOFTMAX_HALF_SIZE) ?
                              srcM * (elementNumPerBlk * SOFTMAX_TMPBUFFER_COUNT + srcK + SOFTMAX_BASICBLOCK_UNIT) :
                              srcM * (elementNumPerBlk + SOFTMAX_BASICBLOCK_UNIT);
        }
    }

    return needMaxSize * SOFTMAX_FLOAT_SIZE;
}

uint32_t GetSoftMaxFlashV2MinTmpSize(
    const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize1, const uint32_t dataTypeSize2,
    const bool isUpdate, const bool isBasicBlock, const bool isFlashOutputBrc)
{
    HighLevelApiCheck::SrcShapeSizeVerifyingParameters<SOFTMAX_FLASH_V2_GET_MIN>(
        srcShape.GetShapeSize(), dataTypeSize1);
    HighLevelApiCheck::ShapeLastAxisAlignVerifyingParameters<SOFTMAX_FLASH_V2_GET_MIN>(
        srcShape, dataTypeSize1, SOFTMAX_DEFAULT_BLK_SIZE);
    HighLevelApiCheck::TypeSizeVerifyingParameters<SOFTMAX_FLASH_V2_GET_MIN>(dataTypeSize1, SUPPORT_TYPESIZE);
    HighLevelApiCheck::TypeSizeVerifyingParameters<SOFTMAX_FLASH_V2_GET_MIN>(dataTypeSize2, SUPPORT_TYPESIZE);
    const uint32_t inputSize = srcShape.GetShapeSize();
    ASCENDC_HOST_ASSERT(inputSize > 0, return 0, "input srcShape size must be greater than 0.");
    ASCENDC_HOST_ASSERT(
        (dataTypeSize1 != 0) && (dataTypeSize1 == SOFTMAX_HALF_SIZE || dataTypeSize1 == SOFTMAX_FLOAT_SIZE), return 0,
        "dataTypeSize1 input error, dataType now only support half or float.");
    ASCENDC_HOST_ASSERT(
        (dataTypeSize2 != 0) && (dataTypeSize2 == SOFTMAX_HALF_SIZE || dataTypeSize2 == SOFTMAX_FLOAT_SIZE), return 0,
        "dataTypeSize2 input error, dataType now only support half or float.");

    std::vector<uint32_t> retVec = GetLastAxisShapeND(srcShape);
    const uint32_t srcM = retVec[0];
    const uint32_t srcK = retVec[1];
    const uint32_t elementNumPerBlk = SOFTMAX_DEFAULT_BLK_SIZE / dataTypeSize2;
    platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
    ASCENDC_HOST_ASSERT((platform != nullptr), return 0, "Failed to get PlatformAscendC.");
    const auto npuArch = platform->GetCurNpuArch();

    uint32_t needMinSize = 0;
    if (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_5102) {
        if (isUpdate) {
            if (dataTypeSize1 == SOFTMAX_HALF_SIZE) {
                uint32_t size1 =
                    (srcM * SOFTMAX_TMPBUFFER_COUNT + SOFTMAX_BASICBLOCK_UNIT * SOFTMAX_TMPBUFFER_COUNT - 1) /
                    (SOFTMAX_BASICBLOCK_UNIT * SOFTMAX_TMPBUFFER_COUNT) *
                    (SOFTMAX_BASICBLOCK_UNIT * SOFTMAX_TMPBUFFER_COUNT);
                if (dataTypeSize2 == SOFTMAX_HALF_SIZE) {
                    needMinSize = size1 + srcM * (elementNumPerBlk * SOFTMAX_TMPBUFFER_COUNT + srcK);
                } else {
                    needMinSize = size1 + srcM * (elementNumPerBlk * SOFTMAX_TMPBUFFER_COUNT + srcK + elementNumPerBlk);
                }
            } else {
                needMinSize = (srcM + SOFTMAX_BASICBLOCK_UNIT - 1) / SOFTMAX_BASICBLOCK_UNIT * SOFTMAX_BASICBLOCK_UNIT +
                              srcM * (elementNumPerBlk * SOFTMAX_TMPBUFFER_COUNT);
            }
        } else {
            uint32_t size0 = BASIC_TILE_NUM;
            if (isBasicBlock) {
                size0 = SOFTMAX_TMPFLASHUPDATE_COUNT;
            }
            uint32_t needSize1 = srcM * (size0 + srcK) + SOFTMAX_BASICBLOCK_UNIT * SOFTMAX_TMPFLASHUPDATE_COUNT +
                                 (srcM + BASIC_TILE_NUM - 1) / BASIC_TILE_NUM * BASIC_TILE_NUM;
            uint32_t needSize2 = srcM * (elementNumPerBlk + srcK);
            needMinSize = std::max(needSize1, needSize2);
        }
    } else {
        if (isBasicBlock && srcK % SOFTMAX_BASICBLOCK_UNIT == 0) {
            if (dataTypeSize1 == SOFTMAX_HALF_SIZE) {
                needMinSize =
                    (srcK == SOFTMAX_SPECIAL_BASICBLOCK_LEN) ?
                        BASIC_TILE_NUM * (elementNumPerBlk + srcK + SOFTMAX_BASICBLOCK_UNIT * SOFTMAX_HALF_SIZE) :
                        BASIC_TILE_NUM * (elementNumPerBlk + srcK + SOFTMAX_BASICBLOCK_UNIT);
            } else {
                needMinSize = (srcK == SOFTMAX_SPECIAL_BASICBLOCK_LEN) ?
                                  BASIC_TILE_NUM * (elementNumPerBlk + SOFTMAX_BASICBLOCK_UNIT * SOFTMAX_HALF_SIZE) :
                                  BASIC_TILE_NUM * (elementNumPerBlk + SOFTMAX_BASICBLOCK_UNIT);
            }
        } else {
            needMinSize = (dataTypeSize1 == SOFTMAX_HALF_SIZE) ?
                              elementNumPerBlk * SOFTMAX_TMPBUFFER_COUNT + srcK + SOFTMAX_BASICBLOCK_UNIT :
                              elementNumPerBlk + SOFTMAX_BASICBLOCK_UNIT;
        }

        if (isFlashOutputBrc) {
            if (isBasicBlock && srcK % SOFTMAX_BASICBLOCK_UNIT == 0) {
                needMinSize =
                    (needMinSize / BASIC_TILE_NUM) * std::min((SOFTMAX_DEFAULT_BLK_SIZE / dataTypeSize1), srcM);
            } else {
                needMinSize = needMinSize * std::min((SOFTMAX_DEFAULT_BLK_SIZE / dataTypeSize1), srcM);
            }
        }
    }

    return needMinSize * SOFTMAX_FLOAT_SIZE;
}

void SoftMaxFlashV2TilingFunc(
    const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize1, const uint32_t dataTypeSize2,
    const uint32_t localWorkSpaceSize, optiling::SoftMaxTiling& softmaxFlashTiling, UNUSED const bool isUpdate,
    const bool isBasicBlock, const bool isFlashOutputBrc)
{
    HighLevelApiCheck::SrcShapeSizeVerifyingParameters<SOFTMAX_FLASH_V2_TILING>(srcShape.GetShapeSize(), dataTypeSize1);
    HighLevelApiCheck::ShapeLastAxisAlignVerifyingParameters<SOFTMAX_FLASH_V2_TILING>(
        srcShape, dataTypeSize1, SOFTMAX_DEFAULT_BLK_SIZE);
    HighLevelApiCheck::TypeSizeVerifyingParameters<SOFTMAX_FLASH_V2_TILING>(dataTypeSize1, SUPPORT_TYPESIZE);
    HighLevelApiCheck::TypeSizeVerifyingParameters<SOFTMAX_FLASH_V2_TILING>(dataTypeSize2, SUPPORT_TYPESIZE);
    HighLevelApiCheck::LocalWorkSpaceSizeVerifyingParameters<SOFTMAX_FLASH_V2_TILING>(localWorkSpaceSize);
    const uint32_t inputSize = srcShape.GetShapeSize();
    ASCENDC_HOST_ASSERT(inputSize > 0, return, "input srcShape size must be greater than 0.");
    ASCENDC_HOST_ASSERT(
        (dataTypeSize1 != 0) && (dataTypeSize1 == SOFTMAX_HALF_SIZE || dataTypeSize1 == SOFTMAX_FLOAT_SIZE), return,
        "dataTypeSize1 input error, dataType now only support half or float.");
    ASCENDC_HOST_ASSERT(
        (dataTypeSize2 != 0) && (dataTypeSize2 == SOFTMAX_HALF_SIZE || dataTypeSize2 == SOFTMAX_FLOAT_SIZE), return,
        "dataTypeSize2 input error, dataType now only support half or float.");

    std::vector<uint32_t> retVec = GetLastAxisShapeND(srcShape);
    const uint32_t elementNumPerBlk = SOFTMAX_DEFAULT_BLK_SIZE / dataTypeSize2;
    const uint32_t workLocalSize = localWorkSpaceSize / SOFTMAX_FLOAT_SIZE;
    const uint32_t srcK = retVec[1];
    const uint32_t srcM = retVec[0];
    uint32_t baseM = 0;

    if (isBasicBlock && srcK % SOFTMAX_BASICBLOCK_UNIT == 0 && srcM % BASIC_TILE_NUM == 0) {
        if (dataTypeSize1 == SOFTMAX_HALF_SIZE) {
            baseM = (srcK == SOFTMAX_SPECIAL_BASICBLOCK_LEN) ?
                        workLocalSize / (elementNumPerBlk + srcK + SOFTMAX_BASICBLOCK_UNIT * SOFTMAX_HALF_SIZE) :
                        workLocalSize / (elementNumPerBlk + srcK + SOFTMAX_BASICBLOCK_UNIT);
        } else {
            baseM = (srcK == SOFTMAX_SPECIAL_BASICBLOCK_LEN) ?
                        workLocalSize / (elementNumPerBlk + SOFTMAX_BASICBLOCK_UNIT * SOFTMAX_HALF_SIZE) :
                        workLocalSize / (elementNumPerBlk + SOFTMAX_BASICBLOCK_UNIT);
        }

        baseM = std::min(baseM, srcM);
        AdjustToBasicBlockBaseM(baseM, srcM, srcK);
    } else {
        baseM = (dataTypeSize1 == SOFTMAX_HALF_SIZE) ?
                    workLocalSize / (elementNumPerBlk * SOFTMAX_TMPBUFFER_COUNT + srcK + SOFTMAX_BASICBLOCK_UNIT) :
                    workLocalSize / (elementNumPerBlk + SOFTMAX_BASICBLOCK_UNIT);
    }

    uint32_t softmaxBasicTileNum = BASIC_TILE_NUM;
    if (isFlashOutputBrc && dataTypeSize1 == SOFTMAX_HALF_SIZE) {
        softmaxBasicTileNum = SOFTMAX_DEFAULT_BLK_SIZE / SOFTMAX_HALF_SIZE;
    }

    baseM = std::min(baseM, srcM);
    if (baseM < srcM && baseM > softmaxBasicTileNum) {
        baseM = baseM / softmaxBasicTileNum * softmaxBasicTileNum;
    }

    softmaxFlashTiling.set_srcM(srcM);
    softmaxFlashTiling.set_srcK(srcK);
    softmaxFlashTiling.set_srcSize(srcM * srcK);

    softmaxFlashTiling.set_outMaxM(srcM);
    softmaxFlashTiling.set_outMaxK(elementNumPerBlk);
    softmaxFlashTiling.set_outMaxSize(srcM * elementNumPerBlk);

    softmaxFlashTiling.set_reduceM(baseM);
    softmaxFlashTiling.set_reduceK(elementNumPerBlk);
    softmaxFlashTiling.set_reduceSize(baseM * elementNumPerBlk);

    softmaxFlashTiling.set_splitM(baseM);
    softmaxFlashTiling.set_splitK(srcK);
    softmaxFlashTiling.set_splitSize(baseM * srcK);

    uint32_t tail = 0;
    if (baseM != 0) {
        tail = srcM % baseM;
        softmaxFlashTiling.set_rangeM(srcM / baseM);
    }
    softmaxFlashTiling.set_tailM(tail);
    softmaxFlashTiling.set_tailSplitSize(tail * srcK);
    softmaxFlashTiling.set_tailReduceSize(tail * elementNumPerBlk);

    if (isFlashOutputBrc && (softmaxFlashTiling.get_rangeM() > 1 || softmaxFlashTiling.get_tailM() != 0)) {
        ASCENDC_HOST_ASSERT(
            (softmaxFlashTiling.get_reduceM() % (SOFTMAX_DEFAULT_BLK_SIZE / dataTypeSize1) == 0), return,
            "When dataTypeSize1(%u) is float(or half), softmaxFlashTiling.reduceM(%u) must be a multiple of 8(or 16), "
            "Adjust the input parameter -> localWorkSpaceSize.\n",
            dataTypeSize1, softmaxFlashTiling.get_reduceM());
    }
}

void SoftMaxFlashV2TilingFunc(
    const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize1, const uint32_t dataTypeSize2,
    const uint32_t localWorkSpaceSize, AscendC::tiling::SoftMaxTiling& softmaxFlashTiling, UNUSED const bool isUpdate,
    const bool isBasicBlock, const bool isFlashOutputBrc)
{
    optiling::SoftMaxTiling tiling;
    SoftMaxFlashV2TilingFunc(
        srcShape, dataTypeSize1, dataTypeSize2, localWorkSpaceSize, tiling, isUpdate, isBasicBlock, isFlashOutputBrc);
    tiling.SaveToBuffer(&softmaxFlashTiling, sizeof(SoftMaxTiling));
}

void GetSoftMaxFlashV3MaxMinTmpSize(
    const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize1, const uint32_t dataTypeSize2,
    uint32_t& maxValue, uint32_t& minValue, const bool isUpdate, UNUSED const bool isBasicBlock)
{
    HighLevelApiCheck::SrcShapeSizeVerifyingParameters<SOFTMAX_FLASH_V3_GET_MAX_MIN>(
        srcShape.GetShapeSize(), dataTypeSize1);
    HighLevelApiCheck::ShapeLastAxisAlignVerifyingParameters<SOFTMAX_FLASH_V3_GET_MAX_MIN>(
        srcShape, dataTypeSize1, SOFTMAX_DEFAULT_BLK_SIZE);
    HighLevelApiCheck::TypeSizeVerifyingParameters<SOFTMAX_FLASH_V3_GET_MAX_MIN>(
        dataTypeSize1, std::set<uint32_t>{SOFTMAX_HALF_SIZE});
    HighLevelApiCheck::TypeSizeVerifyingParameters<SOFTMAX_FLASH_V3_GET_MAX_MIN>(
        dataTypeSize2, std::set<uint32_t>{SOFTMAX_FLOAT_SIZE});
    ASCENDC_HOST_ASSERT(dataTypeSize1 == SOFTMAX_HALF_SIZE, return, "dataTypeSize1 must be sizeof(half).");
    ASCENDC_HOST_ASSERT(dataTypeSize2 > 0, return, "dataTypeSize2 must be greater than 0.");
    std::vector<uint32_t> retVec = GetLastAxisShapeND(srcShape);
    ASCENDC_HOST_ASSERT(retVec.size() > 1, return, "retVec must be greater than 1.");

    const uint32_t srcM = retVec[0];
    const uint32_t srcK = retVec[1];
    if (dataTypeSize2 == 0) {
        return;
    }
    const uint32_t elementNumPerBlk = SOFTMAX_DEFAULT_BLK_SIZE / dataTypeSize2;
    platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
    ASCENDC_HOST_ASSERT((platform != nullptr), return, "Failed to get PlatformAscendC.");
    const auto npuArch = platform->GetCurNpuArch();
    if (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_5102 || npuArch == NpuArch::DAV_3003 ||
        npuArch == NpuArch::DAV_3113) {
        if (!isUpdate) {
            minValue = (srcM * SOFTMAX_BASICBLOCK_UNIT + srcM * srcK) * SOFTMAX_FLOAT_SIZE;
        } else {
            minValue = (srcM * SOFTMAX_BASICBLOCK_UNIT + srcM * srcK + srcM * elementNumPerBlk) * SOFTMAX_FLOAT_SIZE;
        }
        maxValue = minValue;
    } else {
        minValue =
            elementNumPerBlk * SOFTMAXV3_TMPBUFFER_COUNT + SOFTMAX_TMPBUFFER_COUNT * srcK + SOFTMAX_BASICBLOCK_UNIT;
        minValue = minValue * SOFTMAX_FLOAT_SIZE;
        maxValue = srcM * minValue;
    }
}

void SoftMaxFlashV3TilingFunc(
    const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize1, const uint32_t dataTypeSize2,
    const uint32_t localWorkSpaceSize, optiling::SoftMaxTiling& softmaxFlashV3Tiling, UNUSED const bool isUpdate,
    UNUSED const bool isBasicBlock)
{
    HighLevelApiCheck::SrcShapeSizeVerifyingParameters<SOFTMAX_FLASH_V3_TILING>(srcShape.GetShapeSize(), dataTypeSize1);
    HighLevelApiCheck::ShapeLastAxisAlignVerifyingParameters<SOFTMAX_FLASH_V3_TILING>(
        srcShape, dataTypeSize1, SOFTMAX_DEFAULT_BLK_SIZE);
    HighLevelApiCheck::TypeSizeVerifyingParameters<SOFTMAX_FLASH_V3_TILING>(
        dataTypeSize1, std::set<uint32_t>{SOFTMAX_HALF_SIZE});
    HighLevelApiCheck::TypeSizeVerifyingParameters<SOFTMAX_FLASH_V3_TILING>(
        dataTypeSize2, std::set<uint32_t>{SOFTMAX_FLOAT_SIZE});
    HighLevelApiCheck::LocalWorkSpaceSizeVerifyingParameters<SOFTMAX_FLASH_V3_TILING>(localWorkSpaceSize);
    ASCENDC_HOST_ASSERT(dataTypeSize1 == SOFTMAX_HALF_SIZE, return, "dataTypeSize1 must be sizeof(half).");
    ASCENDC_HOST_ASSERT(dataTypeSize2 > 0, return, "dataTypeSize2 must be greater than 0.");

    std::vector<uint32_t> retVec = GetLastAxisShapeND(srcShape);
    ASCENDC_HOST_ASSERT(retVec.size() > 1, return, "retVec must be greater than 1.");
    if (dataTypeSize2 == 0) {
        return;
    }
    const uint32_t elementNumPerBlk = SOFTMAX_DEFAULT_BLK_SIZE / dataTypeSize2;
    const uint32_t workLocalSize = localWorkSpaceSize / SOFTMAX_FLOAT_SIZE;
    const uint32_t srcK = retVec[1];
    const uint32_t srcM = retVec[0];
    uint32_t baseM = 0;

    baseM = workLocalSize /
            (elementNumPerBlk * SOFTMAXV3_TMPBUFFER_COUNT + SOFTMAX_TMPBUFFER_COUNT * srcK + SOFTMAX_BASICBLOCK_UNIT);
    baseM = std::min(baseM, srcM);
    if (baseM < srcM && baseM > BASIC_TILE_NUM) {
        baseM = baseM / BASIC_TILE_NUM * BASIC_TILE_NUM;
    }

    softmaxFlashV3Tiling.set_srcM(srcM);
    softmaxFlashV3Tiling.set_srcK(srcK);
    softmaxFlashV3Tiling.set_srcSize(srcM * srcK);

    softmaxFlashV3Tiling.set_outMaxM(srcM);
    softmaxFlashV3Tiling.set_outMaxK(elementNumPerBlk);
    softmaxFlashV3Tiling.set_outMaxSize(srcM * elementNumPerBlk);

    softmaxFlashV3Tiling.set_splitM(baseM);
    softmaxFlashV3Tiling.set_splitK(srcK);
    softmaxFlashV3Tiling.set_splitSize(baseM * srcK);

    softmaxFlashV3Tiling.set_reduceM(baseM);
    softmaxFlashV3Tiling.set_reduceK(elementNumPerBlk);
    softmaxFlashV3Tiling.set_reduceSize(baseM * elementNumPerBlk);

    if (baseM != 0) {
        uint32_t tail = srcM % baseM;
        softmaxFlashV3Tiling.set_rangeM(srcM / baseM);
        softmaxFlashV3Tiling.set_tailM(tail);
        softmaxFlashV3Tiling.set_tailSplitSize(tail * srcK);
        softmaxFlashV3Tiling.set_tailReduceSize(tail * elementNumPerBlk);
    }
}

void SoftMaxFlashV3TilingFunc(
    const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize1, const uint32_t dataTypeSize2,
    const uint32_t localWorkSpaceSize, AscendC::tiling::SoftMaxTiling& softmaxFlashV3Tiling, UNUSED const bool isUpdate,
    UNUSED const bool isBasicBlock)
{
    optiling::SoftMaxTiling tiling;
    SoftMaxFlashV3TilingFunc(
        srcShape, dataTypeSize1, dataTypeSize2, localWorkSpaceSize, tiling, isUpdate, isBasicBlock);
    tiling.SaveToBuffer(&softmaxFlashV3Tiling, sizeof(SoftMaxTiling));
}

} // namespace AscendC
