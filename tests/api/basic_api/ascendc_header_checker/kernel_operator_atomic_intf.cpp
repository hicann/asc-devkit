/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef VERIFY_SINGLE_HEADER
#include "kernel_operator.h"
#else
#include "kernel_operator_atomic_intf.h"
#endif

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510))

// __aicore__ inline T AtomicAdd(__gm__ T *address, T value);
extern "C" __global__ __aicore__ void KernelTestAtomicAdd1()
{
    __gm__ int32_t* address;
    int32_t value = 0;
    AscendC::AtomicAdd<int32_t>(address, value);
}

// __aicore__ inline T AtomicMax(__gm__ T *address, T value);
extern "C" __global__ __aicore__ void KernelTestAtomicMax1()
{
    __gm__ int32_t* address;
    int32_t value = 0;
    AscendC::AtomicMax<int32_t>(address, value);
}

// __aicore__ inline T AtomicMin(__gm__ T *address, T value);
extern "C" __global__ __aicore__ void KernelTestAtomicMin1()
{
    __gm__ int32_t* address;
    int32_t value = 0;
    AscendC::AtomicMin<int32_t>(address, value);
}

// __aicore__ inline T AtomicCas(__gm__ T *address, T value1, T value2);
extern "C" __global__ __aicore__ void KernelTestAtomicCas1()
{
    __gm__ uint32_t* address;
    uint32_t value1 = 0;
    uint32_t value2 = 0;
    AscendC::AtomicCas<uint32_t>(address, value1, value2);
}

// __aicore__ inline T AtomicExch(__gm__ T *address, T value);
extern "C" __global__ __aicore__ void KernelTestAtomicExch1()
{
    __gm__ uint32_t* address;
    uint32_t value = 0;
    AscendC::AtomicExch<uint32_t>(address, value);
}

#endif
