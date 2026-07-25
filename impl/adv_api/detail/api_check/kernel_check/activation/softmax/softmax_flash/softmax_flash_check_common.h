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
 * \file softmax_flash_check_common.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/activation/softmax/softmax_flash/softmax_flash_check_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/softmaxflash.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASH_CHECK_COMMON_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_SOFTMAX_SOFTMAX_FLASH_SOFTMAX_FLASH_CHECK_COMMON_H_
#define IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_SOFTMAX_SOFTMAX_FLASH_SOFTMAX_FLASH_CHECK_COMMON_H_

#include "../../../basic_check/datatype_check.h"
#include "../../../basic_check/single_tensor_check.h"
#include "../../../basic_check/multiple_tensor_check.h"
#include "../../../../../../../../include/adv_api/activation/softmax_utils.h"

namespace AscendC {
namespace HighLevelApiCheck {
constexpr uint8_t CHECK_SOFTMAXFLASH_K_SIZE = 64;
constexpr uint8_t CHECK_SOFTMAXFLASH_SRCM_SIZE = 8;

class CheckSoftMaxFlashSameTParamsClass {
public:
    template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
    __aicore__ inline void CheckSoftMaxFlashSameTParams(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& sumTensor, const LocalTensor<T>& maxTensor,
        const LocalTensor<T>& srcTensor, const LocalTensor<T>& expMaxTensor, const LocalTensor<T>& inSumTensor,
        const LocalTensor<T>& inMaxTensor, const SoftMaxTiling& tiling, bool isUpdate,
        const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        VerifyingParameters<T, isReuseSource, isBasicBlock>(
            dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor, tiling, isUpdate,
            softmaxShapeInfo);
        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters<T, isReuseSource, isBasicBlock>(
                dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor, tiling, isUpdate,
                softmaxShapeInfo);
        }
    }

private:
    template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& sumTensor, const LocalTensor<T>& maxTensor,
        const LocalTensor<T>& srcTensor, const LocalTensor<T>& expMaxTensor, const LocalTensor<T>& inSumTensor,
        const LocalTensor<T>& inMaxTensor, const SoftMaxTiling& tiling, bool isUpdate,
        const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        ASCENDC_ASSERT((softmaxShapeInfo.srcK * sizeof(T) % ONE_BLK_SIZE == 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[SoftMaxFlash] The softmaxShapeInfo.srcK is %u, should be 32B aligned.",
                softmaxShapeInfo.srcK);
        });
        if (isBasicBlock == true && (softmaxShapeInfo.srcK != 0 || softmaxShapeInfo.srcM != 0)) {
            bool ans = (softmaxShapeInfo.srcK < 2048) && (softmaxShapeInfo.srcK >= DEFAULT_BLOCK_SIZE / sizeof(T)) &&
                       (softmaxShapeInfo.srcK % CHECK_SOFTMAXFLASH_K_SIZE == 0);
            ASCENDC_ASSERT((ans || HighLevelAPIParametersPrint), {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "[SoftMaxFlash] The softmaxShapeInfo.srcK is %u, should be less than 2048 and greater than or "
                    "equal to "
                    "256/sizeof(T), and should be an integer multiple of 64.",
                    softmaxShapeInfo.srcK);
            });
            ASCENDC_ASSERT((softmaxShapeInfo.srcM % CHECK_SOFTMAXFLASH_SRCM_SIZE == 0 || HighLevelAPIParametersPrint), {
                KERNEL_LOG(
                    KERNEL_ERROR, "[SoftMaxFlash] The softmaxShapeInfo.srcM is %u, should be an integer multiple of 8.",
                    softmaxShapeInfo.srcM);
            });
        }
        ASCENDC_ASSERT(
            (softmaxShapeInfo.srcK * softmaxShapeInfo.srcM <= srcTensor.GetSize() || HighLevelAPIParametersPrint), {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "[SoftMaxFlash] The softmaxShapeInfo.srcK is %u, softmaxShapeInfo.srcM is %u, "
                    "the product of softmaxShapeInfo.srcM and softmaxShapeInfo.srcK should not be greater than "
                    "srcTensor size %u.",
                    softmaxShapeInfo.srcK, softmaxShapeInfo.srcM, srcTensor.GetSize());
            });
        ASCENDC_ASSERT((srcTensor.GetSize() == dstTensor.GetSize() || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[SoftMaxFlash] The dstTensor size is %u, should be equal to srcTensor size %u.",
                dstTensor.GetSize(), srcTensor.GetSize());
        });
    }

    template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
    __aicore__ inline void PrintParameters(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& sumTensor, const LocalTensor<T>& maxTensor,
        const LocalTensor<T>& srcTensor, const LocalTensor<T>& expMaxTensor, const LocalTensor<T>& inSumTensor,
        const LocalTensor<T>& inMaxTensor, const SoftMaxTiling& tiling, bool isUpdate,
        const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        KERNEL_LOG(
            KERNEL_INFO,
            "[SoftMaxFlash] The softmaxShapeInfo.srcK is %u, softmaxShapeInfo.srcM is %u, "
            "softmaxShapeInfo.oriSrcM is %u, softmaxShapeInfo.oriSrcK is %u.",
            softmaxShapeInfo.srcK, softmaxShapeInfo.srcM, softmaxShapeInfo.oriSrcM, softmaxShapeInfo.oriSrcK);
    }
};

template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
class CheckFuncClassSoftmaxFlashSameT : public DataTypeCheckFuncBasicClass,
                                        public ReuseSourceCheckFuncBasicClass,
                                        public SingleTensorCheckFuncBasicClass,
                                        public MultipleTensorCheckFuncBasicClass,
                                        public CheckSoftMaxFlashSameTParamsClass {
public:
    __aicore__ inline CheckFuncClassSoftmaxFlashSameT(){};
    __aicore__ inline CheckFuncClassSoftmaxFlashSameT(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          ReuseSourceCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& sumTensor, const LocalTensor<T>& maxTensor,
        const LocalTensor<T>& srcTensor, const LocalTensor<T>& expMaxTensor, const LocalTensor<T>& inSumTensor,
        const LocalTensor<T>& inMaxTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling,
        bool isUpdate, const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, half, float>(
            "template parameter (T) is not half or float");

        ReuseSourceCheckFuncBasicClass::IsReuseSourceVerifyingParameters<false>(ARG_AND_STRING(isReuseSource));

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(
                dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, sumTensor, maxTensor, expMaxTensor, sharedTmpBuffer));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, inSumTensor, maxTensor, expMaxTensor, sharedTmpBuffer));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, sumTensor, inMaxTensor, expMaxTensor, sharedTmpBuffer));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(srcTensor, sumTensor, maxTensor, expMaxTensor, sharedTmpBuffer));
        CheckSoftMaxFlashSameTParamsClass::CheckSoftMaxFlashSameTParams<T, isReuseSource, isBasicBlock>(
            dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor, tiling, isUpdate,
            softmaxShapeInfo);
    };

    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& sumTensor, const LocalTensor<T>& maxTensor,
        const LocalTensor<T>& srcTensor, const LocalTensor<T>& expMaxTensor, const LocalTensor<T>& inSumTensor,
        const LocalTensor<T>& inMaxTensor, const SoftMaxTiling& tiling, bool isUpdate,
        const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, half, float>(
            "template parameter (T) is not half or float");

        ReuseSourceCheckFuncBasicClass::IsReuseSourceVerifyingParameters<false>(ARG_AND_STRING(isReuseSource));

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, sumTensor, maxTensor, expMaxTensor));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, inSumTensor, maxTensor, expMaxTensor));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, sumTensor, inMaxTensor, expMaxTensor));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(srcTensor, sumTensor, maxTensor, expMaxTensor));

        CheckSoftMaxFlashSameTParamsClass::CheckSoftMaxFlashSameTParams<T, isReuseSource, isBasicBlock>(
            dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor, tiling, isUpdate,
            softmaxShapeInfo);
    };
};

class CheckSoftMaxFlashParamsClass {
public:
    template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
    __aicore__ inline void CheckSoftMaxFlashParams(
        const LocalTensor<half>& dstTensor, const LocalTensor<float>& sumTensor, const LocalTensor<float>& maxTensor,
        const LocalTensor<half>& srcTensor, const LocalTensor<half>& expMaxTensor,
        const LocalTensor<float>& inSumTensor, const LocalTensor<float>& inMaxTensor, const SoftMaxTiling& tiling,
        bool isUpdate, const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        VerifyingParameters<T, isReuseSource, isBasicBlock>(
            dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor, tiling, isUpdate,
            softmaxShapeInfo);
        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters<T, isReuseSource, isBasicBlock>(
                dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor, tiling, isUpdate,
                softmaxShapeInfo);
        }
    }

private:
    template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<half>& dstTensor, const LocalTensor<float>& sumTensor, const LocalTensor<float>& maxTensor,
        const LocalTensor<half>& srcTensor, const LocalTensor<half>& expMaxTensor,
        const LocalTensor<float>& inSumTensor, const LocalTensor<float>& inMaxTensor, const SoftMaxTiling& tiling,
        bool isUpdate, const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        ASCENDC_ASSERT((softmaxShapeInfo.srcK * sizeof(T) % ONE_BLK_SIZE == 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[SoftMaxFlash] The softmaxShapeInfo.srcK is %u, should be 32B aligned.",
                softmaxShapeInfo.srcK);
        });
        ASCENDC_ASSERT(
            (softmaxShapeInfo.srcK * softmaxShapeInfo.srcM <= srcTensor.GetSize() || HighLevelAPIParametersPrint), {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "[SoftMaxFlash] The softmaxShapeInfo.srcK is %u, softmaxShapeInfo.srcM is %u, "
                    "the product of softmaxShapeInfo.srcM and softmaxShapeInfo.srcK should not be greater than "
                    "srcTensor size %u.",
                    softmaxShapeInfo.srcK, softmaxShapeInfo.srcM, srcTensor.GetSize());
            });
        ASCENDC_ASSERT((dstTensor.GetSize() == srcTensor.GetSize() || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[SoftMaxFlash] The dstTensor size is %u, should be equal to srcTensor size %u.",
                dstTensor.GetSize(), srcTensor.GetSize());
        });
        if (isBasicBlock == true && (softmaxShapeInfo.srcK != 0 || softmaxShapeInfo.srcM != 0)) {
            bool ans = (softmaxShapeInfo.srcK < 2048) && (softmaxShapeInfo.srcK >= DEFAULT_BLOCK_SIZE / sizeof(T)) &&
                       (softmaxShapeInfo.srcK % CHECK_SOFTMAXFLASH_K_SIZE == 0);
            ASCENDC_ASSERT((ans || HighLevelAPIParametersPrint), {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "[SoftMaxFlash] The softmaxShapeInfo.srcK is %u, should be less than 2048 and greater than or "
                    "equal to "
                    "256/sizeof(T), and should be an integer multiple of 64.",
                    softmaxShapeInfo.srcK);
            });
            ASCENDC_ASSERT((softmaxShapeInfo.srcM % CHECK_SOFTMAXFLASH_SRCM_SIZE == 0 || HighLevelAPIParametersPrint), {
                KERNEL_LOG(
                    KERNEL_ERROR, "[SoftMaxFlash] The softmaxShapeInfo.srcM is %u, should be an integer multiple of 8.",
                    softmaxShapeInfo.srcM);
            });
        }
    }

    template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
    __aicore__ inline void PrintParameters(
        const LocalTensor<half>& dstTensor, const LocalTensor<float>& sumTensor, const LocalTensor<float>& maxTensor,
        const LocalTensor<half>& srcTensor, const LocalTensor<half>& expMaxTensor,
        const LocalTensor<float>& inSumTensor, const LocalTensor<float>& inMaxTensor, const SoftMaxTiling& tiling,
        bool isUpdate, const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        KERNEL_LOG(
            KERNEL_INFO,
            "[SoftMaxFlash] The softmaxShapeInfo.srcK is %u, softmaxShapeInfo.srcM is %u, "
            "softmaxShapeInfo.oriSrcM is %u, softmaxShapeInfo.oriSrcK is %u.",
            softmaxShapeInfo.srcK, softmaxShapeInfo.srcM, softmaxShapeInfo.oriSrcM, softmaxShapeInfo.oriSrcK);
    }
};

template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
class CheckFuncClassSoftmaxFlash : public DataTypeCheckFuncBasicClass,
                                   public SingleTensorCheckFuncBasicClass,
                                   public ReuseSourceCheckFuncBasicClass,
                                   public MultipleTensorCheckFuncBasicClass,
                                   public CheckSoftMaxFlashParamsClass {
public:
    __aicore__ inline CheckFuncClassSoftmaxFlash(){};
    __aicore__ inline CheckFuncClassSoftmaxFlash(__gm__ const char* apiName)
        : ReuseSourceCheckFuncBasicClass(apiName),
          DataTypeCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<half>& dstTensor, const LocalTensor<float>& sumTensor, const LocalTensor<float>& maxTensor,
        const LocalTensor<half>& srcTensor, const LocalTensor<half>& expMaxTensor,
        const LocalTensor<float>& inSumTensor, const LocalTensor<float>& inMaxTensor,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling, bool isUpdate,
        const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        ReuseSourceCheckFuncBasicClass::IsReuseSourceVerifyingParameters<false>(ARG_AND_STRING(isReuseSource));

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(
                dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, inSumTensor, maxTensor, expMaxTensor, sharedTmpBuffer));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, sumTensor, inMaxTensor, expMaxTensor, sharedTmpBuffer));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, sumTensor, expMaxTensor, maxTensor, sharedTmpBuffer));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(srcTensor, sumTensor, maxTensor, expMaxTensor, sharedTmpBuffer));

        CheckSoftMaxFlashParamsClass::CheckSoftMaxFlashParams<T, isReuseSource, isBasicBlock>(
            dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor, tiling, isUpdate,
            softmaxShapeInfo);
    };

    __aicore__ inline void VerifyingParameters(
        const LocalTensor<half>& dstTensor, const LocalTensor<float>& sumTensor, const LocalTensor<float>& maxTensor,
        const LocalTensor<half>& srcTensor, const LocalTensor<half>& expMaxTensor,
        const LocalTensor<float>& inSumTensor, const LocalTensor<float>& inMaxTensor, const SoftMaxTiling& tiling,
        bool isUpdate, const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        ReuseSourceCheckFuncBasicClass::IsReuseSourceVerifyingParameters<false>(ARG_AND_STRING(isReuseSource));

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, sumTensor, maxTensor, expMaxTensor));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, inSumTensor, maxTensor, expMaxTensor));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, inMaxTensor, sumTensor, expMaxTensor));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(srcTensor, sumTensor, maxTensor, expMaxTensor));

        CheckSoftMaxFlashParamsClass::CheckSoftMaxFlashParams<T, isReuseSource, isBasicBlock>(
            dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor, tiling, isUpdate,
            softmaxShapeInfo);
    };
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_SOFTMAX_SOFTMAX_FLASH_SOFTMAX_FLASH_CHECK_COMMON_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASH_CHECK_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASH_CHECK_COMMON_H__
#endif
