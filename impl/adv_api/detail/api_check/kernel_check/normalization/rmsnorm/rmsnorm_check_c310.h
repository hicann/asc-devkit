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
 * \file rmsnorm_check_c310.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/normalization/rmsnorm/rmsnorm_check_c310.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/normalization/rmsnorm.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_RMSNORM_CHECK_C310_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_NORMALIZATION_RMSNORM_RMSNORM_CHECK_C310_H_
#define IMPL_API_CHECK_KERNEL_CHECK_NORMALIZATION_RMSNORM_RMSNORM_CHECK_C310_H_

#include "../../basic_check/datatype_check.h"
#include "../../basic_check/single_tensor_check.h"
#include "../../basic_check/multiple_tensor_check.h"

namespace AscendC {
namespace HighLevelApiCheck {
template <typename T, bool isBasicBlock = false>
class CheckFuncClassRmsNorm : public DataTypeCheckFuncBasicClass,
                              public SingleTensorCheckFuncBasicClass,
                              public MultipleTensorCheckFuncBasicClass {
public:
    __aicore__ inline CheckFuncClassRmsNorm(){};
    __aicore__ inline CheckFuncClassRmsNorm(__gm__ const char* name)
        : DataTypeCheckFuncBasicClass(name),
          SingleTensorCheckFuncBasicClass(name),
          MultipleTensorCheckFuncBasicClass(name){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<T>& gammaLocal,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const T epsilon, const RmsNormTiling& tiling)
    {
        ShapeInfo dstShape = dstLocal.GetShapeInfo();
        ShapeInfo srcShape = srcLocal.GetShapeInfo();
        ShapeInfo gammaShape = gammaLocal.GetShapeInfo();
        ASCENDC_ASSERT(((dstShape.shape[0] == srcShape.shape[0]) || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[RmsNorm] The B dims of dstLocal, srcLocal are %u, %u. They should be same.",
                dstShape.shape[0], srcShape.shape[0]);
        });
        ASCENDC_ASSERT(((dstShape.shape[1] == srcShape.shape[1]) || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[RmsNorm] The S dims of dstLocal, srcLocal are %u, %u. They should be same.",
                dstShape.shape[1], srcShape.shape[1]);
        });
        ASCENDC_ASSERT(
            (((dstShape.shape[2] == srcShape.shape[2]) && (dstShape.shape[2] == gammaShape.shape[0])) ||
             HighLevelAPIParametersPrint),
            {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "[RmsNorm] The H dims of dstLocal, srcLocal, gammaLocal are %u, %u, %u. They should be same.",
                    dstShape.shape[2], srcShape.shape[2], gammaShape.shape[0]);
            });
        ASCENDC_ASSERT(((gammaShape.shape[0] * sizeof(T) % ONE_BLK_SIZE == 0) || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[RmsNorm] The gammaLocal length is %u, should be 32B aligned.",
                gammaShape.shape[0] * sizeof(T));
        });
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, half, float>(
            "template parameter (T) is not half or float");
        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstLocal, srcLocal, gammaLocal),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(VA_ARGS_TO_MAKE_TUPLE(dstLocal, gammaLocal));
    };
};
} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_NORMALIZATION_RMSNORM_RMSNORM_CHECK_C310_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_RMSNORM_CHECK_C310_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_RMSNORM_CHECK_C310_H__
#endif
