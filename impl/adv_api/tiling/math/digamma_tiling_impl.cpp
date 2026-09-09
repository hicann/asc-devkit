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
 * \file digamma_tiling_impl.cpp
 * \brief
 */
#include "adv_api/utils/types.h"
#include "adv_api/math/digamma_tiling.h"
#include "utils/tiling/platform/platform_ascendc.h"
#include "../../detail/host_log.h"

namespace AscendC {
namespace {
constexpr uint32_t DIGAMMA_ONE_REPEAT_BYTE_SIZE = 256;
constexpr uint32_t DIGAMMA_HALF_CALC_FAC = 8U * 2U;
constexpr uint32_t DIGAMMA_FLOAT_NOREUSE_CALC_PROC = 7U;
constexpr uint32_t DIGAMMA_FLOAT_REUSE_CALC_PROC = 6U;
constexpr uint32_t DIGAMMA_HALF_CALC_FAC_Arch3510 = 4U;

inline uint32_t GetMaxTmpSize(const uint32_t inputSize, const uint32_t typeSize, const bool isReuseSource)
{
    uint32_t calcPro = 0U;
    if (typeSize == sizeof(float)) {
        calcPro = isReuseSource ? DIGAMMA_FLOAT_REUSE_CALC_PROC : DIGAMMA_FLOAT_NOREUSE_CALC_PROC;
    } else {
        calcPro = DIGAMMA_HALF_CALC_FAC;
    }
    return calcPro * std::max(inputSize * typeSize, DIGAMMA_ONE_REPEAT_BYTE_SIZE);
}

inline uint32_t GetMaxTmpSizeArch3510(const uint32_t inputSize, const uint32_t typeSize)
{
    if (typeSize == 2U) {
        return DIGAMMA_HALF_CALC_FAC_Arch3510 * std::max(inputSize * typeSize, DIGAMMA_ONE_REPEAT_BYTE_SIZE);
    }
    return 0;
}

inline uint32_t GetMinTmpSize(const uint32_t typeSize, const bool isReuseSource)
{
    if (typeSize == sizeof(float)) {
        return isReuseSource ? DIGAMMA_FLOAT_REUSE_CALC_PROC * DIGAMMA_ONE_REPEAT_BYTE_SIZE :
                               DIGAMMA_FLOAT_NOREUSE_CALC_PROC * DIGAMMA_ONE_REPEAT_BYTE_SIZE;
    } else {
        return DIGAMMA_HALF_CALC_FAC * DIGAMMA_ONE_REPEAT_BYTE_SIZE;
    }
}
} // namespace

void GetDigammaTmpBufferFactorSize(const uint32_t typeSize, uint32_t& maxLiveNodeCount, uint32_t& extraBuffer)
{
    ASCENDC_HOST_ASSERT(
        platform_ascendc::PlatformAscendCManager::GetInstance() != nullptr, return,
        "PlatformAscendCManager gets instance failed!");
    const auto npuArch = platform_ascendc::PlatformAscendCManager::GetInstance()->GetCurNpuArch();
    if (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201 || npuArch == NpuArch::DAV_5102) {
        extraBuffer = uint32_t(0);
        maxLiveNodeCount = (typeSize == sizeof(float)) ? 0 : DIGAMMA_HALF_CALC_FAC_Arch3510;
    } else {
        extraBuffer = uint32_t(0);
        maxLiveNodeCount = (typeSize == sizeof(float)) ? DIGAMMA_FLOAT_NOREUSE_CALC_PROC : DIGAMMA_HALF_CALC_FAC;
    }
}

void GetDigammaMaxMinTmpSize(
    const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue,
    uint32_t& minValue)
{
    const uint32_t inputSize = srcShape.GetShapeSize();
    ASCENDC_HOST_ASSERT(inputSize > 0, return, "Input Shape size must be greater than 0.");
    ASCENDC_HOST_ASSERT(
        platform_ascendc::PlatformAscendCManager::GetInstance() != nullptr, return,
        "PlatformAscendCManager gets instance failed!");

    const auto npuArch = platform_ascendc::PlatformAscendCManager::GetInstance()->GetCurNpuArch();
    if (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201 || npuArch == NpuArch::DAV_5102) {
        maxValue = GetMaxTmpSizeArch3510(inputSize, typeSize);
        minValue = maxValue;
        return;
    }
    maxValue = GetMaxTmpSize(inputSize, typeSize, isReuseSource);
    minValue = GetMinTmpSize(typeSize, isReuseSource);
}
} // namespace AscendC
