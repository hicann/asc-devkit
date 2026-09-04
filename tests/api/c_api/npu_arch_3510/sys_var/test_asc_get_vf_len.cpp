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
#include "include/c_api/utils/sys_var.h"
#include "impl/c_api/instr_impl/npu_arch_3510/utils_impl/utils_impl.h"

class TestSysVarGetVfLen : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(TestSysVarGetVfLen, get_vf_len_equals_256)
{
    int64_t val = asc_get_vf_len();
    EXPECT_EQ(256, val);
    GlobalMockObject::verify();
}
