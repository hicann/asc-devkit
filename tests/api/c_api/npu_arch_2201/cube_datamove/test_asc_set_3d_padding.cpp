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
#include "c_api/asc_simd.h"

class TestCubeDatamoveSet3DPadding : public testing::Test {
protected:
    void SetUp() { g_coreType = C_API_AIC_TYPE; }
    void TearDown() { g_coreType = C_API_AIV_TYPE; }
};

namespace {
void set_padding_stub(uint64_t config) { EXPECT_EQ(0x0000000000000101, config); }
} // namespace

TEST_F(TestCubeDatamoveSet3DPadding, set_l12l0a_3d_padding_supported_types_succ)
{
    MOCKER(set_padding, void(uint64_t)).times(10);

    asc_set_l12l0a_3d_padding(static_cast<uint64_t>(1));
    asc_set_l12l0a_3d_padding(static_cast<int8_t>(1));
    asc_set_l12l0a_3d_padding(static_cast<uint8_t>(1));
    asc_set_l12l0a_3d_padding(static_cast<int16_t>(1));
    asc_set_l12l0a_3d_padding(static_cast<uint16_t>(1));
    asc_set_l12l0a_3d_padding(static_cast<half>(1));
    asc_set_l12l0a_3d_padding(static_cast<bfloat16_t>(1));
    asc_set_l12l0a_3d_padding(static_cast<int32_t>(1));
    asc_set_l12l0a_3d_padding(static_cast<uint32_t>(1));
    asc_set_l12l0a_3d_padding(1.0F);
    GlobalMockObject::verify();
}

TEST_F(TestCubeDatamoveSet3DPadding, set_gm2l1_padding_supported_types_succ)
{
    MOCKER(set_padding, void(uint64_t)).times(10);

    asc_set_gm2l1_padding(static_cast<uint64_t>(1));
    asc_set_gm2l1_padding(static_cast<int8_t>(1));
    asc_set_gm2l1_padding(static_cast<uint8_t>(1));
    asc_set_gm2l1_padding(static_cast<int16_t>(1));
    asc_set_gm2l1_padding(static_cast<uint16_t>(1));
    asc_set_gm2l1_padding(static_cast<half>(1));
    asc_set_gm2l1_padding(static_cast<bfloat16_t>(1));
    asc_set_gm2l1_padding(static_cast<int32_t>(1));
    asc_set_gm2l1_padding(static_cast<uint32_t>(1));
    asc_set_gm2l1_padding(1.0F);
    GlobalMockObject::verify();
}

TEST_F(TestCubeDatamoveSet3DPadding, set_l12l0a_3d_padding_int8_encoding_succ)
{
    MOCKER(set_padding, void(uint64_t)).times(1).will(invoke(set_padding_stub));

    asc_set_l12l0a_3d_padding(static_cast<int8_t>(1));
    GlobalMockObject::verify();
}

TEST_F(TestCubeDatamoveSet3DPadding, set_gm2l1_padding_int8_encoding_succ)
{
    MOCKER(set_padding, void(uint64_t)).times(1).will(invoke(set_padding_stub));

    asc_set_gm2l1_padding(static_cast<int8_t>(1));
    GlobalMockObject::verify();
}
