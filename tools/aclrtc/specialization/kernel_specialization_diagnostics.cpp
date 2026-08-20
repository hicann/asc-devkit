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

#include <boost/filesystem.hpp>
#include <cerrno>
#include <cstring>
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

const char* GetProcessTerminationName(CompilationProcessTermination termination)
{
    switch (termination) {
        case CompilationProcessTermination::InfrastructureFailure:
            return "infrastructure_failure";
        case CompilationProcessTermination::Exited:
            return "exited";
        case CompilationProcessTermination::Signaled:
            return "signaled";
        case CompilationProcessTermination::TimedOut:
            return "timed_out";
        default:
            return "unknown";
    }
}

bool WriteDiagnosticTextFile(const fs::path& filePath, const std::string& text, std::ios::openmode writeMode)
{
    std::ofstream outputStream(filePath.string(), std::ios::out | writeMode);
    if (!outputStream) {
        const int openError = errno;
        ASCENDLOGW(
            "Failed to open diagnostic file for writing: path=%s errno=%d message=%s", filePath.c_str(), openError,
            std::strerror(openError));
        return false;
    }

    outputStream << text;
    outputStream.close();
    if (outputStream) {
        return true;
    }
    const int writeError = errno;
    ASCENDLOGW(
        "Failed to write diagnostic file: path=%s errno=%d message=%s", filePath.c_str(), writeError,
        std::strerror(writeError));
    return false;
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
    if (!WriteDiagnosticTextFile(replayScriptPath, replayScriptHeader, std::ios::trunc)) {
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
    constexpr int diagnosticJsonIndentationWidth = 2;
    try {
        const fs::path manifestPath = resourceWorktreePath_ / DIAGNOSTIC_MANIFEST_FILE_NAME;
        WriteDiagnosticTextFile(
            manifestPath, resourceManifest.dump(diagnosticJsonIndentationWidth) + '\n', std::ios::trunc);
    } catch (...) {
        ASCENDLOGW(
            "Failed to record specialization manifest: resource_worktree=%s file=%s", resourceWorktreePath_.c_str(),
            DIAGNOSTIC_MANIFEST_FILE_NAME);
    }
}

void KernelSpecializationDiagnostics::AppendCommandToReplayScript(const CompilationCommand& compilationCommand) noexcept
{
    if (!diagnosticsEnabled_) {
        return;
    }
    try {
        std::string replayLine = QuoteArgumentForReplayScript(compilationCommand.executablePath.string());
        for (const std::string& argument : compilationCommand.commandArguments) {
            replayLine += " " + QuoteArgumentForReplayScript(argument);
        }
        replayLine += '\n';
        AppendTextToDiagnosticFile(resourceWorktreePath_ / DIAGNOSTIC_REPLAY_SCRIPT_FILE_NAME, replayLine);
    } catch (...) {
        ASCENDLOGW(
            "Failed to record command %s: resource_worktree=%s file=%s", compilationCommand.diagnosticLabel.c_str(),
            resourceWorktreePath_.c_str(), DIAGNOSTIC_REPLAY_SCRIPT_FILE_NAME);
    }
}

void KernelSpecializationDiagnostics::AppendCommandResultToCompilationLog(
    const CompilationCommand& compilationCommand, const CompilationProcessResult& processResult) noexcept
{
    if (!diagnosticsEnabled_) {
        return;
    }
    try {
        std::ostringstream resultText;
        resultText << '[' << compilationCommand.diagnosticLabel
                   << "] termination=" << GetProcessTerminationName(processResult.termination)
                   << " code=" << processResult.terminationCode << " elapsed_ms=" << processResult.elapsedTime.count()
                   << " output_truncated=" << (processResult.capturedOutputTruncated ? "true" : "false") << '\n'
                   << processResult.capturedOutput;
        if (!processResult.capturedOutput.empty() && processResult.capturedOutput.back() != '\n') {
            resultText << '\n';
        }
        AppendTextToDiagnosticFile(resourceWorktreePath_ / DIAGNOSTIC_COMPILE_LOG_FILE_NAME, resultText.str());
    } catch (...) {
        ASCENDLOGW(
            "Failed to record result for command %s: resource_worktree=%s file=%s",
            compilationCommand.diagnosticLabel.c_str(), resourceWorktreePath_.c_str(),
            DIAGNOSTIC_COMPILE_LOG_FILE_NAME);
    }
}

void KernelSpecializationDiagnostics::LogCommandFailureRecoveryHint() const noexcept
{
    if (!diagnosticsEnabled_) {
        ASCENDLOGI("Set ASCEND_OP_COMPILE_SAVE_KERNEL_META=1 and retry to retain the resource worktree and save "
                   "specialization diagnostics");
        return;
    }
    try {
        const fs::path compilationLogPath = resourceWorktreePath_ / DIAGNOSTIC_COMPILE_LOG_FILE_NAME;
        const fs::path replayScriptPath = resourceWorktreePath_ / DIAGNOSTIC_REPLAY_SCRIPT_FILE_NAME;
        ASCENDLOGI(
            "Compilation diagnostics saved: compile_log=%s replay_script=%s; inspect the log or run the replay "
            "script to reproduce the failed command",
            compilationLogPath.c_str(), replayScriptPath.c_str());
    } catch (...) {
        ASCENDLOGI("Compilation diagnostics were enabled; inspect the retained resource worktree");
    }
}

void KernelSpecializationDiagnostics::WriteSpecializationResult(
    const fs::path& outputElfPath, OutputPublicationStatus publicationStatus) noexcept
{
    if (!diagnosticsEnabled_) {
        return;
    }
    constexpr int diagnosticJsonIndentationWidth = 2;
    try {
        nlohmann::json resultJson{
            {"specialization_session_id", specializationSessionId_},
            {"output_path", outputElfPath.string()},
            {"output_published", publicationStatus == OutputPublicationStatus::Published},
        };
        const fs::path resultPath = resourceWorktreePath_ / DIAGNOSTIC_RESULT_FILE_NAME;
        WriteDiagnosticTextFile(resultPath, resultJson.dump(diagnosticJsonIndentationWidth) + '\n', std::ios::trunc);
    } catch (...) {
        ASCENDLOGW(
            "Failed to record specialization result: resource_worktree=%s file=%s", resourceWorktreePath_.c_str(),
            DIAGNOSTIC_RESULT_FILE_NAME);
    }
}

void KernelSpecializationDiagnostics::AppendTextToDiagnosticFile(
    const fs::path& filePath, const std::string& text) noexcept
{
    if (!diagnosticsEnabled_) {
        return;
    }
    try {
        WriteDiagnosticTextFile(filePath, text, std::ios::app);
    } catch (...) {
        ASCENDLOGW("Failed to append diagnostic file: path=%s", filePath.c_str());
    }
}

} // namespace aclrtc
} // namespace ascendc
