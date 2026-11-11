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

#define TEST_VECTOR_DATAMOVE_SCATTER_INSTR_0(class_name, c_api_name, cce_name, data_type, index_data_type)          \
                                                                                                                    \
    class TestVectorDataMove##class_name##_##data_type##_CApi : public testing::Test {                              \
    protected:                                                                                                      \
        void SetUp() {}                                                                                             \
        void TearDown() {}                                                                                          \
    };                                                                                                              \
                                                                                                                    \
    namespace {                                                                                                     \
    void cce_name##_##data_type##_Stub(                                                                             \
        vector_##data_type& src, __ubuf__ data_type* dst, vector_##index_data_type index, vector_bool mask)         \
    {}                                                                                                              \
    }                                                                                                               \
                                                                                                                    \
    TEST_F(TestVectorDataMove##class_name##_##data_type##_CApi, c_api_name##_##data_type##_Succ)                    \
    {                                                                                                               \
        vector_##data_type src;                                                                                     \
        __ubuf__ data_type* dst = reinterpret_cast<__ubuf__ data_type*>(0);                                         \
        vector_##index_data_type index;                                                                             \
        vector_bool mask;                                                                                           \
                                                                                                                    \
        MOCKER_CPP(cce_name, void(vector_##data_type&, __ubuf__ data_type*, vector_##index_data_type, vector_bool)) \
            .times(2)                                                                                               \
            .will(invoke(cce_name##_##data_type##_Stub));                                                           \
                                                                                                                    \
        c_api_name(dst, src, index, mask);                                                                          \
        c_api_name(src, dst, index, mask);                                                                          \
        GlobalMockObject::verify();                                                                                 \
    }

TEST_VECTOR_DATAMOVE_SCATTER_INSTR_0(Vscatter, asc_scatter, vscatter, int8_t, uint16_t);
TEST_VECTOR_DATAMOVE_SCATTER_INSTR_0(Vscatter, asc_scatter, vscatter, uint8_t, uint16_t);
TEST_VECTOR_DATAMOVE_SCATTER_INSTR_0(Vscatter, asc_scatter, vscatter, int16_t, uint16_t);
TEST_VECTOR_DATAMOVE_SCATTER_INSTR_0(Vscatter, asc_scatter, vscatter, uint16_t, uint16_t);
TEST_VECTOR_DATAMOVE_SCATTER_INSTR_0(Vscatter, asc_scatter, vscatter, int32_t, uint32_t);
TEST_VECTOR_DATAMOVE_SCATTER_INSTR_0(Vscatter, asc_scatter, vscatter, uint32_t, uint32_t);
TEST_VECTOR_DATAMOVE_SCATTER_INSTR_0(Vscatter, asc_scatter, vscatter, bfloat16_t, uint16_t);
TEST_VECTOR_DATAMOVE_SCATTER_INSTR_0(Vscatter, asc_scatter, vscatter, half, uint16_t);
TEST_VECTOR_DATAMOVE_SCATTER_INSTR_0(Vscatter, asc_scatter, vscatter, float, uint32_t);

TEST(TestVectorDatamoveScatterFp8E8m0, CallsScatter)
{
    auto* dst = reinterpret_cast<__ubuf__ fp8_e8m0_t*>(32);
    vector_fp8_e8m0_t src{};
    vector_uint16_t index{};
    vector_bool mask{};
    MOCKER_CPP(vscatter, void(vector_fp8_e8m0_t&, __ubuf__ fp8_e8m0_t*, vector_uint16_t, vector_bool)).times(1);
    asc_scatter(dst, src, index, mask);
    GlobalMockObject::verify();
}
