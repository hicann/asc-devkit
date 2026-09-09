/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file sigmoid.h
 * \brief Sigmoid is an activation function,
 * Mathematical formulas: Sigmoid(x) = 1 / (1 + exp(-x))
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "sigmoid.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SIGMOID_H__
#endif

#ifndef LIB_SIGMOID_SIGMOID_H
#define LIB_SIGMOID_SIGMOID_H

#include "kernel_tensor.h"
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 1001 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || \
     __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/activation/sigmoid/sigmoid_common_impl.h"
#endif
namespace AscendC {
#pragma begin_pipe(V)
/*!
 * \ingroup Sigmoid
 * \brief compute Sigmoid elementwisely
 * \tparam T: input dataType, support half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 *         not enabled currently.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] sharedTmpBuffer：extra temporary shared space used for intermediate values among calculation process,
 *             whose required space size should refer to corresponding tiling API, which is defined at
 *             sigmoid_tiling.h. Generally, the more space you allocate, the better performance you will achieve,
 *             and the performance reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it
 *             is not guaranteed that the shared space will be cleared after usage, the data could be anything.
 * \param [in] calCount: amount of input data to be calculated
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Sigmoid(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t calCount)
{
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 1001 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || \
     __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    SigmoidImpl<T, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, calCount);
#endif
}

/*!
 * \ingroup Sigmoid
 * \brief compute Sigmoid elementwisely
 * \tparam T: input dataType, support half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 *         not enabled currently.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] calCount: amount of input data to be calculated
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Sigmoid(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 1001 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || \
     __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    SigmoidImpl<T, isReuseSource>(dstTensor, srcTensor, calCount);
#endif
}

/*!
 * \ingroup Sigmoid
 * \brief compute Sigmoid elementwisely
 * \tparam T: input dataType, support half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 *         not enabled currently.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] sharedTmpBuffer：extra temporary shared space used for intermediate values among calculation process,
 *             whose required space size should refer to corresponding tiling API, which is defined at
 *             sigmoid_tiling.h. Generally, the more space you allocate, the better performance you will achieve,
 *             and the performance reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it
 *             is not guaranteed that the shared space will be cleared after usage, the data could be anything.
 * \param [in] calCount: amount of input data to be calculated
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Sigmoid(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
{
    Sigmoid<T, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, srcTensor.GetSize());
}

/*!
 * \ingroup Sigmoid
 * \brief compute Sigmoid elementwisely
 * \tparam T: input dataType, support half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 *         not enabled currently.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Sigmoid(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
{
    Sigmoid<T, isReuseSource>(dstTensor, srcTensor, srcTensor.GetSize());
}

#pragma end_pipe
} // namespace AscendC
#endif // LIB_SIGMOID_SIGMOID_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SIGMOID_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SIGMOID_H__
#endif
