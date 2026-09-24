/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef TESTS_API_C_API_NPU_ARCH_9201_UTILS_TEST_PREFETCH_L2CACHE_INSTR_UTILS_H
#define TESTS_API_C_API_NPU_ARCH_9201_UTILS_TEST_PREFETCH_L2CACHE_INSTR_UTILS_H

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "tests/api/c_api/stub/cce_stub.h"

#ifndef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#endif

// CPU stub path: exercise tikicpulib/cce empty stubs without MOCKER_CPP.

#define TEST_CUBE_DATAMOVE_PREFETCH_GM2L2CACHE_AIC(class_name, c_api_name, data_type, index)                          \
                                                                                                                      \
    class TestCubeDatamove##class_name##_##data_type##_Gm2L2_Aic_##index : public testing::Test {                     \
    protected:                                                                                                        \
        void SetUp() { g_coreType = C_API_AIC_TYPE; }                                                                 \
        void TearDown() { g_coreType = C_API_AIV_TYPE; }                                                              \
    };                                                                                                                \
                                                                                                                      \
    TEST_F(TestCubeDatamove##class_name##_##data_type##_Gm2L2_Aic_##index, c_api_name##_##data_type##_Gm2L2_Aic_Succ) \
    {                                                                                                                 \
        __gm__ data_type* src = reinterpret_cast<__gm__ data_type*>(0x100);                                           \
        uint32_t burst_num = 2;                                                                                       \
        uint32_t burst_len = 32;                                                                                      \
        uint64_t burst_src_stride = 64;                                                                               \
        c_api_name(src, burst_num, burst_len, burst_src_stride);                                                      \
        GlobalMockObject::verify();                                                                                   \
    }

#define TEST_CUBE_DATAMOVE_PREFETCH_GM2L2CACHE_AIV(class_name, c_api_name, data_type, index)                          \
                                                                                                                      \
    class TestCubeDatamove##class_name##_##data_type##_Gm2L2_Aiv_##index : public testing::Test {                     \
    protected:                                                                                                        \
        void SetUp() { g_coreType = C_API_AIV_TYPE; }                                                                 \
        void TearDown() { g_coreType = C_API_AIV_TYPE; }                                                              \
    };                                                                                                                \
                                                                                                                      \
    TEST_F(TestCubeDatamove##class_name##_##data_type##_Gm2L2_Aiv_##index, c_api_name##_##data_type##_Gm2L2_Aiv_Succ) \
    {                                                                                                                 \
        __gm__ data_type* src = reinterpret_cast<__gm__ data_type*>(0x200);                                           \
        uint32_t burst_num = 3;                                                                                       \
        uint32_t burst_len = 16;                                                                                      \
        uint64_t burst_src_stride = 48;                                                                               \
        c_api_name(src, burst_num, burst_len, burst_src_stride);                                                      \
        GlobalMockObject::verify();                                                                                   \
    }

#define TEST_CUBE_DATAMOVE_PREFETCH_DN2NZ(class_name, c_api_name, data_type, index)                           \
                                                                                                              \
    class TestCubeDatamove##class_name##_##data_type##_Dn2nz_##index : public testing::Test {                 \
    protected:                                                                                                \
        void SetUp() { g_coreType = C_API_AIC_TYPE; }                                                         \
        void TearDown() { g_coreType = C_API_AIV_TYPE; }                                                      \
    };                                                                                                        \
                                                                                                              \
    TEST_F(TestCubeDatamove##class_name##_##data_type##_Dn2nz_##index, c_api_name##_##data_type##_Dn2nz_Succ) \
    {                                                                                                         \
        __gm__ data_type* src = reinterpret_cast<__gm__ data_type*>(0x300);                                   \
        uint64_t loop1_src_stride = 33;                                                                       \
        uint16_t n_value = 55;                                                                                \
        uint32_t d_value = 66;                                                                                \
        uint64_t loop4_src_stride = 77;                                                                       \
        c_api_name(src, loop1_src_stride, n_value, d_value, loop4_src_stride);                                \
        GlobalMockObject::verify();                                                                           \
    }

#define TEST_CUBE_DATAMOVE_PREFETCH_ND2NZ(class_name, c_api_name, data_type, index)                           \
                                                                                                              \
    class TestCubeDatamove##class_name##_##data_type##_Nd2nz_##index : public testing::Test {                 \
    protected:                                                                                                \
        void SetUp() { g_coreType = C_API_AIC_TYPE; }                                                         \
        void TearDown() { g_coreType = C_API_AIV_TYPE; }                                                      \
    };                                                                                                        \
                                                                                                              \
    TEST_F(TestCubeDatamove##class_name##_##data_type##_Nd2nz_##index, c_api_name##_##data_type##_Nd2nz_Succ) \
    {                                                                                                         \
        __gm__ data_type* src = reinterpret_cast<__gm__ data_type*>(0x400);                                   \
        uint64_t loop1_src_stride = 11;                                                                       \
        uint16_t n_value = 22;                                                                                \
        uint32_t d_value = 33;                                                                                \
        uint64_t loop4_src_stride = 44;                                                                       \
        c_api_name(src, loop1_src_stride, n_value, d_value, loop4_src_stride);                                \
        GlobalMockObject::verify();                                                                           \
    }

#define TEST_CUBE_DATAMOVE_PREFETCH_STOP(class_name, c_api_name, index)        \
                                                                               \
    class TestCubeDatamove##class_name##_Stop_##index : public testing::Test { \
    protected:                                                                 \
        void SetUp() {}                                                        \
        void TearDown() {}                                                     \
    };                                                                         \
                                                                               \
    TEST_F(TestCubeDatamove##class_name##_Stop_##index, c_api_name##_Succ)     \
    {                                                                          \
        c_api_name();                                                          \
        GlobalMockObject::verify();                                            \
    }

#endif
