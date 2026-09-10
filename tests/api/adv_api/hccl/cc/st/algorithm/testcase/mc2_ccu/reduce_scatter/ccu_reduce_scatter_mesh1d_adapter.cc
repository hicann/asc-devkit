/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_reduce_scatter_mesh1d_adapter.h"

#include "ccu_reduce_scatter.h"
#include "ccu_kernel_kfc_reduce_scatter_mesh1d_mem2mem.h"
#include "ccu_primitives_stub.h"
#include "ccu_temp_kfc_reduce_scatter_mesh_1D_mem2mem.h"

#include <algorithm>
#include <limits>

namespace HcclSim {
namespace CcuSt {
namespace {

// 与宿主 CcuPrepareForReduceScatterM2M / hccl_inner_def.h 对齐：
// CalcGoSize(sliceSize, CCU_LOOP_COUNT_M2M_RE=16, CCU_MEMSLICE_SIZE=4096)，注意无 ×8（区别于 AllToAllV）。
constexpr uint64_t MEM_SLICE = 4ULL * 1024ULL;
constexpr uint64_t LOOP_SIZE = 16ULL * MEM_SLICE;
// KFC RS scratch 总量（与 AIV prepare 的 64MB 及 host 侧 alloc_ctx_res.cc 对齐）；
// kernel 按 chunkSize = scratchSize / rankNum / 128 * 128 自适应切分，ST 将 scratch 放 parameter 缓冲。
constexpr uint64_t RS_MESH_SCRATCH_SIZE = 64ULL * 1024ULL * 1024ULL;
constexpr uint64_t MIN_SLICE_ALIGN = 128;

uint64_t PackParallelParameters(uint64_t repeatNum, uint64_t repeatLoopIndex, uint64_t totalLoopNum)
{
    constexpr uint64_t mask = 0x7f;
    return ((repeatNum & mask) << 55U) | ((repeatLoopIndex & mask) << 48U) | ((totalLoopNum & mask) << 41U);
}

// 产品kernelArg（CcuKernelArgKfcReduceScatterMesh1DMem2Mem）→ ST运行配置。
// 通道按子rank升序重建（跳过自身），与kernel内InitResource按peerId递增消费channelIdx的顺序一致。
void* ConvertReduceScatterMeshArg(const void* productArg, uint32_t)
{
    if (productArg == nullptr) {
        return nullptr;
    }
    const auto* source = static_cast<const mc2_ops_hccl::CcuKernelArgKfcReduceScatterMesh1DMem2Mem*>(productArg);
    if (source->subCommRanks.empty()) {
        return nullptr;
    }
    auto* config = new ReduceScatterMeshKernelConfig();
    config->rankId = source->rankId;
    config->rankSize = static_cast<uint32_t>(source->rankSize);
    config->dataType = source->opParam.DataDes.dataType;
    config->outputType = source->opParam.DataDes.outputType == HcclDataType::HCCL_DATA_TYPE_RESERVED ?
                             source->opParam.DataDes.dataType :
                             source->opParam.DataDes.outputType;
    config->reduceType = source->opParam.reduceType;
    for (uint32_t subRank = 0; subRank < config->rankSize; ++subRank) {
        if (subRank != config->rankId) {
            config->channels.push_back(MakeChannel(config->rankId, subRank));
        }
    }
    return config;
}

bool g_registered = []() {
    RegisterCaptureFunction(
        "CcuKernelKfcReduceScatterMesh1DMem2Mem", CaptureCcuKfcReduceScatterMesh1DKernel, ConvertReduceScatterMeshArg,
        [](void* pointer) { delete static_cast<ReduceScatterMeshKernelConfig*>(pointer); });
    CcuStReduceScatter::RegisterVariant("CcuSchedReduceScatterSoleMesh", BuildReduceScatterMeshScenario);
    return true;
}();

} // namespace

Result CaptureCcuKfcReduceScatterMesh1DKernel(void* kernelArg)
{
    if (kernelArg == nullptr) {
        return Result::PARAM_ERROR;
    }
    const auto& config = *static_cast<ReduceScatterMeshKernelConfig*>(kernelArg);
    if (config.rankSize < 2 || config.rankId >= config.rankSize || config.channels.size() != config.rankSize - 1) {
        return Result::PARAM_ERROR;
    }
    std::array<AscendC::ccu::Variable, REDUCE_SCATTER_MESH_TASK_ARG_COUNT> arguments;
    for (uint32_t index = 0; index < arguments.size(); ++index) {
        if (AscendC::ccu::LoadArg(arguments[index], index) != Result::SUCCESS) {
            return Result::CONTRACT_ERROR;
        }
    }
    CompilerContext::Current().ConfigureProgram(config.rankId, config.rankSize, config.channels);
    const CcuResult result = mc2_ops_hccl::CcuReduceScatterMesh1DMem2MemKernel(
        arguments[0], arguments[1], arguments[2], arguments[3], arguments[4], arguments[5], arguments[6], arguments[7],
        arguments[8], arguments[9], arguments[10], arguments[11], arguments[12], arguments[13], arguments[14],
        arguments[15], config.channels.data(), static_cast<uint32_t>(config.channels.size()), config.rankSize,
        config.rankId, config.dataType, config.outputType, config.reduceType);
    return result == CCU_SUCCESS ? Result::SUCCESS : Result::CONTRACT_ERROR;
}

// 与宿主 CcuPrepareForReduceScatterM2M 的 CalcGoSize(x, 16, 4096) 参数对齐
std::array<uint64_t, 4> CalculateReduceScatterMeshGoSize(uint64_t size)
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

// 16个taskArgs按kernel形参顺序排列，模拟AIV侧CcuPrepareForReduceScatterM2M写入HBM、
// KFC dispatch（ccu_kernel_kfc_server.cc 的 RS 分支）逐槽转发的参数：
// [0]=input [1]=output [2]=token [3]=scratch [4]=currentRankSliceInputOffset
// [5]=chunkSize [6]=chunkLoopNum [7]=tailSize [8..11]=fullGoSize [12..15]=tailGoSize
std::vector<uint64_t> PrepareReduceScatterMeshTaskArgs(const ReduceScatterMeshLaunchConfig& config)
{
    return {
        config.inputAddress,
        config.outputAddress,
        config.token,
        config.scratchAddress,
        config.currentRankSliceInputOffset,
        config.chunkSize,
        config.chunkLoopNum,
        config.tailSize,
        config.fullGoSize[0],
        config.fullGoSize[1],
        config.fullGoSize[2],
        config.fullGoSize[3],
        config.tailGoSize[0],
        config.tailGoSize[1],
        config.tailGoSize[2],
        config.tailGoSize[3]};
}

ScenarioData BuildReduceScatterMeshScenario(const CcuStScenario& scenario, const std::vector<KernelHandle>& handles)
{
    const uint32_t rankSize = CcuStFixture::CountRanks(scenario.topoMeta);
    const uint64_t typeSize = DATATYPE_SIZE_TABLE[scenario.dataType];
    const uint64_t sliceSize = scenario.count * typeSize;
    const uint64_t totalSize = sliceSize * rankSize;
    constexpr uint64_t guard = 32;

    // chunk切分公式与宿主 CcuPrepareForReduceScatterM2M 逐行对齐（生产64MB scratch）
    const uint64_t chunkSize = RS_MESH_SCRATCH_SIZE / rankSize / MIN_SLICE_ALIGN * MIN_SLICE_ALIGN;
    const uint64_t fullChunkCount = sliceSize == 0 ? 0 : (sliceSize - 1) / chunkSize;
    const uint64_t chunkCount = sliceSize == 0 ? 0 : fullChunkCount + 1;
    const uint64_t tailSize = sliceSize - fullChunkCount * chunkSize;

    ScenarioData data;
    data.memories.resize(rankSize);
    data.launches.resize(rankSize);
    data.srcOffsets.assign(rankSize, std::vector<uint64_t>(rankSize, 0));
    data.dstOffsets.assign(rankSize, std::vector<uint64_t>(rankSize, 0));
    data.reduceOp = scenario.reduceType;

    for (uint32_t rank = 0; rank < rankSize; ++rank) {
        RankMemory& memory = data.memories[rank];
        memory.input.assign(guard + totalSize, 0x5a);
        memory.output.assign(guard + totalSize, 0xa5);
        // scratch（parameter缓冲）：kernel把本地scratch切成rankSize片，容量rankSize*chunkSize；
        // 零长度时chunk循环不执行、scratch不被触碰，仅保留最小缓冲使地址有效。
        const uint64_t scratchBytes = sliceSize == 0 ? sizeof(uint64_t) : rankSize * chunkSize;
        memory.parameter.resize((scratchBytes + sizeof(uint64_t) - 1) / sizeof(uint64_t));

        for (uint32_t slice = 0; slice < rankSize; ++slice) {
            // src rank 的第 slice 片被 dst rank slice 读取；Mesh kernel 输出落点恒为 offset 0
            // （kernel 内 currentRankSliceOutputOffset 固定为 0），区别于 NHR 的 rank*sliceSize。
            data.srcOffsets[rank][slice] = guard + slice * sliceSize;
            data.dstOffsets[rank][slice] = guard;
            for (uint64_t byte = 0; byte < sliceSize; ++byte) {
                // 取值1..5：8 rank SUM结果不超过40，FP16/BFP16精确可表示、INT32不溢出
                memory.input[guard + slice * sliceSize + byte] =
                    static_cast<uint8_t>(1U + ((rank * 3U + slice * 5U + byte) % 5U));
            }
        }

        ReduceScatterMeshLaunchConfig config;
        config.inputAddress = memory.InputAddress() + guard;
        config.outputAddress = memory.OutputAddress() + guard;
        config.token = 0x1000U + rank;
        config.scratchAddress = memory.ParameterAddress();
        config.currentRankSliceInputOffset = rank * sliceSize; // strideCount=0
        config.chunkSize = chunkSize;
        config.chunkLoopNum = std::numeric_limits<uint64_t>::max() - chunkCount;
        config.tailSize = tailSize;
        config.fullGoSize = CalculateReduceScatterMeshGoSize(chunkSize);
        config.tailGoSize = CalculateReduceScatterMeshGoSize(tailSize);
        data.launches[rank] = RankLaunch{handles[rank], PrepareReduceScatterMeshTaskArgs(config), &memory};
    }
    return data;
}

} // namespace CcuSt
} // namespace HcclSim
