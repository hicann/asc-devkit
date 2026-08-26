/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef INCLUDE_SIMT_API_DEVICE_FUNCTIONS_H
#define INCLUDE_SIMT_API_DEVICE_FUNCTIONS_H

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEVICE_FUNCTIONS_H__
#endif

#include "simt_api/device_types.h"
#include "simt_api/device_sync_functions.h"
#include "simt_api/device_warp_functions.h"
#include "simt_api/device_atomic_functions.h"

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __float2float_rn(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __float2float_rz(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __float2float_rd(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __float2float_ru(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __float2float_rna(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __float2uint_rn(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __float2uint_rz(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __float2uint_rd(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __float2uint_ru(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __float2uint_rna(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __float2int_rn(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __float2int_rz(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __float2int_rd(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __float2int_ru(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __float2int_rna(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __float2ull_rn(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __float2ull_rz(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __float2ull_rd(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __float2ull_ru(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __float2ull_rna(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __float2ll_rn(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __float2ll_rz(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __float2ll_rd(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __float2ll_ru(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __float2ll_rna(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __uint2float_rn(const unsigned int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __uint2float_rz(const unsigned int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __uint2float_rd(const unsigned int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __uint2float_ru(const unsigned int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __uint2float_rna(const unsigned int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __int2float_rn(const int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __int2float_rz(const int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __int2float_rd(const int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __int2float_ru(const int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __int2float_rna(const int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __ull2float_rn(const unsigned long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __ull2float_rz(const unsigned long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __ull2float_rd(const unsigned long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __ull2float_ru(const unsigned long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __ull2float_rna(const unsigned long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __ll2float_rn(const long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __ll2float_rz(const long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __ll2float_rd(const long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __ll2float_ru(const long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __ll2float_rna(const long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __int_as_float(const int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __uint_as_float(const unsigned int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __float_as_int(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __float_as_uint(const float x);

#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long int asc_ldcg(__gm__ long int* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long int asc_ldcg(__gm__ unsigned long int* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int asc_ldcg(__gm__ long long int* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int asc_ldcg(__gm__ unsigned long long int* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long2 asc_ldcg(__gm__ long2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulong2 asc_ldcg(__gm__ ulong2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long4 asc_ldcg(__gm__ long4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulong4 asc_ldcg(__gm__ ulong4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline longlong2 asc_ldcg(__gm__ longlong2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulonglong2 asc_ldcg(__gm__ ulonglong2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline longlong4 asc_ldcg(__gm__ longlong4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulonglong4 asc_ldcg(__gm__ ulonglong4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline signed char asc_ldcg(__gm__ signed char* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned char asc_ldcg(__gm__ unsigned char* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline char2 asc_ldcg(__gm__ char2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uchar2 asc_ldcg(__gm__ uchar2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline char4 asc_ldcg(__gm__ char4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uchar4 asc_ldcg(__gm__ uchar4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short asc_ldcg(__gm__ short* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned short asc_ldcg(__gm__ unsigned short* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short2 asc_ldcg(__gm__ short2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ushort2 asc_ldcg(__gm__ ushort2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short4 asc_ldcg(__gm__ short4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ushort4 asc_ldcg(__gm__ ushort4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int asc_ldcg(__gm__ int* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int asc_ldcg(__gm__ unsigned int* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int2 asc_ldcg(__gm__ int2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint2 asc_ldcg(__gm__ uint2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int4 asc_ldcg(__gm__ int4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint4 asc_ldcg(__gm__ uint4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asc_ldcg(__gm__ float* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float2 asc_ldcg(__gm__ float2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4 asc_ldcg(__gm__ float4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long int asc_ldca(__gm__ long int* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long int asc_ldca(__gm__ unsigned long int* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int asc_ldca(__gm__ long long int* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int asc_ldca(__gm__ unsigned long long int* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long2 asc_ldca(__gm__ long2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulong2 asc_ldca(__gm__ ulong2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long4 asc_ldca(__gm__ long4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulong4 asc_ldca(__gm__ ulong4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline longlong2 asc_ldca(__gm__ longlong2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulonglong2 asc_ldca(__gm__ ulonglong2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline longlong4 asc_ldca(__gm__ longlong4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulonglong4 asc_ldca(__gm__ ulonglong4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline signed char asc_ldca(__gm__ signed char* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned char asc_ldca(__gm__ unsigned char* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline char2 asc_ldca(__gm__ char2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uchar2 asc_ldca(__gm__ uchar2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline char4 asc_ldca(__gm__ char4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uchar4 asc_ldca(__gm__ uchar4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short asc_ldca(__gm__ short* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned short asc_ldca(__gm__ unsigned short* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short2 asc_ldca(__gm__ short2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ushort2 asc_ldca(__gm__ ushort2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short4 asc_ldca(__gm__ short4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ushort4 asc_ldca(__gm__ ushort4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int asc_ldca(__gm__ int* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int asc_ldca(__gm__ unsigned int* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int2 asc_ldca(__gm__ int2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint2 asc_ldca(__gm__ uint2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int4 asc_ldca(__gm__ int4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint4 asc_ldca(__gm__ uint4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asc_ldca(__gm__ float* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float2 asc_ldca(__gm__ float2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4 asc_ldca(__gm__ float4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ long int* address, long int val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ unsigned long int* address, unsigned long int val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ long long int* address, long long int val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ unsigned long long int* address, unsigned long long int val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ long2* address, long2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ ulong2* address, ulong2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ long4* address, long4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ ulong4* address, ulong4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ longlong2* address, longlong2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ ulonglong2* address, ulonglong2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ longlong4* address, longlong4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ ulonglong4* address, ulonglong4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ signed char* address, signed char val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ unsigned char* address, unsigned char val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ char* address, char val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ char2* address, char2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ uchar2* address, uchar2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ char4* address, char4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ uchar4* address, uchar4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ short* address, short val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ unsigned short* address, unsigned short val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ short2* address, short2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ ushort2* address, ushort2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ short4* address, short4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ ushort4* address, ushort4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ int* address, int val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ unsigned int* address, unsigned int val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ int2* address, int2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ uint2* address, uint2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ int4* address, int4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ uint4* address, uint4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ float* address, float val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ float2* address, float2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ float4* address, float4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ long int* address, long int val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ unsigned long int* address, unsigned long int val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ long long int* address, long long int val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ unsigned long long int* address, unsigned long long int val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ long2* address, long2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ ulong2* address, ulong2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ long4* address, long4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ ulong4* address, ulong4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ longlong2* address, longlong2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ ulonglong2* address, ulonglong2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ longlong4* address, longlong4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ ulonglong4* address, ulonglong4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ signed char* address, signed char val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ unsigned char* address, unsigned char val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ char* address, char val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ char2* address, char2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ uchar2* address, uchar2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ char4* address, char4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ uchar4* address, uchar4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ short* address, short val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ unsigned short* address, unsigned short val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ short2* address, short2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ ushort2* address, ushort2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ short4* address, short4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ ushort4* address, ushort4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ int* address, int val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ unsigned int* address, unsigned int val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ int2* address, int2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ uint2* address, uint2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ int4* address, int4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ uint4* address, uint4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ float* address, float val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ float2* address, float2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ float4* address, float4 val);
#else
static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int __isGlobal(const void* ptr);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int __isUbuf(const void* ptr);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int __isLocal(const void* ptr);

static __SIMT_DEVICE_FUNCTIONS_DECL__ size_t __cvta_generic_to_global(const void* ptr);

static __SIMT_DEVICE_FUNCTIONS_DECL__ size_t __cvta_generic_to_ubuf(const void* ptr);

static __SIMT_DEVICE_FUNCTIONS_DECL__ size_t __cvta_generic_to_local(const void* ptr);

static __SIMT_DEVICE_FUNCTIONS_DECL__ void* __cvta_global_to_generic(size_t rawbits);

static __SIMT_DEVICE_FUNCTIONS_DECL__ void* __cvta_ubuf_to_generic(size_t rawbits);

static __SIMT_DEVICE_FUNCTIONS_DECL__ void* __cvta_local_to_generic(size_t rawbits);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long int asc_ldcg(long int* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long int asc_ldcg(unsigned long int* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int asc_ldcg(long long int* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int asc_ldcg(unsigned long long int* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long2 asc_ldcg(long2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulong2 asc_ldcg(ulong2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long4 asc_ldcg(long4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulong4 asc_ldcg(ulong4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline longlong2 asc_ldcg(longlong2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulonglong2 asc_ldcg(ulonglong2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline longlong4 asc_ldcg(longlong4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulonglong4 asc_ldcg(ulonglong4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline signed char asc_ldcg(signed char* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned char asc_ldcg(unsigned char* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline char2 asc_ldcg(char2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uchar2 asc_ldcg(uchar2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline char4 asc_ldcg(char4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uchar4 asc_ldcg(uchar4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short asc_ldcg(short* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned short asc_ldcg(unsigned short* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short2 asc_ldcg(short2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ushort2 asc_ldcg(ushort2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short4 asc_ldcg(short4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ushort4 asc_ldcg(ushort4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int asc_ldcg(int* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int asc_ldcg(unsigned int* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int2 asc_ldcg(int2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint2 asc_ldcg(uint2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int4 asc_ldcg(int4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint4 asc_ldcg(uint4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asc_ldcg(float* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float2 asc_ldcg(float2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4 asc_ldcg(float4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long int asc_ldca(long int* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long int asc_ldca(unsigned long int* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int asc_ldca(long long int* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int asc_ldca(unsigned long long int* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long2 asc_ldca(long2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulong2 asc_ldca(ulong2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long4 asc_ldca(long4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulong4 asc_ldca(ulong4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline longlong2 asc_ldca(longlong2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulonglong2 asc_ldca(ulonglong2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline longlong4 asc_ldca(longlong4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ulonglong4 asc_ldca(ulonglong4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline signed char asc_ldca(signed char* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned char asc_ldca(unsigned char* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline char2 asc_ldca(char2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uchar2 asc_ldca(uchar2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline char4 asc_ldca(char4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uchar4 asc_ldca(uchar4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short asc_ldca(short* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned short asc_ldca(unsigned short* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short2 asc_ldca(short2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ushort2 asc_ldca(ushort2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short4 asc_ldca(short4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline ushort4 asc_ldca(ushort4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int asc_ldca(int* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int asc_ldca(unsigned int* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int2 asc_ldca(int2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint2 asc_ldca(uint2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int4 asc_ldca(int4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline uint4 asc_ldca(uint4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float asc_ldca(float* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float2 asc_ldca(float2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4 asc_ldca(float4* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(long int* address, long int val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(unsigned long int* address, unsigned long int val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(long long int* address, long long int val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(unsigned long long int* address, unsigned long long int val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(long2* address, long2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(ulong2* address, ulong2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(long4* address, long4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(ulong4* address, ulong4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(longlong2* address, longlong2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(ulonglong2* address, ulonglong2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(longlong4* address, longlong4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(ulonglong4* address, ulonglong4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(signed char* address, signed char val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(unsigned char* address, unsigned char val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(char* address, char val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(char2* address, char2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(uchar2* address, uchar2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(char4* address, char4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(uchar4* address, uchar4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(short* address, short val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(unsigned short* address, unsigned short val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(short2* address, short2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(ushort2* address, ushort2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(short4* address, short4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(ushort4* address, ushort4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(int* address, int val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(unsigned int* address, unsigned int val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(int2* address, int2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(uint2* address, uint2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(int4* address, int4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(uint4* address, uint4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(float* address, float val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(float2* address, float2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(float4* address, float4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(long int* address, long int val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(unsigned long int* address, unsigned long int val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(long long int* address, long long int val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(unsigned long long int* address, unsigned long long int val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(long2* address, long2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(ulong2* address, ulong2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(long4* address, long4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(ulong4* address, ulong4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(longlong2* address, longlong2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(ulonglong2* address, ulonglong2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(longlong4* address, longlong4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(ulonglong4* address, ulonglong4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(signed char* address, signed char val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(unsigned char* address, unsigned char val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(char* address, char val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(char2* address, char2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(uchar2* address, uchar2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(char4* address, char4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(uchar4* address, uchar4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(short* address, short val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(unsigned short* address, unsigned short val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(short2* address, short2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(ushort2* address, ushort2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(short4* address, short4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(ushort4* address, ushort4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(int* address, int val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(unsigned int* address, unsigned int val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(int2* address, int2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(uint2* address, uint2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(int4* address, int4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(uint4* address, uint4 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(float* address, float val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(float2* address, float2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(float4* address, float4 val);
#endif

static __SIMT_DEVICE_FUNCTIONS_DECL__ int __mulhi(int x, int y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int __umulhi(unsigned int x, unsigned int y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ long long __mul64hi(long long x, long long y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned long long __umul64hi(unsigned long long x, unsigned long long y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ long long __mul_i32toi64(int x, int y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned long long __mul_i32toi64(unsigned int x, unsigned int y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int __brev(unsigned int x);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned long long __brev(unsigned long long x);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned long __brev(unsigned long x);

static __SIMT_DEVICE_FUNCTIONS_DECL__ int __clz(int x);

static __SIMT_DEVICE_FUNCTIONS_DECL__ int __clz(long long x);

static __SIMT_DEVICE_FUNCTIONS_DECL__ int __clz(long x);

static __SIMT_DEVICE_FUNCTIONS_DECL__ int __ffs(int x);

static __SIMT_DEVICE_FUNCTIONS_DECL__ int __ffs(long long x);

static __SIMT_DEVICE_FUNCTIONS_DECL__ int __ffs(long x);

static __SIMT_DEVICE_FUNCTIONS_DECL__ int __popc(unsigned int x);

static __SIMT_DEVICE_FUNCTIONS_DECL__ int __popc(unsigned long long x);

static __SIMT_DEVICE_FUNCTIONS_DECL__ int __popc(unsigned long x);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int __byte_perm(unsigned int x, unsigned int y, unsigned int s);

static __SIMT_DEVICE_FUNCTIONS_DECL__ float __saturatef(float x);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int __sad(int x, int y, unsigned int z);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int __usad(unsigned int x, unsigned int y, unsigned int z);

static __SIMT_DEVICE_FUNCTIONS_DECL__ int __mul24(int x, int y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int __umul24(unsigned int x, unsigned int y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ float __fdividef(float x, float y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ int __hadd(int x, int y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ int __rhadd(int x, int y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int __uhadd(unsigned int x, unsigned int y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int __urhadd(unsigned int x, unsigned int y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ int32_t laneid();

static __SIMT_DEVICE_FUNCTIONS_DECL__ int32_t lanemask_eq();

static __SIMT_DEVICE_FUNCTIONS_DECL__ int32_t lanemask_le();

static __SIMT_DEVICE_FUNCTIONS_DECL__ int32_t lanemask_lt();

static __SIMT_DEVICE_FUNCTIONS_DECL__ int32_t lanemask_ge();

static __SIMT_DEVICE_FUNCTIONS_DECL__ int32_t lanemask_gt();

#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_dcci_entire(__gm__ void* dst);
#else
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_dcci_entire(void* dst);
#endif

/**
 * __asc_simt_vf : Implicit namespace, no qualification needed at call sites
 * Usage example:
 *    __gm__ void* dst;
 *    asc_dcci_single(dst);                 // recommended
 *    __asc_simt_vf::asc_dcci_single(dst);  // not recommended
 */
namespace __asc_simt_vf {
#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_dcci_single(__gm__ void* dst);
#else
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_dcci_single(void* dst);
#endif
} // namespace __asc_simt_vf

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_nop();

#include "impl/simt_api/device_functions_impl.h"

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEVICE_FUNCTIONS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEVICE_FUNCTIONS_H__
#endif

#endif // INCLUDE_SIMT_API_DEVICE_FUNCTIONS_H
