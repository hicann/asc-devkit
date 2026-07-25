/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file hcomm_aiv.h
 * \brief Hcomm AIV implementation for V220
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/hcomm/impl/platform_v220/hcomm_aiv.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/simplesoftmax.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HCOMM_AIV_H__
#endif

#ifndef IMPL_ADV_API_DETAIL_HCOMM_IMPL_PLATFORM_V220_HCOMM_AIV_H
#define IMPL_ADV_API_DETAIL_HCOMM_IMPL_PLATFORM_V220_HCOMM_AIV_H

#include "hcomm_aiv_def.h"
#include "../../common/hcomm_utils.h"
#include "../../common/hcomm_inner_def.h"

namespace AscendC {

__aicore__ inline HcommImpl<COMM_PROTOCOL_ROCE>::HcommImpl()
{
    TBuf<TPosition::VECOUT> rdmaInBuf;
    GetTPipePtr()->InitBuffer(rdmaInBuf, ONE_BLK_SIZE);
    ubLocal_ = rdmaInBuf.Get<uint64_t>();

    TBuf<TPosition::VECOUT> rdmaInBuf2;
    GetTPipePtr()->InitBuffer(rdmaInBuf2, ONE_BLK_SIZE);
    ubLocalHead_ = rdmaInBuf2.Get<uint32_t>();
}

__aicore__ inline HcommImpl<COMM_PROTOCOL_ROCE>::~HcommImpl() {}

template <bool commit, pipe_t commitPipe, pipe_t reqPipe, auto const& config>
__aicore__ inline int32_t HcommImpl<COMM_PROTOCOL_ROCE>::WriteNbi(
    ChannelHandle channel, GM_ADDR dst, GM_ADDR src, uint64_t len)
{
    (void)config;
    KERNEL_LOG(KERNEL_INFO, "Hcomm Write channel:%llu, dst:%p, src:%p, len:%llu", channel, dst, src, len);
    PostSend(channel, dst, src, len, false);
    KERNEL_LOG(KERNEL_INFO, "Hcomm Write complete");
    return HCOMM_SUCCESS;
}

template <bool commit, pipe_t commitPipe, pipe_t reqPipe, auto const& config>
__aicore__ inline int32_t HcommImpl<COMM_PROTOCOL_ROCE>::ReadNbi(
    ChannelHandle channel, GM_ADDR dst, GM_ADDR src, uint64_t len)
{
    (void)config;
    KERNEL_LOG(KERNEL_INFO, "Hcomm Read channel:%llu, dst:%p, src:%p, len:%llu", channel, dst, src, len);
    PostSend(channel, dst, src, len, true);
    KERNEL_LOG(KERNEL_INFO, "Hcomm Read complete");
    return HCOMM_SUCCESS;
}

template <bool commit, pipe_t commitPipe, pipe_t reqPipe, auto const& config>
__aicore__ inline int32_t HcommImpl<COMM_PROTOCOL_ROCE>::WriteWithNotifyNbi(
    ChannelHandle channel, GM_ADDR dst, GM_ADDR src, uint64_t len, GM_ADDR notifyAddr, uint64_t notifyVal)
{
    (void)commit;
    (void)commitPipe;
    (void)reqPipe;
    (void)config;
    (void)channel;
    (void)dst;
    (void)src;
    (void)len;
    (void)notifyAddr;
    (void)notifyVal;
    KERNEL_LOG(KERNEL_ERROR, "Hcomm ROCE WriteWithNotifyNbi is not supported");
    return HCOMM_FAILED;
}

__aicore__ inline void HcommImpl<COMM_PROTOCOL_ROCE>::doorBell(__gm__ ChannelEntity* channel, uint64_t curHead)
{
    uint64_t doorBellInfo = 0;
    doorBellInfo |= channel->sqContextAddr[0].contextInfo.roceSq.qpn; // [0:23] DB_TAG (qp_num)
    doorBellInfo |= 0UL << 24UL;                                      // [24:27] DB_CMD = HNS_ROCE_V2_SQ_DB (0)
    doorBellInfo |= (curHead % 65536UL) << 32UL;                      // [32:47] DB_PI = sq.head
    doorBellInfo |= (uint64_t)(channel->sqContextAddr[0].contextInfo.roceSq.sl) << 48UL; // [48:50] DB_SL = qp.sl

    __gm__ uint64_t* doorBellAddr = (__gm__ uint64_t*)(channel->sqContextAddr[0].contextInfo.roceSq.dbHwVa);
    KERNEL_LOG(KERNEL_INFO, "Hcomm doorBell doorBellAddr:%p, doorBellInfo:%llu", doorBellAddr, doorBellInfo);

    ubLocal_.SetValue(0, doorBellInfo);
    AscendC::GlobalTensor<uint64_t> DBGlobalTensor;
    DBGlobalTensor.SetGlobalBuffer(doorBellAddr);
    AscendC::DataCopyExtParams copyParams{1, 1 * sizeof(uint64_t), 0, 0, 0};
    AscendC::DataCopyPad(DBGlobalTensor, ubLocal_, copyParams);
}

__aicore__ inline void HcommImpl<COMM_PROTOCOL_ROCE>::PostSend(
    ChannelHandle channelHandle, GM_ADDR dst, GM_ADDR src, uint64_t len, bool isRead)
{
    __gm__ ChannelEntity* channel = (__gm__ ChannelEntity*)channelHandle;
    auto qpNum = channel->sqContextAddr[0].contextInfo.roceSq.qpn;
    auto sqBaseAddr = channel->sqContextAddr[0].contextInfo.roceSq.sqVa;
    auto wqeSize = channel->sqContextAddr[0].contextInfo.roceSq.wqeSize;
    auto curHardwareHead = channel->sqContextAddr[0].contextInfo.roceSq.headAddr;
    CacheWriteThrough(reinterpret_cast<__gm__ uint8_t*>(curHardwareHead), 8);
    uint64_t curHead = *(__gm__ uint32_t*)(curHardwareHead);

    auto curHardwareTailAddr = channel->sqContextAddr[0].contextInfo.roceSq.tailAddr;
    uint64_t shift = 15U;
    auto qpDepth = channel->sqContextAddr[0].contextInfo.roceSq.depth;

    KERNEL_LOG(
        KERNEL_INFO, "Hcomm doorBell qpNum:%d, sqBaseAddr:%p, wqeSize:%d, curHead:%d, qpDepth:%d", qpNum, sqBaseAddr,
        wqeSize, curHead, qpDepth);

    // Make sure we don't overflow the SQ in an infinite loop - no need to mitigate endless loop as the host
    // will timeout and kill the kernel, same as all2all kernel if it fails to complete (e.g. in case of link loss)
    while (1) {
        CacheWriteThrough((__gm__ uint8_t*)curHardwareTailAddr, 8);
        if ((curHead - *(__gm__ uint32_t*)(curHardwareTailAddr)) < qpDepth - 1) {
            break;
        }
    }

    __gm__ uint8_t* wqeAddr = (__gm__ uint8_t*)(sqBaseAddr + wqeSize * (curHead % qpDepth));
    KERNEL_LOG(KERNEL_INFO, "Hcomm PostSend wqeAddr:%p", wqeAddr);

    // Write the WQE to GM
    uint64_t ownBit = (curHead >> shift) & 1U;
    uint32_t byte_4 = isRead ?
                          static_cast<uint32_t>(HCOMM_OP_TYPE::READ) :
                          static_cast<uint32_t>(HCOMM_OP_TYPE::WRITE); // [0:4] opcode=0x3(RDMA_WRITE), 0x5(RDMA_READ)
    byte_4 |= ((~ownBit) << 7U) & (1U << 7U);                          // [7] owner_bit
    byte_4 |= 1U << 8U;                                                // [8:8] IBV_SEND_SIGNALED

    *(__gm__ uint32_t*)(wqeAddr) = byte_4;         // Control set by local parameter see above lines
    *(__gm__ uint32_t*)(wqeAddr + 4) = len;        // message size
    *(__gm__ uint32_t*)(wqeAddr + 8) = 0;          // immtdata is always 0 till we provide poll CQ flow in AIV
    *(__gm__ uint32_t*)(wqeAddr + 12) = 1U << 24U; // [120:127] num_sge = 1
    *(__gm__ uint32_t*)(wqeAddr + 16) = 0;         // [128:151] start_sge_idx = 0;
    *(__gm__ uint32_t*)(wqeAddr + 20) = channel->remoteBufferAddr[0].bufferInfo.rma.protectionInfo.memInfo.roce.rkey;
    *(__gm__ uint64_t*)(wqeAddr + 24) = (uint64_t)dst; // destination VA

    constexpr uint32_t sgeAddrOffset = 32;
    __gm__ uint8_t* sgeAddr = wqeAddr + sgeAddrOffset;
    KERNEL_LOG(KERNEL_INFO, "Hcomm PostSend sgeAddr:%p", sgeAddr);
    *(__gm__ uint32_t*)(sgeAddr) = len;
    *(__gm__ uint32_t*)(sgeAddr + sizeof(uint32_t)) =
        channel->localBufferAddr[0].bufferInfo.rma.protectionInfo.memInfo.roce.lkey;
    *(__gm__ uint64_t*)(sgeAddr + 2 * sizeof(uint32_t)) = (uint64_t)src; // src VA addr memory registered by RNIC

    constexpr uint32_t wqeAddrWriteLength = 48;
    CacheWriteThrough(wqeAddr, wqeAddrWriteLength);

    curHead++;

    // Post doorbell
    doorBell(channel, curHead);

    ubLocalHead_.SetValue(0, (uint32_t)curHead);
    AscendC::GlobalTensor<uint32_t> HeadGlobalTensor;
    HeadGlobalTensor.SetGlobalBuffer((__gm__ uint32_t*)curHardwareHead);
    AscendC::DataCopyExtParams copyParamsHead{1, 1 * sizeof(uint32_t), 0, 0, 0};
    AscendC::DataCopyPad(HeadGlobalTensor, ubLocalHead_, copyParamsHead);
    KERNEL_LOG(KERNEL_INFO, "Hcomm PostSend finish");
}

} // namespace AscendC

#endif // IMPL_V220_HCOMM_AIV_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HCOMM_AIV_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HCOMM_AIV_H__
#endif
