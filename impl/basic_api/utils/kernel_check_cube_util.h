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
 * \file kernel_check_cube_util.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/utils/kernel_check_cube_util.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_CHECK_CUBE_UTIL_H__
#endif
#ifndef ASCENDC_CHECK_CUBE_UTIL_H
#define ASCENDC_CHECK_CUBE_UTIL_H
#if ASCENDC_CPU_DEBUG
#include <string>
#include "../kernel_utils.h"
namespace AscendC {
namespace check {
struct MmadApiParams {
    MmadApiParams() {}
    MmadApiParams(
        uint64_t dstAddrIn, uint64_t src0AddrIn, uint64_t src1AddrIn, uint32_t dstDtypeBytesIn,
        uint32_t src0DtypeBytesIn, uint32_t src1DtypeBytesIn, uint64_t dstSizeIn, uint64_t src0SizeIn,
        uint64_t src1SizeIn, uint8_t dstPosIn, uint8_t src0PosIn, uint8_t src1PosIn, uint16_t mIn, uint16_t nIn,
        uint16_t kIn, bool isBiasIn, int32_t fmOffsetIn, bool enSsparseIn, bool enWinogradAIn, bool enWinogradBIn)
        : dstAddr(dstAddrIn),
          src0Addr(src0AddrIn),
          src1Addr(src1AddrIn),
          dstDtypeBytes(dstDtypeBytesIn),
          src0DtypeBytes(src0DtypeBytesIn),
          src1DtypeBytes(src1DtypeBytesIn),
          dstSize(dstSizeIn),
          src0Size(src0SizeIn),
          src1Size(src1SizeIn),
          dstLogicPos(dstPosIn),
          src0LogicPos(src0PosIn),
          src1LogicPos(src1PosIn),
          m(mIn),
          n(nIn),
          k(kIn),
          isBias(isBiasIn),
          fmOffset(fmOffsetIn),
          enSsparse(enSsparseIn),
          enWinogradA(enWinogradAIn),
          enWinogradB(enWinogradBIn)
    {
        dstPos = static_cast<uint8_t>(GetPhyType(static_cast<TPosition>(dstPosIn)));
        src0Pos = static_cast<uint8_t>(GetPhyType(static_cast<TPosition>(src0PosIn)));
        src1Pos = static_cast<uint8_t>(GetPhyType(static_cast<TPosition>(src1PosIn)));
    }

    MmadApiParams(
        uint64_t dstAddrIn, uint64_t src0AddrIn, uint64_t src1AddrIn, uint64_t src2AddrIn, uint32_t dstDtypeBytesIn,
        uint32_t src0DtypeBytesIn, uint32_t src1DtypeBytesIn, uint32_t src2DtypeBytesIn, uint64_t dstSizeIn,
        uint64_t src0SizeIn, uint64_t src1SizeIn, uint64_t src2SizeIn, uint8_t dstPosIn, uint8_t src0PosIn,
        uint8_t src1PosIn, uint8_t src2PosIn, uint16_t mIn, uint16_t nIn, uint16_t kIn, bool isBiasIn,
        int32_t fmOffsetIn, bool enSsparseIn, bool enWinogradAIn, bool enWinogradBIn)
        : dstAddr(dstAddrIn),
          src0Addr(src0AddrIn),
          src1Addr(src1AddrIn),
          src2Addr(src2AddrIn),
          dstDtypeBytes(dstDtypeBytesIn),
          src0DtypeBytes(src0DtypeBytesIn),
          src1DtypeBytes(src1DtypeBytesIn),
          src2DtypeBytes(src2DtypeBytesIn),
          dstSize(dstSizeIn),
          src0Size(src0SizeIn),
          src1Size(src1SizeIn),
          src2Size(src2SizeIn),
          dstLogicPos(dstPosIn),
          src0LogicPos(src0PosIn),
          src1LogicPos(src1PosIn),
          src2LogicPos(src2PosIn),
          m(mIn),
          n(nIn),
          k(kIn),
          isBias(isBiasIn),
          fmOffset(fmOffsetIn),
          enSsparse(enSsparseIn),
          enWinogradA(enWinogradAIn),
          enWinogradB(enWinogradBIn)
    {
        dstPos = static_cast<uint8_t>(GetPhyType(static_cast<TPosition>(dstPosIn)));
        src0Pos = static_cast<uint8_t>(GetPhyType(static_cast<TPosition>(src0PosIn)));
        src1Pos = static_cast<uint8_t>(GetPhyType(static_cast<TPosition>(src1PosIn)));
        src2Pos = static_cast<uint8_t>(GetPhyType(static_cast<TPosition>(src2PosIn)));
    }

    uint64_t dstAddr = 0;
    uint64_t src0Addr = 0;
    uint64_t src1Addr = 0;
    uint64_t src2Addr = 0;
    uint32_t dstDtypeBytes = 0;
    uint32_t src0DtypeBytes = 0;
    uint32_t src1DtypeBytes = 0;
    uint32_t src2DtypeBytes = 0;
    uint64_t dstSize = 0;
    uint64_t src0Size = 0;
    uint64_t src1Size = 0;
    uint64_t src2Size = 0;
    uint8_t dstLogicPos = 0;
    uint8_t src0LogicPos = 0;
    uint8_t src1LogicPos = 0;
    uint8_t src2LogicPos = 0;
    uint8_t dstPos = 0;
    uint8_t src0Pos = 0;
    uint8_t src1Pos = 0;
    uint8_t src2Pos = 0;
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
};

struct CubeCreateCxMatrixApiParams {
    CubeCreateCxMatrixApiParams() {}
    CubeCreateCxMatrixApiParams(
        uint64_t dstAddrIn, uint32_t dstDtypeBytesIn, uint64_t dstSizeIn, uint8_t dstPosIn, int64_t repeatIn)
    {
        dstAddr = dstAddrIn;
        dstDtypeBytes = dstDtypeBytesIn;
        dstSize = dstSizeIn;
        dstLogicPos = dstPosIn;
        dstPos = static_cast<uint8_t>(GetPhyType(static_cast<TPosition>(dstPosIn)));
        repeat = repeatIn;
    }

    uint64_t dstAddr = 0;
    uint32_t dstDtypeBytes = 0;
    uint64_t dstSize = 0;
    uint8_t dstLogicPos = 0;
    uint8_t dstPos = 0;
    int32_t repeat = 0;
};

struct CubeInitConstValueApiParams {
    CubeInitConstValueApiParams() {}
    CubeInitConstValueApiParams(
        uint64_t dstAddrIn, uint16_t repeatIn, uint16_t blkNumIn, uint16_t dstGapIn, uint32_t dstDtypeBytesIn,
        uint64_t dstSizeIn, uint8_t dstPosIn)
    {
        dstAddr = dstAddrIn;
        repeatTimes = repeatIn;
        blockNum = blkNumIn;
        dstGap = dstGapIn;
        dstDtypeBytes = dstDtypeBytesIn;
        dstSize = dstSizeIn;
        dstLogicPos = dstPosIn;
        dstPos = static_cast<uint8_t>(GetPhyType(static_cast<TPosition>(dstPosIn)));
    }

    uint64_t dstAddr = 0;
    uint16_t repeatTimes = 0;
    uint16_t blockNum = 0;
    uint16_t dstGap = 0;
    uint32_t dstDtypeBytes = 0;
    uint64_t dstSize = 0;
    uint8_t dstLogicPos = 0;
    uint8_t dstPos = 0;
};

struct LoadData2dApiParams {
    LoadData2dApiParams() {}
    LoadData2dApiParams(
        uint64_t dstAddrIn, uint64_t srcAddrIn, uint16_t startIndexIn, uint8_t repeatTimesIn, uint16_t srcStrideIn,
        uint8_t sidIn, uint16_t dstGapIn, bool ifTransposeIn, uint8_t addrModeIn, uint32_t dstDtypeBytesIn,
        uint32_t srcDtypeBytesIn, uint64_t dstSizeIn, uint64_t srcSizeIn, uint8_t dstPosIn, uint8_t srcPosIn)
    {
        dstAddr = dstAddrIn;
        srcAddr = srcAddrIn;
        startIndex = startIndexIn;
        repeatTimes = repeatTimesIn;
        srcStride = srcStrideIn;
        sid = sidIn;
        dstGap = dstGapIn;
        ifTranspose = ifTransposeIn;
        addrMode = addrModeIn;
        dstDtypeBytes = dstDtypeBytesIn;
        srcDtypeBytes = srcDtypeBytesIn;
        dstSize = dstSizeIn;
        srcSize = srcSizeIn;
        dstLogicPos = dstPosIn;
        srcLogicPos = srcPosIn;
        dstPos = static_cast<uint8_t>(GetPhyType(static_cast<TPosition>(dstPosIn)));
        srcPos = static_cast<uint8_t>(GetPhyType(static_cast<TPosition>(srcPosIn)));
    }
    uint64_t dstAddr = 0;
    uint64_t srcAddr = 0;
    uint16_t startIndex = 0;
    uint8_t repeatTimes = 0;
    uint16_t srcStride = 0;
    uint8_t sid = 0;
    uint16_t dstGap = 0;
    bool ifTranspose = false;
    uint8_t addrMode = 0;
    uint32_t dstDtypeBytes = 0;
    uint32_t srcDtypeBytes = 0;
    uint64_t dstSize = 0;
    uint64_t srcSize = 0;
    uint8_t dstLogicPos = 0;
    uint8_t srcLogicPos = 0;
    uint8_t dstPos = 0;
    uint8_t srcPos = 0;
};

struct LoadData2dv2ApiParams {
    LoadData2dv2ApiParams() {}
    LoadData2dv2ApiParams(
        uint64_t dstAddrIn, uint64_t srcAddrIn, uint32_t mStartPositionIn, uint32_t kStartPositionIn, uint16_t mStepIn,
        uint16_t kStepIn, int32_t srcStrideIn, uint16_t dstStrideIn, bool ifTransposeIn, uint8_t sidIn,
        uint32_t dstDtypeBytesIn, uint32_t srcDtypeBytesIn, uint64_t dstSizeIn, uint64_t srcSizeIn, uint8_t dstPosIn,
        uint8_t srcPosIn)
    {
        dstAddr = dstAddrIn;
        srcAddr = srcAddrIn;
        mStartPosition = mStartPositionIn;
        kStartPosition = kStartPositionIn;
        mStep = mStepIn;
        kStep = kStepIn;
        srcStride = srcStrideIn;
        dstStride = dstStrideIn;
        ifTranspose = ifTransposeIn;
        sid = sidIn;
        dstDtypeBytes = dstDtypeBytesIn;
        srcDtypeBytes = srcDtypeBytesIn;
        dstSize = dstSizeIn;
        srcSize = srcSizeIn;
        dstLogicPos = dstPosIn;
        srcLogicPos = srcPosIn;
        dstPos = static_cast<uint8_t>(GetPhyType(static_cast<TPosition>(dstPosIn)));
        srcPos = static_cast<uint8_t>(GetPhyType(static_cast<TPosition>(srcPosIn)));
    }
    uint64_t dstAddr = 0;
    uint64_t srcAddr = 0;
    uint32_t mStartPosition = 0;
    uint32_t kStartPosition = 0;
    uint16_t mStep = 0;
    uint16_t kStep = 0;
    int32_t srcStride = 0;
    uint16_t dstStride = 0;
    bool ifTranspose = false;
    uint8_t sid = 0;
    uint32_t dstDtypeBytes = 0;
    uint32_t srcDtypeBytes = 0;
    uint64_t dstSize = 0;
    uint64_t srcSize = 0;
    uint8_t dstLogicPos = 0;
    uint8_t srcLogicPos = 0;
    uint8_t dstPos = 0;
    uint8_t srcPos = 0;
};

struct LoadData3dv1ApiParams {
    LoadData3dv1ApiParams() {}
    LoadData3dv1ApiParams(
        uint64_t dstAddrIn, uint64_t srcAddrIn, const uint8_t padListIn[4], uint16_t l1HIn, uint16_t l1WIn,
        uint16_t c1IndexIn, uint8_t fetchFilterWIn, uint8_t fetchFilterHIn, uint16_t leftTopWIn, uint16_t leftTopHIn,
        uint8_t strideWIn, uint8_t strideHIn, uint8_t filterWIn, uint8_t filterHIn, uint8_t dilationFilterWIn,
        uint8_t dilationFilterHIn, uint8_t jumpStrideIn, uint8_t repeatModeIn, uint8_t repeatTimeIn, uint8_t cSizeIn,
        uint32_t dstDtypeBytesIn, uint32_t srcDtypeBytesIn, uint64_t dstSizeIn, uint64_t srcSizeIn, uint8_t dstPosIn,
        uint8_t srcPosIn)
    {
        dstAddr = dstAddrIn;
        srcAddr = srcAddrIn;
        for (size_t i = 0; i < PAD_SIZE; ++i) {
            padList[i] = padListIn[i];
        }
        l1H = l1HIn;
        l1W = l1WIn;
        c1Index = c1IndexIn;
        fetchFilterW = fetchFilterWIn;
        fetchFilterH = fetchFilterHIn;
        leftTopW = leftTopWIn;
        leftTopH = leftTopHIn;
        strideW = strideWIn;
        strideH = strideHIn;
        filterW = filterWIn;
        filterH = filterHIn;
        dilationFilterW = dilationFilterWIn;
        dilationFilterH = dilationFilterHIn;
        jumpStride = jumpStrideIn;
        repeatMode = repeatModeIn;
        repeatTime = repeatTimeIn;
        cSize = cSizeIn;
        dstDtypeBytes = dstDtypeBytesIn;
        srcDtypeBytes = srcDtypeBytesIn;
        dstSize = dstSizeIn;
        srcSize = srcSizeIn;
        dstLogicPos = dstPosIn;
        srcLogicPos = srcPosIn;
        dstPos = static_cast<uint8_t>(GetPhyType(static_cast<TPosition>(dstPosIn)));
        srcPos = static_cast<uint8_t>(GetPhyType(static_cast<TPosition>(srcPosIn)));
    }
    uint64_t dstAddr = 0;
    uint64_t srcAddr = 0;
    uint8_t padList[4];
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
    uint8_t fetchFilterW = 0;
    uint8_t fetchFilterH = 0;
    uint16_t l1H = 0;
    uint16_t l1W = 0;
    uint16_t c1Index = 0;
    int16_t leftTopW = 0;
    int16_t leftTopH = 0;
    uint32_t dstDtypeBytes = 0;
    uint32_t srcDtypeBytes = 0;
    uint64_t dstSize = 0;
    uint64_t srcSize = 0;
    uint8_t dstLogicPos = 0;
    uint8_t srcLogicPos = 0;
    uint8_t dstPos = 0;
    uint8_t srcPos = 0;
};

template <typename Params>
inline void InitLoadData3dv2Common(
    Params& params, uint16_t channelSizeIn, uint16_t kExtensionIn, uint16_t mExtensionIn, uint16_t kStartPtIn,
    uint16_t mStartPtIn, uint8_t strideWIn, uint8_t strideHIn, uint8_t filterWIn, uint8_t filterHIn,
    uint8_t dilationFilterWIn, uint8_t dilationFilterHIn, bool enTransposeIn, bool enSmallKIn, uint32_t dstDtypeBytesIn,
    uint32_t srcDtypeBytesIn, uint64_t dstSizeIn, uint64_t srcSizeIn, uint8_t dstPosIn, uint8_t srcPosIn)
{
    params.channelSize = channelSizeIn;
    params.kExtension = kExtensionIn;
    params.mExtension = mExtensionIn;
    params.kStartPt = kStartPtIn;
    params.mStartPt = mStartPtIn;
    params.strideW = strideWIn;
    params.strideH = strideHIn;
    params.filterW = filterWIn;
    params.filterH = filterHIn;
    params.dilationFilterW = dilationFilterWIn;
    params.dilationFilterH = dilationFilterHIn;
    params.enTranspose = enTransposeIn;
    params.enSmallK = enSmallKIn;
    params.filterSizeW = false;
    params.filterSizeH = false;
    params.fMatrixCtrl = false;
    params.dstDtypeBytes = dstDtypeBytesIn;
    params.srcDtypeBytes = srcDtypeBytesIn;
    params.dstSize = dstSizeIn;
    params.srcSize = srcSizeIn;
    params.dstLogicPos = dstPosIn;
    params.srcLogicPos = srcPosIn;
    params.dstPos = static_cast<uint8_t>(GetPhyType(static_cast<TPosition>(dstPosIn)));
    params.srcPos = static_cast<uint8_t>(GetPhyType(static_cast<TPosition>(srcPosIn)));
}

struct LoadData3dv2ApiParams {
    LoadData3dv2ApiParams() {}
    LoadData3dv2ApiParams(
        uint64_t dstAddrIn, uint64_t srcAddrIn, const uint8_t padListIn[4], uint16_t l1HIn, uint16_t l1WIn,
        uint16_t channelSizeIn, uint16_t kExtensionIn, uint16_t mExtensionIn, uint16_t kStartPtIn, uint16_t mStartPtIn,
        uint8_t strideWIn, uint8_t strideHIn, uint8_t filterWIn, uint8_t filterHIn, uint8_t dilationFilterWIn,
        uint8_t dilationFilterHIn, bool enTransposeIn, bool enSmallKIn, uint32_t dstDtypeBytesIn,
        uint32_t srcDtypeBytesIn, uint64_t dstSizeIn, uint64_t srcSizeIn, uint8_t dstPosIn, uint8_t srcPosIn)
    {
        dstAddr = dstAddrIn;
        srcAddr = srcAddrIn;
        for (size_t i = 0; i < PAD_SIZE; ++i) {
            padList[i] = padListIn[i];
        }
        l1H = l1HIn;
        l1W = l1WIn;
        InitLoadData3dv2Common(
            *this, channelSizeIn, kExtensionIn, mExtensionIn, kStartPtIn, mStartPtIn, strideWIn, strideHIn, filterWIn,
            filterHIn, dilationFilterWIn, dilationFilterHIn, enTransposeIn, enSmallKIn, dstDtypeBytesIn,
            srcDtypeBytesIn, dstSizeIn, srcSizeIn, dstPosIn, srcPosIn);
    }
    uint64_t dstAddr = 0;
    uint64_t srcAddr = 0;
    uint8_t padList[4];
    uint16_t l1H = 0;
    uint16_t l1W = 0;
    uint16_t channelSize = 0;
    uint16_t kExtension = 0;
    uint16_t mExtension = 0;
    uint16_t kStartPt = 0;
    uint16_t mStartPt = 0;
    uint8_t strideW = 0;
    uint8_t strideH = 0;
    uint8_t filterW = 0;
    uint8_t filterH = 0;
    uint8_t dilationFilterW = 0;
    uint8_t dilationFilterH = 0;
    bool enTranspose = false;
    bool enSmallK = false;
    bool filterSizeW = false;
    bool filterSizeH = false;
    bool fMatrixCtrl = false;
    uint32_t dstDtypeBytes = 0;
    uint32_t srcDtypeBytes = 0;
    uint64_t dstSize = 0;
    uint64_t srcSize = 0;
    uint8_t dstLogicPos = 0;
    uint8_t srcLogicPos = 0;
    uint8_t dstPos = 0;
    uint8_t srcPos = 0;
};

struct LoadData3dv2ProApiParams {
    LoadData3dv2ProApiParams() {}
    LoadData3dv2ProApiParams(
        uint64_t dstAddrIn, uint64_t srcAddrIn, uint16_t channelSizeIn, uint16_t kExtensionIn, uint16_t mExtensionIn,
        uint16_t kStartPtIn, uint16_t mStartPtIn, uint8_t strideWIn, uint8_t strideHIn, uint8_t filterWIn,
        uint8_t filterHIn, uint8_t dilationFilterWIn, uint8_t dilationFilterHIn, bool enTransposeIn, bool enSmallKIn,
        uint32_t dstDtypeBytesIn, uint32_t srcDtypeBytesIn, uint64_t dstSizeIn, uint64_t srcSizeIn, uint8_t dstPosIn,
        uint8_t srcPosIn)
    {
        dstAddr = dstAddrIn;
        srcAddr = srcAddrIn;
        InitLoadData3dv2Common(
            *this, channelSizeIn, kExtensionIn, mExtensionIn, kStartPtIn, mStartPtIn, strideWIn, strideHIn, filterWIn,
            filterHIn, dilationFilterWIn, dilationFilterHIn, enTransposeIn, enSmallKIn, dstDtypeBytesIn,
            srcDtypeBytesIn, dstSizeIn, srcSizeIn, dstPosIn, srcPosIn);
    }
    uint64_t dstAddr = 0;
    uint64_t srcAddr = 0;
    uint16_t channelSize = 0;
    uint16_t kExtension = 0;
    uint16_t mExtension = 0;
    uint16_t kStartPt = 0;
    uint16_t mStartPt = 0;
    uint8_t strideW = 0;
    uint8_t strideH = 0;
    uint8_t filterW = 0;
    uint8_t filterH = 0;
    uint8_t dilationFilterW = 0;
    uint8_t dilationFilterH = 0;
    bool enTranspose = false;
    bool enSmallK = false;
    bool filterSizeW = false;
    bool filterSizeH = false;
    bool fMatrixCtrl = false;
    uint32_t dstDtypeBytes = 0;
    uint32_t srcDtypeBytes = 0;
    uint64_t dstSize = 0;
    uint64_t srcSize = 0;
    uint8_t dstLogicPos = 0;
    uint8_t srcLogicPos = 0;
    uint8_t dstPos = 0;
    uint8_t srcPos = 0;
};

struct LoadImageToLocalApiParams {
    __aicore__ LoadImageToLocalApiParams()
    {
        dstAddr = 0;
        horizSize = 0;
        vertSize = 0;
        horizStartPos = 0;
        vertStartPos = 0;
        srcHorizSize = 0;
        topPadSize = 0;
        botPadSize = 0;
        leftPadSize = 0;
        rightPadSize = 0;
        dstDtypeBytes = 0;
        dstSize = 0;
        dstLogicPos = 0;
        dstPos = 0;
    }

    __aicore__ LoadImageToLocalApiParams(
        uint64_t dstAddrIn, uint16_t horizSizeIn, uint16_t vertSizeIn, uint16_t horizStartPosIn,
        uint16_t vertStartPosIn, uint16_t srcHorizSizeIn, uint8_t topPadSizeIn, uint8_t botPadSizeIn,
        uint16_t leftPadSizeIn, uint16_t rightPadSizeIn, uint32_t dstDtypeBytesIn, uint64_t dstSizeIn, uint8_t dstPosIn)
    {
        dstAddr = dstAddrIn;
        horizSize = horizSizeIn;
        vertSize = vertSizeIn;
        horizStartPos = horizStartPosIn;
        vertStartPos = vertStartPosIn;
        srcHorizSize = srcHorizSizeIn;
        topPadSize = topPadSizeIn;
        botPadSize = botPadSizeIn;
        leftPadSize = leftPadSizeIn;
        rightPadSize = rightPadSizeIn;
        dstDtypeBytes = dstDtypeBytesIn;
        dstSize = dstSizeIn;
        dstLogicPos = dstPosIn;
        dstPos = static_cast<uint8_t>(GetPhyType(static_cast<TPosition>(dstPosIn)));
    }

    uint64_t dstAddr = 0;
    uint16_t horizSize = 0;
    uint16_t vertSize = 0;
    uint16_t horizStartPos = 0;
    uint16_t vertStartPos = 0;
    uint16_t srcHorizSize = 0;
    uint8_t topPadSize = 0;
    uint8_t botPadSize = 0;
    uint16_t leftPadSize = 0;
    uint16_t rightPadSize = 0;
    uint32_t dstDtypeBytes = 0;
    uint64_t dstSize = 0;
    uint8_t dstLogicPos = 0;
    uint8_t dstPos = 0;
};

bool CheckFuncMmadImpl(MmadApiParams& chkParams, const char* intriName);

bool CheckFuncInitConstValueImpl(CubeInitConstValueApiParams& chkParams, const char* intriName);

bool CheckFuncLoadData2dImpl(LoadData2dApiParams& chkParams, const char* intriName);

bool CheckFuncLoadData2dv2Impl(LoadData2dv2ApiParams& chkParams, const char* intriName);

bool CheckFuncLoadData3dv1Impl(LoadData3dv1ApiParams& chkParams, const char* intriName);

bool CheckFuncLoadData3dv2Impl(LoadData3dv2ApiParams& chkParams, const char* intriName);

bool CheckFuncLoadData3dv2ProImpl(LoadData3dv2ProApiParams& chkParams, const char* intriName);

bool CheckFuncLoadImageToLocalImpl(LoadImageToLocalApiParams& chkParams, const char* intriName);

} // namespace check
} // namespace AscendC
#endif
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_CHECK_CUBE_UTIL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_CHECK_CUBE_UTIL_H__
#endif
