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

template <typename srcType>
class KernelFastGelu {
public:
    __aicore__ inline KernelFastGelu() {}
    __aicore__ inline void Init(GM_ADDR srcGm, GM_ADDR dstGm, uint32_t inputSize)
    {
        dataSize = inputSize;

        srcGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ srcType*>(srcGm), dataSize);
        dstGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ srcType*>(dstGm), dataSize);

        pipe.InitBuffer(inQueueX, 1, dataSize * sizeof(srcType));
        pipe.InitBuffer(outQueue, 1, dataSize * sizeof(srcType));
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
        LocalTensor<srcType> srcLocal = inQueueX.AllocTensor<srcType>();

        DataCopy(srcLocal, srcGlobal, dataSize);

        inQueueX.EnQue(srcLocal);
    }
    __aicore__ inline void Compute()
    {
        LocalTensor<srcType> dstLocal = outQueue.AllocTensor<srcType>();

        LocalTensor<srcType> srcLocal = inQueueX.DeQue<srcType>();

        FasterGelu(dstLocal, srcLocal, dataSize);

        outQueue.EnQue<srcType>(dstLocal);

        inQueueX.FreeTensor(srcLocal);
    }
    __aicore__ inline void CopyOut()
    {
        LocalTensor<srcType> dstLocal = outQueue.DeQue<srcType>();
        DataCopy(dstGlobal, dstLocal, dataSize);
        outQueue.FreeTensor(dstLocal);
    }

private:
    GlobalTensor<srcType> srcGlobal;
    GlobalTensor<srcType> dstGlobal;

    TPipe pipe;
    TQue<TPosition::VECIN, 1> inQueueX;
    TQue<TPosition::VECOUT, 1> outQueue;

    uint32_t dataSize = 0;
};

template <typename dataType>
__aicore__ void main_FastGelu_test(GM_ADDR srcGm, GM_ADDR dstGm, uint32_t dataSize)
{
    KernelFastGelu<dataType> op;
    op.Init(srcGm, dstGm, dataSize);
    op.Process();
}

struct FastGeluTestParams {
    uint32_t dataSize;
    uint32_t dataTypeSize;
    void (*calFunc)(uint8_t*, uint8_t*, uint32_t);
};

class FastGeluTestSuite : public testing::Test, public testing::WithParamInterface<FastGeluTestParams> {
protected:
    static void SetUpTestCase() { std::cout << "FastGeluTestSuite SetUpTestCase" << std::endl; }
    static void TearDownTestCase() { std::cout << "FastGeluTestSuite TearDownTestCase" << std::endl; }
    virtual void SetUp() { AscendC::SetGCoreType(2); }
    virtual void TearDown() { AscendC::SetGCoreType(0); }
};

INSTANTIATE_TEST_CASE_P(
    TEST_PACKAGE_FastGelu, FastGeluTestSuite,
    ::testing::Values(
        FastGeluTestParams{32, sizeof(half), main_FastGelu_test<half>},
        FastGeluTestParams{64, sizeof(half), main_FastGelu_test<half>},
        FastGeluTestParams{256, sizeof(half), main_FastGelu_test<half>},
        FastGeluTestParams{512, sizeof(half), main_FastGelu_test<half>},
        FastGeluTestParams{1024, sizeof(half), main_FastGelu_test<half>},
        FastGeluTestParams{2048, sizeof(half), main_FastGelu_test<half>},
        FastGeluTestParams{4096, sizeof(half), main_FastGelu_test<half>},
        FastGeluTestParams{8192, sizeof(half), main_FastGelu_test<half>},
        FastGeluTestParams{16384, sizeof(half), main_FastGelu_test<half>},
        FastGeluTestParams{32, sizeof(float), main_FastGelu_test<float>},
        FastGeluTestParams{64, sizeof(float), main_FastGelu_test<float>},
        FastGeluTestParams{256, sizeof(float), main_FastGelu_test<float>},
        FastGeluTestParams{512, sizeof(float), main_FastGelu_test<float>},
        FastGeluTestParams{1024, sizeof(float), main_FastGelu_test<float>},
        FastGeluTestParams{2048, sizeof(float), main_FastGelu_test<float>},
        FastGeluTestParams{4096, sizeof(float), main_FastGelu_test<float>},
        FastGeluTestParams{8192, sizeof(float), main_FastGelu_test<float>}
#if !(defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)) // 用例超出kirinx90 9030 ub size
        ,
        FastGeluTestParams{32768, sizeof(half), main_FastGelu_test<half>},
        FastGeluTestParams{16384, sizeof(float), main_FastGelu_test<float>}
#endif
        ));

TEST_P(FastGeluTestSuite, FastGeluTestCase)
{
    auto param = GetParam();

    uint8_t inputGm[param.dataSize * param.dataTypeSize]{0x00};
    uint8_t outputGm[param.dataSize * param.dataTypeSize]{0x00};

    param.calFunc(inputGm, outputGm, param.dataSize);

    for (int32_t i = 0; i < param.dataSize; i++) {
        EXPECT_EQ(outputGm[i], 0x00);
    }
}
