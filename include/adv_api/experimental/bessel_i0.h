/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file bessel_i0.h
 * \brief Defines the BesselI0 high-level API used to compute the modified Bessel function
 * of the first kind of order 0, I0(x), elementwisely.
 * Formula: I0(x) = sum_{k=0}^{+inf} (1 / (k!)^2) * (x/2)^{2k}
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "bessel_i0.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ADV_API_EXPERIMENTAL_BESSEL_I0_H__
#endif

#ifndef INCLUDE_ADV_API_EXPERIMENTAL_BESSEL_I0_H
#define INCLUDE_ADV_API_EXPERIMENTAL_BESSEL_I0_H

#ifndef CANN_ASC_USE_EXPERIMENTAL
#error "this is experimental feature, pls set CANN_ASC_USE_EXPERIMENTAL = ON to use it"
#endif
// WARNING: The APIs in this header are unstable and may change or be removed in future releases.

#include "kernel_tensor.h"

// The implementation header uses AscendC::Reg::RegTensor/MaskReg and other RegBase types that are
// only declared in the device-side DAV_3510 compile pass, so it must only be included when
// compiling for __NPU_ARCH__ == 3510 (same pattern as erf.h).
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "../../../impl/adv_api/detail/experimental/math/bessel_i0/bessel_i0_3510_impl.h"
#endif

namespace AscendC {
namespace experimental {
#pragma begin_pipe(V)
/*!
 * \ingroup BesselI0
 * \brief compute the modified Bessel function of the first kind of order 0, I0(x), elementwisely
 * \tparam T: data type, only float is supported
 * \tparam isReuseSource: reserved parameter, pass the default value false
 * \param [out] dstTensor: output LocalTensor, same shape/dtype as srcTensor,
 *        element count >= calCount
 * \param [in] srcTensor: input LocalTensor, element count >= calCount, address must not overlap
 *        with dstTensor
 * \param [in] sharedTmpBuffer: extra temporary shared space. This interface needs no temporary
 *        space (the max/min temporary space sizes obtained from GetBesselI0MaxMinTmpSize are
 *        both 0); the parameter is accepted but not used, and any valid LocalTensor<uint8_t>
 *        (including a zero-length one) may be passed.
 * \param [in] calCount: the number of elements to be processed
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst tensor address overlap.
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void BesselI0(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t calCount)
{
    (void)sharedTmpBuffer;
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    BesselI0Impl<T, isReuseSource>(dstTensor, srcTensor, calCount);
#endif
}

/*!
 * \ingroup BesselI0
 * \brief compute I0(x) elementwisely for the whole source tensor
 *        (calCount = srcTensor.GetSize())
 * \tparam T: data type, only float is supported
 * \tparam isReuseSource: reserved parameter, pass the default value false
 * \param [out] dstTensor: output LocalTensor, same as the overload with calCount
 * \param [in] srcTensor: input LocalTensor, same as the overload with calCount
 * \param [in] sharedTmpBuffer: accepted but not used, same as the overload with calCount
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst tensor address overlap.
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void BesselI0(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
{
    BesselI0<T, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, srcTensor.GetSize());
}

/*!
 * \ingroup BesselI0
 * \brief compute I0(x) elementwisely, with the temporary space applied by the interface framework
 * \tparam T: data type, only float is supported
 * \tparam isReuseSource: reserved parameter, pass the default value false
 * \param [out] dstTensor: output LocalTensor, same as the overload with sharedTmpBuffer
 * \param [in] srcTensor: input LocalTensor, same as the overload with sharedTmpBuffer
 * \param [in] calCount: the number of elements to be processed
 * \note This implementation needs no temporary space, so this overload behaves identically to the
 *       overload with sharedTmpBuffer. src/dst Tensor must be 32B aligned, and it doesn't allow
 *       src/dst tensor address overlap.
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void BesselI0(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    BesselI0Impl<T, isReuseSource>(dstTensor, srcTensor, calCount);
#endif
}

/*!
 * \ingroup BesselI0
 * \brief compute I0(x) elementwisely for the whole source tensor, with the temporary space
 *        applied by the interface framework (calCount = srcTensor.GetSize())
 * \tparam T: data type, only float is supported
 * \tparam isReuseSource: reserved parameter, pass the default value false
 * \param [out] dstTensor: output LocalTensor, same as the overload with sharedTmpBuffer
 * \param [in] srcTensor: input LocalTensor, same as the overload with sharedTmpBuffer
 * \note src/dst Tensor must be 32B aligned, and it doesn't allow src/dst tensor address overlap.
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void BesselI0(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
{
    BesselI0<T, isReuseSource>(dstTensor, srcTensor, srcTensor.GetSize());
}
#pragma end_pipe
} // namespace experimental
} // namespace AscendC

#endif // INCLUDE_ADV_API_EXPERIMENTAL_BESSEL_I0_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ADV_API_EXPERIMENTAL_BESSEL_I0_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ADV_API_EXPERIMENTAL_BESSEL_I0_H__
#endif
