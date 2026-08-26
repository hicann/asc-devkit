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
 * \file kernel_struct_fixpipe.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "kernel_struct_fixpipe.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_STRUCT_FIXPIPE_H__
#endif

#ifndef ASCENDC_MODULE_STRUCT_FIXPIPE_H
#define ASCENDC_MODULE_STRUCT_FIXPIPE_H

#include "../../impl/basic_api/kernel_macros.h"
#include "../../impl/basic_api/utils/kernel_utils_struct_dma_params.h"

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <cstdint>
#include "stub_def.h"
#include "stub_fun.h"
#endif

#if (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5101) || (__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || \
    (__NPU_ARCH__ == 5163)
#define CANN_ASC_ENABLE_FIX_SHIFT
#endif

namespace AscendC {
enum class CO2Layout : uint8_t {
    NZ = 0,
    ROW_MAJOR,   // ND Row
    COLUMN_MAJOR // ND Column
};

struct FixpipeConfig {
    CO2Layout format;
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102)
    bool isToUB;
#endif
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5102)
    bool enableFixVal = false;
#endif

    __aicore__ constexpr inline FixpipeConfig(CO2Layout format_)
        : format(format_)
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102)
          ,
          isToUB(false)
#endif
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5102)
          ,
          enableFixVal(false)
#endif
    {}
    __aicore__ constexpr inline FixpipeConfig(CO2Layout format_, bool isToUB_)
        : format(format_)
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102)
          ,
          isToUB(isToUB_)
#endif
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5102)
          ,
          enableFixVal(false)
#endif
    {}
    __aicore__ constexpr inline FixpipeConfig(CO2Layout format_, bool isToUB_, bool enableFixVal_)
        : format(format_)
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102)
          ,
          isToUB(isToUB_)
#endif
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5102)
          ,
          enableFixVal(enableFixVal_)
#endif
    {}
};

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102)
#if (__NPU_ARCH__ == 5102)
constexpr FixpipeConfig CFG_NZ_FIX = {CO2Layout::NZ, false, true};
#endif
constexpr FixpipeConfig CFG_NZ = {CO2Layout::NZ, false};
constexpr FixpipeConfig CFG_ROW_MAJOR = {CO2Layout::ROW_MAJOR, false};
constexpr FixpipeConfig CFG_COLUMN_MAJOR = {CO2Layout::COLUMN_MAJOR, false};

#else
constexpr FixpipeConfig CFG_NZ = {CO2Layout::NZ};
constexpr FixpipeConfig CFG_ROW_MAJOR = {CO2Layout::ROW_MAJOR};
constexpr FixpipeConfig CFG_COLUMN_MAJOR = {CO2Layout::COLUMN_MAJOR};
#endif

struct FixpipeParamsV220 {
    __aicore__ FixpipeParamsV220() {}

    __aicore__ FixpipeParamsV220(
        const uint16_t nSizeIn, const uint16_t mSizeIn, const uint16_t srcStrideIn, const uint32_t dstStrideIn,
        const bool reluEnIn)
        : nSize(nSizeIn), mSize(mSizeIn), srcStride(srcStrideIn), dstStride(dstStrideIn), reluEn(reluEnIn)
    {}

    __aicore__ FixpipeParamsV220(
        const uint16_t nSizeIn, const uint16_t mSizeIn, const uint16_t srcStrideIn, const uint32_t dstStrideIn,
        const bool reluEnIn, const QuantMode_t quantPreIn, const int64_t deqScalarIn, const uint16_t ndNumIn,
        const uint16_t srcNdStrideIn, const uint16_t dstNdStrideIn, const uint8_t unitFlagIn)
        : nSize(nSizeIn),
          mSize(mSizeIn),
          srcStride(srcStrideIn),
          dstStride(dstStrideIn),
          reluEn(reluEnIn),
          quantPre(quantPreIn),
          deqScalar(deqScalarIn),
          ndNum(ndNumIn),
          srcNdStride(srcNdStrideIn),
          dstNdStride(dstNdStrideIn),
          unitFlag(unitFlagIn)
    {}

    __aicore__ FixpipeParamsV220(
        const uint16_t nSizeIn, const uint16_t mSizeIn, const uint16_t srcStrideIn, const uint32_t dstStrideIn,
        const bool reluEnIn, const QuantMode_t quantPreIn, const int64_t deqScalarIn, const uint16_t ndNumIn,
        const uint16_t srcNdStrideIn, const uint16_t dstNdStrideIn, const uint8_t unitFlagIn,
        const bool isChannelSplitIn)
        : nSize(nSizeIn),
          mSize(mSizeIn),
          srcStride(srcStrideIn),
          dstStride(dstStrideIn),
          reluEn(reluEnIn),
          quantPre(quantPreIn),
          deqScalar(deqScalarIn),
          ndNum(ndNumIn),
          srcNdStride(srcNdStrideIn),
          dstNdStride(dstNdStrideIn),
          unitFlag(unitFlagIn),
          isChannelSplit(isChannelSplitIn)
    {}

    uint16_t nSize = 0;
    uint16_t mSize = 0; // M-DirectionSize
    uint16_t srcStride = 0;
    uint32_t dstStride = 0;
    // Params: used for Quant
    QuantMode_t quantPre = QuantMode_t::NoQuant;
    uint64_t deqScalar;
    // Params: used for nz2nd
    uint16_t ndNum = 1;
    uint16_t srcNdStride = 0;
    uint16_t dstNdStride = 0;
    bool reluEn = false;
    uint8_t unitFlag = 0;
    bool isChannelSplit = false;
};

// 根据模板参数选结构体
template <CO2Layout format>
struct TransformParams {};

template <>
struct TransformParams<CO2Layout::NZ> {
    __aicore__ inline TransformParams(){};
    using PARAMS = uint8_t;
};

template <>
struct TransformParams<CO2Layout::ROW_MAJOR> {
    __aicore__ inline TransformParams(){};
    using PARAMS = Nz2NdParams;
};

template <>
struct TransformParams<CO2Layout::COLUMN_MAJOR> {
    __aicore__ inline TransformParams(){};
    using PARAMS = Nz2DnParams;
};

template <CO2Layout format = CO2Layout::ROW_MAJOR>
struct FixpipeParamsArch3510 {
    __aicore__ FixpipeParamsArch3510() {}

    __aicore__ FixpipeParamsArch3510(
        const uint16_t nSizeIn, const uint16_t mSizeIn, const uint16_t srcStrideIn, const uint32_t dstStrideIn)
    {
        nSize = nSizeIn;
        mSize = mSizeIn;
        srcStride = srcStrideIn;
        dstStride = dstStrideIn;
    }
    ReluMode preReluMode = ReluMode::NO_RELU;
    ClipReluMode preClipReluMode = ClipReluMode::NOCLIP_RELU;
    uint64_t reluScalar;
    uint64_t vectorRelu;
    uint16_t nSize = 0;
    uint16_t mSize = 0; // M-DirectionSize
    uint16_t srcStride = 0;
    uint32_t dstStride = 0;
    // Params: used for Quant
    QuantMode_t quantPre = QuantMode_t::NoQuant;
    uint64_t deqScalar;
    bool reluEn = false;
    uint8_t unitFlag = 0;
    // c310 extend param
    uint8_t dualDstCtl = 0;
    bool subBlockId = false;
    typename TransformParams<format>::PARAMS params;
    bool isChannelSplit = false;
#ifdef CANN_ASC_ENABLE_FIX_SHIFT
    uint8_t fixShiftVal = 0;
#endif
};

template <CO2Layout format = CO2Layout::ROW_MAJOR>
struct FixpipeParamsC310 : FixpipeParamsArch3510<format> {
    __aicore__ FixpipeParamsC310() : FixpipeParamsArch3510<format>() {}

    __aicore__ FixpipeParamsC310(
        const uint16_t nSizeIn, const uint16_t mSizeIn, const uint16_t srcStrideIn, const uint32_t dstStrideIn)
        : FixpipeParamsArch3510<format>(nSizeIn, mSizeIn, srcStrideIn, dstStrideIn)
    {}
};

using FixpipeParamsM300 = FixpipeParamsV220;
using FixpipeParamsM310 = FixpipeParamsV220;
} // namespace AscendC

#endif // ASCENDC_MODULE_STRUCT_FIXPIPE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_STRUCT_FIXPIPE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_STRUCT_FIXPIPE_H__
#endif
