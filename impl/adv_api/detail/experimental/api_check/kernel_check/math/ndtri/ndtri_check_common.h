/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file ndtri_check_common.h
 * \brief Ndtri high-level API parameter checks for common architectures.
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NDTRI_CHECK_COMMON_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_MATH_NDTRI_NDTRI_CHECK_COMMON_H_
#define IMPL_API_CHECK_KERNEL_CHECK_MATH_NDTRI_NDTRI_CHECK_COMMON_H_

#include "impl/adv_api/detail/api_check/kernel_check/basic_check/calcount_check.h"
#include "impl/adv_api/detail/api_check/kernel_check/basic_check/datatype_check.h"
#include "impl/adv_api/detail/api_check/kernel_check/basic_check/reuse_source_check.h"
#include "impl/adv_api/detail/api_check/kernel_check/basic_check/single_tensor_check.h"

namespace AscendC {
namespace experimental {
namespace HighLevelApiCheck {

using AscendC::HighLevelApiCheck::CalCountCheckFuncBasicClass;
using AscendC::HighLevelApiCheck::DataTypeCheckFuncBasicClass;
using AscendC::HighLevelApiCheck::MakeParameters2Tuple;
using AscendC::HighLevelApiCheck::MakeString2Tuple;
using AscendC::HighLevelApiCheck::ReuseSourceCheckFuncBasicClass;
using AscendC::HighLevelApiCheck::SingleTensorCheckFuncBasicClass;

template <typename T, bool isReuseSource = false>
class CheckFuncClassNdtri : public DataTypeCheckFuncBasicClass,
                            public CalCountCheckFuncBasicClass,
                            public ReuseSourceCheckFuncBasicClass,
                            public SingleTensorCheckFuncBasicClass {
public:
    __aicore__ inline CheckFuncClassNdtri(){};
    __aicore__ inline CheckFuncClassNdtri(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          CalCountCheckFuncBasicClass(apiName),
          ReuseSourceCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName){};

    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, float>("template parameter (T) is not float");
        ReuseSourceCheckFuncBasicClass::IsReuseSourceVerifyingParameters<false>(ARG_AND_STRING(isReuseSource));
        CalCountCheckFuncBasicClass::CalCountVerifyingParameters(
            ARG_AND_STRING(calCount), VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor));
        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");
    };
};

} // namespace HighLevelApiCheck
} // namespace experimental
} // namespace AscendC

#endif // IMPL_API_CHECK_KERNEL_CHECK_MATH_NDTRI_NDTRI_CHECK_COMMON_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NDTRI_CHECK_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NDTRI_CHECK_COMMON_H__
#endif
