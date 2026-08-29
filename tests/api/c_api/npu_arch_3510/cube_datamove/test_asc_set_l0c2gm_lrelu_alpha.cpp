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

class TestCubeDmamoveSetL0c2GmLreluAlpha : public testing::Test {
protected:
    void SetUp() { g_coreType = C_API_AIC_TYPE; }
    void TearDown() { g_coreType = C_API_AIV_TYPE; }
};

namespace {
template <typename T>
void set_lrelu_alpha_stub(T config)
{
    EXPECT_EQ((T)25, config);
}
} // namespace

TEST_F(TestCubeDmamoveSetL0c2GmLreluAlpha, asc_set_l0c2gm_lrelu_alpha_float_succ)
{
    MOCKER(set_lrelu_alpha, void(float)).times(1).will(invoke(set_lrelu_alpha_stub<float>));

    float config = 25.0;
    asc_set_l0c2gm_lrelu_alpha(config);
    GlobalMockObject::verify();
}

TEST_F(TestCubeDmamoveSetL0c2GmLreluAlpha, asc_set_l0c2gm_lrelu_alpha_half_succ)
{
    MOCKER(set_lrelu_alpha, void(half)).times(1).will(invoke(set_lrelu_alpha_stub<half>));

    half config = 25.0;
    asc_set_l0c2gm_lrelu_alpha(config);
    GlobalMockObject::verify();
}

TEST_F(TestCubeDmamoveSetL0c2GmLreluAlpha, asc_set_l0c_copy_lrelu_alpha_float_succ)
{
    MOCKER(set_lrelu_alpha, void(float)).times(1).will(invoke(set_lrelu_alpha_stub<float>));

    float scalar_relu_pre_alpha = 25.0;
    asc_set_l0c_copy_lrelu_alpha(scalar_relu_pre_alpha);
    GlobalMockObject::verify();
}
