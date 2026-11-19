/**
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

#include <algorithm>
#include <cstring>
#include <memory>

namespace HcclSim {
namespace CcuSt {
namespace {
constexpr uint64_t GUARD = 32;
constexpr uint32_t REQUESTS = 5;
constexpr uint32_t QUEUE_WORDS = mc2_ops_hccl::CCU_PARAM_NUM_MAX;
constexpr uint32_t CKE_WORD = (REQUESTS + 1) * QUEUE_WORDS;
constexpr uint32_t CKE_REGION = mc2_ops_hccl::CCU_TASK_NUM_MAX;

class PeerOnlyLocalCopyTest : public ::testing::Test {
protected:
    void SetUp() override { RegisterManager::Global().Reset(); }
    void TearDown() override { RegisterManager::Global().Reset(); }

    template <typename T>
    static T Input(uint32_t source, uint32_t dest, uint32_t request, uint64_t index)
    {
        return static_cast<T>(static_cast<int>((source * 5 + dest * 3 + request + index) % 15) - 7);
    }

    template <typename T>
    void Verify(HcclDataType type, HcclReduceOp op, uint64_t count, bool paddedStride)
    {
        constexpr uint64_t opId = 1U;
        const uint64_t bytes = count * sizeof(T);
        const uint64_t stride = bytes + (paddedStride ? 128 : 0);
        const uint64_t requestSpan = 2 * stride + GUARD;
        const uint64_t outputSpan = bytes + GUARD;
        std::vector<RankMemory> memories(2);
        std::vector<RankLaunch> launches(2);
        std::vector<std::vector<uint8_t>> expected(2);
        const bool skip[REQUESTS] = {false, false, true, false, true};

        for (uint32_t rank = 0; rank < 2; ++rank) {
            auto& memory = memories[rank];
            memory.input.assign(GUARD + REQUESTS * requestSpan + GUARD, 0x5a);
            memory.output.assign(GUARD + REQUESTS * outputSpan + GUARD, 0xa5);
            memory.parameter.assign(CKE_WORD + 2 * CKE_REGION, 0);
            expected[rank] = memory.output;
            for (uint32_t request = 0; request < REQUESTS; ++request) {
                const uint64_t inBase = GUARD + request * requestSpan;
                const uint64_t outBase = GUARD + request * outputSpan;
                for (uint64_t byte = 0; byte < bytes; ++byte) {
                    memory.output[outBase + byte] =
                        request == 0 ? 0 : (request == 1 ? 0x55 : static_cast<uint8_t>(byte * 37 + rank + 11));
                }
                for (uint64_t i = 0; i < count; ++i) {
                    for (uint32_t dest = 0; dest < 2; ++dest) {
                        T value = Input<T>(rank, dest, request, i);
                        std::memcpy(memory.input.data() + inBase + dest * stride + i * sizeof(T), &value, sizeof(T));
                    }
                    const T local = Input<T>(rank, rank, request, i);
                    const T peer = Input<T>(rank ^ 1U, rank, request, i);
                    T result = op == HCCL_REDUCE_MAX ?
                                   std::max(local, peer) :
                                   (op == HCCL_REDUCE_MIN ? std::min(local, peer) :
                                                            (op == HCCL_REDUCE_PROD ? static_cast<T>(local * peer) :
                                                                                      static_cast<T>(local + peer)));
                    std::memcpy(expected[rank].data() + outBase + i * sizeof(T), &result, sizeof(T));
                    if (skip[request]) {
                        std::memcpy(memory.output.data() + outBase + i * sizeof(T), &local, sizeof(T));
                    }
                }
                if (skip[request]) {
                    // Matmul need not populate its own input-workspace slice: any copy would corrupt output.
                    std::fill_n(memory.input.data() + inBase + rank * stride, bytes, 0x7f);
                }
                auto* p = memory.parameter.data() + request * QUEUE_WORDS;
                p[0] = opId;
                p[1] = memory.InputAddress() + inBase;
                p[2] = memory.OutputAddress() + outBase;
                p[4] = rank * stride;
                p[5] = skip[request] ? 1 : 0;
                const uint64_t chunks = bytes == 0 ? 0 : (bytes - 1) / (256ULL * 1024 * 1024) + 1;
                p[7] = chunks == 0 ? 0 : bytes - (chunks - 1) * (256ULL * 1024 * 1024);
                p[8] = UINT64_MAX - chunks;
                memory.parameter[CKE_WORD + request] = 1;
            }
            memory.parameter[REQUESTS * QUEUE_WORDS] = UINT64_MAX;
            memory.parameter[CKE_WORD + REQUESTS] = 1;

            mc2_ops_hccl::CcuKernelArgKfcServer arg;
            arg.opId = opId;
            arg.opParam.opType = HCCL_CMD_REDUCE_SCATTER;
            arg.opParam.DataDes.dataType = type;
            arg.opParam.reduceType = op;
            std::strcpy(arg.opParam.algName, "CcuSchedReduceScatterSoleMeshPeerOnly");
            arg.rankSize = 2;
            arg.rankId = rank;
            arg.channelCount = 1;
            arg.channels[0] = MakeChannel(rank, rank ^ 1U);
            arg.tilingArgs.push_back(std::make_shared<mc2_ops_hccl::CcuKernelArgKfcServer>(arg));
            std::vector<ChannelHandle> channels{arg.channels[0]};
            auto& registry = RegisterManager::Global();
            ASSERT_EQ(registry.RegisterStart(rank + 1), Result::SUCCESS);
            KernelHandle handle = 0;
            ASSERT_EQ(
                registry.Register(
                    rank + 1, "peer-only-local-copy",
                    [&](void*) {
                        CompilerContext::Current().ConfigureProgram(rank, 2, channels);
                        return mc2_ops_hccl::CcuKfcServerKernel(&arg) == CCU_SUCCESS ? Result::SUCCESS :
                                                                                       Result::CONTRACT_ERROR;
                    },
                    nullptr, handle),
                Result::SUCCESS);
            ASSERT_EQ(registry.RegisterEnd(rank + 1), Result::SUCCESS);
            launches[rank] = {
                handle,
                {memory.ParameterAddress(), memory.ParameterAddress() + CKE_WORD * 8, 1, 1, 0, 0x1000U + rank},
                &memory};
        }
        RuntimeWorld runtime;
        ASSERT_EQ(runtime.Launch(launches), Result::SUCCESS) << runtime.LastError();
        ASSERT_TRUE(CcuDagChecker().Check(runtime.ExecutionLogData()).Ok());
        for (uint32_t rank = 0; rank < 2; ++rank) {
            EXPECT_EQ(memories[rank].output, expected[rank]) << "rank=" << rank;
            const auto& executed = runtime.LastExecutionStats()[rank].executed;
            auto copies = executed.find(OpCode::LOCAL_COPY);
            EXPECT_EQ(copies == executed.end() ? 0U : copies->second, count == 0 ? 0U : 3U);
            auto reductions = executed.find(OpCode::READ_REDUCE);
            EXPECT_EQ(reductions == executed.end() ? 0U : reductions->second, count == 0 ? 0U : REQUESTS);
        }
    }
};

TEST_F(PeerOnlyLocalCopyTest, Int8MaxAlternatingModes)
{
    Verify<int8_t>(HCCL_DATA_TYPE_INT8, HCCL_REDUCE_MAX, 257, false);
}
TEST_F(PeerOnlyLocalCopyTest, Int8MaxStrideAndTail)
{
    Verify<int8_t>(HCCL_DATA_TYPE_INT8, HCCL_REDUCE_MAX, 476625, true);
}
TEST_F(PeerOnlyLocalCopyTest, Int8Min) { Verify<int8_t>(HCCL_DATA_TYPE_INT8, HCCL_REDUCE_MIN, 257, true); }
TEST_F(PeerOnlyLocalCopyTest, Int8Sum) { Verify<int8_t>(HCCL_DATA_TYPE_INT8, HCCL_REDUCE_SUM, 257, true); }
TEST_F(PeerOnlyLocalCopyTest, Int8Prod) { Verify<int8_t>(HCCL_DATA_TYPE_INT8, HCCL_REDUCE_PROD, 257, true); }
TEST_F(PeerOnlyLocalCopyTest, FloatSum) { Verify<float>(HCCL_DATA_TYPE_FP32, HCCL_REDUCE_SUM, 65, true); }
TEST_F(PeerOnlyLocalCopyTest, ZeroLength) { Verify<int8_t>(HCCL_DATA_TYPE_INT8, HCCL_REDUCE_MAX, 0, false); }

} // namespace
} // namespace CcuSt
} // namespace HcclSim
