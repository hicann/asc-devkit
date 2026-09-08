/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file ndtri_check.h
 * \brief Ndtri high-level API parameter check declaration.
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NDTRI_CHECK_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_MATH_NDTRI_NDTRI_CHECK_H_
#define IMPL_API_CHECK_KERNEL_CHECK_MATH_NDTRI_NDTRI_CHECK_H_

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510)
#include "ndtri_check_common.h"
#else
#include "ndtri_check_aicore.h"
#endif

namespace AscendC {
namespace experimental {
namespace HighLevelApiCheck {

template <typename T, bool isReuseSource = false>
__aicore__ inline void CheckFuncNdtri(
    __gm__ const char* apiName, const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
    const uint32_t calCount)
{
    CheckFuncClassNdtri<T, isReuseSource> checkFunc(apiName);
    checkFunc.VerifyingParameters(dstTensor, srcTensor, calCount);
}

} // namespace HighLevelApiCheck
} // namespace experimental
} // namespace AscendC

#endif // IMPL_API_CHECK_KERNEL_CHECK_MATH_NDTRI_NDTRI_CHECK_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NDTRI_CHECK_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NDTRI_CHECK_H__
#endif
