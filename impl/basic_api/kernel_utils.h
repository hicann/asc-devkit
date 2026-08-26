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
 * \file kernel_utils.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_utils.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_utils_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_UTILS_H__
#endif
#ifndef ASCENDC_MODULE_UTILS_H
#define ASCENDC_MODULE_UTILS_H
#include "utils/kernel_utils_macros.h"
#include "utils/kernel_utils_ceil_oom_que.h"
#include "utils/kernel_utils_constants.h"
#include "utils/kernel_utils_mode.h"
#include "utils/kernel_utils_struct_confusion_pad.h"
#include "utils/kernel_utils_struct_dma_params.h"
#include "utils/kernel_utils_struct_norm_sort.h"
#include "utils/kernel_utils_struct_param.h"
#include "../utils/debug/asc_debug_utils.h"
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0) &&             \
    ((defined(ASCENDC_CPU_DEBUG) && (ASCENDC_CPU_DEBUG == 1)) || \
     (defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510) && !defined(__ASC_DISABLE_RESERVED_UBUF__)))
#define ASCENDC_INTERNAL_SIMD_VF_OVERFLOW_WARNING_ENABLED
#include "../utils/debug/asc_aicore_printf_impl.h"
#endif

#include "../utils/base/helpers_impl.h"
#include "../../include/basic_api/kernel_struct_data_copy.h"
#include "kernel_scalar_convert.h"
#include "kernel_utils_base.h"

#if ENABLE_CV_COMM_VIA_SSBUF != 0 && __MIX_CORE_AIC_RATION__ != 1
#define KFC_C310_SSBUF 1
#else
#define KFC_C310_SSBUF 0
#endif

inline __gm__ void* g_sysFftsAddr = nullptr;
namespace AscendC {
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
namespace Internal {
__BLOCK_LOCAL__ __inline__ half g_deqValue;
}
#endif

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
namespace Internal {
__BLOCK_LOCAL__ __inline__ uint64_t g_rptConfig;
}
#endif

#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || \
     __NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
namespace Internal {
// global variables g_cmpMaskLow and g_cmpMaskHigh are used to simulate the register CMPMASK in 1971
// both of them are 64 bits and they are used to store the result of API Compare
__BLOCK_LOCAL__ __inline__ uint64_t g_cmpMaskLow;
__BLOCK_LOCAL__ __inline__ uint64_t g_cmpMaskHigh;
// the global variable g_deqScale is used to store the scale offset and signMode of API CastDeq
// when you are using API "SetDeqScaleImpl(float scale, int16_t offset, bool signMode)", g_deqScale will save
// the result of the transformation of three variable data
// otherwise, if you are using API "SetDeqScaleImpl(const LocalTensor<T> &vdeq, const VdeqInfo &vdeqInfo)"
// g_deqScale will store the UB address of vdeq, and the data of vdeqInfo will be stored in vdeq
__BLOCK_LOCAL__ __inline__ uint64_t g_deqScale;
// manage the global id for get/rls buff.
__BLOCK_LOCAL__ __inline__ uint32_t g_bufId;
__BLOCK_LOCAL__ __inline__ uint32_t g_sharedEvtId;
// global variables g_aipp* are used to simulate the spr for SetAippFunctions and LoadImageToLocal, they will save
// the configs and apply them to pre-process the input image in LoadImageToLocal function.
__BLOCK_LOCAL__ __inline__ uint64_t g_aippSrc0;
__BLOCK_LOCAL__ __inline__ uint64_t g_aippSrc1;
__BLOCK_LOCAL__ __inline__ uint64_t g_aippCscRc0;
__BLOCK_LOCAL__ __inline__ uint64_t g_aippCscRc1;
__BLOCK_LOCAL__ __inline__ uint64_t g_aippCscBias;
__BLOCK_LOCAL__ __inline__ uint64_t g_aippDtcMean;
__BLOCK_LOCAL__ __inline__ uint64_t g_aippDtcMin;
__BLOCK_LOCAL__ __inline__ uint64_t g_aippDtcVar;
__BLOCK_LOCAL__ __inline__ uint64_t g_aippPaddingVal;
__BLOCK_LOCAL__ __inline__ uint64_t g_aippArgs;

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || \
                              __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
__BLOCK_LOCAL__ __inline__ int64_t g_accVal;
#endif
} // namespace Internal
#endif

#ifdef ASCENDC_CPU_DEBUG
enum AtomicType { SUM, MAX, MIN };
extern bool g_isAtomic;
extern AtomicType g_atomicType;

template <typename T>
__aicore__ inline void DataCopyWithAtomic(__gm__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
    if (!g_isAtomic) {
        return;
    }
    const uint16_t nBurst = intriParams.blockCount;
    const uint16_t lenBurst = intriParams.blockLen;
    const uint16_t srcStride = intriParams.srcStride;
    const uint16_t dstStride = intriParams.dstStride;
    // new one buffer and do add
    uint32_t dstOffset = 0;
    uint32_t srcOffset = 0;
    const int repeatTime = (lenBurst * ONE_BLK_SIZE + ONE_REPEAT_BYTE_SIZE - 1) / ONE_REPEAT_BYTE_SIZE;
    for (int index = 0; index < nBurst; ++index) {
        for (int indexJ = 0; indexJ < lenBurst * ONE_BLK_SIZE / sizeof(T); ++indexJ) {
            if (g_atomicType == SUM) {
                *(static_cast<T*>(src) + srcOffset + indexJ) =
                    *(static_cast<T*>(dst) + dstOffset + indexJ) + *(static_cast<T*>(src) + srcOffset + indexJ);
            } else if (g_atomicType == MAX) {
                *(static_cast<T*>(src) + srcOffset + indexJ) = std::max(
                    *(static_cast<T*>(dst) + dstOffset + indexJ), *(static_cast<T*>(src) + srcOffset + indexJ));
            } else {
                *(static_cast<T*>(src) + srcOffset + indexJ) = std::min(
                    *(static_cast<T*>(dst) + dstOffset + indexJ), *(static_cast<T*>(src) + srcOffset + indexJ));
            }
        }
        dstOffset += ((lenBurst + dstStride) * ONE_BLK_SIZE) / sizeof(T);
        srcOffset += ((lenBurst + srcStride) * ONE_BLK_SIZE) / sizeof(T);
    }
}

template <typename T>
__aicore__ inline void DataCopyWithAtomicCom(__gm__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
    const uint16_t nBurst = intriParams.blockCount;
    const uint16_t lenBurst = intriParams.blockLen;
    const uint16_t srcStride = intriParams.srcStride;
    const uint16_t dstStride = intriParams.dstStride;
    const uint16_t halfSize = sizeof(T);
    // new one buffer and do add
    uint32_t dstOffset = 0;
    uint32_t srcOffset = 0;
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 1001) || (__NPU_ARCH__ == 2002) || (__NPU_ARCH__ == 2201))
    const int repeatTime = (lenBurst * ONE_BLK_SIZE) / ONE_REPEAT_BYTE_SIZE;
    const int countInRepeat = (ONE_REPEAT_BYTE_SIZE / halfSize);
    const int tail = lenBurst * ONE_BLK_SIZE / halfSize - repeatTime * countInRepeat;
#else
    (void)src;
    (void)dst;
#endif
    for (int index = 0; index < nBurst; ++index) {
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 1001) || (__NPU_ARCH__ == 2002) || (__NPU_ARCH__ == 2201))
        __ubuf__ T* dstAddr = static_cast<__ubuf__ T*>(src) + srcOffset;
        __ubuf__ T* src0Addr = static_cast<__ubuf__ T*>(dst) + dstOffset;
        __ubuf__ T* src1Addr = static_cast<__ubuf__ T*>(src) + srcOffset;
        if (repeatTime > 0) {
            AscendCUtils::SetMask<T>(countInRepeat);
            if (g_atomicType == SUM) {
                vadd(
                    static_cast<T*>(dstAddr), static_cast<T*>(src0Addr), static_cast<T*>(src1Addr), repeatTime, 1, 1, 1,
                    DEFAULT_BLK_NUM, DEFAULT_BLK_NUM, DEFAULT_BLK_NUM);
            } else if (g_atomicType == MAX) {
                vmax(
                    static_cast<T*>(dstAddr), static_cast<T*>(src0Addr), static_cast<T*>(src1Addr), repeatTime, 1, 1, 1,
                    DEFAULT_BLK_NUM, DEFAULT_BLK_NUM, DEFAULT_BLK_NUM);
            } else {
                vmin(
                    static_cast<T*>(dstAddr), static_cast<T*>(src0Addr), static_cast<T*>(src1Addr), repeatTime, 1, 1, 1,
                    DEFAULT_BLK_NUM, DEFAULT_BLK_NUM, DEFAULT_BLK_NUM);
            }
            AscendCUtils::ResetMask();
        }
        if (tail != 0) {
            dstAddr = dstAddr + repeatTime * countInRepeat;
            src0Addr = src0Addr + repeatTime * countInRepeat;
            src1Addr = src1Addr + repeatTime * countInRepeat;
            AscendCUtils::SetMask<T>(tail);
            if (g_atomicType == SUM) {
                vadd(
                    static_cast<T*>(dstAddr), static_cast<T*>(src0Addr), static_cast<T*>(src1Addr), 1, 1, 1, 1,
                    DEFAULT_BLK_NUM, DEFAULT_BLK_NUM, DEFAULT_BLK_NUM);
            } else if (g_atomicType == MAX) {
                vmax(
                    static_cast<T*>(dstAddr), static_cast<T*>(src0Addr), static_cast<T*>(src1Addr), 1, 1, 1, 1,
                    DEFAULT_BLK_NUM, DEFAULT_BLK_NUM, DEFAULT_BLK_NUM);
            } else {
                vmin(
                    static_cast<T*>(dstAddr), static_cast<T*>(src0Addr), static_cast<T*>(src1Addr), 1, 1, 1, 1,
                    DEFAULT_BLK_NUM, DEFAULT_BLK_NUM, DEFAULT_BLK_NUM);
            }
            AscendCUtils::ResetMask();
        }
#endif
        dstOffset += ((lenBurst + dstStride) * ONE_BLK_SIZE) / halfSize;
        srcOffset += ((lenBurst + srcStride) * ONE_BLK_SIZE) / halfSize;
    }
}

__aicore__ inline void DataCopyWithAtomic(__gm__ half* dst, __ubuf__ half* src, const DataCopyParams& intriParams)
{
    if (!g_isAtomic) {
        return;
    }
    DataCopyWithAtomicCom(dst, src, intriParams);
}
__aicore__ inline void DataCopyWithAtomic(__gm__ float* dst, __ubuf__ float* src, const DataCopyParams& intriParams)
{
    if (!g_isAtomic) {
        return;
    }
    DataCopyWithAtomicCom(dst, src, intriParams);
}

#if (__NPU_ARCH__ == 3002)
__aicore__ inline void DataCopyWithAtomic(__gm__ int16_t* dst, __ubuf__ int16_t* src, const DataCopyParams& intriParams)
{
    if (!g_isAtomic) {
        return;
    }
    DataCopyWithAtomicCom(dst, src, intriParams);
}

__aicore__ inline void DataCopyWithAtomic(__gm__ int32_t* dst, __ubuf__ int32_t* src, const DataCopyParams& intriParams)
{
    if (!g_isAtomic) {
        return;
    }
    DataCopyWithAtomicCom(dst, src, intriParams);
}
#endif
#endif // ASCENDC_CPU_DEBUG

/***************内部定义time stamp id**************************
定义值范围: 0x000 - 0xfff

time stamp id按块分组，快说明如下:
TIME_STAMP_WRAP: NPU套壳函数中的时间戳打点
TIME_STAMP_TPIPE/BUFFER: TPIPE、BUFFER中的时间戳打点
TIME_STAMP_MATMUL: MATMUL相关时间戳打点
TIME_STAMP_TILING_DATA: TILING DATA模块时间戳打点
TIME_STAMP_MC2_START/END: MC2模块使用打点id范围

TimeStampId更新原则：每个分组新增ID不可改变原有定义的ID值！

***************************************************************/
enum class TimeStampId : uint32_t {
    TIME_STAMP_WRAP_FIRST = 0x000,
    TIME_STAMP_WRAP_MC2_CTX,
    TIME_STAMP_WRAP_WK_SPACE,
    TIME_STAMP_WRAP_INIT_DUMP,
    TIME_STAMP_WRAP_FFTS_ADDR,
    TIME_STAMP_WRAP_CLEAR_WK_SPAC,

    TIME_STAMP_TPIPE = 0x030,
    TIME_STAMP_BUFFER,
    TIME_STAMP_MTE2_DATACOPY,

    TIME_STAMP_MATMUL_SERVER = 0x060,
    TIME_STAMP_MATMUL_SERVER_INIT,
    TIME_STAMP_MATMUL_SERVER_OBJ,
    TIME_STAMP_MATMUL_MATRIX_KFC,
    TIME_STAMP_MATMUL_CLIENT_KFC,
    TIME_STAMP_MATMUL_WAIT_EVE,
    TIME_STAMP_MATMUL_OBJ,

    TIME_STAMP_TILING_DATA = 0x090,
    TIME_STAMP_TILING_DATA_STRUCT,
    TIME_STAMP_TILING_DATA_MEMBER,

    // MC2 :0x1000-0x1fff
    TIME_STAMP_MC2_START = 0x1000,
    TIME_STAMP_MC2_END = 0x1fff,

    TIME_STAMP_MAX = 0xffff,
};
} // namespace AscendC
#endif // ASCENDC_MODULE_UTILS_H
#ifdef ASCENDC_INTERNAL_SIMD_VF_OVERFLOW_WARNING_ENABLED
#undef ASCENDC_INTERNAL_SIMD_VF_OVERFLOW_WARNING_ENABLED
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_UTILS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_UTILS_H__
#endif
