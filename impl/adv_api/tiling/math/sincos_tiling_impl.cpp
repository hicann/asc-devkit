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
 * \file sincos_tiling_impl.cpp
 * \brief
 */
#include "adv_api/utils/types.h"
#include <vector>
#include <set>
#include <cstdint>
#include "adv_api/math/sincos_tiling.h"
#include "utils/tiling/platform/platform_ascendc.h"
#include "../../detail/host_log.h"
#include "../../detail/api_check/host_apicheck.h"

namespace AscendC {
namespace {
constexpr uint32_t SINCOS_EXTRA_BUF = 32;
constexpr uint32_t SINCOS_DOUBLE = 2;
static constexpr uint32_t SINCOS_HALF_SIZE = 2;
static constexpr uint32_t SINCOS_FLOAT_SIZE = 4;
static const std::set<uint32_t> SUPPORT_TYPESIZE = {SINCOS_HALF_SIZE, SINCOS_FLOAT_SIZE};
static constexpr const char SINCOS_GET_MAX_MIN[] = "GetSinCosMaxMinTmpSize";
static constexpr const char SINCOS_GET_TMP_BUFFER[] = "GetSinCosTmpBufferFactorSize";
} // namespace
void GetSinCosMaxMinTmpSize(
    const platform_ascendc::PlatformAscendC& ascendcPlatform, const AscendC::TensorShape& srcShape,
    const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
{
    constexpr uint32_t alignSize = 32;
    HighLevelApiCheck::SrcShapeSizeVerifyingParameters<SINCOS_GET_MAX_MIN>(srcShape.GetShapeSize(), typeSize);
    HighLevelApiCheck::TypeSizeVerifyingParameters<SINCOS_GET_MAX_MIN>(typeSize, SUPPORT_TYPESIZE);
    HighLevelApiCheck::IsReuseSourceVerifyingParameters<SINCOS_GET_MAX_MIN>(isReuseSource);
    auto npuArch = ascendcPlatform.GetCurNpuArch();
    ASCENDC_HOST_ASSERT(
        (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201 || npuArch == NpuArch::DAV_5102), return,
        "Unsupported NpuArch of CosSin API.");

    std::vector<int64_t> shapeDims = srcShape.GetDims();
    uint32_t inputSize = 1;
    for (const auto dim : shapeDims) {
        inputSize *= dim;
    }
    inputSize = (inputSize + alignSize - 1u) / alignSize * alignSize;
    maxValue = static_cast<uint32_t>(sizeof(float) * inputSize * SINCOS_DOUBLE + SINCOS_EXTRA_BUF);
    minValue = static_cast<uint32_t>(sizeof(float) * inputSize * SINCOS_DOUBLE + SINCOS_EXTRA_BUF);
}

void GetSinCosTmpBufferFactorSize(
    const platform_ascendc::PlatformAscendC& ascendcPlatform, const uint32_t typeSize, uint32_t& maxLivedNodeCount,
    uint32_t& extraBuf)
{
    HighLevelApiCheck::TypeSizeVerifyingParameters<SINCOS_GET_TMP_BUFFER>(typeSize, SUPPORT_TYPESIZE);
    auto npuArch = ascendcPlatform.GetCurNpuArch();
    ASCENDC_HOST_ASSERT(
        (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201 || npuArch == NpuArch::DAV_5102), return,
        "Unsupported NpuArch of SinCos API.");
    if (typeSize == sizeof(float)) {
        extraBuf = SINCOS_EXTRA_BUF;
        maxLivedNodeCount = SINCOS_DOUBLE;
    } else {
        extraBuf = 0u;
        maxLivedNodeCount = SINCOS_DOUBLE * SINCOS_DOUBLE;
    }
}
} // namespace AscendC
