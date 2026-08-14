/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef ASCENDC_MANIFEST_GENERATOR_RESOURCE_MANIFEST_VALIDATOR_H
#define ASCENDC_MANIFEST_GENERATOR_RESOURCE_MANIFEST_VALIDATOR_H

#include <set>
#include <string>

#include <nlohmann/json_fwd.hpp>

namespace ascendc {
namespace manifest_generator {

class ResourceManifestValidator final {
public:
    ResourceManifestValidator(const nlohmann::json& manifest, const std::string& manifestPath);

    bool Validate();

private:
    bool ValidateManifestBaseFields();
    bool ValidateManifestFields();
    bool ValidateKernel(const nlohmann::json& kernel, std::set<std::string>& kernelNames);
    bool ValidateConstants(const nlohmann::json& constantInfos);
    bool ValidateConstant(const nlohmann::json& constant, std::set<std::string>& constantNames);
    bool ValidateObjects(const nlohmann::json& objects);
    bool ValidateObject(const nlohmann::json& object, std::set<std::string>& objectNames);
    bool ValidateCommands(const nlohmann::json& commands, std::set<std::string>& referencedOutputs);
    bool ValidateCommand(const nlohmann::json& command, std::set<std::string>& referencedOutputs);
    bool ValidateCommandOptions(const nlohmann::json& commandValues, std::set<std::string>& referencedOutputs);
    bool ValidateObjectOutputs(const nlohmann::json& outputs);
    bool ValidateObjectOutputReferences(const nlohmann::json& outputs, const std::set<std::string>& referencedOutputs);
    bool ValidateOptionValue(const std::string& value);
    bool ValidateOptionValue(const std::string& value, std::set<std::string>& referencedOutputs);
    bool ValidateMarkerSyntax(const std::string& value);
    bool ValidateOptionReference(const std::string& marker);
    bool ValidateOptionArray(const std::string& name);
    bool ValidateResourceReference(const std::string& value);
    bool ValidateResourceTarget(const std::string& relativePath);
    bool ValidateOutputMarker(const std::string& value);

    static bool IsIdentifier(const std::string& value);
    static bool IsNamedMarker(const std::string& marker, const std::string& prefix);
    static std::string ExtractNamedMarkerName(const std::string& marker, const std::string& prefix);

    const nlohmann::json& manifest_;
    std::string manifestPath_;
    std::string manifestRoot_;
    std::string resourcePath_;
    const nlohmann::json* options_;
    std::set<std::string> validatedOptions_;
    bool hasSourceFile_;
};

bool ValidateResourceManifest(const nlohmann::json& manifest, const std::string& manifestPath);

} // namespace manifest_generator
} // namespace ascendc

#endif // ASCENDC_MANIFEST_GENERATOR_RESOURCE_MANIFEST_VALIDATOR_H
