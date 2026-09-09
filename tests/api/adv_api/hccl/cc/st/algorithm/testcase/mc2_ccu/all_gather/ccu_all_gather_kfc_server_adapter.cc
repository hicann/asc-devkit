/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_all_gather_kfc_server_adapter.h"

#include "alg_param.h"
#include "ccu_all_gather.h"
#include "ccu_kernel_kfc_server.h"
#include "ccu_kernel_proxy.h"
#include "kfc_server_protocol.h"

#include <limits>
#include <vector>

namespace HcclSim {
namespace CcuSt {
namespace {

// The server reads 32 words at a time but advances its parameter cursor by
// CCU_PARAM_NUM_MAX words, reserving the second-die payload as well.
constexpr uint32_t KFC_QUEUE_PARAM_WORDS = mc2_ops_hccl::CCU_PARAM_NUM_MAX;
constexpr uint32_t KFC_QUEUE_ENTRY_COUNT = 2;
constexpr uint32_t KFC_CKE_WORD_OFFSET = KFC_QUEUE_PARAM_WORDS * KFC_QUEUE_ENTRY_COUNT;
constexpr uint32_t KFC_CKE_REGION_WORDS = mc2_ops_hccl::CCU_TASK_NUM_MAX;
constexpr uint32_t KFC_PARAMETER_WORDS = KFC_CKE_WORD_OFFSET + 2 * KFC_CKE_REGION_WORDS;
constexpr uint64_t SOURCE_BASE_OFFSET = 7;
constexpr uint64_t DESTINATION_BASE_OFFSET = 11;
constexpr uint64_t GUARD_SIZE = 32;

struct AllGatherKfcServerConfig {
    mc2_ops_hccl::CcuKernelArgKfcServer kernelArg;
};

void* ConvertAllGatherKfcServerArg(const void* productArg, uint32_t argNum)
{
    if (productArg == nullptr || argNum != 1U) {
        return nullptr;
    }
    const auto* source = static_cast<const mc2_ops_hccl::CcuKernelArgKfcServer*>(productArg);
    if (source->opParam.opType != HcclCMDType::HCCL_CMD_ALLGATHER) {
        return nullptr;
    }
    auto* config = new AllGatherKfcServerConfig();
    config->kernelArg = *source;
    return config;
}

Result CaptureAllGatherKfcServerKernel(void* kernelArg)
{
    if (kernelArg == nullptr) {
        return Result::PARAM_ERROR;
    }
    auto& config = *static_cast<AllGatherKfcServerConfig*>(kernelArg);
    const auto& arg = config.kernelArg;
    if (arg.opParam.opType != HcclCMDType::HCCL_CMD_ALLGATHER || arg.rankSize < 2U || arg.rankId >= arg.rankSize ||
        arg.channelCount != arg.rankSize - 1U) {
        return Result::PARAM_ERROR;
    }

    std::vector<ChannelHandle> channels(arg.channels, arg.channels + arg.channelCount);
    CompilerContext::Current().ConfigureProgram(arg.rankId, static_cast<uint32_t>(arg.rankSize), channels);
    const CcuResult result = mc2_ops_hccl::CcuKfcServerKernel(static_cast<CcuKernelArg>(&config.kernelArg));
    return result == CCU_SUCCESS ? Result::SUCCESS : Result::CONTRACT_ERROR;
}

bool g_registered = []() {
    RegisterCaptureFunction(
        "CcuKernelKfcServer", CaptureAllGatherKfcServerKernel, ConvertAllGatherKfcServerArg,
        [](void* pointer) { delete static_cast<AllGatherKfcServerConfig*>(pointer); });
    CcuStAllGather::RegisterVariant("CcuSchedAllGatherSoleMesh", BuildAllGatherKfcServerScenario);
    return true;
}();

void FillKfcQueue(RankMemory& memory, uint32_t rank, uint64_t sliceSize, uint64_t inputAddress, uint64_t outputAddress)
{
    memory.parameter.assign(KFC_PARAMETER_WORDS, 0U);
    if (sliceSize == 0U) {
        // An empty AllGather does not enqueue a task in the product template.
        // Consume the termination marker immediately instead.
        memory.parameter[0] = std::numeric_limits<uint64_t>::max();
        memory.parameter[KFC_CKE_WORD_OFFSET] = 1U;
        return;
    }

    // Keep this payload in lockstep with CcuTempKfcAllGatherMesh1DMem2Mem::KernelRun.
    memory.parameter[HcclKfcProtocol::KFC_CONCURRENT_AG_OP_ID] = 1U;
    memory.parameter[HcclKfcProtocol::KFC_CONCURRENT_AG_MESH_INPUT] = inputAddress;
    memory.parameter[HcclKfcProtocol::KFC_CONCURRENT_AG_MESH_OUTPUT] = outputAddress;
    memory.parameter[HcclKfcProtocol::KFC_CONCURRENT_AG_MESH_OUTPUT_OFFSET] =
        DESTINATION_BASE_OFFSET + static_cast<uint64_t>(rank) * sliceSize;
    memory.parameter[HcclKfcProtocol::KFC_CONCURRENT_AG_MESH_SLICE_SIZE] = sliceSize;
    memory.parameter[HcclKfcProtocol::KFC_CONCURRENT_AG_MESH_CURRENT_RANK_SLICE_INPUT_OFFSET] = 0U;
    memory.parameter[HcclKfcProtocol::KFC_CONCURRENT_AG_MESH_REPEAT_NUM_INV] =
        std::numeric_limits<uint64_t>::max() - 1U;
    memory.parameter[HcclKfcProtocol::KFC_CONCURRENT_AG_MESH_INPUT_REPEAT_STRIDE] = 0U;
    memory.parameter[HcclKfcProtocol::KFC_CONCURRENT_AG_MESH_OUTPUT_REPEAT_STRIDE] = 0U;
    memory.parameter[HcclKfcProtocol::KFC_CONCURRENT_AG_MESH_LAST_SLICE_SIZE] = sliceSize;
    memory.parameter[HcclKfcProtocol::KFC_CONCURRENT_AG_MESH_INPUT_OUTPUT_EQUAL] = 0U;

    // Queue entry 1 terminates the persistent server after the AllGather task.
    memory.parameter[KFC_QUEUE_PARAM_WORDS] = std::numeric_limits<uint64_t>::max();
    memory.parameter[KFC_CKE_WORD_OFFSET] = 1U;
    memory.parameter[KFC_CKE_WORD_OFFSET + 1U] = 1U;
}

std::vector<uint64_t> PrepareKfcServerTaskArgs(const RankMemory& memory, uint32_t rank)
{
    const uint64_t parameterAddress = memory.ParameterAddress();
    const uint64_t ckeAddress = parameterAddress + static_cast<uint64_t>(KFC_CKE_WORD_OFFSET) * sizeof(uint64_t);
    return {parameterAddress, ckeAddress, 1U, 1U, 0U, 0x1000U + rank};
}

} // namespace

ScenarioData BuildAllGatherKfcServerScenario(const CcuStScenario& scenario, const std::vector<KernelHandle>& handles)
{
    const uint32_t rankSize = CcuStFixture::CountRanks(scenario.topoMeta);
    const uint64_t sliceSize = scenario.count * DATATYPE_SIZE_TABLE[scenario.dataType];

    ScenarioData data;
    data.memories.resize(rankSize);
    data.launches.resize(rankSize);
    data.srcOffsets.assign(rankSize, std::vector<uint64_t>(rankSize, SOURCE_BASE_OFFSET));
    data.dstOffsets.assign(rankSize, std::vector<uint64_t>(rankSize, 0U));

    for (uint32_t source = 0; source < rankSize; ++source) {
        for (uint32_t destination = 0; destination < rankSize; ++destination) {
            data.dstOffsets[source][destination] = DESTINATION_BASE_OFFSET + static_cast<uint64_t>(source) * sliceSize;
        }
    }

    for (uint32_t rank = 0; rank < rankSize; ++rank) {
        RankMemory& memory = data.memories[rank];
        memory.input.assign(SOURCE_BASE_OFFSET + sliceSize + GUARD_SIZE, 0x5a);
        memory.output.assign(DESTINATION_BASE_OFFSET + rankSize * sliceSize + GUARD_SIZE, 0xa5);
        for (uint64_t byte = 0; byte < sliceSize; ++byte) {
            memory.input[SOURCE_BASE_OFFSET + byte] = static_cast<uint8_t>(17U + rank * 41U + byte);
        }

        FillKfcQueue(memory, rank, sliceSize, memory.InputAddress() + SOURCE_BASE_OFFSET, memory.OutputAddress());
        data.launches[rank] = RankLaunch{handles[rank], PrepareKfcServerTaskArgs(memory, rank), &memory};
    }
    return data;
}

} // namespace CcuSt
} // namespace HcclSim
