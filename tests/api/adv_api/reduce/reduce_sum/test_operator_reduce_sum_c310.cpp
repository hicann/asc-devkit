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
#define private public
#define protected public
#include "kernel_operator.h"

using namespace std;
using namespace AscendC;

constexpr int32_t BUFFER_NUM = 1; // tensor num for each queue

template <typename TYPE, int32_t EXTENT>
struct ReduceOpsType {
    using T = TYPE;
    constexpr static int32_t extent = EXTENT;
};

template <class Type>
__aicore__ inline constexpr uint32_t Align32B()
{
    const int alginSize = 32 / sizeof(typename Type::T);
    return (Type::extent + alginSize - 1) / alginSize * alginSize;
}

template <class X_TYPE, class Y_TYPE, uint32_t Pattern, uint32_t IsReuseSource>
class KernelReduceOps {
public:
    using T_TYPE = typename X_TYPE::T;

    __aicore__ inline KernelReduceOps() {}

    __aicore__ inline void Init(
        GM_ADDR x, GM_ADDR y, const uint32_t aLength, const uint32_t rLength, const uint32_t srcInnerPad)
    {
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#ifdef ASCENDC_CPU_DEBUG
        g_coreType = 2;
#endif
#endif
        // get start index for current core, core parallel
        xGm.SetGlobalBuffer((__gm__ T_TYPE*)x, X_TYPE::extent);
        yGm.SetGlobalBuffer((__gm__ T_TYPE*)y, Y_TYPE::extent);

        // pipe alloc memory to queue, the unit is Bytes
        pipe.InitBuffer(inQueueX, BUFFER_NUM, Align32B<X_TYPE>() * sizeof(T_TYPE));
        pipe.InitBuffer(outQueueY, BUFFER_NUM, Y_TYPE::extent * sizeof(T_TYPE));
        const uint32_t alginSize = 32 / sizeof(T_TYPE);

        this->aLength = aLength;
        this->rLength = rLength;
        this->aLengthAlign = (aLength + alginSize - 1) / alginSize * alginSize;
        this->rLengthAlign = (rLength + alginSize - 1) / alginSize * alginSize;
        this->srcInnerPad = (srcInnerPad == 1) ? true : false;
    }

    __aicore__ inline void Compute()
    {
        LocalTensor<T_TYPE> xLocal = inQueueX.DeQue<T_TYPE>();
        LocalTensor<T_TYPE> yLocal = outQueueY.AllocTensor<T_TYPE>();

        if constexpr (Pattern == 0) {
            uint32_t srcShape[2] = {aLength, rLength};
            constexpr bool isReuse = (IsReuseSource == 1) ? true : false;
            ReduceSum<T_TYPE, Pattern::Reduce::AR, isReuse>(yLocal, xLocal, srcShape, srcInnerPad);
        } else if constexpr (Pattern == 1) {
            uint32_t srcShape[2] = {rLength, aLength};
            constexpr bool isReuse = (IsReuseSource == 1) ? true : false;
            ReduceSum<T_TYPE, Pattern::Reduce::RA, isReuse>(yLocal, xLocal, srcShape, srcInnerPad);
        }

        outQueueY.EnQue<T_TYPE>(yLocal);
        inQueueX.FreeTensor(xLocal);
    }
    __aicore__ inline void Process()
    {
        CopyIn();
        Compute();
        CopyOut();
    }

private:
    __aicore__ inline void CopyIn()
    {
        // alloc tensor from queue memory
        LocalTensor<T_TYPE> xLocal = inQueueX.AllocTensor<T_TYPE>();
        DataCopyPadExtParams<T_TYPE> padParams;
        DataCopyExtParams copyParams;
        copyParams.blockCount = 1;
        copyParams.blockLen = X_TYPE::extent * sizeof(T_TYPE);
        copyParams.srcStride = 0;
        copyParams.dstStride = 0;
        padParams.isPad = 0;
        padParams.leftPadding = 0;
        padParams.rightPadding = 0;
        padParams.paddingValue = 0;
        DataCopyPad(xLocal, xGm, copyParams, padParams);
        // enque input tensors to VECIN queue
        inQueueX.EnQue(xLocal);
    }

    __aicore__ inline void CopyOut()
    {
        // deque output tensor from VECOUT queue
        LocalTensor<T_TYPE> yLocal = outQueueY.DeQue<T_TYPE>();
        DataCopyPad(yGm, yLocal, {1, static_cast<uint32_t>(Y_TYPE::extent * sizeof(T_TYPE)), 0, 0});
        // free output tensor for reuse
        outQueueY.FreeTensor(yLocal);
    }

private:
    TPipe pipe;
    // create queues for input, in this case depth is equal to buffer num
    TQue<TPosition::VECIN, BUFFER_NUM> inQueueX;
    // create queue for output, in this case depth is equal to buffer num
    TQue<TPosition::VECOUT, BUFFER_NUM> outQueueY;

    GlobalTensor<T_TYPE> xGm;
    GlobalTensor<T_TYPE> yGm;

    uint32_t aLength;
    uint32_t rLength;
    uint32_t aLengthAlign;
    uint32_t rLengthAlign;
    bool srcInnerPad;
};

template <
    typename T, int32_t xExent, int32_t yExent, uint32_t pattern, bool isReuseSource, uint32_t aLength,
    uint32_t rLength, bool srcInnerPad>
__aicore__ inline void MainReduceOpTest(GM_ADDR x, GM_ADDR y)
{
    typedef ReduceOpsType<T, xExent> xType;
    typedef ReduceOpsType<T, yExent> yType;
    KernelReduceOps<xType, yType, pattern, isReuseSource> op;
    op.Init(x, y, aLength, rLength, srcInnerPad);
    op.Process();
}

struct ReduceOpTestParams {
    void (*cal_func)(uint8_t*, uint8_t*);
};

class ReduceOpTestsuite : public testing::Test, public testing::WithParamInterface<ReduceOpTestParams> {};

INSTANTIATE_TEST_CASE_P(
    TEST_ReduceOp, ReduceOpTestsuite,
    ::testing::Values(
        ReduceOpTestParams{MainReduceOpTest<float, 64, 2, 0, false, 2, 32, false>},
        ReduceOpTestParams{MainReduceOpTest<float, 512, 2, 0, false, 2, 256, false>},
        ReduceOpTestParams{MainReduceOpTest<float, 576, 2, 0, false, 2, 288, false>},
        ReduceOpTestParams{MainReduceOpTest<float, 2416, 2, 0, false, 2, 1208, false>},
        ReduceOpTestParams{MainReduceOpTest<float, 256, 2, 0, false, 2, 128, false>},
        ReduceOpTestParams{MainReduceOpTest<float, 62, 2, 0, false, 2, 31, false>},
        ReduceOpTestParams{MainReduceOpTest<float, 254, 2, 0, false, 2, 127, false>},
        ReduceOpTestParams{MainReduceOpTest<float, 514, 2, 0, false, 2, 257, false>},
        ReduceOpTestParams{MainReduceOpTest<float, 578, 2, 0, false, 2, 289, false>},
        ReduceOpTestParams{MainReduceOpTest<float, 2418, 2, 0, false, 2, 1209, false>},
        ReduceOpTestParams{MainReduceOpTest<int32_t, 256, 2, 0, false, 2, 128, false>},
        ReduceOpTestParams{MainReduceOpTest<int32_t, 62, 2, 0, false, 2, 31, false>},
        ReduceOpTestParams{MainReduceOpTest<int32_t, 254, 2, 0, false, 2, 127, false>},
        ReduceOpTestParams{MainReduceOpTest<float, 256, 2, 0, true, 2, 128, false>},
        ReduceOpTestParams{MainReduceOpTest<int32_t, 256, 2, 0, true, 2, 128, false>},
        ReduceOpTestParams{MainReduceOpTest<uint32_t, 256, 2, 0, true, 2, 128, false>},
        ReduceOpTestParams{MainReduceOpTest<int64_t, 256, 2, 0, true, 2, 128, false>},
        ReduceOpTestParams{MainReduceOpTest<uint64_t, 256, 2, 0, true, 2, 128, false>},
        ReduceOpTestParams{MainReduceOpTest<float, 4032, 64, 1, true, 64, 63, false>},
        ReduceOpTestParams{MainReduceOpTest<float, 1024, 64, 1, true, 64, 16, false>},
        ReduceOpTestParams{MainReduceOpTest<float, 8192, 64, 1, true, 64, 128, false>},
        ReduceOpTestParams{MainReduceOpTest<float, 8192, 8, 1, true, 8, 1024, false>},
        ReduceOpTestParams{MainReduceOpTest<float, 4095, 65, 1, true, 65, 63, false>},
        ReduceOpTestParams{MainReduceOpTest<float, 1040, 65, 1, true, 65, 16, false>},
        ReduceOpTestParams{MainReduceOpTest<float, 8320, 65, 1, true, 65, 128, false>},
        ReduceOpTestParams{MainReduceOpTest<float, 7168, 7, 1, true, 7, 1024, false>},
        ReduceOpTestParams{MainReduceOpTest<int32_t, 4032, 64, 1, true, 64, 63, false>},
        ReduceOpTestParams{MainReduceOpTest<uint32_t, 4032, 64, 1, true, 64, 63, false>},
        ReduceOpTestParams{MainReduceOpTest<int64_t, 4032, 64, 1, true, 64, 63, false>},
        ReduceOpTestParams{MainReduceOpTest<int64_t, 1024, 64, 1, true, 64, 16, false>},
        ReduceOpTestParams{MainReduceOpTest<int64_t, 8192, 64, 1, true, 64, 128, false>},
        ReduceOpTestParams{MainReduceOpTest<int64_t, 8192, 8, 1, true, 8, 1024, false>},
        ReduceOpTestParams{MainReduceOpTest<int64_t, 4095, 65, 1, true, 65, 63, false>},
        ReduceOpTestParams{MainReduceOpTest<int64_t, 1040, 65, 1, true, 65, 16, false>},
        ReduceOpTestParams{MainReduceOpTest<int64_t, 8320, 65, 1, true, 65, 128, false>},
        ReduceOpTestParams{MainReduceOpTest<int64_t, 2048, 2, 1, true, 2, 1024, false>},
        ReduceOpTestParams{MainReduceOpTest<uint64_t, 4032, 64, 1, true, 64, 63, false>}));

TEST_P(ReduceOpTestsuite, ReduceOpTestCase)
{
    auto param = GetParam();
    uint8_t x[40960] = {0};
    uint8_t y[40960] = {0};

    param.cal_func(x, y);
    for (int32_t i = 0; i < (sizeof(y) / sizeof(y[0])); i++) {
        EXPECT_EQ(y[i], 0x00);
    }
}
