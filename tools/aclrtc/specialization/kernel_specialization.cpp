/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "kernel_specialization.h"

#include "file_utils.h"
#include "compilation_plan_executor.h"
#include "kernel_compilation_plan_builder.h"
#include "kernel_compilation_cache_input.h"
#include "kernel_compilation_disk_cache.h"
#include "kernel_compilation_preprocessor.h"
#include "compilation_manifest_parser.h"
#include "kernel_compilation_workspace.h"
#include "kernel_specialization_diagnostics.h"
#include "ascendc_tool_log.h"
#include "resource_registry.h"
#include "process_compilation_context.h"

#include <boost/filesystem.hpp>
#include <memory>

namespace ascendc {
namespace aclrtc {

struct KernelSpecializationInputs {
    const NormalizedKernelSpecializationRequest& request;
    const specialization_compile::ResourceEntry& resource;
    const PreparedKernelCompilationPlan& preparedPlan;
    KernelCompilationCacheInput& cacheInput;
};

struct KernelElfBuildOutput {
    KernelCompilationMemoryCache::KernelElf kernelElf;
    std::unique_ptr<KernelCompilationWorkspace> workspace;
    KernelCompilationPlan plan;
    std::unique_ptr<KernelSpecializationDiagnostics> diagnostics;
};

struct KernelPreprocessingSnapshot {
    MaterializedKernelCompilationResource resource;
    std::unique_ptr<KernelCompilationWorkspace> workspace;
    std::vector<PreprocessedCompilationSource> sources;
};

namespace {
namespace fs = boost::filesystem;
using specialization_compile::IsKernelMetaSavingEnabled;
using specialization_compile::ResourceData;
using specialization_compile::ResourceEntry;
using specialization_compile::ResourceRegistry;
using specialization_compile::ResourceStatus;

bool ShouldSaveCompilationDiagnostics()
{
    return !ProcessCompilationContext::Instance().IsCacheEnabled() && IsKernelMetaSavingEnabled();
}

void LogCacheHit(const KernelSpecializationInputs& inputs, const char* source)
{
    ASCENDLOGI(
        "Kernel specialization cache hit: session=%s resource=%s kernel=%s source=%s key=%s",
        inputs.request.specializationSessionId.c_str(), inputs.request.resourceId.c_str(),
        inputs.request.kernelName.c_str(), source, inputs.cacheInput.key.c_str());
}

void LogCacheBypass(const KernelSpecializationInputs& inputs, const char* reason)
{
    ASCENDLOGD(
        "Kernel specialization cache bypass: session=%s resource=%s kernel=%s reason=%s; use normal compilation",
        inputs.request.specializationSessionId.c_str(), inputs.request.resourceId.c_str(),
        inputs.request.kernelName.c_str(), reason);
}

KernelCompilationMemoryCache::KernelElf ReadKernelElf(const fs::path& path)
{
    constexpr uint64_t maximumKernelElfBytes = 512U * 1024U * 1024U;
    try {
        auto bytes = std::make_shared<std::vector<uint8_t>>();
        if (FileUtils::ReadRegularFile(path.string(), maximumKernelElfBytes, *bytes)) {
            return bytes;
        }
        ASCENDLOGW("Failed to read compiled ELF into memory: path=%s; skip cache and use file delivery", path.c_str());
    } catch (const std::bad_alloc&) {
        ASCENDLOGW("Failed to allocate kernel compilation artifact: path=%s; use file delivery", path.c_str());
    }
    return {};
}

void InitializeCompilationDiagnostics(
    const NormalizedKernelSpecializationRequest& request, const MaterializedKernelCompilationResource& resource,
    KernelElfBuildOutput& output)
{
    const bool saveKernelMetaEnabled = ShouldSaveCompilationDiagnostics();
    output.diagnostics = std::make_unique<KernelSpecializationDiagnostics>(
        saveKernelMetaEnabled ? output.workspace->GetWorktreePath() : fs::path(), request.specializationSessionId);
    output.diagnostics->WriteManifestSnapshot(resource.manifest);
}

void InitializeCompilationWorkspace(
    const NormalizedKernelSpecializationRequest& request, MaterializedKernelCompilationResource& resource,
    KernelElfBuildOutput& output)
{
    const bool saveKernelMetaEnabled = ShouldSaveCompilationDiagnostics();
    output.workspace = std::make_unique<KernelCompilationWorkspace>(
        std::move(resource.ownedResourceDirectoryPath), saveKernelMetaEnabled ?
                                                            WorktreeRetentionPolicy::RetainAfterCompilation :
                                                            WorktreeRetentionPolicy::RemoveAfterCompilation);
    InitializeCompilationDiagnostics(request, resource, output);
}

} // namespace

aclError KernelSpecializationSession::LoadCompilationResource(
    const std::string& resourceId, const ResourceEntry*& resource)
{
    ResourceRegistry& resourceRegistry = ResourceRegistry::Instance();
    const ResourceStatus loadStatus = resourceRegistry.Load(nullptr);
    if (loadStatus != ResourceStatus::Success) {
        ASCENDLOGE(
            "Failed to load compilation resources before specializing resourceId '%s': status=%d; check the "
            "preceding ResourceRegistry logs and configured OPP paths",
            resourceId.c_str(), static_cast<int32_t>(loadStatus));
        return ACLRTC_ERROR_FAILURE;
    }

    const ResourceStatus lookupStatus = resourceRegistry.Lookup(resourceId.c_str(), resource);
    if (lookupStatus != ResourceStatus::Success) {
        ASCENDLOGE(
            "Failed to find compilation resource '%s': status=%d; verify resourceId and check the "
            "preceding ResourceRegistry logs",
            resourceId.c_str(), static_cast<int32_t>(lookupStatus));
        return ACLRTC_ERROR_FAILURE;
    }
    return ACLRTC_SUCCESS;
}

aclError KernelSpecializationSession::MaterializeCompilationResource(
    const std::string& resourceId, const ResourceEntry& resource,
    MaterializedKernelCompilationResource& compilationResource)
{
    ResourceData resourceData;
    const ResourceStatus materializeStatus =
        ResourceRegistry::Instance().Materialize(resourceId, resource, resourceData);
    if (materializeStatus != ResourceStatus::Success) {
        ASCENDLOGE(
            "Failed to materialize compilation resource '%s': status=%d; check the preceding ResourceRegistry logs",
            resourceId.c_str(), static_cast<int32_t>(materializeStatus));
        return ACLRTC_ERROR_FAILURE;
    }
    compilationResource.manifest = std::move(resourceData.json);
    compilationResource.ownedResourceDirectoryPath = std::move(resourceData.resourceDir);
    compilationResource.externalSourceDirectoryPath = std::move(resourceData.sourceFilePath);
    return ACLRTC_SUCCESS;
}

aclError KernelSpecializationSession::ReportSpecializationStatus(
    const NormalizedKernelSpecializationRequest& specializationRequest, KernelSpecializationDiagnostics* diagnostics,
    aclError specializationStatus, const char* stage) noexcept
{
    if (diagnostics != nullptr) {
        diagnostics->WriteSpecializationResult(specializationRequest.outputElfPath, specializationStatus, stage);
    }
    if (specializationStatus == ACLRTC_SUCCESS) {
        ASCENDLOGI(
            "Kernel specialization completed: session=%s resource=%s kernel=%s output=%s",
            specializationRequest.specializationSessionId.c_str(), specializationRequest.resourceId.c_str(),
            specializationRequest.kernelName.c_str(), specializationRequest.outputElfPath.c_str());
    } else {
        ASCENDLOGE(
            "Kernel specialization failed: session=%s resource=%s kernel=%s stage=%s status=%d output=%s",
            specializationRequest.specializationSessionId.c_str(), specializationRequest.resourceId.c_str(),
            specializationRequest.kernelName.c_str(), stage, static_cast<int32_t>(specializationStatus),
            specializationRequest.outputElfPath.c_str());
    }
    return specializationStatus;
}

aclError KernelSpecializationSession::RunKernelSpecialization(
    const aclrtcKernelSpecRequest* apiRequest, const char* outputElfPath) const
{
    if (apiRequest == nullptr) {
        ASCENDLOGE("aclrtcKernelSpecialization request must not be null");
        return ACLRTC_ERROR_INVALID_INPUT;
    }
    if (outputElfPath == nullptr) {
        ASCENDLOGE("aclrtcKernelSpecialization outPath must not be null");
        return ACLRTC_ERROR_INVALID_INPUT;
    }

    NormalizedKernelSpecializationRequest specializationRequest;
    aclError specializationStatus =
        NormalizeKernelSpecializationRequest(*apiRequest, outputElfPath, specializationRequest);
    if (specializationStatus != ACLRTC_SUCCESS) {
        return specializationStatus;
    }

    const bool cacheEnabled = ProcessCompilationContext::Instance().IsCacheEnabled();
    if (cacheEnabled && IsKernelMetaSavingEnabled()) {
        ASCENDLOGW(
            "Kernel specialization cache is enabled: session=%s; kernelmeta saves only cache input JSON, "
            "intermediate compilation artifacts are not retained",
            specializationRequest.specializationSessionId.c_str());
    }
    const ResourceEntry* resource = nullptr;
    PreparedKernelCompilationPlan preparedPlan;
    KernelCompilationCacheInput cacheInput;
    aclError preparationStatus = ACLRTC_SUCCESS;
    specializationStatus =
        PrepareSpecializationInputs(specializationRequest, resource, preparedPlan, cacheInput, preparationStatus);
    if (specializationStatus != ACLRTC_SUCCESS) {
        return ReportSpecializationStatus(specializationRequest, nullptr, specializationStatus, "prepare");
    }
    const KernelSpecializationInputs inputs{specializationRequest, *resource, preparedPlan, cacheInput};
    if (!cacheEnabled) {
        LogCacheBypass(inputs, "disabled");
        return RunUncachedKernelSpecialization(inputs, preparationStatus);
    }
    return GetAndPublishKernelElf(inputs);
}

aclError KernelSpecializationSession::PrepareSpecializationInputs(
    const NormalizedKernelSpecializationRequest& specializationRequest, const ResourceEntry*& resource,
    PreparedKernelCompilationPlan& preparedPlan, KernelCompilationCacheInput& cacheInput, aclError& preparationStatus)
{
    const aclError loadStatus = LoadCompilationResource(specializationRequest.resourceId, resource);
    if (loadStatus != ACLRTC_SUCCESS) {
        return loadStatus;
    }

    CompilationManifest manifest;
    preparationStatus =
        CompilationManifestParser(resource->data.json)
            .ParseSelected(specializationRequest.kernelName, specializationRequest.enableSuperKernel, manifest);
    if (preparationStatus == ACLRTC_SUCCESS) {
        preparationStatus = KernelCompilationPlanBuilder(specializationRequest, {}, resource->data.sourceFilePath)
                                .PrepareCompilationPlan(manifest, preparedPlan);
    }
    if (preparationStatus == ACLRTC_SUCCESS) {
        preparationStatus = CaptureKernelConstants(specializationRequest, preparedPlan.constants, cacheInput.constants);
    }
    if (preparationStatus != ACLRTC_SUCCESS && !ShouldSaveCompilationDiagnostics()) {
        return preparationStatus;
    }

    return ACLRTC_SUCCESS;
}

bool KernelSpecializationSession::BuildPreprocessedCacheInput(
    const KernelSpecializationInputs& inputs, KernelPreprocessingSnapshot& snapshot)
{
    if (MaterializeCompilationResource(inputs.request.resourceId, inputs.resource, snapshot.resource) !=
        ACLRTC_SUCCESS) {
        return false;
    }
    snapshot.workspace = std::make_unique<KernelCompilationWorkspace>(
        std::move(snapshot.resource.ownedResourceDirectoryPath), WorktreeRetentionPolicy::RemoveAfterCompilation);
    KernelCompilationPlan preprocessingPlan;
    aclError status =
        KernelCompilationPlanBuilder(
            inputs.request, snapshot.workspace->GetWorktreePath(), snapshot.resource.externalSourceDirectoryPath)
            .BuildPreprocessingPlan(inputs.preparedPlan, inputs.cacheInput.constants, preprocessingPlan);
    if (status == ACLRTC_SUCCESS) {
        status = snapshot.workspace->CreateOutputDirectoriesAndApplySourcePatches(preprocessingPlan);
    }
    return status == ACLRTC_SUCCESS && AddPreprocessedSourcesToCacheInput(
                                           preprocessingPlan, snapshot.workspace->GetWorktreePath(),
                                           inputs.request.kernelName, inputs.cacheInput, snapshot.sources);
}

aclError KernelSpecializationSession::RunSpecializationWithMaterializedResource(
    const NormalizedKernelSpecializationRequest& specializationRequest,
    MaterializedKernelCompilationResource compilationResource, const PreparedKernelCompilationPlan& preparedPlan,
    const KernelCompilationCacheInput& cacheInput, aclError preparationStatus) const
{
    KernelElfBuildOutput output;
    InitializeCompilationWorkspace(specializationRequest, compilationResource, output);

    if (preparationStatus != ACLRTC_SUCCESS) {
        return ReportSpecializationStatus(
            specializationRequest, output.diagnostics.get(), preparationStatus, "prepare");
    }
    aclError specializationStatus =
        CompileKernelPlan(specializationRequest, compilationResource, preparedPlan, cacheInput, nullptr, output);
    if (specializationStatus != ACLRTC_SUCCESS) {
        return ReportSpecializationStatus(
            specializationRequest, output.diagnostics.get(), specializationStatus, "compile");
    }

    specializationStatus =
        PublishKernelElf(output.plan.linkedKernelElfPath, specializationRequest.outputElfPath, *output.workspace);
    return ReportSpecializationStatus(specializationRequest, output.diagnostics.get(), specializationStatus, "publish");
}

aclError KernelSpecializationSession::RunUncachedKernelSpecialization(
    const KernelSpecializationInputs& inputs, aclError preparationStatus) const
{
    MaterializedKernelCompilationResource materialized;
    const aclError status = MaterializeCompilationResource(inputs.request.resourceId, inputs.resource, materialized);
    if (status != ACLRTC_SUCCESS) {
        return ReportSpecializationStatus(inputs.request, nullptr, status, "materialize");
    }
    if (ProcessCompilationContext::Instance().IsCacheEnabled() && IsKernelMetaSavingEnabled()) {
        KernelSpecializationDiagnostics::WriteCacheInputSnapshot(
            materialized.ownedResourceDirectoryPath, inputs.cacheInput.description);
    }
    return RunSpecializationWithMaterializedResource(
        inputs.request, std::move(materialized), inputs.preparedPlan, inputs.cacheInput, preparationStatus);
}

aclError KernelSpecializationSession::GetAndPublishKernelElf(const KernelSpecializationInputs& inputs) const
{
    auto& context = ProcessCompilationContext::Instance();
    BuildKernelCompilationCacheIdentity(inputs.request, inputs.resource, inputs.preparedPlan, inputs.cacheInput);
    if (inputs.cacheInput.key.empty()) {
        LogCacheBypass(inputs, "identity_unavailable");
        return RunUncachedKernelSpecialization(inputs, ACLRTC_SUCCESS);
    }

    KernelPreprocessingSnapshot snapshot;
    if (!BuildPreprocessedCacheInput(inputs, snapshot)) {
        LogCacheBypass(inputs, "preprocessing_unavailable");
        snapshot.workspace.reset();
        inputs.cacheInput.key.clear();
        inputs.cacheInput.description["key"] = "";
        return RunUncachedKernelSpecialization(inputs, ACLRTC_SUCCESS);
    }

    if (IsKernelMetaSavingEnabled()) {
        KernelSpecializationDiagnostics::WriteCacheInputSnapshot(
            snapshot.workspace->GetWorktreePath(), inputs.cacheInput.description);
    }
    KernelElfBuildOutput output;
    auto& memoryCache = context.GetCompilationMemoryCache();
    output.kernelElf = memoryCache.GetKernelElf(inputs.cacheInput.key);
    aclError status = ACLRTC_SUCCESS;
    if (output.kernelElf) {
        LogCacheHit(inputs, "memory");
    } else {
        status = GetOrBuildKernelElf(inputs, snapshot, output);
        if (status != ACLRTC_SUCCESS) {
            return ReportSpecializationStatus(inputs.request, output.diagnostics.get(), status, "build");
        }
        memoryCache.CacheKernelElf(inputs.cacheInput.key, output.kernelElf);
    }
    if (!output.kernelElf) {
        status = PublishKernelElf(output.plan.linkedKernelElfPath, inputs.request.outputElfPath, *output.workspace);
    } else {
        status = FileUtils::WriteBinaryFileAtomically(inputs.request.outputElfPath.string(), *output.kernelElf) ?
                     ACLRTC_SUCCESS :
                     ACLRTC_ERROR_FAILURE;
    }
    return ReportSpecializationStatus(inputs.request, output.diagnostics.get(), status, "publish");
}

aclError KernelSpecializationSession::GetOrBuildKernelElf(
    const KernelSpecializationInputs& inputs, KernelPreprocessingSnapshot& snapshot, KernelElfBuildOutput& output) const
{
    auto& context = ProcessCompilationContext::Instance();
    const auto& diskConfig = context.GetDiskCacheConfig();
    const KernelCompilationDiskCache diskCache(diskConfig.directory);
    ASCENDLOGD(
        "Kernel specialization disk cache lookup: session=%s resource=%s kernel=%s key=%s",
        inputs.request.specializationSessionId.c_str(), inputs.request.resourceId.c_str(),
        inputs.request.kernelName.c_str(), inputs.cacheInput.key.c_str());
    output.kernelElf = diskCache.ReadKernelElf(inputs.cacheInput.key, inputs.cacheInput.baseKey);
    if (output.kernelElf) {
        LogCacheHit(inputs, "disk");
        return ACLRTC_SUCCESS;
    }
    // Compile the same preprocessed files that were hashed for the cache key.
    output.workspace = std::move(snapshot.workspace);
    InitializeCompilationDiagnostics(inputs.request, snapshot.resource, output);
    aclError status = CompileKernelPlan(
        inputs.request, snapshot.resource, inputs.preparedPlan, inputs.cacheInput, &snapshot.sources, output);
    if (status != ACLRTC_SUCCESS) {
        return status;
    }
    output.kernelElf = ReadKernelElf(output.plan.linkedKernelElfPath);
    if (!output.kernelElf) {
        return ACLRTC_SUCCESS;
    }
    // Preserve the requirement to finish workspace cleanup before publishing the ELF.
    status = output.workspace->CleanupWorktreeBeforeElfPublication();
    if (status == ACLRTC_SUCCESS) {
        // Cache publication and cleanup must not change the successful compilation result.
        (void)diskCache.PublishKernelElf(inputs.cacheInput, *output.kernelElf);
        diskCache.CleanCacheIfOverLimit(diskConfig.maximumSizeBytes, diskConfig.cleanupRatio);
    }
    return status;
}

aclError KernelSpecializationSession::CompileKernelPlan(
    const NormalizedKernelSpecializationRequest& request, const MaterializedKernelCompilationResource& resource,
    const PreparedKernelCompilationPlan& preparedPlan, const KernelCompilationCacheInput& cacheInput,
    const std::vector<PreprocessedCompilationSource>* preprocessedSources, KernelElfBuildOutput& output)
{
    aclError status =
        KernelCompilationPlanBuilder(request, output.workspace->GetWorktreePath(), resource.externalSourceDirectoryPath)
            .BuildCompilationPlan(preparedPlan, cacheInput.constants, output.plan, preprocessedSources);
    if (status != ACLRTC_SUCCESS) {
        return status;
    }
    status = output.workspace->CreateOutputDirectoriesAndApplySourcePatches(output.plan);
    if (status != ACLRTC_SUCCESS) {
        return status;
    }
    CompilationPlanExecutor executor(*output.diagnostics, CompilationCommandExecutionLimits::ProductionDefaults());
    return executor.ExecuteCompilationPlan(output.plan);
}

aclError KernelSpecializationSession::PublishKernelElf(
    const fs::path& linkedKernelElfPath, const fs::path& outputElfPath,
    KernelCompilationWorkspace& compilationWorkspace) const
{
    const bool published =
        FileUtils::CopyFileAtomically(linkedKernelElfPath.string(), outputElfPath.string(), [&compilationWorkspace]() {
            return compilationWorkspace.CleanupWorktreeBeforeElfPublication() == ACLRTC_SUCCESS;
        });
    if (!published) {
        return ACLRTC_ERROR_FAILURE;
    }
    return ACLRTC_SUCCESS;
}

} // namespace aclrtc
} // namespace ascendc

extern "C" aclError aclrtcKernelSpecialization(const aclrtcKernelSpecRequest* request, const char* outPath)
{
    try {
        ascendc::aclrtc::KernelSpecializationSession specializationSession;
        return specializationSession.RunKernelSpecialization(request, outPath);
    } catch (const std::bad_alloc&) {
        ASCENDLOGE("Out of memory while specializing kernel");
        return ascendc::aclrtc::ACLRTC_ERROR_OUT_OF_MEMORY;
    } catch (const std::exception& exception) {
        ASCENDLOGE("Kernel specialization failed with exception: %s", exception.what());
        return ascendc::aclrtc::ACLRTC_ERROR_FAILURE;
    } catch (...) {
        ASCENDLOGE("Kernel specialization failed with an unknown exception");
        return ascendc::aclrtc::ACLRTC_ERROR_FAILURE;
    }
}
