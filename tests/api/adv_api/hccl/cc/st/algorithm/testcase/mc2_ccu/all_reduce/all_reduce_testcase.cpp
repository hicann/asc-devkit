/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_all_reduce.h"

#include <vector>

using namespace HcclSim::CcuSt;

namespace {

CcuStScenario MakeScenario(const TopoMeta& topoMeta, HcclDataType dataType, uint64_t count)
{
    const uint32_t rankSize = CcuStFixture::CountRanks(topoMeta);
    const uint32_t typeSize = DATATYPE_SIZE_TABLE[dataType];
    const uint64_t dataSize = count * typeSize;
    CcuStScenario scenario;
    scenario.topoMeta = topoMeta;
    scenario.dataType = dataType;
    scenario.opType = HcclCMDType::HCCL_CMD_ALLREDUCE;
    scenario.count = count;
    scenario.reduceType = HCCL_REDUCE_SUM;
    scenario.sizes.assign(rankSize, std::vector<uint64_t>(rankSize, dataSize));
    return scenario;
}

} // namespace

// ===== 拓扑泛化（FP16, CcuSchedAllReduceSoleMesh）=====
TEST_F(CcuStAllReduce, CcuSchedAllReduceSoleMesh_2Rank_SingleDie_Fp16_50Elem)
{
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_FP16, 50);
    scenario.expectedAlgName = "CcuSchedAllReduceSoleMesh";
    VerifyScenario(scenario);
}

TEST_F(CcuStAllReduce, CcuSchedAllReduceSoleMesh_4Rank_SingleDie_Fp16_50Elem)
{
    TopoMeta topo{{{{0, 1, 2, 3}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_FP16, 50);
    scenario.expectedAlgName = "CcuSchedAllReduceSoleMesh";
    VerifyScenario(scenario);
}

TEST_F(CcuStAllReduce, CcuSchedAllReduceSoleMesh_8Rank_SingleDie_Fp16_50Elem)
{
    TopoMeta topo{{{{0, 1, 2, 3, 4, 5, 6, 7}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_FP16, 50);
    scenario.expectedAlgName = "CcuSchedAllReduceSoleMesh";
    VerifyScenario(scenario);
}

// ===== 数据类型泛化（2Rank, SingleDie, 50Elem）=====
TEST_F(CcuStAllReduce, CcuSchedAllReduceSoleMesh_2Rank_SingleDie_Fp32_50Elem)
{
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_FP32, 50);
    scenario.expectedAlgName = "CcuSchedAllReduceSoleMesh";
    VerifyScenario(scenario);
}

TEST_F(CcuStAllReduce, CcuSchedAllReduceSoleMesh_2Rank_SingleDie_Int32_50Elem)
{
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_INT32, 50);
    scenario.expectedAlgName = "CcuSchedAllReduceSoleMesh";
    VerifyScenario(scenario);
}

TEST_F(CcuStAllReduce, CcuSchedAllReduceSoleMesh_2Rank_SingleDie_Bfp16_50Elem)
{
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_BFP16, 50);
    scenario.expectedAlgName = "CcuSchedAllReduceSoleMesh";
    VerifyScenario(scenario);
}

// ===== 数据量泛化（2Rank, SingleDie, FP16）=====
TEST_F(CcuStAllReduce, CcuSchedAllReduceSoleMesh_2Rank_SingleDie_Fp16_1Elem)
{
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_FP16, 1);
    scenario.expectedAlgName = "CcuSchedAllReduceSoleMesh";
    VerifyScenario(scenario);
}

TEST_F(CcuStAllReduce, CcuSchedAllReduceSoleMesh_2Rank_SingleDie_Fp16_512Elem)
{
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_FP16, 512);
    scenario.expectedAlgName = "CcuSchedAllReduceSoleMesh";
    VerifyScenario(scenario);
}

TEST_F(CcuStAllReduce, CcuSchedAllReduceSoleMesh_2Rank_SingleDie_Fp16_4096Elem)
{
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_FP16, 4096);
    scenario.expectedAlgName = "CcuSchedAllReduceSoleMesh";
    VerifyScenario(scenario);
}

TEST_F(CcuStAllReduce, CcuSchedAllReduceSoleMesh_2Rank_SingleDie_Fp16_512KElem)
{
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_FP16, 512 * 1024);
    scenario.expectedAlgName = "CcuSchedAllReduceSoleMesh";
    VerifyScenario(scenario);
}

// ===== 边界条件 =====
TEST_F(CcuStAllReduce, CcuSchedAllReduceSoleMesh_2Rank_SingleDie_Fp16_0Elem)
{
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_FP16, 0);
    scenario.expectedAlgName = "CcuSchedAllReduceSoleMesh";
    VerifyScenario(scenario);
}

// ===== 强制算法配置 =====
TEST_F(CcuStAllReduce, CcuSchedAllReduceSoleMesh_ForcedAlg_2Rank_SingleDie_Fp16_50Elem)
{
    TopoMeta topo{{{{0, 1}}}};
    CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_FP16, 50);
    scenario.algConfig = "CcuSchedAllReduceSoleMesh";
    scenario.expectedAlgName = "CcuSchedAllReduceSoleMesh";
    VerifyScenario(scenario);
}

// ===== 日志级别动态控制验证 =====
TEST_F(CcuStAllReduce, CcuSchedAllReduceSoleMesh_LogLevel_Control_2Rank_SingleDie_Fp16_50Elem)
{
    TopoMeta topo{{{{0, 1}}}};

    testing::internal::CaptureStdout();
    EnableLog(0);
    {
        CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_FP16, 50);
        scenario.expectedAlgName = "CcuSchedAllReduceSoleMesh";
        VerifyScenario(scenario);
    }
    DisableLog();
    std::string debugOutput = testing::internal::GetCapturedStdout();
    EXPECT_FALSE(debugOutput.empty());
    EXPECT_NE(debugOutput.find("INFO"), std::string::npos);

    testing::internal::CaptureStdout();
    DisableLog();
    {
        CcuStScenario scenario = MakeScenario(topo, HCCL_DATA_TYPE_FP16, 50);
        scenario.expectedAlgName = "CcuSchedAllReduceSoleMesh";
        VerifyScenario(scenario);
    }
    std::string errorOnlyOutput = testing::internal::GetCapturedStdout();
    EXPECT_EQ(errorOnlyOutput.find("INFO"), std::string::npos);
}
