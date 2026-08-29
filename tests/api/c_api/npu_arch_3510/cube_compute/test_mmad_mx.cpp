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

#define TEST_MMAD_MX_INSTR(class_name, c_api_name, cce_name, dst_type, src0_type, src1_type)                         \
                                                                                                                     \
    class TestVectorCompute##class_name##_##src0_type##_##src1_type##_CApi : public testing::Test {                  \
    protected:                                                                                                       \
        void SetUp() { g_coreType = C_API_AIC_TYPE; }                                                                \
        void TearDown() { g_coreType = C_API_AIV_TYPE; }                                                             \
    };                                                                                                               \
                                                                                                                     \
    namespace {                                                                                                      \
    uint8_t expected_unit_flag_##src0_type##_##src1_type = 0;                                                        \
    void cce_name##_##src0_type##_##src1_type##_Stub(                                                                \
        dst_type* dst, src0_type* src0, src1_type* src1, uint16_t left_height, uint16_t n_dim, uint16_t right_width, \
        uint8_t unitFlag, bool disableGemv, bool cMatrixSource, bool cMatrixInitVal)                                 \
    {                                                                                                                \
        EXPECT_EQ(unitFlag, expected_unit_flag_##src0_type##_##src1_type);                                           \
    }                                                                                                                \
    }                                                                                                                \
                                                                                                                     \
    TEST_F(                                                                                                          \
        TestVectorCompute##class_name##_##src0_type##_##src1_type##_CApi,                                            \
        c_api_name##_##src0_type##_##src1_type##_Succ)                                                               \
    {                                                                                                                \
        dst_type* dst;                                                                                               \
        src0_type* src0;                                                                                             \
        src1_type* src1;                                                                                             \
        uint16_t left_height = 64;                                                                                   \
        uint16_t right_width = 64;                                                                                   \
        uint16_t n_dim = 64;                                                                                         \
        uint8_t unitFlag = 0;                                                                                        \
        bool disableGemv = true;                                                                                     \
        bool cMatrixSource = false;                                                                                  \
        bool cMatrixInitVal = false;                                                                                 \
                                                                                                                     \
        expected_unit_flag_##src0_type##_##src1_type = 0;                                                            \
        MOCKER_CPP(                                                                                                  \
            cce_name,                                                                                                \
            void(dst_type*, src0_type*, src1_type*, uint16_t, uint16_t, uint16_t, uint8_t, bool, bool, bool))        \
            .times(1)                                                                                                \
            .will(invoke(cce_name##_##src0_type##_##src1_type##_Stub));                                              \
                                                                                                                     \
        c_api_name(                                                                                                  \
            dst, src0, src1, left_height, n_dim, right_width, unitFlag, disableGemv, cMatrixSource, cMatrixInitVal); \
        GlobalMockObject::verify();                                                                                  \
    }                                                                                                                \
                                                                                                                     \
    TEST_F(                                                                                                          \
        TestVectorCompute##class_name##_##src0_type##_##src1_type##_CApi,                                            \
        c_api_name##_enum_##src0_type##_##src1_type##_Succ)                                                          \
    {                                                                                                                \
        dst_type* dst;                                                                                               \
        src0_type* src0;                                                                                             \
        src1_type* src1;                                                                                             \
        uint16_t left_height = 64;                                                                                   \
        uint16_t right_width = 64;                                                                                   \
        uint16_t n_dim = 64;                                                                                         \
        asc_unit_flag_mode unitFlag = asc_unit_flag_mode::ENABLE_KEEP;                                               \
        bool disableGemv = true;                                                                                     \
        bool cMatrixSource = false;                                                                                  \
        bool cMatrixInitVal = false;                                                                                 \
                                                                                                                     \
        expected_unit_flag_##src0_type##_##src1_type = 2;                                                            \
        MOCKER_CPP(                                                                                                  \
            cce_name,                                                                                                \
            void(dst_type*, src0_type*, src1_type*, uint16_t, uint16_t, uint16_t, uint8_t, bool, bool, bool))        \
            .times(1)                                                                                                \
            .will(invoke(cce_name##_##src0_type##_##src1_type##_Stub));                                              \
                                                                                                                     \
        c_api_name(                                                                                                  \
            dst, src0, src1, left_height, n_dim, right_width, unitFlag, disableGemv, cMatrixSource, cMatrixInitVal); \
        GlobalMockObject::verify();                                                                                  \
    }

TEST_MMAD_MX_INSTR(Mmadmx, asc_mmad_mx, mad_mx, float, fp4x2_e1m2_t, fp4x2_e1m2_t)
TEST_MMAD_MX_INSTR(Mmadmx, asc_mmad_mx, mad_mx, float, fp4x2_e1m2_t, fp4x2_e2m1_t)
TEST_MMAD_MX_INSTR(Mmadmx, asc_mmad_mx, mad_mx, float, fp4x2_e2m1_t, fp4x2_e1m2_t)
TEST_MMAD_MX_INSTR(Mmadmx, asc_mmad_mx, mad_mx, float, fp4x2_e2m1_t, fp4x2_e2m1_t)
TEST_MMAD_MX_INSTR(Mmadmx, asc_mmad_mx, mad_mx, float, fp8_e4m3fn_t, fp8_e4m3fn_t)
TEST_MMAD_MX_INSTR(Mmadmx, asc_mmad_mx, mad_mx, float, fp8_e4m3fn_t, fp8_e5m2_t)
TEST_MMAD_MX_INSTR(Mmadmx, asc_mmad_mx, mad_mx, float, fp8_e5m2_t, fp8_e4m3fn_t)
TEST_MMAD_MX_INSTR(Mmadmx, asc_mmad_mx, mad_mx, float, fp8_e5m2_t, fp8_e5m2_t)

#pragma GCC diagnostic pop
