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
 * \file kernel_operator_data_copy_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_m310/kernel_operator_data_copy_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_DATA_COPY_IMPL_H
#define ASCENDC_MODULE_OPERATOR_DATA_COPY_IMPL_H
#include "../../../include/basic_api/kernel_common.h"
#include "kernel_operator_vec_duplicate_impl.h"
#include "../../../include/basic_api/kernel_struct_unary.h"

namespace AscendC {
/* **************************************************************************************************
 * DataCopy                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void DataCopyGM2L1Impl(__cbuf__ T* dst, __gm__ T* src, const DataCopyParams& intriParams)
{
    copy_gm_to_cbuf(
        (__cbuf__ void*)dst, (__gm__ void*)src, (int8_t)0, static_cast<uint16_t>(intriParams.blockCount),
        static_cast<uint16_t>(intriParams.blockLen), static_cast<uint16_t>(intriParams.srcStride),
        static_cast<uint16_t>(intriParams.dstStride), (pad_t)0);
}

template <typename T>
__aicore__ inline void DataCopyUB2UBImpl(__ubuf__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
    copy_ubuf_to_ubuf(
        (__ubuf__ void*)dst, (__ubuf__ void*)src, 0, intriParams.blockCount, intriParams.blockLen,
        intriParams.srcStride, intriParams.dstStride);
}

template <typename T>
__aicore__ inline __in_pipe__(MTE3)
    __out_pipe__(MTE3) void DataCopyUB2L1Impl(__cbuf__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
    copy_ubuf_to_cbuf(
        (__cbuf__ void*)dst, (__ubuf__ void*)src, 0, intriParams.blockCount, intriParams.blockLen,
        intriParams.srcStride, intriParams.dstStride);
}

template <typename T>
__aicore__ inline void DataCopyL12UBImpl(__ubuf__ T* dst, __cbuf__ T* src, const DataCopyParams& intriParams)
{
    copy_cbuf_to_ubuf(
        (__ubuf__ void*)dst, (__cbuf__ void*)src, 0, intriParams.blockCount, intriParams.blockLen,
        intriParams.srcStride, intriParams.dstStride);
}

template <typename T>
__aicore__ inline void DataCopyL12BTImpl(
    const uint64_t dst, __cbuf__ T* src, const uint16_t isEnableConv, const DataCopyParams& intriParams)
{
    if constexpr (IsSameType<T, float>::value || IsSameType<T, int32_t>::value) {
        copy_cbuf_to_bt(
            dst, src, isEnableConv, intriParams.blockCount, intriParams.blockLen, intriParams.srcStride,
            intriParams.dstStride);
    } else {
        ASSERT(false && "unsupported data type of DataCopy from cbuf to bt on current device");
    }
}

template <typename T>
__aicore__ inline __in_pipe__(FIX)
    __out_pipe__(FIX) void DataCopyL12FBImpl(__fbuf__ T* dst, __cbuf__ T* src, const DataCopyParams& intriParams)
{
    copy_cbuf_to_fbuf(
        (__fbuf__ void*)dst, (__cbuf__ void*)src, intriParams.blockCount, intriParams.blockLen, intriParams.srcStride,
        intriParams.dstStride);
}

template <typename T>
__aicore__ inline void TransND2NZ(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t high, uint16_t width, T scalar)
{
    struct UnaryRepeatParams intriParams;
    intriParams.srcBlkStride = 1;
    intriParams.dstBlkStride = 1;
    intriParams.srcRepStride = width * sizeof(T) / ONE_BLK_SIZE;
    intriParams.dstRepStride = 1;

    int highBlock = MAX_REPEAT_TIMES;
    int highBlocks = high / highBlock;
    int highTail = high % highBlock;

    uint64_t mask[2];
    mask[0] = (1 << (32 / sizeof(T))) - 1;
    mask[1] = 0;

    int widthFractal = width * sizeof(T) / 32;
    for (int i = 0; i < widthFractal; ++i) {
        for (int j = 0; j < highBlocks; ++j) {
            AddsImpl(
                dstAddr + i * (32 / sizeof(T)) * high + j * highBlock * (32 / sizeof(T)),
                srcAddr + i * (32 / sizeof(T)) + j * highBlock * width, scalar, mask, highBlock, intriParams);
        }
        if (highTail) {
            AddsImpl(
                dstAddr + i * (32 / sizeof(T)) * high + highBlocks * highBlock * (32 / sizeof(T)),
                srcAddr + i * (32 / sizeof(T)) + highBlocks * highBlock * width, scalar, mask, highTail, intriParams);
        }
    }
}

template <typename T>
__aicore__ inline void DataCopyGM2L1ND2NZImpl(__cbuf__ T* dst, __gm__ T* src, const Nd2NzParams& intriParams)
{
    ASSERT(false && "unsupported data copy from gm to cbuf on current device");
}

template <typename T>
__aicore__ inline void DataCopyUB2GMNZ2NDImplBase(
    __gm__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t high, uint16_t width, uint16_t srcNStride, uint16_t dstDStride)
{
    uint16_t highBlock = MAX_REPEAT_TIMES;
    uint16_t highBlocks = high / highBlock;
    uint16_t highTail = high % highBlock;

    uint16_t widthFractal = width / BLOCK_CUBE;
    for (int i = 0; i < widthFractal; ++i) {
        for (int j = 0; j < highBlocks; ++j) {
            DataCopyUB2GMImpl(
                dstAddr + i * BLOCK_CUBE + j * highBlock * dstDStride,
                srcAddr + i * srcNStride * BLOCK_CUBE + j * highBlock * BLOCK_CUBE,
                {highBlock, BLOCK_CUBE * sizeof(T) / 32, 0,
                 static_cast<uint16_t>((dstDStride - BLOCK_CUBE) * sizeof(T) / 32)});
        }
        if (highTail) {
            DataCopyUB2GMImpl(
                dstAddr + i * BLOCK_CUBE + highBlocks * highBlock * dstDStride,
                srcAddr + i * srcNStride * BLOCK_CUBE + highBlocks * highBlock * BLOCK_CUBE,
                {highTail, BLOCK_CUBE * sizeof(T) / 32, 0,
                 static_cast<uint16_t>((dstDStride - BLOCK_CUBE) * sizeof(T) / 32)});
        }
    }
}

template <typename T>
__aicore__ inline void DataCopyUB2GMNZ2NDImpl(__gm__ T* dst, __ubuf__ T* src, const Nz2NdParamsFull& intriParams)
{
    uint16_t ndNum = intriParams.ndNum;
    uint16_t nValue = intriParams.nValue;
    uint16_t dValue = intriParams.dValue;
    uint16_t srcNdMatrixStride = intriParams.srcNdMatrixStride;
    uint16_t srcNStride = intriParams.srcNStride;
    uint16_t dstDStride = intriParams.dstDStride;
    uint16_t dstNdMatrixStride = intriParams.dstNdMatrixStride;

    for (int i = 0; i < ndNum; ++i) {
        DataCopyUB2GMNZ2NDImplBase(
            dst + i * dstNdMatrixStride, src + i * srcNdMatrixStride * BLOCK_CUBE * BLOCK_CUBE, nValue, dValue,
            srcNStride, dstDStride);
    }
}

/* **************************************************************************************************
 * Copy                                             *
 * ************************************************************************************************* */
// Copy::Level 0 - mask bit mode
template <typename T = int16_t, bool isSetMask = true>
__aicore__ inline void CopyImpl(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, const uint64_t mask[], uint8_t repeatTime,
    const CopyRepeatParams& repeatParams)
{
    if constexpr (isSetMask) {
        SetVectorMask<T>(mask[1], mask[0]);
    }
    __VEC_SCOPE__
    {
        vector_s16 vreg;
        vector_bool preg;
        preg = movp_b16();
        uint32_t strideConfig0 = (static_cast<uint32_t>(repeatParams.srcStride) << 16);
        uint32_t strideConfig1 = (static_cast<uint32_t>(repeatParams.dstStride) << 16);
        for (uint16_t i = 0; i < (static_cast<uint16_t>(repeatTime)); ++i) {
            mem_bar(VST_VLD);
            vsldb(vreg, (__ubuf__ int16_t*)src + i * repeatParams.srcRepeatSize * 16, strideConfig0, preg);
            vsstb(vreg, (__ubuf__ int16_t*)dst + i * repeatParams.dstRepeatSize * 16, strideConfig1, preg);
        }
    }
}

template <typename T = uint16_t, bool isSetMask = true>
__aicore__ inline void CopyImpl(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, const uint64_t mask[], uint8_t repeatTime,
    const CopyRepeatParams& repeatParams)
{
    if constexpr (isSetMask) {
        SetVectorMask<T>(mask[1], mask[0]);
    }
    __VEC_SCOPE__
    {
        vector_u16 vreg;
        vector_bool preg;
        preg = movp_b16();
        uint32_t strideConfig0 = (static_cast<uint32_t>(repeatParams.srcStride) << 16);
        uint32_t strideConfig1 = (static_cast<uint32_t>(repeatParams.dstStride) << 16);
        for (uint16_t i = 0; i < (static_cast<uint16_t>(repeatTime)); ++i) {
            mem_bar(VST_VLD);
            vsldb(vreg, (__ubuf__ uint16_t*)src + i * repeatParams.srcRepeatSize * 16, strideConfig0, preg);
            vsstb(vreg, (__ubuf__ uint16_t*)dst + i * repeatParams.dstRepeatSize * 16, strideConfig1, preg);
        }
    }
}

template <typename T = half, bool isSetMask = true>
__aicore__ inline void CopyImpl(
    __ubuf__ half* dst, __ubuf__ half* src, const uint64_t mask[], uint8_t repeatTime,
    const CopyRepeatParams& repeatParams)
{
    if constexpr (isSetMask) {
        SetVectorMask<T>(mask[1], mask[0]);
    }
    __VEC_SCOPE__
    {
        vector_f16 vreg;
        vector_bool preg;
        preg = movp_b16();
        uint32_t strideConfig0 = (static_cast<uint32_t>(repeatParams.srcStride) << 16);
        uint32_t strideConfig1 = (static_cast<uint32_t>(repeatParams.dstStride) << 16);
        for (uint16_t i = 0; i < (static_cast<uint16_t>(repeatTime)); ++i) {
            mem_bar(VST_VLD);
            vsldb(vreg, (__ubuf__ half*)src + i * repeatParams.srcRepeatSize * 16, strideConfig0, preg);
            vsstb(vreg, (__ubuf__ half*)dst + i * repeatParams.dstRepeatSize * 16, strideConfig1, preg);
        }
    }
}

template <typename T = int32_t, bool isSetMask = true>
__aicore__ inline void CopyImpl(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, const uint64_t mask[], uint8_t repeatTime,
    const CopyRepeatParams& repeatParams)
{
    if constexpr (isSetMask) {
        SetVectorMask<T>(mask[1], mask[0]);
    }
    __VEC_SCOPE__
    {
        vector_s32 vreg;
        vector_bool preg;
        preg = movp_b32();
        uint32_t strideConfig0 = (static_cast<uint32_t>(repeatParams.srcStride) << 16);
        uint32_t strideConfig1 = (static_cast<uint32_t>(repeatParams.dstStride) << 16);
        for (uint16_t i = 0; i < (static_cast<uint16_t>(repeatTime)); ++i) {
            mem_bar(VST_VLD);
            vsldb(vreg, (__ubuf__ int32_t*)src + i * repeatParams.srcRepeatSize * 8, strideConfig0, preg);
            vsstb(vreg, (__ubuf__ int32_t*)dst + i * repeatParams.dstRepeatSize * 8, strideConfig1, preg);
        }
    }
}

template <typename T = uint32_t, bool isSetMask = true>
__aicore__ inline void CopyImpl(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, const uint64_t mask[], uint8_t repeatTime,
    const CopyRepeatParams& repeatParams)
{
    if constexpr (isSetMask) {
        SetVectorMask<T>(mask[1], mask[0]);
    }
    __VEC_SCOPE__
    {
        vector_u32 vreg;
        vector_bool preg;
        preg = movp_b32();
        uint32_t strideConfig0 = (static_cast<uint32_t>(repeatParams.srcStride) << 16);
        uint32_t strideConfig1 = (static_cast<uint32_t>(repeatParams.dstStride) << 16);
        for (uint16_t i = 0; i < (static_cast<uint16_t>(repeatTime)); ++i) {
            mem_bar(VST_VLD);
            vsldb(vreg, (__ubuf__ uint32_t*)src + i * repeatParams.srcRepeatSize * 8, strideConfig0, preg);
            vsstb(vreg, (__ubuf__ uint32_t*)dst + i * repeatParams.dstRepeatSize * 8, strideConfig1, preg);
        }
    }
}

template <typename T = float, bool isSetMask = true>
__aicore__ inline void CopyImpl(
    __ubuf__ float* dst, __ubuf__ float* src, const uint64_t mask[], uint8_t repeatTime,
    const CopyRepeatParams& repeatParams)
{
    if constexpr (isSetMask) {
        SetVectorMask<T>(mask[1], mask[0]);
    }
    __VEC_SCOPE__
    {
        vector_f32 vreg;
        vector_bool preg;
        preg = movp_b32();
        uint32_t strideConfig0 = (static_cast<uint32_t>(repeatParams.srcStride) << 16);
        uint32_t strideConfig1 = (static_cast<uint32_t>(repeatParams.dstStride) << 16);
        for (uint16_t i = 0; i < (static_cast<uint16_t>(repeatTime)); ++i) {
            mem_bar(VST_VLD);
            vsldb(vreg, (__ubuf__ float*)src + i * repeatParams.srcRepeatSize * 8, strideConfig0, preg);
            vsstb(vreg, (__ubuf__ float*)dst + i * repeatParams.dstRepeatSize * 8, strideConfig1, preg);
        }
    }
}

template <typename T, bool isSetMask = true>
__aicore__ inline void CopyImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask[], uint8_t repeatTime, const CopyRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, "current data type is not supported on current device.");
}

// Copy::Level 0 - mask count mode
template <typename T = int16_t, bool isSetMask = true>
__aicore__ inline void CopyImpl(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, const uint64_t mask, const uint8_t repeatTime,
    const CopyRepeatParams& repeatParams)
{
    if constexpr (isSetMask) {
        uint32_t sreg = mask;
        __VEC_SCOPE__
        {
            vector_s16 vreg;
            uint32_t srcStride = (static_cast<uint32_t>(repeatParams.srcStride) << 16);
            uint32_t dstStride = (static_cast<uint32_t>(repeatParams.dstStride) << 16);
            vector_bool preg = plt_b16(sreg, POST_UPDATE);
            for (uint16_t i = 0; i < (static_cast<uint16_t>(repeatTime)); ++i) {
                mem_bar(VST_VLD);
                vsldb(vreg, src + i * repeatParams.srcRepeatSize * 16, srcStride, preg);
                vsstb(vreg, dst + i * repeatParams.dstRepeatSize * 16, dstStride, preg);
            }
        }
        return;
    }
    uint64_t count = g_maskCount;
    uint32_t sreg = static_cast<uint32_t>(count);
    uint16_t loop = static_cast<uint16_t>(sreg / 128);
    uint32_t tail = static_cast<uint32_t>(sreg % 128);
    uint16_t tailRepeat = (tail == 0) ? 0 : 1;
    __VEC_SCOPE__
    {
        vector_s16 vreg;
        uint32_t srcStride = (static_cast<uint32_t>(repeatParams.srcStride) << 16);
        uint32_t dstStride = (static_cast<uint32_t>(repeatParams.dstStride) << 16);
        vector_bool preg = pset_b16(PAT_ALL);
        for (uint16_t i = 0; i < loop; ++i) {
            mem_bar(VST_VLD);
            vsldb(vreg, src + i * repeatParams.srcRepeatSize * 16, srcStride, preg);
            vsstb(vreg, dst + i * repeatParams.dstRepeatSize * 16, dstStride, preg);
        }
        for (uint16_t i = 0; i < tailRepeat; ++i) {
            vector_bool tailPreg = plt_b16(tail, POST_UPDATE);
            vsldb(vreg, src + loop * repeatParams.srcRepeatSize * 16, srcStride, tailPreg);
            vsstb(vreg, dst + loop * repeatParams.dstRepeatSize * 16, dstStride, tailPreg);
        }
    }
}

template <typename T = uint16_t, bool isSetMask = true>
__aicore__ inline void CopyImpl(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, const uint64_t mask, const uint8_t repeatTime,
    const CopyRepeatParams& repeatParams)
{
    if constexpr (isSetMask) {
        uint32_t sreg = mask;
        __VEC_SCOPE__
        {
            vector_u16 vreg;
            uint32_t srcStride = (static_cast<uint32_t>(repeatParams.srcStride) << 16);
            uint32_t dstStride = (static_cast<uint32_t>(repeatParams.dstStride) << 16);
            vector_bool preg = plt_b16(sreg, POST_UPDATE);
            for (uint16_t i = 0; i < (static_cast<uint16_t>(repeatTime)); ++i) {
                mem_bar(VST_VLD);
                vsldb(vreg, src + i * repeatParams.srcRepeatSize * 16, srcStride, preg);
                vsstb(vreg, dst + i * repeatParams.dstRepeatSize * 16, dstStride, preg);
            }
        }
        return;
    }
    uint64_t count = g_maskCount;
    uint32_t sreg = static_cast<uint32_t>(count);
    uint16_t loop = static_cast<uint16_t>(sreg / 128);
    uint32_t tail = static_cast<uint32_t>(sreg % 128);
    uint16_t tailRepeat = (tail == 0) ? 0 : 1;
    __VEC_SCOPE__
    {
        vector_u16 vreg;
        uint32_t srcStride = (static_cast<uint32_t>(repeatParams.srcStride) << 16);
        uint32_t dstStride = (static_cast<uint32_t>(repeatParams.dstStride) << 16);
        vector_bool preg = pset_b16(PAT_ALL);
        for (uint16_t i = 0; i < loop; ++i) {
            mem_bar(VST_VLD);
            vsldb(vreg, src + i * repeatParams.srcRepeatSize * 16, srcStride, preg);
            vsstb(vreg, dst + i * repeatParams.dstRepeatSize * 16, dstStride, preg);
        }
        for (uint16_t i = 0; i < tailRepeat; ++i) {
            vector_bool tailPreg = plt_b16(tail, POST_UPDATE);
            vsldb(vreg, src + loop * repeatParams.srcRepeatSize * 16, srcStride, tailPreg);
            vsstb(vreg, dst + loop * repeatParams.dstRepeatSize * 16, dstStride, tailPreg);
        }
    }
}

template <typename T = half, bool isSetMask = true>
__aicore__ inline void CopyImpl(
    __ubuf__ half* dst, __ubuf__ half* src, const uint64_t mask, const uint8_t repeatTime,
    const CopyRepeatParams& repeatParams)
{
    if constexpr (isSetMask) {
        uint32_t sreg = mask;
        __VEC_SCOPE__
        {
            vector_f16 vreg;
            uint32_t srcStride = (static_cast<uint32_t>(repeatParams.srcStride) << 16);
            uint32_t dstStride = (static_cast<uint32_t>(repeatParams.dstStride) << 16);
            vector_bool preg = plt_b16(sreg, POST_UPDATE);
            for (uint16_t i = 0; i < (static_cast<uint16_t>(repeatTime)); ++i) {
                mem_bar(VST_VLD);
                vsldb(vreg, src + i * repeatParams.srcRepeatSize * 16, srcStride, preg);
                vsstb(vreg, dst + i * repeatParams.dstRepeatSize * 16, dstStride, preg);
            }
        }
        return;
    }
    uint64_t count = g_maskCount;
    uint32_t sreg = static_cast<uint32_t>(count);
    uint16_t loop = static_cast<uint16_t>(sreg / 128);
    uint32_t tail = static_cast<uint32_t>(sreg % 128);
    uint16_t tailRepeat = (tail == 0) ? 0 : 1;
    __VEC_SCOPE__
    {
        vector_f16 vreg;
        uint32_t srcStride = (static_cast<uint32_t>(repeatParams.srcStride) << 16);
        uint32_t dstStride = (static_cast<uint32_t>(repeatParams.dstStride) << 16);
        vector_bool preg = pset_b16(PAT_ALL);
        for (uint16_t i = 0; i < loop; ++i) {
            mem_bar(VST_VLD);
            vsldb(vreg, src + i * repeatParams.srcRepeatSize * 16, srcStride, preg);
            vsstb(vreg, dst + i * repeatParams.dstRepeatSize * 16, dstStride, preg);
        }
        for (uint16_t i = 0; i < tailRepeat; ++i) {
            vector_bool tailPreg = plt_b16(tail, POST_UPDATE);
            vsldb(vreg, src + loop * repeatParams.srcRepeatSize * 16, srcStride, tailPreg);
            vsstb(vreg, dst + loop * repeatParams.dstRepeatSize * 16, dstStride, tailPreg);
        }
    }
}

template <typename T = int32_t, bool isSetMask = true>
__aicore__ inline void CopyImpl(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, const uint64_t mask, const uint8_t repeatTime,
    const CopyRepeatParams& repeatParams)
{
    if constexpr (isSetMask) {
        uint32_t sreg = mask;
        __VEC_SCOPE__
        {
            vector_s32 vreg;
            uint32_t srcStride = (static_cast<uint32_t>(repeatParams.srcStride) << 16);
            uint32_t dstStride = (static_cast<uint32_t>(repeatParams.dstStride) << 16);
            vector_bool preg = plt_b32(sreg, POST_UPDATE);
            for (uint16_t i = 0; i < (static_cast<uint16_t>(repeatTime)); ++i) {
                mem_bar(VST_VLD);
                vsldb(vreg, src + i * repeatParams.srcRepeatSize * 8, srcStride, preg);
                vsstb(vreg, dst + i * repeatParams.dstRepeatSize * 8, dstStride, preg);
            }
        }
        return;
    }
    uint64_t count = g_maskCount;
    uint32_t sreg = static_cast<uint32_t>(count);
    uint16_t loop = static_cast<uint16_t>(sreg / 64);
    uint32_t tail = static_cast<uint32_t>(sreg % 64);
    uint16_t tailRepeat = (tail == 0) ? 0 : 1;
    __VEC_SCOPE__
    {
        vector_s32 vreg;
        uint32_t srcStride = (static_cast<uint32_t>(repeatParams.srcStride) << 16);
        uint32_t dstStride = (static_cast<uint32_t>(repeatParams.dstStride) << 16);
        vector_bool preg = pset_b32(PAT_ALL);
        for (uint16_t i = 0; i < loop; ++i) {
            mem_bar(VST_VLD);
            vsldb(vreg, src + i * repeatParams.srcRepeatSize * 8, srcStride, preg);
            vsstb(vreg, dst + i * repeatParams.dstRepeatSize * 8, dstStride, preg);
        }
        for (uint16_t i = 0; i < tailRepeat; ++i) {
            vector_bool tailPreg = plt_b32(tail, POST_UPDATE);
            vsldb(vreg, src + loop * repeatParams.srcRepeatSize * 8, srcStride, tailPreg);
            vsstb(vreg, dst + loop * repeatParams.dstRepeatSize * 8, dstStride, tailPreg);
        }
    }
}

template <typename T = uint32_t, bool isSetMask = true>
__aicore__ inline void CopyImpl(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, const uint64_t mask, const uint8_t repeatTime,
    const CopyRepeatParams& repeatParams)
{
    if constexpr (isSetMask) {
        uint32_t sreg = mask;
        __VEC_SCOPE__
        {
            vector_u32 vreg;
            uint32_t srcStride = (static_cast<uint32_t>(repeatParams.srcStride) << 16);
            uint32_t dstStride = (static_cast<uint32_t>(repeatParams.dstStride) << 16);
            vector_bool preg = plt_b32(sreg, POST_UPDATE);
            for (uint16_t i = 0; i < (static_cast<uint16_t>(repeatTime)); ++i) {
                mem_bar(VST_VLD);
                vsldb(vreg, src + i * repeatParams.srcRepeatSize * 8, srcStride, preg);
                vsstb(vreg, dst + i * repeatParams.dstRepeatSize * 8, dstStride, preg);
            }
        }
        return;
    }
    uint64_t count = g_maskCount;
    uint32_t sreg = static_cast<uint32_t>(count);
    uint16_t loop = static_cast<uint16_t>(sreg / 64);
    uint32_t tail = static_cast<uint32_t>(sreg % 64);
    uint16_t tailRepeat = (tail == 0) ? 0 : 1;
    __VEC_SCOPE__
    {
        vector_u32 vreg;
        uint32_t srcStride = (static_cast<uint32_t>(repeatParams.srcStride) << 16);
        uint32_t dstStride = (static_cast<uint32_t>(repeatParams.dstStride) << 16);
        vector_bool preg = pset_b32(PAT_ALL);
        for (uint16_t i = 0; i < loop; ++i) {
            mem_bar(VST_VLD);
            vsldb(vreg, src + i * repeatParams.srcRepeatSize * 8, srcStride, preg);
            vsstb(vreg, dst + i * repeatParams.dstRepeatSize * 8, dstStride, preg);
        }
        for (uint16_t i = 0; i < tailRepeat; ++i) {
            vector_bool tailPreg = plt_b32(tail, POST_UPDATE);
            vsldb(vreg, src + loop * repeatParams.srcRepeatSize * 8, srcStride, tailPreg);
            vsstb(vreg, dst + loop * repeatParams.dstRepeatSize * 8, dstStride, tailPreg);
        }
    }
}

template <typename T = float, bool isSetMask = true>
__aicore__ inline void CopyImpl(
    __ubuf__ float* dst, __ubuf__ float* src, const uint64_t mask, const uint8_t repeatTime,
    const CopyRepeatParams& repeatParams)
{
    if constexpr (isSetMask) {
        uint32_t sreg = mask;
        __VEC_SCOPE__
        {
            vector_f32 vreg;
            uint32_t srcStride = (static_cast<uint32_t>(repeatParams.srcStride) << 16);
            uint32_t dstStride = (static_cast<uint32_t>(repeatParams.dstStride) << 16);
            vector_bool preg = plt_b32(sreg, POST_UPDATE);
            for (uint16_t i = 0; i < (static_cast<uint16_t>(repeatTime)); ++i) {
                mem_bar(VST_VLD);
                vsldb(vreg, src + i * repeatParams.srcRepeatSize * 8, srcStride, preg);
                vsstb(vreg, dst + i * repeatParams.dstRepeatSize * 8, dstStride, preg);
            }
        }
        return;
    }
    uint64_t count = g_maskCount;
    uint32_t sreg = static_cast<uint32_t>(count);
    uint16_t loop = static_cast<uint16_t>(sreg / 64);
    uint32_t tail = static_cast<uint32_t>(sreg % 64);
    uint16_t tailRepeat = (tail == 0) ? 0 : 1;
    __VEC_SCOPE__
    {
        vector_f32 vreg;
        uint32_t srcStride = (static_cast<uint32_t>(repeatParams.srcStride) << 16);
        uint32_t dstStride = (static_cast<uint32_t>(repeatParams.dstStride) << 16);
        vector_bool preg = pset_b32(PAT_ALL);
        for (uint16_t i = 0; i < loop; ++i) {
            mem_bar(VST_VLD);
            vsldb(vreg, src + i * repeatParams.srcRepeatSize * 8, srcStride, preg);
            vsstb(vreg, dst + i * repeatParams.dstRepeatSize * 8, dstStride, preg);
        }
        for (uint16_t i = 0; i < tailRepeat; ++i) {
            vector_bool tailPreg = plt_b32(tail, POST_UPDATE);
            vsldb(vreg, src + loop * repeatParams.srcRepeatSize * 8, srcStride, tailPreg);
            vsstb(vreg, dst + loop * repeatParams.dstRepeatSize * 8, dstStride, tailPreg);
        }
    }
}

template <typename T, bool isSetMask = true>
__aicore__ inline void CopyImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask, const uint8_t repeatTime,
    const CopyRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, "current data type is not supported on current device.");
}

/* **************************************************************************************************
 * DataCopy Enhanced                                             *
 * ************************************************************************************************* */

template <typename T, typename U>
__aicore__ inline void DataCopyL12L0CImpl(
    __cc__ T* dst, __cbuf__ U* src, const DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams)
{
    ASSERT(false && "unsupported data copy from cbuf to l0c on current device");
}

/* **************************************************************************************************
 * DataCopy                                             *
 * ************************************************************************************************* */

// ------------  ------------
template <typename T, typename U>
__aicore__ inline void DataCopyL0C2UBImpl(
    __ubuf__ T* dst, __cc__ U* src, const DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported data copy from l0c to ubuf on current device"); });
}

template <typename T, typename U>
__aicore__ inline void DataCopyUB2L0CImpl(
    __cc__ T* dst, __ubuf__ U* src, const DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported data copy from ubuf to l0c on current device"); });
}

template <typename T>
__aicore__ inline void DataCopySliceGm2UBImpl(__ubuf__ T* dst, __gm__ T* src, const DataCopyParams& intriParams)
{
    uint32_t offsetSrc = 0;
    uint32_t offsetDst = 0;
    for (uint32_t i = 0; i < intriParams.blockCount; i++) {
        offsetSrc = offsetSrc + i * (intriParams.blockLen * ONE_BLK_SIZE + intriParams.srcStride);
        offsetDst = offsetDst + i * (intriParams.blockLen * ONE_BLK_SIZE + intriParams.dstStride);
        DataCopyGM2UBImpl(dst + offsetDst / sizeof(T), src + offsetSrc / sizeof(T), {1, intriParams.blockLen, 0, 0});
    }
}

template <typename T>
__aicore__ inline void DataCopySliceUB2GMImpl(__gm__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
    uint32_t offsetSrc = 0;
    uint32_t offsetDst = 0;
    for (uint32_t i = 0; i < intriParams.blockCount; i++) {
        offsetSrc = offsetSrc + i * (intriParams.blockLen * ONE_BLK_SIZE + intriParams.srcStride);
        offsetDst = offsetDst + i * (intriParams.blockLen * ONE_BLK_SIZE + intriParams.dstStride);
        DataCopyUB2GMImpl(dst + offsetDst / sizeof(T), src + offsetSrc / sizeof(T), {1, intriParams.blockLen, 0, 0});
    }
}

template <typename T>
__aicore__ inline void DataCopyPadGm2UBImpl(
    __ubuf__ T* dst, __gm__ T* src, const DataCopyParams& intriParams, const DataCopyPadParams& padParams)
{
    if (padParams.isPad) {
        set_pad_val_outtoub(padParams.paddingValue);
    }
    if constexpr (sizeof(T) > 4) {
        ASCENDC_ASSERT((false), {
            KERNEL_LOG(KERNEL_ERROR, "unsupported dtype for data copy from global to local on current device");
        });
    }
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(src, workSpace, true, true, intriParams);
    }
    copy_gm_to_ubuf_align(
        dst, src, 0, static_cast<uint16_t>(intriParams.blockCount), intriParams.blockLen, padParams.leftPadding,
        padParams.rightPadding, static_cast<uint32_t>(intriParams.srcStride),
        static_cast<uint32_t>(intriParams.dstStride));
}

template <typename T>
__aicore__ inline void DataCopyPadGm2UBImpl(
    __ubuf__ T* dst, __gm__ T* src, const DataCopyExtParams& intriParams, const DataCopyPadExtParams<T>& padParams)
{
    if (padParams.isPad) {
        set_pad_val_outtoub(GetScalarBitcodeValue((T)padParams.paddingValue));
    }
    if constexpr (sizeof(T) > 4) {
        ASCENDC_ASSERT((false), {
            KERNEL_LOG(KERNEL_ERROR, "unsupported dtype for data copy from global to local on current device");
        });
    }
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(src, workSpace, true, true, intriParams);
    }
    copy_gm_to_ubuf_align(
        dst, src, 0, intriParams.blockCount, intriParams.blockLen, padParams.leftPadding, padParams.rightPadding,
        intriParams.srcStride, intriParams.dstStride);
}

template <typename T>
__aicore__ inline void DataCopyPadUB2GMImpl(__gm__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
#if ASCENDC_CPU_DEBUG
    uint64_t absUbAddr = (uint8_t*)src - (uint8_t*)(GetTPipePtr()->GetBaseAddr((int8_t)TPosition::VECIN));
    ASCENDC_ASSERT((absUbAddr % ONE_BLK_SIZE == 0), {
        KERNEL_LOG(KERNEL_ERROR, "absUbAddr is 0x%lx, which must be 32B aligned", absUbAddr);
    });
#endif
    if constexpr (sizeof(T) > 4) {
        ASCENDC_ASSERT((false), {
            KERNEL_LOG(KERNEL_ERROR, "unsupported dtype for data copy from global to local on current device");
        });
    }
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(dst, workSpace, false, true, intriParams);
    }
    copy_ubuf_to_gm_align(
        dst, src, 0, intriParams.blockCount, intriParams.blockLen, static_cast<uint32_t>(intriParams.srcStride),
        static_cast<uint32_t>(intriParams.dstStride));
}

template <typename T>
__aicore__ inline void DataCopyPadUB2GMImpl(__gm__ T* dst, __ubuf__ T* src, const DataCopyExtParams& intriParams)
{
#if ASCENDC_CPU_DEBUG
    uint64_t absUbAddr = (uint8_t*)src - (uint8_t*)(GetTPipePtr()->GetBaseAddr((int8_t)TPosition::VECIN));
    ASCENDC_ASSERT((absUbAddr % ONE_BLK_SIZE == 0), {
        KERNEL_LOG(KERNEL_ERROR, "absUbAddr is 0x%lx, which must be 32B aligned", absUbAddr);
    });
#endif
    if constexpr (sizeof(T) > 4) {
        ASCENDC_ASSERT((false), {
            KERNEL_LOG(KERNEL_ERROR, "unsupported dtype for data copy from global to local on current device");
        });
    }
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(dst, workSpace, false, true, intriParams);
    }
    copy_ubuf_to_gm_align(
        dst, src, 0, static_cast<uint16_t>(intriParams.blockCount), intriParams.blockLen,
        static_cast<uint32_t>(intriParams.srcStride), static_cast<uint32_t>(intriParams.dstStride));
}

template <typename T>
__aicore__ inline void DataCopyPadL12GMImpl(__gm__ T* dst, __cbuf__ T* src, const DataCopyParams& intriParams)
{
    if constexpr (sizeof(T) > 4) {
        ASCENDC_ASSERT((false), {
            KERNEL_LOG(
                KERNEL_ERROR, "unsupported dtype for data copy from L1 Buffer(TSCM) to global on current device");
        });
    }
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(dst, workSpace, false, true, intriParams);
    }
    copy_cbuf_to_gm_align(
        dst, src, 0, static_cast<uint16_t>(intriParams.blockCount), intriParams.blockLen,
        static_cast<uint32_t>(intriParams.srcStride), static_cast<uint32_t>(intriParams.dstStride));
}

template <typename T>
__aicore__ inline void DataCopyPadL12GMImpl(__gm__ T* dst, __cbuf__ T* src, const DataCopyExtParams& intriParams)
{
    if constexpr (sizeof(T) > 4) {
        ASCENDC_ASSERT((false), {
            KERNEL_LOG(
                KERNEL_ERROR, "unsupported dtype for data copy from L1 Buffer(TSCM) to global on current device");
        });
    }
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(dst, workSpace, false, true, intriParams);
    }
    copy_cbuf_to_gm_align(
        dst, src, 0, static_cast<uint16_t>(intriParams.blockCount), intriParams.blockLen,
        static_cast<uint32_t>(intriParams.srcStride), static_cast<uint32_t>(intriParams.dstStride));
}

template <typename T>
__aicore__ inline void DataCopyPadGM2L1Impl(
    __cbuf__ T* dst, __gm__ T* src, const DataCopyParams& intriParams, const DataCopyPadParams& padParams)
{
    if (padParams.isPad) {
        set_pad_val_outtol1(padParams.paddingValue);
    }
    if constexpr (sizeof(T) > 4) {
        ASCENDC_ASSERT((false), {
            KERNEL_LOG(KERNEL_ERROR, "unsupported dtype for data copy from global to local on current device");
        });
    }
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(src, workSpace, true, true, intriParams);
    }
    copy_gm_to_cbuf_align(
        dst, src, 0, static_cast<uint16_t>(intriParams.blockCount), intriParams.blockLen, padParams.leftPadding,
        padParams.rightPadding, static_cast<uint32_t>(intriParams.srcStride),
        static_cast<uint32_t>(intriParams.dstStride));
}

template <typename T>
__aicore__ inline void DataCopyPadGM2L1Impl(
    __cbuf__ T* dst, __gm__ T* src, const DataCopyExtParams& intriParams, const DataCopyPadExtParams<T>& padParams)
{
    if (padParams.isPad) {
        set_pad_val_outtol1(GetScalarBitcodeValue((T)padParams.paddingValue));
    }
    if constexpr (sizeof(T) > 4) {
        ASCENDC_ASSERT((false), {
            KERNEL_LOG(KERNEL_ERROR, "unsupported dtype for data copy from global to local on current device");
        });
    }
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(src, workSpace, true, true, intriParams);
    }
    copy_gm_to_cbuf_align(
        dst, src, 0, intriParams.blockCount, intriParams.blockLen, padParams.leftPadding, padParams.rightPadding,
        intriParams.srcStride, intriParams.dstStride);
}

template <typename T>
__aicore__ inline void DataCopyPadUB2L1Impl(
    __cbuf__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams, const Nd2NzParams& nd2nzParams)
{
    ASSERT(false && "unsupported data copy from ubuf to cbuf with pad on current device");
}

template <typename T>
__aicore__ inline void DataCopyPadUB2L1Impl(
    __cbuf__ T* dst, __ubuf__ T* src, const DataCopyExtParams& intriParams, const Nd2NzParams& nd2nzParams)
{
    ASSERT(false && "unsupported data copy from ubuf to cbuf with pad on current device");
}

template <typename T>
__aicore__ inline void DataCopyGM2UBND2NZImpl(__ubuf__ T* dst, __gm__ T* src, const Nd2NzParams& intriParams)
{
    ASSERT(false && "unsupported data copy from gm to ubuf nd2nz on current device");
}

template <typename T>
__aicore__ inline void DataCopyGM2UBImpl(__ubuf__ T* dst, __gm__ T* src, const DataCopyParams& intriParams)
{
    if constexpr (IsSameType<T, int4b_t>::value) {
        ASCENDC_ASSERT(
            false, { KERNEL_LOG(KERNEL_ERROR, "unsupported data type for copy from out to ubuf on current device"); });
    } else {
        uint32_t burstLen = intriParams.blockLen * ONE_BLK_SIZE;
        DataCopyExtParams extParams{intriParams.blockCount, burstLen, intriParams.srcStride, intriParams.dstStride, 0};
        if constexpr (sizeof(T) == 8) {
            DataCopyPadExtParams<uint32_t> padExtParams;
            DataCopyPadGm2UBImpl((__ubuf__ uint32_t*)dst, (__gm__ uint32_t*)src, extParams, padExtParams);
        } else {
            DataCopyPadExtParams<T> padExtParams;
            DataCopyPadGm2UBImpl(dst, src, extParams, padExtParams);
        }
    }
}

template <typename T>
__aicore__ inline void DataCopyUB2GMImpl(__gm__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
#ifdef ASCENDC_CPU_DEBUG
    DataCopyWithAtomic(dst, src, intriParams);
#endif // ASCENDC_CPU_DEBUG
    uint32_t burstLen = intriParams.blockLen * ONE_BLK_SIZE;
    DataCopyExtParams extParams{intriParams.blockCount, burstLen, intriParams.srcStride, intriParams.dstStride, 0};
    if constexpr (sizeof(T) == 8) {
        DataCopyPadUB2GMImpl((__gm__ uint32_t*)dst, (__ubuf__ uint32_t*)src, extParams);
    } else {
        DataCopyPadUB2GMImpl(dst, src, extParams);
    }
}

template <typename T>
__aicore__ inline void DataCopyL12GMImpl(__gm__ T* dst, __cbuf__ T* src, const DataCopyParams& intriParams)
{
    uint16_t burstLen = intriParams.blockLen * ONE_BLK_SIZE;
    DataCopyParams padParams{intriParams.blockCount, burstLen, intriParams.srcStride, intriParams.dstStride};
    if constexpr (sizeof(T) == 8) {
        DataCopyPadL12GMImpl((__gm__ uint32_t*)dst, (__cbuf__ uint32_t*)src, padParams);
    } else {
        DataCopyPadL12GMImpl(dst, src, padParams);
    }
}

template <typename T, typename U>
__aicore__ inline void DataCopyL0C2L1Impl(__cbuf__ T* dst, __cc__ U* src, const DataCopyCO12DstParams& intriParams)
{
    ASCENDC_ASSERT(false, {
        KERNEL_LOG(KERNEL_ERROR, "can not move data from L0C Buffer(CO1) to L1 Buffer(A1) on current device!");
    });
}

template <typename T, typename U>
__aicore__ inline void DataCopyL0C2GMImpl(__gm__ T* dst, __cc__ U* src, const DataCopyCO12DstParams& intriParams)
{
    ASCENDC_ASSERT(
        false, { KERNEL_LOG(KERNEL_ERROR, "can not move data from L0C Buffer(CO1) to GM on current device!"); });
}

template <typename T>
__aicore__ inline void DataCopyUB2L0CIntf(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
    DataCopyUB2L0CImpl(
        (__cc__ PrimT<T>*)dst.GetPhyAddr(), (__ubuf__ PrimT<T>*)src.GetPhyAddr(), intriParams, enhancedParams);
}

#pragma begin_pipe(V)
template <typename T>
__aicore__ inline void DataCopyUB2UBIntf(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& intriParams)
{
    DataCopyUB2UBImpl((__ubuf__ PrimT<T>*)dst.GetPhyAddr(), (__ubuf__ PrimT<T>*)src.GetPhyAddr(), intriParams);
}
#pragma end_pipe

template <typename T>
__aicore__ inline __in_pipe__(MTE1) __out_pipe__(MTE1) void DataCopyL12UBIntf(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& intriParams)
{
    DataCopyL12UBImpl((__ubuf__ PrimT<T>*)dst.GetPhyAddr(), (__cbuf__ PrimT<T>*)src.GetPhyAddr(), intriParams);
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_DATA_COPY_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_IMPL_H__
#endif
