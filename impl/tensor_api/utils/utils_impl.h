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
    "impl/tensor_api/utils/utils_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file utils_impl.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_UTILS_UTILS_IMPL_H
#define IMPL_TENSOR_API_UTILS_UTILS_IMPL_H

#include "include/tensor_api/utils/utils.h"
#include "impl/tensor_api/legacy/legacy_utils.h"
#include "impl/tensor_api/utils/constant_impl.h"
#include "impl/tensor_api/utils/extra_impl.h"
#include "impl/tensor_api/utils/macro_impl.h"
#include "impl/tensor_api/utils/map_impl.h"
#include "impl/tensor_api/utils/location_impl.h"

namespace asc {
namespace te {

__aicore__ inline constexpr cache_mode normalize_cache_mode(cache_mode mode)
{
    return mode;
}

__aicore__ inline constexpr cache_mode normalize_cache_mode(CacheMode mode)
{
    switch (mode) {
    case CacheMode::CACHE_MODE_DISABLE:
        return cache_mode::disable;
    case CacheMode::CACHE_MODE_LAST:
        return cache_mode::last;
    case CacheMode::CACHE_MODE_PERSISTENT:
        return cache_mode::persistent;
    case CacheMode::CACHE_MODE_NORMAL:
    default:
        return cache_mode::normal;
    }
}

__aicore__ inline constexpr mmad_type normalize_mmad_type(mmad_type type)
{
    return type;
}

__aicore__ inline constexpr mmad_type normalize_mmad_type(MmadType type)
{
    switch (type) {
    case MmadType::MX:
        return mmad_type::mx;
    case MmadType::NORMAL:
    default:
        return mmad_type::normal;
    }
}

__aicore__ inline constexpr round_mode normalize_round_mode(round_mode mode)
{
    return mode;
}

__aicore__ inline constexpr round_mode normalize_round_mode(RoundMode mode)
{
    switch (mode) {
    case RoundMode::HYBRID:
        return round_mode::hybrid;
    case RoundMode::DEFAULT:
    default:
        return round_mode::default_round;
    }
}

__aicore__ inline constexpr dual_dst_mode normalize_dual_dst_mode(dual_dst_mode mode)
{
    return mode;
}

__aicore__ inline constexpr dual_dst_mode normalize_dual_dst_mode(DualDstMode mode)
{
    switch (mode) {
    case DUAL_DST_SPLIT_M:
        return dual_dst_mode::split_m;
    case DUAL_DST_SPLIT_N:
        return dual_dst_mode::split_n;
    case DUAL_DST_DISABLE:
    default:
        return dual_dst_mode::disable;
    }
}

template <typename Trait>
__aicore__ inline constexpr mmad_trait normalize_mmad_trait(const Trait& trait)
{
    using trait_type = Std::remove_cvref_t<Trait>;
    if constexpr (Std::is_same_v<trait_type, mmad_trait>) {
        return trait;
    } else {
        static_assert(Std::is_same_v<trait_type, MmadTrait>, "Trait must be mmad_trait or MmadTrait.");
        return mmad_trait{trait.fmOffset, trait.kDirectionAlign, trait.cmatrixSource, trait.disableGemv,
                          normalize_mmad_type(trait.mmadType)};
    }
}

template <typename Params>
__aicore__ inline constexpr mmad_params normalize_mmad_params(const Params& params)
{
    using params_type = Std::remove_cvref_t<Params>;
    if constexpr (Std::is_same_v<params_type, mmad_params>) {
        return params;
    } else {
        static_assert(Std::is_same_v<params_type, MmadParams>, "Params must be mmad_params or MmadParams.");
        return mmad_params{params.m, params.n, params.k, params.unitFlag, params.cmatrixInitVal};
    }
}

template <typename Params>
__aicore__ inline constexpr fixpipe_params normalize_fixpipe_params(const Params& params)
{
    using params_type = Std::remove_cvref_t<Params>;
    if constexpr (Std::is_same_v<params_type, fixpipe_params>) {
        return params;
    } else {
        static_assert(Std::is_same_v<params_type, FixpipeParams>, "Params must be fixpipe_params or FixpipeParams.");
        return fixpipe_params{params.unitFlag, params.subBlockId};
    }
}

template <typename Params>
__aicore__ inline constexpr copy_gm_to_ub_params normalize_copy_gm_to_ub_params(const Params& params)
{
    using params_type = Std::remove_cvref_t<Params>;
    if constexpr (Std::is_same_v<params_type, copy_gm_to_ub_params>) {
        return params;
    } else {
        static_assert(Std::is_same_v<params_type, CopyGM2UBParams>,
                      "Params must be copy_gm_to_ub_params or CopyGM2UBParams.");
        return copy_gm_to_ub_params{params.leftPaddingCount, params.rightPaddingCount, params.enableConstantPad};
    }
}

template <typename PadType>
__aicore__ inline constexpr auto normalize_img2col_params(const img2col_params<PadType>& params)
{
    return params;
}

template <typename PadType>
__aicore__ inline constexpr auto normalize_img2col_params(const Img2ColParams<PadType>& params)
{
    img2col_params<PadType> normalized_params;
    normalized_params.m_extension = params.mExtension;
    normalized_params.k_extension = params.kExtension;
    normalized_params.m_start_pt = params.mStartPt;
    normalized_params.k_start_pt = params.kStartPt;
    for (uint32_t i = 0; i < 4; ++i) {
        normalized_params.pad_list[i] = params.padList[i];
    }
    normalized_params.stride_w = params.strideW;
    normalized_params.stride_h = params.strideH;
    normalized_params.filter_w = params.filterW;
    normalized_params.filter_h = params.filterH;
    normalized_params.dilation_filter_w = params.dilationFilterW;
    normalized_params.dilation_filter_h = params.dilationFilterH;
    normalized_params.filter_size_w = params.filterSizeW;
    normalized_params.filter_size_h = params.filterSizeH;
    normalized_params.transpose = params.transpose;
    normalized_params.f_matrix_ctrl = params.fMatrixCtrl;
    normalized_params.pad_value = params.padValue;
    return normalized_params;
}

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_UTILS_UTILS_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
