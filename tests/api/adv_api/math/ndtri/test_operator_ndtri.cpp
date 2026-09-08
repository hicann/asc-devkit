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
#define CANN_ASC_USE_EXPERIMENTAL
#include "adv_api/experimental/ndtri.h"

using namespace AscendC;
using namespace AscendC::experimental;

class TestNdtri : public testing::Test {
protected:
    void SetUp() override { SetGCoreType(2); }
    void TearDown() override { SetGCoreType(0); }
};

enum class NdtriCallMode : uint8_t {
    CAL_COUNT,
    WHOLE_TENSOR,
    IN_PLACE,
};

template <NdtriCallMode mode>
void RunNdtri(__gm__ float* outputGm, __gm__ float* inputGm, uint32_t dataSize, uint32_t calCount)
{
    TPipe pipe;
    GlobalTensor<float> inputGlobal;
    GlobalTensor<float> outputGlobal;
    inputGlobal.SetGlobalBuffer(inputGm, dataSize);
    outputGlobal.SetGlobalBuffer(outputGm, dataSize);

    TBuf<TPosition::VECCALC> inputBuffer;
    TBuf<TPosition::VECCALC> outputBuffer;
    pipe.InitBuffer(inputBuffer, dataSize * sizeof(float));
    pipe.InitBuffer(outputBuffer, dataSize * sizeof(float));
    LocalTensor<float> inputLocal = inputBuffer.Get<float>();
    LocalTensor<float> outputLocal = outputBuffer.Get<float>();
    DataCopy(inputLocal, inputGlobal, dataSize);
    Duplicate(outputLocal, 0.0f, dataSize);
    SetFlag<HardEvent::MTE2_V>(EVENT_ID0);
    WaitFlag<HardEvent::MTE2_V>(EVENT_ID0);

    if constexpr (mode == NdtriCallMode::CAL_COUNT) {
        Ndtri<float, false>(outputLocal, inputLocal, calCount);
    } else if constexpr (mode == NdtriCallMode::WHOLE_TENSOR) {
        Ndtri<float, false>(outputLocal, inputLocal);
    } else {
        Ndtri<float, false>(inputLocal, inputLocal, calCount);
        outputLocal = inputLocal;
    }

    SetFlag<HardEvent::V_MTE3>(EVENT_ID0);
    WaitFlag<HardEvent::V_MTE3>(EVENT_ID0);
    DataCopy(outputGlobal, outputLocal, dataSize);
    PipeBarrier<PIPE_ALL>();
}

// Kernel UTs validate that each overload can be compiled and invoked. Numerical accuracy and special-value
// behavior are covered by the Ndtri sample, because host-side kernel UT execution does not model vector results.

TEST_F(TestNdtri, FloatCalCount)
{
    constexpr uint32_t dataSize = 256;
    float inputGm[dataSize] = {};
    float outputGm[dataSize] = {0};
    for (uint32_t i = 0; i < dataSize; ++i) {
        inputGm[i] = 0.001f + static_cast<float>(i) / static_cast<float>(dataSize + 1);
    }
    RunNdtri<NdtriCallMode::CAL_COUNT>(outputGm, inputGm, dataSize, dataSize);
    SUCCEED();
}

TEST_F(TestNdtri, FloatZeroCalCount)
{
    constexpr uint32_t dataSize = 256;
    float inputGm[dataSize] = {0};
    float outputGm[dataSize] = {0};
    RunNdtri<NdtriCallMode::CAL_COUNT>(outputGm, inputGm, dataSize, 0);
    for (uint32_t i = 0; i < dataSize; ++i) {
        EXPECT_EQ(outputGm[i], 0.0f);
    }
}

TEST_F(TestNdtri, FloatWholeTensor)
{
    constexpr uint32_t dataSize = 256;
    float inputGm[dataSize] = {};
    float outputGm[dataSize] = {0};
    for (uint32_t i = 0; i < dataSize; ++i) {
        inputGm[i] = 0.999f - static_cast<float>(i) / static_cast<float>(dataSize + 1);
    }
    RunNdtri<NdtriCallMode::WHOLE_TENSOR>(outputGm, inputGm, dataSize, dataSize);
    SUCCEED();
}

TEST_F(TestNdtri, FloatInPlace)
{
    constexpr uint32_t dataSize = 256;
    float inputGm[dataSize] = {};
    float outputGm[dataSize] = {0};
    for (uint32_t i = 0; i < dataSize; ++i) {
        inputGm[i] = 0.001f + static_cast<float>(i) / static_cast<float>(dataSize + 1);
    }
    RunNdtri<NdtriCallMode::IN_PLACE>(outputGm, inputGm, dataSize, dataSize);
    SUCCEED();
}
