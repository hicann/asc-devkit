/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <vector>

#include "file_utils.h"
#include "test_support.h"

#define main AscCompileExporterProgramMain
#include "asc_compile_exporter.cpp"
#undef main

namespace {

using asc_compile_exporter_test::ModuleTest;
using asc_compile_exporter_test::WriteTestFile;
using ascendc::FileUtils;
using ascendc::manifest_generator::BuildCollectedBundleRequest;
using ascendc::manifest_generator::BundleOutputKind;

class CerrCapture final {
public:
    CerrCapture() : previous_(std::cerr.rdbuf(stream_.rdbuf())) {}

    ~CerrCapture() { std::cerr.rdbuf(previous_); }

    std::string Content() const { return stream_.str(); }

    CerrCapture(const CerrCapture&) = delete;
    CerrCapture& operator=(const CerrCapture&) = delete;

private:
    std::ostringstream stream_;
    std::streambuf* previous_;
};

bool ParseCommand(std::vector<std::string> arguments, BuildCollectedBundleRequest& request)
{
    std::vector<char*> argv;
    argv.reserve(arguments.size());
    for (std::string& argument : arguments) {
        argv.push_back(const_cast<char*>(argument.c_str()));
    }
    return ParseArguments(static_cast<int>(argv.size()), argv.data(), request);
}

int RunCommand(std::vector<std::string> arguments)
{
    std::vector<char*> argv;
    argv.reserve(arguments.size());
    for (std::string& argument : arguments) {
        argv.push_back(const_cast<char*>(argument.c_str()));
    }
    return ::Run(static_cast<int>(argv.size()), argv.data());
}

int RunProgramCommand(std::vector<std::string> arguments)
{
    std::vector<char*> argv;
    argv.reserve(arguments.size());
    for (std::string& argument : arguments) {
        argv.push_back(const_cast<char*>(argument.c_str()));
    }
    return AscCompileExporterProgramMain(static_cast<int>(argv.size()), argv.data());
}

TEST(CompileExporterCliUnitTest, ParseJobsAcceptsOnlyPositiveUint32Values)
{
    uint32_t jobs = 99U;
    EXPECT_TRUE(ParseJobs("1", jobs));
    EXPECT_EQ(jobs, 1U);
    EXPECT_TRUE(ParseJobs("4294967295", jobs));
    EXPECT_EQ(jobs, 4294967295U);

    EXPECT_FALSE(ParseJobs("0", jobs));
    EXPECT_FALSE(ParseJobs("-1", jobs));
    EXPECT_FALSE(ParseJobs("4jobs", jobs));
    EXPECT_FALSE(ParseJobs("4294967296", jobs));
    EXPECT_FALSE(ParseJobs("", jobs));
    EXPECT_FALSE(ParseJobs(std::string(128U, '9'), jobs));
    EXPECT_EQ(jobs, 4294967295U);
}

TEST(CompileExporterCliUnitTest, ResolveJobCountUsesCpuLimitAndDefault)
{
    const uint32_t reportedCpuCount = static_cast<uint32_t>(std::thread::hardware_concurrency());
    const uint32_t cpuCount = reportedCpuCount == 0U ? 1U : reportedCpuCount;
    EXPECT_EQ(ResolveJobCount(UINT32_MAX), cpuCount);
    EXPECT_EQ(ResolveJobCount(0U), cpuCount < 2U ? 1U : cpuCount / 2U);
}

TEST_F(ModuleTest, CliParsesKeywordArgumentsInAnyOrder)
{
    const std::string manifestRoot = FileUtils::JoinPath(root_, "collection");
    const std::string output = FileUtils::JoinPath(root_, "output/bundle.so");
    ASSERT_TRUE(FileUtils::CreateDirectories(manifestRoot));

    BuildCollectedBundleRequest request;
    ASSERT_TRUE(ParseCommand(
        {"asc_compile_exporter", "--cxx", "/usr/bin/c++", "--save-temp-files", "--output", output, "--jobs", "3",
         "--input-dir", manifestRoot, "--make", "/usr/bin/make"},
        request));

    std::string resolvedManifestRoot;
    ASSERT_TRUE(FileUtils::ResolveDirectory(manifestRoot, resolvedManifestRoot));
    std::string resolvedOutputParent;
    ASSERT_TRUE(FileUtils::ResolveDirectory(FileUtils::ParentPath(output), resolvedOutputParent));
    EXPECT_EQ(request.manifestSearchRoot, resolvedManifestRoot);
    EXPECT_EQ(request.outputPath, FileUtils::JoinPath(resolvedOutputParent, "bundle.so"));
    EXPECT_EQ(request.outputKind, BundleOutputKind::kSharedObject);
    EXPECT_EQ(request.makeExecutable, "/usr/bin/make");
    EXPECT_EQ(request.cxxCompiler, "/usr/bin/c++");
    EXPECT_TRUE(request.keepTemp);
    EXPECT_EQ(request.jobs, ResolveJobCount(3U));
}

TEST_F(ModuleTest, CliUsesDefaultValues)
{
    const std::string manifestRoot = FileUtils::JoinPath(root_, "collection");
    const std::string output = FileUtils::JoinPath(root_, "output/bundle.o");
    ASSERT_TRUE(FileUtils::CreateDirectories(manifestRoot));
    BuildCollectedBundleRequest request;
    ASSERT_TRUE(ParseCommand(
        {"asc_compile_exporter", "--input-dir", manifestRoot, "--output", output, "--make", "/usr/bin/make", "--cxx",
         "/usr/bin/c++"},
        request));
    std::string resolvedOutputParent;
    ASSERT_TRUE(FileUtils::ResolveDirectory(FileUtils::ParentPath(output), resolvedOutputParent));
    EXPECT_EQ(request.outputPath, FileUtils::JoinPath(resolvedOutputParent, "bundle.o"));
    EXPECT_EQ(request.outputKind, BundleOutputKind::kRelocatableObject);
    EXPECT_FALSE(request.keepTemp);
    EXPECT_EQ(request.jobs, ResolveJobCount(0U));
}

TEST_F(ModuleTest, CliRejectsUnknownPositionalAndMissingArguments)
{
    CerrCapture capture;
    BuildCollectedBundleRequest request;
    EXPECT_FALSE(ParseCommand({"asc_compile_exporter"}, request));
    EXPECT_FALSE(ParseCommand({"asc_compile_exporter", "collection"}, request));
    EXPECT_FALSE(ParseCommand({"asc_compile_exporter", "--unknown"}, request));
    EXPECT_FALSE(ParseCommand({"asc_compile_exporter", "--cxx"}, request));
    EXPECT_FALSE(ParseCommand({"asc_compile_exporter", "--cxx", "--unknown"}, request));

    const std::string output = capture.Content();
    EXPECT_NE(output.find("Missing required argument: --input-dir"), std::string::npos);
    EXPECT_NE(output.find("Unable to parse argument: collection"), std::string::npos);
    EXPECT_NE(output.find("Unable to parse argument: --unknown"), std::string::npos);
    EXPECT_NE(output.find("Missing value for argument: --cxx"), std::string::npos);
    EXPECT_NE(output.find("Missing value for argument --cxx: --unknown"), std::string::npos);
}

TEST_F(ModuleTest, CliRejectsDuplicateArguments)
{
    CerrCapture capture;
    const std::string manifestRoot = FileUtils::JoinPath(root_, "collection");
    ASSERT_TRUE(FileUtils::CreateDirectories(manifestRoot));
    BuildCollectedBundleRequest request;
    EXPECT_FALSE(
        ParseCommand({"asc_compile_exporter", "--input-dir", manifestRoot, "--input-dir", manifestRoot}, request));
    EXPECT_FALSE(ParseCommand({"asc_compile_exporter", "--save-temp-files", "--save-temp-files"}, request));
    EXPECT_FALSE(ParseCommand({"asc_compile_exporter", "--jobs", "1", "--jobs", "2"}, request));

    const std::string output = capture.Content();
    EXPECT_NE(output.find("Duplicate argument: --input-dir"), std::string::npos);
    EXPECT_NE(output.find("Duplicate argument: --save-temp-files"), std::string::npos);
    EXPECT_NE(output.find("Duplicate argument: --jobs"), std::string::npos);
}

TEST_F(ModuleTest, CliValidatesArgumentsWhenRead)
{
    CerrCapture capture;
    BuildCollectedBundleRequest request;
    const std::string missingManifestRoot = FileUtils::JoinPath(root_, "missing");
    EXPECT_FALSE(ParseCommand({"asc_compile_exporter", "--input-dir", missingManifestRoot}, request));
    EXPECT_FALSE(ParseCommand({"asc_compile_exporter", "--output", "bundle.so"}, request));
    EXPECT_FALSE(ParseCommand({"asc_compile_exporter", "--output", FileUtils::JoinPath(root_, "output/")}, request));
    EXPECT_FALSE(ParseCommand({"asc_compile_exporter", "--output", "/dev/null/bundle.so"}, request));
    const std::string outputDirectory = FileUtils::JoinPath(root_, "real-output");
    const std::string outputDirectoryLink = FileUtils::JoinPath(root_, "output-link");
    ASSERT_TRUE(FileUtils::CreateDirectories(outputDirectory));
    ASSERT_EQ(symlink(outputDirectory.c_str(), outputDirectoryLink.c_str()), 0);
    EXPECT_FALSE(ParseCommand(
        {"asc_compile_exporter", "--output", FileUtils::JoinPath(outputDirectoryLink, "bundle.so")}, request));
    const std::string unsupportedOutputParent = FileUtils::JoinPath(root_, "unsupported-output");
    EXPECT_FALSE(ParseCommand(
        {"asc_compile_exporter", "--output", FileUtils::JoinPath(unsupportedOutputParent, "bundle")}, request));
    EXPECT_FALSE(ParseCommand(
        {"asc_compile_exporter", "--output", FileUtils::JoinPath(unsupportedOutputParent, "bundle.a")}, request));
    EXPECT_FALSE(ParseCommand(
        {"asc_compile_exporter", "--output", FileUtils::JoinPath(unsupportedOutputParent, "bundle.obj")}, request));
    EXPECT_FALSE(ParseCommand(
        {"asc_compile_exporter", "--output", FileUtils::JoinPath(unsupportedOutputParent, "bundle.so.1")}, request));
    EXPECT_FALSE(ParseCommand(
        {"asc_compile_exporter", "--output", FileUtils::JoinPath(unsupportedOutputParent, "bundle.SO")}, request));
    EXPECT_FALSE(FileUtils::IsDirectory(unsupportedOutputParent));
    EXPECT_FALSE(ParseCommand({"asc_compile_exporter", "--make", ""}, request));
    EXPECT_FALSE(ParseCommand({"asc_compile_exporter", "--jobs", "0"}, request));

    const std::string output = capture.Content();
    EXPECT_NE(output.find(missingManifestRoot), std::string::npos);
    EXPECT_NE(output.find("Invalid value for argument --output: bundle.so"), std::string::npos);
    EXPECT_NE(output.find(FileUtils::JoinPath(root_, "output/")), std::string::npos);
    EXPECT_NE(output.find("/dev/null/bundle.so"), std::string::npos);
    EXPECT_NE(output.find(FileUtils::JoinPath(outputDirectoryLink, "bundle.so")), std::string::npos);
    EXPECT_NE(output.find("Unsupported output suffix for argument --output (expected .so or .o)"), std::string::npos);
    EXPECT_NE(output.find("Invalid value for argument: --make"), std::string::npos);
    EXPECT_NE(output.find("Invalid value for argument --jobs: 0"), std::string::npos);
}

TEST_F(ModuleTest, RunPassesValidatedArgumentsToCompiler)
{
    const std::string manifestRoot = FileUtils::JoinPath(root_, "collection");
    const std::string output = FileUtils::JoinPath(root_, "output/bundle.so");
    ASSERT_TRUE(FileUtils::CreateDirectories(manifestRoot));
    EXPECT_EQ(
        RunCommand(
            {"asc_compile_exporter", "--input-dir", manifestRoot, "--output", output, "--make", "/usr/bin/make",
             "--cxx", "/usr/bin/c++"}),
        1);
    EXPECT_TRUE(FileUtils::IsDirectory(FileUtils::ParentPath(output)));
}

TEST_F(ModuleTest, RunBuildsBundleWithKeywordArguments)
{
    const std::string manifestRoot = FileUtils::JoinPath(root_, "collection");
    const std::string unit = FileUtils::JoinPath(manifestRoot, "Unit");
    const std::string output = FileUtils::JoinPath(root_, "output/bundle.so");
    const std::string objectOutput = FileUtils::JoinPath(root_, "output/bundle.o");
    WriteTestFile(FileUtils::JoinPath(unit, "resources/kernel.cpp"), "source");
    CreateManifest(unit, "Unit");

    EXPECT_EQ(
        RunCommand(
            {"asc_compile_exporter", "--output", output, "--make", "/usr/bin/make", "--input-dir", manifestRoot,
             "--cxx", "/usr/bin/c++", "--jobs", "1"}),
        0);
    EXPECT_TRUE(FileUtils::IsRegularFile(output));

    EXPECT_EQ(
        RunCommand(
            {"asc_compile_exporter", "--output", objectOutput, "--make", "/usr/bin/make", "--input-dir", manifestRoot,
             "--cxx", "/usr/bin/c++", "--jobs", "1"}),
        0);
    EXPECT_TRUE(FileUtils::IsRegularFile(objectOutput));
}

TEST(CompileExporterCliUnitTest, ProgramMainReturnsRunStatusAndPrintsArgument)
{
    CerrCapture capture;
    EXPECT_EQ(RunProgramCommand({"asc_compile_exporter", "--unknown"}), 1);
    EXPECT_NE(capture.Content().find("Unable to parse argument: --unknown"), std::string::npos);
    EXPECT_NE(capture.Content().find("Usage:"), std::string::npos);
    EXPECT_NE(capture.Content().find("Required arguments:"), std::string::npos);
    EXPECT_NE(capture.Content().find("Optional arguments:"), std::string::npos);
}

} // namespace
