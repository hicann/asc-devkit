/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "compilation_manifest_parser.h"

#include "ascendc_tool_log.h"
#include <nlohmann/json.hpp>
#include <limits>

namespace ascendc {
namespace aclrtc {
namespace {
using Json = nlohmann::json;
constexpr char SCHEMA_VERSION[] = "1.0";
constexpr char OPTIONS_PREFIX[] = "${options:";

bool ReadBoundedNonnegativeInteger(const Json& value, uint64_t maximum, uint64_t& result)
{
    if (!value.is_number_integer() || (!value.is_number_unsigned() && value.get<int64_t>() < 0)) {
        return false;
    }
    result = value.get<uint64_t>();
    return result <= maximum;
}

} // namespace

aclError CompilationManifestParser::ParseConstants(const Json& kernel, std::vector<ManifestConstant>& constants) const
{
    const auto infos = kernel.find("constant_infos");
    if (infos == kernel.end() || !infos->is_array() || infos->empty()) {
        ASCENDLOGE("Selected kernel requires a nonempty constant_infos array");
        return ACLRTC_ERROR_FAILURE;
    }
    for (size_t index = 0; index < infos->size(); ++index) {
        const Json& info = infos->at(index);
        ManifestConstant spec;
        if (!spec.ParseDefinition(info)) {
            return ACLRTC_ERROR_FAILURE;
        }
        constants.emplace_back(std::move(spec));
    }
    return ACLRTC_SUCCESS;
}

aclError CompilationManifestParser::ExpandOptionsReference(
    const std::string& argument, const std::string& location, std::vector<std::string>& arguments) const
{
    if (argument.compare(0, sizeof(OPTIONS_PREFIX) - 1U, OPTIONS_PREFIX) != 0 ||
        argument.size() <= sizeof(OPTIONS_PREFIX) || argument.back() != '}') {
        arguments.push_back(argument);
        return ACLRTC_SUCCESS;
    }
    const std::string name = argument.substr(sizeof(OPTIONS_PREFIX) - 1U, argument.size() - sizeof(OPTIONS_PREFIX));
    const auto options = borrowedManifest_.find("options");
    if (options == borrowedManifest_.end() || !options->is_object() || !options->contains(name)) {
        ASCENDLOGE("Manifest value %s references undefined options key '%s'", location.c_str(), name.c_str());
        return ACLRTC_ERROR_FAILURE;
    }
    const auto values = options->at(name).get<std::vector<std::string>>();
    arguments.insert(arguments.end(), values.begin(), values.end());
    return ACLRTC_SUCCESS;
}

aclError CompilationManifestParser::ParseCommand(
    const Json& command, const std::string& objectName, ManifestObjectKind objectKind, size_t index,
    ManifestCommand& spec) const
{
    const std::string type = command.at("type").get<std::string>();
    if (type == "compile") {
        spec.commandKind = CompilationCommandKind::Compile;
    } else if (type == "objcopy") {
        spec.commandKind = CompilationCommandKind::ObjectCopy;
    } else {
        ASCENDLOGE(
            "Manifest object %s command[%zu] has unsupported type '%s'", objectName.c_str(), index, type.c_str());
        return ACLRTC_ERROR_FAILURE;
    }
    spec.objectKind = objectKind;
    spec.diagnosticLabel = objectName + "/" + type + "[" + std::to_string(index) + "]";
    if (command.contains("stage")) {
        uint64_t stage = 0U;
        if (!ReadBoundedNonnegativeInteger(command.at("stage"), std::numeric_limits<uint32_t>::max(), stage)) {
            ASCENDLOGE("Manifest command %s has invalid stage", spec.diagnosticLabel.c_str());
            return ACLRTC_ERROR_FAILURE;
        }
        spec.parallelStage = static_cast<uint32_t>(stage);
    }
    const auto argv = command.at("cmd").get<std::vector<std::string>>();
    if (argv.empty()) {
        ASCENDLOGE("Manifest command %s has an empty cmd array", spec.diagnosticLabel.c_str());
        return ACLRTC_ERROR_FAILURE;
    }
    std::vector<std::string> expandedArguments;
    for (size_t argumentIndex = 0U; argumentIndex < argv.size(); ++argumentIndex) {
        const aclError result = ExpandOptionsReference(
            argv[argumentIndex], spec.diagnosticLabel + ".cmd[" + std::to_string(argumentIndex) + "]",
            expandedArguments);
        if (result != ACLRTC_SUCCESS) {
            return result;
        }
    }
    if (expandedArguments.empty()) {
        ASCENDLOGE("Manifest command %s expands to an empty cmd array", spec.diagnosticLabel.c_str());
        return ACLRTC_ERROR_FAILURE;
    }
    spec.executable = std::move(expandedArguments.front());
    expandedArguments.erase(expandedArguments.begin());
    spec.arguments = std::move(expandedArguments);
    return ACLRTC_SUCCESS;
}

aclError CompilationManifestParser::ParseSelectedObjects(
    const Json& kernel, bool enableSuperKernel, CompilationManifest& spec) const
{
    bool foundBasic = false;
    bool foundSuperKernel = false;
    const auto& objects = kernel.at("objects");
    if (!objects.is_array()) {
        ASCENDLOGE("Selected kernel objects must be an array");
        return ACLRTC_ERROR_FAILURE;
    }
    for (const auto& object : objects) {
        const std::string type = object.at("object_type").get<std::string>();
        ManifestObjectKind kind;
        if (type == "basic") {
            foundBasic = true;
            kind = ManifestObjectKind::Basic;
        } else if (type == "sk") {
            foundSuperKernel = true;
            if (!enableSuperKernel) {
                continue;
            }
            kind = ManifestObjectKind::SuperKernel;
        } else {
            continue;
        }
        const std::string name = object.at("object_name").get<std::string>();
        const auto& commands = object.at("commands");
        if (!commands.is_array() || commands.empty()) {
            ASCENDLOGE("Selected object %s requires a nonempty commands array", name.c_str());
            return ACLRTC_ERROR_FAILURE;
        }
        for (size_t index = 0; index < commands.size(); ++index) {
            ManifestCommand command;
            const aclError result = ParseCommand(commands.at(index), name, kind, index, command);
            if (result != ACLRTC_SUCCESS) {
                return result;
            }
            spec.commands.emplace_back(std::move(command));
        }
        const auto outputs = object.at("outputs").get<std::vector<std::string>>();
        spec.linkInputs.insert(spec.linkInputs.end(), outputs.begin(), outputs.end());
    }
    if (!foundBasic) {
        ASCENDLOGE("Selected kernel is missing a required basic object");
        return ACLRTC_ERROR_FAILURE;
    }
    if (enableSuperKernel && !foundSuperKernel) {
        ASCENDLOGI("Selected kernel has no sk object; compiling basic only despite --enable-super-kernel");
    }
    return ACLRTC_SUCCESS;
}

aclError CompilationManifestParser::ParseSelected(
    const std::string& kernelName, bool enableSuperKernel, CompilationManifest& parsedManifest) const
{
    try {
        if (borrowedManifest_.at("schema_version").get<std::string>() != SCHEMA_VERSION) {
            ASCENDLOGE("Unsupported manifest schema_version; expected %s", SCHEMA_VERSION);
            return ACLRTC_ERROR_FAILURE;
        }
        const auto& kernels = borrowedManifest_.at("kernels");
        if (!kernels.is_array()) {
            ASCENDLOGE("Manifest kernels must be an array");
            return ACLRTC_ERROR_FAILURE;
        }
        const Json* selectedKernel = nullptr;
        for (const auto& kernel : kernels) {
            if (kernel.at("kernel_name").get<std::string>() == kernelName) {
                selectedKernel = &kernel;
                break;
            }
        }
        if (selectedKernel == nullptr) {
            ASCENDLOGE("Kernel entry '%s' is not present in the manifest", kernelName.c_str());
            return ACLRTC_ERROR_FAILURE;
        }
        CompilationManifest spec;
        aclError result = ParseConstants(*selectedKernel, spec.constants);
        if (result != ACLRTC_SUCCESS) {
            return result;
        }
        result = ParseSelectedObjects(*selectedKernel, enableSuperKernel, spec);
        if (result != ACLRTC_SUCCESS) {
            return result;
        }
        if (selectedKernel->contains("link_options")) {
            spec.linkOptions = selectedKernel->at("link_options").get<std::vector<std::string>>();
        }
        parsedManifest = std::move(spec);
        return ACLRTC_SUCCESS;
    } catch (const nlohmann::json::exception& error) {
        ASCENDLOGE("Invalid manifest fields for kernel '%s': %s", kernelName.c_str(), error.what());
        return ACLRTC_ERROR_FAILURE;
    }
}

} // namespace aclrtc
} // namespace ascendc
