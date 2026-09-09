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
 * \file cos.h
 * \brief Defines a series of interface used to do elementwise math cos calculation.
 * Formula: cos(x) = (-1)^k*sin(x0 + π/2), sin(x) = xP(x)
 * The Cos function does not have an elementary function expression, first normalize (x0 + π/2) to [-π/2, π/2]
 * and then calculating by function approximation.
 * k=round(x/π + 1/2), x0=x-kπ, x0 belongs to [-π, 0], (x0 + π/2) belongs to [-π/2, π/2]
 * π=π_0+π_1+π_2+π_3+π_4 achieve final precision compensation.
 * Final solution�?
 *   k = round(x * invpi + 1/2)
 *   x -= k * pi_0
 *   x -= k * pi_1
 *   x = x + COS_PI_DOWN
 *   x -= k * pi_2
 *   x -= k * pi_3
 *   x -= k * pi_4
 *   x = x + COS_PI_RESDOWN_ADDS_NEG
 *   P(x) = (((x^2 * R0 + R1) * x^2 + R2) * x^2 + R3) * x^2 + 1.0
 *   COS_PI_DOWN = 1.57079637050628662109375
 *   COS_PI_RESDOWN_ADDS_NEG = -0.00000004371139000189375
 *   R0 = 2.604926501e-6
 *   R1 = -0.0001980894471
 *   R2 = 0.008333049340
 *   R3 = -0.1666665792
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "cos.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_COS_H__
#endif

#ifndef LIB_MATH_COS_H
#define LIB_MATH_COS_H

#include "kernel_tensor.h"
#include "include/adv_api/math/cos_utils.h"

#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
#include "../../../impl/adv_api/detail/math/cos/cos_common_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || \
                                __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/math/cos/cos_3510_impl.h"
#endif
#endif

namespace AscendC {
#pragma begin_pipe(V)

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
/*!
 * \ingroup Cos
 * \brief compute Cos elementwisely
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason, only support
 * float input data type
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] sharedTmpBuffer: extra temporary shared space used for intermediate values among calculation process,
 * whose required space size should refer to corresponding tiling API, which is defined at cos_tiling.h.
 * Generally, the more space you allocate, the better performance you will achieve, and the performance
 * reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it is not guaranteed
 * that the shared space will be cleared after usage, the data could be anything.
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 * Input data valid range should be [-65504, 65504]
 */
template <typename T, bool isReuseSource = false, const CosConfig& config = defaultCosConfig>
__aicore__ inline void Cos(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
{
    Cos<T, isReuseSource, config>(dstTensor, srcTensor, sharedTmpBuffer, srcTensor.GetSize());
}

/*!
 * \ingroup Cos
 * \brief compute Cos elementwisely
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] sharedTmpBuffer: extra temporary shared space used for intermediate values among calculation process,
 * whose required space size should refer to corresponding tiling API, which is defined at cos_tiling.h.
 * Generally, the more space you allocate, the better performance you will achieve, and the performance
 * reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it is not guaranteed
 * that the shared space will be cleared after usage, the data could be anything.
 * \param [in] calCount: the number of elements to be processed.
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 * Input data valid range should be [-65504, 65504]
 */
template <typename T, bool isReuseSource = false, const CosConfig& config = defaultCosConfig>
__aicore__ inline void Cos(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t calCount)
{
    CosImpl<T, isReuseSource, config>(dstTensor, srcTensor, sharedTmpBuffer, calCount);
}

/*!
 * \ingroup Cos
 * \brief compute Cos elementwisely
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 * Input data valid range should be [-65504, 65504]
 */
template <typename T, bool isReuseSource = false, const CosConfig& config = defaultCosConfig>
__aicore__ inline void Cos(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
{
    Cos<T, isReuseSource, config>(dstTensor, srcTensor, srcTensor.GetSize());
}

/*!
 * \ingroup Cos
 * \brief compute Cos elementwisely
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] calCount: the number of elements to be processed.
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 * Input data valid range should be [-65504, 65504]
 */
template <typename T, bool isReuseSource = false, const CosConfig& config = defaultCosConfig>
__aicore__ inline void Cos(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
    CosImpl<T, isReuseSource, config>(dstTensor, srcTensor, calCount);
}
#else
/*!
 * \ingroup Cos
 * \brief compute Cos elementwisely
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason, only support
 * float input data type
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] sharedTmpBuffer: extra temporary shared space used for intermediate values among calculation process,
 * whose required space size should refer to corresponding tiling API, which is defined at cos_tiling.h.
 * Generally, the more space you allocate, the better performance you will achieve, and the performance
 * reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it is not guaranteed
 * that the shared space will be cleared after usage, the data could be anything.
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 * Input data valid range should be [-65504, 65504]
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Cos(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
{
    Cos<T, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, srcTensor.GetSize());
}

/*!
 * \ingroup Cos
 * \brief compute Cos elementwisely
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] sharedTmpBuffer: extra temporary shared space used for intermediate values among calculation process,
 * whose required space size should refer to corresponding tiling API, which is defined at cos_tiling.h.
 * Generally, the more space you allocate, the better performance you will achieve, and the performance
 * reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it is not guaranteed
 * that the shared space will be cleared after usage, the data could be anything.
 * \param [in] calCount: the number of elements to be processed.
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 * Input data valid range should be [-65504, 65504]
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Cos(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t calCount)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
    CosImpl<T, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, calCount);
#endif
}

/*!
 * \ingroup Cos
 * \brief compute Cos elementwisely
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 * Input data valid range should be [-65504, 65504]
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Cos(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
{
    Cos<T, isReuseSource>(dstTensor, srcTensor, srcTensor.GetSize());
}

/*!
 * \ingroup Cos
 * \brief compute Cos elementwisely
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] calCount: the number of elements to be processed.
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 * Input data valid range should be [-65504, 65504]
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Cos(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
    CosImpl<T, isReuseSource>(dstTensor, srcTensor, calCount);
#endif
}
#endif
#pragma end_pipe
} // namespace AscendC

#endif // LIB_MATH_COS_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_COS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_COS_H__
#endif
