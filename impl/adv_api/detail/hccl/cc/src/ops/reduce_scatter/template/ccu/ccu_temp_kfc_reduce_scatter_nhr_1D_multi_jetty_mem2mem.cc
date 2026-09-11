/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "alg_data_trans_wrapper.h"
#include "channel.h"
#include "ccu_temp_kfc_reduce_scatter_nhr_1D_multi_jetty_mem2mem.h"

namespace mc2_ops_hccl {
namespace {
// 与 hccl CcuTempReduceScatterNhrMultiJettyMem2Mem1D 保持一致：当前 portNum 为 1。
constexpr uint32_t KFC_RS_NHR_PORT_NUM = 1U;
} // namespace

CcuTempKfcReduceScatterNHR1DMultiJettyMem2Mem::CcuTempKfcReduceScatterNHR1DMultiJettyMem2Mem(
    const OpParam& param, u32 rankId, const std::vector<std::vector<u32>>& subCommRanks)
    : CcuAlgTemplateBase(param, rankId, subCommRanks)
{
    const auto& ranks = subCommRanks_[0];
    const auto it = std::find(ranks.begin(), ranks.end(), rankId);
    if (it != ranks.end()) {
        mySubCommRank_ = static_cast<uint32_t>(std::distance(ranks.begin(), it));
    }
    tempRankSize_ = static_cast<uint32_t>(ranks.size());
}

HcclResult CcuTempKfcReduceScatterNHR1DMultiJettyMem2Mem::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    CHK_RET(GetRes(resourceRequest));
    resourceRequest.ccuKernelNum.push_back(1U);

    std::vector<HcclChannelDesc> channelDescs;
    CHK_RET(CalcChannelRequestNhrMultiJetty(comm, param, topoInfo, subCommRanks_, channelDescs));
    // 与 hccl GetNhrStepInfo 的 channelResort 语义对齐：每 peer 只保留首条通道
    // （hccl 同样只把 rankIdToChannelDesc_[rank] 的第一条压入 channelResort），
    // 保证 kernel 的 rank2ChannelIdx.size() == channels.size() 不变式成立。
    std::vector<HcclChannelDesc> channelResort;
    auto kernelArg = std::make_shared<CcuKernelArgKfcReduceScatterNHR1DMultiJettyMem2Mem>();
    for (const auto& channel : channelDescs) {
        CHK_PRT_RET(
            channel.channelProtocol != COMM_PROTOCOL_UBC_CTP,
            HCCL_ERROR("[CcuTempKfcReduceScatterNHR1DMultiJettyMem2Mem] invalid protocol[%u]", channel.channelProtocol),
            HCCL_E_INTERNAL);
        const uint32_t remoteSubRank = RemoteRankIdToSubRank(channel.remoteRank);
        if (kernelArg->rank2ChannelIdx.count(remoteSubRank) == 0U) {
            kernelArg->rank2ChannelIdx[remoteSubRank] = static_cast<uint32_t>(channelResort.size());
            channelResort.push_back(channel);
        }
    }

    kernelArg->rankSize = tempRankSize_;
    kernelArg->rankId = mySubCommRank_;
    kernelArg->portNum = KFC_RS_NHR_PORT_NUM;
    kernelArg->opParam = param;
    kernelArg->subCommRanks = subCommRanks_;
    CHK_RET(CalcNhrInfo(kernelArg->stepInfoVector));

    CcuKernelInfo kernelInfo{};
    CHK_SAFETY_FUNC_RET(strcpy_s(
        kernelInfo.kernelFuncName, sizeof(kernelInfo.kernelFuncName), "CcuKernelReduceScatterNHR1DMultiJettyMem2Mem"));
    kernelInfo.channels = channelResort;
    kernelInfo.setKernelArg(kernelArg);
    resourceRequest.ccuKernelInfos.push_back(kernelInfo);
    return HCCL_SUCCESS;
}

HcclResult CcuTempKfcReduceScatterNHR1DMultiJettyMem2Mem::CalcNhrInfo(std::vector<KfcNhrStepInfo>& stepInfoVector) const
{
    u32 stepNum = 0;
    for (u32 ranks = tempRankSize_ - 1U; ranks != 0U; ranks >>= 1U) {
        ++stepNum;
    }
    for (u32 step = 0; step < stepNum; ++step) {
        KfcNhrStepInfo stepInfo;
        CHK_RET(GetStepInfo(step, stepInfo));
        stepInfoVector.push_back(stepInfo);
    }
    return HCCL_SUCCESS;
}

// 与 hccl CcuTempReduceScatterNhrMultiJettyMem2Mem1D::GetStepInfo 逐行对照：
// RS 的 sendTo/recvFrom 方向与 AG 相反（sendTo 为减 deltaRank），不可互抄。
HcclResult CcuTempKfcReduceScatterNHR1DMultiJettyMem2Mem::GetStepInfo(u32 step, KfcNhrStepInfo& stepInfo) const
{
    const u32 deltaRank = 1U << step;
    const u32 deltaSlice = 1U << (step + 1U);
    stepInfo.step = step;
    stepInfo.myRank = mySubCommRank_;
    stepInfo.toRank = (mySubCommRank_ + tempRankSize_ - deltaRank) % tempRankSize_;
    stepInfo.fromRank = (mySubCommRank_ + deltaRank) % tempRankSize_;
    stepInfo.nSlices = (tempRankSize_ - 1U + deltaRank) / deltaSlice;
    u32 txSliceIdx = stepInfo.toRank;
    u32 rxSliceIdx = mySubCommRank_;
    for (u32 i = 0; i < stepInfo.nSlices; ++i) {
        stepInfo.txSliceIdxs.push_back(txSliceIdx);
        stepInfo.rxSliceIdxs.push_back(rxSliceIdx);
        txSliceIdx = (txSliceIdx + tempRankSize_ - deltaSlice) % tempRankSize_;
        rxSliceIdx = (rxSliceIdx + tempRankSize_ - deltaSlice) % tempRankSize_;
    }
    return HCCL_SUCCESS;
}

uint32_t CcuTempKfcReduceScatterNHR1DMultiJettyMem2Mem::RemoteRankIdToSubRank(uint32_t remoteRankId) const
{
    const auto& ranks = subCommRanks_[0];
    const auto it = std::find(ranks.begin(), ranks.end(), remoteRankId);
    return it == ranks.end() ? 0U : static_cast<uint32_t>(std::distance(ranks.begin(), it));
}

HcclResult CcuTempKfcReduceScatterNHR1DMultiJettyMem2Mem::KernelRun(
    const OpParam& param, const TemplateDataParams& templateDataParams, TemplateResource& templateResource)
{
    // Per-round parameters are written by CcuPrepareForReduceScatterSoleNhrM2M and consumed by KFC server.
    (void)param;
    (void)templateDataParams;
    (void)templateResource;
    return HCCL_SUCCESS;
}

HcclResult CcuTempKfcReduceScatterNHR1DMultiJettyMem2Mem::GetRes(AlgResourceRequest& resourceRequest) const
{
    resourceRequest.slaveThreadNum = 0;
    resourceRequest.notifyNumOnMainThread = 0;
    resourceRequest.notifyNumPerThread.assign(resourceRequest.slaveThreadNum, 1);
    return HCCL_SUCCESS;
}

u64 CcuTempKfcReduceScatterNHR1DMultiJettyMem2Mem::GetThreadNum() const { return 1; }

u64 CcuTempKfcReduceScatterNHR1DMultiJettyMem2Mem::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType)
{
    // 与 hccl 一致：NHR 流直接远端规约（WriteReduce），不占用 scratch。
    (void)inBuffType;
    (void)outBuffType;
    return 0;
}

} // namespace mc2_ops_hccl
