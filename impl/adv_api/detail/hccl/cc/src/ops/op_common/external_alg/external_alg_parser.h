/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef EXTERNAL_ALG_PARSER_H
#define EXTERNAL_ALG_PARSER_H

#include <stddef.h>
#include <string>
#include <vector>

namespace mc2_ops_hccl {

// 外部算法名文法:
//   external := executor '[' layer (',' layer)* ']' ('[order.strong]')?
//   executor := sole | sequence | parallel | concur | pipeline
//   layer    := atom ('.' attr)*
//   atom     := mesh | nhr
//   attr     := chunk | one_shot | two_shot | multi_channel | single_channel
//   （multi_channel：原始需求文档误写为 muti_channel；hccl/devkit 注册名从来都是 Multi 拼写）
// 约束:
//   - 空格/制表符全部忽略
//   - 层数 1~3（系统最大 3 层物理拓扑，超过即语法错）
//   - 仅词法/结构校验；语义合法性（候选是否存在）由消歧器查表负责

enum class ExternalExecutor { SOLE = 0, SEQUENCE, PARALLEL, CONCUR, PIPELINE };

// 层数上限对齐系统最大拓扑层数（3 层网）
constexpr size_t MAX_EXTERNAL_ALG_LAYERS = 3;

struct ExternalAlgLayer {
    std::string atom;               // "mesh" | "nhr"
    std::vector<std::string> attrs; // 属性按书写顺序保留
};

struct ExternalAlgSpec {
    ExternalExecutor executor = ExternalExecutor::SOLE;
    std::vector<ExternalAlgLayer> layers;
    bool orderStrong = false;

    // 规范化外部名 = 候选表主键
    std::string canonical() const
    {
        static const char* EXECUTOR_NAME[] = {"sole", "sequence", "parallel", "concur", "pipeline"};
        std::string text = EXECUTOR_NAME[static_cast<size_t>(executor)];
        text += '[';
        for (size_t i = 0; i < layers.size(); ++i) {
            if (i > 0) {
                text += ',';
            }
            text += layers[i].atom;
            for (const std::string& attr : layers[i].attrs) {
                text += '.';
                text += attr;
            }
        }
        text += ']';
        if (orderStrong) {
            text += "[order.strong]";
        }
        return text;
    }
};

// tiling algConfig 四态识别（纯函数；tiling 侧与 mc2 侧共用）:
//   空串 -> NONE
//   含 '=' -> LEGACY（HCCL_ALGO 老语法，保持现状拒绝语义，由调用方走默认 selector）
//   含 '[' -> EXTERNAL（解析成功: spec 有效/errMsg 空; 解析失败: errMsg 给出词法错误，
//   spec 已复位为默认值——但默认值 canonical() 为 "sole[]" 并非失败标记，失败判据只能是 errMsg 非空）
//   其余 -> BARE_NAME（裸注册名，现状语义逐字节保留）
enum class ForcedAlgKind { NONE, LEGACY, EXTERNAL, BARE_NAME };

namespace external_alg_internal {
inline bool WordInTable(const std::string& word, const char* const* table, size_t size)
{
    for (size_t i = 0; i < size; ++i) {
        if (word == table[i]) {
            return true;
        }
    }
    return false;
}
} // namespace external_alg_internal

// 外部名词法解析（纯函数）。成功: spec 有效/errMsg 空; 失败: errMsg 给出词法错误原因。
// 失败路径不清场——spec 为部分解析残留（边解析边写，中途出错即返回），调用方不得使用;
// 需要失败态干净的 spec 请走 ClassifyForcedAlgConfig（其失败时已做复位兜底）。
inline bool ParseExternalAlg(const std::string& algConfig, ExternalAlgSpec& spec, std::string& errMsg)
{
    spec = ExternalAlgSpec();
    errMsg.clear();

    // 1. 去空格
    std::string text;
    text.reserve(algConfig.size());
    for (size_t i = 0; i < algConfig.size(); ++i) {
        const char ch = algConfig[i];
        if (ch != ' ' && ch != '\t') {
            text += ch;
        }
    }
    if (text.empty()) {
        errMsg = "external alg name is empty";
        return false;
    }

    // 2. executor 与层列表括号
    const size_t openPos = text.find('[');
    if (openPos == std::string::npos || openPos == 0) {
        errMsg = "missing '[' after executor in \"" + algConfig + "\"";
        return false;
    }
    const std::string executorWord = text.substr(0, openPos);
    static const char* const EXECUTORS[] = {"sole", "sequence", "parallel", "concur", "pipeline"};
    const size_t EXECUTOR_COUNT = sizeof(EXECUTORS) / sizeof(EXECUTORS[0]);
    if (!external_alg_internal::WordInTable(executorWord, EXECUTORS, EXECUTOR_COUNT)) {
        errMsg = "unknown executor \"" + executorWord +
                 "\" (expected one of: sole, sequence, parallel, concur, pipeline) in \"" + algConfig + "\"";
        return false;
    }
    for (size_t i = 0; i < EXECUTOR_COUNT; ++i) {
        if (executorWord == EXECUTORS[i]) {
            spec.executor = static_cast<ExternalExecutor>(i);
            break;
        }
    }

    const size_t closePos = text.find(']', openPos);
    if (closePos == std::string::npos) {
        errMsg = "missing ']' in \"" + algConfig + "\"";
        return false;
    }

    // 3. 可选保序后缀
    const std::string tail = text.substr(closePos + 1);
    if (!tail.empty()) {
        if (tail != "[order.strong]") {
            errMsg = "invalid suffix \"" + tail + "\" after ']' (only \"[order.strong]\" is allowed) in \"" +
                     algConfig + "\"";
            return false;
        }
        spec.orderStrong = true;
    }

    // 4. 层列表（逗号分隔；每层点号分词：首词=原子，其余=属性）
    const std::string layerText = text.substr(openPos + 1, closePos - openPos - 1);
    size_t layerStart = 0;
    for (size_t pos = 0; pos <= layerText.size(); ++pos) {
        if (pos != layerText.size() && layerText[pos] != ',') {
            continue;
        }
        const std::string layerWord = layerText.substr(layerStart, pos - layerStart);
        const size_t layerNo = spec.layers.size() + 1;
        if (layerWord.empty()) {
            errMsg = "empty layer " + std::to_string(layerNo) + " in \"" + algConfig + "\"";
            return false;
        }
        ExternalAlgLayer layer;
        size_t segStart = 0;
        bool atomSeen = false;
        for (size_t dot = 0; dot <= layerWord.size(); ++dot) {
            if (dot != layerWord.size() && layerWord[dot] != '.') {
                continue;
            }
            const std::string seg = layerWord.substr(segStart, dot - segStart);
            if (seg.empty()) {
                errMsg = "empty token in layer " + std::to_string(layerNo) + " of \"" + algConfig + "\"";
                return false;
            }
            if (!atomSeen) {
                static const char* const ATOMS[] = {"mesh", "nhr"};
                if (!external_alg_internal::WordInTable(seg, ATOMS, sizeof(ATOMS) / sizeof(ATOMS[0]))) {
                    errMsg = "unknown atom \"" + seg + "\" in layer " + std::to_string(layerNo) +
                             " (expected: mesh or nhr) in \"" + algConfig + "\"";
                    return false;
                }
                layer.atom = seg;
                atomSeen = true;
            } else {
                static const char* const ATTRS[] = {"chunk", "one_shot", "two_shot", "multi_channel", "single_channel"};
                if (!external_alg_internal::WordInTable(seg, ATTRS, sizeof(ATTRS) / sizeof(ATTRS[0]))) {
                    errMsg = "unknown attribute \"" + seg + "\" in layer " + std::to_string(layerNo) +
                             " (expected one of: chunk, one_shot, two_shot, multi_channel, single_channel) in \"" +
                             algConfig + "\"";
                    return false;
                }
                layer.attrs.push_back(seg);
            }
            segStart = dot + 1;
        }
        spec.layers.push_back(layer);
        layerStart = pos + 1;
    }
    if (spec.layers.empty()) {
        errMsg = "no layer in \"" + algConfig + "\"";
        return false;
    }
    if (spec.layers.size() > MAX_EXTERNAL_ALG_LAYERS) {
        errMsg = "too many layers (" + std::to_string(spec.layers.size()) + ", max " +
                 std::to_string(MAX_EXTERNAL_ALG_LAYERS) + ") in \"" + algConfig + "\"";
        return false;
    }
    return true;
}

inline ForcedAlgKind ClassifyForcedAlgConfig(
    const std::string& algConfig, std::string& bareName, ExternalAlgSpec& spec, std::string& errMsg)
{
    bareName.clear();
    errMsg.clear();
    spec = ExternalAlgSpec();
    if (algConfig.empty()) {
        return ForcedAlgKind::NONE;
    }
    if (algConfig.find('=') != std::string::npos) {
        return ForcedAlgKind::LEGACY;
    }
    if (algConfig.find('[') != std::string::npos) {
        if (!ParseExternalAlg(algConfig, spec, errMsg)) {
            spec = ExternalAlgSpec();
        }
        return ForcedAlgKind::EXTERNAL;
    }
    bareName = algConfig;
    return ForcedAlgKind::BARE_NAME;
}

} // namespace mc2_ops_hccl
#endif // EXTERNAL_ALG_PARSER_H
