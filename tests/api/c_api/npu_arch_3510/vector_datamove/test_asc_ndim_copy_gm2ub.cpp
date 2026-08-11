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

#define TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_INSTR(data_type, cce_name)                                                \
                                                                                                                       \
    class TestNdimCopyGM2UB##data_type##CApi : public testing::Test {                                                  \
    protected:                                                                                                         \
        void SetUp() {}                                                                                                \
        void TearDown() {}                                                                                             \
    };                                                                                                                 \
                                                                                                                       \
    namespace {                                                                                                        \
                                                                                                                       \
    void nddma_out_to_ub_##data_type##_Stub(                                                                           \
        __ubuf__ void* dst, __gm__ void* src, uint8_t sid, uint32_t loop0_size, uint32_t loop1_size,                   \
        uint32_t loop2_size, uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, \
        bool padding_mode, uint8_t cache_mode)                                                                         \
    {                                                                                                                  \
        EXPECT_EQ(dst, reinterpret_cast<__ubuf__ void*>(11));                                                          \
        EXPECT_EQ(src, reinterpret_cast<__gm__ void*>(22));                                                            \
        EXPECT_EQ(sid, static_cast<uint8_t>(0));                                                                       \
        EXPECT_EQ(loop0_size, static_cast<uint32_t>(1));                                                               \
        EXPECT_EQ(loop1_size, static_cast<uint32_t>(2));                                                               \
        EXPECT_EQ(loop2_size, static_cast<uint32_t>(3));                                                               \
        EXPECT_EQ(loop3_size, static_cast<uint32_t>(4));                                                               \
        EXPECT_EQ(loop4_size, static_cast<uint32_t>(5));                                                               \
        EXPECT_EQ(loop0_lp_count, static_cast<uint8_t>(6));                                                            \
        EXPECT_EQ(loop0_rp_count, static_cast<uint8_t>(7));                                                            \
        EXPECT_EQ(padding_mode, static_cast<bool>(1));                                                                 \
        EXPECT_EQ(cache_mode, static_cast<uint8_t>(6));                                                                \
    }                                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    TEST_F(TestNdimCopyGM2UB##data_type##CApi, asc_ndim_copy_gm2ub_##data_type##_Succ)                                 \
    {                                                                                                                  \
        __ubuf__ data_type* dst = reinterpret_cast<__ubuf__ data_type*>(11);                                           \
        __gm__ data_type* src = reinterpret_cast<__gm__ data_type*>(22);                                               \
                                                                                                                       \
        uint32_t loop0_size = static_cast<uint64_t>(1);                                                                \
        uint32_t loop1_size = static_cast<uint64_t>(2);                                                                \
        uint32_t loop2_size = static_cast<uint64_t>(3);                                                                \
        uint32_t loop3_size = static_cast<uint64_t>(4);                                                                \
        uint32_t loop4_size = static_cast<uint64_t>(5);                                                                \
        uint8_t loop0_lp_count = static_cast<uint64_t>(6);                                                             \
        uint8_t loop0_rp_count = static_cast<uint64_t>(7);                                                             \
        bool padding_mode = static_cast<uint64_t>(1);                                                                  \
        asc_load_l2_cache_mode cache_mode = asc_load_l2_cache_mode::NOTALLOC_DROP;                                     \
                                                                                                                       \
        MOCKER_CPP(                                                                                                    \
            cce_name, void(                                                                                            \
                          __ubuf__ void*, __gm__ void*, uint8_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,     \
                          uint8_t, uint8_t, bool, uint8_t))                                                            \
            .times(1)                                                                                                  \
            .will(invoke(nddma_out_to_ub_##data_type##_Stub));                                                         \
                                                                                                                       \
        asc_ndim_copy_gm2ub(                                                                                           \
            dst, src, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size, loop0_lp_count, loop0_rp_count,      \
            padding_mode, cache_mode);                                                                                 \
        GlobalMockObject::verify();                                                                                    \
    }

//===========asc_ndim_copy_gm2ub(int8/uint8/e2m1/e1m2/e8m0/e5m2/e4m3fn/int16/uint16/half/bfloat16/int32/uint32/float)===========
TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_INSTR(int8_t, nddma_out_to_ub_b8);
TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_INSTR(uint8_t, nddma_out_to_ub_b8);
TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_INSTR(fp4x2_e2m1_t, nddma_out_to_ub_b8);
TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_INSTR(fp4x2_e1m2_t, nddma_out_to_ub_b8);
TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_INSTR(fp8_e8m0_t, nddma_out_to_ub_b8);
TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_INSTR(fp8_e5m2_t, nddma_out_to_ub_b8);
TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_INSTR(fp8_e4m3fn_t, nddma_out_to_ub_b8);
TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_INSTR(int16_t, nddma_out_to_ub_b16);
TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_INSTR(uint16_t, nddma_out_to_ub_b16);
TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_INSTR(half, nddma_out_to_ub_b16);
TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_INSTR(bfloat16_t, nddma_out_to_ub_b16);
TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_INSTR(int32_t, nddma_out_to_ub_b32);
TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_INSTR(uint32_t, nddma_out_to_ub_b32);
TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_INSTR(float, nddma_out_to_ub_b32);
