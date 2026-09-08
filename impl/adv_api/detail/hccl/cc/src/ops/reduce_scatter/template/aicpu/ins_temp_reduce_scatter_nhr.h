/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef INS_TEMP_REDUCE_SCATTER_NHR_H
#define INS_TEMP_REDUCE_SCATTER_NHR_H

#include <set>
#include "alg_v2_template_base.h"
#include "executor_v2_base.h"
#include "alg_data_trans_wrapper.h"

namespace mc2_ops_hccl {

class InsTempReduceScatterNHR : public InsAlgTemplateBase {
public:
    InsTempReduceScatterNHR() = default;
    explicit InsTempReduceScatterNHR(
        const OpParam& param, const u32 rankId, // 传通信域的rankId，userRank
        const std::vector<std::vector<u32>>& subCommRanks);
    ~InsTempReduceScatterNHR() override;

    std::string Describe() const override
    {
        std::string info = "Template of reduce scatter NHR with tempRankSize ";
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
    HcclResult GetStepInfoList(std::vector<AicpuNHRStepInfo>& stepInfoList);
    HcclResult LocalDataCopy(const std::vector<ThreadHandle>& threads, u32 channelIdx);
    HcclResult RunNHR(const std::vector<ThreadHandle>& threads, u32 channelIdx);
    HcclResult PostLocalCopy(const std::vector<ThreadHandle>& threads, u32 channelIdx);
    TemplateDataParams tempAlgParams_;
    std::map<u32, std::vector<ChannelInfo>> channels_;
    u64 dataTypeSize_{0};
    std::vector<u64> sizeOut_;
    std::vector<u64> elemOffset_;
    std::vector<u64> sizeOutTail_;
    std::vector<u64> elemOffsetTail_;
    bool isDmaRead_{false};
    bool doPreCopy_{false};
    bool skipStep0TxPreCopy_{false};
    std::set<u32> step0TxSliceIdxs_;
};

} // namespace mc2_ops_hccl

#endif // !HCCLV2_INS_TEMP_REDUCE_SCATTER_NHR
