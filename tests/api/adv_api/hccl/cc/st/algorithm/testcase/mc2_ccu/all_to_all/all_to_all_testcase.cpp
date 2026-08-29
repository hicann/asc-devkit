/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_all_to_all.h"

#include <vector>

using namespace HcclSim::CcuSt;

namespace {

CcuStScenario MakeScenario(const TopoMeta& topoMeta, HcclDataType dataType, uint64_t sliceSize)
{
    const uint32_t rankSize = CcuStFixture::CountRanks(topoMeta);
    CcuStScenario scenario;
    scenario.topoMeta = topoMeta;
    scenario.dataType = dataType;
    scenario.opType = HcclCMDType::HCCL_CMD_ALLTOALL;
    scenario.sizes.assign(rankSize, std::vector<uint64_t>(rankSize, sliceSize));
    return scenario;
}

} // namespace

// ===== 拓扑泛化（FP16, CcuSchedAllToAllSoleMesh）=====
TEST_F(CcuStAllToAll, CcuSchedAllToAllSoleMesh_2Rank_SingleDie_Fp16_100B)
{
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_FP16, 100);
    scenario.expectedAlgName = "CcuSchedAllToAllSoleMesh";
    VerifyScenario(scenario);
}

TEST_F(CcuStAllToAll, CcuSchedAllToAllSoleMesh_4Rank_SingleDie_Fp16_64B)
{
    TopoMeta topo{{{{0, 1, 2, 3}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_FP16, 64);
    scenario.expectedAlgName = "CcuSchedAllToAllSoleMesh";
    VerifyScenario(scenario);
}

TEST_F(CcuStAllToAll, CcuSchedAllToAllSoleMesh_8Rank_SingleDie_Fp16_64B)
{
    TopoMeta topo{{{{0, 1, 2, 3, 4, 5, 6, 7}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_FP16, 64);
    scenario.expectedAlgName = "CcuSchedAllToAllSoleMesh";
    VerifyScenario(scenario);
}

// ===== 数据类型泛化（2Rank, SingleDie, 100B/rank）=====
TEST_F(CcuStAllToAll, CcuSchedAllToAllSoleMesh_2Rank_SingleDie_Fp32_100B)
{
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_FP32, 100);
    scenario.expectedAlgName = "CcuSchedAllToAllSoleMesh";
    VerifyScenario(scenario);
}

TEST_F(CcuStAllToAll, CcuSchedAllToAllSoleMesh_2Rank_SingleDie_Int8_100B)
{
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_INT8, 100);
    scenario.expectedAlgName = "CcuSchedAllToAllSoleMesh";
    VerifyScenario(scenario);
}

TEST_F(CcuStAllToAll, CcuSchedAllToAllSoleMesh_2Rank_SingleDie_Int32_100B)
{
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_INT32, 100);
    scenario.expectedAlgName = "CcuSchedAllToAllSoleMesh";
    VerifyScenario(scenario);
}

TEST_F(CcuStAllToAll, CcuSchedAllToAllSoleMesh_2Rank_SingleDie_Bfp16_100B)
{
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_BFP16, 100);
    scenario.expectedAlgName = "CcuSchedAllToAllSoleMesh";
    VerifyScenario(scenario);
}

// ===== 数据量泛化（2Rank, SingleDie, FP16）=====
TEST_F(CcuStAllToAll, CcuSchedAllToAllSoleMesh_2Rank_SingleDie_Fp16_1KB)
{
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_FP16, 1024);
    scenario.expectedAlgName = "CcuSchedAllToAllSoleMesh";
    VerifyScenario(scenario);
}

// ===== GroupCopy CalGoSize 分块边界 =====
// CalGoSize config: loopCount=8, memSlice=32768, loopSize=262144
// 以下用例覆盖 CalGoSize 的 m/n/p 分块边界和 GroupCopy 两个 LoopGroup 的执行路径

TEST_F(CcuStAllToAll, CcuSchedAllToAllSoleMesh_GroupCopy_Boundary_1B)
{
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_FP16, 1);
    scenario.expectedAlgName = "CcuSchedAllToAllSoleMesh";
    VerifyScenario(scenario);
}

TEST_F(CcuStAllToAll, CcuSchedAllToAllSoleMesh_GroupCopy_Boundary_MemSliceMinus1)
{
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_INT8, 32767);
    scenario.expectedAlgName = "CcuSchedAllToAllSoleMesh";
    VerifyScenario(scenario);
}

TEST_F(CcuStAllToAll, CcuSchedAllToAllSoleMesh_GroupCopy_Boundary_MemSlice)
{
    // memSlice=32768: n=1, p=0, 第二个 LoopGroup 走 n!=0 && p==0 分支
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_INT8, 32768);
    scenario.expectedAlgName = "CcuSchedAllToAllSoleMesh";
    VerifyScenario(scenario);
}

TEST_F(CcuStAllToAll, CcuSchedAllToAllSoleMesh_GroupCopy_Boundary_MemSlicePlus1)
{
    // memSlice+1=32769: n=1, p=1, 第二个 LoopGroup 走 n!=0 && p!=0 分支
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_INT8, 32769);
    scenario.expectedAlgName = "CcuSchedAllToAllSoleMesh";
    VerifyScenario(scenario);
}

TEST_F(CcuStAllToAll, CcuSchedAllToAllSoleMesh_GroupCopy_Boundary_LoopSizeMinus1)
{
    // loopSize-1=262143: n=7, p=32767, 第二个 LoopGroup 两个 loop 都执行
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_INT8, 262143);
    scenario.expectedAlgName = "CcuSchedAllToAllSoleMesh";
    VerifyScenario(scenario);
}

TEST_F(CcuStAllToAll, CcuSchedAllToAllSoleMesh_GroupCopy_Boundary_LoopSize)
{
    // loopSize=262144: m=1, n=0, p=0, 只有第一个 LoopGroup 执行
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_INT8, 262144);
    scenario.expectedAlgName = "CcuSchedAllToAllSoleMesh";
    VerifyScenario(scenario);
}

TEST_F(CcuStAllToAll, CcuSchedAllToAllSoleMesh_GroupCopy_Boundary_LoopSizePlus1)
{
    // loopSize+1=262145: m=1, n=0, p=1, 两个 LoopGroup 都执行
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_INT8, 262145);
    scenario.expectedAlgName = "CcuSchedAllToAllSoleMesh";
    VerifyScenario(scenario);
}

TEST_F(CcuStAllToAll, CcuSchedAllToAllSoleMesh_GroupCopy_Boundary_2xLoopSize)
{
    // 2*loopSize=524288: m=2, n=0, p=0, 第一个 LoopGroup 内层迭代 3 次
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_INT8, 524288);
    scenario.expectedAlgName = "CcuSchedAllToAllSoleMesh";
    VerifyScenario(scenario);
}

TEST_F(CcuStAllToAll, CcuSchedAllToAllSoleMesh_GroupCopy_Boundary_2xLoopSizePlus1)
{
    // 2*loopSize+1=524289: m=2, n=0, p=1, 两个 LoopGroup 都执行
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_INT8, 524289);
    scenario.expectedAlgName = "CcuSchedAllToAllSoleMesh";
    VerifyScenario(scenario);
}

TEST_F(CcuStAllToAll, CcuSchedAllToAllSoleMesh_GroupCopy_Boundary_LoopSizePlusMemSlice)
{
    // loopSize+memSlice=294912: m=1, n=1, p=0, 第一个+第二个 LoopGroup 都执行
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_INT8, 294912);
    scenario.expectedAlgName = "CcuSchedAllToAllSoleMesh";
    VerifyScenario(scenario);
}

TEST_F(CcuStAllToAll, CcuSchedAllToAllSoleMesh_2Rank_SingleDie_Fp16_1MB)
{
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_FP16, 1024 * 1024);
    scenario.expectedAlgName = "CcuSchedAllToAllSoleMesh";
    VerifyScenario(scenario);
}

TEST_F(CcuStAllToAll, CcuSchedAllToAllSoleMesh_2Rank_SingleDie_Fp16_10MB)
{
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_FP16, 10 * 1024 * 1024);
    scenario.expectedAlgName = "CcuSchedAllToAllSoleMesh";
    VerifyScenario(scenario);
}

// ===== 边界条件 =====
TEST_F(CcuStAllToAll, CcuSchedAllToAllSoleMesh_2Rank_SingleDie_Fp16_0B)
{
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_FP16, 0);
    scenario.expectedAlgName = "CcuSchedAllToAllSoleMesh";
    VerifyScenario(scenario);
}

// ===== 强制算法配置 =====
TEST_F(CcuStAllToAll, CcuSchedAllToAllSoleMesh_ForcedAlg_2Rank_SingleDie_Fp16_100B)
{
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_FP16, 100);
    scenario.algConfig = "CcuSchedAllToAllSoleMesh";
    scenario.expectedAlgName = "CcuSchedAllToAllSoleMesh";
    VerifyScenario(scenario);
}

// ===== 日志级别动态控制验证 =====
TEST_F(CcuStAllToAll, CcuSchedAllToAllSoleMesh_LogLevel_Control_2Rank_SingleDie_Fp16_100B)
{
    TopoMeta topo{{{{0, 1}}}};

    // EnableLog(0) 开 DEBUG，验证 INFO 日志能输出
    testing::internal::CaptureStdout();
    EnableLog(0);
    {
        CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_FP16, 100);
        scenario.expectedAlgName = "CcuSchedAllToAllSoleMesh";
        VerifyScenario(scenario);
    }
    DisableLog();
    std::string debugOutput = testing::internal::GetCapturedStdout();
    EXPECT_FALSE(debugOutput.empty());
    EXPECT_NE(debugOutput.find("INFO"), std::string::npos);

    // DisableLog() 关到 ERROR，验证 INFO 日志不输出
    testing::internal::CaptureStdout();
    DisableLog();
    {
        CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_FP16, 100);
        scenario.expectedAlgName = "CcuSchedAllToAllSoleMesh";
        VerifyScenario(scenario);
    }
    std::string errorOnlyOutput = testing::internal::GetCapturedStdout();
    EXPECT_EQ(errorOnlyOutput.find("INFO"), std::string::npos);
}
