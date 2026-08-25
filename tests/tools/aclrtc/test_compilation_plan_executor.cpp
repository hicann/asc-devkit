/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "specialization/compilation_plan_executor.h"
#include "specialization/kernel_specialization_diagnostics.h"

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>

#include <chrono>
#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>
#include <fstream>
#include <poll.h>
#include <thread>
#include <sys/resource.h>
#include <sys/wait.h>
#include <time.h>
#include <type_traits>
#include <unistd.h>

namespace {
namespace fs = boost::filesystem;
using namespace std::chrono_literals;
using ascendc::aclrtc::CompilationCommand;
using ascendc::aclrtc::CompilationCommandExecutionLimits;
using ascendc::aclrtc::CompilationCommandKind;
using ascendc::aclrtc::CompilationPlanExecutor;
using ascendc::aclrtc::KernelCompilationPlan;
using ascendc::aclrtc::KernelCompilationVariant;
using ascendc::aclrtc::KernelSpecializationDiagnostics;
using namespace mockcpp;

int FailCommandOutputPoll(pollfd*, nfds_t, int)
{
    errno = EIO;
    return -1;
}

pid_t FailCommandWait(pid_t, int*, int)
{
    errno = ECHILD;
    return -1;
}

uint32_t COMMAND_OUTPUT_READ_CALL_COUNT = 0U;

ssize_t FailFinalCommandOutputRead(int, void*, size_t)
{
    ++COMMAND_OUTPUT_READ_CALL_COUNT;
    errno = COMMAND_OUTPUT_READ_CALL_COUNT == 3U ? EIO : EAGAIN;
    return -1;
}

pid_t ReapCommandProcess(pid_t processId, int* waitStatus, int)
{
    pid_t waitResult;
    do {
        waitResult = wait4(processId, waitStatus, 0, nullptr);
    } while (waitResult < 0 && errno == EINTR);
    return waitResult;
}

static_assert(
    std::is_same<std::underlying_type_t<KernelCompilationVariant>, uint32_t>::value,
    "KernelCompilationVariant must have a stable width");
static_assert(
    std::is_same<std::underlying_type_t<CompilationCommandKind>, uint32_t>::value,
    "CompilationCommandKind must have a stable width");
CompilationCommand CreateCommand(
    CompilationCommandKind commandKind, const char* executable, std::vector<std::string> arguments)
{
    return {commandKind, 0U, "test-command", executable, std::move(arguments)};
}

class ScopedDiagnosticCurrentDirectory final {
public:
    ScopedDiagnosticCurrentDirectory()
        : previousDirectoryPath_(fs::current_path()),
          temporaryDirectoryPath_(fs::temp_directory_path() / fs::unique_path("aclrtc_executor_diagnostics_%%%%-%%%%"))
    {
        fs::create_directories(temporaryDirectoryPath_);
        fs::current_path(temporaryDirectoryPath_);
    }

    ~ScopedDiagnosticCurrentDirectory()
    {
        boost::system::error_code ignoredError;
        fs::current_path(previousDirectoryPath_, ignoredError);
        fs::remove_all(temporaryDirectoryPath_, ignoredError);
    }

private:
    fs::path previousDirectoryPath_;
    fs::path temporaryDirectoryPath_;
};

std::string ReadTextFile(const fs::path& filePath)
{
    std::ifstream inputStream(filePath.string());
    return {std::istreambuf_iterator<char>(inputStream), std::istreambuf_iterator<char>()};
}

TEST(CompilationPlanExecutorTest, ProductionLimitsMatchReviewedPerCommandTimeouts)
{
    const CompilationCommandExecutionLimits limits = CompilationCommandExecutionLimits::ProductionDefaults();
    EXPECT_EQ(limits.compileCommandTimeout, 5min);
    EXPECT_EQ(limits.objectCopyAndLinkCommandTimeout, 1min);
    EXPECT_EQ(limits.terminationGracePeriod, 5s);
}

TEST(CompilationPlanExecutorTest, ExecutesCommandsWithoutShellParsing)
{
    boost::system::error_code ignoredError;
    fs::remove("/tmp/aclrtc_should_not_exist", ignoredError);
    KernelSpecializationDiagnostics diagnostics(fs::path(), "executor_success");
    CompilationPlanExecutor executor(diagnostics, CompilationCommandExecutionLimits::ProductionDefaults());
    KernelCompilationPlan plan;
    plan.compilationCommands.push_back(
        CreateCommand(CompilationCommandKind::Compile, "/bin/echo", {"$(touch /tmp/aclrtc_should_not_exist);literal"}));

    EXPECT_EQ(executor.ExecuteCompilationPlan(plan), 0);
    EXPECT_FALSE(fs::exists("/tmp/aclrtc_should_not_exist"));
}

TEST(CompilationPlanExecutorTest, RejectsEmbeddedNullBeforeSpawningProcess)
{
    KernelSpecializationDiagnostics diagnostics(fs::path(), "executor_embedded_null");
    CompilationPlanExecutor executor(diagnostics, CompilationCommandExecutionLimits::ProductionDefaults());

    std::string executableWithEmbeddedNull("/bin/true");
    executableWithEmbeddedNull.push_back('\0');
    executableWithEmbeddedNull += "/bin/false";
    KernelCompilationPlan executablePlan;
    CompilationCommand executableCommand = CreateCommand(CompilationCommandKind::Compile, "/bin/true", {});
    executableCommand.executablePath = fs::path(executableWithEmbeddedNull);
    executablePlan.compilationCommands.push_back(std::move(executableCommand));
    EXPECT_EQ(executor.ExecuteCompilationPlan(executablePlan), ascendc::aclrtc::ACLRTC_ERROR_FAILURE);

    std::string argumentWithEmbeddedNull("accepted-prefix");
    argumentWithEmbeddedNull.push_back('\0');
    argumentWithEmbeddedNull += "discarded-suffix";
    KernelCompilationPlan argumentPlan;
    argumentPlan.compilationCommands.push_back(
        CreateCommand(CompilationCommandKind::Compile, "/bin/true", {argumentWithEmbeddedNull}));
    EXPECT_EQ(executor.ExecuteCompilationPlan(argumentPlan), ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
}

TEST(CompilationPlanExecutorTest, MapsCompileAndLinkFailuresToDifferentErrors)
{
    KernelSpecializationDiagnostics diagnostics(fs::path(), "executor_failure");
    CompilationPlanExecutor executor(diagnostics, CompilationCommandExecutionLimits::ProductionDefaults());
    KernelCompilationPlan plan;
    plan.compilationCommands.push_back(CreateCommand(CompilationCommandKind::Compile, "/bin/false", {}));
    EXPECT_EQ(executor.ExecuteCompilationPlan(plan), ascendc::aclrtc::ACLRTC_ERROR_COMPILATION);

    plan.compilationCommands[0].commandKind = CompilationCommandKind::Link;
    EXPECT_EQ(executor.ExecuteCompilationPlan(plan), ascendc::aclrtc::ACLRTC_ERROR_LINKING);
}

TEST(CompilationPlanExecutorTest, UsesInjectedShortTimeoutInUnitTests)
{
    KernelSpecializationDiagnostics diagnostics(fs::path(), "executor_timeout");
    CompilationCommandExecutionLimits limits;
    limits.compileCommandTimeout = 20ms;
    limits.objectCopyAndLinkCommandTimeout = 20ms;
    limits.terminationGracePeriod = 20ms;
    CompilationPlanExecutor executor(diagnostics, limits);
    KernelCompilationPlan plan;
    plan.compilationCommands.push_back(CreateCommand(CompilationCommandKind::Compile, "/bin/sleep", {"10"}));

    const auto startedAt = std::chrono::steady_clock::now();
    EXPECT_EQ(executor.ExecuteCompilationPlan(plan), ascendc::aclrtc::ACLRTC_ERROR_COMPILATION);
    EXPECT_LT(std::chrono::steady_clock::now() - startedAt, 2s);
}

TEST(CompilationPlanExecutorTest, EnforcesTimeoutWhileCommandContinuouslyProducesOutput)
{
    KernelSpecializationDiagnostics diagnostics(fs::path(), "executor_continuous_output");
    CompilationCommandExecutionLimits limits{20ms, 20ms, 20ms};
    CompilationPlanExecutor executor(diagnostics, limits);
    KernelCompilationPlan plan;
    plan.compilationCommands.push_back(CreateCommand(
        CompilationCommandKind::Compile, "/bin/sh", {"-c", "yes & yes & yes & yes & yes & yes & yes & yes & wait"}));

    const auto startedAt = std::chrono::steady_clock::now();
    EXPECT_EQ(executor.ExecuteCompilationPlan(plan), ascendc::aclrtc::ACLRTC_ERROR_COMPILATION);
    EXPECT_LT(std::chrono::steady_clock::now() - startedAt, 2s);
}

TEST(CompilationPlanExecutorTest, WaitsWithoutBusyPollingAfterCommandOutputCloses)
{
    KernelSpecializationDiagnostics diagnostics(fs::path(), "executor_closed_output");
    CompilationCommandExecutionLimits limits{1s, 1s, 20ms};
    CompilationPlanExecutor executor(diagnostics, limits);
    KernelCompilationPlan plan;
    plan.compilationCommands.push_back(
        CreateCommand(CompilationCommandKind::Compile, "/bin/sh", {"-c", "exec >/dev/null 2>&1; sleep 0.25"}));

    timespec cpuStartedAt{};
    timespec cpuFinishedAt{};
    ASSERT_EQ(clock_gettime(CLOCK_THREAD_CPUTIME_ID, &cpuStartedAt), 0);
    EXPECT_EQ(executor.ExecuteCompilationPlan(plan), ascendc::aclrtc::ACLRTC_SUCCESS);
    ASSERT_EQ(clock_gettime(CLOCK_THREAD_CPUTIME_ID, &cpuFinishedAt), 0);
    const int64_t consumedCpuNanoseconds =
        (static_cast<int64_t>(cpuFinishedAt.tv_sec) - static_cast<int64_t>(cpuStartedAt.tv_sec)) * 1000000000LL +
        static_cast<int64_t>(cpuFinishedAt.tv_nsec) - static_cast<int64_t>(cpuStartedAt.tv_nsec);
    EXPECT_LT(consumedCpuNanoseconds, 100000000LL);
}

TEST(CompilationPlanExecutorTest, CompletesWhenSynchronousCommandLeaderExits)
{
    KernelSpecializationDiagnostics diagnostics(fs::path(), "executor_leader_exit");
    CompilationCommandExecutionLimits limits{500ms, 500ms, 20ms};
    CompilationPlanExecutor executor(diagnostics, limits);
    KernelCompilationPlan plan;
    plan.compilationCommands.push_back(
        CreateCommand(CompilationCommandKind::Compile, "/bin/sh", {"-c", "sleep 0.3 & exec /bin/true"}));

    const auto startedAt = std::chrono::steady_clock::now();
    EXPECT_EQ(executor.ExecuteCompilationPlan(plan), ascendc::aclrtc::ACLRTC_SUCCESS);
    EXPECT_LT(std::chrono::steady_clock::now() - startedAt, 200ms);
}

TEST(CompilationPlanExecutorTest, TimeoutTerminatesActiveProcessGroup)
{
    static uint64_t sequence = 0U;
    const fs::path descendantMarkerPath =
        fs::temp_directory_path() /
        ("aclrtc_descendant_marker_" + std::to_string(getpid()) + "_" + std::to_string(sequence++));
    KernelSpecializationDiagnostics diagnostics(fs::path(), "executor_process_group");
    CompilationCommandExecutionLimits limits{20ms, 20ms, 20ms};
    CompilationPlanExecutor executor(diagnostics, limits);
    KernelCompilationPlan plan;
    const std::string commandScript =
        "(trap '' TERM; while true; do printf x >> '" + descendantMarkerPath.string() + "'; sleep 0.01; done) & wait";
    plan.compilationCommands.push_back(
        CreateCommand(CompilationCommandKind::Compile, "/bin/sh", {"-c", commandScript}));

    ASSERT_EQ(executor.ExecuteCompilationPlan(plan), ascendc::aclrtc::ACLRTC_ERROR_COMPILATION);
    // An orphaned descendant can remain as a zombie until the container init reaps it, so do not use kill(pid, 0)
    // as the assertion. A heartbeat that stops growing after the timeout directly verifies that the descendant did
    // not continue.
    const uintmax_t markerSizeAfterTimeout =
        fs::exists(descendantMarkerPath) ? fs::file_size(descendantMarkerPath) : 0U;
    std::this_thread::sleep_for(100ms);
    const uintmax_t markerSizeAfterWait = fs::exists(descendantMarkerPath) ? fs::file_size(descendantMarkerPath) : 0U;
    EXPECT_EQ(markerSizeAfterWait, markerSizeAfterTimeout);
    boost::system::error_code ignoredError;
    fs::remove(descendantMarkerPath, ignoredError);
}

TEST(CompilationPlanExecutorTest, RecordsSpawnFailureAsExecutorFailure)
{
    ScopedDiagnosticCurrentDirectory diagnosticCurrentDirectory;
    const std::string specializationSessionId = "executor_spawn_failure";
    KernelSpecializationDiagnostics diagnostics(fs::current_path(), specializationSessionId);
    CompilationPlanExecutor executor(diagnostics, CompilationCommandExecutionLimits::ProductionDefaults());
    KernelCompilationPlan plan;
    plan.compilationCommands.push_back(CreateCommand(CompilationCommandKind::Compile, "/path/that/does/not/exist", {}));
    EXPECT_EQ(executor.ExecuteCompilationPlan(plan), ascendc::aclrtc::ACLRTC_ERROR_FAILURE);

    const std::string compilationLog = ReadTextFile(fs::current_path() / "aclrtc_compile.log");
    EXPECT_NE(compilationLog.find("outcome=executor_failure"), std::string::npos);
    EXPECT_EQ(compilationLog.find("termination=exited code=0"), std::string::npos);
}

TEST(CompilationPlanExecutorTest, MirrorsCompleteProcessOutputWhenDiagnosticsAreEnabled)
{
    ScopedDiagnosticCurrentDirectory diagnosticCurrentDirectory;
    const std::string specializationSessionId = "executor_output_limit";
    KernelSpecializationDiagnostics diagnostics(fs::current_path(), specializationSessionId);
    CompilationPlanExecutor executor(diagnostics, CompilationCommandExecutionLimits::ProductionDefaults());
    KernelCompilationPlan plan;
    plan.compilationCommands.push_back(
        CreateCommand(CompilationCommandKind::Compile, "/usr/bin/printf", {"0123456789abcdef"}));
    EXPECT_EQ(executor.ExecuteCompilationPlan(plan), ascendc::aclrtc::ACLRTC_SUCCESS);

    const std::string compilationLog = ReadTextFile(fs::current_path() / "aclrtc_compile.log");
    EXPECT_NE(compilationLog.find("[test-command] output:\n"), std::string::npos);
    EXPECT_NE(compilationLog.find("0123456789abcdef"), std::string::npos);
    EXPECT_NE(compilationLog.find("\n[test-command] outcome=exited code=0"), std::string::npos);
}

TEST(CompilationPlanExecutorTest, ReportsToolProcessTerminatedBySignal)
{
    KernelSpecializationDiagnostics diagnostics(fs::path(), "executor_signaled");
    CompilationPlanExecutor executor(diagnostics, CompilationCommandExecutionLimits::ProductionDefaults());
    KernelCompilationPlan plan;
    plan.compilationCommands.push_back(
        CreateCommand(CompilationCommandKind::Compile, "/bin/sh", {"-c", "kill -TERM $$"}));

    EXPECT_EQ(executor.ExecuteCompilationPlan(plan), ascendc::aclrtc::ACLRTC_ERROR_COMPILATION);
}

TEST(CompilationPlanExecutorTest, ReportsPipeCreationFailureWhenFileDescriptorsAreExhausted)
{
    KernelSpecializationDiagnostics diagnostics(fs::path(), "executor_pipe_failure");
    CompilationPlanExecutor executor(diagnostics, CompilationCommandExecutionLimits::ProductionDefaults());
    KernelCompilationPlan plan;
    plan.compilationCommands.push_back(CreateCommand(CompilationCommandKind::Compile, "/bin/true", {}));

    struct rlimit originalFileDescriptorLimit {};
    ASSERT_EQ(getrlimit(RLIMIT_NOFILE, &originalFileDescriptorLimit), 0);
    struct rlimit exhaustedFileDescriptorLimit = originalFileDescriptorLimit;
    exhaustedFileDescriptorLimit.rlim_cur = 0U;
    ASSERT_EQ(setrlimit(RLIMIT_NOFILE, &exhaustedFileDescriptorLimit), 0);
    const aclError planExecutionStatus = executor.ExecuteCompilationPlan(plan);
    const int restoreLimitResult = setrlimit(RLIMIT_NOFILE, &originalFileDescriptorLimit);

    ASSERT_EQ(restoreLimitResult, 0);
    EXPECT_EQ(planExecutionStatus, ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
}

TEST(CompilationPlanExecutorTest, ReportsCommandOutputPollFailure)
{
    KernelSpecializationDiagnostics diagnostics(fs::path(), "executor_poll_failure");
    CompilationCommandExecutionLimits limits{1s, 1s, 20ms};
    CompilationPlanExecutor executor(diagnostics, limits);
    KernelCompilationPlan plan;
    plan.compilationCommands.push_back(CreateCommand(CompilationCommandKind::Compile, "/bin/sleep", {"1"}));
    MOCKER(poll, int (*)(pollfd*, nfds_t, int)).expects(once()).will(invoke(FailCommandOutputPoll));

    const aclError planExecutionStatus = executor.ExecuteCompilationPlan(plan);

    GlobalMockObject::verify();
    EXPECT_EQ(planExecutionStatus, ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
}

TEST(CompilationPlanExecutorTest, ReportsCommandWaitFailure)
{
    KernelSpecializationDiagnostics diagnostics(fs::path(), "executor_waitid_failure");
    CompilationCommandExecutionLimits limits{1s, 1s, 20ms};
    CompilationPlanExecutor executor(diagnostics, limits);
    KernelCompilationPlan plan;
    plan.compilationCommands.push_back(CreateCommand(CompilationCommandKind::Compile, "/bin/sleep", {"1"}));
    MOCKER(waitpid, pid_t(*)(pid_t, int*, int)).expects(once()).will(invoke(FailCommandWait));

    const aclError planExecutionStatus = executor.ExecuteCompilationPlan(plan);

    GlobalMockObject::verify();
    EXPECT_EQ(planExecutionStatus, ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
}

TEST(CompilationPlanExecutorTest, ReportsFinalCommandOutputReadFailure)
{
    KernelSpecializationDiagnostics diagnostics(fs::path(), "executor_final_output_read_failure");
    CompilationPlanExecutor executor(diagnostics, CompilationCommandExecutionLimits::ProductionDefaults());
    KernelCompilationPlan plan;
    plan.compilationCommands.push_back(CreateCommand(CompilationCommandKind::Compile, "/bin/true", {}));
    COMMAND_OUTPUT_READ_CALL_COUNT = 0U;
    MOCKER(read, ssize_t(*)(int, void*, size_t)).expects(exactly(4)).will(invoke(FailFinalCommandOutputRead));
    MOCKER(waitpid, pid_t(*)(pid_t, int*, int)).expects(once()).will(invoke(ReapCommandProcess));

    const aclError planExecutionStatus = executor.ExecuteCompilationPlan(plan);

    GlobalMockObject::verify();
    EXPECT_EQ(planExecutionStatus, ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
}

} // namespace
