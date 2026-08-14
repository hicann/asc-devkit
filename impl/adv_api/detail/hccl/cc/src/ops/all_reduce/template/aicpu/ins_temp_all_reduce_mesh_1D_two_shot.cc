/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ins_temp_all_reduce_mesh_1D_two_shot.h"

namespace mc2_ops_hccl {

InsTempAllReduceMesh1DTwoShot::InsTempAllReduceMesh1DTwoShot(
    const OpParam& param, const u32 rankId, const std::vector<std::vector<u32>>& subCommRanks)
    : InsAlgTemplateBase(param, rankId, subCommRanks)
{}

InsTempAllReduceMesh1DTwoShot::~InsTempAllReduceMesh1DTwoShot() {}

u64 InsTempAllReduceMesh1DTwoShot::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType)
{
    (void)inBuffType;
    (void)outBuffType;
    u64 multiple = 2; // multiple=1且数据非均衡切分时，hcclBuffer会不足，因此用2
    HCCL_INFO("[InsTempAllReduceMesh1DTwoShot] Ccl Buffer multiple is [%llu].", multiple);
    return multiple;
}

HcclResult InsTempAllReduceMesh1DTwoShot::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    CHK_RET(GetRes(resourceRequest));

    std::vector<HcclChannelDesc> channelReq;
    CHK_RET(CalcChannelRequestMesh1D(comm, param, topoInfo, subCommRanks_, channelReq));
    resourceRequest.channels.push_back(channelReq);

    HCCL_INFO(
        "[InsTempAllReduceMesh1DTwoShot] Calculate resource finished."
        "resource request: threadNum[%u], main thread notifyNum[%u], channelNum[%u]",
        resourceRequest.slaveThreadNum + 1, resourceRequest.notifyNumOnMainThread,
        resourceRequest.channels.at(0).size());
    return HCCL_SUCCESS;
}

HcclResult InsTempAllReduceMesh1DTwoShot::GetRes(AlgResourceRequest& resourceRequest) const
{
    resourceRequest.slaveThreadNum = templateRankSize_ - 1;

    resourceRequest.notifyNumPerThread.assign(resourceRequest.slaveThreadNum, 1);
    resourceRequest.notifyNumOnMainThread = resourceRequest.slaveThreadNum;

    return HCCL_SUCCESS;
}

u64 InsTempAllReduceMesh1DTwoShot::GetThreadNum() const
{
    // 需要rankSize个线程并行
    return templateRankSize_;
}

void InsTempAllReduceMesh1DTwoShot::GetNotifyIdxMainToSub(std::vector<u32>& notifyIdxMainToSub)
{
    notifyIdxMainToSub.clear();
    u32 slaveThreadNum = threadNum_ - 1;
    notifyIdxMainToSub.assign(slaveThreadNum, 0); // 从线程全部用第0个Notify等待主线程的同步信号
}

void InsTempAllReduceMesh1DTwoShot::GetNotifyIdxSubToMain(std::vector<u32>& notifyIdxSubToMain)
{
    notifyIdxSubToMain.clear();
    u32 slaveThreadNum = threadNum_ - 1;
    for (u32 notifyIdx = 0; notifyIdx < slaveThreadNum; ++notifyIdx) {
        notifyIdxSubToMain.emplace_back(notifyIdx);
    }
}

HcclResult InsTempAllReduceMesh1DTwoShot::KernelRun(
    const OpParam& param, const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource)
{
    HCCL_INFO("[InsTempAllReduceMesh1DTwoShot] KernelRun Start.");

    threadNum_ = templateResource.threads.size();
    CHK_PRT_RET(
        threadNum_ != templateRankSize_,
        HCCL_ERROR(
            "[InsTempAllReduceMesh1DTwoShot][KernelRun] thread num is invalid, need[%u], actual[%u].",
            templateRankSize_, threadNum_),
        HcclResult::HCCL_E_INTERNAL);

    CHK_PRT_RET(
        subCommRanks_.size() == 0, HCCL_ERROR("[InsTempAllReduceMesh1DTwoShot][KernelRun] subCommRanks is empty."),
        HcclResult::HCCL_E_INTERNAL);
    rankList_ = subCommRanks_.at(0);

    CHK_PRT_RET(
        rankList_.size() != templateRankSize_,
        HCCL_ERROR("[InsTempAllReduceMesh1DTwoShot][KernelRun] rank[%u] count is invalid in rank list.", myRank_),
        HcclResult::HCCL_E_INTERNAL);

    // 获取当前rank在rank列表中的序号
    CHK_RET(GetAlgRank(myRank_, rankList_, myRankIdx_));

    processSize_ = tempAlgParams.sliceSize;
    count_ = tempAlgParams.count;
    dataType_ = param.DataDes.dataType;
    dataTypeSize_ = DATATYPE_SIZE_TABLE[dataType_];
    needAicpuReduce_ =
        dataType_ == HcclDataType::HCCL_DATA_TYPE_INT64 || dataType_ == HcclDataType::HCCL_DATA_TYPE_UINT64 ||
        dataType_ == HcclDataType::HCCL_DATA_TYPE_FP64 || param.reduceType == HcclReduceOp::HCCL_REDUCE_PROD;

    if (count_ == 0) {
        HCCL_WARNING("[InsTempAllReduceMesh1DTwoShot][KernelRun] data count is 0.");
        return HcclResult::HCCL_SUCCESS;
    }

    // 数据切片
    CHK_RET(SplitData());
    CHK_PRT_RET(
        sliceInfoList_.size() != templateRankSize_,
        HCCL_ERROR(
            "[InsTempAllReduceMesh1DTwoShot][KernelRun] slice num[%u] is not equal to rank size[%u].",
            sliceInfoList_.size(), templateRankSize_),
        HcclResult::HCCL_E_INTERNAL);

    // TwoShot算法，第一步ReduceScatter
    CHK_RET(RunReduceScatter(param, tempAlgParams, templateResource.channels, templateResource.threads));

    // TwoShot算法，第二步AllGather
    CHK_RET(RunAllGather(tempAlgParams, templateResource.channels, templateResource.threads));

    HCCL_INFO("[InsTempAllReduceMesh1DTwoShot] KernelRun finished.");

    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllReduceMesh1DTwoShot::SplitData()
{
    u32 sliceNum = templateRankSize_;
    sliceInfoList_.clear();
    sliceInfoList_.reserve(sliceNum);

    u64 sliceCount = RoundUp(count_, sliceNum);
    u64 sliceSize = sliceCount * dataTypeSize_;

    u64 offsetCount = 0;
    u64 offsetSize = 0;
    for (u32 sliceIdx = 0; sliceIdx < sliceNum; ++sliceIdx) {
        if (count_ - offsetCount > sliceCount) {
            sliceInfoList_.emplace_back(offsetSize, sliceSize, sliceCount);
            offsetCount += sliceCount;
            offsetSize = offsetCount * dataTypeSize_;
        } else {
            u64 curSliceCount = count_ - offsetCount;
            u64 curSliceSize = curSliceCount * dataTypeSize_;
            sliceInfoList_.emplace_back(offsetSize, curSliceSize, curSliceCount);
            offsetCount = count_;
            offsetSize = offsetCount * dataTypeSize_;
        }
    }

    for (u32 i = 0; i < sliceInfoList_.size(); ++i) {
        HCCL_DEBUG(
            "[InsTempAllReduceMesh1DTwoShot] SliceInfo: offset[%llu] size[%llu] count[%llu]",
            sliceInfoList_.at(i).offset, sliceInfoList_.at(i).size, sliceInfoList_.at(i).count);
    }

    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllReduceMesh1DTwoShot::RunReduceScatter(
    const OpParam& param, const TemplateDataParams& tempAlgParams,
    const std::map<u32, std::vector<ChannelInfo>>& channels, const std::vector<ThreadHandle>& threads)
{
    // 主线程向从线程发送启动信号
    PreSync(threads);

    CHK_RET(ScatterData(tempAlgParams, channels, threads));

    // 从线程往主线程返回结束信号
    PostSync(threads);

    // 增加thread synchronize以支持64类数据类型
    if (needAicpuReduce_) {
        // 启动任务并等待所有threads任务执行完成
        CHK_RET(static_cast<HcclResult>(HcommBatchModeEnd(param.algTag)));
        CHK_RET(static_cast<HcclResult>(HcommBatchModeStart(param.algTag)));
        for (const auto& thread : threads) {
            CHK_RET(static_cast<HcclResult>(HcommThreadJoin(thread, CUSTOM_TIMEOUT)));
        }
    }

    // 将数据reduce到第0片数据的位置
    CHK_RET(ReduceData(tempAlgParams, threads));

    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllReduceMesh1DTwoShot::ScatterData(
    const TemplateDataParams& tempAlgParams, const std::map<u32, std::vector<ChannelInfo>>& channels,
    const std::vector<ThreadHandle>& threads)
{
    void* localInBuffPtr = tempAlgParams.buffInfo.inputPtr;
    void* localHcclBuffPtr = tempAlgParams.buffInfo.hcclBuff.addr;
    u64 inBuffBaseOffset = tempAlgParams.buffInfo.inBuffBaseOff;
    u64 hcclBuffBaseOffset = tempAlgParams.buffInfo.hcclBuffBaseOff;

    u64 recvSize = sliceInfoList_.at(myRankIdx_).size;
    u64 recvCount = sliceInfoList_.at(myRankIdx_).count;
    u64 recvOffset = sliceInfoList_.at(myRankIdx_).offset;

    for (u32 remoteIdx = 0; remoteIdx < templateRankSize_; ++remoteIdx) {
        u64 sendSize = sliceInfoList_.at(remoteIdx).size;
        u64 sendCount = sliceInfoList_.at(remoteIdx).count;
        u64 sendOffset = sliceInfoList_.at(remoteIdx).offset;

        // 发送和接收数据量都为0的时候，既不发送也不接收
        if (sendSize == 0 && recvSize == 0) {
            continue;
        }

        // 数据片序号等于自身rank序号时，本地拷贝数据
        if (remoteIdx == myRankIdx_) {
            DataSlice copySrcSlice(localInBuffPtr, inBuffBaseOffset + sendOffset, sendSize, sendCount);
            DataSlice copyDstSlice(localHcclBuffPtr, hcclBuffBaseOffset + remoteIdx * recvSize, recvSize, recvCount);
            CHK_PRT_RET(
                LocalCopy(threads.at(remoteIdx), copySrcSlice, copyDstSlice),
                HCCL_ERROR("[InsTempAllReduceMesh1DTwoShot][ScatterData] LocalCopy failed."),
                HcclResult::HCCL_E_INTERNAL);
            continue;
        }

        // 数据片序号不等于自身rank序号时，跨rank发送和接收数据
        u32 remoteRank = rankList_.at(remoteIdx);
        const ChannelInfo& sendRecvChannel = channels.at(remoteRank).at(0);

        void* remoteInBuffPtr = sendRecvChannel.remoteCclMem.addr;
        void* remoteHcclBuffPtr = sendRecvChannel.remoteCclMem.addr;

        DataSlice sendSrcSlice(localInBuffPtr, inBuffBaseOffset + sendOffset, sendSize, sendCount);
        DataSlice sendDstSlice(remoteHcclBuffPtr, hcclBuffBaseOffset + myRankIdx_ * sendSize, sendSize, sendCount);
        std::vector<DataSlice> sendSrcSlicesList{sendSrcSlice};
        std::vector<DataSlice> sendDstSlicesList{sendDstSlice};

        DataSlice recvSrcSlice(remoteInBuffPtr, inBuffBaseOffset + recvOffset, recvSize, recvCount);
        DataSlice recvDstSlice(localHcclBuffPtr, hcclBuffBaseOffset + remoteIdx * recvSize, recvSize, recvCount);
        std::vector<DataSlice> recvSrcSlicesList{recvSrcSlice};
        std::vector<DataSlice> recvDstSlicesList{recvDstSlice};

        if (sendSize == 0) {
            // 发送数据片为0时，只接收数据
            SlicesList recvSlicesList(recvSrcSlicesList, recvDstSlicesList);
            DataInfo recvInfo(sendRecvChannel, recvSlicesList);
            CHK_PRT_RET(
                RecvWrite(recvInfo, threads.at(remoteIdx)),
                HCCL_ERROR("[InsTempAllReduceMesh1DTwoShot][ScatterData] Recv failed."), HcclResult::HCCL_E_INTERNAL);
        } else if (recvSize == 0) {
            // 接收数据片为0时，只发送数据
            SlicesList sendSlicesList(sendSrcSlicesList, sendDstSlicesList);
            DataInfo sendInfo(sendRecvChannel, sendSlicesList, dataType_);
            CHK_PRT_RET(
                SendBatchWrite(sendInfo, threads.at(remoteIdx)),
                HCCL_ERROR("[InsTempAllReduceMesh1DTwoShot][ScatterData] Send failed."), HcclResult::HCCL_E_INTERNAL);
        } else {
            TxRxChannels sendRecvChannels(sendRecvChannel, sendRecvChannel); // 收发双向用同一个Channel
            TxRxSlicesList sendRecvSlicesList(
                {sendSrcSlicesList, sendDstSlicesList}, {recvSrcSlicesList, recvDstSlicesList});
            SendRecvInfo sendRecvInfo(sendRecvChannels, sendRecvSlicesList, dataType_);
            CHK_PRT_RET(
                SendRecvBatchWrite(sendRecvInfo, threads.at(remoteIdx)),
                HCCL_ERROR("[InsTempAllReduceMesh1DTwoShot][ScatterData] SendRecv failed."),
                HcclResult::HCCL_E_INTERNAL);
        }
    }

    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllReduceMesh1DTwoShot::ReduceData(
    const TemplateDataParams& tempAlgParams, const std::vector<ThreadHandle>& threads)
{
    void* localHcclBuffPtr = tempAlgParams.buffInfo.hcclBuff.addr;
    u64 hcclBuffBaseOffset = tempAlgParams.buffInfo.hcclBuffBaseOff;

    u64 sliceSize = sliceInfoList_.at(myRankIdx_).size;
    u64 sliceCount = sliceInfoList_.at(myRankIdx_).count;

    // 数据量为0的数据片无需Reduce
    if (sliceSize == 0) {
        return HcclResult::HCCL_SUCCESS;
    }

    // 每片数据Reduce到第0片数据的位置
    DataSlice reduceDstSlice(localHcclBuffPtr, hcclBuffBaseOffset, sliceSize, sliceCount);

    for (u32 sliceIdx = 1; sliceIdx < templateRankSize_; ++sliceIdx) {
        DataSlice reduceSrcSlice(localHcclBuffPtr, hcclBuffBaseOffset + sliceIdx * sliceSize, sliceSize, sliceCount);
        // 确定性计算，顺序Reduce
        CHK_PRT_RET(
            LocalReduce(threads[0], reduceSrcSlice, reduceDstSlice, dataType_, reduceOp_),
            HCCL_ERROR("[InsTempAllReduceMesh1DTwoShot][ReduceData] LocalReduce failed"), HcclResult::HCCL_E_INTERNAL);
    }

    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllReduceMesh1DTwoShot::RunAllGather(
    const TemplateDataParams& tempAlgParams, const std::map<u32, std::vector<ChannelInfo>>& channels,
    const std::vector<ThreadHandle>& threads)
{
    // 主线程向从线程发送启动信号
    PreSync(threads);

    CHK_RET(GatherData(tempAlgParams, channels, threads));

    // 从线程往主线程返回结束信号
    PostSync(threads);

    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllReduceMesh1DTwoShot::GatherData(
    const TemplateDataParams& tempAlgParams, const std::map<u32, std::vector<ChannelInfo>>& channels,
    const std::vector<ThreadHandle>& threads)
{
    void* localHcclBuffPtr = tempAlgParams.buffInfo.hcclBuff.addr;
    void* localOutBuffPtr = tempAlgParams.buffInfo.outputPtr;
    u64 hcclBuffBaseOffset = tempAlgParams.buffInfo.hcclBuffBaseOff;
    u64 outBuffBaseOffset = tempAlgParams.buffInfo.outBuffBaseOff;

    u64 sendSize = sliceInfoList_.at(myRankIdx_).size;
    u64 sendCount = sliceInfoList_.at(myRankIdx_).count;
    u64 sendOffset = sliceInfoList_.at(myRankIdx_).offset;

    for (u32 remoteIdx = 0; remoteIdx < sliceInfoList_.size(); ++remoteIdx) {
        u64 recvSize = sliceInfoList_.at(remoteIdx).size;
        u64 recvCount = sliceInfoList_.at(remoteIdx).count;
        u64 recvOffset = sliceInfoList_.at(remoteIdx).offset;

        // 发送和接收数据量都为0的时候，既不发送也不接收
        if (sendSize == 0 && recvSize == 0) {
            continue;
        }

        // 数据片序号等于自身rank序号时，本地拷贝数据
        if (remoteIdx == myRankIdx_) {
            DataSlice copySrcSlice(localHcclBuffPtr, hcclBuffBaseOffset, sendSize, sendCount);
            DataSlice copyDstSlice(localOutBuffPtr, outBuffBaseOffset + recvOffset, recvSize, recvCount);
            CHK_PRT_RET(
                LocalCopy(threads.at(remoteIdx), copySrcSlice, copyDstSlice),
                HCCL_ERROR("[InsTempAllReduceMesh1DTwoShot][ScatterData] LocalCopy failed."),
                HcclResult::HCCL_E_INTERNAL);
            continue;
        }

        // 数据片序号不等于自身rank序号时，跨rank发送和接收数据
        u32 remoteRank = rankList_.at(remoteIdx);
        const ChannelInfo& sendRecvChannel = channels.at(remoteRank).at(0);

        void* remoteHcclBuffPtr = sendRecvChannel.remoteCclMem.addr;

        DataSlice sendSrcSlice(localHcclBuffPtr, hcclBuffBaseOffset, sendSize, sendCount);
        DataSlice sendDstSlice(remoteHcclBuffPtr, outBuffBaseOffset + sendOffset, sendSize, sendCount);
        std::vector<DataSlice> sendSrcSlicesList{sendSrcSlice};
        std::vector<DataSlice> sendDstSlicesList{sendDstSlice};

        DataSlice recvSrcSlice(remoteHcclBuffPtr, hcclBuffBaseOffset, recvSize, recvCount);
        DataSlice recvDstSlice(localOutBuffPtr, outBuffBaseOffset + recvOffset, recvSize, recvCount);
        std::vector<DataSlice> recvSrcSlicesList{recvSrcSlice};
        std::vector<DataSlice> recvDstSlicesList{recvDstSlice};

        if (sendSize == 0) {
            // 发送数据片为0时，只接收数据
            SlicesList recvSlicesList(recvSrcSlicesList, recvDstSlicesList);
            DataInfo recvInfo(sendRecvChannel, recvSlicesList, dataType_);
            CHK_PRT_RET(
                RecvBatchRead(recvInfo, threads.at(remoteIdx)),
                HCCL_ERROR("[InsTempAllReduceMesh1DTwoShot][ScatterData] Recv failed."), HcclResult::HCCL_E_INTERNAL);
        } else if (recvSize == 0) {
            // 接收数据片为0时，只发送数据
            SlicesList sendSlicesList(sendSrcSlicesList, sendDstSlicesList);
            DataInfo sendInfo(sendRecvChannel, sendSlicesList, dataType_);
            CHK_PRT_RET(
                SendRead(sendInfo, threads.at(remoteIdx)),
                HCCL_ERROR("[InsTempAllReduceMesh1DTwoShot][ScatterData] Send failed."), HcclResult::HCCL_E_INTERNAL);
        } else {
            TxRxChannels sendRecvChannels(sendRecvChannel, sendRecvChannel); // 收发双向用同一个Channel
            TxRxSlicesList sendRecvSlicesList(
                {sendSrcSlicesList, sendDstSlicesList}, {recvSrcSlicesList, recvDstSlicesList});
            SendRecvInfo sendRecvInfo(sendRecvChannels, sendRecvSlicesList, dataType_);
            CHK_PRT_RET(
                SendRecvBatchRead(sendRecvInfo, threads.at(remoteIdx)),
                HCCL_ERROR("[InsTempAllReduceMesh1DTwoShot][ScatterData] SendRecv failed."),
                HcclResult::HCCL_E_INTERNAL);
        }
    }

    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllReduceMesh1DTwoShot::PreSync(const std::vector<ThreadHandle>& threads)
{
    if (threads.size() > 1) {
        std::vector<ThreadHandle> slaveThreads(threads.begin() + 1, threads.end());
        GetNotifyIdxMainToSub(notifyIdxMainToSub_);
        CHK_RET(PreSyncInterThreads(threads.at(0), slaveThreads, notifyIdxMainToSub_));
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllReduceMesh1DTwoShot::PostSync(const std::vector<ThreadHandle>& threads)
{
    if (threads.size() > 1) {
        std::vector<ThreadHandle> slaveThreads(threads.begin() + 1, threads.end());
        GetNotifyIdxSubToMain(notifyIdxSubToMain_);
        CHK_RET(PostSyncInterThreads(threads.at(0), slaveThreads, notifyIdxSubToMain_));
    }
    return HcclResult::HCCL_SUCCESS;
}

} // namespace mc2_ops_hccl
