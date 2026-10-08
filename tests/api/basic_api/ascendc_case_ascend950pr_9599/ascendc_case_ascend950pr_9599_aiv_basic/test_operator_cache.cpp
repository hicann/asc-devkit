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
#include <type_traits>
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "kernel_operator.h"

using namespace AscendC;

static_assert(sizeof(::BinaryMetaIcachePreloadFlag) == 8, "ICache preload TLV must occupy 8 bytes");
static_assert(
    std::is_same<decltype(::BinaryMetaIcachePreloadFlag::icachePreloadFlag), uint16_t>::value,
    "ICache preload flag must be uint16_t");
static_assert(
    std::is_same<decltype(::BinaryMetaIcachePreloadFlag::reserved), uint16_t>::value,
    "ICache preload reserved field must be uint16_t");
static_assert(
    offsetof(::BinaryMetaIcachePreloadFlag, icachePreloadFlag) == 4, "ICache preload flag must follow the TLV header");
static_assert(
    offsetof(::BinaryMetaIcachePreloadFlag, reserved) == 6, "ICache preload reserved field must follow the flag");

class TestCacheSuite : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(TestCacheSuite, DataCachePreloadTest)
{
    uint64_t src[10] = {0};
    int16_t cacheOffset = 10;
    EXPECT_NO_THROW(AscendC::DataCachePreloadImpl(src, cacheOffset));
}

TEST_F(TestCacheSuite, PreloadImplTest)
{
    int64_t preFetchLen = 2;
    void* pc = reinterpret_cast<void*>(0x1000);
    EXPECT_NO_THROW(AscendC::PreLoadImpl(pc, preFetchLen));
}

TEST_F(TestCacheSuite, GetICachePreloadStatusTest) { EXPECT_EQ(AscendC::GetICachePreloadStatusImpl(), 0); }

TEST_F(TestCacheSuite, PreloadTest)
{
    int64_t preFetchLen = 2;
    EXPECT_NO_THROW(AscendC::PreLoad(preFetchLen));
}

TEST_F(TestCacheSuite, ICachePreLoadTest) { EXPECT_NO_THROW(AscendC::ICachePreLoad(2)); }
