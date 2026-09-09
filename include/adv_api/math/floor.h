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
 * \file floor.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "floor.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_FLOOR_H__
#endif

#ifndef LIB_MATH_FLOOR_H
#define LIB_MATH_FLOOR_H

#include "kernel_tensor.h"
#include "../../../impl/adv_api/detail/math/floor/floor_common_impl.h"

namespace AscendC {
#pragma begin_pipe(V)
/*!
 * \brief the floor function is the function that takes as input a real number x,
 * and gives as output the greatest integer less than or equal to x. (e.g. floor(2.4) is 2, floor(-2.4) is -3).
 * For details about the interface description, see
 * https://pytorch.org/docs/stable/generated/torch.floor.html.
 *
 * \note support data type: half and float
 *
 * \param [out] dstTensor, output LocalTensor
 * \param [in] srcTensor, input LocalTensor
 * \param [in] sharedTmpBuffer, input local temporary Tensor
 * \param [in] calCount, amount of data to be calculated
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Floor(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t calCount)
{
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
#endif
    static_assert(
        (std::is_same<T, float>::value || std::is_same<T, half>::value),
        "Failed to check the data types, current api support data types are half/float.");
#if (                                                                                                \
    defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 3510 || \
     __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113))
    FloorImpl<T, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, calCount);
#endif
}

/* !
 * \ingroup Floor
 * \note support data type: half and float
 * \param [out] dstTensor, output LocalTensor
 * \param [in] srcTensor, input LocalTensor
 * \param [in] calCount, amount of data to be calculated
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Floor(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
#endif
    static_assert(
        (std::is_same<T, float>::value || std::is_same<T, half>::value),
        "Failed to check the data types, current api support data types are half/float.");
#if (                                                                                                \
    defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 3510 || \
     __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113))
    FloorImpl<T, isReuseSource>(dstTensor, srcTensor, calCount);
#endif
}

#pragma end_pipe
} // namespace AscendC
#endif // LIB_MATH_FLOOR_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_FLOOR_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_FLOOR_H__
#endif
