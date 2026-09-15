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
 * \file cumsum_check.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/math/cumsum/cumsum_check.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/cumsum.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CUMSUM_CHECK_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_MATH_CUMSUM_CHECK_H_
#define IMPL_API_CHECK_KERNEL_CHECK_MATH_CUMSUM_CHECK_H_

#include "../../../../../../../include/adv_api/math/cumsum_utils.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
#include "cumsum_check_common.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "cumsum_check_c310.h"
#else
#include "cumsum_check_aicore.h"
#endif

namespace AscendC {
namespace HighLevelApiCheck {

template <typename T, const CumSumConfig& config>
__aicore__ inline void CheckFuncCumSum(
    __gm__ const char* apiName, const LocalTensor<T>& dstTensor, const LocalTensor<T>& lastRowTensor,
    const LocalTensor<T>& srcTensor, LocalTensor<uint8_t>& sharedTmpBuffer, const CumSumInfo& cumSumInfo)
{
    CheckFuncClassCumSum<T, config> checkFun(apiName);
    checkFun.VerifyingParameters(dstTensor, lastRowTensor, srcTensor, sharedTmpBuffer, cumSumInfo);
}

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_MATH_CUMSUM_CHECK_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CUMSUM_CHECK_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CUMSUM_CHECK_H__
#endif
