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

#include <algorithm>

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
        const uint32_t ranks = meshSize * nhrSize;
        const uint64_t bytes = count * elementSize;
        const uint64_t part0 = count / 2 * elementSize;
        const uint64_t stride = 2 * bytes + 16; // 两个请求 + rank 间 guard
        const uint32_t mission = mesh ? 0U : 1U;
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
                p[KFC_PARALLEL_AG_OP_ID] = 1;
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
            auto& registry = RegisterManager::Global();
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
