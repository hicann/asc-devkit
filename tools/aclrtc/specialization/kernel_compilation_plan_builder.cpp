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
constexpr char ENVIRONMENT_REFERENCE_PREFIX[] = "${env:";

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

bool FindSeparatedCompilerOutputOptionPosition(const std::vector<std::string>& arguments, size_t& outputOptionPosition)
{
    for (size_t argumentIndex = 0; argumentIndex < arguments.size(); ++argumentIndex) {
        const std::string& argument = arguments[argumentIndex];
        if (argument != "-o") {
            continue;
        }
        if (argumentIndex + 1U < arguments.size() && !arguments[argumentIndex + 1U].empty()) {
            outputOptionPosition = argumentIndex;
            return true;
        }
    }
    return false;
}

std::string FormatBytesAsCppInitializerList(const void* constantAddress, uint64_t constantByteCount)
{
    constexpr int32_t hexadecimalByteWidth = 2;
    const auto* constantBytes = static_cast<const uint8_t*>(constantAddress);
    std::ostringstream encodedBytes;
    encodedBytes << '{';
    for (uint64_t byteIndex = 0; byteIndex < constantByteCount; ++byteIndex) {
        if (byteIndex != 0U) {
            encodedBytes << ", ";
        }
        encodedBytes << "0x" << std::hex << std::nouppercase << std::setw(hexadecimalByteWidth) << std::setfill('0')
                     << static_cast<unsigned int>(constantBytes[byteIndex]);
    }
    encodedBytes << '}';
    return encodedBytes.str();
}

} // namespace

bool KernelCompilationPlanBuilder::ExpandPathAndEnvironmentReferences(
    const std::string& text, const std::string& location, std::string& resolvedText) const
{
    std::string pathResolvedText = text;
    ReplaceAllLiteralOccurrences(pathResolvedText, "${resource}", resourceWorktreePath_.string());
    ReplaceAllLiteralOccurrences(pathResolvedText, "${output}", outputDirectoryPath_.string());
    ReplaceAllLiteralOccurrences(pathResolvedText, "${source_file_path}", externalSourceDirectoryPath_.string());
    return ExpandEnvironmentVariablePlaceholders(pathResolvedText, location, resolvedText);
}

aclError KernelCompilationPlanBuilder::BuildConstantSourcePatches(KernelCompilationPlan& plan) const
{
    auto constants = borrowedManifest_.constants;
    std::vector<KernelSourcePatch> patches;
    for (ManifestConstant& constant : constants) {
        if (!constant.BindArgument(
                specializationRequest_.kernelArgumentCount, specializationRequest_.borrowedKernelArgumentDataPointers,
                specializationRequest_.borrowedKernelArgumentByteCounts)) {
            return ACLRTC_ERROR_INVALID_INPUT;
        }
        std::string targetFile;
        if (!ExpandPathAndEnvironmentReferences(constant.GetTargetFile(), constant.GetName(), targetFile)) {
            return ACLRTC_ERROR_FAILURE;
        }
        patches.push_back({targetFile, constant.GetTemplateText(), {}});
    }
    // Avoid reading caller-owned buffers if any constant binding or target expansion fails.
    for (size_t index = 0; index < constants.size(); ++index) {
        patches[index].replacementText =
            FormatBytesAsCppInitializerList(constants[index].GetBoundData(), constants[index].GetBoundByteSize());
    }
    plan.sourcePatches = std::move(patches);
    return ACLRTC_SUCCESS;
}

aclError KernelCompilationPlanBuilder::BindManifestCommand(
    const ManifestCommand& spec, CompilationCommand& compilationCommand) const
{
    compilationCommand.commandKind = spec.commandKind;
    compilationCommand.parallelStage = spec.parallelStage;
    compilationCommand.diagnosticLabel = spec.diagnosticLabel;
    std::string executable;
    if (!ExpandPathAndEnvironmentReferences(spec.executable, spec.diagnosticLabel, executable)) {
        return ACLRTC_ERROR_FAILURE;
    }
    compilationCommand.executablePath = std::move(executable);
    if (!IsAbsoluteExecutableRegularFile(compilationCommand.executablePath)) {
        ASCENDLOGE(
            "Manifest command %s requires an absolute path to an existing executable regular file: path=%s; "
            "check the referenced environment variable and tool installation",
            compilationCommand.diagnosticLabel.c_str(), compilationCommand.executablePath.c_str());
        return ACLRTC_ERROR_FAILURE;
    }

    for (const std::string& argument : spec.arguments) {
        std::string resolvedArgument;
        if (!ExpandPathAndEnvironmentReferences(argument, spec.diagnosticLabel, resolvedArgument)) {
            return ACLRTC_ERROR_FAILURE;
        }
        compilationCommand.arguments.emplace_back(std::move(resolvedArgument));
    }

    if (compilationCommand.commandKind == CompilationCommandKind::Compile) {
        size_t outputOptionPosition = 0U;
        if (!FindSeparatedCompilerOutputOptionPosition(compilationCommand.arguments, outputOptionPosition)) {
            ASCENDLOGE(
                "Manifest compile command %s must contain a valid separated '-o' output option and a non-empty "
                "output path; regenerate the JIT resource with '-o' followed by its output path",
                compilationCommand.diagnosticLabel.c_str());
            return ACLRTC_ERROR_FAILURE;
        }
        const auto& options = spec.objectKind == ManifestObjectKind::Basic ?
                                  specializationRequest_.compilerOptions.basicOptions :
                                  specializationRequest_.compilerOptions.superKernelOptions;
        compilationCommand.arguments.insert(
            compilationCommand.arguments.begin() + static_cast<std::ptrdiff_t>(outputOptionPosition), options.begin(),
            options.end());
    }
    return ACLRTC_SUCCESS;
}

aclError KernelCompilationPlanBuilder::BindLinkCommand(
    const CompilationManifest& spec, KernelCompilationPlan& plan) const
{
    CompilationCommand linkCommand;
    std::set<fs::path> requiredOutputDirectories{outputDirectoryPath_};
    linkCommand.commandKind = CompilationCommandKind::Link;
    linkCommand.diagnosticLabel = "link";
    std::string linkerPath;
    if (!ExpandPathAndEnvironmentReferences(
            "${env:ASCEND_HOME_PATH}/bin/ld.lld", "internal linker executable", linkerPath)) {
        return ACLRTC_ERROR_FAILURE;
    }
    linkCommand.executablePath = linkerPath;

    for (const std::string& option : spec.linkOptions) {
        std::string resolvedOption;
        if (!ExpandPathAndEnvironmentReferences(option, "link_options", resolvedOption)) {
            return ACLRTC_ERROR_FAILURE;
        }
        linkCommand.arguments.emplace_back(std::move(resolvedOption));
    }
    for (const std::string& input : spec.linkInputs) {
        std::string resolvedInput;
        if (!ExpandPathAndEnvironmentReferences(input, "link_inputs", resolvedInput)) {
            return ACLRTC_ERROR_FAILURE;
        }
        requiredOutputDirectories.emplace(fs::path(resolvedInput).parent_path());
        linkCommand.arguments.emplace_back(std::move(resolvedInput));
    }
    plan.linkedKernelElfPath = outputDirectoryPath_ / "linked_kernel.elf";
    linkCommand.arguments.emplace_back("-o");
    linkCommand.arguments.emplace_back(plan.linkedKernelElfPath.string());
    if (!IsAbsoluteExecutableRegularFile(linkCommand.executablePath)) {
        ASCENDLOGE(
            "Linker requires an absolute path to an existing executable regular file: path=%s; verify "
            "ASCEND_HOME_PATH points to a complete CANN installation",
            linkCommand.executablePath.c_str());
        return ACLRTC_ERROR_FAILURE;
    }
    plan.compilationCommands.emplace_back(std::move(linkCommand));
    plan.requiredOutputDirectoryPaths.assign(requiredOutputDirectories.begin(), requiredOutputDirectories.end());
    return ACLRTC_SUCCESS;
}

aclError KernelCompilationPlanBuilder::CheckResourceWorktreeDirectory() const
{
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
    return ACLRTC_SUCCESS;
}

KernelCompilationPlanBuilder::KernelCompilationPlanBuilder(
    const NormalizedKernelSpecializationRequest& specializationRequest, const CompilationManifest& manifest,
    fs::path resourceWorktreePath, fs::path externalSourceDirectoryPath)
    : specializationRequest_(specializationRequest),
      borrowedManifest_(manifest),
      resourceWorktreePath_(std::move(resourceWorktreePath)),
      externalSourceDirectoryPath_(std::move(externalSourceDirectoryPath)),
      outputDirectoryPath_(
          resourceWorktreePath_ / (".aclrtc_" + specializationRequest_.specializationSessionId) / "outputs")
{}

aclError KernelCompilationPlanBuilder::BuildCompilationPlan(KernelCompilationPlan& compilationPlan) const
{
    aclError result = CheckResourceWorktreeDirectory();
    if (result != ACLRTC_SUCCESS) {
        return result;
    }
    KernelCompilationPlan plan;
    for (const ManifestCommand& spec : borrowedManifest_.commands) {
        CompilationCommand command;
        result = BindManifestCommand(spec, command);
        if (result != ACLRTC_SUCCESS) {
            return result;
        }
        plan.compilationCommands.emplace_back(std::move(command));
    }
    result = BindLinkCommand(borrowedManifest_, plan);
    if (result != ACLRTC_SUCCESS) {
        return result;
    }
    result = BuildConstantSourcePatches(plan);
    if (result != ACLRTC_SUCCESS) {
        return result;
    }
    compilationPlan = std::move(plan);
    return ACLRTC_SUCCESS;
}

} // namespace aclrtc
} // namespace ascendc
