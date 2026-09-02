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
 * \file asc_aicore_assert_impl.h
 * \brief
 */

#ifndef IMPL_UTILS_DEBUG_ASC_AICORE_ASSERT_IMPL_H
#define IMPL_UTILS_DEBUG_ASC_AICORE_ASSERT_IMPL_H

#ifndef ASCENDC_CPU_DEBUG
#include "impl/utils/debug/asc_aicore_printf_impl.h"

namespace __asc_aicore {
inline __aicore__ void __assert_fail(
    const __gm__ char* __assertion, const __gm__ char* __file, unsigned int __line, const __gm__ char* __function)
{
    printf_impl_assert("[ASSERT] %s:%u: %s: Assertion '%s' failed.\n", __file, __line, __function, __assertion);
    trap();
}

template <typename... Args>
inline __aicore__ void __assert_fail_msg(
    const __gm__ char* __assertion, const __gm__ char* __file, unsigned int __line, const __gm__ char* __function,
    const __gm__ char* fmt, Args&&... args)
{
    printf_impl_assert_msg(__assertion, __file, __line, __function, fmt, args...);
    trap();
}
} // namespace __asc_aicore
#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_AICORE_ASSERT_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_AICORE_ASSERT_IMPL__
#endif

#endif
