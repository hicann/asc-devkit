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

#include <nlohmann/json.hpp>

namespace ascendc {
namespace aclrtc {

class KernelCompilationWorkspace;
class KernelSpecializationDiagnostics;

struct MaterializedKernelCompilationResource {
    nlohmann::json manifest;
    boost::filesystem::path ownedResourceDirectoryPath;
    boost::filesystem::path externalSourceDirectoryPath;
};

class KernelSpecializationSession final {
public:
    aclError RunKernelSpecialization(const aclrtcKernelSpecRequest* apiRequest, const char* outputElfPath);

private:
    static aclError PrepareCompilationResource(
        const std::string& resourceId, MaterializedKernelCompilationResource& compilationResource);

    aclError RunSpecializationWithMaterializedResource(
        const NormalizedKernelSpecializationRequest& specializationRequest,
        MaterializedKernelCompilationResource compilationResource);

    static aclError FinalizeSpecializationResult(
        const NormalizedKernelSpecializationRequest& specializationRequest,
        KernelSpecializationDiagnostics& diagnostics, aclError specializationResult) noexcept;

    aclError PublishKernelElf(
        const boost::filesystem::path& linkedKernelElfPath, const boost::filesystem::path& outputElfPath,
        KernelCompilationWorkspace& compilationWorkspace) const;
};

} // namespace aclrtc
} // namespace ascendc

#endif // ASCENDC_ACLRTC_KERNEL_SPECIALIZATION_H
