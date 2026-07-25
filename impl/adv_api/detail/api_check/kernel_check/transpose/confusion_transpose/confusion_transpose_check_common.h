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
 * \file confusion_transpose_check_common.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/transpose/confusion_transpose/confusion_transpose_check_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/transpose/confusion_transpose.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONFUSION_TRANSPOSE_CHECK_COMMON_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_TRANSPOSE_CONFUSION_TRANSPOSE_CONFUSION_TRANSPOSE_CHECK_COMMON_H_
#define IMPL_API_CHECK_KERNEL_CHECK_TRANSPOSE_CONFUSION_TRANSPOSE_CONFUSION_TRANSPOSE_CHECK_COMMON_H_

#include "../../basic_check/datatype_check.h"
#include "../../basic_check/single_tensor_check.h"
#include "../../basic_check/multiple_tensor_check.h"

namespace AscendC {
namespace HighLevelApiCheck {
class TransposeTypeCheckFuncBasicClass {
public:
    __aicore__ inline TransposeTypeCheckFuncBasicClass(){};
    __aicore__ inline TransposeTypeCheckFuncBasicClass(__gm__ const char* apiName) { this->apiName = apiName; };

    template <typename U>
    __aicore__ inline void TransposeTypeCheck(
        const TransposeType curType, U validTypeTuple, __gm__ const char* validTypeString)
    {
        static_assert((Std::is_tuple_v<U>), "Input template U is not tuple!");
        TransposeTypeCheckLoopCheck<U>(curType, validTypeTuple, validTypeString, tuple_sequence<U>{});
    }

private:
    template <typename U, size_t... Is>
    __aicore__ inline void TransposeTypeCheckLoopCheck(
        const TransposeType checkType, U posTuple, __gm__ const char* posString, Std::index_sequence<Is...>)
    {
        bool status = ((checkType == Std::get<Is>(posTuple)) || ...);

        auto typeTuple = MakeParameters2Tuple(
            TransposeType::TRANSPOSE_TYPE_NONE, TransposeType::TRANSPOSE_NZ2ND_0213,
            TransposeType::TRANSPOSE_NZ2NZ_0213, TransposeType::TRANSPOSE_NZ2NZ_012_WITH_N,
            TransposeType::TRANSPOSE_NZ2ND_012_WITH_N, TransposeType::TRANSPOSE_NZ2ND_012_WITHOUT_N,
            TransposeType::TRANSPOSE_NZ2NZ_012_WITHOUT_N, TransposeType::TRANSPOSE_ND2ND_ONLY,
            TransposeType::TRANSPOSE_ND_UB_GM, TransposeType::TRANSPOSE_GRAD_ND_UB_GM,
            TransposeType::TRANSPOSE_ND2ND_B16, TransposeType::TRANSPOSE_NCHW2NHWC, TransposeType::TRANSPOSE_NHWC2NCHW);

        auto typeStringTuple = MakeString2Tuple(
            "TransposeType::TRANSPOSE_TYPE_NONE", "TransposeType::TRANSPOSE_NZ2ND_0213",
            "TransposeType::TRANSPOSE_NZ2NZ_0213", "TransposeType::TRANSPOSE_NZ2NZ_012_WITH_N",
            "TransposeType::TRANSPOSE_NZ2ND_012_WITH_N", "TransposeType::TRANSPOSE_NZ2ND_012_WITHOUT_N",
            "TransposeType::TRANSPOSE_NZ2NZ_012_WITHOUT_N", "TransposeType::TRANSPOSE_ND2ND_ONLY",
            "TransposeType::TRANSPOSE_ND_UB_GM", "TransposeType::TRANSPOSE_GRAD_ND_UB_GM",
            "TransposeType::TRANSPOSE_ND2ND_B16", "TransposeType::TRANSPOSE_NCHW2NHWC",
            "TransposeType::TRANSPOSE_NHWC2NCHW");

        auto typeName =
            FindStringFromTuple(checkType, typeTuple, typeStringTuple, tuple_sequence<decltype(typeTuple)>{});

        ASCENDC_ASSERT((status) || HighLevelAPIParametersPrint, {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[%s] Failed to check transpose type, current api support types are %s, "
                "current type is %s.",
                apiName, posString,
                FindStringFromTuple(checkType, typeTuple, typeStringTuple, tuple_sequence<decltype(typeTuple)>{}));
        });

        if constexpr (HighLevelAPIParametersPrint) {
            PrintTransposeType(checkType, typeTuple, typeStringTuple);
        }
    }

    template <typename U, typename W>
    __aicore__ inline void PrintTransposeType(const TransposeType checkType, U typeTuple, W typeStringTuple)
    {
        KERNEL_LOG(
            KERNEL_INFO, "[%s] The transpose type is %s.", apiName,
            FindStringFromTuple(checkType, typeTuple, typeStringTuple, tuple_sequence<decltype(typeTuple)>{}));
    }

private:
    __gm__ const char* apiName = nullptr;
};

template <typename T>
class CheckFuncClassConfusionTranspose : public DataTypeCheckFuncBasicClass,
                                         public SingleTensorCheckFuncBasicClass,
                                         public MultipleTensorCheckFuncBasicClass,
                                         public TransposeTypeCheckFuncBasicClass {
public:
    __aicore__ inline CheckFuncClassConfusionTranspose(){};
    __aicore__ inline CheckFuncClassConfusionTranspose(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName),
          TransposeTypeCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
        TransposeType transposeType, ConfusionTransposeTiling& tiling)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, half, float, uint16_t, int16_t, uint32_t, int32_t>(
            "template parameter (T) is not half/float/uint16_t/int16_t/uint32_t/int32_t");

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstTensor, srcTensor, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        TransposeTypeCheckFuncBasicClass::TransposeTypeCheck(
            transposeType, VA_ARGS_TO_MAKE_TUPLE_STRING(
                               TransposeType::TRANSPOSE_NZ2ND_0213, TransposeType::TRANSPOSE_NZ2NZ_0213,
                               TransposeType::TRANSPOSE_NZ2NZ_012_WITH_N, TransposeType::TRANSPOSE_NZ2ND_012_WITH_N,
                               TransposeType::TRANSPOSE_NZ2ND_012_WITHOUT_N,
                               TransposeType::TRANSPOSE_NZ2NZ_012_WITHOUT_N, TransposeType::TRANSPOSE_ND2ND_ONLY));
    };
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_TRANSPOSE_CONFUSION_TRANSPOSE_CONFUSION_TRANSPOSE_CHECK_COMMON_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONFUSION_TRANSPOSE_CHECK_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONFUSION_TRANSPOSE_CHECK_COMMON_H__
#endif
