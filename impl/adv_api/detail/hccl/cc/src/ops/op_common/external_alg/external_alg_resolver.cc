/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "external_alg_resolver.h"

#include <algorithm>

#include "alg_whitelist.h"
#include "external_alg_rules.h"
#include "log.h"

namespace mc2_ops_hccl {
namespace {
const char* EngineName(AlgEngine engine)
{
    switch (engine) {
        case AlgEngine::CCU:
            return "CCU";
        case AlgEngine::DPU:
            return "DPU";
        case AlgEngine::AICPU:
        default:
            return "AICPU";
    }
}

// opParam.engine 是请求引擎（CommEngine），与候选引擎（AlgEngine）是两个枚举，分开命名
const char* EngineName(CommEngine engine)
{
    switch (engine) {
        case CommEngine::COMM_ENGINE_CPU:
            return "CPU";
        case CommEngine::COMM_ENGINE_CPU_TS:
            return "CPU_TS";
        case CommEngine::COMM_ENGINE_AICPU:
            return "AICPU";
        case CommEngine::COMM_ENGINE_AICPU_TS:
            return "AICPU_TS";
        case CommEngine::COMM_ENGINE_AIV:
            return "AIV";
        case CommEngine::COMM_ENGINE_CCU:
            return "CCU";
        case CommEngine::COMM_ENGINE_RESERVED:
        default:
            return "RESERVED";
    }
}

// 进程级候选表（首次使用时聚合一次；static-init 已全部完成后才可能被调用）。
// 聚合时套用算法白名单，白名单外注册名不进入候选表。
const std::map<ExtAlgKey, std::vector<AlgCandidate>>& CandidateTable()
{
    static const std::map<ExtAlgKey, std::vector<AlgCandidate>> table = [] {
        const bool whitelistEnabled = IsAlgWhitelistEnabled();
        std::vector<AlgMetaRow> rows;
        std::string excluded;
        size_t total = 0U;
        for (const AlgMetaRow& row : AlgMetaRegistry::Instance().GetAll()) {
            total++;
            if (whitelistEnabled && !IsAlgAllowed(row.registeredName)) {
                excluded += excluded.empty() ? "" : ", ";
                excluded += row.registeredName;
                continue;
            }
            rows.push_back(row);
        }
        if (whitelistEnabled) {
            HCCL_INFO(
                "[MC2_EXT_ALG] alg whitelist: %zu/%zu kept, excluded: [%s].", rows.size(), total, excluded.c_str());
        } else {
            HCCL_INFO("[MC2_EXT_ALG] alg whitelist disabled: %zu/%zu kept.", rows.size(), total);
        }
        return BuildCandidateTable(rows);
    }();
    return table;
}

std::string DescribeCandidates(const std::vector<AlgCandidate>& candidates)
{
    std::string text;
    for (const AlgCandidate& cand : candidates) {
        if (!text.empty()) {
            text += ", ";
        }
        text += cand.registeredName;
        if (cand.variantCondId != -1) {
            text += "(" + std::string(VariantCondName(cand.variantCondId)) + ")";
        }
        if (cand.machineFlagId != -1) {
            text += "(" + std::string(MachineFlagName(cand.machineFlagId)) + ")";
        }
    }
    return text;
}
} // namespace

std::map<ExtAlgKey, std::vector<AlgCandidate>> BuildCandidateTable(const std::vector<AlgMetaRow>& rows)
{
    std::map<ExtAlgKey, std::vector<AlgCandidate>> table;
    for (const AlgMetaRow& row : rows) {
        table[std::make_pair(row.cmd, std::string(row.meta.externalName))].push_back(AlgCandidate{
            row.registeredName, row.meta.engine, row.meta.variantCondId, row.meta.machineFlagId, row.meta.priority});
    }
    for (auto& entry : table) {
        std::sort(entry.second.begin(), entry.second.end(), [](const AlgCandidate& a, const AlgCandidate& b) {
            const int specA = (a.variantCondId != -1 ? 1 : 0) + (a.machineFlagId != -1 ? 1 : 0);
            const int specB = (b.variantCondId != -1 ? 1 : 0) + (b.machineFlagId != -1 ? 1 : 0);
            if (specA != specB) {
                return specA > specB; // 键1: 特异性降序（约束多者先试，最特化者胜）
            }
            if (a.priority != b.priority) {
                return a.priority > b.priority; // 键2: priority 降序（同特异性时业务声明谁先；当前全 0）
            }
            return a.registeredName < b.registeredName; // 键3: 名字字典序（确定性兜底）
        });
    }
    return table;
}

const std::vector<AlgCandidate>& GetExternalCandidates(HcclCMDType cmd, const std::string& canonicalExtName)
{
    static const std::vector<AlgCandidate> kEmpty;
    const auto& table = CandidateTable();
    const auto it = table.find(std::make_pair(cmd, canonicalExtName));
    return it == table.end() ? kEmpty : it->second;
}

// 三级漏斗核心（引擎→变体条件→机型flag）：第一个全过的候选胜出，无则 nullptr
const AlgCandidate* SelectCandidate(
    const std::vector<AlgCandidate>& candidates, const OpParam& opParam, const TopoInfoWithNetLayerDetails* topoInfo)
{
    for (const AlgCandidate& cand : candidates) {
        if (!EngineMatch(cand.engine, opParam)) {
            continue;
        }
        // 变体条件：下标双边判防注册侧笔误 OOB（Register() 无法校验：rules.h 反向 include 循环依赖）
        if (cand.variantCondId >= 0 && cand.variantCondId < VARIANT_COND_COUNT &&
            !VARIANT_COND_TABLE[cand.variantCondId](topoInfo, opParam)) {
            continue;
        }
        // 机型 flag：同上
        if (cand.machineFlagId >= 0 && cand.machineFlagId < MACHINE_FLAG_COUNT &&
            !MACHINE_FLAG_TABLE[cand.machineFlagId](topoInfo)) {
            continue;
        }
        return &cand;
    }
    return nullptr;
}

ResolveStatus ResolveExternalAlg(
    const OpParam& opParam, const TopoInfoWithNetLayerDetails* topoInfo, const ExternalAlgSpec& spec,
    std::string& algName, std::string& errMsg)
{
    algName.clear();
    errMsg.clear();
    const std::string key = spec.canonical();
    const std::vector<AlgCandidate>& candidates = GetExternalCandidates(opParam.opType, key);

    if (HcclCheckLogLevel(HCCL_LOG_INFO)) {
        HCCL_INFO(
            "[MC2_EXT_ALG] op[%u] ext[%s] engine[%s] candidates(%zu): [%s]", static_cast<u32>(opParam.opType),
            key.c_str(), EngineName(opParam.engine), candidates.size(), DescribeCandidates(candidates).c_str());
    }

    const AlgCandidate* selected = SelectCandidate(candidates, opParam, topoInfo);
    if (selected != nullptr) {
        algName = selected->registeredName;
        HCCL_INFO("[MC2_EXT_ALG] resolved: %s (engine=%s)", algName.c_str(), EngineName(selected->engine));
        return ResolveStatus::RESOLVED;
    }

    // errMsg 随返回值交调用方打日志（AICPU 链为 WARNING+回退）
    const std::string reason =
        candidates.empty() ?
            "(no candidate registered in this build for engine " + std::string(EngineName(opParam.engine)) + ")" :
            "(all candidates rejected by rules for engine " + std::string(EngineName(opParam.engine)) + ")";
    errMsg = "resolve failed, ext=\"" + key + "\" op=" + std::to_string(static_cast<u32>(opParam.opType)) +
             ", candidates=[" + DescribeCandidates(candidates) + "] " + reason;
    return ResolveStatus::ERROR_INVALID;
}
} // namespace mc2_ops_hccl
