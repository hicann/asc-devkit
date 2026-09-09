/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file sin_tiling_impl.cpp
 * \brief
 */
#include "adv_api/utils/types.h"
#include <cstdint>
#include <vector>
#include "../../detail/host_log.h"
#include "adv_api/math/sin_tiling.h"
#include "utils/tiling/platform/platform_ascendc.h"

namespace AscendC {
namespace {
constexpr uint32_t SIN_HALF_CALC_FAC = 8;
constexpr uint32_t SIN_FLOAT_NOREUSE_CALC_FAC = 3;
constexpr uint32_t SIN_FLOAT_REUSE_CALC_FAC = 2;
constexpr uint32_t SIN_ONE_REPEAT_BYTE_SIZE = 256;
constexpr uint32_t SIN_EXTRA_BUF = 32;
constexpr uint32_t SIN_DOUBLE = 2;
} // namespace

void GetSinTmpBufferFactorSize(const uint32_t typeSize, uint32_t& maxLiveNodeCount, uint32_t& extraBuf)
{
    extraBuf = 0;
    maxLiveNodeCount = (typeSize == sizeof(float)) ? SIN_FLOAT_NOREUSE_CALC_FAC : SIN_HALF_CALC_FAC;
}

inline uint32_t GetSinMaxTmpSize(
    const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource)
{
    std::vector<int64_t> shapeDims = srcShape.GetDims();
    uint32_t inputSize = 1;
    for (const auto dim : shapeDims) {
        inputSize *= dim;
    }
    ASCENDC_HOST_ASSERT(inputSize > 0, return 0, "Input Shape size must be greater than 0.");
    uint32_t calcFactor = 0;
    if (typeSize == sizeof(float)) {
        calcFactor = isReuseSource ? SIN_FLOAT_REUSE_CALC_FAC : SIN_FLOAT_NOREUSE_CALC_FAC;
    } else {
        ASCENDC_HOST_ASSERT(
            isReuseSource == false, return 0, "when the input data type is half, isReuseSource is not supported");
        calcFactor = SIN_HALF_CALC_FAC;
    }
    return inputSize * calcFactor * typeSize;
}

inline uint32_t GetSinMinTmpSize(
    const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource)
{
    (void)(srcShape);
    (void)(isReuseSource);
    uint32_t calcFactor = 0;
    if (typeSize == sizeof(float)) {
        calcFactor = isReuseSource ? SIN_FLOAT_REUSE_CALC_FAC : SIN_FLOAT_NOREUSE_CALC_FAC;
    } else {
        ASCENDC_HOST_ASSERT(
            isReuseSource == false, return 0, "when the input data type is half, isReuseSource is not supported");
        calcFactor = SIN_HALF_CALC_FAC;
    }
    return calcFactor * SIN_ONE_REPEAT_BYTE_SIZE;
}

void GetSinMaxMinTmpSize(
    const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue,
    uint32_t& minValue)
{
    uint32_t max = GetSinMaxTmpSize(srcShape, typeSize, isReuseSource);
    minValue = GetSinMinTmpSize(srcShape, typeSize, isReuseSource);
    maxValue = std::max(max, minValue);
}

void GetSinTmpBufferFactorSize(
    const SinConfig& config, const uint32_t typeSize, uint32_t& maxLiveNodeCount, uint32_t& extraBuf)
{
    platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
    ASCENDC_HOST_ASSERT((platform != nullptr), return, "Failed to get PlatformAscendC");

    auto npuArch = platform->GetCurNpuArch();
    ASCENDC_HOST_ASSERT(
        (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201 || npuArch == NpuArch::DAV_3003 ||
         npuArch == NpuArch::DAV_5102),
        return, "Unsupported NpuArch of Cos API.");
    if (config.algo == SinAlgo::POLYNOMIAL_APPROXIMATION) {
        extraBuf = 0;
        maxLiveNodeCount = 0;
    } else if (config.algo == SinAlgo::RADIAN_REDUCTION) {
        if (typeSize == sizeof(float)) {
            extraBuf = SIN_EXTRA_BUF;
            maxLiveNodeCount = SIN_DOUBLE;
        } else {
            extraBuf = 0;
            maxLiveNodeCount = SIN_DOUBLE * SIN_DOUBLE;
        }
    }
}

void GetSinMaxMinTmpSize(
    const SinConfig& config, const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource,
    uint32_t& maxValue, uint32_t& minValue)
{
    (void)typeSize;
    (void)isReuseSource;
    constexpr uint32_t alignSize = 32;
    platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
    ASCENDC_HOST_ASSERT((platform != nullptr), return, "Failed to get PlatformAscendC");

    auto npuArch = platform->GetCurNpuArch();
    ASCENDC_HOST_ASSERT(
        (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201 || npuArch == NpuArch::DAV_3003 ||
         npuArch == NpuArch::DAV_5102),
        return, "Unsupported NpuArch of Cos API.");

    if (config.algo == SinAlgo::POLYNOMIAL_APPROXIMATION) {
        maxValue = 0;
        minValue = 0;
    } else if (config.algo == SinAlgo::RADIAN_REDUCTION) {
        std::vector<int64_t> shapeDims = srcShape.GetDims();
        uint32_t inputSize = 1;
        for (const auto dim : shapeDims) {
            inputSize *= dim;
        }
        inputSize = (inputSize + alignSize - 1u) / alignSize * alignSize;
        maxValue = sizeof(float) * inputSize * SIN_DOUBLE + SIN_EXTRA_BUF;
        minValue = sizeof(float) * inputSize * SIN_DOUBLE + SIN_EXTRA_BUF;
    }
}
} // namespace AscendC
