/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef MC2_OPS_HCCL_CANN_DISPATCH_BRIDGE_H
#define MC2_OPS_HCCL_CANN_DISPATCH_BRIDGE_H

#include <string>
#include "alg_param.h"

namespace mc2_ops_hccl {
namespace cann_abi {

class InsCollAlgBase {
public:
    virtual ~InsCollAlgBase() {}
    virtual std::string Describe() const = 0;                                     // slot: Describe
    virtual void* CalcCostCoeffAbiPlaceholder(void*, void*, const char*) = 0;     // slot: CalcCostCoeff
    virtual void* GetAlgNetMetaAbiPlaceholder(void*) const = 0;                   // slot: GetAlgNetMeta
    virtual int CalcAlgHierarchyInfo(void*, void*, void*) = 0;                    // slot: CalcAlgHierarchyInfo
    virtual int CalcRes(void*, const void*, const void*, const void*, void*) = 0; // slot: CalcRes
    virtual HcclResult Orchestrate(
        const OpParam& param,
        const AlgResourceCtxSerializable& resCtx) = 0; // slot: Orchestrate
};

} // namespace cann_abi

HcclResult LaunchViaCann(const OpParam& ascParam, const AlgResourceCtxSerializable& ascResCtx);

} // namespace mc2_ops_hccl

#endif // MC2_OPS_HCCL_CANN_KERNEL_BRIDGE_H
