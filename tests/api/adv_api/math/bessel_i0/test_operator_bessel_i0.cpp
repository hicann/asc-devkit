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
#include "experimental/bessel_i0.h"

using namespace std;
using namespace AscendC;
using namespace AscendC::experimental;

enum BesselI0TestMode {
    TMP_BUFFER_COUNT_MODE,
    TMP_BUFFER_WHOLE_MODE,
    FRAMEWORK_TMP_COUNT_MODE,
    FRAMEWORK_TMP_WHOLE_MODE,
};

class TEST_BESSEL_I0 : public testing::Test {
protected:
    void SetUp() { AscendC::SetGCoreType(2); }
    void TearDown() { AscendC::SetGCoreType(0); }
};

template <typename T>
void MainVecBesselI0Demo(
    __gm__ uint8_t* __restrict__ dstGm, __gm__ uint8_t* __restrict__ srcGm, uint32_t dataSize,
    BesselI0TestMode testMode)
{
    TPipe tpipe;
    GlobalTensor<T> inputGlobal;
    GlobalTensor<T> outputGlobal;
    inputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(srcGm), dataSize);
    outputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(dstGm), dataSize);

    TBuf<TPosition::VECCALC> tbuf1;
    tpipe.InitBuffer(tbuf1, dataSize * sizeof(T));
    LocalTensor<T> inputLocal = tbuf1.Get<T>();

    TBuf<TPosition::VECCALC> tbuf2;
    tpipe.InitBuffer(tbuf2, dataSize * sizeof(T));
    LocalTensor<T> outputLocal = tbuf2.Get<T>();

    DataCopy(inputLocal, inputGlobal, dataSize);

    SetFlag<HardEvent::MTE2_V>(EVENT_ID0);
    WaitFlag<HardEvent::MTE2_V>(EVENT_ID0);

    if (testMode == TMP_BUFFER_COUNT_MODE) {
        TBuf<TPosition::VECCALC> tbuf3;
        tpipe.InitBuffer(tbuf3, dataSize * sizeof(T));
        LocalTensor<uint8_t> tmpLocal = tbuf3.Get<uint8_t>();
        BesselI0<T>(outputLocal, inputLocal, tmpLocal, dataSize);
    } else if (testMode == TMP_BUFFER_WHOLE_MODE) {
        TBuf<TPosition::VECCALC> tbuf3;
        tpipe.InitBuffer(tbuf3, dataSize * sizeof(T));
        LocalTensor<uint8_t> tmpLocal = tbuf3.Get<uint8_t>();
        BesselI0<T>(outputLocal, inputLocal, tmpLocal);
    } else if (testMode == FRAMEWORK_TMP_COUNT_MODE) {
        BesselI0<T>(outputLocal, inputLocal, dataSize);
    } else if (testMode == FRAMEWORK_TMP_WHOLE_MODE) {
        BesselI0<T>(outputLocal, inputLocal);
    }

    SetFlag<HardEvent::V_MTE3>(EVENT_ID0);
    WaitFlag<HardEvent::V_MTE3>(EVENT_ID0);

    DataCopy(outputGlobal, outputLocal, dataSize);

    PipeBarrier<PIPE_ALL>();
}

#define VEC_BESSEL_I0_TESTCASE(DATA_TYPE, testMode)                            \
    TEST_F(TEST_BESSEL_I0, BesselI0##DATA_TYPE##testMode##Case)                \
    {                                                                          \
        uint32_t dataSize = 512;                                               \
        uint8_t inputGm[dataSize * sizeof(DATA_TYPE)] = {0};                   \
        uint8_t outputGm[dataSize * sizeof(DATA_TYPE)] = {0};                  \
                                                                               \
        MainVecBesselI0Demo<DATA_TYPE>(outputGm, inputGm, dataSize, testMode); \
                                                                               \
        for (uint32_t i = 0; i < dataSize; i++) {                              \
            EXPECT_EQ(outputGm[i], 0x00);                                      \
        }                                                                      \
    }

VEC_BESSEL_I0_TESTCASE(float, TMP_BUFFER_COUNT_MODE);
VEC_BESSEL_I0_TESTCASE(float, TMP_BUFFER_WHOLE_MODE);
VEC_BESSEL_I0_TESTCASE(float, FRAMEWORK_TMP_COUNT_MODE);
VEC_BESSEL_I0_TESTCASE(float, FRAMEWORK_TMP_WHOLE_MODE);
