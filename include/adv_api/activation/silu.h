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
 * \file silu.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "silu.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SILU_H__
#endif

#ifndef LIB_SILU_SILU_H
#define LIB_SILU_SILU_H

#include "kernel_tensor.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/activation/silu/silu_3510_impl.h"
#else
#include "../../../impl/adv_api/detail/activation/silu/silu_common_impl.h"
#endif
namespace AscendC {
/* !
 * \brief This function implements the Sigmoid Linear Unit (SiLU) function. silu(x) = x / (1 + e^(-x))
 * (e.g. silu(1.04788) is 0.7758789). For details about the interface description, see
 * https://pytorch.org/docs/stable/generated/torch.nn.SiLU.html
 *
 * \note support data type: half and float
 *
 * \param [out] dstTensor, output LocalTensor
 * \param [in] srcTensor, input LocalTensor
 * \param [in] dataSize, amount of data to be calculated
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline __inout_pipe__(V) void Silu(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, uint32_t dataSize)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    SiluCompute<T, isReuseSource>(dstLocal, srcLocal, dataSize);
}

} // namespace AscendC
#endif // LIB_SILU_SILU_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SILU_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SILU_H__
#endif
