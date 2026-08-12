/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef MC2_AICPU_TASK_CACHE_H
#define MC2_AICPU_TASK_CACHE_H

#include <array>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "alg_param.h"

namespace mc2_ops_hccl {

constexpr size_t MC2_TASK_CACHE_MAX_TRACKED_TAG_COUNT = 65536U;
constexpr size_t MC2_TASK_CACHE_MAX_TRACKED_TAG_BYTES = 8U * 1024U * 1024U;

enum class Mc2TaskCacheReserveResult {
    EXISTING,
    RESERVED,
    FULL,
};

struct Mc2AicpuTaskCachePlan {
    bool enabled = false;
    std::string tag;
    void* comm = nullptr;
    std::array<void*, 2> addrs{{nullptr, nullptr}};
    std::array<uint64_t, 2> sizes{{0U, 0U}};
};

HcclResult BuildMc2AicpuTaskCachePlan(
    const OpParam& param, const AlgResourceCtxSerializable& resCtx, Mc2AicpuTaskCachePlan& plan);

HcclResult Mc2AicpuTaskCacheLookup(const Mc2AicpuTaskCachePlan& plan, bool& isHit);
HcclResult Mc2AicpuTaskCacheStart(const Mc2AicpuTaskCachePlan& plan);
HcclResult Mc2AicpuTaskCacheEnd(const Mc2AicpuTaskCachePlan& plan);
HcclResult Mc2AicpuTaskCacheExecute(const Mc2AicpuTaskCachePlan& plan);
HcclResult Mc2AicpuTaskCacheClear(const Mc2AicpuTaskCachePlan& plan);

class Mc2AicpuTaskCacheManager {
public:
    static Mc2AicpuTaskCacheManager& Instance();

    explicit Mc2AicpuTaskCacheManager(
        size_t maxTagCount = MC2_TASK_CACHE_MAX_TRACKED_TAG_COUNT,
        size_t maxTagBytes = MC2_TASK_CACHE_MAX_TRACKED_TAG_BYTES);
    ~Mc2AicpuTaskCacheManager() = default;

    Mc2TaskCacheReserveResult TryReserve(void* comm, const std::string& tag);
    void Unregister(void* comm, const std::string& tag);
    HcclResult Evict(void* comm);

private:
    Mc2AicpuTaskCacheManager(const Mc2AicpuTaskCacheManager&) = delete;
    Mc2AicpuTaskCacheManager& operator=(const Mc2AicpuTaskCacheManager&) = delete;

    std::mutex mutex_;
    std::unordered_map<void*, std::unordered_set<std::string>> commTags_;
    size_t trackedTagCount_ = 0U;
    size_t trackedTagBytes_ = 0U;
    const size_t maxTagCount_;
    const size_t maxTagBytes_;
    bool capacityWarningReported_ = false;
};

} // namespace mc2_ops_hccl

#endif // MC2_AICPU_TASK_CACHE_H
