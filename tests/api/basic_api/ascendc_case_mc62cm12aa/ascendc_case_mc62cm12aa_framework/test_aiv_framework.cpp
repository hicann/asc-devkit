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
#include "kernel_tpipe_impl.h"
#include "mockcpp/mockcpp.hpp"
#include <vector>

namespace AscendC {
namespace {
constexpr int32_t TILE_NUM = 8;                          // split data into 8 tiles for each core
constexpr int32_t TILE_LENGTH = 2048;                    // length of each tile
constexpr int32_t BUFFER_LENGTH = 1024;                  // length of each tile
constexpr int32_t BLOCK_LENGTH = TILE_NUM * TILE_LENGTH; // length computed of each core
constexpr int32_t BUFFER_NUM = 2;                        // tensor num for each queue
constexpr int32_t QUE_DEPTH = 1;                         // tensor num for each queue
constexpr int32_t BUF_ID_SIZE = 4;                       // tensor num for each queue
} // namespace

template <typename T>
void GetData(LocalTensor<T>& tensor, uint32_t& addr, uint32_t& len, uint8_t& bufId)
{
    TBufHandle handle = tensor.GetBufferHandle();
    TBufType* bufType = reinterpret_cast<TBufType*>(handle);
    addr = bufType->address;
    len = bufType->dataLen;
    bufId = bufType->bufId;
}

class TQueWithBufIDTest : public testing::Test {
protected:
    void SetUp() { AscendC::SetGCoreType(2); }
    void TearDown()
    {
        AscendC::SetGCoreType(0);
        GlobalMockObject::verify();
    }
};

TEST_F(TQueWithBufIDTest, testAivQueInitBufferWithBufferID)
{
    TQue<TPosition::VECIN, 1> que;
    Hardware hardPos = GetPhyType(TPosition::VECIN);
    uint32_t len = 1024;
    TPipe pipe;
    EXPECT_EQ(pipe.g_tpipeImpl.curBufSize_, 0);
    EXPECT_EQ(pipe.g_tpipeImpl.bufPool_[static_cast<uint8_t>(hardPos)].maxAddr, 0);
    pipe.InitBuffer(que, 1, len);
    EXPECT_EQ(que.bufStart, &(pipe.g_tpipeImpl.buf_[0]));
    EXPECT_EQ(que.bufStart->address, 0);
    EXPECT_EQ(que.bufStart->state, TBufState::FREE);
    EXPECT_EQ(que.bufStart->dataLen, len);
    EXPECT_EQ(que.bufStart->usertag, -1);
    EXPECT_EQ(pipe.g_tpipeImpl.curBufSize_, 1);
    EXPECT_EQ(pipe.g_tpipeImpl.bufPool_[static_cast<uint8_t>(hardPos)].maxAddr, len);

    uint32_t lenGet = 32;
    auto tensor = que.AllocTensor<float>();
    que.EnQue(tensor);
    que.DeQue<float>();
    que.FreeTensor(tensor);
    EXPECT_EQ(que.VacantInQue(), true);
    EXPECT_EQ(que.HasTensorInQue(), false);
    EXPECT_EQ(que.GetTensorCountInQue(), 0);
    EXPECT_EQ(que.HasIdleBuffer(), true);
}

TEST_F(TQueWithBufIDTest, TBufGetWithOffsetSubByteTest)
{
    TBuf<TPosition::VECIN> buf;
    TPipe pipe;
    pipe.InitBuffer(buf, 96);

    auto int2Tensor = buf.GetWithOffset<int2b_t>(128, 32);
    EXPECT_EQ(int2Tensor.address_.bufferAddr, buf.bufStart->address + 32);
    EXPECT_EQ(int2Tensor.address_.dataLen, 32);
    EXPECT_EQ(int2Tensor.GetSize(), 128);

    auto uint1Tensor = buf.GetWithOffset<uint1b_t>(256, 64);
    EXPECT_EQ(uint1Tensor.address_.bufferAddr, buf.bufStart->address + 64);
    EXPECT_EQ(uint1Tensor.address_.dataLen, 32);
    EXPECT_EQ(uint1Tensor.GetSize(), 256);
}

TEST_F(TQueWithBufIDTest, testAivQueInitMultipleBuffers)
{
    constexpr int32_t size = 28;
    TQue<TPosition::VECIN, 1> que[size];
    Hardware hardPos = GetPhyType(TPosition::VECIN);
    uint32_t len = 1024;
    TPipe pipe;
    EXPECT_EQ(pipe.g_tpipeImpl.curBufSize_, 0);
    EXPECT_EQ(pipe.g_tpipeImpl.bufPool_[static_cast<uint8_t>(hardPos)].maxAddr, 0);
    for (int32_t i = 0; i < size; i++) {
        pipe.InitBuffer(que[i], 1, len);
        EXPECT_EQ(que[i].bufStart, &(pipe.g_tpipeImpl.buf_[i]));
        EXPECT_EQ(que[i].bufStart->address, i * len);
        EXPECT_EQ(que[i].bufStart->state, TBufState::FREE);
        EXPECT_EQ(que[i].bufStart->dataLen, len);
        EXPECT_EQ(que[i].bufStart->usertag, -1);
        EXPECT_EQ(pipe.g_tpipeImpl.curBufSize_, i + 1);
    }
}

TEST_F(TQueWithBufIDTest, testAivQueInitBufferWithDifferentQue)
{
    TQue<TPosition::VECIN, 1> inQue;
    TQue<TPosition::VECOUT, 1> outQue;
    Hardware hardPos = GetPhyType(TPosition::A1);
    uint32_t len = 1024;
    TPipe pipe;
    EXPECT_EQ(pipe.g_tpipeImpl.curBufSize_, 0);
    EXPECT_EQ(pipe.g_tpipeImpl.bufPool_[static_cast<uint8_t>(hardPos)].maxAddr, 0);
    pipe.InitBuffer(inQue, 2, len);
    pipe.InitBuffer(outQue, 2, len);
    EXPECT_EQ(inQue.bufStart, &(pipe.g_tpipeImpl.buf_[0]));
    EXPECT_EQ(inQue.bufStart->address, 0);
    EXPECT_EQ(inQue.bufStart->state, TBufState::FREE);
    EXPECT_EQ(inQue.bufStart->dataLen, len);
    EXPECT_EQ(inQue.bufStart->usertag, -1);
    EXPECT_EQ(outQue.bufStart, &(pipe.g_tpipeImpl.buf_[2]));
    EXPECT_EQ(outQue.bufStart->address, 2 * len);
    EXPECT_EQ(outQue.bufStart->state, TBufState::FREE);
    EXPECT_EQ(outQue.bufStart->dataLen, len);
    EXPECT_EQ(outQue.bufStart->usertag, -1);
    EXPECT_EQ(pipe.g_tpipeImpl.curBufSize_, 4);
}

TEST_F(TQueWithBufIDTest, testAivQueInitBufferWithNoneUBQue)
{
    TQue<TPosition::A1, 1> inQue;
    TQue<TPosition::B1, 1> outQue;
    Hardware hardPos = GetPhyType(TPosition::A1);
    uint32_t len = 1024;
    TPipe pipe;
    EXPECT_EQ(pipe.g_tpipeImpl.curBufSize_, 0);
    EXPECT_EQ(pipe.g_tpipeImpl.bufPool_[static_cast<uint8_t>(hardPos)].maxAddr, 0);
    pipe.InitBuffer(inQue, 2, len);
    pipe.InitBuffer(outQue, 2, len);
}

class TEST_TBUFPOOL : public testing::Test {
protected:
    void SetUp() { AscendC::SetGCoreType(0); }
    void TearDown() { AscendC::SetGCoreType(0); }
};

/* **************************** Tpipe InitBufPool api ****************************** */
TEST_F(TEST_TBUFPOOL, TpipeInitBufPool)
{
    TPipe pipe;
    TBufPool<TPosition::VECOUT> tbufPool1, tbufPool2;
    TQue<TPosition::VECOUT, 2> que1, que2;
    Hardware hardPos = GetPhyType(TPosition::VECOUT);
    uint8_t num = 2;
    uint32_t len = 128;
    uint32_t poolLen = 65536;
    pipe.InitBufPool(tbufPool1, poolLen);
    pipe.InitBufPool(tbufPool2, poolLen, tbufPool1);

    tbufPool1.InitBuffer(que1, num, len);
    EXPECT_EQ(tbufPool1.tBufPoolImpl.curBufSize_, 2);
    EXPECT_EQ(tbufPool1.tBufPoolImpl.maxAddr_, num * len);
    EXPECT_EQ(tbufPool1.tBufPoolImpl.maxLen_, poolLen);
    tbufPool2.InitBuffer(que2, num, len);
    EXPECT_EQ(tbufPool2.tBufPoolImpl.curBufSize_, 2);
    EXPECT_EQ(tbufPool2.tBufPoolImpl.maxAddr_, num * len);
    EXPECT_EQ(tbufPool2.tBufPoolImpl.maxLen_, poolLen);
    EXPECT_EQ(tbufPool1.tBufPoolImpl.startAddr_, 0);
    EXPECT_EQ(tbufPool2.tBufPoolImpl.startAddr_, 0);

    tbufPool1.Reset();
    EXPECT_EQ(tbufPool1.tBufPoolImpl.curBufSize_, 0);
    EXPECT_EQ(tbufPool1.tBufPoolImpl.maxAddr_, 0);
    tbufPool2.Reset();
    EXPECT_EQ(tbufPool2.tBufPoolImpl.curBufSize_, 0);
    EXPECT_EQ(tbufPool2.tBufPoolImpl.maxAddr_, 0);
}

/* **************************** TbufPool InitBufPool api ****************************** */
TEST_F(TEST_TBUFPOOL, TbufPoolInitBufPool)
{
    TPipe pipe;
    TBufPool<TPosition::VECCALC, 6> tbufPool1;
    TBufPool<TPosition::VECCALC> tbufPool2, tbufPool3;
    TQue<TPosition::VECOUT, 2> que1;
    Hardware hardPos = GetPhyType(TPosition::VECOUT);
    uint8_t num = 2;
    uint32_t len = 128;
    uint32_t poolLen = 65536;
    pipe.InitBufPool(tbufPool1, poolLen * 2);

    tbufPool1.InitBuffer(que1, num, len);
    tbufPool1.InitBufPool(tbufPool2, poolLen);
    tbufPool1.InitBufPool(tbufPool3, poolLen, tbufPool2);

    EXPECT_EQ(tbufPool1.tBufPoolImpl.startAddr_, 0);
    EXPECT_EQ(tbufPool1.tBufPoolImpl.maxAddr_, num * len + poolLen);
    EXPECT_EQ(tbufPool2.tBufPoolImpl.curBufSize_, 0);
    EXPECT_EQ(tbufPool2.tBufPoolImpl.startAddr_, num * len);
    EXPECT_EQ(tbufPool2.tBufPoolImpl.maxAddr_, num * len);
    EXPECT_EQ(tbufPool2.tBufPoolImpl.maxLen_, poolLen);
    EXPECT_EQ(tbufPool3.tBufPoolImpl.curBufSize_, 0);
    EXPECT_EQ(tbufPool3.tBufPoolImpl.startAddr_, num * len);
    EXPECT_EQ(tbufPool3.tBufPoolImpl.maxAddr_, num * len);
    EXPECT_EQ(tbufPool3.tBufPoolImpl.maxLen_, poolLen);
}

/* **************************** TbufPool InitBuffer api ****************************** */
TEST_F(TEST_TBUFPOOL, TbufPoolInitBuffer)
{
    TPipe pipe;
    TBufPool<TPosition::VECIN> tbufPool;
    TQue<TPosition::VECOUT, 2> que;
    TBuf<TPosition::VECIN> tbuf;

    Hardware hardPos = GetPhyType(TPosition::VECOUT);
    uint8_t num = 2;
    uint32_t poolLen = 65536;
    pipe.InitBufPool(tbufPool, poolLen);

    tbufPool.InitBuffer(que, num, BUFFER_LENGTH);
    tbufPool.InitBuffer(tbuf, BUFFER_LENGTH);
    EXPECT_EQ(tbufPool.tBufPoolImpl.curBufSize_, 3);
    EXPECT_EQ(tbufPool.tBufPoolImpl.startAddr_, 0);
    EXPECT_EQ(tbufPool.tBufPoolImpl.maxAddr_, 3 * BUFFER_LENGTH);
    EXPECT_EQ(tbufPool.tBufPoolImpl.maxLen_, poolLen);

    tbufPool.Reset();
    EXPECT_EQ(tbufPool.tBufPoolImpl.curBufSize_, 0);
    EXPECT_EQ(tbufPool.tBufPoolImpl.maxAddr_, 0);
}

class CustomTBufpoolTest : public testing::Test {
protected:
    void SetUp() { AscendC::SetGCoreType(2); }
    void TearDown() { AscendC::SetGCoreType(0); }
};

class MyBufPool {
public:
    __aicore__ inline MyBufPool() { Init(); }

    template <class T>
    __aicore__ inline bool InitBuffer(T& que)
    {
        auto curPoolAddr = this->GetCurAddr();

        // call internal func to initnitial bufhandle
        que.InitStartBufHandle(this->GetBufHandle(0), BUF_ID_SIZE, BUFFER_LENGTH);
        for (int32_t i = 0; i < BUF_ID_SIZE; i++) {
            que.InitBufHandle(this, i, this->GetBufHandle(i), curPoolAddr + i * BUFFER_LENGTH, BUFFER_LENGTH);
        }

        this->SetCurBufSize(BUF_ID_SIZE);

        return true;
    }

    template <AscendC::TPosition bufPos>
    __aicore__ inline bool InitBuffer(AscendC::TBuf<bufPos>& buf, uint32_t len)
    {
        auto ptr = this->GetBufHandle(this->GetCurBufSize());
        auto curPoolAddr = this->GetCurAddr();

        // call internal func to initnitial bufhandle
        buf.InitStartBufHandle(ptr, 1, len);

        this->SetCurBufSize(BUF_ID_SIZE);
        return true;
    }
    EXTERN_IMPL_BUFPOOL(MyBufPool, AscendC::TPosition::VECCALC, BUF_ID_SIZE);
};

TEST_F(CustomTBufpoolTest, testCirculaTBufpool)
{
    TPipe pipe;
    TQue<TPosition::VECIN, 1> tmpQue;
    TQue<TPosition::VECIN, 1> inQueX;
    TQue<TPosition::VECIN, 1> inQueY;
    TQue<TPosition::VECOUT, 1> outQue;
    TBuf<TPosition::VECCALC> tmpBuf;
    MyBufPool myPool;
    pipe.InitBuffer(tmpQue, 2, BUFFER_LENGTH);
    pipe.InitBufPool(myPool, BUF_ID_SIZE * BUFFER_LENGTH);
    myPool.InitBuffer(inQueX);
    myPool.InitBuffer(inQueY);
    myPool.InitBuffer(outQue);
    myPool.InitBuffer(tmpBuf, BUFFER_LENGTH);

    EXPECT_EQ(myPool.tBufPoolImpl.curBufSize_, BUF_ID_SIZE);
    EXPECT_EQ(myPool.tBufPoolImpl.startAddr_, 2 * BUFFER_LENGTH);
    EXPECT_EQ(myPool.tBufPoolImpl.maxAddr_, 2 * BUFFER_LENGTH);

    LocalTensor<float> inX = inQueX.AllocTensor<float>();
    inQueX.EnQue(inX);
    inX = inQueX.DeQue<float>();

    myPool.Reset();
    EXPECT_EQ(myPool.tBufPoolImpl.curBufSize_, 0);
}

} // namespace AscendC
