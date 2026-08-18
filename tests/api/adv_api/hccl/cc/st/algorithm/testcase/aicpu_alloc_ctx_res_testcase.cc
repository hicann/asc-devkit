/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "gtest/gtest.h"

#include "sim_communicator.h"
#include "sim_world.h"
#include "hccl_alloc_ctx_res.h"

namespace mc2_ops_hccl {
extern bool g_rejectDirectAclrtMemcpy;
extern uint32_t g_aclrtMemcpyCallCount;
extern uint32_t g_hcclEngineCtxCopyCallCount;
} // namespace mc2_ops_hccl

namespace checker {
namespace {
using namespace HcclSim;
using namespace mc2_ops_hccl;

class AicpuAllocCtxResTest : public testing::Test {
protected:
    void SetUp() override
    {
        const TopoMeta topoMeta{{{0U}}};
        SimWorld::Global()->Init(topoMeta, DevType::DEV_TYPE_950);
        ASSERT_EQ(Sim_HcclCommInitClusterInfo(topoMeta, 0U, &comm_), HCCL_SUCCESS);
        ASSERT_NE(comm_, nullptr);
        g_rejectDirectAclrtMemcpy = false;
        g_aclrtMemcpyCallCount = 0U;
        g_hcclEngineCtxCopyCallCount = 0U;
    }

    void TearDown() override
    {
        g_rejectDirectAclrtMemcpy = false;
        g_aclrtMemcpyCallCount = 0U;
        g_hcclEngineCtxCopyCallCount = 0U;
        if (comm_ != nullptr) {
            EXPECT_EQ(HcclCommDestroy(comm_), HCCL_SUCCESS);
            comm_ = nullptr;
        }
        SimWorld::Global()->Deinit();
    }

    HcclComm comm_ = nullptr;
};

TEST_F(AicpuAllocCtxResTest, CaptureSafeCopyCoversOpParamAndOpResCtx)
{
    constexpr uint32_t tilingOffset = 64U;
    Mc2InitTilingInner initTiling{};
    initTiling.offset[0] = tilingOffset;

    Mc2CcTilingInner ccTiling{};
    ccTiling.commEngine = static_cast<uint8_t>(OpExecuteConfig::AICPU_TS);
    const void* ccTilingList[MAX_CC_TILING_NUM] = {&ccTiling};

    OpParam opParam{};
    opParam.opType = HcclCMDType::HCCL_CMD_ALLGATHER;
    const std::vector<OpParam> opParams{opParam};

    g_rejectDirectAclrtMemcpy = true;
    void* opResCtxPtr = nullptr;
    ASSERT_EQ(
        HcclAllocOpResCtx(comm_, "aicpu_capture_safe_copy", opParams, &initTiling, ccTilingList, &opResCtxPtr),
        HCCL_SUCCESS);

    EXPECT_EQ(g_aclrtMemcpyCallCount, 0U);
    EXPECT_EQ(g_hcclEngineCtxCopyCallCount, 2U);
    ASSERT_NE(opResCtxPtr, nullptr);

    const auto* opResCtx = static_cast<const OpResCtx*>(opResCtxPtr);
    EXPECT_EQ(opResCtx->rankId, 0U);
    EXPECT_EQ(opResCtx->rankSize, 1U);
    EXPECT_EQ(opResCtx->algInfo[0].offset, tilingOffset);
    ASSERT_NE(opResCtx->algInfo[0].opParam, 0U);

    const auto* copiedOpParam = reinterpret_cast<const OpParam*>(opResCtx->algInfo[0].opParam);
    EXPECT_EQ(copiedOpParam->opType, HcclCMDType::HCCL_CMD_ALLGATHER);
}

} // namespace
} // namespace checker
