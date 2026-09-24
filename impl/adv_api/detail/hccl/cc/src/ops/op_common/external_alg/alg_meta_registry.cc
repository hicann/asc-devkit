/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "alg_meta_registry.h"

#include "log.h"

namespace mc2_ops_hccl {

AlgMetaRegistry& AlgMetaRegistry::Instance()
{
    // Meyers 单例：首次注册前完成构造，规避跨 .cc 文件 static-init 顺序问题
    // （与 CollAlgExecRegistryV2::Instance 同款模式）
    static AlgMetaRegistry instance;
    return instance;
}

HcclResult AlgMetaRegistry::Register(HcclCMDType cmd, const std::string& registeredName, const AlgMeta& meta)
{
    std::lock_guard<std::mutex> lock(mu_);
    const auto key = std::make_pair(cmd, registeredName);
    if (metas_.find(key) != metas_.end()) {
        HCCL_ERROR(
            "[AlgMetaRegistry] duplicate sidecar registration, cmd[%u], name[%s].", static_cast<u32>(cmd),
            registeredName.c_str());
        return HCCL_E_PARA;
    }
    metas_[key] = meta;
    return HCCL_SUCCESS;
}

const AlgMeta* AlgMetaRegistry::Get(HcclCMDType cmd, const std::string& registeredName) const
{
    std::lock_guard<std::mutex> lock(mu_);
    const auto it = metas_.find(std::make_pair(cmd, registeredName));
    return it == metas_.end() ? nullptr : &it->second;
}

std::vector<AlgMetaRow> AlgMetaRegistry::GetAll() const
{
    std::lock_guard<std::mutex> lock(mu_);
    std::vector<AlgMetaRow> rows;
    rows.reserve(metas_.size());
    for (const auto& entry : metas_) {
        rows.push_back(AlgMetaRow{entry.first.first, entry.first.second, entry.second});
    }
    return rows;
}
} // namespace mc2_ops_hccl
