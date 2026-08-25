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
 * \file utils.h
 * \brief
 */
#ifndef INCLUDE_TENSOR_API_UTILS_UTILS_H
#define INCLUDE_TENSOR_API_UTILS_UTILS_H

#include "utils/base/sys_macros.h"
#include "utils/base/sys_constants.h"
#include "utils/common_types.h"

namespace AscendC {
namespace Te {

enum class CacheMode : uint8_t {
    CACHE_MODE_NORMAL = 0,
    CACHE_MODE_DISABLE = 4,
    CACHE_MODE_LAST,
    CACHE_MODE_PERSISTENT
};

enum class MmadType : uint8_t { NORMAL = 0, MX = 1 };

struct MmadTrait {
    __aicore__ constexpr MmadTrait(){};

    __aicore__ constexpr MmadTrait(
        int32_t fmOffsetIn, bool kDirectionAlignIn, bool cmatrixSourceIn, bool disableGemvIn, MmadType mmadTypeIn)
    {
        fmOffset = fmOffsetIn;
        kDirectionAlign = kDirectionAlignIn;
        cmatrixSource = cmatrixSourceIn;
        disableGemv = disableGemvIn;
        mmadType = mmadTypeIn;
    };

    int32_t fmOffset = 0;
    bool kDirectionAlign = false;
    bool cmatrixSource = false;
    bool disableGemv = true;
    MmadType mmadType = MmadType::NORMAL;
};

struct MmadParams {
    __aicore__ constexpr MmadParams(){};

    __aicore__ constexpr MmadParams(uint16_t mIn, uint16_t nIn, uint16_t kIn, uint8_t unitFlagIn, bool cmatrixInitValIn)
        : m(mIn), n(nIn), k(kIn), unitFlag(unitFlagIn), cmatrixInitVal(cmatrixInitValIn){};

    uint16_t m = 0;
    uint16_t n = 0;
    uint16_t k = 0;
    uint8_t unitFlag = 0;
    bool cmatrixInitVal = false;
};

enum class RoundMode : uint8_t { DEFAULT = 0, HYBRID };

enum DualDstMode : uint8_t { DUAL_DST_DISABLE = 0, DUAL_DST_SPLIT_M, DUAL_DST_SPLIT_N };

struct FixpipeParams {
    __aicore__ constexpr FixpipeParams(){};

    __aicore__ constexpr FixpipeParams(uint8_t unitFlagIn, bool subBlockIdIn = false)
        : unitFlag(unitFlagIn), subBlockId(subBlockIdIn){};

    uint8_t unitFlag = 0;

    bool subBlockId = false;
};

template <typename T>
struct Img2ColParams {
    uint16_t mExtension = 0;
    uint16_t kExtension = 0;
    uint16_t mStartPt = 0;
    uint16_t kStartPt = 0;
    uint8_t padList[4] = {0, 0, 0, 0};
    uint8_t strideW = 1;
    uint8_t strideH = 1;
    uint8_t filterW = 1;
    uint8_t filterH = 1;
    uint8_t dilationFilterW = 1;
    uint8_t dilationFilterH = 1;
    bool filterSizeW = false;
    bool filterSizeH = false;
    bool transpose = false;
    bool fMatrixCtrl = false;
    T padValue = 0;
};

} // namespace Te
} // namespace AscendC

#endif // INCLUDE_TENSOR_API_UTILS_UTILS_H
