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

#define TEST_CUBE_DATAMOVE_L0C2UB(class_name, c_api_name, cce_name, dst_data_type, src_data_type)                   \
    TEST_F(TestCubeDatamoveCopyL0C2UB, c_api_name##_##dst_data_type##_##src_data_type##_Succ)                       \
    {                                                                                                               \
        __ubuf__ dst_data_type* dst = reinterpret_cast<__ubuf__ dst_data_type*>(1);                                 \
        __cc__ src_data_type* src = reinterpret_cast<__cc__ src_data_type*>(2);                                     \
        uint16_t n_size = 3;                                                                                        \
        uint16_t m_size = 4;                                                                                        \
        uint32_t dst_stride = 6;                                                                                    \
        uint16_t src_stride = 7;                                                                                    \
        int8_t sub_blockid = 0;                                                                                     \
        asc_dual_dst_mode dual_dst_ctrl = asc_dual_dst_mode::DUAL_DST_DISABLE;                                      \
        asc_unit_flag_mode unit_flag_mode = asc_unit_flag_mode::DISABLE;                                            \
        asc_quant_mode quant_pre_mode = asc_quant_mode::NoQuant;                                                    \
        asc_relu_pre_mode relu_pre_mode = asc_relu_pre_mode::NONE;                                                  \
        bool enable_channel_split = true;                                                                           \
        bool enable_nz2nd = true;                                                                                   \
        bool enable_nz2dn = false;                                                                                  \
        bool enable_clip_relu_pre = false;                                                                          \
        c_api_name(                                                                                                 \
            dst, src, n_size, m_size, dst_stride, src_stride, sub_blockid, dual_dst_ctrl, unit_flag_mode,           \
            quant_pre_mode, relu_pre_mode, enable_channel_split, enable_nz2nd, enable_nz2dn, enable_clip_relu_pre); \
        GlobalMockObject::verify();                                                                                 \
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
