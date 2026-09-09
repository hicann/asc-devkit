/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file tan.h
 * \brief Defines a series of interface used to do elementwise math tan calculation.
 * Formula: tan(x) = xP(x) / ((π/2 - x)(π/2 + x)Q(x))
 * The Tan function does not have an elementary function expression, first normalize x to (-π/2, π/2)
 * and then calculating by function approximation.
 * Final solution�?
 *  k=round(x/π), x0=x-kπ, x0 belongs to (-π/2, π/2)
 *  π=π_0+π_1+π_2+π_3+π_4 achieve final precision compensation.
 *  Final solution�?
 *  k = round(x * invpi)
 *  x -= k * pi_0
 *  x -= k * pi_1
 *  down1 = x + pio2_high // pi/2 + x
 *  down2 = x - pio2_high // x - pi/2
 *  x -= k * pi_2
 *  down1 -= k * pi_2
 *  down2 -= k * pi_2
 *  x -= k * pi_3
 *  down1 -= k * pi_3
 *  down2 -= k * pi_3
 *  x -= k * pi_4
 *  down1 -= k * pi_4
 *  down2 -= k * pi_4
 *  P(x) = (x^2 * R0 + R1) * x^2 + R2
 *  Q(x) = x^2 * R3
 *  R0 = 0.0698520831551998762793
 *  R1 = -6.8711573651634203789
 *  R2 = 61.20362572811089435388
 *  R3 = -24.8048928861126769186219
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "tan.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TAN_H__
#endif

#ifndef LIB_MATH_TAN_H
#define LIB_MATH_TAN_H

#include "kernel_tensor.h"

#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
#include "../../../impl/adv_api/detail/math/tan/tan_common_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || \
                                __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/math/tan/tan_3510_impl.h"
#endif
#endif

namespace AscendC {
#pragma begin_pipe(V)
/*!
 * \ingroup Tan
 * \brief compute Tan elementwisely
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 * this parameter is reserved, please use the default value.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] sharedTmpBuffer: extra temporary shared space used for intermediate values among calculation process,
 * whose required space size should refer to corresponding tiling API, which is defined at tan_tiling.h.
 * Generally, the more space you allocate, the better performance you will achieve, and the performance
 * reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it is not guaranteed
 * that the shared space will be cleared after usage, the data could be anything.
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 * Input data valid range should be (-65504, 65504)
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Tan(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
{
    Tan<T, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, srcTensor.GetSize());
}

/*!
 * \ingroup Tan
 * \brief compute Tan elementwisely
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 * this parameter is reserved, please use the default value.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] sharedTmpBuffer: extra temporary shared space used for intermediate values among calculation process,
 * whose required space size should refer to corresponding tiling API, which is defined at tan_tiling.h.
 * Generally, the more space you allocate, the better performance you will achieve, and the performance
 * reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it is not guaranteed
 * that the shared space will be cleared after usage, the data could be anything.
 * \param [in] calCount: the number of elements to be processed.
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 * Input data valid range should be (-65504, 65504)
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Tan(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t calCount)
{
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    TanImpl<T, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, calCount);
#endif
}

/*!
 * \ingroup Tan
 * \brief compute Tan elementwisely
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 * this parameter is reserved, please use the default value.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] calCount: the number of elements to be processed.
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 * Input data valid range should be (-65504, 65504)
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Tan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    TanImpl<T, isReuseSource>(dstTensor, srcTensor, calCount);
#endif
}

/*!
 * \ingroup Tan
 * \brief compute Tan elementwisely
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 * this parameter is reserved, please use the default value.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 * Input data valid range should be (-65504, 65504)
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Tan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
{
    Tan<T, isReuseSource>(dstTensor, srcTensor, srcTensor.GetSize());
}
#pragma end_pipe
} // namespace AscendC

#endif // LIB_MATH_TAN_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TAN_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TAN_H__
#endif
