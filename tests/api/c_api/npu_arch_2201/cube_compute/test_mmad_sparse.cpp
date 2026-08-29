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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

class TestMmadSparseCAPI : public testing::Test {
protected:
    void SetUp() { g_coreType = C_API_AIC_TYPE; }
    void TearDown() { g_coreType = C_API_AIV_TYPE; }
};

namespace {
uint8_t expectedUnitFlag = 1;

void mad_sp_Stub(
    __cc__ int32_t* c, __ca__ int8_t* a, __cb__ int8_t* b, uint16_t m, uint16_t k, uint16_t n, uint8_t unit_flag,
    bool cmatrix_source, bool cmatrix_init_val)
{
    EXPECT_EQ(c, reinterpret_cast<__cc__ int32_t*>(1));
    EXPECT_EQ(a, reinterpret_cast<__ca__ int8_t*>(2));
    EXPECT_EQ(b, reinterpret_cast<__cb__ int8_t*>(3));
    EXPECT_EQ(m, static_cast<uint16_t>(4));
    EXPECT_EQ(k, static_cast<uint16_t>(5));
    EXPECT_EQ(n, static_cast<uint16_t>(6));
    EXPECT_EQ(unit_flag, expectedUnitFlag);
    EXPECT_EQ(cmatrix_source, false);
    EXPECT_EQ(cmatrix_init_val, true);
}
} // namespace

TEST_F(TestMmadSparseCAPI, mmad_sparse_Succ)
{
    __cc__ int32_t* c = reinterpret_cast<__cc__ int32_t*>(1);
    __ca__ int8_t* a = reinterpret_cast<__ca__ int8_t*>(2);
    __cb__ int8_t* b = reinterpret_cast<__cb__ int8_t*>(3);
    uint16_t m = 4;
    uint16_t k = 5;
    uint16_t n = 6;
    uint8_t unit_flag = 1;
    bool cmatrix_source = false;
    bool cmatrix_init_val = true;
    expectedUnitFlag = 1;
    MOCKER(
        mad_sp, void(
                    __cc__ int32_t * c, __ca__ int8_t * a, __cb__ int8_t * b, uint16_t m, uint16_t k, uint16_t n,
                    uint8_t unit_flag, bool cmatrix_source, bool cmatrix_init_val))
        .times(1)
        .will(invoke(mad_sp_Stub));

    asc_mmad_sparse(c, a, b, m, k, n, unit_flag, cmatrix_source, cmatrix_init_val);
    GlobalMockObject::verify();
}

TEST_F(TestMmadSparseCAPI, mmad_sparse_enum_Succ)
{
    __cc__ int32_t* c = reinterpret_cast<__cc__ int32_t*>(1);
    __ca__ int8_t* a = reinterpret_cast<__ca__ int8_t*>(2);
    __cb__ int8_t* b = reinterpret_cast<__cb__ int8_t*>(3);
    uint16_t m = 4;
    uint16_t k = 5;
    uint16_t n = 6;
    bool cmatrix_source = false;
    bool cmatrix_init_val = true;
    expectedUnitFlag = 2;
    MOCKER(
        mad_sp, void(
                    __cc__ int32_t * c, __ca__ int8_t * a, __cb__ int8_t * b, uint16_t m, uint16_t k, uint16_t n,
                    uint8_t unit_flag, bool cmatrix_source, bool cmatrix_init_val))
        .times(1)
        .will(invoke(mad_sp_Stub));

    asc_mmad_sparse(c, a, b, m, k, n, asc_unit_flag_mode::ENABLE_KEEP, cmatrix_source, cmatrix_init_val);
    GlobalMockObject::verify();
}

TEST_F(TestMmadSparseCAPI, mmad_sparse_sync_Succ)
{
    __cc__ int32_t* c = reinterpret_cast<__cc__ int32_t*>(1);
    __ca__ int8_t* a = reinterpret_cast<__ca__ int8_t*>(2);
    __cb__ int8_t* b = reinterpret_cast<__cb__ int8_t*>(3);
    uint16_t m = 4;
    uint16_t k = 5;
    uint16_t n = 6;
    uint8_t unit_flag = 1;
    bool cmatrix_source = false;
    bool cmatrix_init_val = true;
    expectedUnitFlag = 1;
    MOCKER(
        mad_sp, void(
                    __cc__ int32_t * c, __ca__ int8_t * a, __cb__ int8_t * b, uint16_t m, uint16_t k, uint16_t n,
                    uint8_t unit_flag, bool cmatrix_source, bool cmatrix_init_val))
        .times(1)
        .will(invoke(mad_sp_Stub));

    asc_mmad_sparse_sync(c, a, b, m, k, n, unit_flag, cmatrix_source, cmatrix_init_val);
    GlobalMockObject::verify();
}

#pragma GCC diagnostic pop
