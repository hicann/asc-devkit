/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "channel.h"
#include "ccu_temp_all_to_all_v_mesh_1D_multi_jetty.h"
#include "kernel/ccu_kernel_all_to_all_v_mesh1d_multi_jetty.h"

namespace mc2_ops_hccl {

CcuTempAllToAllVMesh1DMultiJetty::CcuTempAllToAllVMesh1DMultiJetty(
    const OpParam& param, const u32 rankId, const std::vector<std::vector<u32>>& subCommRanks)
    : CcuAlgTemplateBase(param, rankId, subCommRanks)
{
    const auto& ranks = subCommRanks[0];
    const auto it = std::find(ranks.begin(), ranks.end(), rankId);
    if (it != ranks.end()) {
        mySubCommRank_ = static_cast<uint32_t>(std::distance(ranks.begin(), it));
    }
    tempRankSize_ = static_cast<uint32_t>(ranks.size());
}

HcclResult CcuTempAllToAllVMesh1DMultiJetty::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    CHK_PRT_RET(
        param.opType != HcclCMDType::HCCL_CMD_ALLTOALLV,
        HCCL_ERROR(
            "[CcuTempAllToAllVMesh1DMultiJetty::CalcRes] only AllToAllV is supported, opType[%u].",
            static_cast<u32>(param.opType)),
        HCCL_E_NOT_SUPPORT);
    CHK_PRT_RET(
        tempRankSize_ < 2U || tempRankSize_ > CCU_MAX_RANK_SIZE || mySubCommRank_ >= tempRankSize_,
        HCCL_ERROR(
            "[CcuTempAllToAllVMesh1DMultiJetty::CalcRes] invalid rank metadata, rankSize[%u], rankId[%u].",
            tempRankSize_, mySubCommRank_),
        HCCL_E_PARA);
    CHK_RET(GetRes(resourceRequest));
    resourceRequest.ccuKernelNum.push_back(1U);

    // 照 hccl 模板 CalcRes：以 1DMESH 为优先级逐对端建链（mesh 链缺失时回退 clos 链）
    std::vector<HcclChannelDesc> channelDescs;
    CHK_RET(CalcChannelRequestMesh1DWithPriorityTopo(
        comm, param, topoInfo, subCommRanks_, channelDescs, CommTopo::COMM_TOPO_1DMESH));
    return CalcResByChannelDescsImpl(param, channelDescs, resourceRequest);
}

HcclResult CcuTempAllToAllVMesh1DMultiJetty::CalcResByChannelDescs(
    const OpParam& param, const std::vector<HcclChannelDesc>& channelDescs, AlgResourceRequest& resourceRequest) const
{
    CHK_RET(GetRes(resourceRequest));
    resourceRequest.ccuKernelNum.push_back(1U);
    return CalcResByChannelDescsImpl(param, channelDescs, resourceRequest);
}

HcclResult CcuTempAllToAllVMesh1DMultiJetty::CalcResByChannelDescsImpl(
    const OpParam& param, const std::vector<HcclChannelDesc>& channelDescs, AlgResourceRequest& resourceRequest) const
{
    // kernel 按对端序使用通道（每对端 1 条），通道数必须与远端数一致
    CHK_PRT_RET(
        channelDescs.size() + 1U != tempRankSize_,
        HCCL_ERROR(
            "[CcuTempAllToAllVMesh1DMultiJetty::CalcRes] invalid channel count[%zu] for rankSize[%u].",
            channelDescs.size(), tempRankSize_),
        HCCL_E_INTERNAL);

    std::vector<uint32_t> jettyNums;
    CHK_RET(SetJettyNums(jettyNums, false));
    auto kernelArg = std::make_shared<CcuKernelArgAllToAllVMesh1DMultiJetty>();
    kernelArg->rankSize = tempRankSize_;
    kernelArg->rankId = mySubCommRank_;
    kernelArg->opParam = param;
    kernelArg->subCommRanks = subCommRanks_;
    kernelArg->jettyNums = jettyNums;

    CcuKernelInfo kernelInfo{};
    CHK_SAFETY_FUNC_RET(
        strcpy_s(kernelInfo.kernelFuncName, sizeof(kernelInfo.kernelFuncName), "CcuKernelAllToAllVMesh1DMultiJetty"));
    kernelInfo.channels = channelDescs;
    kernelInfo.setKernelArg(kernelArg);
    resourceRequest.ccuKernelInfos.push_back(kernelInfo);
    return HCCL_SUCCESS;
}

HcclResult CcuTempAllToAllVMesh1DMultiJetty::KernelRun(
    const OpParam& param, const TemplateDataParams& templateDataParams, TemplateResource& templateResource)
{
    // Per-round parameters are written by CcuPrepareForConcurrentAllToAllV and consumed by the KFC server.
    (void)param;
    (void)templateDataParams;
    (void)templateResource;
    return HCCL_SUCCESS;
}

HcclResult CcuTempAllToAllVMesh1DMultiJetty::GetRes(AlgResourceRequest& resourceRequest) const
{
    resourceRequest.slaveThreadNum = 0;
    resourceRequest.notifyNumOnMainThread = 0;
    resourceRequest.notifyNumPerThread.assign(resourceRequest.slaveThreadNum, 1);
    return HCCL_SUCCESS;
}

u64 CcuTempAllToAllVMesh1DMultiJetty::GetThreadNum() const { return 1; }

u64 CcuTempAllToAllVMesh1DMultiJetty::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType)
{
    (void)inBuffType;
    (void)outBuffType;
    return 0;
}

HcclResult CcuTempAllToAllVMesh1DMultiJetty::SetJettyNums(std::vector<uint32_t>& jettyNums, const bool multijetty) const
{
    // 照 hccl FillMeshJettyNums：本 rank 单 jetty，远端按开关选择；并发算法恒 multijetty=false
    constexpr uint32_t SINGLE_JETTY_NUM = 1U;
    jettyNums.resize(tempRankSize_, 0U);
    for (uint32_t i = 0; i < tempRankSize_; i++) {
        if (i == mySubCommRank_) {
            jettyNums[i] = SINGLE_JETTY_NUM;
        } else if (multijetty) {
            jettyNums[i] = MAX_JETTY_NUM;
        } else {
            jettyNums[i] = SINGLE_JETTY_NUM;
        }
    }
    return HCCL_SUCCESS;
}

} // namespace mc2_ops_hccl
