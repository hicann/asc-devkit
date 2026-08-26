/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef INS_V2_ALL_GATHER_OMNIPIPE_EXECUTOR_H
#define INS_V2_ALL_GATHER_OMNIPIPE_EXECUTOR_H

#include "alg_param.h"
#include "channel.h"
#include "alg_v2_template_base.h"
#include "executor_v2_base.h"
#include "coll_alg_v2_exec_registry.h"
#include "omnipipe_data_slice_calc.h"

namespace mc2_ops_hccl {
template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1,
    typename InsAlgTemplate2 = InsAlgTemplate1>
class InsV2AllGatherOmniPipeExecutor : public InsCollAlgBase {
public:
    explicit InsV2AllGatherOmniPipeExecutor();
    ~InsV2AllGatherOmniPipeExecutor() override = default;

    HcclResult Orchestrate(const OpParam& param, const AlgResourceCtxSerializable& resCtx) override;

    HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        const AlgHierarchyInfoForAllLevel& algHierarchyInfo, AlgResourceRequest& resourceRequest) override;

    HcclResult CalcAlgHierarchyInfo(
        HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo) override;

protected:
    HcclResult GenTemplateAlgParamsByDimData(TemplateDataParams& tempAlgParams, StepSliceInfo& stepSliceInfo) const;

    HcclResult InitCommInfo(
        const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        const AlgHierarchyInfoForAllLevel& algHierarchyInfo);
    HcclResult OrchestrateLoop(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx,
        std::map<u32, std::shared_ptr<InsAlgTemplateBase>>& tempMap);
    HcclResult RestoreChannelMap(
        const AlgResourceCtxSerializable& resCtx,
        std::vector<std::map<u32, std::vector<ChannelInfo>>>& rankIdToChannelInfo) const;
    HcclResult PrepareResForTemplateLevel(u32 level, std::shared_ptr<InsAlgTemplateBase>& tempBase);
    HcclResult CalcResLevel(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        std::shared_ptr<InsAlgTemplateBase> tempAlg, AlgResourceRequest& resourceRequest) const;

    HcclResult UbxLastStepLocalCopy(
        const OpParam& param, const OmniPipeSliceInfo& omniPipeSliceInfo,
        const OmniPipeSliceInfo& omniPipeSliceLocalcopyInfo, std::map<u32, TemplateDataParams>& tempAlgParamMap,
        const u64 processedDataCount, int step) const;

    HcclResult UbxLocalCopy(
        const OpParam& param, const OmniPipeSliceInfo& omniPipeSliceInfo,
        const OmniPipeSliceInfo& omniPipeSliceLocalcopyInfo, std::map<u32, TemplateDataParams>& tempAlgParamMap,
        const u64 processedDataCount, int step) const;

private:
    enum class TopoType { UBX_2LEVEL, THREE_LEVEL };
    TopoType topoType_ = TopoType::UBX_2LEVEL;

    HcclResult BuildSubCommAndTempMap(
        const OpParam& param, const AlgHierarchyInfoForAllLevel& algHierarchyInfo,
        std::vector<std::vector<u32>>& subCommRanks0, std::vector<std::vector<u32>>& subCommRanks1,
        std::vector<std::vector<u32>>& subCommRanks2, std::map<u32, std::shared_ptr<InsAlgTemplateBase>>& tempMap,
        const TopoInfoWithNetLayerDetails* topoInfo);
    std::vector<uint64_t> rankSizeLevel_;
    std::vector<uint64_t> rankIdxLevel_;
    OpMode opMode_ = OpMode::OPBASE;

    ThreadHandle controlThread_;
    std::vector<ThreadHandle> templateMainThreads_;
    std::vector<ThreadHandle> threads_;
    std::vector<std::map<u32, std::vector<ChannelInfo>>> remoteRankToChannelInfo_;

    std::vector<u32> ntfIdxCtrlToTempXY_;
    std::vector<u32> ntfIdxCtrlToTempZ_;
    std::vector<u32> ntfIdxTempToCtrlXY_;
    std::vector<u32> ntfIdxTempToCtrlZ_;

    std::vector<std::vector<ThreadHandle>> levelThreads_;

    std::vector<ThreadHandle> tempMainThreadsXY_;
    std::vector<ThreadHandle> tempMainThreadsZ_;
    AlgHierarchyInfoForAllLevel algHierarchyInfo_;

    std::vector<std::vector<u32>> subCommRanks0_;
    std::vector<std::vector<u32>> subCommRanks1_;
    std::vector<std::vector<u32>> subCommRanks2_;

    OmniNeedSetStepNum omniNeedSetStepNum_ = OmniNeedSetStepNum::OMNIPIPE_DEFAULT;
    bool omniUbxLastStepRead_ = false;
};
} // namespace mc2_ops_hccl

#endif
