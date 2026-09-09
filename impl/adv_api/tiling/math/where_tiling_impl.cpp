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
 * \file where_tiling_impl.cpp
 * \brief
 */
#include "adv_api/utils/types.h"
#include <set>
#include "adv_api/math/where_tiling.h"
#include "utils/tiling/platform/platform_ascendc.h"
#include "../../detail/host_log.h"
#include "../../detail/api_check/host_apicheck.h"

namespace AscendC {
namespace {
static constexpr uint32_t WHERE_B8_SIZE = 1;
static constexpr uint32_t WHERE_B16_SIZE = 2;
static constexpr uint32_t WHERE_B32_SIZE = 4;
static constexpr uint32_t WHERE_B64_SIZE = 8;
static const std::set<uint32_t> SUPPORT_TYPESIZE = {WHERE_B8_SIZE, WHERE_B16_SIZE, WHERE_B32_SIZE, WHERE_B64_SIZE};
static constexpr const char WHERE_GET_MAX_MIN[] = "GetWhereMaxMinTmpSize";
static constexpr const char WHERE_GET_TMP_BUFFER[] = "GetWhereTmpBufferFactorSize";
} // namespace

void GetWhereMaxMinTmpSize(
    const platform_ascendc::PlatformAscendC& ascendcPlatform, const AscendC::TensorShape& srcShape,
    const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
{
    HighLevelApiCheck::SrcShapeSizeVerifyingParameters<WHERE_GET_MAX_MIN>(srcShape.GetShapeSize(), typeSize);
    HighLevelApiCheck::TypeSizeVerifyingParameters<WHERE_GET_MAX_MIN>(typeSize, SUPPORT_TYPESIZE);
    HighLevelApiCheck::IsReuseSourceVerifyingParameters<WHERE_GET_MAX_MIN>(isReuseSource);
    auto npuArch = ascendcPlatform.GetCurNpuArch();

    ASCENDC_HOST_ASSERT(
        (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201), return, "Unsupported NpuArch for Where API.");
    (void)srcShape;
    (void)typeSize;
    (void)isReuseSource;

    maxValue = 0u;
    minValue = 0u;
}

void GetWhereTmpBufferFactorSize(
    const platform_ascendc::PlatformAscendC& ascendcPlatform, const uint32_t typeSize, uint32_t& maxLivedNodeCount,
    uint32_t& extraBuf)
{
    HighLevelApiCheck::TypeSizeVerifyingParameters<WHERE_GET_TMP_BUFFER>(typeSize, SUPPORT_TYPESIZE);
    auto npuArch = ascendcPlatform.GetCurNpuArch();
    ASCENDC_HOST_ASSERT(
        (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201), return, "Unsupported NpuArch of Where API.");
    (void)typeSize;
    extraBuf = 0u;
    maxLivedNodeCount = 0u;
}
} // namespace AscendC
