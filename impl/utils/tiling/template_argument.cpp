/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file template_argument.cpp
 * \brief
 */

#include "utils/tiling/template_argument.h"
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <csignal>
#include <vector>
#include <string>
#include <algorithm>
#include <array>
#include <unordered_set>
#include <unordered_map>

using namespace std;
namespace {
std::string Vec2Str(const std::vector<uint64_t>& vec)
{
    std::string resStr = "[";
    for (const auto& i : vec) {
        resStr += std::to_string(i) + ", ";
    }
    resStr += "]";
    return resStr;
}
} // namespace

namespace AscendC {
const std::array<const char*, 7> TPL_TYPE_STR_ARR = {
    "DTYPE", "FORMAT", "UINT", "BOOL", "KERNEL_TYPE", "DETERMINISTIC", "SHARED_KERNEL_TYPE"};
static bool CheckParamStructValid(ParamStruct& paramStruct)
{
    if (paramStruct.paramType >= TPL_TYPE_STR_ARR.size()) {
        printf(
            "[ERROR] ASCENDC_TPL_*_%s: %s type value is invalid! Type value should be in [0, 1, 2, 3, 4, 5, 6]\n",
            paramStruct.macroType, paramStruct.name);
        return false;
    }
    if (paramStruct.vals.empty()) {
        printf(
            "[ERROR] Values of ASCENDC_TPL_%s_%s: %s is empty!\n", TPL_TYPE_STR_ARR[paramStruct.paramType],
            paramStruct.macroType, paramStruct.name);
        return false;
    }
    std::unordered_set<uint64_t> valueSet(paramStruct.vals.begin(), paramStruct.vals.end());
    if (paramStruct.vals.size() != valueSet.size()) {
        printf(
            "[ERROR] Existing duplicate values in ASCENDC_TPL_%s_%s: %s! Duplicate values: %s\n",
            TPL_TYPE_STR_ARR[paramStruct.paramType], paramStruct.macroType, paramStruct.name,
            Vec2Str(paramStruct.vals).c_str());
        return false;
    }
    return true;
}

static bool ParseTplUintValue(ParamStruct& paramStruct)
{
    if (paramStruct.paramType != ASCENDC_TPL_UINT) {
        return CheckParamStructValid(paramStruct);
    }
    if (paramStruct.vals.size() < 2U) {
        printf(
            "[ERROR] Value length of ASCENDC_TPL_UINT_%s: %s is less than 2, parse value failed.\n",
            paramStruct.macroType, paramStruct.name);
        return false;
    }
    uint8_t uiFlag = static_cast<uint8_t>(paramStruct.vals[0]);
    if (uiFlag == ASCENDC_TPL_UI_LIST) {
        paramStruct.vals.erase(paramStruct.vals.begin());
        return CheckParamStructValid(paramStruct);
    }
    if (uiFlag != ASCENDC_TPL_UI_RANGE && uiFlag != ASCENDC_TPL_UI_MIX) {
        printf(
            "[ERROR] ASCENDC_TPL_UINT_%s: %s UI flag is invalid,"
            "it should be in [UI_RANGE, UI_LIST, UI_MIX]!\n",
            paramStruct.macroType, paramStruct.name);
        return false;
    }
    std::vector<uint64_t> extendVal;
    size_t rangeNum = paramStruct.vals[1];
    if (rangeNum * VAL_PAIR > paramStruct.vals.size() - VAL_START) {
        printf(
            "[ERROR] ASCENDC_TPL_UINT_%s: %s range declare exceed actual length!\n", paramStruct.macroType,
            paramStruct.name);
        return false;
    }
    size_t elementNum = 0;
    for (size_t i = 1; i <= rangeNum; i++) {
        elementNum += paramStruct.vals[i * VAL_PAIR + 1] - paramStruct.vals[VAL_PAIR * i] + 1;
    }
    if (uiFlag == ASCENDC_TPL_UI_MIX) {
        elementNum += paramStruct.vals.size() - (VAL_START + rangeNum * VAL_PAIR);
    }
    extendVal.reserve(elementNum);
    for (size_t i = 1; i <= rangeNum; i++) {
        for (size_t j = paramStruct.vals[VAL_PAIR * i]; j <= paramStruct.vals[i * VAL_PAIR + 1]; j++) {
            extendVal.emplace_back(j);
        }
    }
    if (uiFlag == ASCENDC_TPL_UI_MIX) {
        size_t mixStart = VAL_START + rangeNum * VAL_PAIR;
        std::copy(paramStruct.vals.begin() + mixStart, paramStruct.vals.end(), std::back_inserter(extendVal));
    }
    paramStruct.vals = std::move(extendVal);
    return CheckParamStructValid(paramStruct);
}

static bool CheckSelectParamValid(const TilingDeclareParams& declareParams, const ParamStruct& selectParam)
{
    for (const auto& declareParam : declareParams) {
        if (declareParam.name == selectParam.name) {
            auto declareType = declareParam.paramType;
            auto declareVals = declareParam.vals;
            if (declareType != selectParam.paramType) {
                printf(
                    "[ERROR] ASCENDC_TPL_%s_SEL: %s has different macro type!\n",
                    TPL_TYPE_STR_ARR[declareParam.paramType], selectParam.name);
                return false;
            }
            for (auto val : selectParam.vals) {
                if (std::find(declareVals.begin(), declareVals.end(), val) == declareVals.cend()) {
                    printf(
                        "[ERROR] ASCENDC_TPL_%s_SEL %s value %lu does not exist in ASCENDC_TPL_%s_DECL, "
                        "please check it!\n",
                        TPL_TYPE_STR_ARR[declareParam.paramType], selectParam.name, val,
                        TPL_TYPE_STR_ARR[declareParam.paramType]);
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}

static bool CheckSelectParamsValid(const TilingDeclareParams& declareParams, const TilingSelectParams& selectParams)
{
    for (const auto& selectParamVec : selectParams) {
        for (const auto& selectParam : selectParamVec) {
            if (selectParam.paramType == ASCENDC_TPL_KERNEL_TYPE ||
                selectParam.paramType == ASCENDC_TPL_DETERMINISTIC ||
                selectParam.paramType == ASCENDC_TPL_DETERMINISTIC_LEVEL) {
                continue;
            }
            if (!CheckSelectParamValid(declareParams, selectParam)) {
                return false;
            }
        }
    }
    return true;
}

static bool CheckInputTilingParamValid(
    const std::vector<uint64_t>& tilingParams, TilingSelectParams& selectParamsList,
    const TilingDeclareParams& declareParams)
{
    if (tilingParams.size() != declareParams.size()) {
        printf("[ERROR] Number of arguments in GET_TPL_TILING_KEY is not the same as ASCENDC_TPL_ARGS_DECL.\n");
        return false;
    }
    std::unordered_map<std::string, size_t> nameOrder;
    for (size_t i = 0; i < declareParams.size(); ++i) {
        nameOrder[declareParams[i].name] = i;
    }

    for (auto& selectParams : selectParamsList) {
        // 删除paramType为{ASCENDC_TPL_KERNEL_TYPE, ASCENDC_TPL_DETERMINISTIC}的元素
        auto newEnd = std::remove_if(selectParams.begin(), selectParams.end(), [](const ParamStruct& param) {
            return param.paramType == ASCENDC_TPL_KERNEL_TYPE || param.paramType == ASCENDC_TPL_DETERMINISTIC ||
                   param.paramType == ASCENDC_TPL_DETERMINISTIC_LEVEL;
        });
        selectParams.erase(newEnd, selectParams.end());
        // sort selectParam in the order of ASCENDC_TPL_DECL
        std::sort(selectParams.begin(), selectParams.end(), [&nameOrder](const ParamStruct& a, const ParamStruct& b) {
            auto itA = nameOrder.find(a.name);
            auto itB = nameOrder.find(b.name);
            if (itA == nameOrder.cend() || itB == nameOrder.cend()) {
                return false;
            }
            return itA->second < itB->second;
        });
        // 认为tilingParams, selectParams, declareParams的参数顺序数量都是对应的
        bool matchFlag = true;
        for (std::size_t i = 0; i < tilingParams.size(); i++) {
            // 默认是匹配的，如果中间有不匹配的则退出
            if (std::find(selectParams[i].vals.begin(), selectParams[i].vals.end(), tilingParams[i]) ==
                selectParams[i].vals.cend()) {
                matchFlag = false;
                break;
            }
        }
        if (matchFlag) {
            return true;
        }
    }
    return false;
}

static bool CheckParamValid(
    const std::vector<uint64_t>& tilingParams, TilingSelectParams& selectParamsVec,
    const TilingDeclareParams& declareParams)
{
    if (!CheckSelectParamsValid(declareParams, selectParamsVec)) {
        printf("[ERROR] ASCEND_TPL_SEL format is incorrect, please check!\n");
        return false;
    }
    // Validate args in GET_TPL_TILING_KEY()
    if (!CheckInputTilingParamValid(tilingParams, selectParamsVec, declareParams)) {
        printf(
            "[ERROR] GET_TPL_TILING_KEY args:%s cannot be found in ASCENDC_TPL_ARGS_SEL.\n",
            Vec2Str(tilingParams).c_str());
        return false;
    }
    return true;
}

static uint64_t EncodeParam(uint64_t val, const ParamStruct& paramStruct)
{
    if (paramStruct.paramType == ASCENDC_TPL_DTYPE || paramStruct.paramType == ASCENDC_TPL_FORMAT ||
        paramStruct.paramType == ASCENDC_TPL_BOOL || paramStruct.paramType == ASCENDC_TPL_SHARED_KERNEL_TYPE) {
        return static_cast<uint64_t>(val);
    } else if (paramStruct.paramType == ASCENDC_TPL_UINT) {
        auto iter = std::find(paramStruct.vals.cbegin(), paramStruct.vals.cend(), val);
        // CheckParamValid ensures val can must be found.
        uint64_t index = iter - paramStruct.vals.cbegin();
        return index;
    } else {
        return INVALID_TILING_KEY;
    }
}

uint64_t EncodeTilingKey(
    TilingDeclareParams declareParams, TilingSelectParams selectParamsVec, std::vector<uint64_t> tilingParams)
{
    // STEP 1: Copy static variable, and convert ASCENDC_TPL_UINT to readable vector value
    if (declareParams.empty() || selectParamsVec.empty()) {
        printf("[ERROR] ASCENDC_TPL params is empty!\n");
        return INVALID_TILING_KEY;
    }
    std::unordered_set<std::string> declareParamNameSet;
    for (auto& declareParam : declareParams) {
        declareParamNameSet.insert(declareParam.name);
        if (!ParseTplUintValue(declareParam)) {
            printf("[ERROR] ASCENDC_TPL_DECL:%s parses value failed!\n", declareParam.name);
            return INVALID_TILING_KEY;
        }
    }
    for (auto& selectParams : selectParamsVec) {
        std::unordered_set<std::string> selectParamNameSet{};
        for (auto& selectParam : selectParams) {
            if (selectParam.paramType == ASCENDC_TPL_KERNEL_TYPE ||
                selectParam.paramType == ASCENDC_TPL_DETERMINISTIC ||
                selectParam.paramType == ASCENDC_TPL_DETERMINISTIC_LEVEL) {
                continue;
            }
            std::string name = selectParam.name;
            selectParamNameSet.insert(name);
            // 保证selectParam在declareParams中存在
            if (declareParamNameSet.count(name) == 0 || !ParseTplUintValue(selectParam)) {
                printf(
                    "[ERROR] ASCENDC_TPL_SEL : %s parses value failed!"
                    " Please check macro define name or values.\n",
                    name.c_str());
                return INVALID_TILING_KEY;
            }
        }
        // 去重之后的selectParams和declareParams长度相同，则两者相等，否则select中有缺少的declareParams
        if (declareParamNameSet.size() != selectParamNameSet.size()) {
            printf("[ERROR] ASCENDC_TPL_SEL : The parameters in ASCENDC_TPL_ARGS_SEL "
                   "do not fully include those in ASCENDC_TPL_ARGS_DECL.\n");
            return INVALID_TILING_KEY;
        }
    }
    if (!CheckParamValid(tilingParams, selectParamsVec, declareParams)) {
        return INVALID_TILING_KEY;
    }
    // Encode each value into bit and concat them into a unique tilingKey
    uint8_t totalBits = 0;
    uint64_t tilingKey = 0;
    for (size_t i = 0; i < declareParams.size(); ++i) {
        auto bitWidth = declareParams.at(i).bitWidth;
        auto paramBit = EncodeParam(tilingParams.at(i), declareParams.at(i));
        if (paramBit == INVALID_TILING_KEY) {
            return INVALID_TILING_KEY;
        }
        tilingKey |= (paramBit << totalBits);
        totalBits += bitWidth;
    }
    if (totalBits > MAX_BITS_NUM) {
        printf("[ERROR] Tiling Key exceeds maximum 64 bit, please adjust ASCENDC_TPL_ARGS_DECL bitWidth"
               "accordingly\n");
        return INVALID_TILING_KEY;
    }
    return tilingKey;
}
} // namespace AscendC
