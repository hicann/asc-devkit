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
#error "copy.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_H
#endif

/*!
 * \file copy.h
 * \brief
 */
#ifndef INCLUDE_TENSOR_API_ALGORITHM_COPY_H
#define INCLUDE_TENSOR_API_ALGORITHM_COPY_H

#include "impl/tensor_api/algorithm/copy_impl.h"

namespace AscendC {
namespace Te {

/**
 * @brief Perform a copy operation with a preconstructed CopyAtom.
 * @param atomCopy : Copy atom object that determines the copy behavior.
 * @param dst : Destination tensor.
 * @param src : Source tensor.
 */
template <typename AtomType, typename DstTensor, typename SrcTensor>
__aicore__ inline void Copy(const CopyAtom<AtomType>& atomCopy, const DstTensor& dst, const SrcTensor& src);

/**
 * @brief Perform a copy operation with a preconstructed CopyAtom and a quantization parameter.
 * @param atomCopy : Copy atom object that determines the copy behavior.
 * @param dst : Destination tensor.
 * @param src : Source tensor.
 * @param quant : Quantization parameter, which can be a scalar or a Tensor API tensor.
 */
template <
    typename AtomType, typename DstTensor, typename SrcTensor, typename QuantParam,
    Std::enable_if_t<IsCopyQuantParamV<QuantParam>, int> Enable>
__aicore__ inline void Copy(
    const CopyAtom<AtomType>& atomCopy, const DstTensor& dst, const SrcTensor& src, const QuantParam& quant);

/**
 * @brief Construct a CopyAtom from the copy operation object.
 * @param copyOperation : Copy operation object.
 */
template <typename CopyOperationType>
__aicore__ inline constexpr auto MakeCopy(const CopyOperationType& copyOperation);

/**
 * @brief Construct a CopyAtom from the copy operation object and trait object.
 * @param copyOperation : Copy operation object.
 * @param copyTrait : Copy trait object.
 */
template <typename CopyOperationType, typename CopyTraitType>
__aicore__ inline constexpr auto MakeCopy(const CopyOperationType& copyOperation, const CopyTraitType& copyTrait);

} // namespace Te
} // namespace AscendC

#endif // INCLUDE_TENSOR_API_ALGORITHM_COPY_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_H
#endif
