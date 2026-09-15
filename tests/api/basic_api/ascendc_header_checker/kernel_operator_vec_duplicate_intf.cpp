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
#include "kernel_operator_vec_duplicate_intf.h"
#endif

// __aicore__ inline void Duplicate(const LocalTensor<T>& dst, const T& scalarValue, uint64_t mask, const uint8_t
// repeatTime, const uint16_t dstBlockStride, const uint8_t dstRepeatStride);
extern "C" __global__ __aicore__ void KernelTestDuplicate1()
{
    AscendC::LocalTensor<float> dst;
    float scalarValue = 0.0f;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    uint16_t dstBlockStride = 1;
    uint8_t dstRepeatStride = 1;
    AscendC::Duplicate(dst, scalarValue, mask, repeatTime, dstBlockStride, dstRepeatStride);
}

// __aicore__ inline void Duplicate(const LocalTensor<T>& dst, const T& scalarValue, uint64_t mask[], const uint8_t
// repeatTime, const uint16_t dstBlockStride, const uint8_t dstRepeatStride);
extern "C" __global__ __aicore__ void KernelTestDuplicate2()
{
    AscendC::LocalTensor<float> dst;
    float scalarValue = 0.0f;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    uint16_t dstBlockStride = 1;
    uint8_t dstRepeatStride = 1;
    AscendC::Duplicate(dst, scalarValue, mask, repeatTime, dstBlockStride, dstRepeatStride);
}

// __aicore__ inline void Duplicate(const LocalTensor<T>& dst, const T& scalarValue, const int32_t& count);
extern "C" __global__ __aicore__ void KernelTestDuplicate3()
{
    AscendC::LocalTensor<float> dst;
    float scalarValue = 0.0f;
    int32_t count = 0;
    AscendC::Duplicate(dst, scalarValue, count);
}

#if (__NPU_ARCH__ == 3510)
// __aicore__ inline void Duplicate(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count);
extern "C" __global__ __aicore__ void KernelTestDuplicate4()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    int32_t count = 0;
    AscendC::Duplicate(dst, src, count);
}

// __aicore__ inline void Interleave(const LocalTensor<T>& dst0, const LocalTensor<T>& dst1, const LocalTensor<T>& src0,
// const LocalTensor<T>& src1, const int32_t count);
extern "C" __global__ __aicore__ void KernelTestInterleave1()
{
    AscendC::LocalTensor<float> dst0;
    AscendC::LocalTensor<float> dst1;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    int32_t count = 0;
    AscendC::Interleave(dst0, dst1, src0, src1, count);
}

// __aicore__ inline void DeInterleave(const LocalTensor<T>& dst0, const LocalTensor<T>& dst1, const LocalTensor<T>&
// src0, const LocalTensor<T>& src1, const int32_t count);
extern "C" __global__ __aicore__ void KernelTestDeInterleave1()
{
    AscendC::LocalTensor<float> dst0;
    AscendC::LocalTensor<float> dst1;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    int32_t count = 0;
    AscendC::DeInterleave(dst0, dst1, src0, src1, count);
}

// __aicore__ inline void DeInterleave(const LocalTensor<T>& dst0, const LocalTensor<T>& dst1, const LocalTensor<T>&
// src, const int32_t srcCount);
extern "C" __global__ __aicore__ void KernelTestDeInterleave2()
{
    AscendC::LocalTensor<float> dst0;
    AscendC::LocalTensor<float> dst1;
    AscendC::LocalTensor<float> src;
    int32_t srcCount = 0;
    AscendC::DeInterleave(dst0, dst1, src, srcCount);
}
#endif
