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

class TEST_SIGN : public testing::Test {
protected:
    void SetUp() { AscendC::SetGCoreType(2); }
    void TearDown() { AscendC::SetGCoreType(0); }
};

template <typename T>
void main_vec_sign_level2_demo(
    __gm__ uint8_t* __restrict__ dstGm, __gm__ uint8_t* __restrict__ srcGm, uint32_t dataSize, TestMode TEST_MODE)
{
    TPipe tpipe;
    GlobalTensor<T> input_global;
    GlobalTensor<T> output_global;
    input_global.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(srcGm), dataSize);
    output_global.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(dstGm), dataSize);

    TBuf<TPosition::VECCALC> tbuf0;
    tpipe.InitBuffer(tbuf0, dataSize * sizeof(T));
    LocalTensor<T> input_local = tbuf0.Get<T>();

    TBuf<TPosition::VECCALC> tbuf1;
    tpipe.InitBuffer(tbuf1, dataSize * sizeof(T));
    LocalTensor<T> output_local = tbuf1.Get<T>();

    DataCopy(input_local, input_global, dataSize);

    SetFlag<HardEvent::MTE2_V>(EVENT_ID0);
    WaitFlag<HardEvent::MTE2_V>(EVENT_ID0);

    if (TEST_MODE == NBUFFER_NCOUNT_MODE) {
        Sign<T>(output_local, input_local);
    } else if (TEST_MODE == NBUFFER_COUNT_MODE) {
        Sign<T>(output_local, input_local, dataSize);
    } else {
        TBuf<TPosition::VECCALC> tbuf2;
        tpipe.InitBuffer(tbuf2, dataSize * 3 * sizeof(uint8_t));
        LocalTensor<uint8_t> tmp_local = tbuf2.Get<uint8_t>();

        if (TEST_MODE == BUFFER_NCOUNT_MODE) {
            Sign<T>(output_local, input_local, tmp_local);
        } else if (TEST_MODE == BUFFER_COUNT_MODE) {
            Sign<T>(output_local, input_local, tmp_local, dataSize);
        }
    }

    SetFlag<HardEvent::V_MTE3>(EVENT_ID0);
    WaitFlag<HardEvent::V_MTE3>(EVENT_ID0);

    DataCopy(output_global, output_local, dataSize);

    PipeBarrier<PIPE_ALL>();
}
#define VEC_SIGN_LEVEL2_TESTCASE(DATA_TYPE, TEST_MODE)                                   \
    TEST_F(TEST_SIGN, Sign##DATA_TYPE##TEST_MODE##Case)                                  \
    {                                                                                    \
        uint32_t data_size = 8192;                                                       \
        uint8_t input_gm[data_size * sizeof(DATA_TYPE)];                                 \
        uint8_t output_gm[data_size * sizeof(DATA_TYPE)];                                \
                                                                                         \
        main_vec_sign_level2_demo<DATA_TYPE>(output_gm, input_gm, data_size, TEST_MODE); \
                                                                                         \
        for (uint32_t i = 0; i < data_size; i++) {                                       \
            EXPECT_EQ(output_gm[i], 0x00);                                               \
        }                                                                                \
    }

VEC_SIGN_LEVEL2_TESTCASE(float, NBUFFER_NCOUNT_MODE);
VEC_SIGN_LEVEL2_TESTCASE(half, NBUFFER_NCOUNT_MODE);
VEC_SIGN_LEVEL2_TESTCASE(float, NBUFFER_COUNT_MODE);
VEC_SIGN_LEVEL2_TESTCASE(half, NBUFFER_COUNT_MODE);
VEC_SIGN_LEVEL2_TESTCASE(float, BUFFER_NCOUNT_MODE);
VEC_SIGN_LEVEL2_TESTCASE(half, BUFFER_NCOUNT_MODE);
VEC_SIGN_LEVEL2_TESTCASE(float, BUFFER_COUNT_MODE);
VEC_SIGN_LEVEL2_TESTCASE(half, BUFFER_COUNT_MODE);
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
VEC_SIGN_LEVEL2_TESTCASE(int64_t, NBUFFER_NCOUNT_MODE);
VEC_SIGN_LEVEL2_TESTCASE(int64_t, NBUFFER_COUNT_MODE);
VEC_SIGN_LEVEL2_TESTCASE(int64_t, BUFFER_NCOUNT_MODE);
VEC_SIGN_LEVEL2_TESTCASE(int64_t, BUFFER_COUNT_MODE);
#endif
