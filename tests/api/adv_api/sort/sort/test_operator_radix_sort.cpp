/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <gtest/gtest.h>
#include "kernel_operator.h"
#include "kernel_utils.h"
#include "sort/sort.h"

namespace AscendC {
template <typename T, typename U, SortType sortType, bool descend, bool isReuseSource, bool extraBuf, int mode = 0>
class KernelRadixSort {
public:
    __aicore__ inline KernelRadixSort() {}
    __aicore__ inline void Init(
        GM_ADDR srcGm, GM_ADDR srcIndexGm, GM_ADDR dstGm, GM_ADDR dstIndexGm, uint32_t shape, uint32_t cnt)
    {
        const uint32_t alginSize = 32 / sizeof(T);
        dataSize = (shape + alginSize - 1) / alginSize * alginSize;
        count = cnt;
        srcGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(srcGm), dataSize);
        srcIndexGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ U*>(srcIndexGm), dataSize);
        dstGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(dstGm), dataSize);
        dstIndexGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ U*>(dstIndexGm), dataSize);
        pipe.InitBuffer(inQueue, 1, dataSize * sizeof(T));
        pipe.InitBuffer(inIndexQueue, 1, dataSize * sizeof(U));
        pipe.InitBuffer(outQueue, 1, dataSize * sizeof(T));
        pipe.InitBuffer(outIndexQueue, 1, dataSize * sizeof(U));
        pipe.InitBuffer(tmpBuf, 30 * 1024);
    }

    __aicore__ inline void Process()
    {
        CopyIn();
        Compute();
        CopyOut();
    }

private:
    __aicore__ inline void CopyIn()
    {
        LocalTensor<T> srcLocal = inQueue.AllocTensor<T>();
        LocalTensor<U> srcIndexLocal = inIndexQueue.AllocTensor<U>();
        DataCopy(srcLocal, srcGlobal, dataSize);
        DataCopy(srcIndexLocal, srcIndexGlobal, dataSize);
        inQueue.EnQue(srcLocal);
        inIndexQueue.EnQue(srcIndexLocal);
    }

    __aicore__ inline void Compute()
    {
        LocalTensor<T> dstLocal = outQueue.AllocTensor<T>();
        LocalTensor<U> dstIndexLocal = outIndexQueue.AllocTensor<U>();
        LocalTensor<T> srcLocal = inQueue.DeQue<T>();
        LocalTensor<U> srcIndexLocal = inIndexQueue.DeQue<U>();
        LocalTensor<uint8_t> tmp = tmpBuf.Get<uint8_t>();

        static constexpr AscendC::SortConfig config = {sortType, descend};

        if constexpr (extraBuf) {
            if constexpr (mode == 2) {
                Sort<T, U, isReuseSource, config>(dstLocal, dstIndexLocal, srcLocal, srcIndexLocal, tmp, count);
            } else if constexpr (mode == 0) {
                Sort<T, isReuseSource, config>(dstLocal, dstIndexLocal, srcLocal, tmp, count);
            } else {
                Sort<T, isReuseSource, config>(dstLocal, srcLocal, tmp, count);
            }
        } else {
            if constexpr (mode == 2) {
                Sort<T, U, isReuseSource, config>(dstLocal, dstIndexLocal, srcLocal, srcIndexLocal, count);
            } else if constexpr (mode == 0) {
                Sort<T, isReuseSource, config>(dstLocal, dstIndexLocal, srcLocal, count);
            } else {
                Sort<T, isReuseSource, config>(dstLocal, srcLocal, count);
            }
        }

        outQueue.EnQue(dstLocal);
        outIndexQueue.EnQue(dstIndexLocal);
        inQueue.FreeTensor(srcLocal);
        inIndexQueue.FreeTensor(srcIndexLocal);
    }

    __aicore__ inline void CopyOut()
    {
        LocalTensor<T> dstLocal = outQueue.DeQue<T>();
        LocalTensor<U> dstIndexLocal = outIndexQueue.DeQue<U>();
        int32_t remainVal = count % (32 / sizeof(T));
        int32_t remainIdx = count % (32 / sizeof(U));
        if (remainVal != count) {
            DataCopy(dstGlobal, dstLocal, count - remainVal);
        }
        if (remainIdx != count) {
            DataCopy(dstIndexGlobal, dstIndexLocal, count - remainIdx);
        }
        AscendC::PipeBarrier<PIPE_ALL>();
        for (int32_t i = count - remainVal; i < count; i++) {
            dstGlobal.SetValue(i, dstLocal.GetValue(i));
        }
        for (int32_t i = count - remainIdx; i < count; i++) {
            dstIndexGlobal.SetValue(i, dstIndexLocal.GetValue(i));
        }
        outQueue.FreeTensor(dstLocal);
        outIndexQueue.FreeTensor(dstIndexLocal);
    }

private:
    GlobalTensor<T> srcGlobal;
    GlobalTensor<U> srcIndexGlobal;
    GlobalTensor<T> dstGlobal;
    GlobalTensor<U> dstIndexGlobal;

    TPipe pipe;
    TQue<TPosition::VECIN, 1> inQueue;
    TQue<TPosition::VECIN, 1> inIndexQueue;
    TQue<TPosition::VECOUT, 1> outQueue;
    TQue<TPosition::VECOUT, 1> outIndexQueue;
    TBuf<TPosition::VECIN> tmpBuf;
    uint32_t dataSize;
    uint32_t count;
};
} // namespace AscendC

template <typename T, typename U, AscendC::SortType sortType, bool descend, bool isReuseSource, bool extraBuf, int mode>
__global__ __aicore__ void testRadixSort(
    GM_ADDR dst, GM_ADDR dstIndex, GM_ADDR src, GM_ADDR srcIndex, uint32_t shape, uint32_t count)
{
    AscendC::KernelRadixSort<T, U, sortType, descend, isReuseSource, extraBuf, mode> op;
    op.Init(src, srcIndex, dst, dstIndex, shape, count);
    op.Process();
}

struct SortTestParams {
    uint32_t dataTypeSize;
    uint32_t indexTypeSize;
    uint32_t shape;
    uint32_t count;
    void (*calFunc)(GM_ADDR, GM_ADDR, GM_ADDR, GM_ADDR, uint32_t, uint32_t);
};

class AdvanceSortTestSuite : public testing::Test, public testing::WithParamInterface<SortTestParams> {
protected:
    void SetUp() { AscendC::SetGCoreType(2); }
    void TearDown() { AscendC::SetGCoreType(0); }
};

INSTANTIATE_TEST_CASE_P(
    TEST_OPEARATION_ADVANCE_API_SORT, AdvanceSortTestSuite,
    ::testing::Values(
        SortTestParams{
            1, 4, 1024, 1024, testRadixSort<int8_t, uint32_t, AscendC::SortType::RADIX_SORT, false, false, true, 0>},
        SortTestParams{
            2, 4, 1024, 1024, testRadixSort<uint16_t, uint32_t, AscendC::SortType::RADIX_SORT, false, false, true, 0>},
        SortTestParams{
            2, 4, 512, 356, testRadixSort<int16_t, uint32_t, AscendC::SortType::RADIX_SORT, false, false, true, 0>},
        SortTestParams{
            4, 4, 1024, 1024, testRadixSort<int32_t, uint32_t, AscendC::SortType::RADIX_SORT, true, false, true, 0>},
        SortTestParams{
            1, 4, 1024, 1024, testRadixSort<uint8_t, uint32_t, AscendC::SortType::RADIX_SORT, true, false, true, 0>},
        SortTestParams{
            4, 4, 1024, 1024, testRadixSort<uint32_t, uint32_t, AscendC::SortType::RADIX_SORT, true, false, true, 0>},
        SortTestParams{
            4, 4, 1024, 1024, testRadixSort<float, uint32_t, AscendC::SortType::RADIX_SORT, true, false, true, 0>},
        SortTestParams{
            2, 4, 1024, 1024, testRadixSort<half, uint32_t, AscendC::SortType::RADIX_SORT, false, true, true, 0>},
        SortTestParams{
            1, 4, 1024, 1024, testRadixSort<int8_t, uint32_t, AscendC::SortType::RADIX_SORT, false, false, true, 1>},
        SortTestParams{
            2, 4, 1024, 1024, testRadixSort<uint16_t, uint32_t, AscendC::SortType::RADIX_SORT, false, false, true, 1>},
        SortTestParams{
            2, 4, 512, 356, testRadixSort<int16_t, uint32_t, AscendC::SortType::RADIX_SORT, false, false, true, 1>},
        SortTestParams{
            4, 4, 1024, 1024, testRadixSort<int32_t, uint32_t, AscendC::SortType::RADIX_SORT, true, false, true, 1>},
        SortTestParams{
            4, 4, 1024, 1024, testRadixSort<uint32_t, uint32_t, AscendC::SortType::RADIX_SORT, true, false, true, 1>},
        SortTestParams{
            4, 4, 1024, 1024, testRadixSort<float, uint32_t, AscendC::SortType::RADIX_SORT, true, false, true, 1>},
        SortTestParams{
            1, 4, 1024, 1024, testRadixSort<int8_t, uint32_t, AscendC::SortType::RADIX_SORT, true, false, true, 1>},
        SortTestParams{
            2, 4, 1024, 1024, testRadixSort<half, uint32_t, AscendC::SortType::RADIX_SORT, false, true, true, 1>},
        SortTestParams{
            2, 4, 1024, 1024, testRadixSort<uint16_t, int32_t, AscendC::SortType::RADIX_SORT, false, false, true, 2>},
        SortTestParams{
            2, 4, 512, 356, testRadixSort<int16_t, uint32_t, AscendC::SortType::RADIX_SORT, false, false, false, 2>},
        SortTestParams{
            4, 4, 1024, 1024, testRadixSort<int32_t, int32_t, AscendC::SortType::RADIX_SORT, true, false, false, 2>},
        SortTestParams{
            1, 4, 512, 512, testRadixSort<int8_t, uint32_t, AscendC::SortType::RADIX_SORT, false, false, false, 2>}
#if !(defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)) // 用例超出kirinx90 9030 ub size
        ,
        SortTestParams{
            8, 4, 1024, 1024, testRadixSort<int64_t, uint32_t, AscendC::SortType::RADIX_SORT, true, false, true, 0>},
        SortTestParams{
            8, 4, 512, 356, testRadixSort<uint64_t, uint32_t, AscendC::SortType::RADIX_SORT, false, true, true, 0>},
        SortTestParams{
            2, 4, 512, 356, testRadixSort<bfloat16_t, uint32_t, AscendC::SortType::RADIX_SORT, false, false, false, 0>},
        SortTestParams{
            8, 4, 1024, 1024, testRadixSort<int64_t, uint32_t, AscendC::SortType::RADIX_SORT, true, false, true, 1>},
        SortTestParams{
            2, 4, 512, 356, testRadixSort<bfloat16_t, uint32_t, AscendC::SortType::RADIX_SORT, false, false, false, 1>},
        SortTestParams{
            8, 4, 512, 356, testRadixSort<uint64_t, uint32_t, AscendC::SortType::RADIX_SORT, false, false, false, 1>},
        SortTestParams{
            4, 8, 1024, 1024, testRadixSort<uint32_t, uint64_t, AscendC::SortType::RADIX_SORT, true, false, true, 2>},
        SortTestParams{
            4, 8, 1024, 1024, testRadixSort<float, int64_t, AscendC::SortType::RADIX_SORT, true, false, true, 2>},
        SortTestParams{
            8, 4, 1024, 1024, testRadixSort<uint64_t, int32_t, AscendC::SortType::RADIX_SORT, true, false, false, 2>},
        SortTestParams{
            1, 8, 1024, 1024, testRadixSort<uint8_t, uint64_t, AscendC::SortType::RADIX_SORT, true, false, true, 2>},
        SortTestParams{
            8, 8, 1024, 1024, testRadixSort<int64_t, int64_t, AscendC::SortType::RADIX_SORT, true, false, true, 2>}
#endif
        ));

TEST_P(AdvanceSortTestSuite, testRadixSort)
{
    auto param = GetParam();
    uint32_t dataTypeSize = param.dataTypeSize;
    uint32_t indexTypeSize = param.indexTypeSize;
    uint32_t shape = param.shape;
    uint32_t count = param.count;
    uint8_t srcGm[dataTypeSize * shape] = {0};
    uint8_t srcIndexGm[indexTypeSize * shape] = {0};
    uint8_t dstGm[dataTypeSize * shape] = {0};
    uint8_t dstIndexGm[indexTypeSize * shape] = {0};
    param.calFunc(dstGm, dstIndexGm, srcGm, srcIndexGm, shape, count);
    for (int32_t i = 0; i < param.count; i++) {
        EXPECT_EQ(dstGm[i], 0x00);
        EXPECT_EQ(dstIndexGm[i], 0x00);
    }
}
