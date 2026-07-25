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
 * \file softmax_check_common.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/activation/softmax/softmax/softmax_check_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/softmax.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_CHECK_COMMON_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_SOFTMAX_SOFTMAX_SOFTMAX_CHECK_COMMON_H_
#define IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_SOFTMAX_SOFTMAX_SOFTMAX_CHECK_COMMON_H_

#include "../../../basic_check/datatype_check.h"
#include "../../../basic_check/reuse_source_check.h"
#include "../../../basic_check/single_tensor_check.h"
#include "../../../basic_check/multiple_tensor_check.h"
#include "../../../../../../../../include/adv_api/activation/softmax_utils.h"

namespace AscendC {
namespace HighLevelApiCheck {
constexpr uint8_t CHECK_SOFTMAX_K_SIZE = 64;
constexpr uint8_t CHECK_SOFTMAX_SRCM_SIZE = 8;

class CheckSoftMaxParamsClass {
public:
    template <
        typename T1, typename T2, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false,
        const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
    __aicore__ inline void CheckSoftMaxParams(
        const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& sumTensor, const LocalTensor<T2>& maxTensor,
        const LocalTensor<T1>& srcTensor, const LocalTensor<float>& sharedTmpBuffer, const SoftMaxTiling& tiling,
        const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        VerifyingParameters<T1, T2, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
            dstTensor, sumTensor, maxTensor, srcTensor, sharedTmpBuffer, tiling, softmaxShapeInfo);
        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters<T1, T2, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
                dstTensor, sumTensor, maxTensor, srcTensor, sharedTmpBuffer, tiling, softmaxShapeInfo);
        }
    }

private:
    template <
        typename T1, typename T2, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false,
        const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& sumTensor, const LocalTensor<T2>& maxTensor,
        const LocalTensor<T1>& srcTensor, const LocalTensor<float>& sharedTmpBuffer, const SoftMaxTiling& tiling,
        const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        ASCENDC_ASSERT((softmaxShapeInfo.srcK * sizeof(T1) % ONE_BLK_SIZE == 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[SoftMax] The softmaxShapeInfo.srcK is %u, should be 32B aligned.",
                softmaxShapeInfo.srcK);
        });
        ASCENDC_ASSERT(
            (softmaxShapeInfo.srcK * softmaxShapeInfo.srcM <= srcTensor.GetSize() || HighLevelAPIParametersPrint), {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "[SoftMax] The softmaxShapeInfo.srcK is %u, softmaxShapeInfo.srcM is %u, "
                    "the product of softmaxShapeInfo.srcM and softmaxShapeInfo.srcK should not be greater than "
                    "srcTensor size %u.",
                    softmaxShapeInfo.srcK, softmaxShapeInfo.srcM, srcTensor.GetSize());
            });
        ASCENDC_ASSERT((dstTensor.GetSize() == srcTensor.GetSize() || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[SoftMax] The dstTensor size is %u, should be equal to srcTensor size %u.",
                dstTensor.GetSize(), srcTensor.GetSize());
        });
#if __NPU_ARCH__ == 3002
        ASCENDC_LOG_IF_CHECK((isBasicBlock == false || HighLevelAPIParametersPrint), {
            KERNEL_LOG(KERNEL_WARN, "[SoftMax] The isBasicBlock is true, may not be effective in this device.");
        });
        ASCENDC_LOG_IF_CHECK((isDataFormatNZ == false || HighLevelAPIParametersPrint), {
            KERNEL_LOG(KERNEL_WARN, "[SoftMax] The isDataFormatNZ is true, may not be effective in this device.");
        });
#endif
#if __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3002
        bool ans = config.isCheckTiling == true && config.oriSrcM == 0 && config.oriSrcK == 0;
        ASCENDC_LOG_IF_CHECK((ans || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_WARN,
                "[SoftMax] The config.isCheckTiling is %d, config.oriSrcM is %u, "
                "config.oriSrcK is %u, should remain at the default value in this device.",
                config.isCheckTiling, config.oriSrcM, config.oriSrcK);
        });
#endif
#if __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201
        if (isBasicBlock == true && (softmaxShapeInfo.srcK != 0 || softmaxShapeInfo.srcM != 0)) {
            bool ans = (softmaxShapeInfo.srcK < 2048) && (softmaxShapeInfo.srcK >= DEFAULT_BLOCK_SIZE / sizeof(T1)) &&
                       (softmaxShapeInfo.srcK % CHECK_SOFTMAX_K_SIZE == 0);
            ASCENDC_ASSERT((ans || HighLevelAPIParametersPrint), {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "[SoftMax] The softmaxShapeInfo.srcK is %u, should be less than 2048 and greater than or equal to "
                    "256/sizeof(T), and should be an integer multiple of 64.",
                    softmaxShapeInfo.srcK);
            });
            ASCENDC_ASSERT((softmaxShapeInfo.srcM % CHECK_SOFTMAX_SRCM_SIZE == 0 || HighLevelAPIParametersPrint), {
                KERNEL_LOG(
                    KERNEL_ERROR, "[SoftMax] The softmaxShapeInfo.srcM is %u, should be an integer multiple of 8.",
                    softmaxShapeInfo.srcM);
            });
        }
#endif
    }

    template <
        typename T1, typename T2, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false,
        const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
    __aicore__ inline void PrintParameters(
        const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& sumTensor, const LocalTensor<T2>& maxTensor,
        const LocalTensor<T1>& srcTensor, const LocalTensor<float>& sharedTmpBuffer, const SoftMaxTiling& tiling,
        const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        KERNEL_LOG(
            KERNEL_INFO,
            "[SoftMax] The softmaxShapeInfo.srcK is %u, softmaxShapeInfo.srcM is %u, "
            "softmaxShapeInfo.oriSrcM is %u, softmaxShapeInfo.oriSrcK is %u.",
            softmaxShapeInfo.srcK, softmaxShapeInfo.srcM, softmaxShapeInfo.oriSrcM, softmaxShapeInfo.oriSrcK);
    }
};

template <
    typename T1, typename T2, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false,
    const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
class CheckFuncClassSoftMax : public DataTypeCheckFuncBasicClass,
                              public ReuseSourceCheckFuncBasicClass,
                              public SingleTensorCheckFuncBasicClass,
                              public MultipleTensorCheckFuncBasicClass,
                              public CheckSoftMaxParamsClass {
public:
    __aicore__ inline CheckFuncClassSoftMax(){};
    __aicore__ inline CheckFuncClassSoftMax(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          ReuseSourceCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& sumTensor, const LocalTensor<T2>& maxTensor,
        const LocalTensor<T1>& srcTensor, const LocalTensor<float>& sharedTmpBuffer, const SoftMaxTiling& tiling,
        const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T1, half, float>("dstTensor is not half or float");

        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T2, half, float>("sumTensor is not half or float");

        ReuseSourceCheckFuncBasicClass::IsReuseSourceVerifyingParameters<false>(ARG_AND_STRING(isReuseSource));

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor, sumTensor, maxTensor, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, sumTensor, maxTensor));

        CheckSoftMaxParamsClass::CheckSoftMaxParams<T1, T2, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
            dstTensor, sumTensor, maxTensor, srcTensor, sharedTmpBuffer, tiling, softmaxShapeInfo);
    };
};

class CheckSoftMaxSameTParamsClass {
public:
    template <
        typename T, bool isReuseSource = false, bool isBasicBlock = false,
        const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
    __aicore__ inline void CheckSoftMaxSameTParams(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<float>& sharedTmpBuffer,
        const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        VerifyingParameters<T, isReuseSource, isBasicBlock, config>(
            dstTensor, srcTensor, sharedTmpBuffer, tiling, softmaxShapeInfo);
        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters<T, isReuseSource, isBasicBlock, config>(
                dstTensor, srcTensor, sharedTmpBuffer, tiling, softmaxShapeInfo);
        }
    }

private:
    template <
        typename T, bool isReuseSource = false, bool isBasicBlock = false,
        const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<float>& sharedTmpBuffer,
        const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        ASCENDC_ASSERT((softmaxShapeInfo.srcK * sizeof(T) % ONE_BLK_SIZE == 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[SoftMax] The softmaxShapeInfo.srcK is %u, should be 32B aligned.",
                softmaxShapeInfo.srcK);
        });
        ASCENDC_ASSERT(
            (softmaxShapeInfo.srcK * softmaxShapeInfo.srcM <= srcTensor.GetSize() || HighLevelAPIParametersPrint), {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "[SoftMax] The softmaxShapeInfo.srcK is %u, softmaxShapeInfo.srcM is %u, "
                    "the product of softmaxShapeInfo.srcM and softmaxShapeInfo.srcK should not be greater than "
                    "srcTensor size %u.",
                    softmaxShapeInfo.srcK, softmaxShapeInfo.srcM, srcTensor.GetSize());
            });
        ASCENDC_ASSERT((dstTensor.GetSize() == srcTensor.GetSize() || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[SoftMax] The dstTensor size is %u, should be equal to srcTensor size %u.",
                dstTensor.GetSize(), srcTensor.GetSize());
        });
#if __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3002
        bool ans = config.isCheckTiling == true && config.oriSrcM == 0 && config.oriSrcK == 0;
        ASCENDC_LOG_IF_CHECK((ans || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_WARN,
                "[SoftMax] The config.isCheckTiling is %d, config.oriSrcM is %u, "
                "config.oriSrcK is %u, should remain at the default value in this device.",
                config.isCheckTiling, config.oriSrcM, config.oriSrcK);
        });
#endif
#if __NPU_ARCH__ == 3002
        ASCENDC_LOG_IF_CHECK((isBasicBlock == false || HighLevelAPIParametersPrint), {
            KERNEL_LOG(KERNEL_WARN, "[SoftMax] The isBasicBlock is true, may not be effective in this device.");
        });
#endif
#if __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201
        if (isBasicBlock == true && (softmaxShapeInfo.srcK != 0 || softmaxShapeInfo.srcM != 0)) {
            ASCENDC_ASSERT((softmaxShapeInfo.srcM % CHECK_SOFTMAX_SRCM_SIZE == 0 || HighLevelAPIParametersPrint), {
                KERNEL_LOG(
                    KERNEL_ERROR, "[SoftMax] The softmaxShapeInfo.srcM is %u, should be an integer multiple of 8.",
                    softmaxShapeInfo.srcM);
            });
            bool ans = (softmaxShapeInfo.srcK < 2048) && (softmaxShapeInfo.srcK >= DEFAULT_BLOCK_SIZE / sizeof(T)) &&
                       (softmaxShapeInfo.srcK % CHECK_SOFTMAX_K_SIZE == 0);
            ASCENDC_ASSERT((ans || HighLevelAPIParametersPrint), {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "[SoftMax] The softmaxShapeInfo.srcK is %u, should be less than 2048 and greater than or equal to "
                    "256/sizeof(T), and should be an integer multiple of 64.",
                    softmaxShapeInfo.srcK);
            });
        }
#endif
    }

    template <
        typename T, bool isReuseSource = false, bool isBasicBlock = false,
        const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
    __aicore__ inline void PrintParameters(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<float>& sharedTmpBuffer,
        const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        KERNEL_LOG(
            KERNEL_INFO,
            "[SoftMax] The softmaxShapeInfo.srcK is %u, softmaxShapeInfo.srcM is %u, "
            "softmaxShapeInfo.oriSrcM is %u, softmaxShapeInfo.oriSrcK is %u.",
            softmaxShapeInfo.srcK, softmaxShapeInfo.srcM, softmaxShapeInfo.oriSrcM, softmaxShapeInfo.oriSrcK);
    }
};

template <
    typename T, bool isReuseSource = false, bool isBasicBlock = false,
    const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
class CheckFuncClassSoftMaxSameT : public DataTypeCheckFuncBasicClass,
                                   public ReuseSourceCheckFuncBasicClass,
                                   public SingleTensorCheckFuncBasicClass,
                                   public MultipleTensorCheckFuncBasicClass,
                                   public CheckSoftMaxSameTParamsClass {
public:
    __aicore__ inline CheckFuncClassSoftMaxSameT(){};
    __aicore__ inline CheckFuncClassSoftMaxSameT(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          ReuseSourceCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<float>& sharedTmpBuffer,
        const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, half, float>(
            "template parameter (T) is not half or float");

        ReuseSourceCheckFuncBasicClass::IsReuseSourceVerifyingParameters<false>(ARG_AND_STRING(isReuseSource));

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        CheckSoftMaxSameTParamsClass::CheckSoftMaxSameTParams<T, isReuseSource, isBasicBlock, config>(
            dstTensor, srcTensor, sharedTmpBuffer, tiling, softmaxShapeInfo);
    };
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_SOFTMAX_SOFTMAX_SOFTMAX_CHECK_COMMON_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_CHECK_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_CHECK_COMMON_H__
#endif
