/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_fixture.h"

#include <functional>

#include "sim_communicator.h"
#include "sim_world.h"
#include "topo_model.h"
#include "hccl_alloc_ctx_res.h"
#include "alg_param.h"
#include "alg_env_config.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

extern "C" {
void ResetCcuKernelRegisterCount();
uint32_t GetCcuKernelRegisterCount();
}

namespace HcclSim {
namespace CcuSt {

constexpr uint32_t GRID_SIZE = 8;

Mc2CcTilingInner CcuStFixture::BuildTiling(HcclCMDType opType, HcclDataType dataType, const std::string& algConfig)
{
    Mc2CcTilingInner tiling{};
    tiling.opType = static_cast<uint32_t>(opType);
    tiling.commEngine = static_cast<uint8_t>(OpExecuteConfig::CCU_SCHED);
    tiling.srcDataType = static_cast<uint8_t>(dataType);
    tiling.dstDataType = static_cast<uint8_t>(dataType);
    strcpy_s(tiling.groupName, sizeof(tiling.groupName), "test_group");
    strcpy_s(tiling.algConfig, sizeof(tiling.algConfig), algConfig.empty() ? "default" : algConfig.c_str());
    return tiling;
}

uint32_t CcuStFixture::CountRanks(const TopoMeta& topoMeta)
{
    uint32_t total = 0;
    for (const auto& pod : topoMeta) {
        for (const auto& server : pod) {
            total += static_cast<uint32_t>(server.size());
        }
    }
    return total;
}

void CcuStFixture::CleanupHostEnv(HcclComm& comm)
{
    if (comm != nullptr) {
        delete static_cast<HcclSim::SimCommunicator*>(comm);
        comm = nullptr;
    }
}

KernelHandle CcuStFixture::RegisterKernelForRank(
    uint32_t rank, uint32_t rankSize, const CcuStScenario& scenario, const TopoMeta& topoMeta,
    const Mc2CcTilingInner& tiling)
{
    if (aclrtSetDevice(rank) != ACL_SUCCESS) {
        HCCL_ERROR("[CcuStFixture] rank[%u] aclrtSetDevice failed", rank);
        return 0;
    }

    aclrtStream stream = nullptr;
    if (aclrtCreateStream(&stream) != ACL_SUCCESS) {
        HCCL_ERROR("[CcuStFixture] rank[%u] aclrtCreateStream failed", rank);
        return 0;
    }

    HcclComm comm = nullptr;
    if (HcclSim::Sim_HcclCommInitClusterInfo(topoMeta, rank, &comm) != HCCL_SUCCESS) {
        HCCL_ERROR("[CcuStFixture] rank[%u] Sim_HcclCommInitClusterInfo failed", rank);
        return 0;
    }

    OpResCtx resCtx{};
    if (AllocCcuOpResCtx(comm, "selector_st_ctx", rankSize, rank, resCtx) != HCCL_SUCCESS) {
        HCCL_ERROR("[CcuStFixture] rank[%u] AllocCcuOpResCtx failed", rank);
        CleanupHostEnv(comm);
        return 0;
    }

    OpParam opParam{};
    if (InitOpParamByTiling(comm, stream, "ccu_st", &tiling, opParam) != HCCL_SUCCESS) {
        HCCL_ERROR("[CcuStFixture] rank[%u] InitOpParamByTiling failed", rank);
        CleanupHostEnv(comm);
        return 0;
    }

    PrepareOpParam(opParam, rankSize, scenario);

    opParam.opExecuteConfig = mc2_ops_hccl::OpExecuteConfig::CCU_SCHED;

    std::string algName;
    auto topoInfo = std::make_unique<TopoInfoWithNetLayerDetails>();

    bool forcedAlgAccepted = false;
    if (TryForcedAlgAndPrepareEngine(comm, &tiling, opParam, algName, topoInfo, forcedAlgAccepted) != HCCL_SUCCESS) {
        HCCL_ERROR("[CcuStFixture] rank[%u] TryForcedAlgAndPrepareEngine failed", rank);
        CleanupHostEnv(comm);
        return 0;
    }
    if (!forcedAlgAccepted) {
        if (SelectAlgAndPrepareEngine(comm, opParam, algName, topoInfo) != HCCL_SUCCESS || algName.empty()) {
            HCCL_ERROR("[CcuStFixture] rank[%u] SelectAlgAndPrepareEngine failed, algName[%s]", rank, algName.c_str());
            CleanupHostEnv(comm);
            return 0;
        }
    }
    HCCL_INFO(
        "[CcuStFixture] rank[%u] selected algName[%s] engine[%u]", rank, algName.c_str(),
        static_cast<uint32_t>(opParam.engine));
    if (rank == 0 && !scenario.expectedAlgName.empty() && algName != scenario.expectedAlgName) {
        HCCL_ERROR(
            "[CcuStFixture] rank[%u] algName mismatch: expected[%s] got[%s]", rank, scenario.expectedAlgName.c_str(),
            algName.c_str());
        CleanupHostEnv(comm);
        return 0;
    }
    sprintf_s(opParam.algName, sizeof(opParam.algName), "%s", algName.c_str());

    opParam.isMc2 = false;
    bool skipGetRes = false;
    if (HandleSingleRankAndCommMode(comm, opParam, skipGetRes) != HCCL_SUCCESS || skipGetRes) {
        HCCL_ERROR(
            "[CcuStFixture] rank[%u] HandleSingleRankAndCommMode failed, skipGetRes[%d]", rank,
            static_cast<int>(skipGetRes));
        CleanupHostEnv(comm);
        return 0;
    }

    void* resCtxOut = nullptr;
    if (GetCcuOpParamResCtx(comm, algName, opParam, topoInfo.get(), resCtx, &resCtxOut) != HCCL_SUCCESS) {
        CleanupHostEnv(comm);
        return 0;
    }

    const auto& lastGroup = RegisterManager::Global().GetLastCompiledGroup();
    KernelHandle handle = lastGroup.empty() ? 0 : lastGroup.back();

    CleanupHostEnv(comm);
    return handle;
}

void CcuStFixture::VerifyScenario(const CcuStScenario& scenario)
{
    const uint32_t rankSize = CountRanks(scenario.topoMeta);

    RegisterManager::Global().Reset();
    ResetCcuKernelRegisterCount();

    // RAII: 保证 ASSERT 失败时也能清理 SimWorld 和环境变量
    struct ScopeGuard {
        std::function<void()> onExit;
        ~ScopeGuard()
        {
            if (onExit)
                onExit();
        }
    } guard{[]() {
        HcclSim::SimWorld::Global()->Deinit();
        unsetenv("HCCL_OP_EXPANSION_MODE");
        unsetenv("ASCEND_ENABLE_CCU_KFC_BRANCH");
    }};

    setenv("HCCL_OP_EXPANSION_MODE", "CCU_SCHED", 1);
    setenv("ASCEND_ENABLE_CCU_KFC_BRANCH", "1", 1);
    mc2_ops_hccl::InitEnvConfig();
    HcclSim::SimWorld::Global()->Init(scenario.topoMeta, scenario.devType);

    const TopoMeta& topoMeta = scenario.topoMeta;
    const Mc2CcTilingInner tiling = BuildTiling(scenario.opType, scenario.dataType, scenario.algConfig);

    std::vector<KernelHandle> handles(rankSize, 0);
    std::vector<std::thread> threads;
    for (uint32_t rank = 0; rank < rankSize; ++rank) {
        threads.emplace_back([this, rank, rankSize, &scenario, &topoMeta, &tiling, &handles]() {
            handles[rank] = RegisterKernelForRank(rank, rankSize, scenario, topoMeta, tiling);
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }
    for (uint32_t rank = 0; rank < rankSize; ++rank) {
        ASSERT_NE(handles[rank], 0U) << "failed to register kernel for rank " << rank;
    }

    ScenarioData data = BuildScenarioData(scenario, handles);

    for (uint32_t rank = 0; rank < rankSize; ++rank) {
        const Program* program = RegisterManager::Global().GetProgram(handles[rank]);
        ASSERT_NE(program, nullptr);
        CheckResult staticCheck = CcuProgramChecker().Check(*program);
        ASSERT_TRUE(staticCheck.Ok()) << staticCheck.message;
    }

    RuntimeExpectation expectation;
    {
        CheckResult capture = CaptureRuntimeExpectation(data, scenario, expectation);
        ASSERT_TRUE(capture.Ok()) << capture.message;
    }

    {
        RuntimeWorld world;
        ASSERT_EQ(world.Launch(data.launches), Result::SUCCESS) << world.LastError();
        CheckResult dagResult = CcuDagChecker().Check(world.ExecutionLogData());
        ASSERT_TRUE(dagResult.Ok()) << dagResult.message;
        CheckResult runtime = CheckRuntimeOutput(data.memories, expectation, world);
        ASSERT_TRUE(runtime.Ok()) << runtime.message;
    }
}

} // namespace CcuSt
} // namespace HcclSim
