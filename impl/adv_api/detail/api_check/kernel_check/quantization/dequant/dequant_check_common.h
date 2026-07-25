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
 * \file dequant_check_common.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/quantization/dequant/dequant_check_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/quantization/dequantize.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEQUANT_CHECK_COMMON_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_QUANTIZATION_DEQUANT_DEQUANT_CHECK_COMMON_H_
#define IMPL_API_CHECK_KERNEL_CHECK_QUANTIZATION_DEQUANT_DEQUANT_CHECK_COMMON_H_

#include "../../basic_check/datatype_check.h"
#include "../../basic_check/single_tensor_check.h"
#include "../../basic_check/multiple_tensor_check.h"
#include "../../../../../../../include/adv_api/quantization/ascend_dequant_utils.h"

namespace AscendC {
namespace HighLevelApiCheck {
constexpr uint32_t CHECK_DEQUANT_FLOAT_PER_BLOCK = 8;

class CheckAscendDequantParamsClass {
public:
    template <typename dstT, typename scaleT, bool isPureDqParams, DeQuantMode mode>
    __aicore__ inline void CheckAscendDequantParams(
        const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const LocalTensor<scaleT>& deqScale,
        const LocalTensor<uint8_t>& sharedTmpBuffer, DequantParams& params, uint32_t calCount)
    {
        VerifyingParameters<dstT, scaleT, isPureDqParams, mode>(
            dstTensor, srcTensor, deqScale, sharedTmpBuffer, params, calCount);
        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters<dstT, scaleT, isPureDqParams, mode>(
                dstTensor, srcTensor, deqScale, sharedTmpBuffer, params, calCount);
        }
    }

private:
    template <typename dstT, typename scaleT, bool isPureDqParams, DeQuantMode mode>
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const LocalTensor<scaleT>& deqScale,
        const LocalTensor<uint8_t>& sharedTmpBuffer, DequantParams& params, uint32_t calCount)
    {
        // params.n must be 32B aligned as FP32
        ASCENDC_ASSERT(params.n % CHECK_DEQUANT_FLOAT_PER_BLOCK == 0 || HighLevelAPIParametersPrint, {
            KERNEL_LOG(KERNEL_ERROR, "[AscendDequant] params.n %u must be divisible by 8", params.n);
        });
        // params.m * params.n <= srcTensor element num
        ASCENDC_ASSERT(params.m * params.n <= srcTensor.GetSize() || HighLevelAPIParametersPrint, {
            KERNEL_LOG(
                KERNEL_ERROR, "[AscendDequant] params.m %u * params.n %u \
            must not be larger than element num of srcTensor %u",
                params.m, params.n, srcTensor.GetSize());
        });
        // params.calCount must be in range (0, params.n]
        ASCENDC_ASSERT((params.calCount > 0 && params.calCount <= params.n) || HighLevelAPIParametersPrint, {
            KERNEL_LOG(
                KERNEL_ERROR, "[AscendDequant] params.calCount is %u, which should be in range (0, params.n %u]",
                params.calCount, params.n);
        });
        // params.m * (params.n after align with dstT) <= dstTensor elementNum
        uint32_t oneBlockNum = ONE_BLK_SIZE / sizeof(dstT);
        uint32_t alignInner = (params.n + oneBlockNum - 1) / oneBlockNum * oneBlockNum;
        ASCENDC_ASSERT((params.m * alignInner <= dstTensor.GetSize()) || HighLevelAPIParametersPrint, {
            KERNEL_LOG(
                KERNEL_ERROR, "[AscendDequant] dstTensor element num should be not less than %u",
                params.m * alignInner);
        });
        // params.calCount must <= deqScale element num
        ASCENDC_ASSERT((params.calCount <= deqScale.GetSize() || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "params.calCount %u must not be \
            larger than deqScale element num %u",
                params.calCount, deqScale.GetSize());
        });
    }

    template <typename dstT, typename scaleT, bool isPureDqParams, DeQuantMode mode>
    __aicore__ inline void PrintParameters(
        const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const LocalTensor<scaleT>& deqScale,
        const LocalTensor<uint8_t>& sharedTmpBuffer, DequantParams& params, uint32_t calCount)
    {
        KERNEL_LOG(
            KERNEL_INFO, "[AscendDequant] The params.m is %u, params.n is %u, params.calCount is %u.", params.m,
            params.n, params.calCount);
    }
};

template <typename dstT, typename scaleT, bool isPureDqParams, DeQuantMode mode>
class CheckFuncClassAscendDequant : public DataTypeCheckFuncBasicClass,
                                    public SingleTensorCheckFuncBasicClass,
                                    public MultipleTensorCheckFuncBasicClass,
                                    public CheckAscendDequantParamsClass {
public:
    __aicore__ inline CheckFuncClassAscendDequant(){};
    __aicore__ inline CheckFuncClassAscendDequant(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const LocalTensor<scaleT>& deqScale,
        const LocalTensor<uint8_t>& sharedTmpBuffer, DequantParams& params, uint32_t calCount)
    {
#if __NPU_ARCH__ == 2002
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<dstT, half, float>(
            "second template parameter (dstT) is not half or float");
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<scaleT, uint64_t, float>(
            "second template parameter (scaleT) is not uint64_t or float");
#elif __NPU_ARCH__ == 2201
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<dstT, half, float, bfloat16_t>(
            "first template parameter (dstT) is not half/float/bfloat16_t");
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<scaleT, uint64_t, bfloat16_t, float>(
            "second template parameter (scaleT) is not uint64_t/bfloat16_t/float");
#endif
        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor, deqScale, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(VA_ARGS_TO_MAKE_TUPLE(dstTensor, deqScale));
        CheckAscendDequantParamsClass::CheckAscendDequantParams<dstT, scaleT, isPureDqParams, mode>(
            dstTensor, srcTensor, deqScale, sharedTmpBuffer, params, calCount);
    };
};

class CheckAscendDequantScalarParamsClass {
public:
    template <typename dstT, typename scaleT, bool isPureDqParams, DeQuantMode mode>
    __aicore__ inline void CheckAscendDequantParams(
        const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const scaleT deqScale,
        const LocalTensor<uint8_t>& sharedTmpBuffer, DequantParams& params)
    {
        VerifyingParameters<dstT, scaleT, isPureDqParams, mode>(
            dstTensor, srcTensor, deqScale, sharedTmpBuffer, params);
        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters<dstT, scaleT, isPureDqParams, mode>(
                dstTensor, srcTensor, deqScale, sharedTmpBuffer, params);
        }
    }

private:
    template <typename dstT, typename scaleT, bool isPureDqParams, DeQuantMode mode>
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const scaleT deqScale,
        const LocalTensor<uint8_t>& sharedTmpBuffer, DequantParams& params)
    {
        // params.n must be 32B aligned as FP32
        ASCENDC_ASSERT(params.n % CHECK_DEQUANT_FLOAT_PER_BLOCK == 0 || HighLevelAPIParametersPrint, {
            KERNEL_LOG(KERNEL_ERROR, "[AscendDequant] params.n %u must be divisible by 8", params.n);
        });
        // params.m * params.n <= srcTensor element num
        ASCENDC_ASSERT(params.m * params.n <= srcTensor.GetSize() || HighLevelAPIParametersPrint, {
            KERNEL_LOG(
                KERNEL_ERROR, "[AscendDequant] params.m %u * params.n %u \
            must not be larger than element num of srcTensor %u",
                params.m, params.n, srcTensor.GetSize());
        });
        // params.calCount must be in range (0, params.n]
        ASCENDC_ASSERT((params.calCount > 0 && params.calCount <= params.n) || HighLevelAPIParametersPrint, {
            KERNEL_LOG(
                KERNEL_ERROR, "[AscendDequant] params.calCount is %u, which should be in range (0, params.n %u]",
                params.calCount, params.n);
        });
        // params.m * (params.n after align with dstT) <= dstTensor elementNum
        uint32_t oneBlockNum = ONE_BLK_SIZE / sizeof(dstT);
        uint32_t alignInner = (params.n + oneBlockNum - 1) / oneBlockNum * oneBlockNum;
        ASCENDC_ASSERT((params.m * alignInner <= dstTensor.GetSize()) || HighLevelAPIParametersPrint, {
            KERNEL_LOG(
                KERNEL_ERROR, "[AscendDequant] dstTensor element num should be not less than %u",
                params.m * alignInner);
        });
    }

    template <typename dstT, typename scaleT, bool isPureDqParams, DeQuantMode mode>
    __aicore__ inline void PrintParameters(
        const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const scaleT deqScale,
        const LocalTensor<uint8_t>& sharedTmpBuffer, DequantParams& params)
    {
        KERNEL_LOG(
            KERNEL_INFO, "[AscendDequant] The params.m is %u, params.n is %u, params.calCount is %u.", params.m,
            params.n, params.calCount);
    }
};

template <typename dstT, typename scaleT, bool isPureDqParams, DeQuantMode mode>
class CheckFuncClassAscendDequantScalar : public DataTypeCheckFuncBasicClass,
                                          public SingleTensorCheckFuncBasicClass,
                                          public MultipleTensorCheckFuncBasicClass,
                                          public CheckAscendDequantScalarParamsClass {
public:
    __aicore__ inline CheckFuncClassAscendDequantScalar(){};
    __aicore__ inline CheckFuncClassAscendDequantScalar(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const scaleT deqScale,
        const LocalTensor<uint8_t>& sharedTmpBuffer, DequantParams& params)
    {
#if __NPU_ARCH__ == 2002
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<dstT, half, float>(
            "second template parameter (dstT) is not half or float");
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<scaleT, float>(
            "second template parameter (scaleT) is not float");
#elif __NPU_ARCH__ == 2201
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<dstT, half, float, bfloat16_t>(
            "first template parameter (dstT) is not half/float/bfloat16_t");
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<scaleT, bfloat16_t, float>(
            "second template parameter (scaleT) is not bfloat16_t or float");
#endif
        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor, sharedTmpBuffer));

        CheckAscendDequantScalarParamsClass::CheckAscendDequantParams<dstT, scaleT, isPureDqParams, mode>(
            dstTensor, srcTensor, deqScale, sharedTmpBuffer, params);
    };
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_QUANTIZATION_DEQUANT_DEQUANT_CHECK_COMMON_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEQUANT_CHECK_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DEQUANT_CHECK_COMMON_H__
#endif
