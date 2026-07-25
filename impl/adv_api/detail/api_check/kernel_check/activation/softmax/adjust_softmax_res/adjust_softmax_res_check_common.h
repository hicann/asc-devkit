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
 * \file adjust_softmax_res_check_common.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/activation/softmax/adjust_softmax_res/adjust_softmax_res_check_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/softmax.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ADJUST_SOFTMAX_RES_CHECK_COMMON_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_SOFTMAX_ADJUST_SOFTMAX_RES_ADJUST_SOFTMAX_RES_CHECK_COMMON_H_
#define IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_SOFTMAX_ADJUST_SOFTMAX_RES_ADJUST_SOFTMAX_RES_CHECK_COMMON_H_

#include "../../../basic_check/datatype_check.h"
#include "../../../basic_check/single_tensor_check.h"
#include "../../../basic_check/multiple_tensor_check.h"
#include "../../../../../../../../include/adv_api/activation/softmax_utils.h"

namespace AscendC {
namespace HighLevelApiCheck {
class CheckAdjustSoftMaxResParamsClass {
public:
    template <typename T1, typename T2, bool isDataFormatNZ = false, uint8_t stepSizeMode = 0>
    __aicore__ inline void CheckAdjustSoftMaxResParams(
        const LocalTensor<T1>& softMaxRes, const LocalTensor<T2>& maxTensor, const uint32_t from, const T1 to,
        const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        VerifyingParameters<T1, T2, isDataFormatNZ, stepSizeMode>(softMaxRes, maxTensor, from, to, softmaxShapeInfo);
        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters<T1, T2, isDataFormatNZ, stepSizeMode>(softMaxRes, maxTensor, from, to, softmaxShapeInfo);
        }
    }

private:
    template <typename T1, typename T2, bool isDataFormatNZ = false, uint8_t stepSizeMode = 0>
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T1>& softMaxRes, const LocalTensor<T2>& maxTensor, const uint32_t from, const T1 to,
        const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        ASCENDC_ASSERT((softmaxShapeInfo.srcK * sizeof(T1) % ONE_BLK_SIZE == 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[AdjustSoftMaxRes] The softmaxShapeInfo.srcK is %u, should be 32B aligned.",
                softmaxShapeInfo.srcK);
        });
        ASCENDC_ASSERT(
            (softmaxShapeInfo.srcK * softmaxShapeInfo.srcM <= softMaxRes.GetSize() || HighLevelAPIParametersPrint), {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "[AdjustSoftMaxRes] The softmaxShapeInfo.srcK is %u, softmaxShapeInfo.srcM is %u, "
                    "the product of softmaxShapeInfo.srcM and softmaxShapeInfo.srcK should not be greater than "
                    "softMaxRes size %u.",
                    softmaxShapeInfo.srcK, softmaxShapeInfo.srcM, softMaxRes.GetSize());
            });
    }

    template <typename T1, typename T2, bool isDataFormatNZ = false, uint8_t stepSizeMode = 0>
    __aicore__ inline void PrintParameters(
        const LocalTensor<T1>& softMaxRes, const LocalTensor<T2>& maxTensor, const uint32_t from, const T1 to,
        const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        KERNEL_LOG(
            KERNEL_INFO,
            "[AdjustSoftMaxRes] The softmaxShapeInfo.srcK is %u, softmaxShapeInfo.srcM is %u, "
            "softmaxShapeInfo.oriSrcM is %u, softmaxShapeInfo.oriSrcK is %u.",
            softmaxShapeInfo.srcK, softmaxShapeInfo.srcM, softmaxShapeInfo.oriSrcM, softmaxShapeInfo.oriSrcK);
    }
};

template <typename T1, typename T2, bool isDataFormatNZ = false, uint8_t stepSizeMode = 0>
class CheckFuncClassAdjustSoftMaxRes : public DataTypeCheckFuncBasicClass,
                                       public SingleTensorCheckFuncBasicClass,
                                       public MultipleTensorCheckFuncBasicClass,
                                       public CheckAdjustSoftMaxResParamsClass {
public:
    __aicore__ inline CheckFuncClassAdjustSoftMaxRes(){};
    __aicore__ inline CheckFuncClassAdjustSoftMaxRes(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T1>& softMaxRes, const LocalTensor<T2>& maxTensor, const uint32_t from, const T1 to,
        const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T1, half, float>(
            "first template parameter (T1) is not half or float");

        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T2, half, float>(
            "second template parameter (T2) is not half or float");

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(softMaxRes, maxTensor),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        CheckAdjustSoftMaxResParamsClass::CheckAdjustSoftMaxResParams<T1, T2, isDataFormatNZ, stepSizeMode>(
            softMaxRes, maxTensor, from, to, softmaxShapeInfo);
    };
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_SOFTMAX_ADJUST_SOFTMAX_RES_ADJUST_SOFTMAX_RES_CHECK_COMMON_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ADJUST_SOFTMAX_RES_CHECK_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ADJUST_SOFTMAX_RES_CHECK_COMMON_H__
#endif
