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
 * \file bitwise_and_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/bitwise_and/bitwise_and_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/bitwise_and.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_BITWISE_AND_BITWISE_AND_COMMON_IMPL_H__
#endif
#ifndef LIB_MATH_BITWISE_AND_IMPL_H
#define LIB_MATH_BITWISE_AND_IMPL_H
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/adv_api/math/bitwise_and_utils.h"
#include "../bitwise_template/bitwise_template.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/math/bitwise_and/bitwise_and_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"
namespace AscendC {
template <const BitwiseAndConfig& config = DEFAULT_BITWISE_AND_CONFIG, typename T>
__aicore__ inline void BitwiseAndImpl(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const uint32_t count)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    CHECK_FUNC_HIGHLEVEL_API(BitwiseAnd, (T, config.isReuseSource), (dst, src0, src1, count));

    if constexpr (sizeof(T) == 8) {
        BitwiseTemplateImpl<Reg::And<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T, Reg::RegTraitNumTwo>>, T>(
            dst, src0, src1, count);
    } else {
        BitwiseTemplateImpl<Reg::And<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T, Reg::RegTraitNumOne>>, T>(
            dst, src0, src1, count);
    }
}

} // namespace AscendC
#endif
#endif // IMPL_MATH_BITWISE_AND_BITWISE_AND_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_BITWISE_AND_BITWISE_AND_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_BITWISE_AND_BITWISE_AND_COMMON_IMPL_H__
#endif
