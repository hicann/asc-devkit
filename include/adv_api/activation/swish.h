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
 * \file swish.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "swish.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SWISH_H__
#endif

#ifndef LIB_SWISH_SWISH_H
#define LIB_SWISH_SWISH_H

#include "kernel_tensor.h"
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/activation/swish/swish_3510_impl.h"
#else
#include "../../../impl/adv_api/detail/activation/swish/swish_common_impl.h"
#endif

namespace AscendC {
/* !
 * \brief This function implements the swish function. swish(x) = x / (1 + e^(-βx))
 * (e.g. swish(0.5312) is 0.3784).
 *
 * \note support data type: half and float
 *
 * \param [out] dstTensor, output LocalTensor
 * \param [in] srcTensor, input LocalTensor
 * \param [in] dataSize, amount of data to be calculated
 * \param [in] scalarValue, The β Parameter in the Activation Function
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline __inout_pipe__(V) void Swish(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, uint32_t dataSize, const T scalarValue)
{
    // Only for AI Vector Core
    if ASCEND_IS_AIC {
        return;
    }
    SwishCompute<T, isReuseSource>(dstLocal, srcLocal, dataSize, scalarValue);
}
} // namespace AscendC
#endif // LIB_SWISH_SWISH_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SWISH_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SWISH_H__
#endif
