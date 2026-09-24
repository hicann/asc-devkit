/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include "alg_meta_registry.h"
#include "alg_param.h"
#include "alg_whitelist.h"
#include "auto_selector_base.h"
#include "coll_alg_v2_exec_registry.h"
#include "external_alg_parser.h"
#include "external_alg_resolver.h"
#include "external_alg_rules.h"
#include "gtest/gtest.h"

using namespace mc2_ops_hccl;

// 须在首次候选表构建前生效
const bool g_disableAlgWhitelist = [] {
    SetAlgWhitelistEnabled(false);
    return true;
}();

namespace {
// 合法外部名样本（30 个规范名的代表集 + 空格/保序变体）
// 变体条目（带空格 / [order.strong] 后缀）解析必须成功，且规范化后与对应规范名相等
const struct {
    const char* name;              // 输入（可为带空格/后缀的变体）
    const char* expectedCanonical; // 期望的规范化名（剥空格；[order.strong] 后缀收编进主键）
} LEGAL_EXTERNAL_NAMES[] = {
    {"sole[mesh]", "sole[mesh]"},
    {"sole[nhr]", "sole[nhr]"},
    {"sole[mesh.chunk]", "sole[mesh.chunk]"},
    {"sole[mesh.one_shot]", "sole[mesh.one_shot]"},
    {"sole[mesh.two_shot]", "sole[mesh.two_shot]"},
    {"sole[mesh.two_shot.chunk]", "sole[mesh.two_shot.chunk]"},
    {"sole[mesh.single_channel]", "sole[mesh.single_channel]"},
    {"sole[mesh.multi_channel]", "sole[mesh.multi_channel]"},
    {"sole[nhr.multi_channel]", "sole[nhr.multi_channel]"},
    {"sequence[mesh,nhr]", "sequence[mesh,nhr]"},
    {"sequence[mesh,mesh]", "sequence[mesh,mesh]"},
    {"parallel[mesh,nhr]", "parallel[mesh,nhr]"},
    {"parallel[mesh,nhr.multi_channel]", "parallel[mesh,nhr.multi_channel]"},
    {"concur[mesh,nhr]", "concur[mesh,nhr]"},
    {"concur[mesh,nhr.multi_channel]", "concur[mesh,nhr.multi_channel]"},
    {"concur[mesh,mesh]", "concur[mesh,mesh]"},
    {"pipeline[mesh,nhr]", "pipeline[mesh,nhr]"},
    {"parallel[mesh, nhr]", "parallel[mesh,nhr]"},
    {"sole[mesh] [order.strong]", "sole[mesh][order.strong]"},
    {"sequence[mesh, nhr, nhr]", "sequence[mesh,nhr,nhr]"},
};
} // namespace

TEST(ExtAlgParser, LegalNamesParseAndCanonicalize)
{
    for (const auto& legal : LEGAL_EXTERNAL_NAMES) {
        ExternalAlgSpec spec;
        std::string errMsg;
        ASSERT_TRUE(ParseExternalAlg(legal.name, spec, errMsg)) << "name=" << legal.name << " err=" << errMsg;
        EXPECT_EQ(spec.canonical(), std::string(legal.expectedCanonical)) << "name=" << legal.name;
    }
}

TEST(ExtAlgParser, SpacesEverywhereAreStripped)
{
    ExternalAlgSpec spec;
    std::string errMsg;
    ASSERT_TRUE(ParseExternalAlg("  parallel [ mesh , nhr . multi_channel ]  [ order.strong ] ", spec, errMsg));
    EXPECT_EQ(spec.canonical(), "parallel[mesh,nhr.multi_channel][order.strong]");
    EXPECT_TRUE(spec.orderStrong);
    EXPECT_EQ(spec.executor, ExternalExecutor::PARALLEL);
    ASSERT_EQ(spec.layers.size(), 2U);
    EXPECT_EQ(spec.layers[0].atom, "mesh");
    EXPECT_TRUE(spec.layers[0].attrs.empty());
    EXPECT_EQ(spec.layers[1].atom, "nhr");
    ASSERT_EQ(spec.layers[1].attrs.size(), 1U);
    EXPECT_EQ(spec.layers[1].attrs[0], "multi_channel");
}

TEST(ExtAlgParser, OrderStrongIsPartOfCanonical)
{
    ExternalAlgSpec spec;
    std::string errMsg;
    ASSERT_TRUE(ParseExternalAlg("sole[mesh] [order.strong]", spec, errMsg));
    EXPECT_TRUE(spec.orderStrong);
    EXPECT_EQ(spec.canonical(), "sole[mesh][order.strong]");
}

TEST(ExtAlgParser, IllegalNamesAreRejectedWithReason)
{
    struct Case {
        const char* name;
        std::string expectedErrFragment;
    };
    const Case cases[] = {
        {"", "empty"},
        {"ring[mesh]", "executor"},
        {"sole[ring]", "atom"},
        {"sole[mesh.bigstep]", "attribute"},
        {"sole[mesh.muti_channel]", "attribute"}, // 需求文档笔误的旧拼写
        {"sole[mesh", "missing"},
        {"sole[]", "layer"},
        {"sole[mesh,]", "layer"},
        {"sole[mesh,nhr,nhr,nhr]", "layer"},
        {"sole[mesh] [order.weak]", "suffix"},
        {"parallel[mesh,nhr]]", "suffix"},
        {"sole[mesh.two_shot.chunk.chunk]", ""},
        {"sole[mesh.]", "empty token"}, // 点号结尾
        {"[mesh]", "executor"},         // 空 executor
        {"sole[,,]", "empty layer"},    // 重复逗号
        {"\t", "empty"},                // 纯空白
    };
    for (const Case& c : cases) {
        ExternalAlgSpec spec;
        std::string errMsg;
        const bool ok = ParseExternalAlg(c.name, spec, errMsg);
        if (c.expectedErrFragment.empty()) {
            EXPECT_TRUE(ok) << "name=" << c.name << " err=" << errMsg;
        } else {
            EXPECT_FALSE(ok) << "name=" << c.name;
            EXPECT_NE(errMsg.find(c.expectedErrFragment), std::string::npos) << "name=" << c.name << " err=" << errMsg;
        }
    }
}

TEST(ExtAlgClassify, FourStates)
{
    ExternalAlgSpec spec;
    std::string bareName;
    std::string errMsg;

    EXPECT_EQ(ClassifyForcedAlgConfig("", bareName, spec, errMsg), ForcedAlgKind::NONE);
    EXPECT_EQ(ClassifyForcedAlgConfig("allgather=level0:fullmesh", bareName, spec, errMsg), ForcedAlgKind::LEGACY);
    EXPECT_EQ(ClassifyForcedAlgConfig("InsReduceScatterMesh1D", bareName, spec, errMsg), ForcedAlgKind::BARE_NAME);
    EXPECT_EQ(bareName, "InsReduceScatterMesh1D");
    EXPECT_EQ(ClassifyForcedAlgConfig("parallel[mesh, nhr]", bareName, spec, errMsg), ForcedAlgKind::EXTERNAL);
    EXPECT_TRUE(errMsg.empty());
    EXPECT_EQ(spec.canonical(), "parallel[mesh,nhr]");
    EXPECT_EQ(ClassifyForcedAlgConfig("sole[ring]", bareName, spec, errMsg), ForcedAlgKind::EXTERNAL);
    EXPECT_FALSE(errMsg.empty());
}

TEST(ExtAlgV2Registry, EnumerateRegisteredTags)
{
    auto& registry = CollAlgExecRegistryV2::Instance();
    const std::vector<std::string> agTags = registry.GetRegisteredTags(HcclCMDType::HCCL_CMD_ALLGATHER);
    // 本目标全量编译（-DMC2_CLIENT_ENABLE_CCU=1，无 AICPU_COMPILE 守卫裁剪），AG 应为 12 条：
    // 9 条 AICPU 注册（含 omnipipe）+ 3 条 CCU 注册（CcuSchedAllGatherSoleMesh /
    // CcuSchedAllGatherParallelMeshNHRMultiLink / CcuSchedAllGatherConcurMeshNHRMultiLink）；
    // 此断言同时防止枚举接口漏实现
    EXPECT_FALSE(agTags.empty());
    for (const std::string& tag : agTags) {
        EXPECT_TRUE(registry.IsRegistered(HcclCMDType::HCCL_CMD_ALLGATHER, tag)) << tag;
    }
    // kfc_server/executor 未编入本目标且 AICPU_COMPILE/HCCL_CANN_COMPAT_850 均未定义，
    // 若意外编入将注册 CcuKfcServer——此断言是 GLOB 排除防线的测试侧副本
    EXPECT_TRUE(registry.GetRegisteredTags(HcclCMDType::HCCL_CMD_KFC_SERVER).empty());
}

TEST(ExtAlgMetaRegistry, RegisterAndQuery)
{
    // 用本目标已注册的 sidecar 行验证
    const AlgMeta* meta =
        AlgMetaRegistry::Instance().Get(HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "InsReduceScatterMesh1D");
    ASSERT_NE(meta, nullptr);
    EXPECT_EQ(meta->engine, AlgEngine::AICPU);
    EXPECT_EQ(std::string(meta->externalName), "sole[mesh]");
}

namespace {
const HcclCMDType COLL_CMDS[] = {
    HcclCMDType::HCCL_CMD_ALLGATHER, HcclCMDType::HCCL_CMD_REDUCE_SCATTER, HcclCMDType::HCCL_CMD_ALLREDUCE,
    HcclCMDType::HCCL_CMD_ALLTOALL,  HcclCMDType::HCCL_CMD_ALLTOALLV,
};
} // namespace

// 不变量：V2 注册表 ↔ AlgMetaRegistry 双向满射。本目标全量编译所有注册现场
// （15 个 executor 文件，-DMC2_CLIENT_ENABLE_CCU=1 且无 AICPU_COMPILE 守卫裁剪），可见全部
// 46 行 sidecar——按 cmd 分布：AG 12 / RS 16 / AR 8 / A2A 7 / A2AV 3。
TEST(ExtAlgSurjectivity, V2AndMetaRegistryAreBijective)
{
    for (HcclCMDType cmd : COLL_CMDS) {
        std::vector<std::string> v2Tags = CollAlgExecRegistryV2::Instance().GetRegisteredTags(cmd);
        std::vector<std::string> metaNames;
        for (const AlgMetaRow& row : AlgMetaRegistry::Instance().GetAll()) {
            if (row.cmd == cmd) {
                metaNames.push_back(row.registeredName);
            }
        }
        std::sort(v2Tags.begin(), v2Tags.end());
        std::sort(metaNames.begin(), metaNames.end());
        EXPECT_EQ(v2Tags, metaNames) << "cmd=" << static_cast<u32>(cmd);
    }
    // 各 cmd 精确分布锚（AG 12 / RS 16 / AR 8 / A2A 7 / A2AV 3 = 46），失败时定位更直接
    EXPECT_EQ(CollAlgExecRegistryV2::Instance().GetRegisteredTags(HcclCMDType::HCCL_CMD_ALLGATHER).size(), 12U);
    EXPECT_EQ(CollAlgExecRegistryV2::Instance().GetRegisteredTags(HcclCMDType::HCCL_CMD_REDUCE_SCATTER).size(), 16U);
    EXPECT_EQ(CollAlgExecRegistryV2::Instance().GetRegisteredTags(HcclCMDType::HCCL_CMD_ALLREDUCE).size(), 9U);
    EXPECT_EQ(CollAlgExecRegistryV2::Instance().GetRegisteredTags(HcclCMDType::HCCL_CMD_ALLTOALL).size(), 7U);
    EXPECT_EQ(CollAlgExecRegistryV2::Instance().GetRegisteredTags(HcclCMDType::HCCL_CMD_ALLTOALLV).size(), 3U);
    // 本目标全量编译（MC2_CLIENT_ENABLE_CCU=1，无 AICPU_COMPILE）：46 行精确锚
    size_t total = 0;
    for (HcclCMDType cmd : COLL_CMDS) {
        for (const AlgMetaRow& row : AlgMetaRegistry::Instance().GetAll()) {
            if (row.cmd == cmd) {
                total++;
            }
        }
    }
    EXPECT_EQ(total, 47U);
}

// 不变量：注册行 externalName 字面量必须自身就是规范形（"与 parser 产出同构"的机器契约）。
// 手写字面量若带空格/词表外词/非规范拼写，将静默变死行——用户输入经 canonical() 规范化后永不相等，
// 46 计数与 registeredName 满射断言均无法发现。往返自检：parse(字面量) 成功且 canonical() == 字面量。
TEST(ExtAlgSurjectivity, ExternalNameLiteralsAreCanonical)
{
    const std::vector<AlgMetaRow> rows = AlgMetaRegistry::Instance().GetAll();
    ASSERT_FALSE(rows.empty()); // 全量编译目标：46 行产品 sidecar + 测试夹具
    for (const AlgMetaRow& row : rows) {
        ExternalAlgSpec spec;
        std::string errMsg;
        EXPECT_TRUE(ParseExternalAlg(row.meta.externalName, spec, errMsg))
            << "registeredName=" << row.registeredName << " externalName=\"" << row.meta.externalName
            << "\" err=" << errMsg;
        EXPECT_EQ(spec.canonical(), std::string(row.meta.externalName))
            << "registeredName=" << row.registeredName << "（字面量非规范形，将成为死行）";
    }
}

namespace {
// ---- topo/opParam 夹具 ----

TopoInfoWithNetLayerDetails MakeFlatMesh1DTopo(u32 rankSize = 8U)
{
    TopoInfoWithNetLayerDetails topo{};
    topo.userRank = 0U;
    topo.userRankSize = rankSize;
    topo.serverNum = 1U;
    topo.topoLevelNums = 1U;
    topo.level0Topo = Level0Shape::MESH_1D;
    topo.netLayerDetails.netLayerNum = 1U;
    topo.netLayerDetails.netLayers = {0U};
    topo.netLayerDetails.localNetInsSizeOfLayer = {1U};
    topo.topoInstDetailsOfLayer.resize(1U);
    topo.topoInstDetailsOfLayer[0].rankNumForTopoType[COMM_TOPO_1DMESH] = {rankSize};
    return topo;
}

TopoInfoWithNetLayerDetails MakeUbxTopo(u32 rankSize = 16U)
{
    TopoInfoWithNetLayerDetails topo{};
    topo.userRank = 0U;
    topo.userRankSize = rankSize;
    topo.serverNum = 1U;
    topo.topoLevelNums = 1U;
    topo.level0Topo = Level0Shape::MESH_1D_CLOS; // UBX: MESH_1D_CLOS 且非 pcieMix
    topo.level0PcieMix = false;
    topo.netLayerDetails.netLayerNum = 1U;
    topo.netLayerDetails.netLayers = {0U};
    // 真实 MESH_1D_CLOS 不变量：localNetInsSizeOfLayer[0] == CLOS rank 数（topo_host.cc
    // CalcLevel0TopoShape 校验），mesh 只覆盖 8 卡 < CLOS 域
    topo.netLayerDetails.localNetInsSizeOfLayer = {rankSize};
    topo.topoInstDetailsOfLayer.resize(1U);
    topo.topoInstDetailsOfLayer[0].rankNumForTopoType[COMM_TOPO_1DMESH] = {8U};
    topo.topoInstDetailsOfLayer[0].rankNumForTopoType[COMM_TOPO_CLOS] = {rankSize};
    return topo;
}

TopoInfoWithNetLayerDetails MakePcieMixTopo(u32 rankSize = 16U)
{
    TopoInfoWithNetLayerDetails topo = MakeUbxTopo(rankSize);
    topo.level0PcieMix = true;
    // level0 非全互联夹具（IsLayerAllConnetedWithTopo(0, 1DMESH) == false）。该函数比较
    // rankNumForTopoType[1DMESH] 与 localNetInsSizeOfLayer[0]（不是 rankSize），真实
    // MESH_1D_CLOS 不变量为 localNetInsSizeOfLayer[0] == CLOS rank 数（topo_host.cc
    // CalcLevel0TopoShape 校验），mesh 只覆盖 8 卡 < CLOS 域 16 卡即需跨 pcie 链路
    topo.netLayerDetails.localNetInsSizeOfLayer = {rankSize};
    return topo;
}

TopoInfoWithNetLayerDetails MakeMultiLevelTopo(u32 rankSize = 16U)
{
    TopoInfoWithNetLayerDetails topo = MakeFlatMesh1DTopo(rankSize);
    topo.topoLevelNums = 2U;
    return topo;
}

// 两层网最小夹具（IsTwoLevelNetLayer 判定: netLayerNum>1 / level1 有 COMM_TOPO_CLOS / localNetInsSizeOfLayer[0]>1）
TopoInfoWithNetLayerDetails MakeTwoLevelNetLayerTopo(u32 rankSize = 16U)
{
    TopoInfoWithNetLayerDetails topo{};
    topo.userRank = 0U;
    topo.userRankSize = rankSize;
    topo.topoLevelNums = 2U;
    topo.level0Topo = Level0Shape::MESH_1D;
    topo.netLayerDetails.netLayerNum = 2U;
    topo.netLayerDetails.netLayers = {0U, 1U};
    topo.netLayerDetails.localNetInsSizeOfLayer = {2U, 1U}; // level0 实例数>1
    topo.topoInstDetailsOfLayer.resize(2U);
    topo.topoInstDetailsOfLayer[1].rankNumForTopoType[COMM_TOPO_CLOS] = {rankSize}; // level1 有 CLOS
    return topo;
}

OpParam MakeOpParam(
    HcclDataType dtype = HCCL_DATA_TYPE_FP32, u64 count = 1U, HcclReduceOp reduce = HcclReduceOp::HCCL_REDUCE_SUM)
{
    OpParam op{};
    op.opType = HcclCMDType::HCCL_CMD_REDUCE_SCATTER;
    op.DataDes.dataType = dtype;
    op.DataDes.count = count;
    op.reduceType = reduce;
    return op;
}
} // namespace

TEST(ExtAlgRules, CondZAxisAlignsWithHccl)
{
    // IsTwoLevelNetLayer && (2P 且 dataSize>=4MB 或 dataSize*rankSize>1536MB)
    // dataSize = count × dtypeSize，FP32 为 4B/元素
    TopoInfoWithNetLayerDetails twoLevel = MakeTwoLevelNetLayerTopo();
    TopoInfoWithNetLayerDetails flat = MakeFlatMesh1DTopo();

    EXPECT_FALSE(CondZAxis(&flat, MakeOpParam(HCCL_DATA_TYPE_FP32, 100U * 1024 * 1024))); // 非两层网
    EXPECT_FALSE(CondZAxis(&twoLevel, MakeOpParam(HCCL_DATA_TYPE_FP32, 1024U)));          // 两层网小数据
    // 2P + 4MB 边界：1M FP32 元素恰为 4MB
    TopoInfoWithNetLayerDetails twoRank = MakeTwoLevelNetLayerTopo(2U);
    EXPECT_TRUE(CondZAxis(&twoRank, MakeOpParam(HCCL_DATA_TYPE_FP32, 1024U * 1024)));
    EXPECT_FALSE(CondZAxis(&twoRank, MakeOpParam(HCCL_DATA_TYPE_FP32, 1024U * 1024 - 1)));
    // 多 rank + 总量 1536MB 边界：
    // 16 rank 时需 dataSize > 96MB；24M FP32 元素 = 96MB，×16 = 1536MB 恰不超阈值
    EXPECT_FALSE(CondZAxis(&twoLevel, MakeOpParam(HCCL_DATA_TYPE_FP32, 24U * 1024 * 1024)));
    EXPECT_TRUE(CondZAxis(&twoLevel, MakeOpParam(HCCL_DATA_TYPE_FP32, 24U * 1024 * 1024 + 1)));
}

TEST(ExtAlgRules, CondSpecialDtAlignsWithHccl)
{
    TopoInfoWithNetLayerDetails flat = MakeFlatMesh1DTopo();
    EXPECT_TRUE(CondSpecialDt(&flat, MakeOpParam(HCCL_DATA_TYPE_INT64)));
    EXPECT_TRUE(CondSpecialDt(&flat, MakeOpParam(HCCL_DATA_TYPE_UINT64)));
    EXPECT_TRUE(CondSpecialDt(&flat, MakeOpParam(HCCL_DATA_TYPE_FP64)));
    EXPECT_TRUE(CondSpecialDt(&flat, MakeOpParam(HCCL_DATA_TYPE_FP32, 1U, HcclReduceOp::HCCL_REDUCE_PROD)));
    EXPECT_FALSE(CondSpecialDt(&flat, MakeOpParam(HCCL_DATA_TYPE_FP32)));
}

TEST(ExtAlgRules, CondPeerOnlySemantics)
{
    TopoInfoWithNetLayerDetails twoRank = MakeFlatMesh1DTopo(2U);
    TopoInfoWithNetLayerDetails eightRank = MakeFlatMesh1DTopo(8U);
    OpParam op = MakeOpParam();                 // hcclComm 为空 -> 查询前提不满足，保守不命中
    EXPECT_FALSE(CondPeerOnly(&eightRank, op)); // rankSize!=2 直接 false
    EXPECT_FALSE(CondPeerOnly(&twoRank, op));   // 无 comm 环境保守 false（真实链路走 ST）
}

TEST(ExtAlgRules, MachineFlagsAreMutuallyExclusive)
{
    TopoInfoWithNetLayerDetails flat = MakeFlatMesh1DTopo();
    TopoInfoWithNetLayerDetails ubx = MakeUbxTopo();
    TopoInfoWithNetLayerDetails pcie = MakePcieMixTopo();
    TopoInfoWithNetLayerDetails multi = MakeMultiLevelTopo();

    EXPECT_FALSE(FlagPcieMix(&flat));
    EXPECT_FALSE(FlagUbx(&flat));
    EXPECT_FALSE(FlagMultiLevel(&flat));
    EXPECT_TRUE(FlagFlat1D(&flat));

    EXPECT_TRUE(FlagUbx(&ubx));
    EXPECT_FALSE(FlagPcieMix(&ubx));
    EXPECT_FALSE(FlagFlat1D(&ubx));

    EXPECT_TRUE(FlagPcieMix(&pcie));
    EXPECT_FALSE(FlagUbx(&pcie));

    EXPECT_TRUE(FlagMultiLevel(&multi));
    EXPECT_FALSE(FlagUbx(&multi));
    EXPECT_TRUE(FlagFlat1D(&multi));
}

TEST(ExtAlgRules, EngineMatchMatrix)
{
    OpParam op = MakeOpParam();
    op.engine = CommEngine::COMM_ENGINE_CPU; // DPU 行仅 CPU 引擎命中
    EXPECT_TRUE(EngineMatch(AlgEngine::DPU, op));
    EXPECT_FALSE(EngineMatch(AlgEngine::AICPU, op));
    EXPECT_FALSE(EngineMatch(AlgEngine::CCU, op));

    op.engine = CommEngine::COMM_ENGINE_AICPU_TS;
    EXPECT_TRUE(EngineMatch(AlgEngine::AICPU, op));
    EXPECT_FALSE(EngineMatch(AlgEngine::DPU, op));
    op.engine = CommEngine::COMM_ENGINE_AICPU;
    EXPECT_TRUE(EngineMatch(AlgEngine::AICPU, op));

    op.engine = CommEngine::COMM_ENGINE_CCU;
    EXPECT_TRUE(EngineMatch(AlgEngine::CCU, op));
    EXPECT_FALSE(EngineMatch(AlgEngine::AICPU, op));
}

namespace {
OpParam MakeResolveParam(HcclCMDType cmd, CommEngine engine)
{
    OpParam op = MakeOpParam();
    op.opType = cmd;
    op.engine = engine;
    return op;
}
} // namespace

// golden：36 个 (引擎×op×外部名) 组合的定名全覆盖，期望值与注册现场 sidecar 表逐行核对
// （grep REGISTER_ALG_META 可复核）
TEST(ExtAlgResolver, GoldenAllEngines)
{
    struct Case {
        HcclCMDType cmd;
        const char* extName;
        const TopoInfoWithNetLayerDetails* topo;
        CommEngine engine;
        const char* expected;
    };
    TopoInfoWithNetLayerDetails flat = MakeFlatMesh1DTopo();
    TopoInfoWithNetLayerDetails ubx = MakeUbxTopo();
    TopoInfoWithNetLayerDetails pcie = MakePcieMixTopo();
    TopoInfoWithNetLayerDetails multi = MakeMultiLevelTopo();
    TopoInfoWithNetLayerDetails twoLevel = MakeTwoLevelNetLayerTopo();

    const Case cases[] = {
        // ---- 纯查表组合（AICPU/DPU）----
        {HcclCMDType::HCCL_CMD_ALLGATHER, "sole[mesh]", &flat, CommEngine::COMM_ENGINE_AICPU_TS, "InsAllGatherMesh1D"},
        {HcclCMDType::HCCL_CMD_ALLGATHER, "sole[nhr]", &flat, CommEngine::COMM_ENGINE_AICPU_TS,
         "AicpuAllGatherSoleNHR"},
        {HcclCMDType::HCCL_CMD_ALLGATHER, "parallel[mesh,nhr.multi_channel]", &ubx, CommEngine::COMM_ENGINE_AICPU_TS,
         "InsAllGatherParallelMesh1DNHRMultiJetty"},
        {HcclCMDType::HCCL_CMD_ALLGATHER, "concur[mesh,nhr]", &ubx, CommEngine::COMM_ENGINE_AICPU_TS,
         "AicpuAllGatherConcurMeshNHR"},
        {HcclCMDType::HCCL_CMD_ALLGATHER, "pipeline[mesh,nhr]", &pcie, CommEngine::COMM_ENGINE_AICPU_TS,
         "AicpuAllGatherPipeLinePcie"},
        {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "sole[mesh.chunk]", &flat, CommEngine::COMM_ENGINE_AICPU_TS,
         "InsReduceScatterMesh1DMeshChunk"},
        {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "sequence[mesh,nhr]", &multi, CommEngine::COMM_ENGINE_AICPU_TS,
         "InsReduceScatterSequenceMesh1DNhr"},
        {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "concur[mesh,nhr]", &ubx, CommEngine::COMM_ENGINE_AICPU_TS,
         "AicpuReduceScatterConcurMeshNHR"},
        {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "pipeline[mesh,nhr]", &pcie, CommEngine::COMM_ENGINE_AICPU_TS,
         "AicpuReduceScatterPipeLinePcie"},
        {HcclCMDType::HCCL_CMD_ALLREDUCE, "sole[mesh.two_shot]", &flat, CommEngine::COMM_ENGINE_AICPU_TS,
         "AicpuAllReduceSoleMeshTwoShot"},
        {HcclCMDType::HCCL_CMD_ALLREDUCE, "sole[mesh.one_shot]", &flat, CommEngine::COMM_ENGINE_AICPU_TS,
         "AicpuAllReduceSoleMeshOneShot"},
        {HcclCMDType::HCCL_CMD_ALLREDUCE, "sole[mesh.two_shot.chunk]", &flat, CommEngine::COMM_ENGINE_AICPU_TS,
         "AicpuAllReduceSoleMeshChunkTwoShot"},
        {HcclCMDType::HCCL_CMD_ALLTOALL, "sole[mesh.single_channel]", &flat, CommEngine::COMM_ENGINE_AICPU_TS,
         "AicpuAllToAllSoleMeshSingleChannel"},
        {HcclCMDType::HCCL_CMD_ALLTOALL, "concur[mesh,mesh]", &ubx, CommEngine::COMM_ENGINE_AICPU_TS,
         "AicpuAllToAllSoleMeshConcurrent"},
        {HcclCMDType::HCCL_CMD_ALLTOALLV, "sole[mesh]", &flat, CommEngine::COMM_ENGINE_AICPU_TS,
         "AicpuAllToAllVSoleMesh"},
        {HcclCMDType::HCCL_CMD_ALLTOALLV, "concur[mesh,mesh]", &ubx, CommEngine::COMM_ENGINE_AICPU_TS,
         "AicpuAllToAllVSoleMeshConcurrent"},
        // ---- 纯查表组合（CCU）----
        {HcclCMDType::HCCL_CMD_ALLGATHER, "sole[mesh]", &flat, CommEngine::COMM_ENGINE_CCU,
         "CcuSchedAllGatherSoleMesh"},
        {HcclCMDType::HCCL_CMD_ALLGATHER, "parallel[mesh,nhr.multi_channel]", &ubx, CommEngine::COMM_ENGINE_CCU,
         "CcuSchedAllGatherParallelMeshNHRMultiLink"},
        {HcclCMDType::HCCL_CMD_ALLGATHER, "concur[mesh,nhr.multi_channel]", &ubx, CommEngine::COMM_ENGINE_CCU,
         "CcuSchedAllGatherConcurMeshNHRMultiLink"},
        {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "sole[nhr.multi_channel]", &flat, CommEngine::COMM_ENGINE_CCU,
         "CcuSchedReduceScatterSoleNHRMultiLink"},
        {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "concur[mesh,nhr.multi_channel]", &ubx, CommEngine::COMM_ENGINE_CCU,
         "CcuSchedReduceScatterConcurMeshNHRMultiLink"},
        {HcclCMDType::HCCL_CMD_ALLREDUCE, "sole[mesh]", &flat, CommEngine::COMM_ENGINE_CCU,
         "CcuSchedAllReduceSoleMesh"},
        {HcclCMDType::HCCL_CMD_ALLTOALL, "sole[mesh]", &flat, CommEngine::COMM_ENGINE_CCU, "CcuSchedAllToAllSoleMesh"},
        {HcclCMDType::HCCL_CMD_ALLTOALL, "sole[mesh.multi_channel]", &ubx, CommEngine::COMM_ENGINE_CCU,
         "CcuSchedAllToAllMesh1DMultiJetty"},
        {HcclCMDType::HCCL_CMD_ALLTOALL, "concur[mesh,mesh]", &ubx, CommEngine::COMM_ENGINE_CCU,
         "CcuSchedAllToAllSoleMeshConcurrent"},
        {HcclCMDType::HCCL_CMD_ALLTOALLV, "sole[mesh]", &flat, CommEngine::COMM_ENGINE_CCU,
         "CcuSchedAllToAllVSoleMesh"},
        // ---- 8 处一对多 ----
        // AICPU·RS·sole[mesh]：小数据走裸名（大数据触发在下一用例）
        {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "sole[mesh]", &twoLevel, CommEngine::COMM_ENGINE_AICPU_TS,
         "InsReduceScatterMesh1D"},
        // CCU·RS·sole[mesh]：UT 无真实 comm，COND_PEER_ONLY 保守不命中 -> 裸名兜底
        {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "sole[mesh]", &flat, CommEngine::COMM_ENGINE_CCU,
         "CcuSchedReduceScatterSoleMesh"},
        // AICPU·AG·parallel[mesh,nhr] 三 flag 互斥分流
        {HcclCMDType::HCCL_CMD_ALLGATHER, "parallel[mesh,nhr]", &multi, CommEngine::COMM_ENGINE_AICPU_TS,
         "InsAllGatherParallelMesh1DNHR"},
        {HcclCMDType::HCCL_CMD_ALLGATHER, "parallel[mesh,nhr]", &ubx, CommEngine::COMM_ENGINE_AICPU_TS,
         "InsAllGatherParallelMesh1DNHRUBX"},
        {HcclCMDType::HCCL_CMD_ALLGATHER, "parallel[mesh,nhr]", &pcie, CommEngine::COMM_ENGINE_AICPU_TS,
         "InsAllGatherParallelMesh1DNHRPcie"},
        // AICPU·RS·parallel[mesh,nhr]
        {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "parallel[mesh,nhr]", &multi, CommEngine::COMM_ENGINE_AICPU_TS,
         "InsReduceScatterParallelMesh1DNHR"},
        {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "parallel[mesh,nhr]", &ubx, CommEngine::COMM_ENGINE_AICPU_TS,
         "AicpuReduceScatterParallelMeshNHRUBX"},
        {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "parallel[mesh,nhr]", &pcie, CommEngine::COMM_ENGINE_AICPU_TS,
         "InsReduceScatterParallelMesh1DNHRPcie"},
        // AICPU·AR·parallel[mesh,nhr]（无 UBX 变体）
        {HcclCMDType::HCCL_CMD_ALLREDUCE, "parallel[mesh,nhr]", &multi, CommEngine::COMM_ENGINE_AICPU_TS,
         "AicpuAllReduceParallelMeshNHR"},
        {HcclCMDType::HCCL_CMD_ALLREDUCE, "parallel[mesh,nhr]", &pcie, CommEngine::COMM_ENGINE_AICPU_TS,
         "AicpuAllreduceParallelMeshNHRPcie"},
        // AICPU·A2A·sole[mesh]
        {HcclCMDType::HCCL_CMD_ALLTOALL, "sole[mesh]", &flat, CommEngine::COMM_ENGINE_AICPU_TS,
         "AicpuAllToAllSoleMesh"},
        {HcclCMDType::HCCL_CMD_ALLTOALL, "sole[mesh]", &ubx, CommEngine::COMM_ENGINE_AICPU_TS,
         "AicpuAllToAllSoleMeshUBX"},
        // ---- DPU 行只在 engine==COMM_ENGINE_CPU 命中 ----
        {HcclCMDType::HCCL_CMD_ALLGATHER, "sequence[mesh,nhr]", &multi, CommEngine::COMM_ENGINE_CPU,
         "InsAllGatherMeshNhrDPU"},
        {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "sequence[mesh,mesh]", &multi, CommEngine::COMM_ENGINE_CPU,
         "InsReduceScatterSequenceMeshMeshDPU"},
    };

    ExternalAlgSpec spec;
    std::string errMsg;
    for (const Case& c : cases) {
        ASSERT_TRUE(ParseExternalAlg(c.extName, spec, errMsg)) << c.extName;
        OpParam op = MakeResolveParam(c.cmd, c.engine);
        std::string algName;
        EXPECT_EQ(ResolveExternalAlg(op, c.topo, spec, algName, errMsg), ResolveStatus::RESOLVED)
            << "ext=" << c.extName << " engine=" << static_cast<int>(c.engine) << " err=" << errMsg;
        EXPECT_EQ(algName, std::string(c.expected)) << "ext=" << c.extName;
    }
}

TEST(ExtAlgResolver, VariantPairDisambiguation)
{
    ExternalAlgSpec spec;
    std::string errMsg;
    std::string algName;
    TopoInfoWithNetLayerDetails twoLevel = MakeTwoLevelNetLayerTopo();
    TopoInfoWithNetLayerDetails multi = MakeMultiLevelTopo();

    // 两层网 + 大数据 -> ZAxisDetour；小数据 -> 裸 Mesh1D
    ASSERT_TRUE(ParseExternalAlg("sole[mesh]", spec, errMsg));
    OpParam big = MakeResolveParam(HcclCMDType::HCCL_CMD_REDUCE_SCATTER, CommEngine::COMM_ENGINE_AICPU_TS);
    big.DataDes.dataType = HCCL_DATA_TYPE_FP32;
    big.DataDes.count = 24U * 1024 * 1024 + 1U; // 96MB+4B，x16 rank > 1536MB
    EXPECT_EQ(ResolveExternalAlg(big, &twoLevel, spec, algName, errMsg), ResolveStatus::RESOLVED);
    EXPECT_EQ(algName, "InsReduceScatterMesh1DZAxisDetour");

    OpParam small = big;
    small.DataDes.count = 1024U;
    EXPECT_EQ(ResolveExternalAlg(small, &twoLevel, spec, algName, errMsg), ResolveStatus::RESOLVED);
    EXPECT_EQ(algName, "InsReduceScatterMesh1D");

    // specialDT -> AicpuReduceNHR；否则裸 NHR
    ASSERT_TRUE(ParseExternalAlg("sole[nhr]", spec, errMsg));
    OpParam int64 = MakeResolveParam(HcclCMDType::HCCL_CMD_REDUCE_SCATTER, CommEngine::COMM_ENGINE_AICPU_TS);
    int64.DataDes.dataType = HCCL_DATA_TYPE_INT64;
    EXPECT_EQ(ResolveExternalAlg(int64, &twoLevel, spec, algName, errMsg), ResolveStatus::RESOLVED);
    EXPECT_EQ(algName, "InsReduceScatterAicpuReduceNHR");

    OpParam fp32 = MakeResolveParam(HcclCMDType::HCCL_CMD_REDUCE_SCATTER, CommEngine::COMM_ENGINE_AICPU_TS);
    EXPECT_EQ(ResolveExternalAlg(fp32, &twoLevel, spec, algName, errMsg), ResolveStatus::RESOLVED);
    EXPECT_EQ(algName, "InsReduceScatterNHR");

    // AR sole[nhr]
    OpParam arProd = MakeResolveParam(HcclCMDType::HCCL_CMD_ALLREDUCE, CommEngine::COMM_ENGINE_AICPU_TS);
    arProd.reduceType = HcclReduceOp::HCCL_REDUCE_PROD;
    EXPECT_EQ(ResolveExternalAlg(arProd, &twoLevel, spec, algName, errMsg), ResolveStatus::RESOLVED);
    EXPECT_EQ(algName, "AicpuAllReduceSoleNHRAicpuReduce");
    OpParam arSum = MakeResolveParam(HcclCMDType::HCCL_CMD_ALLREDUCE, CommEngine::COMM_ENGINE_AICPU_TS);
    EXPECT_EQ(ResolveExternalAlg(arSum, &twoLevel, spec, algName, errMsg), ResolveStatus::RESOLVED);
    EXPECT_EQ(algName, "AicpuAllReduceSoleNHR");

    // AICPU 引擎下 DPU-only 外部名 -> 候选为空 -> ERROR_INVALID
    ASSERT_TRUE(ParseExternalAlg("sequence[mesh,nhr]", spec, errMsg));
    OpParam aicpu = MakeResolveParam(HcclCMDType::HCCL_CMD_ALLGATHER, CommEngine::COMM_ENGINE_AICPU_TS);
    EXPECT_EQ(ResolveExternalAlg(aicpu, &multi, spec, algName, errMsg), ResolveStatus::ERROR_INVALID);
}

TEST(ExtAlgResolver, ErrorCasesCarryThreeElements)
{
    ExternalAlgSpec spec;
    std::string errMsg;
    std::string algName;
    OpParam op = MakeResolveParam(HcclCMDType::HCCL_CMD_ALLGATHER, CommEngine::COMM_ENGINE_AICPU_TS);

    // 候选为空（devkit 无 3 层注册）
    TopoInfoWithNetLayerDetails flat = MakeFlatMesh1DTopo();
    ASSERT_TRUE(ParseExternalAlg("pipeline[mesh,nhr,nhr]", spec, errMsg));
    EXPECT_EQ(ResolveExternalAlg(op, &flat, spec, algName, errMsg), ResolveStatus::ERROR_INVALID);
    EXPECT_NE(errMsg.find("pipeline[mesh,nhr,nhr]"), std::string::npos); // 三要素: 外部名原文

    // 规则全灭（AG parallel[mesh,nhr] 在单层 flat 拓扑：三 flag 全不命中）
    ASSERT_TRUE(ParseExternalAlg("parallel[mesh,nhr]", spec, errMsg));
    EXPECT_EQ(ResolveExternalAlg(op, &flat, spec, algName, errMsg), ResolveStatus::ERROR_INVALID);
    EXPECT_NE(errMsg.find("parallel[mesh,nhr]"), std::string::npos);
}

TEST(ExtAlgResolver, CandidateTableIsDeterministic)
{
    // 聚合确定性：同数据不同输入序（重跑 / 逆序）构建候选表，逐位一致（不依赖注册顺序）
    const std::vector<AlgMetaRow> rows = AlgMetaRegistry::Instance().GetAll();
    const std::map<ExtAlgKey, std::vector<AlgCandidate>> table1 = BuildCandidateTable(rows);
    const std::map<ExtAlgKey, std::vector<AlgCandidate>> table2 = BuildCandidateTable(rows);
    std::vector<AlgMetaRow> reversedRows = rows;
    std::reverse(reversedRows.begin(), reversedRows.end());
    const std::map<ExtAlgKey, std::vector<AlgCandidate>> table3 = BuildCandidateTable(reversedRows);
    const std::map<ExtAlgKey, std::vector<AlgCandidate>>* otherTables[] = {&table2, &table3};
    for (const auto* other : otherTables) {
        ASSERT_EQ(table1.size(), other->size());
        for (const auto& entry : table1) {
            const auto it = other->find(entry.first);
            ASSERT_NE(it, other->end());
            ASSERT_EQ(entry.second.size(), it->second.size());
            for (size_t i = 0; i < entry.second.size(); ++i) {
                EXPECT_EQ(entry.second[i].registeredName, it->second[i].registeredName);
                EXPECT_EQ(static_cast<int>(entry.second[i].engine), static_cast<int>(it->second[i].engine));
            }
        }
    }
    // 覆盖数锚（限 5 个集合通信 CMD）：30 个 (op,外部名) 键（46 行归并）；
    // 36 个 (引擎,op,外部名) 组合（键内引擎去重求和）。
    // 限定 COLL_CMDS 范围使锚不受测试内注册行（如 KFC cmd 的宏验证行）影响。
    const std::set<HcclCMDType> collCmds(std::begin(COLL_CMDS), std::end(COLL_CMDS));
    size_t keyCount = 0;
    size_t comboCount = 0;
    for (const auto& entry : table1) {
        if (collCmds.find(entry.first.first) == collCmds.end()) {
            continue; // 集合通信域外的注册（KFC/测试行）不计入锚
        }
        keyCount++;
        std::vector<AlgEngine> engines;
        for (const AlgCandidate& cand : entry.second) {
            if (std::find(engines.begin(), engines.end(), cand.engine) == engines.end()) {
                engines.push_back(cand.engine);
            }
        }
        comboCount += engines.size();
    }
    EXPECT_EQ(keyCount, 31U);
    EXPECT_EQ(comboCount, 37U);
}

TEST(ExtAlgResolver, SpecificityOrdering)
{
    // 特异性排序：特化候选(带 cond/flag)在前，裸名(双-1)在后；同特异性内字典序
    const std::vector<AlgCandidate>& cands = GetExternalCandidates(HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "sole[mesh]");
    // 本目标全量：4 候选 = ZAxisDetour(COND) / CcuSchedReduceScatterSoleMesh(裸) /
    //              CcuSchedReduceScatterSoleMeshPeerOnly(COND) / InsReduceScatterMesh1D(裸)
    ASSERT_EQ(cands.size(), 4U);
    int lastSpecificity = 2;
    for (const AlgCandidate& cand : cands) {
        const int specificity = (cand.variantCondId != COND_NONE ? 1 : 0) + (cand.machineFlagId != FLAG_NONE ? 1 : 0);
        EXPECT_LE(specificity, lastSpecificity); // 降序
        lastSpecificity = specificity;
    }
    EXPECT_EQ(cands[0].registeredName, "CcuSchedReduceScatterSoleMeshPeerOnly"); // 特化字典序在前
    EXPECT_EQ(cands[1].registeredName, "InsReduceScatterMesh1DZAxisDetour");
    EXPECT_EQ(cands[2].registeredName, "CcuSchedReduceScatterSoleMesh"); // 裸名字典序
    EXPECT_EQ(cands[3].registeredName, "InsReduceScatterMesh1D");
}

// 显式 priority 注册可用性：REGISTER_ALG_META 第 7 参落库（KFC cmd 不触碰满射断言的 5 个 COLL_CMDS）。
// externalName 须为规范形——ExternalNameLiteralsAreCanonical 往返自检覆盖所有注册行，含测试夹具
REGISTER_ALG_META(
    HcclCMDType::HCCL_CMD_KFC_SERVER, TestPrioMacro, AlgEngine::AICPU, "sole[mesh]", COND_NONE, FLAG_NONE, 7);

// 三键排序语义（specificity 降序 > priority 降序 > name 字典序）——纯函数构建器直接验证，
// 测试内构造行（KFC cmd）+ 逆序传入（同时验证顺序无关性）。
// 产品行 priority 全 0 时三键退化为双键，此用例钉住非 0 语义
TEST(ExtAlgResolver, PriorityOrdering)
{
    // 宏注册行可查且 priority 落库
    const AlgMeta* macroMeta = AlgMetaRegistry::Instance().Get(HcclCMDType::HCCL_CMD_KFC_SERVER, "TestPrioMacro");
    ASSERT_NE(macroMeta, nullptr);
    EXPECT_EQ(macroMeta->priority, 7);

    std::vector<AlgMetaRow> rows = {
        {HcclCMDType::HCCL_CMD_KFC_SERVER, "TestPrioLow", {AlgEngine::AICPU, "test[prio]", COND_NONE, FLAG_UBX, 1}},
        {HcclCMDType::HCCL_CMD_KFC_SERVER, "TestPrioHigh", {AlgEngine::AICPU, "test[prio]", COND_NONE, FLAG_UBX, 5}},
        {HcclCMDType::HCCL_CMD_KFC_SERVER, "TestPrioSameA", {AlgEngine::AICPU, "test[prio]", COND_NONE, FLAG_UBX, 5}},
        // priority=99 的裸名：特异性第一级不可被 priority 越级，仍排单挂之后
        {HcclCMDType::HCCL_CMD_KFC_SERVER, "TestBareName", {AlgEngine::AICPU, "test[prio]", COND_NONE, FLAG_NONE, 99}},
        // 双挂（cond+flag）：特异性 2 分最特化，排最前
        {HcclCMDType::HCCL_CMD_KFC_SERVER,
         "TestDoubleSlot",
         {AlgEngine::AICPU, "test[prio]", COND_SPECIAL_DT, FLAG_UBX, 0}},
    };
    std::reverse(rows.begin(), rows.end()); // 逆序传入：排序与输入序无关
    const std::map<ExtAlgKey, std::vector<AlgCandidate>> table = BuildCandidateTable(rows);
    const auto it = table.find(std::make_pair(HcclCMDType::HCCL_CMD_KFC_SERVER, std::string("test[prio]")));
    ASSERT_NE(it, table.end());
    const std::vector<AlgCandidate>& cands = it->second;
    ASSERT_EQ(cands.size(), 5U);
    // 键1 特异性：双挂(2) > 单挂(1) > 裸名(0)
    EXPECT_EQ(cands[0].registeredName, "TestDoubleSlot");
    // 键2 priority：同单挂特异性，5 > 1（大者优先）
    EXPECT_EQ(cands[1].registeredName, "TestPrioHigh");
    EXPECT_EQ(cands[1].priority, 5);
    // 键3 字典序：同特异性同 priority(5)，"TestPrioHigh" < "TestPrioSameA"
    EXPECT_EQ(cands[2].registeredName, "TestPrioSameA");
    EXPECT_EQ(cands[3].registeredName, "TestPrioLow");
    // 裸名兜底：priority=99 仍排最后（特异性不可越级）
    EXPECT_EQ(cands[4].registeredName, "TestBareName");
}

// extern 表转发初始化器的运行时绑定断言
TEST(ExtAlgResolver, RuleTableBinding)
{
    EXPECT_EQ(VARIANT_COND_TABLE[COND_Z_AXIS], &CondZAxis);
    EXPECT_EQ(VARIANT_COND_TABLE[COND_PEER_ONLY], &CondPeerOnly);
    EXPECT_EQ(VARIANT_COND_TABLE[COND_SPECIAL_DT], &CondSpecialDt);
    EXPECT_EQ(MACHINE_FLAG_TABLE[FLAG_PCIE_MIX], &FlagPcieMix);
    EXPECT_EQ(MACHINE_FLAG_TABLE[FLAG_UBX], &FlagUbx);
    EXPECT_EQ(MACHINE_FLAG_TABLE[FLAG_MULTI_LEVEL], &FlagMultiLevel);
    EXPECT_EQ(MACHINE_FLAG_TABLE[FLAG_FLAT_1D], &FlagFlat1D);
    EXPECT_STREQ(VariantCondName(COND_Z_AXIS), "COND_Z_AXIS");
    EXPECT_STREQ(MachineFlagName(FLAG_FLAT_1D), "FLAG_FLAT_1D");
    EXPECT_STREQ(VariantCondName(-1), "n/a");
}

TEST(ExtAlgWhitelist, IsAlgAllowedMatchesNameList)
{
    for (const char* name : {
             "InsAllGatherMesh1D",
             "AicpuAllGatherSoleNHR",
             "CcuSchedAllGatherSoleMesh",
             "AicpuAllGatherConcurMeshNHR",
             "CcuSchedAllGatherConcurMeshNHRMultiLink",
             "InsAllGatherParallelMesh1DNHRMultiJetty",
             "InsAllGatherParallelMesh1DNHRPcie",
             "CcuSchedAllGatherParallelMeshNHRMultiLink",
             "AicpuAllGatherPipeLinePcie",
             "InsReduceScatterMesh1D",
             "InsReduceScatterMesh1DMeshChunk",
             "InsReduceScatterNHR",
             "InsReduceScatterParallelMesh1DNHRPcie",
             "AicpuReduceScatterParallelMeshNHRUBX",
             "AicpuReduceScatterConcurMeshNHR",
             "CcuSchedReduceScatterSoleMesh",
             "CcuSchedReduceScatterSoleNHRMultiLink",
             "CcuSchedReduceScatterConcurMeshNHRMultiLink",
             "AicpuReduceScatterPipeLinePcie",
             "AicpuAllReduceSoleNHR",
             "AicpuAllReduceSoleMeshOneShot",
             "AicpuAllReduceSoleMeshTwoShot",
             "AicpuAllReduceSoleMeshChunkTwoShot",
             "AicpuAllReduceParallelMeshNHR",
             "AicpuAllReducePipeLinePcie",
             "CcuSchedAllReduceSoleMesh",
             "AicpuAllToAllSoleMesh",
             "AicpuAllToAllSoleMeshSingleChannel",
             "AicpuAllToAllSoleMeshUBX",
             "CcuSchedAllToAllSoleMesh",
             "CcuSchedAllToAllMesh1DMultiJetty",
             "AicpuAllToAllSoleMeshConcurrent",
             "AicpuAllToAllVSoleMesh",
             "AicpuAllToAllVSoleMeshConcurrent",
             "CcuSchedAllToAllSoleMeshConcurrent",
             "CcuSchedAllToAllVSoleMesh",
         }) {
        EXPECT_TRUE(IsAlgAllowed(name)) << name;
    }
    for (const char* name : {
             "InsAllGatherMeshNhrDPU",
             "InsAllGatherParallelMesh1DNHR",
             "InsAllGatherParallelMesh1DNHRUBX",
             "InsReduceScatterMesh1DZAxisDetour",
             "CcuSchedReduceScatterSoleMeshPeerOnly",
             "InsReduceScatterAicpuReduceNHR",
             "InsReduceScatterSequenceMesh1DNhr",
             "InsReduceScatterSequenceMeshMeshDPU",
             "InsReduceScatterParallelMesh1DNHR",
             "AicpuAllReduceSoleNHRAicpuReduce",
             "AicpuAllreduceParallelMeshNHRPcie",
         }) {
        EXPECT_FALSE(IsAlgAllowed(name)) << name;
    }
    EXPECT_FALSE(IsAlgAllowed("NotAnAlgorithm"));
    EXPECT_EQ(ALG_WHITELIST.size(), 36U);
}

// 改名漏改防线：白名单出现死名即红
TEST(ExtAlgWhitelist, WhitelistNamesAreRegistered)
{
    const auto& rows = AlgMetaRegistry::Instance().GetAll();
    for (const char* allowed : ALG_WHITELIST) {
        bool found = false;
        for (const AlgMetaRow& row : rows) {
            if (row.registeredName == allowed) {
                found = true;
                break;
            }
        }
        EXPECT_TRUE(found) << allowed << " 不在注册表中";
    }
}

// 生产口径映射：全量注册经 IsAlgAllowed 过滤后的计数/死键/定名期望
TEST(ExtAlgWhitelist, DifferentialProductionView)
{
    std::vector<AlgMetaRow> kept;
    for (const AlgMetaRow& row : AlgMetaRegistry::Instance().GetAll()) {
        if (IsAlgAllowed(row.registeredName)) {
            kept.push_back(row);
        }
    }
    EXPECT_EQ(kept.size(), 36U);
    const auto prodTable = BuildCandidateTable(kept);

    const std::pair<HcclCMDType, const char*> deadKeys[] = {
        {HcclCMDType::HCCL_CMD_ALLGATHER, "sequence[mesh,nhr]"},
        {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "sequence[mesh,nhr]"},
        {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "sequence[mesh,mesh]"},
    };
    for (const auto& dead : deadKeys) {
        EXPECT_EQ(prodTable.count(std::make_pair(dead.first, std::string(dead.second))), 0U) << dead.second;
    }

    struct Case {
        HcclCMDType cmd;
        const char* extName;
        const TopoInfoWithNetLayerDetails* topo;
        CommEngine engine;
        HcclDataType dtype;
        u64 count;
        const char* expected; // nullptr = 全灭回退 selector
    };
    TopoInfoWithNetLayerDetails flat = MakeFlatMesh1DTopo();
    TopoInfoWithNetLayerDetails ubx = MakeUbxTopo();
    TopoInfoWithNetLayerDetails pcie = MakePcieMixTopo();
    TopoInfoWithNetLayerDetails multi = MakeMultiLevelTopo();
    TopoInfoWithNetLayerDetails twoLevel = MakeTwoLevelNetLayerTopo();

    const Case cases[] = {
        // AllGather
        {HcclCMDType::HCCL_CMD_ALLGATHER, "sole[mesh]", &flat, CommEngine::COMM_ENGINE_AICPU_TS, HCCL_DATA_TYPE_FP32,
         1U, "InsAllGatherMesh1D"},
        {HcclCMDType::HCCL_CMD_ALLGATHER, "sole[mesh]", &flat, CommEngine::COMM_ENGINE_CCU, HCCL_DATA_TYPE_FP32, 1U,
         "CcuSchedAllGatherSoleMesh"},
        {HcclCMDType::HCCL_CMD_ALLGATHER, "sole[nhr]", &flat, CommEngine::COMM_ENGINE_AICPU_TS, HCCL_DATA_TYPE_FP32, 1U,
         "AicpuAllGatherSoleNHR"},
        // parallel[mesh,nhr] 仅剩 Pcie 候选
        {HcclCMDType::HCCL_CMD_ALLGATHER, "parallel[mesh,nhr]", &pcie, CommEngine::COMM_ENGINE_AICPU_TS,
         HCCL_DATA_TYPE_FP32, 1U, "InsAllGatherParallelMesh1DNHRPcie"},
        {HcclCMDType::HCCL_CMD_ALLGATHER, "parallel[mesh,nhr]", &ubx, CommEngine::COMM_ENGINE_AICPU_TS,
         HCCL_DATA_TYPE_FP32, 1U, nullptr},
        {HcclCMDType::HCCL_CMD_ALLGATHER, "parallel[mesh,nhr.multi_channel]", &ubx, CommEngine::COMM_ENGINE_AICPU_TS,
         HCCL_DATA_TYPE_FP32, 1U, "InsAllGatherParallelMesh1DNHRMultiJetty"},
        {HcclCMDType::HCCL_CMD_ALLGATHER, "parallel[mesh,nhr.multi_channel]", &ubx, CommEngine::COMM_ENGINE_CCU,
         HCCL_DATA_TYPE_FP32, 1U, "CcuSchedAllGatherParallelMeshNHRMultiLink"},
        {HcclCMDType::HCCL_CMD_ALLGATHER, "concur[mesh,nhr]", &ubx, CommEngine::COMM_ENGINE_AICPU_TS,
         HCCL_DATA_TYPE_FP32, 1U, "AicpuAllGatherConcurMeshNHR"},
        {HcclCMDType::HCCL_CMD_ALLGATHER, "concur[mesh,nhr.multi_channel]", &ubx, CommEngine::COMM_ENGINE_CCU,
         HCCL_DATA_TYPE_FP32, 1U, "CcuSchedAllGatherConcurMeshNHRMultiLink"},
        {HcclCMDType::HCCL_CMD_ALLGATHER, "pipeline[mesh,nhr]", &pcie, CommEngine::COMM_ENGINE_AICPU_TS,
         HCCL_DATA_TYPE_FP32, 1U, "AicpuAllGatherPipeLinePcie"},
        // ReduceScatter
        // ZAxis 档被滤：两层网大数据仍落裸名
        {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "sole[mesh]", &twoLevel, CommEngine::COMM_ENGINE_AICPU_TS,
         HCCL_DATA_TYPE_FP32, 100U * 1024 * 1024, "InsReduceScatterMesh1D"},
        {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "sole[mesh]", &flat, CommEngine::COMM_ENGINE_CCU, HCCL_DATA_TYPE_FP32,
         1U, "CcuSchedReduceScatterSoleMesh"},
        {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "sole[mesh.chunk]", &flat, CommEngine::COMM_ENGINE_AICPU_TS,
         HCCL_DATA_TYPE_FP32, 1U, "InsReduceScatterMesh1DMeshChunk"},
        // SPECIAL_DT 变体被滤：64bit 也落裸名
        {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "sole[nhr]", &flat, CommEngine::COMM_ENGINE_AICPU_TS,
         HCCL_DATA_TYPE_FP64, 1U, "InsReduceScatterNHR"},
        {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "sole[nhr.multi_channel]", &flat, CommEngine::COMM_ENGINE_CCU,
         HCCL_DATA_TYPE_FP32, 1U, "CcuSchedReduceScatterSoleNHRMultiLink"},
        // MULTI_LEVEL 被滤：多层网全灭
        {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "parallel[mesh,nhr]", &ubx, CommEngine::COMM_ENGINE_AICPU_TS,
         HCCL_DATA_TYPE_FP32, 1U, "AicpuReduceScatterParallelMeshNHRUBX"},
        {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "parallel[mesh,nhr]", &pcie, CommEngine::COMM_ENGINE_AICPU_TS,
         HCCL_DATA_TYPE_FP32, 1U, "InsReduceScatterParallelMesh1DNHRPcie"},
        {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "parallel[mesh,nhr]", &multi, CommEngine::COMM_ENGINE_AICPU_TS,
         HCCL_DATA_TYPE_FP32, 1U, nullptr},
        {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "concur[mesh,nhr]", &ubx, CommEngine::COMM_ENGINE_AICPU_TS,
         HCCL_DATA_TYPE_FP32, 1U, "AicpuReduceScatterConcurMeshNHR"},
        {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "concur[mesh,nhr.multi_channel]", &ubx, CommEngine::COMM_ENGINE_CCU,
         HCCL_DATA_TYPE_FP32, 1U, "CcuSchedReduceScatterConcurMeshNHRMultiLink"},
        {HcclCMDType::HCCL_CMD_REDUCE_SCATTER, "pipeline[mesh,nhr]", &pcie, CommEngine::COMM_ENGINE_AICPU_TS,
         HCCL_DATA_TYPE_FP32, 1U, "AicpuReduceScatterPipeLinePcie"},
        // AllReduce
        {HcclCMDType::HCCL_CMD_ALLREDUCE, "sole[mesh]", &flat, CommEngine::COMM_ENGINE_CCU, HCCL_DATA_TYPE_FP32, 1U,
         "CcuSchedAllReduceSoleMesh"},
        {HcclCMDType::HCCL_CMD_ALLREDUCE, "sole[mesh.one_shot]", &flat, CommEngine::COMM_ENGINE_AICPU_TS,
         HCCL_DATA_TYPE_FP32, 1U, "AicpuAllReduceSoleMeshOneShot"},
        {HcclCMDType::HCCL_CMD_ALLREDUCE, "sole[mesh.two_shot]", &flat, CommEngine::COMM_ENGINE_AICPU_TS,
         HCCL_DATA_TYPE_FP32, 1U, "AicpuAllReduceSoleMeshTwoShot"},
        {HcclCMDType::HCCL_CMD_ALLREDUCE, "sole[mesh.two_shot.chunk]", &flat, CommEngine::COMM_ENGINE_AICPU_TS,
         HCCL_DATA_TYPE_FP32, 1U, "AicpuAllReduceSoleMeshChunkTwoShot"},
        // AllToAll
        {HcclCMDType::HCCL_CMD_ALLTOALL, "sole[mesh]", &flat, CommEngine::COMM_ENGINE_AICPU_TS, HCCL_DATA_TYPE_FP32, 1U,
         "AicpuAllToAllSoleMesh"},
        {HcclCMDType::HCCL_CMD_ALLTOALL, "sole[mesh]", &ubx, CommEngine::COMM_ENGINE_AICPU_TS, HCCL_DATA_TYPE_FP32, 1U,
         "AicpuAllToAllSoleMeshUBX"},
        {HcclCMDType::HCCL_CMD_ALLTOALL, "sole[mesh]", &flat, CommEngine::COMM_ENGINE_CCU, HCCL_DATA_TYPE_FP32, 1U,
         "CcuSchedAllToAllSoleMesh"},
        {HcclCMDType::HCCL_CMD_ALLTOALL, "sole[mesh.single_channel]", &flat, CommEngine::COMM_ENGINE_AICPU_TS,
         HCCL_DATA_TYPE_FP32, 1U, "AicpuAllToAllSoleMeshSingleChannel"},
        {HcclCMDType::HCCL_CMD_ALLTOALL, "sole[mesh.multi_channel]", &flat, CommEngine::COMM_ENGINE_CCU,
         HCCL_DATA_TYPE_FP32, 1U, "CcuSchedAllToAllMesh1DMultiJetty"},
        {HcclCMDType::HCCL_CMD_ALLTOALL, "concur[mesh,mesh]", &ubx, CommEngine::COMM_ENGINE_AICPU_TS,
         HCCL_DATA_TYPE_FP32, 1U, "AicpuAllToAllSoleMeshConcurrent"},
        {HcclCMDType::HCCL_CMD_ALLTOALL, "concur[mesh,mesh]", &ubx, CommEngine::COMM_ENGINE_CCU, HCCL_DATA_TYPE_FP32,
         1U, "CcuSchedAllToAllSoleMeshConcurrent"},
        // AllToAllV
        {HcclCMDType::HCCL_CMD_ALLTOALLV, "sole[mesh]", &flat, CommEngine::COMM_ENGINE_AICPU_TS, HCCL_DATA_TYPE_FP32,
         1U, "AicpuAllToAllVSoleMesh"},
        {HcclCMDType::HCCL_CMD_ALLTOALLV, "sole[mesh]", &flat, CommEngine::COMM_ENGINE_CCU, HCCL_DATA_TYPE_FP32, 1U,
         "CcuSchedAllToAllVSoleMesh"},
        {HcclCMDType::HCCL_CMD_ALLTOALLV, "concur[mesh,mesh]", &ubx, CommEngine::COMM_ENGINE_AICPU_TS,
         HCCL_DATA_TYPE_FP32, 1U, "AicpuAllToAllVSoleMeshConcurrent"},
    };
    for (const Case& c : cases) {
        const auto it = prodTable.find(std::make_pair(c.cmd, std::string(c.extName)));
        ASSERT_NE(it, prodTable.end()) << c.extName;
        OpParam op = MakeResolveParam(c.cmd, c.engine);
        op.DataDes.dataType = c.dtype;
        op.DataDes.count = c.count;
        const AlgCandidate* selected = SelectCandidate(it->second, op, c.topo);
        if (c.expected == nullptr) {
            EXPECT_EQ(selected, nullptr) << c.extName;
        } else {
            ASSERT_NE(selected, nullptr) << c.extName;
            EXPECT_STREQ(selected->registeredName.c_str(), c.expected) << c.extName;
        }
    }
}
