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
    // NHR 需要的 que Num 为 1
    resourceRequest.slaveThreadNum = 0;
    resourceRequest.notifyNumOnMainThread = 0;

    std::vector<HcclChannelDesc> channels;
    CHK_RET(CalcChannelRequestNhr(comm, param, topoInfo, subCommRanks_, channels));
    resourceRequest.channels.push_back(channels);
    HCCL_INFO(
        "[InsTempReduceScatterNHR][CalcRes] slaveThreadNum: [%u], notifyNumOnMainThread: [%u].",
        resourceRequest.slaveThreadNum, resourceRequest.notifyNumOnMainThread);
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempReduceScatterNHR::GetRes(AlgResourceRequest& resourceRequest) const
{
    resourceRequest.slaveThreadNum = 0;
    resourceRequest.notifyNumOnMainThread = 0;

    return HCCL_SUCCESS;
}

u64 InsTempReduceScatterNHR::GetThreadNum() const { return 1; }

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
    CHK_RET(LocalDataCopy(templateResource.threads));

    if (templateRankSize_ <= 1) {
        CHK_RET(PostLocalCopy(templateResource.threads));
        return HcclResult::HCCL_SUCCESS;
    }

    CHK_RET(RunNHR(templateResource.threads));
    CHK_RET(PostLocalCopy(templateResource.threads));
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempReduceScatterNHR::LocalDataCopy(const std::vector<ThreadHandle>& threads)
{
    CHK_PRT_RET(
        threads.empty(), HCCL_ERROR("[InsTempReduceScatterNHR][LocalDataCopy] empty threads"),
        HcclResult::HCCL_E_INTERNAL);
    ThreadHandle q = threads[0];
    const u64 rptNum = std::max<u64>(1, tempAlgParams_.repeatNum);
    for (u32 localRandId = 0; localRandId < templateRankSize_; ++localRandId) {
        u64 sliceSize = tempAlgParams_.sliceSize;
        if (localRandId == templateRankSize_ - 1 && tempAlgParams_.tailSize > 0) {
            sliceSize = tempAlgParams_.tailSize;
        }
        for (u64 rpt = 0; rpt < rptNum; ++rpt) {
            const u64 inBaseOff = tempAlgParams_.buffInfo.inBuffBaseOff + rpt * tempAlgParams_.inputRepeatStride;
            const u64 scratchBase = tempAlgParams_.buffInfo.hcclBuffBaseOff + rpt * tempAlgParams_.outputRepeatStride;

            const u64 inOff = inBaseOff + localRandId * tempAlgParams_.inputSliceStride;
            const u64 scOff = scratchBase + localRandId * tempAlgParams_.sliceSize;

            DataSlice src = DataSlice(tempAlgParams_.buffInfo.inputPtr, inOff, sliceSize);
            DataSlice dst = DataSlice(tempAlgParams_.buffInfo.hcclBuff.addr, scOff, sliceSize);

            // 如果源地址和目标地址相同，则不需要做拷贝
            if (tempAlgParams_.buffInfo.inBuffType != tempAlgParams_.buffInfo.hcclBuffType || inOff != scOff) {
                CHK_RET(LocalCopy(q, src, dst));
            }
        }
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempReduceScatterNHR::PostLocalCopy(const std::vector<ThreadHandle>& threads)
{
    CHK_PRT_RET(threads.empty(), HCCL_ERROR("[RS-NHR][PostLocalCopy] empty queue"), HcclResult::HCCL_E_INTERNAL);

    u32 myAlgIdx = 0;
    CHK_RET(GetAlgRank(myRank_, subCommRanks_[0], myAlgIdx));
    u64 sliceSize = 0;
    if (myAlgIdx == templateRankSize_ - 1 && tempAlgParams_.tailSize > 0) {
        sliceSize = tempAlgParams_.tailSize;
    } else {
        sliceSize = tempAlgParams_.sliceSize;
    }
    ThreadHandle q = threads[0];

    const u64 rptNum = std::max<u64>(1, tempAlgParams_.repeatNum);
    for (u64 rpt = 0; rpt < rptNum; ++rpt) {
        const u64 outBaseOff = tempAlgParams_.buffInfo.outBuffBaseOff + rpt * tempAlgParams_.outputRepeatStride;
        const u64 scratchBase = tempAlgParams_.buffInfo.hcclBuffBaseOff + rpt * tempAlgParams_.outputRepeatStride;

        const u64 scOff = scratchBase + tempAlgParams_.sliceSize * myAlgIdx;
        const u64 outOff = outBaseOff + myAlgIdx * tempAlgParams_.outputSliceStride;

        DataSlice src = DataSlice(tempAlgParams_.buffInfo.hcclBuff.addr, scOff, sliceSize);
        DataSlice dst = DataSlice(tempAlgParams_.buffInfo.outputPtr, outOff, sliceSize);

        if (tempAlgParams_.buffInfo.hcclBuffType != tempAlgParams_.buffInfo.outBuffType || scOff != outOff) {
            CHK_RET(LocalCopy(q, src, dst));
        }
    }
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempReduceScatterNHR::RunNHR(const std::vector<ThreadHandle>& threads)
{
    CHK_PRT_RET(threads.empty(), HCCL_ERROR("[RS-NHR][RunNHR] empty queue"), HcclResult::HCCL_E_INTERNAL);

    if (templateRankSize_ <= 1)
        return HcclResult::HCCL_SUCCESS;

    ThreadHandle q = threads[0];

    // 步进参数
    const u64 rptNum = std::max<u64>(1, tempAlgParams_.repeatNum);

    // 预计算步骤列表（算法序）
    std::vector<AicpuNHRStepInfo> steps;
    CHK_RET(GetStepInfoList(steps));
    for (u64 rpt = 0; rpt < rptNum; ++rpt) {
        const u64 scratchBase = tempAlgParams_.buffInfo.hcclBuffBaseOff + rpt * tempAlgParams_.outputRepeatStride;
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
                    channels_[recvFromRank].size() == 0 || channels_[sendToRank].size() == 0,
                HCCL_ERROR("[RS-NHR][RunNHR] link missing: recvFrom=%d sendTo=%d", recvFromRank, sendToRank),
                HcclResult::HCCL_E_INTERNAL);
            ChannelInfo linkRecv = channels_[recvFromRank].at(0);
            ChannelInfo linkSend = channels_[sendToRank].at(0);

            std::vector<DataSlice> txSrcSlices;
            std::vector<DataSlice> txDstSlices;
            std::vector<DataSlice> rxSlices;
            txSrcSlices.reserve(st.nSlices);
            txDstSlices.reserve(st.nSlices);
            rxSlices.reserve(st.nSlices);

            void* remoteCclBuffAddr = linkSend.remoteCclMem.addr;
            // RS：在 SCRATCH 上进行规约交换
            for (u32 i = 0; i < st.nSlices; ++i) {
                const u32 txIdx = st.txSliceIdxs[i]; // 算法序
                const u32 rxIdx = st.rxSliceIdxs[i];

                const u64 txScOff = scratchBase + tempAlgParams_.sliceSize * txIdx;
                const u64 rxScOff = scratchBase + tempAlgParams_.sliceSize * rxIdx;

                const u64 txSliceSize = (txIdx == templateRankSize_ - 1 && tempAlgParams_.tailSize > 0) ?
                                            tempAlgParams_.tailSize :
                                            tempAlgParams_.sliceSize;
                const u64 rxSliceSize = (rxIdx == templateRankSize_ - 1 && tempAlgParams_.tailSize > 0) ?
                                            tempAlgParams_.tailSize :
                                            tempAlgParams_.sliceSize;

                DataSlice txSrcSlice = DataSlice(
                    tempAlgParams_.buffInfo.hcclBuff.addr, txScOff, txSliceSize,
                    txSliceSize / DATATYPE_SIZE_TABLE[dataType_]); // 发送源
                DataSlice txDstSlice = DataSlice(
                    remoteCclBuffAddr, txScOff, txSliceSize, txSliceSize / DATATYPE_SIZE_TABLE[dataType_]); // 发送目标
                DataSlice rxSlice = DataSlice(
                    tempAlgParams_.buffInfo.hcclBuff.addr, rxScOff, rxSliceSize,
                    rxSliceSize / DATATYPE_SIZE_TABLE[dataType_]);
                txSrcSlices.push_back(txSrcSlice);
                txDstSlices.push_back(txDstSlice);
                rxSlices.emplace_back(rxSlice);
            }

            SendRecvReduceInfo info{
                {linkSend, linkRecv}, {{txSrcSlices, txDstSlices}, {rxSlices, rxSlices}}, dataType_, reduceOp_};

            CHK_PRT_RET(
                SendRecvBatchWriteReduce(info, threads[0]),
                HCCL_ERROR(
                    "[RS-NHR][RunNHR] SendRecvReduce failed (step=%u, rpt=%llu)", st.step,
                    static_cast<unsigned long long>(rpt)),
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
        u32 txSliceIdx = sendTo;
        u32 rxSliceIdx = u32x;

        AicpuNHRStepInfo& currStepInfo = stepInfoList[step];
        currStepInfo.step = step;
        currStepInfo.myRank = u32x;
        currStepInfo.nSlices = nSlices;
        currStepInfo.toRank = sendTo;
        currStepInfo.fromRank = recvFrom;

        // 计算本rank在每轮收/发中的slice编号
        currStepInfo.txSliceIdxs.reserve(nSlices);
        currStepInfo.rxSliceIdxs.reserve(nSlices);
        for (u32 i = 0; i < nSlices; i++) {
            currStepInfo.txSliceIdxs.push_back(txSliceIdx);
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

void InsTempReduceScatterNHR::GetNotifyIdxMainToSub(std::vector<u32>& notifyIdxMianToSub) {}

void InsTempReduceScatterNHR::GetNotifyIdxSubToMain(std::vector<u32>& notifyIdxSubToMain) {}

} // namespace mc2_ops_hccl
