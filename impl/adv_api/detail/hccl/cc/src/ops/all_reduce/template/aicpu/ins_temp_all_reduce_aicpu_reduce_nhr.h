/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef AICPU_ALLREDUCE_AICPU_REDUCE_NHR_H
#define AICPU_ALLREDUCE_AICPU_REDUCE_NHR_H

#include <cstring>
#include "alg_v2_template_base.h"
#include "executor_base.h"
#include "alg_data_trans_wrapper.h"

namespace mc2_ops_hccl {

class InsTempAllReduceAicpuReduceNHR : public InsAlgTemplateBase {
public:
    InsTempAllReduceAicpuReduceNHR() = default;
    explicit InsTempAllReduceAicpuReduceNHR(
        const OpParam& param, const u32 rankId, // 传通信域的rankId，userRank
        const std::vector<std::vector<u32>>& subCommRanks);

    ~InsTempAllReduceAicpuReduceNHR() override;

    std::string Describe() const override
    {
        std::string info = "Template of All Reduce AicpuReduce NHR with tempRankSize ";
        info += std::to_string(templateRankSize_);
        return info;
    }

    // 现在的Kernel就是之前的GenExtIns
    HcclResult KernelRun(
        const OpParam& param, const TemplateDataParams& tempAlgParams,
        const TemplateResource& templateResource) override;
    void SetRoot(u32 root);
    HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        AlgResourceRequest& resourceRequest) override;
    u64 CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType) override;

    HcclResult PostCopy(const TemplateDataParams& tempAlgParams, const std::vector<ThreadHandle>& threads);

    void GetNotifyIdxMainToSub(std::vector<u32>& notifyIdxMainToSub);
    void GetNotifyIdxSubToMain(std::vector<u32>& notifyIdxSubToMain);

    u64 GetThreadNum() const override;

private:
    HcclResult CalcSlice(const u64 chunkSize);
    HcclResult PreCopy(const TemplateDataParams& tempAlgParams);
    HcclResult RunReduce(
        const std::map<u32, std::vector<ChannelInfo>>& channels, const TemplateDataParams& tempAlgParams,
        const std::string& algTag);
    HcclResult RunGather(const std::map<u32, std::vector<ChannelInfo>>& channels);
    HcclResult PostCopy(const TemplateDataParams& tempAlgParams);

    HcclResult GetStepInfo(u32 step, u32 nSteps, AicpuNHRStepInfo& stepInfo);
    u32 GetAlgRank(u32 rank) const;

    ThreadHandle thread_ = 0;
    u64 processSize_{0};
    u64 count_{0};
    u32 myIdx_ = UINT32_MAX; // 本rank在通信域内的索引

    RankSliceInfo sliceInfoVec_;

    bool isDmaRead_{false};
};

} // namespace mc2_ops_hccl

#endif // AICPU_ALLREDUCE_AICPU_REDUCE_NHR_H
