/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_reduce_scatter.h"

#include "alg_param.h"

using namespace HcclSim::CcuSt;

namespace {

// count 为 per-rank 元素数（slice大小 = count * typeSize），与KFC路径 commParam->count 口径一致
CcuStScenario MakeScenario(
    const TopoMeta& topoMeta, HcclDataType dataType, uint64_t count, HcclReduceOp reduceType = HCCL_REDUCE_SUM)
{
    const uint32_t rankSize = CcuStFixture::CountRanks(topoMeta);
    const uint64_t typeSize = DATATYPE_SIZE_TABLE[dataType];
    const uint64_t sliceSize = count * typeSize;
    CcuStScenario scenario;
    scenario.topoMeta = topoMeta;
    scenario.dataType = dataType;
    scenario.opType = HcclCMDType::HCCL_CMD_REDUCE_SCATTER;
    scenario.expectedAlgName = "CcuSchedReduceScatterSoleMesh";
    scenario.algConfig = scenario.expectedAlgName;
    scenario.count = count;
    scenario.reduceType = reduceType;
    scenario.sizes.assign(rankSize, std::vector<uint64_t>(rankSize, sliceSize));
    return scenario;
}

} // namespace

TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleMesh_2Rank_Fp16_50Elem)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_FP16, 50));
}

TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleMesh_4Rank_Fp16_32Elem)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1, 2, 3}}}}, HCCL_DATA_TYPE_FP16, 32));
}

TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleMesh_8Rank_Fp16_32Elem)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1, 2, 3, 4, 5, 6, 7}}}}, HCCL_DATA_TYPE_FP16, 32));
}

TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleMesh_2Rank_Fp32_25Elem)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_FP32, 25));
}

TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleMesh_2Rank_Bfp16_50Elem)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_BFP16, 50));
}

TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleMesh_2Rank_Int32_25Elem)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_INT32, 25));
}

// slice=4096B=MemSlice：goSize 走 n=1,p=0 分支（LoopGroup1 单整块）
TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleMesh_GoSize_MemSlice)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_FP16, 2048));
}

// slice=4098B=MemSlice+2：goSize 走 n=0,p!=0 分支（仅 residual）
TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleMesh_GoSize_MemSlicePlus2B)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_FP16, 2049));
}

// slice=65536B=LoopSize(16*4096)：goSize 走 m=1,n=0,p=0 分支（LoopGroup0 整循环）
TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleMesh_GoSize_LoopSize)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_FP16, 32768));
}

TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleMesh_ZeroLength)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_FP16, 0));
}

TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleMesh_2Rank_Fp32_Max)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_FP32, 25, HCCL_REDUCE_MAX));
}

// slice=40MB > chunkSize(2rank,32MB)：chunk 循环走 1 个满 chunk + 8MB 尾 chunk
TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleMesh_2Rank_Fp16_MultiChunk)
{
    VerifyScenario(MakeScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_FP16, 20ULL * 1024 * 1024));
}
