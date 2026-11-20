/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef ASCENDC_ACLRTC_KERNEL_COMPILATION_PLAN_BUILDER_H
#define ASCENDC_ACLRTC_KERNEL_COMPILATION_PLAN_BUILDER_H

#include "compilation_manifest.h"

#include <boost/filesystem/path.hpp>
#include <map>

namespace ascendc {
namespace aclrtc {

struct KernelConstantSnapshot;

struct PreprocessedCompilationSource {
    boost::filesystem::path sourcePath;
    boost::filesystem::path preprocessedPath;
};

struct PreparedCompilationCommand {
    CompilationCommandKind commandKind;
    std::optional<uint32_t> parallelStage;
    std::string diagnosticLabel;
    std::string executable;
    std::vector<std::string> arguments;
    std::vector<std::string> compilerOptions;
    std::optional<KernelSymbolRename> kernelSymbolRename;
};

struct PreparedKernelCompilationPlan {
    std::vector<ManifestConstant> constants;
    std::vector<PreparedCompilationCommand> compilationCommands;
    std::vector<std::string> linkInputs;
    size_t basicCompileCommandIndex{0U};
    std::string sourceDirectory;
    std::map<std::string, std::string> environmentValues;
    std::map<std::string, std::optional<std::string>> compilerEnvironment;
};

class KernelCompilationPlanBuilder final {
public:
    KernelCompilationPlanBuilder(
        const NormalizedKernelSpecializationRequest& specializationRequest,
        boost::filesystem::path resourceWorktreePath, boost::filesystem::path externalSourceDirectoryPath);

    aclError PrepareCompilationPlan(
        const CompilationManifest& manifest, PreparedKernelCompilationPlan& compilationPlan) const;
    aclError BuildPreprocessingPlan(
        const PreparedKernelCompilationPlan& preparedPlan, const KernelConstantSnapshot& constants,
        KernelCompilationPlan& compilationPlan) const;
    aclError BuildCompilationPlan(
        const PreparedKernelCompilationPlan& preparedPlan, const KernelConstantSnapshot& constants,
        KernelCompilationPlan& compilationPlan,
        const std::vector<PreprocessedCompilationSource>* preprocessedSources = nullptr) const;

private:
    aclError CheckResourceWorktreeDirectory() const;
    std::string ExpandPathAndEnvironmentReferences(
        const std::string& text, const PreparedKernelCompilationPlan& plan) const;
    void BuildConstantSourcePatches(
        const PreparedKernelCompilationPlan& preparedPlan, const KernelConstantSnapshot& constants,
        KernelCompilationPlan& plan) const;
    void AppendLinkCommand(const CompilationManifest& manifest, PreparedKernelCompilationPlan& plan) const;
    void AppendKernelRenameCommand(PreparedKernelCompilationPlan& plan) const;
    aclError BuildSpecializationMetadata(
        const PreparedKernelCompilationPlan& preparedPlan, const CompilationCommand& basicCommand,
        boost::filesystem::path& objectPath) const;
    aclError BindCompilationCommand(
        const PreparedCompilationCommand& preparedCommand, const PreparedKernelCompilationPlan& preparedPlan,
        CompilationCommand& command) const;
    aclError BindCompilationCommands(
        const PreparedKernelCompilationPlan& preparedPlan,
        const std::vector<PreprocessedCompilationSource>* preprocessedSources, KernelCompilationPlan& plan) const;

    const NormalizedKernelSpecializationRequest& specializationRequest_;
    boost::filesystem::path resourceWorktreePath_;
    boost::filesystem::path externalSourceDirectoryPath_;
    boost::filesystem::path outputDirectoryPath_;
};

} // namespace aclrtc
} // namespace ascendc

#endif // ASCENDC_ACLRTC_KERNEL_COMPILATION_PLAN_BUILDER_H
