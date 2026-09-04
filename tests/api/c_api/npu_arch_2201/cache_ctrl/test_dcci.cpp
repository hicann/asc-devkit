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
#include "c_api/defs/union.h"
#include "c_api/defs/enum.h"

class TestAscCapiSimdDcci : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

namespace {
void dcci_single_Stub(__gm__ void* dst, uint64_t entire)
{
    EXPECT_EQ((__gm__ void*)3, dst);
    EXPECT_EQ(cache_line_t::SINGLE_CACHE_LINE, entire);
}

void dcci_single_ub_Stub(__ubuf__ void* dst, uint64_t entire)
{
    EXPECT_EQ((__ubuf__ void*)3, dst);
    EXPECT_EQ(cache_line_t::SINGLE_CACHE_LINE, entire);
}
} // namespace

TEST_F(TestAscCapiSimdDcci, dcci_gm_single)
{
    __gm__ void* dst = (__gm__ void*)3;
    MOCKER(dcci, void(__gm__ void*, uint64_t)).times(1).will(invoke(dcci_single_Stub));
#ifdef ASCENDC_CPU_DEBUG
    __asc_aicore::asc_dcci_single(dst);
#else
    asc_dcci_single(dst);
#endif
    GlobalMockObject ::verify();
}

TEST_F(TestAscCapiSimdDcci, dcci_ub_single)
{
    __ubuf__ void* dst = (__ubuf__ void*)3;
    MOCKER(dcci, void(__ubuf__ void*, uint64_t)).times(1).will(invoke(dcci_single_ub_Stub));
    asc_ub_dcci_single(dst);
    GlobalMockObject ::verify();
}

namespace {
void dcci_entire_all_Stub(__gm__ void* dst, uint64_t entire, uint64_t type)
{
    EXPECT_EQ((__gm__ void*)0, dst);
    EXPECT_EQ(cache_line_t::ENTIRE_DATA_CACHE, entire);
    EXPECT_EQ(dcci_dst_t::CACHELINE_ALL, type);
}
} // namespace

TEST_F(TestAscCapiSimdDcci, dcci_gm_entire_all)
{
    MOCKER(dcci, void(__gm__ void*, uint64_t, uint64_t)).times(1).will(invoke(dcci_entire_all_Stub));
    asc_dcci_entire_all();
    GlobalMockObject ::verify();
}

namespace {
void dcci_entire_out_Stub(__gm__ void* dst, uint64_t entire, uint64_t type)
{
    EXPECT_EQ((__gm__ void*)0, dst);
    EXPECT_EQ(cache_line_t::ENTIRE_DATA_CACHE, entire);
    EXPECT_EQ(dcci_dst_t::CACHELINE_OUT, type);
}
} // namespace

TEST_F(TestAscCapiSimdDcci, dcci_gm_entire_out)
{
    MOCKER(dcci, void(__gm__ void*, uint64_t, uint64_t)).times(1).will(invoke(dcci_entire_out_Stub));
    asc_dcci_entire_out();
    GlobalMockObject ::verify();
}

namespace {
void dcci_entire_ub_Stub(__gm__ void* dst, uint64_t entire, uint64_t type)
{
    EXPECT_EQ((__gm__ void*)0, dst);
    EXPECT_EQ(cache_line_t::ENTIRE_DATA_CACHE, entire);
    EXPECT_EQ(dcci_dst_t::CACHELINE_UB, type);
}
} // namespace

TEST_F(TestAscCapiSimdDcci, dcci_gm_entire_ub)
{
    MOCKER(dcci, void(__gm__ void*, uint64_t, uint64_t)).times(1).will(invoke(dcci_entire_ub_Stub));
    asc_dcci_entire_ub();
    GlobalMockObject ::verify();
}

namespace {
void dcci_entire_atomic_Stub(__gm__ void* dst, uint64_t entire, uint64_t type)
{
    EXPECT_EQ((__gm__ void*)0, dst);
    EXPECT_EQ(cache_line_t::ENTIRE_DATA_CACHE, entire);
    EXPECT_EQ(dcci_dst_t::CACHELINE_ATOMIC, type);
}
} // namespace

TEST_F(TestAscCapiSimdDcci, dcci_gm_entire_atomic)
{
    MOCKER(dcci, void(__gm__ void*, uint64_t, uint64_t)).times(1).will(invoke(dcci_entire_atomic_Stub));
    asc_dcci_entire_atomic();
    GlobalMockObject ::verify();
}
