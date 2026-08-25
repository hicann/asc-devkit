/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "channel.h"
#include "hccl_ccu_res.h"
#include "ccu_assist_pub.h"
#include "alg_data_trans_wrapper.h"
#include "kernel/ccu_kernel_kfc_server.h"
#include "ccu_temp_kfc_server.h"
#include "ccu_launch_dl.h"

namespace mc2_ops_hccl {
// 从源 CcuKernelInfo 继承属性到 KfcServer 的 CcuKernelInfo
void InheritKfcServerKernelArg(
    const CcuKernelInfo& srcKernel, CcuKernelInfo& dstKernel, const OpParam& param, uint32_t rankId,
    const std::vector<std::vector<u32>>& subCommRanks)
{
    std::string srcName(srcKernel.kernelFuncName);
    auto kfcArg = std::make_shared<CcuKernelArgKfcServer>();

    const bool isAllGather =
        srcName == "CcuKernelAllGatherMesh1DMem2Mem" && param.opType == HcclCMDType::HCCL_CMD_ALLGATHER;
    const bool isReduceScatter =
        srcName == "CcuKernelKfcReduceScatterMesh1DMem2Mem" && param.opType == HcclCMDType::HCCL_CMD_REDUCE_SCATTER;
    if (isAllGather || isReduceScatter) {
        kfcArg->rankSize = subCommRanks[0].size();
        kfcArg->rankId = rankId;
        kfcArg->loadFromMem = false;
        kfcArg->opParam = param;
        kfcArg->subCommRanks = subCommRanks;
    } else {
        HCCL_WARNING(
            "[InheritKfcServerKernelArg] unknown src kernel type[%s], no attributes inherited", srcName.c_str());
    }

    dstKernel.setKernelArg(kfcArg);
    // 继承源 kernelInfo 的 channels（由 CalcChannelRequestMesh1D 在 CalcRes 中赋值）
    dstKernel.channels = srcKernel.channels;
}

CcuTempKfcServer::CcuTempKfcServer(
    const OpParam& param, const u32 rankId, const std::vector<std::vector<u32>>& subCommRanks)
    : CcuAlgTemplateBase(param, rankId, subCommRanks)
{
    tempRankSize_ = subCommRanks[0].size();
    auto it = std::find(subCommRanks[0].begin(), subCommRanks[0].end(), rankId);
    if (it != subCommRanks[0].end()) {
        mySubCommRank_ = std::distance(subCommRanks[0].begin(), it);
    }
}

CcuTempKfcServer::~CcuTempKfcServer() {}

HcclResult CcuTempKfcServer::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    HCCL_INFO("[CcuTempKfcServer::CalcRes start]");
    if (param.opType != HcclCMDType::HCCL_CMD_ALLGATHER && param.opType != HcclCMDType::HCCL_CMD_REDUCE_SCATTER) {
        HCCL_ERROR("[CcuTempKfcServer::CalcRes] unsupported opType[%u]", static_cast<uint32_t>(param.opType));
        return HCCL_E_NOT_SUPPORT;
    }
    // 不需要从流
    resourceRequest.notifyNumOnMainThread = 0;
    resourceRequest.slaveThreadNum = 0;
    HCCL_DEBUG(
        "[CcuTempKfcServer::CalcRes] notifyNumOnMainThread[%u] slaveThreadNum[%u]",
        resourceRequest.notifyNumOnMainThread, resourceRequest.slaveThreadNum);

    // 构建 KfcServer kernelInfo
    CcuKernelInfo kernelInfo;
    CHK_SAFETY_FUNC_RET(strcpy_s(kernelInfo.kernelFuncName, sizeof(kernelInfo.kernelFuncName), "CcuKernelKfcServer"));
    kernelInfo.kernelFunc = reinterpret_cast<void*>(CcuKfcServerKernel);

    // 从 ccuKernelInfos 最后一项继承属性到当前 KfcServer kernel，然后替换最后一项
    if (!resourceRequest.ccuKernelInfos.empty()) {
        const std::string sourceName(resourceRequest.ccuKernelInfos.back().kernelFuncName);
        const bool isAllGather =
            sourceName == "CcuKernelAllGatherMesh1DMem2Mem" && param.opType == HcclCMDType::HCCL_CMD_ALLGATHER;
        const bool isReduceScatter = sourceName == "CcuKernelKfcReduceScatterMesh1DMem2Mem" &&
                                     param.opType == HcclCMDType::HCCL_CMD_REDUCE_SCATTER;
        if (!isAllGather && !isReduceScatter) {
            HCCL_ERROR(
                "[CcuTempKfcServer::CalcRes] unsupported source kernel[%s]",
                resourceRequest.ccuKernelInfos.back().kernelFuncName);
            return HCCL_E_NOT_SUPPORT;
        }
        HCCL_INFO(
            "[CcuTempKfcServer::CalcRes] inheriting kernelargs from[%s]",
            resourceRequest.ccuKernelInfos.back().kernelFuncName);
        InheritKfcServerKernelArg(
            resourceRequest.ccuKernelInfos.back(), kernelInfo, param, mySubCommRank_, subCommRanks_);
        resourceRequest.ccuKernelInfos.back() = kernelInfo;
    } else {
        HCCL_ERROR(
            "[CcuTempKfcServer::CalcRes] no preceding kernel to inherit from, KfcServer requires a source kernel");
        return HCCL_E_PARA;
    }

    HCCL_DEBUG(
        "[CcuTempKfcServer::CalcRes] dimsize=%llu, ccuKernelInfos.size()=%llu", subCommRanks_[0].size(),
        resourceRequest.ccuKernelInfos.size());

    return HcclResult::HCCL_SUCCESS;
}

HcclResult CcuTempKfcServer::KernelRun(
    const OpParam& param, const TemplateDataParams& templateDataParams, TemplateResource& templateResource)
{
    // 此处仅用于当CCU-KFC作为通信算子时使用
    uint64_t xnAddr = templateDataParams.ccuKfcArgs[0];
    uint64_t ckeAddr = templateDataParams.ccuKfcArgs[1];
    uint64_t workSpace = templateDataParams.ccuKfcArgs[2];
    uint64_t workSpaceSize = templateDataParams.ccuKfcArgs[3];
    uint64_t token = hcomm::CcuRep::GetTokenInfo(workSpace, workSpaceSize);

    // 临时用于参数测试
    uint64_t dieNum = 1;
    uint64_t missionNum = 1;
    uint64_t missionIndex = 0;

    std::unique_ptr<hcomm::CcuTaskArg> taskArg =
        std::make_unique<CcuTaskArgKfcServer>(xnAddr, ckeAddr, dieNum, missionNum, missionIndex, token);
    void* taskArgPtr = static_cast<void*>(taskArg.get());

    HCCL_INFO("[CcuTempKfcServer] KernelRun End.");
    return HcclResult::HCCL_SUCCESS;
}

u64 CcuTempKfcServer::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType)
{
    (void)inBuffType;
    (void)outBuffType;
    return 0;
}

} // namespace mc2_ops_hccl
