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

class TestCubeComputeSetL0cCopyPrequant : public testing::Test {
protected:
    void SetUp() { g_coreType = C_API_AIC_TYPE; }
    void TearDown() { g_coreType = C_API_AIV_TYPE; }
};

namespace {
void set_quant_pre_Stub(uint64_t config)
{
    uint64_t conf = 123;
    EXPECT_EQ(conf, config);
}

void set_quant_pre_multi_Stub(uint64_t config)
{
    float scale = 1.5f;
    uint32_t scale_bits = (*reinterpret_cast<uint32_t*>(&scale)) & 0xFFFFE000u;
    uint64_t expected = (static_cast<uint64_t>(1) << 46) | (static_cast<uint64_t>(100 & 0x1FF) << 37) |
                        static_cast<uint64_t>(scale_bits);
    EXPECT_EQ(expected, config);
}
} // namespace

TEST_F(TestCubeComputeSetL0cCopyPrequant, set_l0c_copy_prequant_Succ)
{
    MOCKER(set_quant_pre, void(uint64_t)).times(1).will(invoke(set_quant_pre_Stub));
    uint64_t config = 123;

    asc_set_l0c_copy_prequant(config);
    GlobalMockObject::verify();
}

TEST_F(TestCubeComputeSetL0cCopyPrequant, set_l0c_copy_prequant_multi_Succ)
{
    MOCKER(set_quant_pre, void(uint64_t)).times(1).will(invoke(set_quant_pre_multi_Stub));
    float scale = 1.5f;
    uint16_t offset = 100;
    bool is_signed = true;

    asc_set_l0c_copy_prequant(scale, offset, is_signed);
    GlobalMockObject::verify();
}
