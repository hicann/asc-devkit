/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "kernel_compilation_plan_builder.h"

#include "ascendc_tool_log.h"

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>
#include <cstdlib>
#include <iomanip>
#include <set>
#include <sstream>
#include <unistd.h>

namespace ascendc {
namespace aclrtc {
namespace {
namespace fs = boost::filesystem;
using Json = nlohmann::json;

constexpr char SUPPORTED_MANIFEST_SCHEMA_VERSION[] = "1.0";
constexpr char OPTIONS_REFERENCE_PREFIX[] = "${options:";
constexpr char ENVIRONMENT_REFERENCE_PREFIX[] = "${env:";

enum class ManifestObjectKind : uint32_t {
    Basic,
    SuperKernel,
};

struct SelectedManifestObject {
    const Json* manifestObject{nullptr};
    ManifestObjectKind objectKind{ManifestObjectKind::Basic};
};

struct ManifestExpansionContext {
    const Json& resourceManifest;
    const fs::path& resourceWorktreePath;
    const fs::path& outputDirectoryPath;
    const fs::path& externalSourceDirectoryPath;
};

void ReplaceAllLiteralOccurrences(std::string& text, const std::string& literalText, const std::string& replacementText)
{
    size_t occurrencePosition = 0U;
    while ((occurrencePosition = text.find(literalText, occurrencePosition)) != std::string::npos) {
        text.replace(occurrencePosition, literalText.size(), replacementText);
        occurrencePosition += replacementText.size();
    }
}

bool ExpandEnvironmentVariablePlaceholders(
    const std::string& text, const std::string& manifestValueLocation, std::string& resolvedText)
{
    resolvedText.clear();
    size_t sourcePosition = 0U;
    while (sourcePosition < text.size()) {
        const size_t referencePosition = text.find(ENVIRONMENT_REFERENCE_PREFIX, sourcePosition);
        if (referencePosition == std::string::npos) {
            resolvedText.append(text, sourcePosition, std::string::npos);
            break;
        }
        resolvedText.append(text, sourcePosition, referencePosition - sourcePosition);
        const size_t namePosition = referencePosition + sizeof(ENVIRONMENT_REFERENCE_PREFIX) - 1U;
        const size_t closingBracePosition = text.find('}', namePosition);
        if (closingBracePosition == std::string::npos) {
            ASCENDLOGE(
                "Unterminated environment variable placeholder at %s byte %zu; regenerate the JIT resource "
                "using the form ${env:NAME}",
                manifestValueLocation.c_str(), referencePosition);
            return false;
        }
        if (closingBracePosition == namePosition) {
            ASCENDLOGE(
                "Environment variable placeholder at %s byte %zu has an empty name; regenerate the JIT resource "
                "using the form ${env:NAME}",
                manifestValueLocation.c_str(), referencePosition);
            return false;
        }
        const std::string environmentName = text.substr(namePosition, closingBracePosition - namePosition);
        const char* environmentValue = std::getenv(environmentName.c_str());
        if (environmentValue == nullptr) {
            ASCENDLOGE(
                "Environment variable %s referenced at %s byte %zu is not set; "
                "set it with: export %s=<value>",
                environmentName.c_str(), manifestValueLocation.c_str(), referencePosition, environmentName.c_str());
            return false;
        }
        resolvedText.append(environmentValue);
        sourcePosition = closingBracePosition + 1U;
    }
    return true;
}

bool ExpandPathAndEnvironmentPlaceholders(
    const std::string& manifestText, const std::string& manifestValueLocation,
    const ManifestExpansionContext& expansionContext, std::string& resolvedText)
{
    std::string pathResolvedText = manifestText;
    ReplaceAllLiteralOccurrences(pathResolvedText, "${resource}", expansionContext.resourceWorktreePath.string());
    ReplaceAllLiteralOccurrences(pathResolvedText, "${output}", expansionContext.outputDirectoryPath.string());
    ReplaceAllLiteralOccurrences(
        pathResolvedText, "${source_file_path}", expansionContext.externalSourceDirectoryPath.string());
    return ExpandEnvironmentVariablePlaceholders(pathResolvedText, manifestValueLocation, resolvedText);
}

bool ParseWholeArgumentOptionsPlaceholder(const std::string& manifestArgument, std::string& optionName)
{
    if (manifestArgument.compare(0, sizeof(OPTIONS_REFERENCE_PREFIX) - 1U, OPTIONS_REFERENCE_PREFIX) != 0 ||
        manifestArgument.size() <= sizeof(OPTIONS_REFERENCE_PREFIX) || manifestArgument.back() != '}') {
        return false;
    }
    optionName = manifestArgument.substr(
        sizeof(OPTIONS_REFERENCE_PREFIX) - 1U, manifestArgument.size() - sizeof(OPTIONS_REFERENCE_PREFIX));
    return !optionName.empty();
}

aclError ExpandManifestArgument(
    const std::string& manifestArgument, const std::string& argumentLocation,
    const ManifestExpansionContext& expansionContext, std::vector<std::string>& expandedArguments)
{
    std::string optionName;
    if (!ParseWholeArgumentOptionsPlaceholder(manifestArgument, optionName)) {
        std::string resolvedArgument;
        if (!ExpandPathAndEnvironmentPlaceholders(
                manifestArgument, argumentLocation, expansionContext, resolvedArgument)) {
            return ACLRTC_ERROR_FAILURE;
        }
        expandedArguments.emplace_back(std::move(resolvedArgument));
        return ACLRTC_SUCCESS;
    }

    const auto optionTable = expansionContext.resourceManifest.find("options");
    if (optionTable == expansionContext.resourceManifest.end() || !optionTable->is_object() ||
        optionTable->find(optionName) == optionTable->end()) {
        ASCENDLOGE(
            "Manifest value %s references undefined options key '%s'; define this key in the top-level options "
            "object and regenerate the JIT resource",
            argumentLocation.c_str(), optionName.c_str());
        return ACLRTC_ERROR_FAILURE;
    }
    const std::vector<std::string> optionArguments = optionTable->at(optionName).get<std::vector<std::string>>();
    for (size_t optionIndex = 0U; optionIndex < optionArguments.size(); ++optionIndex) {
        std::string resolvedArgument;
        const std::string optionLocation = "options['" + optionName + "'][" + std::to_string(optionIndex) + "]";
        if (!ExpandPathAndEnvironmentPlaceholders(
                optionArguments[optionIndex], optionLocation, expansionContext, resolvedArgument)) {
            return ACLRTC_ERROR_FAILURE;
        }
        expandedArguments.emplace_back(std::move(resolvedArgument));
    }
    return ACLRTC_SUCCESS;
}

bool IsAbsoluteExecutableRegularFile(const fs::path& executablePath)
{
    boost::system::error_code fileError;
    return executablePath.is_absolute() && fs::is_regular_file(executablePath, fileError) && !fileError &&
           access(executablePath.c_str(), X_OK) == 0;
}

bool IsMissingPathError(const boost::system::error_code& pathError) noexcept
{
    return pathError.value() == ENOENT || pathError.value() == ENOTDIR;
}

bool FindSeparatedCompilerOutputOptionPosition(
    const std::vector<std::string>& commandArguments, size_t& outputOptionPosition)
{
    for (size_t argumentIndex = 0; argumentIndex < commandArguments.size(); ++argumentIndex) {
        const std::string& argument = commandArguments[argumentIndex];
        if (argument != "-o") {
            continue;
        }
        if (argumentIndex + 1U < commandArguments.size() && !commandArguments[argumentIndex + 1U].empty()) {
            outputOptionPosition = argumentIndex;
            return true;
        }
    }
    return false;
}

std::string FormatBytesAsCppInitializerList(const void* constantAddress, uint64_t constantByteCount)
{
    const auto* constantBytes = static_cast<const uint8_t*>(constantAddress);
    std::ostringstream encodedBytes;
    encodedBytes << '{';
    for (uint64_t byteIndex = 0; byteIndex < constantByteCount; ++byteIndex) {
        if (byteIndex != 0U) {
            encodedBytes << ", ";
        }
        encodedBytes << "0x" << std::hex << std::nouppercase << std::setw(2) << std::setfill('0')
                     << static_cast<unsigned int>(constantBytes[byteIndex]);
    }
    encodedBytes << '}';
    return encodedBytes.str();
}

aclError BuildConstantSourcePatches(
    const Json& selectedKernel, const NormalizedKernelSpecializationRequest& specializationRequest,
    const ManifestExpansionContext& expansionContext, std::vector<KernelSourcePatch>& sourcePatches)
{
    const auto constantInfos = selectedKernel.find("constant_infos");
    if (constantInfos == selectedKernel.end() || constantInfos->empty()) {
        ASCENDLOGE("Selected kernel must contain at least one constant_info for static compilation; regenerate the JIT "
                   "resource with its static constant metadata");
        return ACLRTC_ERROR_FAILURE;
    }
    for (size_t constantIndex = 0U; constantIndex < constantInfos->size(); ++constantIndex) {
        const Json& constantInfo = constantInfos->at(constantIndex);
        const uint64_t constantArgumentIndex = constantInfo.at("parameter_index").get<uint64_t>();
        const uint64_t requiredConstantByteCount = constantInfo.at("byte_size").get<uint64_t>();
        if (specializationRequest.borrowedKernelArgumentDataPointers == nullptr ||
            specializationRequest.borrowedKernelArgumentByteCounts == nullptr) {
            ASCENDLOGE(
                "constant parameter_index %llu requires argsAddr and argsBytes; set both request arrays with at "
                "least argsCount entries",
                static_cast<unsigned long long>(constantArgumentIndex));
            return ACLRTC_ERROR_INVALID_INPUT;
        }
        if (constantArgumentIndex >= specializationRequest.kernelArgumentCount) {
            ASCENDLOGE(
                "constant parameter_index %llu is outside argsCount %llu; set argsCount greater than %llu and "
                "provide matching argsAddr and argsBytes entries",
                static_cast<unsigned long long>(constantArgumentIndex),
                static_cast<unsigned long long>(specializationRequest.kernelArgumentCount),
                static_cast<unsigned long long>(constantArgumentIndex));
            return ACLRTC_ERROR_INVALID_INPUT;
        }
        if (specializationRequest.borrowedKernelArgumentDataPointers[constantArgumentIndex] == nullptr) {
            ASCENDLOGE(
                "argsAddr[%llu] must not be null for a static constant; set it to the constant data buffer",
                static_cast<unsigned long long>(constantArgumentIndex));
            return ACLRTC_ERROR_INVALID_INPUT;
        }
        const uint64_t suppliedConstantByteCount =
            specializationRequest.borrowedKernelArgumentByteCounts[constantArgumentIndex];
        if (suppliedConstantByteCount != requiredConstantByteCount) {
            ASCENDLOGE(
                "argsBytes[%llu]=%llu does not match required byte_size %llu; set argsBytes[%llu] to %llu",
                static_cast<unsigned long long>(constantArgumentIndex),
                static_cast<unsigned long long>(suppliedConstantByteCount),
                static_cast<unsigned long long>(requiredConstantByteCount),
                static_cast<unsigned long long>(constantArgumentIndex),
                static_cast<unsigned long long>(requiredConstantByteCount));
            return ACLRTC_ERROR_INVALID_INPUT;
        }

        std::string resolvedTargetFilePath;
        const std::string constantFileLocation = "constant_infos[" + std::to_string(constantIndex) + "].file";
        if (!ExpandPathAndEnvironmentPlaceholders(
                constantInfo.at("file").get<std::string>(), constantFileLocation, expansionContext,
                resolvedTargetFilePath)) {
            return ACLRTC_ERROR_FAILURE;
        }
        sourcePatches.push_back(
            {resolvedTargetFilePath, constantInfo.at("template").get<std::string>(),
             FormatBytesAsCppInitializerList(
                 specializationRequest.borrowedKernelArgumentDataPointers[constantArgumentIndex],
                 requiredConstantByteCount)});
    }
    return ACLRTC_SUCCESS;
}

aclError BuildManifestCommand(
    const Json& manifestCommand, const SelectedManifestObject& selectedObject, size_t commandIndex,
    const NormalizedKernelSpecializationRequest& specializationRequest,
    const ManifestExpansionContext& expansionContext, CompilationCommand& compilationCommand)
{
    const std::string manifestObjectName = selectedObject.manifestObject->at("object_name").get<std::string>();
    const std::string commandType = manifestCommand.at("type").get<std::string>();
    if (commandType == "compile") {
        compilationCommand.commandKind = CompilationCommandKind::Compile;
    } else if (commandType == "objcopy") {
        compilationCommand.commandKind = CompilationCommandKind::ObjectCopy;
    } else {
        ASCENDLOGE(
            "Manifest object %s command[%zu] has unsupported type '%s'; regenerate the JIT resource using a "
            "command type supported by ACLRTC",
            manifestObjectName.c_str(), commandIndex, commandType.c_str());
        return ACLRTC_ERROR_FAILURE;
    }
    const auto parallelStage = manifestCommand.find("stage");
    if (parallelStage != manifestCommand.end()) {
        compilationCommand.parallelStage = parallelStage->get<uint32_t>();
    }
    compilationCommand.diagnosticLabel =
        manifestObjectName + "/" + commandType + "[" + std::to_string(commandIndex) + "]";

    const std::vector<std::string> manifestArguments = manifestCommand.at("cmd").get<std::vector<std::string>>();
    if (manifestArguments.empty()) {
        ASCENDLOGE(
            "Manifest command %s has an empty cmd array; regenerate the JIT resource",
            compilationCommand.diagnosticLabel.c_str());
        return ACLRTC_ERROR_FAILURE;
    }
    std::vector<std::string> expandedExecutable;
    aclError result = ExpandManifestArgument(
        manifestArguments.front(), compilationCommand.diagnosticLabel + ".cmd[0]", expansionContext,
        expandedExecutable);
    if (result != ACLRTC_SUCCESS) {
        return result;
    }
    if (expandedExecutable.size() != 1U) {
        ASCENDLOGE(
            "Manifest command %s executable expanded to %zu argv items; regenerate the JIT resource so cmd[0] "
            "resolves to exactly one executable path",
            compilationCommand.diagnosticLabel.c_str(), expandedExecutable.size());
        return ACLRTC_ERROR_FAILURE;
    }
    compilationCommand.executablePath = std::move(expandedExecutable.front());
    if (!IsAbsoluteExecutableRegularFile(compilationCommand.executablePath)) {
        ASCENDLOGE(
            "Manifest command %s requires an absolute path to an existing executable regular file: path=%s; "
            "check the referenced environment variable and tool installation",
            compilationCommand.diagnosticLabel.c_str(), compilationCommand.executablePath.c_str());
        return ACLRTC_ERROR_FAILURE;
    }

    for (size_t argumentIndex = 1U; argumentIndex < manifestArguments.size(); ++argumentIndex) {
        const std::string argumentLocation =
            compilationCommand.diagnosticLabel + ".cmd[" + std::to_string(argumentIndex) + "]";
        result = ExpandManifestArgument(
            manifestArguments[argumentIndex], argumentLocation, expansionContext, compilationCommand.commandArguments);
        if (result != ACLRTC_SUCCESS) {
            return result;
        }
    }

    if (compilationCommand.commandKind == CompilationCommandKind::Compile) {
        size_t outputOptionPosition = 0U;
        if (!FindSeparatedCompilerOutputOptionPosition(compilationCommand.commandArguments, outputOptionPosition)) {
            ASCENDLOGE(
                "Manifest compile command %s must contain a valid separated '-o' output option and a non-empty "
                "output path; regenerate the JIT resource with '-o' followed by its output path",
                compilationCommand.diagnosticLabel.c_str());
            return ACLRTC_ERROR_FAILURE;
        }
        const std::vector<std::string>& userOptions = selectedObject.objectKind == ManifestObjectKind::Basic ?
                                                          specializationRequest.compilerOptions.basicOptions :
                                                          specializationRequest.compilerOptions.superKernelOptions;
        compilationCommand.commandArguments.insert(
            compilationCommand.commandArguments.begin() + static_cast<std::ptrdiff_t>(outputOptionPosition),
            userOptions.begin(), userOptions.end());
    }
    return ACLRTC_SUCCESS;
}

aclError SelectManifestObjectsForCompilation(
    const Json& selectedKernel, CompilationVariant compilationVariant,
    std::vector<SelectedManifestObject>& selectedObjects)
{
    bool foundBasicObject = false;
    bool foundSuperKernelObject = false;
    for (const Json& manifestObject : selectedKernel.at("objects")) {
        const std::string objectType = manifestObject.at("object_type").get<std::string>();
        if (objectType == "basic") {
            foundBasicObject = true;
            selectedObjects.push_back({&manifestObject, ManifestObjectKind::Basic});
        } else if (objectType == "sk") {
            foundSuperKernelObject = true;
            if (compilationVariant == CompilationVariant::BasicWithSuperKernel) {
                selectedObjects.push_back({&manifestObject, ManifestObjectKind::SuperKernel});
            }
        }
    }
    if (!foundBasicObject) {
        ASCENDLOGE("Selected kernel does not contain a basic object; regenerate the JIT resource");
        return ACLRTC_ERROR_FAILURE;
    }
    if (compilationVariant == CompilationVariant::BasicWithSuperKernel && !foundSuperKernelObject) {
        ASCENDLOGE("Super-kernel compilation was requested but the selected kernel has no sk object; remove "
                   "--enable-super-kernel or use a JIT resource that contains an sk object");
        return ACLRTC_ERROR_FAILURE;
    }
    return ACLRTC_SUCCESS;
}

aclError BuildLinkCommand(
    const Json& selectedKernel, const std::vector<SelectedManifestObject>& selectedObjects,
    const ManifestExpansionContext& expansionContext, CompilationCommand& linkCommand,
    std::set<fs::path>& requiredOutputDirectories, fs::path& linkedKernelElfPath)
{
    linkCommand.commandKind = CompilationCommandKind::Link;
    linkCommand.diagnosticLabel = "link";
    std::string linkerPath;
    if (!ExpandPathAndEnvironmentPlaceholders(
            "${env:ASCEND_HOME_PATH}/bin/ld.lld", "internal linker executable", expansionContext, linkerPath)) {
        return ACLRTC_ERROR_FAILURE;
    }
    linkCommand.executablePath = linkerPath;

    const auto linkOptions = selectedKernel.find("link_options");
    if (linkOptions != selectedKernel.end()) {
        const std::vector<std::string> configuredLinkOptions = linkOptions->get<std::vector<std::string>>();
        for (size_t linkOptionIndex = 0U; linkOptionIndex < configuredLinkOptions.size(); ++linkOptionIndex) {
            std::string resolvedLinkOption;
            const std::string linkOptionLocation = "link_options[" + std::to_string(linkOptionIndex) + "]";
            if (!ExpandPathAndEnvironmentPlaceholders(
                    configuredLinkOptions[linkOptionIndex], linkOptionLocation, expansionContext, resolvedLinkOption)) {
                return ACLRTC_ERROR_FAILURE;
            }
            linkCommand.commandArguments.emplace_back(std::move(resolvedLinkOption));
        }
    }
    for (const SelectedManifestObject& selectedObject : selectedObjects) {
        const std::string manifestObjectName = selectedObject.manifestObject->at("object_name").get<std::string>();
        const std::vector<std::string> outputs =
            selectedObject.manifestObject->at("outputs").get<std::vector<std::string>>();
        for (size_t outputIndex = 0U; outputIndex < outputs.size(); ++outputIndex) {
            std::string resolvedOutput;
            const std::string outputLocation = manifestObjectName + ".outputs[" + std::to_string(outputIndex) + "]";
            if (!ExpandPathAndEnvironmentPlaceholders(
                    outputs[outputIndex], outputLocation, expansionContext, resolvedOutput)) {
                return ACLRTC_ERROR_FAILURE;
            }
            linkCommand.commandArguments.emplace_back(resolvedOutput);
            requiredOutputDirectories.emplace(fs::path(resolvedOutput).parent_path());
        }
    }
    linkedKernelElfPath = expansionContext.outputDirectoryPath / "linked_kernel.elf";
    requiredOutputDirectories.emplace(linkedKernelElfPath.parent_path());
    linkCommand.commandArguments.emplace_back("-o");
    linkCommand.commandArguments.emplace_back(linkedKernelElfPath.string());
    if (!IsAbsoluteExecutableRegularFile(linkCommand.executablePath)) {
        ASCENDLOGE(
            "Linker requires an absolute path to an existing executable regular file: path=%s; verify "
            "ASCEND_HOME_PATH points to a complete CANN installation",
            linkCommand.executablePath.c_str());
        return ACLRTC_ERROR_FAILURE;
    }
    return ACLRTC_SUCCESS;
}
} // namespace

KernelCompilationPlanBuilder::KernelCompilationPlanBuilder(
    const NormalizedKernelSpecializationRequest& specializationRequest, const nlohmann::json& resourceManifest,
    fs::path resourceWorktreePath, fs::path externalSourceDirectoryPath)
    : specializationRequest_(specializationRequest),
      resourceManifest_(resourceManifest),
      resourceWorktreePath_(std::move(resourceWorktreePath)),
      externalSourceDirectoryPath_(std::move(externalSourceDirectoryPath))
{}

aclError KernelCompilationPlanBuilder::BuildCompilationPlan(KernelCompilationPlan& compilationPlan) const
{
    try {
        if (!resourceWorktreePath_.is_absolute()) {
            ASCENDLOGE(
                "ResourceRegistry returned a non-absolute worktree path: %s; fix the registry integration",
                resourceWorktreePath_.c_str());
            return ACLRTC_ERROR_FAILURE;
        }
        boost::system::error_code worktreeError;
        const bool worktreeIsDirectory = fs::is_directory(resourceWorktreePath_, worktreeError);
        if (worktreeError && !IsMissingPathError(worktreeError)) {
            ASCENDLOGE(
                "Failed to inspect resource worktree: path=%s error=%d message=%s; fix the registry "
                "integration",
                resourceWorktreePath_.c_str(), worktreeError.value(), worktreeError.message().c_str());
            return ACLRTC_ERROR_FAILURE;
        }
        if (!worktreeIsDirectory) {
            ASCENDLOGE(
                "Resource worktree must be an existing directory: path=%s; fix the registry integration",
                resourceWorktreePath_.c_str());
            return ACLRTC_ERROR_FAILURE;
        }
        const std::string schemaVersion = resourceManifest_.at("schema_version").get<std::string>();
        if (schemaVersion != SUPPORTED_MANIFEST_SCHEMA_VERSION) {
            ASCENDLOGE(
                "Unsupported Manifest schema_version '%s'; expected '%s'. Regenerate the JIT resource with a "
                "compatible pack tool",
                schemaVersion.c_str(), SUPPORTED_MANIFEST_SCHEMA_VERSION);
            return ACLRTC_ERROR_FAILURE;
        }

        const Json* selectedKernel = nullptr;
        for (const Json& kernel : resourceManifest_.at("kernels")) {
            if (kernel.at("kernel_name").get<std::string>() == specializationRequest_.kernelName) {
                selectedKernel = &kernel;
                break;
            }
        }
        if (selectedKernel == nullptr) {
            ASCENDLOGE(
                "Kernel entry '%s' is not present in JIT resource '%s'; correct kernelEntry or resourceId",
                specializationRequest_.kernelName.c_str(), specializationRequest_.resourceId.c_str());
            return ACLRTC_ERROR_FAILURE;
        }

        KernelCompilationPlan builtPlan;
        const fs::path outputDirectoryPath =
            resourceWorktreePath_ / (".aclrtc_" + specializationRequest_.specializationSessionId) / "outputs";
        const ManifestExpansionContext expansionContext{
            resourceManifest_, resourceWorktreePath_, outputDirectoryPath, externalSourceDirectoryPath_};
        aclError result = BuildConstantSourcePatches(
            *selectedKernel, specializationRequest_, expansionContext, builtPlan.sourcePatches);
        if (result != ACLRTC_SUCCESS) {
            return result;
        }

        std::vector<SelectedManifestObject> selectedObjects;
        result = SelectManifestObjectsForCompilation(
            *selectedKernel, specializationRequest_.compilationVariant, selectedObjects);
        if (result != ACLRTC_SUCCESS) {
            return result;
        }

        for (const SelectedManifestObject& selectedObject : selectedObjects) {
            const Json& commands = selectedObject.manifestObject->at("commands");
            for (size_t commandIndex = 0U; commandIndex < commands.size(); ++commandIndex) {
                CompilationCommand command;
                result = BuildManifestCommand(
                    commands.at(commandIndex), selectedObject, commandIndex, specializationRequest_, expansionContext,
                    command);
                if (result != ACLRTC_SUCCESS) {
                    return result;
                }
                builtPlan.compilationCommands.emplace_back(std::move(command));
            }
        }

        std::set<fs::path> requiredOutputDirectories{outputDirectoryPath};
        CompilationCommand linkCommand;
        result = BuildLinkCommand(
            *selectedKernel, selectedObjects, expansionContext, linkCommand, requiredOutputDirectories,
            builtPlan.linkedKernelElfPath);
        if (result != ACLRTC_SUCCESS) {
            return result;
        }
        builtPlan.compilationCommands.emplace_back(std::move(linkCommand));
        builtPlan.requiredOutputDirectoryPaths.assign(
            requiredOutputDirectories.begin(), requiredOutputDirectories.end());
        compilationPlan = std::move(builtPlan);
        return ACLRTC_SUCCESS;
    } catch (const nlohmann::json::exception& exception) {
        ASCENDLOGE(
            "Manifest data consumed for kernel entry '%s' is incompatible with schema %s: %s; regenerate the JIT "
            "resource with a compatible pack tool",
            specializationRequest_.kernelName.c_str(), SUPPORTED_MANIFEST_SCHEMA_VERSION, exception.what());
        return ACLRTC_ERROR_FAILURE;
    }
}

} // namespace aclrtc
} // namespace ascendc
