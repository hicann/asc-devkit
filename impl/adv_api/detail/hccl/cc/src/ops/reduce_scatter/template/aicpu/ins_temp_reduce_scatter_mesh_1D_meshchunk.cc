/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "ins_temp_reduce_scatter_mesh_1D_meshchunk.h"

namespace mc2_ops_hccl {
InsTempReduceScatterMesh1DMeshChunk::InsTempReduceScatterMesh1DMeshChunk(
    const OpParam& param, const u32 rankId, // 传通信域的rankId，userRank
    const std::vector<std::vector<u32>>& subCommRanks)
    : InsAlgTemplateBase(param, rankId, subCommRanks)
{}

InsTempReduceScatterMesh1DMeshChunk::~InsTempReduceScatterMesh1DMeshChunk() {}

HcclResult InsTempReduceScatterMesh1DMeshChunk::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    u32 threadNum = templateRankSize_ > 1 ? templateRankSize_ - 1 : 1;
    resourceRequest.slaveThreadNum = threadNum - 1;
    const u32 NOTIFY_NUM_PER_SLAVE_THREAD = 2;
    for (u32 index = 0; index < threadNum - 1; index++) {
        resourceRequest.notifyNumPerThread.push_back(NOTIFY_NUM_PER_SLAVE_THREAD);
    }
    resourceRequest.notifyNumOnMainThread = (threadNum - 1) * NOTIFY_NUM_PER_SLAVE_THREAD;
    std::vector<HcclChannelDesc> level0Channels;
    CHK_RET(CalcChannelRequestMesh1D(comm, param, topoInfo, subCommRanks_, level0Channels));
    resourceRequest.channels.push_back(level0Channels);
    HCCL_WARNING("Resource calculation is temporarily not performed in the template.");
    return HcclResult::HCCL_SUCCESS;
}

u64 InsTempReduceScatterMesh1DMeshChunk::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType)
{
    (void)inBuffType;
    (void)outBuffType;
    u64 scratchMultiple = templateRankSize_ - 1;
    return scratchMultiple;
}

HcclResult InsTempReduceScatterMesh1DMeshChunk::CalcSliceInfoVec(const u64& dataSize, RankSliceInfo& sliceInfoVec)
{
    std::vector<SliceInfo> tmp(subCommRanks_.size());
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
        HCCL_ERROR("[CollAlgFactory] Rank [%d], SliceInfo calculation error!", myRank_), HcclResult::HCCL_E_INTERNAL);

    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempReduceScatterMesh1DMeshChunk::KernelRun(
    const OpParam& param, const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource)
{
    threadNum_ = templateResource.threads.size();
    processSize_ = tempAlgParams.sliceSize;
    count_ = tempAlgParams.count;
    dataType_ = param.DataDes.dataType;
    dataTypeSize_ = DATATYPE_SIZE_TABLE[dataType_];
    CHK_RET(GetAlgRank(myRank_, subCommRanks_[0], rankIdx_));
    RankSliceInfo sliceInfoVec;
    CHK_RET(CalcSliceInfoVec(tempAlgParams.sliceSize, sliceInfoVec));
    HCCL_INFO("[InsTempReduceScatterMesh1DMeshChunk] Run Start");
    PreCopy(tempAlgParams, templateResource.threads);
    if (threadNum_ > 1) {
        std::vector<ThreadHandle> subThreads(templateResource.threads.begin() + 1, templateResource.threads.end());
        GetNotifyIdxMainToSub(notifyIdxMainToSub_);
        CHK_RET(PreSyncInterThreads(templateResource.threads[0], subThreads, notifyIdxMainToSub_));
    }

    CHK_RET(RunReduceScatter(templateResource.channels, templateResource.threads, tempAlgParams, sliceInfoVec));

    if (threadNum_ > 1) {
        std::vector<ThreadHandle> subThreads(templateResource.threads.begin() + 1, templateResource.threads.end());
        GetNotifyIdxSubToMain(notifyIdxSubToMain_);
        CHK_RET(PostSyncInterThreads(templateResource.threads[0], subThreads, notifyIdxSubToMain_));
    }
    PostCopy(tempAlgParams, templateResource.threads);
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempReduceScatterMesh1DMeshChunk::PreCopy(
    const TemplateDataParams& tempAlgParams, const std::vector<ThreadHandle>& threads) const
{
    HCCL_INFO("[InsTempReduceScatterMesh1DMeshChunk][PreCopy], copy from userIn to scratch");
    for (u32 repeatIdx = 0; repeatIdx < tempAlgParams.repeatNum; repeatIdx++) {
        DataSlice srcSlice = DataSlice(
            tempAlgParams.buffInfo.inputPtr,
            tempAlgParams.buffInfo.inBuffBaseOff + repeatIdx * tempAlgParams.inputRepeatStride +
                rankIdx_ * tempAlgParams.inputSliceStride,
            processSize_);
        DataSlice dstSlice =
            DataSlice(tempAlgParams.buffInfo.hcclBuff.addr, tempAlgParams.buffInfo.hcclBuffBaseOff, processSize_);
        CHK_RET(LocalCopy(threads[0], srcSlice, dstSlice));
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempReduceScatterMesh1DMeshChunk::RunReduceScatter(
    const std::map<u32, std::vector<ChannelInfo>>& channels, const std::vector<ThreadHandle>& threads,
    const TemplateDataParams& tempAlgParams, RankSliceInfo& sliceInfoVec)
{
    HCCL_INFO("[InsTempReduceScatterMesh1DMeshChunk][RunReduceScatter] myRank[%d]", myRank_);
    u32 myAlgRank = 0;
    CHK_RET(GetAlgRank(myRank_, subCommRanks_[0], myAlgRank));

    uint64_t sliceNum = templateRankSize_ - 1;
    uint64_t mySliceSize = sliceInfoVec[myAlgRank][0].size; // 获取本rank需要处理的数据量
    uint64_t mySliceCount = mySliceSize / DATATYPE_SIZE_TABLE[dataType_];
    // 数据切分为sliceNum块，当数据量不能均匀切分时，后面smallDataSliceNum个数据块比前面bigDataSliceNum个数据块每块少1个数据
    uint64_t bigDataSliceNum = mySliceCount % sliceNum;
    uint64_t bigDataSliceSize = (mySliceCount / sliceNum + 1) * DATATYPE_SIZE_TABLE[dataType_];
    uint64_t smallDataSliceNum = sliceNum - mySliceCount % sliceNum;
    uint64_t smallDataSliceSize = mySliceCount / sliceNum * DATATYPE_SIZE_TABLE[dataType_];

    std::vector<uint64_t> sliceSize;
    for (uint64_t i = 0; i < bigDataSliceNum; i++) {
        sliceSize.push_back(bigDataSliceSize);
    }
    for (uint64_t i = 0; i < smallDataSliceNum; i++) {
        sliceSize.push_back(smallDataSliceSize);
    }
    uint64_t sliceRecvBaseOffset = 0;
    uint16_t rankNum = 2;
    for (uint16_t i = 0; i < (templateRankSize_ - rankNum); i++) {
        sliceRecvBaseOffset += sliceSize[i];
    }
    for (u32 repeatIdx = 0; repeatIdx < tempAlgParams.repeatNum; repeatIdx++) {
        uint64_t sliceSendOffset_;
        uint64_t sliceRecvOffset_;
        DoMeshChunk(
            channels, threads, tempAlgParams, sliceSize, repeatIdx, myAlgRank, sliceSendOffset_, sliceRecvOffset_,
            sliceRecvBaseOffset);
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempReduceScatterMesh1DMeshChunk::DoMeshChunk(
    const std::map<u32, std::vector<ChannelInfo>>& channels, const std::vector<ThreadHandle>& threads,
    const TemplateDataParams& tempAlgParams, const std::vector<uint64_t>& sliceSize, const u32& repeatIdx,
    const u32& myAlgRank, uint64_t& sliceSendOffset_, uint64_t& sliceRecvOffset_, const uint64_t& sliceRecvBaseOffset)
{
    for (uint16_t stepIdx = 0; stepIdx < (templateRankSize_ - 1); stepIdx++) {
        sliceSendOffset_ = 0;
        sliceRecvOffset_ = sliceRecvBaseOffset;
        uint16_t rankNum = 2;
        uint16_t tempNum = 3;
        for (uint16_t i = 0; i < (templateRankSize_ - 1); i++) {
            uint16_t nextNum = stepIdx + i + 1;
            if (nextNum >= templateRankSize_) {
                nextNum += 1;
            }
            uint16_t nextRank = (myAlgRank + nextNum) % templateRankSize_;
            uint16_t frontNum = 2 * myAlgRank - nextRank + templateRankSize_;
            uint16_t frontRank = frontNum % templateRankSize_;
            u32 toRank = subCommRanks_[0][frontRank];
            const ChannelInfo& linkSend = channels.at(toRank)[0];
            const ChannelInfo& linkRecv = channels.at(toRank)[0];
            void* remoteCclBuffAddr = linkSend.remoteCclMem.addr;
            uint16_t queIdx;
            if (frontRank < myAlgRank) {
                queIdx = frontRank;
            } else {
                queIdx = frontRank - 1;
            }
            DataSlice rxSrcSlice = DataSlice(
                tempAlgParams.buffInfo.inputPtr,
                tempAlgParams.buffInfo.inBuffBaseOff + repeatIdx * tempAlgParams.inputRepeatStride +
                    myAlgRank * tempAlgParams.inputSliceStride + sliceRecvOffset_,
                sliceSize[i], sliceSize[i] / dataTypeSize_); // 接收源
            DataSlice rxDstSlice = DataSlice(
                tempAlgParams.buffInfo.hcclBuff.addr, tempAlgParams.buffInfo.hcclBuffBaseOff + sliceRecvOffset_,
                sliceSize[i], sliceSize[i] / dataTypeSize_); // 接收目标
            DataSlice txSrcSlice = DataSlice(
                tempAlgParams.buffInfo.inputPtr,
                tempAlgParams.buffInfo.inBuffBaseOff + repeatIdx * tempAlgParams.inputRepeatStride +
                    frontRank * tempAlgParams.inputSliceStride + sliceSendOffset_,
                sliceSize[i], sliceSize[i] / dataTypeSize_); // 发送源
            DataSlice txDstSlice = DataSlice(
                remoteCclBuffAddr, tempAlgParams.buffInfo.hcclBuffBaseOff + sliceSendOffset_, sliceSize[i],
                sliceSize[i] / dataTypeSize_); // 发送目标

            std::vector<DataSlice> txSrcSlices;
            std::vector<DataSlice> txDstSlices;
            std::vector<DataSlice> rxSrcSlices;
            std::vector<DataSlice> rxDstSlices;
            rxSrcSlices.push_back(rxSrcSlice);
            rxDstSlices.push_back(rxDstSlice);
            txSrcSlices.push_back(txSrcSlice);
            txDstSlices.push_back(txDstSlice);

            SendRecvReduceInfo sendRecvReduceInfo{
                {linkSend, linkRecv}, {{txSrcSlices, txDstSlices}, {rxSrcSlices, rxDstSlices}}, dataType_, reduceOp_};

            CHK_PRT_RET(
                SendRecvBatchWriteReduce(sendRecvReduceInfo, threads[queIdx]),
                HCCL_ERROR("[InsTempReduceScatterMesh1DMeshChunk] RunReduceScatter SendRecvReduce failed"),
                HcclResult::HCCL_E_INTERNAL);

            sliceSendOffset_ += sliceSize[i];
            if (templateRankSize_ > rankNum && i < (templateRankSize_ - rankNum)) {
                sliceRecvOffset_ -= sliceSize[templateRankSize_ - tempNum - i];
            }
        }
        if (threadNum_ > 1 && stepIdx < (templateRankSize_ - rankNum)) {
            std::vector<ThreadHandle> subThreads(threads.begin() + 1, threads.end());
            NotifyIdxMainToSubInMeshChunk(notifyIdxMainToSub_);
            CHK_RET(PreSyncInterThreads(threads[0], subThreads, notifyIdxMainToSub_));
            NotifyIdxSubToMainInMeshChunk(notifyIdxSubToMain_);
            CHK_RET(PostSyncInterThreads(threads[0], subThreads, notifyIdxSubToMain_));
        }
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempReduceScatterMesh1DMeshChunk::PostCopy(
    const TemplateDataParams& tempAlgParams, const std::vector<ThreadHandle>& threads)
{
    // 如果是单算子模式, 并且是最后一步算子，需要将数据从 scratch 拷贝到 userOut
    HCCL_INFO("[InsTempReduceScatterMesh1DMeshChunk][PostCopy], copy from scratch to userOut");
    u32 myAlgRank;
    CHK_RET(GetAlgRank(myRank_, subCommRanks_[0], myAlgRank));
    // 先把本卡的数据从input搬运到output
    for (u32 repeatIdx = 0; repeatIdx < tempAlgParams.repeatNum; repeatIdx++) {
        DataSlice myRankSlice =
            DataSlice(tempAlgParams.buffInfo.hcclBuff.addr, tempAlgParams.buffInfo.hcclBuffBaseOff, processSize_);
        DataSlice outputSlice =
            DataSlice(tempAlgParams.buffInfo.outputPtr, tempAlgParams.buffInfo.outBuffBaseOff, processSize_);
        CHK_RET(LocalCopy(threads[0], myRankSlice, outputSlice));
    }
    return HcclResult::HCCL_SUCCESS;
}

void InsTempReduceScatterMesh1DMeshChunk::GetNotifyIdxMainToSub(std::vector<u32>& notifyIdxMainToSub)
{
    notifyIdxMainToSub.clear();
    u32 threadNum = templateRankSize_ > 1 ? templateRankSize_ - 1 : 1;
    u32 slaveThreadNum = threadNum - 1;
    for (u32 slaveThreadIdx = 0; slaveThreadIdx < slaveThreadNum; slaveThreadIdx++) {
        notifyIdxMainToSub.push_back(0);
    }
}

void InsTempReduceScatterMesh1DMeshChunk::GetNotifyIdxSubToMain(std::vector<u32>& notifyIdxSubToMain)
{
    notifyIdxSubToMain.clear();
    u32 threadNum = templateRankSize_ > 1 ? templateRankSize_ - 1 : 1;
    u32 notifyNum = threadNum - 1;
    for (u32 notifyIdx = 0; notifyIdx < notifyNum; notifyIdx++) {
        notifyIdxSubToMain.push_back(notifyIdx);
    }
}

void InsTempReduceScatterMesh1DMeshChunk::NotifyIdxMainToSubInMeshChunk(std::vector<u32>& notifyIdxMainToSub)
{
    notifyIdxMainToSub.clear();
    u32 threadNum = templateRankSize_ > 1 ? templateRankSize_ - 1 : 1;
    u32 slaveThreadNum = threadNum - 1;
    for (u32 slaveThreadIdx = 0; slaveThreadIdx < slaveThreadNum; slaveThreadIdx++) {
        notifyIdxMainToSub.push_back(1);
    }
}

void InsTempReduceScatterMesh1DMeshChunk::NotifyIdxSubToMainInMeshChunk(std::vector<u32>& notifyIdxSubToMain)
{
    notifyIdxSubToMain.clear();
    u32 threadNum = templateRankSize_ > 1 ? templateRankSize_ - 1 : 1;
    u32 notifyNum = threadNum - 1;
    for (u32 notifyIdx = 0; notifyIdx < notifyNum; notifyIdx++) {
        notifyIdxSubToMain.push_back(notifyIdx + threadNum);
    }
}
} // namespace mc2_ops_hccl
