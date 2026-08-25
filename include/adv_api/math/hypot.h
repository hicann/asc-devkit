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
 * \file hypot.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "hypot.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HYPOT_H__
#endif

#ifndef LIB_MATH_HYPOT_H
#define LIB_MATH_HYPOT_H

#include "kernel_tensor.h"

#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/math/hypot/hypot_common_impl.h"
#endif

namespace AscendC {
#pragma begin_pipe(V)
/*!
 * \ingroup Hypot
 * \brief compute Hypot elementwisely
 * \tparam T: half/float/bf16
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 * this parameter is reserved, please use the default value.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] src0Tensor: input LocalTensor
 * \param [in] src1Tensor: input LocalTensor
 * \param [in] sharedTmpBuffer: extra temporary shared space used for intermediate values among calculation process,
 * whose required space size should refer to corresponding tiling API, which is defined at atan_tiling.h.
 * Generally, the more space you allocate, the better performance you will achieve, and the performance
 * reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it is not guaranteed
 * that the shared space will be cleared after usage, the data could be anything.
 * \note src/dst Tensor must be 32B align, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Hypot(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer)
{
#if defined(ASCENDC_CPU_DEBUG) && (ASCENDC_CPU_DEBUG == 1)
    bool result = (src0Tensor.GetSize() == src1Tensor.GetSize());
    ASCENDC_ASSERT(result, { KERNEL_LOG(KERNEL_ERROR, "operands must be equal in size"); });
#endif
    Hypot<T, isReuseSource>(dstTensor, src0Tensor, src1Tensor, sharedTmpBuffer, src0Tensor.GetSize());
}

/*!
 * \ingroup Hypot
 * \brief compute Hypot elementwisely
 * \tparam T: half/float/bf16
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 * this parameter is reserved, please use the default value.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] src0Tensor: input LocalTensor
 * \param [in] src1Tensor: input LocalTensor
 * \param [in] sharedTmpBuffer: extra temporary shared space used for intermediate values among calculation process,
 * whose required space size should refer to corresponding tiling API, which is defined at atan_tiling.h.
 * Generally, the more space you allocate, the better performance you will achieve, and the performance
 * reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it is not guaranteed
 * that the shared space will be cleared after usage, the data could be anything.
 * \param [in] calCount: the number of elements to be processed.
 * \note src/dst Tensor must be 32B align, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Hypot(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
{
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    HypotImpl(dstTensor, src0Tensor, src1Tensor, sharedTmpBuffer, calCount);
#endif
}

/*!
 * \ingroup Hypot
 * \brief compute Hypot elementwisely
 * \tparam T: half/float/bf16
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 * this parameter is reserved, please use the default value.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] src0Tensor: input LocalTensor
 * \param [in] src1Tensor: input LocalTensor
 * \note src/dst Tensor must be 32B align, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Hypot(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor)
{
#if defined(ASCENDC_CPU_DEBUG) && (ASCENDC_CPU_DEBUG == 1)
    bool result = (src0Tensor.GetSize() == src1Tensor.GetSize());
    ASCENDC_ASSERT(result, { KERNEL_LOG(KERNEL_ERROR, "operands must be equal in size"); });
#endif
    Hypot<T, isReuseSource>(dstTensor, src0Tensor, src1Tensor, src0Tensor.GetSize());
}

/*!
 * \ingroup Hypot
 * \brief compute Hypot elementwisely
 * \tparam T: half/float/bf16
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 * this parameter is reserved, please use the default value.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] src0Tensor: input LocalTensor
 * \param [in] src1Tensor: input LocalTensor
 * \param [in] calCount: the number of elements to be processed.
 * \note src/dst Tensor must be 32B align, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Hypot(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor,
    const uint32_t calCount)
{
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    HypotImpl(dstTensor, src0Tensor, src1Tensor, calCount);
#endif
}
#pragma end_pipe
} // namespace AscendC
#endif // LIB_MATH_HYPOT_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HYPOT_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HYPOT_H__
#endif
