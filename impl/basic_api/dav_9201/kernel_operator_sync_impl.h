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
 * \file kernel_operator_sync_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_9201/kernel_operator_sync_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SYNC_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_SYNC_IMPL_H
#define ASCENDC_MODULE_OPERATOR_SYNC_IMPL_H

#include "../utils/kernel_utils_constants.h"
#if !defined(ASCENDC_CPU_DEBUG)
#include "../inner_kernel_operator_hscb_intf.h"
#endif
#include "kernel_operator_block_sync_intf.h"
#include "../../../include/basic_api/kernel_common.h"
#include "../../utils/base/compat_utils.h"

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
            static_cast<__gm__ int32_t*>(localSyncGM), static_cast<__ubuf__ int32_t*>(ubWorkspaceAddr), 0, 1, 32, false,
            false, 0, 0, 32, 32, false);
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
    if ASCEND_IS_AIC {
        return;
    }
    PipeBarrier<PIPE_ALL>();

    int32_t totalBlocks = isAIVOnly ? get_block_num() : (GetTaskRationImpl() * get_block_num());
    totalBlocks = usedCores != 0 ? usedCores : totalBlocks;
    int32_t blockIdx = isAIVOnly ? get_block_idx() : GetBlockIdxImpl();

    __gm__ int32_t* localSyncGM = gmWorkspaceAddr + (blockIdx * DEFAULT_BLK_NUM);
    copy_gm_to_ubuf_align_v2(
        static_cast<__ubuf__ int32_t*>(ubWorkspaceAddr), static_cast<__gm__ int32_t*>(localSyncGM), 0, 1, 32, 0, 0,
        true, false, 0, 32, 32, false);
    set_flag(PIPE_MTE2, PIPE_S, EVENT_ID0);
    wait_flag(PIPE_MTE2, PIPE_S, EVENT_ID0);
    int32_t curValue = *(reinterpret_cast<__ubuf__ int32_t*>(ubWorkspaceAddr)) + 1;
    bool isFirst = curValue == 1 ? true : false;
    *(reinterpret_cast<__ubuf__ int32_t*>(ubWorkspaceAddr)) = curValue;
    set_flag(PIPE_S, PIPE_MTE3, EVENT_ID0);
    wait_flag(PIPE_S, PIPE_MTE3, EVENT_ID0);
    copy_ubuf_to_gm_align_v2(
        static_cast<__gm__ int32_t*>(localSyncGM), static_cast<__ubuf__ int32_t*>(ubWorkspaceAddr), 0, 1, 32, false,
        false, 0, 0, 32, 32, false);
    set_flag(PIPE_MTE3, PIPE_MTE2, EVENT_ID0);
    wait_flag(PIPE_MTE3, PIPE_MTE2, EVENT_ID0);
    int32_t totalBlockCount = ONE_BLK_FLOAT_NUM * totalBlocks;
    uint16_t blockLen = totalBlockCount / AscendCUtils::GetC0Count(sizeof(int32_t));
    while (true) {
        copy_gm_to_ubuf_align_v2(
            static_cast<__ubuf__ int32_t*>(ubWorkspaceAddr), static_cast<__gm__ int32_t*>(gmWorkspaceAddr), 0, 1,
            blockLen * 32, 0, 0, true, false, 0, blockLen * 32, blockLen * 32, false);
        set_flag(PIPE_MTE2, PIPE_S, EVENT_ID0);
        wait_flag(PIPE_MTE2, PIPE_S, EVENT_ID0);
        int32_t count = 0;
        ClcSyncCount(localSyncGM, ubWorkspaceAddr, blockIdx, totalBlocks, isFirst, count);
        PipeBarrier<PIPE_ALL>();
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

__aicore__ inline uint8_t GetIntraFlagId(uint8_t instID, uint8_t eventType, uint8_t subBlockId)
{
    return (instID + eventType + subBlockId * INTRA_NUM);
}

template <bool isAIVOnly, pipe_t triggerPipe, pipe_t waitPipe>
__aicore__ inline void SyncAllImpl()
{
    if constexpr (isAIVOnly) {
        if ASCEND_IS_AIV {
            static_assert(
                SupportEnum<triggerPipe, PIPE_ALL, PIPE_MTE2, PIPE_MTE3>(),
                "SyncAllConfig triggerPipe only supports PIPE_MTE2, PIPE_MTE3, PIPE_ALL on current device!");
            static_assert(
                SupportEnum<waitPipe, PIPE_ALL, PIPE_MTE2, PIPE_MTE3>(),
                "SyncAllConfig waitPipe only supports PIPE_MTE2, PIPE_MTE3, PIPE_ALL on current device!");
            PipeBarrier<triggerPipe>();
            if constexpr (triggerPipe == PIPE_ALL) {
                ffts_cross_core_sync(PIPE_MTE3, GetffstMsg(0x0, AscendC::SYNC_AIV_ONLY_ALL));
            } else {
                ffts_cross_core_sync(triggerPipe, GetffstMsg(0x0, AscendC::SYNC_AIV_ONLY_ALL));
            }
            if constexpr (waitPipe == PIPE_ALL) {
                wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_ONLY_ALL);
            } else {
                wait_flag_dev(waitPipe, AscendC::SYNC_AIV_ONLY_ALL);
            }
        }
        return;
    }
    // trans software sync to hardware sync
    PipeBarrier<PIPE_ALL>();
    if ASCEND_IS_AIC {
        wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_FLAG);
        ffts_cross_core_sync(PIPE_FIX, GetffstMsg(0x0, AscendC::SYNC_AIC_FLAG));
        wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_FLAG);
        ffts_cross_core_sync(PIPE_FIX, GetffstMsg(0x02, AscendC::SYNC_AIC_AIV_FLAG));
        return;
    }
    if ASCEND_IS_AIV {
        ffts_cross_core_sync(PIPE_MTE3, GetffstMsg(0x02, AscendC::SYNC_AIV_FLAG));
        wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_AIV_FLAG);
        return;
    }
}

template <pipe_t AIV_PIPE, pipe_t AIC_PIPE>
__aicore__ inline void SetNextTaskStartV3Impl(uint32_t earlyStartConfig)
{
    if ASCEND_IS_AIC {
        if (earlyStartConfig & Internal::ASCENDC_SUPER_KERNEL_EARLY_START_MASK_AIC_TO_AIC_SET) {
            ffts_cross_core_sync(AIC_PIPE, AscendC::GetffstMsg(0x0, AscendC::SYNC_AIC_FLAG)); // SET:C--->C
        }
    }
    if ASCEND_IS_AIV {
        if (earlyStartConfig & Internal::ASCENDC_SUPER_KERNEL_EARLY_START_MASK_AIV_TO_AIV_SET) {
            ffts_cross_core_sync(AIV_PIPE, AscendC::GetffstMsg(0x0, AscendC::SYNC_AIV_ONLY_ALL)); // SET:V--->V
        }
    }
}

template <pipe_t AIV_PIPE = PIPE_MTE3, pipe_t AIC_PIPE = PIPE_FIX, bool FORCE = false>
__aicore__ inline void SetNextTaskStartImpl()
{
#if defined(__ASCENDC_SUPERKERNEL_EARLY_START_V1) || defined(__ASCENDC_SUPERKERNEL_EARLY_START_V2)
    if ASCEND_IS_AIC {
        ffts_cross_core_sync(AIC_PIPE, AscendC::GetffstMsg(0x0, AscendC::SYNC_AIC_FLAG));
    }
    if ASCEND_IS_AIV {
        ffts_cross_core_sync(AIV_PIPE, AscendC::GetffstMsg(0x0, AscendC::SYNC_AIV_ONLY_ALL));
    }
#elif defined(__ASCENDC_SUPERKERNEL_EARLY_START_V3)
    SetNextTaskStartV3Impl<AIV_PIPE, AIC_PIPE>(g_super_kernel_early_start_config);
#else
    if constexpr (FORCE) {
        SetNextTaskStartV3Impl<AIV_PIPE, AIC_PIPE>(g_super_kernel_early_start_config);
    }
#endif
}

template <int8_t enableTightSync = -1>
__aicore__ inline void WaitPreTaskEndV1Impl()
{
    if constexpr (enableTightSync) {
        if ASCEND_IS_AIC {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_FLAG);
            set_intra_block(PIPE_MTE3, AscendC::SYNC_AIC_AIV_FLAG);
            set_intra_block(PIPE_MTE3, AscendC::SYNC_AIC_AIV_FLAG + AscendC::SYNC_FLAG_ID_MAX);
            wait_intra_block(PIPE_S, AscendC::SYNC_AIV_FLAG);
            wait_intra_block(PIPE_S, AscendC::SYNC_AIV_FLAG + AscendC::SYNC_FLAG_ID_MAX);
        }
        if ASCEND_IS_AIV {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_ONLY_ALL);
            set_intra_block(PIPE_MTE3, AscendC::SYNC_AIV_FLAG);
            wait_intra_block(PIPE_S, AscendC::SYNC_AIC_AIV_FLAG);
        }
    } else {
        if ASCEND_IS_AIC {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_FLAG);
            ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x02, AscendC::SYNC_AIC_AIV_FLAG));
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_FLAG);
        }
        if ASCEND_IS_AIV {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_ONLY_ALL);
            ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x02, AscendC::SYNC_AIV_FLAG));
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_AIV_FLAG);
        }
    }
    return;
}

__aicore__ inline void WaitPreTaskEndTightMixImpl()
{
    if ASCEND_IS_AIC {
        wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_FLAG);
        set_intra_block(PIPE_MTE3, AscendC::SYNC_AIC_AIV_FLAG);
        set_intra_block(PIPE_MTE3, AscendC::SYNC_AIC_AIV_FLAG + AscendC::SYNC_FLAG_ID_MAX);
        wait_intra_block(PIPE_S, AscendC::SYNC_AIV_FLAG);
        wait_intra_block(PIPE_S, AscendC::SYNC_AIV_FLAG + AscendC::SYNC_FLAG_ID_MAX);
    }
    if ASCEND_IS_AIV {
        wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_ONLY_ALL);
        set_intra_block(PIPE_MTE3, AscendC::SYNC_AIV_FLAG);
        wait_intra_block(PIPE_S, AscendC::SYNC_AIC_AIV_FLAG);
    }
}

__aicore__ inline void WaitPreTaskEndTightV2DefaultImpl()
{
#if defined(__ASCENDC_SUPERKERNEL_EARLY_START_V2)
    if (g_super_kernel_early_start_config == ASCENDC_SUPER_KERNEL_EARLY_START_AIC_TO_AIC) {
        if ASCEND_IS_AIC {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_FLAG);
        }
    } else if (g_super_kernel_early_start_config == ASCENDC_SUPER_KERNEL_EARLY_START_AIV_TO_AIV) {
        if ASCEND_IS_AIV {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_ONLY_ALL);
        }
    } else if (g_super_kernel_early_start_config == ASCENDC_SUPER_KERNEL_EARLY_START_AIC_TO_AIV) {
        if ASCEND_IS_AIC {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_FLAG);
            set_intra_block(PIPE_MTE3, AscendC::SYNC_AIC_AIV_FLAG);
            set_intra_block(PIPE_MTE3, AscendC::SYNC_AIC_AIV_FLAG + AscendC::SYNC_FLAG_ID_MAX);
        }
        if ASCEND_IS_AIV {
            wait_intra_block(PIPE_S, AscendC::SYNC_AIC_AIV_FLAG);
        }
    } else if (g_super_kernel_early_start_config == ASCENDC_SUPER_KERNEL_EARLY_START_AIV_TO_AIC) {
        if ASCEND_IS_AIC {
            wait_intra_block(PIPE_S, AscendC::SYNC_AIV_FLAG);
            wait_intra_block(PIPE_S, AscendC::SYNC_AIV_FLAG + AscendC::SYNC_FLAG_ID_MAX);
        }
        if ASCEND_IS_AIV {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_ONLY_ALL);
            set_intra_block(PIPE_MTE3, AscendC::SYNC_AIV_FLAG);
        }
    } else if (g_super_kernel_early_start_config == ASCENDC_SUPER_KERNEL_EARLY_START_MIX_TO_AIV) {
        if ASCEND_IS_AIC {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_FLAG);
            set_intra_block(PIPE_MTE3, AscendC::SYNC_AIC_AIV_FLAG);
            set_intra_block(PIPE_MTE3, AscendC::SYNC_AIC_AIV_FLAG + AscendC::SYNC_FLAG_ID_MAX);
        }
        if ASCEND_IS_AIV {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_ONLY_ALL);
            wait_intra_block(PIPE_S, AscendC::SYNC_AIC_AIV_FLAG);
        }
    } else if (g_super_kernel_early_start_config == ASCENDC_SUPER_KERNEL_EARLY_START_MIX_TO_AIC) {
        if ASCEND_IS_AIC {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_FLAG);
            wait_intra_block(PIPE_S, AscendC::SYNC_AIV_FLAG);
            wait_intra_block(PIPE_S, AscendC::SYNC_AIV_FLAG + AscendC::SYNC_FLAG_ID_MAX);
        }
        if ASCEND_IS_AIV {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_ONLY_ALL);
            set_intra_block(PIPE_MTE3, AscendC::SYNC_AIV_FLAG);
        }
    } else if (g_super_kernel_early_start_config == ASCENDC_SUPER_KERNEL_EARLY_START_AIC_TO_MIX) {
        if ASCEND_IS_AIC {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_FLAG);
            set_intra_block(PIPE_MTE3, AscendC::SYNC_AIC_AIV_FLAG);
            set_intra_block(PIPE_MTE3, AscendC::SYNC_AIC_AIV_FLAG + AscendC::SYNC_FLAG_ID_MAX);
        }
        if ASCEND_IS_AIV {
            wait_intra_block(PIPE_S, AscendC::SYNC_AIC_AIV_FLAG);
        }
    } else if (g_super_kernel_early_start_config == ASCENDC_SUPER_KERNEL_EARLY_START_AIV_TO_MIX) {
        if ASCEND_IS_AIC {
            wait_intra_block(PIPE_S, AscendC::SYNC_AIV_FLAG);
            wait_intra_block(PIPE_S, AscendC::SYNC_AIV_FLAG + AscendC::SYNC_FLAG_ID_MAX);
        }
        if ASCEND_IS_AIV {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_ONLY_ALL);
            set_intra_block(PIPE_MTE3, AscendC::SYNC_AIV_FLAG);
        }
    } else if (g_super_kernel_early_start_config == ASCENDC_SUPER_KERNEL_EARLY_START_MIX_TO_MIX) {
        WaitPreTaskEndTightMixImpl();
    }
    return;
#endif
}

template <int8_t earlyStartConfig = -1>
__aicore__ inline void WaitPreTaskEndTightV2Impl()
{
#if defined(__ASCENDC_SUPERKERNEL_EARLY_START_V2)
    if constexpr (earlyStartConfig == -1) {
        WaitPreTaskEndTightV2DefaultImpl();
    } else if constexpr (earlyStartConfig == ASCENDC_SUPER_KERNEL_EARLY_START_AIC_TO_AIC) {
        if ASCEND_IS_AIC {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_FLAG);
        }
    } else if constexpr (earlyStartConfig == ASCENDC_SUPER_KERNEL_EARLY_START_AIV_TO_AIV) {
        if ASCEND_IS_AIV {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_ONLY_ALL);
        }
    } else if constexpr (earlyStartConfig == ASCENDC_SUPER_KERNEL_EARLY_START_AIC_TO_AIV) {
        if ASCEND_IS_AIC {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_FLAG);
            set_intra_block(PIPE_MTE3, AscendC::SYNC_AIC_AIV_FLAG);
            set_intra_block(PIPE_MTE3, AscendC::SYNC_AIC_AIV_FLAG + AscendC::SYNC_FLAG_ID_MAX);
        }
        if ASCEND_IS_AIV {
            wait_intra_block(PIPE_S, AscendC::SYNC_AIC_AIV_FLAG);
        }
    } else if constexpr (earlyStartConfig == ASCENDC_SUPER_KERNEL_EARLY_START_AIV_TO_AIC) {
        if ASCEND_IS_AIC {
            wait_intra_block(PIPE_S, AscendC::SYNC_AIV_FLAG);
            wait_intra_block(PIPE_S, AscendC::SYNC_AIV_FLAG + AscendC::SYNC_FLAG_ID_MAX);
        }
        if ASCEND_IS_AIV {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_ONLY_ALL);
            set_intra_block(PIPE_MTE3, AscendC::SYNC_AIV_FLAG);
        }
    } else if constexpr (earlyStartConfig == ASCENDC_SUPER_KERNEL_EARLY_START_MIX_TO_AIV) {
        if ASCEND_IS_AIC {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_FLAG);
            set_intra_block(PIPE_MTE3, AscendC::SYNC_AIC_AIV_FLAG);
            set_intra_block(PIPE_MTE3, AscendC::SYNC_AIC_AIV_FLAG + AscendC::SYNC_FLAG_ID_MAX);
        }
        if ASCEND_IS_AIV {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_ONLY_ALL);
            wait_intra_block(PIPE_S, AscendC::SYNC_AIC_AIV_FLAG);
        }
    } else if constexpr (earlyStartConfig == ASCENDC_SUPER_KERNEL_EARLY_START_MIX_TO_AIC) {
        if ASCEND_IS_AIC {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_FLAG);
            wait_intra_block(PIPE_S, AscendC::SYNC_AIV_FLAG);
            wait_intra_block(PIPE_S, AscendC::SYNC_AIV_FLAG + AscendC::SYNC_FLAG_ID_MAX);
        }
        if ASCEND_IS_AIV {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_ONLY_ALL);
            set_intra_block(PIPE_MTE3, AscendC::SYNC_AIV_FLAG);
        }
    } else if constexpr (earlyStartConfig == ASCENDC_SUPER_KERNEL_EARLY_START_AIC_TO_MIX) {
        if ASCEND_IS_AIC {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_FLAG);
            set_intra_block(PIPE_MTE3, AscendC::SYNC_AIC_AIV_FLAG);
            set_intra_block(PIPE_MTE3, AscendC::SYNC_AIC_AIV_FLAG + AscendC::SYNC_FLAG_ID_MAX);
        }
        if ASCEND_IS_AIV {
            wait_intra_block(PIPE_S, AscendC::SYNC_AIC_AIV_FLAG);
        }
    } else if constexpr (earlyStartConfig == ASCENDC_SUPER_KERNEL_EARLY_START_AIV_TO_MIX) {
        if ASCEND_IS_AIC {
            wait_intra_block(PIPE_S, AscendC::SYNC_AIV_FLAG);
            wait_intra_block(PIPE_S, AscendC::SYNC_AIV_FLAG + AscendC::SYNC_FLAG_ID_MAX);
        }
        if ASCEND_IS_AIV {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_ONLY_ALL);
            set_intra_block(PIPE_MTE3, AscendC::SYNC_AIV_FLAG);
        }
    } else if constexpr (earlyStartConfig == ASCENDC_SUPER_KERNEL_EARLY_START_MIX_TO_MIX) {
        WaitPreTaskEndTightMixImpl();
    }
    return;
#endif
}

template <int8_t earlyStartConfig = -1>
__aicore__ inline void WaitPreTaskEndTightImpl()
{
#ifdef __ASCENDC_SUPERKERNEL_EARLY_START_V1
    WaitPreTaskEndV1Impl<true>();
#elif defined(__ASCENDC_SUPERKERNEL_EARLY_START_V2)
    WaitPreTaskEndTightV2Impl<earlyStartConfig>();
#endif
    return;
}

__aicore__ inline void WaitPreTaskEndLooseV2DefaultImpl()
{
#if defined(__ASCENDC_SUPERKERNEL_EARLY_START_V2)
    if (g_super_kernel_early_start_config == ASCENDC_SUPER_KERNEL_EARLY_START_AIC_TO_AIC) {
        if ASCEND_IS_AIC {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_FLAG);
        }
    } else if (g_super_kernel_early_start_config == ASCENDC_SUPER_KERNEL_EARLY_START_AIV_TO_AIV) {
        if ASCEND_IS_AIV {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_ONLY_ALL);
        }
    } else if (g_super_kernel_early_start_config == ASCENDC_SUPER_KERNEL_EARLY_START_AIC_TO_AIV) {
        if ASCEND_IS_AIC {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_FLAG);
            ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x02, AscendC::SYNC_AIC_AIV_FLAG));
        }
        if ASCEND_IS_AIV {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_AIV_FLAG);
        }
    } else if (g_super_kernel_early_start_config == ASCENDC_SUPER_KERNEL_EARLY_START_AIV_TO_AIC) {
        if ASCEND_IS_AIC {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_FLAG);
        }
        if ASCEND_IS_AIV {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_ONLY_ALL);
            ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x02, AscendC::SYNC_AIV_FLAG));
        }
    } else if (g_super_kernel_early_start_config == ASCENDC_SUPER_KERNEL_EARLY_START_MIX_TO_AIV) {
        if ASCEND_IS_AIC {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_FLAG);
            ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x02, AscendC::SYNC_AIC_AIV_FLAG));
        }
        if ASCEND_IS_AIV {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_ONLY_ALL);
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_AIV_FLAG);
        }
    } else if (g_super_kernel_early_start_config == ASCENDC_SUPER_KERNEL_EARLY_START_MIX_TO_AIC) {
        if ASCEND_IS_AIC {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_FLAG);
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_FLAG);
        }
        if ASCEND_IS_AIV {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_ONLY_ALL);
            ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x02, AscendC::SYNC_AIV_FLAG));
        }
    } else if (g_super_kernel_early_start_config == ASCENDC_SUPER_KERNEL_EARLY_START_AIC_TO_MIX) {
        if ASCEND_IS_AIC {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_FLAG);
            ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x02, AscendC::SYNC_AIC_AIV_FLAG));
        }
        if ASCEND_IS_AIV {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_AIV_FLAG);
        }
    } else if (g_super_kernel_early_start_config == ASCENDC_SUPER_KERNEL_EARLY_START_AIV_TO_MIX) {
        if ASCEND_IS_AIC {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_FLAG);
        }
        if ASCEND_IS_AIV {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_ONLY_ALL);
            ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x02, AscendC::SYNC_AIV_FLAG));
        }
    } else if (g_super_kernel_early_start_config == ASCENDC_SUPER_KERNEL_EARLY_START_MIX_TO_MIX) {
        if ASCEND_IS_AIC {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_FLAG);
            ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x02, AscendC::SYNC_AIC_AIV_FLAG));
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_FLAG);
        }
        if ASCEND_IS_AIV {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_ONLY_ALL);
            ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x02, AscendC::SYNC_AIV_FLAG));
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_AIV_FLAG);
        }
    }
    return;
#endif
}

template <int8_t earlyStartConfig = -1>
__aicore__ inline void WaitPreTaskEndLooseV2Impl()
{
#if defined(__ASCENDC_SUPERKERNEL_EARLY_START_V2)
    if constexpr (earlyStartConfig == -1) {
        WaitPreTaskEndLooseV2DefaultImpl();
    } else if constexpr (earlyStartConfig == ASCENDC_SUPER_KERNEL_EARLY_START_AIC_TO_AIC) {
        if ASCEND_IS_AIC {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_FLAG);
        }
    } else if constexpr (earlyStartConfig == ASCENDC_SUPER_KERNEL_EARLY_START_AIV_TO_AIV) {
        if ASCEND_IS_AIV {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_ONLY_ALL);
        }
    } else if constexpr (earlyStartConfig == ASCENDC_SUPER_KERNEL_EARLY_START_AIC_TO_AIV) {
        if ASCEND_IS_AIC {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_FLAG);
            ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x02, AscendC::SYNC_AIC_AIV_FLAG));
        }
        if ASCEND_IS_AIV {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_AIV_FLAG);
        }
    } else if constexpr (earlyStartConfig == ASCENDC_SUPER_KERNEL_EARLY_START_AIV_TO_AIC) {
        if ASCEND_IS_AIC {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_FLAG);
        }
        if ASCEND_IS_AIV {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_ONLY_ALL);
            ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x02, AscendC::SYNC_AIV_FLAG));
        }
    } else if constexpr (earlyStartConfig == ASCENDC_SUPER_KERNEL_EARLY_START_MIX_TO_AIV) {
        if ASCEND_IS_AIC {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_FLAG);
            ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x02, AscendC::SYNC_AIC_AIV_FLAG));
        }
        if ASCEND_IS_AIV {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_ONLY_ALL);
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_AIV_FLAG);
        }
    } else if constexpr (earlyStartConfig == ASCENDC_SUPER_KERNEL_EARLY_START_MIX_TO_AIC) {
        if ASCEND_IS_AIC {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_FLAG);
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_FLAG);
        }
        if ASCEND_IS_AIV {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_ONLY_ALL);
            ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x02, AscendC::SYNC_AIV_FLAG));
        }
    } else if constexpr (earlyStartConfig == ASCENDC_SUPER_KERNEL_EARLY_START_AIC_TO_MIX) {
        if ASCEND_IS_AIC {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_FLAG);
            ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x02, AscendC::SYNC_AIC_AIV_FLAG));
        }
        if ASCEND_IS_AIV {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_AIV_FLAG);
        }
    } else if constexpr (earlyStartConfig == ASCENDC_SUPER_KERNEL_EARLY_START_AIV_TO_MIX) {
        if ASCEND_IS_AIC {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_FLAG);
        }
        if ASCEND_IS_AIV {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_ONLY_ALL);
            ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x02, AscendC::SYNC_AIV_FLAG));
        }
    } else if constexpr (earlyStartConfig == ASCENDC_SUPER_KERNEL_EARLY_START_MIX_TO_MIX) {
        if ASCEND_IS_AIC {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_FLAG);
            ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x02, AscendC::SYNC_AIC_AIV_FLAG));
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_FLAG);
        }
        if ASCEND_IS_AIV {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_ONLY_ALL);
            ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x02, AscendC::SYNC_AIV_FLAG));
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_AIV_FLAG);
        }
    }
    return;
#endif
}

template <int8_t earlyStartConfig = -1>
__aicore__ inline void WaitPreTaskEndLooseImpl()
{
#ifdef __ASCENDC_SUPERKERNEL_EARLY_START_V1
    WaitPreTaskEndV1Impl<false>();
#elif defined(__ASCENDC_SUPERKERNEL_EARLY_START_V2)
    WaitPreTaskEndLooseV2Impl<earlyStartConfig>();
#endif
}

__aicore__ inline void WaitPreTaskEndTightV3Impl(uint32_t earlyStartConfig)
{
    if ASCEND_IS_AIC {
        if (earlyStartConfig & Internal::ASCENDC_SUPER_KERNEL_EARLY_START_MASK_AIC_TO_AIC_WAIT) {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_FLAG); // WAIT:C--->C
        }
        if (earlyStartConfig & Internal::ASCENDC_SUPER_KERNEL_EARLY_START_MASK_AIC_TO_AIV_SET) {
            set_intra_block(PIPE_MTE3, AscendC::SYNC_AIC_AIV_FLAG); // SET:C--->V
            set_intra_block(PIPE_MTE3, AscendC::SYNC_AIC_AIV_FLAG + AscendC::SYNC_FLAG_ID_MAX);
        }
        if (earlyStartConfig & Internal::ASCENDC_SUPER_KERNEL_EARLY_START_MASK_AIV_TO_AIC_WAIT) {
            wait_intra_block(PIPE_S, AscendC::SYNC_AIV_FLAG); // WAIT:V--->C
            wait_intra_block(PIPE_S, AscendC::SYNC_AIV_FLAG + AscendC::SYNC_FLAG_ID_MAX);
        }
    }
    if ASCEND_IS_AIV {
        if (earlyStartConfig & Internal::ASCENDC_SUPER_KERNEL_EARLY_START_MASK_AIV_TO_AIV_WAIT) {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_ONLY_ALL); // WAIT:V--->V
        }
        if (earlyStartConfig & Internal::ASCENDC_SUPER_KERNEL_EARLY_START_MASK_AIV_TO_AIC_SET) {
            set_intra_block(PIPE_MTE3, AscendC::SYNC_AIV_FLAG); // SET:V--->C
        }
        if (earlyStartConfig & Internal::ASCENDC_SUPER_KERNEL_EARLY_START_MASK_AIC_TO_AIV_WAIT) {
            wait_intra_block(PIPE_S, AscendC::SYNC_AIC_AIV_FLAG); // WAIT:C--->V
        }
    }
}

__aicore__ inline void WaitPreTaskEndLooseV3Impl(uint32_t earlyStartConfig)
{
    if ASCEND_IS_AIC {
        if (earlyStartConfig & Internal::ASCENDC_SUPER_KERNEL_EARLY_START_MASK_AIC_TO_AIC_WAIT) {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_FLAG); // WAIT:C--->C
        }
        if (earlyStartConfig & Internal::ASCENDC_SUPER_KERNEL_EARLY_START_MASK_AIC_TO_AIV_SET) {
            ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x02, AscendC::SYNC_AIC_AIV_FLAG)); // SET:C--->V
        }
        if (earlyStartConfig & Internal::ASCENDC_SUPER_KERNEL_EARLY_START_MASK_AIV_TO_AIC_WAIT) {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_FLAG); // WAIT:V--->C
        }
    }
    if ASCEND_IS_AIV {
        if (earlyStartConfig & Internal::ASCENDC_SUPER_KERNEL_EARLY_START_MASK_AIV_TO_AIV_WAIT) {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_ONLY_ALL); // WAIT:V--->V
        }
        if (earlyStartConfig & Internal::ASCENDC_SUPER_KERNEL_EARLY_START_MASK_AIV_TO_AIC_SET) {
            ffts_cross_core_sync(PIPE_MTE3, AscendC::GetffstMsg(0x02, AscendC::SYNC_AIV_FLAG)); // SET:V--->C
        }
        if (earlyStartConfig & Internal::ASCENDC_SUPER_KERNEL_EARLY_START_MASK_AIC_TO_AIV_WAIT) {
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_AIV_FLAG); // WAIT:C--->V
        }
    }
}

__aicore__ inline void WaitPreTaskEndV3Impl(uint32_t earlyStartConfig)
{
    if (earlyStartConfig & Internal::ASCENDC_SUPER_KERNEL_EARLY_START_CTRL_SPLIT_CORE) {
        WaitPreTaskEndLooseV3Impl(earlyStartConfig);
    } else {
        WaitPreTaskEndTightV3Impl(earlyStartConfig);
    }
}

// optimize if-else scalar cost, will be called only in superkernel.cpp
template <int8_t earlyStartConfig = -1>
__aicore__ inline void WaitPreTaskEndImpl()
{
#ifndef __ASCENDC_DAVID_SPLIT_CORE__
    WaitPreTaskEndTightImpl<earlyStartConfig>();
#else
    WaitPreTaskEndLooseImpl<earlyStartConfig>();
#endif
    return;
}

#if defined(__ASCENDC_SUPERKERNEL_AUTO_SYNC_ALL__)
template <bool isAIVOnly = true>
__aicore__ inline void SuperKernelAutoSyncAllImpl()
{
    if constexpr (isAIVOnly) {
        if ASCEND_IS_AIV {
            PipeBarrier<PIPE_ALL>();
            ffts_cross_core_sync(PIPE_MTE3, GetffstMsg(0x0, AscendC::SYNC_AIV_ONLY_ALL));
            wait_flag_dev(PIPE_S, AscendC::SYNC_AIV_ONLY_ALL);
        }
        return;
    }
    // trans software sync to hardware sync
    PipeBarrier<PIPE_ALL>();

    if ASCEND_IS_AIC {
        wait_intra_block(PIPE_S, AscendC::SYNC_AIV_FLAG);
        wait_intra_block(PIPE_S, AscendC::SYNC_AIV_FLAG + AscendC::SYNC_FLAG_ID_MAX);
        ffts_cross_core_sync(PIPE_FIX, GetffstMsg(0x0, AscendC::SYNC_AIC_FLAG));
        wait_flag_dev(PIPE_S, AscendC::SYNC_AIC_FLAG);
        set_intra_block(PIPE_S, AscendC::SYNC_AIC_AIV_FLAG);
        set_intra_block(PIPE_S, AscendC::SYNC_AIC_AIV_FLAG + AscendC::SYNC_FLAG_ID_MAX);
        return;
    }

    if ASCEND_IS_AIV {
        set_intra_block(PIPE_MTE3, AscendC::SYNC_AIV_FLAG);
        wait_intra_block(PIPE_S, AscendC::SYNC_AIC_AIV_FLAG);
        return;
    }
}

template <bool isAIVOnly = true>
__aicore__ inline void SuperKernelAutoSyncAllEndImpl()
{
    if (g_superKernelAutoSyncAllEnable) {
        if (GetBlockIdxImpl() == 0) {
            g_superKernelAutoSyncAllSyncIdx += 1;
            // |    32bits  |   16bits  |   8bits   |   8bits   |
            // |  sync idx  | sync type |   is end  | is valid  |
            *reinterpret_cast<__gm__ uint64_t*>(g_superKernelAutoSyncAllConfigGmAddr) =
                (static_cast<uint64_t>(g_superKernelAutoSyncAllSyncIdx)
                 << SK_AUTO_SYNC_ALL_SYNC_IDX_CONFIG_BIT_OFFSET) |
                (static_cast<uint64_t>(1) << SK_AUTO_SYNC_ALL_IS_END_CONFIG_BIT_OFFSET) |
                static_cast<uint64_t>(SK_AUTO_SYNC_ALL_VALID_MAGIC_NUM);
            dcci(
                reinterpret_cast<__gm__ uint64_t*>(g_superKernelAutoSyncAllConfigGmAddr),
                cache_line_t::SINGLE_CACHE_LINE, dcci_dst_t::CACHELINE_OUT);
        }
    }
}

__aicore__ inline void SuperKernelAutoSyncAllComplementImpl()
{
    if (!g_superKernelAutoSyncAllEnable) {
        return;
    }
    while (1) {
        SuperKernelAutoSyncAllDcciBarrier();
        dcci(
            reinterpret_cast<__gm__ uint64_t*>(g_superKernelAutoSyncAllConfigGmAddr), cache_line_t::SINGLE_CACHE_LINE,
            dcci_dst_t::CACHELINE_OUT);
        SuperKernelAutoSyncAllDcciBarrier();

        uint64_t tmpAutoSyncAllConfig = *reinterpret_cast<__gm__ uint64_t*>(g_superKernelAutoSyncAllConfigGmAddr);
        uint32_t currentSyncIdx =
            static_cast<uint32_t>(tmpAutoSyncAllConfig >> SK_AUTO_SYNC_ALL_SYNC_IDX_CONFIG_BIT_OFFSET);
        uint16_t syncType =
            static_cast<uint16_t>((tmpAutoSyncAllConfig & 0xffff0000) >> SK_AUTO_SYNC_ALL_SYNC_TYPE_CONFIG_BIT_OFFSET);
        uint8_t isSyncEnd =
            static_cast<uint8_t>((tmpAutoSyncAllConfig & 0xff00) >> SK_AUTO_SYNC_ALL_IS_END_CONFIG_BIT_OFFSET);
        uint8_t validNum = static_cast<uint8_t>(tmpAutoSyncAllConfig & 0xff);
        if (validNum != SK_AUTO_SYNC_ALL_VALID_MAGIC_NUM) {
            trap();
        }

        if (isSyncEnd == 1) {
            // is end
            g_superKernelAutoSyncAllSyncIdx += 1;
            break;
        }

        if (currentSyncIdx > g_superKernelAutoSyncAllSyncIdx) {
            if (syncType == 1) {
                // isAIVOnly
                SuperKernelAutoSyncAllImpl<true>();
            } else {
                // mix
                SuperKernelAutoSyncAllImpl<false>();
            }
            g_superKernelAutoSyncAllSyncIdx += 1;
        }
    }
}
#endif

// in Mode 4 cube core only notify corresponding vector core using hard path
// vector core have to notify corresponding cube core and opposite cube core at same time
template <uint8_t modeId, pipe_t pipe>
__aicore__ inline void NotifyEventCompatibleMode4Impl(uint16_t flagId)
{
    uint8_t streamId = get_acsqid();
    if constexpr (IsSplitVectorPipe<pipe>()) {
        if ASCEND_IS_AIV {
            // notify corresponding cube core
            set_intra_block(pipe, flagId);

            // stall pipe
            if constexpr (pipe != PIPE_S) {
                set_flag(pipe, PIPE_S, EVENT_ID5);
                wait_flag(pipe, PIPE_S, EVENT_ID5);
            }

            // notify opposite cube core
            uint8_t oppositeBlockId = get_block_idx() % 2 == 0 ? get_block_idx() + 1 : get_block_idx() - 1;
            uint8_t oppositeWriteCoreId = ld_dev(
                g_sysCV11CompatCV12WorkSpace + COMPATIBLE_COREID_MAP_REGION_OFFSET +
                    COMPATIBLE_COREID_MAP_SLOT_SIZE * streamId + oppositeBlockId * sizeof(CompatibleCoreIdInfo),
                0);

            __gm__ uint8_t* oppositeSyncCounterAddr = g_sysCV11CompatCV12WorkSpace +
                                                      COMPATIBLE_SYNC_MODE4_COUNTER_REGION_OFFSET +
                                                      oppositeWriteCoreId * COMPATIBLE_SYNC_SLOT_SIZE +
                                                      flagId * sizeof(uint64_t); // first half represent cube counter
            atomicAdd((__gm__ uint32_t*)oppositeSyncCounterAddr, (uint32_t)(1));
        }
    }
    if constexpr (IsSplitCubePipe<pipe>()) {
        if ASCEND_IS_AIC {
            // notify corresponding vec core
            if ((flagId <= 15 && get_block_idx() % 2 == 0) || (flagId > 15 && get_block_idx() % 2 == 1)) {
                set_intra_block(pipe, flagId % 16);
            }
        }
    }
}

// in Mode 2 cube core only notify corresponding vector core
// because data flow into vector core only comes from corresponding cube core
// vector core have to notify corresponding cube core and opposite cube core at same time
// can't use hard path here since it's full core sync
template <uint8_t modeId, pipe_t pipe>
__aicore__ inline void NotifyEventCompatibleMode2Impl(uint16_t flagId)
{
    uint8_t streamId = get_acsqid();
    if constexpr (IsSplitVectorPipe<pipe>()) {
        if ASCEND_IS_AIV {
            // stall pipe
            if constexpr (pipe != PIPE_S) {
                set_flag(pipe, PIPE_S, EVENT_ID5);
                wait_flag(pipe, PIPE_S, EVENT_ID5);
            }

            // write two cube core sync counter
            uint8_t oppositeBlockId = get_block_idx() % 2 == 0 ? get_block_idx() + 1 : get_block_idx() - 1;
            uint8_t oppositeWriteCoreId = ld_dev(
                g_sysCV11CompatCV12WorkSpace + COMPATIBLE_COREID_MAP_REGION_OFFSET +
                    COMPATIBLE_COREID_MAP_SLOT_SIZE * streamId + oppositeBlockId * sizeof(CompatibleCoreIdInfo),
                0);

            __gm__ uint8_t* syncCounterAddr = g_sysCV11CompatCV12WorkSpace + COMPATIBLE_SYNC_COUNTER_REGION_OFFSET +
                                              get_only_coreid() * COMPATIBLE_SYNC_SLOT_SIZE + flagId * sizeof(uint64_t);
            __gm__ uint8_t* oppositeSyncCounterAddr = g_sysCV11CompatCV12WorkSpace +
                                                      COMPATIBLE_SYNC_COUNTER_REGION_OFFSET +
                                                      oppositeWriteCoreId * COMPATIBLE_SYNC_SLOT_SIZE +
                                                      flagId * sizeof(uint64_t); // first half represent cube counter
            atomicAdd((__gm__ uint32_t*)syncCounterAddr, (uint32_t)(1));
            atomicAdd((__gm__ uint32_t*)oppositeSyncCounterAddr, (uint32_t)(1));
        }
    }
    if constexpr (IsSplitCubePipe<pipe>()) {
        if ASCEND_IS_AIC {
            // stall pipe
            if constexpr (pipe != PIPE_S) {
                set_flag(pipe, PIPE_S, EVENT_ID5);
                wait_flag(pipe, PIPE_S, EVENT_ID5);
            }

            // write corresponding vector sync counter
            __gm__ uint8_t* syncCounterAddr = g_sysCV11CompatCV12WorkSpace + COMPATIBLE_SYNC_COUNTER_REGION_OFFSET +
                                              get_only_coreid() * COMPATIBLE_SYNC_SLOT_SIZE +
                                              flagId * sizeof(uint64_t) + sizeof(uint32_t);
            atomicAdd((__gm__ uint32_t*)syncCounterAddr, (uint32_t)(1));
        }
    }
}

// in Mode 1 there is only AIV core sync
// AIV write opposite AIV core sync counter
template <uint8_t modeId, pipe_t pipe>
__aicore__ inline void NotifyEventCompatibleMode1Impl(uint16_t flagId)
{
    uint8_t streamId = get_acsqid();
    if constexpr (IsSplitVectorPipe<pipe>()) {
        if ASCEND_IS_AIV {
            // stall pipe
            if constexpr (pipe != PIPE_S) {
                set_flag(pipe, PIPE_S, EVENT_ID5);
                wait_flag(pipe, PIPE_S, EVENT_ID5);
            }

            // write opposite vector sync counter
            uint8_t oppositeBlockId = get_block_idx() % 2 == 0 ? get_block_idx() + 1 : get_block_idx() - 1;
            uint8_t oppositeWriteCoreId = ld_dev(
                g_sysCV11CompatCV12WorkSpace + COMPATIBLE_COREID_MAP_REGION_OFFSET +
                    COMPATIBLE_COREID_MAP_SLOT_SIZE * streamId + oppositeBlockId * sizeof(CompatibleCoreIdInfo) +
                    sizeof(uint8_t),
                0);

            __gm__ uint8_t* oppositeSyncCounterAddr =
                g_sysCV11CompatCV12WorkSpace + COMPATIBLE_SYNC_COUNTER_REGION_OFFSET +
                oppositeWriteCoreId * COMPATIBLE_SYNC_SLOT_SIZE + flagId * sizeof(uint64_t) +
                sizeof(uint32_t); // later half represent vector counter
            atomicAdd((__gm__ uint32_t*)oppositeSyncCounterAddr, (uint32_t)(1));
        }
    }
}

template <uint8_t modeId, pipe_t pipe>
__aicore__ inline void NotifyEventCompatibleImpl(uint16_t flagId)
{
    ASCENDC_ASSERT((g_sysCV11CompatCV12WorkSpace != nullptr), {
        KERNEL_LOG(
            KERNEL_ERROR, "Compatible Workspace pointer is nullptr! Try set environment variable "
                          "ASCEND_OP_COMPATIBILITY_MODE=1 and retry");
    });
    if constexpr (modeId == 4) {
        NotifyEventCompatibleMode4Impl<modeId, pipe>(flagId);
    } else if constexpr (modeId == 2) {
        NotifyEventCompatibleMode2Impl<modeId, pipe>(flagId);
    } else if constexpr (modeId == 1) {
        NotifyEventCompatibleMode1Impl<modeId, pipe>(flagId);
    } else if constexpr (modeId == 0) {
        if constexpr (IsSplitVectorPipe<pipe>()) {
            if ASCEND_IS_AIV {
                ffts_cross_core_sync(pipe, GetffstMsg(modeId, flagId));
            }
        }
        if constexpr (IsSplitCubePipe<pipe>()) {
            if ASCEND_IS_AIC {
                ffts_cross_core_sync(pipe, GetffstMsg(modeId, flagId));
            }
        }
    }
}

template <uint8_t modeId, pipe_t pipe>
__aicore__ inline void NotifyEventImpl(uint16_t flagId)
{
    if (IsCompatibleTo3510()) {
        NotifyEventCompatibleImpl<modeId, pipe>(flagId);
    } else {
        if constexpr (modeId == 4) {
            if constexpr (IsSplitVectorPipe<pipe>()) {
                if ASCEND_IS_AIV {
                    set_intra_block(pipe, flagId);
                }
            }
            if constexpr (IsSplitCubePipe<pipe>()) {
                if ASCEND_IS_AIC {
                    set_intra_block(pipe, flagId);
                }
            }
        } else {
            if constexpr (IsSplitVectorPipe<pipe>()) {
                if ASCEND_IS_AIV {
                    ffts_cross_core_sync(pipe, GetffstMsg(modeId, flagId));
                }
            }
            if constexpr (IsSplitCubePipe<pipe>()) {
                if ASCEND_IS_AIC {
                    ffts_cross_core_sync(pipe, GetffstMsg(modeId, flagId));
                }
            }
        }
    }
}

// in Mode 4 vec core wait would use hard path
// cube core would use hard path to accept corresponding vector notify
// and soft gm to accept opposite vector notify
template <uint8_t modeId, pipe_t pipe>
__aicore__ inline void WaitEventCompatibleMode4Impl(uint16_t flagId)
{
    if constexpr (IsSplitVectorPipe<pipe>()) {
        if ASCEND_IS_AIV {
            wait_intra_block(pipe, flagId);
        }
    }
    if constexpr (IsSplitCubePipe<pipe>()) {
        if ASCEND_IS_AIC {
            if ((flagId <= 15 && get_block_idx() % 2 == 0) || (flagId > 15 && get_block_idx() % 2 == 1)) {
                wait_intra_block(pipe, flagId % 16);
            } else {
                // read own cube sync counter
                __gm__ uint8_t* syncCounterAddr =
                    g_sysCV11CompatCV12WorkSpace + COMPATIBLE_SYNC_MODE4_COUNTER_REGION_OFFSET +
                    get_only_coreid() * COMPATIBLE_SYNC_SLOT_SIZE + (flagId % 16) * sizeof(uint64_t);
                uint32_t counterValue = ld_dev((__gm__ uint32_t*)syncCounterAddr, 0);
                while (counterValue == 0) {
                    counterValue = ld_dev((__gm__ uint32_t*)syncCounterAddr, 0);
                }
                atomicAdd((__gm__ int32_t*)syncCounterAddr, (int32_t)(-1));

                // stall pipe
                if constexpr (pipe != PIPE_S) {
                    set_flag(PIPE_S, pipe, EVENT_ID5);
                    wait_flag(PIPE_S, pipe, EVENT_ID5);
                }
            }
        }
    }
}

// in Mode 2 vector would read own sync counter
// cube core have to accept two notify to end loop
template <uint8_t modeId, pipe_t pipe>
__aicore__ inline void WaitEventCompatibleMode2Impl(uint16_t flagId)
{
    if constexpr (IsSplitVectorPipe<pipe>()) {
        if ASCEND_IS_AIV {
            // read own vector sync counter
            __gm__ uint8_t* syncCounterAddr = g_sysCV11CompatCV12WorkSpace + COMPATIBLE_SYNC_COUNTER_REGION_OFFSET +
                                              get_only_coreid() * COMPATIBLE_SYNC_SLOT_SIZE +
                                              flagId * sizeof(uint64_t) + sizeof(uint32_t);
            uint32_t counterValue = ld_dev((__gm__ uint32_t*)syncCounterAddr, 0);
            while (counterValue == 0) {
                counterValue = ld_dev((__gm__ uint32_t*)syncCounterAddr, 0);
            }
            atomicAdd((__gm__ int32_t*)syncCounterAddr, (int32_t)(-1));

            // stall pipe
            if constexpr (pipe != PIPE_S) {
                set_flag(PIPE_S, pipe, EVENT_ID5);
                wait_flag(PIPE_S, pipe, EVENT_ID5);
            }
        }
    }
    if constexpr (IsSplitCubePipe<pipe>()) {
        if ASCEND_IS_AIC {
            // read own cube sync counter
            __gm__ uint8_t* syncCounterAddr = g_sysCV11CompatCV12WorkSpace + COMPATIBLE_SYNC_COUNTER_REGION_OFFSET +
                                              get_only_coreid() * COMPATIBLE_SYNC_SLOT_SIZE + flagId * sizeof(uint64_t);
            uint32_t counterValue = ld_dev((__gm__ uint32_t*)syncCounterAddr, 0);
            while (counterValue < 2) {
                counterValue = ld_dev((__gm__ uint32_t*)syncCounterAddr, 0);
            }
            atomicAdd((__gm__ int32_t*)syncCounterAddr, (int32_t)(-2));

            // stall pipe
            if constexpr (pipe != PIPE_S) {
                set_flag(PIPE_S, pipe, EVENT_ID5);
                wait_flag(PIPE_S, pipe, EVENT_ID5);
            }
        }
    }
}

// in Mode 1 there is only AIV core sync
// read own sync counter
template <uint8_t modeId, pipe_t pipe>
__aicore__ inline void WaitEventCompatibleMode1Impl(uint16_t flagId)
{
    if constexpr (IsSplitVectorPipe<pipe>()) {
        if ASCEND_IS_AIV {
            // read own vector sync counter
            __gm__ uint8_t* syncCounterAddr = g_sysCV11CompatCV12WorkSpace + COMPATIBLE_SYNC_COUNTER_REGION_OFFSET +
                                              get_only_coreid() * COMPATIBLE_SYNC_SLOT_SIZE +
                                              flagId * sizeof(uint64_t) + sizeof(uint32_t);
            uint32_t counterValue = ld_dev((__gm__ uint32_t*)syncCounterAddr, 0);
            while (counterValue == 0) {
                counterValue = ld_dev((__gm__ uint32_t*)syncCounterAddr, 0);
            }
            atomicAdd((__gm__ int32_t*)syncCounterAddr, (int32_t)(-1));

            // stall pipe
            if constexpr (pipe != PIPE_S) {
                set_flag(PIPE_S, pipe, EVENT_ID5);
                wait_flag(PIPE_S, pipe, EVENT_ID5);
            }
        }
    }
}

template <uint8_t modeId, pipe_t pipe>
__aicore__ inline void WaitEventCompatibleImpl(uint16_t flagId)
{
    ASCENDC_ASSERT((g_sysCV11CompatCV12WorkSpace != nullptr), {
        KERNEL_LOG(
            KERNEL_ERROR, "Compatible Workspace pointer is nullptr! Try set environment variable "
                          "ASCEND_OP_COMPATIBILITY_MODE=1 and retry");
    });
    if constexpr (modeId == 4) {
        WaitEventCompatibleMode4Impl<modeId, pipe>(flagId);
    } else if constexpr (modeId == 2) {
        WaitEventCompatibleMode2Impl<modeId, pipe>(flagId);
    } else if constexpr (modeId == 1) {
        WaitEventCompatibleMode1Impl<modeId, pipe>(flagId);
    } else if constexpr (modeId == 0) {
        if constexpr (IsSplitVectorPipe<pipe>()) {
            if ASCEND_IS_AIV {
                wait_flag_dev(pipe, flagId);
            }
        }
        if constexpr (IsSplitCubePipe<pipe>()) {
            if ASCEND_IS_AIC {
                wait_flag_dev(pipe, flagId);
            }
        }
    }
}

template <uint8_t modeId, pipe_t pipe>
__aicore__ inline void WaitEventImpl(uint16_t flagId)
{
    if (IsCompatibleTo3510()) {
        WaitEventCompatibleImpl<modeId, pipe>(flagId);
    } else {
        if constexpr (modeId == 4) {
            if constexpr (IsSplitVectorPipe<pipe>()) {
                if ASCEND_IS_AIV {
                    wait_intra_block(pipe, flagId);
                }
            }
            if constexpr (IsSplitCubePipe<pipe>()) {
                if ASCEND_IS_AIC {
                    wait_intra_block(pipe, flagId);
                }
            }
        } else {
            if constexpr (IsSplitVectorPipe<pipe>()) {
                if ASCEND_IS_AIV {
                    wait_flag_dev(pipe, flagId);
                }
            }
            if constexpr (IsSplitCubePipe<pipe>()) {
                if ASCEND_IS_AIC {
                    wait_flag_dev(pipe, flagId);
                }
            }
        }
    }
}

__aicore__ inline void SetSyncBaseAddrImpl(uint64_t config) { return; }

__aicore__ inline void SetSyncBaseAddr(uint64_t config) { SetSyncBaseAddrImpl(config); }

#if !defined(ASCENDC_CPU_DEBUG)
namespace BlockSync {
template <SyncScope Scope>
class GroupBarrier {
public:
    __aicore__ inline GroupBarrier(uint8_t groupID, uint16_t arriveNum);

    template <pipe_t pipe = PIPE_S>
    __aicore__ inline void SendWait(
        uint64_t waitAddr, uint64_t notifyId, uint16_t expValue, uint8_t sprId, bool clrFlag);

    template <pipe_t pipe = PIPE_S>
    __aicore__ inline void Record(uint64_t recordAddr, uint64_t notifyId);

    template <pipe_t pipe = PIPE_S>
    __aicore__ inline void WaitSPR(uint8_t sprId);

private:
    uint8_t groupID;
    uint16_t arriveNum;
};

#if defined(__DAV_CUBE__)
constexpr uint64_t BASE_ADDR = 0x400000;
#else
constexpr uint64_t BASE_ADDR = 0x400000 + 0x10000;
#endif
constexpr uint64_t STRIDE = 16;
constexpr uint64_t RECORD_CMD_OFFSET = 0x4;
constexpr uint64_t WAIT_CMD_OFFSET = 0x10;
constexpr uint32_t GROUP_SIZE = 160;
constexpr uint8_t SPR_ID = 0;

template <SyncScope Scope>
__aicore__ inline GroupBarrier<Scope>::GroupBarrier(uint8_t groupID, uint16_t arriveNum)
{
    this->groupID = groupID;
    this->arriveNum = arriveNum;
}

template <SyncScope Scope>
template <pipe_t pipe>
__aicore__ inline void GroupBarrier<Scope>::SendWait(
    uint64_t waitAddr, uint64_t notifyId, uint16_t expValue, uint8_t sprId, bool clrFlag)
{
    uint64_t payload = 0;
    int64_t acsqId = get_acsqid();
    payload |= (static_cast<uint64_t>(acsqId) & 0x7F) << 25;
    payload |= (clrFlag ? 1UL : 0UL) << 22;
    payload |= (static_cast<uint64_t>(sprId + 8) & 0x1F) << 17;
    payload |= (notifyId & 0x1FFFF);
    payload |= (static_cast<uint64_t>(expValue) & 0xFFFFFFFF) << 32;
    Hscb::Store<uint64_t, true>(waitAddr, payload);
}

template <SyncScope Scope>
template <pipe_t pipe>
__aicore__ inline void GroupBarrier<Scope>::Record(uint64_t recordAddr, uint64_t notifyId)
{
    uint64_t payload = 0;
    int64_t acsqId = get_acsqid();
    payload |= (static_cast<uint64_t>(acsqId) & 0x7F) << 25;
    payload |= (notifyId & 0x1FFFF);
    Hscb::Store<uint64_t, true>(recordAddr, payload);
}

template <SyncScope Scope>
template <pipe_t pipe>
__aicore__ inline void GroupBarrier<Scope>::WaitSPR(uint8_t sprId)
{
    Hscb::WaitSpr<pipe>(sprId, 1);
}

template <SyncScope Scope = SyncScope::InterBlock>
__aicore__ inline void MakeBarrierImpl(uint8_t groupID, uint16_t arriveNum)
{
#if defined(ASCENDC_CPU_DEBUG)
    ASCENDC_ASSERT((groupID > GROUP_SIZE), {
        KERNEL_LOG(KERNEL_ERROR, "groupID should be less than GROUP_SIZE (%zu)", GROUP_SIZE);
    });
#else
    GroupBarrier<Scope> bar(groupID, arriveNum);
    uint16_t expValue = arriveNum;
    uint64_t recordAddr = BASE_ADDR + RECORD_CMD_OFFSET;
    uint64_t waitAddr = BASE_ADDR + WAIT_CMD_OFFSET;
    set_ipc_scb_0(0);
    PipeBarrier<PIPE_ALL>();
    bar.SendWait(waitAddr, groupID, expValue, SPR_ID, true);
    bar.Record(recordAddr, groupID);
    bar.WaitSPR(SPR_ID);
#endif
}

} // namespace BlockSync
#endif

} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_SYNC_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SYNC_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SYNC_IMPL_H__
#endif
