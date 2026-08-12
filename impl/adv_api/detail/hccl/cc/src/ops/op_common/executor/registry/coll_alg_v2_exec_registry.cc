/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "coll_alg_v2_exec_registry.h"

namespace mc2_ops_hccl {

CollAlgExecRegistryV2& CollAlgExecRegistryV2::Instance()
{
    static CollAlgExecRegistryV2 globalExecRegistry;
    return globalExecRegistry;
}

HcclResult CollAlgExecRegistryV2::Register(
    const HcclCMDType type, const std::string& tag, const CollExecCreatorV2& collExecCreator)
{
    const std::lock_guard<std::mutex> lock(mu_);
    if (execCreators_[type].count(tag) != 0) {
        HCCL_ERROR("[CollAlgExecRegistryV2]Exec tag[%s] already registered.", tag.c_str());
        return HcclResult::HCCL_E_INTERNAL;
    }
    execCreators_[type].emplace(tag, collExecCreator);
    return HcclResult::HCCL_SUCCESS;
}

bool CollAlgExecRegistryV2::IsRegistered(const HcclCMDType type, const std::string& tag) const
{
    auto typeIter = execCreators_.find(type);
    return typeIter != execCreators_.end() && typeIter->second.count(tag) != 0U;
}

std::unique_ptr<InsCollAlgBase> CollAlgExecRegistryV2::GetAlgExec(const HcclCMDType type, const std::string& tag)
{
    if (execCreators_.count(type) == 0 || execCreators_[type].count(tag) == 0) {
        HCCL_INFO(
            "[asc][AlgoExecute][CollAlgExecRegistryV2::GetAlgExec] miss, opType[%d], algName[%s].", type, tag.c_str());
        return nullptr;
    }
    auto executor = std::unique_ptr<InsCollAlgBase>(execCreators_[type][tag]());
    HCCL_INFO(
        "[asc][AlgoExecute][CollAlgExecRegistryV2::GetAlgExec] hit, opType[%d], algName[%s], "
        "executor[%p].",
        type, tag.c_str(), executor.get());
    return executor;
}

} // namespace mc2_ops_hccl
