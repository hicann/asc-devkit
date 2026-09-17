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
 * \file mmad_s2.h
 * \brief 场景2（bfloat16 输入、float 输出、A 不转置 B 转置、无 Bias、两次 Mmad 累加）的常量与完整流水线函数
 */

#pragma once

#ifndef ceil_div
#define ceil_div(a, b) (((a) + (b) - 1) / (b))
#define ceil_align(a, b) (ceil_div(a, b) * (b))
#endif

// ========== 场景2 常量 ==========
constexpr uint32_t S2_C0_SIZE = 16;
constexpr uint32_t S2_FRACTAL_NUM = 1;
constexpr uint32_t S2_FRACTAL_SIZE = BLOCK_CUBE * S2_C0_SIZE; // 256

constexpr uint32_t S2_A_L1_SIZE = ceil_align(M, BLOCK_CUBE) * ceil_align(K, S2_C0_SIZE); // 32 Ã 80 = 2560
constexpr uint32_t S2_A_L0_SIZE = ceil_align(M, BLOCK_CUBE) * ceil_align(K, S2_C0_SIZE);
constexpr uint32_t S2_B_L1_SIZE = ceil_align(N, BLOCK_CUBE) * ceil_align(K, S2_C0_SIZE); // 48 Ã 80 = 3840
constexpr uint32_t S2_B_L0_SIZE = ceil_align(K, S2_C0_SIZE) * ceil_align(N, BLOCK_CUBE); // 80 Ã 48 = 3840
constexpr uint32_t S2_C_L0_SIZE = ceil_align(M, BLOCK_CUBE) * ceil_align(N, BLOCK_CUBE); // 32 Ã 48 = 1536

// ========== 阶段1：GM → L1 ==========

__aicore__ inline void s2_copy_matrix_a_to_l1(__cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src)
{
    uint64_t config = (static_cast<uint64_t>(0) << 48) | (static_cast<uint64_t>(ceil_align(M, BLOCK_CUBE)) << 32) |
                      (static_cast<uint64_t>(1) << 16) | (static_cast<uint64_t>(1));

    asc_set_gm2l1_nz_para(config);

    uint64_t loop1_src_stride = K * sizeof(bfloat16_t);
    uint8_t l2_cache_ctl = 0;
    uint16_t n_value = M;
    uint32_t d_value = K;
    uint64_t loop4_src_stride = 0;
    bool smallc0_en = false;

    asc_copy_gm2l1_nd2nz(dst, src, loop1_src_stride, l2_cache_ctl, n_value, d_value, loop4_src_stride, smallc0_en);

    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
}

__aicore__ inline void s2_copy_matrix_b_to_l1(__cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src)
{
    uint64_t config = (static_cast<uint64_t>(0) << 48) | (static_cast<uint64_t>(ceil_align(N, BLOCK_CUBE)) << 32) |
                      (static_cast<uint64_t>(1) << 16) | (static_cast<uint64_t>(1));

    asc_set_gm2l1_nz_para(config);

    uint64_t loop1_src_stride = K * sizeof(bfloat16_t);
    uint8_t l2_cache_ctl = 0;
    uint16_t n_value = N;
    uint32_t d_value = K;
    uint64_t loop4_src_stride = 0;
    bool smallc0_en = false;

    asc_copy_gm2l1_nd2nz(dst, src, loop1_src_stride, l2_cache_ctl, n_value, d_value, loop4_src_stride, smallc0_en);

    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID1);
}

// ========== 阶段2：L1 → L0A / L0B ==========

__aicore__ inline void s2_split_matrix_a_to_l0a(__ca__ bfloat16_t* dst, __cbuf__ bfloat16_t* src)
{
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    uint16_t m_start_position = 0;
    uint16_t k_start_position = 0;
    uint8_t m_step = ceil_div(M, BLOCK_CUBE);
    uint8_t k_step = ceil_div(K, S2_C0_SIZE);
    int16_t src_stride = ceil_div(M, BLOCK_CUBE);
    uint16_t dst_stride = ceil_div(M, BLOCK_CUBE);

    asc_copy_l12l0a(dst, src, m_start_position, k_start_position, m_step, k_step, src_stride, dst_stride);

    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
}

__aicore__ inline void s2_split_matrix_b_to_l0b(__cb__ bfloat16_t* dst, __cbuf__ bfloat16_t* src)
{
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID1);

    uint16_t m_start_position = 0;
    uint16_t k_start_position = 0;
    uint8_t m_step = ceil_div(N, BLOCK_CUBE);
    uint8_t k_step = ceil_div(K, S2_C0_SIZE);
    int16_t src_stride = ceil_div(N, BLOCK_CUBE);
    uint16_t dst_stride = ceil_div(N, BLOCK_CUBE);

    asc_copy_l12l0b(dst, src, m_start_position, k_start_position, m_step, k_step, src_stride, dst_stride);

    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID1);
}

// ========== 阶段3：Mmad（两次调用） ==========

__aicore__ inline void s2_compute_mmad(__cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ bfloat16_t* b_matrix)
{
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID1);

    uint16_t left_height = M;
    uint16_t n_dim = K;
    uint16_t right_width = N;
    uint8_t unit_flag = 0;
    bool disable_gemv = true;
    bool c_matrix_source = false;
    bool c_matrix_init_val = true;

    asc_mmad(
        c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width, unit_flag, disable_gemv, c_matrix_source,
        c_matrix_init_val);

    c_matrix_source = false;
    c_matrix_init_val = false;

    asc_mmad(
        c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width, unit_flag, disable_gemv, c_matrix_source,
        c_matrix_init_val);

    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
}

// ========== 阶段4：L0C → GM ==========

__aicore__ inline void s2_copy_result_to_gm(__gm__ float* dst, __cc__ float* src)
{
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

    uint16_t matrix_num = 1;
    uint16_t src_nz_matrix_stride = 0;
    uint32_t dst_matrix_stride = 0;

    asc_set_l0c_copy_nz_para(matrix_num, src_nz_matrix_stride, dst_matrix_stride);

    uint16_t n_size = N;
    uint16_t m_size = M;
    uint32_t loop_dst_stride = N;
    uint16_t loop_src_stride = ceil_align(M, BLOCK_CUBE);
    uint8_t l2_cache_ctl = 0;
    uint8_t clip_relu_pre = 0;
    uint8_t unit_flag_ctl = 0;
    uint64_t quant_pre = 0;
    uint8_t relu_pre = 0;
    bool split_en = false;
    bool nz2nd_en = true;
    uint64_t quant_post = 0;
    uint8_t relu_post = 0;
    bool clip_relu_post = false;
    uint8_t eltwise_op = 0;
    bool eltwise_antq_en = false;
    bool c0_pad_en = false;
    bool broadcast_en = false;
    bool nz2dn_en = false;

    asc_copy_l0c2gm(
        dst, src, n_size, m_size, loop_dst_stride, loop_src_stride, l2_cache_ctl, clip_relu_pre, unit_flag_ctl,
        quant_pre, relu_pre, split_en, nz2nd_en, quant_post, relu_post, clip_relu_post, eltwise_op, eltwise_antq_en,
        c0_pad_en, broadcast_en, nz2dn_en);
}
