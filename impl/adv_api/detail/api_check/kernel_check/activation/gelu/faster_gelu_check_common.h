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
 * \file faster_gelu_check_common.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/activation/gelu/faster_gelu_check_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/fastergelu.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_FASTER_GELU_CHECK_COMMON_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_GELU_FASTER_GELU_CHECK_COMMON_H_
#define IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_GELU_FASTER_GELU_CHECK_COMMON_H_

#include "../../basic_check/datatype_check.h"
#include "../../basic_check/calcount_check.h"
#include "../../basic_check/single_tensor_check.h"
#include "../../basic_check/multiple_tensor_check.h"

namespace AscendC {
namespace HighLevelApiCheck {
class CheckFasterGeluParamsClass {
public:
    template <typename T, bool highPrecision = false, bool highPerformance = false>
    __aicore__ inline void CheckFasterGeluParams(
        const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& sharedTmpBuffer,
        const uint32_t dataSize)
    {
        VerifyingParameters<T, highPrecision, highPerformance>(dstLocal, srcLocal, sharedTmpBuffer, dataSize);
        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters<T, highPrecision, highPerformance>(dstLocal, srcLocal, sharedTmpBuffer, dataSize);
        }
    }

private:
    template <typename T, bool highPrecision = false, bool highPerformance = false>
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& sharedTmpBuffer,
        const uint32_t dataSize)
    {
        if (std::is_same<T, float>::value) {
            ASCENDC_ASSERT((highPrecision == false || HighLevelAPIParametersPrint), {
                KERNEL_LOG(KERNEL_WARN, "[FasterGelu] The highPrecision is true, may not be effective.");
            });
        }
    }

    template <typename T, bool highPrecision = false, bool highPerformance = false>
    __aicore__ inline void PrintParameters(
        const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& sharedTmpBuffer,
        const uint32_t dataSize)
    {
        KERNEL_LOG(
            KERNEL_INFO, "[FasterGelu] The highPrecision is %d, highPerformance is %d.", highPrecision,
            highPerformance);
    }
};

template <typename T, bool highPrecision = false, bool highPerformance = false>
class CheckFuncClassFasterGelu : public DataTypeCheckFuncBasicClass,
                                 public CalCountCheckFuncBasicClass,
                                 public SingleTensorCheckFuncBasicClass,
                                 public MultipleTensorCheckFuncBasicClass,
                                 public CheckFasterGeluParamsClass {
public:
    __aicore__ inline CheckFuncClassFasterGelu(){};
    __aicore__ inline CheckFuncClassFasterGelu(__gm__ const char* apiName)
        : MultipleTensorCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          DataTypeCheckFuncBasicClass(apiName),
          CalCountCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& sharedTmpBuffer,
        const uint32_t dataSize)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, half, float>(
            "template parameter (T) is not half or float");

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstLocal, sharedTmpBuffer, srcLocal),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");
        CalCountCheckFuncBasicClass::CalCountVerifyingParameters(
            ARG_AND_STRING(dataSize), VA_ARGS_TO_MAKE_TUPLE(dstLocal, srcLocal));
        CheckFasterGeluParamsClass::CheckFasterGeluParams<T, highPrecision, highPerformance>(
            dstLocal, srcLocal, sharedTmpBuffer, dataSize);
    };
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_GELU_FASTER_GELU_CHECK_COMMON_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_FASTER_GELU_CHECK_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_FASTER_GELU_CHECK_COMMON_H__
#endif
