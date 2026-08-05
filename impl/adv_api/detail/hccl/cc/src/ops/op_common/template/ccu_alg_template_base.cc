/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "ccu_alg_template_base.h"
#include "log.h"
#include "ccu_assist_pub.h"

namespace mc2_ops_hccl {
CcuAlgTemplateBase::CcuAlgTemplateBase() {}

CcuAlgTemplateBase::CcuAlgTemplateBase(
    const OpParam& param, const u32 rankId, // 传通信域的rankId，userRank
    const std::vector<std::vector<u32>>& subCommRanks)
    : myRank_(rankId), subCommRanks_(subCommRanks), opMode_(param.opMode), root_(param.root)
{
    for (const auto& ranks : subCommRanks) {
        templateRankSize_ += ranks.size();
    }
}

CcuAlgTemplateBase::~CcuAlgTemplateBase() {}

HcclResult CcuAlgTemplateBase::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    (void)comm;
    (void)param;
    (void)topoInfo;
    (void)resourceRequest;
    HCCL_ERROR("[CcuAlgTemplateBase] Unsupported interface of CcuAlgTemplateBase::CalcRes!");
    return HcclResult::HCCL_E_INTERNAL;
}

HcclResult CcuAlgTemplateBase::KernelRun(
    const OpParam& param, const TemplateDataParams& templateDataParams, TemplateResource& templateResource)
{
    (void)param;
    (void)templateDataParams;
    (void)templateResource;
    HCCL_ERROR("[CcuAlgTemplateBase] Unsupported interface of CcuAlgTemplateBase::KernelRun!");
    return HcclResult::HCCL_E_INTERNAL;
}

HcclResult CcuAlgTemplateBase::FastLaunch(const OpParam& param, const TemplateFastLaunchCtx& tempFastLaunchCtx)
{
    (void)param;
    (void)tempFastLaunchCtx;
    HCCL_ERROR("[CcuAlgTemplateBase] Unsupported interface of CcuAlgTemplateBase::FastLaunch!");
    return HcclResult::HCCL_E_INTERNAL;
}

u64 CcuAlgTemplateBase::GetThreadNum() const { return 0; }

HcclResult CcuAlgTemplateBase::GetRes(AlgResourceRequest& resourceRequest) const
{
    (void)resourceRequest;
    HCCL_ERROR("[CcuAlgTemplateBase] Unsupported interface of resource calculation!");
    return HcclResult::HCCL_E_INTERNAL;
}

u64 CcuAlgTemplateBase::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType)
{
    (void)inBuffType;
    (void)outBuffType;
    return 0;
}

uint64_t CcuAlgTemplateBase::PointerToAddr(void* pointer) const
{
    if (pointer != nullptr) {
        return reinterpret_cast<uint64_t>(pointer);
    } else {
        return 0;
    }
}

HcclResult CcuAlgTemplateBase::RestoreChannelMap(
    const std::vector<HcclChannelDesc>& channelDescs,
    std::map<u32, std::vector<HcclChannelDesc>>& rankIdToChannelDesc) const
{
    for (auto& channel : channelDescs) {
        u32 remoteRank = channel.remoteRank;
        rankIdToChannelDesc[remoteRank].push_back(channel);
    }
    return HCCL_SUCCESS;
}

HcclResult CcuAlgTemplateBase::GetChannelDieId(
    HcclComm comm, uint32_t rankId, const HcclChannelDesc& channelDesc, uint32_t& dieId) const
{
    EndpointAttrDieId tmpDieId{};
    uint32_t infoLen = sizeof(EndpointAttrDieId);
    CHK_RET(HcclRankGraphGetEndpointInfo(
        comm, rankId, &(channelDesc.localEndpoint), ENDPOINT_ATTR_DIE_ID, infoLen, &tmpDieId));
    dieId = tmpDieId;
    HCCL_DEBUG("[CcuAlgTemplateBase::GetChannelDieId] rank[%d]: get channel die id [%d]", rankId, dieId);
    return HcclResult::HCCL_SUCCESS;
}

/* nhr算法，需要遍历得到的channelDesc，判断使用几个die，如果是1个die，则还需要得到dieId。
   以便于算法挑选相应dieId的channelDesc */
HcclResult CcuAlgTemplateBase::GetDieInfoFromChannelDescs(
    HcclComm comm, const std::map<u32, std::vector<HcclChannelDesc>>& rankIdToChannelDesc, u32 myRankId,
    uint32_t& dieNum, uint32_t& dieId)
{
    constexpr u32 LINK_NUM_1 = 1;
    constexpr u32 LINK_NUM_2 = 2;
    // 遍历每个对端有几个channel
    for (const auto& pair : rankIdToChannelDesc) {
        u32 rmtRankId = pair.first;
        const std::vector<HcclChannelDesc>& channels = pair.second;
        if (channels.size() == LINK_NUM_1) {
            dieNum = 1;
            GetChannelDieId(comm, myRankId, channels[0], dieId);
            HCCL_INFO("[CcuAlgTemplateBase::GetDieNumFromChannelDescs] only 1 channel, dieNum = 1, dieId = %u.", dieId);
            return HcclResult::HCCL_SUCCESS;
        }

        if (channels.size() == LINK_NUM_2) {
            // 检查2个channel是否在2个die上
            uint32_t dieId0 = 0;
            uint32_t dieId1 = 0;
            GetChannelDieId(comm, myRankId, channels[0], dieId0);
            GetChannelDieId(comm, myRankId, channels[1], dieId1);
            if (dieId0 == dieId1) {
                dieNum = LINK_NUM_1;
                dieId = dieId0;
                HCCL_INFO(
                    "[CcuAlgTemplateBase::GetDieNumFromChannelDescs] 2 channels on the same die, dieNum = 1, dieId = "
                    "%u.",
                    dieId);
                return HcclResult::HCCL_SUCCESS;
            }
        } else {
            HCCL_ERROR(
                "[CcuAlgTemplateBase::GetDieNumFromChannelDescs] get channelDescs fail: there are [%u] link to rank "
                "[%u]",
                channels.size(), rmtRankId);
            return HcclResult::HCCL_E_INTERNAL;
        }
    }
    // 为适配不对称场景，当前nhr仅使能1个die
    dieNum = LINK_NUM_1;
    HCCL_INFO("[CcuAlgTemplateBase::GetDieNumFromChannelDescs] 2 channels on 2 dies, dieNum = 2.");
    return HcclResult::HCCL_SUCCESS;
}

/* 从rankIdToChannelDesc的map中，挑选出指定die上的channel加入到vec中，并将Index放入rank2ChannelIdx*/
HcclResult CcuAlgTemplateBase::SelectChannelToVec(
    const HcclComm comm, const u32 myRankId, const u32 rmtRankId,
    const std::map<u32, std::vector<HcclChannelDesc>>& rankIdToChannelDesc, const u32 dieId,
    std::map<u32, u32>& rank2ChannelIdx, std::vector<HcclChannelDesc>& channels)
{
    auto it = rank2ChannelIdx.find(rmtRankId);
    if (it != rank2ChannelIdx.end()) {
        // 已经有对应channel，直接返回成功
        return HcclResult::HCCL_SUCCESS;
    }

    auto vecIt = rankIdToChannelDesc.find(rmtRankId);
    if (vecIt == rankIdToChannelDesc.end()) {
        // 不存在到对端rank的channel，报错
        HCCL_ERROR(
            "[CcuAlgTemplateBase::SelectChannelToVec] there's no channel from rank[%u] to rank[%u]", myRankId,
            rmtRankId);
        return HcclResult::HCCL_E_INTERNAL;
    }

    u32 curChannelIdx = channels.size();
    bool isFound = false;
    for (const HcclChannelDesc& channel : rankIdToChannelDesc.at(rmtRankId)) {
        uint32_t tmpDieId = 0;
        CHK_RET(GetChannelDieId(comm, myRankId, channel, tmpDieId));
        if (tmpDieId == dieId) {
            channels.push_back(channel);
            rank2ChannelIdx[rmtRankId] = curChannelIdx;
            isFound = true;
            break;
        }
    }
    if (!isFound) {
        HCCL_ERROR(
            "[CcuAlgTemplateBase::SelectChannelToVec] there's no channel from rank[%u] to rank[%u] on die[%u]",
            myRankId, rmtRankId, dieId);
        return HcclResult::HCCL_E_INTERNAL;
    }
    HCCL_INFO(
        "[CcuAlgTemplateBase::SelectChannelToVec] find channel rank[%u] to rank[%u] on die[%u] succeed."
        "Index=[%u]",
        myRankId, rmtRankId, dieId, curChannelIdx);
    return HcclResult::HCCL_SUCCESS;
}

HcclResult CcuAlgTemplateBase::GetToken(const BuffInfo& buffinfo, uint64_t& token) const
{
    if (buffinfo.inputPtr != nullptr) {
        token =
            hcomm::CcuRep::GetTokenInfo(PointerToAddr(buffinfo.inputPtr), static_cast<uint64_t>(buffinfo.inputSize));
        return HCCL_SUCCESS;
    } else if (buffinfo.outputPtr != nullptr) {
        token =
            hcomm::CcuRep::GetTokenInfo(PointerToAddr(buffinfo.outputPtr), static_cast<uint64_t>(buffinfo.outputSize));
        return HCCL_SUCCESS;
    } else if (buffinfo.hcclBuff.addr != nullptr) {
        token = hcomm::CcuRep::GetTokenInfo(
            PointerToAddr(buffinfo.hcclBuff.addr), static_cast<uint64_t>(buffinfo.hcclBuff.size));
        return HCCL_SUCCESS;
    }
    HCCL_WARNING("[GetToken] inputMem, outputMem and hcclBuff are all null");
    return HCCL_E_PTR;
}

} // namespace mc2_ops_hccl
