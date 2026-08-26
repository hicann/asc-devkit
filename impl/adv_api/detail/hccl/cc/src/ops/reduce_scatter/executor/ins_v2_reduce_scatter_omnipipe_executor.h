/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef MC2_HCCLV2_INS_V2_REDUCE_SCATTER_OMNIPIPE_EXECUTOR_H
#define MC2_HCCLV2_INS_V2_REDUCE_SCATTER_OMNIPIPE_EXECUTOR_H

#include "alg_param.h"
#include "topo_host.h"
#include "channel.h"
#include "alg_v2_template_base.h"
#include "utils.h"
#include "log.h"
#include "workflow.h"
#include "sal.h"
#include "config_log.h"
#include "executor_v2_base.h"
#include "coll_alg_v2_exec_registry.h"
#include "topo_match_base.h"
#include "topo_match_multilevel.h"
#include "topo_match_ubx.h"
#include "omnipipe_data_slice_calc.h"

namespace mc2_ops_hccl {
template <
    typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1,
    typename InsAlgTemplate2 = InsAlgTemplate1>
class InsV2ReduceScatterOmniPipeExecutor : public InsCollAlgBase {
public:
    explicit InsV2ReduceScatterOmniPipeExecutor();
    ~InsV2ReduceScatterOmniPipeExecutor() override = default;

    HcclResult Orchestrate(const OpParam& param, const AlgResourceCtxSerializable& resCtx) override;

    /* *************** 资源计算 *************** */
    // 这些函数为ExecutorBase纯虚函数，必须重写
    HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        const AlgHierarchyInfoForAllLevel& algHierarchyInfo, AlgResourceRequest& resourceRequest) override;

    HcclResult CalcAlgHierarchyInfo(
        HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo) override;
    HcclResult RestoreChannelMap(
        const AlgResourceCtxSerializable& resCtx,
        std::vector<std::map<u32, std::vector<ChannelInfo>>>& rankIdToChannelInfo) const;

protected:
    /* *************** 算法编排 *************** */
    HcclResult InitExectorInfo(const OpParam& param);
    HcclResult OrchestrateLoop(
        const OpParam& param, const AlgResourceCtxSerializable& resCtx,
        std::map<u32, std::shared_ptr<InsAlgTemplateBase>> tempMap);
    HcclResult InitCommInfo(
        const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        const AlgHierarchyInfoForAllLevel& algHierarchyInfo);
    HcclResult PrepareResForTemplateLevel(u32 level, std::shared_ptr<InsAlgTemplateBase>& tempBase);
    HcclResult GenTemplateAlgParamsByDimData(
        TemplateDataParams& tempAlgParams, const StepSliceInfo& stepSliceInfo) const;
    HcclResult CalcResLevel(
        HcclComm comm, const OpParam& param, const TopoInfo* topoInfo, std::shared_ptr<InsAlgTemplateBase> tempAlg,
        AlgResourceRequest& resourceRequest) const;

    uint64_t rankSizeLevel0_{0};
    uint64_t rankSizeLevel1_{0};
    uint64_t rankSizeLevel2_{0};

    uint64_t rankIdxLevel0_{0};
    uint64_t rankIdxLevel1_{0};
    uint64_t rankIdxLevel2_{0};
    std::vector<uint64_t> rankSizeLevel_;
    std::vector<uint64_t> rankIdxLevel_;
    ThreadHandle controlThread_;
    std::vector<std::vector<ThreadHandle>> levelThreads_;
    // xy两轴间同步使用
    std::vector<u32> notifyIdxCtrlToTempLevel01_;
    std::vector<u32> notifyIdxTempToCtrlLevel01_;
    // localcopy、轴间reduce使用
    std::vector<u32> notifyIdxCtrlToTempLevel0_;
    std::vector<u32> notifyIdxTempToCtrlLevel0_;
    std::vector<u32> notifyIdxCtrlToTempLevel1_;
    std::vector<u32> notifyIdxTempToCtrlLevel1_;
    std::vector<u32> notifyIdxCtrlToTempLevel2_;
    std::vector<u32> notifyIdxTempToCtrlLevel2_;

    std::vector<ThreadHandle> tempMainThreadsLevel0_;
    std::vector<ThreadHandle> tempMainThreadsLevel1_;
    std::vector<ThreadHandle> tempMainThreadsLevel2_;
    std::vector<ThreadHandle> tempMainThreadsLevel01_;

    std::vector<ThreadHandle> level0Threads_;
    std::vector<ThreadHandle> level1Threads_;
    std::vector<ThreadHandle> level2Threads_;

    AlgHierarchyInfoForAllLevel algHierarchyInfo_;
    std::vector<std::map<u32, std::vector<ChannelInfo>>> remoteRankToChannelInfo_;
    std::vector<ThreadHandle> threads_; // 相当于之前的std::vector<InsQuePtr> tempInsQue_;

    enum class TopoType { UBX_2LEVEL, THREE_LEVEL };
    TopoType topoType_ = TopoType::UBX_2LEVEL;

    HcclResult BuildSubCommAndTempMap(
        const OpParam& param, const AlgHierarchyInfoForAllLevel& algHierarchyInfo,
        std::vector<std::vector<u32>>& subCommRanks0, std::vector<std::vector<u32>>& subCommRanks1,
        std::vector<std::vector<u32>>& subCommRanks2, std::map<u32, std::shared_ptr<InsAlgTemplateBase>>& tempMap,
        const TopoInfoWithNetLayerDetails* topoInfo);

    std::vector<std::vector<u32>> subCommRanks0_;
    std::vector<std::vector<u32>> subCommRanks1_;
    std::vector<std::vector<u32>> subCommRanks2_;
    OmniNeedSetStepNum omniNeedSetStepNum_ = OmniNeedSetStepNum::OMNIPIPE_DEFAULT;
};
} // namespace mc2_ops_hccl

#endif
