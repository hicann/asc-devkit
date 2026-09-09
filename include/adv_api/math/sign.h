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
 * \file sign.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "sign.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SIGN_H__
#endif

#ifndef LIB_MATH_SIGN_H
#define LIB_MATH_SIGN_H
#include <type_traits>
#include "kernel_basic_intf.h"
#include "../../../impl/basic_api/kernel_log.h"
#include "kernel_tensor.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
#include "../../../impl/adv_api/detail/math/sign/sign_common_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || \
                                __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/math/sign/sign_3510_impl.h"
#endif

#pragma begin_pipe(V)

namespace AscendC {
/*!
 * \ingroup Sign
 * \brief compute the Sign operation by element. (e.g. sign(0.1) = 1, sign(-0.1) is -1), sign(0) is 0)
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] sharedTmpBuffer: input local temporary Tensor
 * \param [in] calCount: the number of elements to be processed.
 */
template <typename T, bool isReuseSource = false>
__ASC_USE_RESERVED_UBUF__(2201, "Sign is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Sign(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t calCount)
{
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    SignCompute<T, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, calCount);
#endif
}

/*!
 * \ingroup Sign
 * \brief compute the Sign operation by element. (e.g. sign(0.1) = 1, sign(-0.1) is -1), sign(0) is 0)
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] sharedTmpBuffer: input local temporary Tensor
 */
template <typename T, bool isReuseSource = false>
__ASC_USE_RESERVED_UBUF__(2201, "Sign is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Sign(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
{
    Sign<T, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, srcTensor.GetSize());
}

/*!
 * \ingroup Sign
 * \brief compute the Sign operation by element. (e.g. sign(0.1) = 1, sign(-0.1) is -1), sign(0) is 0)
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] calCount: the number of elements to be processed.
 */
template <typename T, bool isReuseSource = false>
__ASC_USE_RESERVED_UBUF__(2201, "Sign is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Sign(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102)
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    SignCompute<T, isReuseSource>(dstTensor, srcTensor, calCount);
#endif
#else
    // Using the Stack Space to Allocate sharedTmpBuffer
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    Sign<T, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, calCount);
#endif
}

/*!
 * \ingroup Sign
 * \brief compute the Sign operation by element. (e.g. sign(0.1) = 1, sign(-0.1) is -1), sign(0) is 0)
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data.
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 */
template <typename T, bool isReuseSource = false>
__ASC_USE_RESERVED_UBUF__(2201, "Sign is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Sign(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
{
    Sign<T, isReuseSource>(dstTensor, srcTensor, srcTensor.GetSize());
}
} // namespace AscendC
#pragma end_pipe

#endif // LIB_MATH_SIGN_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SIGN_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SIGN_H__
#endif
