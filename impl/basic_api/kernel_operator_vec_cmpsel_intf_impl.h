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
 * \file kernel_operator_vec_cmpsel_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_vec_cmpsel_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_vec_cmpsel_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_CMPSEL_INTF_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_CMPSEL_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_CMPSEL_INTERFACE_IMPL_H
#include "../../include/basic_api/kernel_tensor.h"
#include "kernel_check.h"
#include "../../include/basic_api/kernel_struct_binary.h"
#include "../../include/basic_api/kernel_struct_unary.h"
#include "mstx_local_tensor_info.h"
#include "kernel_npu_debug.h"

#if __NPU_ARCH__ == 1001
#include "dav_c100/kernel_operator_vec_cmpsel_impl.h"
#elif __NPU_ARCH__ == 2002
#include "dav_m200/kernel_operator_vec_cmpsel_impl.h"
#elif __NPU_ARCH__ == 2201
#include "dav_c220/kernel_operator_vec_cmpsel_impl.h"
#elif __NPU_ARCH__ == 3002
#include "dav_m300/kernel_operator_vec_cmpsel_impl.h"
#elif __NPU_ARCH__ == 3102
#include "dav_m310/kernel_operator_vec_cmpsel_impl.h"
#elif __NPU_ARCH__ == 3510
#include "dav_3510/kernel_operator_vec_cmp_impl.h"
#include "dav_3510/kernel_operator_vec_sel_impl.h"
#elif (__NPU_ARCH__ == 5102)
#include "dav_m510/kernel_operator_vec_cmpsel_impl.h"
#elif __NPU_ARCH__ == 3003
#include "dav_l300/kernel_operator_vec_cmpsel_impl.h"
#elif __NPU_ARCH__ == 3113
#include "dav_l311/kernel_operator_vec_cmpsel_impl.h"
#endif

#pragma begin_pipe(V)
namespace AscendC {
/* **************************************************************************************************
 * Compare                                           *
 * ************************************************************************************************* */
/*
 * @ingroup Compare Level 0
 * @brief Compare the size of two tensors one by one. If true, the corresponding bit is 1, otherwise it is 0
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] cmpMode compare mode
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
__aicore__ inline void Compare(
    const LocalTensor<U>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, CMPMODE cmpMode,
    const uint64_t mask[], uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Compare", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryCmpInfo(
        dst, src0, src1, mask[0], mask[1], repeatTime, repeatParams, isSetMask, "Compare");
#endif
    using SrcPrimType = PrimT<T>;
    using DstPrimType = PrimT<U>;
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinaryCmp(dst, src0, src1, mask, repeatTime, repeatParams, "Compare")) {
        ASCENDC_REPORT_CHECK_ERROR("Compare", KernelFuncType::MASK_BIT_MODE);
    }
#endif
    VcmpvImpl<SrcPrimType, DstPrimType, isSetMask>(
        (__ubuf__ DstPrimType*)dst.GetPhyAddr(), (__ubuf__ SrcPrimType*)src0.GetPhyAddr(),
        (__ubuf__ SrcPrimType*)src1.GetPhyAddr(), cmpMode, mask, repeatTime, repeatParams);
}

template <typename T, typename U, bool isSetMask>
__aicore__ inline void Compare(
    const LocalTensor<U>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, CMPMODE cmpMode,
    const uint64_t mask, uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Compare", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryCmpInfo(dst, src0, src1, mask, repeatTime, repeatParams, isSetMask, "Compare");
#endif
    using SrcPrimType = PrimT<T>;
    using DstPrimType = PrimT<U>;
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinaryCmp(dst, src0, src1, mask, repeatTime, repeatParams, "Compare")) {
        ASCENDC_REPORT_CHECK_ERROR("Compare", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
    VcmpvImpl<SrcPrimType, DstPrimType, isSetMask>(
        (__ubuf__ DstPrimType*)dst.GetPhyAddr(), (__ubuf__ SrcPrimType*)src0.GetPhyAddr(),
        (__ubuf__ SrcPrimType*)src1.GetPhyAddr(), cmpMode, mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask>
__aicore__ inline void Compare(
    const LocalTensor<T>& src0, const LocalTensor<T>& src1, CMPMODE cmpMode, const uint64_t mask[],
    const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Compare", NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskArray<PrimType, isSetMask>(mask, "Compare");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinaryCmpRgt(src0, src1, mask, repeatParams, "Compare")) {
        ASCENDC_REPORT_CHECK_ERROR("Compare", KernelFuncType::MASK_BIT_MODE);
    }
#endif
    VcmpImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)src0.GetPhyAddr(), (__ubuf__ PrimType*)src1.GetPhyAddr(), cmpMode, mask, repeatParams);
}

template <typename T, bool isSetMask>
__aicore__ inline void Compare(
    const LocalTensor<T>& src0, const LocalTensor<T>& src1, CMPMODE cmpMode, const uint64_t mask,
    const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Compare", NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskValue<PrimType, isSetMask>(mask, "Compare");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinaryCmpRgt(src0, src1, mask, repeatParams, "Compare")) {
        ASCENDC_REPORT_CHECK_ERROR("Compare", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
    VcmpImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)src0.GetPhyAddr(), (__ubuf__ PrimType*)src1.GetPhyAddr(), cmpMode, mask, repeatParams);
}

/*
 * @ingroup Compare Level 2
 * @brief Compare the size of two tensors one by one. If true, the corresponding bit is 1, otherwise it is 0
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] cmpMode compare mode
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, typename U>
__aicore__ inline void Compare(
    const LocalTensor<U>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, CMPMODE cmpMode, uint32_t count)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Compare", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    ASCENDC_DEBUG_ASSERT(
        ((count * sizeof(T)) % ONE_REPEAT_BYTE_SIZE == 0),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR,
            "Failed "
            "to check count in Compare, count * sizeof(T) must be divisible by 256, current count value is %u.\n",
            count));
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryCmpInfo(dst, src0, src1, "Compare", count);
#endif
    using SrcPrimType = PrimT<T>;
    using DstPrimType = PrimT<U>;
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinaryCmp(dst, src0, src1, count, "Compare")) {
        ASCENDC_REPORT_CHECK_ERROR("Compare", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    ASCENDC_ASSERT(((count * sizeof(T)) % ONE_REPEAT_BYTE_SIZE == 0), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "Failed to check count elements size in Compare, current size "
            "is %u, should be an integer multiple of 256.",
            count * sizeof(T));
    });
#endif
    VcmpvImpl(
        (__ubuf__ DstPrimType*)dst.GetPhyAddr(), (__ubuf__ SrcPrimType*)src0.GetPhyAddr(),
        (__ubuf__ SrcPrimType*)src1.GetPhyAddr(), cmpMode, count);
}

template <typename T>
__aicore__ inline void GetCmpMask(const LocalTensor<T>& dst)
{
    using PrimType = PrimT<T>;
#if __NPU_ARCH__ == 2201
    if (g_coreType == AIC) {
        return;
    }
#endif
#if ASCENDC_CPU_DEBUG
    constexpr uint64_t align16B = 16;
    CheckTensorAlign<T>(dst, align16B, "dst", "GetCmpMask");
    CheckTensorPos<T>(dst, Hardware::UB, "dst", "VECIN/VECCALC/VECOUT", "GetCmpMask");
#endif
    GetCmpMaskImpl((__ubuf__ PrimType*)dst.GetPhyAddr());
}

template <typename T>
__aicore__ inline void SetCmpMask(const LocalTensor<T>& src)
{
    using PrimType = PrimT<T>;
#if __NPU_ARCH__ == 2201
    if (g_coreType == AIC) {
        return;
    }
#endif
#if ASCENDC_CPU_DEBUG
    constexpr uint64_t align16B = 16;
    CheckTensorAlign<T>(src, align16B, "src", "SetCmpMask");
    CheckTensorPos<T>(src, Hardware::UB, "src", "VECIN/VECCALC/VECOUT", "SetCmpMask");
#endif
    SetCmpMaskImpl((__ubuf__ PrimType*)src.GetPhyAddr());
}

/* **************************************************************************************************
 * CompareScalar                                           *
 * ************************************************************************************************* */
/*
 * @ingroup Compare Level 0
 * @brief Compare the size of a tensor and a scalar one by one. If true, the corresponding bit is 1, otherwise it is 0
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1Scalar input Scalar
 * @param [in] cmpMode compare mode
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.srcBlkStride src0 block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.srcRepStride src0 repeat stride
 */

/*
 * @ingroup CompareScalar Level 2
 * @brief CompareScalar the size of two tensors one by one. If true, the corresponding bit is 1, otherwise it is 0
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1Scalar input Scalar
 * @param [in] cmpMode compare mode
 * @param [in] count number Number of data involved in calculation
 */

/* **************************************************************************************************
 * Compares                                           *
 * ************************************************************************************************* */
/*
 * @ingroup Compare Level 0
 * @brief Compare the size of a tensor and a scalar one by one. If true, the corresponding bit is 1, otherwise it is 0
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1Scalar input Scalar
 * @param [in] cmpMode compare mode
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.srcBlkStride src0 block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.srcRepStride src0 repeat stride
 */
template <typename T, typename U, bool isSetMask>
__aicore__ inline void Compares(
    const LocalTensor<U>& dst, const LocalTensor<T>& src0, const T src1Scalar, CMPMODE cmpMode, const uint64_t mask[],
    uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Compares", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"));
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryCmpsInfo<U, T, isSetMask>(
        dst, src0, mask[0], mask[1], repeatTime, repeatParams, "Compares");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinaryScalarCmp(
            dst, src0, src1Scalar, ONE_REPEAT_BYTE_SIZE / sizeof(T), repeatTime, repeatParams, "Compares")) {
        ASCENDC_REPORT_CHECK_ERROR("Compares", KernelFuncType::MASK_BIT_MODE);
    }
#endif
    VcmpvsImpl<T, U, isSetMask>(
        (__ubuf__ U*)dst.GetPhyAddr(), (__ubuf__ T*)src0.GetPhyAddr(), src1Scalar, cmpMode, mask, repeatTime,
        repeatParams);
}

template <typename T, typename U, bool isSetMask>
__aicore__ inline void Compares(
    const LocalTensor<U>& dst, const LocalTensor<T>& src0, const T src1Scalar, CMPMODE cmpMode, const uint64_t mask,
    uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Compares", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"));
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryCmpsInfo<U, T, isSetMask>(dst, src0, mask, repeatTime, repeatParams, "Compares");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinaryScalarCmp(
            dst, src0, src1Scalar, ONE_REPEAT_BYTE_SIZE / sizeof(T), repeatTime, repeatParams, "Compares")) {
        ASCENDC_REPORT_CHECK_ERROR("Compares", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
    VcmpvsImpl<T, U, isSetMask>(
        (__ubuf__ U*)dst.GetPhyAddr(), (__ubuf__ T*)src0.GetPhyAddr(), src1Scalar, cmpMode, mask, repeatTime,
        repeatParams);
}

// CompareScalar has been updated, please use Compares instead.
template <typename T, typename U, bool isSetMask>
__aicore__ inline void CompareScalar(
    const LocalTensor<U>& dst, const LocalTensor<T>& src0, const T src1Scalar, CMPMODE cmpMode, const uint64_t mask[],
    uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("CompareScalar", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"));
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryCmpsInfo<U, T, isSetMask>(
        dst, src0, mask[0], mask[1], repeatTime, repeatParams, "CompareScalar");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinaryScalarCmp(
            dst, src0, src1Scalar, ONE_REPEAT_BYTE_SIZE / sizeof(T), repeatTime, repeatParams, "CompareScalar")) {
        ASCENDC_REPORT_CHECK_ERROR("CompareScalar", KernelFuncType::MASK_BIT_MODE);
    }
#endif
    VcmpvsImpl<T, U, isSetMask>(
        (__ubuf__ U*)dst.GetPhyAddr(), (__ubuf__ T*)src0.GetPhyAddr(), src1Scalar, cmpMode, mask, repeatTime,
        repeatParams);
}

// CompareScalar has been updated, please use Compares instead.
template <typename T, typename U, bool isSetMask>
__aicore__ inline void CompareScalar(
    const LocalTensor<U>& dst, const LocalTensor<T>& src0, const T src1Scalar, CMPMODE cmpMode, const uint64_t mask,
    uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("CompareScalar", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"));
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryCmpsInfo<U, T, isSetMask>(dst, src0, mask, repeatTime, repeatParams, "CompareScalar");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinaryScalarCmp(
            dst, src0, src1Scalar, ONE_REPEAT_BYTE_SIZE / sizeof(T), repeatTime, repeatParams, "CompareScalar")) {
        ASCENDC_REPORT_CHECK_ERROR("CompareScalar", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
    VcmpvsImpl<T, U, isSetMask>(
        (__ubuf__ U*)dst.GetPhyAddr(), (__ubuf__ T*)src0.GetPhyAddr(), src1Scalar, cmpMode, mask, repeatTime,
        repeatParams);
}

/*
 * @ingroup Compares Level 2
 * @brief Compares the size of two tensors one by one. If true, the corresponding bit is 1, otherwise it is 0
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1Scalar input Scalar
 * @param [in] cmpMode compare mode
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, typename U>
__aicore__ inline void Compares(
    const LocalTensor<U>& dst, const LocalTensor<T>& src0, const T src1Scalar, CMPMODE cmpMode, uint32_t count)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Compares", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"));
    ASCENDC_DEBUG_ASSERT(
        ((count * sizeof(T)) % ONE_REPEAT_BYTE_SIZE == 0),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR,
            "Failed "
            "to check count in Compares, count * sizeof(T) must be divisible by 256, current count value is %u.\n",
            count));
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryCmpsInfo<U, T, true>(dst, src0, "Compares", count);
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinaryScalarCmp(dst, src0, src1Scalar, count, "Compares")) {
        ASCENDC_REPORT_CHECK_ERROR("Compares", KernelFuncType::CALCOUNT_MODE);
    }
#endif
    VcmpvsImpl((__ubuf__ U*)dst.GetPhyAddr(), (__ubuf__ T*)src0.GetPhyAddr(), src1Scalar, cmpMode, count);
}

// CompareScalar has been updated, please use Compares instead.
template <typename T, typename U>
__aicore__ inline void CompareScalar(
    const LocalTensor<U>& dst, const LocalTensor<T>& src0, const T src1Scalar, CMPMODE cmpMode, uint32_t count)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("CompareScalar", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"));
    ASCENDC_DEBUG_ASSERT(
        ((count * sizeof(T)) % ONE_REPEAT_BYTE_SIZE == 0),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR,
            "Failed "
            "to check count in CompareScalar, count * sizeof(T) must be divisible by 256, current count value is %u.\n",
            count));
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryCmpsInfo<U, T, true>(dst, src0, "CompareScalar", count);
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinaryScalarCmp(dst, src0, src1Scalar, count, "CompareScalar")) {
        ASCENDC_REPORT_CHECK_ERROR("CompareScalar", KernelFuncType::CALCOUNT_MODE);
    }
#endif
    VcmpvsImpl((__ubuf__ U*)dst.GetPhyAddr(), (__ubuf__ T*)src0.GetPhyAddr(), src1Scalar, cmpMode, count);
}

/* **************************************************************************************************
 * Select                                            *
 * ************************************************************************************************* */
// T must be half or Float
// U must be uint8_t

// ================================
/*
 * @ingroup Select Level 0
 * @brief Select element according to the bit value of sel
 * @param [out] dst output LocalTensor
 * @param [in] selMask select mask LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] selMode select mode
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.src0BlkStride src0 block stride
 * @param [in] intriParams.src1BlkStride src1 block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src0 repeat stride
 * @param [in] intriParams.src1RepStride src1 repeat stride
 */
// select mode: 0/1/2
template <typename T, typename U, bool isSetMask>
__aicore__ inline void Select(
    const LocalTensor<T>& dst, const LocalTensor<U>& selMask, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
    SELMODE selMode, uint64_t mask[], uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinarySelInfo(
        dst, selMask, src0, src1, mask[0], mask[1], repeatTime, repeatParams, isSetMask, selMode, "Select");
#endif
    using DataPrimType = PrimT<T>;
    using MaskPrimType = PrimT<U>;
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncSelectVec(dst, selMask, src0, src1, mask, repeatTime, repeatParams, "Select")) {
        ASCENDC_REPORT_CHECK_ERROR("Select", KernelFuncType::MASK_BIT_MODE);
    }
#endif
    VselImpl(
        (__ubuf__ DataPrimType*)dst.GetPhyAddr(), (__ubuf__ MaskPrimType*)selMask.GetPhyAddr(),
        (__ubuf__ DataPrimType*)src0.GetPhyAddr(), (__ubuf__ DataPrimType*)src1.GetPhyAddr(), selMode, mask, repeatTime,
        repeatParams);
}

// select mode: 0/1/2
template <typename T, typename U, bool isSetMask>
__aicore__ inline void Select(
    const LocalTensor<T>& dst, const LocalTensor<U>& selMask, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
    SELMODE selMode, uint64_t mask, uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinarySelInfo(
        dst, selMask, src0, src1, mask, repeatTime, repeatParams, isSetMask, selMode, "Select");
#endif
    using DataPrimType = PrimT<T>;
    using MaskPrimType = PrimT<U>;
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncSelectVec(dst, selMask, src0, src1, mask, repeatTime, repeatParams, "Select")) {
        ASCENDC_REPORT_CHECK_ERROR("Select", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
    VselImpl(
        (__ubuf__ DataPrimType*)dst.GetPhyAddr(), (__ubuf__ MaskPrimType*)selMask.GetPhyAddr(),
        (__ubuf__ DataPrimType*)src0.GetPhyAddr(), (__ubuf__ DataPrimType*)src1.GetPhyAddr(), selMode, mask, repeatTime,
        repeatParams);
}

template <typename T, SELMODE selMode>
__aicore__ inline void Select(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinarySelInfo(dst, src0, src1, repeatTime, repeatParams, selMode, "Select");
#endif
    using PrimType = PrimT<T>;
    SelectCal<PrimType, selMode>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), repeatTime, repeatParams);
}

template <typename T, typename U>
__aicore__ inline void Select(
    const LocalTensor<T>& dst, const LocalTensor<U>& selMask, const LocalTensor<T>& src0, uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinarySelInfo(
        dst, selMask, src0, repeatTime, repeatParams, SELMODE::VSEL_TENSOR_SCALAR_MODE, "Select");
#endif
    using DataPrimType = PrimT<T>;
    using MaskPrimType = PrimT<U>;
    SelectCal<DataPrimType, MaskPrimType>(
        (__ubuf__ DataPrimType*)dst.GetPhyAddr(), (__ubuf__ MaskPrimType*)selMask.GetPhyAddr(),
        (__ubuf__ DataPrimType*)src0.GetPhyAddr(), repeatTime, repeatParams);
}

/*
 * @ingroup Select Level 2
 * @brief Select element according to the bit value of sel
 * @param [out] dst output LocalTensor
 * @param [in] selMask select mask LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] selMode select mode
 * @param [in] count number Number of data involved in calculation
 */
// select mode: 0/1/2
template <typename T, typename U>
__aicore__ inline void Select(
    const LocalTensor<T>& dst, const LocalTensor<U>& selMask, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
    SELMODE selMode, uint32_t count)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinarySelInfo(dst, selMask, src0, src1, count, true, selMode, "Select");
#endif
    using DataPrimType = PrimT<T>;
    using MaskPrimType = PrimT<U>;
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncSelectVec(dst, selMask, src0, src1, (int32_t)count, "Select")) {
        ASCENDC_REPORT_CHECK_ERROR("Select", KernelFuncType::CALCOUNT_MODE);
    }
#endif
    VselImpl(
        (__ubuf__ DataPrimType*)dst.GetPhyAddr(), (__ubuf__ MaskPrimType*)selMask.GetPhyAddr(),
        (__ubuf__ DataPrimType*)src0.GetPhyAddr(), (__ubuf__ DataPrimType*)src1.GetPhyAddr(), selMode, count);
}

// ================================
/*
 * @ingroup Select Level 0
 * @brief Select element according to the bit value of sel
 * @param [out] dst output LocalTensor
 * @param [in] selMask select mask LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input number
 * @param [in] selMode select mode
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.src0BlkStride src0 block stride
 * @param [in] intriParams.src1BlkStride src1 block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src0 repeat stride
 * @param [in] intriParams.src1RepStride src1 repeat stride
 */
// select mode: 1
template <typename T, typename U, bool isSetMask>
__aicore__ inline void Select(
    const LocalTensor<T>& dst, const LocalTensor<U>& selMask, const LocalTensor<T>& src0, T src1, SELMODE selMode,
    uint64_t mask[], uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinarySelInfo(
        dst, selMask, src0, mask[0], mask[1], repeatTime, repeatParams, isSetMask, selMode, "Select");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src0, src1, mask, repeatTime, repeatParams, "Select")) {
        ASCENDC_REPORT_CHECK_ERROR("Select", KernelFuncType::MASK_BIT_MODE);
    }
#endif
    CheckTensorPos<U>(selMask, Hardware::UB, "selMask", "VECIN/VECCALC/VECOUT", "Select");
    VselImpl(
        (__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ U*)selMask.GetPhyAddr(), (__ubuf__ T*)src0.GetPhyAddr(), src1, selMode,
        mask, repeatTime, repeatParams);
}

// select mode: 1
template <typename T, typename U, bool isSetMask>
__aicore__ inline void Select(
    const LocalTensor<T>& dst, const LocalTensor<U>& selMask, const LocalTensor<T>& src0, T src1, SELMODE selMode,
    uint64_t mask, uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinarySelInfo(
        dst, selMask, src0, mask, repeatTime, repeatParams, isSetMask, selMode, "Select");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar(dst, src0, src1, mask, repeatTime, repeatParams, "Select")) {
        ASCENDC_REPORT_CHECK_ERROR("Select", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
    CheckTensorPos<U>(selMask, Hardware::UB, "selMask", "VECIN/VECCALC/VECOUT", "Select");
    VselImpl(
        (__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ U*)selMask.GetPhyAddr(), (__ubuf__ T*)src0.GetPhyAddr(), src1, selMode,
        mask, repeatTime, repeatParams);
}

// select mode: 1
/*
 * @ingroup Select Level 2
 * @brief Select element according to the bit value of sel
 * @param [out] dst output LocalTensor
 * @param [in] selMask select mask LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input number
 * @param [in] selMode select mode
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, typename U>
__aicore__ inline void Select(
    const LocalTensor<T>& dst, const LocalTensor<U>& selMask, const LocalTensor<T>& src0, T src1, SELMODE selMode,
    uint32_t count)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinarySelInfo(dst, selMask, src0, count, true, selMode, "Select");
#endif
    using DataPrimType = PrimT<T>;
    using MaskPrimType = PrimT<U>;
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecBinaryScalar(dst, src0, src1, (int32_t)count, "Select")) {
        ASCENDC_REPORT_CHECK_ERROR("Select", KernelFuncType::CALCOUNT_MODE);
    }
#endif
    CheckTensorPos<U>(selMask, Hardware::UB, "selMask", "VECIN/VECCALC/VECOUT", "Select");
    VselImpl(
        (__ubuf__ DataPrimType*)dst.GetPhyAddr(), (__ubuf__ MaskPrimType*)selMask.GetPhyAddr(),
        (__ubuf__ DataPrimType*)src0.GetPhyAddr(), src1, selMode, count);
}

#if defined(__NPU_ARCH__) && \
    ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
/* **************************************************************************************************
 * Compares                                           *
 * ************************************************************************************************* */
/*
 * @ingroup Compare Level 0
 * @brief Compare the size of a tensor and a scalar one by one. If true, the corresponding bit is 1, otherwise it is 0
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1Scalar input Scalar
 * @param [in] cmpMode compare mode
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.srcBlkStride src0 block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.srcRepStride src0 repeat stride
 */
template <bool isSetMask, const BinaryConfig& config, typename T2, typename T3, typename T4, typename MaskType>
__aicore__ inline void CompareScalarCommon(
    const T2& dst, const T3& src0, const T4& src1, CMPMODE cmpMode, MaskType mask, uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert(!TypeUtils::IsInnerDefaultType<T3, T4>(), "One of src0 and src1 should be Tensor");
    static_assert(SupportType<T2, LocalTensor<uint8_t>>());

    using ActualU = typename T2::PrimType;
    if constexpr (TypeUtils::IsLocalTensorType<T3, T4>()) {
        static_assert(Std::is_same<T3, T4>::value);
        using ActualT = typename T3::PrimType;
        static_assert(
            (config.scalarTensorIndex == 0 || config.scalarTensorIndex == 1), "scalarTensorIndex out of range");
        VcmpvsImpl<ActualT, ActualU, isSetMask, config.scalarTensorIndex>(
            (__ubuf__ ActualU*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(),
            (__ubuf__ ActualT*)src1.GetPhyAddr(), cmpMode, mask, repeatTime, repeatParams);
    } else if constexpr (TypeUtils::IsLocalTensorType<T4>() && TypeUtils::IsInnerDefaultType<T3>()) {
        using ActualT = typename T4::PrimType;
        VcmpvsImpl<ActualT, ActualU, isSetMask>(
            (__ubuf__ ActualU*)dst.GetPhyAddr(), src0, (__ubuf__ ActualT*)src1.GetPhyAddr(), cmpMode, mask, repeatTime,
            repeatParams);
    } else if constexpr (TypeUtils::IsLocalTensorType<T3>() && TypeUtils::IsInnerDefaultType<T4>()) {
        using ActualT = typename T3::PrimType;
        VcmpvsImpl<ActualT, ActualU, isSetMask>(
            (__ubuf__ ActualU*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(), src1, cmpMode, mask, repeatTime,
            repeatParams);
    }
}

template <typename T0, typename T1, bool isSetMask, const BinaryConfig& config, typename T2, typename T3, typename T4>
__aicore__ inline void Compares(
    const T2& dst, const T3& src0, const T4& src1, CMPMODE cmpMode, const uint64_t mask[], uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinaryScalarCmp<config>(
            dst, src0, src1, ONE_REPEAT_BYTE_SIZE / sizeof(T0), repeatTime, repeatParams, "Compares")) {
        ASCENDC_REPORT_CHECK_ERROR("Compares", KernelFuncType::MASK_BIT_MODE);
    }
#endif
    CompareScalarCommon<isSetMask, config>(dst, src0, src1, cmpMode, mask, repeatTime, repeatParams);
}

template <typename T0, typename T1, bool isSetMask, const BinaryConfig& config, typename T2, typename T3, typename T4>
__aicore__ inline void Compares(
    const T2& dst, const T3& src0, const T4& src1, CMPMODE cmpMode, const uint64_t mask, uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinaryScalarCmp<config>(
            dst, src0, src1, ONE_REPEAT_BYTE_SIZE / sizeof(T0), repeatTime, repeatParams, "Compares")) {
        ASCENDC_REPORT_CHECK_ERROR("Compares", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
    CompareScalarCommon<isSetMask, config>(dst, src0, src1, cmpMode, mask, repeatTime, repeatParams);
}

// CompareScalar has been updated, please use Compares instead.
template <typename T0, typename T1, bool isSetMask, const BinaryConfig& config, typename T2, typename T3, typename T4>
__aicore__ inline void CompareScalar(
    const T2& dst, const T3& src0, const T4& src1, CMPMODE cmpMode, const uint64_t mask[], uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinaryScalarCmp<config>(
            dst, src0, src1, ONE_REPEAT_BYTE_SIZE / sizeof(T0), repeatTime, repeatParams, "CompareScalar")) {
        ASCENDC_REPORT_CHECK_ERROR("CompareScalar", KernelFuncType::MASK_BIT_MODE);
    }
#endif
    CompareScalarCommon<isSetMask, config>(dst, src0, src1, cmpMode, mask, repeatTime, repeatParams);
}

// CompareScalar has been updated, please use Compares instead.
template <typename T0, typename T1, bool isSetMask, const BinaryConfig& config, typename T2, typename T3, typename T4>
__aicore__ inline void CompareScalar(
    const T2& dst, const T3& src0, const T4& src1, CMPMODE cmpMode, const uint64_t mask, uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinaryScalarCmp<config>(
            dst, src0, src1, ONE_REPEAT_BYTE_SIZE / sizeof(T0), repeatTime, repeatParams, "CompareScalar")) {
        ASCENDC_REPORT_CHECK_ERROR("CompareScalar", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
    CompareScalarCommon<isSetMask, config>(dst, src0, src1, cmpMode, mask, repeatTime, repeatParams);
}

/*
 * @ingroup Compares Level 2
 * @brief Compares the size of two tensors one by one. If true, the corresponding bit is 1, otherwise it is 0
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1Scalar input Scalar
 * @param [in] cmpMode compare mode
 * @param [in] count number Number of data involved in calculation
 */
template <typename T0, typename T1, bool isSetMask, const BinaryConfig& config, typename T2, typename T3, typename T4>
__aicore__ inline void Compares(const T2& dst, const T3& src0, const T4& src1, CMPMODE cmpMode, uint32_t count)
{
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinaryScalarCmp<config>(dst, src0, src1, count, "Compares")) {
        ASCENDC_REPORT_CHECK_ERROR("Compares", KernelFuncType::CALCOUNT_MODE);
    }
#endif
    static_assert(!TypeUtils::IsInnerDefaultType<T3, T4>(), "One of src0 and src1 should be Tensor");
    static_assert(SupportType<T2, LocalTensor<uint8_t>>());

    using ActualU = typename T2::PrimType;
#if defined(__NPU_ARCH__) && \
    ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
    ASCENDC_ASSERT(((count * sizeof(T3)) % ONE_REPEAT_BYTE_SIZE == 0), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "Failed to check count elements size in Compares, current size "
            "is %u, should be an integer multiple of 256.",
            count * sizeof(T3));
    });
#endif
    if constexpr (TypeUtils::IsLocalTensorType<T3, T4>()) {
        static_assert(Std::is_same<T3, T4>::value);
        using ActualT = typename T3::PrimType;
        static_assert(
            (config.scalarTensorIndex == 0 || config.scalarTensorIndex == 1), "scalarTensorIndex out of range");
        VcmpvsImpl<ActualT, ActualU, isSetMask, config.scalarTensorIndex>(
            (__ubuf__ ActualU*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(),
            (__ubuf__ ActualT*)src1.GetPhyAddr(), cmpMode, count);
    } else if constexpr (TypeUtils::IsLocalTensorType<T4>() && TypeUtils::IsInnerDefaultType<T3>()) {
        using ActualT = typename T4::PrimType;
        VcmpvsImpl<ActualT, ActualU, isSetMask>(
            (__ubuf__ ActualU*)dst.GetPhyAddr(), src0, (__ubuf__ ActualT*)src1.GetPhyAddr(), cmpMode, count);
    } else if constexpr (TypeUtils::IsLocalTensorType<T3>() && TypeUtils::IsInnerDefaultType<T4>()) {
        using ActualT = typename T3::PrimType;
        VcmpvsImpl<ActualT, ActualU, isSetMask>(
            (__ubuf__ ActualU*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(), src1, cmpMode, count);
    }
}

// CompareScalar has been updated, please use Compares instead.
template <typename T0, typename T1, bool isSetMask, const BinaryConfig& config, typename T2, typename T3, typename T4>
__aicore__ inline void CompareScalar(const T2& dst, const T3& src0, const T4& src1, CMPMODE cmpMode, uint32_t count)
{
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinaryScalarCmp<config>(dst, src0, src1, count, "CompareScalar")) {
        ASCENDC_REPORT_CHECK_ERROR("CompareScalar", KernelFuncType::CALCOUNT_MODE);
    }
#endif
    static_assert(!TypeUtils::IsInnerDefaultType<T3, T4>(), "One of src0 and src1 should be Tensor");
    static_assert(SupportType<T2, LocalTensor<uint8_t>>());

    using ActualU = typename T2::PrimType;
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    ASCENDC_ASSERT(((count * sizeof(T3)) % ONE_REPEAT_BYTE_SIZE == 0), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "Failed to check count elements size in CompareScalar, current size "
            "is %u, should be an integer multiple of 256.",
            count * sizeof(T3));
    });
#endif
    if constexpr (TypeUtils::IsLocalTensorType<T3, T4>()) {
        static_assert(Std::is_same<T3, T4>::value);
        using ActualT = typename T3::PrimType;
        static_assert(
            (config.scalarTensorIndex == 0 || config.scalarTensorIndex == 1), "scalarTensorIndex out of range");
        VcmpvsImpl<ActualT, ActualU, isSetMask, config.scalarTensorIndex>(
            (__ubuf__ ActualU*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(),
            (__ubuf__ ActualT*)src1.GetPhyAddr(), cmpMode, count);
    } else if constexpr (TypeUtils::IsLocalTensorType<T4>() && TypeUtils::IsInnerDefaultType<T3>()) {
        using ActualT = typename T4::PrimType;
        VcmpvsImpl<ActualT, ActualU, isSetMask>(
            (__ubuf__ ActualU*)dst.GetPhyAddr(), src0, (__ubuf__ ActualT*)src1.GetPhyAddr(), cmpMode, count);
    } else if constexpr (TypeUtils::IsLocalTensorType<T3>() && TypeUtils::IsInnerDefaultType<T4>()) {
        using ActualT = typename T3::PrimType;
        VcmpvsImpl<ActualT, ActualU, isSetMask>(
            (__ubuf__ ActualU*)dst.GetPhyAddr(), (__ubuf__ ActualT*)src0.GetPhyAddr(), src1, cmpMode, count);
    }
}

// ================================
/*
 * @ingroup Select Level 0
 * @brief Select element according to the bit value of sel
 * @param [out] dst output LocalTensor
 * @param [in] selMask select mask LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input number
 * @param [in] selMode select mode
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.src0BlkStride src0 block stride
 * @param [in] intriParams.src1BlkStride src1 block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src0 repeat stride
 * @param [in] intriParams.src1RepStride src1 repeat stride
 */
// select mode: 1
template <
    typename T1, bool isSetMask, const BinaryConfig& config, typename T2, typename T3, typename T4, typename MaskType>
__aicore__ inline void SelectCommon(
    const T2& dst, const LocalTensor<T1>& selMask, const T3& src0, const T4& src1, SELMODE selMode, MaskType mask,
    uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    CheckTensorPos<T1>(selMask, Hardware::UB, "selMask", "VECIN/VECCALC/VECOUT", "Select");
    static_assert(!TypeUtils::IsInnerDefaultType<T3, T4>(), "One of src0 and src1 should be Tensor");
    static_assert(TypeUtils::IsLocalTensorType<T2>(), "dst should be Tensor");

    using ActualT = typename T2::PrimType;
    if constexpr (TypeUtils::IsLocalTensorType<T3, T4>()) {
        static_assert(Std::is_same<T3, T4>::value);
        static_assert(Std::is_same<ActualT, typename T3::PrimType>::value);
        static_assert(
            (config.scalarTensorIndex == 0 || config.scalarTensorIndex == 1), "scalarTensorIndex out of range");
        VselImpl<ActualT, T1, config.scalarTensorIndex>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ T1*)selMask.GetPhyAddr(),
            (__ubuf__ ActualT*)src0.GetPhyAddr(), (__ubuf__ ActualT*)src1.GetPhyAddr(), selMode, mask, repeatTime,
            repeatParams);
    } else if constexpr (TypeUtils::IsLocalTensorType<T4>() && TypeUtils::IsInnerDefaultType<T3>()) {
        static_assert(Std::is_same<ActualT, typename T4::PrimType>::value);
        VselImpl(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ T1*)selMask.GetPhyAddr(), src0,
            (__ubuf__ ActualT*)src1.GetPhyAddr(), selMode, mask, repeatTime, repeatParams);
    } else if constexpr (TypeUtils::IsLocalTensorType<T3>() && TypeUtils::IsInnerDefaultType<T4>()) {
        static_assert(Std::is_same<ActualT, typename T3::PrimType>::value);
        VselImpl(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ T1*)selMask.GetPhyAddr(),
            (__ubuf__ ActualT*)src0.GetPhyAddr(), src1, selMode, mask, repeatTime, repeatParams);
    }
}

// select mode: 1
template <typename T0, typename T1, bool isSetMask, const BinaryConfig& config, typename T2, typename T3, typename T4>
__aicore__ inline void Select(
    const T2& dst, const LocalTensor<T1>& selMask, const T3& src0, const T4& src1, SELMODE selMode, uint64_t mask[],
    uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar<config>(dst, src0, src1, mask, repeatTime, repeatParams, "Select")) {
        ASCENDC_REPORT_CHECK_ERROR("Select", KernelFuncType::MASK_BIT_MODE);
    }
#endif
    SelectCommon<T1, isSetMask, config>(dst, selMask, src0, src1, selMode, mask, repeatTime, repeatParams);
}

// select mode: 1
template <typename T0, typename T1, bool isSetMask, const BinaryConfig& config, typename T2, typename T3, typename T4>
__aicore__ inline void Select(
    const T2& dst, const LocalTensor<T1>& selMask, const T3& src0, const T4& src1, SELMODE selMode, uint64_t mask,
    uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalar<config>(dst, src0, src1, mask, repeatTime, repeatParams, "Select")) {
        ASCENDC_REPORT_CHECK_ERROR("Select", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
    SelectCommon<T1, isSetMask, config>(dst, selMask, src0, src1, selMode, mask, repeatTime, repeatParams);
}

// select mode: 1
/*
 * @ingroup Select Level 2
 * @brief Select element according to the bit value of sel
 * @param [out] dst output LocalTensor
 * @param [in] selMask select mask LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input number
 * @param [in] selMode select mode
 * @param [in] count number Number of data involved in calculation
 */
template <typename T0, typename T1, const BinaryConfig& config, typename T2, typename T3, typename T4>
__aicore__ inline void Select(
    const T2& dst, const LocalTensor<T1>& selMask, const T3& src0, const T4& src1, SELMODE selMode, uint32_t count)
{
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecBinaryScalar<config>(dst, src0, src1, count, "Select")) {
        ASCENDC_REPORT_CHECK_ERROR("Select", KernelFuncType::CALCOUNT_MODE);
    }
#endif
    CheckTensorPos<T1>(selMask, Hardware::UB, "selMask", "VECIN/VECCALC/VECOUT", "Select");
    static_assert(!TypeUtils::IsInnerDefaultType<T3, T4>(), "One of src0 and src1 should be Tensor");
    static_assert(TypeUtils::IsLocalTensorType<T2>(), "dst should be Tensor");

    using ActualT = typename T2::PrimType;
    if constexpr (TypeUtils::IsLocalTensorType<T3, T4>()) {
        static_assert(Std::is_same<T3, T4>::value);
        static_assert(Std::is_same<ActualT, typename T3::PrimType>::value);
        static_assert(
            (config.scalarTensorIndex == 0 || config.scalarTensorIndex == 1), "scalarTensorIndex out of range");
        VselImpl<ActualT, T1, config.scalarTensorIndex>(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ T1*)selMask.GetPhyAddr(),
            (__ubuf__ ActualT*)src0.GetPhyAddr(), (__ubuf__ ActualT*)src1.GetPhyAddr(), selMode, count);
    } else if constexpr (TypeUtils::IsLocalTensorType<T4>() && TypeUtils::IsInnerDefaultType<T3>()) {
        static_assert(Std::is_same<ActualT, typename T4::PrimType>::value);
        VselImpl(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ T1*)selMask.GetPhyAddr(), src0,
            (__ubuf__ ActualT*)src1.GetPhyAddr(), selMode, count);
    } else if constexpr (TypeUtils::IsLocalTensorType<T3>() && TypeUtils::IsInnerDefaultType<T4>()) {
        static_assert(Std::is_same<ActualT, typename T3::PrimType>::value);
        VselImpl(
            (__ubuf__ ActualT*)dst.GetPhyAddr(), (__ubuf__ T1*)selMask.GetPhyAddr(),
            (__ubuf__ ActualT*)src0.GetPhyAddr(), src1, selMode, count);
    }
}
#endif
} // namespace AscendC
#pragma end_pipe
#endif // ASCENDC_MODULE_OPERATOR_VEC_CMPSEL_INTERFACE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_CMPSEL_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_CMPSEL_INTF_IMPL_H__
#endif
