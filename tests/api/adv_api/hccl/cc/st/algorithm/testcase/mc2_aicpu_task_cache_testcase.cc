/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <array>
#include <cstring>
#include <limits>
#include <string>

#include "gtest/gtest.h"

#include "alg_env_config.h"
#include "hcomm_primitives_dl.h"
#include "task_cache/mc2_aicpu_task_cache.h"
#include "task_cache/mc2_aicpu_task_cache_runner.h"
#include "task_cache_test_stub.h"

namespace {
using namespace mc2_ops_hccl;

int32_t TaskCacheLookupStub(const char*, bool* isHit)
{
    auto& state = GetMc2TaskCacheTestStubState();
    ++state.lookupCalls;
    *isHit = state.lookupHit;
    return state.lookupRet;
}

int32_t TaskCacheStartStub(const char*, void**, uint64_t*, uint64_t)
{
    auto& state = GetMc2TaskCacheTestStubState();
    ++state.cacheStartCalls;
    return state.cacheStartRet;
}

int32_t TaskCacheEndStub(const char*)
{
    auto& state = GetMc2TaskCacheTestStubState();
    ++state.cacheEndCalls;
    return state.cacheEndRet;
}

int32_t TaskCacheExecuteStub(const char*, void**, uint64_t*, uint64_t)
{
    auto& state = GetMc2TaskCacheTestStubState();
    ++state.executeCalls;
    return state.executeRet;
}

int32_t TaskCacheClearStub(const char*)
{
    auto& state = GetMc2TaskCacheTestStubState();
    ++state.clearCalls;
    return state.clearRet;
}

struct TaskCacheInputs {
    OpParam param{};
    AlgResourceCtxSerializable resCtx{};
    std::array<uint64_t, 8> sendCounts{};
    std::array<uint64_t, 8> recvCounts{};
    std::array<uint64_t, 8> sendDispls{};
    std::array<uint64_t, 8> recvDispls{};

    void RefreshAllToAllPointers()
    {
        param.all2AllVDataDes.sendCounts = sendCounts.data();
        param.all2AllVDataDes.recvCounts = recvCounts.data();
        param.all2AllVDataDes.sdispls = sendDispls.data();
        param.all2AllVDataDes.rdispls = recvDispls.data();
    }
};

TaskCacheInputs MakeTaskCacheInputs(HcclCMDType opType)
{
    TaskCacheInputs inputs;
    inputs.param.hcclComm = reinterpret_cast<void*>(0x1000U);
    inputs.param.inputPtr = reinterpret_cast<void*>(0x100000U);
    inputs.param.outputPtr = reinterpret_cast<void*>(0x100000000ULL);
    inputs.param.opMode = OpMode::OPBASE;
    inputs.param.engine = CommEngine::COMM_ENGINE_AICPU_TS;
    inputs.param.opType = opType;
    inputs.param.DataDes.count = 871U;
    inputs.param.DataDes.dataType = HCCL_DATA_TYPE_FP32;
    inputs.param.DataDes.outputType = HCCL_DATA_TYPE_FP32;
    inputs.param.DataDes.strideCount = 6269244U;
    std::strncpy(inputs.param.algName, "InsTestExecutor", sizeof(inputs.param.algName) - 1U);
    std::strncpy(inputs.param.algTag, "mc2-task-cache-ut", sizeof(inputs.param.algTag) - 1U);
    std::strncpy(inputs.param.commName, "comm-ut", sizeof(inputs.param.commName) - 1U);

    inputs.resCtx.topoInfo.userRank = 0U;
    inputs.resCtx.topoInfo.userRankSize = 8U;
    inputs.resCtx.threads.push_back(static_cast<ThreadHandle>(1U));
    inputs.resCtx.cclMem.addr = reinterpret_cast<void*>(0x200000000ULL);
    inputs.resCtx.cclMem.size = 1024U * 1024U;
    ChannelInfo channel;
    channel.isValid = true;
    channel.remoteRank = 1U;
    channel.protocol = CommProtocol::COMM_PROTOCOL_UBC_CTP;
    channel.handle = static_cast<ChannelHandle>(2U);
    channel.remoteCclMem.addr = reinterpret_cast<void*>(0x300000000ULL);
    channel.remoteCclMem.size = 1024U * 1024U;
    inputs.resCtx.channels = {{channel}};

    if (opType == HCCL_CMD_ALLTOALL || opType == HCCL_CMD_ALLTOALLV) {
        for (uint32_t rank = 0U; rank < inputs.resCtx.topoInfo.userRankSize; ++rank) {
            inputs.sendCounts[rank] = rank + 1U;
            inputs.recvCounts[rank] = rank + 2U;
            inputs.sendDispls[rank] = rank * 16U;
            inputs.recvDispls[rank] = rank * 32U;
        }
        inputs.param.all2AllVDataDes.sendType = HCCL_DATA_TYPE_FP32;
        inputs.param.all2AllVDataDes.recvType = HCCL_DATA_TYPE_FP32;
        inputs.RefreshAllToAllPointers();
    }
    return inputs;
}

class Mc2AicpuTaskCacheTest : public testing::Test {
protected:
    void SetUp() override
    {
        ResetAlgEnvConfigInitState();
        ResetMc2TaskCacheTestStubState();
        Mc2HcommAicpuTsTaskCacheSetTestFunctions(
            TaskCacheLookupStub, TaskCacheStartStub, TaskCacheEndStub, TaskCacheExecuteStub, TaskCacheClearStub);
    }

    void TearDown() override { Mc2HcommAicpuTsTaskCacheResetTestFunctions(); }
};

TEST_F(Mc2AicpuTaskCacheTest, MissingDynamicSymbolsReturnNotSupport)
{
    Mc2HcommAicpuTsTaskCacheResetTestFunctions();
    bool isHit = false;
    void* addrs[2] = {nullptr, nullptr};
    uint64_t sizes[2] = {0U, 0U};
    EXPECT_FALSE(Mc2HcommIsSupportAicpuTsTaskCache());
    EXPECT_EQ(Mc2HcommAicpuTsTaskCacheLookup("tag", &isHit), HCCL_E_NOT_SUPPORT);
    EXPECT_EQ(Mc2HcommAicpuTsTaskCacheStart("tag", addrs, sizes, 2U), HCCL_E_NOT_SUPPORT);
    EXPECT_EQ(Mc2HcommAicpuTsTaskCacheEnd("tag"), HCCL_E_NOT_SUPPORT);
    EXPECT_EQ(Mc2HcommAicpuTsTaskCacheExecute("tag", addrs, sizes, 2U), HCCL_E_NOT_SUPPORT);
    EXPECT_EQ(Mc2HcommAicpuTsTaskCacheClear("tag"), HCCL_E_NOT_SUPPORT);
}

TEST_F(Mc2AicpuTaskCacheTest, CapabilityAndWrappersUseTheSamePrivateFunctionTable)
{
    Mc2HcommAicpuTsTaskCacheSetTestFunctions(TaskCacheLookupStub, nullptr, nullptr, nullptr, nullptr);
    EXPECT_FALSE(Mc2HcommIsSupportAicpuTsTaskCache());

    Mc2HcommAicpuTsTaskCacheSetTestFunctions(
        TaskCacheLookupStub, TaskCacheStartStub, TaskCacheEndStub, TaskCacheExecuteStub, TaskCacheClearStub);
    EXPECT_TRUE(Mc2HcommIsSupportAicpuTsTaskCache());
    bool isHit = false;
    EXPECT_EQ(Mc2HcommAicpuTsTaskCacheLookup("tag", &isHit), HCCL_SUCCESS);
    EXPECT_EQ(GetMc2TaskCacheTestStubState().lookupCalls, 1U);
}

TEST_F(Mc2AicpuTaskCacheTest, BuildsPlansForAllSupportedOperations)
{
    const HcclCMDType opTypes[] = {
        HCCL_CMD_ALLGATHER, HCCL_CMD_REDUCE_SCATTER, HCCL_CMD_ALLREDUCE, HCCL_CMD_ALLTOALL, HCCL_CMD_ALLTOALLV};
    for (HcclCMDType opType : opTypes) {
        auto inputs = MakeTaskCacheInputs(opType);
        if (opType == HCCL_CMD_ALLTOALL || opType == HCCL_CMD_ALLTOALLV) {
            inputs.RefreshAllToAllPointers();
        }
        Mc2AicpuTaskCachePlan plan;
        ASSERT_EQ(BuildMc2AicpuTaskCachePlan(inputs.param, inputs.resCtx, plan), HCCL_SUCCESS);
        EXPECT_TRUE(plan.enabled);
        EXPECT_EQ(plan.tag.find("mc2tc:v1:"), 0U);
    }
}

TEST_F(Mc2AicpuTaskCacheTest, ShapeAndAllToAllVArraysChangeTheTag)
{
    auto regular = MakeTaskCacheInputs(HCCL_CMD_ALLGATHER);
    Mc2AicpuTaskCachePlan first;
    Mc2AicpuTaskCachePlan second;
    ASSERT_EQ(BuildMc2AicpuTaskCachePlan(regular.param, regular.resCtx, first), HCCL_SUCCESS);
    regular.param.DataDes.strideCount += 1U;
    ASSERT_EQ(BuildMc2AicpuTaskCachePlan(regular.param, regular.resCtx, second), HCCL_SUCCESS);
    EXPECT_NE(first.tag, second.tag);

    auto allToAllV = MakeTaskCacheInputs(HCCL_CMD_ALLTOALLV);
    allToAllV.RefreshAllToAllPointers();
    ASSERT_EQ(BuildMc2AicpuTaskCachePlan(allToAllV.param, allToAllV.resCtx, first), HCCL_SUCCESS);
    allToAllV.recvCounts[3] += 1U;
    ASSERT_EQ(BuildMc2AicpuTaskCachePlan(allToAllV.param, allToAllV.resCtx, second), HCCL_SUCCESS);
    EXPECT_NE(first.tag, second.tag);
}

TEST_F(Mc2AicpuTaskCacheTest, MissingDynamicSymbolsBypassPlan)
{
    Mc2HcommAicpuTsTaskCacheResetTestFunctions();
    auto inputs = MakeTaskCacheInputs(HCCL_CMD_ALLGATHER);
    Mc2AicpuTaskCachePlan plan;
    ASSERT_EQ(BuildMc2AicpuTaskCachePlan(inputs.param, inputs.resCtx, plan), HCCL_SUCCESS);
    EXPECT_FALSE(plan.enabled);
}

TEST_F(Mc2AicpuTaskCacheTest, ComputesAllGatherAndReduceScatterStridedSpans)
{
    auto allGather = MakeTaskCacheInputs(HCCL_CMD_ALLGATHER);
    Mc2AicpuTaskCachePlan allGatherPlan;
    ASSERT_EQ(BuildMc2AicpuTaskCachePlan(allGather.param, allGather.resCtx, allGatherPlan), HCCL_SUCCESS);
    EXPECT_EQ(allGatherPlan.sizes[0], 871U * sizeof(float));
    EXPECT_EQ(allGatherPlan.sizes[1], (7U * 6269244U + 871U) * sizeof(float));

    auto reduceScatter = MakeTaskCacheInputs(HCCL_CMD_REDUCE_SCATTER);
    Mc2AicpuTaskCachePlan reduceScatterPlan;
    ASSERT_EQ(BuildMc2AicpuTaskCachePlan(reduceScatter.param, reduceScatter.resCtx, reduceScatterPlan), HCCL_SUCCESS);
    EXPECT_EQ(reduceScatterPlan.sizes[0], (7U * 6269244U + 871U) * sizeof(float));
    EXPECT_EQ(reduceScatterPlan.sizes[1], 871U * sizeof(float));
}

TEST_F(Mc2AicpuTaskCacheTest, InvalidAndOverlappingRangesBypassCache)
{
    auto inputs = MakeTaskCacheInputs(HCCL_CMD_ALLGATHER);
    inputs.param.outputPtr = inputs.param.inputPtr;
    Mc2AicpuTaskCachePlan plan;
    ASSERT_EQ(BuildMc2AicpuTaskCachePlan(inputs.param, inputs.resCtx, plan), HCCL_SUCCESS);
    EXPECT_FALSE(plan.enabled);

    inputs = MakeTaskCacheInputs(HCCL_CMD_ALLGATHER);
    inputs.param.DataDes.count = std::numeric_limits<uint64_t>::max();
    ASSERT_EQ(BuildMc2AicpuTaskCachePlan(inputs.param, inputs.resCtx, plan), HCCL_SUCCESS);
    EXPECT_FALSE(plan.enabled);
}

TEST_F(Mc2AicpuTaskCacheTest, ManagerEnforcesCountAndByteLimits)
{
    void* comm = reinterpret_cast<void*>(0x1000U);
    Mc2AicpuTaskCacheManager countManager(1U, 1024U);
    EXPECT_EQ(countManager.TryReserve(comm, "tag0"), Mc2TaskCacheReserveResult::RESERVED);
    EXPECT_EQ(countManager.TryReserve(comm, "tag0"), Mc2TaskCacheReserveResult::EXISTING);
    EXPECT_EQ(countManager.TryReserve(comm, "tag1"), Mc2TaskCacheReserveResult::FULL);
    countManager.Unregister(comm, "tag0");
    EXPECT_EQ(countManager.TryReserve(comm, "tag1"), Mc2TaskCacheReserveResult::RESERVED);

    Mc2AicpuTaskCacheManager byteManager(8U, 5U);
    EXPECT_EQ(byteManager.TryReserve(comm, "1234"), Mc2TaskCacheReserveResult::RESERVED);
    EXPECT_EQ(byteManager.TryReserve(comm, "56"), Mc2TaskCacheReserveResult::FULL);
}

TEST_F(Mc2AicpuTaskCacheTest, EvictRetainsTagsUntilClearSucceeds)
{
    void* comm = reinterpret_cast<void*>(0x1000U);
    Mc2AicpuTaskCacheManager manager(2U, 1024U);
    ASSERT_EQ(manager.TryReserve(comm, "tag0"), Mc2TaskCacheReserveResult::RESERVED);
    GetMc2TaskCacheTestStubState().clearRet = HCCL_E_PARA;
    EXPECT_EQ(manager.Evict(comm), HCCL_E_PARA);
    EXPECT_EQ(manager.TryReserve(comm, "tag0"), Mc2TaskCacheReserveResult::EXISTING);

    GetMc2TaskCacheTestStubState().clearRet = HCCL_SUCCESS;
    EXPECT_EQ(manager.Evict(comm), HCCL_SUCCESS);
    EXPECT_EQ(manager.TryReserve(comm, "tag0"), Mc2TaskCacheReserveResult::RESERVED);
}

TEST_F(Mc2AicpuTaskCacheTest, CacheMissCapturesAndCacheHitSkipsExecutor)
{
    auto inputs = MakeTaskCacheInputs(HCCL_CMD_ALLGATHER);
    Mc2AicpuTaskCacheManager manager(4U, 1024U);
    uint32_t executorCalls = 0U;
    EXPECT_EQ(
        RunWithMc2TaskCache(
            inputs.param, inputs.resCtx,
            [&executorCalls]() {
                ++executorCalls;
                return HCCL_SUCCESS;
            },
            manager),
        HCCL_SUCCESS);
    auto& state = GetMc2TaskCacheTestStubState();
    EXPECT_EQ(executorCalls, 1U);
    EXPECT_EQ(state.lookupCalls, 1U);
    EXPECT_EQ(state.cacheStartCalls, 1U);
    EXPECT_EQ(state.cacheEndCalls, 1U);

    ResetMc2TaskCacheTestStubState();
    state.lookupHit = true;
    EXPECT_EQ(
        RunWithMc2TaskCache(
            inputs.param, inputs.resCtx,
            [&executorCalls]() {
                ++executorCalls;
                return HCCL_SUCCESS;
            },
            manager),
        HCCL_SUCCESS);
    EXPECT_EQ(executorCalls, 1U);
    EXPECT_EQ(state.executeCalls, 1U);
    EXPECT_EQ(state.cacheStartCalls, 0U);
}

TEST_F(Mc2AicpuTaskCacheTest, CapacityFullBypassesAllHcommCacheCalls)
{
    auto inputs = MakeTaskCacheInputs(HCCL_CMD_ALLGATHER);
    Mc2AicpuTaskCacheManager manager(1U, 1024U);
    ASSERT_EQ(manager.TryReserve(reinterpret_cast<void*>(0x2000U), "existing"), Mc2TaskCacheReserveResult::RESERVED);
    uint32_t executorCalls = 0U;
    EXPECT_EQ(
        RunWithMc2TaskCache(
            inputs.param, inputs.resCtx,
            [&executorCalls]() {
                ++executorCalls;
                return HCCL_SUCCESS;
            },
            manager),
        HCCL_SUCCESS);
    const auto& state = GetMc2TaskCacheTestStubState();
    EXPECT_EQ(executorCalls, 1U);
    EXPECT_EQ(state.lookupCalls, 0U);
    EXPECT_EQ(state.cacheStartCalls, 0U);
    EXPECT_EQ(state.cacheEndCalls, 0U);
    EXPECT_EQ(state.executeCalls, 0U);
    EXPECT_EQ(state.clearCalls, 0U);
}

TEST_F(Mc2AicpuTaskCacheTest, ExecuteFailureClearsWithoutFallingBack)
{
    auto inputs = MakeTaskCacheInputs(HCCL_CMD_ALLGATHER);
    Mc2AicpuTaskCacheManager manager(2U, 1024U);
    auto& state = GetMc2TaskCacheTestStubState();
    state.lookupHit = true;
    state.executeRet = HCCL_E_PARA;
    uint32_t executorCalls = 0U;
    EXPECT_EQ(
        RunWithMc2TaskCache(
            inputs.param, inputs.resCtx,
            [&executorCalls]() {
                ++executorCalls;
                return HCCL_SUCCESS;
            },
            manager),
        HCCL_E_PARA);
    EXPECT_EQ(executorCalls, 0U);
    EXPECT_EQ(state.executeCalls, 1U);
    EXPECT_EQ(state.clearCalls, 1U);
}

TEST_F(Mc2AicpuTaskCacheTest, LookupFailureClearsThenFallsBack)
{
    auto inputs = MakeTaskCacheInputs(HCCL_CMD_ALLGATHER);
    Mc2AicpuTaskCacheManager manager(2U, 1024U);
    GetMc2TaskCacheTestStubState().lookupRet = HCCL_E_PARA;
    uint32_t executorCalls = 0U;
    EXPECT_EQ(
        RunWithMc2TaskCache(
            inputs.param, inputs.resCtx,
            [&executorCalls]() {
                ++executorCalls;
                return HCCL_SUCCESS;
            },
            manager),
        HCCL_SUCCESS);
    const auto& state = GetMc2TaskCacheTestStubState();
    EXPECT_EQ(executorCalls, 1U);
    EXPECT_EQ(state.clearCalls, 1U);
    EXPECT_EQ(manager.TryReserve(inputs.param.hcclComm, "replacement"), Mc2TaskCacheReserveResult::RESERVED);
}

TEST_F(Mc2AicpuTaskCacheTest, StartFailureClearsThenFallsBack)
{
    auto inputs = MakeTaskCacheInputs(HCCL_CMD_ALLGATHER);
    Mc2AicpuTaskCacheManager manager(2U, 1024U);
    GetMc2TaskCacheTestStubState().cacheStartRet = HCCL_E_PARA;
    uint32_t executorCalls = 0U;
    EXPECT_EQ(
        RunWithMc2TaskCache(
            inputs.param, inputs.resCtx,
            [&executorCalls]() {
                ++executorCalls;
                return HCCL_SUCCESS;
            },
            manager),
        HCCL_SUCCESS);
    const auto& state = GetMc2TaskCacheTestStubState();
    EXPECT_EQ(executorCalls, 1U);
    EXPECT_EQ(state.cacheStartCalls, 1U);
    EXPECT_EQ(state.clearCalls, 1U);
}

TEST_F(Mc2AicpuTaskCacheTest, ExecutorFailureClearsOnceAndReturnsError)
{
    auto inputs = MakeTaskCacheInputs(HCCL_CMD_ALLGATHER);
    Mc2AicpuTaskCacheManager manager(2U, 1024U);
    uint32_t executorCalls = 0U;
    EXPECT_EQ(
        RunWithMc2TaskCache(
            inputs.param, inputs.resCtx,
            [&executorCalls]() {
                ++executorCalls;
                return HCCL_E_PARA;
            },
            manager),
        HCCL_E_PARA);
    const auto& state = GetMc2TaskCacheTestStubState();
    EXPECT_EQ(executorCalls, 1U);
    EXPECT_EQ(state.cacheStartCalls, 1U);
    EXPECT_EQ(state.cacheEndCalls, 0U);
    EXPECT_EQ(state.clearCalls, 1U);
}

TEST_F(Mc2AicpuTaskCacheTest, CacheEndFailureClearsAndReturnsError)
{
    auto inputs = MakeTaskCacheInputs(HCCL_CMD_ALLGATHER);
    Mc2AicpuTaskCacheManager manager(2U, 1024U);
    GetMc2TaskCacheTestStubState().cacheEndRet = HCCL_E_PARA;
    uint32_t executorCalls = 0U;
    EXPECT_EQ(
        RunWithMc2TaskCache(
            inputs.param, inputs.resCtx,
            [&executorCalls]() {
                ++executorCalls;
                return HCCL_SUCCESS;
            },
            manager),
        HCCL_E_PARA);
    const auto& state = GetMc2TaskCacheTestStubState();
    EXPECT_EQ(executorCalls, 1U);
    EXPECT_EQ(state.cacheEndCalls, 1U);
    EXPECT_EQ(state.clearCalls, 1U);
}

} // namespace
