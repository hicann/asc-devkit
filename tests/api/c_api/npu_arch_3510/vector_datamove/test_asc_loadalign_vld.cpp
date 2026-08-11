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
#include "tests/api/c_api/stub/cce_stub.h"
#include "include/c_api/asc_simd.h"

#define TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(class_name, c_api_name, cce_name, dst_type, src_type)    \
                                                                                                      \
    class TestVectorDatamove##class_name##dst_type##dst_type##src_type##CApi : public testing::Test { \
    protected:                                                                                        \
        void SetUp() {}                                                                               \
        void TearDown() {}                                                                            \
    };                                                                                                \
                                                                                                      \
    namespace {                                                                                       \
    void c_api_name##_##dst_type##_##dst_type##_##src_type##_Stub(                                    \
        dst_type& dst, __ubuf__ src_type* src, addr_reg offset, Literal load_dist)                    \
    {}                                                                                                \
    }                                                                                                 \
                                                                                                      \
    TEST_F(                                                                                           \
        TestVectorDatamove##class_name##dst_type##dst_type##src_type##CApi,                           \
        c_api_name##_##dst_type##_##src_type##_Succ)                                                  \
    {                                                                                                 \
        dst_type dst;                                                                                 \
        __ubuf__ src_type* src;                                                                       \
        addr_reg offset;                                                                              \
                                                                                                      \
        MOCKER_CPP(cce_name, void(dst_type&, __ubuf__ src_type*, addr_reg, Literal))                  \
            .times(1)                                                                                 \
            .will(invoke(c_api_name##_##dst_type##_##dst_type##_##src_type##_Stub));                  \
                                                                                                      \
        c_api_name(dst, src, offset);                                                                 \
        GlobalMockObject::verify();                                                                   \
    }

#define TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR_HIF8(class_name, c_api_name, cce_name, dst_type, src_type) \
                                                                                                        \
    class TestVectorDatamove##class_name##dst_type##dst_type##src_type##CApi : public testing::Test {   \
    protected:                                                                                          \
        void SetUp() {}                                                                                 \
        void TearDown() {}                                                                              \
    };                                                                                                  \
                                                                                                        \
    namespace {                                                                                         \
    void c_api_name##_##dst_type##_##dst_type##_##src_type##_Stub(                                      \
        vector_uint8_t& dst, __ubuf__ uint8_t* src, addr_reg offset, Literal load_dist)                 \
    {}                                                                                                  \
    }                                                                                                   \
                                                                                                        \
    TEST_F(                                                                                             \
        TestVectorDatamove##class_name##dst_type##dst_type##src_type##CApi,                             \
        c_api_name##_##dst_type##_##src_type##_Succ)                                                    \
    {                                                                                                   \
        dst_type dst;                                                                                   \
        __ubuf__ src_type* src;                                                                         \
        addr_reg offset;                                                                                \
                                                                                                        \
        MOCKER_CPP(cce_name, void(vector_uint8_t&, __ubuf__ uint8_t*, addr_reg, Literal))               \
            .times(1)                                                                                   \
            .will(invoke(c_api_name##_##dst_type##_##dst_type##_##src_type##_Stub));                    \
                                                                                                        \
        c_api_name(dst, src, offset);                                                                   \
        GlobalMockObject::verify();                                                                     \
    }

#define TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR_INT4(class_name, c_api_name, cce_name, dst_type, src_type) \
                                                                                                        \
    class TestVectorDatamove##class_name##dst_type##dst_type##src_type##CApi : public testing::Test {   \
    protected:                                                                                          \
        void SetUp() {}                                                                                 \
        void TearDown() {}                                                                              \
    };                                                                                                  \
                                                                                                        \
    namespace {                                                                                         \
    void c_api_name##_##dst_type##_##dst_type##_##src_type##_Stub(                                      \
        vector_fp4x2_e1m2_t& dst, __ubuf__ float4_e1m2x2_t* src, addr_reg offset, Literal load_dist)    \
    {}                                                                                                  \
    }                                                                                                   \
                                                                                                        \
    TEST_F(                                                                                             \
        TestVectorDatamove##class_name##dst_type##dst_type##src_type##CApi,                             \
        c_api_name##_##dst_type##_##src_type##_Succ)                                                    \
    {                                                                                                   \
        dst_type dst;                                                                                   \
        __ubuf__ src_type* src;                                                                         \
        addr_reg offset;                                                                                \
                                                                                                        \
        MOCKER_CPP(cce_name, void(vector_fp4x2_e1m2_t&, __ubuf__ float4_e1m2x2_t*, addr_reg, Literal))  \
            .times(1)                                                                                   \
            .will(invoke(c_api_name##_##dst_type##_##dst_type##_##src_type##_Stub));                    \
                                                                                                        \
        c_api_name(dst, src, offset);                                                                   \
        GlobalMockObject::verify();                                                                     \
    }

#define TEST_VECTOR_DATAMOVE_LOADALIGN_DEINTLV_INSTR(class_name, c_api_name, cce_name, dst_type, src_type) \
                                                                                                           \
    class TestVectorDatamove##class_name##dst_type##dst_type##src_type##CApi : public testing::Test {      \
    protected:                                                                                             \
        void SetUp() {}                                                                                    \
        void TearDown() {}                                                                                 \
    };                                                                                                     \
                                                                                                           \
    namespace {                                                                                            \
    void c_api_name##_##dst_type##_##dst_type##_##src_type##_Stub(                                         \
        dst_type& dst0, dst_type& dst1, __ubuf__ src_type* src, addr_reg offset, Literal load_dist)        \
    {}                                                                                                     \
    }                                                                                                      \
                                                                                                           \
    TEST_F(                                                                                                \
        TestVectorDatamove##class_name##dst_type##dst_type##src_type##CApi,                                \
        c_api_name##_##dst_type##_##src_type##_Succ)                                                       \
    {                                                                                                      \
        dst_type dst0;                                                                                     \
        dst_type dst1;                                                                                     \
        __ubuf__ src_type* src;                                                                            \
        addr_reg offset;                                                                                   \
                                                                                                           \
        MOCKER_CPP(cce_name, void(dst_type&, dst_type&, __ubuf__ src_type*, addr_reg, Literal))            \
            .times(1)                                                                                      \
            .will(invoke(c_api_name##_##dst_type##_##dst_type##_##src_type##_Stub));                       \
                                                                                                           \
        c_api_name(dst0, dst1, src, offset);                                                               \
        GlobalMockObject::verify();                                                                        \
    }

#define TEST_VECTOR_DATAMOVE_LOADALIGN_DEINTLV_INSTR_HIF8(class_name, c_api_name, cce_name, dst_type, src_type) \
                                                                                                                \
    class TestVectorDatamove##class_name##dst_type##dst_type##src_type##CApi : public testing::Test {           \
    protected:                                                                                                  \
        void SetUp() {}                                                                                         \
        void TearDown() {}                                                                                      \
    };                                                                                                          \
                                                                                                                \
    namespace {                                                                                                 \
    void c_api_name##_##dst_type##_##dst_type##_##src_type##_Stub(                                              \
        vector_uint8_t& dst0, vector_uint8_t& dst1, __ubuf__ uint8_t* src, addr_reg offset, Literal load_dist)  \
    {}                                                                                                          \
    }                                                                                                           \
                                                                                                                \
    TEST_F(                                                                                                     \
        TestVectorDatamove##class_name##dst_type##dst_type##src_type##CApi,                                     \
        c_api_name##_##dst_type##_##src_type##_Succ)                                                            \
    {                                                                                                           \
        dst_type dst0;                                                                                          \
        dst_type dst1;                                                                                          \
        __ubuf__ src_type* src;                                                                                 \
        addr_reg offset;                                                                                        \
                                                                                                                \
        MOCKER_CPP(cce_name, void(vector_uint8_t&, vector_uint8_t&, __ubuf__ uint8_t*, addr_reg, Literal))      \
            .times(1)                                                                                           \
            .will(invoke(c_api_name##_##dst_type##_##dst_type##_##src_type##_Stub));                            \
                                                                                                                \
        c_api_name(dst0, dst1, src, offset);                                                                    \
        GlobalMockObject::verify();                                                                             \
    }

#define TEST_VECTOR_DATAMOVE_LOADALIGN_DEINTLV_INSTR_INT4(class_name, c_api_name, cce_name, dst_type, src_type)       \
                                                                                                                      \
    class TestVectorDatamove##class_name##dst_type##dst_type##src_type##CApi : public testing::Test {                 \
    protected:                                                                                                        \
        void SetUp() {}                                                                                               \
        void TearDown() {}                                                                                            \
    };                                                                                                                \
                                                                                                                      \
    namespace {                                                                                                       \
    void c_api_name##_##dst_type##_##dst_type##_##src_type##_Stub(                                                    \
        vector_fp4x2_e1m2_t& dst0, vector_fp4x2_e1m2_t& dst1, __ubuf__ float4_e1m2x2_t* src, addr_reg offset,         \
        Literal load_dist)                                                                                            \
    {}                                                                                                                \
    }                                                                                                                 \
                                                                                                                      \
    TEST_F(                                                                                                           \
        TestVectorDatamove##class_name##dst_type##dst_type##src_type##CApi,                                           \
        c_api_name##_##dst_type##_##src_type##_Succ)                                                                  \
    {                                                                                                                 \
        dst_type dst0;                                                                                                \
        dst_type dst1;                                                                                                \
        __ubuf__ src_type* src;                                                                                       \
        addr_reg offset;                                                                                              \
                                                                                                                      \
        MOCKER_CPP(                                                                                                   \
            cce_name, void(vector_fp4x2_e1m2_t&, vector_fp4x2_e1m2_t&, __ubuf__ float4_e1m2x2_t*, addr_reg, Literal)) \
            .times(1)                                                                                                 \
            .will(invoke(c_api_name##_##dst_type##_##dst_type##_##src_type##_Stub));                                  \
                                                                                                                      \
        c_api_name(dst0, dst1, src, offset);                                                                          \
        GlobalMockObject::verify();                                                                                   \
    }

// vld norm (fp4x2_e2m1/fp4x2_e1m2/u8/s8/f8_e4m3/fp8_e5m2/fp8_e8m0/u16/s16/bf16/half/u32/s32/float/u64/s64)
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldNorm, asc_loadalign, vld, vector_fp4x2_e1m2_t, fp4x2_e1m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldNorm, asc_loadalign, vld, vector_fp4x2_e2m1_t, fp4x2_e2m1_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldNorm, asc_loadalign, vld, vector_uint8_t, uint8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldNorm, asc_loadalign, vld, vector_int8_t, int8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldNorm, asc_loadalign, vld, vector_fp8_e4m3fn_t, fp8_e4m3fn_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR_HIF8(LoadAlignVldNorm, asc_loadalign, vld, vector_hifloat8_t, hifloat8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldNorm, asc_loadalign, vld, vector_fp8_e5m2_t, fp8_e5m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldNorm, asc_loadalign, vld, vector_fp8_e8m0_t, fp8_e8m0_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldNorm, asc_loadalign, vld, vector_uint16_t, uint16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldNorm, asc_loadalign, vld, vector_int16_t, int16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldNorm, asc_loadalign, vld, vector_bfloat16_t, bfloat16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldNorm, asc_loadalign, vld, vector_uint32_t, uint32_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldNorm, asc_loadalign, vld, vector_int32_t, int32_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldNorm, asc_loadalign, vld, vector_float, float);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldNorm, asc_loadalign, vld, vector_int64_t, int64_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldNorm, asc_loadalign, vld, vector_uint64_t, uint64_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR_INT4(LoadAlignVldNorm, asc_loadalign, vld, vector_int4x2_t, int4b_t);

// vld brc_b8(fp4x2_e2m1/fp4x2_e1m2/u8/s8/f8_e4m3/fp8_e5m2/fp8_e8m0)
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcB8, asc_loadalign_brc, vld, vector_fp4x2_e1m2_t, fp4x2_e1m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcB8, asc_loadalign_brc, vld, vector_fp4x2_e2m1_t, fp4x2_e2m1_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcB8, asc_loadalign_brc, vld, vector_uint8_t, uint8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcB8, asc_loadalign_brc, vld, vector_int8_t, int8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcB8, asc_loadalign_brc, vld, vector_fp8_e4m3fn_t, fp8_e4m3fn_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR_HIF8(LoadAlignVldBrcB8, asc_loadalign_brc, vld, vector_hifloat8_t, hifloat8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcB8, asc_loadalign_brc, vld, vector_fp8_e5m2_t, fp8_e5m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcB8, asc_loadalign_brc, vld, vector_fp8_e8m0_t, fp8_e8m0_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR_INT4(LoadAlignVldBrcB8, asc_loadalign_brc, vld, vector_int4x2_t, int4b_t);

// vld brc_b16(u16/s16/bf16/half)
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcB16, asc_loadalign_brc, vld, vector_uint16_t, uint16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcB16, asc_loadalign_brc, vld, vector_int16_t, int16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcB16, asc_loadalign_brc, vld, vector_bfloat16_t, bfloat16_t);

// vld brc_b32(u32/s32/float)
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcB32, asc_loadalign_brc, vld, vector_uint32_t, uint32_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcB32, asc_loadalign_brc, vld, vector_int32_t, int32_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcB32, asc_loadalign_brc, vld, vector_float, float);

// vld us(b8/b16)
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(
    LoadAlignVldUpSample, asc_loadalign_upsample, vld, vector_fp4x2_e1m2_t, fp4x2_e1m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(
    LoadAlignVldUpSample, asc_loadalign_upsample, vld, vector_fp4x2_e2m1_t, fp4x2_e2m1_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldUpSample, asc_loadalign_upsample, vld, vector_uint8_t, uint8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldUpSample, asc_loadalign_upsample, vld, vector_int8_t, int8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(
    LoadAlignVldUpSample, asc_loadalign_upsample, vld, vector_fp8_e4m3fn_t, fp8_e4m3fn_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR_HIF8(
    LoadAlignVldUpSample, asc_loadalign_upsample, vld, vector_hifloat8_t, hifloat8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldUpSample, asc_loadalign_upsample, vld, vector_fp8_e5m2_t, fp8_e5m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldUpSample, asc_loadalign_upsample, vld, vector_fp8_e8m0_t, fp8_e8m0_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldUpSample, asc_loadalign_upsample, vld, vector_uint16_t, uint16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldUpSample, asc_loadalign_upsample, vld, vector_int16_t, int16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldUpSample, asc_loadalign_upsample, vld, vector_bfloat16_t, bfloat16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldUpSample, asc_loadalign_upsample, vld, vector_half, half);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR_INT4(LoadAlignVldUpSample, asc_loadalign_upsample, vld, vector_int4x2_t, int4b_t);

// vld ds(b8/b16)
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(
    LoadAlignVldDownSample, asc_loadalign_downsample, vld, vector_fp4x2_e1m2_t, fp4x2_e1m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(
    LoadAlignVldDownSample, asc_loadalign_downsample, vld, vector_fp4x2_e2m1_t, fp4x2_e2m1_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldDownSample, asc_loadalign_downsample, vld, vector_uint8_t, uint8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldDownSample, asc_loadalign_downsample, vld, vector_int8_t, int8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(
    LoadAlignVldDownSample, asc_loadalign_downsample, vld, vector_fp8_e4m3fn_t, fp8_e4m3fn_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR_HIF8(
    LoadAlignVldDownSample, asc_loadalign_downsample, vld, vector_hifloat8_t, hifloat8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(
    LoadAlignVldDownSample, asc_loadalign_downsample, vld, vector_fp8_e5m2_t, fp8_e5m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(
    LoadAlignVldDownSample, asc_loadalign_downsample, vld, vector_fp8_e8m0_t, fp8_e8m0_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldDownSample, asc_loadalign_downsample, vld, vector_uint16_t, uint16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldDownSample, asc_loadalign_downsample, vld, vector_int16_t, int16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(
    LoadAlignVldDownSample, asc_loadalign_downsample, vld, vector_bfloat16_t, bfloat16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldDownSample, asc_loadalign_downsample, vld, vector_half, half);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR_INT4(
    LoadAlignVldDownSample, asc_loadalign_downsample, vld, vector_int4x2_t, int4b_t);

// vld dintlv(b8/b16/b32)
TEST_VECTOR_DATAMOVE_LOADALIGN_DEINTLV_INSTR(
    LoadAlignVldDintlv, asc_loadalign_deintlv, vld, vector_fp4x2_e1m2_t, fp4x2_e1m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_DEINTLV_INSTR(
    LoadAlignVldDintlv, asc_loadalign_deintlv, vld, vector_fp4x2_e2m1_t, fp4x2_e2m1_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_DEINTLV_INSTR(LoadAlignVldDintlv, asc_loadalign_deintlv, vld, vector_uint8_t, uint8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_DEINTLV_INSTR(LoadAlignVldDintlv, asc_loadalign_deintlv, vld, vector_int8_t, int8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_DEINTLV_INSTR(
    LoadAlignVldDintlv, asc_loadalign_deintlv, vld, vector_fp8_e4m3fn_t, fp8_e4m3fn_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_DEINTLV_INSTR_HIF8(
    LoadAlignVldDintlv, asc_loadalign_deintlv, vld, vector_hifloat8_t, hifloat8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_DEINTLV_INSTR(
    LoadAlignVldDintlv, asc_loadalign_deintlv, vld, vector_fp8_e5m2_t, fp8_e5m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_DEINTLV_INSTR(
    LoadAlignVldDintlv, asc_loadalign_deintlv, vld, vector_fp8_e8m0_t, fp8_e8m0_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_DEINTLV_INSTR(LoadAlignVldDintlv, asc_loadalign_deintlv, vld, vector_uint16_t, uint16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_DEINTLV_INSTR(LoadAlignVldDintlv, asc_loadalign_deintlv, vld, vector_int16_t, int16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_DEINTLV_INSTR(LoadAlignVldDintlv, asc_loadalign_deintlv, vld, vector_half, half);
TEST_VECTOR_DATAMOVE_LOADALIGN_DEINTLV_INSTR(
    LoadAlignVldDintlv, asc_loadalign_deintlv, vld, vector_bfloat16_t, bfloat16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_DEINTLV_INSTR(LoadAlignVldDintlv, asc_loadalign_deintlv, vld, vector_uint32_t, uint32_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_DEINTLV_INSTR(LoadAlignVldDintlv, asc_loadalign_deintlv, vld, vector_int32_t, int32_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_DEINTLV_INSTR(LoadAlignVldDintlv, asc_loadalign_deintlv, vld, vector_float, float);
TEST_VECTOR_DATAMOVE_LOADALIGN_DEINTLV_INSTR_INT4(
    LoadAlignVldDintlv, asc_loadalign_deintlv, vld, vector_int4x2_t, int4b_t);

// vld unpack(b8/b16/b32)
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldUnpack, asc_loadalign_unpack, vld, vector_fp4x2_e1m2_t, fp4x2_e1m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldUnpack, asc_loadalign_unpack, vld, vector_fp4x2_e2m1_t, fp4x2_e2m1_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldUnpack, asc_loadalign_unpack, vld, vector_uint8_t, uint8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldUnpack, asc_loadalign_unpack, vld, vector_int8_t, int8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldUnpack, asc_loadalign_unpack, vld, vector_fp8_e4m3fn_t, fp8_e4m3fn_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR_HIF8(LoadAlignVldUnpack, asc_loadalign_unpack, vld, vector_hifloat8_t, hifloat8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldUnpack, asc_loadalign_unpack, vld, vector_fp8_e5m2_t, fp8_e5m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldUnpack, asc_loadalign_unpack, vld, vector_fp8_e8m0_t, fp8_e8m0_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldUnpack, asc_loadalign_unpack, vld, vector_uint16_t, uint16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldUnpack, asc_loadalign_unpack, vld, vector_half, half);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldUnpack, asc_loadalign_unpack, vld, vector_int16_t, int16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldUnpack, asc_loadalign_unpack, vld, vector_bfloat16_t, bfloat16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldUnpack, asc_loadalign_unpack, vld, vector_uint32_t, uint32_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldUnpack, asc_loadalign_unpack, vld, vector_int32_t, int32_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldUnpack, asc_loadalign_unpack, vld, vector_float, float);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR_INT4(LoadAlignVldUnpack, asc_loadalign_unpack, vld, vector_int4x2_t, int4b_t);

// vld unpack v2 (b8)
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(
    LoadAlignVldUnpackV2, asc_loadalign_unpack_v2, vld, vector_fp4x2_e1m2_t, fp4x2_e1m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(
    LoadAlignVldUnpackV2, asc_loadalign_unpack_v2, vld, vector_fp4x2_e2m1_t, fp4x2_e2m1_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldUnpackV2, asc_loadalign_unpack_v2, vld, vector_uint8_t, uint8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldUnpackV2, asc_loadalign_unpack_v2, vld, vector_int8_t, int8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(
    LoadAlignVldUnpackV2, asc_loadalign_unpack_v2, vld, vector_fp8_e4m3fn_t, fp8_e4m3fn_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR_HIF8(
    LoadAlignVldUnpackV2, asc_loadalign_unpack_v2, vld, vector_hifloat8_t, hifloat8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldUnpackV2, asc_loadalign_unpack_v2, vld, vector_fp8_e5m2_t, fp8_e5m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldUnpackV2, asc_loadalign_unpack_v2, vld, vector_fp8_e8m0_t, fp8_e8m0_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR_INT4(LoadAlignVldUnpackV2, asc_loadalign_unpack_v2, vld, vector_int4x2_t, int4b_t);

// vld brc_v2 (b8/b16/b32)
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcV2, asc_loadalign_brc_v2, vld, vector_fp4x2_e1m2_t, fp4x2_e1m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcV2, asc_loadalign_brc_v2, vld, vector_fp4x2_e2m1_t, fp4x2_e2m1_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcV2, asc_loadalign_brc_v2, vld, vector_uint8_t, uint8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcV2, asc_loadalign_brc_v2, vld, vector_int8_t, int8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcV2, asc_loadalign_brc_v2, vld, vector_fp8_e4m3fn_t, fp8_e4m3fn_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR_HIF8(LoadAlignVldBrcV2, asc_loadalign_brc_v2, vld, vector_hifloat8_t, hifloat8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcV2, asc_loadalign_brc_v2, vld, vector_fp8_e5m2_t, fp8_e5m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcV2, asc_loadalign_brc_v2, vld, vector_fp8_e8m0_t, fp8_e8m0_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcV2, asc_loadalign_brc_v2, vld, vector_uint16_t, uint16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcV2, asc_loadalign_brc_v2, vld, vector_int16_t, int16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcV2, asc_loadalign_brc_v2, vld, vector_bfloat16_t, bfloat16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcV2, asc_loadalign_brc_v2, vld, vector_half, half);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcV2, asc_loadalign_brc_v2, vld, vector_uint32_t, uint32_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcV2, asc_loadalign_brc_v2, vld, vector_int32_t, int32_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcV2, asc_loadalign_brc_v2, vld, vector_float, float);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR_INT4(LoadAlignVldBrcV2, asc_loadalign_brc_v2, vld, vector_int4x2_t, int4b_t);

// vld brc_v3 (b16/b32)
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcV3, asc_loadalign_brc_v3, vld, vector_uint16_t, uint16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcV3, asc_loadalign_brc_v3, vld, vector_int16_t, int16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcV3, asc_loadalign_brc_v3, vld, vector_bfloat16_t, bfloat16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcV3, asc_loadalign_brc_v3, vld, vector_uint32_t, uint32_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcV3, asc_loadalign_brc_v3, vld, vector_int32_t, int32_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_INSTR(LoadAlignVldBrcV3, asc_loadalign_brc_v3, vld, vector_float, float);
