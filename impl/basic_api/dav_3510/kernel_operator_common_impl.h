/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_3510/kernel_operator_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_COMMON_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_COMMON_IMPL_H
#define ASCENDC_MODULE_OPERATOR_COMMON_IMPL_H
#include "../../../include/basic_api/kernel_common.h"
#include "../kernel_utils.h"
#include "../../../include/basic_api/kernel_struct_mm.h"
#include "../../../include/basic_api/kernel_base_types.h"
namespace AscendC {

__aicore__ inline void SetSysWorkspace(GM_ADDR workspace)
{
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    ASCENDC_ASSERT((workspace != nullptr), { KERNEL_LOG(KERNEL_ERROR, "workspace can not be nullptr"); });
#else
    if (g_sysWorkspaceReserved == nullptr) {
        g_sysWorkspaceReserved = workspace;
    }
#endif
}

__aicore__ inline void SetSysWorkspaceForce(GM_ADDR workspace)
{
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    ASCENDC_ASSERT((workspace != nullptr), { KERNEL_LOG(KERNEL_ERROR, "workspace can not be nullptr"); });
#else
#if (WORKSPACE_PARAM_OFFSET == 0xffffffff)
#if defined(__NPU_DEVICE__)
    __set_kfc_workspace_addr(workspace);
#else
    g_sysWorkspaceReserved = workspace;
#endif
#endif
#endif
}

__aicore__ inline GM_ADDR GetUserWorkspace(GM_ADDR workspace)
{
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    ASCENDC_ASSERT((workspace != nullptr), { KERNEL_LOG(KERNEL_ERROR, "workspace can not be nullptr"); });
    return workspace;
#else
    (void)(workspace);
#if ENABLE_CV_COMM_VIA_SSBUF != 0 && __MIX_CORE_AIC_RATION__ != 1
    return GetSysWorkSpacePtr();
#else
    return GetSysWorkSpacePtr() + RESERVED_WORKSPACE;
#endif
#endif
}

__aicore__ inline int64_t GetStoreAtomicConfigImpl() { return get_st_atomic_cfg(); }

__aicore__ inline void GetStoreAtomicConfigImpl(uint16_t& atomicType, uint16_t& atomicOp)
{
    int64_t stAtomic = get_st_atomic_cfg();
    constexpr uint64_t typeMask = 0x7;
    constexpr uint64_t opBit = 4;
    constexpr uint64_t opMask = 0x3;
    atomicType = (static_cast<uint64_t>(stAtomic) & typeMask);
    atomicOp = ((static_cast<uint64_t>(stAtomic) >> opBit) & opMask);
}

__aicore__ inline void CheckLocalMemoryIAImpl(const CheckLocalMemoryIAParam& checkParams)
{
    (void)(checkParams);
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "CheckLocalMemoryIA is not supported on current device"); });
}

template <atomic_type_t type, atomic_op_t op>
__aicore__ inline void SetStoreAtomicConfigImpl()
{
    set_st_atomic_cfg(type, op);
}

template <int8_t startBit, int8_t endBit>
__aicore__ static inline void SetCtrlSprImpl(int64_t value)
{
    static_assert((startBit <= endBit && startBit >= 0 && endBit < 64), "Invalid bit range on current device!");
    static_assert(
        (6 <= startBit && startBit <= 10 && 6 <= endBit && endBit <= 10) ||
            (startBit == endBit && (startBit == 45 || startBit == 48 || startBit == 50 || startBit == 53 ||
                                    startBit == 59 || startBit == 60)),
        "Invalid startBit/endBit on current device!");
    if (endBit - startBit == 63) {
        set_ctrl(value);
        return;
    }
    uint64_t mask = ((uint64_t(1) << (endBit - startBit + 1)) - 1) << startBit;
    mask = ~mask;
    int64_t setValue = get_ctrl() & mask;
    setValue |= (value << startBit);
    set_ctrl(setValue);
}

template <int8_t startBit, int8_t endBit>
__aicore__ static inline int64_t GetCtrlSprImpl()
{
    static_assert((startBit <= endBit && startBit >= 0 && endBit < 64), "Invalid bit range on current device!");
    int64_t value = get_ctrl();
    if (endBit - startBit == 63) {
        return value;
    }
    value = value >> startBit;
    value &= ((uint64_t(1) << (endBit - startBit + 1)) - 1);
    return value;
}

template <int8_t startBit, int8_t endBit>
__aicore__ static inline void ResetCtrlSprImpl()
{
    static_assert((startBit <= endBit && startBit >= 0 && endBit < 64), "Invalid bit range on current device!");
    static_assert(
        (6 <= startBit && startBit <= 10 && 6 <= endBit && endBit <= 10) ||
            (startBit == endBit && (startBit == 45 || startBit == 48 || startBit == 50 || startBit == 53 ||
                                    startBit == 59 || startBit == 60)),
        "Invalid startBit/endBit on current device!");
    int64_t defaultCtrl = 0x1000000000000008; // default value of ctrl
    if (endBit - startBit == 63) {
        set_ctrl(defaultCtrl);
        return;
    }
    uint64_t mask = ((uint64_t(1) << (endBit - startBit + 1)) - 1) << startBit;
    defaultCtrl = defaultCtrl & mask;
    mask = ~mask;
    int64_t value = get_ctrl() & mask;
    value = value | defaultCtrl;
    set_ctrl(value);
}

namespace Internal {
template <SaturationMode mode>
__aicore__ inline constexpr bool IsSupportedSaturationMode()
{
    return mode == SaturationMode::FLOAT || mode == SaturationMode::FLOAT8 || mode == SaturationMode::INT ||
           mode == SaturationMode::CAST;
}
} // namespace Internal

template <CacheMode cacheMode>
__aicore__ inline constexpr uint8_t GetCacheModeBits()
{
    switch (cacheMode) {
        case CacheMode::CACHE_MODE_DISABLE:
            return 4; // 4'b0100
        case CacheMode::CACHE_MODE_NORMAL:
            return 0; // 4'b0000
        case CacheMode::CACHE_MODE_LAST:
            return 1; // 4'b0001
        case CacheMode::CACHE_MODE_PERSISTENT:
            return 2; // 4'b0010
        default:
            return 0;
    }
}

template <CacheRwMode rwMode, CacheMode cacheMode>
__aicore__ inline void SetScalarCacheModeImpl()
{
    static_assert(
        rwMode == CacheRwMode::READ || rwMode == CacheRwMode::WRITE,
        "SetScalarCacheMode only supports READ or WRITE mode, RW mode is not supported");
    // READ mode: CTRL[19:16], WRITE mode: CTRL[23:20]
    constexpr uint8_t startBit = (rwMode == CacheRwMode::READ) ? 16 : 20;
    constexpr uint8_t cacheBits = GetCacheModeBits<cacheMode>();

    uint64_t mask = ~((uint64_t(0xF) << startBit));
    int64_t ctrlValue = get_ctrl() & mask;
    ctrlValue |= (static_cast<uint64_t>(cacheBits) << startBit);
    set_ctrl(ctrlValue);
}

template <CacheRwMode rwMode>
__aicore__ inline CacheMode GetScalarCacheModeImpl()
{
    static_assert(
        rwMode == CacheRwMode::READ || rwMode == CacheRwMode::WRITE,
        "GetScalarCacheMode only supports READ or WRITE mode, RW mode is not supported");
    // READ mode: CTRL[19:16], WRITE mode: CTRL[23:20]
    constexpr uint8_t startBit = (rwMode == CacheRwMode::READ) ? 16 : 20;
    int64_t ctrlValue = get_ctrl();
    uint8_t cacheBits = static_cast<uint8_t>((ctrlValue >> startBit) & 0xF);

    switch (cacheBits) {
        case 0:
            return CacheMode::CACHE_MODE_NORMAL;
        case 1:
            return CacheMode::CACHE_MODE_LAST;
        case 2:
            return CacheMode::CACHE_MODE_PERSISTENT;
        case 4:
            return CacheMode::CACHE_MODE_DISABLE;
        default:
            return CacheMode::CACHE_MODE_NORMAL;
    }
}

} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_COMMON_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_COMMON_IMPL_H__
#endif
