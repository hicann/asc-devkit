/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file bessel_i0_tiling_impl.cpp
 * \brief Host-side temporary-space query interface implementation of the BesselI0 high-level API.
 *
 * BesselI0 needs no temporary space, so both query functions always return 0.
 */

#include "adv_api/utils/types.h"

#include "adv_api/experimental/bessel_i0_tiling.h"

#include <cstdint>

namespace AscendC {
namespace experimental {

void GetBesselI0MaxMinTmpSize(
    const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue,
    uint32_t& minValue)
{
    (void)srcShape;
    (void)typeSize;
    (void)isReuseSource;
    maxValue = 0;
    minValue = 0;
}

void GetBesselI0TmpBufferFactorSize(const uint32_t typeSize, uint32_t& maxLiveNodeCount, uint32_t& extraBuffer)
{
    (void)typeSize;
    maxLiveNodeCount = 0;
    extraBuffer = 0;
}

} // namespace experimental
} // namespace AscendC
