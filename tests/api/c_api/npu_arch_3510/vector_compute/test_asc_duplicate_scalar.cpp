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
#include "tests/api/c_api/stub/cce_stub.h"
#include "include/c_api/asc_simd.h"

#define TEST_VECTOR_COMPUTE_DUPLICATE_SCALAR_INSTR(class_name, c_api_name, cce_name, data_type)                    \
                                                                                                                   \
    class TestVectorCompute##class_name####data_type##CApi : public testing::Test {                                \
    protected:                                                                                                     \
        void SetUp() {}                                                                                            \
        void TearDown() {}                                                                                         \
    };                                                                                                             \
                                                                                                                   \
    namespace {                                                                                                    \
    void cce_name##_##data_type##_Stub(vector_##data_type& dst, data_type src0, vector_bool mask, Literal mode) {} \
    }                                                                                                              \
                                                                                                                   \
    TEST_F(TestVectorCompute##class_name####data_type##CApi, c_api_name##_##data_type##_Succ)                      \
    {                                                                                                              \
        vector_##data_type dst;                                                                                    \
        data_type src0;                                                                                            \
        vector_bool mask;                                                                                          \
                                                                                                                   \
        MOCKER_CPP(cce_name, void(vector_##data_type&, data_type, vector_bool, Literal))                           \
            .times(1)                                                                                              \
            .will(invoke(cce_name##_##data_type##_Stub));                                                          \
                                                                                                                   \
        c_api_name(dst, src0, mask);                                                                               \
        GlobalMockObject::verify();                                                                                \
    }

TEST_VECTOR_COMPUTE_DUPLICATE_SCALAR_INSTR(Vdups, asc_duplicate_scalar, vdup, uint8_t);
TEST_VECTOR_COMPUTE_DUPLICATE_SCALAR_INSTR(Vdups, asc_duplicate_scalar, vdup, int8_t);
TEST_VECTOR_COMPUTE_DUPLICATE_SCALAR_INSTR(Vdups, asc_duplicate_scalar, vdup, fp8_e4m3fn_t);
TEST_VECTOR_COMPUTE_DUPLICATE_SCALAR_INSTR(Vdups, asc_duplicate_scalar, vdup, fp8_e5m2_t);
TEST_VECTOR_COMPUTE_DUPLICATE_SCALAR_INSTR(Vdups, asc_duplicate_scalar, vdup, uint16_t);
TEST_VECTOR_COMPUTE_DUPLICATE_SCALAR_INSTR(Vdups, asc_duplicate_scalar, vdup, int16_t);
TEST_VECTOR_COMPUTE_DUPLICATE_SCALAR_INSTR(Vdups, asc_duplicate_scalar, vdup, half);
TEST_VECTOR_COMPUTE_DUPLICATE_SCALAR_INSTR(Vdups, asc_duplicate_scalar, vdup, bfloat16_t);
TEST_VECTOR_COMPUTE_DUPLICATE_SCALAR_INSTR(Vdups, asc_duplicate_scalar, vdup, uint32_t);
TEST_VECTOR_COMPUTE_DUPLICATE_SCALAR_INSTR(Vdups, asc_duplicate_scalar, vdup, int32_t);
TEST_VECTOR_COMPUTE_DUPLICATE_SCALAR_INSTR(Vdups, asc_duplicate_scalar, vdup, float);

#define TEST_VECTOR_COMPUTE_DUPLICATE_SCALAR_VBR_INSTR(class_name, c_api_name, cce_name, data_type) \
                                                                                                    \
    class TestVectorCompute##class_name####data_type##CApi : public testing::Test {                 \
    protected:                                                                                      \
        void SetUp() {}                                                                             \
        void TearDown() {}                                                                          \
    };                                                                                              \
                                                                                                    \
    namespace {                                                                                     \
    void cce_name##_##data_type##_Stub(vector_##data_type& dst, data_type value) {}                 \
    }                                                                                               \
                                                                                                    \
    TEST_F(TestVectorCompute##class_name####data_type##CApi, c_api_name##_##data_type##_Succ)       \
    {                                                                                               \
        vector_##data_type dst;                                                                     \
        data_type value;                                                                            \
                                                                                                    \
        MOCKER_CPP(cce_name, void(vector_##data_type&, data_type))                                  \
            .times(1)                                                                               \
            .will(invoke(cce_name##_##data_type##_Stub));                                           \
                                                                                                    \
        c_api_name(dst, value);                                                                     \
        GlobalMockObject::verify();                                                                 \
    }

TEST_VECTOR_COMPUTE_DUPLICATE_SCALAR_VBR_INSTR(Vbr, asc_duplicate_scalar, vbr, uint8_t);
TEST_VECTOR_COMPUTE_DUPLICATE_SCALAR_VBR_INSTR(Vbr, asc_duplicate_scalar, vbr, int8_t);
TEST_VECTOR_COMPUTE_DUPLICATE_SCALAR_VBR_INSTR(Vbr, asc_duplicate_scalar, vbr, uint16_t);
TEST_VECTOR_COMPUTE_DUPLICATE_SCALAR_VBR_INSTR(Vbr, asc_duplicate_scalar, vbr, int16_t);
TEST_VECTOR_COMPUTE_DUPLICATE_SCALAR_VBR_INSTR(Vbr, asc_duplicate_scalar, vbr, uint32_t);
TEST_VECTOR_COMPUTE_DUPLICATE_SCALAR_VBR_INSTR(Vbr, asc_duplicate_scalar, vbr, int32_t);
TEST_VECTOR_COMPUTE_DUPLICATE_SCALAR_VBR_INSTR(Vbr, asc_duplicate_scalar, vbr, half);
TEST_VECTOR_COMPUTE_DUPLICATE_SCALAR_VBR_INSTR(Vbr, asc_duplicate_scalar, vbr, float);
TEST_VECTOR_COMPUTE_DUPLICATE_SCALAR_VBR_INSTR(Vbr, asc_duplicate_scalar, vbr, bfloat16_t);
TEST_VECTOR_COMPUTE_DUPLICATE_SCALAR_VBR_INSTR(Vbr, asc_duplicate_scalar, vbr, fp8_e4m3fn_t);
TEST_VECTOR_COMPUTE_DUPLICATE_SCALAR_VBR_INSTR(Vbr, asc_duplicate_scalar, vbr, fp8_e5m2_t);
TEST_VECTOR_COMPUTE_DUPLICATE_SCALAR_VBR_INSTR(Vbr, asc_duplicate_scalar, vbr, fp8_e8m0_t);
