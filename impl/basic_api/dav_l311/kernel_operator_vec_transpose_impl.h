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
 * \file kernel_operator_vec_transpose_impl.h
 * \brief AscendC l311 support vector transpose api.
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_l311/kernel_operator_vec_transpose_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_TRANSPOSE_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_TRANSPOSE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_TRANSPOSE_IMPL_H

namespace AscendC {
/* **************************************************************************************************
 * TransdataTo5HD                                             *
 * ************************************************************************************************* */
// TransdataTo5HD::Level 0
#define TRANSDATA_IMPL(TYPE, WIDTH)                                                                               \
    __aicore__ inline void TransDataTo5HDIntrinsicsImpl(                                                          \
        __ubuf__ TYPE* dstList[16], __ubuf__ TYPE* srcList[16], const TransDataTo5HDParams& transDataTo5HDParams) \
    {                                                                                                             \
        scatter_vnchwconv_b##WIDTH(                                                                               \
            VA0, VA2, transDataTo5HDParams.repeatTimes, transDataTo5HDParams.dstRepStride,                        \
            transDataTo5HDParams.srcRepStride);                                                                   \
    }

#define TRANSDATA_LIST_IMPL(TYPE, WIDTH)                                                              \
    template <>                                                                                       \
    __aicore__ inline void TransDataTo5HDIntrinsicsImpl<TYPE>(                                        \
        uint64_t dstList[16], uint64_t srcList[16], const TransDataTo5HDParams& transDataTo5HDParams) \
    {                                                                                                 \
        scatter_vnchwconv_b##WIDTH(                                                                   \
            VA0, VA2, transDataTo5HDParams.repeatTimes, transDataTo5HDParams.dstRepStride,            \
            transDataTo5HDParams.srcRepStride);                                                       \
    }

TRANSDATA_IMPL(float, 32);
TRANSDATA_IMPL(int32_t, 32);
TRANSDATA_IMPL(uint32_t, 32);
TRANSDATA_IMPL(half, 16);
TRANSDATA_IMPL(int16_t, 16);
TRANSDATA_IMPL(uint16_t, 16);

template <typename T>
__aicore__ inline void TransDataTo5HDIntrinsicsImpl(
    uint64_t dstList[16], uint64_t srcList[16], const TransDataTo5HDParams& transDataTo5HDParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "TransDataTo5HD with current dtype is not supported"); });
}

TRANSDATA_LIST_IMPL(float, 32);
TRANSDATA_LIST_IMPL(int32_t, 32);
TRANSDATA_LIST_IMPL(uint32_t, 32);
TRANSDATA_LIST_IMPL(half, 16);
TRANSDATA_LIST_IMPL(int16_t, 16);
TRANSDATA_LIST_IMPL(uint16_t, 16);

template <typename T>
__aicore__ inline void TransDataTo5HDB8IntrinsicsImpl(
    __ubuf__ T* dstList[16], __ubuf__ T* srcList[16], const TransDataTo5HDParams& transDataTo5HDParams)
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
__aicore__ inline void TransDataTo5HDB8IntrinsicsImpl(
    uint64_t dstList[16], uint64_t srcList[16], const TransDataTo5HDParams& transDataTo5HDParams)
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

/* **************************************************************************************************
 * Transpose                                             *
 * ************************************************************************************************* */
// Transpose::Level 0
template <typename T>
__aicore__ inline void TransposeIntrinsicsImpl(__ubuf__ T* dst, __ubuf__ T* src)
{
    vtranspose((__ubuf__ uint16_t*)dst, (__ubuf__ uint16_t*)src);
}

template <typename T>
__aicore__ inline void TransposeImpl(__ubuf__ T* dst, __ubuf__ T* src)
{
    TransposeIntrinsicsImpl((__ubuf__ uint16_t*)dst, (__ubuf__ uint16_t*)src);
}

template <typename T>
__aicore__ inline void TransposeUB2UBImpl(__ubuf__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
    static_assert((SupportBytes<T, 1, 2, 4>()), "Transpose only supports type fp4/b8/b16/b32 on current device");
    DataCopyUB2UBImpl(dst, src, intriParams);
}

template <typename T>
typename std::enable_if<(sizeof(T) == 1)>::type __aicore__ inline Transpose4DImpl(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const TransposeParamsExt& transposeParams)
{
    uint16_t imageSize = transposeParams.hSize * transposeParams.wSize;
    uint32_t channelImageSize = imageSize * transposeParams.cSize;
    if (transposeParams.transposeType == TransposeType::TRANSPOSE_NCHW2NHWC) {
        for (int i = 0; i < transposeParams.nSize; i++) {
            v4dtrans(
                (__ubuf__ uint8_t*)dst[channelImageSize * i].GetPhyAddr(),
                (__ubuf__ uint8_t*)src[channelImageSize * i].GetPhyAddr(), imageSize, transposeParams.cSize, false);
        }
    } else if (transposeParams.transposeType == TransposeType::TRANSPOSE_NHWC2NCHW) {
        for (int i = 0; i < transposeParams.nSize; i++) {
            v4dtrans(
                (__ubuf__ uint8_t*)dst[channelImageSize * i].GetPhyAddr(),
                (__ubuf__ uint8_t*)src[channelImageSize * i].GetPhyAddr(), imageSize, transposeParams.cSize, true);
        }
    }
}

template <typename T>
typename std::enable_if<(sizeof(T) == 2)>::type __aicore__ inline Transpose4DImpl(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const TransposeParamsExt& transposeParams)
{
    uint16_t imageSize = transposeParams.hSize * transposeParams.wSize;
    uint32_t channelImageSize = imageSize * transposeParams.cSize;
    if (transposeParams.transposeType == TransposeType::TRANSPOSE_NCHW2NHWC) {
        for (int i = 0; i < transposeParams.nSize; i++) {
            v4dtrans(
                (__ubuf__ uint16_t*)dst[channelImageSize * i].GetPhyAddr(),
                (__ubuf__ uint16_t*)src[channelImageSize * i].GetPhyAddr(), imageSize, transposeParams.cSize, false);
        }
    } else if (transposeParams.transposeType == TransposeType::TRANSPOSE_NHWC2NCHW) {
        for (int i = 0; i < transposeParams.nSize; i++) {
            v4dtrans(
                (__ubuf__ uint16_t*)dst[channelImageSize * i].GetPhyAddr(),
                (__ubuf__ uint16_t*)src[channelImageSize * i].GetPhyAddr(), imageSize, transposeParams.cSize, true);
        }
    }
}

template <typename T>
typename std::enable_if<(sizeof(T) == 4)>::type __aicore__ inline Transpose4DImpl(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const TransposeParamsExt& transposeParams)
{
    uint16_t imageSize = transposeParams.hSize * transposeParams.wSize;
    uint32_t channelImageSize = imageSize * transposeParams.cSize;
    if (transposeParams.transposeType == TransposeType::TRANSPOSE_NCHW2NHWC) {
        for (int i = 0; i < transposeParams.nSize; i++) {
            v4dtrans(
                (__ubuf__ uint32_t*)dst[channelImageSize * i].GetPhyAddr(),
                (__ubuf__ uint32_t*)src[channelImageSize * i].GetPhyAddr(), imageSize, transposeParams.cSize, false);
        }
    } else if (transposeParams.transposeType == TransposeType::TRANSPOSE_NHWC2NCHW) {
        for (int i = 0; i < transposeParams.nSize; i++) {
            v4dtrans(
                (__ubuf__ uint32_t*)dst[channelImageSize * i].GetPhyAddr(),
                (__ubuf__ uint32_t*)src[channelImageSize * i].GetPhyAddr(), imageSize, transposeParams.cSize, true);
        }
    }
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_TRANSPOSE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_TRANSPOSE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_TRANSPOSE_IMPL_H__
#endif
