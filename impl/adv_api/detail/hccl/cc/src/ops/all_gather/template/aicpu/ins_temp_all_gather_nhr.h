/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef INS_TEMP_ALL_GATHER_NHR_H
#define INS_TEMP_ALL_GATHER_NHR_H

#include "alg_v2_template_base.h"
#include "executor_base.h"

namespace mc2_ops_hccl {

class InsTempAllGatherNHR : public InsAlgTemplateBase {
public:
    InsTempAllGatherNHR() = default;
    explicit InsTempAllGatherNHR(
        const OpParam& param, const u32 rankId, const std::vector<std::vector<u32>>& subCommRanks);
    ~InsTempAllGatherNHR() override;

    std::string Describe() const override
    {
        std::string info = "Template of all gather nhr with tempRankSize ";
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
    HcclResult SetchannelsPerRank(const std::map<u32, std::vector<ChannelInfo>>& channels) override;
    void GetNotifyIdxMainToSub(std::vector<u32>& notifyIdxMianToSub) override;
    void GetNotifyIdxSubToMain(std::vector<u32>& notifyIdxSubToMain) override;

protected:
    virtual HcclResult GetStepInfo(u32 step, u32 nSteps, AicpuNHRStepInfo& stepInfo);
    u32 GetRankFromMap(const u32 algRankIdx) const;
    TemplateDataParams tempAlgParams_;
    bool isDmaRead_{false};

private:
    HcclResult ValidateKernelRunParams(const TemplateResource& templateResource) const;
    bool CanReadLastStepToOutput() const;
    bool CanSkipOwnSliceCopy() const;
    bool IsLastStepReadSlice(u32 algRank) const;
    HcclResult PrepareDataSplitForMultiChannel(const TemplateResource& templateResource);
    HcclResult LocalDataCopy(const std::vector<ThreadHandle>& threads, const u32& channelIdx);
    HcclResult PostLocalCopy(const ThreadHandle& thread, const u32& channelIdx);
    HcclResult RunAllGatherNHR(
        const std::vector<ThreadHandle>& threads, const std::map<u32, std::vector<ChannelInfo>>& channels,
        const u32& channelIdx, bool& postLocalCopyLaunched);
    struct SliceCalcInfo {
        u32 txIdx;
        u32 rxIdx;
        u64 txPartialOffset;
        u64 rxPartialOffset;
        u64 txScratchOff;
        u64 rxScratchOff;
        u64 txSliceSize;
        u64 rxSliceSize;
        u64 scratchBase;
    };
    SliceCalcInfo CalcSliceInfo(const AicpuNHRStepInfo& stepInfo, u32 rpt, u32 i, u32 channelIdx) const;
    enum class StepBuildMode { NORMAL, LAST_STEP_READ_TO_OUTPUT };
    HcclResult BuildStepSlices(
        const ChannelInfo& channelSend, const ChannelInfo& channelRecv, const AicpuNHRStepInfo& stepInfo,
        const u32& channelIdx, StepBuildMode mode, std::vector<DataSlice>& txSrcSlices,
        std::vector<DataSlice>& txDstSlices, std::vector<DataSlice>& rxSrcSlices, std::vector<DataSlice>& rxDstSlices);
    HcclResult RunStepNHR(
        const std::vector<ThreadHandle>& threads, const std::map<u32, std::vector<ChannelInfo>>& channels,
        const u32& channelIdx, u32 step, u32 nSteps, bool& postLocalCopyLaunched);
    HcclResult BuildLastStepReadToOutputSlices(
        const ChannelInfo& channelSend, const ChannelInfo& channelRecv, const AicpuNHRStepInfo& stepInfo,
        const u32& channelIdx, std::vector<DataSlice>& txSrcSlices, std::vector<DataSlice>& txDstSlices,
        std::vector<DataSlice>& rxSrcSlices, std::vector<DataSlice>& rxDstSlices)
    {
        return BuildStepSlices(
            channelSend, channelRecv, stepInfo, channelIdx, StepBuildMode::LAST_STEP_READ_TO_OUTPUT, txSrcSlices,
            txDstSlices, rxSrcSlices, rxDstSlices);
    }
    HcclResult BuildNormalStepSlices(
        const ChannelInfo& channelSend, const ChannelInfo& channelRecv, const AicpuNHRStepInfo& stepInfo,
        const u32& channelIdx, std::vector<DataSlice>& txSrcSlices, std::vector<DataSlice>& txDstSlices,
        std::vector<DataSlice>& rxSrcSlices, std::vector<DataSlice>& rxDstSlices)
    {
        return BuildStepSlices(
            channelSend, channelRecv, stepInfo, channelIdx, StepBuildMode::NORMAL, txSrcSlices, txDstSlices,
            rxSrcSlices, rxDstSlices);
    }
    HcclResult RunLastStepReadToOutput(
        const std::vector<ThreadHandle>& threads, const ChannelInfo& channelSend, const ChannelInfo& channelRecv,
        const AicpuNHRStepInfo& stepInfo, const u32& channelIdx, u32 step, bool& postLocalCopyLaunched);
    bool readLastStepToOutput_{false};
    bool skipOwnSliceCopy_{false};
    std::vector<u32> lastStepReadSliceIdxs_;
    u64 dataTypeSize_{0};
    u32 maxChannelsPerRank_{1};
    std::vector<u64> dataSplit_;
    std::vector<u64> dataOffset_;
    std::vector<u64> dataSplitTail_;
    std::vector<u64> dataOffsetTail_;
};

} // namespace mc2_ops_hccl

#endif // INS_TEMP_ALL_GATHER_NHR_H
