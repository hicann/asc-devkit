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
 * \file cumsum.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "cumsum.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CUMSUM_H__
#endif

#ifndef LIB_MATH_CUMSUM_H
#define LIB_MATH_CUMSUM_H

#include "kernel_basic_intf.h"
#include "kernel_tensor.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/math/cumsum/regbase/3510/cumsum_3510_impl.h"
#else
#include "../../../impl/adv_api/detail/math/cumsum/cumsum_common_impl.h"
#endif
#include "include/adv_api/math/cumsum_utils.h"
#if ASCENDC_CPU_DEBUG
#include "../../../impl/basic_api/kernel_log.h"
#endif

namespace AscendC {
#pragma begin_pipe(V)

constexpr CumSumConfig defaultCumSumConfig = {true, false, true};

/* !
 * \brief This function calculates the average based on the orientation of the last axis or fist axis.
 * For details about the interface description, see
 * https://pytorch.org/docs/stable/generated/torch.cumsum.html
 *
 * \note support data type: half and float
 *
 * \param [out] dstTensor, output LocalTensor
 * \param [out] lastRowTensor, the last row of the output LocalTensor
 * \param [in] srcTensor, input LocalTensor
 * \param [in] sharedTmpBuffer, input local temporary Tensor
 * \param [in] cumSumInfo, shape information of srcTensor
 */

template <typename T, const CumSumConfig& config = defaultCumSumConfig>
__aicore__ inline void CumSum(
    LocalTensor<T>& dstTensor, LocalTensor<T>& lastRowTensor, const LocalTensor<T>& srcTensor,
    LocalTensor<uint8_t>& sharedTmpBuffer, const CumSumInfo& cumSumInfo)
{
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    CumSumImpl<T, config>(dstTensor, lastRowTensor, srcTensor, sharedTmpBuffer, cumSumInfo);
#endif
}

/* !
 * \brief This function calculates the average based on the orientation of the last axis or fist axis.
 * For details about the interface description, see
 * https://pytorch.org/docs/stable/generated/torch.cumsum.html
 *
 * \note support data type: half and float
 *
 * \param [out] dstTensor, output LocalTensor
 * \param [out] lastRowTensor, the last row of the output LocalTensor
 * \param [in] srcTensor, input LocalTensor
 * \param [in] cumSumInfo, shape information of srcTensor
 */
template <typename T, const CumSumConfig& config = defaultCumSumConfig>
__aicore__ inline void CumSum(
    LocalTensor<T>& dstTensor, LocalTensor<T>& lastRowTensor, const LocalTensor<T>& srcTensor,
    const CumSumInfo& cumSumInfo)
{
    if ASCEND_IS_AIC {
        return;
    }
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    CumSum<T, config>(dstTensor, lastRowTensor, srcTensor, sharedTmpBuffer, cumSumInfo);
}

#pragma end_pipe
} // namespace AscendC

#endif // LIB_CUMSUM_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CUMSUM_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CUMSUM_H__
#endif
