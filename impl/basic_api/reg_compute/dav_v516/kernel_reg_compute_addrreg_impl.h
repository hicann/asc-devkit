/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file kernel_reg_compute_addrreg_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_v516/kernel_reg_compute_addrreg_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_addrreg_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_ADDRREG_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_ADDRREG_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_ADDRREG_IMPL_H

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include "stub_fun.h"
#endif

namespace AscendC {
namespace Reg {
template <typename T>
__simd_callee__ inline AddrReg CreateAddrRegImpl(uint16_t index0, uint32_t stride0)
{
    static_assert(
        sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8,
        "CreateAddrReg only support type b8/b16/b32/b64 on current device");
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    return VagCpuSim(sizeof(T), index0, stride0);
#endif
    if constexpr (sizeof(T) == 1) {
        return vag_b8(stride0);
    } else if constexpr (sizeof(T) == 2) {
        return vag_b16(stride0);
    } else if constexpr (sizeof(T) == 8) {
        return vag_b32(stride0 * 2);
    }
    return vag_b32(stride0);
}

template <typename T>
__simd_callee__ inline AddrReg CreateAddrRegImpl(uint16_t index0, uint32_t stride0, uint16_t index1, uint32_t stride1)
{
    static_assert(
        sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8,
        "CreateAddrReg only support type b8/b16/b32/b64 on current device");
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    return VagCpuSim(sizeof(T), index0, stride0, index1, stride1);
#endif
    if constexpr (sizeof(T) == 1) {
        return vag_b8(stride0, stride1);
    } else if constexpr (sizeof(T) == 2) {
        return vag_b16(stride0, stride1);
    } else if constexpr (sizeof(T) == 8) {
        return vag_b32(stride0 * 2, stride1 * 2);
    }
    return vag_b32(stride0, stride1);
}

template <typename T>
__simd_callee__ inline AddrReg CreateAddrRegImpl(
    uint16_t index0, uint32_t stride0, uint16_t index1, uint32_t stride1, uint16_t index2, uint32_t stride2)
{
    static_assert(
        sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8,
        "CreateAddrReg only support type b8/b16/b32/b64 on current device");
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    return VagCpuSim(sizeof(T), index0, stride0, index1, stride1, index2, stride2);
#endif
    if constexpr (sizeof(T) == 1) {
        return vag_b8(stride0, stride1, stride2);
    } else if constexpr (sizeof(T) == 2) {
        return vag_b16(stride0, stride1, stride2);
    } else if constexpr (sizeof(T) == 8) {
        return vag_b32(stride0 * 2, stride1 * 2, stride2 * 2);
    }
    return vag_b32(stride0, stride1, stride2);
}

template <typename T>
__simd_callee__ inline AddrReg CreateAddrRegImpl(
    uint16_t index0, uint32_t stride0, uint16_t index1, uint32_t stride1, uint16_t index2, uint32_t stride2,
    uint16_t index3, uint32_t stride3)
{
    static_assert(
        sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8,
        "CreateAddrReg only support type b8/b16/b32/b64 on current device");
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    return VagCpuSim(sizeof(T), index0, stride0, index1, stride1, index2, stride2, index3, stride3);
#endif
    if constexpr (sizeof(T) == 1) {
        return vag_b8(stride0, stride1, stride2, stride3);
    } else if constexpr (sizeof(T) == 2) {
        return vag_b16(stride0, stride1, stride2, stride3);
    } else if constexpr (sizeof(T) == 8) {
        return vag_b32(stride0 * 2, stride1 * 2, stride2 * 2, stride3 * 2);
    }
    return vag_b32(stride0, stride1, stride2, stride3);
}
} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_ADDRREG_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_ADDRREG_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_ADDRREG_IMPL__
#endif
