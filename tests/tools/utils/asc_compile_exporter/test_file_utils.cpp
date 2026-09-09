/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the LICENSE.
 */

#include <gtest/gtest.h>

#include <fstream>
#include <boost/filesystem.hpp>
#include <sys/stat.h>
#include <string>
#include <vector>

#include <unistd.h>

#include "file_utils.h"
#include "test_support.h"

namespace ascendc {
namespace {
namespace fs = boost::filesystem;

using asc_compile_exporter_test::LstatPathExists;
using asc_compile_exporter_test::ModuleTest;
using asc_compile_exporter_test::ReadTestFile;
using asc_compile_exporter_test::WriteTestFile;

TEST_F(ModuleTest, FileUtilsBuildsAndComparesPaths)
{
    EXPECT_EQ(FileUtils::JoinPath("", "right"), "right");
    EXPECT_EQ(FileUtils::JoinPath("left", ""), "left");
    EXPECT_EQ(FileUtils::JoinPath("/left", "right"), "/left/right");
    EXPECT_EQ(FileUtils::ParentPath("/one/two.txt"), "/one");
    EXPECT_EQ(FileUtils::ParentPath("single"), "");
    EXPECT_EQ(FileUtils::FileName("/one/two.txt"), "two.txt");

    std::string absolute;
    ASSERT_TRUE(FileUtils::MakeAbsolutePath("relative/path", absolute));
    EXPECT_FALSE(absolute.empty());
    EXPECT_EQ(absolute[0], '/');

    EXPECT_TRUE(FileUtils::IsSafeRelativePath("relative/path"));
    EXPECT_TRUE(FileUtils::IsSafeRelativePath("file.cpp"));
    EXPECT_FALSE(FileUtils::IsSafeRelativePath(""));
    EXPECT_FALSE(FileUtils::IsSafeRelativePath("/absolute/path"));
    EXPECT_FALSE(FileUtils::IsSafeRelativePath("relative\\path"));
    EXPECT_FALSE(FileUtils::IsSafeRelativePath("relative/../outside"));
    EXPECT_FALSE(FileUtils::IsSafeRelativePath("./relative"));
    EXPECT_FALSE(FileUtils::IsSafeRelativePath("relative//path"));
    EXPECT_FALSE(FileUtils::IsSafeRelativePath(std::string("embedded\0nul", 12U)));

    EXPECT_TRUE(FileUtils::IsPathWithin("/root/child", "/root"));
    EXPECT_TRUE(FileUtils::IsPathWithin("/root", "/root"));
    EXPECT_TRUE(FileUtils::IsPathWithin("/root/nested/../child", "/root"));
    EXPECT_TRUE(FileUtils::IsPathWithin("/child", "/"));
    EXPECT_FALSE(FileUtils::IsPathWithin("/rooted", "/root"));
    EXPECT_FALSE(FileUtils::IsPathWithin("/root/../outside", "/root"));
    EXPECT_FALSE(FileUtils::IsPathWithin("relative", "/"));
    EXPECT_FALSE(FileUtils::IsPathWithin("", "/"));
}

TEST_F(ModuleTest, FileUtilsInspectsAndResolvesPaths)
{
    const std::string directory = FileUtils::JoinPath(root_, "directory/nested");
    const std::string regular = FileUtils::JoinPath(directory, "regular.bin");
    const std::string link = FileUtils::JoinPath(root_, "directory-link");
    ASSERT_TRUE(FileUtils::CreateDirectories(directory));
    WriteTestFile(regular, "data");
    ASSERT_EQ(symlink(directory.c_str(), link.c_str()), 0);

    EXPECT_TRUE(FileUtils::PathExists(regular));
    EXPECT_TRUE(FileUtils::IsDirectory(directory));
    EXPECT_TRUE(FileUtils::IsRegularFile(regular));
    EXPECT_TRUE(FileUtils::IsSymlink(link));
    EXPECT_FALSE(FileUtils::IsSymlink(regular));
    EXPECT_FALSE(FileUtils::PathExists(FileUtils::JoinPath(root_, "missing")));

    std::string canonical;
    ASSERT_TRUE(FileUtils::ResolveCanonicalPath(directory, canonical));
    std::string resolved;
    EXPECT_TRUE(FileUtils::ResolveDirectory(directory, resolved));
    EXPECT_EQ(resolved, canonical);
    EXPECT_TRUE(FileUtils::ResolveSubdirectory(directory, root_, resolved));
    EXPECT_EQ(resolved, canonical);
    EXPECT_FALSE(FileUtils::ResolveCanonicalPath(FileUtils::JoinPath(root_, "missing"), resolved));
    EXPECT_TRUE(resolved.empty());
    EXPECT_FALSE(FileUtils::ResolveDirectory(regular, resolved));
    EXPECT_FALSE(FileUtils::ResolveDirectory(link, resolved));
    EXPECT_FALSE(FileUtils::ResolveDirectory(FileUtils::JoinPath(root_, "missing"), resolved));
    EXPECT_FALSE(FileUtils::ResolveSubdirectory("/tmp", root_, resolved));
}

TEST_F(ModuleTest, FileUtilsResolvesNewOutputUnderCanonicalParentWithoutCreatingIt)
{
    const std::string directory = FileUtils::JoinPath(root_, "output");
    const std::string alias = FileUtils::JoinPath(root_, "output-alias");
    ASSERT_TRUE(FileUtils::CreateDirectories(directory));
    ASSERT_EQ(symlink(directory.c_str(), alias.c_str()), 0);
    std::string resolved;
    ASSERT_TRUE(FileUtils::ResolveRegularFilePathForWrite(alias + "/./new.log", resolved));
    EXPECT_EQ(resolved, directory + "/new.log");
    EXPECT_FALSE(FileUtils::PathExists(resolved));
    WriteTestFile(resolved, "keep");
    ASSERT_TRUE(FileUtils::ResolveRegularFilePathForWrite(alias + "/new.log", resolved));
    EXPECT_EQ(ReadTestFile(resolved), "keep");
}

TEST_F(ModuleTest, FileUtilsRejectsInvalidOutputNamesAndSpecialFiles)
{
    const std::string regular = FileUtils::JoinPath(root_, "regular");
    const std::string link = FileUtils::JoinPath(root_, "link");
    const std::string danglingLink = FileUtils::JoinPath(root_, "dangling");
    WriteTestFile(regular, "keep");
    ASSERT_EQ(symlink(regular.c_str(), link.c_str()), 0);
    ASSERT_EQ(symlink("missing", danglingLink.c_str()), 0);
    const std::vector<std::string> invalidPaths{
        "",
        root_,
        root_ + "/.",
        root_ + "/..",
        root_ + "/missing/file",
        regular + "/file",
        link,
        danglingLink,
        "/dev/null",
        regular + std::string("\0suffix", 7U),
        root_ + "/" + std::string(5000U, 'x')};
    for (const std::string& path : invalidPaths) {
        std::string resolved = "stale";
        EXPECT_FALSE(FileUtils::ResolveRegularFilePathForWrite(path, resolved)) << path;
        EXPECT_TRUE(resolved.empty()) << path;
    }
    EXPECT_EQ(ReadTestFile(regular), "keep");
}

TEST_F(ModuleTest, FileUtilsOpensCanonicalInputAndRejectsInvalidInputWithoutOpening)
{
    const std::string source = root_ + "/source";
    const std::string alias = root_ + "/alias";
    WriteTestFile(source, "source-text");
    ASSERT_EQ(symlink(source.c_str(), alias.c_str()), 0);
    std::ifstream input;
    ASSERT_TRUE(FileUtils::OpenRegularFileForRead(alias, input));
    std::string text;
    input >> text;
    EXPECT_EQ(text, "source-text");
    EXPECT_FALSE(FileUtils::OpenRegularFileForRead(source, input));
    input.close();
    for (const std::string& path :
         std::vector<std::string>{"", root_, root_ + "/missing", source + std::string("\0x", 2U)}) {
        std::ifstream invalidInput;
        EXPECT_FALSE(FileUtils::OpenRegularFileForRead(path, invalidInput));
        EXPECT_FALSE(invalidInput.is_open());
    }
}

TEST_F(ModuleTest, FileUtilsWritesAndAppendsTextThroughTheSameCheckedOpen)
{
    const std::string destination = root_ + "/log";
    const std::string alias = root_ + "/alias";
    ASSERT_TRUE(FileUtils::WriteTextFile(destination, "first", std::ios::trunc));
    ASSERT_TRUE(FileUtils::WriteTextFile(destination, "second", std::ios::app));
    EXPECT_EQ(ReadTestFile(destination), "firstsecond");
    ASSERT_EQ(symlink(destination.c_str(), alias.c_str()), 0);
    EXPECT_FALSE(FileUtils::WriteTextFile(alias, "unexpected", std::ios::trunc));
    EXPECT_EQ(ReadTestFile(destination), "firstsecond");
}

TEST_F(ModuleTest, FileUtilsCreatesAndRemovesDirectoryTrees)
{
    EXPECT_TRUE(FileUtils::CreateDirectories(""));
    EXPECT_TRUE(FileUtils::CreateDirectories("/"));
    const std::string tree = FileUtils::JoinPath(root_, "tree");
    const std::string nested = FileUtils::JoinPath(tree, "nested");
    ASSERT_TRUE(FileUtils::CreateDirectories(nested));
    EXPECT_TRUE(FileUtils::CreateDirectories(nested));
    WriteTestFile(FileUtils::JoinPath(nested, "leaf.txt"), "leaf");

    const std::string blockingFile = FileUtils::JoinPath(root_, "blocking-file");
    WriteTestFile(blockingFile, "block");
    EXPECT_FALSE(FileUtils::CreateDirectories(FileUtils::JoinPath(blockingFile, "child")));

    const std::string link = FileUtils::JoinPath(root_, "tree-link");
    ASSERT_EQ(symlink(tree.c_str(), link.c_str()), 0);
    EXPECT_TRUE(FileUtils::RemoveAll(link));
    EXPECT_TRUE(FileUtils::IsDirectory(tree));
    EXPECT_TRUE(FileUtils::RemoveAll(tree));
    EXPECT_FALSE(LstatPathExists(tree));
    EXPECT_TRUE(FileUtils::RemoveAll(FileUtils::JoinPath(root_, "missing")));
    EXPECT_TRUE(FileUtils::RemoveAll("/dev/null/child"));
    EXPECT_FALSE(FileUtils::RemoveAll(FileUtils::JoinPath(root_, std::string(5000U, 'x'))));
}

TEST_F(ModuleTest, FileUtilsReadsRegularFilesWithinSizeLimit)
{
    const std::string binary = FileUtils::JoinPath(root_, "binary/data.bin");
    const std::string content("\0\x7f\xff", 3U);
    WriteTestFile(binary, content);

    std::vector<uint8_t> data;
    ASSERT_TRUE(FileUtils::ReadRegularFile(binary, content.size(), data));
    EXPECT_EQ(std::string(data.begin(), data.end()), content);
    EXPECT_FALSE(FileUtils::ReadRegularFile(binary, content.size() - 1U, data));
    EXPECT_TRUE(data.empty());
    EXPECT_FALSE(FileUtils::ReadRegularFile(root_, 100U, data));
    EXPECT_FALSE(FileUtils::ReadRegularFile(FileUtils::JoinPath(root_, "missing"), 100U, data));

    const std::string empty = FileUtils::JoinPath(root_, "binary/empty.bin");
    WriteTestFile(empty, "");
    EXPECT_TRUE(FileUtils::ReadRegularFile(empty, 0U, data));
    EXPECT_TRUE(data.empty());

    const std::string link = FileUtils::JoinPath(root_, "binary-link");
    ASSERT_EQ(symlink(binary.c_str(), link.c_str()), 0);
    EXPECT_FALSE(FileUtils::ReadRegularFile(link, 100U, data));
}

TEST_F(ModuleTest, FileUtilsWritesTextAtomicallyWithPrivatePermissions)
{
    const std::string destination = root_ + "/output";
    WriteTestFile(destination, "original");
    const std::string text("a\0b", 3U);
    const mode_t previousMask = umask(0022);
    const bool written = FileUtils::WriteTextFileAtomically(destination, text);
    umask(previousMask);
    ASSERT_TRUE(written);
    EXPECT_EQ(ReadTestFile(destination), text);
    struct stat status {};
    ASSERT_EQ(stat(destination.c_str(), &status), 0);
    EXPECT_EQ(status.st_mode & 0777U, 0600U);
}

TEST_F(ModuleTest, FileUtilsRemovesTemporaryCopyOnCallbackException)
{
    const std::string source = root_ + "/source";
    const std::string destination = root_ + "/output";
    WriteTestFile(source, "new");
    WriteTestFile(destination, "original");
    EXPECT_THROW(
        FileUtils::CopyFileAtomically(
            source, destination, []() -> bool { throw std::runtime_error("simulated cleanup failure"); }),
        std::runtime_error);
    EXPECT_EQ(ReadTestFile(destination), "original");
    EXPECT_EQ(std::distance(fs::directory_iterator(root_), fs::directory_iterator()), 2);
}

TEST_F(ModuleTest, FileUtilsCopiesBeforeCleanupAndReplacesAfterCleanup)
{
    const std::string source = root_ + "/worktree/source";
    const std::string destination = root_ + "/output";
    const std::string bytes(150000U, 'x');
    WriteTestFile(source, bytes);
    WriteTestFile(destination, "original");
    const mode_t previousMask = umask(0027);
    const bool copied = FileUtils::CopyFileAtomically(source, destination, [&]() {
        EXPECT_EQ(ReadTestFile(destination), "original");
        return FileUtils::RemoveAll(root_ + "/worktree");
    });
    umask(previousMask);
    ASSERT_TRUE(copied);
    EXPECT_EQ(ReadTestFile(destination), bytes);
    struct stat status {};
    ASSERT_EQ(stat(destination.c_str(), &status), 0);
    EXPECT_EQ(status.st_mode & 0777U, 0640U);
}

TEST_F(ModuleTest, FileUtilsUsesOriginalCanonicalParentForAtomicCopy)
{
    const std::string source = root_ + "/source";
    const std::string first = root_ + "/first";
    const std::string second = root_ + "/second";
    const std::string alias = root_ + "/alias";
    WriteTestFile(source, "new");
    ASSERT_TRUE(FileUtils::CreateDirectories(first));
    ASSERT_TRUE(FileUtils::CreateDirectories(second));
    ASSERT_EQ(symlink(first.c_str(), alias.c_str()), 0);
    ASSERT_TRUE(FileUtils::CopyFileAtomically(source, alias + "/output", [&]() {
        return unlink(alias.c_str()) == 0 && symlink(second.c_str(), alias.c_str()) == 0;
    }));
    EXPECT_EQ(ReadTestFile(first + "/output"), "new");
    EXPECT_FALSE(FileUtils::PathExists(second + "/output"));
}

TEST_F(ModuleTest, FileUtilsRemovesTemporaryCopyWhenRenameFails)
{
    const std::string source = root_ + "/source";
    const std::string destination = root_ + "/output";
    WriteTestFile(source, "new");
    EXPECT_FALSE(FileUtils::CopyFileAtomically(
        source, destination, [&]() { return FileUtils::CreateDirectories(destination); }));
    EXPECT_TRUE(FileUtils::IsDirectory(destination));
    EXPECT_EQ(std::distance(fs::directory_iterator(root_), fs::directory_iterator()), 2);
}

TEST_F(ModuleTest, FileUtilsKeepsExistingFilesWhenTemporaryNamesCollide)
{
    const std::string destination = root_ + "/output";
    const std::string victim = root_ + "/victim";
    WriteTestFile(victim, "preserve");
    for (uint64_t sequence = 0U; sequence < 128U; ++sequence) {
        const std::string collision =
            destination + ".asc_tmp_" + std::to_string(getpid()) + "_" + std::to_string(sequence);
        ASSERT_EQ(symlink(victim.c_str(), collision.c_str()), 0);
    }
    EXPECT_FALSE(FileUtils::WriteTextFileAtomically(destination, "unexpected"));
    EXPECT_EQ(ReadTestFile(victim), "preserve");
    EXPECT_EQ(std::distance(fs::directory_iterator(root_), fs::directory_iterator()), 129);
}

TEST_F(ModuleTest, FileUtilsRejectsMissingCopySourceBeforeCallingCleanup)
{
    bool cleanupCalled = false;
    EXPECT_FALSE(FileUtils::CopyFileAtomically(root_ + "/missing", root_ + "/output", [&]() {
        cleanupCalled = true;
        return true;
    }));
    EXPECT_FALSE(cleanupCalled);
    const std::string source = root_ + "/source";
    WriteTestFile(source, "new");
    ASSERT_TRUE(FileUtils::CopyFileAtomically(source, root_ + "/output"));
    EXPECT_EQ(ReadTestFile(root_ + "/output"), "new");
}

TEST_F(ModuleTest, FileUtilsKeepsDestinationAndCleansTemporaryCopyWhenCallbackReturnsFalse)
{
    const std::string source = root_ + "/source";
    const std::string destination = root_ + "/output";
    WriteTestFile(source, "new");
    WriteTestFile(destination, "original");
    EXPECT_FALSE(FileUtils::CopyFileAtomically(source, destination, []() { return false; }));
    EXPECT_EQ(ReadTestFile(destination), "original");
    EXPECT_EQ(std::distance(fs::directory_iterator(root_), fs::directory_iterator()), 2);
}

TEST_F(ModuleTest, FileUtilsFinalizesStreamsAndCopiesFiles)
{
    const std::string generated = FileUtils::JoinPath(root_, "generated.cpp");
    std::ofstream output(generated.c_str(), std::ios::trunc);
    ASSERT_TRUE(output.is_open());
    output << "generated";
    EXPECT_TRUE(FileUtils::FinalizeOutput(output));
    EXPECT_EQ(ReadTestFile(generated), "generated");

    std::ofstream failedOutput;
    failedOutput.setstate(std::ios::badbit);
    EXPECT_FALSE(FileUtils::FinalizeOutput(failedOutput));

    const std::string source = FileUtils::JoinPath(root_, "source.bin");
    const std::string destination = FileUtils::JoinPath(root_, "destination.bin");
    WriteTestFile(source, "copy data");
    EXPECT_TRUE(FileUtils::CopyFile(source, destination));
    EXPECT_EQ(ReadTestFile(destination), "copy data");
    EXPECT_FALSE(FileUtils::CopyFile(source, destination));
    EXPECT_FALSE(FileUtils::CopyFile(FileUtils::JoinPath(root_, "missing"), FileUtils::JoinPath(root_, "copy")));
    EXPECT_FALSE(FileUtils::CopyFile(root_, FileUtils::JoinPath(root_, "directory-copy")));
    EXPECT_FALSE(FileUtils::CopyFile(source, FileUtils::JoinPath(root_, "missing-parent/copy")));
}

} // namespace
} // namespace ascendc
