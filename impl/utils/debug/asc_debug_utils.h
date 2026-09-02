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
 * \file asc_debug_utils.h
 * \brief
 */
#ifndef IMPL_UTILS_DEBUG_ASC_DEBUG_UTILS_H
#define IMPL_UTILS_DEBUG_ASC_DEBUG_UTILS_H

#include "impl/utils/debug/asc_simd_debug_traits.h"

inline __gm__ uint8_t* __gm__ g_sysPrintFifoSpace = nullptr;

#ifndef ASCENDC_CPU_DEBUG
#include <type_traits>

#include "impl/utils/sys_macros.h"
#include "impl/utils/debug/asc_utils_types.h"
#include "impl/utils/debug/asc_debug_types.h"

#if __NPU_ARCH__ == 2002
#include "impl/utils/debug/npu_arch_2002/asc_debug_utils_impl.h"
#elif __NPU_ARCH__ == 2201
#include "impl/utils/debug/npu_arch_2201/asc_debug_utils_impl.h"
#elif __NPU_ARCH__ == 3510
#include "impl/utils/debug/npu_arch_3510/asc_debug_utils_impl.h"
#elif __NPU_ARCH__ == 5102
#include "impl/utils/debug/npu_arch_5102/asc_debug_utils_impl.h"
#else
#include "impl/utils/debug/asc_debug_utils_impl_stub.h"
#endif

namespace __asc_aicore {
__aicore__ inline void enable_asc_diagnostics()
{
#if (!defined(ASCENDC_DUMP) || (ASCENDC_DUMP != 0)) || defined(ASCENDC_TIME_STAMP_ON)
    static const struct AscTlv __asc_debug_meta_section__ __attribute__((used, section(".ascend.meta"))) = {4, 4, 1};
#endif // defined(ASCENDC_DUMP) || defined(ASCENDC_TIME_STAMP_ON)
}

__aicore__ inline void enable_asc_assert()
{
#if (!defined(ASCENDC_DUMP) || (ASCENDC_DUMP != 0)) || defined(ASCENDC_TIME_STAMP_ON)
    static const struct AscTlv __asc_assert_meta_section__ __attribute__((used, section(".ascend.meta"))) = {4, 4, 5};
#endif // defined(ASCENDC_DUMP) || defined(ASCENDC_TIME_STAMP_ON)
}

template <typename T, typename U, typename... Args>
__aicore__ constexpr bool is_same_in_list()
{
    if constexpr (sizeof...(Args) > 0) {
        return std::is_same<T, U>::value || is_same_in_list<T, Args...>();
    }
    return std::is_same<T, U>::value;
}

__aicore__ inline void asc_entire_dcci(__gm__ uint64_t* ptr) { asc_entire_dcci_impl(ptr); }

__aicore__ inline uint64_t asc_debug_get_system_cycle() { return asc_debug_get_system_cycle_impl(); }

__aicore__ inline uint32_t asc_debug_get_core_idx() { return asc_debug_get_core_idx_impl(); }

__aicore__ inline uint64_t asc_debug_get_block_idx() { return asc_debug_get_block_idx_impl(); }

__aicore__ inline uint64_t asc_debug_get_block_num() { return get_block_num(); }

__aicore__ inline uint64_t asc_debug_get_block_total_num()
{
#if (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3510)
    return asc_debug_get_block_num() * get_task_ration();
#else
    return asc_debug_get_block_num();
#endif
}

__aicore__ inline int64_t asc_debug_get_program_counter() { return get_pc(); }

template <uint64_t timeoutCycle = 15 * 1000 * 1000> // 20ms * 15
__aicore__ inline void ringbuf_wait_rts_sync()
{
    return ringbuf_wait_rts_sync_impl<timeoutCycle>();
}

__aicore__ inline uint32_t get_cstring_len(__gm__ const char* s)
{
    uint32_t i = 0;
    while (*(s + i) != '\0') {
        i++;
    }
    return i + 1;
}

__aicore__ constexpr inline uint32_t div_ceil(uint32_t a, uint32_t b)
{
    if (b == 0) {
        return 0;
    }
    return (a + b - 1) / b;
}

__aicore__ constexpr inline uint32_t align_up(uint32_t a, uint32_t b) { return div_ceil(a, b) * b; }

__aicore__ inline void sync_all() { sync_all_impl(); }

__aicore__ inline uint32_t mem_copy_gm_to_gm(__gm__ uint8_t* dst, __gm__ const uint8_t* src, const uint32_t& len)
{
    if (dst == nullptr || src == nullptr) {
        return 1;
    }
    for (uint32_t i = 0; i < len; i++) {
        *(dst + i) = *(src + i);
    }
    asc_entire_dcci((__gm__ uint64_t*)dst);
    return 0;
}

__aicore__ __gm__ inline DebugBlockHeadInfo* get_block_info()
{
    if (g_sysPrintFifoSpace == nullptr) {
        return nullptr;
    }
    uint32_t blockIdx = asc_debug_get_core_idx();
    uint32_t blockLength = reinterpret_cast<__gm__ DebugBlockHeadInfo*>(g_sysPrintFifoSpace)->length;
    __gm__ DebugBlockHeadInfo* blockInfo =
        reinterpret_cast<__gm__ DebugBlockHeadInfo*>(g_sysPrintFifoSpace + blockLength * blockIdx);
    if (blockInfo->magic != 0xAE86) {
        return nullptr;
    }
    if ASCEND_IS_AIV {
        blockInfo->flag = 1;
    } else {
        blockInfo->flag = 0;
    }
    asc_entire_dcci(reinterpret_cast<__gm__ uint64_t*>(blockInfo));
    return blockInfo;
}

__aicore__ __gm__ inline DebugBlockReadInfo* get_block_read_info(__gm__ DebugBlockHeadInfo* blockInfo)
{
    __gm__ uint8_t* blockHeadAddr = reinterpret_cast<__gm__ uint8_t*>(blockInfo);

    return reinterpret_cast<__gm__ DebugBlockReadInfo*>(blockHeadAddr + sizeof(DebugBlockHeadInfo));
}

__aicore__ __gm__ inline DebugBlockWriteInfo* get_block_write_info(__gm__ DebugBlockHeadInfo* blockInfo)
{
    __gm__ uint8_t* ringBufAddr = reinterpret_cast<__gm__ uint8_t*>(blockInfo->ringBufAddr);

    return reinterpret_cast<__gm__ DebugBlockWriteInfo*>(ringBufAddr + blockInfo->ringBufLen);
}

__aicore__ __gm__ inline uint8_t* get_ringbuf_tlv_addr(__gm__ DebugBlockHeadInfo* blockInfo)
{
    __gm__ DebugBlockWriteInfo* writeInfo = get_block_write_info(blockInfo);
    __gm__ uint8_t* ringBufAddr = reinterpret_cast<__gm__ uint8_t*>(blockInfo->ringBufAddr);
    return ringBufAddr + writeInfo->bufOffset;
}

__aicore__ inline void update_write_info(__gm__ DebugBlockWriteInfo* writeInfo, const uint32_t& tlvLen)
{
    writeInfo->bufOffset += tlvLen;
    writeInfo->packIdx += 1;
    asc_entire_dcci((__gm__ uint64_t*)writeInfo);
}

__aicore__ inline void ringbuf_skip_directly(__gm__ DebugBlockWriteInfo* writeInfo)
{
    writeInfo->bufOffset = 0;
    asc_entire_dcci((__gm__ uint64_t*)writeInfo);
}

__aicore__ inline void ringbuf_skip_with_info(
    __gm__ DebugBlockWriteInfo* writeInfo, __gm__ uint8_t* ringBufAddr, const uint32_t& ringBufLen)
{
    __gm__ SkipTlv* skipInfo = reinterpret_cast<__gm__ SkipTlv*>(ringBufAddr + writeInfo->bufOffset);
    skipInfo->type = static_cast<uint32_t>(DumpType::DUMP_SKIP);
    skipInfo->length = ringBufLen - writeInfo->bufOffset - sizeof(SkipTlv);
    writeInfo->bufOffset = 0;
    writeInfo->packIdx += 1;
    asc_entire_dcci((__gm__ uint64_t*)skipInfo);
    asc_entire_dcci((__gm__ uint64_t*)writeInfo);
    return;
}

__aicore__ inline void do_overlow_skip(
    __gm__ DebugBlockWriteInfo* writeInfo, __gm__ uint8_t* ringBufAddr, const uint32_t& ringBufLen,
    const uint32_t& minTlvLen)
{
    if (writeInfo->bufOffset + minTlvLen >= ringBufLen) {
        ringbuf_skip_directly(writeInfo);
        return;
    }
    ringbuf_skip_with_info(writeInfo, ringBufAddr, ringBufLen);
}

#if __NPU_ARCH__ == 3510
__aicore__ inline bool ringbuf_overflow_wait(__gm__ DebugBlockReadInfo* readInfo, __gm__ DebugBlockWriteInfo* writeInfo)
{
    constexpr uint32_t maxCounter = 15;
    // DAV_3510 SYS_CNT runs at 1 GHz, 20 times the 50 MHz baseline used by the default timeout.
    // Scale the cycle budget to retain a 300 ms wait and give RTS time to consume pending ring-buffer data.
    constexpr uint64_t rtsWaitCycle = 15 * 1000 * 1000 * 20;
    uint32_t counter = 0;
    volatile uint64_t readofst = ld_dev((__gm__ uint64_t*)(&(readInfo->bufOffset)), 0);
    volatile uint64_t writeofst = ld_dev((__gm__ uint64_t*)(&(writeInfo->bufOffset)), 0);
    // notice all state which need to wait in function check_ringbuf_space has (w != r) before wait
    while (readofst != writeofst) {
        if (counter >= maxCounter) { // max wait 15 * 300ms, rts read gm per 200ms
            return false;
        }
        ringbuf_wait_rts_sync<rtsWaitCycle>();
        ++counter;
        asc_entire_dcci((__gm__ uint64_t*)readInfo);
        readofst = ld_dev((__gm__ uint64_t*)(&(readInfo->bufOffset)), 0);
        writeofst = ld_dev((__gm__ uint64_t*)(&(writeInfo->bufOffset)), 0);
    }
    return true;
}
#else
__aicore__ inline bool ringbuf_overflow_wait(__gm__ DebugBlockReadInfo* readInfo, __gm__ DebugBlockWriteInfo* writeInfo)
{
    constexpr uint32_t maxCounter = 15;
    uint32_t counter = 0;
    // notice all state which need to wait in function check_ringbuf_space has (w != r) before wait
    while (writeInfo->bufOffset != readInfo->bufOffset) {
        if (counter >= maxCounter) { // max wait 15 * 300ms, rts read gm per 200ms
            return false;
        }
        ringbuf_wait_rts_sync(); // wait 20 * 15 ms
        ++counter;
        asc_entire_dcci((__gm__ uint64_t*)readInfo);
    }
    return true;
}
#endif

__aicore__ inline bool check_ringbuf_space(__gm__ DebugBlockHeadInfo* blockInfo, const uint32_t& tlvLen)
{
    constexpr uint32_t minTlvLen = sizeof(SkipTlv);

    __gm__ uint8_t* ringBufAddr = reinterpret_cast<__gm__ uint8_t*>(blockInfo->ringBufAddr);
    uint32_t ringBufLen = blockInfo->ringBufLen;

    __gm__ DebugBlockReadInfo* readInfo = get_block_read_info(blockInfo);
    __gm__ DebugBlockWriteInfo* writeInfo = get_block_write_info(blockInfo);

    if (minTlvLen >= ringBufLen || tlvLen > ringBufLen) {
        return false;
    }

    // 1. Check if wrap-around is needed
    if (writeInfo->bufOffset + tlvLen > ringBufLen) {
        // Wrap-around risks overwriting unread data. Wait if RTS is behind or at buffer start.
        if (readInfo->bufOffset > writeInfo->bufOffset || readInfo->bufOffset == 0) {
            // Wait ensures RTS consumes pending data since w != r.
            if (!ringbuf_overflow_wait(readInfo, writeInfo)) {
                return false;
            }
        }
        // Safe to wrap(set writeInfo->bufOffset=0) after RTS sync.
        do_overlow_skip(writeInfo, ringBufAddr, ringBufLen, minTlvLen);
    }
    // 2. No wrap-around needed, check for overwrite in current position
    if (writeInfo->bufOffset + tlvLen >= readInfo->bufOffset && readInfo->bufOffset > writeInfo->bufOffset) {
        // Write would overwrite unread region or stall RTS. Wait for RTS to advance.
        if (!ringbuf_overflow_wait(readInfo, writeInfo)) {
            return false;
        }
    }

    return true;
}

template <typename T>
__aicore__ constexpr inline DumpTensorDataType get_dump_datatype_impl();

template <typename T>
__aicore__ constexpr inline DumpTensorDataType get_dump_datatype()
{
    return get_dump_datatype_impl<T>();
}

__aicore__ inline void asc_debug_get_cann_vserion(__gm__ char*& versionStr, uint64_t& version, uint64_t& timeStamp)
{
#ifdef ASC_DEVKIT_VERSION_STR
    versionStr = const_cast<__gm__ char*>(ASC_DEVKIT_VERSION_STR);
#else
    versionStr = const_cast<__gm__ char*>("Unknown ASC_DEVKIT version");
#endif

#ifdef ASC_DEVKIT_TIMESTAMP
    timeStamp = static_cast<uint64_t>(ASC_DEVKIT_TIMESTAMP);
#else
    timeStamp = 0;
#endif

#ifdef ASC_DEVKIT_VERSION
    version = static_cast<uint64_t>(ASC_DEVKIT_VERSION);
#else
    version = 0;
#endif
}

} // namespace __asc_aicore
#endif

__aicore__ static __attribute__((noinline)) void AscVFDebugInitUb()
{
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0) && !defined(ASCENDC_CPU_DEBUG) && defined(__NPU_ARCH__) && \
    __NPU_ARCH__ == 3510 && !defined(__ASC_DISABLE_RESERVED_UBUF__)
    init_printf_ubuf_addr_aicore(asc_debug_get_block_idx());
#endif
}

__aicore__ static __attribute__((noinline)) void AscVFDebugFinish()
{
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0) && !defined(ASCENDC_CPU_DEBUG) && defined(__NPU_ARCH__) && \
    __NPU_ARCH__ == 3510 && !defined(__ASC_DISABLE_RESERVED_UBUF__)
    __asc_simd_vf::asc_finish_flag();
#endif
}

__aicore__ static __attribute__((noinline)) bool AscVFDebugTransferUb()
{
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0) && !defined(ASCENDC_CPU_DEBUG) && defined(__NPU_ARCH__) && \
    __NPU_ARCH__ == 3510 && !defined(__ASC_DISABLE_RESERVED_UBUF__)
    if (g_sysPrintFifoSpace != nullptr) {
        return asc_vf_debug_ub2gm();
    }
#endif
    return false;
}

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_DEBUG_UTILS__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_DEBUG_UTILS__
#endif

#endif // IMPL_UTILS_DEBUG_ASC_DEBUG_UTILS_H
