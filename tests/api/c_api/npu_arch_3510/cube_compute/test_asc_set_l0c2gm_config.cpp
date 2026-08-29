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

class TestCubeComputeSetL0c2gmConfig : public testing::Test {
protected:
    void SetUp() { g_coreType = C_API_AIC_TYPE; }
    void TearDown() { g_coreType = C_API_AIV_TYPE; }
};

namespace {
void set_set_l0c2gm_config_Stub(uint64_t config) { EXPECT_EQ(0x8000000000000201, config); }
} // namespace

TEST_F(TestCubeComputeSetL0c2gmConfig, set_l0c2gm_config_Succ)
{
    MOCKER(set_fpc, void(uint64_t)).times(1).will(invoke(set_set_l0c2gm_config_Stub));
    uint64_t relu_pre = 1;
    uint64_t quant_pre = 2;
    bool enable_unit_flag = true;

    asc_set_l0c2gm_config(relu_pre, quant_pre, enable_unit_flag);
    GlobalMockObject::verify();
}

TEST_F(TestCubeComputeSetL0c2gmConfig, set_l0c_copy_config_Succ)
{
    MOCKER(set_fpc, void(uint64_t)).times(1).will(invoke(set_set_l0c2gm_config_Stub));
    uint64_t relu_pre_addr = 1;
    uint64_t quant_pre_addr = 2;
    bool is_clean_unit_flag = true;

    asc_set_l0c_copy_config(relu_pre_addr, quant_pre_addr, is_clean_unit_flag);
    GlobalMockObject::verify();
}
