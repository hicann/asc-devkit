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
 * \file antiquantize_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/quantization/antiquantize/antiquantize_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/quantization/antiquantize.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_ANTIQUANTIZE_ANTIQUANTIZE_IMPL_H__
#endif

#ifndef IMPL_QUANTIZATION_ANTIQUANTIZE_IMPL_H
#define IMPL_QUANTIZATION_ANTIQUANTIZE_IMPL_H

#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../basic_api/kernel_pop_stack_buffer.h"
#include "antiquantize_common.h"
#include "../../common/check.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102)
#include "../antiquant/ascend_antiquant_3510_impl.h"
#include "antiquantize_3510_impl.h"
#endif

namespace AscendC {
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102)
template <typename DstT, typename SrcT, typename ScaleT, AntiQuantizePolicy policy>
__aicore__ inline constexpr bool IsDataTypeValid()
{
    if constexpr (policy == AntiQuantizePolicy::PER_TENSOR || policy == AntiQuantizePolicy::PER_CHANNEL) {
        constexpr bool isValid1 = (IsSameType<ScaleT, bfloat16_t>::value) && (IsSameType<DstT, bfloat16_t>::value);
        constexpr bool isValid2 = (IsSameType<ScaleT, half>::value) && (IsSameType<DstT, half>::value);
        return isValid1 || isValid2;
    } else if constexpr (policy == AntiQuantizePolicy::PER_TOKEN) {
        constexpr bool isValid1 = (IsSameType<ScaleT, half>::value) && (IsSameType<DstT, half>::value);
        constexpr bool isValid2 = (IsSameType<ScaleT, bfloat16_t>::value) && (IsSameType<DstT, bfloat16_t>::value);
        constexpr bool isValid3 = (IsSameType<ScaleT, float>::value) && (IsSameType<DstT, float>::value);
        constexpr bool isValid4 = (IsSameType<ScaleT, float>::value) && (IsSameType<DstT, half>::value);
        constexpr bool isValid5 = (IsSameType<ScaleT, float>::value) && (IsSameType<DstT, bfloat16_t>::value);
        return isValid1 || isValid2 || isValid3 || isValid4 || isValid5;
    } else if constexpr (policy == AntiQuantizePolicy::PER_GROUP) {
        if constexpr (SupportType<SrcT, fp4x2_e1m2_t, fp4x2_e2m1_t>()) {
            constexpr bool isValid1 = (IsSameType<ScaleT, fp8_e8m0_t>::value) && (IsSameType<DstT, half>::value);
            constexpr bool isValid2 = (IsSameType<ScaleT, fp8_e8m0_t>::value) && (IsSameType<DstT, bfloat16_t>::value);
            return isValid1 || isValid2;
        }
        constexpr bool isValid3 = (IsSameType<ScaleT, half>::value) && (IsSameType<DstT, half>::value);
        constexpr bool isValid4 = (IsSameType<ScaleT, bfloat16_t>::value) && (IsSameType<DstT, bfloat16_t>::value);
        constexpr bool isValid5 = (IsSameType<ScaleT, float>::value) && (IsSameType<DstT, float>::value);
        constexpr bool isValid6 = (IsSameType<ScaleT, float>::value) && (IsSameType<DstT, half>::value);
        constexpr bool isValid7 = (IsSameType<ScaleT, float>::value) && (IsSameType<DstT, bfloat16_t>::value);
        return isValid3 || isValid4 || isValid5 || isValid6 || isValid7;
    }
    return false;
}

template <const AntiQuantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
__aicore__ inline void AntiQuantizePerTensorImpl(
    const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const AntiQuantizeParams& params)
{
    if ASCEND_IS_AIC {
        return;
    }
    static_assert(
        SupportType<SrcT, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t, int8_t>(),
        "This AntiQuantize PER_TENSOR only support fp8_e4m3fn_t/fp8_e5m2_t/hifloat8_t/int8_t input dtype");
    static_assert(TypeUtils::IsInnerDefaultType<ScaleT, OffsetT>());
    static_assert(IsSameType<ScaleT, OffsetT>::value, "Dtype of scale should be same as offset");
    static_assert(
        IsDataTypeValid<DstT, SrcT, ScaleT, config.policy>(),
        "current combination of scale dtype and dstTensor dtype is not supported, please check the document");
    static_assert(IsSameType<DstT, ScaleT>::value && IsSameType<DstT, OffsetT>::value);
    ASCENDC_ASSERT(
        params.n * sizeof(SrcT) % 32 == 0, { KERNEL_LOG(KERNEL_ERROR, "params.n %u must be 32B aligned", params.n); });
    AntiQuantizePerTensor<config>(dstTensor, srcTensor, offset, scale, sharedTmpBuffer, params);
}

template <const AntiQuantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
__aicore__ inline void AntiQuantizePerChannelImpl(
    const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const AntiQuantizeParams& params)
{
    if ASCEND_IS_AIC {
        return;
    }
    static_assert(TypeUtils::IsLocalTensorType<ScaleT, OffsetT>());
    static_assert(
        SupportType<SrcT, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t, int8_t>(),
        "This AntiQuantize PER_CHANNEL only support fp8_e4m3fn_t/fp8_e5m2_t/hifloat8_t/int8_t input dtype");
    static_assert(IsSameType<ScaleT, OffsetT>::value, "Dtype of scale should be same as offset");
    static_assert(
        IsDataTypeValid<DstT, SrcT, typename ScaleT::PrimType, config.policy>(),
        "current combination of scale dtype and dstTensor dtype is not supported, please check the document");
    ASCENDC_ASSERT(
        params.n * sizeof(SrcT) % 32 == 0, { KERNEL_LOG(KERNEL_ERROR, "params.n %u must be 32B aligned", params.n); });
    AntiQuantizePerChannel<config>(dstTensor, srcTensor, scale, offset, sharedTmpBuffer, params);
}

template <const AntiQuantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
__aicore__ inline void AntiQuantizePerTokenImpl(
    const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const AntiQuantizeParams& params)
{
    if ASCEND_IS_AIC {
        return;
    }
    static_assert(TypeUtils::IsLocalTensorType<ScaleT, OffsetT>());
    static_assert(
        SupportType<SrcT, int8_t, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t>(),
        "AntiQuantize PerToken only support int8_t/fp8_e4m3fn_t/fp8_e5m2_t/hifloat8_t input dtype");
    static_assert(IsSameType<ScaleT, OffsetT>::value, "Dtype of scale should be same as offset");
    static_assert(
        IsDataTypeValid<DstT, SrcT, typename ScaleT::PrimType, config.policy>(),
        "current combination of scale dtype and dstTensor dtype is not supported, please check the document");
    ASCENDC_ASSERT(
        params.n * sizeof(SrcT) % 32 == 0, { KERNEL_LOG(KERNEL_ERROR, "params.n %u must be 32B aligned", params.n); });
    static constexpr AscendAntiQuantConfig antiConfig = {config.hasOffset, false, config.kDim};
    AscendAntiQuantParam antiParams = {params.m, params.n, params.m * params.n, params.groupSize};
    AntiQuantizePerTokenCommon<DstT, SrcT, typename ScaleT::PrimType, antiConfig>(
        dstTensor, srcTensor, scale, offset, antiParams);
}

template <const AntiQuantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
__aicore__ inline void AntiQuantizePerGroupImpl(
    const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const AntiQuantizeParams& params)
{
    if ASCEND_IS_AIC {
        return;
    }
    static_assert(TypeUtils::IsLocalTensorType<ScaleT, OffsetT>());
    static_assert(IsSameType<ScaleT, OffsetT>::value, "Dtype of scale should be same as offset");
    static_assert(
        SupportType<SrcT, int8_t, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t, fp4x2_e1m2_t, fp4x2_e2m1_t>(),
        "AntiQuantize PerGroup only support "
        "int8_t/fp8_e4m3fn_t/fp8_e5m2_t/hifloat8_t/fp4x2_e1m2_t/fp4x2_e2m1_t input dtype");

    static_assert(
        IsDataTypeValid<DstT, SrcT, typename ScaleT::PrimType, config.policy>(),
        "current combination of scale dtype and dstTensor dtype is not supported, please check the document");
    static_assert(((config.kDim == 1) || (config.kDim == 0)), "AntiQuantize PerGroup only support K is axis 0/1!");
    ASCENDC_ASSERT((params.groupSize > 0 && params.groupSize % 32 == 0), {
        KERNEL_LOG(KERNEL_ERROR, "groupSize must be an integer multiple of 32 and greater than 0 !");
    });

    if constexpr (IsSameType<SrcT, fp4x2_e1m2_t>::value || IsSameType<SrcT, fp4x2_e2m1_t>::value) {
        static_assert(!config.hasOffset, "AntiQuantize do not support offset when SrcT is fp4x2_e1m2_t / fp4x2_e2m1_t");
        ASCENDC_ASSERT(params.n % 64 == 0, { KERNEL_LOG(KERNEL_ERROR, "params.n %u must be 32B aligned", params.n); });
    } else {
        ASCENDC_ASSERT(params.n * sizeof(SrcT) % 32 == 0, {
            KERNEL_LOG(KERNEL_ERROR, "params.n %u must be 32B aligned", params.n);
        });
    }
    static constexpr AscendAntiQuantConfig antiConfig = {config.hasOffset, false, config.kDim};
    AscendAntiQuantParam antiParams = {params.m, params.n, params.m * params.n, params.groupSize};

    if constexpr (config.kDim == 1) {
        if constexpr (SupportType<SrcT, fp4x2_e1m2_t, fp4x2_e2m1_t>()) {
            AntiQuantPerGroupForColFp4<DstT, SrcT, typename ScaleT::PrimType, antiConfig>(
                dstTensor, srcTensor, scale, antiParams);
        } else {
            AntiQuantizePerGroupForColCommon<DstT, SrcT, typename ScaleT::PrimType, antiConfig>(
                dstTensor, srcTensor, scale, offset, antiParams);
        }
    } else {
        if constexpr (SupportType<SrcT, fp4x2_e1m2_t, fp4x2_e2m1_t>()) {
            AntiQuantPerGroupForRowFp4<DstT, SrcT, typename ScaleT::PrimType, antiConfig>(
                dstTensor, srcTensor, scale, antiParams);
        } else {
            AntiQuantizePerGroupForRowCommon<DstT, SrcT, typename ScaleT::PrimType, antiConfig>(
                dstTensor, srcTensor, scale, offset, antiParams);
        }
    }
}

template <const AntiQuantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
__aicore__ inline void AntiQuantizePerTensorImpl(
    const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset,
    const AntiQuantizeParams& params)
{
    LocalTensor<uint8_t> stackTensor;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(stackTensor);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    AntiQuantizePerTensorImpl<config>(dstTensor, srcTensor, scale, offset, stackTensor, params);
}

template <const AntiQuantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
__aicore__ inline void AntiQuantizePerChannelImpl(
    const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset,
    const AntiQuantizeParams& params)
{
    LocalTensor<uint8_t> stackTensor;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(stackTensor);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    AntiQuantizePerChannelImpl<config>(dstTensor, srcTensor, scale, offset, stackTensor, params);
}

template <const AntiQuantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
__aicore__ inline void AntiQuantizePerTokenImpl(
    const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset,
    const AntiQuantizeParams& params)
{
    LocalTensor<uint8_t> stackTensor;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(stackTensor);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    AntiQuantizePerTokenImpl<config>(dstTensor, srcTensor, scale, offset, stackTensor, params);
}

template <const AntiQuantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
__aicore__ inline void AntiQuantizePerGroupImpl(
    const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset,
    const AntiQuantizeParams& params)
{
    LocalTensor<uint8_t> stackTensor;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(stackTensor);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    AntiQuantizePerGroupImpl<config>(dstTensor, srcTensor, scale, offset, stackTensor, params);
}

template <typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
__aicore__ inline void CheckAntiQuantizeTensorPos(
    const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset)
{
    CheckTensorPosition(dstTensor, "dstTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcTensor, "srcTensor", "VECIN/VECOUT/VECCALC");
    if constexpr (TypeUtils::IsLocalTensorType<ScaleT, OffsetT>()) {
        CheckTensorPosition(scale, "scale", "VECIN/VECOUT/VECCALC");
        CheckTensorPosition(offset, "offset", "VECIN/VECOUT/VECCALC");
    }
}

template <const AntiQuantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
__aicore__ inline void AntiQuantizeImpl(
    const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const AntiQuantizeParams& params)
{
    CheckAntiQuantizeTensorPos(dstTensor, srcTensor, scale, offset);
    CheckTensorPosition(sharedTmpBuffer, "sharedTmpBuffer", "VECIN/VECOUT/VECCALC");
    if constexpr (config.policy == AntiQuantizePolicy::PER_TENSOR) {
        AntiQuantizePerTensorImpl<config>(dstTensor, srcTensor, scale, offset, sharedTmpBuffer, params);
    } else if constexpr (config.policy == AntiQuantizePolicy::PER_CHANNEL) {
        AntiQuantizePerChannelImpl<config>(dstTensor, srcTensor, scale, offset, sharedTmpBuffer, params);
    } else if constexpr (config.policy == AntiQuantizePolicy::PER_TOKEN) {
        AntiQuantizePerTokenImpl<config>(dstTensor, srcTensor, scale, offset, sharedTmpBuffer, params);
    } else if constexpr (config.policy == AntiQuantizePolicy::PER_GROUP) {
        AntiQuantizePerGroupImpl<config>(dstTensor, srcTensor, scale, offset, sharedTmpBuffer, params);
    } else {
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "Unsupported AntiQuantize policy on current device"); });
    }
}

template <const AntiQuantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
__aicore__ inline void AntiQuantizeImpl(
    const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset,
    const AntiQuantizeParams& params)
{
    CheckAntiQuantizeTensorPos(dstTensor, srcTensor, scale, offset);

    if constexpr (config.policy == AntiQuantizePolicy::PER_TENSOR) {
        AntiQuantizePerTensorImpl<config>(dstTensor, srcTensor, scale, offset, params);
    } else if constexpr (config.policy == AntiQuantizePolicy::PER_CHANNEL) {
        AntiQuantizePerChannelImpl<config>(dstTensor, srcTensor, scale, offset, params);
    } else if constexpr (config.policy == AntiQuantizePolicy::PER_TOKEN) {
        AntiQuantizePerTokenImpl<config>(dstTensor, srcTensor, scale, offset, params);
    } else if constexpr (config.policy == AntiQuantizePolicy::PER_GROUP) {
        AntiQuantizePerGroupImpl<config>(dstTensor, srcTensor, scale, offset, params);
    } else {
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "Unsupported AntiQuantize policy on current device"); });
    }
}
#endif
} // namespace AscendC
#endif // IMPL_QUANTIZATION_ANTIQUANTIZE_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_ANTIQUANTIZE_ANTIQUANTIZE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_ANTIQUANTIZE_ANTIQUANTIZE_IMPL_H__
#endif
