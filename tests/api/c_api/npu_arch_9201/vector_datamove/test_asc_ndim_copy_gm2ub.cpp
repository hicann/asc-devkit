/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software; you can redistribute it and/or modify it under the terms and conditions of
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

#define TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_PRE_LOAD(data_type)                                                  \
                                                                                                                  \
    class TestNdimCopyGM2UBPreLoad##data_type##CApi : public testing::Test {                                      \
    protected:                                                                                                    \
        void SetUp() {}                                                                                           \
        void TearDown() {}                                                                                        \
    };                                                                                                            \
                                                                                                                  \
    TEST_F(TestNdimCopyGM2UBPreLoad##data_type##CApi, asc_ndim_copy_gm2ub_pre_load_##data_type##_Succ)            \
    {                                                                                                             \
        __ubuf__ data_type* dst = reinterpret_cast<__ubuf__ data_type*>(11);                                      \
        __gm__ data_type* src = reinterpret_cast<__gm__ data_type*>(22);                                          \
        asc_load_l2_cache_mode cache_mode = asc_load_l2_cache_mode::NOTALLOC_DROP;                                \
                                                                                                                  \
        asc_ndim_copy_gm2ub(                                                                                      \
            dst, src, static_cast<uint32_t>(1), static_cast<uint32_t>(2), static_cast<uint32_t>(3),               \
            static_cast<uint32_t>(4), static_cast<uint32_t>(5), static_cast<uint8_t>(6), static_cast<uint8_t>(7), \
            true, true, cache_mode);                                                                              \
        GlobalMockObject::verify();                                                                               \
    }

TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_PRE_LOAD(int8_t);
TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_PRE_LOAD(uint8_t);
TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_PRE_LOAD(fp4x2_e2m1_t);
TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_PRE_LOAD(fp4x2_e1m2_t);
TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_PRE_LOAD(fp8_e8m0_t);
TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_PRE_LOAD(fp8_e5m2_t);
TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_PRE_LOAD(fp8_e4m3fn_t);
TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_PRE_LOAD(fp8_e6m2_t);
TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_PRE_LOAD(int16_t);
TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_PRE_LOAD(uint16_t);
TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_PRE_LOAD(half);
TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_PRE_LOAD(bfloat16_t);
TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_PRE_LOAD(int32_t);
TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_PRE_LOAD(uint32_t);
TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_PRE_LOAD(float);

#undef TEST_VECTOR_DATAMOVE_NDIM_COPY_GM2UB_PRE_LOAD
