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
 * \file kernel_operator_vec_reduce_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_vec_reduce_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_vec_reduce_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_REDUCE_INTF_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_REDUCE_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_REDUCE_INTERFACE_IMPL_H
#include "../../include/basic_api/kernel_tensor.h"
#include "mstx_local_tensor_info.h"
#include "../utils/std/type_traits/enable_if.h"

#if __NPU_ARCH__ == 1001
#include "dav_c100/kernel_operator_vec_reduce_impl.h"
#elif __NPU_ARCH__ == 2002
#include "dav_m200/kernel_operator_vec_reduce_impl.h"
#elif __NPU_ARCH__ == 2201
#include "dav_c220/kernel_operator_vec_reduce_impl.h"
#elif __NPU_ARCH__ == 3002
#include "dav_m300/kernel_operator_vec_reduce_impl.h"
#elif __NPU_ARCH__ == 3102
#include "dav_m310/kernel_operator_vec_reduce_impl.h"
#elif __NPU_ARCH__ == 3510
#include "dav_3510/kernel_operator_vec_reduce_impl.h"
#elif (__NPU_ARCH__ == 5102)
#include "dav_m510/kernel_operator_vec_reduce_impl.h"
#elif (__NPU_ARCH__ == 3003)
#include "dav_l300/kernel_operator_vec_reduce_impl.h"
#elif (__NPU_ARCH__ == 3113)
#include "dav_l311/kernel_operator_vec_reduce_impl.h"
#endif

#include "kernel_check.h"
#include "kernel_npu_debug.h"

namespace AscendC {
#pragma begin_pipe(V)

namespace Internal {
// Reduce base checks: dtype, repeatTime, position, src alignment
// Shared by ReduceDataBlock and ReduceRepeat (both maskCount and maskBit overloads)
// only support same src and dst type
template <typename T>
__aicore__ inline void CheckReduceBaseParams(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t repeatTime, const __gm__ char* apiName)
{
    using PrimType = PrimT<T>;
    ASCENDC_DEBUG_ASSERT(
        (SupportType<PrimType, half, float>()),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR,
            "Failed to check dtype in %s, current api support dtype combination is "
            "src and dst both: half / float.\n",
            apiName));
    ReportNopWarning<int32_t>(repeatTime, "repeatTime", apiName);
    CheckValueRange<int32_t>(repeatTime, 0, 255, "repeatTime", apiName);
    CheckTensorPhyPosition<Hardware::UB>(dst, "dst", "UB(VECIN/VECCALC/VECOUT)", apiName);
    CheckTensorPhyPosition<Hardware::UB>(src, "src", "UB(VECIN/VECCALC/VECOUT)", apiName);
    CheckTensorAlignment(src, ONE_BLK_SIZE, "src", apiName);
}

// ReduceDataBlock common checks: dtype, repeatTime, position, alignment, mask, strides
// Used by: ReduceDataBlock (both maskCount and maskBit overloads)
template <typename T, typename U, bool isSetMask, typename MaskType>
__aicore__ inline void CheckReduceDataBlockParams(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const MaskType mask, const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, const __gm__ char* apiName)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckReduceBaseParams<T>(dst, src, repeatTime, apiName);
    using DstPrimType = PrimT<T>;
    using SrcPrimType = PrimT<U>;
    CheckTensorAlignment(dst, 8 * sizeof(DstPrimType), "dst", apiName); // half: 16B, float: 32B
    if constexpr (Std::is_same_v<MaskType, int32_t>) {
        CheckMaskValue<SrcPrimType, isSetMask>(mask, apiName);
    } else {
        CheckMaskArray<SrcPrimType, isSetMask>(mask, apiName);
    }
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecReduceOther(
            dst, src, repeatTime, mask, dstRepStride, srcBlkStride, srcRepStride, "BlockReduceMax")) {
        ASCENDC_REPORT_CHECK_ERROR(
            apiName,
            (Std::is_same_v<MaskType, int32_t> ? KernelFuncType::MASK_COUNT_MODE : KernelFuncType::MASK_BIT_MODE));
    }
#endif
}

// internal check for ReducePairElem, used for both maskCount and maskBit overloads
template <typename T, typename U, bool isSetMask, typename MaskType>
__aicore__ inline void CheckReducePairElemParams(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const MaskType mask, const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, const __gm__ char* apiName)
{
    using SrcPrimType = PrimT<U>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    ASCENDC_DEBUG_ASSERT(
        (SupportType<SrcPrimType, half, float>()),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR,
            "Failed to check dtype in %s, current api support dtype combination is "
            "src and dst both: half / float.\n",
            apiName));
    CheckValueRange<int32_t>(repeatTime, 0, 255, "repeatTime", apiName);
    ReportNopWarning<int32_t>(repeatTime, "repeatTime", apiName);
    CheckVectorTensor(apiName, NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    if constexpr (Std::is_same_v<MaskType, int32_t>) {
        CheckMaskValue<SrcPrimType, isSetMask>(mask, apiName);
    } else {
        CheckMaskArray<SrcPrimType, isSetMask>(mask, apiName);
    }
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecReduceOther(
            dst, src, repeatTime, mask, dstRepStride, srcBlkStride, srcRepStride, "PairReduceSum")) {
        ASCENDC_REPORT_CHECK_ERROR(
            apiName,
            (Std::is_same_v<MaskType, int32_t> ? KernelFuncType::MASK_COUNT_MODE : KernelFuncType::MASK_BIT_MODE));
    }
#endif
}

// internal check for ReduceRepeat with ReduceType::SUM, only used for sum since max/min has order param check
template <typename T, typename U, bool isSetMask, typename MaskType>
__aicore__ inline void CheckReduceRepeatSumParams(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const MaskType mask, const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, const __gm__ char* apiName)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    using DstPrimType = PrimT<T>;
    CheckTensorAlignment(dst, sizeof(DstPrimType), "dst", apiName);
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecReduceOther(
            dst, src, repeatTime, mask, dstRepStride, srcBlkStride, srcRepStride, "WholeReduceSum")) {
        ASCENDC_REPORT_CHECK_ERROR(
            apiName,
            (Std::is_same_v<MaskType, int32_t> ? KernelFuncType::MASK_COUNT_MODE : KernelFuncType::MASK_BIT_MODE));
    }
#endif
}

// internal check for ReduceRepeat with ReduceType::MAX/MIN, used for max/min since it has order param check
template <typename T, typename U, bool isSetMask, typename MaskType>
__aicore__ inline void CheckReduceRepeatMaxMinParams(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const MaskType mask, const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, ReduceOrder order,
    const __gm__ char* apiName)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
#if defined(__NPU_ARCH__) &&                                                                                 \
    ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 5102) || \
     (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || (__NPU_ARCH__ == 3510))
    CheckValueRange<int>(static_cast<int>(order), 0, 3, "order", apiName);
#elif (__NPU_ARCH__ == 1001) || (__NPU_ARCH__ == 2002)
    CheckValueRange<int>(static_cast<int>(order), 0, 1, "order", apiName);
#endif
    using DstPrimType = PrimT<T>;
    CheckTensorAlignment(dst, 2 * sizeof(DstPrimType), "dst", apiName);
#endif
#if ASCENDC_CPU_DEBUG && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecReduceOtherWhl(
            dst, src, repeatTime, mask, dstRepStride, srcBlkStride, srcRepStride, order,
            "WholeReduceMax")) { // WholeReduceMin has same check as WholeReduceMax in cpudebug
        ASCENDC_REPORT_CHECK_ERROR(
            apiName,
            (Std::is_same_v<MaskType, int32_t> ? KernelFuncType::MASK_COUNT_MODE : KernelFuncType::MASK_BIT_MODE));
    }
#endif
#if ASCENDC_CPU_DEBUG && (__NPU_ARCH__ == 3002 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecReduceOther(
            dst, src, repeatTime, mask, dstRepStride, srcBlkStride, srcRepStride,
            "WholeReduceMax")) { // WholeReduceMin has same check as WholeReduceMax in cpudebug
        ASCENDC_REPORT_CHECK_ERROR(
            apiName,
            (Std::is_same_v<MaskType, int32_t> ? KernelFuncType::MASK_COUNT_MODE : KernelFuncType::MASK_BIT_MODE));
    }
#endif
}

// internal check for ReduceRepeat, used for SUM/MAX/MIN and both maskCount and maskBit overloads
template <ReduceType reduceType, typename T, typename U, bool isSetMask, typename MaskType>
__aicore__ inline void CheckReduceRepeatParams(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const MaskType mask, const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, ReduceOrder order,
    const __gm__ char* apiName)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
#if __NPU_ARCH__ == 3510
    CheckValueRange<int32_t>(repeatTime, 0, 255, "repeatTime", apiName);
    ReportNopWarning<int32_t>(repeatTime, "repeatTime", apiName);
    CheckTensorPhyPosition<Hardware::UB>(dst, "dst", "UB(VECIN/VECCALC/VECOUT)", apiName);
    CheckTensorPhyPosition<Hardware::UB>(src, "src", "UB(VECIN/VECCALC/VECOUT)", apiName);
    CheckTensorAlignment(src, ONE_BLK_SIZE, "src", apiName);
#else
    CheckReduceBaseParams<T>(dst, src, repeatTime, apiName);
#endif
    using SrcPrimType = PrimT<U>;
    if constexpr (Std::is_same_v<MaskType, int32_t>) {
        CheckMaskValue<SrcPrimType, isSetMask>(mask, apiName);
    } else {
        CheckMaskArray<SrcPrimType, isSetMask>(mask, apiName);
    }
#endif
    if constexpr (reduceType == ReduceType::SUM) {
        CheckReduceRepeatSumParams<T, U, isSetMask>(
            dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, apiName);
    } else {
        CheckReduceRepeatMaxMinParams<T, U, isSetMask>(
            dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, order, apiName);
    }
}

// internal check for ReduceRepeat SUM, which has no order semantics
template <
    ReduceType reduceType, typename T, typename U, bool isSetMask, typename MaskType,
    typename Std::enable_if<reduceType == ReduceType::SUM, bool>::type = true>
__aicore__ inline void CheckReduceRepeatParams(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const MaskType mask, const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, const __gm__ char* apiName)
{
    CheckReduceRepeatParams<reduceType, T, U, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, ReduceOrder::ORDER_VALUE_INDEX, apiName);
}

// internal implementation for ReduceDataBlock, used for both maskCount and maskBit overloads
template <ReduceType reduceType, typename T, typename U, bool isSetMask, typename MaskType>
__aicore__ inline void ReduceDataBlockCommon(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const MaskType mask, const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
{
    using DstPrimType = PrimT<T>;
    using SrcPrimType = PrimT<U>;
    static_assert(
        (SupportEnum<reduceType, ReduceType::SUM, ReduceType::MAX, ReduceType::MIN>()),
        "Invalid reduceType for ReduceDataBlock, only ReduceType::SUM, ReduceType::MAX and ReduceType::MIN are "
        "supported.");
    static_assert(
        Std::is_same_v<SrcPrimType, DstPrimType>,
        "ReduceDataBlock only supports identical src type and dst type for current NPU_ARCH.");
    if constexpr (reduceType == ReduceType::SUM) {
        BlockReduceSumImpl<SrcPrimType, isSetMask>(
            (__ubuf__ DstPrimType*)dst.GetPhyAddr(), (__ubuf__ SrcPrimType*)src.GetPhyAddr(), repeatTime, mask,
            dstRepStride, srcBlkStride, srcRepStride);
    } else if constexpr (reduceType == ReduceType::MAX) {
        BlockReduceMaxImpl<SrcPrimType, isSetMask>(
            (__ubuf__ DstPrimType*)dst.GetPhyAddr(), (__ubuf__ SrcPrimType*)src.GetPhyAddr(), repeatTime, mask,
            dstRepStride, srcBlkStride, srcRepStride);
    } else {
        BlockReduceMinImpl<SrcPrimType, isSetMask>(
            (__ubuf__ DstPrimType*)dst.GetPhyAddr(), (__ubuf__ SrcPrimType*)src.GetPhyAddr(), repeatTime, mask,
            dstRepStride, srcBlkStride, srcRepStride);
    }
}

// internal implementation for ReducePairElem, used for both maskCount and maskBit overloads
template <ReduceType reduceType, typename T, typename U, bool isSetMask, typename MaskType>
__aicore__ inline void ReducePairElemCommon(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const MaskType mask, const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
{
    using DstPrimType = PrimT<T>;
    using SrcPrimType = PrimT<U>;
    static_assert(
        (SupportEnum<reduceType, ReduceType::SUM>()),
        "Invalid reduceType for ReducePairElem, only ReduceType::SUM is supported.");
    static_assert(
        Std::is_same_v<SrcPrimType, DstPrimType>,
        "ReducePairElem only supports identical src type and dst type for current NPU_ARCH.");
    PairReduceSumImpl<SrcPrimType, isSetMask>(
        (__ubuf__ DstPrimType*)dst.GetPhyAddr(), (__ubuf__ SrcPrimType*)src.GetPhyAddr(), repeatTime, mask,
        dstRepStride, srcBlkStride, srcRepStride);
}

// internal implementation for ReduceRepeat, used for both maskCount and maskBit overloads
template <ReduceType reduceType, typename T, typename U, bool isSetMask, typename MaskType>
__aicore__ inline void ReduceRepeatCommon(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const MaskType mask, const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride,
    ReduceOrder order = ReduceOrder::ORDER_VALUE_INDEX)
{
    using DstPrimType = PrimT<T>;
    using SrcPrimType = PrimT<U>;
    static_assert(
        (SupportEnum<reduceType, ReduceType::SUM, ReduceType::MAX, ReduceType::MIN>()),
        "Invalid reduceType for ReduceRepeat, only ReduceType::SUM, ReduceType::MAX and ReduceType::MIN are "
        "supported.");
#if defined(__NPU_ARCH__) && \
    ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
    static_assert(
        (reduceType == ReduceType::SUM || Std::is_same_v<SrcPrimType, DstPrimType>),
        "ReduceRepeat for MAX/MIN only supports identical dst type (T) and src type (U) for current NPU_ARCH.");
#else
    static_assert(
        Std::is_same_v<SrcPrimType, DstPrimType>,
        "ReduceRepeat only supports identical dst type (T) and src type (U) for current NPU_ARCH.");
#endif
    if constexpr (reduceType == ReduceType::SUM) {
        // DstPrimType is auto derived when __NPU_ARCH__ is 3510 / 5102 / 3003 / 3113
        WholeReduceSumImpl<SrcPrimType, isSetMask>(
            (__ubuf__ DstPrimType*)dst.GetPhyAddr(), (__ubuf__ SrcPrimType*)src.GetPhyAddr(), mask, repeatTime,
            dstRepStride, srcBlkStride, srcRepStride);
    } else {
        if constexpr (reduceType == ReduceType::MAX) {
            WholeReduceMaxImpl<SrcPrimType, isSetMask>(
                (__ubuf__ DstPrimType*)dst.GetPhyAddr(), (__ubuf__ SrcPrimType*)src.GetPhyAddr(), mask, repeatTime,
                dstRepStride, srcBlkStride, srcRepStride, order);
        } else {
            WholeReduceMinImpl<SrcPrimType, isSetMask>(
                (__ubuf__ DstPrimType*)dst.GetPhyAddr(), (__ubuf__ SrcPrimType*)src.GetPhyAddr(), mask, repeatTime,
                dstRepStride, srcBlkStride, srcRepStride, order);
        }
    }
}

template <ReduceType reduceType>
__aicore__ inline constexpr const __gm__ char* GetReduceDataBlockApiName()
{
    if constexpr (reduceType == ReduceType::SUM) {
        return "ReduceDataBlock<SUM>";
    } else if constexpr (reduceType == ReduceType::MAX) {
        return "ReduceDataBlock<MAX>";
    } else {
        return "ReduceDataBlock<MIN>";
    }
}

template <ReduceType reduceType>
__aicore__ inline constexpr const __gm__ char* GetReduceRepeatApiName()
{
    if constexpr (reduceType == ReduceType::SUM) {
        return "ReduceRepeat<SUM>";
    } else if constexpr (reduceType == ReduceType::MAX) {
        return "ReduceRepeat<MAX>";
    } else {
        return "ReduceRepeat<MIN>";
    }
}

} // namespace Internal

/* *************** BlockReduceMax /BlockReduceMin /BlockReduceSum PairReduceSum ********************* */
/*
 * BlockReduceSum has been updated, please use ReduceDataBlock instead.
 * @ingroup BlockReduceSum
 * @brief Sum all elements in each block
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] repeatTime repeat times
 * @param [in] mask[]/maskcount mask array/count
 * @param [in] dstRepStride dst repeat stride
 * @param [in] srcBlkStride src block stride
 * @param [in] srcRepStride src repeat stride
 */
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "BlockReduceSum is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void BlockReduceSum(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t repeatTime, const int32_t mask,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    using PrimType = PrimT<T>;
    static_assert((SupportType<PrimType, half, float>()), "The data type is not supported by BlockReduceSum.");
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReduceBlkInfo(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, isSetMask, "BlockReduceSum");
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    Internal::CheckReduceDataBlockParams<T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride,
        "BlockReduceSum(deprecated, use ReduceDataBlock instead)");
#endif
    Internal::ReduceDataBlockCommon<ReduceType::SUM, T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride);
}

/*
 * BlockReduceMax has been updated, please use ReduceDataBlock instead.
 * @ingroup BlockReduceMax
 * @brief Maximize all elements in each block
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] repeatTime repeat times
 * @param [in] mask[]/maskcount mask array/count
 * @param [in] dstRepStride dst repeat stride
 * @param [in] srcBlkStride src block stride
 * @param [in] srcRepStride src repeat stride
 */
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "BlockReduceMax is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void BlockReduceMax(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t repeatTime, const int32_t mask,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    using PrimType = PrimT<T>;
    static_assert((SupportType<PrimType, half, float>()), "The data type is not supported by BlockReduceMax.");
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReduceBlkInfo(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, isSetMask, "BlockReduceMax");
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    Internal::CheckReduceDataBlockParams<T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride,
        "BlockReduceMax(deprecated, use ReduceDataBlock instead)");
#endif
    Internal::ReduceDataBlockCommon<ReduceType::MAX, T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride);
}

/*
 * BlockReduceMin has been updated, please use ReduceDataBlock instead.
 * @ingroup BlockReduceMin
 * @brief Find the minimum value of all elements in each block
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] repeatTime repeat times
 * @param [in] mask[]/maskcount mask array/count
 * @param [in] dstRepStride dst repeat stride
 * @param [in] srcBlkStride src block stride
 * @param [in] srcRepStride src repeat stride
 */
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "BlockReduceMin is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void BlockReduceMin(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t repeatTime, const int32_t mask,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    using PrimType = PrimT<T>;
    static_assert((SupportType<PrimType, half, float>()), "The data type is not supported by BlockReduceMin.");
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReduceBlkInfo(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, isSetMask, "BlockReduceMin");
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    Internal::CheckReduceDataBlockParams<T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride,
        "BlockReduceMin(deprecated, use ReduceDataBlock instead)");
#endif
    Internal::ReduceDataBlockCommon<ReduceType::MIN, T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride);
}

/*
 * PairReduceSum has been updated, please use ReducePairElem instead.
 * @ingroup PairReduceSum
 * @brief Sum of adjacent inner pair (parity) elements
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] repeatTime repeat times
 * @param [in] mask[]/maskcount mask array/count
 * @param [in] dstRepStride dst repeat stride
 * @param [in] srcBlkStride src block stride
 * @param [in] srcRepStride src repeat stride
 */
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "PairReduceSum is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void PairReduceSum(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t repeatTime, const int32_t mask,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    using PrimType = PrimT<T>;
    static_assert((SupportType<PrimType, half, float>()), "The data type is not supported by PairReduceSum.");
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReducePairInfo(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, isSetMask, "PairReduceSum");
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    Internal::CheckReducePairElemParams<T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride,
        "PairReduceSum(deprecated, use ReducePairElem instead)");
#endif
    Internal::ReducePairElemCommon<ReduceType::SUM, T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride);
}

// BlockReduceSum has been updated, please use ReduceDataBlock instead.
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "BlockReduceSum is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void BlockReduceSum(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t repeatTime, const uint64_t mask[],
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    using PrimType = PrimT<T>;
    static_assert((SupportType<PrimType, half, float>()), "The data type is not supported by BlockReduceSum.");
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReduceBlkInfo(
        dst, src, mask[0], mask[1], repeatTime, dstRepStride, srcBlkStride, srcRepStride, isSetMask, "BlockReduceSum");
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    Internal::CheckReduceDataBlockParams<T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride,
        "BlockReduceSum(deprecated, use ReduceDataBlock instead)");
#endif
    Internal::ReduceDataBlockCommon<ReduceType::SUM, T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride);
}

// BlockReduceMax has been updated, please use ReduceDataBlock instead.
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "BlockReduceMax is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void BlockReduceMax(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t repeatTime, const uint64_t mask[],
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    using PrimType = PrimT<T>;
    static_assert((SupportType<PrimType, half, float>()), "The data type is not supported by BlockReduceMax.");
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReduceBlkInfo(
        dst, src, mask[0], mask[1], repeatTime, dstRepStride, srcBlkStride, srcRepStride, isSetMask, "BlockReduceMax");
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    Internal::CheckReduceDataBlockParams<T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride,
        "BlockReduceMax(deprecated, use ReduceDataBlock instead)");
#endif
    Internal::ReduceDataBlockCommon<ReduceType::MAX, T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride);
}

// BlockReduceMin has been updated, please use ReduceDataBlock instead.
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "BlockReduceMin is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void BlockReduceMin(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t repeatTime, const uint64_t mask[],
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    using PrimType = PrimT<T>;
    static_assert((SupportType<PrimType, half, float>()), "The data type is not supported by BlockReduceMin.");
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReduceBlkInfo(
        dst, src, mask[0], mask[1], repeatTime, dstRepStride, srcBlkStride, srcRepStride, isSetMask, "BlockReduceMin");
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    Internal::CheckReduceDataBlockParams<T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride,
        "BlockReduceMin(deprecated, use ReduceDataBlock instead)");
#endif
    Internal::ReduceDataBlockCommon<ReduceType::MIN, T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride);
}

// PairReduceSum has been updated, please use ReducePairElem instead.
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "PairReduceSum is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void PairReduceSum(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t repeatTime, const uint64_t mask[],
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    using PrimType = PrimT<T>;
    static_assert((SupportType<PrimType, half, float>()), "The data type is not supported by PairReduceSum.");
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReducePairInfo(
        dst, src, mask[0], mask[1], repeatTime, dstRepStride, srcBlkStride, srcRepStride, isSetMask, "PairReduceSum");
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    Internal::CheckReducePairElemParams<T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride,
        "PairReduceSum(deprecated, use ReducePairElem instead)");
#endif
    Internal::ReducePairElemCommon<ReduceType::SUM, T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride);
}

#if defined(__NPU_ARCH__) && \
    ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
// RepeatReduceSum has been updated, please use ReduceRepeat instead.
template <typename T, bool isSetMask = true, typename U = T>
__ASC_USE_RESERVED_UBUF__(
    3510, "RepeatReduceSum is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void RepeatReduceSum(
    const LocalTensor<U>& dst, const LocalTensor<T>& src, const int32_t repeatTime, const int32_t mask,
    const int32_t dstBlkStride, const int32_t srcBlkStride, const int32_t dstRepStride, const int32_t srcRepStride)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    using SrcPrimType = PrimT<T>;
    using DstPrimType = PrimT<U>;
    static_assert(
        (SupportType<SrcPrimType, int16_t, uint16_t, int32_t, uint32_t, half, float>()),
        "The source data type is not supported by RepeatReduceSum.");
    static_assert(
        (SupportType<DstPrimType, int32_t, uint32_t, half, float>()),
        "The destination data type is not supported by RepeatReduceSum.");
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReduceInfo(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, isSetMask, "RepeatReduceSum");
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    Internal::CheckReduceRepeatParams<ReduceType::SUM, U, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride,
        "RepeatReduceSum(deprecated, use ReduceRepeat instead)");
#endif
    // the typename in ReduceRepeat is reversed for better readability of dst/src
    Internal::ReduceRepeatCommon<ReduceType::SUM, U, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride);
}
#else
// RepeatReduceSum has been updated, please use ReduceRepeat instead.
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "RepeatReduceSum is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void RepeatReduceSum(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t repeatTime, const int32_t mask,
    const int32_t dstBlkStride, const int32_t srcBlkStride, const int32_t dstRepStride, const int32_t srcRepStride)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReduceInfo(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, isSetMask, "RepeatReduceSum");
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    Internal::CheckReduceRepeatParams<ReduceType::SUM, T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride,
        "RepeatReduceSum(deprecated, use ReduceRepeat instead)");
#endif
    Internal::ReduceRepeatCommon<ReduceType::SUM, T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride);
}
#endif

/* **************************************** Whole Reduce Interface ****************************************** */
/*
 * WholeReduceSum has been updated, please use ReduceRepeat instead.
 * @ingroup WholeReduceSum
 * @brief Sum of all effective elements in each repeatTime
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] repeatTime repeat times
 * @param [in] mask[]/maskcount mask array/count
 * @param [in] dstRepStride dst repeat stride
 * @param [in] srcBlkStride src block stride
 * @param [in] srcRepStride src repeat stride
 */
#if defined(__NPU_ARCH__) && \
    ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
template <typename T, bool isSetMask = true, typename U = T>
__ASC_USE_RESERVED_UBUF__(
    3510, "WholeReduceSum is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void WholeReduceSum(
    const LocalTensor<U>& dst, const LocalTensor<T>& src, const uint64_t mask[], const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    using SrcPrimType = PrimT<T>;
    using DstPrimType = PrimT<U>;
    static_assert(
        (SupportType<SrcPrimType, int16_t, uint16_t, int32_t, uint32_t, half, float>()),
        "The source data type is not supported by WholeReduceSum.");
    static_assert(
        (SupportType<DstPrimType, int32_t, uint32_t, half, float>()),
        "The destination data type is not supported by WholeReduceSum.");
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReduceInfo(
        dst, src, mask[0], mask[1], repeatTime, dstRepStride, srcBlkStride, srcRepStride, isSetMask, "WholeReduceSum");
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    Internal::CheckReduceRepeatParams<ReduceType::SUM, U, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride,
        "WholeReduceSum(deprecated, use ReduceRepeat instead)");
#endif
    // the typename in ReduceRepeat is reversed for better readability of dst/src
    Internal::ReduceRepeatCommon<ReduceType::SUM, U, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride);
}
#else
// WholeReduceSum has been updated, please use ReduceRepeat instead.
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "WholeReduceSum is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void WholeReduceSum(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint64_t mask[], const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReduceInfo(
        dst, src, mask[0], mask[1], repeatTime, dstRepStride, srcBlkStride, srcRepStride, isSetMask, "WholeReduceSum");
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    Internal::CheckReduceRepeatParams<ReduceType::SUM, T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride,
        "WholeReduceSum(deprecated, use ReduceRepeat instead)");
#endif
    Internal::ReduceRepeatCommon<ReduceType::SUM, T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride);
}
#endif

/*
 * WholeReduceMax has been updated, please use ReduceRepeat instead.
 * @ingroup WholeReduceMax
 * @brief Index of the maximum value of all elements in each repeat
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] repeatTime repeat times
 * @param [in] mask[]/maskcount mask array/count
 * @param [in] dstRepStride dst repeat stride
 * @param [in] srcBlkStride src block stride
 * @param [in] srcRepStride src repeat stride
 */
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "WholeReduceMax is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void WholeReduceMax(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint64_t mask[], const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride,
    ReduceOrder order = ReduceOrder::ORDER_VALUE_INDEX)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    using PrimType = PrimT<T>;
    static_assert(
        (SupportType<PrimType, int16_t, uint16_t, int32_t, uint32_t, half, float>()),
        "The data type is not supported by WholeReduceMax.");
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReduceInfo(
        dst, src, mask[0], mask[1], repeatTime, dstRepStride, srcBlkStride, srcRepStride, isSetMask, "WholeReduceMax");
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    Internal::CheckReduceRepeatParams<ReduceType::MAX, T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, order,
        "WholeReduceMax(deprecated, use ReduceRepeat instead)");
#endif
    Internal::ReduceRepeatCommon<ReduceType::MAX, T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, order);
}

// WholeReduceMin has been updated, please use ReduceRepeat instead.
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "WholeReduceMin is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void WholeReduceMin(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint64_t mask[], const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride,
    ReduceOrder order = ReduceOrder::ORDER_VALUE_INDEX)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    using PrimType = PrimT<T>;
    static_assert(
        (SupportType<PrimType, int16_t, uint16_t, int32_t, uint32_t, half, float>()),
        "The data type is not supported by WholeReduceMin.");
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReduceInfo(
        dst, src, mask[0], mask[1], repeatTime, dstRepStride, srcBlkStride, srcRepStride, isSetMask, "WholeReduceMin");
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    Internal::CheckReduceRepeatParams<ReduceType::MIN, T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, order,
        "WholeReduceMin(deprecated, use ReduceRepeat instead)");
#endif
    Internal::ReduceRepeatCommon<ReduceType::MIN, T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, order);
}

#if defined(__NPU_ARCH__) && \
    ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
// WholeReduceSum has been updated, please use ReduceRepeat instead.
template <typename T, bool isSetMask = true, typename U = T>
__ASC_USE_RESERVED_UBUF__(
    3510, "WholeReduceSum is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void WholeReduceSum(
    const LocalTensor<U>& dst, const LocalTensor<T>& src, const int32_t mask, const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    using SrcPrimType = PrimT<T>;
    using DstPrimType = PrimT<U>;
    static_assert(
        (SupportType<SrcPrimType, int16_t, uint16_t, int32_t, uint32_t, half, float>()),
        "The source data type is not supported by WholeReduceSum.");
    static_assert(
        (SupportType<DstPrimType, int32_t, uint32_t, half, float>()),
        "The destination data type is not supported by WholeReduceSum.");
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReduceInfo(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, isSetMask, "WholeReduceSum");
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    Internal::CheckReduceRepeatParams<ReduceType::SUM, U, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride,
        "WholeReduceSum(deprecated, use ReduceRepeat instead)");
#endif
    // the typename in ReduceRepeat is reversed for better readability of dst/src
    Internal::ReduceRepeatCommon<ReduceType::SUM, U, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride);
}
#else
// WholeReduceSum has been updated, please use ReduceRepeat instead.
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "WholeReduceSum is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void WholeReduceSum(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t mask, const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReduceInfo(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, isSetMask, "WholeReduceSum");
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    Internal::CheckReduceRepeatParams<ReduceType::SUM, T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride,
        "WholeReduceSum(deprecated, use ReduceRepeat instead)");
#endif
    Internal::ReduceRepeatCommon<ReduceType::SUM, T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride);
}
#endif

// WholeReduceMax has been updated, please use ReduceRepeat instead.
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "WholeReduceMax is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void WholeReduceMax(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t mask, const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride,
    ReduceOrder order = ReduceOrder::ORDER_VALUE_INDEX)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    using PrimType = PrimT<T>;
    static_assert(
        (SupportType<PrimType, int16_t, uint16_t, int32_t, uint32_t, half, float>()),
        "The data type is not supported by WholeReduceMax.");
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReduceInfo(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, isSetMask, "WholeReduceMax");
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    Internal::CheckReduceRepeatParams<ReduceType::MAX, T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, order,
        "WholeReduceMax(deprecated, use ReduceRepeat instead)");
#endif
    Internal::ReduceRepeatCommon<ReduceType::MAX, T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, order);
}

// WholeReduceMin has been updated, please use ReduceRepeat instead.
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "WholeReduceMin is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void WholeReduceMin(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t mask, const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride,
    ReduceOrder order = ReduceOrder::ORDER_VALUE_INDEX)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    using PrimType = PrimT<T>;
    static_assert(
        (SupportType<PrimType, int16_t, uint16_t, int32_t, uint32_t, half, float>()),
        "The data type is not supported by WholeReduceMin.");
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReduceInfo(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, isSetMask, "WholeReduceMin");
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    Internal::CheckReduceRepeatParams<ReduceType::MIN, T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, order,
        "WholeReduceMin(deprecated, use ReduceRepeat instead)");
#endif
    Internal::ReduceRepeatCommon<ReduceType::MIN, T, T, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, order);
}

#if defined(__NPU_ARCH__)
template <ReduceType reduceType, typename T, typename U, bool isSetMask>
__aicore__ inline void ReduceDataBlock(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const int32_t mask, const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReduceBlkInfo(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, isSetMask, "ReduceDataBlock");
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    Internal::CheckReduceDataBlockParams<T, U, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride,
        Internal::GetReduceDataBlockApiName<reduceType>());
#endif
    Internal::ReduceDataBlockCommon<reduceType, T, U, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride);
}

template <ReduceType reduceType, typename T, typename U, bool isSetMask>
__aicore__ inline void ReduceDataBlock(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint64_t mask[], const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReduceBlkInfo(
        dst, src, mask[0], mask[1], repeatTime, dstRepStride, srcBlkStride, srcRepStride, isSetMask, "ReduceDataBlock");
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    Internal::CheckReduceDataBlockParams<T, U, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride,
        Internal::GetReduceDataBlockApiName<reduceType>());
#endif
    Internal::ReduceDataBlockCommon<reduceType, T, U, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride);
}

template <ReduceType reduceType, typename T, typename U, bool isSetMask>
__aicore__ inline void ReducePairElem(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const int32_t mask, const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReducePairInfo(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, isSetMask, "ReducePairElem");
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    Internal::CheckReducePairElemParams<T, U, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, "ReducePairElem<SUM>");
#endif
    Internal::ReducePairElemCommon<reduceType, T, U, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride);
}

template <ReduceType reduceType, typename T, typename U, bool isSetMask>
__aicore__ inline void ReducePairElem(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint64_t mask[], const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReducePairInfo(
        dst, src, mask[0], mask[1], repeatTime, dstRepStride, srcBlkStride, srcRepStride, isSetMask, "ReducePairElem");
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    Internal::CheckReducePairElemParams<T, U, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, "ReducePairElem<SUM>");
#endif
    Internal::ReducePairElemCommon<reduceType, T, U, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride);
}

template <ReduceType reduceType, typename T, typename U, bool isSetMask>
__aicore__ inline void ReduceRepeat(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const int32_t mask, const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, ReduceOrder order)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReduceInfo(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, isSetMask, "ReduceRepeat");
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    Internal::CheckReduceRepeatParams<reduceType, T, U, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, order,
        Internal::GetReduceRepeatApiName<reduceType>());
#endif
    Internal::ReduceRepeatCommon<reduceType, T, U, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, order);
}

template <ReduceType reduceType, typename T, typename U, bool isSetMask>
__aicore__ inline void ReduceRepeat(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint64_t mask[], const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, ReduceOrder order)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReduceInfo(
        dst, src, mask[0], mask[1], repeatTime, dstRepStride, srcBlkStride, srcRepStride, isSetMask, "ReduceRepeat");
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    Internal::CheckReduceRepeatParams<reduceType, T, U, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, order,
        Internal::GetReduceRepeatApiName<reduceType>());
#endif
    Internal::ReduceRepeatCommon<reduceType, T, U, isSetMask>(
        dst, src, mask, repeatTime, dstRepStride, srcBlkStride, srcRepStride, order);
}

/* **************************************** Reduce Interface ****************************************** */
/*
 * @ingroup ReduceMax Level 0
 * @brief Index of the maximum value of all input elements
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] sharedTmpBuffer LocalTensor to store the intermediate results
 * @param [in] repeatTime repeat times
 * @param [in] mask[]/maskcount mask array/count
 * @param [in] srcRepStride src repeat stride
 * @param [in] calIndex Specify whether to get the index with the highest value
 */
template <typename T>
__aicore__ inline void ReduceMax(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const int32_t mask,
    const int32_t repeatTime, const int32_t srcRepStride, bool calIndex)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReduceComplexInfo(dst, src, sharedTmpBuffer, mask, repeatTime, srcRepStride, "ReduceMax");
#endif
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecReduce(dst, src, sharedTmpBuffer, repeatTime, mask, calIndex, srcRepStride, "ReduceMax")) {
        ASCENDC_REPORT_CHECK_ERROR("ReduceMax", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#if defined(__NPU_ARCH__) && \
    ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
    ReduceMaxImpl<PrimType>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(),
        (__ubuf__ PrimType*)sharedTmpBuffer.GetPhyAddr(), mask, repeatTime, srcRepStride, calIndex);
#else
    ASCENDC_ASSERT((SupportType<PrimType, half, float>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in ReduceMax, "
                          "current api support dtype combination is src and dst both: half / float");
    });
    if (mask == 0) {
        return;
    }
    ReduceRepeatParams params(mask, repeatTime, DEFAULT_REDUCE_DST_REP_STRIDE, DEFAULT_BLK_STRIDE, srcRepStride);

    ReduceImpl<PrimType>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(),
        (__ubuf__ PrimType*)sharedTmpBuffer.GetPhyAddr(), params, calIndex, ReduceMode::REDUCE_MAX);
#endif
}

/*
 * @ingroup ReduceMin
 * @brief Index of the minimum value of all input elements
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] sharedTmpBuffer LocalTensor to store the intermediate results
 * @param [in] repeatTime repeat times
 * @param [in] mask[]/maskcount mask array/count
 * @param [in] srcRepStride src repeat stride
 * @param [in] calIndex Specify whether to get the index with the highest value
 */
template <typename T>
__aicore__ inline void ReduceMin(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const int32_t mask,
    const int32_t repeatTime, const int32_t srcRepStride, bool calIndex)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReduceComplexInfo(dst, src, sharedTmpBuffer, mask, repeatTime, srcRepStride, "ReduceMin");
#endif
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecReduce(dst, src, sharedTmpBuffer, repeatTime, mask, calIndex, srcRepStride, "ReduceMin")) {
        ASCENDC_REPORT_CHECK_ERROR("ReduceMin", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113)
    ReduceMinImpl<PrimType>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(),
        (__ubuf__ PrimType*)sharedTmpBuffer.GetPhyAddr(), mask, repeatTime, srcRepStride, calIndex);
#else
    ASCENDC_ASSERT((SupportType<PrimType, half, float>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in ReduceMin, "
                          "current api support dtype combination is src and dst both: half / float");
    });
    if (mask == 0) {
        return;
    }
    struct ReduceRepeatParams params(mask, repeatTime, DEFAULT_REDUCE_DST_REP_STRIDE, DEFAULT_BLK_STRIDE, srcRepStride);

    ReduceImpl<PrimType>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(),
        (__ubuf__ PrimType*)sharedTmpBuffer.GetPhyAddr(), params, calIndex, ReduceMode::REDUCE_MIN);
#endif
}

/*
 * @ingroup ReduceSum
 * @brief sum all input elements
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] sharedTmpBuffer LocalTensor to store the intermediate results
 * @param [in] repeatTime repeat times
 * @param [in] mask[]/maskcount mask array/count
 * @param [in] srcRepStride src repeat stride
 */
template <typename T>
__aicore__ inline void ReduceSum(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const int32_t mask,
    const int32_t repeatTime, const int32_t srcRepStride)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReduceComplexInfo(dst, src, sharedTmpBuffer, mask, repeatTime, srcRepStride, "ReduceSum");
#endif
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecReduce(dst, src, sharedTmpBuffer, repeatTime, mask, srcRepStride, "ReduceSum")) {
        ASCENDC_REPORT_CHECK_ERROR("ReduceSum", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113)
    ReduceSumImpl<PrimType>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(),
        (__ubuf__ PrimType*)sharedTmpBuffer.GetPhyAddr(), mask, repeatTime, srcRepStride);
#else
    ASCENDC_ASSERT((SupportType<PrimType, half, float>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in ReduceSum, "
                          "current api support dtype combination is src and dst both: half / float");
    });
    if (mask == 0) {
        return;
    }
    ReduceRepeatParams params(mask, repeatTime, DEFAULT_REDUCE_DST_REP_STRIDE, DEFAULT_BLK_STRIDE, srcRepStride);

    ReduceImpl<PrimType>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(),
        (__ubuf__ PrimType*)sharedTmpBuffer.GetPhyAddr(), params, 0, ReduceMode::REDUCE_SUM);
#endif
}

template <typename T>
__aicore__ inline void ReduceMax(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const uint64_t mask[],
    const int32_t repeatTime, const int32_t srcRepStride, bool calIndex)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReduceComplexInfo(
        dst, src, sharedTmpBuffer, mask[0], mask[1], repeatTime, srcRepStride, "ReduceMax");
#endif
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecReduce(dst, src, sharedTmpBuffer, repeatTime, mask, calIndex, srcRepStride, "ReduceMax")) {
        ASCENDC_REPORT_CHECK_ERROR("ReduceMax", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#if defined(__NPU_ARCH__) && \
    ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
    ReduceMaxImpl<PrimType>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(),
        (__ubuf__ PrimType*)sharedTmpBuffer.GetPhyAddr(), mask, repeatTime, srcRepStride, calIndex);
#else
    ASCENDC_ASSERT((SupportType<PrimType, half, float>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in ReduceMax, "
                          "current api support dtype combination is src and dst both: half / float");
    });
    if (mask[0] == 0 && mask[1] == 0) {
        return;
    }
    struct ReduceRepeatParams params(mask, repeatTime, DEFAULT_REDUCE_DST_REP_STRIDE, DEFAULT_BLK_STRIDE, srcRepStride);

    ReduceImpl<PrimType>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(),
        (__ubuf__ PrimType*)sharedTmpBuffer.GetPhyAddr(), params, calIndex, ReduceMode::REDUCE_MAX);
#endif
}

template <typename T>
__aicore__ inline void ReduceMin(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const uint64_t mask[],
    const int32_t repeatTime, const int32_t srcRepStride, bool calIndex)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReduceComplexInfo(
        dst, src, sharedTmpBuffer, mask[0], mask[1], repeatTime, srcRepStride, "ReduceMin");
#endif
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecReduce(dst, src, sharedTmpBuffer, repeatTime, mask, calIndex, srcRepStride, "ReduceMin")) {
        ASCENDC_REPORT_CHECK_ERROR("ReduceMin", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113)
    ReduceMinImpl<PrimType>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(),
        (__ubuf__ PrimType*)sharedTmpBuffer.GetPhyAddr(), mask, repeatTime, srcRepStride, calIndex);
#else
    ASCENDC_ASSERT((SupportType<PrimType, half, float>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in ReduceMin, "
                          "current api support dtype combination is src and dst both: half / float");
    });
    if (mask[0] == 0 && mask[1] == 0) {
        return;
    }
    struct ReduceRepeatParams params(mask, repeatTime, DEFAULT_REDUCE_DST_REP_STRIDE, DEFAULT_BLK_STRIDE, srcRepStride);

    ReduceImpl<PrimType>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(),
        (__ubuf__ PrimType*)sharedTmpBuffer.GetPhyAddr(), params, calIndex, ReduceMode::REDUCE_MIN);
#endif
}

template <typename T>
__aicore__ inline void ReduceSum(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const uint64_t mask[],
    const int32_t repeatTime, const int32_t srcRepStride)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReduceComplexInfo(
        dst, src, sharedTmpBuffer, mask[0], mask[1], repeatTime, srcRepStride, "ReduceSum");
#endif
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecReduce(dst, src, sharedTmpBuffer, repeatTime, mask, srcRepStride, "ReduceSum")) {
        ASCENDC_REPORT_CHECK_ERROR("ReduceSum", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113)
    ReduceSumImpl<PrimType>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(),
        (__ubuf__ PrimType*)sharedTmpBuffer.GetPhyAddr(), mask, repeatTime, srcRepStride);
#else
    ASCENDC_ASSERT((SupportType<PrimType, half, float>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in ReduceSum, "
                          "current api support dtype combination is src and dst both: half / float");
    });
    if (mask[0] == 0 && mask[1] == 0) {
        return;
    }
    struct ReduceRepeatParams params(mask, repeatTime, DEFAULT_REDUCE_DST_REP_STRIDE, DEFAULT_BLK_STRIDE, srcRepStride);

    ReduceImpl<PrimType>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(),
        (__ubuf__ PrimType*)sharedTmpBuffer.GetPhyAddr(), params, 0, ReduceMode::REDUCE_SUM);
#endif
}

/*
 * @ingroup ReduceMin Level 2
 * @brief Index of the minimum value of all input elements
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] sharedTmpBuffer LocalTensor to store the intermediate results
 * @param [in] count Number of data involved in calculation
 * @param [in] calIndex Specify whether to get the index with the highest value
 */
template <typename T>
__aicore__ inline void ReduceMin(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const int32_t count,
    bool calIndex)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReduceComplexInfo(dst, src, sharedTmpBuffer, count, "ReduceMin");
#endif
    using PrimType = PrimT<T>;
#if defined(__NPU_ARCH__) && \
    ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
#if ASCENDC_CPU_DEBUG
    int32_t oneRepSize = ONE_REPEAT_BYTE_SIZE / sizeof(PrimType);
    int32_t repeats = count < oneRepSize ? 1 : (count / oneRepSize);
    if (!CheckFunVecReduce(dst, src, sharedTmpBuffer, repeats, count, calIndex, "vreduce_min")) {
        ASSERT(false && "check vreduce min instr failed");
    }
#endif
    ReduceMinImpl<PrimType>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(),
        (__ubuf__ PrimType*)sharedTmpBuffer.GetPhyAddr(), count, calIndex);
#else
    ASCENDC_ASSERT((SupportType<PrimType, half, float>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in ReduceMin, "
                          "current api support dtype combination is src and dst both: half / float");
    });
    ASCENDC_CHECK_VALUE_RANGE(count, 1, TOTAL_UB_SIZE / sizeof(PrimType), "count", "ReduceMin");
    int32_t elementNumPerRep = ONE_REPEAT_BYTE_SIZE / sizeof(PrimType); // fp16=128 , fp32=64
    int32_t repeatTime = count / elementNumPerRep;
    int32_t tailCount = count % elementNumPerRep; // tailCount  <= 128/64 repeatTime=1
    int32_t bodyCount = elementNumPerRep;

    if (repeatTime == 0) { // if count < elementNumPerRep ,repeatTime will be 0
        repeatTime = 1;
        bodyCount = count;
        tailCount = 0;
    }
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecReduce(dst, src, sharedTmpBuffer, repeatTime, count, calIndex, "ReduceMin")) {
        ASCENDC_REPORT_CHECK_ERROR("ReduceMin", KernelFuncType::NONE_MODE);
    }
#endif
    if (count == 0) {
        return;
    }
    struct ReduceRepeatParams params(
        bodyCount, repeatTime, DEFAULT_REDUCE_DST_REP_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    ReduceImpl<PrimType>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(),
        (__ubuf__ PrimType*)sharedTmpBuffer.GetPhyAddr(), params, calIndex, ReduceMode::REDUCE_MIN);

    if (tailCount != 0) {
        ReduceTailCompute(dst, src, sharedTmpBuffer, count, calIndex, ReduceMode::REDUCE_MIN);
    }
#endif
}

/*
 * @ingroup ReduceMax Level 2
 * @brief Index of the maximum value of all input elements
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] sharedTmpBuffer LocalTensor to store the intermediate results
 * @param [in] count Number of data involved in calculation
 * @param [in] calIndex Specify whether to get the index with the highest value
 */
template <typename T>
__aicore__ inline void ReduceMax(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const int32_t count,
    bool calIndex)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReduceComplexInfo(dst, src, sharedTmpBuffer, count, "ReduceMax");
#endif
    using PrimType = PrimT<T>;
#if defined(__NPU_ARCH__) && \
    ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
#if ASCENDC_CPU_DEBUG
    int32_t oneRepSize = ONE_REPEAT_BYTE_SIZE / sizeof(PrimType);
    int32_t repeats = count < oneRepSize ? 1 : (count / oneRepSize);
    if (!CheckFunVecReduce(dst, src, sharedTmpBuffer, repeats, count, calIndex, "vreduce_max")) {
        ASSERT(false && "check vreduce max instr failed");
    }
#endif
    ReduceMaxImpl<PrimType>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(),
        (__ubuf__ PrimType*)sharedTmpBuffer.GetPhyAddr(), count, calIndex);
#else
    ASCENDC_ASSERT((SupportType<PrimType, half, float>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in ReduceMax, "
                          "current api support dtype combination is src and dst both: half / float");
    });
    ASCENDC_CHECK_VALUE_RANGE(count, 1, TOTAL_UB_SIZE / sizeof(PrimType), "count", "ReduceMax");
    int32_t elementNumPerRep = ONE_REPEAT_BYTE_SIZE / sizeof(PrimType); // fp16=128 , fp32=64
    int32_t repeatTime = count / elementNumPerRep;
    int32_t tailCount = count % elementNumPerRep; // tailCount  <= 128/64 repeatTime=1
    int32_t bodyCount = elementNumPerRep;

    if (repeatTime == 0) { // if count < elementNumPerRep ,repeatTime will be 0
        repeatTime = 1;
        bodyCount = count;
        tailCount = 0;
    }
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecReduce(dst, src, sharedTmpBuffer, repeatTime, count, calIndex, "ReduceMax")) {
        ASCENDC_REPORT_CHECK_ERROR("ReduceMax", KernelFuncType::NONE_MODE);
    }
#endif
    if (count == 0) {
        return;
    }

    struct ReduceRepeatParams params(
        bodyCount, repeatTime, DEFAULT_REDUCE_DST_REP_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    ReduceImpl<PrimType>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(),
        (__ubuf__ PrimType*)sharedTmpBuffer.GetPhyAddr(), params, calIndex, ReduceMode::REDUCE_MAX);

    if (tailCount != 0) {
        ReduceTailCompute(dst, src, sharedTmpBuffer, count, calIndex, ReduceMode::REDUCE_MAX);
    }
#endif
}

/*
 * @ingroup ReduceSum Level 2
 * @brief sum all input elements
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] sharedTmpBuffer LocalTensor to store the intermediate results
 * @param [in] count Number of data involved in calculation
 */
template <typename T, bool isSetMask>
__aicore__ inline void ReduceSum(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const int32_t count)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecReduceComplexInfo(dst, src, sharedTmpBuffer, count, "ReduceSum");
#endif
    using PrimType = PrimT<T>;
    ASCENDC_CHECK_VALUE_RANGE(count, 1, TOTAL_UB_SIZE / sizeof(PrimType), "count", "ReduceSum");
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
    ASCENDC_ASSERT((SupportType<PrimType, half, float>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in ReduceSum, "
                          "current api support dtype combination is src and dst both: half / float");
    });
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecReduceMode2(dst, src, count, "ReduceSum")) {
        ASCENDC_REPORT_CHECK_ERROR("ReduceSum", KernelFuncType::CALCOUNT_MODE);
    }
#endif
    if (count == 0) {
        return;
    }
    ReduceSumImpl<PrimType, isSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), count);
#elif (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113)
#if ASCENDC_CPU_DEBUG
    int32_t oneRepSize = ONE_REPEAT_BYTE_SIZE / sizeof(PrimType);
    int32_t repeats = count < oneRepSize ? 1 : (count / oneRepSize);
    if (!CheckFunVecReduce(dst, src, sharedTmpBuffer, count, repeats, "vreduce_sum")) {
        ASSERT(false && "check vreduce sum instr failed");
    }
#endif
    ReduceSumImpl<PrimType>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(),
        (__ubuf__ PrimType*)sharedTmpBuffer.GetPhyAddr(), count);
#elif defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
    ReduceSumImpl<PrimType>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(),
        (__ubuf__ PrimType*)sharedTmpBuffer.GetPhyAddr(), count);
#else // other version
    ASCENDC_ASSERT((SupportType<PrimType, half, float>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in ReduceSum, "
                          "current api support dtype combination is src and dst both: half / float");
    });
    int32_t elementNumPerRep = ONE_REPEAT_BYTE_SIZE / sizeof(PrimType); // fp16=128 , fp32=64
    int32_t repeatTime = count / elementNumPerRep;
    int32_t tailCount = count % elementNumPerRep; // tailCount  <= 128/64 repeatTime=1
    int32_t bodyCount = elementNumPerRep;

    if (repeatTime == 0) { // if count < elementNumPerRep ,repeatTime will be 0
        repeatTime = 1;
        bodyCount = count;
        tailCount = 0;
    }
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecReduce(dst, src, sharedTmpBuffer, count, repeatTime, "ReduceSum")) {
        ASCENDC_REPORT_CHECK_ERROR("ReduceSum", KernelFuncType::CALCOUNT_MODE);
    }
#endif
    if (count == 0) {
        return;
    }
    struct ReduceRepeatParams params(
        bodyCount, repeatTime, DEFAULT_REDUCE_DST_REP_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    ReduceImpl<PrimType>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(),
        (__ubuf__ PrimType*)sharedTmpBuffer.GetPhyAddr(), params, 0, ReduceMode::REDUCE_SUM);
    event_t eventIdVToS = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_S));
    SetFlag<HardEvent::V_S>(eventIdVToS);
    WaitFlag<HardEvent::V_S>(eventIdVToS);
    PrimType bodySumValue = dst.GetValue(0);

    if (tailCount != 0) {
        struct ReduceRepeatParams tailParams(
            tailCount, 1, DEFAULT_REDUCE_DST_REP_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);

        ReduceImpl<PrimType>(
            (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(elementNumPerRep * repeatTime),
            (__ubuf__ PrimType*)sharedTmpBuffer.GetPhyAddr(), tailParams, 0, ReduceMode::REDUCE_SUM);
        event_t eventIdVToS1 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_S));
        SetFlag<HardEvent::V_S>(eventIdVToS1);
        WaitFlag<HardEvent::V_S>(eventIdVToS1);
        PrimType tailSumValue = dst.GetValue(0);

        sharedTmpBuffer.SetValue(0, bodySumValue);
        sharedTmpBuffer.SetValue(1, tailSumValue); // bodyresult tailresult vcadd again
        event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
        SetFlag<HardEvent::S_V>(eventIdSToV);
        WaitFlag<HardEvent::S_V>(eventIdSToV);
        struct ReduceRepeatParams newParams(
            2, 1, DEFAULT_REDUCE_DST_REP_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);

        ReduceImpl<PrimType>(
            (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)sharedTmpBuffer.GetPhyAddr(),
            (__ubuf__ PrimType*)sharedTmpBuffer.GetPhyAddr(), newParams, 0, ReduceMode::REDUCE_SUM);
    }
#endif
}
#endif
#pragma end_pipe

// GetReduceMaxMinCount has been updated, please use GetReduceRepeatMaxMinSpr instead.
template <typename T>
__aicore__ inline void GetReduceMaxMinCount(uint32_t& maxMinValue, uint32_t& maxMinIndex)
{
    using PrimType = PrimT<T>;
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
    if (g_coreType == AIC) {
        return;
    }
#endif
    GetReduceMaxMinCountImpl<PrimType>(maxMinValue, maxMinIndex);
}

// GetReduceMaxMinCount has been updated, please use GetReduceRepeatMaxMinSpr instead.
template <typename T>
__aicore__ inline void GetReduceMaxMinCount(uint32_t& maxMinValue)
{
    using PrimType = PrimT<T>;
    GetReduceMaxMinCountImpl<PrimType>(maxMinValue);
}

template <typename T>
__aicore__ inline void GetReduceRepeatMaxMinSpr(uint32_t& maxMinValue, uint32_t& maxMinIndex)
{
    using PrimType = PrimT<T>;
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
    if (g_coreType == AIC) {
        return;
    }
#endif
    GetReduceMaxMinCountImpl<PrimType>(maxMinValue, maxMinIndex);
}

template <typename T>
__aicore__ inline void GetReduceRepeatMaxMinSpr(uint32_t& maxMinValue)
{
    using PrimType = PrimT<T>;
    GetReduceMaxMinCountImpl<PrimType>(maxMinValue);
}

__aicore__ inline int64_t GetReduceRepeatSumSpr()
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
    if (g_coreType == AIC) {
        return 0;
    }
#endif
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    return GetAccValImpl<int64_t>();
#else
    return get_acc_val();
#endif
}

// GetAccVal has been updated, please use GetReduceRepeatSumSpr instead.
__aicore__ inline int64_t GetAccVal() { return GetReduceRepeatSumSpr(); }

// GetReduceMaxMinCount has been updated, please use GetReduceRepeatMaxMinSpr instead.
template <typename T>
__aicore__ inline __inout_pipe__(S) void GetReduceMaxMinCount(T& maxMinValue, T& maxMinIndex)
{
    ASCENDC_ASSERT((SupportType<T, half, float>()), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "Failed to check dtype in "
            "GetReduceMaxMinCount, current api support dtype combination is maxMinValue and maxMinIndex both: half / "
            "float");
    });
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
    if (g_coreType == AIC) {
        return;
    }
#endif
    GetReduceMaxMinCountImpl<T>(maxMinValue, maxMinIndex);
}

// GetReduceMaxMinCount has been updated, please use GetReduceRepeatMaxMinSpr instead.
template <typename T>
__aicore__ inline __inout_pipe__(S) void GetReduceMaxMinCount(T& maxMinValue)
{
    ASCENDC_ASSERT((SupportType<T, half, float>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in "
                          "GetReduceMaxMinCount, current api support dtype combination is maxMinValue: half / float");
    });
    GetReduceMaxMinCountImpl<T>(maxMinValue);
}

template <typename T>
__aicore__ inline __inout_pipe__(S) void GetReduceRepeatMaxMinSpr(T& maxMinValue, T& maxMinIndex)
{
    ASCENDC_ASSERT((SupportType<T, half, float>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in "
                          "GetReduceRepeatMaxMinSpr, current api support dtype combination is maxMinValue and "
                          "maxMinIndex both: half / "
                          "float");
    });
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
    if (g_coreType == AIC) {
        return;
    }
#endif
    GetReduceMaxMinCountImpl<T>(maxMinValue, maxMinIndex);
}

template <typename T>
__aicore__ inline __inout_pipe__(S) void GetReduceRepeatMaxMinSpr(T& maxMinValue)
{
    ASCENDC_ASSERT((SupportType<T, half, float>()), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "Failed to check dtype in "
            "GetReduceRepeatMaxMinSpr, current api support dtype combination is maxMinValue: half / float");
    });
    GetReduceMaxMinCountImpl<T>(maxMinValue);
}

template <typename T>
__aicore__ inline __inout_pipe__(S) T GetReduceRepeatSumSpr()
{
    ASCENDC_ASSERT((SupportType<T, half, float>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in GetReduceRepeatSumSpr, "
                          "current api support dtype combination is half / float");
    });
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
    if (g_coreType == AIC) {
        return 0;
    }
#endif
    return GetAccValImpl<T>();
}

// GetAccVal has been updated, please use GetReduceRepeatSumSpr instead.
template <typename T>
__aicore__ inline __inout_pipe__(S) T GetAccVal()
{
    ASCENDC_ASSERT((SupportType<T, half, float>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in GetAccVal, "
                          "current api support dtype combination is half / float");
    });
    return GetReduceRepeatSumSpr<T>();
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_REDUCE_INTERFACE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_REDUCE_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_REDUCE_INTF_IMPL_H__
#endif
