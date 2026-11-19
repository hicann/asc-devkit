/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "alg_env_config.h"
#include "alltoall_auto_selector.h"
#include "alltoallv_auto_selector.h"
#include "gtest/gtest.h"

using namespace mc2_ops_hccl;

namespace checker {
namespace {

OpParam MakeCcuParam(HcclCMDType opType)
{
    OpParam param{};
    param.opType = opType;
    param.engine = COMM_ENGINE_CCU;
    param.opExecuteConfig = OpExecuteConfig::CCU_SCHED;
    param.userRank = 0U;
    param.all2AllVDataDes.sendType = HCCL_DATA_TYPE_FP32; // 零值0是INT8，会被多级CCU分支拒绝
    param.all2AllVDataDes.recvType = HCCL_DATA_TYPE_FP32;
    return param;
}

TopoInfoWithNetLayerDetails MakeMesh1DTopo(u32 rankSize = 4U, u32 topoLevelNums = 1U)
{
    TopoInfoWithNetLayerDetails topo{};
    topo.userRank = 0U;
    topo.userRankSize = rankSize;
    topo.serverNum = 1U;
    topo.level0Topo = Level0Shape::MESH_1D;
    topo.level0MeshType = Level0MeshType::SINGLE_DIE;
    topo.topoLevelNums = topoLevelNums;
    return topo;
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

// 框架行为无关的断言：CCU 分支不得命中。
// 当前框架 CCU NOT_MATCH 后降级 AICPU（整链 MATCH + Aicpu 名）；框架计划取消该兜底改为直接报错
// （整链 NOT_MATCH，且 algName 可能残留"设名后拒绝"的 CCU 名）。两版共同不变量：
// 只有 MATCH 且结果为 CCU 算法名才表示 CCU 分支命中——此情况绝不允许发生。
::testing::AssertionResult CcuNotMatched(SelectorStatus status, const std::string& algName)
{
    if (status == SelectorStatus::MATCH && algName.rfind("CcuSched", 0) == 0) {
        return ::testing::AssertionFailure() << "CCU algorithm unexpectedly matched: " << algName;
    }
    return ::testing::AssertionSuccess();
}

} // namespace

class ST_ALL_TO_ALL_CCU_TEST : public ::testing::Test {
protected:
    void SetUp() override { ResetAlgEnvConfigInitState(); }
};

TEST_F(ST_ALL_TO_ALL_CCU_TEST, ccu_mesh1d_clos_nullptr_sendcounts_selects_smallest_path)
{
    AlltoAllAutoSelector selector;
    TopoInfoWithNetLayerDetails topo = MakeConcurrentTopo();
    OpParam param = MakeCcuParam(HCCL_CMD_ALLTOALL);
    param.all2AllVDataDes.sendCounts = nullptr;
    std::string algName;
    EXPECT_EQ(selector.Select(param, &topo, algName), SelectorStatus::MATCH);
    EXPECT_EQ(algName, "CcuSchedAllToAllMesh1DMultiJetty"); // dataSize=0 → 非并发
}

TEST_F(ST_ALL_TO_ALL_CCU_TEST, ccu_multilevel_mesh1d_never_matches_ccu)
{
    AlltoAllAutoSelector selector;
    TopoInfoWithNetLayerDetails topo = MakeMesh1DTopo(8U, 2U); // 多级 MESH_1D ≤64
    OpParam param = MakeCcuParam(HCCL_CMD_ALLTOALL);
    std::string algName;
    // CCU MultiLink 未注册 → CCU 分支必须拒绝（断言与框架兜底行为无关，见 CcuNotMatched）
    EXPECT_TRUE(CcuNotMatched(selector.Select(param, &topo, algName), algName));
}

TEST_F(ST_ALL_TO_ALL_CCU_TEST, ccu_alltoallv_single_level_mesh1d_selects_sole_mesh)
{
    AlltoAllVAutoSelector selector;
    TopoInfoWithNetLayerDetails topo = MakeMesh1DTopo(8U); // 单级 MESH_1D，alltoallv kernel 多event无rank上限
    OpParam param = MakeCcuParam(HCCL_CMD_ALLTOALLV);
    std::string algName;
    EXPECT_EQ(selector.Select(param, &topo, algName), SelectorStatus::MATCH);
    EXPECT_EQ(algName, "CcuSchedAllToAllVSoleMesh");
}

TEST_F(ST_ALL_TO_ALL_CCU_TEST, ccu_alltoallv_multilevel_small_rank_selects_sole_mesh)
{
    AlltoAllVAutoSelector selector;
    TopoInfoWithNetLayerDetails topo = MakeMesh1DTopo(4U, 2U); // 多级 ≤4P
    OpParam param = MakeCcuParam(HCCL_CMD_ALLTOALLV);
    std::string algName;
    EXPECT_EQ(selector.Select(param, &topo, algName), SelectorStatus::MATCH);
    EXPECT_EQ(algName, "CcuSchedAllToAllVSoleMesh");
}

TEST_F(ST_ALL_TO_ALL_CCU_TEST, ccu_alltoallv_pcie_mix_all_connected_selects_sole_mesh)
{
    AlltoAllVAutoSelector selector;
    TopoInfoWithNetLayerDetails topo = MakeConcurrentTopo();
    topo.level0PcieMix = true;
    topo.netLayerDetails.localNetInsSizeOfLayer = {4U}; // 全连通 1DMESH
    OpParam param = MakeCcuParam(HCCL_CMD_ALLTOALLV);
    std::string algName;
    EXPECT_EQ(selector.Select(param, &topo, algName), SelectorStatus::MATCH);
    EXPECT_EQ(algName, "CcuSchedAllToAllVSoleMesh");
}

TEST_F(ST_ALL_TO_ALL_CCU_TEST, ccu_alltoallv_mesh1d_clos_selects_concurrent_or_not_match)
{
    AlltoAllVAutoSelector selector;
    TopoInfoWithNetLayerDetails topo = MakeConcurrentTopo();
    topo.topoInstDetailsOfLayer[0].rankNumForTopoType[COMM_TOPO_CLOS] = {3U}; // mesh != clos
    OpParam param = MakeCcuParam(HCCL_CMD_ALLTOALLV);
    std::string algName;
    // CCU: MultiJetty 未注册 NOT_MATCH；AICPU: mesh!=clos 也 NOT_MATCH → 整链 NOT_MATCH
    // （该形态 AICPU 同样不支持，断言在框架取消兜底前后一致）
    EXPECT_EQ(selector.Select(param, &topo, algName), SelectorStatus::NOT_MATCH);

    // mesh == clos 且 4P：CCU Concurrent 已注册 → CCU 分支命中并发算法
    // （Select 会按降级链改写 opParam.opExecuteConfig，二次调用前需重置，否则 CCU 分支不会进入）
    topo = MakeConcurrentTopo();
    param = MakeCcuParam(HCCL_CMD_ALLTOALLV);
    EXPECT_EQ(selector.Select(param, &topo, algName), SelectorStatus::MATCH);
    EXPECT_EQ(algName, "CcuSchedAllToAllVSoleMeshConcurrent");
}

TEST_F(ST_ALL_TO_ALL_CCU_TEST, ccu_alltoallv_int8_multilevel_never_matches_ccu)
{
    AlltoAllVAutoSelector selector;
    TopoInfoWithNetLayerDetails topo = MakeMesh1DTopo(4U, 2U);
    OpParam param = MakeCcuParam(HCCL_CMD_ALLTOALLV);
    param.all2AllVDataDes.sendType = HCCL_DATA_TYPE_INT8;
    std::string algName;
    // CCU: int8 拒绝（对齐 hccl）→ CCU 分支必须拒绝（断言与框架兜底行为无关）
    EXPECT_TRUE(CcuNotMatched(selector.Select(param, &topo, algName), algName));
}

TEST_F(ST_ALL_TO_ALL_CCU_TEST, ccu_alltoallv_inplace_never_matches_ccu)
{
    AlltoAllVAutoSelector selector;
    TopoInfoWithNetLayerDetails topo = MakeMesh1DTopo(8U);
    OpParam param = MakeCcuParam(HCCL_CMD_ALLTOALLV);
    static u64 inplaceBuf[64] = {0};
    param.inputPtr = inplaceBuf; // sendbuf == recvbuf：原地 AllToAllV
    param.outputPtr = inplaceBuf;
    param.inputSize = sizeof(inplaceBuf);
    param.outputSize = sizeof(inplaceBuf);
    std::string algName;
    // ccu_schedule 不支持原地算子（与 alltoall/all_gather 等兄弟实现契约一致）：
    // CCU 分支必须拒绝（断言与框架兜底行为无关）
    EXPECT_TRUE(CcuNotMatched(selector.Select(param, &topo, algName), algName));
}

TEST_F(ST_ALL_TO_ALL_CCU_TEST, ccu_alltoallv_multilevel_2die_never_matches_ccu)
{
    AlltoAllVAutoSelector selector;
    TopoInfoWithNetLayerDetails topo = MakeMesh1DTopo(4U, 2U); // 多级 MESH_1D ≤4P
    OpParam param = MakeCcuParam(HCCL_CMD_ALLTOALLV);
    std::string algName;
    // 2Die 形态未经 SoleMesh 验证（单级路径同样拒绝），对齐单级契约：CCU 分支必须拒绝
    // （Select 会按降级链改写 opParam.opExecuteConfig，二次调用前需重置，否则 CCU 分支不会进入）
    topo.level0MeshType = Level0MeshType::TWO_DIE_REGULAR;
    EXPECT_TRUE(CcuNotMatched(selector.Select(param, &topo, algName), algName));
    topo.level0MeshType = Level0MeshType::TWO_DIE_NOT_REGULAR;
    param = MakeCcuParam(HCCL_CMD_ALLTOALLV);
    EXPECT_TRUE(CcuNotMatched(selector.Select(param, &topo, algName), algName));
}

TEST_F(ST_ALL_TO_ALL_CCU_TEST, ccu_alltoallv_multilevel_pcie_mix_connectivity_contract)
{
    AlltoAllVAutoSelector selector;
    OpParam param = MakeCcuParam(HCCL_CMD_ALLTOALLV);
    std::string algName;

    // 多级 pcie 混插未全连通：与单级契约一致拒绝（SoleMesh 走不了 pcie 通道），
    // 断言与框架兜底行为无关
    TopoInfoWithNetLayerDetails topo = MakeConcurrentTopo(4U);
    topo.topoLevelNums = 2U;
    topo.level0PcieMix = true; // 未设 localNetInsSizeOfLayer → 非全连通
    EXPECT_TRUE(CcuNotMatched(selector.Select(param, &topo, algName), algName));

    // 多级 pcie 混插全连通：与单级一致，仍可 MATCH CCU SoleMesh
    // （Select 会按降级链改写 opParam.opExecuteConfig，二次调用前需重置）
    topo.netLayerDetails.localNetInsSizeOfLayer = {4U};
    param = MakeCcuParam(HCCL_CMD_ALLTOALLV);
    EXPECT_EQ(selector.Select(param, &topo, algName), SelectorStatus::MATCH);
    EXPECT_EQ(algName, "CcuSchedAllToAllVSoleMesh");
}

TEST_F(ST_ALL_TO_ALL_CCU_TEST, ccu_alltoallv_single_level_mesh1d_over_rank_limit_never_matches_ccu)
{
    AlltoAllVAutoSelector selector;
    TopoInfoWithNetLayerDetails topo = MakeMesh1DTopo(17U); // 单级 MESH_1D，超模板 rank 上限
    OpParam param = MakeCcuParam(HCCL_CMD_ALLTOALLV);
    std::string algName;
    // 模板 CalcRes 有 CCU_MAX_RANK_SIZE=16 元数据守卫（alg_param.h），超限会在资源阶段报错；
    // selector 需前置拒绝（kernel 虽为多 event，模板守卫仍限 16）
    EXPECT_TRUE(CcuNotMatched(selector.Select(param, &topo, algName), algName));
}

TEST_F(ST_ALL_TO_ALL_CCU_TEST, ccu_alltoallv_multilevel_mesh1d_clos_non_pcie_mix_never_matches_ccu)
{
    AlltoAllVAutoSelector selector;
    TopoInfoWithNetLayerDetails topo = MakeConcurrentTopo(4U); // MESH_1D_CLOS 非 pcieMix
    topo.topoLevelNums = 2U;
    OpParam param = MakeCcuParam(HCCL_CMD_ALLTOALLV);
    std::string algName;
    // SoleMesh（TopoMatch1D 扁平 hierarchy）被 executor 的 MESH_1D_CLOS 双实例契约阻塞
    // （BuildResourceHierarchyInfo 要求 infos[0].size()==2，资源阶段必报错），selector 需前置拒绝；
    // pcieMix 场景不受影响（走 else 扁平分支，见 pcie_mix_connectivity_contract 用例）
    EXPECT_TRUE(CcuNotMatched(selector.Select(param, &topo, algName), algName));
}

TEST_F(ST_ALL_TO_ALL_CCU_TEST, ccu_alltoallv_pcie_mix_all_connected_over_rank_limit_never_matches_ccu)
{
    AlltoAllVAutoSelector selector;
    TopoInfoWithNetLayerDetails topo = MakeConcurrentTopo(17U); // pcieMix 全连通但超模板 rank 上限
    topo.level0PcieMix = true;
    topo.netLayerDetails.localNetInsSizeOfLayer = {17U};
    OpParam param = MakeCcuParam(HCCL_CMD_ALLTOALLV);
    std::string algName;
    // 与单级 MESH_1D 分支命中同一算法/模板（CcuTempAlltoAllVMesh1D），
    // 模板 CalcRes 的 CCU_MAX_RANK_SIZE=16 守卫同样生效，selector 需一致前置拒绝
    EXPECT_TRUE(CcuNotMatched(selector.Select(param, &topo, algName), algName));
}

TEST_F(ST_ALL_TO_ALL_CCU_TEST, ccu_pcie_mix_all_connected_selects_sole_mesh)
{
    AlltoAllAutoSelector selector;
    TopoInfoWithNetLayerDetails topo = MakeConcurrentTopo();
    topo.level0PcieMix = true;
    topo.netLayerDetails.localNetInsSizeOfLayer = {4U}; // 全连通 1DMESH
    OpParam param = MakeCcuParam(HCCL_CMD_ALLTOALL);
    std::string algName;
    EXPECT_EQ(selector.Select(param, &topo, algName), SelectorStatus::MATCH);
    EXPECT_EQ(algName, "CcuSchedAllToAllSoleMesh");
}

TEST_F(ST_ALL_TO_ALL_CCU_TEST, ccu_pcie_mix_all_connected_over_rank_limit_never_matches_ccu)
{
    AlltoAllAutoSelector selector;
    TopoInfoWithNetLayerDetails topo = MakeConcurrentTopo(17U); // pcieMix 全连通但超 rank 上限
    topo.level0PcieMix = true;
    topo.netLayerDetails.localNetInsSizeOfLayer = {17U};
    OpParam param = MakeCcuParam(HCCL_CMD_ALLTOALL);
    std::string algName;
    // 与单级 MESH_1D 分支命中同一算法/模板，rank 上限契约一致，前置拒绝（V 版同款）
    EXPECT_TRUE(CcuNotMatched(selector.Select(param, &topo, algName), algName));
}

TEST_F(ST_ALL_TO_ALL_CCU_TEST, ccu_pcie_mix_not_all_connected_never_matches_ccu)
{
    AlltoAllAutoSelector selector;
    TopoInfoWithNetLayerDetails topo = MakeConcurrentTopo(4U);
    topo.level0PcieMix = true;
    topo.netLayerDetails.localNetInsSizeOfLayer = {2U}; // 1DMESH 未覆盖全 rank
    OpParam param = MakeCcuParam(HCCL_CMD_ALLTOALL);
    std::string algName;
    EXPECT_TRUE(CcuNotMatched(selector.Select(param, &topo, algName), algName));
}
} // namespace checker
