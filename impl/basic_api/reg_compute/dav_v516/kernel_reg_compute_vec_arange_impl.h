/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file kernel_reg_compute_vec_arange_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_v516/kernel_reg_compute_vec_arange_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_vec_arange_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_ARANGE_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_VEC_ARANGE_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_VEC_ARANGE_IMPL_H

#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_vec_duplicate_intf.h"

namespace AscendC {
namespace Reg {
template <IndexOrder order = IndexOrder::INCREASE_ORDER, typename T, typename U>
__simd_callee__ inline void ArangeB64Impl(U& dstReg, T scalarValue)
{
    using ActualT = typename U::ActualT;
    static_assert((SupportType<ActualT, int64_t>()), "ArangeB64Impl only support B64 data type");
    constexpr auto orderMode = std::integral_constant<::Order, static_cast<::Order>(order)>();
    static_assert(CheckRegTrait<U, RegTraitNumTwo>(), "ArangeB64Impl only support RegTraitNumTwo");
    MaskReg maskReg = AscendC::Reg::CreateMask<uint8_t, AscendC::Reg::MaskPattern::ALL>();
    Duplicate((RegTensor<int32_t>&)dstReg.reg[1], int32_t(0), maskReg);
    Arange<DefaultType, order>((RegTensor<int32_t>&)dstReg.reg[0], int32_t(0));
    Adds(dstReg, dstReg, scalarValue, maskReg);
}

template <typename T = DefaultType, IndexOrder order = IndexOrder::INCREASE_ORDER, typename U, typename S>
__simd_callee__ inline void ArangeImpl(S& dstReg, U scalarValue)
{
    using ActualT = typename S::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        (SupportType<ActualT, int8_t, int16_t, int32_t, float, half, int64_t>()),
        "current Arange data type is not supported on current device!");
    static_assert(Std::is_convertible<U, ActualT>(), "scalarValue data type could be converted to RegTensor data type");
    constexpr auto orderMode = std::integral_constant<::Order, static_cast<::Order>(order)>();
    if constexpr (sizeof(ActualT) != 8) {
        vci(dstReg, scalarValue, orderMode);
    } else {
        if constexpr (CheckRegTrait<S, RegTraitNumOne>()) {
            RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
            ArangeB64Impl(traitTwoDstReg, scalarValue);
            B64TraitTwoToTraitOne(dstReg, traitTwoDstReg);
        } else if constexpr (CheckRegTrait<S, RegTraitNumTwo>()) {
            S dstTemp;
            ArangeB64Impl<order, U, S>(dstTemp, scalarValue);
            dstReg = dstTemp;
        }
    }
}
} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_VEC_ARANGE_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_ARANGE_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_ARANGE_IMPL__
#endif
