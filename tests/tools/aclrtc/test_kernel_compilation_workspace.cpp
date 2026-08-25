/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "specialization/kernel_compilation_workspace.h"

#include <gtest/gtest.h>

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>
#include <csignal>
#include <fstream>
#include <string>
#include <sys/resource.h>
#include <unistd.h>

namespace {
namespace fs = boost::filesystem;
using ascendc::aclrtc::KernelCompilationPlan;
using ascendc::aclrtc::KernelCompilationWorkspace;
using ascendc::aclrtc::WorktreeRetentionPolicy;

fs::path CreateWorkspacePath(const std::string& name)
{
    static uint64_t sequence = 0;
    const fs::path path =
        fs::temp_directory_path() / (name + "_" + std::to_string(getpid()) + "_" + std::to_string(sequence++));
    fs::create_directories(path);
    return path;
}

void WriteFile(const fs::path& path, const std::string& text)
{
    fs::create_directories(path.parent_path());
    std::ofstream stream(path.string());
    stream << text;
}

std::string ReadFile(const fs::path& path)
{
    std::ifstream stream(path.string());
    return std::string(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
}

TEST(KernelCompilationWorkspaceTest, CreatesOutputDirectoriesAndAppliesAllPatchesPerFile)
{
    const fs::path worktreePath = CreateWorkspacePath("aclrtc_workspace_patch");
    const fs::path sourcePath = worktreePath / "resources/constants.h";
    WriteFile(sourcePath, "A=@@A@@; B=@@B@@;\n");
    KernelCompilationPlan plan;
    plan.requiredOutputDirectoryPaths = {worktreePath / "outputs/nested"};
    plan.sourcePatches = {
        {sourcePath, "@@A@@", "{0x01}"},
        {sourcePath, "@@B@@", "{0x02}"},
    };

    KernelCompilationWorkspace workspace(worktreePath, WorktreeRetentionPolicy::RetainAfterCompilation);
    EXPECT_EQ(workspace.CreateOutputDirectoriesAndApplySourcePatches(plan), 0);
    EXPECT_TRUE(fs::is_directory(worktreePath / "outputs/nested"));
    EXPECT_EQ(ReadFile(sourcePath), "A={0x01}; B={0x02};\n");

    boost::system::error_code ignoredError;
    fs::remove_all(worktreePath, ignoredError);
}

TEST(KernelCompilationWorkspaceTest, ReplacesEveryOccurrenceOfTheSameSourceTemplate)
{
    const fs::path worktreePath = CreateWorkspacePath("aclrtc_workspace_repeated_template");
    const fs::path sourcePath = worktreePath / "resources/constants.h";
    WriteFile(sourcePath, "FIRST=@@VALUE@@; SECOND=@@VALUE@@;\n");
    KernelCompilationPlan plan;
    plan.sourcePatches = {{sourcePath, "@@VALUE@@", "{0x01}"}};

    KernelCompilationWorkspace workspace(worktreePath, WorktreeRetentionPolicy::RetainAfterCompilation);
    EXPECT_EQ(workspace.CreateOutputDirectoriesAndApplySourcePatches(plan), ascendc::aclrtc::ACLRTC_SUCCESS);
    EXPECT_EQ(ReadFile(sourcePath), "FIRST={0x01}; SECOND={0x01};\n");

    boost::system::error_code ignoredError;
    fs::remove_all(worktreePath, ignoredError);
}

TEST(KernelCompilationWorkspaceTest, FailsWhenPatchTemplateIsMissing)
{
    const fs::path worktreePath = CreateWorkspacePath("aclrtc_workspace_missing_template");
    const fs::path sourcePath = worktreePath / "resources/constants.h";
    WriteFile(sourcePath, "unchanged\n");
    KernelCompilationPlan plan;
    plan.sourcePatches = {{sourcePath, "@@MISSING@@", "{0x01}"}};

    KernelCompilationWorkspace workspace(worktreePath, WorktreeRetentionPolicy::RetainAfterCompilation);
    EXPECT_EQ(workspace.CreateOutputDirectoriesAndApplySourcePatches(plan), ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
    EXPECT_EQ(ReadFile(sourcePath), "unchanged\n");

    boost::system::error_code ignoredError;
    fs::remove_all(worktreePath, ignoredError);
}

TEST(KernelCompilationWorkspaceTest, FailsWhenPatchFileIsMissing)
{
    const fs::path worktreePath = CreateWorkspacePath("aclrtc_workspace_missing_file");
    const fs::path missingSourcePath = worktreePath / "resources/missing_constants.h";
    KernelCompilationPlan plan;
    plan.sourcePatches = {{missingSourcePath, "@@MISSING@@", "{0x01}"}};

    KernelCompilationWorkspace workspace(worktreePath, WorktreeRetentionPolicy::RetainAfterCompilation);
    EXPECT_EQ(workspace.CreateOutputDirectoriesAndApplySourcePatches(plan), ascendc::aclrtc::ACLRTC_ERROR_FAILURE);

    boost::system::error_code ignoredError;
    fs::remove_all(worktreePath, ignoredError);
}

TEST(KernelCompilationWorkspaceTest, FailsWhenPatchSourceCannotBeRead)
{
    const fs::path worktreePath = CreateWorkspacePath("aclrtc_workspace_unreadable_source");
    const fs::path directoryUsedAsSource = worktreePath / "resources/constants.h";
    fs::create_directories(directoryUsedAsSource);
    KernelCompilationPlan plan;
    plan.sourcePatches = {{directoryUsedAsSource, "@@VALUE@@", "{0x01}"}};

    KernelCompilationWorkspace workspace(worktreePath, WorktreeRetentionPolicy::RetainAfterCompilation);
    EXPECT_EQ(workspace.CreateOutputDirectoriesAndApplySourcePatches(plan), ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
    EXPECT_TRUE(fs::is_directory(directoryUsedAsSource));

    boost::system::error_code ignoredError;
    fs::remove_all(worktreePath, ignoredError);
}

TEST(KernelCompilationWorkspaceTest, ReportsSourceReadFailureAfterFileOpen)
{
    KernelCompilationPlan plan;
    plan.sourcePatches = {{"/proc/self/mem", "@@VALUE@@", "{0x01}"}};

    KernelCompilationWorkspace workspace({}, WorktreeRetentionPolicy::RetainAfterCompilation);
    EXPECT_EQ(workspace.CreateOutputDirectoriesAndApplySourcePatches(plan), ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
}

TEST(KernelCompilationWorkspaceTest, PreservesOriginalSourceWhenFinalPatchFlushFails)
{
    const fs::path worktreePath = CreateWorkspacePath("aclrtc_workspace_patch_flush_failure");
    const fs::path sourcePath = worktreePath / "resources/constants.h";
    WriteFile(sourcePath, "VALUE=@@VALUE@@;\n");
    KernelCompilationPlan plan;
    plan.sourcePatches = {{sourcePath, "@@VALUE@@", "{0x01}"}};

    struct rlimit originalFileSizeLimit {};
    ASSERT_EQ(getrlimit(RLIMIT_FSIZE, &originalFileSizeLimit), 0);
    struct sigaction ignoreFileSizeSignal {};
    struct sigaction originalFileSizeSignal {};
    ignoreFileSizeSignal.sa_handler = SIG_IGN;
    ASSERT_EQ(sigemptyset(&ignoreFileSizeSignal.sa_mask), 0);
    ASSERT_EQ(sigaction(SIGXFSZ, &ignoreFileSizeSignal, &originalFileSizeSignal), 0);
    struct rlimit zeroFileSizeLimit = originalFileSizeLimit;
    zeroFileSizeLimit.rlim_cur = 0;
    ASSERT_EQ(setrlimit(RLIMIT_FSIZE, &zeroFileSizeLimit), 0);

    KernelCompilationWorkspace workspace(worktreePath, WorktreeRetentionPolicy::RetainAfterCompilation);
    const aclError patchResult = workspace.CreateOutputDirectoriesAndApplySourcePatches(plan);

    const int restoreLimitResult = setrlimit(RLIMIT_FSIZE, &originalFileSizeLimit);
    const int restoreSignalResult = sigaction(SIGXFSZ, &originalFileSizeSignal, nullptr);
    ASSERT_EQ(restoreLimitResult, 0);
    ASSERT_EQ(restoreSignalResult, 0);
    EXPECT_EQ(patchResult, ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
    EXPECT_EQ(ReadFile(sourcePath), "VALUE=@@VALUE@@;\n");

    boost::system::error_code ignoredError;
    fs::remove_all(worktreePath, ignoredError);
}

TEST(KernelCompilationWorkspaceTest, PreservesOriginalSourceWhenLargePatchWriteFails)
{
    const fs::path worktreePath = CreateWorkspacePath("aclrtc_workspace_patch_write_failure");
    const fs::path sourcePath = worktreePath / "resources/constants.h";
    WriteFile(sourcePath, "VALUE=@@VALUE@@;\n");
    KernelCompilationPlan plan;
    plan.sourcePatches = {{sourcePath, "@@VALUE@@", std::string(64U * 1024U, 'x')}};

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

    KernelCompilationWorkspace workspace(worktreePath, WorktreeRetentionPolicy::RetainAfterCompilation);
    const aclError patchResult = workspace.CreateOutputDirectoriesAndApplySourcePatches(plan);

    const int restoreLimitResult = setrlimit(RLIMIT_FSIZE, &originalFileSizeLimit);
    const int restoreSignalResult = sigaction(SIGXFSZ, &originalFileSizeSignal, nullptr);
    ASSERT_EQ(restoreLimitResult, 0);
    ASSERT_EQ(restoreSignalResult, 0);
    EXPECT_EQ(patchResult, ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
    EXPECT_EQ(ReadFile(sourcePath), "VALUE=@@VALUE@@;\n");

    boost::system::error_code ignoredError;
    fs::remove_all(worktreePath, ignoredError);
}

TEST(KernelCompilationWorkspaceTest, DeletesWorktreeByDefaultAndRetainsItWhenRequested)
{
    const fs::path removedWorktreePath = CreateWorkspacePath("aclrtc_workspace_remove");
    {
        KernelCompilationWorkspace workspace(removedWorktreePath, WorktreeRetentionPolicy::RemoveAfterCompilation);
    }
    EXPECT_FALSE(fs::exists(removedWorktreePath));

    const fs::path retainedWorktreePath = CreateWorkspacePath("aclrtc_workspace_retain");
    {
        KernelCompilationWorkspace workspace(retainedWorktreePath, WorktreeRetentionPolicy::RetainAfterCompilation);
    }
    EXPECT_TRUE(fs::is_directory(retainedWorktreePath));
    boost::system::error_code ignoredError;
    fs::remove_all(retainedWorktreePath, ignoredError);
}

TEST(KernelCompilationWorkspaceTest, RemovesWorktreeBeforeOutputPublicationWhenNotRetained)
{
    const fs::path worktreePath = CreateWorkspacePath("aclrtc_workspace_prepublish_cleanup");
    KernelCompilationWorkspace workspace(worktreePath, WorktreeRetentionPolicy::RemoveAfterCompilation);
    EXPECT_EQ(workspace.CleanupWorktreeBeforeElfPublication(), 0);
    EXPECT_FALSE(fs::exists(worktreePath));
}

TEST(KernelCompilationWorkspaceTest, FailsWhenOutputDirectoryConflictsWithExistingFile)
{
    const fs::path worktreePath = CreateWorkspacePath("aclrtc_workspace_output_conflict");
    const fs::path conflictingOutputPath = worktreePath / "outputs";
    WriteFile(conflictingOutputPath, "not a directory");
    KernelCompilationPlan plan;
    plan.requiredOutputDirectoryPaths = {conflictingOutputPath / "nested"};

    KernelCompilationWorkspace workspace(worktreePath, WorktreeRetentionPolicy::RetainAfterCompilation);
    EXPECT_EQ(workspace.CreateOutputDirectoriesAndApplySourcePatches(plan), ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
    EXPECT_TRUE(fs::is_regular_file(conflictingOutputPath));

    boost::system::error_code ignoredError;
    fs::remove_all(worktreePath, ignoredError);
}

TEST(KernelCompilationWorkspaceTest, PreservesSourceWhenTemporaryPatchFilenameIsTooLong)
{
    const fs::path worktreePath = CreateWorkspacePath("aclrtc_workspace_long_patch_name");
    const fs::path sourcePath = worktreePath / std::string(245U, 'a');
    WriteFile(sourcePath, "VALUE=@@VALUE@@;\n");
    KernelCompilationPlan plan;
    plan.sourcePatches = {{sourcePath, "@@VALUE@@", "{0x01}"}};

    KernelCompilationWorkspace workspace(worktreePath, WorktreeRetentionPolicy::RetainAfterCompilation);
    EXPECT_EQ(workspace.CreateOutputDirectoriesAndApplySourcePatches(plan), ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
    EXPECT_EQ(ReadFile(sourcePath), "VALUE=@@VALUE@@;\n");

    boost::system::error_code ignoredError;
    fs::remove_all(worktreePath, ignoredError);
}

TEST(KernelCompilationWorkspaceTest, PrepublicationCleanupNoOpsForEmptyAndRetainedWorktrees)
{
    KernelCompilationWorkspace emptyWorkspace({}, WorktreeRetentionPolicy::RemoveAfterCompilation);
    EXPECT_EQ(emptyWorkspace.CleanupWorktreeBeforeElfPublication(), ascendc::aclrtc::ACLRTC_SUCCESS);

    const fs::path retainedWorktreePath = CreateWorkspacePath("aclrtc_workspace_retained_prepublish");
    KernelCompilationWorkspace retainedWorkspace(retainedWorktreePath, WorktreeRetentionPolicy::RetainAfterCompilation);
    EXPECT_EQ(retainedWorkspace.CleanupWorktreeBeforeElfPublication(), ascendc::aclrtc::ACLRTC_SUCCESS);
    EXPECT_TRUE(fs::is_directory(retainedWorktreePath));

    boost::system::error_code ignoredError;
    fs::remove_all(retainedWorktreePath, ignoredError);
}

TEST(KernelCompilationWorkspaceTest, ReportsWorktreeRemovalFailure)
{
    KernelCompilationWorkspace workspace("/proc/self/status", WorktreeRetentionPolicy::RemoveAfterCompilation);
    EXPECT_EQ(workspace.CleanupWorktreeBeforeElfPublication(), ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
    EXPECT_EQ(workspace.GetWorktreePath(), "/proc/self/status");
}
} // namespace
