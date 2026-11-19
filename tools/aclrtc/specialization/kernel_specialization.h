/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef ASCENDC_ACLRTC_KERNEL_SPECIALIZATION_H
#define ASCENDC_ACLRTC_KERNEL_SPECIALIZATION_H

#include "kernel_specialization_types.h"
#include "kernel_compilation_memory_cache.h"

#include <memory>

#include <nlohmann/json.hpp>

namespace ascendc {
namespace specialization_compile {
struct ResourceEntry;
} // namespace specialization_compile
namespace aclrtc {

class KernelCompilationWorkspace;
class KernelSpecializationDiagnostics;
struct PreparedKernelCompilationPlan;
struct KernelCompilationCacheInput;
struct KernelSpecializationInputs;
struct KernelElfBuildOutput;
struct KernelPreprocessingSnapshot;
struct PreprocessedCompilationSource;

struct MaterializedKernelCompilationResource {
    nlohmann::json manifest;
    boost::filesystem::path ownedResourceDirectoryPath;
    boost::filesystem::path externalSourceDirectoryPath;
};

class KernelSpecializationSession final {
public:
    aclError RunKernelSpecialization(const aclrtcKernelSpecRequest* apiRequest, const char* outputElfPath) const;

private:
    static bool BuildPreprocessedCacheInput(
        const KernelSpecializationInputs& inputs, KernelPreprocessingSnapshot& snapshot);
    static aclError PrepareSpecializationInputs(
        const NormalizedKernelSpecializationRequest& specializationRequest,
        const specialization_compile::ResourceEntry*& resource, PreparedKernelCompilationPlan& preparedPlan,
        KernelCompilationCacheInput& cacheInput, aclError& preparationStatus);

    aclError RunUncachedKernelSpecialization(
        const KernelSpecializationInputs& inputs, aclError preparationStatus) const;
    aclError GetAndPublishKernelElf(const KernelSpecializationInputs& inputs) const;
    aclError GetOrBuildKernelElf(
        const KernelSpecializationInputs& inputs, KernelPreprocessingSnapshot& snapshot,
        KernelElfBuildOutput& output) const;
    static aclError CompileKernelPlan(
        const NormalizedKernelSpecializationRequest& request, const MaterializedKernelCompilationResource& resource,
        const PreparedKernelCompilationPlan& preparedPlan, const KernelCompilationCacheInput& cacheInput,
        const std::vector<PreprocessedCompilationSource>* preprocessedSources, KernelElfBuildOutput& output);

    static aclError LoadCompilationResource(
        const std::string& resourceId, const specialization_compile::ResourceEntry*& resource);
    static aclError MaterializeCompilationResource(
        const std::string& resourceId, const specialization_compile::ResourceEntry& resource,
        MaterializedKernelCompilationResource& compilationResource);

    aclError RunSpecializationWithMaterializedResource(
        const NormalizedKernelSpecializationRequest& specializationRequest,
        MaterializedKernelCompilationResource compilationResource, const PreparedKernelCompilationPlan& preparedPlan,
        const KernelCompilationCacheInput& cacheInput, aclError preparationStatus) const;

    static aclError ReportSpecializationStatus(
        const NormalizedKernelSpecializationRequest& specializationRequest,
        KernelSpecializationDiagnostics* diagnostics, aclError specializationStatus, const char* stage) noexcept;

    aclError PublishKernelElf(
        const boost::filesystem::path& linkedKernelElfPath, const boost::filesystem::path& outputElfPath,
        KernelCompilationWorkspace& compilationWorkspace) const;
};

} // namespace aclrtc
} // namespace ascendc

#endif // ASCENDC_ACLRTC_KERNEL_SPECIALIZATION_H
