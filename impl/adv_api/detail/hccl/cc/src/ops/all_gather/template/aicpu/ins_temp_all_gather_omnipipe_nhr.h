/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef INS_TEMP_ALL_GATHER_OMNIPIPE_NHR_H
#define INS_TEMP_ALL_GATHER_OMNIPIPE_NHR_H

#include "ins_temp_all_gather_nhr.h"

namespace mc2_ops_hccl {

class InsTempAllGatherOmniPipeNHR : public InsTempAllGatherNHR {
public:
    explicit InsTempAllGatherOmniPipeNHR(
        const OpParam& param, const u32 rankId, const std::vector<std::vector<u32>>& subCommRanks);
    ~InsTempAllGatherOmniPipeNHR() override;
    std::string Describe() const override
    {
        std::string info = "Template of all gather nhr (omniPipe) with tempRankSize ";
        info += std::to_string(templateRankSize_);
        return info;
    }
    HcclResult KernelRun(
        const OpParam& param, const TemplateDataParams& tempAlgParams,
        const TemplateResource& templateResource) override;
    HcclResult SetchannelsPerRank(const std::map<u32, std::vector<ChannelInfo>>& channels) override;

private:
    HcclResult RunAllGatherNHRChannel(
        const std::vector<ThreadHandle>& threads, const std::map<u32, std::vector<ChannelInfo>>& channels,
        const u32& channelIdx);

    HcclResult DoLastStepCopyNhr(
        const std::vector<ThreadHandle>& threads, const std::map<u32, std::vector<ChannelInfo>>& channels,
        const u32& channelIdx);

    u64 dataTypeSize_{0};
    std::vector<std::vector<std::vector<u64>>> dataSplitVec_;
    std::vector<std::vector<std::vector<u64>>> dataOffsetVec_;
    bool omniLastStepRead_ = false;
    bool lastStepNhrCopy_ = false;
};

} // namespace mc2_ops_hccl

#endif // INS_TEMP_ALL_GATHER_OMNIPIPE_NHR_H
