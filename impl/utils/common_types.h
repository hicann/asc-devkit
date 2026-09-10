/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file common_types.h
 * \brief
 */
#ifndef IMPL_UTILS_COMMON_TYPES_H
#define IMPL_UTILS_COMMON_TYPES_H

#include <cstdint>
#include <type_traits>
#include "sys_macros.h"

namespace AscendC {
enum class Hardware : uint8_t {
    GM,
    UB,
    L1,
    L0A,
    L0B,
    L0C,
    BIAS,
    FIXBUF,
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    PT,
#endif
    MAX
};

enum class ReduceType {
    NONE = -1,
    SUM = 0,
    MAX,
    MIN,
};
} // namespace AscendC

#if !defined(ASCENDC_CPU_DEBUG)
using fp4x2_e2m1_t = float4_e2m1x2_t;
using fp4x2_e1m2_t = float4_e1m2x2_t;
using fp8_e5m2_t = float8_e5m2_t;
using fp8_e4m3fn_t = float8_e4m3_t;
using fp8_e8m0_t = float8_e8m0_t;
#endif

constexpr uint32_t INT4_BIT_NUM = 4;

template <bool condition, class T1, class T2>
struct conditional {
    using type = T1;
};

template <int bit_num, bool sign = true>
struct integer_sub_type {
    static int const k_bits = bit_num;
    static bool const k_signed = sign;

    using T = typename conditional<k_signed, int8_t, uint8_t>::type;
    using Storage = uint8_t;

    static Storage const mask = Storage(((static_cast<uint64_t>(1)) << static_cast<uint32_t>(k_bits)) - 1);
    Storage storage;
    __aicore__ inline integer_sub_type() = default;

    __aicore__ inline integer_sub_type(uint32_t value) : storage(reinterpret_cast<Storage const&>(value) & mask) {}

    __aicore__ inline integer_sub_type(int32_t value) : storage(reinterpret_cast<Storage const&>(value) & mask) {}

    __aicore__ inline operator T() const
    {
        if (k_signed && ((storage & Storage(static_cast<uint64_t>(1) << static_cast<uint32_t>(k_bits - 1))) != 0)) {
            // Sign extend
            return T(storage) | ~T(mask);
        }
        return T(storage);
    }

    __aicore__ inline bool operator==(integer_sub_type const& rhs) const { return storage == rhs.storage; }

    __aicore__ inline bool operator!=(integer_sub_type const& rhs) const { return storage != rhs.storage; }

    __aicore__ inline bool operator>(integer_sub_type const& rhs) const
    {
        bool lhs_is_neg = (this->storage & (static_cast<uint64_t>(1) << static_cast<uint32_t>(this->k_bits - 1)));
        bool rhs_is_neg = (rhs.storage & (static_cast<uint64_t>(1) << static_cast<uint32_t>(rhs.k_bits - 1)));
        if (k_signed && (lhs_is_neg != rhs_is_neg)) {
            return (!lhs_is_neg) && rhs_is_neg;
        }
        return this->storage > rhs.storage;
    }

    __aicore__ inline bool operator>=(integer_sub_type const& rhs) const
    {
        bool lhs_is_neg = (this->storage & (static_cast<uint64_t>(1) << static_cast<uint32_t>(this->k_bits - 1)));
        bool rhs_is_neg = (rhs.storage & (static_cast<uint64_t>(1) << static_cast<uint32_t>(rhs.k_bits - 1)));
        if (k_signed && (lhs_is_neg != rhs_is_neg)) {
            return (!lhs_is_neg) && rhs_is_neg;
        }
        return storage >= rhs.storage;
    }

    __aicore__ inline bool operator<(integer_sub_type const& rhs) const { return !(*this >= rhs); }

    __aicore__ inline bool operator<=(integer_sub_type const& rhs) const { return !(*this > rhs); }
};

using int4b_t = integer_sub_type<INT4_BIT_NUM, true>;

#endif
