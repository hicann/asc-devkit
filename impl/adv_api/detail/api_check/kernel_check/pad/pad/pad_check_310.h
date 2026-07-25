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
 * \file pad_check_310.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/pad/pad/pad_check_310.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/pad/pad.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PAD_CHECK_310_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_PAD_PAD_PAD_CHECK_310_H_
#define IMPL_API_CHECK_KERNEL_CHECK_PAD_PAD_PAD_CHECK_310_H_

#include "../../basic_check/datatype_check.h"
#include "../../basic_check/single_tensor_check.h"
#include "../../basic_check/multiple_tensor_check.h"

namespace AscendC {
namespace HighLevelApiCheck {
class CheckPadParamsClass {
public:
    template <typename T>
    __aicore__ inline void CheckPadParams(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, PadParams& padParams,
        const LocalTensor<uint8_t>& sharedTmpBuffer, PadTiling& tiling)
    {
        VerifyingParameters<T>(dstTensor, srcTensor, padParams, sharedTmpBuffer, tiling);
        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters<T>(dstTensor, srcTensor, padParams, sharedTmpBuffer, tiling);
        }
    }

private:
    template <typename T>
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, PadParams& padParams,
        const LocalTensor<uint8_t>& sharedTmpBuffer, PadTiling& tiling)
    {
        ASCENDC_ASSERT((padParams.leftPad * sizeof(T) < ONE_BLK_SIZE || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[Pad] The result of padParams.leftPad * sizeof(T) cannot be %lu, should be less than 32.",
                padParams.leftPad * sizeof(T));
        });
        ASCENDC_ASSERT((padParams.rightPad * sizeof(T) < ONE_BLK_SIZE || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[Pad] The result of padParams.rightPad * sizeof(T) cannot be %lu, should be less than 32.",
                padParams.rightPad * sizeof(T));
        });

        if (tiling.srcWidth * sizeof(T) % ONE_BLK_SIZE == 0) {
            ASCENDC_ASSERT((tiling.srcWidth != tiling.srcOriWidth || HighLevelAPIParametersPrint), {
                KERNEL_LOG(
                    KERNEL_ERROR, "[Pad] The tiling.srcWidth is %u, tiling.srcOriWidth is %u, which can not be equal.",
                    tiling.srcWidth, tiling.srcOriWidth);
            });
            ASCENDC_ASSERT((padParams.leftPad == 0 || HighLevelAPIParametersPrint), {
                KERNEL_LOG(KERNEL_ERROR, "[Pad] The padParams.leftPad is %u, which should be 0.", padParams.leftPad);
            });
            ASCENDC_ASSERT((padParams.rightPad != 0 || HighLevelAPIParametersPrint), {
                KERNEL_LOG(
                    KERNEL_ERROR, "[Pad] The padParams.rightPad is %u, which should not be 0.", padParams.rightPad);
            });
        } else {
            bool ans = ((tiling.srcWidth + padParams.leftPad + padParams.rightPad) * sizeof(T) % ONE_BLK_SIZE == 0);
            ASCENDC_ASSERT((ans || HighLevelAPIParametersPrint), {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "[Pad] The tiling.srcWidth is %u, padParams.leftPad is %u, padParams.rightPad is %u, the result of "
                    "(tiling.srcWidth + padParams.leftPad + padParams.rightPad) * sizeof(T) should be an integer "
                    "multiple of 32.",
                    tiling.srcWidth, padParams.leftPad, padParams.rightPad);
            });
        }
    }

    template <typename T>
    __aicore__ inline void PrintParameters(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, PadParams& padParams,
        const LocalTensor<uint8_t>& sharedTmpBuffer, PadTiling& tiling)
    {
        KERNEL_LOG(
            KERNEL_INFO, "[Pad] The tiling.srcWidth is %u, tiling.srcOriWidth is %u.", tiling.srcWidth,
            tiling.srcOriWidth);
        KERNEL_LOG(
            KERNEL_INFO, "[Pad] The padParams.leftPad is %u, padParams.rightPad is %u.", padParams.leftPad,
            padParams.rightPad);
    }
};

template <typename T>
class CheckFuncClassPad : public DataTypeCheckFuncBasicClass,
                          public SingleTensorCheckFuncBasicClass,
                          public MultipleTensorCheckFuncBasicClass,
                          public CheckPadParamsClass {
public:
    __aicore__ inline CheckFuncClassPad(){};
    __aicore__ inline CheckFuncClassPad(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, PadParams& padParams,
        const LocalTensor<uint8_t>& sharedTmpBuffer, PadTiling& tiling)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, int16_t, uint16_t, int32_t, uint32_t, half, float>(
            "template parameter (T) is not int16_t/uint16_t/int32_t/uint32_t/half/float");

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor, sharedTmpBuffer));

        CheckPadParamsClass::CheckPadParams<T>(dstTensor, srcTensor, padParams, sharedTmpBuffer, tiling);
    };
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_PAD_PAD_PAD_CHECK_310_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PAD_CHECK_310_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PAD_CHECK_310_H__
#endif
