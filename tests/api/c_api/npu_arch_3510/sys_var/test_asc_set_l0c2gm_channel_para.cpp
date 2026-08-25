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
#include "c_api/stub/cce_stub.h"
#include "c_api/cube_datamove/cube_datamove.h"
#include "c_api/utils/sys_init.h"

class TestSysVarSetL0c2gmChannelPara : public testing::Test {
protected:
    void SetUp() { g_coreType = C_API_AIC_TYPE; }
    void TearDown() { g_coreType = C_API_AIV_TYPE; }
};

namespace {
void set_channel_para_stub(uint64_t config) { EXPECT_EQ(config, 135); }
void set_channel_para_multi_stub(uint64_t config) { EXPECT_EQ(config, static_cast<uint64_t>(5) << 48); }
} // namespace

TEST_F(TestSysVarSetL0c2gmChannelPara, c_api_get_set_l0c2gm_channel_para_Succ)
{
    int64_t input = 135;
    MOCKER_CPP(set_channel_para, void(uint64_t)).times(1).will(invoke(set_channel_para_stub));
    asc_set_l0c2gm_channel_para(input);
    asc_init();
    GlobalMockObject::verify();
}

TEST_F(TestSysVarSetL0c2gmChannelPara, c_api_set_l0c_copy_channel_para_Succ)
{
    uint16_t src_nz_matrix_stride = 5;
    MOCKER_CPP(set_channel_para, void(uint64_t)).times(1).will(invoke(set_channel_para_multi_stub));
    asc_set_l0c_copy_channel_para(src_nz_matrix_stride);
    asc_init();
    GlobalMockObject::verify();
}
