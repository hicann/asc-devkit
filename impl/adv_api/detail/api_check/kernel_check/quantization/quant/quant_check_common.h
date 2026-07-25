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
 * \file quant_check_common.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/quantization/quant/quant_check_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/quantization/quantize.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANT_CHECK_COMMON_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_QUANTIZATION_QUANT_QUANT_CHECK_COMMON_H_
#define IMPL_API_CHECK_KERNEL_CHECK_QUANTIZATION_QUANT_QUANT_CHECK_COMMON_H_

#include "../../basic_check/datatype_check.h"
#include "../../basic_check/calcount_check.h"
#include "../../basic_check/reuse_source_check.h"
#include "../../basic_check/single_tensor_check.h"
#include "../../basic_check/multiple_tensor_check.h"
#include "../../../../../../../include/adv_api/quantization/ascend_quant_utils.h"

namespace AscendC {
namespace HighLevelApiCheck {

template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
class CheckFuncClassAscendQuantTensor : public DataTypeCheckFuncBasicClass,
                                        public CalCountCheckFuncBasicClass,
                                        public ReuseSourceCheckFuncBasicClass,
                                        public SingleTensorCheckFuncBasicClass,
                                        public MultipleTensorCheckFuncBasicClass {
public:
    __aicore__ inline CheckFuncClassAscendQuantTensor(){};
    __aicore__ inline CheckFuncClassAscendQuantTensor(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          CalCountCheckFuncBasicClass(apiName),
          ReuseSourceCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const float scale, const float offset, const uint32_t calCount)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, half, float>(
            "template parameter (T) is not half or float");

        ReuseSourceCheckFuncBasicClass::IsReuseSourceVerifyingParameters<false>(ARG_AND_STRING(isReuseSource));

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECCALC, TPosition::VECOUT), "UB(VECIN/VECCALC/VECOUT)");

        CalCountCheckFuncBasicClass::CalCountVerifyingParameters(
            ARG_AND_STRING(calCount), VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor));
        CalCountCheckFuncBasicClass::CalCountVerifyingParameters(
            ARG_AND_STRING(config.calcCount), VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor));
    };
};

class CheckAscendQuantChannelOffsetParamsClass {
public:
    template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
    __aicore__ inline void CheckAscendQuantChannelOffsetParams(
        const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const LocalTensor<T>& scaleTensor, const T offset,
        const uint32_t scaleCount, const uint32_t calCount)
    {
        VerifyingParameters<T, isReuseSource, config>(
            dstTensor, srcTensor, sharedTmpBuffer, scaleTensor, offset, scaleCount, calCount);
        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters<T, isReuseSource, config>(
                dstTensor, srcTensor, sharedTmpBuffer, scaleTensor, offset, scaleCount, calCount);
        }
    }

private:
    template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const LocalTensor<T>& scaleTensor, const T offset,
        const uint32_t scaleCount, const uint32_t calCount)
    {
        bool ans = scaleCount == 0 || calCount % scaleCount == 0;
        ASCENDC_ASSERT((ans || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[AscendQuant] The calCount is %u, should be an integer multiple of scaleCount %u.",
                calCount, scaleCount);
        });
        ans = scaleCount * sizeof(T) % ONE_BLK_SIZE == 0;
        ASCENDC_ASSERT((ans || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[AscendQuant] The result of scaleCount * sizeof(T) is %u, should be an integer multiple of 32.",
                scaleCount * sizeof(T));
        });
        ans = config.scaleCount == 0 || config.calcCount % config.scaleCount == 0;
        ASCENDC_ASSERT((ans || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[AscendQuant] The config.calcCount is %u, should be an integer multiple of config.scaleCount %u.",
                config.calcCount, config.scaleCount);
        });
        ans = config.scaleCount * sizeof(T) % ONE_BLK_SIZE == 0;
        ASCENDC_ASSERT((ans || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[AscendQuant] The result of config.scaleCount * sizeof(T) is %u, should be an integer multiple of 32.",
                config.scaleCount * sizeof(T));
        });
    }

    template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
    __aicore__ inline void PrintParameters(
        const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const LocalTensor<T>& scaleTensor, const T offset,
        const uint32_t scaleCount, const uint32_t calCount)
    {
        KERNEL_LOG(KERNEL_INFO, "[AscendQuant] The calCount is %u, scaleCount is %u.", calCount, scaleCount);
        KERNEL_LOG(
            KERNEL_INFO, "[AscendQuant] The config.calcCount is %u, config.scaleCount is %u, config.offsetCount is %u.",
            config.calcCount, config.scaleCount, config.offsetCount);
    }
};

template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
class CheckFuncClassAscendQuantChannelOffset : public DataTypeCheckFuncBasicClass,
                                               public CalCountCheckFuncBasicClass,
                                               public ReuseSourceCheckFuncBasicClass,
                                               public SingleTensorCheckFuncBasicClass,
                                               public MultipleTensorCheckFuncBasicClass,
                                               public CheckAscendQuantChannelOffsetParamsClass {
public:
    __aicore__ inline CheckFuncClassAscendQuantChannelOffset(){};
    __aicore__ inline CheckFuncClassAscendQuantChannelOffset(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          CalCountCheckFuncBasicClass(apiName),
          ReuseSourceCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const LocalTensor<T>& scaleTensor, const T offset,
        const uint32_t scaleCount, const uint32_t calCount)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, half, float>(
            "template parameter (T) is not half or float");

        ReuseSourceCheckFuncBasicClass::IsReuseSourceVerifyingParameters<false>(ARG_AND_STRING(isReuseSource));

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor, sharedTmpBuffer, scaleTensor),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        CalCountCheckFuncBasicClass::CalCountVerifyingParameters(
            ARG_AND_STRING(calCount), VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor));
        CalCountCheckFuncBasicClass::CalCountVerifyingParameters(
            ARG_AND_STRING(scaleCount), VA_ARGS_TO_MAKE_TUPLE(dstTensor, scaleTensor));
        CalCountCheckFuncBasicClass::CalCountVerifyingParameters(
            ARG_AND_STRING(config.calcCount), VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor));
        CalCountCheckFuncBasicClass::CalCountVerifyingParameters(
            ARG_AND_STRING(config.scaleCount), VA_ARGS_TO_MAKE_TUPLE(dstTensor, scaleTensor));

        CheckAscendQuantChannelOffsetParamsClass::CheckAscendQuantChannelOffsetParams<T, isReuseSource, config>(
            dstTensor, srcTensor, sharedTmpBuffer, scaleTensor, offset, scaleCount, calCount);
    };
};

class CheckAscendQuantChannelOffsetTensorParamsClass {
public:
    template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
    __aicore__ inline void CheckAscendQuantChannelOffsetTensorParams(
        const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const LocalTensor<T>& scaleTensor,
        const LocalTensor<T>& offsetTensor, const uint32_t scaleCount, const uint32_t offsetCount,
        const uint32_t calCount)
    {
        VerifyingParameters<T, isReuseSource, config>(
            dstTensor, srcTensor, sharedTmpBuffer, scaleTensor, offsetTensor, scaleCount, offsetCount, calCount);
        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters<T, isReuseSource, config>(
                dstTensor, srcTensor, sharedTmpBuffer, scaleTensor, offsetTensor, scaleCount, offsetCount, calCount);
        }
    }

private:
    template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const LocalTensor<T>& scaleTensor,
        const LocalTensor<T>& offsetTensor, const uint32_t scaleCount, const uint32_t offsetCount,
        const uint32_t calCount)
    {
        bool ans = scaleCount == 0 || calCount % scaleCount == 0;
        ASCENDC_ASSERT((ans || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[AscendQuant] The calCount is %u, should be an integer multiple of scaleCount %u.",
                calCount, scaleCount);
        });
        ans = scaleCount * sizeof(T) % ONE_BLK_SIZE == 0;
        ASCENDC_ASSERT((ans || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[AscendQuant] The result of scaleCount * sizeof(T) is %lu, should be an integer multiple of 32.",
                scaleCount * sizeof(T));
        });
        ans = scaleCount == offsetCount;
        ASCENDC_ASSERT((ans || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[AscendQuant] The scaleCount is %u, should be equal to offsetCount %u.", scaleCount,
                offsetCount);
        });
        ans = config.scaleCount == 0 || config.calcCount % config.scaleCount == 0;
        ASCENDC_ASSERT((ans || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[AscendQuant] The config.calcCount is %u, should be an integer multiple of config.scaleCount %u.",
                config.calcCount, config.scaleCount);
        });
        ans = config.scaleCount * sizeof(T) % ONE_BLK_SIZE == 0;
        ASCENDC_ASSERT((ans || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[AscendQuant] The result of config.scaleCount * sizeof(T) is %lu, should be an integer multiple of "
                "32.",
                config.scaleCount * sizeof(T));
        });
        ans = config.scaleCount == config.offsetCount;
        ASCENDC_ASSERT((ans || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[AscendQuant] The config.scaleCount is %u, should be equal to config.offsetCount %u.",
                config.scaleCount, config.offsetCount);
        });
    }

    template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
    __aicore__ inline void PrintParameters(
        const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const LocalTensor<T>& scaleTensor,
        const LocalTensor<T>& offsetTensor, const uint32_t scaleCount, const uint32_t offsetCount,
        const uint32_t calCount)
    {
        KERNEL_LOG(
            KERNEL_INFO, "[AscendQuant] The calCount is %u, scaleCount is %u, offsetCount is %u.", calCount, scaleCount,
            offsetCount);
        KERNEL_LOG(
            KERNEL_INFO, "[AscendQuant] The config.calcCount is %u, config.scaleCount is %u, config.offsetCount is %u.",
            config.calcCount, config.scaleCount, config.offsetCount);
    }
};

template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
class CheckFuncClassAscendQuantChannelOffsetTensor : public DataTypeCheckFuncBasicClass,
                                                     public CalCountCheckFuncBasicClass,
                                                     public ReuseSourceCheckFuncBasicClass,
                                                     public SingleTensorCheckFuncBasicClass,
                                                     public MultipleTensorCheckFuncBasicClass,
                                                     public CheckAscendQuantChannelOffsetTensorParamsClass {
public:
    __aicore__ inline CheckFuncClassAscendQuantChannelOffsetTensor(){};
    __aicore__ inline CheckFuncClassAscendQuantChannelOffsetTensor(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          CalCountCheckFuncBasicClass(apiName),
          ReuseSourceCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const LocalTensor<T>& scaleTensor,
        const LocalTensor<T>& offsetTensor, const uint32_t scaleCount, const uint32_t offsetCount,
        const uint32_t calCount)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, half, float>(
            "template parameter (T) is not half or float");

        ReuseSourceCheckFuncBasicClass::IsReuseSourceVerifyingParameters<false>(ARG_AND_STRING(isReuseSource));

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor, sharedTmpBuffer, scaleTensor, offsetTensor),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        CalCountCheckFuncBasicClass::CalCountVerifyingParameters(
            ARG_AND_STRING(calCount), VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor));
        CalCountCheckFuncBasicClass::CalCountVerifyingParameters(
            ARG_AND_STRING(scaleCount), VA_ARGS_TO_MAKE_TUPLE(dstTensor, scaleTensor));
        CalCountCheckFuncBasicClass::CalCountVerifyingParameters(
            ARG_AND_STRING(offsetCount), VA_ARGS_TO_MAKE_TUPLE(dstTensor, offsetTensor));
        CalCountCheckFuncBasicClass::CalCountVerifyingParameters(
            ARG_AND_STRING(config.calcCount), VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor));
        CalCountCheckFuncBasicClass::CalCountVerifyingParameters(
            ARG_AND_STRING(config.scaleCount), VA_ARGS_TO_MAKE_TUPLE(dstTensor, scaleTensor));
        CalCountCheckFuncBasicClass::CalCountVerifyingParameters(
            ARG_AND_STRING(config.offsetCount), VA_ARGS_TO_MAKE_TUPLE(dstTensor, offsetTensor));

        CheckAscendQuantChannelOffsetTensorParamsClass::CheckAscendQuantChannelOffsetTensorParams<
            T, isReuseSource, config>(
            dstTensor, srcTensor, sharedTmpBuffer, scaleTensor, offsetTensor, scaleCount, offsetCount, calCount);
    };
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_QUANTIZATION_QUANT_QUANT_CHECK_COMMON_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANT_CHECK_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANT_CHECK_COMMON_H__
#endif
