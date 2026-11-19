/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef ASCENDC_ACLRTC_KERNEL_COMPILATION_CACHE_INPUT_H
#define ASCENDC_ACLRTC_KERNEL_COMPILATION_CACHE_INPUT_H

#include "kernel_compilation_plan_builder.h"

#include <nlohmann/json.hpp>

namespace ascendc {
namespace specialization_compile {
struct ResourceEntry;
}
namespace aclrtc {

struct KernelConstantBindingValue {
    uint64_t parameterIndex;
    uint64_t byteCount;
};

struct KernelConstantSnapshot {
    std::vector<KernelConstantBindingValue> bindings;
    std::map<uint64_t, std::vector<uint8_t>> argumentBytes;
};

struct KernelCompilationCacheInput {
    KernelConstantSnapshot constants;
    nlohmann::json description;
    std::string baseKey;
    std::string key;
};

aclError CaptureKernelConstants(
    const NormalizedKernelSpecializationRequest& request, const std::vector<ManifestConstant>& constants,
    KernelConstantSnapshot& snapshot);

void BuildKernelCompilationCacheIdentity(
    const NormalizedKernelSpecializationRequest& request, const specialization_compile::ResourceEntry& resource,
    const PreparedKernelCompilationPlan& plan, KernelCompilationCacheInput& cacheInput);

} // namespace aclrtc
} // namespace ascendc

#endif // ASCENDC_ACLRTC_KERNEL_COMPILATION_CACHE_INPUT_H
