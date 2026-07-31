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

using namespace std;
using namespace AscendC;

constexpr uint32_t FLOAT_PER_BLOCK = 8;
constexpr uint32_t DTYPE16_PER_BLOCK = 16; // half / bfloat16

template <typename dstT, typename scaleT, bool scaleIsTensor, uint8_t mode>
void AscendDequantKernel(
    __gm__ uint8_t* __restrict__ srcGm, __gm__ uint8_t* __restrict__ dstGm, __gm__ uint8_t* __restrict__ deqScaleGm,
    __gm__ int32_t dataSize)
{
    TPipe tpipe;
    TQue<TPosition::VECIN, 1> vecQue;
    TQue<TPosition::VECIN, 1> vecDeqQue;
    TQue<TPosition::VECOUT, 1> vecOutQue;
    GlobalTensor<int32_t> inputGlobal;
    GlobalTensor<dstT> outputGlobal;
    GlobalTensor<scaleT> deqScaleGlobal;
    inputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ int32_t*>(srcGm), dataSize);
    outputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ dstT*>(dstGm), dataSize);
    deqScaleGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ scaleT*>(deqScaleGm), dataSize);
    tpipe.InitBuffer(vecQue, 1, dataSize * sizeof(int32_t));
    tpipe.InitBuffer(vecDeqQue, 1, dataSize * sizeof(scaleT));
    tpipe.InitBuffer(vecOutQue, 1, dataSize * sizeof(dstT));
    LocalTensor<int32_t> inputLocal = vecQue.AllocTensor<int32_t>();

    LocalTensor<scaleT> deqScaleLocal = vecDeqQue.AllocTensor<scaleT>();

    LocalTensor<dstT> outputLocal = vecOutQue.AllocTensor<dstT>();

    DataCopy(inputLocal, inputGlobal, dataSize);
    PipeBarrier<PIPE_MTE2>();
    DataCopy(deqScaleLocal, deqScaleGlobal, dataSize);

    SetFlag<HardEvent::MTE2_V>(EVENT_ID0);
    WaitFlag<HardEvent::MTE2_V>(EVENT_ID0);

    AscendDequant<dstT, scaleT>(outputLocal, inputLocal, deqScaleLocal);
    SetFlag<HardEvent::V_MTE3>(EVENT_ID0);
    WaitFlag<HardEvent::V_MTE3>(EVENT_ID0);

    DataCopy(outputGlobal, outputLocal, dataSize);
    PipeBarrier<PIPE_ALL>();
    vecQue.FreeTensor(inputLocal);
    vecDeqQue.FreeTensor(deqScaleLocal);
    vecOutQue.FreeTensor(outputLocal);
}

struct AscendDequantTestParams {
    int32_t dataSize;
    int32_t dataBitSize;
    void (*calFunc)(uint8_t*, uint8_t*, uint8_t*, int32_t);
};

class AscendDequantTestsuite : public testing::Test, public testing::WithParamInterface<AscendDequantTestParams> {
protected:
    void SetUp() {}
    void TearDown() {}
};

INSTANTIATE_TEST_CASE_P(
    TEST_ASCEND_DEQUANT, AscendDequantTestsuite,
    ::testing::Values(
        AscendDequantTestParams{24, 4, AscendDequantKernel<float, float, true, 0>},
        AscendDequantTestParams{24, 4, AscendDequantKernel<float, float, true, 1>},
        AscendDequantTestParams{24, 4, AscendDequantKernel<float, float, false, 0>},
        AscendDequantTestParams{24, 4, AscendDequantKernel<float, float, false, 1>}
#if !(                       \
    defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)) // 用例超出kirinx90 9030 ub size，bf16 kirinx90 9030不支持
        ,
        AscendDequantTestParams{256, 4, AscendDequantKernel<half, uint64_t, false, 0>},
        AscendDequantTestParams{512, 4, AscendDequantKernel<half, uint64_t, false, 1>},
        AscendDequantTestParams{256, 4, AscendDequantKernel<half, uint64_t, true, 0>},
        AscendDequantTestParams{512, 4, AscendDequantKernel<half, uint64_t, true, 1>},
        AscendDequantTestParams{16, 4, AscendDequantKernel<bfloat16_t, bfloat16_t, true, 0>},
        AscendDequantTestParams{128, 4, AscendDequantKernel<float, bfloat16_t, true, 0>},
        AscendDequantTestParams{128, 4, AscendDequantKernel<float, bfloat16_t, true, 1>},
        AscendDequantTestParams{16, 4, AscendDequantKernel<bfloat16_t, bfloat16_t, false, 0>},
        AscendDequantTestParams{128, 4, AscendDequantKernel<float, bfloat16_t, false, 0>},
        AscendDequantTestParams{128, 4, AscendDequantKernel<float, bfloat16_t, false, 1>},
        AscendDequantTestParams{5120, 4, AscendDequantKernel<bfloat16_t, bfloat16_t, true, 0>}
#endif
        ));

TEST_P(AscendDequantTestsuite, AscendDequantTestCase)
{
    auto param = GetParam();
    uint8_t srcGm[param.dataSize * sizeof(int32_t)] = {0};
    uint8_t dstGm[param.dataSize * sizeof(half)] = {0};
    uint8_t deqScaleGm[param.dataSize * sizeof(uint64_t)] = {0};

    param.calFunc(srcGm, dstGm, deqScaleGm, param.dataSize);
    for (int32_t i = 0; i < param.dataSize; i++) {
        EXPECT_EQ(dstGm[i], 0x00);
    }
}
