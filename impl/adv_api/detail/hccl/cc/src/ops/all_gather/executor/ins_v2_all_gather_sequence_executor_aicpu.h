/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCLV2_INS_V2_ALL_GATHER_AICPU_SEQUENCE_EXECUTOR_H
#define HCCLV2_INS_V2_ALL_GATHER_AICPU_SEQUENCE_EXECUTOR_H

#include "executor_common_ops.h"
#include "topo_match_base.h"
#include "topo_match_multilevel.h"

namespace mc2_ops_hccl {
template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
class InsV2AllGatherSequenceExecutorAicpu : public InsCollAlgBase {
public:
    InsV2AllGatherSequenceExecutorAicpu() {}
    ~InsV2AllGatherSequenceExecutorAicpu() override {}

    HcclResult CalcAlgHierarchyInfo(
        HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo) override;

    HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        const AlgHierarchyInfoForAllLevel& algHierarchyInfo, AlgResourceRequest& resourceRequest) override;

    HcclResult Orchestrate(const OpParam& param, const AlgResourceCtxSerializable& resCtx) override;

private:
    HcclResult InitCommInfo(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        const AlgHierarchyInfoForAllLevel& algHierarchyInfo);
    void GenInterTemplateParams(
        TemplateDataParams& interTempDataParams, const u64 processedDataCount, const u64 currDataCount,
        const u64 loop) const;
    void GenIntraTemplateParams(
        TemplateDataParams& intraTempDataParams, const u64 processedDataCount, const u64 currDataCount,
        const u64 loop) const;
    HcclResult OrchestrateLoop(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, InsAlgTemplate0& intraTempAlg,
        InsAlgTemplate1& interTempAlg);
    HcclResult SplitData(const u64 dataCount, const u64 rankSize, TemplateDataParams& tempAlgParams);
    template <typename InsAlgTemplate>
    HcclResult GenTempResource(
        const AlgResourceCtxSerializable& resCtx, const u32 channelLevelIdx, const InsAlgTemplate& algTemplate,
        TemplateResource& tempReousrce) const;

    uint32_t rankSizeLevel0_{0};
    uint32_t rankSizeLevel1_{0};
    uint32_t rankIdxLevel0_{0};
    uint32_t rankIdxLevel1_{0};

    AlgHierarchyInfoForAllLevel algHierarchyInfo_;
    std::vector<ThreadHandle> threads_;
    std::vector<std::map<u32, std::vector<ChannelInfo>>> remoteRankToChannelInfo_;
};
} // namespace mc2_ops_hccl

#endif
