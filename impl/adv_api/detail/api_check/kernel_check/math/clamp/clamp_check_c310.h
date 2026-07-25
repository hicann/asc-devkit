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
 * \file clamp_check_c310.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/math/clamp/clamp_check_c310.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/clamp.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CLAMP_CHECK_C310_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_MATH_CLAMP_CLAMP_CHECK_C310_H_
#define IMPL_API_CHECK_KERNEL_CHECK_MATH_CLAMP_CLAMP_CHECK_C310_H_

#include "../math_common_check.h"

namespace AscendC {
namespace HighLevelApiCheck {

template <typename T, bool isReuseSource = false>
class CheckFuncClassClampMax {
public:
    __aicore__ inline CheckFuncClassClampMax(){};
    __aicore__ inline CheckFuncClassClampMax(__gm__ const char* name){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint8_t>& sharedTmpBuffer,
        const T scalar, const uint32_t count) {};
};

template <typename T, bool isReuseSource = false>
class CheckFuncClassClampMin {
public:
    __aicore__ inline CheckFuncClassClampMin(){};
    __aicore__ inline CheckFuncClassClampMin(__gm__ const char* name){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint8_t>& sharedTmpBuffer,
        const T scalar, const uint32_t count) {};
};

template <typename T, typename U, typename S, bool isReuseSource = false>
class CheckFuncClassClamp : public DataTypeCheckFuncBasicClass,
                            public CalCountCheckFuncBasicClass,
                            public ReuseSourceCheckFuncBasicClass,
                            public SingleTensorCheckFuncBasicClass,
                            public MultipleTensorCheckFuncBasicClass {
public:
    __aicore__ inline CheckFuncClassClamp(){};
    __aicore__ inline CheckFuncClassClamp(__gm__ const char* name)
        : DataTypeCheckFuncBasicClass(name),
          CalCountCheckFuncBasicClass(name),
          ReuseSourceCheckFuncBasicClass(name),
          SingleTensorCheckFuncBasicClass(name),
          MultipleTensorCheckFuncBasicClass(name){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& min, const S& max, const uint32_t count)
    {
        ReuseSourceCheckFuncBasicClass::IsReuseSourceVerifyingParameters<false>(ARG_AND_STRING(isReuseSource));

        if constexpr (TypeUtils::IsLocalTensorType<U>() && TypeUtils::IsLocalTensorType<S>()) {
            CalCountCheckFuncBasicClass::CalCountVerifyingParameters(
                ARG_AND_STRING(count), VA_ARGS_TO_MAKE_TUPLE(dst, src, min, max));
            SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
                VA_ARGS_TO_MAKE_TUPLE(dst, src, min, max),
                MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC),
                "UB(VECIN/VECOUT/VECCALC)");
            MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(VA_ARGS_TO_MAKE_TUPLE(dst, src));
            MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(VA_ARGS_TO_MAKE_TUPLE(dst, min));
            MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(VA_ARGS_TO_MAKE_TUPLE(dst, max));
        } else if constexpr (TypeUtils::IsLocalTensorType<U>() && TypeUtils::IsInnerDefaultType<S>()) {
            CalCountCheckFuncBasicClass::CalCountVerifyingParameters(
                ARG_AND_STRING(count), VA_ARGS_TO_MAKE_TUPLE(dst, src, min));
            SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
                VA_ARGS_TO_MAKE_TUPLE(dst, src, min),
                MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC),
                "UB(VECIN/VECOUT/VECCALC)");
            MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(VA_ARGS_TO_MAKE_TUPLE(dst, src));
            MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(VA_ARGS_TO_MAKE_TUPLE(dst, min));
        } else if constexpr (TypeUtils::IsLocalTensorType<S>() && TypeUtils::IsInnerDefaultType<U>()) {
            CalCountCheckFuncBasicClass::CalCountVerifyingParameters(
                ARG_AND_STRING(count), VA_ARGS_TO_MAKE_TUPLE(dst, src, max));
            SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
                VA_ARGS_TO_MAKE_TUPLE(dst, src, max),
                MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC),
                "UB(VECIN/VECOUT/VECCALC)");
            MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(VA_ARGS_TO_MAKE_TUPLE(dst, src));
            MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(VA_ARGS_TO_MAKE_TUPLE(dst, max));
        } else {
            CalCountCheckFuncBasicClass::CalCountVerifyingParameters(
                ARG_AND_STRING(count), VA_ARGS_TO_MAKE_TUPLE(dst, src));
            SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
                VA_ARGS_TO_MAKE_TUPLE(dst, src),
                MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC),
                "UB(VECIN/VECOUT/VECCALC)");
            MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(VA_ARGS_TO_MAKE_TUPLE(dst, src));
        }
    };
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_MATH_CLAMP_CLAMP_CHECK_C310_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CLAMP_CHECK_C310_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CLAMP_CHECK_C310_H__
#endif
