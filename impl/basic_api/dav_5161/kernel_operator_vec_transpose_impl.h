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
    "impl/basic_api/dav_5161/kernel_operator_vec_transpose_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_TRANSPOSE_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_TRANSPOSE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_TRANSPOSE_IMPL_H

#include "../../../include/basic_api/kernel_operator_block_sync_intf.h"

namespace AscendC {
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

__aicore__ inline void SetVaReg(uint64_t dst[NCHW_CONV_ADDR_LIST_SIZE], uint64_t src[NCHW_CONV_ADDR_LIST_SIZE])
{
    set_va_reg_sb(VA0, dst);
    set_va_reg_sb(VA1, dst + VA_REG_ARRAY_LEN);
    set_va_reg_sb(VA2, src);
    set_va_reg_sb(VA3, src + VA_REG_ARRAY_LEN);
}

template <typename T>
__aicore__ inline void TransDataTo5HDImpl(
    uint64_t dstList[NCHW_CONV_ADDR_LIST_SIZE], uint64_t srcList[NCHW_CONV_ADDR_LIST_SIZE],
    const TransDataTo5HDParams& transDataTo5HDParams)
{
    SetVaReg(dstList, srcList);
    TransDataTo5HDIntrinsicsImpl<T>(dstList, srcList, transDataTo5HDParams);
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_TRANSPOSE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_TRANSPOSE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_TRANSPOSE_IMPL_H__
#endif
