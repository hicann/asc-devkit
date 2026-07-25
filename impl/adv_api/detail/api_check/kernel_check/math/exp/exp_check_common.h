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
 * \file exp_check_common.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/math/exp/exp_check_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/exp.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_EXP_CHECK_COMMON_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_MATH_EXP_EXP_CHECK_COMMON_H_
#define IMPL_API_CHECK_KERNEL_CHECK_MATH_EXP_EXP_CHECK_COMMON_H_

#include "../../basic_check/datatype_check.h"
#include "../../basic_check/calcount_check.h"
#include "../../basic_check/reuse_source_check.h"
#include "../../basic_check/single_tensor_check.h"
#include "../../basic_check/multiple_tensor_check.h"

namespace AscendC {
namespace HighLevelApiCheck {
template <typename T, uint8_t taylorExpandLevel, bool isReuseSource>
class CheckFuncClassExp : public DataTypeCheckFuncBasicClass,
                          public CalCountCheckFuncBasicClass,
                          public ReuseSourceCheckFuncBasicClass,
                          public SingleTensorCheckFuncBasicClass,
                          public MultipleTensorCheckFuncBasicClass {
public:
    __aicore__ inline CheckFuncClassExp(){};
    __aicore__ inline CheckFuncClassExp(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          CalCountCheckFuncBasicClass(apiName),
          ReuseSourceCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& sharedTmpBuffer,
        const uint32_t calCount)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, half, float>(
            "template parameter (T) is not half or float");

        if (std::is_same<T, half>::value) {
            ReuseSourceCheckFuncBasicClass::IsReuseSourceVerifyingParameters<false>(ARG_AND_STRING(isReuseSource));
        }

        CalCountCheckFuncBasicClass::CalCountVerifyingParameters(
            ARG_AND_STRING(calCount), VA_ARGS_TO_MAKE_TUPLE(dstLocal, srcLocal));

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstLocal, srcLocal, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstLocal, srcLocal, sharedTmpBuffer));
    };
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_MATH_EXP_EXP_CHECK_COMMON_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_EXP_CHECK_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_EXP_CHECK_COMMON_H__
#endif
