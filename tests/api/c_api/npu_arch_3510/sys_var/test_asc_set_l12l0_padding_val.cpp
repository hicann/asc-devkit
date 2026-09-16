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

class TestSysVarSetL12L0PaddingVal : public testing::Test {
protected:
    void SetUp() { g_coreType = C_API_AIC_TYPE; }
    void TearDown() { g_coreType = C_API_AIV_TYPE; }
};

#define TEST_NEW_SYS_VAR_SET_L12L0B_PADDING(data_type)                  \
    TEST(TestSysVarSetL12L0BPadding, c_api_set_new_padding_##data_type) \
    {                                                                   \
        g_coreType = C_API_AIC_TYPE;                                    \
        data_type input = 33;                                           \
        MOCKER_CPP(set_padding_b, void(uint64_t)).times(1);             \
        asc_set_l12l0b_3d_padding(input);                               \
        asc_init();                                                     \
        GlobalMockObject::verify();                                     \
        g_coreType = C_API_AIV_TYPE;                                    \
    }

TEST_NEW_SYS_VAR_SET_L12L0B_PADDING(uint64_t);
TEST_NEW_SYS_VAR_SET_L12L0B_PADDING(int8_t);
TEST_NEW_SYS_VAR_SET_L12L0B_PADDING(uint8_t);
TEST_NEW_SYS_VAR_SET_L12L0B_PADDING(int16_t);
TEST_NEW_SYS_VAR_SET_L12L0B_PADDING(uint16_t);
TEST_NEW_SYS_VAR_SET_L12L0B_PADDING(half);
TEST_NEW_SYS_VAR_SET_L12L0B_PADDING(bfloat16_t);
TEST_NEW_SYS_VAR_SET_L12L0B_PADDING(int32_t);
TEST_NEW_SYS_VAR_SET_L12L0B_PADDING(uint32_t);
TEST_NEW_SYS_VAR_SET_L12L0B_PADDING(float);
