/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_3510/kernel_operator_vec_reduce_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_REDUCE_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_VEC_REDUCE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_REDUCE_IMPL_H

#include "kernel_operator_vec_template_impl.h"
#include "../kernel_pop_stack_buffer.h"
#include "../../../include/basic_api/reg_compute/kernel_reg_compute_intf.h"

namespace AscendC {

__BLOCK_LOCAL__ static __inline__ float accValFloat;
__BLOCK_LOCAL__ static __inline__ half accValHalf;

template <bool isBitMask, typename T>
__simd_callee__ inline void GenPredicate(Reg::MaskReg& preg, uint32_t maskReg)
{
    if constexpr (isBitMask) {
        preg = Reg::MoveMask<T>();
    } else {
        preg = Reg::UpdateMask<T>(maskReg);
    }
}

template <bool isSetMask, bool isBitMask, bool isCounterMode, typename T>
__simd_callee__ inline void ReduceCommonCall(
    Reg::MaskReg& mask, uint16_t& newRepeatTimes, uint32_t& countSreg, uint32_t maskReg, __ubuf__ uint64_t* maskBuf)
{
    if constexpr (isCounterMode) {
        if constexpr (!isSetMask) {
            // get SPR.MASK in VF
            Reg::MaskReg sprLoadMaskReg = Reg::MoveMask<uint16_t>();
            Reg::StoreAlign<uint64_t, Reg::MaskDist::DIST_PACK>(maskBuf, sprLoadMaskReg);
            // insert membar(vec store operation) before load maskBuf[0](scalar load operation)
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::SCALAR_LOAD>();
            countSreg = static_cast<uint32_t>(maskBuf[0]);
        }
        constexpr uint16_t oneRepSize = GetVecLen() / sizeof(T);
        newRepeatTimes = CeilDivision(countSreg, oneRepSize);
    } else {
        if constexpr (isBitMask) { // mask[]
            mask = Reg::MoveMask<T>();
        } else { // mask
            if constexpr (!isSetMask) {
                mask = Reg::MoveMask<T>();
            } else {
                mask = Reg::UpdateMask<T>(maskReg);
            }
        }
    }
}

template <bool isSetMask, bool isBitMask, bool isCounterMode, auto func, typename T>
__simd_vf__ inline void ReduceAlignCall(
    __ubuf__ T* dst, __ubuf__ T* src, int32_t repeat, uint32_t dstRepOffset, uint32_t srcBlkStride,
    uint32_t srcRepStride, uint32_t maskReg, __ubuf__ uint64_t* maskBuf)
{
    Reg::MaskReg stMask = Reg::CreateMask<T, Reg::MaskPattern::H>();
    Reg::MaskReg mask;
    uint16_t newRepeatTimes = static_cast<uint16_t>(repeat);
    uint32_t countSreg = static_cast<uint32_t>(maskReg);
    ReduceCommonCall<isSetMask, isBitMask, isCounterMode, T>(mask, newRepeatTimes, countSreg, maskReg, maskBuf);
    Reg::RegTensor<T> srcVreg;
    Reg::RegTensor<T> dstVreg;
    for (uint16_t i = 0; i < newRepeatTimes; ++i) {
        if constexpr (isCounterMode) {
            mask = Reg::UpdateMask<T>(countSreg);
        }
        Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
            srcVreg, src, srcBlkStride, srcRepStride, mask);
        func(dstVreg, srcVreg, mask);
        Reg::StoreAlign(dst + i * dstRepOffset, dstVreg, stMask);
    }
}

template <bool isSetMask, bool isBitMask, bool isCounterMode, bool withStride, auto func, typename T, typename U = T>
__simd_vf__ inline void ReduceUnalignCall(
    __ubuf__ U* dst, __ubuf__ T* src, int32_t repeat, uint32_t oneRepOffset, uint32_t dstRepOffsetPost,
    uint32_t srcBlkStride, uint32_t srcRepStride, uint32_t maskReg, __ubuf__ uint64_t* maskBuf)
{
    Reg::MaskReg mask;
    uint16_t newRepeatTimes = static_cast<uint16_t>(repeat);
    uint32_t countSreg = static_cast<uint32_t>(maskReg);
    ReduceCommonCall<isSetMask, isBitMask, isCounterMode, T>(mask, newRepeatTimes, countSreg, maskReg, maskBuf);
    Reg::RegTensor<T> srcVreg;
    Reg::RegTensor<U> dstVreg;
    Reg::UnalignReg ureg;
    for (uint16_t i = 0; i < newRepeatTimes; ++i) {
        if constexpr (isCounterMode) {
            mask = Reg::UpdateMask<T>(countSreg);
        }
        Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
            srcVreg, src, srcBlkStride, srcRepStride, mask);
        func(dstVreg, srcVreg, mask);
        Reg::StoreUnAlign(dst, dstVreg, ureg, oneRepOffset);
        if constexpr (withStride) {
            Reg::StoreUnAlignPost(dst, ureg, dstRepOffsetPost);
        }
    }
    if constexpr (!withStride) {
        Reg::StoreUnAlignPost(dst, ureg, dstRepOffsetPost);
    }
}

template <bool isSetMask, bool isBitMask, bool isCounterMode, bool withStride, auto func, typename T, typename U = T>
__simd_vf__ inline void WholeReduceUnalignCall(
    __ubuf__ U* dst, __ubuf__ T* src, int32_t repeat, uint32_t oneRepOffset, uint32_t dstRepOffsetPost,
    uint32_t srcBlkStride, uint32_t srcRepStride, uint32_t maskReg, __ubuf__ uint64_t* maskBuf, const ReduceOrder order)
{
    Reg::MaskReg mask;
    uint16_t newRepeatTimes = static_cast<uint16_t>(repeat);
    uint32_t countSreg = static_cast<uint32_t>(maskReg);
    ReduceCommonCall<isSetMask, isBitMask, isCounterMode, T>(mask, newRepeatTimes, countSreg, maskReg, maskBuf);
    Reg::RegTensor<T> srcVreg;
    Reg::RegTensor<U> dstVreg;
    Reg::RegTensor<U> tmpVreg1;
    Reg::RegTensor<U> tmpVreg2;
    Reg::RegTensor<U> tmpVreg3;
    Reg::RegTensor<U> tmpVreg4;
    Reg::UnalignReg ureg;
    for (uint16_t i = 0; i < newRepeatTimes; ++i) {
        if constexpr (isCounterMode) {
            mask = Reg::UpdateMask<T>(countSreg);
        }
        Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
            srcVreg, src, srcBlkStride, srcRepStride, mask);
        if (order == ReduceOrder::ORDER_VALUE_INDEX || order == ReduceOrder::ORDER_ONLY_VALUE) {
            func(dstVreg, srcVreg, mask);
        } else {
            func(tmpVreg1, srcVreg, mask);
            Reg::Duplicate(tmpVreg2, static_cast<T>(0), mask);
            Reg::DeInterleave(tmpVreg3, tmpVreg4, tmpVreg1, tmpVreg2);
            Reg::Interleave(dstVreg, tmpVreg1, tmpVreg4, tmpVreg3);
            if (sizeof(T) == 2 && order == ReduceOrder::ORDER_ONLY_INDEX) {
                Reg::Interleave(dstVreg, tmpVreg1, dstVreg, tmpVreg2);
            }
        }
        Reg::StoreUnAlign(dst, dstVreg, ureg, oneRepOffset);
        if constexpr (withStride) {
            Reg::StoreUnAlignPost(dst, ureg, dstRepOffsetPost);
        }
    }
    if constexpr (!withStride) {
        Reg::StoreUnAlignPost(dst, ureg, dstRepOffsetPost);
    }
}

template <bool isSetMask, bool isBitMask, auto func, typename T>
__aicore__ inline void PairReduceTemplate(
    __ubuf__ T* dst, __ubuf__ T* src, int32_t repeat, int32_t dstRepStride, uint32_t oneRepOffset, int32_t srcBlkStride,
    int32_t srcRepStride, uint32_t maskReg)
{
    constexpr uint32_t ONE_BLK_ELEMENT_NUM = GetDataBlockSizeInBytes() / sizeof(T);
    uint32_t dstRepOffset = oneRepOffset * dstRepStride;
    int32_t newRepeat = repeat;
    __ubuf__ T* newSrc = src;
    if (dstRepStride == 0 && repeat > 0) {
        uint32_t srcStrideOffset = srcRepStride * ONE_BLK_ELEMENT_NUM;
        newSrc += (srcStrideOffset * (repeat - 1));
        newRepeat = 1;
    }
    bool isCounterMode = Internal::IsCounterMode();
    __ubuf__ uint64_t* maskBuf = nullptr;
    if (isCounterMode) {
        if constexpr (!isSetMask) {
            maskBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(GetRuntimeUBSize(), 2);
        }
        ReduceAlignCall<isSetMask, isBitMask, true, func, T>(
            dst, newSrc, newRepeat, dstRepOffset, srcBlkStride, srcRepStride, maskReg, maskBuf);
        if constexpr (!isSetMask) {
            AscendCUtils::FreeTemporaryBuffer<uint64_t>(maskBuf);
        }
    } else {
        ReduceAlignCall<isSetMask, isBitMask, false, func, T>(
            dst, newSrc, newRepeat, dstRepOffset, srcBlkStride, srcRepStride, maskReg, maskBuf);
    }
}

template <bool isSetMask, bool isBitMask, auto func, typename T, typename U = T>
__aicore__ inline void ReduceTemplate(
    __ubuf__ U* dst, __ubuf__ T* src, int32_t repeat, int32_t dstRepStride, uint32_t oneRepOffset, int32_t srcBlkStride,
    int32_t srcRepStride, uint32_t maskReg)
{
    constexpr uint32_t ONE_BLK_ELEMENT_NUM = GetDataBlockSizeInBytes() / sizeof(T);
    bool isCounterMode = Internal::IsCounterMode();
    __ubuf__ uint64_t* maskBuf = nullptr;
    if (isCounterMode) {
        if constexpr (!isSetMask) {
            maskBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(GetRuntimeUBSize(), 2);
        }
        if (dstRepStride == 0 && repeat > 0) {
            uint32_t srcStrideOffset = srcRepStride * ONE_BLK_ELEMENT_NUM;
            __ubuf__ T* newSrc = src + srcStrideOffset * (repeat - 1);
            ReduceUnalignCall<isSetMask, isBitMask, true, true, func, T, U>(
                dst, newSrc, 1, oneRepOffset, 0, srcBlkStride, srcRepStride, maskReg, maskBuf);
        } else if (dstRepStride == 1 && repeat > 0) {
            ReduceUnalignCall<isSetMask, isBitMask, true, false, func, T, U>(
                dst, src, repeat, oneRepOffset, 0, srcBlkStride, srcRepStride, maskReg, maskBuf);
        } else {
            uint32_t dstRepOffsetPost = oneRepOffset * (dstRepStride - 1);
            ReduceUnalignCall<isSetMask, isBitMask, true, true, func, T, U>(
                dst, src, repeat, oneRepOffset, dstRepOffsetPost, srcBlkStride, srcRepStride, maskReg, maskBuf);
        }
        if constexpr (!isSetMask) {
            AscendCUtils::FreeTemporaryBuffer<uint64_t>(maskBuf);
        }
    } else {
        if (dstRepStride == 0 && repeat > 0) {
            uint32_t srcStrideOffset = srcRepStride * ONE_BLK_ELEMENT_NUM;
            __ubuf__ T* newSrc = src + srcStrideOffset * (repeat - 1);
            ReduceUnalignCall<isSetMask, isBitMask, false, true, func, T, U>(
                dst, newSrc, 1, oneRepOffset, 0, srcBlkStride, srcRepStride, maskReg, maskBuf);
        } else if (dstRepStride == 1 && repeat > 0) {
            ReduceUnalignCall<isSetMask, isBitMask, false, false, func, T, U>(
                dst, src, repeat, oneRepOffset, 0, srcBlkStride, srcRepStride, maskReg, maskBuf);
        } else {
            uint32_t dstRepOffsetPost = oneRepOffset * (dstRepStride - 1);
            ReduceUnalignCall<isSetMask, isBitMask, false, true, func, T, U>(
                dst, src, repeat, oneRepOffset, dstRepOffsetPost, srcBlkStride, srcRepStride, maskReg, maskBuf);
        }
    }
}

template <bool isSetMask, bool isBitMask, auto func, typename T, typename U = T>
__aicore__ inline void ReduceTemplate(
    __ubuf__ U* dst, __ubuf__ T* src, int32_t repeat, int32_t dstRepStride, uint32_t oneRepOffset, int32_t srcBlkStride,
    int32_t srcRepStride, uint32_t maskReg, const ReduceOrder order)
{
    constexpr uint32_t ONE_BLK_ELEMENT_NUM = GetDataBlockSizeInBytes() / sizeof(T);
    bool isCounterMode = Internal::IsCounterMode();
    __ubuf__ uint64_t* maskBuf = nullptr;
    if (isCounterMode) {
        if constexpr (!isSetMask) {
            maskBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(GetRuntimeUBSize(), 2);
        }
        if (dstRepStride == 0 && repeat > 0) {
            uint32_t srcStrideOffset = srcRepStride * ONE_BLK_ELEMENT_NUM;
            __ubuf__ T* newSrc = src + srcStrideOffset * (repeat - 1);
            WholeReduceUnalignCall<isSetMask, isBitMask, true, true, func, T, U>(
                dst, newSrc, 1, oneRepOffset, 0, srcBlkStride, srcRepStride, maskReg, maskBuf, order);
        } else if (dstRepStride == 1 && repeat > 0) {
            WholeReduceUnalignCall<isSetMask, isBitMask, true, false, func, T, U>(
                dst, src, repeat, oneRepOffset, 0, srcBlkStride, srcRepStride, maskReg, maskBuf, order);
        } else {
            uint32_t dstRepOffsetPost = oneRepOffset * (dstRepStride - 1);
            WholeReduceUnalignCall<isSetMask, isBitMask, true, true, func, T, U>(
                dst, src, repeat, oneRepOffset, dstRepOffsetPost, srcBlkStride, srcRepStride, maskReg, maskBuf, order);
        }
        if constexpr (!isSetMask) {
            AscendCUtils::FreeTemporaryBuffer<uint64_t>(maskBuf);
        }
    } else {
        if (dstRepStride == 0 && repeat > 0) {
            uint32_t srcStrideOffset = srcRepStride * ONE_BLK_ELEMENT_NUM;
            __ubuf__ T* newSrc = src + srcStrideOffset * (repeat - 1);
            WholeReduceUnalignCall<isSetMask, isBitMask, false, true, func, T, U>(
                dst, newSrc, 1, oneRepOffset, 0, srcBlkStride, srcRepStride, maskReg, maskBuf, order);
        } else if (dstRepStride == 1 && repeat > 0) {
            WholeReduceUnalignCall<isSetMask, isBitMask, false, false, func, T, U>(
                dst, src, repeat, oneRepOffset, 0, srcBlkStride, srcRepStride, maskReg, maskBuf, order);
        } else {
            uint32_t dstRepOffsetPost = oneRepOffset * (dstRepStride - 1);
            WholeReduceUnalignCall<isSetMask, isBitMask, false, true, func, T, U>(
                dst, src, repeat, oneRepOffset, dstRepOffsetPost, srcBlkStride, srcRepStride, maskReg, maskBuf, order);
        }
    }
}

/* **************************************** Pair Reduce Impl ****************************************** */
template <typename T, bool isSetMask = true>
__aicore__ inline void PairReduceSumImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const int32_t repeat, const int32_t mask, const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride)
{
    static_assert((SupportType<T, half, float>()), "The data type is not supported by ReducePairElem<SUM>.");
    constexpr uint32_t oneRepOffset = (ONE_REPEAT_BYTE_SIZE / sizeof(T)) / HALF_FACTOR;
    uint32_t maskReg = static_cast<uint32_t>(mask);
    PairReduceTemplate<isSetMask, false, Reg::PairReduceSum<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>>(
        dst, src, repeat, dstRepStride, oneRepOffset, srcBlkStride, srcRepStride, maskReg);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void PairReduceSumImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const int32_t repeat, const uint64_t mask[], const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride)
{
    static_assert((SupportType<T, half, float>()), "The data type is not supported by ReducePairElem<SUM>.");
    constexpr uint32_t oneRepOffset = (ONE_REPEAT_BYTE_SIZE / sizeof(T)) / HALF_FACTOR;
    if constexpr (isSetMask) {
        SetVectorMask<T>(mask[1], mask[0]);
    }
    PairReduceTemplate<isSetMask, true, Reg::PairReduceSum<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>>(
        dst, src, repeat, dstRepStride, oneRepOffset, srcBlkStride, srcRepStride, mask[0]);
}

/* **************************************** Block Reduce Impl ****************************************** */
template <typename T, bool isSetMask = true>
__aicore__ inline void BlockReduceSumImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const int32_t repeat, const uint64_t mask[], const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride)
{
    static_assert((SupportType<T, half, float>()), "The data type is not supported by ReduceDataBlock<SUM>.");
    if constexpr (isSetMask) {
        SetVectorMask<T>(mask[1], mask[0]);
    }
    ReduceTemplate<isSetMask, true, Reg::ReduceSumWithDataBlock<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>>(
        dst, src, repeat, dstRepStride, DEFAULT_BLK_NUM, srcBlkStride, srcRepStride, mask[0]);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void BlockReduceSumImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const int32_t repeat, const int32_t mask, const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride)
{
    static_assert((SupportType<T, half, float>()), "The data type is not supported by ReduceDataBlock<SUM>.");
    uint32_t maskReg = static_cast<uint32_t>(mask);
    ReduceTemplate<isSetMask, false, Reg::ReduceSumWithDataBlock<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>>(
        dst, src, repeat, dstRepStride, DEFAULT_BLK_NUM, srcBlkStride, srcRepStride, maskReg);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void BlockReduceMaxImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const int32_t repeat, const uint64_t mask[], const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride)
{
    static_assert((SupportType<T, half, float>()), "The data type is not supported by ReduceDataBlock<MAX>.");
    if constexpr (isSetMask) {
        SetVectorMask<T>(mask[1], mask[0]);
    }
    ReduceTemplate<isSetMask, true, Reg::ReduceMaxWithDataBlock<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>>(
        dst, src, repeat, dstRepStride, DEFAULT_BLK_NUM, srcBlkStride, srcRepStride, mask[0]);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void BlockReduceMaxImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const int32_t repeat, const int32_t mask, const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride)
{
    static_assert((SupportType<T, half, float>()), "The data type is not supported by ReduceDataBlock<MAX>.");
    uint32_t maskReg = static_cast<uint32_t>(mask);
    ReduceTemplate<isSetMask, false, Reg::ReduceMaxWithDataBlock<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>>(
        dst, src, repeat, dstRepStride, DEFAULT_BLK_NUM, srcBlkStride, srcRepStride, maskReg);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void BlockReduceMinImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const int32_t repeat, const uint64_t mask[], const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride)
{
    static_assert((SupportType<T, half, float>()), "The data type is not supported by ReduceDataBlock<MIN>.");
    if constexpr (isSetMask) {
        SetVectorMask<T>(mask[1], mask[0]);
    }
    ReduceTemplate<isSetMask, true, Reg::ReduceMinWithDataBlock<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>>(
        dst, src, repeat, dstRepStride, DEFAULT_BLK_NUM, srcBlkStride, srcRepStride, mask[0]);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void BlockReduceMinImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const int32_t repeat, const int32_t mask, const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride)
{
    static_assert((SupportType<T, half, float>()), "The data type is not supported by ReduceDataBlock<MIN>.");
    uint32_t maskReg = static_cast<uint32_t>(mask);
    ReduceTemplate<isSetMask, false, Reg::ReduceMinWithDataBlock<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>>(
        dst, src, repeat, dstRepStride, DEFAULT_BLK_NUM, srcBlkStride, srcRepStride, maskReg);
}

template <typename T, bool isSetMask = true, typename U = T>
__aicore__ inline void RepeatReduceSumImpl(
    __ubuf__ U* dstLocal, __ubuf__ T* srcLocal, const int32_t repeat, const int32_t elemsInOneRepeat,
    const int32_t dstBlkStride, const int32_t srcBlkStride, const int32_t dstRepStride, const int32_t srcRepStride)
{
    static_assert(
        (SupportType<T, int16_t, uint16_t, int32_t, uint32_t, half, float>()),
        "The source data type is not supported by ReduceRepeat<SUM>.");
    static_assert(
        (SupportType<U, int32_t, uint32_t, half, float>()),
        "The destination data type is not supported by ReduceRepeat<SUM>.");
    uint32_t maskReg = static_cast<uint32_t>(elemsInOneRepeat);
    ReduceTemplate<
        isSetMask, false, Reg::ReduceSum<U, T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<U>, Reg::RegTensor<T>>, T,
        U>(dstLocal, srcLocal, repeat, dstRepStride, 1, srcBlkStride, srcRepStride, maskReg);
}

/* **************************************** Whole Reduce Interface ****************************************** */
template <typename T, bool isSetMask = true>
__aicore__ inline void WholeReduceMaxImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, const uint64_t mask[], const int32_t repeat, const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride, const ReduceOrder order)
{
    static_assert(
        (SupportType<T, int16_t, uint16_t, int32_t, uint32_t, half, float>()),
        "The data type is not supported by ReduceRepeat<MAX>.");
    if constexpr (isSetMask) {
        SetVectorMask<T>(mask[1], mask[0]);
    }
    uint32_t oneRepOffset =
        (order == ReduceOrder::ORDER_VALUE_INDEX || order == ReduceOrder::ORDER_INDEX_VALUE) ? 2 : 1;
    if (sizeof(T) == 2 && order == ReduceOrder::ORDER_ONLY_INDEX) {
        oneRepOffset = 2;
    }

    ReduceTemplate<isSetMask, true, Reg::ReduceMax<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>>(
        dstLocal, srcLocal, repeat, dstRepStride, oneRepOffset, srcBlkStride, srcRepStride, mask[0], order);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void WholeReduceMaxImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, const int32_t mask, const int32_t repeat, const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride, const ReduceOrder order)
{
    static_assert(
        (SupportType<T, int16_t, uint16_t, int32_t, uint32_t, half, float>()),
        "The data type is not supported by ReduceRepeat<MAX>.");

    uint32_t maskReg = static_cast<uint32_t>(mask);
    uint32_t oneRepOffset =
        (order == ReduceOrder::ORDER_VALUE_INDEX || order == ReduceOrder::ORDER_INDEX_VALUE) ? 2 : 1;
    if (sizeof(T) == 2 && order == ReduceOrder::ORDER_ONLY_INDEX) {
        oneRepOffset = 2;
    }

    ReduceTemplate<isSetMask, false, Reg::ReduceMax<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>>(
        dstLocal, srcLocal, repeat, dstRepStride, oneRepOffset, srcBlkStride, srcRepStride, maskReg, order);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void WholeReduceMinImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, const uint64_t mask[], const int32_t repeat, const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride, const ReduceOrder order)
{
    static_assert(
        (SupportType<T, int16_t, uint16_t, int32_t, uint32_t, half, float>()),
        "The data type is not supported by ReduceRepeat<MIN>.");
    if constexpr (isSetMask) {
        SetVectorMask<T>(mask[1], mask[0]);
    }
    uint32_t oneRepOffset =
        (order == ReduceOrder::ORDER_VALUE_INDEX || order == ReduceOrder::ORDER_INDEX_VALUE) ? 2 : 1;
    if (sizeof(T) == 2 && order == ReduceOrder::ORDER_ONLY_INDEX) {
        oneRepOffset = 2;
    }

    ReduceTemplate<isSetMask, true, Reg::ReduceMin<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>>(
        dstLocal, srcLocal, repeat, dstRepStride, oneRepOffset, srcBlkStride, srcRepStride, mask[0], order);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void WholeReduceMinImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, const int32_t mask, const int32_t repeat, const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride, const ReduceOrder order)
{
    static_assert(
        (SupportType<T, int16_t, uint16_t, int32_t, uint32_t, half, float>()),
        "The data type is not supported by ReduceRepeat<MIN>.");
    uint32_t maskReg = static_cast<uint32_t>(mask);
    uint32_t oneRepOffset =
        (order == ReduceOrder::ORDER_VALUE_INDEX || order == ReduceOrder::ORDER_INDEX_VALUE) ? 2 : 1;
    if (sizeof(T) == 2 && order == ReduceOrder::ORDER_ONLY_INDEX) {
        oneRepOffset = 2;
    }

    ReduceTemplate<isSetMask, false, Reg::ReduceMin<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>>(
        dstLocal, srcLocal, repeat, dstRepStride, oneRepOffset, srcBlkStride, srcRepStride, maskReg, order);
}

template <typename T, bool isSetMask = true, typename U = T>
__aicore__ inline void WholeReduceSumImpl(
    __ubuf__ U* dstLocal, __ubuf__ T* srcLocal, const uint64_t mask[], const int32_t repeat, const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride)
{
    static_assert(
        (SupportType<T, int16_t, uint16_t, int32_t, uint32_t, half, float>()),
        "The source data type is not supported by ReduceRepeat<SUM>.");
    static_assert(
        (SupportType<U, int32_t, uint32_t, half, float>()),
        "The destination data type is not supported by ReduceRepeat<SUM>.");
    if constexpr (isSetMask) {
        SetVectorMask<T>(mask[1], mask[0]);
    }
    ReduceTemplate<
        isSetMask, true, Reg::ReduceSum<U, T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<U>, Reg::RegTensor<T>>, T, U>(
        dstLocal, srcLocal, repeat, dstRepStride, 1, srcBlkStride, srcRepStride, mask[0]);
}

template <typename T, bool isSetMask = true, typename U = T>
__aicore__ inline void WholeReduceSumImpl(
    __ubuf__ U* dstLocal, __ubuf__ T* srcLocal, const int32_t mask, const int32_t repeat, const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride)
{
    static_assert(
        (SupportType<T, int16_t, uint16_t, int32_t, uint32_t, half, float>()),
        "The source data type is not supported by ReduceRepeat<SUM>.");
    static_assert(
        (SupportType<U, int32_t, uint32_t, half, float>()),
        "The destination data type is not supported by ReduceRepeat<SUM>.");
    uint32_t maskReg = static_cast<uint32_t>(mask);
    ReduceTemplate<
        isSetMask, false, Reg::ReduceSum<U, T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<U>, Reg::RegTensor<T>>, T,
        U>(dstLocal, srcLocal, repeat, dstRepStride, 1, srcBlkStride, srcRepStride, maskReg);
}

/* **************************************** Reduce Interface ****************************************** */
template <ReduceType type, typename T>
__simd_callee__ inline void ReduceCount(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, uint32_t count, int32_t repeat, const int32_t srcRepStride)
{
    uint32_t srcRepOffset = srcRepStride * GetDataBlockSizeInBytes() / sizeof(T);
    Reg::MaskReg preg;
    Reg::RegTensor<T> srcVreg;
    Reg::RegTensor<T> dstVreg;
    Reg::UnalignReg ureg;
    for (uint16_t i = 0; i < static_cast<uint16_t>(repeat); ++i) {
        preg = Reg::UpdateMask<T>(count);
        Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(srcVreg, srcLocal, srcRepOffset);
        Reg::Reduce<type>(dstVreg, srcVreg, preg);
        Reg::StoreUnAlign(dstLocal, dstVreg, ureg, 1);
    }
    Reg::StoreUnAlignPost(dstLocal, ureg, 0);
}

template <ReduceType type, typename T, bool isBitMask>
__simd_callee__ inline void ReduceMask(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, uint32_t mask, int32_t repeat, const int32_t srcRepStride)
{
    uint32_t srcRepOffset = srcRepStride * GetDataBlockSizeInBytes() / sizeof(T);
    Reg::MaskReg preg;
    GenPredicate<isBitMask, T>(preg, mask);
    Reg::RegTensor<T> srcVreg;
    Reg::RegTensor<T> dstVreg;
    Reg::UnalignReg ureg;
    for (uint16_t i = 0; i < static_cast<uint16_t>(repeat); ++i) {
        Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(srcVreg, srcLocal, srcRepOffset);
        Reg::Reduce<type>(dstVreg, srcVreg, preg);
        Reg::StoreUnAlign(dstLocal, dstVreg, ureg, 1);
    }
    Reg::StoreUnAlignPost(dstLocal, ureg, 0);
}

template <typename T, int shapeScope>
__simd_vf__ inline void ReduceSumCounterMode(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* workLocal, uint32_t count, const int32_t srcRepStride)
{
    constexpr uint32_t oneRepSize = GetVecLen() / sizeof(T);
    constexpr uint32_t continuousSrcRepStride = GetVecLen() / GetDataBlockSizeInBytes();
    if constexpr (shapeScope == 1) {
        ReduceCount<ReduceType::SUM>(dstLocal, srcLocal, count, 1, srcRepStride);
    } else if constexpr (shapeScope == 2) {
        uint32_t srcRepOffset = srcRepStride * GetDataBlockSizeInBytes() / sizeof(T);
        Reg::MaskReg fullMask = Reg::CreateMask<T>();
        uint32_t sreg = count - oneRepSize;
        Reg::MaskReg mask = Reg::UpdateMask<T>(sreg);
        Reg::MaskReg oneMask = Reg::CreateMask<T, Reg::MaskPattern::VL1>();
        Reg::RegTensor<T> srcVreg, dstVreg;
        Reg::LoadAlign<T>(dstVreg, srcLocal);
        Reg::LoadAlign<T>(srcVreg, srcLocal + srcRepOffset);
        Reg::Add<T, Reg::MaskMergeMode::MERGING>(dstVreg, dstVreg, srcVreg, mask);
        Reg::Reduce<ReduceType::SUM>(dstVreg, dstVreg, fullMask);
        Reg::StoreAlign(dstLocal, dstVreg, oneMask);
    } else if constexpr (shapeScope == 3) {
        uint32_t srcRepOffset = srcRepStride * GetDataBlockSizeInBytes() / sizeof(T);
        Reg::MaskReg fullMask = Reg::CreateMask<T>();
        uint32_t sreg = count - 2 * oneRepSize;
        Reg::MaskReg mask = Reg::UpdateMask<T>(sreg);
        Reg::MaskReg oneMask = Reg::CreateMask<T, Reg::MaskPattern::VL1>();
        Reg::RegTensor<T> srcVreg1, srcVreg2, dstVreg;
        Reg::LoadAlign<T>(dstVreg, srcLocal);
        Reg::LoadAlign<T>(srcVreg1, srcLocal + srcRepOffset);
        Reg::LoadAlign<T>(srcVreg2, srcLocal + srcRepOffset * 2);
        Reg::Add(dstVreg, dstVreg, srcVreg1, fullMask);
        Reg::Add<T, Reg::MaskMergeMode::MERGING>(dstVreg, dstVreg, srcVreg2, mask);
        Reg::Reduce<ReduceType::SUM>(dstVreg, dstVreg, fullMask);
        Reg::StoreAlign(dstLocal, dstVreg, oneMask);
    } else if constexpr (shapeScope == 4) {
        uint32_t srcRepOffset = srcRepStride * GetDataBlockSizeInBytes() / sizeof(T);
        Reg::MaskReg fullMask = Reg::CreateMask<T>();
        uint32_t sreg = count - 3 * oneRepSize;
        Reg::MaskReg mask = Reg::UpdateMask<T>(sreg);
        Reg::MaskReg oneMask = Reg::CreateMask<T, Reg::MaskPattern::VL1>();
        Reg::RegTensor<T> srcVreg1, srcVreg2, srcVreg3, dstVreg;
        Reg::LoadAlign<T>(dstVreg, srcLocal);
        Reg::LoadAlign<T>(srcVreg1, srcLocal + srcRepOffset);
        Reg::LoadAlign<T>(srcVreg2, srcLocal + srcRepOffset * 2);
        Reg::LoadAlign<T>(srcVreg3, srcLocal + srcRepOffset * 3);
        Reg::Add(dstVreg, dstVreg, srcVreg1, fullMask);
        Reg::Add<T, Reg::MaskMergeMode::MERGING>(srcVreg2, srcVreg2, srcVreg3, mask);
        Reg::Add(dstVreg, dstVreg, srcVreg2, fullMask);
        Reg::Reduce<ReduceType::SUM>(dstVreg, dstVreg, fullMask);
        Reg::StoreAlign(dstLocal, dstVreg, oneMask);
    } else if constexpr (shapeScope == 5) {
        uint32_t count2 = CeilDivision(count, oneRepSize);
        ReduceCount<ReduceType::SUM>(workLocal, srcLocal, count, count2, srcRepStride);
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
        ReduceCount<ReduceType::SUM>(dstLocal, workLocal, count2, 1, continuousSrcRepStride);
    } else {
        uint32_t count2 = CeilDivision(count, oneRepSize);
        uint32_t count3 = CeilDivision(count2, oneRepSize);
        ReduceCount<ReduceType::SUM>(workLocal, srcLocal, count, count2, srcRepStride);
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
        ReduceCount<ReduceType::SUM>(workLocal, workLocal, count2, count3, continuousSrcRepStride);
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
        ReduceCount<ReduceType::SUM>(dstLocal, workLocal, count3, 1, continuousSrcRepStride);
    }
}

template <typename T>
__aicore__ __inline__ void DispatchReduceSumCounterMode(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* workLocal, uint32_t count, const int32_t srcRepStride)
{
    constexpr uint32_t oneRepSize = GetVecLen() / sizeof(T);
    if (count <= oneRepSize) {
        ReduceSumCounterMode<T, 1>(dstLocal, srcLocal, workLocal, count, srcRepStride);
    } else if (count <= oneRepSize * 2) {
        ReduceSumCounterMode<T, 2>(dstLocal, srcLocal, workLocal, count, srcRepStride);
    } else if (count <= oneRepSize * 3) {
        ReduceSumCounterMode<T, 3>(dstLocal, srcLocal, workLocal, count, srcRepStride);
    } else if (count <= oneRepSize * 4) {
        ReduceSumCounterMode<T, 4>(dstLocal, srcLocal, workLocal, count, srcRepStride);
    } else if (count <= oneRepSize * oneRepSize) {
        ReduceSumCounterMode<T, 5>(dstLocal, srcLocal, workLocal, count, srcRepStride);
    } else {
        ReduceSumCounterMode<T, 6>(dstLocal, srcLocal, workLocal, count, srcRepStride);
    }
}

template <typename T, int shapeScope, bool isBitMask>
__simd_vf__ inline void ReduceSumNormalMode(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* workLocal, uint32_t mask, int32_t repeat,
    const int32_t srcRepStride)
{
    constexpr uint32_t oneRepSize = GetVecLen() / sizeof(T);
    constexpr uint32_t continuousSrcRepStride = GetVecLen() / GetDataBlockSizeInBytes();
    if constexpr (shapeScope == 1) {
        ReduceMask<ReduceType::SUM, T, isBitMask>(dstLocal, srcLocal, mask, 1, srcRepStride);
    } else if constexpr (shapeScope == 2) {
        ReduceMask<ReduceType::SUM, T, isBitMask>(workLocal, srcLocal, mask, repeat, srcRepStride);
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
        ReduceCount<ReduceType::SUM>(dstLocal, workLocal, repeat, 1, continuousSrcRepStride);
    } else {
        uint32_t count = CeilDivision(repeat, oneRepSize);
        ReduceMask<ReduceType::SUM, T, isBitMask>(workLocal, srcLocal, mask, repeat, srcRepStride);
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
        ReduceCount<ReduceType::SUM>(workLocal, workLocal, repeat, count, continuousSrcRepStride);
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
        ReduceCount<ReduceType::SUM>(dstLocal, workLocal, count, 1, continuousSrcRepStride);
    }
}

template <typename T>
__aicore__ inline void ReduceSumImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* workLocal, const uint64_t mask[], const int32_t repeat,
    const int32_t srcRepStride)
{
    static_assert((SupportType<T, half, float, uint32_t, int32_t>()), "The data type is not supported by ReduceSum.");
    constexpr uint32_t oneRepSize = GetVecLen() / sizeof(T);
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        uint32_t count = static_cast<uint32_t>(mask[0]);
        DispatchReduceSumCounterMode<T>(dstLocal, srcLocal, workLocal, count, srcRepStride);
    } else {
        SetVectorMask<T>(mask[1], mask[0]);
        if (repeat <= 1) {
            ReduceSumNormalMode<T, 1, true>(dstLocal, srcLocal, workLocal, 0, 1, srcRepStride);
        } else if (repeat <= oneRepSize) {
            ReduceSumNormalMode<T, 2, true>(dstLocal, srcLocal, workLocal, 0, repeat, srcRepStride);
        } else {
            ReduceSumNormalMode<T, 3, true>(dstLocal, srcLocal, workLocal, 0, repeat, srcRepStride);
        }
    }
}

template <typename T>
__aicore__ inline void ReduceSumImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* workLocal, const int32_t mask, const int32_t repeat,
    const int32_t srcRepStride)
{
    static_assert((SupportType<T, half, float, uint32_t, int32_t>()), "The data type is not supported by ReduceSum.");
    constexpr uint32_t oneRepSize = GetVecLen() / sizeof(T);
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        uint32_t count = static_cast<uint32_t>(mask);
        DispatchReduceSumCounterMode<T>(dstLocal, srcLocal, workLocal, count, srcRepStride);
    } else {
        if (repeat <= 1) {
            ReduceSumNormalMode<T, 1, false>(dstLocal, srcLocal, workLocal, mask, 1, srcRepStride);
        } else if (repeat <= oneRepSize) {
            ReduceSumNormalMode<T, 2, false>(dstLocal, srcLocal, workLocal, mask, repeat, srcRepStride);
        } else {
            ReduceSumNormalMode<T, 3, false>(dstLocal, srcLocal, workLocal, mask, repeat, srcRepStride);
        }
    }
}

template <typename T>
__simd_vf__ inline void ReduceB64SumImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* workLocal, uint32_t count)
{
    constexpr uint32_t oneRepSize = 2 * GetVecLen() / sizeof(T);
    uint16_t repeatTime = CeilDivision(count, oneRepSize);
    uint32_t sreg = count;
    Reg::RegTensor<T, Reg::RegTraitNumTwo> vregDup;
    Reg::RegTensor<T, Reg::RegTraitNumTwo> vregTmp;
    Reg::RegTensor<T, Reg::RegTraitNumTwo> vreg0;
    Reg::RegTensor<T, Reg::RegTraitNumTwo> vreg1;
    Reg::MaskReg fullMask = Reg::CreateMask<T, Reg::MaskPattern::ALL, Reg::RegTraitNumTwo>();
    Reg::MaskReg mask;
    Reg::Duplicate(vregDup, T(0), fullMask);

    for (uint16_t i = 0; i < repeatTime; ++i) {
        mask = Reg::UpdateMask<T, Reg::RegTraitNumTwo>(sreg);
        Reg::LoadAlign(vreg0, srcLocal + i * oneRepSize);
        Reg::Add(vregTmp, vregDup, vreg0, mask);
        Reg::Select(vregDup, vregTmp, vregDup, mask);
    }
    Reg::Reduce<ReduceType::SUM>(vreg1, vregDup, fullMask);
    Reg::MaskReg maskFirstVal = Reg::CreateMask<T, Reg::MaskPattern::VL1, Reg::RegTraitNumTwo>();
    Reg::StoreAlign(dstLocal, vreg1, maskFirstVal);
}

template <typename T>
__aicore__ inline void ReduceSumImpl(__ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* workLocal, uint32_t count)
{
    static_assert(
        (SupportType<T, half, float, uint32_t, int32_t, uint64_t, int64_t>()),
        "The data type is not supported by ReduceSum.");
    if constexpr (SupportType<T, uint64_t, int64_t>()) {
        ReduceB64SumImpl<T>(dstLocal, srcLocal, workLocal, count);
    } else {
        constexpr uint32_t srcRepStride = GetVecLen() / GetDataBlockSizeInBytes();
        DispatchReduceSumCounterMode<T>(dstLocal, srcLocal, workLocal, count, srcRepStride);
    }
    if constexpr (AscendC::Std::is_same<T, float>::value || AscendC::Std::is_same<T, half>::value) {
        TEventID eventID = GetTPipePtr()->AllocEventID<HardEvent::V_S>();
        SetFlag<HardEvent::V_S>(eventID);
        WaitFlag<HardEvent::V_S>(eventID);
        GetTPipePtr()->ReleaseEventID<AscendC::HardEvent::V_S>(eventID);
        // extract the value from dstLocal and store it as a float in block_local
        if constexpr (AscendC::Std::is_same<T, float>::value) {
            accValFloat = dstLocal[0];
        } else if constexpr (AscendC::Std::is_same<T, half>::value) {
            accValHalf = dstLocal[0];
        }
    }
}

/***************************** Reduce Max & Min ******************/
template <typename T>
__aicore__ inline T GetMinValue()
{
    if constexpr (AscendC::Std::is_same<T, half>::value) {
        return GetScalarBitcodeValue<uint16_t, T>(0xFBFF);
    } else if constexpr (AscendC::Std::is_same<T, float>::value) {
        return GetScalarBitcodeValue<uint32_t, T>(0xFF7FFFFF);
    } else if constexpr (AscendC::Std::is_same<T, uint16_t>::value) {
        return 0;
    } else if constexpr (AscendC::Std::is_same<T, int16_t>::value) {
        return 0x8000;
    } else if constexpr (AscendC::Std::is_same<T, uint32_t>::value) {
        return 0;
    } else if constexpr (AscendC::Std::is_same<T, int32_t>::value) {
        return 0x80000000;
    } else if constexpr (AscendC::Std::is_same<T, uint64_t>::value) {
        return 0;
    } else if constexpr (AscendC::Std::is_same<T, int64_t>::value) {
        return 0x8000000000000000;
    }
}

template <typename T>
__aicore__ inline T GetMaxValue()
{
    if constexpr (AscendC::Std::is_same<T, half>::value) {
        return GetScalarBitcodeValue<uint16_t, T>(0x7BFF);
    } else if constexpr (AscendC::Std::is_same<T, float>::value) {
        return GetScalarBitcodeValue<uint32_t, T>(0x7F7FFFFF);
    } else if constexpr (AscendC::Std::is_same<T, uint16_t>::value) {
        return 0xFFFF;
    } else if constexpr (AscendC::Std::is_same<T, int16_t>::value) {
        return 0x7FFF;
    } else if constexpr (AscendC::Std::is_same<T, uint32_t>::value) {
        return 0xFFFFFFFF;
    } else if constexpr (AscendC::Std::is_same<T, int32_t>::value) {
        return 0x7FFFFFFF;
    } else if constexpr (AscendC::Std::is_same<T, uint64_t>::value) {
        return 0xFFFFFFFFFFFFFFFF;
    } else if constexpr (AscendC::Std::is_same<T, int64_t>::value) {
        return 0x7FFFFFFFFFFFFFFF;
    }
}

template <ReduceMode mode, typename T>
__simd_vf__ inline void ReduceNoIndexTemplate(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* workLocal, uint32_t count, const int32_t srcRepStride,
    T initValue)
{
    constexpr uint16_t oneRepSize = GetVecLen() / sizeof(T);
    uint16_t repeat = CeilDivision(count, oneRepSize);
    uint32_t srcRepOffset = srcRepStride * GetDataBlockSizeInBytes() / sizeof(T);
    Reg::MaskReg preg;
    Reg::MaskReg pregFull = Reg::CreateMask<T, Reg::MaskPattern::ALL>();
    Reg::RegTensor<T> srcVreg, dstVreg, tmpVreg;
    Reg::UnalignReg ureg;
    Reg::Duplicate(dstVreg, initValue);
    for (uint16_t i = 0; i < repeat; ++i) {
        preg = Reg::UpdateMask<T>(count);
        Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(srcVreg, srcLocal, srcRepOffset);
        if constexpr (mode == ReduceMode::REDUCE_MAX) {
            Reg::Max(tmpVreg, dstVreg, srcVreg, preg);
        } else {
            Reg::Min(tmpVreg, dstVreg, srcVreg, preg);
        }
        // merge new masked tmpVreg to dstVreg, keep non-masked old value in dstVreg
        Reg::Select(dstVreg, tmpVreg, dstVreg, preg);
    }
    if constexpr (mode == ReduceMode::REDUCE_MAX) {
        Reg::ReduceMax(dstVreg, dstVreg, pregFull);
    } else {
        Reg::ReduceMin(dstVreg, dstVreg, pregFull);
    }
    Reg::StoreUnAlign(dstLocal, dstVreg, ureg, 1);
    Reg::StoreUnAlignPost(dstLocal, ureg, 0);
}

template <ReduceMode mode, typename T>
__simd_vf__ inline void ReduceB64NoIndexTemplate(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* workLocal, uint32_t count, T initValue)
{
    constexpr uint16_t oneRepSize = 2 * GetVecLen() / sizeof(T);
    uint16_t repeat = CeilDivision(count, oneRepSize);
    Reg::MaskReg preg;
    Reg::MaskReg pregFull = Reg::CreateMask<T, Reg::MaskPattern::ALL, Reg::RegTraitNumTwo>();
    Reg::RegTensor<T, Reg::RegTraitNumTwo> b64SrcVreg, b64DstVreg, b64TmpVreg;
    Reg::UnalignReg ureg;
    Reg::Duplicate(b64DstVreg, initValue, pregFull);
    for (uint16_t i = 0; i < repeat; ++i) {
        preg = Reg::UpdateMask<T, Reg::RegTraitNumTwo>(count);
        Reg::LoadAlign(b64SrcVreg, srcLocal + i * oneRepSize);
        if constexpr (mode == ReduceMode::REDUCE_MAX) {
            Reg::Max(b64TmpVreg, b64DstVreg, b64SrcVreg, preg);
        } else {
            Reg::Min(b64TmpVreg, b64DstVreg, b64SrcVreg, preg);
        }
        // merge new masked b64TmpVreg to b64DstVreg, keep non-masked old value in b64DstVreg
        Reg::Select(b64DstVreg, b64TmpVreg, b64DstVreg, preg);
    }
    if constexpr (mode == ReduceMode::REDUCE_MAX) {
        Reg::ReduceMax(b64DstVreg, b64DstVreg, pregFull);
    } else {
        Reg::ReduceMin(b64DstVreg, b64DstVreg, pregFull);
    }
    Reg::StoreUnAlign(dstLocal, b64DstVreg, ureg, 1);
    Reg::StoreUnAlignPost(dstLocal, ureg, 0);
}

template <ReduceMode mode, bool isBitMask, typename T>
__simd_vf__ inline void ReduceNoIndexTemplate(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* workLocal, uint32_t maskReg, const int32_t repeat,
    const int32_t srcRepStride, T initValue)
{
    Reg::MaskReg preg;
    GenPredicate<isBitMask, T>(preg, maskReg);
    Reg::RegTensor<T> srcVreg, dstVreg;
    Reg::UnalignReg ureg;
    Reg::Duplicate(dstVreg, initValue);
    int32_t postUpdateStride = srcRepStride * GetDataBlockSizeInBytes() / sizeof(T);
    for (uint16_t i = 0; i < repeat; ++i) {
        Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(srcVreg, srcLocal, postUpdateStride);
        if constexpr (mode == ReduceMode::REDUCE_MAX) {
            Reg::Max(dstVreg, dstVreg, srcVreg, preg);
        } else {
            Reg::Min(dstVreg, dstVreg, srcVreg, preg);
        }
    }
    if constexpr (mode == ReduceMode::REDUCE_MAX) {
        Reg::ReduceMax(dstVreg, dstVreg, preg);
    } else {
        Reg::ReduceMin(dstVreg, dstVreg, preg);
    }
    Reg::StoreUnAlign(dstLocal, dstVreg, ureg, 1);
    Reg::StoreUnAlignPost(dstLocal, ureg, 0);
}

template <ReduceMode mode, typename T, typename IndexT>
__simd_vf__ inline void ReduceIndexTemplate(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* workLocal, uint32_t count, const int32_t srcRepStride,
    T initValue)
{
    constexpr uint16_t oneRepSize = GetVecLen() / sizeof(T);
    uint16_t repeat = CeilDivision(count, oneRepSize);
    uint32_t srcRepOffset = srcRepStride * GetDataBlockSizeInBytes() / sizeof(T);
    Reg::MaskReg preg, pregCond;
    Reg::MaskReg pregIndexFull = Reg::CreateMask<IndexT, Reg::MaskPattern::ALL>();
    Reg::MaskReg pregFull = Reg::CreateMask<T, Reg::MaskPattern::ALL>();
    Reg::UnalignReg ureg;
    Reg::RegTensor<T> srcVreg, dstValueVreg, tmpValueVreg;
    Reg::RegTensor<IndexT> dstIndexVreg, subIndexVreg, tmpIndexVreg, maskIndexVreg;
    Reg::Duplicate(subIndexVreg, (IndexT)1);
    Reg::Duplicate(maskIndexVreg, (IndexT)0);
    Reg::Duplicate(dstValueVreg, initValue);
    if constexpr (AscendC::Std::is_same<IndexT, uint16_t>::value) {
        Reg::Arange((Reg::RegTensor<int16_t>&)tmpIndexVreg, 1);
    } else {
        Reg::Arange((Reg::RegTensor<int32_t>&)tmpIndexVreg, 1);
    }
    dstIndexVreg = tmpIndexVreg;
    // step1: from [count] to [oneRepSize] value index pair
    for (uint16_t i = 0; i < repeat; ++i) {
        preg = Reg::UpdateMask<T>(count);
        Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(srcVreg, srcLocal, srcRepOffset);
        if constexpr (mode == ReduceMode::REDUCE_MAX) {
            Reg::Max(tmpValueVreg, dstValueVreg, srcVreg, preg);
        } else {
            Reg::Min(tmpValueVreg, dstValueVreg, srcVreg, preg);
        }
        // merge old non-masked masked dstValueVreg to tmpValue, keep masked new value in tmpValue
        // now tmpValueVreg is this round new value, dstValueVreg is previous round value
        Reg::Select(tmpValueVreg, tmpValueVreg, dstValueVreg, preg);
        // if previous round and this round value is change, update index
        Reg::Compare<T, CMPMODE::NE>(pregCond, dstValueVreg, tmpValueVreg, pregFull);
        Reg::Select(dstIndexVreg, tmpIndexVreg, dstIndexVreg, pregCond);
        // make next round index
        Reg::Adds(tmpIndexVreg, tmpIndexVreg, (IndexT)oneRepSize, pregFull);
        // update value
        dstValueVreg = tmpValueVreg;
    }
    // step2: from [oneRepSize] to [1] value index and store it to ub
    if constexpr (mode == ReduceMode::REDUCE_MAX) {
        Reg::ReduceMax(tmpValueVreg, dstValueVreg, pregFull);
    } else {
        Reg::ReduceMin(tmpValueVreg, dstValueVreg, pregFull);
    }
    Reg::StoreUnAlign(dstLocal, tmpValueVreg, ureg, 1); // store value
    // get dst value mask and squeeze dst index
    Reg::Duplicate(tmpValueVreg, tmpValueVreg, pregFull);
    Reg::Compare<T, CMPMODE::EQ>(pregCond, dstValueVreg, tmpValueVreg, pregFull);
    Reg::GatherMask<IndexT, Reg::GatherMaskMode::NO_STORE_REG>(tmpIndexVreg, dstIndexVreg, pregCond);
    // cal preg for how much index has the same max or min value
    Reg::Compare<IndexT, CMPMODE::NE>(pregCond, tmpIndexVreg, maskIndexVreg, pregIndexFull);
    Reg::ReduceMin(tmpIndexVreg, tmpIndexVreg, pregCond);
    Reg::Sub(tmpIndexVreg, tmpIndexVreg, subIndexVreg, pregIndexFull);
    Reg::StoreUnAlign((__ubuf__ IndexT*&)dstLocal, tmpIndexVreg, ureg, 1);
    Reg::StoreUnAlignPost(dstLocal, ureg, 0);
}

template <ReduceMode mode, typename T, typename IndexT>
__simd_vf__ inline void ReduceB64IndexTemplate(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* workLocal, uint32_t count, T initValue)
{
    constexpr uint16_t oneRepSize = 2 * GetVecLen() / sizeof(T);
    uint16_t repeat = CeilDivision(count, oneRepSize);
    Reg::MaskReg preg, pregCond;
    Reg::MaskReg pregFull = Reg::CreateMask<T, Reg::MaskPattern::ALL, Reg::RegTraitNumTwo>();
    Reg::MaskReg pregIndexFull = Reg::CreateMask<IndexT, Reg::MaskPattern::ALL>();
    Reg::RegTensor<T, Reg::RegTraitNumTwo> b64SrcVreg, b64DstValueVreg, b64TmpValueVreg;
    Reg::RegTensor<IndexT> dstIndexVreg, tmpIndexVreg, maskIndexVreg, subIndexVreg;
    Reg::UnalignReg ureg;
    Reg::Duplicate(b64DstValueVreg, initValue, pregFull);
    Reg::Duplicate(maskIndexVreg, (IndexT)0);
    Reg::Duplicate(subIndexVreg, (IndexT)1);
    // b64 type, index is uint32_t
    Reg::Arange((Reg::RegTensor<int32_t>&)tmpIndexVreg, 1);
    dstIndexVreg = tmpIndexVreg;
    // step1: from [count] to [oneRepSize] value index pair
    for (uint16_t i = 0; i < repeat; ++i) {
        preg = Reg::UpdateMask<T, Reg::RegTraitNumTwo>(count);
        Reg::LoadAlign(b64SrcVreg, srcLocal + i * oneRepSize);
        if constexpr (mode == ReduceMode::REDUCE_MAX) {
            Reg::Max(b64TmpValueVreg, b64DstValueVreg, b64SrcVreg, preg);
        } else {
            Reg::Min(b64TmpValueVreg, b64DstValueVreg, b64SrcVreg, preg);
        }
        // merge old non-masked masked b64DstValueVreg to tmpValue, keep masked new value in tmpValue
        // now b64TmpValueVreg is this round new value, b64DstValueVreg is previous round value
        Reg::Select(b64TmpValueVreg, b64TmpValueVreg, b64DstValueVreg, preg);
        // if previous round and this round value is change, update index
        Reg::Compare<T, CMPMODE::NE>(pregCond, b64DstValueVreg, b64TmpValueVreg, pregFull);
        Reg::Select(dstIndexVreg, tmpIndexVreg, dstIndexVreg, pregCond);
        // make next round index
        Reg::Adds(tmpIndexVreg, tmpIndexVreg, (IndexT)oneRepSize, pregIndexFull);
        // update value
        b64DstValueVreg = b64TmpValueVreg;
    }
    // step2: from [oneRepSize] to [1] value index and store it to ub
    if constexpr (mode == ReduceMode::REDUCE_MAX) {
        Reg::ReduceMax(b64TmpValueVreg, b64DstValueVreg, pregFull);
    } else {
        Reg::ReduceMin(b64TmpValueVreg, b64DstValueVreg, pregFull);
    }
    Reg::StoreUnAlign(dstLocal, b64TmpValueVreg, ureg, 1); // store value
    // get dst value mask and squeeze dst index
    Reg::Duplicate(b64TmpValueVreg, b64TmpValueVreg, pregFull);
    Reg::Compare<T, CMPMODE::EQ>(pregCond, b64DstValueVreg, b64TmpValueVreg, pregFull);
    // gather mask index
    Reg::GatherMask<IndexT, Reg::GatherMaskMode::NO_STORE_REG>(tmpIndexVreg, dstIndexVreg, pregCond);
    // cal preg for how much index has the same max or min value
    Reg::Compare<IndexT, CMPMODE::NE>(pregCond, tmpIndexVreg, maskIndexVreg, pregIndexFull);
    Reg::ReduceMin(tmpIndexVreg, tmpIndexVreg, pregCond);
    Reg::Sub(tmpIndexVreg, tmpIndexVreg, subIndexVreg, pregIndexFull);
    Reg::StoreUnAlign((__ubuf__ IndexT*&)dstLocal, tmpIndexVreg, ureg, 1);
    // for b64 type, pad 0 to b64 bytes, which is 4 bytes
    Reg::StoreUnAlign((__ubuf__ IndexT*&)dstLocal, maskIndexVreg, ureg, 1);
    Reg::StoreUnAlignPost(dstLocal, ureg, 0);
}

template <ReduceMode mode, bool isBitMask, typename T, typename IndexT>
__simd_vf__ inline void ReduceIndexTemplate(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* workLocal, uint32_t maskReg, const int32_t repeat,
    const int32_t srcRepStride, T initValue)
{
    Reg::MaskReg preg, pregCond;
    GenPredicate<isBitMask, T>(preg, maskReg);
    Reg::MaskReg pregIndexFull = Reg::CreateMask<IndexT, Reg::MaskPattern::ALL>();
    Reg::RegTensor<T> srcVreg, dstValueVreg, tmpValueVreg;
    Reg::RegTensor<IndexT> dstIndexVreg, tmpIndexVreg, maskIndexVreg, subIndexVreg;
    Reg::UnalignReg ureg;
    Reg::Duplicate(dstValueVreg, initValue);
    Reg::Duplicate(maskIndexVreg, (IndexT)0);
    Reg::Duplicate(subIndexVreg, (IndexT)1);
    if constexpr (AscendC::Std::is_same<IndexT, uint16_t>::value) {
        Reg::Arange((Reg::RegTensor<int16_t>&)tmpIndexVreg, 1);
    } else {
        Reg::Arange((Reg::RegTensor<int32_t>&)tmpIndexVreg, 1);
    }
    dstIndexVreg = tmpIndexVreg;
    int32_t postUpdateStride = srcRepStride * GetDataBlockSizeInBytes() / sizeof(T);
    // step1: from [count] to [oneRepSize] value index pair
    for (uint16_t i = 0; i < repeat; ++i) {
        Reg::LoadAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(srcVreg, srcLocal, postUpdateStride);
        if constexpr (mode == ReduceMode::REDUCE_MAX) {
            Reg::Max(tmpValueVreg, dstValueVreg, srcVreg, preg);
        } else {
            Reg::Min(tmpValueVreg, dstValueVreg, srcVreg, preg);
        }
        // now tmpValueVreg is this round new value, dstValueVreg is previous round value
        // if previous round and this round value is change, update index
        Reg::Compare<T, CMPMODE::NE>(pregCond, dstValueVreg, tmpValueVreg, preg);
        Reg::Select(dstIndexVreg, tmpIndexVreg, dstIndexVreg, pregCond);
        // make next round index
        Reg::Adds(tmpIndexVreg, tmpIndexVreg, (IndexT)postUpdateStride, preg);
        // update value
        dstValueVreg = tmpValueVreg;
    }
    // step2: from [oneRepSize] to [1] value index and store it to ub
    if constexpr (mode == ReduceMode::REDUCE_MAX) {
        Reg::ReduceMax(tmpValueVreg, dstValueVreg, preg);
    } else {
        Reg::ReduceMin(tmpValueVreg, dstValueVreg, preg);
    }
    Reg::StoreUnAlign(dstLocal, tmpValueVreg, ureg, 1); // store value
    // get dst value mask and squeeze dst index
    Reg::Duplicate(tmpValueVreg, tmpValueVreg, preg);
    Reg::Compare<T, CMPMODE::EQ>(pregCond, dstValueVreg, tmpValueVreg, preg);
    // gather mask index
    Reg::GatherMask<IndexT, Reg::GatherMaskMode::NO_STORE_REG>(tmpIndexVreg, dstIndexVreg, pregCond);
    // cal preg for how much index has the same max or min value
    Reg::Compare<IndexT, CMPMODE::NE>(pregCond, tmpIndexVreg, maskIndexVreg, pregIndexFull);
    Reg::ReduceMin(tmpIndexVreg, tmpIndexVreg, pregCond);
    Reg::Sub(tmpIndexVreg, tmpIndexVreg, subIndexVreg, pregIndexFull);
    Reg::StoreUnAlign((__ubuf__ IndexT*&)dstLocal, tmpIndexVreg, ureg, 1);
    Reg::StoreUnAlignPost(dstLocal, ureg, 0);
}

template <typename T>
__aicore__ inline void ReduceMaxImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* workLocal, uint32_t count, bool calIndex)
{
    static_assert(
        (SupportType<T, int16_t, uint16_t, int32_t, uint32_t, half, float, uint64_t, int64_t>()),
        "The data type is not supported by ReduceMax.");
    T initValue = GetMinValue<T>();
    if constexpr (sizeof(T) == 8) {
        if (calIndex) {
            ReduceB64IndexTemplate<ReduceMode::REDUCE_MAX, T, uint32_t>(
                dstLocal, srcLocal, workLocal, count, initValue);
        } else {
            ReduceB64NoIndexTemplate<ReduceMode::REDUCE_MAX, T>(dstLocal, srcLocal, workLocal, count, initValue);
        }
    } else if constexpr (sizeof(T) == 4) {
        if (calIndex) {
            ReduceIndexTemplate<ReduceMode::REDUCE_MAX, T, uint32_t>(
                dstLocal, srcLocal, workLocal, count, DEFAULT_BLK_NUM, initValue);
        } else {
            ReduceNoIndexTemplate<ReduceMode::REDUCE_MAX, T>(
                dstLocal, srcLocal, workLocal, count, DEFAULT_BLK_NUM, initValue);
        }
    } else {
        if (calIndex) {
            ReduceIndexTemplate<ReduceMode::REDUCE_MAX, T, uint16_t>(
                dstLocal, srcLocal, workLocal, count, DEFAULT_BLK_NUM, initValue);
        } else {
            ReduceNoIndexTemplate<ReduceMode::REDUCE_MAX, T>(
                dstLocal, srcLocal, workLocal, count, DEFAULT_BLK_NUM, initValue);
        }
    }
}

template <typename T>
__aicore__ inline void ReduceMaxImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* workLocal, const uint64_t mask[], const int32_t repeat,
    const int32_t srcRepStride, bool calIndex)
{
    static_assert(
        (SupportType<T, int16_t, uint16_t, int32_t, uint32_t, half, float>()),
        "The data type is not supported by ReduceMax.");
    T initValue = GetMinValue<T>();
    uint32_t count = static_cast<uint32_t>(mask[0]);
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        if constexpr (sizeof(T) == 4) {
            if (calIndex) {
                ReduceIndexTemplate<ReduceMode::REDUCE_MAX, T, uint32_t>(
                    dstLocal, srcLocal, workLocal, count, srcRepStride, initValue);
            } else {
                ReduceNoIndexTemplate<ReduceMode::REDUCE_MAX, T>(
                    dstLocal, srcLocal, workLocal, count, srcRepStride, initValue);
            }
        } else {
            if (calIndex) {
                ReduceIndexTemplate<ReduceMode::REDUCE_MAX, T, uint16_t>(
                    dstLocal, srcLocal, workLocal, count, srcRepStride, initValue);
            } else {
                ReduceNoIndexTemplate<ReduceMode::REDUCE_MAX, T>(
                    dstLocal, srcLocal, workLocal, count, srcRepStride, initValue);
            }
        }
    } else {
        SetVectorMask<T>(mask[1], mask[0]);
        if constexpr (sizeof(T) == 4) {
            if (calIndex) {
                ReduceIndexTemplate<ReduceMode::REDUCE_MAX, true, T, uint32_t>(
                    dstLocal, srcLocal, workLocal, 0, repeat, srcRepStride, initValue);
            } else {
                ReduceNoIndexTemplate<ReduceMode::REDUCE_MAX, true, T>(
                    dstLocal, srcLocal, workLocal, 0, repeat, srcRepStride, initValue);
            }
        } else {
            if (calIndex) {
                ReduceIndexTemplate<ReduceMode::REDUCE_MAX, true, T, uint16_t>(
                    dstLocal, srcLocal, workLocal, 0, repeat, srcRepStride, initValue);
            } else {
                ReduceNoIndexTemplate<ReduceMode::REDUCE_MAX, true, T>(
                    dstLocal, srcLocal, workLocal, 0, repeat, srcRepStride, initValue);
            }
        }
    }
}

template <typename T>
__aicore__ inline void ReduceMaxImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* workLocal, const int32_t mask, const int32_t repeat,
    const int32_t srcRepStride, bool calIndex)
{
    static_assert(
        (SupportType<T, int16_t, uint16_t, int32_t, uint32_t, half, float>()),
        "The data type is not supported by ReduceMax.");
    T initValue = GetMinValue<T>();
    uint32_t maskReg = static_cast<uint32_t>(mask);
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        if constexpr (sizeof(T) == 4) {
            if (calIndex) {
                ReduceIndexTemplate<ReduceMode::REDUCE_MAX, T, uint32_t>(
                    dstLocal, srcLocal, workLocal, maskReg, srcRepStride, initValue);
            } else {
                ReduceNoIndexTemplate<ReduceMode::REDUCE_MAX, T>(
                    dstLocal, srcLocal, workLocal, maskReg, srcRepStride, initValue);
            }
        } else {
            if (calIndex) {
                ReduceIndexTemplate<ReduceMode::REDUCE_MAX, T, uint16_t>(
                    dstLocal, srcLocal, workLocal, maskReg, srcRepStride, initValue);
            } else {
                ReduceNoIndexTemplate<ReduceMode::REDUCE_MAX, T>(
                    dstLocal, srcLocal, workLocal, maskReg, srcRepStride, initValue);
            }
        }
    } else {
        if constexpr (sizeof(T) == 4) {
            if (calIndex) {
                ReduceIndexTemplate<ReduceMode::REDUCE_MAX, false, T, uint32_t>(
                    dstLocal, srcLocal, workLocal, maskReg, repeat, srcRepStride, initValue);
            } else {
                ReduceNoIndexTemplate<ReduceMode::REDUCE_MAX, false, T>(
                    dstLocal, srcLocal, workLocal, maskReg, repeat, srcRepStride, initValue);
            }
        } else {
            if (calIndex) {
                ReduceIndexTemplate<ReduceMode::REDUCE_MAX, false, T, uint16_t>(
                    dstLocal, srcLocal, workLocal, maskReg, repeat, srcRepStride, initValue);
            } else {
                ReduceNoIndexTemplate<ReduceMode::REDUCE_MAX, false, T>(
                    dstLocal, srcLocal, workLocal, maskReg, repeat, srcRepStride, initValue);
            }
        }
    }
}

template <typename T>
__aicore__ inline void ReduceMinImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* workLocal, uint32_t count, bool calIndex)
{
    static_assert(
        (SupportType<T, int16_t, uint16_t, int32_t, uint32_t, half, float, uint64_t, int64_t>()),
        "The data type is not supported by ReduceMin.");
    T initValue = GetMaxValue<T>();
    if constexpr (sizeof(T) == 8) {
        if (calIndex) {
            ReduceB64IndexTemplate<ReduceMode::REDUCE_MIN, T, uint32_t>(
                dstLocal, srcLocal, workLocal, count, initValue);
        } else {
            ReduceB64NoIndexTemplate<ReduceMode::REDUCE_MIN, T>(dstLocal, srcLocal, workLocal, count, initValue);
        }
    } else if constexpr (sizeof(T) == 4) {
        if (calIndex) {
            ReduceIndexTemplate<ReduceMode::REDUCE_MIN, T, uint32_t>(
                dstLocal, srcLocal, workLocal, count, DEFAULT_BLK_NUM, initValue);
        } else {
            ReduceNoIndexTemplate<ReduceMode::REDUCE_MIN, T>(
                dstLocal, srcLocal, workLocal, count, DEFAULT_BLK_NUM, initValue);
        }
    } else {
        if (calIndex) {
            ReduceIndexTemplate<ReduceMode::REDUCE_MIN, T, uint16_t>(
                dstLocal, srcLocal, workLocal, count, DEFAULT_BLK_NUM, initValue);
        } else {
            ReduceNoIndexTemplate<ReduceMode::REDUCE_MIN, T>(
                dstLocal, srcLocal, workLocal, count, DEFAULT_BLK_NUM, initValue);
        }
    }
}

template <typename T>
__aicore__ inline void ReduceMinImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* workLocal, const uint64_t mask[], const int32_t repeat,
    const int32_t srcRepStride, bool calIndex)
{
    static_assert(
        (SupportType<T, int16_t, uint16_t, int32_t, uint32_t, half, float>()),
        "The data type is not supported by ReduceMin.");
    T initValue = GetMaxValue<T>();
    uint32_t count = static_cast<uint32_t>(mask[0]);
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        if constexpr (sizeof(T) == 4) {
            if (calIndex) {
                ReduceIndexTemplate<ReduceMode::REDUCE_MIN, T, uint32_t>(
                    dstLocal, srcLocal, workLocal, count, srcRepStride, initValue);
            } else {
                ReduceNoIndexTemplate<ReduceMode::REDUCE_MIN, T>(
                    dstLocal, srcLocal, workLocal, count, srcRepStride, initValue);
            }
        } else {
            if (calIndex) {
                ReduceIndexTemplate<ReduceMode::REDUCE_MIN, T, uint16_t>(
                    dstLocal, srcLocal, workLocal, count, srcRepStride, initValue);
            } else {
                ReduceNoIndexTemplate<ReduceMode::REDUCE_MIN, T>(
                    dstLocal, srcLocal, workLocal, count, srcRepStride, initValue);
            }
        }
    } else {
        SetVectorMask<T>(mask[1], mask[0]);
        if constexpr (sizeof(T) == 4) {
            if (calIndex) {
                ReduceIndexTemplate<ReduceMode::REDUCE_MIN, true, T, uint32_t>(
                    dstLocal, srcLocal, workLocal, 0, repeat, srcRepStride, initValue);
            } else {
                ReduceNoIndexTemplate<ReduceMode::REDUCE_MIN, true, T>(
                    dstLocal, srcLocal, workLocal, 0, repeat, srcRepStride, initValue);
            }
        } else {
            if (calIndex) {
                ReduceIndexTemplate<ReduceMode::REDUCE_MIN, true, T, uint16_t>(
                    dstLocal, srcLocal, workLocal, 0, repeat, srcRepStride, initValue);
            } else {
                ReduceNoIndexTemplate<ReduceMode::REDUCE_MIN, true, T>(
                    dstLocal, srcLocal, workLocal, 0, repeat, srcRepStride, initValue);
            }
        }
    }
}

template <typename T>
__aicore__ inline void ReduceMinImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* workLocal, const int32_t mask, const int32_t repeat,
    const int32_t srcRepStride, bool calIndex)
{
    static_assert(
        (SupportType<T, int16_t, uint16_t, int32_t, uint32_t, half, float>()),
        "The data type is not supported by ReduceMin.");
    T initValue = GetMaxValue<T>();
    uint32_t maskReg = static_cast<uint32_t>(mask);
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        if constexpr (sizeof(T) == 4) {
            if (calIndex) {
                ReduceIndexTemplate<ReduceMode::REDUCE_MIN, T, uint32_t>(
                    dstLocal, srcLocal, workLocal, maskReg, srcRepStride, initValue);
            } else {
                ReduceNoIndexTemplate<ReduceMode::REDUCE_MIN, T>(
                    dstLocal, srcLocal, workLocal, maskReg, srcRepStride, initValue);
            }
        } else {
            if (calIndex) {
                ReduceIndexTemplate<ReduceMode::REDUCE_MIN, T, uint16_t>(
                    dstLocal, srcLocal, workLocal, maskReg, srcRepStride, initValue);
            } else {
                ReduceNoIndexTemplate<ReduceMode::REDUCE_MIN, T>(
                    dstLocal, srcLocal, workLocal, maskReg, srcRepStride, initValue);
            }
        }
    } else {
        if constexpr (sizeof(T) == 4) {
            if (calIndex) {
                ReduceIndexTemplate<ReduceMode::REDUCE_MIN, false, T, uint32_t>(
                    dstLocal, srcLocal, workLocal, maskReg, repeat, srcRepStride, initValue);
            } else {
                ReduceNoIndexTemplate<ReduceMode::REDUCE_MIN, false, T>(
                    dstLocal, srcLocal, workLocal, maskReg, repeat, srcRepStride, initValue);
            }
        } else {
            if (calIndex) {
                ReduceIndexTemplate<ReduceMode::REDUCE_MIN, false, T, uint16_t>(
                    dstLocal, srcLocal, workLocal, maskReg, repeat, srcRepStride, initValue);
            } else {
                ReduceNoIndexTemplate<ReduceMode::REDUCE_MIN, false, T>(
                    dstLocal, srcLocal, workLocal, maskReg, repeat, srcRepStride, initValue);
            }
        }
    }
}

template <typename T>
__aicore__ inline void GetReduceMaxMinCountImpl(T& maxMinValue, T& maxMinIndex)
{
    ASCENDC_ASSERT((false), "GetReduceRepeatMaxMinSpr is not supported on current device");
}

template <typename T>
__aicore__ inline void GetReduceMaxMinCountImpl(T& maxMinValue)
{
    ASCENDC_ASSERT((false), "GetReduceRepeatMaxMinSpr is not supported on current device");
}

template <typename T>
__aicore__ inline T GetAccValImpl()
{
    if constexpr (sizeof(T) == 2) {
        return accValHalf;
    } else {
        return accValFloat;
    }
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_REDUCE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_REDUCE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_REDUCE_IMPL_H__
#endif
