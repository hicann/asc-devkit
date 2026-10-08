/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "c_api/stub/cce_stub.h"
#include "c_api/asc_simd.h"

class TestCacheCtrlIcachePreload3510 : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

class TestCacheCtrlIcachePreloadPrefetch3510 : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

namespace {
using IcachePreloadFlag = ::internal::asc_binary_meta_icache_preload_flag;
static_assert(sizeof(IcachePreloadFlag) == 8, "ICache preload TLV must occupy 8 bytes");
static_assert(
    std::is_same<decltype(IcachePreloadFlag::icache_preload_flag), uint16_t>::value,
    "ICache preload flag must be uint16_t");
static_assert(
    std::is_same<decltype(IcachePreloadFlag::reserved), uint16_t>::value,
    "ICache preload reserved field must be uint16_t");
static_assert(offsetof(IcachePreloadFlag, icache_preload_flag) == 4, "ICache preload flag must follow the TLV header");
static_assert(offsetof(IcachePreloadFlag, reserved) == 6, "ICache preload reserved field must follow the flag");

void icache_preload_prefetch_Stub(const void* addr, int64_t prefetchlen)
{
    int64_t len = 2;
    EXPECT_EQ(addr, reinterpret_cast<const void*>(32));
    EXPECT_EQ(prefetchlen, len);
}
} // namespace

TEST_F(TestCacheCtrlIcachePreloadPrefetch3510, icache_preload_voidptr_Succ)
{
    const void* ptr = reinterpret_cast<const void*>(32);
    int64_t len = 2;
    MOCKER(preload, void(const void*, int64_t)).times(1).will(invoke(icache_preload_prefetch_Stub));

    asc_icache_preload(ptr, len);
    GlobalMockObject::verify();
}
