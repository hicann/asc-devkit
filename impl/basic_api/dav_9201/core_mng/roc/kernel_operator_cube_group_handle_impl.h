/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file kernel_operator_cube_group_handle_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_9201/core_mng/roc/kernel_operator_cube_group_handle_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_CUBE_GROUP_H__ANDLE_IMPL_H
#endif
#ifndef ASCENDC_MODULE_OPERATOR_CUBE_GROUP_HANDLE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_CUBE_GROUP_HANDLE_IMPL_H
#include "../../../../../include/basic_api/core_mng/roc/kernel_operator_cube_group_intf.h"
namespace AscendC {
__aicore__ inline KfcWorkspace::KfcWorkspace(GM_ADDR workspace)
{
    msgStart = workspace;
    if ASCEND_IS_AIV {
        evtID = GetTPipePtr()->AllocEventID<HardEvent::MTE3_S>();
        SetFlag<HardEvent::MTE3_S>(evtID);
    }
}

__aicore__ inline void KfcWorkspace::UpdateKfcWorkspace(uint32_t offset) { msgStart += offset; }

__aicore__ inline GM_ADDR KfcWorkspace::GetKfcWorkspace() { return msgStart; }

__aicore__ inline KfcWorkspace::~KfcWorkspace()
{
    if ASCEND_IS_AIV {
        WaitFlag<HardEvent::MTE3_S>(evtID);
        GetTPipePtr()->ReleaseEventID<HardEvent::MTE3_S>(evtID);
    }
}

__aicore__ inline uint8_t GetEventId(KfcWorkspace& kfcWorkspace) { return kfcWorkspace.evtID; }

template <typename CubeMsgType>
__aicore__ inline CubeResGroupHandle<CubeMsgType>::CubeResGroupHandle(
    GM_ADDR workspace, uint8_t blockStart, uint8_t blockSize, uint8_t msgQueueSize, uint8_t evtIDIn)
{
    ASCENDC_DEBUG_ASSERT(
        ((blockStart % MIX_NUM) == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "blockStart is %u, must be even number.", blockStart));
    ASCENDC_DEBUG_ASSERT(
        ((blockSize % MIX_NUM) == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "blockSize is %u, must be even number.", blockSize));
    aicSize = blockSize / MIX_NUM;
    aivSize = msgQueueSize;

    aivPerAic = Ceil(aivSize, aicSize);
    int8_t aivInLastAic = aivSize - (aicSize - 1) * aivPerAic;
    ASCENDC_DEBUG_ASSERT((aivInLastAic > 0), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "AIV num in last AIC must be positive"));
    // aic update config，aivNumForAic in aiv need to be updated by AssignQueue
    aivNumForCurAic = (GetBlockIdxImpl() == blockStart / MIX_NUM + aicSize - 1) ? aivInLastAic : aivPerAic;
    aivWorkState = (static_cast<uint64_t>(1) << aivNumForCurAic) - 1;

    msgSize = aivPerAic * aicSize * MAX_MSG_PER_AIV;
    msgHead = (__gm__ CubeMsgType*)__GetAicMsgHead(workspace, blockStart / MIX_NUM, aivPerAic * MAX_MSG_PER_AIV);
    msgCurrent = msgHead;
    if ASCEND_IS_AIV {
        eventID = evtIDIn;
        uint32_t ubMsgAddr = TOTAL_UB_SIZE - ONE_BLK_SIZE * AIV_CORE_NUM - sizeof(CubeMsgType);

#if ASCENDC_CPU_DEBUG
        ubMsg =
            reinterpret_cast<__ubuf__ CubeMsgType*>(GetTPipePtr()->GetBaseAddr((int8_t)TPosition::VECOUT) + ubMsgAddr);
#else
        ubMsg = reinterpret_cast<__ubuf__ CubeMsgType*>(ubMsgAddr);
#endif
    }
}

// aiv call: update aivNumForCurAic + msgHead + msgCurrent by aic, aiv index
// queueIdIn : index of msgque in in this cubegrouphandle aicSubgroupID: index of aic in this cubegrouphandle
// aivSubGroupID: index of aiv among aivs for that aic
template <typename CubeMsgType>
__aicore__ inline void CubeResGroupHandle<CubeMsgType>::AssignQueue(uint8_t queueIdIn)
{
    ASCENDC_DEBUG_ASSERT(
        (queueIdIn < aivSize),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "queueID (%u) must be less than msgQueueSize(%u)", queueIdIn, aivSize));
    if ASCEND_IS_AIV {
        uint8_t aicSubgroupID = queueIdIn / aivPerAic;
        uint8_t aivSubgroupID = queueIdIn % aivPerAic;
        queueId = queueIdIn;
        ASCENDC_DEBUG_ASSERT(
            (aicSubgroupID < aicSize),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "aicSubgroupID (%u) must be less than aicSize (%u)", aicSubgroupID, aicSize));
        if (aicSubgroupID != aicSize - 1) {
            aivNumForCurAic = aivPerAic;
        } else { // if last aic, aiv num = total - sum(aivs for previous aics)
            aivNumForCurAic = aivSize - (aicSize - 1) * aivPerAic;
        }
        ASCENDC_DEBUG_ASSERT(
            (aivSubgroupID < aivNumForCurAic),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "aivSubgroupID (%u) must be less than aivNumForCurAic (%u)", aivSubgroupID,
                aivNumForCurAic));
        msgHead += aicSubgroupID * aivPerAic * MAX_MSG_PER_AIV + aivSubgroupID; // set msgHead and no more changes
        msgCurrent = msgHead;
    }
}

// aiv call: check msgState of msgCurrent is FREE, can allocate only when state is FREE
// to guarantee the order, not allowed to skip, must wait until msgState is updated to FREE
template <typename CubeMsgType>
template <PipeMode pipeMode>
__aicore__ inline __gm__ CubeMsgType* CubeResGroupHandle<CubeMsgType>::AllocMessage()
{
    if ASCEND_IS_AIV {
        if constexpr (pipeMode == PipeMode::MTE3_MODE) {
            WaitFlag<HardEvent::MTE3_S>((event_t)eventID);
        }
        dcci(reinterpret_cast<__gm__ int64_t*>(msgCurrent), cache_line_t::SINGLE_CACHE_LINE, dcci_dst_t::CACHELINE_OUT);
        while ((msgCurrent->head).msgState != CubeMsgState::FREE) {
            dcci(
                reinterpret_cast<__gm__ int64_t*>(msgCurrent), cache_line_t::SINGLE_CACHE_LINE,
                dcci_dst_t::CACHELINE_OUT);
        }
        __gm__ CubeMsgType* msgReturn = msgCurrent;
        __AivUpdateMsgCurrent();
        return msgReturn;
    }
    return msgCurrent;
}

// aiv call: use dcci to send msg. return offset, the msg position is msgHead + offset
template <typename CubeMsgType>
template <PipeMode pipeMode>
__aicore__ inline uint16_t CubeResGroupHandle<CubeMsgType>::PostMessage(__gm__ CubeMsgType* msg, CubeMsgType& msgInput)
{
    if ASCEND_IS_AIV {
        if constexpr (pipeMode == PipeMode::SCALAR_MODE) {
            __CopyCubeMsg(msg, msgInput);
            __WriteGmCubeMsgByScalar(msg);
        } else if constexpr (pipeMode == PipeMode::MTE3_MODE) {
            __WriteGmCubeMsgByDatacopy(msg, msgInput);
            SetFlag<HardEvent::MTE3_S>((event_t)eventID);
        } else {
            ASCENDC_DEBUG_ASSERT(
                false, KERNEL_LOG_INTERNAL(KERNEL_ERROR, "PostMessage Mode only support SCALAR_MODE or MTE3_MODE"));
        }
    }
    return msg - msgHead;
}

// aic call: set msgState to FREE without waiting
template <typename CubeMsgType>
__aicore__ inline uint16_t CubeResGroupHandle<CubeMsgType>::FreeMessage(__gm__ CubeMsgType* msg)
{
    if ASCEND_IS_AIC {
        __WriteGmStateByScalar(msg, CubeMsgState::FREE);
    }
    return msg - msgHead;
}

// aic call: wait until aiv update msgState to waitState, then aic set state to FREE. Used in FAKE msg.
template <typename CubeMsgType>
__aicore__ inline uint16_t CubeResGroupHandle<CubeMsgType>::FreeMessage(__gm__ CubeMsgType* msg, CubeMsgState waitState)
{
    if ASCEND_IS_AIC {
        dcci(reinterpret_cast<__gm__ int64_t*>(msg), cache_line_t::SINGLE_CACHE_LINE, dcci_dst_t::CACHELINE_OUT);
        while ((msg->head).msgState != waitState) {
            dcci(reinterpret_cast<__gm__ int64_t*>(msg), cache_line_t::SINGLE_CACHE_LINE, dcci_dst_t::CACHELINE_OUT);
        }
        __WriteGmStateByScalar(msg, CubeMsgState::FREE);
    }
    return msg - msgHead;
}

// aiv call: set msgState to FAKE. Then aic can tell status by updating msgState
template <typename CubeMsgType>
__aicore__ inline uint16_t CubeResGroupHandle<CubeMsgType>::PostFakeMsg(__gm__ CubeMsgType* msg)
{
    if ASCEND_IS_AIV {
        __WriteGmStateByScalar(msg, CubeMsgState::FAKE);
    }
    return msg - msgHead;
}

// aiv call: send msg to tell aic to update aivWorkState cause one aiv ends service
template <typename CubeMsgType>
__aicore__ inline void CubeResGroupHandle<CubeMsgType>::SetQuit(__gm__ CubeMsgType* msg)
{
    uint8_t aivID = queueId % aivPerAic;
    if ASCEND_IS_AIV {
        ASCENDC_DEBUG_ASSERT(
            (aivID < aivNumForCurAic),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "In SendQuitMsg, aivID (%u) should not be larger than aivNumForCurAic (%u)", aivID,
                aivNumForCurAic));
        (msg->head).aivID = aivID;
        __WriteGmStateByScalar(msg, CubeMsgState::QUIT);
    }
}

// aic move msgPos + msgCurrent due to skipCnt. do not allow skip to next row.
// Ex: 1 row has aiv 0~4, current in aiv1. allow skipCnt=3, next pos is aiv0 of next row. Not allow skipCnt=4
template <typename CubeMsgType>
__aicore__ inline void CubeResGroupHandle<CubeMsgType>::SetSkipMsg(uint8_t skipCnt)
{
    if ASCEND_IS_AIC {
        aivWork += skipCnt;
        ASCENDC_DEBUG_ASSERT(
            (aivWork < aivNumForCurAic),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "aivWork + skipCnt(%u) is %u, it cannot be larger than aivNumForCurAic (%u)", skipCnt,
                aivWork, aivNumForCurAic));
        msgPos += skipCnt;
    }
}

// aiv call: sync = true: wait until aic finish task and set state to FREE  sync = false: check if aic finish task
// return true: aic has finished the task      false: aic has not finished yet
template <typename CubeMsgType>
template <bool sync>
__aicore__ inline bool CubeResGroupHandle<CubeMsgType>::Wait(uint16_t offset)
{
    if ASCEND_IS_AIV {
        __gm__ CubeMsgType* cubeMsgCur = msgHead + offset;
        dcci(reinterpret_cast<__gm__ int64_t*>(cubeMsgCur), cache_line_t::SINGLE_CACHE_LINE, dcci_dst_t::CACHELINE_OUT);
        if constexpr (sync) {
            while ((cubeMsgCur->head).msgState != CubeMsgState::FREE) {
                dcci(
                    reinterpret_cast<__gm__ int64_t*>(cubeMsgCur), cache_line_t::SINGLE_CACHE_LINE,
                    dcci_dst_t::CACHELINE_OUT);
            }
            return true;
        } else {
            return (cubeMsgCur->head).msgState == CubeMsgState::FREE;
        }
    }
    return true;
}

template <typename T>
__aicore__ inline bool __IsRun(T handle)
{
    if ASCEND_IS_AIC {
        return handle.aivWorkState;
    }
    return false;
};

template <typename T>
__aicore__ inline uint32_t __GetMsgAreaLen(T handle, uint32_t sizeOfCubeMsgStruct)
{
    return handle.msgSize * sizeOfCubeMsgStruct;
}

// aic call: set index aivID in aivWorkState to 0
template <typename T>
__aicore__ inline void __SetAivQuit(T* handle, uint8_t aivID)
{
    if ASCEND_IS_AIC {
        uint64_t mask = ~(static_cast<uint64_t>(1) << aivID);
        handle->aivWorkState &= mask;
    }
}

// aic call: update msgCurrent to next available pos(skipCnt + not quit)
template <typename T>
__aicore__ inline void __ReleaseMessage(T* handle)
{
    if ASCEND_IS_AIC {
        if (handle->aivWork + 1 >= handle->aivNumForCurAic) { // to next loop, which starts from aiv0
            uint8_t lineCnt = handle->msgPos / handle->aivPerAic;
            handle->msgPos = (lineCnt == MAX_MSG_PER_AIV - 1) ?
                                 0 :
                                 handle->msgPos + (handle->aivPerAic - handle->aivNumForCurAic) + 1;
            handle->aivWork = 0;
        } else {
            handle->msgPos += 1;
            handle->aivWork += 1;
        }
        handle->msgCurrent = handle->msgHead + handle->msgPos;
        if (handle->aivWorkState == 0) { // if all aiv end service, aic quit
            return;
        }
        while (!(handle->aivWorkState & (static_cast<uint64_t>(1) << handle->aivWork))) {
            if (handle->aivWork + 1 >= handle->aivNumForCurAic) {
                uint8_t lineCnt = handle->msgPos / handle->aivPerAic;
                handle->msgPos = (lineCnt == MAX_MSG_PER_AIV - 1) ?
                                     0 :
                                     handle->msgPos + (handle->aivPerAic - handle->aivNumForCurAic) + 1;
                handle->aivWork = 0;
            } else {
                handle->msgPos += 1;
                handle->aivWork += 1;
            }
            handle->msgCurrent = handle->msgHead + handle->msgPos;
        }
    }
}

template <typename CubeMsgType>
__aicore__ inline __gm__ CubeMsgType* __RcvMessage(CubeResGroupHandle<CubeMsgType> handle)
{
    if ASCEND_IS_AIC {
        dcci(
            reinterpret_cast<__gm__ int64_t*>(handle.msgCurrent), cache_line_t::SINGLE_CACHE_LINE,
            dcci_dst_t::CACHELINE_OUT);
        while ((handle.msgCurrent->head).msgState == CubeMsgState::FREE) {
            dcci(
                reinterpret_cast<__gm__ int64_t*>(handle.msgCurrent), cache_line_t::SINGLE_CACHE_LINE,
                dcci_dst_t::CACHELINE_OUT);
        }
    }
    return handle.msgCurrent;
}

// return the msgHead of this aic
template <typename CubeMsgType>
__aicore__ inline GM_ADDR CubeResGroupHandle<CubeMsgType>::__GetAicMsgHead(
    GM_ADDR workspace, uint8_t aicStart, uint16_t msgSizePerAic)
{
    if ASCEND_IS_AIC {
        uint8_t aicIndex = GetBlockIdxImpl() - aicStart;
        auto ptr = reinterpret_cast<__gm__ CubeMsgType*>(workspace);
        return reinterpret_cast<GM_ADDR>(&ptr[aicIndex * msgSizePerAic]);
    }
    return workspace;
}

// aiv update msgPos + msgCurrent to next place
template <typename CubeMsgType>
__aicore__ inline void CubeResGroupHandle<CubeMsgType>::__AivUpdateMsgCurrent()
{
    msgPos = (msgPos == 3) ? 0 : msgPos + 1; // = 3 means reach end of column, need jump to head
    msgCurrent = msgHead + msgPos * aivPerAic;
}

// aic update msgPos + msgCurrent + aivWork to next place
template <typename CubeMsgType>
__aicore__ inline void CubeResGroupHandle<CubeMsgType>::__AicUpdateMsgCurrent()
{
    if (aivWork + 1 >= aivNumForCurAic) { // to next loop, which starts from aiv0
        uint8_t lineCnt = msgPos / aivPerAic;
        msgPos = (lineCnt == MAX_MSG_PER_AIV - 1) ? 0 : msgPos + (aivPerAic - aivNumForCurAic) + 1;
        aivWork = 0;
    } else {
        msgPos += 1;
        aivWork += 1;
    }
    msgCurrent = msgHead + msgPos;
}

// true: aiv run   false: aiv quit
template <typename CubeMsgType>
__aicore__ inline bool CubeResGroupHandle<CubeMsgType>::__AivIsRun(uint8_t aivID)
{
    return aivWorkState & (static_cast<uint64_t>(1) << aivID);
}

// msg data has been prepared. Refresh Scalar of whole CubeMsgType
template <typename CubeMsgType>
__aicore__ inline void CubeResGroupHandle<CubeMsgType>::__WriteGmCubeMsgByScalar(__gm__ CubeMsgType* msg)
{
    ASCENDC_DEBUG_ASSERT(
        (msg != nullptr), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "In __WriteGmStateByScalar, msg can not be nullptr"));
    // Note: must first refresh last (n-1) * 64B msg content, then refresh first 64B of msgState
    for (uint32_t i = 1; i < sizeof(CubeMsgType) / sizeof(int64_t); i++) {
        dcci(
            reinterpret_cast<__gm__ int64_t*>(msg) + sizeof(int64_t) * i, cache_line_t::SINGLE_CACHE_LINE,
            dcci_dst_t::CACHELINE_OUT);
    }
    dcci(reinterpret_cast<__gm__ int64_t*>(msg), cache_line_t::SINGLE_CACHE_LINE, dcci_dst_t::CACHELINE_OUT);
}

// set msgState to newState by Scalar
template <typename CubeMsgType>
__aicore__ inline void CubeResGroupHandle<CubeMsgType>::__WriteGmStateByScalar(
    __gm__ CubeMsgType* msg, CubeMsgState newState)
{
    ASCENDC_DEBUG_ASSERT(
        (msg != nullptr), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "In __WriteGmStateByScalar, msg can not be nullptr"));
    (msg->head).msgState = newState;
    dcci(reinterpret_cast<__gm__ int64_t*>(msg), cache_line_t::SINGLE_CACHE_LINE, dcci_dst_t::CACHELINE_OUT);
}

template <typename CubeMsgType>
__aicore__ inline void CubeResGroupHandle<CubeMsgType>::__WriteGmCubeMsgByDatacopy(
    __gm__ CubeMsgType* msgPtr, CubeMsgType& cubeMsgInput)
{
    ASCENDC_DEBUG_ASSERT(
        (msgPtr != nullptr),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "In __WriteGmCubeMsgByDatacopy, msg can not be nullptr"));
    auto ubData = reinterpret_cast<__ubuf__ uint64_t*>(ubMsg);
    auto msgData = reinterpret_cast<uint64_t*>(&cubeMsgInput);
    for (uint32_t i = 0; i < sizeof(CubeMsgType) / sizeof(uint64_t); i++, ubData++, msgData++) {
        *ubData = *msgData;
    }
    event_t evtID = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_MTE3));
    SetFlag<HardEvent::S_MTE3>(evtID);
    WaitFlag<HardEvent::S_MTE3>(evtID);
    PipeBarrier<PIPE_MTE3>();
    copy_ubuf_to_gm((__gm__ void*)msgPtr, (__ubuf__ void*)ubMsg, 0, 1, sizeof(CubeMsgType) / ONE_BLK_SIZE, 0, 0);
}

// copy msgInput to GM
template <typename CubeMsgType>
__aicore__ inline void CubeResGroupHandle<CubeMsgType>::__CopyCubeMsg(__gm__ CubeMsgType* msg, CubeMsgType& msgInput)
{
    auto gmPtr = reinterpret_cast<__gm__ uint64_t*>(msg);
    auto msgDataPtr = reinterpret_cast<uint64_t*>(&msgInput);
    for (uint32_t i = 0; i < sizeof(CubeMsgType) / sizeof(int64_t); i++, gmPtr++, msgDataPtr++) {
        *gmPtr = *msgDataPtr;
    }
}

template <int groupID, class MatmulApiType, template <class, class> class CallBack, typename CubeMsgType>
__aicore__ inline CubeResGroupHandle<CubeMsgType> CreateCubeResGroup(
    KfcWorkspace& desc, uint8_t blockStart, uint8_t blockSize, uint8_t msgQueueSize, GM_ADDR tiling)
{
    ASCENDC_DEBUG_ASSERT(
        (sizeof(CubeMsgType) % 64 == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "CubeMsgType Size is %u must be aligned to 64B.", sizeof(CubeMsgType)));
    CubeResGroupHandle handle =
        CubeResGroupHandle<CubeMsgType>(desc.GetKfcWorkspace(), blockStart, blockSize, msgQueueSize, GetEventId(desc));
    desc.UpdateKfcWorkspace(__GetMsgAreaLen(handle, sizeof(CubeMsgType)));

    if ASCEND_IS_AIV {
        return handle;
    }

    // only when aic belongs to this CubeGroupHandle, run the while loop process
    auto aicId = GetBlockIdxImpl();
    if ((aicId < blockStart / MIX_NUM) || (aicId >= (blockStart / MIX_NUM + blockSize / MIX_NUM))) {
        return handle;
    }
    MatmulApiType mm;
    CallBack<MatmulApiType, CubeMsgType> obj;
    obj.Init(obj, mm, tiling);
    while (__IsRun(handle)) {
        auto rcvMsg = __RcvMessage(handle);
        if ((rcvMsg->head).msgState == CubeMsgState::QUIT) {
            __SetAivQuit(&handle, (rcvMsg->head).aivID);
        } else {
            obj.Call(mm, rcvMsg, handle);
        }
        __ReleaseMessage(&handle);
    }
    return handle;
}

} // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_CUBE_GROUP_H__ANDLE_IMPL_H)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_CUBE_GROUP_H__ANDLE_IMPL_H
#endif
