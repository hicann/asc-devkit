/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef EXECUTOR_BASE_V2_H
#define EXECUTOR_BASE_V2_H

#include "alg_param.h"
#include "topo_host.h"
#include "channel.h"
#include "alg_template_base.h"
#include "alg_template_register.h"
#include "utils.h"
#include "log.h"
#include "workflow.h"
#include "sal.h"
#include "executor_base.h"
#include "template_utils.h"

namespace mc2_ops_hccl {

class InsCollAlgBase {
public:
    InsCollAlgBase();
    virtual ~InsCollAlgBase();

    virtual std::string Describe() const;

    virtual HcclResult CalcAlgHierarchyInfo(
        HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo, AlgHierarchyInfoForAllLevel& algHierarchyInfo) = 0;

    virtual HcclResult CalcRes(
        HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
        const AlgHierarchyInfoForAllLevel& algHierarchyInfo, AlgResourceRequest& resourceRequest) = 0;

    // device
    virtual HcclResult Orchestrate(const OpParam& param, const AlgResourceCtxSerializable& resCtx) = 0;

    virtual HcclResult FastLaunch(const OpParam& param, const CcuFastLaunchCtx* resCtx);

    HcclResult SetTempFastLaunchAddr(
        TemplateFastLaunchCtx& tempFastLaunchCtx, void* inputPtr, void* outputPtr, const HcclMem& hcclBuff) const;

    HcclResult RestoreChannelMap(
        const AlgResourceCtxSerializable& resCtx,
        std::vector<std::map<u32, std::vector<ChannelInfo>>>& rankIdToChannelInfo) const;

protected:
    // CollAlg base params
    u32 myRank_ = INVALID_VALUE_RANKID;
    u32 rankSize_ = 0;
    DevType devType_ = DevType::DEV_TYPE_COUNT;

    // opInfo
    HcclReduceOp reduceOp_;
    u32 root_ = INVALID_VALUE_RANKID;
    // dataInfo
    HcclDataType dataType_;
    u64 dataCount_ = 0;
    u64 strideCount_ = 0;

    u64 maxTmpMemSize_ = 0;

    // dataSize
    u64 dataSize_ = 0;
    u64 dataTypeSize_ = 0;
};

} // namespace mc2_ops_hccl

#endif // !HCCLV2_INS_COLL_ALG_BASE
