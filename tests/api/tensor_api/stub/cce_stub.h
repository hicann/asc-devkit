/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef TESTS_API_TENSOR_API_STUB_CCE_STUB_H
#define TESTS_API_TENSOR_API_STUB_CCE_STUB_H

#include "c_api/stub/cce_stub.h"

using hif4x2_t = hifloat4x2_t;
using float8_e8m0_t = fp8_e8m0_t;

inline void vgather2(vector_s16& dst, __ubuf__ int8_t* src, vector_u16 index, vector_bool mask) {}
inline void vgather2(vector_u16& dst, __ubuf__ uint8_t* src, vector_u16 index, vector_bool mask) {}
inline void vgather2(vector_s8& dst, __ubuf__ int8_t* src, vector_u16 index, vector_bool mask) {}
inline void vgather2(vector_u8& dst, __ubuf__ uint8_t* src, vector_u16 index, vector_bool mask) {}
inline void vgather2(vector_s16& dst, __ubuf__ int16_t* src, vector_u16 index, vector_bool mask) {}
inline void vgather2(vector_u16& dst, __ubuf__ uint16_t* src, vector_u16 index, vector_bool mask) {}
inline void vgather2(vector_s32& dst, __ubuf__ int32_t* src, vector_u32 index, vector_bool mask) {}
inline void vgather2(vector_u32& dst, __ubuf__ uint32_t* src, vector_u32 index, vector_bool mask) {}
inline void vgather2(vector_f16& dst, __ubuf__ half* src, vector_u16 index, vector_bool mask) {}
inline void vgather2(vector_f32& dst, __ubuf__ float* src, vector_u32 index, vector_bool mask) {}
inline void vgather2(vector_bf16& dst, __ubuf__ bfloat16_t* src, vector_u16 index, vector_bool mask) {}
inline void vgather2(vector_f8e4m3& dst, __ubuf__ float8_e4m3_t* src, vector_u16 index, vector_bool mask) {}
inline void vgather2(vector_f8e5m2& dst, __ubuf__ float8_e5m2_t* src, vector_u16 index, vector_bool mask) {}
inline void vgather2(vector_f8e8m0& dst, __ubuf__ float8_e8m0_t* src, vector_u16 index, vector_bool mask) {}
inline void vselr(vector_s8& dst, vector_s8 src, vector_u8 index) {}
inline void vselr(vector_u8& dst, vector_u8 src, vector_u8 index) {}
inline void vselr(vector_s16& dst, vector_s16 src, vector_u16 index) {}
inline void vselr(vector_u16& dst, vector_u16 src, vector_u16 index) {}
inline void vselr(vector_s32& dst, vector_s32 src, vector_u32 index) {}
inline void vselr(vector_u32& dst, vector_u32 src, vector_u32 index) {}
inline void vselr(vector_f16& dst, vector_f16 src, vector_u16 index) {}
inline void vselr(vector_bf16& dst, vector_bf16 src, vector_u16 index) {}
inline void vselr(vector_f8e4m3& dst, vector_f8e4m3 src, vector_u8 index) {}
inline void vselr(vector_f8e5m2& dst, vector_f8e5m2 src, vector_u8 index) {}
inline void vselr(vector_f8e8m0& dst, vector_f8e8m0 src, vector_u8 index) {}
inline void vci(vector_s8& dst, int8_t offset, Literal order) {}
inline void vci(vector_s16& dst, int16_t offset, Literal order) {}
inline void vci(vector_s32& dst, int32_t offset, Literal order) {}
inline void vci(vector_f16& dst, half offset, Literal order) {}
inline void vci(vector_f32& dst, float offset, Literal order) {}

template <typename T>
inline void asc_mem_bar(T)
{}

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ != 3510)
using vector_uint8_t = uint8_t;
using vector_uint16_t = uint8_t;
using vector_uint32_t = uint8_t;
using vector_uint64_t = uint8_t;
using vector_int8_t = uint8_t;
using vector_int16_t = uint8_t;
using vector_int32_t = uint8_t;
using vector_int64_t = uint8_t;
using vector_bfloat16_t = uint8_t;
using vector_half = uint8_t;
using vector_float = uint8_t;
using vector_hifloat8_t = uint8_t;
using vector_fp8_e4m3fn_t = uint8_t;
using vector_fp8_e5m2_t = uint8_t;
using vector_fp8_e8m0_t = uint8_t;
using vector_int4x2_t = uint8_t;
using vector_fp4x2_e2m1_t = uint8_t;
using vector_fp4x2_e1m2_t = uint8_t;
#endif

#endif
