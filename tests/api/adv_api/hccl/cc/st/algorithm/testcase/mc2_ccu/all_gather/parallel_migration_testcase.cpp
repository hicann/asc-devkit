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
#include "ccu_kernel_kfc_server.h"
#include "ccu_temp_kfc_all_gather_nhr_1D_multi_jetty_mem2mem.h"
#include "sim_world.h"
#include "acl/acl_rt.h"
#include "op_common.h"
#include <cstring>

#include <algorithm>
#include <functional>
#include <memory>
#include <numeric>

namespace HcclSim {
namespace CcuSt {
namespace {
using namespace HcclKfcProtocol;
using mc2_ops_hccl::CcuKernelArgKfcServer;
using mc2_ops_hccl::KfcNhrStepInfo;

constexpr uint64_t GUARD = 32;
constexpr uint32_t QUEUE_WORDS = mc2_ops_hccl::CCU_PARAM_NUM_MAX;
constexpr uint32_t CKE_WORD = 3 * QUEUE_WORDS;
constexpr uint32_t CKE_REGION = mc2_ops_hccl::CCU_TASK_NUM_MAX;

uint8_t InputByte(uint32_t rank, uint64_t offset) { return static_cast<uint8_t>(rank * 41U + offset * 17U + 3U); }

// 使用 hccl NHR 的倒序 doubling 步骤；oracle 独立按全局 rank 拼接输入，
// 不使用步骤表计算期望结果。覆盖非 2 的幂 rankSize，可检测转发分片错误。
std::vector<KfcNhrStepInfo> NhrSteps(uint32_t rank, uint32_t size)
{
    std::vector<KfcNhrStepInfo> steps;
    uint32_t distance = 1;
    while (distance < size) {
        distance *= 2;
    }
    for (distance /= 2; distance > 0; distance /= 2) {
        KfcNhrStepInfo step;
        step.toRank = (rank + distance) % size;
        step.fromRank = (rank + size - distance) % size;
        step.nSlices = (size - 1 + distance) / (2 * distance);
        uint32_t slice = rank;
        for (uint32_t i = 0; i < step.nSlices; ++i) {
            step.txSliceIdxs.push_back(slice);
            slice = (slice + size - 2 * distance) % size;
        }
        steps.push_back(step);
    }
    return steps;
}

class ParallelAllGatherMigrationTest : public ::testing::Test {
protected:
    void SetUp() override { RegisterManager::Global().Reset(); }
    void TearDown() override { RegisterManager::Global().Reset(); }

    // 直接编译产品 KFC server + dispatch + 两阶段子核，不替换算法计算。
    // 当前 RuntimeWorld 每 rank 只有一个执行上下文，故分别验证 Mesh/NHR
    // mission，预填另一 mission 的 phase-0 结果和完成标志。此测试不声称
    // 覆盖双 mission 的真实并发、GM 可见性或硬件资源容量。
    void VerifyMission(uint32_t meshSize, uint32_t nhrSize, uint64_t count, uint64_t elementSize, bool mesh)
    {
        constexpr uint64_t opId = 1U;
        const uint32_t ranks = meshSize * nhrSize;
        const uint64_t bytes = count * elementSize;
        const uint64_t part0 = count / 2 * elementSize;
        const uint64_t stride = 2 * bytes + 16; // 两个请求 + rank 间 guard
        const uint32_t mission = mesh ? 0U : 1U;

        // patch 新增的 GetCcuVersion() 会经 hrtGetDeviceType 查 SimWorld 的 SimNpu，
        // 必须先 Init SimWorld 并在每 rank Register 前 aclrtSetDevice，否则
        // curr_dev_id=UINT32_MAX 会在 CommDomain 查询时抛 InvalidParamsException。
        // DevType 固定 950：本用例预计算 GoSize 使用默认 CcuVersion::CCU_V1，
        // 960 会使 GetCcuVersion() 返回 CCU_V2 走 V2 位布局，与 V1 编码不匹配。
        struct ScopeGuard {
            std::function<void()> onExit;
            ~ScopeGuard()
            {
                if (onExit) {
                    onExit();
                }
            }
        } simGuard{[]() { HcclSim::SimWorld::Global()->Deinit(); }};
        // 单 Pod / 单 Server / ranks 个 device，CommDomain::Init 按顺序分配 rankId 0..ranks-1
        SuperPodMeta pod{ServerMeta(ranks)};
        std::iota(pod[0].begin(), pod[0].end(), 0U);
        TopoMeta topoMeta{pod};
        HcclSim::SimWorld::Global()->Init(topoMeta, DevType::DEV_TYPE_950);

        std::vector<RankMemory> memories(ranks);
        std::vector<RankLaunch> launches(ranks);
        std::vector<std::vector<uint8_t>> expected(ranks);

        for (uint32_t rank = 0; rank < ranks; ++rank) {
            SCOPED_TRACE(::testing::Message() << "rank=" << rank << " mesh=" << mesh);
            const uint32_t m = rank % meshSize;
            const uint32_t n = rank / meshSize;
            auto& memory = memories[rank];
            memory.input.assign(GUARD + 2 * bytes + GUARD, 0x5a);
            memory.output.assign(GUARD + ranks * stride + GUARD, 0xa5);
            memory.parameter.assign(CKE_WORD + 4 * CKE_REGION, 0);
            for (uint64_t i = 0; i < 2 * bytes; ++i) {
                memory.input[GUARD + i] = InputByte(rank, i);
            }
            expected[rank] = memory.output;
            for (uint32_t source = 0; source < ranks; ++source) {
                for (uint64_t entry = 0; entry < 2; ++entry) {
                    for (uint64_t i = 0; i < bytes; ++i) {
                        const auto value = InputByte(source, entry * bytes + i);
                        const uint64_t dest = GUARD + source * stride + entry * bytes + i;
                        // Mesh phase 1 消费 NHR 已收齐的 part1；NHR phase 1
                        // 消费 Mesh 已收齐的 part0。只预填对应维度内的结果。
                        const bool supplied =
                            mesh ? (source % meshSize == m && i >= part0) : (source / meshSize == n && i < part0);
                        if (supplied) {
                            memory.output[dest] = value;
                        }
                        // 本 mission phase 0 只完成自己的维度，phase 1 则
                        // 将另一半传播到所有 rank；其余空洞必须保持 guard。
                        const bool written =
                            mesh ? (source / meshSize == n || i >= part0) : (source % meshSize == m || i < part0);
                        if (written || supplied) {
                            expected[rank][dest] = value;
                        }
                    }
                }
            }
            for (uint32_t entry = 0; entry < 2; ++entry) {
                auto* p = memory.parameter.data() + entry * QUEUE_WORDS;
                p[KFC_PARALLEL_AG_OP_ID] = opId;
                p[KFC_PARALLEL_AG_INPUT] = memory.InputAddress() + GUARD + entry * bytes;
                p[KFC_PARALLEL_AG_OUTPUT] = memory.OutputAddress() + GUARD + entry * bytes;
                p[KFC_PARALLEL_AG_TOTAL_SIZE] = bytes;
                p[KFC_PARALLEL_AG_OUTPUT_STRIDE] = stride;
                p[KFC_PARALLEL_AG_PART0_SIZE] = part0;
                p[KFC_PARALLEL_AG_PART1_SIZE] = bytes - part0;
                p[KFC_PARALLEL_AG_PART1_OFFSET] = part0;
                // NHR 的本地搬运使用 GroupCopy，必须像 Device prepare 一样
                // 填写 GoSize；全零只会构建零长度 copy，遗漏本 rank 的 part1。
                mc2_ops_hccl::LoopGroupConfig config{};
                config.msInterleave = mc2_ops_hccl::CCU_MS_INTERLEAVE;
                config.loopCount = mc2_ops_hccl::CCU_MS_LOCAL_COPY_LOOP_COUNT;
                config.memSlice = mc2_ops_hccl::CCU_MS_SIZE * mc2_ops_hccl::LOCAL_COPY_MS_PER_LOOP;
                const auto part0GoSize = mc2_ops_hccl::CalGoSize(part0, config);
                const auto part1GoSize = mc2_ops_hccl::CalGoSize(bytes - part0, config);
                std::copy(part0GoSize.begin(), part0GoSize.end(), p + KFC_PARALLEL_AG_PART0_GO_SIZE_0);
                std::copy(part1GoSize.begin(), part1GoSize.end(), p + KFC_PARALLEL_AG_PART1_GO_SIZE_0);
                p[KFC_PARALLEL_AG_PART0_SLICE_PER_JETTY] = part0;
                p[KFC_PARALLEL_AG_PART0_LAST_SLICE_PER_JETTY] = part0;
                p[KFC_PARALLEL_AG_PART1_SLICE_PER_JETTY] = bytes - part0;
                p[KFC_PARALLEL_AG_PART1_LAST_SLICE_PER_JETTY] = bytes - part0;
                const uint64_t base = memory.ParameterAddress() + entry * QUEUE_WORDS * sizeof(uint64_t);
                p[KFC_PARALLEL_AG_MESH_PHASE_DONE_ADDR] = base + KFC_PARALLEL_AG_MESH_PHASE_DONE_STORAGE * 8;
                p[KFC_PARALLEL_AG_NHR_PHASE_DONE_ADDR] = base + KFC_PARALLEL_AG_NHR_PHASE_DONE_STORAGE * 8;
                p[mesh ? KFC_PARALLEL_AG_NHR_PHASE_DONE_STORAGE : KFC_PARALLEL_AG_MESH_PHASE_DONE_STORAGE] = 1;
            }
            memory.parameter[2 * QUEUE_WORDS] = UINT64_MAX; // 终止常驻 kernel
            for (uint32_t entry = 0; entry < 3; ++entry) {
                memory.parameter[CKE_WORD + mission * 2 * CKE_REGION + entry] = 1;
            }

            CcuKernelArgKfcServer arg;
            arg.opId = opId;
            arg.algSubType = mc2_ops_hccl::KFC_PARALLEL_ALL_GATHER_SUB_TYPE;
            auto parallel = std::make_shared<mc2_ops_hccl::KfcParallelAllGatherArg>();
            arg.algArg = parallel;
            arg.opParam.opType = HCCL_CMD_ALLGATHER;
            arg.role =
                mesh ? mc2_ops_hccl::KfcServerRole::ALL_GATHER_MESH : mc2_ops_hccl::KfcServerRole::ALL_GATHER_NHR;
            parallel->rankSizeLevel0 = meshSize;
            parallel->rankIdxLevel0 = m;
            parallel->rankSizeLevel1 = nhrSize;
            parallel->rankIdxLevel1 = n;
            arg.rankSize = mesh ? meshSize : nhrSize;
            arg.rankId = mesh ? m : n;
            parallel->jettyNum = 1;
            parallel->stepInfoVector = NhrSteps(n, nhrSize);
            std::vector<ChannelHandle> channels;
            for (uint32_t peer = 0; peer < arg.rankSize; ++peer) {
                if (peer == arg.rankId) {
                    continue;
                }
                parallel->rank2ChannelIdx[peer] = channels.size();
                channels.push_back(MakeChannel(rank, mesh ? n * meshSize + peer : peer * meshSize + m));
            }
            ASSERT_LE(channels.size(), sizeof(arg.channels) / sizeof(arg.channels[0]));
            std::copy(channels.begin(), channels.end(), arg.channels);
            arg.channelCount = channels.size();
            arg.tilingArgs.push_back(std::make_shared<CcuKernelArgKfcServer>(arg));
            auto& registry = RegisterManager::Global();
            // Register 在调用线程同步执行 kernel lambda，GetCcuVersion() 经
            // thread_local 的 curr_dev_id 查 SimNpu，须先绑定当前 rank 的设备
            ASSERT_EQ(aclrtSetDevice(static_cast<int32_t>(rank)), ACL_SUCCESS);
            ASSERT_EQ(registry.RegisterStart(rank + 1), Result::SUCCESS);
            KernelHandle handle = 0;
            ASSERT_EQ(
                registry.Register(
                    rank + 1, "parallel-kfc",
                    [&](void*) {
                        CompilerContext::Current().ConfigureProgram(rank, ranks, channels);
                        return mc2_ops_hccl::CcuKfcServerKernel(&arg) == CCU_SUCCESS ? Result::SUCCESS :
                                                                                       Result::CONTRACT_ERROR;
                    },
                    nullptr, handle),
                Result::SUCCESS);
            ASSERT_EQ(registry.RegisterEnd(rank + 1), Result::SUCCESS);
            launches[rank] = {
                handle,
                {memory.ParameterAddress(), memory.ParameterAddress() + CKE_WORD * 8, 1, 2, mission, 0x1000U + rank},
                &memory};
        }
        RuntimeWorld runtime;
        ASSERT_EQ(runtime.Launch(launches), Result::SUCCESS) << runtime.LastError();
        for (uint32_t rank = 0; rank < ranks; ++rank) {
            const auto mismatch =
                std::mismatch(memories[rank].output.begin(), memories[rank].output.end(), expected[rank].begin());
            ASSERT_TRUE(mismatch.first == memories[rank].output.end())
                << "rank=" << rank << " mesh=" << mesh
                << " byte=" << std::distance(memories[rank].output.begin(), mismatch.first)
                << " actual=" << static_cast<uint32_t>(*mismatch.first)
                << " expected=" << static_cast<uint32_t>(*mismatch.second);
            for (uint32_t entry = 0; entry < 2; ++entry) {
                EXPECT_EQ(memories[rank].parameter[CKE_WORD + (mission * 2 + 1) * CKE_REGION + entry], 1U);
            }
        }
    }
};

TEST_F(ParallelAllGatherMigrationTest, Mesh4Nhr2OddFp16)
{
    VerifyMission(4, 2, 65, 2, true);
    VerifyMission(4, 2, 65, 2, false);
}

TEST_F(ParallelAllGatherMigrationTest, Mesh8Nhr2OddFp32)
{
    VerifyMission(8, 2, 33, 4, true);
    VerifyMission(8, 2, 33, 4, false);
}

TEST_F(ParallelAllGatherMigrationTest, NonPowerOfTwoNhr)
{
    VerifyMission(4, 3, 67, 1, true);
    VerifyMission(4, 3, 67, 1, false);
}

TEST_F(ParallelAllGatherMigrationTest, SharedQueueSelectsOpIdsAndDifferentSubgraphs)
{
    constexpr uint32_t ranks = 2;
    constexpr uint32_t count = 16;
    constexpr uint64_t bytes = count * sizeof(int32_t);
    constexpr uint32_t signalBase = mc2_ops_hccl::CCU_TASK_NUM_MAX * QUEUE_WORDS;
    constexpr uint32_t scratchBase = signalBase + 4 * CKE_REGION;
    struct SimGuard {
        ~SimGuard() { HcclSim::SimWorld::Global()->Deinit(); }
    } guard;
    SuperPodMeta pod{ServerMeta(ranks)};
    std::iota(pod[0].begin(), pod[0].end(), 0U);
    HcclSim::SimWorld::Global()->Init(TopoMeta{pod}, DevType::DEV_TYPE_950);
    std::vector<RankMemory> memories(ranks);
    std::vector<RankLaunch> launches(ranks);
    for (uint32_t rank = 0; rank < ranks; ++rank) {
        auto& memory = memories[rank];
        memory.input.assign(GUARD + bytes + GUARD, 0x5a);
        memory.output.assign(GUARD + 3 * bytes + GUARD, 0xa5);
        memory.parameter.assign(scratchBase + bytes / sizeof(uint64_t), 0);
        for (uint32_t i = 0; i < count; ++i) {
            const int32_t value = (rank + 1) * 10 + i;
            std::memcpy(memory.input.data() + GUARD + i * sizeof(value), &value, sizeof(value));
        }
        std::vector<ChannelHandle> channels{MakeChannel(rank, 1U - rank)};
        CcuKernelArgKfcServer server;
        for (uint32_t i = 0; i < 2; ++i) {
            auto arg = std::make_shared<CcuKernelArgKfcServer>();
            arg->rankSize = ranks;
            arg->rankId = rank;
            arg->channels[0] = channels[0];
            arg->channelCount = channels.size();
            arg->opParam.opType = i == 1 ? HCCL_CMD_ALLGATHER : HCCL_CMD_ALLREDUCE;
            arg->opParam.DataDes.dataType = HCCL_DATA_TYPE_INT32;
            arg->opParam.DataDes.outputType = HCCL_DATA_TYPE_INT32;
            arg->opParam.reduceType = i == 0 ? HCCL_REDUCE_SUM : HCCL_REDUCE_MAX;
            constexpr uint32_t dataTypeShift = 8U;
            constexpr uint32_t outputTypeShift = 24U;
            arg->opId = static_cast<uint64_t>(arg->opParam.opType) |
                        (static_cast<uint64_t>(HCCL_DATA_TYPE_INT32) << dataTypeShift) |
                        (static_cast<uint64_t>(HCCL_DATA_TYPE_INT32) << outputTypeShift);
            server.tilingArgs.push_back(arg);
        }
        // AllGather -> AllReduce differs from host registration order.
        for (uint32_t entry = 0; entry < 2; ++entry) {
            auto* frame = memory.parameter.data() + entry * QUEUE_WORDS;
            constexpr uint32_t dataTypeShift = 8U;
            constexpr uint32_t outputTypeShift = 24U;
            frame[0] = static_cast<uint64_t>(entry == 0 ? HCCL_CMD_ALLGATHER : HCCL_CMD_ALLREDUCE) |
                       (static_cast<uint64_t>(HCCL_DATA_TYPE_INT32) << dataTypeShift) |
                       (static_cast<uint64_t>(HCCL_DATA_TYPE_INT32) << outputTypeShift);
            frame[1] = memory.InputAddress() + GUARD;
            frame[2] = memory.OutputAddress() + GUARD + entry * 2 * bytes;
            mc2_ops_hccl::LoopGroupConfig config{};
            config.msInterleave = mc2_ops_hccl::CCU_MS_INTERLEAVE;
            config.loopCount =
                entry == 0 ? mc2_ops_hccl::CCU_MS_LOCAL_COPY_LOOP_COUNT : mc2_ops_hccl::CCU_M2M_LOCAL_COPY_LOOP_COUNT;
            config.memSlice = mc2_ops_hccl::CCU_MS_SIZE * (entry == 0 ? mc2_ops_hccl::LOCAL_COPY_MS_PER_LOOP : 1U);
            const auto go = mc2_ops_hccl::CalGoSize(entry == 0 ? bytes : bytes / ranks, config);
            if (entry == 0) {
                frame[3] = rank * bytes;
                frame[4] = bytes;
                std::copy(go.begin(), go.end(), frame + 5);
                frame[10] = UINT64_MAX - 1;
                frame[13] = bytes;
            } else {
                frame[3] = memory.ParameterAddress() + scratchBase * sizeof(uint64_t);
                frame[6] = frame[7] = frame[8] = bytes / ranks;
                frame[9] = rank * bytes / ranks;
                std::copy(go.begin(), go.end(), frame + 11);
                frame[15] = frame[16] = bytes / ranks;
                frame[17] = UINT64_MAX - 1;
                std::copy(go.begin(), go.end(), frame + 18);
                std::copy(go.begin(), go.end(), frame + 22);
            }
            memory.parameter[signalBase + entry] = 1;
        }
        memory.parameter[2 * QUEUE_WORDS] = UINT64_MAX;
        memory.parameter[signalBase + 2] = 1;
        ASSERT_EQ(aclrtSetDevice(rank), ACL_SUCCESS);
        auto& registry = RegisterManager::Global();
        ASSERT_EQ(registry.RegisterStart(rank + 1), Result::SUCCESS);
        KernelHandle handle = 0;
        ASSERT_EQ(
            registry.Register(
                rank + 1, "shared-kfc",
                [&](void*) {
                    CompilerContext::Current().ConfigureProgram(rank, ranks, channels);
                    return mc2_ops_hccl::CcuKfcServerKernel(&server) == CCU_SUCCESS ? Result::SUCCESS :
                                                                                      Result::CONTRACT_ERROR;
                },
                nullptr, handle),
            Result::SUCCESS);
        ASSERT_EQ(registry.RegisterEnd(rank + 1), Result::SUCCESS);
        launches[rank] = {
            handle,
            {memory.ParameterAddress(), memory.ParameterAddress() + signalBase * sizeof(uint64_t), 1, 1, 0,
             0x1000U + rank},
            &memory};
    }
    RuntimeWorld runtime;
    ASSERT_EQ(runtime.Launch(launches), Result::SUCCESS) << runtime.LastError();
    for (const auto& memory : memories) {
        for (uint32_t i = 0; i < 3 * count; ++i) {
            int32_t actual;
            std::memcpy(&actual, memory.output.data() + GUARD + i * sizeof(actual), sizeof(actual));
            const int32_t expected = i < count ? 10 + i : i < 2 * count ? 20 + i - count : 30 + 2 * (i - 2 * count);
            EXPECT_EQ(actual, expected) << "element " << i;
        }
        EXPECT_TRUE(std::all_of(
            memory.output.begin(), memory.output.begin() + GUARD, [](uint8_t value) { return value == 0xa5; }));
        EXPECT_TRUE(
            std::all_of(memory.output.end() - GUARD, memory.output.end(), [](uint8_t value) { return value == 0xa5; }));
        for (uint32_t entry = 0; entry < 2; ++entry) {
            EXPECT_EQ(memory.parameter[signalBase + CKE_REGION + entry], 1U);
        }
        EXPECT_EQ(memory.parameter[signalBase + CKE_REGION + 2], 0U);
    }
}

TEST_F(ParallelAllGatherMigrationTest, BuildPreservesPerTilingResourcesOnSameDie)
{
    mc2_ops_hccl::OpParam param;
    param.isMultiTiling = true;
    mc2_ops_hccl::AlgResourceRequest request;
    auto resources = std::make_unique<mc2_ops_hccl::AlgResourceCtxSerializable>();
    resources->kfcServerArgSize = 6;
    resources->kfcServerArgs = {0x1000, 0x2000, 1, 1, 0, 0x3000};
    constexpr uint64_t opIdBase = 0x1000U;
    // Three single-mission tilings share die 1 and one persistent server.
    for (uint32_t i = 0; i < 3; ++i) {
        mc2_ops_hccl::CcuKernelInfo info{};
        std::strcpy(info.kernelFuncName, "CcuKernelKfcServer");
        auto arg = std::make_shared<CcuKernelArgKfcServer>();
        arg->channels[0] = 100 + i;
        arg->channelCount = 1;
        arg->opParam.opType = i == 0 ? HCCL_CMD_ALLREDUCE : (i == 1 ? HCCL_CMD_ALLGATHER : HCCL_CMD_REDUCE_SCATTER);
        arg->opParam.reduceType = HCCL_REDUCE_MAX;
        arg->opId = opIdBase + i;
        info.dieId = 1;
        info.setKernelArg(arg);
        request.ccuKernelInfos.push_back(info);
    }
    auto crossDieRequest = request;
    crossDieRequest.ccuKernelInfos.back().dieId = 0;
    ASSERT_EQ(mc2_ops_hccl::BuildKfcServer(request, resources, param), HCCL_SUCCESS);
    ASSERT_EQ(request.ccuKernelInfos.size(), 1U);
    EXPECT_EQ(request.ccuKernelInfos[0].dieId, 1U);
    EXPECT_EQ(request.ccuKernelNum, std::vector<uint32_t>({1U}));
    EXPECT_EQ(request.slaveThreadNum, 0U);
    EXPECT_TRUE(request.notifyNumPerThread.empty());
    EXPECT_TRUE(resources->notifyNumPerThread.empty());
    EXPECT_EQ(resources->kfcServerArgs, std::vector<uint64_t>({0x1000, 0x2000, 1, 1, 0, 0x3000}));
    const auto* first = static_cast<const CcuKernelArgKfcServer*>(request.ccuKernelInfos[0].kernelArg);
    EXPECT_STREQ(request.ccuKernelInfos[0].kernelFuncName, "CcuKernelKfcServer");
    EXPECT_EQ(first->hostMissionIndex, 0U);
    ASSERT_EQ(first->tilingArgs.size(), 3U);
    ASSERT_NE(first->tilingArgs[0], nullptr);
    EXPECT_EQ(first->tilingArgs[0]->channels[0], 100U);
    EXPECT_EQ(first->tilingArgs[0]->opParam.reduceType, HCCL_REDUCE_MAX);
    EXPECT_EQ(first->tilingArgs[0]->opId, opIdBase);
    ASSERT_NE(first->tilingArgs[1], nullptr);
    EXPECT_EQ(first->tilingArgs[1]->channels[0], 101U);
    EXPECT_EQ(first->tilingArgs[1]->opId, opIdBase + 1U);
    ASSERT_NE(first->tilingArgs[2], nullptr);
    EXPECT_EQ(first->tilingArgs[2]->channels[0], 102U);
    EXPECT_EQ(first->tilingArgs[2]->opId, opIdBase + 2U);
    EXPECT_EQ(first->tilingArgs[2]->hostMissionIndex, 0U);
    EXPECT_EQ(mc2_ops_hccl::BuildKfcServer(crossDieRequest, resources, param), HCCL_E_NOT_SUPPORT);
}

TEST_F(ParallelAllGatherMigrationTest, NhrRankBeyondEventMaskWidth) { VerifyMission(2, 17, 9, 1, false); }

TEST_F(ParallelAllGatherMigrationTest, EmptyFirstHalf)
{
    VerifyMission(4, 2, 1, 2, true);
    VerifyMission(4, 2, 1, 2, false);
}

TEST_F(ParallelAllGatherMigrationTest, LargeCopyWithTail)
{
    // per-rank >512 KiB，跨越 GroupCopy 整块边界，并带非对齐尾块。
    VerifyMission(4, 2, 262147, 2, true);
    VerifyMission(4, 2, 262147, 2, false);
}
} // namespace
} // namespace CcuSt
} // namespace HcclSim
