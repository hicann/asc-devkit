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

#define TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_0(                                                             \
    class_name, c_api_name, cce_name, dst_data_type, data_type, cce_data_type)                                     \
                                                                                                                   \
    class TestVectorDataMove##class_name##_##data_type##_CApi : public testing::Test {                             \
    protected:                                                                                                     \
        void SetUp() {}                                                                                            \
        void TearDown() {}                                                                                         \
    };                                                                                                             \
                                                                                                                   \
    namespace {                                                                                                    \
    void cce_name##_##data_type##_Stub(                                                                            \
        vector_##cce_data_type& dst, __ubuf__ cce_data_type* src, vector_uint32_t index, vector_bool mask)         \
    {}                                                                                                             \
    }                                                                                                              \
                                                                                                                   \
    TEST_F(TestVectorDataMove##class_name##_##data_type##_CApi, c_api_name##_##data_type##_Succ)                   \
    {                                                                                                              \
        __ubuf__ data_type* src = reinterpret_cast<__ubuf__ data_type*>(0);                                        \
        dst_data_type dst;                                                                                         \
        vector_uint32_t index;                                                                                     \
        vector_bool mask;                                                                                          \
                                                                                                                   \
        MOCKER_CPP(cce_name, void(vector_##cce_data_type&, __ubuf__ cce_data_type*, vector_uint32_t, vector_bool)) \
            .times(1)                                                                                              \
            .will(invoke(cce_name##_##data_type##_Stub));                                                          \
                                                                                                                   \
        c_api_name(dst, src, index, mask);                                                                         \
        GlobalMockObject::verify();                                                                                \
    }

#define TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_1(                                                \
    class_name, c_api_name, cce_name, dst_data_type, data_type, cce_data_type)                        \
                                                                                                      \
    class TestVectorDataMove##class_name##_##data_type##_CApi : public testing::Test {                \
    protected:                                                                                        \
        void SetUp() {}                                                                               \
        void TearDown() {}                                                                            \
    };                                                                                                \
                                                                                                      \
    namespace {                                                                                       \
    void cce_name##_##data_type##_Stub_1(                                                             \
        vector_##cce_data_type& dst, __ubuf__ cce_data_type* src, vector_uint32_t index)              \
    {}                                                                                                \
    }                                                                                                 \
                                                                                                      \
    TEST_F(TestVectorDataMove##class_name##_##data_type##_CApi, c_api_name##_##data_type##_Succ)      \
    {                                                                                                 \
        __ubuf__ data_type* src = reinterpret_cast<__ubuf__ data_type*>(0);                           \
        dst_data_type dst;                                                                            \
        vector_uint32_t index;                                                                        \
                                                                                                      \
        MOCKER_CPP(cce_name, void(vector_##cce_data_type&, __ubuf__ cce_data_type*, vector_uint32_t)) \
            .times(1)                                                                                 \
            .will(invoke(cce_name##_##data_type##_Stub_1));                                           \
                                                                                                      \
        c_api_name(dst, src, index);                                                                  \
        GlobalMockObject::verify();                                                                   \
    }

#define TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_0(                                                      \
    class_name, c_api_name, cce_name, dst_data_type, data_type, cce_data_type)                                     \
                                                                                                                   \
    class TestVectorDataMoveReturn##class_name##_##data_type##_CApi : public testing::Test {                       \
    protected:                                                                                                     \
        void SetUp() {}                                                                                            \
        void TearDown() {}                                                                                         \
    };                                                                                                             \
                                                                                                                   \
    namespace {                                                                                                    \
    void cce_name##_##data_type##_Return_Stub(                                                                     \
        vector_##cce_data_type& dst, __ubuf__ cce_data_type* src, vector_uint32_t index, vector_bool mask)         \
    {}                                                                                                             \
    }                                                                                                              \
                                                                                                                   \
    TEST_F(TestVectorDataMoveReturn##class_name##_##data_type##_CApi, c_api_name##_return_##data_type##_Succ)      \
    {                                                                                                              \
        __ubuf__ data_type* src = reinterpret_cast<__ubuf__ data_type*>(0);                                        \
        vector_uint32_t index;                                                                                     \
        vector_bool mask;                                                                                          \
                                                                                                                   \
        MOCKER_CPP(cce_name, void(vector_##cce_data_type&, __ubuf__ cce_data_type*, vector_uint32_t, vector_bool)) \
            .times(1)                                                                                              \
            .will(invoke(cce_name##_##data_type##_Return_Stub));                                                   \
                                                                                                                   \
        dst_data_type dst = c_api_name(src, index, mask);                                                          \
        (void)dst;                                                                                                 \
        GlobalMockObject::verify();                                                                                \
    }

#define TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_1(                                                 \
    class_name, c_api_name, cce_name, dst_data_type, data_type, cce_data_type)                                \
                                                                                                              \
    class TestVectorDataMoveReturn##class_name##_##data_type##_CApi : public testing::Test {                  \
    protected:                                                                                                \
        void SetUp() {}                                                                                       \
        void TearDown() {}                                                                                    \
    };                                                                                                        \
                                                                                                              \
    namespace {                                                                                               \
    void cce_name##_##data_type##_Return_Stub_1(                                                              \
        vector_##cce_data_type& dst, __ubuf__ cce_data_type* src, vector_uint32_t index)                      \
    {}                                                                                                        \
    }                                                                                                         \
                                                                                                              \
    TEST_F(TestVectorDataMoveReturn##class_name##_##data_type##_CApi, c_api_name##_return_##data_type##_Succ) \
    {                                                                                                         \
        __ubuf__ data_type* src = reinterpret_cast<__ubuf__ data_type*>(0);                                   \
        vector_uint32_t index;                                                                                \
                                                                                                              \
        MOCKER_CPP(cce_name, void(vector_##cce_data_type&, __ubuf__ cce_data_type*, vector_uint32_t))         \
            .times(1)                                                                                         \
            .will(invoke(cce_name##_##data_type##_Return_Stub_1));                                            \
                                                                                                              \
        dst_data_type dst = c_api_name(src, index);                                                           \
        (void)dst;                                                                                            \
        GlobalMockObject::verify();                                                                           \
    }

TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_0(Vgatherb, asc_gather_datablock, vgatherb, vector_int8_t, int8_t, int8_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_uint8_t, uint8_t, uint8_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_int16_t, int16_t, int16_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_uint16_t, uint16_t, uint16_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_int32_t, int32_t, int32_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_uint32_t, uint32_t, uint32_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_int64_t, int64_t, int64_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_uint64_t, uint64_t, uint64_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_0(Vgatherb, asc_gather_datablock, vgatherb, vector_half, half, half);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_0(Vgatherb, asc_gather_datablock, vgatherb, vector_float, float, float);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_bfloat16_t, bfloat16_t, bfloat16_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_fp8_e4m3fn_t, fp8_e4m3fn_t, fp8_e4m3fn_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_hifloat8_t, hifloat8_t, uint8_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_fp8_e5m2_t, fp8_e5m2_t, fp8_e5m2_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_fp4x2_e2m1_t, fp4x2_e2m1_t, fp4x2_e2m1_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_fp4x2_e1m2_t, fp4x2_e1m2_t, fp4x2_e1m2_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_int4x2_t, int4b_t, fp4x2_e1m2_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_fp8_e8m0_t, fp8_e8m0_t, fp8_e8m0_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_int8_t, int8_t, int8_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_uint8_t, uint8_t, uint8_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_int16_t, int16_t, int16_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_uint16_t, uint16_t, uint16_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_int32_t, int32_t, int32_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_uint32_t, uint32_t, uint32_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_int64_t, int64_t, int64_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_uint64_t, uint64_t, uint64_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_1(Vgatherb_1, asc_gather_datablock, vgatherb, vector_half, half, half);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_1(Vgatherb_1, asc_gather_datablock, vgatherb, vector_float, float, float);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_bfloat16_t, bfloat16_t, bfloat16_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_fp8_e4m3fn_t, fp8_e4m3fn_t, fp8_e4m3fn_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_hifloat8_t, hifloat8_t, uint8_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_fp8_e5m2_t, fp8_e5m2_t, fp8_e5m2_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_fp4x2_e2m1_t, fp4x2_e2m1_t, fp4x2_e2m1_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_fp4x2_e1m2_t, fp4x2_e1m2_t, fp4x2_e1m2_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_int4x2_t, int4b_t, fp4x2_e1m2_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_fp8_e8m0_t, fp8_e8m0_t, fp8_e8m0_t);

TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_int8_t, int8_t, int8_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_uint8_t, uint8_t, uint8_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_int16_t, int16_t, int16_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_uint16_t, uint16_t, uint16_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_int32_t, int32_t, int32_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_uint32_t, uint32_t, uint32_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_0(Vgatherb, asc_gather_datablock, vgatherb, vector_half, half, half);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_float, float, float);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_bfloat16_t, bfloat16_t, bfloat16_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_fp8_e4m3fn_t, fp8_e4m3fn_t, fp8_e4m3fn_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_hifloat8_t, hifloat8_t, uint8_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_fp8_e5m2_t, fp8_e5m2_t, fp8_e5m2_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_fp4x2_e2m1_t, fp4x2_e2m1_t, fp4x2_e2m1_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_fp4x2_e1m2_t, fp4x2_e1m2_t, fp4x2_e1m2_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_int4x2_t, int4b_t, fp4x2_e1m2_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_0(
    Vgatherb, asc_gather_datablock, vgatherb, vector_fp8_e8m0_t, fp8_e8m0_t, fp8_e8m0_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_int8_t, int8_t, int8_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_uint8_t, uint8_t, uint8_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_int16_t, int16_t, int16_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_uint16_t, uint16_t, uint16_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_int32_t, int32_t, int32_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_uint32_t, uint32_t, uint32_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_half, half, half);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_float, float, float);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_bfloat16_t, bfloat16_t, bfloat16_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_fp8_e4m3fn_t, fp8_e4m3fn_t, fp8_e4m3fn_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_hifloat8_t, hifloat8_t, uint8_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_fp8_e5m2_t, fp8_e5m2_t, fp8_e5m2_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_fp4x2_e2m1_t, fp4x2_e2m1_t, fp4x2_e2m1_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_fp4x2_e1m2_t, fp4x2_e1m2_t, fp4x2_e1m2_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_int4x2_t, int4b_t, fp4x2_e1m2_t);
TEST_VECTOR_DATAMOVE_GATHER_DATABLOCK_RETURN_INSTR_1(
    Vgatherb_1, asc_gather_datablock, vgatherb, vector_fp8_e8m0_t, fp8_e8m0_t, fp8_e8m0_t);
