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
 * \file bitwise_xor_tiling_impl.cpp
 * \brief
 */
#include "adv_api/utils/types.h"
#include <set>
#include "adv_api/math/bitwise_xor_tiling.h"
#include "utils/tiling/platform/platform_ascendc.h"
#include "../../detail/host_log.h"
#include "../../detail/api_check/host_apicheck.h"

namespace AscendC {
namespace {
static const std::set<uint32_t> SUPPORT_TYPESIZE = {1, 2, 4, 8};
static constexpr const char BITWISE_XOR_GET_MAX_MIN[] = "GetBitwiseXorMaxMinTmpSize";
static constexpr const char BITWISE_XOR_GET_TMP_BUFFER[] = "GetBitwiseXorTmpBufferFactorSize";

uint32_t GetBitwiseXorMaxTmpSize(const NpuArch npuArch, const uint32_t inputSize, const uint32_t typeSize)
{
    (void)npuArch;
    (void)typeSize;
    (void)inputSize;
    return 0;
}

uint32_t GetBitwiseXorMinTmpSize(const NpuArch npuArch, const uint32_t typeSize)
{
    (void)npuArch;
    (void)typeSize;
    return 0;
}
} // namespace
void GetBitwiseXorMaxMinTmpSize(
    const platform_ascendc::PlatformAscendC& ascendcPlatform, const AscendC::TensorShape& srcShape,
    const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
{
    (void)isReuseSource;

    const uint32_t inputSize = srcShape.GetShapeSize();
    auto npuArch = ascendcPlatform.GetCurNpuArch();
    ASCENDC_HOST_ASSERT(
        (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201), return,
        "Unsupported NpuArch of BitwiseXor API.");

    HighLevelApiCheck::SrcShapeSizeVerifyingParameters<BITWISE_XOR_GET_MAX_MIN>(inputSize, typeSize);
    HighLevelApiCheck::TypeSizeVerifyingParameters<BITWISE_XOR_GET_MAX_MIN>(typeSize, SUPPORT_TYPESIZE);
    HighLevelApiCheck::IsReuseSourceVerifyingParameters<BITWISE_XOR_GET_MAX_MIN>(isReuseSource);

    maxValue = GetBitwiseXorMaxTmpSize(npuArch, inputSize, typeSize);
    minValue = GetBitwiseXorMinTmpSize(npuArch, typeSize);
}

void GetBitwiseXorTmpBufferFactorSize(
    const platform_ascendc::PlatformAscendC& ascendcPlatform, const uint32_t typeSize, uint32_t& maxLivedNodeCount,
    uint32_t& extraBuf)
{
    auto npuArch = ascendcPlatform.GetCurNpuArch();
    ASCENDC_HOST_ASSERT(
        (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201), return,
        "Unsupported NpuArch of BitwiseXor API.");
    HighLevelApiCheck::TypeSizeVerifyingParameters<BITWISE_XOR_GET_TMP_BUFFER>(typeSize, SUPPORT_TYPESIZE);

    extraBuf = 0u;
    maxLivedNodeCount = 0u;
}

} // namespace AscendC
