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
 * \file asc_assert.h
 * \brief
 */
#ifndef INCLUDE_UTILS_DEBUG_ASC_ASSERT_H
#define INCLUDE_UTILS_DEBUG_ASC_ASSERT_H

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_COMPILER_INTERNAL_HEADERS_ASC_ASSERT_H__
#endif

#include <cassert>
#include "impl/utils/sys_macros.h"
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
#include "simt_api/device_types.h"
#include "impl/utils/debug/asc_assert_simt_impl.h"

namespace __asc_simt_vf {
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void __trap();
} // namespace __asc_simt_vf
#endif

#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
#if (__NPU_ARCH__ == 2002) || (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
#include "impl/utils/debug/asc_aicore_assert_impl.h"
#endif
#endif

#define ASC_INTERNAL_COUNT_ARGS_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, N, ...) N
#define ASC_INTERNAL_COUNT_ARGS(...) ASC_INTERNAL_COUNT_ARGS_IMPL(__VA_ARGS__, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1)
#define ASC_INTERNAL_GET_ARG_COUNT(...) ASC_INTERNAL_COUNT_ARGS(__VA_ARGS__)
#define ASC_INTERNAL_ASSERT_IMPL(N, ...) ASC_INTERNAL_ASSERT_FUNC(N, __VA_ARGS__)
#define ASC_INTERNAL_ASSERT_FUNC(N, ...) ASC_INTERNAL_ASSERT_##N(__VA_ARGS__)
#define ASC_INTERNAL_ASSERT_1(expr) (static_cast<bool>(expr) ? void(0) : __assert_fail(#expr, __FILE__, __LINE__, (__PRETTY_FUNCTION__)))
#ifndef ASCENDC_CPU_DEBUG
#define ASC_INTERNAL_ASSERT_2(expr, fmt, ...) (static_cast<bool>(expr) ? void(0) : __assert_fail_msg(#expr, __FILE__, __LINE__, (__PRETTY_FUNCTION__), fmt, ##__VA_ARGS__))
#else
#define ASC_INTERNAL_ASSERT_2(expr, ...) ASC_INTERNAL_ASSERT_1(expr)
#endif

#undef assert
#undef ascendc_assert
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0) && !(defined(NDEBUG))
#define assert(...) ASC_INTERNAL_ASSERT_IMPL(ASC_INTERNAL_GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)
#define ascendc_assert(...) ASC_INTERNAL_ASSERT_IMPL(ASC_INTERNAL_GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)
#else
#define assert(...)
#define ascendc_assert(...)
#endif


#if defined(__UNDEF_ASCENDC_INCLUDE_COMPILER_INTERNAL_HEADERS_ASC_ASSERT_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_COMPILER_INTERNAL_HEADERS_ASC_ASSERT_H__
#endif

#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__

#if (__NPU_ARCH__ == 3510) && !defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#include "impl/utils/debug/asc_simd_debug_enable.h"
#include "impl/utils/debug/asc_assert_simd_impl.h"
#endif

#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__

#endif
