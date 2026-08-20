/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "specialization/kernel_specialization_diagnostics.h"

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include <nlohmann/json.hpp>

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>
#include <cerrno>
#include <csignal>
#include <cstdint>
#include <fcntl.h>
#include <fstream>
#include <string>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

namespace {
namespace fs = boost::filesystem;
using ascendc::aclrtc::CompilationCommand;
using ascendc::aclrtc::CompilationCommandKind;
using ascendc::aclrtc::CompilationProcessResult;
using ascendc::aclrtc::KernelSpecializationDiagnostics;
using ascendc::aclrtc::OutputPublicationStatus;
using namespace mockcpp;

int RejectReplayScriptPermissionChange(const char*, mode_t)
{
    errno = EPERM;
    return -1;
}

class ScopedCurrentDirectory final {
public:
    explicit ScopedCurrentDirectory(const fs::path& currentDirectory) : previousDirectory_(fs::current_path())
    {
        fs::current_path(currentDirectory);
    }

    ~ScopedCurrentDirectory()
    {
        boost::system::error_code ignoredError;
        fs::current_path(previousDirectory_, ignoredError);
    }

private:
    fs::path previousDirectory_;
};

std::string ReadTextFile(const fs::path& filePath)
{
    std::ifstream inputStream(filePath.string());
    return std::string(std::istreambuf_iterator<char>(inputStream), std::istreambuf_iterator<char>());
}

int ExecuteReplayScriptAndWait(const fs::path& replayScriptPath)
{
    const pid_t childProcessId = fork();
    if (childProcessId == 0) {
        execl(replayScriptPath.c_str(), replayScriptPath.c_str(), static_cast<char*>(nullptr));
        _exit(127);
    }
    if (childProcessId < 0) {
        return -1;
    }

    int childStatus = 0;
    pid_t waitResult;
    do {
        waitResult = waitpid(childProcessId, &childStatus, 0);
    } while (waitResult < 0 && errno == EINTR);
    if (waitResult != childProcessId || !WIFEXITED(childStatus)) {
        return -1;
    }
    return WEXITSTATUS(childStatus);
}

TEST(KernelSpecializationDiagnosticsTest, SavesOnlyReviewedDiagnosticFiles)
{
    static uint64_t sequence = 0U;
    const fs::path testRoot = fs::temp_directory_path() / ("aclrtc_diagnostics_test_" + std::to_string(getpid()) + "_" +
                                                           std::to_string(sequence++));
    fs::create_directories(testRoot);
    {
        ScopedCurrentDirectory currentDirectory(testRoot);
        KernelSpecializationDiagnostics diagnostics(testRoot, "session_1");
        diagnostics.WriteManifestSnapshot(nlohmann::json{{"schema_version", "1.0"}});

        const CompilationCommand command{
            CompilationCommandKind::Compile,
            0U,
            "basic/compile[0]",
            "/bin/echo",
            {"argument with space", "quote'value"}};
        diagnostics.AppendCommandToReplayScript(command);
        const CompilationCommand workingDirectoryCheck{
            CompilationCommandKind::Compile,
            std::nullopt,
            "basic/compile[1]",
            "/bin/sh",
            {"-c", "test \"$PWD\" = \"$1\"", "replay-working-directory-check", testRoot.string()}};
        diagnostics.AppendCommandToReplayScript(workingDirectoryCheck);
        CompilationProcessResult processResult;
        processResult.capturedOutput = "compiler output\n";
        diagnostics.AppendCommandResultToCompilationLog(command, processResult);
        diagnostics.WriteSpecializationResult("/tmp/kernel.elf", OutputPublicationStatus::Published);
    }

    const fs::path replayScriptPath = testRoot / "aclrtc_replay.sh";
    EXPECT_TRUE(fs::is_regular_file(testRoot / "aclrtc_manifest.json"));
    EXPECT_TRUE(fs::is_regular_file(replayScriptPath));
    EXPECT_TRUE(fs::is_regular_file(testRoot / "aclrtc_compile.log"));
    EXPECT_TRUE(fs::is_regular_file(testRoot / "aclrtc_result.json"));
    EXPECT_EQ(std::distance(fs::directory_iterator(testRoot), fs::directory_iterator()), 4);

    const std::string replayScript = ReadTextFile(replayScriptPath);
    EXPECT_NE(replayScript.find("'/bin/echo' 'argument with space' 'quote'\"'\"'value'"), std::string::npos);

    struct stat replayScriptStatus {};
    ASSERT_EQ(stat(replayScriptPath.c_str(), &replayScriptStatus), 0);
    EXPECT_EQ(replayScriptStatus.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO), S_IRWXU);
    EXPECT_EQ(ExecuteReplayScriptAndWait(replayScriptPath), 0);

    const nlohmann::json result = nlohmann::json::parse(ReadTextFile(testRoot / "aclrtc_result.json"));
    EXPECT_EQ(result.at("specialization_session_id"), "session_1");
    EXPECT_EQ(result.at("output_path"), "/tmp/kernel.elf");
    EXPECT_TRUE(result.at("output_published"));
    EXPECT_FALSE(result.contains("result"));

    boost::system::error_code ignoredError;
    fs::remove_all(testRoot, ignoredError);
}

TEST(KernelSpecializationDiagnosticsTest, DisablesDiagnosticsWhenReplayScriptCannotBeCreated)
{
    const fs::path missingWorktreePath =
        fs::temp_directory_path() / fs::unique_path("aclrtc_missing_diagnostics_%%%%-%%%%") / "worktree";
    KernelSpecializationDiagnostics diagnostics(missingWorktreePath, "disabled_session");
    diagnostics.WriteManifestSnapshot(nlohmann::json{{"schema_version", "1.0"}});

    const CompilationCommand command{CompilationCommandKind::Compile, std::nullopt, "compile", "/bin/true", {}};
    diagnostics.AppendCommandToReplayScript(command);
    diagnostics.AppendCommandResultToCompilationLog(command, CompilationProcessResult{});
    diagnostics.LogCommandFailureRecoveryHint();
    diagnostics.WriteSpecializationResult("/tmp/kernel.elf", OutputPublicationStatus::NotPublished);

    EXPECT_FALSE(fs::exists(missingWorktreePath));
}

TEST(KernelSpecializationDiagnosticsTest, RemovesReplayScriptWhenInitialWriteCannotBeFinalized)
{
    const fs::path testRoot = fs::temp_directory_path() / fs::unique_path("aclrtc_diagnostics_write_failure_%%%%-%%%%");
    fs::create_directories(testRoot);
    struct rlimit originalFileSizeLimit {};
    ASSERT_EQ(getrlimit(RLIMIT_FSIZE, &originalFileSizeLimit), 0);
    struct sigaction ignoreFileSizeSignal {};
    struct sigaction originalFileSizeSignal {};
    ignoreFileSizeSignal.sa_handler = SIG_IGN;
    ASSERT_EQ(sigemptyset(&ignoreFileSizeSignal.sa_mask), 0);
    ASSERT_EQ(sigaction(SIGXFSZ, &ignoreFileSizeSignal, &originalFileSizeSignal), 0);
    struct rlimit zeroFileSizeLimit = originalFileSizeLimit;
    zeroFileSizeLimit.rlim_cur = 0U;
    ASSERT_EQ(setrlimit(RLIMIT_FSIZE, &zeroFileSizeLimit), 0);

    KernelSpecializationDiagnostics diagnostics(testRoot, "write_failure_session");

    const int restoreLimitResult = setrlimit(RLIMIT_FSIZE, &originalFileSizeLimit);
    const int restoreSignalResult = sigaction(SIGXFSZ, &originalFileSizeSignal, nullptr);
    ASSERT_EQ(restoreLimitResult, 0);
    ASSERT_EQ(restoreSignalResult, 0);
    EXPECT_FALSE(fs::exists(testRoot / "aclrtc_replay.sh"));

    boost::system::error_code ignoredError;
    fs::remove_all(testRoot, ignoredError);
}

TEST(KernelSpecializationDiagnosticsTest, KeepsNonEmptyReplayPathWhenInitialOpenFails)
{
    const fs::path testRoot =
        fs::temp_directory_path() / fs::unique_path("aclrtc_diagnostics_replay_directory_%%%%-%%%%");
    const fs::path replayScriptDirectory = testRoot / "aclrtc_replay.sh";
    fs::create_directories(replayScriptDirectory);
    std::ofstream(replayScriptDirectory / "existing.log") << "keep this diagnostic evidence\n";

    KernelSpecializationDiagnostics diagnostics(testRoot, "replay_directory_session");
    EXPECT_TRUE(fs::is_directory(replayScriptDirectory));
    EXPECT_TRUE(fs::is_regular_file(replayScriptDirectory / "existing.log"));

    boost::system::error_code ignoredError;
    fs::remove_all(testRoot, ignoredError);
}

TEST(KernelSpecializationDiagnosticsTest, DisablesDiagnosticsWhenReplayScriptCannotBeMadeExecutable)
{
    const fs::path testRoot = fs::temp_directory_path() / fs::unique_path("aclrtc_diagnostics_chmod_failure_%%%%-%%%%");
    fs::create_directories(testRoot);
    MOCKER(chmod, int (*)(const char*, mode_t)).expects(once()).will(invoke(RejectReplayScriptPermissionChange));

    KernelSpecializationDiagnostics diagnostics(testRoot, "chmod_failure_session");

    GlobalMockObject::verify();
    EXPECT_FALSE(fs::exists(testRoot / "aclrtc_replay.sh"));
    boost::system::error_code ignoredError;
    fs::remove_all(testRoot, ignoredError);
}

TEST(KernelSpecializationDiagnosticsTest, HandlesMissingCurrentDirectoryDuringInitialization)
{
    const int originalDirectoryFileDescriptor = open(".", O_RDONLY | O_DIRECTORY | O_CLOEXEC);
    ASSERT_GE(originalDirectoryFileDescriptor, 0);
    const fs::path removedCurrentDirectory =
        fs::temp_directory_path() / fs::unique_path("aclrtc_diagnostics_removed_cwd_%%%%-%%%%");
    fs::create_directories(removedCurrentDirectory);
    ASSERT_EQ(chdir(removedCurrentDirectory.c_str()), 0);
    ASSERT_EQ(rmdir(removedCurrentDirectory.c_str()), 0);

    KernelSpecializationDiagnostics diagnostics(removedCurrentDirectory, "removed_cwd_session");

    const int restoreDirectoryResult = fchdir(originalDirectoryFileDescriptor);
    const int closeDirectoryResult = close(originalDirectoryFileDescriptor);
    ASSERT_EQ(restoreDirectoryResult, 0);
    ASSERT_EQ(closeDirectoryResult, 0);
    EXPECT_FALSE(fs::exists(removedCurrentDirectory));
}

TEST(KernelSpecializationDiagnosticsTest, IgnoresManifestSerializationFailure)
{
    const fs::path testRoot =
        fs::temp_directory_path() / fs::unique_path("aclrtc_diagnostics_invalid_manifest_%%%%-%%%%");
    fs::create_directories(testRoot);
    KernelSpecializationDiagnostics diagnostics(testRoot, "invalid_manifest_session");
    const nlohmann::json invalidUtf8Manifest = {{"invalid_utf8", std::string(1U, static_cast<char>(0xff))}};

    diagnostics.WriteManifestSnapshot(invalidUtf8Manifest);

    EXPECT_FALSE(fs::exists(testRoot / "aclrtc_manifest.json"));
    boost::system::error_code ignoredError;
    fs::remove_all(testRoot, ignoredError);
}

TEST(KernelSpecializationDiagnosticsTest, RecordsTerminationKindsAndUnpublishedResult)
{
    const fs::path testRoot = fs::temp_directory_path() / fs::unique_path("aclrtc_diagnostics_states_%%%%-%%%%");
    fs::create_directories(testRoot);
    KernelSpecializationDiagnostics diagnostics(testRoot, "session_states");
    const CompilationCommand command{CompilationCommandKind::Compile, std::nullopt, "compile", "/bin/true", {}};

    CompilationProcessResult processResult;
    processResult.termination = ascendc::aclrtc::CompilationProcessTermination::Signaled;
    processResult.terminationCode = SIGTERM;
    diagnostics.AppendCommandResultToCompilationLog(command, processResult);
    processResult.termination = ascendc::aclrtc::CompilationProcessTermination::TimedOut;
    diagnostics.AppendCommandResultToCompilationLog(command, processResult);
    processResult.termination = static_cast<ascendc::aclrtc::CompilationProcessTermination>(UINT32_MAX);
    diagnostics.AppendCommandResultToCompilationLog(command, processResult);
    diagnostics.LogCommandFailureRecoveryHint();
    diagnostics.WriteSpecializationResult("/tmp/not_published.elf", OutputPublicationStatus::NotPublished);

    const std::string compilationLog = ReadTextFile(testRoot / "aclrtc_compile.log");
    EXPECT_NE(compilationLog.find("termination=signaled"), std::string::npos);
    EXPECT_NE(compilationLog.find("termination=timed_out"), std::string::npos);
    EXPECT_NE(compilationLog.find("termination=unknown"), std::string::npos);
    const nlohmann::json result = nlohmann::json::parse(ReadTextFile(testRoot / "aclrtc_result.json"));
    EXPECT_FALSE(result.at("output_published"));

    boost::system::error_code ignoredError;
    fs::remove_all(testRoot, ignoredError);
}

TEST(KernelSpecializationDiagnosticsTest, IgnoresDiagnosticWriteFailuresAfterWorktreeRemoval)
{
    const fs::path testRoot = fs::temp_directory_path() / fs::unique_path("aclrtc_diagnostics_removed_%%%%-%%%%");
    fs::create_directories(testRoot);
    KernelSpecializationDiagnostics diagnostics(testRoot, "removed_session");
    boost::system::error_code removeError;
    fs::remove_all(testRoot, removeError);
    ASSERT_FALSE(removeError);

    const CompilationCommand command{
        CompilationCommandKind::Compile, std::nullopt, "compile", "/bin/true", {"argument"}};
    diagnostics.WriteManifestSnapshot(nlohmann::json{{"schema_version", "1.0"}});
    diagnostics.AppendCommandToReplayScript(command);
    diagnostics.AppendCommandResultToCompilationLog(command, CompilationProcessResult{});
    diagnostics.WriteSpecializationResult("/tmp/kernel.elf", OutputPublicationStatus::NotPublished);

    EXPECT_FALSE(fs::exists(testRoot));
}

} // namespace
