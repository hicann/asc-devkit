/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_all_to_all_v.h"

#include "alg_param.h"

using namespace HcclSim::CcuSt;

namespace {

CcuStScenario MakeScenario(const TopoMeta& topoMeta, HcclDataType dataType, uint64_t baseSize)
{
    const uint32_t rankSize = CcuStFixture::CountRanks(topoMeta);
    const uint64_t typeSize = DATATYPE_SIZE_TABLE[dataType];
    CcuStScenario scenario;
    scenario.topoMeta = topoMeta;
    scenario.dataType = dataType;
    scenario.opType = HcclCMDType::HCCL_CMD_ALLTOALLV;
    scenario.expectedAlgName = "CcuSchedAllToAllVSoleMesh";
    scenario.algConfig = scenario.expectedAlgName;
    scenario.sizes.resize(rankSize, std::vector<uint64_t>(rankSize));
    for (uint32_t source = 0; source < rankSize; ++source) {
        for (uint32_t destination = 0; destination < rankSize; ++destination) {
            scenario.sizes[source][destination] =
                baseSize == 0 ? 0 : baseSize + ((source * rankSize + destination) % 3U) * typeSize;
        }
    }
    return scenario;
}

} // namespace

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMesh_2Rank_Fp16_100B)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_FP16, 100));
}

// ===== 大禹纯 CLOS（DIE0-only 建链）=====
// scenario.pureClosTopo=true 时 TopoModel 把 L0 建模为单 CLOS 实例全互联（UBC_CTP 链路），
// selector 走纯 CLOS 分支选 CcuSchedAllToAllVSoleMesh，模板 CalcChannelRequestMesh1DOnDie
// 在 devPhyId 4/4 翻转 die 的双 die 链路中只取 DIE0（stub：devPhyId>=4 → die1）。

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMesh_2Rank_PureClos_Fp16_100B)
{
    CcuStScenario scenario = MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_FP16, 100);
    scenario.pureClosTopo = true;
    VerifyScenario(scenario);
}

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMesh_8Rank_PureClos_Fp16_64B)
{
    // 8 rank 双 die 混布（phyId 0-3 = die0，4-7 = die1）：盲选建链会混出跨 die 通道集，
    // DIE0-only 过滤后 7 条通道全部落在 die0
    CcuStScenario scenario = MakeScenario(TopoMeta{{{{0, 1, 2, 3, 4, 5, 6, 7}}}}, HCCL_DATA_TYPE_FP16, 64);
    scenario.pureClosTopo = true;
    VerifyScenario(scenario);
}

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMesh_8Rank_PureClos_Fp32_1KB)
{
    CcuStScenario scenario = MakeScenario(TopoMeta{{{{0, 1, 2, 3, 4, 5, 6, 7}}}}, HCCL_DATA_TYPE_FP32, 1024);
    scenario.pureClosTopo = true;
    VerifyScenario(scenario);
}

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMesh_4Rank_Fp16_64B)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1, 2, 3}}}}, HCCL_DATA_TYPE_FP16, 64));
}

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMesh_8Rank_Fp16_64B)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1, 2, 3, 4, 5, 6, 7}}}}, HCCL_DATA_TYPE_FP16, 64));
}

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMesh_2Rank_Fp32_100B)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_FP32, 100));
}

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMesh_2Rank_Int8_100B)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_INT8, 100));
}

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMesh_2Rank_Int32_100B)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_INT32, 100));
}

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMesh_2Rank_Bfp16_100B)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_BFP16, 100));
}

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMesh_2Rank_Fp16_1KB)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_FP16, 1024));
}

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMesh_GroupCopy_Boundary_1B)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_INT8, 1));
}

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMesh_GroupCopy_Boundary_MemSliceMinus1)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_INT8, 32767));
}

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMesh_GroupCopy_Boundary_MemSlice)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_INT8, 32768));
}

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMesh_GroupCopy_Boundary_MemSlicePlus1)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_INT8, 32769));
}

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMesh_GroupCopy_Boundary_LoopSizeMinus1)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_INT8, 262143));
}

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMesh_GroupCopy_Boundary_LoopSize)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_INT8, 262144));
}

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMesh_GroupCopy_Boundary_LoopSizePlus1)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_INT8, 262145));
}

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMesh_GroupCopy_Boundary_2xLoopSize)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_INT8, 524288));
}

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMesh_GroupCopy_Boundary_2xLoopSizePlus1)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_INT8, 524289));
}

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMesh_GroupCopy_Boundary_LoopSizePlusMemSlice)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_INT8, 294912));
}

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMesh_2Rank_Fp16_1MB)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_FP16, 1024 * 1024));
}

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMesh_2Rank_Fp16_10MB)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_FP16, 10 * 1024 * 1024));
}

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMesh_ZeroLength)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_FP16, 0));
}

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMesh_2Rank_Fp16_256MB)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_FP16, 256ULL * 1024 * 1024));
}

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMesh_ForcedAlg)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_FP16, 100));
}

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMesh_LogLevel_Control)
{
    const TopoMeta topo{{{{0, 1}}}};

    testing::internal::CaptureStdout();
    EnableLog(0);
    VerifyScenario(MakeScenario(topo, HCCL_DATA_TYPE_FP16, 100));
    DisableLog();
    const std::string debugOutput = testing::internal::GetCapturedStdout();
    EXPECT_FALSE(debugOutput.empty());
    EXPECT_NE(debugOutput.find("INFO"), std::string::npos);

    testing::internal::CaptureStdout();
    DisableLog();
    VerifyScenario(MakeScenario(topo, HCCL_DATA_TYPE_FP16, 100));
    const std::string errorOnlyOutput = testing::internal::GetCapturedStdout();
    EXPECT_EQ(errorOnlyOutput.find("INFO"), std::string::npos);
}

// ===== 并发 A2AV（MESH_1D_CLOS 双实例：mesh 优先链路 + CLOS MultiJetty 链路，双 mission）=====
// 单 server 内全部 phyId 携带 MESH_1D_CLOS_PHYID_MARK 时按 MESH_1D_CLOS 建模，
// selector 判定 meshNum==closNum 且 rankSize<=4 → CcuSchedAllToAllVSoleMeshConcurrent

namespace {

CcuStScenario MakeConcurrentScenario(const TopoMeta& topoMeta, HcclDataType dataType, uint64_t baseSize)
{
    CcuStScenario scenario = MakeScenario(topoMeta, dataType, baseSize);
    scenario.expectedAlgName = "CcuSchedAllToAllVSoleMeshConcurrent";
    scenario.algConfig = scenario.expectedAlgName;
    // mesh/clos 双链路：mission0 mesh 优先建链、mission1 CLOS-only 建链同时产出通道
    scenario.meshClosDualLink = true;
    return scenario;
}

TopoMeta MakeMesh1dClosTopo(const std::vector<uint32_t>& phyIds)
{
    ServerMeta server;
    for (auto phyId : phyIds) {
        server.push_back(phyId | MESH_1D_CLOS_PHYID_MARK);
    }
    return TopoMeta{{server}};
}

} // namespace

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMeshConcurrent_4Rank_Mesh1DClos_Fp16_100B)
{
    VerifyScenario(MakeConcurrentScenario(MakeMesh1dClosTopo({0, 1, 2, 3}), HCCL_DATA_TYPE_FP16, 100));
}

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMeshConcurrent_4Rank_Mesh1DClos_Fp32_1KB)
{
    VerifyScenario(MakeConcurrentScenario(MakeMesh1dClosTopo({0, 1, 2, 3}), HCCL_DATA_TYPE_FP32, 1024));
}

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMeshConcurrent_2Rank_Mesh1DClos_Bfp16_64B)
{
    VerifyScenario(MakeConcurrentScenario(MakeMesh1dClosTopo({0, 1}), HCCL_DATA_TYPE_BFP16, 64));
}

TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMeshConcurrent_4Rank_Mesh1DClos_Int32_128B)
{
    VerifyScenario(MakeConcurrentScenario(MakeMesh1dClosTopo({0, 1, 2, 3}), HCCL_DATA_TYPE_INT32, 128));
}

// 零长度：全部对端 sendSize=0，双 mission 各自空转（尾块为 0，仅 EventRecord）
TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMeshConcurrent_4Rank_Mesh1DClos_Fp16_ZeroLength)
{
    VerifyScenario(MakeConcurrentScenario(MakeMesh1dClosTopo({0, 1, 2, 3}), HCCL_DATA_TYPE_FP16, 0));
}

// selector 自动选算（非 forced）：mesh 实例数==clos 实例数且 4P → Concurrent
TEST_F(CcuStAllToAllV, CcuSchedAllToAllVSoleMeshConcurrent_4Rank_Mesh1DClos_Selector_Fp16_100B)
{
    CcuStScenario scenario = MakeConcurrentScenario(MakeMesh1dClosTopo({0, 1, 2, 3}), HCCL_DATA_TYPE_FP16, 100);
    scenario.algConfig.clear();
    VerifyScenario(scenario);
}
