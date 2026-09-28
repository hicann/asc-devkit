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

// ============================================================================
// CcuSchedReduceScatterSoleNHRMultiLink（KFC NHR MultiJetty，portNum=1）
// ============================================================================

namespace {

CcuStScenario MakeNhrMultiLinkScenario(
    const TopoMeta& topoMeta, HcclDataType dataType, uint64_t count, HcclReduceOp reduceType = HCCL_REDUCE_SUM)
{
    const uint32_t rankSize = CcuStFixture::CountRanks(topoMeta);
    const uint64_t typeSize = DATATYPE_SIZE_TABLE[dataType];
    const uint64_t sliceSize = count * typeSize;
    CcuStScenario scenario;
    scenario.topoMeta = topoMeta;
    scenario.dataType = dataType;
    scenario.opType = HcclCMDType::HCCL_CMD_REDUCE_SCATTER;
    scenario.expectedAlgName = "CcuSchedReduceScatterSoleNHRMultiLink";
    scenario.algConfig = scenario.expectedAlgName;
    scenario.count = count;
    scenario.reduceType = reduceType;
    scenario.sizes.assign(rankSize, std::vector<uint64_t>(rankSize, sliceSize));
    scenario.ubxTopo = true; // NHR MultiJetty 通道计算需要 CLOS 型 L0 实例
    return scenario;
}

} // namespace

TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleNHRMultiLink_2Rank_Fp16_50Elem)
{
    VerifyScenario(MakeNhrMultiLinkScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_FP16, 50));
}

TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleNHRMultiLink_4Rank_Fp16_32Elem)
{
    VerifyScenario(MakeNhrMultiLinkScenario(TopoMeta{{{{0, 1, 2, 3}}}}, HCCL_DATA_TYPE_FP16, 32));
}

TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleNHRMultiLink_8Rank_Fp16_32Elem)
{
    VerifyScenario(MakeNhrMultiLinkScenario(TopoMeta{{{{0, 1, 2, 3, 4, 5, 6, 7}}}}, HCCL_DATA_TYPE_FP16, 32));
}

TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleNHRMultiLink_2Rank_Fp32_25Elem)
{
    VerifyScenario(MakeNhrMultiLinkScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_FP32, 25));
}

TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleNHRMultiLink_2Rank_Bfp16_50Elem)
{
    VerifyScenario(MakeNhrMultiLinkScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_BFP16, 50));
}

TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleNHRMultiLink_2Rank_Int32_25Elem)
{
    VerifyScenario(MakeNhrMultiLinkScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_INT32, 25));
}

TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleNHRMultiLink_2Rank_Fp16_512Elem)
{
    VerifyScenario(MakeNhrMultiLinkScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_FP16, 512));
}

// 大尺寸：NHR 无 chunk 循环，单 turn 全量搬运（WriteReduce 直接远端规约）
TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleNHRMultiLink_8Rank_Fp16_512KElem)
{
    VerifyScenario(MakeNhrMultiLinkScenario(TopoMeta{{{{0, 1, 2, 3, 4, 5, 6, 7}}}}, HCCL_DATA_TYPE_FP16, 512 * 1024));
}

TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleNHRMultiLink_ZeroLength)
{
    VerifyScenario(MakeNhrMultiLinkScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_FP16, 0));
}

TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleNHRMultiLink_2Rank_Fp32_Max)
{
    VerifyScenario(MakeNhrMultiLinkScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_FP32, 25, HCCL_REDUCE_MAX));
}

TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleNHRMultiLink_2Rank_Fp16_Min)
{
    VerifyScenario(MakeNhrMultiLinkScenario(TopoMeta{{{{0, 1}}}}, HCCL_DATA_TYPE_FP16, 50, HCCL_REDUCE_MIN));
}

// ============================================================================
// CcuSchedReduceScatterSoleNHR（两级拓扑 L1 NHR 中继，forced-only，双 die 自适应）
// ============================================================================

namespace {

CcuStScenario MakeSoleNhr2DieScenario(
    const TopoMeta& topoMeta, HcclDataType dataType, uint64_t count, HcclReduceOp reduceType = HCCL_REDUCE_SUM)
{
    const uint32_t rankSize = CcuStFixture::CountRanks(topoMeta);
    const uint64_t typeSize = DATATYPE_SIZE_TABLE[dataType];
    const uint64_t sliceSize = count * typeSize;
    CcuStScenario scenario;
    scenario.topoMeta = topoMeta;
    scenario.dataType = dataType;
    scenario.opType = HcclCMDType::HCCL_CMD_REDUCE_SCATTER;
    scenario.expectedAlgName = "CcuSchedReduceScatterSoleNHR";
    scenario.algConfig = scenario.expectedAlgName;
    scenario.count = count;
    scenario.reduceType = reduceType;
    scenario.sizes.assign(rankSize, std::vector<uint64_t>(rankSize, sliceSize));
    return scenario;
}

} // namespace

// 单 die 两级拓扑（2 server × 4 卡，L0=server 内 mesh，L1=跨 server NHR 中继）：
// 每 rank 对端仅 1 条 L1 链路 → dieNum=1 → 单 mission
TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleNHR_2Server8Rank_Fp16_50Elem)
{
    VerifyScenario(MakeSoleNhr2DieScenario(TopoMeta{{{{0, 1, 2, 3}}, {{4, 5, 6, 7}}}}, HCCL_DATA_TYPE_FP16, 50));
}

TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleNHR_2Server8Rank_Int32_25Elem)
{
    VerifyScenario(MakeSoleNhr2DieScenario(TopoMeta{{{{0, 1, 2, 3}}, {{4, 5, 6, 7}}}}, HCCL_DATA_TYPE_INT32, 25));
}

// goSize 三分支逐个过（(8,32K) 口径）：MemSlice / MemSlice+2B / LoopSize
TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleNHR_2Server8Rank_GoSize_MemSlice)
{
    VerifyScenario(MakeSoleNhr2DieScenario(TopoMeta{{{{0, 1, 2, 3}}, {{4, 5, 6, 7}}}}, HCCL_DATA_TYPE_FP16, 16384));
}

TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleNHR_2Server8Rank_GoSize_MemSlicePlus2B)
{
    VerifyScenario(MakeSoleNhr2DieScenario(TopoMeta{{{{0, 1, 2, 3}}, {{4, 5, 6, 7}}}}, HCCL_DATA_TYPE_FP16, 16385));
}

TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleNHR_2Server8Rank_GoSize_LoopSize)
{
    VerifyScenario(MakeSoleNhr2DieScenario(TopoMeta{{{{0, 1, 2, 3}}, {{4, 5, 6, 7}}}}, HCCL_DATA_TYPE_FP16, 131072));
}

TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleNHR_2Server8Rank_ZeroLength)
{
    VerifyScenario(MakeSoleNhr2DieScenario(TopoMeta{{{{0, 1, 2, 3}}, {{4, 5, 6, 7}}}}, HCCL_DATA_TYPE_FP16, 0));
}

TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleNHR_2Server8Rank_Fp32_Max)
{
    VerifyScenario(
        MakeSoleNhr2DieScenario(TopoMeta{{{{0, 1, 2, 3}}, {{4, 5, 6, 7}}}}, HCCL_DATA_TYPE_FP32, 25, HCCL_REDUCE_MAX));
}

// 大数据触发 die 切分（> rankSize*4 元素 → die0/die1 对半）：
// 每 rank 对端仅 1 条链路时 dieNum 仍为 1（die1 参数不被消费），数据切分路径由 AIV prepare 公式覆盖
TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleNHR_2Server8Rank_Fp16_512KElem)
{
    VerifyScenario(
        MakeSoleNhr2DieScenario(TopoMeta{{{{0, 1, 2, 3}}, {{4, 5, 6, 7}}}}, HCCL_DATA_TYPE_FP16, 512 * 1024));
}

// —— 双 die（L1 双链路 → dieNum=2 → 双 mission）：die1 数据为 0（链路待接期）空转，
// 验证双 mission 调度/channelsPerDie 分组/axisId 机制的正确性 ——
TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleNHR_2DieLink_2Server8Rank_Fp16_50Elem)
{
    CcuStScenario scenario =
        MakeSoleNhr2DieScenario(TopoMeta{{{{0, 1, 2, 3}}, {{4, 5, 6, 7}}}}, HCCL_DATA_TYPE_FP16, 50);
    scenario.twoDieLink = true;
    VerifyScenario(scenario);
}

TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleNHR_2DieLink_2Server8Rank_GoSize_LoopSize)
{
    CcuStScenario scenario =
        MakeSoleNhr2DieScenario(TopoMeta{{{{0, 1, 2, 3}}, {{4, 5, 6, 7}}}}, HCCL_DATA_TYPE_FP16, 131072);
    scenario.twoDieLink = true;
    VerifyScenario(scenario);
}

TEST_F(CcuStReduceScatter, CcuSchedReduceScatterSoleNHR_2DieLink_2Server8Rank_Fp32_Max)
{
    CcuStScenario scenario =
        MakeSoleNhr2DieScenario(TopoMeta{{{{0, 1, 2, 3}}, {{4, 5, 6, 7}}}}, HCCL_DATA_TYPE_FP32, 25, HCCL_REDUCE_MAX);
    scenario.twoDieLink = true;
    VerifyScenario(scenario);
}
