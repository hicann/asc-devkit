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
#include "kernel_operator.h"
// #include "model/model_factory.h"
#include "stub_def.h"
#include "stub_fun.h"

using namespace std;
using namespace AscendC;

class TestScalarInstr : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

TEST_F(TestScalarInstr, ScalarInstrCase)
{
    uint64_t u64XnVal = 28;
    int64_t s64XnVal = 28;
    EXPECT_EQ(ScalarGetCountOfValue<0>(u64XnVal), 61);
    EXPECT_EQ(ScalarGetCountOfValue<1>(u64XnVal), 3);
    EXPECT_EQ(ScalarCountLeadingZero(u64XnVal), 59);
    EXPECT_EQ(CountBitsCntSameAsSignBit(s64XnVal), 58);
    EXPECT_EQ(ScalarGetSFFValue<0>(u64XnVal), 0);
    EXPECT_EQ(ScalarGetSFFValue<1>(u64XnVal), 2);

    float convXnVal = 2.5;
    half f16XdVal = convXnVal;

    float maxOverFlowXnVal = 2147483648;
    float minOverFlowXnVal = -__FLT_MAX__;
    float underFlowXnVal = 0.0000000061;

    half f16MaxVal = maxOverFlowXnVal;
    half f16MinVal = minOverFlowXnVal;
    constexpr int64_t s32Max = 2147483647;
    constexpr int64_t s32Min = -2147483648;
    set_ctrl(sbitset0(get_ctrl(), 59));

    half f16Res = ScalarCast<float, half, RoundMode::CAST_ODD>(convXnVal);
    EXPECT_EQ(f16Res, f16XdVal);
    int32_t s32Res = ScalarCast<float, int32_t, RoundMode::CAST_ROUND>(convXnVal);
    EXPECT_EQ(s32Res, 3);
    s32Res = ScalarCast<float, int32_t, RoundMode::CAST_CEIL>(convXnVal);
    EXPECT_EQ(s32Res, 3);
    s32Res = ScalarCast<float, int32_t, RoundMode::CAST_FLOOR>(convXnVal);
    EXPECT_EQ(s32Res, 2);
    s32Res = ScalarCast<float, int32_t, RoundMode::CAST_RINT>(convXnVal);
    EXPECT_EQ(s32Res, 2);

    half maxf16Res = ScalarCast<float, half, RoundMode::CAST_ODD>(maxOverFlowXnVal);
    EXPECT_EQ(maxf16Res, f16MaxVal);
    s32Res = ScalarCast<float, int32_t, RoundMode::CAST_ROUND>(maxOverFlowXnVal);
    EXPECT_EQ(s32Res, s32Max);
    s32Res = ScalarCast<float, int32_t, RoundMode::CAST_CEIL>(maxOverFlowXnVal);
    EXPECT_EQ(s32Res, s32Max);
    s32Res = ScalarCast<float, int32_t, RoundMode::CAST_FLOOR>(maxOverFlowXnVal);
    EXPECT_EQ(s32Res, s32Max);
    s32Res = ScalarCast<float, int32_t, RoundMode::CAST_RINT>(maxOverFlowXnVal);
    EXPECT_EQ(s32Res, s32Max);

    half minf16Res = ScalarCast<float, half, RoundMode::CAST_ODD>(minOverFlowXnVal);
    EXPECT_EQ(minf16Res, f16MinVal);
    s32Res = ScalarCast<float, int32_t, RoundMode::CAST_ROUND>(minOverFlowXnVal);
    EXPECT_EQ(s32Res, s32Min);
    s32Res = ScalarCast<float, int32_t, RoundMode::CAST_CEIL>(minOverFlowXnVal);
    EXPECT_EQ(s32Res, s32Min);
    s32Res = ScalarCast<float, int32_t, RoundMode::CAST_FLOOR>(minOverFlowXnVal);
    EXPECT_EQ(s32Res, s32Min);
    s32Res = ScalarCast<float, int32_t, RoundMode::CAST_RINT>(minOverFlowXnVal);
    EXPECT_EQ(s32Res, s32Min);

    set_ctrl(sbitset1(get_ctrl(), 59));
    s32Res = ScalarCast<float, int32_t, RoundMode::CAST_ROUND>(minOverFlowXnVal);
    EXPECT_EQ(s32Res, s32Min);
    s32Res = ScalarCast<float, int32_t, RoundMode::CAST_CEIL>(minOverFlowXnVal);
    EXPECT_EQ(s32Res, s32Min);
    s32Res = ScalarCast<float, int32_t, RoundMode::CAST_FLOOR>(minOverFlowXnVal);
    EXPECT_EQ(s32Res, s32Min);
    s32Res = ScalarCast<float, int32_t, RoundMode::CAST_RINT>(minOverFlowXnVal);
    EXPECT_EQ(s32Res, s32Min);

    f16Res = ScalarCast<float, half, RoundMode::CAST_ODD>(underFlowXnVal);
    EXPECT_EQ(f16Res, static_cast<half>(0));

    half hScale = 1.0;
    SetDeqScale(hScale);

    float fScale = 1.0;
    int16_t offset = 0;
    bool signMode = true;
    SetDeqScale(fScale, offset, signMode);

    TPipe tpipe;
    TBuf<TPosition::VECCALC> tbuf;
    tpipe.InitBuffer(tbuf, 64 * sizeof(uint64_t));
    LocalTensor<uint64_t> tmpBuffer = tbuf.Get<uint64_t>();
    float vdeqScale[16] = {0};
    int16_t vdeqOffset[16] = {0};
    bool vdeqSignMode[16] = {0};
    for (int i = 0; i < 16; i++) {
        vdeqScale[i] = 1.0;
        vdeqOffset[i] = 0;
        vdeqSignMode[i] = true;
    }
    VdeqInfo vdeqInfo(vdeqScale, vdeqOffset, vdeqSignMode);
    SetDeqScale<uint64_t>(tmpBuffer, vdeqInfo);

    SetStoreAtomicConfig<AtomicDtype::ATOMIC_F16, AtomicOp::ATOMIC_SUM>();
    uint16_t type = 1;
    uint16_t op = 1;
    GetStoreAtomicConfig(type, op);
    EXPECT_EQ(type, 0);
    EXPECT_EQ(op, 0);
    int64_t stAtomicCfg = GetStoreAtomicConfig();
    EXPECT_EQ(stAtomicCfg, 0);

    SetHF32Mode(true);
    SetHF32TransMode(true);
    SetMMLayoutTransform(true);

    set_cond(u64XnVal);
    EXPECT_EQ(get_cond(), 0);
    set_condition_flag(u64XnVal);
    EXPECT_EQ(get_condition_flag(), 0);
}

TEST_F(TestScalarInstr, ScalarInstrNewCase)
{
    uint64_t u64XnVal = 28;
    int64_t s64XnVal = 28;
    EXPECT_EQ(ScalarGetCountOfValue<0>(u64XnVal), 61);
    EXPECT_EQ(ScalarGetCountOfValue<1>(u64XnVal), 3);
    EXPECT_EQ(ScalarCountLeadingZero(u64XnVal), 59);
    EXPECT_EQ(CountBitsCntSameAsSignBit(s64XnVal), 58);
    EXPECT_EQ(ScalarGetSFFValue<0>(u64XnVal), 0);
    EXPECT_EQ(ScalarGetSFFValue<1>(u64XnVal), 2);

    float convXnVal = 2.5;
    half f16XdVal = convXnVal;

    float maxOverFlowXnVal = 2147483648;
    float minOverFlowXnVal = -__FLT_MAX__;
    float underFlowXnVal = 0.0000000061;

    half f16MaxVal = maxOverFlowXnVal;
    half f16MinVal = minOverFlowXnVal;
    constexpr int64_t s32Max = 2147483647;
    constexpr int64_t s32Min = -2147483648;
    set_ctrl(sbitset0(get_ctrl(), 59));

    half f16Res = Cast<float, half, RoundMode::CAST_ODD>(convXnVal);
    EXPECT_EQ(f16Res, f16XdVal);
    int32_t s32Res = Cast<float, int32_t, RoundMode::CAST_ROUND>(convXnVal);
    EXPECT_EQ(s32Res, 3);
    s32Res = Cast<float, int32_t, RoundMode::CAST_CEIL>(convXnVal);
    EXPECT_EQ(s32Res, 3);
    s32Res = Cast<float, int32_t, RoundMode::CAST_FLOOR>(convXnVal);
    EXPECT_EQ(s32Res, 2);
    s32Res = Cast<float, int32_t, RoundMode::CAST_RINT>(convXnVal);
    EXPECT_EQ(s32Res, 2);

    half maxf16Res = Cast<float, half, RoundMode::CAST_ODD>(maxOverFlowXnVal);
    EXPECT_EQ(maxf16Res, f16MaxVal);
    s32Res = Cast<float, int32_t, RoundMode::CAST_ROUND>(maxOverFlowXnVal);
    EXPECT_EQ(s32Res, s32Max);
    s32Res = Cast<float, int32_t, RoundMode::CAST_CEIL>(maxOverFlowXnVal);
    EXPECT_EQ(s32Res, s32Max);
    s32Res = Cast<float, int32_t, RoundMode::CAST_FLOOR>(maxOverFlowXnVal);
    EXPECT_EQ(s32Res, s32Max);
    s32Res = Cast<float, int32_t, RoundMode::CAST_RINT>(maxOverFlowXnVal);
    EXPECT_EQ(s32Res, s32Max);

    half minf16Res = Cast<float, half, RoundMode::CAST_ODD>(minOverFlowXnVal);
    EXPECT_EQ(minf16Res, f16MinVal);
    s32Res = Cast<float, int32_t, RoundMode::CAST_ROUND>(minOverFlowXnVal);
    EXPECT_EQ(s32Res, s32Min);
    s32Res = Cast<float, int32_t, RoundMode::CAST_CEIL>(minOverFlowXnVal);
    EXPECT_EQ(s32Res, s32Min);
    s32Res = Cast<float, int32_t, RoundMode::CAST_FLOOR>(minOverFlowXnVal);
    EXPECT_EQ(s32Res, s32Min);
    s32Res = Cast<float, int32_t, RoundMode::CAST_RINT>(minOverFlowXnVal);
    EXPECT_EQ(s32Res, s32Min);

    set_ctrl(sbitset1(get_ctrl(), 59));
    s32Res = Cast<float, int32_t, RoundMode::CAST_ROUND>(minOverFlowXnVal);
    EXPECT_EQ(s32Res, s32Min);
    s32Res = Cast<float, int32_t, RoundMode::CAST_CEIL>(minOverFlowXnVal);
    EXPECT_EQ(s32Res, s32Min);
    s32Res = Cast<float, int32_t, RoundMode::CAST_FLOOR>(minOverFlowXnVal);
    EXPECT_EQ(s32Res, s32Min);
    s32Res = Cast<float, int32_t, RoundMode::CAST_RINT>(minOverFlowXnVal);
    EXPECT_EQ(s32Res, s32Min);

    f16Res = Cast<float, half, RoundMode::CAST_ODD>(underFlowXnVal);
    EXPECT_EQ(f16Res, static_cast<half>(0));

    half hScale = 1.0;
    SetDeqScale(hScale);

    float fScale = 1.0;
    int16_t offset = 0;
    bool signMode = true;
    SetDeqScale(fScale, offset, signMode);

    TPipe tpipe;
    TBuf<TPosition::VECCALC> tbuf;
    tpipe.InitBuffer(tbuf, 64 * sizeof(uint64_t));
    LocalTensor<uint64_t> tmpBuffer = tbuf.Get<uint64_t>();
    float vdeqScale[16] = {0};
    int16_t vdeqOffset[16] = {0};
    bool vdeqSignMode[16] = {0};
    for (int i = 0; i < 16; i++) {
        vdeqScale[i] = 1.0;
        vdeqOffset[i] = 0;
        vdeqSignMode[i] = true;
    }
    VdeqInfo vdeqInfo(vdeqScale, vdeqOffset, vdeqSignMode);
    SetDeqScale<uint64_t>(tmpBuffer, vdeqInfo);

    SetStoreAtomicConfig<AtomicDtype::ATOMIC_F16, AtomicOp::ATOMIC_SUM>();
    uint16_t type = 1;
    uint16_t op = 1;
    GetStoreAtomicConfig(type, op);
    EXPECT_EQ(type, 0);
    EXPECT_EQ(op, 0);
    int64_t stAtomicCfg = GetStoreAtomicConfig();
    EXPECT_EQ(stAtomicCfg, 0);

    SetHF32Mode(AscendC::HF32Mode::ENABLE);
    SetHF32TransMode(AscendC::HF32TransMode::NEAREST_AWAY);
    SetMMRowMajor();

    set_cond(u64XnVal);
    EXPECT_EQ(get_cond(), 0);
    set_condition_flag(u64XnVal);
    EXPECT_EQ(get_condition_flag(), 0);
}
