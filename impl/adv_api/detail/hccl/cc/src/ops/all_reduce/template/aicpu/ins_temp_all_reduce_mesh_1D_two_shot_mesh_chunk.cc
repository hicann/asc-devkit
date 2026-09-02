/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ins_temp_all_reduce_mesh_1D_two_shot_mesh_chunk.h"

namespace mc2_ops_hccl {

InsTempAllReduceMesh1DTwoShotMeshChunk::InsTempAllReduceMesh1DTwoShotMeshChunk(
    const OpParam& param, const u32 rankId, const std::vector<std::vector<u32>>& subCommRanks)
    : InsAlgTemplateBase(param, rankId, subCommRanks)
{}

InsTempAllReduceMesh1DTwoShotMeshChunk::~InsTempAllReduceMesh1DTwoShotMeshChunk() {}

u64 InsTempAllReduceMesh1DTwoShotMeshChunk::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType)
{
    (void)inBuffType;
    (void)outBuffType;
    u64 multiple = 2; // multiple=1且数据非均衡切分时，hcclBuffer会不足，因此用2
    HCCL_INFO("[InsTempAllReduceMesh1DTwoShotMeshChunk] Ccl Buffer multiple is [%u].", multiple);
    return multiple;
}

HcclResult InsTempAllReduceMesh1DTwoShotMeshChunk::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    u32 threadNum = templateRankSize_ > 1 ? templateRankSize_ : 1;
    resourceRequest.slaveThreadNum = threadNum - 1; // 主thread可以通过接口传入的stream来做转换
    const u32 NOTIFY_NUM_PER_SLAVE_THREAD = 3;
    resourceRequest.notifyNumPerThread.assign(resourceRequest.slaveThreadNum, NOTIFY_NUM_PER_SLAVE_THREAD);
    resourceRequest.notifyNumOnMainThread = (threadNum - 1) * NOTIFY_NUM_PER_SLAVE_THREAD;

    std::vector<HcclChannelDesc> level0Channels;
    CHK_RET(CalcChannelRequestMesh1D(comm, param, topoInfo, subCommRanks_, level0Channels));
    resourceRequest.channels.push_back(level0Channels);
    HCCL_WARNING("Resource calculation is temporarily not performed in the template.");
    HCCL_INFO(
        "[InsTempAllReduceMesh1DTwoShotMeshChunk] Calculate resource finished."
        "resource request: threadNum[%u], main thread notifyNum[%u], channelNum[%u]",
        resourceRequest.slaveThreadNum + 1, resourceRequest.notifyNumOnMainThread,
        resourceRequest.channels.at(0).size());
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllReduceMesh1DTwoShotMeshChunk::CalcSliceInfoVec(const u64& dataSize, RankSliceInfo& sliceInfoVec)
{
    CHK_PRT_RET(
        templateRankSize_ == 0, HCCL_ERROR("[InsTempAllReduceMesh1DTwoShotMeshChunk] rankSize is 0."),
        HcclResult::HCCL_E_INTERNAL);
    std::vector<SliceInfo> tmp(subCommRanks_.size());
    sliceInfoVec.resize(templateRankSize_, tmp);

    // 计算对齐后的块大小，并确保块大小是数据类型整数倍，尽量均匀分配
    u64 chunkCount = RoundUp(count_, templateRankSize_);
    u64 chunkSize = chunkCount * dataTypeSize_;

    u64 accumOff = 0;
    for (u32 rankIdx = 0; rankIdx < sliceInfoVec.size(); rankIdx++) {
        // 计算当前rank分配到的数据大小
        u64 currChunkSize = ((dataSize - accumOff) > chunkSize) ? chunkSize : (dataSize - accumOff);
        SliceInfo slice = {accumOff, currChunkSize};
        sliceInfoVec[rankIdx][0] = slice;
        accumOff += currChunkSize;
    }
    CHK_PRT_RET(
        (sliceInfoVec[templateRankSize_ - 1][0].offset + sliceInfoVec[templateRankSize_ - 1][0].size != dataSize),
        HCCL_ERROR(
            "[InsAllReduceCombExecutor] chunkSize:[%llu], Rank:[%d], SliceInfo calculation error!", chunkSize, myRank_),
        HcclResult::HCCL_E_INTERNAL);

    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllReduceMesh1DTwoShotMeshChunk::KernelRun(
    const OpParam& param, const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource)
{
    HCCL_INFO("[InsTempAllReduceMesh1DTwoShotMeshChunk] Run Start");
    threadNum_ = templateResource.threads.size();
    CHK_PRT_RET(
        threadNum_ != templateRankSize_,
        HCCL_ERROR(
            "[InsTempAllReduceMesh1DTwoShotMeshChunk][KernelRun] thread num is invalid, need[%u], actual[%u].",
            templateRankSize_, threadNum_),
        HcclResult::HCCL_E_INTERNAL);
    CHK_PRT_RET(
        subCommRanks_.size() == 0,
        HCCL_ERROR("[InsTempAllReduceMesh1DTwoShotMeshChunk][KernelRun] subCommRanks is empty."),
        HcclResult::HCCL_E_INTERNAL);
    CHK_PRT_RET(
        subCommRanks_[0].size() != templateRankSize_,
        HCCL_ERROR("[InsTempAllReduceMesh1DTwoShotMeshChunk][KernelRun] rank count is invalid in rank list.", myRank_),
        HcclResult::HCCL_E_INTERNAL);

    // 获取当前rank在rank列表中的序号
    CHK_RET(GetAlgRank(myRank_, subCommRanks_[0], myAlgRank_));

    processSize_ = tempAlgParams.sliceSize;
    count_ = tempAlgParams.count;
    dataType_ = param.DataDes.dataType;
    dataTypeSize_ = DATATYPE_SIZE_TABLE[dataType_];

    if (count_ == 0) {
        HCCL_WARNING("[InsTempAllReduceMesh1DTwoShotMeshChunk][KernelRun] data count is 0.");
        return HcclResult::HCCL_SUCCESS;
    }
    HCCL_DEBUG(
        "[InsTempAllReduceMesh1DTwoShotMeshChunk]Rank [%d], Input size:[%llu], BfSize:[%llu]", myRank_,
        tempAlgParams.sliceSize, tempAlgParams.buffInfo.hcclBuffSize);

    RankSliceInfo sliceInfoVec;
    CHK_RET(CalcSliceInfoVec(tempAlgParams.sliceSize, sliceInfoVec));
    CHK_PRT_RET(
        sliceInfoVec.size() != templateRankSize_,
        HCCL_ERROR(
            "[InsTempAllReduceMesh1DTwoShotMeshChunk][KernelRun] slice num[%u] is not equal to rank size[%u].",
            sliceInfoVec.size(), templateRankSize_),
        HcclResult::HCCL_E_INTERNAL);

    HCCL_INFO("[InsTempAllReduceMesh1DTwoShotMeshChunk][PreCopy] Rank [%d].", myRank_);
    CHK_RET(PreCopy(tempAlgParams, templateResource.threads, sliceInfoVec));
    CHK_RET(RunReduceScatter(templateResource.channels, templateResource.threads, tempAlgParams, sliceInfoVec));
    CHK_RET(RunAllgather(templateResource.channels, templateResource.threads, tempAlgParams, sliceInfoVec));

    HCCL_INFO(
        "[InsTempAllReduceMesh1DTwoShotMeshChunk][Run] InsTempAllReduceMesh1DTwoShotMeshChunk finished: rank[%d] end",
        myRank_);
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllReduceMesh1DTwoShotMeshChunk::RunReduceScatter(
    const std::map<u32, std::vector<ChannelInfo>>& channels, const std::vector<ThreadHandle>& threads,
    const TemplateDataParams& tempAlgParams, RankSliceInfo& sliceInfoVec)
{
    // 二级切片: 计算单个rank内一片数据再次分片成ranksize-1大小
    u64 sliceNum = templateRankSize_ - 1;
    std::vector<std::vector<u64>> sliceSize(templateRankSize_, std::vector<u64>(templateRankSize_ - 1));
    for (u32 rankId = 0; rankId < templateRankSize_; rankId++) {
        u64 rankIdSliceSize = sliceInfoVec[rankId][0].size;
        u64 rankIdSliceCount = rankIdSliceSize / dataTypeSize_;
        // 数据切分为sliceNum块，当数据量不能均匀切分时，后面smallDataSliceNum个数据块比前面bigDataSliceNum个数据块每块少1个数据
        // bigDataSlice - 容纳数据切分后的余数部分; smallDataSlice - 容纳数据切分后的整除部分
        u64 bigDataSliceNum = rankIdSliceCount % sliceNum;
        u64 bigDataSliceSize = (rankIdSliceCount / sliceNum + 1) * dataTypeSize_;
        u64 smallDataSliceNum = sliceNum - rankIdSliceCount % sliceNum;
        u64 smallDataSliceSize = rankIdSliceCount / sliceNum * dataTypeSize_;
        for (uint64_t i = 0; i < bigDataSliceNum; i++) {
            sliceSize[rankId][i] = bigDataSliceSize;
        }
        for (uint64_t i = 0; i < smallDataSliceNum; i++) {
            sliceSize[rankId][i + bigDataSliceNum] = smallDataSliceSize;
        }
    }

    if (threadNum_ > 1) {
        std::vector<ThreadHandle> subThreads(threads.begin() + 1, threads.end());
        GetNotifyIdxMainToSub(notifyIdxMainToSub_);
        CHK_RET(PreSyncInterThreads(threads[0], subThreads, notifyIdxMainToSub_));
    }

    for (u32 stepIndex = 0; stepIndex < (templateRankSize_ - 1); stepIndex++) {
        ReduceScatterMeshChunk(sliceInfoVec, channels, threads, tempAlgParams, sliceSize, stepIndex);
    }

    if (threadNum_ > 1) {
        std::vector<ThreadHandle> subThreads(threads.begin() + 1, threads.end());
        GetNotifyIdxSubToMain(notifyIdxSubToMain_);
        CHK_RET(PostSyncInterThreads(threads[0], subThreads, notifyIdxSubToMain_));
    }

    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllReduceMesh1DTwoShotMeshChunk::ReduceScatterMeshChunk(
    const RankSliceInfo& sliceInfoVec, const std::map<u32, std::vector<ChannelInfo>>& channels,
    const std::vector<ThreadHandle>& threads, const TemplateDataParams& tempAlgParams,
    const std::vector<std::vector<u64>>& sliceSize, const u32& stepIndex)
{
    u64 inBuffBaseOff = tempAlgParams.buffInfo.inBuffBaseOff;
    u64 cclBuffBaseOff = tempAlgParams.buffInfo.hcclBuffBaseOff;
    for (u32 chunkIndex = 0; chunkIndex < (templateRankSize_ - 1); chunkIndex++) {
        u64 sliceRxOffset_ = 0;
        u64 sliceTxOffset_ = 0;
        u32 nextNum = stepIndex + chunkIndex + 1;
        if (nextNum >= templateRankSize_) {
            nextNum += 1;
        }
        u32 nextRank = (myAlgRank_ + nextNum) % templateRankSize_;
        u32 preNum = 2 * myAlgRank_ + templateRankSize_ - nextRank;
        u32 preRank = preNum % templateRankSize_;
        u32 fromRank = subCommRanks_[0][nextRank];
        u32 toRank = subCommRanks_[0][preRank];

        u32 queIdx = (preRank < myAlgRank_) ? preRank : preRank - 1;
        for (u32 m = 0; m < chunkIndex; m++) {
            sliceRxOffset_ += sliceSize[fromRank][m];
            sliceTxOffset_ += sliceSize[toRank][m];
        }

        const ChannelInfo& linkRecv = channels.at(toRank)[0]; // linkRecv - 从fromRank接收的链路
        const ChannelInfo& linkSend = channels.at(toRank)[0]; // linkSend - 向toRank发送的链路

        std::vector<DataSlice> rxSrcSlices;
        std::vector<DataSlice> rxDstSlices;
        void* rxCclBuffAddr = linkRecv.remoteCclMem.addr;
        DataSlice rxSrcSlice = DataSlice(
            tempAlgParams.buffInfo.inputPtr, inBuffBaseOff + sliceInfoVec[myAlgRank_][0].offset + sliceRxOffset_,
            sliceSize[fromRank][chunkIndex], sliceSize[fromRank][chunkIndex] / dataTypeSize_); // 接收源
        DataSlice rxDstSlice = DataSlice(
            rxCclBuffAddr, cclBuffBaseOff + sliceInfoVec[myAlgRank_][0].offset + sliceRxOffset_,
            sliceSize[fromRank][chunkIndex], sliceSize[fromRank][chunkIndex] / dataTypeSize_); // 接收目标
        rxSrcSlices.push_back(rxSrcSlice);
        rxDstSlices.push_back(rxDstSlice);

        std::vector<DataSlice> txSrcSlices;
        std::vector<DataSlice> txDstSlices;
        void* txCclBuffAddr = linkSend.remoteCclMem.addr;
        DataSlice txSrcSlice = DataSlice(
            tempAlgParams.buffInfo.inputPtr, inBuffBaseOff + sliceInfoVec[toRank][0].offset + sliceTxOffset_,
            sliceSize[toRank][chunkIndex], sliceSize[toRank][chunkIndex] / dataTypeSize_); // 发送源
        DataSlice txDstSlice = DataSlice(
            txCclBuffAddr, cclBuffBaseOff + sliceInfoVec[toRank][0].offset + sliceTxOffset_,
            sliceSize[toRank][chunkIndex], sliceSize[toRank][chunkIndex] / dataTypeSize_); // 发送目标
        txSrcSlices.push_back(txSrcSlice);
        txDstSlices.push_back(txDstSlice);

        SendRecvReduceInfo sendRecvReduceInfo{
            {linkSend, linkRecv}, {{txSrcSlices, txDstSlices}, {rxSrcSlices, rxDstSlices}}, dataType_, reduceOp_};
        CHK_PRT_RET(
            SendRecvBatchWriteReduce(sendRecvReduceInfo, threads[queIdx]),
            HCCL_ERROR("[InsTempAllReduceMesh1DTwoShotMeshChunk] RunReduceScatter SendRecvWriteReduce failed"),
            HcclResult::HCCL_E_INTERNAL);
    }
    u32 rankNum = 2;
    if (stepIndex < (templateRankSize_ - rankNum)) {
        if (threadNum_ > 1) {
            std::vector<ThreadHandle> subThreads(threads.begin() + 1, threads.end());
            NotifyIdxSubToMainInRSMeshChunk(notifyIdxSubToMain_);
            CHK_RET(PostSyncInterThreads(threads[0], subThreads, notifyIdxSubToMain_));
            NotifyIdxMainToSubInRSMeshChunk(notifyIdxMainToSub_);
            CHK_RET(PreSyncInterThreads(threads[0], subThreads, notifyIdxMainToSub_));
        }
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllReduceMesh1DTwoShotMeshChunk::RunAllgather(
    const std::map<u32, std::vector<ChannelInfo>>& channels, const std::vector<ThreadHandle>& threads,
    const TemplateDataParams& tempAlgParams, RankSliceInfo& sliceInfoVec)
{
    // sync:前同步
    if (threadNum_ > 1) {
        std::vector<ThreadHandle> subThreads(threads.begin() + 1, threads.end());
        NotifyIdxMainToSubInAG(notifyIdxMainToSub_);
        CHK_RET(PreSyncInterThreads(threads[0], subThreads, notifyIdxMainToSub_));
    }

    // allgather
    for (u32 rankId = 0; rankId < templateRankSize_; rankId++) {
        if (u32(myAlgRank_) == rankId) {
            DataSlice rsrcSlice = DataSlice(
                tempAlgParams.buffInfo.hcclBuff.addr,
                tempAlgParams.buffInfo.hcclBuffBaseOff + sliceInfoVec[rankId][0].offset, sliceInfoVec[rankId][0].size,
                sliceInfoVec[rankId][0].size / dataTypeSize_);
            DataSlice rdestSlice = DataSlice(
                tempAlgParams.buffInfo.outputPtr,
                tempAlgParams.buffInfo.outBuffBaseOff + sliceInfoVec[rankId][0].offset, sliceInfoVec[rankId][0].size,
                sliceInfoVec[rankId][0].size / dataTypeSize_);

            if (sliceInfoVec[rankId][0].size != 0) {
                // copy本端计算的结果到user output
                CHK_RET(static_cast<HcclResult>(LocalCopy(threads[0], rsrcSlice, rdestSlice)));
            }
        } else {
            u32 queIdx = (rankId < myAlgRank_) ? rankId : rankId - 1;
            const ChannelInfo& linkSendRecv = channels.at(rankId)[0];
            // 接收, 未过滤size为0的情况
            void* rxCclBuffAddr = linkSendRecv.remoteCclMem.addr;
            DataSlice rsrcSlice = DataSlice(
                rxCclBuffAddr, tempAlgParams.buffInfo.hcclBuffBaseOff + sliceInfoVec[rankId][0].offset,
                sliceInfoVec[rankId][0].size, sliceInfoVec[rankId][0].size / dataTypeSize_);
            DataSlice rdestSlice = DataSlice(
                tempAlgParams.buffInfo.outputPtr,
                tempAlgParams.buffInfo.outBuffBaseOff + sliceInfoVec[rankId][0].offset, sliceInfoVec[rankId][0].size,
                sliceInfoVec[rankId][0].size / dataTypeSize_);
            std::vector<DataSlice> recvSrcSlices{rsrcSlice};
            std::vector<DataSlice> recvDestSlices{rdestSlice};

            // 发送，未过滤size为0的情况
            void* txCclBuffAddr = linkSendRecv.remoteCclMem.addr;
            DataSlice ssrcSlice = DataSlice(
                rxCclBuffAddr, tempAlgParams.buffInfo.hcclBuffBaseOff + sliceInfoVec[myAlgRank_][0].offset,
                sliceInfoVec[myAlgRank_][0].size, sliceInfoVec[myAlgRank_][0].size / dataTypeSize_);
            DataSlice sdestSlice = DataSlice(
                tempAlgParams.buffInfo.outputPtr,
                tempAlgParams.buffInfo.outBuffBaseOff + sliceInfoVec[myAlgRank_][0].offset,
                sliceInfoVec[myAlgRank_][0].size, sliceInfoVec[myAlgRank_][0].size / dataTypeSize_);
            std::vector<DataSlice> sendSrcSlices{ssrcSlice};
            std::vector<DataSlice> sendDestSlices{sdestSlice};

            TxRxChannels sendRecvChannels(linkSendRecv, linkSendRecv); // 收发双向用同一个Channel
            TxRxSlicesList sendRecvSlicesList({sendSrcSlices, sendDestSlices}, {recvSrcSlices, recvDestSlices});
            SendRecvInfo sendRecvInfo(sendRecvChannels, sendRecvSlicesList);
            CHK_PRT_RET(
                SendRecvRead(sendRecvInfo, threads[queIdx]),
                HCCL_ERROR("[InsTempAllReduceMesh1DTwoShotMeshChunk][RunAllgather] RunAllReduce AllGather failed"),
                HcclResult::HCCL_E_INTERNAL);
        }
    }
    if (threadNum_ > 1) {
        std::vector<ThreadHandle> subThreads(threads.begin() + 1, threads.end());
        NotifyIdxSubToMainInAG(notifyIdxSubToMain_);
        CHK_RET(PostSyncInterThreads(threads[0], subThreads, notifyIdxSubToMain_));
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllReduceMesh1DTwoShotMeshChunk::PreCopy(
    const TemplateDataParams& tempAlgParams, const std::vector<ThreadHandle>& threads,
    const RankSliceInfo& sliceInfoVec)
{
    HCCL_INFO("[InsTempAllReduceMesh1DTwoShotMeshChunk][PreCopy], copy from userIn to scratch");
    for (u32 rankId = 0; rankId < subCommRanks_[0].size(); rankId++) {
        DataSlice localsrcSlice = DataSlice(
            tempAlgParams.buffInfo.inputPtr, sliceInfoVec[rankId][0].offset + tempAlgParams.buffInfo.inBuffBaseOff,
            sliceInfoVec[rankId][0].size, sliceInfoVec[rankId][0].size / dataTypeSize_);
        DataSlice localdestSlice = DataSlice(
            tempAlgParams.buffInfo.hcclBuff.addr,
            sliceInfoVec[rankId][0].offset + tempAlgParams.buffInfo.hcclBuffBaseOff, sliceInfoVec[rankId][0].size,
            sliceInfoVec[rankId][0].size / dataTypeSize_);

        if (rankId == u32(myAlgRank_)) {
            // 本地rank对应一片直接拷贝到scratch对应位置
            CHK_RET(static_cast<HcclResult>(LocalCopy(threads[0], localsrcSlice, localdestSlice)));
        }
    }
    return HcclResult::HCCL_SUCCESS;
}

void InsTempAllReduceMesh1DTwoShotMeshChunk::GetNotifyIdxMainToSub(std::vector<u32>& notifyIdxMainToSub)
{
    notifyIdxMainToSub.clear();
    u32 threadNum = templateRankSize_ > 1 ? templateRankSize_ : 1;
    u32 slaveThreadNum = threadNum - 1;
    for (u32 slaveThreadIdx = 0; slaveThreadIdx < slaveThreadNum; slaveThreadIdx++) {
        notifyIdxMainToSub.push_back(0);
    }
}

void InsTempAllReduceMesh1DTwoShotMeshChunk::GetNotifyIdxSubToMain(std::vector<u32>& notifyIdxSubToMain)
{
    notifyIdxSubToMain.clear();
    u32 threadNum = templateRankSize_ > 1 ? templateRankSize_ : 1;
    u32 notifyNum = threadNum - 1;
    for (u32 notifyIdx = 0; notifyIdx < notifyNum; notifyIdx++) {
        notifyIdxSubToMain.push_back(notifyIdx);
    }
}

void InsTempAllReduceMesh1DTwoShotMeshChunk::NotifyIdxMainToSubInRSMeshChunk(std::vector<u32>& notifyIdxMainToSub)
{
    notifyIdxMainToSub.clear();
    u32 threadNum = templateRankSize_ > 1 ? templateRankSize_ : 1;
    u32 slaveThreadNum = threadNum - 1;
    for (u32 slaveThreadIdx = 0; slaveThreadIdx < slaveThreadNum; slaveThreadIdx++) {
        notifyIdxMainToSub.push_back(1);
    }
}

void InsTempAllReduceMesh1DTwoShotMeshChunk::NotifyIdxSubToMainInRSMeshChunk(std::vector<u32>& notifyIdxSubToMain)
{
    notifyIdxSubToMain.clear();
    u32 threadNum = templateRankSize_ > 1 ? templateRankSize_ : 1;
    u32 notifyNum = threadNum - 1;
    for (u32 notifyIdx = 0; notifyIdx < notifyNum; notifyIdx++) {
        notifyIdxSubToMain.push_back(notifyIdx + notifyNum);
    }
}

void InsTempAllReduceMesh1DTwoShotMeshChunk::NotifyIdxMainToSubInAG(std::vector<u32>& notifyIdxMainToSub)
{
    notifyIdxMainToSub.clear();
    u32 threadNum = templateRankSize_ > 1 ? templateRankSize_ : 1;
    u32 slaveThreadNum = threadNum - 1;
    u32 AGThreadIndex = 2;
    for (u32 slaveThreadIdx = 0; slaveThreadIdx < slaveThreadNum; slaveThreadIdx++) {
        notifyIdxMainToSub.push_back(AGThreadIndex);
    }
}

void InsTempAllReduceMesh1DTwoShotMeshChunk::NotifyIdxSubToMainInAG(std::vector<u32>& notifyIdxSubToMain)
{
    notifyIdxSubToMain.clear();
    u32 threadNum = templateRankSize_ > 1 ? templateRankSize_ : 1;
    u32 notifyNum = threadNum - 1;
    u32 AGThreadNum = 3;
    for (u32 notifyIdx = 0; notifyIdx < notifyNum; notifyIdx++) {
        notifyIdxSubToMain.push_back(AGThreadNum * (threadNum - 1) - notifyIdx - 1);
    }
}

} // namespace mc2_ops_hccl
