/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_reduce_scatter_nhr_2die_adapter.h"

#include "ccu_reduce_scatter.h"
#include "ccu_primitives_stub.h"
#include "ccu_temp_kfc_reduce_scatter_nhr_1D_2die_mem2mem.h"

#include <algorithm>
#include <cstring>
#include <limits>

namespace HcclSim {
namespace CcuSt {
namespace {

// 产品kernelArg（CcuKernelArgKfcReduceScatterNHR1D2Die）→ ST运行配置。
// 通道不在kernelArg中（归属CcuKernelInfo.channels，按 die 分组），按rank2ChannelIdx反演子rank重建；
// 双 die 时每 die 一份 kernelInfo（各自 capture），axisId/axisSize 随 kernelArg 传递。
void* ConvertReduceScatterNhr2DieArg(const void* productArg, uint32_t)
{
    if (productArg == nullptr) {
        return nullptr;
    }
    const auto* source = static_cast<const mc2_ops_hccl::CcuKernelArgKfcReduceScatterNHR1D2Die*>(productArg);
    auto* config = new ReduceScatterNhr2DieKernelConfig();
    config->rankId = source->rankId;
    config->rankSize = static_cast<uint32_t>(source->rankSize);
    config->axisId = source->axisId;
    config->axisSize = source->axisSize;
    config->dataType = source->opParam.DataDes.dataType;
    config->outputType = source->opParam.DataDes.outputType == HcclDataType::HCCL_DATA_TYPE_RESERVED ?
                             source->opParam.DataDes.dataType :
                             source->opParam.DataDes.outputType;
    config->reduceType = source->opParam.reduceType;
    config->stepInfoVector = source->stepInfoVector;
    config->rank2ChannelIdx = source->rank2ChannelIdx;

    std::map<uint32_t, uint32_t> channelIdx2SubRank;
    for (const auto& entry : source->rank2ChannelIdx) {
        channelIdx2SubRank[entry.second] = entry.first;
    }
    for (const auto& entry : channelIdx2SubRank) {
        if (entry.second != config->rankId) {
            config->channels.push_back(MakeChannel(config->rankId, entry.second));
        }
    }
    return config;
}

bool g_registered = []() {
    RegisterCaptureFunction(
        "CcuKernelKfcReduceScatterNHR1D2DieMem2Mem", CaptureCcuKfcReduceScatterNhr2DieKernel,
        ConvertReduceScatterNhr2DieArg,
        [](void* pointer) { delete static_cast<ReduceScatterNhr2DieKernelConfig*>(pointer); });
    CcuStReduceScatter::RegisterVariant("CcuSchedReduceScatterSoleNHR", BuildReduceScatterNhr2DieScenario);
    return true;
}();

} // namespace

Result CaptureCcuKfcReduceScatterNhr2DieKernel(void* kernelArg)
{
    if (kernelArg == nullptr) {
        return Result::PARAM_ERROR;
    }
    const auto& config = *static_cast<ReduceScatterNhr2DieKernelConfig*>(kernelArg);
    if (config.rankSize < 2 || config.rankId >= config.rankSize || config.axisId >= config.axisSize ||
        config.axisSize == 0U || config.channels.size() != config.rank2ChannelIdx.size() ||
        config.stepInfoVector.empty()) {
        return Result::PARAM_ERROR;
    }
    std::array<AscendC::ccu::Variable, REDUCE_SCATTER_NHR_2DIE_TASK_ARG_COUNT> arguments;
    for (uint32_t index = 0; index < arguments.size(); ++index) {
        if (AscendC::ccu::LoadArg(arguments[index], index) != Result::SUCCESS) {
            return Result::CONTRACT_ERROR;
        }
    }
    CompilerContext::Current().ConfigureProgram(config.rankId, config.rankSize, config.channels);
    const CcuResult result = mc2_ops_hccl::CcuKfcReduceScatterNHR1D2DieMem2MemKernel(
        arguments[0], arguments[1], arguments[2], arguments[3], arguments[4], arguments[5], arguments[6], arguments[7],
        arguments[8], arguments[9], arguments[10], arguments[11], arguments[12], arguments[13], arguments[14],
        arguments[15], arguments[16], arguments[17], arguments[18], arguments[19], arguments[20],
        config.channels.data(), static_cast<uint32_t>(config.channels.size()), config.rankSize, config.rankId,
        config.axisId, config.axisSize, config.dataType, config.outputType, config.reduceType, config.stepInfoVector,
        config.rank2ChannelIdx);
    return result == CCU_SUCCESS ? Result::SUCCESS : Result::CONTRACT_ERROR;
}

// 21个taskArgs按kernel形参顺序排列，模拟AIV侧CcuPrepareForReduceScatterSoleNhr2DieM2M写入HBM、
// KFC dispatch（ccu_kernel_kfc_server.cc 的 RS 分支）逐槽转发的参数：
// [0]=input [1]=output [2]=token [3]=die0Size [4]=die1Size [5]=die0Last [6]=die1Last
// [7]=inputSliceStride [8]=currentRankSliceOutputOffset(=0) [9]=inputRepeatStride [10]=outputRepeatStride
// [11]=repeatNumVar [12]=isInputOutputEqual [13..16]=goSizeNormal [17..20]=goSizeLast
std::vector<uint64_t> PrepareReduceScatterNhr2DieTaskArgs(const ReduceScatterNhr2DieLaunchConfig& config)
{
    return {
        config.inputAddress,
        config.outputAddress,
        config.token,
        config.die0Size,
        config.die1Size,
        config.die0LastSliceSize,
        config.die1LastSliceSize,
        config.inputSliceStride,
        config.currentRankSliceOutputOffset,
        config.inputRepeatStride,
        config.outputRepeatStride,
        std::numeric_limits<uint64_t>::max() - 1U, // repeatNumVar（sole executor repeatNum=1）
        config.isInputOutputEqual,
        config.goSizeNormal[0],
        config.goSizeNormal[1],
        config.goSizeNormal[2],
        config.goSizeNormal[3],
        config.goSizeLast[0],
        config.goSizeLast[1],
        config.goSizeLast[2],
        config.goSizeLast[3]};
}

namespace {

// 与 mesh adapter 的 PackParallelParameters 同款 V1 位布局（本分支基线为 A5/CCU_V1）：
// repeatNum<<55 | repeatLoopIndex<<48 | totalLoopNum<<41，各 7bit。
uint64_t PackNhr2DieParallelParameters(uint64_t repeatNum, uint64_t repeatLoopIndex, uint64_t totalLoopNum)
{
    constexpr uint64_t mask = 0x7f;
    return ((repeatNum & mask) << 55U) | ((repeatLoopIndex & mask) << 48U) | ((totalLoopNum & mask) << 41U);
}

// 与 AIV prepare 的 CalcGoSize(x, 8, 32768) 参数对齐（hccl 模板 config：
// loopCount=CCU_MS_LOCAL_COPY_LOOP_COUNT=8, memSlice=CCU_MS_SIZE*LOCAL_COPY_MS_PER_LOOP=32K）
std::array<uint64_t, 4> CalculateNhr2DieGoSize(uint64_t size)
{
    const uint64_t fullLoops = size / NHR_2DIE_LOOP_SIZE;
    const uint64_t remaining = size % NHR_2DIE_LOOP_SIZE;
    const uint64_t slices = remaining / NHR_2DIE_MEM_SLICE;
    const uint64_t residual = remaining % NHR_2DIE_MEM_SLICE;
    std::array<uint64_t, 4> result{{fullLoops * NHR_2DIE_LOOP_SIZE, fullLoops, 0, 0}};
    if (slices != 0 && residual == 0) {
        result[2] = PackNhr2DieParallelParameters(slices - 1, 0, 1);
        result[3] = NHR_2DIE_MEM_SLICE;
    } else if (slices == 0 && residual != 0) {
        result[2] = PackNhr2DieParallelParameters(0, 0, 1);
        result[3] = residual;
    } else if (slices != 0) {
        result[2] = PackNhr2DieParallelParameters(slices - 1, 1, 2);
        result[3] = residual;
    }
    return result;
}

// 逐元素编码小整数（1..5）为各 dtype 的精确可表示常规幅值位模式（与 MultiLink adapter 同款：
// 避开 denormal 的 checker/sim 转换偏差）
uint64_t EncodeNhr2DieElement(HcclDataType dataType, uint32_t value)
{
    switch (dataType) {
        case HCCL_DATA_TYPE_FP16: {
            static const uint16_t fp16Table[6] = {0, 0x3C00, 0x4000, 0x4200, 0x4400, 0x4500};
            return fp16Table[value];
        }
        case HCCL_DATA_TYPE_BFP16: {
            static const uint16_t bfp16Table[6] = {0, 0x3F80, 0x4000, 0x4040, 0x4080, 0x40A0};
            return bfp16Table[value];
        }
        case HCCL_DATA_TYPE_FP32: {
            const float f = static_cast<float>(value);
            uint32_t bits = 0;
            std::memcpy(&bits, &f, sizeof(bits));
            return bits;
        }
        default:
            return value;
    }
}

} // namespace

ScenarioData BuildReduceScatterNhr2DieScenario(const CcuStScenario& scenario, const std::vector<KernelHandle>& handles)
{
    const uint32_t rankSize = CcuStFixture::CountRanks(scenario.topoMeta);
    const uint64_t typeSize = DATATYPE_SIZE_TABLE[scenario.dataType];
    const uint64_t sliceSize = scenario.count * typeSize;
    const uint64_t totalSize = sliceSize * rankSize;
    constexpr uint64_t guard = 32;
    // 与 AIV prepare 对齐：dieNum/dieSplitRatio 到 AIV 的链路接通前 die1 恒 0（die0=全量）。
    // 单 die 语义正确；双 die 时 die1 mission 空转（sliceSize==0 分支 EventRecord 占位）。
    const uint64_t die0Size = sliceSize;
    const uint64_t die1Size = 0;
    // hccl executor 赋 sliceSize == tailSize，normal/last 两组保真相等
    const uint64_t die0LastSliceSize = die0Size;
    const uint64_t die1LastSliceSize = die1Size;
    const std::array<uint64_t, 4> goSizeNormal = CalculateNhr2DieGoSize(sliceSize);
    const std::array<uint64_t, 4> goSizeLast = goSizeNormal;

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

        for (uint32_t slice = 0; slice < rankSize; ++slice) {
            // 输出落点：kernel 内 localDst.addr = output + currentRankSliceOutputOffset(=0)，
            // 且 axisId==1 时额外 +die0Size——按 die 段的起始对齐（die0 段落 offset0，die1 段落 offset die0Size）
            data.srcOffsets[rank][slice] = guard + slice * sliceSize;
            data.dstOffsets[rank][slice] = guard;
            for (uint64_t elem = 0; elem < scenario.count; ++elem) {
                const uint64_t base = guard + slice * sliceSize + elem * typeSize;
                const uint64_t encoded =
                    EncodeNhr2DieElement(scenario.dataType, 1U + ((rank * 3U + slice * 5U + elem) % 5U));
                for (uint64_t byte = 0; byte < typeSize; ++byte) {
                    memory.input[base + byte] = static_cast<uint8_t>((encoded >> (byte * 8U)) & 0xFFU);
                }
            }
        }

        ReduceScatterNhr2DieLaunchConfig config;
        config.inputAddress = memory.InputAddress() + guard;
        config.outputAddress = memory.OutputAddress() + guard;
        config.token = 0x1000U + rank;
        config.die0Size = die0Size;
        config.die1Size = die1Size;
        config.die0LastSliceSize = die0LastSliceSize;
        config.die1LastSliceSize = die1LastSliceSize;
        config.inputSliceStride = sliceSize; // strideCount=0
        config.currentRankSliceOutputOffset = 0;
        config.inputRepeatStride = 0;
        config.outputRepeatStride = 0;
        config.isInputOutputEqual = 0;
        config.goSizeNormal = goSizeNormal;
        config.goSizeLast = goSizeLast;
        data.launches[rank] = RankLaunch{handles[rank], PrepareReduceScatterNhr2DieTaskArgs(config), &memory};
    }
    return data;
}

} // namespace CcuSt
} // namespace HcclSim
