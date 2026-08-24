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
#warning                                                                                                               \
    "impl/tensor_api/arch/vector/experimental/composite_compute_impl.h is internal and must not be used directly."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_COMPUTE_IMPL
#endif

#ifndef IMPL_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_COMPOSITE_COMPUTE_IMPL_H
#define IMPL_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_COMPOSITE_COMPUTE_IMPL_H

#include "impl/tensor_api/arch/utils/reg_compute_traits.h"

namespace asc {
namespace te {
namespace experimental {

template <typename T, typename scalar_type>
__simd_callee__ inline decltype(auto) axpy(
    const reg_tensor<T>& dst, const reg_tensor<T>& src, const scalar_type& scalar)
{
    static_assert(detail::supports_axpy_v<T>, "axpy supports half and float");
    static_assert(Std::is_same_v<Std::remove_cvref_t<scalar_type>, T>,
        "axpy requires the scalar and reg_tensor element types to match");
    reg_tensor<T> result = dst;
    asc_axpy(result.reg, src.reg, scalar, dst.mask);
    return result;
}

template <typename T>
__simd_callee__ inline decltype(auto) absdiff(const reg_tensor<T>& src0, const reg_tensor<T>& src1)
{
    static_assert(detail::supports_absdiff_v<T>, "absdiff supports half and float");
    reg_tensor<T> dst;
    asc_abs_sub(dst.reg, src0.reg, src1.reg, src0.mask);
    dst.mask = src0.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline decltype(auto) exp_sub(const reg_tensor<T>& src0, const reg_tensor<T>& src1)
{
    static_assert(detail::supports_exp_sub_v<T>, "exp_sub supports half and float");
    reg_tensor<float> dst;
    if constexpr (Std::is_same_v<T, half>) {
        // Preserve the deprecated half overload's PART_EVEN behavior.
        asc_exp_sub_half2float(dst.reg, src0.reg, src1.reg, src0.mask,
            std::integral_constant<asc_position_mode, asc_position_mode::EVEN> {});
    } else {
        asc_exp_sub(dst.reg, src0.reg, src1.reg, src0.mask);
    }
    dst.mask = src0.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline decltype(auto) fma(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1, const reg_tensor<T>& src2)
{
    static_assert(detail::supports_fma_v<T>, "fma supports half and float");
    reg_tensor<T> dst = src0;
    asc_madd(dst.reg, src1.reg, src2.reg, src0.mask);
    dst.mask = src0.mask;
    return dst;
}

__simd_callee__ inline decltype(auto) muls_cast(const reg_tensor<float>& src, const float& scalar)
{
    reg_tensor<float> product;
    asc_mul_scalar(product.reg, src.reg, scalar, src.mask);
    reg_tensor<half> dst;
    asc_float2half_rna(dst.reg, product.reg, src.mask, ASC_POSITION_EVEN);
    dst.mask = src.mask;
    return dst;
}

} // namespace experimental
} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_ARCH_VECTOR_EXPERIMENTAL_COMPOSITE_COMPUTE_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_COMPUTE_IMPL)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_COMPUTE_IMPL
#endif
