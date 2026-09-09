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
 * \file erfc.h
 * \brief Defines a series of interface used to do elementwise math Erfc calculation.
 * Formula: Supplement of Gauss error function. Erfc(x) = 1 - Erf(x)
 * The Erfc function does not have an elementary function expression, and there is calculating by
 * function approximation.
 * The approximate calculation formula is as follows:
 * Erfc(x) = (-xa^2)*(R(z)/S(z))*(x/xa)+�?-x/xa)
 * xa = |x| + min_float
 * z = min(xa, 10)
 * min_float is the smallest value could be represented by float.
 * R(z) = (((((((z * R0 + R1) * z + R2) * z + R3) * z + R4) * z + R5) * z + R6) * z + R7) * z + R8
 * S(z) = (((((z + S1) * z + S2) * z + S3) * z + S4) * z + S5
 * R0 = 0.1735313680e-7
 * R1 = -0.9856738394e-6
 * R2 = 0.2517003236e-4
 * R3 = -0.3848015171e-3
 * R4 = 0.5681528564e0
 * R5 = 0.5245623129e1
 * R6 = 0.2107740710e2
 * R7 = 0.4212761755e2
 * R8 = 0.4380524149e2
 * S1 = 0.9349684299e1
 * S2 = 0.3756930664e2
 * S3 = 0.8058268949e2
 * S4 = 0.9155653738e2
 * S5 = 0.4380524152e2
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "erfc.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ERFC_H__
#endif

#ifndef LIB_MATH_ERFC_H
#define LIB_MATH_ERFC_H

#include "kernel_tensor.h"

#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/math/erfc/erfc_3510_impl.h"
#else
#include "../../../impl/adv_api/detail/math/erfc/erfc_common_impl.h"
#endif
#endif

namespace AscendC {
#pragma begin_pipe(V)
/*!
 * \ingroup Erfc
 * \brief compute Erfc elementwisely
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] sharedTmpBuffer: extra temporary shared space used for intermediate values among calculation process,
 *             whose required space size should refer to corresponding tiling API, which is defined at erfc_tiling.h.
 *             Generally, the more space you allocate, the better performance you will achieve, and the performance
 *             reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it is not guaranteed
 *             that the shared space will be cleared after usage, the data could be anything.
 * \param [in] calCount: the number of elements to be processed.
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Erfc(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t calCount)
{
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    ErfcImpl<T, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, calCount);
#endif
}

/*!
 * \ingroup Erfc
 * \brief compute Erfc elementwisely for whole source tensor
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Erfc(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
{
    Erfc<T, isReuseSource>(dstTensor, srcTensor, srcTensor.GetSize());
}

/*!
 * \ingroup Erfc
 * \brief compute Erfc elementwisely
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] calCount: the number of elements to be processed.
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Erfc(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    ErfcImpl<T, isReuseSource>(dstTensor, srcTensor, calCount);
#endif
}

/*!
 * \ingroup Erfc
 * \brief compute Erfc elementwisely for whole source tensor
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] sharedTmpBuffer: extra temporary shared space used for intermediate values among calculation process,
 *             whose required space size should refer to corresponding tiling API, which is defined at erfc_tiling.h.
 *             Generally, the more space you allocate, the better performance you will achieve, and the performance
 *             reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it is not guaranteed
 *             that the shared space will be cleared after usage, the data could be anything.
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Erfc(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
{
    Erfc<T, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, srcTensor.GetSize());
}

#pragma end_pipe
} // namespace AscendC
#endif // LIB_MATH_ERFC_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ERFC_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ERFC_H__
#endif
