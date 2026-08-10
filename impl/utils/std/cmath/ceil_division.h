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
 * \file ceil_division.h
 * \brief
 */
#ifndef IMPL_STD_CEIL_DIVISION_H
#define IMPL_STD_CEIL_DIVISION_H

namespace AscendC {
namespace Std {

template <typename T, typename U>
__host__ __aicore__ inline constexpr auto ceil_division(const T& num1, const U& num2)
{
    return (num1 + num2 - 1) / num2;
}

template <typename T, typename U>
__host__ __aicore__ inline constexpr auto ceil_div(const T& a, const U& b)
{
    using ReType = decltype(ceil_division(a, b));
    if (b == 0) {
        return ReType(0);
    }
#if defined(__NPU_HOST__)
    return ceil_division(a, b);
#elif defined(__NPU_DEVICE__)
#if defined(__NPU_ARCH__) && \
    ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
#if (defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1) || !defined(SPLIT_CORE_VEC)
    return ceil_division(a, b);
#else
    return ReType(get_repeat_ceiling(a, b));
#endif
#else
    return ceil_division(a, b);
#endif
#else
    return ceil_division(a, b);
#endif
}

} // namespace Std
} // namespace AscendC
#endif
