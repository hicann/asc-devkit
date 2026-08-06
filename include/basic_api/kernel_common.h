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
 * \file kernel_common.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_COMMON_H__
#endif

#ifndef ASCENDC_KERNEL_COMMON_H
#define ASCENDC_KERNEL_COMMON_H

#include "../../impl/basic_api/kernel_macros.h"
#include "../../impl/basic_api/kernel_event.h"
#include "../../impl/basic_api/kernel_log.h"
#include "../../impl/basic_api/kernel_reg.h"
#include "../../impl/basic_api/kernel_process_lock.h"
#include "kernel_struct_aipp.h"
#include "../../impl/basic_api/kernel_utils.h"
#include "../../impl/basic_api/utils/kernel_utils_macros.h"
#include "kernel_operator_swap_mem_intf.h"
#include "kernel_operator_sys_var_intf.h"

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <cstdint>
#include "stub_def.h"
#include "stub_fun.h"
#include "kernel_fp16.h"
#endif

namespace AscendC {
class TPipe;

template <typename T>
class GlobalTensor;
} // namespace AscendC

template <auto funcPtr, typename... Args> __aicore__ inline void asc_vf_call(Args &&... args)
{
    if ASCEND_IS_AIV {
        AscendC::AscVFCallImpl<funcPtr, AscendC::Internal::SimdVfDebugTag>(args...);
    }
}

#if __NPU_ARCH__ == 2201 || (__NPU_ARCH__ == 3510)
__BLOCK_LOCAL__ __inline__ uint32_t g_super_kernel_early_start_config;
#endif

// disabled on CPU mode for compatiablilty
#if defined(ASCENDC_DEBUG) && !defined(ASCENDC_CPU_DEBUG)
#ifdef SPLIT_CORE_CUBE
__BLOCK_LOCAL__ __inline__ AscendC::TPipe* g_cubeTPipePtr = nullptr;
#elif defined(SPLIT_CORE_VEC)
__BLOCK_LOCAL__ __inline__ AscendC::TPipe* g_vecTPipePtr = nullptr;
#else
__BLOCK_LOCAL__ __inline__ AscendC::TPipe* g_tPipePtr = nullptr;
#endif
__BLOCK_LOCAL__ __inline__ uint64_t g_lastTpipeInitPos = 0;
#else // end ASCENDC_DEBUG
#ifdef SPLIT_CORE_CUBE
__BLOCK_LOCAL__ __inline__ AscendC::TPipe* g_cubeTPipePtr;
#elif defined(SPLIT_CORE_VEC)
__BLOCK_LOCAL__ __inline__ AscendC::TPipe* g_vecTPipePtr;
#else
__BLOCK_LOCAL__ __inline__ AscendC::TPipe* g_tPipePtr;
#endif
#endif // end ASCENDC_DEBUG

#if __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 3102 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113
__BLOCK_LOCAL__ __inline__ uint64_t g_maskCount;
#if __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 3102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113
__BLOCK_LOCAL__ __inline__ half g_deqValue;
#endif
#endif
#if __NPU_ARCH__ == 2201
__BLOCK_LOCAL__ __inline__ half g_deqValue;
#endif

__BLOCK_LOCAL__ __inline__ __gm__ uint8_t* g_dumpWorkspaceReserved;
__BLOCK_LOCAL__ __inline__ __gm__ uint8_t* g_hcclContextReserved[2];

#if defined(UT_TEST) || defined(ST_TEST)
__aicore__ AscendC::TPipe* GetTPipePtr();
#else
__aicore__ inline AscendC::TPipe* GetTPipePtr()
{
#ifdef SPLIT_CORE_CUBE
    return g_cubeTPipePtr;
#elif defined(SPLIT_CORE_VEC)
    return g_vecTPipePtr;
#else
    return g_tPipePtr;
#endif
}
#endif

namespace AscendC {
template <typename T, MaskMode mode = MaskMode::NORMAL>
__aicore__ static inline void SetVectorMask(const uint64_t maskHigh, const uint64_t maskLow)
{
#if __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 3102
    if (mode == MaskMode::COUNTER) {
        g_maskCount = maskLow;
    }
#endif
    SetVectorMaskImpl<T, mode>(maskHigh, maskLow);
}

template <typename T, MaskMode mode = MaskMode::NORMAL>
__aicore__ static inline void SetVectorMask(int32_t len)
{
#if __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 3102
    g_maskCount = len;
#endif
    SetVectorMaskImpl<T, mode>(len);
}

__aicore__ inline void ResetMask()
{
#if __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 3102
    g_maskCount = 0;
#endif
    ResetMaskImpl();
}

using MutexID = uint8_t;

class Mutex {
public:
    template <pipe_t pipe>
    static __aicore__ inline void Lock(MutexID id)
    {
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510))
        ASCENDC_ASSERT((id <= MAX_MUTEXID),
            { KERNEL_LOG(KERNEL_ERROR, "For Mutex::Lock current id is %u, max MutexID is %u", id, MAX_MUTEXID); });
        GetBufInternal<pipe, 0>(id);
#endif
    }

    template <pipe_t pipe>
    static __aicore__ inline void Unlock(MutexID id)
    {
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510))
        ASCENDC_ASSERT((id <= MAX_MUTEXID),
            { KERNEL_LOG(KERNEL_ERROR, "For Mutex::Unlock current id is %u, max MutexID is %u", id, MAX_MUTEXID); });
        RlsBufInternal<pipe, 0>(id);
#endif
    }
};

__aicore__ inline MutexID AllocMutexID()
{
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510))
    MutexID id = static_cast<uint8_t>(sff0(Internal::g_bufId));
    Internal::g_bufId = sbitset1(Internal::g_bufId, id);
    ASCENDC_ASSERT((id <= MAX_MUTEXID), {
        KERNEL_LOG(KERNEL_ERROR, "current id is %u, max buffer ID allocated is %u", static_cast<uint32_t>(id),
                   static_cast<uint32_t>(MAX_MUTEXID));
    });
    return id;
#else
    return 0;
#endif
}

__aicore__ inline void ReleaseMutexID(MutexID id)
{
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510))
    ASCENDC_ASSERT((id < MAX_MUTEXID), {
        KERNEL_LOG(KERNEL_ERROR, "current id is %d, which should be larger than or equals to 0, and smaller than %d",
            static_cast<int32_t>(id), MAX_MUTEXID);
    });
    Internal::g_bufId = sbitset0(Internal::g_bufId, id);
#endif
}


__aicore__ inline void SetMaskCount()
{
    SetMaskCountImpl();
}

__aicore__ inline void SetMaskNorm()
{
    SetMaskNormImpl();
}

template <uint32_t index>
__aicore__ inline void SetHcclContext(__gm__ uint8_t* context)
{
    if constexpr (index > 1) {
        return;
    }
    g_hcclContextReserved[index] = context;
}

template <uint32_t index>
__aicore__ inline __gm__ uint8_t* __gm__ GetHcclContext(void)
{
    if constexpr (index > 1) {
        return nullptr;
    }
    return g_hcclContextReserved[index];
}


template <typename T, typename U>
__aicore__ inline void SetAippFunctions(const GlobalTensor<T>& src0, AippInputFormat format, AippParams<U> config)
{
#if defined(__NPU_ARCH__) &&                                                            \
    ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 2002) || (__NPU_ARCH__ == 3002) ||      \
     (__NPU_ARCH__ == 3510))
    SetAippFunctionsImpl<PrimT<T>, U>(const_cast<__gm__ PrimT<T>*>(src0.GetPhyAddr()), format, config);
#endif
}

template <typename T, typename U>
__aicore__ inline void SetAippFunctions(const GlobalTensor<T>& src0, const GlobalTensor<T>& src1,
                                        AippInputFormat format, AippParams<U> config)
{
#if defined(__NPU_ARCH__) &&                                                            \
    ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 2002) || (__NPU_ARCH__ == 3002) ||      \
     (__NPU_ARCH__ == 3510))
    SetAippFunctionsImpl<PrimT<T>, U>(const_cast<__gm__ PrimT<T>*>(src0.GetPhyAddr()),
                                      const_cast<__gm__ PrimT<T>*>(src1.GetPhyAddr()), format, config);
#endif // (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 2002) || (__NPU_ARCH__ == 3002)
}
} // namespace AscendC

[[deprecated("NOTICE: SetDumpWorkSpacePtr has been deprecated and will be removed in the next version. "
             "Please do not use it!")]]
__aicore__ inline __gm__ uint8_t* __gm__ SetDumpWorkSpacePtr(__gm__ uint8_t* workspace)
{
    return g_dumpWorkspaceReserved = workspace;
}
[[deprecated("NOTICE: GetDumpWorkSpacePtr has been deprecated and will be removed in the next version. "
             "Please do not use it!")]]
__aicore__ inline __gm__ uint8_t* __gm__ GetDumpWorkSpacePtr()
{
    return g_dumpWorkspaceReserved;
}
#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_COMMON_H__
#endif
