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
 * \file asc_aicore_printf_impl.h
 * \brief
 */
#ifndef IMPL_UTILS_DEBUG_ASC_AICORE_PRINTF_IMPL_H
#define IMPL_UTILS_DEBUG_ASC_AICORE_PRINTF_IMPL_H

#ifndef ASCENDC_CPU_DEBUG
#include "impl/utils/debug/asc_debug_utils.h"

#if __NPU_ARCH__ == 2002
#include "impl/utils/debug/npu_arch_2002/asc_aicore_printf_utils.h"
#elif __NPU_ARCH__ == 2201
#include "impl/utils/debug/npu_arch_2201/asc_aicore_printf_utils.h"
#elif __NPU_ARCH__ == 3510
#include "impl/utils/debug/npu_arch_3510/asc_aicore_printf_utils.h"
#elif __NPU_ARCH__ == 5102
#include "impl/utils/debug/npu_arch_5102/asc_aicore_printf_utils.h"
#endif

namespace __asc_aicore {
#if (__NPU_ARCH__ == 2002) || (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
template <typename T>
__aicore__ inline void set_scalar_param(__gm__ uint8_t* paramAddr, uint32_t paramIdx, T scalar)
{
    set_scalar_param_impl(paramAddr, paramIdx, scalar);
}

__aicore__ inline void set_string_param(
    __gm__ uint8_t* paramAddr, uint32_t paramIdx, __gm__ const char* s, uint32_t& offset)
{
    __gm__ uint64_t* stringAddr = reinterpret_cast<__gm__ uint64_t*>(paramAddr) + paramIdx;
    __gm__ uint64_t* dstStrAddr = reinterpret_cast<__gm__ uint64_t*>(paramAddr + offset);

    // write string value offset
    *((__gm__ uint64_t*)stringAddr) = static_cast<uint64_t>(offset - sizeof(uint64_t) * paramIdx);
    asc_entire_dcci((__gm__ uint64_t*)stringAddr);

    // write string content
    __gm__ char* d = (__gm__ char*)(dstStrAddr);
    uint32_t strLen = get_cstring_len(s);

    for (uint32_t i = 0; i < strLen; i++) {
        *(d + i) = *(s + i);
    }
    asc_entire_dcci((__gm__ uint64_t*)d);
    offset += strLen;
}

__aicore__ inline void set_param(__gm__ uint8_t* paramAddr, uint32_t paramIdx, uint32_t& offset) { return; }

template <typename... Args>
__aicore__ inline void set_param(__gm__ uint8_t* paramAddr, uint32_t paramIdx, uint32_t& offset, Args&&... args);

template <typename... Args>
__aicore__ inline void set_param_impl(
    __gm__ uint8_t* paramAddr, uint32_t paramIdx, uint32_t& offset, __gm__ const char* s, Args&&... args)
{
    set_string_param(paramAddr, paramIdx, s, offset);
    set_param(paramAddr, paramIdx + 1, offset, args...);
}

template <typename T, typename... Args>
__aicore__ inline void set_param_impl(
    __gm__ uint8_t* paramAddr, uint32_t paramIdx, uint32_t& offset, T scalar, Args&&... args)
{
    set_scalar_param(paramAddr, paramIdx, scalar);
    set_param(paramAddr, paramIdx + 1, offset, args...);
}

template <typename... Args>
__aicore__ inline void set_param(__gm__ uint8_t* paramAddr, uint32_t paramIdx, uint32_t& offset, Args&&... args)
{
    set_param_impl(paramAddr, paramIdx, offset, args...);
}

__aicore__ inline uint32_t get_args_len(uint32_t& argsNum) { return 0; }

template <typename... Args>
__aicore__ inline uint32_t get_args_len(uint32_t& argsNum, Args&&... args);

template <typename... Args>
__aicore__ inline uint32_t get_args_len_impl(uint32_t& argsNum, __gm__ const char* s, Args&&... args)
{
    constexpr uint32_t paramSize = sizeof(uint64_t);
    const uint32_t strLen = get_cstring_len(s);
    argsNum += 1;
    return paramSize + strLen + get_args_len(argsNum, args...);
}

template <typename T, typename... Args>
__aicore__ inline uint32_t get_args_len_impl(uint32_t& argsNum, T scalar, Args&&... args)
{
    constexpr uint32_t paramSize = sizeof(uint64_t);
    argsNum += 1;
    return paramSize + get_args_len(argsNum, args...);
}

template <typename... Args>
__aicore__ inline uint32_t get_args_len(uint32_t& argsNum, Args&&... args)
{
    return get_args_len_impl(argsNum, args...);
}

__aicore__ constexpr uint32_t align_print_tlv_len(const uint32_t dataLen)
{
    constexpr uint32_t alignBytes = 8;

    return ((dataLen + (alignBytes - 1)) & ~(alignBytes - 1)) + alignBytes;
}

template <typename... Args>
__aicore__ inline uint32_t get_print_tlv_len(
    uint32_t& argsNum, __gm__ const char* fmt, __gm__ const char* dumphead, Args&&... args)
{
    constexpr uint32_t printInfoLen = sizeof(PrintTlv);
    const uint32_t argsLen = get_args_len(argsNum, args...);
    const uint32_t fmtLen = get_cstring_len(fmt);
    const uint32_t dumpheadLen = get_cstring_len(dumphead) - 1;                // without '\0'
    return align_print_tlv_len(printInfoLen + argsLen + fmtLen + dumpheadLen); // gm need 8 byte align
}

__aicore__ inline void set_print_tlv_info(
    DumpType debugType, __gm__ PrintTlv* printTlv, const uint32_t& tlvLen, const uint32_t& argsNum)
{
    printTlv->type = static_cast<uint32_t>(debugType);
    printTlv->length = tlvLen - sizeof(uint32_t[2]); // exclude type and length
    printTlv->blockIdx = static_cast<uint32_t>(asc_debug_get_block_idx());
    printTlv->resv = static_cast<uint32_t>(0U);
    printTlv->fmtOffset = (argsNum + 1) * sizeof(uint64_t); // include fmt offset
    asc_entire_dcci((__gm__ uint64_t*)printTlv);
}

template <typename... Args>
__aicore__ inline void set_print_tlv_data(
    __gm__ PrintTlv* printTlv, __gm__ const char* fmt, __gm__ const char* dumphead, Args&&... args)
{
    const uint32_t& dumpheadLen = get_cstring_len(dumphead) - 1;
    const uint32_t& strLen = get_cstring_len(fmt);
    __gm__ uint8_t* paramAddr = reinterpret_cast<__gm__ uint8_t*>(printTlv + 1);
    __gm__ uint8_t* dumpheadAddr = paramAddr + printTlv->fmtOffset - sizeof(uint64_t);
    __gm__ uint8_t* fmtAddr = reinterpret_cast<__gm__ uint8_t*>(dumpheadAddr) + dumpheadLen;
    __gm__ uint8_t* strParamAddr = reinterpret_cast<__gm__ uint8_t*>(dumpheadAddr) + dumpheadLen + strLen;
    mem_copy_gm_to_gm(dumpheadAddr, reinterpret_cast<__gm__ const uint8_t*>(dumphead), dumpheadLen);
    mem_copy_gm_to_gm(fmtAddr, reinterpret_cast<__gm__ const uint8_t*>(fmt), strLen);
    uint32_t strParamOffset = printTlv->fmtOffset + strLen + dumpheadLen;
    set_param(paramAddr, 0, strParamOffset, args...);
}

template <class... Args>
__aicore__ inline void scalar_printf_impl(
    DumpType debugType, __gm__ const char* fmt, __gm__ const char* dumphead, Args&&... args)
{
    __gm__ DebugBlockHeadInfo* blockInfo = get_block_info();
    if (blockInfo == nullptr) {
        return;
    }
    uint32_t argsNum = 0;
    const uint32_t tlvLen = get_print_tlv_len(argsNum, fmt, dumphead, args...);
    if (!check_ringbuf_space(blockInfo, tlvLen)) {
        return;
    }

    __gm__ PrintTlv* printTlv = reinterpret_cast<__gm__ PrintTlv*>(get_ringbuf_tlv_addr(blockInfo));

    set_print_tlv_info(debugType, printTlv, tlvLen, argsNum);
    set_print_tlv_data(printTlv, fmt, dumphead, args...);

    __gm__ DebugBlockWriteInfo* writeInfo = get_block_write_info(blockInfo);

    update_write_info(writeInfo, tlvLen);
}

template <class... Args>
__aicore__ inline void print_common_head()
{
    __gm__ DebugBlockHeadInfo* blockInfo = get_block_info();
    if (blockInfo == nullptr) {
        return;
    }
    __gm__ DebugBlockWriteInfo* writeInfo = get_block_write_info(blockInfo);
    if (writeInfo == nullptr || writeInfo->packIdx != 0) {
        return;
    }

    uint64_t __ascendc_tStamp = 0;
    uint64_t __ascendc_version = 0;
    __gm__ char* __ascendc_versionStr = nullptr;
    asc_debug_get_cann_vserion(__ascendc_versionStr, __ascendc_version, __ascendc_tStamp);
    if (__ascendc_tStamp == 0) {
        scalar_printf_impl(
            DumpType::DUMP_SCALAR, "[WARNING]: CANN TimeStamp is invalid, CANN TimeStamp is %u\n", "",
            __ascendc_tStamp);
    } else {
        scalar_printf_impl(
            DumpType::DUMP_SCALAR, "CANN Version: %s, TimeStamp: %u\n", "", (__gm__ const char*)(__ascendc_versionStr),
            __ascendc_tStamp);
    }
}

template <class... Args>
__aicore__ inline void printf_impl_common(DumpType debugType, __gm__ const char* fmt, Args&&... args)
{
    uint64_t ctrlValue = get_ctrl();
    set_atomic_none();
#if !defined(__NPU_DEVICE__)
    if (debugType != DumpType::DUMP_ASSERT) {
        print_common_head();
    }
#endif
#ifdef __DAV_VEC__
    scalar_printf_impl(
        debugType, fmt, "[AIV Block %u/%u] ", asc_debug_get_block_idx(), asc_debug_get_block_total_num(), args...);
#else
    scalar_printf_impl(
        debugType, fmt, "[AIC Block %u/%u] ", asc_debug_get_block_idx(), asc_debug_get_block_total_num(), args...);
#endif
    set_ctrl(ctrlValue);
}

template <class... Args>
__aicore__ inline void printf_impl(__gm__ const char* fmt, Args&&... args)
{
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0)
    enable_asc_diagnostics();
    printf_impl_common(DumpType::DUMP_SCALAR, fmt, args...);
#endif
}

template <class... Args>
__aicore__ static __attribute__((noinline)) void printf_impl_assert(__gm__ const char* fmt, Args&&... args)
{
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0)
    enable_asc_assert();
    printf_impl_common(DumpType::DUMP_ASSERT, fmt, args...);
#endif
}

template <typename... Args>
static __attribute__((noinline)) __aicore__ void printf_impl_assert_msg(
    const __gm__ char* __assertion, const __gm__ char* __file, unsigned int __line, const __gm__ char* __function,
    const __gm__ char* fmt, Args&&... args)
{
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0)
    uint64_t ctrlValue = get_ctrl();
    enable_asc_assert();
    set_atomic_none();
#ifdef __DAV_VEC__
    scalar_printf_impl(
        DumpType::DUMP_ASSERT, fmt, "[AIV Block %u/%u] [ASSERT] %s:%u: %s: Assertion `%s' failed. ",
        asc_debug_get_block_idx(), asc_debug_get_block_total_num(), __file, __line, __function, __assertion, args...);
#else
    scalar_printf_impl(
        DumpType::DUMP_ASSERT, fmt, "[AIC Block %u/%u] [ASSERT] %s:%u: %s: Assertion `%s' failed. ",
        asc_debug_get_block_idx(), asc_debug_get_block_total_num(), __file, __line, __function, __assertion, args...);
#endif
    set_ctrl(ctrlValue);
#endif
}
#else
template <class... Args>
__aicore__ inline void printf_impl(__gm__ const char* fmt, Args&&... args)
{}

template <class... Args>
__aicore__ inline void printf_impl_assert(__gm__ const char* fmt, Args&&... args)
{}

template <typename... Args>
inline __aicore__ void printf_impl_assert_msg(
    const __gm__ char* __assertion, const __gm__ char* __file, unsigned int __line, const __gm__ char* __function,
    const __gm__ char* fmt, Args&&... args)
{}
#endif
} // namespace __asc_aicore
#else
#include <cstdio>
namespace __asc_aicore {
enum class DumpType : uint8_t {
    DUMP_DEFAULT = 0,
    DUMP_SCALAR,
};

template <class... Args>
__aicore__ inline void printf_impl(__gm__ const char* fmt, Args&&... args)
{
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0)
    std::printf(fmt, args...);
#endif
}

template <class... Args>
__aicore__ static __attribute__((noinline)) void printf_impl_assert(__gm__ const char* fmt, Args&&... args)
{
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0)
    std::printf(fmt, args...);
#endif
}

template <typename... Args>
static __attribute__((noinline)) __aicore__ void printf_impl_assert_msg(
    const __gm__ char* __assertion, const __gm__ char* __file, unsigned int __line, const __gm__ char* __function,
    const __gm__ char* fmt, Args&&... args)
{
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0)
    std::printf("[ASSERT] %s:%u: %s: Assertion `%s' failed. ", __file, __line, __function, __assertion);
    std::printf(fmt, args...);
#endif
}

} // namespace __asc_aicore

using namespace __asc_aicore;
#endif

#if !defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__) && !defined(__CHECK_FEATURE_AT_PRECOMPILE)
namespace __asc_aicore {
template <class... Args>
__aicore__ static __attribute__((noinline)) void printf(__gm__ const char* fmt, Args&&... args)
{
    printf_impl(fmt, args...);
}

template <class... Args>
__aicore__ static __attribute__((noinline)) void PRINTF(__gm__ const char* fmt, Args&&... args)
{
    printf_impl(fmt, args...);
}
} // namespace __asc_aicore
#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_AICORE_PRINTF_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_AICORE_PRINTF_IMPL__
#endif

#endif // IMPL_UTILS_DEBUG_ASC_AICORE_PRINTF_IMPL_H
