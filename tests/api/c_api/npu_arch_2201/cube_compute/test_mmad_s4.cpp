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

class TestMmadS4CAPI : public testing::Test {
protected:
    void SetUp() { g_coreType = C_API_AIC_TYPE; }
    void TearDown() { g_coreType = C_API_AIV_TYPE; }
};

namespace {
void mad_s4_Stub(
    __cc__ int32_t* c, __ca__ void* a, __cb__ void* b, uint16_t m, uint16_t k, uint16_t n, uint8_t unitFlag,
    bool kDirectionAlign, bool cmatrixSource, bool cmatrixInitVal)
{
    EXPECT_EQ(c, reinterpret_cast<__cc__ int32_t*>(1));
    EXPECT_EQ(a, reinterpret_cast<__ca__ void*>(2));
    EXPECT_EQ(b, reinterpret_cast<__cb__ void*>(3));
    EXPECT_EQ(m, static_cast<uint16_t>(4));
    EXPECT_EQ(k, static_cast<uint16_t>(5));
    EXPECT_EQ(n, static_cast<uint16_t>(6));
    EXPECT_EQ(unitFlag, static_cast<uint8_t>(2));
    EXPECT_TRUE(kDirectionAlign);
    EXPECT_FALSE(cmatrixSource);
    EXPECT_TRUE(cmatrixInitVal);
}
} // namespace

TEST_F(TestMmadS4CAPI, mmad_s4_enum_Succ)
{
    __cc__ int32_t* c = reinterpret_cast<__cc__ int32_t*>(1);
    __ca__ int4b_t* a = reinterpret_cast<__ca__ int4b_t*>(2);
    __cb__ int4b_t* b = reinterpret_cast<__cb__ int4b_t*>(3);
    MOCKER(
        mad_s4,
        void(__cc__ int32_t*, __ca__ void*, __cb__ void*, uint16_t, uint16_t, uint16_t, uint8_t, bool, bool, bool))
        .times(1)
        .will(invoke(mad_s4_Stub));

    asc_mmad_s4(c, a, b, 4, 5, 6, asc_unit_flag_mode::ENABLE_KEEP, true, false, true);
    GlobalMockObject::verify();
}
