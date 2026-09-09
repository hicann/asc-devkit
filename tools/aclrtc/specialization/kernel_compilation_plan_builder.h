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

namespace ascendc {
namespace aclrtc {

class KernelCompilationPlanBuilder final {
public:
    KernelCompilationPlanBuilder(
        const NormalizedKernelSpecializationRequest& specializationRequest, const CompilationManifest& manifest,
        boost::filesystem::path resourceWorktreePath, boost::filesystem::path externalSourceDirectoryPath);

    aclError BuildCompilationPlan(KernelCompilationPlan& compilationPlan) const;

private:
    aclError CheckResourceWorktreeDirectory() const;
    bool ExpandPathAndEnvironmentReferences(
        const std::string& text, const std::string& location, std::string& resolvedText) const;
    aclError BuildConstantSourcePatches(KernelCompilationPlan& plan) const;
    aclError BindManifestCommand(const ManifestCommand& spec, CompilationCommand& compilationCommand) const;
    aclError BindLinkCommand(const CompilationManifest& manifest, KernelCompilationPlan& plan) const;

    const NormalizedKernelSpecializationRequest& specializationRequest_;
    const CompilationManifest& borrowedManifest_;
    boost::filesystem::path resourceWorktreePath_;
    boost::filesystem::path externalSourceDirectoryPath_;
    boost::filesystem::path outputDirectoryPath_;
};

} // namespace aclrtc
} // namespace ascendc

#endif // ASCENDC_ACLRTC_KERNEL_COMPILATION_PLAN_BUILDER_H
