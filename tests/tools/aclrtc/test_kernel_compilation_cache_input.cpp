/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "specialization/cache_digest.h"
#include "specialization/kernel_compilation_cache_input.h"
#include "specialization/kernel_compilation_preprocessor.h"
#include "specialization/process_compilation_context.h"
#include "specialization/resource_registry.h"
#include "specialization/compilation_plan_executor.h"
#include "specialization/kernel_specialization_diagnostics.h"
#include "process_executor.h"

#include <gtest/gtest.h>
#include <boost/filesystem.hpp>
#include <cstdlib>
#include <fstream>
#include <sys/mman.h>
#include <unistd.h>

namespace {
namespace fs = boost::filesystem;
using namespace ascendc::aclrtc;
using ascendc::ManifestConstant;
using ascendc::specialization_compile::ResourceEntry;
using Json = nlohmann::json;

TEST(CacheDigestTest, MatchesStandardSha256VectorsAndPaddingBoundaries)
{
    EXPECT_EQ(ComputeSha256(nullptr, 0U), "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    EXPECT_EQ(ComputeSha256("abc", 3U), "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
    const std::map<size_t, std::string> vectors = {
        {55U, "9f4390f8d30c2dd92ec9f095b65e2b9ae9b0a925a5258e241c9f1e910f734318"},
        {56U, "b35439a4ac6f0948b6d6f9e3c6af0f5f590ce20f1bde7090ef7970686ec6738a"},
        {63U, "7d3e74a05d7db15bce4ad9ec0658ea98e3f06eeecf16b4c6fff2da457ddc2f34"},
        {64U, "ffe054fe7ae0cb6dc65c3af9b61d5209f439851db43d0ba5997337df154668eb"},
        {65U, "635361c48bb9eab14198e76ea8ab7f1a41685d6ad62aa9146d301d4f17eb0ae0"},
        {1000000U, "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0"}};
    for (const auto& vector : vectors) {
        const std::string bytes(vector.first, 'a');
        EXPECT_EQ(ComputeSha256(bytes.data(), bytes.size()), vector.second) << vector.first;
    }
}

TEST(CacheDigestTest, UsesCanonicalJsonAndSeparatesDomains)
{
    const Json first{{"z", 2}, {"a", Json::array({"ab", "c"})}};
    const Json reordered = Json::parse("{\"a\":[\"ab\",\"c\"],\"z\":2}");
    EXPECT_EQ(ComputeCacheIdentityDigest("one", first), ComputeCacheIdentityDigest("one", reordered));
    EXPECT_NE(ComputeCacheIdentityDigest("one", first), ComputeCacheIdentityDigest("two", first));
    EXPECT_NE(
        ComputeCacheIdentityDigest("one", first), ComputeCacheIdentityDigest("one", {{"z", 2}, {"a", {"a", "bc"}}}));
}

class KernelCompilationCacheInputTest : public testing::Test {
protected:
    void SetUp() override
    {
        previousWorkingDirectory_ = fs::current_path();
        for (const char* name :
             {"ASCEND_HOME_PATH", "CPATH", "C_INCLUDE_PATH", "CPLUS_INCLUDE_PATH", "ACLRTC_TEST_INHERITED"}) {
            const char* value = std::getenv(name);
            previousEnvironment_[name] = value == nullptr ? std::nullopt : std::optional<std::string>(value);
        }
        root_ = fs::temp_directory_path() / fs::unique_path("aclrtc_input_%%%%-%%%%");
        for (const char* package : {"bisheng-compiler", "asc-devkit"}) {
            const fs::path directory = root_ / "share/info" / package;
            fs::create_directories(directory);
            std::ofstream((directory / "version.info").string()) << "version=1\ntimestamp=" << package << "-first\r\n";
        }
        ASSERT_EQ(setenv("ASCEND_HOME_PATH", root_.c_str(), 1), 0);
        ResetProcessContext();
        request_.kernelName = "kernel";
        request_.resourceId = "resource";
        request_.kernelArgumentCount = 2U;
        request_.borrowedKernelArgumentDataPointers = addresses_;
        request_.borrowedKernelArgumentByteCounts = sizes_;
        ManifestConstant constant;
        ASSERT_TRUE(constant.ParseDefinition(
            {{"name", "constant"},
             {"parameter_index", 0},
             {"arg_type", "pointer"},
             {"byte_size", 2},
             {"file", "${resource}/constant.h"},
             {"template", "@@C@@"}}));
        plan_.constants.push_back(constant);
        plan_.compilationCommands.push_back(
            {CompilationCommandKind::Compile,
             {},
             "basic",
             "${env:ASCEND_HOME_PATH}/bin/bisheng",
             {"${resource}/source.cpp", "-o", "${output}/basic.o"},
             {"-O2"},
             {}});
        plan_.compilationCommands.push_back(
            {CompilationCommandKind::Link,
             {},
             "link",
             "${env:ASCEND_HOME_PATH}/bin/ld.lld",
             {"${output}/basic.o"},
             {},
             {}});
        plan_.sourceDirectory = (root_ / "source").string();
        plan_.environmentValues["ASCEND_HOME_PATH"] = root_.string();
        plan_.compilerEnvironment = ProcessCompilationContext::Instance().GetCompilerEnvironment();
        resource_.data.json = {{"resource_id", "resource"}, {"kernel", "kernel"}};
        resource_.files = {{"z", "z.cpp", {1U, 2U}}, {"a", "a.h", {3U}}};
    }

    void TearDown() override
    {
        fs::current_path(previousWorkingDirectory_);
        for (const auto& variable : previousEnvironment_) {
            if (variable.second.has_value()) {
                setenv(variable.first.c_str(), variable.second->c_str(), 1);
            } else {
                unsetenv(variable.first.c_str());
            }
        }
        ResetProcessContext();
        boost::system::error_code ignored;
        fs::remove_all(root_, ignored);
    }

    void ResetProcessContext()
    {
        auto& context = ProcessCompilationContext::Instance();
        context.environmentValues_.clear();
        context.toolchainTimestampsRead_ = false;
        context.toolchainTimestamps_.reset();
    }

    KernelCompilationCacheInput Prepare()
    {
        KernelCompilationCacheInput input;
        EXPECT_EQ(CaptureKernelConstants(request_, plan_.constants, input.constants), ACLRTC_SUCCESS);
        BuildKernelCompilationCacheIdentity(request_, resource_, plan_, input);
        return input;
    }

    KernelCompilationCacheInput Preprocess(const std::string& name, const std::string& contents)
    {
        const fs::path resource = root_ / name;
        fs::create_directory(resource);
        const fs::path source = resource / "wrapper.cpp";
        std::ofstream(source.string()) << contents;
        KernelCompilationPlan plan;
        plan.compilationCommands.push_back(
            {CompilationCommandKind::Compile,
             {},
             "basic",
             "/usr/bin/c++",
             {"-E", source.string(), "-o", (resource / "wrapper.i").string()},
             {}});
        KernelCompilationCacheInput input;
        input.key = "same-request";
        input.description = {{"domains", {{"artifact", "aclrtc.artifact.v1"}}}};
        std::vector<PreprocessedCompilationSource> preprocessedSources;
        EXPECT_TRUE(AddPreprocessedSourcesToCacheInput(plan, resource, "test_kernel", input, preprocessedSources));
        EXPECT_EQ(preprocessedSources.size(), 1U);
        return input;
    }

    fs::path previousWorkingDirectory_;
    fs::path root_;
    uint8_t bytes_[4] = {1U, 2U, 3U, 4U};
    const void* addresses_[2] = {bytes_, nullptr};
    uint64_t sizes_[2] = {sizeof(bytes_), 0U};
    NormalizedKernelSpecializationRequest request_;
    PreparedKernelCompilationPlan plan_;
    ResourceEntry resource_;
    std::map<std::string, std::optional<std::string>> previousEnvironment_;
};

TEST_F(KernelCompilationCacheInputTest, IgnoresAddressesTailBytesAndPublicationPaths)
{
    const auto first = Prepare();
    ASSERT_EQ(first.key.size(), 64U);
    const uint8_t samePrefix[] = {1U, 2U, 99U, 88U};
    addresses_[0] = samePrefix;
    request_.specializationSessionId = "another-session";
    request_.outputElfPath = "/another/output.elf";
    plan_.compilationCommands[0].diagnosticLabel = "another-label";
    plan_.compilationCommands[0].parallelStage = 17U;
    request_.compilerOptions.superKernelOptions = {"-DUNUSED=1"};
    const auto second = Prepare();
    EXPECT_EQ(first.key, second.key);
    EXPECT_EQ(first.description, second.description);
    EXPECT_EQ(first.constants.argumentBytes.at(0), (std::vector<uint8_t>{1U, 2U}));
    EXPECT_EQ(first.description.at("resource_inputs").at("files")[0].at("path"), "a.h");
    EXPECT_FALSE(fs::exists(root_ / "source"));
}

TEST_F(KernelCompilationCacheInputTest, DetectsEffectiveConstantAndCompilationChanges)
{
    const auto first = Prepare();
    bytes_[0] = 9U;
    const auto changedConstant = Prepare();
    EXPECT_EQ(first.baseKey, changedConstant.baseKey);
    EXPECT_NE(first.key, changedConstant.key);
    EXPECT_EQ(first.constants.argumentBytes.at(0)[0], 1U);
    bytes_[0] = 1U;
    plan_.compilationCommands[0].compilerOptions = {"-O3"};
    EXPECT_NE(first.baseKey, Prepare().baseKey);
    plan_.compilationCommands[0].compilerOptions = {"-O2"};
    plan_.compilationCommands.back().arguments.push_back("${output}/extra.o");
    EXPECT_NE(first.key, Prepare().key);
    plan_.compilationCommands.back().arguments.pop_back();
    plan_.environmentValues["ASCEND_HOME_PATH"] += "/other";
    EXPECT_NE(first.key, Prepare().key);
    plan_.environmentValues["ASCEND_HOME_PATH"] = root_.string();
    plan_.compilerEnvironment["CPATH"] = "other/include";
    EXPECT_NE(first.key, Prepare().key);
    plan_.compilerEnvironment["CPATH"] = std::nullopt;
    const auto unset = Prepare();
    plan_.compilerEnvironment["CPATH"] = "";
    EXPECT_NE(unset.key, Prepare().key);
}

TEST_F(KernelCompilationCacheInputTest, IncludesEntrySourcePathAndSymbolRename)
{
    const auto first = Prepare();
    request_.kernelName = "another_kernel";
    EXPECT_NE(first.key, Prepare().key);
    request_.kernelName = "kernel";
    plan_.sourceDirectory += "/other";
    EXPECT_NE(first.key, Prepare().key);
    plan_.sourceDirectory = (root_ / "source").string();
    plan_.compilationCommands.back().kernelSymbolRename = KernelSymbolRename{"kernel", "renamed"};
    EXPECT_NE(first.key, Prepare().key);
}

TEST_F(KernelCompilationCacheInputTest, IncludesWorkingDirectoryForRelativeCompilationInputs)
{
    const fs::path firstDirectory = root_ / "first";
    const fs::path secondDirectory = root_ / "second";
    fs::create_directories(firstDirectory / "inc");
    fs::create_directories(secondDirectory / "inc");
    std::ofstream((firstDirectory / "inc/config.h").string()) << "#define VALUE 1\n";
    std::ofstream((secondDirectory / "inc/config.h").string()) << "#define VALUE 2\n";
    plan_.compilationCommands[0].compilerOptions.push_back("-Iinc");

    fs::current_path(firstDirectory);
    const auto first = Prepare();
    EXPECT_EQ(first.description.at("base_inputs").at("working_directory"), firstDirectory.string());
    EXPECT_EQ(first.key, Prepare().key);

    fs::current_path(secondDirectory);
    const auto second = Prepare();
    EXPECT_EQ(second.description.at("base_inputs").at("working_directory"), secondDirectory.string());
    EXPECT_NE(first.baseKey, second.baseKey);
    EXPECT_NE(first.key, second.key);

    fs::current_path(firstDirectory);
    EXPECT_EQ(first.key, Prepare().key);
}

TEST_F(KernelCompilationCacheInputTest, BypassesCacheWhenWorkingDirectoryIsUnavailable)
{
    const fs::path directory = root_ / "removed";
    fs::create_directory(directory);
    fs::current_path(directory);
    ASSERT_TRUE(fs::remove(directory));

    const auto input = Prepare();

    EXPECT_TRUE(input.baseKey.empty());
    EXPECT_TRUE(input.key.empty());
    EXPECT_TRUE(input.description.at("base_inputs").at("working_directory").is_null());
    EXPECT_EQ(input.constants.argumentBytes.at(0), (std::vector<uint8_t>{1U, 2U}));
}

TEST_F(KernelCompilationCacheInputTest, SortsResourceFilesAndDetectsResourceChanges)
{
    const auto first = Prepare();
    ResourceEntry reordered;
    reordered.data = resource_.data;
    reordered.files = {resource_.files[1], resource_.files[0]};
    KernelCompilationCacheInput input = first;
    BuildKernelCompilationCacheIdentity(request_, reordered, plan_, input);
    EXPECT_EQ(first.key, input.key);
    ResourceEntry changed;
    changed.data = resource_.data;
    changed.files = resource_.files;
    changed.files[0].bytes[0] = 99U;
    BuildKernelCompilationCacheIdentity(request_, changed, plan_, input);
    EXPECT_NE(first.key, input.key);
    ResourceEntry changedManifest;
    changedManifest.data = resource_.data;
    changedManifest.files = resource_.files;
    changedManifest.data.json["other_entry"] = "changed";
    BuildKernelCompilationCacheIdentity(request_, changedManifest, plan_, input);
    EXPECT_NE(first.key, input.key);
}

TEST_F(KernelCompilationCacheInputTest, CopiesEachArgumentOnceForAllBindingRanges)
{
    ManifestConstant longer;
    ASSERT_TRUE(longer.ParseDefinition(
        {{"name", "longer"},
         {"parameter_index", 0},
         {"arg_type", "struct"},
         {"file", "${resource}/constant.h"},
         {"template", "@@LONGER@@"}}));
    plan_.constants.push_back(longer);
    const auto first = Prepare();
    ASSERT_EQ(first.constants.argumentBytes.size(), 1U);
    EXPECT_EQ(first.constants.argumentBytes.at(0).size(), 4U);
    const auto& bindings = first.description.at("bindings");
    EXPECT_EQ(bindings[0].at("size"), "2");
    EXPECT_EQ(bindings[1].at("size"), "4");
    bytes_[3] = 99U;
    const auto second = Prepare();
    EXPECT_EQ(bindings[0], second.description.at("bindings")[0]);
    EXPECT_NE(bindings[1], second.description.at("bindings")[1]);
    EXPECT_NE(first.key, second.key);
    sizes_[0] = 3U;
    EXPECT_NE(second.key, Prepare().key);
}

TEST_F(KernelCompilationCacheInputTest, ValidatesAllBindingsBeforeReadingAndPreservesOutputOnFailure)
{
    const size_t pageSize = static_cast<size_t>(sysconf(_SC_PAGESIZE));
    void* unreadable = mmap(nullptr, pageSize, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    ASSERT_NE(unreadable, MAP_FAILED);
    addresses_[0] = unreadable;
    ManifestConstant invalid;
    ASSERT_TRUE(invalid.ParseDefinition(
        {{"name", "invalid"},
         {"parameter_index", 1},
         {"arg_type", "pointer"},
         {"byte_size", 1},
         {"file", "constant.h"},
         {"template", "@@I@@"}}));
    plan_.constants.push_back(invalid);
    KernelConstantSnapshot constants;
    EXPECT_EQ(CaptureKernelConstants(request_, plan_.constants, constants), ACLRTC_ERROR_INVALID_INPUT);
    EXPECT_TRUE(constants.argumentBytes.empty());
    EXPECT_EQ(munmap(unreadable, pageSize), 0);
}

TEST_F(KernelCompilationCacheInputTest, ReadsTimestampsOnceAndFallsBackWithoutCompleteIdentity)
{
    const auto first = Prepare();
    EXPECT_EQ(first.description.at("base_inputs").at("toolchain").at("asc-devkit"), "asc-devkit-first");
    const fs::path version = root_ / "share/info/asc-devkit/version.info";
    std::ofstream(version.string()) << "timestamp=second\n";
    EXPECT_EQ(first.key, Prepare().key);
    ResetProcessContext();
    EXPECT_NE(first.key, Prepare().key);
    for (const std::string& text : std::vector<std::string>{
             "version=1\n", "timestamp=\n", "timestamp=a\ntimestamp=b\n", std::string(65537U, 'a')}) {
        std::ofstream(version.string()) << text;
        ResetProcessContext();
        const auto input = Prepare();
        EXPECT_TRUE(input.key.empty());
        EXPECT_TRUE(input.baseKey.empty());
        EXPECT_EQ(input.constants.argumentBytes.at(0), (std::vector<uint8_t>{1U, 2U}));
        std::ofstream(version.string()) << "timestamp=repaired\n";
        EXPECT_TRUE(Prepare().key.empty());
    }
    fs::remove(version);
    ResetProcessContext();
    EXPECT_TRUE(Prepare().key.empty());
}

TEST_F(KernelCompilationCacheInputTest, ExecutesWithTheSavedCompilerEnvironment)
{
    ASSERT_EQ(setenv("CPATH", "first/include", 1), 0);
    ASSERT_EQ(unsetenv("C_INCLUDE_PATH"), 0);
    ASSERT_EQ(setenv("CPLUS_INCLUDE_PATH", "", 1), 0);
    ASSERT_EQ(setenv("ACLRTC_TEST_INHERITED", "inherited", 1), 0);
    ResetProcessContext();
    plan_.compilerEnvironment = ProcessCompilationContext::Instance().GetCompilerEnvironment();
    ASSERT_EQ(setenv("CPATH", "second/include", 1), 0);
    ASSERT_EQ(setenv("C_INCLUDE_PATH", "later/include", 1), 0);
    ASSERT_EQ(unsetenv("CPLUS_INCLUDE_PATH"), 0);
    const auto input = Prepare();
    EXPECT_EQ(input.description.at("base_inputs").at("environment").at("CPATH"), "first/include");
    KernelCompilationPlan executablePlan;
    executablePlan.compilerEnvironment = plan_.compilerEnvironment;
    executablePlan.compilationCommands.push_back(
        {CompilationCommandKind::Compile,
         {},
         "environment",
         "/bin/sh",
         {"-c",
          "printf '%s|%s|%s|%s' \"$CPATH\" \"${C_INCLUDE_PATH-unset}\" "
          "\"${CPLUS_INCLUDE_PATH-unset}\" \"$ACLRTC_TEST_INHERITED\" > \"$1\"",
          "environment", (root_ / "environment.txt").string()},
         {}});
    KernelSpecializationDiagnostics diagnostics(root_, "environment");
    CompilationPlanExecutor executor(diagnostics, CompilationCommandExecutionLimits::ProductionDefaults());
    ASSERT_EQ(executor.ExecuteCompilationPlan(executablePlan), ACLRTC_SUCCESS);
    std::ifstream output((root_ / "environment.txt").string());
    const std::string value{std::istreambuf_iterator<char>(output), std::istreambuf_iterator<char>()};
    EXPECT_EQ(value, "first/include|unset||inherited");
    output.close();
    fs::remove(root_ / "environment.txt");
    ascendc::ProcessExecutorRequest replay;
    replay.arguments = {"/bin/sh", (root_ / "aclrtc_replay.sh").string()};
    replay.executionTimeout = std::chrono::seconds(5);
    replay.terminationGracePeriod = std::chrono::milliseconds(20);
    EXPECT_TRUE(ascendc::ProcessExecutor::Execute(replay).HasSuccessfulExit());
    std::ifstream replayOutput((root_ / "environment.txt").string());
    const std::string replayValue{std::istreambuf_iterator<char>(replayOutput), std::istreambuf_iterator<char>()};
    EXPECT_EQ(replayValue, "first/include|unset||inherited");
}

TEST_F(KernelCompilationCacheInputTest, NormalizesGeneratedResourcePathsInLineMarkers)
{
    const auto first = Preprocess("first", "int value = 1;\n");
    const auto second = Preprocess("second", "int value = 1;\n");

    EXPECT_EQ(first.key, second.key);
    EXPECT_EQ(first.description.at("request_key"), "same-request");
    EXPECT_EQ(first.description.at("key"), first.key);
    EXPECT_EQ(first.description.at("preprocessed_sources"), second.description.at("preprocessed_sources"));
}

TEST_F(KernelCompilationCacheInputTest, PreservesExpandedSourceFileStrings)
{
    const auto first = Preprocess("first", "const char* file = __FILE__;\n");
    const auto second = Preprocess("second", "const char* file = __FILE__;\n");

    EXPECT_NE(first.key, second.key);
}

TEST_F(KernelCompilationCacheInputTest, PreservesExternalFilePathsAndSourceContents)
{
    const fs::path firstHeader = root_ / "first.h";
    const fs::path secondHeader = root_ / "second.h";
    std::ofstream(firstHeader.string()) << "int value = 1;\n";
    std::ofstream(secondHeader.string()) << "int value = 1;\n";
    const auto first = Preprocess("first", "#include \"" + firstHeader.string() + "\"\n");
    const auto second = Preprocess("second", "#include \"" + secondHeader.string() + "\"\n");
    EXPECT_NE(first.key, second.key);

    std::ofstream(firstHeader.string()) << "int value = 2;\n";
    const auto changed = Preprocess("changed", "#include \"" + firstHeader.string() + "\"\n");
    EXPECT_NE(first.key, changed.key);
}

} // namespace
