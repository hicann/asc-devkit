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
#error "datamove_atomic.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ATOMIC_DATAMOVE_ATOMIC_H
#endif

#ifndef INCLUDE_C_API_ATOMIC_DATAMOVE_ATOMIC_H
#define INCLUDE_C_API_ATOMIC_DATAMOVE_ATOMIC_H

#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "impl/c_api/memory_base_impl/datamove_atomic_intf_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/datamove_atomic_intf_impl.h"
#endif

/**
 * @ingroup simd_c_api.atomic.datamove_atomic
 * @brief Enables atomic accumulation for subsequent GM-destined data-move instructions, adding the moved 32-bit
 * floating point data to the content already held in GM.
 *
 * @override_id asc_set_atomic_add_float__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_set_atomic_add_float();
/**
 * @ingroup simd_c_api.atomic.datamove_atomic
 * @brief Enables atomic accumulation for subsequent GM-destined data-move instructions, adding the moved 8-bit signed
 * integer data to the content already held in GM.
 *
 * @override_id asc_set_atomic_add_int8__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_set_atomic_add_int8();
/**
 * @ingroup simd_c_api.atomic.datamove_atomic
 * @brief Enables atomic accumulation for subsequent GM-destined data-move instructions, adding the moved 16-bit signed
 * integer data to the content already held in GM.
 *
 * @override_id asc_set_atomic_add_int16__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_set_atomic_add_int16();
/**
 * @ingroup simd_c_api.atomic.datamove_atomic
 * @brief Enables atomic accumulation for subsequent GM-destined data-move instructions, adding the moved 32-bit signed
 * integer data to the content already held in GM.
 *
 * @override_id asc_set_atomic_add_int32__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_set_atomic_add_int32();
/**
 * @ingroup simd_c_api.atomic.datamove_atomic
 * @brief Disables DMA atomic operations after an atomic sequence so that later data movement is not affected by the
 * previous atomic configuration.
 *
 * @override_id asc_disable_dma_atomic__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_disable_dma_atomic();
/**
 * @ingroup simd_c_api.atomic.datamove_atomic
 * @brief Enables atomic accumulation for subsequent GM-destined data-move instructions, adding the moved 16-bit brain
 * floating point data to the content already held in GM.
 *
 * @override_id asc_set_atomic_add_bfloat__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_set_atomic_add_bfloat();
/**
 * @ingroup simd_c_api.atomic.datamove_atomic
 * @brief Enables atomic accumulation for subsequent GM-destined data-move instructions, adding the moved 16-bit
 * floating point (half) data to the content already held in GM.
 *
 * @override_id asc_set_atomic_add_float16__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_set_atomic_add_float16();
/**
 * @ingroup simd_c_api.atomic.datamove_atomic
 * @brief Enables atomic compare-and-take-maximum for subsequent GM-destined data-move instructions, writing the larger
 * of the moved 16-bit brain floating point data and the content already held in GM.
 *
 * @override_id asc_set_atomic_max_bfloat__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_set_atomic_max_bfloat();
/**
 * @ingroup simd_c_api.atomic.datamove_atomic
 * @brief Enables atomic compare-and-take-maximum for subsequent GM-destined data-move instructions, writing the larger
 * of the moved 32-bit floating point data and the content already held in GM.
 *
 * @override_id asc_set_atomic_max_float__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_set_atomic_max_float();
/**
 * @ingroup simd_c_api.atomic.datamove_atomic
 * @brief Enables atomic compare-and-take-maximum for subsequent GM-destined data-move instructions, writing the larger
 * of the moved 16-bit floating point (half) data and the content already held in GM.
 *
 * @override_id asc_set_atomic_max_float16__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_set_atomic_max_float16();
/**
 * @ingroup simd_c_api.atomic.datamove_atomic
 * @brief Enables atomic compare-and-take-maximum for subsequent GM-destined data-move instructions, writing the larger
 * of the moved 8-bit signed integer data and the content already held in GM.
 *
 * @override_id asc_set_atomic_max_int8__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_set_atomic_max_int8();
/**
 * @ingroup simd_c_api.atomic.datamove_atomic
 * @brief Enables atomic compare-and-take-maximum for subsequent GM-destined data-move instructions, writing the larger
 * of the moved 16-bit signed integer data and the content already held in GM.
 *
 * @override_id asc_set_atomic_max_int16__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_set_atomic_max_int16();
/**
 * @ingroup simd_c_api.atomic.datamove_atomic
 * @brief Enables atomic compare-and-take-maximum for subsequent GM-destined data-move instructions, writing the larger
 * of the moved 32-bit signed integer data and the content already held in GM.
 *
 * @override_id asc_set_atomic_max_int32__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_set_atomic_max_int32();
/**
 * @ingroup simd_c_api.atomic.datamove_atomic
 * @brief Enables atomic compare-and-take-minimum for subsequent GM-destined data-move instructions, writing the smaller
 * of the moved 16-bit brain floating point data and the content already held in GM.
 *
 * @override_id asc_set_atomic_min_bfloat__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_set_atomic_min_bfloat();
/**
 * @ingroup simd_c_api.atomic.datamove_atomic
 * @brief Enables atomic compare-and-take-minimum for subsequent GM-destined data-move instructions, writing the smaller
 * of the moved 32-bit floating point data and the content already held in GM.
 *
 * @override_id asc_set_atomic_min_float__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_set_atomic_min_float();
/**
 * @ingroup simd_c_api.atomic.datamove_atomic
 * @brief Enables atomic compare-and-take-minimum for subsequent GM-destined data-move instructions, writing the smaller
 * of the moved 16-bit floating point (half) data and the content already held in GM.
 *
 * @override_id asc_set_atomic_min_float16__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_set_atomic_min_float16();
/**
 * @ingroup simd_c_api.atomic.datamove_atomic
 * @brief Enables atomic compare-and-take-minimum for subsequent GM-destined data-move instructions, writing the smaller
 * of the moved 8-bit signed integer data and the content already held in GM.
 *
 * @override_id asc_set_atomic_min_int8__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_set_atomic_min_int8();
/**
 * @ingroup simd_c_api.atomic.datamove_atomic
 * @brief Enables atomic compare-and-take-minimum for subsequent GM-destined data-move instructions, writing the smaller
 * of the moved 16-bit signed integer data and the content already held in GM.
 *
 * @override_id asc_set_atomic_min_int16__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_set_atomic_min_int16();
/**
 * @ingroup simd_c_api.atomic.datamove_atomic
 * @brief Enables atomic compare-and-take-minimum for subsequent GM-destined data-move instructions, writing the smaller
 * of the moved 32-bit signed integer data and the content already held in GM.
 *
 * @override_id asc_set_atomic_min_int32__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_set_atomic_min_int32();

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_disable_dma_atomic)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated switch that turns atomic accumulation/comparison off after use.
 *
 * @override_id asc_set_atomic_none__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_set_atomic_none();

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_set_atomic_add_int32)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated switch that enables int32 atomic accumulation for subsequent UB/L0C/L1-to-GM transfers.
 *
 * @override_id asc_set_atomic_add_int__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_set_atomic_add_int();

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_set_atomic_max_int32)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated switch that makes subsequent GM transfers write the element-wise maximum against existing GM data
 * for int32_t.
 *
 * @override_id asc_set_atomic_max_int__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_set_atomic_max_int();

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_set_atomic_min_int32)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated switch that makes subsequent GM transfers write the element-wise minimum against existing GM data
 * for int32_t.
 *
 * @override_id asc_set_atomic_min_int__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_set_atomic_min_int();

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ATOMIC_DATAMOVE_ATOMIC_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ATOMIC_DATAMOVE_ATOMIC_H
#endif
