/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef INS_TEMP_REDUCE_SCATTER_AICPU_REDUCE_NHR_H
#define INS_TEMP_REDUCE_SCATTER_AICPU_REDUCE_NHR_H

#include <cstring>
#include "alg_v2_template_base.h"
#include "executor_v2_base.h"
#include "alg_data_trans_wrapper.h"

namespace mc2_ops_hccl {

class InsTempReduceScatterAicpuReduceNHR : public InsAlgTemplateBase {
public:
    InsTempReduceScatterAicpuReduceNHR() = default;
    explicit InsTempReduceScatterAicpuReduceNHR(
        const OpParam& param, const u32 rankId, // 传通信域的rankId，userRank
        const std::vector<std::vector<u32>>& subCommRanks);
    ~InsTempReduceScatterAicpuReduceNHR() override;

    std::string Describe() const override
    {
        std::string info = "Template of reduce scatter AicpuReduceNHR with tempRankSize ";
        info += std::to_string(templateRankSize_);
        return info;
    }

    HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        AlgResourceRequest& resourceRequest) override;
    HcclResult GetRes(AlgResourceRequest& resourceRequest) const override;
    HcclResult KernelRun(
        const OpParam& param, const TemplateDataParams& tempAlgParams,
        const TemplateResource& templateResource) override;
    u64 CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType) override;
    u64 GetThreadNum() const override;

    void GetNotifyIdxMainToSub(std::vector<u32>& notifyIdxMainToSub) override;
    void GetNotifyIdxSubToMain(std::vector<u32>& notifyIdxSubToMain) override;

private:
    HcclResult GetStepInfo(u32 step, u32 nSteps, AicpuNHRStepInfo& stepInfo);
    u32 GetRankFromMap(const u32 algRankIdx);
    HcclResult GetStepInfoList(std::vector<AicpuNHRStepInfo>& stepInfoList);
    HcclResult LocalDataCopy(const std::vector<ThreadHandle>& threads, u32 sliceIdx);
    HcclResult RunAllGather(const std::vector<ThreadHandle>& threads);
    HcclResult PostLocalReduce(const std::vector<ThreadHandle>& threads);
    HcclResult LocalCopyToOutput(const std::vector<ThreadHandle>& threads, u32 sliceIdx);
    TemplateDataParams tempAlgParams_;
    std::map<u32, std::vector<ChannelInfo>> channels_;
};

} // namespace mc2_ops_hccl

#endif // INS_TEMP_REDUCE_SCATTER_AICPU_REDUCE_NHR_H
