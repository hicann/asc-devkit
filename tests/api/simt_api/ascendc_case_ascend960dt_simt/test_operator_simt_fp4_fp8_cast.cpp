/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <cstdint>
#include <gtest/gtest.h>

namespace {
enum class ROUND { R, A, F, C, Z, O, H };

enum class RoundingSaturation {
    RS_DISABLE_VALUE = 0,
    RS_ENABLE_VALUE = 1,
};

struct Bf16Stub {
    uint16_t val = 0;
    Bf16Stub() = default;
    Bf16Stub(float) : val(0) {}
    float ToFloat() const { return 0.0f; }
};
using bfloat16_t = Bf16Stub;

struct Bfloat16x2Stub {
    bfloat16_t x;
    bfloat16_t y;
};
using bfloat16x2_t = Bfloat16x2Stub;

struct HalfStub {
    uint16_t val = 0;
    HalfStub() = default;
    HalfStub(float) : val(0) {}
    float ToFloat() const { return 0.0f; }
};
using half = HalfStub;

struct Float2Stub {
    float x = 0.0f;
    float y = 0.0f;
};
using float2 = Float2Stub;

struct Half2Stub {
    half x;
    half y;
};
using half2 = Half2Stub;

struct Fp4x2Stub {
    uint8_t val = 0;
};
using float4_e1m2x2_t = Fp4x2Stub;
using float4_e2m1x2_t = Fp4x2Stub;

struct Fp8E8m0Stub {
    uint8_t val = 0;
    Fp8E8m0Stub() = default;
    Fp8E8m0Stub(int) : val(0) {}
};
using fp8_e8m0_t = Fp8E8m0Stub;

struct Fp8E6m2Stub {
    uint8_t val = 0;
    Fp8E6m2Stub() = default;
    Fp8E6m2Stub(int) : val(0) {}
};
using fp8_e6m2_t = Fp8E6m2Stub;

struct Fp8E4m3Stub {
    uint8_t val = 0;
    Fp8E4m3Stub() = default;
    Fp8E4m3Stub(float) : val(0) {}
};
using fp8_e4m3fn_t = Fp8E4m3Stub;

struct Fp8E5m2Stub {
    uint8_t val = 0;
    Fp8E5m2Stub() = default;
    Fp8E5m2Stub(float) : val(0) {}
};
using fp8_e5m2_t = Fp8E5m2Stub;

struct Hif8Stub {
    uint8_t val = 0;
    Hif8Stub() = default;
    Hif8Stub(float) : val(0) {}
    float ToFloat() const { return 0.0f; }
};
using hifloat8_t = Hif8Stub;

struct Float8E8m0x2Stub {
    fp8_e8m0_t x;
    fp8_e8m0_t y;
};
using float8_e8m0x2_t = Float8E8m0x2Stub;

struct Float8E6m2x2Stub {
    fp8_e6m2_t x;
    fp8_e6m2_t y;
};
using float8_e6m2x2_t = Float8E6m2x2Stub;

struct Float8E4m3x2Stub {
    fp8_e4m3fn_t x;
    fp8_e4m3fn_t y;
};
using float8_e4m3x2_t = Float8E4m3x2Stub;

struct Float8E5m2x2Stub {
    fp8_e5m2_t x;
    fp8_e5m2_t y;
};
using float8_e5m2x2_t = Float8E5m2x2Stub;

struct Hifloat8x2Stub {
    hifloat8_t x;
    hifloat8_t y;
};
using hifloat8x2_t = Hifloat8x2Stub;

using __asc_fp8x2_storage_t = unsigned short int;
using __asc_fp8_storage_t = unsigned char;

enum __asc_fp8_interpretation_t { __ASC_E4M3, __ASC_E5M2 };
enum __asc_saturation_t { __ASC_NOSAT, __ASC_SATFINITE };
} // namespace

template <ROUND rnd, RoundingSaturation rst, typename SRC_TYPE>
bfloat16x2_t __cvt_bfloat16x2(SRC_TYPE x)
{
    (void)x;
    return bfloat16x2_t{};
}

template <ROUND rnd, RoundingSaturation rst, typename SRC_TYPE>
float4_e1m2x2_t __cvt_float4_e1m2x2(SRC_TYPE x)
{
    (void)x;
    return float4_e1m2x2_t{};
}

template <ROUND rnd, RoundingSaturation rst, typename SRC_TYPE>
float4_e2m1x2_t __cvt_float4_e2m1x2(SRC_TYPE x)
{
    (void)x;
    return float4_e2m1x2_t{};
}

template <ROUND rnd, RoundingSaturation rst, typename SRC_TYPE>
float8_e8m0x2_t __cvt_float8_e8m0x2(SRC_TYPE x)
{
    (void)x;
    return float8_e8m0x2_t{};
}

template <ROUND rnd, RoundingSaturation rst, typename SRC_TYPE>
float8_e6m2x2_t __cvt_float8_e6m2x2(SRC_TYPE x)
{
    (void)x;
    return float8_e6m2x2_t{};
}

template <ROUND rnd, RoundingSaturation rst, typename SRC_TYPE>
bfloat16x2_t __rcp_cvt_bfloat16x2(SRC_TYPE x)
{
    (void)x;
    return bfloat16x2_t{};
}

template <ROUND rnd, RoundingSaturation rst, typename SRC_TYPE>
hifloat8x2_t __cvt_hifloat8x2_t(SRC_TYPE x)
{
    (void)x;
    return hifloat8x2_t{};
}

template <ROUND rnd, RoundingSaturation rst, typename SRC_TYPE>
float2 __cvt_float2(SRC_TYPE x)
{
    (void)x;
    return float2{};
}

template <ROUND rnd, RoundingSaturation rst, typename SRC_TYPE>
half2 __cvt_half2(SRC_TYPE x)
{
    (void)x;
    return half2{};
}

template <ROUND rnd, RoundingSaturation rst, typename SRC_TYPE>
float8_e4m3x2_t __cvt_float8_e4m3x2_t(SRC_TYPE x)
{
    (void)x;
    return float8_e4m3x2_t{};
}

template <ROUND rnd, RoundingSaturation rst, typename SRC_TYPE>
float8_e5m2x2_t __cvt_float8_e5m2x2_t(SRC_TYPE x)
{
    (void)x;
    return float8_e5m2x2_t{};
}

#define __SIMT_DEVICE_FUNCTIONS_DECL__
#define __simt_callee__
#define __ASCC_PRE__
#define __aicore__
#define TILING_KEY_VAR g_tilingKey

#undef ASCENDC_CPU_DEBUG
#include "impl/simt_api/asc_fp4_impl.h"
#include "impl/simt_api/asc_fp8_impl.h"

class Fp4Fp8Bf16EncapsulationTestsuite : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(Fp4Fp8Bf16EncapsulationTestsuite, Fp4E1m2Bfloat16EncapsulationTest)
{
    bfloat16x2_t bf16x2{};
    float4_e1m2x2_t fp4x2{};

    bfloat16x2_t r0 = __fp4x2_e1m22bfloat162(fp4x2);
    (void)r0;
    float4_e1m2x2_t r1 = __bfloat1622fp4x2_e1m2_rn(bf16x2);
    (void)r1;
    float4_e1m2x2_t r2 = __bfloat1622fp4x2_e1m2_rna(bf16x2);
    (void)r2;
    float4_e1m2x2_t r3 = __bfloat1622fp4x2_e1m2_rd(bf16x2);
    (void)r3;
    float4_e1m2x2_t r4 = __bfloat1622fp4x2_e1m2_ru(bf16x2);
    (void)r4;
    float4_e1m2x2_t r5 = __bfloat1622fp4x2_e1m2_rz(bf16x2);
    (void)r5;

    EXPECT_EQ(r0.x.ToFloat(), 0.0f);
    EXPECT_EQ(r1.val, 0);
}

TEST_F(Fp4Fp8Bf16EncapsulationTestsuite, Fp4E2m1Bfloat16EncapsulationTest)
{
    bfloat16x2_t bf16x2{};
    float4_e2m1x2_t fp4x2{};

    bfloat16x2_t r0 = __fp4x2_e2m12bfloat162(fp4x2);
    (void)r0;
    float4_e2m1x2_t r1 = __bfloat1622fp4x2_e2m1_rn(bf16x2);
    (void)r1;
    float4_e2m1x2_t r2 = __bfloat1622fp4x2_e2m1_rna(bf16x2);
    (void)r2;
    float4_e2m1x2_t r3 = __bfloat1622fp4x2_e2m1_rd(bf16x2);
    (void)r3;
    float4_e2m1x2_t r4 = __bfloat1622fp4x2_e2m1_ru(bf16x2);
    (void)r4;
    float4_e2m1x2_t r5 = __bfloat1622fp4x2_e2m1_rz(bf16x2);
    (void)r5;

    EXPECT_EQ(r0.x.ToFloat(), 0.0f);
    EXPECT_EQ(r1.val, 0);
}

TEST_F(Fp4Fp8Bf16EncapsulationTestsuite, Fp8E8m0Bfloat16EncapsulationTest)
{
    bfloat16x2_t bf16x2{};
    float8_e8m0x2_t fp8x2{};

    bfloat16x2_t r0 = __fp8x2_e8m02bfloat162(fp8x2);
    (void)r0;
    float8_e8m0x2_t r1 = __bfloat1622fp8x2_e8m0_ru(bf16x2);
    (void)r1;
    float8_e8m0x2_t r2 = __bfloat1622fp8x2_e8m0_ru_sat(bf16x2);
    (void)r2;
    float8_e8m0x2_t r3 = __bfloat1622fp8x2_e8m0_rz(bf16x2);
    (void)r3;
    float8_e8m0x2_t r4 = __bfloat1622fp8x2_e8m0_rz_sat(bf16x2);
    (void)r4;

    EXPECT_EQ(r0.x.ToFloat(), 0.0f);
    EXPECT_EQ(r1.x.val, 0);
}

TEST_F(Fp4Fp8Bf16EncapsulationTestsuite, Fp8E6m2Bfloat16EncapsulationTest)
{
    bfloat16x2_t bf16x2{};
    float8_e6m2x2_t fp8x2{};

    bfloat16x2_t r0 = __fp8x2_e6m22bfloat162(fp8x2);
    (void)r0;
    bfloat16x2_t r1 = __rcp_fp8x2_e6m22bfloat162(fp8x2);
    (void)r1;
    float8_e6m2x2_t r2 = __bfloat1622fp8x2_e6m2_rn(bf16x2);
    (void)r2;

    EXPECT_EQ(r0.x.ToFloat(), 0.0f);
    EXPECT_EQ(r1.x.ToFloat(), 0.0f);
    EXPECT_EQ(r2.x.val, 0);
}
