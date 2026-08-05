/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef DLSYM_COMMON_H
#define DLSYM_COMMON_H

#include "log.h"
#include <dlfcn.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DECL_WEAK_FUNC(type, func_name, ...) type func_name(__VA_ARGS__) __attribute__((weak));

#define DEFINE_WEAK_FUNC(type, func_name, ...)                                           \
    static bool g_##func_name##Supported = false;                                        \
    extern "C" bool HcommIsSupport##func_name(void) { return g_##func_name##Supported; } \
    type func_name(__VA_ARGS__) __attribute__((weak));                                   \
    type func_name(__VA_ARGS__)                                                          \
    {                                                                                    \
        HCCL_ERROR("[HcclWrapper] %s not supported", __func__);                          \
        return (type)(-1);                                                               \
    }

#define DECL_SUPPORT_FLAG(func_name) extern "C" bool HcommIsSupport##func_name(void)

#define INIT_SUPPORT_FLAG(handle, func_name)                                                        \
    do {                                                                                            \
        dlerror();                                                                                  \
        void* ptr = (void*)dlsym(handle, #func_name);                                               \
        const char* err = dlerror();                                                                \
        if (ptr == nullptr) {                                                                       \
            g_##func_name##Supported = false;                                                       \
            HCCL_DEBUG("[HcclWrapper] %s not supported, %s", #func_name, err ? err : "(no error)"); \
        } else {                                                                                    \
            g_##func_name##Supported = true;                                                        \
        }                                                                                           \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif // DLSYM_COMMON_H
