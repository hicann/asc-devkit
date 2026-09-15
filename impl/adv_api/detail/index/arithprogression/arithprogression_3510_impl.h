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
 * \file arithprogression_3510_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/index/arithprogression/arithprogression_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/index/arithprogression.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ARITHPROGRESSION_C310_IMPL_H__
#endif

#ifndef IMPL_INDEX_ARITHPROGRESSION_ARITHPROGRESSION_C310_IMPL_H
#define IMPL_INDEX_ARITHPROGRESSION_ARITHPROGRESSION_C310_IMPL_H

#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../basic_api/kernel_utils.h"
#include "../../../../basic_api/kernel_log.h"
namespace AscendC {
// Generating an underlying arithmetic sequence through scalar operations.
template <typename RegT, typename ScalarT>
__simd_callee__ inline void GetBaseArithProgression(RegT& dstReg, const ScalarT firstValue, const ScalarT diffValue)
{
    Reg::MaskReg fullMask = Reg::CreateMask<uint8_t>();
    Reg::Arange(dstReg, ScalarT(0));
    Reg::Muls(dstReg, dstReg, diffValue, fullMask);
    Reg::Adds(dstReg, dstReg, firstValue, fullMask);
}

template <typename T, const Reg::RegTrait& regTrait>
__simd_vf__ inline void VfCallArithProgression(
    __ubuf__ T* dstLocalAddr, const T firstValue, const T diffValue, const int32_t count, const uint16_t repeatTimes)
{
    Reg::RegTensor<T, regTrait> tmpReg;
    Reg::RegTensor<T, regTrait> stepReg;
    Reg::MaskReg fullMask = Reg::CreateMask<T, Reg::MaskPattern::ALL, regTrait>();
    GetBaseArithProgression(tmpReg, firstValue, diffValue);
    uint32_t sreg = static_cast<uint32_t>(count);
    Reg::MaskReg preg;
    const uint32_t sregLower = static_cast<uint32_t>(regTrait.REG_NUM * ONE_REPEAT_BYTE_SIZE / sizeof(T));
    Reg::Duplicate(stepReg, static_cast<T>(sregLower));
    Reg::Muls(stepReg, stepReg, diffValue, fullMask);
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        preg = Reg::UpdateMask<T, regTrait>(sreg);
        Reg::StoreAlign(dstLocalAddr + i * sregLower, tmpReg, preg);
        Reg::Add(tmpReg, tmpReg, stepReg, fullMask);
    }
}

template <typename T>
__aicore__ inline void ArithProgressionImpl(
    const LocalTensor<T>& dstLocal, const T firstValue, const T diffValue, const int32_t count)
{
    ASCENDC_ASSERT(
        (dstLocal.GetSize() >= count), { KERNEL_LOG(KERNEL_ERROR, "dst length must equal with Arange length"); });
    ASCENDC_ASSERT((static_cast<float>(diffValue) >= static_cast<float>(0)), {
        KERNEL_LOG(KERNEL_ERROR, "diff value must bigger than 0");
    });
    static_assert(
        SupportType<T, int16_t, int32_t, half, float, int64_t>(),
        "current data type is not supported on current device!");

    __ubuf__ T* dstLocalAddr = (__ubuf__ T*)dstLocal.GetPhyAddr();
    if constexpr (sizeof(T) != 8) {
        uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(count, ONE_REPEAT_BYTE_SIZE / sizeof(T)));
        VfCallArithProgression<T, Reg::RegTraitNumOne>(dstLocalAddr, firstValue, diffValue, count, repeatTimes);
    } else {
        uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(count, 2 * ONE_REPEAT_BYTE_SIZE / sizeof(T)));
        VfCallArithProgression<T, Reg::RegTraitNumTwo>(dstLocalAddr, firstValue, diffValue, count, repeatTimes);
    }
}

template <typename T>
__aicore__ inline __in_pipe__(S) __out_pipe__(V, S) void ArithProgression(
    const LocalTensor<T>& dstLocal, const T firstValue, const T diffValue, const int32_t count)
{
    ArithProgressionImpl(dstLocal, firstValue, diffValue, count);
}
} // namespace AscendC

#endif // IMPL_INDEX_ARITHPROGRESSION_ARITHPROGRESSION_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ARITHPROGRESSION_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ARITHPROGRESSION_C310_IMPL_H__
#endif
