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
 * \file sigmoid_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/activation/sigmoid/sigmoid_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/sigmoid.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SIGMOID_COMMON_IMPL_H__
#endif

#ifndef IMPL_ACTIVATION_SIGMOID_SIGMOID_COMMON_IMPL_H
#define IMPL_ACTIVATION_SIGMOID_SIGMOID_COMMON_IMPL_H

#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../basic_api/kernel_pop_stack_buffer.h"
#include "kernel_tiling/kernel_tiling.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/activation/sigmoid/sigmoid_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

// V100 does not support counter mode. Thus different implementation is needed.
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
#include "sigmoid_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 1001
#include "sigmoid_v100_impl.h"
#elif defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102 || \
                                __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "sigmoid_3510_impl.h"
#endif

namespace AscendC {

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 1001)
template <typename T, bool isReuseSource = false>
__aicore__ inline void SigmoidImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t calCount)
{
    CHECK_FUNC_HIGHLEVEL_API(Sigmoid, (T, isReuseSource), (dstTensor, srcTensor, sharedTmpBuffer, calCount));
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    ASSERT(
        ((TPosition)dstTensor.GetPosition() == TPosition::VECIN ||
         (TPosition)dstTensor.GetPosition() == TPosition::VECOUT ||
         (TPosition)dstTensor.GetPosition() == TPosition::VECCALC));
    ASCENDC_ASSERT((calCount <= srcTensor.GetSize()), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "calCount is %u, which should not "
            "larger than srcTensor size %u",
            calCount, srcTensor.GetSize());
    });

    uint32_t splitSize = sharedTmpBuffer.GetSize() / sizeof(T);
    ASCENDC_ASSERT((splitSize != 0), { KERNEL_LOG(KERNEL_ERROR, "splitSize should not be 0!"); });

    uint32_t loopCount = calCount / splitSize;
    uint32_t calcTail = calCount % splitSize;
    LocalTensor<T> tmpBuffer = sharedTmpBuffer.ReinterpretCast<T>();
    SigmoidCompute<T, isReuseSource>(dstTensor, srcTensor, tmpBuffer, splitSize, loopCount, calcTail);
}
#endif

template <typename T, bool isReuseSource = false>
__aicore__ inline void SigmoidImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    LocalTensor<uint8_t> stackBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(stackBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    SigmoidImpl<T, isReuseSource>(dstTensor, srcTensor, stackBuffer, calCount);
}
} // namespace AscendC
#endif // IMPL_ACTIVATION_SIGMOID_SIGMOID_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SIGMOID_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SIGMOID_COMMON_IMPL_H__
#endif
