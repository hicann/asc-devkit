/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/sort/extract/extract_check_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/sort/sort.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_EXTRACT_CHECK_COMMON_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_SORT_EXTRACT_EXTRACT_CHECK_COMMON_H_
#define IMPL_API_CHECK_KERNEL_CHECK_SORT_EXTRACT_EXTRACT_CHECK_COMMON_H_

#include "../../basic_check/datatype_check.h"
#include "../../basic_check/multiple_tensor_check.h"
#include "../../basic_check/reuse_source_check.h"
#include "../../basic_check/single_tensor_check.h"

namespace AscendC {
namespace HighLevelApiCheck {
class CheckExtractParamsClass {
public:
    __aicore__ inline CheckExtractParamsClass() {}
    __aicore__ inline CheckExtractParamsClass(__gm__ const char* apiName) { this->apiName = apiName; }

public:
    template <typename T>
    __aicore__ inline void CheckExtractParams(
        const LocalTensor<T>& dstValue, const LocalTensor<uint32_t>& dstIndex, const LocalTensor<T>& sorted,
        const int32_t repeatTime)
    {
        VerifyingParameters<T>(dstValue, dstIndex, sorted, repeatTime);

        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters<T>(dstValue, dstIndex, sorted, repeatTime);
        }
    }

private:
    template <typename T>
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstValue, const LocalTensor<uint32_t>& dstIndex, const LocalTensor<T>& sorted,
        const int32_t repeatTime)
    {
        ASCENDC_ASSERT(((repeatTime >= 0 && repeatTime <= MAX_REPEAT_TIMES) || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[%s] The repeatTime is %d, should in range [0, %u].", this->apiName, repeatTime,
                MAX_REPEAT_TIMES);
        });
    }

    template <typename T>
    __aicore__ inline void PrintParameters(
        const LocalTensor<T>& dstValue, const LocalTensor<uint32_t>& dstIndex, const LocalTensor<T>& sorted,
        const int32_t repeatTime)
    {
        KERNEL_LOG(KERNEL_INFO, "[%s] The repeatTime is %d.", this->apiName, repeatTime);
    }

private:
    __gm__ const char* apiName = nullptr;
};

template <typename T>
class CheckFuncClassExtract : public DataTypeCheckFuncBasicClass,
                              public ReuseSourceCheckFuncBasicClass,
                              public SingleTensorCheckFuncBasicClass,
                              public MultipleTensorCheckFuncBasicClass,
                              public CheckExtractParamsClass {
public:
    __aicore__ inline CheckFuncClassExtract() {}
    __aicore__ inline CheckFuncClassExtract(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          ReuseSourceCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName),
          CheckExtractParamsClass(apiName)
    {}

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstValue, const LocalTensor<uint32_t>& dstIndex, const LocalTensor<T>& sorted,
        const int32_t repeatTime)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, half, float>("template parameter (T) is not half "
                                                                                 "or float");

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstValue, dstIndex, sorted),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(VA_ARGS_TO_MAKE_TUPLE(dstValue, dstIndex));

        CheckExtractParamsClass::CheckExtractParams<T>(dstValue, dstIndex, sorted, repeatTime);
    }
};

} // namespace HighLevelApiCheck
} // namespace AscendC

#endif // IMPL_API_CHECK_KERNEL_CHECK_SORT_EXTRACT_EXTRACT_CHECK_COMMON_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_EXTRACT_CHECK_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_EXTRACT_CHECK_COMMON_H__
#endif
