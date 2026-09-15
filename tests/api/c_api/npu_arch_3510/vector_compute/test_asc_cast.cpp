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

#define TEST_ASC_CAST_ROUND(tag, expected_round)                                             \
    namespace {                                                                              \
    void AscCastInt16ToHalf##tag##Stub(                                                      \
        vector_half& dst, vector_int16_t src, vector_bool mask, int32_t round, int32_t mode) \
    {                                                                                        \
        EXPECT_EQ(round, expected_round);                                                    \
        EXPECT_EQ(mode, MODE_ZEROING);                                                       \
    }                                                                                        \
    }                                                                                        \
                                                                                             \
    TEST(AscCastRound, Int16ToHalf##tag)                                                     \
    {                                                                                        \
        vector_half dst;                                                                     \
        vector_int16_t src;                                                                  \
        vector_bool mask;                                                                    \
                                                                                             \
        MOCKER_CPP(vcvt, void(vector_half&, vector_int16_t, vector_bool, int32_t, int32_t))  \
            .times(1)                                                                        \
            .will(invoke(AscCastInt16ToHalf##tag##Stub));                                    \
                                                                                             \
        asc_cast(dst, src, mask, ASC_CAST_##tag);                                            \
        GlobalMockObject::verify();                                                          \
    }

TEST_ASC_CAST_ROUND(RN, ROUND_R);
TEST_ASC_CAST_ROUND(RNA, ROUND_A);
TEST_ASC_CAST_ROUND(RD, ROUND_F);
TEST_ASC_CAST_ROUND(RU, ROUND_C);
TEST_ASC_CAST_ROUND(RZ, ROUND_Z);

#undef TEST_ASC_CAST_ROUND
