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
 * \file kernel_operator_vec_vconv_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_vec_vconv_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_vec_vconv_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_VCONV_INTF_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_VCONV_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_VCONV_INTERFACE_IMPL_H
#include "../../include/basic_api/kernel_tensor.h"
#include "kernel_check.h"
#include "../../include/basic_api/kernel_struct_binary.h"
#include "../../include/basic_api/kernel_struct_unary.h"
#include "../../include/basic_api/kernel_struct_vdeq.h"
#include "kernel_npu_debug.h"
#include "mstx_local_tensor_info.h"

#if __NPU_ARCH__ == 1001
#include "dav_c100/kernel_operator_vec_vconv_impl.h"
#elif __NPU_ARCH__ == 2002
#include "dav_m200/kernel_operator_vec_vconv_impl.h"
#elif __NPU_ARCH__ == 2201
#include "dav_c220/kernel_operator_vec_vconv_impl.h"
#elif __NPU_ARCH__ == 3002
#include "dav_m300/kernel_operator_vec_vconv_impl.h"
#elif __NPU_ARCH__ == 3102
#include "dav_m310/kernel_operator_vec_vconv_impl.h"
#elif __NPU_ARCH__ == 3510
#include "dav_3510/kernel_operator_vec_vconv_impl.h"
#elif (__NPU_ARCH__ == 5102)
#include "dav_m510/kernel_operator_vec_vconv_impl.h"
#elif __NPU_ARCH__ == 3003
#include "dav_l300/kernel_operator_vec_vconv_impl.h"
#elif __NPU_ARCH__ == 3113
#include "dav_l311/kernel_operator_vec_vconv_impl.h"
#endif

namespace AscendC {
template <bool castMode>
__aicore__ inline void SetCastOverflowMode()
{
    SetCastOverflowModeImpl<castMode>();
}

#if defined(__NPU_ARCH__)
#pragma begin_pipe(V)
/* **************************************************************************************************
 * Cast                                             *
 * ************************************************************************************************* */
/*
 * @ingroup Cast Level 0
 * @brief Precision conversion
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] roundMode round mode
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.srcBlkStride src block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src repeat stride
 */
// Cast::Level 0 - mask bit mode
template <typename T, typename U, bool isSetMask>
__aicore__ inline void Cast(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const RoundMode& roundMode, const uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
    using DstPrimType = PrimT<T>;
    using SrcPrimType = PrimT<U>;
    using MaskCheckType =
        typename Conditional<(sizeof(DstPrimType) >= sizeof(SrcPrimType)), DstPrimType, SrcPrimType>::type;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Cast", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<MaskCheckType, isSetMask>(mask, repeatTime, "Cast");
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryCastInfo<T, U, isSetMask>(dst, src, mask[0], mask[1], repeatTime, repeatParams, "Cast");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if constexpr (Std::is_same<DstPrimType, int4b_t>::value) {
        Int4Setter::Instance().SetDstInt4();
    } else if constexpr (Std::is_same<SrcPrimType, int4b_t>::value) {
        Int4Setter::Instance().SetSrcInt4();
    }
#if !((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    if (!CheckFunVecBinaryScalarDiffType(
            dst, src, static_cast<SrcPrimType>(0), mask, repeatTime, repeatParams, "Cast")) {
        ASCENDC_REPORT_CHECK_ERROR("Cast", KernelFuncType::MASK_BIT_MODE);
    }
#endif
    Int4Setter::Instance().ResetDstSrcInt4();
#endif
    CastImpl<DstPrimType, SrcPrimType, isSetMask>(
        (__ubuf__ DstPrimType*)dst.GetPhyAddr(), (__ubuf__ SrcPrimType*)src.GetPhyAddr(), roundMode, mask, repeatTime,
        repeatParams);
}

// Cast::Level 0 - mask count mode
template <typename T, typename U, bool isSetMask>
__aicore__ inline void Cast(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const RoundMode& roundMode, const uint64_t mask,
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
    using DstPrimType = PrimT<T>;
    using SrcPrimType = PrimT<U>;
    using MaskCheckType =
        typename Conditional<(sizeof(DstPrimType) >= sizeof(SrcPrimType)), DstPrimType, SrcPrimType>::type;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Cast", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<MaskCheckType, isSetMask>(mask, repeatTime, "Cast");
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryCastInfo<T, U, isSetMask>(dst, src, mask, repeatTime, repeatParams, "Cast");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if constexpr (Std::is_same<DstPrimType, int4b_t>::value) {
        Int4Setter::Instance().SetDstInt4();
    } else if constexpr (Std::is_same<SrcPrimType, int4b_t>::value) {
        Int4Setter::Instance().SetSrcInt4();
    }
#if !((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    if (!CheckFunVecBinaryScalarDiffType(
            dst, src, static_cast<SrcPrimType>(0), mask, repeatTime, repeatParams, "Cast")) {
        ASCENDC_REPORT_CHECK_ERROR("Cast", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
    Int4Setter::Instance().ResetDstSrcInt4();
#endif
    CastImpl<DstPrimType, SrcPrimType, isSetMask>(
        (__ubuf__ DstPrimType*)dst.GetPhyAddr(), (__ubuf__ SrcPrimType*)src.GetPhyAddr(), roundMode, mask, repeatTime,
        repeatParams);
}

/*
 * @ingroup Cast Level 2
 * @brief dst[i] = Precision conversion
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] roundMode round mode
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, typename U>
__aicore__ inline void Cast(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const RoundMode& roundMode, const uint32_t count)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Cast", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryCastInfo<T, U, true>(dst, src, "Cast", count);
#endif
    using DstPrimType = PrimT<T>;
    using SrcPrimType = PrimT<U>;
#if ASCENDC_CPU_DEBUG
    if constexpr (Std::is_same<DstPrimType, int4b_t>::value) {
        Int4Setter::Instance().SetDstInt4();
    } else if constexpr (Std::is_same<SrcPrimType, int4b_t>::value) {
        Int4Setter::Instance().SetSrcInt4();
    }
#if !((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    if (!CheckFunVecBinaryScalarDiffType(dst, src, static_cast<SrcPrimType>(0), count, "Cast")) {
        ASCENDC_REPORT_CHECK_ERROR("Cast", KernelFuncType::CALCOUNT_MODE);
    }
#endif
    Int4Setter::Instance().ResetDstSrcInt4();
#endif
    if constexpr (!(Std::is_same<DstPrimType, int4b_t>::value) && !(Std::is_same<SrcPrimType, int4b_t>::value)) {
        ASCENDC_DEBUG_ASSERT(
            CheckCastOverlappingHigh(
                static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
                static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())), sizeof(DstPrimType),
                sizeof(SrcPrimType), count),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "Failed to pass Cast count mode check."));
    }
    CastImpl((__ubuf__ DstPrimType*)dst.GetPhyAddr(), (__ubuf__ SrcPrimType*)src.GetPhyAddr(), roundMode, count);
}

/*
 * @ingroup CastDequant Level 0
 * @brief Dequant from int16_t to uint8_t/int8_t
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.srcBlkStride src block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.srcRepStride src repeat stride
 */
template <typename T, typename U, bool isSetMask, bool isVecDeq, bool halfBlock>
__aicore__ inline void CastDequant(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint64_t mask[], uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using DstPrimType = PrimT<T>;
    using SrcPrimType = PrimT<U>;
    using MaskCheckType =
        typename Conditional<(sizeof(DstPrimType) >= sizeof(SrcPrimType)), DstPrimType, SrcPrimType>::type;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("CastDequant", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<MaskCheckType, isSetMask>(mask, repeatTime, "CastDequant");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalarDiffType(
            dst.template ReinterpretCast<half>(), src, static_cast<SrcPrimType>(0), mask, repeatTime, repeatParams,
            "CastDequant")) {
        ASCENDC_REPORT_CHECK_ERROR("CastDequant", KernelFuncType::MASK_BIT_MODE);
    }
#endif
    CastDeqImpl<DstPrimType, SrcPrimType, isSetMask, isVecDeq, halfBlock>(
        (__ubuf__ DstPrimType*)dst.GetPhyAddr(), (__ubuf__ SrcPrimType*)src.GetPhyAddr(), mask, repeatTime,
        repeatParams);
}
template <typename T, typename U, bool isSetMask, bool isVecDeq, bool halfBlock>
__aicore__ inline void CastDequant(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const int32_t mask, uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using DstPrimType = PrimT<T>;
    using SrcPrimType = PrimT<U>;
    using MaskCheckType =
        typename Conditional<(sizeof(DstPrimType) >= sizeof(SrcPrimType)), DstPrimType, SrcPrimType>::type;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("CastDequant", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskValue<MaskCheckType, isSetMask>(mask, "CastDequant");
    CheckRepeatValue(repeatTime, "CastDequant");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalarDiffType(
            dst.template ReinterpretCast<half>(), src, static_cast<SrcPrimType>(0), mask, repeatTime, repeatParams,
            "CastDequant")) {
        ASCENDC_REPORT_CHECK_ERROR("CastDequant", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
    CastDeqImpl<DstPrimType, SrcPrimType, isSetMask, isVecDeq, halfBlock>(
        (__ubuf__ DstPrimType*)dst.GetPhyAddr(), (__ubuf__ SrcPrimType*)src.GetPhyAddr(), mask, repeatTime,
        repeatParams);
}

// CastDeq has been updated, please use CastDequant instead.
template <typename T, typename U, bool isSetMask, bool isVecDeq, bool halfBlock>
__aicore__ inline void CastDeq(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint64_t mask[], uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryCastDeqInfo<T, U, isSetMask>(
        dst, src, mask[0], mask[1], repeatTime, repeatParams, "CastDeq", halfBlock);
#endif
    using DstPrimType = PrimT<T>;
    using SrcPrimType = PrimT<U>;
    using MaskCheckType =
        typename Conditional<(sizeof(DstPrimType) >= sizeof(SrcPrimType)), DstPrimType, SrcPrimType>::type;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("CastDeq", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskRepeat<MaskCheckType, isSetMask>(mask, repeatTime, "CastDeq");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalarDiffType(
            dst.template ReinterpretCast<half>(), src, static_cast<SrcPrimType>(0), mask, repeatTime, repeatParams,
            "CastDeq")) {
        ASCENDC_REPORT_CHECK_ERROR("CastDeq", KernelFuncType::MASK_BIT_MODE);
    }
#endif
    CastDeqImpl<DstPrimType, SrcPrimType, isSetMask, isVecDeq, halfBlock>(
        (__ubuf__ DstPrimType*)dst.GetPhyAddr(), (__ubuf__ SrcPrimType*)src.GetPhyAddr(), mask, repeatTime,
        repeatParams);
}

// CastDeq has been updated, please use CastDequant instead.
template <typename T, typename U, bool isSetMask, bool isVecDeq, bool halfBlock>
__aicore__ inline void CastDeq(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const int32_t mask, uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryCastDeqInfo<T, U, isSetMask>(
        dst, src, mask, repeatTime, repeatParams, "CastDeq", halfBlock);
#endif
    using DstPrimType = PrimT<T>;
    using SrcPrimType = PrimT<U>;
    using MaskCheckType =
        typename Conditional<(sizeof(DstPrimType) >= sizeof(SrcPrimType)), DstPrimType, SrcPrimType>::type;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("CastDeq", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckMaskValue<MaskCheckType, isSetMask>(mask, "CastDeq");
    CheckRepeatValue(repeatTime, "CastDeq");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFunVecBinaryScalarDiffType(
            dst.template ReinterpretCast<half>(), src, static_cast<SrcPrimType>(0), mask, repeatTime, repeatParams,
            "CastDeq")) {
        ASCENDC_REPORT_CHECK_ERROR("CastDeq", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
    CastDeqImpl<DstPrimType, SrcPrimType, isSetMask, isVecDeq, halfBlock>(
        (__ubuf__ DstPrimType*)dst.GetPhyAddr(), (__ubuf__ SrcPrimType*)src.GetPhyAddr(), mask, repeatTime,
        repeatParams);
}

/*
 * @ingroup CastDequant Level 2
 * @brief Dequant from int16_t to uint8_t/int8_t
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.srcBlkStride src block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.srcRepStride src repeat stride
 */
template <typename T, typename U, bool isVecDeq, bool halfBlock>
__aicore__ inline void CastDequant(const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint32_t count)
{
    using DstPrimType = PrimT<T>;
    using SrcPrimType = PrimT<U>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("CastDequant", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckCalcount(static_cast<int32_t>(count), "count", "CastDequant");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecBinaryScalarDiffType(
            dst.template ReinterpretCast<half>(), src, static_cast<SrcPrimType>(0), count, "CastDequant")) {
        ASCENDC_REPORT_CHECK_ERROR("CastDequant", KernelFuncType::CALCOUNT_MODE);
    }
#endif
    CastDeqImpl<DstPrimType, SrcPrimType, isVecDeq, halfBlock>(
        (__ubuf__ DstPrimType*)dst.GetPhyAddr(), (__ubuf__ SrcPrimType*)src.GetPhyAddr(), count);
}

// CastDeq has been updated, please use CastDequant instead.
template <typename T, typename U, bool isVecDeq, bool halfBlock>
__aicore__ inline void CastDeq(const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint32_t count)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecUnaryCastDeqInfo<T, U, true>(dst, src, "CastDeq", count, halfBlock);
#endif
    using DstPrimType = PrimT<T>;
    using SrcPrimType = PrimT<U>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("CastDeq", NamedTensor(dst, "dst"), NamedTensor(src, "src"));
    CheckCalcount(static_cast<int32_t>(count), "count", "CastDeq");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFunVecBinaryScalarDiffType(
            dst.template ReinterpretCast<half>(), src, static_cast<SrcPrimType>(0), count, "CastDeq")) {
        ASCENDC_REPORT_CHECK_ERROR("CastDeq", KernelFuncType::CALCOUNT_MODE);
    }
#endif
    CastDeqImpl<DstPrimType, SrcPrimType, isVecDeq, halfBlock>(
        (__ubuf__ DstPrimType*)dst.GetPhyAddr(), (__ubuf__ SrcPrimType*)src.GetPhyAddr(), count);
}

/* **************************************************************************************************
 * AddReluCast                                             *
 * ************************************************************************************************* */
/*
 * @ingroup AddReluCast Level 0
 * @brief Precision conversion
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.src0BlkStride src block stride
 * @param [in] intriParams.src1BlkStride src block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src repeat stride
 * @param [in] intriParams.src1RepStride src repeat stride
 */
template <typename T, typename U, bool isSetMask>
__aicore__ inline void AddReluCast(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using SrcPrimType = PrimT<U>;
    using DstPrimType = PrimT<T>;
    using MaskCheckType =
        typename Conditional<(sizeof(DstPrimType) >= sizeof(SrcPrimType)), DstPrimType, SrcPrimType>::type;
#if __NPU_ARCH__ == 2201
    if (g_coreType == AIC) {
        return;
    }
#endif
#if defined(ASCENDC_CPU_DEBUG) || defined(ASCENDC_DEBUG)
    CheckVectorTensor("AddReluCast", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<MaskCheckType, isSetMask>(mask, repeatTime, "AddReluCast");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinaryCmp(dst, src0, src1, mask, repeatTime, repeatParams, "AddReluCast")) {
        ASCENDC_REPORT_CHECK_ERROR("AddReluCast", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, mask, repeatTime, repeatParams, isSetMask, "AddReluCast");
#endif
    AddReluCastImpl<DstPrimType, SrcPrimType, isSetMask>(
        (__ubuf__ DstPrimType*)dst.GetPhyAddr(), (__ubuf__ SrcPrimType*)src0.GetPhyAddr(),
        (__ubuf__ SrcPrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

template <typename T, typename U, bool isSetMask>
__aicore__ inline void AddReluCast(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using SrcPrimType = PrimT<U>;
    using DstPrimType = PrimT<T>;
    using MaskCheckType =
        typename Conditional<(sizeof(DstPrimType) >= sizeof(SrcPrimType)), DstPrimType, SrcPrimType>::type;
#if __NPU_ARCH__ == 2201
    if (g_coreType == AIC) {
        return;
    }
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("AddReluCast", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<MaskCheckType, isSetMask>(mask, repeatTime, "AddReluCast");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinaryCmp(dst, src0, src1, mask, repeatTime, repeatParams, "AddReluCast")) {
        ASCENDC_REPORT_CHECK_ERROR("AddReluCast", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(
        dst, src0, src1, mask[0], mask[1], repeatTime, repeatParams, isSetMask, "AddReluCast");
#endif
    AddReluCastImpl<DstPrimType, SrcPrimType, isSetMask>(
        (__ubuf__ DstPrimType*)dst.GetPhyAddr(), (__ubuf__ SrcPrimType*)src0.GetPhyAddr(),
        (__ubuf__ SrcPrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

/*
 * @ingroup AddReluCast Level 2
 * @brief dst[i] = Precision conversion
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, typename U>
__aicore__ inline void AddReluCast(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, const uint32_t count)
{
#if __NPU_ARCH__ == 2201
    if (g_coreType == AIC) {
        return;
    }
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("AddReluCast", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckCalcount(static_cast<int32_t>(count), "count", "AddReluCast");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinaryCmp(dst, src0, src1, count, "AddReluCast")) {
        ASCENDC_REPORT_CHECK_ERROR("AddReluCast", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, "AddReluCast", count);
#endif
    AddReluCastImpl(
        (__ubuf__ PrimT<T>*)dst.GetPhyAddr(), (__ubuf__ PrimT<U>*)src0.GetPhyAddr(),
        (__ubuf__ PrimT<U>*)src1.GetPhyAddr(), count);
}

/* **************************************************************************************************
 * SubReluCast                                             *
 * ************************************************************************************************* */
/*
 * @ingroup SubReluCast Level 0
 * @brief Precision conversion
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.src0BlkStride src block stride
 * @param [in] intriParams.src1BlkStride src block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src repeat stride
 * @param [in] intriParams.src1RepStride src repeat stride
 */
template <typename T, typename U, bool isSetMask>
__aicore__ inline void SubReluCast(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using DstPrimType = PrimT<T>;
    using SrcPrimType = PrimT<U>;
    using MaskCheckType =
        typename Conditional<(sizeof(DstPrimType) >= sizeof(SrcPrimType)), DstPrimType, SrcPrimType>::type;
#if __NPU_ARCH__ == 2201
    if (g_coreType == AIC) {
        return;
    }
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("SubReluCast", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<MaskCheckType, isSetMask>(mask, repeatTime, "SubReluCast");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinaryCmp(dst, src0, src1, mask, repeatTime, repeatParams, "SubReluCast")) {
        ASCENDC_REPORT_CHECK_ERROR("SubReluCast", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, mask, repeatTime, repeatParams, isSetMask, "SubReluCast");
#endif
    SubReluCastImpl<DstPrimType, SrcPrimType, isSetMask>(
        (__ubuf__ DstPrimType*)dst.GetPhyAddr(), (__ubuf__ SrcPrimType*)src0.GetPhyAddr(),
        (__ubuf__ SrcPrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

template <typename T, typename U, bool isSetMask>
__aicore__ inline void SubReluCast(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using SrcPrimType = PrimT<U>;
    using DstPrimType = PrimT<T>;
    using MaskCheckType =
        typename Conditional<(sizeof(DstPrimType) >= sizeof(SrcPrimType)), DstPrimType, SrcPrimType>::type;
#if __NPU_ARCH__ == 2201
    if (g_coreType == AIC) {
        return;
    }
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("SubReluCast", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckMaskRepeat<MaskCheckType, isSetMask>(mask, repeatTime, "SubReluCast");
#endif
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(isSetMask);
    if (!CheckFuncVecBinaryCmp(dst, src0, src1, mask, repeatTime, repeatParams, "SubReluCast")) {
        ASCENDC_REPORT_CHECK_ERROR("SubReluCast", KernelFuncType::MASK_BIT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(
        dst, src0, src1, mask[0], mask[1], repeatTime, repeatParams, isSetMask, "SubReluCast");
#endif
    SubReluCastImpl<DstPrimType, SrcPrimType, isSetMask>(
        (__ubuf__ DstPrimType*)dst.GetPhyAddr(), (__ubuf__ SrcPrimType*)src0.GetPhyAddr(),
        (__ubuf__ SrcPrimType*)src1.GetPhyAddr(), mask, repeatTime, repeatParams);
}

/*
 * @ingroup SubReluCast Level 2
 * @brief dst[i] = Precision conversion
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, typename U>
__aicore__ inline void SubReluCast(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, const uint32_t count)
{
    using DstPrimType = PrimT<T>;
    using SrcPrimType = PrimT<U>;
#if __NPU_ARCH__ == 2201
    if (g_coreType == AIC) {
        return;
    }
#endif
#if defined(ASCENDC_CPU_DEBUG) || defined(ASCENDC_DEBUG)
    CheckVectorTensor("SubReluCast", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1, "src1"));
    CheckCalcount(static_cast<int32_t>(count), "count", "SubReluCast");
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecBinaryCmp(dst, src0, src1, count, "SubReluCast")) {
        ASCENDC_REPORT_CHECK_ERROR("SubReluCast", KernelFuncType::CALCOUNT_MODE);
    }
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBinaryInfo(dst, src0, src1, "SubReluCast", count);
#endif
    SubReluCastImpl(
        (__ubuf__ DstPrimType*)dst.GetPhyAddr(), (__ubuf__ SrcPrimType*)src0.GetPhyAddr(),
        (__ubuf__ SrcPrimType*)src1.GetPhyAddr(), count);
}

#pragma end_pipe
__aicore__ inline void SetDeqScale(half scale)
{
#if __NPU_ARCH__ == 2201
    if (g_coreType == AIC) {
        return;
    }
#endif
#if ASCENDC_CPU_DEBUG
    AscendC::g_isVdeq = false;
#endif
    SetDeqScaleImpl(scale);
}

__aicore__ inline void SetDeqScale(float scale, int16_t offset, bool signMode)
{
#if __NPU_ARCH__ == 2201
    if (g_coreType == AIC) {
        return;
    }
#endif
#if ASCENDC_CPU_DEBUG
    AscendC::g_isVdeq = false;
#endif
    SetDeqScaleImpl(scale, offset, signMode);
}

template <typename T>
__aicore__ inline void SetDeqScale(const LocalTensor<T>& vdeq, const VdeqInfo& vdeqInfo)
{
#if __NPU_ARCH__ == 2201
    if (g_coreType == AIC) {
        return;
    }
#endif
#if ASCENDC_CPU_DEBUG
    AscendC::g_isVdeq = true;
    ASCENDC_ASSERT((SupportType<PrimT<T>, uint64_t>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in SetDeqScale, "
                          "current api support dtype combination is vdeq: uint64_t");
    });
    CheckTensorAlign<T>(vdeq, ONE_BLK_SIZE, "vdeq", "SetDeqScale");
    CheckTensorPos<T>(vdeq, Hardware::UB, "vdeq", "VECIN/VECCALC/VECOUT", "SetDeqScale");
#endif
    SetDeqScaleImpl<T>(vdeq, vdeqInfo);
}

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113)
/*
 * @ingroup Truncate Level 2
 * @brief dst[i] = Precision conversion
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, RoundMode roundMode>
__aicore__ inline void Truncate(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
{
    using PrimType = PrimT<T>;
    CheckTensorPos<T>(dst, Hardware::UB, "dst", "VECIN/VECCALC/VECOUT", "Truncate");
    CheckTensorPos<T>(src, Hardware::UB, "src", "VECIN/VECCALC/VECOUT", "Truncate");
    ASCENDC_ASSERT((count <= src.GetSize() && count <= dst.GetSize()), {
        KERNEL_LOG(KERNEL_ERROR, "count is %u, which should not larger than tensor size of dst / src", count);
    });
    TruncateImpl<PrimType, roundMode>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), count);
}
#endif
#endif // defined(__NPU_ARCH__)
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_VCONV_INTERFACE_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_VCONV_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_VCONV_INTF_IMPL_H__
#endif
