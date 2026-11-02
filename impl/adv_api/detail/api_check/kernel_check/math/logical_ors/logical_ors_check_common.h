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
 * \file logical_ors_check_common.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/math/logical_ors/logical_ors_check_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/logical_ors.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LOGICAL_ORS_CHECK_COMMON_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_MATH_LOGICAL_ORS_LOGICAL_ORS_CHECK_COMMON_H
#define IMPL_API_CHECK_KERNEL_CHECK_MATH_LOGICAL_ORS_LOGICAL_ORS_CHECK_COMMON_H

#include "../logical_binary_check_common.h"

namespace AscendC {
namespace HighLevelApiCheck {
template <typename T, typename U, typename S, bool isReuseSource>
class CheckFuncClassLogicalOrs : public LogicalBinaryCheckBase<T, U, S, isReuseSource> {
public:
    __aicore__ inline CheckFuncClassLogicalOrs(){};
    __aicore__ inline CheckFuncClassLogicalOrs(__gm__ const char* name)
        : LogicalBinaryCheckBase<T, U, S, isReuseSource>(name){};
};
} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_MATH_LOGICAL_ORS_LOGICAL_ORS_CHECK_COMMON_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LOGICAL_ORS_CHECK_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LOGICAL_ORS_CHECK_COMMON_H__
#endif
