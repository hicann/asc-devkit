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
 * \file is_integral.h
 * \brief
 */
#ifndef IMPL_STD_ASCENDC_STD_IS_INTEGRAL_IMPL__H
#define IMPL_STD_ASCENDC_STD_IS_INTEGRAL_IMPL__H

#include "is_same.h"
#include "remove_cv.h"
#include "integral_constant.h"

namespace AscendC {
namespace Std {

template <typename T>
struct is_integral {
private:
    template <typename Tp, typename... Tps>
    __host__ __aicore__ inline static constexpr bool IsUnqualifiedAnyOf()
    {
        return (... || is_same_v<remove_cv_t<Tp>, Tps>);
    }

public:
    static constexpr bool value = IsUnqualifiedAnyOf<
        T, bool, unsigned long long, long long, unsigned long, long, unsigned int, int, unsigned short, short,
        unsigned char, signed char, char>();
};

template <typename T, T v>
struct is_integral<integral_constant<T, v>> : true_type {};

template <typename T>
constexpr bool is_integral_v = is_integral<T>::value;

} // namespace Std
} // namespace AscendC

#endif // IMPL_STD_ASCENDC_STD_IS_INTEGRAL_IMPL__H
