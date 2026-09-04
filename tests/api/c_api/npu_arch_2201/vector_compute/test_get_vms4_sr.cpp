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

class TestVectorComputeGetVms4Sr : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(TestVectorComputeGetVms4Sr, get_vms4_sr_uint16_t_array_Succ)
{
    uint16_t sortedNum[4] = {1};
    MOCKER(get_vms4_sr, int64_t(void)).times(1).will(returnValue(int64_t(0x4444333322221111LL)));
    asc_get_vms4_sr(sortedNum);
    uint16_t val0 = sortedNum[0];
    uint16_t val1 = sortedNum[1];
    uint16_t val2 = sortedNum[2];
    uint16_t val3 = sortedNum[3];
    EXPECT_EQ(4369, val0);
    EXPECT_EQ(8738, val1);
    EXPECT_EQ(13107, val2);
    EXPECT_EQ(17476, val3);
    GlobalMockObject::verify();
}
