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
 * \file math_common_util.h
 * \brief defined commonly used math related function.
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/math_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/round.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_MATH_COMMON_IMPL_H__
#endif
#ifndef IMPL_MATH_MATH_COMMON_UTIL_H
#define IMPL_MATH_MATH_COMMON_UTIL_H

#include "../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../include/basic_api/kernel_tensor.h"
#include "../../../basic_api/kernel_pop_stack_buffer.h"

namespace AscendC {
namespace Internal {

template <typename T>
__aicore__ inline void CommonCheckInputsValidness(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
#if ASCENDC_CPU_DEBUG
    ASCENDC_ASSERT(
        ((TPosition)dstTensor.GetPosition() == TPosition::VECIN ||
         (TPosition)dstTensor.GetPosition() == TPosition::VECOUT ||
         (TPosition)dstTensor.GetPosition() == TPosition::VECCALC),
        { KERNEL_LOG(KERNEL_ERROR, "dst position not support, supported positions are UB(VECIN/VECOUT/VECCALC)."); });

    ASCENDC_ASSERT((calCount <= srcTensor.GetSize()), {
        KERNEL_LOG(
            KERNEL_ERROR, "calCount is %u, which should not larger than srcTensor size %u", calCount,
            srcTensor.GetSize());
    });
#endif
}
} // namespace Internal
} // namespace AscendC

#endif // IMPL_MATH_MATH_COMMON_UTIL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_MATH_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_MATH_COMMON_IMPL_H__
#endif
