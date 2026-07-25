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
 * \file deepnorm_check_common.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/normalization/deepnorm/deepnorm_check_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/normalization/deepnorm.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEEPNORM_CHECK_COMMON_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_NORMALIZATION_DEEPNORM_DEEPNORM_CHECK_COMMON_H_
#define IMPL_API_CHECK_KERNEL_CHECK_NORMALIZATION_DEEPNORM_DEEPNORM_CHECK_COMMON_H_

#include "../../basic_check/datatype_check.h"
#include "../../basic_check/reuse_source_check.h"
#include "../../basic_check/single_tensor_check.h"
#include "../../basic_check/multiple_tensor_check.h"

namespace AscendC {
namespace HighLevelApiCheck {
class CheckDeepNormParamsClass {
public:
    template <typename T, bool isReuseSrc, bool isBasicBlock>
    __aicore__ inline void CheckDeepNormParams(
        const LocalTensor<T>& dstLocal, const LocalTensor<T>& meanLocal, const LocalTensor<T>& rstdLocal,
        const LocalTensor<T>& srcLocal, const LocalTensor<T>& gxLocal, const LocalTensor<T>& betaLocal,
        const LocalTensor<T>& gammaLocal, const LocalTensor<uint8_t>& sharedTmpBuffer, const T alpha, const T epsilon,
        DeepNormTiling& tiling)
    {
        VerifyingParameters<T, isReuseSrc, isBasicBlock>(
            dstLocal, meanLocal, rstdLocal, srcLocal, gxLocal, betaLocal, gammaLocal, sharedTmpBuffer, alpha, epsilon,
            tiling);
    }

private:
    template <typename T, bool isReuseSrc, bool isBasicBlock>
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstLocal, const LocalTensor<T>& meanLocal, const LocalTensor<T>& rstdLocal,
        const LocalTensor<T>& srcLocal, const LocalTensor<T>& gxLocal, const LocalTensor<T>& betaLocal,
        const LocalTensor<T>& gammaLocal, const LocalTensor<uint8_t>& sharedTmpBuffer, const T alpha, const T epsilon,
        DeepNormTiling& tiling)
    {
        ASCENDC_ASSERT(((betaLocal.GetSize() * sizeof(T)) % ONE_BLK_SIZE == 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[DeepNorm] The betaLocal length is %lu, should be 32B aligned.",
                betaLocal.GetSize() * sizeof(T));
        });
        ASCENDC_ASSERT(((gammaLocal.GetSize() * sizeof(T)) % ONE_BLK_SIZE == 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[DeepNorm] The gammaLocal length is %lu, should be 32B aligned.",
                gammaLocal.GetSize() * sizeof(T));
        });
        ASCENDC_ASSERT((betaLocal.GetSize() <= 2040 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[DeepNorm] The value of last axis can not be greater than 2040! current size is %u\n",
                betaLocal.GetSize());
        });
        ASCENDC_ASSERT((gammaLocal.GetSize() <= 2040 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[DeepNorm] The value of last axis can not be greater than 2040! current size is %u\n",
                betaLocal.GetSize());
        });
    }
};

template <typename T, bool isReuseSrc, bool isBasicBlock>
class CheckFuncClassDeepNorm : public DataTypeCheckFuncBasicClass,
                               public ReuseSourceCheckFuncBasicClass,
                               public SingleTensorCheckFuncBasicClass,
                               public MultipleTensorCheckFuncBasicClass,
                               public CheckDeepNormParamsClass {
public:
    __aicore__ inline CheckFuncClassDeepNorm(){};
    __aicore__ inline CheckFuncClassDeepNorm(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          ReuseSourceCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstLocal, const LocalTensor<T>& meanLocal, const LocalTensor<T>& rstdLocal,
        const LocalTensor<T>& srcLocal, const LocalTensor<T>& gxLocal, const LocalTensor<T>& betaLocal,
        const LocalTensor<T>& gammaLocal, const LocalTensor<uint8_t>& sharedTmpBuffer, const T alpha, const T epsilon,
        DeepNormTiling& tiling)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, half, float>(
            "template parameter (T) is not half or float");

        if (std::is_same<T, half>::value) {
            ReuseSourceCheckFuncBasicClass::IsReuseSourceVerifyingParameters<false>(ARG_AND_STRING(isReuseSrc));
        }

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(
                dstLocal, meanLocal, rstdLocal, srcLocal, gxLocal, betaLocal, gammaLocal, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        CheckDeepNormParamsClass::CheckDeepNormParams<T, isReuseSrc, isBasicBlock>(
            dstLocal, meanLocal, rstdLocal, srcLocal, gxLocal, betaLocal, gammaLocal, sharedTmpBuffer, alpha, epsilon,
            tiling);
    };
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_NORMALIZATION_DEEPNORM_DEEPNORM_CHECK_COMMON_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEEPNORM_CHECK_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEEPNORM_CHECK_COMMON_H__
#endif
