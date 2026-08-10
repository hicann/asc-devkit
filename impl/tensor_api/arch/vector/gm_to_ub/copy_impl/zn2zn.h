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
    "impl/tensor_api/arch/vector/gm_to_ub/copy_impl/zn2zn.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file zn2zn.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_GM_TO_UB_COPY_IMPL_ZN2ZN_H
#define IMPL_TENSOR_API_ARCH_VECTOR_GM_TO_UB_COPY_IMPL_ZN2ZN_H

#include "impl/tensor_api/arch/vector/gm_to_ub/copy_impl/data_copy.h"

namespace asc {
namespace te {

class copy_gm_to_ub_zn : private copy_gm_to_ub_common {
public:
    template <const copy_gm_to_ub_trait& trait, typename T, typename U>
    __aicore__ inline static void run(const T& dst, const U& src, const copy_gm_to_ub_params& params)
    {
        data_copy_impl<trait, T, U>(dst, src, params);
    }

private:
    template <const copy_gm_to_ub_trait& trait, typename T, typename U>
    __aicore__ inline static constexpr void check_template()
    {
        check_layout_pattern<U, T>();
        check_data_type::check_gm_to_ub_data_type<T, U>();
    }

    template <const copy_gm_to_ub_trait& trait, typename T, typename U>
    __aicore__ inline static void data_copy_impl(const T& dst, const U& src, const copy_gm_to_ub_params& params)
    {
        using src_type = typename U::element_type;
        using dst_type = typename T::element_type;

        check_template<trait, T, U>();

        auto dst_layout = dst.layout();
        auto src_layout = src.layout();

        uint16_t block_count = get_element<attr_info::shape, attr_info::row, 1>(src_layout);
        uint32_t block_len = get_total_column_shape(src_layout)
                             * get_element<attr_info::shape, attr_info::row, 0>(src_layout) * sizeof(src_type);
        int64_t src_stride = get_element<attr_info::stride, attr_info::row, 1>(src_layout) * sizeof(src_type);
        int64_t dst_stride = get_element<attr_info::stride, attr_info::row, 1>(dst_layout) * sizeof(dst_type);

        emit_copy(dst, src, block_count, block_len, src_stride, dst_stride, params);
    }
};

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_ARCH_VECTOR_GM_TO_UB_COPY_IMPL_ZN2ZN_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
