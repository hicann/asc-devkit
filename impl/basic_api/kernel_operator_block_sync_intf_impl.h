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
 * \file kernel_operator_block_sync_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_block_sync_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_block_sync_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_BLOCK_SYNC_INTF_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_BLOCK_SYNC_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_BLOCK_SYNC_INTERFACE_IMPL_H

#include "kernel_event.h"
#include "../../include/basic_api/kernel_tensor.h"
#include "../../include/basic_api/kernel_tpipe.h"
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TPIPE_H__
#endif
#include "../../include/basic_api/kernel_struct_mm.h"
#include "../../include/basic_api/kernel_operator_data_copy_intf.h"
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_INTF_H__
#endif
#include "../../include/basic_api/kernel_operator_sys_var_intf.h"

#if __NPU_ARCH__ == 1001
#include "dav_c100/kernel_operator_sync_impl.h"
#elif __NPU_ARCH__ == 2002
#include "dav_m200/kernel_operator_sync_impl.h"
#elif __NPU_ARCH__ == 2201
#include "dav_c220/kernel_operator_sync_impl.h"
#elif __NPU_ARCH__ == 3002
#include "dav_m300/kernel_operator_common_impl.h"
#elif __NPU_ARCH__ == 3003
#include "dav_l300/kernel_operator_sync_impl.h"
#elif __NPU_ARCH__ == 3102
#include "dav_m310/kernel_operator_common_impl.h"
#elif __NPU_ARCH__ == 3510
#include "dav_3510/kernel_operator_sync_impl.h"
#elif (__NPU_ARCH__ == 5102)
#include "dav_m510/kernel_operator_sync_impl.h"
#elif (__NPU_ARCH__ == 3113)
#include "dav_l311/kernel_operator_sync_impl.h"
#endif

namespace AscendC {

template <HardEvent event>
__aicore__ inline void SetFlag(int32_t eventID)
{
    if ASCEND_IS_AIC {
        if constexpr (
            event == HardEvent::MTE2_V || event == HardEvent::V_MTE2 || event == HardEvent::MTE3_V ||
            event == HardEvent::V_MTE3 || event == HardEvent::V_V || event == HardEvent::S_V ||
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
            event == HardEvent::V_S || event == HardEvent::MTE2_MTE3 || event == HardEvent::MTE3_MTE2 ||
            event == HardEvent::MTE3_S || event == HardEvent::S_MTE3) {
#else
            event == HardEvent::V_S) {
#endif
            return;
        }
    }
    if ASCEND_IS_AIV {
        if constexpr (
            (event == HardEvent::MTE2_MTE1) || (event == HardEvent::MTE1_MTE2) || (event == HardEvent::MTE1_M) ||
            (event == HardEvent::M_MTE1) || (event == HardEvent::M_FIX) || (event == HardEvent::FIX_M)) {
            return;
        }
    }
    SetFlagImpl<event>(eventID);
}

template <HardEvent event>
__aicore__ inline void WaitFlag(int32_t eventID)
{
    if ASCEND_IS_AIC {
        if constexpr (
            event == HardEvent::MTE2_V || event == HardEvent::V_MTE2 || event == HardEvent::MTE3_V ||
            event == HardEvent::V_MTE3 || event == HardEvent::V_V || event == HardEvent::S_V ||
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
            event == HardEvent::V_S || event == HardEvent::MTE2_MTE3 || event == HardEvent::MTE3_MTE2 ||
            event == HardEvent::MTE3_S || event == HardEvent::S_MTE3) {
#else
            event == HardEvent::V_S) {
#endif
            return;
        }
    }
    if ASCEND_IS_AIV {
        if constexpr (
            (event == HardEvent::MTE2_MTE1) || (event == HardEvent::MTE1_MTE2) || (event == HardEvent::MTE1_M) ||
            (event == HardEvent::M_MTE1) || (event == HardEvent::M_FIX) || (event == HardEvent::FIX_M)) {
            return;
        }
    }
    WaitFlagImpl(event, eventID);
}

template <pipe_t pipe>
__aicore__ inline void PipeBarrier()
{
    PipeBarrierImpl<pipe>();
}

#if defined(__NPU_ARCH__) &&                                                                                 \
    ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 3510) || \
     (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3103) || (__NPU_ARCH__ == 3113))
template <MemDsbT arg0>
__aicore__ inline void DataSyncBarrier()
{
    DataSyncBarrierImpl<arg0>();
}
#endif

/*
 * @ingroup：IBSet, IBWait
 * @brief：Set the flag bit of a core
 * @param [in] gmWorkspace GlobalTensor to store core state
 * @param [in] ubWorkspce LocalTensor for current core
 * @param [in] blockIdx the idx number waiting for the core
 * @param [in] eventID Set and wait events
 */
template <bool isAIVOnly>
__aicore__ inline void IBSet(
    const GlobalTensor<int32_t>& gmWorkspace, const LocalTensor<int32_t>& ubWorkspace, int32_t blockIdx,
    int32_t eventID)
{
    int32_t blockNum = GetBlockNum();
#if (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
#if (__NPU_ARCH__ != 5102)
    if ASCEND_IS_AIC {
        return;
    }
#endif
    if (!isAIVOnly) {
        blockNum = GetBlockNum() * 2;
    }
#endif
#if __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002
    __ib_set_stub(blockIdx, eventID, isAIVOnly);
#endif
    auto localSyncGM = gmWorkspace[blockNum * 8 * eventID + blockIdx * 8];
    pipe_barrier(PIPE_ALL);

    while (true) {
        DataCopy(ubWorkspace, localSyncGM, ONE_BLK_SIZE / sizeof(int32_t));
        event_t eventIdMte2ToS = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE2_S));
        SetFlag<HardEvent::MTE2_S>(eventIdMte2ToS);
        WaitFlag<HardEvent::MTE2_S>(eventIdMte2ToS);
        if (ubWorkspace.GetValue(0) == 0) {
            ubWorkspace.SetValue(0, 1);
            event_t eventIdSToMte3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_MTE3));
            SetFlag<HardEvent::S_MTE3>(eventIdSToMte3);
            WaitFlag<HardEvent::S_MTE3>(eventIdSToMte3);
            DataCopy(localSyncGM, ubWorkspace, ONE_BLK_SIZE / sizeof(int32_t));
            break;
        }
    }
    pipe_barrier(PIPE_ALL);
#if __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002
    __ib_set_stub(blockIdx, eventID, isAIVOnly);
#endif
}

template <bool isAIVOnly>
__aicore__ inline void IBWait(
    const GlobalTensor<int32_t>& gmWorkspace, const LocalTensor<int32_t>& ubWorkspace, int32_t blockIdx,
    int32_t eventID)
{
    int32_t blockNum = GetBlockNum();
#if (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
#if (__NPU_ARCH__ != 5102)
    if ASCEND_IS_AIC {
        return;
    }
#endif
    if (!isAIVOnly) {
        blockNum = GetBlockNum() * 2;
    }
#endif
#if __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002
    __ib_wait_stub(blockIdx, eventID, isAIVOnly);
#endif
    auto localSyncGM = gmWorkspace[blockNum * 8 * eventID + blockIdx * 8];
    pipe_barrier(PIPE_ALL);

    while (true) {
        DataCopy(ubWorkspace, localSyncGM, ONE_BLK_SIZE / sizeof(int32_t));
        event_t eventIdMte2ToS = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE2_S));
        SetFlag<HardEvent::MTE2_S>(eventIdMte2ToS);
        WaitFlag<HardEvent::MTE2_S>(eventIdMte2ToS);
        if (ubWorkspace.GetValue(0) == 1) {
            ubWorkspace.SetValue(0, 0);
            event_t eventIdSToMte3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_MTE3));
            SetFlag<HardEvent::S_MTE3>(eventIdSToMte3);
            WaitFlag<HardEvent::S_MTE3>(eventIdSToMte3);
            DataCopy(localSyncGM, ubWorkspace, ONE_BLK_SIZE / sizeof(int32_t));
            break;
        }
    }
    pipe_barrier(PIPE_ALL);
#if __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002
    __ib_wait_stub(blockIdx, eventID, isAIVOnly);
#endif
}

/*
 * @ingroup：SyncALL
 * @brief：Set flag bits of all cores
 * @param [in] gmWorkspace GlobalTensor to store core state
 * @param [in] ubWorkspce LocalTensor for current core
 */
template <bool isAIVOnly>
__aicore__ inline void SyncAll(
    const GlobalTensor<int32_t>& gmWorkspace, const LocalTensor<int32_t>& ubWorkspace, const int usedCores)
{
#if ASCENDC_CPU_DEBUG
    SoftSyncAllImpl<false>(
        (__gm__ int32_t*)gmWorkspace.GetPhyAddr(), (__ubuf__ int32_t*)ubWorkspace.GetPhyAddr(), usedCores);
#else
    SoftSyncAllImpl<isAIVOnly>(
        (__gm__ int32_t*)gmWorkspace.GetPhyAddr(), (__ubuf__ int32_t*)ubWorkspace.GetPhyAddr(), usedCores);
#endif
}

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
template <bool isAIVOnly, const SyncAllConfig& config>
__aicore__ inline void SyncAll()
{
    SyncAllImpl<isAIVOnly, config.triggerPipe, config.waitPipe>();
}
#else
template <bool isAIVOnly>
__aicore__ inline void SyncAll()
{
    SyncAllImpl<isAIVOnly>();
}
#endif

template <uint8_t modeId, pipe_t pipe>
__aicore__ inline void CrossCoreSetFlag(uint16_t flagId)
{
    NotifyEventImpl<modeId, pipe>(flagId);
}

template <uint8_t modeId, pipe_t pipe>
__aicore__ inline void CrossCoreWaitFlag(uint16_t flagId)
{
    WaitEventImpl<modeId, pipe>(flagId);
}

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
template <pipe_t pipe, uint8_t subBlockSyncMode = 2>
__aicore__ inline void NotifyEvent(uint16_t flagId)
{
    NotifyEventImpl<subBlockSyncMode, pipe>(flagId);
}

template <pipe_t pipe = PIPE_S, uint8_t mode = 0>
__aicore__ inline void WaitEvent(uint16_t flagId)
{
    WaitEventImpl<mode, pipe>(flagId);
}
#else
template <pipe_t pipe>
__aicore__ inline void NotifyEvent(uint16_t flagId)
{
    constexpr uint8_t subBlockSyncMode = 0x02;
    NotifyEventImpl<subBlockSyncMode, pipe>(flagId);
}

template <pipe_t pipe = PIPE_S>
__aicore__ inline void WaitEvent(uint16_t flagId)
{
    constexpr uint8_t mode = 0;
    WaitEventImpl<mode, pipe>(flagId);
}
#endif

} // namespace AscendC

#endif // ASCENDC_MODULE_OPERATOR_BLOCK_SYNC_INTERFACE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_BLOCK_SYNC_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_BLOCK_SYNC_INTF_IMPL_H__
#endif
