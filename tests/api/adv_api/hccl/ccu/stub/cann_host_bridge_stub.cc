/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "cann_host_bridge.h"
#include "cann_host_bridge_stub.h"

namespace mc2_ops_hccl {

CannBridgeTestState g_cannBridgeTestState;

bool UseCannBridge(const OpParam& param)
{
    return g_cannBridgeTestState.enabled &&
           (param.engine == COMM_ENGINE_AICPU_TS || param.engine == COMM_ENGINE_AICPU) &&
           (param.opType == HcclCMDType::HCCL_CMD_ALLREDUCE || param.opType == HcclCMDType::HCCL_CMD_ALLTOALL ||
            param.opType == HcclCMDType::HCCL_CMD_ALLTOALLV);
}

class ForcedAlgBridgeExecutor : public InsCollAlgBase {
public:
    explicit ForcedAlgBridgeExecutor(const std::string& algName) : algName_(algName) {}

    AlgAttrs GetAlgoMeta(const std::string& algName) const override
    {
        g_cannBridgeTestState.metadataName = algName;
        AlgAttrs attrs;
        attrs.name = algName;
        // Two distinct levels model the metadata required by the parallel executor.
        attrs.algoTypes = {static_cast<AlgoType>(1), static_cast<AlgoType>(2)};
        return attrs;
    }

    HcclResult CalcAlgHierarchyInfo(HcclComm, TopoInfoWithNetLayerDetails*, AlgHierarchyInfoForAllLevel&) override
    {
        ++g_cannBridgeTestState.legacyCalls;
        return HCCL_E_PARA; // Reproduce the old entry's empty-AlgAttrs failure.
    }

    HcclResult CalcAlgHierarchyInfoV2(
        TopoInfoWithNetLayerDetails*, AlgHierarchyInfoForAllLevel& hierarchy, const AlgAttrs& attrs) override
    {
        ++g_cannBridgeTestState.v2Calls;
        if (attrs.name != algName_ ||
            attrs.algoTypes != std::vector<AlgoType>{static_cast<AlgoType>(1), static_cast<AlgoType>(2)}) {
            return HCCL_E_PARA;
        }
        hierarchy.infos.resize(2);
        return g_cannBridgeTestState.hierarchyResult;
    }

    HcclResult CalcRes(
        HcclComm, const OpParam&, const TopoInfoWithNetLayerDetails*, const AlgHierarchyInfoForAllLevel& hierarchy,
        AlgResourceRequest&) override
    {
        ++g_cannBridgeTestState.resourceCalls;
        return hierarchy.infos.size() == 2 ? g_cannBridgeTestState.resourceResult : HCCL_E_PARA;
    }

    HcclResult Orchestrate(const OpParam&, const AlgResourceCtxSerializable&) override { return HCCL_SUCCESS; }

private:
    std::string algName_;
};

std::unique_ptr<InsCollAlgBase> GetAlgExecViaCann(HcclCMDType opType, const std::string& algName)
{
    ++g_cannBridgeTestState.lookupCalls;
    if (!g_cannBridgeTestState.enabled || g_cannBridgeTestState.missingExecutor) {
        return nullptr;
    }
    return std::make_unique<ForcedAlgBridgeExecutor>(algName);
}

} // namespace mc2_ops_hccl
