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

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `lroundf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline long int lroundf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `llroundf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int llroundf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `lrintf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline long int lrintf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `llrintf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int llrintf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `truncf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float truncf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float roundf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float rintf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float floorf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float ceilf(float x);

__ASC_SIMT_SIMULATION_SQRT_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `sqrtf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float sqrtf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `rsqrtf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float rsqrtf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `log2f`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float log2f(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `logf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float logf(float x);

__ASC_SIMT_SIMULATION_EXP_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `expf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float expf(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float fmaf(float x, float y, float z);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __fmaf_ieee_rn(float x, float y, float z);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __funnelshift_l(
    unsigned int lo, unsigned int hi, unsigned int shift);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __funnelshift_lc(
    unsigned int lo, unsigned int hi, unsigned int shift);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __funnelshift_r(
    unsigned int lo, unsigned int hi, unsigned int shift);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __funnelshift_rc(
    unsigned int lo, unsigned int hi, unsigned int shift);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __dp2a_lo(int src_a, int src_b, int c);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __dp2a_lo(unsigned int src_a, unsigned int src_b, unsigned int c);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __dp2a_lo(short2 src_a, char4 src_b, int c);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __dp2a_lo(ushort2 src_a, uchar4 src_b, unsigned int c);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __dp2a_hi(int src_a, int src_b, int c);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __dp2a_hi(unsigned int src_a, unsigned int src_b, unsigned int c);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __dp2a_hi(short2 src_a, char4 src_b, int c);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __dp2a_hi(ushort2 src_a, uchar4 src_b, unsigned int c);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __dp4a(int src_a, int src_b, int c);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __dp4a(unsigned int src_a, unsigned int src_b, unsigned int c);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __dp4a(char4 src_a, char4 src_b, int c);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __dp4a(uchar4 src_a, uchar4 src_b, unsigned int c);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float fabsf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `normcdfinvf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float normcdfinvf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `modff`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float modff(float x, float* n);
#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `modff`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float modff(float x, __ubuf__ float* n);
__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `modff`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float modff(float x, __gm__ float* n);
#endif

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bool isfinite(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bool isnan(float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bool isinf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `fdimf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float fdimf(float x, float y);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `remquof`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float remquof(float x, float y, int* quo);
#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `remquof`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float remquof(float x, float y, __ubuf__ int* quo);
__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `remquof`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float remquof(float x, float y, __gm__ int* quo);
#endif

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `fmodf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float fmodf(float x, float y);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `remainderf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float remainderf(float x, float y);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `copysignf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float copysignf(float x, float y);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `nearbyintf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float nearbyintf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `nextafterf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float nextafterf(float x, float y);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `scalbnf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float scalbnf(float x, int32_t n);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `scalblnf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float scalblnf(float x, int64_t n);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `fmaxf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float fmaxf(float x, float y);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `fminf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float fminf(float x, float y);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `nanf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float nanf(const char* tagp);
#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `nanf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float nanf(const __gm__ char* tagp);
__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `nanf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float nanf(const __ubuf__ char* tagp);
#endif

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `tanf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float tanf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `tanhf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float tanhf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `tanpif`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float tanpif(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `atanf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float atanf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `atan2f`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float atan2f(float y, float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `atanhf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float atanhf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `cosf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float cosf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `coshf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float coshf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `cospif`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float cospif(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `asinf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asinf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `acosf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float acosf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `acoshf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float acoshf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `sinf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float sinf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `sinhf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float sinhf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `sinpif`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float sinpif(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `asinhf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asinhf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `sincosf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincosf(float x, float* s, float* c);
#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `sincosf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincosf(float x, float* s, __ubuf__ float* c);
__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `sincosf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincosf(float x, float* s, __gm__ float* c);
__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `sincosf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincosf(float x, __ubuf__ float* s, float* c);
__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `sincosf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincosf(float x, __ubuf__ float* s, __ubuf__ float* c);
__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `sincosf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincosf(float x, __ubuf__ float* s, __gm__ float* c);
__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `sincosf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincosf(float x, __gm__ float* s, float* c);
__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `sincosf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincosf(float x, __gm__ float* s, __ubuf__ float* c);
__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `sincosf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincosf(float x, __gm__ float* s, __gm__ float* c);
#endif

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `sincospif`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincospif(float x, float* s, float* c);
#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `sincospif`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincospif(float x, float* s, __ubuf__ float* c);
__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `sincospif`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincospif(float x, float* s, __gm__ float* c);
__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `sincospif`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincospif(float x, __ubuf__ float* s, float* c);
__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `sincospif`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincospif(float x, __ubuf__ float* s, __ubuf__ float* c);
__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `sincospif`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincospif(float x, __ubuf__ float* s, __gm__ float* c);
__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `sincospif`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincospif(float x, __gm__ float* s, float* c);
__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `sincospif`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincospif(float x, __gm__ float* s, __ubuf__ float* c);
__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `sincospif`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void sincospif(float x, __gm__ float* s, __gm__ float* c);
#endif

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `exp2f`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float exp2f(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `exp10f`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float exp10f(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `expm1f`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float expm1f(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `frexpf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float frexpf(float x, int* exp);
#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `frexpf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float frexpf(float x, __ubuf__ int* exp);
__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `frexpf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float frexpf(float x, __gm__ int* exp);
#endif

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `ldexpf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float ldexpf(float x, int exp);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `hypotf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float hypotf(float x, float y);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `rhypotf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float rhypotf(float x, float y);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `norm3df`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float norm3df(float a, float b, float c);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `rnorm3df`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float rnorm3df(float a, float b, float c);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `norm4df`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float norm4df(float a, float b, float c, float d);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `rnorm4df`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float rnorm4df(float a, float b, float c, float d);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `normf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float normf(int n, float* a);
#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `normf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float normf(int n, __ubuf__ float* a);
__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `normf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float normf(int n, __gm__ float* a);
#endif

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `rnormf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float rnormf(int n, float* a);
#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `rnormf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float rnormf(int n, __ubuf__ float* a);
__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `rnormf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float rnormf(int n, __gm__ float* a);
#endif

__ASC_SIMT_SIMULATION_POW_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `powf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float powf(float x, float y);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `log10f`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float log10f(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `log1pf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float log1pf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `logbf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float logbf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `ilogbf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int ilogbf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `cbrtf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float cbrtf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `rcbrtf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float rcbrtf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `erff`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float erff(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `erfcf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float erfcf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `erfinvf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float erfinvf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `erfcinvf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float erfcinvf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `erfcxf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float erfcxf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `tgammaf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float tgammaf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `lgammaf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float lgammaf(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `cyl_bessel_i0f`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float cyl_bessel_i0f(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `cyl_bessel_i1f`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float cyl_bessel_i1f(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `normcdff`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float normcdff(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `j0f`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float j0f(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `j1f`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float j1f(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `jnf`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float jnf(int n, float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `y0f`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float y0f(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `y1f`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float y1f(float x);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `ynf`. Falling back to software emulation.")
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
__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `asc_uintdiv`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint32_t asc_uintdiv(uint32_t dividend, uint32_t magic, uint32_t shift);

__ASC_SIMT_SIMULATION_CHECK__(
    3510, 5102, 9201, 9202, "Hardware lacks native support for `asc_uintdiv`. Falling back to software emulation.")
__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint64_t asc_uintdiv(uint64_t dividend, uint64_t magic, uint64_t shift);

__aicore__ inline void asc_get_uintdiv_magic_and_shift(uint32_t* magic, uint32_t* shift, uint32_t divisor);

__aicore__ inline void asc_get_uintdiv_magic_and_shift(uint64_t* magic, uint64_t* shift, uint64_t divisor);
#endif

#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 9201) || (__NPU_ARCH__ == 9202) || (__NPU_ARCH__ == 5102)
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

#include "simt_api/device_functions.h"
#include "impl/simt_api/math_functions_impl.h"

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_FUNCTIONS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_FUNCTIONS_H__
#endif

#endif // INCLUDE_SIMT_API_MATH_FUNCTIONS_H
