/**
* Copyright (c) 2025 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

/*!
 * \file cmath.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "cmath.h cannot be used with compile flag --enable-simt enabled."
#endif

#ifndef AICORE_UTILS_STD_CMATH_H
#define AICORE_UTILS_STD_CMATH_H

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <cstdint>
#include "stub_def.h"
#endif

namespace AscendC {
#if defined(__NPU_ARCH__) &&                                                                                      \
        ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) ||  \
         (__NPU_ARCH__ == 5101) || (__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163)) || \
    defined(__ASC_NPU_HOST__)
    __aicore__ constexpr inline int32_t CeilDivision(int32_t num1, int32_t num2);
#else
    __aicore__ inline int32_t CeilDivision(int32_t num1, int32_t num2);
#endif
namespace Std {
template <typename T> __aicore__ inline T sqrt(const T src);
template <typename T> __aicore__ inline T abs(const T src);

/*
 * @ingroup Cmath
 * @brief Divide a by b and return the ceiling of the quotient
 * @param [in] a dividend
 * @param [in] b divisor
 * @return Ceiling of the quotient a / b
 * @note For signed inputs, callers must ensure a >= 0 and b > 0.
 */
template <typename T, typename U> __host__ __aicore__ inline constexpr auto ceil_div(const T& a, const U& b);

}
}

#include "impl/utils/std/cmath/sqrt.h"
#include "impl/utils/std/cmath/abs.h"
#include "impl/utils/std/cmath/ceil_division.h"
#include "impl/utils/std/cmath/ceil_align.h"
#include "impl/utils/std/cmath/divide.h"

#endif  // AICORE_UTILS_STD_CMATH_H
