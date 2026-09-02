/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
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

#define TEST_MMAD_MX_BIAS(a_type, b_type, type_prefix)                                                                \
    class TestMmadMxBias##type_prefix##CAPI : public testing::Test {                                                  \
    protected:                                                                                                        \
        void SetUp() { g_coreType = C_API_AIC_TYPE; }                                                                 \
        void TearDown() { g_coreType = C_API_AIV_TYPE; }                                                              \
    };                                                                                                                \
                                                                                                                      \
    namespace {                                                                                                       \
    void mad_mx_bias_##type_prefix##_Stub(                                                                            \
        __cc__ float* c_matrix, __ca__ a_type* a_matrix, __cb__ b_type* b_matrix, uint16_t m, uint16_t k, uint16_t n, \
        uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)                           \
    {                                                                                                                 \
        constexpr uint64_t expected_xd = 0x8765432112345678ULL;                                                       \
        EXPECT_EQ(c_matrix, reinterpret_cast<__cc__ float*>(expected_xd));                                            \
        EXPECT_EQ(a_matrix, reinterpret_cast<__ca__ a_type*>(2));                                                     \
        EXPECT_EQ(b_matrix, reinterpret_cast<__cb__ b_type*>(3));                                                     \
        EXPECT_EQ(m, static_cast<uint16_t>(4));                                                                       \
        EXPECT_EQ(k, static_cast<uint16_t>(5));                                                                       \
        EXPECT_EQ(n, static_cast<uint16_t>(6));                                                                       \
        EXPECT_EQ(unit_flag, static_cast<uint8_t>(asc_unit_flag_mode::ENABLE_KEEP));                                  \
        EXPECT_FALSE(disable_gemv);                                                                                   \
        EXPECT_TRUE(c_matrix_source);                                                                                 \
        EXPECT_FALSE(c_matrix_init_val);                                                                              \
    }                                                                                                                 \
    }                                                                                                                 \
                                                                                                                      \
    TEST_F(TestMmadMxBias##type_prefix##CAPI, MmadMxBiasSucc)                                                         \
    {                                                                                                                 \
        __cc__ float* c_matrix = reinterpret_cast<__cc__ float*>(0x12345678ULL);                                      \
        __ca__ a_type* a_matrix = reinterpret_cast<__ca__ a_type*>(2);                                                \
        __cb__ b_type* b_matrix = reinterpret_cast<__cb__ b_type*>(3);                                                \
        constexpr uint64_t bias = 0x87654321ULL;                                                                      \
                                                                                                                      \
        MOCKER_CPP(                                                                                                   \
            mad_mx, void(                                                                                             \
                        __cc__ float*, __ca__ a_type*, __cb__ b_type*, uint16_t, uint16_t, uint16_t, uint8_t, bool,   \
                        bool, bool))                                                                                  \
            .times(1)                                                                                                 \
            .will(invoke(mad_mx_bias_##type_prefix##_Stub));                                                          \
                                                                                                                      \
        asc_mmad_mx(c_matrix, a_matrix, b_matrix, bias, 4, 5, 6, asc_unit_flag_mode::ENABLE_KEEP, false);             \
        GlobalMockObject::verify();                                                                                   \
    }

TEST_MMAD_MX_BIAS(fp4x2_e1m2_t, fp4x2_e1m2_t, e1m2_e1m2)
TEST_MMAD_MX_BIAS(fp4x2_e1m2_t, fp4x2_e2m1_t, e1m2_e2m1)
TEST_MMAD_MX_BIAS(fp4x2_e2m1_t, fp4x2_e1m2_t, e2m1_e1m2)
TEST_MMAD_MX_BIAS(fp4x2_e2m1_t, fp4x2_e2m1_t, e2m1_e2m1)
TEST_MMAD_MX_BIAS(fp8_e4m3fn_t, fp8_e4m3fn_t, e4m3_e4m3)
TEST_MMAD_MX_BIAS(fp8_e4m3fn_t, fp8_e5m2_t, e4m3_e5m2)
TEST_MMAD_MX_BIAS(fp8_e5m2_t, fp8_e4m3fn_t, e5m2_e4m3)
TEST_MMAD_MX_BIAS(fp8_e5m2_t, fp8_e5m2_t, e5m2_e5m2)
