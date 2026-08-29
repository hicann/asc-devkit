/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <type_traits>
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "tests/api/c_api/stub/cce_stub.h"
#include "include/c_api/asc_simd.h"

#define TEST_VECTOR_DATAMOVE_LOAD_INSTR(                                                                     \
    class_name, c_api_name, cce_name0, cce_name1, dst_data_type, data_type, cce_data_type)                   \
                                                                                                             \
    class TestVectorDataMove##class_name##_##data_type##_CApi : public testing::Test {                       \
    protected:                                                                                               \
        void SetUp() {}                                                                                      \
        void TearDown() {}                                                                                   \
    };                                                                                                       \
                                                                                                             \
    namespace {                                                                                              \
    void cce_name1##_##data_type##_Stub1(vector_load_unalign& src0, __ubuf__ cce_data_type* src1)            \
    {                                                                                                        \
        EXPECT_EQ(src1, reinterpret_cast<__ubuf__ cce_data_type*>(11));                                      \
    }                                                                                                        \
    void cce_name0##_##data_type##_Stub0(                                                                    \
        vector_##cce_data_type& dst, vector_load_unalign& src0, __ubuf__ cce_data_type* src1)                \
    {                                                                                                        \
        EXPECT_EQ(src1, reinterpret_cast<__ubuf__ cce_data_type*>(11));                                      \
    }                                                                                                        \
    }                                                                                                        \
                                                                                                             \
    TEST_F(TestVectorDataMove##class_name##_##data_type##_CApi, c_api_name##_##data_type##_Succ)             \
    {                                                                                                        \
        __ubuf__ data_type* src = reinterpret_cast<__ubuf__ data_type*>(11);                                 \
        dst_data_type dst;                                                                                   \
                                                                                                             \
        MOCKER_CPP(cce_name1, void(vector_load_unalign&, __ubuf__ cce_data_type*))                           \
            .times(1)                                                                                        \
            .will(invoke(cce_name1##_##data_type##_Stub1));                                                  \
                                                                                                             \
        MOCKER_CPP(cce_name0, void(vector_##cce_data_type&, vector_store_unalign&, __ubuf__ cce_data_type*)) \
            .times(1)                                                                                        \
            .will(invoke(cce_name0##_##data_type##_Stub0));                                                  \
                                                                                                             \
        c_api_name(dst, src);                                                                                \
        GlobalMockObject::verify();                                                                          \
    }

#define TEST_VECTOR_DATAMOVE_LOAD_RETURN(                                                                    \
    class_name, c_api_name, cce_name0, cce_name1, dst_data_type, data_type, cce_data_type)                   \
    TEST_F(TestVectorDataMove##class_name##_##data_type##_CApi, c_api_name##_##data_type##_ReturnSucc)       \
    {                                                                                                        \
        __ubuf__ data_type* src = reinterpret_cast<__ubuf__ data_type*>(11);                                 \
                                                                                                             \
        MOCKER_CPP(cce_name1, void(vector_load_unalign&, __ubuf__ cce_data_type*))                           \
            .times(1)                                                                                        \
            .will(invoke(cce_name1##_##data_type##_Stub1));                                                  \
                                                                                                             \
        MOCKER_CPP(cce_name0, void(vector_##cce_data_type&, vector_store_unalign&, __ubuf__ cce_data_type*)) \
            .times(1)                                                                                        \
            .will(invoke(cce_name0##_##data_type##_Stub0));                                                  \
                                                                                                             \
        static_assert(std::is_same_v<decltype(c_api_name(src)), dst_data_type>);                             \
        dst_data_type dst = c_api_name(src);                                                                 \
        (void)dst;                                                                                           \
        GlobalMockObject::verify();                                                                          \
    }

TEST_VECTOR_DATAMOVE_LOAD_INSTR(Vldasandvldus, asc_load, vldus, vldas, vector_int8_t, int8_t, int8_t);
TEST_VECTOR_DATAMOVE_LOAD_INSTR(Vldasandvldus, asc_load, vldus, vldas, vector_uint8_t, uint8_t, uint8_t);
TEST_VECTOR_DATAMOVE_LOAD_INSTR(Vldasandvldus, asc_load, vldus, vldas, vector_int16_t, int16_t, int16_t);
TEST_VECTOR_DATAMOVE_LOAD_INSTR(Vldasandvldus, asc_load, vldus, vldas, vector_uint16_t, uint16_t, uint16_t);
TEST_VECTOR_DATAMOVE_LOAD_INSTR(Vldasandvldus, asc_load, vldus, vldas, vector_int32_t, int32_t, int32_t);
TEST_VECTOR_DATAMOVE_LOAD_INSTR(Vldasandvldus, asc_load, vldus, vldas, vector_uint32_t, uint32_t, uint32_t);
TEST_VECTOR_DATAMOVE_LOAD_INSTR(Vldasandvldus, asc_load, vldus, vldas, vector_int64_t, int64_t, int64_t);
TEST_VECTOR_DATAMOVE_LOAD_INSTR(Vldasandvldus, asc_load, vldus, vldas, vector_half, half, half);
TEST_VECTOR_DATAMOVE_LOAD_INSTR(Vldasandvldus, asc_load, vldus, vldas, vector_float, float, float);
TEST_VECTOR_DATAMOVE_LOAD_INSTR(Vldasandvldus, asc_load, vldus, vldas, vector_bfloat16_t, bfloat16_t, bfloat16_t);
TEST_VECTOR_DATAMOVE_LOAD_INSTR(Vldasandvldus, asc_load, vldus, vldas, vector_fp8_e4m3fn_t, fp8_e4m3fn_t, fp8_e4m3fn_t);
TEST_VECTOR_DATAMOVE_LOAD_INSTR(Vldasandvldus, asc_load, vldus, vldas, vector_hifloat8_t, hifloat8_t, uint8_t);
TEST_VECTOR_DATAMOVE_LOAD_INSTR(Vldasandvldus, asc_load, vldus, vldas, vector_fp8_e5m2_t, fp8_e5m2_t, fp8_e5m2_t);
TEST_VECTOR_DATAMOVE_LOAD_INSTR(Vldasandvldus, asc_load, vldus, vldas, vector_fp8_e8m0_t, fp8_e8m0_t, fp8_e8m0_t);
TEST_VECTOR_DATAMOVE_LOAD_INSTR(Vldasandvldus, asc_load, vldus, vldas, vector_fp4x2_e2m1_t, fp4x2_e2m1_t, fp4x2_e2m1_t);
TEST_VECTOR_DATAMOVE_LOAD_INSTR(Vldasandvldus, asc_load, vldus, vldas, vector_fp4x2_e1m2_t, fp4x2_e1m2_t, fp4x2_e1m2_t);
TEST_VECTOR_DATAMOVE_LOAD_INSTR(Vldasandvldus, asc_load, vldus, vldas, vector_int4x2_t, int4b_t, fp4x2_e1m2_t);

TEST_VECTOR_DATAMOVE_LOAD_RETURN(Vldasandvldus, asc_load, vldus, vldas, vector_int8_t, int8_t, int8_t);
TEST_VECTOR_DATAMOVE_LOAD_RETURN(Vldasandvldus, asc_load, vldus, vldas, vector_uint8_t, uint8_t, uint8_t);
TEST_VECTOR_DATAMOVE_LOAD_RETURN(Vldasandvldus, asc_load, vldus, vldas, vector_int16_t, int16_t, int16_t);
TEST_VECTOR_DATAMOVE_LOAD_RETURN(Vldasandvldus, asc_load, vldus, vldas, vector_uint16_t, uint16_t, uint16_t);
TEST_VECTOR_DATAMOVE_LOAD_RETURN(Vldasandvldus, asc_load, vldus, vldas, vector_int32_t, int32_t, int32_t);
TEST_VECTOR_DATAMOVE_LOAD_RETURN(Vldasandvldus, asc_load, vldus, vldas, vector_uint32_t, uint32_t, uint32_t);
TEST_VECTOR_DATAMOVE_LOAD_RETURN(Vldasandvldus, asc_load, vldus, vldas, vector_half, half, half);
TEST_VECTOR_DATAMOVE_LOAD_RETURN(Vldasandvldus, asc_load, vldus, vldas, vector_float, float, float);
TEST_VECTOR_DATAMOVE_LOAD_RETURN(Vldasandvldus, asc_load, vldus, vldas, vector_bfloat16_t, bfloat16_t, bfloat16_t);
TEST_VECTOR_DATAMOVE_LOAD_RETURN(
    Vldasandvldus, asc_load, vldus, vldas, vector_fp8_e4m3fn_t, fp8_e4m3fn_t, fp8_e4m3fn_t);
TEST_VECTOR_DATAMOVE_LOAD_RETURN(Vldasandvldus, asc_load, vldus, vldas, vector_hifloat8_t, hifloat8_t, uint8_t);
TEST_VECTOR_DATAMOVE_LOAD_RETURN(Vldasandvldus, asc_load, vldus, vldas, vector_fp8_e5m2_t, fp8_e5m2_t, fp8_e5m2_t);
TEST_VECTOR_DATAMOVE_LOAD_RETURN(Vldasandvldus, asc_load, vldus, vldas, vector_fp8_e8m0_t, fp8_e8m0_t, fp8_e8m0_t);
TEST_VECTOR_DATAMOVE_LOAD_RETURN(
    Vldasandvldus, asc_load, vldus, vldas, vector_fp4x2_e2m1_t, fp4x2_e2m1_t, fp4x2_e2m1_t);
TEST_VECTOR_DATAMOVE_LOAD_RETURN(
    Vldasandvldus, asc_load, vldus, vldas, vector_fp4x2_e1m2_t, fp4x2_e1m2_t, fp4x2_e1m2_t);
TEST_VECTOR_DATAMOVE_LOAD_RETURN(Vldasandvldus, asc_load, vldus, vldas, vector_int4x2_t, int4b_t, fp4x2_e1m2_t);
