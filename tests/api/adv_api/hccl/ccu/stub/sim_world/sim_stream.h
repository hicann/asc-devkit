/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef SIM_STREAM_H
#define SIM_STREAM_H

namespace HcclSim {
class SimStream {
public:
    SimStream(uint32_t sqId) : streamId_(sqId), isUsed_{false} {}

    inline uint32_t GetStreamId() { return streamId_; }

    inline bool IsAllocated() { return isUsed_; }

    inline void Allocate() { isUsed_ = true; }

    inline void Release() { isUsed_ = false; }

private:
    bool isUsed_;
    uint32_t streamId_;
};
} // namespace HcclSim
#endif
