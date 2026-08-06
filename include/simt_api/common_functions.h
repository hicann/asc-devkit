/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

#ifndef INCLUDE_SIMT_API_COMMON_FUNCTIONS_H
#define INCLUDE_SIMT_API_COMMON_FUNCTIONS_H

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_COMMON_FUNCTIONS_H__
#endif

#include "simt_api/device_types.h"
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
#include "impl/utils/debug/asc_printf_simt_impl.h"
#endif
#include "utils/debug/asc_assert.h"
#include "utils/debug/asc_time.h"

#ifndef SPLIT_CORE_VEC
#if (defined(__DAV_VEC__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510)) 
#define SPLIT_CORE_VEC
#endif
#endif

#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
using dim3 = cce::dim3;
#if (__NPU_ARCH__ == 3510)

template <auto funcPtr, typename... Args>
__aicore__ inline void asc_vf_call(dim3 threadNums, Args &&...args)
{
#if defined(SPLIT_CORE_VEC) || defined(ASCENDC_CPU_DEBUG)
    cce::async_invoke<funcPtr>(threadNums, args...);
#endif
}
#endif
#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_COMMON_FUNCTIONS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_COMMON_FUNCTIONS_H__
#endif
#endif  // INCLUDE_SIMT_API_COMMON_FUNCTIONS_H