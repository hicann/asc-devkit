/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCLV2_INS_V2_ALL_TO_ALL_V_SOLE_EXECUTOR_H
#define HCCLV2_INS_V2_ALL_TO_ALL_V_SOLE_EXECUTOR_H

#include "executor_common_ops.h"

namespace mc2_ops_hccl {

template <typename AlgTopoMatch, typename InsAlgTemplate>
class InsV2AlltoAllVSoleExecutor : public InsCollAlgBase {
public:
    explicit InsV2AlltoAllVSoleExecutor() = default;
    ~InsV2AlltoAllVSoleExecutor() override = default;

    HcclResult Orchestrate(const OpParam& param, const AlgResourceCtxSerializable& resCtx) override;
    HcclResult CalcAlgHierarchyInfo(
        HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo) override;
    HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        const AlgHierarchyInfoForAllLevel& algHierarchyInfo, AlgResourceRequest& resourceRequest) override;

private:
    HcclResult OrchestrateLoop(const OpParam& param, const AlgResourceCtxSerializable& resCtx);
    HcclResult ValidateAndInit(const OpParam& param, const AlgResourceCtxSerializable& resCtx);

    std::vector<std::map<u32, std::vector<ChannelInfo>>> remoteRankToChannelInfo_;
};

} // namespace mc2_ops_hccl
#endif
