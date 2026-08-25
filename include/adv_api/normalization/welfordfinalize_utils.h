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
 * \file welfordfinalize_utils.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "welfordfinalize_utils.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_WELFORDFINALIZE_UTILS_H__
#endif

#ifndef LIB_NORMALIZATION_WELFORDFINALIZE_UTILS_H
#define LIB_NORMALIZATION_WELFORDFINALIZE_UTILS_H

#include <cstdint>
#include "kernel_basic_intf.h"

namespace AscendC {
struct WelfordFinalizeConfig {
    __aicore__ constexpr WelfordFinalizeConfig(const bool isCorrectionIn) { isCorrection = isCorrectionIn; }
    bool isCorrection = false;
};

constexpr WelfordFinalizeConfig WFFINALIZE_DEFAULT_CFG = {false};

struct WelfordFinalizePara {
    uint32_t rnLength;
    uint32_t abLength;
    uint32_t headCount;
    uint32_t headCountLength;
    uint32_t tailCount;
    uint32_t tailCountLength;
    float abRec;
    float rRec;
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    float rRecWithCorrection;
#endif
};

}; // namespace AscendC
#endif // LIB_NORMALIZATION_WELFORDFINALIZE_UTILS_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_WELFORDFINALIZE_UTILS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_WELFORDFINALIZE_UTILS_H__
#endif
