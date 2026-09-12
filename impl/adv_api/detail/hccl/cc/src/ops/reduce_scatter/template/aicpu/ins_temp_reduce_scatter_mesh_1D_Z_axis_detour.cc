/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ins_temp_reduce_scatter_mesh_1D_Z_axis_detour.h"

namespace mc2_ops_hccl {

InsTempReduceScatterMesh1DZAxisDetour::InsTempReduceScatterMesh1DZAxisDetour(
    const OpParam& param, const u32 rankId, const std::vector<std::vector<u32>>& subCommRanks)
    : InsTempReduceScatterMesh1D(param, rankId, subCommRanks)
{}

InsTempReduceScatterMesh1DZAxisDetour::~InsTempReduceScatterMesh1DZAxisDetour() {}

HcclResult InsTempReduceScatterMesh1DZAxisDetour::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    CHK_PRT_RET(
        topoInfo == nullptr, HCCL_ERROR("[InsTempReduceScatterMesh1DZAxisDetour][CalcRes] topoInfo is nullptr"),
        HCCL_E_PARA);
    std::vector<HcclChannelDesc> level0Channels;
    CHK_RET(CalcChannelRequestMesh1DLevel0(comm, param, topoInfo, subCommRanks_, level0Channels));
    std::vector<HcclChannelDesc> level1Channels;
    CHK_RET(CalcChannelRequestMesh1DLevel1(comm, param, topoInfo, subCommRanks_, level1Channels));
    std::vector<HcclChannelDesc> mergedChannels;
    mergedChannels.insert(mergedChannels.end(), level0Channels.begin(), level0Channels.end());
    mergedChannels.insert(mergedChannels.end(), level1Channels.begin(), level1Channels.end());
    resourceRequest.channels.push_back(mergedChannels);
    level0ChannelNumPerRank_ = level0Channels.empty() ? 0 : CalcChannelsPerRank(level0Channels);
    level1ChannelNumPerRank_ = level1Channels.empty() ? 0 : CalcChannelsPerRank(level1Channels);
    channelsPerRank_ = level0ChannelNumPerRank_ + level1ChannelNumPerRank_;
    CHK_RET(GetRes(resourceRequest));
    HCCL_DEBUG(
        "[InsTempReduceScatterMesh1DZAxisDetour][CalcRes] myRank[%u], channelsPerRank_[%u], "
        "level0ChannelNum[%zu], level1ChannelNum[%zu], notifyNumOnMainThread[%u], slaveThreadNum[%u]",
        myRank_, channelsPerRank_, level0Channels.size(), level1Channels.size(), resourceRequest.notifyNumOnMainThread,
        resourceRequest.slaveThreadNum);
    HCCL_INFO(
        "[InsTempReduceScatterMesh1DZAxisDetour][CalcRes]myRank[%u], channelsPerRank_[%u], "
        "level0ChannelNumPerRank_[%u], level1ChannelNumPerRank_[%u], level0DataRatio_[%.2f]",
        myRank_, channelsPerRank_, level0ChannelNumPerRank_, level1ChannelNumPerRank_, level0DataRatio_);
    return HCCL_SUCCESS;
}

u64 InsTempReduceScatterMesh1DZAxisDetour::GetThreadNum() const
{
    u32 threadNum = templateRankSize_ > 1 ? ((templateRankSize_ - 1) * channelsPerRank_ + 1) : 1;
    HCCL_INFO(
        "[InsTempReduceScatterMesh1DZAxisDetour][GetThreadNum] templateRankSize_[%u] channelsPerRank_[%u] "
        "threadNum[%u]",
        templateRankSize_, channelsPerRank_, threadNum);
    return threadNum;
}

HcclResult InsTempReduceScatterMesh1DZAxisDetour::CalcDataSplitByPortGroup(
    const u64 totalDataCount, const u64 dataTypeSize, const std::vector<ChannelInfo>& channels,
    std::vector<u64>& elemCountOut, std::vector<u64>& sizeOut, std::vector<u64>& elemOffset)
{
    HCCL_INFO("[InsTempReduceScatterMesh1DZAxisDetour][CalcDataSplitByPortGroup] Run Start");
    return CalcDataSplitByPortGroupZAxisDetour(
        totalDataCount, dataTypeSize, channels, elemCountOut, sizeOut, elemOffset, level0ChannelNumPerRank_,
        level1ChannelNumPerRank_, level0DataRatio_);
}

HcclResult InsTempReduceScatterMesh1DZAxisDetour::SetchannelsPerRank(
    const std::map<u32, std::vector<ChannelInfo>>& channels)
{
    CHK_PRT_RET(channels.empty(), HCCL_ERROR("[SetchannelsPerRank] channels is empty."), HCCL_E_INTERNAL);
    channelsPerRank_ = CalcChannelsPerRank(channels);
    if (channelsPerRank_ > 1) {
        level0ChannelNumPerRank_ = MESH_CHANNELS_NUM;
        level1ChannelNumPerRank_ = channelsPerRank_ - level0ChannelNumPerRank_;
        level0DataRatio_ = (templateRankSize_ == 2) ? 0.25f : 0.5f;
    }
    HCCL_INFO(
        "[InsTempReduceScatterMesh1DZAxisDetour][SetchannelsPerRank], channelsPerRank_[%u], "
        "level0ChannelNumPerRank_[%u], level1ChannelNumPerRank_[%u], level0DataRatio_[%.2f]",
        channelsPerRank_, level0ChannelNumPerRank_, level1ChannelNumPerRank_, level0DataRatio_);
    return HCCL_SUCCESS;
}

} // namespace mc2_ops_hccl
