/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_temp_all_to_all_v_mesh_1D.h"

#include "channel.h"
#include "kernel/ccu_kernel_all_to_all_v_mesh1d.h"

namespace mc2_ops_hccl {

CcuTempAlltoAllVMesh1D::CcuTempAlltoAllVMesh1D(
    const OpParam& param, u32 rankId, const std::vector<std::vector<u32>>& subCommRanks)
    : CcuAlgTemplateBase(param, rankId, subCommRanks)
{
    tempRankSize_ = subCommRanks[0].size();
    auto it = std::find(subCommRanks[0].begin(), subCommRanks[0].end(), rankId);
    if (it != subCommRanks[0].end()) {
        mySubCommRank_ = std::distance(subCommRanks[0].begin(), it);
    }
}

HcclResult CcuTempAlltoAllVMesh1D::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    CHK_PRT_RET(
        param.opType != HcclCMDType::HCCL_CMD_ALLTOALLV || !param.isKfc,
        HCCL_ERROR(
            "[CcuTempAlltoAllVMesh1D::CalcRes] only KFC AllToAllV is supported, opType[%u], isKfc[%d].",
            static_cast<u32>(param.opType), static_cast<int>(param.isKfc)),
        HCCL_E_NOT_SUPPORT);
    CHK_PRT_RET(
        tempRankSize_ < 2U || tempRankSize_ > CCU_MAX_RANK_SIZE || mySubCommRank_ >= tempRankSize_,
        HCCL_ERROR(
            "[CcuTempAlltoallVMesh1D::CalcRes] invalid rank metadata, rankSize[%u], rankId[%u].", tempRankSize_,
            mySubCommRank_),
        HCCL_E_PARA);

    resourceRequest.notifyNumOnMainThread = 0;
    resourceRequest.slaveThreadNum = 0;
    resourceRequest.ccuKernelNum.push_back(1);

    CcuKernelInfo kernelInfo{};
    CHK_SAFETY_FUNC_RET(
        strcpy_s(kernelInfo.kernelFuncName, sizeof(kernelInfo.kernelFuncName), "CcuKernelAlltoAllVMesh1D"));
    std::vector<HcclChannelDesc> channelDescs;
    if (topoInfo->level0Topo != Level0Shape::MESH_1D_CLOS) {
        CHK_RET(CalcChannelRequestMesh1D(comm, param, topoInfo, subCommRanks_, channelDescs));
    } else {
        CHK_RET(CalcChannelRequestMesh1DWithPriorityTopo(
            comm, param, topoInfo, subCommRanks_, channelDescs, CommTopo::COMM_TOPO_1DMESH));
        for (const auto& channel : channelDescs) {
            CHK_PRT_RET(
                channel.channelProtocol != COMM_PROTOCOL_UBC_CTP,
                HCCL_ERROR("[CcuTempAlltoAllVMesh1D::CalcRes] invalid channel protocol[%u].", channel.channelProtocol),
                HCCL_E_INTERNAL);
        }
    }
    CHK_PRT_RET(
        channelDescs.size() + 1U != tempRankSize_,
        HCCL_ERROR(
            "[CcuTempAlltoallVMesh1D::CalcRes] invalid channel count[%zu] for rankSize[%u].", channelDescs.size(),
            tempRankSize_),
        HCCL_E_INTERNAL);

    auto kernelArg = std::make_shared<CcuKernelArgAlltoAllVMesh1D>();
    kernelArg->rankSize = subCommRanks_[0].size();
    kernelArg->rankId = mySubCommRank_;
    kernelArg->loadFromMem = param.isMc2;
    kernelArg->opParam = param;
    kernelArg->subCommRanks = subCommRanks_;
    kernelInfo.setKernelArg(kernelArg);
    kernelInfo.channels = channelDescs;
    resourceRequest.ccuKernelInfos.push_back(kernelInfo);
    return HCCL_SUCCESS;
}

HcclResult CcuTempAlltoAllVMesh1D::KernelRun(
    const OpParam& param, const TemplateDataParams& templateDataParams, TemplateResource& templateResource)
{
    // Per-round parameters are written by CcuPrepareForAllToAllV and consumed by the KFC server.
    (void)param;
    (void)templateDataParams;
    (void)templateResource;
    return HCCL_SUCCESS;
}

u64 CcuTempAlltoAllVMesh1D::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType)
{
    (void)inBuffType;
    (void)outBuffType;
    return 0;
}

} // namespace mc2_ops_hccl
