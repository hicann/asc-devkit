/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef ASCENDC_ACLRTC_KERNEL_COMPILATION_MEMORY_CACHE_H
#define ASCENDC_ACLRTC_KERNEL_COMPILATION_MEMORY_CACHE_H

#include "kernel_specialization_types.h"

#include <memory>
#include <mutex>

namespace ascendc {
namespace aclrtc {

class KernelCompilationMemoryCache final {
public:
    using KernelElf = std::shared_ptr<const std::vector<uint8_t>>;
    static constexpr uint64_t DEFAULT_MEMORY_BUDGET = 64U * BYTES_PER_MEBIBYTE;

    explicit KernelCompilationMemoryCache(uint64_t memoryBudget = DEFAULT_MEMORY_BUDGET);
    KernelElf GetKernelElf(const std::string& key);
    void CacheKernelElf(const std::string& key, const KernelElf& kernelElf);

private:
    std::mutex mutex_;
    // The budget covers retained entries; callers may keep artifacts alive after eviction.
    const uint64_t memoryBudget_;
    uint64_t retainedBytes_{0U};
    std::map<std::string, KernelElf> cachedElfs_;
};

} // namespace aclrtc
} // namespace ascendc

#endif // ASCENDC_ACLRTC_KERNEL_COMPILATION_MEMORY_CACHE_H
