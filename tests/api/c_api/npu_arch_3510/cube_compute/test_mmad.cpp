/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "c_api/stub/cce_stub.h"
#include "c_api/asc_simd.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#define TEST_CUBE_COMPUTE_MMAD_INSTR(c_type, a_type, b_type, type_prefix)                                              \
                                                                                                                       \
    class TestMmad##type_prefix##CAPI : public testing::Test {                                                         \
    protected:                                                                                                         \
        void SetUp() { g_coreType = C_API_AIC_TYPE; }                                                                  \
        void TearDown() { g_coreType = C_API_AIV_TYPE; }                                                               \
    };                                                                                                                 \
                                                                                                                       \
    namespace {                                                                                                        \
    uint8_t expected_unit_flag_##type_prefix = 1;                                                                      \
    void mad_##type_prefix##_params_Stub(                                                                              \
        __cc__ c_type* c_matrix, __ca__ a_type* a_matrix, __cb__ b_type* b_matrix, uint16_t left_height,               \
        uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,              \
        bool c_matrix_init_val)                                                                                        \
    {                                                                                                                  \
        EXPECT_EQ(c_matrix, reinterpret_cast<__cc__ c_type*>(1));                                                      \
        EXPECT_EQ(a_matrix, reinterpret_cast<__ca__ a_type*>(2));                                                      \
        EXPECT_EQ(b_matrix, reinterpret_cast<__cb__ b_type*>(3));                                                      \
        EXPECT_EQ(left_height, static_cast<uint16_t>(4));                                                              \
        EXPECT_EQ(n_dim, static_cast<uint16_t>(5));                                                                    \
        EXPECT_EQ(right_width, static_cast<uint16_t>(6));                                                              \
        EXPECT_EQ(unit_flag, expected_unit_flag_##type_prefix);                                                        \
        EXPECT_EQ(disable_gemv, true);                                                                                 \
        EXPECT_EQ(c_matrix_source, false);                                                                             \
        EXPECT_EQ(c_matrix_init_val, true);                                                                            \
    }                                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    TEST_F(TestMmad##type_prefix##CAPI, mmad_##type_prefix##_params_Succ)                                              \
    {                                                                                                                  \
        __cc__ c_type* c_matrix = reinterpret_cast<__cc__ c_type*>(1);                                                 \
        __ca__ a_type* a_matrix = reinterpret_cast<__ca__ a_type*>(2);                                                 \
        __cb__ b_type* b_matrix = reinterpret_cast<__cb__ b_type*>(3);                                                 \
        uint16_t left_height = 4;                                                                                      \
        uint16_t n_dim = 5;                                                                                            \
        uint16_t right_width = 6;                                                                                      \
        uint8_t unit_flag = 1;                                                                                         \
        bool disable_gemv = true;                                                                                      \
        bool c_matrix_source = false;                                                                                  \
        bool c_matrix_init_val = true;                                                                                 \
                                                                                                                       \
        expected_unit_flag_##type_prefix = 1;                                                                          \
        MOCKER_CPP(                                                                                                    \
            mad, void(                                                                                                 \
                     __cc__ c_type * c_matrix, __ca__ a_type * a_matrix, __cb__ b_type * b_matrix,                     \
                     uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, \
                     bool c_matrix_source, bool c_matrix_init_val))                                                    \
            .times(1)                                                                                                  \
            .will(invoke(mad_##type_prefix##_params_Stub));                                                            \
                                                                                                                       \
        asc_mmad(                                                                                                      \
            c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width, unit_flag, disable_gemv, c_matrix_source,   \
            c_matrix_init_val);                                                                                        \
        GlobalMockObject::verify();                                                                                    \
    }                                                                                                                  \
                                                                                                                       \
    TEST_F(TestMmad##type_prefix##CAPI, mmad_enum_##type_prefix##_params_Succ)                                         \
    {                                                                                                                  \
        __cc__ c_type* c_matrix = reinterpret_cast<__cc__ c_type*>(1);                                                 \
        __ca__ a_type* a_matrix = reinterpret_cast<__ca__ a_type*>(2);                                                 \
        __cb__ b_type* b_matrix = reinterpret_cast<__cb__ b_type*>(3);                                                 \
        uint16_t left_height = 4;                                                                                      \
        uint16_t n_dim = 5;                                                                                            \
        uint16_t right_width = 6;                                                                                      \
        asc_unit_flag_mode unit_flag = asc_unit_flag_mode::ENABLE_KEEP;                                                \
        bool disable_gemv = true;                                                                                      \
        bool c_matrix_source = false;                                                                                  \
        bool c_matrix_init_val = true;                                                                                 \
                                                                                                                       \
        expected_unit_flag_##type_prefix = 2;                                                                          \
        MOCKER_CPP(                                                                                                    \
            mad, void(                                                                                                 \
                     __cc__ c_type * c_matrix, __ca__ a_type * a_matrix, __cb__ b_type * b_matrix,                     \
                     uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, \
                     bool c_matrix_source, bool c_matrix_init_val))                                                    \
            .times(1)                                                                                                  \
            .will(invoke(mad_##type_prefix##_params_Stub));                                                            \
                                                                                                                       \
        asc_mmad(                                                                                                      \
            c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width, unit_flag, disable_gemv, c_matrix_source,   \
            c_matrix_init_val);                                                                                        \
        GlobalMockObject::verify();                                                                                    \
    }                                                                                                                  \
                                                                                                                       \
    TEST_F(TestMmad##type_prefix##CAPI, mmad_sync_##type_prefix##_params_Succ)                                         \
    {                                                                                                                  \
        __cc__ c_type* c_matrix = reinterpret_cast<__cc__ c_type*>(1);                                                 \
        __ca__ a_type* a_matrix = reinterpret_cast<__ca__ a_type*>(2);                                                 \
        __cb__ b_type* b_matrix = reinterpret_cast<__cb__ b_type*>(3);                                                 \
        uint16_t left_height = 4;                                                                                      \
        uint16_t n_dim = 5;                                                                                            \
        uint16_t right_width = 6;                                                                                      \
        uint8_t unit_flag = 1;                                                                                         \
        bool disable_gemv = true;                                                                                      \
        bool c_matrix_source = false;                                                                                  \
        bool c_matrix_init_val = true;                                                                                 \
                                                                                                                       \
        expected_unit_flag_##type_prefix = 1;                                                                          \
        MOCKER_CPP(                                                                                                    \
            mad, void(                                                                                                 \
                     __cc__ c_type * c_matrix, __ca__ a_type * a_matrix, __cb__ b_type * b_matrix,                     \
                     uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, \
                     bool c_matrix_source, bool c_matrix_init_val))                                                    \
            .times(1)                                                                                                  \
            .will(invoke(mad_##type_prefix##_params_Stub));                                                            \
                                                                                                                       \
        asc_mmad_sync(                                                                                                 \
            c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width, unit_flag, disable_gemv, c_matrix_source,   \
            c_matrix_init_val);                                                                                        \
        GlobalMockObject::verify();                                                                                    \
    }

TEST_CUBE_COMPUTE_MMAD_INSTR(float, bfloat16_t, bfloat16_t, bfloat16);
TEST_CUBE_COMPUTE_MMAD_INSTR(float, float8_e4m3_t, float8_e4m3_t, e4m3_e4m3);
TEST_CUBE_COMPUTE_MMAD_INSTR(float, float8_e4m3_t, float8_e5m2_t, e4m3_e5m2);
TEST_CUBE_COMPUTE_MMAD_INSTR(float, float8_e5m2_t, float8_e4m3_t, e5m2_e4m3);
TEST_CUBE_COMPUTE_MMAD_INSTR(float, float8_e5m2_t, float8_e5m2_t, e5m2_e5m2);
TEST_CUBE_COMPUTE_MMAD_INSTR(float, half, half, half);
TEST_CUBE_COMPUTE_MMAD_INSTR(float, float, float, float);
TEST_CUBE_COMPUTE_MMAD_INSTR(int32_t, int8_t, int8_t, int8);
TEST_CUBE_COMPUTE_MMAD_INSTR(float, hifloat8_t, hifloat8_t, hif8);

#pragma GCC diagnostic pop
