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
 * \file kernel_operator_group_barrier_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_9201/core_mng/roc/kernel_operator_group_barrier_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_GROUP_BARRIER_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_GROUP_BARRIER_IMPL_H
#define ASCENDC_MODULE_OPERATOR_GROUP_BARRIER_IMPL_H
#include "../../../../../include/basic_api/core_mng/roc/kernel_operator_group_barrier_intf.h"
#include "../../../kernel_process_lock.h"
#include "../../kernel_operator_data_copy_impl.h"
namespace AscendC {
__aicore__ inline int64_t GetBlockNum();
template <PipeMode pipeMode>
__aicore__ inline GroupBarrier<pipeMode>::GroupBarrier(
    GM_ADDR groupWorkspace, uint32_t arriveSizeIn, uint32_t waitSizeIn)
{
    if ASCEND_IS_AIV {
        ASCENDC_DEBUG_ASSERT(
            (pipeMode == PipeMode::MTE3_MODE),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "Currently GroupBarrier only support PipeMode::MTE3_MODE"));
        ASCENDC_DEBUG_ASSERT(
            (arriveSizeIn > 0),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "arriveSizeIn is %u, which should be larger than 0", arriveSizeIn));
        ASCENDC_DEBUG_ASSERT(
            (waitSizeIn > 0),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "waitSizeIn is %u, which should be larger than 0", waitSizeIn));
        ASCENDC_DEBUG_ASSERT(
            (waitSizeIn <= GetBlockNum()),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "waitSizeIn %u is larger than max waitSize is %lld \n", waitSizeIn, GetBlockNum()));
        ASCENDC_DEBUG_ASSERT(
            (arriveSizeIn <= GetBlockNum()),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "waitSize is %u is larger than max arriveSize is %lld \n", arriveSizeIn, GetBlockNum()));
        this->barrierInfoArrive = reinterpret_cast<__gm__ BarrierInfo*>(groupWorkspace);
        this->barrierInfoWait = reinterpret_cast<__gm__ BarrierInfo*>(groupWorkspace + BARRIER_SIZE);
        this->arriveSize = arriveSizeIn;
        this->waitSize = waitSizeIn;
        this->counter = 1;
        this->hasArrive = false;

        // worst case: max aiv wait max aiv, thus need at least max aiv block which first element is 1 to atomic add
#if ASCENDC_CPU_DEBUG
        __ubuf__ int32_t* dst = reinterpret_cast<__ubuf__ int32_t*>(
            GetTPipePtr()->GetBaseAddr(static_cast<uint8_t>(TPosition::VECOUT)) + UB_START_ADDR);
#else
        __ubuf__ int32_t* dst = reinterpret_cast<__ubuf__ int32_t*>(UB_START_ADDR);
#endif
        for (uint32_t i = 0; i < BARRIER_MAX_AIV; i++) {
            *(__ubuf__ uint32_t*)(dst + DEFAULT_BLK_NUM * i) = 1; // Set first element in each block to be 1
        }
    }
}

template <PipeMode pipeMode>
__aicore__ inline void GroupBarrier<pipeMode>::Arrive(uint32_t arriveIndex)
{
    if ASCEND_IS_AIV {
        if (counter > 1) { // must wait last round to end
            uint32_t expectedWaitNum = (counter - 1) * waitSize;
            GlobalTensor<uint32_t> barrierInfoWaitGlobal;
            __gm__ BarrierInfo* BarrierInfoAddr =
                barrierInfoWait + (CACHE_LINE_LEN / sizeof(BarrierInfo)) * arriveIndex;
            dcci((__gm__ uint64_t*)BarrierInfoAddr, cache_line_t::SINGLE_CACHE_LINE, dcci_dst_t::CACHELINE_OUT);
            while (BarrierInfoAddr->head != expectedWaitNum) { // check wait in last round all finished
                dcci((__gm__ uint64_t*)BarrierInfoAddr, cache_line_t::SINGLE_CACHE_LINE, dcci_dst_t::CACHELINE_OUT);
            }
        }
        __WriteCurrentValue(barrierInfoArrive);
        counter += 1;
        hasArrive = true;
    }
}

// stuck in while loop until all aiv has arrived, then update wait counter
template <PipeMode pipeMode>
__aicore__ inline void GroupBarrier<pipeMode>::Wait(uint32_t waitIndex)
{
    // Get the counter by whether that aiv call arrive before wait
    // Ex: aiv call arrive + wait: In arrive, counter++. thus in wait, should counter - 1
    // Ex: aiv call only wait:     No arrive. thus no need to update counter
    if ASCEND_IS_AIV {
        uint32_t waitCounter = (hasArrive) ? counter - 1 : counter;
        uint32_t expectedArriveNum = waitCounter * arriveSize;
        __gm__ BarrierInfo* BarrierInfoAddr = barrierInfoArrive + (CACHE_LINE_LEN / sizeof(BarrierInfo)) * waitIndex;
        dcci((__gm__ uint64_t*)BarrierInfoAddr, cache_line_t::SINGLE_CACHE_LINE, dcci_dst_t::CACHELINE_OUT);
        while (BarrierInfoAddr->head < expectedArriveNum) { // check in current round, all aiv has arrived
            dcci((__gm__ uint64_t*)BarrierInfoAddr, cache_line_t::SINGLE_CACHE_LINE, dcci_dst_t::CACHELINE_OUT);
        }
        __WriteCurrentValue(barrierInfoWait);
        counter = (hasArrive) ? counter : counter + 1; // If counter updated by calling arrive before, no need to update
        hasArrive = false;
    }
}

template <PipeMode pipeMode>
__aicore__ inline uint64_t GroupBarrier<pipeMode>::GetWorkspaceLen()
{
    if ASCEND_IS_AIV {
        ASCENDC_DEBUG_ASSERT(
            (arriveSize > 0),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "arriveSize is %u, it must be larger than 0", arriveSize));
        ASCENDC_DEBUG_ASSERT(
            (waitSize > 0), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "waitSize is %u, it must be larger than 0", waitSize));
        ASCENDC_DEBUG_ASSERT(
            (waitSize <= GetBlockNum()),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "waitSize is %u, max waitSize is %lld \n", waitSize, GetBlockNum()));
        ASCENDC_DEBUG_ASSERT(
            (arriveSize <= GetBlockNum()),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "waitSize is %u, max waitSize is %lld \n", arriveSize, GetBlockNum()));
        return (arriveSize > waitSize) ? arriveSize * CACHE_LINE_LEN : waitSize * CACHE_LINE_LEN;
    }
}

template <PipeMode pipeMode>
__aicore__ inline void GroupBarrier<pipeMode>::__WriteCurrentValue(__gm__ BarrierInfo* BarrierInfoAddr)
{
    if ASCEND_IS_AIV {
        uint16_t num = (arriveSize >= waitSize) ? arriveSize : waitSize;
        event_t eventID = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_MTE3));
        SetFlag<HardEvent::S_MTE3>(eventID);
        WaitFlag<HardEvent::S_MTE3>(eventID);
        SetAtomicAddImpl<int32_t>();
#if ASCENDC_CPU_DEBUG
        ProcessLock::GetProcessLock()->Write();
        __ubuf__ int32_t* dst =
            (__ubuf__ int32_t*)(GetTPipePtr()->GetBaseAddr(static_cast<uint8_t>(TPosition::VECIN)) + UB_START_ADDR);
        DataCopyParams dataCopyParams = {num, 1, 0, CACHELINE_BLKNUM - 1};
        DataCopyUB2GMImpl((__gm__ void*)BarrierInfoAddr, (__ubuf__ void*)dst, dataCopyParams);
        ProcessLock::GetProcessLock()->Unlock();
#else
        __ubuf__ int32_t* dst = (__ubuf__ int32_t*)(UB_START_ADDR);
        // total: num * 32B block     src: consecutive   dst: apart by 512B
        DataCopyParams dataCopyParams = {num, 1, 0, CACHELINE_BLKNUM - 1};
        DataCopyUB2GMImpl((__gm__ void*)BarrierInfoAddr, (__ubuf__ void*)dst, dataCopyParams);
#endif
        SetAtomicNoneImpl();
    }
}
} // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_GROUP_BARRIER_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_GROUP_BARRIER_IMPL_H__
#endif
