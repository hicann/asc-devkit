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
#include "kernel_common.h"
#endif
#include "kernel_tensor.h" // todo

// __aicore__ static inline void SetVectorMask(const uint64_t maskHigh, const uint64_t maskLow)
extern "C" __global__ __aicore__ void KernelTestSetVectorMask1()
{
    uint64_t maskHigh = 0xFFFFFFFFFFFFFFFF;
    uint64_t maskLow = 0xFFFFFFFFFFFFFFFF;
    AscendC::SetVectorMask<uint32_t>(maskHigh, maskLow);
}

// __aicore__ static inline void SetVectorMask(int32_t len)
extern "C" __global__ __aicore__ void KernelTestSetVectorMask2()
{
    int32_t len = 0;
    AscendC::SetVectorMask<uint32_t>(len);
}

// __aicore__ inline void ResetMask()
extern "C" __global__ __aicore__ void KernelTestResetMask1() { AscendC::ResetMask(); }

// __aicore__ inline void SetMaskCount()
extern "C" __global__ __aicore__ void KernelTestSetMaskCount1() { AscendC::SetMaskCount(); }

// __aicore__ inline void SetMaskNorm()
extern "C" __global__ __aicore__ void KernelTestSetMaskNorm1() { AscendC::SetMaskNorm(); }

// __aicore__ inline void SetHcclContext(__gm__ uint8_t* context)
extern "C" __global__ __aicore__ void KernelTestSetHcclContext1()
{
    __gm__ uint8_t* context;
    AscendC::SetHcclContext<1>(context);
}

#if defined(__NPU_ARCH__) && \
    ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 2002) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3510))
// __aicore__ inline void SetAippFunctions(const GlobalTensor<T>& src0, AippInputFormat format, AippParams<U> config)
extern "C" __global__ __aicore__ void KernelTestSetAippFunctions1()
{
    AscendC::GlobalTensor<uint8_t> src0;
    AscendC::AippInputFormat format;
    AscendC::AippParams<uint8_t> config;
    AscendC::SetAippFunctions(src0, format, config);
}

// __aicore__ inline void SetAippFunctions(const GlobalTensor<T>& src0, const GlobalTensor<T>& src1, AippInputFormat
// format, AippParams<U> config)
extern "C" __global__ __aicore__ void KernelTestSetAippFunctions2()
{
    AscendC::GlobalTensor<uint8_t> src0;
    AscendC::GlobalTensor<uint8_t> src1;
    AscendC::AippInputFormat format;
    AscendC::AippParams<uint8_t> config;
    AscendC::SetAippFunctions(src0, src1, format, config);
}
#endif
