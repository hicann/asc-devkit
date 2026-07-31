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
#include "kernel_utils.h"
#include <math.h>
#include <iostream>
#include <fstream>

using namespace std;
using namespace AscendC;
constexpr SoftmaxConfig config = {true, 16, 16};
constexpr SoftmaxConfig softmaxOutputWithoutBrc = {false, 0, 0, SoftmaxMode::SOFTMAX_OUTPUT_WITHOUT_BRC};
namespace AscendC {
template <
    typename T1, typename T2, bool isBasicBlock = false, bool isDataFormatNZ = false,
    const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
class KernelSoftmax {
public:
    __aicore__ inline KernelSoftmax() {}
    __aicore__ inline void Init(
        __gm__ uint8_t* src0Gm, __gm__ uint8_t* src1Gm, __gm__ uint8_t* dstGm, uint32_t inheight, uint32_t inwidth)
    {
        height = inheight;
        width = inwidth;
        int elementNumPerBlk = 32 / sizeof(T2); // half=16 float=8
        src1Global.SetGlobalBuffer((__gm__ T1*)src0Gm);
        src2Global.SetGlobalBuffer((__gm__ T1*)src1Gm);
        dstGlobal.SetGlobalBuffer((__gm__ T1*)dstGm);
        pipe.InitBuffer(inQueueSrc1, 1, height * width * sizeof(T1));
        pipe.InitBuffer(inQueueSrc2, 1, height * width * sizeof(T1));
        pipe.InitBuffer(outQueueDst, 1, height * width * sizeof(T1));
        pipe.InitBuffer(inMaxQueue, 1, height * elementNumPerBlk * sizeof(T2));
        pipe.InitBuffer(outMaxQueue, 1, height * elementNumPerBlk * sizeof(T2));
        pipe.InitBuffer(inSumQueue, 1, height * elementNumPerBlk * sizeof(T2));
        elementNumPerBlk = 32 / sizeof(T1); // half=16 float=8
        pipe.InitBuffer(expMaxQueue, 1, height * elementNumPerBlk * sizeof(T1));
        pipe.InitBuffer(logSumQueue, 1, height * 8 * sizeof(float));
        height = inheight;
        width = inwidth;
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
        LocalTensor<T1> srcLocal1 = inQueueSrc1.AllocTensor<T1>();
        LocalTensor<T1> srcLocal2 = inQueueSrc2.AllocTensor<T1>();
        DataCopy(srcLocal1, src1Global, height * width);
        DataCopy(srcLocal2, src2Global, height * width);
        inQueueSrc1.EnQue(srcLocal1);
        inQueueSrc2.EnQue(srcLocal2);
    }
    __aicore__ inline void Compute()
    {
        uint32_t elementNumPerBlk = 32 / sizeof(T2); // half=16 float=8
        LocalTensor<T1> srcLocal1 = inQueueSrc1.DeQue<T1>();
        LocalTensor<T1> srcLocal2 = inQueueSrc2.DeQue<T1>();

        LocalTensor<T2> inmaxLocal = inMaxQueue.AllocTensor<T2>();
        LocalTensor<T2> outMaxLocal = outMaxQueue.AllocTensor<T2>();
        LocalTensor<T2> insumLocal = inSumQueue.AllocTensor<T2>();
        LocalTensor<T1> expMaxTensor = expMaxQueue.AllocTensor<T1>();
        LocalTensor<float> logSumLocal = logSumQueue.AllocTensor<float>();

        LocalTensor<T1> dstLocal = outQueueDst.AllocTensor<T1>();
        const int shapeDim = 2;
        uint32_t array[2] = {height, width};

        srcLocal1.SetShapeInfo(ShapeInfo(shapeDim, array)); // default ND
        srcLocal2.SetShapeInfo(ShapeInfo(shapeDim, array)); // default ND
        dstLocal.SetShapeInfo(ShapeInfo(shapeDim, array));
        SoftMaxShapeInfo srcShape = {height, width, height, width};
        SoftMaxTiling tiling;
        SoftMax<T1, false, isBasicBlock>(srcLocal1, insumLocal, inmaxLocal, srcLocal1, tiling, srcShape);
        SoftMax<T1, false, isBasicBlock, false, config>(srcLocal1, insumLocal, inmaxLocal, srcLocal1, tiling, srcShape);
        SimpleSoftMax<T1, false>(dstLocal, insumLocal, inmaxLocal, srcLocal1, tiling, srcShape);
        SimpleSoftMax<T1, false, false, false, config>(dstLocal, insumLocal, inmaxLocal, srcLocal1, tiling, srcShape);

        SoftmaxFlash<T1, false, isBasicBlock>(
            dstLocal, insumLocal, inmaxLocal, srcLocal1, expMaxTensor, insumLocal, inmaxLocal, tiling, false, srcShape);
        SoftmaxFlash<T1, false, isBasicBlock>(
            dstLocal, insumLocal, inmaxLocal, srcLocal2, expMaxTensor, insumLocal, inmaxLocal, tiling, true, srcShape);

        SoftmaxFlash<T1, false, isBasicBlock>(
            dstLocal, insumLocal, inmaxLocal, srcLocal1, expMaxTensor, insumLocal, inmaxLocal, tiling, false);
        SoftmaxFlash<T1, false, isBasicBlock>(
            dstLocal, insumLocal, inmaxLocal, srcLocal2, expMaxTensor, insumLocal, inmaxLocal, tiling, true);

        SoftMaxTiling gradTiling;
        SoftmaxGrad<T1, false>(dstLocal, srcLocal2, srcLocal1, gradTiling, true, srcShape);
        SoftmaxGrad<T1, false>(dstLocal, srcLocal2, srcLocal1, gradTiling, false, srcShape);

        SoftMaxTiling flashv2Tiling;
        SoftmaxFlashV2<T1, false, true, isBasicBlock, isDataFormatNZ>(
            dstLocal, insumLocal, inmaxLocal, srcLocal1, expMaxTensor, insumLocal, inmaxLocal, flashv2Tiling, srcShape);
        SoftmaxFlashV2<T1, true, true, isBasicBlock, isDataFormatNZ>(
            dstLocal, insumLocal, inmaxLocal, srcLocal2, expMaxTensor, insumLocal, inmaxLocal, flashv2Tiling, srcShape);
        SoftmaxFlashV2<T1, false, true, isBasicBlock, isDataFormatNZ, config>(
            dstLocal, insumLocal, inmaxLocal, srcLocal1, expMaxTensor, insumLocal, inmaxLocal, flashv2Tiling, srcShape);
        SoftmaxFlashV2<T1, true, true, isBasicBlock, isDataFormatNZ, config>(
            dstLocal, insumLocal, inmaxLocal, srcLocal2, expMaxTensor, insumLocal, inmaxLocal, flashv2Tiling, srcShape);
        if constexpr (std::is_same<T1, T2>::value) {
            SoftmaxFlashV2<T1, false, true, isBasicBlock, isDataFormatNZ, config>(
                dstLocal, outMaxLocal, insumLocal, inmaxLocal, srcLocal1, expMaxTensor, insumLocal, inmaxLocal,
                flashv2Tiling, srcShape);
            SoftmaxFlashV2<T1, true, true, isBasicBlock, isDataFormatNZ, config>(
                dstLocal, outMaxLocal, insumLocal, inmaxLocal, srcLocal2, expMaxTensor, insumLocal, inmaxLocal,
                flashv2Tiling, srcShape);
        }
        outQueueDst.EnQue<T1>(dstLocal);
        inMaxQueue.FreeTensor(inmaxLocal);
        outMaxQueue.FreeTensor(outMaxLocal);
        inSumQueue.FreeTensor(insumLocal);
        inQueueSrc1.FreeTensor(srcLocal1);
        inQueueSrc2.FreeTensor(srcLocal2);
        expMaxQueue.FreeTensor(expMaxTensor);
        logSumQueue.FreeTensor(logSumLocal);
    }
    __aicore__ inline void CopyOut()
    {
        LocalTensor<T1> dstLocal = outQueueDst.DeQue<T1>();
        DataCopy(dstGlobal, dstLocal, height * width);
        outQueueDst.FreeTensor(dstLocal);
    }

private:
    TPipe pipe;
    TQue<TPosition::VECIN, 1> inQueueSrc1;
    TQue<TPosition::VECIN, 1> inQueueSrc2;
    TQue<TPosition::VECIN, 1> inMaxQueue;
    TQue<TPosition::VECIN, 1> inSumQueue;
    TQue<TPosition::VECIN, 1> expMaxQueue;
    TQue<TPosition::VECIN, 1> logSumQueue;

    TQue<TPosition::VECOUT, 1> outMaxQueue;
    TQue<TPosition::VECOUT, 1> outQueueDst;
    GlobalTensor<T1> src1Global, dstGlobal;
    GlobalTensor<T1> src2Global;
    uint32_t height = 16;
    uint32_t width = 16;
};
} // namespace AscendC

template <
    typename T1, typename T2, bool isBasicBlock = false, bool isDataFormatNZ = false,
    const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__global__ __aicore__ void MainSoftmax(
    __gm__ uint8_t* dstGm, __gm__ uint8_t* src0Gm, __gm__ uint8_t* src1Gm, uint32_t height, uint32_t width)
{
    AscendC::KernelSoftmax<T1, T2, isBasicBlock, isDataFormatNZ, config> op;
    op.Init(src0Gm, src1Gm, dstGm, height, width);
    op.Process();
}

struct SoftMaxTestParams {
    uint32_t typeSize;
    uint32_t height;
    uint32_t width;
    void (*cal_func)(uint8_t*, uint8_t*, uint8_t*, uint32_t, uint32_t);
};

class SoftMaxTestsuite : public testing::Test, public testing::WithParamInterface<SoftMaxTestParams> {
protected:
    void SetUp() { AscendC::SetGCoreType(2); }

    void TearDown() { AscendC::SetGCoreType(0); }
};

INSTANTIATE_TEST_CASE_P(
    TEST_OPEARATION_SOFTMAX, SoftMaxTestsuite,
    ::testing::Values(
        SoftMaxTestParams{2, 8, 880, MainSoftmax<half, half>}, SoftMaxTestParams{2, 50, 144, MainSoftmax<half, half>},
        SoftMaxTestParams{2, 50, 288, MainSoftmax<half, float>},
        SoftMaxTestParams{4, 100, 32, MainSoftmax<float, float>},
#if !(defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)) // 用例超出kirinx90 9030 ub size
        SoftMaxTestParams{4, 50, 288, MainSoftmax<float, float>},
        SoftMaxTestParams{4, 16, 960, MainSoftmax<float, float, true>},
        SoftMaxTestParams{2, 100, 256, MainSoftmax<half, half, false>},
#endif
        SoftMaxTestParams{4, 1, 512, MainSoftmax<float, float>},
        SoftMaxTestParams{4, 8, 880, MainSoftmax<float, float>},
        SoftMaxTestParams{2, 8, 1024, MainSoftmax<half, half, true>},
        SoftMaxTestParams{2, 8, 960, MainSoftmax<half, half, true>},
        SoftMaxTestParams{2, 1, 16, MainSoftmax<half, half, false>},
        SoftMaxTestParams{2, 64, 128, MainSoftmax<half, float, true>},
        SoftMaxTestParams{4, 64, 128, MainSoftmax<float, float, true>},
        SoftMaxTestParams{4, 50, 144, MainSoftmax<float, float, false>},
        SoftMaxTestParams{4, 8, 1024, MainSoftmax<float, float, true>},
        SoftMaxTestParams{2, 64, 128, MainSoftmax<half, float, true, true>},
        SoftMaxTestParams{4, 64, 128, MainSoftmax<float, float, true, true>},
        SoftMaxTestParams{4, 50, 144, MainSoftmax<float, float, false, false, config>},
        SoftMaxTestParams{2, 50, 144, MainSoftmax<half, half, false, false, softmaxOutputWithoutBrc>},
        SoftMaxTestParams{2, 50, 144, MainSoftmax<half, float, false, false, softmaxOutputWithoutBrc>},
        SoftMaxTestParams{4, 50, 144, MainSoftmax<float, float, false, false, softmaxOutputWithoutBrc>},
        SoftMaxTestParams{2, 50, 144, MainSoftmax<half, half, false, false, softmaxOutputWithoutBrc>},
        SoftMaxTestParams{2, 50, 144, MainSoftmax<half, float, false, false, softmaxOutputWithoutBrc>},
        SoftMaxTestParams{4, 50, 144, MainSoftmax<float, float, false, false, softmaxOutputWithoutBrc>}));
TEST_P(SoftMaxTestsuite, SoftMaxOpTestCase)
{
    auto param = GetParam();
    uint8_t src0Gm[param.height * param.width * param.typeSize];
    uint8_t src1Gm[param.height * param.width * param.typeSize];
    uint8_t dstGm[param.height * param.width * param.typeSize];
    param.cal_func(dstGm, src0Gm, src1Gm, param.height, param.width);
}
