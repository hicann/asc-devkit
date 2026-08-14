/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "resource_manifest_validator.h"

#include <map>
#include <set>

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

#include "ascendc_tool_log.h"
#include "file_utils.h"
#include "nlohmann/json.hpp"

namespace ascendc {
namespace manifest_generator {
namespace {

using Json = nlohmann::json;

constexpr const char* RESOURCE_ROOT_MARKER = "${resource}";
constexpr const char* OUTPUT_ROOT_MARKER = "${output}";
constexpr const char* SOURCE_FILE_PATH_MARKER = "${source_file_path}";
constexpr const char* ENVIRONMENT_MARKER_PREFIX = "${env:";
constexpr const char* OPTIONS_MARKER_PREFIX = "${options:";

enum class MarkerRequirement { None, SourceFile };

const std::map<std::string, MarkerRequirement> KNOWN_MARKERS = {
    {RESOURCE_ROOT_MARKER, MarkerRequirement::None},
    {SOURCE_FILE_PATH_MARKER, MarkerRequirement::SourceFile},
};

bool CheckAndLog(bool condition, int lineNumber, const std::string& message) noexcept
{
    if (condition) {
        return true;
    }
    ASCENDLOGE("Manifest validation failed: line=%d reason=%s", lineNumber, message.c_str());
    return false;
}

} // namespace

ResourceManifestValidator::ResourceManifestValidator(const Json& manifest, const std::string& manifestPath)
    : manifest_(manifest),
      manifestPath_(manifestPath),
      manifestRoot_(FileUtils::ParentPath(manifestPath)),
      options_(nullptr),
      hasSourceFile_(false)
{}

bool ResourceManifestValidator::IsIdentifier(const std::string& value)
{
    if (value.empty() ||
        !((value[0] >= 'A' && value[0] <= 'Z') || (value[0] >= 'a' && value[0] <= 'z') || value[0] == '_')) {
        return false;
    }
    for (size_t index = 1U; index < value.size(); ++index) {
        const char character = value[index];
        if (!((character >= 'A' && character <= 'Z') || (character >= 'a' && character <= 'z') ||
              (character >= '0' && character <= '9') || character == '_')) {
            return false;
        }
    }
    return true;
}

bool ResourceManifestValidator::IsNamedMarker(const std::string& marker, const std::string& prefix)
{
    return marker.size() >= prefix.size() + 1U && marker.compare(0U, prefix.size(), prefix) == 0 &&
           marker.back() == '}';
}

std::string ResourceManifestValidator::ExtractNamedMarkerName(const std::string& marker, const std::string& prefix)
{
    return marker.substr(prefix.size(), marker.size() - prefix.size() - 1U);
}

bool ResourceManifestValidator::ValidateManifestBaseFields()
{
    return CheckAndLog(manifest_.is_object(), __LINE__, "invalid manifest: expected an object") &&
           CheckAndLog(manifest_.contains("resource_id"), __LINE__, "missing required field: resource_id") &&
           CheckAndLog(manifest_.at("resource_id").is_string(), __LINE__, "invalid resource_id: expected a string") &&
           CheckAndLog(
               !manifest_.at("resource_id").get_ref<const std::string&>().empty(), __LINE__,
               "invalid resource_id: expected a nonempty string") &&
           CheckAndLog(manifest_.contains("soc_version"), __LINE__, "missing required field: soc_version") &&
           CheckAndLog(manifest_.at("soc_version").is_string(), __LINE__, "invalid soc_version: expected a string") &&
           CheckAndLog(
               !manifest_.at("soc_version").get_ref<const std::string&>().empty(), __LINE__,
               "invalid soc_version: expected a nonempty string") &&
           CheckAndLog(manifest_.contains("resource_path"), __LINE__, "missing required field: resource_path") &&
           CheckAndLog(
               manifest_.at("resource_path").is_string(), __LINE__, "invalid resource_path: expected a string") &&
           CheckAndLog(manifest_.contains("kernels"), __LINE__, "missing required field: kernels") &&
           CheckAndLog(manifest_.at("kernels").is_array(), __LINE__, "invalid kernels: expected an array") &&
           CheckAndLog(!manifest_.at("kernels").empty(), __LINE__, "invalid kernels: expected a nonempty array");
}

bool ResourceManifestValidator::ValidateManifestFields()
{
    std::string sourceFile;
    if (manifest_.contains("source_file")) {
        if (!CheckAndLog(manifest_.at("source_file").is_string(), __LINE__, "invalid source_file: expected a string")) {
            return false;
        }
        sourceFile = manifest_.at("source_file").get_ref<const std::string&>();
        if (!CheckAndLog(
                FileUtils::IsSafeRelativePath(sourceFile) && FileUtils::FileName(sourceFile) == sourceFile, __LINE__,
                "invalid source_file: expected a plain file name")) {
            return false;
        }
        hasSourceFile_ = true;
    }
    if (!resourcePath_.empty() &&
        !CheckAndLog(
            FileUtils::IsSafeRelativePath(resourcePath_), __LINE__,
            "invalid resource_path: expected a safe relative POSIX path, value=" + resourcePath_)) {
        return false;
    }
    if (manifest_.contains("options")) {
        if (!CheckAndLog(manifest_.at("options").is_object(), __LINE__, "invalid options: expected an object")) {
            return false;
        }
        options_ = &manifest_.at("options");
    }

    ASCENDLOGD(
        "Validated manifest fields: resource_id=%s soc_version=%s source_file=%s resource_path=%s",
        manifest_.at("resource_id").get_ref<const std::string&>().c_str(),
        manifest_.at("soc_version").get_ref<const std::string&>().c_str(), sourceFile.c_str(), resourcePath_.c_str());
    return true;
}

bool ResourceManifestValidator::ValidateResourceTarget(const std::string& relativePath)
{
    const std::string target = FileUtils::JoinPath(manifestRoot_, relativePath);
    boost::system::error_code error;
    const boost::filesystem::file_status status =
        boost::filesystem::symlink_status(boost::filesystem::path(target), error);
    return CheckAndLog(
               !error, __LINE__, "failed to inspect resource target: path=" + target + " error=" + error.message()) &&
           CheckAndLog(
               boost::filesystem::exists(status), __LINE__,
               "invalid resource target: path does not exist, path=" + target) &&
           CheckAndLog(
               !boost::filesystem::is_symlink(status), __LINE__,
               "invalid resource target: symlink is not allowed, path=" + target) &&
           CheckAndLog(
               boost::filesystem::is_regular_file(status) || boost::filesystem::is_directory(status), __LINE__,
               "invalid resource target: expected a regular file or directory, path=" + target);
}

bool ResourceManifestValidator::ValidateResourceReference(const std::string& value)
{
    const size_t marker = value.find(RESOURCE_ROOT_MARKER);
    if (marker == std::string::npos) {
        return true;
    }
    const size_t pathBegin = marker + std::char_traits<char>::length(RESOURCE_ROOT_MARKER);
    if (!CheckAndLog(
            value.find(RESOURCE_ROOT_MARKER, pathBegin) == std::string::npos, __LINE__,
            "invalid resource reference: multiple ${resource} markers are not allowed") ||
        !CheckAndLog(
            pathBegin + 1U < value.size() && value[pathBegin] == '/', __LINE__,
            "invalid resource reference: expected ${resource}/<relative-path>")) {
        return false;
    }
    const std::string path = value.substr(pathBegin + 1U);
    return CheckAndLog(
               FileUtils::IsSafeRelativePath(path), __LINE__,
               "invalid resource reference: expected a safe relative POSIX path, path=" + path) &&
           CheckAndLog(
               !resourcePath_.empty(), __LINE__,
               "invalid resource reference: ${resource} requires a nonempty resource_path") &&
           CheckAndLog(
               FileUtils::IsPathWithin(path, resourcePath_), __LINE__,
               "invalid resource reference: path is outside resource_path, path=" + path) &&
           ValidateResourceTarget(path);
}

bool ResourceManifestValidator::ValidateOutputMarker(const std::string& value)
{
    const size_t markerSize = std::char_traits<char>::length(OUTPUT_ROOT_MARKER);
    if (!CheckAndLog(
            value.rfind(OUTPUT_ROOT_MARKER, 0U) == 0U && value.size() > markerSize + 1U && value[markerSize] == '/',
            __LINE__, "invalid output reference: expected ${output}/<file-name>")) {
        return false;
    }
    const std::string path = value.substr(markerSize + 1U);
    return CheckAndLog(
               path.find("${") == std::string::npos, __LINE__,
               "invalid output reference: relative path must not contain another marker") &&
           CheckAndLog(
               FileUtils::IsSafeRelativePath(path), __LINE__,
               "invalid output reference: expected a safe relative POSIX path, path=" + path) &&
           CheckAndLog(
               FileUtils::FileName(path) == path, __LINE__,
               "invalid output reference: expected a file name without a path, value=" + path);
}

bool ResourceManifestValidator::ValidateMarkerSyntax(const std::string& value)
{
    if (!CheckAndLog(
            value.find('\0') == std::string::npos, __LINE__, "invalid command value: embedded NUL is not allowed")) {
        return false;
    }
    size_t begin = 0U;
    while ((begin = value.find("${", begin)) != std::string::npos) {
        const size_t end = value.find('}', begin + 2U);
        if (!CheckAndLog(end != std::string::npos, __LINE__, "invalid command marker: missing closing brace")) {
            return false;
        }
        const std::string marker = value.substr(begin, end - begin + 1U);
        const std::map<std::string, MarkerRequirement>::const_iterator knownMarker = KNOWN_MARKERS.find(marker);
        if (knownMarker != KNOWN_MARKERS.end()) {
            if (knownMarker->second == MarkerRequirement::SourceFile &&
                !CheckAndLog(hasSourceFile_, __LINE__, "invalid ${source_file_path} marker: source_file is required")) {
                return false;
            }
        } else if (IsNamedMarker(marker, ENVIRONMENT_MARKER_PREFIX)) {
            const std::string name = ExtractNamedMarkerName(marker, ENVIRONMENT_MARKER_PREFIX);
            if (!CheckAndLog(
                    IsIdentifier(name), __LINE__,
                    "invalid environment marker name: expected an ASCII identifier, name=" + name)) {
                return false;
            }
        } else {
            return CheckAndLog(false, __LINE__, "unsupported command marker: marker=" + marker);
        }
        begin = end + 1U;
    }
    return ValidateResourceReference(value);
}

bool ResourceManifestValidator::ValidateOptionValue(const std::string& value)
{
    const size_t outputBegin = value.find(OUTPUT_ROOT_MARKER);
    if (outputBegin == std::string::npos) {
        return ValidateMarkerSyntax(value);
    }
    return ValidateOutputMarker(value.substr(outputBegin));
}

bool ResourceManifestValidator::ValidateOptionValue(const std::string& value, std::set<std::string>& referencedOutputs)
{
    if (!ValidateOptionValue(value)) {
        return false;
    }
    const size_t outputBegin = value.find(OUTPUT_ROOT_MARKER);
    if (outputBegin != std::string::npos) {
        referencedOutputs.insert(value.substr(outputBegin));
    }
    return true;
}

bool ResourceManifestValidator::ValidateOptionReference(const std::string& marker)
{
    if (!CheckAndLog(
            IsNamedMarker(marker, OPTIONS_MARKER_PREFIX), __LINE__,
            "invalid options marker: expected ${options:<name>}, marker=" + marker)) {
        return false;
    }
    const std::string name = ExtractNamedMarkerName(marker, OPTIONS_MARKER_PREFIX);
    return CheckAndLog(
               IsIdentifier(name), __LINE__,
               "invalid options marker name: expected an ASCII identifier, name=" + name) &&
           ValidateOptionArray(name);
}

bool ResourceManifestValidator::ValidateOptionArray(const std::string& name)
{
    if (validatedOptions_.count(name) != 0U) {
        return true;
    }
    if (!CheckAndLog(options_ != nullptr, __LINE__, "invalid options marker: manifest options object is missing") ||
        !CheckAndLog(
            options_->contains(name), __LINE__,
            "invalid options marker: referenced field does not exist, name=" + name)) {
        return false;
    }
    const Json& values = options_->at(name);
    if (!CheckAndLog(values.is_array(), __LINE__, "invalid options field: expected an array, name=" + name) ||
        !CheckAndLog(!values.empty(), __LINE__, "invalid options field: expected a nonempty array, name=" + name)) {
        return false;
    }
    for (const Json& value : values) {
        if (!CheckAndLog(value.is_string(), __LINE__, "invalid options item: expected a string, name=" + name) ||
            !ValidateOptionValue(value.get_ref<const std::string&>())) {
            return false;
        }
    }
    validatedOptions_.insert(name);
    return true;
}

bool ResourceManifestValidator::ValidateConstant(const Json& constant, std::set<std::string>& constantNames)
{
    if (!CheckAndLog(constant.is_object(), __LINE__, "invalid constant: expected an object") ||
        !CheckAndLog(constant.contains("name"), __LINE__, "missing required constant field: name") ||
        !CheckAndLog(
            constant.contains("parameter_index"), __LINE__, "missing required constant field: parameter_index") ||
        !CheckAndLog(constant.contains("byte_size"), __LINE__, "missing required constant field: byte_size") ||
        !CheckAndLog(constant.contains("file"), __LINE__, "missing required constant field: file") ||
        !CheckAndLog(constant.contains("template"), __LINE__, "missing required constant field: template") ||
        !CheckAndLog(constant.size() == 5U, __LINE__, "invalid constant: expected exactly five fields") ||
        !CheckAndLog(constant.at("name").is_string(), __LINE__, "invalid constant name: expected a string") ||
        !CheckAndLog(
            !constant.at("name").get_ref<const std::string&>().empty(), __LINE__,
            "invalid constant name: expected a nonempty string") ||
        !CheckAndLog(
            constant.at("parameter_index").is_number_unsigned(), __LINE__,
            "invalid parameter_index: expected an unsigned integer") ||
        !CheckAndLog(
            constant.at("byte_size").is_number_unsigned(), __LINE__,
            "invalid byte_size: expected an unsigned integer") ||
        !CheckAndLog(
            constant.at("byte_size").get<Json::number_unsigned_t>() != 0U, __LINE__,
            "invalid byte_size: expected a nonzero value") ||
        !CheckAndLog(constant.at("file").is_string(), __LINE__, "invalid constant file: expected a string") ||
        !CheckAndLog(constant.at("template").is_string(), __LINE__, "invalid constant template: expected a string") ||
        !CheckAndLog(
            !constant.at("template").get_ref<const std::string&>().empty(), __LINE__,
            "invalid constant template: expected a nonempty string")) {
        return false;
    }

    const std::string name = constant.at("name").get_ref<const std::string&>();
    const std::string file = constant.at("file").get_ref<const std::string&>();
    if (!CheckAndLog(
            file.rfind(std::string(RESOURCE_ROOT_MARKER) + "/", 0U) == 0U, __LINE__,
            "invalid constant file: expected ${resource}/<relative-path>") ||
        !ValidateResourceReference(file) ||
        !CheckAndLog(constantNames.insert(name).second, __LINE__, "duplicate constant name: name=" + name)) {
        return false;
    }

    ASCENDLOGD(
        "Validated constant: name=%s parameter_index=%llu byte_size=%llu", name.c_str(),
        static_cast<unsigned long long>(constant.at("parameter_index").get<Json::number_unsigned_t>()),
        static_cast<unsigned long long>(constant.at("byte_size").get<Json::number_unsigned_t>()));
    return true;
}

bool ResourceManifestValidator::ValidateConstants(const Json& constantInfos)
{
    std::set<std::string> constantNames;
    for (const Json& constant : constantInfos) {
        if (!ValidateConstant(constant, constantNames)) {
            return false;
        }
    }
    return true;
}

bool ResourceManifestValidator::ValidateObjectOutputs(const Json& outputs)
{
    for (const Json& output : outputs) {
        if (!CheckAndLog(output.is_string(), __LINE__, "invalid object output: expected a string") ||
            !ValidateOutputMarker(output.get_ref<const std::string&>())) {
            return false;
        }
    }
    return true;
}

bool ResourceManifestValidator::ValidateObjectOutputReferences(
    const Json& outputs, const std::set<std::string>& referencedOutputs)
{
    for (const Json& output : outputs) {
        const std::string value = output.get_ref<const std::string&>();
        if (!CheckAndLog(
                referencedOutputs.count(value) != 0U, __LINE__,
                "invalid object output: output is not referenced by its commands, value=" + value)) {
            return false;
        }
    }
    ASCENDLOGD(
        "Validated object output references: declared=%zu referenced=%zu", outputs.size(), referencedOutputs.size());
    return true;
}

bool ResourceManifestValidator::ValidateCommandOptions(
    const Json& commandValues, std::set<std::string>& referencedOutputs)
{
    for (const Json& option : commandValues) {
        if (!CheckAndLog(option.is_string(), __LINE__, "invalid command option: expected a string")) {
            return false;
        }
        const std::string value = option.get_ref<const std::string&>();
        if (value.find(OPTIONS_MARKER_PREFIX) != std::string::npos) {
            if (!ValidateOptionReference(value)) {
                return false;
            }
            continue;
        }
        if (!ValidateOptionValue(value, referencedOutputs)) {
            return false;
        }
    }
    return true;
}

bool ResourceManifestValidator::ValidateCommand(const Json& command, std::set<std::string>& referencedOutputs)
{
    if (!CheckAndLog(command.is_object(), __LINE__, "invalid command: expected an object") ||
        !CheckAndLog(command.contains("type"), __LINE__, "missing required command field: type") ||
        !CheckAndLog(command.at("type").is_string(), __LINE__, "invalid command type: expected a string") ||
        !CheckAndLog(command.contains("stage"), __LINE__, "missing required command field: stage") ||
        !CheckAndLog(
            command.at("stage").is_number_unsigned(), __LINE__,
            "invalid command stage: expected an unsigned integer") ||
        !CheckAndLog(command.contains("cmd"), __LINE__, "missing required command field: cmd") ||
        !CheckAndLog(command.at("cmd").is_array(), __LINE__, "invalid command field cmd: expected an array")) {
        return false;
    }

    const std::string type = command.at("type").get_ref<const std::string&>();
    if (!ValidateCommandOptions(command.at("cmd"), referencedOutputs)) {
        return false;
    }
    ASCENDLOGD(
        "Validated command: type=%s stage=%llu cmd=%zu", type.c_str(),
        static_cast<unsigned long long>(command.at("stage").get<Json::number_unsigned_t>()), command.at("cmd").size());
    return true;
}

bool ResourceManifestValidator::ValidateCommands(const Json& commands, std::set<std::string>& referencedOutputs)
{
    for (const Json& command : commands) {
        if (!ValidateCommand(command, referencedOutputs)) {
            return false;
        }
    }
    return true;
}

bool ResourceManifestValidator::ValidateObject(const Json& object, std::set<std::string>& objectNames)
{
    if (!CheckAndLog(object.is_object(), __LINE__, "invalid object: expected an object") ||
        !CheckAndLog(object.contains("object_name"), __LINE__, "missing required object field: object_name") ||
        !CheckAndLog(object.at("object_name").is_string(), __LINE__, "invalid object_name: expected a string") ||
        !CheckAndLog(
            !object.at("object_name").get_ref<const std::string&>().empty(), __LINE__,
            "invalid object_name: expected a nonempty string") ||
        !CheckAndLog(object.contains("object_type"), __LINE__, "missing required object field: object_type") ||
        !CheckAndLog(object.at("object_type").is_string(), __LINE__, "invalid object_type: expected a string") ||
        !CheckAndLog(object.contains("commands"), __LINE__, "missing required object field: commands") ||
        !CheckAndLog(object.at("commands").is_array(), __LINE__, "invalid commands: expected an array") ||
        !CheckAndLog(!object.at("commands").empty(), __LINE__, "invalid commands: expected a nonempty array") ||
        !CheckAndLog(object.contains("outputs"), __LINE__, "missing required object field: outputs") ||
        !CheckAndLog(object.at("outputs").is_array(), __LINE__, "invalid outputs: expected an array") ||
        !CheckAndLog(!object.at("outputs").empty(), __LINE__, "invalid outputs: expected a nonempty array")) {
        return false;
    }

    const std::string objectName = object.at("object_name").get_ref<const std::string&>();
    const std::string objectType = object.at("object_type").get_ref<const std::string&>();
    if (!CheckAndLog(
            FileUtils::IsSafeRelativePath(objectName) && FileUtils::FileName(objectName) == objectName, __LINE__,
            "invalid object_name: expected a plain name") ||
        !CheckAndLog(
            objectNames.insert(objectName).second, __LINE__, "duplicate object_name: object_name=" + objectName) ||
        !ValidateObjectOutputs(object.at("outputs"))) {
        return false;
    }

    std::set<std::string> referencedOutputs;
    if (!ValidateCommands(object.at("commands"), referencedOutputs) ||
        !ValidateObjectOutputReferences(object.at("outputs"), referencedOutputs)) {
        return false;
    }
    ASCENDLOGD(
        "Validated object: object_name=%s object_type=%s commands=%zu outputs=%zu", objectName.c_str(),
        objectType.c_str(), object.at("commands").size(), object.at("outputs").size());
    return true;
}

bool ResourceManifestValidator::ValidateObjects(const Json& objects)
{
    std::set<std::string> objectNames;
    for (const Json& object : objects) {
        if (!ValidateObject(object, objectNames)) {
            return false;
        }
    }
    return true;
}

bool ResourceManifestValidator::ValidateKernel(const Json& kernel, std::set<std::string>& kernelNames)
{
    if (!CheckAndLog(kernel.is_object(), __LINE__, "invalid kernel: expected an object") ||
        !CheckAndLog(kernel.contains("kernel_name"), __LINE__, "missing required kernel field: kernel_name") ||
        !CheckAndLog(kernel.at("kernel_name").is_string(), __LINE__, "invalid kernel_name: expected a string") ||
        !CheckAndLog(
            !kernel.at("kernel_name").get_ref<const std::string&>().empty(), __LINE__,
            "invalid kernel_name: expected a nonempty string") ||
        !CheckAndLog(
            !kernel.contains("constant_infos") || kernel.at("constant_infos").is_array(), __LINE__,
            "invalid constant_infos: expected an array") ||
        !CheckAndLog(kernel.contains("objects"), __LINE__, "missing required kernel field: objects") ||
        !CheckAndLog(kernel.at("objects").is_array(), __LINE__, "invalid objects: expected an array") ||
        !CheckAndLog(!kernel.at("objects").empty(), __LINE__, "invalid objects: expected a nonempty array") ||
        !CheckAndLog(
            !kernel.contains("link_options") || kernel.at("link_options").is_array(), __LINE__,
            "invalid link_options: expected an array")) {
        return false;
    }

    const std::string kernelName = kernel.at("kernel_name").get_ref<const std::string&>();
    if (!CheckAndLog(
            kernelNames.insert(kernelName).second, __LINE__, "duplicate kernel_name: kernel_name=" + kernelName) ||
        (kernel.contains("constant_infos") && !ValidateConstants(kernel.at("constant_infos"))) ||
        !ValidateObjects(kernel.at("objects"))) {
        return false;
    }
    ASCENDLOGD(
        "Validated kernel: kernel_name=%s constants=%zu objects=%zu link_options=%zu", kernelName.c_str(),
        kernel.contains("constant_infos") ? kernel.at("constant_infos").size() : 0U, kernel.at("objects").size(),
        kernel.contains("link_options") ? kernel.at("link_options").size() : 0U);
    return true;
}

bool ResourceManifestValidator::Validate()
{
    options_ = nullptr;
    validatedOptions_.clear();
    hasSourceFile_ = false;

    if (!ValidateManifestBaseFields()) {
        return false;
    }
    ASCENDLOGD("Manifest JSON and base fields are valid: path=%s", manifestPath_.c_str());

    resourcePath_ = manifest_.at("resource_path").get_ref<const std::string&>();
    if (!ValidateManifestFields()) {
        return false;
    }

    std::set<std::string> kernelNames;
    for (const Json& kernel : manifest_.at("kernels")) {
        if (!ValidateKernel(kernel, kernelNames)) {
            return false;
        }
    }
    ASCENDLOGI(
        "Manifest validated successfully: resource_id=%s soc_version=%s kernels=%zu",
        manifest_.at("resource_id").get_ref<const std::string&>().c_str(),
        manifest_.at("soc_version").get_ref<const std::string&>().c_str(), manifest_.at("kernels").size());
    return true;
}

bool ValidateResourceManifest(const Json& manifest, const std::string& manifestPath)
{
    ASCENDLOGI("Validating resource manifest: path=%s", manifestPath.c_str());
    const bool valid = ResourceManifestValidator(manifest, manifestPath).Validate();
    if (!valid) {
        ASCENDLOGE("Resource manifest validation failed: path=%s", manifestPath.c_str());
    }
    return valid;
}

} // namespace manifest_generator
} // namespace ascendc
