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
    "impl/adv_api/detail/reduce/reduce_prod/reduce_prod_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/reduce/reduce.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_PROD_REDUCE_PROD_C310_IMPL_H__
#endif

#ifndef IMPL_REDUCE_REDUCE_PROD_REDUCE_PROD_C310_IMPL_H_
#define IMPL_REDUCE_REDUCE_PROD_REDUCE_PROD_C310_IMPL_H_

#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../reduce_common_util_impl.h"
#include "../reduce_common_util_3510_impl.h"
#include "../reduce_common_ar_reuse_align_3510_impl.h"
#include "../reduce_common_ra_reuse_align_3510_impl.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/reduce/reduce_prod/reduce_prod_check.h"
#include "../../api_check/kernel_api_check.h"
#include "../../common/check.h"
#endif // ASCENDC_CPU_DEBUG

namespace AscendC {
namespace Internal {
template <typename T>
__aicore__ inline void ReduceProd(Reg::RegTensor<T>& dst, Reg::RegTensor<T> src, Reg::MaskReg mask)
{
    Reg::RegTensor<T> tempOne;
    // mask invalid data in src to one
    Reg::Duplicate(tempOne, 1);
    Reg::Select(src, src, tempOne, mask);

    if constexpr (sizeof(T) == 1) {
        // fold to 128
        Reg::DeInterleave(dst, src, src, tempOne);
        Reg::Mul(src, dst, src, mask);
    }
    if constexpr (sizeof(T) <= 2) {
        // fold to 64
        Reg::DeInterleave(dst, src, src, tempOne);
        Reg::Mul(src, dst, src, mask);
    }
    // fold from 64 to 2
    Reg::DeInterleave(dst, src, src, tempOne);
    Reg::Mul(src, dst, src, mask);
    Reg::DeInterleave(dst, src, src, tempOne);
    Reg::Mul(src, dst, src, mask);
    Reg::DeInterleave(dst, src, src, tempOne);
    Reg::Mul(src, dst, src, mask);
    Reg::DeInterleave(dst, src, src, tempOne);
    Reg::Mul(src, dst, src, mask);
    Reg::DeInterleave(dst, src, src, tempOne);
    Reg::Mul(src, dst, src, mask);
    // fold to 1
    Reg::DeInterleave(dst, src, src, tempOne);
    Reg::Mul(dst, dst, src, mask);
}

template <class T, const Reg::RegTrait& Trait, const uint16_t vlSize>
__simd_vf__ inline void ReduceProdARLessThanVLVF(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t dimA, uint32_t dimR, const uint16_t repeatTime)
{
    Reg::RegTensor<T, Trait> vreg0;
    Reg::RegTensor<T, Trait> vreg1;
    Reg::UnalignReg uDst;
    Reg::RegTensor<T> tempOne;
    // mask invalid data in src to one
    Reg::Duplicate(tempOne, 1);
    uint32_t sreg1 = dimR;
    Reg::MaskReg mask = Reg::UpdateMask<T, Trait>(sreg1);
    for (uint16_t loopA = 0; loopA < static_cast<uint16_t>(dimA); loopA++) {
        Reg::LoadAlign(vreg0, srcAddr + loopA * dimR);
        Reg::Select(vreg0, vreg0, tempOne, mask);
        if constexpr (sizeof(T) == 1) {
            // fold to 128
            Reg::DeInterleave(vreg1, vreg0, vreg0, tempOne);
            Reg::Mul(vreg0, vreg1, vreg0, mask);
        }
        if constexpr (sizeof(T) <= 2) {
            // fold to 64
            Reg::DeInterleave(vreg1, vreg0, vreg0, tempOne);
            Reg::Mul(vreg0, vreg1, vreg0, mask);
        }
        for (uint16_t i = 0; i < repeatTime; ++i) {
            Reg::DeInterleave(vreg1, vreg0, vreg0, tempOne);
            Reg::Mul(vreg0, vreg1, vreg0, mask);
        }
        // fold to 1
        Reg::DeInterleave(vreg1, vreg0, vreg0, tempOne);
        Reg::Mul(vreg1, vreg1, vreg0, mask);
        Reg::StoreUnAlign((__ubuf__ T*&)dstAddr, vreg1, uDst, 1);
    }
    Reg::StoreUnAlignPost((__ubuf__ T*&)dstAddr, uDst, 0);
}

template <class T, bool isReuseSource = false>
__aicore__ inline void ReduceProdARImpl(
    __ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ T* tmpAddr, uint32_t dimA, uint32_t dimR)
{
    constexpr uint16_t vlSize = GetVecLen() / sizeof(T);
    if (dimR == 1) {
        ReduceOpInternal::ReduceCopyOutImpl<T>(dstAddr, srcAddr, dimA);
    } else if (dimR <= vlSize) {
        uint16_t repeatTime = FindClosestPowerOfTwo(dimR);
        repeatTime = (1 << repeatTime) < dimR ? repeatTime : repeatTime - 1;
        ReduceProdARLessThanVLVF<T, Reg::RegTraitNumOne, vlSize>(dstAddr, srcAddr, dimA, dimR, repeatTime);
    } else {
        ReduceAROverVLImpl<
            T, Reg::RegTraitNumOne, vlSize, Reg::Mul<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>, ReduceProd<T>,
            isReuseSource>(dstAddr, srcAddr, tmpAddr, dimA, dimR);
    }
}

template <typename T, typename pattern, bool isReuseSource = false>
__aicore__ inline void ReduceProdImpl(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t srcShape[], bool srcInnerPad)
{
    CHECK_FUNC_HIGHLEVEL_API(ReduceProd, (T, pattern), (dst, src, sharedTmpBuffer, srcShape, srcInnerPad, srcShape[1]));

    CheckTensorPos<T>(dst, Hardware::UB, "dst", "VECIN/VECCALC/VECOUT", "ReduceProd");
    CheckTensorPos<T>(src, Hardware::UB, "src", "VECIN/VECCALC/VECOUT", "ReduceProd");
    CheckTensorPos<uint8_t>(sharedTmpBuffer, Hardware::UB, "sharedTmpBuffer", "VECIN/VECCALC/VECOUT", "ReduceProd");
    static_assert(SupportType<T, float>(), "ReduceProd only support float data type on current device!");
    static_assert(
        std::is_same_v<pattern, Pattern::Reduce::AR> || std::is_same_v<pattern, Pattern::Reduce::RA>,
        "ReduceProd only support AR and RA pattern on current device!");

    __ubuf__ T* dstAddr = (__ubuf__ T*)dst.GetPhyAddr();
    __ubuf__ T* srcAddr = (__ubuf__ T*)src.GetPhyAddr();
    LocalTensor<T> tmpBuf = sharedTmpBuffer.ReinterpretCast<T>();
    __ubuf__ T* tmpAddr = (__ubuf__ T*)tmpBuf.GetPhyAddr();
    if constexpr (std::is_same_v<pattern, Pattern::Reduce::AR>) {
        ReduceProdARImpl<T, isReuseSource>(dstAddr, srcAddr, tmpAddr, srcShape[0], srcShape[1]);
    } else {
        ReduceRAImpl<
            T, Reg::RegTraitNumOne, Reg::Mul<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>, isReuseSource>(
            dstAddr, srcAddr, tmpAddr, srcShape[1], srcShape[0]);
    }
}
} // namespace Internal
} // namespace AscendC
#endif // IMPL_REDUCE_REDUCE_PROD_REDUCE_PROD_C310_IMPL_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_PROD_REDUCE_PROD_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_REDUCE_PROD_REDUCE_PROD_C310_IMPL_H__
#endif
