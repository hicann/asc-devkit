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
 * \file kernel_operator_common_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_common_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_common_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_COMMON_INTF_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_COMMON_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_COMMON_INTERFACE_IMPL_H
#include "../../include/basic_api/kernel_tensor.h"
#include "../../include/basic_api/kernel_struct_mm.h"
#include "../../include/basic_api/kernel_operator_data_copy_intf.h"
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_INTF_H__
#endif
#include "../../include/basic_api/kernel_operator_sys_var_intf.h"

/*
 * ingroup：SetAtomicAdd
 * brief：Set the next data from UB to the outside of AI Core whether the move write Tensor operation performs
 * atomic accumulation.
 */
#if __NPU_ARCH__ == 1001
#include "dav_c100/kernel_operator_set_atomic_impl.h"
#include "dav_c100/kernel_operator_common_impl.h"
#include "dav_c100/kernel_operator_vec_duplicate_impl.h"
#include "dav_c100/kernel_operator_sync_impl.h"
#elif __NPU_ARCH__ == 2002
#include "dav_m200/kernel_operator_set_atomic_impl.h"
#include "dav_m200/kernel_operator_common_impl.h"
#include "dav_m200/kernel_operator_vec_duplicate_impl.h"
#include "dav_m200/kernel_operator_sync_impl.h"
#elif __NPU_ARCH__ == 2201
#include "dav_c220/kernel_operator_set_atomic_impl.h"
#include "dav_c220/kernel_operator_common_impl.h"
#include "dav_c220/kernel_operator_sync_impl.h"
#include "dav_c220/kernel_operator_vec_duplicate_impl.h"
#include "dav_c220/kfc/kfc_comm_client.h"
#include "dav_c220/kfc/kfc_comm_server.h"
#include "dav_c220/core_mng/roc/kernel_operator_cube_group_handle_impl.h"
#include "dav_c220/core_mng/roc/kernel_operator_group_barrier_impl.h"
#elif __NPU_ARCH__ == 3002
#include "dav_m300/kernel_operator_set_atomic_impl.h"
#elif __NPU_ARCH__ == 3003
#include "dav_l300/kernel_operator_sync_impl.h"
#include "dav_l300/kernel_operator_set_atomic_impl.h"
#include "dav_3510/kernel_operator_common_impl.h"
#elif __NPU_ARCH__ == 3102
#include "dav_m310/kernel_operator_set_atomic_impl.h"
#elif __NPU_ARCH__ == 3510
#include "dav_3510/kernel_operator_set_atomic_impl.h"
#include "dav_3510/kernel_operator_common_impl.h"
#include "dav_3510/kernel_operator_sync_impl.h"
#include "dav_3510/kernel_operator_vec_duplicate_impl.h"
#if KFC_C310_SSBUF == 1
#include "dav_3510/kfc/kfc_comm_client.h"
#include "dav_3510/kfc/kfc_comm_server.h"
#else
#include "dav_3510/kfc/kfc_comm_client_gm.h"
#include "dav_3510/kfc/kfc_comm_server_gm.h"
#endif
#include "dav_3510/core_mng/roc/kernel_operator_cube_group_handle_impl.h"
#include "dav_3510/core_mng/roc/kernel_operator_group_barrier_impl.h"
#elif (__NPU_ARCH__ == 5102)
#include "dav_m510/kernel_operator_set_atomic_impl.h"
#include "dav_m510/kernel_operator_common_impl.h"
#include "dav_m510/kernel_operator_sync_impl.h"
#include "dav_m510/kernel_operator_vec_duplicate_impl.h"
#elif (__NPU_ARCH__ == 3113)
#include "dav_l311/kernel_operator_sync_impl.h"
#include "dav_l311/kernel_operator_set_atomic_impl.h"
#include "dav_3510/kernel_operator_common_impl.h"
#elif (__NPU_ARCH__ == 5101) || (__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163)
#include "dav_5161/kernel_operator_sync_impl.h"
#include "dav_5161/kernel_operator_set_atomic_impl.h"
#include "dav_3510/kernel_operator_common_impl.h"
#endif
#include "kernel_pop_stack_buffer.h"

namespace AscendC {

/*
 * @ingroup：SetNextTaskStart, WaitPreTaskEnd
 * @brief：In SuperKernel fusion mode, set wait flag between two operators
 */
template <pipe_t AIV_PIPE, pipe_t AIC_PIPE, bool FORCE>
__aicore__ inline void SetNextTaskStart()
{
#ifdef __ASCENDC_ENABLE_SET_NEXT_TASK_START
    SetNextTaskStartImpl<AIV_PIPE, AIC_PIPE>();
#else
    if constexpr (FORCE) {
        SetNextTaskStartImpl<AIV_PIPE, AIC_PIPE, true>();
    }
#endif
}

template <bool FORCE>
__aicore__ inline void WaitPreTaskEnd()
{
#ifdef __ASCENDC_ENABLE_WAIT_PRE_TASK_END
    WaitPreTaskEndImpl();
#else
    if constexpr (FORCE) {
        WaitPreTaskEndImpl<-1, true>();
    }
#endif
}

__aicore__ inline void InitSocState() { AscendCUtils::InitSocStateImpl(); }

// NOTICE: InitOutput has been deprecated and will be removed in the next version. Please use Fill instead!
template <typename T>
__aicore__ inline __in_pipe__(V)
    __out_pipe__(MTE3) void InitOutput(GlobalTensor<T> gmWorkspaceAddr, uint32_t size, T value)
{
#if (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
#if (__NPU_ARCH__ != 5102)
    if ASCEND_IS_AIC {
        return;
    }
#endif
    LocalTensor<T> popBuffer;
    bool ret = PopStackBuffer<T, TPosition::LCM>(popBuffer);
    uint32_t maxBurstSize = (MAX_REPEAT_TIMES * ONE_BLK_SIZE) / sizeof(T);
    uint32_t popSize = popBuffer.GetSize() >= maxBurstSize ? maxBurstSize : popBuffer.GetSize();
    uint32_t round = size / popSize;
    uint32_t tail = size % popSize;
    uint32_t roundSize = round != 0 ? popSize : 0;
    DuplicateImpl<T>((__ubuf__ T*)popBuffer.GetPhyAddr(), value, popSize);
    event_t eventIDVToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE3));
    SetFlag<HardEvent::V_MTE3>(eventIDVToMTE3);
    WaitFlag<HardEvent::V_MTE3>(eventIDVToMTE3);
    struct DataCopyExtParams repeatParams;
    uint32_t comOffset = 0;
    // compute the main block
    repeatParams = {1, static_cast<uint32_t>(roundSize * sizeof(T)), 0, 0, 0};
    for (int index = 0; index < round; ++index) {
        DataCopyPadUB2GMImpl(
            (__gm__ T*)gmWorkspaceAddr.GetPhyAddr() + comOffset, (__ubuf__ T*)popBuffer.GetPhyAddr(), repeatParams);
        comOffset += roundSize;
    }
    // compute the tail block
    repeatParams = {1, static_cast<uint32_t>(tail * sizeof(T)), 0, 0, 0};
    if (tail != 0) {
        comOffset = round * roundSize;
        DataCopyPadUB2GMImpl(
            (__gm__ T*)gmWorkspaceAddr.GetPhyAddr() + comOffset, (__ubuf__ T*)popBuffer.GetPhyAddr(), repeatParams);
    }
#endif
}

template <AtomicDtype type, AtomicOp op>
__aicore__ inline void SetStoreAtomicConfig()
{
    SetStoreAtomicConfigImpl<static_cast<atomic_type_t>(type), static_cast<atomic_op_t>(op)>();
}

__aicore__ inline int64_t GetStoreAtomicConfig() { return GetStoreAtomicConfigImpl(); }

__aicore__ inline void GetStoreAtomicConfig(uint16_t& atomicType, uint16_t& atomicOp)
{
    GetStoreAtomicConfigImpl(atomicType, atomicOp);
}

__aicore__ inline void CheckLocalMemoryIA(const CheckLocalMemoryIAParam& checkParams)
{
    CheckLocalMemoryIAImpl(checkParams);
}

#if (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 3510) || \
    (__NPU_ARCH__ == 5102)
template <HardEvent event, MemoryT memT, bool isVirtual>
__aicore__ inline void HSetFlag(int32_t eventID)
{
    if (g_coreType == AIV) {
        return;
    }
    HSetFlagImpl<event, memT, isVirtual>(eventID);
}

template <HardEvent event, MemoryT memT, bool isVirtual>
__aicore__ inline void HWaitFlag(int32_t eventID)
{
    if (g_coreType == AIV) {
        return;
    }
    HWaitFlagImpl<event, memT, isVirtual>(eventID);
}
#endif

#if (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || \
    (__NPU_ARCH__ == 3113)
template <int8_t startBit, int8_t endBit>
__aicore__ static inline void SetCtrlSpr(int64_t value)
{
    SetCtrlSprImpl<startBit, endBit>(value);
}

template <int8_t startBit, int8_t endBit>
__aicore__ static inline int64_t GetCtrlSpr()
{
    return GetCtrlSprImpl<startBit, endBit>();
}

#if (__NPU_ARCH__ != 2201)
template <int8_t startBit, int8_t endBit>
__aicore__ static inline void ResetCtrlSpr()
{
    ResetCtrlSprImpl<startBit, endBit>();
}
#endif
#endif

#if (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3510)
namespace Internal {
template <SaturationMode mode>
__aicore__ inline constexpr int8_t GetSaturationModeBit()
{
    static_assert(IsSupportedSaturationMode<mode>(), "SaturationMode is not supported on current platform!");
    if constexpr (mode == SaturationMode::FLOAT) {
        return 48; // 0 => sat, 1 => no sat
    } else if constexpr (mode == SaturationMode::FLOAT8) {
        return 50; // 0 => sat, 1 => no sat
    } else if constexpr (mode == SaturationMode::INT) {
        return 53; // 0 => truncation, 1 => saturation
    } else {
        return 59; // 0 => saturation, 1 => truncation
    }
}

template <SaturationMode mode>
__aicore__ static inline void SetSaturationFlagImpl(bool enableSat)
{
    constexpr int8_t sprBit = GetSaturationModeBit<mode>();
    int64_t ctrlValue = get_ctrl();
    uint64_t value;
    if constexpr (mode == SaturationMode::INT) {
        value = enableSat ? sbitset1(ctrlValue, sprBit) : sbitset0(ctrlValue, sprBit);
    } else {
        value = enableSat ? sbitset0(ctrlValue, sprBit) : sbitset1(ctrlValue, sprBit);
    }
    set_ctrl(value);
}

template <SaturationMode mode>
__aicore__ static inline bool GetSaturationFlagImpl()
{
    constexpr int8_t sprBit = GetSaturationModeBit<mode>();
    int64_t value = (get_ctrl() >> sprBit) & 1;
    if constexpr (mode == SaturationMode::INT) {
        return value != 0;
    } else {
        return value == 0;
    }
}
} // namespace Internal

template <SaturationMode mode>
__aicore__ inline void SetSaturationFlag(bool enableSat)
{
    Internal::SetSaturationFlagImpl<mode>(enableSat);
}

template <SaturationMode mode>
__aicore__ inline bool GetSaturationFlag()
{
    return Internal::GetSaturationFlagImpl<mode>();
}
#endif

#if (__NPU_ARCH__ == 3510)
template <OverrideStrategy strategy>
__aicore__ inline void SetSaturationStrategy()
{
    int64_t ctrlValue = get_ctrl();
    if constexpr (strategy == OverrideStrategy::USE_API) {
        set_ctrl(sbitset0(ctrlValue, 60));
    } else {
        set_ctrl(sbitset1(ctrlValue, 60));
    }
}

__aicore__ inline OverrideStrategy GetSaturationStrategy()
{
    int64_t value = (get_ctrl() >> 60) & 1;
    return value == 0 ? OverrideStrategy::USE_API : OverrideStrategy::USE_GLOBAL;
}

template <CacheRwMode rwMode, CacheMode cacheMode>
__aicore__ inline void SetScalarCacheMode()
{
    SetScalarCacheModeImpl<rwMode, cacheMode>();
}

template <CacheRwMode rwMode>
__aicore__ inline CacheMode GetScalarCacheMode()
{
    return GetScalarCacheModeImpl<rwMode>();
}
#endif

} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_COMMON_INTERFACE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_COMMON_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_COMMON_INTF_IMPL_H__
#endif
