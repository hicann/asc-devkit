/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef INS_TEMP_UBX_ALL_TO_ALL_V_MESH_1D_H
#define INS_TEMP_UBX_ALL_TO_ALL_V_MESH_1D_H

#include "alg_v2_template_base.h"
#include "executor_base.h"
#include "alg_data_trans_wrapper.h"

namespace mc2_ops_hccl {

class InsTempUBXAllToAllVMesh1D : public InsAlgTemplateBase {
public:
    InsTempUBXAllToAllVMesh1D() = default;
    explicit InsTempUBXAllToAllVMesh1D(
        const OpParam& param, const u32 rankId, const std::vector<std::vector<u32>>& subCommRanks);

    ~InsTempUBXAllToAllVMesh1D() override;

    std::string Describe() const override
    {
        std::string info = "Template of alltoallv Mesh with tempRankSize ";
        info += std::to_string(templateRankSize_);
        return info;
    }

    // 现在的RunAsync就是之前的GenExtIns
    HcclResult KernelRun(
        const OpParam& param, const TemplateDataParams& tempAlgParams,
        const TemplateResource& templateResource) override;
    HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        AlgResourceRequest& resourceRequest) override;
    u64 CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType) override;

    void GetNotifyIdxMainToSub(std::vector<u32>& notifyIdxMianToSub) override;
    void GetNotifyIdxSubToMain(std::vector<u32>& notifyIdxSubToMain) override;

private:
    void GetNotifyIdxMainToClos(std::vector<u32>& notifyIdxMianToSub) const;
    void GetNotifyIdxClosToMain(std::vector<u32>& notifyIdxSubToMain) const;
    void GetNotifyIdxMainToFullMesh(std::vector<u32>& notifyIdxMianToSub) const;
    void GetNotifyIdxFullMeshToMain(std::vector<u32>& notifyIdxSubToMain) const;
    HcclResult InitParam(
        const OpParam& param, const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource);
    HcclResult GetBoardSendRecvMatrix(u32 n, std::vector<std::vector<u32>>& sendRecvMatrix) const;
    HcclResult GetRankSendRecvMatrix(u32 board1, u32 board2, std::vector<std::vector<u32>>& rankSendRecvMatrix) const;
    HcclResult GetRankNumPerBoard(const TemplateResource& templateResource);
    HcclResult CheckPathNum(const TemplateResource& templateResource) const;
    HcclResult RunFullMeshSelfCopy(
        const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource, u32 targetRank,
        u32 fullMeshThreadId) const;
    HcclResult RunFullMeshPeer(
        const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource, u32 targetRank,
        u32 fullMeshThreadId);
    HcclResult RunFullMesh(const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource);
    HcclResult RunPairwiseStep(const std::vector<ThreadHandle>& threads);
    HcclResult RunPairwiseChannel(
        const TemplateDataParams& tempAlgParams, const std::vector<ChannelInfo>& channelSendRecv,
        const std::vector<ThreadHandle>& threads, u32 targetRank, u32 channelIndex, u64& innerSendOffset,
        u64& innerRecvOffset, u64 curSendDataCount, u64 curRecvDataCount);
    HcclResult RunPairwiseTransfer(
        const ChannelInfo& channel, const std::vector<ThreadHandle>& threads, u32 queId,
        const std::vector<DataSlice>& txSrcSlices, const std::vector<DataSlice>& txDstSlices,
        const std::vector<DataSlice>& rxSrcSlices, const std::vector<DataSlice>& rxDstSlices, u64 sendDataSize,
        u64 recvDataSize) const;
    HcclResult SavePairwiseLocalCopy(const TemplateDataParams& tempAlgParams, u32 targetRank, u64 curRecvDataCount);
    HcclResult RunPairwise(
        const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource, u32 targetBoard);
    HcclResult InitRankAndDataType(const OpParam& param);
    HcclResult InitBoardState(const TemplateResource& templateResource);
    HcclResult InitDataState(const TemplateDataParams& tempAlgParams);
    HcclResult InitThreadState(const TemplateResource& templateResource);
    void LogInitState() const;
    void LogDataSlices(const TemplateDataParams& tempAlgParams) const;

    u64 dataTypeSize_{0};
    bool isDmaRead_{false};
    u32 concurrentSendRecvNum_{1};
    std::vector<u64> sendCountsSplit_;
    std::vector<u64> sendSizeSplit_;
    std::vector<u64> sendOffsetSplit_;
    std::vector<u64> recvCountsSplit_;
    std::vector<u64> recvSizeSplit_;
    std::vector<u64> recvOffsetSplit_;

    bool needDealWithFullMeshInfo_{false};
    u32 myAlgRank_{0};
    u32 rankNumPerBoard_{0};
    u32 maxPathNum_{4};         // 跨框最多4jetty
    u32 maxRankNumPerBoard_{4}; // ubx机型fullmesh内最多4P
    u32 currBoard_{0};
    u32 currRankIndex_{0};
    u32 boardNum_{0};
    u32 algBoardNum_{0};
    u32 channelsPerRank_{1};
    u64 dataCountPerRank_{0};
    u64 dataSizePerRank_{0};
    u64 scratchBufferSizePerRank_{0};
    u64 dataStridePerRank_{0};
    u64 curProcessedDataCount_{0};
    u64 curDataCount_{0};
    u64 curDataSize_{0};
    std::vector<std::vector<DataSlice>> localCopyInfo_;
    std::vector<std::vector<DataSlice>> localCopyInfoFullMesh_;
    std::vector<ThreadHandle> subThreadsBoard_;
    std::vector<ThreadHandle> subThreadsFullMesh_;
};

} // namespace mc2_ops_hccl

#endif // INS_TEMP_UBX_ALL_TO_ALL_V_MESH_1D_H
