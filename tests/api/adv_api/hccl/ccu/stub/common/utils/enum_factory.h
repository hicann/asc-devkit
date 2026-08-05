/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef ENUM_FACTORY_H
#define ENUM_FACTORY_H

#include "string_util.h"
#include <string>
#include <sstream>

#define MAKE_ENUM(enumClass, ...)                                                                                    \
    class enumClass {                                                                                                \
    public:                                                                                                          \
        enum Value : uint8_t { __VA_ARGS__, __COUNT__, INVALID };                                                    \
                                                                                                                     \
        enumClass() = default;                                                                                       \
                                                                                                                     \
        constexpr enumClass(Value v) : value(v) {}                                                                   \
                                                                                                                     \
        constexpr operator Value() const { return value; }                                                           \
                                                                                                                     \
        constexpr bool operator==(enumClass a) const { return value == a.value; }                                    \
                                                                                                                     \
        constexpr bool operator!=(enumClass a) const { return value != a.value; }                                    \
                                                                                                                     \
        constexpr bool operator<(enumClass a) const { return value < a.value; }                                      \
                                                                                                                     \
        constexpr bool operator==(Value v) const { return value == v; }                                              \
                                                                                                                     \
        constexpr bool operator!=(Value v) const { return value != v; }                                              \
                                                                                                                     \
        constexpr bool operator<(Value v) const { return value < v; }                                                \
                                                                                                                     \
        std::string Describe() const                                                                                 \
        {                                                                                                            \
            static std::vector<std::string> m = InitStrs();                                                          \
            if (value > m.size())                                                                                    \
                return std::string(#enumClass) + "::Invalid";                                                        \
            return m[value];                                                                                         \
        }                                                                                                            \
                                                                                                                     \
        friend std::ostream& operator<<(std::ostream& stream, const enumClass& v) { return stream << v.Describe(); } \
                                                                                                                     \
    private:                                                                                                         \
        Value value;                                                                                                 \
                                                                                                                     \
        static std::vector<std::string> InitStrs()                                                                   \
        {                                                                                                            \
            std::vector<std::string> strings;                                                                        \
            std::string s = #__VA_ARGS__;                                                                            \
            std::string token;                                                                                       \
            for (char c : s) {                                                                                       \
                if (c == ' ' || c == ',') {                                                                          \
                    if (!token.empty()) {                                                                            \
                        strings.push_back({std::string(#enumClass) + "::" + token});                                 \
                        token.clear();                                                                               \
                    }                                                                                                \
                } else {                                                                                             \
                    token += c;                                                                                      \
                }                                                                                                    \
            }                                                                                                        \
            if (!token.empty())                                                                                      \
                strings.push_back({std::string(#enumClass) + "::" + token});                                         \
            return strings;                                                                                          \
        }                                                                                                            \
    };

#endif
