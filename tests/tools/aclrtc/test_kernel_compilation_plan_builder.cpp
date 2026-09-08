/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "specialization/kernel_compilation_plan_builder.h"
#include "specialization/compilation_manifest_parser.h"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <algorithm>
#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

namespace {
namespace fs = boost::filesystem;
using Json = nlohmann::json;
using ascendc::aclrtc::CompilationCommandKind;
using ascendc::aclrtc::KernelCompilationPlan;
using ascendc::aclrtc::KernelCompilationPlanBuilder;
using ascendc::aclrtc::NormalizedKernelSpecializationRequest;

class ScopedEnvironmentVariable final {
public:
    ScopedEnvironmentVariable(const char* name, const std::string& value) : name_(name)
    {
        const char* previousValue = std::getenv(name);
        if (previousValue != nullptr) {
            previousValue_ = previousValue;
            wasSet_ = true;
        }
        setenv(name, value.c_str(), 1);
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

class KernelPlanTestWorkspace final {
public:
    KernelPlanTestWorkspace()
    {
        static uint64_t sequence = 0;
        rootPath_ = fs::temp_directory_path() /
                    ("aclrtc_plan_test_" + std::to_string(getpid()) + "_" + std::to_string(sequence++));
        resourcePath_ = rootPath_ / "resource";
        toolchainPath_ = rootPath_ / "toolchain";
        sourceDirectoryPath_ = rootPath_ / "user_source";
        fs::create_directories(resourcePath_ / "resources/include");
        fs::create_directories(resourcePath_ / "resources/src");
        fs::create_directories(toolchainPath_ / "bin");
        fs::create_directories(sourceDirectoryPath_);
        WriteFile(sourceDirectoryPath_ / "add_custom.cpp", "// user source\n");
        WriteFile(resourcePath_ / "resources/include/constants.h", "@@STATIC_TILING@@\n");
        WriteFile(resourcePath_ / "resources/src/basic.cpp", "// basic\n");
        WriteFile(resourcePath_ / "resources/src/sk.cpp", "// sk\n");
        WriteExecutable(toolchainPath_ / "bin/bisheng");
        WriteExecutable(toolchainPath_ / "bin/llvm-objcopy");
        WriteExecutable(toolchainPath_ / "bin/ld.lld");
    }

    ~KernelPlanTestWorkspace()
    {
        boost::system::error_code ignoredError;
        fs::remove_all(rootPath_, ignoredError);
    }

    const fs::path& ResourcePath() const { return resourcePath_; }
    const fs::path& ToolchainPath() const { return toolchainPath_; }
    const fs::path& SourceDirectoryPath() const { return sourceDirectoryPath_; }

private:
    static void WriteFile(const fs::path& path, const std::string& contents)
    {
        std::ofstream stream(path.string());
        stream << contents;
    }

    static void WriteExecutable(const fs::path& path)
    {
        WriteFile(path, "#!/bin/sh\nexit 0\n");
        chmod(path.c_str(), 0755);
    }

    fs::path rootPath_;
    fs::path resourcePath_;
    fs::path toolchainPath_;
    fs::path sourceDirectoryPath_;
};

Json CreateManifest()
{
    return Json{
        {"schema_version", "1.0"},
        {"source_file", "add_custom.cpp"},
        {"options",
         {{"common_compile", Json::array({"-I", "${source_file_path}", "-I${env:ACLRTC_TEST_TOOLCHAIN}/include"})}}},
        {"kernels",
         Json::array(
             {{{"kernel_name", "other_kernel"}, {"unsupported_future_field", Json::object()}},
              {{"kernel_name", "add_custom_100000"},
               {"constant_infos", Json::array(
                                      {{{"name", "tiling"},
                                        {"parameter_index", 0},
                                        {"arg_type", "pointer"},
                                        {"byte_size", 2},
                                        {"file", "${resource}/resources/include/constants.h"},
                                        {"template", "@@STATIC_TILING@@"}}})},
               {"link_options", Json::array({"-m", "aicorelinux", "-r", "-Ttext=0", "-q", "-x"})},
               {"objects",
                Json::array(
                    {{{"object_name", "basic"},
                      {"object_type", "basic"},
                      {"commands",
                       Json::array(
                           {{{"type", "compile"},
                             {"stage", 0},
                             {"cmd", Json::array(
                                         {"${env:ACLRTC_TEST_TOOLCHAIN}/bin/bisheng", "${options:common_compile}",
                                          "${resource}/resources/src/basic.cpp", "-o", "${output}/basic.o"})}}})},
                      {"outputs", Json::array({"${output}/basic.o"})}},
                     {{"object_name", "sk"},
                      {"object_type", "sk"},
                      {"commands", Json::array(
                                       {{{"type", "compile"},
                                         {"stage", 0},
                                         {"cmd", Json::array(
                                                     {"${env:ACLRTC_TEST_SK_TOOLCHAIN}/bin/bisheng",
                                                      "${resource}/resources/src/sk.cpp", "-o", "${output}/sk.o"})}},
                                        {{"type", "objcopy"},
                                         {"stage", 1},
                                         {"cmd", Json::array(
                                                     {"${env:ACLRTC_TEST_TOOLCHAIN}/bin/llvm-objcopy",
                                                      "--redefine-sym=old_symbol=new_symbol", "${output}/sk.o",
                                                      "${output}/sk_split.o"})}}})},
                      {"outputs", Json::array({"${output}/sk.o", "${output}/sk_split.o"})}}})}}})}};
}

NormalizedKernelSpecializationRequest CreateRequest(
    const KernelPlanTestWorkspace& workspace, const void* const* argumentAddresses, const uint64_t* argumentByteCounts)
{
    NormalizedKernelSpecializationRequest request;
    request.specializationSessionId = "session_1";
    request.resourceId = "resource";
    request.kernelName = "add_custom_100000";
    request.outputElfPath = workspace.ResourcePath() / "published.elf";
    request.kernelArgumentCount = 1;
    request.borrowedKernelArgumentDataPointers = argumentAddresses;
    request.borrowedKernelArgumentByteCounts = argumentByteCounts;
    return request;
}

aclError BuildPlanFromManifest(
    const NormalizedKernelSpecializationRequest& request, const Json& json, const fs::path& worktree,
    const fs::path& sourceDirectory, KernelCompilationPlan& plan)
{
    ascendc::aclrtc::CompilationManifest manifest;
    const aclError status = ascendc::aclrtc::CompilationManifestParser(json).ParseSelected(
        request.kernelName, request.enableSuperKernel, manifest);
    if (status != ascendc::aclrtc::ACLRTC_SUCCESS) {
        return status;
    }
    return KernelCompilationPlanBuilder(request, manifest, worktree, sourceDirectory).BuildCompilationPlan(plan);
}

TEST(CompilationManifestParserTest, RepeatedSelectionsDoNotRetainPartialResults)
{
    const Json json = CreateManifest();
    ascendc::aclrtc::CompilationManifestParser parser(json);
    ascendc::aclrtc::CompilationManifest manifest;
    EXPECT_EQ(parser.ParseSelected("missing", false, manifest), ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
    ASSERT_EQ(parser.ParseSelected("add_custom_100000", true, manifest), ascendc::aclrtc::ACLRTC_SUCCESS);
    ASSERT_EQ(manifest.commands.size(), 3U);
    EXPECT_EQ(parser.ParseSelected("missing", false, manifest), ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
    EXPECT_EQ(manifest.commands.size(), 3U);
    ASSERT_EQ(parser.ParseSelected("add_custom_100000", false, manifest), ascendc::aclrtc::ACLRTC_SUCCESS);
    EXPECT_EQ(manifest.commands.size(), 1U);
    EXPECT_EQ(manifest.constants.size(), 1U);
}

TEST(KernelCompilationPlanBuilderTest, BuildsRepeatedPlansWithoutMutatingParsedConstants)
{
    KernelPlanTestWorkspace workspace;
    ScopedEnvironmentVariable toolchain("ACLRTC_TEST_TOOLCHAIN", workspace.ToolchainPath().string());
    ScopedEnvironmentVariable ascendHome("ASCEND_HOME_PATH", workspace.ToolchainPath().string());
    const uint8_t bytes[] = {1, 2};
    const void* addresses[] = {bytes};
    auto request = CreateRequest(workspace, addresses, nullptr);
    ascendc::aclrtc::CompilationManifest manifest;
    const Json json = CreateManifest();
    ASSERT_EQ(
        ascendc::aclrtc::CompilationManifestParser(json).ParseSelected(
            request.kernelName, request.enableSuperKernel, manifest),
        ascendc::aclrtc::ACLRTC_SUCCESS);
    KernelCompilationPlanBuilder builder(request, manifest, workspace.ResourcePath(), workspace.SourceDirectoryPath());
    KernelCompilationPlan plan;
    ASSERT_EQ(builder.BuildCompilationPlan(plan), ascendc::aclrtc::ACLRTC_SUCCESS);
    ASSERT_EQ(builder.BuildCompilationPlan(plan), ascendc::aclrtc::ACLRTC_SUCCESS);
    ASSERT_EQ(plan.sourcePatches.size(), 1U);
    EXPECT_EQ(plan.sourcePatches[0].replacementText, "{0x01, 0x02}");
    EXPECT_EQ(manifest.constants[0].GetBoundData(), nullptr);
    EXPECT_EQ(manifest.constants[0].GetBoundByteSize(), 0U);
}

TEST(CompilationManifestParserTest, OwnsSelectedFieldsWithoutResolvingRuntimeEnvironment)
{
    Json manifest = CreateManifest();
    manifest["kernels"][1]["objects"][1].erase("commands");
    manifest["options"]["unused"] = 7;
    ascendc::aclrtc::CompilationManifest spec;
    ASSERT_EQ(
        ascendc::aclrtc::CompilationManifestParser(manifest).ParseSelected("add_custom_100000", false, spec),
        ascendc::aclrtc::ACLRTC_SUCCESS);
    manifest.clear();
    ASSERT_EQ(spec.constants.size(), 1U);
    const uint8_t byte = 0;
    const void* addresses[] = {&byte};
    ASSERT_TRUE(spec.constants[0].BindArgument(1U, addresses, nullptr));
    EXPECT_EQ(spec.constants[0].GetBoundByteSize(), 2U);
    ASSERT_EQ(spec.commands.size(), 1U);
    EXPECT_EQ(spec.commands[0].executable, "${env:ACLRTC_TEST_TOOLCHAIN}/bin/bisheng");
    ASSERT_GE(spec.commands[0].arguments.size(), 3U);
    EXPECT_EQ(spec.commands[0].arguments[2], "-I${env:ACLRTC_TEST_TOOLCHAIN}/include");
}

TEST(KernelCompilationPlanBuilderTest, BuildsBasicPlanFromOnlySelectedManifestFields)
{
    KernelPlanTestWorkspace workspace;
    ScopedEnvironmentVariable toolchainEnvironment("ACLRTC_TEST_TOOLCHAIN", workspace.ToolchainPath().string());
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", workspace.ToolchainPath().string());
    unsetenv("ACLRTC_TEST_SK_TOOLCHAIN");
    const uint8_t tilingBytes[] = {0x01, 0xfe};
    const void* argumentAddresses[] = {tilingBytes};
    const uint64_t argumentByteCounts[] = {sizeof(tilingBytes)};
    NormalizedKernelSpecializationRequest request = CreateRequest(workspace, argumentAddresses, argumentByteCounts);
    request.compilerOptions.basicOptions = {"-g", "-DVALUE=1"};

    KernelCompilationPlan plan;
    EXPECT_EQ(
        BuildPlanFromManifest(
            request, CreateManifest(), workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        0);

    ASSERT_EQ(plan.sourcePatches.size(), 1U);
    EXPECT_EQ(plan.sourcePatches[0].targetFilePath, workspace.ResourcePath() / "resources/include/constants.h");
    EXPECT_EQ(plan.sourcePatches[0].replacementText, "{0x01, 0xfe}");

    ASSERT_EQ(plan.compilationCommands.size(), 2U);
    const auto& compileCommand = plan.compilationCommands[0];
    EXPECT_EQ(compileCommand.commandKind, CompilationCommandKind::Compile);
    EXPECT_EQ(compileCommand.executablePath, workspace.ToolchainPath() / "bin/bisheng");
    EXPECT_EQ(
        compileCommand.arguments,
        (std::vector<std::string>{
            "-I", workspace.SourceDirectoryPath().string(), "-I" + (workspace.ToolchainPath() / "include").string(),
            (workspace.ResourcePath() / "resources/src/basic.cpp").string(), "-g", "-DVALUE=1", "-o",
            (workspace.ResourcePath() / ".aclrtc_session_1/outputs/basic.o").string()}));
    EXPECT_EQ(plan.compilationCommands[1].commandKind, CompilationCommandKind::Link);
    EXPECT_EQ(plan.compilationCommands[1].executablePath, workspace.ToolchainPath() / "bin/ld.lld");
    EXPECT_EQ(plan.linkedKernelElfPath, workspace.ResourcePath() / ".aclrtc_session_1/outputs/linked_kernel.elf");
}

TEST(KernelCompilationPlanBuilderTest, BuildsPlanWithoutOptionalExternalSourceFile)
{
    KernelPlanTestWorkspace workspace;
    ScopedEnvironmentVariable toolchainEnvironment("ACLRTC_TEST_TOOLCHAIN", workspace.ToolchainPath().string());
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", workspace.ToolchainPath().string());
    const uint8_t tilingBytes[] = {0x01, 0x02};
    const void* argumentAddresses[] = {tilingBytes};
    const uint64_t argumentByteCounts[] = {sizeof(tilingBytes)};
    NormalizedKernelSpecializationRequest request = CreateRequest(workspace, argumentAddresses, argumentByteCounts);
    Json manifest = CreateManifest();
    manifest.erase("source_file");
    manifest["options"]["common_compile"] = Json::array({"-I${env:ACLRTC_TEST_TOOLCHAIN}/include"});

    KernelCompilationPlan plan;
    EXPECT_EQ(
        BuildPlanFromManifest(request, manifest, workspace.ResourcePath(), fs::path(), plan),
        ascendc::aclrtc::ACLRTC_SUCCESS);
}

TEST(KernelCompilationPlanBuilderTest, RejectsCompileCommandWithoutOutputPath)
{
    KernelPlanTestWorkspace workspace;
    ScopedEnvironmentVariable toolchainEnvironment("ACLRTC_TEST_TOOLCHAIN", workspace.ToolchainPath().string());
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", workspace.ToolchainPath().string());
    const uint8_t tilingBytes[] = {0x01, 0x02};
    const void* argumentAddresses[] = {tilingBytes};
    const uint64_t argumentByteCounts[] = {sizeof(tilingBytes)};
    NormalizedKernelSpecializationRequest request = CreateRequest(workspace, argumentAddresses, argumentByteCounts);
    Json manifest = CreateManifest();
    manifest["kernels"][1]["objects"][0]["commands"][0]["cmd"].erase(4);

    KernelCompilationPlan plan;
    EXPECT_EQ(
        BuildPlanFromManifest(request, manifest, workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
}

TEST(KernelCompilationPlanBuilderTest, DoesNotTreatNonOutputOptionAsCompilerOutput)
{
    KernelPlanTestWorkspace workspace;
    ScopedEnvironmentVariable toolchainEnvironment("ACLRTC_TEST_TOOLCHAIN", workspace.ToolchainPath().string());
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", workspace.ToolchainPath().string());
    const uint8_t tilingBytes[] = {0x01, 0x02};
    const void* argumentAddresses[] = {tilingBytes};
    const uint64_t argumentByteCounts[] = {sizeof(tilingBytes)};
    NormalizedKernelSpecializationRequest request = CreateRequest(workspace, argumentAddresses, argumentByteCounts);
    Json manifest = CreateManifest();
    manifest["kernels"][1]["objects"][0]["commands"][0]["cmd"][3] = "-opaque-pointers";

    KernelCompilationPlan plan;
    EXPECT_EQ(
        BuildPlanFromManifest(request, manifest, workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
}

TEST(KernelCompilationPlanBuilderTest, AcceptsCommandWithoutStageAndPreservesManifestOrder)
{
    KernelPlanTestWorkspace workspace;
    ScopedEnvironmentVariable toolchainEnvironment("ACLRTC_TEST_TOOLCHAIN", workspace.ToolchainPath().string());
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", workspace.ToolchainPath().string());
    const uint8_t tilingBytes[] = {0x01, 0x02};
    const void* argumentAddresses[] = {tilingBytes};
    const uint64_t argumentByteCounts[] = {sizeof(tilingBytes)};
    NormalizedKernelSpecializationRequest request = CreateRequest(workspace, argumentAddresses, argumentByteCounts);
    Json manifest = CreateManifest();
    manifest["kernels"][1]["objects"][0]["commands"][0].erase("stage");

    KernelCompilationPlan plan;
    ASSERT_EQ(
        BuildPlanFromManifest(request, manifest, workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan), 0);
    ASSERT_EQ(plan.compilationCommands.size(), 2U);
    EXPECT_EQ(plan.compilationCommands[0].diagnosticLabel, "basic/compile[0]");
    EXPECT_FALSE(plan.compilationCommands[0].parallelStage.has_value());
    EXPECT_EQ(plan.compilationCommands[1].commandKind, CompilationCommandKind::Link);
    EXPECT_FALSE(plan.compilationCommands[1].parallelStage.has_value());
}

TEST(KernelCompilationPlanBuilderTest, PreservesExplicitStagesAndManifestCommandOrder)
{
    KernelPlanTestWorkspace workspace;
    ScopedEnvironmentVariable toolchainEnvironment("ACLRTC_TEST_TOOLCHAIN", workspace.ToolchainPath().string());
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", workspace.ToolchainPath().string());
    ScopedEnvironmentVariable skToolchainEnvironment("ACLRTC_TEST_SK_TOOLCHAIN", workspace.ToolchainPath().string());
    const uint8_t tilingBytes[] = {0x01, 0x02};
    const void* argumentAddresses[] = {tilingBytes};
    const uint64_t argumentByteCounts[] = {sizeof(tilingBytes)};
    NormalizedKernelSpecializationRequest request = CreateRequest(workspace, argumentAddresses, argumentByteCounts);
    request.enableSuperKernel = true;
    request.compilerOptions.superKernelOptions = {"-g"};

    KernelCompilationPlan plan;
    ASSERT_EQ(
        BuildPlanFromManifest(
            request, CreateManifest(), workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        0);
    ASSERT_EQ(plan.compilationCommands.size(), 4U);
    EXPECT_EQ(plan.compilationCommands[0].diagnosticLabel, "basic/compile[0]");
    EXPECT_EQ(plan.compilationCommands[1].diagnosticLabel, "sk/compile[0]");
    EXPECT_EQ(plan.compilationCommands[2].diagnosticLabel, "sk/objcopy[1]");
    EXPECT_EQ(plan.compilationCommands[3].diagnosticLabel, "link");
    EXPECT_EQ(plan.compilationCommands[0].parallelStage, 0U);
    EXPECT_EQ(plan.compilationCommands[1].parallelStage, 0U);
    EXPECT_EQ(plan.compilationCommands[2].parallelStage, 1U);
    EXPECT_FALSE(plan.compilationCommands[3].parallelStage.has_value());
    EXPECT_NE(
        std::find(plan.compilationCommands[1].arguments.begin(), plan.compilationCommands[1].arguments.end(), "-g"),
        plan.compilationCommands[1].arguments.end());
}

TEST(KernelCompilationPlanBuilderTest, SelectsAllRequestedObjectsInManifestOrder)
{
    KernelPlanTestWorkspace workspace;
    ScopedEnvironmentVariable toolchainEnvironment("ACLRTC_TEST_TOOLCHAIN", workspace.ToolchainPath().string());
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", workspace.ToolchainPath().string());
    ScopedEnvironmentVariable skToolchainEnvironment("ACLRTC_TEST_SK_TOOLCHAIN", workspace.ToolchainPath().string());
    const uint8_t tilingBytes[] = {0x01, 0x02};
    const void* argumentAddresses[] = {tilingBytes};
    const uint64_t argumentByteCounts[] = {sizeof(tilingBytes)};
    NormalizedKernelSpecializationRequest request = CreateRequest(workspace, argumentAddresses, argumentByteCounts);
    request.enableSuperKernel = true;

    Json manifest = CreateManifest();
    Json secondBasicObject = manifest["kernels"][1]["objects"][0];
    secondBasicObject["object_name"] = "basic_aux";
    secondBasicObject["commands"][0]["cmd"][4] = "${output}/basic_aux.o";
    secondBasicObject["outputs"][0] = "${output}/basic_aux.o";
    manifest["kernels"][1]["objects"].push_back(secondBasicObject);

    Json secondSuperKernelObject = manifest["kernels"][1]["objects"][1];
    secondSuperKernelObject["object_name"] = "sk_aux";
    secondSuperKernelObject["commands"][0]["cmd"][3] = "${output}/sk_aux.o";
    secondSuperKernelObject["commands"][1]["cmd"][2] = "${output}/sk_aux.o";
    secondSuperKernelObject["commands"][1]["cmd"][3] = "${output}/sk_aux_split.o";
    secondSuperKernelObject["outputs"] = Json::array({"${output}/sk_aux.o", "${output}/sk_aux_split.o"});
    manifest["kernels"][1]["objects"].push_back(secondSuperKernelObject);

    KernelCompilationPlan plan;
    ASSERT_EQ(
        BuildPlanFromManifest(request, manifest, workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan), 0);

    ASSERT_EQ(plan.compilationCommands.size(), 7U);
    EXPECT_EQ(plan.compilationCommands[0].diagnosticLabel, "basic/compile[0]");
    EXPECT_EQ(plan.compilationCommands[1].diagnosticLabel, "sk/compile[0]");
    EXPECT_EQ(plan.compilationCommands[2].diagnosticLabel, "sk/objcopy[1]");
    EXPECT_EQ(plan.compilationCommands[3].diagnosticLabel, "basic_aux/compile[0]");
    EXPECT_EQ(plan.compilationCommands[4].diagnosticLabel, "sk_aux/compile[0]");
    EXPECT_EQ(plan.compilationCommands[5].diagnosticLabel, "sk_aux/objcopy[1]");
    EXPECT_EQ(plan.compilationCommands[6].diagnosticLabel, "link");

    const std::vector<std::string>& linkArguments = plan.compilationCommands.back().arguments;
    EXPECT_NE(
        std::find(
            linkArguments.begin(), linkArguments.end(),
            (workspace.ResourcePath() / ".aclrtc_session_1/outputs/basic_aux.o").string()),
        linkArguments.end());
    EXPECT_NE(
        std::find(
            linkArguments.begin(), linkArguments.end(),
            (workspace.ResourcePath() / ".aclrtc_session_1/outputs/sk_aux_split.o").string()),
        linkArguments.end());
}

TEST(KernelCompilationPlanBuilderTest, RequiresOnlyEnvironmentUsedBySelectedObjects)
{
    KernelPlanTestWorkspace workspace;
    ScopedEnvironmentVariable toolchainEnvironment("ACLRTC_TEST_TOOLCHAIN", workspace.ToolchainPath().string());
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", workspace.ToolchainPath().string());
    unsetenv("ACLRTC_TEST_SK_TOOLCHAIN");
    const uint8_t tilingBytes[] = {0x01, 0x02};
    const void* argumentAddresses[] = {tilingBytes};
    const uint64_t argumentByteCounts[] = {sizeof(tilingBytes)};
    NormalizedKernelSpecializationRequest request = CreateRequest(workspace, argumentAddresses, argumentByteCounts);

    KernelCompilationPlan plan;
    EXPECT_EQ(
        BuildPlanFromManifest(
            request, CreateManifest(), workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        0);

    request.enableSuperKernel = true;
    EXPECT_EQ(
        BuildPlanFromManifest(
            request, CreateManifest(), workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
}

TEST(KernelCompilationPlanBuilderTest, RejectsMalformedEnvironmentPlaceholders)
{
    KernelPlanTestWorkspace workspace;
    ScopedEnvironmentVariable toolchainEnvironment("ACLRTC_TEST_TOOLCHAIN", workspace.ToolchainPath().string());
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", workspace.ToolchainPath().string());
    const uint8_t tilingBytes[] = {0x01, 0x02};
    const void* argumentDataPointers[] = {tilingBytes};
    const uint64_t argumentByteCounts[] = {sizeof(tilingBytes)};
    NormalizedKernelSpecializationRequest request = CreateRequest(workspace, argumentDataPointers, argumentByteCounts);
    KernelCompilationPlan plan;

    Json manifestWithUnterminatedPlaceholder = CreateManifest();
    manifestWithUnterminatedPlaceholder["kernels"][1]["objects"][0]["commands"][0]["cmd"][2] =
        "${env:ACLRTC_TEST_TOOLCHAIN";
    EXPECT_EQ(
        BuildPlanFromManifest(
            request, manifestWithUnterminatedPlaceholder, workspace.ResourcePath(), workspace.SourceDirectoryPath(),
            plan),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);

    Json manifestWithEmptyEnvironmentName = CreateManifest();
    manifestWithEmptyEnvironmentName["kernels"][1]["objects"][0]["commands"][0]["cmd"][2] = "${env:}";
    EXPECT_EQ(
        BuildPlanFromManifest(
            request, manifestWithEmptyEnvironmentName, workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
}

TEST(KernelCompilationPlanBuilderTest, RejectsUndefinedOptionReference)
{
    KernelPlanTestWorkspace workspace;
    ScopedEnvironmentVariable toolchainEnvironment("ACLRTC_TEST_TOOLCHAIN", workspace.ToolchainPath().string());
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", workspace.ToolchainPath().string());
    const uint8_t tilingBytes[] = {0x01, 0x02};
    const void* argumentDataPointers[] = {tilingBytes};
    const uint64_t argumentByteCounts[] = {sizeof(tilingBytes)};
    NormalizedKernelSpecializationRequest request = CreateRequest(workspace, argumentDataPointers, argumentByteCounts);
    Json manifest = CreateManifest();
    manifest["kernels"][1]["objects"][0]["commands"][0]["cmd"][1] = "${options:missing}";

    KernelCompilationPlan plan;
    EXPECT_EQ(
        BuildPlanFromManifest(request, manifest, workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
}

TEST(KernelCompilationPlanBuilderTest, DoesNotRecursivelyExpandEnvironmentOrUserOptions)
{
    KernelPlanTestWorkspace workspace;
    ScopedEnvironmentVariable toolchainEnvironment("ACLRTC_TEST_TOOLCHAIN", workspace.ToolchainPath().string());
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", workspace.ToolchainPath().string());
    ScopedEnvironmentVariable literalEnvironment("ACLRTC_TEST_LITERAL", "${options:not_reexpanded}");
    const uint8_t tilingBytes[] = {0x01, 0x02};
    const void* argumentAddresses[] = {tilingBytes};
    const uint64_t argumentByteCounts[] = {sizeof(tilingBytes)};
    NormalizedKernelSpecializationRequest request = CreateRequest(workspace, argumentAddresses, argumentByteCounts);
    request.compilerOptions.basicOptions = {"${env:USER_OPTION_STAYS_LITERAL}"};
    Json manifest = CreateManifest();
    manifest["options"]["common_compile"].push_back("${env:ACLRTC_TEST_LITERAL}");

    KernelCompilationPlan plan;
    ASSERT_EQ(
        BuildPlanFromManifest(request, manifest, workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan), 0);
    const std::vector<std::string>& arguments = plan.compilationCommands.front().arguments;
    EXPECT_NE(std::find(arguments.begin(), arguments.end(), "${options:not_reexpanded}"), arguments.end());
    EXPECT_NE(std::find(arguments.begin(), arguments.end(), "${env:USER_OPTION_STAYS_LITERAL}"), arguments.end());
}

TEST(KernelCompilationPlanBuilderTest, AcceptsKernelWithoutLinkOptions)
{
    KernelPlanTestWorkspace workspace;
    ScopedEnvironmentVariable toolchainEnvironment("ACLRTC_TEST_TOOLCHAIN", workspace.ToolchainPath().string());
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", workspace.ToolchainPath().string());
    Json manifest = CreateManifest();
    manifest["kernels"][1].erase("link_options");
    const uint8_t tilingBytes[] = {0x01, 0x02};
    const void* argumentAddresses[] = {tilingBytes};
    const uint64_t argumentByteCounts[] = {sizeof(tilingBytes)};
    NormalizedKernelSpecializationRequest request = CreateRequest(workspace, argumentAddresses, argumentByteCounts);

    KernelCompilationPlan plan;
    EXPECT_EQ(
        BuildPlanFromManifest(request, manifest, workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan), 0);
    EXPECT_EQ(plan.sourcePatches.size(), 1U);
    ASSERT_EQ(plan.compilationCommands.size(), 2U);
    EXPECT_EQ(plan.compilationCommands.back().commandKind, CompilationCommandKind::Link);
}

TEST(KernelCompilationPlanBuilderTest, RejectsKernelWithoutStaticConstants)
{
    KernelPlanTestWorkspace workspace;
    ScopedEnvironmentVariable toolchainEnvironment("ACLRTC_TEST_TOOLCHAIN", workspace.ToolchainPath().string());
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", workspace.ToolchainPath().string());
    NormalizedKernelSpecializationRequest request = CreateRequest(workspace, nullptr, nullptr);
    KernelCompilationPlan plan;

    Json manifestWithoutConstants = CreateManifest();
    manifestWithoutConstants["kernels"][1].erase("constant_infos");
    EXPECT_EQ(
        BuildPlanFromManifest(
            request, manifestWithoutConstants, workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);

    Json manifestWithEmptyConstants = CreateManifest();
    manifestWithEmptyConstants["kernels"][1]["constant_infos"] = Json::array();
    EXPECT_EQ(
        BuildPlanFromManifest(
            request, manifestWithEmptyConstants, workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
}

TEST(KernelCompilationPlanBuilderTest, RejectsMissingRuntimeConstantBuffer)
{
    KernelPlanTestWorkspace workspace;
    ScopedEnvironmentVariable toolchainEnvironment("ACLRTC_TEST_TOOLCHAIN", workspace.ToolchainPath().string());
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", workspace.ToolchainPath().string());
    NormalizedKernelSpecializationRequest request = CreateRequest(workspace, nullptr, nullptr);

    KernelCompilationPlan plan;
    EXPECT_EQ(
        BuildPlanFromManifest(
            request, CreateManifest(), workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_ERROR_INVALID_INPUT);
}

TEST(KernelCompilationPlanBuilderTest, PointerConstantUsesManifestByteSize)
{
    KernelPlanTestWorkspace workspace;
    ScopedEnvironmentVariable toolchainEnvironment("ACLRTC_TEST_TOOLCHAIN", workspace.ToolchainPath().string());
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", workspace.ToolchainPath().string());
    const uint8_t tilingBytes[] = {0x01, 0x02, 0x03};
    const void* argumentDataPointers[] = {tilingBytes};
    NormalizedKernelSpecializationRequest request = CreateRequest(workspace, argumentDataPointers, nullptr);

    KernelCompilationPlan plan;
    EXPECT_EQ(
        BuildPlanFromManifest(
            request, CreateManifest(), workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_SUCCESS);
    ASSERT_EQ(plan.sourcePatches.size(), 1U);
    EXPECT_EQ(plan.sourcePatches[0].replacementText, "{0x01, 0x02}");
}

TEST(KernelCompilationPlanBuilderTest, DefaultStructConstantUsesArgumentByteSize)
{
    KernelPlanTestWorkspace workspace;
    ScopedEnvironmentVariable toolchainEnvironment("ACLRTC_TEST_TOOLCHAIN", workspace.ToolchainPath().string());
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", workspace.ToolchainPath().string());
    const uint8_t tilingBytes[] = {0x01, 0x02, 0x03};
    const void* argumentDataPointers[] = {tilingBytes};
    const uint64_t argumentByteCounts[] = {sizeof(tilingBytes)};
    NormalizedKernelSpecializationRequest request = CreateRequest(workspace, argumentDataPointers, argumentByteCounts);
    Json manifest = CreateManifest();
    manifest["kernels"][1]["constant_infos"][0].erase("arg_type");
    manifest["kernels"][1]["constant_infos"][0].erase("byte_size");

    KernelCompilationPlan plan;
    EXPECT_EQ(
        BuildPlanFromManifest(request, manifest, workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_SUCCESS);
    ASSERT_EQ(plan.sourcePatches.size(), 1U);
    EXPECT_EQ(plan.sourcePatches[0].replacementText, "{0x01, 0x02, 0x03}");
}

TEST(KernelCompilationPlanBuilderTest, RejectsStructConstantWithManifestByteSize)
{
    KernelPlanTestWorkspace workspace;
    ScopedEnvironmentVariable toolchainEnvironment("ACLRTC_TEST_TOOLCHAIN", workspace.ToolchainPath().string());
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", workspace.ToolchainPath().string());
    const uint8_t tilingBytes[] = {0x01, 0x02, 0x03};
    const void* argumentDataPointers[] = {tilingBytes};
    const uint64_t argumentByteCounts[] = {sizeof(tilingBytes)};
    NormalizedKernelSpecializationRequest request = CreateRequest(workspace, argumentDataPointers, argumentByteCounts);
    Json manifest = CreateManifest();
    manifest["kernels"][1]["constant_infos"][0]["arg_type"] = "struct";

    KernelCompilationPlan plan;
    EXPECT_EQ(
        BuildPlanFromManifest(request, manifest, workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
    manifest["kernels"][1]["constant_infos"][0].erase("arg_type");
    EXPECT_EQ(
        BuildPlanFromManifest(request, manifest, workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
    EXPECT_TRUE(plan.sourcePatches.empty());
}

TEST(KernelCompilationPlanBuilderTest, RejectsInvalidInputsBeforeReadingAnyConstantBytes)
{
    KernelPlanTestWorkspace workspace;
    ScopedEnvironmentVariable toolchainEnvironment("ACLRTC_TEST_TOOLCHAIN", workspace.ToolchainPath().string());
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", workspace.ToolchainPath().string());
    for (int invalidInputKind : {0, 1, 2}) {
        // A protected buffer detects reads before the remaining inputs have been checked.
        ASSERT_EXIT(
            {
                const size_t pageSize = static_cast<size_t>(sysconf(_SC_PAGESIZE));
                void* unreadable = mmap(nullptr, pageSize, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
                if (unreadable == MAP_FAILED) {
                    _exit(2);
                }
                const void* addresses[] = {unreadable};
                const uint64_t sizes[] = {2U};
                auto request = CreateRequest(workspace, addresses, sizes);
                Json manifest = CreateManifest();
                auto& kernel = manifest["kernels"][1];
                if (invalidInputKind == 0) {
                    Json constant = kernel["constant_infos"][0];
                    constant["parameter_index"] = 1U;
                    kernel["constant_infos"].push_back(constant);
                } else if (invalidInputKind == 1) {
                    kernel["objects"][0]["commands"][0]["cmd"][0] = "/aclrtc-test-missing-compiler";
                } else {
                    unsetenv("ACLRTC_TEST_MISSING_LINK_OPTION");
                    kernel["link_options"] = Json::array({"${env:ACLRTC_TEST_MISSING_LINK_OPTION}"});
                }
                KernelCompilationPlan plan;
                plan.linkedKernelElfPath = "unchanged.elf";
                const auto status = BuildPlanFromManifest(
                    request, manifest, workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan);
                const auto expected = invalidInputKind == 0 ? ascendc::aclrtc::ACLRTC_ERROR_INVALID_INPUT :
                                                              ascendc::aclrtc::ACLRTC_ERROR_FAILURE;
                munmap(unreadable, pageSize);
                _exit(status == expected && plan.linkedKernelElfPath == "unchanged.elf" ? 0 : 1);
            },
            ::testing::ExitedWithCode(0), "");
    }
}

TEST(KernelCompilationPlanBuilderTest, BuildsMixedConstantsUsingEachParameterIndexAndSizeSource)
{
    KernelPlanTestWorkspace workspace;
    ScopedEnvironmentVariable toolchainEnvironment("ACLRTC_TEST_TOOLCHAIN", workspace.ToolchainPath().string());
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", workspace.ToolchainPath().string());
    const uint8_t pointerBytes[] = {0x01, 0x02, 0x03};
    const uint8_t structBytes[] = {0xfe, 0xff, 0x04};
    const void* addresses[] = {nullptr, pointerBytes, structBytes};
    const uint64_t sizes[] = {0U, sizeof(void*), sizeof(structBytes)};
    auto request = CreateRequest(workspace, addresses, sizes);
    request.kernelArgumentCount = 3U;
    Json manifest = CreateManifest();
    auto& constants = manifest["kernels"][1]["constant_infos"];
    constants[0]["parameter_index"] = 1U;
    Json structConstant = constants[0];
    structConstant["parameter_index"] = 2U;
    structConstant["arg_type"] = "struct";
    structConstant.erase("byte_size");
    structConstant["template"] = "@@STRUCT@@";
    constants.push_back(structConstant);

    KernelCompilationPlan plan;
    ASSERT_EQ(
        BuildPlanFromManifest(request, manifest, workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_SUCCESS);
    ASSERT_EQ(plan.sourcePatches.size(), 2U);
    EXPECT_EQ(plan.sourcePatches[0].replacementText, "{0x01, 0x02}");
    EXPECT_EQ(plan.sourcePatches[1].replacementText, "{0xfe, 0xff, 0x04}");
    EXPECT_EQ(plan.sourcePatches[1].templateText, "@@STRUCT@@");
}

TEST(KernelCompilationPlanBuilderTest, RejectsMissingResourceWorktree)
{
    KernelPlanTestWorkspace workspace;
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", workspace.ToolchainPath().string());
    const uint8_t constantBytes[] = {0x01, 0x02};
    const void* argumentDataPointers[] = {constantBytes};
    const uint64_t argumentByteCounts[] = {sizeof(constantBytes)};
    NormalizedKernelSpecializationRequest request = CreateRequest(workspace, argumentDataPointers, argumentByteCounts);
    KernelCompilationPlan plan;

    const fs::path missingWorktreePath = workspace.ResourcePath() / "missing";
    EXPECT_EQ(
        BuildPlanFromManifest(request, CreateManifest(), missingWorktreePath, workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
}

TEST(KernelCompilationPlanBuilderTest, ExpandsCommandPrefixAndRejectsInvalidCommands)
{
    KernelPlanTestWorkspace workspace;
    ScopedEnvironmentVariable toolchainEnvironment("ACLRTC_TEST_TOOLCHAIN", workspace.ToolchainPath().string());
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", workspace.ToolchainPath().string());
    const uint8_t constantBytes[] = {0x01, 0x02};
    const void* argumentDataPointers[] = {constantBytes};
    const uint64_t argumentByteCounts[] = {sizeof(constantBytes)};
    const NormalizedKernelSpecializationRequest request =
        CreateRequest(workspace, argumentDataPointers, argumentByteCounts);
    KernelCompilationPlan plan;

    Json unsupportedCommandManifest = CreateManifest();
    unsupportedCommandManifest["kernels"][1]["objects"][0]["commands"][0]["type"] = "future-command";
    EXPECT_EQ(
        BuildPlanFromManifest(
            request, unsupportedCommandManifest, workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);

    Json emptyCommandManifest = CreateManifest();
    emptyCommandManifest["kernels"][1]["objects"][0]["commands"][0]["cmd"] = Json::array();
    EXPECT_EQ(
        BuildPlanFromManifest(
            request, emptyCommandManifest, workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);

    Json expandedExecutableManifest = CreateManifest();
    expandedExecutableManifest["options"]["common_compile"] = Json::array();
    expandedExecutableManifest["kernels"][1]["objects"][0]["commands"][0]["cmd"] =
        Json::array({"${options:common_compile}"});
    EXPECT_EQ(
        BuildPlanFromManifest(
            request, expandedExecutableManifest, workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);

    KernelCompilationPlan expectedPlan;
    ASSERT_EQ(
        BuildPlanFromManifest(
            request, CreateManifest(), workspace.ResourcePath(), workspace.SourceDirectoryPath(), expectedPlan),
        ascendc::aclrtc::ACLRTC_SUCCESS);
    Json commandPrefixManifest = CreateManifest();
    auto& arguments = commandPrefixManifest["kernels"][1]["objects"][0]["commands"][0]["cmd"];
    Json commandPrefix = Json::array({arguments[0]});
    for (const auto& argument : commandPrefixManifest["options"]["common_compile"]) {
        commandPrefix.push_back(argument);
    }
    commandPrefixManifest["options"]["command_prefix"] = std::move(commandPrefix);
    arguments.erase(0);
    arguments[0] = "${options:command_prefix}";
    ASSERT_EQ(
        BuildPlanFromManifest(
            request, commandPrefixManifest, workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_SUCCESS);
    EXPECT_EQ(plan.compilationCommands[0].executablePath, expectedPlan.compilationCommands[0].executablePath);
    EXPECT_EQ(plan.compilationCommands[0].arguments, expectedPlan.compilationCommands[0].arguments);

    Json relativeExecutableManifest = CreateManifest();
    relativeExecutableManifest["kernels"][1]["objects"][0]["commands"][0]["cmd"][0] = "bisheng";
    EXPECT_EQ(
        BuildPlanFromManifest(
            request, relativeExecutableManifest, workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
}

TEST(KernelCompilationPlanBuilderTest, RequiresBasicAndFallsBackWhenSuperKernelIsAbsent)
{
    KernelPlanTestWorkspace workspace;
    ScopedEnvironmentVariable toolchainEnvironment("ACLRTC_TEST_TOOLCHAIN", workspace.ToolchainPath().string());
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", workspace.ToolchainPath().string());
    ScopedEnvironmentVariable skToolchainEnvironment("ACLRTC_TEST_SK_TOOLCHAIN", workspace.ToolchainPath().string());
    const uint8_t constantBytes[] = {0x01, 0x02};
    const void* argumentDataPointers[] = {constantBytes};
    const uint64_t argumentByteCounts[] = {sizeof(constantBytes)};
    NormalizedKernelSpecializationRequest request = CreateRequest(workspace, argumentDataPointers, argumentByteCounts);
    KernelCompilationPlan plan;

    Json manifestWithoutBasic = CreateManifest();
    manifestWithoutBasic["kernels"][1]["objects"].erase(0);
    EXPECT_EQ(
        BuildPlanFromManifest(
            request, manifestWithoutBasic, workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);

    request.enableSuperKernel = true;
    request.compilerOptions.superKernelOptions = {"--sk-only-test-option"};
    Json manifestWithoutSuperKernel = CreateManifest();
    manifestWithoutSuperKernel["kernels"][1]["objects"].erase(1);
    ASSERT_EQ(
        BuildPlanFromManifest(
            request, manifestWithoutSuperKernel, workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_SUCCESS);
    request.enableSuperKernel = false;
    KernelCompilationPlan basicPlan;
    ASSERT_EQ(
        BuildPlanFromManifest(
            request, manifestWithoutSuperKernel, workspace.ResourcePath(), workspace.SourceDirectoryPath(), basicPlan),
        ascendc::aclrtc::ACLRTC_SUCCESS);
    ASSERT_EQ(plan.compilationCommands.size(), basicPlan.compilationCommands.size());
    for (size_t index = 0; index < plan.compilationCommands.size(); ++index) {
        EXPECT_EQ(plan.compilationCommands[index].executablePath, basicPlan.compilationCommands[index].executablePath);
        EXPECT_EQ(plan.compilationCommands[index].arguments, basicPlan.compilationCommands[index].arguments);
    }
}

TEST(KernelCompilationPlanBuilderTest, RejectsUnresolvableLinkInputs)
{
    KernelPlanTestWorkspace workspace;
    ScopedEnvironmentVariable toolchainEnvironment("ACLRTC_TEST_TOOLCHAIN", workspace.ToolchainPath().string());
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", workspace.ToolchainPath().string());
    const uint8_t constantBytes[] = {0x01, 0x02};
    const void* argumentDataPointers[] = {constantBytes};
    const uint64_t argumentByteCounts[] = {sizeof(constantBytes)};
    const NormalizedKernelSpecializationRequest request =
        CreateRequest(workspace, argumentDataPointers, argumentByteCounts);
    KernelCompilationPlan plan;
    unsetenv("ACLRTC_TEST_MISSING_LINK_INPUT");

    Json unresolvedLinkOptionManifest = CreateManifest();
    unresolvedLinkOptionManifest["kernels"][1]["link_options"] = Json::array({"${env:ACLRTC_TEST_MISSING_LINK_INPUT}"});
    EXPECT_EQ(
        BuildPlanFromManifest(
            request, unresolvedLinkOptionManifest, workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);

    Json unresolvedObjectOutputManifest = CreateManifest();
    unresolvedObjectOutputManifest["kernels"][1]["objects"][0]["outputs"][0] = "${env:ACLRTC_TEST_MISSING_LINK_INPUT}";
    EXPECT_EQ(
        BuildPlanFromManifest(
            request, unresolvedObjectOutputManifest, workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);

    ScopedEnvironmentVariable missingLinkerEnvironment(
        "ASCEND_HOME_PATH", (workspace.ResourcePath() / "missing-toolchain").string());
    EXPECT_EQ(
        BuildPlanFromManifest(
            request, CreateManifest(), workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
}

TEST(KernelCompilationPlanBuilderTest, RejectsInvalidManifestSelectionInputs)
{
    KernelPlanTestWorkspace workspace;
    ScopedEnvironmentVariable toolchainEnvironment("ACLRTC_TEST_TOOLCHAIN", workspace.ToolchainPath().string());
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", workspace.ToolchainPath().string());
    const uint8_t constantBytes[] = {0x01, 0x02};
    const void* argumentDataPointers[] = {constantBytes};
    const uint64_t argumentByteCounts[] = {sizeof(constantBytes)};
    NormalizedKernelSpecializationRequest request = CreateRequest(workspace, argumentDataPointers, argumentByteCounts);
    KernelCompilationPlan plan;

    EXPECT_EQ(
        BuildPlanFromManifest(request, CreateManifest(), "relative-worktree", workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);

    Json unsupportedSchemaManifest = CreateManifest();
    unsupportedSchemaManifest["schema_version"] = "2.0";
    EXPECT_EQ(
        BuildPlanFromManifest(
            request, unsupportedSchemaManifest, workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);

    request.kernelName = "missing_kernel";
    EXPECT_EQ(
        BuildPlanFromManifest(
            request, CreateManifest(), workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);

    request.kernelName = "add_custom_100000";
    Json malformedSchemaManifest = CreateManifest();
    malformedSchemaManifest["schema_version"] = Json::array();
    EXPECT_EQ(
        BuildPlanFromManifest(
            request, malformedSchemaManifest, workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_ERROR_FAILURE);
}

TEST(KernelCompilationPlanBuilderTest, RejectsMissingEnvironmentVariablesAtEachExpansionLayer)
{
    KernelPlanTestWorkspace workspace;
    ScopedEnvironmentVariable toolchainEnvironment("ACLRTC_TEST_TOOLCHAIN", workspace.ToolchainPath().string());
    ScopedEnvironmentVariable missingEnvironment("ACLRTC_TEST_MISSING_ENV", "temporarily-set");
    ScopedEnvironmentVariable ascendHomeEnvironment("ASCEND_HOME_PATH", workspace.ToolchainPath().string());
    ASSERT_EQ(unsetenv("ACLRTC_TEST_MISSING_ENV"), 0);
    const uint8_t tilingBytes[] = {0x01, 0x02};
    const void* argumentAddresses[] = {tilingBytes};
    const uint64_t argumentByteCounts[] = {sizeof(tilingBytes)};
    const NormalizedKernelSpecializationRequest request =
        CreateRequest(workspace, argumentAddresses, argumentByteCounts);

    Json manifest = CreateManifest();
    manifest["options"]["common_compile"] = Json::array({"${env:ACLRTC_TEST_MISSING_ENV}/include"});
    KernelCompilationPlan plan;
    EXPECT_NE(
        BuildPlanFromManifest(request, manifest, workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_SUCCESS);

    manifest = CreateManifest();
    manifest["kernels"][1]["constant_infos"][0]["file"] = "${env:ACLRTC_TEST_MISSING_ENV}/constants.h";
    EXPECT_NE(
        BuildPlanFromManifest(request, manifest, workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_SUCCESS);

    ASSERT_EQ(unsetenv("ASCEND_HOME_PATH"), 0);
    manifest = CreateManifest();
    EXPECT_NE(
        BuildPlanFromManifest(request, manifest, workspace.ResourcePath(), workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_SUCCESS);
}

TEST(KernelCompilationPlanBuilderTest, ReportsResourceWorktreeInspectionErrors)
{
    KernelPlanTestWorkspace workspace;
    const fs::path selfReferentialLink = workspace.ResourcePath().parent_path() / "worktree-link-loop";
    boost::system::error_code symlinkError;
    fs::create_symlink(selfReferentialLink.filename(), selfReferentialLink, symlinkError);
    ASSERT_FALSE(symlinkError);
    const uint8_t tilingBytes[] = {0x01, 0x02};
    const void* argumentAddresses[] = {tilingBytes};
    const uint64_t argumentByteCounts[] = {sizeof(tilingBytes)};
    const NormalizedKernelSpecializationRequest request =
        CreateRequest(workspace, argumentAddresses, argumentByteCounts);

    KernelCompilationPlan plan;
    EXPECT_NE(
        BuildPlanFromManifest(request, CreateManifest(), selfReferentialLink, workspace.SourceDirectoryPath(), plan),
        ascendc::aclrtc::ACLRTC_SUCCESS);
}
} // namespace
