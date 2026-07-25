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
 * \file batchnorm_check_c310.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/normalization/batchnorm/batchnorm_check_c310.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/normalization/batchnorm.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_BATCHNORM_CHECK_C310_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_NORMALIZATION_BATCHNORM_BATCHNORM_CHECK_C310_H_
#define IMPL_API_CHECK_KERNEL_CHECK_NORMALIZATION_BATCHNORM_BATCHNORM_CHECK_C310_H_

#include "../../basic_check/datatype_check.h"
#include "../../basic_check/reuse_source_check.h"
#include "../../basic_check/single_tensor_check.h"
#include "../../basic_check/multiple_tensor_check.h"

namespace AscendC {
namespace HighLevelApiCheck {
class CheckBatchNormParamsClass {
public:
    template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
    __aicore__ inline void CheckBatchNormParams(
        const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance,
        const LocalTensor<T>& inputX, const LocalTensor<T>& gamm, const LocalTensor<T>& beta,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const T epsilon, BatchNormTiling& tiling)
    {
        VerifyingParameters<T, isReuseSource>(
            output, outputMean, outputVariance, inputX, gamm, beta, sharedTmpBuffer, epsilon, tiling);
    }

private:
    template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance,
        const LocalTensor<T>& inputX, const LocalTensor<T>& gamm, const LocalTensor<T>& beta,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const T epsilon, BatchNormTiling& tiling)
    {
        ASCENDC_ASSERT(((outputMean.GetSize() * sizeof(T) % ONE_BLK_SIZE == 0) || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[BatchNorm] slength * hlength is %u, should be 32B aligned.",
                outputMean.GetSize() * sizeof(T));
        });
        ASCENDC_ASSERT(((gamm.GetSize() * sizeof(T) % ONE_BLK_SIZE == 0) || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[BatchNorm] The gamm length is %u, should be 32B aligned.", gamm.GetSize() * sizeof(T));
        });
        ASCENDC_ASSERT(((beta.GetSize() * sizeof(T) % ONE_BLK_SIZE == 0) || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[BatchNorm] The beta length is %u, should be 32B aligned.", beta.GetSize() * sizeof(T));
        });
    }
};

template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
class CheckFuncClassBatchNorm : public DataTypeCheckFuncBasicClass,
                                public ReuseSourceCheckFuncBasicClass,
                                public SingleTensorCheckFuncBasicClass,
                                public MultipleTensorCheckFuncBasicClass,
                                public CheckBatchNormParamsClass {
public:
    __aicore__ inline CheckFuncClassBatchNorm(){};
    __aicore__ inline CheckFuncClassBatchNorm(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          ReuseSourceCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance,
        const LocalTensor<T>& inputX, const LocalTensor<T>& gamm, const LocalTensor<T>& beta,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const T epsilon, BatchNormTiling& tiling)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, half, float>(
            "template parameter (T) is not half or float");
        ReuseSourceCheckFuncBasicClass::IsReuseSourceVerifyingParameters<false>(ARG_AND_STRING(isReuseSource));
        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(output, outputMean, outputVariance, inputX, gamm, beta, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(output, outputMean, outputVariance));
        CheckBatchNormParamsClass::CheckBatchNormParams(
            output, outputMean, outputVariance, inputX, gamm, beta, sharedTmpBuffer, epsilon, tiling);
    };
};
} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_NORMALIZATION_BATCHNORM_BATCHNORM_CHECK_C310_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_BATCHNORM_CHECK_C310_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_BATCHNORM_CHECK_C310_H__
#endif
