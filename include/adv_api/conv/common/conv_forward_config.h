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
 * \file conv_config.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "conv_forward_config.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONV_FORWARD_CONFIG_H__
#endif

#ifndef ADV_API_CONV_COMMON_CONV_FORWARD_CONFIG_H
#define ADV_API_CONV_COMMON_CONV_FORWARD_CONFIG_H

#include <climits>
#include "../../../../impl/basic_api/kernel_utils.h"
#include "conv_common.h"
#include "../../../../impl/adv_api/detail/conv/common/conv_forward_framework_util.h"
#include "../../../../impl/adv_api/detail/conv/common/conv_forward_util.h"

namespace ConvApi {

enum ConvCfgTypeID { CONV_ID_Unknown, CONV_ID_Normal, CONV_ID_Test, CONV_ID_END };

struct ConvParam {
    __aicore__ inline ConvParam(){};
    constexpr static int8_t outputOrder = -1;
    constexpr static int8_t l0pingpong = -1;
    constexpr static int8_t bl1bypass = -1;
    constexpr static int8_t groupConvType = -1;
};

CONV_DECLARE_DEFINE_MEMBER(ConvParam, outputOrder, int8_t, INT8_MAX)
CONV_DECLARE_DEFINE_MEMBER(ConvParam, l0pingpong, int8_t, INT8_MAX)
CONV_DECLARE_DEFINE_MEMBER(ConvParam, bl1bypass, int8_t, INT8_MAX)
CONV_DECLARE_DEFINE_MEMBER(ConvParam, groupConvType, int8_t, INT8_MAX)

template <typename T>
struct GetDstType {
    using Type = T;
};

template <>
struct GetDstType<float> {
    using Type = float;
};

template <>
struct GetDstType<half> {
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165)
    using Type = int32_t;
#else
    using Type = float;
#endif
};

template <>
struct GetDstType<bfloat16_t> {
    using Type = float;
};

template <>
struct GetDstType<int8_t> {
    using Type = int32_t;
};

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165)
template <>
struct GetDstType<int16_t> {
    using Type = int32_t;
};

template <>
struct GetDstType<fp8_e4m3fn_t> {
    using Type = int32_t;
};
#endif

template <AscendC::TPosition POSITION, ConvCommonApi::ConvFormat FORMAT, typename TYPE>
struct ConvType {
    constexpr static AscendC::TPosition pos = POSITION;
    constexpr static ConvCommonApi::ConvFormat format = FORMAT;
    using T = TYPE;
};

template <class INPUT_TYPE, class WEIGHT_TYPE, class OUTPUT_TYPE, class BIAS_TYPE, class CONV_CFG>
struct ConvDataType {
    using ConvParam = CONV_CFG;
    using SrcT = typename INPUT_TYPE::T;
    using SrcAT = typename INPUT_TYPE::T;
    using SrcBT = typename WEIGHT_TYPE::T;
    using DstT = typename OUTPUT_TYPE::T;
    using BiasT = typename BIAS_TYPE::T;
    using L0cT = typename GetDstType<SrcT>::Type;

    constexpr static uint32_t configID = (uint32_t)ConvCfgTypeID::CONV_ID_Normal;
    constexpr static uint32_t implType = (uint32_t)ConvCfgTypeID::CONV_ID_Normal;
    constexpr static uint32_t intfType = (uint32_t)ConvCfgTypeID::CONV_ID_Normal;

    constexpr static auto formatA = ConvCommonApi::ConvFormat::NDC1HWC0;
    constexpr static auto formatB = WEIGHT_TYPE::format;
    constexpr static auto formatC = OUTPUT_TYPE::format;
    constexpr static auto formatBias = BIAS_TYPE::format;

    constexpr static auto posA = INPUT_TYPE::pos;
    constexpr static auto posB = WEIGHT_TYPE::pos;
    constexpr static auto posC = OUTPUT_TYPE::pos;
    constexpr static auto posBias = BIAS_TYPE::pos;

    constexpr static bool isBias = true;

    using ContextData = struct _ {
        __aicore__ inline _() {}
    };
};

template <class ConvDataType>
struct ConvConfig : public ConvDataType {
public:
    __aicore__ inline ConvConfig() {}

    using ContextData = struct _ : public ConvDataType::ContextData {
        __aicore__ inline _() {}
    };
};

} // namespace ConvApi
#endif // ADV_API_CONV_COMMON_CONV_FORWARD_CONFIG_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONV_FORWARD_CONFIG_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_CONV_FORWARD_CONFIG_H__
#endif
