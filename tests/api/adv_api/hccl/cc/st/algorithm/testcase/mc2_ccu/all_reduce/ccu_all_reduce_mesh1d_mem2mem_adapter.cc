/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_all_reduce_mesh1d_mem2mem_adapter.h"
#include "ccu_all_reduce.h"

#include "ccu_kernel_kfc_all_reduce_mesh1d_mem2mem.h"
#include "ccu_kernel_alg_base.h"
#include "ccu_primitives_stub.h"
#include "alg_param.h"

#include <limits>

namespace HcclSim {
namespace CcuSt {

namespace {

void* ConvertAllReduceArg(const void* productArg, uint32_t)
{
    if (productArg == nullptr) {
        return nullptr;
    }
    const auto* src = static_cast<const mc2_ops_hccl::CcuKernelArgAllReduceKfc*>(productArg);
    auto* config = new AllReduceKernelConfig();
    config->rankId = src->rankId;
    config->rankSize = static_cast<uint32_t>(src->rankSize);
    config->dataType = src->opParam.DataDes.dataType;
    config->outputType = src->opParam.DataDes.outputType;
    if (config->outputType == HcclDataType::HCCL_DATA_TYPE_RESERVED) {
        config->outputType = config->dataType;
    }
    config->reduceOp = src->opParam.reduceType;
    for (uint32_t i = 0; i < src->channelCount; ++i) {
        config->channels.push_back(src->channels[i]);
    }
    return config;
}

bool g_registered = []() {
    RegisterCaptureFunction(
        "CcuKernelAllReduceMesh1DMem2Mem", CaptureAllReduceMeshMem2Mem1DKernel, ConvertAllReduceArg,
        [](void* p) { delete static_cast<AllReduceKernelConfig*>(p); });

    CcuStAllReduce::RegisterVariant(
        "CcuSchedAllReduceSoleMesh",
        [](const CcuStScenario& scenario, const std::vector<KernelHandle>& handles) -> ScenarioData {
            return BuildAllReduceMesh1DScenario(scenario, handles);
        });
    return true;
}();

uint64_t RoundUpAligned(uint64_t dataSize, uint64_t unitSize)
{
    if (unitSize == 0) {
        return dataSize;
    }
    return (dataSize + unitSize - 1) / unitSize * unitSize;
}

} // namespace

Result CaptureAllReduceMeshMem2Mem1DKernel(void* kernelArg)
{
    if (kernelArg == nullptr) {
        return Result::PARAM_ERROR;
    }
    const auto& config = *static_cast<AllReduceKernelConfig*>(kernelArg);
    if (config.rankSize < 2 || config.rankId >= config.rankSize || config.channels.size() != config.rankSize - 1) {
        return Result::PARAM_ERROR;
    }

    std::array<AscendC::ccu::Variable, ALL_REDUCE_TASK_ARG_COUNT> arguments;
    for (uint32_t index = 0; index < arguments.size(); ++index) {
        if (AscendC::ccu::LoadArg(arguments[index], index) != Result::SUCCESS) {
            return Result::CONTRACT_ERROR;
        }
    }
    CompilerContext::Current().ConfigureProgram(config.rankId, config.rankSize, config.channels);

    const CcuResult result = mc2_ops_hccl::CcuKfcAllReduceMesh1DMem2MemKernel(
        arguments[0], arguments[1], arguments[2], arguments[3], arguments[4], arguments[5], arguments[6], arguments[7],
        arguments[8], arguments[9], arguments[10], arguments[11], arguments[12], arguments[13], arguments[14],
        arguments[15], arguments[16], arguments[17], arguments[18], arguments[19], arguments[20], arguments[21],
        arguments[22], arguments[23], arguments[24], arguments[25], config.channels.data(),
        static_cast<uint32_t>(config.channels.size()), config.rankSize, config.rankId, config.dataType,
        config.outputType, config.reduceOp);
    return result == CCU_SUCCESS ? Result::SUCCESS : Result::CONTRACT_ERROR;
}

std::vector<uint64_t> PrepareAllReduceTaskArgs(const AllReduceLaunchConfig& config)
{
    return {
        config.inputAddress,
        config.outputAddress,
        config.token,
        config.scratchAddress,
        config.currentRankSliceInputOffset,
        config.currentRankSliceOutputOffset,
        config.normalSliceSize,
        config.lastSliceSize,
        config.mySliceSize,
        config.sliceOffset,
        config.isInputOutputEqual,
        config.goSize[0],
        config.goSize[1],
        config.goSize[2],
        config.goSize[3],
        config.chunkSize,
        config.tailSize,
        config.chunkLoopNum,
        config.fullGoSize[0],
        config.fullGoSize[1],
        config.fullGoSize[2],
        config.fullGoSize[3],
        config.tailGoSize[0],
        config.tailGoSize[1],
        config.tailGoSize[2],
        config.tailGoSize[3]};
}

ScenarioData BuildAllReduceMesh1DScenario(const CcuStScenario& scenario, const std::vector<KernelHandle>& handles)
{
    const uint32_t rankSize = CcuStFixture::CountRanks(scenario.topoMeta);
    const uint32_t typeSize = DATATYPE_SIZE_TABLE[scenario.dataType];
    const uint64_t dataSize = scenario.count * typeSize;
    const uint64_t unitAlignSize = typeSize;
    const uint64_t chunkSize =
        (rankSize * unitAlignSize > 0) ? RoundUpAligned(dataSize, rankSize * unitAlignSize) / rankSize : dataSize;
    const uint64_t normalSliceSize = chunkSize;
    const uint64_t lastSliceSize =
        (dataSize > normalSliceSize * (rankSize - 1)) ? dataSize - normalSliceSize * (rankSize - 1) : 0;

    constexpr uint64_t guard = 32;
    const uint64_t inputSize = guard + dataSize + guard;
    const uint64_t outputSize = guard + dataSize + guard;

    mc2_ops_hccl::LoopGroupConfig goConfig{};
    goConfig.msInterleave = mc2_ops_hccl::CCU_MS_INTERLEAVE;
    goConfig.loopCount = mc2_ops_hccl::CCU_M2M_LOCAL_COPY_LOOP_COUNT;
    goConfig.memSlice = mc2_ops_hccl::CCU_MS_SIZE;

    ScenarioData data;
    data.memories.resize(rankSize);
    data.launches.resize(rankSize);
    data.reduceOp = scenario.reduceType;
    data.srcOffsets.assign(rankSize, std::vector<uint64_t>(rankSize, 0));
    data.dstOffsets.assign(rankSize, std::vector<uint64_t>(rankSize, 0));

    for (uint32_t rank = 0; rank < rankSize; ++rank) {
        RankMemory& memory = data.memories[rank];
        memory.input.assign(inputSize, 0x5a);
        memory.output.assign(outputSize, 0xa5);

        const uint64_t inputBase = guard;
        const uint64_t outputBase = guard;

        for (uint64_t byte = 0; byte < dataSize; ++byte) {
            memory.input[inputBase + byte] = static_cast<uint8_t>(17U + rank * 41U + byte);
        }

        const uint64_t mySliceSize = (rank == rankSize - 1) ? lastSliceSize : normalSliceSize;
        const uint64_t sliceOffset = normalSliceSize * rank;
        const uint64_t scratchBytes = rankSize * mySliceSize;
        memory.parameter.resize((scratchBytes + sizeof(uint64_t) - 1) / sizeof(uint64_t));

        const auto goSize = mc2_ops_hccl::CalGoSize(mySliceSize, goConfig);

        AllReduceLaunchConfig config;
        config.inputAddress = memory.InputAddress() + inputBase;
        config.outputAddress = memory.OutputAddress() + outputBase;
        config.token = 0x1000U + rank;
        config.scratchAddress = memory.ParameterAddress();
        config.currentRankSliceInputOffset = 0;
        config.currentRankSliceOutputOffset = 0;
        config.normalSliceSize = normalSliceSize;
        config.lastSliceSize = lastSliceSize;
        config.mySliceSize = mySliceSize;
        config.sliceOffset = sliceOffset;
        config.isInputOutputEqual = 0;
        config.goSize = {goSize[0], goSize[1], goSize[2], goSize[3]};
        config.chunkSize = mySliceSize;
        config.tailSize = mySliceSize;
        config.chunkLoopNum = std::numeric_limits<uint64_t>::max() - 1;
        config.fullGoSize = {goSize[0], goSize[1], goSize[2], goSize[3]};
        config.tailGoSize = {goSize[0], goSize[1], goSize[2], goSize[3]};

        data.launches[rank] = RankLaunch{handles[rank], PrepareAllReduceTaskArgs(config), &memory};

        for (uint32_t srcRank = 0; srcRank < rankSize; ++srcRank) {
            data.srcOffsets[srcRank][rank] = inputBase;
            data.dstOffsets[srcRank][rank] = outputBase;
        }
    }
    return data;
}

} // namespace CcuSt
} // namespace HcclSim
