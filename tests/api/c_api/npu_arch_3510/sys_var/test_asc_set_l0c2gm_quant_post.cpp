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
#include "tests/api/c_api/stub/cce_stub.h"
#include "c_api/cube_datamove/cube_datamove.h"
#include "c_api/utils/sys_init.h"

class TestSysVarSetL0c2gmQuantPost : public testing::Test {
protected:
    void SetUp() { g_coreType = C_API_AIC_TYPE; }
    void TearDown() { g_coreType = C_API_AIV_TYPE; }
};

void set_l0c2gm_quant_post_Stub(uint64_t config) { EXPECT_EQ(config, static_cast<uint64_t>(10)); }

TEST_F(TestSysVarSetL0c2gmQuantPost, c_api_set_l0c2gm_quant_post_Succ)
{
    int64_t input = 10;
    MOCKER_CPP(set_quant_post, void(uint64_t)).times(1).will(invoke(set_l0c2gm_quant_post_Stub));
    asc_set_l0c2gm_quant_post(input);
    asc_init();
    GlobalMockObject::verify();
}
