/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "template_utils.h"
#include "ins_kfc_server_sole_executor.h"
#ifndef AICPU_COMPILE
#if !defined(HCCL_CANN_COMPAT_850)
#include "ccu_temp_kfc_server.h"
#endif
#endif

namespace mc2_ops_hccl {

template <typename AlgTopoMatch, typename InsAlgTemplate>
InsKfcServerSoleExecutor<AlgTopoMatch, InsAlgTemplate>::InsKfcServerSoleExecutor()
{}

template <typename AlgTopoMatch, typename InsAlgTemplate>
HcclResult InsKfcServerSoleExecutor<AlgTopoMatch, InsAlgTemplate>::CalcAlgHierarchyInfo(
    HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo)
{
    AlgTopoMatch topoMatch;
    CHK_RET(topoMatch.MatchTopo(comm, topoInfo, algHierarchyInfo));
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate>
HcclResult InsKfcServerSoleExecutor<AlgTopoMatch, InsAlgTemplate>::InitCommInfo(
    const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo)
{
    HCCL_INFO("[InsKfcServerSoleExecutor][InitCommInfo]");
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate>
HcclResult InsKfcServerSoleExecutor<AlgTopoMatch, InsAlgTemplate>::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    const AlgHierarchyInfoForAllLevel& algHierarchyInfo, AlgResourceRequest& resourceRequest)
{
    CHK_RET(InitCommInfo(param, topoInfo));

    std::vector<std::vector<u32>> tempAlgHierachyInfo = algHierarchyInfo.infos[0];

    std::shared_ptr<InsAlgTemplate> algTemplate =
        std::make_shared<InsAlgTemplate>(param, topoInfo->userRank, tempAlgHierachyInfo);
    algTemplate->CalcRes(comm, param, topoInfo, resourceRequest);

    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate>
HcclResult InsKfcServerSoleExecutor<AlgTopoMatch, InsAlgTemplate>::Orchestrate(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx)
{
    HCCL_INFO("[InsKfcServerSoleExecutor][Orchestrate] Orchestrate Start");

    CHK_RET(InitCommInfo(param, &resCtx.topoInfo));

    threads_ = resCtx.threads;
    if (param.engine != CommEngine::COMM_ENGINE_CCU) {
        CHK_RET(RestoreChannelMap(resCtx, remoteRankToChannelInfo_));
    }

    HcclResult ret = OrchestrateLoop(param, resCtx);
    CHK_PRT_RET(
        ret != HCCL_SUCCESS,
        HCCL_ERROR(
            "[InsKfcServerSoleExecutor][Orchestrate]errNo[0x%016llx] KfcServer excutor kernel run failed",
            HCCL_ERROR_CODE(ret)),
        ret);
    return HCCL_SUCCESS;
}

template <typename AlgTopoMatch, typename InsAlgTemplate>
HcclResult InsKfcServerSoleExecutor<AlgTopoMatch, InsAlgTemplate>::OrchestrateLoop(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx)
{
    HCCL_INFO("[InsKfcServerSoleExecutor][OrchestrateLoop] Start");
    // TemplateResource参数计算
    TemplateResource templateAlgRes;
    if (remoteRankToChannelInfo_.size() > 0) {
        templateAlgRes.channels = remoteRankToChannelInfo_[0];
    }
    if (param.engine == COMM_ENGINE_CCU) {
        templateAlgRes.ccuKernels = resCtx.ccuKernels;
    }

    templateAlgRes.threads = resCtx.threads;

    // 准备数据
    TemplateDataParams tempAlgParams;
    tempAlgParams.ccuKfcArgs.push_back(param.ccuKfcArgs[0]);
    tempAlgParams.ccuKfcArgs.push_back(param.ccuKfcArgs[1]);
    tempAlgParams.ccuKfcArgs.push_back(param.ccuKfcArgs[2]);
    tempAlgParams.ccuKfcArgs.push_back(param.ccuKfcArgs[3]);

    // 构建template
    std::shared_ptr<InsAlgTemplate> algTemplate =
        std::make_shared<InsAlgTemplate>(param, resCtx.topoInfo.userRank, resCtx.algHierarchyInfo.infos[0]);
    CHK_RET(algTemplate->KernelRun(param, tempAlgParams, templateAlgRes));

    HCCL_INFO("[InsKfcServerSoleExecutor][OrchestrateLoop] End.");
    return HCCL_SUCCESS;
}

#ifndef AICPU_COMPILE
#if !defined(HCCL_CANN_COMPAT_850)
REGISTER_EXEC_V2(
    HcclCMDType::HCCL_CMD_KFC_SERVER, CcuKfcServer, InsKfcServerSoleExecutor, TopoMatch1D, CcuTempKfcServer);
#endif
#endif

} // namespace mc2_ops_hccl
