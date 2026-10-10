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
 * \file kernel_operator_gqm_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_9201/kernel_operator_gqm_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_gqm_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_GQM_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_GQM_IMPL_H
#define ASCENDC_MODULE_OPERATOR_GQM_IMPL_H

namespace AscendC {
namespace Internal {
constexpr uint8_t GQM_RETCODE_MASK = 0xFF; // Low 8 bits of ReturnData is the ErrorCode
}

// ===================== public class Gqm =====================
__aicore__ inline uint64_t Gqm::Init(uint64_t addr, int msgCount)
{
    constexpr uint64_t errNotAlign = 1;  // address not 64B-aligned
    constexpr uint64_t errDepthZero = 4; // queue max depth must be > 0

    if (addr & 0x3F) {
        return errNotAlign;
    }
    if (msgCount <= 0) {
        return errDepthZero;
    }

    this->addr_ = addr;

    constexpr uint64_t gqmPayLoadOffset = 21; // HasPayload flag bit
    constexpr uint64_t gqmCmdBits = 6;        // Command field width in CmdData
    constexpr uint64_t cmd = 1 << gqmPayLoadOffset;

    // Make prior memory accesses visible before Init writes the metadata
    dsb(mem_dsb_t::DSB_DDR);

    auto ret = __gqm(addr, 0, (cmd + (msgCount << gqmCmdBits)));

    return ret & Internal::GQM_RETCODE_MASK;
}

__aicore__ inline uint64_t Gqm::EnQue(uint64_t data)
{
    // User data must fit in 41 bits (max 2199023255551)
    constexpr uint64_t maxData = (1ULL << 41) - 1;
    ASCENDC_DEBUG_ASSERT(
        (data <= maxData), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "Gqm::EnQue data exceeds 41-bit range: %lu\n", data));

    constexpr uint64_t cmd = 0x1;

    // Drain prior in-flight requests so this push advances the tail in issue order
    dsb(mem_dsb_t::DSB_DDR);

    auto ret = __gqm(this->addr_, data, cmd);
    return ret & Internal::GQM_RETCODE_MASK;
}

__aicore__ inline uint64_t Gqm::DeQue(uint64_t& retValue)
{
    constexpr uint64_t popDataShift = 23; // PopData start bit in ReturnData
    constexpr uint64_t cmd = 0x5;

    // Flush prior in-flight requests so this pop reads data already landed
    dsb(mem_dsb_t::DSB_DDR);

    auto ret = __gqm(this->addr_, 0, cmd);

    retValue = ret >> popDataShift;
    return ret & Internal::GQM_RETCODE_MASK;
}

__aicore__ inline uint64_t Gqm::GetFreeSpace()
{
    constexpr uint64_t cmd = 0xF;

    // Flush prior in-flight requests so the queried RmnCnt is up-to-date
    dsb(mem_dsb_t::DSB_DDR);

    auto ret = __gqm(this->addr_, 0, cmd);

    constexpr uint64_t rmnCntShift = 8;     // RmnCnt start bit in ReturnData
    constexpr uint64_t rmnCntMask = 0x7FFF; // RmnCnt width is 15 bits
    return (ret >> rmnCntShift) & rmnCntMask;
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_GQM_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_GQM_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_GQM_IMPL_H__
#endif
