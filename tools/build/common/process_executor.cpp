/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "process_executor.h"

#include "ascendc_tool_log.h"

#include <algorithm>
#include <array>
#include <cerrno>
#include <csignal>
#include <cstring>
#include <exception>
#include <fcntl.h>
#include <fstream>
#include <map>
#include <new>
#include <poll.h>
#include <spawn.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <utility>

namespace ascendc {
namespace {
using SteadyClock = std::chrono::steady_clock;

constexpr uint64_t MAX_OUTPUT_READ_BYTES_PER_ITERATION = 64U * 1024U;
constexpr int64_t MAX_PROCESS_WAIT_MILLISECONDS = 5;

// Owns a file descriptor and guarantees that it is closed on every return path.
class FileDescriptor final {
public:
    explicit FileDescriptor(int value = -1) noexcept : value_(value) {}
    ~FileDescriptor() noexcept { Close(); }

    FileDescriptor(const FileDescriptor&) = delete;
    FileDescriptor& operator=(const FileDescriptor&) = delete;

    int Get() const noexcept { return value_; }

    void Reset(int value = -1) noexcept
    {
        Close();
        value_ = value;
    }

    void Close() noexcept
    {
        if (value_ < 0) {
            return;
        }
        const int valueToClose = value_;
        value_ = -1;
        if (close(valueToClose) != 0) {
            const int closeError = errno;
            ASCENDLOGW(
                "Failed to close process file descriptor: fd=%d errno=%d message=%s", valueToClose, closeError,
                std::strerror(closeError));
        }
    }

private:
    int value_;
};

// Moves a pipe descriptor away from stdin/stdout/stderr so that the spawn-time
// dup2 operations cannot accidentally overwrite another descriptor used below.
bool RelocateFileDescriptorAboveStandardStreams(FileDescriptor& fileDescriptor) noexcept
{
    if (fileDescriptor.Get() > STDERR_FILENO) {
        return true;
    }
    const int relocatedFileDescriptor = fcntl(fileDescriptor.Get(), F_DUPFD_CLOEXEC, STDERR_FILENO + 1);
    if (relocatedFileDescriptor < 0) {
        const int duplicateError = errno;
        ASCENDLOGE(
            "Failed to relocate process pipe file descriptor: fd=%d errno=%d message=%s", fileDescriptor.Get(),
            duplicateError, std::strerror(duplicateError));
        return false;
    }
    fileDescriptor.Reset(relocatedFileDescriptor);
    return true;
}

// Releases partially initialized posix_spawn resources when setup fails.
struct PosixSpawnResources {
    ~PosixSpawnResources() noexcept
    {
        if (attributesInitialized_) {
            (void)posix_spawnattr_destroy(&attributes_);
        }
        if (fileActionsInitialized_) {
            (void)posix_spawn_file_actions_destroy(&fileActions_);
        }
    }

    posix_spawn_file_actions_t fileActions_{};
    posix_spawnattr_t attributes_{};
    bool fileActionsInitialized_{false};
    bool attributesInitialized_{false};
};

// Builds the null-terminated argv/envp layout required by posix_spawn. The
// returned pointers remain valid only while the source strings are unchanged.
std::vector<char*> BuildCStringPointers(const std::vector<std::string>& strings)
{
    std::vector<char*> pointers;
    pointers.reserve(strings.size() + 1U);
    for (const std::string& string : strings) {
        pointers.push_back(const_cast<char*>(string.c_str()));
    }
    pointers.push_back(nullptr);
    return pointers;
}

enum class ProcessOutputKind : uint32_t {
    StandardOutput,
    StandardError,
};

// Captures one child output stream and forwards each byte to both plog and the
// optional mirrored output file.
class ProcessOutputForwarder final {
public:
    ProcessOutputForwarder(ProcessOutputKind outputKind, std::string executablePath, std::ofstream& outputLog)
        : outputKind_(outputKind), executablePath_(std::move(executablePath)), outputLog_(outputLog)
    {}

    int GetReadFileDescriptor() const noexcept { return readFileDescriptor_.Get(); }
    int GetWriteFileDescriptor() const noexcept { return writeFileDescriptor_.Get(); }
    bool IsOpen() const noexcept { return readFileDescriptor_.Get() >= 0; }

    // Creates a nonblocking, close-on-exec pipe for reading child output without
    // delaying timeout checks or leaking unrelated descriptors into the child.
    bool CreatePipe() noexcept
    {
        constexpr size_t pipeFileDescriptorCount = 2U;
        int pipeFileDescriptors[pipeFileDescriptorCount]{-1, -1};
        if (pipe2(pipeFileDescriptors, O_CLOEXEC) != 0) {
            const int pipeError = errno;
            ASCENDLOGE(
                "Failed to create process output pipe: errno=%d message=%s", pipeError, std::strerror(pipeError));
            return false;
        }
        readFileDescriptor_.Reset(pipeFileDescriptors[0]);
        writeFileDescriptor_.Reset(pipeFileDescriptors[1]);
        if (!RelocateFileDescriptorAboveStandardStreams(readFileDescriptor_) ||
            !RelocateFileDescriptorAboveStandardStreams(writeFileDescriptor_)) {
            return false;
        }
        const int readFlags = fcntl(readFileDescriptor_.Get(), F_GETFL, 0);
        if (readFlags < 0 || fcntl(readFileDescriptor_.Get(), F_SETFL, readFlags | O_NONBLOCK) != 0) {
            const int flagError = errno;
            ASCENDLOGE(
                "Failed to make process output pipe nonblocking: fd=%d errno=%d message=%s", readFileDescriptor_.Get(),
                flagError, std::strerror(flagError));
            return false;
        }
        return true;
    }

    void CloseWriteEnd() noexcept { writeFileDescriptor_.Close(); }

    // Drains currently available data. Each call has a byte budget so a noisy
    // stream cannot indefinitely postpone checking the child state and timeout.
    bool ForwardAvailableOutput() noexcept
    {
        char readBuffer[4096];
        uint64_t totalReadBytes = 0U;
        while (totalReadBytes < MAX_OUTPUT_READ_BYTES_PER_ITERATION) {
            const ssize_t readBytes = read(readFileDescriptor_.Get(), readBuffer, sizeof(readBuffer));
            if (readBytes > 0) {
                const size_t outputBytes = static_cast<size_t>(readBytes);
                totalReadBytes += static_cast<uint64_t>(outputBytes);
                MirrorOutputToFile(readBuffer, outputBytes);
                ForwardOutputToPlog(readBuffer, outputBytes);
                continue;
            }
            if (readBytes == 0) {
                Close();
                return true;
            }
            if (errno == EINTR) {
                continue;
            }
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return true;
            }
            const int readError = errno;
            ASCENDLOGE(
                "Failed to read process output: executable=%s stream=%s errno=%d message=%s", executablePath_.c_str(),
                GetOutputKindName(), readError, std::strerror(readError));
            Close();
            return false;
        }
        return true;
    }

    void Close() noexcept { readFileDescriptor_.Close(); }

private:
    void MirrorOutputToFile(const char* output, size_t outputBytes) noexcept
    {
        if (!outputLog_.is_open()) {
            return;
        }
        outputLog_.write(output, static_cast<std::streamsize>(outputBytes));
        outputLog_.flush();
        if (!outputLog_) {
            // Mirroring is diagnostic only; a log I/O failure must not turn a
            // successfully running compiler process into an executor failure.
            ASCENDLOGW("Failed to save process output; compilation will continue");
            outputLog_.close();
        }
    }

    void ForwardOutputToPlog(const char* output, size_t outputBytes) noexcept
    {
        const int plogOutputBytes = static_cast<int>(outputBytes);
        if (outputKind_ == ProcessOutputKind::StandardOutput) {
            ASCENDLOGI("Process stdout: executable=%s output=%.*s", executablePath_.c_str(), plogOutputBytes, output);
        } else {
            ASCENDLOGE("Process stderr: executable=%s output=%.*s", executablePath_.c_str(), plogOutputBytes, output);
        }
    }

    const char* GetOutputKindName() const noexcept
    {
        return outputKind_ == ProcessOutputKind::StandardOutput ? "stdout" : "stderr";
    }

    ProcessOutputKind outputKind_;
    std::string executablePath_;
    std::ofstream& outputLog_;
    FileDescriptor readFileDescriptor_;
    FileDescriptor writeFileDescriptor_;
};

// Configures the child process group and redirects its output streams. Keeping
// these file actions together preserves the ordering required before posix_spawn.
bool ConfigureSpawnResources(
    PosixSpawnResources& spawnResources, const ProcessOutputForwarder& stdoutForwarder,
    const ProcessOutputForwarder& stderrForwarder) noexcept
{
    int setupResult = posix_spawnattr_init(&spawnResources.attributes_);
    if (setupResult == 0) {
        spawnResources.attributesInitialized_ = true;
        // A dedicated group lets timeout handling terminate the whole command
        // tree instead of leaving compiler/helper descendants behind.
        setupResult = posix_spawnattr_setflags(&spawnResources.attributes_, POSIX_SPAWN_SETPGROUP);
    }
    if (setupResult == 0) {
        setupResult = posix_spawnattr_setpgroup(&spawnResources.attributes_, 0);
    }
    if (setupResult == 0) {
        setupResult = posix_spawn_file_actions_init(&spawnResources.fileActions_);
        spawnResources.fileActionsInitialized_ = setupResult == 0;
    }
    const std::array<int, 2U> readFileDescriptors{
        stdoutForwarder.GetReadFileDescriptor(), stderrForwarder.GetReadFileDescriptor()};
    for (const int readFileDescriptor : readFileDescriptors) {
        if (setupResult == 0) {
            setupResult = posix_spawn_file_actions_addclose(&spawnResources.fileActions_, readFileDescriptor);
        }
    }
    if (setupResult == 0) {
        setupResult = posix_spawn_file_actions_adddup2(
            &spawnResources.fileActions_, stdoutForwarder.GetWriteFileDescriptor(), STDOUT_FILENO);
    }
    if (setupResult == 0) {
        setupResult = posix_spawn_file_actions_adddup2(
            &spawnResources.fileActions_, stderrForwarder.GetWriteFileDescriptor(), STDERR_FILENO);
    }
    const std::array<int, 2U> writeFileDescriptors{
        stdoutForwarder.GetWriteFileDescriptor(), stderrForwarder.GetWriteFileDescriptor()};
    for (const int writeFileDescriptor : writeFileDescriptors) {
        if (setupResult == 0) {
            setupResult = posix_spawn_file_actions_addclose(&spawnResources.fileActions_, writeFileDescriptor);
        }
    }
    if (setupResult != 0) {
        ASCENDLOGE("Failed to configure process spawn: error=%d message=%s", setupResult, std::strerror(setupResult));
        return false;
    }
    return true;
}

bool ContainsEmbeddedNull(const std::string& text) noexcept { return text.find('\0') != std::string::npos; }

bool IsEnvironmentVariableNameValid(const std::string& name) noexcept
{
    return !name.empty() && name.find('=') == std::string::npos && !ContainsEmbeddedNull(name);
}

// waitpid cannot reliably observe a child when SIGCHLD is ignored or automatic
// child reaping is enabled, so reject such process-wide signal configurations.
bool CanWaitForChildProcesses() noexcept
{
    struct sigaction sigchldDisposition {};
    if (sigaction(SIGCHLD, nullptr, &sigchldDisposition) != 0) {
        const int sigactionError = errno;
        ASCENDLOGE(
            "Failed to inspect SIGCHLD disposition: errno=%d message=%s", sigactionError,
            std::strerror(sigactionError));
        return false;
    }
    if (sigchldDisposition.sa_handler == SIG_IGN || (sigchldDisposition.sa_flags & SA_NOCLDWAIT) != 0) {
        ASCENDLOGE("Process execution requires waitable children; restore the default SIGCHLD disposition and remove "
                   "SA_NOCLDWAIT before invoking ProcessExecutor");
        return false;
    }
    return true;
}

// Validates every value before it is exposed through C-style process APIs.
bool ValidateProcessExecutorRequest(const ProcessExecutorRequest& request) noexcept
{
    if (!CanWaitForChildProcesses()) {
        return false;
    }
    if (request.arguments.empty()) {
        ASCENDLOGE("Process arguments must not be empty");
        return false;
    }
    if (request.executionTimeout.count() <= 0) {
        ASCENDLOGE("Process execution timeout must be positive");
        return false;
    }
    if (request.terminationGracePeriod.count() < 0) {
        ASCENDLOGE("Process termination grace period must not be negative");
        return false;
    }
    for (size_t argumentIndex = 0U; argumentIndex < request.arguments.size(); ++argumentIndex) {
        if (ContainsEmbeddedNull(request.arguments[argumentIndex])) {
            ASCENDLOGE("Process argv[%zu] contains an embedded NUL", argumentIndex);
            return false;
        }
    }
    if (ContainsEmbeddedNull(request.mirroredOutputLogFilePath)) {
        ASCENDLOGE("Process output log path contains an embedded NUL");
        return false;
    }
    for (size_t overrideIndex = 0U; overrideIndex < request.environmentOverrides.size(); ++overrideIndex) {
        const std::pair<std::string, std::string>& environmentOverride = request.environmentOverrides[overrideIndex];
        if (!IsEnvironmentVariableNameValid(environmentOverride.first) ||
            ContainsEmbeddedNull(environmentOverride.second)) {
            ASCENDLOGE(
                "Process environment override[%zu] is invalid: name=%s", overrideIndex,
                environmentOverride.first.c_str());
            return false;
        }
    }
    for (size_t removalIndex = 0U; removalIndex < request.removedEnvironmentVariables.size(); ++removalIndex) {
        if (!IsEnvironmentVariableNameValid(request.removedEnvironmentVariables[removalIndex])) {
            ASCENDLOGE(
                "Process environment removal[%zu] has an invalid variable name: name=%s", removalIndex,
                request.removedEnvironmentVariables[removalIndex].c_str());
            return false;
        }
    }
    return true;
}

// Creates a deterministic environment snapshot. Explicit removals are applied
// first and explicit overrides last, so an override wins if a name appears in
// both request lists.
std::vector<std::string> BuildSpawnEnvironment(const ProcessExecutorRequest& request)
{
    std::map<std::string, std::string> environmentByName;
    for (char** inheritedEnvironment = ::environ; inheritedEnvironment != nullptr && *inheritedEnvironment != nullptr;
         ++inheritedEnvironment) {
        const std::string environmentEntry(*inheritedEnvironment);
        const size_t separatorPosition = environmentEntry.find('=');
        if (separatorPosition != std::string::npos) {
            environmentByName[environmentEntry.substr(0U, separatorPosition)] =
                environmentEntry.substr(separatorPosition + 1U);
        }
    }
    for (const std::string& removedVariable : request.removedEnvironmentVariables) {
        environmentByName.erase(removedVariable);
    }
    for (const std::pair<std::string, std::string>& environmentOverride : request.environmentOverrides) {
        environmentByName[environmentOverride.first] = environmentOverride.second;
    }

    std::vector<std::string> environmentEntries;
    environmentEntries.reserve(environmentByName.size());
    for (const std::pair<const std::string, std::string>& environmentVariable : environmentByName) {
        environmentEntries.emplace_back(environmentVariable.first + "=" + environmentVariable.second);
    }
    return environmentEntries;
}

// Opens the optional combined stdout/stderr mirror. Failure is non-fatal because
// process execution and plog forwarding can still proceed normally.
void OpenMirroredOutputLog(const std::string& outputLogFilePath, std::ofstream& outputLog) noexcept
{
    if (outputLogFilePath.empty()) {
        return;
    }
    outputLog.open(outputLogFilePath, std::ios::out | std::ios::binary | std::ios::app);
    if (outputLog.is_open()) {
        return;
    }
    ASCENDLOGW("Failed to open process output log; compilation will continue: path=%s", outputLogFilePath.c_str());
}

// The negative pid targets the process group created for the child, ensuring
// timeout cleanup also reaches subprocesses started by the requested command.
void SignalProcessGroup(pid_t processLeaderId, int signalNumber) noexcept
{
    if (kill(-processLeaderId, signalNumber) == 0 || errno == ESRCH) {
        return;
    }
    const int processGroupError = errno;
    ASCENDLOGW(
        "Failed to signal process group: pid=%d signal=%d errno=%d message=%s", static_cast<int>(processLeaderId),
        signalNumber, processGroupError, std::strerror(processGroupError));
}

// Owns the complete child lifecycle: spawn configuration, output forwarding,
// timeout enforcement, termination, and final waitpid reaping.
class ChildProcess final {
public:
    explicit ChildProcess(const ProcessExecutorRequest& request)
        : request_(request),
          stdoutForwarder_(ProcessOutputKind::StandardOutput, request.arguments.front(), outputLog_),
          stderrForwarder_(ProcessOutputKind::StandardError, request.arguments.front(), outputLog_)
    {}

    ~ChildProcess() noexcept
    {
        if (needsReaping_) {
            TerminateAndReap();
        }
    }

    ChildProcess(const ChildProcess&) = delete;
    ChildProcess& operator=(const ChildProcess&) = delete;

    // Starts the requested executable in a dedicated process group with stdout
    // and stderr redirected to the parent's forwarding pipes.
    bool Start()
    {
        OpenMirroredOutputLog(request_.mirroredOutputLogFilePath, outputLog_);
        if (!stdoutForwarder_.CreatePipe() || !stderrForwarder_.CreatePipe()) {
            return false;
        }

        PosixSpawnResources spawnResources;
        if (!ConfigureSpawnResources(spawnResources, stdoutForwarder_, stderrForwarder_)) {
            return false;
        }

        std::vector<std::string> environment = BuildSpawnEnvironment(request_);
        std::vector<char*> argumentPointers = BuildCStringPointers(request_.arguments);
        std::vector<char*> environmentPointers = BuildCStringPointers(environment);
        startedAt_ = SteadyClock::now();
        const int spawnResult = posix_spawn(
            &processLeaderId_, request_.arguments.front().c_str(), &spawnResources.fileActions_,
            &spawnResources.attributes_, argumentPointers.data(), environmentPointers.data());
        // The parent must never retain pipe write ends; otherwise EOF would not
        // be observable after the child closes its redirected streams.
        stdoutForwarder_.CloseWriteEnd();
        stderrForwarder_.CloseWriteEnd();
        if (spawnResult != 0) {
            ASCENDLOGE(
                "Failed to spawn process: executable=%s error=%d message=%s", request_.arguments.front().c_str(),
                spawnResult, std::strerror(spawnResult));
            return false;
        }
        needsReaping_ = true;
        return true;
    }

    // Pumps output while polling child state until normal completion, timeout,
    // or an executor I/O/wait failure. Any unfinished child is then reaped.
    void Wait(ProcessExecutorResult& result) noexcept
    {
        const SteadyClock::time_point executionDeadline = startedAt_ + request_.executionTimeout;
        int waitStatus = 0;
        bool terminateRunningProcess = false;
        while (true) {
            if (!ForwardAvailableOutput()) {
                terminateRunningProcess = true;
                break;
            }
            const pid_t waitResult = WaitWithoutBlocking(waitStatus);
            if (waitResult == processLeaderId_) {
                needsReaping_ = false;
                // Capture bytes already queued between the last drain and the
                // observed process exit before closing the forwarding pipes.
                if (ForwardAvailableOutput()) {
                    SetResultFromWaitStatus(waitStatus, result);
                }
                break;
            }
            if (waitResult < 0) {
                const int waitError = errno;
                if (waitError == ECHILD) {
                    needsReaping_ = false;
                } else {
                    terminateRunningProcess = true;
                }
                ASCENDLOGE(
                    "Failed to wait for process: pid=%d errno=%d message=%s", static_cast<int>(processLeaderId_),
                    waitError, std::strerror(waitError));
                break;
            }
            if (SteadyClock::now() >= executionDeadline) {
                result.outcome = ProcessOutcome::TimedOut;
                ASCENDLOGE(
                    "Process exceeded its execution timeout: executable=%s timeout_ms=%lld",
                    request_.arguments.front().c_str(), static_cast<long long>(request_.executionTimeout.count()));
                terminateRunningProcess = true;
                break;
            }
            if (!WaitForOutput(executionDeadline)) {
                terminateRunningProcess = true;
                break;
            }
        }
        if (terminateRunningProcess) {
            TerminateAndReap();
        }
        if (result.outcome == ProcessOutcome::TimedOut) {
            (void)ForwardAvailableOutput();
        }
        stdoutForwarder_.Close();
        stderrForwarder_.Close();
        result.elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(SteadyClock::now() - startedAt_);
    }

private:
    pid_t WaitWithoutBlocking(int& waitStatus) noexcept
    {
        pid_t waitResult;
        do {
            waitResult = waitpid(processLeaderId_, &waitStatus, WNOHANG);
        } while (waitResult < 0 && errno == EINTR);
        return waitResult;
    }

    bool ForwardAvailableOutput() noexcept
    {
        const bool stdoutForwarded = !stdoutForwarder_.IsOpen() || stdoutForwarder_.ForwardAvailableOutput();
        const bool stderrForwarded = !stderrForwarder_.IsOpen() || stderrForwarder_.ForwardAvailableOutput();
        return stdoutForwarded && stderrForwarded;
    }

    bool WaitForOutput(const SteadyClock::time_point& executionDeadline) noexcept
    {
        const std::chrono::milliseconds remainingTime =
            std::chrono::duration_cast<std::chrono::milliseconds>(executionDeadline - SteadyClock::now());
        // Short bounded waits avoid busy-spinning while keeping timeout and
        // process-exit detection responsive even when no output is produced.
        const int timeoutMilliseconds = static_cast<int>(
            std::max<int64_t>(1, std::min<int64_t>(remainingTime.count(), MAX_PROCESS_WAIT_MILLISECONDS)));
        std::array<pollfd, 2U> outputDescriptors{{
            {stdoutForwarder_.GetReadFileDescriptor(), POLLIN, 0},
            {stderrForwarder_.GetReadFileDescriptor(), POLLIN, 0},
        }};
        const int pollResult = poll(outputDescriptors.data(), outputDescriptors.size(), timeoutMilliseconds);
        if (pollResult >= 0 || errno == EINTR) {
            return true;
        }
        const int pollError = errno;
        ASCENDLOGE("Failed to poll process output: errno=%d message=%s", pollError, std::strerror(pollError));
        return false;
    }

    // Requests graceful shutdown first, then force-kills the process group and
    // performs the blocking wait required to prevent a zombie child.
    void TerminateAndReap() noexcept
    {
        if (!needsReaping_) {
            return;
        }
        SignalProcessGroup(processLeaderId_, SIGTERM);
        std::this_thread::sleep_for(request_.terminationGracePeriod);
        SignalProcessGroup(processLeaderId_, SIGKILL);
        int ignoredWaitStatus = 0;
        pid_t waitResult;
        do {
            waitResult = waitpid(processLeaderId_, &ignoredWaitStatus, 0);
        } while (waitResult < 0 && errno == EINTR);
        if (waitResult < 0 && errno != ECHILD) {
            const int waitError = errno;
            ASCENDLOGW(
                "Failed to reap terminated process: pid=%d errno=%d message=%s", static_cast<int>(processLeaderId_),
                waitError, std::strerror(waitError));
        }
        needsReaping_ = false;
    }

    static void SetResultFromWaitStatus(int waitStatus, ProcessExecutorResult& result) noexcept
    {
        if (WIFEXITED(waitStatus)) {
            result.outcome = ProcessOutcome::Exited;
            result.terminationCode = static_cast<int32_t>(WEXITSTATUS(waitStatus));
        } else if (WIFSIGNALED(waitStatus)) {
            result.outcome = ProcessOutcome::Signaled;
            result.terminationCode = static_cast<int32_t>(WTERMSIG(waitStatus));
        }
    }

    const ProcessExecutorRequest& request_;
    std::ofstream outputLog_;
    ProcessOutputForwarder stdoutForwarder_;
    ProcessOutputForwarder stderrForwarder_;
    pid_t processLeaderId_{-1};
    SteadyClock::time_point startedAt_{};
    bool needsReaping_{false};
};

} // namespace

// Reports success only for a conventional zero exit status.
bool ProcessExecutorResult::HasSuccessfulExit() const noexcept
{
    return outcome == ProcessOutcome::Exited && terminationCode == 0;
}

// Returns a stable log-friendly name for the result category.
const char* ProcessExecutorResult::GetOutcomeName() const noexcept
{
    switch (outcome) {
        case ProcessOutcome::Exited:
            return "exited";
        case ProcessOutcome::Signaled:
            return "signaled";
        case ProcessOutcome::TimedOut:
            return "timed_out";
        case ProcessOutcome::ExecutorFailure:
            return "executor_failure";
        default:
            return "unknown";
    }
}

// Validates the request and executes it synchronously. Operational failures are
// represented by the default ExecutorFailure result; allocation failure remains
// exceptional so callers do not lose an out-of-memory condition.
ProcessExecutorResult ProcessExecutor::Execute(const ProcessExecutorRequest& request)
{
    ProcessExecutorResult result;
    if (!ValidateProcessExecutorRequest(request)) {
        return result;
    }
    try {
        ChildProcess childProcess(request);
        if (childProcess.Start()) {
            childProcess.Wait(result);
        }
    } catch (const std::bad_alloc&) {
        throw;
    } catch (const std::exception& exception) {
        ASCENDLOGE(
            "Process execution failed with exception: executable=%s message=%s", request.arguments.front().c_str(),
            exception.what());
    } catch (...) {
        ASCENDLOGE(
            "Process execution failed with an unknown exception: executable=%s", request.arguments.front().c_str());
    }
    return result;
}

} // namespace ascendc
