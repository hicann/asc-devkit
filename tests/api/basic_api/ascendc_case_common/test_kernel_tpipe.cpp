/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#define private public
#define protected public
#include "kernel_tpipe_impl.h"
#include <vector>
using namespace AscendC;
using namespace std;
/* ********************************* GetPhyType TPosition -> Hardware ********************************* */
struct PhyTypeParams {
    TPosition pos;
    Hardware hard;
};

class TEST_GET_PHY_TYPE : public testing::Test, public testing::WithParamInterface<PhyTypeParams> {
protected:
    void SetUp() {}
    void TearDown() {}
};

INSTANTIATE_TEST_CASE_P(
    SET_PHY_TYPE, TEST_GET_PHY_TYPE,
    ::testing::Values(
        PhyTypeParams{TPosition::GM, Hardware::GM}, PhyTypeParams{TPosition::A1, Hardware::L1},
        PhyTypeParams{TPosition::A2, Hardware::L0A}, PhyTypeParams{TPosition::B1, Hardware::L1},
        PhyTypeParams{TPosition::B2, Hardware::L0B},
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 1001)
        PhyTypeParams{TPosition::C1, Hardware::UB}, PhyTypeParams{TPosition::C2, Hardware::L0C},
        PhyTypeParams{TPosition::CO2, Hardware::UB},
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
        PhyTypeParams{TPosition::C1, Hardware::L1}, PhyTypeParams{TPosition::C2, Hardware::BIAS},
        PhyTypeParams{TPosition::CO2, Hardware::L1},
#endif
        PhyTypeParams{TPosition::CO1, Hardware::L0C}, PhyTypeParams{TPosition::VECIN, Hardware::UB},
        PhyTypeParams{TPosition::VECOUT, Hardware::UB}, PhyTypeParams{TPosition::VECCALC, Hardware::UB},
        PhyTypeParams{TPosition::LCM, Hardware::UB}, PhyTypeParams{TPosition::SPM, Hardware::L1},
        PhyTypeParams{TPosition::SHM, Hardware::L1}));

TEST_P(TEST_GET_PHY_TYPE, GetPhyTypeTest)
{
    auto param = GetParam();
    auto ret = GetPhyType(param.pos);
    EXPECT_EQ(ret, param.hard);
}

/* ********************************* GetBufferLogicPos TPosition -> TPosition ********************************* */
struct GetPositionParams {
    TPosition pos1;
    bool isSrc;
    TPosition pos2;
};

class TEST_GET_POSITION : public testing::Test, public testing::WithParamInterface<GetPositionParams> {
protected:
    void SetUp() {}
    void TearDown() {}
};

INSTANTIATE_TEST_CASE_P(
    GET_POSITION_TYPE, TEST_GET_POSITION,
    ::testing::Values(
        GetPositionParams{TPosition::A1, true, TPosition::GM}, GetPositionParams{TPosition::A1, false, TPosition::A1},
        GetPositionParams{TPosition::A2, true, TPosition::A1}, GetPositionParams{TPosition::A2, false, TPosition::A2},
        GetPositionParams{TPosition::B1, true, TPosition::GM}, GetPositionParams{TPosition::B1, false, TPosition::B1},
        GetPositionParams{TPosition::B2, true, TPosition::B1}, GetPositionParams{TPosition::B2, false, TPosition::B2},
        GetPositionParams{TPosition::C1, true, TPosition::GM}, GetPositionParams{TPosition::C1, false, TPosition::C1},
        GetPositionParams{TPosition::C2, true, TPosition::C1}, GetPositionParams{TPosition::C2, false, TPosition::C2},
        GetPositionParams{TPosition::CO1, true, TPosition::CO1},
        GetPositionParams{TPosition::CO1, false, TPosition::CO2},
        GetPositionParams{TPosition::CO2, true, TPosition::CO2},
        GetPositionParams{TPosition::CO2, false, TPosition::GM},
        GetPositionParams{TPosition::VECIN, true, TPosition::GM},
        GetPositionParams{TPosition::VECIN, false, TPosition::VECIN},
        GetPositionParams{TPosition::VECOUT, true, TPosition::VECOUT},
        GetPositionParams{TPosition::VECOUT, false, TPosition::GM},
        GetPositionParams{TPosition::SPM, true, TPosition::VECOUT},
        GetPositionParams{TPosition::SPM, false, TPosition::GM},
        GetPositionParams{TPosition::SHM, true, TPosition::VECOUT},
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 1001)
        GetPositionParams{TPosition::SHM, false, TPosition::GM}));
#else
        GetPositionParams{TPosition::SHM, false, TPosition::GM},
        GetPositionParams{TPosition::C2PIPE2GM, true, TPosition::B1},
        GetPositionParams{TPosition::C2PIPE2GM, false, TPosition::C2PIPE2GM}));
#endif

TEST_P(TEST_GET_POSITION, GetPositionTest)
{
    auto param = GetParam();
    auto ret = GetBufferLogicPos(param.pos1, param.isSrc);
    EXPECT_EQ(ret, param.pos2);
}

/* ********************************* GetQueEvt src + dst -> HardEvent ********************************* */
struct GetQueEvtParams {
    Hardware src;
    Hardware dst;
    bool fwdDirect;
    HardEvent evt;
};

class TEST_GET_Q_EVT : public testing::Test, public testing::WithParamInterface<GetQueEvtParams> {
protected:
    void SetUp() {}
    void TearDown() {}
};

INSTANTIATE_TEST_CASE_P(
    GET_Q_EVT, TEST_GET_Q_EVT,
    ::testing::Values(
        GetQueEvtParams{Hardware::GM, Hardware::UB, true, HardEvent::MTE2_V},
        GetQueEvtParams{Hardware::GM, Hardware::UB, false, HardEvent::V_MTE2},
        GetQueEvtParams{Hardware::GM, Hardware::L1, true, HardEvent::MTE2_MTE1},
        GetQueEvtParams{Hardware::GM, Hardware::L1, false, HardEvent::MTE1_MTE2},
        GetQueEvtParams{Hardware::GM, Hardware::L0A, true, HardEvent::MTE2_M},
        GetQueEvtParams{Hardware::GM, Hardware::L0A, false, HardEvent::M_MTE2},
        GetQueEvtParams{Hardware::GM, Hardware::L0B, true, HardEvent::MTE2_M},
        GetQueEvtParams{Hardware::GM, Hardware::L0B, false, HardEvent::M_MTE2},
        GetQueEvtParams{Hardware::UB, Hardware::GM, true, HardEvent::V_MTE3},
        GetQueEvtParams{Hardware::UB, Hardware::GM, false, HardEvent::MTE3_V},
        GetQueEvtParams{Hardware::UB, Hardware::L1, true, HardEvent::MTE3_MTE1},
        GetQueEvtParams{Hardware::UB, Hardware::L1, false, HardEvent::MTE1_MTE3},
        GetQueEvtParams{Hardware::UB, Hardware::L0C, true, HardEvent::V_V},
        GetQueEvtParams{Hardware::UB, Hardware::L0C, false, HardEvent::MAX},
        GetQueEvtParams{Hardware::L1, Hardware::UB, true, HardEvent::MTE1_V},
        GetQueEvtParams{Hardware::L1, Hardware::UB, false, HardEvent::V_MTE1},
        GetQueEvtParams{Hardware::L1, Hardware::L0A, true, HardEvent::MTE1_M},
        GetQueEvtParams{Hardware::L1, Hardware::L0A, false, HardEvent::M_MTE1},
        GetQueEvtParams{Hardware::L1, Hardware::L0B, true, HardEvent::MTE1_M},
        GetQueEvtParams{Hardware::L1, Hardware::L0B, false, HardEvent::M_MTE1},
        GetQueEvtParams{Hardware::L0A, Hardware::L0C, true, HardEvent::M_V},
        GetQueEvtParams{Hardware::L0A, Hardware::L0C, false, HardEvent::V_M},
        GetQueEvtParams{Hardware::L0C, Hardware::UB, true, HardEvent::M_V},
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 1001)
        GetQueEvtParams{Hardware::L0C, Hardware::UB, false, HardEvent::V_M}));
#else
        GetQueEvtParams{Hardware::L0C, Hardware::UB, false, HardEvent::V_M},
        GetQueEvtParams{Hardware::L1, Hardware::FIXBUF, true, HardEvent::MTE1_FIX},
        GetQueEvtParams{Hardware::L1, Hardware::FIXBUF, false, HardEvent::FIX_MTE1}));
#endif

TEST_P(TEST_GET_Q_EVT, GetQueEvtTest)
{
    auto param = GetParam();
    auto ret = GetQueEvt(param.src, param.dst, param.fwdDirect);
    EXPECT_EQ(ret, param.evt);
}

class TEST_TPIPE : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};
/* ********************************* TPipe Construction ********************************* */
TEST_F(TEST_TPIPE, TPipeConstructionTest)
{
    TPipe* pipe = new TPipe();
    // check maxsize
    EXPECT_EQ(pipe->g_tpipeImpl.curBufSize_, 0);
    // check hardware addr, alias bufpool
    for (int32_t i = 0; i < (int32_t)Hardware::MAX; i++) {
        EXPECT_EQ(pipe->g_tpipeImpl.bufPool_[i].maxAddr, 0);
    }

    // for cpu debug, check all space allocated
    for (int32_t i = 0; i < (int32_t)Hardware::MAX; i++) {
        EXPECT_TRUE(pipe->g_tpipeImpl.bufPoolBaseAddr_[i].absAddr != nullptr);
        EXPECT_EQ(
            pipe->g_tpipeImpl.bufPoolBaseAddr_[i].phySpace, ConstDefiner::Instance().bufferInitLen.at(Hardware(i)));
    }
    delete pipe;
}

/* **************************** TPipe InitBuffer With Tposition ****************************** */
TEST_F(TEST_TPIPE, TPipeInitBufferWithTBufTest)
{
    TBuf<TPosition::A1> buf;
    Hardware hardPos = GetPhyType(TPosition::A1);
    uint32_t len = 128;
    TPipe pipe;
    EXPECT_EQ(pipe.g_tpipeImpl.curBufSize_, 0);
    EXPECT_EQ(pipe.g_tpipeImpl.bufPool_[static_cast<uint8_t>(hardPos)].maxAddr, 0);
    pipe.InitBuffer(buf, len);
    EXPECT_EQ(buf.bufStart, &(pipe.g_tpipeImpl.buf_[0]));
    EXPECT_EQ(buf.bufStart->address, 0);
    EXPECT_EQ(buf.bufStart->freeBufEvtID, INVALID_TEVENTID);
    EXPECT_EQ(buf.bufStart->enQueEvtID, INVALID_TEVENTID);
    EXPECT_EQ(buf.bufStart->state, TBufState::FREE);
    EXPECT_EQ(buf.bufStart->dataLen, len);
    EXPECT_EQ(buf.bufStart->usertag, -1);
    EXPECT_EQ(pipe.g_tpipeImpl.curBufSize_, 1);
    EXPECT_EQ(pipe.g_tpipeImpl.bufPool_[static_cast<uint8_t>(hardPos)].maxAddr, len);

    uint32_t lenGet = 32;
    TBufHandle bufHandle = buf.Get(lenGet);
    TBufType* bufType = reinterpret_cast<TBufType*>(bufHandle);
    EXPECT_EQ(bufType->dataLen, lenGet);

    bufHandle = buf.Get();
    bufType = reinterpret_cast<TBufType*>(bufHandle);
    EXPECT_EQ(bufType->dataLen, len);

    LocalTensor<uint32_t> bufTensor = buf.Get<uint32_t>(lenGet);
    EXPECT_EQ(bufTensor.GetSize(), lenGet);
    EXPECT_EQ(bufTensor.GetLength(), lenGet * sizeof(uint32_t));

    bufTensor = buf.Get<uint32_t>();
    EXPECT_EQ(bufTensor.GetSize(), len / sizeof(uint32_t));
    EXPECT_EQ(bufTensor.GetLength(), len);
}

TEST_F(TEST_TPIPE, TBufGetWithOffsetInt4Test)
{
    TBuf<TPosition::A1> buf;
    TPipe pipe;
    pipe.InitBuffer(buf, 96);

    auto tensor = buf.GetWithOffset<int4b_t>(128, 32);

    EXPECT_EQ(tensor.address_.bufferAddr, buf.bufStart->address + 32);
    EXPECT_EQ(tensor.address_.dataLen, 64);
    EXPECT_EQ(tensor.GetSize(), 128);

    auto int32Tensor = buf.GetWithOffset<int32_t>(8, 64);
    EXPECT_EQ(int32Tensor.address_.dataLen, 32);
    EXPECT_EQ(int32Tensor.GetSize(), 8);
}

TEST_F(TEST_TPIPE, TBufGetWithOffsetParamCheckTest)
{
    TBuf<TPosition::A1> buf;
    TPipe pipe;
    pipe.InitBuffer(buf, 96);

    KernelRaise::GetInstance().SetRaiseMode(false);
    const uint64_t startRaiseCount = KernelRaise::GetInstance().GetRaiseCount();
    (void)buf.GetWithOffset<int4b_t>(0, 0);
    (void)buf.GetWithOffset<int4b_t>(128, 1);
    (void)buf.GetWithOffset<int4b_t>(128, 64);
    EXPECT_EQ(KernelRaise::GetInstance().GetRaiseCount() - startRaiseCount, 3);
    KernelRaise::GetInstance().SetRaiseMode(true);
}

/* **************************** TPipe InitBuffer With TQue ****************************** */
TEST_F(TEST_TPIPE, TPipeInitBufferWithTQueTest)
{
    constexpr TPosition tp[8] = {TPosition::MAX, TPosition::MAX, TPosition::MAX, TPosition::MAX,
                                 TPosition::MAX, TPosition::MAX, TPosition::MAX, TPosition::MAX};
    static constexpr AscendC::TQueConfig conf = AscendC::GetTQueConfig(false, false, false, 0, 2, 0, tp, false, false);
    TQue<TPosition::VECOUT, 2, &conf> que;
    Hardware hardPos = GetPhyType(TPosition::VECOUT);
    TPipe pipe;
    uint8_t num = 2;
    uint32_t len = 128;

    pipe.InitBuffer(que, num, len);
    EXPECT_EQ(que.head, 0);
    EXPECT_EQ(que.tail, 0);
    EXPECT_EQ(que.usedCount, 0);
    EXPECT_EQ(que.bufStart, &(pipe.g_tpipeImpl.buf_[0]));
    EXPECT_EQ(que.bufCursor, 0);
    EXPECT_EQ(que.bufUsedCount, 0);
    EXPECT_EQ(que.bufNum, 2);
    EXPECT_EQ(que.bufLen, num * len);
    struct TBufType* buf = que.bufStart;
    for (int32_t i = 0; i < num; i++) {
        EXPECT_EQ(buf->address, i * len);
        EXPECT_EQ(buf->freeBufEvtID, INVALID_TEVENTID);
        EXPECT_EQ(buf->enQueEvtID, INVALID_TEVENTID);
        EXPECT_EQ(buf->state, TBufState::FREE);
        EXPECT_EQ(buf->dataLen, len);
        EXPECT_EQ(buf->usertag, -1);
        EXPECT_EQ(buf->freeBufEvt, HardEvent::MTE3_V);
        buf++;
    }
    EXPECT_EQ(pipe.g_tpipeImpl.curBufSize_, 2);
    EXPECT_EQ(pipe.g_tpipeImpl.bufPool_[static_cast<uint8_t>(hardPos)].maxAddr, num * len);
}

/* **************************** TPipe Reset api ****************************** */
TEST_F(TEST_TPIPE, TPipeReset)
{
    TQue<TPosition::VECOUT, 2> que;
    Hardware hardPos = GetPhyType(TPosition::VECOUT);
    TPipe pipe;
    uint8_t num = 2;
    uint32_t len = 128;

    pipe.InitBuffer(que, num, len);
    EXPECT_EQ(pipe.g_tpipeImpl.curBufSize_, 2);
    EXPECT_EQ(pipe.g_tpipeImpl.bufPool_[static_cast<uint8_t>(hardPos)].maxAddr, num * len);
    pipe.Reset();
    EXPECT_EQ(pipe.g_tpipeImpl.curBufSize_, 0);
    EXPECT_EQ(pipe.g_tpipeImpl.bufPool_[static_cast<uint8_t>(hardPos)].maxAddr, 0);
}

/* **************************** TPipe GetAbsAddr ****************************** */
TEST_F(TEST_TPIPE, TPipeGetAbsAddrWithTbufTest)
{
    TPipe pipe;
    int32_t offset = 0;
    int32_t len = 1024 * 256;
    TBuffAddr addr = pipe.GetAbsAddr<TPosition::VECOUT>(offset, len);
    Hardware hardPos = GetPhyType(TPosition::VECOUT);
    EXPECT_EQ(addr.logicPos, (uint8_t)(TPosition::VECOUT));
    EXPECT_EQ(addr.bufferHandle, nullptr);
    EXPECT_EQ(addr.bufferAddr, offset);
    EXPECT_EQ(addr.dataLen, len);
    EXPECT_EQ(addr.absAddr, pipe.g_tpipeImpl.bufPoolBaseAddr_[(uint8_t)hardPos].absAddr + offset);
}

/* **************************** TPipe GetAbsAddr ****************************** */
TEST_F(TEST_TPIPE, TPipeGetAbsAddrWithTensorTest)
{
    TPipe pipe;
    int32_t offset = 0;
    int32_t size = 1024 * 64;
    Hardware hardPos = GetPhyType(TPosition::VECOUT);
    LocalTensor<int32_t> tensor1 = pipe.GetAbsAddr<TPosition::VECOUT, int32_t>(offset, size);
    EXPECT_EQ(tensor1.GetPosition(), (uint8_t)(TPosition::VECOUT));
    EXPECT_EQ(tensor1.GetBufferHandle(), nullptr);
    EXPECT_EQ(tensor1.GetSize(), size);
    EXPECT_EQ(tensor1.GetLength(), size * sizeof(int32_t));
    EXPECT_EQ(tensor1.GetPhyAddr(), (int32_t*)(pipe.g_tpipeImpl.bufPoolBaseAddr_[(uint8_t)hardPos].absAddr + offset));
}

/* **************************** TQue MaskTest ****************************** */
void TQueMaskTestWrapper()
{
    constexpr int32_t depth = 2;
    constexpr int32_t defaultMask = 0;
    constexpr int32_t nd2NzMask = 1;
    constexpr int32_t nz2NdMask = 2;
    constexpr int32_t bothMask = 3;
    TQue<TPosition::VECOUT, depth> que1;
    TQue<TPosition::VECOUT, depth, defaultMask> que2;
    TQue<TPosition::VECOUT, depth, nd2NzMask> que3;
    TQue<TPosition::VECOUT, depth, nz2NdMask> que4;
    TQue<TPosition::VECOUT, depth, bothMask> que5;
}

TEST_F(TEST_TPIPE, TQueMaskTest)
{
    MOCKER(TQueMaskTestWrapper).expects(once());
    TQueMaskTestWrapper();
    EXPECT_NO_THROW(GlobalMockObject::verify());
}

/* **************************** TQueBind MaskTest ****************************** */
void TQueBindMaskTestWrapper()
{
    constexpr int32_t depth = 2;
    constexpr int32_t defaultMask = 0;
    constexpr int32_t nd2NzMask = 1;
    constexpr int32_t nz2NdMask = 2;
    constexpr int32_t bothMask = 3;
    TQueBind<TPosition::VECOUT, TPosition::A1, depth> que1;
    TQueBind<TPosition::VECOUT, TPosition::A1, depth, defaultMask> que2;
    TQueBind<TPosition::VECOUT, TPosition::A1, depth, nd2NzMask> que3;
    TQueBind<TPosition::VECOUT, TPosition::A1, depth, nz2NdMask> que4;
    TQueBind<TPosition::VECOUT, TPosition::A1, depth, bothMask> que5;
}

TEST_F(TEST_TPIPE, TQueBindMaskTest)
{
    MOCKER(TQueBindMaskTestWrapper).expects(once());
    TQueBindMaskTestWrapper();
    EXPECT_NO_THROW(GlobalMockObject::verify());
}

/* **************************** TPipe AllocEventID / ReleaseEvent ****************************** */
constexpr int32_t A = 9999;
constexpr int32_t R_0 = 0;
constexpr int32_t R_1 = 1;
constexpr int32_t R_2 = 2;
constexpr int32_t R_3 = 3;
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510)
constexpr int32_t R_4 = 4;
constexpr int32_t R_5 = 5;
constexpr int32_t R_6 = 6;
constexpr int32_t R_7 = 7;
#endif
// struct AllocEventIDParams {
//     HardEvent evt;
//     vector<int32_t> allocOrRelease;  // if 999 --- allocEvtID; if 0/1/2/3 ---- release evtID
//     int8_t goldenEvtHead;
//     int8_t goldenEvtTail;
// };

// class TEST_ALLOC_EVT_ID
//     : public testing::Test
//     , public testing::WithParamInterface<AllocEventIDParams> {
// public:
// protected:
//     void SetUp() {}
//     void TearDown() {}
// };

// #if __CCE_AICORE__ >= 200
// INSTANTIATE_TEST_CASE_P(
//     ALLOC_EVT_ID, TEST_ALLOC_EVT_ID,
//     ::testing::Values(
//         AllocEventIDParams{ HardEvent::MTE2_MTE1, { A }, 1, 0 },
//         AllocEventIDParams{ HardEvent::MTE2_MTE1, { A, R_0, A, A, A }, 4, 1 },
//         AllocEventIDParams{ HardEvent::MTE2_MTE1, { A, A, R_0, A, A }, 4, 1 },
//         AllocEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_1 }, 4, 2 },
//         AllocEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_2 }, 4, 3 },
//         AllocEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_3 }, 4, 4 },
//         AllocEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_0, A }, 5, 1 },
//         AllocEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_1, A }, 5, 2 },
//         AllocEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_2, A }, 5, 3 },
//         AllocEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_3, A }, 5, 4 },
//         AllocEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, A, A, A, A, R_6, A }, 9, 7 },
//         AllocEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, A, A, A, A, R_6, A, R_0 }, 9, 7 },
//         AllocEventIDParams{
//             HardEvent::MTE2_MTE1, { A, A, A, A, A, A, A, A, R_0, R_1, R_2, R_3, R_4, R_5, R_6, R_7 }, 8, 8 },
//         AllocEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, A, A, A, A, R_7 }, 8, 8 },
//         AllocEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, A, A, A, A, R_7, A, A, A, A, A }, 13, 8 },
//         AllocEventIDParams{
//             HardEvent::MTE2_MTE1, { A, A, A, A, A, A, A, A, R_7, A, A, A, A, A, R_0 + QUE_MAX_EVENT }, 13, 9 },
//         AllocEventIDParams{
//             HardEvent::MTE2_MTE1, { A, A, A, A, A, A, A, A, R_7, A, A, A, A, A, R_1 + QUE_MAX_EVENT }, 13, 10 },
//         AllocEventIDParams{
//             HardEvent::MTE2_MTE1, { A, A, A, A, A, A, A, A, R_7, A, A, A, A, A, R_2 + QUE_MAX_EVENT }, 13, 11 },
//         AllocEventIDParams{
//             HardEvent::MTE2_MTE1, { A, A, A, A, A, A, A, A, R_0, R_1, R_2, R_3, R_4, R_5, R_6, R_7, A }, 9, 8 }));
// #else
// INSTANTIATE_TEST_CASE_P(
//     ALLOC_EVT_ID, TEST_ALLOC_EVT_ID,
//     ::testing::Values(
//         AllocEventIDParams{ HardEvent::MTE2_MTE1, { A }, 1, 0 },
//         AllocEventIDParams{ HardEvent::MTE2_MTE1, { A, R_0, A, A, A }, 4, 1 },
//         AllocEventIDParams{ HardEvent::MTE2_MTE1, { A, A, R_0, A, A }, 4, 1 },
//         AllocEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_1 }, 4, 2 },
//         AllocEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_2 }, 4, 3 },
//         AllocEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_3 }, 4, 4 },
//         AllocEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_0, A }, 5, 1 },
//         AllocEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_1, A }, 5, 2 },
//         AllocEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_2, A }, 5, 3 },
//         AllocEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_3, A }, 5, 4 },
//         AllocEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_3, R_2, A, A, A }, 7, 4 },
//         AllocEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_3, R_2, A, A, A, R_0 + QUE_MAX_EVENT }, 7, 5 },
//         AllocEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_3, R_2, A, A, A, R_1 + QUE_MAX_EVENT }, 7, 6 },
//         AllocEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_3, R_2, A, A, A, R_2 + QUE_MAX_EVENT }, 7, 7 }));
// #endif

/* **************************** TPipe FetchEventID ****************************** */
// struct FetchEventIDParams {
//     HardEvent evt;
//     vector<int32_t> allocOrRelease;  // if 999 --- allocEvtID; if 0/1/2/3 ---- release evtID
//     int8_t goldenEvtID;
// };

// class TEST_FETCH_EVT_ID
//     : public testing::Test
//     , public testing::WithParamInterface<FetchEventIDParams> {
// public:
// protected:
//     void SetUp() {}
//     void TearDown() {}
// };

// #if __CCE_AICORE__ >= 200
// INSTANTIATE_TEST_CASE_P(
//     FETCH_EVT_ID, TEST_FETCH_EVT_ID,
//     ::testing::Values(
//         FetchEventIDParams{ HardEvent::MTE2_MTE1, { A }, 1 },
//         FetchEventIDParams{ HardEvent::MTE2_MTE1, { A, R_0, A, A, A }, 4 },
//         FetchEventIDParams{ HardEvent::MTE2_MTE1, { A, A, R_0, A, A }, 4 },
//         FetchEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_1 }, 4 },
//         FetchEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_2 }, 4 },
//         FetchEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_3 }, 4 },
//         FetchEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_0, A }, 5 },
//         FetchEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_1, A }, 5 },
//         FetchEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_2, A }, 5 },
//         FetchEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_3, A }, 5 },
//         FetchEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, A, A, A, A, R_6, A }, 1 },
//         FetchEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, A, A, A, A, R_6, A, R_0 }, 1 },
//         FetchEventIDParams{
//             HardEvent::MTE2_MTE1, { A, A, A, A, A, A, A, A, R_0, R_1, R_2, R_3, R_4, R_5, R_6, R_7 }, 0 },
//         FetchEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, A, A, A, A, R_7 }, 0 },
//         FetchEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, A, A, A, A, R_7, A, A, A, A, A }, 5 },
//         FetchEventIDParams{
//             HardEvent::MTE2_MTE1, { A, A, A, A, A, A, A, A, R_7, A, A, A, A, A, R_0 + QUE_MAX_EVENT }, 5 },
//         FetchEventIDParams{
//             HardEvent::MTE2_MTE1, { A, A, A, A, A, A, A, A, R_7, A, A, A, A, A, R_1 + QUE_MAX_EVENT }, 5 },
//         FetchEventIDParams{
//             HardEvent::MTE2_MTE1, { A, A, A, A, A, A, A, A, R_7, A, A, A, A, A, R_2 + QUE_MAX_EVENT }, 5 },
//         FetchEventIDParams{
//             HardEvent::MTE2_MTE1, { A, A, A, A, A, A, A, A, R_0, R_1, R_2, R_3, R_4, R_5, R_6, R_7, A }, 1 }));
// #else
// INSTANTIATE_TEST_CASE_P(
//     FETCH_EVT_ID, TEST_FETCH_EVT_ID,
//     ::testing::Values(
//         FetchEventIDParams{ HardEvent::MTE2_MTE1, { A }, 1 },
//         FetchEventIDParams{ HardEvent::MTE2_MTE1, { A, R_0, A, A, A }, 0 },
//         FetchEventIDParams{ HardEvent::MTE2_MTE1, { A, A, R_0, A, A }, 0 },
//         FetchEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_1 }, 0 },
//         FetchEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_2 }, 0 },
//         FetchEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_3 }, 0 },
//         FetchEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_0, A }, 1 },
//         FetchEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_1, A }, 1 },
//         FetchEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_2, A }, 1 },
//         FetchEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_3, A }, 1 },
//         FetchEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_3, R_2, A, A, A }, 3 },
//         FetchEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_3, R_2, A, A, A, R_0 + QUE_MAX_EVENT }, 3 },
//         FetchEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_3, R_2, A, A, A, R_1 + QUE_MAX_EVENT }, 3 },
//         FetchEventIDParams{ HardEvent::MTE2_MTE1, { A, A, A, A, R_3, R_2, A, A, A, R_2 + QUE_MAX_EVENT }, 3 }));
// #endif

/* **************************** TQue AllocBuffer / FreeBuffer TBuf ****************************** */
constexpr int32_t M = 9999;
constexpr int32_t F_0 = 0;
constexpr int32_t F_1 = 1;
constexpr int32_t F_2 = 2;
constexpr int32_t F_3 = 3;
struct BufferOperateParams {
    vector<int32_t> allocOrFree;         // if 9999 --- InitBuffer; if 0/1/2/3 ---- FreeBuffer with buffer ID
    int32_t goldenBufferIdx;             // there are 4 buffers, this index is between 0~3
    vector<TBufState> goldenBufferState; // buffer state
    vector<int32_t> goldenFreeBufEvtID;  // buffer freeBufEvtID
};

class TEST_ALLOC_FREE_BUFFER : public testing::Test, public testing::WithParamInterface<BufferOperateParams> {
public:
protected:
    void SetUp() {}
    void TearDown() {}
};

INSTANTIATE_TEST_CASE_P(
    ALLOC_FREE_BUFFER, TEST_ALLOC_FREE_BUFFER,
    ::testing::Values(
        BufferOperateParams{
            {M}, 0, {TBufState::OCCUPIED, TBufState::FREE, TBufState::FREE, TBufState::FREE}, {-1, -1, -1, -1}},
        BufferOperateParams{
            {M, M, M, M},
            3,
            {TBufState::OCCUPIED, TBufState::OCCUPIED, TBufState::OCCUPIED, TBufState::OCCUPIED},
            {-1, -1, -1, -1}},
        BufferOperateParams{
            {M, F_0}, 0, {TBufState::FREE, TBufState::FREE, TBufState::FREE, TBufState::FREE}, {0, -1, -1, -1}},
        BufferOperateParams{
            {M, F_0, M}, 1, {TBufState::FREE, TBufState::OCCUPIED, TBufState::FREE, TBufState::FREE}, {0, -1, -1, -1}},
        BufferOperateParams{
            {M, F_0, M, M},
            2,
            {TBufState::FREE, TBufState::OCCUPIED, TBufState::OCCUPIED, TBufState::FREE},
            {0, -1, -1, -1}},
        BufferOperateParams{
            {M, M, M, F_1},
            2,
            {TBufState::OCCUPIED, TBufState::FREE, TBufState::OCCUPIED, TBufState::FREE},
            {-1, 0, -1, -1}},
        BufferOperateParams{
            {M, M, M, F_1, F_0},
            2,
            {TBufState::FREE, TBufState::FREE, TBufState::OCCUPIED, TBufState::FREE},
            {1, 0, -1, -1}},
        BufferOperateParams{
            {M, M, M, M, F_0},
            3,
            {TBufState::FREE, TBufState::OCCUPIED, TBufState::OCCUPIED, TBufState::OCCUPIED},
            {0, -1, -1, -1}},
        BufferOperateParams{
            {M, M, M, M, F_0, F_1, F_2, F_3},
            3,
            {TBufState::FREE, TBufState::FREE, TBufState::FREE, TBufState::FREE},
            {0, 1, 2, 3}}));

TEST_P(TEST_ALLOC_FREE_BUFFER, AllocFreeBufferTest)
{
    TPipe pipe;
    TQue<TPosition::VECOUT, 2> que;
    TBufPool<TPosition::VECCALC> tbufPool;
    TBuf<TPosition::VECCALC> tbuf;

    int32_t num = 4;
    int32_t len = 1024;
    uint32_t poolLen = 65536;
    pipe.InitBufPool(tbufPool, poolLen);

    tbufPool.InitBuffer(que, num, len);
    tbufPool.InitBuffer(tbuf, len);
    pipe.InitBuffer(que, num, len);
    TBufHandle retHandle = nullptr;
    auto param = GetParam();
    int32_t opTimes = param.allocOrFree.size();

    for (int32_t i = 0; i < opTimes; i++) {
        if (param.allocOrFree[i] == M) {
            retHandle = que.AllocBuffer();
        } else {
            que.FreeBuffer(reinterpret_cast<TBufHandle>(que.bufStart + param.allocOrFree[i]));
        }
    }
    EXPECT_EQ(reinterpret_cast<TBufHandle>(que.bufStart + param.goldenBufferIdx), retHandle);
    for (int32_t i = 0; i < num; i++) {
        TBufType* temp = que.bufStart + i;
        EXPECT_EQ(temp->state, param.goldenBufferState[i]);
        EXPECT_EQ(temp->freeBufEvtID, param.goldenFreeBufEvtID[i]);
    }
}

/* **************************** TQueBind Buf2Tensor ****************************** */
TEST_F(TEST_TPIPE, TQueBuf2TensorTest)
{
    TPipe pipe;
    TQue<TPosition::VECOUT, 2> que;
    int32_t num = 4;
    int32_t len = 1024;
    pipe.InitBuffer(que, num, len);

    TBufType* temp = que.bufStart;
    temp->state = TBufState::FREE;
    temp->freeBufEvt = HardEvent::V_M;
    temp->enQueEvtID = 15;
    temp->freeBufEvtID = INVALID_TEVENTID;
    temp->address = 0x1234;
    temp->dataLen = 4096;
    temp->usertag = 7788;
    TBufHandle bufHandle = reinterpret_cast<TBufHandle>(temp);
    LocalTensor<int32_t> tensor1 = que.Buf2Tensor<int32_t>(bufHandle);

    EXPECT_EQ(tensor1.GetBufferHandle(), bufHandle);
    EXPECT_EQ(tensor1.GetPosition(), int32_t(TPosition::VECOUT));
    EXPECT_EQ(tensor1.GetSize(), 1024);
    EXPECT_EQ(tensor1.GetLength(), 4096);
}

TEST_F(TEST_TPIPE, testCustomizeInitBuffer)
{
    TPipe pipe;
    TQue<TPosition::VECIN, 1> que1;
    TQue<TPosition::A1, 1> que2;
    TQue<TPosition::A2, 1> que3;
    auto addr0 = Std::make_tuple(0, 1024);
    auto addr1 = Std::make_tuple(1024, 1024);
    auto addr2 = Std::make_tuple(2048, 1024);
    auto addr3 = Std::make_tuple(3072, 1024);
    auto addr4 = Std::make_tuple(4096, 1024);
    auto addr5 = Std::make_tuple(5120, 1024);

    pipe.InitBuffer(que1, addr0, addr1, addr2);
    pipe.InitBuffer(que2, addr3);
    pipe.InitBuffer(que3, addr4, addr5);

    TBufHandle handle;
    TBufType* buftype;
    LocalTensor<uint8_t> tensor;

    tensor = que1.AllocTensor<uint8_t>();
    buftype = (TBufType*)tensor.GetBufferHandle();
    EXPECT_EQ(tensor.GetSize(), 1024);
    EXPECT_EQ(buftype->address, 0);

    tensor = que1.AllocTensor<uint8_t>();
    buftype = (TBufType*)tensor.GetBufferHandle();
    EXPECT_EQ(tensor.GetSize(), 1024);
    EXPECT_EQ(buftype->address, 1024);

    tensor = que1.AllocTensor<uint8_t>();
    buftype = (TBufType*)tensor.GetBufferHandle();
    EXPECT_EQ(tensor.GetSize(), 1024);
    EXPECT_EQ(buftype->address, 2048);

    tensor = que2.AllocTensor<uint8_t>();
    buftype = (TBufType*)tensor.GetBufferHandle();
    EXPECT_EQ(tensor.GetSize(), 1024);
    EXPECT_EQ(buftype->address, 3072);

    tensor = que3.AllocTensor<uint8_t>();
    buftype = (TBufType*)tensor.GetBufferHandle();
    EXPECT_EQ(tensor.GetSize(), 1024);
    EXPECT_EQ(buftype->address, 4096);

    tensor = que3.AllocTensor<uint8_t>();
    buftype = (TBufType*)tensor.GetBufferHandle();
    EXPECT_EQ(tensor.GetSize(), 1024);
    EXPECT_EQ(buftype->address, 5120);
}

/* **************************** TQue EnQue / DeQue TBuf ****************************** */
// constexpr int32_t D = 9999;
// constexpr int32_t E_0 = 0;
// constexpr int32_t E_1 = 1;
// constexpr int32_t E_2 = 2;
// constexpr int32_t E_3 = 3;
// struct TBufHandleOperateParams {
//     vector<int32_t> deQueOrEnQue;  // if 9999 --- DeQue; if 0/1/2/3 ---- EnQue with TBuf ID
//     int32_t goldenBufferIdx;       // there are 4 buffers, this index is between 0~3
//     int32_t goldenUsedCount;
//     int32_t goldenHeadIndex;
//     int32_t goldenTailIndex;
//     vector<TBufState> goldenBufferState;  // buffer state
//     vector<int32_t> goldenEnQueEvtID;     // buffer EnQueEvtID
//     bool isVacantInQue;
//     bool hasTensorInQue;
//     bool hasIdleBufferOrNot;
// };

// class TEST_DEQUE_ENQUE
//     : public testing::Test
//     , public testing::WithParamInterface<TBufHandleOperateParams> {
// public:
// protected:
//     void SetUp() {}
//     void TearDown() {}
// };

// INSTANTIATE_TEST_CASE_P(
//     DEQUE_ENQUE, TEST_DEQUE_ENQUE,
//     ::testing::Values(
//         TBufHandleOperateParams{
//             { E_0 },                                                                              // deQueOrEnQue
//             -1,                                                                                   // goldenBufferIdx
//             1,                                                                                    // goldenUsedCount
//             0,                                                                                    // goldenHeadIndex
//             1,                                                                                    // goldenTailIndex
//             { TBufState::ENQUE, TBufState::OCCUPIED, TBufState::OCCUPIED, TBufState::OCCUPIED },  //
//             goldenBufferState { 0, -1, -1, -1 }, // goldenEnQueEvtID true, // isVacantInQue true, // isVacantInQue
//             false                                                                                 //
//             hasIdleBufferOrNot
//         },
//         TBufHandleOperateParams{ { E_2 },
//                                  -1,
//                                  1,
//                                  0,
//                                  1,
//                                  { TBufState::OCCUPIED, TBufState::OCCUPIED, TBufState::ENQUE, TBufState::OCCUPIED },
//                                  { -1, -1, 0, -1 },
//                                  true,
//                                  true,
//                                  false },
//         TBufHandleOperateParams{ { E_0, E_1 },
//                                  -1,
//                                  2,
//                                  0,
//                                  2,
//                                  { TBufState::ENQUE, TBufState::ENQUE, TBufState::OCCUPIED, TBufState::OCCUPIED },
//                                  { 0, 1, -1, -1 },
//                                  true,
//                                  true,
//                                  false },
//         TBufHandleOperateParams{ { E_0, E_1, D },
//                                  0,
//                                  1,
//                                  1,
//                                  2,
//                                  { TBufState::DEQUE, TBufState::ENQUE, TBufState::OCCUPIED, TBufState::OCCUPIED },
//                                  { -1, 1, -1, -1 },
//                                  true,
//                                  true,
//                                  false },
//         TBufHandleOperateParams{ { E_0, E_1, D, D },
//                                  1,
//                                  0,
//                                  2,
//                                  2,
//                                  { TBufState::DEQUE, TBufState::DEQUE, TBufState::OCCUPIED, TBufState::OCCUPIED },
//                                  { -1, -1, -1, -1 },
//                                  true,
//                                  false,
//                                  false },
//         TBufHandleOperateParams{ { E_0, E_1, E_2, E_3 },
//                                  -1,
//                                  4,
//                                  0,
//                                  0,
//                                  { TBufState::ENQUE, TBufState::ENQUE, TBufState::ENQUE, TBufState::ENQUE },
//                                  { 0, 1, 2, 3 },
//                                  false,
//                                  true,
//                                  false },
//         TBufHandleOperateParams{ { E_1, E_0, E_3, E_2 },
//                                  -1,
//                                  4,
//                                  0,
//                                  0,
//                                  { TBufState::ENQUE, TBufState::ENQUE, TBufState::ENQUE, TBufState::ENQUE },
//                                  { 1, 0, 3, 2 },
//                                  false,
//                                  true,
//                                  false },
//         TBufHandleOperateParams{ { E_0, E_1, E_2, E_3, D, D, D, D },
//                                  3,
//                                  0,
//                                  0,
//                                  0,
//                                  { TBufState::DEQUE, TBufState::DEQUE, TBufState::DEQUE, TBufState::DEQUE },
//                                  { -1, -1, -1, -1 },
//                                  true,
//                                  false,
//                                  false },
//         TBufHandleOperateParams{ { E_1, E_0, D, E_3, D, E_2, D },
//                                  3,
//                                  1,
//                                  3,
//                                  0,
//                                  { TBufState::DEQUE, TBufState::DEQUE, TBufState::ENQUE, TBufState::DEQUE },
//                                  { -1, -1, 3, -1 },
//                                  true,
//                                  true,
//                                  false },
//         TBufHandleOperateParams{ { E_1, E_0, D, E_3, D, E_2, D, E_1, E_0 },
//                                  3,
//                                  3,
//                                  3,
//                                  2,
//                                  { TBufState::ENQUE, TBufState::ENQUE, TBufState::ENQUE, TBufState::DEQUE },
//                                  { 5, 4, 3, -1 },
//                                  true,
//                                  true,
//                                  false }));

/* **************************** TQueBind SetUserTag ****************************** */
TEST_F(TEST_TPIPE, TQueSetUserTagTest)
{
    TPipe pipe;
    TQue<TPosition::VECOUT, 4> que;
    int32_t num = 4;
    int32_t len = 1024;
    pipe.InitBuffer(que, num, len);
    vector<LocalTensor<int32_t>> bufHandleVec;
    for (int32_t i = 0; i < num; i++) {
        LocalTensor<int32_t> tensor = que.Buf2Tensor<int32_t>(que.AllocBuffer());
        bufHandleVec.push_back(tensor);
    }

    vector<TTagType> tagGolden = {0x1111, 0x2222, 0x3333, 0x4444};
    for (int32_t i = 0; i < num; i++) {
        bufHandleVec[i].SetUserTag(tagGolden[i]);
    }

    // check
    for (int32_t i = 0; i < num; i++) {
        TBufType* temp = que.bufStart + i;
        EXPECT_EQ(temp->usertag, tagGolden[i]);
        EXPECT_EQ(bufHandleVec[i].GetUserTag(), tagGolden[i]);
    }
}

/* **************************** TQue GetBufferPos ****************************** */
struct BufferPosParams {
    TPosition srcPos;
    TPosition dstPos;
    Hardware hard;
};

class TEST_GET_BUFFER_POS : public testing::Test, public testing::WithParamInterface<BufferPosParams> {
protected:
    void SetUp() {}
    void TearDown() {}
};

INSTANTIATE_TEST_CASE_P(
    GET_BUFFER_POS, TEST_GET_BUFFER_POS,
    ::testing::Values(
        BufferPosParams{TPosition::GM, TPosition::A1, Hardware::L1},
        BufferPosParams{TPosition::GM, TPosition::B1, Hardware::L1},
        BufferPosParams{TPosition::GM, TPosition::C1, Hardware::UB},
        BufferPosParams{TPosition::GM, TPosition::VECIN, Hardware::UB},
        BufferPosParams{TPosition::SHM, TPosition::A1, Hardware::L1},
        BufferPosParams{TPosition::SHM, TPosition::B1, Hardware::L1},
        BufferPosParams{TPosition::SHM, TPosition::C1, Hardware::UB},
        BufferPosParams{TPosition::SHM, TPosition::VECIN, Hardware::UB},
        BufferPosParams{TPosition::A1, TPosition::A2, Hardware::L0A},
        BufferPosParams{TPosition::B1, TPosition::B2, Hardware::L0B},
        BufferPosParams{TPosition::VECIN, TPosition::VECCALC, Hardware::UB},
        BufferPosParams{TPosition::VECCALC, TPosition::VECOUT, Hardware::UB},
        BufferPosParams{TPosition::VECOUT, TPosition::GM, Hardware::UB},
        BufferPosParams{TPosition::A2, TPosition::CO1, Hardware::L0C},
        BufferPosParams{TPosition::B2, TPosition::CO1, Hardware::L0C},
        BufferPosParams{TPosition::CO2, TPosition::GM, Hardware::UB},
        BufferPosParams{TPosition::VECOUT, TPosition::A1, Hardware::L1},
        BufferPosParams{TPosition::VECOUT, TPosition::B1, Hardware::L1},
        BufferPosParams{TPosition::VECOUT, TPosition::C1, Hardware::UB},
        BufferPosParams{TPosition::CO2, TPosition::A1, Hardware::L1},
        BufferPosParams{TPosition::CO2, TPosition::B1, Hardware::L1},
        BufferPosParams{TPosition::CO2, TPosition::C1, Hardware::UB},
        BufferPosParams{TPosition::CO2, TPosition::VECIN, Hardware::UB}));

TEST_P(TEST_GET_BUFFER_POS, GetBufferPosTest)
{
    auto param = GetParam();
    Hardware ret = GetBufferPos(param.srcPos, param.dstPos);
    EXPECT_EQ(ret, param.hard);
}

/* **************************** TQue GetPosition ****************************** */
struct BufferLogicPosParams {
    TPosition srcPos;
    TPosition dstPos;
    TPosition logicPos;
};

class TEST_GET_BUFFER_LOGIC_POS : public testing::Test, public testing::WithParamInterface<BufferLogicPosParams> {
protected:
    void SetUp() {}
    void TearDown() {}
};

INSTANTIATE_TEST_CASE_P(
    GET_BUFFER_LOGIC_POS, TEST_GET_BUFFER_LOGIC_POS,
    ::testing::Values(
        BufferLogicPosParams{TPosition::GM, TPosition::A1, TPosition::A1},
        BufferLogicPosParams{TPosition::GM, TPosition::B1, TPosition::B1},
        BufferLogicPosParams{TPosition::GM, TPosition::C1, TPosition::C1},
        BufferLogicPosParams{TPosition::GM, TPosition::VECIN, TPosition::VECIN},
        BufferLogicPosParams{TPosition::SHM, TPosition::A1, TPosition::A1},
        BufferLogicPosParams{TPosition::SHM, TPosition::B1, TPosition::B1},
        BufferLogicPosParams{TPosition::SHM, TPosition::C1, TPosition::C1},
        BufferLogicPosParams{TPosition::SHM, TPosition::VECIN, TPosition::VECIN},
        BufferLogicPosParams{TPosition::A1, TPosition::A2, TPosition::A2},
        BufferLogicPosParams{TPosition::B1, TPosition::B2, TPosition::B2},
        BufferLogicPosParams{TPosition::VECIN, TPosition::VECCALC, TPosition::VECCALC},
        BufferLogicPosParams{TPosition::VECCALC, TPosition::VECOUT, TPosition::VECOUT},
        BufferLogicPosParams{TPosition::VECOUT, TPosition::GM, TPosition::VECOUT},
        BufferLogicPosParams{TPosition::A2, TPosition::CO1, TPosition::CO1},
        BufferLogicPosParams{TPosition::B2, TPosition::CO1, TPosition::CO1},
        BufferLogicPosParams{TPosition::CO1, TPosition::CO2, TPosition::CO1},
        BufferLogicPosParams{TPosition::CO2, TPosition::GM, TPosition::CO2},
        BufferLogicPosParams{TPosition::VECOUT, TPosition::A1, TPosition::A1},
        BufferLogicPosParams{TPosition::VECOUT, TPosition::B1, TPosition::B1},
        BufferLogicPosParams{TPosition::VECOUT, TPosition::C1, TPosition::C1},
        BufferLogicPosParams{TPosition::CO2, TPosition::A1, TPosition::A1},
        BufferLogicPosParams{TPosition::CO2, TPosition::B1, TPosition::B1},
        BufferLogicPosParams{TPosition::CO2, TPosition::C1, TPosition::C1},
        BufferLogicPosParams{TPosition::CO2, TPosition::VECIN, TPosition::VECIN}));

TEST_P(TEST_GET_BUFFER_LOGIC_POS, GetBufferLogicPosTest)
{
    auto param = GetParam();
    TPosition ret = GetPosition(param.srcPos, param.dstPos);
    EXPECT_EQ(ret, param.logicPos);
}

class TEST_TBUFPOOL : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

/* **************************** Tpipe InitBufPool api ****************************** */
TEST_F(TEST_TBUFPOOL, TpipeInitBufPool)
{
    TPipe pipe;
    TBufPool<TPosition::VECCALC> tbufPool1, tbufPool2;
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
    TBufPool<TPosition::VECCALC> tbufPool1, tbufPool2, tbufPool3;
    TQue<TPosition::VECOUT, 2> que1;
    Hardware hardPos = GetPhyType(TPosition::VECOUT);
    uint8_t num = 2;
    uint32_t len = 128;
    uint32_t poolLen = 65536;
    pipe.InitBufPool(tbufPool1, poolLen * 2);

    tbufPool1.InitBuffer(que1, num, len);
    tbufPool1.InitBufPool(tbufPool2, poolLen);
    tbufPool1.InitBufPool(tbufPool3, poolLen, tbufPool2);

    EXPECT_EQ(tbufPool1.tBufPoolImpl.curBufSize_, 2); // InitBufPool doesn't generate bufHandle;
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
    TBufPool<TPosition::VECCALC> tbufPool;
    TQue<TPosition::VECOUT, 2> que;

    TBuf<TPosition::VECCALC> tbuf;

    Hardware hardPos = GetPhyType(TPosition::VECOUT);
    uint8_t num = 2;
    uint32_t len = 128;
    uint32_t poolLen = 65536;
    pipe.InitBufPool(tbufPool, poolLen);

    tbufPool.InitBuffer(que, num, len);
    tbufPool.InitBuffer(tbuf, len);
    EXPECT_EQ(tbufPool.tBufPoolImpl.curBufSize_, 3);
    EXPECT_EQ(tbufPool.tBufPoolImpl.startAddr_, 0);
    EXPECT_EQ(tbufPool.tBufPoolImpl.maxAddr_, 3 * len);
    EXPECT_EQ(tbufPool.tBufPoolImpl.maxLen_, poolLen);
}

/* **************************** TbufPool Unaligned InitBufPool api ****************************** */
TEST_F(TEST_TBUFPOOL, TbufPoolInitBufPoolUnalign)
{
    TPipe pipe;
    TBufPool<TPosition::VECCALC> tbufPool1, tbufPool2, tbufPool3;
    TQue<TPosition::VECOUT, 2> que1;
    Hardware hardPos = GetPhyType(TPosition::VECOUT);
    uint32_t unAligedLen = 65537;
    uint32_t unAligedLen2 = 127;
    uint32_t aligedLen = 65568;
    uint32_t aligedLen2 = 128;

    pipe.InitBufPool(tbufPool1, unAligedLen);
    tbufPool1.InitBufPool(tbufPool2, unAligedLen2);

    EXPECT_EQ(tbufPool1.tBufPoolImpl.startAddr_, 0);
    EXPECT_EQ(tbufPool1.tBufPoolImpl.maxAddr_, aligedLen2);
    EXPECT_EQ(tbufPool1.tBufPoolImpl.maxLen_, aligedLen);
    EXPECT_EQ(tbufPool2.tBufPoolImpl.startAddr_, 0);
    EXPECT_EQ(tbufPool2.tBufPoolImpl.maxAddr_, 0);
    EXPECT_EQ(tbufPool2.tBufPoolImpl.maxLen_, aligedLen2);
}

class MyBufPool {
public:
    __aicore__ inline MyBufPool() { Init(); }

    template <class T>
    __aicore__ inline bool InitBuffer(T& que, uint8_t num, uint32_t len)
    {
        len = (len + ONE_BLK_SIZE - MIN_BLOCK_LEN) / ONE_BLK_SIZE * ONE_BLK_SIZE;
        auto ptr = this->tBufPoolImpl.buf_ + this->tBufPoolImpl.curBufSize_;
        auto curPoolAddr = this->tBufPoolImpl.maxAddr_;

        // call internal func to initnitial bufhandle
        que.InitStartBufHandle(reinterpret_cast<TBufHandle>(ptr), num, len);
        for (int32_t i = 0; i < num; i++) {
            que.InitBufHandle(this, i, reinterpret_cast<TBufHandle>(ptr + i), curPoolAddr + i * len, len);
        }

        this->tBufPoolImpl.maxAddr_ = curPoolAddr + num * len;
        this->tBufPoolImpl.curBufSize_ += num;

        return true;
    }

    template <TPosition bufPos>
    __aicore__ inline bool InitBuffer(TBuf<bufPos>& buf, uint32_t len)
    {
        len = (len + ONE_BLK_SIZE - MIN_BLOCK_LEN) / ONE_BLK_SIZE * ONE_BLK_SIZE;
        auto ptr = this->tBufPoolImpl.buf_ + this->tBufPoolImpl.curBufSize_;
        auto curPoolAddr = this->tBufPoolImpl.maxAddr_;

        // call internal func to initnitial bufhandle
        buf.InitStartBufHandle(reinterpret_cast<TBufHandle>(ptr), 1, len);
        buf.InitBufHandle(this, 0, reinterpret_cast<TBufHandle>(ptr), curPoolAddr, len);

        this->tBufPoolImpl.maxAddr_ = curPoolAddr + len;
        this->tBufPoolImpl.curBufSize_ += 1;
        return true;
    }
    EXTERN_IMPL_BUFPOOL(MyBufPool, TPosition::VECCALC, 16);
};

TEST_F(TEST_TBUFPOOL, MyBufPoolInitBufferCase1)
{
    TPipe pipe;
    TQue<TPosition::VECIN, 1> srcQue0;
    MyBufPool myTbufPool;
    uint32_t poolLen = 65536 * 3;
    uint32_t singleLen = 65536;
    pipe.InitBufPool(myTbufPool, poolLen);
    myTbufPool.InitBuffer(srcQue0, 1, singleLen);
    LocalTensor<half> src0Local = srcQue0.AllocTensor<half>();
    srcQue0.FreeTensor(src0Local);
    src0Local = srcQue0.AllocTensor<half>();
    srcQue0.FreeTensor(src0Local);
    EXPECT_EQ(myTbufPool.tBufPoolImpl.curBufSize_, 1);
    EXPECT_EQ(myTbufPool.tBufPoolImpl.startAddr_, 0);
    EXPECT_EQ(myTbufPool.tBufPoolImpl.maxAddr_, singleLen);
}

TEST_F(TEST_TBUFPOOL, MyBufPoolInitBufferCase2)
{
    TPipe pipe;
    TBuf<TPosition::VECIN> srcBuf1;
    MyBufPool myTbufPool;
    uint32_t poolLen = 65536 * 3;
    uint32_t singleLen = 65536 * 2;
    pipe.InitBufPool(myTbufPool, poolLen);
    myTbufPool.InitBuffer(srcBuf1, singleLen);
    EXPECT_EQ(myTbufPool.tBufPoolImpl.curBufSize_, 1);
    EXPECT_EQ(myTbufPool.tBufPoolImpl.startAddr_, 0);
    EXPECT_EQ(myTbufPool.tBufPoolImpl.maxAddr_, singleLen);
}

/* ************************ TQueBind EnQue with user src and dst position **************************** */
TEST_F(TEST_TPIPE, TQueBindEnQueTest)
{
    constexpr int32_t depth = 10;
    constexpr int32_t len = 1024;
    TPipe pipe;
    TQueBind<TPosition::GM, TPosition::VECIN, depth> que1;
    pipe.InitBuffer(que1, 10, len);
    LocalTensor<float> tensor1 = que1.AllocTensor<float>();
    LocalTensor<float> tensor2 = que1.AllocTensor<float>();
    LocalTensor<float> tensor3 = que1.AllocTensor<float>();
    LocalTensor<float> tensor4 = que1.AllocTensor<float>();
    LocalTensor<float> tensor5 = que1.AllocTensor<float>();
    que1.EnQue<TPosition::GM, TPosition::VECIN>(tensor1);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[0].state, TBufState::ENQUE);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[0].freeBufEvt, HardEvent::V_MTE2);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[0].enQueEvtID, 0);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[0].freeBufEvtID, -1);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[0].dataLen, tensor1.GetLength());

    que1.EnQue<TPosition::VECIN, TPosition::VECOUT>(tensor2);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[1].state, TBufState::ENQUE);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[1].freeBufEvt, HardEvent::V_MTE2);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[1].enQueEvtID, 0);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[1].freeBufEvtID, -1);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[1].dataLen, tensor2.GetLength());

    que1.EnQue<TPosition::VECIN, TPosition::GM>(tensor3);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[2].state, TBufState::ENQUE);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[2].freeBufEvt, HardEvent::V_MTE2);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[2].enQueEvtID, 0);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[2].freeBufEvtID, -1);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[2].dataLen, tensor3.GetLength());

    que1.EnQue<TPosition::VECIN, TPosition::GM>(tensor4);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[3].state, TBufState::ENQUE);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[3].freeBufEvt, HardEvent::V_MTE2);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[3].enQueEvtID, 1);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[3].freeBufEvtID, -1);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[3].dataLen, tensor4.GetLength());

    que1.EnQue<TPosition::VECIN, TPosition::GM>(tensor5);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[4].state, TBufState::ENQUE);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[4].freeBufEvt, HardEvent::V_MTE2);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[4].enQueEvtID, 2);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[4].freeBufEvtID, -1);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[4].dataLen, tensor4.GetLength());

    auto retTensor1 = que1.DeQue<TPosition::GM, TPosition::VECIN, float>();
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[0].state, TBufState::DEQUE);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[0].freeBufEvt, HardEvent::V_MTE2);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[0].enQueEvtID, -1);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[0].freeBufEvtID, -1);
    EXPECT_EQ(retTensor1.GetPhyAddr(), tensor1.GetPhyAddr());

    auto retTensor2 = que1.DeQue<TPosition::VECIN, TPosition::VECOUT, float>();
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[1].state, TBufState::DEQUE);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[1].freeBufEvt, HardEvent::V_MTE2);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[1].enQueEvtID, -1);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[1].freeBufEvtID, -1);
    EXPECT_EQ(retTensor2.GetPhyAddr(), tensor2.GetPhyAddr());

    auto retTensor3 = que1.DeQue<TPosition::VECIN, TPosition::GM, float>();
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[2].state, TBufState::DEQUE);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[2].freeBufEvt, HardEvent::V_MTE2);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[2].enQueEvtID, -1);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[2].freeBufEvtID, -1);
    EXPECT_EQ(retTensor3.GetPhyAddr(), tensor3.GetPhyAddr());

    auto retTensor4 = que1.DeQue<TPosition::VECIN, TPosition::GM, float>();
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[3].state, TBufState::DEQUE);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[3].freeBufEvt, HardEvent::V_MTE2);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[3].enQueEvtID, -1);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[3].freeBufEvtID, -1);
    EXPECT_EQ(retTensor4.GetPhyAddr(), tensor4.GetPhyAddr());

    auto retTensor5 = que1.DeQue<TPosition::VECIN, TPosition::GM, float>();
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[4].state, TBufState::DEQUE);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[4].freeBufEvt, HardEvent::V_MTE2);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[4].enQueEvtID, -1);
    EXPECT_EQ(pipe.g_tpipeImpl.buf_[4].freeBufEvtID, -1);
    EXPECT_EQ(retTensor5.GetPhyAddr(), tensor5.GetPhyAddr());
}

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002)
extern int32_t logLevel;
/* ************************ TQueSync Test **************************** */
struct TQueSyncParams {
    int32_t mode;
    TEventID id;
    std::vector<std::string> golden;
};

class TEST_TQUE_SYNC_PARAMS : public testing::Test, public testing::WithParamInterface<TQueSyncParams> {
protected:
    void SetUp() {}
    void TearDown() {}
    void OpenLog()
    {
        logLevel = 0; // 修改日志级别
        fileName = "tque_sync_test_" + std::to_string(getpid()) + ".txt";
        setvbuf(stdout, nullptr, _IONBF, 0); // 定义流 stream 不使用何缓冲
        save_fd = dup(STDOUT_FILENO);        // 保存标准输出文件描述符
        fd = open(fileName.c_str(), (O_RDWR | O_CREAT), 0644);
        dup2(fd, STDOUT_FILENO); // 用fd替换标准输出
    }

    void CloseLog()
    {
        logLevel = 3; // 将日志级别改回
        close(fd);
        dup2(save_fd, STDOUT_FILENO); // 用完之后恢复标准输出到屏幕
    }

    bool CheckGolden(std::vector<std::string>& goldenVec)
    {
        // 在日志文件中寻找golden字符串并返回结果
        std::ifstream resultFile;
        std::stringstream streambuffer;
        resultFile.open(fileName, std::ios::in);
        streambuffer << resultFile.rdbuf();
        std::string resultString(streambuffer.str());
        for (auto i = 0; i < goldenVec.size(); i++) {
            if (resultString.find(goldenVec[i]) == std::string::npos) {
                return false;
            }
        }
        return true;
    }

    void RemoveLogFile() { remove(fileName.c_str()); }

private:
    std::string fileName;
    int save_fd;
    int fd;
};

constexpr pipe_t srcPipe[] = {PIPE_MTE2, PIPE_MTE3, PIPE_S, PIPE_V, PIPE_S, PIPE_MTE1};
constexpr pipe_t dstPipe[] = {PIPE_MTE3, PIPE_MTE2, PIPE_MTE3, PIPE_S, PIPE_V, PIPE_M};

INSTANTIATE_TEST_CASE_P(
    TQUE_SYNC_PARAMS, TEST_TQUE_SYNC_PARAMS,
    ::testing::Values(
        TQueSyncParams{0, 3, {"set_flag(PIPE_MTE2, PIPE_MTE3, 3);"}},
        TQueSyncParams{1, 3, {"set_flag(PIPE_MTE3, PIPE_MTE2, 3);"}},
        TQueSyncParams{2, 3, {"wait_flag(PIPE_S, PIPE_MTE3, 3);"}},
        TQueSyncParams{3, 3, {"wait_flag(PIPE_V, PIPE_S, 3);"}}, TQueSyncParams{4, 3, {"set_flag(PIPE_S, PIPE_V, 3);"}},
        TQueSyncParams{5, 3, {"set_flag(PIPE_MTE1, PIPE_M, 3);"}}));

TEST_P(TEST_TQUE_SYNC_PARAMS, TQueSyncTest)
{
    TPipe pipe;
    logLevel = 0;
    TQueSyncParams param = GetParam();
    OpenLog();

    // 执行case
    if (param.mode == 0) {
        TQueSync<srcPipe[0], dstPipe[0]>().SetFlag(3);
        TQueSync<srcPipe[0], dstPipe[0]>().WaitFlag(3);
    } else if (param.mode == 1) {
        TQueSync<srcPipe[1], dstPipe[1]>().SetFlag(3);
        TQueSync<srcPipe[1], dstPipe[1]>().WaitFlag(3);
    } else if (param.mode == 2) {
        TQueSync<srcPipe[2], dstPipe[2]>().SetFlag(3);
        TQueSync<srcPipe[2], dstPipe[2]>().WaitFlag(3);
    } else if (param.mode == 3) {
        TQueSync<srcPipe[3], dstPipe[3]>().SetFlag(3);
        TQueSync<srcPipe[3], dstPipe[3]>().WaitFlag(3);
    } else if (param.mode == 4) {
        TQueSync<srcPipe[4], dstPipe[4]>().SetFlag(3);
        TQueSync<srcPipe[4], dstPipe[4]>().WaitFlag(3);
    } else if (param.mode == 5) {
        TQueSync<srcPipe[5], dstPipe[5]>().SetFlag(3);
        TQueSync<srcPipe[5], dstPipe[5]>().WaitFlag(3);
    }

    // 输出并校验日志
    CloseLog();
    EXPECT_TRUE(CheckGolden(param.golden));
    RemoveLogFile();
}
#endif
