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

#define TEST_MMAD_OVERLOAD(c_type, a_type, b_type, b_instr_type, type_prefix, instr)                                \
    class TestMmad##type_prefix##CAPI : public testing::Test {                                                      \
    protected:                                                                                                      \
        void SetUp() override { g_coreType = C_API_AIC_TYPE; }                                                      \
        void TearDown() override { g_coreType = C_API_AIV_TYPE; }                                                   \
    };                                                                                                              \
                                                                                                                    \
    namespace {                                                                                                     \
    void mad_##type_prefix##_Stub(                                                                                  \
        __cc__ c_type* c_matrix, __ca__ a_type* a_matrix, __cb__ b_instr_type* b_matrix, uint16_t m, uint16_t k,    \
        uint16_t n, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)             \
    {                                                                                                               \
        EXPECT_EQ(c_matrix, reinterpret_cast<__cc__ c_type*>(1));                                                   \
        EXPECT_EQ(a_matrix, reinterpret_cast<__ca__ a_type*>(2));                                                   \
        EXPECT_EQ(b_matrix, reinterpret_cast<__cb__ b_instr_type*>(3));                                             \
        EXPECT_EQ(m, static_cast<uint16_t>(4));                                                                     \
        EXPECT_EQ(k, static_cast<uint16_t>(5));                                                                     \
        EXPECT_EQ(n, static_cast<uint16_t>(6));                                                                     \
        EXPECT_EQ(unit_flag, static_cast<uint8_t>(asc_unit_flag_mode::ENABLE_KEEP));                                \
        EXPECT_TRUE(disable_gemv);                                                                                  \
        EXPECT_FALSE(c_matrix_source);                                                                              \
        EXPECT_TRUE(c_matrix_init_val);                                                                             \
    }                                                                                                               \
                                                                                                                    \
    void mad_bias_##type_prefix##_Stub(                                                                             \
        __cc__ c_type* c_matrix, __ca__ a_type* a_matrix, __cb__ b_instr_type* b_matrix, uint16_t m, uint16_t k,    \
        uint16_t n, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)             \
    {                                                                                                               \
        constexpr uint64_t expected_xd = 0x8765432112345678ULL;                                                     \
        EXPECT_EQ(c_matrix, reinterpret_cast<__cc__ c_type*>(expected_xd));                                         \
        EXPECT_EQ(a_matrix, reinterpret_cast<__ca__ a_type*>(2));                                                   \
        EXPECT_EQ(b_matrix, reinterpret_cast<__cb__ b_instr_type*>(3));                                             \
        EXPECT_EQ(m, static_cast<uint16_t>(4));                                                                     \
        EXPECT_EQ(k, static_cast<uint16_t>(5));                                                                     \
        EXPECT_EQ(n, static_cast<uint16_t>(6));                                                                     \
        EXPECT_EQ(unit_flag, static_cast<uint8_t>(asc_unit_flag_mode::ENABLE_UPDATE));                              \
        EXPECT_TRUE(disable_gemv);                                                                                  \
        EXPECT_TRUE(c_matrix_source);                                                                               \
        EXPECT_FALSE(c_matrix_init_val);                                                                            \
    }                                                                                                               \
    }                                                                                                               \
                                                                                                                    \
    TEST_F(TestMmad##type_prefix##CAPI, MmadParamsSucc)                                                             \
    {                                                                                                               \
        __cc__ c_type* c_matrix = reinterpret_cast<__cc__ c_type*>(1);                                              \
        __ca__ a_type* a_matrix = reinterpret_cast<__ca__ a_type*>(2);                                              \
        __cb__ b_type* b_matrix = reinterpret_cast<__cb__ b_type*>(3);                                              \
                                                                                                                    \
        MOCKER_CPP(                                                                                                 \
            instr, void(                                                                                            \
                       __cc__ c_type*, __ca__ a_type*, __cb__ b_instr_type*, uint16_t, uint16_t, uint16_t, uint8_t, \
                       bool, bool, bool))                                                                           \
            .times(1)                                                                                               \
            .will(invoke(mad_##type_prefix##_Stub));                                                                \
                                                                                                                    \
        asc_mmad(c_matrix, a_matrix, b_matrix, 4, 5, 6, asc_unit_flag_mode::ENABLE_KEEP, true, false, true);        \
        GlobalMockObject::verify();                                                                                 \
    }                                                                                                               \
                                                                                                                    \
    TEST_F(TestMmad##type_prefix##CAPI, MmadBiasParamsSucc)                                                         \
    {                                                                                                               \
        __cc__ c_type* c_matrix = reinterpret_cast<__cc__ c_type*>(0x12345678ULL);                                  \
        __ca__ a_type* a_matrix = reinterpret_cast<__ca__ a_type*>(2);                                              \
        __cb__ b_type* b_matrix = reinterpret_cast<__cb__ b_type*>(3);                                              \
        constexpr uint64_t bias = 0x87654321ULL;                                                                    \
                                                                                                                    \
        MOCKER_CPP(                                                                                                 \
            instr, void(                                                                                            \
                       __cc__ c_type*, __ca__ a_type*, __cb__ b_instr_type*, uint16_t, uint16_t, uint16_t, uint8_t, \
                       bool, bool, bool))                                                                           \
            .times(1)                                                                                               \
            .will(invoke(mad_bias_##type_prefix##_Stub));                                                           \
                                                                                                                    \
        asc_mmad(c_matrix, a_matrix, b_matrix, bias, 4, 5, 6, asc_unit_flag_mode::ENABLE_UPDATE, true);             \
        GlobalMockObject::verify();                                                                                 \
    }

TEST_MMAD_OVERLOAD(float, bfloat16_t, bfloat16_t, bfloat16_t, Bfloat16Bfloat16, mad)
TEST_MMAD_OVERLOAD(float, fp8_e4m3fn_t, fp8_e4m3fn_t, fp8_e4m3fn_t, E4m3E4m3, mad)
TEST_MMAD_OVERLOAD(float, fp8_e4m3fn_t, fp8_e5m2_t, fp8_e5m2_t, E4m3E5m2, mad)
TEST_MMAD_OVERLOAD(float, fp8_e5m2_t, fp8_e4m3fn_t, fp8_e4m3fn_t, E5m2E4m3, mad)
TEST_MMAD_OVERLOAD(float, fp8_e5m2_t, fp8_e5m2_t, fp8_e5m2_t, E5m2E5m2, mad)
TEST_MMAD_OVERLOAD(float, half, half, half, HalfHalf, mad)
TEST_MMAD_OVERLOAD(float, float, float, float, FloatFloat, mad)
TEST_MMAD_OVERLOAD(int32_t, int8_t, int8_t, int8_t, Int8Int8, mad)
TEST_MMAD_OVERLOAD(float, hifloat8_t, hifloat8_t, hifloat8_t, Hifloat8Hifloat8, mad)
TEST_MMAD_OVERLOAD(float, half, fp8_e4m3fn_t, fp8_e4m3fn_t, HalfE4m3, mad)
TEST_MMAD_OVERLOAD(float, bfloat16_t, fp8_e4m3fn_t, fp8_e4m3fn_t, Bfloat16E4m3, mad)
TEST_MMAD_OVERLOAD(int32_t, int8_t, int4b_t, void, Int8Int4, mad_s8s4)
TEST_MMAD_OVERLOAD(float, half, int8_t, int8_t, HalfInt8, mad)
TEST_MMAD_OVERLOAD(float, bfloat16_t, int8_t, int8_t, Bfloat16Int8, mad)
TEST_MMAD_OVERLOAD(float, half, int4b_t, void, HalfInt4, mad_f16s4)
TEST_MMAD_OVERLOAD(float, bfloat16_t, int4b_t, void, Bfloat16Int4, mad_bf16s4)
