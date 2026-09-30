/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning "impl/tensor_api/experimental/arch/vector/composite_compute_impl.h is internal and must not be used directly."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_COMPUTE_IMPL
#endif

#ifndef IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_COMPOSITE_COMPUTE_IMPL_H
#define IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_COMPOSITE_COMPUTE_IMPL_H

#include "impl/tensor_api/experimental/arch/utils/reg_utils.h"

namespace asc {
namespace te {
namespace experimental {

template <typename T>
__simd_callee__ inline reg_tensor<T> axpy(reg_tensor<T>& dst, const reg_tensor<T>& src, const T& scalar)
{
    static_assert(detail::supports_axpy_v<T>, "axpy does not support this element type");
    asc_axpy(dst.reg, src.reg, scalar, dst.mask);
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> abs_diff(const reg_tensor<T>& src0, const reg_tensor<T>& src1)
{
    static_assert(detail::supports_abs_diff_v<T>, "abs_diff does not support this element type");
    reg_tensor<T> dst;
    asc_abs_sub(dst.reg, src0.reg, src1.reg, src0.mask);
    dst.mask = src0.mask;
    return dst;
}

template <typename T, typename U>
__simd_callee__ inline reg_tensor<T> exp_diff(const reg_tensor<U>& src0, const reg_tensor<U>& src1)
{
    static_assert(
        ::AscendC::Std::is_same_v<T, float> && ::AscendC::Std::is_same_v<U, float>,
        "exp_diff without a position only supports float input and float output");
    reg_tensor<T> dst(src0.mask);
    dst.reg = asc_exp_sub(src0.reg, src1.reg, src0.mask);
    return dst;
}

template <typename T, typename U, typename PositionType>
__simd_callee__ inline reg_tensor<T> exp_diff(
    const reg_tensor<U>& src0, const reg_tensor<U>& src1, PositionType src_pos)
{
    static_assert(
        ::AscendC::Std::is_same_v<T, float> && ::AscendC::Std::is_same_v<U, half>,
        "exp_diff with a position only supports half input and float output");
    using pos_type = ::AscendC::Std::remove_cvref_t<PositionType>;
    static_assert(
        ::AscendC::Std::is_one_of_v<
            pos_type, ::AscendC::Std::remove_cvref_t<decltype(ASC_POSITION_EVEN)>,
            ::AscendC::Std::remove_cvref_t<decltype(ASC_POSITION_ODD)>>,
        "exp_diff position supports ASC_POSITION_EVEN and ASC_POSITION_ODD");
    reg_tensor<T> dst(src0.mask);
    dst.reg = asc_exp_sub_half2float(src0.reg, src1.reg, src0.mask, src_pos);
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> madd(reg_tensor<T>& dst, const reg_tensor<T>& src0, const reg_tensor<T>& src1)
{
    static_assert(detail::supports_madd_v<T>, "madd does not support this element type");
    asc_madd(dst.reg, src0.reg, src1.reg, dst.mask);
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> mula(reg_tensor<T>& dst, const reg_tensor<T>& src0, const reg_tensor<T>& src1)
{
    static_assert(detail::supports_mula_v<T>, "mula does not support this element type");
    asc_mula(dst.reg, src0.reg, src1.reg, dst.mask);
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> fma(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1, const reg_tensor<T>& src2)
{
    static_assert(detail::supports_fma_v<T>, "fma does not support this element type");
    reg_tensor<T> dst(src0.mask);
    dst.reg = asc_fma(src0.reg, src1.reg, src2.reg, src0.mask);
    return dst;
}

template <cast_layout Layout, typename T, typename U>
__simd_callee__ inline reg_tensor<T> muls_cast(const reg_tensor<U>& src, const U& scalar)
{
    static_assert(detail::supports_muls_cast_v<T, U>, "muls_cast supports only float to half");
    static_assert(
        Layout == cast_layout::zero || Layout == cast_layout::one,
        "muls_cast layout supports cast_layout::zero and cast_layout::one");
    reg_tensor<T> dst(src.mask);
    if constexpr (Layout == cast_layout::zero) {
        dst.reg = asc_mul_scalar_float2half_rn(src.reg, scalar, src.mask, ASC_POSITION_EVEN);
    } else {
        dst.reg = asc_mul_scalar_float2half_rn(src.reg, scalar, src.mask, ASC_POSITION_ODD);
    }
    return dst;
}

} // namespace experimental
} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_COMPOSITE_COMPUTE_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_COMPUTE_IMPL)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_COMPUTE_IMPL
#endif
