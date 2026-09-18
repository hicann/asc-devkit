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
 * \file kernel_operator_vec_createvecindex_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_vec_createvecindex_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_vec_createvecindex_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_CREATEVECINDEX_INTF_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_CREATEVECINDEX_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_CREATEVECINDEX_INTERFACE_IMPL_H
#include "../../include/basic_api/kernel_tensor.h"
#include "kernel_check.h"
#include "mstx_local_tensor_info.h"

#if __NPU_ARCH__ == 1001
#include "dav_c100/kernel_operator_vec_createvecindex_impl.h"
#elif __NPU_ARCH__ == 2002
#include "dav_m200/kernel_operator_vec_createvecindex_impl.h"
#elif __NPU_ARCH__ == 2201
#include "dav_c220/kernel_operator_vec_createvecindex_impl.h"
#elif __NPU_ARCH__ == 3002
#include "dav_m300/kernel_operator_vec_createvecindex_impl.h"
#elif __NPU_ARCH__ == 3102
#include "dav_m310/kernel_operator_vec_createvecindex_impl.h"
#elif __NPU_ARCH__ == 3510
#include "dav_3510/kernel_operator_vec_createvecindex_impl.h"
#elif (__NPU_ARCH__ == 5102)
#include "dav_m510/kernel_operator_vec_createvecindex_impl.h"
#elif (__NPU_ARCH__ == 5162)
#include "dav_v516/kernel_operator_vec_createvecindex_impl.h"
#elif __NPU_ARCH__ == 3003
#include "dav_l300/kernel_operator_vec_createvecindex_impl.h"
#elif __NPU_ARCH__ == 3113
#include "dav_l311/kernel_operator_vec_createvecindex_impl.h"
#include "dav_l311/kernel_operator_vec_cmpsel_impl.h"
#endif

namespace AscendC {
template <typename T>
__aicore__ inline __in_pipe__(S) __out_pipe__(V) void CreateVecIndex(
    LocalTensor<T>& dst, const T& firstValue, uint64_t mask, uint8_t repeatTime, uint16_t dstBlkStride,
    uint8_t dstRepStride)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecIndexInfo(dst, mask, repeatTime, dstBlkStride, dstRepStride, "CreateVecIndex");
#endif
    ASCENDC_ASSERT((SupportType<T, half, int16_t, float, int32_t>()), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "Failed to check "
            "dtype in CreateVecIndex, current api support dtype combination is dst: half / int16_t / float / int32_t");
    });
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncCreateVecIndex(dst, mask, repeatTime, dstBlkStride, dstRepStride, "CreateVecIndex")) {
        ASCENDC_REPORT_CHECK_ERROR("CreateVecIndex", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
    CreateVecIndexCalc(dst, firstValue, mask, repeatTime, dstBlkStride, dstRepStride);
}

template <typename T>
__aicore__ inline __in_pipe__(S) __out_pipe__(V) void CreateVecIndex(
    LocalTensor<T>& dst, const T& firstValue, uint64_t mask[], uint8_t repeatTime, uint16_t dstBlkStride,
    uint8_t dstRepStride)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecIndexInfo(dst, mask[0], mask[1], repeatTime, dstBlkStride, dstRepStride, "CreateVecIndex");
#endif
    ASCENDC_ASSERT((SupportType<T, half, int16_t, float, int32_t>()), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "Failed to check "
            "dtype in CreateVecIndex, current api support dtype combination is dst: half / int16_t / float / int32_t");
    });
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncCreateVecIndex(dst, mask, repeatTime, dstBlkStride, dstRepStride, "CreateVecIndex")) {
        ASCENDC_REPORT_CHECK_ERROR("CreateVecIndex", KernelFuncType::MASK_BIT_MODE);
    }
#endif
    CreateVecIndexCalc(dst, firstValue, mask, repeatTime, dstBlkStride, dstRepStride);
}

template <typename T>
__aicore__ inline __in_pipe__(S)
    __out_pipe__(V) void CreateVecIndex(LocalTensor<T> dst, const T& firstValue, uint32_t count)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecIndexInfo(dst, count, "CreateVecIndex");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncCreateVecIndex(dst, count, "CreateVecIndex")) {
        ASCENDC_REPORT_CHECK_ERROR("CreateVecIndex", KernelFuncType::CALCOUNT_MODE);
    }
#endif
    CreateVecIndexCalc(dst, firstValue, count);
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_CREATEVECINDEX_INTERFACE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_CREATEVECINDEX_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_CREATEVECINDEX_INTF_IMPL_H__
#endif
