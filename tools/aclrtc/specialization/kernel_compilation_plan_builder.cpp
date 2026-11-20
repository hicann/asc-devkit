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
#include "process_compilation_context.h"
#include "kernel_compilation_cache_input.h"

#include <algorithm>
#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>
#include <iomanip>
#include <set>
#include <sstream>
#include <string_view>
#include <unistd.h>

namespace ascendc {
namespace aclrtc {
namespace {
namespace fs = boost::filesystem;
constexpr std::string_view ENVIRONMENT_REFERENCE_PREFIX = "${env:";
constexpr std::string_view RESOURCE_REFERENCE = "${resource}";
constexpr std::string_view OUTPUT_REFERENCE = "${output}";
constexpr std::string_view SOURCE_REFERENCE = "${source_file_path}";
constexpr std::string_view PLACEHOLDER_PREFIX = "${";

bool CollectEnvironmentVariables(
    const std::string& text, const char* location, std::map<std::string, std::string>& environmentValues)
{
    size_t position = 0U;
    while ((position = text.find(ENVIRONMENT_REFERENCE_PREFIX, position)) != std::string::npos) {
        const size_t nameStart = position + ENVIRONMENT_REFERENCE_PREFIX.size();
        const size_t end = text.find('}', nameStart);
        if (end == std::string::npos || end == nameStart) {
            ASCENDLOGE(
                "Invalid environment variable placeholder at %s byte %zu; use the form ${env:NAME}", location,
                position);
            return false;
        }
        const std::string name = text.substr(nameStart, end - nameStart);
        if (environmentValues.find(name) == environmentValues.end()) {
            std::string value;
            if (!ProcessCompilationContext::Instance().GetEnvironmentVariable(name, location, value)) {
                return false;
            }
            environmentValues.emplace(name, std::move(value));
        }
        position = end + 1U;
    }
    return true;
}

std::string ExpandEnvironmentVariables(
    const std::string& text, const std::map<std::string, std::string>& environmentValues)
{
    std::string result;
    size_t position = 0U;
    size_t reference = 0U;
    while ((reference = text.find(ENVIRONMENT_REFERENCE_PREFIX, position)) != std::string::npos) {
        result.append(text, position, reference - position);
        const size_t nameStart = reference + ENVIRONMENT_REFERENCE_PREFIX.size();
        const size_t end = text.find('}', nameStart);
        result.append(environmentValues.at(text.substr(nameStart, end - nameStart)));
        position = end + 1U;
    }
    result.append(text, position, std::string::npos);
    return result;
}

bool CollectCompilationEnvironment(PreparedKernelCompilationPlan& plan)
{
    bool usesSourceDirectory = false;
    for (const PreparedCompilationCommand& command : plan.compilationCommands) {
        if (!CollectEnvironmentVariables(command.executable, command.diagnosticLabel.c_str(), plan.environmentValues)) {
            return false;
        }
        usesSourceDirectory |= command.executable.find(SOURCE_REFERENCE) != std::string::npos;
        for (const std::string& argument : command.arguments) {
            if (!CollectEnvironmentVariables(argument, command.diagnosticLabel.c_str(), plan.environmentValues)) {
                return false;
            }
            usesSourceDirectory |= argument.find(SOURCE_REFERENCE) != std::string::npos;
        }
    }
    for (const ManifestConstant& constant : plan.constants) {
        const std::string& targetFile = constant.GetTargetFile();
        if (!CollectEnvironmentVariables(targetFile, constant.GetName().c_str(), plan.environmentValues)) {
            return false;
        }
        usesSourceDirectory |= targetFile.find(SOURCE_REFERENCE) != std::string::npos;
    }
    if (!usesSourceDirectory) {
        return true;
    }
    return CollectEnvironmentVariables(plan.sourceDirectory, "source_file_path", plan.environmentValues);
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

bool AppendCompilerTargetOptions(const std::vector<std::string>& arguments, std::vector<std::string>& targetArguments)
{
    bool foundTarget = false;
    for (const std::string& argument : arguments) {
        if (argument.compare(0, sizeof("--cce-aicore-arch=") - 1U, "--cce-aicore-arch=") == 0 ||
            argument.compare(0, sizeof("--npu-arch=") - 1U, "--npu-arch=") == 0) {
            if (argument.back() == '=') {
                return false;
            }
            targetArguments.emplace_back(argument);
            foundTarget = true;
        }
    }
    return foundTarget;
}

bool ReplaceCompilationSourceWithPreprocessed(CompilationCommand& command, const PreprocessedCompilationSource& source)
{
    const auto sourceArgument =
        std::find(command.arguments.begin(), command.arguments.end(), source.sourcePath.string());
    if (sourceArgument == command.arguments.end()) {
        ASCENDLOGE(
            "Preprocessed source does not match formal compilation command: label=%s source=%s",
            command.diagnosticLabel.c_str(), source.sourcePath.c_str());
        return false;
    }
    *sourceArgument = source.preprocessedPath.string();
    bool languageSpecified = false;
    for (size_t index = 0U; index < command.arguments.size(); ++index) {
        std::string& argument = command.arguments[index];
        if (argument == "-x" && index + 1U < command.arguments.size()) {
            command.arguments[++index] = "cce-cpp-output";
            languageSpecified = true;
        } else if (argument.compare(0U, sizeof("-x") - 1U, "-x") == 0U) {
            argument = "-xcce-cpp-output";
            languageSpecified = true;
        }
    }
    if (!languageSpecified) {
        command.arguments.insert(sourceArgument, {"-x", "cce-cpp-output"});
    }
    // Preprocessing consumed include and macro options; suppress unused-option warnings while retaining -Werror.
    command.arguments.emplace_back("-Qunused-arguments");
    return true;
}

} // namespace

std::string KernelCompilationPlanBuilder::ExpandPathAndEnvironmentReferences(
    const std::string& text, const PreparedKernelCompilationPlan& plan) const
{
    std::string result;
    size_t position = 0U;
    size_t reference = 0U;
    // Expand only the original template; keep placeholders in replacement values literal.
    while ((reference = text.find(PLACEHOLDER_PREFIX, position)) != std::string::npos) {
        result.append(text, position, reference - position);
        const std::string_view tail(text.data() + reference, text.size() - reference);
        if (tail.compare(0, RESOURCE_REFERENCE.size(), RESOURCE_REFERENCE) == 0) {
            result.append(resourceWorktreePath_.string());
            position = reference + RESOURCE_REFERENCE.size();
        } else if (tail.compare(0, OUTPUT_REFERENCE.size(), OUTPUT_REFERENCE) == 0) {
            result.append(outputDirectoryPath_.string());
            position = reference + OUTPUT_REFERENCE.size();
        } else if (tail.compare(0, SOURCE_REFERENCE.size(), SOURCE_REFERENCE) == 0) {
            result.append(ExpandEnvironmentVariables(plan.sourceDirectory, plan.environmentValues));
            position = reference + SOURCE_REFERENCE.size();
        } else if (tail.compare(0, ENVIRONMENT_REFERENCE_PREFIX.size(), ENVIRONMENT_REFERENCE_PREFIX) == 0) {
            const size_t nameStart = reference + ENVIRONMENT_REFERENCE_PREFIX.size();
            const size_t end = text.find('}', nameStart);
            result.append(plan.environmentValues.at(text.substr(nameStart, end - nameStart)));
            position = end + 1U;
        } else {
            result.append(PLACEHOLDER_PREFIX);
            position = reference + PLACEHOLDER_PREFIX.size();
        }
    }
    result.append(text, position, std::string::npos);
    return result;
}

void KernelCompilationPlanBuilder::BuildConstantSourcePatches(
    const PreparedKernelCompilationPlan& preparedPlan, const KernelConstantSnapshot& constants,
    KernelCompilationPlan& plan) const
{
    std::vector<KernelSourcePatch> patches;
    for (size_t index = 0U; index < preparedPlan.constants.size(); ++index) {
        const ManifestConstant& constant = preparedPlan.constants[index];
        const KernelConstantBindingValue& binding = constants.bindings[index];
        const auto& bytes = constants.argumentBytes.at(binding.parameterIndex);
        patches.push_back(
            {ExpandPathAndEnvironmentReferences(constant.GetTargetFile(), preparedPlan), constant.GetTemplateText(),
             FormatBytesAsCppInitializerList(bytes.data(), binding.byteCount)});
    }
    plan.sourcePatches = std::move(patches);
}

aclError KernelCompilationPlanBuilder::BuildSpecializationMetadata(
    const PreparedKernelCompilationPlan& preparedPlan, const CompilationCommand& basicCommand,
    fs::path& objectPath) const
{
    const fs::path ascendHome = preparedPlan.environmentValues.at("ASCEND_HOME_PATH");
    if (!IsAbsoluteExecutableRegularFile(ascendHome / "bin/bisheng")) {
        ASCENDLOGE("Specialization metadata requires an executable bisheng under ASCEND_HOME_PATH");
        return ACLRTC_ERROR_FAILURE;
    }
    std::vector<std::string> targetOptions;
    if (!AppendCompilerTargetOptions(basicCommand.arguments, targetOptions)) {
        ASCENDLOGE(
            "Basic compile command %s requires an explicit target architecture for specialization metadata",
            basicCommand.diagnosticLabel.c_str());
        return ACLRTC_ERROR_FAILURE;
    }
    return ProcessCompilationContext::Instance().GetSpecializationFlagObject(ascendHome, targetOptions, objectPath);
}

void KernelCompilationPlanBuilder::AppendLinkCommand(
    const CompilationManifest& manifest, PreparedKernelCompilationPlan& plan) const
{
    PreparedCompilationCommand command{CompilationCommandKind::Link, {}, "link", "${env:ASCEND_HOME_PATH}/bin/ld.lld",
                                       manifest.linkOptions,         {}, {}};
    command.arguments.insert(command.arguments.end(), manifest.linkInputs.begin(), manifest.linkInputs.end());
    plan.compilationCommands.emplace_back(std::move(command));
}

void KernelCompilationPlanBuilder::AppendKernelRenameCommand(PreparedKernelCompilationPlan& plan) const
{
    const auto& outputName = specializationRequest_.outputKernelName;
    if (outputName.empty() || outputName == specializationRequest_.kernelName) {
        return;
    }
    // The executor inspects symbols and metadata sections after linking.
    plan.compilationCommands.push_back(
        {CompilationCommandKind::ObjectCopy,
         {},
         "rename_output_kernel",
         "${env:ASCEND_HOME_PATH}/bin/llvm-objcopy",
         {"${output}/linked_kernel.elf"},
         {},
         KernelSymbolRename{specializationRequest_.kernelName, outputName}});
}

aclError KernelCompilationPlanBuilder::BindCompilationCommand(
    const PreparedCompilationCommand& preparedCommand, const PreparedKernelCompilationPlan& preparedPlan,
    CompilationCommand& command) const
{
    command.commandKind = preparedCommand.commandKind;
    command.parallelStage = preparedCommand.parallelStage;
    command.diagnosticLabel = preparedCommand.diagnosticLabel;
    command.kernelSymbolRename = preparedCommand.kernelSymbolRename;
    command.executablePath = ExpandPathAndEnvironmentReferences(preparedCommand.executable, preparedPlan);
    if (!IsAbsoluteExecutableRegularFile(command.executablePath)) {
        ASCENDLOGE(
            "Compilation command %s requires an absolute path to an existing executable regular file: path=%s; "
            "check the referenced environment variable and tool installation",
            command.diagnosticLabel.c_str(), command.executablePath.c_str());
        return ACLRTC_ERROR_FAILURE;
    }
    for (const std::string& argument : preparedCommand.arguments) {
        command.arguments.emplace_back(ExpandPathAndEnvironmentReferences(argument, preparedPlan));
    }
    if (command.commandKind == CompilationCommandKind::Compile) {
        size_t outputOptionPosition = 0U;
        if (!FindSeparatedCompilerOutputOptionPosition(command.arguments, outputOptionPosition)) {
            ASCENDLOGE(
                "Manifest compile command %s must contain a valid separated '-o' output option and a non-empty "
                "output path; regenerate the JIT resource with '-o' followed by its output path",
                command.diagnosticLabel.c_str());
            return ACLRTC_ERROR_FAILURE;
        }
        // Caller-supplied compiler options are literal arguments, without manifest placeholder expansion.
        command.arguments.insert(
            command.arguments.begin() + static_cast<std::ptrdiff_t>(outputOptionPosition),
            preparedCommand.compilerOptions.begin(), preparedCommand.compilerOptions.end());
    }
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
    const NormalizedKernelSpecializationRequest& specializationRequest, fs::path resourceWorktreePath,
    fs::path externalSourceDirectoryPath)
    : specializationRequest_(specializationRequest),
      resourceWorktreePath_(std::move(resourceWorktreePath)),
      externalSourceDirectoryPath_(std::move(externalSourceDirectoryPath)),
      outputDirectoryPath_(
          resourceWorktreePath_ / (".aclrtc_" + specializationRequest_.specializationSessionId) / "outputs")
{}

aclError KernelCompilationPlanBuilder::PrepareCompilationPlan(
    const CompilationManifest& manifest, PreparedKernelCompilationPlan& compilationPlan) const
{
    PreparedKernelCompilationPlan plan{};
    plan.sourceDirectory = externalSourceDirectoryPath_.string();
    plan.constants = manifest.constants;
    plan.linkInputs = manifest.linkInputs;
    std::optional<size_t> basicCompileCommandIndex;
    for (const ManifestCommand& spec : manifest.commands) {
        PreparedCompilationCommand command{
            spec.commandKind, spec.parallelStage, spec.diagnosticLabel, spec.executable, spec.arguments, {}, {}};
        if (spec.commandKind == CompilationCommandKind::Compile) {
            const bool basic = spec.objectKind == ManifestObjectKind::Basic;
            command.compilerOptions = basic ? specializationRequest_.compilerOptions.basicOptions :
                                              specializationRequest_.compilerOptions.superKernelOptions;
            if (basic && !basicCompileCommandIndex.has_value()) {
                basicCompileCommandIndex = plan.compilationCommands.size();
            }
        }
        plan.compilationCommands.emplace_back(std::move(command));
    }
    if (!basicCompileCommandIndex.has_value()) {
        ASCENDLOGE("Specialization metadata requires a basic compile command in the selected manifest");
        return ACLRTC_ERROR_FAILURE;
    }
    plan.basicCompileCommandIndex = *basicCompileCommandIndex;
    AppendLinkCommand(manifest, plan);
    AppendKernelRenameCommand(plan);
    if (!CollectCompilationEnvironment(plan)) {
        return ACLRTC_ERROR_FAILURE;
    }
    plan.compilerEnvironment = ProcessCompilationContext::Instance().GetCompilerEnvironment();
    compilationPlan = std::move(plan);
    return ACLRTC_SUCCESS;
}

aclError KernelCompilationPlanBuilder::BuildPreprocessingPlan(
    const PreparedKernelCompilationPlan& preparedPlan, const KernelConstantSnapshot& constants,
    KernelCompilationPlan& compilationPlan) const
{
    aclError status = CheckResourceWorktreeDirectory();
    if (status != ACLRTC_SUCCESS) {
        return status;
    }
    KernelCompilationPlan plan;
    plan.compilerEnvironment = preparedPlan.compilerEnvironment;
    for (const auto& variable : preparedPlan.environmentValues) {
        plan.compilerEnvironment[variable.first] = variable.second;
    }
    for (const PreparedCompilationCommand& preparedCommand : preparedPlan.compilationCommands) {
        if (preparedCommand.commandKind != CompilationCommandKind::Compile) {
            continue;
        }
        CompilationCommand command;
        status = BindCompilationCommand(preparedCommand, preparedPlan, command);
        if (status != ACLRTC_SUCCESS) {
            return status;
        }
        command.arguments.emplace_back("-E");
        size_t outputOptionPosition = 0U;
        (void)FindSeparatedCompilerOutputOptionPosition(command.arguments, outputOptionPosition);
        command.arguments[outputOptionPosition + 1U] =
            (outputDirectoryPath_ / ("preprocessed_" + std::to_string(plan.compilationCommands.size()) + ".i"))
                .string();
        plan.compilationCommands.emplace_back(std::move(command));
    }
    plan.requiredOutputDirectoryPaths = {outputDirectoryPath_};
    BuildConstantSourcePatches(preparedPlan, constants, plan);
    compilationPlan = std::move(plan);
    return ACLRTC_SUCCESS;
}

aclError KernelCompilationPlanBuilder::BindCompilationCommands(
    const PreparedKernelCompilationPlan& preparedPlan,
    const std::vector<PreprocessedCompilationSource>* preprocessedSources, KernelCompilationPlan& plan) const
{
    size_t preprocessedSourceIndex = 0U;
    for (const PreparedCompilationCommand& preparedCommand : preparedPlan.compilationCommands) {
        CompilationCommand command;
        aclError result = BindCompilationCommand(preparedCommand, preparedPlan, command);
        if (result != ACLRTC_SUCCESS) {
            return result;
        }
        if (command.commandKind == CompilationCommandKind::Compile && preprocessedSources != nullptr) {
            if (preprocessedSourceIndex >= preprocessedSources->size()) {
                return ACLRTC_ERROR_COMPILATION;
            }
            if (!ReplaceCompilationSourceWithPreprocessed(command, (*preprocessedSources)[preprocessedSourceIndex])) {
                return ACLRTC_ERROR_COMPILATION;
            }
            ++preprocessedSourceIndex;
        }
        if (command.commandKind == CompilationCommandKind::Link) {
            fs::path metadataObjectPath;
            result = BuildSpecializationMetadata(
                preparedPlan, plan.compilationCommands[preparedPlan.basicCompileCommandIndex], metadataObjectPath);
            if (result != ACLRTC_SUCCESS) {
                return result;
            }
            command.arguments.insert(
                command.arguments.end(), {metadataObjectPath.string(), "-o", plan.linkedKernelElfPath.string()});
        }
        plan.compilationCommands.emplace_back(std::move(command));
    }
    if (preprocessedSources != nullptr && preprocessedSourceIndex != preprocessedSources->size()) {
        ASCENDLOGE("Preprocessed source count does not match formal compilation command count");
        return ACLRTC_ERROR_COMPILATION;
    }
    return ACLRTC_SUCCESS;
}

aclError KernelCompilationPlanBuilder::BuildCompilationPlan(
    const PreparedKernelCompilationPlan& preparedPlan, const KernelConstantSnapshot& constants,
    KernelCompilationPlan& compilationPlan, const std::vector<PreprocessedCompilationSource>* preprocessedSources) const
{
    aclError result = CheckResourceWorktreeDirectory();
    if (result != ACLRTC_SUCCESS) {
        return result;
    }
    KernelCompilationPlan plan;
    plan.compilerEnvironment = preparedPlan.compilerEnvironment;
    for (const auto& variable : preparedPlan.environmentValues) {
        plan.compilerEnvironment[variable.first] = variable.second;
    }
    plan.linkedKernelElfPath = outputDirectoryPath_ / "linked_kernel.elf";
    result = BindCompilationCommands(preparedPlan, preprocessedSources, plan);
    if (result != ACLRTC_SUCCESS) {
        return result;
    }
    std::set<fs::path> requiredOutputDirectories{outputDirectoryPath_};
    for (const std::string& input : preparedPlan.linkInputs) {
        requiredOutputDirectories.emplace(
            fs::path(ExpandPathAndEnvironmentReferences(input, preparedPlan)).parent_path());
    }
    plan.requiredOutputDirectoryPaths.assign(requiredOutputDirectories.begin(), requiredOutputDirectories.end());
    if (preprocessedSources == nullptr) {
        BuildConstantSourcePatches(preparedPlan, constants, plan);
    }
    compilationPlan = std::move(plan);
    return ACLRTC_SUCCESS;
}

} // namespace aclrtc
} // namespace ascendc
