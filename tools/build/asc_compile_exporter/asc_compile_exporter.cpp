/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <algorithm>
#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <thread>
#include <utility>

#include "ascendc_tool_log.h"
#include "file_utils.h"
#include "manifest_bundle_compiler.h"

namespace {
void PrintUsage(std::ostream& output, const char* program)
{
    output << "Usage:\n  " << program
           << " --input-dir <path> --output <path> --make <path> --cxx <path>"
              " [--save-temp-files] [--jobs <N>]\n  "
           << program
           << " -h | --help\n\n"
              "Required arguments:\n"
              "  --input-dir <path>       Directory recursively searched for *_manifest.json files.\n"
              "  --output <path>          Output path ending in .so or .o, including its parent directory.\n"
              "  --make <path>            Path to the make executable used to build generated sources.\n"
              "  --cxx <path>             Path to the C++ compiler used by the generated Makefile.\n"
              "\nOptional arguments:\n"
              "  --save-temp-files        Preserve the private build directory under TMPDIR or /tmp.\n"
              "  --jobs <N>               Positive make job count, capped at the detected CPU count;\n"
              "                             defaults to half the CPU count, with a minimum of one.\n"
              "  -h, --help               Print this usage and exit.\n";
}

bool IsHelpRequested(int argc, char** argv)
{
    for (int index = 1; index < argc; ++index) {
        if (argv[index] != nullptr) {
            const std::string argument = argv[index];
            if (argument == "-h" || argument == "--help") {
                return true;
            }
        }
    }
    return false;
}

bool ReportArgumentError(const char* reason, const std::string& argument)
{
    std::cerr << reason << ": " << argument << '\n';
    return false;
}

bool ParseJobs(const std::string& text, uint32_t& jobs)
{
    if (text.empty()) {
        return false;
    }
    errno = 0;
    char* end = nullptr;
    const unsigned long parsed = std::strtoul(text.c_str(), &end, 10);
    if (errno == ERANGE || end == text.c_str() || *end != '\0' || parsed == 0UL || parsed > UINT32_MAX) {
        return false;
    }
    jobs = static_cast<uint32_t>(parsed);
    return true;
}

uint32_t ResolveJobCount(uint32_t requested) noexcept
{
    const uint32_t reportedCpuCount = static_cast<uint32_t>(std::thread::hardware_concurrency());
    const uint32_t cpuCount = reportedCpuCount == 0U ? 1U : reportedCpuCount;
    if (requested != 0U) {
        return std::min(requested, cpuCount);
    }
    return cpuCount < 2U ? 1U : cpuCount / 2U;
}

bool ReadOptionValue(int argc, char** argv, int& index, const std::string& option, bool& specified, std::string& value)
{
    if (specified) {
        return ReportArgumentError("Duplicate argument", option);
    }
    if (index + 1 >= argc) {
        return ReportArgumentError("Missing value for argument", option);
    }
    const std::string candidate = argv[index + 1];
    if (candidate.empty()) {
        return ReportArgumentError("Invalid value for argument", option);
    }
    if (candidate.compare(0U, 2U, "--") == 0) {
        std::cerr << "Missing value for argument " << option << ": " << candidate << '\n';
        return false;
    }
    value = candidate;
    specified = true;
    ++index;
    return true;
}

bool PrepareOutputPath(
    const std::string& argument, std::string& output, ascendc::manifest_generator::BundleOutputKind& outputKind)
{
    const std::string outputParent = ascendc::FileUtils::ParentPath(argument);
    const std::string outputName = ascendc::FileUtils::FileName(argument);
    if (outputParent.empty() || outputName.empty() || outputName == "." || outputName == "..") {
        return ReportArgumentError("Invalid value for argument --output", argument);
    }
    if (outputName.size() >= 3U && outputName.compare(outputName.size() - 3U, 3U, ".so") == 0) {
        outputKind = ascendc::manifest_generator::BundleOutputKind::kSharedObject;
    } else if (outputName.size() >= 2U && outputName.compare(outputName.size() - 2U, 2U, ".o") == 0) {
        outputKind = ascendc::manifest_generator::BundleOutputKind::kRelocatableObject;
    } else {
        return ReportArgumentError("Unsupported output suffix for argument --output (expected .so or .o)", argument);
    }
    std::string absoluteOutputParent;
    if (!ascendc::FileUtils::MakeAbsolutePath(outputParent, absoluteOutputParent) ||
        !ascendc::FileUtils::CreateDirectories(absoluteOutputParent)) {
        return ReportArgumentError("Invalid value for argument --output", argument);
    }
    std::string resolvedOutputParent;
    if (!ascendc::FileUtils::ResolveDirectory(absoluteOutputParent, resolvedOutputParent)) {
        return ReportArgumentError("Invalid value for argument --output", argument);
    }
    output = ascendc::FileUtils::JoinPath(resolvedOutputParent, outputName);
    return true;
}

struct ArgumentState {
    ascendc::manifest_generator::BuildCollectedBundleRequest request;
    bool inputDirSpecified{false};
    bool outputSpecified{false};
    bool makeSpecified{false};
    bool cxxSpecified{false};
    bool saveTempFilesSpecified{false};
    bool jobsSpecified{false};
};

bool ParseInputDirectory(int argc, char** argv, int& index, const std::string& argument, ArgumentState& state)
{
    std::string value;
    if (!ReadOptionValue(argc, argv, index, argument, state.inputDirSpecified, value)) {
        return false;
    }
    if (!ascendc::FileUtils::ResolveDirectory(value, state.request.manifestSearchRoot)) {
        return ReportArgumentError("Invalid value for argument --input-dir", value);
    }
    return true;
}

bool ParseOutput(int argc, char** argv, int& index, const std::string& argument, ArgumentState& state)
{
    std::string value;
    return ReadOptionValue(argc, argv, index, argument, state.outputSpecified, value) &&
           PrepareOutputPath(value, state.request.outputPath, state.request.outputKind);
}

bool ParseJobCount(int argc, char** argv, int& index, const std::string& argument, ArgumentState& state)
{
    std::string value;
    if (!ReadOptionValue(argc, argv, index, argument, state.jobsSpecified, value)) {
        return false;
    }
    uint32_t jobs = 0U;
    if (!ParseJobs(value, jobs)) {
        return ReportArgumentError("Invalid value for argument --jobs", value);
    }
    state.request.jobs = ResolveJobCount(jobs);
    return true;
}

bool ParseArgument(int argc, char** argv, int& index, ArgumentState& state)
{
    const std::string argument = argv[index];
    if (argument == "--input-dir") {
        return ParseInputDirectory(argc, argv, index, argument, state);
    }
    if (argument == "--output") {
        return ParseOutput(argc, argv, index, argument, state);
    }
    if (argument == "--make") {
        return ReadOptionValue(argc, argv, index, argument, state.makeSpecified, state.request.makeExecutable);
    }
    if (argument == "--cxx") {
        return ReadOptionValue(argc, argv, index, argument, state.cxxSpecified, state.request.cxxCompiler);
    }
    if (argument == "--save-temp-files") {
        if (state.saveTempFilesSpecified) {
            return ReportArgumentError("Duplicate argument", argument);
        }
        state.saveTempFilesSpecified = true;
        state.request.keepTemp = true;
        return true;
    }
    if (argument == "--jobs") {
        return ParseJobCount(argc, argv, index, argument, state);
    }
    return ReportArgumentError("Unable to parse argument", argument);
}

bool ValidateRequiredArguments(const ArgumentState& state)
{
    bool complete = true;
    if (!state.inputDirSpecified) {
        complete = ReportArgumentError("Missing required argument", "--input-dir");
    }
    if (!state.outputSpecified) {
        complete = ReportArgumentError("Missing required argument", "--output");
    }
    if (!state.makeSpecified) {
        complete = ReportArgumentError("Missing required argument", "--make");
    }
    if (!state.cxxSpecified) {
        complete = ReportArgumentError("Missing required argument", "--cxx");
    }
    return complete;
}

bool ParseArguments(int argc, char** argv, ascendc::manifest_generator::BuildCollectedBundleRequest& request)
{
    ArgumentState state;
    state.request.jobs = ResolveJobCount(0U);
    for (int index = 1; index < argc; ++index) {
        if (!ParseArgument(argc, argv, index, state)) {
            return false;
        }
    }
    if (!ValidateRequiredArguments(state)) {
        return false;
    }
    request = std::move(state.request);
    return true;
}

int RunWithExceptionHandling(int argc, char** argv)
{
    try {
        if (IsHelpRequested(argc, argv)) {
            PrintUsage(std::cout, argv[0]);
            return 0;
        }
        ascendc::manifest_generator::BuildCollectedBundleRequest request;
        if (!ParseArguments(argc, argv, request)) {
            PrintUsage(std::cerr, argv[0]);
            return 1;
        }
        ascendc::manifest_generator::ManifestBundleCompiler compiler(std::move(request));
        if (!compiler.Compile()) {
            std::cerr << "Compilation failed. Please check plog for details." << '\n';
            return 1;
        }
        return 0;
    } catch (const std::exception& error) {
        ASCENDLOGE("asc_compile_exporter failed unexpectedly: %s", error.what());
        std::cerr << "Compilation failed. Please check plog for details." << '\n';
        return 1;
    }
}

} // namespace

int main(int argc, char** argv) { return RunWithExceptionHandling(argc, argv); }
