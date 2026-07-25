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
    "impl/adv_api/detail/reduce/reduce_sum/reduce_sum_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/reduce/reduce.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_SUM_REDUCE_SUM_C310_IMPL_H__
#endif

#ifndef IMPL_REDUCE_REDUCE_SUM_REDUCE_SUM_C310_IMPL_H
#define IMPL_REDUCE_REDUCE_SUM_REDUCE_SUM_C310_IMPL_H

#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "kernel_tiling/kernel_tiling.h"
#include "../reduce_common_util_impl.h"
#include "../reduce_common_util_3510_impl.h"
#include "../reduce_common_ar_ra_reuse_unalign_3510_impl.h"
#include "../reduce_common_ar_reuse_align_3510_impl.h"
#include "../reduce_common_ra_reuse_align_3510_impl.h"
#include "../../common/check.h"
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
namespace Internal {
template <class T, bool isReuseSource = false>
__aicore__ inline void ReduceSumARB64ReuseSourceCompute(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ T* tmpAddr, const uint32_t srcShape[])
{
    if ((srcShape[1] * sizeof(T)) % 32 == 0) {
        ReduceARImpl<
            T, Reg::RegTraitNumTwo, Reg::Add<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T, Reg::RegTraitNumTwo>>,
            Reg::ReduceSum<
                T, T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T, Reg::RegTraitNumTwo>,
                Reg::RegTensor<T, Reg::RegTraitNumTwo>>,
            isReuseSource>(dstAddr, srcAddr, tmpAddr, srcShape[0], srcShape[1]);
    } else {
        ReduceARReuseSourceUnAligned<
            T, Reg::RegTraitNumTwo, Reg::Add<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T, Reg::RegTraitNumTwo>>,
            Reg::ReduceSum<
                T, T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T, Reg::RegTraitNumTwo>,
                Reg::RegTensor<T, Reg::RegTraitNumTwo>>>(dstAddr, srcAddr, srcShape[0], srcShape[1]);
    }
}

template <class T, bool isReuseSource = false>
__aicore__ inline void ReduceSumARReuseSourceCompute(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ T* tmpAddr, const uint32_t srcShape[])
{
    if ((srcShape[1] * sizeof(T)) % 32 == 0 || srcShape[1] == 1) {
        ReduceARImpl<
            T, Reg::RegTraitNumOne, Reg::Add<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>,
            Reg::ReduceSum<T, T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>, Reg::RegTensor<T>>, isReuseSource,
            ReduceType::SUM>(dstAddr, srcAddr, tmpAddr, srcShape[0], srcShape[1]);
    } else {
        ReduceARReuseSourceUnAligned<
            T, Reg::RegTraitNumOne, Reg::Add<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>,
            Reg::ReduceSum<T, T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>, Reg::RegTensor<T>>>(
            dstAddr, srcAddr, srcShape[0], srcShape[1]);
    }
}

template <class T, bool isReuseSource = false>
__aicore__ inline void ReduceSumRAB64ReuseSourceCompute(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ T* tmpAddr, const uint32_t srcShape[])
{
    if ((srcShape[1] * sizeof(T)) % 32 == 0) {
        ReduceRAB64ReuseSource<
            T, Reg::RegTraitNumTwo, Reg::Add<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T, Reg::RegTraitNumTwo>>,
            isReuseSource>(dstAddr, srcAddr, tmpAddr, srcShape[1], srcShape[0]);
    } else {
        ReduceRAReuseSourceUnAlignedB64<
            T, Reg::RegTraitNumTwo, Reg::Add<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T, Reg::RegTraitNumTwo>>>(
            dstAddr, srcAddr, srcShape[1], srcShape[0]);
    }
}

template <class T, bool isReuseSource = false>
__aicore__ inline void ReduceSumRAReuseSourceCompute(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ T* tmpAddr, const uint32_t srcShape[])
{
    if ((srcShape[1] * sizeof(T)) % 32 == 0) {
        ReduceRAImpl<
            T, Reg::RegTraitNumOne, Reg::Add<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>, isReuseSource>(
            dstAddr, srcAddr, tmpAddr, srcShape[1], srcShape[0]);
    } else {
        ReduceRAReuseSourceUnAligned<
            T, Reg::RegTraitNumOne, Reg::Add<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>>(
            dstAddr, srcAddr, srcShape[1], srcShape[0]);
    }
}

template <class T, class pattern, bool isReuseSource = false>
__aicore__ inline void ReduceSumImpl(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t srcShape[], bool srcInnerPad)
{
    CheckTensorPos<T>(dst, Hardware::UB, "dstTensor", "VECIN/VECCALC/VECOUT", "ReduceSum");
    CheckTensorPos<T>(src, Hardware::UB, "srcTensor", "VECIN/VECCALC/VECOUT", "ReduceSum");
    CheckTensorPos<uint8_t>(sharedTmpBuffer, Hardware::UB, "sharedTmpBuffer", "VECIN/VECCALC/VECOUT", "ReduceSum");
    static_assert(
        std::is_same_v<pattern, Pattern::Reduce::AR> || std::is_same_v<pattern, Pattern::Reduce::RA>,
        "ReduceSum only support AR and RA pattern on current device!");
    static_assert(
        SupportType<T, int32_t, uint32_t, float, int64_t, uint64_t>(),
        "ReduceSum only support int32_t/uint32_t/float/int64_t/uint64_t data type on current device!");
    __ubuf__ T* dstAddr = (__ubuf__ T*)dst.GetPhyAddr();
    __ubuf__ T* srcAddr = (__ubuf__ T*)src.GetPhyAddr();
    LocalTensor<T> tmpBuf = sharedTmpBuffer.ReinterpretCast<T>();
    __ubuf__ T* tmpAddr = (__ubuf__ T*)tmpBuf.GetPhyAddr();
    if constexpr (std::is_same_v<pattern, Pattern::Reduce::AR>) {
        if constexpr (SupportBytes<T, 8>()) {
            ReduceSumARB64ReuseSourceCompute<T, isReuseSource>(dstAddr, srcAddr, tmpAddr, srcShape);
        } else {
            ReduceSumARReuseSourceCompute<T, isReuseSource>(dstAddr, srcAddr, tmpAddr, srcShape);
        }
    } else {
        if constexpr (SupportBytes<T, 8>()) {
            ReduceSumRAB64ReuseSourceCompute<T, isReuseSource>(dstAddr, srcAddr, tmpAddr, srcShape);
        } else {
            ReduceSumRAReuseSourceCompute<T, isReuseSource>(dstAddr, srcAddr, tmpAddr, srcShape);
        }
    }
}
} // namespace Internal
} // namespace AscendC
#endif // IMPL_REDUCE_REDUCE_SUM_REDUCE_SUM_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_SUM_REDUCE_SUM_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_SUM_REDUCE_SUM_C310_IMPL_H__
#endif
