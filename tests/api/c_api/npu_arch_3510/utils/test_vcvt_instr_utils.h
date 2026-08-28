/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef TESTS_API_C_API_NPU_ARCH_3510_UTILS_TEST_VCVT_INSTR_UTILS_H
#define TESTS_API_C_API_NPU_ARCH_3510_UTILS_TEST_VCVT_INSTR_UTILS_H

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "tests/api/c_api/stub/cce_stub.h"
#include "include/c_api/asc_simd.h"

#define TEST_VECTOR_COMPUTE_VCVT_INSTR_0(class_name, c_api_name, cce_name, dst_data_type, src_data_type, index) \
                                                                                                                \
    class TestVectorCompute##class_name##_##dst_data_type##_##src_data_type##_CApi_0 : public testing::Test {   \
    protected:                                                                                                  \
        void SetUp() {}                                                                                         \
        void TearDown() {}                                                                                      \
    };                                                                                                          \
                                                                                                                \
    namespace {                                                                                                 \
    void cce_name##_##dst_data_type##_##src_data_type##_Stub_##index(                                           \
        dst_data_type& dst, src_data_type src0, vector_bool mask, int part, int mode)                           \
    {}                                                                                                          \
    }                                                                                                           \
                                                                                                                \
    TEST_F(                                                                                                     \
        TestVectorCompute##class_name##_##dst_data_type##_##src_data_type##_CApi_0,                             \
        c_api_name##_##dst_data_type##_##src_data_type##_Succ)                                                  \
    {                                                                                                           \
        dst_data_type dst;                                                                                      \
        src_data_type src0;                                                                                     \
        vector_bool mask;                                                                                       \
                                                                                                                \
        MOCKER_CPP(cce_name, void(dst_data_type&, src_data_type, vector_bool, int, int))                        \
            .times(1)                                                                                           \
            .will(invoke(cce_name##_##dst_data_type##_##src_data_type##_Stub_##index));                         \
                                                                                                                \
        c_api_name(dst, src0, mask);                                                                            \
        GlobalMockObject::verify();                                                                             \
    }

#define TEST_VECTOR_COMPUTE_VCVT_INSTR_1(class_name, c_api_name, cce_name, dst_data_type, src_data_type, index) \
                                                                                                                \
    class TestVectorCompute##class_name##_##dst_data_type##_##src_data_type##_CApi_1 : public testing::Test {   \
    protected:                                                                                                  \
        void SetUp() {}                                                                                         \
        void TearDown() {}                                                                                      \
    };                                                                                                          \
                                                                                                                \
    namespace {                                                                                                 \
    void cce_name##_##dst_data_type##_##src_data_type##_Stub_##index(                                           \
        dst_data_type& dst, src_data_type src0, vector_bool mask, int rnd, int sat, int part, int mode)         \
    {}                                                                                                          \
    }                                                                                                           \
                                                                                                                \
    TEST_F(                                                                                                     \
        TestVectorCompute##class_name##_##dst_data_type##_##src_data_type##_CApi_1,                             \
        c_api_name##_##dst_data_type##_##src_data_type##_Succ)                                                  \
    {                                                                                                           \
        dst_data_type dst;                                                                                      \
        src_data_type src0;                                                                                     \
        vector_bool mask;                                                                                       \
                                                                                                                \
        MOCKER_CPP(cce_name, void(dst_data_type&, src_data_type, vector_bool, int, int, int, int))              \
            .times(1)                                                                                           \
            .will(invoke(cce_name##_##dst_data_type##_##src_data_type##_Stub_##index));                         \
                                                                                                                \
        c_api_name(dst, src0, mask);                                                                            \
        GlobalMockObject::verify();                                                                             \
    }

#define TEST_VECTOR_COMPUTE_VCVT_INSTR_2(class_name, c_api_name, cce_name, dst_data_type, src_data_type, index) \
                                                                                                                \
    class TestVectorCompute##class_name##_##dst_data_type##_##src_data_type##_CApi_2 : public testing::Test {   \
    protected:                                                                                                  \
        void SetUp() {}                                                                                         \
        void TearDown() {}                                                                                      \
    };                                                                                                          \
                                                                                                                \
    namespace {                                                                                                 \
    void cce_name##_##dst_data_type##_##src_data_type##_Stub_##index(                                           \
        dst_data_type& dst, src_data_type src0, vector_bool mask, int rnd, int mode)                            \
    {}                                                                                                          \
    }                                                                                                           \
                                                                                                                \
    TEST_F(                                                                                                     \
        TestVectorCompute##class_name##_##dst_data_type##_##src_data_type##_CApi_2,                             \
        c_api_name##_##dst_data_type##_##src_data_type##_Succ)                                                  \
    {                                                                                                           \
        dst_data_type dst;                                                                                      \
        src_data_type src0;                                                                                     \
        vector_bool mask;                                                                                       \
                                                                                                                \
        MOCKER_CPP(cce_name, void(dst_data_type&, src_data_type, vector_bool, int, int))                        \
            .times(1)                                                                                           \
            .will(invoke(cce_name##_##dst_data_type##_##src_data_type##_Stub_##index));                         \
                                                                                                                \
        c_api_name(dst, src0, mask);                                                                            \
        GlobalMockObject::verify();                                                                             \
    }

#define TEST_VECTOR_COMPUTE_VCVT_INSTR_3(class_name, c_api_name, cce_name, dst_data_type, src_data_type, index) \
                                                                                                                \
    class TestVectorCompute##class_name##_##dst_data_type##_##src_data_type##_CApi_3 : public testing::Test {   \
    protected:                                                                                                  \
        void SetUp() {}                                                                                         \
        void TearDown() {}                                                                                      \
    };                                                                                                          \
                                                                                                                \
    namespace {                                                                                                 \
    void cce_name##_##dst_data_type##_##src_data_type##_Stub_##index(                                           \
        dst_data_type& dst, src_data_type src0, vector_bool mask, int pp, int mode)                             \
    {}                                                                                                          \
    }                                                                                                           \
                                                                                                                \
    TEST_F(                                                                                                     \
        TestVectorCompute##class_name##_##dst_data_type##_##src_data_type##_CApi_3,                             \
        c_api_name##_##dst_data_type##_##src_data_type##_Succ)                                                  \
    {                                                                                                           \
        dst_data_type dst;                                                                                      \
        src_data_type src0;                                                                                     \
        vector_bool mask;                                                                                       \
                                                                                                                \
        MOCKER_CPP(cce_name, void(dst_data_type&, src_data_type, vector_bool, int, int))                        \
            .times(1)                                                                                           \
            .will(invoke(cce_name##_##dst_data_type##_##src_data_type##_Stub_##index));                         \
                                                                                                                \
        c_api_name(dst, src0, mask);                                                                            \
        GlobalMockObject::verify();                                                                             \
    }

#define TEST_VECTOR_COMPUTE_VCVT_INSTR_4(class_name, c_api_name, cce_name, dst_data_type, src_data_type, index) \
                                                                                                                \
    class TestVectorCompute##class_name##_##dst_data_type##_##src_data_type##_CApi_4 : public testing::Test {   \
    protected:                                                                                                  \
        void SetUp() {}                                                                                         \
        void TearDown() {}                                                                                      \
    };                                                                                                          \
                                                                                                                \
    namespace {                                                                                                 \
    void cce_name##_##dst_data_type##_##src_data_type##_Stub_##index(                                           \
        dst_data_type& dst, src_data_type src0, vector_bool mask, int sat, int pp, int mode)                    \
    {}                                                                                                          \
    }                                                                                                           \
                                                                                                                \
    TEST_F(                                                                                                     \
        TestVectorCompute##class_name##_##dst_data_type##_##src_data_type##_CApi_4,                             \
        c_api_name##_##dst_data_type##_##src_data_type##_Succ)                                                  \
    {                                                                                                           \
        dst_data_type dst;                                                                                      \
        src_data_type src0;                                                                                     \
        vector_bool mask;                                                                                       \
                                                                                                                \
        MOCKER_CPP(cce_name, void(dst_data_type&, src_data_type, vector_bool, int, int, int))                   \
            .times(1)                                                                                           \
            .will(invoke(cce_name##_##dst_data_type##_##src_data_type##_Stub_##index));                         \
                                                                                                                \
        c_api_name(dst, src0, mask);                                                                            \
        GlobalMockObject::verify();                                                                             \
    }

#define TEST_VECTOR_COMPUTE_VCVT_INSTR_5(class_name, c_api_name, cce_name, dst_data_type, src_data_type, index)    \
                                                                                                                   \
    class TestVectorCompute##class_name##_##dst_data_type##_##src_data_type##_CApi_5 : public testing::Test {      \
    protected:                                                                                                     \
        void SetUp() {}                                                                                            \
        void TearDown() {}                                                                                         \
    };                                                                                                             \
                                                                                                                   \
    namespace {                                                                                                    \
    void cce_name##_##dst_data_type##_##src_data_type##_Stub_##index(                                              \
        dst_data_type& dst, src_data_type src0, vector_bool mask, Literal rnd, Literal sat, Literal part,          \
        Literal mode)                                                                                              \
    {}                                                                                                             \
    }                                                                                                              \
                                                                                                                   \
    TEST_F(                                                                                                        \
        TestVectorCompute##class_name##_##dst_data_type##_##src_data_type##_CApi_5,                                \
        c_api_name##_##dst_data_type##_##src_data_type##_Succ)                                                     \
    {                                                                                                              \
        dst_data_type dst;                                                                                         \
        src_data_type src0;                                                                                        \
        vector_bool mask;                                                                                          \
                                                                                                                   \
        MOCKER_CPP(cce_name, void(dst_data_type&, src_data_type, vector_bool, Literal, Literal, Literal, Literal)) \
            .times(1)                                                                                              \
            .will(invoke(cce_name##_##dst_data_type##_##src_data_type##_Stub_##index));                            \
                                                                                                                   \
        c_api_name(dst, src0, mask);                                                                               \
        GlobalMockObject::verify();                                                                                \
    }

#define TEST_VECTOR_COMPUTE_VCVT_RETURN_INSTR_2(class_name, c_api_name, cce_name, dst_data_type, src_data_type, index) \
                                                                                                                       \
    class TestVectorComputeReturn##class_name##_##dst_data_type##_##src_data_type##_CApi_2 : public testing::Test {    \
    protected:                                                                                                         \
        void SetUp() {}                                                                                                \
        void TearDown() {}                                                                                             \
    };                                                                                                                 \
                                                                                                                       \
    namespace {                                                                                                        \
    void cce_name##_##dst_data_type##_##src_data_type##_Return_Stub_##index(                                           \
        dst_data_type& dst, src_data_type src0, vector_bool mask, int rnd, int mode)                                   \
    {}                                                                                                                 \
    }                                                                                                                  \
                                                                                                                       \
    TEST_F(                                                                                                            \
        TestVectorComputeReturn##class_name##_##dst_data_type##_##src_data_type##_CApi_2,                              \
        c_api_name##_return_##dst_data_type##_##src_data_type##_Succ)                                                  \
    {                                                                                                                  \
        src_data_type src0;                                                                                            \
        vector_bool mask;                                                                                              \
                                                                                                                       \
        MOCKER_CPP(cce_name, void(dst_data_type&, src_data_type, vector_bool, int, int))                               \
            .times(1)                                                                                                  \
            .will(invoke(cce_name##_##dst_data_type##_##src_data_type##_Return_Stub_##index));                         \
                                                                                                                       \
        dst_data_type dst = c_api_name(src0, mask);                                                                    \
        (void)dst;                                                                                                     \
        GlobalMockObject::verify();                                                                                    \
    }

#define TEST_VECTOR_COMPUTE_VCVT_RETURN_INSTR_4(class_name, c_api_name, cce_name, dst_data_type, src_data_type, index) \
                                                                                                                       \
    class TestVectorComputeReturn##class_name##_##dst_data_type##_##src_data_type##_CApi_4 : public testing::Test {    \
    protected:                                                                                                         \
        void SetUp() {}                                                                                                \
        void TearDown() {}                                                                                             \
    };                                                                                                                 \
                                                                                                                       \
    namespace {                                                                                                        \
    void cce_name##_##dst_data_type##_##src_data_type##_Return_Stub_##index(                                           \
        dst_data_type& dst, src_data_type src0, vector_bool mask, int sat, int pp, int mode)                           \
    {}                                                                                                                 \
    }                                                                                                                  \
                                                                                                                       \
    TEST_F(                                                                                                            \
        TestVectorComputeReturn##class_name##_##dst_data_type##_##src_data_type##_CApi_4,                              \
        c_api_name##_return_##dst_data_type##_##src_data_type##_Succ)                                                  \
    {                                                                                                                  \
        src_data_type src0;                                                                                            \
        vector_bool mask;                                                                                              \
                                                                                                                       \
        MOCKER_CPP(cce_name, void(dst_data_type&, src_data_type, vector_bool, int, int, int))                          \
            .times(1)                                                                                                  \
            .will(invoke(cce_name##_##dst_data_type##_##src_data_type##_Return_Stub_##index));                         \
                                                                                                                       \
        dst_data_type dst = c_api_name(src0, mask);                                                                    \
        (void)dst;                                                                                                     \
        GlobalMockObject::verify();                                                                                    \
    }

#define TEST_VECTOR_COMPUTE_VCVT_RETURN_POS_INSTR_0(                                                                \
    class_name, c_api_name, cce_name, dst_data_type, src_data_type, index)                                          \
                                                                                                                    \
    class TestVectorComputeReturn##class_name##_##dst_data_type##_##src_data_type##_CApi_0 : public testing::Test { \
    protected:                                                                                                      \
        void SetUp() {}                                                                                             \
        void TearDown() {}                                                                                          \
    };                                                                                                              \
                                                                                                                    \
    namespace {                                                                                                     \
    void cce_name##_##dst_data_type##_##src_data_type##_Return_Stub_##index(                                        \
        dst_data_type& dst, src_data_type src0, vector_bool mask, int part, int mode)                               \
    {}                                                                                                              \
    }                                                                                                               \
                                                                                                                    \
    TEST_F(                                                                                                         \
        TestVectorComputeReturn##class_name##_##dst_data_type##_##src_data_type##_CApi_0,                           \
        c_api_name##_return_##dst_data_type##_##src_data_type##_Succ)                                               \
    {                                                                                                               \
        src_data_type src0;                                                                                         \
        vector_bool mask;                                                                                           \
                                                                                                                    \
        MOCKER_CPP(cce_name, void(dst_data_type&, src_data_type, vector_bool, int, int))                            \
            .times(1)                                                                                               \
            .will(invoke(cce_name##_##dst_data_type##_##src_data_type##_Return_Stub_##index));                      \
                                                                                                                    \
        dst_data_type dst = c_api_name(src0, mask, ASC_POSITION_EVEN);                                              \
        (void)dst;                                                                                                  \
        GlobalMockObject::verify();                                                                                 \
    }

#define TEST_VECTOR_COMPUTE_VCVT_RETURN_POS_INSTR_1(                                                                \
    class_name, c_api_name, cce_name, dst_data_type, src_data_type, index)                                          \
                                                                                                                    \
    class TestVectorComputeReturn##class_name##_##dst_data_type##_##src_data_type##_CApi_1 : public testing::Test { \
    protected:                                                                                                      \
        void SetUp() {}                                                                                             \
        void TearDown() {}                                                                                          \
    };                                                                                                              \
                                                                                                                    \
    namespace {                                                                                                     \
    void cce_name##_##dst_data_type##_##src_data_type##_Return_Stub_##index(                                        \
        dst_data_type& dst, src_data_type src0, vector_bool mask, int rnd, int sat, int part, int mode)             \
    {}                                                                                                              \
    }                                                                                                               \
                                                                                                                    \
    TEST_F(                                                                                                         \
        TestVectorComputeReturn##class_name##_##dst_data_type##_##src_data_type##_CApi_1,                           \
        c_api_name##_return_##dst_data_type##_##src_data_type##_Succ)                                               \
    {                                                                                                               \
        src_data_type src0;                                                                                         \
        vector_bool mask;                                                                                           \
                                                                                                                    \
        MOCKER_CPP(cce_name, void(dst_data_type&, src_data_type, vector_bool, int, int, int, int))                  \
            .times(1)                                                                                               \
            .will(invoke(cce_name##_##dst_data_type##_##src_data_type##_Return_Stub_##index));                      \
                                                                                                                    \
        dst_data_type dst = c_api_name(src0, mask, ASC_POSITION_EVEN);                                              \
        (void)dst;                                                                                                  \
        GlobalMockObject::verify();                                                                                 \
    }

#define TEST_VECTOR_COMPUTE_VCVT_RETURN_POS_INSTR_4(                                                                \
    class_name, c_api_name, cce_name, dst_data_type, src_data_type, index)                                          \
                                                                                                                    \
    class TestVectorComputeReturn##class_name##_##dst_data_type##_##src_data_type##_CApi_4 : public testing::Test { \
    protected:                                                                                                      \
        void SetUp() {}                                                                                             \
        void TearDown() {}                                                                                          \
    };                                                                                                              \
                                                                                                                    \
    namespace {                                                                                                     \
    void cce_name##_##dst_data_type##_##src_data_type##_Return_Stub_##index(                                        \
        dst_data_type& dst, src_data_type src0, vector_bool mask, int sat, int pp, int mode)                        \
    {}                                                                                                              \
    }                                                                                                               \
                                                                                                                    \
    TEST_F(                                                                                                         \
        TestVectorComputeReturn##class_name##_##dst_data_type##_##src_data_type##_CApi_4,                           \
        c_api_name##_return_##dst_data_type##_##src_data_type##_Succ)                                               \
    {                                                                                                               \
        src_data_type src0;                                                                                         \
        vector_bool mask;                                                                                           \
                                                                                                                    \
        MOCKER_CPP(cce_name, void(dst_data_type&, src_data_type, vector_bool, int, int, int))                       \
            .times(1)                                                                                               \
            .will(invoke(cce_name##_##dst_data_type##_##src_data_type##_Return_Stub_##index));                      \
                                                                                                                    \
        dst_data_type dst = c_api_name(src0, mask, ASC_POSITION_EVEN);                                              \
        (void)dst;                                                                                                  \
        GlobalMockObject::verify();                                                                                 \
    }

#endif
