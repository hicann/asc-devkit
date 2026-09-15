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
 * \file math_functions_debug.cpp
 * \brief
 */

#if defined(ASCENDC_CPU_DEBUG)
#include "stub_def.h"
#include <cmath>
#include <dlfcn.h>

constexpr uint32_t float_inf = 0x7F800000;
constexpr uint32_t float_neg_inf = 0xFF800000;
constexpr uint32_t float_max_nan = 0x7FFFFFFF;
constexpr uint32_t sign_bit_shift = 31;
constexpr uint32_t sign_bit_mask = 1;
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510))
float __expf(float x) { return exp(x); }

float __logf(float x) { return log(x); }

typedef float sqrtfPtr(float);

float __sqrtf(float x)
{
#if defined(UT_TEST) || defined(ST_TEST)
    return sqrt(x);
#else
    void* handle = dlopen("libm.so.6", RTLD_LAZY);
    if (handle == nullptr) {
        printf("dlopen failed, strerr: %s", dlerror());
    }
    void* func = dlsym(handle, "sqrtf");
    float tmp = ((sqrtfPtr*)func)(x);
    dlclose(handle);
    return tmp;
#endif
}

bool __isnan(float x)
{
    uint32_t* intX = (uint32_t*)&x;
    return (*intX > float_inf && *intX <= float_max_nan) || (*intX > float_neg_inf);
}

bool __isinf(float x)
{
    uint32_t* intX = (uint32_t*)&x;
    return (*intX == float_inf) || (*intX == float_neg_inf);
}

bool __isfinite(float x) { return !__isnan(x) && !__isinf(x); }

float __powf(float x, float y)
{
    if (x < 0.0f) {
        return NAN;
    }

    if (!(fabs(x) < 1.0f || fabs(x) > 1.0f) && std::isinf(y)) {
        return NAN;
    }
    if (!(x < 1.0f || x > 1.0f) && std::isnan(y)) {
        return NAN;
    }

    if ((!(x < 0.0f || x > 0.0f) || std::isnan(x) || std::isinf(x)) && !(y < 0.0f || y > 0.0f)) {
        return NAN;
    }

    return pow(x, y);
}

float __fmaxf(float x, float y)
{
    if (x > y) {
        return x;
    } else {
        return y;
    }
}

float __fminf(float x, float y)
{
    if (x < y) {
        return x;
    } else {
        return y;
    }
}

long long int max(const long long int x, const long long int y)
{
    if (x > y) {
        return x;
    } else {
        return y;
    }
}
unsigned long long int max(const unsigned long long int x, const unsigned long long int y)
{
    if (x > y) {
        return x;
    } else {
        return y;
    }
}

unsigned int max(const unsigned int x, const unsigned int y)
{
    if (x > y) {
        return x;
    } else {
        return y;
    }
}

long long int min(const long long int x, const long long int y)
{
    if (x < y) {
        return x;
    } else {
        return y;
    }
}

unsigned long long int min(const unsigned long long int x, const unsigned long long int y)
{
    if (x < y) {
        return x;
    } else {
        return y;
    }
}

unsigned int min(const unsigned int x, const unsigned int y)
{
    if (x < y) {
        return x;
    } else {
        return y;
    }
}

int signbitf(float x)
{
    union {
        float f;
        uint32_t u;
    } data;
    data.f = x;
    return (data.u >> sign_bit_shift) & sign_bit_mask;
}

typedef float roundfPtr(float);

float __roundf(float x)
{
#if defined(UT_TEST) || defined(ST_TEST)
    return round(x);
#else
    void* handle = dlopen("libm.so.6", RTLD_LAZY);
    if (handle == nullptr) {
        printf("dlopen failed, strerr: %s", dlerror());
    }
    void* func = dlsym(handle, "roundf");
    float tmp = ((roundfPtr*)func)(x);
    dlclose(handle);
    return tmp;
#endif
}

float __rintf(float x) { return rint(x); }

float __floorf(float x) { return floor(x); }

float __ceilf(float x) { return ceil(x); }

float __fma(float x, float y, float z) { return fma(x, y, z); }

float __fabsf(float x)
{
    if (x < 0.0f) {
        return -x;
    } else {
        return x;
    }
}
#endif
#endif
