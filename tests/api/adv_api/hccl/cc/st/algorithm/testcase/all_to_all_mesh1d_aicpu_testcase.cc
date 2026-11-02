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
#include <vector>

#include "alg_env_config.h"
#include "alltoall_auto_selector.h"
#include "alltoallv_auto_selector.h"
#include "cann_host_bridge.h"
#include "coll_alg_v2_exec_registry.h"
#include "execute_selector.h"
#include "executor_v2_base.h"
#include "gtest/gtest.h"
#include "ins_temp_all_to_all_v_mesh_1D.h"
#include "ins_temp_all_to_all_v_pairwise.h"

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

TEST_F(ST_ALL_TO_ALL_MESH1D_AICPU_TEST, aicpu_collectives_use_expected_bridge_routes)
{
    for (CommEngine engine : {COMM_ENGINE_AICPU_TS, COMM_ENGINE_AICPU}) {
        EXPECT_FALSE(UseCannBridge(MakeAicpuParam(HCCL_CMD_ALLTOALL, engine)));
        EXPECT_FALSE(UseCannBridge(MakeAicpuParam(HCCL_CMD_ALLTOALLV, engine)));
        EXPECT_FALSE(UseCannBridge(MakeAicpuParam(HCCL_CMD_ALLREDUCE, engine)));
        EXPECT_FALSE(UseCannBridge(MakeAicpuParam(HCCL_CMD_ALLGATHER, engine)));
        EXPECT_FALSE(UseCannBridge(MakeAicpuParam(HCCL_CMD_REDUCE_SCATTER, engine)));
    }

    EXPECT_FALSE(UseCannBridge(MakeAicpuParam(HCCL_CMD_ALLTOALL, COMM_ENGINE_CPU)));
    EXPECT_FALSE(UseCannBridge(MakeAicpuParam(HCCL_CMD_ALLTOALLV, COMM_ENGINE_CPU)));
    EXPECT_FALSE(UseCannBridge(MakeAicpuParam(HCCL_CMD_ALLREDUCE, COMM_ENGINE_CPU)));
}

TEST_F(ST_ALL_TO_ALL_MESH1D_AICPU_TEST, execute_selector_uses_local_devkit_for_alltoall)
{
    ExecuteSelector selector;
    TopoInfoWithNetLayerDetails topo = MakeConcurrentTopo();
    u64 sendCounts[4] = {1U, 1U, 1U, 1U};
    OpParam param = MakeAicpuParam(HCCL_CMD_ALLTOALL);
    param.all2AllVDataDes.sendCounts = sendCounts;
    std::string algName;

    EXPECT_EQ(selector.Run(param, &topo, algName), HCCL_SUCCESS);
    EXPECT_EQ(algName, "AicpuAllToAllSoleMeshUBX");
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

// pairwise 命中拓扑（对齐 hccl IsPairwiseCapable + REGISTER_ALG_ATTRS 约束）：
// 跨框多级 MESH_1D、每框 8 卡、rankSize 为 16 的倍数、level0Symmetric、<=128P
TopoInfoWithNetLayerDetails MakePairwiseTopo(u32 rankSize = 32U)
{
    TopoInfoWithNetLayerDetails topo{};
    topo.userRank = 0U;
    topo.userRankSize = rankSize;
    topo.serverNum = 1U;
    topo.level0Topo = Level0Shape::MESH_1D;
    topo.topoLevelNums = 2U;
    topo.level0PcieMix = false;
    topo.level0Symmetric = true;
    topo.deviceNumPerModule = 8U;
    return topo;
}

TEST_F(ST_ALL_TO_ALL_MESH1D_AICPU_TEST, alltoallv_pairwise_selector_hits_and_guards)
{
    AlltoAllVAutoSelector selector;
    OpParam param = MakeAicpuParam(HcclCMDType::HCCL_CMD_ALLTOALLV);
    std::string algName;

    // 命中：pairwise-capable 拓扑优先于 SoleMesh（对齐 hccl selector 顺序）
    TopoInfoWithNetLayerDetails topo = MakePairwiseTopo(32U);
    EXPECT_EQ(selector.Select(param, &topo, algName), SelectorStatus::MATCH);
    EXPECT_EQ(algName, "AicpuAllToAllVSolePairwise");

    // 注册闭环：选中的 algName 有本地可执行注册
    EXPECT_NE(
        CollAlgExecRegistryV2::Instance().GetAlgExec(HcclCMDType::HCCL_CMD_ALLTOALLV, "AicpuAllToAllVSolePairwise"),
        nullptr);

    // 128P 上限（hccl REGISTER_ALG_ATTRS 限制）：超出回落 SoleMesh
    topo = MakePairwiseTopo(256U);
    EXPECT_EQ(selector.Select(param, &topo, algName), SelectorStatus::MATCH);
    EXPECT_EQ(algName, "AicpuAllToAllVSoleMesh");

    // level0 非对称：回落 SoleMesh
    topo = MakePairwiseTopo(32U);
    topo.level0Symmetric = false;
    EXPECT_EQ(selector.Select(param, &topo, algName), SelectorStatus::MATCH);
    EXPECT_EQ(algName, "AicpuAllToAllVSoleMesh");

    // 单级拓扑不走 pairwise
    topo = MakePairwiseTopo(32U);
    topo.topoLevelNums = 1U;
    EXPECT_EQ(selector.Select(param, &topo, algName), SelectorStatus::MATCH);
    EXPECT_EQ(algName, "AicpuAllToAllVSoleMesh");

    // 每框非 8 卡：逻辑板与物理框错位，回落 SoleMesh
    topo = MakePairwiseTopo(32U);
    topo.deviceNumPerModule = 4U;
    EXPECT_EQ(selector.Select(param, &topo, algName), SelectorStatus::MATCH);
    EXPECT_EQ(algName, "AicpuAllToAllVSoleMesh");
}

TEST_F(ST_ALL_TO_ALL_MESH1D_AICPU_TEST, alltoallv_pairwise_supported_topology_boundaries)
{
    AlltoAllVAutoSelector selector;
    OpParam param = MakeAicpuParam(HcclCMDType::HCCL_CMD_ALLTOALLV);
    EXPECT_FALSE(InsTempAllToAllVPairwise::IsSupportedTopology(nullptr));
    for (u32 rankSize : {16U, 32U, 48U, 64U, 128U}) {
        TopoInfoWithNetLayerDetails topo = MakePairwiseTopo(rankSize);
        EXPECT_TRUE(InsTempAllToAllVPairwise::IsSupportedTopology(&topo));
        std::string algName;
        EXPECT_EQ(selector.Select(param, &topo, algName), SelectorStatus::MATCH);
        EXPECT_EQ(algName, "AicpuAllToAllVSolePairwise");
    }
}

TEST_F(ST_ALL_TO_ALL_MESH1D_AICPU_TEST, alltoallv_pairwise_resource_precheck_rejects_unsupported_topology)
{
    OpParam param = MakeAicpuParam(HcclCMDType::HCCL_CMD_ALLTOALLV);
    std::vector<TopoInfoWithNetLayerDetails> unsupportedTopos;
    for (u32 rankSize : {0U, 8U, 24U, 144U, 256U}) {
        unsupportedTopos.push_back(MakePairwiseTopo(rankSize));
    }
    TopoInfoWithNetLayerDetails topo = MakePairwiseTopo();
    topo.deviceNumPerModule = 4U;
    unsupportedTopos.push_back(topo);
    topo = MakePairwiseTopo();
    topo.level0Symmetric = false;
    unsupportedTopos.push_back(topo);
    topo = MakePairwiseTopo();
    topo.topoLevelNums = 1U;
    unsupportedTopos.push_back(topo);
    topo = MakePairwiseTopo();
    topo.level0Topo = Level0Shape::CLOS;
    unsupportedTopos.push_back(topo);

    // Exercise CalcRes directly, as forced-alg preflight does, without the auto selector guard.
    for (const auto& unsupported : unsupportedTopos) {
        EXPECT_FALSE(InsTempAllToAllVPairwise::IsSupportedTopology(&unsupported));
        std::vector<u32> ranks;
        for (u32 rank = 0; rank < unsupported.userRankSize; ++rank) {
            ranks.push_back(rank);
        }
        InsTempAllToAllVPairwise algTemplate(param, 0U, {ranks});
        AlgResourceRequest request;
        EXPECT_EQ(algTemplate.CalcRes(nullptr, param, &unsupported, request), HCCL_E_NOT_SUPPORT);
        EXPECT_TRUE(request.channels.empty());
        EXPECT_TRUE(request.notifyNumPerThread.empty());
    }
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

TEST_F(ST_ALL_TO_ALL_MESH1D_AICPU_TEST, aicpu_mesh1d_clos_nullptr_sendcounts_selects_smallest_path)
{
    AlltoAllAutoSelector selector;
    TopoInfoWithNetLayerDetails topo = MakeConcurrentTopo();
    OpParam param = MakeAicpuParam(HCCL_CMD_ALLTOALL);
    param.all2AllVDataDes.sendCounts = nullptr; // tiling 期常态
    std::string algName;
    EXPECT_EQ(selector.Select(param, &topo, algName), SelectorStatus::MATCH);
    EXPECT_EQ(algName, "AicpuAllToAllSoleMeshUBX"); // 0 > 512 为假 → 最小路径
}

TEST_F(ST_ALL_TO_ALL_MESH1D_AICPU_TEST, aicpu_multilevel_mesh1d_clos_keeps_concurrent_ubx_judgment)
{
    AlltoAllAutoSelector selector;
    TopoInfoWithNetLayerDetails topo = MakeConcurrentTopo();
    topo.topoLevelNums = 2U; // 多级
    u64 sendCounts[4] = {600U, 600U, 600U, 600U};
    OpParam param = MakeAicpuParam(HCCL_CMD_ALLTOALL);
    param.all2AllVDataDes.sendCounts = sendCounts;
    std::string algName;
    // 多级 MESH_1D_CLOS 保持既有行为：落入单级 Concurrent/UBX 判断。
    // Concurrent/UBX 不受"SoleMesh 双实例契约阻塞"影响（与 alltoallv 多级拒绝不矛盾）：
    //   - AicpuAllToAllSoleMeshConcurrent → InsV2AllToAllConcurrentExecutor + TopoMatchUBX，
    //     自有 CalcRes 恰好要求双实例（meshRanks/closRanks 双模板），TopoMatchUBX 正供给该结构；
    //   - AicpuAllToAllSoleMeshUBX → InsV2AlltoAllVSoleExecutor + TopoMatchUBX1d，
    //     该 matcher 产双实例（满足 BuildResourceHierarchyInfo 的 infos[0].size()==2 校验）
    //     且多级时产 infos[1]（layer3 组）；
    //   - 被阻塞的仅是 TopoMatch1D（扁平结构）+ MESH_1D_CLOS 双实例要求的错配组合，
    //     即 alltoallv 多级唯一可选的 AicpuAllToAllVSoleMesh（见下方 not_supported 用例）。
    EXPECT_EQ(selector.Select(param, &topo, algName), SelectorStatus::MATCH);
    EXPECT_EQ(algName, "AicpuAllToAllSoleMeshConcurrent"); // 600>512 且 mesh==clos 且 4P

    param.all2AllVDataDes.sendCounts = nullptr; // 多级 + nullptr → 最小路径 UBX
    EXPECT_EQ(selector.Select(param, &topo, algName), SelectorStatus::MATCH);
    EXPECT_EQ(algName, "AicpuAllToAllSoleMeshUBX");
}

TEST_F(ST_ALL_TO_ALL_MESH1D_AICPU_TEST, aicpu_alltoallv_multilevel_mesh1d_clos_is_not_supported)
{
    AlltoAllVAutoSelector selector;
    OpParam param = MakeAicpuParam(HCCL_CMD_ALLTOALLV);
    std::string algName;

    // 对应实现：alltoallv_auto_selector.cc SelectAicpuAlgo 的 MESH_1D_CLOS 非 pcieMix 路径
    // （多级 MATCH 分支已删除，多级由末尾 topoLevelNums != 1U 判断拒绝）。
    // 拒绝依据：alltoallv 在该形态下唯一可选的 AicpuAllToAllVSoleMesh 用 TopoMatch1D（扁平
    // hierarchy），与 executor 对 MESH_1D_CLOS 的双实例契约（BuildResourceHierarchyInfo 要求
    // infos[0].size()==2）错配，资源阶段必报错。注意该阻塞仅针对 TopoMatch1D 扁平 matcher：
    // alltoall 侧多级可选的 Concurrent/UBX 用 TopoMatchUBX/TopoMatchUBX1d（产双实例），
    // 契约匹配不受影响（见上方 keeps_concurrent_ubx_judgment 用例注释）
    TopoInfoWithNetLayerDetails topo = MakeConcurrentTopo();
    topo.topoLevelNums = 2U;
    EXPECT_EQ(selector.Select(param, &topo, algName), SelectorStatus::NOT_MATCH);

    // 3 级及以上同样拒绝
    topo.topoLevelNums = 3U;
    EXPECT_EQ(selector.Select(param, &topo, algName), SelectorStatus::NOT_MATCH);

    // pcieMix 不受级数限制（该路径按 mesh1d 算法处理，且绕开 executor 双实例契约）
    TopoInfoWithNetLayerDetails pcieMixTopo = MakeConcurrentTopo();
    pcieMixTopo.topoLevelNums = 2U;
    pcieMixTopo.level0PcieMix = true;
    EXPECT_EQ(selector.Select(param, &pcieMixTopo, algName), SelectorStatus::MATCH);
    EXPECT_EQ(algName, "AicpuAllToAllVSoleMesh");
}

} // namespace checker
