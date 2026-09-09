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
#include <cstring>
#include <limits>
#include <map>
#include <numeric>
#include <vector>

#include "gtest/gtest.h"
#include "alg_data_trans_wrapper.h"
#include "hcomm_primitives_dl.h"
#include "ins_temp_reduce_scatter_mesh_1D_meshchunk.h"
#include "ins_temp_reduce_scatter_nhr.h"
#include "coll_alg_v2_exec_registry.h"
#include "executor_v2_base.h"

using namespace mc2_ops_hccl;

namespace {
enum class Kind { COPY, REDUCE, CHANNEL_RECORD, CHANNEL_WAIT, THREAD_RECORD, THREAD_WAIT };
struct Task {
    Kind kind;
    ThreadHandle thread;
    uint64_t target;
    uint32_t notify;
    void* dst = nullptr;
    const void* src = nullptr;
    uint64_t count = 0;
    HcommDataType dtype = static_cast<HcommDataType>(HCCL_DATA_TYPE_INT8);
};
std::vector<Task> tasks;
std::vector<HcclHcommBatchTransferDesc> descriptors;
std::vector<size_t> batchEnds;
bool batchSupported = true;
int32_t batchResult = HCCL_SUCCESS;
int32_t copyResult = HCCL_SUCCESS;
size_t batchCalls = 0;
size_t readReduceCalls = 0;
size_t copyCalls = 0;
size_t failCopyAt = std::numeric_limits<size_t>::max();

class MeshChunkTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        tasks.clear();
        descriptors.clear();
        batchEnds.clear();
        batchSupported = true;
        batchResult = HCCL_SUCCESS;
        copyResult = HCCL_SUCCESS;
        batchCalls = 0;
        readReduceCalls = 0;
        copyCalls = 0;
        failCopyAt = std::numeric_limits<size_t>::max();
        ASSERT_EQ(InitHcommBatchTransferOnThreadSupported(true), HCCL_SUCCESS);
    }
};

SendRecvReduceInfo MakeTransfer(const std::vector<uint64_t>& counts)
{
    ChannelInfo channel{};
    channel.handle = 2;
    std::vector<DataSlice> src, dst;
    uint64_t offset = 0;
    for (auto count : counts) {
        src.emplace_back(reinterpret_cast<void*>(0x100000), offset, count * sizeof(float), count);
        dst.emplace_back(reinterpret_cast<void*>(0x200000), offset, count * sizeof(float), count);
        offset += count * sizeof(float);
    }
    return {{channel, channel}, {{src, dst}, {src, dst}}, HCCL_DATA_TYPE_FP32, HCCL_REDUCE_SUM};
}

size_t Count(Kind kind, uint32_t notify)
{
    return std::count_if(
        tasks.begin(), tasks.end(), [=](const Task& t) { return t.kind == kind && t.notify == notify; });
}
} // namespace

extern "C" bool __wrap_HcommIsSupportHcommBatchTransferOnThread() { return batchSupported; }
extern "C" int32_t __wrap_HcommChannelNotifyRecordOnThread(ThreadHandle t, ChannelHandle c, uint32_t n)
{
    tasks.push_back({Kind::CHANNEL_RECORD, t, c, n});
    return HCCL_SUCCESS;
}
extern "C" int32_t __wrap_HcommChannelNotifyWaitOnThread(ThreadHandle t, ChannelHandle c, uint32_t n, uint32_t)
{
    tasks.push_back({Kind::CHANNEL_WAIT, t, c, n});
    return HCCL_SUCCESS;
}
extern "C" int32_t __wrap_HcommThreadNotifyRecordOnThread(ThreadHandle t, ThreadHandle dst, uint32_t n)
{
    tasks.push_back({Kind::THREAD_RECORD, t, dst, n});
    return HCCL_SUCCESS;
}
extern "C" int32_t __wrap_HcommThreadNotifyWaitOnThread(ThreadHandle t, uint32_t n, uint32_t)
{
    tasks.push_back({Kind::THREAD_WAIT, t, t, n});
    return HCCL_SUCCESS;
}
extern "C" int32_t __wrap_HcommLocalCopyOnThread(ThreadHandle t, void* dst, const void* src, uint64_t len)
{
    if (++copyCalls == failCopyAt) {
        return HCCL_E_RUNTIME;
    }
    if (copyResult != HCCL_SUCCESS) {
        return copyResult;
    }
    tasks.push_back({Kind::COPY, t, 0, 0, dst, src, len});
    return HCCL_SUCCESS;
}
extern "C" int32_t __wrap_HcommWriteReduceOnThread(
    ThreadHandle t, ChannelHandle c, void* dst, const void* src, uint64_t count, HcommDataType dtype, HcommReduceOp)
{
    tasks.push_back({Kind::REDUCE, t, c, 0, dst, src, count, dtype});
    return HCCL_SUCCESS;
}
extern "C" int32_t __wrap_HcommReadReduceOnThread(
    ThreadHandle, ChannelHandle, void*, const void*, uint64_t, HcommDataType, HcommReduceOp)
{
    ++readReduceCalls;
    return HCCL_SUCCESS;
}
extern "C" int32_t __wrap_HcclHcommBatchTransferOnThread(
    ThreadHandle t, ChannelHandle c, const HcclHcommBatchTransferDesc* desc, uint32_t num)
{
    ++batchCalls;
    descriptors.insert(descriptors.end(), desc, desc + num);
    batchEnds.push_back(descriptors.size());
    if (batchResult != HCCL_SUCCESS) {
        return batchResult;
    }
    for (uint32_t i = 0; i < num; ++i) {
        if (desc[i].transType == HCCL_HCOMM_TRANSFER_TYPE_WRITE_REDUCE_WITH_NOTIFY) {
            const auto& r = desc[i].transferInfo.writeReduceWithNotify;
            __wrap_HcommWriteReduceOnThread(t, c, r.dst, r.src, r.count, r.dataType, r.reduceOp);
            // Model completion notification AFTER the payload, on the same ordered thread.
            __wrap_HcommChannelNotifyRecordOnThread(t, c, r.notifyIdx);
        } else {
            EXPECT_EQ(desc[i].transType, HCCL_HCOMM_TRANSFER_TYPE_WRITE_REDUCE);
            const auto& r = desc[i].transferInfo.reduce;
            __wrap_HcommWriteReduceOnThread(t, c, r.dst, r.src, r.count, r.dataType, r.reduceOp);
        }
    }
    return HCCL_SUCCESS;
}

TEST_F(MeshChunkTest, FusesOnlyLastNonemptyDescriptorAndPreservesFields)
{
    for (const auto& counts : {std::vector<uint64_t>{9}, {3, 7, 0}, {0, 3, 0, 7, 0}}) {
        tasks.clear();
        descriptors.clear();
        ASSERT_EQ(SendRecvBatchWriteReduce(MakeTransfer(counts), 1, true), HCCL_SUCCESS);
        ASSERT_FALSE(descriptors.empty());
        for (size_t i = 0; i + 1 < descriptors.size(); ++i) {
            EXPECT_EQ(descriptors[i].transType, HCCL_HCOMM_TRANSFER_TYPE_WRITE_REDUCE);
        }
        const auto& last = descriptors.back();
        EXPECT_EQ(last.transType, HCCL_HCOMM_TRANSFER_TYPE_WRITE_REDUCE_WITH_NOTIFY);
        const auto& fused = last.transferInfo.writeReduceWithNotify;
        const auto lastCount = *std::find_if(counts.rbegin(), counts.rend(), [](uint64_t n) { return n != 0; });
        const auto offset = (std::accumulate(counts.begin(), counts.end(), uint64_t{0}) - lastCount) * sizeof(float);
        EXPECT_EQ(fused.count, lastCount);
        EXPECT_EQ(reinterpret_cast<uintptr_t>(fused.src), 0x100000U + offset);
        EXPECT_EQ(reinterpret_cast<uintptr_t>(fused.dst), 0x200000U + offset);
        EXPECT_EQ(fused.dataType, static_cast<HcommDataType>(HCCL_DATA_TYPE_FP32));
        EXPECT_EQ(fused.reduceOp, static_cast<HcommReduceOp>(HCCL_REDUCE_SUM));
        EXPECT_EQ(fused.notifyIdx, NOTIFY_IDX_DATA_SIGNAL);
        EXPECT_EQ(Count(Kind::CHANNEL_RECORD, NOTIFY_IDX_ACK), 1U);
        EXPECT_EQ(Count(Kind::CHANNEL_WAIT, NOTIFY_IDX_ACK), 1U);
        EXPECT_EQ(Count(Kind::CHANNEL_RECORD, NOTIFY_IDX_DATA_SIGNAL), 1U);
        EXPECT_EQ(Count(Kind::CHANNEL_WAIT, NOTIFY_IDX_DATA_SIGNAL), 1U);
    }
}

TEST_F(MeshChunkTest, EmptyDataStillSignalsAndDefaultPathDoesNotFuse)
{
    ASSERT_EQ(SendRecvBatchWriteReduce(MakeTransfer({0, 0}), 1, true), HCCL_SUCCESS);
    EXPECT_EQ(batchCalls, 0U);
    EXPECT_EQ(Count(Kind::CHANNEL_RECORD, NOTIFY_IDX_DATA_SIGNAL), 1U);
    ASSERT_EQ(SendRecvBatchWriteReduce(MakeTransfer({2}), 1), HCCL_SUCCESS);
    ASSERT_EQ(descriptors.size(), 1U);
    EXPECT_EQ(descriptors[0].transType, HCCL_HCOMM_TRANSFER_TYPE_WRITE_REDUCE);
}

TEST_F(MeshChunkTest, OldInterfaceFallbackAndBatchErrorNeverResubmit)
{
    batchSupported = false;
    ASSERT_EQ(SendRecvBatchWriteReduce(MakeTransfer({3}), 1, true), HCCL_SUCCESS);
    EXPECT_EQ(batchCalls, 0U);
    EXPECT_EQ(Count(Kind::CHANNEL_RECORD, NOTIFY_IDX_DATA_SIGNAL), 1U);
    tasks.clear();
    batchSupported = true;
    batchResult = HCCL_E_RUNTIME;
    EXPECT_EQ(SendRecvBatchWriteReduce(MakeTransfer({3}), 1, true), HCCL_E_RUNTIME);
    EXPECT_EQ(batchCalls, 1U);
    EXPECT_EQ(Count(Kind::REDUCE, 0), 0U);
    EXPECT_EQ(Count(Kind::CHANNEL_RECORD, NOTIFY_IDX_DATA_SIGNAL), 0U);
}

namespace {
// Integer-valued FP16/BF16 samples are exact; memcpy also exercises unaligned user addresses.
float ReadValue(const void* ptr, HcclDataType dtype)
{
    if (dtype == HCCL_DATA_TYPE_INT8) {
        return *static_cast<const int8_t*>(ptr);
    }
    uint32_t bits = 0;
    if (dtype == HCCL_DATA_TYPE_FP32) {
        float value;
        std::memcpy(&value, ptr, sizeof(value));
        return value;
    }
    uint16_t half;
    std::memcpy(&half, ptr, sizeof(half));
    if (dtype == HCCL_DATA_TYPE_BFP16) {
        bits = static_cast<uint32_t>(half) << 16;
    } else if (half != 0) {
        bits = ((half & 0x8000U) << 16) | (((half >> 10) & 31U) + 112U) << 23 | (half & 1023U) << 13;
    }
    float value;
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}

void WriteValue(void* ptr, HcclDataType dtype, float value)
{
    if (dtype == HCCL_DATA_TYPE_INT8) {
        *static_cast<int8_t*>(ptr) = static_cast<int8_t>(value);
        return;
    }
    if (dtype == HCCL_DATA_TYPE_FP32) {
        std::memcpy(ptr, &value, sizeof(value));
        return;
    }
    uint32_t bits;
    std::memcpy(&bits, &value, sizeof(bits));
    uint16_t half =
        dtype == HCCL_DATA_TYPE_BFP16 ?
            bits >> 16 :
            (value == 0 ? 0 :
                          ((bits >> 16) & 0x8000U) | ((((bits >> 23) & 255U) - 112U) << 10) | ((bits >> 13) & 1023U));
    std::memcpy(ptr, &half, sizeof(half));
}

struct Scenario {
    u32 ranks;
    u64 count;
    u64 stride;
    HcclDataType dtype;
    u64 elementSize;
    std::vector<std::vector<uint8_t>> inputs, outputs, scratch;
    std::vector<u32> rankList;

    Scenario(u32 n, u64 c, u64 s, HcclDataType d)
        : ranks(n),
          count(c),
          stride(s == 0 ? c : s),
          dtype(d),
          elementSize(DATATYPE_SIZE_TABLE[d]),
          inputs(n, std::vector<uint8_t>((n * stride) * elementSize + 34, 0xD7)),
          outputs(n, std::vector<uint8_t>(c * elementSize + 34, 0xD7)),
          scratch(n, std::vector<uint8_t>(c * elementSize + 34, 0xD7)),
          rankList(n)
    {
        std::iota(rankList.begin(), rankList.end(), 0);
        for (u32 src = 0; src < n; ++src) {
            for (u32 dst = 0; dst < n; ++dst) {
                for (u64 i = 0; i < c; ++i) {
                    WriteValue(In(src) + (dst * stride + i) * elementSize, d, src + 1 + dst % 3 + i % 3);
                }
            }
        }
    }
    uint8_t* In(u32 r) { return inputs[r].data() + 17; }
    uint8_t* Out(u32 r) { return outputs[r].data() + 17; }
    uint8_t* Tmp(u32 r) { return scratch[r].data() + 17; }
    OpParam Param() const
    {
        OpParam p{};
        p.opType = HCCL_CMD_REDUCE_SCATTER;
        p.engine = COMM_ENGINE_AICPU_TS;
        p.reduceType = HCCL_REDUCE_SUM;
        p.DataDes.count = count;
        p.DataDes.strideCount = stride;
        p.DataDes.dataType = dtype;
        return p;
    }
    TemplateResource Resources(u32 r)
    {
        TemplateResource res{};
        for (u32 i = 0; i < ranks - 1; ++i) {
            res.threads.push_back(1 + r * 16 + i);
        }
        for (u32 peer = 0; peer < ranks; ++peer) {
            if (peer == r) {
                continue;
            }
            ChannelInfo c{};
            c.handle = 1 + r * 64 + peer;
            c.remoteRank = peer;
            c.remoteCclMem.addr = Tmp(peer);
            c.remoteCclMem.size = count * elementSize;
            res.channels[peer] = {c};
        }
        return res;
    }
    TemplateDataParams Data(u32 r, u64 processed, u64 current)
    {
        TemplateDataParams p{};
        p.buffInfo.inputPtr = In(r);
        p.buffInfo.outputPtr = Out(r);
        p.buffInfo.hcclBuff.addr = Tmp(r);
        p.buffInfo.hcclBuff.size = count * elementSize;
        p.buffInfo.inBuffBaseOff = p.buffInfo.outBuffBaseOff = processed * elementSize;
        p.inputSliceStride = stride * elementSize; // Full rank block stride, NOT this loop's size.
        p.sliceSize = p.tailSize = current * elementSize;
        p.count = current;
        p.repeatNum = 1; // MC2 repeat is multiple ordered KernelRun/Orchestrate turns.
        return p;
    }
    void Expand(u64 loopCount)
    {
        for (u32 r = 0; r < ranks; ++r) {
            auto param = Param();
            InsTempReduceScatterMesh1DMeshChunk alg(param, r, {rankList});
            for (u64 processed = 0; processed < count; processed += loopCount) {
                ASSERT_EQ(
                    alg.KernelRun(param, Data(r, processed, std::min(loopCount, count - processed)), Resources(r)),
                    HCCL_SUCCESS);
            }
        }
    }
    void VerifyAddresses(u64 loopCount)
    {
        for (const auto& t : tasks) {
            if (t.kind != Kind::REDUCE) {
                continue;
            }
            const u32 srcRank = (t.thread - 1) / 16;
            const u32 dstRank = (t.target - 1) % 64;
            const auto srcOffset = static_cast<const uint8_t*>(t.src) - In(srcRank);
            const auto dstOffset = static_cast<uint8_t*>(t.dst) - Tmp(dstRank);
            ASSERT_GE(srcOffset, 0);
            ASSERT_GE(dstOffset, 0);
            const u64 localOffset = srcOffset - dstRank * stride * elementSize;
            ASSERT_LT(localOffset, count * elementSize);
            EXPECT_LE(localOffset + t.count * elementSize, count * elementSize);
            EXPECT_EQ(localOffset % (loopCount * elementSize), static_cast<u64>(dstOffset));
            EXPECT_LE(dstOffset + t.count * elementSize, loopCount * elementSize);
        }
    }
    void ExpandExecutor(u64 loopCount, bool defaultStride)
    {
        for (u32 r = 0; r < ranks; ++r) {
            auto param = Param();
            param.DataDes.strideCount = defaultStride ? 0 : stride;
            param.inputPtr = In(r);
            param.outputPtr = Out(r);
            AlgResourceCtxSerializable ctx{};
            ctx.topoInfo.userRank = r;
            ctx.topoInfo.userRankSize = ranks;
            ctx.algHierarchyInfo.infos = {{rankList}};
            auto resources = Resources(r);
            ctx.threads = resources.threads;
            ctx.cclMem.addr = Tmp(r);
            ctx.cclMem.size = loopCount * elementSize * (ranks - 1);
            ctx.channels.resize(1);
            for (const auto& peer : resources.channels) {
                ctx.channels[0].push_back(peer.second[0]);
            }
            auto exec = CollAlgExecRegistryV2::Instance().GetAlgExec(
                HCCL_CMD_REDUCE_SCATTER, "InsReduceScatterMesh1DMeshChunk");
            ASSERT_NE(exec, nullptr);
            ASSERT_EQ(exec->Orchestrate(param, ctx), HCCL_SUCCESS);
        }
    }
    void ExecuteAndCheck()
    {
        std::map<ThreadHandle, std::vector<Task>> queues;
        for (const auto& t : tasks) {
            queues[t.thread].push_back(t);
        }
        std::map<ThreadHandle, size_t> positions;
        // Separate namespaces for local-thread and directed-channel notifications.
        std::map<std::pair<uint64_t, uint32_t>, bool> channelSignals, threadSignals;
        size_t remaining = tasks.size();
        while (remaining != 0) {
            bool progress = false;
            for (auto& entry : queues) {
                auto& pos = positions[entry.first];
                if (pos == entry.second.size()) {
                    continue;
                }
                const auto& t = entry.second[pos];
                auto& signals =
                    (t.kind == Kind::CHANNEL_WAIT || t.kind == Kind::CHANNEL_RECORD) ? channelSignals : threadSignals;
                auto key = std::make_pair(t.target, t.notify);
                if (t.kind == Kind::CHANNEL_WAIT || t.kind == Kind::THREAD_WAIT) {
                    if (!signals[key]) {
                        continue;
                    }
                    signals[key] = false;
                } else if (t.kind == Kind::THREAD_RECORD || t.kind == Kind::CHANNEL_RECORD) {
                    if (t.kind == Kind::CHANNEL_RECORD) {
                        const auto from = (t.target - 1) / 64;
                        const auto to = (t.target - 1) % 64;
                        key.first = 1 + to * 64 + from;
                    }
                    ASSERT_FALSE(signals[key]) << "Notify overwritten before wait";
                    signals[key] = true;
                } else if (t.kind == Kind::COPY) {
                    std::memcpy(t.dst, t.src, t.count);
                } else {
                    for (u64 i = 0; i < t.count; ++i) {
                        auto* dst = static_cast<uint8_t*>(t.dst) + i * elementSize;
                        const auto* src = static_cast<const uint8_t*>(t.src) + i * elementSize;
                        WriteValue(dst, dtype, ReadValue(dst, dtype) + ReadValue(src, dtype));
                    }
                }
                ++pos;
                --remaining;
                progress = true;
            }
            ASSERT_TRUE(progress) << "Task dependency deadlock; remaining=" << remaining;
        }
        for (u32 dst = 0; dst < ranks; ++dst) {
            for (u64 i = 0; i < count; ++i) {
                float reference = 0;
                for (u32 src = 0; src < ranks; ++src) {
                    reference += ReadValue(In(src) + (dst * stride + i) * elementSize, dtype);
                }
                ASSERT_EQ(ReadValue(Out(dst) + i * elementSize, dtype), reference) << "rank=" << dst << " i=" << i;
            }
            for (u64 i = 0; i < 17; ++i) {
                EXPECT_EQ(outputs[dst][i], 0xD7);
                EXPECT_EQ(outputs[dst][outputs[dst].size() - 1 - i], 0xD7);
                EXPECT_EQ(scratch[dst][i], 0xD7);
                EXPECT_EQ(scratch[dst][scratch[dst].size() - 1 - i], 0xD7);
            }
        }
    }
};
} // namespace

TEST_F(MeshChunkTest, ChunkBoundariesConserveDataAndPreserveStridedMultiLoopAddresses)
{
    for (auto dtype : {HCCL_DATA_TYPE_INT8, HCCL_DATA_TYPE_FP16, HCCL_DATA_TYPE_BFP16, HCCL_DATA_TYPE_FP32}) {
        for (u32 ranks : {2U, 4U, 8U}) {
            const u64 e = DATATYPE_SIZE_TABLE[dtype];
            const u64 boundary = (ranks - 1) * 4096 / e;
            for (u64 count : {u64{3}, boundary - 1, boundary, boundary + 11}) {
                for (u64 stride : {u64{0}, count, count + 3}) {
                    tasks.clear();
                    descriptors.clear();
                    Scenario s(ranks, count * 2 + 1, stride == 0 ? 0 : count * 2 + 1 + stride - count, dtype);
                    s.Expand(count);
                    s.VerifyAddresses(count);
                    // Every sender/peer covers each loop exactly once, without rank padding.
                    std::map<std::pair<u32, u32>, u64> sent;
                    for (const auto& t : tasks) {
                        if (t.kind == Kind::REDUCE) {
                            sent[{(t.thread - 1) / 16, (t.target - 1) % 64}] += t.count;
                        }
                    }
                    for (const auto& pair : sent) {
                        EXPECT_EQ(pair.second, s.count);
                    }
                }
            }
        }
    }
}

TEST_F(MeshChunkTest, InvalidLayoutAndPreCopyFailureSubmitNoCommunication)
{
    Scenario s(8, 16000, 16003, HCCL_DATA_TYPE_FP16);
    auto param = s.Param();
    auto data = s.Data(0, 0, s.count);
    auto res = s.Resources(0);
    InsTempReduceScatterMesh1DMeshChunk alg(param, 0, {s.rankList});
    data.inputSliceStride = UINT64_MAX;
    EXPECT_EQ(alg.KernelRun(param, data, res), HCCL_E_PARA);
    EXPECT_TRUE(tasks.empty());
    data = s.Data(0, 0, s.count);
    res.channels.erase(1);
    EXPECT_EQ(alg.KernelRun(param, data, res), HCCL_E_PARA);
    EXPECT_TRUE(tasks.empty());
    res = s.Resources(0);
    copyResult = HCCL_E_RUNTIME;
    EXPECT_EQ(alg.KernelRun(param, data, res), HCCL_E_RUNTIME);
    EXPECT_TRUE(tasks.empty());
    EXPECT_EQ(batchCalls, 0U);
}

TEST_F(MeshChunkTest, CpuReferenceForPaddedInputsMultipleLoopsAndOrderedTurns)
{
    for (auto dtype : {HCCL_DATA_TYPE_INT8, HCCL_DATA_TYPE_FP16, HCCL_DATA_TYPE_BFP16, HCCL_DATA_TYPE_FP32}) {
        for (u32 ranks : {2U, 4U, 8U}) {
            const u64 loopCount = (ranks - 1) * 4096 / DATATYPE_SIZE_TABLE[dtype];
            const u64 count = loopCount * 2 + 11;
            for (u64 stride : {u64{0}, count, count + 3}) {
                Scenario s(ranks, count, stride, dtype);
                const auto originalInputs = s.inputs;
                for (u32 turn = 0; turn < 2; ++turn) {
                    tasks.clear();
                    descriptors.clear();
                    s.ExpandExecutor(loopCount, stride == 0);
                    s.VerifyAddresses(loopCount);
                    s.ExecuteAndCheck();
                    EXPECT_EQ(s.inputs, originalInputs); // padding is never written or included in reduction.
                }
            }
        }
    }
}

TEST_F(MeshChunkTest, EveryIntermediateStepJoinsBeforeReleaseAndUsesOnlyAlgorithmNotifySlots)
{
    for (u32 ranks : {2U, 4U, 8U}) {
        tasks.clear();
        Scenario s(ranks, 20000, 20003, HCCL_DATA_TYPE_FP16);
        s.Expand(s.count);
        const u32 slaves = ranks - 2;
        for (const auto& t : tasks) {
            if (t.kind == Kind::THREAD_WAIT || t.kind == Kind::THREAD_RECORD) {
                const u32 targetSlot = (t.target - 1) % 16;
                EXPECT_LT(t.notify, targetSlot == 0 ? 2 * slaves : 2U);
            }
        }
        for (u32 r = 0; r < ranks; ++r) {
            std::vector<Task> main;
            for (const auto& t : tasks) {
                if (t.thread == 1 + r * 16) {
                    main.push_back(t);
                }
            }
            u32 step = 0;
            for (size_t pos = 0; pos < main.size(); ++pos) {
                if (main[pos].kind != Kind::CHANNEL_WAIT || main[pos].notify != NOTIFY_IDX_DATA_SIGNAL) {
                    continue;
                }
                const bool intermediate = step++ < ranks - 2;
                for (u32 i = 0; i < slaves; ++i) {
                    ASSERT_LT(pos + 1 + i, main.size());
                    EXPECT_EQ(main[pos + 1 + i].kind, Kind::THREAD_WAIT);
                    EXPECT_EQ(main[pos + 1 + i].notify, intermediate ? i + slaves : i);
                }
                if (intermediate) {
                    for (u32 i = 0; i < slaves; ++i) {
                        ASSERT_LT(pos + 1 + slaves + i, main.size());
                        EXPECT_EQ(main[pos + 1 + slaves + i].kind, Kind::THREAD_RECORD);
                        EXPECT_EQ(main[pos + 1 + slaves + i].notify, 1U);
                    }
                }
            }
            EXPECT_EQ(step, ranks - 1);
        }
        s.ExecuteAndCheck();
    }
}

TEST_F(MeshChunkTest, TemplatePropagatesRemoteFailureWithoutPostCopy)
{
    Scenario s(8, 15000, 15003, HCCL_DATA_TYPE_FP16);
    auto p = s.Param();
    InsTempReduceScatterMesh1DMeshChunk alg(p, 0, {s.rankList});
    batchResult = HCCL_E_RUNTIME;
    EXPECT_EQ(alg.KernelRun(p, s.Data(0, 0, s.count), s.Resources(0)), HCCL_E_RUNTIME);
    EXPECT_EQ(batchCalls, 1U);
    EXPECT_EQ(Count(Kind::COPY, 0), 1U); // PreCopy only; no output task after the failure.
}

TEST_F(MeshChunkTest, AlignedChunksRoundDownAndLeaveExactTail)
{
    Scenario s(8, (7 * 4096 + 4) / 2, 15003, HCCL_DATA_TYPE_FP16);
    auto p = s.Param();
    InsTempReduceScatterMesh1DMeshChunk alg(p, 0, {s.rankList});
    ASSERT_EQ(alg.KernelRun(p, s.Data(0, 0, s.count), s.Resources(0)), HCCL_SUCCESS);
    ASSERT_EQ(descriptors.size(), 49U);
    for (size_t i = 0; i < descriptors.size(); ++i) {
        const auto& r = descriptors[i].transferInfo.writeReduceWithNotify;
        EXPECT_EQ(r.count * 2, i % 7 == 6 ? 4100U : 4096U);
        const auto peer = static_cast<const uint8_t*>(r.src) - s.In(0);
        EXPECT_EQ(static_cast<u64>(peer) % (s.stride * 2), (i % 7) * 4096);
    }
}

TEST_F(MeshChunkTest, PostCopyAndInvalidRankOrDatatypeErrorsAreNotSwallowed)
{
    Scenario s(4, 8193, 8196, HCCL_DATA_TYPE_FP32);
    auto p = s.Param();
    InsTempReduceScatterMesh1DMeshChunk empty(p, 0, {});
    RankSliceInfo slices;
    EXPECT_EQ(empty.CalcSliceInfoVec(1, slices), HCCL_E_PARA);
    InsTempReduceScatterMesh1DMeshChunk alg(p, 0, {s.rankList});
    p.DataDes.dataType = HCCL_DATA_TYPE_RESERVED;
    EXPECT_EQ(alg.KernelRun(p, s.Data(0, 0, s.count), s.Resources(0)), HCCL_E_PARA);
    EXPECT_TRUE(tasks.empty());
    p = s.Param();
    failCopyAt = 2; // The first copy is input -> scratch; the second is scratch -> output.
    EXPECT_EQ(alg.KernelRun(p, s.Data(0, 0, s.count), s.Resources(0)), HCCL_E_RUNTIME);
    EXPECT_EQ(copyCalls, 2U);
    EXPECT_EQ(batchCalls, 9U); // Already submitted work must not be retried.
}

namespace {
// Exercise the real NHR template using the same ordered primitive simulator as MeshChunk.
struct NhrScenario : Scenario {
    NhrScenario(u32 n, u64 c, u64 s, HcclDataType d) : Scenario(n, c, s, d)
    {
        for (u32 r = 0; r < ranks; ++r) {
            scratch[r].resize(ranks * count * elementSize + 34, 0xD7);
            for (u32 dst = 0; dst < ranks; ++dst) {
                for (u64 i = 0; i < count; ++i) {
                    // Keep 16-rank INT8 SUM in range.
                    WriteValue(In(r) + (dst * stride + i) * elementSize, dtype, 1 + (r + dst + i) % 3);
                }
            }
        }
    }
    TemplateResource NhrResources(u32 r)
    {
        auto res = Resources(r);
        res.threads.resize(1);
        for (auto& peer : res.channels) {
            peer.second[0].portGroupSize = 1;
            peer.second[0].protocol = CommProtocol::COMM_PROTOCOL_UBC_CTP;
            peer.second[0].remoteCclMem.size = ranks * count * elementSize;
        }
        return res;
    }
    TemplateDataParams NhrData(u32 r, u64 processed, u64 current)
    {
        auto data = Data(r, processed, current);
        data.buffInfo.inBuffType = BufferType::INPUT;
        data.buffInfo.outBuffType = BufferType::OUTPUT;
        data.buffInfo.hcclBuffType = BufferType::HCCL_BUFFER;
        data.buffInfo.hcclBuff.size = ranks * count * elementSize;
        data.outputSliceStride = 0;
        return data;
    }
    void ExpandNhr(u64 loopCount)
    {
        for (u32 r = 0; r < ranks; ++r) {
            auto param = Param();
            InsTempReduceScatterNHR alg(param, r, {rankList});
            for (u64 processed = 0; processed < count; processed += loopCount) {
                ASSERT_EQ(
                    alg.KernelRun(
                        param, NhrData(r, processed, std::min(loopCount, count - processed)), NhrResources(r)),
                    HCCL_SUCCESS);
            }
        }
    }
    void CheckTransfers()
    {
        for (const auto& task : tasks) {
            if (task.kind != Kind::REDUCE) {
                continue;
            }
            const u32 srcRank = (task.thread - 1) / 16;
            const u32 dstRank = (task.target - 1) % 64;
            const auto src = reinterpret_cast<uintptr_t>(task.src);
            const auto dst = reinterpret_cast<uintptr_t>(task.dst);
            const auto input = reinterpret_cast<uintptr_t>(In(srcRank));
            const auto tmp = reinterpret_cast<uintptr_t>(Tmp(srcRank));
            const auto remote = reinterpret_cast<uintptr_t>(Tmp(dstRank));
            const auto bytes = task.count * elementSize;
            ASSERT_EQ(task.dtype, static_cast<HcommDataType>(dtype));
            ASSERT_GE(dst, remote);
            ASSERT_LE(dst - remote + bytes, ranks * count * elementSize);
            if (src >= input && src < input + ranks * stride * elementSize) {
                // Step zero reads user data directly; no padding may be reduced.
                ASSERT_LE((src - input) % (stride * elementSize) + bytes, count * elementSize);
            } else {
                ASSERT_GE(src, tmp);
                ASSERT_LE(src - tmp + bytes, ranks * count * elementSize);
            }
        }
        size_t begin = 0;
        for (auto end : batchEnds) {
            ASSERT_GT(end, begin);
            for (size_t i = begin; i + 1 < end; ++i) {
                EXPECT_EQ(descriptors[i].transType, HCCL_HCOMM_TRANSFER_TYPE_WRITE_REDUCE);
                EXPECT_EQ(descriptors[i].transferInfo.reduce.reduceOp, static_cast<HcommReduceOp>(HCCL_REDUCE_SUM));
            }
            const auto& last = descriptors[end - 1];
            ASSERT_EQ(last.transType, HCCL_HCOMM_TRANSFER_TYPE_WRITE_REDUCE_WITH_NOTIFY);
            EXPECT_EQ(last.transferInfo.writeReduceWithNotify.notifyIdx, NOTIFY_IDX_DATA_SIGNAL);
            EXPECT_EQ(last.transferInfo.writeReduceWithNotify.reduceOp, static_cast<HcommReduceOp>(HCCL_REDUCE_SUM));
            begin = end;
        }
        // The stub expands each fused descriptor as payload then notification.
        // A redundant standalone DATA_SIGNAL would make this count exceed the batch count.
        EXPECT_EQ(Count(Kind::CHANNEL_RECORD, NOTIFY_IDX_DATA_SIGNAL), batchEnds.size());
        EXPECT_EQ(Count(Kind::CHANNEL_WAIT, NOTIFY_IDX_DATA_SIGNAL), batchEnds.size());
        EXPECT_EQ(Count(Kind::CHANNEL_RECORD, NOTIFY_IDX_ACK), batchEnds.size());
        EXPECT_EQ(Count(Kind::CHANNEL_WAIT, NOTIFY_IDX_ACK), batchEnds.size());
    }
};
} // namespace

TEST_F(MeshChunkTest, NhrFusesRealTemplateForSoleAndInterServerRanksWithStridedLoopsAndTurns)
{
    for (auto dtype : {HCCL_DATA_TYPE_INT8, HCCL_DATA_TYPE_FP16, HCCL_DATA_TYPE_BFP16, HCCL_DATA_TYPE_FP32}) {
        for (u32 ranks : {2U, 16U}) {
            for (u64 stride : {u64{0}, u64{43}, u64{46}}) {
                NhrScenario s(ranks, 43, stride, dtype);
                const auto original = s.inputs;
                for (u32 turn = 0; turn < 2; ++turn) {
                    tasks.clear();
                    descriptors.clear();
                    batchEnds.clear();
                    s.ExpandNhr(17); // Three loops, including a short final loop.
                    const u32 steps = ranks == 2 ? 1 : 4;
                    ASSERT_EQ(batchEnds.size(), ranks * steps * 3U);
                    s.CheckTransfers();
                    s.ExecuteAndCheck(); // Checks dependency ordering and actual reduction values.
                    EXPECT_EQ(s.inputs, original);
                }
            }
        }
    }
}

TEST_F(MeshChunkTest, NhrBatchesParallelRepeatsAndSkipsEmptyTemplate)
{
    NhrScenario s(2, 40, 43, HCCL_DATA_TYPE_FP32);
    for (auto& input : s.inputs) {
        input.resize(16 * s.stride * s.elementSize + 34, 0xD7);
    }
    auto param = s.Param();
    InsTempReduceScatterNHR alg(param, 0, {s.rankList});
    auto data = s.NhrData(0, 0, 0);
    ASSERT_EQ(alg.KernelRun(param, data, s.NhrResources(0)), HCCL_SUCCESS);
    EXPECT_TRUE(tasks.empty());
    data = s.NhrData(0, 0, 5);
    data.repeatNum = 8; // ParallelMeshNHR inter-server stage batches eight local-rank slices.
    data.inputSliceStride = 8 * s.stride * s.elementSize;
    data.inputRepeatStride = s.stride * s.elementSize;
    data.outputRepeatStride = 2 * data.sliceSize;
    data.buffInfo.outputPtr = s.Tmp(0);
    data.buffInfo.outBuffType = BufferType::HCCL_BUFFER;
    data.outputSliceStride = data.sliceSize;
    ASSERT_EQ(alg.KernelRun(param, data, s.NhrResources(0)), HCCL_SUCCESS);
    ASSERT_EQ(batchCalls, 1U);
    ASSERT_EQ(descriptors.size(), 8U);
    for (size_t i = 0; i < descriptors.size(); ++i) {
        const auto& d = descriptors[i];
        const auto src =
            i + 1 == descriptors.size() ? d.transferInfo.writeReduceWithNotify.src : d.transferInfo.reduce.src;
        const auto dst =
            i + 1 == descriptors.size() ? d.transferInfo.writeReduceWithNotify.dst : d.transferInfo.reduce.dst;
        // Rank 0 sends the other rank's block in step zero; compare exact stride/repeat offsets.
        EXPECT_EQ(src, s.In(0) + data.inputSliceStride + i * data.inputRepeatStride);
        EXPECT_EQ(dst, s.Tmp(1) + data.sliceSize + i * data.outputRepeatStride);
        EXPECT_EQ(
            d.transType, i + 1 == descriptors.size() ? HCCL_HCOMM_TRANSFER_TYPE_WRITE_REDUCE_WITH_NOTIFY :
                                                       HCCL_HCOMM_TRANSFER_TYPE_WRITE_REDUCE);
    }
    EXPECT_EQ(Count(Kind::CHANNEL_RECORD, NOTIFY_IDX_DATA_SIGNAL), 1U);
    EXPECT_EQ(Count(Kind::CHANNEL_WAIT, NOTIFY_IDX_DATA_SIGNAL), 1U);
}

TEST_F(MeshChunkTest, NhrOldInterfaceAndFailedBatchNeverReplayCommunication)
{
    NhrScenario s(16, 7, 10, HCCL_DATA_TYPE_FP32);
    auto param = s.Param();
    InsTempReduceScatterNHR alg(param, 0, {s.rankList});
    batchSupported = false;
    ASSERT_EQ(alg.KernelRun(param, s.NhrData(0, 0, s.count), s.NhrResources(0)), HCCL_SUCCESS);
    EXPECT_EQ(batchCalls, 0U);
    EXPECT_EQ(Count(Kind::CHANNEL_RECORD, NOTIFY_IDX_DATA_SIGNAL), 4U);
    tasks.clear();
    batchSupported = true;
    batchResult = HCCL_E_RUNTIME;
    EXPECT_EQ(alg.KernelRun(param, s.NhrData(0, 0, s.count), s.NhrResources(0)), HCCL_E_INTERNAL);
    EXPECT_EQ(batchCalls, 1U);
    EXPECT_EQ(Count(Kind::REDUCE, 0), 0U);
    EXPECT_EQ(Count(Kind::CHANNEL_RECORD, NOTIFY_IDX_DATA_SIGNAL), 0U);
    for (const auto& task : tasks) {
        if (task.kind == Kind::COPY) {
            EXPECT_NE(task.dst, s.Out(0));
        }
    }
}

TEST_F(MeshChunkTest, NhrPcieReadReduceDoesNotUseWriteFusion)
{
    NhrScenario s(2, 7, 10, HCCL_DATA_TYPE_FP32);
    auto param = s.Param();
    auto resources = s.NhrResources(0);
    for (auto& peer : resources.channels) {
        peer.second[0].protocol = CommProtocol::COMM_PROTOCOL_PCIE;
    }
    InsTempReduceScatterNHR alg(param, 0, {s.rankList});
    ASSERT_EQ(alg.KernelRun(param, s.NhrData(0, 0, s.count), resources), HCCL_SUCCESS);
    EXPECT_EQ(readReduceCalls, 1U);
    EXPECT_EQ(batchCalls, 0U);
    EXPECT_TRUE(descriptors.empty());
}
