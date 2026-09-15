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
 * \file where_check.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/math/where/where_check.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/where.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_WHERE_CHECK_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_MATH_WHERE_WHERE_CHECK_H_
#define IMPL_API_CHECK_KERNEL_CHECK_MATH_WHERE_WHERE_CHECK_H_

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "where_check_aicore.h"
#endif

namespace AscendC {
namespace HighLevelApiCheck {
template <typename T, typename U, typename S, typename V>
__aicore__ inline void CheckFuncWhere(
    __gm__ const char* name, const LocalTensor<T>& dst, const U& src0, const S& src1, const LocalTensor<V>& condition,
    const uint32_t count)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    CheckFuncClassWhere<T, U, S, V> checkFun(name);
    checkFun.VerifyingParameters(dst, src0, src1, condition, count);
#endif
}

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_MATH_WHERE_WHERE_CHECK_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_WHERE_CHECK_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_WHERE_CHECK_H__
#endif
