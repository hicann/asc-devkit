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

#include "file_utils.h"
#include "directory_cleanup_guard.h"

#include <cstdlib>
#include <string>
#include <sys/stat.h>
#include <utility>
#include <vector>

namespace ascendc {
namespace {

bool PathExistsWithoutFollowingSymlinks(const std::string& path)
{
    struct stat pathStatus {};
    return lstat(path.c_str(), &pathStatus) == 0;
}

class DirectoryCleanupGuardTest : public testing::Test {
protected:
    void SetUp() override
    {
        const char* configuredTemporaryDirectory = std::getenv("TMPDIR");
        const std::string parentDirectoryPath =
            configuredTemporaryDirectory == nullptr || configuredTemporaryDirectory[0] == '\0' ?
                "/tmp" :
                configuredTemporaryDirectory;
        std::string rootDirectoryPattern =
            FileUtils::JoinPath(parentDirectoryPath, "directory_cleanup_guard_ut_XXXXXX");
        std::vector<char> writablePattern(rootDirectoryPattern.begin(), rootDirectoryPattern.end());
        writablePattern.push_back('\0');
        char* createdDirectoryPath = mkdtemp(writablePattern.data());
        ASSERT_NE(createdDirectoryPath, nullptr);
        rootDirectoryPath_ = createdDirectoryPath;
    }

    void TearDown() override { (void)FileUtils::RemoveAll(rootDirectoryPath_); }

    std::string rootDirectoryPath_;
};

TEST_F(DirectoryCleanupGuardTest, CreatesUniqueDirectoryAndRemovesItOnDestruction)
{
    const std::string parentPath = FileUtils::JoinPath(rootDirectoryPath_, "temporary-directories");
    ASSERT_TRUE(FileUtils::CreateDirectories(parentPath));
    std::string removedDirectoryPath;
    {
        DirectoryCleanupGuard directory;
        ASSERT_TRUE(directory.CreateUniqueSubdirectory(parentPath, "test"));
        removedDirectoryPath = directory.GetDirectoryPath();
        EXPECT_TRUE(FileUtils::IsDirectory(removedDirectoryPath));
        EXPECT_TRUE(FileUtils::IsPathWithin(removedDirectoryPath, parentPath));
        struct stat directoryStatus {};
        ASSERT_EQ(stat(removedDirectoryPath.c_str(), &directoryStatus), 0);
        constexpr mode_t permissionBits = S_IRWXU | S_IRWXG | S_IRWXO;
        EXPECT_EQ(directoryStatus.st_mode & permissionBits, S_IRWXU);
        EXPECT_FALSE(directory.CreateUniqueSubdirectory(parentPath, "second"));
    }
    EXPECT_FALSE(PathExistsWithoutFollowingSymlinks(removedDirectoryPath));
}

TEST_F(DirectoryCleanupGuardTest, PreservesCreatedOrRegisteredDirectory)
{
    const std::string createdParentPath = FileUtils::JoinPath(rootDirectoryPath_, "retained-created");
    ASSERT_TRUE(FileUtils::CreateDirectories(createdParentPath));
    std::string retainedCreatedPath;
    {
        DirectoryCleanupGuard directory;
        ASSERT_TRUE(directory.CreateUniqueSubdirectory(createdParentPath, "created"));
        retainedCreatedPath = directory.GetDirectoryPath();
        directory.PreserveDirectory();
    }
    EXPECT_TRUE(FileUtils::IsDirectory(retainedCreatedPath));

    const std::string retainedRegisteredPath = FileUtils::JoinPath(rootDirectoryPath_, "retained-registered");
    ASSERT_TRUE(FileUtils::CreateDirectories(retainedRegisteredPath));
    {
        DirectoryCleanupGuard directory;
        ASSERT_TRUE(directory.RegisterDirectoryForCleanup(retainedRegisteredPath));
        directory.PreserveDirectory();
    }
    EXPECT_TRUE(FileUtils::IsDirectory(retainedRegisteredPath));
}

TEST_F(DirectoryCleanupGuardTest, MovesDirectoryCleanupResponsibilityExactlyOnce)
{
    const std::string parentPath = FileUtils::JoinPath(rootDirectoryPath_, "moved");
    ASSERT_TRUE(FileUtils::CreateDirectories(parentPath));
    std::string movedDirectoryPath;
    {
        DirectoryCleanupGuard original;
        ASSERT_TRUE(original.CreateUniqueSubdirectory(parentPath, "original"));
        movedDirectoryPath = original.GetDirectoryPath();

        DirectoryCleanupGuard moved(std::move(original));
        EXPECT_TRUE(original.GetDirectoryPath().empty());
        EXPECT_EQ(moved.GetDirectoryPath(), movedDirectoryPath);

        DirectoryCleanupGuard assigned;
        assigned = std::move(moved);
        EXPECT_TRUE(moved.GetDirectoryPath().empty());
        EXPECT_EQ(assigned.GetDirectoryPath(), movedDirectoryPath);
    }
    EXPECT_FALSE(PathExistsWithoutFollowingSymlinks(movedDirectoryPath));
}

TEST_F(DirectoryCleanupGuardTest, RemovesRegisteredDirectoryOnRequest)
{
    const std::string registeredPath = FileUtils::JoinPath(rootDirectoryPath_, "registered");
    ASSERT_TRUE(FileUtils::CreateDirectories(registeredPath));
    DirectoryCleanupGuard directory;
    ASSERT_TRUE(directory.RegisterDirectoryForCleanup(registeredPath));
    EXPECT_TRUE(directory.RemoveDirectory());
    EXPECT_TRUE(directory.GetDirectoryPath().empty());
    EXPECT_FALSE(PathExistsWithoutFollowingSymlinks(registeredPath));
    EXPECT_TRUE(directory.RemoveDirectory());
}

TEST_F(DirectoryCleanupGuardTest, MoveAssignmentRemovesPreviouslyRegisteredDirectoryAndEmptiesSource)
{
    const std::string sourceParentPath = FileUtils::JoinPath(rootDirectoryPath_, "move-source");
    const std::string destinationParentPath = FileUtils::JoinPath(rootDirectoryPath_, "move-destination");
    ASSERT_TRUE(FileUtils::CreateDirectories(sourceParentPath));
    ASSERT_TRUE(FileUtils::CreateDirectories(destinationParentPath));
    std::string sourceDirectoryPath;
    std::string destinationDirectoryPath;
    {
        DirectoryCleanupGuard source;
        ASSERT_TRUE(source.CreateUniqueSubdirectory(sourceParentPath, "source"));
        sourceDirectoryPath = source.GetDirectoryPath();

        DirectoryCleanupGuard destination;
        ASSERT_TRUE(destination.CreateUniqueSubdirectory(destinationParentPath, "destination"));
        destinationDirectoryPath = destination.GetDirectoryPath();
        destination = std::move(source);

        EXPECT_EQ(destination.GetDirectoryPath(), sourceDirectoryPath);
        EXPECT_TRUE(source.GetDirectoryPath().empty());
        EXPECT_FALSE(PathExistsWithoutFollowingSymlinks(destinationDirectoryPath));
    }
    EXPECT_FALSE(PathExistsWithoutFollowingSymlinks(sourceDirectoryPath));
    EXPECT_FALSE(PathExistsWithoutFollowingSymlinks(destinationDirectoryPath));
}

TEST_F(DirectoryCleanupGuardTest, RejectsInvalidCreationInputsAndReportsRemovalFailure)
{
    DirectoryCleanupGuard directory;
    EXPECT_FALSE(directory.CreateUniqueSubdirectory(FileUtils::JoinPath(rootDirectoryPath_, "missing"), "test"));
    EXPECT_FALSE(directory.CreateUniqueSubdirectory(rootDirectoryPath_, "nested/test"));
    EXPECT_FALSE(directory.CreateUniqueSubdirectory(rootDirectoryPath_, "invalid%prefix"));
    EXPECT_FALSE(directory.RegisterDirectoryForCleanup(std::string()));

    ASSERT_TRUE(directory.RegisterDirectoryForCleanup("/proc/self/status"));
    EXPECT_FALSE(directory.RemoveDirectory());
    EXPECT_EQ(directory.GetDirectoryPath(), "/proc/self/status");
    directory.PreserveDirectory();
}

} // namespace
} // namespace ascendc
