/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file kernel_operator_limits_intf.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "kernel_operator_limits_intf.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_LIMITS_INTF_H__
#endif

#ifndef ASCENDC_KERNEL_OPERATOR_LIMITS_INTF_H
#define ASCENDC_KERNEL_OPERATOR_LIMITS_INTF_H

#include "../../impl/basic_api/kernel_macros.h"
#include "kernel_tensor.h"
#include "kernel_operator_vec_duplicate_intf.h"
#include "../../impl/basic_api/utils/kernel_utils_ceil_oom_que.h"
#include "../../impl/basic_api/utils/kernel_utils_struct_norm_sort.h"

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <cstdint>
#include "stub_def.h"
#include "kernel_bf16.h"
#endif

namespace AscendC {
template <typename T>
struct NumericLimits {
public:
    constexpr __aicore__ static inline T Max()
    {
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162)
        static_assert(
            SupportType<T, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t>(),
            "current data type is not supported on current device!");
#else
        static_assert(
            SupportType<T, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float>(),
            "current data type is not supported on current device!");
#endif
        if constexpr (AscendC::Std::is_same<T, uint8_t>::value) {
            return GetScalarBitcodeValue<uint8_t, T>(0xFFu);
        } else if constexpr (AscendC::Std::is_same<T, int8_t>::value) {
            return GetScalarBitcodeValue<uint8_t, T>(0x7Fu);
        } else if constexpr (AscendC::Std::is_same<T, uint16_t>::value) {
            return GetScalarBitcodeValue<uint16_t, T>(0xFFFFu);
        } else if constexpr (AscendC::Std::is_same<T, int16_t>::value) {
            return GetScalarBitcodeValue<uint16_t, T>(0x7FFFu);
        } else if constexpr (AscendC::Std::is_same<T, uint32_t>::value) {
            return GetScalarBitcodeValue<uint32_t, T>(0xFFFFFFFFu);
        } else if constexpr (AscendC::Std::is_same<T, int32_t>::value) {
            return GetScalarBitcodeValue<uint32_t, T>(0x7FFFFFFFu);
        } else if constexpr (AscendC::Std::is_same<T, half>::value) {
            return GetScalarBitcodeValue<uint16_t, T>(0x7BFFu);
        } else if constexpr (AscendC::Std::is_same<T, float>::value) {
            return GetScalarBitcodeValue<uint32_t, T>(0x7F7FFFFFu);
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162)
        } else if constexpr (AscendC::Std::is_same<T, bfloat16_t>::value) {
            return GetScalarBitcodeValue<uint16_t, T>(0x7F7Fu);
#endif
        }
        return T();
    }

    constexpr __aicore__ static inline T Lowest()
    {
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162)
        static_assert(
            SupportType<T, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t>(),
            "current data type is not supported on current device!");
#else
        static_assert(
            SupportType<T, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float>(),
            "current data type is not supported on current device!");
#endif
        if constexpr (AscendC::Std::is_same<T, uint8_t>::value) {
            return GetScalarBitcodeValue<uint8_t, T>(0x0u);
        } else if constexpr (AscendC::Std::is_same<T, int8_t>::value) {
            return GetScalarBitcodeValue<uint8_t, T>(0x80u);
        } else if constexpr (AscendC::Std::is_same<T, uint16_t>::value) {
            return GetScalarBitcodeValue<uint16_t, T>(0x0u);
        } else if constexpr (AscendC::Std::is_same<T, int16_t>::value) {
            return GetScalarBitcodeValue<uint16_t, T>(0x8000u);
        } else if constexpr (AscendC::Std::is_same<T, uint32_t>::value) {
            return GetScalarBitcodeValue<uint32_t, T>(0x0u);
        } else if constexpr (AscendC::Std::is_same<T, int32_t>::value) {
            return GetScalarBitcodeValue<uint32_t, T>(0x80000000u);
        } else if constexpr (AscendC::Std::is_same<T, half>::value) {
            return GetScalarBitcodeValue<uint16_t, T>(0xFBFFu);
        } else if constexpr (AscendC::Std::is_same<T, float>::value) {
            return GetScalarBitcodeValue<uint32_t, T>(0xFF7FFFFFu);
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162)
        } else if constexpr (AscendC::Std::is_same<T, bfloat16_t>::value) {
            return GetScalarBitcodeValue<uint16_t, T>(0xFF7Fu);
#endif
        }
        return T();
    }

    constexpr __aicore__ static inline T Min()
    {
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162)
        static_assert(
            SupportType<T, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t>(),
            "current data type is not supported on current device!");
#else
        static_assert(
            SupportType<T, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float>(),
            "current data type is not supported on current device!");
#endif
        if constexpr (AscendC::Std::is_same<T, uint8_t>::value) {
            return GetScalarBitcodeValue<uint8_t, T>(0x0u);
        } else if constexpr (AscendC::Std::is_same<T, int8_t>::value) {
            return GetScalarBitcodeValue<uint8_t, T>(0x80u);
        } else if constexpr (AscendC::Std::is_same<T, uint16_t>::value) {
            return GetScalarBitcodeValue<uint16_t, T>(0x0u);
        } else if constexpr (AscendC::Std::is_same<T, int16_t>::value) {
            return GetScalarBitcodeValue<uint16_t, T>(0x8000u);
        } else if constexpr (AscendC::Std::is_same<T, uint32_t>::value) {
            return GetScalarBitcodeValue<uint32_t, T>(0x0u);
        } else if constexpr (AscendC::Std::is_same<T, int32_t>::value) {
            return GetScalarBitcodeValue<uint32_t, T>(0x80000000u);
        } else if constexpr (AscendC::Std::is_same<T, half>::value) {
            return GetScalarBitcodeValue<uint16_t, T>(0x0400u);
        } else if constexpr (AscendC::Std::is_same<T, float>::value) {
            return GetScalarBitcodeValue<uint32_t, T>(0x00800000u);
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162)
        } else if constexpr (AscendC::Std::is_same<T, bfloat16_t>::value) {
            return GetScalarBitcodeValue<uint16_t, T>(0x0080u);
#endif
        }
        return T();
    }

    constexpr __aicore__ static inline T Infinity()
    {
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162)
        static_assert(
            SupportType<T, half, float, bfloat16_t>(), "current data type is not supported on current device!");
#else
        static_assert(SupportType<T, half, float>(), "current data type is not supported on current device!");
#endif
        if constexpr (AscendC::Std::is_same<T, half>::value) {
            return GetScalarBitcodeValue<uint16_t, T>(0x7C00u);
        } else if constexpr (AscendC::Std::is_same<T, float>::value) {
            return GetScalarBitcodeValue<uint32_t, T>(0x7F800000u);
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162)
        } else if constexpr (AscendC::Std::is_same<T, bfloat16_t>::value) {
            return GetScalarBitcodeValue<uint16_t, T>(0x7F80u);
#endif
        }
        return T();
    }

    constexpr __aicore__ static inline T NegativeInfinity()
    {
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162)
        static_assert(
            SupportType<T, half, float, bfloat16_t>(), "current data type is not supported on current device!");
#else
        static_assert(SupportType<T, half, float>(), "current data type is not supported on current device!");
#endif
        if constexpr (AscendC::Std::is_same<T, half>::value) {
            return GetScalarBitcodeValue<uint16_t, T>(0xFC00u);
        } else if constexpr (AscendC::Std::is_same<T, float>::value) {
            return GetScalarBitcodeValue<uint32_t, T>(0xFF800000u);
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162)
        } else if constexpr (AscendC::Std::is_same<T, bfloat16_t>::value) {
            return GetScalarBitcodeValue<uint16_t, T>(0xFF80u);
#endif
        }
        return T();
    }

    constexpr __aicore__ static inline T QuietNaN()
    {
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162)
        static_assert(
            SupportType<T, half, float, bfloat16_t>(), "current data type is not supported on current device!");
#else
        static_assert(SupportType<T, half, float>(), "current data type is not supported on current device!");
#endif
        if constexpr (AscendC::Std::is_same<T, half>::value) {
            return GetScalarBitcodeValue<uint16_t, T>(0x7E00u);
        } else if constexpr (AscendC::Std::is_same<T, float>::value) {
            return GetScalarBitcodeValue<uint32_t, T>(0x7FC00000u);
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162)
        } else if constexpr (AscendC::Std::is_same<T, bfloat16_t>::value) {
            return GetScalarBitcodeValue<uint16_t, T>(0x7FC0u);
#endif
        }
        return T();
    }

    constexpr __aicore__ static inline T SignalingNaN()
    {
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162)
        static_assert(
            SupportType<T, half, float, bfloat16_t>(), "current data type is not supported on current device!");
#else
        static_assert(SupportType<T, half, float>(), "current data type is not supported on current device!");
#endif
        if constexpr (AscendC::Std::is_same<T, half>::value) {
            return GetScalarBitcodeValue<uint16_t, T>(0x7D00u);
        } else if constexpr (AscendC::Std::is_same<T, float>::value) {
            return GetScalarBitcodeValue<uint32_t, T>(0x7FA00000u);
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162)
        } else if constexpr (AscendC::Std::is_same<T, bfloat16_t>::value) {
            return GetScalarBitcodeValue<uint16_t, T>(0x7FA0u);
#endif
        }
        return T();
    }

    constexpr __aicore__ static inline T DeNormMin()
    {
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162)
        static_assert(
            SupportType<T, half, float, bfloat16_t>(), "current data type is not supported on current device!");
#else
        static_assert(SupportType<T, half, float>(), "current data type is not supported on current device!");
#endif
        if constexpr (AscendC::Std::is_same<T, half>::value) {
            return GetScalarBitcodeValue<uint16_t, T>(0x0001u);
        } else if constexpr (AscendC::Std::is_same<T, float>::value) {
            return GetScalarBitcodeValue<uint32_t, T>(0x00000001u);
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162)
        } else if constexpr (AscendC::Std::is_same<T, bfloat16_t>::value) {
            return GetScalarBitcodeValue<uint16_t, T>(0x0001u);
#endif
        }
        return T();
    }

    __aicore__ static inline void Max(const LocalTensor<T>& dst, uint32_t count) { Duplicate(dst, Max(), count); }

    __aicore__ static inline void Lowest(const LocalTensor<T>& dst, uint32_t count) { Duplicate(dst, Lowest(), count); }

    __aicore__ static inline void Min(const LocalTensor<T>& dst, uint32_t count) { Duplicate(dst, Min(), count); }

    __aicore__ static inline void Infinity(const LocalTensor<T>& dst, uint32_t count)
    {
        Duplicate(dst, Infinity(), count);
    }

    __aicore__ static inline void NegativeInfinity(const LocalTensor<T>& dst, uint32_t count)
    {
        Duplicate(dst, NegativeInfinity(), count);
    }

    __aicore__ static inline void QuietNaN(const LocalTensor<T>& dst, uint32_t count)
    {
        Duplicate(dst, QuietNaN(), count);
    }

    __aicore__ static inline void SignalingNaN(const LocalTensor<T>& dst, uint32_t count)
    {
        Duplicate(dst, SignalingNaN(), count);
    }

    __aicore__ static inline void DeNormMin(const LocalTensor<T>& dst, uint32_t count)
    {
        Duplicate(dst, DeNormMin(), count);
    }
};
} // namespace AscendC

#endif // ASCENDC_KERNEL_OPERATOR_LIMITS_INTF_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_LIMITS_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_LIMITS_INTF_H__
#endif
