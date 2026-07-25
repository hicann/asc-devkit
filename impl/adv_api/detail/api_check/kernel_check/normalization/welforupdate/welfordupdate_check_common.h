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
 * \file welfordupdate_check_common.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/normalization/welforupdate/welfordupdate_check_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/normalization/welfordfinalize.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_WELFORDUPDATE_CHECK_COMMON_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_NORMALIZATION_WELFORDUPDATE_WELFORDUPDATE_CHECK_COMMON_H_
#define IMPL_API_CHECK_KERNEL_CHECK_NORMALIZATION_WELFORDUPDATE_WELFORDUPDATE_CHECK_COMMON_H_

#include "../../basic_check/datatype_check.h"
#include "../../basic_check/reuse_source_check.h"
#include "../../basic_check/single_tensor_check.h"
#include "../../basic_check/multiple_tensor_check.h"
#include "../../../../../../../include/adv_api/normalization/layernorm_utils.h"

namespace AscendC {
namespace HighLevelApiCheck {
class CheckWelfordUpdateParamsClass {
public:
    template <
        typename T, typename U, bool isReuseSource = false, const WelfordUpdateConfig& config = WFUPDATE_DEFAULT_CFG>
    __aicore__ inline void CheckWelfordUpdateParams(
        const LocalTensor<U>& outputMean, const LocalTensor<U>& outputVariance, const LocalTensor<U>& inputMean,
        const LocalTensor<U>& inputVariance, const LocalTensor<T>& inputX, const LocalTensor<uint8_t>& sharedTmpBuffer,
        const WelfordUpdateParam& para)
    {
        VerifyingParameters<T, U, isReuseSource, config>(
            outputMean, outputVariance, inputMean, inputVariance, inputX, sharedTmpBuffer, para);
        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters<T, U, isReuseSource, config>(
                outputMean, outputVariance, inputMean, inputVariance, inputX, sharedTmpBuffer, para);
        }
    }

private:
    template <
        typename T, typename U, bool isReuseSource = false, const WelfordUpdateConfig& config = WFUPDATE_DEFAULT_CFG>
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<U>& outputMean, const LocalTensor<U>& outputVariance, const LocalTensor<U>& inputMean,
        const LocalTensor<U>& inputVariance, const LocalTensor<T>& inputX, const LocalTensor<uint8_t>& sharedTmpBuffer,
        const WelfordUpdateParam& para)
    {
        ASCENDC_ASSERT((para.rnLength == 1 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(KERNEL_ERROR, "[WelfordUpdate] The para.rnLength is %u, should be 1.", para.rnLength);
        });

        ASCENDC_ASSERT((para.abLength * sizeof(T) % ONE_BLK_SIZE == 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[WelfordUpdate] The para.abLength is %u, should be an integral multiple of 32/sizeof(T).",
                para.abLength);
        });

        ASCENDC_ASSERT((para.abComputeLength > 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[WelfordUpdate] The para.abComputeLength is %u, should be greater than 0.",
                para.abComputeLength);
        });

        ASCENDC_ASSERT((para.rnLength * para.abLength <= outputMean.GetSize() || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[WelfordUpdate] The para.rnLength is %u, para.abLength is %u, "
                "the product of para.rnLength and para.abLength should not larger than outputMean size %u.",
                para.rnLength, para.abLength, outputMean.GetSize());
        });
        ASCENDC_ASSERT((para.rnLength * para.abLength <= outputVariance.GetSize() || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[WelfordUpdate] The para.rnLength is %u, para.abLength is %u, "
                "the product of para.rnLength and para.abLength should not larger than outputVariance size %u.",
                para.rnLength, para.abLength, outputVariance.GetSize());
        });
        ASCENDC_ASSERT((para.rnLength * para.abLength <= inputMean.GetSize() || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[WelfordUpdate] The para.rnLength is %u, para.abLength is %u, "
                "the product of para.rnLength and para.abLength should not larger than inputMean size %u.",
                para.rnLength, para.abLength, inputMean.GetSize());
        });
        ASCENDC_ASSERT((para.rnLength * para.abLength <= inputVariance.GetSize() || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[WelfordUpdate] The para.rnLength is %u, para.abLength is %u, "
                "the product of para.rnLength and para.abLength should not larger than inputVariance size %u.",
                para.rnLength, para.abLength, inputVariance.GetSize());
        });
        ASCENDC_ASSERT((para.rnLength * para.abLength <= inputX.GetSize() || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[WelfordUpdate] The para.rnLength is %u, para.abLength is %u, "
                "the product of para.rnLength and para.abLength should not larger than inputX size %u.",
                para.rnLength, para.abLength, inputX.GetSize());
        });
    }

    template <
        typename T, typename U, bool isReuseSource = false, const WelfordUpdateConfig& config = WFUPDATE_DEFAULT_CFG>
    __aicore__ inline void PrintParameters(
        const LocalTensor<U>& outputMean, const LocalTensor<U>& outputVariance, const LocalTensor<U>& inputMean,
        const LocalTensor<U>& inputVariance, const LocalTensor<T>& inputX, const LocalTensor<uint8_t>& sharedTmpBuffer,
        const WelfordUpdateParam& para)
    {
        KERNEL_LOG(
            KERNEL_INFO,
            "[WelfordUpdate] The para.rnLength is %u, para.abLength is %u, "
            "para.abComputeLength is %u, the para.nRec is %f.",
            para.rnLength, para.abLength, para.abComputeLength, para.nRec);
    }
};

template <typename T, typename U, bool isReuseSource = false, const WelfordUpdateConfig& config = WFUPDATE_DEFAULT_CFG>
class CheckFuncClassWelfordUpdate : public DataTypeCheckFuncBasicClass,
                                    public ReuseSourceCheckFuncBasicClass,
                                    public SingleTensorCheckFuncBasicClass,
                                    public MultipleTensorCheckFuncBasicClass,
                                    public CheckWelfordUpdateParamsClass {
public:
    __aicore__ inline CheckFuncClassWelfordUpdate(){};
    __aicore__ inline CheckFuncClassWelfordUpdate(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          ReuseSourceCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<U>& outputMean, const LocalTensor<U>& outputVariance, const LocalTensor<U>& inputMean,
        const LocalTensor<U>& inputVariance, const LocalTensor<T>& inputX, const LocalTensor<uint8_t>& sharedTmpBuffer,
        const WelfordUpdateParam& para)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, half, float>(
            "first template parameter (T) is not half or float");
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<U, float>(
            "second template parameter (U) is not float");
#if __NPU_ARCH__ == 2002
        ReuseSourceCheckFuncBasicClass::IsReuseSourceVerifyingParameters<false>(ARG_AND_STRING(isReuseSource));
#endif

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(outputMean, outputVariance, inputMean, inputVariance, inputX, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(outputMean, outputVariance, inputMean, sharedTmpBuffer));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(outputMean, outputVariance, inputVariance, sharedTmpBuffer));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(outputMean, outputVariance, inputX, sharedTmpBuffer));

        CheckWelfordUpdateParamsClass::CheckWelfordUpdateParams<T, U, isReuseSource, config>(
            outputMean, outputVariance, inputMean, inputVariance, inputX, sharedTmpBuffer, para);
    };
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_NORMALIZATION_WELFORDUPDATE_WELFORDUPDATE_CHECK_COMMON_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_WELFORDUPDATE_CHECK_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_WELFORDUPDATE_CHECK_COMMON_H__
#endif
