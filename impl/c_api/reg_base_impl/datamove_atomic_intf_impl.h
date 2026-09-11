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
    "impl/c_api/reg_base_impl/datamove_atomic_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_DATAMOVE_ATOMIC_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_DATAMOVE_ATOMIC_INTF_IMPL_H

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/reg_base_impl/npu_arch_3510/datamove_atomic_intf_impl.h"
#endif

/// Enables atomic add for data transfer from Unified Buffer/L0C Buffer/L1 Buffer to
/// Global Memory with half precision (half).
__aicore__ inline void asc_set_atomic_add_float16()
{
    set_atomic_add();
    set_atomic_f16();
}

/// Enables atomic max for data transfer from Unified Buffer/L0C Buffer/L1 Buffer to
/// Global Memory with single precision (float).
__aicore__ inline void asc_set_atomic_max_float()
{
    set_atomic_max();
    set_atomic_f32();
}

/// Enables atomic add for data transfer from Unified Buffer/L0C Buffer/L1 Buffer to
/// Global Memory with bfloat16 precision (bfloat16_t).
__aicore__ inline void asc_set_atomic_add_bfloat()
{
    set_atomic_add();
    set_atomic_bf16();
}

/// Enables atomic min for data transfer from Unified Buffer/L0C Buffer/L1 Buffer to
/// Global Memory with single precision (float).
__aicore__ inline void asc_set_atomic_min_float()
{
    set_atomic_min();
    set_atomic_f32();
}

/// Enables atomic min for data transfer from Unified Buffer/L0C Buffer/L1 Buffer to
/// Global Memory with bfloat16 precision (bfloat16_t).
__aicore__ inline void asc_set_atomic_min_bfloat()
{
    set_atomic_min();
    set_atomic_bf16();
}

/// Enables atomic add for data transfer from Unified Buffer/L0C Buffer/L1 Buffer to
/// Global Memory with single precision (float).
__aicore__ inline void asc_set_atomic_add_float()
{
    set_atomic_f32();
    set_atomic_add();
}

/// Enables atomic max for data transfer from Unified Buffer/L0C Buffer/L1 Buffer to
/// Global Memory with half precision (half).
__aicore__ inline void asc_set_atomic_max_float16()
{
    set_atomic_f16();
    set_atomic_max();
}

/// Clears the atomic operation state, disabling any active atomic add/max/min.
__aicore__ inline void asc_disable_dma_atomic() { set_atomic_none(); }

/// Enables atomic max for data transfer from Unified Buffer/L0C Buffer/L1 Buffer to
/// Global Memory with bfloat16 precision (bfloat16_t).
__aicore__ inline void asc_set_atomic_max_bfloat()
{
    set_atomic_max();
    set_atomic_bf16();
}

/// Enables atomic min for data transfer from Unified Buffer/L0C Buffer/L1 Buffer to
/// Global Memory with half precision (half).
__aicore__ inline void asc_set_atomic_min_float16()
{
    set_atomic_min();
    set_atomic_f16();
}

/// Enables atomic add for data transfer from Unified Buffer/L0C Buffer/L1 Buffer to
/// Global Memory with 8-bit signed integer (int8_t).
__aicore__ inline void asc_set_atomic_add_int8()
{
    set_atomic_add();
    set_atomic_s8();
}

/// Enables atomic add for data transfer from Unified Buffer/L0C Buffer/L1 Buffer to
/// Global Memory with 16-bit signed integer (int16_t).
__aicore__ inline void asc_set_atomic_add_int16()
{
    set_atomic_add();
    set_atomic_s16();
}

/// Enables atomic add for data transfer from Unified Buffer/L0C Buffer/L1 Buffer to
/// Global Memory with 32-bit signed integer (int32_t).
__aicore__ inline void asc_set_atomic_add_int32()
{
    set_atomic_add();
    set_atomic_s32();
}

/// Enables atomic max for data transfer from Unified Buffer/L0C Buffer/L1 Buffer to
/// Global Memory with 8-bit signed integer (int8_t).
__aicore__ inline void asc_set_atomic_max_int8()
{
    set_atomic_max();
    set_atomic_s8();
}

/// Enables atomic max for data transfer from Unified Buffer/L0C Buffer/L1 Buffer to
/// Global Memory with 16-bit signed integer (int16_t).
__aicore__ inline void asc_set_atomic_max_int16()
{
    set_atomic_max();
    set_atomic_s16();
}

/// Enables atomic max for data transfer from Unified Buffer/L0C Buffer/L1 Buffer to
/// Global Memory with 32-bit signed integer (int32_t).
__aicore__ inline void asc_set_atomic_max_int32()
{
    set_atomic_max();
    set_atomic_s32();
}

/// Enables atomic min for data transfer from Unified Buffer/L0C Buffer/L1 Buffer to
/// Global Memory with 8-bit signed integer (int8_t).
__aicore__ inline void asc_set_atomic_min_int8()
{
    set_atomic_min();
    set_atomic_s8();
}

/// Enables atomic min for data transfer from Unified Buffer/L0C Buffer/L1 Buffer to
/// Global Memory with 16-bit signed integer (int16_t).
__aicore__ inline void asc_set_atomic_min_int16()
{
    set_atomic_min();
    set_atomic_s16();
}

/// Enables atomic min for data transfer from Unified Buffer/L0C Buffer/L1 Buffer to
/// Global Memory with 32-bit signed integer (int32_t).
__aicore__ inline void asc_set_atomic_min_int32()
{
    set_atomic_min();
    set_atomic_s32();
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
