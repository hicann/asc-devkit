/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software: you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License).
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "tests/api/c_api/stub/cce_stub.h"
#include "include/c_api/asc_simd.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

class TestAscStoreAtomicConfig9201CApi : public testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(TestAscStoreAtomicConfig9201CApi, set_store_atomic_config_v2_noop_Succ)
{
    // 9201 has no store atomic config SPR, so the deprecated setter must not emit set_st_atomic_cfg.
    MOCKER_CPP(set_st_atomic_cfg, void(uint64_t)).times(0);
    MOCKER_CPP(set_st_atomic_cfg, void(atomic_type_t, atomic_op_t)).times(0);

    asc_set_store_atomic_config_v2(1, 2);
    GlobalMockObject::verify();
}

TEST_F(TestAscStoreAtomicConfig9201CApi, get_store_atomic_config_noop_Succ)
{
    // 9201 has no store atomic config SPR, so the getter must not read it and keeps the default value.
    MOCKER_CPP(get_st_atomic_cfg, int64_t(void)).times(0).will(returnValue(int64_t(0)));

    asc_store_atomic_config config;
    asc_get_store_atomic_config(config);
    EXPECT_EQ(config.config, ASC_STORE_ATOMIC_DEFAULT_VALUE);
    GlobalMockObject::verify();
}

#pragma GCC diagnostic pop
