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

#include <string>
#include <vector>

#include <sys/stat.h>
#include <unistd.h>

#include "collected_manifest_repository.h"
#include "file_utils.h"
#include "test_support.h"

namespace ascendc {
namespace manifest_generator {
namespace {

constexpr size_t TEST_MAX_DIRECTORY_DEPTH = 64U;

using asc_compile_exporter_test::ModuleTest;
using asc_compile_exporter_test::WriteTestFile;

TEST_F(ModuleTest, RepositorySortsManifestsAndResourcesAndSkipsEmptyFiles)
{
    const std::string first = FileUtils::JoinPath(root_, "collection/z/First");
    const std::string second = FileUtils::JoinPath(root_, "collection/a/Second");
    WriteTestFile(FileUtils::JoinPath(first, "resources/z.cpp"), "z");
    WriteTestFile(FileUtils::JoinPath(first, "resources/a.cpp"), "a");
    WriteTestFile(FileUtils::JoinPath(first, "resources/empty.cpp"), "");
    WriteTestFile(FileUtils::JoinPath(second, "resources/kernel.cpp"), "second");
    CreateManifest(first, "First");
    CreateManifest(second, "Second");
    WriteTestFile(FileUtils::JoinPath(root_, "collection/ignored.json"), "not a manifest");

    std::vector<ManifestUnit> units;
    ASSERT_TRUE(CollectedManifestRepository(FileUtils::JoinPath(root_, "collection")).Load(units));

    ASSERT_EQ(units.size(), 2U);
    EXPECT_NE(units[0].json.find("Second"), std::string::npos);
    EXPECT_NE(units[1].json.find("First"), std::string::npos);
    ASSERT_EQ(units[1].files.size(), 2U);
    EXPECT_EQ(units[1].files[0].fileName, "a.cpp");
    EXPECT_EQ(units[1].files[0].filePath, "resources/a.cpp");
    EXPECT_EQ(units[1].files[1].filePath, "resources/z.cpp");
    EXPECT_EQ(std::string(units[1].files[0].data.begin(), units[1].files[0].data.end()), "a");
}

TEST_F(ModuleTest, RepositoryReportsManifestJsonAndBaseDirErrors)
{
    const std::string malformed = FileUtils::JoinPath(root_, "malformed/Unit");
    WriteTestFile(FileUtils::JoinPath(malformed, "resources/kernel.cpp"), "source");
    WriteTestFile(FileUtils::JoinPath(malformed, "Unit_manifest.json"), "{\"base_dir\":");
    std::vector<ManifestUnit> units;
    EXPECT_FALSE(CollectedManifestRepository(FileUtils::JoinPath(root_, "malformed")).Load(units));

    const std::string missing = FileUtils::JoinPath(root_, "missing-base/Unit");
    WriteTestFile(FileUtils::JoinPath(missing, "resources/kernel.cpp"), "source");
    WriteTestFile(FileUtils::JoinPath(missing, "Unit_manifest.json"), "{}");
    EXPECT_FALSE(CollectedManifestRepository(FileUtils::JoinPath(root_, "missing-base")).Load(units));

    const std::string wrongType = FileUtils::JoinPath(root_, "wrong-type/Unit");
    WriteTestFile(FileUtils::JoinPath(wrongType, "resources/kernel.cpp"), "source");
    WriteTestFile(FileUtils::JoinPath(wrongType, "Unit_manifest.json"), "{\"base_dir\":7}");
    EXPECT_FALSE(CollectedManifestRepository(FileUtils::JoinPath(root_, "wrong-type")).Load(units));
}

TEST_F(ModuleTest, RepositorySkipsSymlinkedManifestsAndAcceptsAbsoluteBaseDir)
{
    const std::string collection = FileUtils::JoinPath(root_, "collection-absolute");
    const std::string unit = FileUtils::JoinPath(collection, "Unit");
    const std::string resources = FileUtils::JoinPath(unit, "resources");
    WriteTestFile(FileUtils::JoinPath(resources, "kernel.cpp"), "source");
    WriteTestFile(FileUtils::JoinPath(unit, "Unit_manifest.json"), "{\"base_dir\":\"" + resources + "\"}");

    const std::string ignoredManifest = FileUtils::JoinPath(root_, "ignored_manifest.json");
    WriteTestFile(ignoredManifest, "{\"base_dir\":\"resources\"}");
    ASSERT_EQ(symlink(ignoredManifest.c_str(), FileUtils::JoinPath(collection, "Link_manifest.json").c_str()), 0);

    std::vector<ManifestUnit> units;
    ASSERT_TRUE(CollectedManifestRepository(collection).Load(units));
    ASSERT_EQ(units.size(), 1U);
    ASSERT_EQ(units[0].files.size(), 1U);
    EXPECT_EQ(units[0].files[0].filePath, "resources/kernel.cpp");
}

TEST_F(ModuleTest, RepositoryRejectsNonRegularResource)
{
    const std::string unit = FileUtils::JoinPath(root_, "collection-special/Unit");
    const std::string resources = FileUtils::JoinPath(unit, "resources");
    ASSERT_TRUE(FileUtils::CreateDirectories(resources));
    ASSERT_EQ(mkfifo(FileUtils::JoinPath(resources, "resource.pipe").c_str(), 0600), 0);
    CreateManifest(unit, "Unit");

    std::vector<ManifestUnit> units;
    EXPECT_FALSE(CollectedManifestRepository(FileUtils::JoinPath(root_, "collection-special")).Load(units));
}

TEST_F(ModuleTest, RepositoryEnforcesDirectoryDepthLimit)
{
    const std::string allowedCollection = FileUtils::JoinPath(root_, "collection-depth-allowed");
    std::string allowedDirectory = allowedCollection;
    for (size_t depth = 0U; depth < TEST_MAX_DIRECTORY_DEPTH; ++depth) {
        allowedDirectory = FileUtils::JoinPath(allowedDirectory, "level-" + std::to_string(depth));
    }
    WriteTestFile(FileUtils::JoinPath(allowedDirectory, "Deep_manifest.json"), "{\"base_dir\":\".\"}");
    std::vector<ManifestUnit> units;
    EXPECT_TRUE(CollectedManifestRepository(allowedCollection).Load(units));

    const std::string rejectedCollection = FileUtils::JoinPath(root_, "collection-depth-rejected");
    std::string rejectedDirectory = rejectedCollection;
    for (size_t depth = 0U; depth <= TEST_MAX_DIRECTORY_DEPTH; ++depth) {
        rejectedDirectory = FileUtils::JoinPath(rejectedDirectory, "level-" + std::to_string(depth));
    }
    ASSERT_TRUE(FileUtils::CreateDirectories(rejectedDirectory));
    EXPECT_FALSE(CollectedManifestRepository(rejectedCollection).Load(units));
}

TEST_F(ModuleTest, RepositoryReportsDirectoryOpenFailure)
{
    std::vector<ManifestUnit> units;
    EXPECT_FALSE(CollectedManifestRepository("/dev/null").Load(units));
}

} // namespace
} // namespace manifest_generator
} // namespace ascendc
