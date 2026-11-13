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
#include "kernel_operator.h"
#include "mockcpp/mockcpp.hpp"

using namespace AscendC;

class DataCopyL0C2UBTest : public testing::Test {
protected:
    void SetUp() override
    {
        SetGCoreType(1);
        BufIdTracker::GetInstance().Reset();
        savedNdNzParam = g_fixpipeNdNzParam;
        savedNz2dnParams = Internal::GetFixpipeNz2dnParams();
        Internal::GetFixpipeNz2dnParams() = Nz2DnParams();
        g_fixpipeNdNzParam = 0;
    }

    void TearDown() override
    {
        GlobalMockObject::verify();
        KernelRaise::GetInstance().SetRaiseMode(true);
        g_fixpipeNdNzParam = savedNdNzParam;
        Internal::GetFixpipeNz2dnParams() = savedNz2dnParams;
        SetGCoreType(0);
    }

    template <typename DstT, typename SrcT, TPosition dstPosition = TPosition::VECCALC>
    void Copy(QuantMode_t quantMode, bool nz2ndEn = false, uint8_t dualDstCtl = 0, bool subBlockId = false)
    {
        DataCopyCO12DstParams params(
            32, 16, nz2ndEn ? 32 : 32 * 16 * sizeof(DstT) / ONE_BLK_SIZE, 16, quantMode, 0, false, nz2ndEn, dualDstCtl,
            subBlockId);
        CopyParams<DstT, SrcT, dstPosition>(params);
    }

    template <typename DstT, typename SrcT, TPosition dstPosition = TPosition::VECCALC>
    void CopyParams(const DataCopyCO12DstParams& params)
    {
        TPipe pipe;
        TBuf<TPosition::CO1> srcBuffer;
        TBuf<dstPosition> dstBuffer;
        pipe.InitBuffer(srcBuffer, 4096);
        pipe.InitBuffer(dstBuffer, 4096);
        LocalTensor<SrcT> src = srcBuffer.template Get<SrcT>();
        LocalTensor<DstT> dst = dstBuffer.template Get<DstT>();
        DataCopy(dst, src, params);
    }

    uint64_t savedNdNzParam = 0;
    Nz2DnParams savedNz2dnParams;
};

TEST_F(DataCopyL0C2UBTest, ParameterDefaultsAndConstructors)
{
    DataCopyCO12DstParams defaults;
    EXPECT_EQ(defaults.dualDstCtl, 0);
    EXPECT_FALSE(defaults.subBlockId);
    EXPECT_FALSE(defaults.nz2dnEn);
    DataCopyCO12DstParams legacy(32, 16, 32, 16, QuantMode_t::NoQuant, 0, false, true);
    EXPECT_EQ(legacy.dualDstCtl, 0);
    EXPECT_FALSE(legacy.subBlockId);
    DataCopyCO12DstParams extended(32, 16, 16, 16, QuantMode_t::NoQuant, 0, false, true, 2, true);
    EXPECT_EQ(extended.dualDstCtl, 2);
    EXPECT_TRUE(extended.subBlockId);
    EXPECT_FALSE(extended.nz2dnEn);
    DataCopyCO12DstParams dn(32, 16, 16, 16, QuantMode_t::NoQuant, 0, false, false, 0, true, true);
    EXPECT_TRUE(dn.nz2dnEn);
    EXPECT_TRUE(dn.subBlockId);
}

TEST_F(DataCopyL0C2UBTest, UbPositionsAndDualDestinationModes)
{
    Copy<float, float, TPosition::VECIN>(QuantMode_t::NoQuant);
    Copy<float, float, TPosition::VECOUT>(QuantMode_t::NoQuant, true);
    for (uint8_t dualMode = 0; dualMode < 3; ++dualMode) {
        Copy<float, float>(QuantMode_t::NoQuant, false, dualMode);
        Copy<float, float>(QuantMode_t::NoQuant, true, dualMode, true);
    }
}

TEST_F(DataCopyL0C2UBTest, FixpipeDataTypesAndQuantizationModes)
{
    Copy<int32_t, int32_t>(QuantMode_t::NoQuant);
    Copy<half, float>(QuantMode_t::F322F16);
    Copy<bfloat16_t, float>(QuantMode_t::F322BF16);
    Copy<half, int32_t>(QuantMode_t::DEQF16);
    Copy<half, int32_t>(QuantMode_t::VDEQF16);
    Copy<int8_t, float>(QuantMode_t::QF322B8_PRE);
    Copy<uint8_t, float>(QuantMode_t::VQF322B8_PRE);
    Copy<int8_t, int32_t>(QuantMode_t::REQ8);
    Copy<uint8_t, int32_t>(QuantMode_t::VREQ8);
    Copy<half, float>(QuantMode_t::QF322F16_PRE);
    Copy<half, float>(QuantMode_t::VQF322F16_PRE);
    Copy<bfloat16_t, float>(QuantMode_t::QF322BF16_PRE);
    Copy<bfloat16_t, float>(QuantMode_t::VQF322BF16_PRE);
    Copy<fp8_e4m3fn_t, float>(QuantMode_t::QF322FP8_PRE);
    Copy<fp8_e4m3fn_t, float>(QuantMode_t::VQF322FP8_PRE);
    Copy<hifloat8_t, float>(QuantMode_t::QF322HIF8_PRE);
    Copy<hifloat8_t, float>(QuantMode_t::VQF322HIF8_PRE);
    Copy<hifloat8_t, float>(QuantMode_t::QF322HIF8_PRE_HYBRID);
    Copy<hifloat8_t, float>(QuantMode_t::VQF322HIF8_PRE_HYBRID);
    Copy<bfloat16_t, int32_t>(QuantMode_t::QS322BF16_PRE);
    Copy<bfloat16_t, int32_t>(QuantMode_t::VQS322BF16_PRE);
    Copy<float, float>(QuantMode_t::QF322F32_PRE);
    Copy<float, float>(QuantMode_t::VQF322F32_PRE);
}

TEST_F(DataCopyL0C2UBTest, InvalidDualDestinationParameters)
{
    KernelRaise::GetInstance().SetRaiseMode(false);
    const uint64_t initialRaiseCount = KernelRaise::GetInstance().GetRaiseCount();
    Copy<float, float>(QuantMode_t::NoQuant, false, 3);
    Copy<half, float>(QuantMode_t::F322F16, false, 1);
    DataCopyCO12DstParams params(32, 15, 64, 16, QuantMode_t::NoQuant, 0, false, false, 1);
    CopyParams<float, float>(params);
    params.mSize = 16;
    params.nSize = 16;
    params.dualDstCtl = 2;
    CopyParams<float, float>(params);
    params.dualDstCtl = 0;
    params.channelSplit = true;
    CopyParams<float, float>(params);
    EXPECT_EQ(KernelRaise::GetInstance().GetRaiseCount() - initialRaiseCount, 5);
}

TEST_F(DataCopyL0C2UBTest, LegacyL1Destination)
{
    Copy<float, float, TPosition::A1>(QuantMode_t::NoQuant);
    Copy<half, float, TPosition::A1>(QuantMode_t::F322F16);
}

TEST_F(DataCopyL0C2UBTest, DestinationOverflowUsesPerUbShape)
{
    DataCopyCO12DstParams params(64, 32, 64, 32, QuantMode_t::NoQuant, 0, false, true);
    uint64_t srcOffset = 0;
    uint64_t dstOffset = 0;
    check::CalculateDataCopyMaxOffset<float, float>(Hardware::L0C, Hardware::UB, params, srcOffset, dstOffset);
    EXPECT_EQ(srcOffset, 8192);
    EXPECT_EQ(dstOffset, 8192);
    params.dualDstCtl = 1;
    check::CalculateDataCopyMaxOffset<float, float>(Hardware::L0C, Hardware::UB, params, srcOffset, dstOffset);
    EXPECT_EQ(srcOffset, 8192);
    EXPECT_EQ(dstOffset, 4096);
    params.dualDstCtl = 2;
    params.dstStride = 32;
    check::CalculateDataCopyMaxOffset<float, float>(Hardware::L0C, Hardware::UB, params, srcOffset, dstOffset);
    EXPECT_EQ(dstOffset, 4096);
    params.nz2ndEn = false;
    params.dstStride = 64;
    check::CalculateDataCopyMaxOffset<float, float>(Hardware::L0C, Hardware::UB, params, srcOffset, dstOffset);
    EXPECT_EQ(dstOffset, 4096);
}

TEST_F(DataCopyL0C2UBTest, Nz2NdMatrixStrides)
{
    DataCopyCO12DstParams params(64, 32, 64, 32, QuantMode_t::NoQuant, 0, false, true);
    g_fixpipeNdNzParam = 2 | (static_cast<uint64_t>(128) << 16) | (static_cast<uint64_t>(2048) << 32);
    uint64_t srcOffset = 0;
    uint64_t dstOffset = 0;
    check::CalculateDataCopyMaxOffset<float, float>(Hardware::L0C, Hardware::UB, params, srcOffset, dstOffset);
    EXPECT_EQ(srcOffset, 16384);
    EXPECT_EQ(dstOffset, 16384);
}

TEST_F(DataCopyL0C2UBTest, NzByteOutputOverflowUses32ElementFractals)
{
    DataCopyCO12DstParams params(64, 32, 32, 32, QuantMode_t::QF322B8_PRE, 0, false, false);
    uint64_t srcOffset = 0;
    uint64_t dstOffset = 0;
    check::CalculateDataCopyMaxOffset<float, int8_t>(Hardware::L0C, Hardware::UB, params, srcOffset, dstOffset);
    EXPECT_EQ(srcOffset, 8192);
    EXPECT_EQ(dstOffset, 2048);
    check::CalculateDataCopyMaxOffset<float, uint8_t>(Hardware::L0C, Hardware::UB, params, srcOffset, dstOffset);
    EXPECT_EQ(dstOffset, 2048);
    params.quantPre = QuantMode_t::QF322FP8_PRE;
    check::CalculateDataCopyMaxOffset<float, fp8_e4m3fn_t>(Hardware::L0C, Hardware::UB, params, srcOffset, dstOffset);
    EXPECT_EQ(dstOffset, 2048);
    params.quantPre = QuantMode_t::QF322HIF8_PRE;
    params.nSize = 96;
    check::CalculateDataCopyMaxOffset<float, hifloat8_t>(Hardware::L0C, Hardware::UB, params, srcOffset, dstOffset);
    EXPECT_EQ(dstOffset, 3072);
}

TEST_F(DataCopyL0C2UBTest, Nz2DnConfigurationAndCopies)
{
    SetFixpipeNz2dnFlag(2, 240, 2880, 2);
    const auto& config = Internal::GetFixpipeNz2dnParams();
    EXPECT_EQ(config.dnNum, 2);
    EXPECT_EQ(config.srcNzMatrixStride, 240);
    EXPECT_EQ(config.dstDnMatrixStride, 2880);
    EXPECT_EQ(config.srcNzC0Stride, 2);
    SetFixpipeNz2dnFlag(1, 0, 0, 1);
    DataCopyCO12DstParams params(32, 16, 16, 16, QuantMode_t::NoQuant, 0, false, false, 0, false, true);
    CopyParams<float, float>(params);
    params.quantPre = QuantMode_t::F322F16;
    params.subBlockId = true;
    CopyParams<half, float>(params);
    params.quantPre = QuantMode_t::F322BF16;
    CopyParams<bfloat16_t, float>(params);
    SetFixpipeNz2dnFlag(1, 0, 0, 2);
    params.srcStride = 32;
    params.quantPre = QuantMode_t::NoQuant;
    CopyParams<float, float>(params);
}

TEST_F(DataCopyL0C2UBTest, Nz2DnRegistersMatchFixpipe)
{
    uint64_t loop3 = 0;
    uint64_t channel = 0;
    MOCKER(set_loop3_para).stubs().with(spy(loop3));
    MOCKER(set_channel_para).stubs().with(spy(channel));
    SetFixpipeNz2dnFlag(2, 240, 0x87654321U, 2);
    EXPECT_EQ(loop3, (0x87654321ULL << 32) | (240ULL << 16) | 2ULL);
    EXPECT_EQ(channel, 2ULL << 48);
    SetGCoreType(2);
    SetFixpipeNz2dnFlag(3, 128, 1024, 4);
    EXPECT_EQ(loop3, (0x87654321ULL << 32) | (240ULL << 16) | 2ULL);
    EXPECT_EQ(channel, 2ULL << 48);
    EXPECT_EQ(Internal::GetFixpipeNz2dnParams().dnNum, 2);
    SetGCoreType(1);
}

TEST_F(DataCopyL0C2UBTest, InvalidNz2DnParameters)
{
    KernelRaise::GetInstance().SetRaiseMode(false);
    uint64_t initialRaiseCount = KernelRaise::GetInstance().GetRaiseCount();
    DataCopyCO12DstParams params(32, 16, 32, 16, QuantMode_t::NoQuant, 0, false, false, 0, false, true);
    CopyParams<float, float>(params);
    EXPECT_EQ(KernelRaise::GetInstance().GetRaiseCount() - initialRaiseCount, 1);
    SetFixpipeNz2dnFlag(1, 0, 0, 1);
    initialRaiseCount = KernelRaise::GetInstance().GetRaiseCount();
    params.nz2ndEn = true;
    CopyParams<float, float>(params);
    params.nz2ndEn = false;
    params.dualDstCtl = 1;
    CopyParams<float, float>(params);
    params.dualDstCtl = 0;
    params.unitFlag = 2;
    params.srcStride = 32;
    SetFixpipeNz2dnFlag(1, 0, 0, 2);
    CopyParams<float, float>(params);
    SetFixpipeNz2dnFlag(1, 0, 0, 0);
    EXPECT_EQ(KernelRaise::GetInstance().GetRaiseCount() - initialRaiseCount, 4);
}

TEST_F(DataCopyL0C2UBTest, Nz2DnOverflowIncludesC0AndMatrixStrides)
{
    SetFixpipeNz2dnFlag(2, 240, 2880, 2);
    DataCopyCO12DstParams params(24, 24, 60, 80, QuantMode_t::NoQuant, 0, false, false, 0, false, true);
    uint64_t srcOffset = 0;
    uint64_t dstOffset = 0;
    check::CalculateDataCopyMaxOffset<float, float>(Hardware::L0C, Hardware::UB, params, srcOffset, dstOffset);
    EXPECT_EQ(srcOffset, 23456);
    EXPECT_EQ(dstOffset, 17136);
    SetFixpipeNz2dnFlag(2, 240, 0xffffffffU, 2);
    check::CalculateDataCopyMaxOffset<float, float>(Hardware::L0C, Hardware::UB, params, srcOffset, dstOffset);
    EXPECT_EQ(dstOffset, 5616ULL + 0xffffffffULL * sizeof(float));
    SetFixpipeNz2dnFlag(0, 0, 0, 1);
    check::CalculateDataCopyMaxOffset<float, float>(Hardware::L0C, Hardware::UB, params, srcOffset, dstOffset);
    EXPECT_EQ(srcOffset, 0);
    EXPECT_EQ(dstOffset, 0);
}

TEST_F(DataCopyL0C2UBTest, VectorQuantizationTiling)
{
    const auto single = GenFixpipeTiling(1024);
    EXPECT_EQ(single.nIterNum, 1);
    EXPECT_EQ(single.nSize, 1024);
    EXPECT_FALSE(single.isDb);
    const auto segmented = GenFixpipeTiling(1200);
    EXPECT_EQ(segmented.nIterNum, 2);
    EXPECT_EQ(segmented.nSize, 512);
    EXPECT_EQ(segmented.tailNSize, 176);
    EXPECT_TRUE(segmented.isDb);
}
