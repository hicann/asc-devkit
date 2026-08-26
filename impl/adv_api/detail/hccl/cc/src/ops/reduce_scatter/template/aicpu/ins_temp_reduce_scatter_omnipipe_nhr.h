/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef INS_TEMP_REDUCE_SCATTER_OMNIPIPE_NHR_H
#define INS_TEMP_REDUCE_SCATTER_OMNIPIPE_NHR_H

#include "alg_v2_template_base.h"
#include "executor_v2_base.h"
#include "alg_data_trans_wrapper.h"
#include "ins_temp_reduce_scatter_nhr.h"

namespace mc2_ops_hccl {

class InsTempReduceScatterOmniPipeNHR : public InsTempReduceScatterNHR {
public:
    explicit InsTempReduceScatterOmniPipeNHR(
        const OpParam& param, const u32 rankId, // 传通信域的rankId，userRank
        const std::vector<std::vector<u32>>& subCommRanks);
    ~InsTempReduceScatterOmniPipeNHR() override;

    std::string Describe() const override
    {
        std::string info = "Template of reduce scatter omnipipe NHR with tempRankSize ";
        info += std::to_string(templateRankSize_);
        return info;
    }

    u64 CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType) override;
    HcclResult KernelRun(
        const OpParam& param, const TemplateDataParams& tempAlgParams,
        const TemplateResource& templateResource) override;
    HcclResult DoLocalCopy(const TemplateDataParams& tempAlgParams, const std::vector<ThreadHandle>& threads);

private:
    HcclResult GetStepInfoList(std::vector<AicpuNHRStepInfo>& stepInfoList);
    HcclResult RunNHR(const std::vector<ThreadHandle>& threads, u32 channelIdx);

    HcclResult GetNHRDataSize(
        const AicpuNHRStepInfo& st, const u32 channelIdx, void* sendCclBuffAddr, void* recvCclBuffAddr,
        const u32 dataTypeSize, const u64 rptNum, std::vector<DataSlice>& txSrcSlices,
        std::vector<DataSlice>& txDstSlices, std::vector<DataSlice>& rxSrcSlices, std::vector<DataSlice>& rxDstSlices);

    TemplateDataParams tempAlgParams_;
    std::map<u32, std::vector<ChannelInfo>> channels_;
    std::vector<std::vector<std::vector<u64>>> dataSplitVec_;
    std::vector<std::vector<std::vector<u64>>> dataOffsetVec_;
};

} // namespace mc2_ops_hccl

#endif
