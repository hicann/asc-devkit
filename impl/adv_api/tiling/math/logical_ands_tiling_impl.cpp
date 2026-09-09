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
 * \file logical_ands_tiling_impl.cpp
 * \brief
 */
#include "adv_api/utils/types.h"
#include <set>
#include "adv_api/math/logical_ands_tiling.h"
#include "utils/tiling/platform/platform_ascendc.h"
#include "../../detail/host_log.h"
#include "../../detail/api_check/host_apicheck.h"

namespace AscendC {
namespace {
static constexpr uint32_t LOGICAL_ANDS_UINT8_SIZE = 1;
static constexpr uint32_t LOGICAL_ANDS_UINT16_SIZE = 2;
static constexpr uint32_t LOGICAL_ANDS_UINT32_SIZE = 4;
static constexpr uint32_t LOGICAL_ANDS_UINT64_SIZE = 8;
static const std::set<uint32_t> SUPPORT_TYPESIZE = {
    LOGICAL_ANDS_UINT8_SIZE, LOGICAL_ANDS_UINT16_SIZE, LOGICAL_ANDS_UINT32_SIZE, LOGICAL_ANDS_UINT64_SIZE};
static constexpr const char LOGICAL_ANDS_GET_MAX_MIN[] = "GetLogicalAndsMaxMinTmpSize";
static constexpr const char LOGICAL_ANDS_GET_TMP_BUFFER[] = "GetLogicalAndsTmpBufferFactorSize";
} // namespace

void GetLogicalAndsMaxMinTmpSize(
    const platform_ascendc::PlatformAscendC& ascendcPlatform, const AscendC::TensorShape& srcShape,
    const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
{
    HighLevelApiCheck::SrcShapeSizeVerifyingParameters<LOGICAL_ANDS_GET_MAX_MIN>(srcShape.GetShapeSize(), typeSize);
    HighLevelApiCheck::TypeSizeVerifyingParameters<LOGICAL_ANDS_GET_MAX_MIN>(typeSize, SUPPORT_TYPESIZE);
    HighLevelApiCheck::IsReuseSourceVerifyingParameters<LOGICAL_ANDS_GET_MAX_MIN>(isReuseSource);
    auto npuArch = ascendcPlatform.GetCurNpuArch();

    ASCENDC_HOST_ASSERT(
        npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201, return,
        "Unsupported NpuArch of LogicalAnds API.");
    maxValue = 0u;
    minValue = 0u;
}

void GetLogicalAndsTmpBufferFactorSize(
    const platform_ascendc::PlatformAscendC& ascendcPlatform, const uint32_t typeSize, uint32_t& maxLivedNodeCount,
    uint32_t& extraBuf)
{
    HighLevelApiCheck::TypeSizeVerifyingParameters<LOGICAL_ANDS_GET_TMP_BUFFER>(typeSize, SUPPORT_TYPESIZE);
    auto npuArch = ascendcPlatform.GetCurNpuArch();
    ASCENDC_HOST_ASSERT(
        npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201, return,
        "Unsupported NpuArch of LogicalAnds API.");
    extraBuf = 0u;
    maxLivedNodeCount = 0u;
}
} // namespace AscendC
