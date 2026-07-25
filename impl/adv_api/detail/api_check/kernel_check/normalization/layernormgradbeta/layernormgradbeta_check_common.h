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
 * \file layernormgradbeta_check_common.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/normalization/layernormgradbeta/layernormgradbeta_check_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/normalization/layernormgradbeta.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LAYERNORMGRADBETA_CHECK_COMMON_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_NORMALIZATION_LAYERNORMGRADBETA_LAYERNORMGRADBETA_CHECK_COMMON_H_
#define IMPL_API_CHECK_KERNEL_CHECK_NORMALIZATION_LAYERNORMGRADBETA_LAYERNORMGRADBETA_CHECK_COMMON_H_

#include "../../basic_check/datatype_check.h"
#include "../../basic_check/reuse_source_check.h"
#include "../../basic_check/single_tensor_check.h"
#include "../../basic_check/multiple_tensor_check.h"

namespace AscendC {
namespace HighLevelApiCheck {
class CheckLayerNormGradBetaParamsClass {
public:
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void CheckLayerNormGradBetaParams(
        const LocalTensor<T>& outputPdGamma, const LocalTensor<T>& outputPdBeta, const LocalTensor<T>& resForGamma,
        const LocalTensor<T>& inputDy, const LocalTensor<uint8_t>& sharedTmpBuffer,
        const LayerNormGradBetaTiling& tiling)
    {
        VerifyingParameters<T, isReuseSource>(
            outputPdGamma, outputPdBeta, resForGamma, inputDy, sharedTmpBuffer, tiling);
    }

private:
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& outputPdGamma, const LocalTensor<T>& outputPdBeta, const LocalTensor<T>& resForGamma,
        const LocalTensor<T>& inputDy, const LocalTensor<uint8_t>& sharedTmpBuffer,
        const LayerNormGradBetaTiling& tiling)
    {
        ASCENDC_ASSERT(((outputPdGamma.GetSize() * sizeof(T)) % ONE_BLK_SIZE == 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[LayerNormGradBeta] The outputPdGamma length is %lu, should be 32B aligned.",
                outputPdGamma.GetSize() * sizeof(T));
        });
        ASCENDC_ASSERT(((outputPdBeta.GetSize() * sizeof(T)) % ONE_BLK_SIZE == 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[LayerNormGradBeta] The outputPdBeta length is %lu, should be 32B aligned.",
                outputPdBeta.GetSize() * sizeof(T));
        });
    }
};
template <typename T, bool isReuseSource = false>
class CheckFuncClassLayerNormGradBeta : public DataTypeCheckFuncBasicClass,
                                        public ReuseSourceCheckFuncBasicClass,
                                        public SingleTensorCheckFuncBasicClass,
                                        public MultipleTensorCheckFuncBasicClass,
                                        public CheckLayerNormGradBetaParamsClass {
public:
    __aicore__ inline CheckFuncClassLayerNormGradBeta(){};
    __aicore__ inline CheckFuncClassLayerNormGradBeta(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          ReuseSourceCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& outputPdGamma, const LocalTensor<T>& outputPdBeta, const LocalTensor<T>& resForGamma,
        const LocalTensor<T>& inputDy, const LocalTensor<uint8_t>& sharedTmpBuffer,
        const LayerNormGradBetaTiling& tiling)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, half, float>(
            "template parameter (T) is not half or float");

        if (std::is_same<T, half>::value) {
            ReuseSourceCheckFuncBasicClass::IsReuseSourceVerifyingParameters<false>(ARG_AND_STRING(isReuseSource));
        }

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(outputPdGamma, outputPdBeta, resForGamma, inputDy, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(outputPdGamma, outputPdBeta));

        CheckFuncClassLayerNormGradBeta::CheckLayerNormGradBetaParams(
            outputPdGamma, outputPdBeta, resForGamma, inputDy, sharedTmpBuffer, tiling);
    };
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_NORMALIZATION_LAYERNORMGRADBETA_LAYERNORMGRADBETA_CHECK_COMMON_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LAYERNORMGRADBETA_CHECK_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LAYERNORMGRADBETA_CHECK_COMMON_H__
#endif
