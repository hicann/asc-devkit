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
 * \file digamma.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "digamma.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DIGAMMA_H__
#endif

#ifndef LIB_MATH_DIGAMMA_H
#define LIB_MATH_DIGAMMA_H

#include "kernel_tensor.h"
#include "../../../impl/basic_api/kernel_pop_stack_buffer.h"
#include "kernel_tiling/kernel_tiling.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || \
                              __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/math/digamma/digamma_common_impl.h"
#endif

namespace AscendC {
#pragma begin_pipe(V)
/*
 * @ingroup Digamma
 * @brief Computes the logarithmic derivative of the gamma function on input. f(x) = digamma(x)
 * @tparam T: Input and output data types, half or float.
 * @tparam isReuseSrc: Whether temporary variables can reuse the input memory.
 * @param [out] dstTensor: output LocalTensor
 * @param [in] srcTensor: input LocalTensor
 * @param [in] sharedTmpBuffer: input local temporary Tensor
 * @param [in] calCount: amount of data to be calculated
 */
template <typename T, bool isReuseSource = false>
__ASC_USE_RESERVED_UBUF__(2201, "Digamma is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Digamma(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || \
                              __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    DigammaCompute<T, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, calCount);
#endif
}

/*
 * @ingroup Digamma
 * @brief Computes the logarithmic derivative of the gamma function on input. f(x) = digamma(x)
 * @tparam T: Input and output data types, half or float.
 * @tparam isReuseSrc: Whether temporary variables can reuse the input memory.
 * @param [out] dstTensor: output LocalTensor
 * @param [in] srcTensor: input LocalTensor
 * @param [in] calCount: amount of data to be calculated
 */
template <typename T, bool isReuseSource = false>
__ASC_USE_RESERVED_UBUF__(2201, "Digamma is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Digamma(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }

    LocalTensor<uint8_t> tmp;
    const bool ret = PopStackBuffer<uint8_t, TPosition::LCM>(tmp);
    ASCENDC_ASSERT((ret), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || \
                              __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    DigammaCompute<T, isReuseSource>(dstTensor, srcTensor, tmp, calCount);
#endif
}
#pragma end_pipe
} // namespace AscendC

#endif // LIB_MATH_DIGAMMA_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DIGAMMA_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DIGAMMA_H__
#endif
