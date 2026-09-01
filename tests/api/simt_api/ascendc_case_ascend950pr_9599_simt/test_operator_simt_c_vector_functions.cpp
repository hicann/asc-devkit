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
#include <type_traits>
#include <random>
#include "simt_api/vector_functions.h"

using namespace std;

struct VectorFuncParams {
    int32_t mode;
};

class VectorTestsuite : public testing::Test, public testing::WithParamInterface<VectorFuncParams> {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(VectorTestsuite, MakeFloatApiTest)
{
    float result_fp = static_cast<float>(rand()) / RAND_MAX;
    float2 expect_fp2 = make_float2(result_fp, result_fp);
    EXPECT_EQ(expect_fp2.x, result_fp);
    EXPECT_EQ(expect_fp2.y, result_fp);

    float3 expect_fp3 = make_float3(result_fp, result_fp, result_fp);
    EXPECT_EQ(expect_fp3.x, result_fp);
    EXPECT_EQ(expect_fp3.y, result_fp);
    EXPECT_EQ(expect_fp3.z, result_fp);

    float4 expect_fp4 = make_float4(result_fp, result_fp, result_fp, result_fp);
    EXPECT_EQ(expect_fp4.x, result_fp);
    EXPECT_EQ(expect_fp4.y, result_fp);
    EXPECT_EQ(expect_fp4.z, result_fp);
}

TEST_F(VectorTestsuite, MakeCharApiTest)
{
    char result_ch = '1';
    char2 expect_ch2 = make_char2(result_ch, result_ch);
    EXPECT_EQ(expect_ch2.x, result_ch);
    EXPECT_EQ(expect_ch2.y, result_ch);

    char3 expect_ch3 = make_char3(result_ch, result_ch, result_ch);
    EXPECT_EQ(expect_ch3.x, result_ch);
    EXPECT_EQ(expect_ch3.y, result_ch);
    EXPECT_EQ(expect_ch3.z, result_ch);

    char4 expect_ch4 = make_char4(result_ch, result_ch, result_ch, result_ch);
    EXPECT_EQ(expect_ch4.x, result_ch);
    EXPECT_EQ(expect_ch4.y, result_ch);
    EXPECT_EQ(expect_ch4.z, result_ch);
    EXPECT_EQ(expect_ch4.w, result_ch);

    unsigned char result_uch = '1';
    uchar2 expect_uch2 = make_uchar2(result_uch, result_uch);
    EXPECT_EQ(expect_uch2.x, result_uch);
    EXPECT_EQ(expect_uch2.y, result_uch);

    uchar3 expect_uch3 = make_uchar3(result_uch, result_uch, result_uch);
    EXPECT_EQ(expect_uch3.x, result_uch);
    EXPECT_EQ(expect_uch3.y, result_uch);
    EXPECT_EQ(expect_uch3.z, result_uch);

    uchar4 expect_uch4 = make_uchar4(result_uch, result_uch, result_uch, result_uch);
    EXPECT_EQ(expect_uch4.x, result_uch);
    EXPECT_EQ(expect_uch4.y, result_uch);
    EXPECT_EQ(expect_uch4.z, result_uch);
    EXPECT_EQ(expect_uch4.w, result_uch);
}

TEST_F(VectorTestsuite, MakeShortApiTest)
{
    short result_sh = static_cast<short>(rand()) / RAND_MAX;
    short2 expect_sh2 = make_short2(result_sh, result_sh);
    EXPECT_EQ(expect_sh2.x, result_sh);
    EXPECT_EQ(expect_sh2.y, result_sh);

    short3 expect_sh3 = make_short3(result_sh, result_sh, result_sh);
    EXPECT_EQ(expect_sh3.x, result_sh);
    EXPECT_EQ(expect_sh3.y, result_sh);
    EXPECT_EQ(expect_sh3.z, result_sh);

    short4 expect_sh4 = make_short4(result_sh, result_sh, result_sh, result_sh);
    EXPECT_EQ(expect_sh4.x, result_sh);
    EXPECT_EQ(expect_sh4.y, result_sh);
    EXPECT_EQ(expect_sh4.z, result_sh);
    EXPECT_EQ(expect_sh4.w, result_sh);

    unsigned short result_ush = static_cast<unsigned short>(rand()) / RAND_MAX;
    ushort2 expect_ush2 = make_ushort2(result_ush, result_ush);
    EXPECT_EQ(expect_ush2.x, result_ush);
    EXPECT_EQ(expect_ush2.y, result_ush);

    ushort3 expect_ush3 = make_ushort3(result_ush, result_ush, result_ush);
    EXPECT_EQ(expect_ush3.x, result_ush);
    EXPECT_EQ(expect_ush3.y, result_ush);
    EXPECT_EQ(expect_ush3.z, result_ush);

    ushort4 expect_ush4 = make_ushort4(result_ush, result_ush, result_ush, result_ush);
    EXPECT_EQ(expect_ush4.x, result_ush);
    EXPECT_EQ(expect_ush4.y, result_ush);
    EXPECT_EQ(expect_ush4.z, result_ush);
    EXPECT_EQ(expect_ush4.w, result_ush);
}

TEST_F(VectorTestsuite, MakeIntApiTest)
{
    int result_int = static_cast<int>(rand()) / RAND_MAX;
    int2 expect_int2 = make_int2(result_int, result_int);
    EXPECT_EQ(expect_int2.x, result_int);
    EXPECT_EQ(expect_int2.y, result_int);

    int3 expect_int3 = make_int3(result_int, result_int, result_int);
    EXPECT_EQ(expect_int3.x, result_int);
    EXPECT_EQ(expect_int3.y, result_int);
    EXPECT_EQ(expect_int3.z, result_int);

    int4 expect_int4 = make_int4(result_int, result_int, result_int, result_int);
    EXPECT_EQ(expect_int4.x, result_int);
    EXPECT_EQ(expect_int4.y, result_int);
    EXPECT_EQ(expect_int4.z, result_int);
    EXPECT_EQ(expect_int4.w, result_int);

    unsigned int result_uint = static_cast<unsigned int>(rand()) / RAND_MAX;
    uint2 expect_uint2 = make_uint2(result_uint, result_uint);
    EXPECT_EQ(expect_uint2.x, result_uint);
    EXPECT_EQ(expect_uint2.y, result_uint);

    uint3 expect_uint3 = make_uint3(result_uint, result_uint, result_uint);
    EXPECT_EQ(expect_uint3.x, result_uint);
    EXPECT_EQ(expect_uint3.y, result_uint);
    EXPECT_EQ(expect_uint3.z, result_uint);

    uint4 expect_uint4 = make_uint4(result_uint, result_uint, result_uint, result_uint);
    EXPECT_EQ(expect_uint4.x, result_uint);
    EXPECT_EQ(expect_uint4.y, result_uint);
    EXPECT_EQ(expect_uint4.z, result_uint);
    EXPECT_EQ(expect_uint4.w, result_uint);
}

TEST_F(VectorTestsuite, MakeLongApiTest)
{
    long int result_long = static_cast<long int>(rand()) / RAND_MAX;
    long2 expect_long2 = make_long2(result_long, result_long);
    EXPECT_EQ(expect_long2.x, result_long);
    EXPECT_EQ(expect_long2.y, result_long);

    long3 expect_long3 = make_long3(result_long, result_long, result_long);
    EXPECT_EQ(expect_long3.x, result_long);
    EXPECT_EQ(expect_long3.y, result_long);
    EXPECT_EQ(expect_long3.z, result_long);

    long4 expect_long4 = make_long4(result_long, result_long, result_long, result_long);
    EXPECT_EQ(expect_long4.x, result_long);
    EXPECT_EQ(expect_long4.y, result_long);
    EXPECT_EQ(expect_long4.z, result_long);
    EXPECT_EQ(expect_long4.w, result_long);

    unsigned long int result_ulong = static_cast<unsigned long int>(rand()) / RAND_MAX;
    ulong2 expect_ulong2 = make_ulong2(result_ulong, result_ulong);
    EXPECT_EQ(expect_ulong2.x, result_ulong);
    EXPECT_EQ(expect_ulong2.y, result_ulong);

    ulong3 expect_ulong3 = make_ulong3(result_ulong, result_ulong, result_ulong);
    EXPECT_EQ(expect_ulong3.x, result_ulong);
    EXPECT_EQ(expect_ulong3.y, result_ulong);
    EXPECT_EQ(expect_ulong3.z, result_ulong);

    ulong4 expect_ulong4 = make_ulong4(result_ulong, result_ulong, result_ulong, result_ulong);
    EXPECT_EQ(expect_ulong4.x, result_ulong);
    EXPECT_EQ(expect_ulong4.y, result_ulong);
    EXPECT_EQ(expect_ulong4.z, result_ulong);
    EXPECT_EQ(expect_ulong4.w, result_ulong);
}

TEST_F(VectorTestsuite, MakeLongLongApiTest)
{
    long long int result_longlong = static_cast<long long int>(rand()) / RAND_MAX;
    longlong2 expect_longlong2 = make_longlong2(result_longlong, result_longlong);
    EXPECT_EQ(expect_longlong2.x, result_longlong);
    EXPECT_EQ(expect_longlong2.y, result_longlong);

    longlong3 expect_longlong3 = make_longlong3(result_longlong, result_longlong, result_longlong);
    EXPECT_EQ(expect_longlong3.x, result_longlong);
    EXPECT_EQ(expect_longlong3.y, result_longlong);
    EXPECT_EQ(expect_longlong3.z, result_longlong);

    longlong4 expect_longlong4 = make_longlong4(result_longlong, result_longlong, result_longlong, result_longlong);
    EXPECT_EQ(expect_longlong4.x, result_longlong);
    EXPECT_EQ(expect_longlong4.y, result_longlong);
    EXPECT_EQ(expect_longlong4.z, result_longlong);
    EXPECT_EQ(expect_longlong4.w, result_longlong);

    unsigned long long int result_ulonglong = static_cast<unsigned long long int>(rand()) / RAND_MAX;
    ulonglong2 expect_ulonglong2 = make_ulonglong2(result_ulonglong, result_ulonglong);
    EXPECT_EQ(expect_ulonglong2.x, result_ulonglong);
    EXPECT_EQ(expect_ulonglong2.y, result_ulonglong);

    ulonglong3 expect_ulonglong3 = make_ulonglong3(result_ulonglong, result_ulonglong, result_ulonglong);
    EXPECT_EQ(expect_ulonglong3.x, result_ulonglong);
    EXPECT_EQ(expect_ulonglong3.y, result_ulonglong);
    EXPECT_EQ(expect_ulonglong3.z, result_ulonglong);

    ulonglong4 expect_ulonglong4 =
        make_ulonglong4(result_ulonglong, result_ulonglong, result_ulonglong, result_ulonglong);
    EXPECT_EQ(expect_ulonglong4.x, result_ulonglong);
    EXPECT_EQ(expect_ulonglong4.y, result_ulonglong);
    EXPECT_EQ(expect_ulonglong4.z, result_ulonglong);
    EXPECT_EQ(expect_ulonglong4.w, result_ulonglong);
}
