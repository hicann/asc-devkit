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
#ifdef ASCENDC_CPU_DEBUG
#include "impl/adv_api/detail/api_check/kernel_check/math/where/where_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "impl/adv_api/detail/api_check/kernel_api_check.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
class WhereAPICheck : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() { AscendC::KernelRaise::GetInstance().SetRaiseMode(false); }
    void TearDown() { AscendC::KernelRaise::GetInstance().SetRaiseMode(true); }
};

TEST_F(WhereAPICheck, WhereAPICheckTestSuccess)
{
    AscendC::TPipe m_pipe;
    const uint32_t count = 32;

    AscendC::TQue<AscendC::TPosition::VECIN, 1> inQueueX;
    m_pipe.InitBuffer(inQueueX, 1, count * sizeof(float));
    AscendC::LocalTensor<float> src0Tensor = inQueueX.AllocTensor<float>();
    src0Tensor.SetSize(count);

    AscendC::TQue<AscendC::TPosition::VECIN, 1> inQueueY;
    m_pipe.InitBuffer(inQueueY, 1, count * sizeof(float));
    AscendC::LocalTensor<float> src1Tensor = inQueueY.AllocTensor<float>();
    src1Tensor.SetSize(count);

    AscendC::TQue<AscendC::TPosition::VECIN, 1> inQueueZ;
    m_pipe.InitBuffer(inQueueZ, 1, count * sizeof(bool));
    AscendC::LocalTensor<bool> conditionTensor = inQueueZ.AllocTensor<bool>();
    conditionTensor.SetSize(count);

    AscendC::TQue<AscendC::TPosition::VECOUT, 1> outQueue;
    m_pipe.InitBuffer(outQueue, 1, count * sizeof(float));
    AscendC::LocalTensor<float> dstTensor = outQueue.AllocTensor<float>();
    dstTensor.SetSize(count);

    uint8_t calCount = 16;
    uint64_t startCounts = AscendC::KernelRaise::GetInstance().GetRaiseCount();
    AscendC::CHECK_FUNC_HIGHLEVEL_API(
        Where, (float, AscendC::LocalTensor<float>, AscendC::LocalTensor<float>, bool),
        (dstTensor, src0Tensor, src1Tensor, conditionTensor, calCount));
    const uint32_t res = 0;
    EXPECT_EQ(AscendC::KernelRaise::GetInstance().GetRaiseCount() - startCounts, res);
}

TEST_F(WhereAPICheck, WhereAPICheckTestCountFailure)
{
    AscendC::TPipe m_pipe;

    const uint32_t count = 32;
    float src0 = 0;
    float src1 = 1;

    AscendC::TQue<AscendC::TPosition::VECIN, 1> inQueueZ;
    m_pipe.InitBuffer(inQueueZ, 1, count * sizeof(bool));
    AscendC::LocalTensor<bool> conditionTensor = inQueueZ.AllocTensor<bool>();
    conditionTensor.SetSize(count);

    AscendC::TQue<AscendC::TPosition::VECOUT, 1> outQueue;
    m_pipe.InitBuffer(outQueue, 1, count * sizeof(float));
    AscendC::LocalTensor<float> dstTensor = outQueue.AllocTensor<float>();
    dstTensor.SetSize(count);

    uint8_t calCount = 64;
    uint64_t startCounts = AscendC::KernelRaise::GetInstance().GetRaiseCount();
    AscendC::CHECK_FUNC_HIGHLEVEL_API(
        Where, (float, float, float, bool), (dstTensor, src0, src1, conditionTensor, calCount));
    const uint32_t res = 2;
    EXPECT_EQ(AscendC::KernelRaise::GetInstance().GetRaiseCount() - startCounts, res);
}

TEST_F(WhereAPICheck, WhereAPICheckTestPosFailure)
{
    AscendC::TPipe m_pipe;

    const uint32_t count = 32;
    float src0 = 0;
    float src1 = 1;

    AscendC::TQue<AscendC::TPosition::VECIN, 1> inQueueZ;
    m_pipe.InitBuffer(inQueueZ, 1, count * sizeof(bool));
    AscendC::LocalTensor<bool> conditionTensor = inQueueZ.AllocTensor<bool>();
    conditionTensor.SetSize(count);

    AscendC::TQue<AscendC::TPosition::A1, 1> outQueue;
    m_pipe.InitBuffer(outQueue, 1, count * sizeof(float));
    AscendC::LocalTensor<float> dstTensor = outQueue.AllocTensor<float>();
    dstTensor.SetSize(count);

    uint8_t calCount = 23;
    uint64_t startCounts = AscendC::KernelRaise::GetInstance().GetRaiseCount();
    AscendC::CHECK_FUNC_HIGHLEVEL_API(
        Where, (float, float, float, bool), (dstTensor, src0, src1, conditionTensor, calCount));
    const uint32_t res = 1;
    EXPECT_EQ(AscendC::KernelRaise::GetInstance().GetRaiseCount() - startCounts, res);
}
#endif
