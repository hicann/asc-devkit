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
 * \file cumsum_check_common.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/math/cumsum/cumsum_check_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/cumsum.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CUMSUM_CHECK_COMMON_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_MATH_CUMSUM_CUMSUM_CHECK_COMMON_H_
#define IMPL_API_CHECK_KERNEL_CHECK_MATH_CUMSUM_CUMSUM_CHECK_COMMON_H_

#include "../../basic_check/datatype_check.h"
#include "../../basic_check/single_tensor_check.h"
#include "../../basic_check/multiple_tensor_check.h"
#include "../../../../../../../include/adv_api/math/cumsum_utils.h"

namespace AscendC {
namespace HighLevelApiCheck {
class CheckCumSumParams {
public:
    template <typename T, const CumSumConfig& config>
    __aicore__ inline void CheckCumSumInfo(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& lastRowTensor, const LocalTensor<T>& srcTensor,
        const CumSumInfo& cumSumInfo)
    {
        VerifyingParameters<T, config>(dstTensor, lastRowTensor, srcTensor, cumSumInfo);
        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters<T, config>(dstTensor, lastRowTensor, srcTensor, cumSumInfo);
        }
    }

private:
    template <typename T, const CumSumConfig& config>
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& lastRowTensor, const LocalTensor<T>& srcTensor,
        const CumSumInfo& cumSumInfo)
    {
        ASCENDC_ASSERT((cumSumInfo.inner > 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[CumSum] The cumSumInfo.inner parameter is %u, should be larger than 0.",
                cumSumInfo.inner);
        });
        ASCENDC_ASSERT((cumSumInfo.outter > 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[CumSum] The cumSumInfo.inner parameter is %u, should be larger than 0.",
                cumSumInfo.outter);
        });
        ASCENDC_ASSERT(((cumSumInfo.inner * sizeof(T) % ONE_BLK_SIZE == 0) || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[CumSum] The result of cumSumInfo.inner * sizeof(T) is %lu, "
                "should be an integer multiple of 32.",
                cumSumInfo.inner * sizeof(T));
        });
        ASCENDC_ASSERT((srcTensor.GetSize() >= (cumSumInfo.inner * cumSumInfo.outter) || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[CumSum] The result of cumSumInfo.inner * cumSumInfo.outter "
                "is %u, should not be larger than srcTensor size %u.",
                cumSumInfo.inner * cumSumInfo.outter, srcTensor.GetSize());
        });
        ASCENDC_ASSERT((dstTensor.GetSize() >= (cumSumInfo.inner * cumSumInfo.outter) || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[CumSum] The result of cumSumInfo.inner * cumSumInfo.outter "
                "is %u, should not be larger than dstTensor size %u.",
                cumSumInfo.inner * cumSumInfo.outter, dstTensor.GetSize());
        });
        if (config.outputLastRow) {
            ASCENDC_ASSERT((lastRowTensor.GetSize() >= cumSumInfo.inner || HighLevelAPIParametersPrint), {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "[CumSum] The cumSumInfo.inner parameter is %u, should not be larger than lastRowTensor size %u "
                    "when config.outputLastRow is true.",
                    cumSumInfo.inner, lastRowTensor.GetSize());
            });
        }
    }

    template <typename T, const CumSumConfig& config>
    __aicore__ inline void PrintParameters(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& lastRowTensor, const LocalTensor<T>& srcTensor,
        const CumSumInfo& cumSumInfo)
    {
        KERNEL_LOG(
            KERNEL_INFO, "[CumSum] cumSumInfo.outter is %u, cumSumInfo.inner is %u.", cumSumInfo.outter,
            cumSumInfo.inner);
        KERNEL_LOG(
            KERNEL_INFO, "[CumSum] config.isLastAxis is %d, config.isReuseSource is %d, config.outputLastRow is %d.",
            config.isLastAxis, config.isReuseSource, config.outputLastRow);
        KERNEL_LOG(
            KERNEL_INFO, "[CumSum] srcTensor size is %u, dstTensor size is %u, lastRowTensor size is %u.",
            srcTensor.GetSize(), dstTensor.GetSize(), lastRowTensor.GetSize());
    }
};

template <typename T, const CumSumConfig& config>
class CheckFuncClassCumSum : public DataTypeCheckFuncBasicClass,
                             public SingleTensorCheckFuncBasicClass,
                             public MultipleTensorCheckFuncBasicClass,
                             public CheckCumSumParams {
public:
    __aicore__ inline CheckFuncClassCumSum(){};
    __aicore__ inline CheckFuncClassCumSum(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& lastRowTensor, const LocalTensor<T>& srcTensor,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const CumSumInfo& cumSumInfo)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, half, float>(
            "template parameter (T) is not half or float");

        CheckCumSumParams::CheckCumSumInfo<T, config>(dstTensor, lastRowTensor, srcTensor, cumSumInfo);

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, lastRowTensor, srcTensor, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");
    };
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_MATH_CUMSUM_CUMSUM_CHECK_COMMON_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CUMSUM_CHECK_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CUMSUM_CHECK_COMMON_H__
#endif
