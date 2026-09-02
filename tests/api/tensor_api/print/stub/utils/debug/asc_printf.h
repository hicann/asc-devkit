/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef TESTS_API_TENSOR_API_PRINT_STUB_UTILS_DEBUG_ASC_PRINTF_H
#define TESTS_API_TENSOR_API_PRINT_STUB_UTILS_DEBUG_ASC_PRINTF_H

#include <cstdio>
#include <string>
#include <utility>
#include <vector>

namespace tensor_api_print_test {

struct print_record {
    std::string head;
    std::string content;
};

inline std::vector<print_record>& records()
{
    static std::vector<print_record> value;
    return value;
}

inline void reset_records()
{
    records().clear();
}

template <typename... Args>
inline std::string format(const char* fmt, Args&&... args)
{
    int size = std::snprintf(nullptr, 0, fmt, std::forward<Args>(args)...);
    if (size <= 0) {
        return {};
    }
    std::vector<char> buffer(static_cast<size_t>(size) + 1);
    std::snprintf(buffer.data(), buffer.size(), fmt, std::forward<Args>(args)...);
    return std::string(buffer.data(), static_cast<size_t>(size));
}

template <typename... Args>
inline void append_record(const char* head, const char* fmt, Args&&... args)
{
    records().push_back({head, format(fmt, std::forward<Args>(args)...)});
}

inline std::string output()
{
    std::string value;
    for (const auto& record : records()) {
        value += record.head;
        value += record.content;
    }
    return value;
}

} // namespace tensor_api_print_test

namespace __asc_aicore {

enum class DumpType {
    DUMP_SCALAR,
    DUMP_ASSERT,
};

template <typename... Args>
__aicore__ inline void printf(__gm__ const char* fmt, Args&&... args)
{
    tensor_api_print_test::append_record("[AIC Block 0/1] ", fmt, static_cast<Args&&>(args)...);
}

template <typename... Args>
__aicore__ inline void printf_impl_assert(__gm__ const char* fmt, Args&&... args)
{
    tensor_api_print_test::append_record("", fmt, static_cast<Args&&>(args)...);
}

template <typename... Args>
__aicore__ inline void scalar_printf_impl(
    DumpType, __gm__ const char* fmt, __gm__ const char* dumphead, Args&&... args)
{
    tensor_api_print_test::append_record(dumphead, fmt, static_cast<Args&&>(args)...);
}

} // namespace __asc_aicore

#endif // TESTS_API_TENSOR_API_PRINT_STUB_UTILS_DEBUG_ASC_PRINTF_H
