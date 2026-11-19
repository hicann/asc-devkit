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
#include <limits>
#include <mockcpp/mockcpp.hpp>
#include "c_api/stub/cce_stub.h"
#include "c_api/asc_simd.h"

namespace {
void MultiDataCachePreloadStub(__gm__ uint64_t* address, int64_t offset, uint16_t size_minus_one)
{
    EXPECT_EQ(reinterpret_cast<__gm__ uint64_t*>(1), address);
    EXPECT_EQ(static_cast<int64_t>(64), offset);
    EXPECT_EQ(static_cast<uint16_t>(3), size_minus_one);
}

void MultiDataCachePreloadOneLineStub(__gm__ uint64_t* address, int64_t offset, uint16_t size_minus_one)
{
    EXPECT_EQ(reinterpret_cast<__gm__ uint64_t*>(1), address);
    EXPECT_EQ(static_cast<int64_t>(64), offset);
    EXPECT_EQ(static_cast<uint16_t>(0), size_minus_one);
}

void MultiDataCachePreloadMaxLinesStub(__gm__ uint64_t* address, int64_t offset, uint16_t size_minus_one)
{
    EXPECT_EQ(reinterpret_cast<__gm__ uint64_t*>(1), address);
    EXPECT_EQ(static_cast<int64_t>(64), offset);
    EXPECT_EQ(std::numeric_limits<uint16_t>::max(), size_minus_one);
}
} // namespace

TEST(MultiDataCachePreload, MapsCachelineNumToSizeMinusOne)
{
    __gm__ uint64_t* address = reinterpret_cast<__gm__ uint64_t*>(1);
    MOCKER(dc_preload, void(__gm__ uint64_t*, int64_t, uint16_t)).times(1).will(invoke(MultiDataCachePreloadStub));

    asc_multi_datacache_preload(address, 64, 4);
    GlobalMockObject::verify();
}

TEST(MultiDataCachePreload, MapsOneCachelineToZeroBasedHardwareValue)
{
    __gm__ uint64_t* address = reinterpret_cast<__gm__ uint64_t*>(1);
    MOCKER(dc_preload, void(__gm__ uint64_t*, int64_t, uint16_t))
        .times(1)
        .will(invoke(MultiDataCachePreloadOneLineStub));

    asc_multi_datacache_preload(address, 64, 1);
    GlobalMockObject::verify();
}

TEST(MultiDataCachePreload, MapsMaximumCachelineCountToMaximumHardwareValue)
{
    __gm__ uint64_t* address = reinterpret_cast<__gm__ uint64_t*>(1);
    MOCKER(dc_preload, void(__gm__ uint64_t*, int64_t, uint16_t))
        .times(1)
        .will(invoke(MultiDataCachePreloadMaxLinesStub));

    asc_multi_datacache_preload(address, 64, 65536);
    GlobalMockObject::verify();
}
