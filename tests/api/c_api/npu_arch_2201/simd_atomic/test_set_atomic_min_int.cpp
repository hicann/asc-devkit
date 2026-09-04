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

class TestSimdAtomicSetAtomicMinInt : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

namespace {
void set_atomic_min_Stub() {}
void set_atomic_s32_Stub() {}
} // namespace

TEST_F(TestSimdAtomicSetAtomicMinInt, set_atomic_min_int_Succ)
{
    MOCKER(set_atomic_min, void(void)).times(1).will(invoke(set_atomic_min_Stub));

    MOCKER(set_atomic_s32, void(void)).times(1).will(invoke(set_atomic_s32_Stub));

    asc_set_atomic_min_int();
    GlobalMockObject::verify();
}
