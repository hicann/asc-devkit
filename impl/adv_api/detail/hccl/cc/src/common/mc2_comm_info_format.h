/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef MC2_COMM_INFO_FORMAT_H
#define MC2_COMM_INFO_FORMAT_H
#include <cstdint>

namespace Hccl {
// Existing profiler wire layout, shared by the Host reporter and copied runtime adapter.
struct ProfilingDeviceCommResInfo {
    uint64_t groupName;
    uint32_t rankSize;
    uint32_t rankId;
    uint32_t usrRankId;
    uint32_t aicpuKfcStreamId;
    uint32_t commStreamSize;
    uint32_t commStreamIds[8];
    uint32_t reserve;
};
} // namespace Hccl
#endif
