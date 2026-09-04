/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef INS_TEMP_REDUCE_SCATTER_MESH_1D_H
#define INS_TEMP_REDUCE_SCATTER_MESH_1D_H

#include "alg_v2_template_base.h"
#include "executor_v2_base.h"
#include "alg_data_trans_wrapper.h"

namespace mc2_ops_hccl {

class InsTempReduceScatterMesh1D : public InsAlgTemplateBase {
public:
    InsTempReduceScatterMesh1D() = default;
    explicit InsTempReduceScatterMesh1D(
        const OpParam& param, const u32 rankId, // 传通信域的rankId，userRank
        const std::vector<std::vector<u32>>& subCommRanks);

    ~InsTempReduceScatterMesh1D() override;

    std::string Describe() const override
    {
        std::string info = "Template of reduce scatter Mesh with tempRankSize ";
        info += std::to_string(templateRankSize_);
        return info;
    }

    HcclResult KernelRun(
        const OpParam& param, const TemplateDataParams& tempAlgParams,
        const TemplateResource& templateResource) override;
    HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        AlgResourceRequest& resourceRequest) override;
    HcclResult GetRes(AlgResourceRequest& resourceRequest) const override;
    u64 CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType) override;
    u64 GetThreadNum() const override;
    HcclResult PostCopy(
        const OpParam& param, const TemplateDataParams& tempAlgParams, const std::vector<ThreadHandle>& threads);

    void GetNotifyIdxMainToSub(std::vector<u32>& notifyIdxMainToSub) override;
    void GetNotifyIdxSubToMain(std::vector<u32>& notifyIdxSubToMain) override;
    HcclResult SetchannelsPerRank(const std::map<u32, std::vector<ChannelInfo>>& channels) override;

protected:
    HcclResult RunReduceScatter(
        const std::map<u32, std::vector<ChannelInfo>>& channels, const std::vector<ThreadHandle>& threads,
        const TemplateDataParams& tempAlgParam);
    virtual HcclResult CalcDataSplitByPortGroup(
        const u64 totalDataCount, const u64 dataTypeSize, const std::vector<ChannelInfo>& channels,
        std::vector<u64>& elemCountOut, std::vector<u64>& sizeOut, std::vector<u64>& elemOffset);
    u64 processSize_{0};
    u64 count_{0};
    u32 channelsPerRank_{1};
    std::vector<u64> elemCountOut_;
    std::vector<u64> sizeOut_;
    std::vector<u64> elemOffset_;
};

} // namespace mc2_ops_hccl

#endif // OPEN_HCCL_INS_TEMP_REDUCE_SCATTER_MESH_H
