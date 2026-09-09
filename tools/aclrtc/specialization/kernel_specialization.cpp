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
#include "compilation_manifest_parser.h"
#include "kernel_compilation_workspace.h"
#include "kernel_specialization_diagnostics.h"
#include "ascendc_tool_log.h"
#include "resource_registry.h"

#include <boost/filesystem.hpp>
#include <memory>

namespace ascendc {
namespace aclrtc {
namespace {
namespace fs = boost::filesystem;
using specialization_compile::IsKernelMetaSavingEnabled;
using specialization_compile::ResourceData;
using specialization_compile::ResourceRegistry;
using specialization_compile::ResourceStatus;

} // namespace

aclError KernelSpecializationSession::LoadAndMaterializeCompilationResource(
    const std::string& resourceId, MaterializedKernelCompilationResource& compilationResource)
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

    ResourceData resourceData;
    const ResourceStatus lookupStatus = resourceRegistry.Lookup(resourceId.c_str(), resourceData);
    if (lookupStatus != ResourceStatus::Success) {
        ASCENDLOGE(
            "Failed to find or materialize compilation resource '%s': status=%d; verify resourceId and check the "
            "preceding ResourceRegistry logs",
            resourceId.c_str(), static_cast<int32_t>(lookupStatus));
        return ACLRTC_ERROR_FAILURE;
    }

    compilationResource.manifest = std::move(resourceData.json);
    compilationResource.ownedResourceDirectoryPath = std::move(resourceData.resourceDir);
    compilationResource.externalSourceDirectoryPath = std::move(resourceData.sourceFilePath);
    return ACLRTC_SUCCESS;
}

aclError KernelSpecializationSession::ReportSpecializationStatus(
    const NormalizedKernelSpecializationRequest& specializationRequest, KernelSpecializationDiagnostics& diagnostics,
    aclError specializationStatus) noexcept
{
    const KernelElfPublicationStatus publicationStatus = specializationStatus == ACLRTC_SUCCESS ?
                                                             KernelElfPublicationStatus::Published :
                                                             KernelElfPublicationStatus::NotPublished;
    diagnostics.WriteSpecializationResult(specializationRequest.outputElfPath, publicationStatus);
    if (specializationStatus == ACLRTC_SUCCESS) {
        ASCENDLOGI(
            "Kernel %s specialization completed: output=%s", specializationRequest.kernelName.c_str(),
            specializationRequest.outputElfPath.c_str());
    }
    return specializationStatus;
}

aclError KernelSpecializationSession::RunKernelSpecialization(
    const aclrtcKernelSpecRequest* apiRequest, const char* outputElfPath)
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

    MaterializedKernelCompilationResource compilationResource;
    specializationStatus = LoadAndMaterializeCompilationResource(specializationRequest.resourceId, compilationResource);
    if (specializationStatus != ACLRTC_SUCCESS) {
        return specializationStatus;
    }
    return RunSpecializationWithMaterializedResource(specializationRequest, std::move(compilationResource));
}

aclError KernelSpecializationSession::RunSpecializationWithMaterializedResource(
    const NormalizedKernelSpecializationRequest& specializationRequest,
    MaterializedKernelCompilationResource compilationResource) const
{
    const bool saveKernelMetaEnabled = IsKernelMetaSavingEnabled();
    KernelCompilationWorkspace compilationWorkspace(
        std::move(compilationResource.ownedResourceDirectoryPath), saveKernelMetaEnabled ?
                                                                       WorktreeRetentionPolicy::RetainAfterCompilation :
                                                                       WorktreeRetentionPolicy::RemoveAfterCompilation);
    KernelSpecializationDiagnostics diagnostics(
        saveKernelMetaEnabled ? compilationWorkspace.GetWorktreePath() : fs::path(),
        specializationRequest.specializationSessionId);
    diagnostics.WriteManifestSnapshot(compilationResource.manifest);

    CompilationManifest manifest;
    aclError specializationStatus =
        CompilationManifestParser(compilationResource.manifest)
            .ParseSelected(specializationRequest.kernelName, specializationRequest.enableSuperKernel, manifest);
    if (specializationStatus != ACLRTC_SUCCESS) {
        return ReportSpecializationStatus(specializationRequest, diagnostics, specializationStatus);
    }
    KernelCompilationPlan compilationPlan;
    specializationStatus = KernelCompilationPlanBuilder(
                               specializationRequest, manifest, compilationWorkspace.GetWorktreePath(),
                               compilationResource.externalSourceDirectoryPath)
                               .BuildCompilationPlan(compilationPlan);
    if (specializationStatus != ACLRTC_SUCCESS) {
        return ReportSpecializationStatus(specializationRequest, diagnostics, specializationStatus);
    }
    specializationStatus = compilationWorkspace.CreateOutputDirectoriesAndApplySourcePatches(compilationPlan);
    if (specializationStatus != ACLRTC_SUCCESS) {
        return ReportSpecializationStatus(specializationRequest, diagnostics, specializationStatus);
    }

    CompilationPlanExecutor planExecutor(diagnostics, CompilationCommandExecutionLimits::ProductionDefaults());
    specializationStatus = planExecutor.ExecuteCompilationPlan(compilationPlan);
    if (specializationStatus != ACLRTC_SUCCESS) {
        return ReportSpecializationStatus(specializationRequest, diagnostics, specializationStatus);
    }

    specializationStatus = PublishKernelElf(
        compilationPlan.linkedKernelElfPath, specializationRequest.outputElfPath, compilationWorkspace);
    return ReportSpecializationStatus(specializationRequest, diagnostics, specializationStatus);
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
