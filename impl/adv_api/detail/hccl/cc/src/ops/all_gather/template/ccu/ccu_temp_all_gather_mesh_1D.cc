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
#include "ccu_temp_all_gather_mesh_1D.h"

namespace mc2_ops_hccl {

CcuTempAllGatherMesh1D::CcuTempAllGatherMesh1D(
    const OpParam& param, const u32 rankId, const std::vector<std::vector<u32>>& subCommRanks)
    : CcuAlgTemplateBase(param, rankId, subCommRanks)
{
    std::vector<u32> ranks = subCommRanks[0];
    templateRankSize_ = ranks.size();
    // 获取本卡在子通信域(如果有)中的rankid
    auto it = std::find(ranks.begin(), ranks.end(), rankId);
    if (it != ranks.end()) {
        mySubCommRank_ = std::distance(ranks.begin(), it);
    }
}

CcuTempAllGatherMesh1D::~CcuTempAllGatherMesh1D() {}

HcclResult CcuTempAllGatherMesh1D::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    // 不需要从流
    resourceRequest.notifyNumOnMainThread = 0;
    resourceRequest.slaveThreadNum = 0;
    // 多少个kernel
    resourceRequest.ccuKernelNum.push_back(1);
    HCCL_DEBUG(
        "[CcuTempAllGatherMesh1D::CalcRes] notifyNumOnMainThread[%u] slaveThreadNum[%u]",
        resourceRequest.notifyNumOnMainThread, resourceRequest.slaveThreadNum);

    // KFC Server will replace this placeholder kernelInfo before CCU kernel registration.
    CcuKernelInfo kernelInfo;

    CHK_SAFETY_FUNC_RET(
        strcpy_s(kernelInfo.kernelFuncName, sizeof(kernelInfo.kernelFuncName), "CcuKernelAllGatherMesh1D"));

    std::vector<HcclChannelDesc> channelDescs;
    if (topoInfo->level0Topo != Level0Shape::MESH_1D_CLOS) {
        CHK_RET(CalcChannelRequestMesh1D(comm, param, topoInfo, subCommRanks_, channelDescs));
    } else {
        CHK_RET(CalcChannelRequestMesh1DWithPriorityTopo(
            comm, param, topoInfo, subCommRanks_, channelDescs, CommTopo::COMM_TOPO_1DMESH));
        for (auto channel : channelDescs) {
            if (channel.channelProtocol != COMM_PROTOCOL_UBC_CTP) {
                HCCL_ERROR("[CcuTempAllGatherMesh1D][CalcRes] channelProtocol: %u", channel.channelProtocol);
                return HCCL_E_INTERNAL;
            }
        }
    }
    HCCL_DEBUG("[CcuTempAllGatherMesh1D::CalcRes] Get Mesh Channel Success!");

    // Disabled with the legacy direct-launch kernel. KFC Server reconstructs CcuKernelArgKfcServer
    // from param/rank/subCommRanks and reuses the channels below.
    // kernelInfo.kernelArg =
    //     std::make_shared<CcuKernelArgAllGatherMesh1D>(subCommRanks_[0].size(), mySubCommRank_, param, subCommRanks_);
    kernelInfo.channels = channelDescs;
    resourceRequest.ccuKernelInfos.push_back(kernelInfo);

    HCCL_DEBUG(
        "[CcuTempAllGatherMesh1D::CalcRes] channelDescs.size()=%llu, dimsize=%llu, "
        "ccuKernelInfos.size()=%llu",
        channelDescs.size(), subCommRanks_[0].size(), resourceRequest.ccuKernelInfos.size());

    return HcclResult::HCCL_SUCCESS;
}

HcclResult CcuTempAllGatherMesh1D::KernelRun(
    const OpParam& param, const TemplateDataParams& templateDataParams, TemplateResource& templateResource)
{
    buffInfo_ = templateDataParams.buffInfo;

    uint64_t inputAddr = PointerToAddr(buffInfo_.inputPtr) + buffInfo_.inBuffBaseOff;
    uint64_t outputAddr = PointerToAddr(buffInfo_.outputPtr) + buffInfo_.outBuffBaseOff;
    uint64_t token;
    CHK_RET(GetToken(buffInfo_, token));

    uint32_t rankId = mySubCommRank_;
    uint64_t offset = rankId * templateDataParams.outputSliceStride;
    uint64_t sliceSize = templateDataParams.sliceSize;

    HcclDataType dataType = param.DataDes.dataType;
    uint64_t dataTypeSize = DATATYPE_SIZE_TABLE[dataType];
    uint64_t dataCount = sliceSize / dataTypeSize;
    if (dataCount == 0) {
        HCCL_INFO("[CcuTempAllGatherMesh1D] DataCount == 0, Template Run Ends.");
        return HcclResult::HCCL_SUCCESS;
    }

    CcuKernelSubmitInfo submitInfo{};
    submitInfo.kernelHandle = templateResource.ccuKernels[0];
    CHK_RET(FillCachedArgs(submitInfo, inputAddr, outputAddr, token, offset, sliceSize));
    templateResource.submitInfos.push_back(submitInfo);

    return HcclResult::HCCL_SUCCESS;
}

u64 CcuTempAllGatherMesh1D::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType)
{
    // one shot 场景，scratch Buffer 需要是 usrIn的rankSize倍
    (void)inBuffType;
    (void)outBuffType;
    return 0;
}

u64 CcuTempAllGatherMesh1D::GetThreadNum() const { return 1; }

HcclResult CcuTempAllGatherMesh1D::GetRes(AlgResourceRequest& resourceRequest) const
{
    resourceRequest.slaveThreadNum = 0;
    resourceRequest.notifyNumOnMainThread = 0;
    resourceRequest.notifyNumPerThread.assign(resourceRequest.slaveThreadNum, 1);
    return HCCL_SUCCESS;
}
} // namespace mc2_ops_hccl
