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

class TestCubeDmamoveSetL13DRpt : public testing::Test {
protected:
    void SetUp() { g_coreType = C_API_AIC_TYPE; }
    void TearDown() { g_coreType = C_API_AIV_TYPE; }
};

TEST_F(TestCubeDmamoveSetL13DRpt, set_l13d_rpt_uint32_t_Succ)
{
    asc_load3d_v2_config config;
    asc_set_l13d_rpt(config);
    EXPECT_EQ(config.rpt_stride, 0);
    EXPECT_EQ(config.rpt_time, 1);
    EXPECT_EQ(config.rpt_mode, 0);
    GlobalMockObject::verify();
}

namespace {
void set_l3d_rpt_params_stub(uint64_t config)
{
    uint64_t expected = (static_cast<uint64_t>(5) << 48) | (static_cast<uint64_t>(4) << 32) |
                        (static_cast<uint64_t>(1) << 24) | (static_cast<uint64_t>(3) << 16) | 2;
    EXPECT_EQ(config, expected);
}
} // namespace

TEST_F(TestCubeDmamoveSetL13DRpt, set_l13d_rpt_params_Succ)
{
    MOCKER(set_l3d_rpt, void(uint64_t)).times(1).will(invoke(set_l3d_rpt_params_stub));
    asc_set_l13d_rpt(2, 3, asc_l13d_repeat_direction::K_DIRECTION, 4, 5);
    GlobalMockObject::verify();
}
