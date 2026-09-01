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
#include "tests/api/c_api/stub/cce_stub.h"
#include "include/c_api/asc_simd.h"

#define TEST_SQUEEZE_AND_STOREUNALIGN(                                                                              \
    data_type, api_vector_type, squeeze_vector_type, store_vector_type, store_data_type)                            \
                                                                                                                    \
    class TestVectorDataMoveSqueezeAndStoreUnalign_##data_type##_CApi : public testing::Test {                      \
    protected:                                                                                                      \
        void SetUp() {}                                                                                             \
        void TearDown() {}                                                                                          \
    };                                                                                                              \
                                                                                                                    \
    namespace {                                                                                                     \
    const void* expected_dst_##data_type = nullptr;                                                                 \
                                                                                                                    \
    void sprclr_##data_type##_Stub(Literal spr_id) { EXPECT_EQ(spr_id, SPR_AR); }                                   \
                                                                                                                    \
    void vsqz_##data_type##_Stub(squeeze_vector_type& dst, squeeze_vector_type src, vector_bool mask, Literal mode) \
    {                                                                                                               \
        EXPECT_EQ(mode, MODE_STORED);                                                                               \
        EXPECT_EQ(static_cast<const void*>(&dst), expected_dst_##data_type);                                        \
    }                                                                                                               \
                                                                                                                    \
    void vstur_##data_type##_Stub(                                                                                  \
        vector_store_unalign& src0, store_vector_type src1, __ubuf__ store_data_type* dst, Literal post)            \
    {                                                                                                               \
        EXPECT_EQ(post, POST_UPDATE);                                                                               \
    }                                                                                                               \
                                                                                                                    \
    void vstar_##data_type##_Stub(vector_store_unalign src, __ubuf__ store_data_type* dst) {}                       \
    }                                                                                                               \
                                                                                                                    \
    TEST_F(TestVectorDataMoveSqueezeAndStoreUnalign_##data_type##_CApi, FullWorkflowSucc)                           \
    {                                                                                                               \
        __ubuf__ data_type* dst_addr = reinterpret_cast<__ubuf__ data_type*>(0);                                    \
        api_vector_type dst;                                                                                        \
        vector_store_unalign src0;                                                                                  \
        api_vector_type src1;                                                                                       \
        vector_bool squeeze_mask;                                                                                   \
        expected_dst_##data_type = static_cast<const void*>(&dst);                                                  \
                                                                                                                    \
        MOCKER_CPP(sprclr, void(Literal)).times(1).will(invoke(sprclr_##data_type##_Stub));                         \
        MOCKER_CPP(vsqz, void(squeeze_vector_type&, squeeze_vector_type, vector_bool, Literal))                     \
            .times(1)                                                                                               \
            .will(invoke(vsqz_##data_type##_Stub));                                                                 \
        MOCKER_CPP(vstur, void(vector_store_unalign&, store_vector_type, __ubuf__ store_data_type*, Literal))       \
            .times(1)                                                                                               \
            .will(invoke(vstur_##data_type##_Stub));                                                                \
        MOCKER_CPP(vstar, void(vector_store_unalign, __ubuf__ store_data_type*))                                    \
            .times(1)                                                                                               \
            .will(invoke(vstar_##data_type##_Stub));                                                                \
                                                                                                                    \
        asc_squeeze_and_storeunalign_init();                                                                        \
        asc_squeeze_and_storeunalign(dst_addr, dst, src0, src1, squeeze_mask);                                      \
        asc_squeeze_and_storeunalign_finalize(dst_addr, src0);                                                      \
        GlobalMockObject::verify();                                                                                 \
    }

TEST_SQUEEZE_AND_STOREUNALIGN(int8_t, vector_int8_t, vector_int8_t, vector_int8_t, int8_t);
TEST_SQUEEZE_AND_STOREUNALIGN(uint8_t, vector_uint8_t, vector_uint8_t, vector_uint8_t, uint8_t);
TEST_SQUEEZE_AND_STOREUNALIGN(int16_t, vector_int16_t, vector_int16_t, vector_int16_t, int16_t);
TEST_SQUEEZE_AND_STOREUNALIGN(uint16_t, vector_uint16_t, vector_uint16_t, vector_uint16_t, uint16_t);
TEST_SQUEEZE_AND_STOREUNALIGN(int32_t, vector_int32_t, vector_int32_t, vector_int32_t, int32_t);
TEST_SQUEEZE_AND_STOREUNALIGN(uint32_t, vector_uint32_t, vector_uint32_t, vector_uint32_t, uint32_t);
TEST_SQUEEZE_AND_STOREUNALIGN(half, vector_half, vector_half, vector_half, half);
TEST_SQUEEZE_AND_STOREUNALIGN(float, vector_float, vector_float, vector_float, float);
TEST_SQUEEZE_AND_STOREUNALIGN(bfloat16_t, vector_bfloat16_t, vector_bfloat16_t, vector_bfloat16_t, bfloat16_t);
TEST_SQUEEZE_AND_STOREUNALIGN(
    fp8_e4m3fn_t, vector_fp8_e4m3fn_t, vector_fp8_e4m3fn_t, vector_fp8_e4m3fn_t, float8_e4m3_t);
TEST_SQUEEZE_AND_STOREUNALIGN(fp8_e5m2_t, vector_fp8_e5m2_t, vector_fp8_e5m2_t, vector_fp8_e5m2_t, float8_e5m2_t);
TEST_SQUEEZE_AND_STOREUNALIGN(fp8_e8m0_t, vector_fp8_e8m0_t, vector_fp8_e8m0_t, vector_fp8_e8m0_t, fp8_e8m0_t);
TEST_SQUEEZE_AND_STOREUNALIGN(fp4x2_e2m1_t, vector_fp4x2_e2m1_t, vector_int8_t, vector_fp4x2_e2m1_t, float4_e2m1x2_t);
TEST_SQUEEZE_AND_STOREUNALIGN(fp4x2_e1m2_t, vector_fp4x2_e1m2_t, vector_int8_t, vector_fp4x2_e1m2_t, float4_e1m2x2_t);
TEST_SQUEEZE_AND_STOREUNALIGN(int4b_t, vector_int4x2_t, vector_int8_t, vector_fp4x2_e1m2_t, float4_e1m2x2_t);
