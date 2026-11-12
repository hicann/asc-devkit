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
#warning \
    "impl/tensor_api/experimental/arch/vector/reg_data_store_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"tensor_api/experimental/arch/vector/reg_data_store.h\"\" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_DATA_STORE_IMPL_H
#endif

#ifndef IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_DATA_STORE_IMPL_H
#define IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_DATA_STORE_IMPL_H

#include "impl/tensor_api/experimental/arch/utils/reg_utils.h"
#include "tensor_api/tensor/tensor_interface.h"

namespace asc {
namespace te {
namespace experimental {
namespace detail {

template <store_sideband_mode sideband_mode>
struct store_to_ptr_impl;

template <>
struct store_to_ptr_impl<store_sideband_mode::direct> {
    template <typename Pointer, typename RegDataType>
    __simd_callee__ static inline void run(Pointer dst, const reg_tensor<RegDataType>& src)
    {
        if constexpr (Std::is_same_v<RegDataType, bool>) {
            asc_storealign(dst, src.reg);
        } else {
            static_assert(
                is_b4_type_v<RegDataType> || is_b8_type_v<RegDataType> || is_b16_type_v<RegDataType> ||
                    is_b32_type_v<RegDataType>,
                "store direct supports b4, b8, b16, and b32 element types only");
            asc_storealign(dst, src.reg, src.mask);
        }
    }
};

template <>
struct store_to_ptr_impl<store_sideband_mode::store_1st> {
    template <typename Pointer, typename RegDataType>
    __simd_callee__ static inline void run(Pointer dst, const reg_tensor<RegDataType>& src)
    {
        static_assert(
            is_b4_type_v<RegDataType> || is_b8_type_v<RegDataType> || is_b16_type_v<RegDataType> ||
                is_b32_type_v<RegDataType>,
            "store store_1st supports b4, b8, b16, and b32 element types only");
        asc_storealign_1st(dst, src.reg);
    }
};

template <>
struct store_to_ptr_impl<store_sideband_mode::pack> {
    template <typename Pointer, typename RegDataType>
    __simd_callee__ static inline void run(Pointer dst, const reg_tensor<RegDataType>& src)
    {
        if constexpr (Std::is_same_v<RegDataType, bool>) {
            asc_storealign_pack(dst, src.reg);
        } else {
            static_assert(
                is_b16_type_v<RegDataType> || is_b32_type_v<RegDataType> || is_b64_type_v<RegDataType>,
                "store pack supports b16, b32, and b64 element types only");
            asc_storealign_pack(dst, src.reg, src.mask);
        }
    }
};

template <>
struct store_to_ptr_impl<store_sideband_mode::pack_quarter> {
    template <typename Pointer, typename RegDataType>
    __simd_callee__ static inline void run(Pointer dst, const reg_tensor<RegDataType>& src)
    {
        static_assert(
            AscendC::Std::is_one_of_v<RegDataType, int32_t, uint32_t, float>,
            "store pack_quarter supports b32 element types (int32_t, uint32_t, and float) only");
        asc_storealign_pack_quarter(dst, src.reg, src.mask);
    }
};

template <>
struct store_to_ptr_impl<store_sideband_mode::intlv> {
    template <typename Pointer, typename RegDataType>
    __simd_callee__ static inline void run(
        Pointer dst, const reg_tensor<RegDataType>& src0, const reg_tensor<RegDataType>& src1)
    {
        static_assert(
            is_b4_type_v<RegDataType> || is_b8_type_v<RegDataType> || is_b16_type_v<RegDataType> ||
                AscendC::Std::is_one_of_v<RegDataType, int32_t, uint32_t>,
            "store intlv supports b4, b8, b16, int32_t, and uint32_t element types only");
        asc_storealign_intlv(dst, src0.reg, src1.reg);
    }
};

template <store_sideband_mode sideband_mode, typename Pointer, typename RegDataType>
__simd_callee__ inline void store_to_ptr(Pointer dst, const reg_tensor<RegDataType>& src)
{
    store_to_ptr_impl<sideband_mode>::run(dst, src);
}

template <store_sideband_mode sideband_mode, typename Pointer, typename RegDataType>
__simd_callee__ inline void store_to_ptr(
    Pointer dst, const reg_tensor<RegDataType>& src0, const reg_tensor<RegDataType>& src1)
{
    store_to_ptr_impl<sideband_mode>::run(dst, src0, src1);
}

} // namespace detail

template <store_sideband_mode sideband_mode, typename Tensor, typename Coord, typename RegDataType>
__simd_callee__ inline void store(Tensor& tensor, const Coord& coord, const reg_tensor<RegDataType>& src)
{
    using engine_type = typename Tensor::engine_type;
    using data_type = typename Tensor::data_type;
    static_assert(
        Std::is_same_v<get_mem_location<engine_type>, location::ub>, "store only supports tensors located in UB");
    static_assert(
        Std::is_same_v<detail::get_reg_data_type<data_type>, RegDataType> ||
            (Std::is_same_v<RegDataType, bool> && Std::is_one_of_v<data_type, uint8_t, uint16_t, uint32_t>),
        "store requires matching tensor and register element types (int4b_t tensors require int4x2_t registers); "
        "mask registers require uint8_t, uint16_t, or uint32_t tensors");
    TENSOR_API_DEBUG_CHECK(debug_check_coord, tensor.layout(), coord, "store");

    auto dst_engine = tensor.engine() + tensor.layout()(coord);
    detail::store_to_ptr<sideband_mode>(dst_engine.begin().get(), src);
}

template <store_sideband_mode sideband_mode, typename Tensor, typename RegDataType>
__simd_callee__ inline void store(Tensor& tensor, const reg_tensor<RegDataType>& src)
{
    using engine_type = typename Tensor::engine_type;
    using data_type = typename Tensor::data_type;
    static_assert(
        Std::is_same_v<get_mem_location<engine_type>, location::ub>, "store only supports tensors located in UB");
    static_assert(
        Std::is_same_v<detail::get_reg_data_type<data_type>, RegDataType> ||
            (Std::is_same_v<RegDataType, bool> && Std::is_one_of_v<data_type, uint8_t, uint16_t, uint32_t>),
        "store requires matching tensor and register element types (int4b_t tensors require int4x2_t registers); "
        "mask registers require uint8_t, uint16_t, or uint32_t tensors");

    detail::store_to_ptr<sideband_mode>(tensor.engine().begin().get(), src);
}

template <store_sideband_mode sideband_mode, typename Tensor, typename Coord, typename RegDataType>
__simd_callee__ inline void store(
    Tensor& tensor, const Coord& coord, const reg_tensor<RegDataType>& src0, const reg_tensor<RegDataType>& src1)
{
    using engine_type = typename Tensor::engine_type;
    using data_type = typename Tensor::data_type;
    static_assert(
        Std::is_same_v<get_mem_location<engine_type>, location::ub>, "store only supports tensors located in UB");
    static_assert(
        Std::is_same_v<detail::get_reg_data_type<data_type>, RegDataType>,
        "store requires matching tensor and register element types (int4b_t tensors require int4x2_t registers)");
    TENSOR_API_DEBUG_CHECK(debug_check_coord, tensor.layout(), coord, "store");

    auto dst_engine = tensor.engine() + tensor.layout()(coord);
    detail::store_to_ptr<sideband_mode>(dst_engine.begin().get(), src0, src1);
}

template <store_sideband_mode sideband_mode, typename Tensor, typename RegDataType>
__simd_callee__ inline void store(
    Tensor& tensor, const reg_tensor<RegDataType>& src0, const reg_tensor<RegDataType>& src1)
{
    using engine_type = typename Tensor::engine_type;
    using data_type = typename Tensor::data_type;
    static_assert(
        Std::is_same_v<get_mem_location<engine_type>, location::ub>, "store only supports tensors located in UB");
    static_assert(
        Std::is_same_v<detail::get_reg_data_type<data_type>, RegDataType>,
        "store requires matching tensor and register element types (int4b_t tensors require int4x2_t registers)");

    detail::store_to_ptr<sideband_mode>(tensor.engine().begin().get(), src0, src1);
}

template <typename Tensor, typename T, typename Index>
__simd_callee__ inline void store_scatter(Tensor& tensor, const reg_tensor<T>& src, const reg_tensor<Index>& index)
{
    using engine_type = typename Tensor::engine_type;
    using data_type = typename Tensor::data_type;
    static_assert(
        Std::is_same_v<get_mem_location<engine_type>, location::ub>,
        "store_scatter only supports tensors located in UB");
    static_assert(Std::is_same_v<data_type, T>, "store_scatter requires matching tensor and register element types");
    asc_scatter(tensor.engine().begin().get(), src.reg, index.reg, src.mask);
}

template <typename Tensor, typename Coord, typename T, typename Index>
__simd_callee__ inline void store_scatter(
    Tensor& tensor, const Coord& coord, const reg_tensor<T>& src, const reg_tensor<Index>& index)
{
    using engine_type = typename Tensor::engine_type;
    using data_type = typename Tensor::data_type;
    static_assert(
        Std::is_same_v<get_mem_location<engine_type>, location::ub>,
        "store_scatter only supports tensors located in UB");
    static_assert(Std::is_same_v<data_type, T>, "store_scatter requires matching tensor and register element types");
    TENSOR_API_DEBUG_CHECK(debug_check_coord, tensor.layout(), coord, "store_scatter");

    auto dst_engine = tensor.engine() + tensor.layout()(coord);
    asc_scatter(dst_engine.begin().get(), src.reg, index.reg, src.mask);
}

template <typename Tensor, typename RegDataType>
__simd_callee__ inline void unalign_store::store(Tensor dst_tile, const reg_tensor<RegDataType>& src, uint32_t count)
{
    using engine_type = typename Tensor::engine_type;
    using data_type = typename Tensor::data_type;
    using reg_data_type = detail::get_reg_data_type<data_type>;
    static_assert(
        Std::is_same_v<get_mem_location<engine_type>, location::ub>,
        "unalign_store only supports tensors located in UB");
    static_assert(
        Std::is_same_v<reg_data_type, RegDataType>,
        "unalign_store requires matching tensor and register element types");
    static_assert(
        detail::is_b4_type_v<reg_data_type> || detail::is_b8_type_v<reg_data_type> ||
            detail::is_b16_type_v<reg_data_type> || detail::is_b32_type_v<reg_data_type> ||
            detail::is_b64_type_v<reg_data_type>,
        "unalign_store supports b4, b8, b16, b32, and b64 element types only");

    asc_storeunalign(dst_tile.engine().begin().get(), ureg, src.reg, count);
}

template <typename Tensor, typename Coord, typename RegDataType>
__simd_callee__ inline void unalign_store::store(
    Tensor dst_tile, const Coord& coord, const reg_tensor<RegDataType>& src, uint32_t count)
{
    using engine_type = typename Tensor::engine_type;
    using data_type = typename Tensor::data_type;
    using reg_data_type = detail::get_reg_data_type<data_type>;
    static_assert(
        Std::is_same_v<get_mem_location<engine_type>, location::ub>,
        "unalign_store only supports tensors located in UB");
    static_assert(
        Std::is_same_v<reg_data_type, RegDataType>,
        "unalign_store requires matching tensor and register element types");
    static_assert(
        detail::is_b4_type_v<reg_data_type> || detail::is_b8_type_v<reg_data_type> ||
            detail::is_b16_type_v<reg_data_type> || detail::is_b32_type_v<reg_data_type> ||
            detail::is_b64_type_v<reg_data_type>,
        "unalign_store supports b4, b8, b16, b32, and b64 element types only");
    TENSOR_API_DEBUG_CHECK(debug_check_coord, dst_tile.layout(), coord, "unalign_store::store");

    asc_storeunalign((dst_tile.data() + dst_tile.layout()(coord)).get(), ureg, src.reg, count);
}

template <typename Tensor>
__simd_callee__ inline void unalign_store::flush(Tensor dst_base, int32_t coord)
{
    using engine_type = typename Tensor::engine_type;
    using data_type = typename Tensor::data_type;
    using reg_data_type = detail::get_reg_data_type<data_type>;
    static_assert(
        Std::is_same_v<get_mem_location<engine_type>, location::ub>,
        "unalign_store only supports tensors located in UB");
    static_assert(
        detail::is_b4_type_v<reg_data_type> || detail::is_b8_type_v<reg_data_type> ||
            detail::is_b16_type_v<reg_data_type> || detail::is_b32_type_v<reg_data_type> ||
            detail::is_b64_type_v<reg_data_type>,
        "unalign_store supports b4, b8, b16, b32, and b64 element types only");

    asc_storeunalign_post(dst_base.engine().begin().get(), ureg, coord);
}

} // namespace experimental
} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_DATA_STORE_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_DATA_STORE_IMPL_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_DATA_STORE_IMPL_H
#endif
