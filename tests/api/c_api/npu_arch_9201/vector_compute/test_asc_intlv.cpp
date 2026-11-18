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
#include "c_api/asc_simd.h"

namespace {
void VintlvFp8E6M2Stub(vector_fp8_e6m2_t& dst0, vector_fp8_e6m2_t& dst1, vector_fp8_e6m2_t src0, vector_fp8_e6m2_t src1)
{}
} // namespace

TEST(TestAscIntlvFp8E6M2CApi, AscIntlvFp8E6M2Succ)
{
    vector_fp8_e6m2_t dst0;
    vector_fp8_e6m2_t dst1;
    vector_fp8_e6m2_t src0;
    vector_fp8_e6m2_t src1;

    MOCKER_CPP(vintlv, void(vector_fp8_e6m2_t&, vector_fp8_e6m2_t&, vector_fp8_e6m2_t, vector_fp8_e6m2_t))
        .times(1)
        .will(invoke(VintlvFp8E6M2Stub));

    asc_intlv(dst0, dst1, src0, src1);
    GlobalMockObject::verify();
}
