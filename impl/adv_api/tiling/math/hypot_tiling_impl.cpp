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
 * \file hypot_tiling_impl.cpp
 * \brief
 */
#include "adv_api/utils/types.h"
#include <cstdint>
#include "../../detail/host_log.h"
#include "utils/tiling/platform/platform_ascendc.h"
#include "adv_api/math/hypot_tiling.h"

namespace AscendC {
namespace {

inline uint32_t GetHypotMaxTmpSize(const uint32_t inputSize, const uint32_t typeSize)
{
    (void)typeSize;
    (void)inputSize;
    platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
    ASCENDC_HOST_ASSERT((platform != nullptr), return 0, "Failed to get PlatformAscendC");
    const auto npuArch = platform->GetCurNpuArch();
    if (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201 || npuArch == NpuArch::DAV_5102) {
        return 0;
    }
    return 0;
}

inline uint32_t GetHypotMinTmpSize(const uint32_t typeSize)
{
    (void)typeSize;
    platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
    ASCENDC_HOST_ASSERT((platform != nullptr), return 0, "Failed to get PlatformAscendC");
    const auto npuArch = platform->GetCurNpuArch();
    if (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201 || npuArch == NpuArch::DAV_5102) {
        return 0;
    }
    return 0;
}
} // namespace

void GetHypotMaxMinTmpSize(
    const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue,
    uint32_t& minValue)
{
    (void)isReuseSource;
    const uint32_t inputSize = srcShape.GetShapeSize();
    ASCENDC_HOST_ASSERT(inputSize > 0, return, "Input Shape size must be greater than 0.");

    minValue = GetHypotMinTmpSize(typeSize);
    maxValue = GetHypotMaxTmpSize(inputSize, typeSize);
}

void GetHypotTmpBufferFactorSize(const uint32_t typeSize, uint32_t& maxLiveNodeCount, uint32_t& extraBuf)
{
    (void)typeSize;
    extraBuf = 0;
    platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
    ASCENDC_HOST_ASSERT((platform != nullptr), return, "Failed to get PlatformAscendC");
    const auto npuArch = platform->GetCurNpuArch();
    if (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201 || npuArch == NpuArch::DAV_5102) {
        maxLiveNodeCount = 0;
    }
}
} // namespace AscendC
