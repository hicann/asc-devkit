/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <string>
#include <vector>

#include "alg_env_config.h"
#include "execute_selector.h"
#include "gtest/gtest.h"
#include "half_alltoallv_auto_selector.h"

using namespace mc2_ops_hccl;

namespace {
constexpr char HALF_A2AV_ALG_NAME[] = "CcuHalfAll2AllVMesh1D";

TopoInfoWithNetLayerDetails MakeMesh1DTopo(u32 rankSize = 2U)
{
    TopoInfoWithNetLayerDetails topo{};
    topo.userRank = 0U;
    topo.userRankSize = rankSize;
    topo.serverNum = 1U;
    topo.level0Topo = Level0Shape::MESH_1D;
    topo.topoLevelNums = 1U;
    topo.level0PcieMix = false;
    return topo;
}

TopoInfoWithNetLayerDetails MakeMesh1DClosTopo(u32 rankSize = 2U)
{
    TopoInfoWithNetLayerDetails topo = MakeMesh1DTopo(rankSize);
    topo.level0Topo = Level0Shape::MESH_1D_CLOS;
    topo.topoInstDetailsOfLayer.resize(1U);
    topo.topoInstDetailsOfLayer[0].rankNumForTopoType[COMM_TOPO_1DMESH] = {rankSize};
    topo.topoInstDetailsOfLayer[0].rankNumForTopoType[COMM_TOPO_CLOS] = {rankSize};
    return topo;
}

OpParam MakeCcuSchedParam()
{
    OpParam param{};
    param.opType = HcclCMDType::HCCL_CMD_HALF_ALLTOALLV;
    param.engine = COMM_ENGINE_CCU;
    param.opExecuteConfig = OpExecuteConfig::CCU_SCHED;
    param.userRank = 0U;
    return param;
}
} // namespace

class ST_HALF_ALLTOALLV_SELECTOR_TEST : public ::testing::Test {
protected:
    void SetUp() override { ResetAlgEnvConfigInitState(); }
};

// 1D mesh 拓扑兜底：不指定算法名时 level0 为 MESH_1D/MESH_1D_CLOS 即选
// CcuHalfAll2AllVMesh1D
TEST_F(ST_HALF_ALLTOALLV_SELECTOR_TEST, mesh1d_topo_selects_half_alltoallv_fallback)
{
    HalfAlltoAllVAutoSelector selector;
    TopoInfoWithNetLayerDetails topo = MakeMesh1DTopo();
    OpParam param = MakeCcuSchedParam();
    std::string algName;

    EXPECT_EQ(selector.Select(param, &topo, algName), SelectorStatus::MATCH);
    EXPECT_EQ(algName, HALF_A2AV_ALG_NAME);
    EXPECT_EQ(param.opExecuteConfig, OpExecuteConfig::CCU_SCHED);
}

TEST_F(ST_HALF_ALLTOALLV_SELECTOR_TEST, mesh1d_clos_topo_selects_half_alltoallv_fallback)
{
    HalfAlltoAllVAutoSelector selector;
    TopoInfoWithNetLayerDetails topo = MakeMesh1DClosTopo();
    OpParam param = MakeCcuSchedParam();
    std::string algName;

    EXPECT_EQ(selector.Select(param, &topo, algName), SelectorStatus::MATCH);
    EXPECT_EQ(algName, HALF_A2AV_ALG_NAME);
}

// 非 1D mesh 拓扑不兜底：纯 CLOS / MESH_2D 由上层报不支持
TEST_F(ST_HALF_ALLTOALLV_SELECTOR_TEST, non_mesh1d_topo_does_not_match)
{
    HalfAlltoAllVAutoSelector selector;
    OpParam param = MakeCcuSchedParam();
    std::string algName;

    TopoInfoWithNetLayerDetails closTopo = MakeMesh1DTopo();
    closTopo.level0Topo = Level0Shape::CLOS;
    EXPECT_EQ(selector.Select(param, &closTopo, algName), SelectorStatus::NOT_MATCH);

    TopoInfoWithNetLayerDetails twoLevelTopo = MakeMesh1DTopo();
    twoLevelTopo.topoLevelNums = 3U;
    EXPECT_EQ(selector.Select(param, &twoLevelTopo, algName), SelectorStatus::NOT_MATCH);

    TopoInfoWithNetLayerDetails ubRtpTopo = MakeMesh1DTopo();
    ubRtpTopo.level2UbRtp = true;
    EXPECT_EQ(selector.Select(param, &ubRtpTopo, algName), SelectorStatus::NOT_MATCH);
}

// 源端保序上限：rankSize 超过 8 不选择（kernel PostSync 保序方案约束）
TEST_F(ST_HALF_ALLTOALLV_SELECTOR_TEST, rank_size_over_ordering_limit_does_not_match)
{
    HalfAlltoAllVAutoSelector selector;
    TopoInfoWithNetLayerDetails topo = MakeMesh1DTopo(9U);
    OpParam param = MakeCcuSchedParam();
    std::string algName;

    EXPECT_EQ(selector.Select(param, &topo, algName), SelectorStatus::NOT_MATCH);
}

// selector 已注册到 opType 20（不指定 algConfig 时 ExecuteSelector 能查到）
TEST_F(ST_HALF_ALLTOALLV_SELECTOR_TEST, selector_registered_for_half_alltoallv_op_type)
{
    ExecuteSelector selector;
    TopoInfoWithNetLayerDetails topo = MakeMesh1DTopo();
    OpParam param = MakeCcuSchedParam();
    param.opExecuteConfig = OpExecuteConfig::CCU_SCHED;
    std::string algName;

    EXPECT_EQ(selector.Run(param, &topo, algName), HcclResult::HCCL_SUCCESS);
    EXPECT_EQ(algName, HALF_A2AV_ALG_NAME);
}
