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
#include "c_api/cube_datamove/cube_datamove.h"

class TestCubeComputeSetL13DPaddingValue : public testing::Test {
protected:
    void SetUp() { g_coreType = C_API_AIC_TYPE; }
    void TearDown() { g_coreType = C_API_AIV_TYPE; }
};

TEST_F(TestCubeComputeSetL13DPaddingValue, set_l13d_padding_uint64_t_Succ)
{
    MOCKER(set_padding, void(uint64_t)).times(1);

    uint64_t config = 1;
    asc_set_l13d_padding(config);

    GlobalMockObject::verify();
}
TEST_F(TestCubeComputeSetL13DPaddingValue, set_l13d_padding_half_Succ)
{
    MOCKER(set_padding, void(uint64_t)).times(1);

    uint64_t config = 1;
    asc_set_l13d_padding(config);

    GlobalMockObject::verify();
}

TEST_F(TestCubeComputeSetL13DPaddingValue, set_l13d_padding_int16_t_Succ)
{
    MOCKER(set_padding, void(uint64_t)).times(1);

    int16_t config = 1;
    asc_set_l13d_padding(config);

    GlobalMockObject::verify();
}

TEST_F(TestCubeComputeSetL13DPaddingValue, set_l13d_padding_uint16_t_Succ)
{
    MOCKER(set_padding, void(uint64_t)).times(1);

    uint16_t config = 1;
    asc_set_l13d_padding(config);

    GlobalMockObject::verify();
}
