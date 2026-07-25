/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file logical_xor_check_c310.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/math/logical_xor/logical_xor_check_c310.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/logical_xor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LOGICAL_XOR_CHECK_C310_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_MATH_LOGICAL_XOR_LOGICAL_XOR_CHECK_C310_H_
#define IMPL_API_CHECK_KERNEL_CHECK_MATH_LOGICAL_XOR_LOGICAL_XOR_CHECK_C310_H_
#include "../../basic_check/datatype_check.h"
#include "../../basic_check/calcount_check.h"
#include "../../basic_check/reuse_source_check.h"
#include "../../basic_check/single_tensor_check.h"
#include "../../basic_check/multiple_tensor_check.h"

namespace AscendC {
namespace HighLevelApiCheck {
template <typename T, typename U, bool isReuseSource>
class CheckFuncClassLogicalXor : public DataTypeCheckFuncBasicClass,
                                 public CalCountCheckFuncBasicClass,
                                 public ReuseSourceCheckFuncBasicClass,
                                 public SingleTensorCheckFuncBasicClass,
                                 public MultipleTensorCheckFuncBasicClass {
public:
    __aicore__ inline CheckFuncClassLogicalXor(){};
    __aicore__ inline CheckFuncClassLogicalXor(__gm__ const char* name)
        : DataTypeCheckFuncBasicClass(name),
          CalCountCheckFuncBasicClass(name),
          ReuseSourceCheckFuncBasicClass(name),
          SingleTensorCheckFuncBasicClass(name),
          MultipleTensorCheckFuncBasicClass(name){};
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, const uint32_t count)
    {
        ReuseSourceCheckFuncBasicClass::IsReuseSourceVerifyingParameters<false>(ARG_AND_STRING(isReuseSource));

        CalCountCheckFuncBasicClass::CalCountVerifyingParameters(
            ARG_AND_STRING(count), VA_ARGS_TO_MAKE_TUPLE(dst, src0, src1));

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dst, src0, src1),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(VA_ARGS_TO_MAKE_TUPLE(dst, src0));

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(VA_ARGS_TO_MAKE_TUPLE(dst, src1));
    };
};
} // namespace HighLevelApiCheck
} // namespace AscendC

#endif // IMPL_API_CHECK_KERNEL_CHECK_MATH_LOGICAL_XOR_LOGICAL_XOR_CHECK_C310_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LOGICAL_XOR_CHECK_C310_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LOGICAL_XOR_CHECK_C310_H__
#endif
