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
    "type_conversion_impl.h is internal; include <tensor_api/experimental/arch/vector/type_conversion.h> instead."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_TYPE_CONVERSION_IMPL_H__
#endif

#ifndef IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_TYPE_CONVERSION_IMPL_H
#define IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_TYPE_CONVERSION_IMPL_H

#if !defined(INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_TYPE_CONVERSION_H)
#include "tensor_api/experimental/arch/vector/type_conversion.h"
#endif
#include "c_api/reg_compute/reg_convert.h"
#include "impl/tensor_api/arch/utils/arch_utils.h"

namespace asc {
namespace te {
namespace experimental {

namespace detail {

template <typename DstType, typename SrcType, cast_layout Layout, cast_round_mode Round, cast_sat_mode Sat>
struct reg_cast_op {
    static constexpr bool supported = false;
};

} // namespace detail
} // namespace experimental
} // namespace te
} // namespace asc

#define ASC_REG_CAST_OP(TO, FROM, LAYOUT, ROUND, SAT, FUNC)                                                           \
    template <>                                                                                                       \
    struct reg_cast_op<TO, FROM, cast_layout::LAYOUT, cast_round_mode::ROUND, cast_sat_mode::SAT> {                  \
        static constexpr bool supported = true;                                                                       \
                                                                                                                       \
        template <typename DstReg, typename SrcReg>                                                                   \
        __simd_callee__ static void run(DstReg& dst, SrcReg src, vector_bool mask)                                   \
        {                                                                                                              \
            FUNC(dst, src, mask);                                                                                      \
        }                                                                                                              \
    }

#include "impl/tensor_api/experimental/arch/vector/type_conversion_table.h"

#undef ASC_REG_CAST_OP

namespace asc {
namespace te {
namespace experimental {
namespace detail {

template <typename DstType, typename SrcType, cast_layout Layout, cast_round_mode Round, cast_sat_mode Sat>
__simd_callee__ inline reg_tensor<DstType> cast_impl(const reg_tensor<SrcType>& src)
{
    static_assert(!AscendC::Std::is_same_v<DstType, SrcType>,
        "reg_tensor cast requires different source and destination element types");

    using op = reg_cast_op<DstType, SrcType, Layout, Round, Sat>;
    static_assert(op::supported,
        "register cast type or option combination is unsupported on the current architecture");

    reg_tensor<DstType> dst;
    op::run(dst.reg, src.reg, src.mask);
    dst.mask = src.mask;
    return dst;
}

} // namespace detail

namespace detail {

template <typename T>
struct is_supported {
    static constexpr bool value = false;
};

template <>
struct is_supported<half> {
    static constexpr bool value = true;
};

template <>
struct is_supported<bfloat16_t> {
    static constexpr bool value = true;
};

template <>
struct is_supported<float> {
    static constexpr bool value = true;
};

} // namespace detail

template <typename DstType, const cast_options& opt, typename SrcType>
__simd_callee__ inline reg_tensor<DstType> cast(const reg_tensor<SrcType>& src)
{
    return detail::cast_impl<DstType, SrcType, opt.layout, opt.round, opt.sat>(src);
}

template <typename T>
__simd_callee__ inline reg_tensor<T> trunc(const reg_tensor<T>& src)
{
    if constexpr (detail::is_supported<T>::value) {
        reg_tensor<T> dst;
        asc_trunc(dst.reg, src.reg, src.mask);
        dst.mask = src.mask;
        return dst;
    } else {
        static_assert(detail::is_supported<T>::value,
            "reg_tensor trunc supports only half, bfloat16_t, and float");
    }
}

} // namespace experimental
} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_TYPE_CONVERSION_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_TYPE_CONVERSION_IMPL_H__)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_TYPE_CONVERSION_IMPL_H__
#endif
