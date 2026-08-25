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
 * \file kernel_operator_vec_unary_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_vec_unary_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_vec_unary_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_UNARY_INTF_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_UNARY_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_UNARY_INTERFACE_IMPL_H
#include "../../include/basic_api/kernel_tensor.h"
#include "kernel_check.h"
#include "../../include/basic_api/kernel_struct_unary.h"
#include "kernel_npu_debug.h"
#include "mstx_local_tensor_info.h"

#if __NPU_ARCH__ == 1001
#include "dav_c100/kernel_operator_vec_unary_impl.h"
#elif __NPU_ARCH__ == 2002
#include "dav_m200/kernel_operator_vec_unary_impl.h"
#elif __NPU_ARCH__ == 2201
#include "dav_c220/kernel_operator_vec_unary_impl.h"
#elif __NPU_ARCH__ == 3002
#include "dav_m300/kernel_operator_vec_unary_impl.h"
#elif __NPU_ARCH__ == 3102
#include "dav_m310/kernel_operator_vec_unary_impl.h"
#elif __NPU_ARCH__ == 3510
#include "dav_3510/kernel_operator_vec_unary_impl.h"
#elif (__NPU_ARCH__ == 5102)
#include "dav_m510/kernel_operator_vec_unary_impl.h"
#elif __NPU_ARCH__ == 3003
#include "dav_l300/kernel_operator_vec_unary_impl.h"
#elif __NPU_ARCH__ == 3113
#include "dav_l311/kernel_operator_vec_unary_impl.h"
#endif

#pragma begin_pipe(V)
namespace AscendC {
/* **************************************************************************************************
 * Unary                                              *
 * ************************************************************************************************* */

/* **************************************** Relu ****************************************** */
/*
 * @ingroup Relu Level 0
 * @brief dst[i] = (src[i] < 0) ? 0 : src[i]
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.srcBlkStride src block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src repeat stride
 */
template <typename T, bool isSetMask>
__aicore__ inline void Relu(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Relu", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Relu");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), mask, repeatTime, repeatParams, "Relu")) {
        ASCENDC_REPORT_CHECK_ERROR("Relu", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask[0], mask[1], repeatTime, repeatParams, "Relu");
#endif
    ReluImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), mask, repeatTime, repeatParams);
}
template <typename T, bool isSetMask>
__aicore__ inline void Relu(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Relu", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Relu");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), mask, repeatTime, repeatParams, "Relu")) {
        ASCENDC_REPORT_CHECK_ERROR("Relu", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask, repeatTime, repeatParams, "Relu");
#endif
    ReluImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), mask, repeatTime, repeatParams);
}

/*
 * @ingroup Relu Level 2
 * @brief dst[i] = (src[i] < 0) ? 0 : src[i]
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T>
__aicore__ inline void Relu(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Relu", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckValueRange<int32_t>(count, 0, INT32_MAX, "count", "Relu");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), count, "Relu")) {
        ASCENDC_REPORT_CHECK_ERROR("Relu", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, true>(dst, src, "Relu", count);
#endif
    ReluImpl((__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), count);
}

/* **************************************** Exp ****************************************** */
/*
 * @ingroup Exp Level 0
 * @brief dst[i] = exp(src[i])
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.srcBlkStride src block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src repeat stride
 */
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
template <typename T, bool isSetMask, const ExpConfig& config>
__aicore__ inline void Exp(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), mask, repeatTime, repeatParams, "Exp")) {
        ASCENDC_REPORT_CHECK_ERROR("Exp", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask[0], mask[1], repeatTime, repeatParams, "Exp");
#endif
    ExpImpl<PrimType, isSetMask, config>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), mask, repeatTime, repeatParams);
}
template <typename T, bool isSetMask, const ExpConfig& config>
__aicore__ inline void Exp(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), mask, repeatTime, repeatParams, "Exp")) {
        ASCENDC_REPORT_CHECK_ERROR("Exp", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask, repeatTime, repeatParams, "Exp");
#endif
    ExpImpl<PrimType, isSetMask, config>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), mask, repeatTime, repeatParams);
}
#else
template <typename T, bool isSetMask>
__aicore__ inline void Exp(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Exp", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Exp");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), mask, repeatTime, repeatParams, "Exp")) {
        ASCENDC_REPORT_CHECK_ERROR("Exp", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask[0], mask[1], repeatTime, repeatParams, "Exp");
#endif
    ExpImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), mask, repeatTime, repeatParams);
}
template <typename T, bool isSetMask>
__aicore__ inline void Exp(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Exp", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Exp");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), mask, repeatTime, repeatParams, "Exp")) {
        ASCENDC_REPORT_CHECK_ERROR("Exp", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask, repeatTime, repeatParams, "Exp");
#endif
    ExpImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), mask, repeatTime, repeatParams);
}
#endif

/*
 * @ingroup Exp Level 2
 * @brief dst[i] = exp(src[i])
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
template <typename T, const ExpConfig& config>
__aicore__ inline void Exp(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), count, "Exp")) {
        ASCENDC_REPORT_CHECK_ERROR("Exp", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, true>(dst, src, "Exp", count);
#endif
    ExpImpl<PrimType, config>((__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), count);
}
#else
template <typename T>
__aicore__ inline void Exp(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Exp", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckValueRange<int32_t>(count, 0, INT32_MAX, "count", "Exp");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), count, "Exp")) {
        ASCENDC_REPORT_CHECK_ERROR("Exp", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, true>(dst, src, "Exp", count);
#endif
    ExpImpl((__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), count);
}
#endif

/* **************************************** Ln ****************************************** */
/*
 * @ingroup Ln Level 0
 * @brief dst[i] = Ln(src[i])
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.srcBlkStride src block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src repeat stride
 */
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
template <typename T, bool isSetMask, const LnConfig& config>
__aicore__ inline void Ln(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), mask, repeatTime, repeatParams, "Ln")) {
        ASCENDC_REPORT_CHECK_ERROR("Ln", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask[0], mask[1], repeatTime, repeatParams, "Ln");
#endif
    LnImpl<PrimType, isSetMask, config>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), mask, repeatTime, repeatParams);
}
template <typename T, bool isSetMask, const LnConfig& config>
__aicore__ inline void Ln(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), mask, repeatTime, repeatParams, "Ln")) {
        ASCENDC_REPORT_CHECK_ERROR("Ln", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask, repeatTime, repeatParams, "Ln");
#endif
    LnImpl<PrimType, isSetMask, config>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), mask, repeatTime, repeatParams);
}
#else
template <typename T, bool isSetMask>
__aicore__ inline void Ln(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Ln", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Ln");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), mask, repeatTime, repeatParams, "Ln")) {
        ASCENDC_REPORT_CHECK_ERROR("Ln", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask[0], mask[1], repeatTime, repeatParams, "Ln");
#endif
    LnImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), mask, repeatTime, repeatParams);
}
template <typename T, bool isSetMask>
__aicore__ inline void Ln(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Ln", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Ln");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), mask, repeatTime, repeatParams, "Ln")) {
        ASCENDC_REPORT_CHECK_ERROR("Ln", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask, repeatTime, repeatParams, "Ln");
#endif
    LnImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), mask, repeatTime, repeatParams);
}
#endif

/*
 * @ingroup Ln Level 2
 * @brief dst[i] = Ln(src[i])
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
template <typename T, const LnConfig& config>
__aicore__ inline void Ln(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), count, "Ln")) {
        ASCENDC_REPORT_CHECK_ERROR("Ln", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, true>(dst, src, "Ln", count);
#endif
    LnImpl<PrimType, config>((__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), count);
}
#else
template <typename T>
__aicore__ inline void Ln(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Ln", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckValueRange<int32_t>(count, 0, INT32_MAX, "count", "Ln");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), count, "Ln")) {
        ASCENDC_REPORT_CHECK_ERROR("Ln", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, true>(dst, src, "Ln", count);
#endif
    LnImpl((__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), count);
}
#endif

/* **************************************** Abs ****************************************** */
/*
 * @ingroup Abs Level 0
 * @brief dst[i] = abs(src[i])
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.srcBlkStride src block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src repeat stride
 */
template <typename T, bool isSetMask>
__aicore__ inline void Abs(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Abs", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Abs");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), mask, repeatTime, repeatParams, "Abs")) {
        ASCENDC_REPORT_CHECK_ERROR("Abs", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask[0], mask[1], repeatTime, repeatParams, "Abs");
#endif
    AbsImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), mask, repeatTime, repeatParams);
}
template <typename T, bool isSetMask>
__aicore__ inline void Abs(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Abs", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Abs");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), mask, repeatTime, repeatParams, "Abs")) {
        ASCENDC_REPORT_CHECK_ERROR("Abs", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask, repeatTime, repeatParams, "Abs");
#endif
    AbsImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), mask, repeatTime, repeatParams);
}

/*
 * @ingroup Abs Level 2
 * @brief dst[i] = abs(src[i])
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T>
__aicore__ inline void Abs(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Abs", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckValueRange<int32_t>(count, 0, INT32_MAX, "count", "Abs");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), count, "Abs")) {
        ASCENDC_REPORT_CHECK_ERROR("Abs", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, true>(dst, src, "Abs", count);
#endif
    AbsImpl((__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), count);
}

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
/*
 * @ingroup Abs Level 2 for complex32/complex64, only support ascend950
 * @brief dst[i] = abs(src[i])
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, typename U>
__aicore__ inline void Abs(const LocalTensor<T>& dst, const LocalTensor<U>& src, const int32_t& count)
{
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecBinaryScalarDiffType(dst, src, static_cast<PrimT<U>>(0), count, "vabs")) {
        ASSERT(false && "check vabs instr failed");
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, true>(dst, src, "Abs", count);
#endif
    AbsImpl((__ubuf__ PrimT<T>*)dst.GetPhyAddr(), (__ubuf__ PrimT<U>*)src.GetPhyAddr(), count);
}
#endif

/* **************************************** Reciprocal ****************************************** */
/*
 * @ingroup Rec Level 0
 * @brief dst[i] = 1/src[i]
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.srcBlkStride src block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src repeat stride
 */
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
template <typename T, bool isSetMask, const ReciprocalConfig& config>
__aicore__ inline void Reciprocal(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), mask, repeatTime, repeatParams, "Reciprocal")) {
        ASCENDC_REPORT_CHECK_ERROR("Reciprocal", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(
        dst, src, mask[0], mask[1], repeatTime, repeatParams, "Reciprocal");
#endif
    ReciprocalImpl<PrimType, isSetMask, config>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), mask, repeatTime, repeatParams);
}
template <typename T, bool isSetMask, const ReciprocalConfig& config>
__aicore__ inline void Reciprocal(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), mask, repeatTime, repeatParams, "Reciprocal")) {
        ASCENDC_REPORT_CHECK_ERROR("Reciprocal", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask, repeatTime, repeatParams, "Reciprocal");
#endif
    ReciprocalImpl<PrimType, isSetMask, config>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), mask, repeatTime, repeatParams);
}
#else
template <typename T, bool isSetMask>
__aicore__ inline void Reciprocal(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Reciprocal", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Reciprocal");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), mask, repeatTime, repeatParams, "Reciprocal")) {
        ASCENDC_REPORT_CHECK_ERROR("Reciprocal", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(
        dst, src, mask[0], mask[1], repeatTime, repeatParams, "Reciprocal");
#endif
    ReciprocalImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), mask, repeatTime, repeatParams);
}
template <typename T, bool isSetMask>
__aicore__ inline void Reciprocal(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Reciprocal", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Reciprocal");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), mask, repeatTime, repeatParams, "Reciprocal")) {
        ASCENDC_REPORT_CHECK_ERROR("Reciprocal", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask, repeatTime, repeatParams, "Reciprocal");
#endif
    ReciprocalImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), mask, repeatTime, repeatParams);
}
#endif

/*
 * @ingroup Rec Level 2
 * @brief dst[i] = 1/src[i]
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
template <typename T, const ReciprocalConfig& config>
__aicore__ inline void Reciprocal(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), count, "Reciprocal")) {
        ASCENDC_REPORT_CHECK_ERROR("Reciprocal", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, true>(dst, src, "Reciprocal", count);
#endif
    ReciprocalImpl<PrimType, config>((__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), count);
}
#else
template <typename T>
__aicore__ inline void Reciprocal(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Reciprocal", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckValueRange<int32_t>(count, 0, INT32_MAX, "count", "Reciprocal");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), count, "Reciprocal")) {
        ASCENDC_REPORT_CHECK_ERROR("Reciprocal", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, true>(dst, src, "Reciprocal", count);
#endif
    ReciprocalImpl((__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), count);
}
#endif

/* **************************************** Rsqrt ****************************************** */
/*
 * @ingroup Rsqrt Level 0
 * @brief dst[i] = 1/sqrt(src[i])
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.srcBlkStride src block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src repeat stride
 */
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
template <typename T, bool isSetMask, const RsqrtConfig& config>
__aicore__ inline void Rsqrt(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), mask, repeatTime, repeatParams, "Rsqrt")) {
        ASCENDC_REPORT_CHECK_ERROR("Rsqrt", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask[0], mask[1], repeatTime, repeatParams, "Rsqrt");
#endif
    RsqrtImpl<PrimType, isSetMask, config>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), mask, repeatTime, repeatParams);
}
template <typename T, bool isSetMask, const RsqrtConfig& config>
__aicore__ inline void Rsqrt(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), mask, repeatTime, repeatParams, "Rsqrt")) {
        ASCENDC_REPORT_CHECK_ERROR("Rsqrt", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask, repeatTime, repeatParams, "Rsqrt");
#endif
    RsqrtImpl<PrimType, isSetMask, config>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), mask, repeatTime, repeatParams);
}
#else
template <typename T, bool isSetMask>
__aicore__ inline void Rsqrt(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Rsqrt", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Rsqrt");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), mask, repeatTime, repeatParams, "Rsqrt")) {
        ASCENDC_REPORT_CHECK_ERROR("Rsqrt", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask[0], mask[1], repeatTime, repeatParams, "Rsqrt");
#endif
    RsqrtImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), mask, repeatTime, repeatParams);
}
template <typename T, bool isSetMask>
__aicore__ inline void Rsqrt(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Rsqrt", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Rsqrt");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), mask, repeatTime, repeatParams, "Rsqrt")) {
        ASCENDC_REPORT_CHECK_ERROR("Rsqrt", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask, repeatTime, repeatParams, "Rsqrt");
#endif
    RsqrtImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), mask, repeatTime, repeatParams);
}
#endif

/*
 * @ingroup Rsqrt Level 2
 * @brief dst[i] = 1/sqrt(src[i])
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
template <typename T, const RsqrtConfig& config>
__aicore__ inline void Rsqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), count, "Rsqrt")) {
        ASCENDC_REPORT_CHECK_ERROR("Rsqrt", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, true>(dst, src, "Rsqrt", count);
#endif
    RsqrtImpl<PrimType, config>((__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), count);
}
#else
template <typename T>
__aicore__ inline void Rsqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Rsqrt", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckValueRange<int32_t>(count, 0, INT32_MAX, "count", "Rsqrt");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), count, "Rsqrt")) {
        ASCENDC_REPORT_CHECK_ERROR("Rsqrt", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, true>(dst, src, "Rsqrt", count);
#endif
    RsqrtImpl((__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), count);
}
#endif

/* **************************************** Sqrt ****************************************** */
/*
 * @ingroup Sqrt Level 0
 * @brief dst[i] = src[i]^(0.5)
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.srcBlkStride src block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src repeat stride
 */
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
template <typename T, bool isSetMask, const SqrtConfig& config>
__aicore__ inline void Sqrt(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), mask, repeatTime, repeatParams, "Sqrt")) {
        ASCENDC_REPORT_CHECK_ERROR("Sqrt", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask[0], mask[1], repeatTime, repeatParams, "Sqrt");
#endif
    SqrtImpl<PrimType, isSetMask, config>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), mask, repeatTime, repeatParams);
}
template <typename T, bool isSetMask, const SqrtConfig& config>
__aicore__ inline void Sqrt(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), mask, repeatTime, repeatParams, "Sqrt")) {
        ASCENDC_REPORT_CHECK_ERROR("Sqrt", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask, repeatTime, repeatParams, "Sqrt");
#endif
    SqrtImpl<PrimType, isSetMask, config>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), mask, repeatTime, repeatParams);
}
#else
template <typename T, bool isSetMask>
__aicore__ inline void Sqrt(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Sqrt", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Sqrt");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), mask, repeatTime, repeatParams, "Sqrt")) {
        ASCENDC_REPORT_CHECK_ERROR("Sqrt", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask[0], mask[1], repeatTime, repeatParams, "Sqrt");
#endif
    SqrtImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), mask, repeatTime, repeatParams);
}
template <typename T, bool isSetMask>
__aicore__ inline void Sqrt(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Sqrt", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Sqrt");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), mask, repeatTime, repeatParams, "Sqrt")) {
        ASCENDC_REPORT_CHECK_ERROR("Sqrt", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask, repeatTime, repeatParams, "Sqrt");
#endif
    SqrtImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), mask, repeatTime, repeatParams);
}
#endif

/*
 * @ingroup Sqrt Level 2
 * @brief dst[i] = src[i]^(0.5)
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
template <typename T, const SqrtConfig& config>
__aicore__ inline void Sqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), count, "Sqrt")) {
        ASCENDC_REPORT_CHECK_ERROR("Sqrt", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, true>(dst, src, "Sqrt", count);
#endif
    SqrtImpl<PrimType, config>((__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), count);
}
#else
template <typename T>
__aicore__ inline void Sqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Sqrt", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckValueRange<int32_t>(count, 0, INT32_MAX, "count", "Sqrt");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), count, "Sqrt")) {
        ASCENDC_REPORT_CHECK_ERROR("Sqrt", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, true>(dst, src, "Sqrt", count);
#endif
    SqrtImpl((__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), count);
}
#endif

/* **************************************** Not ****************************************** */
/*
 * @ingroup Not Level 0
 * @brief dst[i] = ~src[i]
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.srcBlkStride src block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src repeat stride
 */
template <typename T, bool isSetMask>
__aicore__ inline void Not(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Not", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Not");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), mask, repeatTime, repeatParams, "Not")) {
        ASCENDC_REPORT_CHECK_ERROR("Not", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask[0], mask[1], repeatTime, repeatParams, "Not");
#endif
    NotImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), mask, repeatTime, repeatParams);
}
template <typename T, bool isSetMask>
__aicore__ inline void Not(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Not", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Not");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), mask, repeatTime, repeatParams, "Not")) {
        ASCENDC_REPORT_CHECK_ERROR("Not", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, isSetMask>(dst, src, mask, repeatTime, repeatParams, "Not");
#endif
    NotImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), mask, repeatTime, repeatParams);
}

/*
 * @ingroup Not Level 2
 * @brief dst[i] = ~src[i]
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T>
__aicore__ inline void Not(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Not", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckValueRange<int32_t>(count, 0, INT32_MAX, "count", "Not");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), count, "Not")) {
        ASCENDC_REPORT_CHECK_ERROR("Not", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryInfo<T, T, true>(dst, src, "Not", count);
#endif
    NotImpl((__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), count);
}

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113)
/* **************************************** Neg ****************************************** */
/*
 * @ingroup Neg Level 2
 * @brief dst[i] = -src[i]
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T>
__aicore__ inline void Neg(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
{
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecBinaryScalar(dst, src, static_cast<PrimType>(0), count, "vneg")) {
        ASSERT(false && "check vneg instr failed");
    }
#endif
    NegImpl((__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), count);
}
#endif
} // namespace AscendC
#pragma end_pipe
#endif // ASCENDC_MODULE_OPERATOR_VEC_UNARY_INTERFACE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_UNARY_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_UNARY_INTF_IMPL_H__
#endif
