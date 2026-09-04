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

class TestVectorComputeGetReduceMaxMinCnt : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(TestVectorComputeGetReduceMaxMinCnt, get_max_min_cnt_half_int32_t_Succ)
{
    half val = 0.0;
    uint32_t index = 1;
    MOCKER(get_max_min_cnt, int64_t(void)).times(2).will(returnValue(int64_t(0x00000000FFFF3C00)));

    asc_get_reduce_max_cnt(val, index);
    EXPECT_FLOAT_EQ(val, (float)1.0);
    EXPECT_EQ(index, 0);
    asc_get_reduce_min_cnt(val, index);
    EXPECT_FLOAT_EQ(val, (float)1.0);
    EXPECT_EQ(index, 0);
    GlobalMockObject::verify();
}

TEST_F(TestVectorComputeGetReduceMaxMinCnt, get_max_min_cnt_float_int32_t_Succ)
{
    float val = 0.0;
    uint32_t index = 1;
    MOCKER(get_max_min_cnt, int64_t(void)).times(2).will(returnValue(int64_t(0x000000003F800000)));

    asc_get_reduce_max_cnt(val, index);
    EXPECT_FLOAT_EQ(val, (float)1.0);
    EXPECT_EQ(index, 0);
    asc_get_reduce_min_cnt(val, index);
    EXPECT_FLOAT_EQ(val, (float)1.0);
    EXPECT_EQ(index, 0);
    GlobalMockObject::verify();
}
