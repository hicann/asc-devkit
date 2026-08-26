/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCLV2_INS_V2_REDUCE_SCATTER_PARALLEL_EXECUTOR_H
#define HCCLV2_INS_V2_REDUCE_SCATTER_PARALLEL_EXECUTOR_H

#include "executor_common_ops.h"
#include "topo_match_base.h"
#include "topo_match_multilevel.h"
#include "topo_match_ubx.h"
#include "topo_match_pcie_mix.h"

namespace mc2_ops_hccl {

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
class InsReduceScatterParallelExecutor : public InsCollAlgBase {
public:
    explicit InsReduceScatterParallelExecutor();
    ~InsReduceScatterParallelExecutor() override;

    std::string Describe() const override { return "Instruction based Reduce Scatter Parallel Executor."; }

    // HOST 接口
    HcclResult Orchestrate(const OpParam& param, const AlgResourceCtxSerializable& resCtx) override;

    /* *************** 资源计算 *************** */

    HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        const AlgHierarchyInfoForAllLevel& algHierarchyInfo, AlgResourceRequest& resourceRequest) override;

    HcclResult CalcAlgHierarchyInfo(
        HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo) override;

private:
    HcclResult OrchestrateLoop(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, InsAlgTemplate0& tempAlgIntra,
        InsAlgTemplate1& tempAlgInter);
    void GenTemplateAlgParamsIntra0(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, const u64 dataOffset,
        const u64 dataCountPerLoopAixs0, std::vector<u64>& scratchOffVec,
        TemplateDataParams& tempAlgParamsIntra0) const;
    void GenTemplateAlgParamsIntra1(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, const u64 dataOffset,
        const u64 dataCountPerLoopAixs1, std::vector<u64>& scratchOffVec,
        TemplateDataParams& tempAlgParamsIntra1) const;
    void GenTemplateAlgParamsInter0(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, const u64 dataOffset,
        const u64 dataCountPerLoopAixs0, std::vector<u64>& scratchOffVec,
        TemplateDataParams& tempAlgParamsInter0) const;
    void GenTemplateAlgParamsInter1(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx, const u64 dataOffset,
        const u64 dataCountPerLoopAixs1, std::vector<u64>& scratchOffVec,
        TemplateDataParams& tempAlgParamsInter1) const;
    void GetParallelDataSplit(std::vector<float>& splitDataSize) const;
    HcclResult PrepareResForTemplate(const InsAlgTemplate0& tempAlgIntra, const InsAlgTemplate1& tempAlgInter);
    uint64_t GetRankSize(const std::vector<std::vector<u32>>& subCommRanks) const;

    uint64_t rankSizeLevel0_{0};
    uint64_t rankSizeLevel1_{0};

    uint64_t rankIdxLevel0_{0};
    uint64_t rankIdxLevel1_{0};

    ThreadHandle controlThread_{0};
    std::vector<ThreadHandle> templateMainThreads_;
    std::vector<u32> notifyIdxControlToTemplates_;
    std::vector<u32> notifyIdxTemplatesToControl_;
    std::vector<ThreadHandle> intraThreads_;
    std::vector<ThreadHandle> interThreads_;
    std::map<u32, std::vector<ChannelInfo>> intraChannelMap_;
    std::map<u32, std::vector<ChannelInfo>> interChannelMap_;
    std::vector<ThreadHandle> threads_;
    std::vector<std::map<u32, std::vector<ChannelInfo>>> remoteRankToChannelInfo_;
};

} // namespace mc2_ops_hccl

#endif // HCCLV2_INS_REDUCE_SCATTER_PARALLEL_EXECUTOR_H
