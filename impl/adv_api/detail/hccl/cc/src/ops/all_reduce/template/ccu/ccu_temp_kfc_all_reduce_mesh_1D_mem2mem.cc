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
#include "ccu_temp_kfc_all_reduce_mesh_1D_mem2mem.h"
#include "kernel/ccu_kernel_kfc_all_reduce_mesh1d_mem2mem.h"

namespace mc2_ops_hccl {

CcuTempKfcAllReduceMesh1DMem2Mem::CcuTempKfcAllReduceMesh1DMem2Mem(
    const OpParam& param, const u32 rankId, const std::vector<std::vector<u32>>& subCommRanks)
    : CcuTempKfcMesh1DMem2MemBase(param, rankId, subCommRanks)
{}

CcuTempKfcAllReduceMesh1DMem2Mem::~CcuTempKfcAllReduceMesh1DMem2Mem() {}

HcclResult CcuTempKfcAllReduceMesh1DMem2Mem::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    CHK_RET(GetRes(resourceRequest));
    resourceRequest.ccuKernelNum.push_back(1);

    CcuKernelInfo kernelInfo;
    strcpy_s(kernelInfo.kernelFuncName, sizeof(kernelInfo.kernelFuncName), "CcuKernelAllReduceMesh1DMem2Mem");
    kernelInfo.kernelFunc = reinterpret_cast<void*>(CcuKfcAllReduceMesh1DMem2MemKernel);

    std::vector<HcclChannelDesc> channelDescs;
    if (topoInfo->level0Topo != Level0Shape::MESH_1D_CLOS) {
        CHK_RET(CalcChannelRequestMesh1DFullMesh(comm, param, topoInfo, subCommRanks_, channelDescs));
        CHK_RET(FlattenChannelsToSingleDie(comm, channelDescs));
    } else {
        CHK_RET(CalcChannelRequestMesh1DWithPriorityTopo(
            comm, param, topoInfo, subCommRanks_, channelDescs, CommTopo::COMM_TOPO_1DMESH));
        for (const auto& channel : channelDescs) {
            if (channel.channelProtocol != COMM_PROTOCOL_UBC_CTP) {
                HCCL_ERROR("[CcuTempKfcAllReduceMesh1DMem2Mem][CalcRes] channelProtocol: %u", channel.channelProtocol);
                return HCCL_E_INTERNAL;
            }
        }
    }

    auto kernelArg = std::make_shared<CcuKernelArgAllReduceKfc>();
    kernelArg->rankSize = subCommRanks_[0].size();
    kernelArg->rankId = mySubCommRank_;
    kernelArg->opParam = param;
    kernelArg->subCommRanks = subCommRanks_;
    kernelInfo.setKernelArg(kernelArg);
    kernelInfo.channels = channelDescs;
    resourceRequest.ccuKernelInfos.push_back(kernelInfo);
    return HcclResult::HCCL_SUCCESS;
}

HcclResult CcuTempKfcAllReduceMesh1DMem2Mem::KernelRun(
    const OpParam& param, const TemplateDataParams& templateDataParams, TemplateResource& templateResource)
{
    // Per-round parameters are written by CcuPrepareForAllReduceM2M and consumed by KFC server.
    (void)param;
    (void)templateDataParams;
    (void)templateResource;
    return HcclResult::HCCL_SUCCESS;
}

u64 CcuTempKfcAllReduceMesh1DMem2Mem::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType)
{
    // AllReduce partitions scratch into rankSize slices for the reduce-scatter step.
    (void)inBuffType;
    (void)outBuffType;
    return templateRankSize_;
}

HcclResult CcuTempKfcAllReduceMesh1DMem2Mem::FlattenChannelsToSingleDie(
    HcclComm comm, std::vector<HcclChannelDesc>& channels)
{
    std::map<u32, std::vector<HcclChannelDesc>> rankIdToChannelDesc;
    CHK_RET(RestoreChannelMap(channels, rankIdToChannelDesc));
    if (rankIdToChannelDesc.empty()) {
        return HCCL_SUCCESS;
    }
    u32 enableDieNum = 0;
    u32 enableDieId = 0;
    CHK_RET(GetDieInfoFromChannelDescs(comm, rankIdToChannelDesc, myRank_, enableDieNum, enableDieId));
    CHK_PRT_RET(
        enableDieNum < 1 || enableDieNum > CCU_DIE_NUM_MAX_2,
        HCCL_ERROR("[CcuTempKfcAllReduceMesh1DMem2Mem] get channelDescs fail, dieNum[%u].", enableDieNum),
        HCCL_E_INTERNAL);

    std::vector<std::vector<HcclChannelDesc>> channelsPerDie(enableDieNum);
    std::map<u32, u32> rank2ChannelIdx;
    constexpr u32 DIE_0 = 0;
    constexpr u32 DIE_1 = 1;
    for (u32 rank : subCommRanks_[0]) {
        if (rank == myRank_) {
            continue;
        }
        if (enableDieNum == 1) {
            CHK_RET(SelectChannelToVec(
                comm, myRank_, rank, rankIdToChannelDesc, enableDieId, rank2ChannelIdx, channelsPerDie[DIE_0]));
        } else {
            CHK_RET(SelectChannelToVec(
                comm, myRank_, rank, rankIdToChannelDesc, DIE_0, rank2ChannelIdx, channelsPerDie[DIE_0]));
            CHK_RET(SelectChannelToVec(
                comm, myRank_, rank, rankIdToChannelDesc, DIE_1, rank2ChannelIdx, channelsPerDie[DIE_1]));
        }
    }
    if (enableDieNum > 1) {
        // 跨框 die0 连 die1 场景下，将出框端口数多的 die 的通道组放在前面
        CHK_RET(ReverseChannelPerDieIfNeed(comm, myRank_, channelsPerDie));
    }
    channels = channelsPerDie[DIE_0];
    return HCCL_SUCCESS;
}

} // namespace mc2_ops_hccl
