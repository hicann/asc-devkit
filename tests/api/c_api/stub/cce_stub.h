/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef __TESTS_C_API_STUB__
#define __TESTS_C_API_STUB__
#include <cstdint>
#include "stub_fun.h"
#include "c_api/utils/enum.h"

template <typename C, typename A, typename B>
inline void mad(
    __cc__ C* c_matrix, __ca__ A* a_matrix, __cb__ B* b_matrix, uint16_t left_height, uint16_t n_dim,
    uint16_t right_width, asc_unit_flag_mode unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{
    mad(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width, static_cast<uint8_t>(unit_flag), disable_gemv,
        c_matrix_source, c_matrix_init_val);
}

template <typename C, typename A, typename B>
inline void mad_mx(
    __cc__ C* c_matrix, __ca__ A* a_matrix, __cb__ B* b_matrix, uint16_t left_height, uint16_t n_dim,
    uint16_t right_width, asc_unit_flag_mode unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{
    mad_mx(
        c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width, static_cast<uint8_t>(unit_flag), disable_gemv,
        c_matrix_source, c_matrix_init_val);
}

// The compiler supports this overload, but it is missing from the CPU debug stub header.
int64_t ld_dev(__gm__ int64_t* src, int16_t offset);

// The compiler supports this overload, but it is missing from the CPU debug stub header.
inline void vmadd(vector_bf16& dst, vector_bf16 src0, vector_bf16 src1, vector_bool mask, Literal mode) {}

#ifndef ULL
#define ULL unsigned long long
#endif

static bool is_mock_copy_matrix_cc_to_gm = false;
static uint16_t n_size_global = 0;
static uint16_t m_size_global = 0;
static uint32_t dst_stride_global = 0;
static uint16_t src_stride_global = 0;
static bool NZ2ND_en_global = false;
static bool NZ2DN_en_global = false;
static void* gm_addr_global = nullptr;
static void* l0c_addr_global = nullptr;
static bool is_mock_copy_matrix_cc_to_ub = false;
static void* ub_addr_global = nullptr;
static uint64_t quant_pre_global = 0;

#define mock_copy_matrix_cc_to_gm(DstT, L0cT)                                                                   \
    inline void copy_matrix_cc_to_gm(                                                                           \
        __gm__ DstT* dst_addr, __cc__ L0cT* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,            \
        uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre,        \
        uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en,              \
        uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op,  \
        bool eltwise_antq_en, bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, \
        bool nz2dn_en)                                                                                          \
    {                                                                                                           \
        if (is_mock_copy_matrix_cc_to_gm) {                                                                     \
            EXPECT_EQ(n_size, n_size_global);                                                                   \
            EXPECT_EQ(m_size, m_size_global);                                                                   \
            EXPECT_EQ(loop_dst_stride, dst_stride_global);                                                      \
            EXPECT_EQ(loop_src_stride, src_stride_global);                                                      \
            EXPECT_EQ(nz2nd_en, NZ2ND_en_global);                                                               \
            EXPECT_EQ(nz2dn_en, NZ2DN_en_global);                                                               \
            if (gm_addr_global != nullptr) {                                                                    \
                EXPECT_EQ(dst_addr, gm_addr_global);                                                            \
            }                                                                                                   \
            if (l0c_addr_global != nullptr) {                                                                   \
                EXPECT_EQ(src_addr, l0c_addr_global);                                                           \
            }                                                                                                   \
            EXPECT_EQ(quant_pre, quant_pre_global);                                                             \
        }                                                                                                       \
    }

mock_copy_matrix_cc_to_gm(float, float);
mock_copy_matrix_cc_to_gm(half, float);
mock_copy_matrix_cc_to_gm(int8_t, int32_t);

#ifndef __biasbuf__
#define __biasbuf__
#endif

inline void sprsts(Literal, __ubuf__ uint32_t*, int32_t) {}

inline void sprsts(Literal, __ubuf__ uint32_t*&, int32_t, Literal) {}

void vsts(
    vector_f8e4m3 data, __ubuf__ fp8_e4m3fn_t* base, int32_t offset, Literal dist, vector_bool mask, Literal mode);
void vsts(vector_f8e5m2 data, __ubuf__ fp8_e5m2_t* base, int32_t offset, Literal dist, vector_bool mask, Literal mode);
void vsts(vector_f8e8m0 data, __ubuf__ fp8_e8m0_t* base, int32_t offset, Literal dist, vector_bool mask, Literal mode);
void vsts(
    vector_f4e2m1x2 data, __ubuf__ fp4x2_e2m1_t* base, int32_t offset, Literal dist, vector_bool mask, Literal mode);
void vsts(
    vector_f4e1m2x2 data, __ubuf__ fp4x2_e1m2_t* base, int32_t offset, Literal dist, vector_bool mask, Literal mode);

void vsts(
    vector_s8 src0, vector_s8 src1, __ubuf__ int8_t* base, int32_t offset, Literal dist, vector_bool mask,
    Literal mode);
void vsts(
    vector_u8 src0, vector_u8 src1, __ubuf__ uint8_t* base, int32_t offset, Literal dist, vector_bool mask,
    Literal mode);
void vsts(
    vector_s16 src0, vector_s16 src1, __ubuf__ int16_t* base, int32_t offset, Literal dist, vector_bool mask,
    Literal mode);
void vsts(
    vector_u16 src0, vector_u16 src1, __ubuf__ uint16_t* base, int32_t offset, Literal dist, vector_bool mask,
    Literal mode);
void vsts(
    vector_s32 src0, vector_s32 src1, __ubuf__ int32_t* base, int32_t offset, Literal dist, vector_bool mask,
    Literal mode);
void vsts(
    vector_u32 src0, vector_u32 src1, __ubuf__ uint32_t* base, int32_t offset, Literal dist, vector_bool mask,
    Literal mode);
void vsts(
    vector_f16 src0, vector_f16 src1, __ubuf__ half* base, int32_t offset, Literal dist, vector_bool mask,
    Literal mode);
void vsts(
    vector_bf16 src0, vector_bf16 src1, __ubuf__ bfloat16_t* base, int32_t offset, Literal dist, vector_bool mask,
    Literal mode);
void vsts(
    vector_f8e4m3 src0, vector_f8e4m3 src1, __ubuf__ fp8_e4m3fn_t* base, int32_t offset, Literal dist, vector_bool mask,
    Literal mode);
void vsts(
    vector_f8e5m2 src0, vector_f8e5m2 src1, __ubuf__ fp8_e5m2_t* base, int32_t offset, Literal dist, vector_bool mask,
    Literal mode);
void vsts(
    vector_f8e8m0 src0, vector_f8e8m0 src1, __ubuf__ fp8_e8m0_t* base, int32_t offset, Literal dist, vector_bool mask,
    Literal mode);
void vsts(
    vector_f4e2m1x2 src0, vector_f4e2m1x2 src1, __ubuf__ fp4x2_e2m1_t* base, int32_t offset, Literal dist,
    vector_bool mask, Literal mode);
void vsts(
    vector_f4e1m2x2 src0, vector_f4e1m2x2 src1, __ubuf__ fp4x2_e1m2_t* base, int32_t offset, Literal dist,
    vector_bool mask, Literal mode);

using float8_e4m3_t = fp8_e4m3fn_t;
using float8_e5m2_t = fp8_e5m2_t;
using float4_e1m2x2_t = fp4x2_e1m2_t;
using float4_e2m1x2_t = fp4x2_e2m1_t;
void vcgadd(vector_u32& dst, vector_u16 src, vector_bool pg, int32_t mode);
void vcgadd(vector_s32& dst, vector_s16 src, vector_bool pg, int32_t mode);

// ==========copy_matrix_cc_to_cbuf (25 params)===========
inline void copy_matrix_cc_to_cbuf(
    __cbuf__ half* dst_addr, __cc__ float* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool NZ2ND_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool C0_pad_en, bool wino_post_en, bool broadcast_en, bool NZ2DN_en)
{}
inline void copy_matrix_cc_to_cbuf(
    __cbuf__ bfloat16_t* dst_addr, __cc__ float* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool NZ2ND_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool C0_pad_en, bool wino_post_en, bool broadcast_en, bool NZ2DN_en)
{}
inline void copy_matrix_cc_to_cbuf(
    __cbuf__ int8_t* dst_addr, __cc__ float* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool NZ2ND_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool C0_pad_en, bool wino_post_en, bool broadcast_en, bool NZ2DN_en)
{}
inline void copy_matrix_cc_to_cbuf(
    __cbuf__ uint8_t* dst_addr, __cc__ float* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool NZ2ND_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool C0_pad_en, bool wino_post_en, bool broadcast_en, bool NZ2DN_en)
{}
inline void copy_matrix_cc_to_cbuf(
    __cbuf__ float* dst_addr, __cc__ float* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool NZ2ND_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool C0_pad_en, bool wino_post_en, bool broadcast_en, bool NZ2DN_en)
{}
inline void copy_matrix_cc_to_cbuf(
    __cbuf__ half* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool NZ2ND_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool C0_pad_en, bool wino_post_en, bool broadcast_en, bool NZ2DN_en)
{}
inline void copy_matrix_cc_to_cbuf(
    __cbuf__ int8_t* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool NZ2ND_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool C0_pad_en, bool wino_post_en, bool broadcast_en, bool NZ2DN_en)
{}
inline void copy_matrix_cc_to_cbuf(
    __cbuf__ uint8_t* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool NZ2ND_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool C0_pad_en, bool wino_post_en, bool broadcast_en, bool NZ2DN_en)
{}
inline void copy_matrix_cc_to_cbuf(
    __cbuf__ int32_t* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool NZ2ND_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool C0_pad_en, bool wino_post_en, bool broadcast_en, bool NZ2DN_en)
{}

// ==========copy_matrix_cc_to_cbuf_s4===========
inline void copy_matrix_cc_to_cbuf_s4(
    __cbuf__ void* dst_addr, __cc__ float* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_cbuf_s4(
    __cbuf__ void* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

// ==========copy_matrix_cc_to_gm===========
inline void copy_matrix_cc_to_gm(
    __gm__ bfloat16_t* dst_addr, __cc__ float* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_gm(
    __gm__ float8_e4m3_t* dst_addr, __cc__ float* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_gm(
    __gm__ float8_e5m2_t* dst_addr, __cc__ float* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_gm(
    __gm__ hifloat8_t* dst_addr, __cc__ float* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_gm(
    __gm__ int8_t* dst_addr, __cc__ float* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_gm(
    __gm__ uint8_t* dst_addr, __cc__ float* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_gm(
    __gm__ bfloat16_t* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_gm(
    __gm__ half* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_gm(
    __gm__ float8_e4m3_t* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_gm(
    __gm__ float8_e5m2_t* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_gm(
    __gm__ hifloat8_t* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_gm(
    __gm__ uint8_t* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_gm(
    __gm__ int32_t* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_gm_s4(
    __gm__ void* dst_addr, __cc__ float* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_gm_s4(
    __gm__ void* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

// ==========copy_matrix_cc_to_ub===========
inline void copy_matrix_cc_to_ub(
    __ubuf__ bfloat16_t* dst_addr, __cc__ float* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_ub(
    __ubuf__ half* dst_addr, __cc__ float* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_ub(
    __ubuf__ float8_e4m3_t* dst_addr, __cc__ float* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_ub(
    __ubuf__ float8_e5m2_t* dst_addr, __cc__ float* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_ub(
    __ubuf__ hifloat8_t* dst_addr, __cc__ float* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_ub(
    __ubuf__ int8_t* dst_addr, __cc__ float* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_ub(
    __ubuf__ uint8_t* dst_addr, __cc__ float* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_ub(
    __ubuf__ float* dst_addr, __cc__ float* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{
    if (is_mock_copy_matrix_cc_to_ub) {
        EXPECT_EQ(n_size, n_size_global);
        EXPECT_EQ(m_size, m_size_global);
        EXPECT_EQ(loop_dst_stride, dst_stride_global);
        EXPECT_EQ(loop_src_stride, src_stride_global);
        EXPECT_EQ(nz2nd_en, NZ2ND_en_global);
        EXPECT_EQ(nz2dn_en, NZ2DN_en_global);
        if (ub_addr_global != nullptr) {
            EXPECT_EQ(reinterpret_cast<void*>(dst_addr), ub_addr_global);
        }
        EXPECT_EQ(quant_pre, quant_pre_global);
    }
}

inline void copy_matrix_cc_to_ub(
    __ubuf__ bfloat16_t* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_ub(
    __ubuf__ half* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_ub(
    __ubuf__ float8_e4m3_t* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_ub(
    __ubuf__ float8_e5m2_t* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_ub(
    __ubuf__ hifloat8_t* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_ub(
    __ubuf__ int8_t* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_ub(
    __ubuf__ uint8_t* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_ub(
    __ubuf__ int32_t* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_ub_s4(
    __ubuf__ void* dst_addr, __cc__ float* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_ub_s4(
    __ubuf__ void* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

// ==========load_gm_to_cbuf_2dv2===========
inline void load_gm_to_cbuf_2dv2(
    __cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, uint8_t l2_cache_ctl)
{}

inline void load_gm_to_cbuf_2dv2(
    __cbuf__ float* dst, __gm__ float* src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride,
    uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, uint8_t l2_cache_ctl)
{}

inline void load_gm_to_cbuf_2dv2(
    __cbuf__ float8_e4m3_t* dst, __gm__ float8_e4m3_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, uint8_t l2_cache_ctl)
{}

inline void load_gm_to_cbuf_2dv2(
    __cbuf__ float8_e5m2_t* dst, __gm__ float8_e5m2_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, uint8_t l2_cache_ctl)
{}

inline void load_gm_to_cbuf_2dv2(
    __cbuf__ half* dst, __gm__ half* src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride,
    uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, uint8_t l2_cache_ctl)
{}

inline void load_gm_to_cbuf_2dv2(
    __cbuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, uint8_t l2_cache_ctl)
{}

inline void load_gm_to_cbuf_2dv2(
    __cbuf__ int16_t* dst, __gm__ int16_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, uint8_t l2_cache_ctl)
{}

inline void load_gm_to_cbuf_2dv2(
    __cbuf__ int32_t* dst, __gm__ int32_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, uint8_t l2_cache_ctl)
{}

inline void load_gm_to_cbuf_2dv2(
    __cbuf__ int8_t* dst, __gm__ int8_t* src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride,
    uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, uint8_t l2_cache_ctl)
{}

inline void load_gm_to_cbuf_2dv2(
    __cbuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, uint8_t l2_cache_ctl)
{}

inline void load_gm_to_cbuf_2dv2(
    __cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, uint8_t l2_cache_ctl)
{}

inline void load_gm_to_cbuf_2dv2(
    __cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, uint8_t l2_cache_ctl)
{}

// ==========load_gm_to_cbuf_2dv2_s4===========
inline void load_gm_to_cbuf_2dv2_s4(
    __cbuf__ float4_e1m2x2_t* dst, __gm__ float4_e1m2x2_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, uint8_t l2_cache_ctl)
{}

inline void load_gm_to_cbuf_2dv2_s4(
    __cbuf__ float4_e2m1x2_t* dst, __gm__ float4_e2m1x2_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, uint8_t l2_cache_ctl)
{}

inline void load_gm_to_cbuf_2dv2_s4(
    __cbuf__ void* dst, __gm__ void* src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride,
    uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, uint8_t l2_cache_ctl)
{}

// ==========copy_cbuf_to_ubuf===========
inline void copy_cbuf_to_ubuf(
    __ubuf__ void* dst_addr, __cbuf__ void* src_addr, bool sub_blockid, uint16_t n_burst, uint16_t len_burst,
    uint16_t src_gap, uint16_t dst_gap)
{}

inline void create_cbuf_matrix_h(__cbuf__ bfloat16_t* dst, int64_t repeat, half value) {}

inline void create_cbuf_matrix_ui(__cbuf__ bfloat16_t* dst, int64_t repeat, uint32_t value) {}

inline void vmrgsort4(
    __ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t regionProposalLi0, uint16_t regionProposalLi1,
    uint16_t regionProposalLi2, uint16_t regionProposalLi3, bool isAllStored, uint8_t maskSignal)
{}

inline void vmrgsort4(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t regionProposalLi0, uint16_t regionProposalLi1,
    uint16_t regionProposalLi2, uint16_t regionProposalLi3, bool isAllStored, uint8_t maskSignal)
{}

inline void vbs(
    __ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ uint32_t* src1, uint8_t repeat, uint8_t dstBlockStride,
    uint8_t src0BlockStride, uint8_t src1BlockStride, uint8_t dstRepeatStride, uint8_t src0RepeatStride,
    uint8_t src1RepeatStride, bool repeatStrideMode, bool strideSizeMode)
{}

inline void vbs(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ uint32_t* src1, uint8_t repeat, uint8_t dstBlockStride,
    uint8_t src0BlockStride, uint8_t src1BlockStride, uint8_t dstRepeatStride, uint8_t src0RepeatStride,
    uint8_t src1RepeatStride, bool repeatStrideMode, bool strideSizeMode)
{}

inline void get_buf(pipe_t pipe, uint64_t buf_id, bool mode) {}

inline void create_ca_matrix_h(__ca__ bfloat16_t* dst, int64_t repeat, half value) {}
inline void create_ca_matrix_ui(__ca__ bfloat16_t* dst, int64_t repeat, uint32_t value) {}

inline void create_cb_matrix_h(__cb__ bfloat16_t* dst, int64_t repeat, half value) {}
inline void create_cb_matrix_ui(__cb__ bfloat16_t* dst, int64_t repeat, uint32_t value) {}

inline void vgather2_bc(vector_s16& dst, __ubuf__ int16_t* src, vector_u32 index, vector_bool mask) {}
inline void vgather2_bc(vector_u16& dst, __ubuf__ uint16_t* src, vector_u32 index, vector_bool mask) {}
inline void vgather2_bc(vector_f16& dst, __ubuf__ half* src, vector_u32 index, vector_bool mask) {}
inline void vgather2_bc(vector_bf16& dst, __ubuf__ bfloat16_t* src, vector_u32 index, vector_bool mask) {}

inline void vbr(vector_f8e4m3& dst, fp8_e4m3fn_t value) {}
inline void vbr(vector_f8e5m2& dst, fp8_e5m2_t value) {}
inline void vbr(vector_f8e8m0& dst, fp8_e8m0_t value) {}
inline void vdup(vector_f8e8m0& dst, fp8_e8m0_t value, vector_bool mask, Literal mode) {}
inline void vdup(vector_f8e8m0& dst, vector_f8e8m0 src, vector_bool mask, int32_t type, Literal mode) {}

inline void copy_gm_to_cbuf_v2(
    __cbuf__ void* dst, __gm__ void* src, uint8_t sid, uint32_t n_burst, uint32_t len_burst, uint8_t pad_func_mode,
    uint8_t l2_cache_ctl, uint64_t src_stride, uint32_t dst_stride)
{}
inline void img2colv2_cbuf_to_ca(
    __ca__ int16_t* dst, __cbuf__ int16_t* src, uint16_t step_k, uint16_t step_m, uint16_t pos_k, uint16_t pos_m,
    uint8_t stride_w, uint8_t stride_h, uint8_t w_k, uint8_t h_k, uint8_t dilation_w, uint8_t dilation_h, bool filter_w,
    bool filter_h, bool transpose, bool fmatrix_ctrl, uint16_t size_channel)
{}
inline void img2colv2_cbuf_to_ca(
    __ca__ uint16_t* dst, __cbuf__ uint16_t* src, uint16_t step_k, uint16_t step_m, uint16_t pos_k, uint16_t pos_m,
    uint8_t stride_w, uint8_t stride_h, uint8_t w_k, uint8_t h_k, uint8_t dilation_w, uint8_t dilation_h, bool filter_w,
    bool filter_h, bool transpose, bool fmatrix_ctrl, uint16_t size_channel)
{}
inline void img2colv2_cbuf_to_cb(
    __cb__ int16_t* dst, __cbuf__ int16_t* src, uint16_t step_k, uint16_t step_m, uint16_t pos_k, uint16_t pos_m,
    uint8_t stride_w, uint8_t stride_h, uint8_t w_k, uint8_t h_k, uint8_t dilation_w, uint8_t dilation_h, bool filter_w,
    bool filter_h, bool transpose, bool fmatrix_ctrl, uint16_t size_channel)
{}
inline void img2colv2_cbuf_to_cb(
    __cb__ uint16_t* dst, __cbuf__ uint16_t* src, uint16_t step_k, uint16_t step_m, uint16_t pos_k, uint16_t pos_m,
    uint8_t stride_w, uint8_t stride_h, uint8_t w_k, uint8_t h_k, uint8_t dilation_w, uint8_t dilation_h, bool filter_w,
    bool filter_h, bool transpose, bool fmatrix_ctrl, uint16_t size_channel)
{}
inline void wait_flag_dev(pipe_t pipe, uint8_t flag_id) {}
inline void wait_intra_block(pipe_t pipe, uint8_t flag_id) {}
inline void set_intra_block(pipe_t pipe, uint8_t sync_id) {}
inline void wait_intra_block(pipe_t pipe, uint16_t flag_id) {}
inline void set_intra_block(pipe_t pipe, uint16_t sync_id) {}
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
inline void wait_intra_block(pipe_t pipe, int flag_id) {}
inline void set_intra_block(pipe_t pipe, int sync_id) {}
#endif
inline void rls_buf(pipe_t pipe, uint64_t buf_id, bool mode) {}

inline void psts(vector_bool src, __ubuf__ uint32_t*& base, int32_t offset, Literal dist, Literal post) {}

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
inline int64_t get_iqent(pipe_t pipe) { return 0; }
#endif

#if defined(__DAV_VEC__) && defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "c_api/defs/type.h"
inline void pstu(vector_store_unalign& ureg, vector_bool mask, __ubuf__ uint16_t*& dst) {}
inline void pstu(vector_store_unalign& ureg, vector_bool mask, __ubuf__ uint32_t*& dst) {}
#endif

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
// asc_copy_gm2ub_impl uses void*; stub_fun.h void* overload lacks l2_cache_ctl param.
inline void copy_gm_to_ubuf_align_v2(
    __ubuf__ void* dst_addr, __gm__ void* src_addr, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
    uint8_t left_padding_count, uint8_t right_padding_count, bool constant_padding_ctl, uint8_t l2_cache_ctl,
    uint64_t burst_src_stride, uint32_t burst_dst_stride)
{}
// asc_copy_ub2gm_impl uses void*; stub_fun.h void* overload lacks l2_cache_mode param.
inline void copy_ubuf_to_gm_align_v2(
    __gm__ void* dst_addr, __ubuf__ void* src_addr, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
    uint8_t l2_cache_mode, uint64_t burst_dst_stride, uint32_t burst_src_stride)
{}
#endif

// ==========vstar===========
inline void vstar(vector_align data, __ubuf__ fp8_e4m3fn_t* base) {}
inline void vstar(vector_align data, __ubuf__ fp8_e5m2_t* base) {}
inline void vstar(vector_align data, __ubuf__ fp8_e8m0_t* base) {}
inline void vstar(vector_align data, __ubuf__ fp4x2_e2m1_t* base) {}
inline void vstar(vector_align data, __ubuf__ fp4x2_e1m2_t* base) {}

// ==========vstur===========
inline void vstur(vector_align& alignData, vector_s64 src, __ubuf__ int64_t* base, Literal post) {}
inline void vstur(vector_align& alignData, vector_f8e4m3 src, __ubuf__ fp8_e4m3fn_t* base, Literal post) {}
inline void vstur(vector_align& alignData, vector_f8e5m2 src, __ubuf__ fp8_e5m2_t* base, Literal post) {}
inline void vstur(vector_align& alignData, vector_f8e8m0 src, __ubuf__ fp8_e8m0_t* base, Literal post) {}
inline void vstur(vector_align& alignData, vector_f4e2m1x2 src, __ubuf__ fp4x2_e2m1_t* base, Literal post) {}
inline void vstur(vector_align& alignData, vector_f4e1m2x2 src, __ubuf__ fp4x2_e1m2_t* base, Literal post) {}

inline void nd_dma_dci() {}

inline void load_cbuf_to_cb_transpose(
    float8_e4m3_t* dst, __cbuf__ float8_e4m3_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_stride, bool addrmode, uint16_t dst_frac_stride, uint16_t src_frac_stride)
{}
inline void load_cbuf_to_cb_transpose(
    float8_e5m2_t* dst, __cbuf__ float8_e5m2_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_stride, bool addrmode, uint16_t dst_frac_stride, uint16_t src_frac_stride)
{}
inline void load_cbuf_to_cb_transpose(
    hifloat8_t* dst, __cbuf__ hifloat8_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_stride, bool addrmode, uint16_t dst_frac_stride, uint16_t src_frac_stride)
{}
inline void load_cbuf_to_cb_transpose_s4(
    float4_e1m2x2_t* dst, __cbuf__ float4_e1m2x2_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_stride, bool addrmode, uint16_t dst_frac_stride, uint16_t src_frac_stride)
{}
inline void load_cbuf_to_cb_transpose_s4(
    float4_e2m1x2_t* dst, __cbuf__ float4_e2m1x2_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_stride, bool addrmode, uint16_t dst_frac_stride, uint16_t src_frac_stride)
{}

#if defined(__DAV_CUBE__)
inline int32_t g_coreType = 1;
#else
inline int32_t g_coreType = 2;
#endif

typedef std::integral_constant<Pos, Pos::LOWEST> Lowest_Type;
typedef std::integral_constant<Pos, Pos::HIGHEST> Highest_Type;
constexpr Lowest_Type POS_LOWEST = Lowest_Type();
constexpr Highest_Type POS_HIGHEST = Highest_Type();

#endif
