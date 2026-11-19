/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "kernel_compilation_cache_input.h"

#include "ascendc_tool_log.h"
#include "cache_digest.h"
#include "process_compilation_context.h"
#include "resource_registry.h"

#include <algorithm>
#include <boost/filesystem.hpp>

namespace ascendc {
namespace aclrtc {
namespace {
using Json = nlohmann::json;
using specialization_compile::ResourceEntry;
using specialization_compile::ResourceFileData;
constexpr char RESOURCE_IDENTITY_DOMAIN[] = "aclrtc.resource.v1";
constexpr char COMPILATION_INPUT_DOMAIN[] = "aclrtc.compile-input.v2";
constexpr char ARTIFACT_IDENTITY_DOMAIN[] = "aclrtc.artifact.v1";

const Json& DescribeResourceIdentity(const ResourceEntry& resource)
{
    std::call_once(resource.cacheIdentityOnce, [&resource]() {
        std::vector<const ResourceFileData*> files;
        for (const ResourceFileData& file : resource.files) {
            files.push_back(&file);
        }
        std::sort(files.begin(), files.end(), [](const auto* left, const auto* right) {
            return left->relativePath < right->relativePath;
        });
        Json payload{{"manifest", resource.data.json}, {"files", Json::array()}};
        for (const ResourceFileData* file : files) {
            payload["files"].push_back(
                {{"path", file->relativePath},
                 {"size", std::to_string(file->bytes.size())},
                 {"sha256", ComputeSha256(file->bytes.data(), file->bytes.size())}});
        }
        resource.cacheIdentity = {
            {"inputs", payload}, {"digest", ComputeCacheIdentityDigest(RESOURCE_IDENTITY_DOMAIN, payload)}};
    });
    return resource.cacheIdentity;
}

Json DescribeCompilationPlan(const PreparedKernelCompilationPlan& plan)
{
    // Keep temporary paths and diagnostic labels out of the cache identity.
    Json commands = Json::array();
    for (const PreparedCompilationCommand& command : plan.compilationCommands) {
        Json description{
            {"kind", static_cast<uint32_t>(command.commandKind)},
            {"executable", command.executable},
            {"argv", command.arguments},
            {"compiler_options", command.compilerOptions}};
        if (command.kernelSymbolRename.has_value()) {
            description["rename"] = {
                {"original", command.kernelSymbolRename->original}, {"renamed", command.kernelSymbolRename->renamed}};
        }
        commands.push_back(std::move(description));
    }
    return {
        {"commands", std::move(commands)},
        {"source_directory", plan.sourceDirectory},
        {"path_policy", "template-path-v1"}};
}

Json DescribeConstantBindings(const KernelConstantSnapshot& snapshot)
{
    Json bindings = Json::array();
    for (const KernelConstantBindingValue& binding : snapshot.bindings) {
        const auto& bytes = snapshot.argumentBytes.at(binding.parameterIndex);
        bindings.push_back(
            {{"parameter_index", std::to_string(binding.parameterIndex)},
             {"size", std::to_string(binding.byteCount)},
             {"sha256", ComputeSha256(bytes.data(), binding.byteCount)}});
    }
    return bindings;
}

} // namespace

aclError CaptureKernelConstants(
    const NormalizedKernelSpecializationRequest& request, const std::vector<ManifestConstant>& constants,
    KernelConstantSnapshot& snapshot)
{
    KernelConstantSnapshot result;
    std::map<uint64_t, uint64_t> argumentByteCounts;
    for (ManifestConstant constant : constants) {
        if (!constant.BindArgument(
                request.kernelArgumentCount, request.borrowedKernelArgumentDataPointers,
                request.borrowedKernelArgumentByteCounts)) {
            return ACLRTC_ERROR_INVALID_INPUT;
        }
        const uint64_t index = constant.GetParameterIndex();
        const uint64_t size = constant.GetBoundByteSize();
        result.bindings.push_back({index, size});
        argumentByteCounts[index] = std::max(argumentByteCounts[index], size);
    }
    // Read caller-owned buffers only after every binding has been validated.
    for (const auto& argument : argumentByteCounts) {
        const auto* bytes = static_cast<const uint8_t*>(request.borrowedKernelArgumentDataPointers[argument.first]);
        result.argumentBytes.emplace(argument.first, std::vector<uint8_t>(bytes, bytes + argument.second));
    }
    snapshot = std::move(result);
    return ACLRTC_SUCCESS;
}

void BuildKernelCompilationCacheIdentity(
    const NormalizedKernelSpecializationRequest& request, const ResourceEntry& resource,
    const PreparedKernelCompilationPlan& plan, KernelCompilationCacheInput& cacheInput)
{
    auto& context = ProcessCompilationContext::Instance();
    const auto& timestamps = context.GetToolchainTimestamps(plan.environmentValues.at("ASCEND_HOME_PATH"));
    const Json& resourceIdentity = DescribeResourceIdentity(resource);
    Json environment = Json::object();
    for (const auto& variable : plan.compilerEnvironment) {
        environment[variable.first] = variable.second.has_value() ? Json(*variable.second) : Json(nullptr);
    }
    boost::system::error_code error;
    const auto workingDirectory = boost::filesystem::current_path(error);
    if (error) {
        ASCENDLOGW("Failed to get compilation working directory: error=%s; skip cache", error.message().c_str());
    }
    const Json base{
        {"working_directory", error ? Json(nullptr) : Json(workingDirectory.string())},
        {"resource", resourceIdentity.at("digest")},
        {"entry", request.kernelName},
        {"toolchain", timestamps.has_value() ? Json(*timestamps) : Json(nullptr)},
        {"plan", DescribeCompilationPlan(plan)},
        {"environment", std::move(environment)},
        {"referenced_environment", plan.environmentValues}};
    const Json bindings = DescribeConstantBindings(cacheInput.constants);
    cacheInput.baseKey.clear();
    cacheInput.key.clear();
    if (timestamps.has_value() && !error) {
        cacheInput.baseKey = ComputeCacheIdentityDigest(COMPILATION_INPUT_DOMAIN, base);
        cacheInput.key = ComputeCacheIdentityDigest(
            ARTIFACT_IDENTITY_DOMAIN, {{"base", cacheInput.baseKey}, {"bindings", bindings}});
    }
    cacheInput.description = {
        {"schema", 1},
        {"domains",
         {{"resource", RESOURCE_IDENTITY_DOMAIN},
          {"base", COMPILATION_INPUT_DOMAIN},
          {"artifact", ARTIFACT_IDENTITY_DOMAIN}}},
        {"resource_inputs", resourceIdentity.at("inputs")},
        {"base_inputs", base},
        {"bindings", bindings},
        {"base", cacheInput.baseKey},
        {"key", cacheInput.key}};
}

} // namespace aclrtc
} // namespace ascendc
