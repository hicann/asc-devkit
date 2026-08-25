/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <sstream>
#include "securec.h"
#include "hccl/base.h"
#include "hccl_common.h"
#include "log.h"

namespace mc2_ops_hccl {
HcclMem HcclMemRange(HcclMem inMem, u64 offset, u64 size);

static inline u64 RoundUpWithDivisor(u64 value, u64 divisor)
{
    if ((value == 0) || (divisor == 0)) {
        return divisor;
    }
    // divisor必须大于等于1, 返回value向上取divisor的整数倍的值
    return ((value + (divisor - 1)) / divisor) * divisor;
}

template <typename... Args>
inline std::string StringFormat(const char* format, Args... args)
{
    using namespace std;
    constexpr size_t bufSize = BUFSIZ;
    char buffer[bufSize];
    int formatRet = snprintf_s(&buffer[0], bufSize, bufSize, format, args...);
    if (formatRet < 0) {
        HCCL_ERROR("[StringFormat] failed to format string into the local buffer");
        return "";
    }
    size_t actualSize = static_cast<size_t>(formatRet) + 1;

    if (actualSize > bufSize) {
        std::vector<char> newbuffer(actualSize);
        int retryRet = snprintf_s(newbuffer.data(), actualSize, actualSize, format, args...);
        if (retryRet < 0) {
            HCCL_ERROR("[StringFormat] failed to format string into the resized buffer");
            return "";
        }
        return std::string(newbuffer.data(), static_cast<size_t>(retryRet));
    }
    return std::string(buffer, static_cast<size_t>(formatRet));
}
} // namespace mc2_ops_hccl

#endif
