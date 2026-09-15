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
 * \file mmad_s1.h
 * \brief 场景1（int8_t 输入、int32_t 输出、A/B 不转置、带 Bias）的常量与完整流水线函数
 */

#pragma once

// ========== 场景1 常量 ==========
constexpr uint32_t S1_C0_SIZE = 32;
constexpr uint32_t S1_FRACTAL_NUM = 2;
constexpr uint32_t S1_FRACTAL_SIZE = BLOCK_CUBE * S1_C0_SIZE; // 512

constexpr uint32_t S1_A_L1_SIZE = ceil_align(M, BLOCK_CUBE) * ceil_align(K, S1_C0_SIZE);
constexpr uint32_t S1_A_L0_SIZE = ceil_align(M, BLOCK_CUBE) * ceil_align(K, S1_C0_SIZE);
constexpr uint32_t S1_B_L1_SIZE = ceil_align(K, BLOCK_CUBE* S1_FRACTAL_NUM) * ceil_align(N, S1_C0_SIZE);
constexpr uint32_t S1_B_L0_SIZE = ceil_align(K, S1_C0_SIZE) * ceil_align(N, BLOCK_CUBE* S1_FRACTAL_NUM);
constexpr uint32_t S1_C_L0_SIZE = ceil_align(M, BLOCK_CUBE) * ceil_align(N, BLOCK_CUBE);
constexpr uint32_t S1_BIAS_BYTES = ceil_align(AscendC::Std::ceil_div(N * sizeof(int32_t), 32), 2) * 32;
constexpr uint32_t S1_BIAS_ELEMS = S1_BIAS_BYTES / sizeof(int32_t);

// ========== 阶段1：GM → L1 ==========

__aicore__ inline void s1_copy_matrix_a_to_l1(__cbuf__ int8_t* dst, __gm__ int8_t* src)
{
    uint64_t config = (static_cast<uint64_t>(0) << 48) | (static_cast<uint64_t>(ceil_align(M, BLOCK_CUBE)) << 32) |
                      (static_cast<uint64_t>(1) << 16) | (static_cast<uint64_t>(1));

    asc_set_gm2l1_nz_para(config);

    uint64_t loop1_src_stride = K * sizeof(int8_t);
    uint8_t l2_cache_ctl = 0;
    uint16_t n_value = M;
    uint32_t d_value = K;
    uint64_t loop4_src_stride = 0;
    bool smallc0_en = false;

    asc_copy_gm2l1_nd2nz(dst, src, loop1_src_stride, l2_cache_ctl, n_value, d_value, loop4_src_stride, smallc0_en);

    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
}

__aicore__ inline void s1_copy_matrix_b_to_l1(__cbuf__ int8_t* dst, __gm__ int8_t* src)
{
    uint64_t config = (static_cast<uint64_t>(0) << 48) |
                      (static_cast<uint64_t>(ceil_align(K, BLOCK_CUBE * S1_FRACTAL_NUM)) << 32) |
                      (static_cast<uint64_t>(1) << 16) | (static_cast<uint64_t>(1));

    asc_set_gm2l1_nz_para(config);

    uint64_t loop1_src_stride = N * sizeof(int8_t);
    uint8_t l2_cache_ctl = 0;
    uint16_t n_value = K;
    uint32_t d_value = N;
    uint64_t loop4_src_stride = 0;
    bool smallc0_en = false;

    asc_copy_gm2l1_nd2nz(dst, src, loop1_src_stride, l2_cache_ctl, n_value, d_value, loop4_src_stride, smallc0_en);

    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID1);
}

__aicore__ inline void s1_copy_bias_to_l1(__cbuf__ int32_t* dst, __gm__ int32_t* src)
{
    uint64_t config = (static_cast<uint64_t>(0) << 48) | (static_cast<uint64_t>(1) << 32) |
                      (static_cast<uint64_t>(1) << 16) | (static_cast<uint64_t>(1));

    asc_set_gm2l1_nz_para(config);

    uint64_t loop1_src_stride = N * sizeof(int32_t);
    uint8_t l2_cache_ctl = 0;
    uint16_t n_value = 1;
    uint32_t d_value = N;
    uint64_t loop4_src_stride = 0;
    bool smallc0_en = false;

    asc_copy_gm2l1_nd2nz(dst, src, loop1_src_stride, l2_cache_ctl, n_value, d_value, loop4_src_stride, smallc0_en);

    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID2);
}

// ========== 阶段2：L1 → L0A / L0B / BT ==========

__aicore__ inline void s1_split_matrix_a_to_l0a(__ca__ int8_t* dst, __cbuf__ int8_t* src)
{
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);

    uint16_t m_start_position = 0;
    uint16_t k_start_position = 0;
    uint8_t m_step = AscendC::Std::ceil_div(M, BLOCK_CUBE);
    uint8_t k_step = AscendC::Std::ceil_div(K, S1_C0_SIZE);
    int16_t src_stride = AscendC::Std::ceil_div(M, BLOCK_CUBE);
    uint16_t dst_stride = AscendC::Std::ceil_div(M, BLOCK_CUBE);

    asc_copy_l12l0a(dst, src, m_start_position, k_start_position, m_step, k_step, src_stride, dst_stride);

    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
}

__aicore__ inline void s1_split_matrix_b_to_l0b_trans(__cb__ int8_t* dst, __cbuf__ int8_t* src)
{
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID1);

    constexpr uint32_t k_blocks = AscendC::Std::ceil_div(K, BLOCK_CUBE * S1_FRACTAL_NUM);
    constexpr uint32_t dst_offset =
        AscendC::Std::ceil_div(N, BLOCK_CUBE * S1_FRACTAL_NUM) * S1_FRACTAL_SIZE * S1_FRACTAL_NUM;
    constexpr uint32_t src_offset = S1_FRACTAL_SIZE * S1_FRACTAL_NUM;

    uint16_t index_id = 0;
    uint8_t repeat = AscendC::Std::ceil_div(N, S1_C0_SIZE);
    uint16_t src_stride = AscendC::Std::ceil_div(K, BLOCK_CUBE * S1_FRACTAL_NUM) * 2;
    uint16_t dst_gap = 1;
    uint16_t dst_frac_gap = 0;
    uint16_t src_frac_gap = 0;

    for (uint32_t i = 0; i < k_blocks; ++i) {
        asc_copy_l12l0b_trans(
            dst + i * dst_offset, src + i * src_offset, index_id, repeat, src_stride, dst_gap, dst_frac_gap,
            src_frac_gap);
    }

    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID1);
}

__aicore__ inline void s1_split_bias_to_bt(__cbuf__ int32_t* src)
{
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID2);

    uint64_t dst = 0;
    uint16_t conv_control = 0;
    uint16_t n_burst = 1;
    uint16_t len_burst = static_cast<uint16_t>(S1_BIAS_BYTES / 32);
    uint16_t source_gap = 0;
    uint16_t dst_gap = 0;

    asc_copy_l12bt(dst, src, conv_control, n_burst, len_burst, source_gap, dst_gap);

    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID2);
}

// ========== 阶段3：Mmad ==========

__aicore__ inline void s1_compute_mmad(__cc__ int32_t* c_matrix, __ca__ int8_t* a_matrix, __cb__ int8_t* b_matrix)
{
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID1);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID2);

    uint16_t left_height = M;
    uint16_t n_dim = K;
    uint16_t right_width = ceil_align(N, BLOCK_CUBE * S1_FRACTAL_NUM);
    asc_unit_flag_mode unit_flag_mode = asc_unit_flag_mode::DISABLE;
    bool disable_gemv = true;
    bool c_matrix_source = true;
    bool c_matrix_init_val = false;

    asc_mmad(
        c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width, unit_flag_mode, disable_gemv, c_matrix_source,
        c_matrix_init_val);

    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
}

// ========== 阶段4：L0C → GM ==========

__aicore__ inline void s1_copy_result_to_gm(__gm__ int32_t* dst, __cc__ int32_t* src)
{
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);

    uint64_t nd_num = 1;
    uint64_t src_nd_stride = 0;
    uint64_t dst_nd_stride = 0;

    asc_set_l0c2gm_nz2nd(nd_num, src_nd_stride, dst_nd_stride);

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
