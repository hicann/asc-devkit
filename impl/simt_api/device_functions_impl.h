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
 * \file device_functions_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEVICE_FUNCTIONS_IMPL__
#warning "impl/simt_api/device_functions_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "simt_api/device_functions.h" and use public functions or variables defined in interface header files."
#endif

#ifndef IMPL_SIMT_API_DEVICE_FUNCTIONS_IMPL_H
#define IMPL_SIMT_API_DEVICE_FUNCTIONS_IMPL_H

#include "simt_api/device_types.h"
#include "impl/simt_api/internal_functions_impl.h"

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)

#define ASCRT_Y_OFFSET 1 // 1 : offset of y
#define ASCRT_Z_OFFSET 2 // 2 : offset of z
#define ASCRT_W_OFFSET 3 // 3 : offset of w

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __float2float_rn(const float x)
{
    return __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __float2float_rz(const float x)
{
    return __cvt_float<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __float2float_rd(const float x)
{
    return __cvt_float<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __float2float_ru(const float x)
{
    return __cvt_float<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __float2float_rna(const float x)
{
    return __cvt_float<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __float2uint_rn(const float x)
{
    return __cvt_uint32_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __float2uint_rz(const float x)
{
    return __cvt_uint32_t<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __float2uint_rd(const float x)
{
    return __cvt_uint32_t<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __float2uint_ru(const float x)
{
    return __cvt_uint32_t<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __float2uint_rna(const float x)
{
    return __cvt_uint32_t<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __float2int_rn(const float x)
{
    return __cvt_int32_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __float2int_rz(const float x)
{
    return __cvt_int32_t<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __float2int_rd(const float x)
{
    return __cvt_int32_t<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __float2int_ru(const float x)
{
    return __cvt_int32_t<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __float2int_rna(const float x)
{
    return __cvt_int32_t<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __float2ull_rn(const float x)
{
    return __cvt_uint64_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __float2ull_rz(const float x)
{
    return __cvt_uint64_t<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __float2ull_rd(const float x)
{
    return __cvt_uint64_t<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __float2ull_ru(const float x)
{
    return __cvt_uint64_t<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __float2ull_rna(const float x)
{
    return __cvt_uint64_t<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __float2ll_rn(const float x)
{
    return __cvt_int64_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __float2ll_rz(const float x)
{
    return __cvt_int64_t<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __float2ll_rd(const float x)
{
    return __cvt_int64_t<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __float2ll_ru(const float x)
{
    return __cvt_int64_t<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __float2ll_rna(const float x)
{
    return __cvt_int64_t<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __uint2float_rn(const unsigned int x)
{
    return __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __uint2float_rz(const unsigned int x)
{
    return __cvt_float<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __uint2float_rd(const unsigned int x)
{
    return __cvt_float<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __uint2float_ru(const unsigned int x)
{
    return __cvt_float<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __uint2float_rna(const unsigned int x)
{
    return __cvt_float<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __int2float_rn(const int x)
{
    return __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __int2float_rz(const int x)
{
    return __cvt_float<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __int2float_rd(const int x)
{
    return __cvt_float<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __int2float_ru(const int x)
{
    return __cvt_float<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __int2float_rna(const int x)
{
    return __cvt_float<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __ull2float_rn(const unsigned long long int x)
{
    uint64_t y = x;
    return __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __ull2float_rz(const unsigned long long int x)
{
    uint64_t y = x;
    return __cvt_float<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __ull2float_rd(const unsigned long long int x)
{
    uint64_t y = x;
    return __cvt_float<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __ull2float_ru(const unsigned long long int x)
{
    uint64_t y = x;
    return __cvt_float<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __ull2float_rna(const unsigned long long int x)
{
    uint64_t y = x;
    return __cvt_float<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __ll2float_rn(const long long int x)
{
    int64_t y = x;
    return __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __ll2float_rz(const long long int x)
{
    int64_t y = x;
    return __cvt_float<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __ll2float_rd(const long long int x)
{
    int64_t y = x;
    return __cvt_float<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __ll2float_ru(const long long int x)
{
    int64_t y = x;
    return __cvt_float<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __ll2float_rna(const long long int x)
{
    int64_t y = x;
    return __cvt_float<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __int_as_float(const int x)
{
    union Data {
        float f;
        int i;
    };
    union Data data = {.i = x};
    return data.f;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __uint_as_float(const unsigned int x)
{
    union Data {
        float f;
        unsigned int i;
    };
    union Data data = {.i = x};
    return data.f;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __float_as_int(const float x)
{
    union Data {
        float f;
        int i;
    };
    union Data data = {.f = x};
    return data.i;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __float_as_uint(const float x)
{
    union Data {
        float f;
        unsigned int u;
    };
    union Data data = {.f = x};
    return data.u;
}

#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
__SIMT_DEVICE_FUNCTIONS_DECL__ inline long int asc_ldcg(__gm__ long int* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long int asc_ldcg(__gm__ unsigned long int* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int asc_ldcg(__gm__ long long int* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int64_t*)address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int asc_ldcg(__gm__ unsigned long long int* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ uint64_t*)address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long2 asc_ldcg(__gm__ long2* address)
{
    longlong2 t =
        __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ longlong2*)address);
    return (long2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulong2 asc_ldcg(__gm__ ulong2* address)
{
    longlong2 t =
        __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ longlong2*)address);
    return (ulong2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long4 asc_ldcg(__gm__ long4* address)
{
    longlong4 t;
    t.x = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int64_t*)address);
    t.y = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Y_OFFSET);
    t.z = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Z_OFFSET);
    t.w = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_W_OFFSET);
    return (long4&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulong4 asc_ldcg(__gm__ ulong4* address)
{
    longlong4 t;
    t.x = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int64_t*)address);
    t.y = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Y_OFFSET);
    t.z = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Z_OFFSET);
    t.w = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_W_OFFSET);
    return (ulong4&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline longlong2 asc_ldcg(__gm__ longlong2* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulonglong2 asc_ldcg(__gm__ ulonglong2* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline longlong4 asc_ldcg(__gm__ longlong4* address)
{
    longlong4 t;
    t.x = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int64_t*)address);
    t.y = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Y_OFFSET);
    t.z = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Z_OFFSET);
    t.w = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_W_OFFSET);
    return t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulonglong4 asc_ldcg(__gm__ ulonglong4* address)
{
    longlong4 t;
    t.x = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int64_t*)address);
    t.y = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Y_OFFSET);
    t.z = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Z_OFFSET);
    t.w = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_W_OFFSET);
    return (ulonglong4&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline signed char asc_ldcg(__gm__ signed char* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int8_t*)address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned char asc_ldcg(__gm__ unsigned char* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int8_t*)address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline char2 asc_ldcg(__gm__ char2* address)
{
    int16_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int16_t*)address);
    return (char2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uchar2 asc_ldcg(__gm__ uchar2* address)
{
    int16_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int16_t*)address);
    return (uchar2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline char4 asc_ldcg(__gm__ char4* address)
{
    int32_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int32_t*)address);
    return (char4&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uchar4 asc_ldcg(__gm__ uchar4* address)
{
    int32_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int32_t*)address);
    return (uchar4&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short asc_ldcg(__gm__ short* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned short asc_ldcg(__gm__ unsigned short* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short2 asc_ldcg(__gm__ short2* address)
{
    int32_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int32_t*)address);
    return (short2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ushort2 asc_ldcg(__gm__ ushort2* address)
{
    int32_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int32_t*)address);
    return (ushort2&)t;
}
__SIMT_DEVICE_FUNCTIONS_DECL__ inline short4 asc_ldcg(__gm__ short4* address)
{
    int64_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int64_t*)address);
    return (short4&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ushort4 asc_ldcg(__gm__ ushort4* address)
{
    int64_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int64_t*)address);
    return (ushort4&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int asc_ldcg(__gm__ int* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int asc_ldcg(__gm__ unsigned int* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int2 asc_ldcg(__gm__ int2* address)
{
    int64_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int64_t*)address);
    return (int2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint2 asc_ldcg(__gm__ uint2* address)
{
    int64_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int64_t*)address);
    return (uint2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int4 asc_ldcg(__gm__ int4* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint4 asc_ldcg(__gm__ uint4* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asc_ldcg(__gm__ float* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float2 asc_ldcg(__gm__ float2* address)
{
    int64_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int64_t*)address);
    return (float2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4 asc_ldcg(__gm__ float4* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long int asc_ldca(__gm__ long int* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long int asc_ldca(__gm__ unsigned long int* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int asc_ldca(__gm__ long long int* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int64_t*)address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int asc_ldca(__gm__ unsigned long long int* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ uint64_t*)address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long2 asc_ldca(__gm__ long2* address)
{
    longlong2 t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ longlong2*)address);
    return (long2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulong2 asc_ldca(__gm__ ulong2* address)
{
    longlong2 t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ longlong2*)address);
    return (ulong2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long4 asc_ldca(__gm__ long4* address)
{
    long4 t;
    t.x = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int64_t*)address);
    t.y = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Y_OFFSET);
    t.z = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Z_OFFSET);
    t.w = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_W_OFFSET);
    return t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulong4 asc_ldca(__gm__ ulong4* address)
{
    ulong4 t;
    t.x = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int64_t*)address);
    t.y = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Y_OFFSET);
    t.z = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Z_OFFSET);
    t.w = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_W_OFFSET);
    return t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline longlong2 asc_ldca(__gm__ longlong2* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulonglong2 asc_ldca(__gm__ ulonglong2* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline longlong4 asc_ldca(__gm__ longlong4* address)
{
    longlong4 t;
    t.x = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int64_t*)address);
    t.y = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Y_OFFSET);
    t.z = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Z_OFFSET);
    t.w = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_W_OFFSET);
    return t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulonglong4 asc_ldca(__gm__ ulonglong4* address)
{
    longlong4 t;
    t.x = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int64_t*)address);
    t.y = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Y_OFFSET);
    t.z = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Z_OFFSET);
    t.w = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_W_OFFSET);
    return (ulonglong4&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline signed char asc_ldca(__gm__ signed char* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned char asc_ldca(__gm__ unsigned char* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline char2 asc_ldca(__gm__ char2* address)
{
    int16_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int16_t*)address);
    return (char2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uchar2 asc_ldca(__gm__ uchar2* address)
{
    int16_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int16_t*)address);
    return (uchar2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline char4 asc_ldca(__gm__ char4* address)
{
    int32_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int32_t*)address);
    return (char4&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uchar4 asc_ldca(__gm__ uchar4* address)
{
    int32_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int32_t*)address);
    return (uchar4&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short asc_ldca(__gm__ short* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned short asc_ldca(__gm__ unsigned short* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short2 asc_ldca(__gm__ short2* address)
{
    int32_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int32_t*)address);
    return (short2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ushort2 asc_ldca(__gm__ ushort2* address)
{
    int32_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int32_t*)address);
    return (ushort2&)t;
}
__SIMT_DEVICE_FUNCTIONS_DECL__ inline short4 asc_ldca(__gm__ short4* address)
{
    int64_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int64_t*)address);
    return (short4&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ushort4 asc_ldca(__gm__ ushort4* address)
{
    int64_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int64_t*)address);
    return (ushort4&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int asc_ldca(__gm__ int* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int asc_ldca(__gm__ unsigned int* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int2 asc_ldca(__gm__ int2* address)
{
    int64_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int64_t*)address);
    return (int2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint2 asc_ldca(__gm__ uint2* address)
{
    int64_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int64_t*)address);
    return (uint2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int4 asc_ldca(__gm__ int4* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint4 asc_ldca(__gm__ uint4* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asc_ldca(__gm__ float* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float2 asc_ldca(__gm__ float2* address)
{
    int64_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int64_t*)address);
    return (float2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4 asc_ldca(__gm__ float4* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ long int* address, long int val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ unsigned long int* address, unsigned long int val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ long long int* address, long long int val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int64_t*)address, (int64_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ unsigned long long int* address, unsigned long long int val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ uint64_t*)address, (uint64_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ long2* address, long2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ longlong2*)address, (longlong2&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ ulong2* address, ulong2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ ulonglong2*)address, (ulonglong2&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ long4* address, long4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ int64_t*)address, (int64_t)val.x);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Y_OFFSET, (int64_t)val.y);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Z_OFFSET, (int64_t)val.z);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_W_OFFSET, (int64_t)val.w);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ ulong4* address, ulong4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ int64_t*)address, (int64_t)val.x);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Y_OFFSET, (int64_t)val.y);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Z_OFFSET, (int64_t)val.z);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_W_OFFSET, (int64_t)val.w);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ longlong2* address, longlong2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ ulonglong2* address, ulonglong2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ longlong4* address, longlong4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ int64_t*)address, (int64_t)val.x);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Y_OFFSET, (int64_t)val.y);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Z_OFFSET, (int64_t)val.z);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_W_OFFSET, (int64_t)val.w);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ ulonglong4* address, ulonglong4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ int64_t*)address, (int64_t)val.x);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Y_OFFSET, (int64_t)val.y);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Z_OFFSET, (int64_t)val.z);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_W_OFFSET, (int64_t)val.w);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ signed char* address, signed char val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ unsigned char* address, unsigned char val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ char* address, char val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (__gm__ signed char*)address, (signed char)(val));
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ char2* address, char2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int16_t*)address, (int16_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ uchar2* address, uchar2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int16_t*)address, (int16_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ char4* address, char4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int32_t*)address, (int32_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ uchar4* address, uchar4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int32_t*)address, (int32_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ short* address, short val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ unsigned short* address, unsigned short val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ short2* address, short2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int32_t*)address, (int32_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ ushort2* address, ushort2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int32_t*)address, (int32_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ short4* address, short4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int64_t*)address, (int64_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ ushort4* address, ushort4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int64_t*)address, (int64_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ int* address, int val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ unsigned int* address, unsigned int val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ int2* address, int2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int64_t*)address, (int64_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ uint2* address, uint2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int64_t*)address, (int64_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ int4* address, int4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ uint4* address, uint4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ float* address, float val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ float2* address, float2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((__gm__ int64_t*)address, (int64_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ float4* address, float4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ long int* address, long int val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ unsigned long int* address, unsigned long int val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ long long int* address, long long int val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int64_t*)address, (int64_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ unsigned long long int* address, unsigned long long int val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ uint64_t*)address, (uint64_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ long2* address, long2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ longlong2*)address, (longlong2&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ ulong2* address, ulong2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (__gm__ ulonglong2*)address, (ulonglong2&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ long4* address, long4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int64_t*)address, (int64_t)val.x);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Y_OFFSET, (int64_t)val.y);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Z_OFFSET, (int64_t)val.z);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_W_OFFSET, (int64_t)val.w);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ ulong4* address, ulong4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ uint64_t*)address, (uint64_t)val.x);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (__gm__ uint64_t*)address + ASCRT_Y_OFFSET, (uint64_t)val.y);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (__gm__ uint64_t*)address + ASCRT_Z_OFFSET, (uint64_t)val.z);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (__gm__ uint64_t*)address + ASCRT_W_OFFSET, (uint64_t)val.w);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ longlong2* address, longlong2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ ulonglong2* address, ulonglong2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ longlong4* address, longlong4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int64_t*)address, (int64_t)val.x);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Y_OFFSET, (int64_t)val.y);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Z_OFFSET, (int64_t)val.z);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_W_OFFSET, (int64_t)val.w);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ ulonglong4* address, ulonglong4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int64_t*)address, (int64_t)val.x);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Y_OFFSET, (int64_t)val.y);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_Z_OFFSET, (int64_t)val.z);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (__gm__ int64_t*)address + ASCRT_W_OFFSET, (int64_t)val.w);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ signed char* address, signed char val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ unsigned char* address, unsigned char val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ char* address, char val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (__gm__ signed char*)address, (signed char)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ char2* address, char2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int16_t*)address, (int16_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ uchar2* address, uchar2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int16_t*)address, (int16_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ char4* address, char4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int32_t*)address, (int32_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ uchar4* address, uchar4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int32_t*)address, (int32_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ short* address, short val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ unsigned short* address, unsigned short val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ short2* address, short2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int32_t*)address, (int32_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ ushort2* address, ushort2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int32_t*)address, (int32_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ short4* address, short4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int64_t*)address, (int64_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ ushort4* address, ushort4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int64_t*)address, (int64_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ int* address, int val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ unsigned int* address, unsigned int val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ int2* address, int2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int64_t*)address, (int64_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ uint2* address, uint2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int64_t*)address, (int64_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ int4* address, int4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ uint4* address, uint4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ float* address, float val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ float2* address, float2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((__gm__ int64_t*)address, (int64_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ float4* address, float4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_dcci_entire(__gm__ void* dst) { dcci(dst, 1); }
#else
__SIMT_DEVICE_FUNCTIONS_DECL__ inline long int asc_ldcg(long int* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long int asc_ldcg(unsigned long int* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int asc_ldcg(long long int* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int asc_ldcg(unsigned long long int* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((uint64_t*)address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long2 asc_ldcg(long2* address)
{
    longlong2 t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((longlong2*)address);
    return (long2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulong2 asc_ldcg(ulong2* address)
{
    longlong2 t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((longlong2*)address);
    return (ulong2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long4 asc_ldcg(long4* address)
{
    longlong4 t;
    t.x = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address);
    t.y =
        __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address + ASCRT_Y_OFFSET);
    t.z =
        __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address + ASCRT_Z_OFFSET);
    t.w =
        __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address + ASCRT_W_OFFSET);
    return (long4&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulong4 asc_ldcg(ulong4* address)
{
    longlong4 t;
    t.x = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address);
    t.y =
        __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address + ASCRT_Y_OFFSET);
    t.z =
        __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address + ASCRT_Z_OFFSET);
    t.w =
        __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address + ASCRT_W_OFFSET);
    return (ulong4&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline longlong2 asc_ldcg(longlong2* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulonglong2 asc_ldcg(ulonglong2* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline longlong4 asc_ldcg(longlong4* address)
{
    longlong4 t;
    t.x = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address);
    t.y =
        __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address + ASCRT_Y_OFFSET);
    t.z =
        __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address + ASCRT_Z_OFFSET);
    t.w =
        __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address + ASCRT_W_OFFSET);
    return t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulonglong4 asc_ldcg(ulonglong4* address)
{
    longlong4 t;
    t.x = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address);
    t.y =
        __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address + ASCRT_Y_OFFSET);
    t.z =
        __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address + ASCRT_Z_OFFSET);
    t.w =
        __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address + ASCRT_W_OFFSET);
    return (ulonglong4&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline signed char asc_ldcg(signed char* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int8_t*)address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned char asc_ldcg(unsigned char* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int8_t*)address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline char2 asc_ldcg(char2* address)
{
    int16_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int16_t*)address);
    return (char2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uchar2 asc_ldcg(uchar2* address)
{
    int16_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int16_t*)address);
    return (uchar2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline char4 asc_ldcg(char4* address)
{
    int32_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int32_t*)address);
    return (char4&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uchar4 asc_ldcg(uchar4* address)
{
    int32_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int32_t*)address);
    return (uchar4&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short asc_ldcg(short* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned short asc_ldcg(unsigned short* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short2 asc_ldcg(short2* address)
{
    int32_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int32_t*)address);
    return (short2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ushort2 asc_ldcg(ushort2* address)
{
    int32_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int32_t*)address);
    return (ushort2&)t;
}
__SIMT_DEVICE_FUNCTIONS_DECL__ inline short4 asc_ldcg(short4* address)
{
    int64_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address);
    return (short4&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ushort4 asc_ldcg(ushort4* address)
{
    int64_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address);
    return (ushort4&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int asc_ldcg(int* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int asc_ldcg(unsigned int* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int2 asc_ldcg(int2* address)
{
    int64_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address);
    return (int2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint2 asc_ldcg(uint2* address)
{
    int64_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address);
    return (uint2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int4 asc_ldcg(int4* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint4 asc_ldcg(uint4* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asc_ldcg(float* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float2 asc_ldcg(float2* address)
{
    int64_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address);
    return (float2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4 asc_ldcg(float4* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long int asc_ldca(long int* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long int asc_ldca(unsigned long int* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int asc_ldca(long long int* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int asc_ldca(unsigned long long int* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((uint64_t*)address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long2 asc_ldca(long2* address)
{
    longlong2 t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((longlong2*)address);
    return (long2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulong2 asc_ldca(ulong2* address)
{
    longlong2 t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((longlong2*)address);
    return (ulong2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long4 asc_ldca(long4* address)
{
    long4 t;
    t.x = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address);
    t.y = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address + ASCRT_Y_OFFSET);
    t.z = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address + ASCRT_Z_OFFSET);
    t.w = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address + ASCRT_W_OFFSET);
    return t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulong4 asc_ldca(ulong4* address)
{
    ulong4 t;
    t.x = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address);
    t.y = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address + ASCRT_Y_OFFSET);
    t.z = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address + ASCRT_Z_OFFSET);
    t.w = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address + ASCRT_W_OFFSET);
    return t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline longlong2 asc_ldca(longlong2* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulonglong2 asc_ldca(ulonglong2* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline longlong4 asc_ldca(longlong4* address)
{
    longlong4 t;
    t.x = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address);
    t.y = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address + ASCRT_Y_OFFSET);
    t.z = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address + ASCRT_Z_OFFSET);
    t.w = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address + ASCRT_W_OFFSET);
    return t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulonglong4 asc_ldca(ulonglong4* address)
{
    longlong4 t;
    t.x = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address);
    t.y = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address + ASCRT_Y_OFFSET);
    t.z = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address + ASCRT_Z_OFFSET);
    t.w = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address + ASCRT_W_OFFSET);
    return (ulonglong4&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline signed char asc_ldca(signed char* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned char asc_ldca(unsigned char* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline char2 asc_ldca(char2* address)
{
    int16_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int16_t*)address);
    return (char2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uchar2 asc_ldca(uchar2* address)
{
    int16_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int16_t*)address);
    return (uchar2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline char4 asc_ldca(char4* address)
{
    int32_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int32_t*)address);
    return (char4&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uchar4 asc_ldca(uchar4* address)
{
    int32_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int32_t*)address);
    return (uchar4&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short asc_ldca(short* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned short asc_ldca(unsigned short* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short2 asc_ldca(short2* address)
{
    int32_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int32_t*)address);
    return (short2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ushort2 asc_ldca(ushort2* address)
{
    int32_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int32_t*)address);
    return (ushort2&)t;
}
__SIMT_DEVICE_FUNCTIONS_DECL__ inline short4 asc_ldca(short4* address)
{
    int64_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address);
    return (short4&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ushort4 asc_ldca(ushort4* address)
{
    int64_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address);
    return (ushort4&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int asc_ldca(int* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int asc_ldca(unsigned int* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int2 asc_ldca(int2* address)
{
    int64_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address);
    return (int2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint2 asc_ldca(uint2* address)
{
    int64_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address);
    return (uint2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int4 asc_ldca(int4* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint4 asc_ldca(uint4* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asc_ldca(float* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float2 asc_ldca(float2* address)
{
    int64_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address);
    return (float2&)t;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4 asc_ldca(float4* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(long int* address, long int val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(unsigned long int* address, unsigned long int val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(long long int* address, long long int val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address, (int64_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(unsigned long long int* address, unsigned long long int val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((uint64_t*)address, (uint64_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(long2* address, long2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((longlong2*)address, (longlong2&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(ulong2* address, ulong2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((ulonglong2*)address, (ulonglong2&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(long4* address, long4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address, (int64_t)val.x);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (int64_t*)address + ASCRT_Y_OFFSET, (int64_t)val.y);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (int64_t*)address + ASCRT_Z_OFFSET, (int64_t)val.z);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (int64_t*)address + ASCRT_W_OFFSET, (int64_t)val.w);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(ulong4* address, ulong4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address, (int64_t)val.x);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (int64_t*)address + ASCRT_Y_OFFSET, (int64_t)val.y);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (int64_t*)address + ASCRT_Z_OFFSET, (int64_t)val.z);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (int64_t*)address + ASCRT_W_OFFSET, (int64_t)val.w);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(longlong2* address, longlong2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(ulonglong2* address, ulonglong2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(longlong4* address, longlong4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address, (int64_t)val.x);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (int64_t*)address + ASCRT_Y_OFFSET, (int64_t)val.y);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (int64_t*)address + ASCRT_Z_OFFSET, (int64_t)val.z);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (int64_t*)address + ASCRT_W_OFFSET, (int64_t)val.w);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(ulonglong4* address, ulonglong4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address, (int64_t)val.x);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (int64_t*)address + ASCRT_Y_OFFSET, (int64_t)val.y);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (int64_t*)address + ASCRT_Z_OFFSET, (int64_t)val.z);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        (int64_t*)address + ASCRT_W_OFFSET, (int64_t)val.w);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(signed char* address, signed char val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(unsigned char* address, unsigned char val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(char* address, char val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((signed char*)address, (signed char)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(char2* address, char2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int16_t*)address, (int16_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(uchar2* address, uchar2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int16_t*)address, (int16_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(char4* address, char4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int32_t*)address, (int32_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(uchar4* address, uchar4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int32_t*)address, (int32_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(short* address, short val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(unsigned short* address, unsigned short val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(short2* address, short2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int32_t*)address, (int32_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(ushort2* address, ushort2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int32_t*)address, (int32_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(short4* address, short4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address, (int64_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(ushort4* address, ushort4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address, (int64_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(int* address, int val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(unsigned int* address, unsigned int val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(int2* address, int2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address, (int64_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(uint2* address, uint2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address, (int64_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(int4* address, int4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(uint4* address, uint4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(float* address, float val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(float2* address, float2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>((int64_t*)address, (int64_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(float4* address, float4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(long int* address, long int val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(unsigned long int* address, unsigned long int val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(long long int* address, long long int val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address, (int64_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(unsigned long long int* address, unsigned long long int val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((uint64_t*)address, (uint64_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(long2* address, long2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((longlong2*)address, (longlong2&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(ulong2* address, ulong2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((ulonglong2*)address, (ulonglong2&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(long4* address, long4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address, (int64_t)val.x);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (int64_t*)address + ASCRT_Y_OFFSET, (int64_t)val.y);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (int64_t*)address + ASCRT_Z_OFFSET, (int64_t)val.z);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (int64_t*)address + ASCRT_W_OFFSET, (int64_t)val.w);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(ulong4* address, ulong4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((uint64_t*)address, (uint64_t)val.x);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (uint64_t*)address + ASCRT_Y_OFFSET, (uint64_t)val.y);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (uint64_t*)address + ASCRT_Z_OFFSET, (uint64_t)val.z);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (uint64_t*)address + ASCRT_W_OFFSET, (uint64_t)val.w);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(longlong2* address, longlong2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(ulonglong2* address, ulonglong2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(longlong4* address, longlong4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address, (int64_t)val.x);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (int64_t*)address + ASCRT_Y_OFFSET, (int64_t)val.y);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (int64_t*)address + ASCRT_Z_OFFSET, (int64_t)val.z);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (int64_t*)address + ASCRT_W_OFFSET, (int64_t)val.w);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(ulonglong4* address, ulonglong4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address, (int64_t)val.x);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (int64_t*)address + ASCRT_Y_OFFSET, (int64_t)val.y);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (int64_t*)address + ASCRT_Z_OFFSET, (int64_t)val.z);
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        (int64_t*)address + ASCRT_W_OFFSET, (int64_t)val.w);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(signed char* address, signed char val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(unsigned char* address, unsigned char val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(char* address, char val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((signed char*)address, (signed char)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(char2* address, char2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int16_t*)address, (int16_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(uchar2* address, uchar2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int16_t*)address, (int16_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(char4* address, char4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int32_t*)address, (int32_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(uchar4* address, uchar4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int32_t*)address, (int32_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(short* address, short val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(unsigned short* address, unsigned short val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(short2* address, short2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int32_t*)address, (int32_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(ushort2* address, ushort2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int32_t*)address, (int32_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(short4* address, short4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address, (int64_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(ushort4* address, ushort4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address, (int64_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(int* address, int val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(unsigned int* address, unsigned int val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(int2* address, int2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address, (int64_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(uint2* address, uint2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address, (int64_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(int4* address, int4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(uint4* address, uint4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(float* address, float val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(float2* address, float2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>((int64_t*)address, (int64_t&)val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(float4* address, float4 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_dcci_entire(void* dst) { dcci(dst, 1); }
#endif

namespace __asc_simt_vf {
#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_dcci_single(__gm__ void* dst) { dcci(dst, 0); }
#else
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_dcci_single(void* dst) { dcci(dst, 0); }
#endif
} // namespace __asc_simt_vf

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_nop() { __nop(); }

#endif
#endif // IMPL_SIMT_API_DEVICE_FUNCTIONS_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEVICE_FUNCTIONS_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEVICE_FUNCTIONS_IMPL__
#endif
