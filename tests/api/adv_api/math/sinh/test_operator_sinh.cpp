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

enum TestMode { NBUFFER_NCOUNT_MODE, NBUFFER_COUNT_MODE, BUFFER_NCOUNT_MODE, BUFFER_COUNT_MODE };

class TEST_SINH : public testing::Test {
protected:
    void SetUp() { AscendC::SetGCoreType(2); }
    void TearDown() { AscendC::SetGCoreType(0); }
};

template <typename T>
void MainVecSinhLevel2Demo(
    __gm__ uint8_t* __restrict__ dstGm, __gm__ uint8_t* __restrict__ srcGm, uint32_t dataSize, TestMode testMode)
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

    if (testMode == NBUFFER_NCOUNT_MODE) {
        Sinh<T>(outputLocal, inputLocal);
    } else if (testMode == NBUFFER_COUNT_MODE) {
        Sinh<T>(outputLocal, inputLocal, dataSize);
    } else {
        TBuf<TPosition::VECCALC> tbuf3;
        if (sizeof(T) == sizeof(float)) {
            tpipe.InitBuffer(tbuf3, dataSize * 1 * sizeof(float));
        } else {
            tpipe.InitBuffer(tbuf3, dataSize * 4 * sizeof(half));
        }
        LocalTensor<uint8_t> tmpLocal = tbuf3.Get<uint8_t>();

        if (testMode == BUFFER_NCOUNT_MODE) {
            Sinh<T>(outputLocal, inputLocal, tmpLocal);
        } else if (testMode == BUFFER_COUNT_MODE) {
            Sinh<T>(outputLocal, inputLocal, tmpLocal, dataSize);
        }
    }

    SetFlag<HardEvent::V_MTE3>(EVENT_ID0);
    WaitFlag<HardEvent::V_MTE3>(EVENT_ID0);

    DataCopy(outputGlobal, outputLocal, dataSize);

    PipeBarrier<PIPE_ALL>();
}
#define VEC_SINH_LEVEL2_TESTCASE(DATA_TYPE, TEST_MODE)                            \
    TEST_F(TEST_SINH, Sinh##DATA_TYPE##TEST_MODE##Case)                           \
    {                                                                             \
        uint32_t dataSize = 16384;                                                \
        uint8_t inputGm[dataSize * sizeof(DATA_TYPE)];                            \
        uint8_t outputGm[dataSize * sizeof(DATA_TYPE)];                           \
                                                                                  \
        MainVecSinhLevel2Demo<DATA_TYPE>(outputGm, inputGm, dataSize, TEST_MODE); \
                                                                                  \
        for (uint32_t i = 0; i < dataSize; i++) {                                 \
            EXPECT_EQ(outputGm[i], 0x00);                                         \
        }                                                                         \
    }

VEC_SINH_LEVEL2_TESTCASE(half, NBUFFER_NCOUNT_MODE);
VEC_SINH_LEVEL2_TESTCASE(half, NBUFFER_COUNT_MODE);
#if !(defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)) // 用例超出kirinx90 9030 ub size
VEC_SINH_LEVEL2_TESTCASE(float, NBUFFER_NCOUNT_MODE);
VEC_SINH_LEVEL2_TESTCASE(float, NBUFFER_COUNT_MODE);
VEC_SINH_LEVEL2_TESTCASE(float, BUFFER_NCOUNT_MODE);
VEC_SINH_LEVEL2_TESTCASE(half, BUFFER_NCOUNT_MODE);
VEC_SINH_LEVEL2_TESTCASE(float, BUFFER_COUNT_MODE);
VEC_SINH_LEVEL2_TESTCASE(half, BUFFER_COUNT_MODE);
#endif
