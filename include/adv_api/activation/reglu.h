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
 * \file reglu.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "reglu.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REGLU_H__
#endif

#ifndef LIB_REGLU_REGLU_H
#define LIB_REGLU_REGLU_H

#include "kernel_tensor.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/activation/reglu/reglu_3510_impl.h"
#else
#include "../../../impl/adv_api/detail/activation/reglu/reglu_common_impl.h"
#endif
namespace AscendC {
#pragma begin_pipe(V)
/*
 * @brief ReGLU is an activation function which is a variant of GLU(e.g.ReGlu(1, 1) is 1).
 * Mathematical formulas: ReGlu(x1, x2) = x1 * max(0, x2)
 * @ingroup ReGlu
 * @param [out] dstTensor, output LocalTensor
 * @param [in] srcTensor, input LocalTensor
 * @param [in] sharedTmpBuffer, input local temporary Tensor
 * @param [in] calCount, amount of input data to be calculated
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void ReGlu(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
{
    if (g_coreType == AIC) {
        return;
    }
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    ReGluImpl<T, isReuseSource>(dstTensor, srcTensor0, srcTensor1, sharedTmpBuffer, calCount);
#endif
}

/*
 * @ingroup ReGlu
 * @param [out] dstTensor, output LocalTensor
 * @param [in] srcTensor, input local temporary Tensor
 * @param [in] calCount, amount of input data to be calculated
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void ReGlu(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1,
    const uint32_t calCount)
{
    if (g_coreType == AIC) {
        return;
    }
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    ReGluImpl<T, isReuseSource>(dstTensor, srcTensor0, srcTensor1, calCount);
#endif
}

#pragma end_pipe
} // namespace AscendC
#endif // LIB_REGLU_REGLU_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REGLU_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REGLU_H__
#endif
