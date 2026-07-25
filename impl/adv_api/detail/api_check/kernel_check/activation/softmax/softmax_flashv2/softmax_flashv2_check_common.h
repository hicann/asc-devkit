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
 * \file softmax_flashv2_check_common.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/activation/softmax/softmax_flashv2/softmax_flashv2_check_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/softmaxflashv2.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASHV2_CHECK_COMMON_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_SOFTMAX_SOFTMAX_FLASHV2_SOFTMAX_FLASHV2_CHECK_COMMON_H_
#define IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_SOFTMAX_SOFTMAX_FLASHV2_SOFTMAX_FLASHV2_CHECK_COMMON_H_

#include "../../../basic_check/datatype_check.h"
#include "../../../basic_check/reuse_source_check.h"
#include "../../../basic_check/single_tensor_check.h"
#include "../../../basic_check/multiple_tensor_check.h"
#include "../../../../../../../../include/adv_api/activation/softmax_utils.h"

namespace AscendC {
namespace HighLevelApiCheck {
constexpr uint8_t CHECK_SOFTMAXFLASHV2_SRCM_SIZE = 8;
constexpr uint8_t CHECK_SOFTMAXFLASHV2_K_SIZE = 64;

class CheckSoftMaxFlashV2ParamsClass {
public:
    __aicore__ inline CheckSoftMaxFlashV2ParamsClass(){};
    __aicore__ inline CheckSoftMaxFlashV2ParamsClass(__gm__ const char* apiName) { this->apiName = apiName; };

public:
    template <
        typename T1, typename T2, bool isUpdate, bool isReuseSource, bool isBasicBlock, bool isDataFormatNZ,
        const SoftmaxConfig& config>
    __aicore__ inline void CheckSoftMaxFlashV2Params(
        const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& expSumTensor, const LocalTensor<T2>& maxTensor,
        const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& expMaxTensor, const LocalTensor<T2>& inExpSumTensor,
        const LocalTensor<T2>& inMaxTensor, const LocalTensor<float>& sharedTmpBuffer, const SoftMaxTiling& tiling,
        const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        VerifyingParameters<T1, T2, isUpdate, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
            dstTensor, expSumTensor, maxTensor, srcTensor, expMaxTensor, inExpSumTensor, inMaxTensor, sharedTmpBuffer,
            tiling, softmaxShapeInfo);

        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters<T1, T2, isUpdate, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
                dstTensor, expSumTensor, maxTensor, srcTensor, expMaxTensor, inExpSumTensor, inMaxTensor,
                sharedTmpBuffer, tiling, softmaxShapeInfo);
        }
    }

private:
    template <
        typename T1, typename T2, bool isUpdate, bool isReuseSource, bool isBasicBlock, bool isDataFormatNZ,
        const SoftmaxConfig& config>
    __aicore__ inline void VerifyingParametersCommonVersion(
        const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& expSumTensor, const LocalTensor<T2>& maxTensor,
        const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& expMaxTensor, const LocalTensor<T2>& inExpSumTensor,
        const LocalTensor<T2>& inMaxTensor, const LocalTensor<float>& sharedTmpBuffer, const SoftMaxTiling& tiling,
        const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        ASCENDC_ASSERT((softmaxShapeInfo.srcK * sizeof(T1) % ONE_BLK_SIZE == 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[%s] The softmaxShapeInfo.srcK is %u, should be 32B aligned.", apiName,
                softmaxShapeInfo.srcK);
        });
        ASCENDC_ASSERT(
            (softmaxShapeInfo.srcK * softmaxShapeInfo.srcM <= srcTensor.GetSize() || HighLevelAPIParametersPrint), {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "[%s] The softmaxShapeInfo.srcK is %u, softmaxShapeInfo.srcM is %u, "
                    "the product of softmaxShapeInfo.srcM and softmaxShapeInfo.srcK should not be greater than "
                    "srcTensor size %u.",
                    apiName, softmaxShapeInfo.srcK, softmaxShapeInfo.srcM, srcTensor.GetSize());
            });
        ASCENDC_ASSERT((dstTensor.GetSize() == srcTensor.GetSize() || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[%s] The dstTensor size is %u, should be equal to srcTensor size %u.", apiName,
                dstTensor.GetSize(), srcTensor.GetSize());
        });
    }

    template <
        typename T1, typename T2, bool isUpdate, bool isReuseSource, bool isBasicBlock, bool isDataFormatNZ,
        const SoftmaxConfig& config>
    __aicore__ inline void VerifyingParametersVersion300(
        const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& expSumTensor, const LocalTensor<T2>& maxTensor,
        const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& expMaxTensor, const LocalTensor<T2>& inExpSumTensor,
        const LocalTensor<T2>& inMaxTensor, const LocalTensor<float>& sharedTmpBuffer, const SoftMaxTiling& tiling,
        const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        ASCENDC_LOG_IF_CHECK((isBasicBlock == false || HighLevelAPIParametersPrint), {
            KERNEL_LOG(KERNEL_WARN, "[%s] The isBasicBlock is true, may not be effective in this device.", apiName);
        });
        ASCENDC_LOG_IF_CHECK((isDataFormatNZ == false || HighLevelAPIParametersPrint), {
            KERNEL_LOG(KERNEL_WARN, "[%s] The isDataFormatNZ is true, may not be effective in this device.", apiName);
        });

        bool ans = config.isCheckTiling == true && config.oriSrcM == 0 && config.oriSrcK == 0 &&
                   config.mode == SoftmaxMode::SOFTMAX_NORMAL;
        ASCENDC_LOG_IF_CHECK((ans || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_WARN,
                "[%s] The config.isCheckTiling is %d, config.oriSrcM is %u, "
                "config.oriSrcK is %u, config.mode is %d, should remain at the default value in this device.",
                apiName, config.isCheckTiling, config.oriSrcM, config.oriSrcK, config.mode);
        });
    }

    template <
        typename T1, typename T2, bool isUpdate, bool isReuseSource, bool isBasicBlock, bool isDataFormatNZ,
        const SoftmaxConfig& config>
    __aicore__ inline void VerifyingParametersVersion200(
        const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& expSumTensor, const LocalTensor<T2>& maxTensor,
        const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& expMaxTensor, const LocalTensor<T2>& inExpSumTensor,
        const LocalTensor<T2>& inMaxTensor, const LocalTensor<float>& sharedTmpBuffer, const SoftMaxTiling& tiling,
        const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        ASCENDC_LOG_IF_CHECK((config.mode == SoftmaxMode::SOFTMAX_NORMAL || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_WARN, "[%s] The config.mode is %d, should be default value SoftmaxMode::SOFTMAX_NORMAL.",
                apiName, config.mode);
        });
    }

    template <
        typename T1, typename T2, bool isUpdate, bool isReuseSource, bool isBasicBlock, bool isDataFormatNZ,
        const SoftmaxConfig& config>
    __aicore__ inline void VerifyingParametersVersion220(
        const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& expSumTensor, const LocalTensor<T2>& maxTensor,
        const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& expMaxTensor, const LocalTensor<T2>& inExpSumTensor,
        const LocalTensor<T2>& inMaxTensor, const LocalTensor<float>& sharedTmpBuffer, const SoftMaxTiling& tiling,
        const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        if (isBasicBlock == true && (softmaxShapeInfo.srcK != 0 || softmaxShapeInfo.srcM != 0)) {
            bool ans = (softmaxShapeInfo.srcK < 2048) && (softmaxShapeInfo.srcK >= DEFAULT_BLOCK_SIZE / sizeof(T1)) &&
                       (softmaxShapeInfo.srcK % CHECK_SOFTMAXFLASHV2_K_SIZE == 0);
            ASCENDC_ASSERT((ans || HighLevelAPIParametersPrint), {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "[%s] The softmaxShapeInfo.srcK is %u, should be less than 2048 and greater than or equal to "
                    "256/sizeof(T), and should be an integer multiple of 64.",
                    apiName, softmaxShapeInfo.srcK);
            });
            ASCENDC_ASSERT(
                (softmaxShapeInfo.srcM % CHECK_SOFTMAXFLASHV2_SRCM_SIZE == 0 || HighLevelAPIParametersPrint), {
                    KERNEL_LOG(
                        KERNEL_ERROR, "[%s] The softmaxShapeInfo.srcM is %u, should be an integer multiple of 8.",
                        apiName, softmaxShapeInfo.srcM);
                });
        }
    }

    template <
        typename T1, typename T2, bool isUpdate, bool isReuseSource, bool isBasicBlock, bool isDataFormatNZ,
        const SoftmaxConfig& config, typename... Args>
    __aicore__ inline void VerifyingParameters(Args... args)
    {
        VerifyingParametersCommonVersion<T1, T2, isUpdate, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
            args...);
        if constexpr (__NPU_ARCH__ == 3002) {
            VerifyingParametersVersion300<T1, T2, isUpdate, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
                args...);
        } else if constexpr (__NPU_ARCH__ == 2002) {
            VerifyingParametersVersion200<T1, T2, isUpdate, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
                args...);
            VerifyingParametersVersion220<T1, T2, isUpdate, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
                args...);
        } else if constexpr (__NPU_ARCH__ == 2201) {
            VerifyingParametersVersion220<T1, T2, isUpdate, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
                args...);
        }
    }

    template <
        typename T1, typename T2, bool isUpdate, bool isReuseSource, bool isBasicBlock, bool isDataFormatNZ,
        const SoftmaxConfig& config>
    __aicore__ inline void PrintParameters(
        const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& expSumTensor, const LocalTensor<T2>& maxTensor,
        const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& expMaxTensor, const LocalTensor<T2>& inExpSumTensor,
        const LocalTensor<T2>& inMaxTensor, const LocalTensor<float>& sharedTmpBuffer, const SoftMaxTiling& tiling,
        const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        KERNEL_LOG(
            KERNEL_INFO,
            "[%s] The softmaxShapeInfo.srcK is %u, softmaxShapeInfo.srcM is %u, "
            "softmaxShapeInfo.oriSrcM is %u, softmaxShapeInfo.oriSrcK is %u.",
            apiName, softmaxShapeInfo.srcK, softmaxShapeInfo.srcM, softmaxShapeInfo.oriSrcM, softmaxShapeInfo.oriSrcK);
    }

private:
    __gm__ const char* apiName = nullptr;
};

template <
    typename T1, typename T2, bool isUpdate, bool isReuseSource, bool isBasicBlock, bool isDataFormatNZ,
    const SoftmaxConfig& config>
class CheckFuncClassSoftmaxFlashV2 : public DataTypeCheckFuncBasicClass,
                                     public CheckSoftMaxFlashV2ParamsClass,
                                     public ReuseSourceCheckFuncBasicClass,
                                     public SingleTensorCheckFuncBasicClass,
                                     public MultipleTensorCheckFuncBasicClass {
public:
    __aicore__ inline CheckFuncClassSoftmaxFlashV2(){};
    __aicore__ inline CheckFuncClassSoftmaxFlashV2(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          ReuseSourceCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& expSumTensor, const LocalTensor<T2>& maxTensor,
        const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& expMaxTensor, const LocalTensor<T2>& inExpSumTensor,
        const LocalTensor<T2>& inMaxTensor, const LocalTensor<float>& sharedTmpBuffer, const SoftMaxTiling& tiling,
        const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T1, half, float>(
            "template parameter (T) is not half or float");

        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T2, half, float>(
            "template parameter (T) is not half or float");

        ReuseSourceCheckFuncBasicClass::IsReuseSourceVerifyingParameters<false>(ARG_AND_STRING(isReuseSource));

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(
                dstTensor, expSumTensor, maxTensor, srcTensor, expMaxTensor, inExpSumTensor, inMaxTensor,
                sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        CheckSoftMaxFlashV2ParamsClass::CheckSoftMaxFlashV2Params<
            T1, T2, isUpdate, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
            dstTensor, expSumTensor, maxTensor, srcTensor, expMaxTensor, inExpSumTensor, inMaxTensor, sharedTmpBuffer,
            tiling, softmaxShapeInfo);
    };

    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& outReduceMax, const LocalTensor<T2>& outSum,
        const LocalTensor<T2>& outMax, const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& outexpMax,
        const LocalTensor<T2>& inSum, const LocalTensor<T2>& inMax, const LocalTensor<float>& sharedTmpBuffer,
        const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        VerifyingParameters(
            dstTensor, outSum, outMax, srcTensor, outexpMax, inSum, inMax, sharedTmpBuffer, tiling, softmaxShapeInfo);

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(outReduceMax),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, outSum, outMax, outexpMax, outReduceMax, sharedTmpBuffer));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, inSum, outMax, outexpMax, outReduceMax, sharedTmpBuffer));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, outSum, inMax, outexpMax, outReduceMax, sharedTmpBuffer));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(srcTensor, outSum, outMax, outexpMax, outReduceMax, sharedTmpBuffer));
    };
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_SOFTMAX_SOFTMAX_FLASHV2_SOFTMAX_FLASHV2_CHECK_COMMON_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASHV2_CHECK_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASHV2_CHECK_COMMON_H__
#endif
