/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ins_temp_all_gather_omnipipe_nhr.h"
#include "alg_data_trans_wrapper.h"
#include "omnipipe_template_utils.h"

namespace mc2_ops_hccl {
InsTempAllGatherOmniPipeNHR::InsTempAllGatherOmniPipeNHR(
    const OpParam& param, const u32 rankId, const std::vector<std::vector<u32>>& subCommRanks)
    : InsTempAllGatherNHR(param, rankId, subCommRanks)
{}

InsTempAllGatherOmniPipeNHR::~InsTempAllGatherOmniPipeNHR() {}

HcclResult InsTempAllGatherOmniPipeNHR::KernelRun(
    const OpParam& param, const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource)
{
    HCCL_INFO("[InsTempAllGatherOmniPipeNHR] Run start");
    if (templateRankSize_ == 1) {
        HCCL_INFO("[InsTempAllGatherOmniPipeNHR] Rank [%d], template ranksize is 1.", myRank_);
        return HcclResult::HCCL_SUCCESS;
    }
    threadNum_ = GetThreadNum();
    tempAlgParams_ = tempAlgParams;
    dataType_ = param.DataDes.dataType;
    dataTypeSize_ = SIZE_TABLE[dataType_];
    tempAlgParams_.buffInfo.outputPtr = param.outputPtr;
    omniLastStepRead_ = tempAlgParams.omniLastStepRead_;

    CHK_RET(PrepareOmniPipeDataSplitForMultiChannel(
        this, tempAlgParams_, dataType_, templateResource, dataSplitVec_, dataOffsetVec_));

    if (threadNum_ > 1) {
        std::vector<ThreadHandle> subThreads(templateResource.threads.begin() + 1, templateResource.threads.end());
        GetNotifyIdxMainToSub(notifyIdxMainToSub_);
        CHK_RET(PreSyncInterThreads(templateResource.threads[0], subThreads, notifyIdxMainToSub_));
    }
    HCCL_DEBUG(
        "MT channelsPerRank_ = %llu, templateRankSize_ = %llu", static_cast<u64>(channelsPerRank_),
        static_cast<u64>(templateRankSize_));
    for (u32 channelIdx = 0; channelIdx < channelsPerRank_; channelIdx++) {
        CHK_RET(RunAllGatherNHRChannel(templateResource.threads, templateResource.channels, channelIdx));
    }
    if (threadNum_ > 1) {
        std::vector<ThreadHandle> subThreads(templateResource.threads.begin() + 1, templateResource.threads.end());
        GetNotifyIdxSubToMain(notifyIdxSubToMain_);
        CHK_RET(PostSyncInterThreads(templateResource.threads[0], subThreads, notifyIdxSubToMain_));
    }
    if (threadNum_ > 1) {
        std::vector<ThreadHandle> subThreads(templateResource.threads.begin() + 1, templateResource.threads.end());
        GetNotifyIdxMainToSub(notifyIdxMainToSub_);
        CHK_RET(PreSyncInterThreads(templateResource.threads[0], subThreads, notifyIdxMainToSub_));
    }
    HCCL_DEBUG(
        "MT channelsPerRank_ = %llu, templateRankSize_ = %llu", static_cast<u64>(channelsPerRank_),
        static_cast<u64>(templateRankSize_));
    for (u32 channelIdx = 0; channelIdx < channelsPerRank_; channelIdx++) {
        if (lastStepNhrCopy_) {
            CHK_RET(DoLastStepCopyNhr(templateResource.threads, templateResource.channels, channelIdx));
        }
    }
    if (threadNum_ > 1) {
        std::vector<ThreadHandle> subThreads(templateResource.threads.begin() + 1, templateResource.threads.end());
        GetNotifyIdxSubToMain(notifyIdxSubToMain_);
        CHK_RET(PostSyncInterThreads(templateResource.threads[0], subThreads, notifyIdxSubToMain_));
    }
    HCCL_INFO("[InsTempAllGatherOmniPipeNHR] Run End");
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllGatherOmniPipeNHR::SetchannelsPerRank(const std::map<u32, std::vector<ChannelInfo>>& channels)
{
    channelsPerRank_ = CalcChannelsPerRank(channels);
    return HCCL_SUCCESS;
}

HcclResult InsTempAllGatherOmniPipeNHR::DoLastStepCopyNhr(
    const std::vector<ThreadHandle>& threads, const std::map<u32, std::vector<ChannelInfo>>& channels,
    const u32& channelIdx)
{
    u32 myAlgRank = 0;
    CHK_RET(GetAlgRank(myRank_, subCommRanks_[0], myAlgRank));
    const u32 nSteps = GetNHRStepNum(templateRankSize_);
    bool isPcieProtocal = IsPcieProtocol(channels);
    const u32 dataTypeSize = dataTypeSize_;
    for (u32 step = 0; step < nSteps - 1; ++step) {
        AicpuNHRStepInfo stepInfo;
        CHK_RET(GetStepInfo(step, nSteps, stepInfo));
        for (u32 i = 0; i < stepInfo.nSlices; ++i) {
            for (u32 rpt = 0; rpt < tempAlgParams_.stepSliceInfo.inputOmniPipeSliceStride[myAlgRank].size(); ++rpt) {
                const u32 txIdx = stepInfo.txSliceIdxs[i];
                const u32 rxIdx = stepInfo.rxSliceIdxs[i];
                u64 txScratchBase = tempAlgParams_.buffInfo.inBuffBaseOff +
                                    tempAlgParams_.stepSliceInfo.inputOmniPipeSliceStride[txIdx][rpt];
                txScratchBase += dataOffsetVec_[txIdx][rpt][channelIdx];
                u64 rxScratchBase = tempAlgParams_.buffInfo.outBuffBaseOff +
                                    tempAlgParams_.stepSliceInfo.outputOmniPipeSliceStride[rxIdx][rpt];
                rxScratchBase += dataOffsetVec_[rxIdx][rpt][channelIdx];
                const u64 txScratchOff = txScratchBase + tempAlgParams_.stepSliceInfo.stepInputSliceStride[txIdx];
                const u64 rxScratchOff = rxScratchBase + tempAlgParams_.stepSliceInfo.stepInputSliceStride[rxIdx];
                u64 txOutBase = tempAlgParams_.buffInfo.inBuffBaseOff +
                                tempAlgParams_.omniReadDstStepSliceInfo.inputOmniPipeSliceStride[txIdx][rpt];
                txOutBase += dataOffsetVec_[txIdx][rpt][channelIdx];
                u64 rxOutBase = tempAlgParams_.buffInfo.outBuffBaseOff +
                                tempAlgParams_.omniReadDstStepSliceInfo.outputOmniPipeSliceStride[rxIdx][rpt];
                rxOutBase += dataOffsetVec_[rxIdx][rpt][channelIdx];
                const u64 txOutOff = txOutBase + tempAlgParams_.omniReadDstStepSliceInfo.stepInputSliceStride[txIdx] +
                                     tempAlgParams_.processedDataCount * dataTypeSize;
                const u64 rxOutOff = rxOutBase + tempAlgParams_.omniReadDstStepSliceInfo.stepInputSliceStride[rxIdx] +
                                     tempAlgParams_.processedDataCount * dataTypeSize;
                DataSlice rxSrcSlices = DataSlice(
                    tempAlgParams_.buffInfo.hcclBuff.addr, rxScratchOff, dataSplitVec_[rxIdx][rpt][channelIdx],
                    dataSplitVec_[rxIdx][rpt][channelIdx] / dataTypeSize);
                DataSlice rxDstSlices = DataSlice(
                    tempAlgParams_.buffInfo.outputPtr, rxOutOff, dataSplitVec_[rxIdx][rpt][channelIdx],
                    dataSplitVec_[rxIdx][rpt][channelIdx] / dataTypeSize);
                CHK_RET(LocalCopy(threads[channelsPerRank_ + channelIdx], rxSrcSlices, rxDstSlices));
            }
        }
    }
    return HCCL_SUCCESS;
}

HcclResult InsTempAllGatherOmniPipeNHR::RunAllGatherNHRChannel(
    const std::vector<ThreadHandle>& threads, const std::map<u32, std::vector<ChannelInfo>>& channels,
    const u32& channelIdx)
{
    HCCL_INFO("RunAllGatherNHRChannel InsTempAllGatherOmniPipeNHR");
    u32 myAlgRank = 0;
    CHK_RET(GetAlgRank(myRank_, subCommRanks_[0], myAlgRank));
    const u32 nSteps = GetNHRStepNum(templateRankSize_);
    bool isPcieProtocal = IsPcieProtocol(channels);
    const u32 dataTypeSize = dataTypeSize_;
    for (u32 step = 0; step < nSteps; ++step) {
        AicpuNHRStepInfo stepInfo;
        CHK_RET(GetStepInfo(step, nSteps, stepInfo));

        const ChannelInfo& channelRecv = channels.at(GetRankFromMap(stepInfo.fromRank))[channelIdx];
        const ChannelInfo& channelSend = channels.at(GetRankFromMap(stepInfo.toRank))[channelIdx];

        std::vector<DataSlice> txSrcSlices;
        std::vector<DataSlice> txDstSlices;
        std::vector<DataSlice> rxSrcSlices;
        std::vector<DataSlice> rxDstSlices;

        void* sendCclBuffAddr = channelSend.remoteCclMem.addr;
        void* recvCclBuffAddr = channelRecv.remoteCclMem.addr;

        if (omniLastStepRead_ && (step == nSteps - 1)) {
            lastStepNhrCopy_ = true;
        }

        HCCL_DEBUG(
            "[InsTempAllGatherOmniPipeNHR] rank[%d] rankSize[%u] recvFrom[%u] sendTo[%u] step[%u] nSteps[%u] "
            "nSlices[%u]",
            myRank_, templateRankSize_, stepInfo.fromRank, stepInfo.toRank, step, nSteps, stepInfo.nSlices);

        for (u32 i = 0; i < stepInfo.nSlices; ++i) {
            const u32 txIdx = stepInfo.txSliceIdxs[i];
            const u32 rxIdx = stepInfo.rxSliceIdxs[i];
            for (u32 rpt = 0; rpt < tempAlgParams_.stepSliceInfo.inputOmniPipeSliceStride[myAlgRank].size(); ++rpt) {
                u64 txScratchBase = tempAlgParams_.buffInfo.inBuffBaseOff +
                                    tempAlgParams_.stepSliceInfo.inputOmniPipeSliceStride[txIdx][rpt];
                txScratchBase += dataOffsetVec_[txIdx][rpt][channelIdx];
                u64 rxScratchBase = tempAlgParams_.buffInfo.outBuffBaseOff +
                                    tempAlgParams_.stepSliceInfo.outputOmniPipeSliceStride[rxIdx][rpt];
                rxScratchBase += dataOffsetVec_[rxIdx][rpt][channelIdx];
                const u64 txScratchOff = txScratchBase + tempAlgParams_.stepSliceInfo.stepInputSliceStride[txIdx];
                const u64 rxScratchOff = rxScratchBase + tempAlgParams_.stepSliceInfo.stepInputSliceStride[rxIdx];

                if (!omniLastStepRead_ || step != nSteps - 1) {
                    txSrcSlices.emplace_back(
                        tempAlgParams_.buffInfo.hcclBuff.addr, txScratchOff, dataSplitVec_[txIdx][rpt][channelIdx],
                        dataSplitVec_[txIdx][rpt][channelIdx] / dataTypeSize);
                    txDstSlices.emplace_back(
                        sendCclBuffAddr, txScratchOff, dataSplitVec_[txIdx][rpt][channelIdx],
                        dataSplitVec_[txIdx][rpt][channelIdx] / dataTypeSize);
                    rxSrcSlices.emplace_back(
                        recvCclBuffAddr, rxScratchOff, dataSplitVec_[rxIdx][rpt][channelIdx],
                        dataSplitVec_[rxIdx][rpt][channelIdx] / dataTypeSize);
                    rxDstSlices.emplace_back(
                        tempAlgParams_.buffInfo.hcclBuff.addr, rxScratchOff, dataSplitVec_[rxIdx][rpt][channelIdx],
                        dataSplitVec_[rxIdx][rpt][channelIdx] / dataTypeSize);
                } else {
                    u64 txOutBase = tempAlgParams_.buffInfo.inBuffBaseOff +
                                    tempAlgParams_.omniReadDstStepSliceInfo.inputOmniPipeSliceStride[txIdx][rpt];
                    txOutBase += dataOffsetVec_[txIdx][rpt][channelIdx];
                    u64 rxOutBase = tempAlgParams_.buffInfo.outBuffBaseOff +
                                    tempAlgParams_.omniReadDstStepSliceInfo.outputOmniPipeSliceStride[rxIdx][rpt];
                    rxOutBase += dataOffsetVec_[rxIdx][rpt][channelIdx];
                    const u64 txOutOff = txOutBase +
                                         tempAlgParams_.omniReadDstStepSliceInfo.stepInputSliceStride[txIdx] +
                                         tempAlgParams_.processedDataCount * dataTypeSize;
                    const u64 rxOutOff = rxOutBase +
                                         tempAlgParams_.omniReadDstStepSliceInfo.stepInputSliceStride[rxIdx] +
                                         tempAlgParams_.processedDataCount * dataTypeSize;

                    txSrcSlices.emplace_back(
                        tempAlgParams_.buffInfo.outputPtr, txOutOff, dataSplitVec_[txIdx][rpt][channelIdx],
                        dataSplitVec_[txIdx][rpt][channelIdx] / dataTypeSize);
                    txDstSlices.emplace_back(
                        sendCclBuffAddr, txScratchOff, dataSplitVec_[txIdx][rpt][channelIdx],
                        dataSplitVec_[txIdx][rpt][channelIdx] / dataTypeSize);
                    rxSrcSlices.emplace_back(
                        recvCclBuffAddr, rxScratchOff, dataSplitVec_[rxIdx][rpt][channelIdx],
                        dataSplitVec_[rxIdx][rpt][channelIdx] / dataTypeSize);
                    rxDstSlices.emplace_back(
                        tempAlgParams_.buffInfo.outputPtr, rxOutOff, dataSplitVec_[rxIdx][rpt][channelIdx],
                        dataSplitVec_[rxIdx][rpt][channelIdx] / dataTypeSize);
                }
            }
        }
        TxRxSlicesList sendRecvSlicesList({txSrcSlices, txDstSlices}, {rxSrcSlices, rxDstSlices});
        TxRxChannels sendRecvChannels(channelSend, channelRecv);
        SendRecvInfo sendRecvInfo(sendRecvChannels, sendRecvSlicesList);

        if (!omniLastStepRead_ || step != nSteps - 1) {
            if (isPcieProtocal) {
                CHK_PRT_RET(
                    SendRecvRead(sendRecvInfo, threads[channelIdx]),
                    HCCL_ERROR("[InsTempAllGatherOmniPipeNHR] sendrecv failed (step=%u)", step),
                    HcclResult::HCCL_E_INTERNAL);
            } else {
                CHK_PRT_RET(
                    SendRecvWrite(sendRecvInfo, threads[channelIdx]),
                    HCCL_ERROR("[InsTempAllGatherOmniPipeNHR] sendrecv failed (step=%u)", step),
                    HcclResult::HCCL_E_INTERNAL);
            }
        } else {
            CHK_PRT_RET(
                SendRecvRead(sendRecvInfo, threads[channelIdx]),
                HCCL_ERROR("[InsTempAllGatherOmniPipeNHR] sendrecv failed (step=%u)", step),
                HcclResult::HCCL_E_INTERNAL);
        }
    }
    return HcclResult::HCCL_SUCCESS;
}
} // namespace mc2_ops_hccl
