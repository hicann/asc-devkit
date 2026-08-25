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
 * \file kernel_operator_vec_binary_scalar_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_vec_binary_scalar_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_vec_binary_scalar_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_SCALAR_INTF_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_BINARY_SCALAR_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_BINARY_SCALAR_INTERFACE_IMPL_H
#include "../../include/basic_api/kernel_tensor.h"
#include "kernel_check.h"
#include "kernel_npu_debug.h"
#include "../../include/basic_api/kernel_struct_unary.h"
#include "mstx_local_tensor_info.h"

#if __NPU_ARCH__ == 1001
#include "dav_c100/kernel_operator_vec_binary_scalar_impl.h"
#elif __NPU_ARCH__ == 2002
#include "dav_m200/kernel_operator_vec_binary_scalar_impl.h"
#elif __NPU_ARCH__ == 2201
#include "dav_c220/kernel_operator_vec_binary_scalar_impl.h"
#elif __NPU_ARCH__ == 3002
#include "dav_m300/kernel_operator_vec_binary_scalar_impl.h"
#elif __NPU_ARCH__ == 3102
#include "dav_m310/kernel_operator_vec_binary_scalar_impl.h"
#elif __NPU_ARCH__ == 3510
#include "dav_3510/kernel_operator_vec_binary_scalar_impl.h"
#elif (__NPU_ARCH__ == 5102)
#include "dav_m510/kernel_operator_vec_binary_scalar_impl.h"
#elif __NPU_ARCH__ == 3003
#include "dav_l300/kernel_operator_vec_binary_scalar_impl.h"
#elif __NPU_ARCH__ == 3113
#include "dav_l311/kernel_operator_vec_binary_scalar_impl.h"
#endif
#pragma begin_pipe(V)
namespace AscendC {
/* **************************************************************************************************
 * Adds                                             *
 * ************************************************************************************************* */
/*
 * @ingroup Adds Level 0
 * @brief dst[i] = src[i] + scalar
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.srcBlkStride src block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src repeat stride
 */
template <typename T, bool isSetMask>
__aicore__ inline void Adds(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Adds", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<T, isSetMask>(mask, repeatTime, "Adds");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, mask, repeatTime, repeatParams, "Adds")) {
        ASCENDC_REPORT_CHECK_ERROR("Adds", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask[0], mask[1], repeatTime, repeatParams, "Adds");
#endif
    AddsImpl<T, isSetMask>(
        (__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src.GetPhyAddr(), scalarValue, mask, repeatTime, repeatParams);
}

template <typename T, typename U, bool isSetMask, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type>
__aicore__ inline void Adds(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Adds", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Adds");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, mask, repeatTime, repeatParams, "Adds")) {
        ASCENDC_REPORT_CHECK_ERROR("Adds", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask[0], mask[1], repeatTime, repeatParams, "Adds");
#endif
    AddsImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), scalarValue, mask, repeatTime,
        repeatParams);
}

template <typename T, bool isSetMask>
__aicore__ inline void Adds(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Adds", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<T, isSetMask>(mask, repeatTime, "Adds");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, mask, repeatTime, repeatParams, "Adds")) {
        ASCENDC_REPORT_CHECK_ERROR("Adds", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask, repeatTime, repeatParams, "Adds");
#endif
    AddsImpl<T, isSetMask>(
        (__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src.GetPhyAddr(), scalarValue, mask, repeatTime, repeatParams);
}

template <typename T, typename U, bool isSetMask, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type>
__aicore__ inline void Adds(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Adds", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Adds");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, mask, repeatTime, repeatParams, "Adds")) {
        ASCENDC_REPORT_CHECK_ERROR("Adds", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask, repeatTime, repeatParams, "Adds");
#endif
    AddsImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), scalarValue, mask, repeatTime,
        repeatParams);
}

/*
 * @ingroup Adds Level 2
 * @brief dst = src[i] + scalar
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, bool isSetMask>
__aicore__ inline void Adds(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, const int32_t& count)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Adds", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckCalcount(count, "count", "Adds");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, count, "Adds")) {
        ASCENDC_REPORT_CHECK_ERROR("Adds", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, "Adds", count);
#endif
    AddsImpl<T, isSetMask>((__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src.GetPhyAddr(), scalarValue, count);
}

template <typename T, typename U, bool isSetMask, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type>
__aicore__ inline void Adds(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Adds", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckCalcount(count, "count", "Adds");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, count, "Adds")) {
        ASCENDC_REPORT_CHECK_ERROR("Adds", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, "Adds", count);
#endif
    AddsImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), scalarValue, count);
}

/* **************************************************************************************************
 * Muls                                             *
 * ************************************************************************************************* */
/*
 * @ingroup Muls Level 0
 * @brief dst[i] = src[i] * scalar
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.srcBlkStride src block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src repeat stride
 */
template <typename T, bool isSetMask>
__aicore__ inline void Muls(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Muls", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<T, isSetMask>(mask, repeatTime, "Muls");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, mask, repeatTime, repeatParams, "Muls")) {
        ASCENDC_REPORT_CHECK_ERROR("Muls", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask[0], mask[1], repeatTime, repeatParams, "Muls");
#endif
    MulsImpl<T, isSetMask>(
        (__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src.GetPhyAddr(), scalarValue, mask, repeatTime, repeatParams);
}

template <typename T, typename U, bool isSetMask, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type>
__aicore__ inline void Muls(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Muls", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Muls");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, mask, repeatTime, repeatParams, "Muls")) {
        ASCENDC_REPORT_CHECK_ERROR("Muls", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask[0], mask[1], repeatTime, repeatParams, "Muls");
#endif
    MulsImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), scalarValue, mask, repeatTime,
        repeatParams);
}

template <typename T, bool isSetMask>
__aicore__ inline void Muls(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Muls", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<T, isSetMask>(mask, repeatTime, "Muls");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, mask, repeatTime, repeatParams, "Muls")) {
        ASCENDC_REPORT_CHECK_ERROR("Muls", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask, repeatTime, repeatParams, "Muls");
#endif
    MulsImpl<T, isSetMask>(
        (__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src.GetPhyAddr(), scalarValue, mask, repeatTime, repeatParams);
}

template <typename T, typename U, bool isSetMask, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type>
__aicore__ inline void Muls(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Muls", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Muls");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, mask, repeatTime, repeatParams, "Muls")) {
        ASCENDC_REPORT_CHECK_ERROR("Muls", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask, repeatTime, repeatParams, "Muls");
#endif
    MulsImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), scalarValue, mask, repeatTime,
        repeatParams);
}

/*
 * @ingroup Muls Level 2
 * @brief dst = src[i] * scalar
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, bool isSetMask>
__aicore__ inline void Muls(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, const int32_t& count)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Muls", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckCalcount(count, "count", "Muls");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, count, "Muls")) {
        ASCENDC_REPORT_CHECK_ERROR("Muls", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, "Muls", count);
#endif
    MulsImpl<T, isSetMask>((__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src.GetPhyAddr(), scalarValue, count);
}

template <typename T, typename U, bool isSetMask, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type>
__aicore__ inline void Muls(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Muls", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckCalcount(count, "count", "Muls");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, count, "Muls")) {
        ASCENDC_REPORT_CHECK_ERROR("Muls", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, "Muls", count);
#endif
    MulsImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), scalarValue, count);
}

/* **************************************************************************************************
 * Maxs                                             *
 * ************************************************************************************************* */
/*
 * @ingroup Maxs Level 0
 * @brief dst[i] = src[i] > scalar ? src[0] : scalar
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.srcBlkStride src block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src repeat stride
 */
template <typename T, bool isSetMask>
__aicore__ inline void Maxs(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Maxs", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<T, isSetMask>(mask, repeatTime, "Maxs");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, mask, repeatTime, repeatParams, "Maxs")) {
        ASCENDC_REPORT_CHECK_ERROR("Maxs", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask[0], mask[1], repeatTime, repeatParams, "Maxs");
#endif
    MaxsImpl<T, isSetMask>(
        (__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src.GetPhyAddr(), scalarValue, mask, repeatTime, repeatParams);
}

template <typename T, typename U, bool isSetMask, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type>
__aicore__ inline void Maxs(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Maxs", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Maxs");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, mask, repeatTime, repeatParams, "Maxs")) {
        ASCENDC_REPORT_CHECK_ERROR("Maxs", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask[0], mask[1], repeatTime, repeatParams, "Maxs");
#endif
    MaxsImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), scalarValue, mask, repeatTime,
        repeatParams);
}

template <typename T, bool isSetMask>
__aicore__ inline void Maxs(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Maxs", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<T, isSetMask>(mask, repeatTime, "Maxs");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, mask, repeatTime, repeatParams, "Maxs")) {
        ASCENDC_REPORT_CHECK_ERROR("Maxs", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask, repeatTime, repeatParams, "Maxs");
#endif
    MaxsImpl<T, isSetMask>(
        (__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src.GetPhyAddr(), scalarValue, mask, repeatTime, repeatParams);
}

template <typename T, typename U, bool isSetMask, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type>
__aicore__ inline void Maxs(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Maxs", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Maxs");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, mask, repeatTime, repeatParams, "Maxs")) {
        ASCENDC_REPORT_CHECK_ERROR("Maxs", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask, repeatTime, repeatParams, "Maxs");
#endif
    MaxsImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), scalarValue, mask, repeatTime,
        repeatParams);
}

/*
 * @ingroup Maxs Level 2
 * @brief dst = src[i] > scalar ? src[0] : scalar
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, bool isSetMask>
__aicore__ inline void Maxs(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, const int32_t& count)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Maxs", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckCalcount(count, "count", "Maxs");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, count, "Maxs")) {
        ASCENDC_REPORT_CHECK_ERROR("Maxs", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, "Maxs", count);
#endif
    MaxsImpl<T, isSetMask>((__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src.GetPhyAddr(), scalarValue, count);
}

template <typename T, typename U, bool isSetMask, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type>
__aicore__ inline void Maxs(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Maxs", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckCalcount(count, "count", "Maxs");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, count, "Maxs")) {
        ASCENDC_REPORT_CHECK_ERROR("Maxs", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, "Maxs", count);
#endif
    MaxsImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), scalarValue, count);
}

/* **************************************************************************************************
 * Mins                                             *
 * ************************************************************************************************* */
/*
 * @ingroup Mins Level 0
 * @brief dst[i] = src[i] < scalar ? src[0] : scalar
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.srcBlkStride src block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src repeat stride
 */
template <typename T, bool isSetMask>
__aicore__ inline void Mins(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Mins", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<T, isSetMask>(mask, repeatTime, "Mins");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, mask, repeatTime, repeatParams, "Mins")) {
        ASCENDC_REPORT_CHECK_ERROR("Mins", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask[0], mask[1], repeatTime, repeatParams, "Mins");
#endif
    MinsImpl<T, isSetMask>(
        (__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src.GetPhyAddr(), scalarValue, mask, repeatTime, repeatParams);
}

template <typename T, typename U, bool isSetMask, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type>
__aicore__ inline void Mins(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Mins", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Mins");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, mask, repeatTime, repeatParams, "Mins")) {
        ASCENDC_REPORT_CHECK_ERROR("Mins", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask[0], mask[1], repeatTime, repeatParams, "Mins");
#endif
    MinsImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), scalarValue, mask, repeatTime,
        repeatParams);
}

template <typename T, bool isSetMask>
__aicore__ inline void Mins(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Mins", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<T, isSetMask>(mask, repeatTime, "Mins");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, mask, repeatTime, repeatParams, "Mins")) {
        ASCENDC_REPORT_CHECK_ERROR("Mins", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask, repeatTime, repeatParams, "Mins");
#endif
    MinsImpl<T, isSetMask>(
        (__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src.GetPhyAddr(), scalarValue, mask, repeatTime, repeatParams);
}

template <typename T, typename U, bool isSetMask, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type>
__aicore__ inline void Mins(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Mins", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Mins");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, mask, repeatTime, repeatParams, "Mins")) {
        ASCENDC_REPORT_CHECK_ERROR("Mins", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask, repeatTime, repeatParams, "Mins");
#endif
    MinsImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), scalarValue, mask, repeatTime,
        repeatParams);
}

/*
 * @ingroup Mins Level 2
 * @brief dst = src[i] < scalar ? src[0] : scalar
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, bool isSetMask>
__aicore__ inline void Mins(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, const int32_t& count)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Mins", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckCalcount(count, "count", "Mins");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, count, "Mins")) {
        ASCENDC_REPORT_CHECK_ERROR("Mins", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, "Mins", count);
#endif
    MinsImpl<T, isSetMask>((__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src.GetPhyAddr(), scalarValue, count);
}

template <typename T, typename U, bool isSetMask, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type>
__aicore__ inline void Mins(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Mins", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckCalcount(count, "count", "Mins");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, count, "Mins")) {
        ASCENDC_REPORT_CHECK_ERROR("Mins", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, "Mins", count);
#endif
    MinsImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), scalarValue, count);
}

/* **************************************************************************************************
 * ShiftLeft                                             *
 * ************************************************************************************************* */
/*
 * @ingroup ShiftLeft Level 0
 * @brief dst[i] = src[i] << scalar
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.srcBlkStride src block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src repeat stride
 */
template <typename T, bool isSetMask>
__aicore__ inline void ShiftLeft(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("ShiftLeft", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<T, isSetMask>(mask, repeatTime, "ShiftLeft");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, mask, repeatTime, repeatParams, "ShiftLeft")) {
        ASCENDC_REPORT_CHECK_ERROR("ShiftLeft", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask[0], mask[1], repeatTime, repeatParams, "ShiftLeft");
#endif
    ShiftLeftImpl<T, isSetMask>(
        (__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src.GetPhyAddr(), scalarValue, mask, repeatTime, repeatParams);
}

template <typename T, typename U, bool isSetMask, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type>
__aicore__ inline void ShiftLeft(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("ShiftLeft", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "ShiftLeft");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, mask, repeatTime, repeatParams, "ShiftLeft")) {
        ASCENDC_REPORT_CHECK_ERROR("ShiftLeft", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask[0], mask[1], repeatTime, repeatParams, "ShiftLeft");
#endif
    ShiftLeftImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), scalarValue, mask, repeatTime,
        repeatParams);
}

template <typename T, bool isSetMask>
__aicore__ inline void ShiftLeft(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("ShiftLeft", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<T, isSetMask>(mask, repeatTime, "ShiftLeft");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, mask, repeatTime, repeatParams, "ShiftLeft")) {
        ASCENDC_REPORT_CHECK_ERROR("ShiftLeft", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask, repeatTime, repeatParams, "ShiftLeft");
#endif
    ShiftLeftImpl<T, isSetMask>(
        (__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src.GetPhyAddr(), scalarValue, mask, repeatTime, repeatParams);
}

template <typename T, typename U, bool isSetMask, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type>
__aicore__ inline void ShiftLeft(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("ShiftLeft", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "ShiftLeft");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, mask, repeatTime, repeatParams, "ShiftLeft")) {
        ASCENDC_REPORT_CHECK_ERROR("ShiftLeft", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask, repeatTime, repeatParams, "ShiftLeft");
#endif
    ShiftLeftImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), scalarValue, mask, repeatTime,
        repeatParams);
}

/*
 * @ingroup Shiftleft Level 2
 * @brief dst = src[i] << scalar
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, bool isSetMask>
__aicore__ inline void ShiftLeft(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, const int32_t& count)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("ShiftLeft", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckCalcount(count, "count", "ShiftLeft");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, count, "ShiftLeft")) {
        ASCENDC_REPORT_CHECK_ERROR("ShiftLeft", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, "ShiftLeft", count);
#endif
    ShiftLeftImpl<T, isSetMask>((__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src.GetPhyAddr(), scalarValue, count);
}

template <typename T, typename U, bool isSetMask, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type>
__aicore__ inline void ShiftLeft(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("ShiftLeft", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckCalcount(count, "count", "ShiftLeft");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, count, "ShiftLeft")) {
        ASCENDC_REPORT_CHECK_ERROR("ShiftLeft", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, "ShiftLeft", count);
#endif
    ShiftLeftImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), scalarValue, count);
}

/* **************************************************************************************************
 * ShiftRight                                             *
 * ************************************************************************************************* */
/*
 * @ingroup ShiftRight Level 0
 * @brief dst[i] = src[i] >> scalar
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.srcBlkStride src block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src repeat stride
 */
template <typename T, bool isSetMask>
__aicore__ inline void ShiftRight(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams, bool roundEn)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("ShiftRight", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<T, isSetMask>(mask, repeatTime, "ShiftRight");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, mask, repeatTime, repeatParams, "ShiftRight")) {
        ASCENDC_REPORT_CHECK_ERROR("ShiftRight", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(
        dst, src, mask[0], mask[1], repeatTime, repeatParams, "ShiftRight");
#endif
    ShiftRightImpl<T, isSetMask>(
        (__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src.GetPhyAddr(), scalarValue, mask, repeatTime, repeatParams,
        roundEn);
}

template <typename T, typename U, bool isSetMask, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type>
__aicore__ inline void ShiftRight(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams, bool roundEn)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("ShiftRight", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "ShiftRight");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, mask, repeatTime, repeatParams, "ShiftRight")) {
        ASCENDC_REPORT_CHECK_ERROR("ShiftRight", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(
        dst, src, mask[0], mask[1], repeatTime, repeatParams, "ShiftRight");
#endif
    ShiftRightImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), scalarValue, mask, repeatTime,
        repeatParams, roundEn);
}

template <typename T, bool isSetMask>
__aicore__ inline void ShiftRight(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams, bool roundEn)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("ShiftRight", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<T, isSetMask>(mask, repeatTime, "ShiftRight");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, mask, repeatTime, repeatParams, "ShiftRight")) {
        ASCENDC_REPORT_CHECK_ERROR("ShiftRight", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask, repeatTime, repeatParams, "ShiftRight");
#endif
    ShiftRightImpl<T, isSetMask>(
        (__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src.GetPhyAddr(), scalarValue, mask, repeatTime, repeatParams,
        roundEn);
}

template <typename T, typename U, bool isSetMask, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type>
__aicore__ inline void ShiftRight(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams, bool roundEn)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("ShiftRight", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "ShiftRight");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, mask, repeatTime, repeatParams, "ShiftRight")) {
        ASCENDC_REPORT_CHECK_ERROR("ShiftRight", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask, repeatTime, repeatParams, "ShiftRight");
#endif
    ShiftRightImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), scalarValue, mask, repeatTime,
        repeatParams, roundEn);
}

/*
 * @ingroup ShiftRight Level 2
 * @brief dst = src[i] >> scalar
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, bool isSetMask>
__aicore__ inline void ShiftRight(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, const int32_t& count)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("ShiftRight", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckCalcount(count, "count", "ShiftRight");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, count, "ShiftRight")) {
        ASCENDC_REPORT_CHECK_ERROR("ShiftRight", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, "ShiftRight", count);
#endif
    ShiftRightImpl<T, isSetMask>((__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src.GetPhyAddr(), scalarValue, count);
}

template <typename T, typename U, bool isSetMask, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type>
__aicore__ inline void ShiftRight(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("ShiftRight", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckCalcount(count, "count", "ShiftRight");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, count, "ShiftRight")) {
        ASCENDC_REPORT_CHECK_ERROR("ShiftRight", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, "ShiftRight", count);
#endif
    ShiftRightImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), scalarValue, count);
}

/* **************************************************************************************************
 * LeakyRelu                                             *
 * ************************************************************************************************* */
/*
 * @ingroup LeakyRelu Level 0
 * @brief dst[i] = src[i] < 0 ? (scalar * src[i]) : src[i]
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.srcBlkStride src block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src repeat stride
 */
template <typename T, bool isSetMask>
__aicore__ inline void LeakyRelu(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("LeakyRelu", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<T, isSetMask>(mask, repeatTime, "LeakyRelu");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, mask, repeatTime, repeatParams, "LeakyRelu")) {
        ASCENDC_REPORT_CHECK_ERROR("LeakyRelu", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask[0], mask[1], repeatTime, repeatParams, "LeakyRelu");
#endif
    LeakyReluImpl<T, isSetMask>(
        (__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src.GetPhyAddr(), scalarValue, mask, repeatTime, repeatParams);
}

template <typename T, typename U, bool isSetMask, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type>
__aicore__ inline void LeakyRelu(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("LeakyRelu", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "LeakyRelu");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, mask, repeatTime, repeatParams, "LeakyRelu")) {
        ASCENDC_REPORT_CHECK_ERROR("LeakyRelu", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask[0], mask[1], repeatTime, repeatParams, "LeakyRelu");
#endif
    LeakyReluImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), scalarValue, mask, repeatTime,
        repeatParams);
}

template <typename T, bool isSetMask>
__aicore__ inline void LeakyRelu(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("LeakyRelu", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<T, isSetMask>(mask, repeatTime, "LeakyRelu");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, mask, repeatTime, repeatParams, "LeakyRelu")) {
        ASCENDC_REPORT_CHECK_ERROR("LeakyRelu", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask, repeatTime, repeatParams, "LeakyRelu");
#endif
    LeakyReluImpl<T, isSetMask>(
        (__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src.GetPhyAddr(), scalarValue, mask, repeatTime, repeatParams);
}

template <typename T, typename U, bool isSetMask, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type>
__aicore__ inline void LeakyRelu(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("LeakyRelu", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "LeakyRelu");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, mask, repeatTime, repeatParams, "LeakyRelu")) {
        ASCENDC_REPORT_CHECK_ERROR("LeakyRelu", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask, repeatTime, repeatParams, "LeakyRelu");
#endif
    LeakyReluImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), scalarValue, mask, repeatTime,
        repeatParams);
}

/*
 * @ingroup LeakyRelu Level 2
 * @brief dst = src[i] < 0 ? (scalar * src[i]) : src[i]
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, bool isSetMask>
__aicore__ inline void LeakyRelu(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, const int32_t& count)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("LeakyRelu", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckCalcount(count, "count", "LeakyRelu");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, count, "LeakyRelu")) {
        ASCENDC_REPORT_CHECK_ERROR("LeakyRelu", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, "LeakyRelu", count);
#endif
    LeakyReluImpl<T, isSetMask>((__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src.GetPhyAddr(), scalarValue, count);
}

template <typename T, typename U, bool isSetMask, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type>
__aicore__ inline void LeakyRelu(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("LeakyRelu", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckCalcount(count, "count", "LeakyRelu");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, scalarValue, count, "LeakyRelu")) {
        ASCENDC_REPORT_CHECK_ERROR("LeakyRelu", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, "LeakyRelu", count);
#endif
    LeakyReluImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), scalarValue, count);
}

#if defined(__NPU_ARCH__) && \
    ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
/* **************************************************************************************************
 * Adds                                             *
 * ************************************************************************************************* */
template <bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V, typename MaskType>
__aicore__ inline void AddsCommon(
    const U& dst, const S& src0, const V& src1, MaskType mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert(!TypeUtils::IsInnerDefaultType<S, V>(), "Adds API: One of src0 and src1 should be Tensor");
    static_assert(TypeUtils::IsLocalTensorType<U>());
    static_assert(
        SupportType<
            typename U::PrimType, uint8_t, int8_t, half, bfloat16_t, int16_t, float, int32_t, int64_t, uint64_t>());
    using ActualT = typename U::PrimType;
    if constexpr (TypeUtils::IsLocalTensorType<S, V>()) {
        static_assert(Std::is_same<S, V>::value);
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        static_assert(
            (config.scalarTensorIndex == 0 || config.scalarTensorIndex == 1), "scalarTensorIndex out of range");
        AddsImpl<ActualT, isSetMask, config.scalarTensorIndex>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(),
            (__ubuf__ ActualT*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
    } else if constexpr (TypeUtils::IsLocalTensorType<V>() && TypeUtils::IsInnerDefaultType<S>()) {
        static_assert(Std::is_same<ActualT, typename V::PrimType>::value);
        AddsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src1.GetPhyAddr(), src0, mask, repeatTime,
            repeatParams);
    } else if constexpr (TypeUtils::IsLocalTensorType<S>() && TypeUtils::IsInnerDefaultType<V>()) {
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        AddsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(), src1, mask, repeatTime,
            repeatParams);
    }
}

template <typename T, bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V>
__aicore__ inline void Adds(
    const U& dst, const S& src0, const V& src1, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar<config>(dst, src0, src1, mask, repeatTime, repeatParams, "Adds")) {
        ASCENDC_REPORT_CHECK_ERROR("Adds", KernelFuncType::MASK_BIT_MODE);
    }
#endif
    AddsCommon<isSetMask, config>(dst, src0, src1, mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V>
__aicore__ inline void Adds(
    const U& dst, const S& src0, const V& src1, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar<config>(dst, src0, src1, mask, repeatTime, repeatParams, "Adds")) {
        ASCENDC_REPORT_CHECK_ERROR("Adds", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
    AddsCommon<isSetMask, config>(dst, src0, src1, mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V>
__aicore__ inline void Adds(const U& dst, const S& src0, const V& src1, const int32_t& count)
{
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar<config>(dst, src0, src1, count, "Adds")) {
        ASCENDC_REPORT_CHECK_ERROR("Adds", KernelFuncType::CALCOUNT_MODE);
    }
#endif
    static_assert(!TypeUtils::IsInnerDefaultType<S, V>(), "One of src0 and src1 should be Tensor");
    static_assert(TypeUtils::IsLocalTensorType<U>());
    static_assert(SupportType<
                  typename U::PrimType, uint8_t, int8_t, half, bfloat16_t, int16_t, float, int32_t, int64_t, uint64_t,
                  complex32, complex64>());
    using ActualT = typename U::PrimType;
    if constexpr (TypeUtils::IsLocalTensorType<S, V>()) {
        static_assert(Std::is_same<S, V>::value);
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        static_assert(
            (config.scalarTensorIndex == 0 || config.scalarTensorIndex == 1), "scalarTensorIndex out of range");
        AddsImpl<ActualT, isSetMask, config.scalarTensorIndex>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(),
            (__ubuf__ ActualT*)src1.GetPhyAddr(), count);
    } else if constexpr (TypeUtils::IsLocalTensorType<V>() && TypeUtils::IsInnerDefaultType<S>()) {
        static_assert(Std::is_same<ActualT, typename V::PrimType>::value);
        AddsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src1.GetPhyAddr(), src0, count);
    } else if constexpr (TypeUtils::IsLocalTensorType<S>() && TypeUtils::IsInnerDefaultType<V>()) {
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        AddsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(), src1, count);
    }
}

/* **************************************************************************************************
 * Muls                                             *
 * ************************************************************************************************* */
template <bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V, typename MaskType>
__aicore__ inline void MulsCommon(
    const U& dst, const S& src0, const V& src1, MaskType mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert(!TypeUtils::IsInnerDefaultType<S, V>(), "Muls API: One of src0 and src1 should be Tensor");
    static_assert(TypeUtils::IsLocalTensorType<U>());
    static_assert(SupportType<typename U::PrimType, half, bfloat16_t, int16_t, float, int32_t, int64_t, uint64_t>());
    using ActualT = typename U::PrimType;
    if constexpr (TypeUtils::IsLocalTensorType<S, V>()) {
        static_assert(Std::is_same<S, V>::value);
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        static_assert(
            (config.scalarTensorIndex == 0 || config.scalarTensorIndex == 1), "scalarTensorIndex out of range");
        MulsImpl<ActualT, isSetMask, config.scalarTensorIndex>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(),
            (__ubuf__ ActualT*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
    } else if constexpr (TypeUtils::IsLocalTensorType<V>() && TypeUtils::IsInnerDefaultType<S>()) {
        static_assert(Std::is_same<ActualT, typename V::PrimType>::value);
        MulsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src1.GetPhyAddr(), src0, mask, repeatTime,
            repeatParams);
    } else if constexpr (TypeUtils::IsLocalTensorType<S>() && TypeUtils::IsInnerDefaultType<V>()) {
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        MulsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(), src1, mask, repeatTime,
            repeatParams);
    }
}

template <typename T, bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V>
__aicore__ inline void Muls(
    const U& dst, const S& src0, const V& src1, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar<config>(dst, src0, src1, mask, repeatTime, repeatParams, "Muls")) {
        ASCENDC_REPORT_CHECK_ERROR("Muls", KernelFuncType::MASK_BIT_MODE);
    }
#endif
    MulsCommon<isSetMask, config>(dst, src0, src1, mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V>
__aicore__ inline void Muls(
    const U& dst, const S& src0, const V& src1, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar<config>(dst, src0, src1, mask, repeatTime, repeatParams, "Muls")) {
        ASCENDC_REPORT_CHECK_ERROR("Muls", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
    MulsCommon<isSetMask, config>(dst, src0, src1, mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V>
__aicore__ inline void Muls(const U& dst, const S& src0, const V& src1, const int32_t& count)
{
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar<config>(dst, src0, src1, count, "Muls")) {
        ASCENDC_REPORT_CHECK_ERROR("Muls", KernelFuncType::CALCOUNT_MODE);
    }
#endif
    static_assert(!TypeUtils::IsInnerDefaultType<S, V>(), "One of src0 and src1 should be Tensor");
    static_assert(TypeUtils::IsLocalTensorType<U>());
    static_assert(SupportType<
                  typename U::PrimType, half, bfloat16_t, int16_t, float, int32_t, int64_t, uint64_t, complex32,
                  complex64>());
    using ActualT = typename U::PrimType;
    if constexpr (TypeUtils::IsLocalTensorType<S, V>()) {
        static_assert(Std::is_same<S, V>::value);
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        static_assert(
            (config.scalarTensorIndex == 0 || config.scalarTensorIndex == 1), "scalarTensorIndex out of range");
        MulsImpl<ActualT, isSetMask, config.scalarTensorIndex>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(),
            (__ubuf__ ActualT*)src1.GetPhyAddr(), count);
    } else if constexpr (TypeUtils::IsLocalTensorType<V>() && TypeUtils::IsInnerDefaultType<S>()) {
        static_assert(Std::is_same<ActualT, typename V::PrimType>::value);
        MulsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src1.GetPhyAddr(), src0, count);
    } else if constexpr (TypeUtils::IsLocalTensorType<S>() && TypeUtils::IsInnerDefaultType<V>()) {
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        MulsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(), src1, count);
    }
}

/* **************************************************************************************************
 * Maxs                                             *
 * ************************************************************************************************* */
template <bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V, typename MaskType>
__aicore__ inline void MaxsCommon(
    const U& dst, const S& src0, const V& src1, MaskType mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert(!TypeUtils::IsInnerDefaultType<S, V>(), "Maxs API: One of src0 and src1 should be Tensor");
    static_assert(TypeUtils::IsLocalTensorType<U>());
    static_assert(
        SupportType<
            typename U::PrimType, uint8_t, int8_t, half, bfloat16_t, int16_t, float, int32_t, int64_t, uint64_t>());
    using ActualT = typename U::PrimType;
    if constexpr (TypeUtils::IsLocalTensorType<S, V>()) {
        static_assert(Std::is_same<S, V>::value);
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        static_assert(
            (config.scalarTensorIndex == 0 || config.scalarTensorIndex == 1), "scalarTensorIndex out of range");
        MaxsImpl<ActualT, isSetMask, config.scalarTensorIndex>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(),
            (__ubuf__ ActualT*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
    } else if constexpr (TypeUtils::IsLocalTensorType<V>() && TypeUtils::IsInnerDefaultType<S>()) {
        static_assert(Std::is_same<ActualT, typename V::PrimType>::value);
        MaxsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src1.GetPhyAddr(), src0, mask, repeatTime,
            repeatParams);
    } else if constexpr (TypeUtils::IsLocalTensorType<S>() && TypeUtils::IsInnerDefaultType<V>()) {
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        MaxsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(), src1, mask, repeatTime,
            repeatParams);
    }
}

template <typename T, bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V>
__aicore__ inline void Maxs(
    const U& dst, const S& src0, const V& src1, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar<config>(dst, src0, src1, mask, repeatTime, repeatParams, "Maxs")) {
        ASCENDC_REPORT_CHECK_ERROR("Maxs", KernelFuncType::MASK_BIT_MODE);
    }
#endif
    MaxsCommon<isSetMask, config>(dst, src0, src1, mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V>
__aicore__ inline void Maxs(
    const U& dst, const S& src0, const V& src1, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar<config>(dst, src0, src1, mask, repeatTime, repeatParams, "Maxs")) {
        ASCENDC_REPORT_CHECK_ERROR("Maxs", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
    MaxsCommon<isSetMask, config>(dst, src0, src1, mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V>
__aicore__ inline void Maxs(const U& dst, const S& src0, const V& src1, const int32_t& count)
{
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar<config>(dst, src0, src1, count, "Maxs")) {
        ASCENDC_REPORT_CHECK_ERROR("Maxs", KernelFuncType::CALCOUNT_MODE);
    }
#endif
    static_assert(!TypeUtils::IsInnerDefaultType<S, V>(), "One of src0 and src1 should be Tensor");
    static_assert(TypeUtils::IsLocalTensorType<U>());
    static_assert(
        SupportType<
            typename U::PrimType, uint8_t, int8_t, half, bfloat16_t, int16_t, float, int32_t, int64_t, uint64_t>());
    using ActualT = typename U::PrimType;
    if constexpr (TypeUtils::IsLocalTensorType<S, V>()) {
        static_assert(Std::is_same<S, V>::value);
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        static_assert(
            (config.scalarTensorIndex == 0 || config.scalarTensorIndex == 1), "scalarTensorIndex out of range");
        MaxsImpl<ActualT, isSetMask, config.scalarTensorIndex>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(),
            (__ubuf__ ActualT*)src1.GetPhyAddr(), count);
    } else if constexpr (TypeUtils::IsLocalTensorType<V>() && TypeUtils::IsInnerDefaultType<S>()) {
        static_assert(Std::is_same<ActualT, typename V::PrimType>::value);
        MaxsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src1.GetPhyAddr(), src0, count);
    } else if constexpr (TypeUtils::IsLocalTensorType<S>() && TypeUtils::IsInnerDefaultType<V>()) {
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        MaxsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(), src1, count);
    }
}

/* **************************************************************************************************
 * Mins                                             *
 * ************************************************************************************************* */
template <bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V, typename MaskType>
__aicore__ inline void MinsCommon(
    const U& dst, const S& src0, const V& src1, MaskType mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert(!TypeUtils::IsInnerDefaultType<S, V>(), "Mins API: One of src0 and src1 should be Tensor");
    static_assert(TypeUtils::IsLocalTensorType<U>());
    static_assert(
        SupportType<
            typename U::PrimType, uint8_t, int8_t, half, bfloat16_t, int16_t, float, int32_t, int64_t, uint64_t>());
    using ActualT = typename U::PrimType;
    if constexpr (TypeUtils::IsLocalTensorType<S, V>()) {
        static_assert(Std::is_same<S, V>::value);
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        static_assert(
            (config.scalarTensorIndex == 0 || config.scalarTensorIndex == 1), "scalarTensorIndex out of range");
        MinsImpl<ActualT, isSetMask, config.scalarTensorIndex>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(),
            (__ubuf__ ActualT*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
    } else if constexpr (TypeUtils::IsLocalTensorType<V>() && TypeUtils::IsInnerDefaultType<S>()) {
        static_assert(Std::is_same<ActualT, typename V::PrimType>::value);
        MinsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src1.GetPhyAddr(), src0, mask, repeatTime,
            repeatParams);
    } else if constexpr (TypeUtils::IsLocalTensorType<S>() && TypeUtils::IsInnerDefaultType<V>()) {
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        MinsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(), src1, mask, repeatTime,
            repeatParams);
    }
}

template <typename T, bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V>
__aicore__ inline void Mins(
    const U& dst, const S& src0, const V& src1, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar<config>(dst, src0, src1, mask, repeatTime, repeatParams, "Mins")) {
        ASCENDC_REPORT_CHECK_ERROR("Mins", KernelFuncType::MASK_BIT_MODE);
    }
#endif
    MinsCommon<isSetMask, config>(dst, src0, src1, mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V>
__aicore__ inline void Mins(
    const U& dst, const S& src0, const V& src1, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar<config>(dst, src0, src1, mask, repeatTime, repeatParams, "Mins")) {
        ASCENDC_REPORT_CHECK_ERROR("Mins", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
    MinsCommon<isSetMask, config>(dst, src0, src1, mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V>
__aicore__ inline void Mins(const U& dst, const S& src0, const V& src1, const int32_t& count)
{
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar<config>(dst, src0, src1, count, "Mins")) {
        ASCENDC_REPORT_CHECK_ERROR("Mins", KernelFuncType::CALCOUNT_MODE);
    }
#endif
    static_assert(!TypeUtils::IsInnerDefaultType<S, V>(), "One of src0 and src1 should be Tensor");
    static_assert(TypeUtils::IsLocalTensorType<U>());
    static_assert(
        SupportType<
            typename U::PrimType, uint8_t, int8_t, half, bfloat16_t, int16_t, float, int32_t, int64_t, uint64_t>());
    using ActualT = typename U::PrimType;
    if constexpr (TypeUtils::IsLocalTensorType<S, V>()) {
        static_assert(Std::is_same<S, V>::value);
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        static_assert(
            (config.scalarTensorIndex == 0 || config.scalarTensorIndex == 1), "scalarTensorIndex out of range");
        MinsImpl<ActualT, isSetMask, config.scalarTensorIndex>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(),
            (__ubuf__ ActualT*)src1.GetPhyAddr(), count);
    } else if constexpr (TypeUtils::IsLocalTensorType<V>() && TypeUtils::IsInnerDefaultType<S>()) {
        static_assert(Std::is_same<ActualT, typename V::PrimType>::value);
        MinsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src1.GetPhyAddr(), src0, count);
    } else if constexpr (TypeUtils::IsLocalTensorType<S>() && TypeUtils::IsInnerDefaultType<V>()) {
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        MinsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(), src1, count);
    }
}

/* **************************************************************************************************
 * Ands                                             *
 * ************************************************************************************************* */
template <bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V, typename MaskType>
__aicore__ inline void AndsCommon(
    const U& dst, const S& src0, const V& src1, MaskType mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert(!TypeUtils::IsInnerDefaultType<S, V>(), "Ands API: One of src0 and src1 should be Tensor");
    static_assert(TypeUtils::IsLocalTensorType<U>());
    static_assert(SupportType<typename U::PrimType, uint16_t, int16_t, int64_t, uint64_t>());
    using ActualT = typename U::PrimType;
    if constexpr (TypeUtils::IsLocalTensorType<S, V>()) {
        static_assert(Std::is_same<S, V>::value);
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        static_assert(
            (config.scalarTensorIndex == 0 || config.scalarTensorIndex == 1), "scalarTensorIndex out of range");
        AndsImpl<ActualT, isSetMask, config.scalarTensorIndex>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(),
            (__ubuf__ ActualT*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
    } else if constexpr (TypeUtils::IsLocalTensorType<V>() && TypeUtils::IsInnerDefaultType<S>()) {
        static_assert(Std::is_same<ActualT, typename V::PrimType>::value);
        AndsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src1.GetPhyAddr(), src0, mask, repeatTime,
            repeatParams);
    } else if constexpr (TypeUtils::IsLocalTensorType<S>() && TypeUtils::IsInnerDefaultType<V>()) {
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        AndsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(), src1, mask, repeatTime,
            repeatParams);
    }
}

template <typename T, bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V>
__aicore__ inline void Ands(
    const U& dst, const S& src0, const V& src1, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar<config>(dst, src0, src1, mask, repeatTime, repeatParams, "Ands")) {
        ASCENDC_REPORT_CHECK_ERROR("Ands", KernelFuncType::MASK_BIT_MODE);
    }
#endif
    AndsCommon<isSetMask, config>(dst, src0, src1, mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V>
__aicore__ inline void Ands(
    const U& dst, const S& src0, const V& src1, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar<config>(dst, src0, src1, mask, repeatTime, repeatParams, "Ands")) {
        ASCENDC_REPORT_CHECK_ERROR("Ands", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
    AndsCommon<isSetMask, config>(dst, src0, src1, mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V>
__aicore__ inline void Ands(const U& dst, const S& src0, const V& src1, const int32_t& count)
{
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar<config>(dst, src0, src1, count, "Ands")) {
        ASCENDC_REPORT_CHECK_ERROR("Ands", KernelFuncType::CALCOUNT_MODE);
    }
#endif
    static_assert(!TypeUtils::IsInnerDefaultType<S, V>(), "One of src0 and src1 should be Tensor");
    static_assert(TypeUtils::IsLocalTensorType<U>());
    static_assert(SupportType<typename U::PrimType, uint16_t, int16_t, int64_t, uint64_t>());
    using ActualT = typename U::PrimType;
    if constexpr (TypeUtils::IsLocalTensorType<S, V>()) {
        static_assert(Std::is_same<S, V>::value);
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        static_assert(
            (config.scalarTensorIndex == 0 || config.scalarTensorIndex == 1), "scalarTensorIndex out of range");
        AndsImpl<ActualT, isSetMask, config.scalarTensorIndex>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(),
            (__ubuf__ ActualT*)src1.GetPhyAddr(), count);
    } else if constexpr (TypeUtils::IsLocalTensorType<V>() && TypeUtils::IsInnerDefaultType<S>()) {
        static_assert(Std::is_same<ActualT, typename V::PrimType>::value);
        AndsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src1.GetPhyAddr(), src0, count);
    } else if constexpr (TypeUtils::IsLocalTensorType<S>() && TypeUtils::IsInnerDefaultType<V>()) {
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        AndsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(), src1, count);
    }
}

/* **************************************************************************************************
 * Ors                                             *
 * ************************************************************************************************* */
template <bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V, typename MaskType>
__aicore__ inline void OrsCommon(
    const U& dst, const S& src0, const V& src1, MaskType mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert(!TypeUtils::IsInnerDefaultType<S, V>(), "Ors API: One of src0 and src1 should be Tensor");
    static_assert(TypeUtils::IsLocalTensorType<U>());
    static_assert(SupportType<typename U::PrimType, uint16_t, int16_t, int64_t, uint64_t>());
    using ActualT = typename U::PrimType;
    if constexpr (TypeUtils::IsLocalTensorType<S, V>()) {
        static_assert(Std::is_same<S, V>::value);
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        static_assert(
            (config.scalarTensorIndex == 0 || config.scalarTensorIndex == 1), "scalarTensorIndex out of range");
        OrsImpl<ActualT, isSetMask, config.scalarTensorIndex>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(),
            (__ubuf__ ActualT*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
    } else if constexpr (TypeUtils::IsLocalTensorType<V>() && TypeUtils::IsInnerDefaultType<S>()) {
        static_assert(Std::is_same<ActualT, typename V::PrimType>::value);
        OrsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src1.GetPhyAddr(), src0, mask, repeatTime,
            repeatParams);
    } else if constexpr (TypeUtils::IsLocalTensorType<S>() && TypeUtils::IsInnerDefaultType<V>()) {
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        OrsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(), src1, mask, repeatTime,
            repeatParams);
    }
}

template <typename T, bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V>
__aicore__ inline void Ors(
    const U& dst, const S& src0, const V& src1, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar<config>(dst, src0, src1, mask, repeatTime, repeatParams, "Ors")) {
        ASCENDC_REPORT_CHECK_ERROR("Ors", KernelFuncType::MASK_BIT_MODE);
    }
#endif
    OrsCommon<isSetMask, config>(dst, src0, src1, mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V>
__aicore__ inline void Ors(
    const U& dst, const S& src0, const V& src1, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar<config>(dst, src0, src1, mask, repeatTime, repeatParams, "Ors")) {
        ASCENDC_REPORT_CHECK_ERROR("Ors", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
    OrsCommon<isSetMask, config>(dst, src0, src1, mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V>
__aicore__ inline void Ors(const U& dst, const S& src0, const V& src1, const int32_t& count)
{
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar<config>(dst, src0, src1, count, "Ors")) {
        ASCENDC_REPORT_CHECK_ERROR("Ors", KernelFuncType::CALCOUNT_MODE);
    }
#endif
    static_assert(!TypeUtils::IsInnerDefaultType<S, V>(), "One of src0 and src1 should be Tensor");
    static_assert(TypeUtils::IsLocalTensorType<U>());
    static_assert(SupportType<typename U::PrimType, uint16_t, int16_t, int64_t, uint64_t>());
    using ActualT = typename U::PrimType;
    if constexpr (TypeUtils::IsLocalTensorType<S, V>()) {
        static_assert(Std::is_same<S, V>::value);
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        static_assert(
            (config.scalarTensorIndex == 0 || config.scalarTensorIndex == 1), "scalarTensorIndex out of range");
        OrsImpl<ActualT, isSetMask, config.scalarTensorIndex>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(),
            (__ubuf__ ActualT*)src1.GetPhyAddr(), count);
    } else if constexpr (TypeUtils::IsLocalTensorType<V>() && TypeUtils::IsInnerDefaultType<S>()) {
        static_assert(Std::is_same<ActualT, typename V::PrimType>::value);
        OrsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src1.GetPhyAddr(), src0, count);
    } else if constexpr (TypeUtils::IsLocalTensorType<S>() && TypeUtils::IsInnerDefaultType<V>()) {
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        OrsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(), src1, count);
    }
}

/* **************************************************************************************************
 * Subs                                             *
 * ************************************************************************************************* */
template <bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V, typename MaskType>
__aicore__ inline void SubsCommon(
    const U& dst, const S& src0, const V& src1, MaskType mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert(!TypeUtils::IsInnerDefaultType<S, V>(), "Subs API: One of src0 and src1 should be Tensor");
    static_assert(TypeUtils::IsLocalTensorType<U>());
    static_assert(SupportType<typename U::PrimType, half, bfloat16_t, int16_t, float, int32_t, int64_t, uint64_t>());
    using ActualT = typename U::PrimType;
    if constexpr (TypeUtils::IsLocalTensorType<S, V>()) {
        static_assert(Std::is_same<S, V>::value);
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        static_assert(
            (config.scalarTensorIndex == 0 || config.scalarTensorIndex == 1), "scalarTensorIndex out of range");
        SubsImpl<ActualT, isSetMask, config.scalarTensorIndex>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(),
            (__ubuf__ ActualT*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
    } else if constexpr (TypeUtils::IsLocalTensorType<V>() && TypeUtils::IsInnerDefaultType<S>()) {
        static_assert(Std::is_same<ActualT, typename V::PrimType>::value);
        SubsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), src0, (__ubuf__ ActualT*)src1.GetPhyAddr(), mask, repeatTime,
            repeatParams);
    } else if constexpr (TypeUtils::IsLocalTensorType<S>() && TypeUtils::IsInnerDefaultType<V>()) {
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        SubsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(), src1, mask, repeatTime,
            repeatParams);
    }
}

template <typename T, bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V>
__aicore__ inline void Subs(
    const U& dst, const S& src0, const V& src1, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar<config>(dst, src0, src1, mask, repeatTime, repeatParams, "Subs")) {
        ASCENDC_REPORT_CHECK_ERROR("Subs", KernelFuncType::MASK_BIT_MODE);
    }
#endif
    SubsCommon<isSetMask, config>(dst, src0, src1, mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V>
__aicore__ inline void Subs(
    const U& dst, const S& src0, const V& src1, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar<config>(dst, src0, src1, mask, repeatTime, repeatParams, "Subs")) {
        ASCENDC_REPORT_CHECK_ERROR("Subs", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
    SubsCommon<isSetMask, config>(dst, src0, src1, mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V>
__aicore__ inline void Subs(const U& dst, const S& src0, const V& src1, const int32_t& count)
{
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar<config>(dst, src0, src1, count, "Subs")) {
        ASCENDC_REPORT_CHECK_ERROR("Subs", KernelFuncType::CALCOUNT_MODE);
    }
#endif
    static_assert(!TypeUtils::IsInnerDefaultType<S, V>(), "One of src0 and src1 should be Tensor");
    static_assert(TypeUtils::IsLocalTensorType<U>());
    static_assert(SupportType<
                  typename U::PrimType, half, bfloat16_t, int16_t, float, int32_t, int64_t, uint64_t, complex32,
                  complex64>());
    using ActualT = typename U::PrimType;
    if constexpr (TypeUtils::IsLocalTensorType<S, V>()) {
        static_assert(Std::is_same<S, V>::value);
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        static_assert(
            (config.scalarTensorIndex == 0 || config.scalarTensorIndex == 1), "scalarTensorIndex out of range");
        SubsImpl<ActualT, isSetMask, config.scalarTensorIndex>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(),
            (__ubuf__ ActualT*)src1.GetPhyAddr(), count);
    } else if constexpr (TypeUtils::IsLocalTensorType<V>() && TypeUtils::IsInnerDefaultType<S>()) {
        static_assert(Std::is_same<ActualT, typename V::PrimType>::value);
        SubsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), src0, (__ubuf__ ActualT*)src1.GetPhyAddr(), count);
    } else if constexpr (TypeUtils::IsLocalTensorType<S>() && TypeUtils::IsInnerDefaultType<V>()) {
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        SubsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(), src1, count);
    }
}

/* **************************************************************************************************
 * Divs                                             *
 * ************************************************************************************************* */
template <bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V, typename MaskType>
__aicore__ inline void DivsCommon(
    const U& dst, const S& src0, const V& src1, MaskType mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert(!TypeUtils::IsInnerDefaultType<S, V>(), "Divs API: One of src0 and src1 should be Tensor");
    static_assert(TypeUtils::IsLocalTensorType<U>());
    static_assert(SupportType<typename U::PrimType, half, float, int64_t, uint64_t>());
    using ActualT = typename U::PrimType;
    if constexpr (TypeUtils::IsLocalTensorType<S, V>()) {
        static_assert(Std::is_same<S, V>::value);
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        static_assert(
            (config.scalarTensorIndex == 0 || config.scalarTensorIndex == 1), "scalarTensorIndex out of range");
        DivsImpl<ActualT, isSetMask, config.scalarTensorIndex>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(),
            (__ubuf__ ActualT*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
    } else if constexpr (TypeUtils::IsLocalTensorType<V>() && TypeUtils::IsInnerDefaultType<S>()) {
        static_assert(Std::is_same<ActualT, typename V::PrimType>::value);
        DivsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), src0, (__ubuf__ ActualT*)src1.GetPhyAddr(), mask, repeatTime,
            repeatParams);
    } else if constexpr (TypeUtils::IsLocalTensorType<S>() && TypeUtils::IsInnerDefaultType<V>()) {
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        DivsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(), src1, mask, repeatTime,
            repeatParams);
    }
}

template <typename T, bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V>
__aicore__ inline void Divs(
    const U& dst, const S& src0, const V& src1, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar<config>(dst, src0, src1, mask, repeatTime, repeatParams, "Divs")) {
        ASCENDC_REPORT_CHECK_ERROR("Divs", KernelFuncType::MASK_BIT_MODE);
    }
#endif
    DivsCommon<isSetMask, config>(dst, src0, src1, mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V>
__aicore__ inline void Divs(
    const U& dst, const S& src0, const V& src1, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar<config>(dst, src0, src1, mask, repeatTime, repeatParams, "Divs")) {
        ASCENDC_REPORT_CHECK_ERROR("Divs", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
    DivsCommon<isSetMask, config>(dst, src0, src1, mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask, const BinaryConfig& config, typename U, typename S, typename V>
__aicore__ inline void Divs(const U& dst, const S& src0, const V& src1, const int32_t& count)
{
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar<config>(dst, src0, src1, count, "Divs")) {
        ASCENDC_REPORT_CHECK_ERROR("Divs", KernelFuncType::CALCOUNT_MODE);
    }
#endif
    static_assert(!TypeUtils::IsInnerDefaultType<S, V>(), "One of src0 and src1 should be Tensor");
    static_assert(TypeUtils::IsLocalTensorType<U>());
    static_assert(SupportType<typename U::PrimType, half, float, int64_t, uint64_t, complex32, complex64>());
    using ActualT = typename U::PrimType;
    if constexpr (TypeUtils::IsLocalTensorType<S, V>()) {
        static_assert(Std::is_same<S, V>::value);
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        static_assert(
            (config.scalarTensorIndex == 0 || config.scalarTensorIndex == 1), "scalarTensorIndex out of range");
        DivsImpl<ActualT, isSetMask, config.scalarTensorIndex>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(),
            (__ubuf__ ActualT*)src1.GetPhyAddr(), count);
    } else if constexpr (TypeUtils::IsLocalTensorType<V>() && TypeUtils::IsInnerDefaultType<S>()) {
        static_assert(Std::is_same<ActualT, typename V::PrimType>::value);
        DivsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), src0, (__ubuf__ ActualT*)src1.GetPhyAddr(), count);
    } else if constexpr (TypeUtils::IsLocalTensorType<S>() && TypeUtils::IsInnerDefaultType<V>()) {
        static_assert(Std::is_same<ActualT, typename S::PrimType>::value);
        DivsImpl<ActualT, isSetMask>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(), src1, count);
    }
}

/* **************************************************************************************************
 *MulsCast                                             *
 * ************************************************************************************************* */
/*
 * @ingroup MulsCast Level 2
 * @brief dst = abs(src0 - src1)
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T0, typename T1, const BinaryConfig& config, typename T2, typename T3, typename T4>
__aicore__ inline void MulsCast(const T2& dst, const T3& src0, const T4& src1, const uint32_t count)
{
    using ActualT = typename T2::PrimType;
    CheckTensorPos<ActualT>(dst, Hardware::UB, "dst", "VECIN/VECCALC/VECOUT", "MulsCast");
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113)
    static_assert(!TypeUtils::IsInnerDefaultType<T3, T4>(), "One of src0 and src1 should be Tensor");
    static_assert(TypeUtils::IsLocalTensorType<T2>());
    static_assert(SupportType<typename T2::PrimType, half>());
    if constexpr (Std::is_same<T3, T4>::value) {
        ASCENDC_ASSERT((count <= src0.GetSize() && count <= src1.GetSize() && count <= dst.GetSize()), {
            KERNEL_LOG(
                KERNEL_ERROR, "count is %u, which should not larger than tensor size of dst / src0 / src1", count);
        });
        using ActualU = typename T3::PrimType;
        CheckTensorPos<ActualU>(src0, Hardware::UB, "src0", "VECIN/VECCALC/VECOUT", "MulsCast");
        CheckTensorPos<ActualU>(src1, Hardware::UB, "src1", "VECIN/VECCALC/VECOUT", "MulsCast");
        static_assert(Std::is_same<T3, T4>::value);
        static_assert(Std::is_same<ActualT, typename T2::PrimType>::value);
        static_assert(
            (config.scalarTensorIndex == 0 || config.scalarTensorIndex == 1), "scalarTensorIndex out of range");
        if constexpr (config.scalarTensorIndex == 0) {
            FusedMulsCastImpl<ActualT, ActualU>(
                (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualU*)src1.GetPhyAddr(),
                (__ubuf__ ActualU*)src0.GetPhyAddr(), count);
        } else {
            FusedMulsCastImpl<ActualT, ActualU>(
                (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualU*)src0.GetPhyAddr(),
                (__ubuf__ ActualU*)src1.GetPhyAddr(), count);
        }
    } else if constexpr (TypeUtils::IsLocalTensorType<T4>() && TypeUtils::IsInnerDefaultType<T3>()) {
        ASCENDC_ASSERT((count <= src1.GetSize() && count <= dst.GetSize()), {
            KERNEL_LOG(KERNEL_ERROR, "count is %u, which should not larger than tensor size of dst / src1", count);
        });
        using ActualU = typename T4::PrimType;
        CheckTensorPos<ActualU>(src1, Hardware::UB, "src1", "VECIN/VECCALC/VECOUT", "MulsCast");
        static_assert(Std::is_same<ActualT, typename T2::PrimType>::value);
        static_assert(Std::is_same<ActualU, typename T4::PrimType>::value);
        static_assert(Std::is_same<ActualU, T3>::value);
        FusedMulsCastImpl<ActualT, ActualU>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualU*)src1.GetPhyAddr(), src0, count);
    } else if constexpr (TypeUtils::IsLocalTensorType<T3>() && TypeUtils::IsInnerDefaultType<T4>()) {
        ASCENDC_ASSERT((count <= src0.GetSize() && count <= dst.GetSize()), {
            KERNEL_LOG(KERNEL_ERROR, "count is %u, which should not larger than tensor size of dst / src0", count);
        });
        using ActualU = typename T3::PrimType;
        CheckTensorPos<ActualU>(src0, Hardware::UB, "src0", "VECIN/VECCALC/VECOUT", "MulsCast");
        static_assert(Std::is_same<ActualT, typename T2::PrimType>::value);
        static_assert(Std::is_same<ActualU, typename T3::PrimType>::value);
        static_assert(Std::is_same<ActualU, T4>::value);
        FusedMulsCastImpl<ActualT, ActualU>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualU*)src0.GetPhyAddr(), src1, count);
    }
#else
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "MulsCast not supported on current device."); });
#endif
}

// FusedMulsCast has been updated, please use MulsCast instead.
template <typename T0, typename T1, const BinaryConfig& config, typename T2, typename T3, typename T4>
__aicore__ inline void FusedMulsCast(const T2& dst, const T3& src0, const T4& src1, const uint32_t count)
{
    using ActualT = typename T2::PrimType;
    CheckTensorPos<ActualT>(dst, Hardware::UB, "dst", "VECIN/VECCALC/VECOUT", "FusedMulsCast");
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113)
    static_assert(!TypeUtils::IsInnerDefaultType<T3, T4>(), "One of src0 and src1 should be Tensor");
    static_assert(TypeUtils::IsLocalTensorType<T2>());
    static_assert(SupportType<typename T2::PrimType, half>());
    if constexpr (Std::is_same<T3, T4>::value) {
        ASCENDC_ASSERT((count <= src0.GetSize() && count <= src1.GetSize() && count <= dst.GetSize()), {
            KERNEL_LOG(
                KERNEL_ERROR, "count is %u, which should not larger than tensor size of dst / src0 / src1", count);
        });
        using ActualU = typename T3::PrimType;
        CheckTensorPos<ActualU>(src0, Hardware::UB, "src0", "VECIN/VECCALC/VECOUT", "FusedMulsCast");
        CheckTensorPos<ActualU>(src1, Hardware::UB, "src1", "VECIN/VECCALC/VECOUT", "FusedMulsCast");
        static_assert(Std::is_same<T3, T4>::value);
        static_assert(Std::is_same<ActualT, typename T2::PrimType>::value);
        static_assert(
            (config.scalarTensorIndex == 0 || config.scalarTensorIndex == 1), "scalarTensorIndex out of range");
        if constexpr (config.scalarTensorIndex == 0) {
            FusedMulsCastImpl<ActualT, ActualU>(
                (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualU*)src1.GetPhyAddr(),
                (__ubuf__ ActualU*)src0.GetPhyAddr(), count);
        } else {
            FusedMulsCastImpl<ActualT, ActualU>(
                (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualU*)src0.GetPhyAddr(),
                (__ubuf__ ActualU*)src1.GetPhyAddr(), count);
        }
    } else if constexpr (TypeUtils::IsLocalTensorType<T4>() && TypeUtils::IsInnerDefaultType<T3>()) {
        ASCENDC_ASSERT((count <= src1.GetSize() && count <= dst.GetSize()), {
            KERNEL_LOG(KERNEL_ERROR, "count is %u, which should not larger than tensor size of dst / src1", count);
        });
        using ActualU = typename T4::PrimType;
        CheckTensorPos<ActualU>(src1, Hardware::UB, "src1", "VECIN/VECCALC/VECOUT", "FusedMulsCast");
        static_assert(Std::is_same<ActualT, typename T2::PrimType>::value);
        static_assert(Std::is_same<ActualU, typename T4::PrimType>::value);
        static_assert(Std::is_same<ActualU, T3>::value);
        FusedMulsCastImpl<ActualT, ActualU>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualU*)src1.GetPhyAddr(), src0, count);
    } else if constexpr (TypeUtils::IsLocalTensorType<T3>() && TypeUtils::IsInnerDefaultType<T4>()) {
        ASCENDC_ASSERT((count <= src0.GetSize() && count <= dst.GetSize()), {
            KERNEL_LOG(KERNEL_ERROR, "count is %u, which should not larger than tensor size of dst / src0", count);
        });
        using ActualU = typename T3::PrimType;
        CheckTensorPos<ActualU>(src0, Hardware::UB, "src0", "VECIN/VECCALC/VECOUT", "FusedMulsCast");
        static_assert(Std::is_same<ActualT, typename T2::PrimType>::value);
        static_assert(Std::is_same<ActualU, typename T3::PrimType>::value);
        static_assert(Std::is_same<ActualU, T4>::value);
        FusedMulsCastImpl<ActualT, ActualU>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ ActualU*)src0.GetPhyAddr(), src1, count);
    }
#else
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "FusedMulsCast not supported on current device."); });
#endif
}
#endif
} // namespace AscendC
#pragma end_pipe
#endif // ASCENDC_MODULE_OPERATOR_VEC_BINARY_SCALAR_INTERFACE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_SCALAR_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_SCALAR_INTF_IMPL_H__
#endif
