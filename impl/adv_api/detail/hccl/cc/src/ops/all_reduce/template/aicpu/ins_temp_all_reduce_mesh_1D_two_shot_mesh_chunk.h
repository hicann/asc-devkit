/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCLV2_INS_TEMP_ALL_REDUCE_1D_MESH_TWO_SHOT_MESH_CHUNK
#define HCCLV2_INS_TEMP_ALL_REDUCE_1D_MESH_TWO_SHOT_MESH_CHUNK

#include "alg_v2_template_base.h"
#include "executor_base.h"
#include "alg_data_trans_wrapper.h"

namespace mc2_ops_hccl {

class InsTempAllReduceMesh1DTwoShotMeshChunk : public InsAlgTemplateBase {
public:
    InsTempAllReduceMesh1DTwoShotMeshChunk() = default;
    explicit InsTempAllReduceMesh1DTwoShotMeshChunk(
        const OpParam& param,
        const u32 rankId, // 传通信域的rankId，userRank
        const std::vector<std::vector<u32>>& subCommRanks);
    ~InsTempAllReduceMesh1DTwoShotMeshChunk() override;

    std::string Describe() const override
    {
        std::string info = "Template of reduce scatter Mesh Chunk with tempRankSize ";
        info += std::to_string(templateRankSize_);
        return info;
    }

    u64 CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType) override;
    HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        AlgResourceRequest& resourceRequest) override;
    HcclResult KernelRun(
        const OpParam& param, const TemplateDataParams& tempAlgParams,
        const TemplateResource& templateResource) override;

    HcclResult PreCopy(
        const TemplateDataParams& tempAlgParams, const std::vector<ThreadHandle>& threads,
        const RankSliceInfo& sliceInfoVec);
    HcclResult CalcSliceInfoVec(const u64& dataSize, RankSliceInfo& sliceInfoVec);

    void GetNotifyIdxMainToSub(std::vector<u32>& notifyIdxMainToSub) override;
    void GetNotifyIdxSubToMain(std::vector<u32>& notifyIdxSubToMain) override;

private:
    HcclResult ReduceScatterMeshChunk(
        const RankSliceInfo& sliceInfoVec, const std::map<u32, std::vector<ChannelInfo>>& channels,
        const std::vector<ThreadHandle>& threads, const TemplateDataParams& tempAlgParams,
        const std::vector<std::vector<u64>>& sliceSize, const u32& stepIndex);
    HcclResult RunReduceScatter(
        const std::map<u32, std::vector<ChannelInfo>>& channels, const std::vector<ThreadHandle>& threads,
        const TemplateDataParams& tempAlgParams, RankSliceInfo& sliceInfoVec);
    HcclResult RunAllgather(
        const std::map<u32, std::vector<ChannelInfo>>& channels, const std::vector<ThreadHandle>& threads,
        const TemplateDataParams& tempAlgParams, RankSliceInfo& sliceInfoVec);
    void NotifyIdxMainToSubInRSMeshChunk(std::vector<u32>& notifyIdxMainToSub);
    void NotifyIdxSubToMainInRSMeshChunk(std::vector<u32>& notifyIdxSubToMain);
    void NotifyIdxMainToSubInAG(std::vector<u32>& notifyIdxMainToSub);
    void NotifyIdxSubToMainInAG(std::vector<u32>& notifyIdxSubToMain);

    u64 processSize_{0};
    u64 count_{0};
    u64 dataTypeSize_{0};
    u32 myAlgRank_{0};
};
} // namespace mc2_ops_hccl

#endif // HCCLV2_INS_TEMP_ALL_REDUCE_1D_MESH_TWO_SHOT_MESH_CHUNK
