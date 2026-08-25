/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "mmad.h cannot be used with compile flag --enable-simt enabled."
#endif

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

namespace AscendC {
namespace Te {

/**
 * @brief Perform matrix multiplication with a preconstructed MmadAtom.
 * @param atomMmad : Matrix multiplication atom object.
 * @param dst : Destination tensor.
 * @param fm : Left input tensor.
 * @param filter : Right input tensor.
 */
template <typename AtomType, typename DstTensor, typename FmTensor, typename FilterTensor>
__aicore__ inline void Mmad(
    const MmadAtom<AtomType>& atomMmad, const DstTensor& dst, const FmTensor& fm, const FilterTensor& filter);

/**
 * @brief Perform matrix multiplication with a preconstructed MmadAtom and a bias tensor.
 * @param atomMmad : Matrix multiplication atom object.
 * @param dst : Destination tensor.
 * @param fm : Left input tensor.
 * @param filter : Right input tensor.
 * @param bias : Bias tensor.
 */
template <
    typename AtomType, typename DstTensor, typename FmTensor, typename FilterTensor, typename BiasTensor,
    Std::enable_if_t<IsAttrTensorV<BiasTensor>, int> Enable>
__aicore__ inline void Mmad(
    const MmadAtom<AtomType>& atomMmad, const DstTensor& dst, const FmTensor& fm, const FilterTensor& filter,
    const BiasTensor& bias);

/**
 * @brief Construct a MmadAtom from the matrix multiplication operation object.
 * @param mmadOperation : Matrix multiplication operation object.
 */
template <typename MmadOperationType>
__aicore__ inline constexpr auto MakeMmad(const MmadOperationType& mmadOperation);

/**
 * @brief Construct a MmadAtom from the matrix multiplication operation object and trait object.
 * @param mmadOperation : Matrix multiplication operation object.
 * @param mmadTrait : Matrix multiplication trait object.
 */
template <typename MmadOperationType, typename MmadTraitType>
__aicore__ inline constexpr auto MakeMmad(const MmadOperationType& mmadOperation, const MmadTraitType& mmadTrait);

} // namespace Te
} // namespace AscendC

#endif // INCLUDE_TENSOR_API_ALGORITHM_MMAD_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_H
#endif
