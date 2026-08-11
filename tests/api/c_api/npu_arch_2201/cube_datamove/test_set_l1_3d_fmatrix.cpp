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

class TestCubeDmamoveSetL13DFmatrix : public testing::Test {
protected:
    void SetUp() { g_coreType = C_API_AIC_TYPE; }
    void TearDown() { g_coreType = C_API_AIV_TYPE; }
};

namespace {
void set_fmatrix_stub(uint64_t config) { EXPECT_EQ(0x0605040300010002, config); }
void set_fmatrix_b_stub(uint64_t config) { EXPECT_EQ(0x0605040300010002, config); }
} // namespace

TEST_F(TestCubeDmamoveSetL13DFmatrix, set_l1_3d_fmatrix_uint32_t_Succ)
{
    asc_l13d_fmatrix_config config;
    config.l1_height = 1;
    config.l1_width = 2;
    config.padding_left_size = 3;
    config.padding_right_size = 4;
    config.padding_top_size = 5;
    config.padding_bottom_size = 6;

    MOCKER(set_fmatrix, void(uint64_t)).times(1).will(invoke(set_fmatrix_stub));

    asc_set_l13d_fmatrix(config);
    GlobalMockObject::verify();
}

class TestCubeDmamoveSetL13DFmatrixB : public testing::Test {
protected:
    void SetUp() { g_coreType = C_API_AIC_TYPE; }
    void TearDown() { g_coreType = C_API_AIV_TYPE; }
};

TEST_F(TestCubeDmamoveSetL13DFmatrixB, asc_set_l13d_fmatrix_b_Succ)
{
    asc_l13d_fmatrix_config config;
    config.l1_height = 1;
    config.l1_width = 2;
    config.padding_left_size = 3;
    config.padding_right_size = 4;
    config.padding_top_size = 5;
    config.padding_bottom_size = 6;

    MOCKER(set_fmatrix_b, void(uint64_t)).times(1).will(invoke(set_fmatrix_b_stub));

    asc_set_l13d_fmatrix_b(config);
    GlobalMockObject::verify();
}
