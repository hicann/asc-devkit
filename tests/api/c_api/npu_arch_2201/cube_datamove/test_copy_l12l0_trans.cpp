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

class TestCubeDatamoveCopyL12l0Trans : public testing::Test {
protected:
    void SetUp() { g_coreType = C_API_AIC_TYPE; }
    void TearDown() { g_coreType = C_API_AIV_TYPE; }
};

#define TEST_CUBE_DATAMOVE_UNARY_SCALAR_INSTR(c_api_name, cce_name, data_type, mem_pos, sync_time)                \
    namespace {                                                                                                   \
    void cce_name##_##data_type##_##sync_time##_stub(                                                             \
        mem_pos data_type* dst, __cbuf__ data_type* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,  \
        uint16_t dst_gap, bool enable_addr_decrement, uint16_t frac_gap)                                          \
    {                                                                                                             \
        EXPECT_EQ(dst, reinterpret_cast<mem_pos data_type*>(11));                                                 \
        EXPECT_EQ(src, reinterpret_cast<__cbuf__ data_type*>(22));                                                \
        EXPECT_EQ(repeat, static_cast<uint8_t>(1));                                                               \
        EXPECT_EQ(index_id, static_cast<uint16_t>(1));                                                            \
        EXPECT_EQ(src_stride, static_cast<uint16_t>(1));                                                          \
        EXPECT_EQ(dst_gap, static_cast<uint16_t>(8));                                                             \
        EXPECT_EQ(frac_gap, static_cast<uint16_t>(8));                                                            \
        EXPECT_EQ(enable_addr_decrement, false);                                                                  \
    }                                                                                                             \
    }                                                                                                             \
    TEST_F(TestCubeDatamoveCopyL12l0Trans, c_api_name##_##data_type##_##sync_time##_Succ)                         \
    {                                                                                                             \
        mem_pos data_type* dst = reinterpret_cast<mem_pos data_type*>(11);                                        \
        __cbuf__ data_type* src = reinterpret_cast<__cbuf__ data_type*>(22);                                      \
                                                                                                                  \
        uint16_t index_id = 1;                                                                                    \
        uint16_t src_stride = 1;                                                                                  \
        uint16_t dst_gap = 8;                                                                                     \
        uint16_t frac_gap = 8;                                                                                    \
        uint8_t repeat = 1;                                                                                       \
        bool enable_addr_decrement = false;                                                                       \
        MOCKER_CPP(pipe_barrier, void(pipe_t)).times(sync_time).will(invoke(+[](pipe_t pipe) {                    \
            EXPECT_EQ(static_cast<int>(pipe), static_cast<int>(pipe_t::PIPE_ALL));                                \
        }));                                                                                                      \
        MOCKER_CPP(                                                                                               \
            cce_name,                                                                                             \
            void(mem_pos data_type*, __cbuf__ data_type*, uint16_t, uint8_t, uint16_t, uint16_t, bool, uint16_t)) \
            .times(1)                                                                                             \
            .will(invoke(cce_name##_##data_type##_##sync_time##_stub));                                           \
                                                                                                                  \
        c_api_name(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, frac_gap);             \
        GlobalMockObject::verify();                                                                               \
    }

// ==========asc_copy_l12l0b_trans(half/float/int32_t/int8_t/uint32_t/uint8_t)==========
TEST_CUBE_DATAMOVE_UNARY_SCALAR_INSTR(asc_copy_l12l0b_trans, load_cbuf_to_cb_transpose, half, __cb__, 0);
TEST_CUBE_DATAMOVE_UNARY_SCALAR_INSTR(asc_copy_l12l0b_trans, load_cbuf_to_cb_transpose, float, __cb__, 0);
TEST_CUBE_DATAMOVE_UNARY_SCALAR_INSTR(asc_copy_l12l0b_trans, load_cbuf_to_cb_transpose, int32_t, __cb__, 0);
TEST_CUBE_DATAMOVE_UNARY_SCALAR_INSTR(asc_copy_l12l0b_trans, load_cbuf_to_cb_transpose, int8_t, __cb__, 0);
TEST_CUBE_DATAMOVE_UNARY_SCALAR_INSTR(asc_copy_l12l0b_trans, load_cbuf_to_cb_transpose, uint32_t, __cb__, 0);
TEST_CUBE_DATAMOVE_UNARY_SCALAR_INSTR(asc_copy_l12l0b_trans, load_cbuf_to_cb_transpose, uint8_t, __cb__, 0);

TEST_CUBE_DATAMOVE_UNARY_SCALAR_INSTR(asc_copy_l12l0a_trans, load_cbuf_to_ca_transpose, half, __cb__, 0);
TEST_CUBE_DATAMOVE_UNARY_SCALAR_INSTR(asc_copy_l12l0a_trans, load_cbuf_to_ca_transpose, float, __cb__, 0);
TEST_CUBE_DATAMOVE_UNARY_SCALAR_INSTR(asc_copy_l12l0a_trans, load_cbuf_to_ca_transpose, int32_t, __cb__, 0);
TEST_CUBE_DATAMOVE_UNARY_SCALAR_INSTR(asc_copy_l12l0a_trans, load_cbuf_to_ca_transpose, int8_t, __cb__, 0);
TEST_CUBE_DATAMOVE_UNARY_SCALAR_INSTR(asc_copy_l12l0a_trans, load_cbuf_to_ca_transpose, uint32_t, __cb__, 0);
TEST_CUBE_DATAMOVE_UNARY_SCALAR_INSTR(asc_copy_l12l0a_trans, load_cbuf_to_ca_transpose, uint8_t, __cb__, 0);
TEST_CUBE_DATAMOVE_UNARY_SCALAR_INSTR(asc_copy_l12l0a_trans, load_cbuf_to_ca_transpose, bfloat16_t, __cb__, 0);

TEST_CUBE_DATAMOVE_UNARY_SCALAR_INSTR(asc_copy_l12l0a_trans_sync, load_cbuf_to_ca_transpose, half, __cb__, 1);
TEST_CUBE_DATAMOVE_UNARY_SCALAR_INSTR(asc_copy_l12l0a_trans_sync, load_cbuf_to_ca_transpose, float, __cb__, 1);
TEST_CUBE_DATAMOVE_UNARY_SCALAR_INSTR(asc_copy_l12l0a_trans_sync, load_cbuf_to_ca_transpose, int32_t, __cb__, 1);
TEST_CUBE_DATAMOVE_UNARY_SCALAR_INSTR(asc_copy_l12l0a_trans_sync, load_cbuf_to_ca_transpose, int8_t, __cb__, 1);
TEST_CUBE_DATAMOVE_UNARY_SCALAR_INSTR(asc_copy_l12l0a_trans_sync, load_cbuf_to_ca_transpose, uint32_t, __cb__, 1);
TEST_CUBE_DATAMOVE_UNARY_SCALAR_INSTR(asc_copy_l12l0a_trans_sync, load_cbuf_to_ca_transpose, uint8_t, __cb__, 1);
TEST_CUBE_DATAMOVE_UNARY_SCALAR_INSTR(asc_copy_l12l0a_trans_sync, load_cbuf_to_ca_transpose, bfloat16_t, __cb__, 1);
