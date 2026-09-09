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
#include <vector>
#define private public
#include "kernel_operator.h"

using namespace std;
using namespace AscendC;
using namespace HcclApi;

namespace {
class HcclSuiteAIC : public testing::Test {
protected:
    virtual void SetUp() { AscendC::SetGCoreType(1); }
    virtual void TearDown() { AscendC::SetGCoreType(0); }
};

constexpr uint32_t kRankNum = 8U;
constexpr size_t workSpaceSize = sizeof(HcclMsgArea);

HcclCombineOpParam GetHcclCombineOpParam(const vector<uint8_t>& workSpace)
{
    uint64_t bufferCke[128];
    GM_ADDR ckeOffset = reinterpret_cast<GM_ADDR>(bufferCke);

    uint64_t bufferXn[64 * 64];
    GM_ADDR xnOffset = reinterpret_cast<GM_ADDR>(bufferXn);

    HcclCombineOpParam hcclCombineOpParam{
        reinterpret_cast<uintptr_t>(workSpace.data()), workSpaceSize, 0, kRankNum, 0, {0}, {0}, xnOffset, ckeOffset};
    return hcclCombineOpParam;
}

HcclMsgArea* GetHcclMsgArea(uint8_t* workspaceGM)
{
    uint64_t msgAddr = reinterpret_cast<uintptr_t>(workspaceGM);
    if (msgAddr & 0x1ff) {
        msgAddr = (msgAddr & (~((uint64_t)0x1ff))) + 0x200;
    }
    return reinterpret_cast<HcclMsgArea*>(msgAddr);
}

// repeat_prepare_commit Repeat = 1 Call the Prepare interface Expected handleId = 0
TEST_F(HcclSuiteAIC, AllGather_Repeat1)
{
    const HcclServerType serverType = HcclServerType::HCCL_SERVER_TYPE_AICPU;
    std::vector<uint8_t> workSpace(workSpaceSize);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl<HcclServerType::HCCL_SERVER_TYPE_CCU> hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));

    HcclHandle handleId = hccl.AllGather(
        reinterpret_cast<__gm__ uint8_t*>(0x1234), reinterpret_cast<__gm__ uint8_t*>(0x4321), 100,
        HcclDataType::HCCL_DATA_TYPE_INT8, 0, 1);
    hccl.Commit(handleId);
    EXPECT_EQ(handleId, 0);
    EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);
}

// repeat_prepare_commit_2_2 repeat = 1 calls the Prepare interface for loop call expected handleId = 0, 1
TEST_F(HcclSuiteAIC, AllGather_repeat_prepare_commit_2)
{
    const HcclServerType serverType = HcclServerType::HCCL_SERVER_TYPE_AICPU;
    std::vector<uint8_t> workSpace(workSpaceSize);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl<HcclServerType::HCCL_SERVER_TYPE_CCU> hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    for (int i = 0; i < 2; i++) {
        HcclHandle handleId = hccl.AllGather(
            reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
            HcclDataType::HCCL_DATA_TYPE_INT8, 0, 1);
        hccl.Commit(handleId);
        EXPECT_EQ(handleId, i);
        EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);
    }
}

// repeat_prepare_commit_2_1_1 calls the Prepare interface, repeat = 2, expected handleId = 0
TEST_F(HcclSuiteAIC, AllGather_repeat_prepare_commit_2_1_1)
{
    const HcclServerType serverType = HcclServerType::HCCL_SERVER_TYPE_AICPU;
    std::vector<uint8_t> workSpace(workSpaceSize);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl<HcclServerType::HCCL_SERVER_TYPE_CCU> hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    HcclHandle handleId = hccl.AllGather(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        HcclDataType::HCCL_DATA_TYPE_INT8, 0, 2);
    EXPECT_EQ(handleId, 0);
    for (uint8_t i = 0; i < 2; i++) {
        hccl.Commit(handleId);
        *(hcclCombineOpParam.ckeOffset + i * 8 + 8 * 64) = 0x1;
        EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);
    }
}

// prepare_commit_2_1_1 calls the Prepare interface, repeat = 2, expected handleId = 0
TEST_F(HcclSuiteAIC, AllGather_repeat_prepare_commit_16_1_1)
{
    const HcclServerType serverType = HcclServerType::HCCL_SERVER_TYPE_AICPU;
    std::vector<uint8_t> workSpace(workSpaceSize);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl<HcclServerType::HCCL_SERVER_TYPE_CCU> hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    for (int i = 0; i < 16; i++) {
        HcclHandle handleId = hccl.AllGather(
            reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
            HcclDataType::HCCL_DATA_TYPE_INT8, 0, 1);
        *(hcclCombineOpParam.ckeOffset + i * 8 + 64 * 8) = 0x1;
        hccl.Commit(handleId);
        EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);
    }
}

// Abnormal test: Commit occurs before Prepare, and the CommitTurnCnt value will not be written
TEST_F(HcclSuiteAIC, AllGather_CommitBeforePrepare)
{
    std::vector<uint8_t> workSpace(workSpaceSize);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl<HcclServerType::HCCL_SERVER_TYPE_CCU> hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    hccl.Commit(0);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, 0);
}

// Abnormal test: Wait occurs before Prepare, intercept exit
TEST_F(HcclSuiteAIC, AllGather_WaitBeforePrepare)
{
    std::vector<uint8_t> workSpace(workSpaceSize);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl<HcclServerType::HCCL_SERVER_TYPE_CCU> hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    EXPECT_EQ(hccl.Wait(0), HCCL_FAILED);
}

// Abnormal test: Wait occurs before Commit, intercepting exit
TEST_F(HcclSuiteAIC, AllGather_WaitBeforeCommit)
{
    std::vector<uint8_t> workSpace(workSpaceSize);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl<HcclServerType::HCCL_SERVER_TYPE_CCU> hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    HcclHandle handleId = hccl.AllReduce(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 3);
    ASSERT_EQ(handleId, 0);
    EXPECT_EQ(hccl.Wait(handleId), HCCL_FAILED);
}

// alltoallv repeat_prepare_commit Repeat = 1 Call the Prepare interface Expected handleId = 0
TEST_F(HcclSuiteAIC, AllToAllv_prepare)
{
    const HcclServerType serverType = HcclServerType::HCCL_SERVER_TYPE_AICPU;
    std::vector<uint8_t> workSpace(workSpaceSize);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl<HcclServerType::HCCL_SERVER_TYPE_CCU> hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));

    uint64_t sendCounts[4] = {1};
    uint64_t sdispls[4] = {1};
    uint64_t recvCounts[4] = {1};
    uint64_t rdispls[4] = {1};

    HcclHandle handleId = hccl.AlltoAllV<true>(
        reinterpret_cast<__gm__ uint8_t*>(0x1234), sendCounts, sdispls, HcclDataType::HCCL_DATA_TYPE_INT8,
        reinterpret_cast<__gm__ uint8_t*>(0x4321), recvCounts, rdispls, HcclDataType::HCCL_DATA_TYPE_INT8, 1);

    EXPECT_EQ(handleId, 0);
    EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);
}

// alltoall repeat_prepare_commit Repeat = 1 Call the Prepare interface Expected handleId = 0
TEST_F(HcclSuiteAIC, AllToAll_prepareWithCommitTrue)
{
    std::vector<uint8_t> workSpace(workSpaceSize);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl<HcclServerType::HCCL_SERVER_TYPE_CCU> hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));

    HcclHandle handleId = hccl.AlltoAll<true>(
        reinterpret_cast<__gm__ uint8_t*>(0x1234), reinterpret_cast<__gm__ uint8_t*>(0x4321), 10,
        HcclDataType::HCCL_DATA_TYPE_INT8);

    EXPECT_EQ(handleId, 0);
    EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);
}

// alltoallvWrite repeat_prepare_commit Repeat = 1 Call the Prepare interface Expected handleId = 0
TEST_F(HcclSuiteAIC, AllToAllVWrite_prepareWithCommitTrue)
{
    std::vector<uint8_t> workSpace(workSpaceSize);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl<HcclServerType::HCCL_SERVER_TYPE_CCU> hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));

    HcclHandle handleId = hccl.AlltoAllvWrite<true>(
        reinterpret_cast<__gm__ uint8_t*>(0x1234), reinterpret_cast<__gm__ uint8_t*>(0x4321),
        reinterpret_cast<__gm__ uint8_t*>(0x432a), 10, 10);

    EXPECT_EQ(handleId, 0);
    EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);
}

// allReduce repeat_prepare_commit Repeat = 128 Call the Prepare interface Expected handleId = 0
TEST_F(HcclSuiteAIC, AllReduce_prepareWithCommitTrue)
{
    std::vector<uint8_t> workSpace(workSpaceSize);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl<HcclServerType::HCCL_SERVER_TYPE_CCU> hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));

    HcclHandle handleId = hccl.AllReduce<true>(
        reinterpret_cast<__gm__ uint8_t*>(0x1234), reinterpret_cast<__gm__ uint8_t*>(0x4321), 10,
        HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 8);

    EXPECT_EQ(handleId, 0);
    for (uint32_t i = 0; i < 8; i++) {
        EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);
    }
}

// ReduceScatter repeat_prepare_commit Repeat = 80 Call the Prepare interface Expected handleId = 0
TEST_F(HcclSuiteAIC, AllReduce_prepareWithCommitFalse)
{
    std::vector<uint8_t> workSpace(workSpaceSize);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl<HcclServerType::HCCL_SERVER_TYPE_CCU> hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));

    HcclHandle handleId = hccl.ReduceScatter(
        reinterpret_cast<__gm__ uint8_t*>(0x1234), reinterpret_cast<__gm__ uint8_t*>(0x4321), 10,
        HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 0, 8);
    for (uint8_t i = 0; i < 8; i++) {
        hccl.Commit(handleId);
    }

    for (uint32_t i = 0; i < 8; i++) {
        EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);
    }
}

TEST_F(HcclSuiteAIC, AllGather_CcuAllGatherMeshMem2Mem1D)
{
    std::vector<uint8_t> workSpace(workSpaceSize);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);
    hcclCombineOpParam.opType[0] = static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER);
    hcclCombineOpParam.algorithmType[0] = static_cast<uint8_t>(AlgorithmType::CcuAllGatherMeshMem2Mem1D);

    Hccl<HcclServerType::HCCL_SERVER_TYPE_CCU> hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));

    HcclHandle handleId = hccl.AllGather(
        reinterpret_cast<__gm__ uint8_t*>(0x1234), reinterpret_cast<__gm__ uint8_t*>(0x4321), 100,
        HcclDataType::HCCL_DATA_TYPE_INT8, 0, 1);
    hccl.Commit(handleId);
    EXPECT_EQ(handleId, 0);
    EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);
}

TEST_F(HcclSuiteAIC, AllReduce_CcuAllReduceMeshMem2Mem1D)
{
    std::vector<uint8_t> workSpace(workSpaceSize);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);
    hcclCombineOpParam.opType[0] = static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLREDUCE);
    hcclCombineOpParam.algorithmType[0] = static_cast<uint8_t>(AlgorithmType::CcuAllReduceMeshMem2Mem1D);

    Hccl<HcclServerType::HCCL_SERVER_TYPE_CCU> hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    HcclHandle handleId = hccl.AllReduce(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 64,
        HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 1);
    hccl.Commit(handleId);
    EXPECT_EQ(handleId, 0);
    EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);
}

TEST_F(HcclSuiteAIC, AllReduce_CcuAllReduceMeshMem2Mem1D_Cout_not_dived)
{
    std::vector<uint8_t> workSpace(workSpaceSize);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);
    hcclCombineOpParam.opType[0] = static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLREDUCE);
    hcclCombineOpParam.algorithmType[0] = static_cast<uint8_t>(AlgorithmType::CcuAllReduceMeshMem2Mem1D);

    Hccl<HcclServerType::HCCL_SERVER_TYPE_CCU> hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    HcclHandle handleId = hccl.AllReduce(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 9,
        HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 1);
    hccl.Commit(handleId);
    EXPECT_EQ(handleId, 0);
    EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);
}

TEST_F(HcclSuiteAIC, AllReduce_CcuAllReduceMeshMem2Mem1D_Cout_zero)
{
    std::vector<uint8_t> workSpace(workSpaceSize);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);
    hcclCombineOpParam.opType[0] = static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLREDUCE);
    hcclCombineOpParam.algorithmType[0] = static_cast<uint8_t>(AlgorithmType::CcuAllReduceMeshMem2Mem1D);

    Hccl<HcclServerType::HCCL_SERVER_TYPE_CCU> hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    HcclHandle handleId = hccl.AllReduce(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 0,
        HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 0);

    EXPECT_EQ(handleId, -1);
    hccl.Commit(handleId);
    EXPECT_EQ(hccl.Wait(handleId), HCCL_FAILED);
}

TEST_F(HcclSuiteAIC, ReduceScatter_CcuReduceScatterMeshMem2Mem1D)
{
    std::vector<uint8_t> workSpace(workSpaceSize);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);
    hcclCombineOpParam.opType[0] = static_cast<uint32_t>(HcclCMDType::HCCL_CMD_REDUCE_SCATTER);
    hcclCombineOpParam.algorithmType[0] = static_cast<uint8_t>(AlgorithmType::CcuReduceScatterMeshMem2Mem1D);

    Hccl<HcclServerType::HCCL_SERVER_TYPE_CCU> hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));

    HcclHandle handleId = hccl.ReduceScatter(
        reinterpret_cast<__gm__ uint8_t*>(0x1234), reinterpret_cast<__gm__ uint8_t*>(0x4321), 10,
        HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 0, 1);

    hccl.Commit(handleId);
    EXPECT_EQ(handleId, 0);
    EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);
}

TEST_F(HcclSuiteAIC, ReduceScatter_CcuReduceScatterMeshMem2Mem1DPeerOnly)
{
    std::vector<uint8_t> workSpace(workSpaceSize);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);
    hcclCombineOpParam.opType[0] = static_cast<uint32_t>(HcclCMDType::HCCL_CMD_REDUCE_SCATTER);
    hcclCombineOpParam.algorithmType[0] = static_cast<uint8_t>(AlgorithmType::CcuReduceScatterMeshMem2Mem1DPeerOnly);

    Hccl<HcclServerType::HCCL_SERVER_TYPE_CCU> hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));

    HcclHandle handleId = hccl.ReduceScatter(
        reinterpret_cast<__gm__ uint8_t*>(0x1234), reinterpret_cast<__gm__ uint8_t*>(0x4321), 10,
        HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 0, 1);

    hccl.Commit(handleId);
    EXPECT_EQ(handleId, 0);
    EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);
}

TEST_F(HcclSuiteAIC, ReduceScatterPeerOnlyChunkParams)
{
    constexpr uint64_t chunkSize = 256ULL * 1024ULL * 1024ULL;
    constexpr uint64_t alignSize = 128ULL;
    struct TestCase {
        uint64_t sliceSize;
        uint64_t expectedTailSize;
        uint64_t expectedChunkCount;
    };
    const TestCase cases[] = {
        {0, 0, 0},
        {chunkSize, chunkSize, 1},
        {chunkSize + alignSize, alignSize, 2},
        {2 * chunkSize, chunkSize, 2},
    };

    for (const auto& testCase : cases) {
        uint64_t tailSize = 0;
        uint64_t chunkLoopNum = 0;
        CalcPeerOnlyChunkParams(testCase.sliceSize, &tailSize, &chunkLoopNum);
        EXPECT_EQ(tailSize, testCase.expectedTailSize);
        EXPECT_EQ(UINT64_MAX - chunkLoopNum, testCase.expectedChunkCount);
    }
}

} // namespace
