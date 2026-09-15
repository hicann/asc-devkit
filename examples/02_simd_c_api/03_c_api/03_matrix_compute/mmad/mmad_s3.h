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
 * \file mmad_s3.h
 * \brief 场景3（float 输入/输出、A/B 转置、显式传入 Bias 地址）的常量与完整流水线函数
 */

#pragma once

// ========== 场景3常量 ==========
constexpr uint32_t S3_C0_SIZE = 8;
constexpr uint32_t S3_FRACTAL_NUM = 2;

// 转置后的 A 输入形状为[K, M]，转置后的 B 输入形状为[N, K]。
constexpr uint32_t S3_A_L1_SIZE = ceil_align(K, BLOCK_CUBE) * ceil_align(M, S3_C0_SIZE* S3_FRACTAL_NUM);
constexpr uint32_t S3_A_L0_SIZE = ceil_align(M, BLOCK_CUBE) * ceil_align(K, S3_C0_SIZE* S3_FRACTAL_NUM);
constexpr uint32_t S3_B_L1_SIZE = ceil_align(N, BLOCK_CUBE) * ceil_align(K, S3_C0_SIZE);
constexpr uint32_t S3_B_L0_SIZE = ceil_align(K, S3_C0_SIZE) * ceil_align(N, BLOCK_CUBE);
constexpr uint32_t S3_C_L0_SIZE = ceil_align(M, BLOCK_CUBE) * ceil_align(N, BLOCK_CUBE);
constexpr uint32_t S3_BIAS_BYTES = ceil_align(AscendC::Std::ceil_div(N * sizeof(float), 32), 2) * 32;
constexpr uint32_t S3_BIAS_ELEMS = S3_BIAS_BYTES / sizeof(float);

// ========== 阶段1：GM -> L1 ==========

__aicore__ inline void s3_copy_matrix_a_to_l1(__cbuf__ float* dst, __gm__ float* src)
{
    asc_set_gm2l1_nz_para(1, 1, ceil_align(K, BLOCK_CUBE), 0);
    asc_copy_gm2l1_nd2nz(dst, src, M * sizeof(float), asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, K, M, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
}

__aicore__ inline void s3_copy_matrix_b_to_l1(__cbuf__ float* dst, __gm__ float* src)
{
    asc_set_gm2l1_nz_para(1, 1, ceil_align(N, BLOCK_CUBE), 0);
    asc_copy_gm2l1_nd2nz(dst, src, K * sizeof(float), asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, N, K, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID1);
}

__aicore__ inline void s3_copy_bias_to_l1(__cbuf__ float* dst, __gm__ float* src)
{
    asc_set_gm2l1_nz_para(1, 1, 1, 0);
    asc_copy_gm2l1_nd2nz(dst, src, N * sizeof(float), asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, 1, N, 0, false);
    asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID2);
}

// ========== 阶段2：L1 -> L0A / L0B / BT ==========

__aicore__ inline void s3_split_matrix_a_to_l0a_transpose(__ca__ float* dst, __cbuf__ float* src)
{
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0);
    asc_copy_l12l0a_transpose(
        dst, src, 0, 0, AscendC::Std::ceil_div(K, BLOCK_CUBE),
        ceil_align(AscendC::Std::ceil_div(M, S3_C0_SIZE), S3_FRACTAL_NUM), AscendC::Std::ceil_div(K, BLOCK_CUBE),
        AscendC::Std::ceil_div(M, S3_C0_SIZE * S3_FRACTAL_NUM));
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0);
}

__aicore__ inline void s3_split_matrix_b_to_l0b(__cb__ float* dst, __cbuf__ float* src)
{
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID1);
    asc_copy_l12l0b(
        dst, src, 0, 0, AscendC::Std::ceil_div(N, BLOCK_CUBE), AscendC::Std::ceil_div(K, S3_C0_SIZE),
        AscendC::Std::ceil_div(N, BLOCK_CUBE), AscendC::Std::ceil_div(N, BLOCK_CUBE));
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID1);
}

__aicore__ inline void s3_split_bias_to_bt(__cbuf__ float* src)
{
    asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID2);
    asc_copy_l12bt(0, src, S3_BIAS_BYTES);
    asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID2);
}

// ========== 阶段3：Mmad ==========

__aicore__ inline void s3_compute_mmad(__cc__ float* c_matrix, __ca__ float* a_matrix, __cb__ float* b_matrix)
{
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID1);
    asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID2);

    uint64_t bias = 0;
    bool disable_gemv = false;
    asc_mmad(c_matrix, a_matrix, b_matrix, bias, M, K, N, asc_unit_flag_mode::DISABLE, disable_gemv);
    asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0);
}

// ========== 阶段4：L0C -> GM ==========

__aicore__ inline void s3_copy_result_to_gm(__gm__ float* dst, __cc__ float* src)
{
    asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0);
    asc_set_l0c2gm_nz2nd(1, 0, 0);
    asc_copy_l0c2gm(
        dst, src, N, M, N, ceil_align(M, BLOCK_CUBE), 0, 0, 0, 0, 0, false, true, 0, 0, false, 0, false, false, false,
        false);
}
