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
 * \file axpy.h
 * \brief Compute axpy   dst = src * scalar + dst
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "axpy.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_AXPY_H__
#endif

#ifndef LIB_MATH_AXPY_H
#define LIB_MATH_AXPY_H
#include "kernel_tensor.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/math/axpy/axpy_3510_impl.h"
#else
#include "../../../impl/adv_api/detail/math/axpy/axpy_common_impl.h"
#endif

namespace AscendC {
#pragma begin_pipe(V)

/*!
 * \ingroup Axpy
 * \brief compute axpy   dst = src * scalar + dst
 * \tparam isReuseSource: Whether to reuse the buffer of srcTensor.
 *         If the value is true, srcTensor can be used as tmpBuffer and the data in it will be overwritten.
 *         If the value is false, srcTensor will not be used as tmpBuffer for calculation.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] scalarValue: input scalarValue
 * \param [in] sharedTmpBuffer: extra temporary shared space used for intermediate values among calculation process,
 *             whose required space size should refer to corresponding tiling API, which is defined at axpy_tiling.h.
 *             Generally, the more space you allocate, the better performance you will achieve, and the performance
 *             reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it is not guaranteed
 *             that the shared space will be cleared after usage, the data could be anything.
 * \param [in] calCount: input total calCount
 */
template <typename T, typename U, bool isReuseSource = false>
__aicore__ inline void Axpy(
    const LocalTensor<T>& dstTensor, const LocalTensor<U>& srcTensor, const U scalarValue,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    AxpyImpl<T, U, isReuseSource>(dstTensor, srcTensor, scalarValue, sharedTmpBuffer, calCount);
}

#pragma end_pipe
} // namespace AscendC
#endif // LIB_MATH_AXPY_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_AXPY_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_AXPY_H__
#endif
