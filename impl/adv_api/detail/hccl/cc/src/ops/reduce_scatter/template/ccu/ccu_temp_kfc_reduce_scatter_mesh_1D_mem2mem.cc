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
    CHK_SAFETY_FUNC_RET(strcpy_s(
        kernelInfo.kernelFuncName, sizeof(kernelInfo.kernelFuncName), "CcuKernelKfcReduceScatterMesh1DMem2Mem"));

    std::vector<HcclChannelDesc> channelDescs;
    // UBX（MESH_1D_CLOS，mesh/clos 同层）机型：CalcChannelRequestMesh1D 按协议逐链路建通道，会把每对端
    // 的全部 clos 链路收进 kernelInfo（实测每对 18 条，2P 即超 CCU_MAX_RANK_SIZE=16 上限），
    // 须与 AG 的 KFC Mesh 模板/hccl 母本同款：走 WithPriorityTopo 按对端选 1 条 mesh 优先链路。
    if (topoInfo->level0Topo != Level0Shape::MESH_1D_CLOS) {
        CHK_RET(CalcChannelRequestMesh1DFullMesh(comm, param, topoInfo, subCommRanks_, channelDescs));
        CHK_RET(FlattenChannelsToSingleDie(comm, channelDescs));
    } else {
        CHK_RET(CalcChannelRequestMesh1DWithPriorityTopo(
            comm, param, topoInfo, subCommRanks_, channelDescs, CommTopo::COMM_TOPO_1DMESH));
        for (const auto& channel : channelDescs) {
            if (channel.channelProtocol != COMM_PROTOCOL_UBC_CTP) {
                HCCL_ERROR(
                    "[CcuTempKfcReduceScatterMesh1DMem2Mem][CalcRes] channelProtocol: %u", channel.channelProtocol);
                return HCCL_E_INTERNAL;
            }
        }
    }

    auto kernelArg = std::make_shared<CcuKernelArgKfcReduceScatterMesh1DMem2Mem>();
    kernelArg->rankSize = subCommRanks_[0].size();
    kernelArg->rankId = mySubCommRank_;
    kernelArg->opParam = param;
    kernelArg->subCommRanks = subCommRanks_;
    kernelInfo.setKernelArg(kernelArg);

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

HcclResult CcuTempKfcReduceScatterMesh1DMem2Mem::FlattenChannelsToSingleDie(
    const HcclComm comm, std::vector<HcclChannelDesc>& channels)
{
    std::map<u32, std::vector<HcclChannelDesc>> rankIdToChannelDesc;
    CHK_RET(RestoreChannelMap(channels, rankIdToChannelDesc));
    u32 enableDieNum = 0;
    u32 enableDieId = 0;
    CHK_RET(GetDieInfoFromChannelDescs(comm, rankIdToChannelDesc, myRank_, enableDieNum, enableDieId));
    CHK_PRT_RET(
        enableDieNum < 1 || enableDieNum > CCU_DIE_NUM_MAX_2,
        HCCL_ERROR("[CcuTempKfcReduceScatterMesh1DMem2Mem] get channelDescs fail, dieNum[%u].", enableDieNum),
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
