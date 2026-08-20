/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <gtest/gtest.h>
#include <type_traits>
#include <cmath>
#include "simt_api/device_atomic_functions.h"
#include "simt_api/asc_fp16.h"
#include "simt_api/asc_bf16.h"

using namespace std;
using namespace __asc_simt_vf;

class AtomicAddTestsuite : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(AtomicAddTestsuite, AtomicAddInt32TestCase)
{
    int32_t address = 100;
    int32_t val = 50;
    int32_t old = asc_atomic_add(&address, val);
    EXPECT_EQ(old, 100);
    EXPECT_EQ(address, 150);
}

TEST_F(AtomicAddTestsuite, AtomicAddUint32TestCase)
{
    uint32_t address = 100;
    uint32_t val = 50;
    uint32_t old = asc_atomic_add(&address, val);
    EXPECT_EQ(old, 100);
    EXPECT_EQ(address, 150);
}

TEST_F(AtomicAddTestsuite, AtomicAddInt64TestCase)
{
    int64_t address = 100;
    int64_t val = 50;
    int64_t old = asc_atomic_add(&address, val);
    EXPECT_EQ(old, 100);
    EXPECT_EQ(address, 150);
}

TEST_F(AtomicAddTestsuite, AtomicAddUint64TestCase)
{
    uint64_t address = 100;
    uint64_t val = 50;
    uint64_t old = asc_atomic_add(&address, val);
    EXPECT_EQ(old, 100);
    EXPECT_EQ(address, 150);
}

TEST_F(AtomicAddTestsuite, AtomicAddFloatTestCase)
{
    float address = 100.0f;
    float val = 50.0f;
    float old = asc_atomic_add(&address, val);
    EXPECT_FLOAT_EQ(old, 100.0f);
    EXPECT_FLOAT_EQ(address, 150.0f);
}

class AtomicSubTestsuite : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(AtomicSubTestsuite, AtomicSubInt32TestCase)
{
    int32_t address = 100;
    int32_t val = 50;
    int32_t old = asc_atomic_sub(&address, val);
    EXPECT_EQ(old, 100);
    EXPECT_EQ(address, 50);
}

TEST_F(AtomicSubTestsuite, AtomicSubUint32TestCase)
{
    uint32_t address = 100;
    uint32_t val = 50;
    uint32_t old = asc_atomic_sub(&address, val);
    EXPECT_EQ(old, 100);
    EXPECT_EQ(address, 50);
}

TEST_F(AtomicSubTestsuite, AtomicSubInt64TestCase)
{
    int64_t address = 100;
    int64_t val = 50;
    int64_t old = asc_atomic_sub(&address, val);
    EXPECT_EQ(old, 100);
    EXPECT_EQ(address, 50);
}

TEST_F(AtomicSubTestsuite, AtomicSubUint64TestCase)
{
    uint64_t address = 100;
    uint64_t val = 50;
    uint64_t old = asc_atomic_sub(&address, val);
    EXPECT_EQ(old, 100);
    EXPECT_EQ(address, 50);
}

TEST_F(AtomicSubTestsuite, AtomicSubFloatTestCase)
{
    float address = 100.0f;
    float val = 50.0f;
    float old = asc_atomic_sub(&address, val);
    EXPECT_FLOAT_EQ(old, 100.0f);
    EXPECT_FLOAT_EQ(address, 50.0f);
}

class AtomicExchTestsuite : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(AtomicExchTestsuite, AtomicExchInt32TestCase)
{
    int32_t address = 100;
    int32_t val = 50;
    int32_t old = asc_atomic_exch(&address, val);
    EXPECT_EQ(old, 100);
    EXPECT_EQ(address, 50);
}

TEST_F(AtomicExchTestsuite, AtomicExchUint32TestCase)
{
    uint32_t address = 100;
    uint32_t val = 50;
    uint32_t old = asc_atomic_exch(&address, val);
    EXPECT_EQ(old, 100);
    EXPECT_EQ(address, 50);
}

TEST_F(AtomicExchTestsuite, AtomicExchInt64TestCase)
{
    int64_t address = 100;
    int64_t val = 50;
    int64_t old = asc_atomic_exch(&address, val);
    EXPECT_EQ(old, 100);
    EXPECT_EQ(address, 50);
}

TEST_F(AtomicExchTestsuite, AtomicExchUint64TestCase)
{
    uint64_t address = 100;
    uint64_t val = 50;
    uint64_t old = asc_atomic_exch(&address, val);
    EXPECT_EQ(old, 100);
    EXPECT_EQ(address, 50);
}

TEST_F(AtomicExchTestsuite, AtomicExchFloatTestCase)
{
    float address = 100.0f;
    float val = 50.0f;
    float old = asc_atomic_exch(&address, val);
    EXPECT_FLOAT_EQ(old, 100.0f);
    EXPECT_FLOAT_EQ(address, 50.0f);
}

class AtomicMaxTestsuite : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(AtomicMaxTestsuite, AtomicMaxInt32TestCase)
{
    int32_t address = 100;
    int32_t val = 150;
    int32_t old = asc_atomic_max(&address, val);
    EXPECT_EQ(old, 100);
    EXPECT_EQ(address, 150);
}

TEST_F(AtomicMaxTestsuite, AtomicMaxUint32TestCase)
{
    uint32_t address = 100;
    uint32_t val = 150;
    uint32_t old = asc_atomic_max(&address, val);
    EXPECT_EQ(old, 100);
    EXPECT_EQ(address, 150);
}

TEST_F(AtomicMaxTestsuite, AtomicMaxInt64TestCase)
{
    int64_t address = 100;
    int64_t val = 150;
    int64_t old = asc_atomic_max(&address, val);
    EXPECT_EQ(old, 100);
    EXPECT_EQ(address, 150);
}

TEST_F(AtomicMaxTestsuite, AtomicMaxUint64TestCase)
{
    uint64_t address = 100;
    uint64_t val = 150;
    uint64_t old = asc_atomic_max(&address, val);
    EXPECT_EQ(old, 100);
    EXPECT_EQ(address, 150);
}

TEST_F(AtomicMaxTestsuite, AtomicMaxFloatTestCase)
{
    float address = 100.0f;
    float val = 150.0f;
    float old = asc_atomic_max(&address, val);
    EXPECT_FLOAT_EQ(old, 100.0f);
    EXPECT_FLOAT_EQ(address, 150.0f);
}

class AtomicMinTestsuite : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(AtomicMinTestsuite, AtomicMinInt32TestCase)
{
    int32_t address = 100;
    int32_t val = 50;
    int32_t old = asc_atomic_min(&address, val);
    EXPECT_EQ(old, 100);
    EXPECT_EQ(address, 50);
}

TEST_F(AtomicMinTestsuite, AtomicMinUint32TestCase)
{
    uint32_t address = 100;
    uint32_t val = 50;
    uint32_t old = asc_atomic_min(&address, val);
    EXPECT_EQ(old, 100);
    EXPECT_EQ(address, 50);
}

TEST_F(AtomicMinTestsuite, AtomicMinInt64TestCase)
{
    int64_t address = 100;
    int64_t val = 50;
    int64_t old = asc_atomic_min(&address, val);
    EXPECT_EQ(old, 100);
    EXPECT_EQ(address, 50);
}

TEST_F(AtomicMinTestsuite, AtomicMinUint64TestCase)
{
    uint64_t address = 100;
    uint64_t val = 50;
    uint64_t old = asc_atomic_min(&address, val);
    EXPECT_EQ(old, 100);
    EXPECT_EQ(address, 50);
}

TEST_F(AtomicMinTestsuite, AtomicMinFloatTestCase)
{
    float address = 100.0f;
    float val = 50.0f;
    float old = asc_atomic_min(&address, val);
    EXPECT_FLOAT_EQ(old, 100.0f);
    EXPECT_FLOAT_EQ(address, 50.0f);
}

class AtomicIncTestsuite : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(AtomicIncTestsuite, AtomicIncUint32TestCase)
{
    uint32_t address = 50;
    uint32_t val = 100;
    uint32_t old = asc_atomic_inc(&address, val);
    EXPECT_EQ(old, 50);
    EXPECT_EQ(address, 51);
}

TEST_F(AtomicIncTestsuite, AtomicIncUint64TestCase)
{
    uint64_t address = 50;
    uint64_t val = 100;
    uint64_t old = asc_atomic_inc(&address, val);
    EXPECT_EQ(old, 50);
    EXPECT_EQ(address, 51);
}

class AtomicDecTestsuite : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(AtomicDecTestsuite, AtomicDecUint32TestCase)
{
    uint32_t address = 50;
    uint32_t val = 100;
    uint32_t old = asc_atomic_dec(&address, val);
    EXPECT_EQ(old, 50);
    EXPECT_EQ(address, 49);
}

TEST_F(AtomicDecTestsuite, AtomicDecUint64TestCase)
{
    uint64_t address = 50;
    uint64_t val = 100;
    uint64_t old = asc_atomic_dec(&address, val);
    EXPECT_EQ(old, 50);
    EXPECT_EQ(address, 49);
}

class AtomicCasTestsuite : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(AtomicCasTestsuite, AtomicCasInt32MatchTestCase)
{
    int32_t address = 100;
    int32_t compare = 100;
    int32_t val = 50;
    int32_t old = asc_atomic_cas(&address, compare, val);
    EXPECT_EQ(old, 100);
    EXPECT_EQ(address, 50);
}

TEST_F(AtomicCasTestsuite, AtomicCasInt32NotMatchTestCase)
{
    int32_t address = 100;
    int32_t compare = 99;
    int32_t val = 50;
    int32_t old = asc_atomic_cas(&address, compare, val);
    EXPECT_EQ(old, 100);
    EXPECT_EQ(address, 100);
}

TEST_F(AtomicCasTestsuite, AtomicCasUint32MatchTestCase)
{
    uint32_t address = 100;
    uint32_t compare = 100;
    uint32_t val = 50;
    uint32_t old = asc_atomic_cas(&address, compare, val);
    EXPECT_EQ(old, 100);
    EXPECT_EQ(address, 50);
}

TEST_F(AtomicCasTestsuite, AtomicCasUint64MatchTestCase)
{
    uint64_t address = 100;
    uint64_t compare = 100;
    uint64_t val = 50;
    uint64_t old = asc_atomic_cas(&address, compare, val);
    EXPECT_EQ(old, 100);
    EXPECT_EQ(address, 50);
}

TEST_F(AtomicCasTestsuite, AtomicCasInt64MatchTestCase)
{
    int64_t address = 100;
    int64_t compare = 100;
    int64_t val = 50;
    int64_t old = asc_atomic_cas(&address, compare, val);
    EXPECT_EQ(old, 100);
    EXPECT_EQ(address, 50);
}

TEST_F(AtomicCasTestsuite, AtomicCasFloatMatchTestCase)
{
    float address = 100.0f;
    float compare = 100.0f;
    float val = 50.0f;
    float old = asc_atomic_cas(&address, compare, val);
    EXPECT_FLOAT_EQ(old, 100.0f);
    EXPECT_FLOAT_EQ(address, 50.0f);
}

class AtomicAndTestsuite : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(AtomicAndTestsuite, AtomicAndInt32TestCase)
{
    int32_t address = 0xFF;
    int32_t val = 0x0F;
    int32_t old = asc_atomic_and(&address, val);
    EXPECT_EQ(old, 0xFF);
    EXPECT_EQ(address, 0x0F);
}

TEST_F(AtomicAndTestsuite, AtomicAndUint32TestCase)
{
    uint32_t address = 0xFF;
    uint32_t val = 0x0F;
    uint32_t old = asc_atomic_and(&address, val);
    EXPECT_EQ(old, 0xFF);
    EXPECT_EQ(address, 0x0F);
}

TEST_F(AtomicAndTestsuite, AtomicAndInt64TestCase)
{
    int64_t address = 0xFF;
    int64_t val = 0x0F;
    int64_t old = asc_atomic_and(&address, val);
    EXPECT_EQ(old, 0xFF);
    EXPECT_EQ(address, 0x0F);
}

TEST_F(AtomicAndTestsuite, AtomicAndUint64TestCase)
{
    uint64_t address = 0xFF;
    uint64_t val = 0x0F;
    uint64_t old = asc_atomic_and(&address, val);
    EXPECT_EQ(old, 0xFF);
    EXPECT_EQ(address, 0x0F);
}

class AtomicOrTestsuite : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(AtomicOrTestsuite, AtomicOrInt32TestCase)
{
    int32_t address = 0xF0;
    int32_t val = 0x0F;
    int32_t old = asc_atomic_or(&address, val);
    EXPECT_EQ(old, 0xF0);
    EXPECT_EQ(address, 0xFF);
}

TEST_F(AtomicOrTestsuite, AtomicOrUint32TestCase)
{
    uint32_t address = 0xF0;
    uint32_t val = 0x0F;
    uint32_t old = asc_atomic_or(&address, val);
    EXPECT_EQ(old, 0xF0);
    EXPECT_EQ(address, 0xFF);
}

TEST_F(AtomicOrTestsuite, AtomicOrInt64TestCase)
{
    int64_t address = 0xF0;
    int64_t val = 0x0F;
    int64_t old = asc_atomic_or(&address, val);
    EXPECT_EQ(old, 0xF0);
    EXPECT_EQ(address, 0xFF);
}

TEST_F(AtomicOrTestsuite, AtomicOrUint64TestCase)
{
    uint64_t address = 0xF0;
    uint64_t val = 0x0F;
    uint64_t old = asc_atomic_or(&address, val);
    EXPECT_EQ(old, 0xF0);
    EXPECT_EQ(address, 0xFF);
}

class AtomicXorTestsuite : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(AtomicXorTestsuite, AtomicXorInt32TestCase)
{
    int32_t address = 0xFF;
    int32_t val = 0xF0;
    int32_t old = asc_atomic_xor(&address, val);
    EXPECT_EQ(old, 0xFF);
    EXPECT_EQ(address, 0x0F);
}

TEST_F(AtomicXorTestsuite, AtomicXorUint32TestCase)
{
    uint32_t address = 0xFF;
    uint32_t val = 0xF0;
    uint32_t old = asc_atomic_xor(&address, val);
    EXPECT_EQ(old, 0xFF);
    EXPECT_EQ(address, 0x0F);
}

TEST_F(AtomicXorTestsuite, AtomicXorInt64TestCase)
{
    int64_t address = 0xFF;
    int64_t val = 0xF0;
    int64_t old = asc_atomic_xor(&address, val);
    EXPECT_EQ(old, 0xFF);
    EXPECT_EQ(address, 0x0F);
}

TEST_F(AtomicXorTestsuite, AtomicXorUint64TestCase)
{
    uint64_t address = 0xFF;
    uint64_t val = 0xF0;
    uint64_t old = asc_atomic_xor(&address, val);
    EXPECT_EQ(old, 0xFF);
    EXPECT_EQ(address, 0x0F);
}

class AtomicAddHalfTestsuite : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(AtomicAddHalfTestsuite, AtomicAddHalfNormalTestCase)
{
    half address = half(100.0f);
    half val = half(50.0f);
    asc_atomic_add(&address, val);
    EXPECT_FLOAT_EQ((float)address, 150.0f);
}

TEST_F(AtomicAddHalfTestsuite, AtomicAddHalfZeroTestCase)
{
    half address = half(0.0f);
    half val = half(0.0f);
    asc_atomic_add(&address, val);
    EXPECT_FLOAT_EQ((float)address, 0.0f);
}

TEST_F(AtomicAddHalfTestsuite, AtomicAddHalfSubnormalTestCase)
{
    half address = half(5.960464477539063e-08f);
    half val = half(5.960464477539063e-08f);
    asc_atomic_add(&address, val);
    EXPECT_FLOAT_EQ((float)address, 1.1920928955078125e-07f);
}

class AtomicSubHalfTestsuite : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(AtomicSubHalfTestsuite, AtomicSubHalfNormalTestCase)
{
    half address = half(100.0f);
    half val = half(50.0f);
    asc_atomic_sub(&address, val);
    EXPECT_FLOAT_EQ((float)address, 50.0f);
}

TEST_F(AtomicSubHalfTestsuite, AtomicSubHalfZeroTestCase)
{
    half address = half(0.0f);
    half val = half(0.0f);
    asc_atomic_sub(&address, val);
    EXPECT_FLOAT_EQ((float)address, 0.0f);
}

TEST_F(AtomicSubHalfTestsuite, AtomicSubHalfResultZeroTestCase)
{
    half address = half(50.0f);
    half val = half(50.0f);
    asc_atomic_sub(&address, val);
    EXPECT_FLOAT_EQ((float)address, 0.0f);
}

TEST_F(AtomicSubHalfTestsuite, AtomicSubHalfSubnormalTestCase)
{
    half address = half(1.1920928955078125e-07f);
    half val = half(5.960464477539063e-08f);
    asc_atomic_sub(&address, val);
    EXPECT_FLOAT_EQ((float)address, 5.960464477539063e-08f);
}

class AtomicAddBf16Testsuite : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(AtomicAddBf16Testsuite, AtomicAddBf16NormalTestCase)
{
    bfloat16_t address = bfloat16_t(100.0f);
    bfloat16_t val = bfloat16_t(50.0f);
    asc_atomic_add(&address, val);
    EXPECT_FLOAT_EQ((float)address, 150.0f);
}

TEST_F(AtomicAddBf16Testsuite, AtomicAddBf16ZeroTestCase)
{
    bfloat16_t address = bfloat16_t(0.0f);
    bfloat16_t val = bfloat16_t(0.0f);
    asc_atomic_add(&address, val);
    EXPECT_FLOAT_EQ((float)address, 0.0f);
}

TEST_F(AtomicAddBf16Testsuite, AtomicAddBf16NaNTestCase)
{
    bfloat16_t address = bfloat16_t(NAN);
    bfloat16_t val = bfloat16_t(1.0f);
    asc_atomic_add(&address, val);
    EXPECT_TRUE(std::isnan((float)address));
}

TEST_F(AtomicAddBf16Testsuite, AtomicAddBf16OverflowTestCase)
{
    bfloat16_t address = bfloat16_t(3.389e38f);
    bfloat16_t val = bfloat16_t(3.389e38f);
    asc_atomic_add(&address, val);
    EXPECT_TRUE(std::isinf((float)address));
}

class AtomicSubBf16Testsuite : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(AtomicSubBf16Testsuite, AtomicSubBf16NormalTestCase)
{
    bfloat16_t address = bfloat16_t(100.0f);
    bfloat16_t val = bfloat16_t(50.0f);
    asc_atomic_sub(&address, val);
    EXPECT_FLOAT_EQ((float)address, 50.0f);
}

TEST_F(AtomicSubBf16Testsuite, AtomicSubBf16ZeroTestCase)
{
    bfloat16_t address = bfloat16_t(0.0f);
    bfloat16_t val = bfloat16_t(0.0f);
    asc_atomic_sub(&address, val);
    EXPECT_FLOAT_EQ((float)address, 0.0f);
}

TEST_F(AtomicSubBf16Testsuite, AtomicSubBf16ResultZeroTestCase)
{
    bfloat16_t address = bfloat16_t(50.0f);
    bfloat16_t val = bfloat16_t(50.0f);
    asc_atomic_sub(&address, val);
    EXPECT_FLOAT_EQ((float)address, 0.0f);
}

TEST_F(AtomicSubBf16Testsuite, AtomicSubBf16NaNTestCase)
{
    bfloat16_t address = bfloat16_t(NAN);
    bfloat16_t val = bfloat16_t(1.0f);
    asc_atomic_sub(&address, val);
    EXPECT_TRUE(std::isnan((float)address));
}

TEST_F(AtomicSubBf16Testsuite, AtomicSubBf16OverflowTestCase)
{
    bfloat16_t address = bfloat16_t(-3.389e38f);
    bfloat16_t val = bfloat16_t(3.389e38f);
    asc_atomic_sub(&address, val);
    EXPECT_TRUE(std::isinf((float)address));
    EXPECT_LT((float)address, 0.0f);
}
