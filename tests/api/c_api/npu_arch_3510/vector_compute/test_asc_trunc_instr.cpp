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

#define TEST_VECTOR_COMPUTE_VCONV_TRUNC_INSTR(class_name, c_api_name, cce_name, dst_type, src_type)              \
                                                                                                                 \
    class TestVectorCompute##c_api_name##dst_type##src_type##CApi : public testing::Test {                       \
    protected:                                                                                                   \
        void SetUp() {}                                                                                          \
        void TearDown() {}                                                                                       \
    };                                                                                                           \
                                                                                                                 \
    namespace {                                                                                                  \
    void c_api_name##cce_name##_##dst_type##_##src_type##_Stub(                                                  \
        dst_type& dst, src_type src, int32_t part, vector_bool mask, int32_t mode)                               \
    {}                                                                                                           \
    }                                                                                                            \
                                                                                                                 \
    TEST_F(TestVectorCompute##c_api_name##dst_type##src_type##CApi, c_api_name##_##dst_type##_##src_type##_Succ) \
    {                                                                                                            \
        dst_type dst;                                                                                            \
        src_type src;                                                                                            \
        vector_bool mask;                                                                                        \
                                                                                                                 \
        MOCKER_CPP(cce_name, void(dst_type&, src_type, int32_t, vector_bool, int32_t))                           \
            .times(1)                                                                                            \
            .will(invoke(c_api_name##cce_name##_##dst_type##_##src_type##_Stub));                                \
                                                                                                                 \
        c_api_name(dst, src, mask);                                                                              \
        GlobalMockObject::verify();                                                                              \
    }

#define TEST_VECTOR_COMPUTE_VCONV_TRUNC_RETURN_INSTR(class_name, c_api_name, cce_name, dst_type, src_type) \
                                                                                                           \
    class TestVectorComputeReturn##c_api_name##dst_type##src_type##CApi : public testing::Test {           \
    protected:                                                                                             \
        void SetUp() {}                                                                                    \
        void TearDown() {}                                                                                 \
    };                                                                                                     \
                                                                                                           \
    namespace {                                                                                            \
    void c_api_name##cce_name##Return_##dst_type##_##src_type##_Stub(                                      \
        dst_type& dst, src_type src, int32_t part, vector_bool mask, int32_t mode)                         \
    {}                                                                                                     \
    }                                                                                                      \
                                                                                                           \
    TEST_F(                                                                                                \
        TestVectorComputeReturn##c_api_name##dst_type##src_type##CApi,                                     \
        c_api_name##_return_##dst_type##_##src_type##_Succ)                                                \
    {                                                                                                      \
        src_type src;                                                                                      \
        vector_bool mask;                                                                                  \
                                                                                                           \
        MOCKER_CPP(cce_name, void(dst_type&, src_type, int32_t, vector_bool, int32_t))                     \
            .times(1)                                                                                      \
            .will(invoke(c_api_name##cce_name##Return_##dst_type##_##src_type##_Stub));                    \
                                                                                                           \
        dst_type dst = c_api_name(src, mask);                                                              \
        (void)dst;                                                                                         \
        GlobalMockObject::verify();                                                                        \
    }

// ==========asc_ceil==========
TEST_VECTOR_COMPUTE_VCONV_TRUNC_INSTR(ASCTRUNC, asc_ceil, vtrc, vector_half, vector_half);
TEST_VECTOR_COMPUTE_VCONV_TRUNC_INSTR(ASCTRUNC, asc_ceil, vtrc, vector_bfloat16_t, vector_bfloat16_t);
TEST_VECTOR_COMPUTE_VCONV_TRUNC_INSTR(ASCTRUNC, asc_ceil, vtrc, vector_float, vector_float);
TEST_VECTOR_COMPUTE_VCONV_TRUNC_RETURN_INSTR(ASCTRUNC, asc_ceil, vtrc, vector_half, vector_half);
TEST_VECTOR_COMPUTE_VCONV_TRUNC_RETURN_INSTR(ASCTRUNC, asc_ceil, vtrc, vector_bfloat16_t, vector_bfloat16_t);
TEST_VECTOR_COMPUTE_VCONV_TRUNC_RETURN_INSTR(ASCTRUNC, asc_ceil, vtrc, vector_float, vector_float);
// ==========asc_floor==========
TEST_VECTOR_COMPUTE_VCONV_TRUNC_INSTR(ASCTRUNC, asc_floor, vtrc, vector_half, vector_half);
TEST_VECTOR_COMPUTE_VCONV_TRUNC_INSTR(ASCTRUNC, asc_floor, vtrc, vector_bfloat16_t, vector_bfloat16_t);
TEST_VECTOR_COMPUTE_VCONV_TRUNC_INSTR(ASCTRUNC, asc_floor, vtrc, vector_float, vector_float);
TEST_VECTOR_COMPUTE_VCONV_TRUNC_RETURN_INSTR(ASCTRUNC, asc_floor, vtrc, vector_half, vector_half);
TEST_VECTOR_COMPUTE_VCONV_TRUNC_RETURN_INSTR(ASCTRUNC, asc_floor, vtrc, vector_bfloat16_t, vector_bfloat16_t);
TEST_VECTOR_COMPUTE_VCONV_TRUNC_RETURN_INSTR(ASCTRUNC, asc_floor, vtrc, vector_float, vector_float);
// ==========asc_rint==========
TEST_VECTOR_COMPUTE_VCONV_TRUNC_INSTR(ASCTRUNC, asc_rint, vtrc, vector_half, vector_half);
TEST_VECTOR_COMPUTE_VCONV_TRUNC_INSTR(ASCTRUNC, asc_rint, vtrc, vector_bfloat16_t, vector_bfloat16_t);
TEST_VECTOR_COMPUTE_VCONV_TRUNC_INSTR(ASCTRUNC, asc_rint, vtrc, vector_float, vector_float);
TEST_VECTOR_COMPUTE_VCONV_TRUNC_RETURN_INSTR(ASCTRUNC, asc_rint, vtrc, vector_half, vector_half);
TEST_VECTOR_COMPUTE_VCONV_TRUNC_RETURN_INSTR(ASCTRUNC, asc_rint, vtrc, vector_bfloat16_t, vector_bfloat16_t);
TEST_VECTOR_COMPUTE_VCONV_TRUNC_RETURN_INSTR(ASCTRUNC, asc_rint, vtrc, vector_float, vector_float);
// ==========asc_round==========
TEST_VECTOR_COMPUTE_VCONV_TRUNC_INSTR(ASCTRUNC, asc_round, vtrc, vector_half, vector_half);
TEST_VECTOR_COMPUTE_VCONV_TRUNC_INSTR(ASCTRUNC, asc_round, vtrc, vector_bfloat16_t, vector_bfloat16_t);
TEST_VECTOR_COMPUTE_VCONV_TRUNC_INSTR(ASCTRUNC, asc_round, vtrc, vector_float, vector_float);
TEST_VECTOR_COMPUTE_VCONV_TRUNC_RETURN_INSTR(ASCTRUNC, asc_round, vtrc, vector_half, vector_half);
TEST_VECTOR_COMPUTE_VCONV_TRUNC_RETURN_INSTR(ASCTRUNC, asc_round, vtrc, vector_bfloat16_t, vector_bfloat16_t);
TEST_VECTOR_COMPUTE_VCONV_TRUNC_RETURN_INSTR(ASCTRUNC, asc_round, vtrc, vector_float, vector_float);
// ==========asc_trunc==========
TEST_VECTOR_COMPUTE_VCONV_TRUNC_INSTR(ASCTRUNC, asc_trunc, vtrc, vector_half, vector_half);
TEST_VECTOR_COMPUTE_VCONV_TRUNC_INSTR(ASCTRUNC, asc_trunc, vtrc, vector_bfloat16_t, vector_bfloat16_t);
TEST_VECTOR_COMPUTE_VCONV_TRUNC_INSTR(ASCTRUNC, asc_trunc, vtrc, vector_float, vector_float);
TEST_VECTOR_COMPUTE_VCONV_TRUNC_RETURN_INSTR(ASCTRUNC, asc_trunc, vtrc, vector_half, vector_half);
TEST_VECTOR_COMPUTE_VCONV_TRUNC_RETURN_INSTR(ASCTRUNC, asc_trunc, vtrc, vector_bfloat16_t, vector_bfloat16_t);
TEST_VECTOR_COMPUTE_VCONV_TRUNC_RETURN_INSTR(ASCTRUNC, asc_trunc, vtrc, vector_float, vector_float);
