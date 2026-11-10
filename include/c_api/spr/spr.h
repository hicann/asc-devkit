/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "spr/spr.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SPR_SPR_H
#endif

#ifndef INCLUDE_C_API_SPR_SPR_H
#define INCLUDE_C_API_SPR_SPR_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "impl/c_api/memory_base_impl/spr_intf_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/spr_intf_impl.h"
#endif

/**
 * @ingroup simd_c_api.spr
 * @brief Reads the value of the CTRL control register.
 *
 * @override_id asc_get_ctrl__z
 * @archs 2201 3510
 */
__aicore__ inline int64_t asc_get_ctrl();

/**
 * @ingroup simd_c_api.spr
 * @brief Sets specific bits of the CTRL control register.
 *
 * @override_id asc_set_ctrl__u64
 * @archs 2201 3510
 * @param [in] config Register value to write; the page documents the commonly used bits: CTRL[48] floating-point
 * saturation mode (0 saturates INF to +/-MAX and NAN to 0, 1 keeps them), CTRL[49] whether scalar UB access goes
 * through DCache, CTRL[50] saturation for fp8_e8m0_t/fp8_e5m2_t/fp8_e4m3fn_t scenarios, CTRL[53] integer saturation
 * versus truncation, CTRL[56] MASK mode (0 per-element mask register, 1 MASK[31:0] element count) and CTRL[59]/CTRL[60]
 * for float-to-integer and integer-to-integer precision-conversion saturation.
 */
__aicore__ inline void asc_set_ctrl(uint64_t config);

/**
 * @ingroup simd_c_api.spr
 * @brief Reads the valid data length that squeeze operations accumulated in the AR special register.
 *
 * @override_id asc_get_squeeze_status__z
 * @archs 3510
 */
__aicore__ inline int64_t asc_get_squeeze_status();

/**
 * @ingroup simd_c_api.spr
 * @brief Enables or disables saturation for a given saturation mode.
 *
 * @override_id asc_set_saturation_flag__saturation_b
 * @archs 3510
 * @param [in] saturation_mode Saturation mode to configure, of type asc_saturation_mode (FLOAT = 48 for floating-point
 * computation and part of the precision conversions, FLOAT8 = 50 for fp8 scenarios, INT = 53 for integer computation,
 * CAST = 59 for float-to-integer and integer-to-integer conversions).
 * @param [in] enable_sat Whether to enable saturation: true enables it, false disables it, which means truncation for
 * INT and non-saturation or truncation for FLOAT/FLOAT8/CAST as detailed in the mode table.
 */
__aicore__ inline void asc_set_saturation_flag(asc_saturation_mode saturation_mode, bool enable_sat);
/**
 * @ingroup simd_c_api.spr
 * @brief Reads whether saturation is enabled for a given saturation mode.
 *
 * @override_id asc_get_saturation_flag__saturation
 * @archs 3510
 * @param [in] saturation_mode Saturation mode whose enabled state is queried, of type asc_saturation_mode (header enum
 * values FLOAT = 48, FLOAT8 = 50, INT = 53, CAST = 59).
 */
__aicore__ inline bool asc_get_saturation_flag(asc_saturation_mode saturation_mode);
/**
 * @ingroup simd_c_api.spr
 * @brief Sets the saturation control strategy that decides between per-API and global saturation configuration.
 *
 * @override_id asc_set_saturation_strategy__strategy
 * @archs 3510
 * @param [in] strategy Saturation control strategy of type asc_override_strategy: USE_API configures saturation per
 * instruction or API path, USE_GLOBAL uses the global saturation configuration, which is the default.
 */
__aicore__ inline void asc_set_saturation_strategy(asc_override_strategy strategy);
/**
 * @ingroup simd_c_api.spr
 * @brief Reads the current saturation control strategy.
 *
 * @override_id asc_get_saturation_strategy__z
 * @archs 3510
 */
__aicore__ inline asc_override_strategy asc_get_saturation_strategy();

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_get_squeeze_status)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated getter that reads the value of a special register.
 *
 * @override_id asc_get_ar_spr__z
 * @archs 3510
 */
__aicore__ inline int64_t asc_get_ar_spr();

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SPR_SPR_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SPR_SPR_H
#endif
