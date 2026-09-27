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
#include "op_common.h"

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

TEST_F(AicpuAllocCtxResTest, UnfoldContextIsSharedAcrossAlgorithms)
{
    OpParam first{};
    strcpy_s(first.commName, sizeof(first.commName), "comm");
    strcpy_s(first.algTag, sizeof(first.algTag), "allgather");
    ASSERT_EQ(SaveUnfoldThreadInfo(comm_, first, 123U), HCCL_SUCCESS);
    OpParam second = first;
    strcpy_s(second.algTag, sizeof(second.algTag), "allreduce");
    ThreadHandle thread = 0;
    EXPECT_EQ(GetUnfoldThreadInfo(comm_, second, thread), HCCL_SUCCESS);
    EXPECT_EQ(thread, 123U);
    void* ctx = nullptr;
    uint64_t size = 0;
    EXPECT_EQ(HcclEngineCtxGet(comm_, "comm_unfold", COMM_ENGINE_CPU_TS, &ctx, &size), HCCL_SUCCESS);
    EXPECT_EQ(size, sizeof(ThreadHandle));
    EXPECT_EQ(HcclEngineCtxGet(comm_, "allgather_unfold", COMM_ENGINE_CPU_TS, &ctx, &size), HCCL_E_NOT_FOUND);
}

TEST_F(AicpuAllocCtxResTest, ExistingUnfoldThreadIsNotOverwrittenByResourceAllocation)
{
    OpParam param{};
    param.engine = COMM_ENGINE_AICPU_TS;
    strcpy_s(param.commName, sizeof(param.commName), "comm");
    strcpy_s(param.algTag, sizeof(param.algTag), "allgather");
    ASSERT_EQ(SaveUnfoldThreadInfo(comm_, param, 123U), HCCL_SUCCESS);
    AlgResourceRequest request{};
    auto resources = std::make_unique<AlgResourceCtxSerializable>();
    ASSERT_EQ(HcclGetThread(comm_, param, request, resources), HCCL_SUCCESS);
    EXPECT_EQ(resources->unfoldThread, 123U);
}

TEST_F(AicpuAllocCtxResTest, MalformedUnfoldContextDoesNotTriggerReallocation)
{
    OpParam param{};
    param.engine = COMM_ENGINE_AICPU_TS;
    strcpy_s(param.commName, sizeof(param.commName), "comm");
    strcpy_s(param.algTag, sizeof(param.algTag), "allgather");
    ThreadHandle thread = 0;
    EXPECT_EQ(GetUnfoldThreadInfo(comm_, param, thread), HCCL_E_NOT_FOUND);
    void* ctx = nullptr;
    ASSERT_EQ(HcclEngineCtxCreate(comm_, "comm_unfold", COMM_ENGINE_CPU_TS, 1U, &ctx), HCCL_SUCCESS);
    EXPECT_EQ(GetUnfoldThreadInfo(comm_, param, thread), HCCL_E_PARA);
    AlgResourceRequest request{};
    auto resources = std::make_unique<AlgResourceCtxSerializable>();
    EXPECT_EQ(HcclGetThread(comm_, param, request, resources), HCCL_E_PARA);
    EXPECT_EQ(SaveUnfoldThreadInfo(comm_, param, 0U), HCCL_E_PARA);
}

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
