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

static const uint32_t tmpBufferSize = 1537;

enum TestMode {
    CLAMP_MAX,
    CLAMP_MIN,
    CLAMP_BOTH_TENSOR,
    CLAMP_TENSOR_SCALAR,
    CLAMP_SCALAR_TENSOR,
    CLAMP_BOTH_SCALAR,
};

enum TmpMode {
    MODE_NORMAL,
    MODE_TMPBUFFER,
};

class TEST_CLAMP : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

namespace AscendC {
template <typename T, TestMode testMode>
void main_vec_clamp_demo(
    __gm__ uint8_t* __restrict__ dstGm, __gm__ uint8_t* __restrict__ srcGm, __gm__ uint8_t* __restrict__ minGm,
    __gm__ uint8_t* __restrict__ maxGm, uint32_t dataSize, TmpMode tmpMode)
{
    TPipe tpipe;
    GlobalTensor<T> inputGlobal;
    GlobalTensor<T> outputGlobal;
    GlobalTensor<T> minGlobal;
    GlobalTensor<T> maxGlobal;
    inputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(srcGm), dataSize);
    outputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(dstGm), dataSize);
    minGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(minGm), dataSize);
    maxGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(maxGm), dataSize);

    TBuf<TPosition::VECCALC> tbuf1;
    tpipe.InitBuffer(tbuf1, dataSize * sizeof(T));
    LocalTensor<T> inputLocal = tbuf1.Get<T>();
    TBuf<TPosition::VECCALC> tbuf2;
    tpipe.InitBuffer(tbuf2, dataSize * sizeof(T));
    LocalTensor<T> minLocal = tbuf2.Get<T>();
    TBuf<TPosition::VECCALC> tbuf3;
    tpipe.InitBuffer(tbuf3, dataSize * sizeof(T));
    LocalTensor<T> maxLocal = tbuf3.Get<T>();

    TBuf<TPosition::VECCALC> tbuf4;
    tpipe.InitBuffer(tbuf4, dataSize * sizeof(T));
    LocalTensor<T> outputLocal = tbuf4.Get<T>();

    DataCopy(inputLocal, inputGlobal, dataSize);
    DataCopy(minLocal, minGlobal, dataSize);
    DataCopy(maxLocal, maxGlobal, dataSize);

    SetFlag<HardEvent::MTE2_V>(EVENT_ID0);
    WaitFlag<HardEvent::MTE2_V>(EVENT_ID0);

    T scalar = 1;
    if (tmpMode == MODE_NORMAL) {
        if constexpr (testMode == CLAMP_MAX) {
            ClampMax<T, false>(outputLocal, inputLocal, scalar, dataSize);
        } else if constexpr (testMode == CLAMP_MIN) {
            ClampMin<T, false>(outputLocal, inputLocal, scalar, dataSize);
        } else {
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
            if constexpr (testMode == CLAMP_BOTH_TENSOR) {
                Clamp(outputLocal, inputLocal, minLocal, maxLocal, dataSize);
            } else if constexpr (testMode == CLAMP_TENSOR_SCALAR) {
                Clamp(outputLocal, inputLocal, minLocal, scalar, dataSize);
            } else if constexpr (testMode == CLAMP_SCALAR_TENSOR) {
                Clamp(outputLocal, inputLocal, scalar, maxLocal, dataSize);
            } else if constexpr (testMode == CLAMP_BOTH_SCALAR) {
                Clamp(outputLocal, inputLocal, scalar, scalar, dataSize);
            }
#endif
        }
    } else if (tmpMode == MODE_TMPBUFFER) {
        LocalTensor<uint8_t> sharedTmpBuffer;
        bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
        sharedTmpBuffer.SetSize(tmpBufferSize);
        if constexpr (testMode == CLAMP_MAX) {
            ClampMax<T, false>(outputLocal, inputLocal, sharedTmpBuffer, scalar, dataSize);
        } else if constexpr (testMode == CLAMP_MIN) {
            ClampMin<T, false>(outputLocal, inputLocal, sharedTmpBuffer, scalar, dataSize);
        }
    }

    SetFlag<HardEvent::V_MTE3>(EVENT_ID0);
    WaitFlag<HardEvent::V_MTE3>(EVENT_ID0);

    DataCopy(outputGlobal, outputLocal, dataSize);

    PipeBarrier<PIPE_ALL>();
}
} // namespace AscendC
#define VEC_CLAMP_TESTCASE(DATA_TYPE, TEST_MODE, CALCOUNT, TMP_MODE)                                    \
    TEST_F(TEST_CLAMP, Clamp##_##DATA_TYPE##_##CALCOUNT##_##TEST_MODE##_##TMP_MODE##_##Case)            \
    {                                                                                                   \
        uint32_t dataSize = CALCOUNT;                                                                   \
        uint8_t inputGm[dataSize * sizeof(DATA_TYPE)] = {0};                                            \
        uint8_t outputGm[dataSize * sizeof(DATA_TYPE)] = {0};                                           \
        uint8_t minGm[dataSize * sizeof(DATA_TYPE)] = {0};                                              \
        uint8_t maxGm[dataSize * sizeof(DATA_TYPE)] = {0};                                              \
        main_vec_clamp_demo<DATA_TYPE, TEST_MODE>(outputGm, inputGm, minGm, maxGm, dataSize, TMP_MODE); \
        for (uint32_t i = 0; i < dataSize; i++) {                                                       \
            EXPECT_EQ(outputGm[i], 0x00);                                                               \
        }                                                                                               \
    }
VEC_CLAMP_TESTCASE(float, CLAMP_MAX, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(float, CLAMP_MIN, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(half, CLAMP_MAX, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(half, CLAMP_MIN, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(half, CLAMP_MIN, 2048, MODE_TMPBUFFER);
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
VEC_CLAMP_TESTCASE(uint8_t, CLAMP_BOTH_TENSOR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(int8_t, CLAMP_BOTH_TENSOR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(bfloat16_t, CLAMP_BOTH_TENSOR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(half, CLAMP_BOTH_TENSOR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(uint16_t, CLAMP_BOTH_TENSOR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(int16_t, CLAMP_BOTH_TENSOR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(float, CLAMP_BOTH_TENSOR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(uint32_t, CLAMP_BOTH_TENSOR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(int32_t, CLAMP_BOTH_TENSOR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(int64_t, CLAMP_BOTH_TENSOR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(uint64_t, CLAMP_BOTH_TENSOR, 256, MODE_NORMAL);

VEC_CLAMP_TESTCASE(uint8_t, CLAMP_BOTH_SCALAR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(int8_t, CLAMP_BOTH_SCALAR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(bfloat16_t, CLAMP_BOTH_SCALAR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(half, CLAMP_BOTH_SCALAR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(uint16_t, CLAMP_BOTH_SCALAR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(int16_t, CLAMP_BOTH_SCALAR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(float, CLAMP_BOTH_SCALAR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(uint32_t, CLAMP_BOTH_SCALAR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(int32_t, CLAMP_BOTH_SCALAR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(int64_t, CLAMP_BOTH_SCALAR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(uint64_t, CLAMP_BOTH_SCALAR, 256, MODE_NORMAL);

VEC_CLAMP_TESTCASE(uint8_t, CLAMP_TENSOR_SCALAR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(int8_t, CLAMP_TENSOR_SCALAR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(bfloat16_t, CLAMP_TENSOR_SCALAR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(half, CLAMP_TENSOR_SCALAR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(uint16_t, CLAMP_TENSOR_SCALAR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(int16_t, CLAMP_TENSOR_SCALAR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(float, CLAMP_TENSOR_SCALAR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(uint32_t, CLAMP_TENSOR_SCALAR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(int32_t, CLAMP_TENSOR_SCALAR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(int64_t, CLAMP_TENSOR_SCALAR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(uint64_t, CLAMP_TENSOR_SCALAR, 256, MODE_NORMAL);

VEC_CLAMP_TESTCASE(uint8_t, CLAMP_SCALAR_TENSOR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(int8_t, CLAMP_SCALAR_TENSOR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(bfloat16_t, CLAMP_SCALAR_TENSOR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(half, CLAMP_SCALAR_TENSOR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(uint16_t, CLAMP_SCALAR_TENSOR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(int16_t, CLAMP_SCALAR_TENSOR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(float, CLAMP_SCALAR_TENSOR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(uint32_t, CLAMP_SCALAR_TENSOR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(int32_t, CLAMP_SCALAR_TENSOR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(int64_t, CLAMP_SCALAR_TENSOR, 256, MODE_NORMAL);
VEC_CLAMP_TESTCASE(uint64_t, CLAMP_SCALAR_TENSOR, 256, MODE_NORMAL);
#endif
