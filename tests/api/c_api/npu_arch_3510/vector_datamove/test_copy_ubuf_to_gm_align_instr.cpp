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
#include "c_api/stub/cce_stub.h"
#include "c_api/asc_simd.h"

#define MOCK_COPY_UB2GM_ALIGN(cce_name, data_type)                                                                     \
    MOCKER_CPP(cce_name, void(__gm__ void*, __ubuf__ void*, uint8_t, uint32_t, uint32_t, uint8_t, uint64_t, uint32_t)) \
        .times(1)                                                                                                      \
        .will(invoke(cce_name##_##data_type##_Stub))

#define TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN(class_name, c_api_name, cce_name, data_type)                \
                                                                                                               \
    class TestVectorDatamove##class_name##_##data_type##_CApi : public testing::Test {                         \
    protected:                                                                                                 \
        void SetUp() {}                                                                                        \
        void TearDown() {}                                                                                     \
    };                                                                                                         \
                                                                                                               \
    namespace {                                                                                                \
                                                                                                               \
    void cce_name##_##data_type##_Stub(                                                                        \
        __gm__ void* dst, __ubuf__ void* src, uint8_t sid, uint32_t n_burst, uint32_t len_burst,               \
        uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)                                       \
    {                                                                                                          \
        EXPECT_EQ(dst, reinterpret_cast<__gm__ void*>(11));                                                    \
        EXPECT_EQ(src, reinterpret_cast<__ubuf__ void*>(22));                                                  \
        EXPECT_EQ(sid, static_cast<uint8_t>(0));                                                               \
        EXPECT_EQ(n_burst, static_cast<uint32_t>(1));                                                          \
        EXPECT_EQ(len_burst, static_cast<uint32_t>(1));                                                        \
        EXPECT_EQ(l2_cache_mode, static_cast<uint8_t>(0));                                                     \
        EXPECT_EQ(dst_stride, static_cast<uint64_t>(0));                                                       \
        EXPECT_EQ(src_stride, static_cast<uint32_t>(0));                                                       \
    }                                                                                                          \
    }                                                                                                          \
                                                                                                               \
    TEST_F(TestVectorDatamove##class_name##_##data_type##_CApi, c_api_name##_CopyConfig_Succ)                  \
    {                                                                                                          \
        __gm__ data_type* dst = reinterpret_cast<__gm__ data_type*>(11);                                       \
        __ubuf__ data_type* src = reinterpret_cast<__ubuf__ data_type*>(22);                                   \
        MOCK_COPY_UB2GM_ALIGN(cce_name, data_type);                                                            \
        c_api_name(                                                                                            \
            dst, src, static_cast<uint16_t>(1), static_cast<uint32_t>(1),                                      \
            asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, static_cast<uint64_t>(0), static_cast<uint32_t>(0)); \
        GlobalMockObject::verify();                                                                            \
    }                                                                                                          \
                                                                                                               \
    TEST_F(TestVectorDatamove##class_name##_##data_type##_CApi, c_api_name##_size_Succ)                        \
    {                                                                                                          \
        __gm__ data_type* dst = reinterpret_cast<__gm__ data_type*>(11);                                       \
        __ubuf__ data_type* src = reinterpret_cast<__ubuf__ data_type*>(22);                                   \
        MOCK_COPY_UB2GM_ALIGN(cce_name, data_type);                                                            \
        c_api_name(dst, src, static_cast<uint32_t>(1));                                                        \
        GlobalMockObject::verify();                                                                            \
    }                                                                                                          \
                                                                                                               \
    TEST_F(TestVectorDatamove##class_name##_##data_type##_CApi, c_api_name##_sync_Succ)                        \
    {                                                                                                          \
        __gm__ data_type* dst = reinterpret_cast<__gm__ data_type*>(11);                                       \
        __ubuf__ data_type* src = reinterpret_cast<__ubuf__ data_type*>(22);                                   \
        MOCK_COPY_UB2GM_ALIGN(cce_name, data_type);                                                            \
        c_api_name##_sync(dst, src, static_cast<uint32_t>(1));                                                 \
        GlobalMockObject::verify();                                                                            \
    }

TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN(CopyUB2GMALIGN, asc_copy_ub2gm_align, copy_ubuf_to_gm_align_v2, uint8_t);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN(CopyUB2GMALIGN, asc_copy_ub2gm_align, copy_ubuf_to_gm_align_v2, int8_t);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN(CopyUB2GMALIGN, asc_copy_ub2gm_align, copy_ubuf_to_gm_align_v2, uint16_t);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN(CopyUB2GMALIGN, asc_copy_ub2gm_align, copy_ubuf_to_gm_align_v2, int16_t);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN(CopyUB2GMALIGN, asc_copy_ub2gm_align, copy_ubuf_to_gm_align_v2, uint32_t);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN(CopyUB2GMALIGN, asc_copy_ub2gm_align, copy_ubuf_to_gm_align_v2, int32_t);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN(CopyUB2GMALIGN, asc_copy_ub2gm_align, copy_ubuf_to_gm_align_v2, half);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN(CopyUB2GMALIGN, asc_copy_ub2gm_align, copy_ubuf_to_gm_align_v2, float);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN(CopyUB2GMALIGN, asc_copy_ub2gm_align, copy_ubuf_to_gm_align_v2, hifloat8_t);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN(CopyUB2GMALIGN, asc_copy_ub2gm_align, copy_ubuf_to_gm_align_v2, bfloat16_t);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN(CopyUB2GMALIGN, asc_copy_ub2gm_align, copy_ubuf_to_gm_align_v2, fp8_e5m2_t);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN(
    CopyUB2GMALIGN, asc_copy_ub2gm_align, copy_ubuf_to_gm_align_v2, fp8_e4m3fn_t);

#undef MOCK_COPY_UB2GM_ALIGN
#undef TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN
