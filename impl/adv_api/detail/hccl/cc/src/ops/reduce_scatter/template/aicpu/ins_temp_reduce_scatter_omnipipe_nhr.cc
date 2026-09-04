/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ins_temp_reduce_scatter_omnipipe_nhr.h"
#include "omnipipe_template_utils.h"
constexpr u32 SMALL_COUNT_512KB = 512 * 1024;
namespace mc2_ops_hccl {
InsTempReduceScatterOmniPipeNHR::InsTempReduceScatterOmniPipeNHR(
    const OpParam& param, const u32 rankId, // 传通信域的u32，userRank
    const std::vector<std::vector<u32>>& subCommRanks)
    : InsTempReduceScatterNHR(param, rankId, subCommRanks)
{}

InsTempReduceScatterOmniPipeNHR::~InsTempReduceScatterOmniPipeNHR() {}

// 语义改为返回当前template的类型，mesh返回1，nhr返回0
u64 InsTempReduceScatterOmniPipeNHR::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType) { return 0; }

HcclResult InsTempReduceScatterOmniPipeNHR::KernelRun(
    const OpParam& param, const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource)
{
    HCCL_INFO("[InsTempReduceScatterOmniPipeNHR] GenExtIns start");
    if (templateRankSize_ == 1) {
        HCCL_INFO("[InsTempReduceScatterOmniPipeNHR] Rank [%d], template ranksize is 1.", myRank_);
        return HcclResult::HCCL_SUCCESS;
    }
    tempAlgParams_ = tempAlgParams;
    channels_ = templateResource.channels;
    dataType_ = param.DataDes.dataType;

    threadNum_ = GetThreadNum();

    // 这里的步骤nhr无需流同步、前后copy单独拿出来在executor中控制执行
    CHK_RET(PrepareOmniPipeDataSplitForMultiChannel(
        this, tempAlgParams_, dataType_, templateResource, dataSplitVec_, dataOffsetVec_));
    HCCL_DEBUG("MT channelsPerRank_ = %llu, templateRankSize_ = %llu", channelsPerRank_, templateRankSize_);
    if (threadNum_ > 1) {
        std::vector<ThreadHandle> subThreads(templateResource.threads.begin() + 1, templateResource.threads.end());
        GetNotifyIdxMainToSub(notifyIdxMainToSub_);
        CHK_RET(PreSyncInterThreads(templateResource.threads[0], subThreads, notifyIdxMainToSub_));
    }
    HCCL_DEBUG("MT channelsPerRank_ = %llu, templateRankSize_ = %llu", channelsPerRank_, templateRankSize_);
    for (u32 channelIdx = 0; channelIdx < channelsPerRank_; channelIdx++) {
        CHK_RET(RunNHR(templateResource.threads, channelIdx));
    }
    if (threadNum_ > 1) {
        std::vector<ThreadHandle> subThreads(templateResource.threads.begin() + 1, templateResource.threads.end());
        GetNotifyIdxSubToMain(notifyIdxSubToMain_);
        CHK_RET(PostSyncInterThreads(templateResource.threads[0], subThreads, notifyIdxSubToMain_));
    }
    HCCL_INFO("[InsTempAllGatherOmniPipeNHR] Run End");
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempReduceScatterOmniPipeNHR::DoLocalCopy(
    const TemplateDataParams& tempAlgParams, const std::vector<ThreadHandle>& threads) const
{
    HCCL_INFO("[InsTempReduceScatterOmniPipeNHR][DoLocalCopy] DoLocalCopy myRank_ = [%u]", myRank_);
    u32 rankIdx = 0;
    auto iter = std::find(subCommRanks_[0].begin(), subCommRanks_[0].end(), myRank_);
    if (iter != subCommRanks_[0].end()) {
        rankIdx = std::distance(subCommRanks_[0].begin(), iter);
    } else {
        HCCL_ERROR("[%s]subCommRanks_ or myRank_ is error.", __func__);
        return HCCL_E_INTERNAL;
    }

    // 区分前后搬运
    void* srcAddr;
    void* dstAddr;
    if (tempAlgParams.buffInfo.inBuffType == BufferType::INPUT) {
        srcAddr = tempAlgParams.buffInfo.inputPtr;
        dstAddr = tempAlgParams.buffInfo.hcclBuff.addr;
    } else if (tempAlgParams.buffInfo.inBuffType == BufferType::HCCL_BUFFER) {
        srcAddr = tempAlgParams.buffInfo.hcclBuff.addr;
        dstAddr = tempAlgParams.buffInfo.outputPtr;
    } else {
        HCCL_ERROR("[%s]InputBufferType Error.", __func__);
        return HCCL_E_PARA;
    }
    // 这里的循环precopy是ranksize-1，postcopy是1
    for (auto i = 0; i < tempAlgParams.repeatNum; ++i) {
        auto srcSlice = DataSlice(
            srcAddr, tempAlgParams.buffInfo.inBuffBaseOff + i * tempAlgParams.inputSliceStride, tempAlgParams.sliceSize,
            tempAlgParams.count);
        auto dstSlice = DataSlice(
            dstAddr, tempAlgParams.buffInfo.outBuffBaseOff + i * tempAlgParams.outputSliceStride,
            tempAlgParams.sliceSize, tempAlgParams.count);
        CHK_RET(static_cast<HcclResult>(LocalCopy(threads[0], srcSlice, dstSlice)));
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempReduceScatterOmniPipeNHR::GetNHRDataSize(
    const AicpuNHRStepInfo& st, const u32 channelIdx, void* sendCclBuffAddr, void* recvCclBuffAddr,
    const u32 dataTypeSize, const u64 rptNum, std::vector<DataSlice>& txSrcSlices, std::vector<DataSlice>& txDstSlices,
    std::vector<DataSlice>& rxSrcSlices, std::vector<DataSlice>& rxDstSlices)
{
    for (u32 i = 0; i < st.nSlices; ++i) {
        const u32 txIdx = st.txSliceIdxs[i]; // 算法序
        const u32 rxIdx = st.rxSliceIdxs[i];
        for (u64 rpt = 0; rpt < rptNum; ++rpt) {
            u64 scratchBaseTx = tempAlgParams_.buffInfo.inBuffBaseOff +
                                tempAlgParams_.stepSliceInfo.inputOmniPipeSliceStride[txIdx][rpt];
            u64 scratchBaseRx = tempAlgParams_.buffInfo.inBuffBaseOff +
                                tempAlgParams_.stepSliceInfo.inputOmniPipeSliceStride[rxIdx][rpt];
            scratchBaseTx += dataOffsetVec_[txIdx][rpt][channelIdx];
            scratchBaseRx += dataOffsetVec_[rxIdx][rpt][channelIdx];
            // 已对齐，这边都用input
            const u64 txScOff = scratchBaseTx + tempAlgParams_.stepSliceInfo.stepInputSliceStride[txIdx];
            const u64 rxScOff = scratchBaseRx + tempAlgParams_.stepSliceInfo.stepInputSliceStride[rxIdx];

            DataSlice txSrcSlice = DataSlice(
                tempAlgParams_.buffInfo.hcclBuff.addr, txScOff, dataSplitVec_[txIdx][rpt][channelIdx],
                dataSplitVec_[txIdx][rpt][channelIdx] / dataTypeSize); // 发送源
            DataSlice txDstSlice = DataSlice(
                sendCclBuffAddr, txScOff, dataSplitVec_[txIdx][rpt][channelIdx],
                dataSplitVec_[txIdx][rpt][channelIdx] / dataTypeSize); // 发送目标
            DataSlice rxSrcSlice = DataSlice(
                recvCclBuffAddr, rxScOff, dataSplitVec_[rxIdx][rpt][channelIdx],
                dataSplitVec_[rxIdx][rpt][channelIdx] / dataTypeSize);
            DataSlice rxDstSlice = DataSlice(
                tempAlgParams_.buffInfo.hcclBuff.addr, rxScOff, dataSplitVec_[rxIdx][rpt][channelIdx],
                dataSplitVec_[rxIdx][rpt][channelIdx] / dataTypeSize);
            txSrcSlices.emplace_back(txSrcSlice);
            txDstSlices.emplace_back(txDstSlice);
            rxSrcSlices.emplace_back(rxSrcSlice);
            rxDstSlices.emplace_back(rxDstSlice);
        }
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempReduceScatterOmniPipeNHR::RunNHR(const std::vector<ThreadHandle>& threads, u32 channelIdx)
{
    u32 dataTypeSize = DATATYPE_SIZE_TABLE[dataType_];
    CHK_PRT_RET(threads.empty(), HCCL_ERROR("[RS-NHR][RunNHR] empty queue"), HcclResult::HCCL_E_INTERNAL);

    if (templateRankSize_ <= 1)
        return HcclResult::HCCL_SUCCESS;
    bool isPcieProtocal = IsPcieProtocol(channels_);
    // 步进参数，片数由inputOmniPipeSliceStride确定
    const u64 rptNum = std::max<u64>(1, tempAlgParams_.stepSliceInfo.inputOmniPipeSliceStride[0].size());

    // 预计算步骤列表（算法序）
    std::vector<AicpuNHRStepInfo> steps;
    CHK_RET(GetStepInfoList(steps));
    // 基础位置由 hcclBuffBaseOff 和 inputOmniPipeSliceStride 确定
    for (u32 s = 0; s < steps.size(); ++s) {
        const auto& st = steps[s];
        const u32 recvFromRank = subCommRanks_[0].at(st.fromRank);
        const u32 sendToRank = subCommRanks_[0].at(st.toRank);
        CHK_PRT_RET(
            recvFromRank == static_cast<u32>(-1) || sendToRank == static_cast<u32>(-1),
            HCCL_ERROR("[RS-NHR][RunNHR] rank map failed: from[%u] to[%u]", st.fromRank, st.toRank),
            HcclResult::HCCL_E_INTERNAL);

        CHK_PRT_RET(
            channels_.count(recvFromRank) == 0 || channels_.count(sendToRank) == 0 ||
                channels_[recvFromRank].size() == 0 || channels_[sendToRank].size() == 0,
            HCCL_ERROR("[RS-NHR][RunNHR] link missing: recvFrom=%d sendTo=%d", recvFromRank, sendToRank),
            HcclResult::HCCL_E_INTERNAL);

        ChannelInfo linkRecv = channels_[recvFromRank].at(channelIdx);
        ChannelInfo linkSend = channels_[sendToRank].at(channelIdx);
        HCCL_DEBUG(
            "recvFromRank=[%u], sendToRank=[%u], linkRecv.remoteRank=[%u], linkSend.remoteRank=[%u]", recvFromRank,
            sendToRank, linkRecv.remoteRank, linkSend.remoteRank);

        std::vector<DataSlice> txSrcSlices, txDstSlices, rxSrcSlices, rxDstSlices;

        void* sendCclBuffAddr = linkSend.remoteCclMem.addr;
        void* recvCclBuffAddr = linkRecv.remoteCclMem.addr;
        // RS：在 SCRATCH 上进行规约交换
        CHK_RET(GetNHRDataSize(
            st, channelIdx, sendCclBuffAddr, recvCclBuffAddr, dataTypeSize, rptNum, txSrcSlices, txDstSlices,
            rxSrcSlices, rxDstSlices));

        SendRecvReduceInfo info{
            {linkSend, linkRecv}, {{txSrcSlices, txDstSlices}, {rxSrcSlices, rxDstSlices}}, dataType_, reduceOp_};
        if (isPcieProtocal) {
            CHK_PRT_RET(
                SendRecvReadReduce(info, threads[channelIdx]),
                HCCL_ERROR("[RS-NHR][RunNHR] SendRecvReduce failed (step=%u)", st.step), HcclResult::HCCL_E_INTERNAL);
        } else {
            CHK_PRT_RET(
                SendRecvWriteReduce(info, threads[channelIdx]),
                HCCL_ERROR("[RS-NHR][RunNHR] SendRecvReduce failed (step=%u)", st.step), HcclResult::HCCL_E_INTERNAL);
        }
    }
    return HcclResult::HCCL_SUCCESS;
}

//  计算每轮收发的对端以及slice编号
HcclResult InsTempReduceScatterOmniPipeNHR::GetStepInfoList(std::vector<AicpuNHRStepInfo>& stepInfoList) const
{
    // 将本 rank 号转换成算法使用的索引号
    u32 u32x = 0;
    CHK_RET(GetAlgRank(myRank_, subCommRanks_[0], u32x));
    stepInfoList.clear();
    u32 nSteps = GetNHRStepNum(templateRankSize_);
    stepInfoList.resize(nSteps);

    for (u32 step = 0; step < nSteps; step++) {
        // 计算通信对象
        u32 deltaRank = 1 << step;
        u32 sendTo = (u32x + templateRankSize_ - deltaRank) % templateRankSize_;
        u32 recvFrom = (u32x + deltaRank) % templateRankSize_;

        // 数据份数和数据编号增量
        u32 nSlices = (templateRankSize_ - 1 + (1 << step)) / (1 << (step + 1));
        u32 deltaSliceIndex = 1 << (step + 1);
        u32 rxSliceIdx = u32x;
        u32 txSliceIdx = sendTo;

        AicpuNHRStepInfo& currStepInfo = stepInfoList[step];
        currStepInfo.step = step;
        currStepInfo.toRank = sendTo;
        currStepInfo.myRank = u32x;
        currStepInfo.fromRank = recvFrom;
        currStepInfo.nSlices = nSlices;

        // 计算本rank在每轮收/发中的slice编号
        currStepInfo.txSliceIdxs.reserve(nSlices);
        currStepInfo.rxSliceIdxs.reserve(nSlices);
        for (u32 i = 0; i < nSlices; i++) {
            currStepInfo.txSliceIdxs.push_back(txSliceIdx);
            currStepInfo.rxSliceIdxs.push_back(rxSliceIdx);
            HCCL_DEBUG(
                "[InsTempReduceScatterOmniPipeNHR][GetStepInfoList] i[%u] txSliceIdx[%u] rxSliceIdx[%u], step[%u]", i,
                txSliceIdx, rxSliceIdx, step);
            txSliceIdx = (txSliceIdx + templateRankSize_ - deltaSliceIndex) % templateRankSize_;
            rxSliceIdx = (rxSliceIdx + templateRankSize_ - deltaSliceIndex) % templateRankSize_;
        }
    }
    return HcclResult::HCCL_SUCCESS;
}

} // namespace mc2_ops_hccl
