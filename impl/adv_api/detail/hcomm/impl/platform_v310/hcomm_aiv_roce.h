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
 * \file hcomm_aiv_roce.h
 * \brief Hcomm AIV implementation for V310
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/hcomm/impl/platform_v310/hcomm_aiv_roce.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/simplesoftmax.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HCOMM_AIV_ROCE_H__
#endif

#ifndef IMPL_ADV_API_DETAIL_HCOMM_IMPL_PLATFORM_V310_HCOMM_AIV_ROCE_H
#define IMPL_ADV_API_DETAIL_HCOMM_IMPL_PLATFORM_V310_HCOMM_AIV_ROCE_H

#include "hcomm_aiv_roce_def.h"
#include "../../common/hcomm_utils.h"
#include "../../common/hcomm_inner_def.h"

namespace AscendC {

__aicore__ inline int32_t HcommImpl<COMM_PROTOCOL_ROCE>::Init(__ubuf__ uint8_t* buff, uint32_t len)
{
    if (len < HCOMM_UB_BUF_SIZE) {
        return HCOMM_FAILED;
    }
    __ubuf__ uint8_t* alignedAddr = AlignAddrTo32Bytes(buff);
    TBuffAddr addr;
    addr.logicPos = static_cast<uint8_t>(TPosition::VECOUT);
    addr.dataLen = len;
    addr.bufferAddr = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(alignedAddr));
#if defined(UT_TEST)
    addr.absAddr = reinterpret_cast<uint8_t*>(alignedAddr);
#endif

    wqeUB_.SetAddr(addr);
    cqeUB_ = wqeUB_[ROCE_CQE_POS];
    return HCOMM_SUCCESS;
}

template <typename T>
__aicore__ inline int32_t HcommImpl<COMM_PROTOCOL_ROCE>::Init(const LocalTensor<T>& buff, uint32_t len)
{
    if (len < HCOMM_UB_BUF_SIZE || buff.GetSize() < HCOMM_UB_BUF_SIZE) {
        return HCOMM_FAILED;
    }
    wqeUB_ = buff.template ReinterpretCast<uint8_t>();
    cqeUB_ = wqeUB_[ROCE_CQE_POS];
    return HCOMM_SUCCESS;
}

__aicore__ inline void HcommImpl<COMM_PROTOCOL_ROCE>::FillCtrlSeg(
    __ubuf__ RoceWqeEntry* wqePtr, uint32_t sqHead, uint32_t sqDepth)
{
    uint16_t wfBdsl =
        (uint16_t)(ROCE_1825_WQE_DATA_SEG_BDSL | ((sqHead & ROCE_1825_WQE_MSN_MASK) << ROCE_1825_WQE_MSN_SHIFT));
    wqePtr->ctrl.ownerSl =
        (((sqHead & sqDepth) == 0) ? 0 : (1U << ROCE_1825_WQE_OWNER_SHIFT)) | ROCE_1825_WQE_CTRL_VALUE;
    wqePtr->ctrl.dfTsl = (uint8_t)((1U << ROCE_1825_WQE_CQE_SIGNAL_SHIFT) | ROCE_1825_WQE_VA_VALUE |
                                   (sizeof(RoceWqeTaskSeg) / ROCE_1825_SEG_LEN_UNIT));
    wqePtr->ctrl.wfBdsl = HtoNS(wfBdsl);
    wqePtr->ctrl.clPi = HtoNL(1U << ROCE_1825_WQE_CMP_TASK_LEN_SHIFT);
}

__aicore__ inline void HcommImpl<COMM_PROTOCOL_ROCE>::FillTaskSeg(
    __ubuf__ RoceWqeEntry* wqePtr, GM_ADDR dst, uint64_t len, uint32_t opType, uint32_t rKey, uint32_t lKey)
{
    wqePtr->task.comTask.value = 0;
    wqePtr->task.comTask.bs.signal = 1;
    wqePtr->task.comTask.bs.opType = opType;
    wqePtr->task.comTask.value = HtoNL(wqePtr->task.comTask.value);

    wqePtr->task.dataLen = HtoNL((uint32_t)len);
    wqePtr->task.immData = 0;
    wqePtr->task.dw3.value = 0;
    if (opType == (uint32_t)HCOMM_ROCE_OP_TYPE::READ) {
        wqePtr->task.dw3.bs.lastExtLen = ROCE_1825_RDMA_READ_LAST_EXT_LEN;
        wqePtr->task.dw3.value = HtoNL(wqePtr->task.dw3.value);
    }
    wqePtr->task.vaRemote = HtoNLL((uint64_t)dst);
    wqePtr->task.rKey = HtoNL(rKey);
    wqePtr->task.ulp = HtoNL(lKey & 0xffffU);
}

__aicore__ inline void HcommImpl<COMM_PROTOCOL_ROCE>::FillDataSeg(
    __ubuf__ RoceWqeEntry* wqePtr, GM_ADDR src, uint64_t len, uint32_t lKey)
{
    wqePtr->data.vaLocal = HtoNLL((uint64_t)src);
    wqePtr->data.rLen = HtoNL((uint32_t)len);
    wqePtr->data.leKey = HtoNL((lKey & (~ROCE_1825_WQE_NEXT_SGE_INVALID)) | ROCE_1825_WQE_NEXT_SGE_INVALID);
}

// Pre-mark the next WQEBB owner byte as invalid so the hardware stops there until the following WQE is posted.
__aicore__ inline void HcommImpl<COMM_PROTOCOL_ROCE>::WriteInvalidWqebb(
    __gm__ uint8_t* sqAddr, uint32_t sqHead, uint32_t sqDepth)
{
    __gm__ RoceWqeCtrlSeg* ctrl = (__gm__ RoceWqeCtrlSeg*)sqAddr;
    ctrl->ownerSl = ((sqHead & sqDepth) == 0) ? 0xff : 0x7f;
    CacheWriteThrough<uint8_t>(sqAddr, 1);
}

__aicore__ inline int32_t HcommImpl<COMM_PROTOCOL_ROCE>::MakeWqe(
    __gm__ ChannelEntity* chnlPtr, GM_ADDR dst, GM_ADDR src, uint64_t len, uint32_t opType, uint32_t sqHead,
    uint32_t sqDepth)
{
    int32_t remoteIdx = HcommFindBufferIdx(chnlPtr->remoteBufferAddr, chnlPtr->remoteBufferNum, dst, len);
    if (remoteIdx < 0) {
        KERNEL_LOG(KERNEL_INFO, "Hcomm MakeWqe: failed with invalid remote buffer addr %llu.\n", dst);
        return HCOMM_FAILED;
    }
    int32_t localIdx = HcommFindBufferIdx(chnlPtr->localBufferAddr, chnlPtr->localBufferNum, src, len);
    if (localIdx < 0) {
        KERNEL_LOG(KERNEL_INFO, "Hcomm MakeWqe: failed with invalid local buffer addr %llu.\n", src);
        return HCOMM_FAILED;
    }
    __gm__ uint8_t* sqBaseAddr = (__gm__ uint8_t*)(chnlPtr->sqContextAddr->contextInfo.roceSq.sqVa);
    uint32_t wqeSize = chnlPtr->sqContextAddr->contextInfo.roceSq.wqeSize;
    KERNEL_LOG(KERNEL_INFO, "Hcomm MakeWqe: Get wqeSize from channelEntiry, wqeSize = %u.\n", wqeSize);
    __gm__ uint8_t* sqAddr = (__gm__ uint8_t*)(sqBaseAddr + (sqHead & (sqDepth - 1)) * wqeSize);
    GlobalTensor<uint8_t> sqGlobal;
    sqGlobal.SetGlobalBuffer(sqAddr);

    __ubuf__ RoceWqeEntry* wqePtr = (__ubuf__ RoceWqeEntry*)(wqeUB_.GetPhyAddr());
    FillCtrlSeg(wqePtr, sqHead, sqDepth);
    uint32_t rKey = chnlPtr->remoteBufferAddr[remoteIdx].bufferInfo.rma.protectionInfo.memInfo.roce.rkey;
    uint32_t lKey = chnlPtr->localBufferAddr[localIdx].bufferInfo.rma.protectionInfo.memInfo.roce.lkey;
    FillTaskSeg(wqePtr, dst, len, opType, rKey, lKey);
    FillDataSeg(wqePtr, src, len, lKey);
    __gm__ uint8_t* sqAddrNext = (__gm__ uint8_t*)(sqBaseAddr + ((sqHead + 1) & (sqDepth - 1)) * wqeSize);
    WriteInvalidWqebb(sqAddrNext, (sqHead + 1), sqDepth);

    DataCopy(sqGlobal, wqeUB_, sizeof(RoceWqeEntry));
    SyncAction<HardEvent::MTE3_S>();

    KERNEL_LOG(KERNEL_INFO, "Hcomm MakeWqe: set wqe to qp ok.\n");
    return HCOMM_SUCCESS;
}

template <bool commit, pipe_t commitPipe, pipe_t reqPipe>
__aicore__ inline int32_t HcommImpl<COMM_PROTOCOL_ROCE>::PostSend(
    ChannelHandle channel, GM_ADDR dst, GM_ADDR src, uint64_t len, uint32_t opType)
{
    __gm__ ChannelEntity* chnlPtr = (__gm__ ChannelEntity*)(channel);
    if (chnlPtr == nullptr || chnlPtr->sqNum == 0 || chnlPtr->cqNum == 0 || chnlPtr->sqContextAddr == nullptr ||
        chnlPtr->cqContextAddr == nullptr || dst == nullptr || src == nullptr) {
        KERNEL_LOG(
            KERNEL_INFO, "Hcomm PostSend: chnlPtr = %llu, sqNum = %u, cqNum = %u, dst = %llu, src = %llu.\n", chnlPtr,
            chnlPtr->sqNum, chnlPtr->cqNum, dst, src);
        return HCOMM_FAILED;
    }
    uint32_t sqDepth = chnlPtr->sqContextAddr[0].contextInfo.roceSq.depth;
    uint32_t cqDepth = chnlPtr->cqContextAddr[0].contextInfo.roceCq.cqDepth;
    auto sqPIAddr = reinterpret_cast<__gm__ uint32_t*>(chnlPtr->sqContextAddr[0].contextInfo.roceSq.headAddr);
    uint32_t sqHead = static_cast<uint32_t>(ld_dev(sqPIAddr, 0));
    auto sqCIAddr = reinterpret_cast<__gm__ uint32_t*>(chnlPtr->sqContextAddr[0].contextInfo.roceSq.tailAddr);
    uint32_t sqTail = static_cast<uint32_t>(ld_dev(sqCIAddr, 0));
    KERNEL_LOG(
        KERNEL_INFO, "Hcomm PostSend: opType = %u, sqHead = %u, sqTail = %u, sqDepth = %u, cqDepth = %u.\n", opType,
        sqHead, sqTail, sqDepth, cqDepth);

    constexpr uint32_t POLL_CQ_THRESHOLD = 10;
    constexpr uint32_t NUM_CQE_PER_POLL_CQ = 100;
    uint32_t minDepth = (sqDepth <= cqDepth) ? sqDepth : cqDepth;
    if ((sqHead + POLL_CQ_THRESHOLD) % minDepth == sqTail % minDepth) {
        uint32_t idx = (sqTail + NUM_CQE_PER_POLL_CQ) > sqHead ? sqHead : (sqTail + NUM_CQE_PER_POLL_CQ);
        KERNEL_LOG(
            KERNEL_INFO, "Hcomm PostSend: RoCE SQ overflow sqHead=%u sqTail=%u idx=%u minDepth=%u\n", sqHead, sqTail,
            idx, minDepth);
        if (PollCq(chnlPtr, idx) != HCOMM_SUCCESS) {
            KERNEL_LOG(KERNEL_INFO, "Hcomm PostSend: RoCE SQ overflow, PollCq failed.\n");
            return HCOMM_FAILED;
        }
    }

    if (MakeWqe(chnlPtr, dst, src, len, opType, sqHead, sqDepth) != HCOMM_SUCCESS) {
        KERNEL_LOG(KERNEL_INFO, "Hcomm PostSend: MakeWqe failed.\n");
        return HCOMM_FAILED;
    }

    sqHead++;
    st_dev(sqHead, sqPIAddr, 0);
    KERNEL_LOG(KERNEL_INFO, "Hcomm PostSend: update PI to GM sqHead = %u\n", sqHead);

    if constexpr (commit) {
        KnockDoorBell(chnlPtr, sqHead);
        KERNEL_LOG(KERNEL_INFO, "Hcomm PostSend: Commit ok.\n");
    }
    return HCOMM_SUCCESS;
}

template <bool commit, pipe_t commitPipe, pipe_t reqPipe, auto const& config>
__aicore__ inline int32_t HcommImpl<COMM_PROTOCOL_ROCE>::WriteNbi(
    ChannelHandle channel, GM_ADDR dst, GM_ADDR src, uint64_t len)
{
    (void)config;
    return PostSend<commit, commitPipe, reqPipe>(
        channel, dst, src, len, static_cast<uint32_t>(HCOMM_ROCE_OP_TYPE::WRITE));
}

template <bool commit, pipe_t commitPipe, pipe_t reqPipe, auto const& config>
__aicore__ inline int32_t HcommImpl<COMM_PROTOCOL_ROCE>::ReadNbi(
    ChannelHandle channel, GM_ADDR dst, GM_ADDR src, uint64_t len)
{
    (void)config;
    return PostSend<commit, commitPipe, reqPipe>(
        channel, src, dst, len, static_cast<uint32_t>(HCOMM_ROCE_OP_TYPE::READ));
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
    KERNEL_LOG(KERNEL_ERROR, "Hcomm ROCE WriteWithNotifyNbi is not supported.");
    return HCOMM_FAILED;
}

__aicore__ inline uint64_t HcommImpl<COMM_PROTOCOL_ROCE>::GetDbValue(uint32_t qpn, uint8_t mtuShift)
{
    RoceDbEntry dbEntry;
    dbEntry.dw0.value = 0;
    dbEntry.dw0.bs.c = 0;
    dbEntry.dw0.bs.r = 0;
    dbEntry.dw0.bs.ctxSize = 1;
    dbEntry.dw0.bs.qpn = qpn;
    dbEntry.dw0.bs.subType = 0;
    dbEntry.dw0.bs.resv = 0;
    dbEntry.dw0.bs.pi = 0;
    dbEntry.dw0.bs.sgidIdx = ROCE_1825_SQ_DB_SGIT_IDX;
    dbEntry.dw0.bs.type = ROCE_1825_SQ_DB_TYPE;
    dbEntry.dw0.bs.mtuShift = mtuShift;
    dbEntry.dw0.bs.cos = ROCE_1825_SQ_DB_COS;
    dbEntry.dw0.bs.xrcVld = 0;
    return dbEntry.dw0.value;
}

__aicore__ inline void HcommImpl<COMM_PROTOCOL_ROCE>::KnockDoorBell(__gm__ ChannelEntity* chnlPtr, uint32_t sqHead)
{
    __gm__ uint32_t* dbSwAddr = reinterpret_cast<__gm__ uint32_t*>(chnlPtr->sqContextAddr->contextInfo.roceSq.dbSwVa);
    st_dev(HtoNL(sqHead), dbSwAddr, 0);
    KERNEL_LOG(KERNEL_INFO, "Hcomm KnockDoorBell: write sw db ok, swDbVal = %u\n", sqHead);

    uint64_t dbValue =
        GetDbValue(chnlPtr->sqContextAddr->contextInfo.roceSq.qpn, chnlPtr->sqContextAddr->contextInfo.roceSq.mtuShift);
    KERNEL_LOG(KERNEL_INFO, "Hcomm KnockDoorBell: dbValue = %llu\n", dbValue);
    __gm__ uint64_t* dbHwAddr = reinterpret_cast<__gm__ uint64_t*>(chnlPtr->sqContextAddr->contextInfo.roceSq.dbHwVa);
    uint64_t dbFinalVal =
        dbValue | ((((uint64_t)(sqHead) >> ROCE_1825_SQ_DB_PI_HIGH_SHIFT) & 0xffULL) << ROCE_1825_SQ_DB_PI_FIELD_SHIFT);
    st_dev(dbFinalVal, dbHwAddr, 0);
    KERNEL_LOG(KERNEL_INFO, "Hcomm KnockDoorBell: write hw db ok, hwDbVal = %llu\n", dbFinalVal);
}

template <pipe_t pipe>
__aicore__ inline int32_t HcommImpl<COMM_PROTOCOL_ROCE>::Commit(ChannelHandle channel)
{
    KERNEL_LOG(KERNEL_INFO, "Hcomm Commit: Enter\n");
    __gm__ ChannelEntity* chnlPtr = (__gm__ ChannelEntity*)(channel);
    auto sqPIAddr = reinterpret_cast<__gm__ uint32_t*>(chnlPtr->sqContextAddr->contextInfo.roceSq.headAddr);
    uint32_t sqHead = static_cast<uint32_t>(ld_dev(sqPIAddr, 0));
    KERNEL_LOG(KERNEL_INFO, "Hcomm Commit: sqHead = %u\n", sqHead);

    KnockDoorBell(chnlPtr, sqHead);
    KERNEL_LOG(KERNEL_INFO, "Hcomm Commit: Exit ok.\n");
    return HCOMM_SUCCESS;
}

__aicore__ inline bool HcommImpl<COMM_PROTOCOL_ROCE>::CheckCqeOwner(
    __ubuf__ RoceCqeEntry* cqePtr, uint32_t cqTail, uint32_t depth)
{
    uint32_t curOwner = ((cqePtr->ownerIdQpn & (1U << ROCE_1825_CQE_OWNER_SHIFT)) != 0);
    uint32_t expectOwner = (uint32_t)(((cqTail & depth) == 0));
    return (expectOwner ^ curOwner) != 0;
}

__aicore__ inline int32_t HcommImpl<COMM_PROTOCOL_ROCE>::PollCq(__gm__ ChannelEntity* chnlPtr, uint32_t expectIdx)
{
    if (expectIdx == 0) {
        return HCOMM_SUCCESS;
    }
    auto cqContextInfo = chnlPtr->cqContextAddr[0].contextInfo;
    uint32_t cqeSize = cqContextInfo.roceCq.cqeSize;
    uint32_t cqDepth = cqContextInfo.roceCq.cqDepth;
    auto cqCIAddr = reinterpret_cast<__gm__ uint32_t*>(cqContextInfo.roceCq.tailAddr);
    uint32_t cqTail = static_cast<uint32_t>(ld_dev(cqCIAddr, 0));
    KERNEL_LOG(
        KERNEL_INFO, "Hcomm PollCq: cqeSize = %u cqDepth = %u cqTail= %u expectIdx = %u\n", cqeSize, cqDepth, cqTail,
        expectIdx);
    __ubuf__ RoceCqeEntry* cqePtr = (__ubuf__ RoceCqeEntry*)(cqeUB_.GetPhyAddr());
    __gm__ uint8_t* cqBaseBuf = (__gm__ uint8_t*)(cqContextInfo.roceCq.cqVa);
    AscendC::GlobalTensor<uint8_t> cqeGlobalTensor;

    while (cqTail < expectIdx) {
        __gm__ uint8_t* cqeAddr = (__gm__ uint8_t*)(cqBaseBuf + cqeSize * (cqTail & (cqDepth - 1)));
        cqeGlobalTensor.SetGlobalBuffer(cqeAddr);
        uint32_t loop = 0;
        uint32_t cqeType = ROCE_1825_CQE_OPTYPE_INVALID;
        for (; loop < HCOMM_POLLCQ_MAX_RETRY_TIMES; loop++) {
            DataCopy(cqeUB_, cqeGlobalTensor, sizeof(RoceCqeEntry));
            SyncAction<HardEvent::MTE2_S>();
#if defined(UT_TEST)
            cqTail = expectIdx - 1;
            break;
#else
            cqeType = (cqePtr->opSrWqebb >> ROCE_1825_CQE_OPCODE_SHIFT) & ROCE_1825_CQE_OPCODE_MASK;
            KERNEL_LOG(KERNEL_INFO, "Hcomm PollCq: cqeType = %u\n", cqeType);
            if (cqeType != ROCE_1825_CQE_OPTYPE_INVALID && CheckCqeOwner(cqePtr, cqTail, cqDepth)) {
                break;
            }
#endif
        }
        if (loop >= HCOMM_POLLCQ_MAX_RETRY_TIMES) {
            KERNEL_LOG(KERNEL_INFO, "Hcomm PollCq: failed, overtime and exit.\n");
            return HCOMM_FAILED;
        }
        if (cqeType == ROCE_1825_CQE_OPTYPE_ERROR) {
            KERNEL_LOG(
                KERNEL_INFO, "Hcomm PollCq: failed, syndrome = 0x%x, qpn = %u, cqTail = %u\n", cqePtr->syndrome,
                cqePtr->ownerIdQpn & 0xfffffU, cqTail);
            return HCOMM_FAILED;
        }
        cqTail += 1;
        KERNEL_LOG(KERNEL_INFO, "Hcomm PollCq: cqTail = %u\n", cqTail);
    }
    st_dev(cqTail, cqCIAddr, 0);
    auto sqCIAddr = reinterpret_cast<__gm__ uint32_t*>(chnlPtr->sqContextAddr[0].contextInfo.roceSq.tailAddr);
    st_dev(cqTail, sqCIAddr, 0);
    __gm__ uint32_t* dbSwAddr = reinterpret_cast<__gm__ uint32_t*>(cqContextInfo.roceCq.dbSwVa);
    st_dev(cqTail & ROCE_1825_CQE_UPDATE_CI_MASK, dbSwAddr, 0);
    KERNEL_LOG(KERNEL_INFO, "Hcomm PollCq: knock cq doorbell ok, cqTail = %u, sqTail = %u\n", cqTail, cqTail);
    return HCOMM_SUCCESS;
}

template <pipe_t pipe>
__aicore__ inline int32_t HcommImpl<COMM_PROTOCOL_ROCE>::Drain(ChannelHandle channel)
{
    KERNEL_LOG(KERNEL_INFO, "Hcomm Drain: Enter\n");
    __gm__ ChannelEntity* chnlPtr = (__gm__ ChannelEntity*)(channel);
    auto sqPIAddr = reinterpret_cast<__gm__ uint32_t*>(chnlPtr->sqContextAddr[0].contextInfo.roceSq.headAddr);
    uint32_t sqHead = static_cast<uint32_t>(ld_dev(sqPIAddr, 0));
    KERNEL_LOG(KERNEL_INFO, "Hcomm Drain: sqHead = %u\n", sqHead);
    if (PollCq(chnlPtr, sqHead) != HCOMM_SUCCESS) {
        KERNEL_LOG(KERNEL_INFO, "Hcomm Drain: PollCq failed.\n");
        return HCOMM_FAILED;
    }
    KERNEL_LOG(KERNEL_INFO, "Hcomm Drain: Exit ok.\n");
    return HCOMM_SUCCESS;
}
} // namespace AscendC

#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HCOMM_AIV_ROCE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HCOMM_AIV_ROCE_H__
#endif
