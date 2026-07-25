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
 * \file kernel_operator_data_copy_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_data_copy_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_data_copy_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_INTF_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_DATA_COPY_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_DATA_COPY_INTERFACE_IMPL_H
#include "../../include/basic_api/kernel_tensor.h"
#include "kernel_process_lock.h"
#include "mstx_local_tensor_info.h"

#include "kernel_check.h"
#include "kernel_operator_data_copy_check.h"

#include "kernel_operator_data_copy_base_impl.h"

namespace AscendC {
__aicore__ inline void PrintTimeStamp(uint32_t descId);

/* **************************************************************************************************
 * DataCopy                                             *
 * ************************************************************************************************* */
/*
 * @ingroup DataCopy Level 0
 * @brief datacopy from src to dst, applicable to vector data
 * @param [out] dst output LocalTensor
 * @param [in] src input GlobalTensor
 * @param [in] repeatParams.blockCount number of blocks
 * @param [in] repeatParams.blockLen Length of blocks
 * @param [in] repeatParams.srcGap src block gap
 * @param [in] repeatParams.dstGap dst block gap
 */
template <typename T>
__aicore__ inline void __inout_pipe__(MTE2)
    DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyParams& repeatParams)
{
#ifdef ASCENDC_TIME_STAMP_ON
    PrintTimeStamp(static_cast<uint32_t>(TimeStampId::TIME_STAMP_MTE2_DATACOPY));
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxDataCopyInfo(dst, src, repeatParams, "DataCopy");
#endif
    using PrimType = PrimT<T>;
    const Hardware dstHWPos = GetPhyType((TPosition)dst.GetPosition());
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
    CheckBasicDataCopyTypeSupport<T>("DataCopy from GlobalTensor to LocalTensor with DataCopyParams");
    CheckDataCopyTensor(dst, src, repeatParams, "DataCopy from GlobalTensor to LocalTensor with DataCopyParams");
#endif
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncDataCopy(dst, src, repeatParams, "DataCopy from GlobalTensor to LocalTensor")) {
        ASCENDC_REPORT_CHECK_ERROR("DataCopy from GlobalTensor to LocalTensor", KernelFuncType::NONE_MODE);
    }
    ASCENDC_REPORT_OVERFLOW_MEM(CheckDataCopyTensorSizeOverflow(dst, src, repeatParams));
#endif
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
    const uint8_t cacheMode = ExtractCacheMode(src);
#endif
    if (dstHWPos == Hardware::UB) {
        // gm -> ub
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
        DataCopyGM2UBImpl(
            (__ubuf__ PrimType*)dst.GetPhyAddr(), (__gm__ PrimType*)src.GetPhyAddr(), repeatParams, cacheMode);
#else
        DataCopyGM2UBImpl((__ubuf__ PrimType*)dst.GetPhyAddr(), (__gm__ PrimType*)src.GetPhyAddr(), repeatParams);
#endif
    } else if (dstHWPos == Hardware::L1) {
        // gm -> l1
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
        DataCopyGM2L1Impl(
            (__cbuf__ PrimType*)dst.GetPhyAddr(), (__gm__ PrimType*)src.GetPhyAddr(), repeatParams, cacheMode);
#else
        DataCopyGM2L1Impl((__cbuf__ PrimType*)dst.GetPhyAddr(), (__gm__ PrimType*)src.GetPhyAddr(), repeatParams);
#endif
    } else {
        ASCENDC_CHECK_TPOSITION(
            false, "dst", "L1 Buffer(A1/B1/C1)/UB(VECIN)",
            "DataCopy from GlobalTensor to LocalTensor with DataCopyParams",
            GetPositionDisplay(static_cast<TPosition>(dst.GetPosition())));
    }
}

__aicore__ inline void CheckNd2NzParams(Nd2NzParams params, const __gm__ char* msg)
{
    constexpr uint16_t nd2NzLimit = 16384; // nValue, dstNzC0Stride, dstNzNStride must be in range [0, 16384]
    ASCENDC_CHECK_VALUE_RANGE(params.ndNum, 0, UINT12_MAX, "ndNum", msg);
    ASCENDC_CHECK_VALUE_RANGE(params.nValue, 0, nd2NzLimit, "nValue", msg);
    ASCENDC_CHECK_VALUE_RANGE(params.dstNzC0Stride, 0, nd2NzLimit, "dstNzC0Stride", msg);
    ASCENDC_CHECK_VALUE_RANGE(params.dstNzNStride, 0, nd2NzLimit, "dstNzNStride", msg);
}

/*
 * @ingroup DataCopy Level 0
 * @brief format transform(such as nd2nz) during data load from OUT to L1
 * @param [out] dst output LocalTensor
 * @param [in] src input GlobalTensor
 * @param [in] intriParams.ndNum nd number of data to be moved
 * @param [in] intriParams.nValue n value
 * @param [in] intriParams.dValue d value in unit of element
 * @param [in] intriParams.srcNdMatrixStride stride between nd matrixs at source ND matrix in unit of element
 * @param [in] intriParams.srcDValue SRC_D value in unit of element
 * @param [in] intriParams.dstNzC0Stride stride of nz between 2 C0 in L1 in unit of C0_size
 * @param [in] intriParams.dstNzNStride stride of n between 2 C0 in L1
 * @param [in] intriParams.dstNzMatrixStride DST_nz_matrix_stride in L1 in unit of element
 */
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
template <typename T, bool enableSmallC0>
__aicore__ inline __inout_pipe__(MTE2) void DataCopy(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams)
{
#ifdef ASCENDC_TIME_STAMP_ON
    PrintTimeStamp(static_cast<uint32_t>(TimeStampId::TIME_STAMP_MTE2_DATACOPY));
#endif
    CheckNd2NzParams(intriParams, "DataCopy with Nd2NzParams");
    using PrimType = PrimT<T>;
    const Hardware dstHWPos = GetPhyType((TPosition)dst.GetPosition());
    ASCENDC_REPORT_OVERFLOW_MEM(CheckDataCopyTensorSizeOverflow(dst, src, intriParams));

    // dav_3510 DataCopyGM2L1ND2NZ support small C0 mode and antiquant mode
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
    if constexpr (enableSmallC0) {
        DataCopyGM2L1ND2NZ<T, enableSmallC0>(dst, src, intriParams);
        return;
    }
#endif
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
    const uint8_t cacheMode = ExtractCacheMode(src);
#endif
    if (dstHWPos == Hardware::L1) {
        // gm -> l1
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
        DataCopyGM2L1ND2NZImpl(
            (__cbuf__ PrimType*)dst.GetPhyAddr(), (__gm__ PrimType*)src.GetPhyAddr(), intriParams, cacheMode);
#else
        DataCopyGM2L1ND2NZImpl((__cbuf__ PrimType*)dst.GetPhyAddr(), (__gm__ PrimType*)src.GetPhyAddr(), intriParams);
#endif
    } else if (dstHWPos == Hardware::UB) {
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
        DataCopyGM2UBND2NZImpl(
            (__ubuf__ PrimType*)dst.GetPhyAddr(), (__gm__ PrimType*)src.GetPhyAddr(), intriParams, cacheMode);
#else
        DataCopyGM2UBND2NZImpl((__ubuf__ PrimType*)dst.GetPhyAddr(), (__gm__ PrimType*)src.GetPhyAddr(), intriParams);
#endif
    } else {
        ASCENDC_CHECK_TPOSITION(
            false, "dst", "L1 Buffer(A1/B1)/UB(VECIN)", "DataCopy from GlobalTensor to LocalTensor with Nd2NzParams",
            GetPositionDisplay(static_cast<TPosition>(dst.GetPosition())));
    }
}
#else
template <typename T>
__aicore__ inline __inout_pipe__(MTE2) void DataCopy(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams)
{
#ifdef ASCENDC_TIME_STAMP_ON
    PrintTimeStamp(static_cast<uint32_t>(TimeStampId::TIME_STAMP_MTE2_DATACOPY));
#endif
    CheckNd2NzParams(intriParams, "DataCopy with Nd2NzParams");
    using PrimType = PrimT<T>;
    const Hardware dstHWPos = GetPhyType((TPosition)dst.GetPosition());
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
    CheckNd2NzTypeSupport<T>("DataCopy from GlobalTensor to LocalTensor with Nd2NzParams");
    CheckDataCopyTensor(dst, src, intriParams, "DataCopy from GlobalTensor to LocalTensor with Nd2NzParams");
#endif
#endif
    ASCENDC_REPORT_OVERFLOW_MEM(CheckDataCopyTensorSizeOverflow(dst, src, intriParams));
    if (dstHWPos == Hardware::L1) {
        // gm -> l1
        DataCopyGM2L1ND2NZImpl((__cbuf__ PrimType*)dst.GetPhyAddr(), (__gm__ PrimType*)src.GetPhyAddr(), intriParams);
    } else if (dstHWPos == Hardware::UB) {
        DataCopyGM2UBND2NZImpl((__ubuf__ PrimType*)dst.GetPhyAddr(), (__gm__ PrimType*)src.GetPhyAddr(), intriParams);
    } else {
        ASCENDC_CHECK_TPOSITION(
            false, "dst", "L1 Buffer(A1/B1)/UB(VECIN)", "DataCopy from GlobalTensor to LocalTensor with Nd2NzParams",
            GetPositionDisplay(static_cast<TPosition>(dst.GetPosition())));
    }
}
#endif

#if (defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3510)))
/*
 * @ingroup DataCopy Level 0
 * @brief format transform(such as dn2nz) during data load from OUT to L1
 * @param [out] dst output LocalTensor
 * @param [in] src input GlobalTensor
 * @param [in] intriParams.ndNum nd number of data to be moved
 * @param [in] intriParams.nValue n value
 * @param [in] intriParams.dValue d value in unit of element
 * @param [in] intriParams.srcDnMatrixStride stride between DN matrixs at source DN matrix in unit of element
 * @param [in] intriParams.srcDValue SRC_D value in unit of element
 * @param [in] intriParams.dstNzC0Stride stride of nz between 2 C0 in L1 in unit of C0_size
 * @param [in] intriParams.dstNzNStride stride of n between 2 C0 in L1
 * @param [in] intriParams.dstNzMatrixStride DST_nz_matrix_stride in L1 in unit of element
 */
template <typename T, bool enableSmallC0>
__aicore__ inline __inout_pipe__(MTE2) void DataCopy(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Dn2NzParams& intriParams)
{
#ifdef ASCENDC_TIME_STAMP_ON
    PrintTimeStamp(static_cast<uint32_t>(TimeStampId::TIME_STAMP_MTE2_DATACOPY));
#endif
    using PrimType = PrimT<T>;
    const Hardware dstHWPos = GetPhyType((TPosition)dst.GetPosition());
    if (dstHWPos == Hardware::L1) { // GM -> L1
        const uint8_t cacheMode = ExtractCacheMode(src);
        DataCopyGM2L1DN2NZImpl(
            (__cbuf__ PrimType*)dst.GetPhyAddr(), (__gm__ PrimType*)src.GetPhyAddr(), intriParams, enableSmallC0,
            cacheMode);
        return;
    }
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "DataCopy dn2nz only support position:GM to position: L1"); });
}
#endif

/*
 * @ingroup DataCopy Level 0
 * @brief format transform(such as nd2nz) during data load from UB to L1(Only TSCM)
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] intriParams.ndNum nd number of data to be moved, only can be 1
 * @param [in] intriParams.nValue n value
 * @param [in] intriParams.dValue d value in unit of element
 * @param [in] intriParams.srcNdMatrixStride stride between nd matrixs at source ND matrix in unit of element
 * @param [in] intriParams.srcDValue SRC_D value in unit of element
 * @param [in] intriParams.dstNzC0Stride stride of nz between 2 C0 in L1 in unit of C0_size
 * @param [in] intriParams.dstNzNStride stride of n between 2 C0 in L1
 * @param [in] intriParams.dstNzMatrixStride DST_nz_matrix_stride in L1 in unit of element
 */
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const Nd2NzParams& intriParams)
{
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
    CheckNd2NzTypeSupport<T>("DataCopy from LocalTensor to LocalTensor with Nd2NzParams");
    CheckDataCopyTensor(dst, src, intriParams, "DataCopy from LocalTensor to LocalTensor with Nd2NzParams");
#endif
#endif
    ASCENDC_REPORT_OVERFLOW_MEM(CheckDataCopyTensorSizeOverflow(dst, src, intriParams));
    DataCopyUB2L1ND2NZImpl((__cbuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), intriParams);
}

/*
 * @ingroup DataCopy Level 0
 * @brief datacopy from src to dst, applicable to vector data
 * @param [out] dst output GlobalTensor
 * @param [in] src input LocalTensor
 * @param [in] repeatParams.blockCount number of blocks
 * @param [in] repeatParams.blockLen Length of blocks
 * @param [in] repeatParams.srcGap src block gap
 * @param [in] repeatParams.dstGap dst block gap
 */
template <typename T>
__aicore__ inline __inout_pipe__(MTE3) void DataCopy(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxDataCopyInfo(dst, src, repeatParams, "DataCopy");
#endif
    using PrimType = PrimT<T>;
    const Hardware srcHWPos = GetPhyType((TPosition)src.GetPosition());
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
    CheckBasicDataCopyTypeSupport<T>("DataCopy from LocalTensor to GlobalTensor with DataCopyParams");
    CheckDataCopyTensor(dst, src, repeatParams, "DataCopy from LocalTensor to GlobalTensor with DataCopyParams");
#endif
#endif
#ifdef ASCENDC_CPU_DEBUG
    if (!CheckFuncDataCopy(dst, src, repeatParams, "DataCopy from LocalTensor to GlobalTensor")) {
        ASCENDC_REPORT_CHECK_ERROR("DataCopy from LocalTensor to GlobalTensor", KernelFuncType::NONE_MODE);
    }
    ASCENDC_REPORT_OVERFLOW_MEM(CheckDataCopyTensorSizeOverflow(dst, src, repeatParams));
    bool isUsedProcessLock = false;
    if (g_isAtomic == true) {
        ProcessLock::GetProcessLock()->Write();
        isUsedProcessLock = true;
    }
#endif // ASCENDC_CPU_DEBUG
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
    const uint8_t cacheMode = ExtractCacheMode(dst);
#endif
    if (srcHWPos == Hardware::UB) {
        // ub -> gm
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
        DataCopyUB2GMImpl(
            (__gm__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), repeatParams, cacheMode);
#else
        DataCopyUB2GMImpl((__gm__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), repeatParams);
#endif
    } else if (srcHWPos == Hardware::L1) {
        // l1 -> gm
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
        DataCopyL12GMImpl(
            (__gm__ PrimType*)dst.GetPhyAddr(), (__cbuf__ PrimType*)src.GetPhyAddr(), repeatParams, cacheMode);
#else
        DataCopyL12GMImpl((__gm__ PrimType*)dst.GetPhyAddr(), (__cbuf__ PrimType*)src.GetPhyAddr(), repeatParams);
#endif
    } else {
#if __NPU_ARCH__ == 2002
        ASCENDC_CHECK_TPOSITION(
            false, "src", "L1 Buffer(A1/B1)/UB(CO2/VECOUT)",
            "DataCopy from LocalTensor to GlobalTensor with DataCopyParams",
            GetPositionDisplay(static_cast<TPosition>(src.GetPosition())));
#else
        ASCENDC_CHECK_TPOSITION(
            false, "src", "L1 Buffer(A1/B1)/UB(VECOUT)",
            "DataCopy from LocalTensor to GlobalTensor with DataCopyParams",
            GetPositionDisplay(static_cast<TPosition>(src.GetPosition())));
#endif
    }

#ifdef ASCENDC_CPU_DEBUG
    if (isUsedProcessLock == true) {
        isUsedProcessLock = false;
        ProcessLock::GetProcessLock()->Unlock();
    }
#endif // ASCENDC_CPU_DEBUG
}

/*
 * @ingroup DataCopy Level 0
 * @brief datacopy from src to dst, applicable to vector data
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] repeatParams.blockCount number of blocks
 * @param [in] repeatParams.blockLen Length of blocks
 * @param [in] repeatParams.srcGap src block gap
 * @param [in] repeatParams.dstGap dst block gap
 */
template <typename T>
__aicore__ inline void DataCopy(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxDataCopyInfo(dst, src, repeatParams, "DataCopy");
#endif
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
    CheckBasicDataCopyTypeSupport<T>("DataCopy from LocalTensor to LocalTensor with DataCopyParams");
    CheckDataCopyTensor(dst, src, repeatParams, "DataCopy from LocalTensor to LocalTensor with DataCopyParams");
#endif
#endif

    const Hardware dstHWPos = GetPhyType((TPosition)dst.GetPosition());
    const Hardware srcHWPos = GetPhyType((TPosition)src.GetPosition());

    ASCENDC_REPORT_OVERFLOW_MEM(CheckDataCopyTensorSizeOverflow(dst, src, repeatParams));
    if (srcHWPos == Hardware::UB) {
        if (dstHWPos == Hardware::UB) {
            // ub -> ub
#if ASCENDC_CPU_DEBUG
            if (!CheckFuncDataCopy(dst, src, repeatParams, "DataCopy from LocalTensor to LocalTensor")) {
                ASCENDC_REPORT_CHECK_ERROR("DataCopy from LocalTensor to LocalTensor", KernelFuncType::NONE_MODE);
            }
#endif
            DataCopyUB2UBIntf(dst, src, repeatParams);
        } else if (dstHWPos == Hardware::L1) {
            // ub -> l1
            DataCopyUB2L1Impl((__cbuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), repeatParams);
        } else {
#if __NPU_ARCH__ == 2201
            ASCENDC_CHECK_TPOSITION(
                false, "dst", "UB(VECCALC/VECOUT)/L1 Buffer(TSCM)",
                "DataCopy from LocalTensor(UB(VECIN/VECCALC/VECOUT)) to LocalTensor with DataCopyParams",
                GetPositionDisplay(static_cast<TPosition>(dst.GetPosition())));
#else
            ASCENDC_CHECK_TPOSITION(
                false, "dst", "UB(VECCALC/VECOUT)/L1 Buffer(A1/B1)",
                "DataCopy from LocalTensor(UB(VECIN/VECCALC/VECOUT)) to LocalTensor with DataCopyParams",
                GetPositionDisplay(static_cast<TPosition>(dst.GetPosition())));
#endif
        }
    } else if (srcHWPos == Hardware::L1) {
        if (dstHWPos == Hardware::UB) {
            // l1 -> ub
            DataCopyL12UBIntf(dst, src, repeatParams);
        } else if (dstHWPos == Hardware::BIAS) {
            CheckTensorAlign<T>(dst, 64, "dst", "DataCopy from L1 Buffer(C1) to BiasTable Buffer(C2)"); // 64B align
            CheckTensorAlign<T>(
                src, ONE_BLK_SIZE, "src", "DataCopy from L1 Buffer(C1) to BiasTable Buffer(C2)"); // 32B align
            DataCopyL12BTImpl(
                (uint64_t)dst.GetPhyAddr(), (__cbuf__ PrimType*)src.GetPhyAddr(), static_cast<uint16_t>(0),
                repeatParams);
#if (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 3510) || \
    (__NPU_ARCH__ == 5102)
        } else if (dstHWPos == Hardware::FIXBUF) {
            CheckTensorAlign<T>(
                dst, 128, "dst", "DataCopy from L1 Buffer(A1/B1/C1) to Fixpipe Buffer(C2PIPE2GM)"); // 128B align
            CheckTensorAlign<T>(
                src, ONE_BLK_SIZE, "src",
                "DataCopy from L1 Buffer(A1/B1/C1) to Fixpipe Buffer(C2PIPE2GM)"); // 32B align
            DataCopyL12FBImpl((__fbuf__ PrimType*)dst.GetPhyAddr(), (__cbuf__ PrimType*)src.GetPhyAddr(), repeatParams);
#endif
        } else {
            ASCENDC_CHECK_TPOSITION(
                false, "dst", "BiasTable Buffer(C2)/Fixpipe Buffer(C2PIPE2GM)",
                "DataCopy from LocalTensor(L1 Buffer(A1/B1/C1)) to LocalTensor with DataCopyParams",
                GetPositionDisplay(static_cast<TPosition>(dst.GetPosition())));
        }
    } else {
        ASCENDC_CHECK_TPOSITION(
            false, "src", "UB(VECIN/VECCALC/VECOUT)/L1 Buffer(A1/B1/C1)",
            "DataCopy from LocalTensor to LocalTensor with DataCopyParams",
            GetPositionDisplay(static_cast<TPosition>(src.GetPosition())));
    }
}

/*
 * @ingroup DataCopy Level 0
 * @brief datacopy from L1 to bt, applicable to vector data
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] repeatParams.blockCount number of blocks
 * @param [in] repeatParams.blockLen Length of blocks
 * @param [in] repeatParams.srcGap src block gap
 * @param [in] repeatParams.dstGap dst block gap
 */
template <typename T, typename U>
__aicore__ inline void DataCopy(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& repeatParams)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxDataCopyInfo(dst, src, repeatParams, "DataCopy");
#endif
    using PrimDstType = PrimT<T>;
    using PrimSrcType = PrimT<U>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
    CheckBasicDataCopyMixedTypeSupport<T, U>("DataCopy from LocalTensor to LocalTensor with T / U");
    CheckDataCopyTensor(dst, src, repeatParams, "DataCopy from LocalTensor to LocalTensor with T / U");
#endif
#endif
    const Hardware dstHWPos = GetPhyType((TPosition)dst.GetPosition());
    const Hardware srcHWPos = GetPhyType((TPosition)src.GetPosition());

    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyTensorSizeOverflow<T, U>(dst, src, repeatParams)));
    if (srcHWPos == Hardware::L1) {
        if (dstHWPos == Hardware::BIAS) {
            // l1 -> bt
            CheckTensorAlign<T>(dst, 64, "dst", "DataCopy from L1 Buffer(C1) to BiasTable Buffer(C2)"); // 64B align
            CheckTensorAlign<U>(
                src, ONE_BLK_SIZE, "src", "DataCopy from L1 Buffer(C1) to BiasTable Buffer(C2)"); // 32B align
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5102)
            if constexpr (
                Std::is_same<PrimDstType, int32_t>::value &&
                    (Std::is_same<PrimSrcType, half>::value || Std::is_same<PrimSrcType, bfloat16_t>::value ||
                     Std::is_same<PrimSrcType, float>::value) ||
                Std::is_same<PrimSrcType, int16_t>::value) {
                DataCopyL12BTImpl(
                    (uint64_t)dst.GetPhyAddr(), (__cbuf__ PrimSrcType*)src.GetPhyAddr(), (uint16_t)2, repeatParams);
                return;
            }
#endif
            if constexpr (Std::is_same<PrimSrcType, bfloat16_t>::value && (!Std::is_same<PrimDstType, float>::value)) {
                ASCENDC_ASSERT((false), {
                    KERNEL_LOG(
                        KERNEL_ERROR,
                        "unsupported case where src dtype is bfloat16, dst dtype is not float on current device");
                });
            } else if constexpr (
                Std::is_same<PrimDstType, PrimSrcType>::value ||
                (Std::is_same<PrimSrcType, bfloat16_t>::value && Std::is_same<PrimDstType, float>::value)) {
#else
            if constexpr (Std::is_same<PrimDstType, PrimSrcType>::value) {
#endif
                DataCopyL12BTImpl(
                    (uint64_t)dst.GetPhyAddr(), (__cbuf__ PrimSrcType*)src.GetPhyAddr(), static_cast<uint16_t>(0),
                    repeatParams);
            } else if constexpr (Std::is_same<PrimDstType, float>::value && Std::is_same<PrimSrcType, half>::value) {
                DataCopyL12BTImpl(
                    (uint64_t)dst.GetPhyAddr(), (__cbuf__ half*)src.GetPhyAddr(), static_cast<uint16_t>(1),
                    repeatParams);
            } else {
                ASCENDC_ASSERT(false, {
                    KERNEL_LOG(
                        KERNEL_ERROR, "Failed to check dtype in DataCopy from L1 Buffer(C1) to BiasTable Buffer(C2), "
                                      "current api support dtype combination is U = T or src: half, dst: float.");
                });
            }
        } else {
            ASCENDC_CHECK_TPOSITION(
                false, "dst", "BiasTable Buffer(C2)", "DataCopy from LocalTensor to LocalTensor with T / U",
                GetPositionDisplay(static_cast<TPosition>(dst.GetPosition())));
        }
    } else {
        ASCENDC_CHECK_TPOSITION(
            false, "src", "L1 Buffer(C1)", "DataCopy from LocalTensor to LocalTensor with T / U",
            GetPositionDisplay(static_cast<TPosition>(src.GetPosition())));
    }
}

template <typename T, uint8_t subBlockId>
__aicore__ inline void DataCopyL1ToUB(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
{
#if (defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510))
    using PrimType = PrimT<T>;
    ASCENDC_REPORT_OVERFLOW_MEM(CheckDataCopyTensorSizeOverflow(dst, src, repeatParams));
    CheckTensorAlign<T>(dst, ONE_BLK_SIZE, "dst", "DataCopy from L1 Buffer(A1/B1) to UB(VECIN/VECOUT)"); // 32B align
    CheckTensorAlign<T>(src, ONE_BLK_SIZE, "src", "DataCopy from L1 Buffer(A1/B1) to UB(VECIN/VECOUT)"); // 32B align
    DataCopyL12UBImpl<PrimType, subBlockId>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__cbuf__ PrimType*)src.GetPhyAddr(), repeatParams);
#endif
}

/*
 * @ingroup Copy Level 0
 * @brief datacopy from src to dst, applicable to vector data
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] repeatParams.dstStride dst block stride
 * @param [in] repeatParams.srcStride src block stride
 * @param [in] repeatParams.dstRepeatSize dst repeat stride
 * @param [in] repeatParams.srcRepeatSize src repeat stride
 */
// Copy::Level 0 - mask bit mode
template <typename T, bool IsSetMask>
__aicore__ inline __inout_pipe__(V) void Copy(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint64_t mask[], const uint8_t repeatTime,
    const CopyRepeatParams& repeatParams)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecCopyInfo(dst, src, mask[0], mask[1], repeatTime, repeatParams, IsSetMask, "Copy");
#endif
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(IsSetMask);
    if (!CheckFuncCopy(dst, src, mask, repeatTime, repeatParams, "Copy")) {
        ASCENDC_REPORT_CHECK_ERROR("Copy", KernelFuncType::MASK_BIT_MODE);
    }
#endif
    CopyImpl<PrimType, IsSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), mask, repeatTime, repeatParams);
}

// Copy::Level 0 - mask count mode
template <typename T, bool IsSetMask>
__aicore__ inline __inout_pipe__(V) void Copy(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint64_t mask, const uint8_t repeatTime,
    const CopyRepeatParams& repeatParams)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecCopyInfo(dst, src, mask, repeatTime, repeatParams, IsSetMask, "Copy");
#endif
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    MaskSetter::Instance().SetMask(IsSetMask);
    if (!CheckFuncCopy(dst, src, mask, repeatTime, repeatParams, "Copy")) {
        ASCENDC_REPORT_CHECK_ERROR("Copy", KernelFuncType::MASK_COUNT_MODE);
    }
#endif
    CopyImpl<PrimType, IsSetMask>(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), mask, repeatTime, repeatParams);
}

/*
 * @ingroup Copy Level 2
 * @brief datacopy from src to dst, applicable to vector data
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] count copy count
 */
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
template <typename T, bool isSetMask>
__aicore__ inline __inout_pipe__(V) void Copy(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
{
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncCopy(dst, src, count, "Copy")) {
        ASCENDC_REPORT_CHECK_ERROR("Copy", KernelFuncType::CALCOUNT_MODE);
    }
#endif
    CopyImpl<PrimType>((__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), count);
}
#endif

/*
 * @ingroup DataCopy Level 1
 * @brief datacopy from src to dst, applicable to vector data
 * @param [out] dst output LocalTensor
 * @param [in] src input GlobalTensor
 * @param [in] SliceInfo dstSliceInfo[] ub
 * @param [in] SliceInfo srcSliceInfo[] gm
 * @param [in] dimValue dim value also for length for dstSliceInfo[] and srcSliceInfo[]
 */
template <typename T>
__aicore__ inline __inout_pipe__(MTE2) void DataCopy(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const SliceInfo dstSliceInfo[],
    const SliceInfo srcSliceInfo[], const uint32_t dimValue)
{
#ifdef ASCENDC_TIME_STAMP_ON
    PrintTimeStamp(static_cast<uint32_t>(TimeStampId::TIME_STAMP_MTE2_DATACOPY));
#endif
    using PrimType = PrimT<T>;
    static_assert(Std::is_same<PrimType, T>::value, "TensorTrait is not supported by DataCopy with SliceInfo!");
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncDataCopySlice(dst, src, dstSliceInfo, srcSliceInfo, dimValue, "DataCopy with SliceInfo")) {
        ASCENDC_REPORT_CHECK_ERROR("DataCopy with SliceInfo", KernelFuncType::NONE_MODE);
    }
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyTensorSizeOverflow(dst, src, dstSliceInfo, srcSliceInfo, dimValue)));
#endif
    uint32_t srcStartIndex = 0;
    uint32_t dstStartIndex = 0;
    uint32_t srcOffsetListSize = 0;
    uint32_t dstOffsetListSize = 0;
    uint32_t srcShapeInfo[K_MAX_SHAPE_DIM];
    uint32_t dstShapeInfo[K_MAX_SHAPE_DIM];
    bool useShapeValue = !(srcSliceInfo[0].shapeValue == 0);
    for (int i = 0; i < dimValue; i++) {
        srcShapeInfo[i] = useShapeValue ? srcSliceInfo[i].shapeValue : src.GetShapeInfo().shape[i];
        dstShapeInfo[i] = useShapeValue ? dstSliceInfo[i].shapeValue : dst.GetShapeInfo().shape[i];
    }

    srcStartIndex = DataCopyGetPhyStartIndex(srcSliceInfo, srcShapeInfo, dimValue);
    dstStartIndex = DataCopyGetPhyStartIndex(dstSliceInfo, dstShapeInfo, dimValue);
    uint32_t srcOffsetList[MAX_SLICE_SIZE];
    uint32_t dstOffsetList[MAX_SLICE_SIZE];
    DataCopyGetOffsetList(srcSliceInfo, srcShapeInfo, dimValue, &srcOffsetListSize, srcOffsetList);
    DataCopyGetOffsetList(dstSliceInfo, dstShapeInfo, dimValue, &dstOffsetListSize, dstOffsetList);
    struct DataCopyParams repeatParams;
    repeatParams.blockLen = srcSliceInfo[0].burstLen;
    uint32_t oneSliceLen = srcSliceInfo[0].burstLen * AscendCUtils::GetC0Count(sizeof(T)) + srcSliceInfo[0].stride;
    repeatParams.blockCount =
        (srcSliceInfo[0].endIndex - srcSliceInfo[0].startIndex + 1 + srcSliceInfo[0].stride) / oneSliceLen;
    repeatParams.dstStride = dstSliceInfo[0].stride * sizeof(T) / AscendCUtils::GetC0Size();

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
    const uint8_t cacheMode = ExtractCacheMode(src);
#endif
    if ((srcSliceInfo[0].stride * sizeof(T)) % AscendCUtils::GetC0Size() == 0) {
        repeatParams.srcStride = srcSliceInfo[0].stride * sizeof(T) / AscendCUtils::GetC0Size();
        for (uint32_t i = 0; i < srcOffsetListSize; i++) {
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
            DataCopyGM2UBImpl(
                (__ubuf__ T*)dst.GetPhyAddr() + dstStartIndex + dstOffsetList[i],
                (__gm__ T*)src.GetPhyAddr() + srcStartIndex + srcOffsetList[i], repeatParams, cacheMode);
#else
            DataCopyGM2UBImpl(
                (__ubuf__ T*)dst.GetPhyAddr() + dstStartIndex + dstOffsetList[i],
                (__gm__ T*)src.GetPhyAddr() + srcStartIndex + srcOffsetList[i], repeatParams);
#endif
        }
    } else {
        repeatParams.srcStride = srcSliceInfo[0].stride * sizeof(T);
        for (uint32_t i = 0; i < srcOffsetListSize; i++) {
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
            DataCopySliceGm2UBImpl(
                (__ubuf__ T*)dst.GetPhyAddr() + dstStartIndex + dstOffsetList[i],
                (__gm__ T*)src.GetPhyAddr() + srcStartIndex + srcOffsetList[i], repeatParams, cacheMode);
#else
            DataCopySliceGm2UBImpl(
                (__ubuf__ T*)dst.GetPhyAddr() + dstStartIndex + dstOffsetList[i],
                (__gm__ T*)src.GetPhyAddr() + srcStartIndex + srcOffsetList[i], repeatParams);
#endif
        }
    }
}

/*
 * @ingroup DataCopy Level 1
 * @brief datacopy from src to dst, applicable to vector data
 * @param [out] dst output LocalTensor
 * @param [in] src input GlobalTensor
 * @param [in] SliceInfo dstSliceInfo[] gm
 * @param [in] SliceInfo srcSliceInfo[] ub
 * @param [in] dimValue dim value also for length for dstSliceInfo[] and srcSliceInfo[]
 */
template <typename T>
__aicore__ inline __inout_pipe__(MTE3) void DataCopy(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const SliceInfo dstSliceInfo[],
    const SliceInfo srcSliceInfo[], const uint32_t dimValue)
{
    using PrimType = PrimT<T>;
    static_assert(Std::is_same<PrimType, T>::value, "TensorTrait is not supported by DataCopy with SliceInfo!");
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncDataCopySlice(dst, src, dstSliceInfo, srcSliceInfo, dimValue, "DataCopy with SliceInfo")) {
        ASCENDC_REPORT_CHECK_ERROR("DataCopy with SliceInfo", KernelFuncType::NONE_MODE);
    }
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyTensorSizeOverflow(dst, src, dstSliceInfo, srcSliceInfo, dimValue)));
#endif
    uint32_t srcStartIndex = 0;
    uint32_t dstStartIndex = 0;
    uint32_t srcOffsetListSize = 0;
    uint32_t dstOffsetListSize = 0;
    uint32_t srcShapeInfo[K_MAX_SHAPE_DIM];
    uint32_t dstShapeInfo[K_MAX_SHAPE_DIM];
    bool useShapeValue = !(srcSliceInfo[0].shapeValue == 0);
    for (int i = 0; i < dimValue; i++) {
        srcShapeInfo[i] = useShapeValue ? srcSliceInfo[i].shapeValue : src.GetShapeInfo().shape[i];
        dstShapeInfo[i] = useShapeValue ? dstSliceInfo[i].shapeValue : dst.GetShapeInfo().shape[i];
    }

    srcStartIndex = DataCopyGetPhyStartIndex(srcSliceInfo, srcShapeInfo, dimValue);
    dstStartIndex = DataCopyGetPhyStartIndex(dstSliceInfo, dstShapeInfo, dimValue);
    uint32_t dstOffsetList[MAX_SLICE_SIZE];
    uint32_t srcOffsetList[MAX_SLICE_SIZE];
    DataCopyGetOffsetList(srcSliceInfo, srcShapeInfo, dimValue, &srcOffsetListSize, srcOffsetList);
    DataCopyGetOffsetList(dstSliceInfo, dstShapeInfo, dimValue, &dstOffsetListSize, dstOffsetList);

    struct DataCopyParams repeatParams;
    repeatParams.blockLen = srcSliceInfo[0].burstLen;
    uint32_t oneSliceLen = srcSliceInfo[0].burstLen * AscendCUtils::GetC0Count(sizeof(T)) + srcSliceInfo[0].stride;
    repeatParams.blockCount =
        (srcSliceInfo[0].endIndex - srcSliceInfo[0].startIndex + 1 + srcSliceInfo[0].stride) / oneSliceLen;
    repeatParams.srcStride = srcSliceInfo[0].stride * sizeof(T) / AscendCUtils::GetC0Size();

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
    const uint8_t cacheMode = ExtractCacheMode(dst);
#endif
    if ((dstSliceInfo[0].stride * sizeof(T)) % AscendCUtils::GetC0Size() == 0) {
        repeatParams.dstStride = dstSliceInfo[0].stride * sizeof(T) / AscendCUtils::GetC0Size();
        for (uint32_t i = 0; i < srcOffsetListSize; i++) {
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
            DataCopyUB2GMImpl(
                (__gm__ T*)dst.GetPhyAddr() + dstStartIndex + dstOffsetList[i],
                (__ubuf__ T*)src.GetPhyAddr() + srcStartIndex + srcOffsetList[i], repeatParams, cacheMode);
#else
            DataCopyUB2GMImpl(
                (__gm__ T*)dst.GetPhyAddr() + dstStartIndex + dstOffsetList[i],
                (__ubuf__ T*)src.GetPhyAddr() + srcStartIndex + srcOffsetList[i], repeatParams);
#endif
        }
    } else {
        repeatParams.dstStride = dstSliceInfo[0].stride * sizeof(T);
        for (uint32_t i = 0; i < srcOffsetListSize; i++) {
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
            DataCopySliceUB2GMImpl(
                (__gm__ T*)dst.GetPhyAddr() + dstStartIndex + dstOffsetList[i],
                (__ubuf__ T*)src.GetPhyAddr() + srcStartIndex + srcOffsetList[i], repeatParams, cacheMode);
#else
            DataCopySliceUB2GMImpl(
                (__gm__ T*)dst.GetPhyAddr() + dstStartIndex + dstOffsetList[i],
                (__ubuf__ T*)src.GetPhyAddr() + srcStartIndex + srcOffsetList[i], repeatParams);
#endif
        }
    }
}

template <typename T>
__aicore__ inline void DataCopyCheck(const uint32_t count, DataCopyParams& repeatParams)
{
    using PrimType = PrimT<T>;
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5102)
    if constexpr (Std::is_same<PrimType, int2b_t>::value) {
        ASCENDC_ASSERT((count % ConstantsInternal::ONE_BLK_B2_NUM == 0), {
            KERNEL_LOG(
                KERNEL_ERROR, "DataCopy count is %d, which should be 32B align. \
                    In NPU mode, no error is reported. The value is rounded down by 32B.",
                count);
        });
        repeatParams.blockLen = count / ConstantsInternal::ONE_BLK_B2_NUM;
    } else if constexpr (Std::is_same<PrimType, uint1b_t>::value) {
        ASCENDC_ASSERT((count % ConstantsInternal::ONE_BLK_B1_NUM == 0), {
            KERNEL_LOG(
                KERNEL_ERROR, "DataCopy count is %d, which should be 32B align. \
                    In NPU mode, no error is reported. The value is rounded down by 32B.",
                count);
        });
        repeatParams.blockLen = count / ConstantsInternal::ONE_BLK_B1_NUM;
    } else if constexpr (
        Std::is_same<PrimType, fp4x2_e2m1_t>::value || Std::is_same<PrimType, fp4x2_e1m2_t>::value ||
        Std::is_same<PrimType, int4b_t>::value) {
        ASCENDC_ASSERT((count % ConstantsInternal::ONE_BLK_FP4_NUM == 0), {
            KERNEL_LOG(
                KERNEL_ERROR, "DataCopy count is %d, which should be 32B align. \
                    In NPU mode, no error is reported. The value is rounded down by 32B.",
                count);
        });
        repeatParams.blockLen = count / ConstantsInternal::ONE_BLK_FP4_NUM;
    } else
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    if constexpr (Std::is_same<PrimType, fp4x2_e2m1_t>::value || Std::is_same<PrimType, fp4x2_e1m2_t>::value) {
        ASCENDC_ASSERT((count % ConstantsInternal::ONE_BLK_FP4_NUM == 0), {
            KERNEL_LOG(
                KERNEL_ERROR, "DataCopy count is %d, which should be 32B align. \
                    In NPU mode, no error is reported. The value is rounded down by 32B.",
                count);
        });
        repeatParams.blockLen = count / ConstantsInternal::ONE_BLK_FP4_NUM;
    } else
#endif
    {
        ASCENDC_ASSERT((count % AscendCUtils::GetC0Count(sizeof(PrimType)) == 0), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "Failed to "
                "check count value in DataCopy from GlobalTensor to LocalTensor or from LocalTensor to GlobalTensor, "
                "count * sizeof(T) must be 32B align, current count value is %u. "
                "In NPU mode, no error is reported. The value is rounded down by 32B.",
                count);
        });
        repeatParams.blockLen = count / AscendCUtils::GetC0Count(sizeof(PrimType));
    }
}

/*
 * @ingroup DataCopy Level 2
 * @brief datacopy from src to dst, applicable to vector data
 * @param [out] dst output LocalTensor
 * @param [in] src input GlobalTensor
 * @param [in] count Number of operands
 */
template <typename T>
__aicore__ inline __inout_pipe__(MTE2) void DataCopy(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const uint32_t count)
{
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyTensorSizeOverflow(dst, src, count)));
    struct DataCopyParams repeatParams;
    DataCopyCheck<T>(count, repeatParams);
    DataCopy(dst, src, repeatParams);
}

/*
 * @ingroup DataCopy Level 2
 * @brief datacopy from src to dst, applicable to vector data
 * @param [out] dst output GlobalTensor
 * @param [in] src input LocalTensor
 * @param [in] count Number of operands
 */
template <typename T>
__aicore__ inline __inout_pipe__(MTE3) void DataCopy(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
{
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyTensorSizeOverflow(dst, src, count)));
    struct DataCopyParams repeatParams;
    DataCopyCheck<T>(count, repeatParams);
    DataCopy(dst, src, repeatParams);
}

/*
 * @ingroup DataCopy Level 2
 * @brief datacopy from src to dst, applicable to vector data
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] count Number of operands
 */
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
{
    using PrimType = PrimT<T>;
    ASCENDC_ASSERT((count % AscendCUtils::GetC0Count(sizeof(PrimType)) == 0), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "Failed to "
            "check count value in DataCopy from LocalTensor to LocalTensor, count * sizeof(T) must be 32B align, "
            "current count value is %u. In NPU mode, no error is reported. The value is rounded down by 32B.",
            count);
    });
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyTensorSizeOverflow(dst, src, count)));
    struct DataCopyParams repeatParams;

    const Hardware dstHWPos = GetPhyType((TPosition)dst.GetPosition());
    const Hardware srcHWPos = GetPhyType((TPosition)src.GetPosition());
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
    repeatParams.blockLen = count / AscendCUtils::GetC0Count(sizeof(T));
#else
    if (srcHWPos != Hardware::L1) { // UB -> UB, UB -> L1
        repeatParams.blockLen = count / AscendCUtils::GetC0Count(sizeof(PrimType));
    } else { // L1 -> UB, L1 -> BT, L1 -> FB
        if (dstHWPos == Hardware::UB) {
            repeatParams.blockLen = count / AscendCUtils::GetC0Count(sizeof(PrimType));
        } else if (dstHWPos == Hardware::BIAS) {
#if (__NPU_ARCH__ == 3510)
            repeatParams.blockLen = count / (32 / sizeof(PrimType)); // BT blockLen is in unit of 32B
#else
            repeatParams.blockLen = count / (64 / sizeof(PrimType)); // BT blockLen is in unit of 64B
#endif
        } else if (dstHWPos == Hardware::FIXBUF) {
            repeatParams.blockLen = count / (128 / sizeof(PrimType)); // FB blockLen is in unit of 128B
        }
    }
#endif
    DataCopy(dst, src, repeatParams);
}

template <typename T, uint8_t subBlockId>
__aicore__ inline void DataCopyL1ToUB(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
{
#if (defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510))
    using PrimType = PrimT<T>;
    ASCENDC_ASSERT((count % AscendCUtils::GetC0Count(sizeof(PrimType)) == 0), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "Failed to "
            "check count value in DataCopy from LocalTensor to LocalTensor, count * sizeof(T) must be 32B align, "
            "current count value is %u. In NPU mode, no error is reported. The value is rounded down by 32B.",
            count);
    });
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyTensorSizeOverflow(dst, src, count)));
    struct DataCopyParams repeatParams;
    repeatParams.blockLen = count / AscendCUtils::GetC0Count(sizeof(PrimType));
    DataCopyL1ToUB<T, subBlockId>(dst, src, repeatParams);
#endif
}

__aicore__ inline void CheckNz2NdParams(const Nz2NdParamsFull& params)
{
    constexpr uint16_t nz2NdLimit = 8192; // nValue, dstNzC0Stride, dstNzNStride must be in range [0, 16384]
    ASCENDC_CHECK_VALUE_RANGE(params.ndNum, 0, UINT12_MAX, "ndNum", "DataCopy with Nz2NdParamsFull");
    ASCENDC_CHECK_VALUE_RANGE(params.nValue, 1, nz2NdLimit, "nValue", "DataCopy with Nz2NdParamsFull");
    ASCENDC_CHECK_VALUE_RANGE(params.dValue, 1, nz2NdLimit, "dValue", "DataCopy with Nz2NdParamsFull");
    ASCENDC_CHECK_VALUE_RANGE(
        params.srcNdMatrixStride, 1, VALUE_512, "srcNdMatrixStride", "DataCopy with Nz2NdParamsFull");
    ASCENDC_CHECK_VALUE_RANGE(params.srcNStride, 0, UINT12_MAX, "srcNStride", "DataCopy with Nz2NdParamsFull");
}

/*
 * @ingroup DataCopy Level 2
 * @brief datacopy from src to dst, nz2nd, applicable to simulated cube data(such as data from l0c, 16*16)
 * @param [out] dst output GlobalTensor
 * @param [in] src input LocalTensor
 */
template <typename T>
__aicore__ inline __inout_pipe__(MTE3) void DataCopy(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const Nz2NdParamsFull& intriParams)
{
    CheckNz2NdParams(intriParams);
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
    CheckDataCopyTensor(dst, src, intriParams, "DataCopy with Nz2NdParamsFull");
#endif
#endif
#ifdef ASCENDC_CPU_DEBUG
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyTensorSizeOverflow(dst, src, intriParams)));
    bool isUsedProcessLock = false;
    if (g_isAtomic == true) {
        ProcessLock::GetProcessLock()->Write();
        isUsedProcessLock = true;
    }
#endif // ASCENDC_CPU_DEBUG
    const Hardware srcHWPos = GetPhyType((TPosition)src.GetPosition());
    if (srcHWPos != Hardware::UB) {
#if __NPU_ARCH__ == 2002
        ASCENDC_CHECK_TPOSITION(
            false, "src", "UB(VECOUT/CO2)", "DataCopy with Nz2NdParamsFull",
            GetPositionDisplay(static_cast<TPosition>(src.GetPosition())));
#else
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
        if (srcHWPos == Hardware::L1) {
            DataCopyL12GMNZ2NDImpl((__gm__ T*)dst.GetPhyAddr(), (__cbuf__ T*)src.GetPhyAddr(), intriParams);
            return;
        }
#endif
        ASCENDC_CHECK_TPOSITION(
            false, "src", "UB(VECOUT)", "DataCopy with Nz2NdParamsFull",
            GetPositionDisplay(static_cast<TPosition>(src.GetPosition())));
#endif
    }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    const uint8_t cacheMode = ExtractCacheMode(dst);
    DataCopyUB2GMNZ2NDImpl(
        (__gm__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), intriParams, cacheMode);
#else
    DataCopyUB2GMNZ2NDImpl((__gm__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), intriParams);
#endif
#ifdef ASCENDC_CPU_DEBUG
    if (isUsedProcessLock == true) {
        isUsedProcessLock = false;
        ProcessLock::GetProcessLock()->Unlock();
    }
#endif // ASCENDC_CPU_DEBUG
}

/* **************************************************************************************************
 * DataCopy Enhanced                                             *
 * ************************************************************************************************* */
/*
 * @ingroup DataCopy
 * @brief datacopy from src to dst, applicable to cube data
 * @param [out] dst output LocalTensor
 * @param [in] src input GlobalTensor
 * @param [in] intriParams.blockCount number of blocks
 * @param [in] intriParams.blockLen Length of blocks
 * @param [in] intriParams.srcGap src block gap
 * @param [in] intriParams.dstGap dst block gap
 * @param [in] enhancedParams.blockMode Basic fractal of data movement
 * @param [in] enhancedParams.deqScale Auxiliary parameters for path accuracy conversion
 * @param [in] enhancedParams.deqValue size of convert with path precision
 * @param [in] enhancedParams.sidStoreMode Multiplex input
 * @param [in] enhancedParams.isRelu Configure whether Relu can be performed along the circuit
 */
template <typename T>
__aicore__ inline __inout_pipe__(MTE2) void DataCopy(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#ifdef ASCENDC_TIME_STAMP_ON
    PrintTimeStamp(static_cast<uint32_t>(TimeStampId::TIME_STAMP_MTE2_DATACOPY));
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxDataCopyInfo(dst, src, intriParams, "DataCopy");
#endif
    using PrimType = PrimT<T>;
    const Hardware dstHWPos = GetPhyType((TPosition)dst.GetPosition());
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyTensorSizeOverflow(dst, src, intriParams, enhancedParams)));

    if (dstHWPos == Hardware::UB) {
        // gm -> ub
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
        const uint8_t cacheMode = ExtractCacheMode(src);
        DataCopyGM2UBImpl(
            (__ubuf__ PrimType*)dst.GetPhyAddr(), (__gm__ PrimType*)src.GetPhyAddr(), intriParams, cacheMode);
#else
        DataCopyGM2UBImpl((__ubuf__ PrimType*)dst.GetPhyAddr(), (__gm__ PrimType*)src.GetPhyAddr(), intriParams);
#endif
    } else if (dstHWPos == Hardware::L1) {
        // gm -> l1
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
        const uint8_t cacheMode = ExtractCacheMode(src);
        DataCopyGM2L1Impl(
            (__cbuf__ PrimType*)dst.GetPhyAddr(), (__gm__ PrimType*)src.GetPhyAddr(), intriParams, cacheMode);
#else
        DataCopyGM2L1Impl((__cbuf__ PrimType*)dst.GetPhyAddr(), (__gm__ PrimType*)src.GetPhyAddr(), intriParams);
#endif
    } else {
        ASCENDC_CHECK_TPOSITION(
            false, "dst", "L1 Buffer(A1/B1)/UB(VECIN)",
            "DataCopy from GlobalTensor to LocalTensor with DataCopyEnhancedParams",
            GetPositionDisplay(static_cast<TPosition>(dst.GetPosition())));
    }
}

template <typename T>
__aicore__ inline __inout_pipe__(MTE3) void DataCopy(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxDataCopyInfo(dst, src, intriParams, "DataCopy");
#endif
    using PrimType = PrimT<T>;
    const Hardware srcHWPos = GetPhyType((TPosition)src.GetPosition());
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyTensorSizeOverflow(dst, src, intriParams, enhancedParams)));

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    const uint8_t cacheMode = ExtractCacheMode(dst);
#endif

    if (srcHWPos == Hardware::UB) {
        // ub -> gm
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
        DataCopyUB2GMImpl(
            (__gm__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), intriParams, cacheMode);
#else
        DataCopyUB2GMImpl((__gm__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), intriParams);
#endif
    } else if (srcHWPos == Hardware::L1) {
        // l1 -> gm
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
        DataCopyL12GMImpl(
            (__gm__ PrimType*)dst.GetPhyAddr(), (__cbuf__ PrimType*)src.GetPhyAddr(), intriParams, cacheMode);
#else
        DataCopyL12GMImpl((__gm__ PrimType*)dst.GetPhyAddr(), (__cbuf__ PrimType*)src.GetPhyAddr(), intriParams);
#endif
    } else {
        ASCENDC_CHECK_TPOSITION(
            false, "src", "L1 Buffer(A1/B1)/UB(VECOUT)",
            "DataCopy from LocalTensor to GlobalTensor with DataCopyEnhancedParams",
            GetPositionDisplay(static_cast<TPosition>(src.GetPosition())));
    }
}

template <typename T>
__aicore__ inline void DataCopy(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxDataCopyInfo(dst, src, intriParams, "DataCopy");
#endif
    using PrimType = PrimT<T>;
    const Hardware dstHWPos = GetPhyType((TPosition)dst.GetPosition());
    const Hardware srcHWPos = GetPhyType((TPosition)src.GetPosition());
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyTensorSizeOverflow(dst, src, intriParams, enhancedParams)));

    if (srcHWPos == Hardware::UB) {
        if (dstHWPos == Hardware::L1) {
            // ub -> l1
            DataCopyUB2L1Impl((__cbuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), intriParams);
        } else if (dstHWPos == Hardware::L0C) {
            // ub -> l0c
            DataCopyUB2L0CIntf(dst, src, intriParams, enhancedParams);
        } else if (dstHWPos == Hardware::UB) {
            // ub -> ub
            DataCopyUB2UBIntf(dst, src, intriParams);
        } else {
            ASCENDC_CHECK_TPOSITION(
                false, "dst", "UB(VECCALC/VECOUT)/L1 Buffer(A1/B1/TSCM)",
                "DataCopy from LocalTensor(UB(VECIN/VECCALC/VECOUT)) to LocalTensor with DataCopyEnhancedParams",
                GetPositionDisplay(static_cast<TPosition>(dst.GetPosition())));
        }
    } else if (srcHWPos == Hardware::L1) {
        if (dstHWPos == Hardware::UB) {
            // l1 -> ub
            DataCopyL12UBIntf(dst, src, intriParams);
        } else if (dstHWPos == Hardware::L0C) {
            // l1 -> l0c
            DataCopyL12L0CImpl(
                (__cc__ PrimType*)dst.GetPhyAddr(), (__cbuf__ PrimType*)src.GetPhyAddr(), intriParams, enhancedParams);
        } else {
            ASCENDC_CHECK_TPOSITION(
                false, "dst", "L0C Buffer(CO1)",
                "DataCopy from LocalTensor(L1 Buffer(A1/B1)) to LocalTensor with DataCopyEnhancedParams",
                GetPositionDisplay(static_cast<TPosition>(dst.GetPosition())));
        }
    } else if (srcHWPos == Hardware::L0C) {
        if (dstHWPos == Hardware::UB) {
            // l0c -> ub
            DataCopyL0C2UBImpl(
                (__ubuf__ PrimType*)dst.GetPhyAddr(), (__cc__ PrimType*)src.GetPhyAddr(), intriParams, enhancedParams);
        } else {
            ASCENDC_CHECK_TPOSITION(
                false, "dst", "UB(CO2)",
                "DataCopy from LocalTensor(L0C Buffer(CO1)) to LocalTensor with DataCopyEnhancedParams",
                GetPositionDisplay(static_cast<TPosition>(dst.GetPosition())));
        }
    } else {
#if __NPU_ARCH__ == 2002
        ASCENDC_CHECK_TPOSITION(
            false, "src", "UB(VECIN)/L0C Buffer(CO1)",
            "DataCopy from LocalTensor to LocalTensor with DataCopyEnhancedParams",
            GetPositionDisplay(static_cast<TPosition>(src.GetPosition())));
#else
        ASCENDC_CHECK_TPOSITION(
            false, "src", "UB(VECIN)", "DataCopy from LocalTensor to LocalTensor with DataCopyEnhancedParams",
            GetPositionDisplay(static_cast<TPosition>(src.GetPosition())));
#endif
    }
}

template <typename T, typename U>
__aicore__ inline void DataCopy(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyCO12DstParams& intriParams)
{
    CheckTensorPos<U>(
        src, Hardware::L0C, "src", "CO1", "DataCopy from LocalTensor to LocalTensor with DataCopyCO12DstParams");
    CheckTensorPos<T>(
        dst, Hardware::L1, "dst", "A1", "DataCopy from LocalTensor to LocalTensor with DataCopyCO12DstParams");
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyTensorSizeOverflow(dst, src, intriParams)));
    // l0c -> l1
    DataCopyL0C2L1Impl((__cbuf__ PrimT<T>*)dst.GetPhyAddr(), (__cc__ PrimT<U>*)src.GetPhyAddr(), intriParams);
}

template <typename T, typename U>
__aicore__ inline void DataCopy(
    const GlobalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyCO12DstParams& intriParams)
{
    CheckTensorPos<U>(
        src, Hardware::L0C, "src", "CO1", "DataCopy from LocalTensor to GlobalTensor with DataCopyCO12DstParams");
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyTensorSizeOverflow(dst, src, intriParams)));
    // l0c -> gm
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    const uint8_t cacheMode = ExtractCacheMode(dst);
    DataCopyL0C2GMImpl((__gm__ PrimT<T>*)dst.GetPhyAddr(), (__cc__ PrimT<U>*)src.GetPhyAddr(), intriParams, cacheMode);
#else
    DataCopyL0C2GMImpl((__gm__ PrimT<T>*)dst.GetPhyAddr(), (__cc__ PrimT<U>*)src.GetPhyAddr(), intriParams);
#endif
}

#if (defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201))
// float to bfloat16_t
template <
    typename T, typename U,
    typename Std::enable_if<
        Std::is_same<PrimT<T>, bfloat16_t>::value && Std::is_same<PrimT<U>, float>::value, bool>::type>
__aicore__ inline void DataCopy(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxDataCopyInfo(dst, src, intriParams, "DataCopy");
#endif
    const Hardware dstHWPos = GetPhyType((TPosition)dst.GetPosition());
    const Hardware srcHWPos = GetPhyType((TPosition)src.GetPosition());
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyTensorSizeOverflow(dst, src, intriParams, enhancedParams)));
    if (srcHWPos == Hardware::L1) {
        if (dstHWPos == Hardware::L0C) {
            // l1 -> l0c
            DataCopyL12L0CImpl(
                (__cc__ PrimT<T>*)dst.GetPhyAddr(), (__cbuf__ PrimT<U>*)src.GetPhyAddr(), intriParams, enhancedParams);
        } else {
            ASCENDC_CHECK_TPOSITION(
                false, "dst", "L0C Buffer(CO1)",
                "DataCopy from LocalTensor(U) to LocalTensor(T) with DataCopyEnhancedParams",
                GetPositionDisplay(static_cast<TPosition>(dst.GetPosition())));
        }
    } else {
        ASCENDC_CHECK_TPOSITION(
            false, "src", "L1 Buffer(A1/B1)",
            "DataCopy from LocalTensor(U) to LocalTensor(T) with DataCopyEnhancedParams",
            GetPositionDisplay(static_cast<TPosition>(src.GetPosition())));
    }
}
#endif

// float to half
template <
    typename T, typename U,
    typename Std::enable_if<Std::is_same<PrimT<T>, half>::value && Std::is_same<PrimT<U>, float>::value, bool>::type>
__aicore__ inline void DataCopy(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxDataCopyInfo(dst, src, intriParams, "DataCopy");
#endif
    const Hardware dstHWPos = GetPhyType((TPosition)dst.GetPosition());
    const Hardware srcHWPos = GetPhyType((TPosition)src.GetPosition());
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyTensorSizeOverflow(dst, src, intriParams, enhancedParams)));
    if (srcHWPos == Hardware::L1) {
        if (dstHWPos == Hardware::L0C) {
            // l1 -> l0c
            DataCopyL12L0CImpl(
                (__cc__ half*)dst.GetPhyAddr(), (__cbuf__ float*)src.GetPhyAddr(), intriParams, enhancedParams);
        } else {
            ASCENDC_CHECK_TPOSITION(
                false, "dst", "L0C Buffer(CO1)",
                "DataCopy from LocalTensor(L1 Buffer(A1/B1)) to LocalTensor with DataCopyEnhancedParams",
                GetPositionDisplay(static_cast<TPosition>(dst.GetPosition())));
        }
    } else if (srcHWPos == Hardware::L0C) {
        if (dstHWPos == Hardware::UB) {
            // l0c -> ub
            DataCopyL0C2UBImpl(
                (__ubuf__ half*)dst.GetPhyAddr(), (__cc__ float*)src.GetPhyAddr(), intriParams, enhancedParams);
        } else {
            ASCENDC_CHECK_TPOSITION(
                false, "dst", "UB(CO2)",
                "DataCopy from LocalTensor(L0C Buffer(CO1)) to LocalTensor with DataCopyEnhancedParams",
                GetPositionDisplay(static_cast<TPosition>(dst.GetPosition())));
        }
    } else {
        ASCENDC_CHECK_TPOSITION(
            false, "dst", "L1 Buffer(A1/B1)/L0C Buffer(CO1)",
            "DataCopy from LocalTensor(U) to LocalTensor(T) with DataCopyEnhancedParams",
            GetPositionDisplay(static_cast<TPosition>(dst.GetPosition())));
    }
}

template <typename T, typename U>
__aicore__ inline void CheckTensorL0C2UB(const LocalTensor<T>& dst, const LocalTensor<U>& src)
{
    CheckTensorPos<U>(
        src, Hardware::L0C, "src", "CO1",
        "DataCopy from LocalTensor(L0C Buffer(CO1)) to LocalTensor(UB(CO2)) with DataCopyEnhancedParams");
    CheckTensorPos<T>(
        dst, Hardware::UB, "dst", "CO2",
        "DataCopy from LocalTensor(L0C Buffer(CO1)) to LocalTensor(UB(CO2)) with DataCopyEnhancedParams");
}

// int32_t to half
template <
    typename T, typename U,
    typename Std::enable_if<Std::is_same<PrimT<T>, half>::value && Std::is_same<PrimT<U>, int32_t>::value, bool>::type>
__aicore__ inline __inout_pipe__(V) void DataCopy(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxDataCopyInfo(dst, src, intriParams, "DataCopy");
#endif
    CheckTensorL0C2UB(dst, src);
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyTensorSizeOverflow(dst, src, intriParams, enhancedParams)));
    DataCopyL0C2UBImpl(
        (__ubuf__ half*)dst.GetPhyAddr(), (__cc__ int32_t*)src.GetPhyAddr(), intriParams, enhancedParams);
}

// int32_t to int16_t
template <
    typename T, typename U,
    typename Std::enable_if<
        Std::is_same<PrimT<T>, int16_t>::value && Std::is_same<PrimT<U>, int32_t>::value, bool>::type>
__aicore__ inline __inout_pipe__(V) void DataCopy(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxDataCopyInfo(dst, src, intriParams, "DataCopy");
#endif
    CheckTensorL0C2UB(dst, src);
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyTensorSizeOverflow(dst, src, intriParams, enhancedParams)));
    DataCopyL0C2UBImpl(
        (__ubuf__ int16_t*)dst.GetPhyAddr(), (__cc__ int32_t*)src.GetPhyAddr(), intriParams, enhancedParams);
}

// int32_t to int8_t
template <
    typename T, typename U,
    typename Std::enable_if<
        Std::is_same<PrimT<T>, int8_t>::value && Std::is_same<PrimT<U>, int32_t>::value, bool>::type>
__aicore__ inline __inout_pipe__(V) void DataCopy(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxDataCopyInfo(dst, src, intriParams, "DataCopy");
#endif
    CheckTensorL0C2UB(dst, src);
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyTensorSizeOverflow(dst, src, intriParams, enhancedParams)));
    DataCopyL0C2UBImpl(
        (__ubuf__ int8_t*)dst.GetPhyAddr(), (__cc__ int32_t*)src.GetPhyAddr(), intriParams, enhancedParams);
}

// int32_t to uint8_t
template <
    typename T, typename U,
    typename Std::enable_if<
        Std::is_same<PrimT<T>, uint8_t>::value && Std::is_same<PrimT<U>, int32_t>::value, bool>::type>
__aicore__ inline __inout_pipe__(V) void DataCopy(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxDataCopyInfo(dst, src, intriParams, "DataCopy");
#endif
    CheckTensorL0C2UB(dst, src);
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyTensorSizeOverflow(dst, src, intriParams, enhancedParams)));
    DataCopyL0C2UBImpl(
        (__ubuf__ uint8_t*)dst.GetPhyAddr(), (__cc__ int32_t*)src.GetPhyAddr(), intriParams, enhancedParams);
}

// half to float
template <
    typename T, typename U,
    typename Std::enable_if<Std::is_same<PrimT<T>, float>::value && Std::is_same<PrimT<U>, half>::value, bool>::type>
__aicore__ inline __inout_pipe__(V) void DataCopy(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxDataCopyInfo(dst, src, intriParams, "DataCopy");
#endif
    CheckTensorPos<U>(
        src, Hardware::UB, "src", "CO2",
        "DataCopy from LocalTensor(UB(CO2)) to LocalTensor(CO1) with DataCopyEnhancedParams");
    CheckTensorPos<T>(
        dst, Hardware::L0C, "dst", "CO1",
        "DataCopy from LocalTensor(UB(CO2)) to LocalTensor(CO1) with DataCopyEnhancedParams");
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyTensorSizeOverflow(dst, src, intriParams, enhancedParams)));
    DataCopyUB2L0CImpl((__cc__ float*)dst.GetPhyAddr(), (__ubuf__ half*)src.GetPhyAddr(), intriParams, enhancedParams);
}

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
template <typename T, PaddingMode mode>
__aicore__ inline __inout_pipe__(MTE2) void DataCopyPad(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyParams& dataCopyParams,
    const DataCopyPadParams& padParams)
{
#ifdef ASCENDC_TIME_STAMP_ON
    PrintTimeStamp(static_cast<uint32_t>(TimeStampId::TIME_STAMP_MTE2_DATACOPY));
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxDataCopyPadInfo(dst, src, dataCopyParams, padParams, "DataCopyPad");
#endif
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
    CheckDataCopyPadTypeSupport<T>("DataCopyPad from GlobalTensor to LocalTensor with DataCopyPadParams");
    CheckDataCopyPadTensor(
        dst, src, dataCopyParams, padParams, "DataCopyPad from GlobalTensor to LocalTensor with DataCopyPadParams");
#endif
#endif
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncDataCopyPad(dst, src, dataCopyParams, padParams, "DataCopyPad from GM to UB(VECIN/VECOUT)")) {
        ASCENDC_REPORT_CHECK_ERROR("DataCopyPad from GM to UB(VECIN/VECOUT)", KernelFuncType::NONE_MODE);
    }
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyPadTensorSizeOverflow(dst, src, dataCopyParams, padParams)));
#endif
    const Hardware dstHWPos = GetPhyType((TPosition)dst.GetPosition());
    const uint8_t cacheMode = ExtractCacheMode(src);
    ASCENDC_ASSERT(((dstHWPos == Hardware::UB) || (dstHWPos == Hardware::L1)), {
        KERNEL_LOG(KERNEL_ERROR, "DataCopyPad dst position must be UB(VECIN/VECOUT/LCM)/L1 Buffer(TSCM)");
    });
    if (dstHWPos == Hardware::UB) {
        DataCopyPadGm2UBImpl<PrimType, mode>(
            (__ubuf__ PrimType*)dst.GetPhyAddr(), (__gm__ PrimType*)src.GetPhyAddr(), dataCopyParams, padParams,
            cacheMode);
    } else if (dstHWPos == Hardware::L1) {
        DataCopyPadGm2L1Impl<PrimType, mode>(
            (__cbuf__ PrimType*)dst.GetPhyAddr(), (__gm__ PrimType*)src.GetPhyAddr(), dataCopyParams, padParams,
            cacheMode);
    } else {
        ASCENDC_CHECK_TPOSITION(
            false, "dst", "UB(VECIN/VECOUT)", "DataCopyPad from GlobalTensor to LocalTensor with DataCopyPadParams",
            GetPositionDisplay(static_cast<TPosition>(dst.GetPosition())));
    }
}

template <typename T, PaddingMode mode>
__aicore__ inline __inout_pipe__(MTE3) void DataCopyPad(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& dataCopyParams)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxDataCopyPadInfo(dst, src, dataCopyParams, "DataCopyPad");
#endif
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
    CheckDataCopyPadTypeSupport<T>("DataCopyPad from LocalTensor to GlobalTensor with DataCopyParams");
    CheckDataCopyPadTensor(
        dst, src, dataCopyParams, "DataCopyPad from LocalTensor to GlobalTensor with DataCopyParams");
#endif
#endif
#if (__NPU_ARCH__ != 5102)
    if ASCEND_IS_AIC {
        return;
    }
#endif
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyPadTensorSizeOverflow(dst, src, dataCopyParams)));
    const Hardware srcHWPos = GetPhyType((TPosition)src.GetPosition());
    const uint8_t cacheMode = ExtractCacheMode(dst);
    if (srcHWPos == Hardware::UB) {
        DataCopyPadUB2GMImpl<T, mode>(
            (__gm__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), dataCopyParams, cacheMode);
    } else {
        ASCENDC_CHECK_TPOSITION(
            false, "src", "UB(VECIN/VECOUT)", "DataCopyPad from LocalTensor to GlobalTensor with DataCopyParams",
            GetPositionDisplay(static_cast<TPosition>(src.GetPosition())));
    }
}
#else //  defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
template <typename T>
__aicore__ inline __inout_pipe__(MTE2) void DataCopyPad(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyParams& dataCopyParams,
    const DataCopyPadParams& padParams)
{
#ifdef ASCENDC_TIME_STAMP_ON
    PrintTimeStamp(static_cast<uint32_t>(TimeStampId::TIME_STAMP_MTE2_DATACOPY));
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxDataCopyPadInfo(dst, src, dataCopyParams, padParams, "DataCopyPad");
#endif
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
    CheckDataCopyPadTypeSupport<T>("DataCopyPad from GlobalTensor to LocalTensor with DataCopyPadParams");
    CheckDataCopyPadTensor(
        dst, src, dataCopyParams, padParams, "DataCopyPad from GlobalTensor to LocalTensor with DataCopyPadParams");
#endif
#endif
    if ASCEND_IS_AIC {
        return;
    }
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncDataCopyPad(dst, src, dataCopyParams, padParams, "DataCopyPad from GM to UB(VECIN/VECOUT)")) {
        ASCENDC_REPORT_CHECK_ERROR("DataCopyPad from GM to UB(VECIN/VECOUT)", KernelFuncType::NONE_MODE);
    }
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyPadTensorSizeOverflow(dst, src, dataCopyParams, padParams)));
#endif
    const Hardware dstHWPos = GetPhyType((TPosition)dst.GetPosition());
    if (dstHWPos == Hardware::UB) {
        DataCopyPadGm2UBImpl(
            (__ubuf__ PrimType*)dst.GetPhyAddr(), (__gm__ PrimType*)src.GetPhyAddr(), dataCopyParams, padParams);
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
    } else {
        DataCopyPadGm2L1Impl((__cbuf__ T*)dst.GetPhyAddr(), (__gm__ T*)src.GetPhyAddr(), dataCopyParams, padParams);
#else
    } else if (dstHWPos == Hardware::L1) {
        DataCopyPadGM2L1Impl(
            (__cbuf__ PrimType*)dst.GetPhyAddr(), (__gm__ PrimType*)src.GetPhyAddr(), dataCopyParams, padParams);
    } else {
#if (__NPU_ARCH__ == 3102)
        ASCENDC_CHECK_TPOSITION(
            false, "dst", "L1 Buffer(A1/B1/C1)/UB(VECIN/VECOUT)",
            "DataCopyPad from GlobalTensor to LocalTensor with DataCopyPadParams",
            GetPositionDisplay(static_cast<TPosition>(dst.GetPosition())));
#else
        ASCENDC_CHECK_TPOSITION(
            false, "dst", "UB(VECIN/VECOUT)", "DataCopyPad from GlobalTensor to LocalTensor with DataCopyPadParams",
            GetPositionDisplay(static_cast<TPosition>(dst.GetPosition())));
#endif
#endif
    }
}

template <typename T>
__aicore__ inline __inout_pipe__(MTE3) void DataCopyPad(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& dataCopyParams)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxDataCopyPadInfo(dst, src, dataCopyParams, "DataCopyPad");
#endif
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
    CheckDataCopyPadTypeSupport<T>("DataCopyPad from LocalTensor to GlobalTensor with DataCopyParams");
    CheckDataCopyPadTensor(
        dst, src, dataCopyParams, "DataCopyPad from LocalTensor to GlobalTensor with DataCopyParams");
#endif
#endif
    if ASCEND_IS_AIC {
        return;
    }
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyPadTensorSizeOverflow(dst, src, dataCopyParams)));
    const Hardware srcHWPos = GetPhyType((TPosition)src.GetPosition());
    if (srcHWPos == Hardware::UB) {
        DataCopyPadUB2GMImpl((__gm__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), dataCopyParams);
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
    } else {
        DataCopyPadL12GMImpl((__gm__ T*)dst.GetPhyAddr(), (__cbuf__ T*)src.GetPhyAddr(), dataCopyParams);
#else
    } else if (srcHWPos == Hardware::L1) {
        DataCopyPadL12GMImpl((__gm__ PrimType*)dst.GetPhyAddr(), (__cbuf__ PrimType*)src.GetPhyAddr(), dataCopyParams);
    } else {
#if (__NPU_ARCH__ == 3102)
        ASCENDC_CHECK_TPOSITION(
            false, "src", "L1 Buffer(A1/B1/C1)/UB(VECIN/VECOUT)",
            "DataCopyPad from LocalTensor to GlobalTensor with DataCopyParams",
            GetPositionDisplay(static_cast<TPosition>(src.GetPosition())));
#else
        ASCENDC_CHECK_TPOSITION(
            false, "src", "UB(VECIN/VECOUT)", "DataCopyPad from LocalTensor to GlobalTensor with DataCopyParams",
            GetPositionDisplay(static_cast<TPosition>(src.GetPosition())));
#endif
#endif
    }
}
#endif

template <typename T>
__aicore__ inline void DataCopyPad(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& dataCopyParams,
    const Nd2NzParams& nd2nzParams)
{
    CheckNd2NzParams(nd2nzParams, "DataCopyPad with Nd2NzParams");
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
    CheckDataCopyPadTypeSupport<T>("DataCopyPad with Nd2NzParams");
    CheckDataCopyPadTensor(dst, src, dataCopyParams, nd2nzParams, "DataCopyPad with Nd2NzParams");
#endif
#endif
    CheckTensorPos<T>(dst, Hardware::L1, "dst", "TSCM", "DataCopyPad with Nd2NzParams");
    CheckTensorPos<T>(src, Hardware::UB, "src", "VECIN/VECOUT", "DataCopyPad with Nd2NzParams");
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyPadTensorSizeOverflow(dst, src, dataCopyParams, nd2nzParams)));
    DataCopyPadUB2L1Impl(
        (__cbuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), dataCopyParams, nd2nzParams);
}

// override DataCopyPad, use new param DataCopyExtParams
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
template <typename T, PaddingMode mode>
__aicore__ inline __inout_pipe__(MTE2) void DataCopyPad(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyExtParams& dataCopyParams,
    const DataCopyPadExtParams<T>& padParams)
{
#ifdef ASCENDC_TIME_STAMP_ON
    PrintTimeStamp(static_cast<uint32_t>(TimeStampId::TIME_STAMP_MTE2_DATACOPY));
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxDataCopyPadInfo(dst, src, dataCopyParams, padParams, "DataCopyPad");
#endif

#if ASCENDC_CPU_DEBUG
    if (!CheckFuncDataCopyPad(dst, src, dataCopyParams, padParams, "DataCopyPad from GM to UB(VECIN/VECOUT)")) {
        ASCENDC_REPORT_CHECK_ERROR("DataCopyPad from GM to UB(VECIN/VECOUT)", KernelFuncType::NONE_MODE);
    }
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyPadTensorSizeOverflow(dst, src, dataCopyParams, padParams)));
#endif
    const Hardware dstHWPos = GetPhyType((TPosition)dst.GetPosition());
    const uint8_t cacheMode = ExtractCacheMode(src);
    if (dstHWPos == Hardware::UB) {
        DataCopyPadGm2UBImpl<T, mode>(
            (__ubuf__ T*)dst.GetPhyAddr(), (__gm__ T*)src.GetPhyAddr(), dataCopyParams, padParams, cacheMode);
    } else if (dstHWPos == Hardware::L1) {
        DataCopyPadGm2L1Impl<T, mode>(
            (__cbuf__ T*)dst.GetPhyAddr(), (__gm__ T*)src.GetPhyAddr(), dataCopyParams, padParams, cacheMode);
    } else {
        ASCENDC_CHECK_TPOSITION(
            false, "dst", "UB(VECIN/VECOUT)", "DataCopyPad from GM to UB(VECIN/VECOUT)",
            GetPositionDisplay(static_cast<TPosition>(dst.GetPosition())));
    }
}
#else // defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
template <typename T>
__aicore__ inline __inout_pipe__(MTE2) void DataCopyPad(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyExtParams& dataCopyParams,
    const DataCopyPadExtParams<T>& padParams)
{
#ifdef ASCENDC_TIME_STAMP_ON
    PrintTimeStamp(static_cast<uint32_t>(TimeStampId::TIME_STAMP_MTE2_DATACOPY));
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxDataCopyPadInfo(dst, src, dataCopyParams, padParams, "DataCopyPad");
#endif
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
    CheckDataCopyPadTypeSupport<T>("DataCopyPad from GM to UB(VECIN/VECOUT)");
    CheckDataCopyPadTensor(dst, src, dataCopyParams, padParams, "DataCopyPad from GM to UB(VECIN/VECOUT)");
#endif
#endif
    if ASCEND_IS_AIC {
        return;
    }
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncDataCopyPad(dst, src, dataCopyParams, padParams, "DataCopyPad from GM to UB(VECIN/VECOUT)")) {
        ASCENDC_REPORT_CHECK_ERROR("DataCopyPad from GM to UB(VECIN/VECOUT)", KernelFuncType::NONE_MODE);
    }
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyPadTensorSizeOverflow(dst, src, dataCopyParams, padParams)));
#endif
    const Hardware dstHWPos = GetPhyType((TPosition)dst.GetPosition());
    if (dstHWPos == Hardware::UB) {
        DataCopyPadGm2UBImpl((__ubuf__ T*)dst.GetPhyAddr(), (__gm__ T*)src.GetPhyAddr(), dataCopyParams, padParams);
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
    } else {
        DataCopyPadGm2L1Impl((__cbuf__ T*)dst.GetPhyAddr(), (__gm__ T*)src.GetPhyAddr(), dataCopyParams, padParams);
#else
    } else if (dstHWPos == Hardware::L1) {
        DataCopyPadGM2L1Impl((__cbuf__ T*)dst.GetPhyAddr(), (__gm__ T*)src.GetPhyAddr(), dataCopyParams, padParams);
    } else {
#if (__NPU_ARCH__ == 3102)
        ASCENDC_CHECK_TPOSITION(
            false, "dst", "L1 Buffer(A1/B1/C1)/UB(VECIN/VECOUT)", "DataCopyPad from GM to UB(VECIN/VECOUT)",
            GetPositionDisplay(static_cast<TPosition>(dst.GetPosition())));
#else
        ASCENDC_CHECK_TPOSITION(
            false, "dst", "UB(VECIN/VECOUT)", "DataCopyPad from GM to UB(VECIN/VECOUT)",
            GetPositionDisplay(static_cast<TPosition>(dst.GetPosition())));
#endif
#endif
    }
}
#endif

// override DataCopyPad, use new param DataCopyExtParams
// T use TensorTrait while U is primitive type
template <
    typename T, typename U,
    typename Std::enable_if<Std::is_same<PrimT<T>, U>::value && (!Std::is_same<T, U>::value), bool>::type>
__aicore__ inline __inout_pipe__(MTE2) void DataCopyPad(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyExtParams& dataCopyParams,
    const DataCopyPadExtParams<U>& padParams)
{
#ifdef ASCENDC_TIME_STAMP_ON
    PrintTimeStamp(static_cast<uint32_t>(TimeStampId::TIME_STAMP_MTE2_DATACOPY));
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxDataCopyPadInfo(dst, src, dataCopyParams, padParams, "DataCopyPad");
#endif
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
    CheckDataCopyPadTypeSupport<T>("DataCopyPad from GM to UB(VECIN/VECOUT)");
    CheckDataCopyPadTensor(dst, src, dataCopyParams, padParams, "DataCopyPad from GM to UB(VECIN/VECOUT)");
#endif
#endif
    if ASCEND_IS_AIC {
        return;
    }
#if ASCENDC_CPU_DEBUG
    if (!CheckFuncDataCopyPad(dst, src, dataCopyParams, padParams, "DataCopyPad from GM to UB(VECIN/VECOUT)")) {
        ASCENDC_REPORT_CHECK_ERROR("DataCopyPad from GM to UB(VECIN/VECOUT)", KernelFuncType::NONE_MODE);
    }
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyPadTensorSizeOverflow(dst, src, dataCopyParams, padParams)));
#endif
    CheckTensorPos<T>(dst, Hardware::UB, "dst", "VECIN/VECOUT", "DataCopyPad from GM to UB(VECIN/VECOUT)");
    DataCopyPadGm2UBImpl(
        (__ubuf__ PrimType*)dst.GetPhyAddr(), (__gm__ PrimType*)src.GetPhyAddr(), dataCopyParams, padParams);
}

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
template <typename T, PaddingMode mode>
__aicore__ inline __inout_pipe__(MTE3) void DataCopyPad(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyExtParams& dataCopyParams)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxDataCopyPadInfo(dst, src, dataCopyParams, "DataCopyPad");
#endif
    using PrimType = PrimT<T>;
#if (__NPU_ARCH__ != 5102)
    if ASCEND_IS_AIC {
        return;
    }
#endif
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyPadTensorSizeOverflow(dst, src, dataCopyParams)));
    const Hardware srcHWPos = GetPhyType((TPosition)src.GetPosition());
    const uint8_t cacheMode = ExtractCacheMode(dst);
    if (srcHWPos == Hardware::UB) {
        DataCopyPadUB2GMImpl<T, mode>(
            (__gm__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), dataCopyParams, cacheMode);
    } else {
        ASCENDC_CHECK_TPOSITION(
            false, "src", "UB(VECIN/VECOUT)", "DataCopyPad from LocalTensor to GlobalTensor with DataCopyExtParams",
            GetPositionDisplay(static_cast<TPosition>(src.GetPosition())));
    }
}
#else // defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
template <typename T>
__aicore__ inline __inout_pipe__(MTE3) void DataCopyPad(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyExtParams& dataCopyParams)
{
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxDataCopyPadInfo(dst, src, dataCopyParams, "DataCopyPad");
#endif
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
    CheckDataCopyPadTypeSupport<T>("DataCopyPad from LocalTensor to GlobalTensor with DataCopyExtParams");
    CheckDataCopyPadTensor(
        dst, src, dataCopyParams, "DataCopyPad from LocalTensor to GlobalTensor with DataCopyExtParams");
#endif
#endif
    if ASCEND_IS_AIC {
        return;
    }
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyPadTensorSizeOverflow(dst, src, dataCopyParams)));
    const Hardware srcHWPos = GetPhyType((TPosition)src.GetPosition());
    if (srcHWPos == Hardware::UB) {
        DataCopyPadUB2GMImpl((__gm__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), dataCopyParams);
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
    } else {
        DataCopyPadL12GMImpl((__gm__ T*)dst.GetPhyAddr(), (__cbuf__ T*)src.GetPhyAddr(), dataCopyParams);
#else
    } else if (srcHWPos == Hardware::L1) {
        DataCopyPadL12GMImpl((__gm__ PrimType*)dst.GetPhyAddr(), (__cbuf__ PrimType*)src.GetPhyAddr(), dataCopyParams);
    } else {
#if (__NPU_ARCH__ == 3102)
        ASCENDC_CHECK_TPOSITION(
            false, "src", "L1 Buffer(A1/B1/C1)/UB(VECIN/VECOUT)",
            "DataCopyPad from LocalTensor to GlobalTensor with DataCopyExtParams",
            GetPositionDisplay(static_cast<TPosition>(src.GetPosition())));
#else
        ASCENDC_CHECK_TPOSITION(
            false, "src", "UB(VECIN/VECOUT)", "DataCopyPad from LocalTensor to GlobalTensor with DataCopyExtParams",
            GetPositionDisplay(static_cast<TPosition>(src.GetPosition())));
#endif
#endif
    }
}
#endif

template <typename T>
__aicore__ inline void DataCopyPad(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyExtParams& dataCopyParams,
    const Nd2NzParams& nd2nzParams)
{
    CheckNd2NzParams(nd2nzParams, "DataCopyPad with Nd2NzParams");
    using PrimType = PrimT<T>;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
    CheckDataCopyPadTypeSupport<T>("DataCopyPad with Nd2NzParams");
    CheckDataCopyPadTensor(dst, src, dataCopyParams, nd2nzParams, "DataCopyPad with Nd2NzParams");
#endif
#endif
    CheckTensorPos<T>(dst, Hardware::L1, "dst", "TSCM", "DataCopyPad with Nd2NzParams");
    CheckTensorPos<T>(src, Hardware::UB, "src", "VECIN/VECOUT", "DataCopyPad with Nd2NzParams");
    ASCENDC_REPORT_OVERFLOW_MEM((CheckDataCopyPadTensorSizeOverflow(dst, src, dataCopyParams, nd2nzParams)));
    DataCopyPadUB2L1Impl(
        (__cbuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src.GetPhyAddr(), dataCopyParams, nd2nzParams);
}

template <typename T, TPosition pos>
__aicore__ inline void SetPadValue(T paddingValue)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510)
    if (g_coreType == AIC) {
        return;
    }
    set_mov_pad_val(GetScalarBitcodeValue((T)paddingValue));
#elif (__NPU_ARCH__ == 3102)
    if constexpr (pos == TPosition::MAX || GetPhyType(pos) == Hardware::UB) {
        set_pad_val_outtoub(GetScalarBitcodeValue((T)paddingValue));
    } else if constexpr (GetPhyType(pos) == Hardware::L1) {
        set_pad_val_outtol1(GetScalarBitcodeValue((T)paddingValue));
    } else {
        ASCENDC_REPORT_NOT_SUPPORT(false, "SetPadValue");
    }
#else
    ASCENDC_REPORT_NOT_SUPPORT(false, "SetPadValue");
#endif
}

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
template <typename T, uint8_t dim, const NdDmaConfig& config>
__aicore__ inline void DataCopy(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const MultiCopyParams<T, dim>& params)
{
#ifdef ASCENDC_TIME_STAMP_ON
    PrintTimeStamp(static_cast<uint32_t>(TimeStampId::TIME_STAMP_MTE2_DATACOPY));
#endif
#if (__NPU_ARCH__ != 5102)
    if ASCEND_IS_AIC {
        return;
    }
#endif
    const uint8_t cacheMode = ExtractCacheMode(src);
    DataCopyWithNDDMAImpl<T, dim, config>(
        (__ubuf__ PrimT<T>*)dst.GetPhyAddr(), (__gm__ PrimT<T>*)src.GetPhyAddr(), params.loopInfo, params.constantValue,
        cacheMode);
}

__aicore__ inline void NdDmaDci() { NdDmaDciImpl(); }

__aicore__ inline void SetLoopModePara(const LoopModeParams& loopParams, DataCopyMVType type)
{
    if (type == DataCopyMVType::UB_TO_OUT) {
        SetLoopModeUBParaImpl(loopParams);
    } else {
        SetLoopModeOutParaImpl(loopParams);
    }
}
__aicore__ inline void ResetLoopModePara(DataCopyMVType type)
{
    if (type == DataCopyMVType::UB_TO_OUT) {
        ResetUBLoopModeParaImpl();
    } else {
        ResetOutLoopModeParaImpl();
    }
}
#endif
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_VCONV_INTERFACE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_INTF_IMPL_H__
#endif
