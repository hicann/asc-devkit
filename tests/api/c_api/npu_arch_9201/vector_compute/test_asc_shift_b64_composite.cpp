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
#include "c_api/reg_compute/compute/reg_logic.h"

#define TEST_SHIFT_B64_COMPOSITE(api, instr, data_type, shift_type)                                                \
    namespace {                                                                                                    \
    void api##_##data_type##_Stub(data_type& dst, data_type src, shift_type shift, vector_bool mask, Literal mode) \
    {                                                                                                              \
        EXPECT_EQ(mode, MODE_ZEROING);                                                                             \
        dst = src;                                                                                                 \
    }                                                                                                              \
    }                                                                                                              \
                                                                                                                   \
    TEST(TestShiftB64Composite, api##_##data_type##_Reference)                                                     \
    {                                                                                                              \
        data_type dst{};                                                                                           \
        data_type src{};                                                                                           \
        shift_type shift{};                                                                                        \
        vector_bool mask{};                                                                                        \
        MOCKER_CPP(instr, void(data_type&, data_type, shift_type, vector_bool, Literal))                           \
            .times(1)                                                                                              \
            .will(invoke(api##_##data_type##_Stub));                                                               \
        api(dst, src, shift, mask);                                                                                \
        GlobalMockObject::verify();                                                                                \
    }                                                                                                              \
                                                                                                                   \
    TEST(TestShiftB64Composite, api##_##data_type##_Return)                                                        \
    {                                                                                                              \
        data_type src{};                                                                                           \
        shift_type shift{};                                                                                        \
        vector_bool mask{};                                                                                        \
        MOCKER_CPP(instr, void(data_type&, data_type, shift_type, vector_bool, Literal))                           \
            .times(1)                                                                                              \
            .will(invoke(api##_##data_type##_Stub));                                                               \
        data_type result = api(src, shift, mask);                                                                  \
        (void)result;                                                                                              \
        GlobalMockObject::verify();                                                                                \
    }

TEST_SHIFT_B64_COMPOSITE(asc_shiftleft, vshl, vector_uint64_t, vector_uint32_t);
TEST_SHIFT_B64_COMPOSITE(asc_shiftleft, vshl, vector_int64_t, vector_uint32_t);
TEST_SHIFT_B64_COMPOSITE(asc_shiftright, vshr, vector_uint64_t, vector_uint32_t);
TEST_SHIFT_B64_COMPOSITE(asc_shiftright, vshr, vector_int64_t, vector_uint32_t);
TEST_SHIFT_B64_COMPOSITE(asc_shiftleft_scalar, vshls, vector_uint64_t, uint32_t);
TEST_SHIFT_B64_COMPOSITE(asc_shiftleft_scalar, vshls, vector_int64_t, uint32_t);
TEST_SHIFT_B64_COMPOSITE(asc_shiftright_scalar, vshrs, vector_uint64_t, uint32_t);
TEST_SHIFT_B64_COMPOSITE(asc_shiftright_scalar, vshrs, vector_int64_t, uint32_t);

#undef TEST_SHIFT_B64_COMPOSITE
