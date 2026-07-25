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
 * \file mean_check_common.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/reduce/mean/mean_check_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/reduce/mean.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MEAN_CHECK_COMMON_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_REDUCE_MEAN_MEAN_CHECK_COMMON_H_
#define IMPL_API_CHECK_KERNEL_CHECK_REDUCE_MEAN_MEAN_CHECK_COMMON_H_

#include "../reduce_check_utils.h"

namespace AscendC {
namespace HighLevelApiCheck {

class MeanParamsCheck {
public:
    __aicore__ inline MeanParamsCheck(__gm__ const char* apiName) { this->apiName = apiName; };

    template <typename T>
    __aicore__ inline void CheckMeanParamsMeanParams(const MeanParams& meanParams)
    {
        ASCENDC_ASSERT(((meanParams.outter != 0) || HighLevelAPIParametersPrint), {
            KERNEL_LOG(KERNEL_ERROR, "[%s] MeanParams.outter can't be zero!", this->apiName);
        });
        ASCENDC_ASSERT(((meanParams.inner != 0) || HighLevelAPIParametersPrint), {
            KERNEL_LOG(KERNEL_ERROR, "[%s] MeanParams.inner can't be zero!", this->apiName);
        });
        ASCENDC_ASSERT(((meanParams.inner * sizeof(T) % ONE_BLK_SIZE == 0) || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[%s] MeanParams.inner must be 32B aligned, but the actual value is %u!", this->apiName,
                meanParams.inner);
        });
        bool ans = ((meanParams.n >= 1) && (meanParams.n <= meanParams.inner));
        ASCENDC_ASSERT((ans || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[%s] MeanParams.n must be greater than or equal to 1 and less than or equal to MeanParams.inner, but "
                "actual only %u!",
                this->apiName, meanParams.n);
        });
        if constexpr (HighLevelAPIParametersPrint) {
            KERNEL_LOG(KERNEL_INFO, "[%s] The parameter MeanParams.outter is %u!", apiName, meanParams.outter);
            KERNEL_LOG(KERNEL_INFO, "[%s] The parameter MeanParams.inner is %u!", apiName, meanParams.inner);
            KERNEL_LOG(KERNEL_INFO, "[%s] The parameter MeanParams.n is %u!", apiName, meanParams.n);
        }
    }

    template <typename T>
    __aicore__ inline void CheckMeanParamsSrcTensorSize(const LocalTensor<T>& srcTensor, const MeanParams& meanParams)
    {
        bool ans = srcTensor.GetSize() >= meanParams.outter * meanParams.inner;
        ASCENDC_ASSERT((ans || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[%s] srcTensor size should be greater than or equal to %u, but actual only %u!",
                this->apiName, meanParams.outter * meanParams.inner, srcTensor.GetSize());
        });
        if constexpr (HighLevelAPIParametersPrint) {
            KERNEL_LOG(KERNEL_INFO, "[%s] The parameter srcTensor size is %u!", apiName, srcTensor.GetSize());
        }
    }

    template <typename T>
    __aicore__ inline void CheckMeanParamsDstTensorSize(const LocalTensor<T>& dstTensor, const MeanParams& meanParams)
    {
        uint32_t dstNeedSize =
            (meanParams.outter * sizeof(T) + ONE_BLK_SIZE - 1) / ONE_BLK_SIZE * ONE_BLK_SIZE / sizeof(T);
        uint32_t dstCurSize = dstTensor.GetSize();
        bool ans = dstCurSize >= dstNeedSize;
        ASCENDC_ASSERT((ans || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[%s] dstTensor size should be greater than or equal to %u, but actual only %u!",
                this->apiName, dstNeedSize, dstCurSize);
        });
        if constexpr (HighLevelAPIParametersPrint) {
            KERNEL_LOG(KERNEL_INFO, "[%s] The parameter dstTensor size is %u!", apiName, dstTensor.GetSize());
        }
    }

    __aicore__ inline void CheckMeanParamsTmpBufferSize(
        const LocalTensor<uint8_t>& sharedTmpBuffer, uint32_t tmpBufferSize)
    {
        bool ans = sharedTmpBuffer.GetSize() >= tmpBufferSize;
        ASCENDC_ASSERT((ans || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[%s] sharedTmpBuffer size should be greater than or equal to %u, but actual only %u!",
                this->apiName, tmpBufferSize, sharedTmpBuffer.GetSize());
        });
        if constexpr (HighLevelAPIParametersPrint) {
            KERNEL_LOG(
                KERNEL_INFO, "[%s] The parameter sharedTmpBuffer size is %u!", apiName, sharedTmpBuffer.GetSize());
        }
    }

    template <bool isConfigurable>
    __aicore__ inline void CheckMeanParamsBasicBlock(const bool isBasicBlock, __gm__ const char* paraName)
    {
        if ((!isConfigurable && isBasicBlock == true) || HighLevelAPIParametersPrint) {
            KERNEL_LOG(KERNEL_WARN, "[%s] The parameter %s is true, may not be effective.", apiName, paraName);
        }
        if constexpr (HighLevelAPIParametersPrint) {
            KERNEL_LOG(KERNEL_INFO, "[%s] The parameter %s is %u!", apiName, paraName, isBasicBlock);
        }
    }

    template <bool isConfigurable>
    __aicore__ inline void CheckMeanParamsReduceDim(const int32_t reduceDim, __gm__ const char* paraName)
    {
        if ((!isConfigurable && reduceDim != -1) || HighLevelAPIParametersPrint) {
            KERNEL_LOG(KERNEL_WARN, "[%s] The parameter %s is %d, may not be effective.", apiName, paraName, reduceDim);
        }
        if constexpr (HighLevelAPIParametersPrint) {
            KERNEL_LOG(KERNEL_INFO, "[%s] The parameter %s is %d!", apiName, paraName, reduceDim);
        }
    }

private:
    __gm__ const char* apiName = nullptr;
};

template <typename T, typename accType, bool isReuseSource, bool isBasicBlock, int32_t reduceDim>
class CheckFuncClassMean : public CheckFuncClassReduceBase {
public:
    __aicore__ inline CheckFuncClassMean(){};
    __aicore__ inline CheckFuncClassMean(__gm__ const char* apiName)
        : CheckFuncClassReduceBase(apiName), meanParamsCheck(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
        const MeanParams& meanParams, uint32_t tmpBufferSize)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<
            Std::tuple<T, accType>, Std::tuple<half, half>, Std::tuple<half, float>, Std::tuple<float, float>>(
            "input template parameter (T, accType) is not (half, float) or (half, half) or (float, float)!");
        ReuseSourceCheckFuncBasicClass::IsReuseSourceVerifyingParameters<false>(ARG_AND_STRING(isReuseSource));
        this->meanParamsCheck.template CheckMeanParamsMeanParams<T>(meanParams);
        this->meanParamsCheck.template CheckMeanParamsSrcTensorSize<T>(srcTensor, meanParams);
        this->meanParamsCheck.template CheckMeanParamsDstTensorSize<T>(dstTensor, meanParams);
        this->meanParamsCheck.CheckMeanParamsTmpBufferSize(sharedTmpBuffer, tmpBufferSize);
        this->meanParamsCheck.CheckMeanParamsBasicBlock<false>(ARG_AND_STRING(isBasicBlock));
        this->meanParamsCheck.CheckMeanParamsReduceDim<false>(ARG_AND_STRING(reduceDim));

        SingleTensorCheckFuncBasicClass::TPositionVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        SingleTensorCheckFuncBasicClass::TensorSizeVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor, sharedTmpBuffer));

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor, sharedTmpBuffer));
    };

private:
    MeanParamsCheck meanParamsCheck;
};
} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_REDUCE_MEAN_MEAN_CHECK_COMMON_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MEAN_CHECK_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MEAN_CHECK_COMMON_H__
#endif
