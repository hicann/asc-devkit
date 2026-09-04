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

class TestCacheCtrlIcachePreload : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

class TestCacheCtrlIcachePreloadPrefetch : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

namespace {
void icache_preload_prefetch_Stub(const void* addr, int64_t prefetchlen)
{
    int64_t len = 2;
    EXPECT_EQ(addr, reinterpret_cast<const void*>(32));
    EXPECT_EQ(prefetchlen, reinterpret_cast<int64_t>(len));
}
} // namespace

TEST_F(TestCacheCtrlIcachePreloadPrefetch, icache_preload_voidptr_Succ)
{
    const void* ptr = reinterpret_cast<const void*>(32);
    int64_t len = 2;
    MOCKER(asc_icache_preload, void(const void*, int64_t)).times(1).will(invoke(icache_preload_prefetch_Stub));

    asc_icache_preload(ptr, len);
    GlobalMockObject::verify();
}
