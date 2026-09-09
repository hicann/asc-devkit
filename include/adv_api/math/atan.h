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
 * \file atan.h
 * \brief Defines a series of interface used to do elementwise math arctan calculation.
 * Formula: Atan(x) = arctan(x) = x - x^3/3 + x^5/5 + ... + (-1)^k*x^(k*2+1)/(k*2+1)
 * The Atan function is calculating by taylor expansion function approximation.
 * when x < 0, Atan(x) = atan(-x)
 * when x belongs to (0, tan(pi/8)), Atan(x) = atan(x)
 * when x belongs to (tan(pi/8), tan(pi/4)), Atan(x) = pi/8 + atan((x- tan(pi/8)) / (1+ x*tan(pi/8)))
 * when x belongs to (tan(pi/4), +�?, Atan(x) = pi/4 + atan((x-1)/(x+1))
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "atan.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ATAN_H__
#endif

#ifndef LIB_MATH_ATAN_H
#define LIB_MATH_ATAN_H

#include "kernel_tensor.h"
#include "include/adv_api/math/atan_utils.h"

#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
#include "../../../impl/adv_api/detail/math/atan/atan_common_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || \
                                __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/math/atan/atan_3510_impl.h"
#endif
#endif

namespace AscendC {
#pragma begin_pipe(V)
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
/*!
 * \ingroup Atan
 * \brief compute Atan elementwisely
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 * this parameter is reserved, please use the default value.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] sharedTmpBuffer: extra temporary shared space used for intermediate values among calculation process,
 * whose required space size should refer to corresponding tiling API, which is defined at atan_tiling.h.
 * Generally, the more space you allocate, the better performance you will achieve, and the performance
 * reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it is not guaranteed
 * that the shared space will be cleared after usage, the data could be anything.
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 */
template <typename T, bool isReuseSource = false, const AtanConfig& config = defaultAtanConfig>
__aicore__ inline void Atan(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
{
    Atan<T, isReuseSource, config>(dstTensor, srcTensor, sharedTmpBuffer, srcTensor.GetSize());
}

/*!
 * \ingroup Atan
 * \brief compute Atan elementwisely
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 * this parameter is reserved, please use the default value.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] sharedTmpBuffer: extra temporary shared space used for intermediate values among calculation process,
 * whose required space size should refer to corresponding tiling API, which is defined at atan_tiling.h.
 * Generally, the more space you allocate, the better performance you will achieve, and the performance
 * reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it is not guaranteed
 * that the shared space will be cleared after usage, the data could be anything.
 * \param [in] calCount: the number of elements to be processed.
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 */
template <typename T, bool isReuseSource = false, const AtanConfig& config = defaultAtanConfig>
__aicore__ inline void Atan(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t calCount)
{
    AtanImpl<T, isReuseSource, config>(dstTensor, srcTensor, sharedTmpBuffer, calCount);
}

/*!
 * \ingroup Atan
 * \brief compute Atan elementwisely
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 * this parameter is reserved, please use the default value.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 */
template <typename T, bool isReuseSource = false, const AtanConfig& config = defaultAtanConfig>
__aicore__ inline void Atan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
{
    Atan<T, isReuseSource, config>(dstTensor, srcTensor, srcTensor.GetSize());
}

/*!
 * \ingroup Atan
 * \brief compute Atan elementwisely
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 * this parameter is reserved, please use the default value.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] calCount: the number of elements to be processed.
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 */
template <typename T, bool isReuseSource = false, const AtanConfig& config = defaultAtanConfig>
__aicore__ inline void Atan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
    AtanImpl<T, isReuseSource, config>(dstTensor, srcTensor, calCount);
}
#else
/*!
 * \ingroup Atan
 * \brief compute Atan elementwisely
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 * this parameter is reserved, please use the default value.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] sharedTmpBuffer: extra temporary shared space used for intermediate values among calculation process,
 * whose required space size should refer to corresponding tiling API, which is defined at atan_tiling.h.
 * Generally, the more space you allocate, the better performance you will achieve, and the performance
 * reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it is not guaranteed
 * that the shared space will be cleared after usage, the data could be anything.
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Atan(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
{
    Atan<T, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, srcTensor.GetSize());
}

/*!
 * \ingroup Atan
 * \brief compute Atan elementwisely
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 * this parameter is reserved, please use the default value.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] sharedTmpBuffer: extra temporary shared space used for intermediate values among calculation process,
 * whose required space size should refer to corresponding tiling API, which is defined at atan_tiling.h.
 * Generally, the more space you allocate, the better performance you will achieve, and the performance
 * reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it is not guaranteed
 * that the shared space will be cleared after usage, the data could be anything.
 * \param [in] calCount: the number of elements to be processed.
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Atan(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t calCount)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
    AtanImpl<T, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, calCount);
#endif
}

/*!
 * \ingroup Atan
 * \brief compute Atan elementwisely
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 * this parameter is reserved, please use the default value.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Atan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
{
    Atan<T, isReuseSource>(dstTensor, srcTensor, srcTensor.GetSize());
}

/*!
 * \ingroup Atan
 * \brief compute Atan elementwisely
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 * this parameter is reserved, please use the default value.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] calCount: the number of elements to be processed.
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst/sharedTmpBuffer tensor address overlap.
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Atan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
    AtanImpl<T, isReuseSource>(dstTensor, srcTensor, calCount);
#endif
}
#endif
#pragma end_pipe
} // namespace AscendC

#endif // LIB_MATH_ATAN_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ATAN_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ATAN_H__
#endif
