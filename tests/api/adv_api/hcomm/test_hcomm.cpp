/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <gtest/gtest.h>
#include <vector>
#define private public
#include "kernel_operator.h"
#include "hcomm/hcomm.h"

using namespace std;
using namespace AscendC;
class HcommCommonTestSuite : public testing::Test {
protected:
    virtual void SetUp()
    {
        blockIdxBak_ = block_idx;
        channel_.sqNum = 1;
        sqCtx_.contextInfo.roceSq.sqVa = (uint64_t)sqVa_;
        sqCtx_.contextInfo.roceSq.dbHwVa = (uint64_t)dbVa_;
        sqCtx_.contextInfo.roceSq.wqeSize = 48;
        sqCtx_.contextInfo.roceSq.depth = 10;
        sqCtx_.contextInfo.roceSq.qpn = 1;
        sqCtx_.contextInfo.roceSq.headAddr = (uint64_t)(&head_);
        sqCtx_.contextInfo.roceSq.tailAddr = (uint64_t)(&tail_);
        sqCtx_.contextInfo.roceSq.sl = 1;
        channel_.sqContextAddr = &sqCtx_;
        channel_.localBufferNum = 1;
        localBuff_.type = RegedBufferType::REGED_BUFFER_RMA;
        localBuff_.bufferInfo.rma.protectionInfo.memInfo.roce.lkey = 123456;
        channel_.localBufferAddr = &localBuff_;
        channel_.remoteBufferNum = 1;
        remoteBuff_.type = RegedBufferType::REGED_BUFFER_RMA;
        remoteBuff_.bufferInfo.rma.protectionInfo.memInfo.roce.rkey = 123456;
        channel_.remoteBufferAddr = &remoteBuff_;
    }
    virtual void TearDown() { block_idx = blockIdxBak_; }

private:
    int64_t blockIdxBak_;
    ChannelEntity channel_;
    SqContext sqCtx_;
    uint8_t sqVa_[100] = {0};
    uint8_t dbVa_[8] = {0};
    uint32_t head_ = 0;
    uint32_t tail_ = 0;
    RegedBufferEntity localBuff_;
    RegedBufferEntity remoteBuff_;
};

TEST_F(HcommCommonTestSuite, Aiv_Read)
{
    Hcomm<AscendC::COMM_PROTOCOL_ROCE> hcomm;
    ChannelHandle channelHandle = reinterpret_cast<ChannelHandle>(&channel_);
    int32_t ret = hcomm.ReadNbi(channelHandle, reinterpret_cast<GM_ADDR>(0x11), reinterpret_cast<GM_ADDR>(0x22), 1);
    EXPECT_EQ(ret, 0);
}

TEST_F(HcommCommonTestSuite, Aiv_Write)
{
    Hcomm<AscendC::COMM_PROTOCOL_ROCE> hcomm;
    ChannelHandle channelHandle = reinterpret_cast<ChannelHandle>(&channel_);
    int32_t ret = hcomm.WriteNbi(channelHandle, reinterpret_cast<GM_ADDR>(0x11), reinterpret_cast<GM_ADDR>(0x22), 1);
    EXPECT_EQ(ret, 0);
}

TEST_F(HcommCommonTestSuite, Aiv_WriteWithNotifyNbi)
{
    Hcomm<AscendC::COMM_PROTOCOL_ROCE> hcomm;
    ChannelHandle channelHandle = reinterpret_cast<ChannelHandle>(&channel_);
    int32_t ret = hcomm.WriteWithNotifyNbi(
        channelHandle, reinterpret_cast<GM_ADDR>(0x11), reinterpret_cast<GM_ADDR>(0x22), 1,
        reinterpret_cast<GM_ADDR>(0x33), 2);
    EXPECT_EQ(ret, -1);
}
