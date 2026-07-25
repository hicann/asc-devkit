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
 * \file kernel_operator_mm_load2d_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_mm_load2d_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_mm_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_LOAD2D_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_MM_LOAD2D_H
#define ASCENDC_MODULE_OPERATOR_MM_LOAD2D_H
#include "../../include/basic_api/kernel_struct_mm.h"

namespace AscendC {
/* **************************************************************************************************
 * LoadData 2d                                             *
 * ************************************************************************************************* */
/*
 * @ingroup DataLoad
 * @brief Cube data loading
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] loadDataParams.startIndex Fractal matrix ID
 * @param [in] loadDataParams.repeatTimes repeat times
 * @param [in] loadDataParams.srcStride src block stride
 * @param [in] loadDataParams.sid SMMU SID
 * @param [in] loadDataParams.dstGap interval between the previous tail and the next fractal head
 * @param [in] loadDataParams.ifTranspose enable parameters of transpose function
 */
template <typename T>
__aicore__ inline void LoadDataImpl(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2DParams& loadDataParams)
{
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncLoadData2d(dst, src, loadDataParams, "LoadData with LoadData2DParams")) {
        ASCENDC_REPORT_CHECK_ERROR("LoadData with LoadData2DParams", KernelFuncType::NONE_MODE);
    }
#endif
    const Hardware dstScope = GetPhyType((TPosition)dst.GetPosition());
    if (dstScope == Hardware::L0A) {
        LoadData2DL12L0ACal((__ca__ PrimT<T>*)dst.GetPhyAddr(), (__cbuf__ PrimT<T>*)src.GetPhyAddr(), loadDataParams);
    } else if (dstScope == Hardware::L0B) {
        LoadData2DL12L0BCal((__cb__ PrimT<T>*)dst.GetPhyAddr(), (__cbuf__ PrimT<T>*)src.GetPhyAddr(), loadDataParams);
    }
}

template <typename T>
__aicore__ inline __inout_pipe__(MTE2) void LoadDataImpl(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const LoadData2DParams& loadDataParams)
{
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncLoadData2d(dst, src, loadDataParams, "LoadData with LoadData2DParams")) {
        ASCENDC_REPORT_CHECK_ERROR("LoadData with LoadData2DParams", KernelFuncType::NONE_MODE);
    }
#endif
    const Hardware dstScope = GetPhyType((TPosition)dst.GetPosition());
    if (dstScope == Hardware::L0A) {
        LoadData2DGM2L0ACal((__ca__ PrimT<T>*)dst.GetPhyAddr(), (__gm__ PrimT<T>*)src.GetPhyAddr(), loadDataParams);
    } else if (dstScope == Hardware::L0B) {
        LoadData2DGM2L0BCal((__cb__ PrimT<T>*)dst.GetPhyAddr(), (__gm__ PrimT<T>*)src.GetPhyAddr(), loadDataParams);
    } else if (dstScope == Hardware::L1) {
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
        const uint8_t cacheMode = ExtractCacheMode(src);
        LoadData2DGM2L1Cal(
            (__cbuf__ PrimT<T>*)dst.GetPhyAddr(), (__gm__ PrimT<T>*)src.GetPhyAddr(), loadDataParams, cacheMode);
#else
        LoadData2DGM2L1Cal((__cbuf__ PrimT<T>*)dst.GetPhyAddr(), (__gm__ PrimT<T>*)src.GetPhyAddr(), loadDataParams);
#endif
    }
}

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
template <TPosition Dst, TPosition Src, typename T>
__aicore__ inline void LoadDataImpl(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const Load2DBitModeParam& loadDataParams)
{
    CheckTensorAlign<T>(src, ONE_BLK_SIZE, "src", "LoadData with LoadData2DParams");
    CheckTensorAlign<T>(dst, VALUE_512, "dst", "LoadData with LoadData2DParams");

    if constexpr (Src != TPosition::A1 && Src != TPosition::A2) {
        ASCENDC_CHECK_TPOSITION(
            false, "src", "L1 Buffer(A1/B1)", "LoadData with LoadDataBitModeParams",
            GetPositionDisplay(static_cast<TPosition>(src.GetPosition())));
    };
    if constexpr (Dst == TPosition::A2) {
        LoadData2DL12L0ACal((__ca__ PrimT<T>*)dst.GetPhyAddr(), (__cbuf__ PrimT<T>*)src.GetPhyAddr(), loadDataParams);
    } else if constexpr (Dst == TPosition::B2) {
        LoadData2DL12L0BCal((__cb__ PrimT<T>*)dst.GetPhyAddr(), (__cbuf__ PrimT<T>*)src.GetPhyAddr(), loadDataParams);
    } else {
        ASCENDC_CHECK_TPOSITION(
            false, "dst", "L0A Buffer(A2)/L0B Buffer(B2)", "LoadData with LoadData2DParams",
            GetPositionDisplay(static_cast<TPosition>(dst.GetPosition())));
    }
}
#endif
/* **************************************************************************************************
 * LoadData 2d with transpose                                             *
 * ************************************************************************************************* */
/*
 * @ingroup DataLoad
 * @brief Cube data loading
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] loadDataParams.startIndex Fractal matrix ID in unit of fractal nums depending on dtype
 * @param [in] loadDataParams.repeatTimes repeat times
 * @param [in] loadDataParams.srcStride src block stride in unit of fractal nums depending on dtype
 * @param [in] loadDataParams.dstGap interval between the previous tail and the next fractal head in unit of one 512byte
 * fractal
 * @param [in] loadDataParams.dstFracGap dst fractal gap in unit of one 512byte fractal
 */
template <typename T>
__aicore__ inline void LoadDataWithTransposeImpl(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2dTransposeParams& loadDataParams)
{
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncLoadDataTranspose(dst, src, loadDataParams, "LoadDataWithTranspose")) {
        ASCENDC_REPORT_CHECK_ERROR("LoadDataWithTranspose with LoadData2dTransposeParams", KernelFuncType::NONE_MODE);
    }
#endif
    const Hardware dstScope = GetPhyType((TPosition)dst.GetPosition());
    if (dstScope == Hardware::L0A) {
        LoadData2DL12L0ATransposeCal(
            (__ca__ PrimT<T>*)dst.GetPhyAddr(), (__cbuf__ PrimT<T>*)src.GetPhyAddr(), loadDataParams);
    } else if (dstScope == Hardware::L0B) {
        LoadData2DL12L0BTransposeCal(
            (__cb__ PrimT<T>*)dst.GetPhyAddr(), (__cbuf__ PrimT<T>*)src.GetPhyAddr(), loadDataParams);
    }
}

/*
 * @ingroup DataLoad
 * @brief Cube data loading
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] loadDataParams.startIndex Fractal matrix ID in unit of one 512byte fractal
 * @param [in] loadDataParams.repeatTimes repeat times
 * @param [in] loadDataParams.srcStride src block stride in unit of one 512byte fractal
 * @param [in] loadDataParams.dstGap interval between the previous tail and the next fractal head in unit of one 512byte
 * fractal
 * @param [in] loadDataParams.dstFracGap dst fractal gap in unit of one 512byte fractal
 * @param [in] loadDataParams.srcFracGap src fractal gap in unit of one 512byte fractal
 */
template <typename T>
__aicore__ inline void LoadDataWithTransposeImpl(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2dTransposeParamsV2& loadDataParams)
{
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncLoadDataTranspose(dst, src, loadDataParams, "LoadDataWithTranspose")) {
        ASCENDC_REPORT_CHECK_ERROR("LoadDataWithTranspose with LoadData2dTransposeParamsV2", KernelFuncType::NONE_MODE);
    }
#endif
    const Hardware dstScope = GetPhyType((TPosition)dst.GetPosition());
    if (dstScope == Hardware::L0B) {
        LoadData2DL12L0BTransposeCal(
            (__cb__ PrimT<T>*)dst.GetPhyAddr(), (__cbuf__ PrimT<T>*)src.GetPhyAddr(), loadDataParams);
    }
}

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
__aicore__ inline void LoadDataImpl(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2DParamsV2& loadDataParams)
{
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncLoadData2dv2(dst, src, loadDataParams, "LoadData with LoadData2DParamsV2")) {
        ASCENDC_REPORT_CHECK_ERROR("LoadData with LoadData2DParamsV2", KernelFuncType::NONE_MODE);
    }
#endif
    const Hardware dstScope = GetPhyType((TPosition)dst.GetPosition());
    if (dstScope == Hardware::L0A) {
        LoadData2DL12L0ACal((__ca__ PrimT<T>*)dst.GetPhyAddr(), (__cbuf__ PrimT<T>*)src.GetPhyAddr(), loadDataParams);
    } else if (dstScope == Hardware::L0B) {
        LoadData2DL12L0BCal((__cb__ PrimT<T>*)dst.GetPhyAddr(), (__cbuf__ PrimT<T>*)src.GetPhyAddr(), loadDataParams);
    }
}

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
template <typename T, typename U>
__aicore__ inline void LoadDataImpl(
    const LocalTensor<U>& dst, const LocalTensor<T>& src, const LocalTensor<fp8_e8m0_t>& srcMx,
    const LoadData2DParamsV2& loadDataParams, const LoadData2DMxParams& loadMxDataParams)
{
    CheckTensorPos<T>(src, Hardware::L1, "src", "A1/B1", "LoadData with LoadData2DParamsV2");
    const Hardware dstScope = GetPhyType((TPosition)dst.GetPosition());
    if (dstScope == Hardware::L0A) {
        LoadData2DL12L0ACal(
            (__ca__ PrimT<U>*)dst.GetPhyAddr(), (__cbuf__ PrimT<T>*)src.GetPhyAddr(),
            (__cbuf__ fp8_e8m0_t*)srcMx.GetPhyAddr(), loadDataParams, loadMxDataParams);
    } else if (dstScope == Hardware::L0B) {
        LoadData2DL12L0BCal(
            (__cb__ PrimT<U>*)dst.GetPhyAddr(), (__cbuf__ PrimT<T>*)src.GetPhyAddr(),
            (__cbuf__ fp8_e8m0_t*)srcMx.GetPhyAddr(), loadDataParams, loadMxDataParams);
    } else {
        ASSERT(false);
    }
}
#endif

template <typename T>
__aicore__ inline __inout_pipe__(MTE2) void LoadDataImpl(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const LoadData2DParamsV2& loadDataParams)
{
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncLoadData2dv2(dst, src, loadDataParams, "LoadData with LoadData2DParamsV2")) {
        ASCENDC_REPORT_CHECK_ERROR("LoadData with LoadData2DParamsV2", KernelFuncType::NONE_MODE);
    }
#endif
    const Hardware dstScope = GetPhyType((TPosition)dst.GetPosition());
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    const uint8_t cacheMode = ExtractCacheMode(src);
    if (dstScope == Hardware::L0A) {
        LoadData2DGM2L0ACal(
            (__ca__ PrimT<T>*)dst.GetPhyAddr(), (__gm__ PrimT<T>*)src.GetPhyAddr(), loadDataParams, cacheMode);
    } else if (dstScope == Hardware::L0B) {
        LoadData2DGM2L0BCal(
            (__cb__ PrimT<T>*)dst.GetPhyAddr(), (__gm__ PrimT<T>*)src.GetPhyAddr(), loadDataParams, cacheMode);
    } else if (dstScope == Hardware::L1) {
        LoadData2DGM2L1Cal(
            (__cbuf__ PrimT<T>*)dst.GetPhyAddr(), (__gm__ PrimT<T>*)src.GetPhyAddr(), loadDataParams, cacheMode);
    } else {
        ASCENDC_CHECK_TPOSITION(
            false, "dst", "L1 Buffer(A1/B1)/L0A Buffer(A2)/L0B Buffer(B2)", "LoadData with LoadData2DParamsV2",
            GetPositionDisplay(static_cast<TPosition>(dst.GetPosition())));
    }
#else
    if (dstScope == Hardware::L0A) {
        LoadData2DGM2L0ACal((__ca__ PrimT<T>*)dst.GetPhyAddr(), (__gm__ PrimT<T>*)src.GetPhyAddr(), loadDataParams);
    } else if (dstScope == Hardware::L0B) {
        LoadData2DGM2L0BCal((__cb__ PrimT<T>*)dst.GetPhyAddr(), (__gm__ PrimT<T>*)src.GetPhyAddr(), loadDataParams);
    } else if (dstScope == Hardware::L1) {
        LoadData2DGM2L1Cal((__cbuf__ PrimT<T>*)dst.GetPhyAddr(), (__gm__ PrimT<T>*)src.GetPhyAddr(), loadDataParams);
    }
#endif
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_MM_LOAD2D_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_LOAD2D_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_LOAD2D_IMPL_H__
#endif
