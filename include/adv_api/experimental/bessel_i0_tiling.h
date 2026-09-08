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
 * \file bessel_i0_tiling.h
 * \brief Host-side temporary-space query interfaces of the BesselI0 high-level API.
 *
 * BesselI0 needs no temporary space: both query interfaces always return 0, where 0 means
 * no temporary space is required.
 */
#ifndef INCLUDE_ADV_API_EXPERIMENTAL_BESSEL_I0_TILING_H
#define INCLUDE_ADV_API_EXPERIMENTAL_BESSEL_I0_TILING_H

#include <cstdint>

#include "../utils/types.h"

namespace AscendC {
namespace experimental {

/*!
 * \brief Query the maximum/minimum temporary space (sharedTmpBuffer), in bytes, required by
 * BesselI0. BesselI0 needs no temporary space, so maxValue and minValue are always 0.
 *
 * \param [in] srcShape: input shape information.
 * \param [in] typeSize: size of the input data type, in bytes (e.g. 4 for float).
 * \param [in] isReuseSource: whether the caller allows modifying the source operand, consistent
 *        with the isReuseSource template parameter of the BesselI0 interface.
 * \param [out] maxValue: maximum temporary space required, always 0.
 * \param [out] minValue: minimum temporary space required, always 0.
 */
void GetBesselI0MaxMinTmpSize(
    const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue,
    uint32_t& minValue);

/*!
 * \brief Query the temporary-space scaling factors of BesselI0: maxLiveNodeCount indicates how
 * many times the temporary space is relative to the space of a single computation, and
 * extraBuffer is the extra temporary space size in bytes. BesselI0 needs no temporary space,
 * so both outputs are always 0.
 *
 * \param [in] typeSize: size of the input data type, in bytes (e.g. 4 for float).
 * \param [out] maxLiveNodeCount: maximum number of live nodes used for temporary-space
 *        estimation, always 0.
 * \param [out] extraBuffer: size of the extra temporary space, always 0.
 */
void GetBesselI0TmpBufferFactorSize(const uint32_t typeSize, uint32_t& maxLiveNodeCount, uint32_t& extraBuffer);

} // namespace experimental
} // namespace AscendC

#endif // INCLUDE_ADV_API_EXPERIMENTAL_BESSEL_I0_TILING_H
