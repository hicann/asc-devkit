/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef SIM_CONTEXT_MANAGER_H
#define SIM_CONTEXT_MANAGER_H

#include <functional>
#include <unordered_map>
#include "hccl/hccl_types.h"
#include "dtype_common.h"
#include "hccl_res.h"
#include "hccl_sim_pub.h"
#include "hccl_common.h"

namespace HcclSim {

struct HcclMemHash {
    static constexpr size_t MEM_ADDR_SHIFT = 1;
    static constexpr size_t MEM_SIZE_SHIFT = 2;
    size_t operator()(const HcclMem& mem) const noexcept
    {
        size_t hashMemType = std::hash<uint32_t>{}(static_cast<int>(mem.type));
        size_t hashMemaddr = std::hash<void*>{}(mem.addr);
        size_t hashMemSize = std::hash<uint64_t>{}(mem.size);
        return hashMemType ^ (hashMemaddr << MEM_ADDR_SHIFT) ^ (hashMemSize << MEM_SIZE_SHIFT);
    }
};

struct HcclMemEqual {
    bool operator()(const HcclMem& lhm, const HcclMem& rhm) const
    {
        return lhm.type == rhm.type && lhm.addr == rhm.addr && lhm.size == rhm.size;
    }
};

enum class HcclRtMemcpyKind {
    HCCL_RT_MEMCPY_KIND_HOST_TO_HOST = 0, /**< host to host */
    HCCL_RT_MEMCPY_KIND_HOST_TO_DEVICE,   /**< host to device */
    HCCL_RT_MEMCPY_KIND_DEVICE_TO_HOST,   /**< device to host */
    HCCL_RT_MEMCPY_KIND_DEVICE_TO_DEVICE, /**< device to device */
    HCCL_RT_MEMCPY_ADDR_DEVICE_TO_DEVICE, /**< Level-2 address copy, device to device */
    HCCL_RT_MEMCPY_KIND_RESERVED,
};

class SimContextMgr {
public:
    SimContextMgr() = default;
    ~SimContextMgr();

    HcclResult CreateCommEngineCtx(const std::string& tag, CommEngine engine, uint64_t size, void** ctx);
    HcclResult GetCommEngineCtx(const std::string& tag, CommEngine engine, void** ctx, uint64_t* size);
    HcclResult DestroyCommEngineCtx(const HcclMem* engineCtx);

private:
    std::unordered_map<std::string, std::unordered_map<CommEngine, HcclMem>> contextMap_;
    std::unordered_map<HcclMem, std::string, HcclMemHash, HcclMemEqual> tagMap_;
    std::unordered_map<HcclMem, CommEngine, HcclMemHash, HcclMemEqual> engineMap_;
};

} // namespace HcclSim
#endif // SIM_CONTEXT_MANAGER_H
