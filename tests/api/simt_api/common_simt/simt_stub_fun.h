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
 * \file simt_stub_fun.h
 * \brief
 */
#ifndef __CCE_AICORE_INTRINSICS_STUBS__
#define __CCE_AICORE_INTRINSICS_STUBS__
#include <cstdint>
#include "kernel_bf16.h"
#include "kernel_fp16.h"
#include "kernel_vectorized.h"
#include "kernel_fp8_e5m2.h"
#include "kernel_fp8_e4m3.h"
#include "kernel_fp8_e8m0.h"
#include "kernel_fp4_e2m1.h"
#include "kernel_fp4_e1m2.h"
#include "kernel_hif8.h"
#include "stub_def.h"
typedef enum {
    CAST_RINT = 0,
    CAST_ROUND = 1,
    CAST_FLOOR = 2,
    CAST_CEIL = 3,
    CAST_TRUNC = 4,
    CAST_ODD = 5,
    CAST_HYBRID = 6,
} ROUND;

typedef enum {
    RS_DISABLE_VALUE = 0,
    RS_ENABLE_VALUE = 1,
} RoundingSaturation;

typedef enum {
    PIPE_S = 0,
    PIPE_V,
    PIPE_M,
    PIPE_MTE1,
    PIPE_MTE2,
    PIPE_MTE3,
    PIPE_ALL,
    PIPE_MTE4 = 7,
    PIPE_MTE5 = 8,
    PIPE_V2 = 9,
    PIPE_FIX = 10,
} pipe_t;

void dcci(__gm__ void* dst, uint64_t entire);

uint32_t atomicSub(__gm__ uint32_t* address, uint32_t value);

int32_t atomicSub(__gm__ int32_t* address, int32_t value);

uint64_t atomicSub(__gm__ uint64_t* address, uint64_t value);

int64_t atomicSub(__gm__ int64_t* address, int64_t value);

float atomicSub(__gm__ float* address, float value);

int32_t atomicExch(__gm__ int32_t* address, int32_t value);

int64_t atomicExch(__gm__ int64_t* address, int64_t value);

float atomicExch(__gm__ float* address, float value);

uint32_t atomicInc(__gm__ uint32_t* address, uint32_t value);

uint64_t atomicInc(__gm__ uint64_t* address, uint64_t value);

uint32_t atomicDec(__gm__ uint32_t* address, uint32_t value);

uint64_t atomicDec(__gm__ uint64_t* address, uint64_t value);

int32_t atomicCAS(__gm__ int32_t* address, int32_t value1, int32_t value2);

int64_t atomicCAS(__gm__ int64_t* address, int64_t value1, int64_t value2);

float atomicCAS(__gm__ float* address, float value1, float value2);

uint32_t atomicAnd(__gm__ uint32_t* address, uint32_t value);

int32_t atomicAnd(__gm__ int32_t* address, int32_t value);

uint64_t atomicAnd(__gm__ uint64_t* address, uint64_t value);

int64_t atomicAnd(__gm__ int64_t* address, int64_t value);

uint32_t atomicOr(__gm__ uint32_t* address, uint32_t value);

int32_t atomicOr(__gm__ int32_t* address, int32_t value);

uint64_t atomicOr(__gm__ uint64_t* address, uint64_t value);

int64_t atomicOr(__gm__ int64_t* address, int64_t value);

uint32_t atomicXOr(__gm__ uint32_t* address, uint32_t value);

int32_t atomicXOr(__gm__ int32_t* address, int32_t value);

uint64_t atomicXOr(__gm__ uint64_t* address, uint64_t value);

int64_t atomicXOr(__gm__ int64_t* address, int64_t value);

void __sync_workitems();

void __threadfence();

void __threadfence_block();

int32_t __all(int32_t predicate);

int32_t __any(int32_t predicate);

uint32_t __ballot(int32_t predicate);

uint32_t __activemask();

int32_t __reduce_add(int32_t val);

uint32_t __reduce_add(uint32_t val);

float __reduce_add(float val);

int32_t __reduce_max(int32_t val);

uint32_t __reduce_max(uint32_t val);

float __reduce_max(float val);

int32_t __reduce_min(int32_t val);

uint32_t __reduce_min(uint32_t val);

float __reduce_min(float val);

float __expf(float x);

float __logf(float x);

float __sqrtf(float x);

bool __isfinite(float x);

bool __isnan(float x);

bool __isinf(float x);

float __powf(float x, float y);

int64_t max(const long long int x, const long long int y);

uint64_t max(const unsigned long long int x, const unsigned long long int y);

uint32_t max(const unsigned int x, const unsigned int y);

int64_t min(const long long int x, const long long int y);

uint64_t min(const unsigned long long int x, const unsigned long long int y);

uint32_t min(const unsigned int x, const unsigned int y);

int signbitf(float x);

float __fmaxf(float x, float y);

float __fminf(float x, float y);

float __roundf(float x);

float __rintf(float x);

float __floorf(float x);

float __ceilf(float x);

float __fma(float x, float y, float z);

bool __isnan(half x);

bool __isinf(half x);

half __fma(half x, half y, half z);

half __expf(half x);

half __logf(half x);

half __sqrtf(half x);

half __rintf(half x);

half __floorf(half x);

half __ceilf(half x);

half2 __expf(half2 x);

half2 __logf(half2 x);

half2 __sqrtf(half2 x);

half atomicAdd(__gm__ half* address, half value);

half atomicSub(__gm__ half* address, half value);

half atomicMax(__gm__ half* address, half value);

half atomicMin(__gm__ half* address, half value);

half2 atomicAdd(__gm__ half2* address, half2 value);

half2 atomicSub(__gm__ half2* address, half2 value);

half2 atomicExch(__gm__ half2* address, half2 value);

half2 atomicMax(__gm__ half2* address, half2 value);

half2 atomicMin(__gm__ half2* address, half2 value);

half2 atomicCAS(__gm__ half2* address, half2 value1, half2 value2);

half __reduce_add(half val);

half __reduce_max(half val);

half __reduce_min(half val);

bool __isnan(bfloat16_t x);

bool __isinf(bfloat16_t x);

bfloat16_t __fma(bfloat16_t x, bfloat16_t y, bfloat16_t z);

bfloat16_t __max(bfloat16_t x, bfloat16_t y);

bfloat16_t __min(bfloat16_t x, bfloat16_t y);

bfloat16_t __floorf(bfloat16_t x);

bfloat16_t __rintf(bfloat16_t x);

bfloat16_t __ceilf(bfloat16_t x);

uint32_t atomicAdd(__gm__ uint32_t* address, uint32_t value);

int32_t atomicAdd(__gm__ int32_t* address, int32_t value);

uint64_t atomicAdd(__gm__ uint64_t* address, uint64_t value);

int64_t atomicAdd(__gm__ int64_t* address, int64_t value);

float atomicAdd(__gm__ float* address, float value);

uint32_t atomicMax(__gm__ uint32_t* address, uint32_t value);

int32_t atomicMax(__gm__ int32_t* address, int32_t value);

uint64_t atomicMax(__gm__ uint64_t* address, uint64_t value);

int64_t atomicMax(__gm__ int64_t* address, int64_t value);

float atomicMax(__gm__ float* address, float value);

uint32_t atomicMin(__gm__ uint32_t* address, uint32_t value);

int32_t atomicMin(__gm__ int32_t* address, int32_t value);

uint64_t atomicMin(__gm__ uint64_t* address, uint64_t value);

int64_t atomicMin(__gm__ int64_t* address, int64_t value);

float atomicMin(__gm__ float* address, float value);

uint64_t atomicCAS(__gm__ uint64_t* address, uint64_t value1, uint64_t value2);

uint32_t atomicCAS(__gm__ uint32_t* address, uint32_t value1, uint32_t value2);

uint64_t atomicExch(__gm__ uint64_t* address, uint64_t value);

uint32_t atomicExch(__gm__ uint32_t* address, uint32_t value);

bfloat16_t atomicAdd(__gm__ bfloat16_t* address, bfloat16_t value);

bfloat16_t atomicMax(__gm__ bfloat16_t* address, bfloat16_t value);

bfloat16_t atomicMin(__gm__ bfloat16_t* address, bfloat16_t value);

bfloat16x2_t atomicAdd(__gm__ bfloat16x2_t* address, bfloat16x2_t value);

bfloat16x2_t atomicSub(__gm__ bfloat16x2_t* address, bfloat16x2_t value);

bfloat16x2_t atomicExch(__gm__ bfloat16x2_t* address, bfloat16x2_t value);

bfloat16x2_t atomicMax(__gm__ bfloat16x2_t* address, bfloat16x2_t value);

bfloat16x2_t atomicMin(__gm__ bfloat16x2_t* address, bfloat16x2_t value);

bfloat16x2_t atomicCAS(__gm__ bfloat16x2_t* address, bfloat16x2_t value1, bfloat16x2_t value2);

float __fabsf(float x);

#endif
