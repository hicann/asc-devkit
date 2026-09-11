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
    "impl/c_api/reg_base_impl/scalar_atomic_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_SCALAR_ATOMIC_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_SCALAR_ATOMIC_INTF_IMPL_H

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/reg_base_impl/npu_arch_3510/scalar_atomic_intf_impl.h"
#endif

namespace __asc_aicore {
__aicore__ inline int32_t asc_atomic_add(__gm__ int32_t* address, int32_t val) { return atomicAdd(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline uint32_t asc_atomic_add(__gm__ uint32_t* address, uint32_t val) { return atomicAdd(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline float asc_atomic_add(__gm__ float* address, float val) { return atomicAdd(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline int64_t asc_atomic_add(__gm__ int64_t* address, int64_t val) { return atomicAdd(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline uint64_t asc_atomic_add(__gm__ uint64_t* address, uint64_t val) { return atomicAdd(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline int32_t asc_atomic_sub(__gm__ int32_t* address, int32_t val) { return atomicSub(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline uint32_t asc_atomic_sub(__gm__ uint32_t* address, uint32_t val) { return atomicSub(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline float asc_atomic_sub(__gm__ float* address, float val) { return atomicSub(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline int64_t asc_atomic_sub(__gm__ int64_t* address, int64_t val) { return atomicSub(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline uint64_t asc_atomic_sub(__gm__ uint64_t* address, uint64_t val) { return atomicSub(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline int32_t asc_atomic_exch(__gm__ int32_t* address, int32_t val) { return atomicExch(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline uint32_t asc_atomic_exch(__gm__ uint32_t* address, uint32_t val) { return atomicExch(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline float asc_atomic_exch(__gm__ float* address, float val) { return atomicExch(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline int64_t asc_atomic_exch(__gm__ int64_t* address, int64_t val) { return atomicExch(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline uint64_t asc_atomic_exch(__gm__ uint64_t* address, uint64_t val) { return atomicExch(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline int32_t asc_atomic_max(__gm__ int32_t* address, int32_t val) { return atomicMax(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline uint32_t asc_atomic_max(__gm__ uint32_t* address, uint32_t val) { return atomicMax(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline float asc_atomic_max(__gm__ float* address, float val) { return atomicMax(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline int64_t asc_atomic_max(__gm__ int64_t* address, int64_t val) { return atomicMax(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline uint64_t asc_atomic_max(__gm__ uint64_t* address, uint64_t val) { return atomicMax(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline int32_t asc_atomic_min(__gm__ int32_t* address, int32_t val) { return atomicMin(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline uint32_t asc_atomic_min(__gm__ uint32_t* address, uint32_t val) { return atomicMin(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline float asc_atomic_min(__gm__ float* address, float val) { return atomicMin(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline int64_t asc_atomic_min(__gm__ int64_t* address, int64_t val) { return atomicMin(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline uint64_t asc_atomic_min(__gm__ uint64_t* address, uint64_t val) { return atomicMin(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline int32_t asc_atomic_cas(__gm__ int32_t* address, int32_t compare, int32_t val)
{
    return atomicCAS(address, compare, val);
}
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline uint32_t asc_atomic_cas(__gm__ uint32_t* address, uint32_t compare, uint32_t val)
{
    return atomicCAS(address, compare, val);
}
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline float asc_atomic_cas(__gm__ float* address, float compare, float val)
{
    return atomicCAS(address, compare, val);
}
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline int64_t asc_atomic_cas(__gm__ int64_t* address, int64_t compare, int64_t val)
{
    return atomicCAS(address, compare, val);
}
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline uint64_t asc_atomic_cas(__gm__ uint64_t* address, uint64_t compare, uint64_t val)
{
    return atomicCAS(address, compare, val);
}
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline int32_t asc_atomic_and(__gm__ int32_t* address, int32_t val) { return atomicAnd(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline uint32_t asc_atomic_and(__gm__ uint32_t* address, uint32_t val) { return atomicAnd(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline int64_t asc_atomic_and(__gm__ int64_t* address, int64_t val) { return atomicAnd(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline uint64_t asc_atomic_and(__gm__ uint64_t* address, uint64_t val) { return atomicAnd(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline int32_t asc_atomic_or(__gm__ int32_t* address, int32_t val) { return atomicOr(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline uint32_t asc_atomic_or(__gm__ uint32_t* address, uint32_t val) { return atomicOr(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline int64_t asc_atomic_or(__gm__ int64_t* address, int64_t val) { return atomicOr(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline uint64_t asc_atomic_or(__gm__ uint64_t* address, uint64_t val) { return atomicOr(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline int32_t asc_atomic_xor(__gm__ int32_t* address, int32_t val) { return atomicXOr(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline uint32_t asc_atomic_xor(__gm__ uint32_t* address, uint32_t val) { return atomicXOr(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline int64_t asc_atomic_xor(__gm__ int64_t* address, int64_t val) { return atomicXOr(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline uint64_t asc_atomic_xor(__gm__ uint64_t* address, uint64_t val) { return atomicXOr(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline uint32_t asc_atomic_inc(__gm__ uint32_t* address, uint32_t val) { return atomicInc(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline uint64_t asc_atomic_inc(__gm__ uint64_t* address, uint64_t val) { return atomicInc(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline uint32_t asc_atomic_dec(__gm__ uint32_t* address, uint32_t val) { return atomicDec(address, val); }
} // namespace __asc_aicore

namespace __asc_aicore {
__aicore__ inline uint64_t asc_atomic_dec(__gm__ uint64_t* address, uint64_t val) { return atomicDec(address, val); }
} // namespace __asc_aicore

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
