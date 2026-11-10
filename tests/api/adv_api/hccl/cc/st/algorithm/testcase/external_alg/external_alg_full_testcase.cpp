/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
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

// 四态识别与解析失败契约（含裸名态：--alg 直指注册名）
TEST(ExtAlgForced, FourStateClassification)
{
    ExternalAlgSpec spec;
    std::string errMsg;

    EXPECT_EQ(ClassifyForcedAlgConfig("", spec, errMsg), ForcedAlgKind::NONE);

    EXPECT_EQ(ClassifyForcedAlgConfig("allgather=level0:fullmesh", spec, errMsg), ForcedAlgKind::LEGACY);

    // 裸算法名不再作为强制算法入口，归为 NONE
    EXPECT_EQ(ClassifyForcedAlgConfig("InsAllGatherMesh1D", spec, errMsg), ForcedAlgKind::NONE);

    EXPECT_EQ(ClassifyForcedAlgConfig("parallel[mesh, nhr]", spec, errMsg), ForcedAlgKind::EXTERNAL);
    EXPECT_TRUE(errMsg.empty());
    EXPECT_EQ(spec.canonical(), "parallel[mesh,nhr]");

    // 解析失败: spec 复位为默认值 + errMsg 非空（ClassifyForcedAlgConfig 契约）。
    // 默认值 canonical() 是 "sole[]"（executor 默认 SOLE）而非空串，
    // 失败态断言用默认值判据: layers 空 + orderStrong 复位
    EXPECT_EQ(ClassifyForcedAlgConfig("sole[ring]", spec, errMsg), ForcedAlgKind::EXTERNAL);
    EXPECT_FALSE(errMsg.empty());
    EXPECT_TRUE(spec.layers.empty());
    EXPECT_FALSE(spec.orderStrong);
}

// 失败路径三要素：报错必须含 algConfig 原文/候选或定名/原因（语法错与候选为空/规则全灭分叉）。
TEST(ExtAlgFailure, ErrorMessagesCarryThreeElements)
{
    // 解析层 errMsg 三要素：解析失败消息含原文
    ExternalAlgSpec spec;
    std::string errMsg;
    EXPECT_EQ(ClassifyForcedAlgConfig("sole[ring]", spec, errMsg), ForcedAlgKind::EXTERNAL);
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
