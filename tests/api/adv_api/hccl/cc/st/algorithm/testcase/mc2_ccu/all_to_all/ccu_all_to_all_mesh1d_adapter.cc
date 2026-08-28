/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_all_to_all_mesh1d_adapter.h"
#include "ccu_all_to_all.h"

#include "ccu_kernel_all_to_all_mesh1d.h"
#include "ccu_primitives_stub.h"
#include "ccu_kernel_alg_base.h"
#include "alg_param.h"

namespace HcclSim {
namespace CcuSt {

namespace {

void* ConvertAllToAllArg(const void* productArg, uint32_t argNum)
{
    if (productArg == nullptr) {
        return nullptr;
    }
    const auto* src = static_cast<const mc2_ops_hccl::CcuKernelArgAlltoAllMesh1D*>(productArg);
    auto* config = new AllToAllKernelConfig();
    config->rankId = src->rankId;
    config->rankSize = static_cast<uint32_t>(src->rankSize);
    config->loadFromMem = src->loadFromMem;
    for (uint32_t i = 0; i < src->channelCount; ++i) {
        config->channels.push_back(src->channels[i]);
    }
    return config;
}

bool g_registered = []() {
    RegisterCaptureFunction("CcuKernelAlltoAllMesh1D", CaptureAllToAllMesh1DKernel, ConvertAllToAllArg, [](void* p) {
        delete static_cast<AllToAllKernelConfig*>(p);
    });

    CcuStAllToAll::RegisterVariant(
        "CcuAlltoAllMesh1D",
        [](const CcuStScenario& scenario, const std::vector<KernelHandle>& handles) -> ScenarioData {
            return BuildMesh1DScenario(scenario, handles);
        });
    return true;
}();
} // namespace

Result CaptureAllToAllMesh1DKernel(void* kernelArg)
{
    if (kernelArg == nullptr) {
        return Result::PARAM_ERROR;
    }
    const auto& config = *static_cast<AllToAllKernelConfig*>(kernelArg);
    if (config.rankSize < 2 || config.rankId >= config.rankSize || config.channels.size() != config.rankSize - 1) {
        return Result::PARAM_ERROR;
    }

    std::array<AscendC::ccu::Variable, ALL_TO_ALL_TASK_ARG_COUNT> arguments;
    for (uint32_t index = 0; index < arguments.size(); ++index) {
        if (AscendC::ccu::LoadArg(arguments[index], index) != Result::SUCCESS) {
            return Result::CONTRACT_ERROR;
        }
    }
    CompilerContext::Current().ConfigureProgram(config.rankId, config.rankSize, config.channels);

    const CcuResult result = mc2_ops_hccl::CcuAlltoAllMesh1DKernel(
        arguments[0], arguments[1], arguments[2], arguments[3], arguments[4], arguments[5], arguments[6], arguments[7],
        arguments[8], arguments[9], arguments[10], config.channels.data(),
        static_cast<uint32_t>(config.channels.size()), config.rankSize, config.rankId, config.loadFromMem);
    return result == CCU_SUCCESS ? Result::SUCCESS : Result::CONTRACT_ERROR;
}

std::vector<uint64_t> PrepareAllToAllTaskArgs(const AllToAllLaunchConfig& config)
{
    return {config.inputAddress, config.outputAddress, config.token,     config.sliceSize,
            config.srcStride,    config.srcOffset,     config.dstOffset, config.goSize[0],
            config.goSize[1],    config.goSize[2],     config.goSize[3]};
}

ScenarioData BuildMesh1DScenario(const CcuStScenario& scenario, const std::vector<KernelHandle>& handles)
{
    const uint32_t rankSize = CcuStFixture::CountRanks(scenario.topoMeta);
    const uint64_t sliceSize = scenario.sizes[0][0];
    constexpr uint64_t sourceBaseOffset = 7;
    constexpr uint64_t destinationBaseOffset = 11;
    constexpr uint64_t guard = 32;

    ScenarioData data;
    data.memories.resize(rankSize);
    data.launches.resize(rankSize);
    std::vector<AllToAllLaunchConfig> mesh1dConfigs(rankSize);
    data.srcOffsets.assign(rankSize, std::vector<uint64_t>(rankSize, 0));
    data.dstOffsets.assign(rankSize, std::vector<uint64_t>(rankSize, 0));

    const uint64_t inputSize = sourceBaseOffset + rankSize * sliceSize + guard;
    const uint64_t outputSize = destinationBaseOffset + rankSize * sliceSize + guard;

    mc2_ops_hccl::LoopGroupConfig goConfig{};
    goConfig.msInterleave = mc2_ops_hccl::CCU_MS_INTERLEAVE;
    goConfig.loopCount = mc2_ops_hccl::CCU_MS_LOCAL_COPY_LOOP_COUNT;
    goConfig.memSlice = mc2_ops_hccl::LOCAL_COPY_MS_PER_LOOP * mc2_ops_hccl::CCU_MS_SIZE;
    const auto goSize = mc2_ops_hccl::CalGoSize(sliceSize, goConfig);

    for (uint32_t rank = 0; rank < rankSize; ++rank) {
        RankMemory& memory = data.memories[rank];
        memory.input.assign(inputSize, 0x5a);
        memory.output.assign(outputSize, 0xa5);
        AllToAllLaunchConfig& config = mesh1dConfigs[rank];
        config.inputAddress = memory.InputAddress() + sourceBaseOffset;
        config.outputAddress = memory.OutputAddress();
        config.token = 0x1000U + rank;
        config.sliceSize = sliceSize;
        config.srcStride = sliceSize;
        config.srcOffset = 0;
        config.dstOffset = destinationBaseOffset + static_cast<uint64_t>(rank) * sliceSize;
        config.rankId = rank;
        config.goSize = {goSize[0], goSize[1], goSize[2], goSize[3]};
        for (uint32_t dest = 0; dest < rankSize; ++dest) {
            const uint64_t offset = sourceBaseOffset + static_cast<uint64_t>(dest) * sliceSize;
            for (uint64_t byte = 0; byte < sliceSize; ++byte) {
                memory.input[offset + byte] = static_cast<uint8_t>(17U + rank * 41U + dest * 13U + byte);
            }
            data.srcOffsets[rank][dest] = sourceBaseOffset + static_cast<uint64_t>(dest) * sliceSize;
            data.dstOffsets[rank][dest] = destinationBaseOffset + static_cast<uint64_t>(rank) * sliceSize;
        }
        data.launches[rank] = RankLaunch{handles[rank], PrepareAllToAllTaskArgs(config), &memory};
    }
    return data;
}

} // namespace CcuSt
} // namespace HcclSim
