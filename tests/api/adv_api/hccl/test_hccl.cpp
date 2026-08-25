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
constexpr uint32_t kRankNum = 8U;
constexpr size_t workSpaceSize = sizeof(HcclMsgArea);
HcclCombineOpParam GetHcclCombineOpParam(const vector<uint8_t>& workSpace)
{
    HcclCombineOpParam hcclCombineOpParam{
        reinterpret_cast<uintptr_t>(workSpace.data()), workSpaceSize, 0, kRankNum, 0, {0}, {0}};
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

uint32_t GenXorForHcclMsg(void* msg)
{
    DataBlock* block = reinterpret_cast<DataBlock*>(msg);
    constexpr uint32_t kBlockCntForXor = 15U;
    uint32_t xorVal = 0U;
    for (uint32_t i = 0; i < kBlockCntForXor; ++i) {
        xorVal ^= block->data[i];
    }
    return xorVal;
}

uint64_t GenXorForHcclMsgExt(const HcclMsgExt* msgExt, const uint32_t rankNum)
{
    if (msgExt == nullptr) {
        return 0;
    }
    uint64_t xorVal = 0U;
    for (uint32_t i = 0U; i < rankNum; ++i) {
        xorVal ^= msgExt->sendCounts[i];
        xorVal ^= msgExt->sendOffset[i];
        xorVal ^= msgExt->recvCounts[i];
        xorVal ^= msgExt->recvOffset[i];
    }
    xorVal ^= HCCL_MSG_VALID_MASK;
    return xorVal;
}

void AlltoAllVThreadFunc(int blockIdx, HcclCombineOpParam&& hcclCombineOpParam, bool afterWorkBlockIdx = false)
{
    // blockIdx switch + backup
    auto block_idx_backup = block_idx;
    block_idx = blockIdx;
    KERNEL_LOG(
        KERNEL_INFO, "aicore blockIdx=%ld(recoverBlockIdx=%ld) start working...", GetBlockIdx(), block_idx_backup);

    // The code to be executed by the thread
    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    std::vector<uint64_t> sendCounts(kRankNum, 10);
    std::vector<uint64_t> recvCounts(kRankNum, 11);
    std::vector<uint64_t> sendOffsets(kRankNum, 12);
    std::vector<uint64_t> recvOffsets(kRankNum, 13);
    HcclHandle handleId = hccl.AlltoAllV(
        reinterpret_cast<__gm__ uint8_t*>(0x11), sendCounts.data(), sendOffsets.data(),
        HcclDataType::HCCL_DATA_TYPE_INT8, reinterpret_cast<__gm__ uint8_t*>(0x11), recvCounts.data(),
        recvOffsets.data(), HcclDataType::HCCL_DATA_TYPE_INT8);
    EXPECT_EQ(handleId, 0);
    auto hcclMsgArea = GetHcclMsgArea(reinterpret_cast<uint8_t*>(hcclCombineOpParam.workSpace));
    ASSERT_NE(hcclMsgArea, nullptr);
    if (afterWorkBlockIdx) {
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.repeatCnt, 1);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].dataCnt, 0);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.hcclDataType, HcclReduceOp::HCCL_REDUCE_SUM);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].strideCount, 0);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].commType.prepareType, HcclCMDType::HCCL_CMD_ALLTOALLV);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].opType, HcclReduceOp::HCCL_REDUCE_RESERVED);
        ASSERT_EQ(hcclMsgArea->commMsg.singleMsg.paramExtMsgList[0].valid, HCCL_MSG_VALID_MASK);
        ASSERT_EQ(
            hcclMsgArea->commMsg.singleMsg.paramExtMsgList[0].xorCheck,
            GenXorForHcclMsgExt(&hcclMsgArea->commMsg.singleMsg.paramExtMsgList[0], kRankNum));
        for (int32_t i = 0; i < kRankNum; ++i) {
            EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.paramExtMsgList[0].sendCounts[i], 10);
            EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.paramExtMsgList[0].recvCounts[i], 11);
            EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.paramExtMsgList[0].sendOffset[i], 12);
            EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.paramExtMsgList[0].recvOffset[i], 13);
        }
    } else {
        EXPECT_NE(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);
    }

    hccl.Commit(handleId);
    if (afterWorkBlockIdx) {
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, 1);
    } else {
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, 0);
    }

    hcclMsgArea->commMsg.singleMsg.finishedTurnCnt[0].cnt = 1;
    ASSERT_EQ(hccl.Query(handleId), 1);
    EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);

    // blockIdx recovery
    block_idx = block_idx_backup;
    KERNEL_LOG(
        KERNEL_INFO, "aicore blockIdx=%ld finished working, and recover to blockIdx=%ld.", GetBlockIdx(),
        block_idx_backup);
}

void FinalizeThreadFunc(int blockIdx, Hccl<HcclServerType::HCCL_SERVER_TYPE_AICPU>& hccl)
{
    auto block_idx_backup = block_idx;
    block_idx = blockIdx;
    KERNEL_LOG(
        KERNEL_INFO, "aicore blockIdx=%ld(recoverBlockIdx=%ld) start Finalize...", GetBlockIdx(), block_idx_backup);

    hccl.Finalize();

    block_idx = block_idx_backup;
    KERNEL_LOG(
        KERNEL_INFO, "aicore blockIdx=%ld finished Finalize, and recover to blockIdx=%ld.", GetBlockIdx(),
        block_idx_backup);
}

void ReadFinalizeMsgThreadFunc(const uint8_t msgPos, HcclMsgArea* hcclMsgArea)
{
    while (hcclMsgArea->commMsg.singleMsg.sendMsgs[msgPos].addMsg.v0Msg.valid != HCCL_MSG_VALID_MASK) {
    }
    EXPECT_EQ(
        hcclMsgArea->commMsg.singleMsg.sendMsgs[msgPos].addMsg.v0Msg.xorCheck,
        GenXorForHcclMsg(&hcclMsgArea->commMsg.singleMsg.sendMsgs[msgPos]));
    hcclMsgArea->commMsg.singleMsg.sendMsgs[msgPos].addMsg.v0Msg.valid = ~HCCL_MSG_VALID_MASK;
    hcclMsgArea->commMsg.singleMsg.finishedTurnCnt[msgPos].cnt = FINALIZE_FINISH_CNT;
    KERNEL_LOG(KERNEL_INFO, "Aicpu has read Finalize msg[%u].", msgPos);
}
} // namespace
class HcclCommonTestSuite : public testing::Test {
protected:
    virtual void SetUp() { blockIdxBak_ = block_idx; }
    virtual void TearDown() { block_idx = blockIdxBak_; }

private:
    int64_t blockIdxBak_;
};

class HcclAbnormalTestSuite : public testing::Test {
protected:
    virtual void SetUp() { blockIdxBak_ = block_idx; }
    virtual void TearDown() { block_idx = blockIdxBak_; }

private:
    int64_t blockIdxBak_;
};

class HcclCombineTestSuite : public testing::Test {
protected:
    virtual void SetUp() { blockIdxBak_ = block_idx; }
    virtual void TearDown() { block_idx = blockIdxBak_; }

private:
    int64_t blockIdxBak_;
};

TEST_F(HcclCommonTestSuite, ResetPrimitiveIdStateInControlMsg)
{
    ControlHcclMsg controlMsg{};
    controlMsg.reserved[HCCL_CONTROL_RESERVED_PRIMITIVE_ID_IDX] = 1U;
    controlMsg.reserved[HCCL_CONTROL_RESERVED_PRIMITIVE_RESET_IDX] = 1U;

    ResetPrimitiveIdStateInControlMsg(&controlMsg);
    EXPECT_EQ(controlMsg.reserved[HCCL_CONTROL_RESERVED_PRIMITIVE_ID_IDX], 0U);
    EXPECT_EQ(controlMsg.reserved[HCCL_CONTROL_RESERVED_PRIMITIVE_RESET_IDX], 0U);

    controlMsg.reserved[HCCL_CONTROL_RESERVED_PRIMITIVE_ID_IDX] = 1U;
    ResetPrimitiveIdOnceInControlMsg(&controlMsg);
    EXPECT_EQ(controlMsg.reserved[HCCL_CONTROL_RESERVED_PRIMITIVE_ID_IDX], 0U);
    EXPECT_EQ(controlMsg.reserved[HCCL_CONTROL_RESERVED_PRIMITIVE_RESET_IDX], 1U);

    controlMsg.reserved[HCCL_CONTROL_RESERVED_PRIMITIVE_ID_IDX] = 1U;
    ResetPrimitiveIdOnceInControlMsg(&controlMsg);
    EXPECT_EQ(controlMsg.reserved[HCCL_CONTROL_RESERVED_PRIMITIVE_ID_IDX], 1U);
}

TEST_F(HcclAbnormalTestSuite, UpdateControlMsgCount_InvalidMsgType)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());

    UpdateControlMsgCount(hcclMsgArea, static_cast<ControlMsgType>(0U));
    UpdateControlMsgCount(hcclMsgArea, ControlMsgType::HCCL_CMD_MAX);
    for (uint32_t i = 0U; i < HCCL_MSG_TYPE_CNT; ++i) {
        EXPECT_EQ(hcclMsgArea->apiStats.msgStats[i].cnt, 0U);
    }
}

TEST_F(HcclAbnormalTestSuite, AlltoAllV_InvalidRankNum)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);
    hcclCombineOpParam.rankNum = HCCL_MAX_RANK_NUM_V2 + 1U;
    std::vector<uint64_t> counts(hcclCombineOpParam.rankNum, 0U);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    HcclHandle handleId = hccl.AlltoAllV(
        reinterpret_cast<__gm__ uint8_t*>(0x11), counts.data(), counts.data(), HcclDataType::HCCL_DATA_TYPE_INT8,
        reinterpret_cast<__gm__ uint8_t*>(0x11), counts.data(), counts.data(), HcclDataType::HCCL_DATA_TYPE_INT8);

    EXPECT_EQ(handleId, 0);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);
    EXPECT_NE(hcclMsgArea->commMsg.singleMsg.paramExtMsgList[0].valid, HCCL_MSG_VALID_MASK);
}

// Prepare 1 time (AllReduce interface, repeat=1) + Commit 1 time, verify the content of the message area
TEST_F(HcclCommonTestSuite, AllReduce_Repeat1)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    HcclHandle handleId = hccl.AllReduce(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM);
    EXPECT_EQ(handleId, 0);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.repeatCnt, 1);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].commType.prepareType, HcclCMDType::HCCL_CMD_ALLREDUCE);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].opType, HcclReduceOp::HCCL_REDUCE_SUM);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.selfHandleID, handleId);
    EXPECT_EQ(
        hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.xorCheck,
        GenXorForHcclMsg(&hcclMsgArea->commMsg.singleMsg.sendMsgs[0]));
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, 0);
    hccl.Commit(handleId);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].valid, COMMIT_VALID_MASK);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, 1);
    ASSERT_EQ(hccl.Query(handleId), 0);
    hcclMsgArea->commMsg.singleMsg.finishedTurnCnt[0].cnt++;
    ASSERT_EQ(hccl.Query(handleId), 1);
    EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);

    EXPECT_EQ(hccl.GetRankDim(), kRankNum);
    EXPECT_EQ(hccl.GetRankId(), 0);
    EXPECT_EQ(hccl.GetWindowsInAddr(0), nullptr);
    EXPECT_EQ(hccl.GetWindowsOutAddr(0), nullptr);
}

// Prepare 1 time (AllGather interface, repeat=1) + Commit 1 time, verify the content of the message area
TEST_F(HcclCommonTestSuite, AllGather_Repeat1)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    HcclHandle handleId = hccl.AllGather(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        HcclDataType::HCCL_DATA_TYPE_INT8, 0);
    EXPECT_EQ(handleId, 0);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.repeatCnt, 1);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].commType.prepareType, HcclCMDType::HCCL_CMD_ALLGATHER);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].opType, HcclReduceOp::HCCL_REDUCE_RESERVED);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, 0);
    hccl.Commit(handleId);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, 1);
    ASSERT_EQ(hccl.Query(handleId), 0);
    hcclMsgArea->commMsg.singleMsg.finishedTurnCnt[0].cnt++;
    ASSERT_EQ(hccl.Query(handleId), 1);
    EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);
}

// Prepare 1 time (ReduceScatter interface, repeat=1) + Commit 1 time, verify the content of the message area
TEST_F(HcclCommonTestSuite, ReduceScatter_Repeat1)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    HcclHandle handleId = hccl.ReduceScatter(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 100 * 8);
    EXPECT_EQ(handleId, 0);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.repeatCnt, 1);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].commType.prepareType, HcclCMDType::HCCL_CMD_REDUCE_SCATTER);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].opType, HcclReduceOp::HCCL_REDUCE_SUM);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, 0);
    hccl.Commit(handleId);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, 1);
    ASSERT_EQ(hccl.Query(handleId), 0);
    hcclMsgArea->commMsg.singleMsg.finishedTurnCnt[0].cnt++;
    ASSERT_EQ(hccl.Query(handleId), 1);
    EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);
}

// Prepare 1 time (AlltoAll interface, repeat=1) + Commit 1 time, verify the content of the message area
TEST_F(HcclCommonTestSuite, AlltoAll_Repeat1)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    HcclHandle handleId = hccl.AlltoAll(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        HcclDataType::HCCL_DATA_TYPE_INT8, 100 * 8);
    EXPECT_EQ(handleId, 0);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.repeatCnt, 1);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].dataCnt, 100);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.hcclDataType, HcclDataType::HCCL_DATA_TYPE_INT8);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].strideCount, 100 * 8);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].commType.prepareType, HcclCMDType::HCCL_CMD_ALLTOALL);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].opType, HcclReduceOp::HCCL_REDUCE_RESERVED);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, 0);
    hccl.Commit(handleId);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, 1);
    ASSERT_EQ(hccl.Query(handleId), 0);
    hcclMsgArea->commMsg.singleMsg.finishedTurnCnt[0].cnt++;
    ASSERT_EQ(hccl.Query(handleId), 1);
    EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);
    std::thread t1(FinalizeThreadFunc, 0, std::ref(hccl));
    std::thread t2(ReadFinalizeMsgThreadFunc, 1, hcclMsgArea);
    t1.join();
    t2.join();
    ASSERT_EQ(hccl.Query(handleId), 0);
}

// Prepare once (AlltoAll interface, repeat=2) to verify the content of the message area
TEST_F(HcclCommonTestSuite, AlltoAll_Repeat2_CommitWhenPrepare)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    HcclHandle handleId = hccl.AlltoAll<true>(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        HcclDataType::HCCL_DATA_TYPE_INT8, 100 * 8, 2);
    EXPECT_EQ(handleId, 0);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.repeatCnt, 2);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].dataCnt, 100);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.hcclDataType, HcclDataType::HCCL_DATA_TYPE_INT8);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].strideCount, 100 * 8);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].commType.prepareType, HcclCMDType::HCCL_CMD_ALLTOALL);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].opType, HcclReduceOp::HCCL_REDUCE_RESERVED);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, 2);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].valid, COMMIT_VALID_MASK);
    for (int i = 0; i < 2; ++i) {
        ASSERT_EQ(hccl.Query(handleId), i);
        hcclMsgArea->commMsg.singleMsg.finishedTurnCnt[0].cnt++;
        ASSERT_EQ(hccl.Query(handleId), i + 1);
        EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);
    }
    std::thread t1(FinalizeThreadFunc, 0, std::ref(hccl));
    std::thread t2(ReadFinalizeMsgThreadFunc, 1, hcclMsgArea);
    t1.join();
    t2.join();
    ASSERT_EQ(hccl.Query(handleId), 0);
}

// Prepare 10 times (AlltoAll interface, repeat=1) + Commit 10 times to verify the content of the message area
TEST_F(HcclCommonTestSuite, AlltoAll_Prepare10Repeat1)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    for (int i = 0; i < 10; ++i) {
        HcclHandle handleId = hccl.AlltoAll(
            reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
            HcclDataType::HCCL_DATA_TYPE_INT8, 100 * 8);
        EXPECT_EQ(handleId, i);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[i].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[i].addMsg.v0Msg.repeatCnt, 1);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[i].dataCnt, 100);
        EXPECT_EQ(
            hcclMsgArea->commMsg.singleMsg.sendMsgs[i].addMsg.v0Msg.hcclDataType, HcclDataType::HCCL_DATA_TYPE_INT8);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[i].strideCount, 100 * 8);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[i].commType.prepareType, HcclCMDType::HCCL_CMD_ALLTOALL);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[i].opType, HcclReduceOp::HCCL_REDUCE_RESERVED);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[i].cnt, 0);
        hccl.Commit(handleId);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[i].cnt, 1);
        ASSERT_EQ(hccl.Query(handleId), 0);
        hcclMsgArea->commMsg.singleMsg.finishedTurnCnt[i].cnt++;
        ASSERT_EQ(hccl.Query(handleId), 1);
        EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);
    }
    std::thread t1(FinalizeThreadFunc, 0, std::ref(hccl));
    std::thread t2(ReadFinalizeMsgThreadFunc, 10, hcclMsgArea);
    t1.join();
    t2.join();
}

// Prepare 1 time (AlltoAllV interface, repeat=1) + Commit 1 time, verify the content of the message area
TEST_F(HcclCommonTestSuite, AlltoAllV_Repeat1)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    int64_t blockIdx = 0;
    std::thread t1(AlltoAllVThreadFunc, blockIdx, hcclCombineOpParam, true);
    t1.join();
}

// Prepare once (AlltoAllV interface, repeat=2) to verify the content of the message area
TEST_F(HcclCommonTestSuite, AlltoAllV_Repeat2_CommitWhenPrepare)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    std::vector<uint64_t> sendCounts(kRankNum, 10);
    std::vector<uint64_t> recvCounts(kRankNum, 11);
    std::vector<uint64_t> sendOffsets(kRankNum, 12);
    std::vector<uint64_t> recvOffsets(kRankNum, 13);
    HcclHandle handleId = hccl.AlltoAllV<true>(
        reinterpret_cast<__gm__ uint8_t*>(0x11), sendCounts.data(), sendOffsets.data(),
        HcclDataType::HCCL_DATA_TYPE_INT8, reinterpret_cast<__gm__ uint8_t*>(0x11), recvCounts.data(),
        recvOffsets.data(), HcclDataType::HCCL_DATA_TYPE_INT8, 2);
    EXPECT_EQ(handleId, 0);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.repeatCnt, 2);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].dataCnt, 0);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.hcclDataType, HcclReduceOp::HCCL_REDUCE_SUM);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].strideCount, 0);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].commType.prepareType, HcclCMDType::HCCL_CMD_ALLTOALLV);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].opType, HcclReduceOp::HCCL_REDUCE_RESERVED);
    ASSERT_EQ(hcclMsgArea->commMsg.singleMsg.paramExtMsgList[0].valid, HCCL_MSG_VALID_MASK);
    ASSERT_EQ(
        hcclMsgArea->commMsg.singleMsg.paramExtMsgList[0].xorCheck,
        GenXorForHcclMsgExt(&hcclMsgArea->commMsg.singleMsg.paramExtMsgList[0], kRankNum));
    for (int32_t i = 0; i < kRankNum; ++i) {
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.paramExtMsgList[0].sendCounts[i], 10);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.paramExtMsgList[0].recvCounts[i], 11);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.paramExtMsgList[0].sendOffset[i], 12);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.paramExtMsgList[0].recvOffset[i], 13);
    }
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, 2);
    for (int i = 0; i < 2; ++i) {
        ASSERT_EQ(hccl.Query(handleId), i);
        hcclMsgArea->commMsg.singleMsg.finishedTurnCnt[0].cnt++;
        ASSERT_EQ(hccl.Query(handleId), i + 1);
        EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);
    }
    std::thread t1(FinalizeThreadFunc, 0, std::ref(hccl));
    std::thread t2(ReadFinalizeMsgThreadFunc, 1, hcclMsgArea);
    t1.join();
    t2.join();
    ASSERT_EQ(hccl.Query(handleId), 0);
}

// Prepare 10 times (AlltoAllV interface, repeat=1) + Commit 10 times to verify the content of the message area
TEST_F(HcclCommonTestSuite, AlltoAllV_Prepare10Repeat1)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    std::vector<uint64_t> sendCounts(kRankNum, 10);
    std::vector<uint64_t> recvCounts(kRankNum, 11);
    std::vector<uint64_t> sendOffsets(kRankNum, 12);
    std::vector<uint64_t> recvOffsets(kRankNum, 13);
    for (int i = 0; i < 10; ++i) {
        HcclHandle handleId = hccl.AlltoAllV(
            reinterpret_cast<__gm__ uint8_t*>(0x11), sendCounts.data(), sendOffsets.data(),
            HcclDataType::HCCL_DATA_TYPE_INT8, reinterpret_cast<__gm__ uint8_t*>(0x11), recvCounts.data(),
            recvOffsets.data(), HcclDataType::HCCL_DATA_TYPE_INT8);
        EXPECT_EQ(handleId, i);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[i].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[i].addMsg.v0Msg.repeatCnt, 1);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[i].dataCnt, 0);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[i].commType.prepareType, HcclCMDType::HCCL_CMD_ALLTOALLV);
        ASSERT_EQ(hcclMsgArea->commMsg.singleMsg.paramExtMsgList[0].valid, HCCL_MSG_VALID_MASK);
        ASSERT_EQ(
            hcclMsgArea->commMsg.singleMsg.paramExtMsgList[0].xorCheck,
            GenXorForHcclMsgExt(&hcclMsgArea->commMsg.singleMsg.paramExtMsgList[0], kRankNum));
        for (int32_t j = 0; j < kRankNum; ++j) {
            EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.paramExtMsgList[i].sendCounts[j], 10);
            EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.paramExtMsgList[i].recvCounts[j], 11);
            EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.paramExtMsgList[i].sendOffset[j], 12);
            EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.paramExtMsgList[i].recvOffset[j], 13);
        }

        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[i].cnt, 0);
        hccl.Commit(handleId);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[i].cnt, 1);
        ASSERT_EQ(hccl.Query(handleId), 0);
        hcclMsgArea->commMsg.singleMsg.finishedTurnCnt[i].cnt++;
        ASSERT_EQ(hccl.Query(handleId), 1);
        EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);
    }
    std::thread t1(FinalizeThreadFunc, 0, std::ref(hccl));
    std::thread t2(ReadFinalizeMsgThreadFunc, 10, hcclMsgArea);
    t1.join();
    t2.join();
}

// When testing a core with blockIdx≠0, the AlltoAllV interface will not actually send a message.
TEST_F(HcclCommonTestSuite, AlltoAllV_BlockIdxNot0_MsgIsNotWritten)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    int64_t blockIdx = 2;
    std::thread t1(AlltoAllVThreadFunc, blockIdx, hcclCombineOpParam, false);
    t1.join();
}

TEST_F(HcclCommonTestSuite, InterHcclGroupSync)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    hccl.InterHcclGroupSync(0, 1);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.commDepGroupID, 0);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.commDepHandleID, 1);
    EXPECT_EQ(
        hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.xorCheck,
        GenXorForHcclMsg(&hcclMsgArea->commMsg.singleMsg.sendMsgs[0]));
    std::thread t1(FinalizeThreadFunc, 0, std::ref(hccl));
    std::thread t2(ReadFinalizeMsgThreadFunc, 1, hcclMsgArea);
    t1.join();
    t2.join();
}

// Prepare 2 times (AllReduce interface, repeat=1) + Commit 2 times to verify the content of the message area
TEST_F(HcclCommonTestSuite, AllReduce_Repeat1Prepare2Commit2)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    for (size_t i = 0U; i < 2; ++i) {
        HcclHandle handleId = hccl.AllReduce(
            reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
            HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM);
        EXPECT_EQ(handleId, i);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[i].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[i].addMsg.v0Msg.repeatCnt, 1);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[i].commType.prepareType, HcclCMDType::HCCL_CMD_ALLREDUCE);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[i].opType, HcclReduceOp::HCCL_REDUCE_SUM);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[i].cnt, 0);
        hccl.Commit(handleId);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[i].cnt, 1);
        ASSERT_EQ(hccl.Query(handleId), 0);
        hcclMsgArea->commMsg.singleMsg.finishedTurnCnt[i].cnt++;
        ASSERT_EQ(hccl.Query(handleId), 1);
        EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);
    }
}

// Prepare once (AllReduce interface, repeat=2) + Commit twice to verify the content of the message area
TEST_F(HcclCommonTestSuite, AllReduce_Repeat2Prepare1Commit2)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    HcclHandle handleId = hccl.AllReduce(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 2);
    EXPECT_EQ(handleId, 0);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.repeatCnt, 2);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].commType.prepareType, HcclCMDType::HCCL_CMD_ALLREDUCE);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].opType, HcclReduceOp::HCCL_REDUCE_SUM);
    for (size_t i = 0U; i < 2; ++i) {
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, i);
        hccl.Commit(handleId);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, i + 1);
        ASSERT_EQ(hccl.Query(handleId), i);
        hcclMsgArea->commMsg.singleMsg.finishedTurnCnt[0].cnt++;
        ASSERT_EQ(hccl.Query(handleId), i + 1);
        EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);
    }
}

// In the scenario of computing first and communicating later,
// users can skip calling the Wait interface and only call the Finalize interface before exiting.
TEST_F(HcclCommonTestSuite, AllReduceCallFinalize_ResetFinishedCount)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    HcclHandle handleId = hccl.AllReduce(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 2);
    EXPECT_EQ(handleId, 0);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.repeatCnt, 2);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].commType.prepareType, HcclCMDType::HCCL_CMD_ALLREDUCE);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].opType, HcclReduceOp::HCCL_REDUCE_SUM);
    for (size_t i = 0U; i < 2; ++i) {
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, i);
        hccl.Commit(handleId);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, i + 1);
        ASSERT_EQ(hccl.Query(handleId), i);
        hcclMsgArea->commMsg.singleMsg.finishedTurnCnt[0].cnt++;
        ASSERT_EQ(hccl.Query(handleId), i + 1);
    }
    std::thread t1(FinalizeThreadFunc, 0, std::ref(hccl));
    std::thread t2(ReadFinalizeMsgThreadFunc, 1, hcclMsgArea);
    t1.join();
    t2.join();
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.finishedTurnCnt[0].cnt, 0);
}

// BatchPrepare once (AllReduce interface, repeat=2), verify the content of the message area
TEST_F(HcclCommonTestSuite, AllReduce_Repeat2Prepare1WithCommit)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    HcclHandle handleId = hccl.AllReduce<true>(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 2);
    EXPECT_EQ(handleId, 0);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.repeatCnt, 2);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].commType.prepareType, HcclCMDType::HCCL_CMD_ALLREDUCE);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].opType, HcclReduceOp::HCCL_REDUCE_SUM);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, 2);
    ASSERT_EQ(hccl.Query(handleId), 0);
    hcclMsgArea->commMsg.singleMsg.finishedTurnCnt[0].cnt += 2;
    ASSERT_EQ(hccl.Query(handleId), 2);
    for (size_t i = 0U; i < 2; ++i) {
        EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);
    }
}

// Mixing BatchPrepare and Prepare interfaces + Finalize, checking the content in the message area
TEST_F(HcclCombineTestSuite, 3Prepare_3TasksForeach)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));

    HcclHandle handleId = hccl.AllReduce(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 3);
    EXPECT_EQ(handleId, 0);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.repeatCnt, 3);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].commType.prepareType, HcclCMDType::HCCL_CMD_ALLREDUCE);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].opType, HcclReduceOp::HCCL_REDUCE_SUM);

    for (size_t i = 0U; i < 3; ++i) {
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, i);
        hccl.Commit(handleId);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, i + 1);
        ASSERT_EQ(hccl.Query(handleId), i);
        hcclMsgArea->commMsg.singleMsg.finishedTurnCnt[0].cnt++;
        ASSERT_EQ(hccl.Query(handleId), i + 1);
        EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);
    }

    handleId = hccl.AllGather<true>(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        HcclDataType::HCCL_DATA_TYPE_INT8, 0, 3);
    EXPECT_EQ(handleId, 1);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[1].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[1].addMsg.v0Msg.repeatCnt, 3);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[1].commType.prepareType, HcclCMDType::HCCL_CMD_ALLGATHER);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[1].opType, HcclReduceOp::HCCL_REDUCE_RESERVED);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[1].cnt, 3);
    for (size_t i = 0U; i < 3; ++i) {
        ASSERT_EQ(hccl.Query(handleId), i);
        hcclMsgArea->commMsg.singleMsg.finishedTurnCnt[1].cnt++;
        ASSERT_EQ(hccl.Query(handleId), i + 1);
        EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);
    }

    for (int i = 0; i < 3; ++i) {
        handleId = hccl.ReduceScatter(
            reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
            HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 100 * 8);
        EXPECT_EQ(handleId, 2 + i);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[2 + i].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[2 + i].addMsg.v0Msg.repeatCnt, 1);
        EXPECT_EQ(
            hcclMsgArea->commMsg.singleMsg.sendMsgs[2 + i].commType.prepareType, HcclCMDType::HCCL_CMD_REDUCE_SCATTER);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[2 + i].opType, HcclReduceOp::HCCL_REDUCE_SUM);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[2 + i].cnt, 0);
        hccl.Commit(handleId);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[2 + i].cnt, 1);
        ASSERT_EQ(hccl.Query(handleId), 0);
        hcclMsgArea->commMsg.singleMsg.finishedTurnCnt[2 + i].cnt++;
        ASSERT_EQ(hccl.Query(handleId), 1);
        EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);
    }

    std::thread t1(FinalizeThreadFunc, 0, std::ref(hccl));
    std::thread t2(ReadFinalizeMsgThreadFunc, 5, hcclMsgArea);
    t1.join();
    t2.join();
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[5].addMsg.v0Msg.valid, ~HCCL_MSG_VALID_MASK);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[5].commType.msgType, ControlMsgType::HCCL_CMD_FINALIZE);
}

// The order of Commit and Prepare is inconsistent, but the order of Wait and Prepare is consistent,
// and the use case works normally.
// step1 Prepare(AllReduce)
// step2 Prepare(AlltoAll)同步Commit
// step3 Prepare(AllGather)同步Commit
// step4 AllReduce Commit+Wait
// step5 AlltoAll Wait
// step6 AllGather Wait
// step7 Finalize
TEST_F(HcclCombineTestSuite, MultiPrepareInvoked_CommitNotAccordingToPrepareOrder)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));

    HcclHandle handleId0 = hccl.AllReduce(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 3);
    EXPECT_EQ(handleId0, 0);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.repeatCnt, 3);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].commType.prepareType, HcclCMDType::HCCL_CMD_ALLREDUCE);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].opType, HcclReduceOp::HCCL_REDUCE_SUM);

    HcclHandle handleId1 = hccl.AlltoAll<true>(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        HcclDataType::HCCL_DATA_TYPE_INT8, 100 * 8, 2);
    EXPECT_EQ(handleId1, 1);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[1].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[1].addMsg.v0Msg.repeatCnt, 2);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[1].commType.prepareType, HcclCMDType::HCCL_CMD_ALLTOALL);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[1].opType, HcclReduceOp::HCCL_REDUCE_RESERVED);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[1].cnt, 2);

    HcclHandle handleId2 = hccl.AllGather<true>(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        HcclDataType::HCCL_DATA_TYPE_INT8, 0, 3);
    EXPECT_EQ(handleId2, 2);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[2].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[2].addMsg.v0Msg.repeatCnt, 3);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[2].commType.prepareType, HcclCMDType::HCCL_CMD_ALLGATHER);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[2].opType, HcclReduceOp::HCCL_REDUCE_RESERVED);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[2].cnt, 3);

    for (size_t i = 0U; i < 3; ++i) {
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, i);
        hccl.Commit(handleId0);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, i + 1);
        ASSERT_EQ(hccl.Query(handleId0), i);
        hcclMsgArea->commMsg.singleMsg.finishedTurnCnt[0].cnt++;
        ASSERT_EQ(hccl.Query(handleId0), i + 1);
        EXPECT_EQ(hccl.Wait(handleId0), HCCL_SUCCESS);
    }

    // AlltoAll Wait
    for (size_t i = 0U; i < 2; ++i) {
        ASSERT_EQ(hccl.Query(handleId1), i);
        hcclMsgArea->commMsg.singleMsg.finishedTurnCnt[1].cnt++;
        ASSERT_EQ(hccl.Query(handleId1), i + 1);
        EXPECT_EQ(hccl.Wait(handleId1), HCCL_SUCCESS);
    }

    // AllGather Wait
    for (size_t i = 0U; i < 3; ++i) {
        ASSERT_EQ(hccl.Query(handleId2), i);
        hcclMsgArea->commMsg.singleMsg.finishedTurnCnt[2].cnt++;
        ASSERT_EQ(hccl.Query(handleId2), i + 1);
        EXPECT_EQ(hccl.Wait(handleId2), HCCL_SUCCESS);
    }

    // Finalize
    std::thread t1(FinalizeThreadFunc, 0, std::ref(hccl));
    std::thread t2(ReadFinalizeMsgThreadFunc, 3, hcclMsgArea);
    t1.join();
    t2.join();
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[3].addMsg.v0Msg.valid, ~HCCL_MSG_VALID_MASK);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[3].commType.msgType, ControlMsgType::HCCL_CMD_FINALIZE);
}

// Critical test: After the message area hcclSendMsg exceeds MAX_MSG_COUNT, it is recycled
TEST_F(HcclCommonTestSuite, HcclCriticalTest_MsgUsageOverMAX_MSG_COUNT)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    for (int i = 0; i < MAX_MSG_COUNT; ++i) {
        hccl.InterHcclGroupSync(0, 0);
    }
    hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.valid =
        ~hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.valid;
    HcclHandle handleId = hccl.AllReduce(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 3);
    EXPECT_EQ(handleId, INVALID_HANDLE_ID);
}

// Abnormal test: Commit occurs before Prepare, and the CommitTurnCnt value will not be written
TEST_F(HcclAbnormalTestSuite, CommitTurnCntNotUpdate_CommitBeforePrepare)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    hccl.Commit(0);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, 0);
}

// Abnormal test: Wait and Query occur before Prepare, intercept exit
TEST_F(HcclAbnormalTestSuite, ReturnInvalid_WaitAndQueryBeforePrepare)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    EXPECT_EQ(hccl.Wait(0), HCCL_FAILED);
    EXPECT_EQ(hccl.Query(0), HCCL_FAILED);
}

// Abnormal test: Wait occurs before Commit, intercepting exit
TEST_F(HcclAbnormalTestSuite, ReturnInvalid_WaitBeforeCommit)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    HcclHandle handleId = hccl.AllReduce(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 3);
    ASSERT_EQ(handleId, 0);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.repeatCnt, 3);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].commType.prepareType, HcclCMDType::HCCL_CMD_ALLREDUCE);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].opType, HcclReduceOp::HCCL_REDUCE_SUM);
    EXPECT_EQ(hccl.Wait(handleId), HCCL_FAILED);
}

// Abnormal test: hccl is not initialized to call each interface, and no core dump occurs when hccl is destroyed
TEST_F(HcclAbnormalTestSuite, HcclNotInit)
{
    char ch[2048] = {'\0'}; // Prevent stack memory reuse
    Hccl hccl;
    auto hanleId1 = hccl.AllReduce(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 3);
    EXPECT_EQ(hanleId1, INVALID_HANDLE_ID);
    auto hanleId2 = hccl.AllGather(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        HcclDataType::HCCL_DATA_TYPE_INT8, 0, 3);
    EXPECT_EQ(hanleId2, INVALID_HANDLE_ID);
    auto hanleId3 = hccl.ReduceScatter(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 100 * 3, 3);
    EXPECT_EQ(hanleId3, INVALID_HANDLE_ID);
    hccl.Finalize();

    hccl.Commit(hanleId1);
    auto ret = hccl.Wait(hanleId1);
    EXPECT_EQ(ret, HCCL_FAILED);
    ret = hccl.Query(hanleId1);
    EXPECT_EQ(ret, HCCL_FAILED);
}

// Abnormal test: Too many Prepare messages sent by the hccl client
TEST_F(HcclAbnormalTestSuite, PrepareCntLargerThanHCCL_MAX_HANDLE_ID)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    HcclHandle handleId;
    for (int i = 0; i < HCCL_MAX_HANDLE_ID; ++i) {
        handleId = hccl.AllReduce(
            reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
            HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 3);
        EXPECT_EQ(handleId, i);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[i].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[i].addMsg.v0Msg.repeatCnt, 3);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[i].commType.prepareType, HcclCMDType::HCCL_CMD_ALLREDUCE);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[i].opType, HcclReduceOp::HCCL_REDUCE_SUM);
    }
    handleId = hccl.AllReduce(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 3);

    EXPECT_EQ(handleId, INVALID_HANDLE_ID);
    EXPECT_NE(hcclMsgArea->commMsg.singleMsg.sendMsgs[63].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);
}

// Abnormal test: Communication message with Repeat=0 is intercepted by the client and not sent
TEST_F(HcclAbnormalTestSuite, PrepareFailed_RepeatIs0)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    HcclHandle handleId = hccl.AllReduce(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 0);
    EXPECT_EQ(handleId, INVALID_HANDLE_ID);
    EXPECT_NE(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);
}

// Abnormal test: Communication messages with illegal dtype are intercepted and not sent by the client
TEST_F(HcclAbnormalTestSuite, PrepareFailed_InvalidDtype)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    HcclHandle handleId = hccl.AllReduce(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        HcclDataType::HCCL_DATA_TYPE_RESERVED, HcclReduceOp::HCCL_REDUCE_SUM, 1);
    EXPECT_EQ(handleId, INVALID_HANDLE_ID);
    EXPECT_NE(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);

    handleId = hccl.AllReduce(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        static_cast<HcclDataType>(-1), HcclReduceOp::HCCL_REDUCE_SUM, 1);
    EXPECT_EQ(handleId, INVALID_HANDLE_ID);
    EXPECT_NE(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);
}

// sendBuf/recvBuf nullptr, HCCL_REDUCE_RESERVED for AllReduce.
TEST_F(HcclAbnormalTestSuite, PrepareFailed_CheckCommonPrepareParamValid)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    HcclHandle handleId =
        hccl.AllReduce(nullptr, nullptr, 100, HcclDataType::HCCL_DATA_TYPE_RESERVED, HcclReduceOp::HCCL_REDUCE_SUM, 1);
    EXPECT_EQ(handleId, INVALID_HANDLE_ID);
    EXPECT_NE(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);

    handleId = hccl.AllReduce(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        static_cast<HcclDataType>(-1), HcclReduceOp::HCCL_REDUCE_SUM, 1);
    EXPECT_EQ(handleId, INVALID_HANDLE_ID);
    EXPECT_NE(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v0Msg.valid, HCCL_MSG_VALID_MASK);
}

// Abnormal test: Wait, Query and Commit interface handleId out of bounds
TEST_F(HcclAbnormalTestSuite, WaitAndQueryFailed_HanldIdOutOfRange)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));

    auto ret = hccl.Wait(-1);
    EXPECT_EQ(ret, HCCL_FAILED);
    ret = hccl.Query(-1);
    EXPECT_EQ(ret, HCCL_FAILED);
    ret = hccl.Wait(HCCL_MAX_HANDLE_ID);
    EXPECT_EQ(ret, HCCL_FAILED);
    ret = hccl.Query(HCCL_MAX_HANDLE_ID);
    EXPECT_EQ(ret, HCCL_FAILED);
}

// Abnormal test: Wait and Query interface, handleId not generated by Prepare, intercept exit
TEST_F(HcclAbnormalTestSuite, WaitAndQueryFailed_HandleIdNotFromPrepare)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    EXPECT_EQ(hccl.Query(1), HCCL_FAILED);
    EXPECT_EQ(hccl.Wait(1), HCCL_FAILED);
}

// Test content: AlltoAll Prepare 33 times, the last time failed
TEST_F(HcclAbnormalTestSuite, AlltoAll_FailedWhenPrepare33)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    for (int i = 0; i < 63; ++i) {
        HcclHandle handleId = hccl.AlltoAll(
            reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
            HcclDataType::HCCL_DATA_TYPE_INT8, 100 * 8);
        EXPECT_EQ(handleId, i);
    }
    HcclHandle handleId = hccl.AlltoAll(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        HcclDataType::HCCL_DATA_TYPE_INT8, 100 * 8);
    EXPECT_EQ(handleId, INVALID_HANDLE_ID);
}

// Test content: AlltoAllV Prepare 33 times, the last time failed
TEST_F(HcclAbnormalTestSuite, AlltoAllV_FailedWhenPrepare33)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    std::vector<uint64_t> sendCounts(kRankNum, 10);
    std::vector<uint64_t> recvCounts(kRankNum, 11);
    std::vector<uint64_t> sendOffsets(kRankNum, 12);
    std::vector<uint64_t> recvOffsets(kRankNum, 13);
    for (int i = 0; i < 63; ++i) {
        HcclHandle handleId = hccl.AlltoAllV<true>(
            reinterpret_cast<__gm__ uint8_t*>(0x11), sendCounts.data(), sendOffsets.data(),
            HcclDataType::HCCL_DATA_TYPE_INT8, reinterpret_cast<__gm__ uint8_t*>(0x11), recvCounts.data(),
            recvOffsets.data(), HcclDataType::HCCL_DATA_TYPE_INT8, 1);
        EXPECT_EQ(handleId, i);
    }
    HcclHandle handleId = hccl.AlltoAllV<true>(
        reinterpret_cast<__gm__ uint8_t*>(0x11), sendCounts.data(), sendOffsets.data(),
        HcclDataType::HCCL_DATA_TYPE_INT8, reinterpret_cast<__gm__ uint8_t*>(0x11), recvCounts.data(),
        recvOffsets.data(), HcclDataType::HCCL_DATA_TYPE_INT8, 1);
    EXPECT_EQ(handleId, INVALID_HANDLE_ID);
}

// Test content: AlltoAll abnormal input test
TEST_F(HcclAbnormalTestSuite, AlltoAll_InputParamInvalid_ReturnInvalidHandleId)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    GM_ADDR sendBuf = nullptr;
    HcclHandle handleId = hccl.AlltoAll(
        sendBuf, reinterpret_cast<__gm__ uint8_t*>(0x11), 100, HcclDataType::HCCL_DATA_TYPE_INT8, 100 * 8);
    EXPECT_EQ(handleId, INVALID_HANDLE_ID);

    uint64_t dataCount = 0;
    handleId = hccl.AlltoAll(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), dataCount,
        HcclDataType::HCCL_DATA_TYPE_INT8, 100 * 8);
    EXPECT_EQ(handleId, INVALID_HANDLE_ID);

    uint8_t repeat = 0;
    handleId = hccl.AlltoAll(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        HcclDataType::HCCL_DATA_TYPE_INT8, 100 * 8, repeat);
    EXPECT_EQ(handleId, INVALID_HANDLE_ID);
}

// Test content: AlltoAllV abnormal input test
TEST_F(HcclAbnormalTestSuite, AlltoAllV_InputParamInvalid_ReturnInvalidHandleId)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    std::vector<uint64_t> sendCounts(kRankNum, 10);
    std::vector<uint64_t> recvCounts(kRankNum, 11);
    std::vector<uint64_t> sendOffsets(kRankNum, 12);
    std::vector<uint64_t> recvOffsets(kRankNum, 13);
    GM_ADDR sendBuf = nullptr;
    HcclHandle handleId = hccl.AlltoAllV(
        sendBuf, sendCounts.data(), sendOffsets.data(), HcclDataType::HCCL_DATA_TYPE_INT8,
        reinterpret_cast<__gm__ uint8_t*>(0x11), recvCounts.data(), recvOffsets.data(),
        HcclDataType::HCCL_DATA_TYPE_INT8, 1);
    EXPECT_EQ(handleId, INVALID_HANDLE_ID);

    uint8_t repeat = 0;
    handleId = hccl.AlltoAllV(
        reinterpret_cast<__gm__ uint8_t*>(0x11), sendCounts.data(), sendOffsets.data(),
        HcclDataType::HCCL_DATA_TYPE_INT8, reinterpret_cast<__gm__ uint8_t*>(0x11), recvCounts.data(),
        recvOffsets.data(), HcclDataType::HCCL_DATA_TYPE_INT8, repeat);
    EXPECT_EQ(handleId, INVALID_HANDLE_ID);

    // All 0 input needs to support
    std::vector<uint64_t> InvalidSendCounts(kRankNum, 0);
    handleId = hccl.AlltoAllV(
        reinterpret_cast<__gm__ uint8_t*>(0x11), InvalidSendCounts.data(), sendOffsets.data(),
        HcclDataType::HCCL_DATA_TYPE_INT8, reinterpret_cast<__gm__ uint8_t*>(0x11), recvCounts.data(),
        recvOffsets.data(), HcclDataType::HCCL_DATA_TYPE_INT8, 1);
    EXPECT_NE(handleId, INVALID_HANDLE_ID);

    handleId = hccl.AlltoAllV(
        reinterpret_cast<__gm__ uint8_t*>(0x11), sendCounts.data(), sendOffsets.data(),
        HcclDataType::HCCL_DATA_TYPE_INT8, reinterpret_cast<__gm__ uint8_t*>(0x11), nullptr, recvOffsets.data(),
        HcclDataType::HCCL_DATA_TYPE_INT8, 1);
    EXPECT_EQ(handleId, INVALID_HANDLE_ID);
}

// Combined test: Use 3 threads to simulate cores with different blockIdx,
// only core 0 can write, and all cores can read the message area
TEST_F(HcclCombineTestSuite, AlltoAllV_3CoresWork_OnlyCore0Write)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);

    int64_t blockIdx = 2;
    std::thread t1(AlltoAllVThreadFunc, blockIdx, hcclCombineOpParam, false);
    t1.join();

    blockIdx = 0;
    std::thread t2(AlltoAllVThreadFunc, blockIdx, hcclCombineOpParam, true);
    t2.join();

    blockIdx = 1;
    std::thread t3(AlltoAllVThreadFunc, blockIdx, hcclCombineOpParam, true);
    t3.join();
}

class Mc2InitTilingTest {
    uint32_t version = 0U;
    uint32_t mc2HcommCnt = 0U;
    uint32_t offset[MAX_CC_TILING_NUM] = {0U};
    uint8_t debugMode = 0U;
    uint8_t preparePosition = 0U;
    uint16_t queueNum = 0U;
    uint16_t commBlockNum = 0U;
    uint8_t devType = 0U;
    char reserved[17] = {0U};
};

struct Mc2CcTilingTest {
    uint8_t skipLocalRankCopy;
    uint8_t skipBufferWindowCopy;
    uint8_t stepSize;
    uint8_t version;
    char reserved[12];
    char groupName[128];
    char algConfig[128];
    uint32_t opType;
    uint32_t reduceType;
};

class Mc2TilingTest {
    Mc2InitTilingTest init;
    Mc2CcTilingTest cc;
};

// New tiling test, normal call
TEST_F(HcclCommonTestSuite, TilingV1_AllReduce_Repeat1)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);
    Mc2InitTilingTest mc2InitTiling;
    Mc2CcTilingTest mc2CcTiling;
    mc2CcTiling.opType = (uint32_t)HcclCMDType::HCCL_CMD_ALLREDUCE;
    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam), static_cast<__gm__ void*>(&mc2InitTiling));
    auto ret = hccl.SetCcTiling(static_cast<__gm__ void*>(&mc2CcTiling));
    HcclHandle handleId = hccl.AllReduce(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 1);
    EXPECT_EQ(handleId, 0);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v1Msg.version, HcclTilingVersion::NEW_TILING_VERSION);
    EXPECT_EQ(
        hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v1Msg.ccOpTilingData,
        reinterpret_cast<uint64_t>(&mc2CcTiling));
    EXPECT_EQ(
        hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v1Msg.xorCheck,
        GenXorForHcclMsg(&hcclMsgArea->commMsg.singleMsg.sendMsgs[0]));
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, 0);
    hccl.Commit(handleId);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].valid, COMMIT_VALID_MASK);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, 1);
    ASSERT_EQ(hccl.Query(handleId), 0);
    hcclMsgArea->commMsg.singleMsg.finishedTurnCnt[0].cnt++;
    ASSERT_EQ(hccl.Query(handleId), 1);
    EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);

    // hccl ctx interface verification
    EXPECT_EQ(hccl.GetRankDim(), kRankNum);
    EXPECT_EQ(hccl.GetRankId(), 0);
    EXPECT_EQ(hccl.GetWindowsInAddr(0), nullptr);
    EXPECT_EQ(hccl.GetWindowsOutAddr(0), nullptr);
}

// Abnormal call: calling initv1 and v0
TEST_F(HcclCommonTestSuite, TilingV1_AllReduce_Init_InitV1)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);
    Mc2InitTilingTest mc2InitTiling;
    Mc2CcTilingTest mc2CcTiling;
    mc2CcTiling.opType = (uint32_t)HcclCMDType::HCCL_CMD_ALLREDUCE;

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam), static_cast<__gm__ void*>(&mc2InitTiling));
    auto ret = hccl.SetCcTiling(static_cast<__gm__ void*>(&mc2CcTiling));
    EXPECT_EQ(ret, HCCL_FAILED);
}

// Call SetCctiling directly without calling Init
TEST_F(HcclCommonTestSuite, TilingV1_AllReduce_Prepare_noInitV1)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);
    Mc2CcTilingTest mc2CcTiling;
    mc2CcTiling.opType = (uint32_t)HcclCMDType::HCCL_CMD_ALLREDUCE;

    Hccl hccl;
    auto ret = hccl.SetCcTiling(static_cast<__gm__ void*>(&mc2CcTiling));
    EXPECT_EQ(ret, HCCL_FAILED);
}

// Call InitV0 Call set
TEST_F(HcclCommonTestSuite, TilingV1_AllReduce_Init_PrepareV1)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);
    Mc2InitTilingTest mc2InitTiling;
    Mc2CcTilingTest mc2CcTiling;
    mc2CcTiling.opType = (uint32_t)HcclCMDType::HCCL_CMD_ALLREDUCE;

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam));
    auto ret = hccl.SetCcTiling(static_cast<__gm__ void*>(&mc2CcTiling));
    EXPECT_EQ(ret, HCCL_FAILED);
}

// Set exception opType
TEST_F(HcclCommonTestSuite, TilingV1_AllReduce_OpType_100)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);
    Mc2InitTilingTest mc2InitTiling;
    Mc2CcTilingTest mc2CcTiling;
    mc2CcTiling.opType = (uint32_t)ControlMsgType::HCCL_CMD_FINALIZE;

    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam), static_cast<__gm__ void*>(&mc2InitTiling));
    auto ret = hccl.SetCcTiling(static_cast<__gm__ void*>(&mc2CcTiling));
    EXPECT_EQ(ret, HCCL_FAILED);
}

TEST_F(HcclCommonTestSuite, BatchWrite_InvalidPrepare)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    IbVerbsData ibData;
    HcclCombineOpParam hcclCombineOpParam{
        reinterpret_cast<uintptr_t>(workSpace.data()), workSpaceSize, 0, kRankNum, 0, {0}, {0}, {0}, true, &ibData};
    Mc2InitTilingTest mc2InitTiling;
    Mc2CcTilingTest mc2CcTiling;
    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam), static_cast<__gm__ void*>(&mc2InitTiling));

    mc2CcTiling.opType = (uint32_t)HcclCMDType::HCCL_CMD_BATCH_WRITE;
    auto ret = hccl.SetCcTiling(static_cast<__gm__ void*>(&mc2CcTiling));
    HcclHandle handleId = hccl.BatchWrite(reinterpret_cast<__gm__ uint8_t*>(0), 1);
    EXPECT_EQ(handleId, INVALID_HANDLE_ID);
}

TEST_F(HcclCommonTestSuite, BatchWrite_Prepare)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    IbVerbsData ibData[2] = {
        {{0, 0x11, 0}, {0, 0x22, 0}, {0, 0x33, 0}, {0, 0x44, 0}},
        {{0, 0x55, 0}, {0, 0x66, 0}, {0, 0x77, 0}, {0, 0x88, 0}}};
    HcclCombineOpParam hcclCombineOpParam{
        reinterpret_cast<uintptr_t>(workSpace.data()), workSpaceSize, 0, kRankNum, 0, {0}, {0}, {0}, true, &ibData[0]};
    Mc2InitTilingTest mc2InitTiling;
    mc2InitTiling.devType = 1U;
    Mc2CcTilingTest mc2CcTiling;
    mc2CcTiling.opType = (uint32_t)HcclCMDType::HCCL_CMD_BATCH_WRITE;
    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam), static_cast<__gm__ void*>(&mc2InitTiling));
    auto ret = hccl.SetCcTiling(static_cast<__gm__ void*>(&mc2CcTiling));
    HcclHandle handleId = hccl.BatchWrite(reinterpret_cast<__gm__ uint8_t*>(0x11), 1);
    EXPECT_EQ(handleId, 0);
    EXPECT_EQ(hccl.GetRankDim(), kRankNum);
    EXPECT_EQ(hccl.GetRankId(), 0);
    EXPECT_EQ((uint64_t)hccl.GetWindowsInAddr(0), 0x33);
    EXPECT_EQ((uint64_t)hccl.GetWindowsOutAddr(0), 0x44);
    EXPECT_EQ((uint64_t)hccl.GetWindowsInAddr(1), 0x55);
    EXPECT_EQ((uint64_t)hccl.GetWindowsOutAddr(1), 0x66);
}

TEST_F(HcclCommonTestSuite, All2AllV_FineGrainedSend)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);
    Mc2InitTilingTest mc2InitTiling;
    Mc2CcTilingTest mc2CcTiling;
    mc2CcTiling.stepSize = 1U;
    mc2CcTiling.opType = (uint32_t)HcclCMDType::HCCL_CMD_ALLTOALLV;
    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam), static_cast<__gm__ void*>(&mc2InitTiling));
    auto ret = hccl.SetCcTiling(static_cast<__gm__ void*>(&mc2CcTiling));
    std::vector<uint64_t> tmpCounts(kRankNum, 10);
    HcclHandle handleId = hccl.AlltoAllV(
        reinterpret_cast<__gm__ uint8_t*>(0x11), tmpCounts.data(), tmpCounts.data(), HcclDataType::HCCL_DATA_TYPE_INT8,
        reinterpret_cast<__gm__ uint8_t*>(0x11), tmpCounts.data(), tmpCounts.data(), HcclDataType::HCCL_DATA_TYPE_INT8);
    EXPECT_EQ(handleId, 0);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v1Msg.version, HcclTilingVersion::NEW_TILING_VERSION);
    EXPECT_EQ(
        hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v1Msg.ccOpTilingData,
        reinterpret_cast<uint64_t>(&mc2CcTiling));
    EXPECT_EQ(
        hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v1Msg.xorCheck,
        GenXorForHcclMsg(&hcclMsgArea->commMsg.singleMsg.sendMsgs[0]));
    uint16_t sliceList[kRankNum] = {0, 1, 2, 3, 4, 5, 6, 7};
    for (int i = 0; i < kRankNum; ++i) {
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, i);
        uint16_t sliceId;
        ret = hccl.Iterate<false>(handleId, &sliceId, 1);
        EXPECT_EQ(ret, 1);
        EXPECT_EQ(sliceId, sliceList[i]);
        hccl.Commit(handleId);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].valid, COMMIT_VALID_MASK);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, i + 1);
        ASSERT_EQ(hccl.Query(handleId), i);
        hcclMsgArea->commMsg.singleMsg.finishedTurnCnt[0].cnt++;
        ASSERT_EQ(hccl.Query(handleId), i + 1);
        EXPECT_EQ(hccl.Wait(handleId), HCCL_SUCCESS);
    }
}

TEST_F(HcclCommonTestSuite, All2AllV_FineGrainedRecv)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);
    Mc2InitTilingTest mc2InitTiling;
    Mc2CcTilingTest mc2CcTiling;
    mc2CcTiling.stepSize = 1U;
    mc2CcTiling.opType = (uint32_t)HcclCMDType::HCCL_CMD_ALLTOALLV;
    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam), static_cast<__gm__ void*>(&mc2InitTiling));
    auto ret = hccl.SetCcTiling(static_cast<__gm__ void*>(&mc2CcTiling));
    std::vector<uint64_t> tmpCounts(kRankNum, 10);
    HcclHandle handleId = hccl.AlltoAllV(
        reinterpret_cast<__gm__ uint8_t*>(0x11), tmpCounts.data(), tmpCounts.data(), HcclDataType::HCCL_DATA_TYPE_INT8,
        reinterpret_cast<__gm__ uint8_t*>(0x11), tmpCounts.data(), tmpCounts.data(), HcclDataType::HCCL_DATA_TYPE_INT8);
    EXPECT_EQ(handleId, 0);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v1Msg.version, HcclTilingVersion::NEW_TILING_VERSION);
    EXPECT_EQ(
        hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v1Msg.ccOpTilingData,
        reinterpret_cast<uint64_t>(&mc2CcTiling));
    EXPECT_EQ(
        hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v1Msg.xorCheck,
        GenXorForHcclMsg(&hcclMsgArea->commMsg.singleMsg.sendMsgs[0]));
    uint16_t sliceList[kRankNum] = {0, 7, 6, 5, 4, 3, 2, 1};
    uint16_t sliceId;
    for (int i = 0; i < kRankNum; ++i) {
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, i);
        hccl.Commit(handleId);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].valid, COMMIT_VALID_MASK);
        EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, i + 1);
        hcclMsgArea->commMsg.singleMsg.finishedTurnCnt[0].cnt++;
        ret = hccl.Iterate<true>(handleId, &sliceId, 1);
        EXPECT_EQ(ret, 1);
        EXPECT_EQ(sliceId, sliceList[i]);
        ASSERT_EQ(hccl.Query(handleId), i + 1);
    }
    EXPECT_EQ(hccl.Iterate<true>(handleId, &sliceId, 1), 0);
}

TEST_F(HcclCommonTestSuite, All2AllV_FineGrainedRecvWithStepSize)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);
    Mc2InitTilingTest mc2InitTiling;
    Mc2CcTilingTest mc2CcTiling;
    mc2CcTiling.stepSize = 4U;
    mc2CcTiling.opType = (uint32_t)HcclCMDType::HCCL_CMD_ALLTOALLV;
    uint8_t repeat = 8U;
    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam), static_cast<__gm__ void*>(&mc2InitTiling));
    auto ret = hccl.SetCcTiling(static_cast<__gm__ void*>(&mc2CcTiling));
    std::vector<uint64_t> tmpCounts(kRankNum, 10);
    HcclHandle handleId = hccl.AlltoAllV<true>(
        reinterpret_cast<__gm__ uint8_t*>(0x11), tmpCounts.data(), tmpCounts.data(), HcclDataType::HCCL_DATA_TYPE_INT8,
        reinterpret_cast<__gm__ uint8_t*>(0x11), tmpCounts.data(), tmpCounts.data(), HcclDataType::HCCL_DATA_TYPE_INT8,
        repeat);
    EXPECT_EQ(handleId, 0);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v1Msg.version, HcclTilingVersion::NEW_TILING_VERSION);
    EXPECT_EQ(
        hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v1Msg.ccOpTilingData,
        reinterpret_cast<uint64_t>(&mc2CcTiling));
    EXPECT_EQ(
        hcclMsgArea->commMsg.singleMsg.sendMsgs[0].addMsg.v1Msg.xorCheck,
        GenXorForHcclMsg(&hcclMsgArea->commMsg.singleMsg.sendMsgs[0]));
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].valid, COMMIT_VALID_MASK);
    EXPECT_EQ(hcclMsgArea->commMsg.singleMsg.commitTurnCnt[0].cnt, kRankNum * repeat);
    uint16_t sliceList[kRankNum] = {0, 7, 6, 5, 4, 3, 2, 1};
    uint16_t sliceId[4];
    const uint8_t sliceCnt = sizeof(sliceId) / sizeof(sliceId[0]);
    for (uint8_t i = 0U; i < repeat; ++i) {
        for (uint32_t j = 0; j < kRankNum / mc2CcTiling.stepSize; ++j) {
            hcclMsgArea->commMsg.singleMsg.finishedTurnCnt[0].cnt += mc2CcTiling.stepSize;
            ret = hccl.Iterate<true>(handleId, sliceId, sliceCnt);
            EXPECT_EQ(ret, sliceCnt);
            for (uint8_t k = 0; k < sizeof(sliceId) / sizeof(sliceId[0]); ++k) {
                EXPECT_EQ(sliceId[k], sliceList[k % sliceCnt + j * sliceCnt]);
            }
            ASSERT_EQ(hccl.Query(handleId), mc2CcTiling.stepSize * (1 + j + i * kRankNum / mc2CcTiling.stepSize));
        }
    }
    EXPECT_EQ(hccl.Iterate<true>(handleId, sliceId, sliceCnt), 0);
}

constexpr HcclServerConfig HCCL_CFG = {CoreType::ON_AIV, 10};
TEST_F(HcclCommonTestSuite, TestHcclConfig)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);
    Mc2InitTilingTest mc2InitTiling;
    Mc2CcTilingTest mc2CcTiling;
    mc2CcTiling.opType = (uint32_t)HcclCMDType::HCCL_CMD_ALLREDUCE;
    Hccl<HcclServerType::HCCL_SERVER_TYPE_AICPU, HCCL_CFG> hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam), static_cast<__gm__ void*>(&mc2InitTiling));
    auto ret = hccl.SetCcTiling(static_cast<__gm__ void*>(&mc2CcTiling));
    HcclHandle handleId = hccl.AllReduce(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 1);
    EXPECT_EQ(handleId, 0);
}

TEST_F(HcclCommonTestSuite, TestInitV2)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);
    Mc2TilingTest tilingData;
    tilingData.cc.opType = (uint32_t)HcclCMDType::HCCL_CMD_ALLREDUCE;
    Hccl hccl;
    hccl.InitV2(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam), static_cast<const void*>(&tilingData));
    auto ret = hccl.SetCcTilingV2(sizeof(Mc2InitTilingTest));
    HcclHandle handleId = hccl.AllReduce(
        reinterpret_cast<__gm__ uint8_t*>(0x11), reinterpret_cast<__gm__ uint8_t*>(0x11), 100,
        HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 1);
    EXPECT_EQ(handleId, 0);
}

TEST_F(HcclCommonTestSuite, TestInitV1V2Mixed)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);
    Mc2TilingTest tilingData;
    tilingData.cc.opType = (uint32_t)HcclCMDType::HCCL_CMD_ALLREDUCE;
    Hccl hccl;
    hccl.Init(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam), static_cast<__gm__ void*>(&tilingData));
    auto ret = hccl.SetCcTilingV2(sizeof(Mc2InitTilingTest));
    EXPECT_EQ(ret, HCCL_FAILED);
}

TEST_F(HcclCommonTestSuite, TestInitV2V1Mixed)
{
    std::vector<uint8_t> workSpace(workSpaceSize + 1024);
    HcclMsgArea* hcclMsgArea = GetHcclMsgArea(workSpace.data());
    HcclCombineOpParam hcclCombineOpParam = GetHcclCombineOpParam(workSpace);
    Mc2TilingTest tilingData;
    tilingData.cc.opType = (uint32_t)HcclCMDType::HCCL_CMD_ALLREDUCE;
    Hccl hccl;
    hccl.InitV2(reinterpret_cast<GM_ADDR>(&hcclCombineOpParam), static_cast<const void*>(&tilingData));
    auto ret = hccl.SetCcTiling(static_cast<__gm__ void*>(&tilingData.cc));
    EXPECT_EQ(ret, HCCL_FAILED);
}
