/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_all_to_all_v_concur_adapter.h"

#include "ccu_all_to_all_v.h"
#include "ccu_kernel_all_to_all_v_mesh1d_multi_jetty.h"
#include "ccu_primitives_stub.h"
#include "ccu_kernel_alg_base.h"
#include "alg_param.h"

#include <algorithm>
#include <limits>

namespace HcclSim {
namespace CcuSt {
namespace {

// 切分口径与 AIV 侧 CcuPrepareForConcurrentAllToAllV 一致（复刻 hccl
// SplitA2ASendRecvInfo/AddTaskArgA2AInfo）：100:96、元素粒度整除、jetty 恒 1
constexpr uint64_t CONCUR_CLOS_BANDWIDTH = 96U;
constexpr uint64_t CONCUR_TOTAL_BANDWIDTH = 196U;
constexpr uint64_t CONCUR_JETTY_NUM = 1U;
constexpr uint64_t CONCUR_SPLIT_ALIGNMENT = 128U;
constexpr uint64_t CONCUR_BLOCK_SIZE = 256ULL * 1024ULL * 1024ULL;
constexpr uint64_t MEM_SLICE = 32ULL * 1024ULL;
constexpr uint64_t LOOP_SIZE = 8ULL * MEM_SLICE;

uint64_t PackParallelParameters(uint64_t repeatNum, uint64_t repeatLoopIndex, uint64_t totalLoopNum)
{
    constexpr uint64_t mask = 0x7f;
    return ((repeatNum & mask) << 55U) | ((repeatLoopIndex & mask) << 48U) | ((totalLoopNum & mask) << 41U);
}

std::array<uint64_t, 4> CalculateConcurGoSize(uint64_t size)
{
    const uint64_t fullLoops = size / LOOP_SIZE;
    const uint64_t remaining = size % LOOP_SIZE;
    const uint64_t slices = remaining / MEM_SLICE;
    const uint64_t residual = remaining % MEM_SLICE;
    std::array<uint64_t, 4> result{{fullLoops * LOOP_SIZE, fullLoops, 0, 0}};
    if (slices != 0 && residual == 0) {
        result[2] = PackParallelParameters(slices - 1, 0, 1);
        result[3] = MEM_SLICE;
    } else if (slices == 0 && residual != 0) {
        result[2] = PackParallelParameters(0, 0, 1);
        result[3] = residual;
    } else if (slices != 0) {
        result[2] = PackParallelParameters(slices - 1, 1, 2);
        result[3] = residual;
    }
    return result;
}

// 复刻 AssembleA2avMjExt（hccl AddTaskArgA2AInfo 的设备侧公式，jetty=1）
void FillConcurA2avExt(
    std::vector<uint64_t>& ext, uint32_t peer, uint64_t sendCount, uint64_t sendDispls, uint64_t recvCount,
    uint64_t recvDispls, uint64_t dataSize)
{
    const uint64_t sendLength = sendCount * dataSize;
    const uint64_t fullBlockCount = sendLength / CONCUR_BLOCK_SIZE;
    const uint32_t base = peer * CONCUR_A2AV_EXT_FIELD_COUNT;
    ext[base] = 0;                                                           // sliceSize（jetty=1 恒 0）
    ext[base + 1] = sendLength >= CONCUR_BLOCK_SIZE ? CONCUR_BLOCK_SIZE : 0; // tailSliceSize
    const uint64_t lastBlockSize = sendLength % CONCUR_BLOCK_SIZE;
    ext[base + 2] = 0;             // lastSliceSize（jetty=1 恒 0）
    ext[base + 3] = lastBlockSize; // lastTailSliceSize
    ext[base + 4] = std::numeric_limits<uint64_t>::max() - (fullBlockCount + 1U) * CONCUR_JETTY_NUM; // loopNum
    ext[base + 5] = sendDispls * dataSize; // sendOffset（repeatIndex 恒 0）
    ext[base + 6] = recvDispls * dataSize; // recvOffset
    const auto tailGoSize = CalculateConcurGoSize(lastBlockSize);
    ext[base + 7] = tailGoSize[0];
    ext[base + 8] = tailGoSize[1];
    ext[base + 9] = tailGoSize[2];
    ext[base + 10] = tailGoSize[3];
}

void* ConvertConcurrentAllToAllVArg(const void* productArg, uint32_t)
{
    if (productArg == nullptr) {
        return nullptr;
    }
    const auto* source = static_cast<const mc2_ops_hccl::CcuKernelArgAllToAllVMesh1DMultiJetty*>(productArg);
    if (source->subCommRanks.empty()) {
        return nullptr;
    }
    auto* config = new ConcurA2avKernelConfig();
    config->rankId = source->rankId;
    config->rankSize = static_cast<uint32_t>(source->rankSize);
    config->jettyNums = source->jettyNums;
    for (uint32_t i = 0; i < source->channelCount; ++i) {
        config->channels.push_back(source->channels[i]);
    }
    return config;
}

bool g_registered = []() {
    RegisterCaptureFunction(
        "CcuKernelAllToAllVMesh1DMultiJetty", CaptureConcurrentAllToAllVMultiJettyKernel, ConvertConcurrentAllToAllVArg,
        [](void* pointer) { delete static_cast<ConcurA2avKernelConfig*>(pointer); });
    CcuStAllToAllV::RegisterVariant("CcuSchedAllToAllVSoleMeshConcurrent", BuildConcurrentAllToAllVScenario);
    return true;
}();

} // namespace

Result CaptureConcurrentAllToAllVMultiJettyKernel(void* kernelArg)
{
    if (kernelArg == nullptr) {
        return Result::PARAM_ERROR;
    }
    const auto& config = *static_cast<ConcurA2avKernelConfig*>(kernelArg);
    if (config.rankSize < 2 || config.rankId >= config.rankSize || config.jettyNums.size() != config.rankSize ||
        config.channels.size() != config.rankSize - 1) {
        return Result::PARAM_ERROR;
    }
    std::array<AscendC::ccu::Variable, CONCUR_A2AV_TASK_ARG_COUNT> arguments;
    for (uint32_t index = 0; index < arguments.size(); ++index) {
        if (AscendC::ccu::LoadArg(arguments[index], index) != Result::SUCCESS) {
            return Result::CONTRACT_ERROR;
        }
    }
    CompilerContext::Current().ConfigureProgram(config.rankId, config.rankSize, config.channels);
    const CcuResult result = mc2_ops_hccl::CcuAllToAllVMesh1DMultiJettyKernel(
        arguments[0], arguments[1], arguments[2], arguments[3], arguments[4], arguments[5], arguments[6], arguments[7],
        arguments[8], arguments[9], config.channels.data(), static_cast<uint32_t>(config.channels.size()),
        config.rankSize, config.rankId, config.jettyNums.data());
    return result == CCU_SUCCESS ? Result::SUCCESS : Result::CONTRACT_ERROR;
}

std::vector<uint64_t> PrepareConcurA2avTaskArgs(const ConcurA2avMissionLaunchConfig& config)
{
    return {config.inputAddress,
            config.outputAddress,
            config.token,
            config.sourceBaseOffset,
            config.destinationBaseOffset,
            config.goSize[0],
            config.goSize[1],
            config.goSize[2],
            config.goSize[3],
            config.parameterAddress};
}

ScenarioData BuildConcurrentAllToAllVScenario(const CcuStScenario& scenario, const std::vector<KernelHandle>& handles)
{
    (void)handles; // 双 mission handle 经 CcuStFixture::LastHandleGroups() 获取
    const uint32_t rankSize = CcuStFixture::CountRanks(scenario.topoMeta);
    const uint64_t typeSize = DATATYPE_SIZE_TABLE[scenario.dataType];
    constexpr uint64_t guard = 32;

    ScenarioData data;
    data.memories.resize(rankSize);
    data.launches.resize(rankSize);
    data.srcOffsets.assign(rankSize, std::vector<uint64_t>(rankSize, 0));
    data.dstOffsets.assign(rankSize, std::vector<uint64_t>(rankSize, 0));

    // 元素粒度计数/displs（与 fixture PrepareOpParam 同口径）
    std::vector<std::vector<uint64_t>> sendCounts(rankSize, std::vector<uint64_t>(rankSize, 0));
    std::vector<std::vector<uint64_t>> recvCounts(rankSize, std::vector<uint64_t>(rankSize, 0));
    std::vector<std::vector<uint64_t>> sendDispls(rankSize, std::vector<uint64_t>(rankSize, 0));
    std::vector<std::vector<uint64_t>> recvDispls(rankSize, std::vector<uint64_t>(rankSize, 0));
    for (uint32_t rank = 0; rank < rankSize; ++rank) {
        uint64_t sendOffset = 0;
        uint64_t recvOffset = 0;
        for (uint32_t peer = 0; peer < rankSize; ++peer) {
            sendCounts[rank][peer] = scenario.sizes[rank][peer] / typeSize;
            recvCounts[rank][peer] = scenario.sizes[peer][rank] / typeSize;
            sendDispls[rank][peer] = sendOffset;
            recvDispls[rank][peer] = recvOffset;
            sendOffset += sendCounts[rank][peer];
            recvOffset += recvCounts[rank][peer];
        }
    }

    const uint64_t extFieldBytes = CONCUR_A2AV_EXT_FIELD_COUNT * sizeof(uint64_t);
    for (uint32_t rank = 0; rank < rankSize; ++rank) {
        RankMemory& memory = data.memories[rank];
        uint64_t inputBytes = guard;
        uint64_t outputBytes = guard;
        for (uint32_t peer = 0; peer < rankSize; ++peer) {
            inputBytes += scenario.sizes[rank][peer];
            outputBytes += scenario.sizes[peer][rank];
        }
        memory.input.assign(inputBytes, 0x5a);
        memory.output.assign(outputBytes, 0xa5);
        // 双 mission 各一份 ext 数组（mission0=mesh 优先链路，mission1=clos 链路）
        memory.parameter.resize(2U * rankSize * CONCUR_A2AV_EXT_FIELD_COUNT);

        std::vector<uint64_t> meshExt(rankSize * CONCUR_A2AV_EXT_FIELD_COUNT, 0);
        std::vector<uint64_t> closExt(rankSize * CONCUR_A2AV_EXT_FIELD_COUNT, 0);
        for (uint32_t peer = 0; peer < rankSize; ++peer) {
            const uint64_t size = scenario.sizes[rank][peer];
            // 切分复刻 AIV prepare：head（96/196，元素整除）→ mission0，余量 → mission1
            const uint64_t meshSendCount = sendCounts[rank][peer] / CONCUR_TOTAL_BANDWIDTH * CONCUR_CLOS_BANDWIDTH;
            const uint64_t meshRecvCount = recvCounts[rank][peer] / CONCUR_TOTAL_BANDWIDTH * CONCUR_CLOS_BANDWIDTH;
            FillConcurA2avExt(
                meshExt, peer, meshSendCount, sendDispls[rank][peer], meshRecvCount, recvDispls[rank][peer], typeSize);
            FillConcurA2avExt(
                closExt, peer, sendCounts[rank][peer] - meshSendCount, sendDispls[rank][peer] + meshSendCount,
                recvCounts[rank][peer] - meshRecvCount, recvDispls[rank][peer] + meshRecvCount, typeSize);

            data.srcOffsets[rank][peer] = guard + sendDispls[rank][peer] * typeSize;
            data.dstOffsets[rank][peer] = guard + recvDispls[peer][rank] * typeSize;
            for (uint64_t byte = 0; byte < size; ++byte) {
                memory.input[data.srcOffsets[rank][peer] + byte] =
                    static_cast<uint8_t>(17U + rank * 41U + peer * 13U + byte);
            }
        }
        std::copy(meshExt.begin(), meshExt.end(), memory.parameter.begin());
        std::copy(closExt.begin(), closExt.end(), memory.parameter.begin() + rankSize * CONCUR_A2AV_EXT_FIELD_COUNT);

        ConcurA2avMissionLaunchConfig config;
        config.inputAddress = memory.InputAddress();
        config.outputAddress = memory.OutputAddress();
        config.token = 0x1000U + rank;
        config.sourceBaseOffset = guard;
        config.destinationBaseOffset = guard;
        config.goSize = CalculateConcurGoSize(CONCUR_BLOCK_SIZE);

        const auto& rankHandles = CcuStFixture::LastHandleGroups()[rank];
        if (rankHandles.size() < 2U) {
            continue;
        }
        config.parameterAddress = memory.ParameterAddress();
        data.launches[rank] = RankLaunch{rankHandles[0], PrepareConcurA2avTaskArgs(config), &memory};
        config.parameterAddress = memory.ParameterAddress() + rankSize * extFieldBytes;
        data.launches2.resize(rankSize);
        data.launches2[rank] = RankLaunch{rankHandles[1], PrepareConcurA2avTaskArgs(config), &memory};
    }
    return data;
}

} // namespace CcuSt
} // namespace HcclSim
