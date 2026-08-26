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
 * \file kernel_struct_mm.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "kernel_struct_mm.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_STRUCT_MM_H__
#endif
#ifndef ASCENDC_MODULE_STRUCT_MM_H
#define ASCENDC_MODULE_STRUCT_MM_H

#include "../../impl/basic_api/kernel_macros.h"
#include "../../impl/basic_api/utils/kernel_utils_constants.h"

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <cstdint>
#include "stub_def.h"
#endif

#if (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5101) || (__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || \
    (__NPU_ARCH__ == 5163)
#define CANN_ASC_ENABLE_FIX_SHIFT
#endif

namespace AscendC {
// MM intr params
using LoadData2dParams = struct LoadData2DParams;
struct LoadData2DParams {
    __aicore__ LoadData2DParams() {}

    __aicore__ LoadData2DParams(
        const uint16_t startIndexIn, const uint8_t repeatTimesIn, const uint16_t srcStrideIn, const uint8_t sidIn,
        const uint16_t dstGapIn, const bool ifTransposeIn, const uint8_t addrModeIn)
        : startIndex(startIndexIn),
          repeatTimes(repeatTimesIn),
          srcStride(srcStrideIn),
          sid(sidIn),
          dstGap(dstGapIn),
          ifTranspose(ifTransposeIn),
          addrMode(addrModeIn)
    {}

    uint16_t startIndex = 0;
    uint16_t dstGap = 0;
    uint16_t srcStride = 0;
    bool ifTranspose = 0;
    uint8_t repeatTimes = 0;

    uint8_t sid = 0;
    uint8_t addrMode = 0;
};

struct LoadData2DParamsV2 {
    __aicore__ LoadData2DParamsV2() {}

    __aicore__ LoadData2DParamsV2(
        const uint32_t mStartPositionIn, const uint32_t kStartPositionIn, const uint16_t mStepIn,
        const uint16_t kStepIn, const int32_t srcStrideIn, const uint16_t dstStrideIn, const bool ifTransposeIn,
        const uint8_t sidIn)
        : mStartPosition(mStartPositionIn),
          kStartPosition(kStartPositionIn),
          mStep(mStepIn),
          kStep(kStepIn),
          srcStride(srcStrideIn),
          dstStride(dstStrideIn),
          ifTranspose(ifTransposeIn),
          sid(sidIn)
    {}

    uint32_t mStartPosition = 0;
    uint32_t kStartPosition = 0;
    uint16_t mStep = 0;
    uint16_t kStep = 0;
    int32_t srcStride = 0;
    uint16_t dstStride = 0;
    bool ifTranspose = false;
    uint8_t sid = 0;
};

struct LoadData2dTransposeParams {
    __aicore__ LoadData2dTransposeParams() {}

    __aicore__ LoadData2dTransposeParams(
        const uint16_t startIndexIn, const uint8_t repeatTimesIn, const uint16_t srcStrideIn, const uint16_t dstGapIn,
        const uint16_t dstfracGapIn, const uint8_t addrModeIn)
        : startIndex(startIndexIn),
          repeatTimes(repeatTimesIn),
          srcStride(srcStrideIn),
          dstGap(dstGapIn),
          dstFracGap(dstfracGapIn),
          addrMode(addrModeIn)
    {}

    __aicore__ LoadData2dTransposeParams(
        const uint16_t startIndexIn, const uint8_t repeatTimesIn, const uint16_t srcStrideIn, const uint16_t dstGapIn,
        const uint16_t dstfracGapIn)
        : startIndex(startIndexIn),
          repeatTimes(repeatTimesIn),
          srcStride(srcStrideIn),
          dstGap(dstGapIn),
          dstFracGap(dstfracGapIn)
    {}

    uint16_t startIndex = 0;
    uint8_t repeatTimes = 0;
    uint16_t srcStride = 0;
    uint16_t dstGap = 0;
    uint16_t dstFracGap = 0;
    uint8_t addrMode = 0;
};

struct Nd2NzParamsV2 {
    uint64_t lookupTable0 = 0;
    uint64_t lookupTable1 = 0;
};

struct LoadData2DMxParams {
    __aicore__ LoadData2DMxParams() {}

    __aicore__ LoadData2DMxParams(
        const uint16_t xStartPositionIn, const uint16_t yStartPositionIn, const uint8_t xStepIn, const uint8_t yStepIn,
        const uint16_t srcStrideIn, const uint16_t dstStrideIn)
    {
        xStartPosition = xStartPositionIn;
        yStartPosition = yStartPositionIn;
        xStep = xStepIn;
        yStep = yStepIn;
        srcStride = srcStrideIn;
        dstStride = dstStrideIn;
    }

    uint16_t xStartPosition = 0;
    uint16_t yStartPosition = 0;
    uint8_t xStep = 0;
    uint8_t yStep = 0;
    uint16_t srcStride = 0;
    uint16_t dstStride = 0;
};

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
template <typename TYPE>
struct LoadData3DParamsV1 {
    using T = typename GetPadValueType<TYPE>::Type;
#else
template <typename T>
struct LoadData3DParamsV1 {
#endif
    __aicore__ LoadData3DParamsV1()
    {
        for (int32_t i = 0; i < PAD_SIZE; ++i) {
            padList[i] = 0;
        }
    }

    __aicore__ LoadData3DParamsV1(
        const uint8_t padListIn[PAD_SIZE], const uint16_t l1HIn, const uint16_t l1WIn, const uint16_t c1IndexIn,
        const uint8_t fetchFilterWIn, const uint8_t fetchFilterHIn, const int16_t leftTopWIn, const int16_t leftTopHIn,
        const uint8_t strideWIn, const uint8_t strideHIn, const uint8_t filterWIn, const uint8_t filterHIn,
        const uint8_t dilationFilterWIn, const uint8_t dilationFilterHIn, const uint8_t jumpStrideIn,
        const uint8_t repeatModeIn, const uint8_t repeatTimeIn, const uint8_t cSizeIn, const T padValueIn)
        : l1H(l1HIn),
          l1W(l1WIn),
          c1Index(c1IndexIn),
          fetchFilterW(fetchFilterWIn),
          fetchFilterH(fetchFilterHIn),
          leftTopW(leftTopWIn),
          leftTopH(leftTopHIn),
          strideW(strideWIn),
          strideH(strideHIn),
          filterW(filterWIn),
          filterH(filterHIn),
          dilationFilterW(dilationFilterWIn),
          dilationFilterH(dilationFilterHIn),
          jumpStride(jumpStrideIn),
          repeatMode(repeatModeIn),
          repeatTime(repeatTimeIn),
          cSize(cSizeIn),
          padValue(padValueIn)
    {
        for (int32_t i = 0; i < PAD_SIZE; ++i) {
            padList[i] = padListIn[i];
        }
    }

    uint8_t padList[PAD_SIZE] = {0};
    uint8_t strideW = 0;
    uint8_t strideH = 0;
    uint8_t filterW = 0;
    uint8_t filterH = 0;
    uint8_t dilationFilterW = 0;
    uint8_t dilationFilterH = 0;
    uint8_t jumpStride = 0;
    uint8_t repeatMode = 0;
    uint8_t repeatTime = 0;
    uint8_t cSize = 0;
    T padValue = 0;
    uint8_t fetchFilterW = 0;
    uint8_t fetchFilterH = 0;
    uint16_t l1H = 0;
    uint16_t l1W = 0;
    uint16_t c1Index = 0;
    int16_t leftTopW = 0;
    int16_t leftTopH = 0;
};

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
template <typename TYPE>
struct LoadData3DParamsV2 {
    using T = typename GetPadValueType<TYPE>::Type;
#else
template <typename T>
struct LoadData3DParamsV2 {
#endif
    __aicore__ LoadData3DParamsV2()
    {
        for (int32_t i = 0; i < PAD_SIZE; ++i) {
            padList[i] = 0;
        }
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
        enDualSrc = BM_DISABLE;
#endif
    }

    __aicore__ LoadData3DParamsV2(
        const uint8_t padListIn[PAD_SIZE], const uint16_t l1HIn, const uint16_t l1WIn, const uint16_t channelSizeIn,
        const uint16_t kExtensionIn, const uint16_t mExtensionIn, const uint16_t kStartPtIn, const uint16_t mStartPtIn,
        const uint8_t strideWIn, const uint8_t strideHIn, const uint8_t filterWIn, const uint8_t filterHIn,
        const uint8_t dilationFilterWIn, const uint8_t dilationFilterHIn, const bool enTransposeIn,
        const bool enSmallKIn, const T padValueIn)
        : l1H(l1HIn),
          l1W(l1WIn),
          channelSize(channelSizeIn),
          kExtension(kExtensionIn),
          mExtension(mExtensionIn),
          kStartPt(kStartPtIn),
          mStartPt(mStartPtIn),
          strideW(strideWIn),
          strideH(strideHIn),
          filterW(filterWIn),
          filterH(filterHIn),
          dilationFilterW(dilationFilterWIn),
          dilationFilterH(dilationFilterHIn),
          enTranspose(enTransposeIn),
          enSmallK(enSmallKIn),
          padValue(padValueIn)
    {
        for (int32_t i = 0; i < PAD_SIZE; ++i) {
            padList[i] = padListIn[i];
        }
    }

    __aicore__ LoadData3DParamsV2(
        const uint8_t padListIn[PAD_SIZE], const uint16_t l1HIn, const uint16_t l1WIn, const uint16_t channelSizeIn,
        const uint16_t kExtensionIn, const uint16_t mExtensionIn, const uint16_t kStartPtIn, const uint16_t mStartPtIn,
        const uint8_t strideWIn, const uint8_t strideHIn, const uint8_t filterWIn, const uint8_t filterHIn,
        const uint8_t dilationFilterWIn, const uint8_t dilationFilterHIn, const bool enTransposeIn,
        const bool enSmallKIn, const T padValueIn, const bool filterSizeWIn, const bool filterSizeHIn,
        const bool fMatrixCtrlIn)
        : l1H(l1HIn),
          l1W(l1WIn),
          channelSize(channelSizeIn),
          kExtension(kExtensionIn),
          mExtension(mExtensionIn),
          kStartPt(kStartPtIn),
          mStartPt(mStartPtIn),
          strideW(strideWIn),
          strideH(strideHIn),
          filterW(filterWIn),
          filterH(filterHIn),
          dilationFilterW(dilationFilterWIn),
          dilationFilterH(dilationFilterHIn),
          enTranspose(enTransposeIn),
          enSmallK(enSmallKIn),
          padValue(padValueIn),
          filterSizeW(filterSizeWIn),
          filterSizeH(filterSizeHIn),
          fMatrixCtrl(fMatrixCtrlIn)
    {
        for (int32_t i = 0; i < PAD_SIZE; ++i) {
            padList[i] = padListIn[i];
        }
    }

    uint8_t padList[PAD_SIZE] = {0};
    uint16_t l1H = 0;
    uint16_t l1W = 0;
    uint16_t channelSize = 0;
    uint16_t kExtension = 0;
    uint16_t mExtension = 0;
    uint16_t kStartPt = 0;
    uint16_t mStartPt = 0;

    uint8_t strideW = 1;
    uint8_t strideH = 1;
    uint8_t filterW = 1;
    uint8_t filterH = 1;
    uint8_t dilationFilterW = 1;
    uint8_t dilationFilterH = 1;
    bool enTranspose = false;
    bool enSmallK = false;
    T padValue = 0;
    bool filterSizeW = false;
    bool filterSizeH = false;
    bool fMatrixCtrl = false;
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
    bm_t enDualSrc = BM_DISABLE;
#endif
};
struct LoadData3DParamsV2Pro {
    __aicore__ LoadData3DParamsV2Pro()
    {
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
        enDualSrc = BM_DISABLE;
#endif
    }

    __aicore__ LoadData3DParamsV2Pro(
        const uint16_t channelSizeIn, const bool enTransposeIn, const bool enSmallKIn, const bool filterSizeWIn,
        const bool filterSizeHIn, const bool fMatrixCtrlIn, const uint64_t extConfigIn, const uint64_t filterConfigIn)
        : channelSize(channelSizeIn),
          enTranspose(enTransposeIn),
          enSmallK(enSmallKIn),
          filterSizeW(filterSizeWIn),
          filterSizeH(filterSizeHIn),
          fMatrixCtrl(fMatrixCtrlIn),
          extConfig(extConfigIn),
          filterConfig(filterConfigIn)
    {}

    uint16_t channelSize = 0;
    bool enTranspose = false;
    bool enSmallK = false;
    bool filterSizeW = false;
    bool filterSizeH = false;
    bool fMatrixCtrl = false;
    uint64_t extConfig = 0;
    uint64_t filterConfig = 0X10101010101;
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
    bm_t enDualSrc = BM_DISABLE;
#endif
};

struct LoadData2dTransposeParamsV2 {
    __aicore__ LoadData2dTransposeParamsV2() {}

    __aicore__ LoadData2dTransposeParamsV2(
        const uint16_t startIndexIn, const uint8_t repeatTimesIn, const uint16_t srcStrideIn, const uint16_t dstGapIn,
        const uint16_t dstFracGapIn, const uint16_t srcFracGapIn)
        : startIndex(startIndexIn),
          repeatTimes(repeatTimesIn),
          srcStride(srcStrideIn),
          dstGap(dstGapIn),
          dstFracGap(dstFracGapIn),
          srcFracGap(srcFracGapIn)
    {}

    __aicore__ LoadData2dTransposeParamsV2(
        const uint16_t startIndexIn, const uint8_t repeatTimesIn, const uint16_t srcStrideIn, const uint16_t dstGapIn,
        const uint16_t dstFracGapIn, const uint16_t srcFracGapIn, const uint8_t addrModeIn)
        : startIndex(startIndexIn),
          repeatTimes(repeatTimesIn),
          srcStride(srcStrideIn),
          dstGap(dstGapIn),
          dstFracGap(dstFracGapIn),
          srcFracGap(srcFracGapIn),
          addrMode(addrModeIn)
    {}

    uint16_t startIndex = 0;
    uint8_t repeatTimes = 0;
    uint16_t srcStride = 0;
    uint16_t dstGap = 0;
    uint16_t dstFracGap = 0;
    uint16_t srcFracGap = 0;
    uint8_t addrMode = 0;
};

struct MmadParams {
    __aicore__ MmadParams() {}

    __aicore__ MmadParams(
        const uint16_t mIn, const uint16_t nIn, const uint16_t kIn, const bool isBiasIn, const int32_t fmOffsetIn,
        const bool enSsparseIn, const bool enWinogradAIn, const bool enWinogradBIn)
        : m(mIn),
          n(nIn),
          k(kIn),
          isBias(isBiasIn),
          fmOffset(fmOffsetIn),
          enSsparse(enSsparseIn),
          enWinogradA(enWinogradAIn),
          enWinogradB(enWinogradBIn)
    {}

    __aicore__ MmadParams(
        const uint16_t mIn, const uint16_t nIn, const uint16_t kIn, const uint8_t unitFlagIn,
        const bool cmatrixSourceIn, const bool cmatrixInitValIn)
        : m(mIn), n(nIn), k(kIn), unitFlag(unitFlagIn), cmatrixSource(cmatrixSourceIn), cmatrixInitVal(cmatrixInitValIn)
    {}

    uint16_t m = 0;
    uint16_t n = 0;
    uint16_t k = 0;
    // Indicates whether to accumulate the initial matrix, 0: matrix multiplication, 1: matrix multiplication and
    // addition
    bool isBias = false;
    // Left matrix offset
    int32_t fmOffset = 0;
    // Enable the structured sparse feature, default value is false
    bool enSsparse = false;
    // Indicates whether matrix a is generated by winograd_feature_map_transform, default value is false;
    bool enWinogradA = false;
    // Indicates whether matrix b is generated by winograd_feature_map_transform, default value is false;
    bool enWinogradB = false;
    uint8_t unitFlag = 0;
    // also mean gemvCtrl in 3101 and 5102
    bool kDirectionAlign = false;
    // Indicates the C matrix source, 1: the C matrix is in bias table buffer, 0: the C matrix is in L0C
    bool cmatrixSource = false;
    // Indicates the initial matrix, 1: the number in C matrix is 0, 0：use the real number in C matrix
    bool cmatrixInitVal = true;
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    bool disableGemv = false;
#endif
#ifdef CANN_ASC_ENABLE_FIX_SHIFT
    uint8_t fixShiftVal = 0;
#endif
};

template <typename T>
struct InitConstValueParams {
    __aicore__ InitConstValueParams() {}

    __aicore__ InitConstValueParams(
        const uint16_t repeatTimesIn, const uint16_t blockNumIn, const uint16_t dstGapIn, const T initValueIn)
        : repeatTimes(repeatTimesIn), blockNum(blockNumIn), dstGap(dstGapIn), initValue(initValueIn)
    {}

    __aicore__ InitConstValueParams(const uint16_t repeatTimesIn, const T initValueIn)
        : repeatTimes(repeatTimesIn), initValue(initValueIn)
    {}

    uint16_t repeatTimes = 0;
    uint16_t blockNum = 0;
    uint16_t dstGap = 0;
    T initValue = 0;
};

enum class FmatrixMode : uint8_t {
    FMATRIX_LEFT = 0,
    FMATRIX_RIGHT = 1,
};

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 5102))
struct LoadDataRepeatParam {
    __aicore__ LoadDataRepeatParam() {}

    __aicore__ LoadDataRepeatParam(
        const uint16_t repeatStrideIn, const uint8_t repeatTimeIn, const uint8_t repeatModeIn,
        const uint16_t dstStrideIn)
        : repeatStride(repeatStrideIn), repeatTime(repeatTimeIn), repeatMode(repeatModeIn), dstStride(dstStrideIn)
    {}

    uint16_t repeatStride = 0;
    uint8_t repeatTime = 1;
    uint8_t repeatMode = 0;
    uint16_t dstStride = 0;
};
#else
struct LoadDataRepeatParam {
    __aicore__ LoadDataRepeatParam() {}

    __aicore__ LoadDataRepeatParam(
        const uint16_t repeatStrideIn, const uint8_t repeatTimeIn, const uint8_t repeatModeIn)
        : repeatStride(repeatStrideIn), repeatTime(repeatTimeIn), repeatMode(repeatModeIn)
    {}

    uint16_t repeatStride = 0;
    uint8_t repeatTime = 1;
    uint8_t repeatMode = 0;
    uint8_t reserved = 0;
};
#endif // Turing versions

struct LoadDataRepeatParamWithStride {
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    __aicore__ LoadDataRepeatParamWithStride() {}

    __aicore__ LoadDataRepeatParamWithStride(
        const uint16_t repeatStrideIn, const uint8_t repeatTimeIn, const uint8_t repeatModeIn,
        const uint16_t dstStrideIn)
        : repeatStride(repeatStrideIn), repeatTime(repeatTimeIn), repeatMode(repeatModeIn), dstStride(dstStrideIn)
    {}

    uint16_t repeatStride = 0;
    uint8_t repeatTime = 1;
    uint8_t repeatMode = 0;
    uint16_t dstStride = 0;
#endif
};

struct LoadImageToLocalParams {
    __aicore__ LoadImageToLocalParams() {}

    __aicore__ LoadImageToLocalParams(
        const uint16_t horizSizeIn, const uint16_t vertSizeIn, const uint16_t horizStartPosIn,
        const uint16_t vertStartPosIn, const uint16_t srcHorizSizeIn, const uint8_t topPadSizeIn,
        const uint8_t botPadSizeIn, const uint16_t leftPadSizeIn, const uint16_t rightPadSizeIn)
        : horizSize(horizSizeIn),
          vertSize(vertSizeIn),
          horizStartPos(horizStartPosIn),
          vertStartPos(vertStartPosIn),
          srcHorizSize(srcHorizSizeIn),
          topPadSize(topPadSizeIn),
          botPadSize(botPadSizeIn),
          leftPadSize(leftPadSizeIn),
          rightPadSize(rightPadSizeIn)
    {}

    uint16_t horizSize = 0;
    uint16_t vertSize = 0;
    uint16_t horizStartPos = 0;
    uint16_t vertStartPos = 0;
    uint16_t srcHorizSize = 0;
    uint8_t topPadSize = 0;
    uint8_t botPadSize = 0;
    uint16_t leftPadSize = 0;
    uint16_t rightPadSize = 0;
    uint8_t sid = 0;
};

struct CheckLocalMemoryIAParam {
    __aicore__ CheckLocalMemoryIAParam() {}

    __aicore__ CheckLocalMemoryIAParam(
        const uint8_t enableBitIn, const uint32_t startAddrIn, const uint32_t endAddrIn, const bool isScalarReadIn,
        const bool isScalarWriteIn, const bool isVectorReadIn, const bool isVectorWriteIn, const bool isMteReadIn,
        const bool isMteWriteIn, const bool isEnableIn)
        : enableBit(enableBitIn),
          startAddr(startAddrIn),
          endAddr(endAddrIn),
          isScalarRead(isScalarReadIn),
          isScalarWrite(isScalarWriteIn),
          isVectorRead(isVectorReadIn),
          isVectorWrite(isVectorWriteIn),
          isMteRead(isMteReadIn),
          isMteWrite(isMteWriteIn),
          isEnable(isEnableIn)
    {}

    uint8_t enableBit = 0;
    uint32_t startAddr = 0;
    uint32_t endAddr = 0;
    bool isScalarRead = false;
    bool isScalarWrite = false;
    bool isVectorRead = false;
    bool isVectorWrite = false;
    bool isMteRead = false;
    bool isMteWrite = false;
    bool isEnable = false;
    uint32_t reserved = 0;
};
} // namespace AscendC

#endif // ASCENDC_MODULE_STRUCT_MM_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_STRUCT_MM_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_STRUCT_MM_H__
#endif
