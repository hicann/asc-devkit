/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "channel.h"
#include "hccl_ccu_res.h"
#include "ccu_assist_pub.h"
#include "kfc_server_protocol.h"
#include "ccu_temp_kfc_reduce_scatter_mesh_1D_mem2mem_peer_only.h"

namespace mc2_ops_hccl {

CcuTempKfcReduceScatterMesh1DMem2MemPeerOnly::CcuTempKfcReduceScatterMesh1DMem2MemPeerOnly(
    const OpParam& param, const u32 rankId, const std::vector<std::vector<u32>>& subCommRanks)
    : CcuTempKfcMesh1DMem2MemBase(param, rankId, subCommRanks)
{}

CcuTempKfcReduceScatterMesh1DMem2MemPeerOnly::~CcuTempKfcReduceScatterMesh1DMem2MemPeerOnly() {}

HcclResult CcuTempKfcReduceScatterMesh1DMem2MemPeerOnly::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    CHK_PRT_RET(
        subCommRanks_.empty() || subCommRanks_[0].size() != 2U,
        HCCL_ERROR(
            "[CcuTempKfcReduceScatterMesh1DMem2MemPeerOnly][CalcRes] only TP2 is supported, rankSize[%zu].",
            subCommRanks_.empty() ? 0U : subCommRanks_[0].size()),
        HCCL_E_NOT_SUPPORT);

    CHK_RET(GetRes(resourceRequest));
    resourceRequest.ccuKernelNum.push_back(1);

    CcuKernelInfo kernelInfo;
    CHK_SAFETY_FUNC_RET(strcpy_s(
        kernelInfo.kernelFuncName, sizeof(kernelInfo.kernelFuncName), KFC_REDUCE_SCATTER_PEER_ONLY_KERNEL_NAME));

    std::vector<HcclChannelDesc> channelDescs;
    CHK_RET(CalcChannelRequestMesh1D(comm, param, topoInfo, subCommRanks_, channelDescs));
    CHK_PRT_RET(
        channelDescs.size() != 1U,
        HCCL_ERROR(
            "[CcuTempKfcReduceScatterMesh1DMem2MemPeerOnly][CalcRes] expected one peer channel, got[%zu].",
            channelDescs.size()),
        HCCL_E_NOT_SUPPORT);

    kernelInfo.channels = channelDescs;
    resourceRequest.ccuKernelInfos.push_back(kernelInfo);
    return HcclResult::HCCL_SUCCESS;
}

HcclResult CcuTempKfcReduceScatterMesh1DMem2MemPeerOnly::KernelRun(
    const OpParam& param, const TemplateDataParams& templateDataParams, TemplateResource& templateResource)
{
    (void)param;
    (void)templateDataParams;
    (void)templateResource;
    return HcclResult::HCCL_SUCCESS;
}

u64 CcuTempKfcReduceScatterMesh1DMem2MemPeerOnly::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType)
{
    (void)inBuffType;
    (void)outBuffType;
    return 0U;
}

} // namespace mc2_ops_hccl
