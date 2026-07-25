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
 * \file fmod_check_common.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/math/fmod/fmod_check_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/fmod.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_FMOD_CHECK_COMMON_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_MATH_FMOD_FMOD_CHECK_COMMON_H_
#define IMPL_API_CHECK_KERNEL_CHECK_MATH_FMOD_FMOD_CHECK_COMMON_H_

#include "../../basic_check/datatype_check.h"
#include "../../basic_check/calcount_check.h"
#include "../../basic_check/reuse_source_check.h"
#include "../../basic_check/single_tensor_check.h"
#include "../../basic_check/multiple_tensor_check.h"

namespace AscendC {
namespace HighLevelApiCheck {
class CheckFmodParams {
public:
    template <typename T>
    __aicore__ inline void CheckTensorSize(const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor)
    {
        VerifyingParameters<T>(src0Tensor, src1Tensor);
        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters<T>(src0Tensor, src1Tensor);
        }
    }

private:
    template <typename T>
    __aicore__ inline void VerifyingParameters(const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor)
    {
        ASCENDC_ASSERT((src0Tensor.GetSize() == src1Tensor.GetSize() || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[Fmod] src0Tensor size is %u, should be equal with src1Tensor size %u!",
                src0Tensor.GetSize(), src1Tensor.GetSize());
        });
    }

    template <typename T>
    __aicore__ inline void PrintParameters(const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor)
    {
        KERNEL_LOG(
            KERNEL_INFO, "[Fmod] src0Tensor size is %u, src1Tensor size is %u. ", src0Tensor.GetSize(),
            src1Tensor.GetSize());
    }
};

template <typename T, bool isReuseSource = false>
class CheckFuncClassFmod : public DataTypeCheckFuncBasicClass,
                           public CalCountCheckFuncBasicClass,
                           public ReuseSourceCheckFuncBasicClass,
                           public SingleTensorCheckFuncBasicClass,
                           public MultipleTensorCheckFuncBasicClass,
                           public CheckFmodParams {
public:
    __aicore__ inline CheckFuncClassFmod(){};
    __aicore__ inline CheckFuncClassFmod(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          CalCountCheckFuncBasicClass(apiName),
          ReuseSourceCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, half, float>(
            "template parameter (T) is not half or float");

        ReuseSourceCheckFuncBasicClass::IsReuseSourceVerifyingParameters<false>(ARG_AND_STRING(isReuseSource));

        CheckFmodParams::CheckTensorSize<T>(src0Tensor, src1Tensor);

        CalCountCheckFuncBasicClass::CalCountVerifyingParameters(
            ARG_AND_STRING(calCount), VA_ARGS_TO_MAKE_TUPLE(dstTensor, src0Tensor, src1Tensor));

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, src0Tensor, src1Tensor, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, src0Tensor, sharedTmpBuffer));

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, src1Tensor, sharedTmpBuffer));
    };
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_MATH_FMOD_FMOD_CHECK_COMMON_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_FMOD_CHECK_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_FMOD_CHECK_COMMON_H__
#endif
