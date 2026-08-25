/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file dequantize.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "dequantize.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEQUANTIZE_H__
#endif

#ifndef LIB_QUANTIZATION_DEQUANTIZE_H
#define LIB_QUANTIZATION_DEQUANTIZE_H
#include "kernel_tensor.h"
#include "../../../impl/adv_api/detail/quantization/dequantize/dequantize_common.h"
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/quantization/dequantize/dequantize_impl.h"
#endif
namespace AscendC {
#pragma begin_pipe(V)

/*!
 * \ingroup Dequantize
 * \brief For DequantizeParams {m, n, groupSize}, m means src tensor has m rows, each row has n num;
 *        Calculation process depends on dequantize policy. For example, Dequantize PER_TENSOR is specified as below:
 *            dstTensor[i][j] = round(src[i][j] * scale)
 *        For more information, please refer to document.
 * \tparam DequantizeConfig: {policy, hasOffset, kDim} indicate dequantize config parameters.
 * \tparam DstT: dstTensor data type.
 * \tparam SrcT: srcTensor data type.
 * \tparam ScaleT: scale data type, which can be scalar or LocalTensor depending on policy.
 * \tparam OffsetT: offset data type, reserved.
 * \param [out] dstTensor: Output localTensor.
 * \param [in] srcTensor: Input src localTensor.
 * \param [in] scale: Input scale.
 * \param [in] offset: Reserved input offset.
 * \param [in] sharedTmpBuffer�?extra temporary shared space used for intermediate values among calculation process,
 *             whose required space size should refer to corresponding tiling API, which is defined at
 *             ascend_dequant_tiling.h. Generally, the more space you allocate, the better performance you will achieve,
 *             and the performance reaches peak when buffer size is maximum(calculated by tiling function). Moreover, it
 *             is not guaranteed that the shared space will be cleared after usage, the data could be anything.
 * \param [in] params: DequantizeParams with m, n, groupSize to describe the calculation process like above.
 */
template <const DequantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
__aicore__ inline void Dequantize(
    const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const DequantizeParams& params)
{
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    DequantizeImpl<config>(dstTensor, srcTensor, scale, offset, sharedTmpBuffer, params);
#endif
}

/*!
 * \ingroup Dequantize
 * \brief For DequantizeParams {m, n, groupSize}, m means src tensor has m rows, each row has n num;
 *        Calculation process depends on dequantize policy. For example, Dequantize PER_TENSOR is specified as below:
 *            dstTensor[i][j] = round(src[i][j] * scale)
 *        For more information, please refer to document.
 * \tparam DequantizeConfig: {policy, hasOffset, kDim} indicate dequantize config parameters.
 * \tparam DstT: dstTensor data type.
 * \tparam SrcT: srcTensor data type.
 * \tparam ScaleT: scale data type, which can be scalar or LocalTensor depending on policy.
 * \tparam OffsetT: offset data type, reserved.
 * \param [out] dstTensor: Output localTensor.
 * \param [in] srcTensor: Input src localTensor.
 * \param [in] scale: Input scale.
 * \param [in] offset: Reserved input offset.
 * \param [in] params: DequantizeParams with m, n, groupSize to describe the calculation process like above.
 */
template <const DequantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
__aicore__ inline void Dequantize(
    const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset,
    const DequantizeParams& params)
{
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    DequantizeImpl<config>(dstTensor, srcTensor, scale, offset, params);
#endif
}
#pragma end_pipe
} // namespace AscendC
#endif // LIB_QUANTIZATION_DEQUANTIZE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEQUANTIZE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEQUANTIZE_H__
#endif
