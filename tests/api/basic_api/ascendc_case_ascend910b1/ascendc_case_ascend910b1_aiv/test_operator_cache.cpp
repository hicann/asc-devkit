/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <cstddef>
#include <type_traits>
#include <gtest/gtest.h>
#include "kernel_operator.h"

using namespace std;
using namespace AscendC;

static_assert(sizeof(::BinaryMetaIcachePreloadFlag) == 8, "ICache preload TLV must occupy 8 bytes");
static_assert(
    std::is_same<decltype(::BinaryMetaIcachePreloadFlag::icachePreloadFlag), uint16_t>::value,
    "ICache preload flag must be uint16_t");
static_assert(
    std::is_same<decltype(::BinaryMetaIcachePreloadFlag::reserved), uint16_t>::value,
    "ICache preload reserved field must be uint16_t");
static_assert(
    offsetof(::BinaryMetaIcachePreloadFlag, icachePreloadFlag) == 4, "ICache preload flag must follow the TLV header");
static_assert(
    offsetof(::BinaryMetaIcachePreloadFlag, reserved) == 6, "ICache preload reserved field must follow the flag");

class TEST_CACHE : public testing::Test {
protected:
    void SetUp() { AscendC::SetGCoreType(2); }
    void TearDown() { AscendC::SetGCoreType(0); }
};

class KernelAdd {
public:
    __aicore__ inline KernelAdd() {}
    __aicore__ inline void Init(
        __gm__ uint8_t* src0Gm, __gm__ uint8_t* src1Gm, __gm__ uint8_t* cacheGm, __gm__ uint8_t* dstGm)
    {
        src0Global.SetGlobalBuffer((__gm__ half*)src0Gm);
        src1Global.SetGlobalBuffer((__gm__ half*)src1Gm);
        cacheGlobal.SetGlobalBuffer((__gm__ uint64_t*)cacheGm);
        dstGlobal.SetGlobalBuffer((__gm__ half*)dstGm);
        pipe.InitBuffer(inQueueSrc0, 1, 32 * sizeof(half));
        pipe.InitBuffer(inQueueSrc1, 1, 32 * sizeof(half));
        pipe.InitBuffer(outQueueDst, 1, 32 * sizeof(half));
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
        LocalTensor<half> src0Local = inQueueSrc0.AllocTensor<half>();
        LocalTensor<half> src1Local = inQueueSrc1.AllocTensor<half>();
        // test for cache
        DataCacheCleanAndInvalid<uint64_t, CacheLine::ENTIRE_DATA_CACHE, DcciDst::CACHELINE_OUT>(cacheGlobal);
        DataCacheCleanAndInvalid<uint64_t, CacheLine::ENTIRE_DATA_CACHE>(cacheGlobal);
        DataCopy(src0Local, src0Global, 32);
        DataCopy(src1Local, src1Global, 32);
        inQueueSrc0.EnQue(src0Local);
        inQueueSrc1.EnQue(src1Local);
    }
    __aicore__ inline void Compute()
    {
        LocalTensor<half> src0Local = inQueueSrc0.DeQue<half>();
        LocalTensor<half> src1Local = inQueueSrc1.DeQue<half>();
        LocalTensor<half> dstLocal = outQueueDst.AllocTensor<half>();

        Add(dstLocal, src0Local, src1Local, 32);

        outQueueDst.EnQue<half>(dstLocal);
        inQueueSrc0.FreeTensor(src0Local);
        inQueueSrc1.FreeTensor(src1Local);
    }
    __aicore__ inline void CopyOut()
    {
        LocalTensor<half> dstLocal = outQueueDst.DeQue<half>();
        DataCopy(dstGlobal, dstLocal, 32);
        PipeBarrier<PIPE_ALL>();
        outQueueDst.FreeTensor(dstLocal);
    }

private:
    TPipe pipe;
    TQue<QuePosition::VECIN, 1> inQueueSrc0, inQueueSrc1;
    TQue<QuePosition::VECOUT, 1> outQueueDst;
    GlobalTensor<half> src0Global, src1Global, dstGlobal;
    GlobalTensor<uint64_t> cacheGlobal;
};

TEST_F(TEST_CACHE, CacheSimpleTestCase)
{
    KernelAdd op;
    int64_t dataSize = 32;
    uint8_t src0Gm[dataSize * sizeof(half)] = {0};
    uint8_t src1Gm[dataSize * sizeof(half)] = {0};
    uint8_t cacheGm[dataSize * sizeof(uint64_t)] = {0};
    uint8_t dstGm[dataSize * sizeof(half)] = {0};
    op.Init(src0Gm, src1Gm, cacheGm, dstGm);
    op.Process();
    for (int32_t i = 0; i < dataSize; i++) {
        EXPECT_EQ(dstGm[i], 0x00);
    }
}

TEST_F(TEST_CACHE, ICachePreLoadTest) { EXPECT_NO_THROW(AscendC::ICachePreLoad(2)); }
