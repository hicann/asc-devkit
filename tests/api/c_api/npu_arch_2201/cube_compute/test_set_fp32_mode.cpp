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

class TestSetFP32ModeCAPI : public testing::Test {
protected:
    void SetUp() { g_coreType = C_API_AIC_TYPE; }
    void TearDown() { g_coreType = C_API_AIV_TYPE; }
};

namespace {

int64_t get_ctrl_Stub() { return 0x12345678; }

} // namespace

TEST_F(TestSetFP32ModeCAPI, c_api_SetFP32Mode_Success)
{
    MOCKER_CPP(get_ctrl, int64_t()).times(1).will(invoke(get_ctrl_Stub));

    MOCKER_CPP(set_ctrl, void(uint64_t)).times(1);

    asc_set_fp32_mode();
    GlobalMockObject::verify();
}

TEST_F(TestSetFP32ModeCAPI, c_api_DisableHF32_Success)
{
    MOCKER_CPP(get_ctrl, int64_t()).times(1).will(invoke(get_ctrl_Stub));

    MOCKER_CPP(set_ctrl, void(uint64_t)).times(1);

    asc_disable_hf32();
    GlobalMockObject::verify();
}
