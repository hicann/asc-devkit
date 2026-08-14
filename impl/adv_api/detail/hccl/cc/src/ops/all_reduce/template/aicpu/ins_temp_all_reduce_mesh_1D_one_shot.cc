/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ins_temp_all_reduce_mesh_1D_one_shot.h"
#include "alg_data_trans_wrapper.h"

namespace mc2_ops_hccl {
InsTempAllReduceMesh1DOneShot::InsTempAllReduceMesh1DOneShot(
    const OpParam& param, const u32 rankId, const std::vector<std::vector<u32>>& subCommRanks)
    : InsAlgTemplateBase(param, rankId, subCommRanks)
{}

InsTempAllReduceMesh1DOneShot::~InsTempAllReduceMesh1DOneShot() {}

HcclResult InsTempAllReduceMesh1DOneShot::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    u32 threadNum = templateRankSize_ > 1 ? templateRankSize_ : 1;
    resourceRequest.slaveThreadNum = threadNum - 1;
    resourceRequest.notifyNumPerThread.assign(resourceRequest.slaveThreadNum, 1);
    resourceRequest.notifyNumOnMainThread = threadNum - 1;

    std::vector<HcclChannelDesc> level0Channels;
    CHK_RET(CalcChannelRequestMesh1D(comm, param, topoInfo, subCommRanks_, level0Channels));
    resourceRequest.channels.push_back(level0Channels);
    return HCCL_SUCCESS;
}

u64 InsTempAllReduceMesh1DOneShot::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType)
{
    (void)inBuffType;
    (void)outBuffType;
    return templateRankSize_;
}

HcclResult InsTempAllReduceMesh1DOneShot::CalcSlice(const u64 dataSize, RankSliceInfo& sliceInfoVec) const
{
    std::vector<SliceInfo> tmp(1);
    sliceInfoVec.resize(templateRankSize_, tmp);

    u64 accumOff = 0;
    for (u32 rankIdx = 0; rankIdx < sliceInfoVec.size(); rankIdx++) {
        SliceInfo slice = {accumOff, dataSize};
        sliceInfoVec[rankIdx][0] = slice;
        accumOff += dataSize;
    }
    CHK_PRT_RET(
        (sliceInfoVec[templateRankSize_ - 1][0].offset + sliceInfoVec[templateRankSize_ - 1][0].size !=
         dataSize * templateRankSize_),
        HCCL_ERROR("[InsTempAllReduceMesh1DOneShot] Rank [%d], SliceInfo calculation error!", myRank_),
        HcclResult::HCCL_E_INTERNAL);
    return HCCL_SUCCESS;
}

HcclResult InsTempAllReduceMesh1DOneShot::KernelRun(
    const OpParam& param, const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource)
{
    threadNum_ = templateResource.threads.size();
    processSize_ = tempAlgParams.sliceSize;
    count_ = tempAlgParams.count;
    dataType_ = param.DataDes.dataType;
    needAicpuReduce_ =
        dataType_ == HcclDataType::HCCL_DATA_TYPE_INT64 || dataType_ == HcclDataType::HCCL_DATA_TYPE_UINT64 ||
        dataType_ == HcclDataType::HCCL_DATA_TYPE_FP64 || param.reduceType == HcclReduceOp::HCCL_REDUCE_PROD;
    HCCL_INFO("[InsTempAllReduceMesh1DOneShot] Run Start");
    CHK_PRT_RET(
        threadNum_ != templateRankSize_,
        HCCL_ERROR(
            "[InsTempAllReduceMesh1DOneShot][KernelRun] thread num is invalid, need[%u], actual[%u].",
            templateRankSize_, threadNum_),
        HcclResult::HCCL_E_INTERNAL);

    RankSliceInfo sliceInfoVec;
    CHK_RET(CalcSlice(processSize_, sliceInfoVec));
    if (threadNum_ > 1) {
        std::vector<ThreadHandle> subThreads(templateResource.threads.begin() + 1, templateResource.threads.end());
        GetNotifyIdxMainToSub(notifyIdxMainToSub_);
        CHK_RET(PreSyncInterThreads(templateResource.threads[0], subThreads, notifyIdxMainToSub_));
    }
    CHK_RET(RunAllReduce(param, templateResource.channels, templateResource.threads, tempAlgParams, sliceInfoVec));
    if (threadNum_ > 1) {
        std::vector<ThreadHandle> subThreads(templateResource.threads.begin() + 1, templateResource.threads.end());
        GetNotifyIdxSubToMain(notifyIdxSubToMain_);
        CHK_RET(PostSyncInterThreads(templateResource.threads[0], subThreads, notifyIdxSubToMain_));
    }
    CHK_PRT(PostLocalReduce(param, templateResource.threads, tempAlgParams, sliceInfoVec));
    HCCL_INFO("[InsTempAllReduceMesh1DOneShot] AllReduceMesh1DOneShot finished: rank[%d] end", myRank_);
    return HCCL_SUCCESS;
}

HcclResult InsTempAllReduceMesh1DOneShot::RunAllReduce(
    const OpParam& param, const std::map<u32, std::vector<ChannelInfo>>& channels,
    const std::vector<ThreadHandle>& threads, const TemplateDataParams& tempAlgParams,
    const RankSliceInfo& sliceInfoVec)
{
    (void)param;
    HCCL_INFO("[InsTempAllReduceMesh1DOneShot][RunAllReduce] send/recv: rank[%d]", myRank_);

    DataSlice usrInSlices =
        DataSlice(tempAlgParams.buffInfo.inputPtr, tempAlgParams.buffInfo.inBuffBaseOff, processSize_, count_);
    DataSlice usrOutSlices =
        DataSlice(tempAlgParams.buffInfo.outputPtr, tempAlgParams.buffInfo.outBuffBaseOff, processSize_, count_);

    CHK_RET(static_cast<HcclResult>(LocalCopy(threads[0], usrInSlices, usrOutSlices)));

    if (subCommRanks_[0].size() == 1) {
        return HCCL_SUCCESS;
    }

    for (u32 queIdx = 1; queIdx < threadNum_; queIdx++) {
        u32 nextRank = (myRank_ + queIdx) % templateRankSize_;
        u32 fromRank = subCommRanks_[0][nextRank];
        u32 toRank = subCommRanks_[0][nextRank];

        const ChannelInfo& linkRecv = channels.at(fromRank)[0];
        const ChannelInfo& linkSend = channels.at(toRank)[0];

        std::vector<DataSlice> txSrcSlices;
        std::vector<DataSlice> txDstSlices;
        void* txCclBuffAddr = linkSend.remoteCclMem.addr;
        u64 txDstOffset = sliceInfoVec[myRank_][0].offset + tempAlgParams.buffInfo.hcclBuffBaseOff;
        u64 txDstSize = sliceInfoVec[myRank_][0].size;
        DataSlice txSrcSlice = usrInSlices;
        DataSlice txDstSlice = DataSlice(txCclBuffAddr, txDstOffset, txDstSize, count_);
        txSrcSlices.push_back(txSrcSlice);
        txDstSlices.push_back(txDstSlice);

        std::vector<DataSlice> rxSrcSlices;
        std::vector<DataSlice> rxDstSlices;
        void* rxCclBuffAddr = linkRecv.remoteCclMem.addr;
        u64 rxDstOffset = sliceInfoVec[fromRank][0].offset + tempAlgParams.buffInfo.hcclBuffBaseOff;
        u64 rxDstSize = sliceInfoVec[fromRank][0].size;
        DataSlice rxSrcSlice = usrInSlices;
        DataSlice rxDstSlice = DataSlice(rxCclBuffAddr, rxDstOffset, rxDstSize, count_);
        rxSrcSlices.push_back(rxSrcSlice);
        rxDstSlices.push_back(rxDstSlice);

        SendRecvInfo sendRecvInfo{
            {linkSend, linkRecv}, {{txSrcSlices, txDstSlices}, {rxSrcSlices, rxDstSlices}}, dataType_};
        CHK_PRT_RET(
            SendRecvBatchWrite(sendRecvInfo, threads[queIdx]),
            HCCL_ERROR("[InsTempAllReduceMesh1DOneShot] RunAllReduce SendRecv failed"), HcclResult::HCCL_E_INTERNAL);
    }

    return HCCL_SUCCESS;
}

HcclResult InsTempAllReduceMesh1DOneShot::PostLocalReduce(
    const OpParam& param, const std::vector<ThreadHandle>& threads, const TemplateDataParams& tempAlgParams,
    const RankSliceInfo& sliceInfoVec)
{
    HCCL_INFO("[InsTempAllReduceMesh1DOneShot][PostLocalReduce] reduce: rank[%d]", myRank_);
    if (needAicpuReduce_) {
        CHK_RET(static_cast<HcclResult>(HcommBatchModeEnd(param.algTag)));
        CHK_RET(static_cast<HcclResult>(HcommBatchModeStart(param.algTag)));
        for (const auto& thread : threads) {
            CHK_RET(static_cast<HcclResult>(HcommThreadJoin(thread, CUSTOM_TIMEOUT)));
        }
    }

    DataSlice usrOutSlices =
        DataSlice(tempAlgParams.buffInfo.outputPtr, tempAlgParams.buffInfo.outBuffBaseOff, processSize_, count_);

    for (u32 rankIdx = 0; rankIdx < subCommRanks_[0].size(); rankIdx++) {
        u32 curRank = rankIdx;
        if (curRank == myRank_) {
            continue;
        }

        void* remotePtr = tempAlgParams.buffInfo.hcclBuff.addr;
        u64 curSrcOffset = sliceInfoVec[curRank][0].offset + tempAlgParams.buffInfo.hcclBuffBaseOff;
        u64 curSrcSize = sliceInfoVec[curRank][0].size;
        DataSlice curSrcSlice = DataSlice(remotePtr, curSrcOffset, curSrcSize, count_);
        DataSlice curDstSlice = usrOutSlices;

        CHK_RET(static_cast<HcclResult>(LocalReduce(threads[0], curSrcSlice, curDstSlice, dataType_, reduceOp_)));
    }
    return HCCL_SUCCESS;
}

void InsTempAllReduceMesh1DOneShot::GetNotifyIdxMainToSub(std::vector<u32>& notifyIdxMainToSub)
{
    notifyIdxMainToSub.clear();
    u32 threadNum = templateRankSize_ > 1 ? templateRankSize_ : 1;
    u32 slaveThreadNum = threadNum - 1;
    for (u32 slaveThreadIdx = 0; slaveThreadIdx < slaveThreadNum; slaveThreadIdx++) {
        notifyIdxMainToSub.push_back(0);
    }
}

void InsTempAllReduceMesh1DOneShot::GetNotifyIdxSubToMain(std::vector<u32>& notifyIdxSubToMain)
{
    notifyIdxSubToMain.clear();
    u32 threadNum = templateRankSize_ > 1 ? templateRankSize_ : 1;
    u32 notifyNum = threadNum - 1;
    for (u32 notifyIdx = 0; notifyIdx < notifyNum; notifyIdx++) {
        notifyIdxSubToMain.push_back(notifyIdx);
    }
}

} // namespace mc2_ops_hccl
