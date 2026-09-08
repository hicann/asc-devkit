/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_all_gather.h"

#include "alg_param.h"

using namespace HcclSim::CcuSt;

namespace {

CcuStScenario MakeScenario(const TopoMeta& topoMeta, HcclDataType dataType, uint64_t count)
{
    const uint32_t rankSize = CcuStFixture::CountRanks(topoMeta);
    CcuStScenario scenario;
    scenario.topoMeta = topoMeta;
    scenario.dataType = dataType;
    scenario.opType = HcclCMDType::HCCL_CMD_ALLGATHER;
    scenario.count = count;
    scenario.expectedAlgName = "CcuSchedAllGatherSoleMesh";
    scenario.sizes.assign(rankSize, std::vector<uint64_t>(rankSize, count * DATATYPE_SIZE_TABLE[dataType]));
    return scenario;
}

} // namespace

// ===== Topology coverage (FP16) =====
TEST_F(CcuStAllGather, CcuSchedAllGatherSoleMesh_2Rank_SingleDie_Fp16_100B)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_FP16, 50));
}

TEST_F(CcuStAllGather, CcuSchedAllGatherSoleMesh_4Rank_SingleDie_Fp16_64B)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1, 2, 3}}}}, HCCL_DATA_TYPE_FP16, 32));
}

TEST_F(CcuStAllGather, CcuSchedAllGatherSoleMesh_8Rank_SingleDie_Fp16_64B)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1, 2, 3, 4, 5, 6, 7}}}}, HCCL_DATA_TYPE_FP16, 32));
}

// ===== Data type coverage (2 ranks, 100 B/rank) =====
TEST_F(CcuStAllGather, CcuSchedAllGatherSoleMesh_2Rank_SingleDie_Fp32_100B)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_FP32, 25));
}

TEST_F(CcuStAllGather, CcuSchedAllGatherSoleMesh_2Rank_SingleDie_Int8_100B)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_INT8, 100));
}

TEST_F(CcuStAllGather, CcuSchedAllGatherSoleMesh_2Rank_SingleDie_Int32_100B)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_INT32, 25));
}

TEST_F(CcuStAllGather, CcuSchedAllGatherSoleMesh_2Rank_SingleDie_Bfp16_100B)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_BFP16, 50));
}

// ===== Message-size and empty-queue coverage =====
TEST_F(CcuStAllGather, CcuSchedAllGatherSoleMesh_2Rank_SingleDie_Int8_1B)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_INT8, 1));
}

TEST_F(CcuStAllGather, CcuSchedAllGatherSoleMesh_2Rank_SingleDie_Fp16_1KB)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_FP16, 512));
}

TEST_F(CcuStAllGather, CcuSchedAllGatherSoleMesh_2Rank_SingleDie_Fp16_1MB)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_FP16, 512ULL * 1024ULL));
}

TEST_F(CcuStAllGather, CcuSchedAllGatherSoleMesh_2Rank_SingleDie_Fp16_ZeroLength)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_FP16, 0));
}

// ===== Forced algorithm configuration =====
TEST_F(CcuStAllGather, CcuSchedAllGatherSoleMesh_ForcedAlg_2Rank_SingleDie_Fp16_100B)
{
    CcuStScenario scenario = MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_FP16, 50);
    scenario.algConfig = "CcuSchedAllGatherSoleMesh";
    VerifyScenario(scenario);
}
