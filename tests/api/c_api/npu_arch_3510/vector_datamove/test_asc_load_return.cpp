/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <type_traits>
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "tests/api/c_api/stub/cce_stub.h"
#include "include/c_api/asc_simd.h"

namespace {
template <typename T>
constexpr Literal LoadAlignReturnExpectedDist()
{
    return Literal::NORM;
}

template <typename T>
constexpr Literal LoadAlignBrcElemReturnExpectedDist()
{
    if constexpr (sizeof(T) == 1) {
        return Literal::BRC_B8;
    } else if constexpr (sizeof(T) == 2) {
        return Literal::BRC_B16;
    } else {
        return Literal::BRC_B32;
    }
}

template <typename T>
constexpr Literal LoadAlignBrcDatablockReturnExpectedDist()
{
    return Literal::BLK;
}

template <typename T>
constexpr Literal LoadAlignBrcElem2DatablockReturnExpectedDist()
{
    if constexpr (sizeof(T) == 2) {
        return Literal::E2B_B16;
    } else {
        return Literal::E2B_B32;
    }
}

template <typename T>
constexpr Literal LoadAlignDownsampleReturnExpectedDist()
{
    if constexpr (sizeof(T) == 1) {
        return Literal::DS_B8;
    } else {
        return Literal::DS_B16;
    }
}

template <typename T>
constexpr Literal LoadAlignMaskDownsampleReturnExpectedDist()
{
    return Literal::DS;
}

template <typename T>
constexpr Literal LoadAlignUnpackReturnExpectedDist()
{
    if constexpr (sizeof(T) == 1) {
        return Literal::UNPK_B8;
    } else if constexpr (sizeof(T) == 2) {
        return Literal::UNPK_B16;
    } else {
        return Literal::UNPK_B32;
    }
}

template <typename T>
constexpr Literal LoadAlignUnpack4ReturnExpectedDist()
{
    return Literal::UNPK4_B8;
}

template <typename T>
constexpr Literal LoadAlignUpsampleReturnExpectedDist()
{
    if constexpr (sizeof(T) == 1) {
        return Literal::US_B8;
    } else {
        return Literal::US_B16;
    }
}

template <typename T>
constexpr Literal LoadAlignMaskUpsampleReturnExpectedDist()
{
    return Literal::US;
}
} // namespace

#define TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(                                                                       \
    class_name, c_api_name, cce_name, dst_type, src_type, cce_dst_type, cce_src_type)                                \
    class TestVectorDatamove##class_name##dst_type##src_type##CApi : public testing::Test {                          \
    protected:                                                                                                       \
        void SetUp() {}                                                                                              \
        void TearDown() {}                                                                                           \
    };                                                                                                               \
                                                                                                                     \
    namespace {                                                                                                      \
    void c_api_name##_##dst_type##_##src_type##_ReturnStub(                                                          \
        cce_dst_type& dst, __ubuf__ cce_src_type* src, int32_t offset, Literal load_dist)                            \
    {                                                                                                                \
        EXPECT_EQ(src, reinterpret_cast<__ubuf__ cce_src_type*>(11));                                                \
        EXPECT_EQ(offset, static_cast<int32_t>(0));                                                                  \
        EXPECT_EQ(load_dist, class_name##ExpectedDist<src_type>());                                                  \
    }                                                                                                                \
    }                                                                                                                \
                                                                                                                     \
    TEST_F(                                                                                                          \
        TestVectorDatamove##class_name##dst_type##src_type##CApi, c_api_name##_##dst_type##_##src_type##_ReturnSucc) \
    {                                                                                                                \
        __ubuf__ src_type* src = reinterpret_cast<__ubuf__ src_type*>(11);                                           \
                                                                                                                     \
        MOCKER_CPP(cce_name, void(cce_dst_type&, __ubuf__ cce_src_type*, int32_t, Literal))                          \
            .times(1)                                                                                                \
            .will(invoke(c_api_name##_##dst_type##_##src_type##_ReturnStub));                                        \
                                                                                                                     \
        static_assert(std::is_same_v<decltype(c_api_name(src)), dst_type>);                                          \
        dst_type dst = c_api_name(src);                                                                              \
        (void)dst;                                                                                                   \
        GlobalMockObject::verify();                                                                                  \
    }

TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignReturn, asc_loadalign, vlds, vector_int8_t, int8_t, vector_int8_t, int8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignReturn, asc_loadalign, vlds, vector_uint8_t, uint8_t, vector_uint8_t, uint8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignReturn, asc_loadalign, vlds, vector_fp4x2_e2m1_t, fp4x2_e2m1_t, vector_fp4x2_e2m1_t, fp4x2_e2m1_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignReturn, asc_loadalign, vlds, vector_fp4x2_e1m2_t, fp4x2_e1m2_t, vector_fp4x2_e1m2_t, fp4x2_e1m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignReturn, asc_loadalign, vlds, vector_int4x2_t, int4b_t, vector_fp4x2_e1m2_t, float4_e1m2x2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignReturn, asc_loadalign, vlds, vector_fp8_e8m0_t, fp8_e8m0_t, vector_fp8_e8m0_t, fp8_e8m0_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignReturn, asc_loadalign, vlds, vector_fp8_e5m2_t, fp8_e5m2_t, vector_fp8_e5m2_t, fp8_e5m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignReturn, asc_loadalign, vlds, vector_fp8_e4m3fn_t, fp8_e4m3fn_t, vector_fp8_e4m3fn_t, fp8_e4m3fn_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignReturn, asc_loadalign, vlds, vector_hifloat8_t, hifloat8_t, vector_uint8_t, uint8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignReturn, asc_loadalign, vlds, vector_int16_t, int16_t, vector_int16_t, int16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignReturn, asc_loadalign, vlds, vector_uint16_t, uint16_t, vector_uint16_t, uint16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(LoadAlignReturn, asc_loadalign, vlds, vector_half, half, vector_half, half);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignReturn, asc_loadalign, vlds, vector_bfloat16_t, bfloat16_t, vector_bfloat16_t, bfloat16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignReturn, asc_loadalign, vlds, vector_int32_t, int32_t, vector_int32_t, int32_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignReturn, asc_loadalign, vlds, vector_uint32_t, uint32_t, vector_uint32_t, uint32_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(LoadAlignReturn, asc_loadalign, vlds, vector_float, float, vector_float, float);

TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcElemReturn, asc_loadalign_brc_elem, vlds, vector_int8_t, int8_t, vector_int8_t, int8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcElemReturn, asc_loadalign_brc_elem, vlds, vector_uint8_t, uint8_t, vector_uint8_t, uint8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcElemReturn, asc_loadalign_brc_elem, vlds, vector_fp4x2_e2m1_t, fp4x2_e2m1_t, vector_fp4x2_e2m1_t,
    fp4x2_e2m1_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcElemReturn, asc_loadalign_brc_elem, vlds, vector_fp4x2_e1m2_t, fp4x2_e1m2_t, vector_fp4x2_e1m2_t,
    fp4x2_e1m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcElemReturn, asc_loadalign_brc_elem, vlds, vector_int4x2_t, int4b_t, vector_fp4x2_e1m2_t,
    float4_e1m2x2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcElemReturn, asc_loadalign_brc_elem, vlds, vector_fp8_e8m0_t, fp8_e8m0_t, vector_fp8_e8m0_t, fp8_e8m0_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcElemReturn, asc_loadalign_brc_elem, vlds, vector_fp8_e5m2_t, fp8_e5m2_t, vector_fp8_e5m2_t, fp8_e5m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcElemReturn, asc_loadalign_brc_elem, vlds, vector_fp8_e4m3fn_t, fp8_e4m3fn_t, vector_fp8_e4m3fn_t,
    fp8_e4m3fn_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcElemReturn, asc_loadalign_brc_elem, vlds, vector_hifloat8_t, hifloat8_t, vector_uint8_t, uint8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcElemReturn, asc_loadalign_brc_elem, vlds, vector_int16_t, int16_t, vector_int16_t, int16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcElemReturn, asc_loadalign_brc_elem, vlds, vector_uint16_t, uint16_t, vector_uint16_t, uint16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcElemReturn, asc_loadalign_brc_elem, vlds, vector_half, half, vector_half, half);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcElemReturn, asc_loadalign_brc_elem, vlds, vector_bfloat16_t, bfloat16_t, vector_bfloat16_t, bfloat16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcElemReturn, asc_loadalign_brc_elem, vlds, vector_int32_t, int32_t, vector_int32_t, int32_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcElemReturn, asc_loadalign_brc_elem, vlds, vector_uint32_t, uint32_t, vector_uint32_t, uint32_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcElemReturn, asc_loadalign_brc_elem, vlds, vector_float, float, vector_float, float);

TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcDatablockReturn, asc_loadalign_brc_datablock, vlds, vector_int8_t, int8_t, vector_int8_t, int8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcDatablockReturn, asc_loadalign_brc_datablock, vlds, vector_uint8_t, uint8_t, vector_uint8_t, uint8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcDatablockReturn, asc_loadalign_brc_datablock, vlds, vector_fp4x2_e2m1_t, fp4x2_e2m1_t,
    vector_fp4x2_e2m1_t, fp4x2_e2m1_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcDatablockReturn, asc_loadalign_brc_datablock, vlds, vector_fp4x2_e1m2_t, fp4x2_e1m2_t,
    vector_fp4x2_e1m2_t, fp4x2_e1m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcDatablockReturn, asc_loadalign_brc_datablock, vlds, vector_int4x2_t, int4b_t, vector_fp4x2_e1m2_t,
    float4_e1m2x2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcDatablockReturn, asc_loadalign_brc_datablock, vlds, vector_fp8_e8m0_t, fp8_e8m0_t, vector_fp8_e8m0_t,
    fp8_e8m0_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcDatablockReturn, asc_loadalign_brc_datablock, vlds, vector_fp8_e5m2_t, fp8_e5m2_t, vector_fp8_e5m2_t,
    fp8_e5m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcDatablockReturn, asc_loadalign_brc_datablock, vlds, vector_fp8_e4m3fn_t, fp8_e4m3fn_t,
    vector_fp8_e4m3fn_t, fp8_e4m3fn_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcDatablockReturn, asc_loadalign_brc_datablock, vlds, vector_hifloat8_t, hifloat8_t, vector_uint8_t,
    uint8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcDatablockReturn, asc_loadalign_brc_datablock, vlds, vector_int16_t, int16_t, vector_int16_t, int16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcDatablockReturn, asc_loadalign_brc_datablock, vlds, vector_uint16_t, uint16_t, vector_uint16_t,
    uint16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcDatablockReturn, asc_loadalign_brc_datablock, vlds, vector_half, half, vector_half, half);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcDatablockReturn, asc_loadalign_brc_datablock, vlds, vector_bfloat16_t, bfloat16_t, vector_bfloat16_t,
    bfloat16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcDatablockReturn, asc_loadalign_brc_datablock, vlds, vector_int32_t, int32_t, vector_int32_t, int32_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcDatablockReturn, asc_loadalign_brc_datablock, vlds, vector_uint32_t, uint32_t, vector_uint32_t,
    uint32_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcDatablockReturn, asc_loadalign_brc_datablock, vlds, vector_float, float, vector_float, float);

TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcElem2DatablockReturn, asc_loadalign_brc_elem2datablock, vlds, vector_int16_t, int16_t, vector_int16_t,
    int16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcElem2DatablockReturn, asc_loadalign_brc_elem2datablock, vlds, vector_uint16_t, uint16_t,
    vector_uint16_t, uint16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcElem2DatablockReturn, asc_loadalign_brc_elem2datablock, vlds, vector_half, half, vector_half, half);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcElem2DatablockReturn, asc_loadalign_brc_elem2datablock, vlds, vector_bfloat16_t, bfloat16_t,
    vector_bfloat16_t, bfloat16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcElem2DatablockReturn, asc_loadalign_brc_elem2datablock, vlds, vector_int32_t, int32_t, vector_int32_t,
    int32_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcElem2DatablockReturn, asc_loadalign_brc_elem2datablock, vlds, vector_uint32_t, uint32_t,
    vector_uint32_t, uint32_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignBrcElem2DatablockReturn, asc_loadalign_brc_elem2datablock, vlds, vector_float, float, vector_float, float);

TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignDownsampleReturn, asc_loadalign_downsample, vlds, vector_int8_t, int8_t, vector_int8_t, int8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignDownsampleReturn, asc_loadalign_downsample, vlds, vector_uint8_t, uint8_t, vector_uint8_t, uint8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignDownsampleReturn, asc_loadalign_downsample, vlds, vector_fp4x2_e2m1_t, fp4x2_e2m1_t, vector_fp4x2_e2m1_t,
    fp4x2_e2m1_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignDownsampleReturn, asc_loadalign_downsample, vlds, vector_fp4x2_e1m2_t, fp4x2_e1m2_t, vector_fp4x2_e1m2_t,
    fp4x2_e1m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignDownsampleReturn, asc_loadalign_downsample, vlds, vector_int4x2_t, int4b_t, vector_fp4x2_e1m2_t,
    float4_e1m2x2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignDownsampleReturn, asc_loadalign_downsample, vlds, vector_fp8_e8m0_t, fp8_e8m0_t, vector_fp8_e8m0_t,
    fp8_e8m0_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignDownsampleReturn, asc_loadalign_downsample, vlds, vector_fp8_e5m2_t, fp8_e5m2_t, vector_fp8_e5m2_t,
    fp8_e5m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignDownsampleReturn, asc_loadalign_downsample, vlds, vector_fp8_e4m3fn_t, fp8_e4m3fn_t, vector_fp8_e4m3fn_t,
    fp8_e4m3fn_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignDownsampleReturn, asc_loadalign_downsample, vlds, vector_hifloat8_t, hifloat8_t, vector_uint8_t, uint8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignDownsampleReturn, asc_loadalign_downsample, vlds, vector_int16_t, int16_t, vector_int16_t, int16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignDownsampleReturn, asc_loadalign_downsample, vlds, vector_uint16_t, uint16_t, vector_uint16_t, uint16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignDownsampleReturn, asc_loadalign_downsample, vlds, vector_half, half, vector_half, half);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignDownsampleReturn, asc_loadalign_downsample, vlds, vector_bfloat16_t, bfloat16_t, vector_bfloat16_t,
    bfloat16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignMaskDownsampleReturn, asc_loadalign_mask_downsample, plds, vector_bool, uint32_t, vector_bool, uint32_t);

TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUnpackReturn, asc_loadalign_unpack, vlds, vector_int8_t, int8_t, vector_int8_t, int8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUnpackReturn, asc_loadalign_unpack, vlds, vector_uint8_t, uint8_t, vector_uint8_t, uint8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUnpackReturn, asc_loadalign_unpack, vlds, vector_fp4x2_e2m1_t, fp4x2_e2m1_t, vector_fp4x2_e2m1_t,
    fp4x2_e2m1_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUnpackReturn, asc_loadalign_unpack, vlds, vector_fp4x2_e1m2_t, fp4x2_e1m2_t, vector_fp4x2_e1m2_t,
    fp4x2_e1m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUnpackReturn, asc_loadalign_unpack, vlds, vector_int4x2_t, int4b_t, vector_fp4x2_e1m2_t, float4_e1m2x2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUnpackReturn, asc_loadalign_unpack, vlds, vector_fp8_e8m0_t, fp8_e8m0_t, vector_fp8_e8m0_t, fp8_e8m0_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUnpackReturn, asc_loadalign_unpack, vlds, vector_fp8_e5m2_t, fp8_e5m2_t, vector_fp8_e5m2_t, fp8_e5m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUnpackReturn, asc_loadalign_unpack, vlds, vector_fp8_e4m3fn_t, fp8_e4m3fn_t, vector_fp8_e4m3fn_t,
    fp8_e4m3fn_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUnpackReturn, asc_loadalign_unpack, vlds, vector_hifloat8_t, hifloat8_t, vector_uint8_t, uint8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUnpackReturn, asc_loadalign_unpack, vlds, vector_int16_t, int16_t, vector_int16_t, int16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUnpackReturn, asc_loadalign_unpack, vlds, vector_uint16_t, uint16_t, vector_uint16_t, uint16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUnpackReturn, asc_loadalign_unpack, vlds, vector_half, half, vector_half, half);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUnpackReturn, asc_loadalign_unpack, vlds, vector_bfloat16_t, bfloat16_t, vector_bfloat16_t, bfloat16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUnpackReturn, asc_loadalign_unpack, vlds, vector_int32_t, int32_t, vector_int32_t, int32_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUnpackReturn, asc_loadalign_unpack, vlds, vector_uint32_t, uint32_t, vector_uint32_t, uint32_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUnpackReturn, asc_loadalign_unpack, vlds, vector_float, float, vector_float, float);

TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUnpack4Return, asc_loadalign_unpack4, vlds, vector_int8_t, int8_t, vector_int8_t, int8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUnpack4Return, asc_loadalign_unpack4, vlds, vector_uint8_t, uint8_t, vector_uint8_t, uint8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUnpack4Return, asc_loadalign_unpack4, vlds, vector_fp4x2_e2m1_t, fp4x2_e2m1_t, vector_fp4x2_e2m1_t,
    fp4x2_e2m1_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUnpack4Return, asc_loadalign_unpack4, vlds, vector_fp4x2_e1m2_t, fp4x2_e1m2_t, vector_fp4x2_e1m2_t,
    fp4x2_e1m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUnpack4Return, asc_loadalign_unpack4, vlds, vector_int4x2_t, int4b_t, vector_fp4x2_e1m2_t,
    float4_e1m2x2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUnpack4Return, asc_loadalign_unpack4, vlds, vector_fp8_e8m0_t, fp8_e8m0_t, vector_fp8_e8m0_t, fp8_e8m0_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUnpack4Return, asc_loadalign_unpack4, vlds, vector_fp8_e5m2_t, fp8_e5m2_t, vector_fp8_e5m2_t, fp8_e5m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUnpack4Return, asc_loadalign_unpack4, vlds, vector_fp8_e4m3fn_t, fp8_e4m3fn_t, vector_fp8_e4m3fn_t,
    fp8_e4m3fn_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUnpack4Return, asc_loadalign_unpack4, vlds, vector_hifloat8_t, hifloat8_t, vector_uint8_t, uint8_t);

TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUpsampleReturn, asc_loadalign_upsample, vlds, vector_int8_t, int8_t, vector_int8_t, int8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUpsampleReturn, asc_loadalign_upsample, vlds, vector_uint8_t, uint8_t, vector_uint8_t, uint8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUpsampleReturn, asc_loadalign_upsample, vlds, vector_fp4x2_e2m1_t, fp4x2_e2m1_t, vector_fp4x2_e2m1_t,
    fp4x2_e2m1_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUpsampleReturn, asc_loadalign_upsample, vlds, vector_fp4x2_e1m2_t, fp4x2_e1m2_t, vector_fp4x2_e1m2_t,
    fp4x2_e1m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUpsampleReturn, asc_loadalign_upsample, vlds, vector_int4x2_t, int4b_t, vector_fp4x2_e1m2_t,
    float4_e1m2x2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUpsampleReturn, asc_loadalign_upsample, vlds, vector_fp8_e8m0_t, fp8_e8m0_t, vector_fp8_e8m0_t,
    fp8_e8m0_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUpsampleReturn, asc_loadalign_upsample, vlds, vector_fp8_e5m2_t, fp8_e5m2_t, vector_fp8_e5m2_t,
    fp8_e5m2_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUpsampleReturn, asc_loadalign_upsample, vlds, vector_fp8_e4m3fn_t, fp8_e4m3fn_t, vector_fp8_e4m3fn_t,
    fp8_e4m3fn_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUpsampleReturn, asc_loadalign_upsample, vlds, vector_hifloat8_t, hifloat8_t, vector_uint8_t, uint8_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUpsampleReturn, asc_loadalign_upsample, vlds, vector_int16_t, int16_t, vector_int16_t, int16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUpsampleReturn, asc_loadalign_upsample, vlds, vector_uint16_t, uint16_t, vector_uint16_t, uint16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUpsampleReturn, asc_loadalign_upsample, vlds, vector_half, half, vector_half, half);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignUpsampleReturn, asc_loadalign_upsample, vlds, vector_bfloat16_t, bfloat16_t, vector_bfloat16_t,
    bfloat16_t);
TEST_VECTOR_DATAMOVE_LOADALIGN_RETURN(
    LoadAlignMaskUpsampleReturn, asc_loadalign_mask_upsample, plds, vector_bool, uint32_t, vector_bool, uint32_t);
