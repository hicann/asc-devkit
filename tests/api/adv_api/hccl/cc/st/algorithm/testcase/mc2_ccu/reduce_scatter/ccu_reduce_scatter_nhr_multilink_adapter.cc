/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_reduce_scatter_nhr_multilink_adapter.h"

#include "ccu_reduce_scatter.h"
#include "ccu_primitives_stub.h"
#include "ccu_temp_kfc_reduce_scatter_nhr_1D_multi_jetty_mem2mem.h"

#include <algorithm>
#include <cstring>
#include <limits>

namespace HcclSim {
namespace CcuSt {
namespace {

// 产品kernelArg（CcuKernelArgKfcReduceScatterNHR1DMultiJettyMem2Mem）→ ST运行配置。
// 通道不在kernelArg中（归属CcuKernelInfo.channels），按rank2ChannelIdx反演子rank重建，
// 与kernel内rank2ChannelIdx->at(toRank)的通道查表序保持一致。
void* ConvertReduceScatterNhrMultiLinkArg(const void* productArg, uint32_t)
{
    if (productArg == nullptr) {
        return nullptr;
    }
    const auto* source =
        static_cast<const mc2_ops_hccl::CcuKernelArgKfcReduceScatterNHR1DMultiJettyMem2Mem*>(productArg);
    auto* config = new ReduceScatterNhrMultiLinkKernelConfig();
    config->rankId = source->rankId;
    config->rankSize = static_cast<uint32_t>(source->rankSize);
    config->jettyNum = source->portNum;
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
        "CcuKernelReduceScatterNHR1DMultiJettyMem2Mem", CaptureCcuKfcReduceScatterNhrMultiLinkKernel,
        ConvertReduceScatterNhrMultiLinkArg,
        [](void* pointer) { delete static_cast<ReduceScatterNhrMultiLinkKernelConfig*>(pointer); });
    CcuStReduceScatter::RegisterVariant(
        "CcuSchedReduceScatterSoleNHRMultiLink", BuildReduceScatterNhrMultiLinkScenario);
    return true;
}();

} // namespace

Result CaptureCcuKfcReduceScatterNhrMultiLinkKernel(void* kernelArg)
{
    if (kernelArg == nullptr) {
        return Result::PARAM_ERROR;
    }
    const auto& config = *static_cast<ReduceScatterNhrMultiLinkKernelConfig*>(kernelArg);
    if (config.rankSize < 2 || config.rankId >= config.rankSize ||
        config.channels.size() != config.rank2ChannelIdx.size() || config.stepInfoVector.empty() ||
        config.jettyNum == 0U) {
        return Result::PARAM_ERROR;
    }
    std::array<AscendC::ccu::Variable, REDUCE_SCATTER_NHR_ML_TASK_ARG_COUNT> arguments;
    for (uint32_t index = 0; index < arguments.size(); ++index) {
        if (AscendC::ccu::LoadArg(arguments[index], index) != Result::SUCCESS) {
            return Result::CONTRACT_ERROR;
        }
    }
    CompilerContext::Current().ConfigureProgram(config.rankId, config.rankSize, config.channels);
    const CcuResult result = mc2_ops_hccl::CcuKfcReduceScatterNHR1DMultiJettyMem2MemKernel(
        arguments[0], arguments[1], arguments[2], arguments[3], arguments[4], arguments[5], arguments[6], arguments[7],
        arguments[8], arguments[9], config.channels.data(), static_cast<uint32_t>(config.channels.size()),
        config.rankSize, config.rankId, config.jettyNum, config.dataType, config.outputType, config.reduceType,
        config.stepInfoVector, config.rank2ChannelIdx);
    return result == CCU_SUCCESS ? Result::SUCCESS : Result::CONTRACT_ERROR;
}

// 10个taskArgs按kernel形参顺序排列，模拟AIV侧CcuPrepareForReduceScatterSoleNhrM2M写入HBM、
// KFC dispatch（ccu_kernel_kfc_server.cc 的 RS 分支）逐槽转发的参数：
// [0]=input [1]=output [2]=token [3]=sliceSize [4]=inputSliceStride
// [5]=sliceOneJettySize [6]=sliceLastJettySize [7]=repeatNumInv [8]=inputRepeatStride [9]=outputRepeatStride
std::vector<uint64_t> PrepareReduceScatterNhrMultiLinkTaskArgs(const ReduceScatterNhrMultiLinkLaunchConfig& config)
{
    return {
        config.inputAddress,
        config.outputAddress,
        config.token,
        config.sliceSize,
        config.inputSliceStride,
        config.sliceOneJettySize,
        config.sliceLastJettySize,
        std::numeric_limits<uint64_t>::max() - 1U, // repeatNumInv（sole executor repeatNum=1）
        config.inputRepeatStride,
        config.outputRepeatStride};
}

namespace {

// 逐元素编码小整数（1..5）为各 dtype 的精确可表示常规幅值位模式：
// FP16/BFP16 整数 ≤40 精确（常规数，避开 denormal——ST checker 的 Float16ToFloat 与 sim 规约器
// 对 denormal 均有 ×2 转换偏差，且两者复合方式不同会导致期望/实际错位）；FP32/INT32 天然精确。
uint64_t EncodeReduceScatterElement(HcclDataType dataType, uint32_t value)
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

ScenarioData BuildReduceScatterNhrMultiLinkScenario(
    const CcuStScenario& scenario, const std::vector<KernelHandle>& handles)
{
    const uint32_t rankSize = CcuStFixture::CountRanks(scenario.topoMeta);
    const uint64_t typeSize = DATATYPE_SIZE_TABLE[scenario.dataType];
    const uint64_t sliceSize = scenario.count * typeSize;
    const uint64_t totalSize = sliceSize * rankSize;
    constexpr uint64_t guard = 32;
    constexpr uint64_t hcclMinSliceAlign = 128;
    constexpr uint64_t nhrJettyNum = 1; // 与 hccl CcuTempReduceScatterNhrMultiJettyMem2Mem1D 的 portNum=1 一致
    // 与宿主 CcuPrepareForReduceScatterSoleNhrM2M 同式：sliceAlignCount=128/typeSize（元素数）
    const uint64_t sliceAlignCount = hcclMinSliceAlign / typeSize;
    const uint64_t sliceOneJettySize = sliceSize / nhrJettyNum / sliceAlignCount * sliceAlignCount;
    const uint64_t sliceLastJettySize = sliceSize - (nhrJettyNum - 1U) * sliceOneJettySize;

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
            // src rank 的第 slice 片被 dst rank slice 规约持有；MultiJetty kernel 输出落点恒为 offset 0
            // （kernel 内 localDst.addr = output，无 rank 偏移），区别于 plain NHR 的 rank*sliceSize。
            data.srcOffsets[rank][slice] = guard + slice * sliceSize;
            data.dstOffsets[rank][slice] = guard;
            // 逐元素填充小整数的精确位模式：8 rank SUM ≤ 40 精确，NHR 树状规约序与 checker
            // 逐 src 序不同也不会产生浮点舍入差异。
            for (uint64_t elem = 0; elem < scenario.count; ++elem) {
                const uint64_t base = guard + slice * sliceSize + elem * typeSize;
                const uint64_t encoded =
                    EncodeReduceScatterElement(scenario.dataType, 1U + ((rank * 3U + slice * 5U + elem) % 5U));
                for (uint64_t byte = 0; byte < typeSize; ++byte) {
                    memory.input[base + byte] = static_cast<uint8_t>((encoded >> (byte * 8U)) & 0xFFU);
                }
            }
        }

        ReduceScatterNhrMultiLinkLaunchConfig config;
        config.inputAddress = memory.InputAddress() + guard;
        config.outputAddress = memory.OutputAddress() + guard;
        config.token = 0x1000U + rank;
        config.sliceSize = sliceSize;
        config.inputSliceStride = sliceSize; // strideCount=0
        config.sliceOneJettySize = sliceOneJettySize;
        config.sliceLastJettySize = sliceLastJettySize;
        config.inputRepeatStride = 0;
        config.outputRepeatStride = 0;
        data.launches[rank] = RankLaunch{handles[rank], PrepareReduceScatterNhrMultiLinkTaskArgs(config), &memory};
    }
    return data;
}

} // namespace CcuSt
} // namespace HcclSim
