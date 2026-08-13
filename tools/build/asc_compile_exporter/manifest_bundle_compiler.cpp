/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "manifest_bundle_compiler.h"

#include <utility>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

#include "ascendc_tool_log.h"
#include "build_workspace.h"
#include "bundle_source_generator.h"
#include "collected_manifest_repository.h"
#include "file_utils.h"
#include "manifest_bundle_types.h"
#include "process_runner.h"

namespace ascendc {
namespace manifest_generator {

ManifestBundleCompiler::ManifestBundleCompiler(BuildCollectedBundleRequest request) noexcept
    : request_(std::move(request))
{}

bool ManifestBundleCompiler::BuildBundle(const std::string& workDir, const std::string& output) const
{
    ASCENDLOGD(
        "Building bundle with make=%s compiler=%s jobs=%u", request_.makeExecutable.c_str(),
        request_.cxxCompiler.c_str(), request_.jobs);
    if (!ProcessRunner::Run(
            {request_.makeExecutable, "-C", workDir, "-f", "Makefile", "-B", "-j" + std::to_string(request_.jobs)},
            {{"ASCENDC_BUNDLE_CXX", request_.cxxCompiler}, {"ASCENDC_BUNDLE_OUTPUT", output}})) {
        return false;
    }
    ASCENDLOGI("Built staged compile resource bundle %s", output.c_str());
    return true;
}

bool ManifestBundleCompiler::PublishBundle(const std::string& staged) const
{
    // Prefer rename because replacing the output is atomic when both paths are on the same filesystem.
    boost::system::error_code error;
    boost::filesystem::rename(boost::filesystem::path(staged), boost::filesystem::path(request_.outputPath), error);
    if (!error) {
        ASCENDLOGI("Published compile resource bundle %s", request_.outputPath.c_str());
        return true;
    }
    if (error != boost::system::errc::cross_device_link) {
        ASCENDLOGE("Failed to publish bundle output: %s", error.message().c_str());
        return false;
    }

    // rename cannot cross filesystems. Copy into the output filesystem before renaming again.
    if (!PublishBundleAcrossFileSystems(staged)) {
        return false;
    }
    ASCENDLOGI("Published compile resource bundle %s", request_.outputPath.c_str());
    return true;
}

bool ManifestBundleCompiler::PublishBundleAcrossFileSystems(const std::string& staged) const
{
    ASCENDLOGD("Copying staged bundle across filesystems: %s -> %s", staged.c_str(), request_.outputPath.c_str());
    // A private workspace under the output parent places the copied file on the output filesystem.
    BuildWorkspace publishWorkspace;
    if (!publishWorkspace.Create(FileUtils::ParentPath(request_.outputPath), false)) {
        return false;
    }
    const std::string copied = FileUtils::JoinPath(publishWorkspace.Session(), FileUtils::FileName(staged));
    if (!FileUtils::CopyFile(staged, copied)) {
        ASCENDLOGE("Failed to copy staged bundle across filesystems: %s", staged.c_str());
        return false;
    }

    boost::system::error_code error;
    boost::filesystem::rename(boost::filesystem::path(copied), boost::filesystem::path(request_.outputPath), error);
    if (error) {
        ASCENDLOGE("Failed to publish copied bundle output: %s", error.message().c_str());
        return false;
    }
    return true;
}

bool ManifestBundleCompiler::Compile() const
{
    ASCENDLOGI(
        "Starting bundle generation: manifest_root=%s output=%s keep_temp=%s", request_.manifestSearchRoot.c_str(),
        request_.outputPath.c_str(), request_.keepTemp ? "true" : "false");

    ASCENDLOGI("Using %u parallel bundle build job(s)", request_.jobs);

    // Load and validate the complete collection before allocating a private build workspace.
    std::vector<ManifestUnit> units;
    if (!CollectedManifestRepository(request_.manifestSearchRoot).Load(units)) {
        return false;
    }
    BuildWorkspace workspace;
    if (!workspace.Create(std::string(), request_.keepTemp)) {
        return false;
    }
    if (request_.keepTemp) {
        ASCENDLOGI("Keeping temporary bundle directory %s", workspace.Session().c_str());
    }

    // Generate isolated translation units and compile them into a staged output.
    if (!BundleSourceGenerator(workspace.Session(), units, request_.outputKind).Generate()) {
        return false;
    }
    const std::string stagedName = request_.outputKind == BundleOutputKind::kSharedObject ?
                                       "compile_resource_bundle.so" :
                                       "compile_resource_bundle_relocatable.o";
    const std::string stagedOutput = FileUtils::JoinPath(workspace.Session(), stagedName);
    if (!BuildBundle(workspace.Session(), stagedOutput)) {
        return false;
    }

    // Publish only after a successful build so failures never replace an existing output bundle.
    if (!PublishBundle(stagedOutput)) {
        return false;
    }
    ASCENDLOGI("Bundle generation completed successfully: %s", request_.outputPath.c_str());
    return true;
}

} // namespace manifest_generator
} // namespace ascendc
