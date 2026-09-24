/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <cstdio>
#include <set>
#include <string>
#include <utility>

#include "alg_meta_registry.h"
#include "external_alg_parser.h"
#include "external_alg_resolver.h"
#include "external_alg_rules.h"
#include "gtest/gtest.h"
#include "hccl_alloc_ctx_res.h"

using namespace mc2_ops_hccl;

namespace {
Mc2CcTilingInner MakeTiling(const char* algConfig, uint32_t opType = 6U /*ALLGATHER*/)
{
    Mc2CcTilingInner tiling{};
    tiling.opType = opType;
    tiling.commEngine = static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED);
    (void)snprintf(tiling.algConfig, sizeof(tiling.algConfig), "%s", algConfig);
    return tiling;
}
} // namespace

// CCU 候选 ⊆ algorithmMap（三名单一致性：sidecar 注册表 / tiling 白名单 / algorithmMap）
TEST(ExtAlgFull, CcuCandidatesAreInAlgorithmMap)
{
    for (const AlgMetaRow& row : AlgMetaRegistry::Instance().GetAll()) {
        if (row.meta.engine != AlgEngine::CCU) {
            continue;
        }
        EXPECT_NE(algorithmMap.find(row.registeredName), algorithmMap.end())
            << row.registeredName << " missing in algorithmMap";
    }
}

// 四态识别与解析失败契约
TEST(ExtAlgForced, FourStateClassification)
{
    std::string algName;
    ExternalAlgSpec spec;
    std::string errMsg;

    EXPECT_EQ(ClassifyForcedAlgConfig("", algName, spec, errMsg), ForcedAlgKind::NONE);

    EXPECT_EQ(ClassifyForcedAlgConfig("allgather=level0:fullmesh", algName, spec, errMsg), ForcedAlgKind::LEGACY);

    EXPECT_EQ(ClassifyForcedAlgConfig("InsAllGatherMesh1D", algName, spec, errMsg), ForcedAlgKind::BARE_NAME);
    EXPECT_EQ(algName, "InsAllGatherMesh1D");

    EXPECT_EQ(ClassifyForcedAlgConfig("parallel[mesh, nhr]", algName, spec, errMsg), ForcedAlgKind::EXTERNAL);
    EXPECT_TRUE(errMsg.empty());
    EXPECT_EQ(spec.canonical(), "parallel[mesh,nhr]");

    // 解析失败: spec 复位为默认值 + errMsg 非空（ClassifyForcedAlgConfig 契约）。
    // 默认值 canonical() 是 "sole[]"（executor 默认 SOLE）而非空串，
    // 失败态断言用默认值判据: layers 空 + orderStrong 复位
    EXPECT_EQ(ClassifyForcedAlgConfig("sole[ring]", algName, spec, errMsg), ForcedAlgKind::EXTERNAL);
    EXPECT_FALSE(errMsg.empty());
    EXPECT_TRUE(spec.layers.empty());
    EXPECT_FALSE(spec.orderStrong);
}

// hccl_mc2.cc 内部自由函数，专属目标已编入该翻译单元，符号可见
bool CheckCcuAlgorithmsRegistered(const void* ccTilingList[], uint32_t tilingNum);

// CheckCcuAlgorithmsRegistered：外部名须有 CCU 候选且全部过双名单；AICPU-only 外部名被拒
TEST(ExtAlgGateB, ExternalNameGate)
{
    // sole[mesh] AG 有 CCU 候选 CcuSchedAllGatherSoleMesh（algorithmMap + V2 注册表均在）→ 过
    Mc2CcTilingInner agSoleMesh = MakeTiling("sole[mesh]", 6U);
    const void* goodList[] = {&agSoleMesh};
    EXPECT_TRUE(CheckCcuAlgorithmsRegistered(goodList, 1U));

    // AG pipeline[mesh,nhr] 仅 AICPU 候选 → 拒
    Mc2CcTilingInner agPipeline = MakeTiling("pipeline[mesh,nhr]", 6U);
    const void* badList[] = {&agPipeline};
    EXPECT_FALSE(CheckCcuAlgorithmsRegistered(badList, 1U));

    // 语法错 → 拒
    Mc2CcTilingInner badSyntax = MakeTiling("sole[ring]", 6U);
    const void* badSyntaxList[] = {&badSyntax};
    EXPECT_FALSE(CheckCcuAlgorithmsRegistered(badSyntaxList, 1U));

    // null tiling 守卫
    const void* nullList[] = {nullptr};
    EXPECT_FALSE(CheckCcuAlgorithmsRegistered(nullList, 1U));

    // 裸名/空名/legacy 现状语义不变
    Mc2CcTilingInner bare = MakeTiling("CcuSchedAllGatherSoleMesh", 6U);
    const void* bareList[] = {&bare};
    EXPECT_TRUE(CheckCcuAlgorithmsRegistered(bareList, 1U));
    Mc2CcTilingInner empty = MakeTiling("", 6U);
    const void* emptyList[] = {&empty};
    EXPECT_FALSE(CheckCcuAlgorithmsRegistered(emptyList, 1U));
    Mc2CcTilingInner legacy = MakeTiling("allgather=level0:fullmesh", 6U);
    const void* legacyList[] = {&legacy};
    EXPECT_FALSE(CheckCcuAlgorithmsRegistered(legacyList, 1U));
}

// 失败路径三要素：报错必须含 algConfig 原文/候选或定名/原因（语法错与无候选/规则全灭分叉）。
TEST(ExtAlgFailure, ErrorMessagesCarryThreeElements)
{
    Mc2CcTilingInner rsMeshChunk = MakeTiling("sole[mesh.chunk]", 7U); // RS: AICPU-only 候选
    const void* badList[] = {&rsMeshChunk};
    EXPECT_FALSE(CheckCcuAlgorithmsRegistered(badList, 1U));

    // 解析层 errMsg 三要素：解析失败消息含原文
    std::string algName;
    ExternalAlgSpec spec;
    std::string errMsg;
    EXPECT_EQ(ClassifyForcedAlgConfig("sole[ring]", algName, spec, errMsg), ForcedAlgKind::EXTERNAL);
    EXPECT_NE(errMsg.find("sole[ring]"), std::string::npos); // 要素1: 原文
    EXPECT_NE(errMsg.find("atom"), std::string::npos);       // 要素3: 原因（词表外原子）

    // resolver 层 errMsg 三要素：候选为空/规则全灭消息含原文与原因分叉
    ExternalAlgSpec okSpec;
    std::string parseErr;
    ASSERT_TRUE(ParseExternalAlg("pipeline[mesh,nhr,nhr]", okSpec, parseErr));
    OpParam op{};
    op.opType = HcclCMDType::HCCL_CMD_ALLGATHER;
    op.engine = CommEngine::COMM_ENGINE_AICPU_TS;
    TopoInfoWithNetLayerDetails flat{};
    std::string resolvedName;
    EXPECT_EQ(ResolveExternalAlg(op, &flat, okSpec, resolvedName, errMsg), ResolveStatus::ERROR_INVALID);
    EXPECT_NE(errMsg.find("pipeline[mesh,nhr,nhr]"), std::string::npos);                // 要素1
    EXPECT_NE(errMsg.find("no candidate registered in this build"), std::string::npos); // 要素3 分叉文案

    ASSERT_TRUE(ParseExternalAlg("parallel[mesh,nhr]", okSpec, parseErr));
    EXPECT_EQ(ResolveExternalAlg(op, &flat, okSpec, resolvedName, errMsg), ResolveStatus::ERROR_INVALID);
    EXPECT_NE(errMsg.find("all candidates rejected by rules"), std::string::npos); // 全灭分叉文案

    // [order.strong] 在 canonical 主键内：devkit 无保序注册行，带后缀输入查表无候选（而非静默忽略）
    ExternalAlgSpec strongSpec;
    ASSERT_TRUE(ParseExternalAlg("sole[mesh] [order.strong]", strongSpec, parseErr));
    EXPECT_EQ(strongSpec.canonical(), "sole[mesh][order.strong]");
    EXPECT_EQ(ResolveExternalAlg(op, &flat, strongSpec, resolvedName, errMsg), ResolveStatus::ERROR_INVALID);
    EXPECT_NE(errMsg.find("sole[mesh][order.strong]"), std::string::npos);              // 要素1: 主键含后缀
    EXPECT_NE(errMsg.find("no candidate registered in this build"), std::string::npos); // 要素3: 无候选分叉
}
