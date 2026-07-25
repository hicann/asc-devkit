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
 * \file arithprogression_check_common.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/index/arithprogression/arithprogression_check_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/index/arithprogression.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ARITHPROGRESSION_CHECK_COMMON_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_INDEX_ARITHPROGRESSION_ARITHPROGRESSION_CHECK_COMMON_H_
#define IMPL_API_CHECK_KERNEL_CHECK_INDEX_ARITHPROGRESSION_ARITHPROGRESSION_CHECK_COMMON_H_

#include "../../basic_check/datatype_check.h"
#include "../../basic_check/calcount_check.h"
#include "../../basic_check/single_tensor_check.h"
#include "../../basic_check/multiple_tensor_check.h"

namespace AscendC {
namespace HighLevelApiCheck {
class CheckArithProgressionParamsClass {
public:
    template <typename T>
    __aicore__ inline void CheckArithProgressionParams(
        const LocalTensor<T>& dstLocal, const T firstValue, const T diffValue, const int32_t count)
    {
        VerifyingParameters<T>(dstLocal, firstValue, diffValue, count);
        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters<T>(dstLocal, firstValue, diffValue, count);
        }
    }

private:
    template <typename T>
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstLocal, const T firstValue, const T diffValue, const int32_t count)
    {
        ASCENDC_ASSERT((count > 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(KERNEL_ERROR, "[Arange] The count parameter cannot be %d, should be greater than 0.", count);
        });
        ASCENDC_ASSERT((static_cast<float>(diffValue) >= static_cast<float>(0) || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[Arange] The diffValue parameter cannot be %f, should be greater than or equal to 0.",
                static_cast<float>(diffValue));
        });
    }

    template <typename T>
    __aicore__ inline void PrintParameters(
        const LocalTensor<T>& dstLocal, const T firstValue, const T diffValue, const int32_t count)
    {
        KERNEL_LOG(
            KERNEL_INFO, "[Arange] The diffValue parameter is %f, firstValue is %f.", static_cast<float>(diffValue),
            static_cast<float>(firstValue));
    }
};

template <typename T>
class CheckFuncClassArithProgression : public DataTypeCheckFuncBasicClass,
                                       public CalCountCheckFuncBasicClass,
                                       public SingleTensorCheckFuncBasicClass,
                                       public MultipleTensorCheckFuncBasicClass,
                                       public CheckArithProgressionParamsClass {
public:
    __aicore__ inline CheckFuncClassArithProgression(){};
    __aicore__ inline CheckFuncClassArithProgression(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          CalCountCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstLocal, const T firstValue, const T diffValue, const int32_t count)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, half, float, int16_t, int32_t>(
            "template parameter (T) is not half/float/int16_t/int32_t");

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstLocal),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        CheckArithProgressionParamsClass::CheckArithProgressionParams<T>(dstLocal, firstValue, diffValue, count);

        CalCountCheckFuncBasicClass::CalCountVerifyingParameters(
            ARG_AND_STRING(count), VA_ARGS_TO_MAKE_TUPLE(dstLocal));
    };
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_INDEX_ARITHPROGRESSION_ARITHPROGRESSION_CHECK_COMMON_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ARITHPROGRESSION_CHECK_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ARITHPROGRESSION_CHECK_COMMON_H__
#endif
