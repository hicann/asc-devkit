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
 * \file kernel_operator_vec_ternary_scalar_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_vec_ternary_scalar_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_vec_ternary_scalar_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_TERNARY_SCALAR_INTF_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_TERNARY_SCALAR_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_TERNARY_SCALAR_INTERFACE_IMPL_H
#include "../../include/basic_api/kernel_tensor.h"
#include "../../include/basic_api/kernel_struct_unary.h"
#include "kernel_npu_debug.h"
#include "mstx_local_tensor_info.h"

#if __NPU_ARCH__ == 1001
#include "dav_c100/kernel_operator_vec_ternary_scalar_impl.h"
#elif __NPU_ARCH__ == 2002
#include "dav_m200/kernel_operator_vec_ternary_scalar_impl.h"
#elif __NPU_ARCH__ == 2201
#include "dav_c220/kernel_operator_vec_ternary_scalar_impl.h"
#elif __NPU_ARCH__ == 3002
#include "dav_m300/kernel_operator_vec_ternary_scalar_impl.h"
#elif __NPU_ARCH__ == 3102
#include "dav_m310/kernel_operator_vec_ternary_scalar_impl.h"
#elif __NPU_ARCH__ == 3510
#include "dav_3510/kernel_operator_vec_ternary_scalar_impl.h"
#endif
#include "kernel_check.h"

#pragma begin_pipe(V)
namespace AscendC {
/*
 * @ingroup Axpy Level 0
 * @brief dst[i] = src[i]*scalar + dst[i]
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalarValue input scalar number
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.srcBlkStride src block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src repeat stride
 */
template <typename T, typename U, bool isSetMask>
__aicore__ inline void Axpy(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const U& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    using MaskCheckType = typename Conditional<(sizeof(PrimT<T>) >= sizeof(PrimT<U>)), PrimT<T>, PrimT<U>>::type;
    CheckVectorTensor("Axpy", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<MaskCheckType, isSetMask>(mask, repeatTime, "Axpy");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalarDiffType(dst, src, scalarValue, mask, repeatTime, repeatParams, "Axpy")) {
        ASCENDC_REPORT_CHECK_ERROR("Axpy", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryTenaryInfo<T, U, isSetMask>(dst, src, mask, repeatTime, repeatParams, "Axpy");
#endif
    AxpyImpl<T, U, isSetMask>(
        (__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ U*)src.GetPhyAddr(), scalarValue, mask, repeatTime, repeatParams);
}

template <typename T, typename U, bool isSetMask>
__aicore__ inline void Axpy(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const U& scalarValue, uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    using MaskCheckType = typename Conditional<(sizeof(PrimT<T>) >= sizeof(PrimT<U>)), PrimT<T>, PrimT<U>>::type;
    CheckVectorTensor("Axpy", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<MaskCheckType, isSetMask>(mask, repeatTime, "Axpy");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalarDiffType(dst, src, scalarValue, mask, repeatTime, repeatParams, "Axpy")) {
        ASCENDC_REPORT_CHECK_ERROR("Axpy", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryTenaryInfo<T, U, isSetMask>(
        dst, src, mask[0], mask[1], repeatTime, repeatParams, "Axpy");
#endif
    AxpyImpl<T, U, isSetMask>(
        (__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ U*)src.GetPhyAddr(), scalarValue, mask, repeatTime, repeatParams);
}

/*
 * @ingroup Axpy Level 2
 * @brief dst[i] = src[i]*scalar + dst[i]
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalarValue input scalar number
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, typename U>
__aicore__ inline void Axpy(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const U& scalarValue, const int32_t& count)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Axpy", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckCalcount(count, "count", "Axpy");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecBinaryScalarDiffType(dst, src, scalarValue, count, "Axpy")) {
        ASCENDC_REPORT_CHECK_ERROR("Axpy", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryTenaryInfo<T, U, true>(dst, src, "Axpy", count);
#endif
    AxpyImpl<T, U>((__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ U*)src.GetPhyAddr(), scalarValue, count);
}
} // namespace AscendC
#pragma end_pipe
#endif // ASCENDC_MODULE_OPERATOR_VEC_TERNARY_SCALAR_INTERFACE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_TERNARY_SCALAR_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_TERNARY_SCALAR_INTF_IMPL_H__
#endif
