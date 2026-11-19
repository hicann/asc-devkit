/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef ASCENDC_ACLRTC_PROCESS_COMPILATION_CONTEXT_H
#define ASCENDC_ACLRTC_PROCESS_COMPILATION_CONTEXT_H

#include "kernel_specialization_types.h"
#include "kernel_compilation_memory_cache.h"
#include "directory_cleanup_guard.h"

#include <mutex>
#include <map>

namespace ascendc {
namespace aclrtc {

struct KernelCompilationDiskCacheConfig {
    boost::filesystem::path directory;
    uint64_t maximumSizeBytes{500ULL * 1024ULL * 1024ULL};
    // Percentage of capacity to remove when the cache exceeds its limit.
    uint32_t cleanupRatio{50U};
};

class ProcessCompilationContext final {
public:
    static ProcessCompilationContext& Instance();

    bool GetEnvironmentVariable(const std::string& name, const char* location, std::string& value);
    std::optional<std::string> GetEnvironmentVariable(const std::string& name);
    std::map<std::string, std::optional<std::string>> GetCompilerEnvironment();
    bool IsCacheEnabled();
    const KernelCompilationDiskCacheConfig& GetDiskCacheConfig();
    KernelCompilationMemoryCache& GetCompilationMemoryCache();
    const std::optional<std::map<std::string, std::string>>& GetToolchainTimestamps(
        const boost::filesystem::path& ascendHome);

    aclError GetSpecializationFlagObject(
        const boost::filesystem::path& ascendHome, const std::vector<std::string>& targetOptions,
        boost::filesystem::path& objectPath);

private:
    ProcessCompilationContext() = default;

    std::mutex environmentMutex_;
    // The first observation, including an unset variable, is fixed for this process.
    std::map<std::string, std::optional<std::string>> environmentValues_;

    std::mutex cacheConfigMutex_;
    std::optional<bool> cacheEnabled_;
    std::optional<KernelCompilationDiskCacheConfig> diskCacheConfig_;
    KernelCompilationMemoryCache compilationMemoryCache_;

    std::mutex toolchainMutex_;
    bool toolchainTimestampsRead_{false};
    std::optional<std::map<std::string, std::string>> toolchainTimestamps_;

    std::mutex specializationFlagMutex_;
    std::vector<std::string> specializationFlagConfiguration_;
    DirectoryCleanupGuard specializationFlagDirectory_;
};

} // namespace aclrtc
} // namespace ascendc

#endif // ASCENDC_ACLRTC_PROCESS_COMPILATION_CONTEXT_H
