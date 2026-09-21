/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <cstdlib>
#include <cstring>
#include <functional>
#include <limits>
#include <string>
#include <thread>
#include <vector>

#include "gtest/gtest.h"

#include "acl/acl_rt.h"
#include "alg_env_config.h"
#include "hccl_alloc_ctx_res.h"
#include "mc2_aicpu_launch_test_stub.h"
#include "sim_communicator.h"
#include "sim_world.h"
#include "include/adv_api/hccl/hccl_mc2.h"
#include "include/adv_api/hccl/internal/hccl_msg.h"
#include "runtime/rt.h"
#include "rt_external.h"

namespace mc2_ops_hccl {
extern bool g_rejectDirectAclrtMemcpy;
extern uint32_t g_aclrtMemcpyCallCount;
extern uint32_t g_hcclEngineCtxCopyCallCount;
extern uint32_t g_stubEndpointBwCoeff;
} // namespace mc2_ops_hccl

namespace checker {
namespace {
using namespace HcclSim;
using namespace mc2_ops_hccl;

constexpr uint32_t RANK_SIZE = 8U;
constexpr uint32_t RANK_ID = 0U;
constexpr uint32_t SINGLE_RANK_SIZE = 1U;
constexpr uint32_t PAIR_RANK_SIZE = 2U;
// Keep in sync with MC2_AICPU_HCCL_DEFAULT_TIME, MC2_AICPU_PARAM_LEN and the timeout clamp in
// impl/adv_api/detail/hccl/cc/src/common/hccl_mc2.cc.
constexpr uint16_t DEFAULT_HCCL_TIMEOUT_S = 1836U;
constexpr uint16_t MAX_LAUNCH_TIMEOUT = 65534U;
constexpr uint32_t AICPU_PARAM_LEN = 32U;
constexpr uint32_t SO_NAME_OFFSET = sizeof(uint64_t) * 2U;
constexpr uint32_t KERNEL_NAME_OFFSET = SO_NAME_OFFSET + AICPU_PARAM_LEN;
constexpr uint32_t OP_NAME_OFFSET = KERNEL_NAME_OFFSET + AICPU_PARAM_LEN;
constexpr uint32_t HOST_ARGS_SIZE = OP_NAME_OFFSET + AICPU_PARAM_LEN;
constexpr char SERVER_SO_NAME[] = "libmc2_server.so";
constexpr char SERVER_KERNEL_NAME[] = "Mc2ServerKernel";
constexpr char EXEC_TIMEOUT_ENV[] = "HCCL_EXEC_TIMEOUT";
constexpr int32_t RT_LAUNCH_FAILURE = -1;
// Empty algConfig lets the auto selector pick the algorithm.
constexpr char DEFAULT_ALG_CONFIG[] = "";

TopoMeta BuildTopoMeta(uint32_t rankSize)
{
    TopoMeta topoMeta;
    SuperPodMeta superPod;
    ServerMeta server;
    for (uint32_t i = 0U; i < rankSize; ++i) {
        server.push_back(i);
    }
    superPod.push_back(server);
    topoMeta.push_back(superPod);
    return topoMeta;
}

std::string ArgsFieldAt(const Mc2AicpuLaunchStubState& state, uint32_t offset)
{
    if (state.argsBlob.size() < offset + AICPU_PARAM_LEN) {
        return "";
    }
    const char* field = state.argsBlob.data() + offset;
    uint32_t len = 0U;
    while (len < AICPU_PARAM_LEN && field[len] != '\0') {
        ++len;
    }
    return std::string(field, len);
}

// Per rank result of a concurrent Mc2AcquireCcResCtx run.
struct RankAcquireOutcome {
    HcclResult initRet = HCCL_E_UNAVAIL;
    HcclResult acquireRet = HCCL_E_UNAVAIL;
    HcclResult secondAcquireRet = HCCL_E_UNAVAIL;
    uint32_t ccResCtxSize = 0U;
    bool reusedCtx = false;
    OpResCtx opResCtx{};
    OpParam opParam{};
};

class AicpuMc2Test : public testing::Test {
protected:
    void SetUp() override
    {
        ResetStubState();
        // The simulator reports one port per endpoint so that channel acquisition can complete.
        g_stubEndpointBwCoeff = 1U;
        InitWorld(DevType::DEV_TYPE_950, RANK_SIZE);
    }

    void TearDown() override
    {
        DestroyComm();
        SimWorld::Global()->Deinit();
        ResetStubState();
    }

    void ResetStubState()
    {
        ResetMc2AicpuLaunchStubState();
        ResetAlgEnvConfigInitState();
        g_rejectDirectAclrtMemcpy = false;
        g_aclrtMemcpyCallCount = 0U;
        g_hcclEngineCtxCopyCallCount = 0U;
        g_stubEndpointBwCoeff = 0U;
        unsetenv(EXEC_TIMEOUT_ENV);
    }

    void InitWorld(DevType devType, uint32_t rankSize)
    {
        topoMeta_ = BuildTopoMeta(rankSize);
        SimWorld::Global()->Init(topoMeta_, devType);
        ASSERT_EQ(aclrtSetDevice(RANK_ID), ACL_SUCCESS);
        ASSERT_EQ(Sim_HcclCommInitClusterInfo(topoMeta_, RANK_ID, &comm_), HCCL_SUCCESS);
        ASSERT_NE(comm_, nullptr);
    }

    void ReinitWorld(DevType devType, uint32_t rankSize)
    {
        DestroyComm();
        SimWorld::Global()->Deinit();
        InitWorld(devType, rankSize);
    }

    void DestroyComm()
    {
        if (comm_ != nullptr) {
            EXPECT_EQ(HcclCommDestroy(comm_), HCCL_SUCCESS);
            comm_ = nullptr;
        }
    }

    void* MakeCcArgs(uint8_t commEngine, const char* algConfig)
    {
        void* ccArgs = nullptr;
        if (Mc2GetCcArgs(&ccArgs) != HCCL_SUCCESS || ccArgs == nullptr) {
            return nullptr;
        }
        if (Mc2SetCcCommEngine(ccArgs, commEngine) != HCCL_SUCCESS ||
            Mc2SetCcSrcDataType(ccArgs, HCCL_DATA_TYPE_FP16) != HCCL_SUCCESS ||
            Mc2SetCcDstDataType(ccArgs, HCCL_DATA_TYPE_FP16) != HCCL_SUCCESS ||
            Mc2SetCcReduceType(ccArgs, HCCL_REDUCE_SUM) != HCCL_SUCCESS ||
            (algConfig != nullptr && Mc2SetCcAlgConfig(ccArgs, algConfig) != HCCL_SUCCESS)) {
            (void)Mc2FreeCcArgs(ccArgs);
            return nullptr;
        }
        return ccArgs;
    }

    // The simulator completes a channel only while the peer rank creates its reverse channel, so the
    // resource acquiring path has to run on every rank at the same time.
    void AcquireOnAllRanks(
        uint32_t rankSize, uint8_t ccType, uint8_t commEngine, uint8_t reduceType, const char* algConfig,
        bool acquireTwice, std::vector<RankAcquireOutcome>& outcomes)
    {
        DestroyComm();
        SimWorld::Global()->Deinit();
        topoMeta_ = BuildTopoMeta(rankSize);
        SimWorld::Global()->Init(topoMeta_, DevType::DEV_TYPE_950);
        outcomes.assign(rankSize, {});

        std::vector<std::thread> workers;
        for (uint32_t rank = 0U; rank < rankSize; ++rank) {
            workers.emplace_back([this, rank, ccType, commEngine, reduceType, algConfig, acquireTwice, &outcomes]() {
                auto& outcome = outcomes[rank];
                if (aclrtSetDevice(static_cast<int32_t>(rank)) != ACL_SUCCESS) {
                    return;
                }
                HcclComm comm = nullptr;
                outcome.initRet = Sim_HcclCommInitClusterInfo(topoMeta_, rank, &comm);
                if (outcome.initRet != HCCL_SUCCESS || comm == nullptr) {
                    return;
                }
                void* ccArgs = MakeCcArgs(commEngine, algConfig);
                if (ccArgs != nullptr) {
                    (void)Mc2SetCcReduceType(ccArgs, reduceType);
                    void* ccResCtx = nullptr;
                    outcome.acquireRet = static_cast<HcclResult>(
                        Mc2AcquireCcResCtx(comm, ccType, ccArgs, &ccResCtx, &outcome.ccResCtxSize));
                    if (outcome.acquireRet == HCCL_SUCCESS && ccResCtx != nullptr) {
                        outcome.opResCtx = *static_cast<const OpResCtx*>(ccResCtx);
                        if (outcome.opResCtx.algInfo[0].opParam != 0U) {
                            outcome.opParam = *reinterpret_cast<const OpParam*>(outcome.opResCtx.algInfo[0].opParam);
                        }
                        if (acquireTwice) {
                            void* secondCtx = nullptr;
                            uint32_t secondSize = 0U;
                            outcome.secondAcquireRet = static_cast<HcclResult>(
                                Mc2AcquireCcResCtx(comm, ccType, ccArgs, &secondCtx, &secondSize));
                            outcome.reusedCtx = (secondCtx == ccResCtx) && (secondSize == outcome.ccResCtxSize);
                        }
                    }
                    (void)Mc2FreeCcArgs(ccArgs);
                }
                (void)HcclCommDestroy(comm);
            });
        }
        for (auto& worker : workers) {
            worker.join();
        }
    }

    // Builds a minimal device-side context whose OpParam selects the AICPU launch branch.
    void BuildAicpuLaunchCtx(OpParam& opParam, OpResCtx& opResCtx, CommEngine engine)
    {
        opParam = OpParam{};
        opParam.hcclComm = comm_;
        opParam.engine = engine;
        opParam.resCtx = reinterpret_cast<void*>(0x1);
        opParam.ctxSize = 1U;

        opResCtx = OpResCtx{};
        opResCtx.commEngine = static_cast<uint64_t>(engine);
        opResCtx.workSpace = 1U;
        opResCtx.workSpaceSize = 1U;
        opResCtx.algInfo[0].opParam = reinterpret_cast<uint64_t>(&opParam);
    }

    HcclComm comm_ = nullptr;
    aclrtStream stream_ = reinterpret_cast<aclrtStream>(0x1234);
    TopoMeta topoMeta_;
};

TEST_F(AicpuMc2Test, AcquireRejectsNullParameters)
{
    void* ccArgs = MakeCcArgs(static_cast<uint8_t>(OpExecuteConfig::AICPU_TS), nullptr);
    ASSERT_NE(ccArgs, nullptr);

    void* ccResCtx = reinterpret_cast<void*>(0x1);
    uint32_t ccResCtxSize = 1U;
    EXPECT_EQ(
        Mc2AcquireCcResCtx(
            nullptr, static_cast<uint8_t>(HcclCMDType::HCCL_CMD_ALLGATHER), ccArgs, &ccResCtx, &ccResCtxSize),
        HCCL_E_PTR);
    EXPECT_EQ(
        Mc2AcquireCcResCtx(
            comm_, static_cast<uint8_t>(HcclCMDType::HCCL_CMD_ALLGATHER), nullptr, &ccResCtx, &ccResCtxSize),
        HCCL_E_PTR);
    EXPECT_EQ(
        Mc2AcquireCcResCtx(
            comm_, static_cast<uint8_t>(HcclCMDType::HCCL_CMD_ALLGATHER), ccArgs, nullptr, &ccResCtxSize),
        HCCL_E_PTR);
    EXPECT_EQ(
        Mc2AcquireCcResCtx(comm_, static_cast<uint8_t>(HcclCMDType::HCCL_CMD_ALLGATHER), ccArgs, &ccResCtx, nullptr),
        HCCL_E_PTR);

    EXPECT_EQ(Mc2FreeCcArgs(ccArgs), HCCL_SUCCESS);
}

TEST_F(AicpuMc2Test, AcquireRejectsUnsupportedCcType)
{
    void* ccArgs = MakeCcArgs(static_cast<uint8_t>(OpExecuteConfig::AICPU_TS), nullptr);
    ASSERT_NE(ccArgs, nullptr);

    void* ccResCtx = nullptr;
    uint32_t ccResCtxSize = 0U;
    EXPECT_EQ(Mc2AcquireCcResCtx(comm_, 0xFFU, ccArgs, &ccResCtx, &ccResCtxSize), HCCL_E_NOT_SUPPORT);
    EXPECT_EQ(ccResCtx, nullptr);
    EXPECT_EQ(ccResCtxSize, 0U);
    EXPECT_EQ(Mc2FreeCcArgs(ccArgs), HCCL_SUCCESS);
}

TEST_F(AicpuMc2Test, AcquireRejectsSingleRank)
{
    ReinitWorld(DevType::DEV_TYPE_950, SINGLE_RANK_SIZE);
    ASSERT_NE(comm_, nullptr);

    void* ccArgs = MakeCcArgs(static_cast<uint8_t>(OpExecuteConfig::AICPU_TS), nullptr);
    ASSERT_NE(ccArgs, nullptr);

    void* ccResCtx = nullptr;
    uint32_t ccResCtxSize = 0U;
    EXPECT_EQ(
        Mc2AcquireCcResCtx(
            comm_, static_cast<uint8_t>(HcclCMDType::HCCL_CMD_ALLGATHER), ccArgs, &ccResCtx, &ccResCtxSize),
        HCCL_E_NOT_SUPPORT);
    EXPECT_EQ(ccResCtx, nullptr);
    EXPECT_EQ(Mc2FreeCcArgs(ccArgs), HCCL_SUCCESS);
}

TEST_F(AicpuMc2Test, AcquireRejectsUnsupportedDevice)
{
    ReinitWorld(DevType::DEV_TYPE_910_93, RANK_SIZE);
    ASSERT_NE(comm_, nullptr);

    void* ccArgs = MakeCcArgs(static_cast<uint8_t>(OpExecuteConfig::AICPU_TS), nullptr);
    ASSERT_NE(ccArgs, nullptr);

    void* ccResCtx = nullptr;
    uint32_t ccResCtxSize = 0U;
    EXPECT_EQ(
        Mc2AcquireCcResCtx(
            comm_, static_cast<uint8_t>(HcclCMDType::HCCL_CMD_ALLGATHER), ccArgs, &ccResCtx, &ccResCtxSize),
        HCCL_E_NOT_SUPPORT);
    EXPECT_EQ(ccResCtx, nullptr);
    EXPECT_EQ(Mc2FreeCcArgs(ccArgs), HCCL_SUCCESS);
}

TEST_F(AicpuMc2Test, AcquireAicpuTsBuildsContextDecoupledOpParam)
{
    std::vector<RankAcquireOutcome> outcomes;
    AcquireOnAllRanks(
        PAIR_RANK_SIZE, static_cast<uint8_t>(HcclCMDType::HCCL_CMD_ALLGATHER),
        static_cast<uint8_t>(OpExecuteConfig::AICPU_TS), HCCL_REDUCE_SUM, DEFAULT_ALG_CONFIG, false, outcomes);
    ASSERT_EQ(outcomes.size(), PAIR_RANK_SIZE);

    for (const auto& outcome : outcomes) {
        ASSERT_EQ(outcome.initRet, HCCL_SUCCESS);
        ASSERT_EQ(outcome.acquireRet, HCCL_SUCCESS);
        EXPECT_EQ(outcome.ccResCtxSize, sizeof(OpResCtx));
        EXPECT_EQ(outcome.opResCtx.opType[0], static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER));
        EXPECT_EQ(outcome.opResCtx.rankSize, PAIR_RANK_SIZE);
        EXPECT_NE(outcome.opResCtx.workSpace, 0U);
        EXPECT_NE(outcome.opResCtx.workSpaceSize, 0U);
        EXPECT_EQ(outcome.opResCtx.opParamSize[0], sizeof(OpParam));
        EXPECT_NE(outcome.opResCtx.algInfo[0].opParam, 0U);

        EXPECT_NE(outcome.opParam.hcclComm, nullptr);
        // Algorithm selection applies the AICPU expansion mode, which normalizes the device-side execution
        // engine (opParam.engine) to the thread scheduled variant. The host-side launch dispatch key
        // (opResCtx.commEngine) stays COMM_ENGINE_AICPU, which is what Mc2CcKernelLaunch switches on.
        EXPECT_EQ(outcome.opParam.engine, COMM_ENGINE_AICPU_TS);
        EXPECT_EQ(outcome.opResCtx.commEngine, static_cast<uint64_t>(COMM_ENGINE_AICPU));
        EXPECT_EQ(outcome.opParam.opExecuteConfig, OpExecuteConfig::AICPU_TS);
        EXPECT_EQ(outcome.opParam.commOpExpansionMode, HcclOpExpansionMode::HCCL_OP_EXPANSION_MODE_AI_CPU);
        // The AICPU path is stream decoupled: the launch stream is handed to Mc2CcKernelLaunch instead.
        EXPECT_EQ(outcome.opParam.stream, nullptr);
        EXPECT_EQ(outcome.opParam.opType, HcclCMDType::HCCL_CMD_ALLGATHER);
        EXPECT_EQ(outcome.opParam.DataDes.dataType, HCCL_DATA_TYPE_FP16);
        EXPECT_NE(outcome.opParam.algName[0], '\0');
        EXPECT_NE(outcome.opParam.resCtx, nullptr);
        EXPECT_NE(outcome.opParam.ctxSize, 0U);
    }
    EXPECT_EQ(outcomes[RANK_ID].opResCtx.rankId, RANK_ID);
    EXPECT_EQ(outcomes[1].opResCtx.rankId, 1U);
}

TEST_F(AicpuMc2Test, AcquireAcceptsPlainAicpuCommEngine)
{
    std::vector<RankAcquireOutcome> outcomes;
    AcquireOnAllRanks(
        PAIR_RANK_SIZE, static_cast<uint8_t>(HcclCMDType::HCCL_CMD_ALLGATHER),
        static_cast<uint8_t>(OpExecuteConfig::AICPU_TS), HCCL_REDUCE_SUM, DEFAULT_ALG_CONFIG, false, outcomes);
    ASSERT_EQ(outcomes.size(), PAIR_RANK_SIZE);

    for (const auto& outcome : outcomes) {
        ASSERT_EQ(outcome.acquireRet, HCCL_SUCCESS);
        EXPECT_EQ(outcome.ccResCtxSize, sizeof(OpResCtx));
        EXPECT_EQ(outcome.opResCtx.opType[0], static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER));
        EXPECT_EQ(outcome.opResCtx.commEngine, static_cast<uint64_t>(COMM_ENGINE_AICPU));
        EXPECT_EQ(outcome.opParam.engine, COMM_ENGINE_AICPU_TS);
        EXPECT_EQ(outcome.opParam.opExecuteConfig, OpExecuteConfig::AICPU_TS);
        EXPECT_EQ(outcome.opParam.commOpExpansionMode, HcclOpExpansionMode::HCCL_OP_EXPANSION_MODE_AI_CPU);
    }
}

TEST_F(AicpuMc2Test, AcquireReusesContextForTheSameTag)
{
    std::vector<RankAcquireOutcome> outcomes;
    AcquireOnAllRanks(
        PAIR_RANK_SIZE, static_cast<uint8_t>(HcclCMDType::HCCL_CMD_ALLGATHER),
        static_cast<uint8_t>(OpExecuteConfig::AICPU_TS), HCCL_REDUCE_SUM, DEFAULT_ALG_CONFIG, true, outcomes);
    ASSERT_EQ(outcomes.size(), PAIR_RANK_SIZE);

    for (const auto& outcome : outcomes) {
        ASSERT_EQ(outcome.acquireRet, HCCL_SUCCESS);
        ASSERT_EQ(outcome.secondAcquireRet, HCCL_SUCCESS);
        EXPECT_TRUE(outcome.reusedCtx);
    }
}

TEST_F(AicpuMc2Test, LaunchUsesSimpleCtxProtocol)
{
    OpParam opParam{};
    OpResCtx opResCtx{};
    BuildAicpuLaunchCtx(opParam, opResCtx, COMM_ENGINE_AICPU);

    Mc2CcKernelLaunch(stream_, &opResCtx, sizeof(opResCtx));

    const auto& state = GetMc2AicpuLaunchStubState();
    ASSERT_EQ(state.launchCalls, 1U);
    EXPECT_EQ(state.kernelType, static_cast<uint32_t>(KERNEL_TYPE_AICPU_KFC));
    EXPECT_EQ(state.opName, SERVER_KERNEL_NAME);
    EXPECT_EQ(state.numBlocks, 1U);
    EXPECT_EQ(state.flags, static_cast<uint32_t>(RT_KERNEL_USE_SPECIAL_TIMEOUT));
    EXPECT_EQ(state.stream, stream_);
    EXPECT_EQ(state.argsSize, HOST_ARGS_SIZE);
    EXPECT_EQ(state.soNameAddrOffset, SO_NAME_OFFSET);
    EXPECT_EQ(state.kernelNameAddrOffset, KERNEL_NAME_OFFSET);
    EXPECT_FALSE(state.isNoNeedH2DCopy);
    EXPECT_EQ(state.ctxArgs[0], HcclApi::MC2_AICPU_SIMPLE_CTX_PROTOCOL);
    EXPECT_EQ(state.ctxArgs[1], reinterpret_cast<uint64_t>(&opResCtx));
    EXPECT_EQ(ArgsFieldAt(state, SO_NAME_OFFSET), SERVER_SO_NAME);
    EXPECT_EQ(ArgsFieldAt(state, KERNEL_NAME_OFFSET), SERVER_KERNEL_NAME);
    EXPECT_EQ(ArgsFieldAt(state, OP_NAME_OFFSET), SERVER_KERNEL_NAME);
    EXPECT_EQ(state.timeout, DEFAULT_HCCL_TIMEOUT_S);
}

TEST_F(AicpuMc2Test, LaunchRejectsAicpuTsEngine)
{
    OpParam opParam{};
    OpResCtx opResCtx{};
    BuildAicpuLaunchCtx(opParam, opResCtx, COMM_ENGINE_AICPU_TS);

    Mc2CcKernelLaunch(stream_, &opResCtx, sizeof(opResCtx));

    EXPECT_EQ(GetMc2AicpuLaunchStubState().launchCalls, 0U);
}

TEST_F(AicpuMc2Test, LaunchTimeoutAddsOpExecuteTimeout)
{
    auto& state = GetMc2AicpuLaunchStubState();
    state.opExecuteTimeoutMs = 30000U;

    OpParam opParam{};
    OpResCtx opResCtx{};
    BuildAicpuLaunchCtx(opParam, opResCtx, COMM_ENGINE_AICPU);
    Mc2CcKernelLaunch(stream_, &opResCtx, sizeof(opResCtx));

    ASSERT_EQ(state.launchCalls, 1U);
    EXPECT_EQ(state.opExecuteTimeoutCalls, 1U);
    EXPECT_EQ(state.timeout, DEFAULT_HCCL_TIMEOUT_S + 30U);
}

TEST_F(AicpuMc2Test, LaunchTimeoutHonoursExternalExecTimeout)
{
    ASSERT_EQ(setenv(EXEC_TIMEOUT_ENV, "100", 1), 0);
    ResetAlgEnvConfigInitState();

    OpParam opParam{};
    OpResCtx opResCtx{};
    BuildAicpuLaunchCtx(opParam, opResCtx, COMM_ENGINE_AICPU);
    Mc2CcKernelLaunch(stream_, &opResCtx, sizeof(opResCtx));

    const auto& state = GetMc2AicpuLaunchStubState();
    ASSERT_EQ(state.launchCalls, 1U);
    EXPECT_EQ(state.timeout, 100U);
}

TEST_F(AicpuMc2Test, LaunchTimeoutClampsToMaxUint16)
{
    ASSERT_EQ(setenv(EXEC_TIMEOUT_ENV, "100000", 1), 0);
    ResetAlgEnvConfigInitState();

    OpParam opParam{};
    OpResCtx opResCtx{};
    BuildAicpuLaunchCtx(opParam, opResCtx, COMM_ENGINE_AICPU);
    Mc2CcKernelLaunch(stream_, &opResCtx, sizeof(opResCtx));

    const auto& state = GetMc2AicpuLaunchStubState();
    ASSERT_EQ(state.launchCalls, 1U);
    EXPECT_EQ(state.timeout, MAX_LAUNCH_TIMEOUT);
}

TEST_F(AicpuMc2Test, LaunchSkippedWhenTimeoutQueryFails)
{
    auto& state = GetMc2AicpuLaunchStubState();
    state.opExecuteTimeoutFail = true;

    OpParam opParam{};
    OpResCtx opResCtx{};
    BuildAicpuLaunchCtx(opParam, opResCtx, COMM_ENGINE_AICPU);
    Mc2CcKernelLaunch(stream_, &opResCtx, sizeof(opResCtx));

    EXPECT_EQ(state.launchCalls, 0U);
}

TEST_F(AicpuMc2Test, LaunchSurvivesRuntimeFailure)
{
    auto& state = GetMc2AicpuLaunchStubState();
    state.launchRet = RT_LAUNCH_FAILURE;

    OpParam opParam{};
    OpResCtx opResCtx{};
    BuildAicpuLaunchCtx(opParam, opResCtx, COMM_ENGINE_AICPU);
    Mc2CcKernelLaunch(stream_, &opResCtx, sizeof(opResCtx));

    EXPECT_EQ(state.launchCalls, 1U);
}

TEST_F(AicpuMc2Test, LaunchRejectsUnsupportedDevice)
{
    ReinitWorld(DevType::DEV_TYPE_910_93, RANK_SIZE);
    ASSERT_NE(comm_, nullptr);

    OpParam opParam{};
    OpResCtx opResCtx{};
    BuildAicpuLaunchCtx(opParam, opResCtx, COMM_ENGINE_AICPU);
    Mc2CcKernelLaunch(stream_, &opResCtx, sizeof(opResCtx));

    EXPECT_EQ(GetMc2AicpuLaunchStubState().launchCalls, 0U);
}

TEST_F(AicpuMc2Test, LaunchRejectsInvalidContext)
{
    Mc2CcKernelLaunch(stream_, nullptr, sizeof(OpResCtx));
    EXPECT_EQ(GetMc2AicpuLaunchStubState().launchCalls, 0U);

    OpParam opParam{};
    OpResCtx opResCtx{};
    BuildAicpuLaunchCtx(opParam, opResCtx, COMM_ENGINE_AICPU);
    Mc2CcKernelLaunch(stream_, &opResCtx, sizeof(opResCtx) - 1U);
    EXPECT_EQ(GetMc2AicpuLaunchStubState().launchCalls, 0U);
}

TEST_F(AicpuMc2Test, AllocOpResCtxSingleParamFillsContextWithoutTiling)
{
    OpParam opParam{};
    opParam.opType = HcclCMDType::HCCL_CMD_ALLGATHER;
    opParam.engine = COMM_ENGINE_AICPU;

    g_rejectDirectAclrtMemcpy = true;
    void* opResCtxPtr = nullptr;
    ASSERT_EQ(
        HcclAllocOpResCtx(comm_, "aicpu_single_op_param", opParam, COMM_ENGINE_AICPU, &opResCtxPtr), HCCL_SUCCESS);
    ASSERT_NE(opResCtxPtr, nullptr);
    // OpParam and OpResCtx must both travel through the engine context copy, never through aclrtMemcpy.
    EXPECT_EQ(g_aclrtMemcpyCallCount, 0U);
    EXPECT_EQ(g_hcclEngineCtxCopyCallCount, 2U);

    const auto* opResCtx = static_cast<const OpResCtx*>(opResCtxPtr);
    EXPECT_EQ(opResCtx->rankId, RANK_ID);
    EXPECT_EQ(opResCtx->rankSize, RANK_SIZE);
    EXPECT_EQ(opResCtx->opType[0], static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALLGATHER));
    EXPECT_NE(opResCtx->workSpace, 0U);
    EXPECT_NE(opResCtx->workSpaceSize, 0U);
    EXPECT_EQ(opResCtx->opParamSize[0], sizeof(OpParam));
    ASSERT_NE(opResCtx->algInfo[0].opParam, 0U);
    // The single param overload carries no tiling, so the tiling offset stays untouched.
    EXPECT_EQ(opResCtx->algInfo[0].offset, 0U);

    const auto* copiedOpParam = reinterpret_cast<const OpParam*>(opResCtx->algInfo[0].opParam);
    EXPECT_EQ(copiedOpParam->opType, HcclCMDType::HCCL_CMD_ALLGATHER);
    EXPECT_EQ(copiedOpParam->engine, COMM_ENGINE_AICPU);
}

TEST_F(AicpuMc2Test, AllocOpResCtxSingleParamReservesVarMem)
{
    OpParam opParam{};
    opParam.opType = HcclCMDType::HCCL_CMD_ALLTOALLV;
    opParam.engine = COMM_ENGINE_AICPU;
    opParam.varMemSize = ALL_TO_ALL_V_VECTOR_NUM * RANK_SIZE * sizeof(uint64_t);

    void* opResCtxPtr = nullptr;
    ASSERT_EQ(
        HcclAllocOpResCtx(comm_, "aicpu_single_op_param_var_mem", opParam, COMM_ENGINE_AICPU, &opResCtxPtr),
        HCCL_SUCCESS);
    ASSERT_NE(opResCtxPtr, nullptr);

    const auto* opResCtx = static_cast<const OpResCtx*>(opResCtxPtr);
    EXPECT_EQ(opResCtx->opParamSize[0], sizeof(OpParam) + opParam.varMemSize);
}

TEST_F(AicpuMc2Test, AllocOpResCtxSingleParamReusesTagMemory)
{
    OpParam opParam{};
    opParam.opType = HcclCMDType::HCCL_CMD_ALLGATHER;

    void* firstPtr = nullptr;
    void* secondPtr = nullptr;
    ASSERT_EQ(HcclAllocOpResCtx(comm_, "aicpu_single_reuse", opParam, COMM_ENGINE_AICPU, &firstPtr), HCCL_SUCCESS);
    ASSERT_EQ(HcclAllocOpResCtx(comm_, "aicpu_single_reuse", opParam, COMM_ENGINE_AICPU, &secondPtr), HCCL_SUCCESS);
    ASSERT_NE(firstPtr, nullptr);
    EXPECT_EQ(firstPtr, secondPtr);

    const auto* firstCtx = static_cast<const OpResCtx*>(firstPtr);
    const auto* secondCtx = static_cast<const OpResCtx*>(secondPtr);
    EXPECT_EQ(firstCtx->algInfo[0].opParam, secondCtx->algInfo[0].opParam);
    EXPECT_EQ(firstCtx->workSpace, secondCtx->workSpace);
}

TEST_F(AicpuMc2Test, AllocOpResCtxSingleParamRejectsInvalidInput)
{
    OpParam opParam{};
    opParam.opType = HcclCMDType::HCCL_CMD_ALLGATHER;

    EXPECT_EQ(HcclAllocOpResCtx(comm_, "aicpu_single_null_out", opParam, COMM_ENGINE_AICPU, nullptr), HCCL_E_PTR);

    OpParam hugeOpParam{};
    hugeOpParam.opType = HcclCMDType::HCCL_CMD_ALLGATHER;
    hugeOpParam.varMemSize = std::numeric_limits<size_t>::max();
    void* opResCtxPtr = nullptr;
    EXPECT_EQ(
        HcclAllocOpResCtx(comm_, "aicpu_single_huge_var_mem", hugeOpParam, COMM_ENGINE_AICPU, &opResCtxPtr),
        HCCL_E_PARA);
    EXPECT_EQ(opResCtxPtr, nullptr);
}

TEST_F(AicpuMc2Test, PrepareCcAlgResourcesStreamCheckIsSkippable)
{
    // HCCL_CMD_INVALID has no selector, so the call fails at algorithm selection instead of acquiring
    // channels. That keeps the stream check observable without a multi rank simulation.
    OpParam opParam{};
    opParam.opType = HcclCMDType::HCCL_CMD_INVALID;
    opParam.engine = COMM_ENGINE_AICPU;
    opParam.opExecuteConfig = OpExecuteConfig::AICPU_TS;
    opParam.stream = nullptr;

    EXPECT_EQ(PrepareCcAlgResources(comm_, "", opParam), HCCL_E_PTR);
    EXPECT_EQ(PrepareCcAlgResources(comm_, "", opParam, true), HCCL_E_NOT_SUPPORT);
}

TEST_F(AicpuMc2Test, PrepareCcAlgResourcesRejectsNullArguments)
{
    OpParam opParam{};
    opParam.opType = HcclCMDType::HCCL_CMD_ALLGATHER;
    opParam.stream = stream_;

    EXPECT_EQ(PrepareCcAlgResources(nullptr, "", opParam, true), HCCL_E_PTR);
    EXPECT_EQ(PrepareCcAlgResources(comm_, nullptr, opParam, true), HCCL_E_PTR);
}

} // namespace
} // namespace checker
