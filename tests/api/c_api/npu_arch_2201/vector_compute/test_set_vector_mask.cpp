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
#include "c_api/defs/union.h"
#include "c_api/defs/enum.h"

class TestVectorComputeSetCmpMask : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

namespace {

void set_cmp_mask_1_Stub(__ubuf__ void* src) { EXPECT_EQ(src, reinterpret_cast<__ubuf__ uint32_t*>(33)); }

} // namespace

TEST_F(TestVectorComputeSetCmpMask, set_cmpmask_ubuf_uint32_t_addr_Succ)
{
    __ubuf__ uint32_t* src = reinterpret_cast<__ubuf__ uint32_t*>(33);
    MOCKER_CPP(set_cmpmask, void(__ubuf__ void* src)).times(1).will(invoke(set_cmp_mask_1_Stub));

    asc_set_cmp_mask(src);
    GlobalMockObject::verify();
}
