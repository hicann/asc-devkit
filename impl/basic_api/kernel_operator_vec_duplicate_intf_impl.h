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
 * \file kernel_operator_vec_duplicate_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_vec_duplicate_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_vec_duplicate_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_DUPLICATE_INTF_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_DUPLICATE_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_DUPLICATE_INTERFACE_IMPL_H
#include "../../include/basic_api/kernel_tensor.h"
#include "kernel_check.h"
#include "kernel_npu_debug.h"
#include "mstx_local_tensor_info.h"

#if __NPU_ARCH__ == 1001
#include "dav_c100/kernel_operator_vec_duplicate_impl.h"
#elif __NPU_ARCH__ == 2002
#include "dav_m200/kernel_operator_vec_duplicate_impl.h"
#elif __NPU_ARCH__ == 2201
#include "dav_c220/kernel_operator_vec_duplicate_impl.h"
#elif __NPU_ARCH__ == 3002
#include "dav_m300/kernel_operator_vec_duplicate_impl.h"
#elif __NPU_ARCH__ == 3102
#include "dav_m310/kernel_operator_vec_duplicate_impl.h"
#elif __NPU_ARCH__ == 3510
#include "../../include/basic_api/reg_compute/kernel_reg_compute_intf.h"
#include "dav_3510/kernel_operator_vec_duplicate_impl.h"
#endif

#pragma begin_pipe(V)
namespace AscendC {
/* **************************************************************************************************
 * Duplicate                                            *
 * ************************************************************************************************* */
/*
 * @ingroup Duplicate Level 0
 * @brief dst[i] = scalar
 * @param [out] dst output LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] dstBlockStride dst block stride
 * @param [in] dstRepeatStride dst repeat stride
 */
template <typename T, bool isSetMask>
__aicore__ inline void Duplicate(
    const LocalTensor<T>& dst, const T& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const uint16_t dstBlockStride, const uint8_t dstRepeatStride)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Duplicate", NamedTensor(dst, "dst"));
    CheckMaskRepeat<T, isSetMask>(mask, repeatTime, "Duplicate");
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecDupInfo(dst, mask, repeatTime, dstBlockStride, dstRepeatStride, isSetMask, "Duplicate");
#endif
    CheckDuplicateSupportedType<T>();
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunDup(dst, mask, repeatTime, dstBlockStride, dstRepeatStride, "Duplicate")) {
        ASCENDC_REPORT_CHECK_ERROR("Duplicate", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
    DuplicateImpl<T, isSetMask>(
        (__ubuf__ T*)dst.GetPhyAddr(), scalarValue, mask, repeatTime, dstBlockStride, dstRepeatStride);
}

template <typename T, bool isSetMask>
__aicore__ inline void Duplicate(
    const LocalTensor<T>& dst, const T& scalarValue, uint64_t mask[], const uint8_t repeatTime,
    const uint16_t dstBlockStride, const uint8_t dstRepeatStride)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Duplicate", NamedTensor(dst, "dst"));
    CheckMaskRepeat<T, isSetMask>(mask, repeatTime, "Duplicate");
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecDupInfo(
        dst, mask[0], mask[1], repeatTime, dstBlockStride, dstRepeatStride, isSetMask, "Duplicate");
#endif
    CheckDuplicateSupportedType<T>();
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunDup(dst, mask, repeatTime, dstBlockStride, dstRepeatStride, "Duplicate")) {
        ASCENDC_REPORT_CHECK_ERROR("Duplicate", KernelFuncType::MASK_BIT_MODE);
    }
#endif
    DuplicateImpl<T, isSetMask>(
        (__ubuf__ T*)dst.GetPhyAddr(), scalarValue, mask, repeatTime, dstBlockStride, dstRepeatStride);
}

/*
 * @ingroup Duplicate Level 2
 * @brief dst = dst[i] = scalar
 * @param [out] dst output LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] count number Number of data involved in calculation
 */
template <typename T>
__aicore__ inline void Duplicate(const LocalTensor<T>& dst, const T& scalarValue, const int32_t& count)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Duplicate", NamedTensor(dst, "dst"));
    CheckCalcount(count, "count", "Duplicate");
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecDupInfo(dst, count, "Duplicate");
#endif
    CheckDuplicateSupportedType<T>();
#if ASCENDC_CPU_DEBUG
    if (!CheckFunDup(dst, count, "Duplicate")) {
        ASCENDC_REPORT_CHECK_ERROR("Duplicate", KernelFuncType::CALCOUNT_MODE);
    }
#endif
    DuplicateImpl<T>((__ubuf__ T*)dst.GetPhyAddr(), scalarValue, count);
}

#if __NPU_ARCH__ == 3510
/*
 * @ingroup Duplicate lowest position of src
 * @brief dst = dst[i] = src[0]
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T>
__aicore__ inline void Duplicate(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecDupInfo(dst, count, "Duplicate");
#endif
    using PrimType = PrimT<T>;
    CheckDuplicateSupportedType<PrimType>();
#if ASCENDC_CPU_DEBUG
    if (!CheckFunDup(dst, count, "Duplicate")) {
        ASCENDC_REPORT_CHECK_ERROR("Duplicate", KernelFuncType::CALCOUNT_MODE);
    }
#endif
    DuplicateImpl<PrimType>((__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), count);
}

/* **************************************************************************************************
 * Interleave                                            *
 * ************************************************************************************************* */
/*
 * @ingroup Interleave Level 2
 * @brief Interleave src0 and src1 to dst0 and dst1
 * @param [out] dst0 output0 LocalTensor
 * @param [out] dst1 output1 LocalTensor
 * @param [in] src0 input0 LocalTensor
 * @param [in] src1 input1 LocalTensor
 * @param [count] count number of data calculation, must be even number
 */
template <typename T>
__aicore__ inline void Interleave(
    const LocalTensor<T>& dst0, const LocalTensor<T>& dst1, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
    const int32_t count)
{
    using PrimType = PrimT<T>;
    InterleaveImpl<PrimType>(
        (__ubuf__ PrimType*)dst0.GetPhyAddr(), (__ubuf__ PrimType*)dst1.GetPhyAddr(),
        (__ubuf__ PrimType*)src0.GetPhyAddr(), (__ubuf__ PrimType*)src1.GetPhyAddr(), count);
}

/* **************************************************************************************************
 * DeInterleave                                            *
 * ************************************************************************************************* */
/*
 * @ingroup DeInterleave Level 2
 * @brief DeInterleave src0 and src1 to dst0 and dst1
 * @param [out] dst0 output0 LocalTensor
 * @param [out] dst1 output1 LocalTensor
 * @param [in] src0 input0 LocalTensor
 * @param [in] src1 input1 LocalTensor
 * @param [count] count number of data calculation, must be even number
 */
template <typename T>
__aicore__ inline void DeInterleave(
    const LocalTensor<T>& dst0, const LocalTensor<T>& dst1, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
    const int32_t count)
{
    using PrimType = PrimT<T>;
    DeInterleaveImpl<PrimType>(
        (__ubuf__ PrimType*)dst0.GetPhyAddr(), (__ubuf__ PrimType*)dst1.GetPhyAddr(),
        (__ubuf__ PrimType*)src0.GetPhyAddr(), (__ubuf__ PrimType*)src1.GetPhyAddr(), count);
}

/*
 * @ingroup DeInterleave Level 2
 * @brief DeInterleave src to dst0 and dst1
 * @param [out] dst0 output0 LocalTensor
 * @param [out] dst1 output1 LocalTensor
 * @param [in] src input LocalTensor
 * @param [srcCount] srcCount number of data calculation, must be even number
 */
template <typename T>
__aicore__ inline void DeInterleave(
    const LocalTensor<T>& dst0, const LocalTensor<T>& dst1, const LocalTensor<T>& src, const int32_t srcCount)
{
    using PrimType = PrimT<T>;
    DeInterleaveImpl<PrimType>(
        (__ubuf__ PrimType*)dst0.GetPhyAddr(), (__ubuf__ PrimType*)dst1.GetPhyAddr(),
        (__ubuf__ PrimType*)src.GetPhyAddr(), srcCount);
}
#endif
} // namespace AscendC
#pragma end_pipe
#endif // ASCENDC_MODULE_OPERATOR_VEC_DUPLICATE_INTERFACE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_DUPLICATE_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_DUPLICATE_INTF_IMPL_H__
#endif
