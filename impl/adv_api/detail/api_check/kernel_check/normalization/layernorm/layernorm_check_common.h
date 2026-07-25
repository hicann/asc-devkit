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
 * \file layernorm_check_common.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/normalization/layernorm/layernorm_check_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/normalization/layernorm.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LAYERNORM_CHECK_COMMON_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_NORMALIZATION_LAYERNORM_LAYERNORM_CHECK_COMMON_H_
#define IMPL_API_CHECK_KERNEL_CHECK_NORMALIZATION_LAYERNORM_LAYERNORM_CHECK_COMMON_H_

#include "../../basic_check/datatype_check.h"
#include "../../basic_check/reuse_source_check.h"
#include "../../basic_check/single_tensor_check.h"
#include "../../basic_check/multiple_tensor_check.h"
#include "../../../../../../../include/adv_api/normalization/layernorm_utils.h"
#include "../../../../normalization/layernorm/layernorm_normal_config.h"

namespace AscendC {
namespace HighLevelApiCheck {
class CheckLayerNormParamsClass {
public:
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void CheckLayerNormParams(
        const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance,
        const LocalTensor<T>& inputX, const LocalTensor<T>& gamma, const LocalTensor<T>& beta,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const T epsilon, LayerNormTiling& tiling)
    {
        VerifyingParameters<T, isReuseSource>(
            output, outputMean, outputVariance, inputX, gamma, beta, sharedTmpBuffer, epsilon, tiling);
    }

private:
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance,
        const LocalTensor<T>& inputX, const LocalTensor<T>& gamma, const LocalTensor<T>& beta,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const T epsilon, LayerNormTiling& tiling)
    {
        ASCENDC_ASSERT(((gamma.GetSize() * sizeof(T)) % ONE_BLK_SIZE == 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[LayerNorm] The gamma length is %lu, should be 32B aligned.",
                gamma.GetSize() * sizeof(T));
        });
        ASCENDC_ASSERT(((beta.GetSize() * sizeof(T)) % ONE_BLK_SIZE == 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[LayerNorm] The beta length is %lu, should be 32B aligned.", beta.GetSize() * sizeof(T));
        });
    }
};

template <typename T, bool isReuseSource = false>
class CheckFuncClassLayerNorm : public DataTypeCheckFuncBasicClass,
                                public ReuseSourceCheckFuncBasicClass,
                                public SingleTensorCheckFuncBasicClass,
                                public MultipleTensorCheckFuncBasicClass,
                                public CheckLayerNormParamsClass {
public:
    __aicore__ inline CheckFuncClassLayerNorm(){};
    __aicore__ inline CheckFuncClassLayerNorm(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          ReuseSourceCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance,
        const LocalTensor<T>& inputX, const LocalTensor<T>& gamma, const LocalTensor<T>& beta,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const T epsilon, LayerNormTiling& tiling)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, half, float>(
            "template parameter (T) is not half or float");

        if (std::is_same<T, half>::value) {
            ReuseSourceCheckFuncBasicClass::IsReuseSourceVerifyingParameters<false>(ARG_AND_STRING(isReuseSource));
        }

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(output, outputMean, outputVariance, inputX, gamma, beta, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(output, outputMean, outputVariance, gamma, sharedTmpBuffer));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(output, outputMean, outputVariance, beta, sharedTmpBuffer));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(outputMean, outputVariance, inputX, sharedTmpBuffer));

        CheckLayerNormParamsClass::CheckLayerNormParams<T, isReuseSource>(
            output, outputMean, outputVariance, inputX, gamma, beta, sharedTmpBuffer, epsilon, tiling);
    };
};

class CheckLayerNormRstdParamsClass {
public:
    template <typename U, typename T, bool isReuseSource = false, const LayerNormConfig& config = LNCFG_NORM>
    __aicore__ inline void CheckLayerNormRstdParams(
        const LocalTensor<T>& output, const LocalTensor<float>& outputMean, const LocalTensor<float>& outputRstd,
        const LocalTensor<T>& inputX, const LocalTensor<U>& gamma, const LocalTensor<U>& beta, const float epsilon,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const LayerNormPara& para, const LayerNormSeparateTiling& tiling)
    {
        VerifyingParameters<U, T, isReuseSource, config>(
            output, outputMean, outputRstd, inputX, gamma, beta, epsilon, sharedTmpBuffer, para, tiling);
        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters<U, T, isReuseSource, config>(
                output, outputMean, outputRstd, inputX, gamma, beta, epsilon, sharedTmpBuffer, para, tiling);
        }
    }

private:
    template <typename U, typename T, bool isReuseSource = false, const LayerNormConfig& config = LNCFG_NORM>
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& output, const LocalTensor<float>& outputMean, const LocalTensor<float>& outputRstd,
        const LocalTensor<T>& inputX, const LocalTensor<U>& gamma, const LocalTensor<U>& beta, const float epsilon,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const LayerNormPara& para, const LayerNormSeparateTiling& tiling)
    {
        ASCENDC_ASSERT((config.isOnlyOutput == false || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[LayerNorm Rtsd] The config.isOnlyOutput is %d, should be false.", config.isOnlyOutput);
        });
        ASCENDC_ASSERT((para.aLength * para.rLength <= inputX.GetSize() || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[LayerNorm Rtsd] The result of para.aLength * para.rLength is %u, should not be greater than inputX "
                "size %u.",
                para.aLength * para.rLength, inputX.GetSize());
        });
        ASCENDC_ASSERT((para.rLengthWithPadding * sizeof(T) % ONE_BLK_SIZE == 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[LayerNorm Rtsd] The para.rLengthWithPadding is %u, should be 32B aligned.",
                para.rLengthWithPadding);
        });
    }

    template <typename U, typename T, bool isReuseSource = false, const LayerNormConfig& config = LNCFG_NORM>
    __aicore__ inline void PrintParameters(
        const LocalTensor<T>& output, const LocalTensor<float>& outputMean, const LocalTensor<float>& outputRstd,
        const LocalTensor<T>& inputX, const LocalTensor<U>& gamma, const LocalTensor<U>& beta, const float epsilon,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const LayerNormPara& para, const LayerNormSeparateTiling& tiling)
    {
        KERNEL_LOG(
            KERNEL_INFO,
            "[LayerNorm Rtsd] The config.isOnlyOutput is %d, config.isNoBeta is %d, config.isNoGamma is %d.",
            config.isOnlyOutput, config.isNoBeta, config.isNoGamma);
        KERNEL_LOG(
            KERNEL_INFO, "[LayerNorm Rtsd] The para.aLength is %u, para.rLength is %u, para.rLengthWithPadding is %u.",
            para.aLength, para.rLength, para.rLengthWithPadding);
    }
};

template <typename U, typename T, bool isReuseSource = false, const LayerNormConfig& config = LNCFG_NORM>
class CheckFuncClassLayerNormRstd : public DataTypeCheckFuncBasicClass,
                                    public ReuseSourceCheckFuncBasicClass,
                                    public SingleTensorCheckFuncBasicClass,
                                    public MultipleTensorCheckFuncBasicClass,
                                    public CheckLayerNormRstdParamsClass {
public:
    __aicore__ inline CheckFuncClassLayerNormRstd(){};
    __aicore__ inline CheckFuncClassLayerNormRstd(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          ReuseSourceCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& output, const LocalTensor<float>& outputMean, const LocalTensor<float>& outputRstd,
        const LocalTensor<T>& inputX, const LocalTensor<U>& gamma, const LocalTensor<U>& beta, const float epsilon,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const LayerNormPara& para, const LayerNormSeparateTiling& tiling)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<U, half, float>(
            "first template parameter (U) is not half or float");
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, half, float>(
            "second template parameter (T) is not half or float");

        if (std::is_same<T, half>::value) {
            ReuseSourceCheckFuncBasicClass::IsReuseSourceVerifyingParameters<false>(ARG_AND_STRING(isReuseSource));
        }

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(output, outputMean, outputRstd, inputX, gamma, beta, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(output, outputMean, outputRstd, gamma, sharedTmpBuffer));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(output, outputMean, outputRstd, beta, sharedTmpBuffer));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(output, outputMean, outputRstd, inputX, sharedTmpBuffer));

        CheckLayerNormRstdParamsClass::CheckLayerNormRstdParams<U, T, isReuseSource, config>(
            output, outputMean, outputRstd, inputX, gamma, beta, epsilon, sharedTmpBuffer, para, tiling);
    };
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_NORMALIZATION_LAYERNORM_LAYERNORM_CHECK_COMMON_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LAYERNORM_CHECK_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LAYERNORM_CHECK_COMMON_H__
#endif
