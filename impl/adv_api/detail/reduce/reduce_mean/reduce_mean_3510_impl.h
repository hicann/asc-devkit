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
    "impl/adv_api/detail/reduce/reduce_mean/reduce_mean_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/reduce/reduce.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_MEAN_REDUCE_MEAN_C310_IMPL_H__
#endif

#ifndef IMPL_REDUCE_REDUCE_MEAN_REDUCE_MEAN_C310_IMPL_H
#define IMPL_REDUCE_REDUCE_MEAN_REDUCE_MEAN_C310_IMPL_H

#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "kernel_tiling/kernel_tiling.h"
#include "../reduce_common_util_3510_impl.h"
#include "../../common/check.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/reduce/reduce_mean/reduce_mean_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
namespace Internal {
template <class T, class pattern, bool isReuseSource = false>
__aicore__ inline void ReduceMeanImpl(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t srcShape[], bool srcInnerPad)
{
    CHECK_FUNC_HIGHLEVEL_API(ReduceMean, (T, pattern), (dst, src, sharedTmpBuffer, srcShape, srcInnerPad, srcShape[1]));

    CheckTensorPos<T>(dst, Hardware::UB, "dst", "VECIN/VECCALC/VECOUT", "ReduceMean");
    CheckTensorPos<T>(src, Hardware::UB, "src", "VECIN/VECCALC/VECOUT", "ReduceMean");
    CheckTensorPos<uint8_t>(sharedTmpBuffer, Hardware::UB, "sharedTmpBuffer", "VECIN/VECCALC/VECOUT", "ReduceMean");
    static_assert(SupportType<T, float>(), "ReduceMean only support float data type on current device!");
    static_assert(
        std::is_same_v<pattern, Pattern::Reduce::AR> || std::is_same_v<pattern, Pattern::Reduce::RA>,
        "ReduceMean only support AR and RA pattern on current device!");

    __ubuf__ T* dstAddr = (__ubuf__ T*)dst.GetPhyAddr();
    __ubuf__ T* srcAddr = (__ubuf__ T*)src.GetPhyAddr();
    LocalTensor<T> tmpBuf = sharedTmpBuffer.ReinterpretCast<T>();
    __ubuf__ T* tmpAddr = (__ubuf__ T*)tmpBuf.GetPhyAddr();

    ReduceSumImpl<T, pattern, isReuseSource>(dst, src, sharedTmpBuffer, srcShape, srcInnerPad);
    if constexpr (IsSameType<pattern, Pattern::Reduce::AR>::value) {
        float lastAxisValReciprocal = 1.0f / static_cast<int32_t>(srcShape[1]);
        Muls<T>(dst, dst, lastAxisValReciprocal, srcShape[0]);
    } else if constexpr (IsSameType<pattern, Pattern::Reduce::RA>::value) {
        float firstAxisValReciprocal = 1.0f / static_cast<int32_t>(srcShape[0]);
        Muls<T>(dst, dst, firstAxisValReciprocal, srcShape[1]);
    }
}
} // namespace Internal
} // namespace AscendC
#endif // IMPL_REDUCE_REDUCE_MEAN_REDUCE_MEAN_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_MEAN_REDUCE_MEAN_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_MEAN_REDUCE_MEAN_C310_IMPL_H__
#endif
