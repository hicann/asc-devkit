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
 * \file softmax_flash_check_aicore.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/activation/softmax/softmax_flash/softmax_flash_check_aicore.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/softmaxflash.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASH_CHECK_AICORE_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_SOFTMAX_SOFTMAX_FLASH_SOFTMAX_FLASH_CHECK_AICORE_H_
#define IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_SOFTMAX_SOFTMAX_FLASH_SOFTMAX_FLASH_CHECK_AICORE_H_

#include "kernel_tiling/kernel_tiling.h"
#include "../../../../../../../../include/adv_api/activation/softmax_utils.h"

namespace AscendC {
namespace HighLevelApiCheck {

template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
class CheckFuncClassSoftmaxFlashSameT {
public:
    __aicore__ inline CheckFuncClassSoftmaxFlashSameT(){};
    __aicore__ inline CheckFuncClassSoftmaxFlashSameT(__gm__ const char* apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& sumTensor, const LocalTensor<T>& maxTensor,
        const LocalTensor<T>& srcTensor, const LocalTensor<T>& expMaxTensor, const LocalTensor<T>& inSumTensor,
        const LocalTensor<T>& inMaxTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling,
        bool isUpdate, const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        CheckTensorPos<T>(dstTensor, Hardware::UB, "dstTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        CheckTensorPos<T>(sumTensor, Hardware::UB, "sumTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        CheckTensorPos<T>(maxTensor, Hardware::UB, "maxTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        CheckTensorPos<T>(srcTensor, Hardware::UB, "srcTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        CheckTensorPos<T>(expMaxTensor, Hardware::UB, "expMaxTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        CheckTensorPos<T>(inSumTensor, Hardware::UB, "inSumTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        CheckTensorPos<T>(inMaxTensor, Hardware::UB, "inMaxTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        CheckTensorPos<uint8_t>(
            sharedTmpBuffer, Hardware::UB, "sharedTmpBuffer", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        ASCENDC_ASSERT((softmaxShapeInfo.srcK * sizeof(T) % ONE_BLK_SIZE == 0), {
            KERNEL_LOG(KERNEL_ERROR, "srcK should be 32B aligned, current srcK is %u", softmaxShapeInfo.srcK);
        });

        static_assert(
            std::is_same<T, half>::value || std::is_same<T, float>::value,
            "This Related Api of Softmax only support half/float input dtype");
    };
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<T>& dstTensor, const LocalTensor<T>& sumTensor, const LocalTensor<T>& maxTensor,
        const LocalTensor<T>& srcTensor, const LocalTensor<T>& expMaxTensor, const LocalTensor<T>& inSumTensor,
        const LocalTensor<T>& inMaxTensor, const SoftMaxTiling& tiling, bool isUpdate,
        const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        CheckTensorPos<T>(dstTensor, Hardware::UB, "dstTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        CheckTensorPos<T>(sumTensor, Hardware::UB, "sumTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        CheckTensorPos<T>(maxTensor, Hardware::UB, "maxTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        CheckTensorPos<T>(srcTensor, Hardware::UB, "srcTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        CheckTensorPos<T>(expMaxTensor, Hardware::UB, "expMaxTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        CheckTensorPos<T>(inSumTensor, Hardware::UB, "inSumTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        CheckTensorPos<T>(inMaxTensor, Hardware::UB, "inMaxTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        ASCENDC_ASSERT((softmaxShapeInfo.srcK * sizeof(T) % ONE_BLK_SIZE == 0), {
            KERNEL_LOG(KERNEL_ERROR, "srcK should be 32B aligned, current srcK is %u", softmaxShapeInfo.srcK);
        });

        static_assert(
            std::is_same<T, half>::value || std::is_same<T, float>::value,
            "This Related Api of Softmax only support half/float input dtype");
    };
};

template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
class CheckFuncClassSoftmaxFlash {
public:
    __aicore__ inline CheckFuncClassSoftmaxFlash(){};
    __aicore__ inline CheckFuncClassSoftmaxFlash(__gm__ const char* apiName){};

public:
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<half>& dstTensor, const LocalTensor<float>& sumTensor, const LocalTensor<float>& maxTensor,
        const LocalTensor<half>& srcTensor, const LocalTensor<half>& expMaxTensor,
        const LocalTensor<float>& inSumTensor, const LocalTensor<float>& inMaxTensor,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling, bool isUpdate,
        const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        CheckTensorPos<half>(dstTensor, Hardware::UB, "dstTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        CheckTensorPos<float>(sumTensor, Hardware::UB, "sumTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        CheckTensorPos<float>(maxTensor, Hardware::UB, "maxTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        CheckTensorPos<half>(srcTensor, Hardware::UB, "srcTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        CheckTensorPos<half>(expMaxTensor, Hardware::UB, "expMaxTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        CheckTensorPos<float>(inSumTensor, Hardware::UB, "inSumTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        CheckTensorPos<float>(inMaxTensor, Hardware::UB, "inMaxTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        CheckTensorPos<uint8_t>(
            sharedTmpBuffer, Hardware::UB, "sharedTmpBuffer", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        ASCENDC_ASSERT((softmaxShapeInfo.srcK * sizeof(half) % ONE_BLK_SIZE == 0), {
            KERNEL_LOG(KERNEL_ERROR, "srcK should be 32B aligned, current srcK is %u", softmaxShapeInfo.srcK);
        });
    };
    __aicore__ inline void VerifyingParameters(
        const LocalTensor<half>& dstTensor, const LocalTensor<float>& sumTensor, const LocalTensor<float>& maxTensor,
        const LocalTensor<half>& srcTensor, const LocalTensor<half>& expMaxTensor,
        const LocalTensor<float>& inSumTensor, const LocalTensor<float>& inMaxTensor, const SoftMaxTiling& tiling,
        bool isUpdate, const SoftMaxShapeInfo& softmaxShapeInfo)
    {
        CheckTensorPos<half>(dstTensor, Hardware::UB, "dstTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        CheckTensorPos<float>(sumTensor, Hardware::UB, "sumTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        CheckTensorPos<float>(maxTensor, Hardware::UB, "maxTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        CheckTensorPos<half>(srcTensor, Hardware::UB, "srcTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        CheckTensorPos<half>(expMaxTensor, Hardware::UB, "expMaxTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        CheckTensorPos<float>(inSumTensor, Hardware::UB, "inSumTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        CheckTensorPos<float>(inMaxTensor, Hardware::UB, "inMaxTensor", "VECIN/VECCALC/VECOUT", "SoftmaxFlash");
        ASCENDC_ASSERT((softmaxShapeInfo.srcK * sizeof(half) % ONE_BLK_SIZE == 0), {
            KERNEL_LOG(KERNEL_ERROR, "srcK should be 32B aligned, current srcK is %u", softmaxShapeInfo.srcK);
        });
    };
};

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_ACTIVATION_SOFTMAX_SOFTMAX_FLASH_SOFTMAX_FLASH_CHECK_AICORE_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASH_CHECK_AICORE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASH_CHECK_AICORE_H__
#endif
