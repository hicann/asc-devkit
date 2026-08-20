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
 * \file device_atomic_functions_debug.cpp
 * \brief
 */
#if defined(ASCENDC_CPU_DEBUG)
#include <bitset>
#include <string>
#include "kernel_utils.h"
#include "kernel_simt_cpu.h"
#include "stub_def.h"
#include "kernel_fp16.h"
#include "kernel_bf16.h"

template <typename T>
T AtomicAddCPU(__gm__ T* address, T val)
{
    T ret = *address;
    *address = *address + val;
    return ret;
}

template <typename T>
T AtomicSubCPU(__gm__ T* address, T val)
{
    T ret = *address;
    *address = *address - val;
    return ret;
}

template <typename T>
T AtomicExchCPU(__gm__ T* address, T val)
{
    T ret = *address;
    *address = val;
    return ret;
}

template <typename T>
T AtomicMaxCPU(__gm__ T* address, T val)
{
    T ret = *address;
    *address = (*address > val) ? *address : val;
    return ret;
}

template <typename T>
T AtomicMinCPU(__gm__ T* address, T val)
{
    T ret = *address;
    *address = (*address < val) ? *address : val;
    return ret;
}

template <typename T>
T AtomicIncCPU(__gm__ T* address, T val)
{
    T ret = *address;
    *address = (*address >= val) ? 0 : *address + 1;
    return ret;
}

template <typename T>
T AtomicDecCPU(__gm__ T* address, T val)
{
    T ret = *address;
    *address = (*address == 0 || *address > val) ? val : *address - 1;
    return ret;
}

template <typename T>
T AtomicCasCPU(__gm__ T* address, T compare, T val)
{
    T ret = *address;
    *address = (*address == compare) ? val : *address;
    return ret;
}

template <typename T>
T AtomicAndCPU(__gm__ T* address, T val)
{
    T ret = *address;
    *address = *address & val;
    return ret;
}

template <typename T>
T AtomicOrCPU(__gm__ T* address, T val)
{
    T ret = *address;
    *address = *address | val;
    return ret;
}

template <typename T>
T AtomicXorCPU(__gm__ T* address, T val)
{
    T ret = *address;
    *address = *address ^ val;
    return ret;
}

#define REGISTER_SIMT_ATOMIC_ADD(INTRI, TYPE) \
    TYPE INTRI(__gm__ TYPE* dst, TYPE val) { return AtomicAddCPU<TYPE>(dst, val); }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
REGISTER_SIMT_ATOMIC_ADD(atomicAdd, uint32_t);
REGISTER_SIMT_ATOMIC_ADD(atomicAdd, int32_t);
REGISTER_SIMT_ATOMIC_ADD(atomicAdd, uint64_t);
REGISTER_SIMT_ATOMIC_ADD(atomicAdd, int64_t);
REGISTER_SIMT_ATOMIC_ADD(atomicAdd, float);
REGISTER_SIMT_ATOMIC_ADD(atomicAdd, half);
REGISTER_SIMT_ATOMIC_ADD(atomicAdd, bfloat16_t);
#endif

#define REGISTER_SIMT_ATOMIC_SUB(INTRI, TYPE) \
    TYPE INTRI(__gm__ TYPE* dst, TYPE val) { return AtomicSubCPU<TYPE>(dst, val); }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
REGISTER_SIMT_ATOMIC_SUB(atomicSub, uint32_t);
REGISTER_SIMT_ATOMIC_SUB(atomicSub, int32_t);
REGISTER_SIMT_ATOMIC_SUB(atomicSub, uint64_t);
REGISTER_SIMT_ATOMIC_SUB(atomicSub, int64_t);
REGISTER_SIMT_ATOMIC_SUB(atomicSub, float);
#endif

#define REGISTER_SIMT_ATOMIC_EXCH(INTRI, TYPE) \
    TYPE INTRI(__gm__ TYPE* dst, TYPE val) { return AtomicExchCPU<TYPE>(dst, val); }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
REGISTER_SIMT_ATOMIC_EXCH(atomicExch, uint32_t);
REGISTER_SIMT_ATOMIC_EXCH(atomicExch, int32_t);
REGISTER_SIMT_ATOMIC_EXCH(atomicExch, uint64_t);
REGISTER_SIMT_ATOMIC_EXCH(atomicExch, int64_t);
REGISTER_SIMT_ATOMIC_EXCH(atomicExch, float);
#endif

#define REGISTER_SIMT_ATOMIC_MAX(INTRI, TYPE) \
    TYPE INTRI(__gm__ TYPE* dst, TYPE val) { return AtomicMaxCPU<TYPE>(dst, val); }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
REGISTER_SIMT_ATOMIC_MAX(atomicMax, uint32_t);
REGISTER_SIMT_ATOMIC_MAX(atomicMax, int32_t);
REGISTER_SIMT_ATOMIC_MAX(atomicMax, uint64_t);
REGISTER_SIMT_ATOMIC_MAX(atomicMax, int64_t);
REGISTER_SIMT_ATOMIC_MAX(atomicMax, float);
#endif

#define REGISTER_SIMT_ATOMIC_MIN(INTRI, TYPE) \
    TYPE INTRI(__gm__ TYPE* dst, TYPE val) { return AtomicMinCPU<TYPE>(dst, val); }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
REGISTER_SIMT_ATOMIC_MIN(atomicMin, uint32_t);
REGISTER_SIMT_ATOMIC_MIN(atomicMin, int32_t);
REGISTER_SIMT_ATOMIC_MIN(atomicMin, uint64_t);
REGISTER_SIMT_ATOMIC_MIN(atomicMin, int64_t);
REGISTER_SIMT_ATOMIC_MIN(atomicMin, float);
#endif

#define REGISTER_SIMT_ATOMIC_INC(INTRI, TYPE) \
    TYPE INTRI(__gm__ TYPE* dst, TYPE val) { return AtomicIncCPU<TYPE>(dst, val); }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
REGISTER_SIMT_ATOMIC_INC(atomicInc, uint32_t);
REGISTER_SIMT_ATOMIC_INC(atomicInc, uint64_t);
#endif

#define REGISTER_SIMT_ATOMIC_DEC(INTRI, TYPE) \
    TYPE INTRI(__gm__ TYPE* dst, TYPE val) { return AtomicDecCPU<TYPE>(dst, val); }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
REGISTER_SIMT_ATOMIC_DEC(atomicDec, uint32_t);
REGISTER_SIMT_ATOMIC_DEC(atomicDec, uint64_t);
#endif

#define REGISTER_SIMT_ATOMIC_CAS(INTRI, TYPE) \
    TYPE INTRI(__gm__ TYPE* dst, TYPE val1, TYPE val2) { return AtomicCasCPU<TYPE>(dst, val1, val2); }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
REGISTER_SIMT_ATOMIC_CAS(atomicCAS, uint32_t);
REGISTER_SIMT_ATOMIC_CAS(atomicCAS, int32_t);
REGISTER_SIMT_ATOMIC_CAS(atomicCAS, uint64_t);
REGISTER_SIMT_ATOMIC_CAS(atomicCAS, int64_t);
REGISTER_SIMT_ATOMIC_CAS(atomicCAS, float);
#endif

#define REGISTER_SIMT_ATOMIC_AND(INTRI, TYPE) \
    TYPE INTRI(__gm__ TYPE* dst, TYPE val) { return AtomicAndCPU<TYPE>(dst, val); }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
REGISTER_SIMT_ATOMIC_AND(atomicAnd, uint32_t);
REGISTER_SIMT_ATOMIC_AND(atomicAnd, int32_t);
REGISTER_SIMT_ATOMIC_AND(atomicAnd, uint64_t);
REGISTER_SIMT_ATOMIC_AND(atomicAnd, int64_t);
#endif

#define REGISTER_SIMT_ATOMIC_OR(INTRI, TYPE) \
    TYPE INTRI(__gm__ TYPE* dst, TYPE val) { return AtomicOrCPU<TYPE>(dst, val); }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
REGISTER_SIMT_ATOMIC_OR(atomicOr, uint32_t);
REGISTER_SIMT_ATOMIC_OR(atomicOr, int32_t);
REGISTER_SIMT_ATOMIC_OR(atomicOr, uint64_t);
REGISTER_SIMT_ATOMIC_OR(atomicOr, int64_t);
#endif

#define REGISTER_SIMT_ATOMIC_XOR(INTRI, TYPE) \
    TYPE INTRI(__gm__ TYPE* dst, TYPE val) { return AtomicXorCPU<TYPE>(dst, val); }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
REGISTER_SIMT_ATOMIC_XOR(atomicXOr, uint32_t);
REGISTER_SIMT_ATOMIC_XOR(atomicXOr, int32_t);
REGISTER_SIMT_ATOMIC_XOR(atomicXOr, uint64_t);
REGISTER_SIMT_ATOMIC_XOR(atomicXOr, int64_t);
#endif

template <typename T>
T AtomicAddVecCPU(__gm__ T* address, T val)
{
    T ret = *address;
    return ret;
}

#define REGISTER_ATOMIC_ADD_VEC(INTRI, TYPE) \
    TYPE INTRI(__gm__ TYPE* dst, TYPE val) { return AtomicAddVecCPU<TYPE>(dst, val); }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
REGISTER_ATOMIC_ADD_VEC(atomicAdd, half2);
REGISTER_ATOMIC_ADD_VEC(atomicAdd, bfloat16x2_t);
#endif

template <typename T>
T AtomicSubVecCPU(__gm__ T* address, T val)
{
    T ret = *address;
    return ret;
}

#define REGISTER_ATOMIC_SUB_VEC(INTRI, TYPE) \
    TYPE INTRI(__gm__ TYPE* dst, TYPE val) { return AtomicSubVecCPU<TYPE>(dst, val); }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
REGISTER_ATOMIC_SUB_VEC(atomicSub, half2);
REGISTER_ATOMIC_SUB_VEC(atomicSub, bfloat16x2_t);
#endif

template <typename T>
T AtomicExchVecCPU(__gm__ T* address, T val)
{
    T ret = *address;
    return ret;
}

#define REGISTER_ATOMIC_EXCH_VEC(INTRI, TYPE) \
    TYPE INTRI(__gm__ TYPE* dst, TYPE val) { return AtomicExchVecCPU<TYPE>(dst, val); }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
REGISTER_ATOMIC_EXCH_VEC(atomicExch, half2);
REGISTER_ATOMIC_EXCH_VEC(atomicExch, bfloat16x2_t);
#endif

template <typename T>
T AtomicMaxVecCPU(__gm__ T* address, T val)
{
    T ret = *address;
    return ret;
}

#define REGISTER_ATOMIC_MAX_VEC(INTRI, TYPE) \
    TYPE INTRI(__gm__ TYPE* dst, TYPE val) { return AtomicMaxVecCPU<TYPE>(dst, val); }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
REGISTER_ATOMIC_MAX_VEC(atomicMax, half2);
REGISTER_ATOMIC_MAX_VEC(atomicMax, bfloat16x2_t);
#endif

template <typename T>
T AtomicMinVecCPU(__gm__ T* address, T val)
{
    T ret = *address;
    return ret;
}

#define REGISTER_ATOMIC_MIN_VEC(INTRI, TYPE) \
    TYPE INTRI(__gm__ TYPE* dst, TYPE val) { return AtomicMinVecCPU<TYPE>(dst, val); }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
REGISTER_ATOMIC_MIN_VEC(atomicMin, half2);
REGISTER_ATOMIC_MIN_VEC(atomicMin, bfloat16x2_t);
#endif

template <typename T>
T AtomicCasVecCPU(__gm__ T* address, T compare, T val)
{
    T ret = *address;
    return ret;
}

#define REGISTER_SIMT_ATOMIC_CAS_VEC(INTRI, TYPE) \
    TYPE INTRI(__gm__ TYPE* dst, TYPE val1, TYPE val2) { return AtomicCasVecCPU<TYPE>(dst, val1, val2); }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
REGISTER_SIMT_ATOMIC_CAS_VEC(atomicCAS, half2);
REGISTER_SIMT_ATOMIC_CAS_VEC(atomicCAS, bfloat16x2_t);
#endif

#endif
