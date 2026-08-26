/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef INCLUDE_SIMT_API_MATH_FUNCTIONS_H
#define INCLUDE_SIMT_API_MATH_FUNCTIONS_H

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_FUNCTIONS_H__
#endif

#include "simt_api/math_constants.h"
#include "simt_api/device_types.h"

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long int lroundf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int llroundf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long int lrintf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int llrintf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float truncf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float roundf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float rintf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float floorf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float ceilf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float sqrtf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float rsqrtf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float log2f(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float logf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float expf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float fmaf(float x, float y, float z);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float fabsf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float normcdfinvf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float modff(float x, float* n);
#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float modff(float x, __ubuf__ float* n);
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float modff(float x, __gm__ float* n);
#endif

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bool isfinite(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bool isnan(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bool isinf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float fdimf(float x, float y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float remquof(float x, float y, int* quo);
#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float remquof(float x, float y, __ubuf__ int* quo);
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float remquof(float x, float y, __gm__ int* quo);
#endif

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float fmodf(float x, float y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float remainderf(float x, float y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float copysignf(float x, float y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float nearbyintf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float nextafterf(float x, float y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float scalbnf(float x, int32_t n);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float scalblnf(float x, int64_t n);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float fmaxf(float x, float y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float fminf(float x, float y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float nanf(const char* tagp);
#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float nanf(const __gm__ char* tagp);
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float nanf(const __ubuf__ char* tagp);
#endif

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float tanf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float tanhf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float tanpif(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float atanf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float atan2f(float y, float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float atanhf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float cosf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float coshf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float cospif(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asinf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float acosf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float acoshf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float sinf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float sinhf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float sinpif(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asinhf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincosf(float x, float* s, float* c);
#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincosf(float x, float* s, __ubuf__ float* c);
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincosf(float x, float* s, __gm__ float* c);
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincosf(float x, __ubuf__ float* s, float* c);
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincosf(float x, __ubuf__ float* s, __ubuf__ float* c);
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincosf(float x, __ubuf__ float* s, __gm__ float* c);
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincosf(float x, __gm__ float* s, float* c);
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincosf(float x, __gm__ float* s, __ubuf__ float* c);
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincosf(float x, __gm__ float* s, __gm__ float* c);
#endif

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincospif(float x, float* s, float* c);
#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincospif(float x, float* s, __ubuf__ float* c);
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincospif(float x, float* s, __gm__ float* c);
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincospif(float x, __ubuf__ float* s, float* c);
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincospif(float x, __ubuf__ float* s, __ubuf__ float* c);
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincospif(float x, __ubuf__ float* s, __gm__ float* c);
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincospif(float x, __gm__ float* s, float* c);
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincospif(float x, __gm__ float* s, __ubuf__ float* c);
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincospif(float x, __gm__ float* s, __gm__ float* c);
#endif

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float exp2f(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float exp10f(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float expm1f(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float frexpf(float x, int* exp);
#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float frexpf(float x, __ubuf__ int* exp);
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float frexpf(float x, __gm__ int* exp);
#endif

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float ldexpf(float x, int exp);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float hypotf(float x, float y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float rhypotf(float x, float y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float norm3df(float a, float b, float c);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float rnorm3df(float a, float b, float c);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float norm4df(float a, float b, float c, float d);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float rnorm4df(float a, float b, float c, float d);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float normf(int n, float* a);
#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float normf(int n, __ubuf__ float* a);
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float normf(int n, __gm__ float* a);
#endif

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float rnormf(int n, float* a);
#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float rnormf(int n, __ubuf__ float* a);
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float rnormf(int n, __gm__ float* a);
#endif

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float powf(float x, float y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float log10f(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float log1pf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float logbf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int ilogbf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float cbrtf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float rcbrtf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float erff(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float erfcf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float erfinvf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float erfcinvf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float erfcxf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float tgammaf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float lgammaf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float cyl_bessel_i0f(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float cyl_bessel_i1f(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float normcdff(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float j0f(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float j1f(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float jnf(int n, float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float y0f(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float y1f(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float ynf(int n, float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long int labs(long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int llabs(long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int llmax(const long long int x, const long long int y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int ullmax(
    const unsigned long long int x, const unsigned long long int y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int umax(const unsigned int x, const unsigned int y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int llmin(const long long int x, const long long int y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int ullmin(
    const unsigned long long int x, const unsigned long long int y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int umin(const unsigned int x, const unsigned int y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float fdividef(float x, float y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int signbit(float x);

#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
#if (__NPU_ARCH__ == 3510)
#ifndef ASCENDC_CPU_DEBUG
static __callee__ long long max(long long x, long long y);

static __callee__ unsigned long long max(unsigned long long x, unsigned long long y);

static __callee__ unsigned int max(unsigned int x, unsigned int y);

static __callee__ long max(long x, long y);

static __callee__ int max(int x, int y);

static __callee__ short max(short x, short y);

static __callee__ char max(char x, char y);

static __callee__ unsigned long max(unsigned long x, unsigned long y);

static __callee__ unsigned short max(unsigned short x, unsigned short y);

static __callee__ unsigned char max(unsigned char x, unsigned char y);

static __callee__ float max(float x, float y);

static __callee__ long long min(long long x, long long y);

static __callee__ unsigned long long min(unsigned long long x, unsigned long long y);

static __callee__ unsigned int min(unsigned int x, unsigned int y);

static __callee__ long min(long x, long y);

static __callee__ int min(int x, int y);

static __callee__ short min(short x, short y);

static __callee__ char min(char x, char y);

static __callee__ unsigned long min(unsigned long x, unsigned long y);

static __callee__ unsigned short min(unsigned short x, unsigned short y);

static __callee__ unsigned char min(unsigned char x, unsigned char y);

static __callee__ float min(float x, float y);

static __callee__ float abs(float x);
#endif
#endif
#else
static __SIMT_DEVICE_FUNCTIONS_DECL__ long long max(long long x, long long y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned long long max(unsigned long long x, unsigned long long y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int max(unsigned int x, unsigned int y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ long max(long x, long y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ int max(int x, int y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ short max(short x, short y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ char max(char x, char y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned long max(unsigned long x, unsigned long y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned short max(unsigned short x, unsigned short y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned char max(unsigned char x, unsigned char y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ float max(float x, float y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ long long min(long long x, long long y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned long long min(unsigned long long x, unsigned long long y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int min(unsigned int x, unsigned int y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ long min(long x, long y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ int min(int x, int y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ short min(short x, short y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ char min(char x, char y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned long min(unsigned long x, unsigned long y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned short min(unsigned short x, unsigned short y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned char min(unsigned char x, unsigned char y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ float min(float x, float y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ float abs(float x);
#endif

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int max(unsigned int x, int y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int max(int x, unsigned int y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long int max(long int x, unsigned long int y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long int max(unsigned long int x, long int y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int max(long long int x, unsigned long long int y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int max(unsigned long long int x, long long int y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int min(unsigned int x, int y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int min(int x, unsigned int y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long int min(long int x, unsigned long int y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long int min(unsigned long int x, long int y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int min(long long int x, unsigned long long int y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int min(unsigned long long int x, long long int y);

#include "impl/simt_api/math_functions_impl.h"

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_FUNCTIONS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_FUNCTIONS_H__
#endif

#endif // INCLUDE_SIMT_API_MATH_FUNCTIONS_H
