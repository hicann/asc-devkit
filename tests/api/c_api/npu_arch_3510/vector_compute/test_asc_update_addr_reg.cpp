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

namespace {

constexpr uint32_t OFFSET0 = 32;
constexpr uint32_t OFFSET1 = 64;
constexpr uint32_t OFFSET2 = 96;
constexpr uint32_t OFFSET3 = 128;

#define DEFINE_VAG_STUBS(cce_name)                                                                     \
    addr_reg cce_name##_stub_1(uint32_t offset)                                                        \
    {                                                                                                  \
        EXPECT_EQ(OFFSET0, offset);                                                                    \
        return addr_reg{};                                                                             \
    }                                                                                                  \
    addr_reg cce_name##_stub_2(uint32_t offset0, uint32_t offset1)                                     \
    {                                                                                                  \
        EXPECT_EQ(OFFSET0, offset0);                                                                   \
        EXPECT_EQ(OFFSET1, offset1);                                                                   \
        return addr_reg{};                                                                             \
    }                                                                                                  \
    addr_reg cce_name##_stub_3(uint32_t offset0, uint32_t offset1, uint32_t offset2)                   \
    {                                                                                                  \
        EXPECT_EQ(OFFSET0, offset0);                                                                   \
        EXPECT_EQ(OFFSET1, offset1);                                                                   \
        EXPECT_EQ(OFFSET2, offset2);                                                                   \
        return addr_reg{};                                                                             \
    }                                                                                                  \
    addr_reg cce_name##_stub_4(uint32_t offset0, uint32_t offset1, uint32_t offset2, uint32_t offset3) \
    {                                                                                                  \
        EXPECT_EQ(OFFSET0, offset0);                                                                   \
        EXPECT_EQ(OFFSET1, offset1);                                                                   \
        EXPECT_EQ(OFFSET2, offset2);                                                                   \
        EXPECT_EQ(OFFSET3, offset3);                                                                   \
        return addr_reg{};                                                                             \
    }

DEFINE_VAG_STUBS(vag_b8)
DEFINE_VAG_STUBS(vag_b16)
DEFINE_VAG_STUBS(vag_b32)

#undef DEFINE_VAG_STUBS

} // namespace

#define TEST_UPDATE_ADDR_REG_AIV(bxx, cce_name)                                                                \
    class TestUpdateAddrReg##bxx : public testing::Test {                                                      \
    protected:                                                                                                 \
        void SetUp() {}                                                                                        \
        void TearDown() {}                                                                                     \
    };                                                                                                         \
                                                                                                               \
    TEST_F(TestUpdateAddrReg##bxx, asc_update_addr_reg_##bxx##_1param_Succ)                                    \
    {                                                                                                          \
        MOCKER_CPP(cce_name, addr_reg(uint32_t)).times(1).will(invoke(cce_name##_stub_1));                     \
        asc_update_addr_reg_##bxx(OFFSET0);                                                                    \
        GlobalMockObject::verify();                                                                            \
    }                                                                                                          \
                                                                                                               \
    TEST_F(TestUpdateAddrReg##bxx, asc_update_addr_reg_##bxx##_2param_Succ)                                    \
    {                                                                                                          \
        MOCKER_CPP(cce_name, addr_reg(uint32_t, uint32_t)).times(1).will(invoke(cce_name##_stub_2));           \
        asc_update_addr_reg_##bxx(OFFSET0, OFFSET1);                                                           \
        GlobalMockObject::verify();                                                                            \
    }                                                                                                          \
                                                                                                               \
    TEST_F(TestUpdateAddrReg##bxx, asc_update_addr_reg_##bxx##_3param_Succ)                                    \
    {                                                                                                          \
        MOCKER_CPP(cce_name, addr_reg(uint32_t, uint32_t, uint32_t)).times(1).will(invoke(cce_name##_stub_3)); \
        asc_update_addr_reg_##bxx(OFFSET0, OFFSET1, OFFSET2);                                                  \
        GlobalMockObject::verify();                                                                            \
    }                                                                                                          \
                                                                                                               \
    TEST_F(TestUpdateAddrReg##bxx, asc_update_addr_reg_##bxx##_4param_Succ)                                    \
    {                                                                                                          \
        MOCKER_CPP(cce_name, addr_reg(uint32_t, uint32_t, uint32_t, uint32_t))                                 \
            .times(1)                                                                                          \
            .will(invoke(cce_name##_stub_4));                                                                  \
        asc_update_addr_reg_##bxx(OFFSET0, OFFSET1, OFFSET2, OFFSET3);                                         \
        GlobalMockObject::verify();                                                                            \
    }

TEST_UPDATE_ADDR_REG_AIV(b8, vag_b8)
TEST_UPDATE_ADDR_REG_AIV(b16, vag_b16)
TEST_UPDATE_ADDR_REG_AIV(b32, vag_b32)

#undef TEST_UPDATE_ADDR_REG_AIV
