/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "execute_selector.h"
#include "auto_selector_base.h"
#include "selector_registry.h"
#include "cann_host_bridge.h"

namespace mc2_ops_hccl {

ExecuteSelector::ExecuteSelector() {}

HcclResult ExecuteSelector::Run(
    OpParam& opParam, TopoInfoWithNetLayerDetails* topoInfo, std::string& selectAlgName) const
{
    HCCL_INFO(
        "[asc][AlgoSelect][ExecuteSelector::Run] start, opType[%d], opExecuteConfig[%d], engine[%d], "
        "isMc2[%d], commName[%s], tag[%s].",
        opParam.opType, opParam.opExecuteConfig, opParam.engine, opParam.isMc2, opParam.commName, opParam.tag);

    if (UseCannBridge(opParam)) {
        HCCL_INFO(
            "[asc][AlgoSelect][ExecuteSelector::Run] before SelectViaCann, opType[%d], "
            "opExecuteConfig[%d], engine[%d].",
            opParam.opType, opParam.opExecuteConfig, opParam.engine);
        return SelectViaCann(opParam, topoInfo, selectAlgName);
    }

    std::map<u32, AutoSelectorBase*> selectors = SelectorRegistry::Global()->GetAllSelectors();

    selectors = SelectorRegistry::Global()->GetSelectorsByOpType(opParam.opType);
    HCCL_INFO(
        "[asc][AlgoSelect][ExecuteSelector::Run] selectorNum[%zu] for opType[%d].", selectors.size(), opParam.opType);
    for (auto iter : selectors) {
        HCCL_DEBUG(
            "[asc][AlgoSelect][ExecuteSelector::Run] try selector priority[%llu], opType[%d].", iter.first,
            opParam.opType);
        if (iter.second->Select(opParam, topoInfo, selectAlgName) == SelectorStatus::MATCH) {
            HCCL_INFO(
                "[asc][AlgoSelect][ExecuteSelector::Run] matched, priority[%llu], algName[%s], "
                "opExecuteConfig[%d], engine[%d].",
                iter.first, selectAlgName.c_str(), opParam.opExecuteConfig, opParam.engine);
            return HcclResult::HCCL_SUCCESS;
        }
    }

    HCCL_ERROR("[Algo][Selector] No selector is matched.");
    return HcclResult::HCCL_E_NOT_SUPPORT;
}

} // namespace mc2_ops_hccl
