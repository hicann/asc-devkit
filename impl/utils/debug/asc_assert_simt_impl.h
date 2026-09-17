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
 * \file asc_assert_simt_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_ASSERT_SIMT_IMPL__
#warning "impl/utils/debug/asc_assert_simt_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "utils/debug/asc_assert.h" and use public functions or variables defined in interface header files."
#endif

#ifndef IMPL_UTILS_DEBUG_ASC_ASSERT_SIMT_IMPL_H
#define IMPL_UTILS_DEBUG_ASC_ASSERT_SIMT_IMPL_H

#include "impl/utils/debug/asc_printf_simt_impl.h"

namespace __asc_simt_vf {
#ifdef __NPU_COMPILER_INTERNAL_PURE_SIMT__
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void __trap()
{
#ifndef ASCENDC_CPU_DEBUG
    *((uint8_t*)-1) = 0;
#endif
}

static __attribute__((noinline)) __SIMT_DEVICE_FUNCTIONS_DECL__ void __assert_fail(
    const char* __assertion, const char* __file, unsigned int __line, const char* __function) noexcept
{
    simt_printf_impl(
        DumpType::DUMP_SIMT_ASSERT, "[ASSERT] %s:%u: %s: Assertion `%s' failed.\n", __file, __line, __function,
        __assertion);
    __trap();
}

template <typename... Args>
static __attribute__((noinline)) __SIMT_DEVICE_FUNCTIONS_DECL__ void __assert_fail_msg(
    const char* __assertion, const char* __file, unsigned int __line, const char* __function, const char* fmt,
    Args&&... args) noexcept
{
    simt_printf_impl_assert_msg(
        "[ASSERT] %s:%u: %s: Assertion `%s' failed. ", __file, __line, __function, __assertion, fmt, args...);
    __trap();
}
#else
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void __trap()
{
#ifndef ASCENDC_CPU_DEBUG
    *((uint8_t __gm__*)-1) = 0;
#endif
}

static __attribute__((noinline)) __SIMT_DEVICE_FUNCTIONS_DECL__ void __assert_fail(
    const __gm__ char* __assertion, const __gm__ char* __file, unsigned int __line,
    const __gm__ char* __function) noexcept
{
    simt_printf_impl(
        DumpType::DUMP_SIMT_ASSERT, "[ASSERT] %s:%u: %s: Assertion `%s' failed.\n", __file, __line, __function,
        __assertion);
    __trap();
}

template <typename... Args>
static __attribute__((noinline)) __SIMT_DEVICE_FUNCTIONS_DECL__ void __assert_fail_msg(
    const __gm__ char* __assertion, const __gm__ char* __file, unsigned int __line, const __gm__ char* __function,
    const __gm__ char* fmt, Args&&... args) noexcept
{
    simt_printf_impl_assert_msg(
        "[ASSERT] %s:%u: %s: Assertion `%s' failed. ", __file, __line, __function, __assertion, fmt, args...);
    __trap();
}
#endif
} // namespace __asc_simt_vf

#endif // IMPL_UTILS_DEBUG_ASC_ASSERT_SIMT_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_ASSERT_SIMT_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_ASSERT_SIMT_IMPL__
#endif
