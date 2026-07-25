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
 * \file log_softmax_check_common.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/activation/softmax/log_softmax/log_softmax_check_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/logsoftmax.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LOG_SOFTMAX_CHECK_COMMON_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_SOFTMAX_LOG_SOFTMAX_LOG_SOFTMAX_CHECK_COMMON_H_
#define IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_SOFTMAX_LOG_SOFTMAX_LOG_SOFTMAX_CHECK_COMMON_H_

#include "../../../basic_check/datatype_check.h"
#include "../../../basic_check/reuse_source_check.h"
#include "../../../basic_check/single_tensor_check.h"
#include "../../../basic_check/multiple_tensor_check.h"
#include "../../../../../../../../include/adv_api/activation/softmax_utils.h"

namespace AscendC {
namespace HighLevelApiCheck {
class CheckLogSoftMaxParamsClass {
public:
    template <typename T, bool isReuseSource = false, bool isDataFormatNZ = false>
    __aicore__ inline void CheckLogSoftMaxParams(
        const LocalTensor<T>& dst, const LocalTensor<T>& sumTensor, const LocalTensor<T>& maxTensor,
        const LocalTensor<T>& src, const LocalTensor<uint8_t>& sharedTmpBuffer, const LogSoftMaxTiling& tiling,
        const SoftMaxShapeInfo& softmaxShapeInfo = {})
    {
        VerifyingParameters<T, isReuseSource, isDataFormatNZ>(
            dst, sumTensor, maxTensor, src, sharedTmpBuffer, tiling, softmaxShapeInfo);
        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters<T, isReuseSource, isDataFormatNZ>(
                dst, sumTensor, maxTensor, src, sharedTmpBuffer, tiling, softmaxShapeInfo);
        }
    }

private:
    template <typename T, bool isReuseSource = false, bool isDataFormatNZ = false>
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dst, const LocalTensor<T>& sumTensor, const LocalTensor<T>& maxTensor,
        const LocalTensor<T>& src, const LocalTensor<uint8_t>& sharedTmpBuffer, const LogSoftMaxTiling& tiling,
        const SoftMaxShapeInfo& softmaxShapeInfo = {})
    {
        ASCENDC_ASSERT((softmaxShapeInfo.srcK * sizeof(T) % ONE_BLK_SIZE == 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[LogSoftMax] The softmaxShapeInfo.srcK is %u, should be 32B aligned.",
                softmaxShapeInfo.srcK);
        });
        ASCENDC_ASSERT(
            (softmaxShapeInfo.srcK * softmaxShapeInfo.srcM <= src.GetSize() || HighLevelAPIParametersPrint), {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "[LogSoftMax] The softmaxShapeInfo.srcK is %u, softmaxShapeInfo.srcM is %u, "
                    "the product of softmaxShapeInfo.srcM and softmaxShapeInfo.srcK should not be greater than src "
                    "size %u.",
                    softmaxShapeInfo.srcK, softmaxShapeInfo.srcM, src.GetSize());
            });
    }

    template <typename T, bool isReuseSource = false, bool isDataFormatNZ = false>
    __aicore__ inline void PrintParameters(
        const LocalTensor<T>& dst, const LocalTensor<T>& sumTensor, const LocalTensor<T>& maxTensor,
        const LocalTensor<T>& src, const LocalTensor<uint8_t>& sharedTmpBuffer, const LogSoftMaxTiling& tiling,
        const SoftMaxShapeInfo& softmaxShapeInfo = {})
    {
        KERNEL_LOG(
            KERNEL_INFO,
            "[LogSoftMax] The softmaxShapeInfo.srcK is %u, softmaxShapeInfo.srcM is %u, "
            "softmaxShapeInfo.oriSrcM is %u, softmaxShapeInfo.oriSrcK is %u.",
            softmaxShapeInfo.srcK, softmaxShapeInfo.srcM, softmaxShapeInfo.oriSrcM, softmaxShapeInfo.oriSrcK);
    }
};

template <typename T, bool isReuseSource = false, bool isDataFormatNZ = false>
class CheckFuncClassLogSoftMax : public DataTypeCheckFuncBasicClass,
                                 public ReuseSourceCheckFuncBasicClass,
                                 public SingleTensorCheckFuncBasicClass,
                                 public MultipleTensorCheckFuncBasicClass,
                                 public CheckLogSoftMaxParamsClass {
public:
    __aicore__ inline CheckFuncClassLogSoftMax(){};
    __aicore__ inline CheckFuncClassLogSoftMax(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          ReuseSourceCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dst, const LocalTensor<T>& sumTensor, const LocalTensor<T>& maxTensor,
        const LocalTensor<T>& src, const LocalTensor<uint8_t>& sharedTmpBuffer, const LogSoftMaxTiling& tiling,
        const SoftMaxShapeInfo& softmaxShapeInfo = {})
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, half, float>(
            "template parameter (T) is not half or float");

        ReuseSourceCheckFuncBasicClass::IsReuseSourceVerifyingParameters<false>(ARG_AND_STRING(isReuseSource));

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dst, src, sumTensor, maxTensor, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dst, src, sumTensor, maxTensor, sharedTmpBuffer));

        CheckLogSoftMaxParamsClass::CheckLogSoftMaxParams<T, isReuseSource, isDataFormatNZ>(
            dst, sumTensor, maxTensor, src, sharedTmpBuffer, tiling, softmaxShapeInfo);
    };
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_SOFTMAX_LOG_SOFTMAX_LOG_SOFTMAX_CHECK_COMMON_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LOG_SOFTMAX_CHECK_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LOG_SOFTMAX_CHECK_COMMON_H__
#endif
