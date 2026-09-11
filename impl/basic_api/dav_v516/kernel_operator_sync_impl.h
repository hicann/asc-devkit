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
 * \file kernel_operator_sync_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_v516/kernel_operator_sync_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SYNC_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_SYNC_IMPL_H
#define ASCENDC_MODULE_OPERATOR_SYNC_IMPL_H
namespace AscendC {
__aicore__ inline void ClcSyncCount(
    __gm__ int32_t* localSyncGM, __ubuf__ int32_t* ubWorkspaceAddr, const int32_t blockIdx, const int32_t totalBlocks,
    bool isFirst, int32_t& count)
{
    if (isFirst) {
        *(reinterpret_cast<__ubuf__ int32_t*>(ubWorkspaceAddr)) = 1;
        set_flag(PIPE_S, PIPE_MTE3, EVENT_ID0);
        wait_flag(PIPE_S, PIPE_MTE3, EVENT_ID0);
        copy_ubuf_to_gm_align_v2(
            static_cast<__gm__ int32_t*>(localSyncGM), static_cast<__ubuf__ int32_t*>(ubWorkspaceAddr), 0, 1, 32, 0, 32,
            32);
        count = 1;
        for (int32_t i = 0; i < totalBlocks; i++) {
            if (i != blockIdx) {
                count += *(reinterpret_cast<__ubuf__ int32_t*>(ubWorkspaceAddr) + i * ONE_BLK_FLOAT_NUM);
            }
        }
    } else {
        for (int32_t i = 0; i < totalBlocks; i++) {
            count += *(reinterpret_cast<__ubuf__ int32_t*>(ubWorkspaceAddr) + i * ONE_BLK_FLOAT_NUM);
        }
    }
}

template <bool isAIVOnly = true>
__aicore__ inline void SoftSyncAllImpl(
    __gm__ int32_t* gmWorkspaceAddr, __ubuf__ int32_t* ubWorkspaceAddr, const int usedCores)
{
    pipe_barrier(PIPE_ALL);

    int32_t totalBlocks = isAIVOnly ? get_block_num() : (GetTaskRationImpl() * get_block_num());
    totalBlocks = usedCores != 0 ? usedCores : totalBlocks;
    int32_t blockIdx = isAIVOnly ? get_block_idx() : GetBlockIdxImpl();

    __gm__ int32_t* localSyncGM = gmWorkspaceAddr + (blockIdx * DEFAULT_BLK_NUM);
    copy_gm_to_ubuf_align_v2(
        static_cast<__ubuf__ int32_t*>(ubWorkspaceAddr), static_cast<__gm__ int32_t*>(localSyncGM), 0, 1, 32, 0, 0,
        false, 0, 32, 32);
    set_flag(PIPE_MTE2, PIPE_S, EVENT_ID0);
    wait_flag(PIPE_MTE2, PIPE_S, EVENT_ID0);
    int32_t curValue = *(reinterpret_cast<__ubuf__ int32_t*>(ubWorkspaceAddr)) + 1;
    bool isFirst = curValue == 1 ? true : false;
    *(reinterpret_cast<__ubuf__ int32_t*>(ubWorkspaceAddr)) = curValue;
    set_flag(PIPE_S, PIPE_MTE3, EVENT_ID0);
    wait_flag(PIPE_S, PIPE_MTE3, EVENT_ID0);
    copy_ubuf_to_gm_align_v2(
        static_cast<__gm__ int32_t*>(localSyncGM), static_cast<__ubuf__ int32_t*>(ubWorkspaceAddr), 0, 1, 32, 0, 32,
        32);
    set_flag(PIPE_MTE3, PIPE_MTE2, EVENT_ID0);
    wait_flag(PIPE_MTE3, PIPE_MTE2, EVENT_ID0);
    int32_t totalBlockCount = ONE_BLK_FLOAT_NUM * totalBlocks;
    uint16_t blockLen = totalBlockCount / AscendCUtils::GetC0Count(sizeof(int32_t));
    while (true) {
        copy_gm_to_ubuf_align_v2(
            static_cast<__ubuf__ int32_t*>(ubWorkspaceAddr), static_cast<__gm__ int32_t*>(gmWorkspaceAddr), 0, 1,
            blockLen * 32, 0, 0, false, 0, blockLen * 32, blockLen * 32);
        set_flag(PIPE_MTE2, PIPE_S, EVENT_ID0);
        wait_flag(PIPE_MTE2, PIPE_S, EVENT_ID0);
        int32_t count = 0;
        ClcSyncCount(localSyncGM, ubWorkspaceAddr, blockIdx, totalBlocks, isFirst, count);
        pipe_barrier(PIPE_ALL);
        if ((isFirst && count == (totalBlocks * curValue)) || !isFirst && count >= (totalBlocks * curValue)) {
            break;
        }
    }
}

constexpr uint16_t SYNC_AIV_FLAG = 12;
constexpr uint16_t SYNC_AIC_FLAG = 11;
constexpr uint16_t SYNC_AIC_AIV_FLAG = 13;
constexpr uint16_t SYNC_AIV_ONLY_ALL = 14;
constexpr uint16_t SYNC_MODE_SHIFT_VALUE = 4;
constexpr uint16_t SYNC_FLAG_SHIFT_VALUE = 8;
constexpr uint16_t SYNC_FLAG_ID_MAX = 16;
constexpr uint16_t INTRA_NUM = 16;

__aicore__ inline uint16_t GetffstMsg(uint16_t mode, uint16_t flagId)
{
    return (0x1 + ((mode & 0x3) << SYNC_MODE_SHIFT_VALUE) + ((flagId & 0xf) << SYNC_FLAG_SHIFT_VALUE));
}

template <pipe_t AIV_PIPE = PIPE_MTE3, pipe_t AIC_PIPE = PIPE_FIX, bool FORCE = false>
__aicore__ inline void SetNextTaskStartImpl()
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "SetNextTaskStart");
}

template <int8_t earlyStartConfig = -1, bool FORCE = false>
__aicore__ inline void WaitPreTaskEndImpl()
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "WaitPreTaskEnd");
}

__aicore__ inline uint8_t GetIntraFlagId(uint8_t instID, uint8_t eventType, uint8_t subBlockId)
{
    return (instID + eventType + subBlockId * INTRA_NUM);
}

template <bool isAIVOnly, pipe_t triggerPipe, pipe_t waitPipe>
__aicore__ inline void SyncAllImpl()
{
    PipeBarrier<PIPE_ALL>();
    ffts_cross_core_sync(PIPE_MTE3, GetffstMsg(0x0, SYNC_AIV_ONLY_ALL));
    wait_flag_dev(PIPE_S, SYNC_AIV_ONLY_ALL);
    return;
}

template <uint8_t modeId, pipe_t pipe>
__aicore__ inline void NotifyEventImpl(uint16_t flagId)
{}

template <uint8_t modeId, pipe_t pipe>
__aicore__ inline void WaitEventImpl(uint16_t flagId)
{}

__aicore__ inline void SetSyncBaseAddrImpl(uint64_t config) { return; }

__aicore__ inline void SetSyncBaseAddr(uint64_t config) { SetSyncBaseAddrImpl(config); }
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_SYNC_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SYNC_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SYNC_IMPL_H__
#endif
