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
 * \file unpad_check_310.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/pad/pad/unpad_check_310.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/pad/pad.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_UNPAD_CHECK_310_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_PAD_PAD_UNPAD_CHECK_310_H_
#define IMPL_API_CHECK_KERNEL_CHECK_PAD_PAD_UNPAD_CHECK_310_H_

#include "../../basic_check/datatype_check.h"
#include "../../basic_check/single_tensor_check.h"
#include "../../basic_check/multiple_tensor_check.h"

namespace AscendC {
namespace HighLevelApiCheck {
class CheckUnPadParamsClass {
public:
    template <typename T>
    __aicore__ inline void CheckUnPadParams(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, UnPadParams& unPadParams,
        LocalTensor<uint8_t>& sharedTmpBuffer, UnPadTiling& tiling)
    {
        VerifyingParameters<T>(dstTensor, srcTensor, unPadParams, sharedTmpBuffer, tiling);
        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters<T>(dstTensor, srcTensor, unPadParams, sharedTmpBuffer, tiling);
        }
    }

private:
    template <typename T>
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, UnPadParams& unPadParams,
        LocalTensor<uint8_t>& sharedTmpBuffer, UnPadTiling& tiling)
    {
        ASCENDC_ASSERT(((tiling.srcWidth) * sizeof(T) % ONE_BLK_SIZE == 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[UnPad] The result of tiling.srcWidth * sizeof(T) cannot be %lu, "
                "should be an integer multiple of 32.",
                (tiling.srcWidth) * sizeof(T));
        });
        ASCENDC_ASSERT((unPadParams.rightPad > 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[UnPad] The unPadParams.rightPad is %u, should be greater than 0.",
                unPadParams.rightPad);
        });
        ASCENDC_ASSERT((unPadParams.rightPad * sizeof(T) < ONE_BLK_SIZE || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[UnPad] The result of unPadParams.rightPad * sizeof(T) cannot be %lu, "
                "should be less than 32.",
                unPadParams.rightPad * sizeof(T));
        });
        ASCENDC_LOG_IF_CHECK((unPadParams.leftPad == 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_WARN, "[UnPad] The unPadParams.leftPad is %u, may not be effective.", unPadParams.leftPad);
        });
    }

    template <typename T>
    __aicore__ inline void PrintParameters(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, UnPadParams& unPadParams,
        LocalTensor<uint8_t>& sharedTmpBuffer, UnPadTiling& tiling)
    {
        KERNEL_LOG(KERNEL_INFO, "[UnPad] The tiling.srcWidth is %u.", tiling.srcWidth);
        KERNEL_LOG(
            KERNEL_INFO, "[UnPad] The unPadParams.leftPad is %u, unPadParams.rightPad is %u.", unPadParams.leftPad,
            unPadParams.rightPad);
    }
};

template <typename T>
class CheckFuncClassUnPad : public DataTypeCheckFuncBasicClass,
                            public SingleTensorCheckFuncBasicClass,
                            public MultipleTensorCheckFuncBasicClass,
                            public CheckUnPadParamsClass {
public:
    __aicore__ inline CheckFuncClassUnPad(){};
    __aicore__ inline CheckFuncClassUnPad(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, UnPadParams& unPadParams,
        LocalTensor<uint8_t>& sharedTmpBuffer, UnPadTiling& tiling)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, uint16_t, int16_t, uint32_t, int32_t, half, float>(
            "template parameter (T) is not uint16_t/int16_t/uint32_t/int32_t/half/float");

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor, sharedTmpBuffer));

        CheckUnPadParamsClass::CheckUnPadParams<T>(dstTensor, srcTensor, unPadParams, sharedTmpBuffer, tiling);
    };
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_PAD_PAD_UNPAD_CHECK_310_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_UNPAD_CHECK_310_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_UNPAD_CHECK_310_H__
#endif
