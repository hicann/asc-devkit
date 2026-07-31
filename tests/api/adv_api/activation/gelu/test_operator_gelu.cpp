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
class KernelGelu {
public:
    __aicore__ inline KernelGelu() {}
    __aicore__ inline void Init(GM_ADDR src_gm, GM_ADDR dst_gm, uint32_t inputSize)
    {
        dataSize = inputSize;

        src_global.SetGlobalBuffer(reinterpret_cast<__gm__ srcType*>(src_gm), dataSize);
        dst_global.SetGlobalBuffer(reinterpret_cast<__gm__ srcType*>(dst_gm), dataSize);

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

        DataCopy(srcLocal, src_global, dataSize);

        inQueueX.EnQue(srcLocal);
    }
    __aicore__ inline void Compute()
    {
        LocalTensor<srcType> dstLocal = outQueue.AllocTensor<srcType>();

        LocalTensor<srcType> srcLocal = inQueueX.DeQue<srcType>();

        Gelu<srcType, true>(dstLocal, srcLocal, dataSize);

        outQueue.EnQue<srcType>(dstLocal);

        inQueueX.FreeTensor(srcLocal);
    }
    __aicore__ inline void CopyOut()
    {
        LocalTensor<srcType> dstLocal = outQueue.DeQue<srcType>();
        DataCopy(dst_global, dstLocal, dataSize);
        outQueue.FreeTensor(dstLocal);
    }

private:
    GlobalTensor<srcType> src_global;
    GlobalTensor<srcType> dst_global;

    TPipe pipe;
    TQue<TPosition::VECIN, 1> inQueueX;
    TQue<TPosition::VECOUT, 1> outQueue;

    uint32_t dataSize = 0;
};

template <typename dataType>
__aicore__ void main_Gelu_test(GM_ADDR srcGm, GM_ADDR dstGm, uint32_t dataSize)
{
    KernelGelu<dataType> op;
    op.Init(srcGm, dstGm, dataSize);
    op.Process();
}

struct GeluTestParams {
    uint32_t dataSize;
    uint32_t dataTypeSize;
    void (*calFunc)(uint8_t*, uint8_t*, uint32_t);
};

class GeluTestSuite : public testing::Test, public testing::WithParamInterface<GeluTestParams> {
protected:
    static void SetUpTestCase() { std::cout << "GeluTestSuite SetUpTestCase" << std::endl; }
    static void TearDownTestCase() { std::cout << "GeluTestSuite TearDownTestCase" << std::endl; }
    virtual void SetUp() { AscendC::SetGCoreType(2); }
    virtual void TearDown() { AscendC::SetGCoreType(0); }
};

INSTANTIATE_TEST_CASE_P(
    TEST_PACKAGE_Gelu, GeluTestSuite,
    ::testing::Values(
        GeluTestParams{32, sizeof(half), main_Gelu_test<half>}, GeluTestParams{64, sizeof(half), main_Gelu_test<half>},
        GeluTestParams{256, sizeof(half), main_Gelu_test<half>},
        GeluTestParams{512, sizeof(half), main_Gelu_test<half>},
        GeluTestParams{1024, sizeof(half), main_Gelu_test<half>},
        GeluTestParams{2048, sizeof(half), main_Gelu_test<half>},
        GeluTestParams{4096, sizeof(half), main_Gelu_test<half>},
        GeluTestParams{8192, sizeof(half), main_Gelu_test<half>},
        GeluTestParams{16384, sizeof(half), main_Gelu_test<half>},
        GeluTestParams{32, sizeof(float), main_Gelu_test<float>},
        GeluTestParams{64, sizeof(float), main_Gelu_test<float>},
        GeluTestParams{256, sizeof(float), main_Gelu_test<float>},
        GeluTestParams{512, sizeof(float), main_Gelu_test<float>},
        GeluTestParams{1024, sizeof(float), main_Gelu_test<float>},
        GeluTestParams{2048, sizeof(float), main_Gelu_test<float>},
        GeluTestParams{4096, sizeof(float), main_Gelu_test<float>},
        GeluTestParams{8192, sizeof(float), main_Gelu_test<float>}
#if !(defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)) // 用例超出kirinx90 9030 ub size
        ,
        GeluTestParams{32768, sizeof(half), main_Gelu_test<half>},
        GeluTestParams{16384, sizeof(float), main_Gelu_test<float>}
#endif
        ));

TEST_P(GeluTestSuite, GeluTestCase)
{
    auto param = GetParam();

    uint8_t inputGm[param.dataSize * param.dataTypeSize]{0x00};
    uint8_t outputGm[param.dataSize * param.dataTypeSize]{0x00};

    param.calFunc(inputGm, outputGm, param.dataSize);

    for (int32_t i = 0; i < param.dataSize; i++) {
        EXPECT_EQ(outputGm[i], 0x00);
    }
}
