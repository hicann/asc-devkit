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
 * \file kernel_operator_vec_mulcast_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_vec_mulcast_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_vec_mulcast_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_MULCAST_INTF_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_MULCAST_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_MULCAST_INTERFACE_IMPL_H
#include "../../include/basic_api/kernel_tensor.h"
#include "kernel_check.h"
#include "../../include/basic_api/kernel_struct_binary.h"
#include "kernel_npu_debug.h"
#include "mstx_local_tensor_info.h"

#if __NPU_ARCH__ == 1001
#include "dav_c100/kernel_operator_vec_mulcast_impl.h"
#elif __NPU_ARCH__ == 2002
#include "dav_m200/kernel_operator_vec_mulcast_impl.h"
#elif __NPU_ARCH__ == 2201
#include "dav_c220/kernel_operator_vec_mulcast_impl.h"
#elif __NPU_ARCH__ == 3002
#include "dav_m300/kernel_operator_vec_mulcast_impl.h"
#elif __NPU_ARCH__ == 3102
#include "dav_m310/kernel_operator_vec_mulcast_impl.h"
#elif __NPU_ARCH__ == 3510
#include "dav_3510/kernel_operator_vec_mulcast_impl.h"
#elif (__NPU_ARCH__ == 5102)
#include "dav_m510/kernel_operator_vec_mulcast_impl.h"
#elif (__NPU_ARCH__ == 5162)
#include "dav_v516/kernel_operator_vec_mulcast_impl.h"
#elif (__NPU_ARCH__ == 3003)
#include "dav_l300/kernel_operator_vec_mulcast_impl.h"
#elif (__NPU_ARCH__ == 3113)
#include "dav_l311/kernel_operator_vec_mulcast_impl.h"
#endif

#pragma begin_pipe(V)
namespace AscendC {
template <typename T, typename U, bool isSetMask>
__aicore__ inline void MulCast(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using DstPrimType = PrimT<T>;
    using SrcPrimType = PrimT<U>;
    using MaskCheckType =
        typename Conditional<(sizeof(DstPrimType) >= sizeof(SrcPrimType)), DstPrimType, SrcPrimType>::type;
    ASCENDC_DEBUG_ASSERT(
        (SupportType<SrcPrimType, half>() && SupportType<DstPrimType, int8_t, uint8_t>()),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR,
            "Failed "
            "to check dtype in MulCast, current api support dtype combination is src: half, dst: int8_t / uint8_t"));
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("MulCast", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<MaskCheckType, isSetMask>(mask, repeatTime, "MulCast");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinaryDiffType(dst, src0, src1, mask, repeatTime, repeatParams, "MulCast")) {
        ASCENDC_REPORT_CHECK_ERROR("MulCast", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, mask, repeatTime, repeatParams, isSetMask, "MulCast");
#endif
    MulCastCalc<DstPrimType, SrcPrimType, isSetMask>(dst, src0, src1, mask, repeatTime, repeatParams);
}

template <typename T, typename U, bool isSetMask>
__aicore__ inline void MulCast(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using DstPrimType = PrimT<T>;
    using SrcPrimType = PrimT<U>;
    using MaskCheckType =
        typename Conditional<(sizeof(DstPrimType) >= sizeof(SrcPrimType)), DstPrimType, SrcPrimType>::type;
    ASCENDC_DEBUG_ASSERT(
        (SupportType<SrcPrimType, half>() && SupportType<DstPrimType, int8_t, uint8_t>()),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR,
            "Failed "
            "to check dtype in MulCast, current api support dtype combination is src: half, dst: int8_t / uint8_t"));
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckMaskRepeat<MaskCheckType, isSetMask>(mask, repeatTime, "MulCast");
    CheckVectorTensor("MulCast", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinaryDiffType(dst, src0, src1, mask, repeatTime, repeatParams, "MulCast")) {
        ASCENDC_REPORT_CHECK_ERROR("MulCast", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, mask[0], mask[1], repeatTime, repeatParams, isSetMask, "MulCast");
#endif
    MulCastCalc<DstPrimType, SrcPrimType, isSetMask>(dst, src0, src1, mask, repeatTime, repeatParams);
}

template <typename T, typename U>
__aicore__ inline void MulCast(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, uint32_t count)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("MulCast", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckCalcount(static_cast<int32_t>(count), "count", "MulCast");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinaryDiffType(dst, src0, src1, count, "MulCast")) {
        ASCENDC_REPORT_CHECK_ERROR("MulCast", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, "MulCast", count);
#endif
    MulCastCalc(dst, src0, src1, count);
}
} // namespace AscendC
#pragma end_pipe
#endif // ASCENDC_MODULE_OPERATOR_VEC_MULCAST_INTERFACE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_MULCAST_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_MULCAST_INTF_IMPL_H__
#endif
