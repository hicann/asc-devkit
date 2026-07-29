/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef __TILINGDATAUNALIGN_HEADER__
#define __TILINGDATAUNALIGN_HEADER__

#include "kernel_tiling/kernel_tiling.h"
#ifdef ASCENDC_CPU_DEBUG
#include "kernel_log.h"
#else
#ifndef __aicore__
#define __aicore__ [aicore]
#endif
#endif
class TilingDataUnalign {
public:
    __aicore__ __inline__ __attribute__((always_inline)) TilingDataUnalign() {}
    uint32_t formerNum;
    uint32_t tailNum;
    uint32_t formerLength;
    uint32_t tailLength;
    uint32_t alignNum;
    uint8_t TilingDataUnalignPH[4];
} __attribute__((__may_alias__));

#if defined(ASCENDC_CPU_DEBUG)
template <class T>
inline __aicore__ void InitTilingData(const __gm__ uint8_t* p_tilingdata, T* tilingdata)
#else
template <class T>
__inline__ __attribute__((always_inline)) __aicore__ void InitTilingData(
    const __gm__ uint8_t* p_tilingdata, T* tilingdata)
#endif
{
    constexpr uint64_t all_bytes = sizeof(T);
#if defined(ASCENDC_CPU_DEBUG) || defined(__DAV_C220_CUBE__) || defined(__GET_CODE_CHANNEL__)
#if defined(__DAV_C100__) || defined(ASCENDC_CPU_DEBUG)
    constexpr uint32_t judge_bytes = all_bytes > 15 ? all_bytes - 15 : 0;
    uint32_t i = 0;
    if (judge_bytes > 0) {
        for (; i < judge_bytes; i += 16) {
            (*(uint64_t*)((uint8_t*)tilingdata + i)) =
                (*(const __gm__ uint64_t*)((const __gm__ uint8_t*)p_tilingdata + i));
            (*(uint64_t*)((uint8_t*)tilingdata + i + 8)) =
                (*(const __gm__ uint64_t*)((const __gm__ uint8_t*)p_tilingdata + i + 8));
        }
    }
    if (all_bytes & 0x00000008) {
        (*(uint64_t*)((uint8_t*)tilingdata + i)) = (*(const __gm__ uint64_t*)((const __gm__ uint8_t*)p_tilingdata + i));
        i += 8;
    }
    if (all_bytes & 0x00000004) {
        (*(uint32_t*)((uint8_t*)tilingdata + i)) = (*(const __gm__ uint32_t*)((const __gm__ uint8_t*)p_tilingdata + i));
        i += 4;
    }
    if (all_bytes & 0x00000002) {
        (*(uint16_t*)((uint8_t*)tilingdata + i)) = (*(const __gm__ uint16_t*)((const __gm__ uint8_t*)p_tilingdata + i));
        i += 2;
    }
    if (all_bytes & 0x00000001) {
        (*(uint8_t*)((uint8_t*)tilingdata + i)) = (*(const __gm__ uint8_t*)((const __gm__ uint8_t*)p_tilingdata + i));
    }
#else
    copy_data_align64((uint8_t*)tilingdata, (__gm__ uint8_t*)p_tilingdata, all_bytes);
#endif
#else
    __ubuf__ uint8_t* tilingdata_in_ub = (__ubuf__ uint8_t*)get_imm(0);
    constexpr uint32_t len_burst = (all_bytes + 31) / 32;
#if defined(__DAV_C310__)
    copy_gm_to_ubuf_align_v2(
        (__ubuf__ uint8_t*)tilingdata_in_ub, (__gm__ uint8_t*)p_tilingdata, 0, 1, len_burst * 32, 0, 0, false, 0, 0, 0);
#else
    copy_gm_to_ubuf(((__ubuf__ uint8_t*)tilingdata_in_ub), p_tilingdata, 0, 1, len_burst, 0, 0);
#endif
    set_flag(PIPE_MTE2, PIPE_S, EVENT_ID0);
    wait_flag(PIPE_MTE2, PIPE_S, EVENT_ID0);
#if defined __DAV_C100__
    constexpr uint32_t judge_bytes = all_bytes > 15 ? all_bytes - 15 : 0;
    uint32_t i = 0;
    if (judge_bytes > 0) {
        for (; i < judge_bytes; i += 16) {
            (*(uint64_t*)((uint8_t*)tilingdata + i)) = (*(__ubuf__ uint64_t*)((__ubuf__ uint8_t*)tilingdata_in_ub + i));
            (*(uint64_t*)((uint8_t*)tilingdata + i + 8)) =
                (*(__ubuf__ uint64_t*)((__ubuf__ uint8_t*)tilingdata_in_ub + i + 8));
        }
    }
    if (all_bytes & 0x00000008) {
        (*(uint64_t*)((uint8_t*)tilingdata + i)) = (*(__ubuf__ uint64_t*)((__ubuf__ uint8_t*)tilingdata_in_ub + i));
        i += 8;
    }
    if (all_bytes & 0x00000004) {
        (*(uint32_t*)((uint8_t*)tilingdata + i)) = (*(__ubuf__ uint32_t*)((__ubuf__ uint8_t*)tilingdata_in_ub + i));
        i += 4;
    }
    if (all_bytes & 0x00000002) {
        (*(uint16_t*)((uint8_t*)tilingdata + i)) = (*(__ubuf__ uint16_t*)((__ubuf__ uint8_t*)tilingdata_in_ub + i));
        i += 2;
    }
    if (all_bytes & 0x00000001) {
        (*(uint8_t*)((uint8_t*)tilingdata + i)) = (*(__ubuf__ uint8_t*)((__ubuf__ uint8_t*)tilingdata_in_ub + i));
    }
#else
    copy_data_align64((uint8_t*)tilingdata, (__ubuf__ uint8_t*)tilingdata_in_ub, all_bytes);
#endif
#endif
#ifndef ASCENDC_CPU_DEBUG
    pipe_barrier(PIPE_ALL);
#endif
}

#ifdef ASCENDC_TIME_STAMP_ON
#define GET_TILING_DATA(tiling_data, tiling_arg) \
    TilingDataUnalign tiling_data;               \
    InitTilingData<TilingDataUnalign>(tiling_arg, &tiling_data);

#define GET_TILING_DATA_WITH_STRUCT(tiling_struct, tiling_data, tiling_arg) \
    tiling_struct tiling_data;                                              \
    InitTilingData<tiling_struct>(tiling_arg, &tiling_data);

#define GET_TILING_DATA_MEMBER(tiling_type, member, var, tiling) \
    decltype(((tiling_type*)0)->member) var;                     \
    size_t offset##var = (size_t)(&((tiling_type*)0)->member);   \
    InitTilingData<decltype(((tiling_type*)0)->member)>(tiling + offset##var, &var);

#else
#define GET_TILING_DATA(tiling_data, tiling_arg) \
    TilingDataUnalign tiling_data;               \
    InitTilingData<TilingDataUnalign>(tiling_arg, &tiling_data);
#define GET_TILING_DATA_WITH_STRUCT(tiling_struct, tiling_data, tiling_arg) \
    tiling_struct tiling_data;                                              \
    InitTilingData<tiling_struct>(tiling_arg, &tiling_data);
#define GET_TILING_DATA_MEMBER(tiling_type, member, var, tiling) \
    decltype(((tiling_type*)0)->member) var;                     \
    size_t offset##var = (size_t)(&((tiling_type*)0)->member);   \
    InitTilingData<decltype(((tiling_type*)0)->member)>(tiling + offset##var, &var);
#endif
#endif // __TILINGDATAUNALIGN_HEADER__
