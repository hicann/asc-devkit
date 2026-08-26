/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCLV2_INS_V2_ALL_TO_ALL_CONCURRENT_EXECUTOR_H
#define HCCLV2_INS_V2_ALL_TO_ALL_CONCURRENT_EXECUTOR_H

#include "executor_common_ops.h"
#include "topo_match_ubx.h"

namespace mc2_ops_hccl {

template <typename AlgTopoMatch, typename InsAlgTemplate0, typename InsAlgTemplate1>
class InsV2AllToAllConcurrentExecutor : public InsCollAlgBase {
public:
    InsV2AllToAllConcurrentExecutor() = default;
    ~InsV2AllToAllConcurrentExecutor() override = default;

    HcclResult CalcAlgHierarchyInfo(
        HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo) override;
    HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        const AlgHierarchyInfoForAllLevel& algHierarchyInfo, AlgResourceRequest& resourceRequest) override;
    HcclResult Orchestrate(const OpParam& param, const AlgResourceCtxSerializable& resCtx) override;

private:
    struct SendRecvData {
        std::vector<u64> sendCounts;
        std::vector<u64> recvCounts;
        std::vector<u64> sdispls;
        std::vector<u64> rdispls;
    };

    HcclResult ValidateParam(const OpParam& param, u32 rankSize, u32 myRank) const;
    HcclResult RestoreChannels(
        const AlgResourceCtxSerializable& resCtx, const std::vector<u32>& meshRanks, const std::vector<u32>& closRanks);
    HcclResult SplitSendRecvData(const OpParam& param, std::vector<SendRecvData>& splitData) const;
    HcclResult CalcMaxDataCountPerLoop(
        const std::vector<u64>& scratchMultipliers, std::vector<u64>& maxDataCountPerLoop) const;
    HcclResult SetTemplateDataParams(
        TemplateDataParams& params, const SendRecvData& splitData, u64 currentCount, u64 processedCount,
        u64 maxDataCountPerLoop) const;
    HcclResult OrchestrateLoop(const OpParam& param, const AlgResourceCtxSerializable& resCtx);

    AlgHierarchyInfoForAllLevel algHierarchyInfo_;
    std::vector<std::map<u32, std::vector<ChannelInfo>>> templateChannels_;
    std::vector<ThreadHandle> template0Threads_;
    std::vector<ThreadHandle> template1Threads_;
};

} // namespace mc2_ops_hccl
#endif
