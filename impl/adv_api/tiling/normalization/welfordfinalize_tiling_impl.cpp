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
#include "adv_api/normalization/welfordfinalize_tiling.h"
#include "../../detail/host_log.h"
#include "utils/tiling/platform/platform_ascendc.h"

namespace AscendC {
constexpr uint32_t WELFORDFINALIZE_BASICBLOCK_UNIT = 64;
constexpr uint32_t WELFORDFINALIZE_BASIC_SIZE = 32 / sizeof(float);

void GetWelfordFinalizeMaxMinTmpSize(
    const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue,
    uint32_t& minValue)
{
    (void)isReuseSource;
    (void)typeSize;
    std::vector<int64_t> shapeDims = srcShape.GetDims();
    platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
    ASCENDC_HOST_ASSERT((platform != nullptr), return, "Failed to get PlatformAscendC.");
    const auto npuArch = platform->GetCurNpuArch();
    if (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_5102 || npuArch == NpuArch::DAV_3003 ||
        npuArch == NpuArch::DAV_3113) {
        const uint32_t srcK = shapeDims.back();
        // next is to get the max 2^k
        uint32_t reduceK = srcK;
        reduceK |= reduceK >> 1;
        reduceK |= reduceK >> 2;
        reduceK |= reduceK >> 4;
        reduceK |= reduceK >> 8;
        reduceK |= reduceK >> 16;
        reduceK = (reduceK + 1) >> 1;
        uint32_t reduceTmpSize = (reduceK / 64 + 128 - 1) / 128 * 128; // align to 128 element
        minValue = (srcK + reduceTmpSize) * sizeof(float);
        maxValue = minValue;
        return;
    } else {
        ASCENDC_HOST_ASSERT(shapeDims.size() == 0x1, return, "srcShape dims must be 1.");
        const uint32_t abLength = static_cast<uint32_t>(shapeDims[0]);

        ASCENDC_HOST_ASSERT(
            abLength > 0 && abLength % WELFORDFINALIZE_BASIC_SIZE == 0, return, "abLength must be aligned to 32Bytes.");

        // 0x4 indicates reserving four buffers for the calculate and storage of mean and variance
        minValue = WELFORDFINALIZE_BASICBLOCK_UNIT * 0x4 * sizeof(float);

        if (abLength <= WELFORDFINALIZE_BASICBLOCK_UNIT) {
            maxValue = minValue;
        } else {
            // 0x2 indicates the buffersizes of calculate/storage of mean and variance is different
            maxValue = (WELFORDFINALIZE_BASICBLOCK_UNIT * 0x2 + abLength * 0x2) * sizeof(float);
        }
    }
}
} // namespace AscendC
