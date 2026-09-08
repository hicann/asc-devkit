/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <gtest/gtest.h>
#include "kernel_operator.h"
#define CANN_ASC_USE_EXPERIMENTAL
#include "adv_api/experimental/ndtri.h"
#ifdef ASCENDC_CPU_DEBUG
#include "impl/adv_api/detail/experimental/api_check/kernel_check/math/ndtri/ndtri_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "impl/adv_api/detail/api_check/kernel_api_check.h"

class NdtriApiCheck : public testing::Test {
protected:
    void SetUp() override { AscendC::KernelRaise::GetInstance().SetRaiseMode(false); }
    void TearDown() override { AscendC::KernelRaise::GetInstance().SetRaiseMode(true); }
};

TEST_F(NdtriApiCheck, ValidFloatArgumentsDoNotRaise)
{
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> dstQueue;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> srcQueue;
    pipe.InitBuffer(dstQueue, 1, 32 * sizeof(float));
    pipe.InitBuffer(srcQueue, 1, 32 * sizeof(float));

    AscendC::LocalTensor<float> dstTensor = dstQueue.AllocTensor<float>();
    AscendC::LocalTensor<float> srcTensor = srcQueue.AllocTensor<float>();
    dstTensor.SetSize(32);
    srcTensor.SetSize(32);

    const uint64_t startCount = AscendC::KernelRaise::GetInstance().GetRaiseCount();
    AscendC::experimental::CHECK_FUNC_HIGHLEVEL_API(Ndtri, (float, false), (dstTensor, srcTensor, 8));
    EXPECT_EQ(AscendC::KernelRaise::GetInstance().GetRaiseCount() - startCount, 0);
}

TEST_F(NdtriApiCheck, InPlaceArgumentsDoNotRaise)
{
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> queue;
    pipe.InitBuffer(queue, 1, 32 * sizeof(float));

    AscendC::LocalTensor<float> tensor = queue.AllocTensor<float>();
    tensor.SetSize(32);

    const uint64_t startCount = AscendC::KernelRaise::GetInstance().GetRaiseCount();
    AscendC::experimental::CHECK_FUNC_HIGHLEVEL_API(Ndtri, (float, false), (tensor, tensor, 8));
    EXPECT_EQ(AscendC::KernelRaise::GetInstance().GetRaiseCount() - startCount, 0);
}

TEST_F(NdtriApiCheck, InvalidTensorPositionRaisesExpectedCheck)
{
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::A1, 1> invalidQueue;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> validQueue;
    pipe.InitBuffer(invalidQueue, 1, 32 * sizeof(float));
    pipe.InitBuffer(validQueue, 1, 32 * sizeof(float));

    AscendC::LocalTensor<float> dstTensor = invalidQueue.AllocTensor<float>();
    AscendC::LocalTensor<float> srcTensor = validQueue.AllocTensor<float>();
    dstTensor.SetSize(32);
    srcTensor.SetSize(32);

    const uint64_t startCount = AscendC::KernelRaise::GetInstance().GetRaiseCount();
    AscendC::experimental::CHECK_FUNC_HIGHLEVEL_API(Ndtri, (float, false), (dstTensor, srcTensor, 8));
    EXPECT_EQ(AscendC::KernelRaise::GetInstance().GetRaiseCount() - startCount, 1);
}

TEST_F(NdtriApiCheck, InvalidTypeReuseCountAndAlignmentRaiseExpectedChecks)
{
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> queue;
    pipe.InitBuffer(queue, 1, 32 * sizeof(uint8_t));

    AscendC::LocalTensor<uint8_t> tensor = queue.AllocTensor<uint8_t>();
    tensor.SetSize(16);

    const uint64_t startCount = AscendC::KernelRaise::GetInstance().GetRaiseCount();
    AscendC::experimental::CHECK_FUNC_HIGHLEVEL_API(Ndtri, (uint8_t, true), (tensor[1], tensor[1], 32));
    EXPECT_EQ(AscendC::KernelRaise::GetInstance().GetRaiseCount() - startCount, 5);
}
