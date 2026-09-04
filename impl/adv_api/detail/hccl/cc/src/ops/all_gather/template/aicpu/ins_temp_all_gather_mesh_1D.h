/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef INS_TEMP_ALL_GATHER_MESH_1D_H
#define INS_TEMP_ALL_GATHER_MESH_1D_H

#include "alg_v2_template_base.h"
#include "executor_base.h"

namespace mc2_ops_hccl {

class InsTempAllGatherMesh1D : public InsAlgTemplateBase {
public:
    InsTempAllGatherMesh1D() = default;
    explicit InsTempAllGatherMesh1D(
        const OpParam& param, const u32 rankId, // 传通信域的rankId，userRank
        const std::vector<std::vector<u32>>& subCommRanks);
    // Host侧调用
    ~InsTempAllGatherMesh1D() override;

    std::string Describe() const override
    {
        std::string info = "Template of all gather Mesh with tempRankSize ";
        info += std::to_string(templateRankSize_);
        return info;
    }
    HcclResult KernelRun(
        const OpParam& param, const TemplateDataParams& tempAlgParams,
        const TemplateResource& templateResource) override;
    HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        AlgResourceRequest& resourceRequest) override;
    virtual HcclResult GetRes(AlgResourceRequest& resourceRequest) const;

    u64 CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType) override;
    virtual u64 GetThreadNum() const;
    HcclResult CalcDataSplitByPortGroup(
        const u64 totalDataCount, const u64 dataTypeSize, const std::vector<ChannelInfo>& channels,
        std::vector<u64>& elemCountOut, std::vector<u64>& sizeOut, std::vector<u64>& elemOffset) override;
    HcclResult SetchannelsPerRank(const std::map<u32, std::vector<ChannelInfo>>& channels) override;
    void GetNotifyIdxMainToSub(std::vector<u32>& notifyIdxMianToSub) override;
    void GetNotifyIdxSubToMain(std::vector<u32>& notifyIdxSubToMain) override;

protected:
    virtual HcclResult RunAllGatherMesh(
        const std::vector<ThreadHandle>& threads, const std::map<u32, std::vector<ChannelInfo>>& channels);
    virtual HcclResult LocalDataCopy(const std::vector<ThreadHandle>& threads);
    HcclResult PostLocalCopy(const std::vector<ThreadHandle>& threads);
    TemplateDataParams tempAlgParams_;
    u64 inputOffset_{0};
    u64 outputOffset_{0};
    void* inputSymWindow_{nullptr};
    void* outputSymWindow_{nullptr};
    u32 channelsPerRank_{1};
};

} // namespace mc2_ops_hccl

#endif // INS_TEMP_ALL_GATHER_MESH_1D_H
