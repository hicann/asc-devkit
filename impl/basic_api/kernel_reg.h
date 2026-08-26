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
 * \file kernel_reg.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_reg.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_common.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_H__
#endif
#ifndef ASCENDC_KERNEL_REG_IMPL_H
#define ASCENDC_KERNEL_REG_IMPL_H

#include "kernel_utils.h"
#include "../../include/basic_api/kernel_struct_aipp.h"

namespace AscendC {
constexpr uint64_t MASK_PLACEHOLDER = 0;
constexpr uint64_t MASK_PLACEHOLDER_LIST[2] = {0, 0};

enum class MaskMode : uint8_t { NORMAL = 0, COUNTER };

template <typename T, MaskMode mode>
__aicore__ static inline void SetVectorMaskImpl(const uint64_t maskHigh, const uint64_t maskLow)
{
    if ASCEND_IS_NOT_AIC {
        set_vector_mask(maskHigh, maskLow);
    }
}

template <typename T, MaskMode mode>
__aicore__ static inline void SetVectorMaskImpl(int32_t len)
{
    if constexpr (mode == MaskMode::COUNTER) {
        SetVectorMaskImpl<PrimT<T>, mode>(0, len);
        return;
    }
    AscendCUtils::SetMask<PrimT<T>>(len);
}

__aicore__ inline void ResetMaskImpl()
{
    if ASCEND_IS_NOT_AIC {
        set_vector_mask(FULL_MASK, FULL_MASK);
    }
}

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
template <pipe_t pipe>
__aicore__ inline void PipeBarrierImpl()
{
#if (__NPU_ARCH__ == 5102)
    if constexpr (pipe == PIPE_MTE3) {
        return;
    }
#else
    static_assert(pipe != PIPE_S, "PipeBarrier<PIPE_S> is not supported on current device!");
    if ASCEND_IS_AIC {
        if constexpr (pipe == PIPE_MTE3) {
            return;
        }
    }
#endif
    if constexpr (pipe != PIPE_V) {
        pipe_barrier(pipe);
    }
    return;
}
#else
template <pipe_t pipe>
__aicore__ inline void PipeBarrierImpl()
{
#if __NPU_ARCH__ == 3102
    return;
#endif
#if (__NPU_ARCH__ == 3002)
    if constexpr (pipe == PIPE_S || pipe == PIPE_V) {
        return;
    }
#endif
#if (                                                                                               \
    __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || \
    __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    if constexpr (pipe == PIPE_V) {
        return;
    }
#endif
#if (__NPU_ARCH__ == 2201)
    ASCENDC_DEBUG_ASSERT(
        pipe != PIPE_S, KERNEL_LOG_INTERNAL(KERNEL_ERROR, "PipeBarrier<PIPE_S> is not supported on current device!"));
    if ASCEND_IS_AIC {
        if constexpr (pipe == PIPE_V) {
            return;
        }
    }
#endif
    pipe_barrier(pipe);
}
#endif

enum class CacheLine : uint64_t { SINGLE_CACHE_LINE = 0, ENTIRE_DATA_CACHE };

enum class DcciDst : uint64_t { CACHELINE_ALL = 0, CACHELINE_UB, CACHELINE_OUT, CACHELINE_ATOMIC };

#if defined(__NPU_ARCH__) &&                                                                                 \
    ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || \
     (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || (__NPU_ARCH__ == 5101) || (__NPU_ARCH__ == 5161) || \
     (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
template <typename T, CacheLine entireType, DcciDst dcciDst>
__aicore__ inline void DcciGMImpl(__gm__ T* dst)
{
    dcci(static_cast<__gm__ void*>(dst), static_cast<uint64_t>(entireType), static_cast<uint64_t>(dcciDst));
}

template <typename T, CacheLine entireType, DcciDst dcciDst>
__aicore__ inline void DcciUBImpl(__ubuf__ T* dst)
{
    dcci(static_cast<__ubuf__ void*>(dst), static_cast<uint64_t>(entireType), static_cast<uint64_t>(dcciDst));
}
#endif

#if defined(__NPU_ARCH__) &&                                                                                 \
    ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 2002) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3510) || \
     (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || (__NPU_ARCH__ == 5101) || \
     (__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
template <typename T, CacheLine entireType>
__aicore__ inline void DcciGMImpl(__gm__ T* dst)
{
    dcci(static_cast<__gm__ void*>(dst), static_cast<uint64_t>(entireType));
}
#endif

__aicore__ inline void SetMaskCountImpl()
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3113)
    constexpr uint32_t CTRL_COUNTER = 56;
    set_ctrl(sbitset1(get_ctrl(), CTRL_COUNTER));
#else
    set_mask_count();
#endif
}

__aicore__ inline void SetMaskNormImpl()
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3113)
    constexpr uint32_t CTRL_COUNTER = 56;
    set_ctrl(sbitset0(get_ctrl(), CTRL_COUNTER));
#else
    set_mask_norm();
#endif
}

__aicore__ inline void SetLreluMode(bool lreluMode)
{
    if (lreluMode) {
        set_ctrl(sbitset1(get_ctrl(), LEAKY_RELU_MODE_BIT));
    } else {
        set_ctrl(sbitset0(get_ctrl(), LEAKY_RELU_MODE_BIT));
    }
}

__aicore__ inline void SetHF32ModeImpl(bool hf32Mode)
{
    if (hf32Mode) {
        set_ctrl(sbitset1(get_ctrl(), HF32_MODE_BIT));
    } else {
        set_ctrl(sbitset0(get_ctrl(), HF32_MODE_BIT));
    }
}

__aicore__ inline void SetHF32TransModeImpl(bool hf32TransMode)
{
    if (hf32TransMode) {
        set_ctrl(sbitset1(get_ctrl(), HF32_TRANS_MODE_BIT));
    } else {
        set_ctrl(sbitset0(get_ctrl(), HF32_TRANS_MODE_BIT));
    }
}

__aicore__ inline void SetMMLayoutTransformImpl(bool mmLayoutMode)
{
    if (mmLayoutMode) {
        set_ctrl(sbitset1(get_ctrl(), MM_LAYOUT_MODE_BIT));
    } else {
        set_ctrl(sbitset0(get_ctrl(), MM_LAYOUT_MODE_BIT));
    }
}

template <bool castMode>
__aicore__ inline void SetCastOverflowModeImpl()
{
    if constexpr (castMode) {
        set_ctrl(sbitset1(get_ctrl(), CAST_MODE_BIT));
    } else {
        set_ctrl(sbitset0(get_ctrl(), CAST_MODE_BIT));
    }
}

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 2002) || (__NPU_ARCH__ == 3002) || \
                              (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
template <typename T>
__aicore__ inline void SetAippFunctionsImpl0(__gm__ T* src0)
{
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    Internal::g_aippSrc0 = reinterpret_cast<uint64_t>(src0) & 0xffffffffffff;
#else
    uint64_t aippConfig0 = reinterpret_cast<uint64_t>(src0) & 0xffffffffffff;

    set_aipp_spr_0(aippConfig0);
#endif
}

template <typename T, typename U>
__aicore__ inline void SetAippFunctionsImpl1(__gm__ T* src1, AippParams<U>& config)
{
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    Internal::g_aippSrc1 = reinterpret_cast<uint64_t>(src1) & 0xffffffffffff;
    if (config.cscParams.isEnableCsc) {
        Internal::g_aippSrc1 |= static_cast<uint64_t>(1) << AIPP_OFFSET_CSC_ENABLE;
    }
#else
    uint64_t aippConfig1 = reinterpret_cast<uint64_t>(src1) & 0xffffffffffff;

    if (config.cscParams.isEnableCsc) {
        aippConfig1 |= static_cast<uint64_t>(1) << AIPP_OFFSET_CSC_ENABLE;
    }

    set_aipp_spr_1(aippConfig1);
#endif
}

template <typename T>
__aicore__ inline void SetAippFunctionsImpl2(AippParams<T>& config)
{
    uint16_t cscMatrixR0C0 = GetScalarBitcodeValue(config.cscParams.cscMatrixR0C0);
    uint16_t cscMatrixR0C1 = GetScalarBitcodeValue(config.cscParams.cscMatrixR0C1);
    uint16_t cscMatrixR0C2 = GetScalarBitcodeValue(config.cscParams.cscMatrixR0C2);
    uint16_t cscMatrixR1C0 = GetScalarBitcodeValue(config.cscParams.cscMatrixR1C0);

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    Internal::g_aippCscRc0 = static_cast<uint64_t>(cscMatrixR0C0);
    Internal::g_aippCscRc0 |= static_cast<uint64_t>(cscMatrixR0C1) << AIPP_OFFSET_CH1;
    Internal::g_aippCscRc0 |= static_cast<uint64_t>(cscMatrixR0C2) << AIPP_OFFSET_CH2;
    Internal::g_aippCscRc0 |= static_cast<uint64_t>(cscMatrixR1C0) << AIPP_OFFSET_CH3;
#else
    uint64_t aippConfig2 = static_cast<uint64_t>(cscMatrixR0C0);
    aippConfig2 |= static_cast<uint64_t>(cscMatrixR0C1) << AIPP_OFFSET_CH1;
    aippConfig2 |= static_cast<uint64_t>(cscMatrixR0C2) << AIPP_OFFSET_CH2;
    aippConfig2 |= static_cast<uint64_t>(cscMatrixR1C0) << AIPP_OFFSET_CH3;

    set_aipp_spr_2(aippConfig2);
#endif
}

template <typename T>
__aicore__ inline void SetAippFunctionsImpl3(AippParams<T>& config)
{
    uint16_t cscMatrixR1C1 = GetScalarBitcodeValue(config.cscParams.cscMatrixR1C1);
    uint16_t cscMatrixR1C2 = GetScalarBitcodeValue(config.cscParams.cscMatrixR1C2);
    uint16_t cscMatrixR2C0 = GetScalarBitcodeValue(config.cscParams.cscMatrixR2C0);
    uint16_t cscMatrixR2C1 = GetScalarBitcodeValue(config.cscParams.cscMatrixR2C1);

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    Internal::g_aippCscRc1 = static_cast<uint64_t>(cscMatrixR1C1);
    Internal::g_aippCscRc1 |= static_cast<uint64_t>(cscMatrixR1C2) << AIPP_OFFSET_CH1;
    Internal::g_aippCscRc1 |= static_cast<uint64_t>(cscMatrixR2C0) << AIPP_OFFSET_CH2;
    Internal::g_aippCscRc1 |= static_cast<uint64_t>(cscMatrixR2C1) << AIPP_OFFSET_CH3;
#else
    uint64_t aippConfig3 = static_cast<uint64_t>(cscMatrixR1C1);
    aippConfig3 |= static_cast<uint64_t>(cscMatrixR1C2) << AIPP_OFFSET_CH1;
    aippConfig3 |= static_cast<uint64_t>(cscMatrixR2C0) << AIPP_OFFSET_CH2;
    aippConfig3 |= static_cast<uint64_t>(cscMatrixR2C1) << AIPP_OFFSET_CH3;

    set_aipp_spr_3(aippConfig3);
#endif
}

template <typename T>
__aicore__ inline void SetAippFunctionsImpl4(AippParams<T>& config)
{
    uint16_t cscMatrixR2C2 = GetScalarBitcodeValue(config.cscParams.cscMatrixR2C2);
    uint8_t cscBiasOut0 = GetScalarBitcodeValue(config.cscParams.cscBiasOut0);
    uint8_t cscBiasOut1 = GetScalarBitcodeValue(config.cscParams.cscBiasOut1);
    uint8_t cscBiasOut2 = GetScalarBitcodeValue(config.cscParams.cscBiasOut2);
    uint8_t cscBiasIn0 = GetScalarBitcodeValue(config.cscParams.cscBiasIn0);
    uint8_t cscBiasIn1 = GetScalarBitcodeValue(config.cscParams.cscBiasIn1);
    uint8_t cscBiasIn2 = GetScalarBitcodeValue(config.cscParams.cscBiasIn2);

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    Internal::g_aippCscBias = static_cast<uint64_t>(cscMatrixR2C2);
    Internal::g_aippCscBias |= static_cast<uint64_t>(cscBiasOut0) << AIPP_OFFSET_CSC_OUT_CH0;
    Internal::g_aippCscBias |= static_cast<uint64_t>(cscBiasOut1) << AIPP_OFFSET_CSC_OUT_CH1;
    Internal::g_aippCscBias |= static_cast<uint64_t>(cscBiasOut2) << AIPP_OFFSET_CSC_OUT_CH2;
    Internal::g_aippCscBias |= static_cast<uint64_t>(cscBiasIn0) << AIPP_OFFSET_CSC_IN_CH0;
    Internal::g_aippCscBias |= static_cast<uint64_t>(cscBiasIn1) << AIPP_OFFSET_CSC_IN_CH1;
    Internal::g_aippCscBias |= static_cast<uint64_t>(cscBiasIn2) << AIPP_OFFSET_CSC_IN_CH2;
#else
    uint64_t aippConfig4 = static_cast<uint64_t>(cscMatrixR2C2);
    aippConfig4 |= static_cast<uint64_t>(cscBiasOut0) << AIPP_OFFSET_CSC_OUT_CH0;
    aippConfig4 |= static_cast<uint64_t>(cscBiasOut1) << AIPP_OFFSET_CSC_OUT_CH1;
    aippConfig4 |= static_cast<uint64_t>(cscBiasOut2) << AIPP_OFFSET_CSC_OUT_CH2;
    aippConfig4 |= static_cast<uint64_t>(cscBiasIn0) << AIPP_OFFSET_CSC_IN_CH0;
    aippConfig4 |= static_cast<uint64_t>(cscBiasIn1) << AIPP_OFFSET_CSC_IN_CH1;
    aippConfig4 |= static_cast<uint64_t>(cscBiasIn2) << AIPP_OFFSET_CSC_IN_CH2;

    set_aipp_spr_4(aippConfig4);
#endif
}

template <typename T>
__aicore__ inline void SetAippFunctionsImpl5(AippParams<T>& config)
{
#if __NPU_ARCH__ == 3002
    return;
#endif
    uint8_t dtcMeanCh0 = GetScalarBitcodeValue(config.dtcParams.dtcMeanCh0);
    uint8_t dtcMeanCh1 = GetScalarBitcodeValue(config.dtcParams.dtcMeanCh1);
    uint8_t dtcMeanCh2 = GetScalarBitcodeValue(config.dtcParams.dtcMeanCh2);

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    Internal::g_aippDtcMean = static_cast<uint64_t>(dtcMeanCh0);
    Internal::g_aippDtcMean |= static_cast<uint64_t>(dtcMeanCh1) << AIPP_OFFSET_CH1;
    Internal::g_aippDtcMean |= static_cast<uint64_t>(dtcMeanCh2) << AIPP_OFFSET_CH2;
#else
    uint64_t aippConfig5 = static_cast<uint64_t>(dtcMeanCh0);
    aippConfig5 |= static_cast<uint64_t>(dtcMeanCh1) << AIPP_OFFSET_CH1;
    aippConfig5 |= static_cast<uint64_t>(dtcMeanCh2) << AIPP_OFFSET_CH2;

    set_aipp_spr_5(aippConfig5);
#endif
}

template <typename T>
__aicore__ inline void SetAippFunctionsImpl6(AippParams<T>& config)
{
#if __NPU_ARCH__ == 3002
    return;
#endif
    uint16_t dtcMinCh0 = GetScalarBitcodeValue(config.dtcParams.dtcMinCh0);
    uint16_t dtcMinCh1 = GetScalarBitcodeValue(config.dtcParams.dtcMinCh1);
    uint16_t dtcMinCh2 = GetScalarBitcodeValue(config.dtcParams.dtcMinCh2);

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    Internal::g_aippDtcMin = static_cast<uint64_t>(dtcMinCh0);
    Internal::g_aippDtcMin |= static_cast<uint64_t>(dtcMinCh1) << AIPP_OFFSET_CH1;
    Internal::g_aippDtcMin |= static_cast<uint64_t>(dtcMinCh2) << AIPP_OFFSET_CH2;
#else
    uint64_t aippConfig6 = static_cast<uint64_t>(dtcMinCh0);
    aippConfig6 |= static_cast<uint64_t>(dtcMinCh1) << AIPP_OFFSET_CH1;
    aippConfig6 |= static_cast<uint64_t>(dtcMinCh2) << AIPP_OFFSET_CH2;

    set_aipp_spr_6(aippConfig6);
#endif
}

template <typename T>
__aicore__ inline void SetAippFunctionsImpl7(AippParams<T>& config)
{
#if __NPU_ARCH__ == 3002
    return;
#endif
    uint16_t dtcVarCh0 = GetScalarBitcodeValue(config.dtcParams.dtcVarCh0);
    uint16_t dtcVarCh1 = GetScalarBitcodeValue(config.dtcParams.dtcVarCh1);
    uint16_t dtcVarCh2 = GetScalarBitcodeValue(config.dtcParams.dtcVarCh2);

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    Internal::g_aippDtcVar = static_cast<uint64_t>(dtcVarCh0);
    Internal::g_aippDtcVar |= static_cast<uint64_t>(dtcVarCh1) << AIPP_OFFSET_CH1;
    Internal::g_aippDtcVar |= static_cast<uint64_t>(dtcVarCh2) << AIPP_OFFSET_CH2;
#else
    uint64_t aippConfig7 = static_cast<uint64_t>(dtcVarCh0);
    aippConfig7 |= static_cast<uint64_t>(dtcVarCh1) << AIPP_OFFSET_CH1;
    aippConfig7 |= static_cast<uint64_t>(dtcVarCh2) << AIPP_OFFSET_CH2;

    set_aipp_spr_7(aippConfig7);
#endif
}

template <typename T>
__aicore__ inline void SetAippFunctionsImpl8(AippParams<T>& config)
{
    uint64_t aippConfig8 = 0;
    if constexpr (IsSameType<T, int8_t>::value || IsSameType<T, uint8_t>::value) {
        uint8_t paddingValueCh0 = GetScalarBitcodeValue(config.paddingParams.paddingValueCh0);
        uint8_t paddingValueCh1 = GetScalarBitcodeValue(config.paddingParams.paddingValueCh1);
        uint8_t paddingValueCh2 = GetScalarBitcodeValue(config.paddingParams.paddingValueCh2);
        uint8_t paddingValueCh3 = GetScalarBitcodeValue(config.paddingParams.paddingValueCh3);

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
        Internal::g_aippPaddingVal = static_cast<uint64_t>(paddingValueCh0);
        Internal::g_aippPaddingVal |= static_cast<uint64_t>(paddingValueCh1) << AIPP_OFFSET_CH1;
        Internal::g_aippPaddingVal |= static_cast<uint64_t>(paddingValueCh2) << AIPP_OFFSET_CH2;
        Internal::g_aippPaddingVal |= static_cast<uint64_t>(paddingValueCh3) << AIPP_OFFSET_CH3;
#else
        aippConfig8 |= static_cast<uint64_t>(paddingValueCh0);
        aippConfig8 |= static_cast<uint64_t>(paddingValueCh1) << AIPP_OFFSET_CH1;
        aippConfig8 |= static_cast<uint64_t>(paddingValueCh2) << AIPP_OFFSET_CH2;
        aippConfig8 |= static_cast<uint64_t>(paddingValueCh3) << AIPP_OFFSET_CH3;

        set_aipp_spr_8(aippConfig8);
#endif
    } else {
        uint16_t paddingValueCh0 = GetScalarBitcodeValue(config.paddingParams.paddingValueCh0);
        uint16_t paddingValueCh1 = GetScalarBitcodeValue(config.paddingParams.paddingValueCh1);
        uint16_t paddingValueCh2 = GetScalarBitcodeValue(config.paddingParams.paddingValueCh2);
        uint16_t paddingValueCh3 = GetScalarBitcodeValue(config.paddingParams.paddingValueCh3);

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
        Internal::g_aippPaddingVal = static_cast<uint64_t>(paddingValueCh0);
        Internal::g_aippPaddingVal |= static_cast<uint64_t>(paddingValueCh1) << AIPP_OFFSET_CH1;
        Internal::g_aippPaddingVal |= static_cast<uint64_t>(paddingValueCh2) << AIPP_OFFSET_CH2;
        Internal::g_aippPaddingVal |= static_cast<uint64_t>(paddingValueCh3) << AIPP_OFFSET_CH3;
#else
        aippConfig8 |= static_cast<uint64_t>(paddingValueCh0);
        aippConfig8 |= static_cast<uint64_t>(paddingValueCh1) << AIPP_OFFSET_CH1;
        aippConfig8 |= static_cast<uint64_t>(paddingValueCh2) << AIPP_OFFSET_CH2;
        aippConfig8 |= static_cast<uint64_t>(paddingValueCh3) << AIPP_OFFSET_CH3;

        set_aipp_spr_8(aippConfig8);
#endif
    }
}

template <typename T>
__aicore__ inline void SetAippFunctionsImpl9(AippInputFormat format, AippParams<T>& config)
{
    uint64_t aippConfig9 = 0;

    if constexpr (IsSameType<T, int8_t>::value || IsSameType<T, uint8_t>::value) {
        uint8_t cPaddingValue = GetScalarBitcodeValue(config.cPaddingParams.cPaddingValue);
        aippConfig9 |= static_cast<uint64_t>(cPaddingValue);
    } else {
        uint16_t cPaddingValue = GetScalarBitcodeValue(config.cPaddingParams.cPaddingValue);
        aippConfig9 |= static_cast<uint64_t>(cPaddingValue);
    }

    if (config.swapParams.isSwapRB) {
        aippConfig9 |= static_cast<uint64_t>(1) << AIPP_OFFSET_SWAP_RB;
    }
    if (config.swapParams.isSwapUV) {
        aippConfig9 |= static_cast<uint64_t>(1) << AIPP_OFFSET_SWAP_UV;
    }
    if (config.swapParams.isSwapAX) {
        aippConfig9 |= static_cast<uint64_t>(1) << AIPP_OFFSET_SWAP_AX;
    }

    aippConfig9 |= (static_cast<uint64_t>(format) & 0x1f) << AIPP_OFFSET_FORMAT;

    if (config.singleLineParams.isSingleLineCopy) {
        aippConfig9 |= static_cast<uint64_t>(1) << AIPP_OFFSET_SINGLE_LINE;
    }

    aippConfig9 |= (static_cast<uint64_t>(config.paddingParams.paddingMode) & 0x3) << AIPP_OFFSET_PADDING_MODE;

#if __NPU_ARCH__ == 3002
    aippConfig9 |= (static_cast<uint64_t>(config.dtcParams.dtcRoundMode) & 0x1) << AIPP_OFFSET_DTC_ROUND_MODE;
#endif

    aippConfig9 |= (static_cast<uint64_t>(config.cPaddingParams.cPaddingMode) & 0x1) << AIPP_OFFSET_CPADDING_MODE;

    set_aipp_spr_9(aippConfig9);
}

template <typename T>
__aicore__ inline void SetAippFunctionsImpl18(AippParams<T>& config)
{
#if __NPU_ARCH__ != 3002 && (__NPU_ARCH__ != 5102)
    return;
#endif
    float dtcVarCh0f = static_cast<float>(config.dtcParams.dtcVarCh0);
    float dtcVarCh1f = static_cast<float>(config.dtcParams.dtcVarCh1);
    uint32_t dtcVarCh0 = GetScalarBitcodeValue(dtcVarCh0f);
    uint32_t dtcVarCh1 = GetScalarBitcodeValue(dtcVarCh1f);

    uint64_t aippConfig18 = static_cast<uint64_t>(dtcVarCh0);
    aippConfig18 |= static_cast<uint64_t>(dtcVarCh1) << AIPP_OFFSET_DTC_CH1;

    set_aipp_spr_18(aippConfig18);
}

template <typename T>
__aicore__ inline void SetAippFunctionsImpl19(AippParams<T>& config)
{
#if __NPU_ARCH__ != 3002 && (__NPU_ARCH__ != 5102)
    return;
#endif
    float dtcVarCh2f = static_cast<float>(config.dtcParams.dtcVarCh2);
    uint32_t dtcVarCh2 = GetScalarBitcodeValue(dtcVarCh2f);
    uint64_t aippConfig19 = static_cast<uint64_t>(dtcVarCh2);
    set_aipp_spr_19(aippConfig19);
}

template <typename T>
__aicore__ inline void SetAippFunctionsImpl20(AippParams<T>& config)
{
#if __NPU_ARCH__ != 3002 && (__NPU_ARCH__ != 5102)
    return;
#endif
    float dtcMeanCh0f = static_cast<float>(config.dtcParams.dtcMeanCh0 * 1.0f);
    float dtcMeanCh1f = static_cast<float>(config.dtcParams.dtcMeanCh1 * 1.0f);

    uint32_t dtcMeanCh0 = GetScalarBitcodeValue(dtcMeanCh0f);
    uint32_t dtcMeanCh1 = GetScalarBitcodeValue(dtcMeanCh1f);

    uint64_t aippConfig20 = static_cast<uint64_t>(dtcMeanCh0);
    aippConfig20 |= static_cast<uint64_t>(dtcMeanCh1) << AIPP_OFFSET_DTC_CH1;

    set_aipp_spr_20(aippConfig20);
}

template <typename T>
__aicore__ inline void SetAippFunctionsImpl21(AippParams<T>& config)
{
#if __NPU_ARCH__ != 3002 && (__NPU_ARCH__ != 5102)
    return;
#endif
    float dtcMeanCh2f = static_cast<float>(config.dtcParams.dtcMeanCh2 * 1.0f);
    uint32_t dtcMeanCh2 = GetScalarBitcodeValue(dtcMeanCh2f);
    uint64_t aippConfig21 = static_cast<uint64_t>(dtcMeanCh2);
    set_aipp_spr_21(aippConfig21);
}

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
template <typename T>
__aicore__ inline void SetAippFunctionsImpl22(AippInputFormat format, AippParams<T>& config)
{
    Internal::g_aippArgs = 0;
    if constexpr (IsSameType<T, int8_t>::value || IsSameType<T, uint8_t>::value) {
        uint8_t cPaddingValue = GetScalarBitcodeValue(config.cPaddingParams.cPaddingValue);
        Internal::g_aippArgs |= static_cast<uint64_t>(cPaddingValue);
    } else {
        uint16_t cPaddingValue = GetScalarBitcodeValue(config.cPaddingParams.cPaddingValue);
        Internal::g_aippArgs |= static_cast<uint64_t>(cPaddingValue);
    }

    if (config.swapParams.isSwapRB) {
        Internal::g_aippArgs |= static_cast<uint64_t>(1) << AIPP_OFFSET_SWAP_RB;
    }
    if (config.swapParams.isSwapUV) {
        Internal::g_aippArgs |= static_cast<uint64_t>(1) << AIPP_OFFSET_SWAP_UV;
    }
    if (config.swapParams.isSwapAX) {
        Internal::g_aippArgs |= static_cast<uint64_t>(1) << AIPP_OFFSET_SWAP_AX;
    }

    Internal::g_aippArgs |= (static_cast<uint64_t>(format) & 0x1f) << AIPP_OFFSET_FORMAT;

    if (config.singleLineParams.isSingleLineCopy) {
        Internal::g_aippArgs |= static_cast<uint64_t>(1) << AIPP_OFFSET_SINGLE_LINE;
    }

    Internal::g_aippArgs |= (static_cast<uint64_t>(config.paddingParams.paddingMode) & 0x3) << AIPP_OFFSET_PADDING_MODE;

    Internal::g_aippArgs |= (static_cast<uint64_t>(config.cPaddingParams.cPaddingMode) & 0x1)
                            << AIPP_OFFSET_CPADDING_MODE;
}
#endif

template <typename T, typename U>
__aicore__ inline void SetAippFunctionsImpl(
    __gm__ T* src0, __gm__ T* src1, AippInputFormat format, AippParams<U>& config)
{
#if __NPU_ARCH__ == 2201
    if ASCEND_IS_AIV {
        return;
    }
#endif // __NPU_ARCH__ == 2201
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    static_assert(SupportType<T, uint8_t>(), "Input type T only supports uint8_t on current device.");
    static_assert(
        SupportType<U, uint8_t, int8_t, half>(), "Input type T only supports uint8_t, int8_t, half on current device.");
#endif
#if __NPU_ARCH__ == 3002
    SetAippFunctionsImpl0<T>(src0);
    SetAippFunctionsImpl1<T, U>(src1, config);
    SetAippFunctionsImpl2<U>(config);
    SetAippFunctionsImpl3<U>(config);
    SetAippFunctionsImpl4<U>(config);
    SetAippFunctionsImpl8<U>(config);
    SetAippFunctionsImpl9<U>(format, config);
    SetAippFunctionsImpl18<U>(config);
    SetAippFunctionsImpl19<U>(config);
    SetAippFunctionsImpl20<U>(config);
    SetAippFunctionsImpl21<U>(config);
#else
    SetAippFunctionsImpl0<T>(src0);
    SetAippFunctionsImpl1<T, U>(src1, config);
    SetAippFunctionsImpl2<U>(config);
    SetAippFunctionsImpl3<U>(config);
    SetAippFunctionsImpl4<U>(config);
    SetAippFunctionsImpl5<U>(config);
    SetAippFunctionsImpl6<U>(config);
    SetAippFunctionsImpl7<U>(config);
    SetAippFunctionsImpl8<U>(config);
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    SetAippFunctionsImpl22<U>(format, config);
#else
    SetAippFunctionsImpl9<U>(format, config);
#endif
#endif // __NPU_ARCH__ == 3002
}

template <typename T, typename U>
__aicore__ inline void SetAippFunctionsImpl(__gm__ T* src0, AippInputFormat format, AippParams<U> config)
{
#if __NPU_ARCH__ == 2201
    if ASCEND_IS_AIV {
        return;
    }
#endif // __NPU_ARCH__ == 2201
    SetAippFunctionsImpl(src0, reinterpret_cast<__gm__ T*>(0), format, config);
}
#endif // (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 2002) || (__NPU_ARCH__ == 3002)

} // namespace AscendC
#endif // ASCENDC_KERNEL_REG_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_H__
#endif
