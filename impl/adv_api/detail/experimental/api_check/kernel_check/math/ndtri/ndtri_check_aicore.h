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
 * \file ndtri_check_aicore.h
 * \brief Ndtri high-level API parameter checks for AI Core.
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NDTRI_CHECK_AICORE_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_MATH_NDTRI_NDTRI_CHECK_AICORE_H_
#define IMPL_API_CHECK_KERNEL_CHECK_MATH_NDTRI_NDTRI_CHECK_AICORE_H_

namespace AscendC {
namespace experimental {
namespace HighLevelApiCheck {

template <typename T, bool isReuseSource = false>
class CheckFuncClassNdtri {
public:
    __aicore__ inline CheckFuncClassNdtri(){};
    __aicore__ inline CheckFuncClassNdtri(__gm__ const char* apiName){};

    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount) {};
};

} // namespace HighLevelApiCheck
} // namespace experimental
} // namespace AscendC

#endif // IMPL_API_CHECK_KERNEL_CHECK_MATH_NDTRI_NDTRI_CHECK_AICORE_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NDTRI_CHECK_AICORE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NDTRI_CHECK_AICORE_H__
#endif
