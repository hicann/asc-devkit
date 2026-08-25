/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef INCLUDE_C_API_DEFS_ENUM_H
#define INCLUDE_C_API_DEFS_ENUM_H

#include <cstdint>
#include <type_traits>

enum class asc_load_l2_cache_mode : uint8_t {
    NORMAL_FIRST_VICTIM = 0,
    NORMAL_LAST_VICTIM = 1,
    NORMAL_PERSISTENT = 2,
    NOTALLOC_KEEP = 4,
    NOTALLOC_CLEAN = 5,
    NOTALLOC_DROP = 6
};

enum class asc_store_l2_cache_mode : uint8_t {
    NORMAL_FIRST_VICTIM = 0,
    NORMAL_LAST_VICTIM = 1,
    NORMAL_PERSISTENT = 2,
    NOTALLOC_CLEAN = 4
};

enum class asc_hf32_round_mode : uint8_t {
    NEAREST_AWAY = 0,
    NEAREST_EVEN = 1,
    NEAREST_ZERO = NEAREST_AWAY // Compatible with the old name; the actual behavior is equivalent to NEAREST_AWAY.
};

enum class asc_saturation_mode : uint8_t {
    FLOAT = 48,  // 浮点数计算和浮点数精度转换饱和模式
    FLOAT8 = 50, // 浮点数精度转换时NaN饱和模式
    INT = 53,    // 整数计算指令饱和模式
    CAST = 59    // 浮点数转整数或整数转整数精度转换饱和模式
};

enum class asc_override_strategy : uint8_t {
    USE_API = 0,   // 饱和模式为单指令设置饱和
    USE_GLOBAL = 1 // 饱和模式为全局设置饱和
};

enum class asc_position_mode { EVEN = 0, ODD };

constexpr std::integral_constant<asc_position_mode, asc_position_mode::EVEN> ASC_POSITION_EVEN;
constexpr std::integral_constant<asc_position_mode, asc_position_mode::ODD> ASC_POSITION_ODD;

enum class asc_position_quarter_mode {
    DISPERSE_FIRST_QUARTER = 0,
    DISPERSE_SECOND_QUARTER,
    DISPERSE_THIRD_QUARTER,
    DISPERSE_FOURTH_QUARTER
};

enum class asc_dual_dst_mode : uint8_t { DUAL_DST_DISABLE = 0, DUAL_DST_SPLIT_M = 1, DUAL_DST_SPLIT_N = 2 };

enum class asc_unit_flag_mode : uint8_t { DISABLE = 0, ENABLE_KEEP = 2, ENABLE_UPDATE = 3 };

enum class asc_relu_pre_mode : uint8_t {
    NONE = 0,
    NORMAL = 1,
    SCALAR = 2,
    VECTOR = 3,
};

using asc_quant_mode = QuantMode_t;

constexpr std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FIRST_QUARTER>
    ASC_DISPERSE_FIRST_QUARTER;
constexpr std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_SECOND_QUARTER>
    ASC_DISPERSE_SECOND_QUARTER;
constexpr std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_THIRD_QUARTER>
    ASC_DISPERSE_THIRD_QUARTER;
constexpr std::integral_constant<asc_position_quarter_mode, asc_position_quarter_mode::DISPERSE_FOURTH_QUARTER>
    ASC_DISPERSE_FOURTH_QUARTER;

#endif
