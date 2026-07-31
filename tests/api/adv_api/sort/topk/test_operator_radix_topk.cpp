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

namespace AscendC {
template <
    typename T, bool isInitIndex = false, bool isHasfinish = false, bool isReuseSrc = false,
    enum AscendC::TopKMode topkMode = AscendC::TopKMode::TOPK_NORMAL,
    AscendC::TopKOrder order = AscendC::TopKOrder::UNSET, bool sorted = false>
class KernelRadixTopK {
public:
    __aicore__ inline KernelRadixTopK() {}
    __aicore__ inline void Init(
        GM_ADDR srcGmValue, GM_ADDR srcGmIndex, GM_ADDR finishGm, GM_ADDR dstGmValue, GM_ADDR dstGmIndex, int32_t kGm,
        uint32_t outter, uint32_t inner, int32_t n, bool isLargestGm)
    {
        printf("outter_shape: %u, inner_shape: %u, k: %u\n", outter, inner, kGm);

        k = kGm;
        k_pad = AscendC::Reg::RadixSelectTopK::GetKPad<T>(k);
        kpad_index = AscendC::Reg::RadixSelectTopK::GetKPad<int32_t>(k);
        isLargest = isLargestGm;
        topKInfo.outter = outter;
        topKInfo.inner = inner;
        topKInfo.n = n;

        inValueDataSize = topKInfo.inner * topKInfo.outter;
        outValueDataSize = k_pad * topKInfo.outter;
        outIndexDataSize = kpad_index * topKInfo.outter;

        inIndexDataSize = topKInfo.inner;
        if (topkMode == AscendC::TopKMode::TOPK_NSMALL) {
            inIndexDataSize = inValueDataSize;
        }

        finishLocalBytes = topKInfo.outter * sizeof(bool);
        if (finishLocalBytes % 32 != 0) {
            finishLocalBytes = (finishLocalBytes + 31) / 32 * 32;
        }

        srcValueGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(srcGmValue), inValueDataSize);
        srcIndexGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ int32_t*>(srcGmIndex), inIndexDataSize);
        srcFinishGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ bool*>(finishGm), finishLocalBytes / sizeof(bool));
        dstValueGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(dstGmValue), outValueDataSize);
        dstIndexGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ int32_t*>(dstGmIndex), outIndexDataSize);

        uint32_t alignedCnt = (inner + 31) / 32 * 32;
        tmplocalBytes = 512 + 11 * alignedCnt;
        if constexpr (sizeof(T) == 8) {
            tmplocalBytes += 256 * sizeof(T);
        }

        uint32_t extraTypeSize = sizeof(T);
        uint32_t extraIndexTypeSize = sizeof(int32_t);
        tmplocalBytes += extraTypeSize * alignedCnt;

        tmplocalBytes += 512 + 11 * alignedCnt;
        tmplocalBytes += (4 + extraIndexTypeSize + extraTypeSize) * alignedCnt;

        pipe.InitBuffer(tmpBuf, tmplocalBytes);
        pipe.InitBuffer(inValueQueue, 1, inValueDataSize * sizeof(T));
        pipe.InitBuffer(inIndexQueue, 1, inIndexDataSize * sizeof(int32_t));
        pipe.InitBuffer(inFinishQueue, 1, finishLocalBytes);

        pipe.InitBuffer(outValueQueue, 1, outValueDataSize * sizeof(T));
        pipe.InitBuffer(outIndexQueue, 1, outIndexDataSize * sizeof(int32_t));
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
        LocalTensor<T> srcValueLocal = inValueQueue.AllocTensor<T>();
        LocalTensor<int32_t> srcIndexLocal = inIndexQueue.AllocTensor<int32_t>();
        LocalTensor<bool> srcFinishLocal = inFinishQueue.AllocTensor<bool>();

        DataCopy(srcValueLocal, srcValueGlobal, inValueDataSize);
        DataCopy(srcIndexLocal, srcIndexGlobal, inIndexDataSize);
        DataCopy(srcFinishLocal, srcFinishGlobal, finishLocalBytes / sizeof(bool));

        inValueQueue.EnQue(srcValueLocal);
        inIndexQueue.EnQue(srcIndexLocal);
        inFinishQueue.EnQue(srcFinishLocal);
    }

    __aicore__ inline void Compute()
    {
        LocalTensor<T> dstValueLocal = outValueQueue.AllocTensor<T>();
        LocalTensor<int32_t> dstIndexLocal = outIndexQueue.AllocTensor<int32_t>();

        LocalTensor<T> srcValueLocal = inValueQueue.DeQue<T>();
        LocalTensor<int32_t> srcIndexLocal = inIndexQueue.DeQue<int32_t>();
        LocalTensor<bool> srcFinishLocal = inFinishQueue.DeQue<bool>();
        LocalTensor<uint8_t> tmp = tmpBuf.Get<uint8_t>();

        TopkTiling tiling;

        static constexpr AscendC::TopKConfig config = {AscendC::TopKAlgo::RADIX_SELECT, order, sorted};

        AscendC::TopK<T, isInitIndex, false, isReuseSrc, topkMode, config>(
            dstValueLocal, dstIndexLocal, srcValueLocal, srcIndexLocal, srcFinishLocal, tmp, k, tiling, topKInfo,
            isLargest);

        outValueQueue.EnQue(dstValueLocal);
        outIndexQueue.EnQue(dstIndexLocal);
        inValueQueue.FreeTensor(srcValueLocal);
        inIndexQueue.FreeTensor(srcIndexLocal);
        inFinishQueue.FreeTensor(srcFinishLocal);
    }

    __aicore__ inline void CopyOut()
    {
        LocalTensor<T> dstValueLocal = outValueQueue.DeQue<T>();
        LocalTensor<int32_t> dstIndexLocal = outIndexQueue.DeQue<int32_t>();

        DataCopy(dstValueGlobal, dstValueLocal, outValueDataSize);
        uint32_t count = topKInfo.outter * k;
        int32_t remainIdx = count % (32 / sizeof(int32_t));
        if (remainIdx != count) {
            DataCopy(dstIndexGlobal, dstIndexLocal, count - remainIdx);
        }
        AscendC::PipeBarrier<PIPE_ALL>();
        for (int32_t i = count - remainIdx; i < count; i++) {
            dstIndexGlobal.SetValue(i, dstIndexLocal.GetValue(i));
        }
        outValueQueue.FreeTensor(dstValueLocal);
        outIndexQueue.FreeTensor(dstIndexLocal);
    }

private:
    GlobalTensor<T> srcValueGlobal;
    GlobalTensor<int32_t> srcIndexGlobal;
    GlobalTensor<bool> srcFinishGlobal;

    GlobalTensor<T> dstValueGlobal;
    GlobalTensor<int32_t> dstIndexGlobal;

    TPipe pipe;
    TQue<TPosition::VECIN, 1> inValueQueue;
    TQue<TPosition::VECIN, 1> inIndexQueue;
    TQue<TPosition::VECIN, 1> inFinishQueue;

    TQue<TPosition::VECOUT, 1> outValueQueue;
    TQue<TPosition::VECOUT, 1> outIndexQueue;

    TBuf<TPosition::VECIN> tmpBuf;
    uint32_t tmplocalBytes = 0;

    uint32_t inValueDataSize = 0;
    uint32_t inIndexDataSize = 0;
    uint32_t finishLocalBytes;
    uint32_t outValueDataSize = 0;
    uint32_t outIndexDataSize = 0;
    int32_t k;
    int32_t k_pad;
    int32_t kpad_index;
    bool isLargest = true;
    AscendC::TopKInfo topKInfo;
};
} // namespace AscendC

template <
    typename T, bool isInitIndex, bool isHasfinish, bool isReuseSrc, AscendC::TopKMode topkMode,
    AscendC::TopKOrder order, bool sorted>
__aicore__ void testRadixTopK(
    GM_ADDR srcGmValue, GM_ADDR srcGmIndex, GM_ADDR finishGm, GM_ADDR dstGmValue, GM_ADDR dstGmIndex, int32_t kGm,
    AscendC::TopKInfo tkInfo, bool isLargest)
{
    AscendC::KernelRadixTopK<T, isInitIndex, isHasfinish, isReuseSrc, topkMode, order, sorted> op;
    op.Init(
        srcGmValue, srcGmIndex, finishGm, dstGmValue, dstGmIndex, kGm, tkInfo.outter, tkInfo.inner, tkInfo.n,
        isLargest);
    op.Process();
}

struct TopKTestParams {
    int32_t k;
    int32_t k_pad;
    AscendC::TopKInfo shapeInfo;
    uint32_t dataTypeSize;
    bool isLargest;
    void (*calFunc)(GM_ADDR, GM_ADDR, GM_ADDR, GM_ADDR, GM_ADDR, int32_t, AscendC::TopKInfo, bool);
};

class AdvanceTopKTestSuite : public testing::Test, public testing::WithParamInterface<TopKTestParams> {
protected:
    void SetUp() { AscendC::SetGCoreType(2); }
    void TearDown() { AscendC::SetGCoreType(0); }
};

template <typename T>
inline int32_t GetKPad(int32_t k)
{
    if constexpr (sizeof(T) == 1) {
        return (k + 31) / 32 * 32;
    }

    if constexpr (sizeof(T) == 2) {
        return (k + 15) / 16 * 16;
    }

    if constexpr (sizeof(T) == 4) {
        return (k + 7) / 8 * 8;
    }

    if constexpr (sizeof(T) == 8) {
        return (k + 3) / 4 * 4;
    }

    return k;
}

INSTANTIATE_TEST_CASE_P(
    TEST_OPEARATION_ADVANCE_API_TOPK, AdvanceTopKTestSuite,
    ::testing::Values(
        // uint8_t
        TopKTestParams{
            1,
            GetKPad<uint8_t>(1),
            {1, 32, 32},
            sizeof(uint8_t),
            true,
            testRadixTopK<
                uint8_t, false, false, false, AscendC::TopKMode::TOPK_NSMALL, AscendC::TopKOrder::UNSET, false>},
        TopKTestParams{
            2,
            GetKPad<uint8_t>(2),
            {2, 32, 32},
            sizeof(uint8_t),
            true,
            testRadixTopK<
                uint8_t, true, false, false, AscendC::TopKMode::TOPK_NSMALL, AscendC::TopKOrder::UNSET, false>},
        TopKTestParams{
            20,
            GetKPad<uint8_t>(20),
            {1, 128, 128},
            sizeof(uint8_t),
            true,
            testRadixTopK<
                uint8_t, false, true, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::LARGEST, false>},
        TopKTestParams{
            2,
            GetKPad<uint8_t>(2),
            {2, 256, 256},
            sizeof(uint8_t),
            true,
            testRadixTopK<
                uint8_t, true, true, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::LARGEST, false>},
        TopKTestParams{
            20,
            GetKPad<uint8_t>(20),
            {2, 512, 512},
            sizeof(uint8_t),
            true,
            testRadixTopK<
                uint8_t, false, false, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::SMALLEST, false>},
        TopKTestParams{
            2,
            GetKPad<uint8_t>(2),
            {2, 1024, 1024},
            sizeof(uint8_t),
            true,
            testRadixTopK<
                uint8_t, true, false, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::SMALLEST, false>},
        // int8_t
        TopKTestParams{
            1,
            GetKPad<int8_t>(1),
            {1, 32, 32},
            sizeof(int8_t),
            true,
            testRadixTopK<
                int8_t, false, false, false, AscendC::TopKMode::TOPK_NSMALL, AscendC::TopKOrder::UNSET, false>},
        TopKTestParams{
            2,
            GetKPad<int8_t>(2),
            {2, 32, 32},
            sizeof(int8_t),
            true,
            testRadixTopK<
                int8_t, true, false, false, AscendC::TopKMode::TOPK_NSMALL, AscendC::TopKOrder::UNSET, false>},
        TopKTestParams{
            20,
            GetKPad<int8_t>(20),
            {1, 128, 128},
            sizeof(int8_t),
            true,
            testRadixTopK<
                int8_t, false, true, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::LARGEST, false>},
        TopKTestParams{
            255,
            GetKPad<int8_t>(255),
            {2, 256, 256},
            sizeof(int8_t),
            true,
            testRadixTopK<
                int8_t, true, true, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::LARGEST, false>},
        // uint16_t
        TopKTestParams{
            1,
            GetKPad<uint16_t>(1),
            {1, 32, 32},
            sizeof(uint16_t),
            true,
            testRadixTopK<
                uint16_t, false, false, false, AscendC::TopKMode::TOPK_NSMALL, AscendC::TopKOrder::UNSET, false>},
        TopKTestParams{
            2,
            GetKPad<uint16_t>(2),
            {2, 32, 32},
            sizeof(uint16_t),
            true,
            testRadixTopK<
                uint16_t, true, false, false, AscendC::TopKMode::TOPK_NSMALL, AscendC::TopKOrder::UNSET, false>},
        TopKTestParams{
            20,
            GetKPad<uint16_t>(20),
            {1, 128, 128},
            sizeof(uint16_t),
            true,
            testRadixTopK<
                uint16_t, false, true, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::LARGEST, false>},
        TopKTestParams{
            2,
            GetKPad<uint16_t>(2),
            {2, 256, 256},
            sizeof(uint16_t),
            true,
            testRadixTopK<
                uint16_t, true, true, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::LARGEST, false>},
        TopKTestParams{
            20,
            GetKPad<uint16_t>(20),
            {2, 512, 512},
            sizeof(uint16_t),
            true,
            testRadixTopK<
                uint16_t, false, false, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::SMALLEST, false>},
        TopKTestParams{
            2,
            GetKPad<uint16_t>(2),
            {2, 1024, 1024},
            sizeof(uint16_t),
            true,
            testRadixTopK<
                uint16_t, true, false, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::SMALLEST, false>},
        // int16_t
        TopKTestParams{
            1,
            GetKPad<int16_t>(1),
            {1, 32, 32},
            sizeof(int16_t),
            true,
            testRadixTopK<
                int16_t, false, false, false, AscendC::TopKMode::TOPK_NSMALL, AscendC::TopKOrder::UNSET, false>},
        TopKTestParams{
            2,
            GetKPad<int16_t>(2),
            {2, 32, 32},
            sizeof(int16_t),
            true,
            testRadixTopK<
                int16_t, true, false, false, AscendC::TopKMode::TOPK_NSMALL, AscendC::TopKOrder::UNSET, false>},
        TopKTestParams{
            20,
            GetKPad<int16_t>(20),
            {1, 128, 128},
            sizeof(int16_t),
            true,
            testRadixTopK<
                int16_t, false, true, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::LARGEST, false>},
        TopKTestParams{
            255,
            GetKPad<int16_t>(255),
            {2, 256, 256},
            sizeof(int16_t),
            true,
            testRadixTopK<
                int16_t, true, true, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::LARGEST, false>},
        // uint32_t
        TopKTestParams{
            1,
            GetKPad<uint32_t>(1),
            {1, 32, 32},
            sizeof(uint32_t),
            true,
            testRadixTopK<
                uint32_t, false, false, false, AscendC::TopKMode::TOPK_NSMALL, AscendC::TopKOrder::UNSET, false>},
        TopKTestParams{
            2,
            GetKPad<uint32_t>(2),
            {2, 32, 32},
            sizeof(uint32_t),
            true,
            testRadixTopK<
                uint32_t, true, false, false, AscendC::TopKMode::TOPK_NSMALL, AscendC::TopKOrder::UNSET, false>},
        TopKTestParams{
            20,
            GetKPad<uint32_t>(20),
            {1, 128, 128},
            sizeof(uint32_t),
            true,
            testRadixTopK<
                uint32_t, false, true, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::LARGEST, false>},
        TopKTestParams{
            2,
            GetKPad<uint32_t>(2),
            {2, 256, 256},
            sizeof(uint32_t),
            true,
            testRadixTopK<
                uint32_t, true, true, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::LARGEST, false>},
        TopKTestParams{
            20,
            GetKPad<uint32_t>(20),
            {2, 512, 512},
            sizeof(uint32_t),
            true,
            testRadixTopK<
                uint32_t, false, false, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::SMALLEST, false>},
        TopKTestParams{
            2,
            GetKPad<uint32_t>(2),
            {2, 1024, 1024},
            sizeof(uint32_t),
            true,
            testRadixTopK<
                uint32_t, true, false, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::SMALLEST, false>},
        // int32_t
        TopKTestParams{
            1,
            GetKPad<int32_t>(1),
            {1, 32, 32},
            sizeof(int32_t),
            true,
            testRadixTopK<
                int32_t, false, false, false, AscendC::TopKMode::TOPK_NSMALL, AscendC::TopKOrder::UNSET, false>},
        TopKTestParams{
            2,
            GetKPad<int32_t>(2),
            {2, 32, 32},
            sizeof(int32_t),
            true,
            testRadixTopK<
                int32_t, true, false, false, AscendC::TopKMode::TOPK_NSMALL, AscendC::TopKOrder::UNSET, false>},
        TopKTestParams{
            20,
            GetKPad<int32_t>(20),
            {1, 128, 128},
            sizeof(int32_t),
            true,
            testRadixTopK<
                int32_t, false, true, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::LARGEST, false>},
        TopKTestParams{
            255,
            GetKPad<int32_t>(255),
            {2, 256, 256},
            sizeof(int32_t),
            true,
            testRadixTopK<
                int32_t, true, true, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::LARGEST, false>},
        TopKTestParams{
            512,
            GetKPad<int32_t>(512),
            {1, 512, 512},
            sizeof(int32_t),
            true,
            testRadixTopK<
                int32_t, false, false, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::SMALLEST, false>},
        // half
        TopKTestParams{
            1,
            GetKPad<half>(1),
            {1, 32, 32},
            sizeof(half),
            true,
            testRadixTopK<half, false, false, false, AscendC::TopKMode::TOPK_NSMALL, AscendC::TopKOrder::UNSET, false>},
        TopKTestParams{
            2,
            GetKPad<half>(2),
            {2, 32, 32},
            sizeof(half),
            true,
            testRadixTopK<half, true, false, false, AscendC::TopKMode::TOPK_NSMALL, AscendC::TopKOrder::UNSET, false>},
        TopKTestParams{
            20,
            GetKPad<half>(20),
            {1, 128, 128},
            sizeof(half),
            true,
            testRadixTopK<
                half, false, true, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::LARGEST, false>},
        TopKTestParams{
            2,
            GetKPad<half>(2),
            {2, 256, 256},
            sizeof(half),
            true,
            testRadixTopK<half, true, true, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::LARGEST, false>},
        TopKTestParams{
            20,
            GetKPad<half>(20),
            {2, 512, 512},
            sizeof(half),
            true,
            testRadixTopK<
                half, false, false, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::SMALLEST, false>},
        TopKTestParams{
            2,
            GetKPad<half>(2),
            {2, 1024, 1024},
            sizeof(half),
            true,
            testRadixTopK<
                half, true, false, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::SMALLEST, false>},
        // float
        TopKTestParams{
            1,
            GetKPad<float>(1),
            {1, 32, 32},
            sizeof(float),
            true,
            testRadixTopK<
                float, false, false, false, AscendC::TopKMode::TOPK_NSMALL, AscendC::TopKOrder::UNSET, false>},
        TopKTestParams{
            2,
            GetKPad<float>(2),
            {2, 32, 32},
            sizeof(float),
            true,
            testRadixTopK<float, true, false, false, AscendC::TopKMode::TOPK_NSMALL, AscendC::TopKOrder::UNSET, false>},
        TopKTestParams{
            20,
            GetKPad<float>(20),
            {1, 128, 128},
            sizeof(float),
            true,
            testRadixTopK<
                float, false, true, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::LARGEST, false>},
        TopKTestParams{
            255,
            GetKPad<float>(255),
            {2, 256, 256},
            sizeof(float),
            true,
            testRadixTopK<float, true, true, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::LARGEST, false>}
#if !(                       \
    defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)) // 用例超出kirinx90 9030 ub size，bf16 kirinx90 9030不支持
        ,
        TopKTestParams{
            512,
            GetKPad<int16_t>(512),
            {1, 512, 512},
            sizeof(int16_t),
            true,
            testRadixTopK<
                int16_t, false, false, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::SMALLEST, false>},
        TopKTestParams{
            1,
            GetKPad<int16_t>(1),
            {2, 4096, 4096},
            sizeof(int16_t),
            true,
            testRadixTopK<
                int16_t, true, false, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::SMALLEST, false>},
        TopKTestParams{
            1,
            GetKPad<int32_t>(1),
            {2, 4096, 4096},
            sizeof(int32_t),
            true,
            testRadixTopK<
                int32_t, true, false, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::SMALLEST, false>},
        // uint64_t
        TopKTestParams{
            1,
            GetKPad<uint64_t>(1),
            {1, 32, 32},
            sizeof(uint64_t),
            true,
            testRadixTopK<
                uint64_t, false, false, false, AscendC::TopKMode::TOPK_NSMALL, AscendC::TopKOrder::UNSET, false>},
        TopKTestParams{
            2,
            GetKPad<uint64_t>(2),
            {2, 32, 32},
            sizeof(uint64_t),
            true,
            testRadixTopK<
                uint64_t, true, false, false, AscendC::TopKMode::TOPK_NSMALL, AscendC::TopKOrder::UNSET, false>},
        TopKTestParams{
            20,
            GetKPad<uint64_t>(20),
            {1, 128, 128},
            sizeof(uint64_t),
            true,
            testRadixTopK<
                uint64_t, false, true, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::LARGEST, false>},
        TopKTestParams{
            2,
            GetKPad<uint64_t>(2),
            {2, 256, 256},
            sizeof(uint64_t),
            true,
            testRadixTopK<
                uint64_t, true, true, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::LARGEST, false>},
        TopKTestParams{
            20,
            GetKPad<uint64_t>(20),
            {2, 512, 512},
            sizeof(uint64_t),
            true,
            testRadixTopK<
                uint64_t, false, false, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::SMALLEST, false>},
        TopKTestParams{
            2,
            GetKPad<uint64_t>(2),
            {2, 1024, 1024},
            sizeof(uint64_t),
            true,
            testRadixTopK<
                uint64_t, true, false, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::SMALLEST, false>},
        // int64_t
        TopKTestParams{
            1,
            GetKPad<int64_t>(1),
            {1, 32, 32},
            sizeof(int64_t),
            true,
            testRadixTopK<
                int64_t, false, false, false, AscendC::TopKMode::TOPK_NSMALL, AscendC::TopKOrder::UNSET, false>},
        TopKTestParams{
            2,
            GetKPad<int64_t>(2),
            {2, 32, 32},
            sizeof(int64_t),
            true,
            testRadixTopK<
                int64_t, true, false, false, AscendC::TopKMode::TOPK_NSMALL, AscendC::TopKOrder::UNSET, false>},
        TopKTestParams{
            20,
            GetKPad<int64_t>(20),
            {1, 128, 128},
            sizeof(int64_t),
            true,
            testRadixTopK<
                int64_t, false, true, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::LARGEST, false>},
        TopKTestParams{
            2,
            GetKPad<int64_t>(2),
            {2, 256, 256},
            sizeof(int64_t),
            true,
            testRadixTopK<
                int64_t, true, true, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::LARGEST, false>},
        TopKTestParams{
            20,
            GetKPad<int64_t>(20),
            {2, 512, 512},
            sizeof(int64_t),
            true,
            testRadixTopK<
                int64_t, false, false, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::SMALLEST, false>},
        TopKTestParams{
            2,
            GetKPad<int64_t>(2),
            {2, 1024, 1024},
            sizeof(int64_t),
            true,
            testRadixTopK<
                int64_t, true, false, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::SMALLEST, false>},
        TopKTestParams{
            512,
            GetKPad<int8_t>(512),
            {1, 512, 512},
            sizeof(int8_t),
            true,
            testRadixTopK<
                int8_t, false, false, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::SMALLEST, false>},
        TopKTestParams{
            1,
            GetKPad<int8_t>(1),
            {2, 4096, 4096},
            sizeof(int8_t),
            true,
            testRadixTopK<
                int8_t, true, false, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::SMALLEST, false>},
        TopKTestParams{
            512,
            GetKPad<float>(512),
            {1, 512, 512},
            sizeof(float),
            true,
            testRadixTopK<
                float, false, false, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::SMALLEST, false>},
        TopKTestParams{
            1,
            GetKPad<float>(1),
            {2, 4096, 4096},
            sizeof(float),
            true,
            testRadixTopK<
                float, true, false, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::SMALLEST, false>},
        // bfloat16_t
        TopKTestParams{
            1,
            GetKPad<bfloat16_t>(1),
            {1, 32, 32},
            sizeof(bfloat16_t),
            true,
            testRadixTopK<
                bfloat16_t, false, false, false, AscendC::TopKMode::TOPK_NSMALL, AscendC::TopKOrder::UNSET, false>},
        TopKTestParams{
            2,
            GetKPad<bfloat16_t>(2),
            {2, 32, 32},
            sizeof(bfloat16_t),
            true,
            testRadixTopK<
                bfloat16_t, true, false, false, AscendC::TopKMode::TOPK_NSMALL, AscendC::TopKOrder::UNSET, false>},
        TopKTestParams{
            20,
            GetKPad<bfloat16_t>(20),
            {1, 128, 128},
            sizeof(bfloat16_t),
            true,
            testRadixTopK<
                bfloat16_t, false, true, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::LARGEST, false>},
        TopKTestParams{
            255,
            GetKPad<bfloat16_t>(255),
            {2, 256, 256},
            sizeof(bfloat16_t),
            true,
            testRadixTopK<
                bfloat16_t, true, true, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::LARGEST, false>},
        TopKTestParams{
            512,
            GetKPad<bfloat16_t>(512),
            {1, 512, 512},
            sizeof(bfloat16_t),
            true,
            testRadixTopK<
                bfloat16_t, false, false, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::SMALLEST, false>},
        TopKTestParams{
            1,
            GetKPad<bfloat16_t>(1),
            {2, 4096, 4096},
            sizeof(bfloat16_t),
            true,
            testRadixTopK<
                bfloat16_t, true, false, false, AscendC::TopKMode::TOPK_NORMAL, AscendC::TopKOrder::SMALLEST, false>}
#endif
        ));

TEST_P(AdvanceTopKTestSuite, testRadixTopK)
{
    auto param = GetParam();
    uint32_t indexTypeSize = 4;
    uint32_t inDataSize = param.shapeInfo.outter * param.shapeInfo.inner;
    uint8_t inputGmValue[inDataSize * param.dataTypeSize]{0x00};
    uint8_t inputGmIndex[inDataSize * indexTypeSize]{0x00};
    uint8_t finishGm[param.shapeInfo.inner]{0x00};
    uint32_t outDataSize = param.shapeInfo.outter * param.k_pad;
    uint8_t outputGmValue[outDataSize * param.dataTypeSize]{0x00};
    uint8_t outputGmIndex[outDataSize * indexTypeSize]{0x00};
    bool isLargest = param.isLargest;
    param.calFunc(
        inputGmValue, inputGmIndex, finishGm, outputGmValue, outputGmIndex, param.k, param.shapeInfo, isLargest);
    for (int32_t i = 0; i < outDataSize; i++) {
        EXPECT_EQ(outputGmValue[i], 0x00);
    }
}
