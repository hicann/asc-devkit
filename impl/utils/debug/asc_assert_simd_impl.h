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
 * \file asc_assert_simd_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_ASSERT_SIMD_IMPL__
#warning \
    "impl/utils/debug/asc_assert_simd_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"utils/debug/asc_assert.h\" and use public functions or variables defined in interface header files."
#endif

#ifndef IMPL_UTILS_DEBUG_ASC_ASSERT_SIMD_IMPL_H
#define IMPL_UTILS_DEBUG_ASC_ASSERT_SIMD_IMPL_H

#if !defined(ASCENDC_CPU_DEBUG)
#include "impl/utils/debug/asc_simd_printf_impl.h"
#endif

namespace __asc_simd_vf {
__simd_callee__ inline void __trap()
{
#if !defined(ASCENDC_CPU_DEBUG)
    *((volatile __ubuf__ uint8_t*)-1) = 0;
#endif
}

#if !defined(ASCENDC_CPU_DEBUG)
__ASC_USE_RESERVED_UBUF__(3510, "assert is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
static __attribute__((noinline)) __simd_callee__ void __assert_fail(
    __ubuf__ const char* assertion, __ubuf__ const char* file, unsigned int line,
    __ubuf__ const char* function) noexcept
{
    printf_impl_assert("[ASSERT] %s:%u: %s: Assertion '%s' failed.\n", file, line, function, assertion);
    wait_vf_assert_handshake();
    __trap();
}

template <typename... Args>
__ASC_USE_RESERVED_UBUF__(3510, "assert is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
static __attribute__((noinline)) __simd_callee__ void __assert_fail_msg(
    __ubuf__ const char* assertion, __ubuf__ const char* file, unsigned int line, __ubuf__ const char* function,
    __ubuf__ const char* fmt, Args&&... args) noexcept
{
    printf_impl_assert_msg(assertion, file, line, function, fmt, args...);
    wait_vf_assert_handshake();
    __trap();
}
#endif
} // namespace __asc_simd_vf

#endif // IMPL_UTILS_DEBUG_ASC_ASSERT_SIMD_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_ASSERT_SIMD_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_ASSERT_SIMD_IMPL__
#endif
