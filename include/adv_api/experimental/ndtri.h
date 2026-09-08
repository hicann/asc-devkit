/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file ndtri.h
 * \brief Ndtri high-level API declaration.
 */

#ifndef CANN_ASC_USE_EXPERIMENTAL
#error "This is an experimental feature, please define CANN_ASC_USE_EXPERIMENTAL to use it."
#endif

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "ndtri.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_EXPERIMENTAL_NDTRI_H__
#endif

#ifndef ASCENDC_ADV_API_EXPERIMENTAL_NDTRI_H
#define ASCENDC_ADV_API_EXPERIMENTAL_NDTRI_H

#include "basic_api/kernel_tensor.h"

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
#include "impl/adv_api/detail/experimental/math/ndtri/ndtri_3510_impl.h"
#endif

namespace AscendC {
namespace experimental {
#pragma begin_pipe(V)

/*!
 * \ingroup Ndtri
 * \brief Computes the inverse standard normal cumulative distribution function for calCount elements.
 * The calculation satisfies \f$\operatorname{Ndtri}(p) = \Phi^{-1}(p)\f$, where
 * \f$\Phi(x) = \frac{1}{2}(1 + \operatorname{erf}(x / \sqrt{2}))\f$.
 * \tparam T: input and output data type, only float is supported.
 * \tparam isReuseSource: reserved parameter, use the default value false.
 * \param [out] dstTensor, output LocalTensor.
 * \param [in] srcTensor, read-only input LocalTensor containing probabilities.
 * \param [in] calCount, number of elements to calculate.
 * \note The start addresses of srcTensor and dstTensor must be 32-byte aligned. In-place calculation with identical
 * start addresses is supported, while partial address overlap is not supported. When the addresses are different,
 * srcTensor is not modified. calCount must not exceed the element count of either Tensor. When calCount is 0, the
 * function returns without reading srcTensor or writing dstTensor.
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Ndtri(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ != 3510)
    static_assert(sizeof(T) == 0, "Ndtri is only supported on NPU architecture 3510.");
#endif
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
    NdtriImpl<T, isReuseSource>(dstTensor, srcTensor, calCount);
#endif
}

/*!
 * \ingroup Ndtri
 * \brief Computes the inverse standard normal cumulative distribution function for all elements in srcTensor.
 * The calculation satisfies \f$\operatorname{Ndtri}(p) = \Phi^{-1}(p)\f$, where
 * \f$\Phi(x) = \frac{1}{2}(1 + \operatorname{erf}(x / \sqrt{2}))\f$.
 * \tparam T: input and output data type, only float is supported.
 * \tparam isReuseSource: reserved parameter, use the default value false.
 * \param [out] dstTensor, output LocalTensor.
 * \param [in] srcTensor, read-only input LocalTensor containing probabilities.
 * \note The start addresses of srcTensor and dstTensor must be 32-byte aligned. In-place calculation with identical
 * start addresses is supported, while partial address overlap is not supported. When the addresses are different,
 * srcTensor is not modified. dstTensor must contain at least srcTensor.GetSize() elements.
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Ndtri(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ != 3510)
    static_assert(sizeof(T) == 0, "Ndtri is only supported on NPU architecture 3510.");
#endif
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
    NdtriImpl<T, isReuseSource>(dstTensor, srcTensor, srcTensor.GetSize());
#endif
}

#pragma end_pipe
} // namespace experimental
} // namespace AscendC

#endif // ASCENDC_ADV_API_EXPERIMENTAL_NDTRI_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_EXPERIMENTAL_NDTRI_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_EXPERIMENTAL_NDTRI_H__
#endif
