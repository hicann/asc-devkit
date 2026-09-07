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
#include "include/c_api/asc_simd.h"

namespace {
void SprstsStub(Literal spr_id, __ubuf__ uint32_t* dst, int32_t offset)
{
    EXPECT_EQ(spr_id, SPR_AR);
    EXPECT_EQ(dst, reinterpret_cast<__ubuf__ uint32_t*>(0x20));
    EXPECT_EQ(offset, 32);
}

void SprstsPostUpdateStub(Literal spr_id, __ubuf__ uint32_t*& dst, int32_t offset, Literal post)
{
    EXPECT_EQ(spr_id, SPR_AR);
    EXPECT_EQ(dst, reinterpret_cast<__ubuf__ uint32_t*>(0x40));
    EXPECT_EQ(offset, 32);
    EXPECT_EQ(post, POST_UPDATE);
    dst = reinterpret_cast<__ubuf__ uint32_t*>(0x60);
}
} // namespace

TEST(AscStorealignSqueezeStatus, StoreStatus)
{
    __ubuf__ uint32_t* dst = reinterpret_cast<__ubuf__ uint32_t*>(0x20);
    MOCKER_CPP(sprsts, void(Literal, __ubuf__ uint32_t*, int32_t)).times(1).will(invoke(SprstsStub));

    asc_storealign_squeeze_status(dst, 32);

    GlobalMockObject::verify();
}

TEST(AscStorealignSqueezeStatus, StoreStatusPostUpdate)
{
    __ubuf__ uint32_t* dst = reinterpret_cast<__ubuf__ uint32_t*>(0x40);
    MOCKER_CPP(sprsts, void(Literal, __ubuf__ uint32_t*&, int32_t, Literal))
        .times(1)
        .will(invoke(SprstsPostUpdateStub));

    asc_storealign_squeeze_status_postupdate(dst, 32);

    EXPECT_EQ(dst, reinterpret_cast<__ubuf__ uint32_t*>(0x60));
    GlobalMockObject::verify();
}
