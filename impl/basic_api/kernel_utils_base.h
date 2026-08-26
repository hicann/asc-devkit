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
 * \file kernel_utils_base.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_utils_base.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_utils_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_UTILS_BASE_H__
#endif
#ifndef ASCENDC_MODULE_UTILS_BASE_H
#define ASCENDC_MODULE_UTILS_BASE_H
#include "utils/kernel_utils_macros.h"
#include "utils/kernel_utils_ceil_oom_que.h"
#include "utils/kernel_utils_constants.h"
#include "utils/kernel_utils_mode.h"
#include "utils/kernel_utils_struct_confusion_pad.h"
#include "utils/kernel_utils_struct_dma_params.h"
#include "utils/kernel_utils_struct_norm_sort.h"
#include "utils/kernel_utils_struct_param.h"

#include "../../include/basic_api/kernel_struct_data_copy.h"
#include "kernel_scalar_convert.h"

namespace AscendC {
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
namespace Internal {
__BLOCK_LOCAL__ extern __inline__ half g_deqValue;
}
#endif
class AscendCUtils {
public:
    __aicore__ static inline int32_t GetBitSize(int32_t byteSize) { return byteSize * ONE_BYTE_BIT_SIZE; }

    __aicore__ static inline int32_t GetC0Size() { return DEFAULT_C0_SIZE; }

    __aicore__ static inline void InitCoupledArchSpr()
    {
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5102)
        set_padding(static_cast<uint64_t>(0));
        set_vector_mask(static_cast<uint64_t>(-1), static_cast<uint64_t>(-1));
        uint64_t loopSizePara = (1uL << 21) | 1uL;
        set_loop_size_ubtoout(loopSizePara);
        set_loop_size_outtoub(loopSizePara);
        set_st_atomic_cfg(0b00100100);
#endif
    }

    __aicore__ static inline void InitSplitArchSpr()
    {
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
        if ASCEND_IS_AIC {
            set_padding(static_cast<uint64_t>(0));
        } else {
            set_vector_mask(static_cast<uint64_t>(-1), static_cast<uint64_t>(-1));
            uint64_t loopSizePara = (1uL << 21) | 1uL;
            set_loop_size_ubtoout(loopSizePara);
            set_loop_size_outtoub(loopSizePara);
        }
        set_st_atomic_cfg(0b00100100);
#endif
    }

    __aicore__ static inline void InitSocStateImpl()
    {
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || \
                              __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
#else
        set_atomic_none();
#endif
#if __NPU_ARCH__ == 2201
        set_mask_norm();
        if ASCEND_IS_AIC {
            set_l1_3d_size(static_cast<uint64_t>(0));
            set_padding(static_cast<uint64_t>(0));
        } else {
            set_vector_mask(static_cast<uint64_t>(-1), static_cast<uint64_t>(-1));
        }
#elif (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
        set_mask_norm();
        Internal::g_deqValue = static_cast<half>(1);
        uint64_t prevCtrl = get_ctrl() & 0x1000000000000;
        uint64_t val = 0x1000000000000008 | prevCtrl;
        set_ctrl(val);
#if (__NPU_ARCH__ == 5102)
        InitCoupledArchSpr();
#else
        InitSplitArchSpr();
#endif
#elif __NPU_ARCH__ == 3002
        set_padding(static_cast<uint64_t>(0));
#endif
    }

    __aicore__ static inline int32_t GetC0Count(const int32_t dtypeSize)
    {
        ASCENDC_ASSERT((dtypeSize != 0), { KERNEL_LOG(KERNEL_ERROR, "dtypeSize can not be 0"); });
        return GetC0Size() / dtypeSize;
    }

    __aicore__ static inline int32_t GetDefaultBlockNum() { return DEFAULT_BLK_NUM; }

    __aicore__ static inline int64_t GetRsvdCnt() { return get_rsvd_cnt(); }

    template <typename T, bool isSetMask = true>
    __aicore__ static inline void SetMask(const uint64_t& maskHigh, const uint64_t& maskLow)
    {
        if constexpr (!isSetMask) {
            return;
        }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
        if (sizeof(T) >= sizeof(int32_t)) {
            ASCENDC_ASSERT(
                (maskHigh == 0ULL), { KERNEL_LOG(KERNEL_ERROR, "maskHigh must be 0 for type b32 and b64"); });
        }
        ASCENDC_ASSERT(((maskLow != 0ULL) || (maskHigh != 0ULL)), {
            KERNEL_LOG(KERNEL_ERROR, "maskLow and maskHigh can not be zero at the same time");
        });
#endif
#endif
        if ASCEND_IS_NOT_AIC {
            set_vector_mask(maskHigh, maskLow);
        }
    }

    template <typename T, bool isSetMask = true>
    __aicore__ static inline void SetMask(int32_t len)
    {
        if constexpr (!isSetMask) {
            return;
        }

        int32_t typeLen = 0;
        if constexpr (IsSameType<T, int4b_t>::value) {
            typeLen = DEFAULT_BLOCK_SIZE * INT4_TWO;
#if (__NPU_ARCH__ == 5102)
        } else if constexpr (IsSameType<T, int2b_t>::value) {
            typeLen = DEFAULT_BLOCK_SIZE * INT2_FOUR;
        } else if constexpr (IsSameType<T, uint1b_t>::value) {
            typeLen = DEFAULT_BLOCK_SIZE * INT1_EIGHT;
#endif
        } else {
            typeLen = DEFAULT_BLOCK_SIZE / sizeof(T);
        }
        constexpr int32_t halfTypeLen = 64; // 1 register -> 64 bits -> 64 elements
        constexpr int32_t lenCoeff = 2;     // 2 registers for masks
        if (len == halfTypeLen) {
            SetMask<T>(0, FULL_MASK);
            return;
        } else if (len == typeLen || len >= halfTypeLen * lenCoeff) { // len = max ele per repeat / len >= 128
            SetMask<T>(FULL_MASK, FULL_MASK);
            return;
        }
        SetMask<T>(
            static_cast<uint64_t>(
                (len > halfTypeLen) ? (((static_cast<uint64_t>(1)) << static_cast<uint32_t>(len - halfTypeLen)) - 1) :
                                      0),
            static_cast<uint64_t>(
                (len > halfTypeLen) ? FULL_MASK : (((static_cast<uint64_t>(1)) << static_cast<uint32_t>(len)) - 1)));
    }

    template <typename T>
    __aicore__ static inline void SetMaskCount()
    {
        set_mask_count();
    }

    template <typename T>
    __aicore__ static inline void SetMaskNorm()
    {
        set_mask_norm();
    }

#if defined(__NPU_ARCH__) &&                                                                                 \
    ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 3510) || \
     (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || (__NPU_ARCH__ == 5101) || \
     (__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
    __aicore__ static inline void SetOverflow(uint64_t ctrlValue)
    {
        // set CTRL[48] is 1 --- inf/nan mode
        // set CTRL[48] is 0 --- saturated mode
        if (ctrlValue == 1) {
            set_ctrl(sbitset1(get_ctrl(), CTRL_48_BIT));
        } else {
            set_ctrl(sbitset0(get_ctrl(), CTRL_48_BIT));
        }
    }

#elif __NPU_ARCH__ == 2002
    __aicore__ static inline void SetOverflow(uint64_t ctrlValue)
    {
        // set CTRL[53] is 1 --- saturated mode
        // set CTRL[53] is 0 --- inf/nan mode
        if (ctrlValue == 0) {
            set_ctrl(sbitset1(get_ctrl(), CTRL_53_BIT));
        } else {
            set_ctrl(sbitset0(get_ctrl(), CTRL_53_BIT));
        }
    }
#endif

    template <bool isSetMask = true>
    __aicore__ static inline void ResetMask()
    {
        if constexpr (!isSetMask) {
            return;
        }
        if ASCEND_IS_NOT_AIC {
            set_vector_mask(FULL_MASK, FULL_MASK);
        }
    }

    template <bool isInt4 = false>
    __aicore__ inline static IntriInfo CalIntriInfo(
        const uint32_t dtypeSize, const uint32_t count, uint32_t repStride = DEFAULT_BLK_NUM)
    {
        IntriInfo retIntriInfo;
        retIntriInfo.c0Count = GetC0Count(dtypeSize);
        if constexpr (isInt4) {
            retIntriInfo.c0Count = GetC0Size() * INT4_TWO;
        }
        uint32_t repeatCount = repStride * retIntriInfo.c0Count;
        retIntriInfo.repeat = count / repeatCount;
        retIntriInfo.tail = count % repeatCount;
        retIntriInfo.repeatRounding = retIntriInfo.repeat / MAX_REPEAT_TIMES;
        retIntriInfo.repeatRemaining = retIntriInfo.repeat % MAX_REPEAT_TIMES;

        return retIntriInfo;
    }

    template <typename T>
    __aicore__ static inline __ubuf__ T* GetTemporaryBufferAddr(const int32_t bufferOffset, const int32_t bufferSize)
    {
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
        ASCENDC_ASSERT((bufferOffset % ONE_BLK_SIZE == 0), {
            KERNEL_LOG(KERNEL_ERROR, "bufferOffset is %d, which must be 32B aligned", bufferOffset);
        });
        ASCENDC_ASSERT(
            (bufferOffset + bufferSize * sizeof(T) <= ConstDefiner::Instance().bufferInitLen.at(Hardware::UB)), {
                KERNEL_LOG(
                    KERNEL_ERROR, "bufferOffset is %d, bufferSize is %d, which exceed the limit of ub %d", bufferOffset,
                    bufferSize, ConstDefiner::Instance().bufferInitLen.at(Hardware::UB));
            });
        const int32_t maxTempSize = 0x100000;
        ASCENDC_ASSERT((bufferSize < maxTempSize), {
            KERNEL_LOG(
                KERNEL_ERROR, "bufferSize is %d, which exceed the maxTempSize limits %d", bufferSize, maxTempSize);
        });
        T* addr = reinterpret_cast<T*>(ConstDefiner::Instance().hardwareCpuBufferMap.at(Hardware::UB) + bufferOffset);
#else
        (void)bufferSize;
        __ubuf__ T* addr = reinterpret_cast<__ubuf__ T*>(get_imm(0) + bufferOffset);
#endif
        return addr;
    }

    template <typename T>
    __aicore__ static inline void FreeTemporaryBuffer(__ubuf__ T* addr)
    {
        (void)addr;
    }

#if defined(__NPU_ARCH__) &&                                                                                 \
    ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 5102) || \
     (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5101) || \
     (__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
    template <typename T>
    __aicore__ static inline __fbuf__ T* GetTemporaryFbBufferAddr(const int32_t bufferOffset, const int32_t bufferSize)
    {
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
        ASCENDC_ASSERT((bufferOffset % ONE_BLK_SIZE == 0), {
            KERNEL_LOG(KERNEL_ERROR, "bufferOffset is %d, which must be 32B aligned", bufferOffset);
        });
        ASCENDC_ASSERT(
            (bufferOffset + bufferSize * sizeof(T) <= ConstDefiner::Instance().bufferInitLen.at(Hardware::FIXBUF)), {
                KERNEL_LOG(
                    KERNEL_ERROR, "bufferOffset is %d, bufferSize is %d, which exceed the limit of fixbuf %d",
                    bufferOffset, bufferSize, ConstDefiner::Instance().bufferInitLen.at(Hardware::FIXBUF));
            });
        T* addr =
            reinterpret_cast<T*>(ConstDefiner::Instance().hardwareCpuBufferMap.at(Hardware::FIXBUF) + bufferOffset);
#else
        (void)bufferSize;
        __fbuf__ T* addr = reinterpret_cast<__fbuf__ T*>(get_imm(0) + bufferOffset);
#endif
        return addr;
    }

    template <typename T>
    __aicore__ static inline void FreeTemporaryFbBuffer(__fbuf__ T* addr)
    {
        (void)addr;
    }
#endif

    __aicore__ static inline uint64_t GetGMLen(
        const DataCopyParams& intriParams, const bool& isSrc, const bool& isMovAlignIntri)
    {
        uint16_t stride = intriParams.dstStride;
        uint16_t burstLenUnit = 32;
        uint16_t strideUnit = 32;
        if (isSrc) {
            stride = intriParams.srcStride;
        }
        if (isMovAlignIntri) {
            burstLenUnit = 1;
            strideUnit = 1;
        }
        if (intriParams.blockLen == 0) {
            return 0;
        }
        uint64_t gmLen = static_cast<uint64_t>(intriParams.blockCount) * intriParams.blockLen * burstLenUnit +
                         (intriParams.blockCount - 1) * stride * strideUnit;
        return gmLen;
    }

    __aicore__ static inline uint64_t GetGMLen(
        const DataCopyExtParams& intriParams, const bool& isSrc, const bool& isMovAlignIntri)
    {
        int64_t stride = intriParams.dstStride;
        uint16_t burstLenUnit = 32;
        uint16_t strideUnit = 32;
        if (isSrc) {
            stride = intriParams.srcStride;
        }
        if (isMovAlignIntri) {
            burstLenUnit = 1;
            strideUnit = 1;
        }
        if (intriParams.blockLen == 0) {
            return 0;
        }
        uint64_t gmLen = static_cast<uint64_t>(
            static_cast<int64_t>(intriParams.blockCount * intriParams.blockLen * burstLenUnit) +
            (intriParams.blockCount - 1) * stride * strideUnit);
        return gmLen;
    }

    __aicore__ static inline uint64_t GetGMLen(const uint64_t& srcEleSize, const Nd2NzParams& intriParams)
    {
        uint64_t gmLen = (static_cast<uint64_t>(intriParams.ndNum) - 1) * srcEleSize * intriParams.srcNdMatrixStride +
                         (intriParams.nValue - 1) * intriParams.srcDValue * srcEleSize +
                         intriParams.dValue * srcEleSize;
        return gmLen;
    }

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 5101 || \
                              __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    __aicore__ static inline uint64_t GetGMLen(const uint64_t& srcEleSize, const Dn2NzParams& intriParams)
    {
        uint64_t gmLen = (intriParams.dnNum - 1) * intriParams.srcDnMatrixStride * srcEleSize +
                         intriParams.nValue * srcEleSize +
                         (intriParams.dValue - 1) * intriParams.srcDValue * srcEleSize;
        return gmLen;
    }
#endif

    __aicore__ static inline bool OOMCheckAddrIsOverflow(uintptr_t gmAddrConvert, const uint64_t& gmLen)
    {
        (void)gmAddrConvert;
        (void)gmLen;
#if defined(ASCENDC_OOM) && ASCENDC_OOM == 1
        uintptr_t inputOutputAddr = 0;
        uint64_t inputOutputLen = 0;

        for (uint64_t index = 0; index < g_oomAddrArange.count; index++) {
            if (g_oomAddrArange.addr[index] == 0 || g_oomAddrArange.len[index] == 0) {
                continue;
            }
            if (g_oomAddrArange.isLevelOnePointer[index] == 0 &&
                OOMCheckAddrInTensorList(index, gmAddrConvert, inputOutputAddr, inputOutputLen)) {
                break;
            } else {
                inputOutputAddr = g_oomAddrArange.addr[index];
                inputOutputLen = g_oomAddrArange.len[index];
                if (gmAddrConvert >= inputOutputAddr && gmAddrConvert < inputOutputAddr + inputOutputLen) {
                    break;
                }
            }
            if (index == g_oomAddrArange.count - 1) {
                return true;
            }
        }
        if (gmAddrConvert + gmLen > inputOutputAddr + inputOutputLen) {
            return true;
        }
#endif
        (void)gmAddrConvert;
        (void)gmLen;
        return false;
    }

    template <typename T>
    __aicore__ static inline void CheckGmMemOverflow(__gm__ T* gmAddr, const bool& isSrc, const uint64_t& gmLen)
    {
#if defined(ASCENDC_OOM) && ASCENDC_OOM == 1
        if (gmLen == 0) {
            return;
        }
        if (g_oomAddrArange.count == 0) {
            return;
        }
        uintptr_t gmAddrConvert = reinterpret_cast<uintptr_t>(gmAddr);
        bool status = OOMCheckAddrIsOverflow(gmAddrConvert, gmLen);
#if defined(L2_CACHE_HINT) && (__NPU_ARCH__ == 2201)
        if ASCEND_IS_NOT_AIV {
            if (status) {
                uint64_t oriGmAddr = reinterpret_cast<uint64_t>(gmAddr);
#ifdef __NPU_DEVICE__
                const uint64_t l2Cacheoffset = g_opL2CacheHintCfg.l2Cacheoffset;
                if (oriGmAddr >= l2Cacheoffset) {
                    oriGmAddr -= l2Cacheoffset;
                }
#else  // ifndef __NPU_DEVICE__
                if (oriGmAddr >= g_opSystemRunCfg.l2Cacheoffset) {
                    oriGmAddr -= g_opSystemRunCfg.l2Cacheoffset;
                }
#endif // __NPU_DEVICE__
                gmAddrConvert = reinterpret_cast<uintptr_t>(oriGmAddr);
                status = OOMCheckAddrIsOverflow(gmAddrConvert, gmLen);
            }
        }
#endif // L2_CACHE_HINT
        constexpr uint64_t errCode = 0X5A5A0001;
        if (status) {
#if defined(__NPU_ARCH__) &&                                                                                 \
    ((__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || \
     (__NPU_ARCH__ == 3113) || (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5101) || (__NPU_ARCH__ == 5161) || \
     (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
            trap();
#else
            trap(errCode);
#endif
        }
#endif
    }

    template <typename T>
    __aicore__ static inline void CheckGmMemOverflowNormal(
        __gm__ T* gmAddr, __gm__ uint8_t* workSpace, const bool isSrc, const bool isMovAlignIntri,
        const DataCopyParams& intriParams)
    {
        (void)(workSpace);
        uint64_t gmLen = GetGMLen(intriParams, isSrc, isMovAlignIntri);
        CheckGmMemOverflow(gmAddr, isSrc, gmLen);
    }

    template <typename T>
    __aicore__ static inline void CheckGmMemOverflowNormal(
        __gm__ T* gmAddr, __gm__ uint8_t* workSpace, const bool isSrc, const bool isMovAlignIntri,
        const DataCopyExtParams& intriParams)
    {
        (void)(workSpace);
        uint64_t gmLen = GetGMLen(intriParams, isSrc, isMovAlignIntri);
        CheckGmMemOverflow(gmAddr, isSrc, gmLen);
    }

    template <typename T>
    __aicore__ static inline void CheckGmMemOverflowNd2Nz(
        __gm__ T* gmAddr, __gm__ uint8_t* workSpace, const bool isSrc, const Nd2NzParams& intriParams)
    {
        (void)(workSpace);
        uint64_t srcEleSize = sizeof(T);
        uint64_t gmLen = GetGMLen(srcEleSize, intriParams);
        CheckGmMemOverflow(gmAddr, isSrc, gmLen);
    }

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 5101 || \
                              __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    template <typename T>
    __aicore__ static inline void CheckGmMemOverflowDn2Nz(
        __gm__ T* gmAddr, __gm__ uint8_t* workSpace, const bool& isSrc, const Dn2NzParams& intriParams)
    {
        (void)(workSpace);
        uint64_t srcEleSize = sizeof(T);
        uint64_t gmLen = GetGMLen(srcEleSize, intriParams);
        CheckGmMemOverflow(gmAddr, isSrc, gmLen);
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ != 3003 && __NPU_ARCH__ != 3113)
    template <typename T, uint8_t dim>
    __aicore__ static inline void CheckGmMemOverflowNddma(__gm__ T* gmAddr, const MultiCopyLoopInfo<dim>& params)
    {
        uint64_t maxOffset = 1;
        for (int32_t i = dim - 1; i >= 0; i--) {
            if (params.loopSize[i] == 0) {
                maxOffset = 0;
                break;
            }
            maxOffset += params.loopSrcStride[i] * (params.loopSize[i] - 1);
        }
        CheckGmMemOverflow(gmAddr, true, maxOffset * sizeof(T));
    }
#endif
#endif
};
} // namespace AscendC

#endif // ASCENDC_MODULE_UTILS_BASE_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_UTILS_BASE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_UTILS_BASE_H__
#endif
