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
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_H
#endif

/*!
 * \file mmad.h
 * \brief
 */
#ifndef INCLUDE_TENSOR_API_ALGORITHM_MMAD_H
#define INCLUDE_TENSOR_API_ALGORITHM_MMAD_H

#include "impl/tensor_api/algorithm/mmad_impl.h"

namespace asc {
namespace te {

/**
 * @brief Perform matrix multiplication with a preconstructed mmad_atom.
 * @param atom : Matrix multiplication atom object.
 * @param dst : Destination tensor.
 * @param fm : Left input tensor.
 * @param filter : Right input tensor.
 */
template <typename AtomType, typename DstTensor, typename FmTensor, typename FilterTensor>
__aicore__ inline void mmad(const mmad_atom<AtomType>& atom, const DstTensor& dst, const FmTensor& fm,
                            const FilterTensor& filter);

/**
 * @brief Perform matrix multiplication with a preconstructed mmad_atom and a bias tensor.
 * @param atom : Matrix multiplication atom object.
 * @param dst : Destination tensor.
 * @param fm : Left input tensor.
 * @param filter : Right input tensor.
 * @param bias : Bias tensor.
 */
template <typename AtomType, typename DstTensor, typename FmTensor, typename FilterTensor, typename BiasTensor,
          Std::enable_if_t<is_attr_tensor_v<BiasTensor>, int> Enable>
__aicore__ inline void mmad(const mmad_atom<AtomType>& atom, const DstTensor& dst, const FmTensor& fm,
                            const FilterTensor& filter, const BiasTensor& bias);

/**
 * @brief Construct a mmad_atom from the matrix multiplication operation object.
 * @param mmad_operation : Matrix multiplication operation object.
 */
template <typename MmadOperationType>
__aicore__ inline constexpr auto make_mmad(const MmadOperationType& mmad_operation);

/**
 * @brief Construct a mmad_atom from the matrix multiplication operation object and trait object.
 * @param mmad_operation : Matrix multiplication operation object.
 * @param trait_config : Matrix multiplication trait object.
 */
template <typename MmadOperationType, typename MmadTraitType>
__aicore__ inline constexpr auto make_mmad(const MmadOperationType& mmad_operation, const MmadTraitType& trait_config);

} // namespace te
} // namespace asc

#endif // INCLUDE_TENSOR_API_ALGORITHM_MMAD_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_H
#endif
