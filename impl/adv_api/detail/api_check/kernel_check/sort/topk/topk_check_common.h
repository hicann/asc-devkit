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
 * \file topk_check_common.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/sort/topk/topk_check_common.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/sort/topk.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TOPK_CHECK_COMMON_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_SORT_TOPK_TOPK_CHECK_COMMON_H_
#define IMPL_API_CHECK_KERNEL_CHECK_SORT_TOPK_TOPK_CHECK_COMMON_H_

#include "../../basic_check/datatype_check.h"
#include "../../basic_check/reuse_source_check.h"
#include "../../basic_check/single_tensor_check.h"
#include "../../basic_check/multiple_tensor_check.h"
#include "../../../../../../../include/adv_api/sort/topk_utils.h"

namespace AscendC {
namespace HighLevelApiCheck {
constexpr uint32_t CHECK_TOPK_NSMALL_INNER_LEN = 32;
constexpr uint32_t CHECK_TOPK_NORMAL_INNER_MAX_LEN = 4096;
constexpr uint32_t CHECK_TOPK_NORMAL_INNER_MAX_HALF_LEN = 2048;

class CheckTopKParamsClass {
public:
    template <
        typename T, bool isInitIndex = false, bool isHasfinish = false, bool isReuseSrc = false,
        enum TopKMode topkMode = TopKMode::TOPK_NORMAL>
    __aicore__ inline void CheckTopKParams(
        const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal,
        const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<bool>& finishLocal, const int32_t k,
        const TopkTiling& tilling, const TopKInfo& topKInfo, const bool isLargest = true)
    {
        VerifyingParameters<T, isInitIndex, isHasfinish, isReuseSrc, topkMode>(
            dstValueLocal, dstIndexLocal, srcLocal, srcIndexLocal, finishLocal, k, tilling, topKInfo, isLargest);
        if constexpr (HighLevelAPIParametersPrint) {
            PrintParameters<T, isInitIndex, isHasfinish, isReuseSrc, topkMode>(
                dstValueLocal, dstIndexLocal, srcLocal, srcIndexLocal, finishLocal, k, tilling, topKInfo, isLargest);
        }
    }

private:
    template <
        typename T, bool isInitIndex = false, bool isHasfinish = false, bool isReuseSrc = false,
        enum TopKMode topkMode = TopKMode::TOPK_NORMAL>
    __aicore__ inline void VerifyingParametersNormal(
        const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal,
        const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<bool>& finishLocal, const int32_t k,
        const TopkTiling& tilling, const TopKInfo& topKInfo, const bool isLargest = true)
    {
        ASCENDC_ASSERT((topKInfo.inner <= CHECK_TOPK_NORMAL_INNER_MAX_LEN) || HighLevelAPIParametersPrint, {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[TopK] The value of topKInfo.inner is %u, should be less than or equal to 4096 "
                "when topkMode is TOPK_NORMAL.",
                topKInfo.inner);
        });
        ASCENDC_ASSERT(srcIndexLocal.GetSize() >= topKInfo.inner || HighLevelAPIParametersPrint, {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[TopK] The topKInfo.inner parameter is %u, "
                "should be less than or equal to the srcIndexLocal size %u when topkMode is TOPK_NORMAL.",
                topKInfo.inner, srcIndexLocal.GetSize());
        });
        if (isHasfinish == true) {
            ASCENDC_ASSERT((finishLocal.GetSize() >= topKInfo.outter) || HighLevelAPIParametersPrint, {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "[TopK] The topKInfo.outter parameter is %u, "
                    "should be less than or equal to the finishLocal size %u when isHasfinish is true.",
                    topKInfo.outter, finishLocal.GetSize());
            });
        }
        uint32_t k_pad = (k + (ONE_BLK_SIZE / sizeof(T)) - 1) / (ONE_BLK_SIZE / sizeof(T)) * (ONE_BLK_SIZE / sizeof(T));
        ASCENDC_ASSERT(dstValueLocal.GetSize() >= k_pad * topKInfo.outter || HighLevelAPIParametersPrint, {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[TopK] The result of k_pad * topKInfo.outter is %u, should be less than or equal to dstValueLocal "
                "size %u "
                "when topkMode is TOPK_NORMAL.",
                k_pad * topKInfo.outter, dstValueLocal.GetSize());
        });
        uint32_t kpad_index = (k + (ONE_BLK_SIZE / sizeof(int32_t)) - 1) / (ONE_BLK_SIZE / sizeof(int32_t)) *
                              (ONE_BLK_SIZE / sizeof(int32_t));
        ASCENDC_ASSERT(dstIndexLocal.GetSize() >= kpad_index * topKInfo.outter || HighLevelAPIParametersPrint, {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[TopK] The result of kpad_index * topKInfo.outter is %u, "
                "should be less than or equal to dstIndexLocal size %u when topkMode is TOPK_NORMAL.",
                kpad_index * topKInfo.outter, dstIndexLocal.GetSize());
        });
    }

    template <
        typename T, bool isInitIndex = false, bool isHasfinish = false, bool isReuseSrc = false,
        enum TopKMode topkMode = TopKMode::TOPK_NORMAL>
    __aicore__ inline void VerifyingParametersSmall(
        const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal,
        const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<bool>& finishLocal, const int32_t k,
        const TopkTiling& tilling, const TopKInfo& topKInfo, const bool isLargest = true)
    {
        ASCENDC_ASSERT((topKInfo.inner == CHECK_TOPK_NSMALL_INNER_LEN) || HighLevelAPIParametersPrint, {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[TopK] The value of topKInfo.inner is %u, should be equal to 32 "
                "when topkMode is TOPK_NSMALL.",
                topKInfo.inner);
        });
        ASCENDC_ASSERT(isHasfinish == false || HighLevelAPIParametersPrint, {
            KERNEL_LOG(
                KERNEL_ERROR, "[TopK] The value of isHasfinish is %d, should be false when topkMode is TOPK_NSMALL.",
                isHasfinish);
        });

        ASCENDC_ASSERT(srcIndexLocal.GetSize() >= topKInfo.inner * topKInfo.outter || HighLevelAPIParametersPrint, {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[TopK] The result of topKInfo.inner * topKInfo.outter is %u, "
                "should be less than or equal to the srcIndexLocal size %u when topkMode is TOPK_NSMALL.",
                topKInfo.inner * topKInfo.outter, srcIndexLocal.GetSize());
        });

        ASCENDC_ASSERT(dstValueLocal.GetSize() >= k * topKInfo.outter || HighLevelAPIParametersPrint, {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[TopK] The result of k * topKInfo.outter is %u, "
                "should be less than or equal to the dstValueLocal size %u when topkMode is TOPK_NSMALL.",
                k * topKInfo.outter, dstValueLocal.GetSize());
        });
        ASCENDC_ASSERT(dstIndexLocal.GetSize() >= k * topKInfo.outter || HighLevelAPIParametersPrint, {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[TopK] The result of k * topKInfo.outter is %u, "
                "should be less than or equal to the dstIndexLocal size %u when topkMode is TOPK_NSMALL.",
                k * topKInfo.outter, dstIndexLocal.GetSize());
        });
    }

    template <
        typename T, bool isInitIndex = false, bool isHasfinish = false, bool isReuseSrc = false,
        enum TopKMode topkMode = TopKMode::TOPK_NORMAL>
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal,
        const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<bool>& finishLocal, const int32_t k,
        const TopkTiling& tilling, const TopKInfo& topKInfo, const bool isLargest = true)
    {
        ASCENDC_ASSERT(((1 <= k) && (k <= topKInfo.n)) || HighLevelAPIParametersPrint, {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[TopK] The value of k is %u, should be greater than or equal to 1 and less than or equal to "
                "topKInfo.n %u.",
                k, topKInfo.n);
        });

        ASCENDC_ASSERT(((1 <= topKInfo.n) && (topKInfo.n <= topKInfo.inner)) || HighLevelAPIParametersPrint, {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[TopK] The value of topKInfo.n is %u, should be greater than or equal to 1 and less than or equal to "
                "topKInfo.inner %u.",
                topKInfo.n, topKInfo.inner);
        });

        ASCENDC_ASSERT((topKInfo.inner % ONE_BLK_SIZE == 0) || HighLevelAPIParametersPrint, {
            KERNEL_LOG(
                KERNEL_ERROR, "[TopK] The value of topKInfo.inner is %u, should be an integer multiple of 32.",
                topKInfo.inner);
        });

        ASCENDC_ASSERT(srcLocal.GetSize() >= topKInfo.inner * topKInfo.outter || HighLevelAPIParametersPrint, {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[TopK] The result of topKInfo.inner * topKInfo.outter is %u, "
                "should be less than or equal to the srcLocal size %u.",
                topKInfo.inner * topKInfo.outter, srcLocal.GetSize());
        });

        if constexpr (topkMode == TopKMode::TOPK_NORMAL) {
            VerifyingParametersNormal<T, isInitIndex, isHasfinish, isReuseSrc>(
                dstValueLocal, dstIndexLocal, srcLocal, srcIndexLocal, finishLocal, k, tilling, topKInfo, isLargest);
        }

        if constexpr (topkMode == TopKMode::TOPK_NSMALL) {
            VerifyingParametersSmall<T, isInitIndex, isHasfinish, isReuseSrc>(
                dstValueLocal, dstIndexLocal, srcLocal, srcIndexLocal, finishLocal, k, tilling, topKInfo, isLargest);
        }

#if __NPU_ARCH__ == 2002
        if ((sizeof(T) == sizeof(half)) && (!isInitIndex)) {
            ASCENDC_ASSERT((topKInfo.inner <= CHECK_TOPK_NORMAL_INNER_MAX_HALF_LEN) || HighLevelAPIParametersPrint, {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "[TopK] The value of topKInfo.inner is %u, should be less than or equal to 2048 "
                    "when T is half and isInitIndex is false in current device.",
                    topKInfo.inner);
            });
        }
#endif
    }

    template <
        typename T, bool isInitIndex = false, bool isHasfinish = false, bool isReuseSrc = false,
        enum TopKMode topkMode = TopKMode::TOPK_NORMAL>
    __aicore__ inline void PrintParameters(
        const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal,
        const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<bool>& finishLocal, const int32_t k,
        const TopkTiling& tilling, const TopKInfo& topKInfo, const bool isLargest = true)
    {
        KERNEL_LOG(
            KERNEL_INFO,
            "[TopK] The value of k is %u, topKInfo.outter is %u, topKInfo.inner is %u, "
            "topKInfo.n is %u.",
            k, topKInfo.outter, topKInfo.inner, topKInfo.n);
    }
};

template <
    typename T, bool isInitIndex = false, bool isHasfinish = false, bool isReuseSrc = false,
    enum TopKMode topkMode = TopKMode::TOPK_NORMAL>
class CheckFuncClassTopKTmpBuf : public DataTypeCheckFuncBasicClass,
                                 public ReuseSourceCheckFuncBasicClass,
                                 public SingleTensorCheckFuncBasicClass,
                                 public MultipleTensorCheckFuncBasicClass,
                                 public CheckTopKParamsClass {
public:
    __aicore__ inline CheckFuncClassTopKTmpBuf(){};
    __aicore__ inline CheckFuncClassTopKTmpBuf(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          ReuseSourceCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal,
        const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<bool>& finishLocal,
        const LocalTensor<uint8_t>& tmpLocal, const int32_t k, const TopkTiling& tilling, const TopKInfo& topKInfo,
        const bool isLargest = true)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, half, float>(
            "template parameter (T) is not half or float");

        ReuseSourceCheckFuncBasicClass::IsReuseSourceVerifyingParameters<false>(ARG_AND_STRING(isReuseSrc));

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstValueLocal, dstIndexLocal, srcLocal, srcIndexLocal, finishLocal),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(tmpLocal), MakeParameters2Tuple(TPosition::VECCALC), "UB(VECCALC)");

        CheckTopKParamsClass::CheckTopKParams<T, isInitIndex, isHasfinish, isReuseSrc, topkMode>(
            dstValueLocal, dstIndexLocal, srcLocal, srcIndexLocal, finishLocal, k, tilling, topKInfo, isLargest);
        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstValueLocal, dstIndexLocal, srcLocal));

        if (isInitIndex) {
            SingleTensorCheckFuncBasicClass::TensorSizeVerifyingParameters(VA_ARGS_TO_MAKE_TUPLE(srcIndexLocal));
            MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
                VA_ARGS_TO_MAKE_TUPLE(dstValueLocal, dstIndexLocal, srcIndexLocal));
        }
        if (isHasfinish) {
            MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
                VA_ARGS_TO_MAKE_TUPLE(dstValueLocal, dstIndexLocal, finishLocal));
            SingleTensorCheckFuncBasicClass::TensorSizeVerifyingParameters(VA_ARGS_TO_MAKE_TUPLE(finishLocal));
        }
    };
};

template <
    typename T, bool isInitIndex = false, bool isHasfinish = false, bool isReuseSrc = false,
    enum TopKMode topkMode = TopKMode::TOPK_NORMAL>
class CheckFuncClassTopK : public DataTypeCheckFuncBasicClass,
                           public ReuseSourceCheckFuncBasicClass,
                           public SingleTensorCheckFuncBasicClass,
                           public MultipleTensorCheckFuncBasicClass,
                           public CheckTopKParamsClass {
public:
    __aicore__ inline CheckFuncClassTopK(){};
    __aicore__ inline CheckFuncClassTopK(__gm__ const char* apiName)
        : DataTypeCheckFuncBasicClass(apiName),
          ReuseSourceCheckFuncBasicClass(apiName),
          SingleTensorCheckFuncBasicClass(apiName),
          MultipleTensorCheckFuncBasicClass(apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal,
        const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<bool>& finishLocal, const int32_t k,
        const TopkTiling& tilling, const TopKInfo& topKInfo, const bool isLargest = true)
    {
        DataTypeCheckFuncBasicClass::DataTypeVerifyingParameters<T, half, float>(
            "template parameter (T) is not half or float");

        ReuseSourceCheckFuncBasicClass::IsReuseSourceVerifyingParameters<false>(ARG_AND_STRING(isReuseSrc));

        SingleTensorCheckFuncBasicClass::TensorVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstValueLocal, dstIndexLocal, srcLocal, srcIndexLocal, finishLocal),
            MakeParameters2Tuple(TPosition::VECIN, TPosition::VECOUT, TPosition::VECCALC), "UB(VECIN/VECOUT/VECCALC)");

        MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
            VA_ARGS_TO_MAKE_TUPLE(dstValueLocal, dstIndexLocal, srcLocal));

        CheckTopKParamsClass::CheckTopKParams<T, isInitIndex, isHasfinish, isReuseSrc, topkMode>(
            dstValueLocal, dstIndexLocal, srcLocal, srcIndexLocal, finishLocal, k, tilling, topKInfo, isLargest);

        if (isInitIndex) {
            MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
                VA_ARGS_TO_MAKE_TUPLE(dstValueLocal, dstIndexLocal, srcIndexLocal));
            SingleTensorCheckFuncBasicClass::TensorSizeVerifyingParameters(VA_ARGS_TO_MAKE_TUPLE(srcIndexLocal));
        }
        if (isHasfinish) {
            MultipleTensorCheckFuncBasicClass::TensorReuseVerifyingParameters(
                VA_ARGS_TO_MAKE_TUPLE(dstValueLocal, dstIndexLocal, finishLocal));
            SingleTensorCheckFuncBasicClass::TensorSizeVerifyingParameters(VA_ARGS_TO_MAKE_TUPLE(finishLocal));
        }
    };
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_SORT_TOPK_TOPK_CHECK_COMMON_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TOPK_CHECK_COMMON_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_TOPK_CHECK_COMMON_H__
#endif
