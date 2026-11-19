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
#include "file_utils.h"
#include "specialization/compilation_manifest_parser.h"
#include "specialization/kernel_compilation_plan_builder.h"
#include "specialization/kernel_compilation_cache_input.h"
#include "specialization/kernel_compilation_workspace.h"
#include "specialization/resource_registry.h"
#include "specialization/process_compilation_context.h"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>
#include <csignal>
#include <future>
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
using namespace ascendc::aclrtc;
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
    SpecializationFlagFailure,
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
        // Each fixture represents a fresh process; calls within a test share the same cache.
        auto& cache = ascendc::aclrtc::ProcessCompilationContext::Instance();
        cache.specializationFlagConfiguration_.clear();
        cache.environmentValues_.clear();
        cache.cacheEnabled_.reset();
        cache.compilationMemoryCache_.cachedElfs_.clear();
        cache.compilationMemoryCache_.retainedBytes_ = 0U;
        cache.toolchainTimestampsRead_ = false;
        cache.toolchainTimestamps_.reset();
        (void)cache.specializationFlagDirectory_.RemoveDirectory();
        static uint64_t sequence = 0;
        rootPath_ = fs::temp_directory_path() /
                    ("aclrtc_session_test_" + std::to_string(getpid()) + "_" + std::to_string(sequence++));
        cache.diskCacheConfig_ = KernelCompilationDiskCacheConfig{rootPath_ / "cache"};
        worktreePath_ = rootPath_ / "worktree";
        sourceDirectoryPath_ = rootPath_ / "user";
        toolchainPath_ = rootPath_ / "toolchain";
        outputElfPath_ = rootPath_ / "output/kernel.elf";
        fs::create_directories(worktreePath_ / "resources/include");
        fs::create_directories(worktreePath_ / "resources/src");
        fs::create_directories(sourceDirectoryPath_);
        fs::create_directories(toolchainPath_ / "bin");
        fs::create_directories(outputElfPath_.parent_path());
        for (const char* package : {"bisheng-compiler", "asc-devkit"}) {
            const fs::path info = toolchainPath_ / "share/info" / package;
            fs::create_directories(info);
            WriteFile(info / "version.info", "timestamp=test-toolchain\n");
        }
        WriteFile(sourceDirectoryPath_ / "add_custom.cpp", "// user source\n");
        WriteFile(worktreePath_ / "resources/include/constants.h", "TILING=@@STATIC_TILING@@;\n");
        WriteFile(worktreePath_ / "resources/src/basic.cpp", "// basic source\n");
        std::string compilerScript = CreateOutputScript("object");
        if (compilerOutcome == FakeCompilerOutcome::Failure) {
            compilerScript.insert(
                compilerScript.find('\n') + 1U, "for arg in \"$@\"; do\n"
                                                "  case \"$arg\" in */basic.cpp|*/preprocessed_*.i) exit 2 ;; esac\n"
                                                "done\n");
        } else if (compilerOutcome == FakeCompilerOutcome::SpecializationFlagFailure) {
            compilerScript.insert(
                compilerScript.find('\n') + 1U,
                "for arg in \"$@\"; do\n"
                "  case \"$arg\" in */_ascendc_specialization_flag.cpp) exit 2 ;; esac\n"
                "done\n");
        }
        compilerScript.insert(
            compilerScript.find('\n') + 1U,
            "for arg in \"$@\"; do\n"
            "  case \"$arg\" in\n"
            "    */basic.cpp|*/preprocessed_*.i) printf 'compile\\n' >> \"$0.kernel-calls\"; test ! -e "
            "\"$0.kernel-fail\" || exit 2 ;;\n"
            "    */_ascendc_specialization_flag.cpp) printf 'compile\\n' >> \"$0.calls\" ;;\n"
            "  esac\n"
            "done\n");
        compilerScript.insert(
            compilerScript.find('\n') + 1U,
            "case \" $* \" in *\" -E \"*)\n"
            "  printf 'preprocess\\n' >> \"$0.preprocess-calls\"\n"
            "  test ! -e \"$0.preprocess-fail\" || exit 2\n"
            "  for arg in \"$@\"; do\n"
            "    shift\n"
            "    case \"$arg\" in --cce-aicore-arch=*) ;; *) set -- \"$@\" \"$arg\" ;; esac\n"
            "  done\n"
            "  exec /usr/bin/c++ \"$@\"\n"
            "esac\n");
        WriteExecutable(toolchainPath_ / "bin/bisheng", compilerScript);
        WriteExecutable(toolchainPath_ / "bin/ld.lld", CreateOutputScript("linked-elf"));
        manifest_ = CreateManifest();
    }

    ~EndToEndTestResource()
    {
        (void)ascendc::aclrtc::ProcessCompilationContext::Instance().specializationFlagDirectory_.RemoveDirectory();
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

    void UseElfLinkerOutput() const
    {
        std::string contents = "\\177ELF\\002\\001";
        for (size_t index = 6U; index < 64U; ++index) {
            contents += "\\000";
        }
        contents += "linked-elf";
        WriteExecutable(toolchainPath_ / "bin/ld.lld", CreateOutputScript(contents.c_str()));
    }

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
            {"kernels",
             Json::array(
                 {{{"kernel_name", "add_custom_100000"},
                   {"constant_infos", Json::array(
                                          {{{"name", "tiling"},
                                            {"parameter_index", 0},
                                            {"arg_type", "pointer"},
                                            {"byte_size", 2},
                                            {"file", "${resource}/resources/include/constants.h"},
                                            {"template", "@@STATIC_TILING@@"}}})},
                   {"link_options", Json::array({"-m", "aicorelinux", "-Ttext=0"})},
                   {"objects",
                    Json::array(
                        {{{"object_name", "basic"},
                          {"object_type", "basic"},
                          {"commands", Json::array(
                                           {{{"type", "compile"},
                                             {"stage", 0},
                                             {"cmd", Json::array(
                                                         {"${env:ASCEND_HOME_PATH}/bin/bisheng",
                                                          "${resource}/resources/src/basic.cpp", "-o",
                                                          "${output}/basic.o", "--cce-aicore-arch=dav-c220-cube"})}}})},
                          {"outputs", Json::array({"${output}/basic.o"})}}})}}})}};
    }

    ScopedEnvironmentVariable cacheDisableEnvironment_{"ASCEND_RTC_SPECIALIZE_CACHE_DISABLE", nullptr};
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

std::vector<ResourceFileData> CreateResourceFiles(const EndToEndTestResource& resource)
{
    std::vector<ResourceFileData> files;
    for (const char* relative : {"resources/include/constants.h", "resources/src/basic.cpp"}) {
        const std::string contents = ReadFile(resource.WorktreePath() / relative);
        files.push_back({fs::path(relative).filename().string(), relative, {contents.begin(), contents.end()}});
    }
    return files;
}

aclError RunWithMaterializedResource(
    const KernelSpecializationSession& session, const NormalizedKernelSpecializationRequest& request,
    MaterializedKernelCompilationResource resource)
{
    ProcessCompilationContext::Instance().cacheEnabled_ = false;
    CompilationManifest manifest;
    aclError status = CompilationManifestParser(resource.manifest)
                          .ParseSelected(request.kernelName, request.enableSuperKernel, manifest);
    PreparedKernelCompilationPlan preparedPlan;
    if (status == ACLRTC_SUCCESS) {
        status = KernelCompilationPlanBuilder(request, {}, resource.externalSourceDirectoryPath)
                     .PrepareCompilationPlan(manifest, preparedPlan);
    }
    KernelCompilationCacheInput input;
    if (status == ACLRTC_SUCCESS) {
        status = CaptureKernelConstants(request, preparedPlan.constants, input.constants);
    }
    return session.RunSpecializationWithMaterializedResource(request, std::move(resource), preparedPlan, input, status);
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
        RunWithMaterializedResource(
            session, NormalizeRequestForTest(request, testResource.OutputElfPath()),
            testResource.CreateMaterializedCompilationResource()),
        0);
    EXPECT_EQ(ReadFile(testResource.OutputElfPath()), "linked-elf");
    EXPECT_FALSE(fs::exists(testResource.WorktreePath()));
}

TEST(KernelSpecializationSessionTest, PreservesCallerEnvironmentForKernelAndMetadataCompilation)
{
    EndToEndTestResource testResource(FakeCompilerOutcome::Success);
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", testResource.ToolchainPath().c_str());
    ScopedEnvironmentVariable saveMetaEnvironment("ASCEND_OP_COMPILE_SAVE_KERNEL_META", nullptr);
    ScopedEnvironmentVariable homeEnvironment("HOME", testResource.SourceDirectoryPath().c_str());
    ScopedEnvironmentVariable temporaryDirectoryEnvironment("TMPDIR", testResource.SourceDirectoryPath().c_str());
    ScopedEnvironmentVariable inheritedEnvironment("ACLRTC_TEST_INHERITED", "caller-value");
    const fs::path compilerPath = testResource.ToolchainPath() / "bin/bisheng";
    std::string compilerScript = ReadFile(compilerPath);
    compilerScript.insert(
        compilerScript.find('\n') + 1U,
        "for arg in \"$@\"; do\n"
        "  case \"$arg\" in\n"
        "    */basic.cpp|*/preprocessed_*.i) suffix=kernel ;;\n"
        "    */_ascendc_specialization_flag.cpp) suffix=metadata ;;\n"
        "    *) continue ;;\n"
        "  esac\n"
        "  printf '%s|%s|%s' \"$HOME\" \"$TMPDIR\" \"$ACLRTC_TEST_INHERITED\" > \"$0.env-$suffix\"\n"
        "done\n");
    std::ofstream(compilerPath.string()) << compilerScript;
    const uint8_t constantBytes[] = {0x01, 0xfe};
    const void* constantAddresses[] = {constantBytes};
    const uint64_t constantByteCounts[] = {sizeof(constantBytes)};
    const auto request =
        NormalizeRequestForTest(CreateRequest(constantByteCounts, constantAddresses), testResource.OutputElfPath());

    KernelSpecializationSession session;
    ASSERT_EQ(
        RunWithMaterializedResource(session, request, testResource.CreateMaterializedCompilationResource()),
        ACLRTC_SUCCESS);
    const std::string expected = testResource.SourceDirectoryPath().string() + "|" +
                                 testResource.SourceDirectoryPath().string() + "|caller-value";
    EXPECT_EQ(ReadFile(compilerPath.string() + ".env-kernel"), expected);
    EXPECT_EQ(ReadFile(compilerPath.string() + ".env-metadata"), expected);
}

TEST(KernelSpecializationSessionTest, GeneratesAndLinksStaticSpecializationFlag)
{
    EndToEndTestResource testResource(FakeCompilerOutcome::Success);
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", testResource.ToolchainPath().c_str());
    ScopedEnvironmentVariable saveMetaEnvironment("ASCEND_OP_COMPILE_SAVE_KERNEL_META", "1");
    const uint8_t constantBytes[] = {0x01, 0xfe};
    const void* constantAddresses[] = {constantBytes};
    const uint64_t constantByteCounts[] = {sizeof(constantBytes)};
    const auto request =
        NormalizeRequestForTest(CreateRequest(constantByteCounts, constantAddresses), testResource.OutputElfPath());

    KernelSpecializationSession session;
    ASSERT_EQ(
        RunWithMaterializedResource(session, request, testResource.CreateMaterializedCompilationResource()),
        ascendc::aclrtc::ACLRTC_SUCCESS);
    const fs::path outputDirectory =
        ascendc::aclrtc::ProcessCompilationContext::Instance().specializationFlagDirectory_.GetDirectoryPath();
    EXPECT_FALSE(ascendc::FileUtils::IsPathWithin(outputDirectory.string(), testResource.WorktreePath().string()));
    const fs::path sourcePath = outputDirectory / "_ascendc_specialization_flag.cpp";
    ASSERT_TRUE(fs::is_regular_file(sourcePath));
    const std::string source = ReadFile(sourcePath);
    EXPECT_NE(source.find("section(\".ascend.meta\")"), std::string::npos);
    EXPECT_NE(source.find("BinaryMetaSpecializationFlag"), std::string::npos);
    EXPECT_NE(source.find("{{B_TYPE_SPECIALIZATION_FLAG, sizeof(uint32_t)}, 0}"), std::string::npos);
    EXPECT_TRUE(fs::is_regular_file(outputDirectory / "_ascendc_specialization_flag.o"));
    const std::string replay = ReadFile(testResource.WorktreePath() / "aclrtc_replay.sh");
    const size_t linkPosition = replay.find((testResource.ToolchainPath() / "bin/ld.lld").string());
    ASSERT_NE(linkPosition, std::string::npos);
    EXPECT_NE(
        replay.substr(linkPosition).find((outputDirectory / "_ascendc_specialization_flag.o").string()),
        std::string::npos);
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
        RunWithMaterializedResource(
            session, NormalizeRequestForTest(request, testResource.OutputElfPath()),
            testResource.CreateMaterializedCompilationResource()),
        ascendc::aclrtc::ACLRTC_ERROR_COMPILATION);
    const fs::path metadataDirectory =
        ascendc::aclrtc::ProcessCompilationContext::Instance().specializationFlagDirectory_.GetDirectoryPath();
    EXPECT_TRUE(fs::is_regular_file(metadataDirectory / "_ascendc_specialization_flag.o"));
    EXPECT_EQ(ReadFile(testResource.OutputElfPath()), "previous-elf");
    EXPECT_FALSE(fs::exists(testResource.WorktreePath()));
}

TEST(KernelSpecializationSessionTest, PreservesOutputWhenSpecializationFlagCompilationFails)
{
    EndToEndTestResource testResource(FakeCompilerOutcome::SpecializationFlagFailure);
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", testResource.ToolchainPath().c_str());
    ScopedEnvironmentVariable saveMetaEnvironment("ASCEND_OP_COMPILE_SAVE_KERNEL_META", nullptr);
    std::ofstream(testResource.OutputElfPath().string()) << "previous-elf";
    const uint8_t constantBytes[] = {1, 2};
    const void* constantAddresses[] = {constantBytes};
    const uint64_t constantByteCounts[] = {sizeof(constantBytes)};
    const auto request =
        NormalizeRequestForTest(CreateRequest(constantByteCounts, constantAddresses), testResource.OutputElfPath());

    KernelSpecializationSession session;
    EXPECT_EQ(
        RunWithMaterializedResource(session, request, testResource.CreateMaterializedCompilationResource()),
        ascendc::aclrtc::ACLRTC_ERROR_COMPILATION);
    EXPECT_TRUE(
        ascendc::aclrtc::ProcessCompilationContext::Instance().specializationFlagDirectory_.GetDirectoryPath().empty());
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
        RunWithMaterializedResource(
            session, NormalizeRequestForTest(request, testResource.OutputElfPath()),
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
        RunWithMaterializedResource(
            session, NormalizeRequestForTest(request, testResource.OutputElfPath()),
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

TEST(KernelSpecializationSessionTest, LoadsResourceWithoutMaterializingItsFiles)
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
    const ResourceEntry* resource = nullptr;
    EXPECT_EQ(
        KernelSpecializationSession::LoadCompilationResource(resourceId, resource),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);

    registeredResource.SetAutomaticLoadStatus(ResourceStatus::Success);
    EXPECT_EQ(KernelSpecializationSession::LoadCompilationResource("missing-resource", resource), ACLRTC_ERROR_FAILURE);
    ASSERT_EQ(KernelSpecializationSession::LoadCompilationResource(resourceId, resource), ACLRTC_SUCCESS);
    EXPECT_EQ(resource->data.json, manifest);
    EXPECT_TRUE(ResourceRegistry::Instance().ownedMaterializationRoot_.GetDirectoryPath().empty());

    MaterializedKernelCompilationResource firstResource;
    ASSERT_EQ(
        KernelSpecializationSession::MaterializeCompilationResource(resourceId, *resource, firstResource),
        ascendc::aclrtc::ACLRTC_SUCCESS);
    EXPECT_EQ(firstResource.manifest, manifest);
    EXPECT_EQ(firstResource.externalSourceDirectoryPath, sourceDirectoryPath);
    EXPECT_TRUE(fs::is_regular_file(firstResource.ownedResourceDirectoryPath / "payload.bin"));

    MaterializedKernelCompilationResource secondResource;
    ASSERT_EQ(
        KernelSpecializationSession::MaterializeCompilationResource(resourceId, *resource, secondResource),
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

    for (size_t invocation = 0; invocation < 2U; ++invocation) {
        EXPECT_EQ(
            aclrtcKernelSpecialization(&request, testResource.OutputElfPath().c_str()),
            ascendc::aclrtc::ACLRTC_SUCCESS);
        EXPECT_EQ(ReadFile(testResource.OutputElfPath()), "linked-elf");
    }
    EXPECT_EQ(ReadFile(testResource.ToolchainPath() / "bin/bisheng.calls"), "compile\n");
}

class KernelSpecializationCacheTest : public testing::Test {
protected:
    void RegisterResource(const std::string& source = {})
    {
        auto files = CreateResourceFiles(resource);
        if (!source.empty()) {
            files[1].bytes.assign(source.begin(), source.end());
        }
        registered = std::make_unique<ScopedRegisteredCompilationResource>(
            RESOURCE_ID, resource.Manifest(), resource.SourceDirectoryPath(), std::move(files));
        ASSERT_TRUE(registered->WasInserted());
    }

    aclError Specialize() { return aclrtcKernelSpecialization(&request, resource.OutputElfPath().c_str()); }

    void ClearMemoryCache()
    {
        auto& cache = ProcessCompilationContext::Instance().compilationMemoryCache_;
        cache.cachedElfs_.clear();
        cache.retainedBytes_ = 0U;
    }

    void ExpectCacheHit(const fs::path& output, const std::string& expected)
    {
        ASSERT_EQ(aclrtcKernelSpecialization(&request, output.c_str()), ACLRTC_SUCCESS);
        EXPECT_EQ(ReadFile(output), expected);
        EXPECT_EQ(ReadFile(resource.ToolchainPath() / "bin/bisheng.kernel-calls"), "compile\n");
    }

    EndToEndTestResource resource{FakeCompilerOutcome::Success};
    ScopedEnvironmentVariable home{"ASCEND_HOME_PATH", resource.ToolchainPath().c_str()};
    ScopedEnvironmentVariable diagnostics{"ASCEND_OP_COMPILE_SAVE_KERNEL_META", nullptr};
    std::unique_ptr<ScopedRegisteredCompilationResource> registered;
    uint8_t bytes[3]{1U, 2U, 3U};
    const void* addresses[1]{bytes};
    const uint64_t sizes[1]{sizeof(bytes)};
    aclrtcKernelSpecRequest request = CreateRequest(sizes, addresses);
};

TEST_F(KernelSpecializationCacheTest, SkipsCacheIdentityAndPreprocessingWithCacheDisabled)
{
    ScopedEnvironmentVariable cacheDisabled("ASCEND_RTC_SPECIALIZE_CACHE_DISABLE", "1");
    ScopedEnvironmentVariable diagnostics("ASCEND_OP_COMPILE_SAVE_KERNEL_META", "1");
    RegisterResource();

    ASSERT_EQ(Specialize(), ACLRTC_SUCCESS);
    ASSERT_EQ(setenv("ASCEND_RTC_SPECIALIZE_CACHE_DISABLE", "0", 1), 0);
    ASSERT_EQ(Specialize(), ACLRTC_SUCCESS);
    EXPECT_EQ(ReadFile(resource.ToolchainPath() / "bin/bisheng.kernel-calls"), "compile\ncompile\n");

    const auto& context = ProcessCompilationContext::Instance();
    EXPECT_FALSE(context.toolchainTimestampsRead_);
    EXPECT_TRUE(context.compilationMemoryCache_.cachedElfs_.empty());
    EXPECT_FALSE(fs::exists(context.diskCacheConfig_->directory));
    EXPECT_FALSE(fs::exists(resource.ToolchainPath() / "bin/bisheng.preprocess-calls"));
    EXPECT_EQ(ReadFile(resource.OutputElfPath()), "linked-elf");
    const fs::path externalDirectory =
        fs::path(ResourceRegistry::Instance().ownedMaterializationRoot_.GetDirectoryPath()) / "external";
    const fs::directory_iterator worktree(externalDirectory);
    ASSERT_NE(worktree, fs::directory_iterator());
    EXPECT_TRUE(fs::is_regular_file(worktree->path() / "aclrtc_replay.sh"));
    EXPECT_FALSE(fs::exists(worktree->path() / "aclrtc_cache_inputs.json"));
    EXPECT_EQ(ReadFile(worktree->path() / "resources/include/constants.h"), "TILING={0x01, 0x02};\n");
}

TEST_F(KernelSpecializationCacheTest, ReusesMemoryAfterPreprocessingWithoutCompilingOnHit)
{
    RegisterResource();
    EXPECT_EQ(
        aclrtcKernelSpecialization(&request, resource.OutputElfPath().parent_path().c_str()), ACLRTC_ERROR_FAILURE);
    ASSERT_EQ(Specialize(), ACLRTC_SUCCESS);
    const fs::path secondOutput = resource.OutputElfPath().parent_path() / "second.elf";
    ExpectCacheHit(secondOutput, "linked-elf");
    {
        ScopedEnvironmentVariable saveMeta("ASCEND_OP_COMPILE_SAVE_KERNEL_META", "1");
        ExpectCacheHit(secondOutput, "linked-elf");
        ExpectCacheHit(secondOutput, "linked-elf");
    }
    ExpectCacheHit(secondOutput, "linked-elf");
    request.argsCount = 0U;
    EXPECT_EQ(aclrtcKernelSpecialization(&request, secondOutput.c_str()), ACLRTC_ERROR_INVALID_INPUT);
    EXPECT_EQ(ReadFile(secondOutput), "linked-elf");
}

TEST_F(KernelSpecializationCacheTest, ReusesDiskAfterPreprocessingAndRetainsTheLoadedArtifactInMemory)
{
    RegisterResource();
    resource.UseElfLinkerOutput();
    ASSERT_EQ(Specialize(), ACLRTC_SUCCESS);
    const std::string expected = ReadFile(resource.OutputElfPath());
    auto& context = ProcessCompilationContext::Instance();
    const fs::path secondOutput = resource.OutputElfPath().parent_path() / "disk.elf";
    for (const char* saveMeta : {"0", "1"}) {
        ScopedEnvironmentVariable mode("ASCEND_OP_COMPILE_SAVE_KERNEL_META", saveMeta);
        ClearMemoryCache();
        ExpectCacheHit(secondOutput, expected);
        const fs::path external =
            fs::path(ResourceRegistry::Instance().ownedMaterializationRoot_.GetDirectoryPath()) / "external";
        EXPECT_EQ(fs::is_empty(external), saveMeta[0] == '0');
    }
    fs::remove_all(context.GetDiskCacheConfig().directory);
    ExpectCacheHit(resource.OutputElfPath(), expected);
}

class KernelSpecializationSourceCacheTest : public KernelSpecializationCacheTest,
                                            public testing::WithParamInterface<bool> {
protected:
    aclError SpecializeWithCacheMode()
    {
        if (GetParam()) {
            ClearMemoryCache();
        }
        return Specialize();
    }
};

TEST_P(KernelSpecializationSourceCacheTest, RevalidatesExternalSourcesAndHeadersBeforeMemoryAndDiskHits)
{
    resource.UseElfLinkerOutput();
    const fs::path source = resource.SourceDirectoryPath() / "add_custom.cpp";
    const fs::path header = resource.SourceDirectoryPath() / "config.h";
    const fs::path nested = resource.SourceDirectoryPath() / "nested.h";
    std::ofstream(source.string()) << "#include \"config.h\"\nint value = VALUE;\n";
    std::ofstream(header.string()) << "#include \"nested.h\"\n";
    std::ofstream(nested.string()) << "#define VALUE 1\n";
    const std::string wrapper = "#include \"" + source.string() + "\"\n";
    RegisterResource(wrapper);
    const fs::path calls = resource.ToolchainPath() / "bin/bisheng.kernel-calls";

    ASSERT_EQ(SpecializeWithCacheMode(), ACLRTC_SUCCESS);
    ASSERT_EQ(SpecializeWithCacheMode(), ACLRTC_SUCCESS);
    EXPECT_EQ(ReadFile(calls), "compile\n");
    std::ofstream(nested.string()) << "#define VALUE 2\n";
    ASSERT_EQ(SpecializeWithCacheMode(), ACLRTC_SUCCESS);
    EXPECT_EQ(ReadFile(calls), "compile\ncompile\n");
    ASSERT_EQ(SpecializeWithCacheMode(), ACLRTC_SUCCESS);
    EXPECT_EQ(ReadFile(calls), "compile\ncompile\n");
    std::ofstream(source.string()) << "#include \"config.h\"\nint value = VALUE + 1;\n";
    ASSERT_EQ(SpecializeWithCacheMode(), ACLRTC_SUCCESS);
    EXPECT_EQ(ReadFile(calls), "compile\ncompile\ncompile\n");
    std::ofstream(source.string()) << "#include \"config.h\"\nint value = VALUE;\n";
    std::ofstream(nested.string()) << "#define VALUE 1\n";
    ASSERT_EQ(SpecializeWithCacheMode(), ACLRTC_SUCCESS);
    EXPECT_EQ(ReadFile(calls), "compile\ncompile\ncompile\n");
}

TEST_P(KernelSpecializationSourceCacheTest, CompilesTheHashedSnapshotWhenHeaderChangesAfterPreprocessing)
{
    resource.UseElfLinkerOutput();
    const fs::path header = resource.SourceDirectoryPath() / "config.h";
    std::ofstream(header.string()) << "#define VALUE 1\n";
    const std::string source = "#include \"" + header.string() + "\"\nint value = VALUE;\n";
    RegisterResource(source);

    const fs::path compiler = resource.ToolchainPath() / "bin/bisheng";
    std::string script = ReadFile(compiler);
    const std::string preprocess = "  exec /usr/bin/c++ \"$@\"\n";
    const auto position = script.find(preprocess);
    ASSERT_NE(position, std::string::npos);
    script.replace(
        position, preprocess.size(),
        "  /usr/bin/c++ \"$@\" || exit $?\n"
        "  if test -f \"$0.change-header\"; then\n"
        "    printf '#define VALUE 2\\n' > \"$(cat \"$0.change-header\")\"\n"
        "    rm \"$0.change-header\"\n"
        "  fi\n"
        "  exit 0\n");
    script.insert(
        script.find("\nesac\n") + 6U, "for arg in \"$@\"; do\n"
                                      "  case \"$arg\" in */preprocessed_*.i) cat \"$arg\" > \"$0.snapshot\" ;; esac\n"
                                      "done\n");
    std::ofstream(compiler.string()) << script;
    std::ofstream((compiler.string() + ".change-header")) << header.string();

    ASSERT_EQ(SpecializeWithCacheMode(), ACLRTC_SUCCESS);
    EXPECT_EQ(ReadFile(header), "#define VALUE 2\n");
    EXPECT_NE(ReadFile(compiler.string() + ".snapshot").find("int value = 1;"), std::string::npos);
    ASSERT_EQ(SpecializeWithCacheMode(), ACLRTC_SUCCESS);
    EXPECT_NE(ReadFile(compiler.string() + ".snapshot").find("int value = 2;"), std::string::npos);
    std::ofstream(header.string()) << "#define VALUE 1\n";
    fs::remove(compiler.string() + ".snapshot");
    ASSERT_EQ(SpecializeWithCacheMode(), ACLRTC_SUCCESS);
    EXPECT_FALSE(fs::exists(compiler.string() + ".snapshot"));
    EXPECT_EQ(ReadFile(compiler.string() + ".kernel-calls"), "compile\ncompile\n");
}

INSTANTIATE_TEST_SUITE_P(MemoryAndDisk, KernelSpecializationSourceCacheTest, testing::Bool());

TEST_F(KernelSpecializationCacheTest, RevalidatesIncludeSearchResultsAndForcedHeaders)
{
    const fs::path first = resource.SourceDirectoryPath() / "first";
    const fs::path second = resource.SourceDirectoryPath() / "second";
    const fs::path forced = resource.SourceDirectoryPath() / "forced.h";
    fs::create_directory(first);
    fs::create_directory(second);
    std::ofstream((second / "config.h").string()) << "#define VALUE 1\n";
    std::ofstream(forced.string()) << "#define FORCED 1\n";
    const std::string source = "#include <config.h>\nint value = VALUE + FORCED;\n";
    RegisterResource(source);
    const std::string firstOption = "-I" + first.string();
    const std::string secondOption = "-I" + second.string();
    const std::string forcedOption = "-include" + forced.string();
    const char* options[] = {firstOption.c_str(), secondOption.c_str(), forcedOption.c_str()};
    request.options = options;
    request.optionCount = 3U;
    const fs::path calls = resource.ToolchainPath() / "bin/bisheng.kernel-calls";

    ASSERT_EQ(Specialize(), ACLRTC_SUCCESS);
    std::ofstream((first / "config.h").string()) << "#define VALUE 2\n";
    ASSERT_EQ(Specialize(), ACLRTC_SUCCESS);
    EXPECT_EQ(ReadFile(calls), "compile\ncompile\n");
    std::ofstream(forced.string()) << "#define FORCED 2\n";
    ASSERT_EQ(Specialize(), ACLRTC_SUCCESS);
    EXPECT_EQ(ReadFile(calls), "compile\ncompile\ncompile\n");
}

TEST_F(KernelSpecializationCacheTest, BypassesExistingCachesWhenPreprocessingFails)
{
    RegisterResource();
    resource.UseElfLinkerOutput();
    ASSERT_EQ(Specialize(), ACLRTC_SUCCESS);
    std::ofstream((resource.ToolchainPath() / "bin/bisheng.preprocess-fail").string()).close();
    ASSERT_EQ(Specialize(), ACLRTC_SUCCESS);
    auto& cache = ProcessCompilationContext::Instance().compilationMemoryCache_;
    cache.cachedElfs_.clear();
    cache.retainedBytes_ = 0U;
    ASSERT_EQ(Specialize(), ACLRTC_SUCCESS);
    EXPECT_EQ(ReadFile(resource.ToolchainPath() / "bin/bisheng.kernel-calls"), "compile\ncompile\ncompile\n");
    EXPECT_TRUE(cache.cachedElfs_.empty());
}

TEST_F(KernelSpecializationCacheTest, DeliversSuccessfulCompilationWhenDiskPublicationFails)
{
    RegisterResource();
    resource.UseElfLinkerOutput();
    auto& context = ProcessCompilationContext::Instance();
    context.diskCacheConfig_->directory = resource.ToolchainPath() / "bin/bisheng";
    ASSERT_EQ(Specialize(), ACLRTC_SUCCESS);
    const std::string expected = ReadFile(resource.OutputElfPath());
    ASSERT_EQ(Specialize(), ACLRTC_SUCCESS);
    EXPECT_EQ(ReadFile(resource.OutputElfPath()), expected);
    EXPECT_EQ(ReadFile(resource.ToolchainPath() / "bin/bisheng.kernel-calls"), "compile\n");
}

TEST_F(KernelSpecializationCacheTest, AppliesDiskCacheCleanupConfigurationAfterPublication)
{
    RegisterResource();
    resource.UseElfLinkerOutput();
    auto& context = ProcessCompilationContext::Instance();
    context.diskCacheConfig_->maximumSizeBytes = 1U;
    context.diskCacheConfig_->cleanupRatio = 100U;
    ASSERT_EQ(Specialize(), ACLRTC_SUCCESS);
    ClearMemoryCache();
    ASSERT_EQ(Specialize(), ACLRTC_SUCCESS);
    EXPECT_EQ(ReadFile(resource.ToolchainPath() / "bin/bisheng.kernel-calls"), "compile\ncompile\n");
}

TEST_F(KernelSpecializationCacheTest, RecompilesChangedConstantsAndReusesTheOriginalValue)
{
    RegisterResource();
    ASSERT_EQ(Specialize(), ACLRTC_SUCCESS);
    bytes[2] = 4U;
    ASSERT_EQ(Specialize(), ACLRTC_SUCCESS);
    bytes[0] = 5U;
    ASSERT_EQ(Specialize(), ACLRTC_SUCCESS);
    bytes[0] = 1U;
    ASSERT_EQ(Specialize(), ACLRTC_SUCCESS);
    EXPECT_EQ(ReadFile(resource.ToolchainPath() / "bin/bisheng.kernel-calls"), "compile\ncompile\n");
}

TEST_F(KernelSpecializationCacheTest, DeliversConcurrentOutputsIndependentlyAndReusesCompletedCompilation)
{
    RegisterResource();
    std::promise<void> start;
    const auto ready = start.get_future().share();
    std::vector<std::future<aclError>> results;
    for (size_t index = 0U; index < 8U; ++index) {
        results.push_back(std::async(std::launch::async, [&, index]() {
            ready.wait();
            const fs::path output = index == 0U ?
                                        resource.OutputElfPath().parent_path() :
                                        resource.OutputElfPath().parent_path() / (std::to_string(index) + ".elf");
            return aclrtcKernelSpecialization(&request, output.c_str());
        }));
    }
    start.set_value();
    for (size_t index = 0U; index < results.size(); ++index) {
        EXPECT_EQ(results[index].get(), index == 0U ? ACLRTC_ERROR_FAILURE : ACLRTC_SUCCESS);
        if (index != 0U) {
            EXPECT_EQ(
                ReadFile(resource.OutputElfPath().parent_path() / (std::to_string(index) + ".elf")), "linked-elf");
        }
    }
    const std::string compilerCalls = ReadFile(resource.ToolchainPath() / "bin/bisheng.kernel-calls");
    EXPECT_FALSE(compilerCalls.empty());
    ASSERT_EQ(Specialize(), ACLRTC_SUCCESS);
    EXPECT_EQ(ReadFile(resource.ToolchainPath() / "bin/bisheng.kernel-calls"), compilerCalls);
}

TEST_F(KernelSpecializationCacheTest, RetriesCompilationAfterFailureWithoutReplacingTheOutput)
{
    RegisterResource();
    const fs::path marker = resource.ToolchainPath() / "bin/bisheng.kernel-fail";
    std::ofstream(marker.string()).close();
    std::ofstream(resource.OutputElfPath().string()) << "original";
    EXPECT_EQ(Specialize(), ACLRTC_ERROR_COMPILATION);
    EXPECT_EQ(ReadFile(resource.OutputElfPath()), "original");
    fs::remove(marker);
    ASSERT_EQ(Specialize(), ACLRTC_SUCCESS);
    ASSERT_EQ(Specialize(), ACLRTC_SUCCESS);
    EXPECT_EQ(ReadFile(resource.ToolchainPath() / "bin/bisheng.kernel-calls"), "compile\ncompile\n");
}

TEST_F(KernelSpecializationCacheTest, BypassesMemoryWhenToolchainIdentityIsUnavailable)
{
    RegisterResource();
    fs::remove(resource.ToolchainPath() / "share/info/asc-devkit/version.info");
    ASSERT_EQ(Specialize(), ACLRTC_SUCCESS);
    ASSERT_EQ(Specialize(), ACLRTC_SUCCESS);
    EXPECT_EQ(ReadFile(resource.ToolchainPath() / "bin/bisheng.kernel-calls"), "compile\ncompile\n");
}

TEST_F(KernelSpecializationCacheTest, ExportsOnlyCacheInputsOnMissAndMemoryOrDiskHitsWithKernelMeta)
{
    RegisterResource();
    resource.UseElfLinkerOutput();
    ScopedEnvironmentVariable diagnostics("ASCEND_OP_COMPILE_SAVE_KERNEL_META", "1");
    ASSERT_EQ(Specialize(), ACLRTC_SUCCESS);
    const std::string expected = ReadFile(resource.OutputElfPath());
    const fs::path externalDirectory =
        fs::path(ResourceRegistry::Instance().ownedMaterializationRoot_.GetDirectoryPath()) / "external";
    Json cacheInputs;
    for (size_t invocation = 0U; invocation < 3U; ++invocation) {
        SCOPED_TRACE(invocation);
        if (invocation == 2U) {
            ClearMemoryCache();
        }
        if (invocation != 0U) {
            ExpectCacheHit(resource.OutputElfPath(), expected);
        }
        ASSERT_EQ(std::distance(fs::directory_iterator(externalDirectory), fs::directory_iterator()), invocation + 1U);
        for (const auto& directory : fs::directory_iterator(externalDirectory)) {
            EXPECT_EQ(std::distance(fs::directory_iterator(directory.path()), fs::directory_iterator()), 1);
            const Json current = Json::parse(ReadFile(directory.path() / "aclrtc_cache_inputs.json"));
            EXPECT_EQ(current.at("key").get<std::string>().size(), 64U);
            EXPECT_EQ(current.at("bindings")[0].at("size"), "2");
            EXPECT_FALSE(current.at("bindings")[0].contains("bytes"));
            if (cacheInputs.is_null()) {
                cacheInputs = current;
            }
            EXPECT_EQ(current, cacheInputs);
        }
    }
}

TEST_F(KernelSpecializationCacheTest, ExportsOnlyCacheInputsWhenCompilationFailsWithCacheEnabled)
{
    std::ofstream((resource.ToolchainPath() / "bin/bisheng.kernel-fail").string()).close();
    ScopedEnvironmentVariable diagnostics("ASCEND_OP_COMPILE_SAVE_KERNEL_META", "1");
    RegisterResource();
    EXPECT_EQ(Specialize(), ACLRTC_ERROR_COMPILATION);
    EXPECT_FALSE(fs::exists(resource.OutputElfPath()));
    const fs::path externalDirectory =
        fs::path(ResourceRegistry::Instance().ownedMaterializationRoot_.GetDirectoryPath()) / "external";
    ASSERT_TRUE(fs::is_directory(externalDirectory));
    const fs::directory_iterator worktree(externalDirectory);
    ASSERT_NE(worktree, fs::directory_iterator());
    EXPECT_EQ(std::distance(fs::directory_iterator(externalDirectory), fs::directory_iterator()), 1);
    EXPECT_EQ(std::distance(fs::directory_iterator(worktree->path()), fs::directory_iterator()), 1);
    const Json cacheInputs = Json::parse(ReadFile(worktree->path() / "aclrtc_cache_inputs.json"));
    EXPECT_EQ(cacheInputs.at("key").get<std::string>().size(), 64U);
    EXPECT_TRUE(ProcessCompilationContext::Instance().compilationMemoryCache_.cachedElfs_.empty());
}

TEST(KernelSpecializationApiTest, SkipsMaterializationWhenPreparationFails)
{
    EndToEndTestResource testResource(FakeCompilerOutcome::Success);
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", testResource.ToolchainPath().c_str());
    for (const bool missingKernel : {true, false}) {
        SCOPED_TRACE(missingKernel);
        ProcessCompilationContext::Instance().cacheEnabled_ = true;
        const fs::path temporaryDirectory =
            testResource.SourceDirectoryPath().parent_path() / (missingKernel ? "missing_kernel" : "invalid_template");
        fs::create_directories(temporaryDirectory);
        ScopedEnvironmentVariable temporaryDirectoryEnvironment("TMPDIR", temporaryDirectory.c_str());
        Json manifest = testResource.Manifest();
        manifest["kernels"][0]["objects"][0]["commands"][0]["cmd"] = Json::array(
            {"${env:ASCEND_HOME_PATH}/bin/bisheng", "--cce-aicore-arch=dav-c220-cube",
             "${resource}/resources/src/basic.cpp", "${env:}", "-o", "${output}/basic.o"});
        ScopedRegisteredCompilationResource registeredResource(
            RESOURCE_ID, manifest, testResource.SourceDirectoryPath());
        ASSERT_TRUE(registeredResource.WasInserted());
        const uint8_t constantBytes[] = {0x01, 0xfe};
        const void* constantAddresses[] = {constantBytes};
        const uint64_t constantByteCounts[] = {sizeof(constantBytes)};
        aclrtcKernelSpecRequest request = CreateRequest(constantByteCounts, constantAddresses);
        if (missingKernel) {
            request.kernelEntry = "missing_kernel";
        }

        {
            ScopedEnvironmentVariable saveMetaEnvironment("ASCEND_OP_COMPILE_SAVE_KERNEL_META", nullptr);
            EXPECT_EQ(aclrtcKernelSpecialization(&request, testResource.OutputElfPath().c_str()), ACLRTC_ERROR_FAILURE);
            EXPECT_TRUE(fs::is_empty(temporaryDirectory));
        }

        ScopedEnvironmentVariable saveMetaEnvironment("ASCEND_OP_COMPILE_SAVE_KERNEL_META", "1");
        EXPECT_EQ(aclrtcKernelSpecialization(&request, testResource.OutputElfPath().c_str()), ACLRTC_ERROR_FAILURE);
        EXPECT_TRUE(fs::is_empty(temporaryDirectory));
        ProcessCompilationContext::Instance().cacheEnabled_ = false;
        EXPECT_EQ(aclrtcKernelSpecialization(&request, testResource.OutputElfPath().c_str()), ACLRTC_ERROR_FAILURE);
        EXPECT_FALSE(fs::exists(testResource.OutputElfPath()));
        const fs::path materializationRoot = ResourceRegistry::Instance().ownedMaterializationRoot_.GetDirectoryPath();
        const fs::path externalDirectory = materializationRoot / "external";
        ASSERT_TRUE(fs::is_directory(externalDirectory));
        const fs::directory_iterator worktree(externalDirectory);
        ASSERT_NE(worktree, fs::directory_iterator());
        EXPECT_TRUE(fs::is_regular_file(worktree->path() / "aclrtc_manifest.json"));
        EXPECT_TRUE(fs::is_regular_file(worktree->path() / "aclrtc_result.json"));
    }
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
        RunWithMaterializedResource(
            session, NormalizeRequestForTest(request, testResource.OutputElfPath()), std::move(resource)),
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
