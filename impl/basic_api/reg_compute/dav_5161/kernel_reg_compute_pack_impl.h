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
 * \file kernel_reg_compute_pack_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_5161/kernel_reg_compute_pack_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_pack_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_PACK_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_PACK_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_PACK_IMPL_H

#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_copy_intf.h"
#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_vec_binary_scalar_intf.h"
#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_vec_duplicate_intf.h"

namespace AscendC {
namespace Reg {
template <
    typename T = DefaultType, typename U = DefaultType, HighLowPart part = HighLowPart::LOWEST, typename S, typename V>
__simd_callee__ inline void PackImpl(S& dstReg, V& srcReg)
{
    using ActualT = typename S::ActualT;
    using ActualU = typename V::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(Std::is_same_v<U, DefaultType> || Std::is_same_v<U, ActualU>, "U type is not correct!");
    static_assert(
        (SupportType<
            Tuple<ActualT, ActualU>, Tuple<uint8_t, uint16_t>, Tuple<uint8_t, int16_t>, Tuple<uint16_t, uint32_t>,
            Tuple<uint16_t, int32_t>>()),
        "unsupport datatype");
    constexpr auto partValue = std::integral_constant<::HiloPart, static_cast<::HiloPart>(part)>();
    vpack(dstReg, srcReg, partValue);
}

template <
    typename T = DefaultType, typename U = DefaultType, HighLowPart part = HighLowPart::LOWEST, typename S, typename V>
__simd_callee__ inline void UnPackImpl(S& dstReg, V& srcReg)
{
    using ActualT = typename S::ActualT;
    using ActualU = typename V::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(Std::is_same_v<U, DefaultType> || Std::is_same_v<U, ActualU>, "U type is not correct!");
    static_assert(
        (SupportType<
            Tuple<ActualT, ActualU>, Tuple<uint32_t, uint16_t>, Tuple<int32_t, int16_t>, Tuple<uint16_t, uint8_t>,
            Tuple<int16_t, int8_t>>()),
        "unsupport datatype");
    constexpr auto partValue = std::integral_constant<::HiloPart, static_cast<::HiloPart>(part)>();
    vunpack(dstReg, srcReg, partValue);
}
} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_PACK_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_PACK_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_PACK_IMPL__
#endif
