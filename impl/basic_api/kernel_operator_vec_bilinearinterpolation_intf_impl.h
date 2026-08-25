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
 * \file kernel_operator_vec_bilinearinterpolation_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_vec_bilinearinterpolation_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use kernel_operator.h and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BILINEARINTERPOLATION_INTF_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_BILINEARINTERPOLATION_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_BILINEARINTERPOLATION_INTERFACE_IMPL_H
#include "../../include/basic_api/kernel_tensor.h"
#include "kernel_check.h"
#include "mstx_local_tensor_info.h"

#if __NPU_ARCH__ == 1001
#include "dav_c100/kernel_operator_vec_bilinearinterpolation_impl.h"
#elif __NPU_ARCH__ == 2002
#include "dav_m200/kernel_operator_vec_bilinearinterpolation_impl.h"
#elif __NPU_ARCH__ == 2201
#include "dav_c220/kernel_operator_vec_bilinearinterpolation_impl.h"
#elif __NPU_ARCH__ == 3002
#include "dav_m300/kernel_operator_vec_bilinearinterpolation_impl.h"
#elif __NPU_ARCH__ == 3102
#include "dav_m310/kernel_operator_vec_bilinearinterpolation_impl.h"
#elif __NPU_ARCH__ == 3510
#include "dav_3510/kernel_operator_vec_bilinearinterpolation_impl.h"
#elif (__NPU_ARCH__ == 5102)
#include "dav_m510/kernel_operator_vec_bilinearinterpolation_impl.h"
#elif __NPU_ARCH__ == 3003
#include "dav_l300/kernel_operator_vec_bilinearinterpolation_impl.h"
#elif __NPU_ARCH__ == 3113
#include "dav_l311/kernel_operator_vec_bilinearinterpolation_impl.h"
#endif

#pragma begin_pipe(V)
namespace AscendC {
template <typename T>
__aicore__ inline void BilinearInterpolationCheck(
    const LocalTensor<T>& src0, const LocalTensor<uint32_t>& src0Offset, const LocalTensor<T>& src1, uint16_t vROffset,
    const LocalTensor<uint8_t>& sharedTmpBuffer)
{
#if ASCENDC_CPU_DEBUG
    uint32_t sharedTmpBufferSize = sharedTmpBuffer.GetSize();
#if __NPU_ARCH__ == 2201
    uint32_t expectedTmpBufferSize = (src0.GetSize() + src1.GetSize()) * 32;
#elif (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113)
    uint32_t expectedTmpBufferSize = 0;
#else
    uint32_t expectedTmpBufferSize = src0Offset.GetSize() * sizeof(uint32_t);
#endif
    ASCENDC_ASSERT((sharedTmpBufferSize >= expectedTmpBufferSize), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "Failed to check "
            "sharedTmpBuffer size in BilinearInterpolation, its expected size is at least %u, current size is %u",
            expectedTmpBufferSize, sharedTmpBufferSize);
    });
#endif
    ASCENDC_ASSERT((SupportType<PrimT<T>, half>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in BilinearInterpolation,"
                          " current api support dtype combination is src: half, dst: half");
    });
    ASCENDC_CHECK_VALUE_RANGE(vROffset, 128, UINT16_MAX, "vROffset", "BilinearInterpolation");
    ASCENDC_ASSERT((vROffset % AscendCUtils::GetC0Count(sizeof(PrimT<T>)) == 0), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "Failed to check vROffset value in BilinearInterpolation, vROffset * sizeof(T) must be 32B aligned, "
            "current vROffset value is %u. In NPU mode, no error is reported. The value is rounded down by 32B.",
            vROffset);
    });
}

template <typename T>
__aicore__ inline void BilinearInterpolation(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<uint32_t>& src0Offset,
    const LocalTensor<T>& src1, uint64_t mask, uint8_t hRepeat, bool repeatMode, uint16_t dstBlkStride,
    uint16_t vROffset, uint8_t vRepeat, const LocalTensor<uint8_t>& sharedTmpBuffer)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBilinearInterpolationInfo(
        dst, src0, src0Offset, src1, mask, hRepeat, repeatMode, dstBlkStride, vROffset, vRepeat, sharedTmpBuffer,
        "BilinearInterpolation");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncBilinearInterpolation(
            dst, src0, src0Offset, src1, mask, hRepeat, repeatMode, dstBlkStride, vROffset, vRepeat,
            "BilinearInterpolation")) {
        ASCENDC_REPORT_CHECK_ERROR("BilinearInterpolation", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
    BilinearInterpolationCheck(src0, src0Offset, src1, vROffset, sharedTmpBuffer);
    BilinearInterpolationCalc(
        dst, src0, src0Offset, src1, mask, hRepeat, repeatMode, dstBlkStride, vROffset, vRepeat, sharedTmpBuffer);
}

template <typename T>
__aicore__ inline void BilinearInterpolation(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<uint32_t>& src0Offset,
    const LocalTensor<T>& src1, uint64_t mask[], uint8_t hRepeat, bool repeatMode, uint16_t dstBlkStride,
    uint16_t vROffset, uint8_t vRepeat, const LocalTensor<uint8_t>& sharedTmpBuffer)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBilinearInterpolationInfo(
        dst, src0, src0Offset, src1, mask[0], mask[1], hRepeat, repeatMode, dstBlkStride, vROffset, vRepeat,
        sharedTmpBuffer, "BilinearInterpolation");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncBilinearInterpolation(
            dst, src0, src0Offset, src1, mask, hRepeat, repeatMode, dstBlkStride, vROffset, vRepeat,
            "BilinearInterpolation")) {
        ASCENDC_REPORT_CHECK_ERROR("BilinearInterpolation", KernelFuncType::MASK_BIT_MODE);
    }
#endif
    BilinearInterpolationCheck(src0, src0Offset, src1, vROffset, sharedTmpBuffer);
    BilinearInterpolationCalc(
        dst, src0, src0Offset, src1, mask, hRepeat, repeatMode, dstBlkStride, vROffset, vRepeat, sharedTmpBuffer);
}
} // namespace AscendC
#pragma end_pipe
#endif // ASCENDC_MODULE_OPERATOR_VEC_BILINEARINTERPOLATION_INTERFACE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BILINEARINTERPOLATION_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BILINEARINTERPOLATION_INTF_IMPL_H__
#endif
