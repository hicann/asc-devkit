/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "tests/api/c_api/npu_arch_3510/utils/test_storealign_instr_utils.h"

namespace {
void CheckStoreIntlvFloatImmediate(__ubuf__ uint32_t* dst, int32_t offset, int dist, int32_t expected_offset)
{
    EXPECT_EQ(dst, reinterpret_cast<__ubuf__ uint32_t*>(0x1000));
    EXPECT_EQ(offset, expected_offset);
    EXPECT_EQ(dist, static_cast<int>(INTLV_B32));
}

void StoreIntlvFloatAlignedStub(
    vector_uint32_t src0, vector_uint32_t src1, __ubuf__ uint32_t* dst, int32_t offset, int dist, vector_bool mask)
{
    CheckStoreIntlvFloatImmediate(dst, offset, dist, 0);
}

void StoreIntlvFloatImmediateStub(
    vector_uint32_t src0, vector_uint32_t src1, __ubuf__ uint32_t* dst, int32_t offset, int dist, vector_bool mask)
{
    CheckStoreIntlvFloatImmediate(dst, offset, dist, 8);
}

void StoreIntlvFloatAddrStub(
    vector_uint32_t src0, vector_uint32_t src1, __ubuf__ uint32_t* dst, addr_reg offset, Literal dist, vector_bool mask)
{
    EXPECT_EQ(dst, reinterpret_cast<__ubuf__ uint32_t*>(0x1000));
    EXPECT_EQ(dist, INTLV_B32);
}
} // namespace

class TestStoreAlignIntlvFloat : public testing::Test {
protected:
    void TearDown() override { GlobalMockObject::verify(); }
};

TEST_F(TestStoreAlignIntlvFloat, Aligned)
{
    __ubuf__ float* dst = reinterpret_cast<__ubuf__ float*>(0x1000);
    vector_float src0{};
    vector_float src1{};
    MOCKER_CPP(vsts, void(vector_uint32_t, vector_uint32_t, __ubuf__ uint32_t*, int32_t, int, vector_bool))
        .times(1)
        .will(invoke(StoreIntlvFloatAlignedStub));
    asc_storealign_intlv(dst, src0, src1);
}

TEST_F(TestStoreAlignIntlvFloat, ImmediateOffset)
{
    __ubuf__ float* dst = reinterpret_cast<__ubuf__ float*>(0x1000);
    vector_float src0{};
    vector_float src1{};
    constexpr int32_t offset = 8;
    MOCKER_CPP(vsts, void(vector_uint32_t, vector_uint32_t, __ubuf__ uint32_t*, int32_t, int, vector_bool))
        .times(1)
        .will(invoke(StoreIntlvFloatImmediateStub));
    asc_storealign_intlv(dst, src0, src1, offset);
}

TEST_F(TestStoreAlignIntlvFloat, AddressRegisterOffset)
{
    __ubuf__ float* dst = reinterpret_cast<__ubuf__ float*>(0x1000);
    vector_float src0{};
    vector_float src1{};
    addr_reg offset{};
    MOCKER_CPP(vst, void(vector_uint32_t, vector_uint32_t, __ubuf__ uint32_t*, addr_reg, Literal, vector_bool))
        .times(1)
        .will(invoke(StoreIntlvFloatAddrStub));
    asc_storealign_intlv(dst, src0, src1, offset);
}
