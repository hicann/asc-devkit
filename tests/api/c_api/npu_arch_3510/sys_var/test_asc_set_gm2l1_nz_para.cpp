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

class TestSysVarSetGm2l1NzPara : public testing::Test {
protected:
    void SetUp() { g_coreType = C_API_AIC_TYPE; }
    void TearDown() { g_coreType = C_API_AIV_TYPE; }
};

namespace {
void set_set_gm2l1_nz_para_Stub(uint64_t config) { EXPECT_EQ(config, static_cast<uint64_t>(1)); }
} // namespace

TEST_F(TestSysVarSetGm2l1NzPara, c_api_set_gm2l1_nz_para_Succ)
{
    int64_t input = 1;
    MOCKER_CPP(set_mte2_nz_para, void(uint64_t)).times(1).will(invoke(set_set_gm2l1_nz_para_Stub));
    asc_set_gm2l1_nz_para(input);
    asc_init();
    GlobalMockObject::verify();
}

namespace {
void set_set_gm2l1_nz_para_multi_Stub(uint64_t config)
{
    uint16_t matrix_num = static_cast<uint16_t>(config & 0xFFFF);
    uint16_t dst_nz_n_stride = static_cast<uint16_t>((config >> 16) & 0xFFFF);
    uint16_t dst_nz_c0_stride = static_cast<uint16_t>((config >> 32) & 0xFFFF);
    uint16_t dst_nz_matrix_stride = static_cast<uint16_t>((config >> 48) & 0xFFFF);
    EXPECT_EQ(matrix_num, static_cast<uint16_t>(1));
    EXPECT_EQ(dst_nz_n_stride, static_cast<uint16_t>(2));
    EXPECT_EQ(dst_nz_c0_stride, static_cast<uint16_t>(3));
    EXPECT_EQ(dst_nz_matrix_stride, static_cast<uint16_t>(4));
}
} // namespace

TEST_F(TestSysVarSetGm2l1NzPara, c_api_set_gm2l1_nz_para_multi_Succ)
{
    uint16_t matrix_num = 1;
    uint16_t dst_nz_n_stride = 2;
    uint16_t dst_nz_c0_stride = 3;
    uint16_t dst_nz_matrix_stride = 4;
    MOCKER_CPP(set_mte2_nz_para, void(uint64_t)).times(1).will(invoke(set_set_gm2l1_nz_para_multi_Stub));
    asc_set_gm2l1_nz_para(matrix_num, dst_nz_n_stride, dst_nz_c0_stride, dst_nz_matrix_stride);
    asc_init();
    GlobalMockObject::verify();
}
