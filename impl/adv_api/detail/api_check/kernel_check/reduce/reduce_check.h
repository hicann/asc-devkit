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
 * \file reduce_check.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/reduce/reduce_check.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/reduce/reduce.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_CHECK_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_REDUCE_REDUCE_REDUCE_CHECK_H_
#define IMPL_API_CHECK_KERNEL_CHECK_REDUCE_REDUCE_REDUCE_CHECK_H_

#include "reduce_check_utils.h"
#include "../../../../../../include/adv_api/reduce/reduce_common.h"

namespace AscendC {
namespace HighLevelApiCheck {

class ReduceParamsCheck {
public:
    __aicore__ inline ReduceParamsCheck(__gm__ const char* apiName) { this->apiName = apiName; };

    __aicore__ inline void CheckSrcShape(const uint32_t srcShape[])
    {
        uint32_t firstAxis = srcShape[0];
        uint32_t lastAxis = srcShape[1];
        ASCENDC_ASSERT((((firstAxis > 0) && (lastAxis > 0)) || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[%s] firstAxis and lastAxis must be greater than 0, current firstAxis is %u and lastAxis is %u!",
                apiName, firstAxis, lastAxis);
        });
        if constexpr (HighLevelAPIParametersPrint) {
            KERNEL_LOG(
                KERNEL_INFO, "[%s] The parameter srcShape firstAxis is %u, srcShape firstAxis is %u!", apiName,
                firstAxis, lastAxis);
        }
    }

    template <typename T>
    __aicore__ inline void CheckSrcSize(
        const LocalTensor<T>& srcTensor, const uint32_t srcShape[], const uint32_t padLast)
    {
        uint32_t firstAxis = srcShape[0];
        uint32_t lastAxis = srcShape[1];
        ASCENDC_ASSERT(((srcTensor.GetSize() >= firstAxis * padLast) || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[%s] srcTensor size must be greater than or equal to %u, current size is %u!", apiName,
                firstAxis * padLast, srcTensor.GetSize());
        });
        if constexpr (HighLevelAPIParametersPrint) {
            KERNEL_LOG(KERNEL_INFO, "[%s] The parameter srcTensor is %u!", apiName, srcTensor.GetSize());
        }
    }

    template <typename T, class pattern>
    __aicore__ inline void CheckDstSize(const LocalTensor<T>& dstTensor, const uint32_t srcShape[])
    {
        uint32_t firstAxis = srcShape[0];
        uint32_t lastAxis = srcShape[1];
        if constexpr (IsSameType<pattern, Pattern::Reduce::AR>::value) {
            ASCENDC_ASSERT(((dstTensor.GetSize() >= firstAxis) || HighLevelAPIParametersPrint), {
                KERNEL_LOG(
                    KERNEL_ERROR, "[%s] dstTensor size must be greater than or equal to %u, current size is %u!",
                    apiName, firstAxis, dstTensor.GetSize());
            });
        } else {
            ASCENDC_ASSERT(((dstTensor.GetSize() >= lastAxis) || HighLevelAPIParametersPrint), {
                KERNEL_LOG(
                    KERNEL_ERROR, "[%s] dstTensor size must be greater than or equal to %u, current size is %u!",
                    apiName, lastAxis, dstTensor.GetSize());
            });
        }
        if constexpr (HighLevelAPIParametersPrint) {
            KERNEL_LOG(KERNEL_INFO, "[%s] The parameter dstTensor size is %u!", apiName, dstTensor.GetSize());
        }
    }

    __aicore__ inline void CheckSrcInnerPad(bool srcInnerPad)
    {
        ASCENDC_ASSERT((srcInnerPad || HighLevelAPIParametersPrint), {
            KERNEL_LOG(KERNEL_ERROR, "[%s] srcInnerPad must be set to true!", apiName);
        });
        if constexpr (HighLevelAPIParametersPrint) {
            KERNEL_LOG(
                KERNEL_INFO, "[%s] The parameter %d %s is %d!", apiName, ARG_AND_STRING(srcInnerPad), srcInnerPad);
        }
    }

private:
    __gm__ const char* apiName = nullptr;
};

template <typename T, class pattern>
class CheckFuncClassReduce : public CheckFuncClassReduceBase {
public:
    __aicore__ inline CheckFuncClassReduce(){};
    __aicore__ inline CheckFuncClassReduce(__gm__ const char* apiName)
        : CheckFuncClassReduceBase(apiName), reduceParamsCheck(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
        const uint32_t srcShape[], const bool srcInnerPad, const uint32_t padLast)
    {
        this->reduceParamsCheck.CheckSrcShape(srcShape);
        this->reduceParamsCheck.template CheckSrcSize<T>(srcTensor, srcShape, padLast);
        this->reduceParamsCheck.template CheckDstSize<T, pattern>(dstTensor, srcShape);
        this->reduceParamsCheck.CheckSrcInnerPad(srcInnerPad);
        SingleTensorCheckFuncBasicClass::TPositionVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        SingleTensorCheckFuncBasicClass::TensorSizeVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor, sharedTmpBuffer));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor, sharedTmpBuffer));
    };

private:
    ReduceParamsCheck reduceParamsCheck;
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_REDUCE_REDUCE_REDUCE_CHECK_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_CHECK_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_CHECK_H__
#endif
