/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <cstdio>
#include <memory>
#include <string>
#include <vector>

#include "alg_env_config.h"
#include "all_gather_auto_selector.h"
#include "cann_host_bridge.h"
#include "coll_alg_v2_exec_registry.h"
#include "executor_v2_base.h"
#include "gtest/gtest.h"

using namespace mc2_ops_hccl;

namespace checker {
namespace {
constexpr char MULTI_JETTY_ALGORITHM_NAME[] = "InsAllGatherParallelMesh1DNHRMultiJetty";
constexpr u32 MULTI_JETTY_RANK_SIZE = 16U;
constexpr u32 MULTI_JETTY_MESH_SIZE = 8U;
constexpr u32 MULTI_JETTY_CLOS_SIZE = 16U;
constexpr u32 MULTI_JETTY_CHANNEL_NUM = 4U;
constexpr u32 MULTI_JETTY_EXPECTED_SLAVE_THREAD_NUM = 15U;

OpParam MakeAicpuParam(CommEngine engine = COMM_ENGINE_AICPU_TS)
{
    OpParam param{};
    param.opType = HCCL_CMD_ALLGATHER;
    param.engine = engine;
    param.opExecuteConfig = OpExecuteConfig::AICPU_TS;
    param.userRank = 0U;
    param.DataDes.dataType = HCCL_DATA_TYPE_FP32;
    param.DataDes.count = 1U;
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
    topo.netLayerDetails.localNetInsSizeOfLayer = {rankSize};
    topo.topoInstDetailsOfLayer.resize(1U);
    topo.topoInstDetailsOfLayer[0].rankNumForTopoType[COMM_TOPO_1DMESH] = {rankSize};
    topo.topoInstDetailsOfLayer[0].rankNumForTopoType[COMM_TOPO_CLOS] = {rankSize};
    return topo;
}

TopoInfoWithNetLayerDetails MakeMultiJettyTopo(u32 userRank = 0U)
{
    TopoInfoWithNetLayerDetails topo{};
    topo.userRank = userRank;
    topo.userRankSize = MULTI_JETTY_RANK_SIZE;
    topo.serverNum = 1U;
    topo.level0Topo = Level0Shape::MESH_1D_CLOS;
    topo.topoLevelNums = 1U;
    topo.level0PcieMix = false;
    topo.netLayerDetails.localNetInsSizeOfLayer = {MULTI_JETTY_MESH_SIZE};
    topo.topoInstDetailsOfLayer.resize(1U);
    topo.topoInstDetailsOfLayer[0].rankNumForTopoType[COMM_TOPO_1DMESH] = {MULTI_JETTY_MESH_SIZE};
    topo.topoInstDetailsOfLayer[0].rankNumForTopoType[COMM_TOPO_CLOS] = {MULTI_JETTY_CLOS_SIZE};
    return topo;
}

AlgHierarchyInfoForAllLevel MakeMultiJettyHierarchy(u32 userRank)
{
    const u32 meshBase = userRank / MULTI_JETTY_MESH_SIZE * MULTI_JETTY_MESH_SIZE;
    std::vector<u32> meshRanks;
    std::vector<u32> closRanks;
    for (u32 idx = 0U; idx < MULTI_JETTY_MESH_SIZE; ++idx) {
        meshRanks.push_back(meshBase + idx);
    }
    for (u32 rank = 0U; rank < MULTI_JETTY_RANK_SIZE; ++rank) {
        closRanks.push_back(rank);
    }
    AlgHierarchyInfoForAllLevel hierarchy;
    hierarchy.infos = {{meshRanks, closRanks}, {}};
    return hierarchy;
}

} // namespace

class ST_ALL_GATHER_AICPU_TEST : public ::testing::Test {
protected:
    void SetUp() override { ResetAlgEnvConfigInitState(); }
};

TEST_F(ST_ALL_GATHER_AICPU_TEST, aicpu_all_gather_does_not_use_cann_bridge)
{
    for (CommEngine engine : {COMM_ENGINE_AICPU_TS, COMM_ENGINE_AICPU}) {
        EXPECT_FALSE(UseCannBridge(MakeAicpuParam(engine)));
    }
    EXPECT_FALSE(UseCannBridge(MakeAicpuParam(COMM_ENGINE_CPU)));
}

TEST_F(ST_ALL_GATHER_AICPU_TEST, local_registry_contains_all_gather_algorithms)
{
    CollAlgExecRegistryV2& registry = CollAlgExecRegistryV2::Instance();
    EXPECT_NE(registry.GetAlgExec(HCCL_CMD_ALLGATHER, "InsAllGatherMesh1D"), nullptr);
    EXPECT_NE(registry.GetAlgExec(HCCL_CMD_ALLGATHER, "AicpuAllGatherSoleNHR"), nullptr);
    EXPECT_NE(registry.GetAlgExec(HCCL_CMD_ALLGATHER, "AicpuAllGatherConcurMeshNHR"), nullptr);
    EXPECT_NE(registry.GetAlgExec(HCCL_CMD_ALLGATHER, "InsAllGatherParallelMesh1DNHRMultiJetty"), nullptr);
}

TEST_F(ST_ALL_GATHER_AICPU_TEST, aicpu_selector_obeys_count_and_rank_boundaries)
{
    constexpr u64 countAtLimit = SMALL_COUNT_512KB / sizeof(float);
    AllGatherAutoSelector selector;
    TopoInfoWithNetLayerDetails topo = MakeConcurrentTopo();
    OpParam param = MakeAicpuParam();
    std::string algName;

    param.DataDes.count = countAtLimit;
    EXPECT_EQ(selector.Select(param, &topo, algName), SelectorStatus::MATCH);
    EXPECT_EQ(algName, "InsAllGatherMesh1D");

    param = MakeAicpuParam();
    param.DataDes.count = countAtLimit + 1U;
    EXPECT_EQ(selector.Select(param, &topo, algName), SelectorStatus::MATCH);
    EXPECT_EQ(algName, "AicpuAllGatherConcurMeshNHR");

    topo = MakeConcurrentTopo(5U);
    EXPECT_EQ(selector.Select(param, &topo, algName), SelectorStatus::MATCH);
    EXPECT_EQ(algName, "AicpuAllGatherSoleNHR");
}

TEST_F(ST_ALL_GATHER_AICPU_TEST, concurrent_executor_calculates_resources_with_host_all_gather_param)
{
    constexpr u32 rankSize = 4U;
    OpParam param = MakeAicpuParam();
    param.DataDes.count = 513U;
    param.algName[0] = '\0';
    ASSERT_GT(std::snprintf(param.algName, sizeof(param.algName), "%s", "AicpuAllGatherConcurMeshNHR"), 0);

    TopoInfoWithNetLayerDetails topo = MakeConcurrentTopo(rankSize);
    AlgHierarchyInfoForAllLevel hierarchy;
    hierarchy.infos = {{{0U, 1U, 2U, 3U}, {0U, 1U, 2U, 3U}}};
    std::unique_ptr<InsCollAlgBase> executor =
        CollAlgExecRegistryV2::Instance().GetAlgExec(HCCL_CMD_ALLGATHER, param.algName);
    ASSERT_NE(executor, nullptr);

    AlgResourceRequest request;
    ASSERT_EQ(executor->CalcRes(nullptr, param, &topo, hierarchy, request), HCCL_SUCCESS);
    EXPECT_EQ(request.slaveThreadNum, 4U);
    EXPECT_EQ(request.notifyNumOnMainThread, 3U);
    EXPECT_EQ(request.notifyNumPerThread, std::vector<u32>({1U, 1U, 2U, 2U}));
    ASSERT_EQ(request.channels.size(), 1U);
    EXPECT_EQ(request.channels[0].size(), 6U);
}

TEST_F(ST_ALL_GATHER_AICPU_TEST, multijetty_selector_uses_algorithm_for_uneven_mesh_clos_topology)
{
    AllGatherAutoSelector selector;
    TopoInfoWithNetLayerDetails topo = MakeMultiJettyTopo();
    OpParam param = MakeAicpuParam();
    param.DataDes.count = SMALL_COUNT_512KB / sizeof(float) + 1U;
    std::string algName;

    EXPECT_EQ(selector.Select(param, &topo, algName), SelectorStatus::MATCH);
    EXPECT_EQ(algName, "InsAllGatherParallelMesh1DNHRMultiJetty");
}

TEST_F(ST_ALL_GATHER_AICPU_TEST, multijetty_executor_calculates_multichannel_resources)
{
    OpParam param = MakeAicpuParam();
    ASSERT_GT(std::snprintf(param.algName, sizeof(param.algName), "%s", MULTI_JETTY_ALGORITHM_NAME), 0);

    const TopoInfoWithNetLayerDetails topo = MakeMultiJettyTopo();
    const AlgHierarchyInfoForAllLevel hierarchy = MakeMultiJettyHierarchy(0U);
    std::unique_ptr<InsCollAlgBase> executor =
        CollAlgExecRegistryV2::Instance().GetAlgExec(HCCL_CMD_ALLGATHER, param.algName);
    ASSERT_NE(executor, nullptr);

    AlgResourceRequest request;
    ASSERT_EQ(executor->CalcRes(nullptr, param, &topo, hierarchy, request), HCCL_SUCCESS);
    EXPECT_EQ(request.slaveThreadNum, MULTI_JETTY_EXPECTED_SLAVE_THREAD_NUM);
    EXPECT_EQ(request.notifyNumOnMainThread, 2U);
    EXPECT_EQ(
        request.notifyNumPerThread, std::vector<u32>({7U, 1U, 1U, 1U, 1U, 1U, 1U, 8U, 2U, 2U, 2U, 2U, 2U, 2U, 2U}));
    ASSERT_EQ(request.channels.size(), 2U);
    EXPECT_EQ(request.channels[0].size(), MULTI_JETTY_MESH_SIZE - 1U);
    EXPECT_EQ(request.channels[1].size(), MULTI_JETTY_CHANNEL_NUM);
}

} // namespace checker
