/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

// 与 hccl src/ops/all_gather/algorithm/template/ccu/ccu_temp_all_gather_nhr_1D_mem2mem.cc 逐行对照移植。
// hccl -> KFC 适配：
//  1. CalcRes 只建 1 个 placeholder kernelInfo（KFC 单 mission），kernelFuncName 由 CcuTempKfcServer::CalcRes
//     识别替换；hccl 的 dieNum(1-2) kernel 划分与带宽比切分不迁移（框架限制，见迁移文档 4.2 节）。
//  2. 通道走 CalcChannelRequestNhr（hccl 非 MESH_1D_CLOS 路径）；rank2ChannelIdx 以 sub-rank 为键，
//     每 peer 保留首条通道（与 RS SoleNHR KFC 先例的 channelResort 语义一致）。
//  3. KernelRun 为空壳：每轮参数由 AIV 侧 CcuPrepareForAllGatherSoleNhrM2M 写入 xnData 队列。
//  4. GetStepInfo 的 toRank/fromRank/nSlices/tx/rx 公式与 hccl 逐行一致（sub-rank 口径）。
#include "alg_data_trans_wrapper.h"
#include "channel.h"
#include "ccu_temp_kfc_all_gather_nhr_1d_mem2mem.h"

namespace mc2_ops_hccl {

CcuTempKfcAllGatherNHR1DMem2Mem::CcuTempKfcAllGatherNHR1DMem2Mem(
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

HcclResult CcuTempKfcAllGatherNHR1DMem2Mem::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    CHK_RET(GetRes(resourceRequest));
    resourceRequest.ccuKernelNum.push_back(1U);

    std::vector<HcclChannelDesc> channelDescs;
    CHK_RET(CalcChannelRequestNhr(comm, param, topoInfo, subCommRanks_, channelDescs));

    auto kernelArg = std::make_shared<CcuKernelArgKfcAllGatherNHR1DMem2Mem>();
    // 每 peer 只保留首条通道，保证 kernel 的 rank2ChannelIdx.size() == channels.size() 不变式。
    std::vector<HcclChannelDesc> channelResort;
    for (const auto& channel : channelDescs) {
        const uint32_t remoteSubRank = RemoteRankIdToSubRank(channel.remoteRank);
        if (kernelArg->rank2ChannelIdx.count(remoteSubRank) == 0U) {
            kernelArg->rank2ChannelIdx[remoteSubRank] = static_cast<uint32_t>(channelResort.size());
            channelResort.push_back(channel);
        }
    }

    kernelArg->rankSize = tempRankSize_;
    kernelArg->rankId = mySubCommRank_;
    kernelArg->opParam = param;
    kernelArg->subCommRanks = subCommRanks_;
    CHK_RET(CalcNhrInfo(kernelArg->stepInfoVector));

    CcuKernelInfo kernelInfo{};
    CHK_SAFETY_FUNC_RET(
        strcpy_s(kernelInfo.kernelFuncName, sizeof(kernelInfo.kernelFuncName), "CcuKernelKfcAllGatherNHR1DMem2Mem"));
    kernelInfo.channels = channelResort;
    kernelInfo.setKernelArg(kernelArg);
    resourceRequest.ccuKernelInfos.push_back(kernelInfo);
    return HCCL_SUCCESS;
}

HcclResult CcuTempKfcAllGatherNHR1DMem2Mem::CalcNhrInfo(std::vector<KfcNhrStepInfo>& stepInfoVector) const
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

// 与 hccl CcuTempAllGatherNHR1DMem2Mem::GetStepInfo 逐行对照：
// deltaRank = 1<<(nSteps-1-step)；sendTo = (r+delta)%size；recvFrom = (r+size-delta)%size；
// nSlices = (size-1+deltaRank)/deltaSlice；tx 从 mySubCommRank 起、rx 从 fromRank 起，按 deltaSlice 递减。
HcclResult CcuTempKfcAllGatherNHR1DMem2Mem::GetStepInfo(u32 step, u32 stepNum, KfcNhrStepInfo& stepInfo) const
{
    const u32 deltaRank = 1U << (stepNum - 1U - step);
    const u32 deltaSlice = 1U << (stepNum - step);
    stepInfo.step = step;
    stepInfo.myRank = mySubCommRank_;
    stepInfo.toRank = (mySubCommRank_ + deltaRank) % tempRankSize_;
    stepInfo.fromRank = (mySubCommRank_ + tempRankSize_ - deltaRank) % tempRankSize_;
    stepInfo.nSlices = (tempRankSize_ - 1U + deltaRank) / deltaSlice;
    u32 txSliceIdx = mySubCommRank_;
    u32 rxSliceIdx = stepInfo.fromRank;
    for (u32 i = 0; i < stepInfo.nSlices; ++i) {
        stepInfo.txSliceIdxs.push_back(txSliceIdx);
        stepInfo.rxSliceIdxs.push_back(rxSliceIdx);
        txSliceIdx = (txSliceIdx + tempRankSize_ - deltaSlice) % tempRankSize_;
        rxSliceIdx = (rxSliceIdx + tempRankSize_ - deltaSlice) % tempRankSize_;
    }
    return HCCL_SUCCESS;
}

uint32_t CcuTempKfcAllGatherNHR1DMem2Mem::RemoteRankIdToSubRank(uint32_t remoteRankId) const
{
    const auto& ranks = subCommRanks_[0];
    const auto it = std::find(ranks.begin(), ranks.end(), remoteRankId);
    return it == ranks.end() ? 0U : static_cast<uint32_t>(std::distance(ranks.begin(), it));
}

HcclResult CcuTempKfcAllGatherNHR1DMem2Mem::KernelRun(
    const OpParam& param, const TemplateDataParams& templateDataParams, TemplateResource& templateResource)
{
    // 每轮参数由 CcuPrepareForAllGatherSoleNhrM2M 写入 xnData 队列，由 KFC server 消费。
    (void)param;
    (void)templateDataParams;
    (void)templateResource;
    return HCCL_SUCCESS;
}

HcclResult CcuTempKfcAllGatherNHR1DMem2Mem::GetRes(AlgResourceRequest& resourceRequest) const
{
    resourceRequest.slaveThreadNum = 0;
    resourceRequest.notifyNumOnMainThread = 0;
    resourceRequest.notifyNumPerThread.assign(resourceRequest.slaveThreadNum, 1);
    return HCCL_SUCCESS;
}

u64 CcuTempKfcAllGatherNHR1DMem2Mem::GetThreadNum() const { return 1; }

u64 CcuTempKfcAllGatherNHR1DMem2Mem::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType)
{
    (void)inBuffType;
    (void)outBuffType;
    return 0;
}

} // namespace mc2_ops_hccl
