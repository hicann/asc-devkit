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

#define TEST_VECTOR_DATAMOVE_GATHER_INSTR_0(                                                                      \
    class_name, c_api_name, cce_name, data_type, vector_data_type, index_data_type)                               \
                                                                                                                  \
    class TestVectorDataMove##class_name##_##data_type##_CApi : public testing::Test {                            \
    protected:                                                                                                    \
        void SetUp() {}                                                                                           \
        void TearDown() {}                                                                                        \
    };                                                                                                            \
                                                                                                                  \
    namespace {                                                                                                   \
    void cce_name##_##data_type##_Stub(                                                                           \
        vector_data_type& dst, __ubuf__ data_type* src, vector_##index_data_type index, vector_bool mask)         \
    {}                                                                                                            \
    }                                                                                                             \
                                                                                                                  \
    TEST_F(TestVectorDataMove##class_name##_##data_type##_CApi, c_api_name##_##data_type##_Succ)                  \
    {                                                                                                             \
        __ubuf__ data_type* src = reinterpret_cast<__ubuf__ data_type*>(0);                                       \
        vector_data_type dst;                                                                                     \
        vector_##index_data_type index;                                                                           \
        vector_bool mask;                                                                                         \
                                                                                                                  \
        MOCKER_CPP(cce_name, void(vector_data_type&, __ubuf__ data_type*, vector_##index_data_type, vector_bool)) \
            .times(1)                                                                                             \
            .will(invoke(cce_name##_##data_type##_Stub));                                                         \
                                                                                                                  \
        c_api_name(dst, src, index, mask);                                                                        \
        GlobalMockObject::verify();                                                                               \
    }

#define TEST_VECTOR_DATAMOVE_GATHER_INSTR_1(class_name, c_api_name, cce_name, data_type, index_data_type) \
                                                                                                          \
    class TestVectorDataMove##class_name##_##data_type##_CApi : public testing::Test {                    \
    protected:                                                                                            \
        void SetUp() {}                                                                                   \
        void TearDown() {}                                                                                \
    };                                                                                                    \
                                                                                                          \
    namespace {                                                                                           \
    void cce_name##_##data_type##_Stub(                                                                   \
        vector_##data_type& dst, vector_##data_type src, vector_##index_data_type index)                  \
    {}                                                                                                    \
    }                                                                                                     \
                                                                                                          \
    TEST_F(TestVectorDataMove##class_name##_##data_type##_CApi, c_api_name##_##data_type##_Succ)          \
    {                                                                                                     \
        vector_##data_type src;                                                                           \
        vector_##data_type dst;                                                                           \
        vector_##index_data_type index;                                                                   \
                                                                                                          \
        MOCKER_CPP(cce_name, void(vector_##data_type&, vector_##data_type, vector_##index_data_type))     \
            .times(1)                                                                                     \
            .will(invoke(cce_name##_##data_type##_Stub));                                                 \
                                                                                                          \
        c_api_name(dst, src, index);                                                                      \
        GlobalMockObject::verify();                                                                       \
    }

TEST_VECTOR_DATAMOVE_GATHER_INSTR_0(Vgather2, asc_gather, vgather2, int8_t, vector_int16_t, uint16_t);
TEST_VECTOR_DATAMOVE_GATHER_INSTR_0(Vgather2, asc_gather, vgather2, uint8_t, vector_uint16_t, uint16_t);
TEST_VECTOR_DATAMOVE_GATHER_INSTR_0(Vgather2, asc_gather, vgather2, int16_t, vector_int16_t, uint16_t);
TEST_VECTOR_DATAMOVE_GATHER_INSTR_0(Vgather2, asc_gather, vgather2, uint16_t, vector_uint16_t, uint16_t);
TEST_VECTOR_DATAMOVE_GATHER_INSTR_0(Vgather2, asc_gather, vgather2, int32_t, vector_int32_t, uint32_t);
TEST_VECTOR_DATAMOVE_GATHER_INSTR_0(Vgather2, asc_gather, vgather2, uint32_t, vector_uint32_t, uint32_t);
TEST_VECTOR_DATAMOVE_GATHER_INSTR_0(Vgather2, asc_gather, vgather2, half, vector_half, uint16_t);
TEST_VECTOR_DATAMOVE_GATHER_INSTR_0(Vgather2, asc_gather, vgather2, float, vector_float, uint32_t);
TEST_VECTOR_DATAMOVE_GATHER_INSTR_0(Vgather2, asc_gather, vgather2, bfloat16_t, vector_bfloat16_t, uint16_t);
TEST_VECTOR_DATAMOVE_GATHER_INSTR_0(Vgather2, asc_gather, vgather2, fp8_e4m3fn_t, vector_fp8_e4m3fn_t, uint16_t);
TEST_VECTOR_DATAMOVE_GATHER_INSTR_0(Vgather2, asc_gather, vgather2, fp8_e5m2_t, vector_fp8_e5m2_t, uint16_t);
TEST_VECTOR_DATAMOVE_GATHER_INSTR_0(Vgather2, asc_gather, vgather2, fp8_e8m0_t, vector_fp8_e8m0_t, uint16_t);
TEST_VECTOR_DATAMOVE_GATHER_INSTR_1(Vselr, asc_gather, vselr, int8_t, uint8_t);
TEST_VECTOR_DATAMOVE_GATHER_INSTR_1(Vselr, asc_gather, vselr, uint8_t, uint8_t);
TEST_VECTOR_DATAMOVE_GATHER_INSTR_1(Vselr, asc_gather, vselr, int16_t, uint16_t);
TEST_VECTOR_DATAMOVE_GATHER_INSTR_1(Vselr, asc_gather, vselr, uint16_t, uint16_t);
TEST_VECTOR_DATAMOVE_GATHER_INSTR_1(Vselr, asc_gather, vselr, int32_t, uint32_t);
TEST_VECTOR_DATAMOVE_GATHER_INSTR_1(Vselr, asc_gather, vselr, uint32_t, uint32_t);
TEST_VECTOR_DATAMOVE_GATHER_INSTR_1(Vselr, asc_gather, vselr, half, uint16_t);
TEST_VECTOR_DATAMOVE_GATHER_INSTR_1(Vselr, asc_gather, vselr, bfloat16_t, uint16_t);
TEST_VECTOR_DATAMOVE_GATHER_INSTR_1(Vselr, asc_gather, vselr, fp8_e4m3fn_t, uint8_t);
TEST_VECTOR_DATAMOVE_GATHER_INSTR_1(Vselr, asc_gather, vselr, fp8_e5m2_t, uint8_t);
TEST_VECTOR_DATAMOVE_GATHER_INSTR_1(Vselr, asc_gather, vselr, fp8_e8m0_t, uint8_t);

TEST_VECTOR_DATAMOVE_GATHER_INSTR_0(Vgather2_BC, asc_gather, vgather2_bc, int16_t, vector_int16_t, uint32_t);
TEST_VECTOR_DATAMOVE_GATHER_INSTR_0(Vgather2_BC, asc_gather, vgather2_bc, uint16_t, vector_uint16_t, uint32_t);
TEST_VECTOR_DATAMOVE_GATHER_INSTR_0(Vgather2_BC, asc_gather, vgather2_bc, half, vector_half, uint32_t);
TEST_VECTOR_DATAMOVE_GATHER_INSTR_0(Vgather2_BC, asc_gather, vgather2_bc, bfloat16_t, vector_bfloat16_t, uint32_t);

class TestCApiVectorDataMoveAscGatherHifloat8 : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};
class TestCApiVectorDataMoveAscGatherInt8 : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};
class TestCApiVectorDataMoveAscGatherUint8 : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};
class TestCApiVectorDataMoveAscGatherFloat : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

namespace {
void vgather2_hifloat8_stub(
    vector_fp8_e5m2_t& dst, __ubuf__ float8_e5m2_t* src, vector_uint16_t index, vector_bool mask)
{}
void vgather2_int8_stub(vector_fp8_e5m2_t& dst, __ubuf__ float8_e5m2_t* src, vector_uint16_t index, vector_bool mask) {}
void vgather2_uint8_stub(vector_fp8_e5m2_t& dst, __ubuf__ float8_e5m2_t* src, vector_uint16_t index, vector_bool mask)
{}
void vselr_hifloat8_stub(vector_uint8_t& dst, vector_uint8_t src, vector_uint8_t index) {}
void vselr_float_stub(vector_uint32_t& dst, vector_uint32_t src, vector_uint32_t index) {}
} // namespace

TEST_F(TestCApiVectorDataMoveAscGatherHifloat8, vgather2_Succ)
{
    __ubuf__ hifloat8_t* src = reinterpret_cast<__ubuf__ hifloat8_t*>(0);
    vector_hifloat8_t dst;
    vector_uint16_t index;
    vector_bool mask;

    MOCKER_CPP(vgather2, void(vector_fp8_e5m2_t&, __ubuf__ float8_e5m2_t*, vector_uint16_t, vector_bool))
        .times(1)
        .will(invoke(vgather2_hifloat8_stub));

    asc_gather(dst, src, index, mask);
    GlobalMockObject::verify();
}

TEST_F(TestCApiVectorDataMoveAscGatherInt8, vgather2_Succ)
{
    __ubuf__ int8_t* src = reinterpret_cast<__ubuf__ int8_t*>(0);
    vector_int8_t dst;
    vector_uint16_t index;
    vector_bool mask;

    MOCKER_CPP(vgather2, void(vector_fp8_e5m2_t&, __ubuf__ float8_e5m2_t*, vector_uint16_t, vector_bool))
        .times(1)
        .will(invoke(vgather2_int8_stub));

    asc_gather(dst, src, index, mask);
    GlobalMockObject::verify();
}

TEST_F(TestCApiVectorDataMoveAscGatherUint8, vgather2_Succ)
{
    __ubuf__ uint8_t* src = reinterpret_cast<__ubuf__ uint8_t*>(0);
    vector_uint8_t dst;
    vector_uint16_t index;
    vector_bool mask;

    MOCKER_CPP(vgather2, void(vector_fp8_e5m2_t&, __ubuf__ float8_e5m2_t*, vector_uint16_t, vector_bool))
        .times(1)
        .will(invoke(vgather2_uint8_stub));

    asc_gather(dst, src, index, mask);
    GlobalMockObject::verify();
}

TEST_F(TestCApiVectorDataMoveAscGatherHifloat8, vselr_Succ)
{
    vector_hifloat8_t src;
    vector_hifloat8_t dst;
    vector_uint8_t index;

    MOCKER_CPP(vselr, void(vector_uint8_t&, vector_uint8_t, vector_uint8_t)).times(1).will(invoke(vselr_hifloat8_stub));

    asc_gather(dst, src, index);
    GlobalMockObject::verify();
}

TEST_F(TestCApiVectorDataMoveAscGatherFloat, vselr_Succ)
{
    vector_float src;
    vector_float dst;
    vector_uint32_t index;

    MOCKER_CPP(vselr, void(vector_uint32_t&, vector_uint32_t, vector_uint32_t)).times(1).will(invoke(vselr_float_stub));

    asc_gather(dst, src, index);
    GlobalMockObject::verify();
}
