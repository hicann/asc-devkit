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
    "impl/tensor_api/algorithm/mmad_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file mmad_impl.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ALGORITHM_MMAD_IMPL_H
#define IMPL_TENSOR_API_ALGORITHM_MMAD_IMPL_H

#include "impl/tensor_api/atom/mmad_atom_impl.h"

namespace asc {
namespace te {

template <typename AtomType, typename DstTensor, typename FmTensor, typename FilterTensor>
__aicore__ inline void mmad(const mmad_atom<AtomType>& atom, const DstTensor& dst, const FmTensor& fm,
                            const FilterTensor& filter)
{
    atom.call(dst, fm, filter);
}

template <typename AtomType, typename DstTensor, typename FmTensor, typename FilterTensor, typename BiasTensor,
          Std::enable_if_t<is_attr_tensor_v<BiasTensor>, int> = 0>
__aicore__ inline void mmad(const mmad_atom<AtomType>& atom, const DstTensor& dst, const FmTensor& fm,
                            const FilterTensor& filter, const BiasTensor& bias)
{
    atom.call(dst, fm, filter, bias);
}

template <typename MmadOperationType>
__aicore__ inline constexpr auto make_mmad(const MmadOperationType& mmad_operation)
{
    return mmad_atom<mmad_traits<MmadOperationType>>{};
}

template <typename MmadOperationType, typename MmadTraitType>
__aicore__ inline constexpr auto make_mmad(const MmadOperationType& mmad_operation, const MmadTraitType& trait_config)
{
    return mmad_atom<mmad_traits<MmadOperationType, MmadTraitType>>{};
}

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_ALGORITHM_MMAD_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
