/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef ASCENDC_ACLRTC_KERNEL_COMPILATION_DISK_CACHE_H
#define ASCENDC_ACLRTC_KERNEL_COMPILATION_DISK_CACHE_H

#include "kernel_compilation_memory_cache.h"

namespace ascendc {
namespace aclrtc {

struct KernelCompilationCacheInput;

class KernelCompilationDiskCache final {
public:
    explicit KernelCompilationDiskCache(boost::filesystem::path directory);
    KernelCompilationMemoryCache::KernelElf ReadKernelElf(const std::string& key, const std::string& baseKey) const;
    bool PublishKernelElf(const KernelCompilationCacheInput& cacheInput, const std::vector<uint8_t>& kernelElf) const;
    void CleanCacheIfOverLimit(uint64_t maximumSizeBytes, uint32_t cleanupRatio) const;

private:
    bool PublishDirectory(
        const boost::filesystem::path& stagedDirectory, const boost::filesystem::path& entryDirectory,
        const KernelCompilationCacheInput& cacheInput) const;

    boost::filesystem::path directory_;
};

} // namespace aclrtc
} // namespace ascendc

#endif // ASCENDC_ACLRTC_KERNEL_COMPILATION_DISK_CACHE_H
