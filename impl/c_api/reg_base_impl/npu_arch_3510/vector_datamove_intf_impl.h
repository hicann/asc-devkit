/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning \
    "impl/c_api/reg_base_impl/npu_arch_3510/vector_datamove_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_3510_VECTOR_DATAMOVE_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_3510_VECTOR_DATAMOVE_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

__aicore__ inline void asc_copy_gm2ub(
    __ubuf__ void* dst, __gm__ void* src, uint32_t burst_count, uint32_t burst_len, uint64_t src_stride,
    uint32_t dst_stride)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            (__ubuf__ uint8_t*)dst, (__gm__ uint8_t*)src, 0, burst_count, burst_len, 0, 0, false, 0, src_stride,
            dst_stride);
    }
}

__aicore__ inline void asc_copy_gm2ub(__ubuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2ub(dst, src, 1, size, 0, 0);
}

__aicore__ inline void asc_copy_ub2gm(
    __gm__ void* dst, __ubuf__ void* src, uint32_t burst_count, uint32_t burst_len, uint64_t dst_stride,
    uint32_t src_stride)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(dst, src, 0, burst_count, burst_len, 0, dst_stride, src_stride);
    }
}

__aicore__ inline void asc_copy_ub2gm(__gm__ void* dst, __ubuf__ void* src, uint32_t size)
{
    asc_copy_ub2gm(dst, src, 1, size, 0, 0);
}

// Generate the existing concrete overloads for each intrinsic and signature.
#define ASC_DEFINE_3510_COPY_GM2UB_ALIGN_1(Type1)                                                                   \
    __aicore__ inline void asc_copy_gm2ub_align(                                                                    \
        __ubuf__ Type1* dst, __gm__ Type1* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num, \
        uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode,                  \
        uint64_t src_stride, uint32_t dst_stride)                                                                   \
    {                                                                                                               \
        if ASC_IS_AIV {                                                                                             \
            copy_gm_to_ubuf_align_v2(                                                                               \
                dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad,      \
                static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride);                                       \
        }                                                                                                           \
    }

ASC_DEFINE_3510_COPY_GM2UB_ALIGN_1(uint8_t)

#define ASC_DEFINE_3510_COPY_GM2UB_ALIGN_2(Type1)                                                           \
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ Type1* dst, __gm__ Type1* src, uint32_t size)      \
    {                                                                                                       \
        asc_copy_gm2ub_align(dst, src, 1, size, 0, 0, false, static_cast<asc_load_l2_cache_mode>(0), 0, 0); \
    }

ASC_DEFINE_3510_COPY_GM2UB_ALIGN_2(uint8_t)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_1(int8_t)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_2(int8_t)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_1(uint16_t)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_2(uint16_t)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_1(int16_t)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_2(int16_t)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_1(uint32_t)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_2(uint32_t)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_1(int32_t)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_2(int32_t)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_1(half)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_2(half)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_1(float)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_2(float)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_1(hifloat8_t)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_2(hifloat8_t)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_1(bfloat16_t)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_2(bfloat16_t)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_1(fp8_e4m3fn_t)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_2(fp8_e4m3fn_t)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_1(fp8_e5m2_t)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_2(fp8_e5m2_t)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_1(fp8_e8m0_t)

#undef ASC_DEFINE_3510_COPY_GM2UB_ALIGN_1

ASC_DEFINE_3510_COPY_GM2UB_ALIGN_2(fp8_e8m0_t)

#undef ASC_DEFINE_3510_COPY_GM2UB_ALIGN_2

#define ASC_DEFINE_3510_COPY_UB2GM_ALIGN_1(Type1)                                         \
    __aicore__ inline void asc_copy_ub2gm_align(                                          \
        __gm__ Type1* dst, __ubuf__ Type1* src, uint32_t burst_count, uint32_t burst_len, \
        asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)  \
    {                                                                                     \
        if ASC_IS_AIV {                                                                   \
            copy_ubuf_to_gm_align_v2(                                                     \
                (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len,        \
                static_cast<uint8_t>(l2_cache_mode), dst_stride, src_stride);             \
        }                                                                                 \
    }

ASC_DEFINE_3510_COPY_UB2GM_ALIGN_1(uint8_t)

#define ASC_DEFINE_3510_COPY_UB2GM_ALIGN_2(Type1)                                                      \
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ Type1* dst, __ubuf__ Type1* src, uint32_t size) \
    {                                                                                                  \
        asc_copy_ub2gm_align(dst, src, 1, size, static_cast<asc_store_l2_cache_mode>(0), 0, 0);        \
    }

ASC_DEFINE_3510_COPY_UB2GM_ALIGN_2(uint8_t)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_1(int8_t)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_2(int8_t)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_1(uint16_t)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_2(uint16_t)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_1(int16_t)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_2(int16_t)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_1(uint32_t)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_2(uint32_t)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_1(int32_t)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_2(int32_t)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_1(half)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_2(half)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_1(float)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_2(float)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_1(hifloat8_t)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_2(hifloat8_t)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_1(bfloat16_t)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_2(bfloat16_t)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_1(fp8_e4m3fn_t)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_2(fp8_e4m3fn_t)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_1(fp8_e5m2_t)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_2(fp8_e5m2_t)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_1(fp8_e8m0_t)

#undef ASC_DEFINE_3510_COPY_UB2GM_ALIGN_1

ASC_DEFINE_3510_COPY_UB2GM_ALIGN_2(fp8_e8m0_t)

#undef ASC_DEFINE_3510_COPY_UB2GM_ALIGN_2

#define ASC_DEFINE_3510_NDIM_COPY_GM2UB_ENUM(DataType, CopyIntrinsic)                                                  \
    __aicore__ inline void asc_ndim_copy_gm2ub(                                                                        \
        __ubuf__ DataType* dst, __gm__ DataType* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,   \
        uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,   \
        asc_load_l2_cache_mode l2_cache_mode)                                                                          \
    {                                                                                                                  \
        if ASC_IS_AIV {                                                                                                \
            CopyIntrinsic(                                                                                             \
                (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size, \
                loop0_lp_count, loop0_rp_count, padding_mode, static_cast<uint8_t>(l2_cache_mode));                    \
        }                                                                                                              \
    }

ASC_DEFINE_3510_NDIM_COPY_GM2UB_ENUM(int8_t, nddma_out_to_ub_b8)
ASC_DEFINE_3510_NDIM_COPY_GM2UB_ENUM(uint8_t, nddma_out_to_ub_b8)
ASC_DEFINE_3510_NDIM_COPY_GM2UB_ENUM(fp4x2_e2m1_t, nddma_out_to_ub_b8)
ASC_DEFINE_3510_NDIM_COPY_GM2UB_ENUM(fp4x2_e1m2_t, nddma_out_to_ub_b8)
ASC_DEFINE_3510_NDIM_COPY_GM2UB_ENUM(fp8_e8m0_t, nddma_out_to_ub_b8)
ASC_DEFINE_3510_NDIM_COPY_GM2UB_ENUM(fp8_e5m2_t, nddma_out_to_ub_b8)
ASC_DEFINE_3510_NDIM_COPY_GM2UB_ENUM(fp8_e4m3fn_t, nddma_out_to_ub_b8)
ASC_DEFINE_3510_NDIM_COPY_GM2UB_ENUM(int16_t, nddma_out_to_ub_b16)
ASC_DEFINE_3510_NDIM_COPY_GM2UB_ENUM(uint16_t, nddma_out_to_ub_b16)
ASC_DEFINE_3510_NDIM_COPY_GM2UB_ENUM(half, nddma_out_to_ub_b16)
ASC_DEFINE_3510_NDIM_COPY_GM2UB_ENUM(bfloat16_t, nddma_out_to_ub_b16)
ASC_DEFINE_3510_NDIM_COPY_GM2UB_ENUM(int32_t, nddma_out_to_ub_b32)
ASC_DEFINE_3510_NDIM_COPY_GM2UB_ENUM(uint32_t, nddma_out_to_ub_b32)
ASC_DEFINE_3510_NDIM_COPY_GM2UB_ENUM(float, nddma_out_to_ub_b32)

#undef ASC_DEFINE_3510_NDIM_COPY_GM2UB_ENUM

#define ASC_DEFINE_3510_NDIM_COPY_GM2UB_1(Type1)                                                                       \
    ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)                                                        \
    __aicore__ inline void asc_ndim_copy_gm2ub(                                                                        \
        __ubuf__ Type1* dst, __gm__ Type1* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,         \
        uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,   \
        uint8_t cache_mode)                                                                                            \
    {                                                                                                                  \
        if ASC_IS_AIV {                                                                                                \
            nddma_out_to_ub_b8(                                                                                        \
                (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size, \
                loop0_lp_count, loop0_rp_count, padding_mode, cache_mode);                                             \
        }                                                                                                              \
    }

ASC_DEFINE_3510_NDIM_COPY_GM2UB_1(int8_t)
ASC_DEFINE_3510_NDIM_COPY_GM2UB_1(uint8_t)
ASC_DEFINE_3510_NDIM_COPY_GM2UB_1(fp4x2_e2m1_t)
ASC_DEFINE_3510_NDIM_COPY_GM2UB_1(fp4x2_e1m2_t)
ASC_DEFINE_3510_NDIM_COPY_GM2UB_1(fp8_e8m0_t)
ASC_DEFINE_3510_NDIM_COPY_GM2UB_1(fp8_e5m2_t)
ASC_DEFINE_3510_NDIM_COPY_GM2UB_1(fp8_e4m3fn_t)

#undef ASC_DEFINE_3510_NDIM_COPY_GM2UB_1

#define ASC_DEFINE_3510_NDIM_COPY_GM2UB_2(Type1)                                                                       \
    ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)                                                        \
    __aicore__ inline void asc_ndim_copy_gm2ub(                                                                        \
        __ubuf__ Type1* dst, __gm__ Type1* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,         \
        uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,   \
        uint8_t cache_mode)                                                                                            \
    {                                                                                                                  \
        if ASC_IS_AIV {                                                                                                \
            nddma_out_to_ub_b16(                                                                                       \
                (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size, \
                loop0_lp_count, loop0_rp_count, padding_mode, cache_mode);                                             \
        }                                                                                                              \
    }

ASC_DEFINE_3510_NDIM_COPY_GM2UB_2(int16_t)
ASC_DEFINE_3510_NDIM_COPY_GM2UB_2(uint16_t)
ASC_DEFINE_3510_NDIM_COPY_GM2UB_2(half)
ASC_DEFINE_3510_NDIM_COPY_GM2UB_2(bfloat16_t)

#undef ASC_DEFINE_3510_NDIM_COPY_GM2UB_2

#define ASC_DEFINE_3510_NDIM_COPY_GM2UB_3(Type1)                                                                       \
    ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)                                                        \
    __aicore__ inline void asc_ndim_copy_gm2ub(                                                                        \
        __ubuf__ Type1* dst, __gm__ Type1* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,         \
        uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,   \
        uint8_t cache_mode)                                                                                            \
    {                                                                                                                  \
        if ASC_IS_AIV {                                                                                                \
            nddma_out_to_ub_b32(                                                                                       \
                (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size, \
                loop0_lp_count, loop0_rp_count, padding_mode, cache_mode);                                             \
        }                                                                                                              \
    }

ASC_DEFINE_3510_NDIM_COPY_GM2UB_3(int32_t)
ASC_DEFINE_3510_NDIM_COPY_GM2UB_3(uint32_t)
ASC_DEFINE_3510_NDIM_COPY_GM2UB_3(float)

#undef ASC_DEFINE_3510_NDIM_COPY_GM2UB_3

#define ASC_DEFINE_3510_COPY_GM2UB_ALIGN_3(Type1)                                                                   \
    ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)                                                     \
    __aicore__ inline void asc_copy_gm2ub_align(                                                                    \
        __ubuf__ Type1* dst, __gm__ Type1* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num, \
        uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride,            \
        uint32_t dst_stride)                                                                                        \
    {                                                                                                               \
        if ASC_IS_AIV {                                                                                             \
            copy_gm_to_ubuf_align_v2(                                                                               \
                dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad,      \
                l2_cache_mode, src_stride, dst_stride);                                                             \
        }                                                                                                           \
    }

ASC_DEFINE_3510_COPY_GM2UB_ALIGN_3(uint8_t)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_3(int8_t)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_3(uint16_t)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_3(int16_t)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_3(uint32_t)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_3(int32_t)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_3(half)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_3(float)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_3(hifloat8_t)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_3(bfloat16_t)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_3(fp8_e5m2_t)
ASC_DEFINE_3510_COPY_GM2UB_ALIGN_3(fp8_e4m3fn_t)

#undef ASC_DEFINE_3510_COPY_GM2UB_ALIGN_3

#define ASC_DEFINE_3510_COPY_UB2GM_ALIGN_3(Type1)                                                                \
    ASC_DEPRECATED(9.2.0, "2027/09/07", asc_store_l2_cache_mode)                                                 \
    __aicore__ inline void asc_copy_ub2gm_align(                                                                 \
        __gm__ Type1* dst, __ubuf__ Type1* src, uint32_t burst_count, uint32_t burst_len, uint8_t l2_cache_mode, \
        uint64_t dst_stride, uint32_t src_stride)                                                                \
    {                                                                                                            \
        if ASC_IS_AIV {                                                                                          \
            copy_ubuf_to_gm_align_v2(                                                                            \
                (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, l2_cache_mode, dst_stride,    \
                src_stride);                                                                                     \
        }                                                                                                        \
    }

ASC_DEFINE_3510_COPY_UB2GM_ALIGN_3(uint8_t)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_3(int8_t)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_3(uint16_t)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_3(int16_t)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_3(uint32_t)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_3(int32_t)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_3(half)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_3(float)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_3(hifloat8_t)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_3(bfloat16_t)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_3(fp8_e5m2_t)
ASC_DEFINE_3510_COPY_UB2GM_ALIGN_3(fp8_e4m3fn_t)

#undef ASC_DEFINE_3510_COPY_UB2GM_ALIGN_3

ASC_DEPRECATED(9.1.0, "2027/09/07", asc_set_copy_pad_val)
__aicore__ inline void asc_set_gm2ub_pad(uint32_t pad_val)
{
    if ASC_IS_AIV {
        set_pad_val_outtoub(pad_val);
    }
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
