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

template <typename DTYPE>
__aicore__ inline void copy_gm_to_cbuf_multi_nd2nz_stub(
    __cbuf__ DTYPE* dst, __gm__ DTYPE* src, uint8_t sid, uint64_t loop1_src_stride, uint8_t l2_cache_ctl,
    uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride, bool smallc0_en)
{
    EXPECT_EQ(dst, reinterpret_cast<__cbuf__ DTYPE*>(11));
    EXPECT_EQ(src, reinterpret_cast<__gm__ DTYPE*>(22));
    EXPECT_EQ(sid, static_cast<uint8_t>(0));
    EXPECT_EQ(loop1_src_stride, static_cast<uint64_t>(33));
    EXPECT_EQ(l2_cache_ctl, static_cast<uint8_t>(4));
    EXPECT_EQ(n_value, static_cast<uint16_t>(55));
    EXPECT_EQ(d_value, static_cast<uint32_t>(66));
    EXPECT_EQ(loop4_src_stride, static_cast<uint64_t>(77));
    EXPECT_EQ(smallc0_en, static_cast<bool>(true));
}

class TEST_COPY_GM_TO_L1_ND2NZ : public testing::Test {
protected:
    void SetUp() { g_coreType = C_API_AIC_TYPE; }
    void TearDown() { g_coreType = C_API_AIV_TYPE; }
};

#define TEST_CUBE_DATAMOVE_COPY_GM2L1_ND2NZ(dtype)                                                                  \
                                                                                                                    \
    TEST_F(TEST_COPY_GM_TO_L1_ND2NZ, TEST_COPY_GM_TO_L1_ND2NZ_##dtype)                                              \
    {                                                                                                               \
        MOCKER_CPP(                                                                                                 \
            copy_gm_to_cbuf_multi_nd2nz,                                                                            \
            void(__cbuf__ dtype*, __gm__ dtype*, uint8_t, uint64_t, uint8_t, uint16_t, uint32_t, uint64_t, bool))   \
            .times(1)                                                                                               \
            .will(invoke(copy_gm_to_cbuf_multi_nd2nz_stub<dtype>));                                                 \
                                                                                                                    \
        __cbuf__ dtype* dst = reinterpret_cast<__cbuf__ dtype*>(11);                                                \
        __gm__ dtype* src = reinterpret_cast<__gm__ dtype*>(22);                                                    \
                                                                                                                    \
        uint64_t src_d_value = static_cast<uint64_t>(33);                                                           \
        asc_load_l2_cache_mode l2_cache_mode = asc_load_l2_cache_mode::NOTALLOC_KEEP;                               \
        uint16_t n_value = static_cast<uint16_t>(55);                                                               \
        uint32_t d_value = static_cast<uint32_t>(66);                                                               \
        uint64_t loop4_src_stride = static_cast<uint64_t>(77);                                                      \
        bool smallc0_en = static_cast<bool>(true);                                                                  \
                                                                                                                    \
        asc_copy_gm2l1_nd2nz(dst, src, src_d_value, l2_cache_mode, n_value, d_value, loop4_src_stride, smallc0_en); \
        GlobalMockObject::verify();                                                                                 \
    }

// ==========asc_copy_gm2l1_nd2nz==========
TEST_CUBE_DATAMOVE_COPY_GM2L1_ND2NZ(bfloat16_t);
TEST_CUBE_DATAMOVE_COPY_GM2L1_ND2NZ(half);
TEST_CUBE_DATAMOVE_COPY_GM2L1_ND2NZ(float);
TEST_CUBE_DATAMOVE_COPY_GM2L1_ND2NZ(float8_e4m3_t);
TEST_CUBE_DATAMOVE_COPY_GM2L1_ND2NZ(float8_e5m2_t);
TEST_CUBE_DATAMOVE_COPY_GM2L1_ND2NZ(int16_t);
TEST_CUBE_DATAMOVE_COPY_GM2L1_ND2NZ(int32_t);
TEST_CUBE_DATAMOVE_COPY_GM2L1_ND2NZ(int8_t);
TEST_CUBE_DATAMOVE_COPY_GM2L1_ND2NZ(uint16_t);
TEST_CUBE_DATAMOVE_COPY_GM2L1_ND2NZ(uint32_t);
TEST_CUBE_DATAMOVE_COPY_GM2L1_ND2NZ(uint8_t);
