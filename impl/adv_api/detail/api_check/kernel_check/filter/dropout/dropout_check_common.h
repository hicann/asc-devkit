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
 * \file dropout_check_common.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/filter/dropout/dropout_check_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/filter/dropout.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DROPOUT_CHECK_COMMON_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_FILTER_DROPOUT_DROPOUT_CHECK_COMMON_H_
#define IMPL_API_CHECK_KERNEL_CHECK_FILTER_DROPOUT_DROPOUT_CHECK_COMMON_H_

#include "../../basic_check/datatype_check.h"
#include "../../basic_check/single_tensor_check.h"
#include "../../basic_check/multiple_tensor_check.h"

namespace AscendC {
namespace HighLevelApiCheck {
class CheckDropOutParamsClass {
public:
    template <typename T, bool isInitBitMode = false, uint32_t dropOutMode = 0>
    __aicore__ inline void CheckDropOutParams(
        const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& maskLocal,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const float keepProb, const DropOutShapeInfo& info)
    {
        VerifyingParameters<T, isInitBitMode, dropOutMode>(
            dstLocal, srcLocal, maskLocal, sharedTmpBuffer, keepProb, info);
        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters<T, isInitBitMode, dropOutMode>(
                dstLocal, srcLocal, maskLocal, sharedTmpBuffer, keepProb, info);
        }
    }

private:
    template <typename T, bool isInitBitMode = false, uint32_t dropOutMode = 0>
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& maskLocal,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const float keepProb, const DropOutShapeInfo& info)
    {
        bool ans = dropOutMode == 0 || dropOutMode == 1 || dropOutMode == 2 || dropOutMode == 3 || dropOutMode == 4;
        ASCENDC_ASSERT((ans || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[DropOut] The dropOutMode parameter cannot be %u, should be 0, 1, 2, 3 or 4.",
                dropOutMode);
        });
        ASCENDC_ASSERT((keepProb > 0 && keepProb < 1 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(KERNEL_ERROR, "[DropOut] The keepProb parameter cannot be %f, should be in (0, 1).", keepProb);
        });
        ASCENDC_ASSERT((info.firstAxis > 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(KERNEL_ERROR, "[DropOut] The info.firstAxis parameter (%u) must > 0.", info.firstAxis);
        });
        ASCENDC_ASSERT((info.srcLastAxis > 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(KERNEL_ERROR, "[DropOut] The info.srcLastAxis parameter (%u) must > 0.", info.srcLastAxis);
        });
        ASCENDC_ASSERT((info.maskLastAxis > 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(KERNEL_ERROR, "[DropOut] The info.maskLastAxis parameter (%u) must > 0.", info.maskLastAxis);
        });
        if constexpr (dropOutMode == 1 || dropOutMode == 4) {
            ASCENDC_ASSERT((info.maskLastAxis % 2 == 0 || HighLevelAPIParametersPrint), {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "[DropOut] The info.maskLastAxis is %u, should be multiples of 2 when dropOutMode is 1 or 4.",
                    info.maskLastAxis);
            });
        }
    }

    template <typename T, bool isInitBitMode = false, uint32_t dropOutMode = 0>
    __aicore__ inline void PrintParameters(
        const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& maskLocal,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const float keepProb, const DropOutShapeInfo& info)
    {
        KERNEL_LOG(KERNEL_INFO, "[DropOut] The dropOutMode parameter is %u.", dropOutMode);
        KERNEL_LOG(KERNEL_INFO, "[DropOut] The keepProb parameter is %f.", keepProb);
        KERNEL_LOG(
            KERNEL_INFO, "[DropOut] The info.firstAxis is %u, info.srcLastAxis is %u, info.maskLastAxis is %u.",
            info.firstAxis, info.srcLastAxis, info.maskLastAxis);
    }
};

template <typename T, bool isInitBitMode = false, uint32_t dropOutMode = 0>
class CheckFuncClassDropOut : public DataTypeCheckFuncBasicClass,
                              public SingleTensorCheckFuncBasicClass,
                              public MultipleTensorCheckFuncBasicClass,
                              public CheckDropOutParamsClass {
public:
    __aicore__ inline CheckFuncClassDropOut(){};
    __aicore__ inline CheckFuncClassDropOut(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& maskLocal,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const float keepProb, const DropOutShapeInfo& info)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, half, float>(
            "template parameter (T) is not half or float");

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstLocal, srcLocal, maskLocal, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        CheckDropOutParamsClass::CheckDropOutParams<T, isInitBitMode, dropOutMode>(
            dstLocal, srcLocal, maskLocal, sharedTmpBuffer, keepProb, info);
    };
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_FILTER_DROPOUT_DROPOUT_CHECK_COMMON_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DROPOUT_CHECK_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DROPOUT_CHECK_COMMON_H__
#endif
