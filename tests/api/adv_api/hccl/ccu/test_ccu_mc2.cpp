/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <gtest/gtest.h>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <cstdlib>

#include "dtype_common.h"
#include "base.h"
#include "include/adv_api/hccl/hccl_mc2.h"
#include "hccl_alloc_ctx_res.h"
#include "sim_communicator.h"
#include "sim_world.h"
#include "topo_model.h"
#include "sim_common.h"

namespace mc2_ops_hccl {
extern bool g_stubCcuAlgorithmRegistered;
extern bool g_stubCcuAlgExecNull;
extern std::string g_stubCcuAlgExecNullName;
extern std::string g_stubSelectorAlgName;
extern bool g_stubCcuAlgResUnavailable;
} // namespace mc2_ops_hccl

namespace {

static u32 g_stubRankSize = 8;
static u32 g_stubRankId = 0;
static DevType g_stubDeviceType = DevType::DEV_TYPE_950;
static std::vector<void*> g_allocatedPtrs;
static std::unordered_map<uint32_t, std::string> g_opTypeToAlgName = {
    {static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER), "CcuSchedAllGatherSoleMesh"},
    {static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLREDUCE), "CcuAllReduceMesh1D"},
    {static_cast<uint32_t>(HcclCMDType::HCCL_CMD_REDUCE_SCATTER), "CcuSchedReduceScatterSoleMesh"},
};

static void StubCleanup()
{
    for (void* p : g_allocatedPtrs) {
        free(p);
    }
    g_allocatedPtrs.clear();
    g_stubRankSize = 8;
    g_stubRankId = 0;
    g_stubDeviceType = DevType::DEV_TYPE_950;
    mc2_ops_hccl::g_stubCcuAlgorithmRegistered = true;
    mc2_ops_hccl::g_stubCcuAlgExecNull = false;
    mc2_ops_hccl::g_stubCcuAlgExecNullName.clear();
    mc2_ops_hccl::g_stubSelectorAlgName.clear();
    mc2_ops_hccl::g_stubCcuAlgResUnavailable = false;
    unsetenv("HCCL_OP_EXPANSION_MODE");
}

static void SetCommEngineEnv(uint8_t commEngine)
{
    if (commEngine == static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED)) {
        setenv("HCCL_OP_EXPANSION_MODE", "CCU_SCHED", 1);
    } else if (commEngine == COMM_ENGINE_CCU) {
        setenv("HCCL_OP_EXPANSION_MODE", "CCU_MS", 1);
    } else if (commEngine == COMM_ENGINE_AICPU) {
        setenv("HCCL_OP_EXPANSION_MODE", "AI_CPU", 1);
    } else {
        unsetenv("HCCL_OP_EXPANSION_MODE");
    }
}

static TopoMeta BuildTopoMeta(uint32_t rankSize)
{
    TopoMeta topoMeta;
    SuperPodMeta superPod;
    ServerMeta server;
    for (uint32_t i = 0; i < rankSize; ++i) {
        server.push_back(i);
    }
    superPod.push_back(server);
    topoMeta.push_back(superPod);
    return topoMeta;
}

struct Mc2TilingTestData {
    uint32_t version;
    uint32_t cnt;
    uint32_t offset[Hccl::MC2_MAX_OP_NUM];
    Mc2ServerCfg serverCfg;
    Mc2CcTilingInner ccTiling[Hccl::MC2_MAX_OP_NUM];
};

static Mc2TilingTestData BuildMc2Tiling(
    uint32_t tilingNum, uint8_t commEngine, const uint32_t opTypes[], const uint8_t srcDataTypes[])
{
    SetCommEngineEnv(commEngine);
    Mc2TilingTestData tiling{};
    tiling.version = commEngine == static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED) ? INIT_TILING_CCU_NEW_VERSION :
                                                                                      INIT_TILING_VERSION;
    tiling.cnt = tilingNum;
    uint32_t baseOffset = offsetof(Mc2TilingTestData, ccTiling);
    for (uint32_t i = 0; i < Hccl::MC2_MAX_OP_NUM; ++i) {
        tiling.offset[i] = baseOffset + i * sizeof(Mc2CcTilingInner);
    }
    for (uint32_t i = 0; i < tilingNum; ++i) {
        tiling.ccTiling[i].commEngine = commEngine;
        tiling.ccTiling[i].opType = opTypes[i];
        tiling.ccTiling[i].srcDataType = srcDataTypes ? srcDataTypes[i] : HCCL_DATA_TYPE_FP16;
        tiling.ccTiling[i].dstDataType = HCCL_DATA_TYPE_FP16;
        tiling.ccTiling[i].reduceType = HCCL_REDUCE_SUM;
        strcpy(tiling.ccTiling[i].groupName, "test_group");
        if (commEngine == static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED) &&
            opTypes[i] == static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER)) {
            strcpy(tiling.ccTiling[i].algConfig, "CcuSchedAllGatherSoleMesh");
        } else if (
            commEngine == static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED) &&
            opTypes[i] == static_cast<uint32_t>(HcclCMDType::HCCL_CMD_REDUCE_SCATTER)) {
            strcpy(tiling.ccTiling[i].algConfig, "CcuSchedReduceScatterSoleMesh");
        } else {
            strcpy(tiling.ccTiling[i].algConfig, "default");
        }
    }
    return tiling;
}

static HcclResult RunCcuSelectAlg(
    HcclComm comm, void* stream, const std::string topoTag[], const void* ccTilingList[], uint32_t tilingNum,
    OpResCtx& resCtx)
{
    Mc2InitTilingInner initTiling{};
    return CcuSelectAlg(comm, stream, topoTag, ccTilingList, tilingNum, &initTiling, resCtx);
}

class CcuMc2TestSuite : public testing::Test {
protected:
    void SetUp() override
    {
        StubCleanup();
        TopoMeta topoMeta = BuildTopoMeta(g_stubRankSize);
        HcclSim::SimWorld::Global()->Init(topoMeta, g_stubDeviceType);

        HcclResult ret = HcclSim::Sim_HcclCommInitClusterInfo(topoMeta, g_stubRankId, &comm_);
        ASSERT_EQ(ret, HCCL_SUCCESS) << "Comm init failed";
        ASSERT_NE(comm_, nullptr) << "Comm is nullptr after init";

        int dummyStream = 1;
        stream_ = &dummyStream;
    }
    void TearDown() override
    {
        if (comm_ != nullptr) {
            auto simComm = static_cast<HcclSim::SimCommunicator*>(comm_);
            delete simComm;
            comm_ = nullptr;
        }
        HcclSim::SimWorld::Global()->Deinit();
        StubCleanup();
    }
    HcclComm comm_ = nullptr;
    aclrtStream stream_ = nullptr;
};

TEST_F(CcuMc2TestSuite, AllocCcuOpResCtx_BasicFields)
{
    OpResCtx resCtx{};
    u32 rankSize = 8;
    u32 userRank = 3;
    HcclResult ret = AllocCcuOpResCtx(comm_, "test_tag", rankSize, userRank, resCtx);
    EXPECT_EQ(ret, HCCL_SUCCESS);
    EXPECT_EQ(resCtx.rankId, userRank);
    EXPECT_EQ(resCtx.rankSize, rankSize);
    EXPECT_EQ(resCtx.workSpaceSize, Hccl::MC2_WORKSPACE_SIZE);
    EXPECT_NE(resCtx.res[0], 0U);
}

TEST_F(CcuMc2TestSuite, AllocCcuOpResCtx_FourAddrsDistinct)
{
    OpResCtx resCtx{};
    EXPECT_EQ(AllocCcuOpResCtx(comm_, "tag", 4, 0, resCtx), HCCL_SUCCESS);
    EXPECT_NE(resCtx.workSpace, resCtx.xnAddr);
    EXPECT_NE(resCtx.workSpace, resCtx.ckeAddr);
    EXPECT_NE(resCtx.xnAddr, resCtx.ckeAddr);
    EXPECT_NE(resCtx.res[0], resCtx.workSpace);
    EXPECT_NE(resCtx.res[0], resCtx.xnAddr);
    EXPECT_NE(resCtx.res[0], resCtx.ckeAddr);
}

TEST_F(CcuMc2TestSuite, AllocCcuOpResCtx_ReuseExistingMemory)
{
    OpResCtx firstResCtx{};
    OpResCtx secondResCtx{};
    ASSERT_EQ(AllocCcuOpResCtx(comm_, "reuse_tag", 4, 0, firstResCtx), HCCL_SUCCESS);
    ASSERT_EQ(AllocCcuOpResCtx(comm_, "reuse_tag", 4, 0, secondResCtx), HCCL_SUCCESS);

    EXPECT_EQ(secondResCtx.workSpace, firstResCtx.workSpace);
    EXPECT_EQ(secondResCtx.res[0], firstResCtx.res[0]);
    EXPECT_EQ(secondResCtx.xnAddr, firstResCtx.xnAddr);
    EXPECT_EQ(secondResCtx.ckeAddr, firstResCtx.ckeAddr);
}

TEST_F(CcuMc2TestSuite, CcuSelectAlg_AllGather)
{
    SetCommEngineEnv(COMM_ENGINE_CCU);

    OpResCtx resCtx{};
    ASSERT_EQ(AllocCcuOpResCtx(comm_, "allgather_ctx", g_stubRankSize, g_stubRankId, resCtx), HCCL_SUCCESS);

    Mc2CcTilingInner ccTiling{};
    ccTiling.opType = static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER);
    ccTiling.commEngine = static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED);
    strcpy(ccTiling.algConfig, "CcuSchedAllGatherSoleMesh");
    const void* ccTilingList[] = {&ccTiling};
    std::string topoTag[] = {"tag0"};

    EXPECT_EQ(RunCcuSelectAlg(comm_, stream_, topoTag, ccTilingList, 1, resCtx), HCCL_SUCCESS);
    EXPECT_EQ(resCtx.opType[0], static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER));
    EXPECT_EQ(resCtx.algorithmType[0], static_cast<uint32_t>(AlgorithmType::CcuSchedAllGatherSoleMesh));
}

TEST_F(CcuMc2TestSuite, CcuSelectAlg_ReduceScatterKfcMesh1DMem2Mem)
{
    SetCommEngineEnv(static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED));

    OpResCtx resCtx{};
    ASSERT_EQ(AllocCcuOpResCtx(comm_, "reduce_scatter_ctx", g_stubRankSize, g_stubRankId, resCtx), HCCL_SUCCESS);

    Mc2CcTilingInner ccTiling{};
    ccTiling.opType = static_cast<uint32_t>(HcclCMDType::HCCL_CMD_REDUCE_SCATTER);
    ccTiling.commEngine = static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED);
    ccTiling.srcDataType = HCCL_DATA_TYPE_FP16;
    ccTiling.dstDataType = HCCL_DATA_TYPE_FP16;
    ccTiling.reduceType = HCCL_REDUCE_SUM;
    strcpy(ccTiling.algConfig, "CcuSchedReduceScatterSoleMesh");
    const void* ccTilingList[] = {&ccTiling};
    std::string topoTag[] = {"tag0"};

    EXPECT_EQ(RunCcuSelectAlg(comm_, stream_, topoTag, ccTilingList, 1, resCtx), HCCL_SUCCESS);
    EXPECT_EQ(resCtx.opType[0], static_cast<uint32_t>(HcclCMDType::HCCL_CMD_REDUCE_SCATTER));
    EXPECT_EQ(resCtx.algorithmType[0], static_cast<uint32_t>(AlgorithmType::CcuReduceScatterMeshMem2Mem1D));
}

TEST_F(CcuMc2TestSuite, CcuSelectAlg_ReduceScatterRejectsInt8)
{
    SetCommEngineEnv(static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED));

    OpResCtx resCtx{};
    Mc2CcTilingInner ccTiling{};
    ccTiling.opType = static_cast<uint32_t>(HcclCMDType::HCCL_CMD_REDUCE_SCATTER);
    ccTiling.commEngine = static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED);
    ccTiling.srcDataType = HCCL_DATA_TYPE_INT8;
    ccTiling.dstDataType = HCCL_DATA_TYPE_INT8;
    ccTiling.reduceType = HCCL_REDUCE_SUM;
    const void* ccTilingList[] = {&ccTiling};
    std::string topoTag[] = {"tag0"};

    EXPECT_EQ(RunCcuSelectAlg(comm_, stream_, topoTag, ccTilingList, 1, resCtx), HCCL_E_NOT_SUPPORT);
}

TEST_F(CcuMc2TestSuite, CcuSelectAlg_ReduceScatterRejects64BitTypes)
{
    SetCommEngineEnv(static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED));
    const std::vector<uint8_t> unsupportedTypes = {HCCL_DATA_TYPE_INT64, HCCL_DATA_TYPE_UINT64, HCCL_DATA_TYPE_FP64};

    for (const uint8_t dataType : unsupportedTypes) {
        OpResCtx resCtx{};
        Mc2CcTilingInner ccTiling{};
        ccTiling.opType = static_cast<uint32_t>(HcclCMDType::HCCL_CMD_REDUCE_SCATTER);
        ccTiling.commEngine = static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED);
        ccTiling.srcDataType = dataType;
        ccTiling.dstDataType = dataType;
        ccTiling.reduceType = HCCL_REDUCE_SUM;
        const void* ccTilingList[] = {&ccTiling};
        std::string topoTag[] = {"tag0"};

        EXPECT_EQ(RunCcuSelectAlg(comm_, stream_, topoTag, ccTilingList, 1, resCtx), HCCL_E_NOT_SUPPORT);
    }
}

TEST_F(CcuMc2TestSuite, CcuSelectAlg_ReduceScatterRejectsProd)
{
    SetCommEngineEnv(static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED));

    OpResCtx resCtx{};
    Mc2CcTilingInner ccTiling{};
    ccTiling.opType = static_cast<uint32_t>(HcclCMDType::HCCL_CMD_REDUCE_SCATTER);
    ccTiling.commEngine = static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED);
    ccTiling.srcDataType = HCCL_DATA_TYPE_FP16;
    ccTiling.dstDataType = HCCL_DATA_TYPE_FP16;
    ccTiling.reduceType = HCCL_REDUCE_PROD;
    const void* ccTilingList[] = {&ccTiling};
    std::string topoTag[] = {"tag0"};

    EXPECT_EQ(RunCcuSelectAlg(comm_, stream_, topoTag, ccTilingList, 1, resCtx), HCCL_E_NOT_SUPPORT);
}

TEST_F(CcuMc2TestSuite, CheckCcuKfcFlow_RejectsMixedOps)
{
    Mc2InitTilingInner initTiling{};
    initTiling.version = INIT_TILING_CCU_NEW_VERSION;
    initTiling.mc2HcommCnt = 2U;
    Mc2CcTilingInner allGather{};
    allGather.opType = static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER);
    allGather.commEngine = static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED);
    Mc2CcTilingInner allReduce = allGather;
    allReduce.opType = static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLREDUCE);
    const void* ccTilingList[] = {&allGather, &allReduce};

    EXPECT_EQ(CheckCcuKfcFlow(&initTiling, ccTilingList, 2U), HCCL_E_NOT_SUPPORT);
}

TEST_F(CcuMc2TestSuite, CheckCcuKfcFlow_AcceptsReduceScatter)
{
    Mc2InitTilingInner initTiling{};
    initTiling.version = INIT_TILING_CCU_NEW_VERSION;
    initTiling.mc2HcommCnt = 2U;
    Mc2CcTilingInner reduceScatter0{};
    reduceScatter0.opType = static_cast<uint32_t>(HcclCMDType::HCCL_CMD_REDUCE_SCATTER);
    reduceScatter0.commEngine = static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED);
    Mc2CcTilingInner reduceScatter1 = reduceScatter0;
    const void* ccTilingList[] = {&reduceScatter0, &reduceScatter1};

    EXPECT_EQ(CheckCcuKfcFlow(&initTiling, ccTilingList, 2U), HCCL_SUCCESS);
}

TEST_F(CcuMc2TestSuite, CheckCcuKfcFlow_RejectsAllGatherReduceScatterMix)
{
    Mc2InitTilingInner initTiling{};
    initTiling.version = INIT_TILING_CCU_NEW_VERSION;
    initTiling.mc2HcommCnt = 2U;
    Mc2CcTilingInner allGather{};
    allGather.opType = static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER);
    allGather.commEngine = static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED);
    Mc2CcTilingInner reduceScatter = allGather;
    reduceScatter.opType = static_cast<uint32_t>(HcclCMDType::HCCL_CMD_REDUCE_SCATTER);
    const void* ccTilingList[] = {&allGather, &reduceScatter};

    EXPECT_EQ(CheckCcuKfcFlow(&initTiling, ccTilingList, 2U), HCCL_E_NOT_SUPPORT);
}

TEST_F(CcuMc2TestSuite, ObtainCommEngine_AllCcu)
{
    Mc2CcTilingInner t0{}, t1{};
    t0.commEngine = static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED);
    t1.commEngine = static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED);
    const void* list[] = {&t0, &t1};
    uint8_t engine;
    EXPECT_EQ(ObtainCommEngine(list, 2, engine), HCCL_SUCCESS);
    EXPECT_EQ(engine, static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED));
}

TEST_F(CcuMc2TestSuite, ObtainCommEngine_AllAicpu)
{
    Mc2CcTilingInner t0{}, t1{};
    t0.commEngine = COMM_ENGINE_AICPU;
    t1.commEngine = COMM_ENGINE_AICPU;
    const void* list[] = {&t0, &t1};
    uint8_t engine;
    EXPECT_EQ(ObtainCommEngine(list, 2, engine), HCCL_SUCCESS);
    EXPECT_EQ(engine, static_cast<uint8_t>(COMM_ENGINE_AICPU));
}

TEST_F(CcuMc2TestSuite, ObtainCommEngine_MixedFail)
{
    Mc2CcTilingInner t0{}, t1{};
    t0.commEngine = COMM_ENGINE_CCU;
    t1.commEngine = COMM_ENGINE_AICPU;
    const void* list[] = {&t0, &t1};
    uint8_t engine;
    EXPECT_EQ(ObtainCommEngine(list, 2, engine), HCCL_E_NOT_SUPPORT);
}

TEST_F(CcuMc2TestSuite, ObtainCommEngine_UnsupportedEngine)
{
    Mc2CcTilingInner t0{};
    t0.commEngine = COMM_ENGINE_AIV;
    const void* list[] = {&t0};
    uint8_t engine;
    EXPECT_EQ(ObtainCommEngine(list, 1, engine), HCCL_E_NOT_SUPPORT);
}

TEST_F(CcuMc2TestSuite, algorithmMap_AllEntries)
{
    EXPECT_EQ(algorithmMap.at("CcuSchedAllGatherSoleMesh"), AlgorithmType::CcuSchedAllGatherSoleMesh);
    EXPECT_EQ(algorithmMap.at("CcuAllGatherMeshMem2Mem1D"), AlgorithmType::CcuAllGatherMeshMem2Mem1D);
    EXPECT_EQ(
        algorithmMap.at("CcuSchedAllGatherConcurMeshNHRMultiLink"),
        AlgorithmType::CcuSchedAllGatherConcurMeshNHRMultiLink);
    EXPECT_EQ(algorithmMap.at("CcuSchedReduceScatterSoleMesh"), AlgorithmType::CcuReduceScatterMeshMem2Mem1D);
    EXPECT_EQ(algorithmMap.at("CcuSchedAllToAllSoleMesh"), AlgorithmType::CcuSchedAllToAllSoleMesh);
    EXPECT_EQ(algorithmMap.at("CcuSchedAllToAllVSoleMesh"), AlgorithmType::CcuSchedAllToAllVSoleMesh);
    EXPECT_EQ(algorithmMap.at("CcuSchedAllReduceSoleMesh"), AlgorithmType::CcuAllReduceMeshMem2Mem1D);
    EXPECT_EQ(algorithmMap.at("CcuAllGatherMesh1DMem2Mem"), AlgorithmType::CcuAllGatherMeshMem2Mem1D);
    EXPECT_EQ(algorithmMap.at("CcuSchedAllGatherMesh1DMem2Mem"), AlgorithmType::CcuAllGatherMeshMem2Mem1D);
    EXPECT_EQ(algorithmMap.size(), 9U);
}

TEST_F(CcuMc2TestSuite, AlgorithmType_EnumValues)
{
    EXPECT_EQ(static_cast<uint32_t>(AlgorithmType::CcuAllGatherMesh1D), 0U);
    EXPECT_EQ(static_cast<uint32_t>(AlgorithmType::CcuAllGatherMeshMem2Mem1D), 1U);
    EXPECT_EQ(static_cast<uint32_t>(AlgorithmType::CcuAllGatherMesh2D), 2U);
    EXPECT_EQ(static_cast<uint32_t>(AlgorithmType::CcuSchedAllGatherConcurMeshNHRMultiLink), 3U);
    EXPECT_EQ(static_cast<uint32_t>(AlgorithmType::CcuReduceScatterMesh1D), 50U);
    EXPECT_EQ(static_cast<uint32_t>(AlgorithmType::CcuReduceScatterMeshMem2Mem1D), 51U);
    EXPECT_EQ(static_cast<uint32_t>(AlgorithmType::CcuReduceScatterMesh2D), 52U);
    EXPECT_EQ(static_cast<uint32_t>(AlgorithmType::CcuAllReduceMesh1D), 100U);
    EXPECT_EQ(static_cast<uint32_t>(AlgorithmType::CcuAllReduceMeshMem2Mem1D), 101U);
    EXPECT_EQ(static_cast<uint32_t>(AlgorithmType::CcuAllReduceMesh2DOneShot), 102U);
    EXPECT_EQ(static_cast<uint32_t>(AlgorithmType::CcuSchedAllToAllSoleMesh), 150U);
    EXPECT_EQ(static_cast<uint32_t>(AlgorithmType::CcuSchedAllToAllVSoleMesh), 151U);
    EXPECT_EQ(static_cast<uint32_t>(AlgorithmType::CcuSchedAllGatherSoleMesh), 152U);
}

TEST_F(CcuMc2TestSuite, HcclAllocComResourceByTiling_CcuPath)
{
    uint32_t opTypes[] = {static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER)};
    Mc2TilingTestData tiling = BuildMc2Tiling(1, static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED), opTypes, nullptr);
    void* opResCtx = nullptr;
    HcclResult ret = HcclAllocComResourceByTiling(comm_, stream_, &tiling, &opResCtx);
    EXPECT_EQ(ret, HCCL_SUCCESS);
    ASSERT_NE(opResCtx, nullptr);
    OpResCtx* ctx = static_cast<OpResCtx*>(opResCtx);
    EXPECT_EQ(ctx->rankId, g_stubRankId);
    EXPECT_EQ(ctx->rankSize, g_stubRankSize);
    EXPECT_EQ(ctx->workSpaceSize, Hccl::MC2_WORKSPACE_SIZE);
    EXPECT_NE(ctx->workSpace, 0U);
    EXPECT_NE(ctx->xnAddr, 0U);
    EXPECT_NE(ctx->ckeAddr, 0U);
    EXPECT_EQ(ctx->opType[0], static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER));
    EXPECT_EQ(ctx->algorithmType[0], static_cast<uint32_t>(AlgorithmType::CcuSchedAllGatherSoleMesh));
}

TEST_F(CcuMc2TestSuite, HcclAllocComResourceByTiling_CcuAlgorithmNotRegistered)
{
    uint32_t opTypes[] = {static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER)};
    Mc2TilingTestData tiling = BuildMc2Tiling(1, static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED), opTypes, nullptr);
    mc2_ops_hccl::g_stubCcuAlgorithmRegistered = false;

    void* opResCtx = nullptr;
    HcclResult ret = HcclAllocComResourceByTiling(comm_, stream_, &tiling, &opResCtx);

    EXPECT_EQ(ret, HCCL_E_ALG_NOT_SUPPORTED);
    EXPECT_EQ(opResCtx, nullptr);
}

TEST_F(CcuMc2TestSuite, HcclAllocComResourceByTiling_ReduceScatterCcuPath)
{
    uint32_t opTypes[] = {static_cast<uint32_t>(HcclCMDType::HCCL_CMD_REDUCE_SCATTER)};
    Mc2TilingTestData tiling = BuildMc2Tiling(1, static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED), opTypes, nullptr);
    void* opResCtx = nullptr;
    HcclResult ret = HcclAllocComResourceByTiling(comm_, stream_, &tiling, &opResCtx);
    EXPECT_EQ(ret, HCCL_SUCCESS);
    ASSERT_NE(opResCtx, nullptr);
    OpResCtx* ctx = static_cast<OpResCtx*>(opResCtx);
    EXPECT_NE(ctx->res[0], 0U);
    EXPECT_EQ(ctx->opType[0], static_cast<uint32_t>(HcclCMDType::HCCL_CMD_REDUCE_SCATTER));
    EXPECT_EQ(ctx->algorithmType[0], static_cast<uint32_t>(AlgorithmType::CcuReduceScatterMeshMem2Mem1D));
}

// algConfig为合法强制算法名时：先尝试强制算法（注册/层级/资源计算校验），
// 强制算法不可用或校验失败则回退到默认selector，由调用方重新选择算法得到algName。
TEST_F(CcuMc2TestSuite, CcuSelectAlg_ForcedAlgFallbackToSelector)
{
    SetCommEngineEnv(COMM_ENGINE_CCU);

    Mc2CcTilingInner ccTiling{};
    ccTiling.opType = static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER);
    ccTiling.commEngine = static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED);
    ccTiling.srcDataType = HCCL_DATA_TYPE_FP16;
    ccTiling.dstDataType = HCCL_DATA_TYPE_FP16;
    ccTiling.reduceType = HCCL_REDUCE_SUM;
    // 合法强制算法名（已注册且可校验通过）
    strcpy(ccTiling.algConfig, "CcuAllGatherMesh1DMem2Mem");

    // 场景1：强制算法执行器不可用 → CheckForcedAlgResource校验失败 → 回退到默认selector
    mc2_ops_hccl::g_stubCcuAlgExecNull = true;
    OpParam opParam{};
    ASSERT_EQ(InitOpParamByTiling(comm_, stream_, "allgather_tag", &ccTiling, opParam), HCCL_SUCCESS);
    std::string algName;
    auto topoInfo = std::make_unique<TopoInfoWithNetLayerDetails>();
    bool forcedAlgAccepted = true;
    HcclResult ret = TryForcedAlgAndPrepareEngine(comm_, &ccTiling, opParam, algName, topoInfo, forcedAlgAccepted);
    // 强制算法失败后回退，不向上报错，由调用方走默认selector重新选择
    EXPECT_EQ(ret, HCCL_SUCCESS);
    EXPECT_FALSE(forcedAlgAccepted);
    EXPECT_TRUE(algName.empty());

    // 场景2：强制算法可用 → 接受强制算法
    mc2_ops_hccl::g_stubCcuAlgExecNull = false;
    OpParam opParam2{};
    ASSERT_EQ(InitOpParamByTiling(comm_, stream_, "allgather_tag", &ccTiling, opParam2), HCCL_SUCCESS);
    std::string algName2;
    auto topoInfo2 = std::make_unique<TopoInfoWithNetLayerDetails>();
    bool forcedAlgAccepted2 = false;
    HcclResult ret2 = TryForcedAlgAndPrepareEngine(comm_, &ccTiling, opParam2, algName2, topoInfo2, forcedAlgAccepted2);
    EXPECT_EQ(ret2, HCCL_SUCCESS);
    EXPECT_TRUE(forcedAlgAccepted2);
    EXPECT_EQ(algName2, "CcuAllGatherMesh1DMem2Mem");
}

TEST_F(CcuMc2TestSuite, HcclAllocComResourceByTiling_AicpuPath)
{
    uint32_t opTypes[] = {static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER)};
    Mc2TilingTestData tiling = BuildMc2Tiling(1, COMM_ENGINE_AICPU, opTypes, nullptr);
    void* opResCtx = nullptr;
    HcclResult ret = HcclAllocComResourceByTiling(comm_, stream_, &tiling, &opResCtx);
    EXPECT_EQ(ret, HCCL_SUCCESS);
    ASSERT_NE(opResCtx, nullptr);
}

TEST_F(CcuMc2TestSuite, InitOpParamByTiling_SetsExpansionMode)
{
    Mc2CcTilingInner ccTiling{};
    ccTiling.opType = static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER);
    ccTiling.srcDataType = HCCL_DATA_TYPE_FP16;
    ccTiling.dstDataType = HCCL_DATA_TYPE_FP16;
    ccTiling.reduceType = HCCL_REDUCE_SUM;

    ccTiling.commEngine = static_cast<uint8_t>(OpExecuteConfig::AICPU_TS);
    OpParam aicpuOpParam{};
    ASSERT_EQ(InitOpParamByTiling(comm_, stream_, "aicpu_tag", &ccTiling, aicpuOpParam), HCCL_SUCCESS);
    EXPECT_EQ(aicpuOpParam.commOpExpansionMode, HcclOpExpansionMode::HCCL_OP_EXPANSION_MODE_AI_CPU);

    ccTiling.commEngine = static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED);
    OpParam ccuOpParam{};
    ASSERT_EQ(InitOpParamByTiling(comm_, stream_, "ccu_tag", &ccTiling, ccuOpParam), HCCL_SUCCESS);
    EXPECT_EQ(ccuOpParam.commOpExpansionMode, HcclOpExpansionMode::HCCL_OP_EXPANSION_CCU_SCHED);
}

TEST_F(CcuMc2TestSuite, HcclAllocComResourceByTiling_MixedOpsRejected)
{
    uint32_t opTypes[] = {
        static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER), static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLREDUCE)};
    Mc2TilingTestData tiling = BuildMc2Tiling(2, static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED), opTypes, nullptr);
    void* opResCtx = nullptr;
    EXPECT_EQ(HcclAllocComResourceByTiling(comm_, stream_, &tiling, &opResCtx), HCCL_E_ALG_NOT_SUPPORTED);
}

TEST_F(CcuMc2TestSuite, HcclAllocComResourceByTiling_NullComm)
{
    uint32_t opTypes[] = {static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER)};
    Mc2TilingTestData tiling = BuildMc2Tiling(1, COMM_ENGINE_CCU, opTypes, nullptr);
    void* opResCtx = nullptr;
    EXPECT_EQ(HcclAllocComResourceByTiling(nullptr, stream_, &tiling, &opResCtx), HCCL_E_PTR);
}

TEST_F(CcuMc2TestSuite, HcclAllocComResourceByTiling_NullStream)
{
    uint32_t opTypes[] = {static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER)};
    Mc2TilingTestData tiling = BuildMc2Tiling(1, COMM_ENGINE_CCU, opTypes, nullptr);
    void* opResCtx = nullptr;
    EXPECT_EQ(HcclAllocComResourceByTiling(comm_, nullptr, &tiling, &opResCtx), HCCL_E_PTR);
}

TEST_F(CcuMc2TestSuite, HcclAllocComResourceByTiling_NullTiling)
{
    void* opResCtx = nullptr;
    EXPECT_EQ(HcclAllocComResourceByTiling(comm_, stream_, nullptr, &opResCtx), HCCL_E_PTR);
}

TEST_F(CcuMc2TestSuite, CcuSelectAlg_AicpuAlgNotSupported)
{
    // tiling标记为CCU引擎，但环境变量设为AICPU模式，
    // 导致SelectAlgAndPrepareEngine选出AICPU算法名（如"InsAllGatherMesh"），
    // 该算法名不在CCU的algorithmMap中，CcuSelectAlg应返回HCCL_E_NOT_SUPPORT
    SetCommEngineEnv(COMM_ENGINE_AICPU);
    Mc2CcTilingInner ccTiling{};
    ccTiling.opType = static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER);
    ccTiling.commEngine = COMM_ENGINE_CCU;
    const void* ccTilingList[] = {&ccTiling};
    std::string topoTag[] = {"tag0"};
    OpResCtx resCtx{};
    EXPECT_EQ(RunCcuSelectAlg(comm_, stream_, topoTag, ccTilingList, 1, resCtx), HCCL_E_NOT_SUPPORT);
}

TEST_F(CcuMc2TestSuite, HcclAllocComResourceByTiling_UnsupportedEngine)
{
    uint32_t opTypes[] = {static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER)};
    Mc2TilingTestData tiling = BuildMc2Tiling(1, COMM_ENGINE_AIV, opTypes, nullptr);
    void* opResCtx = nullptr;
    EXPECT_EQ(HcclAllocComResourceByTiling(comm_, stream_, &tiling, &opResCtx), HCCL_E_NOT_SUPPORT);
}

TEST_F(CcuMc2TestSuite, GetCcuOpParamResCtx_AllGatherMesh1D_KfcServerArgs)
{
    SetCommEngineEnv(COMM_ENGINE_CCU);

    // 动态获取CCL buffer地址
    void* cclBufferPtr = nullptr;
    uint64_t cclBufferSize = 0;
    ASSERT_EQ(HcclGetHcclBuffer(comm_, &cclBufferPtr, &cclBufferSize), HCCL_SUCCESS);
    uint64_t cclBufferAddr = reinterpret_cast<uint64_t>(cclBufferPtr);

    // 1. 分配CCU资源，获取opResCtx（含xnAddr、ckeAddr）
    OpResCtx opResCtx{};
    ASSERT_EQ(AllocCcuOpResCtx(comm_, "ctx_tag", g_stubRankSize, g_stubRankId, opResCtx), HCCL_SUCCESS);
    ASSERT_NE(opResCtx.xnAddr, 0U);
    ASSERT_NE(opResCtx.ckeAddr, 0U);

    // 2. 构造OpParam，模拟CcuSelectAlg中的准备流程
    Mc2CcTilingInner ccTiling{};
    ccTiling.opType = static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER);
    ccTiling.commEngine = static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED);
    ccTiling.srcDataType = HCCL_DATA_TYPE_FP16;
    ccTiling.dstDataType = HCCL_DATA_TYPE_FP16;
    ccTiling.reduceType = HCCL_REDUCE_SUM;
    strcpy(ccTiling.algConfig, "CcuSchedAllGatherSoleMesh");

    OpParam opParam{};
    ASSERT_EQ(InitOpParamByTiling(comm_, stream_, "allgather_tag", &ccTiling, opParam), HCCL_SUCCESS);

    std::string algName;
    auto topoInfo = std::make_unique<TopoInfoWithNetLayerDetails>();
    ASSERT_TRUE(GetForcedAlgName(&ccTiling, algName));
    ASSERT_EQ(PrepareTopoInfoForOp(comm_, opParam, topoInfo), HCCL_SUCCESS);
    ASSERT_EQ(PrepareEngineForAlg(opParam, algName), HCCL_SUCCESS);
    ASSERT_EQ(algName, "CcuSchedAllGatherSoleMesh");
    ASSERT_EQ(opParam.engine, COMM_ENGINE_CCU);

    int result = sprintf_s(opParam.algName, sizeof(opParam.algName), "%s", algName.c_str());
    ASSERT_GT(result, 0);

    bool skipGetRes = false;
    ASSERT_EQ(HandleSingleRankAndCommMode(comm_, opParam, skipGetRes), HCCL_SUCCESS);
    ASSERT_FALSE(skipGetRes);

    // 3. 调用GetCcuOpParamResCtx
    void* resCtxOut = nullptr;
    EXPECT_EQ(GetCcuOpParamResCtx(comm_, algName, opParam, topoInfo.get(), opResCtx, &resCtxOut), HCCL_SUCCESS);
    EXPECT_NE(resCtxOut, nullptr);
    EXPECT_EQ(opParam.resCtx, resCtxOut);

    // 4. 通过反序列化resCtxOut还原resourceCtx，验证kfcServerArgs各字段
    ASSERT_NE(opParam.ctxSize, 0U);
    auto* resCtxRaw = static_cast<char*>(resCtxOut);
    std::vector<char> seq(resCtxRaw, resCtxRaw + opParam.ctxSize);
    mc2_ops_hccl::AlgResourceCtxSerializable resourceCtx;
    resourceCtx.DeSerialize(seq);

    // kfcServerArgs应包含6个uint64_t元素（xnAddr, ckeAddr, dieNum, missionNum, missionIndex, token）
    // AcquireAlgResources 预填充所有6字段（token 占位符为0）
    // GetAlgResCcu 内的 HcclAllocAlgResourceCcu 更新占位符为真实 token
    ASSERT_EQ(resourceCtx.kfcServerArgs.size(), 6U);
    EXPECT_EQ(resourceCtx.kfcServerArgSize, 6U);

    // xnAddr
    EXPECT_EQ(resourceCtx.kfcServerArgs[0], opResCtx.xnAddr);
    // ckeAddr
    EXPECT_EQ(resourceCtx.kfcServerArgs[1], opResCtx.ckeAddr);
    // dieNum = 1
    EXPECT_EQ(resourceCtx.kfcServerArgs[2], 1U);
    // missionNum = 1
    EXPECT_EQ(resourceCtx.kfcServerArgs[3], 1U);
    // missionIndex = 0
    EXPECT_EQ(resourceCtx.kfcServerArgs[4], 0U);
    // token (由GetTokenFromBuffInfo获取，stub返回基于CCL buffer地址的非零值)
    uint64_t expectedToken = cclBufferAddr + 0x1234;
    EXPECT_EQ(resourceCtx.kfcServerArgs[5], expectedToken);
}

TEST_F(CcuMc2TestSuite, GetCcuOpParamResCtx_TokenUpdateOnReuse)
{
    SetCommEngineEnv(COMM_ENGINE_CCU);

    // 动态获取CCL buffer地址
    void* cclBufferPtr = nullptr;
    uint64_t cclBufferSize = 0;
    ASSERT_EQ(HcclGetHcclBuffer(comm_, &cclBufferPtr, &cclBufferSize), HCCL_SUCCESS);
    uint64_t cclBufferAddr = reinterpret_cast<uint64_t>(cclBufferPtr);
    uint64_t expectedToken = cclBufferAddr + 0x1234;

    // 1. 第一次调用：创建资源并写入token
    OpResCtx opResCtx1{};
    ASSERT_EQ(AllocCcuOpResCtx(comm_, "ctx_tag1", g_stubRankSize, g_stubRankId, opResCtx1), HCCL_SUCCESS);

    Mc2CcTilingInner ccTiling{};
    ccTiling.opType = static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER);
    ccTiling.commEngine = static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED);
    ccTiling.srcDataType = HCCL_DATA_TYPE_FP16;
    ccTiling.dstDataType = HCCL_DATA_TYPE_FP16;
    ccTiling.reduceType = HCCL_REDUCE_SUM;
    strcpy(ccTiling.algConfig, "CcuSchedAllGatherSoleMesh");

    OpParam opParam1{};
    ASSERT_EQ(InitOpParamByTiling(comm_, stream_, "reuse_tag", &ccTiling, opParam1), HCCL_SUCCESS);

    std::string algName;
    auto topoInfo = std::make_unique<TopoInfoWithNetLayerDetails>();
    ASSERT_TRUE(GetForcedAlgName(&ccTiling, algName));
    ASSERT_EQ(PrepareTopoInfoForOp(comm_, opParam1, topoInfo), HCCL_SUCCESS);
    ASSERT_EQ(PrepareEngineForAlg(opParam1, algName), HCCL_SUCCESS);
    ASSERT_EQ(algName, "CcuSchedAllGatherSoleMesh");

    int result = sprintf_s(opParam1.algName, sizeof(opParam1.algName), "%s", algName.c_str());
    ASSERT_GT(result, 0);

    bool skipGetRes = false;
    ASSERT_EQ(HandleSingleRankAndCommMode(comm_, opParam1, skipGetRes), HCCL_SUCCESS);
    ASSERT_FALSE(skipGetRes);

    void* resCtxOut1 = nullptr;
    EXPECT_EQ(GetCcuOpParamResCtx(comm_, algName, opParam1, topoInfo.get(), opResCtx1, &resCtxOut1), HCCL_SUCCESS);
    EXPECT_NE(resCtxOut1, nullptr);

    // 验证第一次的token
    auto* ctxRaw1 = static_cast<char*>(resCtxOut1);
    std::vector<char> seq1(ctxRaw1, ctxRaw1 + opParam1.ctxSize);
    mc2_ops_hccl::AlgResourceCtxSerializable resourceCtx1;
    resourceCtx1.DeSerialize(seq1);
    EXPECT_EQ(resourceCtx1.kfcServerArgs.size(), 6U);
    EXPECT_EQ(resourceCtx1.kfcServerArgs[5], expectedToken);

    // 2. 第二次调用相同配置（相同algTag）：触发资源复用
    // 注意：必须使用相同的topo tag ("reuse_tag") 才能生成相同的algTag
    OpResCtx opResCtx2{};
    ASSERT_EQ(AllocCcuOpResCtx(comm_, "ctx_tag2", g_stubRankSize, g_stubRankId, opResCtx2), HCCL_SUCCESS);

    OpParam opParam2{};
    ASSERT_EQ(InitOpParamByTiling(comm_, stream_, "reuse_tag", &ccTiling, opParam2), HCCL_SUCCESS); // 相同tag
    ASSERT_TRUE(GetForcedAlgName(&ccTiling, algName));
    ASSERT_EQ(PrepareTopoInfoForOp(comm_, opParam2, topoInfo), HCCL_SUCCESS);
    ASSERT_EQ(PrepareEngineForAlg(opParam2, algName), HCCL_SUCCESS);
    result = sprintf_s(opParam2.algName, sizeof(opParam2.algName), "%s", algName.c_str());
    ASSERT_GT(result, 0);
    ASSERT_EQ(HandleSingleRankAndCommMode(comm_, opParam2, skipGetRes), HCCL_SUCCESS);
    ASSERT_FALSE(skipGetRes);

    void* resCtxOut2 = nullptr;
    EXPECT_EQ(GetCcuOpParamResCtx(comm_, algName, opParam2, topoInfo.get(), opResCtx2, &resCtxOut2), HCCL_SUCCESS);
    EXPECT_NE(resCtxOut2, nullptr);

    // 验证资源复用（相同的ctx地址）
    EXPECT_EQ(resCtxOut1, resCtxOut2);

    // 验证第二次的token（资源复用后仍保持正确值）
    auto* ctxRaw2 = static_cast<char*>(resCtxOut2);
    std::vector<char> seq2(ctxRaw2, ctxRaw2 + opParam2.ctxSize);
    mc2_ops_hccl::AlgResourceCtxSerializable resourceCtx2;
    resourceCtx2.DeSerialize(seq2);
    EXPECT_EQ(resourceCtx2.kfcServerArgs.size(), 6U);
    EXPECT_EQ(resourceCtx2.kfcServerArgs[5], expectedToken);
}

// ---------- CheckOpResSufficient ----------
// checkOnly预检：走真实资源链探测资源是否充足，UNAVAIL统一翻译为HCCL_E_RES_NOT_SUFFICIENT

TEST_F(CcuMc2TestSuite, CheckOpResSufficient_CcuSufficient)
{
    uint32_t opTypes[] = {static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER)};
    Mc2TilingTestData tiling = BuildMc2Tiling(1, static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED), opTypes, nullptr);

    HcclResult ret = CheckOpResSufficient(comm_, stream_, &tiling);

    EXPECT_EQ(ret, HCCL_SUCCESS);
}

// 端到端验证UNAVAIL→RES_NOT_SUFFICIENT映射：打桩HcclGetAlgRes返回UNAVAIL（模拟CCU通道/实例资源不足），
// checkOnly场景在AcquireAlgResources调用点统一翻译为HCCL_E_RES_NOT_SUFFICIENT；
// 非checkOnly场景（HcclAllocComResourceByTiling）保持UNAVAIL原样返回，不受翻译影响。
TEST_F(CcuMc2TestSuite, CheckOpResSufficient_ResourceInsufficient)
{
    uint32_t opTypes[] = {static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER)};
    Mc2TilingTestData tiling = BuildMc2Tiling(1, static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED), opTypes, nullptr);
    mc2_ops_hccl::g_stubCcuAlgResUnavailable = true;

    // checkOnly：UNAVAIL 统一翻译为 HCCL_E_RES_NOT_SUFFICIENT
    EXPECT_EQ(CheckOpResSufficient(comm_, stream_, &tiling), HCCL_E_RES_NOT_SUFFICIENT);

    // 非checkOnly：UNAVAIL 原样返回，保持与原逻辑一致
    void* opResCtx = nullptr;
    EXPECT_EQ(HcclAllocComResourceByTiling(comm_, stream_, &tiling, &opResCtx), HCCL_E_UNAVAIL);
}

TEST_F(CcuMc2TestSuite, CheckOpResSufficient_ReduceScatterCcu)
{
    // ReduceScatter 走 CCU_SCHED 校验分支：强制算法名CcuKfcReduceScatterMesh1DMem2Mem
    // 被直接接受，资源计算成功即视为校验通过
    uint32_t opTypes[] = {static_cast<uint32_t>(HcclCMDType::HCCL_CMD_REDUCE_SCATTER)};
    Mc2TilingTestData tiling = BuildMc2Tiling(1, static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED), opTypes, nullptr);

    HcclResult ret = CheckOpResSufficient(comm_, stream_, &tiling);

    EXPECT_EQ(ret, HCCL_SUCCESS);
}

// 校验路径端到端回退：algConfig为合法强制算法名，但强制算法执行器不可用
// （CheckForcedAlgResource校验失败）→ 回退到默认selector重新选择算法 → 资源校验通过。
// BuildMc2Tiling已将algConfig置为"CcuAllGatherMesh1DMem2Mem"，
// g_stubCcuAlgExecNullName限定仅该强制算法返回nullptr，不影响回退后算法的资源计算。
TEST_F(CcuMc2TestSuite, CheckOpResSufficient_ForcedAlgFallbackToSelector)
{
    uint32_t opTypes[] = {static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER)};
    Mc2TilingTestData tiling = BuildMc2Tiling(1, static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED), opTypes, nullptr);
    mc2_ops_hccl::g_stubCcuAlgExecNull = true;
    mc2_ops_hccl::g_stubCcuAlgExecNullName = "CcuAllGatherMesh1DMem2Mem";
    // 回退后默认selector选择合法CCU算法（与强制算法不同名，避免再次命中执行器不可用桩）
    mc2_ops_hccl::g_stubSelectorAlgName = "CcuAllGatherMeshMem2Mem1D";

    HcclResult ret = CheckOpResSufficient(comm_, stream_, &tiling);

    EXPECT_EQ(ret, HCCL_SUCCESS);
}

TEST_F(CcuMc2TestSuite, CheckOpResSufficient_AicpuBypass)
{
    // 非CCU_SCHED引擎不做CCU校验，直接通过
    uint32_t opTypes[] = {static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER)};
    Mc2TilingTestData tiling = BuildMc2Tiling(1, COMM_ENGINE_AICPU, opTypes, nullptr);

    HcclResult ret = CheckOpResSufficient(comm_, stream_, &tiling);

    EXPECT_EQ(ret, HCCL_SUCCESS);
}

TEST_F(CcuMc2TestSuite, CheckOpResSufficient_SingleRankBypass)
{
    // 单rank场景资源必然充足：入口层统一提前返回SUCCESS，不进入算法选择与资源校验
    // （rankSize为通信域属性，对所有tiling一致，此处对CCU_SCHED引擎验证该守卫）
    if (comm_ != nullptr) {
        auto simComm = static_cast<HcclSim::SimCommunicator*>(comm_);
        delete simComm;
        comm_ = nullptr;
    }
    HcclSim::SimWorld::Global()->Deinit();
    g_stubRankSize = 1;
    TopoMeta topoMeta = BuildTopoMeta(g_stubRankSize);
    HcclSim::SimWorld::Global()->Init(topoMeta, g_stubDeviceType);
    HcclResult initRet = HcclSim::Sim_HcclCommInitClusterInfo(topoMeta, g_stubRankId, &comm_);
    ASSERT_EQ(initRet, HCCL_SUCCESS) << "Comm init failed";

    uint32_t opTypes[] = {static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER)};
    Mc2TilingTestData tiling = BuildMc2Tiling(1, static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED), opTypes, nullptr);

    HcclResult ret = CheckOpResSufficient(comm_, stream_, &tiling);

    EXPECT_EQ(ret, HCCL_SUCCESS);
}

TEST_F(CcuMc2TestSuite, CheckOpResSufficient_AlgNotRegistered)
{
    uint32_t opTypes[] = {static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER)};
    Mc2TilingTestData tiling = BuildMc2Tiling(1, static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED), opTypes, nullptr);
    mc2_ops_hccl::g_stubCcuAlgorithmRegistered = false;

    HcclResult ret = CheckOpResSufficient(comm_, stream_, &tiling);

    EXPECT_EQ(ret, HCCL_E_ALG_NOT_SUPPORTED);
}

TEST_F(CcuMc2TestSuite, CheckOpResSufficient_MixedOpsRejected)
{
    uint32_t opTypes[] = {
        static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER), static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLREDUCE)};
    Mc2TilingTestData tiling = BuildMc2Tiling(2, static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED), opTypes, nullptr);

    // 多算子场景先被CheckCcuAlgorithmsRegistered拒绝（tilingNum>1），不进入逐算子校验
    EXPECT_EQ(CheckOpResSufficient(comm_, stream_, &tiling), HCCL_E_ALG_NOT_SUPPORTED);
}

TEST_F(CcuMc2TestSuite, CheckOpResSufficient_NullComm)
{
    uint32_t opTypes[] = {static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER)};
    Mc2TilingTestData tiling = BuildMc2Tiling(1, static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED), opTypes, nullptr);
    EXPECT_EQ(CheckOpResSufficient(nullptr, stream_, &tiling), HCCL_E_PTR);
}

TEST_F(CcuMc2TestSuite, CheckOpResSufficient_NullStream)
{
    uint32_t opTypes[] = {static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER)};
    Mc2TilingTestData tiling = BuildMc2Tiling(1, static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED), opTypes, nullptr);
    EXPECT_EQ(CheckOpResSufficient(comm_, nullptr, &tiling), HCCL_E_PTR);
}

TEST_F(CcuMc2TestSuite, CheckOpResSufficient_NullTiling)
{
    EXPECT_EQ(CheckOpResSufficient(comm_, stream_, nullptr), HCCL_E_PTR);
}

TEST_F(CcuMc2TestSuite, CheckOpResSufficient_UnsupportedEngine)
{
    uint32_t opTypes[] = {static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER)};
    Mc2TilingTestData tiling = BuildMc2Tiling(1, COMM_ENGINE_AIV, opTypes, nullptr);
    EXPECT_EQ(CheckOpResSufficient(comm_, stream_, &tiling), HCCL_E_NOT_SUPPORT);
}

TEST_F(CcuMc2TestSuite, CheckOpResSufficient_MixedEnginesRejected)
{
    // 两个tiling携带不同commEngine（CCU+AICPU），ObtainCommEngine混合引擎分支拒绝，
    // 验证HcclGetTilingList→ObtainCommEngine整条链路返回HCCL_E_NOT_SUPPORT
    uint32_t opTypes[] = {
        static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER),
        static_cast<uint32_t>(HcclCMDType::HCCL_CMD_REDUCE_SCATTER)};
    Mc2TilingTestData tiling = BuildMc2Tiling(2, COMM_ENGINE_CCU, opTypes, nullptr);
    tiling.ccTiling[1].commEngine = COMM_ENGINE_AICPU;
    EXPECT_EQ(CheckOpResSufficient(comm_, stream_, &tiling), HCCL_E_NOT_SUPPORT);
}

TEST_F(CcuMc2TestSuite, CheckOpResSufficient_ReduceScatterRejectsUnsupportedCombos)
{
    // 端到端：CCU_SCHED + REDUCE_SCATTER 时，64位数据类型与PROD归约在默认selector被拒绝。
    // CheckCcuAlgorithmsRegistered要求algConfig为非空且已注册的强制算法名，
    // 因此保留BuildMc2Tiling写入的CcuSchedReduceScatterSoleMesh；
    // 通过g_stubCcuAlgExecNull令强制算法校验失败→回退到默认selector，
    // 桩ExecuteSelector复刻真实selector的64位/PROD拒绝规则。
    uint32_t opTypes[] = {static_cast<uint32_t>(HcclCMDType::HCCL_CMD_REDUCE_SCATTER)};
    mc2_ops_hccl::g_stubCcuAlgExecNull = true;
    mc2_ops_hccl::g_stubCcuAlgExecNullName = "CcuSchedReduceScatterSoleMesh";

    const std::vector<uint8_t> unsupportedTypes = {HCCL_DATA_TYPE_INT64, HCCL_DATA_TYPE_UINT64, HCCL_DATA_TYPE_FP64};
    for (const uint8_t dataType : unsupportedTypes) {
        Mc2TilingTestData tiling =
            BuildMc2Tiling(1, static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED), opTypes, nullptr);
        tiling.ccTiling[0].srcDataType = dataType;
        tiling.ccTiling[0].dstDataType = dataType;
        EXPECT_EQ(CheckOpResSufficient(comm_, stream_, &tiling), HCCL_E_NOT_SUPPORT);
    }

    Mc2TilingTestData tiling = BuildMc2Tiling(1, static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED), opTypes, nullptr);
    tiling.ccTiling[0].reduceType = HCCL_REDUCE_PROD;
    EXPECT_EQ(CheckOpResSufficient(comm_, stream_, &tiling), HCCL_E_NOT_SUPPORT);
}

} // namespace
