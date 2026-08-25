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
 * \file mean.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "mean.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MEAN_H__
#endif

#ifndef LIB_REDUCE_MEAN_H
#define LIB_REDUCE_MEAN_H

#include "kernel_tensor.h"
#include "kernel_basic_intf.h"
#include "include/adv_api/reduce/mean_utils.h"
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/reduce/mean/mean_3510_impl.h"
#else
#include "../../../impl/adv_api/detail/reduce/mean/mean_common_impl.h"
#endif
#if ASCENDC_CPU_DEBUG
#include "../../../impl/basic_api/kernel_log.h"
#include <type_traits>
#include <stdio.h>
#endif

namespace AscendC {
#pragma begin_pipe(V)
/* !
 * \brief This function calculates the average based on the orientation of the last axis.
 * For details about the interface description, see
 * https://pytorch.org/docs/stable/generated/torch.mean.html
 *
 * \note support data type: half and float
 *
 * \param [out] dstTensor, output LocalTensor
 * \param [in] srcTensor, input LocalTensor
 * \param [in] sharedTmpBuffer, input local temporary Tensor
 * \param [in] meanParams, shape information of srcTensor
 */
template <
    typename T, typename accType = T, bool isReuseSource = false, bool isBasicBlock = false, int32_t reduceDim = -1>
__aicore__ inline void Mean(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const MeanParams& meanParams)
{
    if ASCEND_IS_AIC {
        return;
    }
    MeanImpl<T, accType, isReuseSource, isBasicBlock, reduceDim>(dstTensor, srcTensor, sharedTmpBuffer, meanParams);
}

/* !
 * \brief This function calculates the average based on the orientation of the last axis.
 * For details about the interface description, see
 * https://pytorch.org/docs/stable/generated/torch.mean.html
 *
 * \note support data type: half and float
 *
 * \param [out] dstTensor, output LocalTensor
 * \param [in] srcTensor, input LocalTensor
 * \param [in] meanParams, shape information of srcTensor
 */
template <
    typename T, typename accType = T, bool isReuseSource = false, bool isBasicBlock = false, int32_t reduceDim = -1>
__aicore__ inline void Mean(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const MeanParams& meanParams)
{
    if ASCEND_IS_AIC {
        return;
    }
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    Mean<T, accType, isReuseSource, isBasicBlock, reduceDim>(dstTensor, srcTensor, sharedTmpBuffer, meanParams);
}
#pragma end_pipe
} // namespace AscendC

#endif // LIB_REDUCE_MEAN_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MEAN_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MEAN_H__
#endif
