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
#include "ccu_temp_kfc_all_gather_nhr_1D_multi_jetty_mem2mem.h"

namespace mc2_ops_hccl {
namespace {
constexpr uint32_t KFC_NHR_JETTY_NUM = 4U;
}

CcuTempKfcAllGatherNHR1DMultiJettyMem2Mem::CcuTempKfcAllGatherNHR1DMultiJettyMem2Mem(
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

HcclResult CcuTempKfcAllGatherNHR1DMultiJettyMem2Mem::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    CHK_RET(GetRes(resourceRequest));
    resourceRequest.ccuKernelNum.push_back(1U);

    std::vector<HcclChannelDesc> channelDescs;
    CHK_RET(CalcChannelRequestNhrMultiJetty(comm, param, topoInfo, subCommRanks_, channelDescs));
    for (const auto& channel : channelDescs) {
        CHK_PRT_RET(
            channel.channelProtocol != COMM_PROTOCOL_UBC_CTP,
            HCCL_ERROR("[CcuTempKfcAllGatherNHR1DMultiJettyMem2Mem] invalid protocol[%u]", channel.channelProtocol),
            HCCL_E_INTERNAL);
    }

    auto kernelArg = std::make_shared<CcuKernelArgKfcAllGatherNHR1DMultiJettyMem2Mem>();
    kernelArg->rankSize = tempRankSize_;
    kernelArg->rankId = mySubCommRank_;
    kernelArg->jettyNum = KFC_NHR_JETTY_NUM;
    kernelArg->opParam = param;
    kernelArg->subCommRanks = subCommRanks_;
    for (u32 i = 0; i < channelDescs.size(); ++i) {
        kernelArg->rank2ChannelIdx[RemoteRankIdToSubRank(channelDescs[i].remoteRank)] = i;
    }
    CHK_RET(CalcNhrInfo(kernelArg->stepInfoVector));

    CcuKernelInfo kernelInfo{};
    CHK_SAFETY_FUNC_RET(strcpy_s(
        kernelInfo.kernelFuncName, sizeof(kernelInfo.kernelFuncName), "CcuKernelAllGatherNHR1DMultiJettyMem2Mem"));
    kernelInfo.channels = channelDescs;
    kernelInfo.setKernelArg(kernelArg);
    resourceRequest.ccuKernelInfos.push_back(kernelInfo);
    return HCCL_SUCCESS;
}

HcclResult CcuTempKfcAllGatherNHR1DMultiJettyMem2Mem::CalcNhrInfo(std::vector<KfcNhrStepInfo>& stepInfoVector) const
{
    u32 stepNum = 0;
    for (u32 ranks = tempRankSize_ - 1U; ranks != 0U; ranks >>= 1U) {
        ++stepNum;
    }
    for (u32 step = 0; step < stepNum; ++step) {
        KfcNhrStepInfo stepInfo;
        CHK_RET(GetStepInfo(step, stepNum, stepInfo));
        stepInfoVector.push_back(stepInfo);
    }
    return HCCL_SUCCESS;
}

HcclResult CcuTempKfcAllGatherNHR1DMultiJettyMem2Mem::GetStepInfo(u32 step, u32 stepNum, KfcNhrStepInfo& stepInfo) const
{
    const u32 deltaRank = 1U << (stepNum - 1U - step);
    const u32 deltaSlice = 1U << (stepNum - step);
    stepInfo.step = step;
    stepInfo.myRank = mySubCommRank_;
    stepInfo.toRank = (mySubCommRank_ + deltaRank) % tempRankSize_;
    stepInfo.fromRank = (mySubCommRank_ + tempRankSize_ - deltaRank) % tempRankSize_;
    stepInfo.nSlices = (tempRankSize_ - 1U + deltaRank) / deltaSlice;
    u32 txSlice = mySubCommRank_;
    u32 rxSlice = stepInfo.fromRank;
    for (u32 i = 0; i < stepInfo.nSlices; ++i) {
        stepInfo.txSliceIdxs.push_back(txSlice);
        stepInfo.rxSliceIdxs.push_back(rxSlice);
        txSlice = (txSlice + tempRankSize_ - deltaSlice) % tempRankSize_;
        rxSlice = (rxSlice + tempRankSize_ - deltaSlice) % tempRankSize_;
    }
    return HCCL_SUCCESS;
}

uint32_t CcuTempKfcAllGatherNHR1DMultiJettyMem2Mem::RemoteRankIdToSubRank(uint32_t remoteRankId) const
{
    const auto& ranks = subCommRanks_[0];
    const auto it = std::find(ranks.begin(), ranks.end(), remoteRankId);
    return it == ranks.end() ? 0U : static_cast<uint32_t>(std::distance(ranks.begin(), it));
}

HcclResult CcuTempKfcAllGatherNHR1DMultiJettyMem2Mem::KernelRun(
    const OpParam& param, const TemplateDataParams& templateDataParams, TemplateResource& templateResource)
{
    (void)param;
    (void)templateDataParams;
    (void)templateResource;
    return HCCL_SUCCESS;
}

HcclResult CcuTempKfcAllGatherNHR1DMultiJettyMem2Mem::GetRes(AlgResourceRequest& resourceRequest) const
{
    resourceRequest.slaveThreadNum = 0;
    resourceRequest.notifyNumOnMainThread = 0;
    resourceRequest.notifyNumPerThread.assign(resourceRequest.slaveThreadNum, 1);
    return HCCL_SUCCESS;
}

u64 CcuTempKfcAllGatherNHR1DMultiJettyMem2Mem::GetThreadNum() const { return 1; }

u64 CcuTempKfcAllGatherNHR1DMultiJettyMem2Mem::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType)
{
    (void)inBuffType;
    (void)outBuffType;
    return 0;
}

} // namespace mc2_ops_hccl
