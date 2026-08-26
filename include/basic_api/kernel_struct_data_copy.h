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
 * \file kernel_struct_data_copy.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "kernel_struct_data_copy.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_STRUCT_DATA_COPY_H__
#endif
#ifndef ASCENDC_MODULE_STRUCT_DATA_COPY_H
#define ASCENDC_MODULE_STRUCT_DATA_COPY_H

#include "../../impl/basic_api/kernel_macros.h"
#include "../../impl/basic_api/utils/kernel_utils_constants.h"
#include "../../impl/basic_api/utils/kernel_utils_mode.h"
#include "../../impl/basic_api/utils/kernel_utils_mode_cpu.h"

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <cstdint>
#include "stub_def.h"
#include "stub_fun.h"
#endif

namespace AscendC {
#ifndef ASCC_ENUM_DATAFORMAT
#define ASCC_ENUM_DATAFORMAT
enum class DataFormat : uint8_t {
    ND = 0,
    NZ,
    NCHW,
    NC1HWC0,
    NHWC,
    NCDHW,
    NDC1HWC0,
    FRACTAL_Z_3D,
};
#endif // ASCC_ENUM_DATAFORMAT

#if (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5101) || (__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || \
    (__NPU_ARCH__ == 5163)
#define CANN_ASC_ENABLE_FIX_SHIFT
#endif

#if (__NPU_ARCH__ == 5101) || (__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163)
#define CANN_ASC_ENABLE_POST_PROC_BUF
#define CANN_ASC_ENABLE_SID
#endif

struct DataCopyParams {
    __aicore__ DataCopyParams() {}

    __aicore__ DataCopyParams(
        const uint16_t count, const uint16_t len, const uint16_t srcStrideIn, const uint16_t dstStrideIn)
        : blockCount(count), blockLen(len), srcStride(srcStrideIn), dstStride(dstStrideIn)
    {}

    uint16_t blockCount = DEFAULT_DATA_COPY_NBURST;
    uint16_t blockLen = 0;
    // srcStride and dstStride will be deprecated, use srcGap and dstGap instead.
    union {
        uint16_t srcGap = 0;
        // srcStride will be deprecated, use srcGap instead
        uint16_t srcStride;
    };

    union {
        uint16_t dstGap = 0;
        // dstStride will be deprecated, use dstGap instead
        uint16_t dstStride;
    };
#ifdef CANN_ASC_ENABLE_FIX_SHIFT
    uint8_t fixShiftVal = 0;
#endif
#ifdef CANN_ASC_ENABLE_POST_PROC_BUF
    uint8_t postProcBufBlock = 0;
#endif
#ifdef CANN_ASC_ENABLE_SID
    uint8_t sid = 0;
#endif
};

enum class DataCopyMVType : uint8_t {
    UB_TO_OUT = 0,
    OUT_TO_UB = 1,
};

struct NdDmaConfig {
    static constexpr uint16_t unsetPad = 0xffff;
    bool isNearestValueMode = false;
    uint16_t loopLpSize = unsetPad; // Left padding size of all dimensions, must be less than 256.
    uint16_t loopRpSize = unsetPad; // Right padding size of all dimensions, must be less than 256.
    bool ascOptimize = false;       // used for AscendC optimization on special senario.
};
using MultiCopyConfig = NdDmaConfig; // reserve old name
constexpr NdDmaConfig kDefaultNdDmaConfig = {false, NdDmaConfig::unsetPad, NdDmaConfig::unsetPad, false};
constexpr NdDmaConfig kDefaultMultiCopyConfig = {
    false, NdDmaConfig::unsetPad, NdDmaConfig::unsetPad, false}; // reserve old name

template <uint8_t dim>
struct MultiCopyLoopInfo {
    static_assert(dim >= 1 && dim <= 5, "MultiCopy Dims must be between 1 and 5.");

    // Index [0, dim) represents lowerest dimension to highest dimension accordingly.
    uint64_t loopSrcStride[dim] = {0}; // src stride info per loop.
    uint32_t loopDstStride[dim] = {0}; // dst stride info per loop.
    uint32_t loopSize[dim] = {0};      // Loop size per loop.
    uint8_t loopLpSize[dim] = {0};     // Left padding size per loop.
    uint8_t loopRpSize[dim] = {0};     // Right padding size per loop.
};

template <uint8_t dim>
using NdDmaLoopInfo = MultiCopyLoopInfo<dim>;

template <typename T, uint8_t dim>
struct MultiCopyParams {
    MultiCopyLoopInfo<dim> loopInfo;
    T constantValue;
};

template <typename T, uint8_t dim>
using NdDmaParams = MultiCopyParams<T, dim>;

struct DataCopyEnhancedParams {
    __aicore__ DataCopyEnhancedParams() {}

    __aicore__ DataCopyEnhancedParams(
        const BlockMode blockModeIn, const DeqScale deqScaleIn, const uint64_t deqValueIn, const uint8_t sidStoreModeIn,
        const bool isReluIn, const pad_t padModeIn, const uint64_t padValueIn)
        : blockMode(blockModeIn),
          deqScale(deqScaleIn),
          deqValue(deqValueIn),
          sidStoreMode(sidStoreModeIn),
          isRelu(isReluIn),
          padMode(padModeIn),
          padValue(padValueIn)
    {}

    BlockMode blockMode = BlockMode::BLOCK_MODE_NORMAL;
    DeqScale deqScale = DeqScale::DEQ_NONE;
    uint64_t deqValue = 0;
    uint8_t sidStoreMode = 0;
    bool isRelu = false;
    pad_t padMode = pad_t::PAD_NONE;
    uint64_t padValue = 0;
    uint64_t deqTensorAddr = 0;
};

struct DataCopyCO12DstParams {
    __aicore__ DataCopyCO12DstParams() {}

    __aicore__ DataCopyCO12DstParams(
        const uint16_t nSizeIn, const uint16_t mSizeIn, const uint32_t dstStrideIn, const uint16_t srcStrideIn,
        const QuantMode_t quantPreIn, const uint8_t reluPreIn, const bool channelSplitIn, const bool nz2ndEnIn)
        : nSize(nSizeIn),
          mSize(mSizeIn),
          dstStride(dstStrideIn),
          srcStride(srcStrideIn),
          quantPre(quantPreIn),
          reluPre(reluPreIn),
          channelSplit(channelSplitIn),
          nz2ndEn(nz2ndEnIn)
    {}

    uint8_t sid = 0;
    uint16_t nSize = 0;
    uint16_t mSize = 0;
    uint32_t dstStride = DEFAULT_DATA_COPY_STRIDE;
    uint16_t srcStride = DEFAULT_DATA_COPY_STRIDE;
    uint8_t unitFlag = 0;
    uint8_t clipReluPre = 0;
    uint8_t eltWiseOp = 0;
    QuantMode_t quantPre = QuantMode_t::NoQuant;
    uint8_t reluPre = 0;
    bool channelSplit = false;
    bool nz2ndEn = false;
};

struct DataCopyPadParams {
    __aicore__ DataCopyPadParams() {}

    __aicore__ DataCopyPadParams(
        const bool isPadValue, const uint8_t leftPadValue, const uint8_t rightPadValue, const uint64_t padValue)
        : isPad(isPadValue), leftPadding(leftPadValue), rightPadding(rightPadValue), paddingValue(padValue)
    {}

    bool isPad = false;
    uint8_t leftPadding = 0;
    uint8_t rightPadding = 0;
    uint64_t paddingValue = 0;
};

struct DataCopyExtParams {
    __aicore__ DataCopyExtParams(){}

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5101) || \
                              (__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
    __aicore__ DataCopyExtParams(
        const uint16_t count, const uint32_t len, const int64_t srcStrideIn, const int64_t dstStrideIn,
        const uint32_t rsvIn)
#else
    __aicore__ DataCopyExtParams(
        const uint16_t count, const uint32_t len, const uint32_t srcStrideIn, const uint32_t dstStrideIn,
        const uint32_t rsvIn)
#endif
        : blockCount(count), blockLen(len), srcStride(srcStrideIn), dstStride(dstStrideIn), rsv(rsvIn)
    {}

    uint16_t blockCount = DEFAULT_DATA_COPY_NBURST;
    uint32_t blockLen = 0;
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5101) || \
                              (__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
    int64_t srcStride = static_cast<int64_t>(DEFAULT_DATA_COPY_STRIDE);
    int64_t dstStride = static_cast<int64_t>(DEFAULT_DATA_COPY_STRIDE);
#else
    uint32_t srcStride = DEFAULT_DATA_COPY_STRIDE;
    uint32_t dstStride = DEFAULT_DATA_COPY_STRIDE;
#endif
    uint32_t rsv = 0; // reserved information
#ifdef CANN_ASC_ENABLE_SID
    uint8_t sid = 0;
#endif
};

template <typename T>
struct DataCopyPadExtParams {
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5101) || \
                              (__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
    using TYPE = typename GetPadValueType<T>::Type;
    __aicore__ DataCopyPadExtParams()
    {
        isPad = false;
        leftPadding = 0;
        rightPadding = 0;
        paddingValue = 0;
    }
    __aicore__ DataCopyPadExtParams(
        const bool isPadValue, const uint8_t leftPadValue, const uint8_t rightPadValue, T padValue)
    {
        isPad = isPadValue;
        leftPadding = leftPadValue;
        rightPadding = rightPadValue;
        paddingValue = *(reinterpret_cast<TYPE*>(&padValue));
    }
    bool isPad = false;
    uint8_t leftPadding = 0;
    uint8_t rightPadding = 0;
    TYPE paddingValue = 0;
#else
    __aicore__ DataCopyPadExtParams() {}

    __aicore__ DataCopyPadExtParams(
        const bool isPadValue, const uint8_t leftPadValue, const uint8_t rightPadValue, T padValue)
        : isPad(isPadValue), leftPadding(leftPadValue), rightPadding(rightPadValue), paddingValue(padValue)
    {}

    bool isPad = false;
    uint8_t leftPadding = 0;
    uint8_t rightPadding = 0;
    T paddingValue = 0;
#endif
};

struct Nd2NzParams {
    __aicore__ Nd2NzParams(){}

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5101) || \
                              (__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
    __aicore__ Nd2NzParams(
        const uint16_t ndNumIn, const uint16_t nValueIn, const uint32_t dValueIn, const uint64_t srcNdMatrixStrideIn,
        const uint64_t srcDValueIn, const uint16_t dstNzC0StrideIn, const uint16_t dstNzNStrideIn,
        const uint32_t dstNzMatrixStrideIn)
        : ndNum(ndNumIn),
          nValue(nValueIn),
          dValue(dValueIn),
          srcNdMatrixStride(srcNdMatrixStrideIn),
          srcDValue(srcDValueIn),
          dstNzC0Stride(dstNzC0StrideIn),
          dstNzNStride(dstNzNStrideIn),
          dstNzMatrixStride(dstNzMatrixStrideIn)
    {}

    uint16_t ndNum = 0;
    uint16_t nValue = 0;
    uint32_t dValue = 0;
    uint64_t srcNdMatrixStride = 0;
    uint64_t srcDValue = 0;
    uint16_t dstNzC0Stride = 0;
    uint16_t dstNzNStride = 0;
    uint32_t dstNzMatrixStride = 0;
#ifdef CANN_ASC_ENABLE_SID
    uint8_t sid = 0;
#endif
#else
    __aicore__ Nd2NzParams(
        const uint16_t ndNumIn, const uint16_t nValueIn, const uint16_t dValueIn, const uint16_t srcNdMatrixStrideIn,
        const uint16_t srcDValueIn, const uint16_t dstNzC0StrideIn, const uint16_t dstNzNStrideIn,
        const uint16_t dstNzMatrixStrideIn)
        : ndNum(ndNumIn),
          nValue(nValueIn),
          dValue(dValueIn),
          srcNdMatrixStride(srcNdMatrixStrideIn),
          srcDValue(srcDValueIn),
          dstNzC0Stride(dstNzC0StrideIn),
          dstNzNStride(dstNzNStrideIn),
          dstNzMatrixStride(dstNzMatrixStrideIn)
    {
    }

    uint16_t ndNum = 0;
    uint16_t nValue = 0;
    uint16_t dValue = 0;
    uint16_t srcNdMatrixStride = 0;
    uint16_t srcDValue = 0;
    uint16_t dstNzC0Stride = 0;
    uint16_t dstNzNStride = 0;
    uint16_t dstNzMatrixStride = 0;
#endif
};

struct Nz2NdParamsFull {
    __aicore__ Nz2NdParamsFull(){}

#if defined(__NPU_ARCH__) && \
    ((__NPU_ARCH__ == 5101) || (__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
    __aicore__ Nz2NdParamsFull(
        const uint16_t ndNumIn, const uint16_t nValueIn, const uint32_t dValueIn, const uint16_t srcNdMatrixStrideIn,
        const uint16_t srcNStrideIn, const uint64_t dstDStrideIn, const uint64_t dstNdMatrixStrideIn)
        : ndNum(ndNumIn),
          nValue(nValueIn),
          dValue(dValueIn),
          srcNdMatrixStride(srcNdMatrixStrideIn),
          srcNStride(srcNStrideIn),
          dstDStride(dstDStrideIn),
          dstNdMatrixStride(dstNdMatrixStrideIn)
    {}

    uint16_t ndNum = 1;
    uint16_t nValue = 0;
    uint32_t dValue = 0;
    uint16_t srcNdMatrixStride = 1;
    uint16_t srcNStride = 0;
    uint64_t dstDStride = 0;
    uint64_t dstNdMatrixStride = 1;
#ifdef CANN_ASC_ENABLE_SID
    uint8_t sid = 0;
#endif
#else
    __aicore__ Nz2NdParamsFull(
        const uint16_t ndNumIn, const uint16_t nValueIn, const uint16_t dValueIn, const uint16_t srcNdMatrixStrideIn,
        const uint16_t srcNStrideIn, const uint16_t dstDStrideIn, const uint16_t dstNdMatrixStrideIn)
        : ndNum(ndNumIn),
          nValue(nValueIn),
          dValue(dValueIn),
          srcNdMatrixStride(srcNdMatrixStrideIn),
          srcNStride(srcNStrideIn),
          dstDStride(dstDStrideIn),
          dstNdMatrixStride(dstNdMatrixStrideIn)
    {
    }

    uint16_t ndNum = 1;
    uint16_t nValue = 0;
    uint16_t dValue = 0;
    uint16_t srcNdMatrixStride = 1;
    uint16_t srcNStride = 0;
    uint16_t dstDStride = 0;
    uint16_t dstNdMatrixStride = 1;
#endif
};

struct Dn2NzParams {
    __aicore__ Dn2NzParams() {}

    __aicore__ Dn2NzParams(
        const uint16_t dnNumIn, const uint16_t nValueIn, const uint32_t dValueIn, const uint64_t srcDnMatrixStrideIn,
        const uint64_t srcDValueIn, const uint16_t dstNzC0StrideIn, const uint16_t dstNzNStrideIn,
        const uint32_t dstNzMatrixStrideIn)
    {
        dnNum = dnNumIn;
        nValue = nValueIn;
        dValue = dValueIn;
        srcDnMatrixStride = srcDnMatrixStrideIn;
        srcDValue = srcDValueIn;
        dstNzC0Stride = dstNzC0StrideIn;
        dstNzNStride = dstNzNStrideIn;
        dstNzMatrixStride = dstNzMatrixStrideIn;
    }

    uint16_t dnNum = 0;
    uint16_t nValue = 0;
    uint32_t dValue = 0;
    uint64_t srcDnMatrixStride = 0;
    uint64_t srcDValue = 0;
    uint16_t dstNzC0Stride = 0;
    uint16_t dstNzNStride = 0;
    uint32_t dstNzMatrixStride = 0;
#ifdef CANN_ASC_ENABLE_SID
    uint8_t sid = 0;
#endif
};

struct Nz2DnParamsFull {
    __aicore__ Nz2DnParamsFull() {}

    __aicore__ Nz2DnParamsFull(
        const uint16_t dnNumIn, const uint16_t nValueIn, const uint32_t dValueIn, const uint16_t srcNzMatrixStrideIn,
        const uint16_t srcNStrideIn, const uint64_t dstNStrideIn, const uint64_t dstDnMatrixStrideIn)
    {
        dnNum = dnNumIn;
        nValue = nValueIn;
        dValue = dValueIn;
        srcNzMatrixStride = srcNzMatrixStrideIn;
        srcNStride = srcNStrideIn;
        dstNStride = dstNStrideIn;
        dstDnMatrixStride = dstDnMatrixStrideIn;
    }

    uint16_t dnNum = 0;
    uint16_t nValue = 0;            // loop2_size
    uint32_t dValue = 0;            // loop3_size
    uint16_t srcNzMatrixStride = 0; // loop4_src_stride
    uint16_t srcNStride = 0;        // loop3_src_stride
    uint64_t dstNStride = 0;        // loop2_dst_stride
    uint64_t dstDnMatrixStride = 0; // loop4_dst_stride
#ifdef CANN_ASC_ENABLE_SID
    uint8_t sid = 0;
#endif
};

struct LoopModeParams {
    __aicore__ LoopModeParams()
    {
        loop1Size = 0;
        loop2Size = 0;
        loop1SrcStride = 0;
        loop1DstStride = 0;
        loop2SrcStride = 0;
        loop2DstStride = 0;
    }

    __aicore__ LoopModeParams(
        const uint32_t loop1SizeIn, const uint32_t loop2SizeIn, const uint64_t loop1SrcStrideIn,
        const uint64_t loop1DstStrideIn, const uint64_t loop2SrcStrideIn, const uint64_t loop2DstStrideIn)
    {
        loop1Size = loop1SizeIn;
        loop2Size = loop2SizeIn;
        loop1SrcStride = loop1SrcStrideIn;
        loop1DstStride = loop1DstStrideIn;
        loop2SrcStride = loop2SrcStrideIn;
        loop2DstStride = loop2DstStrideIn;
    }

    uint32_t loop1Size = 0;
    uint32_t loop2Size = 0;
    uint64_t loop1SrcStride = 0;
    uint64_t loop1DstStride = 0;
    uint64_t loop2SrcStride = 0;
    uint64_t loop2DstStride = 0;
};

struct SliceInfo {
    __aicore__ SliceInfo() {}

    __aicore__ SliceInfo(
        const uint32_t startIndexIn, const uint32_t endIndexIn, const uint32_t strideIn, const uint32_t burstLenIn,
        const uint32_t shapeValueIn = 0)
        : startIndex(startIndexIn),
          endIndex(endIndexIn),
          stride(strideIn),
          burstLen(burstLenIn),
          shapeValue(shapeValueIn)
    {}

    uint32_t startIndex = 0;
    uint32_t endIndex = ONE_BLK_SIZE - 1;
    uint32_t stride = 0;
    uint32_t burstLen = ONE_BLK_SIZE;
    uint32_t shapeValue = 0;
};

struct CopyRepeatParams {
    __aicore__ CopyRepeatParams() {}

    __aicore__ CopyRepeatParams(
        const uint16_t dstStrideIn, const uint16_t srcStrideIn, uint16_t dstRepeatSizeIn, uint16_t srcRepeatSizeIn)
        : dstStride(dstStrideIn), srcStride(srcStrideIn), dstRepeatSize(dstRepeatSizeIn), srcRepeatSize(srcRepeatSizeIn)
    {}

    uint16_t dstStride = DEFAULT_DATA_COPY_STRIDE;
    uint16_t srcStride = DEFAULT_DATA_COPY_STRIDE;
    uint16_t dstRepeatSize = DEFAULT_REPEAT_STRIDE;
    uint16_t srcRepeatSize = DEFAULT_REPEAT_STRIDE;
};

} // namespace AscendC

#endif // ASCENDC_MODULE_STRUCT_DATA_COPY_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_STRUCT_DATA_COPY_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_STRUCT_DATA_COPY_H__
#endif
