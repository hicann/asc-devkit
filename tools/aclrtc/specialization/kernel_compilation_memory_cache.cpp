/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "kernel_compilation_memory_cache.h"

#include "ascendc_tool_log.h"

namespace ascendc {
namespace aclrtc {

KernelCompilationMemoryCache::KernelCompilationMemoryCache(uint64_t memoryBudget) : memoryBudget_(memoryBudget) {}

KernelCompilationMemoryCache::KernelElf KernelCompilationMemoryCache::GetKernelElf(const std::string& key)
{
    const std::lock_guard<std::mutex> lock(mutex_);
    const auto cached = cachedElfs_.find(key);
    if (cached == cachedElfs_.end()) {
        return {};
    }
    return cached->second;
}

void KernelCompilationMemoryCache::CacheKernelElf(const std::string& key, const KernelElf& kernelElf)
{
    if (!kernelElf) {
        return;
    }
    const std::lock_guard<std::mutex> lock(mutex_);
    const uint64_t size = kernelElf->size();
    if (size > memoryBudget_ || cachedElfs_.find(key) != cachedElfs_.end()) {
        return;
    }
    // The budget limits cache-owned references; callers may retain evicted ELF buffers.
    if (retainedBytes_ > memoryBudget_ - size) {
        cachedElfs_.clear();
        retainedBytes_ = 0U;
    }
    try {
        cachedElfs_.emplace(key, kernelElf);
        retainedBytes_ += size;
    } catch (const std::bad_alloc&) {
        ASCENDLOGW("Failed to retain kernel compilation artifact: key=%s; keep the prepared result", key.c_str());
    }
}

} // namespace aclrtc
} // namespace ascendc
