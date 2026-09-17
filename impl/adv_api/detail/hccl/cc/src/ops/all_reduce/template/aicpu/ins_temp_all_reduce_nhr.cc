/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ins_temp_all_reduce_nhr.h"

namespace mc2_ops_hccl {

InsTempAllReduceNHR::InsTempAllReduceNHR(
    const OpParam& param, const u32 rankId, const std::vector<std::vector<u32>>& subCommRanks)
    : InsAlgTemplateBase(param, rankId, subCommRanks)
{}

InsTempAllReduceNHR::~InsTempAllReduceNHR() {}

u64 InsTempAllReduceNHR::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType)
{
    (void)inBuffType;
    (void)outBuffType;
    u32 multiple = 1;
    return multiple;
}

HcclResult InsTempAllReduceNHR::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    std::vector<HcclChannelDesc> level1Channels;
    const bool isUBX = topoInfo->level0Topo == Level0Shape::MESH_1D_CLOS && !topoInfo->level0PcieMix;
    if (isUBX) {
        std::vector<HcclChannelDesc> myChannelDescs;
        CHK_RET(CalcChannelRequestNhrMultiJetty(comm, param, topoInfo, subCommRanks_, myChannelDescs));
        for (const auto& channel : myChannelDescs) {
            if (channel.channelProtocol == COMM_PROTOCOL_UBC_CTP) {
                level1Channels.push_back(channel);
            }
        }
        HCCL_DEBUG("[InsTempAllReduceNHR::CalcRes] Get Channel Success!");
    } else {
        CHK_RET(CalcChannelRequestNhr(comm, param, topoInfo, subCommRanks_, level1Channels));
    }
    HCCL_DEBUG(" %s level1Channels.size() is %u ", __func__, level1Channels.size());
    resourceRequest.channels.push_back(level1Channels);
    channelsPerRank_ = CalcChannelsPerRank(level1Channels);
    if (isUBX && channelsPerRank_ > MAX_JETTY_NUM) {
        HCCL_ERROR(
            " %s channelsPerRank_ %u is greater than MAX_JETTY_NUM %u", __func__, channelsPerRank_, MAX_JETTY_NUM);
    } else {
        HCCL_DEBUG(" %s channelsPerRank_ is %u ", __func__, channelsPerRank_);
    }
    CHK_RET(GetRes(resourceRequest));

    HCCL_INFO("[InsTempAllReduceNHR] Calculate resource finished.");
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllReduceNHR::GetRes(AlgResourceRequest& resourceRequest) const
{
    u32 threadNum = channelsPerRank_;
    resourceRequest.slaveThreadNum = threadNum - 1;
    for (u32 index = 0; index < threadNum - 1; index++) {
        resourceRequest.notifyNumPerThread.push_back(1);
    }
    resourceRequest.notifyNumOnMainThread = threadNum - 1;
    return HCCL_SUCCESS;
}

u64 InsTempAllReduceNHR::GetThreadNum() const { return channelsPerRank_; }

void InsTempAllReduceNHR::GetNotifyIdxMainToSub(std::vector<u32>& notifyIdxMainToSub)
{
    notifyIdxMainToSub.clear();
    u32 slaveThreadNum = GetThreadNum() - 1;
    for (u32 i = 0; i < slaveThreadNum; i++) {
        notifyIdxMainToSub.push_back(0);
    }
}

void InsTempAllReduceNHR::GetNotifyIdxSubToMain(std::vector<u32>& notifyIdxSubToMain)
{
    notifyIdxSubToMain.clear();
    u32 notifyNum = GetThreadNum() - 1;
    for (u32 i = 0; i < notifyNum; i++) {
        notifyIdxSubToMain.push_back(i);
    }
}

HcclResult InsTempAllReduceNHR::PrepareDataSplitForMultiChannel(const TemplateResource& templateResource)
{
    CHK_PRT_RET(
        templateResource.channels.empty() || templateResource.channels.begin()->second.empty(),
        HCCL_ERROR("[InsTempAllReduceNHR][PrepareDataSplitForMultiChannel] channels is empty."), HCCL_E_INTERNAL);
    std::vector<u64> elemCountOut;
    u64 totalDataCount = sliceSize_ / dataTypeSize_;
    CHK_RET(CalcDataSplitByPortGroupCommon(
        totalDataCount, dataTypeSize_, templateResource.channels.begin()->second, elemCountOut, dataSplit_, dataOffset_,
        channelsPerRank_));
    if (tailSize_ != sliceSize_ && tailSize_ > 0) {
        std::vector<u64> elemCountOutTail;
        u64 totalDataCountTail = tailSize_ / dataTypeSize_;
        CHK_RET(CalcDataSplitByPortGroupCommon(
            totalDataCountTail, dataTypeSize_, templateResource.channels.begin()->second, elemCountOutTail,
            dataSplitTail_, dataOffsetTail_, channelsPerRank_));
    } else {
        dataSplitTail_ = dataSplit_;
        dataOffsetTail_ = dataOffset_;
    }
    return HCCL_SUCCESS;
}

HcclResult InsTempAllReduceNHR::KernelRun(
    const OpParam& param, const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource)
{
    HCCL_INFO("[InsTempAllReduceNHR] KernelRun Start.");

    CHK_PRT_RET(
        subCommRanks_.empty(), HCCL_ERROR("[InsTempAllReduceNHR][KernelRun] subCommRanks is empty."),
        HcclResult::HCCL_E_INTERNAL);
    rankList_ = subCommRanks_.at(0);

    CHK_PRT_RET(
        rankList_.size() != templateRankSize_,
        HCCL_ERROR("[InsTempAllReduceNHR][KernelRun] rank count is invalid in rank list."),
        HcclResult::HCCL_E_INTERNAL);

    CHK_RET(GetAlgRank(myRank_, rankList_, myRankIdx_));

    processSize_ = tempAlgParams.sliceSize;
    count_ = tempAlgParams.count;
    dataType_ = param.DataDes.dataType;
    dataTypeSize_ = DATATYPE_SIZE_TABLE[dataType_];
    tempAlgParams_ = tempAlgParams;
    isDmaRead_ = IsPcieProtocol(templateResource.channels);
    HCCL_DEBUG("[InsTempAllReduceNHR] Use Dma Read[%d]", isDmaRead_);

    if (count_ == 0) {
        HCCL_WARNING("[InsTempAllReduceNHR][KernelRun] data count is 0.");
        return HcclResult::HCCL_SUCCESS;
    }

    // 数据切片：向下取整，前N-1个rank各拿sliceSize_，最后rank拿tailSize_
    sliceSize_ = (count_ / templateRankSize_) * dataTypeSize_;
    tailSize_ = count_ * dataTypeSize_ - sliceSize_ * (templateRankSize_ - 1);

    CHK_RET(PrepareDataSplitForMultiChannel(templateResource));
    threadNum_ = GetThreadNum();
    CHK_PRT_RET(
        threadNum_ > templateResource.threads.size(),
        HCCL_ERROR(
            "[InsTempAllReduceNHR][KernelRun] thread num[%u] more than threads size[%zu].", threadNum_,
            templateResource.threads.size()),
        HcclResult::HCCL_E_INTERNAL);

    CHK_RET(PreCopy(tempAlgParams, templateResource.threads));
    if (threadNum_ > 1) {
        std::vector<ThreadHandle> subThreads(
            templateResource.threads.begin() + 1, templateResource.threads.begin() + threadNum_);
        GetNotifyIdxMainToSub(notifyIdxMainToSub_);
        CHK_RET(PreSyncInterThreads(templateResource.threads[0], subThreads, notifyIdxMainToSub_));
    }
    for (u32 channelIdx = 0; channelIdx < channelsPerRank_; channelIdx++) {
        // TwoShot算法，第一步ReduceScatter
        CHK_RET(RunReduceScatter(tempAlgParams, templateResource.channels, templateResource.threads, channelIdx));
    }
    for (u32 channelIdx = 0; channelIdx < channelsPerRank_; channelIdx++) {
        // TwoShot算法，第二步AllGather
        CHK_RET(RunAllGather(tempAlgParams, templateResource.channels, templateResource.threads, channelIdx));
    }
    if (threadNum_ > 1) {
        std::vector<ThreadHandle> subThreads(
            templateResource.threads.begin() + 1, templateResource.threads.begin() + threadNum_);
        GetNotifyIdxSubToMain(notifyIdxSubToMain_);
        CHK_RET(PostSyncInterThreads(templateResource.threads[0], subThreads, notifyIdxSubToMain_));
    }
    // 将数据从hcclBuffer上拷贝到output上
    CHK_RET(PostCopy(tempAlgParams, templateResource.threads));

    HCCL_INFO("[InsTempAllReduceNHR] KernelRun finished.");
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllReduceNHR::PreCopy(
    const TemplateDataParams& tempAlgParams, const std::vector<ThreadHandle>& threads) const
{
    HCCL_INFO("[InsTempAllReduceNHR] PreCopy data from input to hccl buffer");

    void* localInBuffPtr = tempAlgParams.buffInfo.inputPtr;
    void* localHcclBuffPtr = tempAlgParams.buffInfo.hcclBuff.addr;
    u64 inBuffBaseOffset = tempAlgParams.buffInfo.inBuffBaseOff;
    u64 hcclBuffBaseOffset = tempAlgParams.buffInfo.hcclBuffBaseOff;

    DataSlice copySrcSlice(localInBuffPtr, inBuffBaseOffset, processSize_, count_);
    DataSlice copyDstSlice(localHcclBuffPtr, hcclBuffBaseOffset, processSize_, count_);
    CHK_RET(LocalCopy(threads.at(0), copySrcSlice, copyDstSlice));
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllReduceNHR::RunReduceScatter(
    const TemplateDataParams& tempAlgParams, const std::map<u32, std::vector<ChannelInfo>>& channels,
    const std::vector<ThreadHandle>& threads, u32 channelIdx)
{
    void* localHcclBuffPtr = tempAlgParams.buffInfo.hcclBuff.addr;
    u64 hcclBuffBaseOffset = tempAlgParams.buffInfo.hcclBuffBaseOff;

    std::vector<NHRStepInfo> stepInfoList;
    CHK_RET(GetReduceScatterStepInfoList(stepInfoList));

    for (auto& stepInfo : stepInfoList) {
        CHK_PRT_RET(
            channels.count(rankList_.at(stepInfo.fromRank)) == 0,
            HCCL_ERROR(
                "[InsTempAllReduceNHR][RunReduceScatter] remoteRank[%u] is not in channels.",
                rankList_.at(stepInfo.fromRank)),
            HcclResult::HCCL_E_INTERNAL);
        CHK_PRT_RET(
            channels.count(rankList_.at(stepInfo.toRank)) == 0,
            HCCL_ERROR(
                "[InsTempAllReduceNHR][RunReduceScatter] remoteRank[%u] is not in channels.",
                rankList_.at(stepInfo.toRank)),
            HcclResult::HCCL_E_INTERNAL);

        const ChannelInfo& recvChannel = channels.at(rankList_.at(stepInfo.fromRank)).at(channelIdx);
        const ChannelInfo& sendChannel = channels.at(rankList_.at(stepInfo.toRank)).at(channelIdx);
        std::vector<DataSlice> recvSrcSlicesList;
        std::vector<DataSlice> sendSrcSlicesList;
        std::vector<DataSlice> recvDstSlicesList;
        std::vector<DataSlice> sendDstSlicesList;

        void* sendRemoteHcclBuffPtr = sendChannel.remoteCclMem.addr;
        void* recvRemoteHcclBuffPtr = recvChannel.remoteCclMem.addr;

        // 在 nhrInBuffType_ 上进行 ReduceScatter 操作
        for (u32 idx = 0; idx < stepInfo.nSlices; ++idx) {
            u32 txIdx = stepInfo.txSliceIdxs.at(idx);
            u32 rxIdx = stepInfo.rxSliceIdxs.at(idx);
            u64 txOff = (txIdx == templateRankSize_ - 1) ? dataOffsetTail_[channelIdx] : dataOffset_[channelIdx];
            u64 txSz = (txIdx == templateRankSize_ - 1) ? dataSplitTail_[channelIdx] : dataSplit_[channelIdx];
            u64 rxOff = (rxIdx == templateRankSize_ - 1) ? dataOffsetTail_[channelIdx] : dataOffset_[channelIdx];
            u64 rxSz = (rxIdx == templateRankSize_ - 1) ? dataSplitTail_[channelIdx] : dataSplit_[channelIdx];
            sendSrcSlicesList.emplace_back(
                localHcclBuffPtr, hcclBuffBaseOffset + txIdx * sliceSize_ + txOff, txSz, txSz / dataTypeSize_);
            sendDstSlicesList.emplace_back(
                sendRemoteHcclBuffPtr, hcclBuffBaseOffset + txIdx * sliceSize_ + txOff, txSz, txSz / dataTypeSize_);
            recvSrcSlicesList.emplace_back(
                recvRemoteHcclBuffPtr, hcclBuffBaseOffset + rxIdx * sliceSize_ + rxOff, rxSz, rxSz / dataTypeSize_);
            recvDstSlicesList.emplace_back(
                localHcclBuffPtr, hcclBuffBaseOffset + rxIdx * sliceSize_ + rxOff, rxSz, rxSz / dataTypeSize_);
        }
        SendRecvReduceInfo sendRecvReduceInfo{
            {sendChannel, recvChannel},
            {{sendSrcSlicesList, sendDstSlicesList}, {recvSrcSlicesList, recvDstSlicesList}},
            dataType_,
            reduceOp_};

        if (isDmaRead_) {
            CHK_PRT_RET(
                SendRecvReadReduce(sendRecvReduceInfo, threads.at(channelIdx)),
                HCCL_ERROR("[InsTempAllReduceNHR] RunReduceScatter SendRecvReduce failed"),
                HcclResult::HCCL_E_INTERNAL);
        } else {
            CHK_PRT_RET(
                SendRecvBatchWriteReduce(sendRecvReduceInfo, threads.at(channelIdx), true),
                HCCL_ERROR("[InsTempAllReduceNHR] RunReduceScatter SendRecvReduce failed"),
                HcclResult::HCCL_E_INTERNAL);
        }
    }

    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllReduceNHR::RunAllGather(
    const TemplateDataParams& tempAlgParams, const std::map<u32, std::vector<ChannelInfo>>& channels,
    const std::vector<ThreadHandle>& threads, u32 channelIdx)
{
    void* localHcclBuffPtr = tempAlgParams.buffInfo.hcclBuff.addr;
    u64 hcclBuffBaseOffset = tempAlgParams.buffInfo.hcclBuffBaseOff;

    std::vector<NHRStepInfo> stepInfoList;
    CHK_RET(GetAllGatherStepInfoList(stepInfoList));

    for (auto& stepInfo : stepInfoList) {
        CHK_PRT_RET(
            channels.count(rankList_.at(stepInfo.fromRank)) == 0,
            HCCL_ERROR(
                "[InsTempAllReduceNHR][RunAllGather] remoteRank[%u] is not in channels.",
                rankList_.at(stepInfo.fromRank)),
            HcclResult::HCCL_E_INTERNAL);
        CHK_PRT_RET(
            channels.count(rankList_.at(stepInfo.toRank)) == 0,
            HCCL_ERROR(
                "[InsTempAllReduceNHR][RunAllGather] remoteRank[%u] is not in channels.",
                rankList_.at(stepInfo.toRank)),
            HcclResult::HCCL_E_INTERNAL);

        const ChannelInfo& recvChannel = channels.at(rankList_.at(stepInfo.fromRank)).at(channelIdx);
        const ChannelInfo& sendChannel = channels.at(rankList_.at(stepInfo.toRank)).at(channelIdx);
        std::vector<DataSlice> sendSrcSlicesList;
        std::vector<DataSlice> sendDstSlicesList;
        std::vector<DataSlice> recvSrcSlicesList;
        std::vector<DataSlice> recvDstSlicesList;

        void* sendRemoteHcclBuffPtr = sendChannel.remoteCclMem.addr;
        void* recvRemoteHcclBuffPtr = recvChannel.remoteCclMem.addr;

        for (u32 idx = 0; idx < stepInfo.nSlices; ++idx) {
            u32 txIdx = stepInfo.txSliceIdxs.at(idx);
            u32 rxIdx = stepInfo.rxSliceIdxs.at(idx);
            u64 txOff = (txIdx == templateRankSize_ - 1) ? dataOffsetTail_[channelIdx] : dataOffset_[channelIdx];
            u64 txSz = (txIdx == templateRankSize_ - 1) ? dataSplitTail_[channelIdx] : dataSplit_[channelIdx];
            u64 rxOff = (rxIdx == templateRankSize_ - 1) ? dataOffsetTail_[channelIdx] : dataOffset_[channelIdx];
            u64 rxSz = (rxIdx == templateRankSize_ - 1) ? dataSplitTail_[channelIdx] : dataSplit_[channelIdx];
            sendSrcSlicesList.emplace_back(
                localHcclBuffPtr, hcclBuffBaseOffset + txIdx * sliceSize_ + txOff, txSz, txSz / dataTypeSize_);
            sendDstSlicesList.emplace_back(
                sendRemoteHcclBuffPtr, hcclBuffBaseOffset + txIdx * sliceSize_ + txOff, txSz, txSz / dataTypeSize_);
            recvSrcSlicesList.emplace_back(
                recvRemoteHcclBuffPtr, hcclBuffBaseOffset + rxIdx * sliceSize_ + rxOff, rxSz, rxSz / dataTypeSize_);
            recvDstSlicesList.emplace_back(
                localHcclBuffPtr, hcclBuffBaseOffset + rxIdx * sliceSize_ + rxOff, rxSz, rxSz / dataTypeSize_);
        }
        SendRecvInfo sendRecvInfo{
            {sendChannel, recvChannel},
            {{sendSrcSlicesList, sendDstSlicesList}, {recvSrcSlicesList, recvDstSlicesList}}};

        if (isDmaRead_) {
            CHK_PRT_RET(
                SendRecvRead(sendRecvInfo, threads.at(channelIdx)),
                HCCL_ERROR("[InsTempAllReduceNHR] RunAllGather SendRecv failed"), HcclResult::HCCL_E_INTERNAL);
        } else {
            CHK_PRT_RET(
                SendRecvBatchWrite(sendRecvInfo, threads.at(channelIdx)),
                HCCL_ERROR("[InsTempAllReduceNHR] RunAllGather SendRecv failed"), HcclResult::HCCL_E_INTERNAL);
        }
    }

    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllReduceNHR::PostCopy(
    const TemplateDataParams& tempAlgParams, const std::vector<ThreadHandle>& threads) const
{
    HCCL_INFO("[InsTempAllReduceNHR][PostCopy] Opbase copy from scratchBuffer to userOut");

    void* localHcclBuffPtr = tempAlgParams.buffInfo.hcclBuff.addr;
    void* localOutBuffPtr = tempAlgParams.buffInfo.outputPtr;
    u64 hcclBuffBaseOffset = tempAlgParams.buffInfo.hcclBuffBaseOff;
    u64 outBuffBaseOffset = tempAlgParams.buffInfo.outBuffBaseOff;

    DataSlice copySrcSlice(localHcclBuffPtr, hcclBuffBaseOffset, tempAlgParams.sliceSize, tempAlgParams.count);
    DataSlice copyDstSlice(localOutBuffPtr, outBuffBaseOffset, tempAlgParams.sliceSize, tempAlgParams.count);
    CHK_RET(LocalCopy(threads.at(0), copySrcSlice, copyDstSlice));
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllReduceNHR::GetReduceScatterStepInfoList(std::vector<NHRStepInfo>& stepInfoList) const
{
    stepInfoList.clear();
    u32 nSteps = GetNHRStepNum();
    stepInfoList.resize(nSteps);

    for (u32 step = 0; step < nSteps; step++) {
        // 计算通信对象
        u32 deltaRank = 1 << step;
        u32 sendToIdx = (myRankIdx_ + templateRankSize_ - deltaRank) % templateRankSize_;
        u32 recvFromIdx = (myRankIdx_ + deltaRank) % templateRankSize_;

        // 数据份数和数据编号增量
        u32 nSlices = (templateRankSize_ - 1 + (1 << step)) / (1 << (step + 1));
        u32 deltaSliceIndex = 1 << (step + 1);
        u32 txSliceIdx = sendToIdx;
        u32 rxSliceIdx = myRankIdx_;

        NHRStepInfo& currStepInfo = stepInfoList[step];
        currStepInfo.step = step;
        currStepInfo.myRank = myRankIdx_;
        currStepInfo.nSlices = nSlices;
        currStepInfo.toRank = sendToIdx;
        currStepInfo.fromRank = recvFromIdx;

        // 计算本rank在每轮收/发中的slice编号
        currStepInfo.txSliceIdxs.reserve(nSlices);
        currStepInfo.rxSliceIdxs.reserve(nSlices);
        for (u32 i = 0; i < nSlices; i++) {
            currStepInfo.txSliceIdxs.push_back(txSliceIdx);
            currStepInfo.rxSliceIdxs.push_back(rxSliceIdx);
            HCCL_DEBUG(
                "[InsTempAllReduceNHR][GetStepInfoList] i[%u] txSliceIdx[%u] rxSliceIdx[%u]", i, txSliceIdx,
                rxSliceIdx);
            txSliceIdx = (txSliceIdx + templateRankSize_ - deltaSliceIndex) % templateRankSize_;
            rxSliceIdx = (rxSliceIdx + templateRankSize_ - deltaSliceIndex) % templateRankSize_;
        }
    }
    return HcclResult::HCCL_SUCCESS;
}

// 计算AllGather每轮收发的对端以及slice编号
HcclResult InsTempAllReduceNHR::GetAllGatherStepInfoList(std::vector<NHRStepInfo>& stepInfoList) const
{
    stepInfoList.clear();

    u32 nSteps = GetNHRStepNum();
    stepInfoList.resize(nSteps);
    for (u32 step = 0; step < nSteps; step++) {
        // 计算通信对象
        u32 deltaRank = 1 << (nSteps - 1 - step);
        u32 sendToIdx = (myRankIdx_ + deltaRank) % templateRankSize_;
        u32 recvFromIdx = (myRankIdx_ + templateRankSize_ - deltaRank) % templateRankSize_;

        // 数据份数和数据编号增量
        u32 nSlices = (templateRankSize_ - 1 + (1 << (nSteps - 1 - step))) / (1 << (nSteps - step));
        u32 deltaSliceIndex = 1 << (nSteps - step);
        u32 txSliceIdx = myRankIdx_;
        u32 rxSliceIdx = (myRankIdx_ - (1 << (nSteps - 1 - step)) + templateRankSize_) % templateRankSize_;

        NHRStepInfo& currStepInfo = stepInfoList[step];
        currStepInfo.step = step;
        currStepInfo.toRank = sendToIdx;
        currStepInfo.fromRank = recvFromIdx;
        currStepInfo.myRank = myRankIdx_;
        currStepInfo.nSlices = nSlices;

        // 计算本rank在每轮收/发中的slice编号
        currStepInfo.txSliceIdxs.reserve(nSlices);
        currStepInfo.rxSliceIdxs.reserve(nSlices);
        for (u32 i = 0; i < nSlices; i++) {
            currStepInfo.txSliceIdxs.push_back(txSliceIdx);
            currStepInfo.rxSliceIdxs.push_back(rxSliceIdx);
            HCCL_DEBUG(
                "[InsTempAllReduceNHR][GetStepInfoList] i[%u] txSliceIdx[%u] rxSliceIdx[%u]", i, txSliceIdx,
                rxSliceIdx);
            txSliceIdx = (txSliceIdx + templateRankSize_ - deltaSliceIndex) % templateRankSize_;
            rxSliceIdx = (rxSliceIdx + templateRankSize_ - deltaSliceIndex) % templateRankSize_;
        }
    }
    return HcclResult::HCCL_SUCCESS;
}

u32 InsTempAllReduceNHR::GetNHRStepNum() const
{
    u32 nSteps = 0;
    for (u32 tmp = templateRankSize_ - 1; tmp != 0; tmp >>= 1, nSteps++) {
    }
    HCCL_DEBUG("[InsTempAllReduceNHR][GetNHRStepNum] rankSize[%u] nSteps[%u]", templateRankSize_, nSteps);
    return nSteps;
}

} // namespace mc2_ops_hccl
