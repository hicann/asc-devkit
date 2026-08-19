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
 * \file kernel_operator_mm_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_mm_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_mm_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_INTF_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_MM_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_MM_INTERFACE_IMPL_H
#include "../../include/basic_api/kernel_tensor.h"
#include "kernel_check.h"
#include "kernel_reg.h"
#include "kernel_npu_debug.h"
#include "kernel_operator_mm_base_impl.h"
#include "../../include/basic_api/kernel_struct_mm.h"

namespace AscendC {
__aicore__ inline void PrintTimeStamp(uint32_t descId);

/* **************************************************************************************************
 * LoadData 2d                                             *
 * ************************************************************************************************* */
/*
 * @ingroup DataLoad
 * @brief Cube data loading
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] loadDataParams.startIndex Fractal matrix ID
 * @param [in] loadDataParams.repeatTime repeat times
 * @param [in] loadDataParams.srcStride src block stride
 * @param [in] loadDataParams.sid SMMU SID
 * @param [in] loadDataParams.dstGap interval between the previous tail and the next fractal head
 * @param [in] loadDataParams.ifTranspose enable parameters of transpose function
 */
template <typename T>
__aicore__ inline void LoadData(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2DParams& loadDataParams)
{
    CheckLoadData2dDatatype<T>();
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckLoadData2dLocal2Local(dst, src, "LoadData with LoadData2DParams");
    CheckLoadData2dParams<T>(loadDataParams, true);
#endif
    LoadDataImpl(dst, src, loadDataParams);
}

template <typename T>
__aicore__ inline __inout_pipe__(MTE2) void LoadData(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const LoadData2DParams& loadDataParams)
{
#ifdef ASCENDC_TIME_STAMP_ON
    PrintTimeStamp(static_cast<uint32_t>(TimeStampId::TIME_STAMP_MTE2_DATACOPY));
#endif
    CheckLoadData2dDatatype<T>();
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckLoadData2dGlobal2Local(dst, "LoadData with LoadData2DParams");
    CheckLoadData2dParams<T>(loadDataParams, false);
#endif
    LoadDataImpl(dst, src, loadDataParams);
}

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
template <TPosition Dst, TPosition Src, typename T>
__aicore__ inline void LoadData(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const Load2DBitModeParam& loadDataParams)
{
    CheckLoadData2dDatatype<T>();
    LoadDataImpl<Dst, Src, T>(dst, src, loadDataParams);
}
#endif
/* **************************************************************************************************
 * LoadData 2dV2                                             *
 * ************************************************************************************************* */
/*
 * @ingroup DataLoad
 * @brief Cube data loading
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor/GlobalTensor
 * @param [in] loadDataParams.mStartPosition m start position
 * @param [in] loadDataParams.kStartPosition k start position
 * @param [in] loadDataParams.srcStride src block stride
 * @param [in] loadDataParams.dstStride dst block stride
 * @param [in] loadDataParams.mStep m step
 * @param [in] loadDataParams.kStep k step
 * @param [in] loadDataParams.sid SMMU SID
 * @param [in] loadDataParams.ifTranspose enable parameters of transpose function
 */
template <typename T>
__aicore__ inline void LoadData(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2DParamsV2& loadDataParams)
{
    CheckLoadData2dDatatype<T>();
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckLoadData2dLocal2Local(dst, src, "LoadData with LoadData2DParamsV2");
#endif
    LoadDataImpl(dst, src, loadDataParams);
}

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
template <typename T, typename U>
__aicore__ inline void LoadData(
    const LocalTensor<U>& dst, const LocalTensor<T>& src0, const LocalTensor<fp8_e8m0_t>& srcMx,
    const LoadData2DParamsV2& loadDataParams, const LoadData2DMxParams& loadMxDataParams)
{
    LoadDataImpl(dst, src0, srcMx, loadDataParams, loadMxDataParams);
}
#endif

template <typename T>
__aicore__ inline __inout_pipe__(MTE2) void LoadData(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const LoadData2DParamsV2& loadDataParams)
{
#ifdef ASCENDC_TIME_STAMP_ON
    PrintTimeStamp(static_cast<uint32_t>(TimeStampId::TIME_STAMP_MTE2_DATACOPY));
#endif
    CheckLoadData2dDatatype<T>();
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckLoadData2dGlobal2Local(dst, "LoadData with LoadData2DParamsV2");
#endif
    LoadDataImpl(dst, src, loadDataParams);
}

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5102)
template <typename T, typename U>
__aicore__ inline __inout_pipe__(MTE2) void LoadData(
    const LocalTensor<T>& dst, const GlobalTensor<U>& src, const LoadData2DParamsV2& loadDataParams,
    const Nd2NzParamsV2& nd2nzParams)
{
#ifdef ASCENDC_TIME_STAMP_ON
    PrintTimeStamp(static_cast<uint32_t>(TimeStampId::TIME_STAMP_MTE2_DATACOPY));
#endif
    LoadDataImpl(dst, src, loadDataParams, nd2nzParams);
}
#endif

/* **************************************************************************************************
 * LoadData 3dv1                                             *
 * ************************************************************************************************* */
/*
 * @ingroup DataLoad
 * @brief Cube data loading
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] loadDataParams.padList padding list
 * @param [in] loadDataParams.l1H operand height
 * @param [in] loadDataParams.l1W operand width
 * @param [in] loadDataParams.c1Inde The starting point of the tensor C1 dimension
 * @param [in] loadDataParams.fetchFilterW The starting position of the w dimension on the convolution kernel
 * @param [in] loadDataParams.fetchFilterH The starting position of the H dimension on the convolution kernel
 * @param [in] loadDataParams.leftTopW Start point of the W dimension on the source operand
 * @param [in] loadDataParams.leftTopH Start point of the H dimension on the source operand
 * @param [in] loadDataParams.strideW W dimension stride
 * @param [in] loadDataParams.strideH H dimension stride
 * @param [in] loadDataParams.filterW Convolution kernel width
 * @param [in] loadDataParams.filterH Convolution kernel height
 * @param [in] loadDataParams.dilationFilterW Convolution kernel width expansion coefficient
 * @param [in] loadDataParams.dilationFilterH Convolution kernel height expansion coefficient
 * @param [in] loadDataParams.jumpStride repeat stride
 * @param [in] loadDataParams.repeatMode repeat mode
 * @param [in] loadDataParams.repeatTimes repeat times
 * @param [in] loadDataParams.cSize judge whether to turn on optimization
 * @param [in] loadDataParams.padValue Value of Pad filling value
 */
template <
    typename T, const IsResetLoad3dConfig& defaultConfig, typename U,
    typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type>
__aicore__ inline void LoadData(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV1<U>& loadDataParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckTensorPhyPosition<Hardware::L1>(src, "src", "L1 Buffer(A1/B1)", "LoadData with LoadData3DParamsV1");
    CheckTensorAlignment(src, ONE_BLK_SIZE, "src", "LoadData with LoadData3DParamsV1");
    const Hardware dstScope = GetPhyType((TPosition)dst.GetPosition());
    if (dstScope == Hardware::L0A || dstScope == Hardware::L0B) {
        CheckTensorAlignment(dst, VALUE_512, "dst", "LoadData with LoadData3DParamsV1");
    } else if (dstScope == Hardware::UB) {
        CheckTensorAlignment(dst, ONE_BLK_SIZE, "dst", "LoadData with LoadData3DParamsV1");
    }
    CheckLoadData3dv1Params<U>(loadDataParams);
    CheckLoadData3dParams(loadDataParams.l1H, loadDataParams.l1W, loadDataParams.strideW, loadDataParams.strideH);
#endif
    LoadDataImpl<T, defaultConfig>(dst, src, loadDataParams);
}

/* **************************************************************************************************
 * LoadData 3dv2                                             *
 * enhanced from v1, suitable for aicore > 200                                             *
 * ************************************************************************************************* */
/*
 * @ingroup DataLoad
 * @brief Cube data loading
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] loadDataParams.padList padding list
 * @param [in] loadDataParams.l1H operand height
 * @param [in] loadDataParams.l1W operand width
 * @param [in] loadDataParams.channelSize number of channels
 * @param [in] loadDataParams.kExtension Transmission length of K dimension
 * @param [in] loadDataParams.mExtension Transmission length of M dimension
 * @param [in] loadDataParams.kStartPt Start point of K dimension
 * @param [in] loadDataParams.mStartPt Start point of M dimension
 * @param [in] loadDataParams.strideW W dimension stride
 * @param [in] loadDataParams.strideH H dimension stride
 * @param [in] loadDataParams.filterW Convolution kernel width
 * @param [in] loadDataParams.filterH Convolution kernel height
 * @param [in] loadDataParams.dilationFilterW Convolution kernel width expansion coefficient
 * @param [in] loadDataParams.dilationFilterH Convolution kernel height expansion coefficient
 * @param [in] loadDataParams.enTranspose judge whether to enable the transpose function
 * @param [in] loadDataParams.enSmallK Whether to enable the small k feature
 * @param [in] loadDataParams.padValue Value of Pad filling value
 */
template <
    typename T, const IsResetLoad3dConfig& defaultConfig, typename U,
    typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type>
__aicore__ inline void LoadData(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV2<U>& loadDataParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckTensorPhyPosition<Hardware::L1>(src, "src", "L1 Buffer(A1/B1)", "LoadData with LoadData3DParamsV2");
    CheckTensorAlignment(src, ONE_BLK_SIZE, "src", "LoadData with LoadData3DParamsV2");
    const Hardware dstScope = GetPhyType((TPosition)dst.GetPosition());
    if (dstScope == Hardware::L0A || dstScope == Hardware::L0B) {
        CheckTensorAlignment(dst, VALUE_512, "dst", "LoadData with LoadData3DParamsV2");
    } else if (dstScope == Hardware::UB) {
        CheckTensorAlignment(dst, ONE_BLK_SIZE, "dst", "LoadData with LoadData3DParamsV2");
    }
    CheckLoadData3dv2Params<U>(loadDataParams);
    CheckLoadData3dv2ChannelSize<T>(loadDataParams.channelSize);
    CheckLoadData3dParams(loadDataParams.l1H, loadDataParams.l1W, loadDataParams.strideW, loadDataParams.strideH);
    CheckLoadData3dv2MatrixParams<T>(
        loadDataParams.kExtension, loadDataParams.mExtension, loadDataParams.kStartPt, loadDataParams.mStartPt);
#endif
    LoadDataImpl<T, defaultConfig>(dst, src, loadDataParams);
}

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
template <
    typename T, const IsResetLoad3dConfig& defaultConfig, typename U,
    typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type>
__aicore__ inline void LoadDataWithStride(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV2<U>& loadDataParams)
{
#if ASCENDC_CPU_DEBUG
    CheckLoadData3dv2ChannelSize<T>(loadDataParams.channelSize);
    CheckLoadData3dParams(loadDataParams.l1H, loadDataParams.l1W, loadDataParams.strideW, loadDataParams.strideH);
    CheckLoadData3dv2MatrixParams<T>(
        loadDataParams.kExtension, loadDataParams.mExtension, loadDataParams.kStartPt, loadDataParams.mStartPt);
#endif
    LoadDataWithStrideImpl<T, defaultConfig>(dst, src, loadDataParams);
}
#endif

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
template <TPosition Dst, TPosition Src, typename T>
__aicore__ inline void LoadData(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const Load3DBitModeParam& loadDataParams)
{
    LoadDataImpl<Dst, Src, T>(dst, src, loadDataParams);
}
#endif
/* **************************************************************************************************
 * LoadData 3dv2Pro                                             *
 * enhanced from v1, suitable for aicore > 200                                             *
 * ************************************************************************************************* */
/*
 * @ingroup DataLoad
 * @brief Cube data loading
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] loadDataParams.padList padding list
 * @param [in] loadDataParams.l1H operand height
 * @param [in] loadDataParams.l1W operand width
 * @param [in] loadDataParams.channelSize number of channels
 * @param [in] loadDataParams.kExtension Transmission length of K dimension
 * @param [in] loadDataParams.mExtension Transmission length of M dimension
 * @param [in] loadDataParams.kStartPt Start point of K dimension
 * @param [in] loadDataParams.mStartPt Start point of M dimension
 * @param [in] loadDataParams.strideW W dimension stride
 * @param [in] loadDataParams.strideH H dimension stride
 * @param [in] loadDataParams.filterW Convolution kernel width
 * @param [in] loadDataParams.filterH Convolution kernel height
 * @param [in] loadDataParams.dilationFilterW Convolution kernel width expansion coefficient
 * @param [in] loadDataParams.dilationFilterH Convolution kernel height expansion coefficient
 * @param [in] loadDataParams.enTranspose judge whether to enable the transpose function
 * @param [in] loadDataParams.enSmallK Whether to enable the small k feature
 * @param [in] loadDataParams.padValue Value of Pad filling value
 */
template <typename T>
__aicore__ inline void LoadData(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData3DParamsV2Pro& loadDataParams)
{
    LoadDataImpl<T>(dst, src, loadDataParams);
}

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003))
// cce compiler process load3d bfloat16_t using B8, so use the half dtype instead
template <>
__aicore__ inline void LoadData(
    const LocalTensor<bfloat16_t>& dst, const LocalTensor<bfloat16_t>& src, const LoadData3DParamsV2Pro& loadDataParams)
{
    LoadDataImpl(dst, src, loadDataParams);
}
#endif

/* **************************************************************************************************
 * LoadDataWithTranspose                                             *
 * ************************************************************************************************* */
/*
 * @ingroup DataLoad
 * @brief Cube data loading
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] loadDataParams.startIndex index of the first fractal in the first repeat in the source matrix
 * in unit of frac num
 * @param [in] loadDataParams.repeatTimes the repeat times
 * @param [in] loadDataParams.srcStride source stride between consecutive repeat times in unit of frac num
 * @param [in] loadDataParams.dstGap destination gap between consecutive repeat times in unit of 512byte
 * @param [in] loadDataParams.dstFracGap dst fractal gap in unit of one 512byte fractal
 */
template <typename T>
__aicore__ inline void LoadDataWithTranspose(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2dTransposeParams& loadDataParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckLoadDataWithTranspose(dst, src, "LoadDataWithTranspose with LoadData2dTransposeParams");
    ReportNopWarning<uint8_t>(
        loadDataParams.repeatTimes, "loadDataParams.repeatTimes",
        "LoadDataWithTranspose with LoadData2dTransposeParams");
#endif
    LoadDataWithTransposeImpl(dst, src, loadDataParams);
}

/*
 * @ingroup DataLoad
 * @brief Cube data loading
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] loadDataParams.startIndex index of the first fractal in the first repeat in the source matrix
 * in unit of 512byte fractal
 * @param [in] loadDataParams.repeatTime the repeat times
 * @param [in] loadDataParams.srcStride source stride between consecutive repeat times in unit of 512byte
 * @param [in] loadDataParams.dstGap destination gap between consecutive repeat times in unit of 512byte
 * @param [in] loadDataParams.dstFracGap dst fractal gap in unit of one 512byte fractal
 * @param [in] loadDataParams.srcFracGap dst fractal gap in unit of one 512byte fractal
 */
template <typename T>
__aicore__ inline void LoadDataWithTranspose(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2dTransposeParamsV2& loadDataParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckLoadDataWithTranspose(dst, src, "LoadDataWithTranspose with LoadData2dTransposeParamsV2");
    ReportNopWarning<uint8_t>(
        loadDataParams.repeatTimes, "loadDataParams.repeatTimes",
        "LoadDataWithTranspose with LoadData2dTransposeParamsV2");
#endif
    LoadDataWithTransposeImpl(dst, src, loadDataParams);
}

/* **************************************************************************************************
 * Mmad                                             *
 * ************************************************************************************************* */
/*
 * @ingroup Mmad
 * @brief Matrix multiplication and addition
 * @param [out] dst output LocalTensor
 * @param [in] fm input LocalTensor
 * @param [in] filter input LocalTensor
 * @param [in] mmadParams.m Left matrix row number
 * @param [in] mmadParams.n right matrix column number
 * @param [in] mmadParams.k Left matrix column number m
 * @param [in] mmadParams.unitFlag whether enable unit flag
 * @param [in] mmadParams.kDirectionAlign is the indicator for alignment in L0A/L0B in the K direction
 * @param [in] mmadParams.cmatrixSource indicates the C matrix source, 1: the C matrix is in bias table buffer, 0: the C
 * matrix is in L0C
 * @param [in] mmadParams.cmatrixInitVal indicates the initial matrix, 1: the number in C matrix is 0, 0：use the real
 * number in C matrix
 */

template <typename T, typename U, typename S>
__aicore__ inline void Mmad(
    const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const MmadParams& mmadParams)
{
    MmadImpl(dst, fm, filter, mmadParams);
}

template <typename T, typename U, typename S, typename V>
__aicore__ inline void Mmad(
    const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const LocalTensor<V>& bias,
    const MmadParams& mmadParams)
{
    MmadImpl(dst, fm, filter, bias, mmadParams);
}

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
template <typename T, typename U, typename S>
__aicore__ inline void Mmad(
    const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter,
    const MmadBitModeParams& mmadParams)
{
    MmadImpl(dst, fm, filter, mmadParams);
}

template <typename T, typename U, typename S, typename V>
__aicore__ inline void Mmad(
    const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const LocalTensor<V>& bias,
    const MmadBitModeParams& mmadParams)
{
    MmadImpl(dst, fm, filter, bias, mmadParams);
}

template <typename T, typename U, typename S>
__aicore__ inline void MmadMx(
    const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const MmadParams& mmadParams)
{
    MmadMxImpl(dst, fm, filter, mmadParams);
}

template <typename T, typename U, typename S, typename V>
__aicore__ inline void MmadMx(
    const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const LocalTensor<V>& bias,
    const MmadParams& mmadParams)
{
    MmadMxImpl(dst, fm, filter, bias, mmadParams);
}

template <typename T, typename U, typename S, typename R>
__aicore__ inline void MmadMx(
    const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter,
    const MmadBitModeParams& mmadParams)
{
    MmadMxImpl(dst, fm, filter, mmadParams);
}

template <typename T, typename U, typename S, typename V, typename R>
__aicore__ inline void MmadMx(
    const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const LocalTensor<V>& bias,
    const MmadBitModeParams& mmadParams)
{
    MmadMxImpl(dst, fm, filter, bias, mmadParams);
}
#endif

#if __NPU_ARCH__ == 2201
template <
    typename T, typename U, typename Std::enable_if<Std::is_same<PrimT<T>, int32_t>::value, bool>::type,
    typename Std::enable_if<Std::is_same<PrimT<U>, int8_t>::value, bool>::type>
__aicore__ inline void MmadWithSparse(
    const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<U>& filter, const MmadParams& mmadParams)
{
    MmadSpImpl(dst, fm, filter, mmadParams);
}

template <
    typename T, typename U, typename Std::enable_if<Std::is_same<PrimT<T>, int8_t>::value, bool>::type,
    typename Std::enable_if<Std::is_same<PrimT<U>, uint8_t>::value, bool>::type>
__aicore__ inline void LoadDataWithSparse(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<U>& idx,
    const LoadData2dParams& loadDataParam)
{
    LoadDataWithSparseImpl(dst, src, idx, loadDataParam);
}
#endif

#if __NPU_ARCH__ == 2002
template <typename T, typename Std::enable_if<Std::is_same<PrimT<T>, int8_t>::value, bool>::type>
__aicore__ inline void LoadUnzipIndex(const GlobalTensor<T>& src, uint32_t numOfIndexTabEntry)
{
    LoadUnzipIndexImpl(src, numOfIndexTabEntry);
}
#endif

/* **************************************************************************************************
 * BroadCastVecToMM                                             *
 * ************************************************************************************************* */
template <typename T, typename U>
__aicore__ inline __inout_pipe__(V) void BroadCastVecToMM(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const int32_t blockCount, const uint8_t blockLen,
    const uint8_t srcGap, const uint8_t dstGap)
{
    BroadCastVecToMMImpl(dst, src, blockCount, blockLen, srcGap, dstGap);
}

/* **************************************************************************************************
 * Fill                                             *
 * ************************************************************************************************* */
/*
 * @ingroup Fill
 * @brief L0A/L0B/L1 value initializing
 * @param [out] dst output LocalTensor
 * @param [in] initConstValueParams.repeatTimes repeat times
 * @param [in] initConstValueParams.repeatTimes blockNum block number
 * @param [in] initConstValueParams.dstGap interval between the previous tail and the next block head
 * @param [in] initConstValueParams.initValue initialize Value
 */
template <typename T, typename U>
__aicore__ inline void CheckInitParams(
    const LocalTensor<T>& dst, const InitConstValueParams<U>& initConstValueParams, const char* intriName)
{
#if ASCENDC_CPU_DEBUG
    uint16_t repeatTime = initConstValueParams.repeatTimes;
#if __NPU_ARCH__ == 2201
    uint16_t blockNum = initConstValueParams.blockNum;
    uint16_t dstGap = initConstValueParams.dstGap;
#else
    uint16_t blockNum = 1;
    uint16_t dstGap = 0;
#endif
    if (!CheckFuncInitConstValue(dst, repeatTime, blockNum, dstGap, intriName)) {
        ASCENDC_REPORT_CHECK_ERROR(intriName, KernelFuncType::NONE_MODE);
    }
#endif
}

namespace Impl {
constexpr const char* FILL_INTRINSIC_NAME = "Fill";
constexpr const char* INITCONSTVALUE_INTRINSIC_NAME = "InitConstValue";
} // namespace Impl

template <typename T, typename U, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type>
__aicore__ inline void Fill(const LocalTensor<T>& dst, const InitConstValueParams<U>& initConstValueParams)
{
    CheckInitParams(dst, initConstValueParams, Impl::FILL_INTRINSIC_NAME);
    FillImpl(dst, initConstValueParams);
}

// InitConstValue has been updated, please use Fill instead.
template <typename T, typename U, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type>
__aicore__ inline void InitConstValue(const LocalTensor<T>& dst, const InitConstValueParams<U>& initConstValueParams)
{
    CheckInitParams(dst, initConstValueParams, Impl::INITCONSTVALUE_INTRINSIC_NAME);
    InitConstValueImpl(dst, initConstValueParams);
}

/* **************************************************************************************************
 * SetLoadDataPaddingValue                                             *
 * ************************************************************************************************* */
/*
 * @ingroup SetLoadDataPaddingValue
 * @brief setting loadData pad value
 * @param [in]padValue padding value
 */
template <typename T>
__aicore__ inline void SetLoadDataPaddingValue(const T padValue)
{
    Load3DSetPaddingImpl(padValue);
}

/* **************************************************************************************************
 * SetFmatrix                                             *
 * ************************************************************************************************* */
/*
 * @ingroup SetFmatrix
 * @brief setting fmatrix
 * @param [in]l1H operand height
 * @param [in]l1W operand width
 * @param [in]padList padding list
 */
__aicore__ inline void SetFmatrix(uint16_t l1H, uint16_t l1W, const uint8_t padList[4], const FmatrixMode& fmatrixMode)
{
    ASCENDC_CHECK_VALUE_RANGE(l1H, MIN_LOAD3D_L1, MAX_LOAD3D_L1, "l1H", "SetFmatrix");
    ASCENDC_CHECK_VALUE_RANGE(l1W, MIN_LOAD3D_L1, MAX_LOAD3D_L1, "l1W", "SetFmatrix");
    SetFmatrixImpl(l1H, l1W, padList, fmatrixMode);
}

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
__aicore__ inline void SetFmatrix(const SetFMatrixBitModeParams& param, const FmatrixMode& fmatrixMode)
{
    SetFmatrixImpl(param, fmatrixMode);
}
#endif

/* **************************************************************************************************
 * SetLoadDataBoundary                                             *
 * ************************************************************************************************* */
/*
 * @ingroup SetLoadDataBoundary
 * @brief setting loaddata boundary
 * @param [in]boundaryValue
 */
__aicore__ inline void SetLoadDataBoundary(uint32_t boundaryValue) { SetLoadDataBoundaryImpl(boundaryValue); }

/* **************************************************************************************************
 * SetLoadDataRepeat                                             *
 * ************************************************************************************************* */
__aicore__ inline void SetLoadDataRepeat(const LoadDataRepeatParam& repeatParams)
{
    ASCENDC_CHECK_VALUE_RANGE(repeatParams.repeatMode, 0, 1, "repeatMode", "SetLoadDataRepeat");
    SetLoadDataRepeatImpl(repeatParams);
}

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
__aicore__ inline void SetLoadDataRepeatWithStride(const LoadDataRepeatParamWithStride& repeatParams)
{
    ASCENDC_CHECK_VALUE_RANGE(repeatParams.repeatMode, 0, 1, "repeatMode", "SetLoadDataRepeat");
    SetLoadDataRepeatWithStrideImpl(repeatParams);
}
#endif

/* **************************************************************************************************
 * LoadImageToLocal                                             *
 * ************************************************************************************************* */
/*
 * @ingroup LoadImageToLocal
 * @brief loadData image from gm to L1
 * @param [out] dst output LocalTensor
 * @param [in] loadImageToLocalParams.horizSize operand height
 * @param [in] loadImageToLocalParams.vertSize operand width
 * @param [in] loadImageToLocalParams.horizStartPos horizontal start position
 * @param [in] loadImageToLocalParams.vertStartPos vertical start position
 * @param [in] loadImageToLocalParams.srcHorizSize src horizontal size
 * @param [in] loadImageToLocalParams.topPadSize top padding size
 * @param [in] loadImageToLocalParams.botPadSize bottom padding size
 * @param [in] loadImageToLocalParams.leftPadSize left hblank/padding size
 * @param [in] loadImageToLocalParams.rightPadSize right hblank/padding size
 */
template <typename T>
__aicore__ inline __inout_pipe__(MTE2) void LoadImageToLocal(
    const LocalTensor<T>& dst, const LoadImageToLocalParams& loadDataParams)
{
#ifdef ASCENDC_TIME_STAMP_ON
    PrintTimeStamp(static_cast<uint32_t>(TimeStampId::TIME_STAMP_MTE2_DATACOPY));
#endif
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    ASCENDC_ASSERT(CheckFuncLoadImageToLocal(dst, loadDataParams, "LoadImageToLocal"), {
        ASCENDC_REPORT_CHECK_ERROR("LoadImageToLocal", KernelFuncType::NONE_MODE);
    });
#endif
    ASCENDC_CHECK_VALUE_RANGE(loadDataParams.horizSize, 2, UINT12_MAX, "horizSize", "LoadImageToLocal");
    ASCENDC_CHECK_VALUE_RANGE(loadDataParams.vertSize, 2, UINT12_MAX, "vertSize", "LoadImageToLocal");
    ASCENDC_CHECK_VALUE_RANGE(loadDataParams.horizStartPos, 0, UINT12_MAX, "horizStartPos", "LoadImageToLocal");
    ASCENDC_CHECK_VALUE_RANGE(loadDataParams.vertStartPos, 0, UINT12_MAX, "vertStartPos", "LoadImageToLocal");
    ASCENDC_CHECK_VALUE_RANGE(loadDataParams.srcHorizSize, 2, UINT12_MAX, "srcHorizSize", "LoadImageToLocal");
    ASCENDC_CHECK_VALUE_RANGE(loadDataParams.topPadSize, 0, 32, "topPadSize", "LoadImageToLocal");
    ASCENDC_CHECK_VALUE_RANGE(loadDataParams.botPadSize, 0, 32, "botPadSize", "LoadImageToLocal");
    ASCENDC_CHECK_VALUE_RANGE(loadDataParams.leftPadSize, 0, 32, "leftPadSize", "LoadImageToLocal");
    ASCENDC_CHECK_VALUE_RANGE(loadDataParams.rightPadSize, 0, 32, "rightPadSize", "LoadImageToLocal");
    const Hardware dstScope = GetPhyType((TPosition)dst.GetPosition());
    if (dstScope == Hardware::L1) {
        LoadImageToLocalCal((__cbuf__ PrimT<T>*)dst.GetPhyAddr(), loadDataParams);
    } else {
        ASCENDC_CHECK_TPOSITION(
            false, "dst", "L1 Buffer(A1/B1)", "LoadImageToLocal",
            GetPositionDisplay(static_cast<TPosition>(dst.GetPosition())));
    }
}

/* **************************************************************************************************
 * LoadDataUnzip                                             *
 * ************************************************************************************************* */
/*
 * @ingroup LoadDataUnzip
 * @brief loadData and unzip
 * @param [out] dst output LocalTensor
 * @param [in] src input GlobalTensor
 */
template <typename T>
__aicore__ inline void LoadDataUnzip(const LocalTensor<T>& dst, const GlobalTensor<T>& src)
{
    LoadDataUnzipImpl(dst, src);
}

/*
 * @brief Sets whether to enable HF32 mode for Mmad computation
 * @param [in] mode HF32 mode enumeration
 * @note When mode is HF32Mode::ENABLE, FP32 data in L0A/L0B will be rounded to HF32 before matrix multiplication;
 * when mode is HF32Mode::DISABLE, regular FP32 matrix multiplication will be executed
 */
__aicore__ inline void SetHF32Mode(HF32Mode mode) { SetHF32ModeImpl(mode == AscendC::HF32Mode::ENABLE); }

/*
 * @brief Sets whether to enable HF32 mode for Mmad computation
 * @param [in] hf32Mode Control parameter for Mmad HF32 mode
 * @note When hf32Mode is true, FP32 data in L0A/L0B will be rounded to HF32 before matrix multiplication; when false,
 * regular FP32 matrix multiplication will be executed
 */
// SetHF32Mode(bool hf32Mode) has been updated, please use SetHF32Mode(HF32Mode mode) instead.
__aicore__ inline void SetHF32Mode(bool hf32Mode) { SetHF32ModeImpl(hf32Mode); }

/*
 * @brief Sets the rounding method for HF32 rounding mode
 * @param [in] mode HF32 trans mode enumeration
 * @note Must call SetHF32Mode to enable HF32 rounding mode first.
 */
__aicore__ inline void SetHF32TransMode(HF32TransMode mode)
{
    SetHF32TransModeImpl(mode == AscendC::HF32TransMode::NEAREST_AWAY);
}

/*
 * @brief Sets the rounding method for HF32 rounding mode
 * @param [in] hf32TransMode Control parameter for Mmad HF32 mode
 * @note Must Call SetHF32Mode to enable HF32 rounding mode first.When hf32TransMode is true, FP32 is rounded to HF32
 * with rounding to nearest, with ties away from zero; when false, rounded to nearest, with ties to even
 */
// SetHF32TransMode(bool hf32TransMode) has been updated, please use SetHF32TransMode(HF32TransMode mode) instead.
__aicore__ inline void SetHF32TransMode(bool hf32TransMode) { SetHF32TransModeImpl(hf32TransMode); }

/*
 * @ingroup MMLayout
 * @brief Sets matrix multiplication result layout to row major
 * @note This function sets the CUBE output to row major format (M direction first, then N direction)
 */
__aicore__ inline void SetMMRowMajor() { SetMMLayoutTransformImpl(true); }

/*
 * @ingroup MMLayout
 * @brief Sets matrix multiplication result layout to column major
 * @note This function sets the CUBE output to column major format (N direction first, then M direction)
 */
__aicore__ inline void SetMMColumnMajor() { SetMMLayoutTransformImpl(false); }

/*
 * @brief Sets the priority direction (M or N) for Mmad/MmadWithSparse computation
 * @param [in] mmLayoutMode Control parameter for Mmad/MmadWithSparse priority direction
 * @note When mmLayoutMode is true, CUBE generates results first through N direction then M direction; when false, first
 * through M direction then N direction
 */
// SetMMLayoutTransform has been updated, please use SetMMRowMajor/SetMMColumnMajor instead.
__aicore__ inline void SetMMLayoutTransform(bool mmLayoutMode) { SetMMLayoutTransformImpl(mmLayoutMode); }

} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_MM_INTERFACE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_INTF_IMPL_H__
#endif
