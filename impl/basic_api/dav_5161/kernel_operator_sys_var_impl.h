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
 * \file kernel_operator_sys_var_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_5161/kernel_operator_sys_var_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SYS_VAR_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_SYS_VAR_IMPL_H
#define ASCENDC_MODULE_OPERATOR_SYS_VAR_IMPL_H

namespace AscendC {
__aicore__ inline int64_t GetSubBlockIdxImpl() { return 0; }

__aicore__ inline int64_t GetTaskRationImpl() { return 1; }

__aicore__ inline int64_t GetBlockIdxImpl() { return block_idx; }

__aicore__ inline void GetArchVersionImpl(uint32_t& coreVersion)
{
    ASCENDC_ASSERT((false), "unsupported GetArchVersion!");
}

__aicore__ inline int64_t GetSubBlockNumImpl() { return 1; }

__aicore__ inline int64_t GetPhyCoreIDImpl()
{
    ASCENDC_ASSERT((false), "unsupported GetPhyCoreID!");
    return 0;
}

__aicore__ inline int64_t GetDataMainBaseImpl()
{
    ASCENDC_ASSERT((false), "unsupported GetDataMainBase!");
    return 0;
}

__aicore__ inline int64_t GetDataSizeImpl()
{
    ASCENDC_ASSERT((false), "unsupported GetDataSize!");
    return 0;
}

__aicore__ inline int64_t GetDataLocalBaseImpl()
{
    ASCENDC_ASSERT((false), "unsupported GetDataLocalBase!");
    return 0;
}

__aicore__ inline int64_t GetL2VirtualAddressImpl()
{
    ASCENDC_ASSERT((false), "unsupported GetL2VirtualAddress!");
    return 0;
}

__aicore__ inline int64_t GetParameterBaseAddrImpl()
{
    ASCENDC_ASSERT((false), "unsupported GetParameterBaseAddr!");
    return 0;
}

__aicore__ inline int64_t GetProgramCounterImpl()
{
    ASCENDC_ASSERT((false), "unsupported GetProgramCounter!");
    return 0;
}

__aicore__ inline int64_t GetSystemCycleImpl()
{
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    const int32_t timeExp = 1000000000;
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    int64_t timeStamp = (int64_t)ts.tv_sec * timeExp + ts.tv_nsec;
    return timeStamp;
#else
    uint64_t sysCnt = 0;
    asm volatile("MOV %0, SYS_CNT\n" : "+l"(sysCnt));
    return (int64_t)(sysCnt);
#endif
    return 0;
}

template <SpecialPurposeReg spr>
__aicore__ inline int64_t GetSprImpl()
{
    static_assert(
        SupportEnum<spr, SpecialPurposeReg::AR>(),
        "current GetSpr api only support SpecialPurposeReg AR on current device!");
    return get_ar();
}

__simd_vf__ inline void ClearARImpl()
{
    constexpr uint8_t SPR_AR_VALUE = 74;
    constexpr auto sprValue = std::integral_constant<::Spr, static_cast<::Spr>(SPR_AR_VALUE)>();
    sprclr(sprValue);
}

template <SpecialPurposeReg spr>
__aicore__ inline void ClearSprImpl()
{
    static_assert(
        SupportEnum<spr, SpecialPurposeReg::AR>(),
        "current ClearSpr api only support SpecialPurposeReg AR on current device!");

    if constexpr (spr == SpecialPurposeReg::AR) {
        ClearARImpl();
    }
}

__aicore__ inline int64_t GetSystemVirtualBaseImpl()
{
    ASCENDC_ASSERT((false), "unsupported GetSystemVirtualBase!");
    return 0;
}

__aicore__ inline void SetPcieRDCtrlImpl(bool isSetPcie, uint8_t maxBurstLen)
{
    ASCENDC_ASSERT((false), "unsupported SetPcieRDCtrl!");
}

__aicore__ inline void SetPcieWRCtrlImpl(bool isSetPcie, uint8_t maxBurstLen)
{
    ASCENDC_ASSERT((false), "unsupported SetPcieWRCtrl!");
}

__aicore__ inline void TrapImpl() { trap(); }

} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_SYS_VAR_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SYS_VAR_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SYS_VAR_IMPL_H__
#endif
