/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef INCLUDE_SIMT_API_ASC_FP16_H
#define INCLUDE_SIMT_API_ASC_FP16_H

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_FP16_H__
#endif

#include "simt_api/asc_simt.h"
#include "simt_api/device_types.h"

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)

#define ASCRT_INF_FP16 __ushort_as_half((unsigned short)0x7C00U)
#define ASCRT_MAX_NORMAL_FP16 __ushort_as_half((unsigned short)0x7BFFU)
#define ASCRT_MIN_DENORM_FP16 __ushort_as_half((unsigned short)0x0001U)
#define ASCRT_NAN_FP16 __ushort_as_half((unsigned short)0x7FFFU)
#define ASCRT_NEG_ZERO_FP16 __ushort_as_half((unsigned short)0x8000U)
#define ASCRT_ONE_FP16 __ushort_as_half((unsigned short)0x3C00U)
#define ASCRT_ZERO_FP16 __ushort_as_half((unsigned short)0x0000U)

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bool __hisnan(half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bool __hisinf(half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __hfma(half x, half y, half z);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __habs(half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __hmax(half x, half y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __hmin(half x, half y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half hcos(half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2cos(half2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half hsin(half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2sin(half2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half htanh(half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2tanh(half2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half hexp(half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2exp(half2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half hexp2(half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2exp2(half2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half hexp10(half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2exp10(half2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half hlog(half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2log(half2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half hlog2(half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2log2(half2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half hlog10(half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2log10(half2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half hsqrt(half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2sqrt(half2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half hrsqrt(half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2rsqrt(half2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half hrcp(half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2rcp(half2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half hfloor(half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2floor(half2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half hrint(half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2rint(half2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half hceil(half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2ceil(half2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half htrunc(half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2trunc(half2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __float2half(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __float2half_rn(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __float2half_rn_sat(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __float2half_rz(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __float2half_rz_sat(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __float2half_rd(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __float2half_rd_sat(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __float2half_ru(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __float2half_ru_sat(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __float2half_rna(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __float2half_rna_sat(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __float2half_ro(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __float2half_ro_sat(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __float22half2_rn_sat(const float2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __float22half2_rz_sat(const float2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __float22half2_rd(const float2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __float22half2_rd_sat(const float2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __float22half2_ru(const float2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __float22half2_ru_sat(const float2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __float22half2_rna(const float2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __float22half2_rna_sat(const float2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __float22half2_ro(const float2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __float22half2_ro_sat(const float2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __half2float(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __half2half_rn(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __half2half_rz(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __half2half_rd(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __half2half_ru(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __half2half_rna(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline signed char __half2char_rz(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned char __half2uchar_rz(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short int __half2short_rn(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short int __half2short_rz(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short int __half2short_rd(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short int __half2short_ru(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned short int __half2ushort_rn(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned short int __half2ushort_rz(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned short int __half2ushort_rd(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned short int __half2ushort_ru(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __half2uint_rn(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __half2uint_rz(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __half2uint_rd(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __half2uint_ru(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __half2uint_rna(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __half2int_rn(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __half2int_rz(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __half2int_rd(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __half2int_ru(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __half2int_rna(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __half2ull_rn(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __half2ull_rz(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __half2ull_rd(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __half2ull_ru(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __half2ull_rna(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __half2ll_rn(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __half2ll_rz(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __half2ll_rd(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __half2ll_ru(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __half2ll_rna(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __uint2half_rn(const unsigned int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __uint2half_rn_sat(const unsigned int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __uint2half_rz(const unsigned int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __uint2half_rz_sat(const unsigned int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __uint2half_rd(const unsigned int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __uint2half_rd_sat(const unsigned int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __uint2half_ru(const unsigned int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __uint2half_ru_sat(const unsigned int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __uint2half_rna(const unsigned int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __uint2half_rna_sat(const unsigned int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __int2half_rn(const int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __int2half_rn_sat(const int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __int2half_rz(const int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __int2half_rz_sat(const int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __int2half_rd(const int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __int2half_rd_sat(const int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __int2half_ru(const int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __int2half_ru_sat(const int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __int2half_rna(const int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __int2half_rna_sat(const int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __short2half_rn(const short int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __short2half_rz(const short int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __short2half_rd(const short int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __short2half_ru(const short int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ushort2half_rn(const unsigned short int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ushort2half_rz(const unsigned short int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ushort2half_rd(const unsigned short int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ushort2half_ru(const unsigned short int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ll2half_rn(const long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ll2half_rz(const long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ll2half_rd(const long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ll2half_ru(const long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ll2half_rna(const long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ull2half_rn(const unsigned long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ull2half_rz(const unsigned long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ull2half_rd(const unsigned long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ull2half_ru(const unsigned long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ull2half_rna(const unsigned long long int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __floats2half2_rn(const float x, const float y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __float22half2_rn(const float2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __float2half2_rn(const float x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __half2half2(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __low2float(const half2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __low2half(const half2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __low2half2(const half2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __lowhigh2highlow(const half2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __high2float(const half2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __high2half(const half2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __high2half2(const half2 x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __highs2half2(const half2 x, const half2 y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __lows2half2(const half2 x, const half2 y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __halves2half2(const half x, const half y);

#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_atomic_add(__ubuf__ half* address, half val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_add(__ubuf__ half2* address, half2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_atomic_add(__gm__ half* address, half val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_add(__gm__ half2* address, half2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_sub(__ubuf__ half2* address, half2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_sub(__gm__ half2* address, half2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_atomic_sub(__ubuf__ half* address, half val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_atomic_sub(__gm__ half* address, half val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_exch(__ubuf__ half2* address, half2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_exch(__gm__ half2* address, half2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_atomic_max(__ubuf__ half* address, half val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_max(__ubuf__ half2* address, half2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_atomic_max(__gm__ half* address, half val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_max(__gm__ half2* address, half2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_atomic_min(__ubuf__ half* address, half val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_min(__ubuf__ half2* address, half2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_atomic_min(__gm__ half* address, half val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_min(__gm__ half2* address, half2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_cas(__ubuf__ half2* address, half2 compare, half2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_cas(__gm__ half2* address, half2 compare, half2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_ldcg(__gm__ half* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_ldcg(__gm__ half2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_ldca(__gm__ half* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_ldca(__gm__ half2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ half* address, half val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ half2* address, half2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ half* address, half val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ half2* address, half2 val);

#else
#ifndef ASCENDC_CPU_DEBUG
__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_atomic_add(half* address, half val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_add(half2* address, half2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_sub(half2* address, half2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_atomic_sub(half* address, half val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_exch(half2* address, half2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_atomic_max(half* address, half val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_max(half2* address, half2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_atomic_min(half* address, half val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_min(half2* address, half2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_cas(half2* address, half2 compare, half2 val);
#endif

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_ldcg(half* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_ldcg(half2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_ldca(half* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_ldca(half2* address);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(half* address, half val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(half2* address, half2 val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(half* address, half val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(half2* address, half2 val);
#endif

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_shfl(half var, int32_t src_lane, int32_t width = warpSize);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_shfl(half2 var, int32_t src_lane, int32_t width = warpSize);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_shfl_up(half var, uint32_t delta, int32_t width = warpSize);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_shfl_up(half2 var, uint32_t delta, int32_t width = warpSize);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_shfl_down(half var, uint32_t delta, int32_t width = warpSize);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_shfl_down(half2 var, uint32_t delta, int32_t width = warpSize);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_shfl_xor(half var, int32_t lane_mask, int32_t width = warpSize);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_shfl_xor(half2 var, int32_t lane_mask, int32_t width = warpSize);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_reduce_add(half val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_reduce_max(half val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_reduce_min(half val);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 make_half2(half x, half y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short int __half_as_short(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned short int __half_as_ushort(const half x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __short_as_half(const short int x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ushort_as_half(const unsigned short int x);

static __SIMT_DEVICE_FUNCTIONS_DECL__ bool __heq(half x, half y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ bool __hne(half x, half y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ bool __hle(half x, half y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ bool __hge(half x, half y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ bool __hlt(half x, half y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ bool __hgt(half x, half y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ bool __hequ(half x, half y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ bool __hneu(half x, half y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ bool __hleu(half x, half y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ bool __hgeu(half x, half y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ bool __hltu(half x, half y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ bool __hgtu(half x, half y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ bool __hbeqx2(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ bool __hbnex2(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ bool __hblex2(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ bool __hbgex2(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ bool __hbltx2(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ bool __hbgtx2(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ bool __hbequx2(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ bool __hbneux2(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ bool __hbleux2(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ bool __hbgeux2(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ bool __hbltux2(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ bool __hbgtux2(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half2 __heqx2(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half2 __hnex2(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half2 __hlex2(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half2 __hgex2(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half2 __hltx2(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half2 __hgtx2(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half2 __hequx2(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half2 __hneux2(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half2 __hleux2(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half2 __hgeux2(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half2 __hltux2(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half2 __hgtux2(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int __heqx2_mask(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int __hnex2_mask(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int __hlex2_mask(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int __hgex2_mask(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int __hltx2_mask(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int __hgtx2_mask(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int __hequx2_mask(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int __hneux2_mask(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int __hleux2_mask(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int __hgeux2_mask(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int __hltux2_mask(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ unsigned int __hgtux2_mask(half2 x, half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half2 __isnanx2(half2 x);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half2 __hmaxx2(const half2 x, const half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half2 __hminx2(const half2 x, const half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half2 __haddx2(const half2 x, const half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half2 __hsubx2(const half2 x, const half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half2 __hmulx2(const half2 x, const half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half2 __hdivx2(const half2 x, const half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half2 __habsx2(const half2 x);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half2 __hfmax2(const half2 x, const half2 y, const half2 z);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half2 __hnegx2(const half2 x);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half __hadd(const half x, const half y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half __hsub(const half x, const half y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half __hmul(const half x, const half y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half __hdiv(const half x, const half y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half __hneg(const half x);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half __hmax_nan(const half x, const half y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half __hmin_nan(const half x, const half y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half __hfma_relu(const half x, const half y, const half z);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half2 __hmaxx2_nan(const half2 x, const half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half2 __hminx2_nan(const half2 x, const half2 y);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half2 __hfmax2_relu(const half2 x, const half2 y, const half2 z);

static __SIMT_DEVICE_FUNCTIONS_DECL__ half2 __hcmadd(const half2 x, const half2 y, const half2 z);

static __SIMT_DEVICE_FUNCTIONS_DECL__ float2 __half22float2(const half2 x);

#include "impl/simt_api/asc_fp16_impl.h"

#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_FP16_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_FP16_H__
#endif
#endif // INCLUDE_SIMT_API_ASC_FP16_H
