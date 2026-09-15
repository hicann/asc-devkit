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
#include "impl/adv_api/detail/api_check/kernel_api_check.h"
#include "../reduce_case_common.h"

class ReduceAllAPICheck : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp()
    {
        AscendC::SetGCoreType(2);
        AscendC::KernelRaise::GetInstance().SetRaiseMode(false);
    }
    void TearDown()
    {
        AscendC::SetGCoreType(0);
        AscendC::KernelRaise::GetInstance().SetRaiseMode(true);
    }
};

TEST_F(ReduceAllAPICheck, ReduceAllAPICheckReduceAllSrcShapeLastAxis)
{
    CheckReduceSrcShapeLastAxis<uint8_t, HighLevelApiCheck::CheckFuncReduceAll<uint8_t, AscendC::Pattern::Reduce::AR>>(
        "ReduceAll");
}

TEST_F(ReduceAllAPICheck, ReduceAllAPICheckReduceAllSrcShapeArFirstAxis)
{
    CheckReduceSrcShapeArFirstAxis<float, HighLevelApiCheck::CheckFuncReduceAll<float, AscendC::Pattern::Reduce::AR>>(
        "ReduceAll");
}

TEST_F(ReduceAllAPICheck, ReduceAllAPICheckReduceAllSrcShapeRaLastAxis)
{
    CheckReduceSrcShapeRaLastAxis<float, HighLevelApiCheck::CheckFuncReduceAll<float, AscendC::Pattern::Reduce::AR>>(
        "ReduceAll");
}

TEST_F(ReduceAllAPICheck, ReduceAllAPICheckReduceAllSrcShapeSrcSize)
{
    CheckReduceSrcShapeSrcSize<float, HighLevelApiCheck::CheckFuncReduceAll<float, AscendC::Pattern::Reduce::RA>>(
        "ReduceAll");
}

TEST_F(ReduceAllAPICheck, ReduceAllAPICheckReduceAllSrcInnerPad)
{
    CheckReduceSrcInnerPad<float, HighLevelApiCheck::CheckFuncReduceAll<float, AscendC::Pattern::Reduce::RA>>(
        "ReduceAll");
}

TEST_F(ReduceAllAPICheck, ReduceAllAPICheckReduceAllSrcPos)
{
    CheckReduceSrcPos<float, HighLevelApiCheck::CheckFuncReduceAll<float, AscendC::Pattern::Reduce::RA>>("ReduceAll");
}

TEST_F(ReduceAllAPICheck, ReduceAllAPICheckReduceAllDstPos)
{
    CheckReduceDstPos<float, HighLevelApiCheck::CheckFuncReduceAll<float, AscendC::Pattern::Reduce::RA>>("ReduceAll");
}

TEST_F(ReduceAllAPICheck, ReduceAllAPICheckTmpPos)
{
    CheckReduceTmpPos<float, HighLevelApiCheck::CheckFuncReduceAll<float, AscendC::Pattern::Reduce::RA>>("ReduceAll");
}

TEST_F(ReduceAllAPICheck, ReduceAllAPICheckOverlap)
{
    CheckReduceOverlap<float, HighLevelApiCheck::CheckFuncReduceAll<float, AscendC::Pattern::Reduce::RA>>("ReduceAll");
}

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
TEST_F(ReduceAllAPICheck, ReduceAllAPICheckWidth32B)
{
    AscendC::TPipe pipe;
    AscendC::TQue<TPosition::VECIN, 1> inQueueX;
    AscendC::TQue<TPosition::VECOUT, 1> outQueueY;
    AscendC::TBuf<TPosition::VECCALC> tmplocalBuf;
    uint32_t first = 33;
    uint32_t last = 33;
    pipe.InitBuffer(outQueueY, 1, last * sizeof(float));
    pipe.InitBuffer(inQueueX, 1, first * last * sizeof(float));
    pipe.InitBuffer(tmplocalBuf, last);
    AscendC::LocalTensor<float> srcTensor = inQueueX.AllocTensor<float>();
    AscendC::LocalTensor<uint8_t> sharedTmpBuffer = tmplocalBuf.Get<uint8_t>();
    AscendC::LocalTensor<float> dstTensor = outQueueY.AllocTensor<float>();

    uint32_t srcShape[] = {first, last};
    constexpr bool isReuseSource = true;
    bool srcInnerPad = true;
    uint64_t startCounts = AscendC::KernelRaise::GetInstance().GetRaiseCount();
    HighLevelApiCheck::CheckFuncReduceAll<float, AscendC::Pattern::Reduce::RA>(
        "ReduceAll", dstTensor, srcTensor, sharedTmpBuffer, srcShape, srcInnerPad, last);
    inQueueX.FreeTensor(srcTensor);
    outQueueY.FreeTensor(dstTensor);
    tmplocalBuf.FreeTensor(sharedTmpBuffer);
    EXPECT_EQ(AscendC::KernelRaise::GetInstance().GetRaiseCount() - startCounts, 1);
}
#endif
