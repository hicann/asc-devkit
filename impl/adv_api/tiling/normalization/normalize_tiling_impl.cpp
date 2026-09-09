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
#include "adv_api/normalization/normalize_tiling.h"
#include "../../detail/host_log.h"
#include "utils/tiling/platform/platform_ascendc.h"

namespace AscendC {
namespace {
constexpr uint32_t NORMALIZE_TMP_TENSOR_MIN = 2;
constexpr uint32_t NORMALIZE_GAMMA_BETA_BLK = 2; // 2 * R(gamma, beta) + 2 * NR
constexpr uint32_t NORMALIZE_ONE_BLK_SIZE = 32;
constexpr uint32_t NORMALIZE_ONE_NUMBER = 1;
constexpr uint32_t NORMALIZE_BRCB_NUM = 8;

uint32_t GetNormalizeMaxTmpSize(const uint32_t aLength, const uint32_t rLengthWithPadding)
{
    uint32_t rLengthByte = rLengthWithPadding * sizeof(float); // [R]
    uint32_t aLengthPad =
        (aLength + NORMALIZE_BRCB_NUM - NORMALIZE_ONE_NUMBER) / NORMALIZE_BRCB_NUM * NORMALIZE_BRCB_NUM;
    uint32_t inputLenByte = aLengthPad * rLengthWithPadding * sizeof(float); // [A, R]

    rLengthByte =
        (rLengthByte + NORMALIZE_ONE_BLK_SIZE - NORMALIZE_ONE_NUMBER) / NORMALIZE_ONE_BLK_SIZE * NORMALIZE_ONE_BLK_SIZE;
    inputLenByte = (inputLenByte + NORMALIZE_ONE_BLK_SIZE - NORMALIZE_ONE_NUMBER) / NORMALIZE_ONE_BLK_SIZE *
                   NORMALIZE_ONE_BLK_SIZE;
    return NORMALIZE_GAMMA_BETA_BLK * rLengthByte + NORMALIZE_GAMMA_BETA_BLK * inputLenByte; // MaxSize: 2R + 2AR
}

uint32_t GetNormalizeMinTmpSize(const uint32_t aLength, const uint32_t rLengthWithPadding, const uint32_t typeSizeU)
{
    uint32_t aLengthByte = aLength * sizeof(float);            // [A]
    uint32_t rLengthByte = rLengthWithPadding * sizeof(float); // [R]

    aLengthByte =
        (aLengthByte + NORMALIZE_ONE_BLK_SIZE - NORMALIZE_ONE_NUMBER) / NORMALIZE_ONE_BLK_SIZE * NORMALIZE_ONE_BLK_SIZE;
    rLengthByte =
        (rLengthByte + NORMALIZE_ONE_BLK_SIZE - NORMALIZE_ONE_NUMBER) / NORMALIZE_ONE_BLK_SIZE * NORMALIZE_ONE_BLK_SIZE;

    uint32_t nrByte;
    if (typeSizeU == sizeof(float)) {
        // MinSize: max(A, 2 * N * R), N >= 1
        nrByte = NORMALIZE_TMP_TENSOR_MIN * rLengthByte; // 2R
    } else {
        // MinSize: max(A, 2R + 2 * N * R), N >= 1
        nrByte = (NORMALIZE_GAMMA_BETA_BLK + NORMALIZE_TMP_TENSOR_MIN) * rLengthByte; // 4R
    }
    return (aLengthByte >= nrByte) ? aLengthByte : nrByte;
}
} // namespace

void GetNormalizeMaxMinTmpSize(
    const AscendC::TensorShape& srcShape, const uint32_t typeSizeU, const uint32_t typeSizeT, const bool isReuseSource,
    const bool isComputeRstd, const bool isOnlyOutput, uint32_t& maxValue, uint32_t& minValue)
{
    (void)isReuseSource;
    ASCENDC_HOST_ASSERT(isComputeRstd == true, return, "isComputeRstd current only support true");
    ASCENDC_HOST_ASSERT(isOnlyOutput == false, return, "isOnlyOutput current only support false");
    ASCENDC_HOST_ASSERT(typeSizeU != 0, return, "typeSizeU can not be 0!");
    ASCENDC_HOST_ASSERT(typeSizeT != 0, return, "typeSizeT can not be 0!");
    platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
    ASCENDC_HOST_ASSERT((platform != nullptr), return, "Failed to get PlatformAscendC.");
    const auto npuArch = platform->GetCurNpuArch();
    if (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201 || npuArch == NpuArch::DAV_5102) {
        (void)srcShape;
        minValue = 0;
        maxValue = 0;
        return;
    } else {
        (void)typeSizeU;
        (void)typeSizeT;
        std::vector<int64_t> shapeDims = srcShape.GetDims();
        const uint32_t aLength = static_cast<uint32_t>(shapeDims[0]);
        const uint32_t rLength = static_cast<uint32_t>(shapeDims[1]);
        uint32_t typeAlignNum = (typeSizeT == 0 ? 1 : NORMALIZE_ONE_BLK_SIZE / typeSizeT);
        uint32_t rLengthWithPadding = (rLength + typeAlignNum - NORMALIZE_ONE_NUMBER) / typeAlignNum * typeAlignNum;
        maxValue = GetNormalizeMaxTmpSize(aLength, rLengthWithPadding);
        minValue = GetNormalizeMinTmpSize(aLength, rLengthWithPadding, typeSizeU);
    }
}
} // namespace AscendC
