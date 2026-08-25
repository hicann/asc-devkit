/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the LICENSE.
 */

#include <gtest/gtest.h>

#include "process_executor.h"

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <fstream>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>
#include <unistd.h>
#include <vector>

namespace ascendc {
namespace {
namespace fs = boost::filesystem;
using namespace std::chrono_literals;

static_assert(
    std::is_same<std::underlying_type_t<ProcessOutcome>, uint32_t>::value, "ProcessOutcome must have a stable width");
static_assert(
    std::is_same<decltype(ProcessExecutorResult::terminationCode), int32_t>::value,
    "terminationCode must have a stable width");

ProcessExecutorRequest CreateProcessRequest(std::vector<std::string> arguments)
{
    ProcessExecutorRequest request;
    request.arguments = std::move(arguments);
    request.executionTimeout = 1s;
    request.terminationGracePeriod = 20ms;
    return request;
}

std::string ReadTextFile(const fs::path& filePath)
{
    std::ifstream inputStream(filePath.string());
    return {std::istreambuf_iterator<char>(inputStream), std::istreambuf_iterator<char>()};
}

class TemporarilyClosedStandardOutputStreams final {
public:
    ~TemporarilyClosedStandardOutputStreams() { (void)Restore(); }

    bool Close()
    {
        savedStandardOutput_ = dup(STDOUT_FILENO);
        savedStandardError_ = dup(STDERR_FILENO);
        if (savedStandardOutput_ < 0 || savedStandardError_ < 0) {
            return false;
        }
        const bool standardOutputClosed = close(STDOUT_FILENO) == 0;
        const bool standardErrorClosed = close(STDERR_FILENO) == 0;
        return standardOutputClosed && standardErrorClosed;
    }

    bool Restore()
    {
        bool restored = true;
        if (savedStandardOutput_ >= 0) {
            restored = dup2(savedStandardOutput_, STDOUT_FILENO) == STDOUT_FILENO && restored;
            restored = close(savedStandardOutput_) == 0 && restored;
            savedStandardOutput_ = -1;
        }
        if (savedStandardError_ >= 0) {
            restored = dup2(savedStandardError_, STDERR_FILENO) == STDERR_FILENO && restored;
            restored = close(savedStandardError_) == 0 && restored;
            savedStandardError_ = -1;
        }
        return restored;
    }

private:
    int savedStandardOutput_{-1};
    int savedStandardError_{-1};
};

TEST(ProcessExecutorTest, RejectsInvalidRequestsBeforeSpawning)
{
    ProcessExecutorRequest emptyArgumentsRequest;
    emptyArgumentsRequest.executionTimeout = 1s;
    EXPECT_EQ(ProcessExecutor::Execute(emptyArgumentsRequest).outcome, ProcessOutcome::ExecutorFailure);

    ProcessExecutorRequest zeroTimeoutRequest = CreateProcessRequest({"/bin/true"});
    zeroTimeoutRequest.executionTimeout = 0ms;
    EXPECT_EQ(ProcessExecutor::Execute(zeroTimeoutRequest).outcome, ProcessOutcome::ExecutorFailure);

    ProcessExecutorRequest negativeGracePeriodRequest = CreateProcessRequest({"/bin/true"});
    negativeGracePeriodRequest.terminationGracePeriod = -1ms;
    EXPECT_EQ(ProcessExecutor::Execute(negativeGracePeriodRequest).outcome, ProcessOutcome::ExecutorFailure);

    ProcessExecutorRequest embeddedNullRequest = CreateProcessRequest({std::string("/bin/true\0invalid", 17U)});
    EXPECT_EQ(ProcessExecutor::Execute(embeddedNullRequest).outcome, ProcessOutcome::ExecutorFailure);

    ProcessExecutorRequest invalidOverrideNameRequest = CreateProcessRequest({"/bin/true"});
    invalidOverrideNameRequest.environmentOverrides.emplace_back("INVALID=NAME", "value");
    EXPECT_EQ(ProcessExecutor::Execute(invalidOverrideNameRequest).outcome, ProcessOutcome::ExecutorFailure);

    ProcessExecutorRequest embeddedNullOverrideValueRequest = CreateProcessRequest({"/bin/true"});
    embeddedNullOverrideValueRequest.environmentOverrides.emplace_back("VALID_NAME", std::string("value\0suffix", 12U));
    EXPECT_EQ(ProcessExecutor::Execute(embeddedNullOverrideValueRequest).outcome, ProcessOutcome::ExecutorFailure);

    ProcessExecutorRequest invalidRemovalNameRequest = CreateProcessRequest({"/bin/true"});
    invalidRemovalNameRequest.removedEnvironmentVariables.emplace_back("INVALID=NAME");
    EXPECT_EQ(ProcessExecutor::Execute(invalidRemovalNameRequest).outcome, ProcessOutcome::ExecutorFailure);
}

TEST(ProcessExecutorTest, ExecutesArgumentsWithoutShellParsing)
{
    const fs::path unexpectedFilePath =
        fs::temp_directory_path() / ("process_executor_shell_" + std::to_string(getpid()));
    boost::system::error_code ignoredError;
    fs::remove(unexpectedFilePath, ignoredError);

    const fs::path outputLogPath = fs::temp_directory_path() / fs::unique_path("process_executor_output_%%%%-%%%%");
    ProcessExecutorRequest request =
        CreateProcessRequest({"/usr/bin/printf", "$(touch " + unexpectedFilePath.string() + ")"});
    request.mirroredOutputLogFilePath = outputLogPath.string();
    const ProcessExecutorResult result = ProcessExecutor::Execute(request);

    EXPECT_TRUE(result.HasSuccessfulExit());
    EXPECT_EQ(ReadTextFile(outputLogPath), "$(touch " + unexpectedFilePath.string() + ")");
    EXPECT_FALSE(fs::exists(unexpectedFilePath));
    fs::remove(outputLogPath, ignoredError);
}

TEST(ProcessExecutorTest, AppliesEnvironmentOverridesAndRemovals)
{
    ASSERT_EQ(setenv("PROCESS_EXECUTOR_REMOVE_ME", "inherited", 1), 0);
    const fs::path outputLogPath = fs::temp_directory_path() / fs::unique_path("process_executor_env_%%%%-%%%%");
    ProcessExecutorRequest request = CreateProcessRequest(
        {"/bin/sh", "-c", "printf '%s|%s' \"$PROCESS_EXECUTOR_VALUE\" \"${PROCESS_EXECUTOR_REMOVE_ME-unset}\""});
    request.environmentOverrides.emplace_back("PROCESS_EXECUTOR_VALUE", "visible");
    request.removedEnvironmentVariables.emplace_back("PROCESS_EXECUTOR_REMOVE_ME");
    request.mirroredOutputLogFilePath = outputLogPath.string();

    const ProcessExecutorResult result = ProcessExecutor::Execute(request);
    EXPECT_TRUE(result.HasSuccessfulExit());
    EXPECT_EQ(ReadTextFile(outputLogPath), "visible|unset");
    EXPECT_EQ(unsetenv("PROCESS_EXECUTOR_REMOVE_ME"), 0);
    boost::system::error_code ignoredError;
    fs::remove(outputLogPath, ignoredError);
}

TEST(ProcessExecutorTest, EnvironmentOverrideTakesPrecedenceOverRemoval)
{
    const fs::path outputLogPath = fs::temp_directory_path() / fs::unique_path("process_executor_precedence_%%%%-%%%%");
    ProcessExecutorRequest request =
        CreateProcessRequest({"/bin/sh", "-c", "printf '%s' \"$PROCESS_EXECUTOR_PRECEDENCE\""});
    request.removedEnvironmentVariables.emplace_back("PROCESS_EXECUTOR_PRECEDENCE");
    request.environmentOverrides.emplace_back("PROCESS_EXECUTOR_PRECEDENCE", "overridden");
    request.mirroredOutputLogFilePath = outputLogPath.string();

    const ProcessExecutorResult result = ProcessExecutor::Execute(request);
    EXPECT_TRUE(result.HasSuccessfulExit());
    EXPECT_EQ(ReadTextFile(outputLogPath), "overridden");
    boost::system::error_code ignoredError;
    fs::remove(outputLogPath, ignoredError);
}

TEST(ProcessExecutorTest, ReportsExitSignalAndSpawnFailureSeparately)
{
    const ProcessExecutorResult nonzeroExit =
        ProcessExecutor::Execute(CreateProcessRequest({"/bin/sh", "-c", "exit 7"}));
    EXPECT_EQ(nonzeroExit.outcome, ProcessOutcome::Exited);
    EXPECT_EQ(nonzeroExit.terminationCode, 7);

    const ProcessExecutorResult signalExit =
        ProcessExecutor::Execute(CreateProcessRequest({"/bin/sh", "-c", "kill -TERM $$"}));
    EXPECT_EQ(signalExit.outcome, ProcessOutcome::Signaled);
    EXPECT_EQ(signalExit.terminationCode, SIGTERM);

    const ProcessExecutorResult spawnFailure =
        ProcessExecutor::Execute(CreateProcessRequest({"/path/that/does/not/exist"}));
    EXPECT_EQ(spawnFailure.outcome, ProcessOutcome::ExecutorFailure);
}

TEST(ProcessExecutorTest, RedirectsChildOutputWhenParentStandardOutputStreamsAreClosed)
{
    const fs::path outputLogPath =
        fs::temp_directory_path() / fs::unique_path("process_executor_closed_stdio_%%%%-%%%%");
    ProcessExecutorRequest request = CreateProcessRequest({"/bin/sh", "-c", "printf stdout; printf stderr >&2"});
    request.mirroredOutputLogFilePath = outputLogPath.string();
    TemporarilyClosedStandardOutputStreams standardOutputStreams;
    const bool streamsClosed = standardOutputStreams.Close();
    ProcessExecutorResult result;
    if (streamsClosed) {
        result = ProcessExecutor::Execute(request);
    }
    const bool streamsRestored = standardOutputStreams.Restore();

    ASSERT_TRUE(streamsClosed);
    ASSERT_TRUE(streamsRestored);
    EXPECT_TRUE(result.HasSuccessfulExit());
    const std::string mirroredOutput = ReadTextFile(outputLogPath);
    EXPECT_NE(mirroredOutput.find("stdout"), std::string::npos);
    EXPECT_NE(mirroredOutput.find("stderr"), std::string::npos);
    boost::system::error_code ignoredError;
    fs::remove(outputLogPath, ignoredError);
}

TEST(ProcessExecutorTest, MirrorsStandardOutputAndStandardErrorToOneLogFile)
{
    const fs::path outputLogPath = fs::temp_directory_path() / fs::unique_path("process_executor_streams_%%%%-%%%%");
    ProcessExecutorRequest request =
        CreateProcessRequest({"/bin/sh", "-c", "printf 'stdout-line\\n'; printf 'stderr-line\\n' >&2"});
    request.mirroredOutputLogFilePath = outputLogPath.string();

    const ProcessExecutorResult result = ProcessExecutor::Execute(request);
    EXPECT_TRUE(result.HasSuccessfulExit());
    const std::string mirroredOutput = ReadTextFile(outputLogPath);
    EXPECT_NE(mirroredOutput.find("stdout-line\n"), std::string::npos);
    EXPECT_NE(mirroredOutput.find("stderr-line\n"), std::string::npos);
    boost::system::error_code ignoredError;
    fs::remove(outputLogPath, ignoredError);
}

TEST(ProcessExecutorTest, DrainsProcessOutputLargerThanPipeCapacity)
{
    constexpr uintmax_t outputByteCount = 200000U;
    const fs::path outputLogPath = fs::temp_directory_path() / fs::unique_path("process_executor_large_%%%%-%%%%");
    ProcessExecutorRequest request = CreateProcessRequest({"/bin/sh", "-c", "head -c 200000 /dev/zero | tr '\\0' x"});
    request.mirroredOutputLogFilePath = outputLogPath.string();

    EXPECT_TRUE(ProcessExecutor::Execute(request).HasSuccessfulExit());
    EXPECT_EQ(fs::file_size(outputLogPath), outputByteCount);
    boost::system::error_code ignoredError;
    fs::remove(outputLogPath, ignoredError);
}

TEST(ProcessExecutorTest, DiagnosticLogFailureDoesNotFailCommand)
{
    ProcessExecutorRequest request = CreateProcessRequest({"/usr/bin/printf", "still-runs"});
    request.mirroredOutputLogFilePath = fs::temp_directory_path().string();

    EXPECT_TRUE(ProcessExecutor::Execute(request).HasSuccessfulExit());
}

TEST(ProcessExecutorTest, RejectsIncompatibleSigchldDispositionBeforeSpawning)
{
    const fs::path spawnMarkerPath = fs::temp_directory_path() / fs::unique_path("process_executor_sigchld_%%%%-%%%%");
    struct sigaction ignoredSigchld {};
    struct sigaction originalSigchld {};
    ignoredSigchld.sa_handler = SIG_IGN;
    ASSERT_EQ(sigemptyset(&ignoredSigchld.sa_mask), 0);
    ASSERT_EQ(sigaction(SIGCHLD, &ignoredSigchld, &originalSigchld), 0);

    const ProcessExecutorResult result =
        ProcessExecutor::Execute(CreateProcessRequest({"/usr/bin/touch", spawnMarkerPath.string()}));

    ASSERT_EQ(sigaction(SIGCHLD, &originalSigchld, nullptr), 0);
    EXPECT_EQ(result.outcome, ProcessOutcome::ExecutorFailure);
    EXPECT_FALSE(fs::exists(spawnMarkerPath));
    boost::system::error_code ignoredError;
    fs::remove(spawnMarkerPath, ignoredError);
}

TEST(ProcessExecutorTest, CompletesWhenSynchronousCommandLeaderExits)
{
    ProcessExecutorRequest request = CreateProcessRequest({"/bin/sh", "-c", "sleep 0.3 & exec /bin/true"});

    const auto startedAt = std::chrono::steady_clock::now();
    const ProcessExecutorResult result = ProcessExecutor::Execute(request);
    EXPECT_TRUE(result.HasSuccessfulExit());
    EXPECT_LT(std::chrono::steady_clock::now() - startedAt, 200ms);
}

TEST(ProcessExecutorTest, TimeoutTerminatesActiveProcessGroup)
{
    static uint64_t sequence = 0U;
    const fs::path descendantMarkerPath =
        fs::temp_directory_path() /
        ("process_executor_descendant_" + std::to_string(getpid()) + "_" + std::to_string(sequence++));
    const std::string commandScript =
        "(trap '' TERM; while true; do printf x >> '" + descendantMarkerPath.string() + "'; sleep 0.01; done) & wait";
    ProcessExecutorRequest request = CreateProcessRequest({"/bin/sh", "-c", commandScript});
    request.executionTimeout = 20ms;

    const ProcessExecutorResult result = ProcessExecutor::Execute(request);
    EXPECT_EQ(result.outcome, ProcessOutcome::TimedOut);

    const uintmax_t markerSizeAfterTimeout =
        fs::exists(descendantMarkerPath) ? fs::file_size(descendantMarkerPath) : 0U;
    std::this_thread::sleep_for(100ms);
    const uintmax_t markerSizeAfterWait = fs::exists(descendantMarkerPath) ? fs::file_size(descendantMarkerPath) : 0U;
    EXPECT_EQ(markerSizeAfterWait, markerSizeAfterTimeout);
    boost::system::error_code ignoredError;
    fs::remove(descendantMarkerPath, ignoredError);
}

} // namespace
} // namespace ascendc
