/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef ASCENDC_ACLRTC_KERNEL_SPECIALIZATION_TYPES_H
#define ASCENDC_ACLRTC_KERNEL_SPECIALIZATION_TYPES_H

#include "acl/acl_rt_compile.h"

#include <boost/filesystem/path.hpp>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace ascendc {
namespace aclrtc {

constexpr aclError ACLRTC_SUCCESS = 0;
constexpr aclError ACLRTC_ERROR_INVALID_INPUT = 176001;
constexpr aclError ACLRTC_ERROR_INVALID_OPTION = 176002;
constexpr aclError ACLRTC_ERROR_COMPILATION = 176003;
constexpr aclError ACLRTC_ERROR_LINKING = 176004;
constexpr aclError ACLRTC_ERROR_OUT_OF_MEMORY = 276001;
constexpr aclError ACLRTC_ERROR_FAILURE = 576000;

enum class KernelCompilationVariant : uint32_t {
    Basic,
    BasicWithSuperKernel,
};

enum class CompilationCommandKind : uint32_t {
    Compile,
    ObjectCopy,
    Link,
};

struct KernelCompilerOptions {
    std::vector<std::string> basicOptions;
    std::vector<std::string> superKernelOptions;
};

struct NormalizedKernelSpecializationRequest {
    std::string specializationSessionId;
    std::string resourceId;
    std::string kernelName;
    boost::filesystem::path outputElfPath;
    KernelCompilationVariant compilationVariant{KernelCompilationVariant::Basic};
    KernelCompilerOptions compilerOptions;
    uint64_t kernelArgumentCount{0};
    const void* const* borrowedKernelArgumentDataPointers{nullptr};
    const uint64_t* borrowedKernelArgumentByteCounts{nullptr};
};

struct KernelSourcePatch {
    boost::filesystem::path targetFilePath;
    std::string templateText;
    std::string replacementText;
};

struct CompilationCommand {
    CompilationCommandKind commandKind;
    std::optional<uint32_t> parallelStage;
    std::string diagnosticLabel;
    boost::filesystem::path executablePath;
    std::vector<std::string> commandArguments;
};

struct KernelCompilationPlan {
    std::vector<KernelSourcePatch> sourcePatches;
    std::vector<CompilationCommand> compilationCommands;
    std::vector<boost::filesystem::path> requiredOutputDirectoryPaths;
    boost::filesystem::path linkedKernelElfPath;
};

aclError NormalizeKernelSpecializationRequest(
    const aclrtcKernelSpecRequest& apiRequest, const char* outputElfPath,
    NormalizedKernelSpecializationRequest& normalizedRequest);

} // namespace aclrtc
} // namespace ascendc

#endif // ASCENDC_ACLRTC_KERNEL_SPECIALIZATION_TYPES_H
