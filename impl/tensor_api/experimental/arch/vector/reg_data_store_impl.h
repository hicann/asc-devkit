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
    "reg_data_store_impl.h is internal; include <tensor_api/experimental/arch/vector/reg_data_store.h> instead."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_DATA_STORE_IMPL
#endif

#ifndef IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_DATA_STORE_IMPL_H
#define IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_DATA_STORE_IMPL_H

#if !defined(INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_DATA_STORE_H)
#include "tensor_api/experimental/arch/vector/reg_data_store.h"
#endif
#include "tensor_api/tensor/tensor.h"

namespace asc {
namespace te {
namespace experimental {
namespace detail {

template <store_sideband_mode sideband_mode, typename Pointer, typename RegDataType>
__simd_callee__ inline void store_to_ptr(Pointer dst, const reg_tensor<RegDataType>& src)
{
    if constexpr (sideband_mode == store_sideband_mode::direct) {
        asc_storealign(dst, src.reg, src.mask);
    } else if constexpr (sideband_mode == store_sideband_mode::store_1st) {
        asc_storealign_1st(dst, src.reg);
    } else if constexpr (sideband_mode == store_sideband_mode::pack) {
        asc_storealign_pack(dst, src.reg, src.mask);
    } else if constexpr (sideband_mode == store_sideband_mode::pack_quarter) {
        asc_storealign_pack_quarter(dst, src.reg, src.mask);
    } else if constexpr (sideband_mode == store_sideband_mode::intlv) {
        static_assert(sideband_mode != store_sideband_mode::intlv,
            "intlv requires the two-source store overload");
    } else {
        static_assert(sideband_mode == store_sideband_mode::direct, "unsupported store sideband mode");
    }
}

template <store_sideband_mode sideband_mode, typename Pointer, typename RegDataType>
__simd_callee__ inline void store_to_ptr(
    Pointer dst, const reg_tensor<RegDataType>& src0, const reg_tensor<RegDataType>& src1)
{
    if constexpr (sideband_mode == store_sideband_mode::intlv) {
        asc_storealign_intlv(dst, src0.reg, src1.reg);
    } else {
        static_assert(sideband_mode == store_sideband_mode::intlv,
            "the two-source store overload only supports intlv");
    }
}

} // namespace detail

template <store_sideband_mode sideband_mode, typename Tensor, typename Coord, typename RegDataType>
__simd_callee__ inline void store(Tensor& tensor, const Coord& coord, const reg_tensor<RegDataType>& src)
{
    using engine_type = typename Tensor::engine_type;
    using data_type = typename Tensor::data_type;
    static_assert(Std::is_same_v<get_mem_location<engine_type>, location::ub>,
        "store only supports tensors located in UB");
    static_assert(Std::is_same_v<data_type, RegDataType>,
        "store requires matching tensor and register element types");
    TENSOR_API_DEBUG_CHECK(debug_check_coord, tensor.layout(), coord, "store");

    auto dst_engine = tensor.engine() + tensor.layout()(coord);
    detail::store_to_ptr<sideband_mode>(dst_engine.begin().get(), src);
}

template <store_sideband_mode sideband_mode, typename Tensor, typename RegDataType>
__simd_callee__ inline void store(Tensor& tensor, const reg_tensor<RegDataType>& src)
{
    using engine_type = typename Tensor::engine_type;
    using data_type = typename Tensor::data_type;
    static_assert(Std::is_same_v<get_mem_location<engine_type>, location::ub>,
        "store only supports tensors located in UB");
    static_assert(Std::is_same_v<data_type, RegDataType>,
        "store requires matching tensor and register element types");

    detail::store_to_ptr<sideband_mode>(tensor.engine().begin().get(), src);
}

template <store_sideband_mode sideband_mode, typename Tensor, typename Coord, typename RegDataType>
__simd_callee__ inline void store(Tensor& tensor, const Coord& coord, const reg_tensor<RegDataType>& src0,
    const reg_tensor<RegDataType>& src1)
{
    using engine_type = typename Tensor::engine_type;
    using data_type = typename Tensor::data_type;
    static_assert(Std::is_same_v<get_mem_location<engine_type>, location::ub>,
        "store only supports tensors located in UB");
    static_assert(Std::is_same_v<data_type, RegDataType>,
        "store requires matching tensor and register element types");
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
    static_assert(Std::is_same_v<get_mem_location<engine_type>, location::ub>,
        "store only supports tensors located in UB");
    static_assert(Std::is_same_v<data_type, RegDataType>,
        "store requires matching tensor and register element types");

    detail::store_to_ptr<sideband_mode>(tensor.engine().begin().get(), src0, src1);
}

} // namespace experimental
} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_DATA_STORE_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_DATA_STORE_IMPL)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_DATA_STORE_IMPL
#endif
