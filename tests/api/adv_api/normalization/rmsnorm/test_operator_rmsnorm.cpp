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

namespace test_rms_norm {
constexpr uint32_t BASIC_BLK_BSLENGTH = 8;
constexpr uint32_t MAX_REPEAT = 255;
constexpr uint32_t HALF_SIZE_IN_BYTE = 2;

inline __aicore__ uint32_t AlignToBlock(const uint32_t inputValue, const uint32_t typeSize)
{
    uint32_t alignUnit = ONE_BLK_SIZE / typeSize;
    return (inputValue + alignUnit - 1) / alignUnit * alignUnit;
}
__aicore__ bool GetRmsNormTilingInfo(
    const ShapeInfo& srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, RmsNormTiling& tiling,
    const bool isBasicBlock)
{
    tiling.bLength = srcShape.shape[0];
    tiling.sLength = srcShape.shape[1];
    tiling.hLength = srcShape.shape[2];
    // 3rd elem is original h-length
    tiling.originalHLength = srcShape.originalShape[2];
    tiling.reciprocalOfHLength = 1.0f / tiling.originalHLength;

    // calculate tail tiling info
    uint32_t oneTmpSize = stackBufferSize / sizeof(float);
    uint32_t alignBsLength = ONE_BLK_FLOAT_NUM;
    // for half need two tmp buffers
    uint32_t halfCoeff = (typeSize == sizeof(float) ? 1u : 2u);
    while (oneTmpSize > alignBsLength * tiling.hLength * halfCoeff + alignBsLength) {
        alignBsLength += ONE_BLK_FLOAT_NUM;
    }
    alignBsLength = alignBsLength == ONE_BLK_FLOAT_NUM ? ONE_BLK_FLOAT_NUM : alignBsLength - ONE_BLK_FLOAT_NUM;
    ASCENDC_ASSERT((alignBsLength > 0), { KERNEL_LOG(KERNEL_ERROR, "stackBufferSize is too small"); });
    oneTmpSize =
        (typeSize == HALF_SIZE_IN_BYTE) ? (oneTmpSize - alignBsLength) / halfCoeff : (oneTmpSize - alignBsLength);
    ASCENDC_ASSERT((oneTmpSize >= tiling.hLength), { KERNEL_LOG(KERNEL_ERROR, "stackBufferSize is too small"); });

    const uint32_t inputXSize = tiling.bLength * tiling.sLength * tiling.hLength;
    if (oneTmpSize > inputXSize) {
        oneTmpSize = inputXSize;
    }
    uint32_t bsLength = oneTmpSize / tiling.hLength;
    if (isBasicBlock) {
        // for basic block bsLength should be multiples of BASIC_BLK_BSLENGTH(8)
        bsLength = bsLength < BASIC_BLK_BSLENGTH ? 1 : bsLength / BASIC_BLK_BSLENGTH * BASIC_BLK_BSLENGTH;
    } else if (bsLength > MAX_REPEAT) {
        bsLength = MAX_REPEAT;
    }
    oneTmpSize = bsLength * tiling.hLength;

    tiling.mainBshLength = oneTmpSize;
    tiling.mainBsLength = oneTmpSize / tiling.hLength;
    tiling.mainBsLengthAlign = AlignToBlock(oneTmpSize / tiling.hLength, sizeof(float));
    tiling.loopRound = inputXSize / oneTmpSize;
    const uint32_t inputTailSize = inputXSize % oneTmpSize;
    tiling.tailBshLength = inputTailSize;
    tiling.inputTailPos = inputXSize - inputTailSize;
    tiling.tailBsLength = inputTailSize / tiling.hLength;
    return true;
}
template <typename dataType, bool isBasicBlock = false>
class KernelRmsNorm {
public:
    __aicore__ inline KernelRmsNorm() {}
    __aicore__ inline void Init(
        GM_ADDR inputGm, GM_ADDR gammaGm, GM_ADDR outputGm, const uint32_t bLength, const uint32_t sLength,
        const uint32_t inHLength)
    {
        hLength = AlignToBlock(inHLength, sizeof(dataType));
        dataType epsilon = 0.001;
        bshLength = bLength * sLength * hLength;
        inputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ dataType*>(inputGm), bshLength);
        gammaGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ dataType*>(gammaGm), hLength);
        outputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ dataType*>(outputGm), bshLength);
        constexpr uint32_t typeSize = sizeof(dataType);
        pipe.InitBuffer(inQueue, 1, bshLength * typeSize);
        pipe.InitBuffer(inQueueGamma, 1, hLength * typeSize);
        pipe.InitBuffer(outQueue, 1, bshLength * typeSize);

        bool res = PopStackBuffer<uint8_t, TPosition::LCM>(stackBuffer);
        const uint32_t stackSize = stackBuffer.GetSize();

        uint32_t inputShape[3] = {bLength, sLength, hLength};
        uint32_t originShape[3] = {bLength, sLength, inHLength};
        DataFormat dataFormat = DataFormat::ND;
        ShapeInfo shapeInfo{3, inputShape, 3, originShape, dataFormat};
        GetRmsNormTilingInfo(shapeInfo, stackSize / sizeof(float), typeSize, tiling, isBasicBlock);
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
        LocalTensor<dataType> inputLocal = inQueue.AllocTensor<dataType>();
        DataCopy(inputLocal, inputGlobal, bshLength);
        inQueue.EnQue(inputLocal);
        LocalTensor<dataType> gammaLocal = inQueueGamma.AllocTensor<dataType>();
        DataCopy(gammaLocal, gammaGlobal, hLength);
        inQueueGamma.EnQue(gammaLocal);
    }

    __aicore__ inline void Compute()
    {
        LocalTensor<dataType> inputLocal = inQueue.DeQue<dataType>();
        LocalTensor<dataType> gammaLocal = inQueueGamma.DeQue<dataType>();
        LocalTensor<dataType> outputLocal = outQueue.AllocTensor<dataType>();
        RmsNorm(outputLocal, inputLocal, gammaLocal, stackBuffer, epsilon, tiling);
        RmsNorm(outputLocal, inputLocal, gammaLocal, epsilon, tiling);

        inQueue.FreeTensor(inputLocal);
        inQueueGamma.FreeTensor(gammaLocal);
        outQueue.EnQue(outputLocal);
    }

    __aicore__ inline void CopyOut()
    {
        LocalTensor<dataType> outputLocal = outQueue.DeQue<dataType>();
        DataCopy(outputGlobal, outputLocal, bshLength);
        outQueue.FreeTensor(outputLocal);
    }

private:
    GlobalTensor<dataType> inputGlobal;
    GlobalTensor<dataType> gammaGlobal;
    GlobalTensor<dataType> outputGlobal;
    LocalTensor<uint8_t> stackBuffer;

    TPipe pipe;
    TQue<TPosition::VECIN, 1> inQueue;
    TQue<TPosition::VECIN, 1> inQueueGamma;
    TQue<TPosition::VECOUT, 1> outQueue;

    RmsNormTiling tiling;
    uint32_t hLength;
    dataType epsilon;
    uint32_t bshLength;
};
} // namespace test_rms_norm

template <typename dataType, bool isBasicBlock = false>
__aicore__ inline void main_rmsnorm_test(
    GM_ADDR inputGm, GM_ADDR gammaGm, GM_ADDR outputGm, uint32_t bLength, uint32_t sLength, uint32_t hLength)
{
    test_rms_norm::KernelRmsNorm<dataType, isBasicBlock> op;
    op.Init(inputGm, gammaGm, outputGm, bLength, sLength, hLength);
    op.Process();
}

struct RmsnormTestParams {
    uint32_t bLength;
    uint32_t sLength;
    uint32_t hLength;
    uint32_t typeSize;
    void (*cal_func)(uint8_t*, uint8_t*, uint8_t*, uint32_t, uint32_t, uint32_t);
};

class RmsnormTestSuite : public testing::Test, public testing::WithParamInterface<RmsnormTestParams> {
protected:
    static void SetUpTestCase() { std::cout << "RmsnormTestSuite SetUpTestCase" << std::endl; }
    static void TearDownTestCase() { std::cout << "RmsnormTestSuite TearDownTestCase" << std::endl; }
    virtual void SetUp() {}
    virtual void TearDown() {}
};

INSTANTIATE_TEST_CASE_P(
    TEST_PACKAGE_RMSNORM, RmsnormTestSuite,
    ::testing::Values(
        RmsnormTestParams{2, 32, 64, sizeof(half), main_rmsnorm_test<half, true>},
        RmsnormTestParams{4, 8, 512, sizeof(half), main_rmsnorm_test<half, false>},
        RmsnormTestParams{1, 7, 2048, sizeof(float), main_rmsnorm_test<float, true>},
        RmsnormTestParams{2, 256, 32, sizeof(float), main_rmsnorm_test<float, false>}));

TEST_P(RmsnormTestSuite, RmsnormTestCase)
{
    auto param = GetParam();

    uint32_t bLength = param.bLength;
    uint32_t sLength = param.sLength;
    uint32_t hLength = param.hLength;
    uint32_t typeSize = param.typeSize;

    uint32_t bshLength = bLength * sLength * hLength;
    uint32_t bsLength = bLength * sLength;

    uint8_t inputGm[bshLength * typeSize]{0x00};
    uint8_t gammGm[hLength * typeSize]{0x00};

    uint8_t outputGm[bshLength * typeSize]{0x00};

    param.cal_func(inputGm, gammGm, outputGm, bLength, sLength, hLength);

    for (int32_t i = 0; i < bshLength * typeSize; i++) {
        EXPECT_EQ(outputGm[i], 0x00);
    }
}
