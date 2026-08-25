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
 * \file lgamma_tiling.cpp
 * \brief
 */
#include "adv_api/utils/types.h"
#include "adv_api/math/lgamma_tiling.h"
#include "utils/tiling/platform/platform_ascendc.h"
#include "../../detail/host_log.h"

namespace AscendC {
namespace {
constexpr uint32_t ONE_REPEAT_BYTE_SIZE = 256;
constexpr uint32_t HALF_CALC_FAC = 13U;
constexpr uint32_t FLOAT_NOREUSE_CALC_PROC = 8U;
constexpr uint32_t FLOAT_REUSE_CALC_PROC = 7U;
constexpr uint32_t NUM_TWO = 2U;
constexpr uint32_t LGAMMA_FLOAT_SIZE = 4;

inline uint32_t GetMaxTmpSize(const uint32_t inputSize, const uint32_t typeSize, const bool isReuseSource)
{
    uint32_t calcPro = 0U;
    if (typeSize == sizeof(float)) {
        calcPro = isReuseSource ? FLOAT_REUSE_CALC_PROC : FLOAT_NOREUSE_CALC_PROC;
    } else {
        calcPro = HALF_CALC_FAC * NUM_TWO;
    }
    return calcPro * std::max(inputSize * typeSize, ONE_REPEAT_BYTE_SIZE);
}

inline uint32_t GetMinTmpSize(const uint32_t typeSize, const bool isReuseSource)
{
    if (typeSize == sizeof(float)) {
        return isReuseSource ? FLOAT_REUSE_CALC_PROC * ONE_REPEAT_BYTE_SIZE :
                               FLOAT_NOREUSE_CALC_PROC * ONE_REPEAT_BYTE_SIZE;
    } else {
        return HALF_CALC_FAC * ONE_REPEAT_BYTE_SIZE * NUM_TWO;
    }
}
} // namespace

void GetLgammaTmpBufferFactorSize(const uint32_t typeSize, uint32_t& maxLiveNodeCount, uint32_t& extraBuffer)
{
    extraBuffer = 0;
    maxLiveNodeCount = (typeSize == sizeof(float)) ? FLOAT_NOREUSE_CALC_PROC : HALF_CALC_FAC; // for half
}

void GetLgammaMaxMinTmpSize(
    const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue,
    uint32_t& minValue)
{
    const uint32_t inputSize = srcShape.GetShapeSize();
    ASCENDC_HOST_ASSERT(inputSize > 0, return, "Input Shape size must be greater than 0.");
    platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
    ASCENDC_HOST_ASSERT((platform != nullptr), return, "Failed to get PlatformAscendC.");
    const auto npuArch = platform->GetCurNpuArch();
    if (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_5102 || npuArch == NpuArch::DAV_3003 ||
        npuArch == NpuArch::DAV_3113) {
        minValue = inputSize * LGAMMA_FLOAT_SIZE;
        maxValue = minValue;
    } else {
        maxValue = GetMaxTmpSize(inputSize, typeSize, isReuseSource);
        minValue = GetMinTmpSize(typeSize, isReuseSource);
        maxValue = std::max(maxValue, minValue);
    }
}
} // namespace AscendC
