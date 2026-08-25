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
 * \file logsoftmax.h
 * \brief LogSoftMax api of AscendC
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "logsoftmax.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LOGSOFTMAX_H__
#endif

#ifndef LIB_SOFTMAX_LOGSOFTMAX_H
#define LIB_SOFTMAX_LOGSOFTMAX_H

#include "kernel_tensor.h"
#include "kernel_tiling/kernel_tiling.h"
#include "include/adv_api/activation/softmax_utils.h"
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 3002 || \
     __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/activation/softmax/softmax_common.h"
#include "../../../impl/adv_api/detail/activation/softmax/logsoftmax_base_impl.h"
#endif
#pragma begin_pipe(V)

namespace AscendC {
/*!
 * \ingroup LogSoftMax
 * \brief compute process: max = rowmax(x), sum = rowsum(exp(x-max)), y = log(exp(x-max)/sum)
 * \note support data type: half and float
 * \param [out] dst: output y
 * \param [out] sumTensor: output sum
 * \param [out] maxTensor: output max
 * \param [in] src: input x
 * \param [in] sharedTmpBuffer: input local temporary Tensor,you can get the range by tilingfunc of
 *                               GetSoftMaxMinTmpSize/GetSoftMaxMaxTmpSize
 * \param [in] softmaxShapeInfo: input src shape
 * \param [in] tiling: input softmaxtiling
 * \param [in] isReuseSource: reserved param
 * \param [in] isDataFormatNZ: if the data format of input src is NZ
 */
template <typename T, bool isReuseSource = false, bool isDataFormatNZ = false>
__aicore__ inline void LogSoftMax(
    const LocalTensor<T>& dst, const LocalTensor<T>& sum, const LocalTensor<T>& max, const LocalTensor<T>& src,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const LogSoftMaxTiling& tiling,
    const SoftMaxShapeInfo& softmaxShapeInfo = {})
{
    if ASCEND_IS_AIC {
        return;
    }

#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 3002 || \
     __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    TRACE_START(TraceId::LogSoftMax);
    LogSoftMaxImpl<T, isReuseSource, isDataFormatNZ>(dst, sum, max, src, sharedTmpBuffer, tiling, softmaxShapeInfo);
    TRACE_STOP(TraceId::LogSoftMax);
#endif
}
} // namespace AscendC

#pragma end_pipe
#endif // LIB_SOFTMAX_LOGSOFTMAX_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LOGSOFTMAX_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LOGSOFTMAX_H__
#endif
