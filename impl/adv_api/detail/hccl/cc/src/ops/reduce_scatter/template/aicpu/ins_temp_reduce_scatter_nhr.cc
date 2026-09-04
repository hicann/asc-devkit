/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "ins_temp_reduce_scatter_nhr.h"

namespace mc2_ops_hccl {
InsTempReduceScatterNHR::InsTempReduceScatterNHR(
    const OpParam& param, const u32 rankId, // 传通信域的u32，userRank
    const std::vector<std::vector<u32>>& subCommRanks)
    : InsAlgTemplateBase(param, rankId, subCommRanks)
{}

InsTempReduceScatterNHR::~InsTempReduceScatterNHR() {}

HcclResult InsTempReduceScatterNHR::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    std::vector<HcclChannelDesc> channels;
    std::vector<HcclChannelDesc> myChannelDescs;
    u64 perDataSize = DATATYPE_SIZE_TABLE[param.DataDes.dataType];
    u64 dataSize = param.DataDes.count * perDataSize;
    if (topoInfo->level0Topo == Level0Shape::MESH_1D_CLOS && !topoInfo->level0PcieMix) {
        bool isIsolation =
            !(IsAllConnetedWithTopo(topoInfo, 0, CommTopo::COMM_TOPO_1DMESH) || dataSize <= SMALL_SIZE_512KB);
        CHK_RET(CalcChannelRequestNhrMultiJetty(comm, param, topoInfo, subCommRanks_, myChannelDescs, isIsolation));
        for (auto channel : myChannelDescs) {
            if (channel.channelProtocol == COMM_PROTOCOL_UBC_CTP) {
                channels.push_back(channel);
            }
        }
    } else {
        CHK_RET(CalcChannelRequestNhr(comm, param, topoInfo, subCommRanks_, myChannelDescs));
        channels = myChannelDescs;
    }
    resourceRequest.channels.push_back(channels);
    u32 channelsPerRank = CalcChannelsPerRank(channels);
    HCCL_INFO("[InsTempReduceScatterNHR][CalcRes] channelsPerRank: [%u].", channelsPerRank);
    channelsPerRank_ = channelsPerRank;
    if (channelsPerRank_ > MAX_JETTY_NUM) {
        HCCL_ERROR(
            " %s channelsPerRank_ %u is greater than MAX_JETTY_NUM %u", __func__, channelsPerRank_, MAX_JETTY_NUM);
    } else {
        HCCL_DEBUG(" %s channelsPerRank_ is %u ", __func__, channelsPerRank_);
    }
    GetRes(resourceRequest);
    HCCL_INFO(
        "[InsTempReduceScatterNHR][CalcRes] slaveThreadNum: [%u], notifyNumOnMainThread: [%u].",
        resourceRequest.slaveThreadNum, resourceRequest.notifyNumOnMainThread);
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempReduceScatterNHR::GetRes(AlgResourceRequest& resourceRequest) const
{
    u32 threadNum = 1 * channelsPerRank_;
    resourceRequest.slaveThreadNum = threadNum - 1;
    for (u32 index = 0; index < threadNum - 1; index++) {
        resourceRequest.notifyNumPerThread.push_back(1);
    }
    resourceRequest.notifyNumOnMainThread = threadNum - 1;
    HCCL_INFO(
        "[GetRes] channelsPerRank_: [%u], slaveThreadNum: [%u].", channelsPerRank_, resourceRequest.slaveThreadNum);
    return HCCL_SUCCESS;
}

u64 InsTempReduceScatterNHR::GetThreadNum() const { return 1 * channelsPerRank_; }

HcclResult InsTempReduceScatterNHR::KernelRun(
    const OpParam& param, const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource)
{
    HCCL_INFO("[InsTempReduceScatterNHR] GenExtIns start");
    if (tempAlgParams.sliceSize == 0 && tempAlgParams.tailSize == 0) {
        HCCL_INFO("[InsTempReduceScatterNHR] sliceSize and tailSize are both 0, skip");
        return HCCL_SUCCESS;
    }
    tempAlgParams_ = tempAlgParams;
    channels_ = templateResource.channels;
    dataType_ = param.DataDes.dataType;
    dataTypeSize_ = DATATYPE_SIZE_TABLE[dataType_];

    bool isPcieProtocal = IsPcieProtocol(channels_); // 判断是否存在pcie链路
    isDmaRead_ = isPcieProtocal;                     // 是否使用Read模式
    doPreCopy_ = false;
    skipStep0TxPreCopy_ = false;
    step0TxSliceIdxs_.clear();
    HCCL_DEBUG("[InsTempReduceScatterNHR] Use Dma Read[%d]", isDmaRead_);

    if (!isDmaRead_ && templateRankSize_ > 1) {
        skipStep0TxPreCopy_ = true;
        HCCL_INFO("[InsTempReduceScatterNHR] Skip step0 tx pre-copy enabled");
    }

    std::vector<u64> elemCountOut;
    std::vector<u64> sizeOut;
    std::vector<u64> elemOffset;
    u64 totalDataCount = tempAlgParams.sliceSize / dataTypeSize_;
    CHK_RET(CalcDataSplitByPortGroup(
        totalDataCount, dataTypeSize_, channels_.begin()->second, elemCountOut, sizeOut, elemOffset));
    elemOffset_ = elemOffset;
    sizeOut_ = sizeOut;

    if (tempAlgParams.tailSize > 0) {
        std::vector<u64> elemCountOutTail;
        std::vector<u64> sizeOutTail;
        std::vector<u64> elemOffsetTail;
        u64 totalDataCountTail = tempAlgParams.tailSize / dataTypeSize_;
        CHK_RET(CalcDataSplitByPortGroup(
            totalDataCountTail, dataTypeSize_, channels_.begin()->second, elemCountOutTail, sizeOutTail,
            elemOffsetTail));
        elemOffsetTail_ = elemOffsetTail;
        sizeOutTail_ = sizeOutTail;
    }

    threadNum_ = GetThreadNum();
    if (threadNum_ > 1) {
        std::vector<ThreadHandle> subThreads(
            templateResource.threads.begin() + 1, templateResource.threads.begin() + threadNum_);
        GetNotifyIdxMainToSub(notifyIdxMainToSub_);
        CHK_RET(PreSyncInterThreads(templateResource.threads[0], subThreads, notifyIdxMainToSub_));
    }

    if (skipStep0TxPreCopy_) {
        std::vector<AicpuNHRStepInfo> tmpSteps;
        CHK_RET(GetStepInfoList(tmpSteps));
    }

    for (u32 channelIdx = 0; channelIdx < channelsPerRank_; channelIdx++) {
        CHK_PRT_RET(
            channelIdx >= sizeOut_.size() || channelIdx >= elemOffset_.size(),
            HCCL_ERROR("[InsTempReduceScatterNHR] channelIdx[%u] out of bounds", channelIdx), HCCL_E_INTERNAL);
        CHK_RET(LocalDataCopy(templateResource.threads, channelIdx));
    }
    if (templateRankSize_ <= 1) {
        for (u32 channelIdx = 0; channelIdx < channelsPerRank_; channelIdx++) {
            CHK_RET(PostLocalCopy(templateResource.threads, channelIdx));
        }
        if (threadNum_ > 1) {
            std::vector<ThreadHandle> subThreads(
                templateResource.threads.begin() + 1, templateResource.threads.begin() + threadNum_);
            GetNotifyIdxSubToMain(notifyIdxSubToMain_);
            CHK_RET(PostSyncInterThreads(templateResource.threads[0], subThreads, notifyIdxSubToMain_));
        }
        return HcclResult::HCCL_SUCCESS;
    }
    for (u32 channelIdx = 0; channelIdx < channelsPerRank_; channelIdx++) {
        CHK_RET(RunNHR(templateResource.threads, channelIdx));
    }
    for (u32 channelIdx = 0; channelIdx < channelsPerRank_; channelIdx++) {
        CHK_RET(PostLocalCopy(templateResource.threads, channelIdx));
    }
    if (threadNum_ > 1) {
        std::vector<ThreadHandle> subThreads(
            templateResource.threads.begin() + 1, templateResource.threads.begin() + threadNum_);
        GetNotifyIdxSubToMain(notifyIdxSubToMain_);
        CHK_RET(PostSyncInterThreads(templateResource.threads[0], subThreads, notifyIdxSubToMain_));
    }

    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempReduceScatterNHR::LocalDataCopy(const std::vector<ThreadHandle>& threads, u32 channelIdx)
{
    CHK_PRT_RET(
        threads.empty(), HCCL_ERROR("[InsTempReduceScatterNHR][LocalDataCopy] empty threads"),
        HcclResult::HCCL_E_INTERNAL);
    ThreadHandle q = threads[channelIdx];
    const u64 rptNum = std::max<u64>(1, tempAlgParams_.repeatNum);
    for (u32 localRandId = 0; localRandId < templateRankSize_; ++localRandId) {
        if (skipStep0TxPreCopy_ && step0TxSliceIdxs_.count(localRandId) > 0) {
            continue;
        }
        u64 sliceSize = tempAlgParams_.sliceSize;
        std::vector<u64> sizeOut;
        std::vector<u64> elemOffset;
        sizeOut = sizeOut_;
        elemOffset = elemOffset_;
        if (localRandId == templateRankSize_ - 1 && tempAlgParams_.tailSize > 0) {
            sliceSize = tempAlgParams_.tailSize;
            sizeOut = sizeOutTail_;
            elemOffset = elemOffsetTail_;
        }
        for (u64 rpt = 0; rpt < rptNum; ++rpt) {
            const u64 inBaseOff = tempAlgParams_.buffInfo.inBuffBaseOff + rpt * tempAlgParams_.inputRepeatStride;
            const u64 scratchBase = tempAlgParams_.buffInfo.hcclBuffBaseOff + rpt * tempAlgParams_.outputRepeatStride;

            const u64 inOff = inBaseOff + localRandId * tempAlgParams_.inputSliceStride + elemOffset[channelIdx];
            const u64 scOff = scratchBase + localRandId * tempAlgParams_.sliceSize + elemOffset[channelIdx];

            DataSlice src = DataSlice(tempAlgParams_.buffInfo.inputPtr, inOff, sizeOut[channelIdx]);
            DataSlice dst = DataSlice(tempAlgParams_.buffInfo.hcclBuff.addr, scOff, sizeOut[channelIdx]);

            if (tempAlgParams_.buffInfo.inBuffType != tempAlgParams_.buffInfo.hcclBuffType ||
                inBaseOff != scratchBase) {
                doPreCopy_ = true;
                CHK_RET(LocalCopy(q, src, dst));
            }
        }
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempReduceScatterNHR::PostLocalCopy(const std::vector<ThreadHandle>& threads, u32 channelIdx)
{
    CHK_PRT_RET(threads.empty(), HCCL_ERROR("[RS-NHR][PostLocalCopy] empty queue"), HcclResult::HCCL_E_INTERNAL);

    u32 myAlgIdx = 0;
    CHK_RET(GetAlgRank(myRank_, subCommRanks_[0], myAlgIdx));
    u64 sliceSize = 0;
    std::vector<u64> sizeOut;
    std::vector<u64> elemOffset;
    if (myAlgIdx == templateRankSize_ - 1 && tempAlgParams_.tailSize > 0) {
        sliceSize = tempAlgParams_.tailSize;
        sizeOut = sizeOutTail_;
        elemOffset = elemOffsetTail_;
    } else {
        sliceSize = tempAlgParams_.sliceSize;
        sizeOut = sizeOut_;
        elemOffset = elemOffset_;
    }
    ThreadHandle q = threads[channelIdx];

    const u64 rptNum = std::max<u64>(1, tempAlgParams_.repeatNum);
    for (u64 rpt = 0; rpt < rptNum; ++rpt) {
        const u64 outBaseOff = tempAlgParams_.buffInfo.outBuffBaseOff + rpt * tempAlgParams_.outputRepeatStride;
        const u64 scratchBase = tempAlgParams_.buffInfo.hcclBuffBaseOff + rpt * tempAlgParams_.outputRepeatStride;
        u64 scOff = scratchBase + tempAlgParams_.sliceSize * myAlgIdx + elemOffset[channelIdx];
        if (!doPreCopy_) {
            scOff = scratchBase + tempAlgParams_.inputSliceStride * myAlgIdx + elemOffset[channelIdx];
        }
        const u64 outOff = outBaseOff + myAlgIdx * tempAlgParams_.outputSliceStride + elemOffset[channelIdx];

        DataSlice src = DataSlice(tempAlgParams_.buffInfo.hcclBuff.addr, scOff, sizeOut[channelIdx]);
        DataSlice dst = DataSlice(tempAlgParams_.buffInfo.outputPtr, outOff, sizeOut[channelIdx]);

        if (tempAlgParams_.buffInfo.hcclBuffType != tempAlgParams_.buffInfo.outBuffType || scOff != outOff) {
            CHK_RET(LocalCopy(q, src, dst));
        }
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempReduceScatterNHR::RunNHR(const std::vector<ThreadHandle>& threads, u32 channelIdx)
{
    CHK_PRT_RET(threads.empty(), HCCL_ERROR("[RS-NHR][RunNHR] empty queue"), HcclResult::HCCL_E_INTERNAL);

    if (templateRankSize_ <= 1)
        return HcclResult::HCCL_SUCCESS;

    ThreadHandle q = threads[channelIdx];

    const u64 rptNum = std::max<u64>(1, tempAlgParams_.repeatNum);
    std::vector<u64> sizeOut;
    std::vector<u64> elemOffset;
    std::vector<AicpuNHRStepInfo> steps;
    CHK_RET(GetStepInfoList(steps));
    for (u32 s = 0; s < steps.size(); ++s) {
        const auto& st = steps[s];

        const u32 recvFromRank = subCommRanks_[0][st.fromRank];
        const u32 sendToRank = subCommRanks_[0][st.toRank];
        CHK_PRT_RET(
            recvFromRank == static_cast<u32>(-1) || sendToRank == static_cast<u32>(-1),
            HCCL_ERROR("[RS-NHR][RunNHR] rank map failed: from[%u] to[%u]", st.fromRank, st.toRank),
            HcclResult::HCCL_E_INTERNAL);

        CHK_PRT_RET(
            channels_.count(recvFromRank) == 0 || channels_.count(sendToRank) == 0 ||
                channelIdx >= channels_[recvFromRank].size() || channelIdx >= channels_[sendToRank].size(),
            HCCL_ERROR(
                "[RS-NHR][RunNHR] link missing: recvFrom=%d sendTo=%d channelIdx=%u", recvFromRank, sendToRank,
                channelIdx),
            HcclResult::HCCL_E_INTERNAL);
        ChannelInfo linkRecv = channels_[recvFromRank].at(channelIdx);
        ChannelInfo linkSend = channels_[sendToRank].at(channelIdx);

        std::vector<DataSlice> txSrcSlices;
        std::vector<DataSlice> txDstSlices;
        std::vector<DataSlice> rxSrcSlices;
        std::vector<DataSlice> rxDstSlices;
        txSrcSlices.reserve(st.nSlices * rptNum);
        txDstSlices.reserve(st.nSlices * rptNum);
        rxSrcSlices.reserve(st.nSlices * rptNum);
        rxDstSlices.reserve(st.nSlices * rptNum);

        void* sendRemoteCclBuffAddr = linkSend.remoteCclMem.addr;
        void* recvRemoteCclBuffAddr = linkRecv.remoteCclMem.addr;
        for (u64 rpt = 0; rpt < rptNum; ++rpt) {
            const u64 scratchBase = tempAlgParams_.buffInfo.hcclBuffBaseOff + rpt * tempAlgParams_.outputRepeatStride;
            const u64 inBaseOff = tempAlgParams_.buffInfo.inBuffBaseOff + rpt * tempAlgParams_.inputRepeatStride;
            for (u32 i = 0; i < st.nSlices; ++i) {
                const u32 txIdx = st.txSliceIdxs[i]; // 算法序
                const u32 rxIdx = st.rxSliceIdxs[i];

                const u64 txelemOffset = (txIdx == templateRankSize_ - 1 && tempAlgParams_.tailSize > 0) ?
                                             elemOffsetTail_[channelIdx] :
                                             elemOffset_[channelIdx];
                const u64 rxelemOffset = (rxIdx == templateRankSize_ - 1 && tempAlgParams_.tailSize > 0) ?
                                             elemOffsetTail_[channelIdx] :
                                             elemOffset_[channelIdx];
                u64 txScOff = scratchBase + tempAlgParams_.sliceSize * txIdx + txelemOffset;
                u64 rxScOff = scratchBase + tempAlgParams_.sliceSize * rxIdx + rxelemOffset;
                if (!doPreCopy_) {
                    txScOff = scratchBase + tempAlgParams_.inputSliceStride * txIdx + txelemOffset;
                    rxScOff = scratchBase + tempAlgParams_.inputSliceStride * rxIdx + rxelemOffset;
                }

                const u64 txSliceSize = (txIdx == templateRankSize_ - 1 && tempAlgParams_.tailSize > 0) ?
                                            sizeOutTail_[channelIdx] :
                                            sizeOut_[channelIdx];
                const u64 rxSliceSize = (rxIdx == templateRankSize_ - 1 && tempAlgParams_.tailSize > 0) ?
                                            sizeOutTail_[channelIdx] :
                                            sizeOut_[channelIdx];

                void* txSrcAddr = tempAlgParams_.buffInfo.hcclBuff.addr;
                u64 txSrcOff = txScOff;
                u64 txDstOff = txScOff;
                if (skipStep0TxPreCopy_ && s == 0) {
                    txSrcAddr = tempAlgParams_.buffInfo.inputPtr;
                    txSrcOff = inBaseOff + tempAlgParams_.inputSliceStride * txIdx + txelemOffset;
                }

                DataSlice txSrcSlice =
                    DataSlice(txSrcAddr, txSrcOff, txSliceSize, txSliceSize / DATATYPE_SIZE_TABLE[dataType_]); // 发送源
                DataSlice txDstSlice = DataSlice(
                    sendRemoteCclBuffAddr, txDstOff, txSliceSize,
                    txSliceSize / DATATYPE_SIZE_TABLE[dataType_]); // 发送目标
                txSrcSlices.push_back(txSrcSlice);
                txDstSlices.push_back(txDstSlice);
                DataSlice rxSrcSlice = DataSlice(
                    recvRemoteCclBuffAddr, rxScOff, rxSliceSize,
                    rxSliceSize / DATATYPE_SIZE_TABLE[dataType_]); // 发送源
                DataSlice rxDstSlice = DataSlice(
                    tempAlgParams_.buffInfo.hcclBuff.addr, rxScOff, rxSliceSize,
                    rxSliceSize / DATATYPE_SIZE_TABLE[dataType_]); // 发送目标
                rxSrcSlices.push_back(rxSrcSlice);
                rxDstSlices.push_back(rxDstSlice);
            }
        }

        SendRecvReduceInfo info{
            {linkSend, linkRecv}, {{txSrcSlices, txDstSlices}, {rxSrcSlices, rxDstSlices}}, dataType_, reduceOp_};

        if (isDmaRead_) {
            CHK_PRT_RET(
                SendRecvReadReduce(info, threads[channelIdx]),
                HCCL_ERROR("[RS-NHR][RunNHR] SendRecvReduce failed (step=%u)", st.step), HcclResult::HCCL_E_INTERNAL);
        } else {
            CHK_PRT_RET(
                SendRecvBatchWriteReduce(info, threads[channelIdx]),
                HCCL_ERROR("[RS-NHR][RunNHR] SendRecvBatchWriteReduce failed (step=%u)", st.step),
                HcclResult::HCCL_E_INTERNAL);
        }
    }

    return HcclResult::HCCL_SUCCESS;
}

u64 InsTempReduceScatterNHR::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType)
{
    (void)inBuffType;
    (void)outBuffType;
    HCCL_INFO("[InsTempReduceScatterNHR][CalcScratchMultiple] templateScratchMultiplier[%llu]", templateRankSize_);
    return templateRankSize_;
}

//  计算每轮收发的对端以及slice编号
HcclResult InsTempReduceScatterNHR::GetStepInfoList(std::vector<AicpuNHRStepInfo>& stepInfoList)
{
    // ReduceScatterNHR 将本rank号转换成算法使用的索引号
    u32 u32x = 0;
    CHK_RET(GetAlgRank(myRank_, subCommRanks_[0], u32x));

    stepInfoList.clear();
    step0TxSliceIdxs_.clear();

    u32 nSteps = GetNHRStepNum(templateRankSize_);
    stepInfoList.clear();
    stepInfoList.resize(nSteps);
    for (u32 step = 0; step < nSteps; step++) {
        // ReduceScatter NHR计算通信对象
        u32 deltaRank = 1 << step;
        u32 sendTo = (u32x + templateRankSize_ - deltaRank) % templateRankSize_;
        u32 recvFrom = (u32x + deltaRank) % templateRankSize_;

        // ReduceScatterNHR 数据份数和数据编号增量
        u32 txSliceIdx = sendTo;
        u32 rxSliceIdx = u32x;
        u32 nSlices = (templateRankSize_ - 1 + (1 << step)) / (1 << (step + 1));
        u32 deltaSliceIndex = 1 << (step + 1);

        AicpuNHRStepInfo& currStepInfo = stepInfoList[step];
        currStepInfo.step = step;
        currStepInfo.myRank = u32x;
        currStepInfo.nSlices = nSlices;
        currStepInfo.toRank = sendTo;
        currStepInfo.fromRank = recvFrom;

        // ReduceScatterNHR 计算本rank在每轮收/发中的slice编号
        currStepInfo.txSliceIdxs.reserve(nSlices);
        currStepInfo.rxSliceIdxs.reserve(nSlices);
        for (u32 i = 0; i < nSlices; i++) {
            currStepInfo.txSliceIdxs.push_back(txSliceIdx);
            if (step == 0) {
                step0TxSliceIdxs_.insert(txSliceIdx);
            }
            currStepInfo.rxSliceIdxs.push_back(rxSliceIdx);
            HCCL_DEBUG(
                "[InsTempReduceScatterNHR][GetStepInfoList] i[%u] txSliceIdx[%u] rxSliceIdx[%u]", i, txSliceIdx,
                rxSliceIdx);
            txSliceIdx = (txSliceIdx + templateRankSize_ - deltaSliceIndex) % templateRankSize_;
            rxSliceIdx = (rxSliceIdx + templateRankSize_ - deltaSliceIndex) % templateRankSize_;
        }
    }
    return HcclResult::HCCL_SUCCESS;
}

void InsTempReduceScatterNHR::GetNotifyIdxMainToSub(std::vector<u32>& notifyIdxMainToSub)
{
    notifyIdxMainToSub.clear();
    u32 threadNum = 1 * channelsPerRank_;
    u32 slaveThreadNum = threadNum - 1;
    for (u32 slaveThreadIdx = 0; slaveThreadIdx < slaveThreadNum; slaveThreadIdx++) {
        notifyIdxMainToSub.push_back(0);
    }
}

void InsTempReduceScatterNHR::GetNotifyIdxSubToMain(std::vector<u32>& notifyIdxSubToMain)
{
    notifyIdxSubToMain.clear();
    u32 threadNum = 1 * channelsPerRank_;
    u32 notifyNum = threadNum - 1;
    for (u32 notifyIdx = 0; notifyIdx < notifyNum; notifyIdx++) {
        notifyIdxSubToMain.push_back(notifyIdx);
    }
}

} // namespace mc2_ops_hccl
