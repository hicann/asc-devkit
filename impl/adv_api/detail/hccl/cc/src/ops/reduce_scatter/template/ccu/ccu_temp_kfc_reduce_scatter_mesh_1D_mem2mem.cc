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
#include "ccu_temp_kfc_reduce_scatter_mesh_1D_mem2mem.h"

namespace mc2_ops_hccl {

CcuTempKfcReduceScatterMesh1DMem2Mem::CcuTempKfcReduceScatterMesh1DMem2Mem(
    const OpParam& param, const u32 rankId, const std::vector<std::vector<u32>>& subCommRanks)
    : CcuTempKfcMesh1DMem2MemBase(param, rankId, subCommRanks)
{}

CcuTempKfcReduceScatterMesh1DMem2Mem::~CcuTempKfcReduceScatterMesh1DMem2Mem() {}

HcclResult CcuTempKfcReduceScatterMesh1DMem2Mem::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    CHK_RET(GetRes(resourceRequest));
    resourceRequest.ccuKernelNum.push_back(1);

    CcuKernelInfo kernelInfo;
    strcpy_s(kernelInfo.kernelFuncName, sizeof(kernelInfo.kernelFuncName), "CcuKernelKfcReduceScatterMesh1DMem2Mem");

    std::vector<HcclChannelDesc> channelDescs;
    CHK_RET(CalcChannelRequestMesh1D(comm, param, topoInfo, subCommRanks_, channelDescs));

    // CcuKernelKfcServer replaces this placeholder before registration and reuses these Mesh1D channels.
    kernelInfo.channels = channelDescs;
    resourceRequest.ccuKernelInfos.push_back(kernelInfo);
    return HcclResult::HCCL_SUCCESS;
}

HcclResult CcuTempKfcReduceScatterMesh1DMem2Mem::KernelRun(
    const OpParam& param, const TemplateDataParams& templateDataParams, TemplateResource& templateResource)
{
    // Per-round parameters are written by CcuPrepareForReduceScatterM2M and consumed by KFC server.
    (void)param;
    (void)templateDataParams;
    (void)templateResource;
    return HcclResult::HCCL_SUCCESS;
}

u64 CcuTempKfcReduceScatterMesh1DMem2Mem::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType)
{
    // Keep the pr_4523 one-shot contract: scratch is partitioned into rankSize slices by the sub-kernel.
    (void)inBuffType;
    (void)outBuffType;
    return templateRankSize_;
}

} // namespace mc2_ops_hccl
