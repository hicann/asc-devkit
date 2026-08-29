/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file internal_functions_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_INTERNAL_FUNCTIONS_IMPL__
#warning \
    "impl/simt_api/internal_functions_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use public functions or variables defined in interface header files."
#endif

#ifndef IMPL_SIMT_API_INTERNAL_FUNCTIONS_IMPL_H
#define IMPL_SIMT_API_INTERNAL_FUNCTIONS_IMPL_H

#include "simt_api/device_types.h"

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)

enum class __RoundMode {
    CAST_RINT = 0,
    CAST_FLOOR,
    CAST_CEIL,
    CAST_ROUND,
    CAST_TRUNC,
    CAST_ODD,
    CAST_HYBRID,
};

#if !defined(__NPU_HOST__) && !defined(__ASC_NPU_HOST__)
template <__RoundMode mode>
__aicore__ inline constexpr ::ROUND __internal_get_round()
{
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    if constexpr (mode == __RoundMode::CAST_RINT) {
        return ::ROUND::CAST_RINT;
    } else if constexpr (mode == __RoundMode::CAST_ROUND) {
        return ::ROUND::CAST_ROUND;
    } else if constexpr (mode == __RoundMode::CAST_FLOOR) {
        return ::ROUND::CAST_FLOOR;
    } else if constexpr (mode == __RoundMode::CAST_CEIL) {
        return ::ROUND::CAST_CEIL;
    } else if constexpr (mode == __RoundMode::CAST_TRUNC) {
        return ::ROUND::CAST_TRUNC;
    } else if constexpr (mode == __RoundMode::CAST_ODD) {
        return ::ROUND::CAST_ODD;
    } else {
        return ::ROUND::CAST_HYBRID;
    }
#else
    if constexpr (mode == __RoundMode::CAST_RINT) {
        return ::ROUND::R;
    } else if constexpr (mode == __RoundMode::CAST_ROUND) {
        return ::ROUND::A;
    } else if constexpr (mode == __RoundMode::CAST_FLOOR) {
        return ::ROUND::F;
    } else if constexpr (mode == __RoundMode::CAST_CEIL) {
        return ::ROUND::C;
    } else if constexpr (mode == __RoundMode::CAST_TRUNC) {
        return ::ROUND::Z;
    } else if constexpr (mode == __RoundMode::CAST_ODD) {
        return ::ROUND::O;
    } else {
        return ::ROUND::H;
    }
#endif
}
#endif

__SIMT_DEVICE_FUNCTIONS_DECL__ inline signed char __internal_int_to_char(int32_t x)
{
    if (x > 127) {                            // 127:upper bound of int8
        return static_cast<signed char>(127); // 127:upper bound of int8
    }
    if (x < -128) {                            // -128:lower bound of int8
        return static_cast<signed char>(-128); // -128:lower bound of int8
    }
    return static_cast<signed char>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned char __internal_uint_to_uchar(uint32_t x)
{
    if (x > 255U) {                              // 255:upper bound of uint8
        return static_cast<unsigned char>(255U); // 255:upper bound of uint8
    }
    return static_cast<unsigned char>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short int __internal_int_to_short(int32_t x)
{
    if (x > 32767) {                          // 32767:upper bound of short
        return static_cast<short int>(32767); // 32767:upper bound of short
    }
    if (x < -32768) {                          // -32768:lower bound of short
        return static_cast<short int>(-32768); // -32768:lower bound of short
    }
    return static_cast<short int>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned short int __internal_uint_to_ushort(uint32_t x)
{
    if (x > 65535U) {                                   // 65535U:upper bound of ushort
        return static_cast<unsigned short int>(65535U); // 65535U:upper bound of ushort
    }
    return static_cast<unsigned short int>(x);
}

#endif
#endif // IMPL_SIMT_API_INTERNAL_FUNCTIONS_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_INTERNAL_FUNCTIONS_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_INTERNAL_FUNCTIONS_IMPL__
#endif
