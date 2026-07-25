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
 * \file math_common_check.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/math/math_common_check.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_COMMON_CHECK_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_MATH_MATH_COMMON_CHECK_H_
#define IMPL_API_CHECK_KERNEL_CHECK_MATH_MATH_COMMON_CHECK_H_

#include "../basic_check/datatype_check.h"
#include "../basic_check/calcount_check.h"
#include "../basic_check/reuse_source_check.h"
#include "../basic_check/single_tensor_check.h"
#include "../basic_check/multiple_tensor_check.h"

namespace AscendC {
namespace HighLevelApiCheck {

class CheckFuncClassMathCommon : public DataTypeCheckFuncBasicClass,
                                 public CalCountCheckFuncBasicClass,
                                 public ReuseSourceCheckFuncBasicClass,
                                 public SingleTensorCheckFuncBasicClass,
                                 public MultipleTensorCheckFuncBasicClass {
public:
    __aicore__ inline CheckFuncClassMathCommon(){};
    __aicore__ inline CheckFuncClassMathCommon(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          CalCountCheckFuncBasicClass(apiName),
          ReuseSourceCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    template <typename T, bool isReuseSource>
    __aicore__ inline void CommonCheck(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
        const uint32_t calCount)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, half, float>(
            "template parameter (T) is not half or float");

        ReuseSourceCheckFuncBasicClass::IsReuseSourceVerifyingParameters<false>(ARG_AND_STRING(isReuseSource));

        CalCountCheckFuncBasicClass::CalCountVerifyingParameters(
            ARG_AND_STRING(calCount), VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor));

        SingleTensorCheckFuncBasicClass::TPositionVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        SingleTensorCheckFuncBasicClass::TensorSizeVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor, sharedTmpBuffer));

        SingleTensorCheckFuncBasicClass::TensorPhyAddrVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor, sharedTmpBuffer));
    };
    template <typename T, bool isReuseSource>
    __aicore__ inline void CommonVerifyingParameters(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
        const uint32_t calCount)
    {
        CommonCheck<T, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, calCount);

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor, sharedTmpBuffer));
    };
    template <typename T, bool isReuseSource>
    __aicore__ inline void CommonVerifyNoTmpBufReuse(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
        const uint32_t calCount)
    {
        CommonCheck<T, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, calCount);

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor));
    };
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_MATH_MATH_COMMON_CHECK_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_COMMON_CHECK_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_COMMON_CHECK_H__
#endif
