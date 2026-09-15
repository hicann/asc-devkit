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
 * \file kernel_operator_vec_scatter_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_vec_scatter_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_vec_scatter_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_SCATTER_INTF_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_SCATTER_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_SCATTER_INTERFACE_IMPL_H
#include "../../include/basic_api/kernel_tensor.h"
#include "kernel_check.h"

#if __NPU_ARCH__ == 1001
#include "dav_c100/kernel_operator_vec_scatter_impl.h"
#elif __NPU_ARCH__ == 2002
#include "dav_m200/kernel_operator_vec_scatter_impl.h"
#elif __NPU_ARCH__ == 2201
#include "dav_c220/kernel_operator_vec_scatter_impl.h"
#elif __NPU_ARCH__ == 3002
#include "dav_m300/kernel_operator_vec_scatter_impl.h"
#elif __NPU_ARCH__ == 3102
#include "dav_m310/kernel_operator_vec_scatter_impl.h"
#elif __NPU_ARCH__ == 3510
#include "dav_3510/kernel_operator_vec_scatter_impl.h"
#endif

#pragma begin_pipe(V)
namespace AscendC {
template <typename T>
__aicore__ inline void ScatterCheck()
{
    using PrimType = PrimT<T>;
#if (__NPU_ARCH__ == 3510)
    ASCENDC_ASSERT(
        (SupportType<
            PrimType, uint8_t, int8_t, half, bfloat16_t, uint16_t, int16_t, float, uint32_t, int32_t, uint64_t,
            int64_t>()),
        {
            KERNEL_LOG(
                KERNEL_ERROR,
                "Failed to check dtype in Scatter, current api support dtype combination is src and dst both: "
                "uint8 / int8 / half / bfloat16_t / uint16_t / int16_t / float / uint32_t / int32_t, uint64_t, "
                "int64_t");
        });
#elif ((__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102))
    ASCENDC_ASSERT(
        (SupportType<PrimType, uint8_t, int8_t, half, bfloat16_t, uint16_t, int16_t, float, uint32_t, int32_t>()), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "Failed to check dtype in Scatter, current api support dtype combination is src and "
                "dst both: uint8 / int8 / half / bfloat16_t / uint16_t / int16_t / float / uint32_t / int32_t");
        });
#else
    ASCENDC_ASSERT((SupportType<PrimType, half, uint16_t, int16_t, float, uint32_t, int32_t>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in Scatter, current api support dtype combination is src and "
                          "dst both: half / uint16_t / int16_t / float / uint32_t / int32_t");
    });
#endif
}

/*
 * @ingroup scatter Level 0
 * @brief scatter element from dst according to dstOffset
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] dstOffset input LocalTensor
 * @param [in] dstBaseAddr base address of dst
 * @param [in] mask valid element count
 * @param [in] repeatTime repeat times
 * @param [in] dstRepStride dst repeat stride
 */
template <typename T>
__aicore__ inline void Scatter(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint32_t>& dstOffset,
    const uint32_t dstBaseAddr, const uint64_t mask, const uint8_t repeatTime, const uint8_t srcRepStride)
{
#if ASCENDC_CPU_DEBUG
    if (!CheckFunScatter(dst, src, dstOffset, dstBaseAddr, mask, repeatTime, srcRepStride, "Scatter")) {
        ASCENDC_REPORT_CHECK_ERROR("Scatter", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
    using PrimType = PrimT<T>;
    ScatterCheck<T>();
    const uint32_t dstLength = dst.GetSize();
    ScatterImpl(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(),
        (__ubuf__ uint32_t*)dstOffset.GetPhyAddr(), dstLength, dstBaseAddr, mask, repeatTime, srcRepStride);
}

/*
 * @ingroup scatter Level 0
 * @brief scatter element from dst according to dstOffset
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] dstOffset input LocalTensor
 * @param [in] dstBaseAddr base address of dst
 * @param [in] mask valid element count(bit mode)
 * @param [in] repeatTime repeat times
 * @param [in] dstRepStride dst repeat stride
 */
template <typename T>
__aicore__ inline void Scatter(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint32_t>& dstOffset,
    const uint32_t dstBaseAddr, const uint64_t mask[], const uint8_t repeatTime, const uint8_t srcRepStride)
{
#if ASCENDC_CPU_DEBUG
    if (!CheckFunScatter(dst, src, dstOffset, dstBaseAddr, mask, repeatTime, srcRepStride, "Scatter")) {
        ASCENDC_REPORT_CHECK_ERROR("Scatter", KernelFuncType::MASK_BIT_MODE);
    }
#endif
    using PrimType = PrimT<T>;
    ScatterCheck<T>();
    const uint32_t dstLength = dst.GetSize();
    ScatterImpl(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(),
        (__ubuf__ uint32_t*)dstOffset.GetPhyAddr(), dstLength, dstBaseAddr, mask, repeatTime, srcRepStride);
}

/*
 * @ingroup scatter Level 2
 * @brief scatter element from dst according to dstOffset
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] dstOffset input LocalTensor
 * @param [in] dstBaseAddr base address of dst
 * @param [in] count element count
 */
template <typename T>
__aicore__ inline void Scatter(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint32_t>& dstOffset,
    const uint32_t dstBaseAddr, const uint32_t count)
{
#if ASCENDC_CPU_DEBUG
    if (!CheckFunScatter(dst, src, dstOffset, dstBaseAddr, count, "Scatter")) {
        ASCENDC_REPORT_CHECK_ERROR("Scatter", KernelFuncType::NONE_MODE);
    }
#endif
    using PrimType = PrimT<T>;
    ScatterCheck<T>();
    uint32_t vectorRegWidth = 256;
#if (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 3510)
    ScatterImpl(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(),
        (__ubuf__ uint32_t*)dstOffset.GetPhyAddr(), dstBaseAddr, count);
#else
    uint32_t elementCountSingleRepeat;
    if constexpr (sizeof(PrimType) == sizeof(uint16_t)) {
        elementCountSingleRepeat = 128;
    } else {
        elementCountSingleRepeat = 64;
    }
    const uint32_t elementCountTail = count % elementCountSingleRepeat;
    const uint8_t repeatTime = count / elementCountSingleRepeat;
    if (repeatTime > 0) {
        Scatter(
            dst, src, dstOffset, dstBaseAddr, static_cast<uint64_t>(elementCountSingleRepeat), repeatTime,
            DEFAULT_REPEAT_STRIDE);
    }
    if (elementCountTail > 0) {
        const uint32_t offset = count - elementCountTail;
        Scatter(
            dst, src[offset], dstOffset[offset], dstBaseAddr, static_cast<uint64_t>(elementCountTail), 1,
            DEFAULT_REPEAT_STRIDE);
    }
#endif
}
} // namespace AscendC
#pragma end_pipe
#endif // ASCENDC_MODULE_OPERATOR_VEC_SCATTER_INTERFACE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_SCATTER_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_SCATTER_INTF_IMPL_H__
#endif
