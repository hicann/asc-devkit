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
 * \file sys_macros.h
 * \brief
 */
#ifndef INCLUDE_UTILS_BASE_SYS_MACROS_H
#define INCLUDE_UTILS_BASE_SYS_MACROS_H

#include "impl/utils/sys_macros.h"

#ifndef likely
#define likely(x) __builtin_expect(!!(x), 1)
#endif

#ifndef unlikely
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif

#define PRELOAD(len) ASCENDC_PRELOAD_IMPL(len)

#if defined(__NPU_ARCH__) &&                                                                                      \
    ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || (__NPU_ARCH__ == 5101) || (__NPU_ARCH__ == 5161) || \
     (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
#define KERNEL_TASK_TYPE(key, value)
#define KERNEL_TASK_TYPE_DEFAULT(value)
#else
#define KERNEL_TASK_TYPE(key, value) ASCENDC_KERNEL_TASK_TYPE_IMPL(key, value)
#ifndef __PLUGIN__KERNEL_TASK_TYPE_DEFAULT_DEFINED__
#define __PLUGIN__KERNEL_TASK_TYPE_DEFAULT_DEFINED__
#define KERNEL_TASK_TYPE_DEFAULT(value) ASCENDC_KERNEL_TASK_TYPE_DEFAULT_IMPL(value)
#endif
#endif

#define REGISTER_TILING_DEFAULT(tiling_struct) ASCENDC_REGISTER_TILING_DEFAULT_IMPL(tiling_struct)
#define REGISTER_TILING_FOR_TILINGKEY(expression, tiling_struct) \
    ASCENDC_REGISTER_TILING_FOR_TILINGKEY_IMPL(expression, tiling_struct)
#define REGISTER_NONE_TILING ASCENDC_REGISTER_NONE_TILING_IMPL

#define TILING_KEY_IS(k) ASCENDC_TILING_KEY_IS_IMPL(k)
#define TILING_KEY_LIST(...) ASCENDC_TILING_KEY_LIST_IMPL(__VA_ARGS__)

#endif
