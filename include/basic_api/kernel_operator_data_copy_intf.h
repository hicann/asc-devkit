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
 * \file kernel_operator_data_copy_intf.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "kernel_operator_data_copy_intf.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_INTF_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_DATA_COPY_INTERFACE_H
#define ASCENDC_MODULE_OPERATOR_DATA_COPY_INTERFACE_H

#include "../../impl/basic_api/kernel_macros.h"
#include "../../impl/basic_api/common_types.h"
#include "kernel_struct_data_copy.h"
#include "../../impl/basic_api/utils/kernel_utils_macros.h"
#include "../../impl/basic_api/utils/kernel_utils_struct_confusion_pad.h"
#include "kernel_tensor.h"

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <cstddef>
#include <cstdint>
#include "stub_def.h"
#include "kernel_fp16.h"
#endif

namespace AscendC {
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
    DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyParams& repeatParams);

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
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3113) || (__NPU_ARCH__ == 5101) || \
    (__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163)
template <typename T, bool enableSmallC0 = false>
__aicore__ inline __inout_pipe__(MTE2) void DataCopy(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams);
#else
template <typename T>
__aicore__ inline __inout_pipe__(MTE2) void DataCopy(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams);
#endif

/*
 * @ingroup DataCopy Level 0
 * @brief format transform(such as nd2nz) during data load from UB to L1(Only TSCM)
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] intriParams.ndNum nd number of data to be moved, onlyc can be 1
 * @param [in] intriParams.nValue n value
 * @param [in] intriParams.dValue d value in unit of element
 * @param [in] intriParams.srcNdMatrixStride stride between nd matrixs at source ND matrix in unit of element
 * @param [in] intriParams.srcDValue SRC_D value in unit of element
 * @param [in] intriParams.dstNzC0Stride stride of nz between 2 C0 in L1 in unit of C0_size
 * @param [in] intriParams.dstNzNStride stride of n between 2 C0 in L1
 * @param [in] intriParams.dstNzMatrixStride DST_nz_matrix_stride in L1 in unit of element
 */
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const Nd2NzParams& intriParams);

/*
 * @ingroup DataCopy Level 0
 * @brief format transform(such as dn2nz) during data load from OUT to L1
 * @param [out] dst output LocalTensor
 * @param [in] src input GlobalTensor
 * @param [in] intriParams.dnNum dn number of data to be moved
 * @param [in] intriParams.nValue n value
 * @param [in] intriParams.dValue d value in unit of element
 * @param [in] intriParams.srcDnMatrixStride stride between DN matrixs at source DN matrix in unit of element
 * @param [in] intriParams.srcDValue SRC_D value in unit of element
 * @param [in] intriParams.dstNzC0Stride stride of nz between 2 C0 in L1 in unit of C0_size
 * @param [in] intriParams.dstNzNStride stride of n between 2 C0 in L1
 * @param [in] intriParams.dstNzMatrixStride DST_nz_matrix_stride in L1 in unit of element
 */
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5101) || (__NPU_ARCH__ == 5161) || \
    (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163)
template <typename T, bool enableSmallC0 = false>
__aicore__ inline __inout_pipe__(MTE2) void DataCopy(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Dn2NzParams& intriParams);
#endif

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
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams);

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
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams);

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
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& repeatParams);

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
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(3510, "Copy is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline __inout_pipe__(V) void Copy(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint64_t mask[], const uint8_t repeatTime,
    const CopyRepeatParams& repeatParams);

// Copy::Level 0 - mask count mode
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(3510, "Copy is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline __inout_pipe__(V) void Copy(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint64_t mask, const uint8_t repeatTime,
    const CopyRepeatParams& repeatParams);

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
// Copy::Level 2 - count mode
template <typename T, bool isSetMask = true>
__aicore__ inline __inout_pipe__(V) void Copy(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count);
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
    const SliceInfo srcSliceInfo[], const uint32_t dimValue = 1);

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
    const SliceInfo srcSliceInfo[], const uint32_t dimValue = 1);

/*
 * @ingroup DataCopy Level 2
 * @brief datacopy from src to dst, applicable to vector data
 * @param [out] dst output LocalTensor
 * @param [in] src input GlobalTensor
 * @param [in] count Number of operands
 */
template <typename T>
__aicore__ inline __inout_pipe__(MTE2) void DataCopy(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const uint32_t count);

/*
 * @ingroup DataCopy Level 2
 * @brief datacopy from src to dst, applicable to vector data
 * @param [out] dst output GlobalTensor
 * @param [in] src input LocalTensor
 * @param [in] count Number of operands
 */
template <typename T>
__aicore__ inline __inout_pipe__(MTE3) void DataCopy(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count);

/*
 * @ingroup DataCopy Level 2
 * @brief datacopy from src to dst, applicable to vector data
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] count Number of operands
 */
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count);

/*
 * @ingroup DataCopy Level 2
 * @brief datacopy from src to dst, nz2nd, applicable to simulated cube data(such as data from l0c, 16*16)
 * @param [out] dst output GlobalTensor
 * @param [in] src input LocalTensor
 */
template <typename T>
__aicore__ inline __inout_pipe__(MTE3) void DataCopy(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const Nz2NdParamsFull& intriParams);

/*
 * @ingroup DataCopy Level 2
 * @brief datacopy from src to dst, nz2dn, applicable to simulated cube data(such as data from l0c, 16*16)
 * @param [out] dst output GlobalTensor
 * @param [in] src input LocalTensor
 */
#if defined(__NPU_ARCH__) && \
    ((__NPU_ARCH__ == 5101) || (__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
template <typename T>
__aicore__ inline __inout_pipe__(MTE3) void DataCopy(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const Nz2DnParamsFull& intriParams);
#endif

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
    const DataCopyEnhancedParams& enhancedParams);

template <typename T>
__aicore__ inline __inout_pipe__(MTE3) void DataCopy(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams);

template <typename T>
__aicore__ inline void DataCopy(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams);

template <typename T, typename U>
__aicore__ inline void DataCopy(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyCO12DstParams& intriParams);

template <typename T, typename U>
__aicore__ inline void DataCopy(
    const GlobalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyCO12DstParams& intriParams);

#if (defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201))
// float to bfloat16_t
template <
    typename T, typename U,
    typename Std::enable_if<
        Std::is_same<PrimT<T>, bfloat16_t>::value && Std::is_same<PrimT<U>, float>::value, bool>::type = true>
__aicore__ inline void DataCopy(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams);
#endif

// float to half
template <
    typename T, typename U,
    typename Std::enable_if<Std::is_same<PrimT<T>, half>::value && Std::is_same<PrimT<U>, float>::value, bool>::type =
        true>
__aicore__ inline void DataCopy(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams);

// int32_t to half
template <
    typename T, typename U,
    typename Std::enable_if<Std::is_same<PrimT<T>, half>::value && Std::is_same<PrimT<U>, int32_t>::value, bool>::type =
        true>
__aicore__ inline __inout_pipe__(V) void DataCopy(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams);

// int32_t to int16_t
template <
    typename T, typename U,
    typename Std::enable_if<
        Std::is_same<PrimT<T>, int16_t>::value && Std::is_same<PrimT<U>, int32_t>::value, bool>::type = true>
__aicore__ inline __inout_pipe__(V) void DataCopy(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams);

// int32_t to int8_t
template <
    typename T, typename U,
    typename Std::enable_if<
        Std::is_same<PrimT<T>, int8_t>::value && Std::is_same<PrimT<U>, int32_t>::value, bool>::type = true>
__aicore__ inline __inout_pipe__(V) void DataCopy(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams);

// int32_t to uint8_t
template <
    typename T, typename U,
    typename Std::enable_if<
        Std::is_same<PrimT<T>, uint8_t>::value && Std::is_same<PrimT<U>, int32_t>::value, bool>::type = true>
__aicore__ inline __inout_pipe__(V) void DataCopy(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams);

// half to float
template <
    typename T, typename U,
    typename Std::enable_if<Std::is_same<PrimT<T>, float>::value && Std::is_same<PrimT<U>, half>::value, bool>::type =
        true>
__aicore__ inline __inout_pipe__(V) void DataCopy(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams);

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
template <typename T, PaddingMode mode = PaddingMode::Normal>
__aicore__ inline __inout_pipe__(MTE2) void DataCopyPad(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyParams& dataCopyParams,
    const DataCopyPadParams& padParams);

template <typename T, PaddingMode mode = PaddingMode::Normal>
__aicore__ inline __inout_pipe__(MTE3) void DataCopyPad(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& dataCopyParams);
#else
template <typename T>
__aicore__ inline __inout_pipe__(MTE2) void DataCopyPad(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyParams& dataCopyParams,
    const DataCopyPadParams& padParams);

template <typename T>
__aicore__ inline __inout_pipe__(MTE3) void DataCopyPad(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& dataCopyParams);
#endif

template <typename T>
__aicore__ inline void DataCopyPad(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& dataCopyParams,
    const Nd2NzParams& nd2nzParams);

// override DataCopyPad, use new param DataCopyExtParams
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5101) || (__NPU_ARCH__ == 5161) || \
    (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163)
template <typename T, PaddingMode mode = PaddingMode::Normal>
__aicore__ inline __inout_pipe__(MTE2) void DataCopyPad(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyExtParams& dataCopyParams,
    const DataCopyPadExtParams<T>& padParams);
#else
template <typename T>
__aicore__ inline __inout_pipe__(MTE2) void DataCopyPad(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyExtParams& dataCopyParams,
    const DataCopyPadExtParams<T>& padParams);
#endif

// override DataCopyPad, use new param DataCopyExtParams
// T use TensorTrait while U is primitive type
template <
    typename T, typename U,
    typename Std::enable_if<Std::is_same<PrimT<T>, U>::value && (!Std::is_same<T, U>::value), bool>::type = true>
__aicore__ inline __inout_pipe__(MTE2) void DataCopyPad(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyExtParams& dataCopyParams,
    const DataCopyPadExtParams<U>& padParams);

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5101) || (__NPU_ARCH__ == 5161) || \
    (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163)
template <typename T, PaddingMode mode = PaddingMode::Normal>
__aicore__ inline __inout_pipe__(MTE3) void DataCopyPad(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyExtParams& dataCopyParams);
#else
template <typename T>
__aicore__ inline __inout_pipe__(MTE3) void DataCopyPad(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyExtParams& dataCopyParams);
#endif

template <typename T>
__aicore__ inline void DataCopyPad(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyExtParams& dataCopyParams,
    const Nd2NzParams& nd2nzParams);

template <typename T, TPosition pos = TPosition::MAX>
__aicore__ inline void SetPadValue(T paddingValue);

template <typename T, uint8_t subBlockId = 0>
__aicore__ inline void DataCopyL1ToUB(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count);

template <typename T, uint8_t subBlockId = 0>
__aicore__ inline void DataCopyL1ToUB(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams);

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
template <typename T, uint8_t dim, const NdDmaConfig& config = kDefaultNdDmaConfig>
__aicore__ inline void DataCopy(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const MultiCopyParams<T, dim>& params);
#endif

__aicore__ inline void NdDmaDci();

__aicore__ inline void SetLoopModePara(const LoopModeParams& loopParams, DataCopyMVType type);

__aicore__ inline void ResetLoopModePara(DataCopyMVType type);
} // namespace AscendC

#if defined(__NPU_ARCH__)
#include "../../impl/basic_api/kernel_operator_data_copy_intf_impl.h"
#endif
#endif // ASCENDC_MODULE_OPERATOR_VEC_VCONV_INTERFACE_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_INTF_H__
#endif
