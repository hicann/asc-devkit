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
 * \file vector_functions_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_VECTOR_FUNCTIONS_IMPL__
#warning "impl/simt_api/vector_functions_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "simt_api/vector_functions.h" and use public functions or variables defined in interface header files."
#endif

#ifndef IMPL_SIMT_API_VECTOR_FUNCTIONS_IMPL_H
#define IMPL_SIMT_API_VECTOR_FUNCTIONS_IMPL_H

#include "simt_api/device_types.h"

#if (__NPU_ARCH__ == 3510)

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short2 make_short2(short x, short y)
{
    short2 tmp;
    tmp.x = x;
    tmp.y = y;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short3 make_short3(short x, short y, short z)
{
    short3 tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.z = z;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short4 make_short4(short x, short y, short z, short w)
{
    short4 tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.z = z;
    tmp.w = w;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ushort2 make_ushort2(unsigned short x, unsigned short y)
{
    ushort2 tmp;
    tmp.x = x;
    tmp.y = y;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ushort3 make_ushort3(unsigned short x, unsigned short y, unsigned short z)
{
    ushort3 tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.z = z;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ushort4 make_ushort4(
    unsigned short x, unsigned short y, unsigned short z, unsigned short w)
{
    ushort4 tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.z = z;
    tmp.w = w;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline char2 make_char2(signed char x, signed char y)
{
    char2 tmp;
    tmp.x = x;
    tmp.y = y;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline char3 make_char3(signed char x, signed char y, signed char z)
{
    char3 tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.z = z;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline char4 make_char4(signed char x, signed char y, signed char z, signed char w)
{
    char4 tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.z = z;
    tmp.w = w;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uchar2 make_uchar2(unsigned char x, unsigned char y)
{
    uchar2 tmp;
    tmp.x = x;
    tmp.y = y;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uchar3 make_uchar3(unsigned char x, unsigned char y, unsigned char z)
{
    uchar3 tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.z = z;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uchar4 make_uchar4(
    unsigned char x, unsigned char y, unsigned char z, unsigned char w)
{
    uchar4 tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.z = z;
    tmp.w = w;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float2 make_float2(float x, float y)
{
    float2 tmp;
    tmp.x = x;
    tmp.y = y;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float3 make_float3(float x, float y, float z)
{
    float3 tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.z = z;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4 make_float4(float x, float y, float z, float w)
{
    float4 tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.z = z;
    tmp.w = w;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long2 make_long2(long int x, long int y)
{
    long2 tmp;
    tmp.x = x;
    tmp.y = y;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long3 make_long3(long int x, long int y, long int z)
{
    long3 tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.z = z;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long4 make_long4(long int x, long int y, long int z, long int w)
{
    long4 tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.z = z;
    tmp.w = w;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulong2 make_ulong2(unsigned long int x, unsigned long int y)
{
    ulong2 tmp;
    tmp.x = x;
    tmp.y = y;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulong3 make_ulong3(unsigned long int x, unsigned long int y, unsigned long int z)
{
    ulong3 tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.z = z;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulong4 make_ulong4(
    unsigned long int x, unsigned long int y, unsigned long int z, unsigned long int w)
{
    ulong4 tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.z = z;
    tmp.w = w;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline longlong2 make_longlong2(long long int x, long long int y)
{
    longlong2 tmp;
    tmp.x = x;
    tmp.y = y;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline longlong3 make_longlong3(long long int x, long long int y, long long int z)
{
    longlong3 tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.z = z;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline longlong4 make_longlong4(
    long long int x, long long int y, long long int z, long long int w)
{
    longlong4 tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.z = z;
    tmp.w = w;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulonglong2 make_ulonglong2(unsigned long long int x, unsigned long long int y)
{
    ulonglong2 tmp;
    tmp.x = x;
    tmp.y = y;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulonglong3 make_ulonglong3(
    unsigned long long int x, unsigned long long int y, unsigned long long int z)
{
    ulonglong3 tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.z = z;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulonglong4 make_ulonglong4(
    unsigned long long int x, unsigned long long int y, unsigned long long int z, unsigned long long int w)
{
    ulonglong4 tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.z = z;
    tmp.w = w;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int2 make_int2(int x, int y)
{
    int2 tmp;
    tmp.x = x;
    tmp.y = y;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int3 make_int3(int x, int y, int z)
{
    int3 tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.z = z;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int4 make_int4(int x, int y, int z, int w)
{
    int4 tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.z = z;
    tmp.w = w;
    return tmp;
}
__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint2 make_uint2(unsigned int x, unsigned int y)
{
    uint2 tmp;
    tmp.x = x;
    tmp.y = y;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint3 make_uint3(unsigned int x, unsigned int y, unsigned int z)
{
    uint3 tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.z = z;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint4 make_uint4(unsigned int x, unsigned int y, unsigned int z, unsigned int w)
{
    uint4 tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.z = z;
    tmp.w = w;
    return tmp;
}

#endif
#endif // IMPL_SIMT_API_VECTOR_FUNCTIONS_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_VECTOR_FUNCTIONS_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_VECTOR_FUNCTIONS_IMPL__
#endif
