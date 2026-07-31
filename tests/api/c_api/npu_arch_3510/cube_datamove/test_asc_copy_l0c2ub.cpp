/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef TESTS_UNIT_BASIC_API_UT_TESTCASE_C_API_NPU_ARCH_3510_UTILS_TEST_FIXPIPE_L0C2UB_INSTR_H
#define TESTS_UNIT_BASIC_API_UT_TESTCASE_C_API_NPU_ARCH_3510_UTILS_TEST_FIXPIPE_L0C2UB_INSTR_H

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "c_api/stub/cce_stub.h"
#include "c_api/asc_simd.h"

class TestCubeDatamoveCopyL0C2UB : public testing::Test {
protected:
    void SetUp() { g_coreType = C_API_AIC_TYPE; }
    void TearDown() { g_coreType = C_API_AIV_TYPE; }
};

#define TEST_CUBE_DATAMOVE_L0C2UB(class_name, c_api_name, cce_name, dst_data_type, src_data_type)                     \
    TEST_F(TestCubeDatamoveCopyL0C2UB, c_api_name##_##dst_data_type##_##src_data_type##_Succ)                         \
    {                                                                                                                 \
        __ubuf__ dst_data_type* dst = reinterpret_cast<__ubuf__ dst_data_type*>(1);                                   \
        __cc__ src_data_type* src = reinterpret_cast<__cc__ src_data_type*>(2);                                       \
        uint16_t n_size = 3;                                                                                          \
        uint16_t m_size = 4;                                                                                          \
        uint32_t dst_stride = 6;                                                                                      \
        uint16_t src_stride = 7;                                                                                      \
        uint8_t dual_dst_ctrl = 5;                                                                                    \
        bool sub_blockid = true;                                                                                      \
        uint8_t enable_clip_relu_pre = 8;                                                                             \
        uint8_t unit_flag_mode = 10;                                                                                  \
        uint64_t quant_pre_mode = 11;                                                                                 \
        uint8_t relu_pre_mode = 12;                                                                                   \
        bool enable_channel_split = true;                                                                             \
        bool enable_nz2nd = true;                                                                                     \
        uint64_t quant_post = 13;                                                                                     \
        uint8_t relu_post = 14;                                                                                       \
        bool clip_relu_post = true;                                                                                   \
        uint8_t eltwise_op = 15;                                                                                      \
        bool eltwise_antq_en = true;                                                                                  \
        bool c0_pad_en = true;                                                                                        \
        bool broadcast_en = false;                                                                                    \
        bool enable_nz2dn = false;                                                                                    \
        c_api_name(                                                                                                   \
            dst, src, n_size, m_size, dst_stride, src_stride, dual_dst_ctrl, sub_blockid, enable_clip_relu_pre,       \
            unit_flag_mode, quant_pre_mode, relu_pre_mode, enable_channel_split, enable_nz2nd, quant_post, relu_post, \
            clip_relu_post, eltwise_op, eltwise_antq_en, c0_pad_en, broadcast_en, enable_nz2dn);                      \
        GlobalMockObject::verify();                                                                                   \
    }

#endif

TEST_CUBE_DATAMOVE_L0C2UB(L0C2UB, asc_copy_l0c2ub, copy_matrix_cc_to_ub, half, float);
TEST_CUBE_DATAMOVE_L0C2UB(L0C2UB, asc_copy_l0c2ub, copy_matrix_cc_to_ub, bfloat16_t, float);
TEST_CUBE_DATAMOVE_L0C2UB(L0C2UB, asc_copy_l0c2ub, copy_matrix_cc_to_ub, float8_e4m3_t, float);
TEST_CUBE_DATAMOVE_L0C2UB(L0C2UB, asc_copy_l0c2ub, copy_matrix_cc_to_ub, hifloat8_t, float);
TEST_CUBE_DATAMOVE_L0C2UB(L0C2UB, asc_copy_l0c2ub, copy_matrix_cc_to_ub, int8_t, float);
TEST_CUBE_DATAMOVE_L0C2UB(L0C2UB, asc_copy_l0c2ub, copy_matrix_cc_to_ub, uint8_t, float);
TEST_CUBE_DATAMOVE_L0C2UB(L0C2UB, asc_copy_l0c2ub, copy_matrix_cc_to_ub, float, float);
TEST_CUBE_DATAMOVE_L0C2UB(L0C2UB, asc_copy_l0c2ub, copy_matrix_cc_to_ub, half, int32_t);
TEST_CUBE_DATAMOVE_L0C2UB(L0C2UB, asc_copy_l0c2ub, copy_matrix_cc_to_ub, bfloat16_t, int32_t);
TEST_CUBE_DATAMOVE_L0C2UB(L0C2UB, asc_copy_l0c2ub, copy_matrix_cc_to_ub, int8_t, int32_t);
TEST_CUBE_DATAMOVE_L0C2UB(L0C2UB, asc_copy_l0c2ub, copy_matrix_cc_to_ub, uint8_t, int32_t);
TEST_CUBE_DATAMOVE_L0C2UB(L0C2UB, asc_copy_l0c2ub, copy_matrix_cc_to_ub, int32_t, int32_t);
TEST_CUBE_DATAMOVE_L0C2UB(L0C2UB, asc_copy_l0c2ub, copy_matrix_cc_to_ub_s4, int4b_t, float);
TEST_CUBE_DATAMOVE_L0C2UB(L0C2UB, asc_copy_l0c2ub, copy_matrix_cc_to_ub_s4, int4b_t, int32_t);
