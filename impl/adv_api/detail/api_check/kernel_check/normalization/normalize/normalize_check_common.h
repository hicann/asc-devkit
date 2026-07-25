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
 * \file normalize_check_common.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/normalization/normalize/normalize_check_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/normalization/normalize.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZE_CHECK_COMMON_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_NORMALIZATION_NORMALIZE_NORMALIZE_CHECK_COMMON_H_
#define IMPL_API_CHECK_KERNEL_CHECK_NORMALIZATION_NORMALIZE_NORMALIZE_CHECK_COMMON_H_

#include "../../basic_check/datatype_check.h"
#include "../../basic_check/reuse_source_check.h"
#include "../../basic_check/single_tensor_check.h"
#include "../../basic_check/multiple_tensor_check.h"
#include "../../../../../../../include/adv_api/normalization/normalize_utils.h"

namespace AscendC {
namespace HighLevelApiCheck {
class CheckNormalizeParamsClass {
public:
    template <typename U, typename T, bool isReuseSource, const NormalizeConfig& config>
    __aicore__ inline void CheckNormalizeParams(
        const LocalTensor<T>& output, const LocalTensor<float>& outputRstd, const LocalTensor<float>& inputMean,
        const LocalTensor<float>& inputVariance, const LocalTensor<T>& inputX, const LocalTensor<U>& gamma,
        const LocalTensor<U>& beta, const LocalTensor<uint8_t>& sharedTmpBuffer, const float epsilon,
        const NormalizePara& para)
    {
        VerifyingParameters<U, T, isReuseSource, config>(
            output, outputRstd, inputMean, inputVariance, inputX, gamma, beta, sharedTmpBuffer, epsilon, para);
        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters<U, T, isReuseSource, config>(
                output, outputRstd, inputMean, inputVariance, inputX, gamma, beta, sharedTmpBuffer, epsilon, para);
        }
    }

private:
    template <typename U, typename T, bool isReuseSource, const NormalizeConfig& config>
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& output, const LocalTensor<float>& outputRstd, const LocalTensor<float>& inputMean,
        const LocalTensor<float>& inputVariance, const LocalTensor<T>& inputX, const LocalTensor<U>& gamma,
        const LocalTensor<U>& beta, const LocalTensor<uint8_t>& sharedTmpBuffer, const float epsilon,
        const NormalizePara& para)
    {
        ASCENDC_ASSERT((config.reducePattern == ReducePattern::AR || HighLevelAPIParametersPrint), {
            KERNEL_LOG(KERNEL_ERROR, "[Normalize] The config.reducePattern should be ReducePattern::AR.");
        });
        ASCENDC_ASSERT(((config.aLength == -1 || config.aLength == para.aLength) || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[Normalize] The config.aLength is %u, should be equal to -1 or equal to para.aLength %u.",
                config.aLength, para.aLength);
        });
        ASCENDC_ASSERT((config.isOnlyOutput == false || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[Normalize] The config.isOnlyOutput is %u, should be false.", config.isOnlyOutput);
        });
        ASCENDC_ASSERT((para.aLength * para.rLength <= inputX.GetSize() || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[Normalize] The result of para.aLength * para.rLength is %u, should not be greater inputX size %u.",
                para.aLength * para.rLength, inputX.GetSize());
        });
        ASCENDC_ASSERT((para.rLengthWithPadding * sizeof(T) % ONE_BLK_SIZE == 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[Normalize] The para.rLengthWithPadding is %u, should be 32B aligned.",
                para.rLengthWithPadding);
        });
    }

    template <typename U, typename T, bool isReuseSource, const NormalizeConfig& config>
    __aicore__ inline void PrintParameters(
        const LocalTensor<T>& output, const LocalTensor<float>& outputRstd, const LocalTensor<float>& inputMean,
        const LocalTensor<float>& inputVariance, const LocalTensor<T>& inputX, const LocalTensor<U>& gamma,
        const LocalTensor<U>& beta, const LocalTensor<uint8_t>& sharedTmpBuffer, const float epsilon,
        const NormalizePara& para)
    {
        KERNEL_LOG(
            KERNEL_INFO, "[Normalize] The config.reducePattern is %d, config.aLength is %u, config.isOnlyOutput is %d.",
            config.reducePattern, config.aLength, config.isOnlyOutput);
        KERNEL_LOG(
            KERNEL_INFO, "[Normalize] The para.aLength is %u, para.rLength is %u, para.rLengthWithPadding is %u.",
            para.aLength, para.rLength, para.rLengthWithPadding);
    }
};

template <typename U, typename T, bool isReuseSource, const NormalizeConfig& config>
class CheckFuncClassNormalize : public DataTypeCheckFuncBasicClass,
                                public ReuseSourceCheckFuncBasicClass,
                                public SingleTensorCheckFuncBasicClass,
                                public MultipleTensorCheckFuncBasicClass,
                                public CheckNormalizeParamsClass {
public:
    __aicore__ inline CheckFuncClassNormalize(){};
    __aicore__ inline CheckFuncClassNormalize(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          ReuseSourceCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& output, const LocalTensor<float>& outputRstd, const LocalTensor<float>& inputMean,
        const LocalTensor<float>& inputVariance, const LocalTensor<T>& inputX, const LocalTensor<U>& gamma,
        const LocalTensor<U>& beta, const LocalTensor<uint8_t>& sharedTmpBuffer, const float epsilon,
        const NormalizePara& para)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<U, half, float>(
            "first template parameter (U) is not half or float");
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, half, float>(
            "second template parameter (T) is not half or float");

        ReuseSourceCheckFuncBasicClass::IsReuseSourceVerifyingParameters<false>(ARG_AND_STRING(isReuseSource));

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(output, outputRstd, inputMean, inputVariance, inputX, gamma, beta, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(output, outputRstd, inputMean));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(output, outputRstd, inputVariance));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(output, outputRstd, inputX));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(output, outputRstd, gamma));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(output, outputRstd, beta));

        CheckNormalizeParamsClass::CheckNormalizeParams<U, T, isReuseSource, config>(
            output, outputRstd, inputMean, inputVariance, inputX, gamma, beta, sharedTmpBuffer, epsilon, para);
    };
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_NORMALIZATION_NORMALIZE_NORMALIZE_CHECK_COMMON_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZE_CHECK_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZE_CHECK_COMMON_H__
#endif
