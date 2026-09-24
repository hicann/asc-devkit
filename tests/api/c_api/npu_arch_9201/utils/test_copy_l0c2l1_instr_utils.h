/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef TESTS_API_C_API_NPU_ARCH_9201_UTILS_TEST_COPY_L0C2L1_INSTR_UTILS_H
#define TESTS_API_C_API_NPU_ARCH_9201_UTILS_TEST_COPY_L0C2L1_INSTR_UTILS_H

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "tests/api/c_api/stub/cce_stub.h"
#include "c_api/defs/enum.h"

#ifndef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#endif

// Base enum overload (aligned with 3510): every overload has enable_nz2dn.
// copy_matrix_cc_to_cbuf arity exceeds mockcpp hook limit; exercise empty CPU stubs without MOCKER.
#define TEST_CUBE_DATAMOVE_L0C2L1_BASIC(class_name, c_api_name, cce_name, dst_data_type, src_data_type, index)      \
                                                                                                                    \
    class TestCubeDatamove##class_name##_##dst_data_type##_##src_data_type##_Basic_##index : public testing::Test { \
    protected:                                                                                                      \
        void SetUp() { g_coreType = C_API_AIC_TYPE; }                                                               \
        void TearDown() { g_coreType = C_API_AIV_TYPE; }                                                            \
    };                                                                                                              \
                                                                                                                    \
    TEST_F(                                                                                                         \
        TestCubeDatamove##class_name##_##dst_data_type##_##src_data_type##_Basic_##index,                           \
        c_api_name##_##dst_data_type##_##src_data_type##_Basic_Succ)                                                \
    {                                                                                                               \
        __cbuf__ dst_data_type* dst = reinterpret_cast<__cbuf__ dst_data_type*>(1);                                 \
        __cc__ src_data_type* src = reinterpret_cast<__cc__ src_data_type*>(2);                                     \
        uint16_t n_size = 4;                                                                                        \
        uint16_t m_size = 5;                                                                                        \
        uint32_t dst_stride = 6;                                                                                    \
        uint16_t src_stride = 7;                                                                                    \
        asc_unit_flag_mode unit_flag_mode = asc_unit_flag_mode::ENABLE_KEEP;                                        \
        asc_quant_mode quant_pre_mode = QuantMode_t::NoQuant;                                                       \
        asc_relu_pre_mode relu_pre_mode = asc_relu_pre_mode::NONE;                                                  \
        bool enable_channel_split = true;                                                                           \
        bool enable_nz2nd = true;                                                                                   \
        bool enable_nz2dn = true;                                                                                   \
        bool enable_clip_relu_pre = true;                                                                           \
                                                                                                                    \
        c_api_name(                                                                                                 \
            dst, src, n_size, m_size, dst_stride, src_stride, unit_flag_mode, quant_pre_mode, relu_pre_mode,        \
            enable_channel_split, enable_nz2nd, enable_nz2dn, enable_clip_relu_pre);                                \
        GlobalMockObject::verify();                                                                                 \
    }

// 9201-unique overload with bool quant_pre_rnd
#define TEST_CUBE_DATAMOVE_L0C2L1_RND(class_name, c_api_name, cce_name, dst_data_type, src_data_type, index)      \
                                                                                                                  \
    class TestCubeDatamove##class_name##_##dst_data_type##_##src_data_type##_Rnd_##index : public testing::Test { \
    protected:                                                                                                    \
        void SetUp() { g_coreType = C_API_AIC_TYPE; }                                                             \
        void TearDown() { g_coreType = C_API_AIV_TYPE; }                                                          \
    };                                                                                                            \
                                                                                                                  \
    TEST_F(                                                                                                       \
        TestCubeDatamove##class_name##_##dst_data_type##_##src_data_type##_Rnd_##index,                           \
        c_api_name##_##dst_data_type##_##src_data_type##_Rnd_Succ)                                                \
    {                                                                                                             \
        __cbuf__ dst_data_type* dst = reinterpret_cast<__cbuf__ dst_data_type*>(1);                               \
        __cc__ src_data_type* src = reinterpret_cast<__cc__ src_data_type*>(2);                                   \
        uint16_t n_size = 4;                                                                                      \
        uint16_t m_size = 5;                                                                                      \
        uint32_t dst_stride = 6;                                                                                  \
        uint16_t src_stride = 7;                                                                                  \
        bool quant_pre_rnd = true;                                                                                \
        asc_unit_flag_mode unit_flag_mode = asc_unit_flag_mode::ENABLE_KEEP;                                      \
        asc_quant_mode quant_pre_mode = QuantMode_t::NoQuant;                                                     \
        asc_relu_pre_mode relu_pre_mode = asc_relu_pre_mode::NONE;                                                \
        bool enable_channel_split = true;                                                                         \
        bool enable_nz2nd = true;                                                                                 \
        bool enable_nz2dn = true;                                                                                 \
        bool enable_clip_relu_pre = true;                                                                         \
                                                                                                                  \
        c_api_name(                                                                                               \
            dst, src, n_size, m_size, dst_stride, src_stride, quant_pre_rnd, unit_flag_mode, quant_pre_mode,      \
            relu_pre_mode, enable_channel_split, enable_nz2nd, enable_nz2dn, enable_clip_relu_pre);               \
        GlobalMockObject::verify();                                                                               \
    }

// int4b_t s4 base overload
#define TEST_CUBE_DATAMOVE_L0C2L1_S4(class_name, c_api_name, cce_name, src_data_type, index)                 \
                                                                                                             \
    class TestCubeDatamove##class_name##_int4b_t_##src_data_type##_S4_##index : public testing::Test {       \
    protected:                                                                                               \
        void SetUp() { g_coreType = C_API_AIC_TYPE; }                                                        \
        void TearDown() { g_coreType = C_API_AIV_TYPE; }                                                     \
    };                                                                                                       \
                                                                                                             \
    TEST_F(                                                                                                  \
        TestCubeDatamove##class_name##_int4b_t_##src_data_type##_S4_##index,                                 \
        c_api_name##_int4b_t_##src_data_type##_S4_Succ)                                                      \
    {                                                                                                        \
        __cbuf__ int4b_t* dst = reinterpret_cast<__cbuf__ int4b_t*>(1);                                      \
        __cc__ src_data_type* src = reinterpret_cast<__cc__ src_data_type*>(2);                              \
        uint16_t n_size = 3;                                                                                 \
        uint16_t m_size = 4;                                                                                 \
        uint32_t dst_stride = 6;                                                                             \
        uint16_t src_stride = 7;                                                                             \
        asc_unit_flag_mode unit_flag_mode = asc_unit_flag_mode::ENABLE_KEEP;                                 \
        asc_quant_mode quant_pre_mode = QuantMode_t::NoQuant;                                                \
        asc_relu_pre_mode relu_pre_mode = asc_relu_pre_mode::NONE;                                           \
        bool enable_channel_split = true;                                                                    \
        bool enable_nz2nd = true;                                                                            \
        bool enable_nz2dn = false;                                                                           \
        bool enable_clip_relu_pre = true;                                                                    \
                                                                                                             \
        c_api_name(                                                                                          \
            dst, src, n_size, m_size, dst_stride, src_stride, unit_flag_mode, quant_pre_mode, relu_pre_mode, \
            enable_channel_split, enable_nz2nd, enable_nz2dn, enable_clip_relu_pre);                         \
        GlobalMockObject::verify();                                                                          \
    }

// int4b_t s4 overload with quant_pre_rnd
#define TEST_CUBE_DATAMOVE_L0C2L1_S4_RND(class_name, c_api_name, cce_name, src_data_type, index)             \
                                                                                                             \
    class TestCubeDatamove##class_name##_int4b_t_##src_data_type##_S4Rnd_##index : public testing::Test {    \
    protected:                                                                                               \
        void SetUp() { g_coreType = C_API_AIC_TYPE; }                                                        \
        void TearDown() { g_coreType = C_API_AIV_TYPE; }                                                     \
    };                                                                                                       \
                                                                                                             \
    TEST_F(                                                                                                  \
        TestCubeDatamove##class_name##_int4b_t_##src_data_type##_S4Rnd_##index,                              \
        c_api_name##_int4b_t_##src_data_type##_S4Rnd_Succ)                                                   \
    {                                                                                                        \
        __cbuf__ int4b_t* dst = reinterpret_cast<__cbuf__ int4b_t*>(1);                                      \
        __cc__ src_data_type* src = reinterpret_cast<__cc__ src_data_type*>(2);                              \
        uint16_t n_size = 3;                                                                                 \
        uint16_t m_size = 4;                                                                                 \
        uint32_t dst_stride = 6;                                                                             \
        uint16_t src_stride = 7;                                                                             \
        bool quant_pre_rnd = true;                                                                           \
        asc_unit_flag_mode unit_flag_mode = asc_unit_flag_mode::ENABLE_KEEP;                                 \
        asc_quant_mode quant_pre_mode = QuantMode_t::NoQuant;                                                \
        asc_relu_pre_mode relu_pre_mode = asc_relu_pre_mode::NONE;                                           \
        bool enable_channel_split = true;                                                                    \
        bool enable_nz2nd = true;                                                                            \
        bool enable_nz2dn = false;                                                                           \
        bool enable_clip_relu_pre = true;                                                                    \
                                                                                                             \
        c_api_name(                                                                                          \
            dst, src, n_size, m_size, dst_stride, src_stride, quant_pre_rnd, unit_flag_mode, quant_pre_mode, \
            relu_pre_mode, enable_channel_split, enable_nz2nd, enable_nz2dn, enable_clip_relu_pre);          \
        GlobalMockObject::verify();                                                                          \
    }

#endif
