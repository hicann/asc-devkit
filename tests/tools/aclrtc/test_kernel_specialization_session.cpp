/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "specialization/kernel_specialization.h"
#include "specialization/kernel_compilation_workspace.h"
#include "specialization/resource_registry.h"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>
#include <csignal>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <string>
#include <sys/resource.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utility>
#include <vector>

namespace {
namespace fs = boost::filesystem;
using Json = nlohmann::json;
using ascendc::DirectoryCleanupGuard;
using ascendc::aclrtc::KernelCompilationWorkspace;
using ascendc::aclrtc::KernelSpecializationSession;
using ascendc::aclrtc::MaterializedKernelCompilationResource;
using ascendc::aclrtc::NormalizedKernelSpecializationRequest;
using ascendc::aclrtc::WorktreeRetentionPolicy;
using ascendc::specialization_compile::ResourceEntry;
using ascendc::specialization_compile::ResourceFileData;
using ascendc::specialization_compile::ResourceRegistry;
using ascendc::specialization_compile::ResourceSourceType;
using ascendc::specialization_compile::ResourceStatus;

enum class FakeCompilerOutcome : uint32_t {
    Success,
    Failure,
};

constexpr char RESOURCE_ID[] = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";

class ScopedEnvironmentVariable final {
public:
    ScopedEnvironmentVariable(const char* name, const char* value) : name_(name)
    {
        const char* previousValue = std::getenv(name);
        if (previousValue != nullptr) {
            previousValue_ = previousValue;
            wasSet_ = true;
        }
        if (value == nullptr) {
            unsetenv(name);
        } else {
            setenv(name, value, 1);
        }
    }

    ~ScopedEnvironmentVariable()
    {
        if (wasSet_) {
            setenv(name_.c_str(), previousValue_.c_str(), 1);
        } else {
            unsetenv(name_.c_str());
        }
    }

private:
    std::string name_;
    std::string previousValue_;
    bool wasSet_{false};
};

class EndToEndTestResource final {
public:
    explicit EndToEndTestResource(FakeCompilerOutcome compilerOutcome)
    {
        static uint64_t sequence = 0;
        rootPath_ = fs::temp_directory_path() /
                    ("aclrtc_session_test_" + std::to_string(getpid()) + "_" + std::to_string(sequence++));
        worktreePath_ = rootPath_ / "worktree";
        sourceDirectoryPath_ = rootPath_ / "user";
        toolchainPath_ = rootPath_ / "toolchain";
        outputElfPath_ = rootPath_ / "output/kernel.elf";
        fs::create_directories(worktreePath_ / "resources/include");
        fs::create_directories(worktreePath_ / "resources/src");
        fs::create_directories(sourceDirectoryPath_);
        fs::create_directories(toolchainPath_ / "bin");
        fs::create_directories(outputElfPath_.parent_path());
        WriteFile(sourceDirectoryPath_ / "add_custom.cpp", "// user source\n");
        WriteFile(worktreePath_ / "resources/include/constants.h", "TILING=@@STATIC_TILING@@;\n");
        WriteFile(worktreePath_ / "resources/src/basic.cpp", "// basic source\n");
        WriteExecutable(
            toolchainPath_ / "bin/bisheng",
            compilerOutcome == FakeCompilerOutcome::Success ? CreateOutputScript("object") : "#!/bin/sh\nexit 2\n");
        WriteExecutable(toolchainPath_ / "bin/ld.lld", CreateOutputScript("linked-elf"));
        manifest_ = CreateManifest();
    }

    ~EndToEndTestResource()
    {
        boost::system::error_code ignoredError;
        fs::remove_all(rootPath_, ignoredError);
    }

    MaterializedKernelCompilationResource CreateMaterializedCompilationResource() const
    {
        return MaterializedKernelCompilationResource{manifest_, worktreePath_, sourceDirectoryPath_};
    }

    const fs::path& ToolchainPath() const { return toolchainPath_; }
    const fs::path& WorktreePath() const { return worktreePath_; }
    const fs::path& SourceDirectoryPath() const { return sourceDirectoryPath_; }
    const fs::path& OutputElfPath() const { return outputElfPath_; }
    const Json& Manifest() const { return manifest_; }

private:
    static void WriteFile(const fs::path& path, const std::string& contents)
    {
        std::ofstream stream(path.string());
        stream << contents;
    }

    static void WriteExecutable(const fs::path& path, const std::string& contents)
    {
        WriteFile(path, contents);
        chmod(path.c_str(), 0755);
    }

    static std::string CreateOutputScript(const char* contents)
    {
        return std::string("#!/bin/sh\n") +
               "while [ \"$#\" -gt 0 ]; do\n"
               "  if [ \"$1\" = \"-o\" ]; then\n"
               "    shift\n"
               "    printf '" +
               contents +
               "' > \"$1\"\n"
               "    exit 0\n"
               "  fi\n"
               "  shift\n"
               "done\n"
               "exit 3\n";
    }

    static Json CreateManifest()
    {
        return Json{
            {"schema_version", "1.0"},
            {"source_file", "add_custom.cpp"},
            {"kernels", Json::array(
                            {{{"kernel_name", "add_custom_100000"},
                              {"constant_infos", Json::array(
                                                     {{{"parameter_index", 0},
                                                       {"byte_size", 2},
                                                       {"file", "${resource}/resources/include/constants.h"},
                                                       {"template", "@@STATIC_TILING@@"}}})},
                              {"link_options", Json::array({"-m", "aicorelinux", "-Ttext=0"})},
                              {"objects", Json::array(
                                              {{{"object_name", "basic"},
                                                {"object_type", "basic"},
                                                {"commands", Json::array(
                                                                 {{{"type", "compile"},
                                                                   {"stage", 0},
                                                                   {"cmd", Json::array(
                                                                               {"${env:ASCEND_HOME_PATH}/bin/bisheng",
                                                                                "${resource}/resources/src/basic.cpp",
                                                                                "-o", "${output}/basic.o"})}}})},
                                                {"outputs", Json::array({"${output}/basic.o"})}}})}}})}};
    }

    fs::path rootPath_;
    fs::path worktreePath_;
    fs::path sourceDirectoryPath_;
    fs::path toolchainPath_;
    fs::path outputElfPath_;
    Json manifest_;
};

class ScopedRegisteredCompilationResource final {
public:
    ScopedRegisteredCompilationResource(
        std::string resourceId, const Json& manifest, const fs::path& sourceDirectoryPath,
        std::vector<ResourceFileData> resourceFiles = {{"payload.bin", "payload.bin", {0x01U, 0x02U}}})
        : registry_(ResourceRegistry::Instance()),
          resourceId_(std::move(resourceId)),
          previousOwnedMaterializationRoot_(std::move(registry_.ownedMaterializationRoot_)),
          previousAutomaticLoadAttempted_(registry_.automaticLoadAttempted_),
          previousRetainMaterializedDirectories_(registry_.retainMaterializedDirectories_),
          previousAutomaticLoadStatus_(registry_.automaticLoadStatus_)
    {
        registry_.automaticLoadAttempted_ = true;
        registry_.retainMaterializedDirectories_ = false;
        registry_.automaticLoadStatus_ = ResourceStatus::Success;

        std::unique_ptr<ResourceEntry> resourceEntry(new ResourceEntry());
        resourceEntry->data.json = manifest;
        resourceEntry->data.sourceFilePath = sourceDirectoryPath.string();
        resourceEntry->files = std::move(resourceFiles);
        resourceEntry->sourceType = ResourceSourceType::External;
        inserted_ = registry_.externalResources_.emplace(resourceId_, std::move(resourceEntry)).second;
    }

    ~ScopedRegisteredCompilationResource()
    {
        if (inserted_) {
            registry_.externalResources_.erase(resourceId_);
        }
        registry_.ownedMaterializationRoot_ = std::move(previousOwnedMaterializationRoot_);
        registry_.automaticLoadAttempted_ = previousAutomaticLoadAttempted_;
        registry_.retainMaterializedDirectories_ = previousRetainMaterializedDirectories_;
        registry_.automaticLoadStatus_ = previousAutomaticLoadStatus_;
    }

    bool WasInserted() const noexcept { return inserted_; }

    void SetAutomaticLoadStatus(ResourceStatus loadStatus) noexcept { registry_.automaticLoadStatus_ = loadStatus; }

private:
    ResourceRegistry& registry_;
    std::string resourceId_;
    DirectoryCleanupGuard previousOwnedMaterializationRoot_;
    bool previousAutomaticLoadAttempted_{false};
    bool previousRetainMaterializedDirectories_{false};
    ResourceStatus previousAutomaticLoadStatus_{ResourceStatus::Success};
    bool inserted_{false};
};

aclrtcKernelSpecRequest CreateRequest(const uint64_t* constantByteCounts, const void* const* constantAddresses)
{
    aclrtcKernelSpecRequest request{};
    request.resourceId = RESOURCE_ID;
    request.kernelEntry = "add_custom_100000";
    request.argsCount = 1;
    request.argsAddr = constantAddresses;
    request.argsBytes = constantByteCounts;
    return request;
}

NormalizedKernelSpecializationRequest NormalizeRequestForTest(
    const aclrtcKernelSpecRequest& request, const fs::path& outputElfPath)
{
    NormalizedKernelSpecializationRequest normalizedRequest;
    EXPECT_EQ(
        ascendc::aclrtc::NormalizeKernelSpecializationRequest(request, outputElfPath.c_str(), normalizedRequest),
        ascendc::aclrtc::ACLRTC_SUCCESS);
    return normalizedRequest;
}

std::string ReadFile(const fs::path& path)
{
    std::ifstream stream(path.string());
    return std::string(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
}

TEST(KernelSpecializationSessionTest, RunsPatchCompileLinkPublishAndCleanupWithMaterializedResource)
{
    EndToEndTestResource testResource(FakeCompilerOutcome::Success);
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", testResource.ToolchainPath().c_str());
    ScopedEnvironmentVariable saveMetaEnvironment("ASCEND_OP_COMPILE_SAVE_KERNEL_META", nullptr);
    const uint8_t constantBytes[] = {0x01, 0xfe};
    const void* constantAddresses[] = {constantBytes};
    const uint64_t constantByteCounts[] = {sizeof(constantBytes)};
    aclrtcKernelSpecRequest request = CreateRequest(constantByteCounts, constantAddresses);

    KernelSpecializationSession session;
    EXPECT_EQ(
        session.RunSpecializationWithMaterializedResource(
            NormalizeRequestForTest(request, testResource.OutputElfPath()),
            testResource.CreateMaterializedCompilationResource()),
        0);
    EXPECT_EQ(ReadFile(testResource.OutputElfPath()), "linked-elf");
    EXPECT_FALSE(fs::exists(testResource.WorktreePath()));
}

TEST(KernelSpecializationSessionTest, DoesNotReplaceExistingOutputWhenCompilationFails)
{
    EndToEndTestResource testResource(FakeCompilerOutcome::Failure);
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", testResource.ToolchainPath().c_str());
    ScopedEnvironmentVariable saveMetaEnvironment("ASCEND_OP_COMPILE_SAVE_KERNEL_META", nullptr);
    std::ofstream(testResource.OutputElfPath().string()) << "previous-elf";
    const uint8_t constantBytes[] = {0x01, 0xfe};
    const void* constantAddresses[] = {constantBytes};
    const uint64_t constantByteCounts[] = {sizeof(constantBytes)};
    aclrtcKernelSpecRequest request = CreateRequest(constantByteCounts, constantAddresses);

    KernelSpecializationSession session;
    EXPECT_EQ(
        session.RunSpecializationWithMaterializedResource(
            NormalizeRequestForTest(request, testResource.OutputElfPath()),
            testResource.CreateMaterializedCompilationResource()),
        ascendc::aclrtc::ACLRTC_ERROR_COMPILATION);
    EXPECT_EQ(ReadFile(testResource.OutputElfPath()), "previous-elf");
    EXPECT_FALSE(fs::exists(testResource.WorktreePath()));
}

TEST(KernelSpecializationSessionTest, FailsWhenOutputDirectoryIsMissing)
{
    EndToEndTestResource testResource(FakeCompilerOutcome::Success);
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", testResource.ToolchainPath().c_str());
    ScopedEnvironmentVariable saveMetaEnvironment("ASCEND_OP_COMPILE_SAVE_KERNEL_META", nullptr);
    boost::system::error_code removeError;
    fs::remove_all(testResource.OutputElfPath().parent_path(), removeError);
    ASSERT_FALSE(removeError);
    const uint8_t constantBytes[] = {0x01, 0xfe};
    const void* constantAddresses[] = {constantBytes};
    const uint64_t constantByteCounts[] = {sizeof(constantBytes)};
    aclrtcKernelSpecRequest request = CreateRequest(constantByteCounts, constantAddresses);

    KernelSpecializationSession session;
    EXPECT_EQ(
        session.RunSpecializationWithMaterializedResource(
            NormalizeRequestForTest(request, testResource.OutputElfPath()),
            testResource.CreateMaterializedCompilationResource()),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
}

TEST(KernelSpecializationSessionTest, RetainsWorktreeWhenKernelMetaSavingValueHasWhitespace)
{
    EndToEndTestResource testResource(FakeCompilerOutcome::Success);
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", testResource.ToolchainPath().c_str());
    ScopedEnvironmentVariable saveMetaEnvironment("ASCEND_OP_COMPILE_SAVE_KERNEL_META", " \t1\r\n");
    const uint8_t constantBytes[] = {0x01, 0xfe};
    const void* constantAddresses[] = {constantBytes};
    const uint64_t constantByteCounts[] = {sizeof(constantBytes)};
    aclrtcKernelSpecRequest request = CreateRequest(constantByteCounts, constantAddresses);

    KernelSpecializationSession session;
    EXPECT_EQ(
        session.RunSpecializationWithMaterializedResource(
            NormalizeRequestForTest(request, testResource.OutputElfPath()),
            testResource.CreateMaterializedCompilationResource()),
        0);
    EXPECT_TRUE(fs::is_directory(testResource.WorktreePath()));
    EXPECT_TRUE(fs::is_regular_file(testResource.WorktreePath() / "aclrtc_manifest.json"));
    EXPECT_TRUE(fs::is_regular_file(testResource.WorktreePath() / "aclrtc_compile.log"));
    EXPECT_TRUE(fs::is_regular_file(testResource.WorktreePath() / "aclrtc_replay.sh"));
    EXPECT_TRUE(fs::is_regular_file(testResource.WorktreePath() / "aclrtc_result.json"));
    EXPECT_NE(
        ReadFile(testResource.WorktreePath() / "aclrtc_replay.sh").find(testResource.WorktreePath().string()),
        std::string::npos);
}

TEST(KernelSpecializationSessionTest, LoadsAndCopiesFreshMaterializedPathsFromRegistry)
{
    const fs::path testRoot = fs::temp_directory_path() / fs::unique_path("aclrtc_session_registry_%%%%-%%%%-%%%%");
    const fs::path sourceDirectoryPath = testRoot / "source";
    fs::create_directories(sourceDirectoryPath);
    ScopedEnvironmentVariable temporaryDirectoryEnvironment("TMPDIR", testRoot.c_str());
    const std::string resourceId = "aclrtc-session-registry-integration";
    const Json manifest = {{"resource_id", resourceId}, {"schema_version", "1.0"}};
    ScopedRegisteredCompilationResource registeredResource(resourceId, manifest, sourceDirectoryPath);
    ASSERT_TRUE(registeredResource.WasInserted());

    registeredResource.SetAutomaticLoadStatus(ResourceStatus::NotFound);
    MaterializedKernelCompilationResource rejectedResource;
    EXPECT_EQ(
        KernelSpecializationSession::LoadAndMaterializeCompilationResource(resourceId, rejectedResource),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);

    registeredResource.SetAutomaticLoadStatus(ResourceStatus::Success);
    MaterializedKernelCompilationResource firstResource;
    ASSERT_EQ(
        KernelSpecializationSession::LoadAndMaterializeCompilationResource(resourceId, firstResource),
        ascendc::aclrtc::ACLRTC_SUCCESS);
    EXPECT_EQ(firstResource.manifest, manifest);
    EXPECT_EQ(firstResource.externalSourceDirectoryPath, sourceDirectoryPath);
    EXPECT_TRUE(fs::is_regular_file(firstResource.ownedResourceDirectoryPath / "payload.bin"));

    MaterializedKernelCompilationResource secondResource;
    ASSERT_EQ(
        KernelSpecializationSession::LoadAndMaterializeCompilationResource(resourceId, secondResource),
        ascendc::aclrtc::ACLRTC_SUCCESS);
    EXPECT_NE(firstResource.ownedResourceDirectoryPath, secondResource.ownedResourceDirectoryPath);
    EXPECT_TRUE(fs::is_regular_file(secondResource.ownedResourceDirectoryPath / "payload.bin"));
}

TEST(KernelSpecializationApiTest, ReportsMissingRegisteredResource)
{
    const fs::path testRoot = fs::temp_directory_path() / fs::unique_path("aclrtc_session_missing_resource_%%%%-%%%%");
    fs::create_directories(testRoot);
    ScopedRegisteredCompilationResource registeredResource(
        "unrelated-resource", Json{{"schema_version", "1.0"}}, testRoot);
    ASSERT_TRUE(registeredResource.WasInserted());
    const uint8_t constantBytes[] = {0x01, 0xfe};
    const void* constantAddresses[] = {constantBytes};
    const uint64_t constantByteCounts[] = {sizeof(constantBytes)};
    aclrtcKernelSpecRequest request = CreateRequest(constantByteCounts, constantAddresses);
    request.resourceId = "missing-resource";

    EXPECT_EQ(
        aclrtcKernelSpecialization(&request, (testRoot / "kernel.elf").c_str()), ascendc::aclrtc::ACLRTC_ERROR_FAILURE);

    boost::system::error_code ignoredError;
    fs::remove_all(testRoot, ignoredError);
}

TEST(KernelSpecializationApiTest, RejectsNullPointersBeforeResourceLookup)
{
    aclrtcKernelSpecRequest request{};
    EXPECT_EQ(aclrtcKernelSpecialization(nullptr, "/tmp/kernel.elf"), ascendc::aclrtc::ACLRTC_ERROR_INVALID_INPUT);

    EXPECT_EQ(aclrtcKernelSpecialization(&request, nullptr), ascendc::aclrtc::ACLRTC_ERROR_INVALID_INPUT);

    EXPECT_EQ(aclrtcKernelSpecialization(&request, "/tmp/kernel.elf"), ascendc::aclrtc::ACLRTC_ERROR_INVALID_INPUT);
}

TEST(KernelSpecializationApiTest, CompilesRegisteredResourceThroughPublicApi)
{
    EndToEndTestResource testResource(FakeCompilerOutcome::Success);
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", testResource.ToolchainPath().c_str());
    ScopedEnvironmentVariable saveMetaEnvironment("ASCEND_OP_COMPILE_SAVE_KERNEL_META", nullptr);
    const std::vector<ResourceFileData> resourceFiles = {
        {"constants.h", "resources/include/constants.h", {'T', 'I', 'L', 'I', 'N', 'G', '=', '@', '@',
                                                          'S', 'T', 'A', 'T', 'I', 'C', '_', 'T', 'I',
                                                          'L', 'I', 'N', 'G', '@', '@', ';', '\n'}},
        {"basic.cpp", "resources/src/basic.cpp", {'/', '/', ' ', 'b', 'a', 's', 'i', 'c', '\n'}},
    };
    ScopedRegisteredCompilationResource registeredResource(
        RESOURCE_ID, testResource.Manifest(), testResource.SourceDirectoryPath(), resourceFiles);
    ASSERT_TRUE(registeredResource.WasInserted());
    const uint8_t constantBytes[] = {0x01, 0xfe};
    const void* constantAddresses[] = {constantBytes};
    const uint64_t constantByteCounts[] = {sizeof(constantBytes)};
    aclrtcKernelSpecRequest request = CreateRequest(constantByteCounts, constantAddresses);

    EXPECT_EQ(
        aclrtcKernelSpecialization(&request, testResource.OutputElfPath().c_str()), ascendc::aclrtc::ACLRTC_SUCCESS);
    EXPECT_EQ(ReadFile(testResource.OutputElfPath()), "linked-elf");
}

TEST(KernelSpecializationSessionTest, ReportsManifestPlanFailure)
{
    EndToEndTestResource testResource(FakeCompilerOutcome::Success);
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", testResource.ToolchainPath().c_str());
    ScopedEnvironmentVariable saveMetaEnvironment("ASCEND_OP_COMPILE_SAVE_KERNEL_META", nullptr);
    const uint8_t constantBytes[] = {0x01, 0xfe};
    const void* constantAddresses[] = {constantBytes};
    const uint64_t constantByteCounts[] = {sizeof(constantBytes)};
    const aclrtcKernelSpecRequest request = CreateRequest(constantByteCounts, constantAddresses);
    MaterializedKernelCompilationResource resource = testResource.CreateMaterializedCompilationResource();
    resource.manifest["schema_version"] = "unsupported";

    KernelSpecializationSession session;
    EXPECT_EQ(
        session.RunSpecializationWithMaterializedResource(
            NormalizeRequestForTest(request, testResource.OutputElfPath()), std::move(resource)),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
    EXPECT_FALSE(fs::exists(testResource.OutputElfPath()));
}

TEST(KernelSpecializationSessionTest, ReportsSourcePatchFailure)
{
    EndToEndTestResource testResource(FakeCompilerOutcome::Success);
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", testResource.ToolchainPath().c_str());
    ScopedEnvironmentVariable saveMetaEnvironment("ASCEND_OP_COMPILE_SAVE_KERNEL_META", nullptr);
    const uint8_t constantBytes[] = {0x01, 0xfe};
    const void* constantAddresses[] = {constantBytes};
    const uint64_t constantByteCounts[] = {sizeof(constantBytes)};
    const aclrtcKernelSpecRequest request = CreateRequest(constantByteCounts, constantAddresses);
    MaterializedKernelCompilationResource resource = testResource.CreateMaterializedCompilationResource();
    resource.manifest["kernels"][0]["constant_infos"][0]["template"] = "@@MISSING_TEMPLATE@@";

    KernelSpecializationSession session;
    EXPECT_EQ(
        session.RunSpecializationWithMaterializedResource(
            NormalizeRequestForTest(request, testResource.OutputElfPath()), std::move(resource)),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
    EXPECT_FALSE(fs::exists(testResource.OutputElfPath()));
}

TEST(KernelSpecializationSessionTest, RejectsMissingLinkedElfDuringPublication)
{
    const fs::path testRoot = fs::temp_directory_path() / fs::unique_path("aclrtc_publish_missing_%%%%-%%%%");
    const fs::path worktreePath = testRoot / "worktree";
    fs::create_directories(worktreePath);
    KernelCompilationWorkspace workspace(worktreePath, WorktreeRetentionPolicy::RetainAfterCompilation);
    KernelSpecializationSession session;

    EXPECT_EQ(
        session.PublishKernelElf(testRoot / "missing.elf", testRoot / "output.elf", workspace),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);

    boost::system::error_code ignoredError;
    fs::remove_all(testRoot, ignoredError);
}

TEST(KernelSpecializationSessionTest, RemovesTemporaryElfWhenPublicationTargetIsDirectory)
{
    const fs::path testRoot = fs::temp_directory_path() / fs::unique_path("aclrtc_publish_directory_%%%%-%%%%");
    const fs::path worktreePath = testRoot / "worktree";
    const fs::path linkedElfPath = testRoot / "linked.elf";
    const fs::path outputDirectoryPath = testRoot / "output.elf";
    fs::create_directories(worktreePath);
    fs::create_directories(outputDirectoryPath);
    std::ofstream(linkedElfPath.string()) << "linked-elf";
    KernelCompilationWorkspace workspace(worktreePath, WorktreeRetentionPolicy::RetainAfterCompilation);
    KernelSpecializationSession session;

    EXPECT_EQ(
        session.PublishKernelElf(linkedElfPath, outputDirectoryPath, workspace), ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
    EXPECT_TRUE(fs::is_directory(outputDirectoryPath));
    EXPECT_EQ(std::distance(fs::directory_iterator(testRoot), fs::directory_iterator()), 3);

    boost::system::error_code ignoredError;
    fs::remove_all(testRoot, ignoredError);
}

TEST(KernelSpecializationSessionTest, RemovesTemporaryElfWhenLinkedElfCannotBeRead)
{
    const fs::path testRoot = fs::temp_directory_path() / fs::unique_path("aclrtc_publish_read_failure_%%%%-%%%%");
    const fs::path linkedElfDirectory = testRoot / "linked.elf";
    fs::create_directories(linkedElfDirectory);
    KernelCompilationWorkspace workspace(testRoot / "worktree", WorktreeRetentionPolicy::RetainAfterCompilation);
    KernelSpecializationSession session;

    EXPECT_EQ(
        session.PublishKernelElf(linkedElfDirectory, testRoot / "output.elf", workspace),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
    EXPECT_EQ(std::distance(fs::directory_iterator(testRoot), fs::directory_iterator()), 1);

    boost::system::error_code ignoredError;
    fs::remove_all(testRoot, ignoredError);
}

TEST(KernelSpecializationSessionTest, RemovesTemporaryElfWhenOutputWriteFails)
{
    const fs::path testRoot = fs::temp_directory_path() / fs::unique_path("aclrtc_publish_write_failure_%%%%-%%%%");
    fs::create_directories(testRoot);
    const fs::path linkedElfPath = testRoot / "linked.elf";
    std::ofstream(linkedElfPath.string()) << "linked-elf";
    KernelCompilationWorkspace workspace(testRoot / "worktree", WorktreeRetentionPolicy::RetainAfterCompilation);
    KernelSpecializationSession session;
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

    const aclError publishResult = session.PublishKernelElf(linkedElfPath, testRoot / "output.elf", workspace);

    const int restoreLimitResult = setrlimit(RLIMIT_FSIZE, &originalFileSizeLimit);
    const int restoreSignalResult = sigaction(SIGXFSZ, &originalFileSizeSignal, nullptr);
    ASSERT_EQ(restoreLimitResult, 0);
    ASSERT_EQ(restoreSignalResult, 0);
    EXPECT_EQ(publishResult, ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
    EXPECT_FALSE(fs::exists(testRoot / "output.elf"));
    EXPECT_EQ(std::distance(fs::directory_iterator(testRoot), fs::directory_iterator()), 1);

    boost::system::error_code ignoredError;
    fs::remove_all(testRoot, ignoredError);
}

TEST(KernelSpecializationSessionTest, StopsPublicationWhenWorktreeCannotBeRemoved)
{
    const fs::path testRoot = fs::temp_directory_path() / fs::unique_path("aclrtc_publish_cleanup_failure_%%%%-%%%%");
    fs::create_directories(testRoot);
    const fs::path linkedElfPath = testRoot / "linked.elf";
    std::ofstream(linkedElfPath.string()) << "linked-elf";
    KernelCompilationWorkspace workspace("/proc/self/status", WorktreeRetentionPolicy::RemoveAfterCompilation);
    KernelSpecializationSession session;

    EXPECT_EQ(
        session.PublishKernelElf(linkedElfPath, testRoot / "output.elf", workspace),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
    EXPECT_FALSE(fs::exists(testRoot / "output.elf"));
    EXPECT_EQ(std::distance(fs::directory_iterator(testRoot), fs::directory_iterator()), 1);

    boost::system::error_code ignoredError;
    fs::remove_all(testRoot, ignoredError);
}
} // namespace
