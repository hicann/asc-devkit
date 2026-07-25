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
 * \file where_check_aicore.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/math/where/where_check_aicore.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/where.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_WHERE_CHECK_AICORE_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_MATH_WHERE_WHERE_CHECK_AICORE_H_
#define IMPL_API_CHECK_KERNEL_CHECK_MATH_WHERE_WHERE_CHECK_AICORE_H_

#include "../../basic_check/calcount_check.h"
#include "../../basic_check/single_tensor_check.h"

namespace AscendC {
namespace HighLevelApiCheck {
template <typename T, typename U, typename S, typename V>
class CheckFuncClassWhere : public CalCountCheckFuncBasicClass, public SingleTensorCheckFuncBasicClass {
public:
    __aicore__ inline CheckFuncClassWhere(){};
    __aicore__ inline CheckFuncClassWhere(__gm__ const char* name)
        : CalCountCheckFuncBasicClass(name), SingleTensorCheckFuncBasicClass(name){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dst, const U& src0, const S& src1, const LocalTensor<V>& condition, const uint32_t count)
    {
        CalCountCheckFuncBasicClass::CalCountVerifyingParameters(
            ARG_AND_STRING(count), VA_ARGS_TO_MAKE_TUPLE(dst, condition));
        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dst, condition),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");
        if constexpr (TypeUtils::IsLocalTensorType<U>()) {
            CalCountCheckFuncBasicClass::CalCountVerifyingParameters(
                ARG_AND_STRING(count), VA_ARGS_TO_MAKE_TUPLE(src0));
            SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
                VA_ARGS_TO_MAKE_TUPLE(src0),
                MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC),
                "UB(VECIN/VECOUT/VECCALC)");
        }
        if constexpr (TypeUtils::IsLocalTensorType<S>()) {
            CalCountCheckFuncBasicClass::CalCountVerifyingParameters(
                ARG_AND_STRING(count), VA_ARGS_TO_MAKE_TUPLE(src1));
            SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
                VA_ARGS_TO_MAKE_TUPLE(src1),
                MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC),
                "UB(VECIN/VECOUT/VECCALC)");
        }
    };
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_MATH_WHERE_WHERE_CHECK_AICORE_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_WHERE_CHECK_AICORE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_WHERE_CHECK_AICORE_H__
#endif
