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
 * \file asc_aicore_time_impl.h
 * \brief
 */
#ifndef IMPL_UTILS_DEBUG_ASC_AICORE_TIME_IMPL_H
#define IMPL_UTILS_DEBUG_ASC_AICORE_TIME_IMPL_H

#if __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102
#ifndef ASCENDC_CPU_DEBUG
#include "impl/utils/debug/asc_debug_utils.h"

namespace __asc_aicore {

#ifdef ASCENDC_TIME_STAMP_ON
__BLOCK_LOCAL__ __inline__ uint8_t g_timeStampEntryFlag = 0;
#endif

__aicore__ inline void asc_write_time_stamp_tlv(
    __gm__ DebugBlockHeadInfo* block_info, uint32_t desc_id, uint64_t cycle, uint64_t entry)
{
    constexpr uint32_t tlv_len = sizeof(TimeStampTlv);
    if (!check_ringbuf_space(block_info, tlv_len)) {
        return;
    }

    __gm__ TimeStampTlv* time_stamp_tlv = reinterpret_cast<__gm__ TimeStampTlv*>(get_ringbuf_tlv_addr(block_info));
    time_stamp_tlv->type = static_cast<uint32_t>(DumpType::DUMP_TIME_STAMP);
    time_stamp_tlv->length = tlv_len - sizeof(uint32_t[2]);
    time_stamp_tlv->descId = desc_id;
    time_stamp_tlv->blockIdx = static_cast<uint16_t>(asc_debug_get_block_idx());
    time_stamp_tlv->resv = static_cast<uint16_t>(0U);
    time_stamp_tlv->cycle = cycle;
    time_stamp_tlv->pc = static_cast<uint64_t>(asc_debug_get_program_counter());
    time_stamp_tlv->entry = entry;
    time_stamp_tlv->resvMem[0] = static_cast<uint32_t>(0U);
    time_stamp_tlv->resvMem[1] = static_cast<uint32_t>(0U);
    asc_entire_dcci((__gm__ uint64_t*)time_stamp_tlv);

    __gm__ DebugBlockWriteInfo* write_info = get_block_write_info(block_info);

    update_write_info(write_info, tlv_len);
}

__aicore__ inline void asc_time_stamp_impl(uint32_t desc_id)
{
#ifdef ASCENDC_TIME_STAMP_ON
    const uint64_t time = asc_debug_get_system_cycle();
    const uint64_t entry = static_cast<uint64_t>(__get_entry_sys_cnt());
    __gm__ DebugBlockHeadInfo* block_info = get_block_info();
    if (block_info == nullptr) {
        return;
    }
    if (g_timeStampEntryFlag == 0) {
        g_timeStampEntryFlag = 1;
        asc_write_time_stamp_tlv(
            block_info, static_cast<uint32_t>(AscendC::TimeStampId::TIME_STAMP_WRAP_FIRST), entry, entry);
    }
    asc_write_time_stamp_tlv(block_info, desc_id, time, entry);
#endif
}

__aicore__ inline void asc_time_stamp(uint32_t desc_id)
{
    enable_asc_diagnostics();
    asc_time_stamp_impl(desc_id);
}

__aicore__ inline void asc_prof_start() { bisheng::cce::metrics_prof_start(); }

__aicore__ inline void asc_prof_stop() { bisheng::cce::metrics_prof_stop(); }

__aicore__ inline uint64_t clock_impl(void) { return static_cast<uint64_t>(get_sys_cnt()); }

__aicore__ inline uint64_t clock(void) { return clock_impl(); }

#if __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102
template <pipe_t pipe>
__aicore__ inline void asc_mark_stamp(uint16_t idx)
{
    bisheng::cce::mark_stamp<pipe>(idx);
}

template <pipe_t pipe, uint16_t idx>
__aicore__ inline void asc_mark_stamp()
{
    bisheng::cce::mark_stamp<pipe>(idx);
}
#endif
} // namespace __asc_aicore
#else
#include <cstdio>

namespace __asc_aicore {
__aicore__ inline void asc_time_stamp(uint32_t desc_id)
{
    assert(false && "asc_time_stamp is not supported in cpu mode.");
}
} // namespace __asc_aicore

using namespace __asc_aicore;
#endif
#endif

#ifdef ASCENDC_TRACE_ON
constexpr uint32_t ASC_PROF_START_EVENT = 0x80000000;
constexpr uint32_t ASC_PROF_STOP_EVENT = 0xc0000000;

__aicore__ inline void prof_mark_event(void)
{
    if (g_coreType == AscendC::AIV) {
        __asm__ volatile("NOP_BAR.V");
    } else if (g_coreType == AscendC::AIC) {
        __asm__ volatile("NOP_BAR.M");
        __asm__ volatile("NOP_BAR.MTE1");
    } else {
        __asm__ volatile("NOP_BAR.V");
        __asm__ volatile("NOP_BAR.M");
        __asm__ volatile("NOP_BAR.MTE1");
    }
    __asm__ volatile("NOP_BAR.MTE2");
    __asm__ volatile("NOP_BAR.MTE3");
}

#define COUNT_ARGS_IMPL(_1, _2, N, ...) N
#define COUNT_ARGS(...) COUNT_ARGS_IMPL(__VA_ARGS__, 2, 1)
#define GET_ARG_COUNT(...) COUNT_ARGS(__VA_ARGS__)
#define TRACE_START_INTERNAL(N, ...) TRACE_START_FUNC(N, __VA_ARGS__)
#define TRACE_START_FUNC(N, ...) TRACE_START_##N(__VA_ARGS__)
#define TRACE_START_IMPL(...) TRACE_START_INTERNAL(GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)
#define TRACE_STOP_INTERNAL(N, ...) TRACE_STOP_FUNC(N, __VA_ARGS__)
#define TRACE_STOP_FUNC(N, ...) TRACE_STOP_##N(__VA_ARGS__)
#define TRACE_STOP_IMPL(...) TRACE_STOP_INTERNAL(GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)
#if __NPU_ARCH__ == 3510
#define TRACE_START_2(pipe, idx) __asc_aicore::asc_mark_stamp<pipe>(static_cast<uint16_t>(idx) | 0x400)
#define TRACE_STOP_2(pipe, idx) __asc_aicore::asc_mark_stamp<pipe>(static_cast<uint16_t>(idx) | 0xc00)
#else
#define TRACE_START_2(pipe, idx) TRACE_START_1(idx)
#define TRACE_STOP_2(pipe, idx) TRACE_STOP_1(idx)
#endif

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5102)
__aicore__ inline void TRACE_START_1(TraceId apid) {}
__aicore__ inline void TRACE_STOP_1(TraceId apid) {}

#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#define TRACE_START_1(apid) __asc_aicore::asc_mark_stamp<PIPE_S>(static_cast<uint16_t>(apid) | 0x400)
#define TRACE_STOP_1(apid) __asc_aicore::asc_mark_stamp<PIPE_S>(static_cast<uint16_t>(apid) | 0xc00)

#elif defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
#define TRACE_START_1(apid)                                                \
    do {                                                                   \
        uint32_t v = (ASC_PROF_START_EVENT | static_cast<uint32_t>(apid)); \
        __asm__ __volatile__("");                                          \
        asm volatile("MOV COND, %0\n" : "+l"(v));                          \
        __asm__ __volatile__("");                                          \
    } while (0)

#define TRACE_STOP_1(apid)                                                \
    do {                                                                  \
        uint32_t v = (ASC_PROF_STOP_EVENT | static_cast<uint32_t>(apid)); \
        __asm__ __volatile__("");                                         \
        asm volatile("MOV COND, %0\n" : "+l"(v));                         \
        __asm__ __volatile__("");                                         \
    } while (0)
#else
#define TRACE_START_1(apid)                                            \
    do {                                                               \
        set_lpcnt(ASC_PROF_START_EVENT | static_cast<uint32_t>(apid)); \
        prof_mark_event();                                             \
    } while (0)

#define TRACE_STOP_1(apid)                                            \
    do {                                                              \
        set_lpcnt(ASC_PROF_STOP_EVENT | static_cast<uint32_t>(apid)); \
        prof_mark_event();                                            \
    } while (0)
#endif
#else

#define TRACE_START_IMPL(...)
#define TRACE_STOP_IMPL(...)
#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_AICORE_TIME_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_AICORE_TIME_IMPL__
#endif

#endif // IMPL_UTILS_DEBUG_ASC_AICORE_TIME_IMPL_H
