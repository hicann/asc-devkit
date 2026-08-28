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
#include <cstdio>
#include <limits>
#include <string>
#include <vector>

#include "alg_env_config.h"
#include "alltoall_auto_selector.h"
#include "alltoallv_auto_selector.h"
#include "cann_host_bridge.h"
#include "coll_alg_v2_exec_registry.h"
#include "executor_v2_base.h"
#include "gtest/gtest.h"
#include "ins_temp_all_to_all_v_mesh_1D.h"

using namespace mc2_ops_hccl;

namespace checker {
namespace {
constexpr u64 ALLTOALL_MULTI_CHANNEL_LIMIT = 150ULL * 1024ULL * 1024ULL;

OpParam MakeAicpuParam(HcclCMDType opType, CommEngine engine = COMM_ENGINE_AICPU_TS)
{
    OpParam param{};
    param.opType = opType;
    param.engine = engine;
    param.opExecuteConfig = OpExecuteConfig::AICPU_TS;
    param.userRank = 0U;
    return param;
}

TopoInfoWithNetLayerDetails MakeConcurrentTopo(u32 rankSize = 4U)
{
    TopoInfoWithNetLayerDetails topo{};
    topo.userRank = 0U;
    topo.userRankSize = rankSize;
    topo.serverNum = 1U;
    topo.level0Topo = Level0Shape::MESH_1D_CLOS;
    topo.topoLevelNums = 1U;
    topo.level0PcieMix = false;
    topo.topoInstDetailsOfLayer.resize(1U);
    topo.topoInstDetailsOfLayer[0].rankNumForTopoType[COMM_TOPO_1DMESH] = {rankSize};
    topo.topoInstDetailsOfLayer[0].rankNumForTopoType[COMM_TOPO_CLOS] = {rankSize};
    return topo;
}

std::vector<HcclChannelDesc> MakeMultiJettyChannels(u32 rankSize, u32 channelsPerRank)
{
    std::vector<HcclChannelDesc> channels;
    for (u32 rank = 1U; rank < rankSize; ++rank) {
        for (u32 channelIdx = 0U; channelIdx < channelsPerRank; ++channelIdx) {
            HcclChannelDesc channel{};
            channel.remoteRank = rank;
            channels.push_back(channel);
        }
    }
    return channels;
}

} // namespace

class ST_ALL_TO_ALL_MESH1D_AICPU_TEST : public ::testing::Test {
protected:
    void SetUp() override { ResetAlgEnvConfigInitState(); }
};

TEST_F(ST_ALL_TO_ALL_MESH1D_AICPU_TEST, aicpu_collectives_use_expected_cann_bridge_routes)
{
    for (CommEngine engine : {COMM_ENGINE_AICPU_TS, COMM_ENGINE_AICPU}) {
        EXPECT_TRUE(UseCannBridge(MakeAicpuParam(HCCL_CMD_ALLTOALL, engine)));
        EXPECT_TRUE(UseCannBridge(MakeAicpuParam(HCCL_CMD_ALLTOALLV, engine)));
        EXPECT_TRUE(UseCannBridge(MakeAicpuParam(HCCL_CMD_ALLREDUCE, engine)));
        EXPECT_FALSE(UseCannBridge(MakeAicpuParam(HCCL_CMD_ALLGATHER, engine)));
        EXPECT_FALSE(UseCannBridge(MakeAicpuParam(HCCL_CMD_REDUCE_SCATTER, engine)));
    }

    EXPECT_FALSE(UseCannBridge(MakeAicpuParam(HCCL_CMD_ALLTOALL, COMM_ENGINE_CPU)));
    EXPECT_FALSE(UseCannBridge(MakeAicpuParam(HCCL_CMD_ALLTOALLV, COMM_ENGINE_CPU)));
    EXPECT_FALSE(UseCannBridge(MakeAicpuParam(HCCL_CMD_ALLREDUCE, COMM_ENGINE_CPU)));
}

TEST_F(ST_ALL_TO_ALL_MESH1D_AICPU_TEST, local_registry_contains_mesh1d_and_concurrent_algorithms)
{
    CollAlgExecRegistryV2& registry = CollAlgExecRegistryV2::Instance();
    EXPECT_NE(registry.GetAlgExec(HCCL_CMD_ALLTOALLV, "AicpuAllToAllVSoleMesh"), nullptr);
    EXPECT_NE(registry.GetAlgExec(HCCL_CMD_ALLTOALL, "AicpuAllToAllSoleMeshSingleChannel"), nullptr);
    EXPECT_NE(registry.GetAlgExec(HCCL_CMD_ALLTOALL, "AicpuAllToAllSoleMesh"), nullptr);
    EXPECT_NE(registry.GetAlgExec(HCCL_CMD_ALLTOALL, "AicpuAllToAllSoleMeshConcurrent"), nullptr);
    EXPECT_NE(registry.GetAlgExec(HCCL_CMD_ALLTOALLV, "AicpuAllToAllVSoleMeshConcurrent"), nullptr);
}

TEST_F(ST_ALL_TO_ALL_MESH1D_AICPU_TEST, local_concurrent_selectors_obey_topology_and_count_boundaries)
{
    AlltoAllAutoSelector alltoallSelector;
    AlltoAllVAutoSelector alltoallvSelector;
    TopoInfoWithNetLayerDetails topo = MakeConcurrentTopo();
    u64 sendCounts[4] = {512U, 512U, 512U, 512U};
    OpParam param = MakeAicpuParam(HCCL_CMD_ALLTOALL);
    param.all2AllVDataDes.sendCounts = sendCounts;
    std::string algName;

    EXPECT_EQ(alltoallSelector.Select(param, &topo, algName), SelectorStatus::MATCH);
    EXPECT_EQ(algName, "AicpuAllToAllSoleMeshUBX");
    sendCounts[0] = 513U;
    param = MakeAicpuParam(HCCL_CMD_ALLTOALL);
    param.all2AllVDataDes.sendCounts = sendCounts;
    EXPECT_EQ(alltoallSelector.Select(param, &topo, algName), SelectorStatus::MATCH);
    EXPECT_EQ(algName, "AicpuAllToAllSoleMeshConcurrent");

    param = MakeAicpuParam(HCCL_CMD_ALLTOALLV);
    EXPECT_EQ(alltoallvSelector.Select(param, &topo, algName), SelectorStatus::MATCH);
    EXPECT_EQ(algName, "AicpuAllToAllVSoleMeshConcurrent");

    topo.userRankSize = 5U;
    EXPECT_EQ(alltoallvSelector.Select(param, &topo, algName), SelectorStatus::NOT_MATCH);
    topo = MakeConcurrentTopo();
    topo.topoInstDetailsOfLayer[0].rankNumForTopoType[COMM_TOPO_CLOS] = {3U};
    EXPECT_EQ(alltoallvSelector.Select(param, &topo, algName), SelectorStatus::NOT_MATCH);
    topo = MakeConcurrentTopo();
    topo.level0PcieMix = true;
    EXPECT_EQ(alltoallvSelector.Select(param, &topo, algName), SelectorStatus::MATCH);
    EXPECT_EQ(algName, "AicpuAllToAllVSoleMesh");
}

TEST_F(ST_ALL_TO_ALL_MESH1D_AICPU_TEST, alltoall_selector_preserves_multi_level_mesh_clos_path)
{
    AlltoAllAutoSelector selector;
    TopoInfoWithNetLayerDetails topo = MakeConcurrentTopo();
    topo.topoLevelNums = 2U;
    u64 sendCounts[4] = {513U, 513U, 513U, 513U};
    OpParam param = MakeAicpuParam(HCCL_CMD_ALLTOALL);
    param.all2AllVDataDes.sendCounts = sendCounts;
    std::string algName;

    EXPECT_EQ(selector.Select(param, &topo, algName), SelectorStatus::MATCH);
#if defined(HCCL_CANN_COMPAT_850)
    EXPECT_EQ(algName, "AicpuAllToAllSoleMeshUBX");
#else
    EXPECT_EQ(algName, "AicpuAllToAllSoleMeshConcurrent");
#endif
}

TEST_F(ST_ALL_TO_ALL_MESH1D_AICPU_TEST, concurrent_template_uses_all_channels_per_rank)
{
    constexpr u32 rankSize = 4U;
    constexpr u32 channelsPerRank = 4U;
    const std::vector<std::vector<u32>> subCommRanks = {{0U, 1U, 2U, 3U}};
    const std::vector<HcclChannelDesc> channels = MakeMultiJettyChannels(rankSize, channelsPerRank);

    for (const char* algName : {"AicpuAllToAllSoleMeshConcurrent", "AicpuAllToAllVSoleMeshConcurrent"}) {
        OpParam param = MakeAicpuParam(HCCL_CMD_ALLTOALL);
        EXPECT_GT(std::snprintf(param.algName, sizeof(param.algName), "%s", algName), 0);
        InsTempAlltoAllVMesh1D temp(param, 0U, subCommRanks);
        AlgResourceRequest request;
        EXPECT_EQ(temp.CalcResByChannelDescs(param, channels, request), HCCL_SUCCESS);
        EXPECT_EQ(request.slaveThreadNum, (rankSize - 1U) * channelsPerRank);
        EXPECT_EQ(request.notifyNumOnMainThread, request.slaveThreadNum);
        ASSERT_EQ(request.notifyNumPerThread.size(), request.slaveThreadNum);
        EXPECT_TRUE(std::all_of(
            request.notifyNumPerThread.begin(), request.notifyNumPerThread.end(),
            [](u32 notifyNum) { return notifyNum == channelsPerRank; }));
    }
}

TEST_F(ST_ALL_TO_ALL_MESH1D_AICPU_TEST, concurrent_executor_calculates_resources_with_host_alltoall_param)
{
    constexpr u32 rankSize = 4U;
    u64 sendCounts[rankSize] = {513U, 513U, 513U, 513U};
    OpParam param = MakeAicpuParam(HCCL_CMD_ALLTOALL);
    param.all2AllVDataDes.sendType = HCCL_DATA_TYPE_FP32;
    param.all2AllVDataDes.recvType = HCCL_DATA_TYPE_FP32;
    param.all2AllVDataDes.sendCounts = sendCounts;
    EXPECT_GT(std::snprintf(param.algName, sizeof(param.algName), "%s", "AicpuAllToAllSoleMeshConcurrent"), 0);

    TopoInfoWithNetLayerDetails topo = MakeConcurrentTopo(rankSize);
    AlgHierarchyInfoForAllLevel hierarchy;
    hierarchy.infos = {{{0U, 1U, 2U, 3U}, {0U, 1U, 2U, 3U}}};
    std::unique_ptr<InsCollAlgBase> executor =
        CollAlgExecRegistryV2::Instance().GetAlgExec(HCCL_CMD_ALLTOALL, param.algName);
    ASSERT_NE(executor, nullptr);

    AlgResourceRequest request;
    EXPECT_EQ(executor->CalcRes(nullptr, param, &topo, hierarchy, request), HCCL_SUCCESS);
    EXPECT_EQ(request.slaveThreadNum, 16U);
    EXPECT_EQ(request.notifyNumOnMainThread, 4U);
    EXPECT_EQ(request.notifyNumPerThread.size(), 16U);
    ASSERT_EQ(request.channels.size(), 1U);
    EXPECT_EQ(request.channels[0].size(), 15U);
}

TEST_F(ST_ALL_TO_ALL_MESH1D_AICPU_TEST, alltoall_multi_channel_threshold_is_strict_and_overflow_safe)
{
    constexpr u64 rankSize = 3U;
    constexpr u64 dataTypeSize = 1U;
    constexpr u64 countAtLimit = ALLTOALL_MULTI_CHANNEL_LIMIT / rankSize;
    EXPECT_FALSE(ShouldUseMultiChannelForAlltoAll(countAtLimit - 1U, dataTypeSize, rankSize));
    EXPECT_FALSE(ShouldUseMultiChannelForAlltoAll(countAtLimit, dataTypeSize, rankSize));
    EXPECT_TRUE(ShouldUseMultiChannelForAlltoAll(countAtLimit + 1U, dataTypeSize, rankSize));
    EXPECT_TRUE(ShouldUseMultiChannelForAlltoAll(std::numeric_limits<u64>::max(), 16U, 18U));
}

TEST_F(ST_ALL_TO_ALL_MESH1D_AICPU_TEST, peer_group_rounds_cover_18_ranks)
{
    const std::vector<u32> firstRound = CalcAlltoallMesh1DCommRanks(0U, 18U, 16U, 0U, 17U);
    const std::vector<u32> secondRound = CalcAlltoallMesh1DCommRanks(0U, 18U, 16U, 1U, 1U);
    EXPECT_EQ(firstRound.size(), 16U);
    EXPECT_EQ(std::find(firstRound.begin(), firstRound.end(), 0U), firstRound.end());
    std::vector<u32> sortedFirstRound = firstRound;
    std::sort(sortedFirstRound.begin(), sortedFirstRound.end());
    EXPECT_EQ(std::unique(sortedFirstRound.begin(), sortedFirstRound.end()), sortedFirstRound.end());
    EXPECT_EQ(
        sortedFirstRound, std::vector<u32>({1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 10U, 11U, 12U, 13U, 14U, 15U, 16U, 17U}));
    EXPECT_EQ(secondRound, std::vector<u32>({9U}));
}
} // namespace checker
