/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "rmsnorm.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_RMSNORM_H__
#endif

#ifndef LIB_NORMALIZATION_RMSNORM_H
#define LIB_NORMALIZATION_RMSNORM_H
#include "kernel_tensor.h"
#include "kernel_basic_intf.h"
#include "../../../impl/basic_api/kernel_pop_stack_buffer.h"
#include "kernel_tiling/kernel_tiling.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002)
#include "../../../impl/adv_api/detail/normalization/rmsnorm/rmsnorm_common_impl.h"
#elif defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/normalization/rmsnorm/rmsnorm_3510_impl.h"
#endif
namespace AscendC {
#pragma begin_pipe(V)
/*!
 * \brief Root Mean Square Layer Normalization, for more info see https://arxiv.org/abs/1910.07467
 *
 * \note support data type: half and float
 *
 * \param [in] isBasicBlock: indicate whether enable basic block
 * \param [out] dstLocal: output LocalTensor
 * \param [in] srcLocal: input LocalTensor
 * \param [in] gammaLocal: input gamma LocalTensor
 * \param [in] sharedTmpBuffer: input temporary localTensor
 * \param [in] epsilon: epslion value
 * \param [in] tiling: tiling for RmsNorm interface
 */
template <typename T, bool isBasicBlock = false>
__aicore__ inline void RmsNorm(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<T>& gammaLocal,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const T epsilon, const RmsNormTiling& tiling)
{
    if ASCEND_IS_AIC {
        return;
    }
    ASCENDC_ASSERT((IsSameType<T, half>::value || IsSameType<T, float>::value), {
        KERNEL_LOG(KERNEL_ERROR, "RmsNorm only support data type: float/half");
    });
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || \
                              __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    RmsNormAPI::RmsNormImpl<T, isBasicBlock>(dstLocal, srcLocal, gammaLocal, sharedTmpBuffer, epsilon, tiling);
#endif
}

/*!
 * \brief Root Mean Square Layer Normalization, for more info see https://arxiv.org/abs/1910.07467
 *
 * \note support data type: half and float. This interface will allocate a tmp buffer.
 *       user should make sure that memory on UB is big enough for input tiling.
 *
 * \param [in] isBasicBlock: indicate whether enable basic block
 * \param [out] dstLocal: output LocalTensor
 * \param [in] srcLocal: input LocalTensor
 * \param [in] gammaLocal: input gamma LocalTensor
 * \param [in] epsilon: epslion value
 * \param [in] tiling: tiling for RmsNorm interface
 */
template <typename T, bool isBasicBlock = false>
__aicore__ inline void RmsNorm(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<T>& gammaLocal, const T epsilon,
    const RmsNormTiling& tiling)
{
    if ASCEND_IS_AIC {
        return;
    }
    LocalTensor<uint8_t> stackBufer;
    bool ret = PopStackBuffer<uint8_t, TPosition::LCM>(stackBufer);
    ASCENDC_ASSERT((ret), { KERNEL_LOG(KERNEL_ERROR, "RmsNorm failed to apply for tmp buffer!"); });
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || \
                              __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    RmsNorm<T, isBasicBlock>(dstLocal, srcLocal, gammaLocal, stackBufer, epsilon, tiling);
#endif
}
#pragma end_pipe
} // namespace AscendC
#endif // LIB_NORMALIZATION_RMSNORM_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_RMSNORM_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_RMSNORM_H__
#endif
