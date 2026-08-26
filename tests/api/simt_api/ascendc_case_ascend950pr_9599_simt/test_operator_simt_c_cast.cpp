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
#include <climits>
#include <type_traits>
#include "simt_compiler_stub.h"
#include "kernel_operator.h"
#include "simt_api/asc_bf16.h"
#include "simt_api/asc_fp8.h"
#include "impl/simt_api/internal_functions_impl.h"
using namespace std;
using namespace AscendC;
using namespace AscendC::Simt;

#define THREAD_DIM 128

// ================================ Test type cast(float) start ==================================
struct TypeCastApiFloatParams {
    int32_t mode;
};

class TypeCastApiFloatTestsuite : public testing::Test, public testing::WithParamInterface<TypeCastApiFloatParams> {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(TypeCastApiFloatTestsuite, TypeCastApiFloatTest)
{
    float x1 = static_cast<float>(rand()) / RAND_MAX;

    uint32_t result1 = __float2uint_rna(x1);
    uint32_t expect1 = static_cast<uint32_t>(round(x1));

    int32_t result2 = __float2int_rna(x1);
    int32_t expect2 = static_cast<int32_t>(round(x1));

    uint64_t result3 = __float2ull_rna(x1);
    uint64_t expect3 = static_cast<uint64_t>(round(x1));

    int64_t result4 = __float2ll_rna(x1);
    int64_t expect4 = static_cast<int64_t>(round(x1));

    half result5 = __float2half_rna(x1);
    half expect5 = static_cast<half>(round(x1));

    half result6 = __float2half_ro(x1);
    half expect6 = static_cast<half>(round(x1));

    bfloat16_t result7 = __float2bfloat16_rna(x1);
    bfloat16_t expect7 = static_cast<bfloat16_t>(round(x1));

    x1 = 1.0f;
    EXPECT_EQ(x1, 1.0f);
}

TEST_F(TypeCastApiFloatTestsuite, GetRoundTest)
{
    EXPECT_EQ(::ROUND::CAST_RINT, __internal_get_round<__RoundMode::CAST_RINT>());
    EXPECT_EQ(::ROUND::CAST_ROUND, __internal_get_round<__RoundMode::CAST_ROUND>());
    EXPECT_EQ(::ROUND::CAST_FLOOR, __internal_get_round<__RoundMode::CAST_FLOOR>());
    EXPECT_EQ(::ROUND::CAST_CEIL, __internal_get_round<__RoundMode::CAST_CEIL>());
    EXPECT_EQ(::ROUND::CAST_TRUNC, __internal_get_round<__RoundMode::CAST_TRUNC>());
    EXPECT_EQ(::ROUND::CAST_ODD, __internal_get_round<__RoundMode::CAST_ODD>());
    EXPECT_EQ(::ROUND::CAST_HYBRID, __internal_get_round<__RoundMode::CAST_HYBRID>());
}

// ================================ Test type cast(float) end ==================================

// ================================ Test type cast(uint32) start ==================================
struct TypeCastApiUintParams {
    int32_t mode;
};

class TypeCastApiUintTestsuite : public testing::Test, public testing::WithParamInterface<TypeCastApiUintParams> {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(TypeCastApiUintTestsuite, TypeCastApiUintTest)
{
    uint32_t x1 = static_cast<uint32_t>(rand()) / RAND_MAX;

    float result1 = __uint2float_rna(x1);
    float expect1 = static_cast<float>(round(x1));

    half result2 = __uint2half_rna(x1);
    half expect2 = static_cast<half>(round(x1));

    bfloat16_t result3 = __uint2bfloat16_rna(x1);
    bfloat16_t expect3 = static_cast<bfloat16_t>(round(x1));

    x1 = 1;
    EXPECT_EQ(x1, 1);
}
// ================================ Test type cast(uint32) end ==================================

// ================================ Test type cast(int32) start ==================================
struct TypeCastApiIntParams {
    int32_t mode;
};

class TypeCastApiIntTestsuite : public testing::Test, public testing::WithParamInterface<TypeCastApiIntParams> {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(TypeCastApiIntTestsuite, TypeCastApiIntTest)
{
    int32_t x1 = static_cast<int32_t>(rand()) / RAND_MAX;

    float result1 = __int2float_rna(x1);
    float expect1 = static_cast<float>(round(x1));

    half result2 = __int2half_rna(x1);
    half expect2 = static_cast<half>(round(x1));

    bfloat16_t result3 = __int2bfloat16_rna(x1);
    bfloat16_t expect3 = static_cast<bfloat16_t>(round(x1));

    x1 = 1;
    EXPECT_EQ(x1, 1);
}
// ================================ Test type cast(int32) end ==================================

// ================================ Test type cast(uint64) start ==================================
struct TypeCastApiUllParams {
    int32_t mode;
};

class TypeCastApiUllTestsuite : public testing::Test, public testing::WithParamInterface<TypeCastApiUllParams> {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(TypeCastApiUllTestsuite, TypeCastApiUllTest)
{
    uint64_t x1 = static_cast<uint64_t>(rand()) / RAND_MAX;

    float result1 = __ull2float_rna(x1);
    float expect1 = static_cast<float>(round(x1));

    half result2 = __ull2half_rna(x1);
    half expect2 = static_cast<half>(round(x1));

    bfloat16_t result3 = __ull2bfloat16_rna(x1);
    bfloat16_t expect3 = static_cast<bfloat16_t>(round(x1));

    x1 = 1;
    EXPECT_EQ(x1, 1);
}
// ================================ Test type cast(uint64) end ==================================

// ================================ Test type cast(int64) start ==================================
struct TypeCastApiLlParams {
    int32_t mode;
};

class TypeCastApiLlTestsuite : public testing::Test, public testing::WithParamInterface<TypeCastApiLlParams> {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(TypeCastApiLlTestsuite, TypeCastApiLlTest)
{
    int64_t x1 = static_cast<int64_t>(rand()) / RAND_MAX;

    float result1 = __ll2float_rna(x1);
    float expect1 = static_cast<float>(round(x1));

    half result2 = __ll2half_rna(x1);
    half expect2 = static_cast<half>(round(x1));

    bfloat16_t result3 = __ll2bfloat16_rna(x1);
    bfloat16_t expect3 = static_cast<bfloat16_t>(round(x1));

    x1 = 1;
    EXPECT_EQ(x1, 1);
}
// ================================ Test type cast(int64) end ==================================

// ================================ Test type cast(half) start ==================================
struct TypeCastApiHalfParams {
    int32_t mode;
};

class TypeCastApiHalfTestsuite : public testing::Test, public testing::WithParamInterface<TypeCastApiHalfParams> {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(TypeCastApiHalfTestsuite, TypeCastApiHalfTest)
{
    float x1 = static_cast<float>(rand()) / RAND_MAX;
    half hx1 = half(x1);

    uint32_t result1 = __half2uint_rna(hx1);
    uint32_t expect1 = static_cast<uint32_t>(round(float(hx1)));

    int32_t result2 = __half2int_rna(hx1);
    int32_t expect2 = static_cast<int32_t>(round(float(hx1)));

    uint64_t result3 = __half2ull_rna(hx1);
    uint64_t expect3 = static_cast<uint64_t>(round(float(hx1)));

    int64_t result4 = __half2ll_rna(hx1);
    int64_t expect4 = static_cast<int64_t>(round(float(hx1)));

    bfloat16_t result5 = __half2bfloat16_rna(hx1);
    bfloat16_t expect5 = static_cast<bfloat16_t>(round(float(hx1)));

    x1 = 1.0f;
    EXPECT_EQ(x1, 1.0f);
}

TEST_F(TypeCastApiHalfTestsuite, TypeCastApiHalfInt16Test)
{
    half pos = half(1.75f);
    half neg = half(-1.25f);
    uint16_t halfPositiveInfBits = 0x7C00;
    half halfPositiveInf = *reinterpret_cast<half*>(&halfPositiveInfBits);

    EXPECT_EQ(static_cast<signed char>(12), __half2char_rz(half(12.75f)));
    EXPECT_EQ(static_cast<unsigned char>(12), __half2uchar_rz(half(12.75f)));
    EXPECT_EQ(static_cast<signed char>(SCHAR_MAX), __half2char_rz(half(200.0f)));
    EXPECT_EQ(static_cast<signed char>(SCHAR_MIN), __half2char_rz(half(-200.0f)));
    EXPECT_EQ(static_cast<unsigned char>(UCHAR_MAX), __half2uchar_rz(half(300.0f)));
    EXPECT_EQ(static_cast<unsigned char>(0), __half2uchar_rz(half(-1.0f)));

    EXPECT_EQ(static_cast<short int>(2), __half2short_rn(pos));
    EXPECT_EQ(static_cast<short int>(1), __half2short_rz(pos));
    EXPECT_EQ(static_cast<short int>(1), __half2short_rd(pos));
    EXPECT_EQ(static_cast<short int>(2), __half2short_ru(pos));
    EXPECT_EQ(static_cast<short int>(-1), __half2short_rn(neg));
    EXPECT_EQ(static_cast<short int>(-1), __half2short_rz(neg));
    EXPECT_EQ(static_cast<short int>(-2), __half2short_rd(neg));
    EXPECT_EQ(static_cast<short int>(-1), __half2short_ru(neg));
    EXPECT_EQ(static_cast<short int>(SHRT_MAX), __half2short_rn(half(40000.0f)));
    EXPECT_EQ(static_cast<short int>(SHRT_MAX), __half2short_rz(half(40000.0f)));
    EXPECT_EQ(static_cast<short int>(SHRT_MAX), __half2short_rd(half(40000.0f)));
    EXPECT_EQ(static_cast<short int>(SHRT_MAX), __half2short_ru(half(40000.0f)));
    EXPECT_EQ(static_cast<short int>(SHRT_MIN), __half2short_rn(half(-40000.0f)));
    EXPECT_EQ(static_cast<short int>(SHRT_MIN), __half2short_rz(half(-40000.0f)));
    EXPECT_EQ(static_cast<short int>(SHRT_MIN), __half2short_rd(half(-40000.0f)));
    EXPECT_EQ(static_cast<short int>(SHRT_MIN), __half2short_ru(half(-40000.0f)));

    EXPECT_EQ(static_cast<unsigned short int>(2), __half2ushort_rn(pos));
    EXPECT_EQ(static_cast<unsigned short int>(1), __half2ushort_rz(pos));
    EXPECT_EQ(static_cast<unsigned short int>(1), __half2ushort_rd(pos));
    EXPECT_EQ(static_cast<unsigned short int>(2), __half2ushort_ru(pos));
    EXPECT_EQ(static_cast<unsigned short int>(USHRT_MAX), __half2ushort_rn(halfPositiveInf));
    EXPECT_EQ(static_cast<unsigned short int>(USHRT_MAX), __half2ushort_rz(halfPositiveInf));
    EXPECT_EQ(static_cast<unsigned short int>(USHRT_MAX), __half2ushort_rd(halfPositiveInf));
    EXPECT_EQ(static_cast<unsigned short int>(USHRT_MAX), __half2ushort_ru(halfPositiveInf));
    EXPECT_EQ(static_cast<unsigned short int>(0), __half2ushort_rn(half(-1.0f)));
    EXPECT_EQ(static_cast<unsigned short int>(0), __half2ushort_rz(half(-1.0f)));
    EXPECT_EQ(static_cast<unsigned short int>(0), __half2ushort_rd(half(-1.0f)));
    EXPECT_EQ(static_cast<unsigned short int>(0), __half2ushort_ru(half(-1.0f)));

    EXPECT_EQ(half(123.0f), __short2half_rn(123));
    EXPECT_EQ(half(123.0f), __short2half_rz(123));
    EXPECT_EQ(half(123.0f), __short2half_rd(123));
    EXPECT_EQ(half(123.0f), __short2half_ru(123));
    EXPECT_EQ(half(456.0f), __ushort2half_rn(456));
    EXPECT_EQ(half(456.0f), __ushort2half_rz(456));
    EXPECT_EQ(half(456.0f), __ushort2half_rd(456));
    EXPECT_EQ(half(456.0f), __ushort2half_ru(456));
}

TEST_F(TypeCastApiHalfTestsuite, TypeCastApiHalfPackAndReinterpretTest)
{
    half x = half(1.5f);
    half2 half2Result = __float2half2_rn(1.5f);
    EXPECT_EQ(x, half2Result.x);
    EXPECT_EQ(x, half2Result.y);

    half2Result = __half2half2(x);
    EXPECT_EQ(x, half2Result.x);
    EXPECT_EQ(x, half2Result.y);

    float2 float2Result = __half22float2(half2Result);
    EXPECT_EQ(1.5f, float2Result.x);
    EXPECT_EQ(1.5f, float2Result.y);

    short int bitValue = __half_as_short(x);
    unsigned short int unsignedBitValue = __half_as_ushort(x);
    EXPECT_EQ(x, __short_as_half(bitValue));
    EXPECT_EQ(static_cast<unsigned short int>(bitValue), unsignedBitValue);
}
// ================================ Test type cast(half) end ==================================

// ================================ Test type cast(bfloat16) start ==================================
struct TypeCastApiBfloat16Params {
    int32_t mode;
};

class TypeCastApiBfloat16Testsuite : public testing::Test,
                                     public testing::WithParamInterface<TypeCastApiBfloat16Params> {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(TypeCastApiBfloat16Testsuite, TypeCastApiBfloat16Test)
{
    float x1 = static_cast<float>(rand()) / RAND_MAX;
    bfloat16_t bfx1 = bfloat16_t(x1);

    half result1 = __bfloat162half_rna(bfx1);
    half expect1 = static_cast<half>(round(float(bfx1)));

    uint32_t result2 = __bfloat162uint_rna(bfx1);
    uint32_t expect2 = static_cast<uint32_t>(round(float(bfx1)));

    int32_t result3 = __bfloat162int_rna(bfx1);
    int32_t expect3 = static_cast<int32_t>(round(float(bfx1)));

    uint64_t result4 = __bfloat162ull_rna(bfx1);
    uint64_t expect4 = static_cast<uint64_t>(round(float(bfx1)));

    int64_t result5 = __bfloat162ll_rna(bfx1);
    int64_t expect5 = static_cast<int64_t>(round(float(bfx1)));

    bfx1 = 1.5f;
    half result6 = __bfloat162half_rn_sat(bfx1);
    EXPECT_EQ(result6, (half)1.5f);

    bfx1 = 2.5f;
    half result7 = __bfloat162half_rz_sat(bfx1);
    EXPECT_EQ(result7, (half)2.5f);

    bfx1 = 3.5f;
    half result8 = __bfloat162half_rd_sat(bfx1);
    EXPECT_EQ(result8, (half)3.5f);

    bfx1 = 4.5f;
    half result9 = __bfloat162half_ru_sat(bfx1);
    EXPECT_EQ(result9, (half)4.5f);

    bfx1 = 5.5f;
    half result10 = __bfloat162half_rna_sat(bfx1);
    EXPECT_EQ(result10, (half)5.5f);

    bfloat16_t result11 = __bfloat162bfloat16_rn(bfx1);

    bfloat16_t result12 = __bfloat162bfloat16_rz(bfx1);

    bfloat16_t result13 = __bfloat162bfloat16_rd(bfx1);

    bfloat16_t result14 = __bfloat162bfloat16_ru(bfx1);

    bfloat16_t result15 = __bfloat162bfloat16_rna(bfx1);

    x1 = 1.0f;
    EXPECT_EQ(x1, 1.0f);
}

TEST_F(TypeCastApiBfloat16Testsuite, TypeCastApiBfloat16Int16Test)
{
    bfloat16_t pos = bfloat16_t(1.75f);
    bfloat16_t neg = bfloat16_t(-1.25f);

    EXPECT_EQ(static_cast<signed char>(12), __bfloat162char_rz(bfloat16_t(12.75f)));
    EXPECT_EQ(static_cast<unsigned char>(12), __bfloat162uchar_rz(bfloat16_t(12.75f)));
    EXPECT_EQ(static_cast<signed char>(SCHAR_MAX), __bfloat162char_rz(bfloat16_t(200.0f)));
    EXPECT_EQ(static_cast<signed char>(SCHAR_MIN), __bfloat162char_rz(bfloat16_t(-200.0f)));
    EXPECT_EQ(static_cast<unsigned char>(UCHAR_MAX), __bfloat162uchar_rz(bfloat16_t(300.0f)));
    EXPECT_EQ(static_cast<unsigned char>(0), __bfloat162uchar_rz(bfloat16_t(-1.0f)));

    EXPECT_EQ(static_cast<short int>(2), __bfloat162short_rn(pos));
    EXPECT_EQ(static_cast<short int>(1), __bfloat162short_rz(pos));
    EXPECT_EQ(static_cast<short int>(1), __bfloat162short_rd(pos));
    EXPECT_EQ(static_cast<short int>(2), __bfloat162short_ru(pos));
    EXPECT_EQ(static_cast<short int>(-1), __bfloat162short_rn(neg));
    EXPECT_EQ(static_cast<short int>(-1), __bfloat162short_rz(neg));
    EXPECT_EQ(static_cast<short int>(-2), __bfloat162short_rd(neg));
    EXPECT_EQ(static_cast<short int>(-1), __bfloat162short_ru(neg));
    EXPECT_EQ(static_cast<short int>(SHRT_MAX), __bfloat162short_rn(bfloat16_t(40000.0f)));
    EXPECT_EQ(static_cast<short int>(SHRT_MAX), __bfloat162short_rz(bfloat16_t(40000.0f)));
    EXPECT_EQ(static_cast<short int>(SHRT_MAX), __bfloat162short_rd(bfloat16_t(40000.0f)));
    EXPECT_EQ(static_cast<short int>(SHRT_MAX), __bfloat162short_ru(bfloat16_t(40000.0f)));
    EXPECT_EQ(static_cast<short int>(SHRT_MIN), __bfloat162short_rn(bfloat16_t(-40000.0f)));
    EXPECT_EQ(static_cast<short int>(SHRT_MIN), __bfloat162short_rz(bfloat16_t(-40000.0f)));
    EXPECT_EQ(static_cast<short int>(SHRT_MIN), __bfloat162short_rd(bfloat16_t(-40000.0f)));
    EXPECT_EQ(static_cast<short int>(SHRT_MIN), __bfloat162short_ru(bfloat16_t(-40000.0f)));

    EXPECT_EQ(static_cast<unsigned short int>(2), __bfloat162ushort_rn(pos));
    EXPECT_EQ(static_cast<unsigned short int>(1), __bfloat162ushort_rz(pos));
    EXPECT_EQ(static_cast<unsigned short int>(1), __bfloat162ushort_rd(pos));
    EXPECT_EQ(static_cast<unsigned short int>(2), __bfloat162ushort_ru(pos));
    EXPECT_EQ(static_cast<unsigned short int>(USHRT_MAX), __bfloat162ushort_rn(bfloat16_t(70000.0f)));
    EXPECT_EQ(static_cast<unsigned short int>(USHRT_MAX), __bfloat162ushort_rz(bfloat16_t(70000.0f)));
    EXPECT_EQ(static_cast<unsigned short int>(USHRT_MAX), __bfloat162ushort_rd(bfloat16_t(70000.0f)));
    EXPECT_EQ(static_cast<unsigned short int>(USHRT_MAX), __bfloat162ushort_ru(bfloat16_t(70000.0f)));
    EXPECT_EQ(static_cast<unsigned short int>(0), __bfloat162ushort_rn(bfloat16_t(-1.0f)));
    EXPECT_EQ(static_cast<unsigned short int>(0), __bfloat162ushort_rz(bfloat16_t(-1.0f)));
    EXPECT_EQ(static_cast<unsigned short int>(0), __bfloat162ushort_rd(bfloat16_t(-1.0f)));
    EXPECT_EQ(static_cast<unsigned short int>(0), __bfloat162ushort_ru(bfloat16_t(-1.0f)));

    EXPECT_EQ(bfloat16_t(123.0f), __short2bfloat16_rn(123));
    EXPECT_EQ(bfloat16_t(123.0f), __short2bfloat16_rz(123));
    EXPECT_EQ(bfloat16_t(123.0f), __short2bfloat16_rd(123));
    EXPECT_EQ(bfloat16_t(123.0f), __short2bfloat16_ru(123));
    EXPECT_EQ(bfloat16_t(456.0f), __ushort2bfloat16_rn(456));
    EXPECT_EQ(bfloat16_t(456.0f), __ushort2bfloat16_rz(456));
    EXPECT_EQ(bfloat16_t(456.0f), __ushort2bfloat16_rd(456));
    EXPECT_EQ(bfloat16_t(456.0f), __ushort2bfloat16_ru(456));
}

TEST_F(TypeCastApiBfloat16Testsuite, TypeCastApiBfloat16ReinterpretTest)
{
    bfloat16_t x = bfloat16_t(1.5f);
    short int bitValue = __bfloat16_as_short(x);
    unsigned short int unsignedBitValue = __bfloat16_as_ushort(x);

    EXPECT_EQ(x, __short_as_bfloat16(bitValue));
    EXPECT_EQ(static_cast<unsigned short int>(bitValue), unsignedBitValue);
}
// ================================ Test type cast(bfloat16) end ==================================

// ================================ Test type cast(hif82) start ==================================
struct TypeCastApiHif82Params {
    int32_t mode;
};

class TypeCastApiHif82Testsuite : public testing::Test, public testing::WithParamInterface<TypeCastApiHif82Params> {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(TypeCastApiHif82Testsuite, TypeCastApiHif82Test)
{
    float x1 = static_cast<float>(rand()) / RAND_MAX;
    half x2 = half(x1);
    float2 f2x1 = make_float2(x1, x1);
    half2 h2x2 = {x2, x2};

    f2x1 = make_float2(1.0f, 2.0f);
    hifloat8x2_t result1 = __float22hif82_rna(f2x1);
    EXPECT_EQ(result1.x.ToFloat(), 1.0f);
    EXPECT_EQ(result1.y.ToFloat(), 2.0f);

    result1 = __float22hif82_rna_sat(f2x1);
    EXPECT_EQ(result1.x.ToFloat(), 1.0f);
    EXPECT_EQ(result1.y.ToFloat(), 2.0f);

    result1 = __float22hif82_rh(f2x1);
    EXPECT_EQ(result1.x.ToFloat(), 1.0f);
    EXPECT_EQ(result1.y.ToFloat(), 2.0f);

    result1 = __float22hif82_rh_sat(f2x1);
    EXPECT_EQ(result1.x.ToFloat(), 1.0f);
    EXPECT_EQ(result1.y.ToFloat(), 2.0f);

    h2x2 = make_half2(1.0f, 2.0f);
    hifloat8x2_t result2 = __half22hif82_rna(h2x2);
    EXPECT_EQ(result2.x.ToFloat(), 1.0f);
    EXPECT_EQ(result2.y.ToFloat(), 2.0f);

    result2 = __half22hif82_rna_sat(h2x2);
    EXPECT_EQ(result2.x.ToFloat(), 1.0f);
    EXPECT_EQ(result1.y.ToFloat(), 2.0f);

    result2 = __half22hif82_rh(h2x2);
    EXPECT_EQ(result2.x.ToFloat(), 1.0f);
    EXPECT_EQ(result2.y.ToFloat(), 2.0f);

    result2 = __half22hif82_rh_sat(h2x2);
    EXPECT_EQ(result2.x.ToFloat(), 1.0f);
    EXPECT_EQ(result2.y.ToFloat(), 2.0f);

    __asc_fp8x2_storage_t result3;
    f2x1 = make_float2(1.0f, 2.0f);
    result3 = __asc_cvt_float2_to_fp8x2(f2x1, __ASC_NOSAT, __ASC_E4M3);
    float fres3 = static_cast<float>(result3);
    EXPECT_EQ(fres3, 16440.0f);

    result3 = __asc_cvt_float2_to_fp8x2(f2x1, __ASC_SATFINITE, __ASC_E4M3);
    fres3 = static_cast<float>(result3);
    EXPECT_EQ(fres3, 16440.0f);

    result3 = __asc_cvt_float2_to_fp8x2(f2x1, __ASC_NOSAT, __ASC_E5M2);
    fres3 = static_cast<float>(result3);
    EXPECT_EQ(fres3, 16444.0f);

    result3 = __asc_cvt_float2_to_fp8x2(f2x1, __ASC_SATFINITE, __ASC_E5M2);
    fres3 = static_cast<float>(result3);
    EXPECT_EQ(fres3, 16444.0f);

    __asc_fp8_storage_t fp8Result = __asc_cvt_float_to_fp8(1.0f, __ASC_NOSAT, __ASC_E4M3);
    EXPECT_EQ(static_cast<unsigned int>(fp8Result), 56U);
    fp8Result = __asc_cvt_float_to_fp8(1.0f, __ASC_NOSAT, __ASC_E5M2);
    EXPECT_EQ(static_cast<unsigned int>(fp8Result), 60U);

    hifloat8x2_t hif8x = {1, 2};
    float2 result4;
    result4 = __hif822float2(hif8x);
    EXPECT_EQ(result4.x, 1.0f);
    EXPECT_EQ(result4.y, 2.0f);

    half2 result5;
    result5 = __hif822half2(hif8x);
    EXPECT_EQ(result5.x.ToFloat(), 1.0f);
    EXPECT_EQ(result5.y.ToFloat(), 2.0f);

    float2 result6;
    float8_e4m3x2_t e4m3x = {1, 2};
    result6 = __e4m3x22float2(e4m3x);
    EXPECT_EQ(result6.x, 1.0f);
    EXPECT_EQ(result6.y, 2.0f);

    float8_e5m2x2_t e5m2x = {1, 2};
    result6 = __e5m2x22float2(e5m2x);
    EXPECT_EQ(result6.x, 1.0f);
    EXPECT_EQ(result6.y, 2.0f);

    x1 = 1.0f;
    EXPECT_EQ(x1, 1.0f);
}
// ================================ Test type cast(hif82) end ==================================
