/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software: you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License).
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef TESTS_API_C_API_NPU_ARCH_9201_UTILS_TEST_HALF_TO_FP8_INSTR_UTILS_H
#define TESTS_API_C_API_NPU_ARCH_9201_UTILS_TEST_HALF_TO_FP8_INSTR_UTILS_H

#include <type_traits>
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "tests/api/c_api/stub/cce_stub.h"
#include "include/c_api/asc_simd.h"

#define TEST_HALF_TO_FP8_VCVT_POS(                                                                                 \
    class_name, c_api_name, cce_name, dst_data_type, src_data_type, pos, round, sat, part, mode)                   \
                                                                                                                   \
    namespace {                                                                                                    \
    void class_name##Stub(                                                                                         \
        dst_data_type&, src_data_type, vector_bool, Literal actual_round, Literal actual_sat, Literal actual_part, \
        Literal actual_mode)                                                                                       \
    {                                                                                                              \
        EXPECT_EQ(actual_round, round);                                                                            \
        EXPECT_EQ(actual_sat, sat);                                                                                \
        EXPECT_EQ(actual_part, part);                                                                              \
        EXPECT_EQ(actual_mode, mode);                                                                              \
    }                                                                                                              \
    }                                                                                                              \
                                                                                                                   \
    TEST(HalfToFp8, class_name)                                                                                    \
    {                                                                                                              \
        dst_data_type dst;                                                                                         \
        src_data_type src;                                                                                         \
        vector_bool mask;                                                                                          \
                                                                                                                   \
        MOCKER_CPP(cce_name, void(dst_data_type&, src_data_type, vector_bool, Literal, Literal, Literal, Literal)) \
            .times(1)                                                                                              \
            .will(invoke(class_name##Stub));                                                                       \
                                                                                                                   \
        c_api_name(dst, src, mask, pos);                                                                           \
        GlobalMockObject::verify();                                                                                \
    }

#define TEST_HALF_TO_FP8_VCVT_RETURN_POS(                                                                          \
    class_name, c_api_name, cce_name, dst_data_type, src_data_type, pos, round, sat, part, mode)                   \
                                                                                                                   \
    namespace {                                                                                                    \
    void class_name##ReturnStub(                                                                                   \
        dst_data_type&, src_data_type, vector_bool, Literal actual_round, Literal actual_sat, Literal actual_part, \
        Literal actual_mode)                                                                                       \
    {                                                                                                              \
        EXPECT_EQ(actual_round, round);                                                                            \
        EXPECT_EQ(actual_sat, sat);                                                                                \
        EXPECT_EQ(actual_part, part);                                                                              \
        EXPECT_EQ(actual_mode, mode);                                                                              \
    }                                                                                                              \
    }                                                                                                              \
                                                                                                                   \
    TEST(HalfToFp8Return, class_name)                                                                              \
    {                                                                                                              \
        src_data_type src;                                                                                         \
        vector_bool mask;                                                                                          \
                                                                                                                   \
        MOCKER_CPP(cce_name, void(dst_data_type&, src_data_type, vector_bool, Literal, Literal, Literal, Literal)) \
            .times(1)                                                                                              \
            .will(invoke(class_name##ReturnStub));                                                                 \
                                                                                                                   \
        auto dst = c_api_name(src, mask, pos);                                                                     \
        static_assert(std::is_same<decltype(dst), dst_data_type>::value);                                          \
        (void)dst;                                                                                                 \
        GlobalMockObject::verify();                                                                                \
    }

#endif
