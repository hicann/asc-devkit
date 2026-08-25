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
#include "tests/api/c_api/stub/cce_stub.h"
#include "include/c_api/asc_simd.h"

class TestScalarCacheModeCAPI : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

namespace {
int64_t g_ctrlValue = 0;

int64_t get_ctrl_stub() { return g_ctrlValue; }

void set_ctrl_stub(uint64_t config) { g_ctrlValue = static_cast<int64_t>(config); }
} // namespace

TEST_F(TestScalarCacheModeCAPI, c_api_set_scalar_load_cache_mode_normal_first_victim)
{
    g_ctrlValue = 0;
    MOCKER_CPP(get_ctrl, int64_t(void)).times(1).will(invoke(get_ctrl_stub));
    MOCKER_CPP(set_ctrl, void(uint64_t)).times(1).will(invoke(set_ctrl_stub));

    asc_set_scalar_cache_mode(asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM);

    uint8_t cacheBits = static_cast<uint8_t>((g_ctrlValue >> 16) & 0xF);
    EXPECT_EQ(cacheBits, static_cast<uint8_t>(asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM));
    GlobalMockObject::verify();
}

TEST_F(TestScalarCacheModeCAPI, c_api_set_scalar_load_cache_mode_normal_last_victim)
{
    g_ctrlValue = 0;
    MOCKER_CPP(get_ctrl, int64_t(void)).times(1).will(invoke(get_ctrl_stub));
    MOCKER_CPP(set_ctrl, void(uint64_t)).times(1).will(invoke(set_ctrl_stub));

    asc_set_scalar_cache_mode(asc_load_l2_cache_mode::NORMAL_LAST_VICTIM);

    uint8_t cacheBits = static_cast<uint8_t>((g_ctrlValue >> 16) & 0xF);
    EXPECT_EQ(cacheBits, static_cast<uint8_t>(asc_load_l2_cache_mode::NORMAL_LAST_VICTIM));
    GlobalMockObject::verify();
}

TEST_F(TestScalarCacheModeCAPI, c_api_set_scalar_load_cache_mode_normal_persistent)
{
    g_ctrlValue = 0;
    MOCKER_CPP(get_ctrl, int64_t(void)).times(1).will(invoke(get_ctrl_stub));
    MOCKER_CPP(set_ctrl, void(uint64_t)).times(1).will(invoke(set_ctrl_stub));

    asc_set_scalar_cache_mode(asc_load_l2_cache_mode::NORMAL_PERSISTENT);

    uint8_t cacheBits = static_cast<uint8_t>((g_ctrlValue >> 16) & 0xF);
    EXPECT_EQ(cacheBits, static_cast<uint8_t>(asc_load_l2_cache_mode::NORMAL_PERSISTENT));
    GlobalMockObject::verify();
}

TEST_F(TestScalarCacheModeCAPI, c_api_set_scalar_load_cache_mode_notalloc_keep)
{
    g_ctrlValue = 0;
    MOCKER_CPP(get_ctrl, int64_t(void)).times(1).will(invoke(get_ctrl_stub));
    MOCKER_CPP(set_ctrl, void(uint64_t)).times(1).will(invoke(set_ctrl_stub));

    asc_set_scalar_cache_mode(asc_load_l2_cache_mode::NOTALLOC_KEEP);

    uint8_t cacheBits = static_cast<uint8_t>((g_ctrlValue >> 16) & 0xF);
    EXPECT_EQ(cacheBits, static_cast<uint8_t>(asc_load_l2_cache_mode::NOTALLOC_KEEP));
    GlobalMockObject::verify();
}

TEST_F(TestScalarCacheModeCAPI, c_api_set_scalar_load_cache_mode_notalloc_clean)
{
    g_ctrlValue = 0;
    MOCKER_CPP(get_ctrl, int64_t(void)).times(1).will(invoke(get_ctrl_stub));
    MOCKER_CPP(set_ctrl, void(uint64_t)).times(1).will(invoke(set_ctrl_stub));

    asc_set_scalar_cache_mode(asc_load_l2_cache_mode::NOTALLOC_CLEAN);

    uint8_t cacheBits = static_cast<uint8_t>((g_ctrlValue >> 16) & 0xF);
    EXPECT_EQ(cacheBits, static_cast<uint8_t>(asc_load_l2_cache_mode::NOTALLOC_CLEAN));
    GlobalMockObject::verify();
}

TEST_F(TestScalarCacheModeCAPI, c_api_set_scalar_load_cache_mode_notalloc_drop)
{
    g_ctrlValue = 0;
    MOCKER_CPP(get_ctrl, int64_t(void)).times(1).will(invoke(get_ctrl_stub));
    MOCKER_CPP(set_ctrl, void(uint64_t)).times(1).will(invoke(set_ctrl_stub));

    asc_set_scalar_cache_mode(asc_load_l2_cache_mode::NOTALLOC_DROP);

    uint8_t cacheBits = static_cast<uint8_t>((g_ctrlValue >> 16) & 0xF);
    EXPECT_EQ(cacheBits, static_cast<uint8_t>(asc_load_l2_cache_mode::NOTALLOC_DROP));
    GlobalMockObject::verify();
}

TEST_F(TestScalarCacheModeCAPI, c_api_set_scalar_load_cache_mode_preserve_other_bits)
{
    g_ctrlValue = static_cast<int64_t>(0xDEAD) << 32;
    MOCKER_CPP(get_ctrl, int64_t(void)).times(1).will(invoke(get_ctrl_stub));
    MOCKER_CPP(set_ctrl, void(uint64_t)).times(1).will(invoke(set_ctrl_stub));

    asc_set_scalar_cache_mode(asc_load_l2_cache_mode::NORMAL_PERSISTENT);

    uint8_t cacheBits = static_cast<uint8_t>((g_ctrlValue >> 16) & 0xF);
    EXPECT_EQ(cacheBits, static_cast<uint8_t>(asc_load_l2_cache_mode::NORMAL_PERSISTENT));
    int64_t preservedBits = g_ctrlValue & (~(0xFLL << 16));
    EXPECT_EQ(preservedBits, static_cast<int64_t>(0xDEAD) << 32);
    GlobalMockObject::verify();
}

TEST_F(TestScalarCacheModeCAPI, c_api_set_scalar_store_cache_mode_normal_first_victim)
{
    g_ctrlValue = 0;
    MOCKER_CPP(get_ctrl, int64_t(void)).times(1).will(invoke(get_ctrl_stub));
    MOCKER_CPP(set_ctrl, void(uint64_t)).times(1).will(invoke(set_ctrl_stub));

    asc_set_scalar_cache_mode(asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM);

    uint8_t cacheBits = static_cast<uint8_t>((g_ctrlValue >> 20) & 0xF);
    EXPECT_EQ(cacheBits, static_cast<uint8_t>(asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM));
    GlobalMockObject::verify();
}

TEST_F(TestScalarCacheModeCAPI, c_api_set_scalar_store_cache_mode_normal_last_victim)
{
    g_ctrlValue = 0;
    MOCKER_CPP(get_ctrl, int64_t(void)).times(1).will(invoke(get_ctrl_stub));
    MOCKER_CPP(set_ctrl, void(uint64_t)).times(1).will(invoke(set_ctrl_stub));

    asc_set_scalar_cache_mode(asc_store_l2_cache_mode::NORMAL_LAST_VICTIM);

    uint8_t cacheBits = static_cast<uint8_t>((g_ctrlValue >> 20) & 0xF);
    EXPECT_EQ(cacheBits, static_cast<uint8_t>(asc_store_l2_cache_mode::NORMAL_LAST_VICTIM));
    GlobalMockObject::verify();
}

TEST_F(TestScalarCacheModeCAPI, c_api_set_scalar_store_cache_mode_normal_persistent)
{
    g_ctrlValue = 0;
    MOCKER_CPP(get_ctrl, int64_t(void)).times(1).will(invoke(get_ctrl_stub));
    MOCKER_CPP(set_ctrl, void(uint64_t)).times(1).will(invoke(set_ctrl_stub));

    asc_set_scalar_cache_mode(asc_store_l2_cache_mode::NORMAL_PERSISTENT);

    uint8_t cacheBits = static_cast<uint8_t>((g_ctrlValue >> 20) & 0xF);
    EXPECT_EQ(cacheBits, static_cast<uint8_t>(asc_store_l2_cache_mode::NORMAL_PERSISTENT));
    GlobalMockObject::verify();
}

TEST_F(TestScalarCacheModeCAPI, c_api_set_scalar_store_cache_mode_notalloc_clean)
{
    g_ctrlValue = 0;
    MOCKER_CPP(get_ctrl, int64_t(void)).times(1).will(invoke(get_ctrl_stub));
    MOCKER_CPP(set_ctrl, void(uint64_t)).times(1).will(invoke(set_ctrl_stub));

    asc_set_scalar_cache_mode(asc_store_l2_cache_mode::NOTALLOC_CLEAN);

    uint8_t cacheBits = static_cast<uint8_t>((g_ctrlValue >> 20) & 0xF);
    EXPECT_EQ(cacheBits, static_cast<uint8_t>(asc_store_l2_cache_mode::NOTALLOC_CLEAN));
    GlobalMockObject::verify();
}

TEST_F(TestScalarCacheModeCAPI, c_api_set_scalar_store_cache_mode_preserve_other_bits)
{
    g_ctrlValue = static_cast<int64_t>(0xDEAD) << 32;
    MOCKER_CPP(get_ctrl, int64_t(void)).times(1).will(invoke(get_ctrl_stub));
    MOCKER_CPP(set_ctrl, void(uint64_t)).times(1).will(invoke(set_ctrl_stub));

    asc_set_scalar_cache_mode(asc_store_l2_cache_mode::NORMAL_PERSISTENT);

    uint8_t cacheBits = static_cast<uint8_t>((g_ctrlValue >> 20) & 0xF);
    EXPECT_EQ(cacheBits, static_cast<uint8_t>(asc_store_l2_cache_mode::NORMAL_PERSISTENT));
    int64_t preservedBits = g_ctrlValue & (~(0xFLL << 20));
    EXPECT_EQ(preservedBits, static_cast<int64_t>(0xDEAD) << 32);
    GlobalMockObject::verify();
}

TEST_F(TestScalarCacheModeCAPI, c_api_get_scalar_load_cache_mode_normal_first_victim)
{
    g_ctrlValue = 0;
    MOCKER_CPP(get_ctrl, int64_t(void)).times(1).will(invoke(get_ctrl_stub));

    auto mode = asc_get_scalar_load_cache_mode();
    EXPECT_EQ(mode, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM);
    GlobalMockObject::verify();
}

TEST_F(TestScalarCacheModeCAPI, c_api_get_scalar_load_cache_mode_normal_last_victim)
{
    g_ctrlValue = static_cast<int64_t>(1) << 16;
    MOCKER_CPP(get_ctrl, int64_t(void)).times(1).will(invoke(get_ctrl_stub));

    auto mode = asc_get_scalar_load_cache_mode();
    EXPECT_EQ(mode, asc_load_l2_cache_mode::NORMAL_LAST_VICTIM);
    GlobalMockObject::verify();
}

TEST_F(TestScalarCacheModeCAPI, c_api_get_scalar_load_cache_mode_normal_persistent)
{
    g_ctrlValue = static_cast<int64_t>(2) << 16;
    MOCKER_CPP(get_ctrl, int64_t(void)).times(1).will(invoke(get_ctrl_stub));

    auto mode = asc_get_scalar_load_cache_mode();
    EXPECT_EQ(mode, asc_load_l2_cache_mode::NORMAL_PERSISTENT);
    GlobalMockObject::verify();
}

TEST_F(TestScalarCacheModeCAPI, c_api_get_scalar_load_cache_mode_notalloc_keep)
{
    g_ctrlValue = static_cast<int64_t>(4) << 16;
    MOCKER_CPP(get_ctrl, int64_t(void)).times(1).will(invoke(get_ctrl_stub));

    auto mode = asc_get_scalar_load_cache_mode();
    EXPECT_EQ(mode, asc_load_l2_cache_mode::NOTALLOC_KEEP);
    GlobalMockObject::verify();
}

TEST_F(TestScalarCacheModeCAPI, c_api_get_scalar_load_cache_mode_notalloc_clean)
{
    g_ctrlValue = static_cast<int64_t>(5) << 16;
    MOCKER_CPP(get_ctrl, int64_t(void)).times(1).will(invoke(get_ctrl_stub));

    auto mode = asc_get_scalar_load_cache_mode();
    EXPECT_EQ(mode, asc_load_l2_cache_mode::NOTALLOC_CLEAN);
    GlobalMockObject::verify();
}

TEST_F(TestScalarCacheModeCAPI, c_api_get_scalar_load_cache_mode_notalloc_drop)
{
    g_ctrlValue = static_cast<int64_t>(6) << 16;
    MOCKER_CPP(get_ctrl, int64_t(void)).times(1).will(invoke(get_ctrl_stub));

    auto mode = asc_get_scalar_load_cache_mode();
    EXPECT_EQ(mode, asc_load_l2_cache_mode::NOTALLOC_DROP);
    GlobalMockObject::verify();
}

TEST_F(TestScalarCacheModeCAPI, c_api_get_scalar_load_cache_mode_passthrough_illegal_encoding)
{
    g_ctrlValue = static_cast<int64_t>(3) << 16;
    MOCKER_CPP(get_ctrl, int64_t(void)).times(1).will(invoke(get_ctrl_stub));

    auto mode = asc_get_scalar_load_cache_mode();
    EXPECT_EQ(mode, static_cast<asc_load_l2_cache_mode>(3));
    GlobalMockObject::verify();
}

TEST_F(TestScalarCacheModeCAPI, c_api_get_scalar_store_cache_mode_normal_first_victim)
{
    g_ctrlValue = 0;
    MOCKER_CPP(get_ctrl, int64_t(void)).times(1).will(invoke(get_ctrl_stub));

    auto mode = asc_get_scalar_store_cache_mode();
    EXPECT_EQ(mode, asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM);
    GlobalMockObject::verify();
}

TEST_F(TestScalarCacheModeCAPI, c_api_get_scalar_store_cache_mode_normal_last_victim)
{
    g_ctrlValue = static_cast<int64_t>(1) << 20;
    MOCKER_CPP(get_ctrl, int64_t(void)).times(1).will(invoke(get_ctrl_stub));

    auto mode = asc_get_scalar_store_cache_mode();
    EXPECT_EQ(mode, asc_store_l2_cache_mode::NORMAL_LAST_VICTIM);
    GlobalMockObject::verify();
}

TEST_F(TestScalarCacheModeCAPI, c_api_get_scalar_store_cache_mode_normal_persistent)
{
    g_ctrlValue = static_cast<int64_t>(2) << 20;
    MOCKER_CPP(get_ctrl, int64_t(void)).times(1).will(invoke(get_ctrl_stub));

    auto mode = asc_get_scalar_store_cache_mode();
    EXPECT_EQ(mode, asc_store_l2_cache_mode::NORMAL_PERSISTENT);
    GlobalMockObject::verify();
}

TEST_F(TestScalarCacheModeCAPI, c_api_get_scalar_store_cache_mode_notalloc_clean)
{
    g_ctrlValue = static_cast<int64_t>(4) << 20;
    MOCKER_CPP(get_ctrl, int64_t(void)).times(1).will(invoke(get_ctrl_stub));

    auto mode = asc_get_scalar_store_cache_mode();
    EXPECT_EQ(mode, asc_store_l2_cache_mode::NOTALLOC_CLEAN);
    GlobalMockObject::verify();
}

TEST_F(TestScalarCacheModeCAPI, c_api_get_scalar_store_cache_mode_passthrough_illegal_encoding)
{
    g_ctrlValue = static_cast<int64_t>(3) << 20;
    MOCKER_CPP(get_ctrl, int64_t(void)).times(1).will(invoke(get_ctrl_stub));

    auto mode = asc_get_scalar_store_cache_mode();
    EXPECT_EQ(mode, static_cast<asc_store_l2_cache_mode>(3));
    GlobalMockObject::verify();
}

TEST_F(TestScalarCacheModeCAPI, c_api_set_and_get_scalar_load_cache_mode_roundtrip)
{
    g_ctrlValue = 0;
    MOCKER_CPP(get_ctrl, int64_t(void)).times(2).will(invoke(get_ctrl_stub));
    MOCKER_CPP(set_ctrl, void(uint64_t)).times(1).will(invoke(set_ctrl_stub));

    asc_set_scalar_cache_mode(asc_load_l2_cache_mode::NOTALLOC_DROP);
    auto mode = asc_get_scalar_load_cache_mode();
    EXPECT_EQ(mode, asc_load_l2_cache_mode::NOTALLOC_DROP);
    GlobalMockObject::verify();
}

TEST_F(TestScalarCacheModeCAPI, c_api_set_and_get_scalar_store_cache_mode_roundtrip)
{
    g_ctrlValue = 0;
    MOCKER_CPP(get_ctrl, int64_t(void)).times(2).will(invoke(get_ctrl_stub));
    MOCKER_CPP(set_ctrl, void(uint64_t)).times(1).will(invoke(set_ctrl_stub));

    asc_set_scalar_cache_mode(asc_store_l2_cache_mode::NOTALLOC_CLEAN);
    auto mode = asc_get_scalar_store_cache_mode();
    EXPECT_EQ(mode, asc_store_l2_cache_mode::NOTALLOC_CLEAN);
    GlobalMockObject::verify();
}

TEST_F(TestScalarCacheModeCAPI, c_api_set_load_and_store_cache_mode_independent)
{
    g_ctrlValue = 0;
    MOCKER_CPP(get_ctrl, int64_t(void)).times(4).will(invoke(get_ctrl_stub));
    MOCKER_CPP(set_ctrl, void(uint64_t)).times(2).will(invoke(set_ctrl_stub));

    asc_set_scalar_cache_mode(asc_load_l2_cache_mode::NORMAL_LAST_VICTIM);
    asc_set_scalar_cache_mode(asc_store_l2_cache_mode::NORMAL_PERSISTENT);

    auto loadMode = asc_get_scalar_load_cache_mode();
    auto storeMode = asc_get_scalar_store_cache_mode();
    EXPECT_EQ(loadMode, asc_load_l2_cache_mode::NORMAL_LAST_VICTIM);
    EXPECT_EQ(storeMode, asc_store_l2_cache_mode::NORMAL_PERSISTENT);
    GlobalMockObject::verify();
}
