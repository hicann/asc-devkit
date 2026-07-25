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
 * \file sum_check_common.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/reduce/sum/sum_check_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/reduce/sum.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SUM_CHECK_COMMON_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_REDUCE_SUM_SUM_CHECK_COMMON_H_
#define IMPL_API_CHECK_KERNEL_CHECK_REDUCE_SUM_SUM_CHECK_COMMON_H_

#include "../reduce_check_utils.h"

namespace AscendC {
namespace HighLevelApiCheck {

class CheckSumParams {
public:
    __aicore__ inline CheckSumParams(__gm__ const char* apiName) { this->apiName = apiName; };

    template <typename T>
    __aicore__ inline void CheckSumParamsSumParams(const LocalTensor<T>& srcTensor, const SumParams& sumParams)
    {
        bool ans = sumParams.inner != 0 && (sumParams.inner * sizeof(T) % ONE_BLK_SIZE == 0);
        ASCENDC_ASSERT((ans || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[%s] SumParams.inner must be 32B aligned and must be greater than 0, current inner is %u!",
                this->apiName, sumParams.inner);
        });
        ans = (sumParams.inner <= srcTensor.GetSize());
        ASCENDC_ASSERT((ans || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[%s] SumParams.inner is %u must be less than or equal to srcTensor size, srcTensor size is %u",
                this->apiName, sumParams.inner, srcTensor.GetSize());
        });
        ans = ((sumParams.n >= 1) && (sumParams.n <= sumParams.inner));
        ASCENDC_ASSERT((ans || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[%s] SumParams.n must be greater than or equal to 1 and less than or equal to SumParams.inner, but "
                "actual only %u",
                this->apiName, sumParams.n);
        });
        if constexpr (HighLevelAPIParametersPrint) {
            KERNEL_LOG(KERNEL_INFO, "[%s] The parameter SumParams.outter is %u!", apiName, sumParams.outter);
            KERNEL_LOG(KERNEL_INFO, "[%s] The parameter SumParams.inner is %u!", apiName, sumParams.inner);
            KERNEL_LOG(KERNEL_INFO, "[%s] The parameter SumParams.n is %u!", apiName, sumParams.n);
        }
    }

    template <typename T>
    __aicore__ inline void CheckSumParamsDstTensorSize(const LocalTensor<T>& dstTensor, const SumParams& sumParams)
    {
        bool ans = dstTensor.GetSize() > 0;
        ASCENDC_ASSERT((ans || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[%s] dstTensor size must be greater than 0, but actual only %u", this->apiName,
                dstTensor.GetSize());
        });
        uint32_t dstNeedSize =
            (sumParams.outter * sizeof(T) + ONE_BLK_SIZE - 1) / ONE_BLK_SIZE * ONE_BLK_SIZE / sizeof(T);
        uint32_t dstCurSize = dstTensor.GetSize();
        ans = dstCurSize >= dstNeedSize;
        ASCENDC_ASSERT((ans || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[%s] dstTensor size should be greater than or equal to %u, but actual only %u!",
                this->apiName, dstNeedSize, dstCurSize);
        });
        if constexpr (HighLevelAPIParametersPrint) {
            KERNEL_LOG(KERNEL_INFO, "[%s] The parameter dstTensor size is %u!", apiName, dstTensor.GetSize());
        }
    }

    template <typename T>
    __aicore__ inline void CheckSumParamsSrcTensorSize(const LocalTensor<T>& srcTensor)
    {
        bool ans = srcTensor.GetSize() > 0;
        ASCENDC_ASSERT((ans || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[%s] srcTensor size must be greater than 0, but actual only %u", this->apiName,
                srcTensor.GetSize());
        });
        if constexpr (HighLevelAPIParametersPrint) {
            KERNEL_LOG(KERNEL_INFO, "[%s] The parameter srcTensor size is %u!", apiName, srcTensor.GetSize());
        }
    }

    template <typename T>
    __aicore__ inline void CheckSumParamsTmpBufferSize(
        const LocalTensor<uint8_t>& sharedTmpBuffer, const SumParams& sumParams)
    {
        uint32_t elementNumPerBlk = ONE_BLK_SIZE / sizeof(T);
        uint32_t elementNumPerRep = ONE_REPEAT_BYTE_SIZE / sizeof(T);
        uint32_t repeatTimes = (sumParams.n + elementNumPerRep - 1) / elementNumPerRep;
        uint32_t finalWorkSize = (repeatTimes + elementNumPerBlk - 1) / elementNumPerBlk * elementNumPerBlk * sizeof(T);
        ASCENDC_ASSERT((sharedTmpBuffer.GetSize() >= finalWorkSize || HighLevelAPIParametersPrint), {
            KERNEL_LOG(KERNEL_ERROR, "[%s] sharedTmpBuffer must be greater than %u!", this->apiName, finalWorkSize);
        });
        bool ans = sharedTmpBuffer.GetSize() > 0;
        ASCENDC_ASSERT((ans || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[%s] sharedTmpBuffer size must be greater than 0, but actual only %u", this->apiName,
                sharedTmpBuffer.GetSize());
        });
        if constexpr (HighLevelAPIParametersPrint) {
            KERNEL_LOG(
                KERNEL_INFO, "[%s] The parameter sharedTmpBuffer size is %u!", apiName, sharedTmpBuffer.GetSize());
        }
    }

    template <bool isConfigurable>
    __aicore__ inline void CheckSumParamsBasicBlock(const bool isBasicBlock, __gm__ const char* paraName)
    {
        if ((!isConfigurable && isBasicBlock == true) || HighLevelAPIParametersPrint) {
            KERNEL_LOG(KERNEL_WARN, "[%s] The parameter %s is true, may not be effective.", apiName, paraName);
        }
        if constexpr (HighLevelAPIParametersPrint) {
            KERNEL_LOG(KERNEL_INFO, "[%s] The parameter %s is %d!", apiName, paraName, isBasicBlock);
        }
    }

    template <bool isConfigurable>
    __aicore__ inline void CheckSumParamsReduceDim(const int32_t reduceDim, __gm__ const char* paraName)
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

template <typename T, int32_t reduceDim = -1, bool isReuseSource = false, bool isBasicBlock = false>
class CheckFuncClassSum : public CheckFuncClassReduceBase {
public:
    __aicore__ inline CheckFuncClassSum(){};
    __aicore__ inline CheckFuncClassSum(__gm__ const char* apiName)
        : CheckFuncClassReduceBase(apiName), checkSumParams(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
        const SumParams& sumParams)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, half, float>(
            "input template parameter (T) is not half or float");
        this->checkSumParams.template CheckSumParamsSumParams<T>(srcTensor, sumParams);
        this->checkSumParams.template CheckSumParamsDstTensorSize<T>(dstTensor, sumParams);
        this->checkSumParams.template CheckSumParamsSrcTensorSize<T>(srcTensor);
        this->checkSumParams.template CheckSumParamsTmpBufferSize<T>(sharedTmpBuffer, sumParams);

        this->checkSumParams.template CheckSumParamsBasicBlock<false>(ARG_AND_STRING(isBasicBlock));
        this->checkSumParams.template CheckSumParamsReduceDim<false>(ARG_AND_STRING(reduceDim));
        ReuseSourceCheckFuncBasicClass::IsReuseSourceVerifyingParameters<false>(ARG_AND_STRING(isReuseSource));

        SingleTensorCheckFuncBasicClass::TPositionVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        SingleTensorCheckFuncBasicClass::TensorSizeVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor, sharedTmpBuffer));

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor, sharedTmpBuffer));
    };

private:
    CheckSumParams checkSumParams;
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_REDUCE_SUM_SUM_CHECK_COMMON_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SUM_CHECK_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SUM_CHECK_COMMON_H__
#endif
