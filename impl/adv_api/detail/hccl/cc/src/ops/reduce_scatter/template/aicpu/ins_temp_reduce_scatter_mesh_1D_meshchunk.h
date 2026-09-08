/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef INS_TEMP_REDUCE_SCATTER_MESH_1D_MESH_CHUNK_H
#define INS_TEMP_REDUCE_SCATTER_MESH_1D_MESH_CHUNK_H

#include "alg_v2_template_base.h"
#include "executor_base.h"
#include "alg_data_trans_wrapper.h"

namespace mc2_ops_hccl {

class InsTempReduceScatterMesh1DMeshChunk : public InsAlgTemplateBase {
public:
    InsTempReduceScatterMesh1DMeshChunk() = default;
    explicit InsTempReduceScatterMesh1DMeshChunk(
        const OpParam& param, const u32 rankId, // 传通信域的rankId，userRank
        const std::vector<std::vector<u32>>& subCommRanks);
    ~InsTempReduceScatterMesh1DMeshChunk() override;

    std::string Describe() const override
    {
        std::string info = "Template of reduce scatter Mesh Chunk with tempRankSize ";
        info += std::to_string(templateRankSize_);
        return info;
    }

    HcclResult KernelRun(
        const OpParam& param, const TemplateDataParams& tempAlgParams,
        const TemplateResource& templateResource) override;
    HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        AlgResourceRequest& resourceRequest) override;
    u64 CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType) override;

    HcclResult PreCopy(const TemplateDataParams& tempAlgParams, const std::vector<ThreadHandle>& threads) const;
    HcclResult PostCopy(const TemplateDataParams& tempAlgParams, const std::vector<ThreadHandle>& threads);
    HcclResult CalcSliceInfoVec(const u64& dataSize, RankSliceInfo& sliceInfoVec);

    void GetNotifyIdxMainToSub(std::vector<u32>& notifyIdxMainToSub) override;
    void GetNotifyIdxSubToMain(std::vector<u32>& notifyIdxSubToMain) override;

private:
    HcclResult PrepareSlicesAndValidate(const TemplateDataParams& params, const TemplateResource& resources);
    HcclResult RunReduceScatter(
        const std::map<u32, std::vector<ChannelInfo>>& channels, const std::vector<ThreadHandle>& threads,
        const TemplateDataParams& tempAlgParams, RankSliceInfo& sliceInfoVec);
    HcclResult DoMeshChunk(
        const std::map<u32, std::vector<ChannelInfo>>& channels, const std::vector<ThreadHandle>& threads,
        const TemplateDataParams& tempAlgParams, const std::vector<uint64_t>& sliceSize, const u32& repeatIdx,
        const u32& myAlgRank, uint64_t& sliceSendOffset_, uint64_t& sliceRecvOffset_,
        const uint64_t& sliceRecvBaseOffset);
    void NotifyIdxMainToSubInMeshChunk(std::vector<u32>& notifyIdxMainToSub);
    void NotifyIdxSubToMainInMeshChunk(std::vector<u32>& notifyIdxSubToMain);
    u64 processSize_{0};
    u32 rankIdx_{0};
    u64 count_{0};
    u64 dataTypeSize_{0};
    std::vector<uint64_t> chunkSizes_;
};

} // namespace mc2_ops_hccl

#endif // OPEN_HCCL_INS_TEMP_REDUCE_SCATTER_MESH_1D_MESH_CHUNK_H
