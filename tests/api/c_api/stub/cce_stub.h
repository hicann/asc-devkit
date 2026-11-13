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
#include "c_api/defs/enum.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 9201)
struct E6m2VectorDatamoveStubState {
    uint32_t load_count = 0;
    uint32_t store_count = 0;
    uint32_t scatter_count = 0;
};

inline E6m2VectorDatamoveStubState g_e6m2_vector_datamove_stub_state;

inline void reset_e6m2_vector_datamove_stub_state() { g_e6m2_vector_datamove_stub_state = {}; }

inline void vlds(vector_f8e6m2& dst, __ubuf__ fp8_e6m2_t* src, int32_t offset, Literal mode)
{
    ++g_e6m2_vector_datamove_stub_state.load_count;
}

inline void vlds(vector_f8e6m2& dst0, vector_f8e6m2& dst1, __ubuf__ fp8_e6m2_t* src, int32_t offset, Literal mode)
{
    ++g_e6m2_vector_datamove_stub_state.load_count;
}

inline void vld(vector_f8e6m2& dst, __ubuf__ fp8_e6m2_t* src, vector_address offset, Literal mode)
{
    ++g_e6m2_vector_datamove_stub_state.load_count;
}

inline void vld(vector_f8e6m2& dst0, vector_f8e6m2& dst1, __ubuf__ fp8_e6m2_t* src, vector_address offset, Literal mode)
{
    ++g_e6m2_vector_datamove_stub_state.load_count;
}

inline void vsldb(vector_f8e6m2& dst, __ubuf__ fp8_e6m2_t* src, int32_t config, vector_bool mask)
{
    ++g_e6m2_vector_datamove_stub_state.load_count;
}

inline void vsldb(vector_f8e6m2& dst, __ubuf__ fp8_e6m2_t*& src, int32_t config, vector_bool mask, Literal post)
{
    ++g_e6m2_vector_datamove_stub_state.load_count;
}

inline void vlds(vector_f8e6m2& dst, __ubuf__ fp8_e6m2_t*& src, int32_t offset, Literal mode, Literal post)
{
    ++g_e6m2_vector_datamove_stub_state.load_count;
}

inline void vlds(
    vector_f8e6m2& dst0, vector_f8e6m2& dst1, __ubuf__ fp8_e6m2_t*& src, int32_t offset, Literal mode, Literal post)
{
    ++g_e6m2_vector_datamove_stub_state.load_count;
}

inline void vldus(vector_f8e6m2& dst, vector_align& align, __ubuf__ fp8_e6m2_t* src)
{
    ++g_e6m2_vector_datamove_stub_state.load_count;
}

inline void vldus(vector_f8e6m2& dst, vector_align& align, __ubuf__ fp8_e6m2_t*& src, uint32_t count, Literal post)
{
    ++g_e6m2_vector_datamove_stub_state.load_count;
}

inline void vldu(
    vector_f8e6m2& dst, vector_align& align, vector_address& offset, __ubuf__ fp8_e6m2_t* src, uint32_t count)
{
    ++g_e6m2_vector_datamove_stub_state.load_count;
}

inline void vldas(vector_align& dst, __ubuf__ fp8_e6m2_t* src) { ++g_e6m2_vector_datamove_stub_state.load_count; }

inline void vlda(vector_align& dst, __ubuf__ fp8_e6m2_t* src, vector_address offset)
{
    ++g_e6m2_vector_datamove_stub_state.load_count;
}

inline void vsts(vector_f8e6m2 src, __ubuf__ fp8_e6m2_t* dst, int32_t offset, Literal mode, vector_bool mask)
{
    ++g_e6m2_vector_datamove_stub_state.store_count;
}

inline void vsts(
    vector_f8e6m2 src, __ubuf__ fp8_e6m2_t*& dst, int32_t offset, Literal mode, vector_bool mask, Literal post)
{
    ++g_e6m2_vector_datamove_stub_state.store_count;
}

inline void vsts(
    vector_f8e6m2 src0, vector_f8e6m2 src1, __ubuf__ fp8_e6m2_t* dst, int32_t offset, Literal mode, vector_bool mask)
{
    ++g_e6m2_vector_datamove_stub_state.store_count;
}

inline void vst(vector_f8e6m2 src, __ubuf__ fp8_e6m2_t* dst, vector_address offset, Literal mode, vector_bool mask)
{
    ++g_e6m2_vector_datamove_stub_state.store_count;
}

inline void vst(
    vector_f8e6m2 src0, vector_f8e6m2 src1, __ubuf__ fp8_e6m2_t* dst, vector_address offset, Literal mode,
    vector_bool mask)
{
    ++g_e6m2_vector_datamove_stub_state.store_count;
}

inline void vsstb(vector_f8e6m2 src, __ubuf__ fp8_e6m2_t* dst, int32_t config, vector_bool mask)
{
    ++g_e6m2_vector_datamove_stub_state.store_count;
}

inline void vsstb(vector_f8e6m2 src, __ubuf__ fp8_e6m2_t*& dst, int32_t config, vector_bool mask, Literal post)
{
    ++g_e6m2_vector_datamove_stub_state.store_count;
}

inline void vstus(vector_align& align, uint32_t count, vector_f8e6m2 src, __ubuf__ fp8_e6m2_t* dst)
{
    ++g_e6m2_vector_datamove_stub_state.store_count;
}

inline void vstus(vector_align& align, uint32_t count, vector_f8e6m2 src, __ubuf__ fp8_e6m2_t*& dst, Literal post)
{
    ++g_e6m2_vector_datamove_stub_state.store_count;
}

inline void vstu(vector_align& align, vector_address& count, vector_f8e6m2 src, __ubuf__ fp8_e6m2_t* dst, Literal post)
{
    ++g_e6m2_vector_datamove_stub_state.store_count;
}

inline void vstas(vector_align align, __ubuf__ fp8_e6m2_t* dst, int32_t offset)
{
    ++g_e6m2_vector_datamove_stub_state.store_count;
}

inline void vstas(vector_align align, __ubuf__ fp8_e6m2_t*& dst, int32_t offset, Literal post)
{
    ++g_e6m2_vector_datamove_stub_state.store_count;
}

inline void vsta(vector_align align, __ubuf__ fp8_e6m2_t* dst, vector_address offset)
{
    ++g_e6m2_vector_datamove_stub_state.store_count;
}

inline void vscatter(vector_f8e6m2 src, __ubuf__ fp8_e6m2_t* dst, vector_u16 index, vector_bool mask)
{
    ++g_e6m2_vector_datamove_stub_state.scatter_count;
}

#endif

using hif4x2_t = hifloat4x2_t;

inline void vcvt(
    vector_f8e4m3& dst, vector_bf16 src, vector_bool preg, Literal tag0, Literal tag1, Literal tag2, Literal tag3)
{}
inline void vcvt(
    vector_f8e5m2& dst, vector_bf16 src, vector_bool preg, Literal tag0, Literal tag1, Literal tag2, Literal tag3)
{}
inline void vcvt_bf162e6m2(
    vector_f8e6m2& dst, vector_bf16 src, vector_bool preg, Literal tag0, Literal tag1, Literal tag2)
{}
inline void vcvt_bf162e8m0(
    vector_f8e8m0& dst, vector_bf16 src, vector_bool preg, Literal tag0, Literal tag1, Literal tag2, Literal tag3)
{}

inline void mad(
    __cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t m, uint16_t k, uint16_t n,
    uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{}

inline void mad(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t m, uint16_t k,
    uint16_t n, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{}

inline void mad(
    __cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ int8_t* b_matrix, uint16_t m, uint16_t k, uint16_t n,
    uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{}

inline void mad(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ int8_t* b_matrix, uint16_t m, uint16_t k, uint16_t n,
    uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{}

inline void mad_s8s4(
    __cc__ int32_t* c_matrix, __ca__ int8_t* a_matrix, __cb__ void* b_matrix, uint16_t m, uint16_t k, uint16_t n,
    uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{}

inline void mad_f16s4(
    __cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ void* b_matrix, uint16_t m, uint16_t k, uint16_t n,
    uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{}

inline void mad_bf16s4(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ void* b_matrix, uint16_t m, uint16_t k, uint16_t n,
    uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{}

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

inline void mad_mx(
    __cc__ float* c_matrix, uint64_t bias, __ca__ fp4x2_e1m2_t* a_matrix, uint64_t scale_a_addr,
    __cb__ fp4x2_e1m2_t* b_matrix, uint64_t scale_b_addr, uint16_t m, uint16_t k, uint16_t n, uint8_t unit_flag,
    bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{}

inline void mad_mx(
    __cc__ float* c_matrix, uint64_t bias, __ca__ fp4x2_e1m2_t* a_matrix, uint64_t scale_a_addr,
    __cb__ fp4x2_e2m1_t* b_matrix, uint64_t scale_b_addr, uint16_t m, uint16_t k, uint16_t n, uint8_t unit_flag,
    bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{}

inline void mad_mx(
    __cc__ float* c_matrix, uint64_t bias, __ca__ fp4x2_e2m1_t* a_matrix, uint64_t scale_a_addr,
    __cb__ fp4x2_e1m2_t* b_matrix, uint64_t scale_b_addr, uint16_t m, uint16_t k, uint16_t n, uint8_t unit_flag,
    bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{}

inline void mad_mx(
    __cc__ float* c_matrix, uint64_t bias, __ca__ fp4x2_e2m1_t* a_matrix, uint64_t scale_a_addr,
    __cb__ fp4x2_e2m1_t* b_matrix, uint64_t scale_b_addr, uint16_t m, uint16_t k, uint16_t n, uint8_t unit_flag,
    bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{}

inline void mad_mx(
    __cc__ float* c_matrix, uint64_t bias, __ca__ fp8_e4m3fn_t* a_matrix, uint64_t scale_a_addr,
    __cb__ fp8_e4m3fn_t* b_matrix, uint64_t scale_b_addr, uint16_t m, uint16_t k, uint16_t n, uint8_t unit_flag,
    bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{}

inline void mad_mx(
    __cc__ float* c_matrix, uint64_t bias, __ca__ fp8_e4m3fn_t* a_matrix, uint64_t scale_a_addr,
    __cb__ fp8_e5m2_t* b_matrix, uint64_t scale_b_addr, uint16_t m, uint16_t k, uint16_t n, uint8_t unit_flag,
    bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{}

inline void mad_mx(
    __cc__ float* c_matrix, uint64_t bias, __ca__ fp8_e5m2_t* a_matrix, uint64_t scale_a_addr,
    __cb__ fp8_e4m3fn_t* b_matrix, uint64_t scale_b_addr, uint16_t m, uint16_t k, uint16_t n, uint8_t unit_flag,
    bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{}

inline void mad_mx(
    __cc__ float* c_matrix, uint64_t bias, __ca__ hif4x2_t* a_matrix, uint64_t scale_a_addr, __cb__ hif4x2_t* b_matrix,
    uint64_t scale_b_addr, uint16_t m, uint16_t k, uint16_t n, uint8_t unit_flag, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val)
{}

inline void mad_mx(
    __cc__ float* c_matrix, uint64_t bias, __ca__ fp8_e4m3fn_t* a_matrix, uint64_t scale_a_addr,
    __cb__ hif4x2_t* b_matrix, uint64_t scale_b_addr, uint16_t m, uint16_t k, uint16_t n, uint8_t unit_flag,
    bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{}

inline void mad_mx(
    __cc__ float* c_matrix, uint64_t bias, __ca__ fp8_e4m3fn_t* a_matrix, uint64_t scale_a_addr,
    __cb__ fp4x2_e2m1_t* b_matrix, uint64_t scale_b_addr, uint16_t m, uint16_t k, uint16_t n, uint8_t unit_flag,
    bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{}

inline void mad_mx(
    __cc__ float* c_matrix, uint64_t bias, __ca__ half* a_matrix, uint64_t scale_a_addr, __cb__ fp4x2_e2m1_t* b_matrix,
    uint64_t scale_b_addr, uint16_t m, uint16_t k, uint16_t n, uint8_t unit_flag, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val)
{}

inline void mad_mx(
    __cc__ float* c_matrix, uint64_t bias, __ca__ half* a_matrix, uint64_t scale_a_addr, __cb__ hif4x2_t* b_matrix,
    uint64_t scale_b_addr, uint16_t m, uint16_t k, uint16_t n, uint8_t unit_flag, bool disable_gemv,
    bool c_matrix_source, bool c_matrix_init_val)
{}

inline void mad_mx(
    __cc__ float* c_matrix, uint64_t bias, __ca__ bfloat16_t* a_matrix, uint64_t scale_a_addr,
    __cb__ fp4x2_e2m1_t* b_matrix, uint64_t scale_b_addr, uint16_t m, uint16_t k, uint16_t n, uint8_t unit_flag,
    bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{}

inline void mad_mx(
    __cc__ float* c_matrix, uint64_t bias, __ca__ bfloat16_t* a_matrix, uint64_t scale_a_addr,
    __cb__ hif4x2_t* b_matrix, uint64_t scale_b_addr, uint16_t m, uint16_t k, uint16_t n, uint8_t unit_flag,
    bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{}

inline void mad_mx(
    __cc__ float* c_matrix, uint64_t bias, __ca__ fp8_e5m2_t* a_matrix, uint64_t scale_a_addr,
    __cb__ fp8_e5m2_t* b_matrix, uint64_t scale_b_addr, uint16_t m, uint16_t k, uint16_t n, uint8_t unit_flag,
    bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{}

// The compiler supports this overload, but it is missing from the CPU debug stub header.
inline void vmadd(vector_bf16& dst, vector_bf16 src0, vector_bf16 src1, vector_bool mask, Literal mode) {}

// The compiler supports this instruction, but it is missing from the CPU debug stub header.
inline void vcvt_rcpe6m22bf16(vector_bf16& dst, vector_f8e6m2 src, vector_bool mask, Literal part) {}

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
inline uint32_t g_vsts_fp8_e8m0_postupdate_calls = 0;
inline void vsts(
    vector_f8e8m0 data, __ubuf__ fp8_e8m0_t* base, int32_t offset, Literal dist, vector_bool mask, Literal mode)
{
    ++g_vsts_fp8_e8m0_postupdate_calls;
}

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

inline void vcvt_e6m22bf16(vector_bf16& dst, vector_f8e6m2 src, vector_bool mask, Literal part) {}
inline void vcvt_e8m02bf16(vector_bf16& dst, vector_f8e8m0 src, vector_bool mask, Literal part) {}

inline void vcvt(
    vector_f8e4m3& dst, vector_f16 src, vector_bool mask, Literal round, Literal sat, Literal part, Literal mode)
{}
inline void vcvt(
    vector_f8e5m2& dst, vector_f16 src, vector_bool mask, Literal round, Literal sat, Literal part, Literal mode)
{}

inline void vpack(vector_u32& dst, vector_u64 src, Literal part) {}
inline void vpack(vector_u32& dst, vector_s64 src, Literal part) {}
inline void vpackv2(vector_u16& dst, vector_u16 src) {}
inline void vpackv2(vector_s16& dst, vector_s16 src) {}
inline void vpackv2(vector_f16& dst, vector_f16 src) {}
inline void vpackv2(vector_u32& dst, vector_u32 src) {}
inline void vpackv2(vector_s32& dst, vector_s32 src) {}
inline void vpackv2(vector_f32& dst, vector_f32 src) {}
inline void vpackv2(vector_u64& dst, vector_u64 src) {}
inline void vpackv2(vector_s64& dst, vector_s64 src) {}

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
inline void copy_matrix_cc_to_cbuf(
    __cbuf__ bfloat16_t* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool NZ2ND_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool C0_pad_en, bool wino_post_en, bool broadcast_en, bool NZ2DN_en)
{}
inline void copy_matrix_cc_to_cbuf(
    __cbuf__ hifloat8_t* dst_addr, __cc__ float* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool NZ2ND_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool C0_pad_en, bool wino_post_en, bool broadcast_en, bool NZ2DN_en)
{}
inline void copy_matrix_cc_to_cbuf(
    __cbuf__ fp8_e4m3fn_t* dst_addr, __cc__ float* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
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

// npu_arch_9201: no sid, bool quant_pre_rnd
inline void copy_matrix_cc_to_cbuf_s4(
    __cbuf__ void* dst_addr, __cc__ float* src_addr, uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride,
    uint16_t loop_src_stride, bool quant_pre_rnd, uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre,
    uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post,
    bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en, bool loop_enhance_merge_en, bool c0_pad_en,
    bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_cbuf_s4(
    __cbuf__ void* dst_addr, __cc__ int32_t* src_addr, uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride,
    uint16_t loop_src_stride, bool quant_pre_rnd, uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre,
    uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post,
    bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en, bool loop_enhance_merge_en, bool c0_pad_en,
    bool wino_post_en, bool broadcast_en, bool nz2dn_en)
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
    __gm__ float8_e5m2_t* dst_addr, __cc__ float* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, bool quant_pre_rnd, uint8_t clip_relu_pre,
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
    __gm__ float8_e4m3_t* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, bool quant_pre_rnd, uint8_t clip_relu_pre,
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
    __gm__ float8_e5m2_t* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, bool quant_pre_rnd, uint8_t clip_relu_pre,
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
    __gm__ hifloat8_t* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, bool quant_pre_rnd, uint8_t clip_relu_pre,
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
    __gm__ void* dst_addr, __cc__ float* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, bool quant_pre_rnd, uint8_t clip_relu_pre,
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

inline void copy_matrix_cc_to_gm_s4(
    __gm__ void* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t l2_cache_ctl, bool quant_pre_rnd, uint8_t clip_relu_pre,
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
    __ubuf__ bfloat16_t* dst_addr, __cc__ float* src_addr, uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride,
    uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, bool quant_pre_rnd, uint8_t clip_relu_pre,
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
    __ubuf__ half* dst_addr, __cc__ float* src_addr, uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride,
    uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, bool quant_pre_rnd, uint8_t clip_relu_pre,
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
    __ubuf__ float8_e4m3_t* dst_addr, __cc__ float* src_addr, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, bool quant_pre_rnd,
    uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en,
    uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op,
    bool eltwise_antq_en, bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en,
    bool nz2dn_en)
{}

inline void copy_matrix_cc_to_ub(
    __ubuf__ float8_e5m2_t* dst_addr, __cc__ float* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_ub(
    __ubuf__ float8_e5m2_t* dst_addr, __cc__ float* src_addr, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, bool quant_pre_rnd,
    uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en,
    uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op,
    bool eltwise_antq_en, bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en,
    bool nz2dn_en)
{}

inline void copy_matrix_cc_to_ub(
    __ubuf__ hifloat8_t* dst_addr, __cc__ float* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_ub(
    __ubuf__ hifloat8_t* dst_addr, __cc__ float* src_addr, uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride,
    uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, bool quant_pre_rnd, uint8_t clip_relu_pre,
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
    __ubuf__ int8_t* dst_addr, __cc__ float* src_addr, uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride,
    uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, bool quant_pre_rnd, uint8_t clip_relu_pre,
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
    __ubuf__ uint8_t* dst_addr, __cc__ float* src_addr, uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride,
    uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, bool quant_pre_rnd, uint8_t clip_relu_pre,
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
    __ubuf__ float* dst_addr, __cc__ float* src_addr, uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride,
    uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, bool quant_pre_rnd, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_ub(
    __ubuf__ bfloat16_t* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_ub(
    __ubuf__ bfloat16_t* dst_addr, __cc__ int32_t* src_addr, uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride,
    uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, bool quant_pre_rnd, uint8_t clip_relu_pre,
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
    __ubuf__ half* dst_addr, __cc__ int32_t* src_addr, uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride,
    uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, bool quant_pre_rnd, uint8_t clip_relu_pre,
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
    __ubuf__ float8_e4m3_t* dst_addr, __cc__ int32_t* src_addr, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, bool quant_pre_rnd,
    uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en,
    uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op,
    bool eltwise_antq_en, bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en,
    bool nz2dn_en)
{}

inline void copy_matrix_cc_to_ub(
    __ubuf__ float8_e5m2_t* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_ub(
    __ubuf__ float8_e5m2_t* dst_addr, __cc__ int32_t* src_addr, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, bool quant_pre_rnd,
    uint8_t clip_relu_pre, uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en,
    uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op,
    bool eltwise_antq_en, bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en,
    bool nz2dn_en)
{}

inline void copy_matrix_cc_to_ub(
    __ubuf__ hifloat8_t* dst_addr, __cc__ int32_t* src_addr, uint8_t sid, uint16_t n_size, uint16_t m_size,
    uint32_t loop_dst_stride, uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, uint8_t clip_relu_pre,
    uint8_t unit_flag_ctl, uint64_t quant_pre, uint8_t relu_pre, bool split_en, bool nz2nd_en, uint64_t quant_post,
    uint8_t relu_post, bool clip_relu_post, bool loop_enhance_en, uint8_t eltwise_op, bool eltwise_antq_en,
    bool loop_enhance_merge_en, bool c0_pad_en, bool wino_post_en, bool broadcast_en, bool nz2dn_en)
{}

inline void copy_matrix_cc_to_ub(
    __ubuf__ hifloat8_t* dst_addr, __cc__ int32_t* src_addr, uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride,
    uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, bool quant_pre_rnd, uint8_t clip_relu_pre,
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
    __ubuf__ int8_t* dst_addr, __cc__ int32_t* src_addr, uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride,
    uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, bool quant_pre_rnd, uint8_t clip_relu_pre,
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
    __ubuf__ uint8_t* dst_addr, __cc__ int32_t* src_addr, uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride,
    uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, bool quant_pre_rnd, uint8_t clip_relu_pre,
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

inline void copy_matrix_cc_to_ub(
    __ubuf__ int32_t* dst_addr, __cc__ int32_t* src_addr, uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride,
    uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, bool quant_pre_rnd, uint8_t clip_relu_pre,
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
    __ubuf__ void* dst_addr, __cc__ float* src_addr, uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride,
    uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, bool quant_pre_rnd, uint8_t clip_relu_pre,
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

inline void copy_matrix_cc_to_ub_s4(
    __ubuf__ void* dst_addr, __cc__ int32_t* src_addr, uint16_t n_size, uint16_t m_size, uint32_t loop_dst_stride,
    uint16_t loop_src_stride, uint8_t dual_dst_ctl, bool sub_blockid, bool quant_pre_rnd, uint8_t clip_relu_pre,
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
    __cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode,
    bool enable_pre_load_ignore_sync, uint8_t l2_cache_ctl)
{}

inline void load_gm_to_cbuf_2dv2(
    __cbuf__ float* dst, __gm__ float* src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride,
    uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, uint8_t l2_cache_ctl)
{}
inline void load_gm_to_cbuf_2dv2(
    __cbuf__ float* dst, __gm__ float* src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride,
    uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, bool enable_pre_load_ignore_sync,
    uint8_t l2_cache_ctl)
{}

inline void load_gm_to_cbuf_2dv2(
    __cbuf__ float8_e4m3_t* dst, __gm__ float8_e4m3_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, uint8_t l2_cache_ctl)
{}
inline void load_gm_to_cbuf_2dv2(
    __cbuf__ float8_e4m3_t* dst, __gm__ float8_e4m3_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode,
    bool enable_pre_load_ignore_sync, uint8_t l2_cache_ctl)
{}

inline void load_gm_to_cbuf_2dv2(
    __cbuf__ float8_e5m2_t* dst, __gm__ float8_e5m2_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, uint8_t l2_cache_ctl)
{}
inline void load_gm_to_cbuf_2dv2(
    __cbuf__ float8_e5m2_t* dst, __gm__ float8_e5m2_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode,
    bool enable_pre_load_ignore_sync, uint8_t l2_cache_ctl)
{}

inline void load_gm_to_cbuf_2dv2(
    __cbuf__ half* dst, __gm__ half* src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride,
    uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, uint8_t l2_cache_ctl)
{}
inline void load_gm_to_cbuf_2dv2(
    __cbuf__ half* dst, __gm__ half* src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride,
    uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, bool enable_pre_load_ignore_sync,
    uint8_t l2_cache_ctl)
{}

inline void load_gm_to_cbuf_2dv2(
    __cbuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, uint8_t l2_cache_ctl)
{}
inline void load_gm_to_cbuf_2dv2(
    __cbuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode,
    bool enable_pre_load_ignore_sync, uint8_t l2_cache_ctl)
{}

inline void load_gm_to_cbuf_2dv2(
    __cbuf__ int16_t* dst, __gm__ int16_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, uint8_t l2_cache_ctl)
{}
inline void load_gm_to_cbuf_2dv2(
    __cbuf__ int16_t* dst, __gm__ int16_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode,
    bool enable_pre_load_ignore_sync, uint8_t l2_cache_ctl)
{}

inline void load_gm_to_cbuf_2dv2(
    __cbuf__ int32_t* dst, __gm__ int32_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, uint8_t l2_cache_ctl)
{}
inline void load_gm_to_cbuf_2dv2(
    __cbuf__ int32_t* dst, __gm__ int32_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode,
    bool enable_pre_load_ignore_sync, uint8_t l2_cache_ctl)
{}

inline void load_gm_to_cbuf_2dv2(
    __cbuf__ int8_t* dst, __gm__ int8_t* src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride,
    uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, uint8_t l2_cache_ctl)
{}
inline void load_gm_to_cbuf_2dv2(
    __cbuf__ int8_t* dst, __gm__ int8_t* src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride,
    uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, bool enable_pre_load_ignore_sync,
    uint8_t l2_cache_ctl)
{}

inline void load_gm_to_cbuf_2dv2(
    __cbuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, uint8_t l2_cache_ctl)
{}
inline void load_gm_to_cbuf_2dv2(
    __cbuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode,
    bool enable_pre_load_ignore_sync, uint8_t l2_cache_ctl)
{}

inline void load_gm_to_cbuf_2dv2(
    __cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, uint8_t l2_cache_ctl)
{}
inline void load_gm_to_cbuf_2dv2(
    __cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode,
    bool enable_pre_load_ignore_sync, uint8_t l2_cache_ctl)
{}

inline void load_gm_to_cbuf_2dv2(
    __cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, uint8_t l2_cache_ctl)
{}
inline void load_gm_to_cbuf_2dv2(
    __cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode,
    bool enable_pre_load_ignore_sync, uint8_t l2_cache_ctl)
{}

// ==========load_gm_to_cbuf_2dv2_s4===========
inline void load_gm_to_cbuf_2dv2_s4(
    __cbuf__ float4_e1m2x2_t* dst, __gm__ float4_e1m2x2_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, uint8_t l2_cache_ctl)
{}
inline void load_gm_to_cbuf_2dv2_s4(
    __cbuf__ float4_e1m2x2_t* dst, __gm__ float4_e1m2x2_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode,
    bool enable_pre_load_ignore_sync, uint8_t l2_cache_ctl)
{}

inline void load_gm_to_cbuf_2dv2_s4(
    __cbuf__ float4_e2m1x2_t* dst, __gm__ float4_e2m1x2_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, uint8_t l2_cache_ctl)
{}
inline void load_gm_to_cbuf_2dv2_s4(
    __cbuf__ float4_e2m1x2_t* dst, __gm__ float4_e2m1x2_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode,
    bool enable_pre_load_ignore_sync, uint8_t l2_cache_ctl)
{}

inline void load_gm_to_cbuf_2dv2_s4(
    __cbuf__ void* dst, __gm__ void* src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride,
    uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, uint8_t l2_cache_ctl)
{}
inline void load_gm_to_cbuf_2dv2_s4(
    __cbuf__ void* dst, __gm__ void* src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride,
    uint16_t m_step, uint16_t k_step, uint8_t sid, uint8_t decomp_mode, bool enable_pre_load_ignore_sync,
    uint8_t l2_cache_ctl)
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
inline void copy_gm_to_cbuf_v2(
    __cbuf__ void* dst, __gm__ void* src, uint8_t sid, uint32_t n_burst, uint32_t len_burst, uint8_t pad_func_mode,
    uint8_t l2_cache_ctl, uint64_t src_stride, uint32_t dst_stride, bool enable_pre_load_ignore_sync)
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
struct CopyGmToUbufAlignV2StubState {
    uint32_t call_count = 0;
    bool pre_allocation = false;
    uint8_t l2_cache_mode = 0;
    uint64_t src_stride = 0;
    uint32_t dst_stride = 0;
    bool non_eod_ctrl = false;
};

inline CopyGmToUbufAlignV2StubState g_copy_gm_to_ubuf_align_v2_stub_state;

inline void reset_copy_gm_to_ubuf_align_v2_stub_state() { g_copy_gm_to_ubuf_align_v2_stub_state = {}; }

inline void record_copy_gm_to_ubuf_align_v2(
    bool pre_allocation, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride, bool non_eod_ctrl)
{
    ++g_copy_gm_to_ubuf_align_v2_stub_state.call_count;
    g_copy_gm_to_ubuf_align_v2_stub_state.pre_allocation = pre_allocation;
    g_copy_gm_to_ubuf_align_v2_stub_state.l2_cache_mode = l2_cache_mode;
    g_copy_gm_to_ubuf_align_v2_stub_state.src_stride = src_stride;
    g_copy_gm_to_ubuf_align_v2_stub_state.dst_stride = dst_stride;
    g_copy_gm_to_ubuf_align_v2_stub_state.non_eod_ctrl = non_eod_ctrl;
}

struct CopyUbufToGmAlignV2StubState {
    uint32_t call_count = 0;
    bool rsw_ctrl = false;
    bool rsw_packet_ctrl = false;
    uint8_t rsw_buffset_size = 0;
    uint8_t l2_cache_mode = 0;
    uint64_t dst_stride = 0;
    uint32_t src_stride = 0;
    bool non_eod_ctrl = false;
};

inline CopyUbufToGmAlignV2StubState g_copy_ubuf_to_gm_align_v2_stub_state;

inline void reset_copy_ubuf_to_gm_align_v2_stub_state() { g_copy_ubuf_to_gm_align_v2_stub_state = {}; }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 9201))
inline void wait_intra_block(pipe_t pipe, int flag_id) {}
inline void set_intra_block(pipe_t pipe, int sync_id) {}
#endif
inline void rls_buf(pipe_t pipe, uint64_t buf_id, bool mode) {}

inline void psts(vector_bool src, __ubuf__ uint32_t*& base, int32_t offset, Literal dist, Literal post) {}

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 9201))
inline int64_t get_iqent(pipe_t pipe) { return 0; }
#endif

#if defined(__DAV_VEC__) && defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 9201))
#include "c_api/defs/type.h"
inline void pstu(vector_store_unalign& ureg, vector_bool mask, __ubuf__ uint16_t*& dst) {}
inline void pstu(vector_store_unalign& ureg, vector_bool mask, __ubuf__ uint32_t*& dst) {}
#endif

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 9201))
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
// npu_arch_9201 asc_copy_ub2gm_align uses void* with rsw/non_eod ctrl params.
inline void copy_ubuf_to_gm_align_v2(
    __gm__ void* dst_addr, __ubuf__ void* src_addr, uint8_t sid, uint32_t burst_num, uint32_t burst_len, bool rsw_ctrl,
    bool rsw_packet_ctrl, uint8_t rsw_buffset_size, uint8_t l2_cache_mode, uint64_t burst_dst_stride,
    uint32_t burst_src_stride, bool non_eod_ctrl)
{
    ++g_copy_ubuf_to_gm_align_v2_stub_state.call_count;
    g_copy_ubuf_to_gm_align_v2_stub_state.rsw_ctrl = rsw_ctrl;
    g_copy_ubuf_to_gm_align_v2_stub_state.rsw_packet_ctrl = rsw_packet_ctrl;
    g_copy_ubuf_to_gm_align_v2_stub_state.rsw_buffset_size = rsw_buffset_size;
    g_copy_ubuf_to_gm_align_v2_stub_state.l2_cache_mode = l2_cache_mode;
    g_copy_ubuf_to_gm_align_v2_stub_state.dst_stride = burst_dst_stride;
    g_copy_ubuf_to_gm_align_v2_stub_state.src_stride = burst_src_stride;
    g_copy_ubuf_to_gm_align_v2_stub_state.non_eod_ctrl = non_eod_ctrl;
}
#endif

// ==========asc_prefetch_l2cache stubs (match npu_arch_9201 impl arity)==========
inline void __prefetch_stop() {}
inline void copy_gm_to_cbuf_align_v2(
    __cbuf__ int8_t* dst_addr, __gm__ int8_t* src_addr, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
    uint8_t left_padding_count, uint8_t right_padding_count, bool constant_padding_ctl, bool reserved,
    uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
{}
inline void copy_gm_to_ubuf_align_v2(
    __ubuf__ int8_t* dst_addr, __gm__ int8_t* src_addr, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
    uint8_t left_padding_count, uint8_t right_padding_count, bool constant_padding_ctl, bool reserved,
    uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride, bool reserved2)
{
    record_copy_gm_to_ubuf_align_v2(reserved, l2_cache_ctl, burst_src_stride, burst_dst_stride, reserved2);
}
inline void copy_gm_to_cbuf_multi_dn2nz(
    __cbuf__ int8_t* dst_addr, __gm__ int8_t* src_addr, uint8_t sid, uint64_t loop1_src_stride, uint8_t l2_cache_ctl,
    uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en, bool reserved)
{}
inline void copy_gm_to_cbuf_multi_nd2nz(
    __cbuf__ int8_t* dst_addr, __gm__ int8_t* src_addr, uint8_t sid, uint64_t loop1_src_stride, uint8_t l2_cache_ctl,
    uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en, bool reserved)
{}
inline void copy_gm_to_cbuf_align_v2(
    __cbuf__ uint8_t* dst_addr, __gm__ uint8_t* src_addr, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
    uint8_t left_padding_count, uint8_t right_padding_count, bool constant_padding_ctl, bool reserved,
    uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
{}
inline void copy_gm_to_ubuf_align_v2(
    __ubuf__ uint8_t* dst_addr, __gm__ uint8_t* src_addr, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
    uint8_t left_padding_count, uint8_t right_padding_count, bool constant_padding_ctl, bool reserved,
    uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride, bool reserved2)
{
    record_copy_gm_to_ubuf_align_v2(reserved, l2_cache_ctl, burst_src_stride, burst_dst_stride, reserved2);
}
inline void copy_gm_to_cbuf_multi_dn2nz(
    __cbuf__ uint8_t* dst_addr, __gm__ uint8_t* src_addr, uint8_t sid, uint64_t loop1_src_stride, uint8_t l2_cache_ctl,
    uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en, bool reserved)
{}
inline void copy_gm_to_cbuf_multi_nd2nz(
    __cbuf__ uint8_t* dst_addr, __gm__ uint8_t* src_addr, uint8_t sid, uint64_t loop1_src_stride, uint8_t l2_cache_ctl,
    uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en, bool reserved)
{}
inline void copy_gm_to_cbuf_align_v2(
    __cbuf__ hifloat8_t* dst_addr, __gm__ hifloat8_t* src_addr, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
    uint8_t left_padding_count, uint8_t right_padding_count, bool constant_padding_ctl, bool reserved,
    uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
{}
inline void copy_gm_to_ubuf_align_v2(
    __ubuf__ hifloat8_t* dst_addr, __gm__ hifloat8_t* src_addr, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
    uint8_t left_padding_count, uint8_t right_padding_count, bool constant_padding_ctl, bool reserved,
    uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride, bool reserved2)
{
    record_copy_gm_to_ubuf_align_v2(reserved, l2_cache_ctl, burst_src_stride, burst_dst_stride, reserved2);
}
inline void copy_gm_to_cbuf_multi_dn2nz(
    __cbuf__ hifloat8_t* dst_addr, __gm__ hifloat8_t* src_addr, uint8_t sid, uint64_t loop1_src_stride,
    uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en, bool reserved)
{}
inline void copy_gm_to_cbuf_multi_nd2nz(
    __cbuf__ hifloat8_t* dst_addr, __gm__ hifloat8_t* src_addr, uint8_t sid, uint64_t loop1_src_stride,
    uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en, bool reserved)
{}
inline void copy_gm_to_cbuf_align_v2(
    __cbuf__ fp8_e5m2_t* dst_addr, __gm__ fp8_e5m2_t* src_addr, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
    uint8_t left_padding_count, uint8_t right_padding_count, bool constant_padding_ctl, bool reserved,
    uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
{}
inline void copy_gm_to_ubuf_align_v2(
    __ubuf__ fp8_e5m2_t* dst_addr, __gm__ fp8_e5m2_t* src_addr, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
    uint8_t left_padding_count, uint8_t right_padding_count, bool constant_padding_ctl, bool reserved,
    uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride, bool reserved2)
{
    record_copy_gm_to_ubuf_align_v2(reserved, l2_cache_ctl, burst_src_stride, burst_dst_stride, reserved2);
}
inline void copy_gm_to_ubuf_align_v2(
    __ubuf__ fp8_e8m0_t* dst_addr, __gm__ fp8_e8m0_t* src_addr, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
    uint8_t left_padding_count, uint8_t right_padding_count, bool constant_padding_ctl, bool reserved,
    uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride, bool reserved2)
{
    record_copy_gm_to_ubuf_align_v2(reserved, l2_cache_ctl, burst_src_stride, burst_dst_stride, reserved2);
}
inline void copy_gm_to_ubuf_align_v2(
    __ubuf__ fp8_e6m2_t* dst_addr, __gm__ fp8_e6m2_t* src_addr, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
    uint8_t left_padding_count, uint8_t right_padding_count, bool constant_padding_ctl, bool reserved,
    uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride, bool reserved2)
{
    record_copy_gm_to_ubuf_align_v2(reserved, l2_cache_ctl, burst_src_stride, burst_dst_stride, reserved2);
}
inline void copy_gm_to_cbuf_multi_dn2nz(
    __cbuf__ fp8_e5m2_t* dst_addr, __gm__ fp8_e5m2_t* src_addr, uint8_t sid, uint64_t loop1_src_stride,
    uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en, bool reserved)
{}
inline void copy_gm_to_cbuf_multi_nd2nz(
    __cbuf__ fp8_e5m2_t* dst_addr, __gm__ fp8_e5m2_t* src_addr, uint8_t sid, uint64_t loop1_src_stride,
    uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en, bool reserved)
{}
inline void copy_gm_to_cbuf_align_v2(
    __cbuf__ fp8_e4m3fn_t* dst_addr, __gm__ fp8_e4m3fn_t* src_addr, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
    uint8_t left_padding_count, uint8_t right_padding_count, bool constant_padding_ctl, bool reserved,
    uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
{}
inline void copy_gm_to_ubuf_align_v2(
    __ubuf__ fp8_e4m3fn_t* dst_addr, __gm__ fp8_e4m3fn_t* src_addr, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
    uint8_t left_padding_count, uint8_t right_padding_count, bool constant_padding_ctl, bool reserved,
    uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride, bool reserved2)
{
    record_copy_gm_to_ubuf_align_v2(reserved, l2_cache_ctl, burst_src_stride, burst_dst_stride, reserved2);
}
inline void copy_gm_to_cbuf_multi_dn2nz(
    __cbuf__ fp8_e4m3fn_t* dst_addr, __gm__ fp8_e4m3fn_t* src_addr, uint8_t sid, uint64_t loop1_src_stride,
    uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en, bool reserved)
{}
inline void copy_gm_to_cbuf_multi_nd2nz(
    __cbuf__ fp8_e4m3fn_t* dst_addr, __gm__ fp8_e4m3fn_t* src_addr, uint8_t sid, uint64_t loop1_src_stride,
    uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en, bool reserved)
{}
inline void copy_gm_to_cbuf_align_v2(
    __cbuf__ int16_t* dst_addr, __gm__ int16_t* src_addr, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
    uint8_t left_padding_count, uint8_t right_padding_count, bool constant_padding_ctl, bool reserved,
    uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
{}
inline void copy_gm_to_ubuf_align_v2(
    __ubuf__ int16_t* dst_addr, __gm__ int16_t* src_addr, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
    uint8_t left_padding_count, uint8_t right_padding_count, bool constant_padding_ctl, bool reserved,
    uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride, bool reserved2)
{
    record_copy_gm_to_ubuf_align_v2(reserved, l2_cache_ctl, burst_src_stride, burst_dst_stride, reserved2);
}
inline void copy_gm_to_cbuf_multi_dn2nz(
    __cbuf__ int16_t* dst_addr, __gm__ int16_t* src_addr, uint8_t sid, uint64_t loop1_src_stride, uint8_t l2_cache_ctl,
    uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en, bool reserved)
{}
inline void copy_gm_to_cbuf_multi_nd2nz(
    __cbuf__ int16_t* dst_addr, __gm__ int16_t* src_addr, uint8_t sid, uint64_t loop1_src_stride, uint8_t l2_cache_ctl,
    uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en, bool reserved)
{}
inline void copy_gm_to_cbuf_align_v2(
    __cbuf__ uint16_t* dst_addr, __gm__ uint16_t* src_addr, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
    uint8_t left_padding_count, uint8_t right_padding_count, bool constant_padding_ctl, bool reserved,
    uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
{}
inline void copy_gm_to_ubuf_align_v2(
    __ubuf__ uint16_t* dst_addr, __gm__ uint16_t* src_addr, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
    uint8_t left_padding_count, uint8_t right_padding_count, bool constant_padding_ctl, bool reserved,
    uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride, bool reserved2)
{
    record_copy_gm_to_ubuf_align_v2(reserved, l2_cache_ctl, burst_src_stride, burst_dst_stride, reserved2);
}
inline void copy_gm_to_cbuf_multi_dn2nz(
    __cbuf__ uint16_t* dst_addr, __gm__ uint16_t* src_addr, uint8_t sid, uint64_t loop1_src_stride,
    uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en, bool reserved)
{}
inline void copy_gm_to_cbuf_multi_nd2nz(
    __cbuf__ uint16_t* dst_addr, __gm__ uint16_t* src_addr, uint8_t sid, uint64_t loop1_src_stride,
    uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en, bool reserved)
{}
inline void copy_gm_to_cbuf_align_v2(
    __cbuf__ half* dst_addr, __gm__ half* src_addr, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
    uint8_t left_padding_count, uint8_t right_padding_count, bool constant_padding_ctl, bool reserved,
    uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
{}
inline void copy_gm_to_ubuf_align_v2(
    __ubuf__ half* dst_addr, __gm__ half* src_addr, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
    uint8_t left_padding_count, uint8_t right_padding_count, bool constant_padding_ctl, bool reserved,
    uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride, bool reserved2)
{
    record_copy_gm_to_ubuf_align_v2(reserved, l2_cache_ctl, burst_src_stride, burst_dst_stride, reserved2);
}
inline void copy_gm_to_cbuf_multi_dn2nz(
    __cbuf__ half* dst_addr, __gm__ half* src_addr, uint8_t sid, uint64_t loop1_src_stride, uint8_t l2_cache_ctl,
    uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en, bool reserved)
{}
inline void copy_gm_to_cbuf_multi_nd2nz(
    __cbuf__ half* dst_addr, __gm__ half* src_addr, uint8_t sid, uint64_t loop1_src_stride, uint8_t l2_cache_ctl,
    uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en, bool reserved)
{}
inline void copy_gm_to_cbuf_align_v2(
    __cbuf__ bfloat16_t* dst_addr, __gm__ bfloat16_t* src_addr, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
    uint8_t left_padding_count, uint8_t right_padding_count, bool constant_padding_ctl, bool reserved,
    uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
{}
inline void copy_gm_to_ubuf_align_v2(
    __ubuf__ bfloat16_t* dst_addr, __gm__ bfloat16_t* src_addr, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
    uint8_t left_padding_count, uint8_t right_padding_count, bool constant_padding_ctl, bool reserved,
    uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride, bool reserved2)
{
    record_copy_gm_to_ubuf_align_v2(reserved, l2_cache_ctl, burst_src_stride, burst_dst_stride, reserved2);
}
inline void copy_gm_to_cbuf_multi_dn2nz(
    __cbuf__ bfloat16_t* dst_addr, __gm__ bfloat16_t* src_addr, uint8_t sid, uint64_t loop1_src_stride,
    uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en, bool reserved)
{}
inline void copy_gm_to_cbuf_multi_nd2nz(
    __cbuf__ bfloat16_t* dst_addr, __gm__ bfloat16_t* src_addr, uint8_t sid, uint64_t loop1_src_stride,
    uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en, bool reserved)
{}
inline void copy_gm_to_cbuf_align_v2(
    __cbuf__ int32_t* dst_addr, __gm__ int32_t* src_addr, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
    uint8_t left_padding_count, uint8_t right_padding_count, bool constant_padding_ctl, bool reserved,
    uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
{}
inline void copy_gm_to_ubuf_align_v2(
    __ubuf__ int32_t* dst_addr, __gm__ int32_t* src_addr, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
    uint8_t left_padding_count, uint8_t right_padding_count, bool constant_padding_ctl, bool reserved,
    uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride, bool reserved2)
{
    record_copy_gm_to_ubuf_align_v2(reserved, l2_cache_ctl, burst_src_stride, burst_dst_stride, reserved2);
}
inline void copy_gm_to_cbuf_multi_dn2nz(
    __cbuf__ int32_t* dst_addr, __gm__ int32_t* src_addr, uint8_t sid, uint64_t loop1_src_stride, uint8_t l2_cache_ctl,
    uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en, bool reserved)
{}
inline void copy_gm_to_cbuf_multi_nd2nz(
    __cbuf__ int32_t* dst_addr, __gm__ int32_t* src_addr, uint8_t sid, uint64_t loop1_src_stride, uint8_t l2_cache_ctl,
    uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en, bool reserved)
{}
inline void copy_gm_to_cbuf_align_v2(
    __cbuf__ uint32_t* dst_addr, __gm__ uint32_t* src_addr, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
    uint8_t left_padding_count, uint8_t right_padding_count, bool constant_padding_ctl, bool reserved,
    uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
{}
inline void copy_gm_to_ubuf_align_v2(
    __ubuf__ uint32_t* dst_addr, __gm__ uint32_t* src_addr, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
    uint8_t left_padding_count, uint8_t right_padding_count, bool constant_padding_ctl, bool reserved,
    uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride, bool reserved2)
{
    record_copy_gm_to_ubuf_align_v2(reserved, l2_cache_ctl, burst_src_stride, burst_dst_stride, reserved2);
}
inline void copy_gm_to_cbuf_multi_dn2nz(
    __cbuf__ uint32_t* dst_addr, __gm__ uint32_t* src_addr, uint8_t sid, uint64_t loop1_src_stride,
    uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en, bool reserved)
{}
inline void copy_gm_to_cbuf_multi_nd2nz(
    __cbuf__ uint32_t* dst_addr, __gm__ uint32_t* src_addr, uint8_t sid, uint64_t loop1_src_stride,
    uint8_t l2_cache_ctl, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en, bool reserved)
{}
inline void copy_gm_to_cbuf_align_v2(
    __cbuf__ float* dst_addr, __gm__ float* src_addr, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
    uint8_t left_padding_count, uint8_t right_padding_count, bool constant_padding_ctl, bool reserved,
    uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
{}
inline void copy_gm_to_ubuf_align_v2(
    __ubuf__ float* dst_addr, __gm__ float* src_addr, uint8_t sid, uint32_t burst_num, uint32_t burst_len,
    uint8_t left_padding_count, uint8_t right_padding_count, bool constant_padding_ctl, bool reserved,
    uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride, bool reserved2)
{
    record_copy_gm_to_ubuf_align_v2(reserved, l2_cache_ctl, burst_src_stride, burst_dst_stride, reserved2);
}
inline void copy_gm_to_cbuf_multi_dn2nz(
    __cbuf__ float* dst_addr, __gm__ float* src_addr, uint8_t sid, uint64_t loop1_src_stride, uint8_t l2_cache_ctl,
    uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en, bool reserved)
{}
inline void copy_gm_to_cbuf_multi_nd2nz(
    __cbuf__ float* dst_addr, __gm__ float* src_addr, uint8_t sid, uint64_t loop1_src_stride, uint8_t l2_cache_ctl,
    uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en, bool reserved)
{}

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

// ==========scatter_ubuf_to_gm===========
inline void scatter_ubuf_to_gm(
    __gm__ uint8_t* dst, __ubuf__ uint8_t* dataSrc, uint8_t sid, uint8_t offset_type, __ubuf__ void* offsetSrc,
    uint8_t l2_cache_ctl, uint32_t burst_len, uint32_t src_stride, uint32_t offset_num)
{}

inline void scatter_ubuf_to_gm(
    __gm__ uint16_t* dst, __ubuf__ uint16_t* dataSrc, uint8_t sid, uint8_t offset_type, __ubuf__ void* offsetSrc,
    uint8_t l2_cache_ctl, uint32_t burst_len, uint32_t src_stride, uint32_t offset_num)
{}

inline void scatter_ubuf_to_gm(
    __gm__ uint32_t* dst, __ubuf__ uint32_t* dataSrc, uint8_t sid, uint8_t offset_type, __ubuf__ void* offsetSrc,
    uint8_t l2_cache_ctl, uint32_t burst_len, uint32_t src_stride, uint32_t offset_num)
{}

inline void scatter_ubuf_to_gm(
    __gm__ uint64_t* dst, __ubuf__ uint64_t* dataSrc, uint8_t sid, uint8_t offset_type, __ubuf__ void* offsetSrc,
    uint8_t l2_cache_ctl, uint32_t burst_len, uint32_t src_stride, uint32_t offset_num)
{}

// ==========gather_gm_to_ubuf===========
inline void gather_gm_to_ubuf(
    __ubuf__ uint8_t* dst, __gm__ uint8_t* baseSrc, uint8_t sid, uint8_t offset_type, bool constant_padding_ctl,
    __ubuf__ void* offsetSrc, uint8_t l2_cache_ctl, uint32_t burst_len, uint32_t dst_stride, uint32_t offset_num)
{}

inline void gather_gm_to_ubuf(
    __ubuf__ uint16_t* dst, __gm__ uint16_t* baseSrc, uint8_t sid, uint8_t offset_type, bool constant_padding_ctl,
    __ubuf__ void* offsetSrc, uint8_t l2_cache_ctl, uint32_t burst_len, uint32_t dst_stride, uint32_t offset_num)
{}

inline void gather_gm_to_ubuf(
    __ubuf__ uint32_t* dst, __gm__ uint32_t* baseSrc, uint8_t sid, uint8_t offset_type, bool constant_padding_ctl,
    __ubuf__ void* offsetSrc, uint8_t l2_cache_ctl, uint32_t burst_len, uint32_t dst_stride, uint32_t offset_num)
{}

inline void gather_gm_to_ubuf(
    __ubuf__ uint64_t* dst, __gm__ uint64_t* baseSrc, uint8_t sid, uint8_t offset_type, bool constant_padding_ctl,
    __ubuf__ void* offsetSrc, uint8_t l2_cache_ctl, uint32_t burst_len, uint32_t dst_stride, uint32_t offset_num)
{}

#if defined(__DAV_CUBE__)
inline int32_t g_coreType = 1;
#else
inline int32_t g_coreType = 2;
#endif

// ==========9201 v2 instruction stubs==========
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 9201)
inline void nddma_out_to_ub_b8(
    __ubuf__ void* dst, __gm__ void* src, uint8_t sid, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, uint8_t l2_cache_ctl)
{}

inline void nddma_out_to_ub_b16(
    __ubuf__ void* dst, __gm__ void* src, uint8_t sid, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, uint8_t l2_cache_ctl)
{}

inline void nddma_out_to_ub_b32(
    __ubuf__ void* dst, __gm__ void* src, uint8_t sid, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, uint8_t l2_cache_ctl)
{}

inline void vfma(vector_f16& dst, vector_f16 src0, vector_f16 src1, vector_bool mask, Literal mode) {}
inline void vfma(vector_bf16& dst, vector_bf16 src0, vector_bf16 src1, vector_bool mask, Literal mode) {}
inline void vfma(vector_f32& dst, vector_f32 src0, vector_f32 src1, vector_bool mask, Literal mode) {}
inline void vfnms(vector_f16& dst, vector_f16 src0, vector_f16 src1, vector_bool mask, Literal mode) {}
inline void vfnms(vector_bf16& dst, vector_bf16 src0, vector_bf16 src1, vector_bool mask, Literal mode) {}
inline void vfnms(vector_f32& dst, vector_f32 src0, vector_f32 src1, vector_bool mask, Literal mode) {}
inline void vfms(vector_f16& dst, vector_f16 src0, vector_f16 src1, vector_bool mask, Literal mode) {}
inline void vfms(vector_bf16& dst, vector_bf16 src0, vector_bf16 src1, vector_bool mask, Literal mode) {}
inline void vfms(vector_f32& dst, vector_f32 src0, vector_f32 src1, vector_bool mask, Literal mode) {}
inline void vfnma(vector_f16& dst, vector_f16 src0, vector_f16 src1, vector_bool mask, Literal mode) {}
inline void vfnma(vector_bf16& dst, vector_bf16 src0, vector_bf16 src1, vector_bool mask, Literal mode) {}
inline void vfnma(vector_f32& dst, vector_f32 src0, vector_f32 src1, vector_bool mask, Literal mode) {}
inline void vshl(vector_u8& dst, vector_u8 src, vector_u8 shift, vector_bool mask, Literal mode) {}
inline void vshl(vector_s8& dst, vector_s8 src, vector_u8 shift, vector_bool mask, Literal mode) {}
inline void vshl(vector_u16& dst, vector_u16 src, vector_u16 shift, vector_bool mask, Literal mode) {}
inline void vshl(vector_s16& dst, vector_s16 src, vector_u16 shift, vector_bool mask, Literal mode) {}
inline void vshl(vector_u32& dst, vector_u32 src, vector_u32 shift, vector_bool mask, Literal mode) {}
inline void vshl(vector_s32& dst, vector_s32 src, vector_u32 shift, vector_bool mask, Literal mode) {}
inline void vshr(vector_u8& dst, vector_u8 src, vector_u8 shift, vector_bool mask, Literal mode) {}
inline void vshr(vector_s8& dst, vector_s8 src, vector_u8 shift, vector_bool mask, Literal mode) {}
inline void vshr(vector_u16& dst, vector_u16 src, vector_u16 shift, vector_bool mask, Literal mode) {}
inline void vshr(vector_s16& dst, vector_s16 src, vector_u16 shift, vector_bool mask, Literal mode) {}
inline void vshr(vector_u32& dst, vector_u32 src, vector_u32 shift, vector_bool mask, Literal mode) {}
inline void vshr(vector_s32& dst, vector_s32 src, vector_u32 shift, vector_bool mask, Literal mode) {}
inline void vshls(vector_u8& dst, vector_u8 src, uint16_t shift, vector_bool mask, Literal mode) {}
inline void vshls(vector_s8& dst, vector_s8 src, uint16_t shift, vector_bool mask, Literal mode) {}
inline void vshls(vector_u16& dst, vector_u16 src, uint16_t shift, vector_bool mask, Literal mode) {}
inline void vshls(vector_s16& dst, vector_s16 src, uint16_t shift, vector_bool mask, Literal mode) {}
inline void vshls(vector_u32& dst, vector_u32 src, uint32_t shift, vector_bool mask, Literal mode) {}
inline void vshls(vector_s32& dst, vector_s32 src, uint32_t shift, vector_bool mask, Literal mode) {}
inline void vshrs(vector_u8& dst, vector_u8 src, uint16_t shift, vector_bool mask, Literal mode) {}
inline void vshrs(vector_s8& dst, vector_s8 src, uint16_t shift, vector_bool mask, Literal mode) {}
inline void vshrs(vector_u16& dst, vector_u16 src, uint16_t shift, vector_bool mask, Literal mode) {}
inline void vshrs(vector_s16& dst, vector_s16 src, uint16_t shift, vector_bool mask, Literal mode) {}
inline void vshrs(vector_u32& dst, vector_u32 src, uint32_t shift, vector_bool mask, Literal mode) {}
inline void vshrs(vector_s32& dst, vector_s32 src, uint32_t shift, vector_bool mask, Literal mode) {}
inline void vshl(vector_u64& dst, vector_u64 src, vector_u32 shift, vector_bool mask, Literal mode) {}
inline void vshl(vector_s64& dst, vector_s64 src, vector_u32 shift, vector_bool mask, Literal mode) {}
inline void vshr(vector_u64& dst, vector_u64 src, vector_u32 shift, vector_bool mask, Literal mode) {}
inline void vshr(vector_s64& dst, vector_s64 src, vector_u32 shift, vector_bool mask, Literal mode) {}
inline void vshls(vector_u64& dst, vector_u64 src, uint32_t shift, vector_bool mask, Literal mode) {}
inline void vshls(vector_s64& dst, vector_s64 src, uint32_t shift, vector_bool mask, Literal mode) {}
inline void vshrs(vector_u64& dst, vector_u64 src, uint32_t shift, vector_bool mask, Literal mode) {}
inline void vshrs(vector_s64& dst, vector_s64 src, uint32_t shift, vector_bool mask, Literal mode) {}
inline void vcgmaxv2(vector_f16& dst, vector_f16 src, vector_bool mask) {}
inline void vcgmaxv2(vector_f32& dst, vector_f32 src, vector_bool mask) {}
inline void vcgmaxv2(vector_u16& dst, vector_u16 src, vector_bool mask) {}
inline void vcgmaxv2(vector_s16& dst, vector_s16 src, vector_bool mask) {}
inline void vcgmaxv2(vector_u32& dst, vector_u32 src, vector_bool mask) {}
inline void vcgmaxv2(vector_s32& dst, vector_s32 src, vector_bool mask) {}
inline void vcgminv2(vector_f16& dst, vector_f16 src, vector_bool mask) {}
inline void vcgminv2(vector_f32& dst, vector_f32 src, vector_bool mask) {}
inline void vcgminv2(vector_u16& dst, vector_u16 src, vector_bool mask) {}
inline void vcgminv2(vector_s16& dst, vector_s16 src, vector_bool mask) {}
inline void vcgminv2(vector_u32& dst, vector_u32 src, vector_bool mask) {}
inline void vcgminv2(vector_s32& dst, vector_s32 src, vector_bool mask) {}
inline void vcgaddv2(vector_f16& dst, vector_f16 src, vector_bool mask) {}
inline void vcgaddv2(vector_f32& dst, vector_f32 src, vector_bool mask) {}
inline void vcgaddv2(vector_u32& dst, vector_u16 src, vector_bool mask) {}
inline void vcgaddv2(vector_s32& dst, vector_s16 src, vector_bool mask) {}
inline void vcgaddv2(vector_u32& dst, vector_u32 src, vector_bool mask) {}
inline void vcgaddv2(vector_s32& dst, vector_s32 src, vector_bool mask) {}
inline void vcpaddv2(vector_f16& dst, vector_f16 src, vector_bool mask) {}
inline void vcpaddv2(vector_f32& dst, vector_f32 src, vector_bool mask) {}
inline void vcmaxv2(vector_f16& dst, vector_f16 src) {}
inline void vcmaxv2(vector_f32& dst, vector_f32 src) {}
inline void vcmaxv2(vector_u16& dst, vector_u16 src) {}
inline void vcmaxv2(vector_s16& dst, vector_s16 src) {}
inline void vcmaxv2(vector_u32& dst, vector_u32 src) {}
inline void vcmaxv2(vector_s32& dst, vector_s32 src) {}
inline void vcminv2(vector_f16& dst, vector_f16 src) {}
inline void vcminv2(vector_f32& dst, vector_f32 src) {}
inline void vcminv2(vector_u16& dst, vector_u16 src) {}
inline void vcminv2(vector_s16& dst, vector_s16 src) {}
inline void vcminv2(vector_u32& dst, vector_u32 src) {}
inline void vcminv2(vector_s32& dst, vector_s32 src) {}
inline void vcaddv2(vector_s32& dst, vector_s32 src) {}
inline void vcaddv2(vector_u32& dst, vector_u32 src) {}
inline void vcaddv2(vector_f16& dst, vector_f16 src) {}
inline void vcaddv2(vector_f32& dst, vector_f32 src) {}
inline void vintlvv2(vector_u8& dst, vector_u8 src0, vector_u8 src1, Literal pos) {}
inline void vintlvv2(vector_s8& dst, vector_s8 src0, vector_s8 src1, Literal pos) {}
inline void vintlvv2(vector_u16& dst, vector_u16 src0, vector_u16 src1, Literal pos) {}
inline void vintlvv2(vector_s16& dst, vector_s16 src0, vector_s16 src1, Literal pos) {}
inline void vintlvv2(vector_u32& dst, vector_u32 src0, vector_u32 src1, Literal pos) {}
inline void vintlvv2(vector_s32& dst, vector_s32 src0, vector_s32 src1, Literal pos) {}
inline void vintlvv2(vector_f16& dst, vector_f16 src0, vector_f16 src1, Literal pos) {}
inline void vintlvv2(vector_f32& dst, vector_f32 src0, vector_f32 src1, Literal pos) {}
inline void vintlvv2(vector_f8e4m3& dst, vector_f8e4m3 src0, vector_f8e4m3 src1, Literal pos) {}
inline void vintlvv2(vector_f8e5m2& dst, vector_f8e5m2 src0, vector_f8e5m2 src1, Literal pos) {}
inline void vintlvv2(vector_f8e8m0& dst, vector_f8e8m0 src0, vector_f8e8m0 src1, Literal pos) {}
inline void vintlvv2(vector_bf16& dst, vector_bf16 src0, vector_bf16 src1, Literal pos) {}
inline void vintlvv2(vector_f8e6m2& dst, vector_f8e6m2 src0, vector_f8e6m2 src1, Literal pos) {}
inline void vdintlvv2(vector_u8& dst, vector_u8 src0, vector_u8 src1, Literal pos) {}
inline void vdintlvv2(vector_s8& dst, vector_s8 src0, vector_s8 src1, Literal pos) {}
inline void vdintlvv2(vector_u16& dst, vector_u16 src0, vector_u16 src1, Literal pos) {}
inline void vdintlvv2(vector_s16& dst, vector_s16 src0, vector_s16 src1, Literal pos) {}
inline void vdintlvv2(vector_u32& dst, vector_u32 src0, vector_u32 src1, Literal pos) {}
inline void vdintlvv2(vector_s32& dst, vector_s32 src0, vector_s32 src1, Literal pos) {}
inline void vdintlvv2(vector_f16& dst, vector_f16 src0, vector_f16 src1, Literal pos) {}
inline void vdintlvv2(vector_f32& dst, vector_f32 src0, vector_f32 src1, Literal pos) {}
inline void vdintlvv2(vector_f8e4m3& dst, vector_f8e4m3 src0, vector_f8e4m3 src1, Literal pos) {}
inline void vdintlvv2(vector_f8e5m2& dst, vector_f8e5m2 src0, vector_f8e5m2 src1, Literal pos) {}
inline void vdintlvv2(vector_f8e8m0& dst, vector_f8e8m0 src0, vector_f8e8m0 src1, Literal pos) {}
inline void vdintlvv2(vector_bf16& dst, vector_bf16 src0, vector_bf16 src1, Literal pos) {}
inline void vdintlvv2(vector_f8e6m2& dst, vector_f8e6m2 src0, vector_f8e6m2 src1, Literal pos) {}
inline void pintlvv2_b8(vector_bool& dst, vector_bool src0, vector_bool src1, Literal pos) {}
inline void pintlvv2_b16(vector_bool& dst, vector_bool src0, vector_bool src1, Literal pos) {}
inline void pintlvv2_b32(vector_bool& dst, vector_bool src0, vector_bool src1, Literal pos) {}
inline void pdintlvv2_b8(vector_bool& dst, vector_bool src0, vector_bool src1, Literal pos) {}
inline void pdintlvv2_b16(vector_bool& dst, vector_bool src0, vector_bool src1, Literal pos) {}
inline void pdintlvv2_b32(vector_bool& dst, vector_bool src0, vector_bool src1, Literal pos) {}
#endif

#endif
