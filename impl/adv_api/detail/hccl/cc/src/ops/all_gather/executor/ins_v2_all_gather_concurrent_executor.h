/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCLV2_INS_ALL_GATHER_CONCURR_EXECUTOR_H
#define HCCLV2_INS_ALL_GATHER_CONCURR_EXECUTOR_H

#include "executor_common_ops.h"
#include "topo_match_base.h"
#include "topo_match_ubx.h"

namespace mc2_ops_hccl {
template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
class InsV2AllGatherConcurrentExecutor : public InsCollAlgBase {
public:
    explicit InsV2AllGatherConcurrentExecutor();
    ~InsV2AllGatherConcurrentExecutor() override = default;

    HcclResult Orchestrate(const OpParam& param, const AlgResourceCtxSerializable& resCtx) override;

    /* *************** 资源计算 *************** */
    HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        const AlgHierarchyInfoForAllLevel& algHierarchyInfo, AlgResourceRequest& resourceRequest) override;

    HcclResult CalcAlgHierarchyInfo(
        HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo) override;

private:
    /* *************** 算法编排 *************** */
    HcclResult OrchestrateLoop(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, InsAlgTemplate0& algTemplate0,
        InsAlgTemplate1& algTemplate1);

    HcclResult InitCommInfo(
        const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        const AlgHierarchyInfoForAllLevel& algHierarchyInfo);

    void GetParallelDataSplit(const OpParam& param, std::vector<float>& splitDataSize) const;

    void GenTemplateAlgParams(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, const u64 dataOffset,
        const u64 dataCountPerLoop, const u64 scratchOffset, TemplateDataParams& tempAlgParams) const;

    HcclResult PrepareResForTemplate(InsAlgTemplate0& algTemplate0, InsAlgTemplate1& algTemplate1);

    std::vector<ThreadHandle> threads_; // 相当于之前的std::vector<InsQuePtr> tempInsQue_;
    std::vector<ThreadHandle> tmp0Threads_;
    std::vector<ThreadHandle> tmp1Threads_;
    ThreadHandle mainThread_{0};
    std::vector<ThreadHandle> templateMainThreads_;
    std::vector<u32> syncNotifyOnTemplates_;
    std::vector<u32> syncNotifyOnMain_;

    AlgHierarchyInfoForAllLevel algHierarchyInfo_;
    std::vector<std::map<u32, std::vector<ChannelInfo>>> remoteRankToChannelInfo_;
    std::map<u32, std::vector<ChannelInfo>> tmp0LinkMap_;
    std::map<u32, std::vector<ChannelInfo>> tmp1LinkMap_;
    std::vector<CcuKernelHandle> tmp0CcuKernels_;
    std::vector<CcuKernelHandle> tmp1CcuKernels_;
};
} // namespace mc2_ops_hccl

#endif // HCCLV2_INS_ALL_GATHER_CONCURR_EXECUTOR_H
