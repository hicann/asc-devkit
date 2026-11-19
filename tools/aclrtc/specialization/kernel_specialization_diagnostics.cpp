/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "kernel_specialization_diagnostics.h"

#include "ascendc_tool_log.h"
#include "directory_cleanup_guard.h"
#include "file_utils.h"
#include "process_executor.h"
#include "process_compilation_context.h"

#include <boost/filesystem.hpp>
#include <cerrno>
#include <cstring>
#include <exception>
#include <fstream>
#include <sstream>
#include <sys/stat.h>

namespace ascendc {
namespace aclrtc {
namespace {
namespace fs = boost::filesystem;

constexpr char DIAGNOSTIC_MANIFEST_FILE_NAME[] = "aclrtc_manifest.json";
constexpr char DIAGNOSTIC_COMPILE_LOG_FILE_NAME[] = "aclrtc_compile.log";
constexpr char DIAGNOSTIC_REPLAY_SCRIPT_FILE_NAME[] = "aclrtc_replay.sh";
constexpr char DIAGNOSTIC_RESULT_FILE_NAME[] = "aclrtc_result.json";

// Build text inside the exception boundary so formatting and allocation failures
// remain diagnostic-only, just like file I/O failures.
template <typename TextBuilder>
void WriteDiagnosticTextBestEffort(
    const fs::path& resourceWorktreePath, const char* fileName, std::ios::openmode writeMode, const char* commandLabel,
    const TextBuilder& buildText) noexcept
{
    try {
        const fs::path filePath = resourceWorktreePath / fileName;
        FileUtils::WriteTextFile(filePath.string(), buildText(), writeMode);
    } catch (const std::exception& exception) {
        ASCENDLOGW(
            "Failed to write diagnostic text: resource_worktree=%s file=%s command=%s message=%s",
            resourceWorktreePath.c_str(), fileName, commandLabel, exception.what());
    } catch (...) {
        ASCENDLOGW(
            "Unknown exception while writing diagnostic text: resource_worktree=%s file=%s command=%s",
            resourceWorktreePath.c_str(), fileName, commandLabel);
    }
}

std::string QuoteArgumentForReplayScript(const std::string& argument)
{
    std::string quotedArgument("'");
    for (char character : argument) {
        if (character == '\'') {
            quotedArgument += "'\"'\"'";
        } else {
            quotedArgument += character;
        }
    }
    quotedArgument += '\'';
    return quotedArgument;
}

void RemoveIncompleteReplayScript(const fs::path& replayScriptPath) noexcept
{
    boost::system::error_code removalError;
    fs::remove(replayScriptPath, removalError);
    if (removalError) {
        ASCENDLOGW(
            "Failed to remove incomplete replay script: path=%s error=%d message=%s", replayScriptPath.c_str(),
            removalError.value(), removalError.message().c_str());
    }
}

bool CreateExecutableReplayScript(const fs::path& replayScriptPath, const fs::path& compilationWorkingDirectoryPath)
{
    const std::string replayScriptHeader =
        "#!/bin/sh\nset -eu\ncd " + QuoteArgumentForReplayScript(compilationWorkingDirectoryPath.string()) + "\n";
    if (!FileUtils::WriteTextFile(replayScriptPath.string(), replayScriptHeader, std::ios::trunc)) {
        RemoveIncompleteReplayScript(replayScriptPath);
        return false;
    }

    constexpr mode_t replayScriptPermission = S_IRUSR | S_IWUSR | S_IXUSR;
    if (chmod(replayScriptPath.c_str(), replayScriptPermission) == 0) {
        return true;
    }

    const int permissionError = errno;
    ASCENDLOGW(
        "Failed to make replay script executable: path=%s errno=%d message=%s", replayScriptPath.c_str(),
        permissionError, std::strerror(permissionError));
    RemoveIncompleteReplayScript(replayScriptPath);
    return false;
}

} // namespace

KernelSpecializationDiagnostics::KernelSpecializationDiagnostics(
    fs::path resourceWorktreePath, std::string specializationSessionId)
    : specializationSessionId_(std::move(specializationSessionId)),
      diagnosticsEnabled_(!resourceWorktreePath.empty()),
      resourceWorktreePath_(std::move(resourceWorktreePath))
{
    if (!diagnosticsEnabled_) {
        return;
    }
    try {
        const fs::path replayScriptPath = resourceWorktreePath_ / DIAGNOSTIC_REPLAY_SCRIPT_FILE_NAME;
        if (!CreateExecutableReplayScript(replayScriptPath, fs::current_path())) {
            diagnosticsEnabled_ = false;
        }
    } catch (const std::exception& exception) {
        diagnosticsEnabled_ = false;
        ASCENDLOGW(
            "Failed to initialize specialization diagnostics in resource worktree: session=%s path=%s message=%s",
            specializationSessionId_.c_str(), resourceWorktreePath_.c_str(), exception.what());
    } catch (...) {
        diagnosticsEnabled_ = false;
        ASCENDLOGW(
            "Failed to initialize specialization diagnostics in resource worktree: session=%s path=%s",
            specializationSessionId_.c_str(), resourceWorktreePath_.c_str());
    }
}

void KernelSpecializationDiagnostics::WriteManifestSnapshot(const nlohmann::json& resourceManifest) noexcept
{
    if (!diagnosticsEnabled_) {
        return;
    }
    WriteDiagnosticTextBestEffort(
        resourceWorktreePath_, DIAGNOSTIC_MANIFEST_FILE_NAME, std::ios::trunc, "", [&resourceManifest]() {
            constexpr int32_t diagnosticJsonIndentationWidth = 2;
            return resourceManifest.dump(diagnosticJsonIndentationWidth) + '\n';
        });
}

void KernelSpecializationDiagnostics::WriteCacheInputSnapshot(
    const fs::path& resourceWorktreePath, const nlohmann::json& cacheInput) noexcept
{
    if (cacheInput.is_null()) {
        return;
    }
    try {
        DirectoryCleanupGuard directory;
        if (!directory.CreateUniqueSubdirectory(resourceWorktreePath.parent_path().string(), "cache-inputs")) {
            ASCENDLOGW(
                "Failed to create cache input diagnostics directory: parent=%s",
                resourceWorktreePath.parent_path().c_str());
            return;
        }
        const std::string path = (fs::path(directory.GetDirectoryPath()) / "aclrtc_cache_inputs.json").string();
        if (!FileUtils::WriteTextFileAtomically(path, cacheInput.dump(2) + '\n')) {
            ASCENDLOGW(
                "Failed to save kernel compilation cache inputs: path=%s; keep specialization result", path.c_str());
            return;
        }
        directory.PreserveDirectory();
        ASCENDLOGI("Kernel compilation cache inputs saved: path=%s", path.c_str());
    } catch (const std::exception& exception) {
        ASCENDLOGW(
            "Failed to save kernel compilation cache inputs: resource_worktree=%s error=%s",
            resourceWorktreePath.c_str(), exception.what());
    }
}

void KernelSpecializationDiagnostics::AppendCommandToReplayScript(
    const CompilationCommand& compilationCommand,
    const std::map<std::string, std::optional<std::string>>& compilerEnvironment) noexcept
{
    if (!diagnosticsEnabled_) {
        return;
    }
    WriteDiagnosticTextBestEffort(
        resourceWorktreePath_, DIAGNOSTIC_REPLAY_SCRIPT_FILE_NAME, std::ios::app,
        compilationCommand.diagnosticLabel.c_str(), [&compilationCommand, &compilerEnvironment]() {
            std::string replayLine;
            if (!compilerEnvironment.empty()) {
                replayLine = "env";
                for (const auto& variable : compilerEnvironment) {
                    if (!variable.second.has_value()) {
                        replayLine += " -u " + QuoteArgumentForReplayScript(variable.first);
                    }
                }
                for (const auto& variable : compilerEnvironment) {
                    if (variable.second.has_value()) {
                        replayLine += " " + QuoteArgumentForReplayScript(variable.first + "=" + *variable.second);
                    }
                }
                replayLine += " ";
            }
            replayLine += QuoteArgumentForReplayScript(compilationCommand.executablePath.string());
            for (const std::string& argument : compilationCommand.arguments) {
                replayLine += " " + QuoteArgumentForReplayScript(argument);
            }
            return replayLine + '\n';
        });
}

std::string KernelSpecializationDiagnostics::GetCompilationLogFilePath() const
{
    return diagnosticsEnabled_ ? (resourceWorktreePath_ / DIAGNOSTIC_COMPILE_LOG_FILE_NAME).string() : std::string();
}

void KernelSpecializationDiagnostics::AppendCommandExecutionHeaderToLog(
    const CompilationCommand& compilationCommand) noexcept
{
    if (!diagnosticsEnabled_) {
        return;
    }
    WriteDiagnosticTextBestEffort(
        resourceWorktreePath_, DIAGNOSTIC_COMPILE_LOG_FILE_NAME, std::ios::app,
        compilationCommand.diagnosticLabel.c_str(),
        [&compilationCommand]() { return "[" + compilationCommand.diagnosticLabel + "] output:\n"; });
}

void KernelSpecializationDiagnostics::AppendCommandExecutionResultToLog(
    const CompilationCommand& compilationCommand, const ProcessExecutorResult& executorResult) noexcept
{
    if (!diagnosticsEnabled_) {
        return;
    }
    WriteDiagnosticTextBestEffort(
        resourceWorktreePath_, DIAGNOSTIC_COMPILE_LOG_FILE_NAME, std::ios::app,
        compilationCommand.diagnosticLabel.c_str(), [&compilationCommand, &executorResult]() {
            std::ostringstream resultText;
            resultText << '\n'
                       << '[' << compilationCommand.diagnosticLabel << "] outcome=" << executorResult.GetOutcomeName()
                       << " code=" << executorResult.terminationCode
                       << " elapsed_ms=" << executorResult.elapsedTime.count() << '\n';
            return resultText.str();
        });
}

void KernelSpecializationDiagnostics::LogCommandFailureRecoveryHint() const noexcept
{
    if (ProcessCompilationContext::Instance().IsCacheEnabled()) {
        ASCENDLOGW("Intermediate compilation artifacts are not retained while caching is enabled; "
                   "set ASCEND_RTC_SPECIALIZE_CACHE_DISABLE=1 and ASCEND_OP_COMPILE_SAVE_KERNEL_META=1 "
                   "to save compilation diagnostics");
        return;
    }
    if (!diagnosticsEnabled_) {
        ASCENDLOGI("Set ASCEND_OP_COMPILE_SAVE_KERNEL_META=1 and retry to retain the resource worktree and save "
                   "specialization diagnostics");
        return;
    }
    ASCENDLOGI(
        "Compilation diagnostics saved: compile_log=%s/%s replay_script=%s/%s; inspect the log or run the replay "
        "script to reproduce the failed command",
        resourceWorktreePath_.c_str(), DIAGNOSTIC_COMPILE_LOG_FILE_NAME, resourceWorktreePath_.c_str(),
        DIAGNOSTIC_REPLAY_SCRIPT_FILE_NAME);
}

void KernelSpecializationDiagnostics::WriteSpecializationResult(
    const fs::path& outputElfPath, aclError specializationStatus, const char* stage) noexcept
{
    if (!diagnosticsEnabled_) {
        return;
    }
    WriteDiagnosticTextBestEffort(
        resourceWorktreePath_, DIAGNOSTIC_RESULT_FILE_NAME, std::ios::trunc, "",
        [this, &outputElfPath, specializationStatus, stage]() {
            constexpr int32_t diagnosticJsonIndentationWidth = 2;
            nlohmann::json resultJson{
                {"specialization_session_id", specializationSessionId_},
                {"output_path", outputElfPath.string()},
                {"output_published", specializationStatus == ACLRTC_SUCCESS},
                {"status", specializationStatus},
                {"stage", stage},
            };
            return resultJson.dump(diagnosticJsonIndentationWidth) + '\n';
        });
}

} // namespace aclrtc
} // namespace ascendc
