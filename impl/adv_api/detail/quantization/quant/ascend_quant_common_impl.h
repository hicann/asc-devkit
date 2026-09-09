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
 * \file ascend_quant_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/quantization/quant/ascend_quant_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/quantization/ascend_quant.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_QUANT_ASCEND_QUANT_COMMON_IMPL_H__
#endif

#ifndef IMPL_QUANTIZATION_QUANT_ASCEND_QUANT_COMMON_IMPL_H
#define IMPL_QUANTIZATION_QUANT_ASCEND_QUANT_COMMON_IMPL_H

#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../basic_api/kernel_pop_stack_buffer.h"
#include "kernel_tiling/kernel_tiling.h"

#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/quantization/quant/quant_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
#include "ascend_quant_v220_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 2002
#include "ascend_quant_v200_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 1001
#include "ascend_quant_v100_impl.h"
#elif defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102)
#include "ascend_quant_3510_impl.h"
#include "ascend_quant_per_group_3510_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "ascend_quant_l300_impl.h"
#include "ascend_quant_per_group_l300_impl.h"
#endif

namespace AscendC {
template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
__aicore__ inline void AscendQuantImpl(
    const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor, const float scale, const float offset,
    uint32_t calCount)
{
    LocalTensor<uint8_t> stackTensor;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(stackTensor);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    AscendQuantImpl<T, isReuseSource, config>(dstTensor, srcTensor, stackTensor, scale, offset, calCount);
}

template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
__aicore__ inline void AscendQuantImpl(
    const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<T>& scaleTensor,
    const T offset, const uint32_t scaleCount, const uint32_t calCount)
{
    LocalTensor<uint8_t> stackTensor;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(stackTensor);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    AscendQuantImpl<T, isReuseSource, config>(
        dstTensor, srcTensor, stackTensor, scaleTensor, offset, scaleCount, calCount);
}

template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
__aicore__ inline void AscendQuantImpl(
    const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<T>& scaleTensor,
    const LocalTensor<T>& offsetTensor, const uint32_t scaleCount, const uint32_t offsetCount, const uint32_t calCount)
{
    LocalTensor<uint8_t> stackTensor;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(stackTensor);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    AscendQuantImpl<T, isReuseSource, config>(
        dstTensor, srcTensor, stackTensor, scaleTensor, offsetTensor, scaleCount, offsetCount, calCount);
}

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
template <typename dstT, typename srcT, bool isReuseSource = false>
__aicore__ inline void AscendQuantImpl(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const float scale, const float offset,
    const uint32_t calCount)
{
    if ASCEND_IS_AIC {
        return;
    }
    LocalTensor<uint8_t> stackTensor;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(stackTensor);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    AscendQuantImpl<dstT, srcT, isReuseSource>(dstTensor, srcTensor, stackTensor, scale, offset, calCount);
}
template <typename dstT, typename srcT, bool isReuseSource = false>
__aicore__ inline void AscendQuantImpl(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<srcT>& scaleTensor,
    const LocalTensor<srcT>& offsetTensor, const uint32_t scaleCount, const uint32_t offsetCount,
    const uint32_t calCount)
{
    if ASCEND_IS_AIC {
        return;
    }
    LocalTensor<uint8_t> stackTensor;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(stackTensor);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    AscendQuantImpl<dstT, srcT, isReuseSource>(
        dstTensor, srcTensor, stackTensor, scaleTensor, offsetTensor, scaleCount, offsetCount, calCount);
}

template <typename dstT, typename srcT, bool isReuseSource = false>
__aicore__ inline void AscendQuantImpl(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<srcT>& scaleTensor,
    const srcT offset, const uint32_t scaleCount, const uint32_t calCount)
{
    if ASCEND_IS_AIC {
        return;
    }
    LocalTensor<uint8_t> stackTensor;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(stackTensor);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    AscendQuantImpl<dstT, srcT, isReuseSource>(
        dstTensor, srcTensor, stackTensor, scaleTensor, offset, scaleCount, calCount);
}

#if ASCENDC_CPU_DEBUG
template <typename T>
__aicore__ inline void IsQuantParamValid(
    const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const LocalTensor<T>& scaleTensor, const LocalTensor<T>& offsetTensor, const uint32_t scaleCount,
    const uint32_t offsetCount, const uint32_t calCount)
{
    ASCENDC_ASSERT((calCount <= srcTensor.GetSize()), {
        KERNEL_LOG(
            KERNEL_ERROR, "calCount is %u, which should not larger than srcTensor size %u.", calCount,
            srcTensor.GetSize());
    });
    ASCENDC_ASSERT((scaleCount <= scaleTensor.GetSize()), {
        KERNEL_LOG(
            KERNEL_ERROR, "scaleCount is %u, which should not larger than scaleTensor size %u.", scaleCount,
            scaleTensor.GetSize());
    });
    ASCENDC_ASSERT((offsetCount <= offsetTensor.GetSize()), {
        KERNEL_LOG(
            KERNEL_ERROR, "offsetCount is %u, which should not larger than offsetTensor size %u.", offsetCount,
            offsetTensor.GetSize());
    });
    ASCENDC_ASSERT((scaleCount == offsetCount && scaleCount > 0), {
        KERNEL_LOG(
            KERNEL_ERROR, "scaleCount is %u, which should be equal to offsetCount %u and not zero.", calCount,
            srcTensor.GetSize());
    });
    ASCENDC_ASSERT((calCount % scaleCount == 0 && calCount > 0), {
        KERNEL_LOG(
            KERNEL_ERROR, "calCount is %u, which should be integral multiple of scaleCount %u and not zero.", calCount,
            scaleCount);
    });
}
template <typename T>
__aicore__ inline void IsQuantParamValid(
    const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const LocalTensor<T>& scaleTensor, const T& offset, const uint32_t scaleCount, const uint32_t calCount)
{
    ASCENDC_ASSERT((calCount <= srcTensor.GetSize()), {
        KERNEL_LOG(
            KERNEL_ERROR, "calCount is %u, which should not larger than srcTensor size %u.", calCount,
            srcTensor.GetSize());
    });
    ASCENDC_ASSERT((scaleCount <= scaleTensor.GetSize()), {
        KERNEL_LOG(
            KERNEL_ERROR, "scaleCount is %u, which should not larger than scaleTensor size %u.", scaleCount,
            scaleTensor.GetSize());
    });
    ASCENDC_ASSERT((scaleCount > 0), {
        KERNEL_LOG(KERNEL_ERROR, "scaleCount is %u, which should be not zero.", calCount, srcTensor.GetSize());
    });
    ASCENDC_ASSERT((calCount % scaleCount == 0 && calCount > 0), {
        KERNEL_LOG(
            KERNEL_ERROR, "calCount is %u, which should be integral multiple of scaleCount %u and not zero.", calCount,
            scaleCount);
    });
}
#endif
template <
    typename dstT, typename srcT, typename scaleT, bool isReuseSource, const AscendQuantConfig& config,
    const AscendQuantPolicy& policy>
__aicore__ inline void AscendQuantImpl(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const LocalTensor<scaleT>& offsetTensor, const AscendQuantParam& para)
{
    if ASCEND_IS_AIC {
        return;
    }
    LocalTensor<uint8_t> stackTensor;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(stackTensor);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    AscendQuantImpl<dstT, srcT, scaleT, isReuseSource, config, policy>(
        dstTensor, srcTensor, stackTensor, scaleTensor, offsetTensor, para);
}

template <
    typename dstT, typename srcT, typename scaleT, bool isReuseSource, const AscendQuantConfig& config,
    const AscendQuantPolicy& policy>
__aicore__ inline void AscendQuantImpl(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const scaleT offset, const AscendQuantParam& para)
{
    if ASCEND_IS_AIC {
        return;
    }
    LocalTensor<uint8_t> stackTensor;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(stackTensor);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    AscendQuantImpl<dstT, srcT, scaleT, isReuseSource, config, policy>(
        dstTensor, srcTensor, stackTensor, scaleTensor, offset, para);
}

#else
template <typename T>
__aicore__ inline void IsQuantParamValid(
    const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const LocalTensor<T>& scaleTensor, const LocalTensor<T>& offsetTensor, const uint32_t scaleCount,
    const uint32_t offsetCount, const uint32_t calCount)
{
    ASCENDC_ASSERT((calCount <= srcTensor.GetSize()), {
        KERNEL_LOG(
            KERNEL_ERROR, "calCount is %u, which should not larger than srcTensor size %u.", calCount,
            srcTensor.GetSize());
    });
    ASCENDC_ASSERT((scaleCount <= scaleTensor.GetSize()), {
        KERNEL_LOG(
            KERNEL_ERROR, "scaleCount is %u, which should not larger than scaleTensor size %u.", scaleCount,
            scaleTensor.GetSize());
    });
    ASCENDC_ASSERT((offsetCount <= offsetTensor.GetSize()), {
        KERNEL_LOG(
            KERNEL_ERROR, "offsetCount is %u, which should not larger than offsetTensor size %u.", offsetCount,
            offsetTensor.GetSize());
    });
    ASCENDC_ASSERT((scaleCount == offsetCount && scaleCount > 0), {
        KERNEL_LOG(
            KERNEL_ERROR, "scaleCount is %u, which should be equal to offsetCount %u and not zero.", scaleCount,
            offsetCount);
    });
    ASCENDC_ASSERT((calCount % scaleCount == 0 && calCount > 0), {
        KERNEL_LOG(
            KERNEL_ERROR, "calCount is %u, which should be integral multiple of scaleCount %u and not zero.", calCount,
            scaleCount);
    });
}
template <typename T>
__aicore__ inline void IsQuantParamValid(
    const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const LocalTensor<T>& scaleTensor, const T& offset, const uint32_t scaleCount, const uint32_t calCount)
{
    ASCENDC_ASSERT((calCount <= srcTensor.GetSize()), {
        KERNEL_LOG(
            KERNEL_ERROR, "calCount is %u, which should not larger than srcTensor size %u.", calCount,
            srcTensor.GetSize());
    });
    ASCENDC_ASSERT((scaleCount <= scaleTensor.GetSize()), {
        KERNEL_LOG(
            KERNEL_ERROR, "scaleCount is %u, which should not larger than scaleTensor size %u.", scaleCount,
            scaleTensor.GetSize());
    });
    ASCENDC_ASSERT(
        (scaleCount > 0), { KERNEL_LOG(KERNEL_ERROR, "scaleCount is %u, which should be not zero.", scaleCount); });
    ASCENDC_ASSERT((calCount % scaleCount == 0 && calCount > 0), {
        KERNEL_LOG(
            KERNEL_ERROR, "calCount is %u, which should be integral multiple of scaleCount %u and not zero.", calCount,
            scaleCount);
    });
}
#endif
} // namespace AscendC
#endif // IMPL_QUANTIZATION_QUANT_ASCEND_QUANT_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_QUANT_ASCEND_QUANT_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_QUANT_ASCEND_QUANT_COMMON_IMPL_H__
#endif
