/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <cstdint>
#include <gtest/gtest.h>
#include "kernel_operator.h"
// #include "api_check/kernel_cpu_check.h"
#include "mockcpp/mockcpp.hpp"

using namespace std;
using namespace AscendC;

struct TestDataCopyPadParams {
    uint32_t dataSize;
    uint32_t typeSize;
    void (*cal_func)(uint8_t*, uint8_t*, int32_t);
};

class TestDataCopyPadSuite : public testing::Test, public testing::WithParamInterface<TestDataCopyPadParams> {
protected:
    void SetUp() {}
    void TearDown() {}
};

template <typename T>
void MainDataCopyPadKernel(
    __gm__ uint8_t* __restrict__ srcGm, __gm__ uint8_t* __restrict__ dstGm, __gm__ int32_t dataSize)
{
    TPipe tpipe;
    GlobalTensor<T> srcGlobal;
    GlobalTensor<T> dstGlobal;

    srcGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(srcGm), dataSize);
    dstGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(dstGm), dataSize);

    GlobalTensor<half> testGlobal;
    testGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ half*>(dstGm), dataSize);

    InitOutput(testGlobal, dataSize * sizeof(T) / sizeof(half));
    LocalTensor<T> inputLocal;
    TBuffAddr tbuf;
    tbuf.logicPos = static_cast<uint8_t>(TPosition::VECCALC);
    inputLocal.SetAddr(tbuf);
    inputLocal.InitBuffer(0, dataSize);
    LocalTensor<T> outputLocal;
    TBuffAddr tbufOut;
    tbufOut.logicPos = static_cast<uint8_t>(TPosition::VECCALC);
    outputLocal.SetAddr(tbufOut);
    outputLocal.InitBuffer(0, dataSize);
    DataCopyParams datacopyParams{1, 2, 0, 0};
    DataCopyPadParams datacopyPadParams{false, 0, 0, 0};
    DataCopyExtParams copyParams{1, 20 * sizeof(T), 0, 0, 0};
    LoopModeParams loopParams{1, 1, 0, 32 * sizeof(T), 0, 32 * sizeof(T)};
    DataCopyPadExtParams<T> padParams{true, 0, 1, 0};
    DataCopyPad(inputLocal, srcGlobal, copyParams, padParams);
    SetLoopModePara(loopParams, DataCopyMVType::OUT_TO_UB);
    ResetLoopModePara(DataCopyMVType::OUT_TO_UB);
    SetLoopModePara(loopParams, DataCopyMVType::UB_TO_OUT);
    DataCopy(outputLocal, inputLocal, datacopyParams);
    ResetLoopModePara(DataCopyMVType::UB_TO_OUT);
    DataCopyPad<T, PaddingMode::Normal>(dstGlobal, outputLocal, copyParams);
    DataCopyPad<T, PaddingMode::Normal>(dstGlobal, outputLocal, datacopyParams);
    DataCopyPad<T, PaddingMode::Normal>(inputLocal, srcGlobal, datacopyParams, datacopyPadParams);
    DataCopyPad<T, PaddingMode::Normal>(inputLocal, srcGlobal, copyParams, DataCopyPadExtParams<T>());

    LocalTensor<T> inputCbufLocal;
    TBuffAddr cbufTbuf;
    cbufTbuf.logicPos = static_cast<uint8_t>(TPosition::TSCM);
    inputCbufLocal.SetAddr(cbufTbuf);
    inputCbufLocal.InitBuffer(0, dataSize);
    DataCopyPad<T, PaddingMode::Normal>(inputCbufLocal, srcGlobal, copyParams, DataCopyPadExtParams<T>());
}

INSTANTIATE_TEST_CASE_P(
    TEST_DATA_COPY_PAD, TestDataCopyPadSuite,
    ::testing::Values(
        TestDataCopyPadParams{64, 4, MainDataCopyPadKernel<int32_t>},
        TestDataCopyPadParams{64, 2, MainDataCopyPadKernel<int16_t>},
        TestDataCopyPadParams{64, 4, MainDataCopyPadKernel<float>},
        TestDataCopyPadParams{64, 8, MainDataCopyPadKernel<uint64_t>},
        TestDataCopyPadParams{64, 2, MainDataCopyPadKernel<half>},
        TestDataCopyPadParams{64, 1, MainDataCopyPadKernel<int8_t>},
        TestDataCopyPadParams{64, 1, MainDataCopyPadKernel<uint8_t>}));

TEST_P(TestDataCopyPadSuite, TestDataCopyPadCases)
{
    SetGCoreType(2);
    auto param = GetParam();
    uint8_t srcGm[param.dataSize * param.typeSize];
    uint8_t dstGm[param.dataSize * param.typeSize];
    param.cal_func(srcGm, dstGm, param.dataSize);
    for (uint32_t i = 0; i < 20; i++) {
        EXPECT_EQ(dstGm[i], 0x00);
    }
    SetGCoreType(0);
}

class DataCopyPadStrideBoundarySuite : public testing::Test {
protected:
    static constexpr int64_t STRIDE_UPPER_BOUND = static_cast<int64_t>(1ULL << 40);

    void SetUp() override
    {
        SetGCoreType(2);
        KernelRaise::GetInstance().SetRaiseMode(false);
        OOMInit();
    }

    void TearDown() override
    {
        KernelRaise::GetInstance().SetRaiseMode(true);
        GlobalMockObject::verify();
        SetGCoreType(0);
    }

    void ExpectSrcStrideRaiseCount(int64_t srcStride, uint64_t expectedRaiseCount)
    {
        DataCopyExtParams copyParams{1, 1, srcStride, 0, 0};
        const uint64_t startRaiseCount = KernelRaise::GetInstance().GetRaiseCount();

        CheckSrcGmDataCopyExtParamsRange(copyParams);

        EXPECT_EQ(KernelRaise::GetInstance().GetRaiseCount() - startRaiseCount, expectedRaiseCount);
    }

    void ExpectDstStrideRaiseCount(int64_t dstStride, uint64_t expectedRaiseCount)
    {
        MOCKER_CPP(GetSysWorkSpacePtr).expects(once()).will(returnValue(static_cast<__gm__ uint8_t*>(nullptr)));
        MOCKER_CPP(
            copy_ubuf_to_gm_align_v2,
            void(__gm__ void*, __ubuf__ void*, uint8_t, uint32_t, uint32_t, uint8_t, uint64_t, uint32_t))
            .expects(once());
        uint8_t src = 0;
        uint8_t dst = 0;
        DataCopyExtParams copyParams{1, 1, 0, dstStride, 0};
        const uint64_t startRaiseCount = KernelRaise::GetInstance().GetRaiseCount();

        DataCopyPadUB2GMImpl(
            reinterpret_cast<__gm__ uint8_t*>(&dst), reinterpret_cast<__ubuf__ uint8_t*>(&src), copyParams);

        EXPECT_EQ(KernelRaise::GetInstance().GetRaiseCount() - startRaiseCount, expectedRaiseCount);
    }
};

TEST_F(DataCopyPadStrideBoundarySuite, SrcStrideBelowUpperBoundDoesNotRaise)
{
    ExpectSrcStrideRaiseCount(STRIDE_UPPER_BOUND - 1, 0);
}

TEST_F(DataCopyPadStrideBoundarySuite, SrcStrideAtUpperBoundRaises)
{
    ExpectSrcStrideRaiseCount(STRIDE_UPPER_BOUND, 1);
}

TEST_F(DataCopyPadStrideBoundarySuite, DstStrideBelowUpperBoundDoesNotRaise)
{
    ExpectDstStrideRaiseCount(STRIDE_UPPER_BOUND - 1, 0);
}

TEST_F(DataCopyPadStrideBoundarySuite, DstStrideAtUpperBoundRaises)
{
    ExpectDstStrideRaiseCount(STRIDE_UPPER_BOUND, 1);
}
