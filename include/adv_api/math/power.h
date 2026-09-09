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
 * \file power.h
 * \brief Power function takes the power of base with exponent and returns a tensor with the result.
 * Mathematical formulas: Power(x, y) = x ^ y
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "power.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_POWER_H__
#endif

#ifndef LIB_MATH_POWER_H
#define LIB_MATH_POWER_H

#include "kernel_tensor.h"
#include "../../../impl/adv_api/detail/math/power/power_common_utils.h"

#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
#include "../../../impl/adv_api/detail/math/power/power_common_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || \
                                __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/math/power/power_3510_impl.h"
#endif
#endif

namespace AscendC {
#pragma begin_pipe(V)
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
/*!
 * \ingroup Power
 * \brief compute Power elementwisely.
 * \tparam T: input dataType, support half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 *         not enabled currently.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] src0Tensor: base LocalTensor
 * \param [in] src1Tensor: exponent LocalTensor
 * \param [in] sharedTmpBuffer：extra temporary shared space used for intermediate values among calculation process,
 *             whose required space size should refer to corresponding tiling API, which is defined at
 *             power_tiling.h. Generally, the more space you allocate, the better performance you will achieve,
 *             and the performance reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it
 *             is not guaranteed that the shared space will be cleared after usage, the data could be anything.
 * \param [in] calCount: amount of output data to be calculated
 */
template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
__aicore__ inline void Power(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, uint32_t calCount)
{
    PowerCommonImpl<T, isReuseSource, config>(dstTensor, src0Tensor, src1Tensor, sharedTmpBuffer, calCount);
}

/*!
 * \ingroup Power
 * \brief compute Power elementwisely.
 * \tparam T: input dataType, support half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 *         not enabled currently.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] src0Tensor: base LocalTensor
 * \param [in] src1Tensor: exponent LocalTensor
 * \param [in] calCount: amount of output data to be calculated
 */
template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
__aicore__ inline void Power(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor,
    uint32_t calCount)
{
    PowerCommonImpl<T, isReuseSource, config>(dstTensor, src0Tensor, src1Tensor, calCount);
}

/*!
 * \ingroup Power
 * \brief compute Power elementwisely.
 * \tparam T: input dataType, support half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 *         not enabled currently.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] src0Tensor: base LocalTensor
 * \param [in] src1Tensor: exponent LocalTensor
 * \param [in] sharedTmpBuffer：extra temporary shared space used for intermediate values among calculation process,
 *             whose required space size should refer to corresponding tiling API, which is defined at
 *             power_tiling.h. Generally, the more space you allocate, the better performance you will achieve,
 *             and the performance reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it
 *             is not guaranteed that the shared space will be cleared after usage, the data could be anything.
 */
template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
__aicore__ inline void Power(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer)
{
    Power<T, isReuseSource, config>(dstTensor, src0Tensor, src1Tensor, sharedTmpBuffer, src0Tensor.GetSize());
}

/*!
 * \ingroup Power
 * \brief compute Power elementwisely.
 * \tparam T: input dataType, support half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 *         not enabled currently.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] src0Tensor: base LocalTensor
 * \param [in] src1Tensor: exponent LocalTensor
 */
template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
__aicore__ inline void Power(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor)
{
    Power<T, isReuseSource, config>(dstTensor, src0Tensor, src1Tensor, src0Tensor.GetSize());
}

/*!
 * \ingroup Power
 * \brief compute Power elementwisely.
 * \tparam T: input dataType, support half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 *         not enabled currently.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] src0Tensor: base LocalTensor
 * \param [in] src1Scalar: exponent Scalar
 * \param [in] sharedTmpBuffer：extra temporary shared space used for intermediate values among calculation process,
 *             whose required space size should refer to corresponding tiling API, which is defined at
 *             power_tiling.h. Generally, the more space you allocate, the better performance you will achieve,
 *             and the performance reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it
 *             is not guaranteed that the shared space will be cleared after usage, the data could be anything.
 * \param [in] calCount: amount of output data to be calculated
 */
template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
__aicore__ inline void Power(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const T& src1Scalar,
    const LocalTensor<uint8_t>& sharedTmpBuffer, uint32_t calCount)
{
    PowerCommonImpl<T, isReuseSource, config>(dstTensor, src0Tensor, src1Scalar, sharedTmpBuffer, calCount);
}

/*!
 * \ingroup Power
 * \brief compute Power elementwisely.
 * \tparam T: input dataType, support half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 *         not enabled currently.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] src0Tensor: base LocalTensor
 * \param [in] src1Scalar: exponent Scalar
 * \param [in] calCount: amount of output data to be calculated
 */
template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
__aicore__ inline void Power(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const T& src1Scalar, uint32_t calCount)
{
    PowerCommonImpl<T, isReuseSource, config>(dstTensor, src0Tensor, src1Scalar, calCount);
}

/*!
 * \ingroup Power
 * \brief compute Power elementwisely.
 * \tparam T: input dataType, support half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 *         not enabled currently.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] src0Tensor: base LocalTensor
 * \param [in] src1Scalar: exponent Scalar
 * \param [in] sharedTmpBuffer：extra temporary shared space used for intermediate values among calculation process,
 *             whose required space size should refer to corresponding tiling API, which is defined at
 *             power_tiling.h. Generally, the more space you allocate, the better performance you will achieve,
 *             and the performance reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it
 *             is not guaranteed that the shared space will be cleared after usage, the data could be anything.
 */
template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
__aicore__ inline void Power(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const T& src1Scalar,
    const LocalTensor<uint8_t>& sharedTmpBuffer)
{
    Power<T, isReuseSource, config>(dstTensor, src0Tensor, src1Scalar, sharedTmpBuffer, src0Tensor.GetSize());
}

/*!
 * \ingroup Power
 * \brief compute Power elementwisely.
 * \tparam T: input dataType, support half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 *         not enabled currently.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] src0Tensor: base LocalTensor
 * \param [in] src1Scalar: exponent Scalar
 */
template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
__aicore__ inline void Power(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const T& src1Scalar)
{
    Power<T, isReuseSource, config>(dstTensor, src0Tensor, src1Scalar, src0Tensor.GetSize());
}

/*!
 * \ingroup Power
 * \brief compute Power elementwisely.
 * \tparam T: input dataType, support half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 *         not enabled currently.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] src0Scalar: base Scalar
 * \param [in] src1Tensor: exponent LocalTensor
 * \param [in] sharedTmpBuffer：extra temporary shared space used for intermediate values among calculation process,
 *             whose required space size should refer to corresponding tiling API, which is defined at
 *             power_tiling.h. Generally, the more space you allocate, the better performance you will achieve,
 *             and the performance reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it
 *             is not guaranteed that the shared space will be cleared after usage, the data could be anything.
 * \param [in] calCount: amount of output data to be calculated
 */
template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
__aicore__ inline void Power(
    const LocalTensor<T>& dstTensor, const T& src0Scalar, const LocalTensor<T>& src1Tensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, uint32_t calCount)
{
    PowerCommonImpl<T, isReuseSource, config>(dstTensor, src0Scalar, src1Tensor, sharedTmpBuffer, calCount);
}

/*!
 * \ingroup Power
 * \brief compute Power elementwisely.
 * \tparam T: input dataType, support half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 *         not enabled currently.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] src0Scalar: input Scalar
 * \param [in] src1Tensor: exponent LocalTensor
 * \param [in] calCount: amount of output data to be calculated
 */
template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
__aicore__ inline void Power(
    const LocalTensor<T>& dstTensor, const T& src0Scalar, const LocalTensor<T>& src1Tensor, uint32_t calCount)
{
    PowerCommonImpl<T, isReuseSource, config>(dstTensor, src0Scalar, src1Tensor, calCount);
}

/*!
 * \ingroup Power
 * \brief compute Power elementwisely.
 * \tparam T: input dataType, support half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 *         not enabled currently.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] src0Scalar: base Scalar
 * \param [in] src1Tensor: exponent LocalTensor
 * \param [in] sharedTmpBuffer：extra temporary shared space used for intermediate values among calculation process,
 *             whose required space size should refer to corresponding tiling API, which is defined at
 *             power_tiling.h. Generally, the more space you allocate, the better performance you will achieve,
 *             and the performance reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it
 *             is not guaranteed that the shared space will be cleared after usage, the data could be anything.
 */
template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
__aicore__ inline void Power(
    const LocalTensor<T>& dstTensor, const T& src0Scalar, const LocalTensor<T>& src1Tensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer)
{
    PowerCommonImpl<T, isReuseSource, config>(dstTensor, src0Scalar, src1Tensor, sharedTmpBuffer, src1Tensor.GetSize());
}

/*!
 * \ingroup Power
 * \brief compute Power elementwisely.
 * \tparam T: input dataType, support half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 *         not enabled currently.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] src0Scalar: base Scalar
 * \param [in] src1Tensor: exponent LocalTensor
 */
template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
__aicore__ inline void Power(const LocalTensor<T>& dstTensor, const T& src0Scalar, const LocalTensor<T>& src1Tensor)
{
    PowerCommonImpl<T, isReuseSource, config>(dstTensor, src0Scalar, src1Tensor, src1Tensor.GetSize());
}
#else
/*!
 * \ingroup Power
 * \brief compute Power elementwisely.
 * \tparam T: input dataType, support half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 *         not enabled currently.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] src0Tensor: base LocalTensor
 * \param [in] src1Tensor: exponent LocalTensor
 * \param [in] sharedTmpBuffer：extra temporary shared space used for intermediate values among calculation process,
 *             whose required space size should refer to corresponding tiling API, which is defined at
 *             power_tiling.h. Generally, the more space you allocate, the better performance you will achieve,
 *             and the performance reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it
 *             is not guaranteed that the shared space will be cleared after usage, the data could be anything.
 * \param [in] calCount: amount of output data to be calculated
 */
template <typename T, bool isReuseSource = false>
__ASC_USE_RESERVED_UBUF__(2201, "Power is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Power(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, uint32_t calCount)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
    PowerCommonImpl<T, isReuseSource>(dstTensor, src0Tensor, src1Tensor, sharedTmpBuffer, calCount);
#endif
}

/*!
 * \ingroup Power
 * \brief compute Power elementwisely.
 * \tparam T: input dataType, support half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 *         not enabled currently.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] src0Tensor: base LocalTensor
 * \param [in] src1Tensor: exponent LocalTensor
 * \param [in] calCount: amount of output data to be calculated
 */
template <typename T, bool isReuseSource = false>
__ASC_USE_RESERVED_UBUF__(2201, "Power is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Power(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor,
    uint32_t calCount)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
    PowerCommonImpl<T, isReuseSource>(dstTensor, src0Tensor, src1Tensor, calCount);
#endif
}

/*!
 * \ingroup Power
 * \brief compute Power elementwisely.
 * \tparam T: input dataType, support half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 *         not enabled currently.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] src0Tensor: base LocalTensor
 * \param [in] src1Tensor: exponent LocalTensor
 * \param [in] sharedTmpBuffer：extra temporary shared space used for intermediate values among calculation process,
 *             whose required space size should refer to corresponding tiling API, which is defined at
 *             power_tiling.h. Generally, the more space you allocate, the better performance you will achieve,
 *             and the performance reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it
 *             is not guaranteed that the shared space will be cleared after usage, the data could be anything.
 */
template <typename T, bool isReuseSource = false>
__ASC_USE_RESERVED_UBUF__(2201, "Power is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Power(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer)
{
    Power<T, isReuseSource>(dstTensor, src0Tensor, src1Tensor, sharedTmpBuffer, src0Tensor.GetSize());
}

/*!
 * \ingroup Power
 * \brief compute Power elementwisely.
 * \tparam T: input dataType, support half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 *         not enabled currently.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] src0Tensor: base LocalTensor
 * \param [in] src1Tensor: exponent LocalTensor
 */
template <typename T, bool isReuseSource = false>
__ASC_USE_RESERVED_UBUF__(2201, "Power is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Power(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor)
{
    Power<T, isReuseSource>(dstTensor, src0Tensor, src1Tensor, src0Tensor.GetSize());
}

/*!
 * \ingroup Power
 * \brief compute Power elementwisely.
 * \tparam T: input dataType, support half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 *         not enabled currently.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] src0Tensor: base LocalTensor
 * \param [in] src1Scalar: exponent Scalar
 * \param [in] sharedTmpBuffer：extra temporary shared space used for intermediate values among calculation process,
 *             whose required space size should refer to corresponding tiling API, which is defined at
 *             power_tiling.h. Generally, the more space you allocate, the better performance you will achieve,
 *             and the performance reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it
 *             is not guaranteed that the shared space will be cleared after usage, the data could be anything.
 * \param [in] calCount: amount of output data to be calculated
 */
template <typename T, bool isReuseSource = false>
__ASC_USE_RESERVED_UBUF__(2201, "Power is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Power(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const T& src1Scalar,
    const LocalTensor<uint8_t>& sharedTmpBuffer, uint32_t calCount)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
    PowerCommonImpl<T, isReuseSource>(dstTensor, src0Tensor, src1Scalar, sharedTmpBuffer, calCount);
#endif
}

/*!
 * \ingroup Power
 * \brief compute Power elementwisely.
 * \tparam T: input dataType, support half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 *         not enabled currently.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] src0Tensor: base LocalTensor
 * \param [in] src1Scalar: exponent Scalar
 * \param [in] calCount: amount of output data to be calculated
 */
template <typename T, bool isReuseSource = false>
__ASC_USE_RESERVED_UBUF__(2201, "Power is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Power(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const T& src1Scalar, uint32_t calCount)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
    PowerCommonImpl<T, isReuseSource>(dstTensor, src0Tensor, src1Scalar, calCount);
#endif
}

/*!
 * \ingroup Power
 * \brief compute Power elementwisely.
 * \tparam T: input dataType, support half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 *         not enabled currently.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] src0Tensor: base LocalTensor
 * \param [in] src1Scalar: exponent Scalar
 * \param [in] sharedTmpBuffer：extra temporary shared space used for intermediate values among calculation process,
 *             whose required space size should refer to corresponding tiling API, which is defined at
 *             power_tiling.h. Generally, the more space you allocate, the better performance you will achieve,
 *             and the performance reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it
 *             is not guaranteed that the shared space will be cleared after usage, the data could be anything.
 */
template <typename T, bool isReuseSource = false>
__ASC_USE_RESERVED_UBUF__(2201, "Power is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Power(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const T& src1Scalar,
    const LocalTensor<uint8_t>& sharedTmpBuffer)
{
    Power<T, isReuseSource>(dstTensor, src0Tensor, src1Scalar, sharedTmpBuffer, src0Tensor.GetSize());
}

/*!
 * \ingroup Power
 * \brief compute Power elementwisely.
 * \tparam T: input dataType, support half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 *         not enabled currently.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] src0Tensor: base LocalTensor
 * \param [in] src1Scalar: exponent Scalar
 */
template <typename T, bool isReuseSource = false>
__ASC_USE_RESERVED_UBUF__(2201, "Power is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Power(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const T& src1Scalar)
{
    Power<T, isReuseSource>(dstTensor, src0Tensor, src1Scalar, src0Tensor.GetSize());
}

/*!
 * \ingroup Power
 * \brief compute Power elementwisely.
 * \tparam T: input dataType, support half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 *         not enabled currently.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] src0Scalar: base Scalar
 * \param [in] src1Tensor: exponent LocalTensor
 * \param [in] sharedTmpBuffer：extra temporary shared space used for intermediate values among calculation process,
 *             whose required space size should refer to corresponding tiling API, which is defined at
 *             power_tiling.h. Generally, the more space you allocate, the better performance you will achieve,
 *             and the performance reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it
 *             is not guaranteed that the shared space will be cleared after usage, the data could be anything.
 * \param [in] calCount: amount of output data to be calculated
 */
template <typename T, bool isReuseSource = false>
__ASC_USE_RESERVED_UBUF__(2201, "Power is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Power(
    const LocalTensor<T>& dstTensor, const T& src0Scalar, const LocalTensor<T>& src1Tensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, uint32_t calCount)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
    PowerCommonImpl<T, isReuseSource>(dstTensor, src0Scalar, src1Tensor, sharedTmpBuffer, calCount);
#endif
}

/*!
 * \ingroup Power
 * \brief compute Power elementwisely.
 * \tparam T: input dataType, support half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 *         not enabled currently.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] src0Scalar: input Scalar
 * \param [in] src1Tensor: exponent LocalTensor
 * \param [in] calCount: amount of output data to be calculated
 */
template <typename T, bool isReuseSource = false>
__ASC_USE_RESERVED_UBUF__(2201, "Power is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Power(
    const LocalTensor<T>& dstTensor, const T& src0Scalar, const LocalTensor<T>& src1Tensor, uint32_t calCount)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
    PowerCommonImpl<T, isReuseSource>(dstTensor, src0Scalar, src1Tensor, calCount);
#endif
}

/*!
 * \ingroup Power
 * \brief compute Power elementwisely.
 * \tparam T: input dataType, support half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 *         not enabled currently.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] src0Scalar: base Scalar
 * \param [in] src1Tensor: exponent LocalTensor
 * \param [in] sharedTmpBuffer：extra temporary shared space used for intermediate values among calculation process,
 *             whose required space size should refer to corresponding tiling API, which is defined at
 *             power_tiling.h. Generally, the more space you allocate, the better performance you will achieve,
 *             and the performance reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it
 *             is not guaranteed that the shared space will be cleared after usage, the data could be anything.
 */
template <typename T, bool isReuseSource = false>
__ASC_USE_RESERVED_UBUF__(2201, "Power is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Power(
    const LocalTensor<T>& dstTensor, const T& src0Scalar, const LocalTensor<T>& src1Tensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
    PowerCommonImpl<T, isReuseSource>(dstTensor, src0Scalar, src1Tensor, sharedTmpBuffer, src1Tensor.GetSize());
#endif
}

/*!
 * \ingroup Power
 * \brief compute Power elementwisely.
 * \tparam T: input dataType, support half/float
 * \tparam isReuseSource: whether allows API to modify source data, usually for performance reason,
 *         not enabled currently.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] src0Scalar: base Scalar
 * \param [in] src1Tensor: exponent LocalTensor
 */
template <typename T, bool isReuseSource = false>
__ASC_USE_RESERVED_UBUF__(2201, "Power is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Power(const LocalTensor<T>& dstTensor, const T& src0Scalar, const LocalTensor<T>& src1Tensor)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
    PowerCommonImpl<T, isReuseSource>(dstTensor, src0Scalar, src1Tensor, src1Tensor.GetSize());
#endif
}
#endif
#pragma end_pipe
} // namespace AscendC
#endif // LIB_MATH_POWER_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_POWER_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_POWER_H__
#endif
