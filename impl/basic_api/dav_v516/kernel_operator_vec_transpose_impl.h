/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file kernel_operator_vec_transpose_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_v516/kernel_operator_vec_transpose_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_TRANSPOSE_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_TRANSPOSE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_TRANSPOSE_IMPL_H

namespace AscendC {
constexpr int8_t TWO_NUM = 2;
__aicore__ inline void TransDataTo5HDIntrinsicsImpl(
    __ubuf__ float* dstList[16], __ubuf__ float* srcList[16], const TransDataTo5HDParams& transDataTo5HDParams)
{
    scatter_vnchwconv_b32(
        VA0, VA2, transDataTo5HDParams.repeatTimes, transDataTo5HDParams.dstRepStride,
        transDataTo5HDParams.srcRepStride);
}

__aicore__ inline void TransDataTo5HDIntrinsicsImpl(
    __ubuf__ int32_t* dstList[16], __ubuf__ int32_t* srcList[16], const TransDataTo5HDParams& transDataTo5HDParams)
{
    scatter_vnchwconv_b32(
        VA0, VA2, transDataTo5HDParams.repeatTimes, transDataTo5HDParams.dstRepStride,
        transDataTo5HDParams.srcRepStride);
}

__aicore__ inline void TransDataTo5HDIntrinsicsImpl(
    __ubuf__ uint32_t* dstList[16], __ubuf__ uint32_t* srcList[16], const TransDataTo5HDParams& transDataTo5HDParams)
{
    scatter_vnchwconv_b32(
        VA0, VA2, transDataTo5HDParams.repeatTimes, transDataTo5HDParams.dstRepStride,
        transDataTo5HDParams.srcRepStride);
}

__aicore__ inline void TransDataTo5HDIntrinsicsImpl(
    __ubuf__ int16_t* dstList[16], __ubuf__ int16_t* srcList[16], const TransDataTo5HDParams& transDataTo5HDParams)
{
    scatter_vnchwconv_b16(
        VA0, VA2, transDataTo5HDParams.repeatTimes, transDataTo5HDParams.dstRepStride,
        transDataTo5HDParams.srcRepStride);
}

__aicore__ inline void TransDataTo5HDIntrinsicsImpl(
    __ubuf__ uint16_t* dstList[16], __ubuf__ uint16_t* srcList[16], const TransDataTo5HDParams& transDataTo5HDParams)
{
    scatter_vnchwconv_b16(
        VA0, VA2, transDataTo5HDParams.repeatTimes, transDataTo5HDParams.dstRepStride,
        transDataTo5HDParams.srcRepStride);
}

__aicore__ inline void TransDataTo5HDIntrinsicsImpl(
    __ubuf__ half* dstList[16], __ubuf__ half* srcList[16], const TransDataTo5HDParams& transDataTo5HDParams)
{
    scatter_vnchwconv_b16(
        VA0, VA2, transDataTo5HDParams.repeatTimes, transDataTo5HDParams.dstRepStride,
        transDataTo5HDParams.srcRepStride);
}

__aicore__ inline void TransDataTo5HDIntrinsicsImpl(
    __ubuf__ bfloat16_t* dstList[16], __ubuf__ bfloat16_t* srcList[16],
    const TransDataTo5HDParams& transDataTo5HDParams)
{
    scatter_vnchwconv_b16(
        VA0, VA2, transDataTo5HDParams.repeatTimes, transDataTo5HDParams.dstRepStride,
        transDataTo5HDParams.srcRepStride);
}

__aicore__ inline void TransDataTo5HDB8IntrinsicsCommonImpl(const TransDataTo5HDParams& transDataTo5HDParams)
{
    if ((transDataTo5HDParams.dstHighHalf == false) && (transDataTo5HDParams.srcHighHalf == false)) {
        scatter_vnchwconv_b8(
            VA0, VA2, transDataTo5HDParams.repeatTimes, transDataTo5HDParams.dstRepStride,
            transDataTo5HDParams.srcRepStride, false, false);
    } else if ((transDataTo5HDParams.dstHighHalf == false) && (transDataTo5HDParams.srcHighHalf == true)) {
        scatter_vnchwconv_b8(
            VA0, VA2, transDataTo5HDParams.repeatTimes, transDataTo5HDParams.dstRepStride,
            transDataTo5HDParams.srcRepStride, false, true);
    } else if ((transDataTo5HDParams.dstHighHalf == true) && (transDataTo5HDParams.srcHighHalf == true)) {
        scatter_vnchwconv_b8(
            VA0, VA2, transDataTo5HDParams.repeatTimes, transDataTo5HDParams.dstRepStride,
            transDataTo5HDParams.srcRepStride, true, true);
    } else {
        scatter_vnchwconv_b8(
            VA0, VA2, transDataTo5HDParams.repeatTimes, transDataTo5HDParams.dstRepStride,
            transDataTo5HDParams.srcRepStride, true, false);
    }
}

template <typename T>
__aicore__ inline void TransDataTo5HDB8IntrinsicsImpl(
    __ubuf__ T* dstList[16], __ubuf__ T* srcList[16], const TransDataTo5HDParams& transDataTo5HDParams)
{
    TransDataTo5HDB8IntrinsicsCommonImpl(transDataTo5HDParams);
}

__aicore__ inline void TransDataTo5HDIntrinsicsImpl(
    __ubuf__ int8_t* dstList[16], __ubuf__ int8_t* srcList[16], const TransDataTo5HDParams& transDataTo5HDParams)
{
    TransDataTo5HDB8IntrinsicsImpl(dstList, srcList, transDataTo5HDParams);
}

__aicore__ inline void TransDataTo5HDIntrinsicsImpl(
    __ubuf__ uint8_t* dstList[16], __ubuf__ uint8_t* srcList[16], const TransDataTo5HDParams& transDataTo5HDParams)
{
    TransDataTo5HDB8IntrinsicsImpl(dstList, srcList, transDataTo5HDParams);
}

template <typename T>
__aicore__ inline void TransDataTo5HDIntrinsicsImpl(
    uint64_t dstList[16], uint64_t srcList[16], const TransDataTo5HDParams& transDataTo5HDParams)
{
    static_assert(
        (SupportType<T, half, bfloat16_t, float, int16_t, uint16_t, int32_t, uint32_t, int8_t, uint8_t>()),
        "TransDataTo5HD does not support current datatype!");
}

template <>
__aicore__ inline void TransDataTo5HDIntrinsicsImpl<float>(
    uint64_t dstList[16], uint64_t srcList[16], const TransDataTo5HDParams& transDataTo5HDParams)
{
    scatter_vnchwconv_b32(
        VA0, VA2, transDataTo5HDParams.repeatTimes, transDataTo5HDParams.dstRepStride,
        transDataTo5HDParams.srcRepStride);
}

template <>
__aicore__ inline void TransDataTo5HDIntrinsicsImpl<int32_t>(
    uint64_t dstList[16], uint64_t srcList[16], const TransDataTo5HDParams& transDataTo5HDParams)
{
    scatter_vnchwconv_b32(
        VA0, VA2, transDataTo5HDParams.repeatTimes, transDataTo5HDParams.dstRepStride,
        transDataTo5HDParams.srcRepStride);
}

template <>
__aicore__ inline void TransDataTo5HDIntrinsicsImpl<uint32_t>(
    uint64_t dstList[16], uint64_t srcList[16], const TransDataTo5HDParams& transDataTo5HDParams)
{
    scatter_vnchwconv_b32(
        VA0, VA2, transDataTo5HDParams.repeatTimes, transDataTo5HDParams.dstRepStride,
        transDataTo5HDParams.srcRepStride);
}

template <>
__aicore__ inline void TransDataTo5HDIntrinsicsImpl<int16_t>(
    uint64_t dstList[16], uint64_t srcList[16], const TransDataTo5HDParams& transDataTo5HDParams)
{
    scatter_vnchwconv_b16(
        VA0, VA2, transDataTo5HDParams.repeatTimes, transDataTo5HDParams.dstRepStride,
        transDataTo5HDParams.srcRepStride);
}

template <>
__aicore__ inline void TransDataTo5HDIntrinsicsImpl<uint16_t>(
    uint64_t dstList[16], uint64_t srcList[16], const TransDataTo5HDParams& transDataTo5HDParams)
{
    scatter_vnchwconv_b16(
        VA0, VA2, transDataTo5HDParams.repeatTimes, transDataTo5HDParams.dstRepStride,
        transDataTo5HDParams.srcRepStride);
}

template <>
__aicore__ inline void TransDataTo5HDIntrinsicsImpl<half>(
    uint64_t dstList[16], uint64_t srcList[16], const TransDataTo5HDParams& transDataTo5HDParams)
{
    scatter_vnchwconv_b16(
        VA0, VA2, transDataTo5HDParams.repeatTimes, transDataTo5HDParams.dstRepStride,
        transDataTo5HDParams.srcRepStride);
}

template <>
__aicore__ inline void TransDataTo5HDIntrinsicsImpl<bfloat16_t>(
    uint64_t dstList[16], uint64_t srcList[16], const TransDataTo5HDParams& transDataTo5HDParams)
{
    scatter_vnchwconv_b16(
        VA0, VA2, transDataTo5HDParams.repeatTimes, transDataTo5HDParams.dstRepStride,
        transDataTo5HDParams.srcRepStride);
}

template <typename T>
__aicore__ inline void TransDataTo5HDB8IntrinsicsImpl(
    uint64_t dstList[16], uint64_t srcList[16], const TransDataTo5HDParams& transDataTo5HDParams)
{
    TransDataTo5HDB8IntrinsicsCommonImpl(transDataTo5HDParams);
}

template <>
__aicore__ inline void TransDataTo5HDIntrinsicsImpl<int8_t>(
    uint64_t dstList[16], uint64_t srcList[16], const TransDataTo5HDParams& transDataTo5HDParams)
{
    TransDataTo5HDB8IntrinsicsImpl<int8_t>(dstList, srcList, transDataTo5HDParams);
}

template <>
__aicore__ inline void TransDataTo5HDIntrinsicsImpl<uint8_t>(
    uint64_t dstList[16], uint64_t srcList[16], const TransDataTo5HDParams& transDataTo5HDParams)
{
    TransDataTo5HDB8IntrinsicsImpl<uint8_t>(dstList, srcList, transDataTo5HDParams);
}

template <typename T>
__aicore__ inline void SetVaReg(__ubuf__ T* dstList[16], __ubuf__ T* srcList[16])
{
    uint64_t vaRegArray1[VA_REG_ARRAY_LEN];
    uint64_t vaRegArray2[VA_REG_ARRAY_LEN];
    uint64_t vaRegArray3[VA_REG_ARRAY_LEN];
    uint64_t vaRegArray4[VA_REG_ARRAY_LEN];

    for (int32_t i = 0; i < VA_REG_ARRAY_LEN; i++) {
        vaRegArray1[i] = (uint64_t)dstList[i];
        vaRegArray2[i] = (uint64_t)dstList[VA_REG_ARRAY_LEN + i];
        vaRegArray3[i] = (uint64_t)srcList[i];
        vaRegArray4[i] = (uint64_t)srcList[VA_REG_ARRAY_LEN + i];
    }

    set_va_reg_sb(VA0, vaRegArray1);
    set_va_reg_sb(VA1, vaRegArray2);
    set_va_reg_sb(VA2, vaRegArray3);
    set_va_reg_sb(VA3, vaRegArray4);
}

__aicore__ inline void SetVaReg(uint64_t dst[NCHW_CONV_ADDR_LIST_SIZE], uint64_t src[NCHW_CONV_ADDR_LIST_SIZE])
{
    set_va_reg_sb(VA0, dst);
    set_va_reg_sb(VA1, dst + VA_REG_ARRAY_LEN);
    set_va_reg_sb(VA2, src);
    set_va_reg_sb(VA3, src + VA_REG_ARRAY_LEN);
}

__aicore__ inline void VldVaReg(__ubuf__ uint64_t* dst, __ubuf__ uint64_t* src)
{
    ldva(VA0, (uint64_t)dst, false); // false refers to low 128 bit;
    ldva(VA1, (uint64_t)dst, true);  // true refers to high 128 bit;
    ldva(VA2, (uint64_t)src, false);
    ldva(VA3, (uint64_t)src, true);
}

template <typename T>
__aicore__ inline void TransDataTo5HDImpl(
    __ubuf__ T* dstList[16], __ubuf__ T* srcList[16], const TransDataTo5HDParams& transDataTo5HDParams)
{
    static_assert(
        (SupportType<T, half, bfloat16_t, float, int16_t, uint16_t, int32_t, uint32_t, int8_t, uint8_t>()),
        "TransDataTo5HD does not support current datatype!");
    SetVaReg(dstList, srcList);
    TransDataTo5HDIntrinsicsImpl(dstList, srcList, transDataTo5HDParams);
}

template <typename T>
__aicore__ inline void TransDataTo5HDImpl(
    uint64_t dstList[NCHW_CONV_ADDR_LIST_SIZE], uint64_t srcList[NCHW_CONV_ADDR_LIST_SIZE],
    const TransDataTo5HDParams& transDataTo5HDParams)
{
    SetVaReg(dstList, srcList);
    TransDataTo5HDIntrinsicsImpl<T>(dstList, srcList, transDataTo5HDParams);
}

template <typename T>
__aicore__ inline void TransDataTo5HDVldVaRegImpl(
    __ubuf__ uint64_t* dst, __ubuf__ uint64_t* src, const TransDataTo5HDParams& transDataTo5HDParams)
{
    VldVaReg(dst, src);
    uint64_t dstList[NCHW_CONV_ADDR_LIST_SIZE] = {0};
    uint64_t srcList[NCHW_CONV_ADDR_LIST_SIZE] = {0};
    TransDataTo5HDIntrinsicsImpl<T>(dstList, srcList, transDataTo5HDParams);
}

template <typename T>
__aicore__ inline void TransposeImpl(__ubuf__ T* dst, __ubuf__ T* src)
{
    vtranspose((__ubuf__ uint16_t*)dst, (__ubuf__ uint16_t*)src);
}

template <typename T>
struct Transpose4dParams {
    __aicore__ Transpose4dParams() {};

    uint8_t blockSize = 1;
    uint16_t tmp1RemainRowCount = 0;
    uint16_t tmp1CopyCount = 0;
    uint32_t tmp1NeedRowCount = 0;
    uint16_t tmp2Count = 0;
    uint16_t tmp2NeedRowCount = NCHW_CONV_ADDR_LIST_SIZE;
    uint16_t tmp3RemainRowCount = 0;
    uint16_t copyCIndex = 0;
    uint16_t srcBlockIndex = 0;
    uint32_t preCinnerOffset = 0;
    uint32_t preCoffset = 0;
    uint16_t dstBlockNum = 0;
    uint16_t dstNeedBlockNum = 0;
    uint16_t imageSize = 0;
    uint32_t oneChwSize = 0;
    uint16_t transRowCount = NCHW_CONV_ADDR_LIST_SIZE;
    uint16_t copyColCount = NCHW_CONV_ADDR_LIST_SIZE;
    uint16_t transLen = 0;
    uint32_t preTmpLen = B16_TMP_ELE_LEN;
    uint32_t dstAllBlockNum = 0;
    uint32_t imageBlockNum = 0;

    // main：src->tmp1
    TransDataTo5HDParams transDataParams1;

    __ubuf__ T* dstList1[NCHW_CONV_ADDR_LIST_SIZE];
    __ubuf__ T* srcList1[NCHW_CONV_ADDR_LIST_SIZE];

    DataCopyParams dataCopyParams1;
    DataCopyParams dataCopyParams2;

    // main：tmp2->tmp3
    TransDataTo5HDParams transDataParams2;

    __ubuf__ T* dstList2[NCHW_CONV_ADDR_LIST_SIZE];
    __ubuf__ T* srcList2[NCHW_CONV_ADDR_LIST_SIZE];
};

template <uint32_t size = sizeof(uint8_t)>
struct ExtractTransposeTypeBySize {
    using T = uint8_t;
};

template <>
struct ExtractTransposeTypeBySize<sizeof(uint16_t)> {
    using T = uint16_t;
};

template <>
struct ExtractTransposeTypeBySize<sizeof(uint32_t)> {
    using T = uint32_t;
};

template <typename T>
__aicore__ inline void TransBroadCastForB8Cal(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, Transpose4dParams<T>& params)
{
    for (int32_t m = 0; m < NCHW_CONV_ADDR_LIST_SIZE; m++) {
        params.dstList1[m] = (__ubuf__ T*)dstLocal[m * params.blockSize].GetPhyAddr();
    }
    for (int32_t n = 0; n < NCHW_CONV_ADDR_LIST_SIZE; n++) {
        params.srcList1[n] = (__ubuf__ T*)srcLocal[params.srcBlockIndex * params.blockSize].GetPhyAddr();
    }
    params.transDataParams1.dstRepStride = params.transDataParams1.repeatTimes > 1 ? ONE_BLK_SIZE : 0;
    params.transDataParams1.srcRepStride = params.transDataParams1.repeatTimes > 1 ? (params.imageBlockNum) : 0;
    params.transDataParams1.dstHighHalf = false;
    params.transDataParams1.srcHighHalf = false;
    TransDataTo5HDImpl<T>(params.dstList1, params.srcList1, params.transDataParams1);
    PipeBarrier<PIPE_V>();
    params.transDataParams1.dstHighHalf = true;
    params.transDataParams1.srcHighHalf = false;
    TransDataTo5HDImpl<T>(params.dstList1, params.srcList1, params.transDataParams1);
    PipeBarrier<PIPE_V>();

    for (int32_t m = 0; m < NCHW_CONV_ADDR_LIST_SIZE; m++) {
        params.dstList1[m] = (__ubuf__ T*)dstLocal[B8_TRANS_FRACTAL + m * params.blockSize].GetPhyAddr();
    }
    for (int32_t n = 0; n < NCHW_CONV_ADDR_LIST_SIZE; n++) {
        params.srcList1[n] = (__ubuf__ T*)srcLocal[params.srcBlockIndex * params.blockSize].GetPhyAddr();
    }
    params.transDataParams1.dstRepStride = params.transDataParams1.repeatTimes > 1 ? ONE_BLK_SIZE : 0;
    params.transDataParams1.srcRepStride = params.transDataParams1.repeatTimes > 1 ? (params.imageBlockNum) : 0;
    params.transDataParams1.dstHighHalf = false;
    params.transDataParams1.srcHighHalf = true;
    TransDataTo5HDImpl<T>(params.dstList1, params.srcList1, params.transDataParams1);
    PipeBarrier<PIPE_V>();
    params.transDataParams1.dstHighHalf = true;
    params.transDataParams1.srcHighHalf = true;
    TransDataTo5HDImpl<T>(params.dstList1, params.srcList1, params.transDataParams1);
}

template <typename T>
__aicore__ inline void TransBroadCastCal(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, Transpose4dParams<T>& params)
{
    if constexpr (sizeof(T) == sizeof(half)) {
        for (int32_t m = 0; m < NCHW_CONV_ADDR_LIST_SIZE; m++) {
            params.dstList1[m] = (__ubuf__ T*)dstLocal[m * params.blockSize].GetPhyAddr();
        }
        for (int32_t n = 0; n < NCHW_CONV_ADDR_LIST_SIZE; n++) {
            params.srcList1[n] = (__ubuf__ T*)srcLocal[params.srcBlockIndex * params.blockSize].GetPhyAddr();
        }
        TransDataTo5HDImpl<T>(params.dstList1, params.srcList1, params.transDataParams1);
    } else if constexpr (sizeof(T) == sizeof(uint8_t)) {
        TransBroadCastForB8Cal(dstLocal, srcLocal, params);
    } else {
        for (int32_t m = 0; m < NCHW_CONV_ADDR_LIST_SIZE; m = m + TWO_NUM) {
            params.dstList1[m] = (__ubuf__ T*)dstLocal[NCHW_CONV_ADDR_LIST_SIZE * (m / TWO_NUM)].GetPhyAddr();
            params.dstList1[m + 1] =
                (__ubuf__ T*)dstLocal[NCHW_CONV_ADDR_LIST_SIZE * (m / TWO_NUM) + params.blockSize].GetPhyAddr();
        }
        for (int32_t n = 0; n < NCHW_CONV_ADDR_LIST_SIZE; n++) {
            params.srcList1[n] = (__ubuf__ T*)srcLocal[params.srcBlockIndex * params.blockSize].GetPhyAddr();
        }
        TransDataTo5HDImpl<T>(params.dstList1, params.srcList1, params.transDataParams1);
    }
    PipeBarrier<PIPE_V>();
}

template <typename T>
__aicore__ inline void CopyFirstBlockCal(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const TransposeParamsExt& transposeParams,
    Transpose4dParams<T>& params, DataCopyParams& dataCopyParams)
{
    if ((params.dstNeedBlockNum != 0) && (params.tmp3RemainRowCount != 0)) {
        DataCopy(dstLocal[params.dstBlockNum * (params.blockSize)], srcLocal, dataCopyParams);
        params.dstNeedBlockNum -= params.tmp3RemainRowCount;
        params.dstBlockNum += params.tmp3RemainRowCount;
        params.tmp3RemainRowCount = 0;
    }
}

template <typename T>
__aicore__ inline void UpdateCopyToTmp2ParamCal(Transpose4dParams<T>& params, const TransposeParamsExt& transposeParams)
{
    params.copyCIndex += 1;
    params.tmp2NeedRowCount -= 1;
    params.tmp1CopyCount += 1;
    params.tmp2Count += 1;
    params.tmp1RemainRowCount -= 1;
    if (params.copyCIndex == transposeParams.cSize) {
        params.copyCIndex = 0;
    }
}

template <typename T>
__aicore__ inline void TransFracForB8Cal(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, Transpose4dParams<T>& params)
{
    for (int32_t m = 0; m < NCHW_CONV_ADDR_LIST_SIZE; m++) {
        params.dstList2[m] = (__ubuf__ T*)dstLocal[m * params.blockSize].GetPhyAddr();
    }
    for (int32_t n = 0; n < NCHW_CONV_ADDR_LIST_SIZE; n++) {
        params.srcList2[n] = (__ubuf__ T*)srcLocal[n * params.blockSize].GetPhyAddr();
    }
    params.transDataParams2.dstHighHalf = false;
    params.transDataParams2.srcHighHalf = false;
    TransDataTo5HDImpl<T>(params.dstList2, params.srcList2, params.transDataParams2);
    PipeBarrier<PIPE_V>();

    for (int32_t m = 0; m < NCHW_CONV_ADDR_LIST_SIZE; m++) {
        params.dstList2[m] = (__ubuf__ T*)dstLocal[B8_TRANS_FRACTAL + m * params.blockSize].GetPhyAddr();
    }
    for (int32_t n = 0; n < NCHW_CONV_ADDR_LIST_SIZE; n++) {
        params.srcList2[n] = (__ubuf__ T*)srcLocal[n * params.blockSize].GetPhyAddr();
    }
    params.transDataParams2.dstHighHalf = false;
    params.transDataParams2.srcHighHalf = true;
    TransDataTo5HDImpl<T>(params.dstList2, params.srcList2, params.transDataParams2);
    PipeBarrier<PIPE_V>();

    for (int32_t m = 0; m < NCHW_CONV_ADDR_LIST_SIZE; m++) {
        params.dstList2[m] = (__ubuf__ T*)dstLocal[m * params.blockSize].GetPhyAddr();
    }
    for (int32_t n = 0; n < NCHW_CONV_ADDR_LIST_SIZE; n++) {
        params.srcList2[n] = (__ubuf__ T*)srcLocal[B8_TRANS_FRACTAL + n * params.blockSize].GetPhyAddr();
    }
    params.transDataParams2.dstHighHalf = true;
    params.transDataParams2.srcHighHalf = false;
    TransDataTo5HDImpl<T>(params.dstList2, params.srcList2, params.transDataParams2);
    PipeBarrier<PIPE_V>();

    for (int32_t m = 0; m < NCHW_CONV_ADDR_LIST_SIZE; m++) {
        params.dstList2[m] = (__ubuf__ T*)dstLocal[B8_TRANS_FRACTAL + m * params.blockSize].GetPhyAddr();
    }
    for (int32_t n = 0; n < NCHW_CONV_ADDR_LIST_SIZE; n++) {
        params.srcList2[n] = (__ubuf__ T*)srcLocal[B8_TRANS_FRACTAL + n * params.blockSize].GetPhyAddr();
    }
    params.transDataParams2.dstHighHalf = true;
    params.transDataParams2.srcHighHalf = true;
    TransDataTo5HDImpl<T>(params.dstList2, params.srcList2, params.transDataParams2);
}

template <typename T>
__aicore__ inline void TransFracCal(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, Transpose4dParams<T>& params)
{
    if constexpr (sizeof(T) == sizeof(half)) {
        for (int32_t m = 0; m < NCHW_CONV_ADDR_LIST_SIZE; m++) {
            params.dstList2[m] = (__ubuf__ T*)dstLocal[m * params.blockSize].GetPhyAddr();
        }
        for (int32_t n = 0; n < NCHW_CONV_ADDR_LIST_SIZE; n++) {
            params.srcList2[n] = (__ubuf__ T*)srcLocal[n * params.blockSize].GetPhyAddr();
        }
        TransDataTo5HDImpl<T>(params.dstList2, params.srcList2, params.transDataParams2);
    } else if constexpr (sizeof(T) == sizeof(uint8_t)) {
        TransFracForB8Cal(dstLocal, srcLocal, params);
    } else {
        for (int32_t m = 0; m < NCHW_CONV_ADDR_LIST_SIZE; m = m + TWO_NUM) {
            params.dstList2[m] = (__ubuf__ T*)dstLocal[NCHW_CONV_ADDR_LIST_SIZE * (m / TWO_NUM)].GetPhyAddr();
            params.dstList2[m + 1] =
                (__ubuf__ T*)dstLocal[NCHW_CONV_ADDR_LIST_SIZE * (m / TWO_NUM) + params.blockSize].GetPhyAddr();
        }
        for (int32_t n = 0; n < NCHW_CONV_ADDR_LIST_SIZE; n++) {
            params.srcList2[n] = (__ubuf__ T*)srcLocal[n * params.blockSize].GetPhyAddr();
        }
        TransDataTo5HDImpl<T>(params.dstList2, params.srcList2, params.transDataParams2);
    }
    PipeBarrier<PIPE_V>();
}

template <typename T>
__aicore__ inline void UpdateTransToTmp3ParamCal(
    Transpose4dParams<T>& params, const uint16_t cSize, const TransposeType transposeType)
{
    if (transposeType == TransposeType::TRANSPOSE_NCHW2NHWC) {
        params.tmp3RemainRowCount = 1;
        params.tmp2Count = 0;
        if (params.dstNeedBlockNum == 1) {
            params.tmp2NeedRowCount = params.imageSize % params.transRowCount == 0 ?
                                          params.transRowCount :
                                          (params.imageSize % params.transRowCount);
        } else {
            params.tmp2NeedRowCount = params.transRowCount;
        }
        if ((params.dstNeedBlockNum > 1) && (sizeof(T) == sizeof(float))) {
            params.tmp3RemainRowCount = TWO_NUM;
        }
    } else if (transposeType == TransposeType::TRANSPOSE_NHWC2NCHW) {
        params.tmp3RemainRowCount = 1;
        params.tmp2Count = 0;
        params.tmp2NeedRowCount = cSize * params.transRowCount;

        if (sizeof(T) == sizeof(float) && (params.dstNeedBlockNum != 1)) {
            params.tmp3RemainRowCount = TWO_NUM;
        }
    }
}

template <typename T>
__aicore__ inline void Transpose2HwcCal(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<T>& sharedTmpBuffer,
    const TransposeParamsExt& transposeParams, Transpose4dParams<T>& params)
{
    LocalTensor<T> tempTensorA = sharedTmpBuffer;
    LocalTensor<T> tempTensorB = sharedTmpBuffer[transposeParams.cSize * params.preTmpLen];
    LocalTensor<T> tempTensorC = sharedTmpBuffer[(transposeParams.cSize + 1) * params.preTmpLen];

    while (params.dstNeedBlockNum != 0) {
        // 1、transdataTo5hd: src->tmp1
        if (params.tmp1RemainRowCount == 0) {
            TransBroadCastCal(tempTensorA, srcLocal, params);
            // update the start address of src
            params.srcBlockIndex += 1;
            // update remaining row numbers of tmp1
            if (params.dstNeedBlockNum == 1) {
                // update last tail block section
                params.tmp1RemainRowCount = params.oneChwSize % params.tmp1NeedRowCount == 0 ?
                                                transposeParams.cSize * params.tmp1NeedRowCount :
                                                (params.oneChwSize % params.tmp1NeedRowCount);
            } else {
                params.tmp1RemainRowCount = transposeParams.cSize * params.tmp1NeedRowCount;
            }
        }
        PipeBarrier<PIPE_V>();
        // 2、datacopy:tmp1->tmp2: jump to carry one block from each C dimension and splice them
        while (params.tmp2NeedRowCount != 0) {
            params.dataCopyParams1.blockCount = 1;
            params.dataCopyParams1.blockLen = 1;

            params.preCinnerOffset = (params.copyColCount * (params.tmp1CopyCount / transposeParams.cSize));
            params.preCoffset = (params.transLen) * (params.copyCIndex % transposeParams.cSize);
            DataCopy(
                tempTensorB[(params.tmp2Count % params.transRowCount) * (params.blockSize)],
                tempTensorA[params.preCoffset + params.preCinnerOffset], params.dataCopyParams1);
            // update params
            UpdateCopyToTmp2ParamCal(params, transposeParams);
            // When all valid data in tmp1 is transferred, the operation stops. Reload from src
            if (params.tmp1RemainRowCount == 0) {
                params.tmp1CopyCount = 0;
                break;
            }
            PipeBarrier<PIPE_V>();
        }
        // 3、transdataTo5hd:tmp2->tmp3
        if (params.tmp2NeedRowCount == 0) {
            TransFracCal(tempTensorC, tempTensorB, params);
            // update params
            UpdateTransToTmp3ParamCal(params, transposeParams.cSize, TransposeType::TRANSPOSE_NCHW2NHWC);
        }
        PipeBarrier<PIPE_V>();
        // 4、dataCopy tmp3->dst
        params.dataCopyParams1.blockCount = 1;
        params.dataCopyParams1.blockLen = params.tmp3RemainRowCount;
        CopyFirstBlockCal(dstLocal, tempTensorC, transposeParams, params, params.dataCopyParams1);
        PipeBarrier<PIPE_V>();
    }
}

template <typename T>
__aicore__ inline void UpdateTransToTmp1ParamCal(Transpose4dParams<T>& params)
{
    // update the start address of src
    params.srcBlockIndex += 1;
    // update remaining row numbers of tmp1
    if (params.dstNeedBlockNum == 1) {
        // update last tail block section
        params.tmp1RemainRowCount = params.oneChwSize % params.tmp1NeedRowCount == 0 ?
                                        params.tmp1NeedRowCount :
                                        (params.oneChwSize % params.tmp1NeedRowCount);
    } else {
        params.tmp1RemainRowCount = params.tmp1NeedRowCount;
    }
}

template <typename T>
__aicore__ inline void TransLastFracCal(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, Transpose4dParams<T>& params)
{
    for (int32_t m = 0; m < NCHW_CONV_ADDR_LIST_SIZE; m = m + TWO_NUM) {
        params.dstList2[m] = (__ubuf__ T*)dstLocal[NCHW_CONV_ADDR_LIST_SIZE * (m / TWO_NUM)].GetPhyAddr();
        params.dstList2[m + 1] =
            (__ubuf__ T*)dstLocal[NCHW_CONV_ADDR_LIST_SIZE * (m / TWO_NUM) + params.blockSize].GetPhyAddr();
    }
    for (int32_t n = 0; n < NCHW_CONV_ADDR_LIST_SIZE / TWO_NUM; n++) {
        params.srcList2[n] = (__ubuf__ T*)srcLocal[n * params.blockSize].GetPhyAddr();
        params.srcList2[n + NCHW_CONV_ADDR_LIST_SIZE / TWO_NUM] =
            (__ubuf__ T*)srcLocal[n * params.blockSize].GetPhyAddr();
    }
    TransDataTo5HDImpl<T>(params.dstList2, params.srcList2, params.transDataParams2);
}

template <typename T>
__aicore__ inline void CopyTodstForChwCal(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint16_t cSize, Transpose4dParams<T>& params)
{
    params.dataCopyParams2.blockCount = cSize;
    params.dataCopyParams2.blockLen = params.tmp3RemainRowCount;
    params.dataCopyParams2.srcStride = params.preTmpLen * sizeof(T) / ONE_BLK_SIZE - params.tmp3RemainRowCount;
    params.dataCopyParams2.dstStride = params.imageBlockNum - params.tmp3RemainRowCount;

    if ((params.dstNeedBlockNum != 0) && (params.tmp3RemainRowCount != 0)) {
        DataCopy(dstLocal[params.dstBlockNum * (params.blockSize)], srcLocal, params.dataCopyParams2);
        params.dstNeedBlockNum -= params.tmp3RemainRowCount;
        params.dstBlockNum += params.tmp3RemainRowCount;
        params.dstAllBlockNum = params.tmp3RemainRowCount * cSize;
        params.tmp3RemainRowCount = 0;
    }
    if (sizeof(T) == sizeof(float) && (params.dstNeedBlockNum == 1) && (params.dstAllBlockNum != 0) &&
        (params.srcBlockIndex % params.dstAllBlockNum == 0)) {
        params.tmp2NeedRowCount = cSize * (params.imageSize % NCHW_CONV_ADDR_LIST_SIZE);
    }
    PipeBarrier<PIPE_V>();
}

template <typename T>
__aicore__ inline void Transpose2ChwCal(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<T>& sharedTmpBuffer,
    const TransposeParamsExt& transposeParams, Transpose4dParams<T>& params)
{
    LocalTensor<T> tempTensorA = sharedTmpBuffer;                   // size: params.preTmpLen;
    LocalTensor<T> tempTensorB = sharedTmpBuffer[params.preTmpLen]; // size: transposeParams.cSize * params.preTmpLen;
    // size: transposeParams.cSize * params.preTmpLen;
    LocalTensor<T> tempTensorC = sharedTmpBuffer[(transposeParams.cSize + 1) * params.preTmpLen];
    while (params.dstNeedBlockNum != 0) {
        if (params.tmp1RemainRowCount == 0) {
            TransBroadCastCal(tempTensorA, srcLocal, params);
            // update params
            UpdateTransToTmp1ParamCal(params);
        }
        PipeBarrier<PIPE_V>();
        // 2、datacopy:tmp1->tmp2:transfer one block in each C dimension from src to dst.
        while (params.tmp1RemainRowCount != 0) {
            params.dataCopyParams1.blockCount = 1;
            params.dataCopyParams1.blockLen = 1;
            params.preCinnerOffset = (params.copyColCount * (params.tmp2Count / transposeParams.cSize));
            params.preCoffset = params.transLen * (params.copyCIndex % transposeParams.cSize);
            DataCopy(
                tempTensorB[params.preCoffset + params.preCinnerOffset],
                tempTensorA[(params.tmp2Count % params.blockSize) * params.transRowCount], params.dataCopyParams1);
            // update params
            UpdateCopyToTmp2ParamCal(params, transposeParams);
            // When all valid data in tmp1 is transferred, the operation stops. Reload from src
            if (params.tmp1RemainRowCount == 0) {
                params.tmp1CopyCount = 0;
                break;
            }
            PipeBarrier<PIPE_V>();
        }
        // 3、transdataTo5hd:tmp2->tmp3
        if (params.tmp2NeedRowCount == 0) {
            if ((params.imageSize % NCHW_CONV_ADDR_LIST_SIZE != 0) && (params.dstNeedBlockNum == 1)) {
                for (int16_t k = 0; k < transposeParams.cSize; k++) {
                    TransLastFracCal(tempTensorC[k * params.preTmpLen], tempTensorB[k * params.preTmpLen], params);
                }
            } else {
                for (int16_t k = 0; k < transposeParams.cSize; k++) {
                    TransFracCal(tempTensorC[k * params.preTmpLen], tempTensorB[k * params.preTmpLen], params);
                }
            }
            UpdateTransToTmp3ParamCal(params, transposeParams.cSize, TransposeType::TRANSPOSE_NHWC2NCHW);
        }
        PipeBarrier<PIPE_V>();
        // 4、dataCopy tmp3->dst
        CopyTodstForChwCal(dstLocal, tempTensorC, transposeParams.cSize, params);
    }
}

template <typename T>
__aicore__ inline void GetTransposeParamCal(const TransposeParamsExt& transposeParams, Transpose4dParams<T>& params)
{
    params.imageSize = transposeParams.hSize * transposeParams.wSize;
    params.imageBlockNum = params.imageSize * sizeof(T) / ONE_BLK_SIZE;
    params.oneChwSize = params.imageSize * transposeParams.cSize;
    params.blockSize = ONE_BLK_SIZE / sizeof(T);
    params.tmp1NeedRowCount = sizeof(T) == 1 ? ONE_BLK_SIZE : params.blockSize;

    params.transDataParams1.repeatTimes = transposeParams.cSize;
    params.transDataParams1.dstRepStride = params.transDataParams1.repeatTimes > 1 ? NCHW_CONV_ADDR_LIST_SIZE : 0;
    params.transDataParams1.srcRepStride = params.transDataParams1.repeatTimes > 1 ? (params.imageBlockNum) : 0;

    params.transLen = BYTE_PER_FRACTAL / sizeof(T);
    if constexpr (sizeof(T) == sizeof(uint8_t)) {
        params.transRowCount = B8_TRANS_ROW;
        params.copyColCount = B8_COPY_COL;
        params.preTmpLen = B8_TMP_ELE_LEN;
        params.transLen = B8_TRANS_LEN / sizeof(T);
    } else if constexpr (sizeof(T) == sizeof(float)) {
        params.preTmpLen = B32_TMP_ELE_LEN;
        if (transposeParams.transposeType == TransposeType::TRANSPOSE_NHWC2NCHW) {
            params.copyColCount = ONE_BLK_SIZE / sizeof(T);
        }
    }
}

template <typename T>
__aicore__ inline void TransposeUB2UBImpl(__ubuf__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
    static_assert((SupportBytes<T, 1, 2, 4>()), "Transpose only supports type fp4/b8/b16/b32 on current device");
    DataCopyUB2UBImpl(dst, src, intriParams);
}

template <typename T>
__aicore__ inline void Transpose4DImpl(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const TransposeParamsExt& transposeParams)
{
    static_assert((SupportBytes<T, 1, 2, 4>()), "Transpose only supports type fp4/b8/b16/b32 on current device");
    ASCENDC_ASSERT((sharedTmpBuffer.GetSize() > 0), { KERNEL_LOG(KERNEL_ERROR, "sharedTmpBuffer size must > 0!"); });
    using Transpose4DType = typename ExtractTransposeTypeBySize<sizeof(T)>::T;
    LocalTensor<Transpose4DType> stackBuffer = sharedTmpBuffer.ReinterpretCast<Transpose4DType>();
    LocalTensor<Transpose4DType> dstTmpLocal = dstLocal.template ReinterpretCast<Transpose4DType>();
    LocalTensor<Transpose4DType> srcTmpLocal = srcLocal.template ReinterpretCast<Transpose4DType>();

    Transpose4dParams<Transpose4DType> params;
    GetTransposeParamCal<Transpose4DType>(transposeParams, params);

    if (transposeParams.transposeType == TransposeType::TRANSPOSE_NCHW2NHWC) {
        for (int16_t i = 0; i < transposeParams.nSize; i++) {
            params.dstBlockNum = 0;   // blockNum for entire dst
            params.srcBlockIndex = 0; // srcLocal has an offset, and srcBlockIndex needs to be set to 0 each time
            params.copyCIndex = 0;
            params.tmp1RemainRowCount = 0;
            params.tmp1CopyCount = 0;
            params.dstNeedBlockNum = params.oneChwSize * sizeof(T) / ONE_BLK_SIZE;
            params.tmp2NeedRowCount =
                params.dstNeedBlockNum == 1 ? (params.imageSize % params.transRowCount) : params.transRowCount;
            Transpose2HwcCal<Transpose4DType>(
                dstTmpLocal[i * params.oneChwSize], srcTmpLocal[i * params.oneChwSize], stackBuffer, transposeParams,
                params);
        }
    } else if (transposeParams.transposeType == TransposeType::TRANSPOSE_NHWC2NCHW) {
        for (int16_t i = 0; i < transposeParams.nSize; i++) {
            params.transDataParams1.repeatTimes = 1;
            params.transDataParams1.dstRepStride =
                params.transDataParams1.repeatTimes > 1 ? NCHW_CONV_ADDR_LIST_SIZE : 0;
            params.transDataParams1.srcRepStride = params.transDataParams1.repeatTimes > 1 ? (params.imageBlockNum) : 0;
            params.dstBlockNum = 0;    // blockNum of each c dimension
            params.dstAllBlockNum = 0; // blockNum for entire dst
            params.srcBlockIndex = 0;  // srcLocal has an offset, and srcBlockIndex needs to be set to 0 each time
            params.dstNeedBlockNum =
                params.imageBlockNum; // In the copy->dst part, repeat is used to transfer dimension C. Therefore,
                                      // dstNeedBlockNum is calculated as h x w.
            params.tmp2NeedRowCount =
                params.oneChwSize * sizeof(T) / ONE_BLK_SIZE == 1 ?
                    (params.imageSize % params.transRowCount) :
                    transposeParams.cSize * params.transRowCount; // In tmp2, c transposed fractals need to be
                                                                  // collected, and then transposing is performed.
            // float type: when H x W = 8, tmp2NeedRowCount = c x 8.
            if ((sizeof(T) == sizeof(float)) && (params.dstNeedBlockNum == 1)) {
                params.tmp2NeedRowCount = transposeParams.cSize * params.blockSize;
            }
            Transpose2ChwCal<Transpose4DType>(
                dstTmpLocal[i * params.oneChwSize], srcTmpLocal[i * params.oneChwSize], stackBuffer, transposeParams,
                params);
        }
    }
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_TRANSPOSE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_TRANSPOSE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_TRANSPOSE_IMPL_H__
#endif
