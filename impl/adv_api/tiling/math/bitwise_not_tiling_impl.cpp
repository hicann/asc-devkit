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
 * \file bitwise_not_tiling_impl.cpp
 * \brief
 */
#include "adv_api/utils/types.h"
#include <set>
#include "../../detail/host_log.h"
#include "../../detail/api_check/host_apicheck.h"
#include "adv_api/math/bitwise_not_tiling.h"
#include "utils/tiling/platform/platform_ascendc.h"

namespace AscendC {
namespace {
static const std::set<uint32_t> SUPPORT_TYPESIZE = {1, 2, 4, 8};
static constexpr const char BITWISE_NOT_GET_MAX_MIN[] = "GetBitwiseNotMaxMinTmpSize";
static constexpr const char BITWISE_NOT_GET_TMP_BUFFER[] = "GetBitwiseNotTmpBufferFactorSize";

uint32_t GetBitwiseNotMaxTmpSize(const NpuArch npuArch, const uint32_t inputSize, const uint32_t typeSize)
{
    (void)npuArch;
    (void)typeSize;
    (void)inputSize;
    return 0;
}

uint32_t GetBitwiseNotMinTmpSize(const NpuArch npuArch, const uint32_t typeSize)
{
    (void)npuArch;
    (void)typeSize;
    return 0;
}
} // namespace
void GetBitwiseNotMaxMinTmpSize(
    const platform_ascendc::PlatformAscendC& ascendcPlatform, const AscendC::TensorShape& srcShape,
    const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
{
    (void)isReuseSource;

    const uint32_t inputSize = srcShape.GetShapeSize();
    auto npuArch = ascendcPlatform.GetCurNpuArch();
    ASCENDC_HOST_ASSERT(
        (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201), return,
        "Unsupported NpuArch of BitwiseNot API.");

    HighLevelApiCheck::SrcShapeSizeVerifyingParameters<BITWISE_NOT_GET_MAX_MIN>(inputSize, typeSize);
    HighLevelApiCheck::TypeSizeVerifyingParameters<BITWISE_NOT_GET_MAX_MIN>(typeSize, SUPPORT_TYPESIZE);
    HighLevelApiCheck::IsReuseSourceVerifyingParameters<BITWISE_NOT_GET_MAX_MIN>(isReuseSource);

    maxValue = GetBitwiseNotMaxTmpSize(npuArch, inputSize, typeSize);
    minValue = GetBitwiseNotMinTmpSize(npuArch, typeSize);
}

void GetBitwiseNotTmpBufferFactorSize(
    const platform_ascendc::PlatformAscendC& ascendcPlatform, const uint32_t typeSize, uint32_t& maxLivedNodeCount,
    uint32_t& extraBuf)
{
    auto npuArch = ascendcPlatform.GetCurNpuArch();
    ASCENDC_HOST_ASSERT(
        (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201), return,
        "Unsupported NpuArch of BitwiseNot API.");
    HighLevelApiCheck::TypeSizeVerifyingParameters<BITWISE_NOT_GET_TMP_BUFFER>(typeSize, SUPPORT_TYPESIZE);

    extraBuf = 0u;
    maxLivedNodeCount = 0u;
}

} // namespace AscendC
