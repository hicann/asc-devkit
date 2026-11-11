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
#include "c_api/stub/cce_stub.h"
#include "c_api/asc_simd.h"

#define RESET_COPY_UB2GM_ALIGN() reset_copy_ubuf_to_gm_align_v2_stub_state()

#define VERIFY_COPY_UB2GM_ALIGN()                                          \
    EXPECT_EQ(g_copy_ubuf_to_gm_align_v2_stub_state.call_count, 1U);       \
    EXPECT_FALSE(g_copy_ubuf_to_gm_align_v2_stub_state.rsw_ctrl);          \
    EXPECT_FALSE(g_copy_ubuf_to_gm_align_v2_stub_state.rsw_packet_ctrl);   \
    EXPECT_EQ(g_copy_ubuf_to_gm_align_v2_stub_state.rsw_buffset_size, 0U); \
    EXPECT_EQ(g_copy_ubuf_to_gm_align_v2_stub_state.l2_cache_mode, 0U);    \
    EXPECT_EQ(g_copy_ubuf_to_gm_align_v2_stub_state.dst_stride, 0U);       \
    EXPECT_EQ(g_copy_ubuf_to_gm_align_v2_stub_state.src_stride, 0U);       \
    EXPECT_FALSE(g_copy_ubuf_to_gm_align_v2_stub_state.non_eod_ctrl)

#define TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN(class_name, c_api_name, data_type)                          \
                                                                                                               \
    class TestVectorDatamove##class_name##_##data_type##_CApi : public testing::Test {                         \
    protected:                                                                                                 \
        void SetUp() {}                                                                                        \
        void TearDown() {}                                                                                     \
    };                                                                                                         \
                                                                                                               \
    TEST_F(TestVectorDatamove##class_name##_##data_type##_CApi, c_api_name##_CopyConfig_Succ)                  \
    {                                                                                                          \
        __gm__ data_type* dst = reinterpret_cast<__gm__ data_type*>(11);                                       \
        __ubuf__ data_type* src = reinterpret_cast<__ubuf__ data_type*>(22);                                   \
        RESET_COPY_UB2GM_ALIGN();                                                                              \
        c_api_name(                                                                                            \
            dst, src, static_cast<uint32_t>(1), static_cast<uint32_t>(1),                                      \
            asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, static_cast<uint64_t>(0), static_cast<uint32_t>(0)); \
        VERIFY_COPY_UB2GM_ALIGN();                                                                             \
    }                                                                                                          \
                                                                                                               \
    TEST_F(TestVectorDatamove##class_name##_##data_type##_CApi, c_api_name##_size_Succ)                        \
    {                                                                                                          \
        __gm__ data_type* dst = reinterpret_cast<__gm__ data_type*>(11);                                       \
        __ubuf__ data_type* src = reinterpret_cast<__ubuf__ data_type*>(22);                                   \
        RESET_COPY_UB2GM_ALIGN();                                                                              \
        c_api_name(dst, src, static_cast<uint32_t>(1));                                                        \
        VERIFY_COPY_UB2GM_ALIGN();                                                                             \
    }

TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN(CopyUB2GMALIGN, asc_copy_ub2gm_align, uint8_t);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN(CopyUB2GMALIGN, asc_copy_ub2gm_align, int8_t);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN(CopyUB2GMALIGN, asc_copy_ub2gm_align, uint16_t);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN(CopyUB2GMALIGN, asc_copy_ub2gm_align, int16_t);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN(CopyUB2GMALIGN, asc_copy_ub2gm_align, uint32_t);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN(CopyUB2GMALIGN, asc_copy_ub2gm_align, int32_t);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN(CopyUB2GMALIGN, asc_copy_ub2gm_align, half);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN(CopyUB2GMALIGN, asc_copy_ub2gm_align, float);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN(CopyUB2GMALIGN, asc_copy_ub2gm_align, hifloat8_t);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN(CopyUB2GMALIGN, asc_copy_ub2gm_align, bfloat16_t);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN(CopyUB2GMALIGN, asc_copy_ub2gm_align, fp8_e5m2_t);
TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN(CopyUB2GMALIGN, asc_copy_ub2gm_align, fp8_e4m3fn_t);

#undef RESET_COPY_UB2GM_ALIGN
#undef VERIFY_COPY_UB2GM_ALIGN
#undef TEST_VECTOR_DATAMOVE_COPY_UBUF_TO_GM_ALIGN
