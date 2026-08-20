/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "compilation_plan_executor.h"

#include "kernel_specialization_diagnostics.h"
#include "ascendc_tool_log.h"

#include <algorithm>
#include <cerrno>
#include <csignal>
#include <cstring>
#include <exception>
#include <fcntl.h>
#include <new>
#include <poll.h>
#include <spawn.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

namespace ascendc {
namespace aclrtc {
namespace {
using SteadyClock = std::chrono::steady_clock;
constexpr uint64_t MAX_PROCESS_OUTPUT_READ_BYTES_PER_WAIT_ITERATION = 64U * 1024U;
constexpr int64_t MAX_OUTPUT_WAIT_INTERVAL_MILLISECONDS = 50;
constexpr int64_t MAX_PROCESS_CHECK_INTERVAL_AFTER_OUTPUT_EOF_MILLISECONDS = 5;

enum class ProcessOutputDrainStatus : uint32_t {
    NoDataAvailable,
    EndOfFile,
    ReadBudgetExhausted,
    Failed,
};

class ScopedFileDescriptor final {
public:
    explicit ScopedFileDescriptor(int fileDescriptor) noexcept : fileDescriptor_(fileDescriptor) {}

    ~ScopedFileDescriptor() noexcept { Close(); }

    ScopedFileDescriptor(const ScopedFileDescriptor&) = delete;
    ScopedFileDescriptor& operator=(const ScopedFileDescriptor&) = delete;

    int GetFileDescriptor() const noexcept { return fileDescriptor_; }

    void TakeOwnership(int fileDescriptor) noexcept
    {
        Close();
        fileDescriptor_ = fileDescriptor;
    }

    void Close() noexcept
    {
        if (fileDescriptor_ < 0) {
            return;
        }
        const int closingFileDescriptor = fileDescriptor_;
        fileDescriptor_ = -1;
        if (close(closingFileDescriptor) != 0) {
            const int closeError = errno;
            ASCENDLOGW(
                "Failed to close command pipe descriptor: fd=%d errno=%d message=%s", closingFileDescriptor, closeError,
                std::strerror(closeError));
        }
    }

private:
    int fileDescriptor_{-1};
};

void CloseFileDescriptorBestEffort(int fileDescriptor) noexcept
{
    if (fileDescriptor >= 0 && close(fileDescriptor) != 0) {
        const int closeError = errno;
        ASCENDLOGW(
            "Failed to close command pipe descriptor: fd=%d errno=%d message=%s", fileDescriptor, closeError,
            std::strerror(closeError));
    }
}

class CommandSpawnConfiguration final {
public:
    CommandSpawnConfiguration() = default;

    ~CommandSpawnConfiguration() noexcept
    {
        if (spawnAttributesInitialized_) {
            const int destroyResult = posix_spawnattr_destroy(&spawnAttributes_);
            if (destroyResult != 0) {
                ASCENDLOGW(
                    "Failed to destroy spawn attributes: error=%d message=%s", destroyResult,
                    std::strerror(destroyResult));
            }
        }
        if (fileActionsInitialized_) {
            const int destroyResult = posix_spawn_file_actions_destroy(&fileActions_);
            if (destroyResult != 0) {
                ASCENDLOGW(
                    "Failed to destroy spawn file actions: error=%d message=%s", destroyResult,
                    std::strerror(destroyResult));
            }
        }
    }

    CommandSpawnConfiguration(const CommandSpawnConfiguration&) = delete;
    CommandSpawnConfiguration& operator=(const CommandSpawnConfiguration&) = delete;

    bool InitializeCapturedOutputAndProcessGroup(int outputReadFileDescriptor, int outputWriteFileDescriptor) noexcept
    {
        int setupResult = posix_spawn_file_actions_init(&fileActions_);
        if (setupResult != 0) {
            ASCENDLOGE(
                "Failed to initialize spawn file actions: error=%d message=%s", setupResult,
                std::strerror(setupResult));
            return false;
        }
        fileActionsInitialized_ = true;

        setupResult = posix_spawn_file_actions_addclose(&fileActions_, outputReadFileDescriptor);
        if (setupResult == 0) {
            setupResult = posix_spawn_file_actions_adddup2(&fileActions_, outputWriteFileDescriptor, STDOUT_FILENO);
        }
        if (setupResult == 0) {
            setupResult = posix_spawn_file_actions_adddup2(&fileActions_, outputWriteFileDescriptor, STDERR_FILENO);
        }
        if (setupResult == 0) {
            setupResult = posix_spawn_file_actions_addclose(&fileActions_, outputWriteFileDescriptor);
        }
        if (setupResult != 0) {
            ASCENDLOGE(
                "Failed to configure spawn file actions: error=%d message=%s", setupResult, std::strerror(setupResult));
            return false;
        }

        setupResult = posix_spawnattr_init(&spawnAttributes_);
        if (setupResult != 0) {
            ASCENDLOGE(
                "Failed to initialize spawn attributes: error=%d message=%s", setupResult, std::strerror(setupResult));
            return false;
        }
        spawnAttributesInitialized_ = true;
        setupResult = posix_spawnattr_setflags(&spawnAttributes_, POSIX_SPAWN_SETPGROUP);
        if (setupResult == 0) {
            setupResult = posix_spawnattr_setpgroup(&spawnAttributes_, 0);
        }
        if (setupResult != 0) {
            ASCENDLOGE(
                "Failed to configure spawn process group: error=%d message=%s", setupResult,
                std::strerror(setupResult));
            return false;
        }
        return true;
    }

    posix_spawn_file_actions_t* GetFileActions() noexcept { return &fileActions_; }

    posix_spawnattr_t* GetSpawnAttributes() noexcept { return &spawnAttributes_; }

private:
    posix_spawn_file_actions_t fileActions_{};
    posix_spawnattr_t spawnAttributes_{};
    bool fileActionsInitialized_{false};
    bool spawnAttributesInitialized_{false};
};

void SignalChildProcessGroup(pid_t childProcessId, int signalNumber) noexcept
{
    if (kill(-childProcessId, signalNumber) == 0) {
        return;
    }
    const int processGroupError = errno;
    if (processGroupError != ESRCH) {
        ASCENDLOGW(
            "Failed to signal command process group: pid=%d signal=%d errno=%d message=%s",
            static_cast<int>(childProcessId), signalNumber, processGroupError, std::strerror(processGroupError));
    }
    if (kill(childProcessId, signalNumber) == 0 || errno == ESRCH) {
        return;
    }
    const int processLeaderError = errno;
    ASCENDLOGW(
        "Failed to signal command process leader: pid=%d signal=%d errno=%d message=%s",
        static_cast<int>(childProcessId), signalNumber, processLeaderError, std::strerror(processLeaderError));
}

ProcessOutputDrainStatus DrainProcessOutputUpToReadBudget(
    int outputFileDescriptor, uint64_t capturedOutputByteLimit, CompilationProcessResult& processResult)
{
    char outputBuffer[4096];
    uint64_t readByteCountThisIteration = 0U;
    while (readByteCountThisIteration < MAX_PROCESS_OUTPUT_READ_BYTES_PER_WAIT_ITERATION) {
        const uint64_t remainingReadBudget =
            MAX_PROCESS_OUTPUT_READ_BYTES_PER_WAIT_ITERATION - readByteCountThisIteration;
        const size_t requestedReadByteCount =
            static_cast<size_t>(std::min<uint64_t>(sizeof(outputBuffer), remainingReadBudget));
        const ssize_t readByteCount = read(outputFileDescriptor, outputBuffer, requestedReadByteCount);
        if (readByteCount > 0) {
            readByteCountThisIteration += static_cast<uint64_t>(readByteCount);
            const uint64_t capturedOutputByteCount = static_cast<uint64_t>(processResult.capturedOutput.size());
            const uint64_t availableCapacity = capturedOutputByteLimit > capturedOutputByteCount ?
                                                   capturedOutputByteLimit - capturedOutputByteCount :
                                                   0U;
            const size_t appendByteCount =
                static_cast<size_t>(std::min(static_cast<uint64_t>(readByteCount), availableCapacity));
            processResult.capturedOutput.append(outputBuffer, appendByteCount);
            processResult.capturedOutputTruncated =
                processResult.capturedOutputTruncated || appendByteCount < static_cast<size_t>(readByteCount);
            continue;
        }
        if (readByteCount == 0) {
            return ProcessOutputDrainStatus::EndOfFile;
        }
        if (errno == EINTR) {
            continue;
        }
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return ProcessOutputDrainStatus::NoDataAvailable;
        }
        const int readError = errno;
        ASCENDLOGE(
            "Failed to read command output: fd=%d errno=%d message=%s", outputFileDescriptor, readError,
            std::strerror(readError));
        return ProcessOutputDrainStatus::Failed;
    }
    return ProcessOutputDrainStatus::ReadBudgetExhausted;
}

bool WaitForOutputOrNextProcessCheck(int outputFileDescriptor, bool outputPipeClosed, int pollTimeout)
{
    pollfd outputPollDescriptor{outputFileDescriptor, POLLIN, 0};
    pollfd* pollDescriptors = outputPipeClosed ? nullptr : &outputPollDescriptor;
    const nfds_t pollDescriptorCount = outputPipeClosed ? 0U : 1U;
    const int pollResult = poll(pollDescriptors, pollDescriptorCount, pollTimeout);
    if (pollResult < 0 && errno != EINTR) {
        const int pollError = errno;
        ASCENDLOGE(
            "Failed to poll command output: fd=%d errno=%d message=%s", outputFileDescriptor, pollError,
            std::strerror(pollError));
        return false;
    }
    return true;
}

bool CreateNonblockingCloseOnExecPipe(int outputPipe[2])
{
    if (pipe2(outputPipe, O_CLOEXEC) != 0) {
        const int pipeError = errno;
        ASCENDLOGE("Failed to create command output pipe: errno=%d message=%s", pipeError, std::strerror(pipeError));
        return false;
    }
    const int readFlags = fcntl(outputPipe[0], F_GETFL, 0);
    if (readFlags < 0) {
        const int flagError = errno;
        ASCENDLOGE(
            "Failed to read command pipe flags: fd=%d errno=%d message=%s", outputPipe[0], flagError,
            std::strerror(flagError));
        CloseFileDescriptorBestEffort(outputPipe[0]);
        CloseFileDescriptorBestEffort(outputPipe[1]);
        return false;
    }
    if (fcntl(outputPipe[0], F_SETFL, readFlags | O_NONBLOCK) != 0) {
        const int nonblockingError = errno;
        ASCENDLOGE(
            "Failed to make command pipe nonblocking: fd=%d errno=%d message=%s", outputPipe[0], nonblockingError,
            std::strerror(nonblockingError));
        CloseFileDescriptorBestEffort(outputPipe[0]);
        CloseFileDescriptorBestEffort(outputPipe[1]);
        return false;
    }
    return true;
}

bool HasEmbeddedNull(const std::string& text) noexcept { return text.find('\0') != std::string::npos; }

const char* GetProcessTerminationName(CompilationProcessTermination termination) noexcept
{
    switch (termination) {
        case CompilationProcessTermination::Exited:
            return "exited";
        case CompilationProcessTermination::Signaled:
            return "signaled";
        case CompilationProcessTermination::TimedOut:
            return "timed_out";
        case CompilationProcessTermination::InfrastructureFailure:
        default:
            return "infrastructure_failure";
    }
}

bool BuildNullTerminatedSpawnArguments(
    const CompilationCommand& compilationCommand, std::vector<std::string>& argumentStorage,
    std::vector<char*>& spawnArgv)
{
    const std::string executablePath = compilationCommand.executablePath.string();
    if (HasEmbeddedNull(executablePath)) {
        ASCENDLOGE(
            "Command %s has an executable path containing an embedded NUL; regenerate the JIT resource",
            compilationCommand.diagnosticLabel.c_str());
        return false;
    }
    for (size_t argumentIndex = 0U; argumentIndex < compilationCommand.commandArguments.size(); ++argumentIndex) {
        const std::string& argument = compilationCommand.commandArguments[argumentIndex];
        if (HasEmbeddedNull(argument)) {
            ASCENDLOGE(
                "Command %s argv[%zu] contains an embedded NUL; regenerate the JIT resource",
                compilationCommand.diagnosticLabel.c_str(), argumentIndex);
            return false;
        }
    }

    argumentStorage.reserve(compilationCommand.commandArguments.size() + 1U);
    argumentStorage.emplace_back(executablePath);
    argumentStorage.insert(
        argumentStorage.end(), compilationCommand.commandArguments.begin(), compilationCommand.commandArguments.end());
    spawnArgv.reserve(argumentStorage.size() + 1U);
    for (std::string& argument : argumentStorage) {
        spawnArgv.push_back(argument.data());
    }
    spawnArgv.push_back(nullptr);
    return true;
}

class SpawnedToolProcess final {
public:
    explicit SpawnedToolProcess(std::chrono::milliseconds terminationGracePeriod) noexcept
        : terminationGracePeriod_(terminationGracePeriod)
    {}

    ~SpawnedToolProcess() noexcept
    {
        if (!childProcessNeedsReaping_) {
            return;
        }
        int ignoredChildStatus = 0;
        TerminateAndReapProcessGroup(ignoredChildStatus);
    }

    SpawnedToolProcess(const SpawnedToolProcess&) = delete;
    SpawnedToolProcess& operator=(const SpawnedToolProcess&) = delete;

    aclError StartWithCapturedOutput(const CompilationCommand& compilationCommand)
    {
        std::vector<std::string> argumentStorage;
        std::vector<char*> spawnArgv;
        if (!BuildNullTerminatedSpawnArguments(compilationCommand, argumentStorage, spawnArgv)) {
            return ACLRTC_ERROR_FAILURE;
        }

        int outputPipe[2]{-1, -1};
        if (!CreateNonblockingCloseOnExecPipe(outputPipe)) {
            return ACLRTC_ERROR_FAILURE;
        }
        outputReadFileDescriptor_.TakeOwnership(outputPipe[0]);
        ScopedFileDescriptor outputWriteFileDescriptor(outputPipe[1]);

        CommandSpawnConfiguration spawnConfiguration;
        if (!spawnConfiguration.InitializeCapturedOutputAndProcessGroup(
                outputReadFileDescriptor_.GetFileDescriptor(), outputWriteFileDescriptor.GetFileDescriptor())) {
            return ACLRTC_ERROR_FAILURE;
        }

        startedAt_ = SteadyClock::now();
        const int spawnResult = posix_spawn(
            &childProcessId_, argumentStorage.front().c_str(), spawnConfiguration.GetFileActions(),
            spawnConfiguration.GetSpawnAttributes(), spawnArgv.data(), ::environ);
        outputWriteFileDescriptor.Close();
        if (spawnResult != 0) {
            ASCENDLOGE(
                "Failed to spawn command %s: error=%d message=%s executable=%s",
                compilationCommand.diagnosticLabel.c_str(), spawnResult, std::strerror(spawnResult),
                compilationCommand.executablePath.c_str());
            return ACLRTC_ERROR_FAILURE;
        }
        childProcessNeedsReaping_ = true;
        return ACLRTC_SUCCESS;
    }

    aclError WaitForExitAndCaptureOutput(
        std::chrono::milliseconds executionTimeout, uint64_t capturedOutputByteLimit,
        const std::string& commandDiagnosticLabel, CompilationProcessResult& processResult)
    {
        const SteadyClock::time_point executionDeadline = startedAt_ + executionTimeout;
        int childStatus = 0;
        bool infrastructureFailure = false;
        bool outputPipeClosed = false;
        bool commandLeaderExited = false;
        while (true) {
            ProcessOutputDrainStatus outputDrainStatus = ProcessOutputDrainStatus::EndOfFile;
            if (!outputPipeClosed) {
                outputDrainStatus = DrainProcessOutputUpToReadBudget(
                    outputReadFileDescriptor_.GetFileDescriptor(), capturedOutputByteLimit, processResult);
                if (outputDrainStatus == ProcessOutputDrainStatus::Failed) {
                    infrastructureFailure = true;
                    break;
                }
                if (outputDrainStatus == ProcessOutputDrainStatus::EndOfFile) {
                    outputReadFileDescriptor_.Close();
                    outputPipeClosed = true;
                }
            }
            if (!commandLeaderExited) {
                const CommandLeaderExitState leaderExitState = ObserveCommandLeaderExitWithoutReaping();
                if (leaderExitState == CommandLeaderExitState::InspectionFailed) {
                    infrastructureFailure = true;
                    break;
                }
                commandLeaderExited = leaderExitState == CommandLeaderExitState::Exited;
            }
            if (commandLeaderExited && outputPipeClosed) {
                break;
            }
            if (SteadyClock::now() >= executionDeadline) {
                processResult.termination = CompilationProcessTermination::TimedOut;
                ASCENDLOGE(
                    "Command %s exceeded its %lld ms execution timeout; inspect the compiler output or a stalled "
                    "tool process",
                    commandDiagnosticLabel.c_str(), static_cast<long long>(executionTimeout.count()));
                TerminateAndReapProcessGroup(childStatus);
                break;
            }
            if (outputDrainStatus == ProcessOutputDrainStatus::ReadBudgetExhausted) {
                continue;
            }

            const auto remainingTime =
                std::chrono::duration_cast<std::chrono::milliseconds>(executionDeadline - SteadyClock::now());
            const int64_t maximumPollInterval = outputPipeClosed ?
                                                    MAX_PROCESS_CHECK_INTERVAL_AFTER_OUTPUT_EOF_MILLISECONDS :
                                                    MAX_OUTPUT_WAIT_INTERVAL_MILLISECONDS;
            const int pollTimeout =
                static_cast<int>(std::max<int64_t>(1, std::min<int64_t>(remainingTime.count(), maximumPollInterval)));
            if (!WaitForOutputOrNextProcessCheck(
                    outputReadFileDescriptor_.GetFileDescriptor(), outputPipeClosed, pollTimeout)) {
                infrastructureFailure = true;
                break;
            }
        }

        if (infrastructureFailure) {
            TerminateAndReapProcessGroup(childStatus);
        } else if (
            processResult.termination != CompilationProcessTermination::TimedOut && !ReapCommandLeader(childStatus)) {
            infrastructureFailure = true;
            TerminateAndReapProcessGroup(childStatus);
        }
        if (!outputPipeClosed) {
            const ProcessOutputDrainStatus finalDrainStatus = DrainProcessOutputUpToReadBudget(
                outputReadFileDescriptor_.GetFileDescriptor(), capturedOutputByteLimit, processResult);
            if (finalDrainStatus == ProcessOutputDrainStatus::Failed) {
                infrastructureFailure = true;
            }
        }
        outputReadFileDescriptor_.Close();
        processResult.elapsedTime =
            std::chrono::duration_cast<std::chrono::milliseconds>(SteadyClock::now() - startedAt_);
        if (infrastructureFailure) {
            processResult.termination = CompilationProcessTermination::InfrastructureFailure;
            return ACLRTC_ERROR_FAILURE;
        }

        if (processResult.termination != CompilationProcessTermination::TimedOut) {
            if (WIFEXITED(childStatus)) {
                processResult.termination = CompilationProcessTermination::Exited;
                processResult.terminationCode = static_cast<int32_t>(WEXITSTATUS(childStatus));
            } else if (WIFSIGNALED(childStatus)) {
                processResult.termination = CompilationProcessTermination::Signaled;
                processResult.terminationCode = static_cast<int32_t>(WTERMSIG(childStatus));
            } else {
                processResult.termination = CompilationProcessTermination::InfrastructureFailure;
                ASCENDLOGE(
                    "Command process returned an unsupported wait status: pid=%d status=%d",
                    static_cast<int>(childProcessId_), childStatus);
                return ACLRTC_ERROR_FAILURE;
            }
        }
        return ACLRTC_SUCCESS;
    }

private:
    enum class CommandLeaderExitState : uint32_t {
        Running,
        Exited,
        InspectionFailed,
    };

    CommandLeaderExitState ObserveCommandLeaderExitWithoutReaping() const noexcept
    {
        siginfo_t childInformation{};
        int waitResult = 0;
        do {
            waitResult =
                waitid(P_PID, static_cast<id_t>(childProcessId_), &childInformation, WEXITED | WNOHANG | WNOWAIT);
        } while (waitResult != 0 && errno == EINTR);
        if (waitResult != 0) {
            const int waitError = errno;
            ASCENDLOGE(
                "Failed to inspect command process: pid=%d errno=%d message=%s", static_cast<int>(childProcessId_),
                waitError, std::strerror(waitError));
            return CommandLeaderExitState::InspectionFailed;
        }
        return childInformation.si_pid == 0 ? CommandLeaderExitState::Running : CommandLeaderExitState::Exited;
    }

    bool ReapCommandLeader(int& childStatus) noexcept
    {
        if (!childProcessNeedsReaping_) {
            return true;
        }
        pid_t waitResult;
        do {
            waitResult = waitpid(childProcessId_, &childStatus, 0);
        } while (waitResult < 0 && errno == EINTR);
        if (waitResult != childProcessId_) {
            const int waitError = errno;
            ASCENDLOGE(
                "Failed to reap command process: pid=%d errno=%d message=%s", static_cast<int>(childProcessId_),
                waitError, std::strerror(waitError));
            return false;
        }
        childProcessNeedsReaping_ = false;
        return true;
    }

    void TerminateAndReapProcessGroup(int& childStatus) noexcept
    {
        if (!childProcessNeedsReaping_) {
            return;
        }
        SignalChildProcessGroup(childProcessId_, SIGTERM);
        // Keep the leader unreaped so its PID cannot be reused as another process group ID during the grace period.
        std::this_thread::sleep_for(terminationGracePeriod_);
        SignalChildProcessGroup(childProcessId_, SIGKILL);
        (void)ReapCommandLeader(childStatus);
    }

    ScopedFileDescriptor outputReadFileDescriptor_{-1};
    pid_t childProcessId_{-1};
    SteadyClock::time_point startedAt_{};
    std::chrono::milliseconds terminationGracePeriod_{0};
    bool childProcessNeedsReaping_{false};
};

aclError MapCommandProcessResultToAclError(
    CompilationCommandKind commandKind, const CompilationProcessResult& processResult) noexcept
{
    const bool commandSucceeded =
        processResult.termination == CompilationProcessTermination::Exited && processResult.terminationCode == 0;
    if (commandSucceeded) {
        return ACLRTC_SUCCESS;
    }
    return commandKind == CompilationCommandKind::Link ? ACLRTC_ERROR_LINKING : ACLRTC_ERROR_COMPILATION;
}
} // namespace

CompilationCommandExecutionLimits CompilationCommandExecutionLimits::ProductionDefaults() noexcept
{
    return {std::chrono::minutes(5), std::chrono::minutes(1), std::chrono::seconds(5), 4U * 1024U * 1024U};
}

CompilationPlanExecutor::CompilationPlanExecutor(
    KernelSpecializationDiagnostics& specializationDiagnostics, CompilationCommandExecutionLimits executionLimits)
    : specializationDiagnostics_(specializationDiagnostics), executionLimits_(executionLimits)
{}

aclError CompilationPlanExecutor::ExecuteCompilationPlan(const KernelCompilationPlan& compilationPlan) const
{
    try {
        for (const CompilationCommand& compilationCommand : compilationPlan.compilationCommands) {
            ASCENDLOGI(
                "Executing command %s with %s", compilationCommand.diagnosticLabel.c_str(),
                compilationCommand.executablePath.c_str());
            specializationDiagnostics_.AppendCommandToReplayScript(compilationCommand);
            CompilationProcessResult processResult;
            const aclError executionResult = ExecuteCompilationCommand(compilationCommand, processResult);
            specializationDiagnostics_.AppendCommandResultToCompilationLog(compilationCommand, processResult);
            if (executionResult != ACLRTC_SUCCESS) {
                constexpr size_t maxLoggedProcessOutputBytes = 4096U;
                const size_t loggedOutputByteCount =
                    std::min(processResult.capturedOutput.size(), maxLoggedProcessOutputBytes);
                const bool loggedOutputTruncated = processResult.capturedOutput.size() > loggedOutputByteCount;
                ASCENDLOGE(
                    "Command %s failed: termination=%s code=%d elapsed_ms=%lld captured_output_truncated=%s "
                    "logged_output_truncated=%s output_prefix_bytes=%zu output=%.*s",
                    compilationCommand.diagnosticLabel.c_str(), GetProcessTerminationName(processResult.termination),
                    static_cast<int>(processResult.terminationCode),
                    static_cast<long long>(processResult.elapsedTime.count()),
                    processResult.capturedOutputTruncated ? "true" : "false", loggedOutputTruncated ? "true" : "false",
                    loggedOutputByteCount, static_cast<int>(loggedOutputByteCount),
                    processResult.capturedOutput.c_str());
                specializationDiagnostics_.LogCommandFailureRecoveryHint();
                return executionResult;
            }
        }
        return ACLRTC_SUCCESS;
    } catch (const std::bad_alloc&) {
        ASCENDLOGE("Out of memory while executing kernel compilation commands");
        return ACLRTC_ERROR_OUT_OF_MEMORY;
    } catch (const std::exception& exception) {
        ASCENDLOGE("Kernel compilation command execution failed with exception: %s", exception.what());
        return ACLRTC_ERROR_FAILURE;
    } catch (...) {
        ASCENDLOGE("Kernel compilation command execution failed with an unknown exception");
        return ACLRTC_ERROR_FAILURE;
    }
}

aclError CompilationPlanExecutor::ExecuteCompilationCommand(
    const CompilationCommand& compilationCommand, CompilationProcessResult& processResult) const
{
    SpawnedToolProcess toolProcess(executionLimits_.terminationGracePeriod);
    aclError executionResult = toolProcess.StartWithCapturedOutput(compilationCommand);
    if (executionResult != ACLRTC_SUCCESS) {
        return executionResult;
    }

    executionResult = toolProcess.WaitForExitAndCaptureOutput(
        GetCommandExecutionTimeout(compilationCommand.commandKind), executionLimits_.capturedOutputByteLimit,
        compilationCommand.diagnosticLabel, processResult);
    if (executionResult != ACLRTC_SUCCESS) {
        return executionResult;
    }
    return MapCommandProcessResultToAclError(compilationCommand.commandKind, processResult);
}

std::chrono::milliseconds CompilationPlanExecutor::GetCommandExecutionTimeout(
    CompilationCommandKind commandKind) const noexcept
{
    return commandKind == CompilationCommandKind::Compile ? executionLimits_.compileCommandTimeout :
                                                            executionLimits_.objectCopyAndLinkCommandTimeout;
}

} // namespace aclrtc
} // namespace ascendc
