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
 * \file acos.h
 * \brief Defines a series of interface used to do elementwise math arccos calculation.
 * Formula: Acos(x) = arccos(x), Acos(x) = PI*0.5 - Asin(x)
 * The Asin function does not have an elementary function expression, and there is calculating by
 * function approximation.
 * The approximate calculation formula is as follows:
 * when x belongs to (-2^(-0.5), 2^(-0.5)), Asin(x) = x +1/6*x^3 +3/40*x^5 +5!!/(6!!*7)*x^7  + �?+13!!/(14!!*15)*x^15
 * when x x belongs to (-1, -2^(-0.5)), Asin(x) = arcsin(sqrt(1-x^2)) - PI*0.5
 * when x belongs to (2^(-0.5), 1), Asin(x) = PI*0.5 - arcsin(sqrt(1-x^2))
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "acos.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ACOS_H__
#endif

#ifndef LIB_MATH_ACOS_H
#define LIB_MATH_ACOS_H
#include "kernel_tensor.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
#include "../../../impl/adv_api/detail/math/acos/acos_common_impl.h"
#elif defined(__NPU_ARCH__) &&                                                                       \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || \
     __NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
#include "../../../impl/adv_api/detail/math/acos/acos_3510_impl.h"
#endif

namespace AscendC {
#pragma begin_pipe(V)
/*!
 * \ingroup Acos
 * \brief compute ACos elementwisely
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] sharedTmpBuffer: extra temporary shared space used for intermediate values among calculation process,
 *             whose required space size should refer to corresponding tiling API, which is defined at acos_tiling.h.
 *             Generally, the more space you allocate, the better performance you will achieve, and the performance
 *             reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it is not guaranteed
 *             that the shared space will be cleared after usage, the data could be anything.
 * \param [in] calCount: the number of elements to be processed.
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 * Input data valid range should be [-1, 1]
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Acos(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t calCount)
{
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || \
     __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || \
     __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    AcosImpl<T, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, calCount);
#endif
}

/*!
 * \ingroup Acos
 * \brief compute ACos elementwisely
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] calCount: the number of elements to be processed.
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 * Input data valid range should be [-1, 1]
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Acos(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || \
     __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || \
     __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    AcosImpl<T, isReuseSource>(dstTensor, srcTensor, calCount);
#endif
}

/*!
 * \ingroup Acos
 * \brief compute Acos elementwisely for whole source tensor
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 * Input data valid range should be [-1, 1]
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Acos(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
{
    Acos<T, isReuseSource>(dstTensor, srcTensor, srcTensor.GetSize());
}

/*!
 * \ingroup Acos
 * \brief compute Acos elementwisely for whole source tensor
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] sharedTmpBuffer: extra temporary shared space used for intermediate values among calculation process,
 *             whose required space size should refer to corresponding tiling API, which is defined at acos_tiling.h.
 *             Generally, the more space you allocate, the better performance you will achieve, and the performance
 *             reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it is not guaranteed
 *             that the shared space will be cleared after usage, the data could be anything.
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 * Input data valid range should be [-1, 1]
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Acos(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
{
    Acos<T, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, srcTensor.GetSize());
}

#pragma end_pipe
} // namespace AscendC
#endif // LIB_MATH_ACOS_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ACOS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ACOS_H__
#endif
