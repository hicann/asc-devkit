/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <cstddef>
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include <vector>
#define private public
#include "include/adv_api/hccl/hccl.h"
#undef private

using namespace AscendC;
using namespace HcclApi;

namespace {
class PeerOnlyPrepareTest : public ::testing::Test {
protected:
    using Impl = HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, DEFAULT_CFG>;
    Impl impl;
    CommonPrepareParamCcu handles[2]{};
    HcclCombineOpParam context{};
    Mc2CcTilingInner tiling{};
    void SetUp() override
    {
        context.rankNum = 2;
        context.rankId = 1;
        impl.newCcuFlag_ = false;
        impl.hcclContext_ = &context;
        impl.handleParamGM_ = handles;
        impl.curVersion_ = HcclTilingVersion::NEW_TILING_VERSION;
        tiling.opType = static_cast<uint32_t>(HcclCMDType::HCCL_CMD_REDUCE_SCATTER);
        tiling.srcDataType = static_cast<uint8_t>(HCCL_DATA_TYPE_INT8);
        tiling.dstDataType = tiling.srcDataType;
        tiling.reduceType = static_cast<uint32_t>(HCCL_REDUCE_MAX);
        impl.ccOpTilingDataTable_[tiling.opType] = reinterpret_cast<uint64_t>(&tiling);
        for (auto& handle : handles) {
            handle.commType.prepareType = HcclCMDType::HCCL_CMD_REDUCE_SCATTER;
            handle.sendBuf = reinterpret_cast<GM_ADDR>(0x100000);
            handle.recvBuf = reinterpret_cast<GM_ADDR>(0x200000);
            handle.count = 257;
            handle.strideCount = 1024;
        }
    }
};

TEST_F(PeerOnlyPrepareTest, SnapshotSurvivesTilingChangeAndRepeat)
{
    tiling.skipLocalRankCopy = 1;
    impl.InitCcuParam(0);
    tiling.skipLocalRankCopy = 0;
    impl.InitCcuParam(1);
    EXPECT_EQ(handles[0].skipLocalRankCopy, 1);
    EXPECT_EQ(handles[1].skipLocalRankCopy, 0);
    for (uint32_t repeat = 0; repeat < 3; ++repeat) {
        impl.ccuParam_.repeatIndex = repeat;
        for (uint32_t handle = 0; handle < 2; ++handle) {
            impl.CcuPrepareForReduceScatterPeerOnlyM2M(&handles[handle]);
            EXPECT_EQ(impl.xnData_[1], 0x100000U + repeat * 257U);
            EXPECT_EQ(impl.xnData_[2], 0x200000U + repeat * 257U);
            EXPECT_EQ(impl.xnData_[4], 1024U);
            EXPECT_EQ(impl.xnData_[5], handle == 0 ? 1U : 0U);
            EXPECT_EQ(impl.xnData_[7], 257U);
            EXPECT_EQ(impl.xnData_[8], UINT64_MAX - 1);
        }
    }
}

TEST_F(PeerOnlyPrepareTest, MissingTilingResetsToSafeDefault)
{
    handles[0].skipLocalRankCopy = 1;
    impl.curVersion_ = HcclTilingVersion::DEPRECATED_TILING_VERSION;
    impl.InitCcuParam(0);
    EXPECT_EQ(handles[0].skipLocalRankCopy, 0);
}

TEST_F(PeerOnlyPrepareTest, OnlineTilingUsesRelativeOffset)
{
    struct TilingBlock {
        Mc2InitTilingInner init{};
        Mc2CcTilingInner cc{};
    } block;
    block.cc = tiling;
    block.cc.skipLocalRankCopy = 1;
    impl.curVersion_ = HcclTilingVersion::ONLINE_COMPILATION_TILING_VERSION;
    impl.tilingBaseAddr_ = reinterpret_cast<uint64_t>(&block);
    impl.ccOpTilingDataTable_[tiling.opType] = offsetof(TilingBlock, cc);
    impl.InitCcuParam(0);
    EXPECT_EQ(handles[0].skipLocalRankCopy, 1);
}

TEST_F(PeerOnlyPrepareTest, RankStrideAndChunkBoundaries)
{
    const uint64_t chunk = 256ULL * 1024 * 1024;
    for (uint32_t rank = 0; rank < 2; ++rank) {
        context.rankId = rank;
        for (uint64_t count : std::vector<uint64_t>{0, chunk, chunk + 128, 2 * chunk}) {
            handles[0].count = count;
            handles[0].strideCount = 0;
            impl.InitCcuParam(0);
            impl.ccuParam_.repeatIndex = 0;
            impl.CcuPrepareForReduceScatterPeerOnlyM2M(&handles[0]);
            EXPECT_EQ(impl.xnData_[4], rank * count);
            EXPECT_EQ(impl.xnData_[5], 0U);
            const uint64_t chunks = count == 0 ? 0 : (count - 1) / chunk + 1;
            EXPECT_EQ(impl.xnData_[8], UINT64_MAX - chunks);
            EXPECT_EQ(impl.xnData_[7], chunks == 0 ? 0 : count - (chunks - 1) * chunk);
        }
    }
}

TEST_F(PeerOnlyPrepareTest, WorkspaceLayoutUnchanged)
{
    EXPECT_EQ(offsetof(CommonPrepareParamCcu, paramExt), offsetof(CommonPrepareParam, paramExt));
    EXPECT_EQ(offsetof(CommonPrepareParamCcu, wParamExt), offsetof(CommonPrepareParam, wParamExt));
    EXPECT_EQ(offsetof(CommonPrepareParamCcu, skipLocalRankCopy), offsetof(CommonPrepareParamCcu, repeat) + 1);
    EXPECT_EQ(sizeof(CommonPrepareParamCcu) % 64, 0U);
}

// Covers the CommonPrepareImpl param checks in hccl_ccu_v0.h L453-467:
// dataTypeSize == 0, count > CCU_MAX_COMM_DATA / dataTypeSize and handleId >= HCCL_MAX_HANDLE_ID
// must all be rejected with INVALID_HANDLE_ID, without consuming a handle slot.
class CommonPrepareCheckTest : public ::testing::Test {
protected:
    using Impl = HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, DEFAULT_CFG>;
    Impl impl;
    CommonPrepareParamCcu handles[HCCL_MAX_HANDLE_ID]{};
    HcclCombineOpParam context{};

    CommonPrepareParam MakeParam(
        uint64_t count, HcclDataType dataType, HcclCMDType prepareType = HcclCMDType::HCCL_CMD_ALLREDUCE)
    {
        CommonPrepareParam param{};
        param.commType.prepareType = prepareType;
        param.sendBuf = reinterpret_cast<GM_ADDR>(0x100000);
        param.recvBuf = reinterpret_cast<GM_ADDR>(0x200000);
        param.count = count;
        param.dataType = dataType;
        param.dstDataType = dataType;
        param.op = HCCL_REDUCE_MAX;
        param.strideCount = 0;
        param.repeat = 1;
        return param;
    }

    void SetUp() override
    {
        context.rankNum = 2;
        context.rankId = 1;
        impl.isInited_ = true;
        impl.curHandleId_ = 0;
        impl.workingFlag_ = false;
        impl.hcclContext_ = &context;
        impl.handleParamGM_ = handles;
    }
};

// Payload of count * dataTypeSize beyond CCU_MAX_COMM_DATA is rejected and does not consume a handle.
TEST_F(CommonPrepareCheckTest, PrepareRejectsCountBeyondCcuMaxCommData)
{
    const uint64_t int8MaxCount = CCU_MAX_COMM_DATA / GetHcclDataTypeSize(HCCL_DATA_TYPE_INT8);
    EXPECT_EQ(impl.CommonPrepareImpl<false>(MakeParam(int8MaxCount + 1, HCCL_DATA_TYPE_INT8)), INVALID_HANDLE_ID);
    EXPECT_EQ(impl.curHandleId_, 0);
    // Multi-byte dtype shrinks the element budget by its size.
    const uint64_t fp32MaxCount = CCU_MAX_COMM_DATA / GetHcclDataTypeSize(HCCL_DATA_TYPE_FP32);
    EXPECT_EQ(impl.CommonPrepareImpl<false>(MakeParam(fp32MaxCount + 1, HCCL_DATA_TYPE_FP32)), INVALID_HANDLE_ID);
    EXPECT_EQ(impl.curHandleId_, 0);
}

// count exactly at CCU_MAX_COMM_DATA / dataTypeSize still passes the check and gets a valid handle.
TEST_F(CommonPrepareCheckTest, PrepareAcceptsCountAtCcuMaxCommDataBoundary)
{
    const uint64_t int8MaxCount = CCU_MAX_COMM_DATA / GetHcclDataTypeSize(HCCL_DATA_TYPE_INT8);
    EXPECT_EQ(impl.CommonPrepareImpl<false>(MakeParam(int8MaxCount, HCCL_DATA_TYPE_INT8)), 0);
    EXPECT_EQ(impl.curHandleId_, 1);
    const uint64_t fp32MaxCount = CCU_MAX_COMM_DATA / GetHcclDataTypeSize(HCCL_DATA_TYPE_FP32);
    EXPECT_EQ(impl.CommonPrepareImpl<false>(MakeParam(fp32MaxCount, HCCL_DATA_TYPE_FP32)), 1);
    EXPECT_EQ(impl.curHandleId_, 2);
}

// Once curHandleId_ reaches HCCL_MAX_HANDLE_ID, any further Prepare is rejected; the last valid slot
// below the limit still succeeds.
TEST_F(CommonPrepareCheckTest, PrepareRejectsWhenHandleIdReachesMax)
{
    impl.curHandleId_ = HCCL_MAX_HANDLE_ID;
    EXPECT_EQ(impl.CommonPrepareImpl<false>(MakeParam(1, HCCL_DATA_TYPE_INT8)), INVALID_HANDLE_ID);
    EXPECT_EQ(impl.curHandleId_, HCCL_MAX_HANDLE_ID);
    impl.curHandleId_ = HCCL_MAX_HANDLE_ID - 1;
    EXPECT_EQ(impl.CommonPrepareImpl<false>(MakeParam(1, HCCL_DATA_TYPE_INT8)), HCCL_MAX_HANDLE_ID - 1);
    EXPECT_EQ(impl.curHandleId_, HCCL_MAX_HANDLE_ID);
}

// BATCH_WRITE is the only prepareType whose zero-sized dtype reaches the dataTypeSize == 0 branch of
// the check (other types are filtered out earlier in CommonPrepareImpl).
TEST_F(CommonPrepareCheckTest, PrepareRejectsBatchWriteWithZeroSizedDtype)
{
    const CommonPrepareParam param = MakeParam(1, HCCL_DATA_TYPE_RESERVED, HcclCMDType::HCCL_CMD_BATCH_WRITE);
    EXPECT_EQ(impl.CommonPrepareImpl<false>(param), INVALID_HANDLE_ID);
    EXPECT_EQ(impl.curHandleId_, 0);
}
class CcuMultiTilingTest : public ::testing::Test {
protected:
    using Impl = HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, DEFAULT_CFG>;
    struct TilingBlock {
        Mc2InitTilingInner init{};
        Mc2CcTilingInner cc[2]{};
    } block;
    Impl impl;
    HcclApi::OpResCtx context{};
    CommonPrepareParamCcu handles[HCCL_MAX_HANDLE_ID]{};
    std::vector<uint64_t> signals;
    std::vector<uint64_t> params;
    uint64_t finished = 0;

    static void StoreSignal(uint64_t* address, uint64_t value) { *address = value; }
    static uint8_t LoadSignal(uint8_t* address) { return *address; }

    void SetUp() override
    {
        MOCKER(AscendC::WriteGmBypassDCache<uint64_t>).stubs().will(invoke(StoreSignal));
        MOCKER(AscendC::ReadGmBypassDCache<uint8_t>).stubs().will(invoke(LoadSignal));
        signals.resize(HcclKfcProtocol::KFC_SIGNAL_REGION_NUM * CCU_MAX_MSG_NUM);
        params.resize(CCU_MSG_XN_NUM * CCU_MAX_MSG_NUM);
        context.ckeAddr = reinterpret_cast<uint64_t>(signals.data());
        context.xnAddr = reinterpret_cast<uint64_t>(params.data());
        context.rankSize = 2;
        context.res[1] = 1000;
        context.res[2] = 1;
        for (uint32_t i = 0; i < 2; ++i) {
            block.cc[i].opType =
                static_cast<uint32_t>(i == 0 ? HcclCMDType::HCCL_CMD_ALLREDUCE : HcclCMDType::HCCL_CMD_REDUCE_SCATTER);
            context.opType[i] = block.cc[i].opType;
            block.cc[i].srcDataType = HCCL_DATA_TYPE_FP16;
            block.cc[i].dstDataType = HCCL_DATA_TYPE_FP16;
            block.cc[i].reduceType = i == 0 ? HCCL_REDUCE_SUM : HCCL_REDUCE_MAX;
            context.algInfo[i].offset = offsetof(TilingBlock, cc) + i * sizeof(Mc2CcTilingInner);
            context.algorithmType[i] = i + 10;
            handles[i].commType.prepareType = static_cast<HcclCMDType>(block.cc[i].opType);
            handles[i].dataType = HCCL_DATA_TYPE_FP16;
            handles[i].dstDataType = HCCL_DATA_TYPE_FP16;
            handles[i].count = 32;
            impl.InitHandleInfo(i);
            impl.handleMissionNum_[i] = context.res[2];
            impl.handleReqId_[i] = i;
        }
        for (auto& available : impl.msgQueueIsAvailable_) {
            available = true;
        }
        impl.newCcuFlag_ = true;
        impl.isInited_ = true;
        impl.workingFlag_ = true;
        impl.hcclNewContext_ = &context;
        impl.handleParamGM_ = handles;
        impl.finishCntGM_ = &finished;
        impl.curVersion_ = HcclTilingVersion::ONLINE_COMPILATION_TILING_VERSION;
        impl.tilingBaseAddr_ = reinterpret_cast<uint64_t>(&block);
    }
    void TearDown() override { GlobalMockObject::verify(); }
};

TEST_F(CcuMultiTilingTest, SelectionAndHandleSnapshot)
{
    impl.curHandleId_ = 0;
    CommonPrepareParam param{};
    param.commType.prepareType = HcclCMDType::HCCL_CMD_ALLREDUCE;
    param.dataType = HCCL_DATA_TYPE_FP16;
    param.dstDataType = HCCL_DATA_TYPE_FP16;
    param.count = 32;
    ASSERT_EQ(impl.SetCcTilingV2(context.algInfo[0].offset), HCCL_SUCCESS);
    ASSERT_EQ(impl.CommonPrepareImpl<false>(param), 0);
    ASSERT_EQ(impl.SetCcTilingV2(context.algInfo[1].offset), HCCL_SUCCESS);
    param.commType.prepareType = HcclCMDType::HCCL_CMD_REDUCE_SCATTER;
    ASSERT_EQ(impl.CommonPrepareImpl<false>(param), 1);
    impl.CcuPrepareForOp(0);
    EXPECT_EQ(handles[0].op, HCCL_REDUCE_SUM);
    EXPECT_EQ(impl.GetAlgorithmType(0), 10U);
    EXPECT_EQ(impl.ccuParam_.dieSplitRatioPermille, 1000U);
    impl.CcuPrepareForOp(1);
    EXPECT_EQ(handles[1].op, HCCL_REDUCE_MAX);
    EXPECT_EQ(impl.GetAlgorithmType(1), 11U);
    EXPECT_EQ(impl.handleMissionNum_[0], 1U);
    EXPECT_EQ(impl.handleMissionNum_[1], 1U);
    EXPECT_EQ(impl.ccuParam_.dieSplitRatioPermille, 1000U);
    EXPECT_NE(GetOpId(&handles[0]), GetOpId(&handles[1]));
    impl.ccuUsedXnNum_ = 1;
    impl.xnData_[0] = GetOpId(&handles[0]);
    impl.CcuSendMsg(0, 0);
    impl.xnData_[0] = GetOpId(&handles[1]);
    impl.CcuSendMsg(1, 1);
    EXPECT_EQ(params[0], GetOpId(&handles[0]));
    EXPECT_EQ(params[CCU_MSG_XN_NUM], GetOpId(&handles[1]));
}

TEST_F(CcuMultiTilingTest, SequentialTilingsWrapTheSharedQueue)
{
    impl.workingFlag_ = false;
    impl.curHandleId_ = 0;
    CommonPrepareParam param{};
    param.commType.prepareType = HcclCMDType::HCCL_CMD_ALLREDUCE;
    param.dataType = HCCL_DATA_TYPE_FP16;
    param.count = 32;
    param.repeat = 2;
    for (uint32_t turn = 0; turn < HCCL_MAX_HANDLE_ID; ++turn) {
        ASSERT_EQ(impl.SetCcTilingV2(context.algInfo[turn % 2U].offset), HCCL_SUCCESS);
        param.commType.prepareType = static_cast<HcclCMDType>(block.cc[turn % 2U].opType);
        const auto handle = impl.CommonPrepareImpl<true>(param);
        ASSERT_NE(handle, INVALID_HANDLE_ID);
        ASSERT_EQ(impl.Wait(handle), HCCL_SUCCESS);
        ASSERT_EQ(impl.Wait(handle), HCCL_SUCCESS);
    }
    EXPECT_EQ(impl.globalCurResId_, (2U * HCCL_MAX_HANDLE_ID) % CCU_MAX_MSG_NUM);
    EXPECT_EQ(impl.globalCurWaitId_, impl.globalCurResId_);
    for (auto available : impl.msgQueueIsAvailable_) {
        EXPECT_TRUE(available);
    }
}

TEST_F(CcuMultiTilingTest, RepeatRefillAndFinalizeUseTheSharedCursor)
{
    impl.workingFlag_ = false;
    impl.curHandleId_ = 1;
    impl.handleRepeatCnt_[0] = CCU_MAX_MSG_NUM + 1U;
    impl.CommitMsgInner(0);
    EXPECT_EQ(impl.handleCommitCnt_[0], CCU_MAX_MSG_NUM);
    for (uint32_t turn = 0; turn < CCU_MAX_MSG_NUM + 1U; ++turn) {
        ASSERT_EQ(impl.Wait(0), HCCL_SUCCESS);
    }
    EXPECT_EQ(impl.handleFinishCnt_[0], CCU_MAX_MSG_NUM + 1U);
    EXPECT_EQ(impl.globalCurResId_, 1U);
    impl.workingFlag_ = true;
    impl.Finalize();
    EXPECT_EQ(params[CCU_MSG_XN_NUM], UINT64_MAX);
    EXPECT_EQ(*reinterpret_cast<uint64_t*>(impl.GetCommitCkeAddr(1, 0)), 1U);
    EXPECT_EQ(*reinterpret_cast<uint64_t*>(impl.GetCommitCkeAddr(1, 1)), 0U);
}

TEST_F(CcuMultiTilingTest, FinalizeAfterSerialWaitsWritesOneSentinel)
{
    impl.workingFlag_ = false;
    impl.curHandleId_ = 2;
    impl.Commit(0);
    ASSERT_EQ(impl.Wait(0), HCCL_SUCCESS);
    impl.Commit(1);
    ASSERT_EQ(impl.Wait(1), HCCL_SUCCESS);
    impl.workingFlag_ = true;
    impl.Finalize();
    EXPECT_EQ(params[2U * CCU_MSG_XN_NUM], UINT64_MAX);
    EXPECT_EQ(params[0], 0U);
    EXPECT_EQ(params[CCU_MSG_XN_NUM], 0U);
}

} // namespace
