/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file math_constants_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_CONSTANTS_IMPL__
#warning "impl/simt_api/math_constants_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "simt_api/math_constants.h" and use public functions or variables defined in interface header files."
#endif

#ifndef IMPL_SIMT_API_MATH_CONSTANTS_IMPL_H
#define IMPL_SIMT_API_MATH_CONSTANTS_IMPL_H

#if defined(ASCENDC_CPU_DEBUG)
#include "simt_stub.h"
#endif

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 9201) || (__NPU_ARCH__ == 9202) || (__NPU_ARCH__ == 5102)

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
__aicore__ inline float __internal_int_as_float(const int x)
#else
__simt_callee__ inline float __internal_int_as_float(const int x)
#endif
{
    union Data {
        float f;
        int i;
    };
    union Data data = {.i = x};
    return data.f;
}
#endif
#endif // IMPL_SIMT_API_MATH_CONSTANTS_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_CONSTANTS_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_CONSTANTS_IMPL__
#endif
