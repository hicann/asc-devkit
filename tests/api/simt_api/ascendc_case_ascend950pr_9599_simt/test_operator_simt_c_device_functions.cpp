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
#include "simt_compiler_stub.h"
#include "kernel_operator.h"
#include "simt_api/device_functions.h"

using namespace std;
using namespace AscendC;
using namespace AscendC::Simt;

struct DeviceFuncParams {
    int32_t mode;
};

class DeviceTestsuite : public testing::Test, public testing::WithParamInterface<DeviceFuncParams> {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(DeviceTestsuite, MathApiTest)
{
    int result = __float_as_int(1);
    EXPECT_EQ(1065353216, result);
}

void VerifyLdStFloatNumber(float x, float xExpected, float epsilon = 1e-5)
{
    if (std::isnan(xExpected)) {
        EXPECT_TRUE(std::isnan(x));
    } else if (std::isinf(xExpected)) {
        EXPECT_TRUE(std::isinf(x));
        if (xExpected > 0.0) {
            EXPECT_GT(x, 0.0);
        } else {
            EXPECT_LT(x, 0.0);
        }
    } else {
        EXPECT_NEAR(x, xExpected, epsilon);
    }
}

// ================================ Test ldcg/ldca/stcg/stwt long int start ================================
struct LdStLongTestParam {
    long int x;
    long int yExpected;
};

class LdStLongTestSuite : public ::testing::TestWithParam<LdStLongTestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};

INSTANTIATE_TEST_CASE_P(
    LdStLongTestCase, LdStLongTestSuite, ::testing::Values(LdStLongTestParam{0, 0}, LdStLongTestParam{-10, -10}));

TEST_P(LdStLongTestSuite, LdStLongTestCase)
{
    const auto param = this->GetParam();
    long int x = param.x;
    long int yExpected = param.yExpected;
    long int y = asc_ldcg(&x);
    EXPECT_EQ(y, yExpected);

    y = asc_ldca(&x);
    EXPECT_EQ(y, yExpected);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt long int end ==================================

// ================================ Test ldcg/ldca/stcg/stwt unsigned long int start ================================
struct LdStUlongTestParam {
    unsigned long int x;
    unsigned long int yExpected;
};

class LdStUlongTestSuite : public ::testing::TestWithParam<LdStUlongTestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};

INSTANTIATE_TEST_CASE_P(
    LdStUlongTestCase, LdStUlongTestSuite, ::testing::Values(LdStUlongTestParam{0, 0}, LdStUlongTestParam{10, 10}));

TEST_P(LdStUlongTestSuite, LdStUlongTestCase)
{
    const auto param = this->GetParam();
    unsigned long int x = param.x;
    unsigned long int yExpected = param.yExpected;
    unsigned long int y = asc_ldcg(&x);
    EXPECT_EQ(y, yExpected);

    y = asc_ldca(&x);
    EXPECT_EQ(y, yExpected);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt unsigned long int end ==================================

// ================================ Test ldcg/ldca/stcg/stwt long long int start ================================
struct LdStLonglongTestParam {
    long long int x;
    long long int yExpected;
};

class LdStLonglongTestSuite : public ::testing::TestWithParam<LdStLonglongTestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};
INSTANTIATE_TEST_CASE_P(
    LdStLonglongTestCase, LdStLonglongTestSuite,
    ::testing::Values(LdStLonglongTestParam{0, 0}, LdStLonglongTestParam{10, 10}));

TEST_P(LdStLonglongTestSuite, LdStLonglongTestCase)
{
    const auto param = this->GetParam();
    long long int x = param.x;
    long long int yExpected = param.yExpected;
    long long int y = asc_ldcg(&x);
    EXPECT_EQ(y, yExpected);

    y = asc_ldca(&x);
    EXPECT_EQ(y, yExpected);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt long long int end ==================================

// ================================ Test ldcg/ldca/stcg/stwt unsigned long long int start  ===========================
struct LdStUlonglongTestParam {
    unsigned long long int x;
    unsigned long long int yExpected;
};

class LdStUlonglongTestSuite : public ::testing::TestWithParam<LdStUlonglongTestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};
INSTANTIATE_TEST_CASE_P(
    LdStUlonglongTestCase, LdStUlonglongTestSuite,
    ::testing::Values(LdStUlonglongTestParam{0, 0}, LdStUlonglongTestParam{10, 10}));

TEST_P(LdStUlonglongTestSuite, LdStUlonglongTestCase)
{
    const auto param = this->GetParam();
    unsigned long long int x = param.x;
    unsigned long long int yExpected = param.yExpected;
    unsigned long long int y = asc_ldcg(&x);
    EXPECT_EQ(y, yExpected);

    y = asc_ldca(&x);
    EXPECT_EQ(y, yExpected);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt unsigned long long int end =====================

// ================================ Test ldcg/ldca/stcg/stwt long2 start ================================
struct LdStLong2TestParam {
    long2 x;
    long2 yExpected;
};

class LdStLong2TestSuite : public ::testing::TestWithParam<LdStLong2TestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};
INSTANTIATE_TEST_CASE_P(
    LdStLong2TestCase, LdStLong2TestSuite,
    ::testing::Values(LdStLong2TestParam{{0, 0}, {0, 0}}, LdStLong2TestParam{{10, 10}, {10, 10}}));

TEST_P(LdStLong2TestSuite, LdStLong2TestCase)
{
    const auto param = this->GetParam();
    long2 x = param.x;
    long2 yExpected = param.yExpected;
    long2 y = asc_ldcg(&x);
    EXPECT_EQ(y.x, yExpected.x);

    y = asc_ldca(&x);
    EXPECT_EQ(y.x, yExpected.x);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt long2 end ==================================

// ================================ Test ldcg/ldca/stcg/stwt ulong2 start ================================
struct LdStUlong2TestParam {
    ulong2 x;
    ulong2 yExpected;
};

class LdStUlong2TestSuite : public ::testing::TestWithParam<LdStUlong2TestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};
INSTANTIATE_TEST_CASE_P(
    LdStUlong2TestCase, LdStUlong2TestSuite,
    ::testing::Values(LdStUlong2TestParam{{0, 0}, {0, 0}}, LdStUlong2TestParam{{10, 10}, {10, 10}}));

TEST_P(LdStUlong2TestSuite, LdStUlong2TestCase)
{
    const auto param = this->GetParam();
    ulong2 x = param.x;
    ulong2 yExpected = param.yExpected;
    ulong2 y = asc_ldcg(&x);
    EXPECT_EQ(y.x, yExpected.x);

    y = asc_ldca(&x);
    EXPECT_EQ(y.x, yExpected.x);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt ulong2 int end ==================================

// ================================ Test ldcg/ldca/stcg/stwt long4 start ================================
struct LdStLong4TestParam {
    long4 x;
    long4 yExpected;
};

class LdStLong4TestSuite : public ::testing::TestWithParam<LdStLong4TestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};
INSTANTIATE_TEST_CASE_P(
    LdStLong4TestCase, LdStLong4TestSuite,
    ::testing::Values(
        LdStLong4TestParam{{0, 0, 0, 0}, {0, 0, 0, 0}}, LdStLong4TestParam{{10, 10, 10, 10}, {10, 10, 10, 10}}));

TEST_P(LdStLong4TestSuite, LdStLong4TestCase)
{
    const auto param = this->GetParam();
    long4 x = param.x;
    long4 yExpected = param.yExpected;
    long4 y = asc_ldcg(&x);
    EXPECT_EQ(y.x, yExpected.x);

    y = asc_ldca(&x);
    EXPECT_EQ(y.x, yExpected.x);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt long4 end ==================================

// ================================ Test ldcg/ldca/stcg/stwt longlong2 start ================================
struct LdStLonglong2TestParam {
    longlong2 x;
    longlong2 yExpected;
};

class LdStLonglong2TestSuite : public ::testing::TestWithParam<LdStLonglong2TestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};
INSTANTIATE_TEST_CASE_P(
    LdStTestCase, LdStLonglong2TestSuite,
    ::testing::Values(LdStLonglong2TestParam{{0, 0}, {0, 0}}, LdStLonglong2TestParam{{10, 10}, {10, 10}}));

TEST_P(LdStLonglong2TestSuite, LdStLonglong2TestCase)
{
    const auto param = this->GetParam();
    longlong2 x = param.x;
    longlong2 yExpected = param.yExpected;
    longlong2 y = asc_ldcg(&x);
    EXPECT_EQ(y.x, yExpected.x);

    y = asc_ldca(&x);
    EXPECT_EQ(y.x, yExpected.x);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt longlong2 end ==================================

// ================================ Test ldcg/ldca/stcg/stwt ulonglong2 start ================================
struct LdStUlonglong2TestParam {
    ulonglong2 x;
    ulonglong2 yExpected;
};

class LdStUlonglong2TestSuite : public ::testing::TestWithParam<LdStUlonglong2TestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};
INSTANTIATE_TEST_CASE_P(
    LdStUlonglong2TestCase, LdStUlonglong2TestSuite,
    ::testing::Values(LdStUlonglong2TestParam{{0, 0}, {0, 0}}, LdStUlonglong2TestParam{{10, 10}, {10, 10}}));

TEST_P(LdStUlonglong2TestSuite, LdStuLonglong2TestCase)
{
    const auto param = this->GetParam();
    ulonglong2 x = param.x;
    ulonglong2 yExpected = param.yExpected;
    ulonglong2 y = asc_ldcg(&x);
    EXPECT_EQ(y.x, yExpected.x);

    y = asc_ldca(&x);
    EXPECT_EQ(y.x, yExpected.x);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt ulonglong2 end ==================================

// ================================ Test ldcg/ldca/stcg/stwt longlong4 start ================================
struct LdStLonglong4TestParam {
    longlong4 x;
    longlong4 yExpected;
};

class LdStLonglong4TestSuite : public ::testing::TestWithParam<LdStLonglong4TestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};
INSTANTIATE_TEST_CASE_P(
    LdStLonglong4TestCase, LdStLonglong4TestSuite,
    ::testing::Values(
        LdStLonglong4TestParam{{0, 0, 0, 0}, {0, 0, 0, 0}},
        LdStLonglong4TestParam{{10, 10, 10, 10}, {10, 10, 10, 10}}));

TEST_P(LdStLonglong4TestSuite, LdStLonglong4TestCase)
{
    const auto param = this->GetParam();
    longlong4 x = param.x;
    longlong4 yExpected = param.yExpected;
    longlong4 y = asc_ldcg(&x);
    EXPECT_EQ(y.x, yExpected.x);

    y = asc_ldca(&x);
    EXPECT_EQ(y.x, yExpected.x);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt longlong4 end ==================================

// ================================ Test ldcg/ldca/stcg/stwt signed char start ================================
struct LdStCharTestParam {
    signed char x;
    signed char yExpected;
};

class LdStCharTestSuite : public ::testing::TestWithParam<LdStCharTestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};
INSTANTIATE_TEST_CASE_P(LdStTestCase, LdStCharTestSuite, ::testing::Values(LdStCharTestParam{'0', '0'}));

TEST_P(LdStCharTestSuite, LdStCharTestCase)
{
    const auto param = this->GetParam();
    signed char x = param.x;
    signed char yExpected = param.yExpected;
    signed char y = asc_ldcg(&x);
    EXPECT_EQ(y, yExpected);

    y = asc_ldca(&x);
    EXPECT_EQ(y, yExpected);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt signed char end ==================================

// ================================ Test ldcg/ldca/stcg/stwt unsigned char start ================================
struct LdStUCharTestParam {
    unsigned char x;
    unsigned char yExpected;
};

class LdStUCharTestSuite : public ::testing::TestWithParam<LdStUCharTestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};
INSTANTIATE_TEST_CASE_P(LdStUCharTestCase, LdStUCharTestSuite, ::testing::Values(LdStUCharTestParam{'0', '0'}));

TEST_P(LdStUCharTestSuite, LdStUCharTestCase)
{
    const auto param = this->GetParam();
    unsigned char x = param.x;
    unsigned char yExpected = param.yExpected;
    unsigned char y = asc_ldcg(&x);
    EXPECT_EQ(y, yExpected);

    y = asc_ldca(&x);
    EXPECT_EQ(y, yExpected);
    EXPECT_EQ(y, yExpected);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt unsigned char end ==================================

// ================================ Test stcg/stwt plain char start ================================
struct StPlainCharTestParam {
    char x;
};

class StPlainCharTestSuite : public ::testing::TestWithParam<StPlainCharTestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};
INSTANTIATE_TEST_CASE_P(StPlainCharTestCase, StPlainCharTestSuite, ::testing::Values(StPlainCharTestParam{'a'}));

TEST_P(StPlainCharTestSuite, StPlainCharTestCase)
{
    const auto param = this->GetParam();
    char x = param.x;
    asc_stcg(&x, x);
    asc_stwt(&x, x);
    EXPECT_EQ(x, param.x);
}
// ================================ Test stcg/stwt plain char end ==================================

// ================================ Test ldcg/ldca/stcg/stwt char2 start ================================
struct LdStChar2TestParam {
    char2 x;
    char2 yExpected;
};

class LdStChar2TestSuite : public ::testing::TestWithParam<LdStChar2TestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};
INSTANTIATE_TEST_CASE_P(
    LdStChar2TestCase, LdStChar2TestSuite, ::testing::Values(LdStChar2TestParam{{'0', '0'}, {'0', '0'}}));

TEST_P(LdStChar2TestSuite, LdStChar2TestCase)
{
    const auto param = this->GetParam();
    char2 x = param.x;
    char2 yExpected = param.yExpected;
    char2 y = asc_ldcg(&x);
    EXPECT_EQ(y.x, yExpected.x);

    y = asc_ldca(&x);
    EXPECT_EQ(y.x, yExpected.x);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt char2 end ==================================

// ================================ Test ldcg/ldca/stcg/stwt uchar2 start ================================
struct LdStUchar2TestParam {
    uchar2 x;
    uchar2 yExpected;
};

class LdStUchar2TestSuite : public ::testing::TestWithParam<LdStUchar2TestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};
INSTANTIATE_TEST_CASE_P(
    LdStUchar2TestCase, LdStUchar2TestSuite, ::testing::Values(LdStUchar2TestParam{{'0', '0'}, {'0', '0'}}));

TEST_P(LdStUchar2TestSuite, LdStUChar2TestCase)
{
    const auto param = this->GetParam();
    uchar2 x = param.x;
    uchar2 yExpected = param.yExpected;
    uchar2 y = asc_ldcg(&x);
    EXPECT_EQ(y.x, yExpected.x);

    y = asc_ldca(&x);
    EXPECT_EQ(y.x, yExpected.x);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt unsigned char end ==================================

// ================================ Test ldcg/ldca/stcg/stwt char4 start ================================
struct LdStChar4TestParam {
    char4 x;
    char4 yExpected;
};

class LdStChar4TestSuite : public ::testing::TestWithParam<LdStChar4TestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};
INSTANTIATE_TEST_CASE_P(
    LdStChar4TestCase, LdStChar4TestSuite,
    ::testing::Values(LdStChar4TestParam{{'0', '0', '0', '0'}, {'0', '0', '0', '0'}}));

TEST_P(LdStChar4TestSuite, LdStChar4TestCase)
{
    const auto param = this->GetParam();
    char4 x = param.x;
    char4 yExpected = param.yExpected;
    char4 y = asc_ldcg(&x);
    EXPECT_EQ(y.x, yExpected.x);

    y = asc_ldca(&x);
    EXPECT_EQ(y.x, yExpected.x);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt char4 end ==================================

// ================================ Test ldcg/ldca/stcg/stwt uchar4 start ================================
struct LdStUchar4TestParam {
    uchar4 x;
    uchar4 yExpected;
};

class LdStUchar4TestSuite : public ::testing::TestWithParam<LdStUchar4TestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};
INSTANTIATE_TEST_CASE_P(
    LdStUchar4TestCase, LdStUchar4TestSuite,
    ::testing::Values(LdStUchar4TestParam{{'0', '0', '0', '0'}, {'0', '0', '0', '0'}}));

TEST_P(LdStUchar4TestSuite, LdStUChar4TestCase)
{
    const auto param = this->GetParam();
    uchar4 x = param.x;
    uchar4 yExpected = param.yExpected;
    uchar4 y = asc_ldcg(&x);
    EXPECT_EQ(y.x, yExpected.x);

    y = asc_ldca(&x);
    EXPECT_EQ(y.x, yExpected.x);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt uchar4 end ==================================

// ================================ Test ldcg/ldca/stcg/stwt short start ================================
struct LdStShortTestParam {
    short x;
    short yExpected;
};

class LdStShortTestSuite : public ::testing::TestWithParam<LdStShortTestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};
INSTANTIATE_TEST_CASE_P(LdStShortTestCase, LdStShortTestSuite, ::testing::Values(LdStShortTestParam{1, 1}));

TEST_P(LdStShortTestSuite, LdStShortTestCase)
{
    const auto param = this->GetParam();
    short x = param.x;
    short yExpected = param.yExpected;
    short y = asc_ldcg(&x);
    EXPECT_EQ(y, yExpected);

    y = asc_ldca(&x);
    EXPECT_EQ(y, yExpected);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt short end ==================================

// ================================ Test ldcg/ldca/stcg/stwt unsigned short start ================================
struct LdStUshortTestParam {
    unsigned short x;
    unsigned short yExpected;
};

class LdStUshortTestSuite : public ::testing::TestWithParam<LdStUshortTestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};
INSTANTIATE_TEST_CASE_P(
    LdStUshortTestCase, LdStUshortTestSuite,
    ::testing::Values(LdStUshortTestParam{(unsigned short)1, (unsigned short)1}));

TEST_P(LdStUshortTestSuite, LdStUshortTestCase)
{
    const auto param = this->GetParam();
    unsigned short x = param.x;
    unsigned short yExpected = param.yExpected;
    unsigned short y = asc_ldcg(&x);
    EXPECT_EQ(y, yExpected);

    y = asc_ldca(&x);
    EXPECT_EQ(y, yExpected);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt unsigned short end ==================================

// ================================ Test ldcg/ldca/stcg/stwt short2 start ================================
struct LdStShort2TestParam {
    short2 x;
    short2 yExpected;
};

class LdStShort2TestSuite : public ::testing::TestWithParam<LdStShort2TestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};
INSTANTIATE_TEST_CASE_P(
    LdStShort2TestCase, LdStShort2TestSuite, ::testing::Values(LdStShort2TestParam{{1, 2}, {1, 2}}));

TEST_P(LdStShort2TestSuite, LdStShort2TestCase)
{
    const auto param = this->GetParam();
    short2 x = param.x;
    short2 yExpected = param.yExpected;
    short2 y = asc_ldcg(&x);
    EXPECT_EQ(y.x, yExpected.x);

    y = asc_ldca(&x);
    EXPECT_EQ(y.x, yExpected.x);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt short2 end ==================================

// ================================ Test ldcg/ldca/stcg/stwt short4 start ================================
struct LdStShort4TestParam {
    short4 x;
    short4 yExpected;
};

class LdStShort4TestSuite : public ::testing::TestWithParam<LdStShort4TestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};
INSTANTIATE_TEST_CASE_P(
    LdStShort4TestCase, LdStShort4TestSuite, ::testing::Values(LdStShort4TestParam{{1, 2, 1, 2}, {1, 2, 1, 2}}));

TEST_P(LdStShort4TestSuite, LdStShort4TestCase)
{
    const auto param = this->GetParam();
    short4 x = param.x;
    short4 yExpected = param.yExpected;
    short4 y = asc_ldcg(&x);
    EXPECT_EQ(y.x, yExpected.x);

    y = asc_ldca(&x);
    EXPECT_EQ(y.x, yExpected.x);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt short4 end ==================================

// ================================ Test ldcg/ldca/stcg/stwt ushort4 start ================================
struct LdStUshort4TestParam {
    ushort4 x;
    ushort4 yExpected;
};

class LdStUshort4TestSuite : public ::testing::TestWithParam<LdStUshort4TestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};
INSTANTIATE_TEST_CASE_P(
    LdStUshort4TestCase, LdStUshort4TestSuite, ::testing::Values(LdStUshort4TestParam{{1, 2, 1, 2}, {1, 2, 1, 2}}));

TEST_P(LdStUshort4TestSuite, LdStUshort4TestCase)
{
    const auto param = this->GetParam();
    ushort4 x = param.x;
    ushort4 yExpected = param.yExpected;
    ushort4 y = asc_ldcg(&x);
    EXPECT_EQ(y.x, yExpected.x);

    y = asc_ldca(&x);
    EXPECT_EQ(y.x, yExpected.x);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt ushort4 end ==================================

// ================================ Test ldcg/ldca/stcg/stwt int start ================================
struct LdStIntTestParam {
    int x;
    int yExpected;
};

class LdStIntTestSuite : public ::testing::TestWithParam<LdStIntTestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};
INSTANTIATE_TEST_CASE_P(LdStIntTestCase, LdStIntTestSuite, ::testing::Values(LdStIntTestParam{1, 1}));

TEST_P(LdStIntTestSuite, LdStIntTestCase)
{
    const auto param = this->GetParam();
    int x = param.x;
    int yExpected = param.yExpected;
    int y = asc_ldcg(&x);
    EXPECT_EQ(y, yExpected);

    y = asc_ldca(&x);
    EXPECT_EQ(y, yExpected);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt int end ==================================

// ================================ Test ldcg/ldca/stcg/stwt unsigned int start ================================
struct LdStUintTestParam {
    unsigned int x;
    unsigned int yExpected;
};

class LdStUintTestSuite : public ::testing::TestWithParam<LdStUintTestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};
INSTANTIATE_TEST_CASE_P(LdStUintTestCase, LdStUintTestSuite, ::testing::Values(LdStUintTestParam{1, 1}));

TEST_P(LdStUintTestSuite, LdStUintTestCase)
{
    const auto param = this->GetParam();
    unsigned int x = param.x;
    unsigned int yExpected = param.yExpected;
    unsigned int y = asc_ldcg(&x);
    EXPECT_EQ(y, yExpected);

    y = asc_ldca(&x);
    EXPECT_EQ(y, yExpected);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt unsigned int end ==================================

// ================================ Test ldcg/ldca/stcg/stwt int2 start ================================
struct LdStInt2TestParam {
    int2 x;
    int2 yExpected;
};

class LdStInt2TestSuite : public ::testing::TestWithParam<LdStInt2TestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};
INSTANTIATE_TEST_CASE_P(LdStInt2TestCase, LdStInt2TestSuite, ::testing::Values(LdStInt2TestParam{{1, 2}, {1, 2}}));

TEST_P(LdStInt2TestSuite, LdStInt2TestCase)
{
    const auto param = this->GetParam();
    int2 x = param.x;
    int2 yExpected = param.yExpected;
    int2 y = asc_ldcg(&x);
    EXPECT_EQ(y.x, yExpected.x);

    y = asc_ldca(&x);
    EXPECT_EQ(y.x, yExpected.x);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt unsigned int2 end ==================================

// ================================ Test ldcg/ldca/stcg/stwt uint2 start ================================
struct LdStUint2TestParam {
    uint2 x;
    uint2 yExpected;
};

class LdStUint2TestSuite : public ::testing::TestWithParam<LdStUint2TestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};
INSTANTIATE_TEST_CASE_P(LdStUint2TestCase, LdStUint2TestSuite, ::testing::Values(LdStUint2TestParam{{1, 2}, {1, 2}}));

TEST_P(LdStUint2TestSuite, LdStint2TestCase)
{
    const auto param = this->GetParam();
    uint2 x = param.x;
    uint2 yExpected = param.yExpected;
    uint2 y = asc_ldcg(&x);
    EXPECT_EQ(y.x, yExpected.x);

    y = asc_ldca(&x);
    EXPECT_EQ(y.x, yExpected.x);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt unsigned int2 end ==================================

// ================================ Test ldcg/ldca/stcg/stwt int4 start ================================
struct LdStInt4TestParam {
    int4 x;
    int4 yExpected;
};

class LdStInt4TestSuite : public ::testing::TestWithParam<LdStInt4TestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};
INSTANTIATE_TEST_CASE_P(
    LdStInt4TestCase, LdStInt4TestSuite, ::testing::Values(LdStInt4TestParam{{1, 2, 3, 4}, {1, 2, 3, 4}}));

TEST_P(LdStInt4TestSuite, LdStInt4TestCase)
{
    const auto param = this->GetParam();
    int4 x = param.x;
    int4 yExpected = param.yExpected;
    int4 y = asc_ldcg(&x);
    EXPECT_EQ(y.x, yExpected.x);

    y = asc_ldca(&x);
    EXPECT_EQ(y.x, yExpected.x);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt unsigned int4 end ==================================

// ================================ Test ldcg/ldca/stcg/stwt uint4 start ================================
struct LdStUint4TestParam {
    uint4 x;
    uint4 yExpected;
};

class LdStUint4TestSuite : public ::testing::TestWithParam<LdStUint4TestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};
INSTANTIATE_TEST_CASE_P(
    LdStUint4TestCase, LdStUint4TestSuite, ::testing::Values(LdStUint4TestParam{{1, 2, 3, 4}, {1, 2, 3, 4}}));

TEST_P(LdStUint4TestSuite, LdStuint4TestCase)
{
    const auto param = this->GetParam();
    uint4 x = param.x;
    uint4 yExpected = param.yExpected;
    uint4 y = asc_ldcg(&x);
    EXPECT_EQ(y.x, yExpected.x);

    y = asc_ldca(&x);
    EXPECT_EQ(y.x, yExpected.x);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt unsigned uint4 end ==================================

// ================================ Test ldcg/ldca/stcg/stwt float start ================================
struct LdStFloatTestParam {
    float x;
    float yExpected;
};

class LdStFloatTestSuite : public ::testing::TestWithParam<LdStFloatTestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};
INSTANTIATE_TEST_CASE_P(LdStFloatTestCase, LdStFloatTestSuite, ::testing::Values(LdStFloatTestParam{1.0f, 1.0f}));

TEST_P(LdStFloatTestSuite, LdStFloatTestCase)
{
    const auto param = this->GetParam();
    float x = param.x;
    float yExpected = param.yExpected;
    float y = asc_ldcg(&x);
    EXPECT_EQ(y, yExpected);

    y = asc_ldca(&x);
    EXPECT_EQ(y, yExpected);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt float end ==================================

// ================================ Test ldcg/ldca/stcg/stwt float2 start ================================

struct LdStFloat2TestParam {
    float2 x;
    float2 yExpected;
};

class LdStFloat2TestSuite : public ::testing::TestWithParam<LdStFloat2TestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};

INSTANTIATE_TEST_CASE_P(
    LdStfloat2TestCase, LdStFloat2TestSuite, ::testing::Values(LdStFloat2TestParam{{1.0f, 2.0f}, {1.0f, 2.0f}}));

TEST_P(LdStFloat2TestSuite, LdStfloat2TestCase)
{
    const auto param = this->GetParam();
    float2 x = param.x;
    float2 yExpected = param.yExpected;
    float2 y = asc_ldcg(&x);
    EXPECT_EQ(y.x, yExpected.x);

    y = asc_ldca(&x);
    EXPECT_EQ(y.x, yExpected.x);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt float2 end ==================================

// ================================ Test ldcg/ldca/stcg/stwt float4 start ================================

struct LdStFloat4TestParam {
    float4 x;
    float4 yExpected;
};

class LdStFloat4TestSuite : public ::testing::TestWithParam<LdStFloat4TestParam> {
public:
    void SetUp() override {}
    void TearDown() override {}
};

INSTANTIATE_TEST_CASE_P(
    LdStFloat4TestSuite, LdStFloat4TestSuite,
    ::testing::Values(LdStFloat4TestParam{{1.0f, 2.0f, 3.0f, 4.0f}, {1.0f, 2.0f, 3.0f, 4.0f}}));

TEST_P(LdStFloat4TestSuite, LdStfloat4TestCase)
{
    const auto param = this->GetParam();
    float4 x = param.x;
    float4 yExpected = param.yExpected;
    float4 y = asc_ldcg(&x);
    EXPECT_EQ(y.x, yExpected.x);

    y = asc_ldca(&x);
    EXPECT_EQ(y.x, yExpected.x);

    asc_stcg(&x, x);
    asc_stwt(&x, x);
}
// ================================ Test ldcg/ldca/stcg/stwt float4 end ==================================
