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
 * \file kernel_operator_vec_gather_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_vec_gather_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_vec_gather_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_GATHER_INTF_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_GATHER_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_GATHER_INTERFACE_IMPL_H
#include "../../include/basic_api/kernel_tensor.h"
#include "kernel_check.h"
#include "../../include/basic_api/kernel_struct_gather.h"
#include "kernel_npu_debug.h"

#if __NPU_ARCH__ == 1001
#include "dav_c100/kernel_operator_vec_gather_impl.h"
#elif __NPU_ARCH__ == 2002
#include "dav_m200/kernel_operator_vec_gather_impl.h"
#elif __NPU_ARCH__ == 2201
#include "dav_c220/kernel_operator_vec_gather_impl.h"
#elif __NPU_ARCH__ == 3002
#include "dav_m300/kernel_operator_vec_gather_impl.h"
#elif __NPU_ARCH__ == 3510
#include "dav_3510/kernel_operator_vec_gather_impl.h"
#endif

#pragma begin_pipe(V)
namespace AscendC {
/*
 * @ingroup gatherb Level 0
 * @brief this function fetches N addresses from offset,then accesses these N addresses(plus the src address)
 * @brief to get N 32Byte block, and finally writes these N blocks into dst.
 * @brief gather element in the uint of block
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] offset input LocalTensor
 * @param [in] repeatTime repeat times
 * @param [in] repeatParams.dstBlkStride dst block stride
 * @param [in] repeatParams.dstRepStride dst repeat stride
 */
template <typename T>
__aicore__ inline void Gatherb(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint32_t>& offset, const uint8_t repeatTime,
    const GatherRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncGatherb(dst, src, offset, repeatTime, repeatParams, "Gatherb")) {
        ASCENDC_REPORT_CHECK_ERROR("Gatherb", KernelFuncType::NONE_MODE);
    }
#endif
    uint32_t srcLength = src.GetSize();
    GatherbImpl(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(),
        (__ubuf__ uint32_t*)offset.GetPhyAddr(), srcLength, repeatTime, repeatParams);
}

/*
 * @ingroup gather Level 0
 * @brief gather element from src according to srcOffset
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] srcOffset input LocalTensor
 * @param [in] srcBaseOffset base address of src
 * @param [in] mask valid element count
 * @param [in] repeatTime repeat times
 * @param [in] dstRepStride dst repeat stride
 */
template <typename T>
__aicore__ inline void Gather(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint32_t>& srcOffset,
    const uint32_t srcBaseOffset, const uint64_t mask, const uint8_t repeatTime, const uint16_t dstRepStride)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Gather", NamedTensor(dst, "dst"), NamedTensor(src, "src"), NamedTensor(srcOffset, "srcOffset"));
    CheckMaskRepeat<PrimType>(mask, repeatTime, "Gather");
    // srcBaseOffset should not exceed the size of src tensor
    CheckValueRange<uint32_t>(srcBaseOffset, 0, src.GetSize() * sizeof(PrimType) - 1, "srcBaseOffset", "Gather");
    // srcBaseOffset should be aligned with src dtype
    ASCENDC_DEBUG_ASSERT(
        (srcBaseOffset % sizeof(PrimType) == 0), KERNEL_LOG_INTERNAL(
                                                     KERNEL_ERROR,
                                                     "Failed to check "
                                                     "srcBaseOffset value in Gather, it must be divisible by "
                                                     "sizeof(T), current srcBaseOffset is %u, sizeof(T) is %u.\n",
                                                     srcBaseOffset, sizeof(PrimType)));
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncGather(dst, src, srcOffset, srcBaseOffset, mask, repeatTime, dstRepStride, "Gather")) {
        ASCENDC_REPORT_CHECK_ERROR("Gather", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
    const uint32_t srcLength = src.GetSize();
    GatherImpl(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(),
        (__ubuf__ uint32_t*)srcOffset.GetPhyAddr(), srcLength, srcBaseOffset, mask, repeatTime, dstRepStride);
}

/*
 * @ingroup gather Level 0
 * @brief gather element from src according to srcOffset
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] srcOffset input LocalTensor
 * @param [in] srcBaseOffset base address of src
 * @param [in] mask valid element count(bit mode)
 * @param [in] repeatTime repeat times
 * @param [in] dstRepStride dst repeat stride
 */
template <typename T>
__aicore__ inline void Gather(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint32_t>& srcOffset,
    const uint32_t srcBaseOffset, const uint64_t mask[], const uint8_t repeatTime, const uint16_t dstRepStride)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Gather", NamedTensor(dst, "dst"), NamedTensor(src, "src"), NamedTensor(srcOffset, "srcOffset"));
    CheckMaskRepeat<PrimType>(mask, repeatTime, "Gather");
    // srcBaseOffset should not exceed the size of src tensor
    CheckValueRange<uint32_t>(srcBaseOffset, 0, src.GetSize() * sizeof(PrimType) - 1, "srcBaseOffset", "Gather");
    // srcBaseOffset should be aligned with src dtype
    ASCENDC_DEBUG_ASSERT(
        (srcBaseOffset % sizeof(PrimType) == 0), KERNEL_LOG_INTERNAL(
                                                     KERNEL_ERROR,
                                                     "Failed to check "
                                                     "srcBaseOffset value in Gather, it must be divisible by "
                                                     "sizeof(T), current srcBaseOffset is %u, sizeof(T) is %u.\n",
                                                     srcBaseOffset, sizeof(PrimType)));
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncGather(dst, src, srcOffset, srcBaseOffset, mask, repeatTime, dstRepStride, "Gather")) {
        ASCENDC_REPORT_CHECK_ERROR("Gather", KernelFuncType::MASK_BIT_MODE);
    }
#endif
    const uint32_t srcLength = src.GetSize();
    GatherImpl(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(),
        (__ubuf__ uint32_t*)srcOffset.GetPhyAddr(), srcLength, srcBaseOffset, mask, repeatTime, dstRepStride);
}

/*
 * @ingroup gather Level 2
 * @brief gather element from src according to srcOffset
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] srcOffset input LocalTensor
 * @param [in] srcBaseOffset base address of src
 * @param [in] count element count
 */
template <typename T>
__aicore__ inline void Gather(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint32_t>& srcOffset,
    const uint32_t srcBaseOffset, const uint32_t count)
{
    using PrimType = PrimT<T>;
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 3510))
    ASCENDC_ASSERT(
        (SupportType<PrimType, uint8_t, int8_t, half, bfloat16_t, uint16_t, int16_t, float, uint32_t, int32_t>()), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "Failed to check dtype in Gather, current api support dtype combination is src and "
                "dst both: uint8 / int8 / half / bfloat16_t / uint16_t / int16_t / float / uint32_t / int32_t");
        });
    GatherImpl(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(),
        (__ubuf__ uint32_t*)srcOffset.GetPhyAddr(), srcBaseOffset, count);
#else
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncGather(dst, src, srcOffset, srcBaseOffset, count, "Gather")) {
        ASCENDC_REPORT_CHECK_ERROR("Gather", KernelFuncType::CALCOUNT_MODE);
    }
#endif
    uint32_t vectorRegWidth = 256;
    uint32_t elementCountSingleRepeat;
    if constexpr (sizeof(PrimType) == sizeof(uint16_t)) {
        elementCountSingleRepeat = 128;
    } else {
        elementCountSingleRepeat = 64;
    }
    const uint32_t elementCountTail = count % elementCountSingleRepeat;
    const uint8_t repeatTime = count / elementCountSingleRepeat;
    if (repeatTime > 0) {
        Gather(
            dst, src, srcOffset, srcBaseOffset, static_cast<uint64_t>(elementCountSingleRepeat), repeatTime,
            DEFAULT_REPEAT_STRIDE);
    }
    if (elementCountTail > 0) {
        const uint32_t offset = count - elementCountTail;
        Gather(
            dst[offset], src, srcOffset[offset], srcBaseOffset, static_cast<uint64_t>(elementCountTail), 1,
            DEFAULT_REPEAT_STRIDE);
    }
#endif
}
} // namespace AscendC
#pragma end_pipe
#endif // ASCENDC_MODULE_OPERATOR_VEC_GATHER_INTERFACE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_GATHER_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_GATHER_INTF_IMPL_H__
#endif
