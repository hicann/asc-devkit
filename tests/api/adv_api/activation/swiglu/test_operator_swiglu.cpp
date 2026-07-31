/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file test_operator_swiglu.cpp
 * \brief
 */
#include <gtest/gtest.h>
#include "kernel_operator.h"
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
constexpr uint32_t SWIGLU_HALF_BUFFER_SIZE = 8; // 1 half data needs 2 float tmpbuffer which is 8 uint8 size
#endif
using namespace std;
using namespace AscendC;

enum TestMode { REUSE_SOURCE_MODE, NOT_REUSE_SOURCE_MODE, TMPBUFFER, TMPBUFFER_COUNT, COUNT };

class TEST_SWIGLU : public testing::Test {
protected:
    void SetUp() { AscendC::SetGCoreType(2); }
    void TearDown() { AscendC::SetGCoreType(0); }
};

template <typename T>
void main_vec_swiglu_level2_demo(
    __gm__ uint8_t* __restrict__ dstGm, __gm__ uint8_t* __restrict__ src0Gm, __gm__ uint8_t* __restrict__ src1Gm,
    const T& beta, uint32_t dataSize, TestMode testMode)
{
    TPipe tpipe;
    GlobalTensor<T> input0Global;
    GlobalTensor<T> input1Global;
    GlobalTensor<T> outputGlobal;
    input0Global.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(src0Gm), dataSize);
    input1Global.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(src1Gm), dataSize);
    outputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(dstGm), dataSize);

    TBuf<TPosition::VECCALC> tbuf1;
    tpipe.InitBuffer(tbuf1, dataSize * sizeof(T));
    LocalTensor<T> input0Local = tbuf1.Get<T>();

    TBuf<TPosition::VECCALC> tbuf2;
    tpipe.InitBuffer(tbuf2, dataSize * sizeof(T));
    LocalTensor<T> input1Local = tbuf2.Get<T>();

    TBuf<TPosition::VECCALC> tbuf3;
    tpipe.InitBuffer(tbuf3, dataSize * sizeof(T));
    LocalTensor<T> outputLocal = tbuf3.Get<T>();

    LocalTensor<uint8_t> tmpLocal;

    DataCopy(input0Local, input0Global, dataSize);
    DataCopy(input1Local, input1Global, dataSize);

    SetFlag<HardEvent::MTE2_V>(EVENT_ID0);
    WaitFlag<HardEvent::MTE2_V>(EVENT_ID0);

    if (testMode == REUSE_SOURCE_MODE) {
        SwiGLU<T, true>(outputLocal, input0Local, input1Local, beta);
    } else if (testMode == NOT_REUSE_SOURCE_MODE) {
        SwiGLU<T, false>(outputLocal, input0Local, input1Local, beta);
    } else if (testMode == COUNT) {
        SwiGLU<T, false>(outputLocal, input0Local, input1Local, beta, dataSize);
    } else {
        TBuf<TPosition::VECCALC> tbuf4;
        if (sizeof(T) == sizeof(float)) {
            tpipe.InitBuffer(tbuf4, dataSize);
        } else {
            tpipe.InitBuffer(tbuf4, dataSize * sizeof(float) * SWIGLU_HALF_BUFFER_SIZE);
        }
        LocalTensor<uint8_t> tmpLocal = tbuf4.Get<uint8_t>();
        if (testMode == TMPBUFFER) {
            SwiGLU<T, false>(outputLocal, input0Local, input1Local, beta, tmpLocal);
        } else {
            SwiGLU<T, false>(outputLocal, input0Local, input1Local, beta, tmpLocal, dataSize);
        }
    }
    SetFlag<HardEvent::V_MTE3>(EVENT_ID0);
    WaitFlag<HardEvent::V_MTE3>(EVENT_ID0);

    DataCopy(outputGlobal, outputLocal, dataSize);

    PipeBarrier<PIPE_ALL>();
}

#define VEC_SWIGLU_LEVEL2_TESTCASE(DATA_TYPE, BETA, TEST_MODE)                                           \
    TEST_F(TEST_SWIGLU, SWIGLU##DATA_TYPE##TEST_MODE##Case)                                              \
    {                                                                                                    \
        uint32_t dataSize = 256;                                                                         \
        uint32_t sel_mask_size = dataSize / AscendCUtils::GetBitSize(sizeof(uint8_t));                   \
        uint8_t input0Gm[dataSize * sizeof(DATA_TYPE)];                                                  \
        uint8_t input1Gm[dataSize * sizeof(DATA_TYPE)];                                                  \
        uint8_t outputGm[dataSize * sizeof(DATA_TYPE)];                                                  \
                                                                                                         \
        main_vec_swiglu_level2_demo<DATA_TYPE>(outputGm, input0Gm, input1Gm, BETA, dataSize, TEST_MODE); \
                                                                                                         \
        for (uint32_t i = 0; i < dataSize; i++) {                                                        \
            EXPECT_EQ(outputGm[i], 0x00);                                                                \
        }                                                                                                \
    }

VEC_SWIGLU_LEVEL2_TESTCASE(half, (half)1.0, REUSE_SOURCE_MODE);
VEC_SWIGLU_LEVEL2_TESTCASE(half, (half)0.0, NOT_REUSE_SOURCE_MODE);
VEC_SWIGLU_LEVEL2_TESTCASE(half, (half)1.0, TMPBUFFER);
VEC_SWIGLU_LEVEL2_TESTCASE(half, (half)0.0, TMPBUFFER_COUNT);
VEC_SWIGLU_LEVEL2_TESTCASE(half, (half)1.0, COUNT);
VEC_SWIGLU_LEVEL2_TESTCASE(float, (float)1.0, REUSE_SOURCE_MODE);
VEC_SWIGLU_LEVEL2_TESTCASE(float, (float)0.0, NOT_REUSE_SOURCE_MODE);
VEC_SWIGLU_LEVEL2_TESTCASE(float, (float)1.0, TMPBUFFER);
VEC_SWIGLU_LEVEL2_TESTCASE(float, (float)0.0, TMPBUFFER_COUNT);
VEC_SWIGLU_LEVEL2_TESTCASE(float, (float)1.0, COUNT);
