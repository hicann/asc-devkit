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
 * \file softmax_flashv3_check_aicore.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/activation/softmax/softmax_flashv3/softmax_flashv3_check_aicore.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/softmaxflashv3.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASHV3_CHECK_AICORE_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_SOFTMAX_SOFTMAX_FLASHV3_SOFTMAX_FLASHV3_CHECK_AICORE_H_
#define IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_SOFTMAX_SOFTMAX_FLASHV3_SOFTMAX_FLASHV3_CHECK_AICORE_H_

#include "kernel_tiling/kernel_tiling.h"
#include "../../../basic_check/basic_check_utils.h"
#include "../../../../../../../../include/adv_api/activation/softmax_utils.h"

namespace AscendC {
namespace HighLevelApiCheck {
constexpr uint32_t CHECK_SOFTMAXFLASHV3_SRCK = 512;
constexpr uint32_t CHECK_SOFTMAXFLASHV3_SRCK_ALIGN = 64;
constexpr uint32_t CHECK_SOFTMAXFLASHV3_SRCM_ALIGN = 8;

template <
    typename T, typename U, bool isUpdate, bool isReuseSource, bool isBasicBlock, bool isDataFormatNZ,
    const SoftmaxConfig& config>
class CheckFuncClassSoftmaxFlashV3 {
public:
    __aicore__ inline CheckFuncClassSoftmaxFlashV3(){};
    __aicore__ inline CheckFuncClassSoftmaxFlashV3(__gm__ const char* apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstTensor, const LocalTensor<U>& meanTensor, const LocalTensor<U>& expSumTensor,
        const LocalTensor<U>& maxTensor, const LocalTensor<T>& srcTensor, const LocalTensor<T>& expMaxTensor,
        const LocalTensor<U>& inMeanTensor, const LocalTensor<U>& inexpSumTensor, const LocalTensor<U>& inMaxTensor,
        const LocalTensor<float>& sharedTmpBuffer, const SoftMaxTiling& tiling, const SoftMaxParams& params)
    {
        CheckTensorPos<float>(
            sharedTmpBuffer, Hardware::UB, "sharedTmpBuffer", "VECIN/VECCALC/VECOUT", "SoftmaxFlashV3");
        CheckTensorPos<T>(dstTensor, Hardware::UB, "dstTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlashV3");
        CheckTensorPos<U>(meanTensor, Hardware::UB, "meanTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlashV3");
        CheckTensorPos<U>(expSumTensor, Hardware::UB, "expSumTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlashV3");
        CheckTensorPos<U>(maxTensor, Hardware::UB, "maxTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlashV3");
        CheckTensorPos<T>(srcTensor, Hardware::UB, "srcTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlashV3");
        CheckTensorPos<T>(expMaxTensor, Hardware::UB, "expMaxTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlashV3");
        CheckTensorPos<U>(inMeanTensor, Hardware::UB, "inMeanTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlashV3");
        CheckTensorPos<U>(inexpSumTensor, Hardware::UB, "inexpSumTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlashV3");
        CheckTensorPos<U>(inMaxTensor, Hardware::UB, "inMaxTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlashV3");
        ASCENDC_ASSERT((params.srcK * sizeof(T) % ONE_BLK_SIZE == 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(KERNEL_ERROR, "[SoftmaxFlashV3] The params.srcK is %u, should be 32B aligned.", params.srcK);
        });
        ASCENDC_ASSERT((params.srcK * params.srcM <= srcTensor.GetSize() || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "[SoftmaxFlashV3] The params.srcK is %u, params.srcM is %u, "
                "the product of params.srcM and params.srcK should not be greater than srcTensor size %u.",
                params.srcK, params.srcM, srcTensor.GetSize());
        });
        ASCENDC_ASSERT((dstTensor.GetSize() == srcTensor.GetSize() || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[SoftmaxFlashV3] The dstTensor size is %u, should be equal to srcTensor size %u.",
                dstTensor.GetSize(), srcTensor.GetSize());
        });
        if constexpr (isUpdate) {
            ASCENDC_ASSERT((params.loopCnt >= 1 || HighLevelAPIParametersPrint), {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "[SoftmaxFlashV3] The params.loopCnt is %u, should be no less than 1 when isUpdate is true.",
                    params.loopCnt);
            });
        }
        ASCENDC_ASSERT((params.splitMeanCnt == 8 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[SoftmaxFlashV3] The params.splitMeanCnt is %u, should be equal to 8.",
                params.splitMeanCnt);
        });
        ASCENDC_ASSERT((params.srcM == params.oriSrcM || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[SoftmaxFlashV3] The params.srcM is %u, params.oriSrcM is %u, they should be equal.",
                params.srcM, params.oriSrcM);
        });
        ASCENDC_ASSERT((params.srcK == params.oriSrcK || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[SoftmaxFlashV3] The params.srcK is %u, params.oriSrcK is %u, they should be equal.",
                params.srcK, params.oriSrcK);
        });
        ASCENDC_ASSERT(
            ((params.srcK >= CHECK_SOFTMAXFLASHV3_SRCK && params.srcK % CHECK_SOFTMAXFLASHV3_SRCK_ALIGN == 0) ||
             HighLevelAPIParametersPrint),
            {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "[SoftmaxFlashV3] The params.srcK is %u, should be greater than or equal to 512, "
                    "and is an integer multiple of 64.",
                    params.srcK);
            });
        ASCENDC_ASSERT((params.srcM % CHECK_SOFTMAXFLASHV3_SRCM_ALIGN == 0 || HighLevelAPIParametersPrint), {
            KERNEL_LOG(
                KERNEL_ERROR, "[SoftmaxFlashV3] The params.srcM is %u, should be an integer multiple of 8.",
                params.srcM);
        });

        bool ans = config.isCheckTiling == true && config.oriSrcM == 0 && config.oriSrcK == 0 &&
                   config.mode == SoftmaxMode::SOFTMAX_NORMAL;
        ASCENDC_LOG_IF_CHECK((ans || HighLevelAPIParametersPrint), {
            KERNEL_LOG(KERNEL_WARN, "[SoftmaxFlashV3] The config must be the default value SOFTMAX_DEFAULT_CFG.");
        });
    };
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_SOFTMAX_SOFTMAX_FLASHV3_SOFTMAX_FLASHV3_CHECK_AICORE_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASHV3_CHECK_AICORE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASHV3_CHECK_AICORE_H__
#endif
