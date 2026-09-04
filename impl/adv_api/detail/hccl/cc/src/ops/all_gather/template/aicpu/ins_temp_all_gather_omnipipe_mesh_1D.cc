/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ins_temp_all_gather_omnipipe_mesh_1D.h"
#include <sstream>
#include "alg_data_trans_wrapper.h"
#include "template_utils.h"

namespace mc2_ops_hccl {
InsTempAllGatherOmniPipeMesh1D::InsTempAllGatherOmniPipeMesh1D(
    const OpParam& param, const u32 rankId, const std::vector<std::vector<u32>>& subCommRanks)
    : InsTempAllGatherMesh1D(param, rankId, subCommRanks)
{}
InsTempAllGatherOmniPipeMesh1D::~InsTempAllGatherOmniPipeMesh1D() {}

HcclResult InsTempAllGatherOmniPipeMesh1D::KernelRun(
    const OpParam& param, const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource)
{
    HCCL_INFO("[InsTempAllGatherOmniPipeMesh1D] Run start");
    if (templateRankSize_ == 1) {
        return HcclResult::HCCL_SUCCESS;
    }
    threadNum_ = templateResource.threads.size();
    tempAlgParams_ = tempAlgParams;
    tempAlgParams_.buffInfo.outputPtr = param.outputPtr;
    omniLastStepRead_ = tempAlgParams.omniLastStepRead_;
    dataType_ = param.DataDes.dataType;
    HCCL_DEBUG("[InsTempAllGatherOmniPipeMesh1D] Rank [%d], get threadNum_[%d].", myRank_, threadNum_);

    if (threadNum_ > 1) {
        std::vector<ThreadHandle> subThreads(templateResource.threads.begin() + 1, templateResource.threads.end());
        GetNotifyIdxMainToSub(notifyIdxMainToSub_);
        CHK_RET(PreSyncInterThreads(templateResource.threads[0], subThreads, notifyIdxMainToSub_));
    }

    CHK_RET(RunAllGatherMesh(templateResource.threads, templateResource.channels));

    if (threadNum_ > 1) {
        std::vector<ThreadHandle> subThreads(templateResource.threads.begin() + 1, templateResource.threads.end());
        GetNotifyIdxSubToMain(notifyIdxSubToMain_);
        CHK_RET(PostSyncInterThreads(templateResource.threads[0], subThreads, notifyIdxSubToMain_));
    }
    HCCL_INFO("[InsTempAllGatherOmniPipeMesh1D] Run End");
    return HcclResult::HCCL_SUCCESS;
}

HcclResult InsTempAllGatherOmniPipeMesh1D::RunAllGatherMesh(
    const std::vector<ThreadHandle>& threads, const std::map<u32, std::vector<ChannelInfo>>& channels)
{
    HCCL_INFO("[InsTempAllGatherOmniPipeMesh1D] RunAllGatherMesh RankIDs[%d].", myRank_);
    const u32 dataTypeSize = SIZE_TABLE[dataType_];

    u32 myAlgRank = 0;
    CHK_RET(GetAlgRank(myRank_, subCommRanks_[0], myAlgRank));

    for (u32 threadIdx = 0; threadIdx < subCommRanks_[0].size() - 1; threadIdx++) {
        u32 connectedRank = subCommRanks_[0][(myAlgRank + 1 + threadIdx) % subCommRanks_[0].size()];

        u32 connectedAlgRank = 0;
        CHK_RET(GetAlgRank(connectedRank, subCommRanks_[0], connectedAlgRank));
        HCCL_INFO(
            "[InsTempAllGatherOmniPipeMesh1D] RunAllGatherMesh RankIDs[%d], connectedRank[%d], "
            "connectedAlgRank[%d].",
            myRank_, connectedRank, connectedAlgRank);

        CHK_PRT_RET(
            threadIdx >= threads.size() || channels.count(connectedRank) == 0,
            HCCL_ERROR(
                "[InsTempAllGatherOmniPipeMesh1D][RankID]=%u threadIdx=%u, threads.size=%u, "
                "connectedRank=%d, channels.size=%u",
                myRank_, threadIdx, threads.size(), connectedRank, channels.size()),
            HcclResult::HCCL_E_INTERNAL);

        ThreadHandle currQue = threads[threadIdx];

        std::vector<DataSlice> txSrcSlices, txDstSlices, rxSrcSlices, rxDstSlices;

        const ChannelInfo& linkRemote = channels.at(connectedRank)[0];
        void* remoteCclBuffAddr = linkRemote.remoteCclMem.addr;

        void* txSrcPtr = nullptr;
        void* txDstPtr = remoteCclBuffAddr;
        void* rxSrcPtr = remoteCclBuffAddr;
        void* rxDstPtr = nullptr;

        for (u32 rpt = 0; rpt < tempAlgParams_.stepSliceInfo.inputOmniPipeSliceStride[myAlgRank].size(); ++rpt) {
            u64 txBaseOff = tempAlgParams_.buffInfo.inBuffBaseOff +
                            tempAlgParams_.stepSliceInfo.inputOmniPipeSliceStride[myAlgRank][rpt];
            u64 rxBaseOff = tempAlgParams_.buffInfo.outBuffBaseOff +
                            tempAlgParams_.stepSliceInfo.outputOmniPipeSliceStride[connectedAlgRank][rpt];
            u64 txOffset = tempAlgParams_.stepSliceInfo.stepInputSliceStride[myAlgRank] + txBaseOff;
            u64 rxOffset = tempAlgParams_.stepSliceInfo.stepOutputSliceStride[connectedAlgRank] + rxBaseOff;

            if (!omniLastStepRead_) {
                txSrcPtr = tempAlgParams_.buffInfo.hcclBuff.addr;
                rxDstPtr = tempAlgParams_.buffInfo.hcclBuff.addr;

                DataSlice txSrcSlice = DataSlice(
                    txSrcPtr, txOffset, tempAlgParams_.stepSliceInfo.stepSliceSize[myAlgRank][rpt],
                    tempAlgParams_.stepSliceInfo.stepCount[myAlgRank][rpt]);
                DataSlice txDstSlice = DataSlice(
                    txDstPtr, txOffset, tempAlgParams_.stepSliceInfo.stepSliceSize[myAlgRank][rpt],
                    tempAlgParams_.stepSliceInfo.stepCount[myAlgRank][rpt]);
                DataSlice rxDstSlice = DataSlice(
                    rxDstPtr, rxOffset, tempAlgParams_.stepSliceInfo.stepSliceSize[connectedAlgRank][rpt],
                    tempAlgParams_.stepSliceInfo.stepSliceSize[connectedAlgRank][rpt]);
                DataSlice rxSrcSlice = DataSlice(
                    rxSrcPtr, rxOffset, tempAlgParams_.stepSliceInfo.stepSliceSize[connectedAlgRank][rpt],
                    tempAlgParams_.stepSliceInfo.stepSliceSize[connectedAlgRank][rpt]);

                rxSrcSlices.push_back(rxSrcSlice);
                rxDstSlices.push_back(rxDstSlice);
                txSrcSlices.push_back(txSrcSlice);
                txDstSlices.push_back(txDstSlice);
            } else {
                txSrcPtr = tempAlgParams_.buffInfo.outputPtr;
                rxDstPtr = tempAlgParams_.buffInfo.outputPtr;

                u64 txWriteSrcBaseOff =
                    tempAlgParams_.buffInfo.inBuffBaseOff +
                    tempAlgParams_.omniReadDstStepSliceInfo.inputOmniPipeSliceStride[myAlgRank][rpt];
                u64 rxReadDstBaseOff =
                    tempAlgParams_.buffInfo.outBuffBaseOff +
                    tempAlgParams_.omniReadDstStepSliceInfo.outputOmniPipeSliceStride[connectedAlgRank][rpt];
                u64 txWriteSrcOffset = tempAlgParams_.omniReadDstStepSliceInfo.stepInputSliceStride[myAlgRank] +
                                       txWriteSrcBaseOff + tempAlgParams_.processedDataCount * dataTypeSize;
                u64 rxReadDstOffset = tempAlgParams_.omniReadDstStepSliceInfo.stepOutputSliceStride[connectedAlgRank] +
                                      rxReadDstBaseOff + tempAlgParams_.processedDataCount * dataTypeSize;

                DataSlice txSrcSlice = DataSlice(
                    txSrcPtr, txWriteSrcOffset, tempAlgParams_.omniReadDstStepSliceInfo.stepSliceSize[myAlgRank][rpt],
                    tempAlgParams_.stepSliceInfo.stepCount[myAlgRank][rpt]);
                DataSlice txDstSlice = DataSlice(
                    txDstPtr, txOffset, tempAlgParams_.stepSliceInfo.stepSliceSize[myAlgRank][rpt],
                    tempAlgParams_.stepSliceInfo.stepCount[myAlgRank][rpt]);
                DataSlice rxDstSlice = DataSlice(
                    rxDstPtr, rxReadDstOffset,
                    tempAlgParams_.omniReadDstStepSliceInfo.stepSliceSize[connectedAlgRank][rpt],
                    tempAlgParams_.omniReadDstStepSliceInfo.stepSliceSize[connectedAlgRank][rpt]);
                DataSlice rxSrcSlice = DataSlice(
                    rxSrcPtr, rxOffset, tempAlgParams_.stepSliceInfo.stepSliceSize[connectedAlgRank][rpt],
                    tempAlgParams_.stepSliceInfo.stepSliceSize[connectedAlgRank][rpt]);

                rxSrcSlices.push_back(rxSrcSlice);
                rxDstSlices.push_back(rxDstSlice);
                txSrcSlices.push_back(txSrcSlice);
                txDstSlices.push_back(txDstSlice);
            }
        }

        TxRxSlicesList sendRecvSlicesList({txSrcSlices, txDstSlices}, {rxSrcSlices, rxDstSlices});
        TxRxChannels sendRecvChannels(linkRemote, linkRemote);
        SendRecvInfo sendRecvInfo(sendRecvChannels, sendRecvSlicesList);

        if (!omniLastStepRead_) {
            CHK_PRT_RET(
                SendRecvWrite(sendRecvInfo, threads[threadIdx]),
                HCCL_ERROR("[InsTempAllGatherOmniPipeMesh1D] RunAllGather Send failed"), HcclResult::HCCL_E_INTERNAL);
        } else {
            CHK_PRT_RET(
                SendRecvRead(sendRecvInfo, threads[threadIdx]),
                HCCL_ERROR("[InsTempAllGatherOmniPipeMesh1D]omniLastStepRead_ RunAllGather SendRecvRead failed"),
                HcclResult::HCCL_E_INTERNAL);
        }
    }
    return HcclResult::HCCL_SUCCESS;
}
} // namespace mc2_ops_hccl
