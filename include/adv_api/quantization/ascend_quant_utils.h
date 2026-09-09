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
 * \file ascend_quant_utils.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "ascend_quant_utils.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASCEND_QUANT_UTILS_H__
#endif

#ifndef LIB_QUANTIZATION_ASCEND_QUANT_UTILS_H
#define LIB_QUANTIZATION_ASCEND_QUANT_UTILS_H

#include "kernel_basic_intf.h"

namespace AscendC {
struct AscendQuantConfig {
    __aicore__ constexpr AscendQuantConfig(
        const uint32_t calcCount, const uint32_t offsetCount, const uint32_t scaleCount, const uint32_t workLocalSize)
        : calcCount(calcCount), offsetCount(offsetCount), scaleCount(scaleCount), workLocalSize(workLocalSize)
    {}
    uint32_t calcCount = 0;
    uint32_t offsetCount = 0;
    uint32_t scaleCount = 0;
    uint32_t workLocalSize = 0;

#if !defined(__NPU_ARCH__) ||                                                                          \
    (defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || \
                               __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113))
    __aicore__ constexpr AscendQuantConfig(
        const uint32_t calcCount, const uint32_t offsetCount, const uint32_t scaleCount, const uint32_t workLocalSize,
        const bool hasOffset, const int32_t kDim, const RoundMode roundMode)
        : calcCount(calcCount),
          offsetCount(offsetCount),
          scaleCount(scaleCount),
          workLocalSize(workLocalSize),
          hasOffset(hasOffset),
          kDim(kDim),
          roundMode(roundMode)
    {}
    __aicore__ constexpr AscendQuantConfig(const bool hasOffset, const int32_t kDim, const RoundMode roundMode)
        : hasOffset(hasOffset), kDim(kDim), roundMode(roundMode)
    {}
    __aicore__ constexpr AscendQuantConfig(const bool hasOffset, const int32_t kDim) : hasOffset(hasOffset), kDim(kDim)
    {}
    __aicore__ constexpr AscendQuantConfig(const bool hasOffset) : hasOffset(hasOffset) {}

    bool hasOffset = false;
    int32_t kDim = 1;
    RoundMode roundMode = RoundMode::CAST_RINT;
#endif
};

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102)
constexpr AscendQuantConfig ASCEND_QUANT_DEFAULT_CFG = {0, 0, 0, 0, false, 1, RoundMode::CAST_RINT};
#else
constexpr AscendQuantConfig ASCEND_QUANT_DEFAULT_CFG = {0, 0, 0, 0};
#endif

enum class AscendQuantPolicy : int32_t {
    PER_TENSOR,
    PER_CHANNEL,
    PER_TOKEN,
    PER_GROUP,
    PER_CHANNEL_PER_GROUP,
    PER_TOKEN_PER_GROUP
};

struct AscendQuantParam {
    uint32_t m;
    uint32_t n;
    uint32_t calCount;
    uint32_t groupSize = 0;
};

}; // namespace AscendC
#endif // LIB_QUANTIZATION_ASCEND_QUANT_UTILS_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASCEND_QUANT_UTILS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASCEND_QUANT_UTILS_H__
#endif
