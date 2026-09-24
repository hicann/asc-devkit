/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef EXTERNAL_ALG_RESOLVER_H
#define EXTERNAL_ALG_RESOLVER_H

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "alg_meta_registry.h"
#include "alg_param.h"
#include "external_alg_parser.h"

namespace mc2_ops_hccl {

struct AlgCandidate {
    std::string registeredName;
    AlgEngine engine;
    int variantCondId;
    int machineFlagId;
    int priority; // 同键定序第二级（specificity 降序 > priority 降序 > name 字典序），见 AlgMeta::priority
};

// 候选表键: (opType, 规范化外部名)
using ExtAlgKey = std::pair<HcclCMDType, std::string>;

enum class ResolveStatus {
    RESOLVED,      // 唯一定名，algName 有效
    ERROR_INVALID, // 候选为空 / 规则全灭 / 引擎无候选；errMsg 含外部名原文与原因
};

// 消歧（纯函数，无资源操作——选名与资源分离）:
//   引擎匹配 -> 变体条件 -> 机型 flag；第一个全过的候选胜出，不做资源实测。
ResolveStatus ResolveExternalAlg(
    const OpParam& opParam, const TopoInfoWithNetLayerDetails* topoInfo, const ExternalAlgSpec& spec,
    std::string& algName, std::string& errMsg);

// 候选表查询；无候选返回空 vector。
// 候选表在进程首次访问时从 AlgMetaRegistry 聚合并固化；此后运行期 REGISTER_ALG_META 的
// 新注册不可见（注册应全部发生在 static-init 期）。
const std::vector<AlgCandidate>& GetExternalCandidates(HcclCMDType cmd, const std::string& canonicalExtName);

// 纯函数构建器：按 (Specificity 降序, priority 降序, registeredName 字典序) 三键排序。
// Specificity = (condId != -1) + (flagId != -1)（约束多者先试，最特化者胜；cond/flag 各计 1 分等权）；
// priority 仅同特异性时生效（业务声明维度裁决权，当前全 0；flag 重叠场景启用）。
// 排序键是候选自带数据属性，
// 与注册顺序/链接顺序/static-init 顺序全部无关（跨 .cc static-init 顺序是 UB，不可依赖）。
std::map<ExtAlgKey, std::vector<AlgCandidate>> BuildCandidateTable(const std::vector<AlgMetaRow>& rows);

// 漏斗核心：候选集（须已按特异性序）中第一个全过引擎/变体条件/机型flag者胜出，无则 nullptr
const AlgCandidate* SelectCandidate(
    const std::vector<AlgCandidate>& candidates, const OpParam& opParam, const TopoInfoWithNetLayerDetails* topoInfo);

} // namespace mc2_ops_hccl
#endif // EXTERNAL_ALG_RESOLVER_H
