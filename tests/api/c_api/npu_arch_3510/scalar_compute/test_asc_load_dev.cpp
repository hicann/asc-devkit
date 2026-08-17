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
#include <type_traits>
#include "tests/api/c_api/stub/cce_stub.h"
#include "include/c_api/asc_simd.h"

int64_t ld_dev(__gm__ int64_t*, int16_t) { return 0; }

#define TEST_ASC_LOAD_DEV(data_type, cce_type, raw_value, expected_value)                                         \
    namespace {                                                                                                   \
    uint64_t ld_dev_##data_type##_stub(__gm__ cce_type* addr, int16_t offset)                                     \
    {                                                                                                             \
        EXPECT_EQ(offset, 0);                                                                                     \
        return static_cast<uint64_t>(raw_value);                                                                  \
    }                                                                                                             \
    }                                                                                                             \
                                                                                                                  \
    TEST(AscLoadDev, data_type)                                                                                   \
    {                                                                                                             \
        __gm__ data_type* addr = nullptr;                                                                         \
        static_assert(std::is_same_v<decltype(asc_load_dev(addr)), data_type>);                                   \
        MOCKER_CPP(ld_dev, uint64_t(__gm__ cce_type*, int16_t)).times(1).will(invoke(ld_dev_##data_type##_stub)); \
        EXPECT_EQ(asc_load_dev(addr), static_cast<data_type>(expected_value));                                    \
        GlobalMockObject::verify();                                                                               \
    }

TEST_ASC_LOAD_DEV(int8_t, uint8_t, UINT8_MAX, -1)
TEST_ASC_LOAD_DEV(uint8_t, uint8_t, 1, 1)
TEST_ASC_LOAD_DEV(int16_t, uint16_t, UINT16_MAX, -1)
TEST_ASC_LOAD_DEV(uint16_t, uint16_t, 2, 2)
TEST_ASC_LOAD_DEV(int32_t, uint32_t, UINT32_MAX, -1)
TEST_ASC_LOAD_DEV(uint32_t, uint32_t, 3, 3)
TEST_ASC_LOAD_DEV(uint64_t, uint64_t, 4, 4)

namespace {
int64_t ld_dev_int64_t_stub(__gm__ int64_t* addr, int16_t offset)
{
    EXPECT_EQ(offset, 0);
    return -1;
}
} // namespace

TEST(AscLoadDev, int64_t)
{
    __gm__ int64_t* addr = nullptr;
    static_assert(std::is_same_v<decltype(asc_load_dev(addr)), int64_t>);
    MOCKER_CPP(ld_dev, int64_t(__gm__ int64_t*, int16_t)).times(1).will(invoke(ld_dev_int64_t_stub));
    EXPECT_EQ(asc_load_dev(addr), -1);
    GlobalMockObject::verify();
}

#undef TEST_ASC_LOAD_DEV
