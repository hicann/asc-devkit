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
#include "kernel_operator_vec_reduce_intf.h"
#endif

// __aicore__ inline void BlockReduceSum(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t repeatTime,
// const int32_t mask, const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride);
extern "C" __global__ __aicore__ void KernelTestBlockReduceSum1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    int32_t repeatTime = 0;
    int32_t mask = 0;
    int32_t dstRepStride = 0;
    int32_t srcBlkStride = 0;
    int32_t srcRepStride = 0;
    AscendC::BlockReduceSum(dst, src, repeatTime, mask, dstRepStride, srcBlkStride, srcRepStride);
}

// __aicore__ inline void BlockReduceMax(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t repeatTime,
// const int32_t mask, const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride);
extern "C" __global__ __aicore__ void KernelTestBlockReduceMax1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    int32_t repeatTime = 0;
    int32_t mask = 0;
    int32_t dstRepStride = 0;
    int32_t srcBlkStride = 0;
    int32_t srcRepStride = 0;
    AscendC::BlockReduceMax(dst, src, repeatTime, mask, dstRepStride, srcBlkStride, srcRepStride);
}

// __aicore__ inline void BlockReduceMin(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t repeatTime,
// const int32_t mask, const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride);
extern "C" __global__ __aicore__ void KernelTestBlockReduceMin1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    int32_t repeatTime = 0;
    int32_t mask = 0;
    int32_t dstRepStride = 0;
    int32_t srcBlkStride = 0;
    int32_t srcRepStride = 0;
    AscendC::BlockReduceMin(dst, src, repeatTime, mask, dstRepStride, srcBlkStride, srcRepStride);
}

// __aicore__ inline void PairReduceSum(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t repeatTime,
// const int32_t mask, const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride);
extern "C" __global__ __aicore__ void KernelTestPairReduceSum1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    int32_t repeatTime = 0;
    int32_t mask = 0;
    int32_t dstRepStride = 0;
    int32_t srcBlkStride = 0;
    int32_t srcRepStride = 0;
    AscendC::PairReduceSum(dst, src, repeatTime, mask, dstRepStride, srcBlkStride, srcRepStride);
}

// __aicore__ inline void BlockReduceSum(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t repeatTime,
// const uint64_t mask[], const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride);
extern "C" __global__ __aicore__ void KernelTestBlockReduceSum2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    int32_t repeatTime = 0;
    uint64_t mask[2] = {0};
    int32_t dstRepStride = 0;
    int32_t srcBlkStride = 0;
    int32_t srcRepStride = 0;
    AscendC::BlockReduceSum(dst, src, repeatTime, mask, dstRepStride, srcBlkStride, srcRepStride);
}

// __aicore__ inline void BlockReduceMax(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t repeatTime,
// const uint64_t mask[], const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride);
extern "C" __global__ __aicore__ void KernelTestBlockReduceMax2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    int32_t repeatTime = 0;
    uint64_t mask[2] = {0};
    int32_t dstRepStride = 0;
    int32_t srcBlkStride = 0;
    int32_t srcRepStride = 0;
    AscendC::BlockReduceMax(dst, src, repeatTime, mask, dstRepStride, srcBlkStride, srcRepStride);
}

// __aicore__ inline void BlockReduceMin(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t repeatTime,
// const uint64_t mask[], const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride);
extern "C" __global__ __aicore__ void KernelTestBlockReduceMin2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    int32_t repeatTime = 0;
    uint64_t mask[2] = {0};
    int32_t dstRepStride = 0;
    int32_t srcBlkStride = 0;
    int32_t srcRepStride = 0;
    AscendC::BlockReduceMin(dst, src, repeatTime, mask, dstRepStride, srcBlkStride, srcRepStride);
}

// __aicore__ inline void PairReduceSum(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t repeatTime,
// const uint64_t mask[], const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride);
extern "C" __global__ __aicore__ void KernelTestPairReduceSum2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    int32_t repeatTime = 0;
    uint64_t mask[2] = {0};
    int32_t dstRepStride = 0;
    int32_t srcBlkStride = 0;
    int32_t srcRepStride = 0;
    AscendC::PairReduceSum(dst, src, repeatTime, mask, dstRepStride, srcBlkStride, srcRepStride);
}

// __aicore__ inline void RepeatReduceSum(const LocalTensor<U>& dst, const LocalTensor<T>& src, const int32_t
// repeatTime, const int32_t mask, const int32_t dstBlkStride, const int32_t srcBlkStride, const int32_t dstRepStride,
// const int32_t srcRepStride);
extern "C" __global__ __aicore__ void KernelTestRepeatReduceSum1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    int32_t repeatTime = 0;
    int32_t mask = 0;
    int32_t dstBlkStride = 0;
    int32_t srcBlkStride = 0;
    int32_t dstRepStride = 0;
    int32_t srcRepStride = 0;
    AscendC::RepeatReduceSum(dst, src, repeatTime, mask, dstBlkStride, srcBlkStride, dstRepStride, srcRepStride);
}

// __aicore__ inline void RepeatReduceSum(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t
// repeatTime, const int32_t mask, const int32_t dstBlkStride, const int32_t srcBlkStride, const int32_t dstRepStride,
// const int32_t srcRepStride);
extern "C" __global__ __aicore__ void KernelTestRepeatReduceSum2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    int32_t repeatTime = 0;
    int32_t mask = 0;
    int32_t dstBlkStride = 0;
    int32_t srcBlkStride = 0;
    int32_t dstRepStride = 0;
    int32_t srcRepStride = 0;
    AscendC::RepeatReduceSum(dst, src, repeatTime, mask, dstBlkStride, srcBlkStride, dstRepStride, srcRepStride);
}

// __aicore__ inline void WholeReduceSum(const LocalTensor<U>& dst, const LocalTensor<T>& src, const uint64_t mask[],
// const int32_t repeatTime, const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride);
extern "C" __global__ __aicore__ void KernelTestWholeReduceSum1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask[2] = {0};
    int32_t repeatTime = 0;
    int32_t dstRepStride = 0;
    int32_t srcBlkStride = 0;
    int32_t srcRepStride = 0;
    AscendC::WholeReduceSum(dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride);
}

// __aicore__ inline void WholeReduceSum(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint64_t mask[],
// const int32_t repeatTime, const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride);
extern "C" __global__ __aicore__ void KernelTestWholeReduceSum2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask[2] = {0};
    int32_t repeatTime = 0;
    int32_t dstRepStride = 0;
    int32_t srcBlkStride = 0;
    int32_t srcRepStride = 0;
    AscendC::WholeReduceSum(dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride);
}

#if __NPU_ARCH__ != 1001 // c100 cpu has a bug
// __aicore__ inline void WholeReduceMax(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint64_t mask[],
// const int32_t repeatTime, const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride,
// ReduceOrder order = AscendC::ReduceOrder::ORDER_VALUE_INDEX);
extern "C" __global__ __aicore__ void KernelTestWholeReduceMax1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask[2] = {0};
    int32_t repeatTime = 0;
    int32_t dstRepStride = 0;
    int32_t srcBlkStride = 0;
    int32_t srcRepStride = 0;
    AscendC::WholeReduceMax(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, AscendC::ReduceOrder::ORDER_VALUE_INDEX);
}

// __aicore__ inline void WholeReduceMin(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint64_t mask[],
// const int32_t repeatTime, const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride,
// ReduceOrder order = AscendC::ReduceOrder::ORDER_VALUE_INDEX);
extern "C" __global__ __aicore__ void KernelTestWholeReduceMin1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask[2] = {0};
    int32_t repeatTime = 0;
    int32_t dstRepStride = 0;
    int32_t srcBlkStride = 0;
    int32_t srcRepStride = 0;
    AscendC::WholeReduceMin(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, AscendC::ReduceOrder::ORDER_VALUE_INDEX);
}
#endif

// __aicore__ inline void WholeReduceSum(const LocalTensor<U>& dst, const LocalTensor<T>& src, const int32_t mask, const
// int32_t repeatTime, const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride);
extern "C" __global__ __aicore__ void KernelTestWholeReduceSum3()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    int32_t mask = 0;
    int32_t repeatTime = 0;
    int32_t dstRepStride = 0;
    int32_t srcBlkStride = 0;
    int32_t srcRepStride = 0;
    AscendC::WholeReduceSum(dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride);
}

// __aicore__ inline void WholeReduceSum(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t mask, const
// int32_t repeatTime, const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride);
extern "C" __global__ __aicore__ void KernelTestWholeReduceSum4()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    int32_t mask = 0;
    int32_t repeatTime = 0;
    int32_t dstRepStride = 0;
    int32_t srcBlkStride = 0;
    int32_t srcRepStride = 0;
    AscendC::WholeReduceSum(dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride);
}

#if __NPU_ARCH__ != 1001 // c100 cpu has a bug
// __aicore__ inline void WholeReduceMax(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t mask, const
// int32_t repeatTime, const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, ReduceOrder
// order = AscendC::ReduceOrder::ORDER_VALUE_INDEX);
extern "C" __global__ __aicore__ void KernelTestWholeReduceMax2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    int32_t mask = 0;
    int32_t repeatTime = 0;
    int32_t dstRepStride = 0;
    int32_t srcBlkStride = 0;
    int32_t srcRepStride = 0;
    AscendC::WholeReduceMax(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, AscendC::ReduceOrder::ORDER_VALUE_INDEX);
}

// __aicore__ inline void WholeReduceMin(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t mask, const
// int32_t repeatTime, const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, ReduceOrder
// order = AscendC::ReduceOrder::ORDER_VALUE_INDEX);
extern "C" __global__ __aicore__ void KernelTestWholeReduceMin2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    int32_t mask = 0;
    int32_t repeatTime = 0;
    int32_t dstRepStride = 0;
    int32_t srcBlkStride = 0;
    int32_t srcRepStride = 0;
    AscendC::WholeReduceMin(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, AscendC::ReduceOrder::ORDER_VALUE_INDEX);
}

// __aicore__ inline void ReduceMax(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>&
// sharedTmpBuffer, const int32_t mask, const int32_t repeatTime, const int32_t srcRepStride, bool calIndex = 0);
extern "C" __global__ __aicore__ void KernelTestReduceMax1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::LocalTensor<float> sharedTmpBuffer;
    int32_t mask = 0;
    int32_t repeatTime = 0;
    int32_t srcRepStride = 0;
    AscendC::ReduceMax(dst, src, sharedTmpBuffer, mask, repeatTime, srcRepStride, 0);
}

// __aicore__ inline void ReduceMin(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>&
// sharedTmpBuffer, const int32_t mask, const int32_t repeatTime, const int32_t srcRepStride, bool calIndex = 0);
extern "C" __global__ __aicore__ void KernelTestReduceMin1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::LocalTensor<float> sharedTmpBuffer;
    int32_t mask = 0;
    int32_t repeatTime = 0;
    int32_t srcRepStride = 0;
    AscendC::ReduceMin(dst, src, sharedTmpBuffer, mask, repeatTime, srcRepStride, 0);
}

// __aicore__ inline void ReduceSum(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>&
// sharedTmpBuffer, const int32_t mask, const int32_t repeatTime, const int32_t srcRepStride);
extern "C" __global__ __aicore__ void KernelTestReduceSum1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::LocalTensor<float> sharedTmpBuffer;
    int32_t mask = 0;
    int32_t repeatTime = 0;
    int32_t srcRepStride = 0;
    AscendC::ReduceSum(dst, src, sharedTmpBuffer, mask, repeatTime, srcRepStride);
}

// __aicore__ inline void ReduceMax(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>&
// sharedTmpBuffer, const uint64_t mask[], const int32_t repeatTime, const int32_t srcRepStride, bool calIndex = 0);
extern "C" __global__ __aicore__ void KernelTestReduceMax2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::LocalTensor<float> sharedTmpBuffer;
    uint64_t mask[2] = {0};
    int32_t repeatTime = 0;
    int32_t srcRepStride = 0;
    AscendC::ReduceMax(dst, src, sharedTmpBuffer, mask, repeatTime, srcRepStride, 0);
}

// __aicore__ inline void ReduceMin(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>&
// sharedTmpBuffer, const uint64_t mask[], const int32_t repeatTime, const int32_t srcRepStride, bool calIndex = 0);
extern "C" __global__ __aicore__ void KernelTestReduceMin2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::LocalTensor<float> sharedTmpBuffer;
    uint64_t mask[2] = {0};
    int32_t repeatTime = 0;
    int32_t srcRepStride = 0;
    AscendC::ReduceMin(dst, src, sharedTmpBuffer, mask, repeatTime, srcRepStride, 0);
}

// __aicore__ inline void ReduceSum(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>&
// sharedTmpBuffer, const uint64_t mask[], const int32_t repeatTime, const int32_t srcRepStride);
extern "C" __global__ __aicore__ void KernelTestReduceSum2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::LocalTensor<float> sharedTmpBuffer;
    uint64_t mask[2] = {0};
    int32_t repeatTime = 0;
    int32_t srcRepStride = 0;
    AscendC::ReduceSum(dst, src, sharedTmpBuffer, mask, repeatTime, srcRepStride);
}

// __aicore__ inline void ReduceMin(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>&
// sharedTmpBuffer, const int32_t count, bool calIndex = 0);
extern "C" __global__ __aicore__ void KernelTestReduceMin3()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::LocalTensor<float> sharedTmpBuffer;
    int32_t count = 0;
    AscendC::ReduceMin(dst, src, sharedTmpBuffer, count, 0);
}

// __aicore__ inline void ReduceMax(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>&
// sharedTmpBuffer, const int32_t count, bool calIndex = 0);
extern "C" __global__ __aicore__ void KernelTestReduceMax3()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::LocalTensor<float> sharedTmpBuffer;
    int32_t count = 0;
    AscendC::ReduceMax(dst, src, sharedTmpBuffer, count, 0);
}

// __aicore__ inline void ReduceSum(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>&
// sharedTmpBuffer, const int32_t count);
extern "C" __global__ __aicore__ void KernelTestReduceSum3()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::LocalTensor<float> sharedTmpBuffer;
    int32_t count = 0;
    AscendC::ReduceSum(dst, src, sharedTmpBuffer, count);
}
#endif

// __aicore__ inline __inout_pipe__(S) void GetReduceMaxMinCount(T &maxMinValue, T &maxMinIndex);
extern "C" __global__ __aicore__ void KernelTestGetReduceMaxMinCount1()
{
    int32_t maxMinValue = 0;
    int32_t maxMinIndex = 0;
    AscendC::GetReduceMaxMinCount(maxMinValue, maxMinIndex);
}

// __aicore__ inline __inout_pipe__(S) void GetReduceMaxMinCount(T &maxMinValue);
extern "C" __global__ __aicore__ void KernelTestGetReduceMaxMinCount2()
{
    int32_t maxMinValue = 0;
    AscendC::GetReduceMaxMinCount(maxMinValue);
}
