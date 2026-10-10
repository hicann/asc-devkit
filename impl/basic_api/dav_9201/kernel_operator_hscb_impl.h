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
 * \file kernel_operator_hscb_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_9201/kernel_operator_hscb_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/inner_kernel_operator_hscb_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_HSCB_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_HSCB_IMPL_H
#define ASCENDC_MODULE_OPERATOR_HSCB_IMPL_H

namespace AscendC {

namespace Hscb {

template <typename T, bool async = true>
__aicore__ inline void StoreImpl(uint64_t phyAddr, T value)
{
    if constexpr (sizeof(T) == B8_BYTE_SIZE) {
        __st_hscb((uint8_t)value, phyAddr, 0, static_cast<bisheng::cce::HscbReqType>(async));
    } else if constexpr (sizeof(T) == B16_BYTE_SIZE) {
        __st_hscb((uint16_t)value, phyAddr, 0, static_cast<bisheng::cce::HscbReqType>(async));
    } else if constexpr (sizeof(T) == B32_BYTE_SIZE) {
        __st_hscb((uint32_t)value, phyAddr, 0, static_cast<bisheng::cce::HscbReqType>(async));
    } else if constexpr (sizeof(T) == B64_BYTE_SIZE) {
        __st_hscb((uint64_t)value, phyAddr, 0, static_cast<bisheng::cce::HscbReqType>(async));
    }
}

template <typename T>
__aicore__ inline T LoadImpl(uint64_t phyAddr)
{
    if constexpr (sizeof(T) == B8_BYTE_SIZE) {
        return (T)(__ld_hscb_u8(phyAddr, 0));
    } else if constexpr (sizeof(T) == B16_BYTE_SIZE) {
        return (T)(__ld_hscb_u16(phyAddr, 0));
    } else if constexpr (sizeof(T) == B32_BYTE_SIZE) {
        return (T)(__ld_hscb_u32(phyAddr, 0));
    } else if constexpr (sizeof(T) == B64_BYTE_SIZE) {
        return (T)(__ld_hscb_s64(phyAddr, 0));
    }
    return T(0);
}

template <pipe_t pipe, typename T, bool async = true>
__aicore__ inline void SyncImpl(uint64_t phyAddr, T value)
{
    if constexpr (sizeof(T) == B8_BYTE_SIZE) {
        __sync_hscb((uint8_t)value, phyAddr, pipe, static_cast<bisheng::cce::HscbReqType>(async));
    } else if constexpr (sizeof(T) == B16_BYTE_SIZE) {
        __sync_hscb((uint16_t)value, phyAddr, pipe, static_cast<bisheng::cce::HscbReqType>(async));
    } else if constexpr (sizeof(T) == B32_BYTE_SIZE) {
        __sync_hscb((uint32_t)value, phyAddr, pipe, static_cast<bisheng::cce::HscbReqType>(async));
    } else if constexpr (sizeof(T) == B64_BYTE_SIZE) {
        __sync_hscb((uint64_t)value, phyAddr, pipe, static_cast<bisheng::cce::HscbReqType>(async));
    }
}

template <pipe_t pipe>
__aicore__ inline void WaitSprImpl(uint8_t sprID, uint16_t expectedValue)
{
    return __wait_ast_scb(sprID, expectedValue, pipe);
}

template <class T>
__aicore__ inline T AtomicAddImpl(uint64_t phyAddr, T value)
{
    static_assert(
        (SupportType<T, int32_t, uint32_t, int64_t, uint64_t>(),
         "current data type is not supported on current device!"));
    return (T)__atom_add_hscb(phyAddr, value);
}

template <class T>
__aicore__ inline T AtomicMinImpl(uint64_t phyAddr, T value)
{
    static_assert(
        (SupportType<T, int32_t, uint32_t, int64_t, uint64_t>(),
         "current data type is not supported on current device!"));
    return (T)__atom_min_hscb(phyAddr, value);
}

template <class T>
__aicore__ inline T AtomicMaxImpl(uint64_t phyAddr, T value)
{
    static_assert(
        (SupportType<T, int32_t, uint32_t, int64_t, uint64_t>(),
         "current data type is not supported on current device!"));
    return (T)__atom_max_hscb(phyAddr, value);
}

template <class T>
__aicore__ inline T AtomicCasImpl(uint64_t phyAddr, T compare, T value)
{
    static_assert(
        (SupportType<T, int32_t, uint32_t, int64_t, uint64_t>(),
         "current data type is not supported on current device!"));
    return (T)__atom_cas_hscb(phyAddr, compare, value);
}

template <class T>
__aicore__ inline T AtomicExchImpl(uint64_t phyAddr, T value)
{
    static_assert(
        (SupportType<T, int32_t, uint32_t, int64_t, uint64_t>(),
         "current data type is not supported on current device!"));
    return (T)__atom_exch_hscb(phyAddr, value);
}

template <class T>
__aicore__ inline void RedAtomicAddImpl(uint64_t phyAddr, T value)
{
    static_assert(
        (SupportType<T, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t>(),
         "current data type is not supported on current device!"));
    __red_add_hscb(phyAddr, value);
}

template <class T>
__aicore__ inline void RedAtomicMaxImpl(uint64_t phyAddr, T value)
{
    static_assert(
        (SupportType<T, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t>(),
         "current data type is not supported on current device!"));
    __red_max_hscb(phyAddr, value);
}

template <class T>
__aicore__ inline void RedAtomicMinImpl(uint64_t phyAddr, T value)
{
    static_assert(
        (SupportType<T, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t>(),
         "current data type is not supported on current device!"));
    __red_min_hscb(phyAddr, value);
}

} // namespace Hscb

} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_HSCB_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_HSCB_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_HSCB_IMPL_H__
#endif
