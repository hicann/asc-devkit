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
#error "reduce.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_H__
#endif

#ifndef LIB_REDUCE_REDUCE_H
#define LIB_REDUCE_REDUCE_H
#include "reduce_common.h"
#include "kernel_tensor.h"
#include "kernel_basic_intf.h"
#include "../../../impl/basic_api/kernel_pop_stack_buffer.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "../../../impl/adv_api/detail/reduce/reduce_prod/reduce_prod_v220_impl.h"
#include "../../../impl/adv_api/detail/reduce/reduce_max/reduce_max_v220_impl.h"
#include "../../../impl/adv_api/detail/reduce/reduce_min/reduce_min_v220_impl.h"
#include "../../../impl/adv_api/detail/reduce/reduce_sum/reduce_sum_v220_impl.h"
#include "../../../impl/adv_api/detail/reduce/reduce_mean/reduce_mean_v220_impl.h"
#include "../../../impl/adv_api/detail/reduce/reduce_any/reduce_any_v220_impl.h"
#include "../../../impl/adv_api/detail/reduce/reduce_all/reduce_all_v220_impl.h"
#elif defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/reduce/reduce_max/reduce_max_3510_impl.h"
#include "../../../impl/adv_api/detail/reduce/reduce_min/reduce_min_3510_impl.h"
#include "../../../impl/adv_api/detail/reduce/reduce_sum/reduce_sum_3510_impl.h"
#include "../../../impl/adv_api/detail/reduce/reduce_mean/reduce_mean_3510_impl.h"
#include "../../../impl/adv_api/detail/reduce/reduce_any/reduce_any_3510_impl.h"
#include "../../../impl/adv_api/detail/reduce/reduce_all/reduce_all_3510_impl.h"
#include "../../../impl/adv_api/detail/reduce/reduce_prod/reduce_prod_3510_impl.h"
#endif
#if ASCENDC_CPU_DEBUG
#include "../../../impl/basic_api/kernel_log.h"
#include <type_traits>
#endif

namespace AscendC {
#pragma begin_pipe(V)
/* !
 * \brief This function calculates the mean of all elements in the input tensor based on the reduction pattern.
 *
 * \note support data type: float
 *
 * \tparam [in] T, data type
 * \tparam [in] pattern, reduce pattern of input tensor, each character is a dimension
 * \tparam [in] isReuseSource, whether allows API to modify source data, usually for performance reason
 * \param [in] srcTensor, input LocalTensor
 * \param [in] sharedTmpBuffer, input local temporary Tensor
 * \param [in] srcShape, actual shape used to reduce information of input tensor, its dim must be matched with pattern
 * \param [in] srcInnerPad, whether the last axis of input tensor is padded to 32B aligned up.
 * \param [out] dstTensor, output LocalTensor
 */
template <class T, class pattern, bool isReuseSource = false>
__aicore__ inline void ReduceMean(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t srcShape[], bool srcInnerPad)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    Internal::ReduceMeanImpl<T, pattern, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, srcShape, srcInnerPad);
#endif
}

/* !
 * \brief This function calculates the mean of all elements in the input tensor based on the reduction pattern.
 *
 * \note support data type: float
 *
 * \tparam [in] T, data type
 * \tparam [in] pattern, reduce pattern of input tensor, each character is a dimension
 * \tparam [in] isReuseSource, whether allows API to modify source data, usually for performance reason
 * \param [in] srcTensor, input LocalTensor
 * \param [in] srcShape, actual shape used to reduce information of input tensor, its dim must be matched with pattern
 * \param [in] srcInnerPad, whether the last axis of input tensor is padded to 32B aligned up.
 * \param [out] dstTensor, output LocalTensor
 */
template <class T, class pattern, bool isReuseSource = false>
__aicore__ inline void ReduceMean(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t srcShape[], bool srcInnerPad)
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    ReduceMean<T, pattern, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, srcShape, srcInnerPad);
#endif
}

/* !
 * \brief This function calculates the max value of all elements in the input tensor based on the reduction pattern.
 *
 * \note support data type: float/half
 *
 * \tparam [in] T, data type
 * \tparam [in] pattern, reduce pattern of input tensor, each character is a dimension
 * \tparam [in] isReuseSource, whether allows API to modify source data, usually for performance reason
 * \param [in] srcTensor, input LocalTensor
 * \param [in] sharedTmpBuffer, input local temporary Tensor
 * \param [in] srcShape, actual shape used to reduce information of input tensor, its dim must be matched with pattern
 * \param [in] srcInnerPad, whether the last axis of input tensor is padded to 32B aligned up.
 * \param [out] dstTensor, output LocalTensor
 */
template <class T, class pattern, bool isReuseSource = false>
__aicore__ inline void ReduceMax(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t srcShape[], bool srcInnerPad)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    Internal::ReduceMaxImpl<T, pattern, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, srcShape, srcInnerPad);
#endif
}

/* !
 * \brief This function calculates the max value of all elements in the input tensor based on the reduction pattern.
 *
 * \note support data type: float/half
 *
 * \tparam [in] T, data type
 * \tparam [in] pattern, reduce pattern of input tensor, each character is a dimension
 * \tparam [in] isReuseSource, whether allows API to modify source data, usually for performance reason
 * \param [in] srcTensor, input LocalTensor
 * \param [in] srcShape, actual shape used to reduce information of input tensor, its dim must be matched with pattern
 * \param [in] srcInnerPad, whether the last axis of input tensor is padded to 32B aligned up.
 * \param [out] dstTensor, output LocalTensor
 */
template <class T, class pattern, bool isReuseSource = false>
__aicore__ inline void ReduceMax(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t srcShape[], bool srcInnerPad)
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    ReduceMax<T, pattern, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, srcShape, srcInnerPad);
#endif
}

/* !
 * \brief This function calculates the min value of all elements in the input tensor based on the reduction pattern.
 *
 * \note support data type: float/half
 *
 * \tparam [in] T, data type
 * \tparam [in] pattern, reduce pattern of input tensor, each character is a dimension
 * \tparam [in] isReuseSource, whether allows API to modify source data, usually for performance reason
 * \param [in] srcTensor, input LocalTensor
 * \param [in] sharedTmpBuffer, input local temporary Tensor
 * \param [in] srcShape, actual shape used to reduce information of input tensor, its dim must be matched with pattern
 * \param [in] srcInnerPad, whether the last axis of input tensor is padded to 32B aligned up.
 * \param [out] dstTensor, output LocalTensor
 */
template <class T, class pattern, bool isReuseSource = false>
__aicore__ inline void ReduceMin(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t srcShape[], bool srcInnerPad)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    Internal::ReduceMinImpl<T, pattern, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, srcShape, srcInnerPad);
#endif
}

/* !
 * \brief This function calculates the min value of all elements in the input tensor based on the reduction pattern.
 *
 * \note support data type: float/half
 *
 * \tparam [in] T, data type
 * \tparam [in] pattern, reduce pattern of input tensor, each character is a dimension
 * \tparam [in] isReuseSource, whether allows API to modify source data, usually for performance reason
 * \param [in] srcTensor, input LocalTensor
 * \param [in] srcShape, actual shape used to reduce information of input tensor, its dim must be matched with pattern
 * \param [in] srcInnerPad, whether the last axis of input tensor is padded to 32B aligned up.
 * \param [out] dstTensor, output LocalTensor
 */
template <class T, class pattern, bool isReuseSource = false>
__aicore__ inline void ReduceMin(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t srcShape[], bool srcInnerPad)
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    ReduceMin<T, pattern, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, srcShape, srcInnerPad);
#endif
}

/* !
 * \brief This function calculates the sum of all elements in the input tensor based on the reduction pattern.
 *
 * \note support data type: float
 *
 * \tparam [in] T, data type
 * \tparam [in] pattern, reduce pattern of input tensor, each character is a dimension
 * \tparam [in] isReuseSource, whether allows API to modify source data, usually for performance reason
 * \param [in] srcTensor, input LocalTensor
 * \param [in] sharedTmpBuffer, input local temporary Tensor
 * \param [in] srcShape, actual shape used to reduce information of input tensor, its dim must be matched with pattern
 * \param [in] srcInnerPad, whether the last axis of input tensor is padded to 32B aligned up.
 * \param [out] dstTensor, output LocalTensor
 */
template <class T, class pattern, bool isReuseSource = false>
__aicore__ inline void ReduceSum(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t srcShape[], bool srcInnerPad)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    Internal::ReduceSumImpl<T, pattern, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, srcShape, srcInnerPad);
#endif
}

/* !
 * \brief This function calculates the sum of all elements in the input tensor based on the reduction pattern.
 *
 * \note support data type: float
 *
 * \tparam [in] T, data type
 * \tparam [in] pattern, reduce pattern of input tensor, each character is a dimension
 * \tparam [in] isReuseSource, whether allows API to modify source data, usually for performance reason
 * \param [in] srcTensor, input LocalTensor
 * \param [in] srcShape, actual shape used to reduce information of input tensor, its dim must be matched with pattern
 * \param [in] srcInnerPad, whether the last axis of input tensor is padded to 32B aligned up.
 * \param [out] dstTensor, output LocalTensor
 */
template <class T, class pattern, bool isReuseSource = false>
__aicore__ inline void ReduceSum(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t srcShape[], bool srcInnerPad)
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    ReduceSum<T, pattern, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, srcShape, srcInnerPad);
#endif
}

/* !
 * \brief This function calculates the product of all elements in the input tensor based on the reduction pattern.
 *
 * \note support data type: float
 *
 * \tparam [in] T, data type
 * \tparam [in] pattern, reduce pattern of input tensor, each character is a dimension
 * \tparam [in] isReuseSource, whether allows API to modify source data, usually for performance reason
 * \param [in] srcTensor, input LocalTensor
 * \param [in] sharedTmpBuffer, input local temporary Tensor
 * \param [in] srcShape, actual shape used to reduce information of input tensor, its dim must be matched with pattern
 * \param [in] srcInnerPad, whether the last axis of input tensor is padded to 32B aligned up.
 * \param [out] dstTensor, output LocalTensor
 */
template <class T, class pattern, bool isReuseSource = false>
__aicore__ inline void ReduceProd(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t srcShape[], bool srcInnerPad)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    Internal::ReduceProdImpl<T, pattern, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, srcShape, srcInnerPad);
#endif
}

/* !
 * \brief This function calculates the product of all elements in the input tensor based on the reduction pattern.
 *
 * \note support data type: float
 *
 * \tparam [in] T, data type
 * \tparam [in] pattern, reduce pattern of input tensor, each character is a dimension
 * \tparam [in] isReuseSource, whether allows API to modify source data, usually for performance reason
 * \param [in] srcTensor, input LocalTensor
 * \param [in] srcShape, actual shape used to reduce information of input tensor, its dim must be matched with pattern
 * \param [in] srcInnerPad, whether the last axis of input tensor is padded to 32B aligned up.
 * \param [out] dstTensor, output LocalTensor
 */
template <class T, class pattern, bool isReuseSource = false>
__aicore__ inline void ReduceProd(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t srcShape[], bool srcInnerPad)
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    ReduceProd<T, pattern, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, srcShape, srcInnerPad);
#endif
}

/* !
 * \brief This function calculates any true value of all elements in the input tensor based on the reduction pattern.
 *
 * \note support data type: float/uint8_t
 *
 * \tparam [in] T, data type
 * \tparam [in] pattern, reduce pattern of input tensor, each character is a dimension
 * \tparam [in] isReuseSource, whether allows API to modify source data, usually for performance reason
 * \param [in] srcTensor, input LocalTensor
 * \param [in] sharedTmpBuffer, input local temporary Tensor
 * \param [in] srcShape, actual shape used to reduce information of input tensor, its dim must be matched with pattern
 * \param [in] srcInnerPad, whether the last axis of input tensor is padded to 32B aligned up.
 * \param [out] dstTensor, output LocalTensor
 */
template <class T, class pattern, bool isReuseSource = false>
__aicore__ inline void ReduceAny(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t srcShape[], bool srcInnerPad)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    Internal::ReduceAnyImpl<T, pattern, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, srcShape, srcInnerPad);
#endif
}

/* !
 * \brief This function calculates any true value of all elements in the input tensor based on the reduction pattern.
 *
 * \note support data type: float
 *
 * \tparam [in] T, data type
 * \tparam [in] pattern, reduce pattern of input tensor, each character is a dimension
 * \tparam [in] isReuseSource, whether allows API to modify source data, usually for performance reason
 * \param [in] srcTensor, input LocalTensor
 * \param [in] srcShape, actual shape used to reduce information of input tensor, its dim must be matched with pattern
 * \param [in] srcInnerPad, whether the last axis of input tensor is padded to 32B aligned up.
 * \param [out] dstTensor, output LocalTensor
 */
template <class T, class pattern, bool isReuseSource = false>
__aicore__ inline void ReduceAny(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t srcShape[], bool srcInnerPad)
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    ReduceAny<T, pattern, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, srcShape, srcInnerPad);
#endif
}

/* !
 * \brief This function calculates all true value of all elements in the input tensor based on the reduction pattern.
 *
 * \note support data type: float/uint8_t
 *
 * \tparam [in] T, data type
 * \tparam [in] pattern, reduce pattern of input tensor, each character is a dimension
 * \tparam [in] isReuseSource, whether allows API to modify source data, usually for performance reason
 * \param [in] srcTensor, input LocalTensor
 * \param [in] sharedTmpBuffer, input local temporary Tensor
 * \param [in] srcShape, actual shape used to reduce information of input tensor, its dim must be matched with pattern
 * \param [in] srcInnerPad, whether the last axis of input tensor is padded to 32B aligned up.
 * \param [out] dstTensor, output LocalTensor
 */
template <class T, class pattern, bool isReuseSource = false>
__aicore__ inline void ReduceAll(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t srcShape[], bool srcInnerPad)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    Internal::ReduceAllImpl<T, pattern, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, srcShape, srcInnerPad);
#endif
}

/* !
 * \brief This function calculates all true value of all elements in the input tensor based on the reduction pattern.
 *
 * \note support data type: float/uint8_t
 *
 * \tparam [in] T, data type
 * \tparam [in] pattern, reduce pattern of input tensor, each character is a dimension
 * \tparam [in] isReuseSource, whether allows API to modify source data, usually for performance reason
 * \param [in] srcTensor, input LocalTensor
 * \param [in] srcShape, actual shape used to reduce information of input tensor, its dim must be matched with pattern
 * \param [in] srcInnerPad, whether the last axis of input tensor is padded to 32B aligned up.
 * \param [out] dstTensor, output LocalTensor
 */
template <class T, class pattern, bool isReuseSource = false>
__aicore__ inline void ReduceAll(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t srcShape[], bool srcInnerPad)
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    ReduceAll<T, pattern, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, srcShape, srcInnerPad);
#endif
}

#pragma end_pipe
} // namespace AscendC
#endif // LIB_REDUCE_REDUCE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_H__
#endif
