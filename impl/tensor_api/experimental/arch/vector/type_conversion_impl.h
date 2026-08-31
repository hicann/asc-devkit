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
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_TYPE_CONVERSION_IMPL_H
#endif

#ifndef IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_TYPE_CONVERSION_IMPL_H
#define IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_TYPE_CONVERSION_IMPL_H

#include "tensor_api/experimental/arch/vector/type_conversion.h"
#include "impl/tensor_api/utils/constant_impl.h"

namespace asc {
namespace te {
namespace experimental {

namespace detail {

template <typename DstType, typename SrcType, cast_layout Layout, cast_round_mode Round, cast_sat_mode Sat>
struct reg_cast_op {
    static constexpr bool supported = false;
};
template <typename T>
struct cast_element_bits {
    using data_type = Std::remove_cvref_t<T>;
    static constexpr uint32_t value = is_b4_type<data_type> ? 4U : sizeof(data_type) * 8U;
};

} // namespace detail
} // namespace experimental
} // namespace te
} // namespace asc

#define ASC_CAST_POSITION_2_zero ASC_POSITION_EVEN
#define ASC_CAST_POSITION_2_one ASC_POSITION_ODD
#define ASC_CAST_POSITION_2_two ASC_POSITION_EVEN
#define ASC_CAST_POSITION_2_three ASC_POSITION_ODD
#define ASC_CAST_POSITION_4_zero ASC_DISPERSE_FIRST_QUARTER
#define ASC_CAST_POSITION_4_one ASC_DISPERSE_SECOND_QUARTER
#define ASC_CAST_POSITION_4_two ASC_DISPERSE_THIRD_QUARTER
#define ASC_CAST_POSITION_4_three ASC_DISPERSE_FOURTH_QUARTER

#define ASC_REG_CAST_OP(TO, FROM, LAYOUT, ROUND, SAT, FUNC)                                         \
    template <>                                                                                     \
    struct reg_cast_op<TO, FROM, cast_layout::LAYOUT, cast_round_mode::ROUND, cast_sat_mode::SAT> { \
        static constexpr bool supported = true;                                                     \
                                                                                                    \
        template <typename DstReg, typename SrcReg>                                                 \
        __simd_callee__ static void run(DstReg& dst, SrcReg src, vector_bool mask)                  \
        {                                                                                           \
            constexpr uint32_t dst_bits = cast_element_bits<TO>::value;                             \
            constexpr uint32_t src_bits = cast_element_bits<FROM>::value;                           \
            if constexpr (dst_bits == src_bits) {                                                   \
                FUNC(dst, src, mask);                                                               \
            } else if constexpr (dst_bits == 2U * src_bits || src_bits == 2U * dst_bits) {          \
                FUNC(dst, src, mask, ASC_CAST_POSITION_2_##LAYOUT);                                 \
            } else if constexpr (dst_bits == 4U * src_bits || src_bits == 4U * dst_bits) {          \
                FUNC(dst, src, mask, ASC_CAST_POSITION_4_##LAYOUT);                                 \
            } else {                                                                                \
                static_assert(sizeof(DstReg) == 0, "unsupported register cast width ratio");        \
            }                                                                                       \
        }                                                                                           \
    }

#define ASC_REG_CAST_OP_NO_POSITION(TO, FROM, LAYOUT, ROUND, SAT, FUNC)                             \
    template <>                                                                                     \
    struct reg_cast_op<TO, FROM, cast_layout::LAYOUT, cast_round_mode::ROUND, cast_sat_mode::SAT> { \
        static constexpr bool supported = true;                                                     \
                                                                                                    \
        template <typename DstReg, typename SrcReg>                                                 \
        __simd_callee__ static void run(DstReg& dst, SrcReg src, vector_bool mask)                  \
        {                                                                                           \
            FUNC(dst, src, mask);                                                                   \
        }                                                                                           \
    }

#include "impl/tensor_api/experimental/arch/vector/type_conversion_table.h"

#undef ASC_REG_CAST_OP
#undef ASC_REG_CAST_OP_NO_POSITION
#undef ASC_CAST_POSITION_2_zero
#undef ASC_CAST_POSITION_2_one
#undef ASC_CAST_POSITION_2_two
#undef ASC_CAST_POSITION_2_three
#undef ASC_CAST_POSITION_4_zero
#undef ASC_CAST_POSITION_4_one
#undef ASC_CAST_POSITION_4_two
#undef ASC_CAST_POSITION_4_three

namespace asc {
namespace te {
namespace experimental {
namespace detail {

template <typename DstType, typename SrcType, cast_layout Layout, cast_round_mode Round, cast_sat_mode Sat>
__simd_callee__ inline reg_tensor<DstType> cast_impl(const reg_tensor<SrcType>& src)
{
    static_assert(
        !AscendC::Std::is_same_v<DstType, SrcType>,
        "reg_tensor cast requires different source and destination element types");

    using op = reg_cast_op<DstType, SrcType, Layout, Round, Sat>;
    static_assert(op::supported, "register cast type or option combination is unsupported on the current architecture");

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
        static_assert(detail::is_supported<T>::value, "reg_tensor trunc supports only half, bfloat16_t, and float");
    }
}

} // namespace experimental
} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_TYPE_CONVERSION_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_TYPE_CONVERSION_IMPL_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_TYPE_CONVERSION_IMPL_H
#endif
