/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_all_to_all_v_mesh1d_adapter.h"

#include "ccu_all_to_all_v.h"
#include "ccu_kernel_all_to_all_v_mesh1d.h"
#include "ccu_primitives_stub.h"
#include "ccu_temp_all_to_all_v_mesh_1D.h"

#include <algorithm>
#include <limits>

namespace HcclSim {
namespace CcuSt {
namespace {

constexpr uint64_t MEM_SLICE = 32ULL * 1024ULL;
constexpr uint64_t LOOP_SIZE = 8ULL * MEM_SLICE;

uint64_t PackParallelParameters(uint64_t repeatNum, uint64_t repeatLoopIndex, uint64_t totalLoopNum)
{
    constexpr uint64_t mask = 0x7f;
    return ((repeatNum & mask) << 55U) | ((repeatLoopIndex & mask) << 48U) | ((totalLoopNum & mask) << 41U);
}

void* ConvertAllToAllVArg(const void* productArg, uint32_t)
{
    if (productArg == nullptr) {
        return nullptr;
    }
    const auto* source = static_cast<const mc2_ops_hccl::CcuKernelArgAlltoAllVMesh1D*>(productArg);
    if (source->subCommRanks.empty()) {
        return nullptr;
    }
    auto* config = new AllToAllVKernelConfig();
    config->rankId = source->rankId;
    config->rankSize = static_cast<uint32_t>(source->rankSize);
    for (uint32_t peer : source->subCommRanks[0]) {
        if (peer != source->rankId) {
            config->channels.push_back(MakeChannel(source->rankId, peer));
        }
    }
    return config;
}

bool g_registered = []() {
    RegisterCaptureFunction(
        "CcuKernelAlltoAllVMesh1D", CaptureAllToAllVMesh1DKernel, ConvertAllToAllVArg,
        [](void* pointer) { delete static_cast<AllToAllVKernelConfig*>(pointer); });
    CcuStAllToAllV::RegisterVariant("CcuSchedAllToAllVSoleMesh", BuildAllToAllVMesh1DScenario);
    return true;
}();

} // namespace

Result CaptureAllToAllVMesh1DKernel(void* kernelArg)
{
    if (kernelArg == nullptr) {
        return Result::PARAM_ERROR;
    }
    const auto& config = *static_cast<AllToAllVKernelConfig*>(kernelArg);
    if (config.rankSize < 2 || config.rankId >= config.rankSize || config.channels.size() != config.rankSize - 1) {
        return Result::PARAM_ERROR;
    }
    std::array<AscendC::ccu::Variable, ALL_TO_ALL_V_TASK_ARG_COUNT> arguments;
    for (uint32_t index = 0; index < arguments.size(); ++index) {
        if (AscendC::ccu::LoadArg(arguments[index], index) != Result::SUCCESS) {
            return Result::CONTRACT_ERROR;
        }
    }
    CompilerContext::Current().ConfigureProgram(config.rankId, config.rankSize, config.channels);
    const CcuResult result = mc2_ops_hccl::CcuAlltoAllVMesh1DKernel(
        arguments[0], arguments[1], arguments[2], arguments[3], arguments[4], arguments[5], arguments[6], arguments[7],
        arguments[8], arguments[9], config.channels.data(), static_cast<uint32_t>(config.channels.size()),
        config.rankSize, config.rankId);
    return result == CCU_SUCCESS ? Result::SUCCESS : Result::CONTRACT_ERROR;
}

std::array<uint64_t, 4> CalculateAllToAllVGoSize(uint64_t size)
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

std::vector<uint64_t> PrepareAllToAllVTaskArgs(const AllToAllVLaunchConfig& config)
{
    return {config.inputAddress,
            config.outputAddress,
            config.token,
            config.sourceBaseOffset,
            config.destinationBaseOffset,
            config.parameterAddress,
            config.goSize[0],
            config.goSize[1],
            config.goSize[2],
            config.goSize[3]};
}

ScenarioData BuildAllToAllVMesh1DScenario(const CcuStScenario& scenario, const std::vector<KernelHandle>& handles)
{
    const uint32_t rankSize = CcuStFixture::CountRanks(scenario.topoMeta);
    constexpr uint64_t guard = 32;
    ScenarioData data;
    data.memories.resize(rankSize);
    data.launches.resize(rankSize);
    data.srcOffsets.assign(rankSize, std::vector<uint64_t>(rankSize, 0));
    data.dstOffsets.assign(rankSize, std::vector<uint64_t>(rankSize, 0));

    std::vector<std::vector<uint64_t>> sendOffsets(rankSize, std::vector<uint64_t>(rankSize, 0));
    std::vector<std::vector<uint64_t>> recvOffsets(rankSize, std::vector<uint64_t>(rankSize, 0));
    for (uint32_t rank = 0; rank < rankSize; ++rank) {
        for (uint32_t peer = 1; peer < rankSize; ++peer) {
            sendOffsets[rank][peer] = sendOffsets[rank][peer - 1] + scenario.sizes[rank][peer - 1];
            recvOffsets[rank][peer] = recvOffsets[rank][peer - 1] + scenario.sizes[peer - 1][rank];
        }
    }

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
        memory.parameter.resize(rankSize * ALL_TO_ALL_V_EXT_FIELD_COUNT);

        for (uint32_t peer = 0; peer < rankSize; ++peer) {
            const uint64_t size = scenario.sizes[rank][peer];
            const uint32_t base = peer * ALL_TO_ALL_V_EXT_FIELD_COUNT;
            memory.parameter[base] = size;
            memory.parameter[base + 1] = std::numeric_limits<uint64_t>::max() - 1U;
            memory.parameter[base + 2] = sendOffsets[rank][peer];
            memory.parameter[base + 3] = recvOffsets[peer][rank];
            data.srcOffsets[rank][peer] = guard + sendOffsets[rank][peer];
            data.dstOffsets[rank][peer] = guard + recvOffsets[peer][rank];
            for (uint64_t byte = 0; byte < size; ++byte) {
                memory.input[data.srcOffsets[rank][peer] + byte] =
                    static_cast<uint8_t>(17U + rank * 41U + peer * 13U + byte);
            }
        }

        AllToAllVLaunchConfig config;
        config.inputAddress = memory.InputAddress();
        config.outputAddress = memory.OutputAddress();
        config.token = 0x1000U + rank;
        config.sourceBaseOffset = guard;
        config.destinationBaseOffset = guard;
        config.parameterAddress = memory.ParameterAddress();
        config.goSize = CalculateAllToAllVGoSize(scenario.sizes[rank][rank]);
        data.launches[rank] = RankLaunch{handles[rank], PrepareAllToAllVTaskArgs(config), &memory};
    }
    return data;
}

} // namespace CcuSt
} // namespace HcclSim
