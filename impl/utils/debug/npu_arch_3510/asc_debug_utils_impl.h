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
 * \file asc_debug_utils_impl.h
 * \brief
 */
#ifndef IMPL_UTILS_DEBUG_NPU_ARCH_3510_ASC_DEBUG_UTILS_H
#define IMPL_UTILS_DEBUG_NPU_ARCH_3510_ASC_DEBUG_UTILS_H

#include "impl/utils/sys_macros.h"
#include "impl/utils/debug/asc_simd_vf_utils.h"

namespace __asc_simd_vf {
template <typename T, typename U, typename... Args>
__simd_callee__ constexpr bool is_same_in_list()
{
    if constexpr (sizeof...(Args) > 0) {
        return std::is_same<T, U>::value || is_same_in_list<T, Args...>();
    }
    return std::is_same<T, U>::value;
}

__simd_callee__ inline uint32_t get_cstring_len_vf(__ubuf__ const char* s)
{
    uint32_t i = 0;
    while (*(s + i) != '\0') {
        i++;
    }
    return i + 1;
}

__simd_callee__ constexpr inline uint32_t div_ceil(uint32_t a, uint32_t b)
{
    if (b == 0) {
        return 0;
    }
    return (a + b - 1) / b;
}

__simd_callee__ constexpr inline uint32_t align_up(uint32_t a, uint32_t b) { return div_ceil(a, b) * b; }

__simd_callee__ inline void wait_vf_debug_buffer_drained(__ubuf__ BlockVFBufInfo* block_info)
{
    while (block_info->readLen != block_info->writeLen) {
        __asm__ __volatile__("");
    }
}

__simd_callee__ inline void reset_vf_debug_buffer(__ubuf__ BlockVFBufInfo* block_info)
{
    block_info->writeLen = 0;
    block_info->readLen = 0;
}

__simd_callee__ inline void wait_vf_debug_buffer_drained_and_reset(__ubuf__ BlockVFBufInfo* block_info)
{
    wait_vf_debug_buffer_drained(block_info);
    reset_vf_debug_buffer(block_info);
}

template <typename T>
__simd_callee__ constexpr inline DumpTensorDataType get_dump_datatype_impl();

template <typename T>
__simd_callee__ constexpr inline DumpTensorDataType get_dump_datatype()
{
    return get_dump_datatype_impl<T>();
}

__simd_callee__ inline void enable_asc_diagnostics()
{
#if (!defined(ASCENDC_DUMP) || (ASCENDC_DUMP != 0)) || defined(ASCENDC_TIME_STAMP_ON)
    static const struct AscTlv __asc_debug_meta_section__ __attribute__((used, section(".ascend.meta"))) = {4, 4, 1};
#endif
}

__simd_callee__ inline void enable_asc_assert()
{
#if (!defined(ASCENDC_DUMP) || (ASCENDC_DUMP != 0)) || defined(ASCENDC_TIME_STAMP_ON)
    static const struct AscTlv __asc_assert_meta_section__ __attribute__((used, section(".ascend.meta"))) = {4, 4, 5};
#endif
}

__simd_callee__ constexpr inline uint32_t get_vf_debug_reserved_ub_size()
{
    constexpr uint32_t ascendcReservedUbSize = 2 * 1024;
    constexpr uint32_t vfStackReservedUbSize = 6 * 1024;

#if defined(__ASC_DISABLE_VF_STACK_RESERVED__)
    return ascendcReservedUbSize;
#else
    return ascendcReservedUbSize + vfStackReservedUbSize;
#endif
}

__simd_callee__ inline uint64_t get_vf_debug_reserved_ub_addr()
{
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0)
    return static_cast<uint64_t>(get_shmem_sz()) - get_vf_debug_reserved_ub_size();
#else
    return 0;
#endif
}

__simd_callee__ __ubuf__ inline BlockVFBufInfo* get_printf_ubuf_addr(uint64_t addr, uint16_t blockIdx = 0)
{
    uint64_t blockInfoAddr = addr;
    if (blockInfoAddr == 0) {
        blockInfoAddr = get_vf_debug_reserved_ub_addr();
    }
    auto* bufInfo = reinterpret_cast<__ubuf__ BlockVFBufInfo*>(blockInfoAddr);
    if (addr != 0) {
        init_debug_buffer(bufInfo, blockIdx);
    }
    return bufInfo;
}

__simd_callee__ inline void wait_vf_assert_handshake()
{
    __ubuf__ BlockVFBufInfo* block_info = get_printf_ubuf_addr(0);
    wait_vf_debug_buffer_drained(block_info);
    __ubuf__ volatile BlockVFBufInfo::AssertState* assertFlag = &block_info->assertFlag;
    *assertFlag = BlockVFBufInfo::AssertState::RAISED;
    while (*assertFlag != BlockVFBufInfo::AssertState::DRAINED) {
        __asm__ __volatile__("");
    }
}

__simd_callee__ __ubuf__ inline BlockVFBufInfo* init_printf_ubuf_addr(uint16_t blockIdx = 0)
{
    return get_printf_ubuf_addr(get_vf_debug_reserved_ub_addr(), blockIdx);
}

__no_simd_vf_fusion__ __simd_vf__ static inline void asc_finish_flag()
{
    __ubuf__ BlockVFBufInfo* blockInfo = get_printf_ubuf_addr(0);
    blockInfo->finish = 1;
}

__simd_callee__ inline void asc_copy_ub2ub(__ubuf__ void* dst, __ubuf__ void* src, uint32_t size)
{
    __cce_scalar::copy_ubuf_to_ubuf(dst, src, 1, size / 32, 0, 0);
}
} // namespace __asc_simd_vf

namespace __asc_aicore {
__aicore__ inline bool check_ringbuf_space(__gm__ DebugBlockHeadInfo* blockInfo, const uint32_t& tlvLen);

__aicore__ inline void asc_entire_dcci_impl(__gm__ uint64_t* ptr)
{
    dcci(ptr, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
}

__aicore__ inline uint64_t asc_debug_get_system_cycle_impl() { return get_sys_cnt(); }

template <uint64_t timeoutCycle>
__aicore__ inline void ringbuf_wait_rts_sync_impl()
{
    const uint64_t firstTimeStamp = asc_debug_get_system_cycle_impl();
    while (static_cast<uint64_t>(asc_debug_get_system_cycle_impl()) - firstTimeStamp < timeoutCycle) {
        // Wait for RTS sync
    }
}

__aicore__ inline uint32_t asc_debug_get_core_idx_impl()
{
    constexpr uint32_t dumpCoreNums = 108;
    return get_coreid() % dumpCoreNums;
}

__aicore__ inline uint64_t asc_debug_get_block_idx_impl()
{
#if defined(__DAV_VEC__)
    return get_block_idx() * get_subblockdim() + get_subblockid();
#else
    return get_block_idx();
#endif
}

__aicore__ inline int64_t get_task_ration()
{
#if defined(__DAV_CUBE__)
    return 1;
#else
    return get_subblockdim();
#endif
}

__aicore__ inline void sync_all_impl() { pipe_barrier(PIPE_ALL); }

template <typename T>
__aicore__ constexpr inline DumpTensorDataType get_dump_datatype_impl()
{
    if constexpr (std::is_same<T, bool>::value) {
        return DumpTensorDataType::ACL_BOOL;
    } else if (std::is_same<T, uint8_t>::value) {
        return DumpTensorDataType::ACL_UINT8;
    } else if (std::is_same<T, int8_t>::value) {
        return DumpTensorDataType::ACL_INT8;
    } else if (std::is_same<T, int16_t>::value) {
        return DumpTensorDataType::ACL_INT16;
    } else if (std::is_same<T, uint16_t>::value) {
        return DumpTensorDataType::ACL_UINT16;
    } else if (std::is_same<T, int32_t>::value) {
        return DumpTensorDataType::ACL_INT32;
    } else if (std::is_same<T, uint32_t>::value) {
        return DumpTensorDataType::ACL_UINT32;
    } else if (std::is_same<T, uint64_t>::value) {
        return DumpTensorDataType::ACL_UINT64;
    } else if (std::is_same<T, int64_t>::value) {
        return DumpTensorDataType::ACL_INT64;
    } else if (std::is_same<T, float>::value) {
        return DumpTensorDataType::ACL_FLOAT;
    } else if (std::is_same<T, half>::value) {
        return DumpTensorDataType::ACL_FLOAT16;
    } else if (std::is_same<T, bfloat16_t>::value) {
        return DumpTensorDataType::ACL_BF16;
    } else if (std::is_same<T, hifloat8_t>::value) {
        return DumpTensorDataType::ACL_HIFLOAT8;
    } else if (std::is_same<T, float8_e5m2_t>::value) {
        return DumpTensorDataType::ACL_FLOAT8_E5M2;
    } else if (std::is_same<T, float8_e4m3_t>::value) {
        return DumpTensorDataType::ACL_FLOAT8_E4M3FN;
    } else if (std::is_same<T, float8_e8m0_t>::value) {
        return DumpTensorDataType::ACL_FLOAT8_E8M0;
    } else if (std::is_same<T, float4_e2m1x2_t>::value) {
        return DumpTensorDataType::ACL_FLOAT4_E2M1;
    } else if (std::is_same<T, float4_e1m2x2_t>::value) {
        return DumpTensorDataType::ACL_FLOAT4_E1M2;
    } else {
        return DumpTensorDataType::ACL_MAX;
    }
}

__aicore__ __ubuf__ inline BlockVFBufInfo* get_printf_ubuf_addr_aicore(uint64_t addr, uint16_t blockIdx = 0)
{
    return __asc_simd_vf::get_printf_ubuf_addr(addr, blockIdx);
}

__aicore__ __ubuf__ inline BlockVFBufInfo* init_printf_ubuf_addr_aicore(uint16_t blockIdx = 0)
{
    return __asc_simd_vf::init_printf_ubuf_addr(blockIdx);
}

__aicore__ __gm__ inline BlockRingBufInfo* get_block_ring_buf_info()
{
    const uint32_t blockIdx = asc_debug_get_core_idx_impl();
    const uint32_t blockLength = reinterpret_cast<__gm__ BlockRingBufInfo*>(g_sysPrintFifoSpace)->length;
    __gm__ BlockRingBufInfo* blockInfo =
        reinterpret_cast<__gm__ BlockRingBufInfo*>(g_sysPrintFifoSpace + blockLength * blockIdx);
    return blockInfo;
}

__aicore__ inline __gm__ RingBufReadInfo* get_ring_buf_read_info(__gm__ BlockRingBufInfo* block_ring_buf_info)
{
    __gm__ uint8_t* blockHead = reinterpret_cast<__gm__ uint8_t*>(block_ring_buf_info);
    return reinterpret_cast<__gm__ RingBufReadInfo*>(blockHead + sizeof(BlockRingBufInfo));
}

__aicore__ inline __gm__ RingBufWriteInfo* get_ring_buf_write_info(__gm__ BlockRingBufInfo* block_ring_buf_info)
{
    __gm__ uint8_t* ringBufAddr = reinterpret_cast<__gm__ uint8_t*>(block_ring_buf_info->ringBufAddr);
    return reinterpret_cast<__gm__ RingBufWriteInfo*>(ringBufAddr + block_ring_buf_info->ringBufLen);
}

__aicore__ __gm__ inline uint8_t* call_get_ring_buf_tlv(__gm__ BlockRingBufInfo* block_ring_buf_info)
{
    __gm__ RingBufWriteInfo* writeInfo = get_ring_buf_write_info(block_ring_buf_info);
    __gm__ uint8_t* ringBufAddr = reinterpret_cast<__gm__ uint8_t*>(block_ring_buf_info->ringBufAddr);
    return ringBufAddr + writeInfo->bufOffset;
}

__aicore__ inline void update_write_info(
    __gm__ RingBufWriteInfo* writeInfo, const uint32_t& tlvLen, uint32_t packageNum = 1)
{
    writeInfo->bufOffset += tlvLen;
    writeInfo->packIdx += packageNum;
    asc_entire_dcci_impl(reinterpret_cast<__gm__ uint64_t*>(writeInfo));
}

__aicore__ static bool count_vf_tlv_packages(__ubuf__ uint8_t* tlv, uint32_t tlvLen, uint32_t& packageNum)
{
    constexpr uint32_t tlvHeadLen = 2 * sizeof(uint32_t);

    uint32_t offset = 0;

    while (offset + tlvHeadLen <= tlvLen) {
        __ubuf__ uint32_t* head = reinterpret_cast<__ubuf__ uint32_t*>(tlv + offset);
        uint32_t payloadLen = head[1];

        if (payloadLen > tlvLen - offset - tlvHeadLen) {
            return false;
        }

        offset += tlvHeadLen + payloadLen;
        packageNum += 1;
    }

    return packageNum != 0 && offset == tlvLen;
}

__aicore__ inline void asc_vf_debug_publish(
    __ubuf__ BlockVFBufInfo* blockInfo, uint32_t curWriteLen, uint32_t curReadLen)
{
    const uint32_t tlvLen = curWriteLen - curReadLen;
    __ubuf__ uint8_t* tlv = reinterpret_cast<__ubuf__ uint8_t*>(blockInfo->buffer) + curReadLen;

    uint32_t packageNum = 0;
    if (!count_vf_tlv_packages(tlv, tlvLen, packageNum)) {
        blockInfo->flag = 1;
        return;
    }

    __gm__ BlockRingBufInfo* blockRingBufInfo = get_block_ring_buf_info();
    auto* debugBlockInfo = reinterpret_cast<__gm__ DebugBlockHeadInfo*>(blockRingBufInfo);
    if (!check_ringbuf_space(debugBlockInfo, tlvLen)) {
        return;
    }
    __gm__ uint8_t* dstTlv = reinterpret_cast<__gm__ uint8_t*>(call_get_ring_buf_tlv(blockRingBufInfo));

    constexpr uint32_t sizeU32 = sizeof(uint32_t);
    const uint32_t totalWords = tlvLen / sizeU32;
    auto* dstWords = reinterpret_cast<__gm__ uint32_t*>(dstTlv);
    auto* srcWords = reinterpret_cast<__ubuf__ uint32_t*>(tlv);
    for (uint32_t i = 0; i < totalWords; ++i) {
        dstWords[i] = srcWords[i];
    }
    for (uint32_t i = totalWords * sizeU32; i < tlvLen; ++i) {
        dstTlv[i] = tlv[i];
    }
    asc_entire_dcci_impl(reinterpret_cast<__gm__ uint64_t*>(dstTlv));

    __gm__ RingBufWriteInfo* writeInfo = get_ring_buf_write_info(blockRingBufInfo);
    update_write_info(writeInfo, tlvLen, packageNum);
    blockInfo->readLen = curWriteLen;
}

__aicore__ inline bool asc_vf_debug_ub2gm()
{
    __ubuf__ BlockVFBufInfo* blockInfo = get_printf_ubuf_addr_aicore(0);
    for (;;) {
        uint32_t curReadLen = blockInfo->readLen;
        uint32_t curWriteLen = blockInfo->writeLen;

        const bool isValidHeader = blockInfo->magic == ASCENDC_SIMD_VF_MAGIC_NUMBER &&
                                   blockInfo->length <= ASCENDC_SIMD_VF_PRINTF_UBUF_MAX_SIZE &&
                                   curWriteLen <= blockInfo->length;
        if (!isValidHeader) {
            blockInfo->flag = 1;
            break;
        }
        // The producer clears writeLen before readLen when resetting a drained UB buffer.
        if (curReadLen > curWriteLen) {
            continue;
        }

        if (curReadLen < curWriteLen) {
            asc_vf_debug_publish(blockInfo, curWriteLen, curReadLen);
            if (blockInfo->flag != 0) {
                break;
            }
            continue;
        }

        if (blockInfo->assertFlag == BlockVFBufInfo::AssertState::RAISED) {
            blockInfo->assertFlag = BlockVFBufInfo::AssertState::DRAINED;
            break;
        }

        if (blockInfo->finish == 1) {
            break;
        }
    }
    return blockInfo->flag != 0;
}
} // namespace __asc_aicore

namespace __asc_simd_vf {
template <typename T>
__simd_callee__ constexpr inline DumpTensorDataType get_dump_datatype_impl()
{
    if constexpr (std::is_same<T, bool>::value) {
        return DumpTensorDataType::ACL_BOOL;
    } else if (std::is_same<T, uint8_t>::value) {
        return DumpTensorDataType::ACL_UINT8;
    } else if (std::is_same<T, int8_t>::value) {
        return DumpTensorDataType::ACL_INT8;
    } else if (std::is_same<T, int16_t>::value) {
        return DumpTensorDataType::ACL_INT16;
    } else if (std::is_same<T, uint16_t>::value) {
        return DumpTensorDataType::ACL_UINT16;
    } else if (std::is_same<T, int32_t>::value) {
        return DumpTensorDataType::ACL_INT32;
    } else if (std::is_same<T, uint32_t>::value) {
        return DumpTensorDataType::ACL_UINT32;
    } else if (std::is_same<T, uint64_t>::value) {
        return DumpTensorDataType::ACL_UINT64;
    } else if (std::is_same<T, int64_t>::value) {
        return DumpTensorDataType::ACL_INT64;
    } else if (std::is_same<T, float>::value) {
        return DumpTensorDataType::ACL_FLOAT;
    } else if (std::is_same<T, half>::value) {
        return DumpTensorDataType::ACL_FLOAT16;
    } else if (std::is_same<T, bfloat16_t>::value) {
        return DumpTensorDataType::ACL_BF16;
    } else if (std::is_same<T, hifloat8_t>::value) {
        return DumpTensorDataType::ACL_HIFLOAT8;
    } else if (std::is_same<T, float8_e5m2_t>::value) {
        return DumpTensorDataType::ACL_FLOAT8_E5M2;
    } else if (std::is_same<T, float8_e4m3_t>::value) {
        return DumpTensorDataType::ACL_FLOAT8_E4M3FN;
    } else if (std::is_same<T, float8_e8m0_t>::value) {
        return DumpTensorDataType::ACL_FLOAT8_E8M0;
    } else if (std::is_same<T, float4_e2m1x2_t>::value) {
        return DumpTensorDataType::ACL_FLOAT4_E2M1;
    } else if (std::is_same<T, float4_e1m2x2_t>::value) {
        return DumpTensorDataType::ACL_FLOAT4_E1M2;
    } else {
        return DumpTensorDataType::ACL_MAX;
    }
}
} // namespace __asc_simd_vf

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_DEBUG_UTILS_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_DEBUG_UTILS_IMPL__
#endif

#endif // IMPL_UTILS_DEBUG_NPU_ARCH_3510_ASC_DEBUG_UTILS_H
