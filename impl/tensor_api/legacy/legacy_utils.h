/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning                                                                                                               \
    "impl/tensor_api/legacy/legacy_utils.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_LEGACY_UTILS_H
#endif

#ifndef IMPL_TENSOR_API_LEGACY_LEGACY_UTILS_H
#define IMPL_TENSOR_API_LEGACY_LEGACY_UTILS_H

namespace asc {
namespace te {

enum class CacheMode : uint8_t {
    CACHE_MODE_NORMAL = 0,
    CACHE_MODE_DISABLE = 4,
    CACHE_MODE_LAST = 5,
    CACHE_MODE_PERSISTENT = 6
};
enum class MmadType : uint8_t { NORMAL = 0, MX = 1 };
enum class RoundMode : uint8_t { DEFAULT = 0, HYBRID = 1 };

enum DualDstMode : uint8_t { DUAL_DST_DISABLE = 0, DUAL_DST_SPLIT_M = 1, DUAL_DST_SPLIT_N = 2 };

struct CopyL0C2GMTrait {
    __aicore__ constexpr CopyL0C2GMTrait() {}

    __aicore__ constexpr CopyL0C2GMTrait(RoundMode roundMode, bool enableRelu, bool enableChannelSplit) :
        roundMode(roundMode), enableRelu(enableRelu), enableChannelSplit(enableChannelSplit)
    {}

    RoundMode roundMode = RoundMode::DEFAULT;
    bool enableRelu = false;
    bool enableChannelSplit = false;
};

struct CopyL0C2UBTrait {
    __aicore__ constexpr CopyL0C2UBTrait() {}

    __aicore__ constexpr CopyL0C2UBTrait(RoundMode roundMode, bool enableRelu, bool enableChannelSplit,
                                         DualDstMode dualDstCtl) :
        roundMode(roundMode), enableRelu(enableRelu), enableChannelSplit(enableChannelSplit),
        dualDstCtl(dualDstCtl)
    {}

    RoundMode roundMode = RoundMode::DEFAULT;
    bool enableRelu = false;
    bool enableChannelSplit = false;
    DualDstMode dualDstCtl = DUAL_DST_DISABLE;
};

struct CopyL0C2L1Trait {
    __aicore__ constexpr CopyL0C2L1Trait() {}

    __aicore__ constexpr CopyL0C2L1Trait(RoundMode roundMode, bool enableRelu, bool enableChannelSplit) :
        roundMode(roundMode), enableRelu(enableRelu), enableChannelSplit(enableChannelSplit)
    {}

    RoundMode roundMode = RoundMode::DEFAULT;
    bool enableRelu = false;
    bool enableChannelSplit = false;
};

struct MmadTrait {
    __aicore__ constexpr MmadTrait(){};

    __aicore__ constexpr MmadTrait(int32_t fmOffset, bool kDirectionAlign, bool cmatrixSource, bool disableGemv,
                                   MmadType mmadType) :
        fmOffset(fmOffset), kDirectionAlign(kDirectionAlign), cmatrixSource(cmatrixSource),
        disableGemv(disableGemv), mmadType(mmadType)
    {}

    int32_t fmOffset = 0;
    bool kDirectionAlign = false;
    bool cmatrixSource = false;
    bool disableGemv = true;
    MmadType mmadType = MmadType::NORMAL;
};

struct MmadParams {
    __aicore__ constexpr MmadParams(){};

    __aicore__ constexpr MmadParams(uint16_t m, uint16_t n, uint16_t k, uint8_t unitFlag,
                                    bool cmatrixInitVal) :
        m(m), n(n), k(k), unitFlag(unitFlag), cmatrixInitVal(cmatrixInitVal)
    {}

    uint16_t m = 0;
    uint16_t n = 0;
    uint16_t k = 0;
    uint8_t unitFlag = 0;
    bool cmatrixInitVal = false;
};

struct FixpipeParams {
    __aicore__ constexpr FixpipeParams(){};

    __aicore__ constexpr FixpipeParams(uint8_t unitFlag, bool subBlockId = false) :
        unitFlag(unitFlag), subBlockId(subBlockId)
    {}

    uint8_t unitFlag = 0;
    bool subBlockId = false;
};

struct CopyGM2UBParams {
    __aicore__ constexpr CopyGM2UBParams() {}

    __aicore__ constexpr CopyGM2UBParams(uint8_t leftPaddingCount, uint8_t rightPaddingCount,
                                         bool enableConstantPad = true) :
        leftPaddingCount(leftPaddingCount), rightPaddingCount(rightPaddingCount),
        enableConstantPad(enableConstantPad)
    {}

    uint8_t leftPaddingCount = 0;
    uint8_t rightPaddingCount = 0;
    bool enableConstantPad = true;
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

} // namespace te
} // namespace asc

namespace AscendC {
namespace Te = asc::te;
}

#endif // IMPL_TENSOR_API_LEGACY_LEGACY_UTILS_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_LEGACY_UTILS_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_LEGACY_UTILS_H
#endif
