/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef INS_TEMP_ALL_REDUCE_NHR
#define INS_TEMP_ALL_REDUCE_NHR

#include "alg_v2_template_base.h"
#include "executor_base.h"
#include "alg_data_trans_wrapper.h"

namespace mc2_ops_hccl {

#ifndef NHR_STEP_INFO_DEFINED
#define NHR_STEP_INFO_DEFINED
struct NHRStepInfo {
    u32 step = 0;
    u32 myRank = 0;
    u32 nSlices;
    u32 toRank = 0;
    u32 fromRank = 0;
    std::vector<u32> txSliceIdxs;
    std::vector<u32> rxSliceIdxs;

    NHRStepInfo() : nSlices(0) {}
};
#endif

struct NHRSliceInfo {
    u64 offset{0};
    u64 size{0};
    u64 count{0};

    NHRSliceInfo(const u64 offset, const u64 size, const u64 count) : offset(offset), size(size), count(count) {}
};

class InsTempAllReduceNHR : public InsAlgTemplateBase {
public:
    InsTempAllReduceNHR() = default;
    explicit InsTempAllReduceNHR(
        const OpParam& param, const u32 rankId, const std::vector<std::vector<u32>>& subCommRanks);
    ~InsTempAllReduceNHR() override;

    std::string Describe() const override
    {
        std::string info = "Template of all reduce NHR with tempRankSize ";
        info += std::to_string(templateRankSize_);
        return info;
    }

    u64 CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType) override;
    HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        AlgResourceRequest& resourceRequest) override;
    HcclResult GetRes(AlgResourceRequest& resourceRequest) const override;
    u64 GetThreadNum() const override;

    void GetNotifyIdxMainToSub(std::vector<u32>& notifyIdxMainToSub) override;
    void GetNotifyIdxSubToMain(std::vector<u32>& notifyIdxSubToMain) override;

    HcclResult KernelRun(
        const OpParam& param, const TemplateDataParams& tempAlgParams,
        const TemplateResource& templateResource) override;

private:
    HcclResult PrepareDataSplitForMultiChannel(const TemplateResource& templateResource);

    HcclResult PreCopy(const TemplateDataParams& tempAlgParams, const std::vector<ThreadHandle>& threads) const;
    HcclResult RunReduceScatter(
        const TemplateDataParams& tempAlgParams, const std::map<u32, std::vector<ChannelInfo>>& channels,
        const std::vector<ThreadHandle>& threads, u32 channelIdx);
    HcclResult RunAllGather(
        const TemplateDataParams& tempAlgParams, const std::map<u32, std::vector<ChannelInfo>>& channels,
        const std::vector<ThreadHandle>& threads, u32 channelIdx);
    HcclResult PostCopy(const TemplateDataParams& tempAlgParams, const std::vector<ThreadHandle>& threads) const;

    HcclResult GetReduceScatterStepInfoList(std::vector<NHRStepInfo>& stepInfoList) const;
    HcclResult GetAllGatherStepInfoList(std::vector<NHRStepInfo>& stepInfoList) const;
    u32 GetNHRStepNum() const;

    TemplateDataParams tempAlgParams_;
    u32 dataTypeSize_{0};
    u64 count_{0};
    u64 processSize_{0};
    u64 sliceSize_{0};
    u64 tailSize_{0};

    u32 myRankIdx_{0};
    std::vector<u32> rankList_;

    bool isDmaRead_{false};
    std::vector<u64> dataSplit_;
    std::vector<u64> dataOffset_;
    std::vector<u64> dataSplitTail_;
    std::vector<u64> dataOffsetTail_;
};

} // namespace mc2_ops_hccl

#endif // INS_TEMP_ALL_REDUCE_NHR
