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
    "impl/tensor_api/tensor/local_tensor_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use tensor_api/tensor/local_tensor.h instead."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define __UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_LOCAL_TENSOR_IMPL_H__
#endif

#ifndef IMPL_TENSOR_API_TENSOR_LOCAL_TENSOR_IMPL_H
#define IMPL_TENSOR_API_TENSOR_LOCAL_TENSOR_IMPL_H

#include "tensor_api/tensor/local_tensor.h"

namespace asc {
namespace te {
namespace detail {

template <load_sideband_mode sideband_mode, typename TensorType, typename Coord>
__simd_callee__ inline decltype(auto) load_local_tensor(const TensorType& tensor, const Coord& coord)
{
    using engine_type = typename TensorType::engine_type;
    using data_type = typename TensorType::data_type;
    static_assert(Std::is_same_v<get_mem_location<engine_type>, location::ub>,
                  "local_tensor::load only supports tensors located in UB");

    auto srcEngine = tensor.engine() + tensor.layout()(coord);
    auto src = srcEngine.begin().get();

    reg_tensor<data_type> dst;
    if constexpr (sideband_mode == load_sideband_mode::direct) {
        asc_loadalign(dst.reg, src);
    } else if constexpr (sideband_mode == load_sideband_mode::downsample) {
        asc_loadalign_downsample(dst.reg, src);
    } else if constexpr (sideband_mode == load_sideband_mode::unpack) {
        asc_loadalign_unpack(dst.reg, src);
    } else if constexpr (sideband_mode == load_sideband_mode::unpack4) {
        asc_loadalign_unpack4(dst.reg, src);
    } else if constexpr (sideband_mode == load_sideband_mode::upsample) {
        asc_loadalign_upsample(dst.reg, src);
    } else if constexpr (sideband_mode == load_sideband_mode::deintlv) {
        static_assert(sideband_mode != load_sideband_mode::deintlv,
                      "deintlv requires the local_tensor::load(coord, dst0, dst1) overload");
    } else {
        static_assert(sideband_mode == load_sideband_mode::direct, "unsupported load sideband mode");
    }
    return dst;
}

template <load_sideband_mode sideband_mode, typename TensorType, typename Coord, typename RegDataType>
__simd_callee__ inline void load_local_tensor(const TensorType& tensor, const Coord& coord,
                                               reg_tensor<RegDataType>& dst0, reg_tensor<RegDataType>& dst1)
{
    using engine_type = typename TensorType::engine_type;
    using data_type = typename TensorType::data_type;
    static_assert(Std::is_same_v<get_mem_location<engine_type>, location::ub>,
                  "local_tensor::load only supports tensors located in UB");
    static_assert(Std::is_same_v<data_type, RegDataType>,
                  "local_tensor::load requires matching tensor and register element types");

    if constexpr (sideband_mode == load_sideband_mode::deintlv) {
        auto srcEngine = tensor.engine() + tensor.layout()(coord);
        asc_loadalign_deintlv(dst0.reg, dst1.reg, srcEngine.begin().get());
    } else {
        static_assert(sideband_mode == load_sideband_mode::deintlv,
                      "the two-destination local_tensor::load overload only supports deintlv");
    }
}

template <broadcast_mode brc_mode, typename TensorType, typename Coord>
__simd_callee__ inline decltype(auto) load_broadcast_local_tensor(const TensorType& tensor, const Coord& coord)
{
    using engine_type = typename TensorType::engine_type;
    using data_type = typename TensorType::data_type;
    static_assert(Std::is_same_v<get_mem_location<engine_type>, location::ub>,
                  "local_tensor::load_broadcast only supports tensors located in UB");

    auto srcEngine = tensor.engine() + tensor.layout()(coord);
    auto src = srcEngine.begin().get();

    reg_tensor<data_type> dst;
    if constexpr (brc_mode == broadcast_mode::elem) {
        asc_loadalign_brc_elem(dst.reg, src);
    } else if constexpr (brc_mode == broadcast_mode::datablock) {
        asc_loadalign_brc_datablock(dst.reg, src);
    } else if constexpr (brc_mode == broadcast_mode::elem2datablock) {
        asc_loadalign_brc_elem2datablock(dst.reg, src);
    } else {
        static_assert(brc_mode == broadcast_mode::elem, "unsupported broadcast mode");
    }
    return dst;
}

template <store_sideband_mode sideband_mode, typename TensorType, typename Coord, typename RegDataType>
__simd_callee__ inline void store_local_tensor(TensorType& tensor, const Coord& coord,
                                                const reg_tensor<RegDataType>& src)
{
    using engine_type = typename TensorType::engine_type;
    using data_type = typename TensorType::data_type;
    static_assert(Std::is_same_v<get_mem_location<engine_type>, location::ub>,
                  "local_tensor::store only supports tensors located in UB");
    static_assert(Std::is_same_v<data_type, RegDataType>,
                  "local_tensor::store requires matching tensor and register element types");

    auto dstEngine = tensor.engine() + tensor.layout()(coord);
    auto dst = dstEngine.begin().get();
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
                      "intlv requires the local_tensor::store(coord, src0, src1) overload");
    } else {
        static_assert(sideband_mode == store_sideband_mode::direct, "unsupported store sideband mode");
    }
}

template <store_sideband_mode sideband_mode, typename TensorType, typename Coord, typename RegDataType>
__simd_callee__ inline void store_local_tensor(TensorType& tensor, const Coord& coord,
                                                const reg_tensor<RegDataType>& src0,
                                                const reg_tensor<RegDataType>& src1)
{
    using engine_type = typename TensorType::engine_type;
    using data_type = typename TensorType::data_type;
    static_assert(Std::is_same_v<get_mem_location<engine_type>, location::ub>,
                  "local_tensor::store only supports tensors located in UB");
    static_assert(Std::is_same_v<data_type, RegDataType>,
                  "local_tensor::store requires matching tensor and register element types");

    if constexpr (sideband_mode == store_sideband_mode::intlv) {
        auto dstEngine = tensor.engine() + tensor.layout()(coord);
        asc_storealign_intlv(dstEngine.begin().get(), src0.reg, src1.reg);
    } else {
        static_assert(sideband_mode == store_sideband_mode::intlv,
                      "the two-source local_tensor::store overload only supports intlv");
    }
}

} // namespace detail

template <typename EngineType, typename LayoutType>
template <load_sideband_mode sideband_mode, typename Coord>
__simd_callee__ inline decltype(auto) local_tensor<EngineType, LayoutType>::load(const Coord& coord) const
{
    TENSOR_API_DEBUG_CHECK(debug_check_coord, this->layout(), coord, "tensor::load");
    return detail::load_local_tensor<sideband_mode>(*this, coord);
}

template <typename EngineType, typename LayoutType>
template <load_sideband_mode sideband_mode, typename Coord, typename RegDataType>
__simd_callee__ inline void local_tensor<EngineType, LayoutType>::load(const Coord& coord,
                                                                       reg_tensor<RegDataType>& dst0,
                                                                       reg_tensor<RegDataType>& dst1) const
{
    TENSOR_API_DEBUG_CHECK(debug_check_coord, this->layout(), coord, "tensor::load");
    detail::load_local_tensor<sideband_mode>(*this, coord, dst0, dst1);
}

template <typename EngineType, typename LayoutType>
template <broadcast_mode brc_mode, typename Coord>
__simd_callee__ inline decltype(auto) local_tensor<EngineType, LayoutType>::load_broadcast(const Coord& coord) const
{
    TENSOR_API_DEBUG_CHECK(debug_check_coord, this->layout(), coord, "tensor::load_broadcast");
    return detail::load_broadcast_local_tensor<brc_mode>(*this, coord);
}

template <typename EngineType, typename LayoutType>
template <store_sideband_mode sideband_mode, typename Coord, typename RegDataType>
__simd_callee__ inline void local_tensor<EngineType, LayoutType>::store(const Coord& coord,
                                                                        const reg_tensor<RegDataType>& src)
{
    TENSOR_API_DEBUG_CHECK(debug_check_coord, this->layout(), coord, "tensor::store");
    detail::store_local_tensor<sideband_mode>(*this, coord, src);
}

template <typename EngineType, typename LayoutType>
template <store_sideband_mode sideband_mode, typename Coord, typename RegDataType>
__simd_callee__ inline void local_tensor<EngineType, LayoutType>::store(const Coord& coord,
                                                                        const reg_tensor<RegDataType>& src0,
                                                                        const reg_tensor<RegDataType>& src1)
{
    TENSOR_API_DEBUG_CHECK(debug_check_coord, this->layout(), coord, "tensor::store");
    detail::store_local_tensor<sideband_mode>(*this, coord, src0, src1);
}

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_TENSOR_LOCAL_TENSOR_IMPL_H

#if defined(__UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_LOCAL_TENSOR_IMPL_H__)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef __UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_LOCAL_TENSOR_IMPL_H__
#endif
