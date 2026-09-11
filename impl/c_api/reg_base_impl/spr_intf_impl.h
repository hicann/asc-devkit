/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning \
    "impl/c_api/reg_base_impl/spr_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_SPR_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_SPR_INTF_IMPL_H

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/reg_base_impl/npu_arch_3510/spr_intf_impl.h"
#endif

__aicore__ inline int64_t asc_get_squeeze_status() { return get_ar(); }

__aicore__ inline void asc_set_ctrl(uint64_t config) { set_ctrl(config); }

__aicore__ inline int64_t asc_get_ctrl() { return get_ctrl(); }

__aicore__ inline void asc_set_saturation_flag(asc_saturation_mode saturation_mode, bool enable_sat)
{
    // Enum underlying values are CTRL bit indices. INT: 1=sat; FLOAT/FLOAT8/CAST: 0=sat.
    const int64_t bit = static_cast<int64_t>(saturation_mode);
    const bool is_bit_set = (saturation_mode == asc_saturation_mode::INT) ? enable_sat : !enable_sat;
    set_ctrl(is_bit_set ? sbitset1(get_ctrl(), bit) : sbitset0(get_ctrl(), bit));
}

__aicore__ inline bool asc_get_saturation_flag(asc_saturation_mode saturation_mode)
{
    // Enum underlying values are CTRL bit indices. INT: 1=sat; FLOAT/FLOAT8/CAST: 0=sat.
    const uint8_t bit = static_cast<uint8_t>(saturation_mode);
    const bool is_bit_set = ((get_ctrl() >> bit) & 1) != 0;
    return (saturation_mode == asc_saturation_mode::INT) ? is_bit_set : !is_bit_set;
}

__aicore__ inline void asc_set_saturation_strategy(asc_override_strategy strategy)
{
    // CTRL[60]: 0 => USE_API, 1 => USE_GLOBAL. Aligns with AscendC::SetSaturationStrategy.
    constexpr int32_t SATURATION_STRATEGY_BIT = 60;
    int64_t ctrl_value = get_ctrl();
    set_ctrl(
        strategy == asc_override_strategy::USE_API ? sbitset0(ctrl_value, SATURATION_STRATEGY_BIT) :
                                                     sbitset1(ctrl_value, SATURATION_STRATEGY_BIT));
}

__aicore__ inline asc_override_strategy asc_get_saturation_strategy()
{
    // CTRL[60]: 0 => USE_API, 1 => USE_GLOBAL. Aligns with AscendC::GetSaturationStrategy.
    int64_t value = (get_ctrl() >> 60) & 1;
    return value == 0 ? asc_override_strategy::USE_API : asc_override_strategy::USE_GLOBAL;
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
