/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <dlfcn.h>
#include <dirent.h>
#include <elf.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cerrno>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "ascendc_manifest_abi.h"
#include "manifest_bundle_compiler.h"
#include "process_executor.h"
#include "test_support.h"

namespace {

using asc_compile_exporter_test::ScopedEnvironment;
using ascendc::ProcessExecutor;
using ascendc::ProcessExecutorRequest;
using ascendc::manifest_generator::BuildCollectedBundleRequest;
using ascendc::manifest_generator::BundleOutputKind;
using ascendc::manifest_generator::ManifestBundleCompiler;

std::vector<int> g_renameErrors;

std::string JoinTestPath(const std::string& left, const std::string& right)
{
    if (left.empty()) {
        return right;
    }
    return left[left.size() - 1U] == '/' ? left + right : left + "/" + right;
}

std::string ParentTestPath(const std::string& path)
{
    const size_t separator = path.find_last_of('/');
    if (separator == std::string::npos) {
        return std::string();
    }
    return separator == 0U ? "/" : path.substr(0U, separator);
}

bool IsTestDirectory(const std::string& path)
{
    struct stat info {};
    return stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
}

bool IsTestRegularFile(const std::string& path)
{
    struct stat info {};
    return stat(path.c_str(), &info) == 0 && S_ISREG(info.st_mode);
}

void CreateTestDirectories(const std::string& path)
{
    if (path.empty() || path == "/" || IsTestDirectory(path)) {
        return;
    }
    CreateTestDirectories(ParentTestPath(path));
    if (mkdir(path.c_str(), 0700) != 0 && !(errno == EEXIST && IsTestDirectory(path))) {
        throw std::runtime_error("failed to create test directory " + path + ": " + std::strerror(errno));
    }
}

void RemoveTestTree(const std::string& path) noexcept
{
    struct stat info {};
    if (lstat(path.c_str(), &info) != 0) {
        return;
    }
    if (!S_ISDIR(info.st_mode) || S_ISLNK(info.st_mode)) {
        (void)unlink(path.c_str());
        return;
    }
    DIR* directory = opendir(path.c_str());
    if (directory != nullptr) {
        while (dirent* entry = readdir(directory)) {
            const std::string name(entry->d_name);
            if (name != "." && name != "..") {
                RemoveTestTree(JoinTestPath(path, name));
            }
        }
        (void)closedir(directory);
    }
    (void)rmdir(path.c_str());
}

std::vector<std::string> ListTestDirectory(const std::string& path)
{
    DIR* directory = opendir(path.c_str());
    if (directory == nullptr) {
        throw std::runtime_error("failed to enumerate test directory: " + path);
    }
    std::vector<std::string> names;
    while (dirent* entry = readdir(directory)) {
        const std::string name(entry->d_name);
        if (name != "." && name != "..") {
            names.push_back(name);
        }
    }
    (void)closedir(directory);
    return names;
}

void WriteFile(const std::string& path, const std::string& content)
{
    CreateTestDirectories(ParentTestPath(path));
    std::ofstream output(path.c_str(), std::ios::binary | std::ios::trunc);
    if (!output.is_open()) {
        throw std::runtime_error("failed to create test file: " + path);
    }
    output.write(content.data(), static_cast<std::streamsize>(content.size()));
    output.close();
    if (!output) {
        throw std::runtime_error("failed to write test file: " + path);
    }
}

std::string ReadFile(const std::string& path)
{
    std::ifstream input(path.c_str(), std::ios::binary);
    return std::string(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
}

std::string ToString(const AcString& value) { return std::string(value.data, value.size); }

bool ReadElfType(const std::string& path, uint16_t& type)
{
    std::ifstream input(path.c_str(), std::ios::binary);
    Elf64_Ehdr header{};
    input.read(reinterpret_cast<char*>(&header), static_cast<std::streamsize>(sizeof(header)));
    if (input.gcount() != static_cast<std::streamsize>(sizeof(header)) ||
        std::memcmp(header.e_ident, ELFMAG, SELFMAG) != 0 || header.e_ident[EI_CLASS] != ELFCLASS64) {
        return false;
    }
    type = header.e_type;
    return true;
}

} // namespace

extern "C" int __real_rename(const char* oldPath, const char* newPath);

extern "C" int __wrap_rename(const char* oldPath, const char* newPath)
{
    if (g_renameErrors.empty()) {
        return __real_rename(oldPath, newPath);
    }
    const int renameError = g_renameErrors.front();
    g_renameErrors.erase(g_renameErrors.begin());
    errno = renameError;
    return -1;
}

namespace {

class AscCompileExporterTest : public testing::Test {
protected:
    void SetUp() override
    {
        const char* temporary = std::getenv("TMPDIR");
        std::string pathTemplate = JoinTestPath(
            temporary == nullptr || temporary[0] == '\0' ? "/tmp" : temporary, "asc_compile_exporter_ut_XXXXXX");
        std::vector<char> buffer(pathTemplate.begin(), pathTemplate.end());
        buffer.push_back('\0');
        char* created = mkdtemp(buffer.data());
        ASSERT_NE(created, nullptr);
        root_ = created;
    }

    void TearDown() override
    {
        g_renameErrors.clear();
        RemoveTestTree(root_);
    }

    BuildCollectedBundleRequest Request() const
    {
        CreateTestDirectories(JoinTestPath(root_, "output"));
        BuildCollectedBundleRequest request;
        request.manifestSearchRoot = JoinTestPath(root_, "collection");
        request.outputPath = JoinTestPath(root_, "output/bundle.so");
        request.makeExecutable = "/usr/bin/make";
        request.cxxCompiler = "/usr/bin/c++";
        request.outputKind = BundleOutputKind::kSharedObject;
        request.keepTemp = false;
        request.jobs = 1U;
        return request;
    }

    void CreateManifest(
        const std::string& unit, const std::string& name, const std::string& resourcePath = "resources") const
    {
        WriteFile(
            JoinTestPath(unit, name + "_manifest.json"),
            "{\"resource_id\":\"" + name + "\",\"soc_version\":\"test\",\"resource_path\":\"" + resourcePath +
                "\",\"kernels\":[{\"kernel_name\":\"" + name +
                "\",\"objects\":[{\"object_name\":\"kernel.o\",\"object_type\":\"basic\",\"commands\":[{\"type\":"
                "\"compile\",\"cmd\":[\"${output}/kernel.o\"]}],\"outputs\":[\"${output}/kernel.o\"]}]}]}");
    }

    static void ExpectCompileFailure(const BuildCollectedBundleRequest& request)
    {
        EXPECT_FALSE(ManifestBundleCompiler(request).Compile());
    }

    std::string root_;
};

TEST_F(AscCompileExporterTest, ReportsWorkspaceAndBuildFailures)
{
    const std::string unit = JoinTestPath(root_, "collection/Failures");
    WriteFile(JoinTestPath(unit, "resources/kernel.cpp"), "source");
    CreateManifest(unit, "Failures");

    {
        ScopedEnvironment temporary("TMPDIR", "/dev/null");
        ExpectCompileFailure(Request());
    }

    BuildCollectedBundleRequest buildFailure = Request();
    buildFailure.makeExecutable = "/path/that/does/not/exist";
    ExpectCompileFailure(buildFailure);
}

TEST_F(AscCompileExporterTest, ReplacesTrustedOutputSymlinkWithoutFollowingIt)
{
    const std::string unit = JoinTestPath(root_, "collection/SymlinkOutput");
    WriteFile(JoinTestPath(unit, "resources/kernel.cpp"), "source");
    CreateManifest(unit, "SymlinkOutput");

    const BuildCollectedBundleRequest request = Request();
    const std::string victim = JoinTestPath(root_, "victim.so");
    WriteFile(victim, "victim");
    ASSERT_EQ(symlink(victim.c_str(), request.outputPath.c_str()), 0);

    ASSERT_TRUE(ManifestBundleCompiler(request).Compile());
    EXPECT_TRUE(IsTestRegularFile(request.outputPath));
    EXPECT_EQ(ReadFile(victim), "victim");
}

TEST_F(AscCompileExporterTest, ReportsDirectPublishRenameFailureWithoutReplacingOutput)
{
    const std::string unit = JoinTestPath(root_, "collection/PublishFailure");
    WriteFile(JoinTestPath(unit, "resources/kernel.cpp"), "source");
    CreateManifest(unit, "PublishFailure");
    const BuildCollectedBundleRequest request = Request();
    WriteFile(request.outputPath, "previous bundle");

    g_renameErrors = {EACCES};
    ExpectCompileFailure(request);
    EXPECT_EQ(ReadFile(request.outputPath), "previous bundle");
}

TEST_F(AscCompileExporterTest, PublishesBundleAfterCrossFileSystemRenameFailure)
{
    const std::string unit = JoinTestPath(root_, "collection/CrossFileSystem");
    WriteFile(JoinTestPath(unit, "resources/kernel.cpp"), "source");
    CreateManifest(unit, "CrossFileSystem");
    const BuildCollectedBundleRequest request = Request();
    WriteFile(request.outputPath, "previous bundle");

    g_renameErrors = {EXDEV};
    ASSERT_TRUE(ManifestBundleCompiler(request).Compile());
    EXPECT_TRUE(IsTestRegularFile(request.outputPath));
    EXPECT_NE(ReadFile(request.outputPath), "previous bundle");
}

TEST_F(AscCompileExporterTest, ReportsCrossFileSystemWorkspaceFailure)
{
    const std::string unit = JoinTestPath(root_, "collection/CrossFileSystemWorkspaceFailure");
    WriteFile(JoinTestPath(unit, "resources/kernel.cpp"), "source");
    CreateManifest(unit, "CrossFileSystemWorkspaceFailure");
    BuildCollectedBundleRequest request = Request();
    request.outputPath = "/proc/asc_compile_exporter_bundle.so";

    g_renameErrors = {EXDEV};
    ExpectCompileFailure(request);
}

TEST_F(AscCompileExporterTest, ReportsCrossFileSystemFinalRenameFailureWithoutReplacingOutput)
{
    const std::string unit = JoinTestPath(root_, "collection/CrossFileSystemRenameFailure");
    WriteFile(JoinTestPath(unit, "resources/kernel.cpp"), "source");
    CreateManifest(unit, "CrossFileSystemRenameFailure");
    const BuildCollectedBundleRequest request = Request();
    WriteFile(request.outputPath, "previous bundle");

    g_renameErrors = {EXDEV, EACCES};
    ExpectCompileFailure(request);
    EXPECT_EQ(ReadFile(request.outputPath), "previous bundle");
}

TEST_F(AscCompileExporterTest, RejectsMissingManifestWithoutReplacingOutput)
{
    CreateTestDirectories(JoinTestPath(root_, "collection"));
    const BuildCollectedBundleRequest request = Request();
    WriteFile(request.outputPath, "previous bundle");

    ExpectCompileFailure(request);
    EXPECT_EQ(ReadFile(request.outputPath), "previous bundle");
}

TEST_F(AscCompileExporterTest, RejectsMalformedJsonWithoutReplacingOutput)
{
    const std::string unit = JoinTestPath(root_, "collection/Malformed");
    WriteFile(JoinTestPath(unit, "resources/kernel.cpp"), "source");
    WriteFile(JoinTestPath(unit, "Malformed_manifest.json"), "{\"base_dir\":\"resources\"");
    const BuildCollectedBundleRequest request = Request();
    WriteFile(request.outputPath, "previous bundle");

    ExpectCompileFailure(request);
    EXPECT_EQ(ReadFile(request.outputPath), "previous bundle");
}

TEST_F(AscCompileExporterTest, RejectsResourcePathOutsideManifestDirectory)
{
    const std::string unit = JoinTestPath(root_, "collection/Traversal");
    WriteFile(JoinTestPath(root_, "collection/outside/kernel.cpp"), "source");
    CreateManifest(unit, "Traversal", "../outside");

    ExpectCompileFailure(Request());
}

TEST_F(AscCompileExporterTest, RejectsSymlinkInResourceDirectory)
{
    const std::string unit = JoinTestPath(root_, "collection/Symlink");
    WriteFile(JoinTestPath(unit, "resources/kernel.cpp"), "source");
    const std::string outside = JoinTestPath(root_, "outside.cpp");
    WriteFile(outside, "outside");
    ASSERT_EQ(symlink(outside.c_str(), JoinTestPath(unit, "resources/link.cpp").c_str()), 0);
    CreateManifest(unit, "Symlink");

    ExpectCompileFailure(Request());
}

TEST_F(AscCompileExporterTest, RejectsOversizedResourceWithoutReplacingOutput)
{
    const std::string unit = JoinTestPath(root_, "collection/Large");
    const std::string resource = JoinTestPath(unit, "resources/kernel.cpp");
    WriteFile(resource, "");
    ASSERT_EQ(truncate(resource.c_str(), 1U * 1024U * 1024U + 1U), 0);
    CreateManifest(unit, "Large");
    const BuildCollectedBundleRequest request = Request();
    WriteFile(request.outputPath, "previous bundle");

    ExpectCompileFailure(request);
    EXPECT_EQ(ReadFile(request.outputPath), "previous bundle");
}

TEST_F(AscCompileExporterTest, BuildsLoadableBundleAndCleansTemporaryWorkspace)
{
    ScopedEnvironment temporary("TMPDIR", root_.c_str());
    ScopedEnvironment cannHome("ASCEND_HOME_PATH", nullptr);
    ScopedEnvironment inheritedMakeFlags("MAKEFLAGS", "--definitely-invalid-option");
    const std::string first = JoinTestPath(root_, "collection/nested/First");
    const std::string second = JoinTestPath(root_, "collection/Second");
    WriteFile(JoinTestPath(first, "resources/kernel.cpp"), "first source");
    WriteFile(JoinTestPath(first, "resources/include/nested.h"), "nested header");
    WriteFile(JoinTestPath(first, "resources/empty.bin"), "");
    WriteFile(JoinTestPath(second, "resources/kernel.cpp"), "second source");
    CreateManifest(first, "First");
    CreateManifest(second, "Second");

    const BuildCollectedBundleRequest request = Request();
    WriteFile(request.outputPath, "previous bundle");
    ASSERT_TRUE(ManifestBundleCompiler(request).Compile());

    ASSERT_TRUE(IsTestRegularFile(request.outputPath));
    EXPECT_NE(ReadFile(request.outputPath), "previous bundle");
    const std::vector<std::string> workEntries = ListTestDirectory(root_);
    for (size_t index = 0U; index < workEntries.size(); ++index) {
        EXPECT_NE(workEntries[index].find(".manifest-generator-"), 0U);
    }

    void* handle = dlopen(request.outputPath.c_str(), RTLD_NOW | RTLD_LOCAL);
    const char* loadError = handle == nullptr ? dlerror() : nullptr;
    ASSERT_NE(handle, nullptr) << (loadError == nullptr ? "dlopen failed" : loadError);
    auto getter = reinterpret_cast<AscendcGetCompileResourceBundleFn>(dlsym(handle, "AscendcGetCompileResourceBundle"));
    ASSERT_NE(getter, nullptr);
    const AcCompileResourceBundleHeader* header = getter();
    ASSERT_NE(header, nullptr);
    EXPECT_EQ(header->magic, AC_COMPILE_RESOURCE_MAGIC);
    EXPECT_EQ(header->abiVersion, AC_COMPILE_RESOURCE_ABI_VERSION);
    EXPECT_EQ(header->structSize, sizeof(AcCompileResourceBundle));
    EXPECT_EQ(header->flags, 0U);

    const auto* bundle = reinterpret_cast<const AcCompileResourceBundle*>(header);
    ASSERT_EQ(bundle->manifestCount, 2U);
    EXPECT_EQ(bundle->extensions, nullptr);
    EXPECT_EQ(bundle->extensionCount, 0U);

    uint64_t totalFiles = 0U;
    bool foundNestedHeader = false;
    bool foundEmptyFile = false;
    bool foundFirstSource = false;
    for (uint64_t manifestIndex = 0U; manifestIndex < bundle->manifestCount; ++manifestIndex) {
        const AcCompileResourceManifest& manifest = bundle->manifests[manifestIndex];
        totalFiles += manifest.fileCount;
        for (uint64_t fileIndex = 0U; fileIndex < manifest.fileCount; ++fileIndex) {
            const AcCompileResourceFile& file = manifest.files[fileIndex];
            const std::string path = ToString(file.filePath);
            foundNestedHeader = foundNestedHeader || path == "resources/include/nested.h";
            foundEmptyFile = foundEmptyFile || path == "resources/empty.bin";
            foundFirstSource = foundFirstSource ||
                               (path == "resources/kernel.cpp" &&
                                std::string(reinterpret_cast<const char*>(file.data), file.size) == "first source");
        }
    }
    EXPECT_EQ(totalFiles, 3U);
    EXPECT_TRUE(foundNestedHeader);
    EXPECT_FALSE(foundEmptyFile);
    EXPECT_TRUE(foundFirstSource);
    EXPECT_EQ(dlclose(handle), 0);
}

TEST_F(AscCompileExporterTest, BuildsRelocatableObjectThatCanBeLinkedIntoSharedLibrary)
{
    const std::string unit = JoinTestPath(root_, "collection/Relocatable");
    WriteFile(JoinTestPath(unit, "resources/kernel.cpp"), "relocatable source");
    CreateManifest(unit, "Relocatable");

    BuildCollectedBundleRequest request = Request();
    request.outputPath = JoinTestPath(root_, "output/bundle.o");
    request.outputKind = BundleOutputKind::kRelocatableObject;
    g_renameErrors = {EXDEV};
    ASSERT_TRUE(ManifestBundleCompiler(request).Compile());
    ASSERT_TRUE(IsTestRegularFile(request.outputPath));

    uint16_t elfType = ET_NONE;
    ASSERT_TRUE(ReadElfType(request.outputPath, elfType));
    EXPECT_EQ(elfType, ET_REL);

    const std::string linkedBundle = JoinTestPath(root_, "output/linked-bundle.so");
    ProcessExecutorRequest linkRequest;
    linkRequest.arguments = {request.cxxCompiler, "-shared", request.outputPath, "-o", linkedBundle};
    linkRequest.executionTimeout = std::chrono::minutes(5);
    linkRequest.terminationGracePeriod = std::chrono::seconds(1);
    ASSERT_TRUE(ProcessExecutor::Execute(linkRequest).HasSuccessfulExit());

    void* handle = dlopen(linkedBundle.c_str(), RTLD_NOW | RTLD_LOCAL);
    const char* loadError = handle == nullptr ? dlerror() : nullptr;
    ASSERT_NE(handle, nullptr) << (loadError == nullptr ? "dlopen failed" : loadError);
    auto getter = reinterpret_cast<AscendcGetCompileResourceBundleFn>(dlsym(handle, "AscendcGetCompileResourceBundle"));
    ASSERT_NE(getter, nullptr);
    const auto* bundle = reinterpret_cast<const AcCompileResourceBundle*>(getter());
    ASSERT_NE(bundle, nullptr);
    ASSERT_EQ(bundle->manifestCount, 1U);
    ASSERT_EQ(bundle->manifests[0].fileCount, 1U);
    EXPECT_EQ(ToString(bundle->manifests[0].files[0].filePath), "resources/kernel.cpp");
    EXPECT_EQ(
        std::string(
            reinterpret_cast<const char*>(bundle->manifests[0].files[0].data), bundle->manifests[0].files[0].size),
        "relocatable source");
    EXPECT_EQ(dlclose(handle), 0);
}

TEST_F(AscCompileExporterTest, KeepsTemporaryWorkspaceWhenRequested)
{
    ScopedEnvironment temporary("TMPDIR", root_.c_str());
    const std::string unit = JoinTestPath(root_, "collection/Keep");
    WriteFile(JoinTestPath(unit, "resources/kernel.cpp"), "source");
    CreateManifest(unit, "Keep");

    BuildCollectedBundleRequest request = Request();
    request.keepTemp = true;
    ASSERT_TRUE(ManifestBundleCompiler(request).Compile());

    const std::vector<std::string> workEntries = ListTestDirectory(root_);
    size_t workspaceCount = 0U;
    for (size_t index = 0U; index < workEntries.size(); ++index) {
        workspaceCount += workEntries[index].find(".manifest-generator-") == 0U ? 1U : 0U;
    }
    EXPECT_EQ(workspaceCount, 1U);
    EXPECT_TRUE(IsTestRegularFile(request.outputPath));
}

} // namespace
