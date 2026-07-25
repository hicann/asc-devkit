/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/reduce/reduce_all/reduce_all_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/reduce/reduce.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_ALL_REDUCE_ALL_C310_IMPL_H__
#endif

#ifndef IMPL_REDUCE_REDUCE_ALL_REDUCE_ALL_C310_IMPL_H_
#define IMPL_REDUCE_REDUCE_ALL_REDUCE_ALL_C310_IMPL_H_

#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../common/check.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/reduce/reduce_all/reduce_all_check.h"
#include "../../api_check/kernel_api_check.h"
#include "../reduce_common_util_3510_impl.h"
#include "../reduce_common_ar_reuse_align_3510_impl.h"
#include "../reduce_common_ra_reuse_align_3510_impl.h"
#endif // ASCENDC_CPU_DEBUG

namespace AscendC {
namespace Internal {

template <class T, bool isReuseSource = false>
__aicore__ inline void ReduceAllARImpl(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ T* tmpAddr, uint32_t dimA, uint32_t dimR)
{
    constexpr uint16_t vlSize = GetVecLen() / sizeof(T);
    using CastType = typename ReduceOpInternal::template ExtractReduceCastType<T>::CastT;

    if (dimR <= vlSize) {
        ReduceARReuseSourceLessThanVL<
            T, Reg::RegTraitNumOne, vlSize, Reg::Min<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>,
            Reg::ReduceMin<CastType, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<CastType>>>(
            dstAddr, srcAddr, dimA, dimR);
    } else {
        ReduceAROverVLImpl<
            T, Reg::RegTraitNumOne, vlSize, Reg::Min<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>,
            Reg::ReduceMin<CastType, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<CastType>>, isReuseSource>(
            dstAddr, srcAddr, tmpAddr, dimA, dimR);
    }
}

template <typename T, typename pattern, bool isReuseSource = false>
__aicore__ inline void ReduceAllImpl(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t srcShape[], bool srcInnerPad)
{
    CHECK_FUNC_HIGHLEVEL_API(ReduceAll, (T, pattern), (dst, src, sharedTmpBuffer, srcShape, srcInnerPad, srcShape[1]));

    CheckTensorPos<T>(dst, Hardware::UB, "dst", "VECIN/VECCALC/VECOUT", "ReduceAll");
    CheckTensorPos<T>(src, Hardware::UB, "src", "VECIN/VECCALC/VECOUT", "ReduceAll");
    CheckTensorPos<uint8_t>(sharedTmpBuffer, Hardware::UB, "sharedTmpBuffer", "VECIN/VECCALC/VECOUT", "ReduceAll");
    static_assert(
        SupportType<T, uint8_t, float>(), "ReduceAll only support uint8_t/float data type on current device!");
    static_assert(
        std::is_same_v<pattern, Pattern::Reduce::AR> || std::is_same_v<pattern, Pattern::Reduce::RA>,
        "ReduceAll only support AR and RA pattern on current device!");

    __ubuf__ T* dstAddr = (__ubuf__ T*)dst.GetPhyAddr();
    __ubuf__ T* srcAddr = (__ubuf__ T*)src.GetPhyAddr();
    LocalTensor<T> tmpBuf = sharedTmpBuffer.ReinterpretCast<T>();
    __ubuf__ T* tmpAddr = (__ubuf__ T*)tmpBuf.GetPhyAddr();
    if constexpr (std::is_same_v<pattern, Pattern::Reduce::AR>) {
        ReduceAllARImpl<T, isReuseSource>(dstAddr, srcAddr, tmpAddr, srcShape[0], srcShape[1]);
    } else {
        ReduceRAImpl<
            T, Reg::RegTraitNumOne, Reg::Min<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>, isReuseSource>(
            dstAddr, srcAddr, tmpAddr, srcShape[1], srcShape[0]);
    }
}
} // namespace Internal
} // namespace AscendC
#endif // IMPL_REDUCE_REDUCE_All_REDUCE_All_C310_IMPL_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_ALL_REDUCE_ALL_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_ALL_REDUCE_ALL_C310_IMPL_H__
#endif
