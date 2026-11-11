/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

// AllReduce CCU selector 行为用例：对齐 hccl SelectCcuScheduleAlgo 分支框架。
// 已注册算法（CcuSchedAllReduceSoleMesh / CcuSchedAllReduceSoleNHR）MATCH；未注册算法设名后 NOT_MATCH，
// CCU 上下文（KFC）无法运行 AICPU 算法，选不到直接失败，不降级。不依赖 SimWorld 设备仿真。

#include "all_reduce_auto_selector.h"

#include <string>
#include <vector>

#include "gtest/gtest.h"

using namespace mc2_ops_hccl;

namespace HcclSim {
namespace CcuSt {
namespace {

constexpr u64 KB = 1024U;
constexpr u64 FOUR_KB = 4U * KB;
constexpr u64 ONE_MB = 1024U * KB;

OpParam MakeCcuSchedParam(
    u64 count, HcclDataType dataType = HCCL_DATA_TYPE_FP32, HcclReduceOp reduceOp = HcclReduceOp::HCCL_REDUCE_SUM)
{
    OpParam param{};
    param.opType = HcclCMDType::HCCL_CMD_ALLREDUCE;
    param.engine = CommEngine::COMM_ENGINE_CCU;
    param.opExecuteConfig = OpExecuteConfig::CCU_SCHED;
    param.userRank = 0U;
    param.reduceType = reduceOp;
    param.DataDes.count = count;
    param.DataDes.dataType = dataType;
    return param;
}

TopoInfoWithNetLayerDetails MakeSingleLevelMesh1DTopo(u32 rankSize)
{
    TopoInfoWithNetLayerDetails topo{};
    topo.userRank = 0U;
    topo.userRankSize = rankSize;
    topo.serverNum = 1U;
    topo.topoLevelNums = 1U;
    topo.level0Topo = Level0Shape::MESH_1D;
    topo.level0MeshType = Level0MeshType::SINGLE_DIE;
    return topo;
}

// 两层 MESH_1D：localNetInsSizeOfLayer[0] = 2 避开机内单实例分支
TopoInfoWithNetLayerDetails MakeMultiLevelMesh1DTopo(u32 rankSize)
{
    TopoInfoWithNetLayerDetails topo = MakeSingleLevelMesh1DTopo(rankSize);
    topo.topoLevelNums = 2U;
    topo.netLayerDetails.netLayerNum = 2U;
    topo.netLayerDetails.netLayers = {0U, 1U};
    topo.netLayerDetails.localNetInsSizeOfLayer = {2U, 1U};
    return topo;
}

// 单层 UBX（MESH_1D_CLOS 非 pcieMix）：meshRankNum/closRankNum 决定 Equal/Multiple/其他形态
TopoInfoWithNetLayerDetails MakeUbxTopo(u32 rankSize, u32 meshRankNum, u32 closRankNum)
{
    TopoInfoWithNetLayerDetails topo{};
    topo.userRank = 0U;
    topo.userRankSize = rankSize;
    topo.serverNum = 1U;
    topo.topoLevelNums = 1U;
    topo.level0Topo = Level0Shape::MESH_1D_CLOS;
    topo.level0MeshType = Level0MeshType::SINGLE_DIE;
    topo.topoInstDetailsOfLayer.resize(1U);
    topo.topoInstDetailsOfLayer[0].rankNumForTopoType[COMM_TOPO_1DMESH] = {meshRankNum};
    topo.topoInstDetailsOfLayer[0].rankNumForTopoType[COMM_TOPO_CLOS] = {closRankNum};
    return topo;
}

} // namespace

class CcuStAllReduceSelector : public ::testing::Test {
protected:
    // CCU 选择命中已注册算法：状态保持 CCU_SCHED
    void ExpectCcuMatch(
        OpParam& param, TopoInfoWithNetLayerDetails& topo, const std::string& expectedAlg = "CcuSchedAllReduceSoleMesh")
    {
        std::string algName;
        EXPECT_EQ(selector_.Select(param, &topo, algName), SelectorStatus::MATCH);
        EXPECT_EQ(algName, expectedAlg);
        EXPECT_EQ(param.opExecuteConfig, OpExecuteConfig::CCU_SCHED);
    }

    // CCU 未命中（含未注册算法）：直接失败，不降级 AICPU，配置保持 CCU_SCHED
    void ExpectCcuDecline(OpParam& param, TopoInfoWithNetLayerDetails& topo)
    {
        std::string algName;
        EXPECT_EQ(selector_.Select(param, &topo, algName), SelectorStatus::NOT_MATCH);
        EXPECT_EQ(param.opExecuteConfig, OpExecuteConfig::CCU_SCHED);
    }

    // CCU/AICPU 均未命中：整体 NOT_MATCH，配置保持 CCU_SCHED
    void ExpectCcuNotMatch(OpParam& param, TopoInfoWithNetLayerDetails& topo)
    {
        std::string algName;
        EXPECT_EQ(selector_.Select(param, &topo, algName), SelectorStatus::NOT_MATCH);
        EXPECT_EQ(param.opExecuteConfig, OpExecuteConfig::CCU_SCHED);
    }

    AllReduceAutoSelector selector_;
};

// ===== 已注册算法（CcuSchedAllReduceSoleMesh）MATCH =====

TEST_F(CcuStAllReduceSelector, Selector_SingleLevelMesh1d_SmallData_MatchSoleMesh)
{
    TopoInfoWithNetLayerDetails topo = MakeSingleLevelMesh1DTopo(8U);
    OpParam param = MakeCcuSchedParam(FOUR_KB / 4U); // FP32 4KB
    ExpectCcuMatch(param, topo);
}

TEST_F(CcuStAllReduceSelector, Selector_MultiLevelMesh1d_SmallData_MatchSoleMesh)
{
    TopoInfoWithNetLayerDetails topo = MakeMultiLevelMesh1DTopo(16U);
    OpParam param = MakeCcuSchedParam(FOUR_KB / 4U); // ≤512KB 且 ≤64P
    ExpectCcuMatch(param, topo);
}

TEST_F(CcuStAllReduceSelector, Selector_MultiLevel_EmptyLayerInfo_SkipSingleInstanceBranch)
{
    // 异常 topo：层实例信息缺失时不越界读，跳过单实例分支继续按数据量选算
    TopoInfoWithNetLayerDetails topo = MakeMultiLevelMesh1DTopo(16U);
    topo.netLayerDetails.localNetInsSizeOfLayer.clear();
    OpParam param = MakeCcuSchedParam(FOUR_KB / 4U);
    ExpectCcuMatch(param, topo);
}

TEST_F(CcuStAllReduceSelector, Selector_Ubx4P_SmallData_MatchSoleMesh)
{
    TopoInfoWithNetLayerDetails topo = MakeUbxTopo(4U, 4U, 4U); // mesh 数等于 clos 数，4P
    OpParam param = MakeCcuSchedParam(FOUR_KB / 4U);
    ExpectCcuMatch(param, topo);
}

TEST_F(CcuStAllReduceSelector, Selector_PcieMixFullMesh_MatchSoleMesh)
{
    TopoInfoWithNetLayerDetails topo = MakeUbxTopo(4U, 4U, 4U);
    topo.level0PcieMix = true;
    topo.netLayerDetails.localNetInsSizeOfLayer = {4U}; // 1DMESH 覆盖全部 4 卡
    OpParam param = MakeCcuSchedParam(FOUR_KB / 4U);
    ExpectCcuMatch(param, topo);
}

// ===== 未注册算法 / 约束拒绝：CCU NOT_MATCH 直接失败 =====

TEST_F(CcuStAllReduceSelector, Selector_Mesh1d_OverMem2MemLimit_Decline)
{
    TopoInfoWithNetLayerDetails topo = MakeSingleLevelMesh1DTopo(2U); // ratio=2，8MB*2 > 8MB
    OpParam param = MakeCcuSchedParam(ONE_MB / 4U * 8U);
    ExpectCcuDecline(param, topo);
}

TEST_F(CcuStAllReduceSelector, Selector_TwoDieRegular_Unregistered2Die_Decline)
{
    TopoInfoWithNetLayerDetails topo = MakeSingleLevelMesh1DTopo(8U);
    topo.level0MeshType = Level0MeshType::TWO_DIE_REGULAR;
    OpParam param = MakeCcuSchedParam(FOUR_KB / 4U);
    ExpectCcuDecline(param, topo);
}

TEST_F(CcuStAllReduceSelector, Selector_Level1Nhr_MatchSoleNHR)
{
    TopoInfoWithNetLayerDetails topo = MakeMultiLevelMesh1DTopo(16U);
    topo.Level1Nhr = true;
    OpParam param = MakeCcuSchedParam(FOUR_KB / 4U);
    ExpectCcuMatch(param, topo, "CcuSchedAllReduceSoleNHR");
}

TEST_F(CcuStAllReduceSelector, Selector_MultiLevel_SequenceData_Unregistered_Decline)
{
    TopoInfoWithNetLayerDetails topo = MakeMultiLevelMesh1DTopo(16U);
    OpParam param = MakeCcuSchedParam(ONE_MB / 4U); // (512KB, 64MB] 且 <64P 选 SequenceMeshMesh（未注册）
    ExpectCcuDecline(param, topo);
}

TEST_F(CcuStAllReduceSelector, Selector_Prod_Decline)
{
    TopoInfoWithNetLayerDetails topo = MakeSingleLevelMesh1DTopo(8U);
    OpParam param = MakeCcuSchedParam(FOUR_KB / 4U, HCCL_DATA_TYPE_FP32, HcclReduceOp::HCCL_REDUCE_PROD);
    ExpectCcuDecline(param, topo);
}

TEST_F(CcuStAllReduceSelector, Selector_Int64_Decline)
{
    TopoInfoWithNetLayerDetails topo = MakeSingleLevelMesh1DTopo(8U);
    OpParam param = MakeCcuSchedParam(FOUR_KB / 8U, HCCL_DATA_TYPE_INT64);
    ExpectCcuDecline(param, topo);
}

TEST_F(CcuStAllReduceSelector, Selector_Int8_Decline)
{
    TopoInfoWithNetLayerDetails topo = MakeSingleLevelMesh1DTopo(8U);
    OpParam param = MakeCcuSchedParam(FOUR_KB, HCCL_DATA_TYPE_INT8); // MS reduce 不支持 int8
    ExpectCcuDecline(param, topo);
}

TEST_F(CcuStAllReduceSelector, Selector_InPlace_Decline)
{
    TopoInfoWithNetLayerDetails topo = MakeSingleLevelMesh1DTopo(8U);
    OpParam param = MakeCcuSchedParam(FOUR_KB / 4U);
    u64 buffer[512] = {0};
    param.inputPtr = buffer;
    param.outputPtr = buffer;
    param.inputSize = FOUR_KB;
    param.outputSize = FOUR_KB;
    ExpectCcuDecline(param, topo);
}

TEST_F(CcuStAllReduceSelector, Selector_ThreeLevel_Decline)
{
    TopoInfoWithNetLayerDetails topo = MakeSingleLevelMesh1DTopo(8U);
    topo.topoLevelNums = 3U;
    OpParam param = MakeCcuSchedParam(FOUR_KB / 4U);
    ExpectCcuDecline(param, topo);
}

TEST_F(CcuStAllReduceSelector, Selector_Level2UbRtp_Decline)
{
    TopoInfoWithNetLayerDetails topo = MakeSingleLevelMesh1DTopo(8U);
    topo.level2UbRtp = true;
    OpParam param = MakeCcuSchedParam(FOUR_KB / 4U);
    ExpectCcuDecline(param, topo);
}

TEST_F(CcuStAllReduceSelector, Selector_PcieMixNotFullyConnected_Decline)
{
    TopoInfoWithNetLayerDetails topo = MakeUbxTopo(4U, 2U, 4U); // 1DMESH 仅 2 卡，无法全连通
    topo.level0PcieMix = true;
    topo.netLayerDetails.localNetInsSizeOfLayer = {4U};
    OpParam param = MakeCcuSchedParam(FOUR_KB / 4U);
    ExpectCcuDecline(param, topo);
}

// ===== CCU/AICPU 均不支持的拓扑与数据量：整体 NOT_MATCH =====

TEST_F(CcuStAllReduceSelector, Selector_Ubx4P_LargeData_UnregisteredConcur_NotMatch)
{
    TopoInfoWithNetLayerDetails topo = MakeUbxTopo(4U, 4U, 4U);
    OpParam param = MakeCcuSchedParam(ONE_MB / 4U); // ≥512KB 选 ConcurMeshNHRMultiLink（未注册）
    ExpectCcuNotMatch(param, topo);
}

TEST_F(CcuStAllReduceSelector, Selector_UbxRectangular_LargeData_Unregistered_NotMatch)
{
    TopoInfoWithNetLayerDetails topo = MakeUbxTopo(8U, 4U, 8U); // clos 数为 mesh 数 2 倍的矩形
    OpParam param = MakeCcuSchedParam(ONE_MB / 4U);
    ExpectCcuNotMatch(param, topo);
}

TEST_F(CcuStAllReduceSelector, Selector_SingleLevelClos_MatchSoleNHR)
{
    TopoInfoWithNetLayerDetails topo = MakeSingleLevelMesh1DTopo(4U);
    topo.level0Topo = Level0Shape::CLOS;
    OpParam param = MakeCcuSchedParam(FOUR_KB / 4U);
    ExpectCcuMatch(param, topo, "CcuSchedAllReduceSoleNHR");
}

} // namespace CcuSt
} // namespace HcclSim
