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
 * \file dequantize_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/quantization/dequantize/dequantize_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/quantization/dequantize.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_DEQUANTIZE_DEQUANTIZE_IMPL_H__
#endif

#ifndef IMPL_QUANTIZATION_DEQUANTIZE_IMPL_H
#define IMPL_QUANTIZATION_DEQUANTIZE_IMPL_H

#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../dequant/ascend_dequant_common_impl.h"
#include "../../../../basic_api/kernel_pop_stack_buffer.h"
#include "dequantize_common.h"
#include "../../common/check.h"

#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../dequant/ascend_dequant_3510_impl.h"
#endif

namespace AscendC {
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
// DequantParams check
template <typename DstT>
__aicore__ inline bool IsDequantizeParamsValid(
    const LocalTensor<int32_t>& srcTensor, const LocalTensor<DstT>& dstTensor, DequantParams& params)
{
    // params.n must be 32B aligned as FP32
    ASCENDC_ASSERT(
        params.n % FLOAT_PER_BLOCK == 0, { KERNEL_LOG(KERNEL_ERROR, "params.n %u must be divisible by 8", params.n); });
    // params.m * params.n <= srcTensor element num
    ASCENDC_ASSERT(params.m * params.n <= srcTensor.GetSize(), {
        KERNEL_LOG(
            KERNEL_ERROR, "params.m %u * params.n %u \
        must not be larger than element num of srcTensor %u",
            params.m, params.n, srcTensor.GetSize());
    });
    // params.m * (params.n after align with DstT) <= dstTensor elementNum
    uint32_t oneBlockNum = ONE_BLK_SIZE / sizeof(DstT);
    uint32_t alignInner = (params.n + oneBlockNum - 1) / oneBlockNum * oneBlockNum;
    uint32_t alignedNum = params.m * alignInner;
    ASCENDC_ASSERT((alignedNum <= dstTensor.GetSize()), {
        KERNEL_LOG(KERNEL_ERROR, "dstTensor element num should be not less than %u", alignedNum);
    });

    return true;
}

template <typename DstT, typename ScaleT, DequantizePolicy policy>
__aicore__ inline constexpr bool IsDataTypeValid()
{
    if constexpr (policy == DequantizePolicy::PER_CHANNEL) {
        constexpr bool isValid1 = (IsSameType<ScaleT, uint64_t>::value) && (IsSameType<DstT, half>::value);
        constexpr bool isValid2 = (IsSameType<ScaleT, float>::value) && (IsSameType<DstT, float>::value);
        constexpr bool isValid3 = (IsSameType<ScaleT, bfloat16_t>::value) && (IsSameType<DstT, float>::value);
        constexpr bool isValid4 = (IsSameType<ScaleT, bfloat16_t>::value) && (IsSameType<DstT, bfloat16_t>::value);
        constexpr bool isValid5 = (IsSameType<ScaleT, float>::value) && (IsSameType<DstT, bfloat16_t>::value);
        return isValid1 || isValid2 || isValid3 || isValid4 || isValid5;
    } else if constexpr (policy == DequantizePolicy::PER_TENSOR) {
        constexpr bool isValid1 = (IsSameType<ScaleT, bfloat16_t>::value) && (IsSameType<DstT, bfloat16_t>::value);
        constexpr bool isValid2 = (IsSameType<ScaleT, bfloat16_t>::value) && (IsSameType<DstT, float>::value);
        constexpr bool isValid3 = (IsSameType<ScaleT, float>::value) && (IsSameType<DstT, float>::value);
        constexpr bool isValid4 = (IsSameType<ScaleT, float>::value) && (IsSameType<DstT, bfloat16_t>::value);
        return isValid1 || isValid2 || isValid3 || isValid4;
    } else if constexpr (policy == DequantizePolicy::PER_TOKEN || policy == DequantizePolicy::PER_GROUP) {
        constexpr bool isValid1 = (IsSameType<ScaleT, half>::value) && (IsSameType<DstT, half>::value);
        constexpr bool isValid2 = (IsSameType<ScaleT, bfloat16_t>::value) && (IsSameType<DstT, bfloat16_t>::value);
        constexpr bool isValid3 = (IsSameType<ScaleT, float>::value) && (IsSameType<DstT, float>::value);
        constexpr bool isValid4 = (IsSameType<ScaleT, float>::value) && (IsSameType<DstT, half>::value);
        constexpr bool isValid5 = (IsSameType<ScaleT, float>::value) && (IsSameType<DstT, bfloat16_t>::value);
        return isValid1 || isValid2 || isValid3 || isValid4 || isValid5;
    }
    return false;
}

template <const DequantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
__aicore__ inline void DequantizePerTensorImpl(
    const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const DequantizeParams& params)
{
    if ASCEND_IS_AIC {
        return;
    }
    static_assert(!config.hasOffset, "Dequantize do not support offset currently");
    static_assert(TypeUtils::IsInnerDefaultType<ScaleT, OffsetT>());
    static_assert(IsSameType<SrcT, int32_t>::value, "Dtype of srcTensor should be int32_t for PER_TENSOR");
    static_assert(
        IsDataTypeValid<DstT, ScaleT, config.policy>(),
        "current combination of scale dtype and dstTensor dtype is not supported, please check the document");
    ASCENDC_ASSERT((params.n * sizeof(SrcT) % 32 == 0) && (params.n * sizeof(DstT) % 32 == 0), {
        KERNEL_LOG(KERNEL_ERROR, "params.n %u must be 32B aligned", params.n);
    });
    DequantParams deqParams{params.m, params.n, params.n};
    if (!IsDequantizeParamsValid<DstT>(srcTensor, dstTensor, deqParams)) {
        return;
    }
    DequantPertensorImpl<DstT, ScaleT, DeQuantMode::DEQUANT_WITH_MULTI_ROW>(dstTensor, srcTensor, scale, deqParams);
}

template <const DequantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
__aicore__ inline void DequantizePerChannelImpl(
    const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const DequantizeParams& params)
{
    static_assert(TypeUtils::IsLocalTensorType<ScaleT, OffsetT>());
    static_assert(IsSameType<SrcT, int32_t>::value, "Dtype of srcTensor should be int32_t for PER_CHANNEL");
    static_assert(!config.hasOffset, "Dequantize do not support offset currently");
    static_assert(
        IsDataTypeValid<DstT, typename ScaleT::PrimType, config.policy>(),
        "current combination of scale dtype and dstTensor dtype is not supported, please check the document");
    ASCENDC_ASSERT((params.n * sizeof(SrcT) % 32 == 0) && (params.n * sizeof(DstT) % 32 == 0), {
        KERNEL_LOG(KERNEL_ERROR, "params.n %u must be 32B aligned", params.n);
    });
    DequantParams deqParams{params.m, params.n, params.n};
    if (!IsDequantizeParamsValid<DstT>(srcTensor, dstTensor, deqParams)) {
        return;
    }
    DequantPerchannelImpl<DstT, typename ScaleT::PrimType, DeQuantMode::DEQUANT_WITH_MULTI_ROW>(
        dstTensor, srcTensor, scale, deqParams);
}

template <const DequantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
__aicore__ inline void DequantizePerTokenImpl(
    const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const DequantizeParams& params)
{
    if ASCEND_IS_AIC {
        return;
    }
    static_assert(!config.hasOffset, "Dequantize do not support offset currently");
    static_assert(TypeUtils::IsLocalTensorType<ScaleT, OffsetT>());
    static_assert(SupportType<SrcT, int32_t, float>(), "Dtype of srcTensor should be int32_t or float for PER_TOKEN");
    static_assert(
        IsDataTypeValid<DstT, typename ScaleT::PrimType, config.policy>(),
        "current combination of scale dtype and dstTensor dtype is not supported, please check the document");
    ASCENDC_ASSERT((params.n * sizeof(SrcT) % 32 == 0) && (params.n * sizeof(DstT) % 32 == 0), {
        KERNEL_LOG(KERNEL_ERROR, "params.n %u must be 32B aligned", params.n);
    });

    static constexpr AscendDeQuantConfig deqConfig = {false, config.kDim};
    AscendDeQuantParam deqParams = {params.m, params.n, params.m * params.n, params.groupSize};
    AscendDeQuantPerToken<DstT, SrcT, typename ScaleT::PrimType, deqConfig>(
        dstTensor, srcTensor, sharedTmpBuffer, scale, scale, deqParams);
}

template <const DequantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
__aicore__ inline void DequantizePerGroupImpl(
    const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const DequantizeParams& params)
{
    if ASCEND_IS_AIC {
        return;
    }
    static_assert(!config.hasOffset, "Dequantize do not support offset currently");
    static_assert(TypeUtils::IsLocalTensorType<ScaleT, OffsetT>());
    static_assert(SupportType<SrcT, int32_t, float>(), "Dtype of srcTensor should be int32_t or float for PER_GROUP");
    static_assert(
        IsDataTypeValid<DstT, typename ScaleT::PrimType, config.policy>(),
        "current combination of scale dtype and dstTensor dtype is not supported, please check the document");
    static_assert(((config.kDim == 1) || (config.kDim == 0)), "Dequantize PER_GROUP only support kDim is axis 0/1!");
    ASCENDC_ASSERT((params.groupSize > 0 && params.groupSize % 32 == 0), {
        KERNEL_LOG(KERNEL_ERROR, "groupSize must be an integer multiple of 32 and greater than 0 !");
    });
    ASCENDC_ASSERT((params.n * sizeof(SrcT) % 32 == 0) && (params.n * sizeof(DstT) % 32 == 0), {
        KERNEL_LOG(KERNEL_ERROR, "params.n %u must be 32B aligned", params.n);
    });
    static constexpr AscendDeQuantConfig deqConfig = {false, config.kDim};
    AscendDeQuantParam deqParams = {params.m, params.n, params.m * params.n, params.groupSize};

    if constexpr (config.kDim == 1) {
        AscendDeQuantPerGroupForCol<DstT, SrcT, typename ScaleT::PrimType, deqConfig>(
            dstTensor, srcTensor, sharedTmpBuffer, scale, offset, deqParams);
    } else {
        AscendDeQuantPerGroupForRow<DstT, SrcT, typename ScaleT::PrimType, deqConfig>(
            dstTensor, srcTensor, sharedTmpBuffer, scale, offset, deqParams);
    }
}

template <const DequantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
__aicore__ inline void DequantizePerTensorImpl(
    const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset,
    const DequantizeParams& params)
{
    LocalTensor<uint8_t> stackTensor;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(stackTensor);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    DequantizePerTensorImpl<config>(dstTensor, srcTensor, scale, offset, stackTensor, params);
}

template <const DequantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
__aicore__ inline void DequantizePerChannelImpl(
    const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset,
    const DequantizeParams& params)
{
    LocalTensor<uint8_t> stackTensor;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(stackTensor);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    DequantizePerChannelImpl<config>(dstTensor, srcTensor, scale, offset, stackTensor, params);
}

template <const DequantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
__aicore__ inline void DequantizePerTokenImpl(
    const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset,
    const DequantizeParams& params)
{
    LocalTensor<uint8_t> stackTensor;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(stackTensor);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    DequantizePerTokenImpl<config>(dstTensor, srcTensor, scale, offset, stackTensor, params);
}

template <const DequantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
__aicore__ inline void DequantizePerGroupImpl(
    const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset,
    const DequantizeParams& params)
{
    LocalTensor<uint8_t> stackTensor;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(stackTensor);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    DequantizePerGroupImpl<config>(dstTensor, srcTensor, scale, offset, stackTensor, params);
}

template <typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
__aicore__ inline void CheckDequantizeTensorPos(
    const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset)
{
    CheckTensorPosition(dstTensor, "dstTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcTensor, "srcTensor", "VECIN/VECOUT/VECCALC");
    if constexpr (TypeUtils::IsLocalTensorType<ScaleT, OffsetT>()) {
        CheckTensorPosition(scale, "scale", "VECIN/VECOUT/VECCALC");
        CheckTensorPosition(offset, "offset", "VECIN/VECOUT/VECCALC");
    }
}

template <const DequantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
__aicore__ inline void DequantizeImpl(
    const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const DequantizeParams& params)
{
    CheckDequantizeTensorPos(dstTensor, srcTensor, scale, offset);
    CheckTensorPosition(sharedTmpBuffer, "sharedTmpBuffer", "VECIN/VECOUT/VECCALC");
    if constexpr (config.policy == DequantizePolicy::PER_TENSOR) {
        DequantizePerTensorImpl<config>(dstTensor, srcTensor, scale, offset, sharedTmpBuffer, params);
    } else if constexpr (config.policy == DequantizePolicy::PER_CHANNEL) {
        DequantizePerChannelImpl<config>(dstTensor, srcTensor, scale, offset, sharedTmpBuffer, params);
    } else if constexpr (config.policy == DequantizePolicy::PER_TOKEN) {
        DequantizePerTokenImpl<config>(dstTensor, srcTensor, scale, offset, sharedTmpBuffer, params);
    } else if constexpr (config.policy == DequantizePolicy::PER_GROUP) {
        DequantizePerGroupImpl<config>(dstTensor, srcTensor, scale, offset, sharedTmpBuffer, params);
    } else {
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "Unsupported Dequantize policy on current device"); });
    }
}

template <const DequantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
__aicore__ inline void DequantizeImpl(
    const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset,
    const DequantizeParams& params)
{
    CheckDequantizeTensorPos(dstTensor, srcTensor, scale, offset);
    if constexpr (config.policy == DequantizePolicy::PER_TENSOR) {
        DequantizePerTensorImpl<config>(dstTensor, srcTensor, scale, offset, params);
    } else if constexpr (config.policy == DequantizePolicy::PER_CHANNEL) {
        DequantizePerChannelImpl<config>(dstTensor, srcTensor, scale, offset, params);
    } else if constexpr (config.policy == DequantizePolicy::PER_TOKEN) {
        DequantizePerTokenImpl<config>(dstTensor, srcTensor, scale, offset, params);
    } else if constexpr (config.policy == DequantizePolicy::PER_GROUP) {
        DequantizePerGroupImpl<config>(dstTensor, srcTensor, scale, offset, params);
    } else {
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "Unsupported Dequantize policy on current device"); });
    }
}
#endif
} // namespace AscendC
#endif // IMPL_QUANTIZATION_DEQUANTIZE_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_DEQUANTIZE_DEQUANTIZE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_DEQUANTIZE_DEQUANTIZE_IMPL_H__
#endif
