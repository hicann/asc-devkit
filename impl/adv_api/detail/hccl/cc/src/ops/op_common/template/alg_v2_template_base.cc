/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "alg_v2_template_base.h"

namespace mc2_ops_hccl {

InsAlgTemplateBase::InsAlgTemplateBase(
    const OpParam& param, const u32 rankId, // 传通信域的rankId，userRank
    const std::vector<std::vector<u32>>& subCommRanks)
    : opMode_(param.opMode),
      root_(param.root),
      myRank_(rankId),
      subCommRanks_(subCommRanks),
      reduceOp_(param.reduceType),
      enableDetour_(param.enableDetour)
{
    if (subCommRanks.empty()) {
        HCCL_ERROR("[InsAlgTemplateBase] subCommRanks is empty, myRank[%u].", myRank_);
        templateRankSize_ = 0;
    } else if (subCommRanks.size() > 1) {
        templateRankSize_ = subCommRanks[0].size() * subCommRanks[1].size();
    } else {
        templateRankSize_ = subCommRanks[0].size();
    }
}

InsAlgTemplateBase::~InsAlgTemplateBase() {}

HcclResult InsAlgTemplateBase::FastLaunch(const OpParam& param, const TemplateFastLaunchCtx& tempFastLaunchCtx)
{
    (void)param;
    (void)tempFastLaunchCtx;
    HCCL_ERROR("[InsAlgTemplateBase] Unsupported interface of InsAlgTemplateBase::FastLaunch!");
    return HcclResult::HCCL_E_INTERNAL;
}

HcclResult InsAlgTemplateBase::KernelRun(
    const OpParam& param, const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource)
{
    (void)param;
    (void)tempAlgParams;
    (void)templateResource;
    HCCL_ERROR("[InsAlgTemplateBase] Unsupported interface of kernel run!");
    return HcclResult::HCCL_E_INTERNAL;
}

HcclResult InsAlgTemplateBase::DPUKernelRun(
    const TemplateDataParams& tempAlgParam, const std::map<u32, std::vector<ChannelInfo>>& channels, const u32 myRank,
    const std::vector<std::vector<uint32_t>>& subCommRanks)
{
    (void)tempAlgParam;
    (void)channels;
    (void)myRank;
    (void)subCommRanks;
    HCCL_ERROR("[InsAlgTemplateBase] Unsupported interface of dpu kernel run!");
    return HcclResult::HCCL_E_INTERNAL;
}

HcclResult InsAlgTemplateBase::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    (void)comm;
    (void)param;
    (void)topoInfo;
    (void)resourceRequest;
    HCCL_ERROR("[InsAlgTemplateBase] Unsupported interface of resource calculation!");
    return HcclResult::HCCL_E_INTERNAL;
}

HcclResult InsAlgTemplateBase::GetRes(AlgResourceRequest& resourceRequest) const
{
    (void)resourceRequest;
    HCCL_ERROR("[InsAlgTemplateBase] Unsupported interface of resource calculation!");
    return HcclResult::HCCL_E_INTERNAL;
}

u64 InsAlgTemplateBase::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType) { return 0; }

u64 InsAlgTemplateBase::GetThreadNum() const { return 0; }

bool InsAlgTemplateBase::IsPcieProtocol(const std::map<u32, std::vector<ChannelInfo>>& channels)
{
    for (auto it = channels.begin(); it != channels.end(); it++) {
        if ((it->second).at(0).protocol == CommProtocol::COMM_PROTOCOL_PCIE) {
            HCCL_DEBUG("[IsPcieProtocol] the protocol of channel is PCIE");
            return true;
        }
    }
    HCCL_DEBUG("[IsPcieProtocol] the protocol of channel is Non-PCIE");
    return false;
}

HcclResult InsAlgTemplateBase::CalcDataSplitByPortGroup(
    const u64 totalDataCount, const u64 dataTypeSize, const std::vector<ChannelInfo>& channels,
    std::vector<u64>& elemCountOut, std::vector<u64>& sizeOut, std::vector<u64>& elemOffset)
{
    CalcDataSplitByPortGroupCommon(
        totalDataCount, dataTypeSize, channels, elemCountOut, sizeOut, elemOffset, channelsPerRank_);
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsAlgTemplateBase::SetchannelsPerRank(const std::map<u32, std::vector<ChannelInfo>>& channels)
{
    CHK_PRT_RET(channels.empty(), HCCL_ERROR("[SetchannelsPerRank] channels is empty."), HcclResult::HCCL_E_INTERNAL);
    channelsPerRank_ = CalcChannelsPerRank(channels);
    return HcclResult::HCCL_SUCCESS;
}

} // namespace mc2_ops_hccl
