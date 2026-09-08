/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "manifest_constant.h"

#include "ascendc_tool_log.h"

#include <nlohmann/json.hpp>
#include <utility>

namespace ascendc {
namespace {
using Json = nlohmann::json;

bool ReadNonnegativeInteger(const Json& value, uint64_t& result)
{
    if (!value.is_number_integer() || (!value.is_number_unsigned() && value.get<int64_t>() < 0)) {
        return false;
    }
    result = value.get<uint64_t>();
    return true;
}
} // namespace

bool ManifestConstant::ParseDefinition(const Json& constant)
{
    if (!constant.is_object()) {
        ASCENDLOGE("constant must be an object");
        return false;
    }
    for (const char* field : {"name", "parameter_index", "file", "template"}) {
        if (!constant.contains(field)) {
            ASCENDLOGE("Missing required constant field: %s", field);
            return false;
        }
    }
    for (const char* field : {"name", "file", "template"}) {
        if (!constant.at(field).is_string() || constant.at(field).get_ref<const std::string&>().empty()) {
            ASCENDLOGE("Constant field must be a nonempty string: %s", field);
            return false;
        }
    }
    ManifestConstant result;
    if (!ReadNonnegativeInteger(constant.at("parameter_index"), result.parameterIndex_)) {
        ASCENDLOGE("parameter_index must be a nonnegative integer");
        return false;
    }
    const bool hasArgumentType = constant.contains("arg_type");
    if (hasArgumentType && !constant.at("arg_type").is_string()) {
        ASCENDLOGE("arg_type must be a string");
        return false;
    }
    const std::string argumentType = hasArgumentType ? constant.at("arg_type").get<std::string>() : "struct";
    const bool hasByteSize = constant.contains("byte_size");
    if (argumentType == "pointer") {
        uint64_t byteCount = 0U;
        if (!hasByteSize || !ReadNonnegativeInteger(constant.at("byte_size"), byteCount) || byteCount == 0U) {
            ASCENDLOGE("pointer constant requires a positive integer byte_size");
            return false;
        }
        result.manifestByteSize_ = byteCount;
    } else if (argumentType == "struct") {
        if (hasByteSize) {
            ASCENDLOGE("struct constant must omit byte_size; size comes from argsBytes");
            return false;
        }
    } else {
        ASCENDLOGE("arg_type must be pointer or struct");
        return false;
    }
    constexpr size_t requiredFieldCount = 4U;
    const size_t expectedFieldCount = requiredFieldCount + (hasArgumentType ? 1U : 0U) + (hasByteSize ? 1U : 0U);
    if (constant.size() != expectedFieldCount) {
        ASCENDLOGE("constant contains unexpected fields");
        return false;
    }
    result.name_ = constant.at("name").get<std::string>();
    result.targetFile_ = constant.at("file").get<std::string>();
    result.templateText_ = constant.at("template").get<std::string>();
    result.definitionParsed_ = true;
    *this = std::move(result);
    return true;
}

bool ManifestConstant::BindArgument(uint64_t argsCount, const void* const* argsAddr, const uint64_t* argsBytes)
{
    if (!definitionParsed_) {
        ASCENDLOGE("Constant definition must be parsed before binding an argument");
        return false;
    }
    if (parameterIndex_ >= argsCount) {
        ASCENDLOGE(
            "Constant %s parameter_index %llu is outside argsCount %llu", name_.c_str(),
            static_cast<unsigned long long>(parameterIndex_), static_cast<unsigned long long>(argsCount));
        return false;
    }
    if (argsAddr == nullptr || argsAddr[parameterIndex_] == nullptr) {
        ASCENDLOGE(
            "Constant %s requires a nonnull argsAddr[%llu]", name_.c_str(),
            static_cast<unsigned long long>(parameterIndex_));
        return false;
    }
    uint64_t byteSize = manifestByteSize_;
    if (byteSize == 0U) {
        if (argsBytes == nullptr || argsBytes[parameterIndex_] == 0U) {
            ASCENDLOGE(
                "Constant %s requires nonzero argsBytes[%llu]", name_.c_str(),
                static_cast<unsigned long long>(parameterIndex_));
            return false;
        }
        byteSize = argsBytes[parameterIndex_];
    }
    borrowedData_ = argsAddr[parameterIndex_];
    boundByteSize_ = byteSize;
    return true;
}

} // namespace ascendc
