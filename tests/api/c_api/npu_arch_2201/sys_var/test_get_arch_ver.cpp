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
#include "c_api/stub/cce_stub.h"
#include "c_api/asc_simd.h"
#include "c_api/utils_intf.h"

class TestVectorComputeGetArchVer : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(TestVectorComputeGetArchVer, get_arch_ver_Succ)
{
    uint32_t core_version = 0;
    asc_get_arch_ver(core_version);

    EXPECT_EQ(core_version, 2201U);
}
