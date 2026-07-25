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
 * \file antiquant_check_common.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/quantization/antiquant/antiquant_check_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/quantization/antiquantize.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ANTIQUANT_CHECK_COMMON_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_QUANTIZATION_ANTIQUANT_ANTIQUANT_CHECK_COMMON_H_
#define IMPL_API_CHECK_KERNEL_CHECK_QUANTIZATION_ANTIQUANT_ANTIQUANT_CHECK_COMMON_H_

#include "../../basic_check/datatype_check.h"
#include "../../basic_check/single_tensor_check.h"
#include "../../basic_check/multiple_tensor_check.h"
#include "../../../../../../../include/adv_api/quantization/ascend_antiquant_utils.h"

namespace AscendC {
namespace HighLevelApiCheck {
class CheckAscendAntiQuantParamsClass {
public:
    template <typename InputDataType, typename OutputDataType, bool isTranspose>
    __aicore__ inline void CheckAscendAntiQuantParams(const uint32_t K)
    {
        VerifyingParameters<InputDataType, OutputDataType, isTranspose>(K);
        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters<InputDataType, OutputDataType, isTranspose>(K);
        }
    }

private:
    template <typename InputDataType, typename OutputDataType, bool isTranspose>
    __aicore__ inline void VerifyingParameters(const uint32_t K)
    {
        if (isTranspose == true) {
            bool ans = K * sizeof(InputDataType) % ONE_BLK_SIZE == 0;
            ASCENDC_ASSERT((ans || HighLevelAPIParametersPrint), {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "[AscendAntiQuant] The result of K * sizeof(InputDataType) cannot be %lu, "
                    "should be an integer multiple of 32.",
                    K * sizeof(InputDataType));
            });
        }
    }

    template <typename InputDataType, typename OutputDataType, bool isTranspose>
    __aicore__ inline void PrintParameters(const uint32_t K)
    {
        KERNEL_LOG(KERNEL_INFO, "[AscendAntiQuant] The isTranspose is %d, K is %u.", isTranspose, K);
    }
};

class CheckAscendAntiQuantParamsChannelClass {
public:
    template <typename InputDataType, typename OutputDataType, bool isTranspose>
    __aicore__ inline void CheckAscendAntiQuantParams(
        const LocalTensor<OutputDataType>& scale, const AntiQuantShapeInfo& shapeInfo = {})
    {
        VerifyingParameters<InputDataType, OutputDataType, isTranspose>(scale, shapeInfo);
        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters<InputDataType, OutputDataType, isTranspose>(scale, shapeInfo);
        }
    }

private:
    template <typename InputDataType, typename OutputDataType, bool isTranspose>
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<OutputDataType>& scale, const AntiQuantShapeInfo& shapeInfo = {})
    {
        ASCENDC_ASSERT(
            (shapeInfo.scaleHeight * shapeInfo.scaleWidth <= scale.GetSize() || HighLevelAPIParametersPrint), {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "[AscendAntiQuant] The result of shapeInfo.scaleHeight * shapeInfo.scaleWidth is %u, "
                    "should not be greater than scale size %u.",
                    shapeInfo.scaleHeight * shapeInfo.scaleWidth, scale.GetSize());
            });
    }

    template <typename InputDataType, typename OutputDataType, bool isTranspose>
    __aicore__ inline void PrintParameters(
        const LocalTensor<OutputDataType>& scale, const AntiQuantShapeInfo& shapeInfo = {})
    {
        KERNEL_LOG(
            KERNEL_INFO, "[AscendAntiQuant] The shapeInfo.scaleHeight is %u, shapeInfo.scaleWidth is %u.",
            shapeInfo.scaleHeight, shapeInfo.scaleWidth);
    }
};

class CheckAscendAntiQuantParamsChannelOffsetClass {
public:
    template <typename InputDataType, typename OutputDataType, bool isTranspose>
    __aicore__ inline void CheckAscendAntiQuantParams(
        const LocalTensor<OutputDataType>& offset, const LocalTensor<OutputDataType>& scale,
        const AntiQuantShapeInfo& shapeInfo = {})
    {
        VerifyingParameters<InputDataType, OutputDataType, isTranspose>(offset, scale, shapeInfo);
        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters<InputDataType, OutputDataType, isTranspose>(offset, scale, shapeInfo);
        }
    }

private:
    template <typename InputDataType, typename OutputDataType, bool isTranspose>
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<OutputDataType>& offset, const LocalTensor<OutputDataType>& scale,
        const AntiQuantShapeInfo& shapeInfo = {})
    {
        ASCENDC_ASSERT(
            (shapeInfo.scaleHeight * shapeInfo.scaleWidth <= scale.GetSize() || HighLevelAPIParametersPrint), {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "[AscendAntiQuant] The result of shapeInfo.scaleHeight * shapeInfo.scaleWidth is %u, "
                    "should not be greater than scale size %u.",
                    shapeInfo.scaleHeight * shapeInfo.scaleWidth, scale.GetSize());
            });
        ASCENDC_ASSERT(
            (shapeInfo.offsetHeight * shapeInfo.offsetWidth <= offset.GetSize() || HighLevelAPIParametersPrint), {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "[AscendAntiQuant] The result of shapeInfo.offsetHeight * shapeInfo.offsetWidth is %u, "
                    "should not be greater than offset size %u.",
                    shapeInfo.offsetHeight * shapeInfo.offsetWidth, offset.GetSize());
            });
    }

    template <typename InputDataType, typename OutputDataType, bool isTranspose>
    __aicore__ inline void PrintParameters(
        const LocalTensor<OutputDataType>& offset, const LocalTensor<OutputDataType>& scale,
        const AntiQuantShapeInfo& shapeInfo = {})
    {
        KERNEL_LOG(
            KERNEL_INFO,
            "[AscendAntiQuant] The shapeInfo.scaleHeight is %u, shapeInfo.scaleWidth is %u, "
            "shapeInfo.offsetHeight is %u, shapeInfo.offsetWidth is %u.",
            shapeInfo.scaleHeight, shapeInfo.scaleWidth, shapeInfo.offsetHeight, shapeInfo.offsetWidth);
    }
};

template <typename InputDataType, typename OutputDataType, bool isTranspose>
class CheckFuncClassAscendAntiQuantChannel : public DataTypeCheckFuncBasicClass,
                                             public SingleTensorCheckFuncBasicClass,
                                             public MultipleTensorCheckFuncBasicClass,
                                             public CheckAscendAntiQuantParamsClass,
                                             public CheckAscendAntiQuantParamsChannelClass {
public:
    __aicore__ inline CheckFuncClassAscendAntiQuantChannel(){};
    __aicore__ inline CheckFuncClassAscendAntiQuantChannel(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<OutputDataType>& dst, const LocalTensor<InputDataType>& src,
        const LocalTensor<OutputDataType>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t K,
        const AntiQuantShapeInfo& shapeInfo = {})
    {
#if __NPU_ARCH__ == 2002
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<OutputDataType, half>(
            "second template parameter (OutputDataType) is not half");
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<InputDataType, int8_t>(
            "first template parameter (InputDataType) is not int8_t");
#elif __NPU_ARCH__ == 2201
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<InputDataType, int4b_t, int8_t>(
            "first template parameter (InputDataType) is not int8_t or int4b_t");
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<OutputDataType, half, bfloat16_t>(
            "second template parameter (OutputDataType) is not half or bfloat16_t");
#endif
        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dst, src, scale, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(VA_ARGS_TO_MAKE_TUPLE(dst, src));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(VA_ARGS_TO_MAKE_TUPLE(dst, scale));
        CheckAscendAntiQuantParamsClass::CheckAscendAntiQuantParams<InputDataType, OutputDataType, isTranspose>(K);
        CheckAscendAntiQuantParamsChannelClass::CheckAscendAntiQuantParams<InputDataType, OutputDataType, isTranspose>(
            scale, shapeInfo);
    };
};

template <typename InputDataType, typename OutputDataType, bool isTranspose>
class CheckFuncClassAscendAntiQuantTensor : public DataTypeCheckFuncBasicClass,
                                            public SingleTensorCheckFuncBasicClass,
                                            public MultipleTensorCheckFuncBasicClass,
                                            public CheckAscendAntiQuantParamsClass {
public:
    __aicore__ inline CheckFuncClassAscendAntiQuantTensor(){};
    __aicore__ inline CheckFuncClassAscendAntiQuantTensor(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<OutputDataType>& dst, const LocalTensor<InputDataType>& src, const OutputDataType scale,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t K, const AntiQuantShapeInfo& shapeInfo = {})
    {
#if __NPU_ARCH__ == 2002
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<InputDataType, int8_t>(
            "first template parameter (InputDataType) is not int8_t");
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<OutputDataType, half>(
            "second template parameter (OutputDataType) is not half");
#elif __NPU_ARCH__ == 2201
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<InputDataType, int4b_t, int8_t>(
            "first template parameter (InputDataType) is not int8_t or int4b_t");
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<OutputDataType, bfloat16_t, half>(
            "second template parameter (OutputDataType) is not half or bfloat16_t");
#endif
        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dst, src, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(VA_ARGS_TO_MAKE_TUPLE(dst, src));
        CheckAscendAntiQuantParamsClass::CheckAscendAntiQuantParams<InputDataType, OutputDataType, isTranspose>(K);
    };
};

template <typename InputDataType, typename OutputDataType, bool isTranspose>
class CheckFuncClassAscendAntiQuantChannelOffset : public DataTypeCheckFuncBasicClass,
                                                   public SingleTensorCheckFuncBasicClass,
                                                   public MultipleTensorCheckFuncBasicClass,
                                                   public CheckAscendAntiQuantParamsClass,
                                                   public CheckAscendAntiQuantParamsChannelOffsetClass {
public:
    __aicore__ inline CheckFuncClassAscendAntiQuantChannelOffset(){};
    __aicore__ inline CheckFuncClassAscendAntiQuantChannelOffset(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<OutputDataType>& dst, const LocalTensor<InputDataType>& src,
        const LocalTensor<OutputDataType>& offset, const LocalTensor<OutputDataType>& scale,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t K, const AntiQuantShapeInfo& shapeInfo = {})
    {
#if __NPU_ARCH__ == 2002
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<InputDataType, int8_t>(
            "first template parameter (InputDataType) is not int8_t");
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<OutputDataType, half>(
            "second template parameter (OutputDataType) is not half");
#elif __NPU_ARCH__ == 2201
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<InputDataType, int8_t, int4b_t>(
            "first template parameter (InputDataType) is not int8_t or int4b_t");
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<OutputDataType, half, bfloat16_t>(
            "second template parameter (OutputDataType) is not half or bfloat16_t");
#endif
        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dst, src, offset, scale, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(VA_ARGS_TO_MAKE_TUPLE(dst, src));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(VA_ARGS_TO_MAKE_TUPLE(dst, offset));
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(VA_ARGS_TO_MAKE_TUPLE(dst, scale));
        CheckAscendAntiQuantParamsClass::CheckAscendAntiQuantParams<InputDataType, OutputDataType, isTranspose>(K);
        CheckAscendAntiQuantParamsChannelOffsetClass::CheckAscendAntiQuantParams<
            InputDataType, OutputDataType, isTranspose>(offset, scale, shapeInfo);
    };
};

template <typename InputDataType, typename OutputDataType, bool isTranspose>
class CheckFuncClassAscendAntiQuantTensorOffset : public DataTypeCheckFuncBasicClass,
                                                  public SingleTensorCheckFuncBasicClass,
                                                  public MultipleTensorCheckFuncBasicClass,
                                                  public CheckAscendAntiQuantParamsClass {
public:
    __aicore__ inline CheckFuncClassAscendAntiQuantTensorOffset(){};
    __aicore__ inline CheckFuncClassAscendAntiQuantTensorOffset(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<OutputDataType>& dst, const LocalTensor<InputDataType>& src, const OutputDataType offset,
        const OutputDataType scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t K,
        const AntiQuantShapeInfo& shapeInfo = {})
    {
#if __NPU_ARCH__ == 2002
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<InputDataType, int8_t>(
            "first template parameter (InputDataType) is not int8_t");
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<OutputDataType, half>(
            "second template parameter (OutputDataType) is not half");
#elif __NPU_ARCH__ == 2201
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<OutputDataType, half, bfloat16_t>(
            "second template parameter (OutputDataType) is not half or bfloat16_t");
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<InputDataType, int8_t, int4b_t>(
            "first template parameter (InputDataType) is not int8_t or int4b_t");
#endif
        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dst, src, sharedTmpBuffer),
            MakeParameters2Tuple(TPosition::VECOUT, TPosition::VECIN, TPosition::VECCALC), "UB(VECOUT/VECIN/VECCALC)");

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(VA_ARGS_TO_MAKE_TUPLE(dst, src));
        CheckAscendAntiQuantParamsClass::CheckAscendAntiQuantParams<InputDataType, OutputDataType, isTranspose>(K);
    };
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_QUANTIZATION_ANTIQUANT_ANTIQUANT_CHECK_COMMON_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ANTIQUANT_CHECK_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ANTIQUANT_CHECK_COMMON_H__
#endif
