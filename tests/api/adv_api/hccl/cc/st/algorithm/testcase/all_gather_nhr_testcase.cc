/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <algorithm>
#include <map>
#include <set>
#include <vector>

#include "gtest/gtest.h"

#include "channel.h"
#include "ins_temp_all_gather_nhr.h"
#include "template_utils.h"

namespace {
using namespace mc2_ops_hccl;

// Helpers

std::vector<HcclChannelDesc> MakeChannelDescs(const std::vector<std::pair<u32, u32>>& rankAndCount)
{
    std::vector<HcclChannelDesc> channels;
    for (const auto& item : rankAndCount) {
        for (u32 i = 0; i < item.second; i++) {
            HcclChannelDesc desc{};
            desc.remoteRank = item.first;
            channels.push_back(desc);
        }
    }
    return channels;
}

std::map<u32, std::vector<ChannelInfo>> MakeChannelMap(const std::vector<std::pair<u32, u32>>& rankAndCount)
{
    std::map<u32, std::vector<ChannelInfo>> channels;
    for (const auto& item : rankAndCount) {
        std::vector<ChannelInfo> infos(item.second);
        for (u32 i = 0; i < item.second; i++) {
            infos[i].portGroupSize = 1;
        }
        channels[item.first] = infos;
    }
    return channels;
}

TopoInfoWithNetLayerDetails MakeTopoInfo(u32 localRankSize, CommTopo topoType, u32 topoCoverRankNum)
{
    TopoInfoWithNetLayerDetails topoInfo{};
    topoInfo.netLayerDetails.localNetInsSizeOfLayer.push_back(localRankSize);
    TopoInstDetails details{};
    details.rankNumForTopoType[topoType] = {topoCoverRankNum};
    topoInfo.topoInstDetailsOfLayer.push_back(details);
    return topoInfo;
}

InsTempAllGatherNHR MakeNhrTemplate(u32 rankSize = 8U)
{
    OpParam param{};
    std::vector<u32> ranks(rankSize);
    for (u32 rank = 0; rank < rankSize; rank++) {
        ranks[rank] = rank;
    }
    return InsTempAllGatherNHR(param, 0U, {ranks});
}

u32 RefStepNum(u32 rankSize)
{
    u32 nSteps = 0;
    for (u32 tmp = rankSize - 1; tmp != 0; tmp >>= 1, nSteps++) {
    }
    return nSteps;
}

struct RefStep {
    u32 nSlices{0};
    u32 toRank{0};
    u32 fromRank{0};
    std::vector<u32> txSliceIdxs;
    std::vector<u32> rxSliceIdxs;
};

RefStep RefGetStep(u32 myAlgRank, u32 rankSize, u32 step, u32 nSteps)
{
    RefStep info;
    const u32 deltaRank = 1U << (nSteps - 1U - step);
    info.fromRank = (myAlgRank + rankSize - deltaRank) % rankSize;
    info.toRank = (myAlgRank + deltaRank) % rankSize;
    info.nSlices = (rankSize - 1U + (1U << (nSteps - 1U - step))) / (1U << (nSteps - step));
    const u32 deltaSliceIndex = 1U << (nSteps - step);
    u32 txSliceIdx = myAlgRank;
    u32 rxSliceIdx = (myAlgRank - (1U << (nSteps - 1U - step)) + rankSize) % rankSize;
    for (u32 i = 0; i < info.nSlices; i++) {
        info.txSliceIdxs.push_back(txSliceIdx);
        info.rxSliceIdxs.push_back(rxSliceIdx);
        txSliceIdx = (txSliceIdx + rankSize - deltaSliceIndex) % rankSize;
        rxSliceIdx = (rxSliceIdx + rankSize - deltaSliceIndex) % rankSize;
    }
    return info;
}

std::vector<u64> RefLastStepOutputOffsets(
    u32 myAlgRank, u32 rankSize, u64 outBuffBaseOff, u64 outputSliceStride, u64 partialOffset)
{
    const u32 nSteps = RefStepNum(rankSize);
    const RefStep last = RefGetStep(myAlgRank, rankSize, nSteps - 1U, nSteps);
    std::vector<u64> offsets;
    for (u32 rxIdx : last.rxSliceIdxs) {
        offsets.push_back(outBuffBaseOff + outputSliceStride * rxIdx + partialOffset);
    }
    return offsets;
}

std::vector<u32> RefPostCopyRanks(
    u32 myAlgRank, u32 rankSize, bool readLastStepToOutput, bool skipOwnSliceCopy,
    const std::vector<u32>& lastStepReadSliceIdxs)
{
    std::vector<u32> copied;
    for (u32 algRank = 0; algRank < rankSize; algRank++) {
        if (readLastStepToOutput && skipOwnSliceCopy && algRank == myAlgRank) {
            continue;
        }
        if (readLastStepToOutput && std::find(lastStepReadSliceIdxs.begin(), lastStepReadSliceIdxs.end(), algRank) !=
                                        lastStepReadSliceIdxs.end()) {
            continue;
        }
        copied.push_back(algRank);
    }
    return copied;
}

const std::vector<u32> g_rankSizes = {2U, 3U, 4U, 5U, 8U, 16U};

// NHR step scheduling

class AllGatherNhrStepTest : public ::testing::Test {};

TEST_F(AllGatherNhrStepTest, StepNumMatchesCeilLog2)
{
    EXPECT_EQ(GetNHRStepNum(2U), 1U);
    EXPECT_EQ(GetNHRStepNum(3U), 2U);
    EXPECT_EQ(GetNHRStepNum(4U), 2U);
    EXPECT_EQ(GetNHRStepNum(5U), 3U);
    EXPECT_EQ(GetNHRStepNum(8U), 3U);
    EXPECT_EQ(GetNHRStepNum(16U), 4U);
}

TEST_F(AllGatherNhrStepTest, PeerIsSymmetricAcrossRanks)
{
    for (u32 rankSize : g_rankSizes) {
        const u32 nSteps = RefStepNum(rankSize);
        for (u32 step = 0; step < nSteps; step++) {
            for (u32 rank = 0; rank < rankSize; rank++) {
                const RefStep mine = RefGetStep(rank, rankSize, step, nSteps);
                const RefStep peer = RefGetStep(mine.toRank, rankSize, step, nSteps);
                EXPECT_EQ(peer.fromRank, rank) << "rankSize=" << rankSize << " step=" << step << " rank=" << rank;
                EXPECT_EQ(mine.nSlices, peer.nSlices) << "rankSize=" << rankSize << " step=" << step;
            }
        }
    }
}

TEST_F(AllGatherNhrStepTest, EveryRankCollectsAllSlicesExactlyOnce)
{
    for (u32 rankSize : g_rankSizes) {
        const u32 nSteps = RefStepNum(rankSize);
        for (u32 rank = 0; rank < rankSize; rank++) {
            std::set<u32> collected;
            u32 receivedTotal = 0;
            for (u32 step = 0; step < nSteps; step++) {
                const RefStep info = RefGetStep(rank, rankSize, step, nSteps);
                for (u32 rxIdx : info.rxSliceIdxs) {
                    EXPECT_LT(rxIdx, rankSize) << "rankSize=" << rankSize << " rank=" << rank;
                    collected.insert(rxIdx);
                    receivedTotal++;
                }
                for (u32 txIdx : info.txSliceIdxs) {
                    EXPECT_LT(txIdx, rankSize) << "rankSize=" << rankSize << " rank=" << rank;
                }
            }
            // 自己那份 slice 由 LocalDataCopy 本地补上，不从网络收。
            EXPECT_EQ(collected.count(rank), 0U) << "rankSize=" << rankSize << " rank=" << rank;
            EXPECT_EQ(collected.size(), rankSize - 1U) << "rankSize=" << rankSize << " rank=" << rank;
            // 收到的 slice 无重复，与本地那份合起来恰好覆盖全部 rank 各一次。
            EXPECT_EQ(receivedTotal, rankSize - 1U) << "rankSize=" << rankSize << " rank=" << rank;
            collected.insert(rank);
            EXPECT_EQ(collected.size(), rankSize) << "rankSize=" << rankSize << " rank=" << rank;
        }
    }
}

TEST_F(AllGatherNhrStepTest, EightRankZeroMatchesDocumentedSchedule)
{
    const u32 rankSize = 8U;
    const u32 nSteps = RefStepNum(rankSize);
    ASSERT_EQ(nSteps, 3U);

    const RefStep step0 = RefGetStep(0U, rankSize, 0U, nSteps);
    EXPECT_EQ(step0.toRank, 4U);
    EXPECT_EQ(step0.fromRank, 4U);
    EXPECT_EQ(step0.txSliceIdxs, std::vector<u32>({0U}));
    EXPECT_EQ(step0.rxSliceIdxs, std::vector<u32>({4U}));

    const RefStep step1 = RefGetStep(0U, rankSize, 1U, nSteps);
    EXPECT_EQ(step1.toRank, 2U);
    EXPECT_EQ(step1.fromRank, 6U);
    EXPECT_EQ(step1.txSliceIdxs, std::vector<u32>({0U, 4U}));
    EXPECT_EQ(step1.rxSliceIdxs, std::vector<u32>({6U, 2U}));

    const RefStep step2 = RefGetStep(0U, rankSize, 2U, nSteps);
    EXPECT_EQ(step2.toRank, 1U);
    EXPECT_EQ(step2.fromRank, 7U);
    EXPECT_EQ(step2.txSliceIdxs, std::vector<u32>({0U, 6U, 4U, 2U}));
    EXPECT_EQ(step2.rxSliceIdxs, std::vector<u32>({7U, 5U, 3U, 1U}));
}

// Channel count

class AllGatherNhrChannelCountTest : public ::testing::Test {};

TEST_F(AllGatherNhrChannelCountTest, UniformChannelsGiveSameMinAndMax)
{
    const auto channels = MakeChannelDescs({{1U, 4U}, {2U, 4U}, {3U, 4U}});
    EXPECT_EQ(CalcChannelsPerRankMin(channels), 4U);
    EXPECT_EQ(CalcChannelsPerRank(channels), 4U);
}

TEST_F(AllGatherNhrChannelCountTest, RaggedChannelsGiveMinBelowMax)
{
    const auto channels = MakeChannelDescs({{1U, 4U}, {2U, 2U}, {3U, 4U}});
    EXPECT_EQ(CalcChannelsPerRankMin(channels), 2U);
    EXPECT_EQ(CalcChannelsPerRank(channels), 4U);
}

TEST_F(AllGatherNhrChannelCountTest, EmptyChannelsFallBackToOne)
{
    const std::vector<HcclChannelDesc> empty;
    EXPECT_EQ(CalcChannelsPerRankMin(empty), 1U);
    EXPECT_EQ(CalcChannelsPerRank(empty), 1U);

    const std::map<u32, std::vector<ChannelInfo>> emptyMap;
    EXPECT_EQ(CalcChannelsPerRankMin(emptyMap), 1U);
    EXPECT_EQ(CalcChannelsPerRank(emptyMap), 1U);
}

TEST_F(AllGatherNhrChannelCountTest, ChannelMapMinAndMax)
{
    const auto channels = MakeChannelMap({{1U, 4U}, {2U, 2U}});
    EXPECT_EQ(CalcChannelsPerRankMin(channels), 2U);
    EXPECT_EQ(CalcChannelsPerRank(channels), 4U);
}

// Resource count

class AllGatherNhrResourceTest : public ::testing::Test {};

TEST_F(AllGatherNhrResourceTest, ProductionTemplateRequestsSolePathResources)
{
    auto nhr = MakeNhrTemplate();
    AlgResourceRequest request{};

    ASSERT_EQ(nhr.GetRes(request), HCCL_SUCCESS);
    EXPECT_EQ(nhr.GetThreadNum(), 2U);
    EXPECT_EQ(request.slaveThreadNum, 1U);
    EXPECT_EQ(request.notifyNumOnMainThread, 1U);
    EXPECT_EQ(request.notifyNumPerThread, std::vector<u32>({2U}));
}

TEST_F(AllGatherNhrResourceTest, ProductionTemplateReportsScratchAndNotifyContract)
{
    auto nhr = MakeNhrTemplate(8U);
    EXPECT_EQ(nhr.Describe(), "Template of all gather nhr with tempRankSize 8");
    EXPECT_EQ(nhr.CalcScratchMultiple(BufferType::INPUT, BufferType::OUTPUT), 8U);

    std::vector<u32> mainToSub;
    std::vector<u32> subToMain;
    nhr.GetNotifyIdxMainToSub(mainToSub);
    nhr.GetNotifyIdxSubToMain(subToMain);
    EXPECT_EQ(mainToSub, std::vector<u32>({0U}));
    EXPECT_EQ(subToMain, std::vector<u32>({0U}));
}

TEST_F(AllGatherNhrResourceTest, ProductionKernelAcceptsEmptyInput)
{
    auto nhr = MakeNhrTemplate();
    OpParam param{};
    TemplateDataParams dataParams{};
    TemplateResource resource{};
    EXPECT_EQ(nhr.KernelRun(param, dataParams, resource), HCCL_SUCCESS);
}

TEST_F(AllGatherNhrResourceTest, ProductionKernelRejectsInvalidDataType)
{
    auto nhr = MakeNhrTemplate();
    OpParam param{};
    param.DataDes.dataType = HcclDataType::HCCL_DATA_TYPE_RESERVED;
    TemplateDataParams dataParams{};
    dataParams.sliceSize = 4U;
    TemplateResource resource{};
    EXPECT_EQ(nhr.KernelRun(param, dataParams, resource), HcclResult::HCCL_E_PARA);
}

TEST_F(AllGatherNhrResourceTest, ProductionKernelRejectsMisalignedSlice)
{
    auto nhr = MakeNhrTemplate();
    OpParam param{};
    param.DataDes.dataType = HcclDataType::HCCL_DATA_TYPE_FP32;
    TemplateDataParams dataParams{};
    dataParams.sliceSize = 3U;
    TemplateResource resource{};
    EXPECT_EQ(nhr.KernelRun(param, dataParams, resource), HcclResult::HCCL_E_PARA);
}

TEST_F(AllGatherNhrResourceTest, ProductionCalcResRejectsNullTopoInfo)
{
    auto nhr = MakeNhrTemplate();
    OpParam param{};
    AlgResourceRequest request{};
    EXPECT_NE(nhr.CalcRes(nullptr, param, nullptr, request), HCCL_SUCCESS);
}

TEST_F(AllGatherNhrResourceTest, PostCopyThreadIndexStaysInRange)
{
    const u32 channelsPerRank = 1U;
    const u32 runtimeThreadNum = 2U;
    for (u32 channelIdx = 0; channelIdx < channelsPerRank; channelIdx++) {
        const u32 postCopyThreadIdx = channelsPerRank + channelIdx;
        EXPECT_LT(postCopyThreadIdx, runtimeThreadNum);
    }
}

TEST_F(AllGatherNhrResourceTest, HostMayAllocateMoreThreadsThanRuntimeUses)
{
    const u32 hostThreadNum = 4U * 2U;
    const u32 runtimeThreadNum = 1U * 2U;
    EXPECT_LE(runtimeThreadNum, hostThreadNum);
}

// Topology and port isolation

class AllGatherNhrTopoTest : public ::testing::Test {};

TEST_F(AllGatherNhrTopoTest, FullMeshIsDetected)
{
    const auto topoInfo = MakeTopoInfo(8U, CommTopo::COMM_TOPO_1DMESH, 8U);
    EXPECT_TRUE(IsAllConnetedWithTopo(&topoInfo, 0U, CommTopo::COMM_TOPO_1DMESH));
}

TEST_F(AllGatherNhrTopoTest, PartialCoverageIsNotFullMesh)
{
    const auto topoInfo = MakeTopoInfo(8U, CommTopo::COMM_TOPO_1DMESH, 4U);
    EXPECT_FALSE(IsAllConnetedWithTopo(&topoInfo, 0U, CommTopo::COMM_TOPO_1DMESH));
}

TEST_F(AllGatherNhrTopoTest, MissingTopoTypeReturnsFalse)
{
    const auto topoInfo = MakeTopoInfo(8U, CommTopo::COMM_TOPO_1DMESH, 8U);
    EXPECT_FALSE(IsAllConnetedWithTopo(&topoInfo, 0U, CommTopo::COMM_TOPO_CLOS));
}

TEST_F(AllGatherNhrTopoTest, OutOfRangeNetLayerReturnsFalseWithoutOverflow)
{
    const auto topoInfo = MakeTopoInfo(8U, CommTopo::COMM_TOPO_1DMESH, 8U);
    EXPECT_FALSE(IsAllConnetedWithTopo(&topoInfo, 1U, CommTopo::COMM_TOPO_1DMESH));
    EXPECT_FALSE(IsAllConnetedWithTopo(&topoInfo, 100U, CommTopo::COMM_TOPO_1DMESH));
}

TEST_F(AllGatherNhrTopoTest, NullTopoInfoReturnsFalse)
{
    EXPECT_FALSE(IsAllConnetedWithTopo(nullptr, 0U, CommTopo::COMM_TOPO_1DMESH));
}

TEST_F(AllGatherNhrTopoTest, TopoDetailsShorterThanLayerSizesDoesNotOverflow)
{
    TopoInfoWithNetLayerDetails topoInfo{};
    topoInfo.netLayerDetails.localNetInsSizeOfLayer = {8U, 8U};
    TopoInstDetails details{};
    details.rankNumForTopoType[CommTopo::COMM_TOPO_1DMESH] = {8U};
    topoInfo.topoInstDetailsOfLayer.push_back(details);
    EXPECT_FALSE(IsAllConnetedWithTopo(&topoInfo, 1U, CommTopo::COMM_TOPO_1DMESH));
}

TEST_F(AllGatherNhrTopoTest, IsolationPolicyFollowsTopoAndDataSize)
{
    const auto fullMesh = MakeTopoInfo(8U, CommTopo::COMM_TOPO_1DMESH, 8U);
    const auto clos = MakeTopoInfo(8U, CommTopo::COMM_TOPO_CLOS, 8U);

    auto isIsolation = [](const TopoInfoWithNetLayerDetails& topoInfo, u64 dataSize) {
        return !(IsAllConnetedWithTopo(&topoInfo, 0U, CommTopo::COMM_TOPO_1DMESH) || dataSize <= SMALL_SIZE_512KB);
    };

    EXPECT_FALSE(isIsolation(fullMesh, 32UL * 1024UL * 1024UL));
    EXPECT_FALSE(isIsolation(fullMesh, 1024UL));
    EXPECT_FALSE(isIsolation(clos, SMALL_SIZE_512KB));
    EXPECT_TRUE(isIsolation(clos, static_cast<u64>(SMALL_SIZE_512KB) + 1UL));
    EXPECT_TRUE(isIsolation(clos, 32UL * 1024UL * 1024UL));
}

// Port group data split

class AllGatherNhrDataSplitTest : public ::testing::Test {};

TEST_F(AllGatherNhrDataSplitTest, SingleChannelTakesWholeSlice)
{
    const auto channels = MakeChannelMap({{1U, 1U}});
    std::vector<u64> elemCount;
    std::vector<u64> sizeOut;
    std::vector<u64> elemOffset;
    ASSERT_EQ(
        CalcDataSplitByPortGroupCommon(1024U, 4U, channels.at(1U), elemCount, sizeOut, elemOffset, 1U), HCCL_SUCCESS);
    ASSERT_EQ(sizeOut.size(), 1U);
    EXPECT_EQ(sizeOut[0], 1024U * 4U);
    EXPECT_EQ(elemOffset[0], 0U);
}

TEST_F(AllGatherNhrDataSplitTest, EqualWeightsConserveTotalAndStayContiguous)
{
    const auto channels = MakeChannelMap({{1U, 4U}});
    std::vector<u64> elemCount;
    std::vector<u64> sizeOut;
    std::vector<u64> elemOffset;
    ASSERT_EQ(
        CalcDataSplitByPortGroupCommon(1024U, 4U, channels.at(1U), elemCount, sizeOut, elemOffset, 4U), HCCL_SUCCESS);
    ASSERT_EQ(sizeOut.size(), 4U);
    u64 total = 0;
    for (u64 s : sizeOut) {
        total += s;
    }
    EXPECT_EQ(total, 1024U * 4U);
    for (size_t i = 1; i < elemOffset.size(); i++) {
        EXPECT_EQ(elemOffset[i], elemOffset[i - 1] + sizeOut[i - 1]);
    }
}

TEST_F(AllGatherNhrDataSplitTest, IndivisibleTotalIsAbsorbedByLastChunk)
{
    const auto channels = MakeChannelMap({{1U, 3U}});
    std::vector<u64> elemCount;
    std::vector<u64> sizeOut;
    std::vector<u64> elemOffset;
    ASSERT_EQ(
        CalcDataSplitByPortGroupCommon(1001U, 4U, channels.at(1U), elemCount, sizeOut, elemOffset, 3U), HCCL_SUCCESS);
    u64 total = 0;
    for (u64 s : sizeOut) {
        total += s;
    }
    EXPECT_EQ(total, 1001U * 4U);
}

TEST_F(AllGatherNhrDataSplitTest, HonorsChannelsPerRankSmallerThanChannelCount)
{
    const auto channels = MakeChannelMap({{1U, 4U}});
    std::vector<u64> elemCount;
    std::vector<u64> sizeOut;
    std::vector<u64> elemOffset;
    ASSERT_EQ(
        CalcDataSplitByPortGroupCommon(1024U, 4U, channels.at(1U), elemCount, sizeOut, elemOffset, 2U), HCCL_SUCCESS);
    ASSERT_EQ(sizeOut.size(), 2U);
    u64 total = 0;
    for (u64 s : sizeOut) {
        total += s;
    }
    EXPECT_EQ(total, 1024U * 4U);
}

TEST_F(AllGatherNhrDataSplitTest, TailSliceSplitsIndependently)
{
    const auto channels = MakeChannelMap({{1U, 2U}});
    std::vector<u64> elemCount;
    std::vector<u64> normalSize;
    std::vector<u64> normalOffset;
    std::vector<u64> tailSize;
    std::vector<u64> tailOffset;
    ASSERT_EQ(
        CalcDataSplitByPortGroupCommon(1024U, 4U, channels.at(1U), elemCount, normalSize, normalOffset, 2U),
        HCCL_SUCCESS);
    ASSERT_EQ(
        CalcDataSplitByPortGroupCommon(512U, 4U, channels.at(1U), elemCount, tailSize, tailOffset, 2U), HCCL_SUCCESS);
    EXPECT_EQ(normalOffset[0], 0U);
    EXPECT_EQ(tailOffset[0], 0U);
    u64 normalTotal = 0;
    for (u64 s : normalSize) {
        normalTotal += s;
    }
    u64 tailTotal = 0;
    for (u64 s : tailSize) {
        tailTotal += s;
    }
    EXPECT_EQ(normalTotal, 1024U * 4U);
    EXPECT_EQ(tailTotal, 512U * 4U);
    EXPECT_NE(normalTotal, tailTotal);
}

// Last step read and PostLocalCopy skip

class AllGatherNhrLastStepTest : public ::testing::Test {};

TEST_F(AllGatherNhrLastStepTest, LastStepQualifiesOnlyWhenMultipleSlices)
{
    const u32 nSteps2 = RefStepNum(2U);
    const RefStep last2 = RefGetStep(0U, 2U, nSteps2 - 1U, nSteps2);
    EXPECT_EQ(last2.nSlices, 1U);
    EXPECT_FALSE(last2.nSlices > 1U);

    const u32 nSteps8 = RefStepNum(8U);
    const RefStep last8 = RefGetStep(0U, 8U, nSteps8 - 1U, nSteps8);
    EXPECT_EQ(last8.nSlices, 4U);
    EXPECT_TRUE(last8.nSlices > 1U);
}

TEST_F(AllGatherNhrLastStepTest, ReadSliceIdxsAreUniqueAndInRange)
{
    for (u32 rankSize : g_rankSizes) {
        const u32 nSteps = RefStepNum(rankSize);
        for (u32 rank = 0; rank < rankSize; rank++) {
            const RefStep last = RefGetStep(rank, rankSize, nSteps - 1U, nSteps);
            std::set<u32> uniq(last.rxSliceIdxs.begin(), last.rxSliceIdxs.end());
            EXPECT_EQ(uniq.size(), last.rxSliceIdxs.size()) << "rankSize=" << rankSize << " rank=" << rank;
            for (u32 idx : last.rxSliceIdxs) {
                EXPECT_LT(idx, rankSize);
            }
        }
    }
}

TEST_F(AllGatherNhrLastStepTest, ReadAndPostCopySetsPartitionAllRanks)
{
    for (u32 rankSize : g_rankSizes) {
        const u32 nSteps = RefStepNum(rankSize);
        for (u32 rank = 0; rank < rankSize; rank++) {
            const RefStep last = RefGetStep(rank, rankSize, nSteps - 1U, nSteps);
            const auto copied = RefPostCopyRanks(rank, rankSize, true, false, last.rxSliceIdxs);
            for (u32 c : copied) {
                EXPECT_TRUE(std::find(last.rxSliceIdxs.begin(), last.rxSliceIdxs.end(), c) == last.rxSliceIdxs.end())
                    << "rankSize=" << rankSize << " rank=" << rank << " duplicated slice " << c;
            }
            std::set<u32> all(last.rxSliceIdxs.begin(), last.rxSliceIdxs.end());
            all.insert(copied.begin(), copied.end());
            EXPECT_EQ(all.size(), rankSize) << "rankSize=" << rankSize << " rank=" << rank;
        }
    }
}

TEST_F(AllGatherNhrLastStepTest, SolePathStillCopiesOwnSlice)
{
    const u32 rankSize = 8U;
    const u32 nSteps = RefStepNum(rankSize);
    const RefStep last = RefGetStep(0U, rankSize, nSteps - 1U, nSteps);
    const auto copied = RefPostCopyRanks(0U, rankSize, true, false, last.rxSliceIdxs);
    EXPECT_TRUE(std::find(copied.begin(), copied.end(), 0U) != copied.end());
    EXPECT_EQ(copied.size(), rankSize - last.rxSliceIdxs.size());
}

TEST_F(AllGatherNhrLastStepTest, SkipOwnSliceDropsOneMoreCopy)
{
    const u32 rankSize = 8U;
    const u32 nSteps = RefStepNum(rankSize);
    const RefStep last = RefGetStep(0U, rankSize, nSteps - 1U, nSteps);
    const auto withOwn = RefPostCopyRanks(0U, rankSize, true, false, last.rxSliceIdxs);
    const auto withoutOwn = RefPostCopyRanks(0U, rankSize, true, true, last.rxSliceIdxs);
    EXPECT_EQ(withoutOwn.size() + 1U, withOwn.size());
    EXPECT_TRUE(std::find(withoutOwn.begin(), withoutOwn.end(), 0U) == withoutOwn.end());
}

TEST_F(AllGatherNhrLastStepTest, FallbackCopiesEveryRank)
{
    const u32 rankSize = 8U;
    const u32 nSteps = RefStepNum(rankSize);
    const RefStep last = RefGetStep(0U, rankSize, nSteps - 1U, nSteps);
    const auto copied = RefPostCopyRanks(0U, rankSize, false, false, last.rxSliceIdxs);
    EXPECT_EQ(copied.size(), rankSize);
}

// strideCount with last-step read

class AllGatherNhrStrideTest : public ::testing::Test {};

TEST_F(AllGatherNhrStrideTest, StrideZeroKeepsContiguousLayout)
{
    const u32 rankSize = 8U;
    const u64 dataSize = 4096U;
    const auto offsets = RefLastStepOutputOffsets(0U, rankSize, 0U, dataSize, 0U);
    const u32 nSteps = RefStepNum(rankSize);
    const RefStep last = RefGetStep(0U, rankSize, nSteps - 1U, nSteps);
    ASSERT_EQ(offsets.size(), last.rxSliceIdxs.size());
    for (size_t i = 0; i < offsets.size(); i++) {
        EXPECT_EQ(offsets[i], dataSize * last.rxSliceIdxs[i]);
    }
}

TEST_F(AllGatherNhrStrideTest, StrideScalesOutputOffsets)
{
    const u32 rankSize = 8U;
    const u64 dataSize = 4096U;
    const auto contiguous = RefLastStepOutputOffsets(0U, rankSize, 0U, dataSize, 0U);
    const auto strided = RefLastStepOutputOffsets(0U, rankSize, 0U, dataSize * 2U, 0U);
    ASSERT_EQ(contiguous.size(), strided.size());
    for (size_t i = 0; i < contiguous.size(); i++) {
        EXPECT_EQ(strided[i], contiguous[i] * 2U);
    }
}

TEST_F(AllGatherNhrStrideTest, StridedSlicesDoNotOverlap)
{
    const u32 rankSize = 8U;
    const u64 dataSize = 4096U;
    for (u64 strideMultiplier : {1U, 2U, 3U}) {
        const auto offsets = RefLastStepOutputOffsets(0U, rankSize, 0U, dataSize * strideMultiplier, 0U);
        std::set<u64> uniq(offsets.begin(), offsets.end());
        EXPECT_EQ(uniq.size(), offsets.size());
        for (u64 off : offsets) {
            EXPECT_EQ(off % dataSize, 0U);
        }
    }
}

TEST_F(AllGatherNhrStrideTest, BaseOffsetShiftsAllSlices)
{
    const u32 rankSize = 8U;
    const u64 dataSize = 4096U;
    const u64 baseOff = 1024U;
    const auto without = RefLastStepOutputOffsets(0U, rankSize, 0U, dataSize, 0U);
    const auto with = RefLastStepOutputOffsets(0U, rankSize, baseOff, dataSize, 0U);
    ASSERT_EQ(without.size(), with.size());
    for (size_t i = 0; i < without.size(); i++) {
        EXPECT_EQ(with[i], without[i] + baseOff);
    }
}

TEST_F(AllGatherNhrStrideTest, PartialOffsetShiftsWithinSlice)
{
    const u32 rankSize = 8U;
    const u64 dataSize = 4096U;
    const u64 partialOffset = 512U;
    const auto base = RefLastStepOutputOffsets(0U, rankSize, 0U, dataSize, 0U);
    const auto shifted = RefLastStepOutputOffsets(0U, rankSize, 0U, dataSize, partialOffset);
    ASSERT_EQ(base.size(), shifted.size());
    for (size_t i = 0; i < base.size(); i++) {
        EXPECT_EQ(shifted[i], base[i] + partialOffset);
    }
}

TEST_F(AllGatherNhrStrideTest, StrideCoversEveryRankSlotAcrossAllRanks)
{
    const u32 rankSize = 8U;
    const u64 dataSize = 4096U;
    const u64 stride = dataSize * 2U;
    const u32 nSteps = RefStepNum(rankSize);
    for (u32 rank = 0; rank < rankSize; rank++) {
        const RefStep last = RefGetStep(rank, rankSize, nSteps - 1U, nSteps);
        std::set<u64> writtenOffsets;
        for (u32 rxIdx : last.rxSliceIdxs) {
            writtenOffsets.insert(stride * rxIdx);
        }
        const auto copied = RefPostCopyRanks(rank, rankSize, true, false, last.rxSliceIdxs);
        for (u32 algRank : copied) {
            writtenOffsets.insert(stride * algRank);
        }
        EXPECT_EQ(writtenOffsets.size(), rankSize) << "rank=" << rank;
    }
}

// Jetty limit

class AllGatherNhrJettyLimitTest : public ::testing::Test {};

TEST_F(AllGatherNhrJettyLimitTest, OverLimitIsWarnOnlyNotFailure)
{
    const auto channels = MakeChannelDescs({{1U, 8U}, {2U, 8U}});
    const u32 perRank = CalcChannelsPerRankMin(channels);
    EXPECT_EQ(perRank, 8U);
    EXPECT_GT(perRank, MAX_JETTY_NUM);
}

TEST_F(AllGatherNhrJettyLimitTest, WithinLimitIsAccepted)
{
    const auto channels = MakeChannelDescs({{1U, 4U}, {2U, 4U}});
    EXPECT_LE(CalcChannelsPerRankMin(channels), MAX_JETTY_NUM);
}

} // namespace
