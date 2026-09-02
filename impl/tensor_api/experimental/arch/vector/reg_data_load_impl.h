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
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_DATA_LOAD_IMPL_H
#endif

#ifndef IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_DATA_LOAD_IMPL_H
#define IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_DATA_LOAD_IMPL_H

#include "impl/tensor_api/experimental/arch/utils/reg_utils.h"
#include "tensor_api/tensor/tensor.h"

namespace asc {
namespace te {
namespace experimental {
namespace detail {

template <load_sideband_mode sideband_mode>
struct load_from_ptr_impl;

template <>
struct load_from_ptr_impl<load_sideband_mode::direct> {
    template <typename DataType, typename Pointer>
    __simd_callee__ static inline reg_tensor<DataType> run(Pointer src)
    {
        static_assert(
            is_b4_type_v<DataType> || is_b8_type_v<DataType> || is_b16_type_v<DataType> || is_b32_type_v<DataType> ||
                is_b64_type_v<DataType>,
            "load direct supports b4, b8, b16, b32, and b64 element types only");
        reg_tensor<DataType> dst;
        asc_loadalign(dst.reg, src);
        return dst;
    }
};

template <>
struct load_from_ptr_impl<load_sideband_mode::downsample> {
    template <typename DataType, typename Pointer>
    __simd_callee__ static inline reg_tensor<DataType> run(Pointer src)
    {
        static_assert(
            is_b4_type_v<DataType> || is_b8_type_v<DataType> || is_b16_type_v<DataType>,
            "load downsample supports b4, b8, and b16 element types only");
        reg_tensor<DataType> dst;
        asc_loadalign_downsample(dst.reg, src);
        return dst;
    }
};

template <>
struct load_from_ptr_impl<load_sideband_mode::unpack> {
    template <typename DataType, typename Pointer>
    __simd_callee__ static inline reg_tensor<DataType> run(Pointer src)
    {
        static_assert(
            is_b4_type_v<DataType> || is_b8_type_v<DataType> || is_b16_type_v<DataType> || is_b32_type_v<DataType>,
            "load unpack supports b4, b8, b16, and b32 element types only");
        reg_tensor<DataType> dst;
        asc_loadalign_unpack(dst.reg, src);
        return dst;
    }
};

template <>
struct load_from_ptr_impl<load_sideband_mode::unpack4> {
    template <typename DataType, typename Pointer>
    __simd_callee__ static inline reg_tensor<DataType> run(Pointer src)
    {
        static_assert(
            is_b4_type_v<DataType> || is_b8_type_v<DataType>, "load unpack4 supports b4 and b8 element types only");
        reg_tensor<DataType> dst;
        asc_loadalign_unpack4(dst.reg, src);
        return dst;
    }
};

template <>
struct load_from_ptr_impl<load_sideband_mode::upsample> {
    template <typename DataType, typename Pointer>
    __simd_callee__ static inline reg_tensor<DataType> run(Pointer src)
    {
        static_assert(
            is_b4_type_v<DataType> || is_b8_type_v<DataType> || is_b16_type_v<DataType>,
            "load upsample supports b4, b8, and b16 element types only");
        reg_tensor<DataType> dst;
        asc_loadalign_upsample(dst.reg, src);
        return dst;
    }
};

template <>
struct load_from_ptr_impl<load_sideband_mode::deintlv> {
    template <typename DataType, typename Pointer>
    __simd_callee__ static inline reg_pair<DataType> run(Pointer src)
    {
        static_assert(
            is_b4_type_v<DataType> || is_b8_type_v<DataType> || is_b16_type_v<DataType> || is_b32_type_v<DataType>,
            "load deintlv supports b4, b8, b16, and b32 element types only");
        reg_pair<DataType> dst;
        asc_loadalign_deintlv(dst.first.reg, dst.second.reg, src);
        return dst;
    }
};

template <load_sideband_mode sideband_mode, typename DataType, typename Pointer>
__simd_callee__ inline decltype(auto) load_from_ptr(Pointer src)
{
    return load_from_ptr_impl<sideband_mode>::template run<DataType>(src);
}

template <broadcast_mode brc_mode>
struct load_broadcast_from_ptr_impl;

template <>
struct load_broadcast_from_ptr_impl<broadcast_mode::elem> {
    template <typename DataType, typename Pointer>
    __simd_callee__ static inline reg_tensor<DataType> run(Pointer src)
    {
        static_assert(
            is_b4_type_v<DataType> || is_b8_type_v<DataType> || is_b16_type_v<DataType> || is_b32_type_v<DataType>,
            "load_broadcast elem does not support this element type");
        reg_tensor<DataType> dst;
        asc_loadalign_brc_elem(dst.reg, src);
        return dst;
    }
};

template <>
struct load_broadcast_from_ptr_impl<broadcast_mode::datablock> {
    template <typename DataType, typename Pointer>
    __simd_callee__ static inline reg_tensor<DataType> run(Pointer src)
    {
        static_assert(
            is_b4_type_v<DataType> || is_b8_type_v<DataType> || is_b16_type_v<DataType> || is_b32_type_v<DataType>,
            "load_broadcast datablock does not support this element type");
        reg_tensor<DataType> dst;
        asc_loadalign_brc_datablock(dst.reg, src);
        return dst;
    }
};

template <>
struct load_broadcast_from_ptr_impl<broadcast_mode::elem2datablock> {
    template <typename DataType, typename Pointer>
    __simd_callee__ static inline reg_tensor<DataType> run(Pointer src)
    {
        static_assert(
            is_b16_type_v<DataType> || is_b32_type_v<DataType>,
            "load_broadcast elem2datablock does not support this element type");
        reg_tensor<DataType> dst;
        asc_loadalign_brc_elem2datablock(dst.reg, src);
        return dst;
    }
};

template <broadcast_mode brc_mode, typename DataType, typename Pointer>
__simd_callee__ inline reg_tensor<DataType> load_broadcast_from_ptr(Pointer src)
{
    return load_broadcast_from_ptr_impl<brc_mode>::template run<DataType>(src);
}

} // namespace detail

template <load_sideband_mode sideband_mode, typename Tensor, typename Coord>
__simd_callee__ inline decltype(auto) load(const Tensor& tensor, const Coord& coord)
{
    using engine_type = typename Tensor::engine_type;
    using data_type = typename Tensor::data_type;
    static_assert(
        Std::is_same_v<get_mem_location<engine_type>, location::ub>, "load only supports tensors located in UB");
    TENSOR_API_DEBUG_CHECK(debug_check_coord, tensor.layout(), coord, "load");

    auto src_engine = tensor.engine() + tensor.layout()(coord);
    return detail::load_from_ptr<sideband_mode, data_type>(src_engine.begin().get());
}

template <load_sideband_mode sideband_mode, typename Tensor>
__simd_callee__ inline decltype(auto) load(const Tensor& tensor)
{
    using engine_type = typename Tensor::engine_type;
    using data_type = typename Tensor::data_type;
    static_assert(
        Std::is_same_v<get_mem_location<engine_type>, location::ub>, "load only supports tensors located in UB");

    return detail::load_from_ptr<sideband_mode, data_type>(tensor.engine().begin().get());
}

template <broadcast_mode brc_mode, typename Tensor, typename Coord>
__simd_callee__ inline decltype(auto) load_broadcast(const Tensor& tensor, const Coord& coord)
{
    using engine_type = typename Tensor::engine_type;
    using data_type = typename Tensor::data_type;
    static_assert(
        Std::is_same_v<get_mem_location<engine_type>, location::ub>,
        "load_broadcast only supports tensors located in UB");
    TENSOR_API_DEBUG_CHECK(debug_check_coord, tensor.layout(), coord, "load_broadcast");

    auto src_engine = tensor.engine() + tensor.layout()(coord);
    return detail::load_broadcast_from_ptr<brc_mode, data_type>(src_engine.begin().get());
}

template <broadcast_mode brc_mode, typename Tensor>
__simd_callee__ inline decltype(auto) load_broadcast(const Tensor& tensor)
{
    using engine_type = typename Tensor::engine_type;
    using data_type = typename Tensor::data_type;
    static_assert(
        Std::is_same_v<get_mem_location<engine_type>, location::ub>,
        "load_broadcast only supports tensors located in UB");

    return detail::load_broadcast_from_ptr<brc_mode, data_type>(tensor.engine().begin().get());
}

} // namespace experimental
} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_DATA_LOAD_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_DATA_LOAD_IMPL_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_DATA_LOAD_IMPL_H
#endif
