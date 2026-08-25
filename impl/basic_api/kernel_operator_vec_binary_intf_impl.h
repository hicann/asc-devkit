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
 * \file kernel_operator_vec_binary_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_vec_binary_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_vec_binary_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_INTF_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_BINARY_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_BINARY_INTERFACE_IMPL_H
#include "../../include/basic_api/kernel_tensor.h"
#include "kernel_check.h"
#include "../../include/basic_api/kernel_struct_binary.h"
#include "kernel_npu_debug.h"
#include "mstx_local_tensor_info.h"

#if __NPU_ARCH__ == 1001
#include "dav_c100/kernel_operator_vec_binary_impl.h"
#elif __NPU_ARCH__ == 2002
#include "dav_m200/kernel_operator_vec_binary_impl.h"
#elif __NPU_ARCH__ == 2201
#include "dav_c220/kernel_operator_vec_binary_impl.h"
#elif __NPU_ARCH__ == 3002
#include "dav_m300/kernel_operator_vec_binary_impl.h"
#elif __NPU_ARCH__ == 3102
#include "dav_m310/kernel_operator_vec_binary_impl.h"
#elif __NPU_ARCH__ == 3510
#include "dav_3510/kernel_operator_vec_binary_impl.h"
#elif (__NPU_ARCH__ == 5102)
#include "dav_m510/kernel_operator_vec_binary_impl.h"
#elif __NPU_ARCH__ == 3003
#include "dav_l300/kernel_operator_vec_binary_impl.h"
#elif __NPU_ARCH__ == 3113
#include "dav_l311/kernel_operator_vec_binary_impl.h"
#endif
#pragma begin_pipe(V)
namespace AscendC {
/* **************************************************************************************************
 * Add                                             *
 * ************************************************************************************************* */
/*
 * @ingroup Add Level 0
 * @brief dst = src0 + src1
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.src0BlkStride src0 block stride
 * @param [in] intriParams.src1BlkStride src1 block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src0 repeat stride
 * @param [in] intriParams.src1RepStride src1 repeat stride
 */
template <typename T, bool isSetMask>
__aicore__ inline void Add(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Add", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Add");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinary(dst, src0, src1, mask, repeatTime, repeatParams, "Add")) {
        ASCENDC_REPORT_CHECK_ERROR("Add", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, mask[0], mask[1], repeatTime, repeatParams, isSetMask, "Add");
#endif
    AddImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask>
__aicore__ inline void Add(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Add", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Add");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinary(dst, src0, src1, mask, repeatTime, repeatParams, "Add")) {
        ASCENDC_REPORT_CHECK_ERROR("Add", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, mask, repeatTime, repeatParams, isSetMask, "Add");
#endif
    AddImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

/*
 * @ingroup Add Level 2
 * @brief dst = src0 + src1
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T>
__aicore__ inline void Add(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Add", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckCalcount(count, "count", "Add");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinary(dst, src0, src1, count, "Add")) {
        ASCENDC_REPORT_CHECK_ERROR("Add", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, "Add", count);
#endif
    AddImpl(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), count);
}

/* **************************************************************************************************
 * Sub                                             *
 * ************************************************************************************************* */
/*
 * @ingroup Sub Level 0
 * @brief dst = src0 - src1
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.src0BlkStride src0 block stride
 * @param [in] intriParams.src1BlkStride src1 block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src0 repeat stride
 * @param [in] intriParams.src1RepStride src1 repeat stride
 */
template <typename T, bool isSetMask>
__aicore__ inline void Sub(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Sub", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Sub");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinary(dst, src0, src1, mask, repeatTime, repeatParams, "Sub")) {
        ASCENDC_REPORT_CHECK_ERROR("Sub", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, mask[0], mask[1], repeatTime, repeatParams, isSetMask, "Sub");
#endif
    SubImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask>
__aicore__ inline void Sub(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Sub", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Sub");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinary(dst, src0, src1, mask, repeatTime, repeatParams, "Sub")) {
        ASCENDC_REPORT_CHECK_ERROR("Sub", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, mask, repeatTime, repeatParams, isSetMask, "Sub");
#endif
    SubImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

/*
 * @ingroup Sub Level 2
 * @brief dst = src0 - src1
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T>
__aicore__ inline void Sub(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Sub", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckCalcount(count, "count", "Sub");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinary(dst, src0, src1, count, "Sub")) {
        ASCENDC_REPORT_CHECK_ERROR("Sub", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, "Sub", count);
#endif
    SubImpl(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), count);
}

/* **************************************************************************************************
 * Mul                                             *
 * ************************************************************************************************* */
/*
 * @ingroup Mul Level 0
 * @brief dst = src0 * src1
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.src0BlkStride src0 block stride
 * @param [in] intriParams.src1BlkStride src1 block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src0 repeat stride
 * @param [in] intriParams.src1RepStride src1 repeat stride
 */
template <typename T, bool isSetMask>
__aicore__ inline void Mul(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Mul", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Mul");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinary(dst, src0, src1, mask, repeatTime, repeatParams, "Mul")) {
        ASCENDC_REPORT_CHECK_ERROR("Mul", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, mask[0], mask[1], repeatTime, repeatParams, isSetMask, "Mul");
#endif
    MulImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask>
__aicore__ inline void Mul(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Mul", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Mul");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinary(dst, src0, src1, mask, repeatTime, repeatParams, "Mul")) {
        ASCENDC_REPORT_CHECK_ERROR("Mul", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, mask, repeatTime, repeatParams, isSetMask, "Mul");
#endif
    MulImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

/*
 * @ingroup Mul Level 2
 * @brief dst = src0 * src1
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T>
__aicore__ inline void Mul(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Mul", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckCalcount(count, "count", "Mul");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinary(dst, src0, src1, count, "Mul")) {
        ASCENDC_REPORT_CHECK_ERROR("Mul", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, "Mul", count);
#endif
    MulImpl(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), count);
}

/* **************************************************************************************************
 * Div                                             *
 * ************************************************************************************************* */
/*
 * @ingroup Div Level 0
 * @brief dst = src0 / src1
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.src0BlkStride src0 block stride
 * @param [in] intriParams.src1BlkStride src1 block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src0 repeat stride
 * @param [in] intriParams.src1RepStride src1 repeat stride
 */
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
template <typename T, bool isSetMask, const DivConfig& config>
__aicore__ inline void Div(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinary(dst, src0, src1, mask, repeatTime, repeatParams, "Div")) {
        ASCENDC_REPORT_CHECK_ERROR("Div", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, mask[0], mask[1], repeatTime, repeatParams, isSetMask, "Div");
#endif
    DivImpl<PrimType, isSetMask, config>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask, const DivConfig& config>
__aicore__ inline void Div(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinary(dst, src0, src1, mask, repeatTime, repeatParams, "Div")) {
        ASCENDC_REPORT_CHECK_ERROR("Div", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, mask, repeatTime, repeatParams, isSetMask, "Div");
#endif
    DivImpl<PrimType, isSetMask, config>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}
#else
template <typename T, bool isSetMask>
__aicore__ inline void Div(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Div", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Div");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinary(dst, src0, src1, mask, repeatTime, repeatParams, "Div")) {
        ASCENDC_REPORT_CHECK_ERROR("Div", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, mask[0], mask[1], repeatTime, repeatParams, isSetMask, "Div");
#endif
    DivImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask>
__aicore__ inline void Div(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Div", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Div");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinary(dst, src0, src1, mask, repeatTime, repeatParams, "Div")) {
        ASCENDC_REPORT_CHECK_ERROR("Div", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, mask, repeatTime, repeatParams, isSetMask, "Div");
#endif
    DivImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}
#endif

/*
 * @ingroup Div Level 2
 * @brief dst = src0 / src1
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
template <typename T, const DivConfig& config>
__aicore__ inline void Div(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinary(dst, src0, src1, count, "Div")) {
        ASCENDC_REPORT_CHECK_ERROR("Div", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, "Div", count);
#endif
    DivImpl<PrimType, config>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), count);
}
#else
template <typename T>
__aicore__ inline void Div(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Div", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckCalcount(count, "count", "Div");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinary(dst, src0, src1, count, "Div")) {
        ASCENDC_REPORT_CHECK_ERROR("Div", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, "Div", count);
#endif
    DivImpl(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), count);
}
#endif

/* **************************************************************************************************
 * MulAddDst                                             *
 * ************************************************************************************************* */
/*
 * @ingroup MulAddDst Level 0
 * @brief dst = src0 * src1 + dst
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.src0BlkStride src0 block stride
 * @param [in] intriParams.src1BlkStride src1 block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src0 repeat stride
 * @param [in] intriParams.src1RepStride src1 repeat stride
 */
template <typename T, typename U, bool isSetMask>
__aicore__ inline void MulAddDst(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, const uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimDstType = PrimT<T>;
    using PrimSrcType = PrimT<U>;
    using MaskCheckType =
        typename Conditional<(sizeof(PrimDstType) >= sizeof(PrimSrcType)), PrimDstType, PrimSrcType>::type;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("MulAddDst", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<MaskCheckType, isSetMask>(mask, repeatTime, "MulAddDst");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinaryDiffType(dst, src0, src1, mask, repeatTime, repeatParams, "MulAddDst")) {
        ASCENDC_REPORT_CHECK_ERROR("MulAddDst", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(
        dst, src0, src1, mask[0], mask[1], repeatTime, repeatParams, isSetMask, "MulAddDst");
#endif
    MulAddDstImpl<PrimDstType, PrimSrcType, isSetMask>(
        (__ubuf__ PrimDstType*)dst.GetPhyAddr(), (__ubuf__ PrimSrcType*)src0.GetPhyAddr(),
        (__ubuf__ PrimSrcType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

template <typename T, typename U, bool isSetMask>
__aicore__ inline void MulAddDst(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimDstType = PrimT<T>;
    using PrimSrcType = PrimT<U>;
    using MaskCheckType =
        typename Conditional<(sizeof(PrimDstType) >= sizeof(PrimSrcType)), PrimDstType, PrimSrcType>::type;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("MulAddDst", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<MaskCheckType, isSetMask>(mask, repeatTime, "MulAddDst");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinaryDiffType(dst, src0, src1, mask, repeatTime, repeatParams, "MulAddDst")) {
        ASCENDC_REPORT_CHECK_ERROR("MulAddDst", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, mask, repeatTime, repeatParams, isSetMask, "MulAddDst");
#endif
    MulAddDstImpl<PrimDstType, PrimSrcType, isSetMask>(
        (__ubuf__ PrimDstType*)dst.GetPhyAddr(), (__ubuf__ PrimSrcType*)src0.GetPhyAddr(),
        (__ubuf__ PrimSrcType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

/*
 * @ingroup MulAddDst Level 2
 * @brief dst = src0 * src1 + dst
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, typename U>
__aicore__ inline void MulAddDst(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, const int32_t& count)
{
    using PrimDstType = PrimT<T>;
    using PrimSrcType = PrimT<U>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("MulAddDst", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckCalcount(count, "count", "MulAddDst");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinaryDiffType(dst, src0, src1, count, "MulAddDst")) {
        ASCENDC_REPORT_CHECK_ERROR("MulAddDst", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, "MulAddDst", count);
#endif
    MulAddDstImpl(
        (__ubuf__ PrimDstType*)dst.GetPhyAddr(), (__ubuf__ PrimSrcType*)src0.GetPhyAddr(),
        (__ubuf__ PrimSrcType*)src1.GetPhyAddr(), count);
}

/* **************************************************************************************************
 * Max                                             *
 * ************************************************************************************************* */
/*
 * @ingroup Max Level 0
 * @brief dst = src0 > src1 ? src0 : src1
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.src0BlkStride src0 block stride
 * @param [in] intriParams.src1BlkStride src1 block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src0 repeat stride
 * @param [in] intriParams.src1RepStride src1 repeat stride
 */
template <typename T, bool isSetMask>
__aicore__ inline void Max(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Max", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Max");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinary(dst, src0, src1, mask, repeatTime, repeatParams, "Max")) {
        ASCENDC_REPORT_CHECK_ERROR("Max", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, mask[0], mask[1], repeatTime, repeatParams, isSetMask, "Max");
#endif
    MaxImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask>
__aicore__ inline void Max(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Max", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Max");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinary(dst, src0, src1, mask, repeatTime, repeatParams, "Max")) {
        ASCENDC_REPORT_CHECK_ERROR("Max", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, mask, repeatTime, repeatParams, isSetMask, "Max");
#endif
    MaxImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

/*
 * @ingroup Max Level 2
 * @brief dst = src0 > src1 ? src0 : src1
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T>
__aicore__ inline void Max(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Max", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckCalcount(count, "count", "Max");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinary(dst, src0, src1, count, "Max")) {
        ASCENDC_REPORT_CHECK_ERROR("Max", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, "Max", count);
#endif
    MaxImpl(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), count);
}

/* **************************************************************************************************
 * Min                                             *
 * ************************************************************************************************* */
/*
 * @ingroup Min Level 0
 * @brief dst = src0 > src1 ? src1 : src0
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.src0BlkStride src0 block stride
 * @param [in] intriParams.src1BlkStride src1 block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src0 repeat stride
 * @param [in] intriParams.src1RepStride src1 repeat stride
 */
template <typename T, bool isSetMask>
__aicore__ inline void Min(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Min", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Min");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinary(dst, src0, src1, mask, repeatTime, repeatParams, "Min")) {
        ASCENDC_REPORT_CHECK_ERROR("Min", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, mask[0], mask[1], repeatTime, repeatParams, isSetMask, "Min");
#endif
    MinImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask>
__aicore__ inline void Min(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Min", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Min");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinary(dst, src0, src1, mask, repeatTime, repeatParams, "Min")) {
        ASCENDC_REPORT_CHECK_ERROR("Min", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, mask, repeatTime, repeatParams, isSetMask, "Min");
#endif
    MinImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

/*
 * @ingroup Min Level 2
 * @brief dst = src0 > src1 ? src1 : src0
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T>
__aicore__ inline void Min(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Min", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckCalcount(count, "count", "Min");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinary(dst, src0, src1, count, "Min")) {
        ASCENDC_REPORT_CHECK_ERROR("Min", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, "Min", count);
#endif
    MinImpl(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), count);
}

/* **************************************************************************************************
 * And                                             *
 * ************************************************************************************************* */
/*
 * @ingroup And Level 0
 * @brief dst = src0 & src1
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.src0BlkStride src0 block stride
 * @param [in] intriParams.src1BlkStride src1 block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src0 repeat stride
 * @param [in] intriParams.src1RepStride src1 repeat stride
 */
template <typename T, bool isSetMask>
__aicore__ inline void And(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("And", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "And");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinary(dst, src0, src1, mask, repeatTime, repeatParams, "And")) {
        ASCENDC_REPORT_CHECK_ERROR("And", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, mask[0], mask[1], repeatTime, repeatParams, isSetMask, "And");
#endif
    AndImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask>
__aicore__ inline void And(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("And", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "And");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinary(dst, src0, src1, mask, repeatTime, repeatParams, "And")) {
        ASCENDC_REPORT_CHECK_ERROR("And", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, mask, repeatTime, repeatParams, isSetMask, "And");
#endif
    AndImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

/*
 * @ingroup And Level 2
 * @brief dst = src0 & src1
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T>
__aicore__ inline void And(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("And", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckCalcount(count, "count", "And");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinary(dst, src0, src1, count, "And")) {
        ASCENDC_REPORT_CHECK_ERROR("And", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, "And", count);
#endif
    AndImpl(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), count);
}

/* **************************************************************************************************
 * Or                                             *
 * ************************************************************************************************* */
/*
 * @ingroup Or Level 0
 * @brief dst = src0 | src1
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.src0BlkStride src0 block stride
 * @param [in] intriParams.src1BlkStride src1 block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src0 repeat stride
 * @param [in] intriParams.src1RepStride src1 repeat stride
 */
template <typename T, bool isSetMask>
__aicore__ inline void Or(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Or", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Or");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinary(dst, src0, src1, mask, repeatTime, repeatParams, "Or")) {
        ASCENDC_REPORT_CHECK_ERROR("Or", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, mask[0], mask[1], repeatTime, repeatParams, isSetMask, "Or");
#endif
    OrImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask>
__aicore__ inline void Or(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Or", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "Or");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinary(dst, src0, src1, mask, repeatTime, repeatParams, "Or")) {
        ASCENDC_REPORT_CHECK_ERROR("Or", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, mask, repeatTime, repeatParams, isSetMask, "Or");
#endif
    OrImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

/*
 * @ingroup Or Level 2
 * @brief dst = src0 | src1
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T>
__aicore__ inline void Or(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Or", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckCalcount(count, "count", "Or");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinary(dst, src0, src1, count, "Or")) {
        ASCENDC_REPORT_CHECK_ERROR("Or", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, "Or", count);
#endif
    OrImpl(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), count);
}

#if defined(__NPU_ARCH__) && \
    ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
/* **************************************************************************************************
 * ShiftLeft                                             *
 * ************************************************************************************************* */
/*
 * @ingroup ShiftLeft Level 2
 * @brief dst = src0 << src1
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, typename U>
__aicore__ inline void ShiftLeft(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<U>& src1, const int32_t& count)
{
    using Src0PrimType = PrimT<T>;
    using Src1PrimType = PrimT<U>;
    CheckTensorPos<T>(dst, Hardware::UB, "dst", "VECIN/VECCALC/VECOUT", "ShiftLeft");
    CheckTensorPos<T>(src0, Hardware::UB, "src0", "VECIN/VECCALC/VECOUT", "ShiftLeft");
    CheckTensorPos<U>(src1, Hardware::UB, "src1", "VECIN/VECCALC/VECOUT", "ShiftLeft");
    ASCENDC_ASSERT((count <= src0.GetSize() && count <= src1.GetSize() && count <= dst.GetSize()), {
        KERNEL_LOG(KERNEL_ERROR, "count is %u, which should not larger than tensor size of dst / src0 / src1", count);
    });
    ShiftLeftImpl<Src0PrimType, Src1PrimType>(
        (__ubuf__ Src0PrimType*)dst.GetPhyAddr(), (__ubuf__ Src0PrimType*)src0.GetPhyAddr(),
        (__ubuf__ Src1PrimType*)src1.GetPhyAddr(), count);
}

/* **************************************************************************************************
 * ShiftRight                                             *
 * ************************************************************************************************* */
/*
 * @ingroup ShiftRight Level 2
 * @brief dst = src0 >> src1
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, typename U>
__aicore__ inline void ShiftRight(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<U>& src1, const int32_t& count)
{
    using Src0PrimType = PrimT<T>;
    using Src1PrimType = PrimT<U>;
    CheckTensorPos<T>(dst, Hardware::UB, "dst", "VECIN/VECCALC/VECOUT", "ShiftRight");
    CheckTensorPos<T>(src0, Hardware::UB, "src0", "VECIN/VECCALC/VECOUT", "ShiftRight");
    CheckTensorPos<U>(src1, Hardware::UB, "src1", "VECIN/VECCALC/VECOUT", "ShiftRight");
    ASCENDC_ASSERT((count <= src0.GetSize() && count <= src1.GetSize() && count <= dst.GetSize()), {
        KERNEL_LOG(KERNEL_ERROR, "count is %u, which should not larger than tensor size of dst / src0 / src1", count);
    });
    ShiftRightImpl<Src0PrimType, Src1PrimType>(
        (__ubuf__ Src0PrimType*)dst.GetPhyAddr(), (__ubuf__ Src0PrimType*)src0.GetPhyAddr(),
        (__ubuf__ Src1PrimType*)src1.GetPhyAddr(), count);
}
#endif

/* **************************************************************************************************
 * AddRelu                                             *
 * ************************************************************************************************* */
/*
 * @ingroup AddRelu Level 0
 * @brief dst = Relu(src0 + src1)
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.src0BlkStride src0 block stride
 * @param [in] intriParams.src1BlkStride src1 block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src0 repeat stride
 * @param [in] intriParams.src1RepStride src1 repeat stride
 */
template <typename T, bool isSetMask>
__aicore__ inline void AddRelu(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if __NPU_ARCH__ == 2201
    if (g_coreType == AIC) {
        return;
    }
#endif
#if defined(ASCENDC_CPU_DEBUG) || defined(ASCENDC_DEBUG)
    CheckVectorTensor("AddRelu", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "AddRelu");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinary(dst, src0, src1, mask, repeatTime, repeatParams, "AddRelu")) {
        ASCENDC_REPORT_CHECK_ERROR("AddRelu", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, mask[0], mask[1], repeatTime, repeatParams, isSetMask, "AddRelu");
#endif
    AddReluImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask>
__aicore__ inline void AddRelu(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if __NPU_ARCH__ == 2201
    if (g_coreType == AIC) {
        return;
    }
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("AddRelu", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "AddRelu");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinary(dst, src0, src1, mask, repeatTime, repeatParams, "AddRelu")) {
        ASCENDC_REPORT_CHECK_ERROR("AddRelu", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, mask, repeatTime, repeatParams, isSetMask, "AddRelu");
#endif
    AddReluImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

template <typename T>
__aicore__ inline void AddRelu(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if __NPU_ARCH__ == 2201
    if (g_coreType == AIC) {
        return;
    }
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("AddRelu", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckCalcount(count, "count", "AddRelu");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinary(dst, src0, src1, count, "AddRelu")) {
        ASCENDC_REPORT_CHECK_ERROR("AddRelu", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, "AddRelu", count);
#endif
    AddReluImpl(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), count);
}

/* **************************************************************************************************
 * AddDeqRelu                                             *
 * ************************************************************************************************* */
template <typename T, typename U, bool isSetMask>
__aicore__ inline void CheckAddDeqReluMaskArrayParams(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("AddDeqRelu", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<PrimT<U>, isSetMask>(mask, repeatTime, "AddDeqRelu");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinaryDiffType(dst, src0, src1, mask, repeatTime, repeatParams, "AddDeqRelu")) {
        ASCENDC_REPORT_CHECK_ERROR("AddDeqRelu", KernelFuncType::MASK_BIT_MODE);
    }
#endif
}

template <typename T, typename U, bool isSetMask>
__aicore__ inline void CheckAddDeqReluMaskValueParams(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("AddDeqRelu", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<PrimT<U>, isSetMask>(mask, repeatTime, "AddDeqRelu");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinaryDiffType(dst, src0, src1, mask, repeatTime, repeatParams, "AddDeqRelu")) {
        ASCENDC_REPORT_CHECK_ERROR("AddDeqRelu", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
}

/*
 * @ingroup AddDeqRelu Level 0
 * @brief dst = DeqRelu(src0 + src1)
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.src0BlkStride src0 block stride
 * @param [in] intriParams.src1BlkStride src1 block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src0 repeat stride
 * @param [in] intriParams.src1RepStride src1 repeat stride
 */
template <bool isSetMask>
__aicore__ inline void AddDeqRelu(
    const LocalTensor<half>& dst, const LocalTensor<int32_t>& src0, const LocalTensor<int32_t>& src1, uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    CheckAddDeqReluMaskArrayParams<half, int32_t, isSetMask>(dst, src0, src1, mask, repeatTime, repeatParams);
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryAddReqReluInfo(
        dst, src0, src1, mask[0], mask[1], repeatTime, repeatParams, isSetMask, "AddDeqRelu");
#endif
    AddDeqReluImpl<isSetMask>(
        (__ubuf__ half*)dst.GetPhyAddr(), (__ubuf__ int32_t*)src0.GetPhyAddr(), (__ubuf__ int32_t*)src1.GetPhyAddr(),
        mask, repeatTime, repeatParams);
}

template <typename T, typename U, bool isSetMask>
__aicore__ inline void AddDeqRelu(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimDstType = PrimT<T>;
    using PrimSrcType = PrimT<U>;
    static_assert(
        (Std::is_same<PrimDstType, half>::value && Std::is_same<PrimSrcType, int32_t>::value) &&
        "Failed to check dtype in AddDeqRelu, current api support dtype combination is src: int32_t, dst: half.");
    CheckAddDeqReluMaskArrayParams<T, U, isSetMask>(dst, src0, src1, mask, repeatTime, repeatParams);
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryAddReqReluInfo(
        dst, src0, src1, mask[0], mask[1], repeatTime, repeatParams, isSetMask, "AddDeqRelu");
#endif
    AddDeqReluImpl<isSetMask>(
        (__ubuf__ PrimDstType*)dst.GetPhyAddr(), (__ubuf__ PrimSrcType*)src0.GetPhyAddr(),
        (__ubuf__ PrimSrcType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

template <bool isSetMask>
__aicore__ inline void AddDeqRelu(
    const LocalTensor<half>& dst, const LocalTensor<int32_t>& src0, const LocalTensor<int32_t>& src1, uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    CheckAddDeqReluMaskValueParams<half, int32_t, isSetMask>(dst, src0, src1, mask, repeatTime, repeatParams);
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryAddReqReluInfo(
        dst, src0, src1, mask, repeatTime, repeatParams, isSetMask, "AddDeqRelu");
#endif
    AddDeqReluImpl<isSetMask>(
        (__ubuf__ half*)dst.GetPhyAddr(), (__ubuf__ int32_t*)src0.GetPhyAddr(), (__ubuf__ int32_t*)src1.GetPhyAddr(),
        mask, repeatTime, repeatParams);
}

template <typename T, typename U, bool isSetMask>
__aicore__ inline void AddDeqRelu(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimDstType = PrimT<T>;
    using PrimSrcType = PrimT<U>;
    static_assert(
        (Std::is_same<PrimDstType, half>::value && Std::is_same<PrimSrcType, int32_t>::value) &&
        "Failed to check dtype in AddDeqRelu, current api support dtype combination is src: int32_t, dst: half.");
    CheckAddDeqReluMaskValueParams<T, U, isSetMask>(dst, src0, src1, mask, repeatTime, repeatParams);
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryAddReqReluInfo(
        dst, src0, src1, mask, repeatTime, repeatParams, isSetMask, "AddDeqRelu");
#endif
    AddDeqReluImpl<isSetMask>(
        (__ubuf__ PrimDstType*)dst.GetPhyAddr(), (__ubuf__ PrimSrcType*)src0.GetPhyAddr(),
        (__ubuf__ PrimSrcType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

/*
 * @ingroup AddDeqRelu Level 2
 * @brief dst = DeqRelu(src0 + src1)
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
__aicore__ inline void AddDeqRelu(
    const LocalTensor<half>& dst, const LocalTensor<int32_t>& src0, const LocalTensor<int32_t>& src1,
    const int32_t& count)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("AddDeqRelu", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckCalcount(count, "count", "AddDeqRelu");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinaryDiffType(dst, src0, src1, count, "AddDeqRelu")) {
        ASCENDC_REPORT_CHECK_ERROR("AddDeqRelu", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryAddReqReluInfo(dst, src0, src1, "AddDeqRelu", count);
#endif
    AddDeqReluImpl(
        (__ubuf__ half*)dst.GetPhyAddr(), (__ubuf__ int32_t*)src0.GetPhyAddr(), (__ubuf__ int32_t*)src1.GetPhyAddr(),
        count);
}

template <typename T, typename U>
__aicore__ inline void AddDeqRelu(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, const int32_t& count)
{
    using PrimDstType = PrimT<T>;
    using PrimSrcType = PrimT<U>;
    static_assert(
        (Std::is_same<PrimDstType, half>::value && Std::is_same<PrimSrcType, int32_t>::value) &&
        "Failed to check dtype in AddDeqRelu, current api support dtype combination is src: int32_t, dst: half.");
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckCalcount(count, "count", "AddDeqRelu");
    CheckVectorTensor("AddDeqRelu", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinaryDiffType(dst, src0, src1, count, "AddDeqRelu")) {
        ASCENDC_REPORT_CHECK_ERROR("AddDeqRelu", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryAddReqReluInfo(dst, src0, src1, "AddDeqRelu", count);
#endif
    AddDeqReluImpl(
        (__ubuf__ PrimDstType*)dst.GetPhyAddr(), (__ubuf__ PrimSrcType*)src0.GetPhyAddr(),
        (__ubuf__ PrimSrcType*)src1.GetPhyAddr(), count);
}

/* **************************************************************************************************
 * FusedMulAdd                                             *
 * ************************************************************************************************* */
/*
 * @ingroup FusedMulAdd Level 0
 * @brief dst = src0 * dst + src1
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.src0BlkStride src0 block stride
 * @param [in] intriParams.src1BlkStride src1 block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src0 repeat stride
 * @param [in] intriParams.src1RepStride src1 repeat stride
 */
template <typename T, bool isSetMask>
__aicore__ inline void FusedMulAdd(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if __NPU_ARCH__ == 2201
    if (g_coreType == AIC) {
        return;
    }
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinary(dst, src0, src1, mask, repeatTime, repeatParams, "FusedMulAdd")) {
        ASCENDC_REPORT_CHECK_ERROR("FusedMulAdd", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(
        dst, src0, src1, mask[0], mask[1], repeatTime, repeatParams, isSetMask, "FusedMulAdd");
#endif
    FusedMulAddImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask>
__aicore__ inline void FusedMulAdd(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if __NPU_ARCH__ == 2201
    if (g_coreType == AIC) {
        return;
    }
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinary(dst, src0, src1, mask, repeatTime, repeatParams, "FusedMulAdd")) {
        ASCENDC_REPORT_CHECK_ERROR("FusedMulAdd", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, mask, repeatTime, repeatParams, isSetMask, "FusedMulAdd");
#endif
    FusedMulAddImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

/*
 * @ingroup FusedMulAdd Level 2
 * @brief dst = src0 * dst + src1
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T>
__aicore__ inline void FusedMulAdd(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if __NPU_ARCH__ == 2201
    if (g_coreType == AIC) {
        return;
    }
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinary(dst, src0, src1, count, "FusedMulAdd")) {
        ASCENDC_REPORT_CHECK_ERROR("FusedMulAdd", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, "FusedMulAdd", count);
#endif
    FusedMulAddImpl(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), count);
}

/* **************************************************************************************************
 * MulAddRelu                                             *
 * ************************************************************************************************* */
/*
 * @ingroup MulAddRelu Level 0
 * @brief dst = src0 * dst + src1
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.src0BlkStride src0 block stride
 * @param [in] intriParams.src1BlkStride src1 block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src0 repeat stride
 * @param [in] intriParams.src1RepStride src1 repeat stride
 */
template <typename T, bool isSetMask>
__aicore__ inline void MulAddRelu(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if __NPU_ARCH__ == 2201
    if (g_coreType == AIC) {
        return;
    }
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("MulAddRelu", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "MulAddRelu");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinary(dst, src0, src1, mask, repeatTime, repeatParams, "MulAddRelu")) {
        ASCENDC_REPORT_CHECK_ERROR("MulAddRelu", KernelFuncType::MASK_BIT_MODE);
    }
#endif
    FusedMulAddReluImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask>
__aicore__ inline void MulAddRelu(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if __NPU_ARCH__ == 2201
    if (g_coreType == AIC) {
        return;
    }
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("MulAddRelu", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "MulAddRelu");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinary(dst, src0, src1, mask, repeatTime, repeatParams, "MulAddRelu")) {
        ASCENDC_REPORT_CHECK_ERROR("MulAddRelu", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
    FusedMulAddReluImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

// FusedMulAddRelu has been updated, please use MulAddRelu instead.
template <typename T, bool isSetMask>
__aicore__ inline void FusedMulAddRelu(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if __NPU_ARCH__ == 2201
    if (g_coreType == AIC) {
        return;
    }
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("FusedMulAddRelu", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "FusedMulAddRelu");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinary(dst, src0, src1, mask, repeatTime, repeatParams, "FusedMulAddRelu")) {
        ASCENDC_REPORT_CHECK_ERROR("FusedMulAddRelu", KernelFuncType::MASK_BIT_MODE);
    }
#endif
    FusedMulAddReluImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

// FusedMulAddRelu has been updated, please use MulAddRelu instead.
template <typename T, bool isSetMask>
__aicore__ inline void FusedMulAddRelu(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if __NPU_ARCH__ == 2201
    if (g_coreType == AIC) {
        return;
    }
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("FusedMulAddRelu", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "FusedMulAddRelu");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinary(dst, src0, src1, mask, repeatTime, repeatParams, "FusedMulAddRelu")) {
        ASCENDC_REPORT_CHECK_ERROR("FusedMulAddRelu", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
    FusedMulAddReluImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

/*
 * @ingroup MulAddRelu Level 2
 * @brief dst = src0 * dst + src1
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T>
__aicore__ inline void MulAddRelu(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if __NPU_ARCH__ == 2201
    if (g_coreType == AIC) {
        return;
    }
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("MulAddRelu", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckCalcount(count, "count", "MulAddRelu");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinary(dst, src0, src1, count, "MulAddRelu")) {
        ASCENDC_REPORT_CHECK_ERROR("MulAddRelu", KernelFuncType::CALCOUNT_MODE);
    }
#endif
    FusedMulAddReluImpl(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), count);
}

template <typename T>
__aicore__ inline void FusedMulAddRelu(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if __NPU_ARCH__ == 2201
    if (g_coreType == AIC) {
        return;
    }
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("FusedMulAddRelu", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckCalcount(count, "count", "FusedMulAddRelu");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinary(dst, src0, src1, count, "FusedMulAddRelu")) {
        ASCENDC_REPORT_CHECK_ERROR("FusedMulAddRelu", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, "FusedMulAddRelu", count);
#endif
    FusedMulAddReluImpl(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), count);
}

/* **************************************************************************************************
 * SubRelu                                             *
 * ************************************************************************************************* */
/*
 * @ingroup SubRelu Level 0
 * @brief dst = Relu(src0 - src1)
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.src0BlkStride src0 block stride
 * @param [in] intriParams.src1BlkStride src1 block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src0 repeat stride
 * @param [in] intriParams.src1RepStride src1 repeat stride
 */
template <typename T, bool isSetMask>
__aicore__ inline void SubRelu(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if __NPU_ARCH__ == 2201
    if (g_coreType == AIC) {
        return;
    }
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("SubRelu", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "SubRelu");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinary(dst, src0, src1, mask, repeatTime, repeatParams, "SubRelu")) {
        ASCENDC_REPORT_CHECK_ERROR("SubRelu", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, mask[0], mask[1], repeatTime, repeatParams, isSetMask, "SubRelu");
#endif
    SubReluImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask>
__aicore__ inline void SubRelu(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if __NPU_ARCH__ == 2201
    if (g_coreType == AIC) {
        return;
    }
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("SubRelu", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<PrimType, isSetMask>(mask, repeatTime, "SubRelu");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinary(dst, src0, src1, mask, repeatTime, repeatParams, "SubRelu")) {
        ASCENDC_REPORT_CHECK_ERROR("SubRelu", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, mask, repeatTime, repeatParams, isSetMask, "SubRelu");
#endif
    SubReluImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

template <typename T>
__aicore__ inline void SubRelu(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
{
    using PrimType = PrimT<T>;
#if __NPU_ARCH__ == 2201
    if (g_coreType == AIC) {
        return;
    }
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("SubRelu", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckCalcount(count, "count", "SubRelu");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinary(dst, src0, src1, count, "SubRelu")) {
        ASCENDC_REPORT_CHECK_ERROR("SubRelu", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, "SubRelu", count);
#endif
    SubReluImpl(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), count);
}

#if defined(__NPU_ARCH__) && \
    ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
/* **************************************************************************************************
 * Prelu                                             *
 * ************************************************************************************************* */
/*
 * @ingroup Prelu Level 2
 * @brief dst = (src0 >= 0) ? src0 : src0 * src1
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T>
__aicore__ inline void Prelu(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const uint32_t count)
{
    using PrimType = PrimT<T>;
    CheckTensorPos<T>(dst, Hardware::UB, "dst", "VECIN/VECCALC/VECOUT", "Prelu");
    CheckTensorPos<T>(src0, Hardware::UB, "src0", "VECIN/VECCALC/VECOUT", "Prelu");
    CheckTensorPos<T>(src1, Hardware::UB, "src1", "VECIN/VECCALC/VECOUT", "Prelu");
    ASCENDC_ASSERT((count <= src0.GetSize() && count <= src1.GetSize() && count <= dst.GetSize()), {
        KERNEL_LOG(KERNEL_ERROR, "count is %u, which should not larger than tensor size of dst / src0 / src1", count);
    });
    PreluImpl(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), count);
}

/* **************************************************************************************************
 * Mull                                             *
 * ************************************************************************************************* */
/*
 * @ingroup Mull Level 2
 * @brief Multiply input data src0 and src1 by element based on the mask, write the result to
        dst0, and write the overflow part to dst1.
 * @param [out] dst0 output LocalTensor
 * @param [out] dst1 output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T>
__aicore__ inline void Mull(
    const LocalTensor<T>& dst0, const LocalTensor<T>& dst1, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
    const uint32_t count)
{
    using PrimType = PrimT<T>;
    CheckTensorPos<T>(dst0, Hardware::UB, "dst0", "VECIN/VECCALC/VECOUT", "Mull");
    CheckTensorPos<T>(dst1, Hardware::UB, "dst1", "VECIN/VECCALC/VECOUT", "Mull");
    CheckTensorPos<T>(src0, Hardware::UB, "src0", "VECIN/VECCALC/VECOUT", "Mull");
    CheckTensorPos<T>(src1, Hardware::UB, "src1", "VECIN/VECCALC/VECOUT", "Mull");
    ASCENDC_ASSERT(
        (count <= src0.GetSize() && count <= src1.GetSize() && count <= dst0.GetSize() && count <= dst1.GetSize()), {
            KERNEL_LOG(
                KERNEL_ERROR, "count is %u, which should not larger than tensor size of dst0 / dst1 / src0 / src1",
                count);
        });
    MullImpl(
        (__ubuf__ PrimType*)dst0.GetPhyAddr(), (__ubuf__ PrimType*)dst1.GetPhyAddr(),
        (__ubuf__ PrimType*)src0.GetPhyAddr(), (__ubuf__ PrimType*)src1.GetPhyAddr(), count);
}

/* **************************************************************************************************
 * AbsSub                                             *
 * ************************************************************************************************* */
/*
 * @ingroup AbsSub Level 2
 * @brief dst = abs(src0 - src1)
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T>
__aicore__ inline void AbsSub(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const uint32_t count)
{
    using PrimType = PrimT<T>;
    CheckTensorPos<T>(dst, Hardware::UB, "dst", "VECIN/VECCALC/VECOUT", "AbsSub");
    CheckTensorPos<T>(src0, Hardware::UB, "src0", "VECIN/VECCALC/VECOUT", "AbsSub");
    CheckTensorPos<T>(src1, Hardware::UB, "src1", "VECIN/VECCALC/VECOUT", "AbsSub");
    ASCENDC_ASSERT((count <= src0.GetSize() && count <= src1.GetSize() && count <= dst.GetSize()), {
        KERNEL_LOG(KERNEL_ERROR, "count is %u, which should not larger than tensor size of dst / src0 / src1", count);
    });
    FusedAbsSubImpl(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), count);
}

// FusedAbsSub has been updated, please use AbsSub instead.
template <typename T>
__aicore__ inline void FusedAbsSub(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const uint32_t count)
{
    using PrimType = PrimT<T>;
    CheckTensorPos<T>(dst, Hardware::UB, "dst", "VECIN/VECCALC/VECOUT", "FusedAbsSub");
    CheckTensorPos<T>(src0, Hardware::UB, "src0", "VECIN/VECCALC/VECOUT", "FusedAbsSub");
    CheckTensorPos<T>(src1, Hardware::UB, "src1", "VECIN/VECCALC/VECOUT", "FusedAbsSub");
    ASCENDC_ASSERT((count <= src0.GetSize() && count <= src1.GetSize() && count <= dst.GetSize()), {
        KERNEL_LOG(KERNEL_ERROR, "count is %u, which should not larger than tensor size of dst / src0 / src1", count);
    });
    FusedAbsSubImpl(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), count);
}

/* **************************************************************************************************
 * ExpSub                                             *
 * ************************************************************************************************* */
/*
 * @ingroup ExpSub Level 2
 * @brief when T is float : dst = e^(src0 - src1); when T is half : dst = e^(cast_f16_to_f32(src0) -
 * cast_f16_to_f32(src1))
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, typename U>
__aicore__ inline void ExpSub(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, const uint32_t count)
{
    using DstPrimType = PrimT<T>;
    using SrcPrimType = PrimT<U>;
    CheckTensorPos<T>(dst, Hardware::UB, "dst", "VECIN/VECCALC/VECOUT", "ExpSub");
    CheckTensorPos<U>(src0, Hardware::UB, "src0", "VECIN/VECCALC/VECOUT", "ExpSub");
    CheckTensorPos<U>(src1, Hardware::UB, "src1", "VECIN/VECCALC/VECOUT", "ExpSub");
    ASCENDC_ASSERT((count <= src0.GetSize() && count <= src1.GetSize() && count <= dst.GetSize()), {
        KERNEL_LOG(KERNEL_ERROR, "count is %u, which should not larger than tensor size of dst / src0 / src1", count);
    });
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    static_assert(
        SupportType<Tuple<DstPrimType, SrcPrimType>, Tuple<float, half>, Tuple<float, float>>(),
        "Failed to check dtype in "
        "ExpSub, current api support dtype combination is src : half / float, dst: float.");
#else
    static_assert(
        SupportType<Tuple<DstPrimType, SrcPrimType>, Tuple<half, half>, Tuple<float, float>>(),
        "Failed to check dtype in "
        "ExpSub, current api support dtype combination is src and dst both: half / float.");
#endif
    FusedExpSubImpl<DstPrimType, SrcPrimType>(
        (__ubuf__ DstPrimType*)dst.GetPhyAddr(), (__ubuf__ SrcPrimType*)src0.GetPhyAddr(),
        (__ubuf__ SrcPrimType*)src1.GetPhyAddr(), count);
}

// FusedExpSub has been updated, please use ExpSub instead.
template <typename T, typename U>
__aicore__ inline void FusedExpSub(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, const uint32_t count)
{
    using DstPrimType = PrimT<T>;
    using SrcPrimType = PrimT<U>;
    CheckTensorPos<T>(dst, Hardware::UB, "dst", "VECIN/VECCALC/VECOUT", "FusedExpSub");
    CheckTensorPos<U>(src0, Hardware::UB, "src0", "VECIN/VECCALC/VECOUT", "FusedExpSub");
    CheckTensorPos<U>(src1, Hardware::UB, "src1", "VECIN/VECCALC/VECOUT", "FusedExpSub");
    ASCENDC_ASSERT((count <= src0.GetSize() && count <= src1.GetSize() && count <= dst.GetSize()), {
        KERNEL_LOG(KERNEL_ERROR, "count is %u, which should not larger than tensor size of dst / src0 / src1", count);
    });
    FusedExpSubImpl<DstPrimType, SrcPrimType>(
        (__ubuf__ DstPrimType*)dst.GetPhyAddr(), (__ubuf__ SrcPrimType*)src0.GetPhyAddr(),
        (__ubuf__ SrcPrimType*)src1.GetPhyAddr(), count);
}

#endif
} // namespace AscendC
#pragma end_pipe
#endif // ASCENDC_MODULE_OPERATOR_VEC_BINARY_INTERFACE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_INTF_IMPL_H__
#endif
