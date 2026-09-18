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
 * \file kernel_operator_vec_gather_mask_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_vec_gather_mask_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_vec_gather_mask_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_GATHER_MASK_INTF_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_REDUCEV2_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_REDUCEV2_INTERFACE_IMPL_H
#include "../../include/basic_api/kernel_tensor.h"
#include "../../include/basic_api/kernel_struct_gather.h"
#include "mstx_local_tensor_info.h"
#include "kernel_npu_debug.h"

#if __NPU_ARCH__ == 1001
#include "dav_c100/kernel_operator_vec_gather_mask_impl.h"
#elif __NPU_ARCH__ == 2002
#include "dav_m200/kernel_operator_vec_gather_mask_impl.h"
#elif __NPU_ARCH__ == 2201
#include "dav_c220/kernel_operator_vec_gather_mask_impl.h"
#elif __NPU_ARCH__ == 3002
#include "dav_m300/kernel_operator_vec_gather_mask_impl.h"
#elif __NPU_ARCH__ == 3102
#include "dav_m310/kernel_operator_vec_gather_mask_impl.h"
#elif __NPU_ARCH__ == 3510
#include "dav_3510/kernel_operator_vec_gather_mask_impl.h"
#elif (__NPU_ARCH__ == 5102)
#include "dav_m510/kernel_operator_vec_gather_mask_impl.h"
#elif (__NPU_ARCH__ == 5162)
#include "dav_v516/kernel_operator_vec_gather_mask_impl.h"
#elif (__NPU_ARCH__ == 3003)
#include "dav_l300/kernel_operator_vec_gather_mask_impl.h"
#elif (__NPU_ARCH__ == 3113)
#include "dav_l311/kernel_operator_vec_gather_mask_impl.h"
#endif
#include "kernel_check.h"

namespace AscendC {
#pragma begin_pipe(V)
template <typename T, typename U, GatherMaskMode mode>
__aicore__ inline void GatherMask(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<U>& src1Pattern, const bool reduceMode,
    const uint32_t mask, const GatherMaskParams& gatherMaskParams, uint64_t& rsvdCnt)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor(
        "GatherMask", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"), NamedTensor(src1Pattern, "src1Pattern"));
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecGatherMaskInfo(dst, src0, mask, gatherMaskParams, mode, "GatherMask");
#endif
    using DstPrimType = PrimT<T>;
    using Src1PrimType = PrimT<U>;
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecGatherMask(dst, src0, src1Pattern, reduceMode, mask, gatherMaskParams, rsvdCnt, "GatherMask")) {
        ASCENDC_REPORT_CHECK_ERROR("GatherMask", KernelFuncType::MASK_COUNT_MODE);
    }
#endif

#if defined(__NPU_ARCH__) &&                                                                                 \
    ((__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162) || \
     (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || (__NPU_ARCH__ == 3510))
    GatherMaskCal(
        (__ubuf__ DstPrimType*)dst.GetPhyAddr(), (__ubuf__ DstPrimType*)src0.GetPhyAddr(),
        (__ubuf__ Src1PrimType*)src1Pattern.GetPhyAddr(), reduceMode, mask, gatherMaskParams, rsvdCnt);
#else
    GatherMaskCal<DstPrimType, Src1PrimType, mode>(
        (__ubuf__ DstPrimType*)dst.GetPhyAddr(), (__ubuf__ DstPrimType*)src0.GetPhyAddr(),
        (__ubuf__ Src1PrimType*)src1Pattern.GetPhyAddr(), reduceMode, mask, gatherMaskParams, rsvdCnt);
#endif
}

template <typename T, GatherMaskMode mode>
__aicore__ inline void GatherMask(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const uint8_t src1Pattern, const bool reduceMode,
    const uint32_t mask, const GatherMaskParams& gatherMaskParams, uint64_t& rsvdCnt)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("GatherMask", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"));
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecGatherMaskInfo(dst, src0, mask, gatherMaskParams, mode, "GatherMask");
#endif
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncVecGatherMask(dst, src0, src1Pattern, reduceMode, mask, gatherMaskParams, rsvdCnt, "GatherMask")) {
        ASCENDC_REPORT_CHECK_ERROR("GatherMask", KernelFuncType::MASK_COUNT_MODE);
    }
#endif

#if defined(__NPU_ARCH__) &&                                                                                 \
    ((__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162) || \
     (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || (__NPU_ARCH__ == 3510))
    GatherMaskCal(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(), src1Pattern, reduceMode, mask,
        gatherMaskParams, rsvdCnt);
#else
    GatherMaskCal<PrimType, mode>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(), src1Pattern, reduceMode, mask,
        gatherMaskParams, rsvdCnt);
#endif
}

template <typename T>
[[deprecated("NOTICE: This GatherMask in this form has been deprecated and will be removed in the next version. "
             "Please do not use it!")]] __aicore__ inline void
GatherMask(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const uint8_t patternMode,
    const GatherMaskParams& gatherMaskParams)
{
    using PrimType = PrimT<T>;
    GatherMaskImpl(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(),
        (__ubuf__ PrimType*)src1.GetPhyAddr(), patternMode, gatherMaskParams);
}

template <typename T>
[[deprecated("NOTICE: This GatherMask in this form has been deprecated and will be removed in the next version. "
             "Please do not use it!")]] __aicore__ inline void
GatherMask(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const uint8_t patternMode,
    const GatherMaskParams& gatherMaskParams)
{
    using PrimType = PrimT<T>;
    GatherMaskImpl(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(), patternMode, gatherMaskParams);
}
#pragma end_pipe

__aicore__ inline __inout_pipe__(S) int64_t GetGatherMaskRemainCount()
{
#if __NPU_ARCH__ == 2201
    if (g_coreType == AIC) {
        return 0;
    }
#endif
    return GetGatherMaskRemainCountImpl();
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_REDUCEV2_INTERFACE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_GATHER_MASK_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_GATHER_MASK_INTF_IMPL_H__
#endif
