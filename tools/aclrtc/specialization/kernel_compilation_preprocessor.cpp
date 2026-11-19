/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "kernel_compilation_preprocessor.h"

#include "ascendc_tool_log.h"
#include "cache_digest.h"
#include "compilation_plan_executor.h"
#include "file_utils.h"
#include "kernel_specialization_diagnostics.h"

#include <algorithm>
#include <fstream>
#include <sstream>

namespace ascendc {
namespace aclrtc {
namespace {
using Json = nlohmann::json;
constexpr char PREPROCESSED_ARTIFACT_DOMAIN[] = "aclrtc.preprocessed-artifact.v2";

void NormalizeResourceLineMarker(std::string& line, const std::string& resourcePrefix)
{
    if (line.empty() || line.front() != '#') {
        return;
    }
    const size_t number = line.find_first_not_of(" \t", 1U);
    if (number == std::string::npos || line[number] < '0' || line[number] > '9') {
        return;
    }
    const size_t quote = line.find('"', number);
    if (quote != std::string::npos && line.compare(quote + 1U, resourcePrefix.size(), resourcePrefix) == 0) {
        // Normalize only line markers; preserve expanded source strings such as __FILE__.
        line.replace(quote + 1U, resourcePrefix.size(), "${resource}/");
    }
}

bool FindCompilationSourcePath(const CompilationCommand& command, boost::filesystem::path& sourcePath)
{
    for (size_t index = 0U; index < command.arguments.size(); ++index) {
        const std::string& argument = command.arguments[index];
        if (argument == "-I" || argument == "-isystem" || argument == "-iquote" || argument == "-include" ||
            argument == "-imacros" || argument == "-x" || argument == "-o") {
            ++index;
            continue;
        }
        const std::string extension = boost::filesystem::path(argument).extension().string();
        if (!argument.empty() && argument.front() != '-' &&
            (extension == ".c" || extension == ".cc" || extension == ".cpp" || extension == ".cxx")) {
            if (!sourcePath.empty()) {
                return false;
            }
            sourcePath = argument;
        }
    }
    if (!sourcePath.empty()) {
        return true;
    }
    ASCENDLOGW(
        "Failed to identify the source input in preprocessing command: label=%s", command.diagnosticLabel.c_str());
    return false;
}

bool DescribePreprocessedSource(
    const CompilationCommand& command, const std::string& resourcePrefix, Json& description,
    PreprocessedCompilationSource& source)
{
    const auto outputOption = std::find(command.arguments.begin(), command.arguments.end(), "-o");
    if (outputOption == command.arguments.end() || outputOption + 1 == command.arguments.end()) {
        return false;
    }
    if (!FindCompilationSourcePath(command, source.sourcePath)) {
        return false;
    }
    source.preprocessedPath = *(outputOption + 1);
    const std::string& path = source.preprocessedPath.string();
    std::ifstream input;
    if (!FileUtils::OpenRegularFileForRead(path, input)) {
        return false;
    }
    std::ostringstream normalized;
    std::string line;
    while (std::getline(input, line)) {
        NormalizeResourceLineMarker(line, resourcePrefix);
        normalized << line << '\n';
    }
    if (input.bad()) {
        ASCENDLOGW("Failed to read preprocessed source: path=%s; skip cache", path.c_str());
        return false;
    }
    const std::string contents = normalized.str();
    if (contents.empty()) {
        ASCENDLOGW("Preprocessed source is empty: path=%s; skip cache", path.c_str());
        return false;
    }
    description = {{"command", command.diagnosticLabel}, {"sha256", ComputeSha256(contents.data(), contents.size())}};
    return true;
}
} // namespace

bool AddPreprocessedSourcesToCacheInput(
    const KernelCompilationPlan& preprocessingPlan, const boost::filesystem::path& resourceDirectory,
    const std::string& kernelName, KernelCompilationCacheInput& cacheInput,
    std::vector<PreprocessedCompilationSource>& preprocessedSources)
{
    preprocessedSources.clear();
    KernelSpecializationDiagnostics diagnostics({}, "preprocessing");
    CompilationPlanExecutor executor(diagnostics, CompilationCommandExecutionLimits::ProductionDefaults());
    if (executor.ExecuteCompilationPlan(preprocessingPlan, kernelName) != ACLRTC_SUCCESS) {
        return false;
    }
    Json sources = Json::array();
    for (const CompilationCommand& command : preprocessingPlan.compilationCommands) {
        Json source;
        PreprocessedCompilationSource sourceInput;
        if (!DescribePreprocessedSource(command, resourceDirectory.string() + "/", source, sourceInput)) {
            return false;
        }
        sources.push_back(std::move(source));
        preprocessedSources.push_back(std::move(sourceInput));
    }
    const std::string requestKey = cacheInput.key;
    cacheInput.key =
        ComputeCacheIdentityDigest(PREPROCESSED_ARTIFACT_DOMAIN, {{"request", requestKey}, {"sources", sources}});
    cacheInput.description["request_key"] = requestKey;
    cacheInput.description["preprocessed_sources"] = std::move(sources);
    cacheInput.description["domains"]["request"] = cacheInput.description.at("domains").at("artifact");
    cacheInput.description["domains"]["artifact"] = PREPROCESSED_ARTIFACT_DOMAIN;
    cacheInput.description["key"] = cacheInput.key;
    return true;
}

} // namespace aclrtc
} // namespace ascendc
