/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file broadcast_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/pad/broadcast/broadcast_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/pad/broadcast.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PAD_BROADCAST_BROADCAST_C310_IMPL_H__
#endif

#ifndef IMPL_PAD_BROADCAST_BROADCAST_C310_IMPL_H
#define IMPL_PAD_BROADCAST_BROADCAST_C310_IMPL_H

#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "broadcast_gather_3510_impl.h"

namespace AscendC {
namespace BroadcastInternal {
template <typename T>
__simd_callee__ inline void E2bLoad(Reg::RegTensor<T>& dstReg, __ubuf__ T* srcUb)
{
    if constexpr (sizeof(T) == 2) {
        Reg::LoadAlign<T, Reg::LoadDist::DIST_E2B_B16>(dstReg, srcUb);
    } else {
        Reg::LoadAlign<T, Reg::LoadDist::DIST_E2B_B32>(dstReg, srcUb);
    }
}

template <typename T>
__simd_callee__ inline void BrcLoad(Reg::RegTensor<T>& dstReg, __ubuf__ T* srcUb)
{
    if constexpr (sizeof(T) == 2) {
        Reg::LoadAlign<T, Reg::LoadDist::DIST_BRC_B16>(dstReg, srcUb);
    } else if constexpr (sizeof(T) == 4) {
        Reg::LoadAlign<T, Reg::LoadDist::DIST_BRC_B32>(dstReg, srcUb);
    } else {
        Reg::LoadAlign<T, Reg::LoadDist::DIST_BRC_B8>(dstReg, srcUb);
    }
}

template <typename T>
__simd_vf__ inline void BrcDuplicate(__ubuf__ T* dstUb, __ubuf__ T* srcUb, uint32_t dstSize)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    uint16_t repeatTimes = CeilDivision(dstSize, VF_LEN);
    uint32_t sreg = dstSize;

    Reg::MaskReg pregCnt;
    Reg::RegTensor<T> srcReg;
    BrcLoad<T>(srcReg, srcUb);
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        pregCnt = Reg::UpdateMask<T>(sreg);
        Reg::StoreAlign(dstUb + i * VF_LEN, srcReg, pregCnt);
    }
}

template <typename T>
__simd_vf__ inline void GenLastGatherIndex(__ubuf__ T* indexUb, uint32_t size1, uint32_t offset)
{
    Reg::MaskReg pregFull = Reg::CreateMask<T>();
    Reg::RegTensor<T> indexReg;
    Reg::RegTensor<T> tmpReg;

    Reg::Duplicate(indexReg, (T)size1, pregFull);
    Reg::Arange(tmpReg, (T)offset);
    Reg::Div(indexReg, tmpReg, indexReg, pregFull);

    Reg::StoreAlign(indexUb, indexReg, pregFull);
}

template <typename T>
__simd_vf__ inline void GenNlastGatherIndex(__ubuf__ T* indexUb, uint32_t size1, uint32_t offset)
{
    Reg::MaskReg pregFull = Reg::CreateMask<T>();
    Reg::RegTensor<T> indexReg;
    Reg::RegTensor<T> tmpReg;
    Reg::RegTensor<T> dstReg;

    Reg::Duplicate(indexReg, (T)size1, pregFull);
    Reg::Arange(tmpReg, (T)offset);
    Reg::Div(dstReg, tmpReg, indexReg, pregFull);
    Reg::Mul(dstReg, indexReg, dstReg, pregFull);
    Reg::Sub(indexReg, tmpReg, dstReg, pregFull);

    Reg::StoreAlign(indexUb, indexReg, pregFull);
}

template <typename T, typename IndexT>
__simd_vf__ inline void BrcLastGatherOne(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, __ubuf__ IndexT* indexUb, uint16_t size0, uint16_t size1)
{
    constexpr uint32_t VF_LEN_HALF = GetVecLen() / 2 / sizeof(T);
    uint32_t main = size0 * size1;

    Reg::MaskReg pregFull = Reg::CreateMask<IndexT>();
    Reg::MaskReg pregCnt;
    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<T> dummyReg;
    Reg::RegTensor<IndexT> srcReg1;
    Reg::RegTensor<IndexT> srcReg2;
    Reg::RegTensor<IndexT> indexReg1;
    Reg::RegTensor<IndexT> indexReg2;

    Reg::LoadAlign(indexReg1, indexUb);
    if constexpr (sizeof(T) == sizeof(uint8_t)) {
        Reg::LoadAlign(indexReg2, indexUb + VF_LEN_HALF);
    }
    uint32_t sreg = main;
    pregCnt = Reg::UpdateMask<T>(sreg);
    if constexpr (sizeof(T) == sizeof(uint8_t)) {
        Reg::Gather(srcReg1, srcUb, indexReg1, pregFull);
        Reg::Gather(srcReg2, srcUb, indexReg2, pregFull);
        Reg::DeInterleave(srcReg, dummyReg, (Reg::RegTensor<T>&)srcReg1, (Reg::RegTensor<T>&)srcReg2);
    } else {
        Reg::Gather(srcReg, srcUb, indexReg1, pregCnt);
    }
    Reg::StoreAlign(dstUb, srcReg, pregCnt);
}

template <typename T, typename IndexT>
__simd_vf__ inline void BrcLastGatherTwo(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, __ubuf__ IndexT* indexUb, uint16_t size0, uint16_t size1)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    constexpr uint32_t VF_LEN_HALF = GetVecLen() / 2 / sizeof(T);
    uint16_t factor = VF_LEN / size1;
    uint16_t repeatTimes = CeilDivision(size0, factor) - 1;
    uint32_t main = factor * size1;
    uint32_t mainBlock = main * repeatTimes;
    uint32_t offset = factor * repeatTimes;
    uint32_t tail = size0 * size1 - mainBlock;

    Reg::MaskReg pregFull = Reg::CreateMask<IndexT>();
    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<T> dummyReg;
    Reg::RegTensor<IndexT> indexReg1;
    Reg::RegTensor<IndexT> indexReg2;
    Reg::RegTensor<IndexT> factorReg;
    Reg::RegTensor<IndexT> srcReg1;
    Reg::RegTensor<IndexT> srcReg2;
    Reg::RegTensor<IndexT> dstReg;
    Reg::RegTensor<IndexT> tmpReg;
    Reg::UnalignReg ureg0;

    Reg::Duplicate(factorReg, (IndexT)factor, pregFull);
    Reg::LoadAlign(indexReg1, indexUb);
    if constexpr (sizeof(T) == sizeof(uint8_t)) {
        Reg::LoadAlign(indexReg2, indexUb + VF_LEN_HALF);
    }
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        Reg::Muls(tmpReg, factorReg, (IndexT)i, pregFull);
        Reg::Add(dstReg, tmpReg, indexReg1, pregFull);
        if constexpr (sizeof(T) == sizeof(uint8_t)) {
            Reg::Gather(srcReg1, srcUb, dstReg, pregFull);
            Reg::Add(dstReg, tmpReg, indexReg2, pregFull);
            Reg::Gather(srcReg2, srcUb, dstReg, pregFull);
            Reg::DeInterleave(srcReg, dummyReg, (Reg::RegTensor<T>&)srcReg1, (Reg::RegTensor<T>&)srcReg2);
        } else {
            Reg::Gather(srcReg, srcUb, dstReg, pregFull);
        }
        Reg::StoreUnAlign(dstUb, srcReg, ureg0, main);
    }
    Reg::Adds(dstReg, indexReg1, (IndexT)offset, pregFull);
    if constexpr (sizeof(T) == sizeof(uint8_t)) {
        Reg::Gather(srcReg1, srcUb, dstReg, pregFull);
        Reg::Adds(dstReg, indexReg2, (IndexT)offset, pregFull);
        Reg::Gather(srcReg2, srcUb, dstReg, pregFull);
        Reg::DeInterleave(srcReg, dummyReg, (Reg::RegTensor<T>&)srcReg1, (Reg::RegTensor<T>&)srcReg2);
    } else {
        Reg::Gather(srcReg, srcUb, dstReg, pregFull);
    }
    Reg::StoreUnAlign(dstUb, srcReg, ureg0, tail);
    Reg::StoreUnAlignPost(dstUb, ureg0, 0);
}

template <typename T, typename IndexT>
__simd_vf__ inline void BrcNlastGatherOne(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, __ubuf__ IndexT* indexUb, uint16_t size0, uint16_t size1)
{
    constexpr uint32_t VF_LEN_HALF = GetVecLen() / 2 / sizeof(T);
    uint32_t main = size0 * size1;

    Reg::MaskReg pregFull = Reg::CreateMask<IndexT>();
    Reg::MaskReg pregCnt;
    Reg::RegTensor<IndexT> indexReg1;
    Reg::RegTensor<IndexT> indexReg2;
    Reg::RegTensor<IndexT> srcReg1;
    Reg::RegTensor<IndexT> srcReg2;
    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<T> dummyReg;
    Reg::UnalignReg ureg0;

    uint32_t sreg = main;
    pregCnt = Reg::UpdateMask<T>(sreg);
    Reg::LoadAlign(indexReg1, indexUb);
    if constexpr (sizeof(T) == sizeof(uint8_t)) {
        Reg::LoadAlign(indexReg2, indexUb + VF_LEN_HALF);
        Reg::Gather(srcReg1, srcUb, indexReg1, pregFull);
        Reg::Gather(srcReg2, srcUb, indexReg2, pregFull);
        Reg::DeInterleave(srcReg, dummyReg, (Reg::RegTensor<T>&)srcReg1, (Reg::RegTensor<T>&)srcReg2);
    } else {
        Reg::Gather(srcReg, srcUb, indexReg1, pregCnt);
    }
    Reg::StoreAlign(dstUb, srcReg, pregCnt);
}

template <typename T, typename IndexT>
__simd_vf__ inline void BrcNlastGatherTwo(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, __ubuf__ IndexT* indexUb, uint16_t size0, uint16_t size1)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    constexpr uint32_t VF_LEN_HALF = GetVecLen() / 2 / sizeof(T);
    uint16_t factor = VF_LEN / size1;
    uint16_t repeatTimes = CeilDivision(size0, factor) - 1;
    uint32_t main = factor * size1;
    uint32_t mainBlock = main * repeatTimes;
    uint32_t tail = size0 * size1 - mainBlock;

    Reg::MaskReg pregFull = Reg::CreateMask<IndexT>();
    Reg::RegTensor<IndexT> indexReg1;
    Reg::RegTensor<IndexT> indexReg2;
    Reg::RegTensor<IndexT> srcReg1;
    Reg::RegTensor<IndexT> srcReg2;
    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<T> dummyReg;
    Reg::UnalignReg ureg0;

    Reg::LoadAlign(indexReg1, indexUb);
    if constexpr (sizeof(T) == sizeof(uint8_t)) {
        Reg::LoadAlign(indexReg2, indexUb + VF_LEN_HALF);
        Reg::Gather(srcReg1, srcUb, indexReg1, pregFull);
        Reg::Gather(srcReg2, srcUb, indexReg2, pregFull);
        Reg::DeInterleave(srcReg, dummyReg, (Reg::RegTensor<T>&)srcReg1, (Reg::RegTensor<T>&)srcReg2);
    } else {
        Reg::Gather(srcReg, srcUb, indexReg1, pregFull);
    }
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        Reg::StoreUnAlign(dstUb, srcReg, ureg0, main);
    }
    Reg::StoreUnAlign(dstUb, srcReg, ureg0, tail);
    Reg::StoreUnAlignPost(dstUb, ureg0, 0);
}

template <typename T>
__simd_vf__ inline void BrcLastE2B(__ubuf__ T* dstUb, __ubuf__ T* srcUb, uint16_t size0, uint16_t size1)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    uint16_t factor = VF_LEN / size1;
    uint16_t repeatTimes = CeilDivision(size0, factor);

    Reg::MaskReg pregCnt;
    Reg::RegTensor<T> srcReg;

    uint32_t sreg = size0 * size1;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        pregCnt = Reg::UpdateMask<T>(sreg);
        E2bLoad<T>(srcReg, srcUb + i * DEFAULT_BLK_NUM);
        Reg::StoreAlign(dstUb + i * VF_LEN, srcReg, pregCnt);
    }
}

template <typename T>
__simd_vf__ inline void BrcLastE2BLargerThanVL(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, uint16_t size0, uint16_t size1, uint16_t size2, uint16_t srcStride0)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    uint16_t factor = VF_LEN / size2;
    uint16_t repeatTimes = CeilDivision(size1, factor);
    uint32_t preg = size1 * size2;
    uint32_t sreg;
    Reg::MaskReg pregCnt;
    Reg::RegTensor<T> srcReg;

    for (uint16_t i = 0; i < size0; ++i) {
        sreg = preg;
        for (uint16_t j = 0; j < repeatTimes; ++j) {
            pregCnt = Reg::UpdateMask<T>(sreg);
            E2bLoad<T>(srcReg, srcUb + j * DEFAULT_BLK_NUM + i * srcStride0);
            Reg::StoreAlign(dstUb + i * size1 * size2 + j * VF_LEN, srcReg, pregCnt);
        }
    }
}

template <typename T>
__simd_vf__ inline void BrcLastE2BLessThanVL(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, uint16_t size0, uint16_t size1, uint16_t size2, uint16_t srcStride0)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    uint32_t preg = size1 * size2;
    uint32_t sreg;
    Reg::MaskReg pregCnt;
    Reg::RegTensor<T> srcReg;
    sreg = preg;
    pregCnt = Reg::UpdateMask<T>(sreg);
    for (uint16_t i = 0; i < size0; ++i) {
        E2bLoad<T>(srcReg, srcUb + i * srcStride0);
        Reg::StoreAlign(dstUb + i * size1 * size2, srcReg, pregCnt);
    }
}

template <typename T>
__simd_vf__ inline void BrcLastE2B(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, uint16_t size0, uint16_t size1, uint16_t size2, uint16_t size3,
    uint16_t srcStride0, uint16_t srcStride1)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    uint16_t factor = VF_LEN / size3;
    uint16_t repeatTimes = CeilDivision(size2, factor);
    uint32_t preg = size2 * size3;
    uint32_t sreg;
    Reg::MaskReg pregCnt;
    Reg::RegTensor<T> srcReg;
    for (uint16_t i = 0; i < size0; ++i) {
        for (uint16_t j = 0; j < size1; ++j) {
            sreg = preg;
            for (uint16_t k = 0; k < repeatTimes; ++k) {
                pregCnt = Reg::UpdateMask<T>(sreg);
                E2bLoad<T>(srcReg, srcUb + i * srcStride0 + j * srcStride1 + k * DEFAULT_BLK_NUM);
                Reg::StoreAlign(dstUb + i * size1 * size2 * size3 + j * size2 * size3 + k * VF_LEN, srcReg, pregCnt);
            }
        }
    }
}

template <typename T>
__simd_vf__ inline void BrcNlastGatherBOne(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, __ubuf__ uint32_t* indexUb, uint16_t size0, uint16_t size1)
{
    uint32_t main = size0 * size1;

    Reg::MaskReg pregFull = Reg::CreateMask<T>();
    Reg::MaskReg pregCnt;
    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<uint32_t> indexReg;

    Reg::LoadAlign(indexReg, indexUb);
    Reg::GatherB(srcReg, srcUb, indexReg, pregFull);
    uint32_t sreg = main;
    pregCnt = Reg::UpdateMask<T>(sreg);
    Reg::StoreAlign(dstUb, srcReg, pregCnt);
}

template <typename T>
__simd_vf__ inline void BrcNlastGatherBTwo(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, __ubuf__ uint32_t* indexUb, uint16_t size0, uint16_t size1)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    uint16_t factor = VF_LEN / size1;
    uint16_t repeatTimes = CeilDivision(size0, factor) - 1;
    uint32_t main = factor * size1;
    uint32_t mainBlock = main * repeatTimes;
    uint32_t tail = size0 * size1 - mainBlock;

    Reg::MaskReg pregFull = Reg::CreateMask<T>();
    Reg::MaskReg pregCnt;
    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<uint32_t> indexReg;

    Reg::LoadAlign(indexReg, indexUb);
    Reg::GatherB(srcReg, srcUb, indexReg, pregFull);
    uint32_t sreg = main;
    pregCnt = Reg::UpdateMask<T>(sreg);
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        Reg::StoreAlign(dstUb + i * main, srcReg, pregCnt);
    }
    sreg = tail;
    pregCnt = Reg::UpdateMask<T>(sreg);
    Reg::StoreAlign(dstUb + mainBlock, srcReg, pregCnt);
}

template <typename T>
__simd_vf__ inline void BrcLastLessThanVLAligned(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, uint16_t size0, uint16_t size1, uint16_t size2, uint16_t srcStride0,
    uint16_t srcStride1)
{
    Reg::MaskReg pregCnt;
    Reg::RegTensor<T> srcReg;

    uint32_t sreg = size2;
    pregCnt = Reg::UpdateMask<T>(sreg);
    for (uint16_t i = 0; i < size1; ++i) {
        for (uint16_t j = 0; j < size0; ++j) {
            BrcLoad<T>(srcReg, srcUb + j * srcStride0 + i * srcStride1);
            Reg::StoreAlign(dstUb + j * size1 * size2 + i * size2, srcReg, pregCnt);
        }
    }
}

template <typename T>
__simd_vf__ inline void BrcLastLessThanVLAligned(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, uint16_t size0, uint16_t size1, uint16_t size2, uint16_t size3,
    uint16_t srcStride0, uint16_t srcStride1, uint16_t srcStride2)
{
    Reg::MaskReg pregCnt;
    Reg::RegTensor<T> srcReg;

    uint32_t sreg = size3;
    pregCnt = Reg::UpdateMask<T>(sreg);
    for (uint16_t i = 0; i < size0; ++i) {
        for (uint16_t j = 0; j < size2; ++j) {
            for (uint16_t k = 0; k < size1; ++k) {
                BrcLoad<T>(srcReg, srcUb + i * srcStride0 + j * srcStride2 + k * srcStride1);
                Reg::StoreAlign(dstUb + i * size1 * size2 * size3 + k * size2 * size3 + j * size3, srcReg, pregCnt);
            }
        }
    }
}

template <typename T>
__simd_vf__ inline void BrcNlastLessThanVLAligned(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, uint16_t size0, uint16_t size1, uint16_t size2, uint16_t size3,
    uint16_t srcStride0, uint16_t srcStride1, uint16_t srcStride2)
{
    Reg::MaskReg pregCnt;
    Reg::RegTensor<T> srcReg;

    uint32_t sreg = size3;
    pregCnt = Reg::UpdateMask<T>(sreg);
    for (uint16_t i = 0; i < size1; ++i) {
        for (uint16_t j = 0; j < size0; ++j) {
            for (uint16_t k = 0; k < size2; ++k) {
                Reg::LoadAlign(srcReg, srcUb + i * srcStride1 + j * srcStride0 + k * srcStride2);
                Reg::StoreAlign(dstUb + j * size1 * size2 * size3 + i * size2 * size3 + k * size3, srcReg, pregCnt);
            }
        }
    }
}

template <typename T>
__simd_vf__ inline void BrcLastLessThanVLUnaligned(__ubuf__ T* dstUb, __ubuf__ T* srcUb, uint16_t size0, uint16_t size1)
{
    Reg::RegTensor<T> srcReg;
    Reg::UnalignReg ureg0;

    for (uint16_t i = 0; i < size0; ++i) {
        BrcLoad<T>(srcReg, srcUb + i);
        Reg::StoreUnAlign(dstUb, srcReg, ureg0, size1);
    }
    Reg::StoreUnAlignPost(dstUb, ureg0, 0);
}

template <typename T>
__simd_vf__ inline void BrcLastLessThanVLUnaligned(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, uint16_t size0, uint16_t size1, uint16_t size2, uint16_t srcStride0,
    uint16_t srcStride1)
{
    Reg::RegTensor<T> srcReg;
    Reg::UnalignReg ureg0;

    for (uint16_t i = 0; i < size0; ++i) {
        for (uint16_t j = 0; j < size1; ++j) {
            BrcLoad<T>(srcReg, srcUb + j * srcStride1 + i * srcStride0);
            Reg::StoreUnAlign(dstUb, srcReg, ureg0, size2);
        }
    }
    Reg::StoreUnAlignPost(dstUb, ureg0, 0);
}

template <typename T>
__simd_vf__ inline void BrcLastLessThanVLUnaligned(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, uint16_t size0, uint16_t size1, uint16_t size2, uint16_t size3,
    uint16_t srcStride0, uint16_t srcStride1, uint16_t srcStride2)
{
    Reg::RegTensor<T> srcReg;
    Reg::UnalignReg ureg0;

    for (uint16_t i = 0; i < size0; ++i) {
        for (uint16_t j = 0; j < size1; ++j) {
            for (uint16_t k = 0; k < size2; ++k) {
                BrcLoad<T>(srcReg, srcUb + i * srcStride0 + j * srcStride1 + k * srcStride2);
                Reg::StoreUnAlign(dstUb, srcReg, ureg0, size3);
            }
        }
    }
    Reg::StoreUnAlignPost(dstUb, ureg0, 0);
}

template <typename T>
__simd_vf__ inline void BrcNlastLessThanVLUnaligned(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, uint16_t size0, uint16_t size1)
{
    Reg::MaskReg pregCnt;
    Reg::RegTensor<T> srcReg;
    Reg::UnalignReg ureg0;

    uint32_t sreg = size1;
    pregCnt = Reg::UpdateMask<T>(sreg);
    Reg::LoadAlign(srcReg, srcUb);
    for (uint16_t i = 0; i < size0; ++i) {
        Reg::StoreUnAlign(dstUb, srcReg, ureg0, size1);
    }
    Reg::StoreUnAlignPost(dstUb, ureg0, 0);
}

template <typename T>
__simd_vf__ inline void BrcNlastLessThanVLUnaligned(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, uint16_t size0, uint16_t size1, uint16_t size2, uint16_t srcStride0,
    uint16_t srcStride1)
{
    Reg::MaskReg pregCnt;
    Reg::RegTensor<T> srcReg;
    Reg::UnalignReg ureg0, ureg1;

    uint32_t sreg = size2;
    pregCnt = Reg::UpdateMask<T>(sreg);
    for (uint16_t i = 0; i < size0; ++i) {
        for (uint16_t j = 0; j < size1; ++j) {
            auto srcUbT = srcUb + i * srcStride0 + j * srcStride1;
            Reg::LoadUnAlignPre(ureg0, srcUbT);
            Reg::LoadUnAlign(srcReg, ureg0, srcUbT, size2);
            Reg::StoreUnAlign(dstUb, srcReg, ureg1, size2);
        }
    }
    Reg::StoreUnAlignPost(dstUb, ureg1, 0);
}

template <typename T>
__simd_vf__ inline void BrcNlastLessThanVLUnaligned(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, uint16_t size0, uint16_t size1, uint16_t size2, uint16_t size3,
    uint16_t srcStride0, uint16_t srcStride1, uint16_t srcStride2)
{
    Reg::MaskReg pregCnt;
    Reg::RegTensor<T> srcReg;
    Reg::UnalignReg ureg0, ureg1;

    uint32_t sreg = size3;
    pregCnt = Reg::UpdateMask<T>(sreg);
    for (uint16_t i = 0; i < size0; ++i) {
        for (uint16_t j = 0; j < size1; ++j) {
            for (uint16_t k = 0; k < size2; ++k) {
                __ubuf__ T* srcUbTmp = srcUb + i * srcStride0 + j * srcStride1 + k * srcStride2;
                Reg::LoadUnAlignPre(ureg0, srcUbTmp);
                Reg::LoadUnAlign(srcReg, ureg0, srcUbTmp, size3);
                Reg::StoreUnAlign(dstUb, srcReg, ureg1, size3);
            }
        }
    }
    Reg::StoreUnAlignPost(dstUb, ureg1, 0);
}

template <typename T>
__simd_vf__ inline void BrcLastLargerThanVLAligned(__ubuf__ T* dstUb, __ubuf__ T* srcUb, uint16_t size0, uint16_t size1)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    uint16_t factor = CeilDivision(size1, VF_LEN);

    Reg::MaskReg pregFull = Reg::CreateMask<T>();
    Reg::MaskReg pregCnt;
    Reg::RegTensor<T> srcReg;

    for (uint16_t i = 0; i < size0; ++i) {
        BrcLoad<T>(srcReg, srcUb + i);
        uint32_t sreg = size1;
        for (uint16_t j = 0; j < factor; ++j) {
            pregCnt = Reg::UpdateMask<T>(sreg);
            Reg::StoreAlign(dstUb + i * size1 + j * VF_LEN, srcReg, pregCnt);
        }
    }
}

template <typename T>
__simd_vf__ inline void BrcLastLargerThanVLAligned(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, uint16_t size0, uint16_t size1, uint16_t size2, uint16_t srcStride0,
    uint16_t srcStride1)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    uint16_t factor = CeilDivision(size2, VF_LEN);

    Reg::MaskReg pregFull = Reg::CreateMask<T>();
    Reg::MaskReg pregCnt;
    Reg::RegTensor<T> srcReg;

    for (uint16_t i = 0; i < size1; ++i) {
        for (uint16_t j = 0; j < size0; ++j) {
            BrcLoad<T>(srcReg, srcUb + i * srcStride1 + j * srcStride0);
            uint32_t sreg = size2;
            for (uint16_t k = 0; k < factor; ++k) {
                pregCnt = Reg::UpdateMask<T>(sreg);
                Reg::StoreAlign(dstUb + j * size1 * size2 + i * size2 + k * VF_LEN, srcReg, pregCnt);
            }
        }
    }
}

template <typename T>
__simd_vf__ inline void BrcLastLargerThanVLAligned(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, uint16_t size0, uint16_t size1, uint16_t size2, uint16_t size3,
    uint16_t srcStride0, uint16_t srcStride1, uint16_t srcStride2)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    uint16_t factor = CeilDivision(size3, VF_LEN);

    Reg::MaskReg pregFull = Reg::CreateMask<T>();
    Reg::MaskReg pregCnt;
    Reg::RegTensor<T> srcReg;
    for (uint16_t i = 0; i < size0; ++i) {
        for (uint16_t j = 0; j < size2; ++j) {
            uint32_t sreg = size3;
            for (uint16_t k = 0; k < factor; ++k) {
                pregCnt = Reg::UpdateMask<T>(sreg);
                for (uint16_t t = 0; t < size1; ++t) {
                    BrcLoad<T>(srcReg, srcUb + i * srcStride0 + j * srcStride2 + t * srcStride1);
                    Reg::StoreAlign(
                        dstUb + i * size1 * size2 * size3 + t * size2 * size3 + j * size3 + k * VF_LEN, srcReg,
                        pregCnt);
                }
            }
        }
    }
}

template <typename T>
__simd_vf__ inline void BrcNlastLargerThanVLAlignedWithBlock(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, uint16_t size0, uint16_t size1)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    uint16_t factor = CeilDivision(size1, VF_LEN);

    Reg::MaskReg pregFull = Reg::CreateMask<T>();
    Reg::MaskReg pregCnt;
    Reg::RegTensor<T> srcReg;

    for (uint16_t i = 0; i < size0; ++i) {
        uint32_t sreg = size1;
        for (uint16_t j = 0; j < factor; ++j) {
            pregCnt = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign(srcReg, srcUb + j * VF_LEN);
            Reg::StoreAlign(dstUb + i * size1 + j * VF_LEN, srcReg, pregCnt);
        }
    }
}

template <typename T>
__simd_vf__ inline void BrcNlastLargerThanVLAlignedWithBlock(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, uint16_t size0, uint16_t size1, uint16_t size2, uint16_t srcStride0,
    uint16_t srcStride1)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    uint16_t factor = CeilDivision(size2, VF_LEN);
    uint16_t jStride = srcStride1 == 0 ? 0 : VF_LEN;

    Reg::MaskReg pregFull = Reg::CreateMask<T>();
    Reg::MaskReg pregCnt;
    Reg::RegTensor<T> srcReg;

    for (uint16_t i = 0; i < size0; ++i) {
        uint32_t sreg = size2;
        for (uint16_t j = 0; j < factor; ++j) {
            pregCnt = Reg::UpdateMask<T>(sreg);
            for (uint16_t k = 0; k < size1; ++k) {
                Reg::LoadAlign(srcReg, srcUb + k * srcStride1 + i * srcStride0 + j * VF_LEN);
                Reg::StoreAlign(dstUb + i * size1 * size2 + k * size2 + j * VF_LEN, srcReg, pregCnt);
            }
        }
    }
}

template <typename T>
__simd_vf__ inline void BrcNlastLargerThanVLAlignedWithBlock(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, uint16_t size0, uint16_t size1, uint16_t size2, uint16_t size3,
    uint16_t srcStride0, uint16_t srcStride1, uint16_t srcStride2)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    uint16_t factor = CeilDivision(size3, VF_LEN);

    Reg::MaskReg pregFull = Reg::CreateMask<T>();
    Reg::MaskReg pregCnt;
    Reg::RegTensor<T> srcReg;
    for (uint16_t i = 0; i < size1; ++i) {
        uint32_t sreg = size3;
        for (uint16_t j = 0; j < factor; ++j) {
            pregCnt = Reg::UpdateMask<T>(sreg);
            for (uint16_t k = 0; k < size0; ++k) {
                for (uint16_t t = 0; t < size2; ++t) {
                    Reg::LoadAlign(srcReg, srcUb + j * VF_LEN + i * srcStride1 + k * srcStride0 + t * srcStride2);
                    Reg::StoreAlign(
                        dstUb + k * size1 * size2 * size3 + i * size2 * size3 + t * size3 + j * VF_LEN, srcReg,
                        pregCnt);
                }
            }
        }
    }
}

template <typename T>
__simd_vf__ inline void BrcNlastLargerThanVLAlignedWithVL(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, uint16_t size0, uint16_t size1)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    uint16_t factor = CeilDivision(size1, VF_LEN);

    Reg::MaskReg pregFull = Reg::CreateMask<T>();
    Reg::RegTensor<T> srcReg;

    for (uint16_t i = 0; i < factor; ++i) {
        Reg::LoadAlign(srcReg, srcUb + i * VF_LEN);
        for (uint16_t j = 0; j < size0; ++j) {
            Reg::StoreAlign(dstUb + i * VF_LEN + j * size1, srcReg, pregFull);
        }
    }
}

template <typename T>
__simd_vf__ inline void BrcNlastLargerThanVLAlignedWithVL(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, uint16_t size0, uint16_t size1, uint16_t size2, uint16_t srcStride0,
    uint16_t srcStride1)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    uint16_t factor = CeilDivision(size2, VF_LEN);

    Reg::MaskReg pregFull = Reg::CreateMask<T>();
    Reg::RegTensor<T> srcReg;

    for (uint16_t i = 0; i < size0; ++i) {
        for (uint16_t j = 0; j < factor; ++j) {
            for (uint16_t k = 0; k < size1; ++k) {
                Reg::LoadAlign(srcReg, srcUb + i * srcStride0 + j * VF_LEN + k * srcStride1);
                Reg::StoreAlign(dstUb + j * VF_LEN + k * size2 + i * size1 * size2, srcReg, pregFull);
            }
        }
    }
}

template <typename T>
__simd_vf__ inline void BrcNlastLargerThanVLAlignedWithVL(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, uint16_t size0, uint16_t size1, uint16_t size2, uint16_t size3,
    uint16_t srcStride0, uint16_t srcStride1, uint16_t srcStride2)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    uint16_t factor = CeilDivision(size3, VF_LEN);

    Reg::MaskReg pregFull = Reg::CreateMask<T>();
    Reg::RegTensor<T> srcReg;

    for (uint16_t i = 0; i < size1; ++i) {
        for (uint16_t j = 0; j < factor; ++j) {
            for (uint16_t k = 0; k < size0; ++k) {
                for (uint16_t t = 0; t < size2; ++t) {
                    Reg::LoadAlign(srcReg, srcUb + i * srcStride1 + j * VF_LEN + k * srcStride0 + t * srcStride2);
                    Reg::StoreAlign(
                        dstUb + j * VF_LEN + t * size3 + i * size2 * size3 + k * size1 * size2 * size3, srcReg,
                        pregFull);
                }
            }
        }
    }
}

template <typename T>
__simd_vf__ inline void BrcLastLargerThanVLUnaligned(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, uint16_t size0, uint16_t size1)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    uint16_t factor = size1 / VF_LEN;
    uint32_t size1tail = size1 - factor * VF_LEN;

    Reg::RegTensor<T> srcReg;
    Reg::UnalignReg ureg0;

    for (uint16_t i = 0; i < size0; ++i) {
        BrcLoad<T>(srcReg, srcUb + i);
        for (uint16_t j = 0; j < factor; ++j) {
            Reg::StoreUnAlign(dstUb, srcReg, ureg0, VF_LEN);
        }
        Reg::StoreUnAlign(dstUb, srcReg, ureg0, size1tail);
    }
    Reg::StoreUnAlignPost(dstUb, ureg0, 0);
}

template <typename T>
__simd_vf__ inline void BrcLastLargerThanVLUnaligned(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, uint16_t size0, uint16_t size1, uint16_t size2, uint16_t srcStride0,
    uint16_t srcStride1)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    uint16_t factor = size2 / VF_LEN;
    uint32_t size2tail = size2 - factor * VF_LEN;

    Reg::RegTensor<T> srcReg;
    Reg::UnalignReg ureg0;

    for (uint16_t i = 0; i < size0; ++i) {
        for (uint16_t j = 0; j < size1; ++j) {
            BrcLoad<T>(srcReg, srcUb + j * srcStride1 + i * srcStride0);
            for (uint16_t k = 0; k < factor; ++k) {
                Reg::StoreUnAlign(dstUb, srcReg, ureg0, VF_LEN);
            }
            Reg::StoreUnAlign(dstUb, srcReg, ureg0, size2tail);
        }
    }
    Reg::StoreUnAlignPost(dstUb, ureg0, 0);
}

template <typename T>
__simd_vf__ inline void BrcLastLargerThanVLUnaligned(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, uint16_t size0, uint16_t size1, uint16_t size2, uint16_t size3,
    uint16_t srcStride0, uint16_t srcStride1, uint16_t srcStride2)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    uint16_t factor = size3 / VF_LEN;
    uint32_t size3tail = size3 - factor * VF_LEN;

    Reg::RegTensor<T> srcReg;
    Reg::UnalignReg ureg0;

    for (uint16_t i = 0; i < size0; ++i) {
        for (uint16_t j = 0; j < size1; ++j) {
            for (uint16_t k = 0; k < size2; ++k) {
                BrcLoad<T>(srcReg, srcUb + i * srcStride0 + j * srcStride1 + k * srcStride2);
                for (uint16_t t = 0; t < factor; ++t) {
                    Reg::StoreUnAlign(dstUb, srcReg, ureg0, VF_LEN);
                }
                Reg::StoreUnAlign(dstUb, srcReg, ureg0, size3tail);
            }
        }
    }
    Reg::StoreUnAlignPost(dstUb, ureg0, 0);
}

template <typename T>
__simd_vf__ inline void BrcNlastLargerThanVLUnaligned(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, uint16_t size0, uint16_t size1)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    uint16_t factor = size1 / VF_LEN;
    uint32_t size1tail = size1 - factor * VF_LEN;

    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<T> tmpReg;
    Reg::UnalignReg ureg0;

    uint32_t sreg = size1tail;
    Reg::LoadAlign(tmpReg, srcUb + factor * VF_LEN);
    for (uint16_t i = 0; i < size0; ++i) {
        for (uint16_t j = 0; j < factor; ++j) {
            Reg::LoadAlign(srcReg, srcUb + j * VF_LEN);
            Reg::StoreUnAlign(dstUb, srcReg, ureg0, VF_LEN);
        }
        Reg::StoreUnAlign(dstUb, tmpReg, ureg0, size1tail);
    }
    Reg::StoreUnAlignPost(dstUb, ureg0, 0);
}

template <typename T>
__simd_vf__ inline void BrcNlastLargerThanVLUnaligned(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, uint16_t size0, uint16_t size1, uint16_t size2, uint16_t srcStride0,
    uint16_t srcStride1)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    uint16_t factor = size2 / VF_LEN;
    uint32_t size2tail = size2 - factor * VF_LEN;

    Reg::RegTensor<T> srcReg;
    Reg::UnalignReg ureg0, ureg1;

    uint32_t sreg = size2tail;
    for (uint16_t i = 0; i < size0; ++i) {
        for (uint16_t j = 0; j < size1; ++j) {
            __ubuf__ T* tmpSrcUb = srcUb + i * srcStride0 + j * srcStride1;
            Reg::LoadUnAlignPre(ureg0, tmpSrcUb);
            for (uint16_t k = 0; k < factor; ++k) {
                Reg::LoadUnAlign(srcReg, ureg0, tmpSrcUb, VF_LEN);
                Reg::StoreUnAlign(dstUb, srcReg, ureg1, VF_LEN);
            }
            Reg::LoadUnAlign(srcReg, ureg0, tmpSrcUb, sreg);
            Reg::StoreUnAlign(dstUb, srcReg, ureg1, sreg);
        }
    }
    Reg::StoreUnAlignPost(dstUb, ureg1, 0);
}

template <typename T>
__simd_vf__ inline void BrcNlastLargerThanVLUnaligned(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, uint16_t size0, uint16_t size1, uint16_t size2, uint16_t size3,
    uint16_t srcStride0, uint16_t srcStride1, uint16_t srcStride2)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    uint16_t factor = size3 / VF_LEN;
    uint32_t size3tail = size3 - factor * VF_LEN;

    Reg::RegTensor<T> srcReg;
    Reg::UnalignReg ureg0, ureg1;

    uint32_t sreg = size3tail;
    for (uint16_t i = 0; i < size0; ++i) {
        for (uint16_t j = 0; j < size1; ++j) {
            for (uint16_t k = 0; k < size2; ++k) {
                __ubuf__ T* tmpSrcUb = srcUb + i * srcStride0 + j * srcStride1 + k * srcStride2;
                Reg::LoadUnAlignPre(ureg0, tmpSrcUb);
                for (uint16_t t = 0; t < factor; ++t) {
                    Reg::LoadUnAlign(srcReg, ureg0, tmpSrcUb, VF_LEN);
                    Reg::StoreUnAlign(dstUb, srcReg, ureg1, VF_LEN);
                }
                Reg::LoadUnAlign(srcReg, ureg0, tmpSrcUb, sreg);
                Reg::StoreUnAlign(dstUb, srcReg, ureg1, sreg);
            }
        }
    }
    Reg::StoreUnAlignPost(dstUb, ureg1, 0);
}

template <typename T, int32_t constRank = -1>
__aicore__ inline bool BrcLastWrapperForTwoDim(__ubuf__ T* dstUb, __ubuf__ T* srcUb, const uint32_t* dstShape)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    constexpr uint32_t VF_LEN_HALF = GetVecLen() / 2 / sizeof(T);
    constexpr uint32_t oneBlockElementNum = GetDataBlockSizeInBytes() / sizeof(T);
    using GatherIndexType = typename ExtractSignedTypeBySize<sizeof(T)>::T;
    using BrcIndexType = typename ExtractIndexTypeBySize<sizeof(T)>::T;

    uint16_t sizeI[2];
    sizeI[0] = static_cast<uint16_t>(dstShape[0]);
    sizeI[1] = static_cast<uint16_t>(dstShape[1]);

    if (sizeI[1] == oneBlockElementNum && sizeof(T) != sizeof(uint8_t)) {
        BrcLastE2B(dstUb, srcUb, sizeI[0], sizeI[1]);
    } else if (sizeI[1] < VF_LEN_HALF) {
        LocalTensor<T> indexLocal;
        PopStackBuffer<T, TPosition::LCM>(indexLocal);
        __ubuf__ GatherIndexType* indexUb1 = (__ubuf__ GatherIndexType*)indexLocal.GetPhyAddr();
        __ubuf__ GatherIndexType* indexUb2 = (__ubuf__ GatherIndexType*)indexLocal.GetPhyAddr(VF_LEN);
        GenLastGatherIndex<GatherIndexType>(indexUb1, sizeI[1], 0);
        if constexpr (sizeof(T) == sizeof(uint8_t)) {
            GenLastGatherIndex<GatherIndexType>(indexUb2, sizeI[1], VF_LEN_HALF);
        }
        __ubuf__ BrcIndexType* indexUb = (__ubuf__ BrcIndexType*)indexLocal.GetPhyAddr();
        if (sizeI[0] * sizeI[1] < VF_LEN) {
            BrcLastGatherOne<T, BrcIndexType>(dstUb, srcUb, indexUb, sizeI[0], sizeI[1]);
        } else if (sizeI[1] < VF_LEN) {
            BrcLastGatherTwo<T, BrcIndexType>(dstUb, srcUb, indexUb, sizeI[0], sizeI[1]);
        }
    } else if (sizeI[1] <= VF_LEN) {
        BrcLastLessThanVLUnaligned<T>(dstUb, srcUb, sizeI[0], sizeI[1]);
    } else {
        if (sizeI[1] % oneBlockElementNum == 0) {
            BrcLastLargerThanVLAligned<T>(dstUb, srcUb, sizeI[0], sizeI[1]);
        } else {
            if constexpr (constRank == -1) {
                return true;
            } else {
                BrcLastLargerThanVLUnaligned<T>(dstUb, srcUb, sizeI[0], sizeI[1]);
            }
        }
    }
    return false;
}

template <typename T, int32_t constRank = -1>
__aicore__ inline bool BrcLastWrapperForThreeDim(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, const uint32_t* dstShape, const uint32_t* srcStride)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    constexpr uint32_t VF_LEN_HALF = GetVecLen() / 2 / sizeof(T);
    constexpr uint32_t oneBlockElementNum = GetDataBlockSizeInBytes() / sizeof(T);
    uint16_t sizeI[3];
    uint16_t stride[3];
    sizeI[0] = static_cast<uint16_t>(dstShape[0]);
    sizeI[1] = static_cast<uint16_t>(dstShape[1]);
    sizeI[2] = static_cast<uint16_t>(dstShape[2]);
    stride[0] = static_cast<uint16_t>(srcStride[0]);
    stride[1] = static_cast<uint16_t>(srcStride[1]);
    stride[2] = static_cast<uint16_t>(srcStride[2]);

    if (sizeI[2] == oneBlockElementNum && sizeof(T) != sizeof(uint8_t) && sizeI[1] * sizeI[2] > VF_LEN_HALF &&
        sizeI[1] % DEFAULT_BLK_NUM == 0 && stride[1] != 0) {
        if (sizeI[1] * sizeI[2] > VF_LEN) {
            BrcLastE2BLargerThanVL(dstUb, srcUb, sizeI[0], sizeI[1], sizeI[2], stride[0]);
        } else {
            BrcLastE2BLessThanVL(dstUb, srcUb, sizeI[0], sizeI[1], sizeI[2], stride[0]);
        }
    } else if (sizeI[2] < VF_LEN_HALF) {
        GatherWrapper(dstUb, srcUb, dstShape, srcStride);
    } else if (sizeI[2] <= VF_LEN) {
        if (sizeI[2] % oneBlockElementNum == 0) {
            BrcLastLessThanVLAligned<T>(dstUb, srcUb, sizeI[0], sizeI[1], sizeI[2], stride[0], stride[1]);
        } else {
            if constexpr (constRank == -1) {
                return true;
            } else {
                BrcLastLessThanVLUnaligned<T>(dstUb, srcUb, sizeI[0], sizeI[1], sizeI[2], stride[0], stride[1]);
            }
        }
    } else {
        if (sizeI[2] % oneBlockElementNum == 0) {
            BrcLastLargerThanVLAligned<T>(dstUb, srcUb, sizeI[0], sizeI[1], sizeI[2], stride[0], stride[1]);
        } else {
            if constexpr (constRank == -1) {
                return true;
            } else {
                BrcLastLargerThanVLUnaligned<T>(dstUb, srcUb, sizeI[0], sizeI[1], sizeI[2], stride[0], stride[1]);
            }
        }
    }
    return false;
}

template <typename T, int32_t constRank = -1>
__aicore__ inline bool BrcLastWrapperForFourDim(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, const uint32_t* dstShape, const uint32_t* srcStride)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    constexpr uint32_t VF_LEN_HALF = GetVecLen() / 2 / sizeof(T);
    constexpr uint32_t oneBlockElementNum = GetDataBlockSizeInBytes() / sizeof(T);
    uint16_t sizeI[4];
    uint16_t stride[4];
    sizeI[0] = static_cast<uint16_t>(dstShape[0]);
    sizeI[1] = static_cast<uint16_t>(dstShape[1]);
    sizeI[2] = static_cast<uint16_t>(dstShape[2]);
    sizeI[3] = static_cast<uint16_t>(dstShape[3]);
    stride[0] = static_cast<uint16_t>(srcStride[0]);
    stride[1] = static_cast<uint16_t>(srcStride[1]);
    stride[2] = static_cast<uint16_t>(srcStride[2]);
    stride[3] = static_cast<uint16_t>(srcStride[3]);

    if (sizeI[3] == oneBlockElementNum && sizeof(T) != sizeof(uint8_t) && stride[2] != 0 &&
        sizeI[2] % DEFAULT_BLK_NUM == 0) {
        BrcLastE2B(dstUb, srcUb, sizeI[0], sizeI[1], sizeI[2], sizeI[3], stride[0], stride[1]);
    } else if (sizeI[3] < VF_LEN_HALF && sizeof(T) != sizeof(uint8_t)) {
        GatherWrapperForFourDim(dstUb, srcUb, dstShape, srcStride);
    } else if (sizeI[3] <= VF_LEN) {
        if (sizeI[3] % oneBlockElementNum == 0) {
            BrcLastLessThanVLAligned<T>(
                dstUb, srcUb, sizeI[0], sizeI[1], sizeI[2], sizeI[3], stride[0], stride[1], stride[2]);
        } else {
            if constexpr (constRank == -1) {
                return true;
            } else {
                BrcLastLessThanVLUnaligned<T>(
                    dstUb, srcUb, sizeI[0], sizeI[1], sizeI[2], sizeI[3], stride[0], stride[1], stride[2]);
            }
        }
    } else {
        if (sizeI[3] % oneBlockElementNum == 0) {
            BrcLastLargerThanVLAligned<T>(
                dstUb, srcUb, sizeI[0], sizeI[1], sizeI[2], sizeI[3], stride[0], stride[1], stride[2]);
        } else {
            if constexpr (constRank == -1) {
                return true;
            } else {
                BrcLastLargerThanVLUnaligned<T>(
                    dstUb, srcUb, sizeI[0], sizeI[1], sizeI[2], sizeI[3], stride[0], stride[1], stride[2]);
            }
        }
    }
    return false;
}

template <typename T, int32_t constRank = -1>
__aicore__ inline bool BrcNlastWrapperForTwoDim(__ubuf__ T* dstUb, __ubuf__ T* srcUb, const uint32_t* dstShape)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    constexpr uint32_t VF_LEN_HALF = GetVecLen() / 2 / sizeof(T);
    constexpr uint32_t oneBlockElementNum = GetDataBlockSizeInBytes() / sizeof(T);
    using GatherIndexType = typename ExtractSignedTypeBySize<sizeof(T)>::T;
    using BrcIndexType = typename ExtractIndexTypeBySize<sizeof(T)>::T;
    uint16_t sizeI[2];
    sizeI[0] = static_cast<uint16_t>(dstShape[0]);
    sizeI[1] = static_cast<uint16_t>(dstShape[1]);

    if (sizeI[1] < VF_LEN_HALF) {
        LocalTensor<T> indexLocal;
        PopStackBuffer<T, TPosition::LCM>(indexLocal);
        if (sizeI[1] % oneBlockElementNum == 0) {
            event_t eventIdVToS = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_S));
            SetFlag<HardEvent::V_S>(eventIdVToS);
            WaitFlag<HardEvent::V_S>(eventIdVToS);
            __ubuf__ uint32_t* indexUb = (__ubuf__ uint32_t*)indexLocal.GetPhyAddr();
            if (sizeI[1] / oneBlockElementNum == 1) {
                indexUb[0] = 0;
                indexUb[1] = 0;
                indexUb[2] = 0;
                indexUb[3] = 0;
                indexUb[4] = 0;
                indexUb[5] = 0;
                indexUb[6] = 0;
                indexUb[7] = 0;
            } else if (sizeI[1] / oneBlockElementNum == 2) {
                indexUb[0] = 0;
                indexUb[1] = 32;
                indexUb[2] = 0;
                indexUb[3] = 32;
                indexUb[4] = 0;
                indexUb[5] = 32;
                indexUb[6] = 0;
                indexUb[7] = 32;
            } else {
                indexUb[0] = 0;
                indexUb[1] = 32;
                indexUb[2] = 64;
                indexUb[3] = 0;
                indexUb[4] = 32;
                indexUb[5] = 64;
                indexUb[6] = 0;
                indexUb[7] = 0;
            }
            event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
            SetFlag<HardEvent::S_V>(eventIdSToV);
            WaitFlag<HardEvent::S_V>(eventIdSToV);
            if (sizeI[0] * sizeI[1] < VF_LEN) {
                BrcNlastGatherBOne<T>(dstUb, srcUb, (__ubuf__ uint32_t*)indexUb, sizeI[0], sizeI[1]);
            } else if (sizeI[1] < VF_LEN) {
                BrcNlastGatherBTwo<T>(dstUb, srcUb, (__ubuf__ uint32_t*)indexUb, sizeI[0], sizeI[1]);
            }
        } else {
            __ubuf__ GatherIndexType* indexUb1 = (__ubuf__ GatherIndexType*)indexLocal.GetPhyAddr();
            __ubuf__ GatherIndexType* indexUb2 = (__ubuf__ GatherIndexType*)indexLocal.GetPhyAddr(VF_LEN);
            GenNlastGatherIndex<GatherIndexType>(indexUb1, sizeI[1], 0);
            if constexpr (sizeof(T) == sizeof(uint8_t)) {
                GenNlastGatherIndex<GatherIndexType>(indexUb2, sizeI[1], VF_LEN_HALF);
            }
            __ubuf__ BrcIndexType* indexUb = (__ubuf__ BrcIndexType*)indexLocal.GetPhyAddr();
            if (sizeI[0] * sizeI[1] < VF_LEN) {
                BrcNlastGatherOne<T, BrcIndexType>(dstUb, srcUb, indexUb, sizeI[0], sizeI[1]);
            } else if (sizeI[1] < VF_LEN) {
                BrcNlastGatherTwo<T, BrcIndexType>(dstUb, srcUb, indexUb, sizeI[0], sizeI[1]);
            }
        }
    } else if (sizeI[1] <= VF_LEN) {
        BrcNlastLessThanVLUnaligned<T>(dstUb, srcUb, sizeI[0], sizeI[1]);
    } else {
        if (sizeI[1] % oneBlockElementNum == 0) {
            if (sizeI[1] % VF_LEN == 0 && sizeI[0] > DEFAULT_BLK_NUM) {
                BrcNlastLargerThanVLAlignedWithVL<T>(dstUb, srcUb, sizeI[0], sizeI[1]);
            } else {
                BrcNlastLargerThanVLAlignedWithBlock<T>(dstUb, srcUb, sizeI[0], sizeI[1]);
            }
        } else {
            if constexpr (constRank == -1) {
                return true;
            } else {
                BrcNlastLargerThanVLUnaligned<T>(dstUb, srcUb, sizeI[0], sizeI[1]);
            }
        }
    }
    return false;
}

template <typename T, int32_t constRank = -1>
__aicore__ inline bool BrcNlastWrapperForThreeDim(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, const uint32_t* dstShape, const uint32_t* srcStride)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    constexpr uint32_t VF_LEN_HALF = GetVecLen() / 2 / sizeof(T);
    constexpr uint32_t oneBlockElementNum = GetDataBlockSizeInBytes() / sizeof(T);
    uint16_t sizeI[3];
    uint16_t stride[3];
    sizeI[0] = static_cast<uint16_t>(dstShape[0]);
    sizeI[1] = static_cast<uint16_t>(dstShape[1]);
    sizeI[2] = static_cast<uint16_t>(dstShape[2]);
    stride[0] = static_cast<uint16_t>(srcStride[0]);
    stride[1] = static_cast<uint16_t>(srcStride[1]);
    stride[2] = static_cast<uint16_t>(srcStride[2]);

    if (sizeI[2] < VF_LEN_HALF) {
        GatherWrapper(dstUb, srcUb, dstShape, srcStride);
    } else if (sizeI[2] <= VF_LEN) {
        BrcNlastLessThanVLUnaligned<T>(dstUb, srcUb, sizeI[0], sizeI[1], sizeI[2], stride[0], stride[1]);
    } else {
        if (sizeI[2] % oneBlockElementNum == 0) {
            if (sizeI[2] % VF_LEN == 0) {
                BrcNlastLargerThanVLAlignedWithVL<T>(dstUb, srcUb, sizeI[0], sizeI[1], sizeI[2], stride[0], stride[1]);
            } else {
                BrcNlastLargerThanVLAlignedWithBlock<T>(
                    dstUb, srcUb, sizeI[0], sizeI[1], sizeI[2], stride[0], stride[1]);
            }
        } else {
            if constexpr (constRank == -1) {
                return true;
            } else {
                BrcNlastLargerThanVLUnaligned<T>(dstUb, srcUb, sizeI[0], sizeI[1], sizeI[2], stride[0], stride[1]);
            }
        }
    }
    return false;
}

template <typename T, int32_t constRank = -1>
__aicore__ inline bool BrcNlastWrapperForFourDim(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, const uint32_t* dstShape, const uint32_t* srcStride)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    constexpr uint32_t VF_LEN_HALF = GetVecLen() / 2 / sizeof(T);
    constexpr uint32_t oneBlockElementNum = GetDataBlockSizeInBytes() / sizeof(T);
    uint16_t sizeI[4];
    uint16_t stride[4];
    sizeI[0] = static_cast<uint16_t>(dstShape[0]);
    sizeI[1] = static_cast<uint16_t>(dstShape[1]);
    sizeI[2] = static_cast<uint16_t>(dstShape[2]);
    sizeI[3] = static_cast<uint16_t>(dstShape[3]);
    stride[0] = static_cast<uint16_t>(srcStride[0]);
    stride[1] = static_cast<uint16_t>(srcStride[1]);
    stride[2] = static_cast<uint16_t>(srcStride[2]);
    stride[3] = static_cast<uint16_t>(srcStride[3]);

    if (sizeI[3] < VF_LEN_HALF && sizeof(T) != sizeof(uint8_t)) {
        GatherWrapperForFourDim(dstUb, srcUb, dstShape, srcStride);
    } else if (sizeI[3] <= VF_LEN) {
        if (sizeI[3] % oneBlockElementNum == 0) {
            BrcNlastLessThanVLAligned<T>(
                dstUb, srcUb, sizeI[0], sizeI[1], sizeI[2], sizeI[3], stride[0], stride[1], stride[2]);
        } else {
            if constexpr (constRank == -1) {
                return true;
            } else {
                BrcNlastLessThanVLUnaligned<T>(
                    dstUb, srcUb, sizeI[0], sizeI[1], sizeI[2], sizeI[3], stride[0], stride[1], stride[2]);
            }
        }
    } else {
        if (sizeI[3] % oneBlockElementNum == 0) {
            if (sizeI[3] % VF_LEN == 0) {
                BrcNlastLargerThanVLAlignedWithVL<T>(
                    dstUb, srcUb, sizeI[0], sizeI[1], sizeI[2], sizeI[3], stride[0], stride[1], stride[2]);
            } else {
                BrcNlastLargerThanVLAlignedWithBlock<T>(
                    dstUb, srcUb, sizeI[0], sizeI[1], sizeI[2], sizeI[3], stride[0], stride[1], stride[2]);
            }
        } else {
            if constexpr (constRank == -1) {
                return true;
            } else {
                BrcNlastLargerThanVLUnaligned<T>(
                    dstUb, srcUb, sizeI[0], sizeI[1], sizeI[2], sizeI[3], stride[0], stride[1], stride[2]);
            }
        }
    }
    return false;
}

template <typename T>
__aicore__ inline void BrcNlastWrapperForMoreDim(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, const uint32_t* dstShape, const uint32_t* dstStride,
    const uint32_t* srcStride)
{
    __ubuf__ T* srcUbTmp = srcUb;
    __ubuf__ T* dstUbTmp = dstUb;
    for (uint16_t p = 0; p < static_cast<uint16_t>(dstShape[0]); ++p) {
        dstUb = dstUbTmp + p * dstStride[0];
        srcUb = srcUbTmp + p * srcStride[0];
        GatherWrapperForFourDim(dstUb, srcUb, dstShape + 1, srcStride + 1);
    }
}

template <typename T, bool isNlast>
__aicore__ inline void BrcWrapperForMoreDimDynamicShapeImpl(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, const uint32_t dim, const uint32_t* dstShape, const uint32_t* dstStride,
    const uint32_t* srcStride)
{
    constexpr uint16_t VF_LEN = GetVecLen() / sizeof(T);
    constexpr uint32_t oneBlockElementNum = GetDataBlockSizeInBytes() / sizeof(T);
    uint16_t sizeI[4] = {1, 1, 1, 1};
    if (dim > 4) {
        sizeI[0] = dstShape[dim - 4];
        sizeI[1] = dstShape[dim - 3];
        sizeI[2] = dstShape[dim - 2];
        sizeI[3] = dstShape[dim - 1];
    } else {
        for (uint16_t i = 0; i < dim; ++i) {
            sizeI[4 - dim + i] = dstShape[i];
        }
    }
    uint32_t totalDim = 9;
    uint16_t loops[5] = {1, 1, 1, 1, 1};
    for (int16_t i = dim - 5, j = 4; i >= 0; --i, --j) {
        loops[j] = static_cast<uint16_t>(dstShape[i]);
    }
    uint16_t stride[4] = {0, 0, 0, 0};
    if (dim > 4) {
        stride[0] = srcStride[dim - 4];
        stride[1] = srcStride[dim - 3];
        stride[2] = srcStride[dim - 2];
        stride[3] = srcStride[dim - 1];
    } else {
        for (uint16_t i = 0; i < dim; ++i) {
            stride[4 - dim + i] = srcStride[i];
        }
    }
    __ubuf__ T* srcUbTmp = srcUb;
    __ubuf__ T* dstUbTmp = dstUb;
    for (uint16_t i = 0; i < loops[0]; ++i) {
        for (uint16_t j = 0; j < loops[1]; ++j) {
            for (uint16_t k = 0; k < loops[2]; ++k) {
                for (uint16_t t = 0; t < loops[3]; ++t) {
                    for (uint16_t p = 0; p < loops[4]; ++p) {
                        dstUb = dstUbTmp + p * dstStride[(dim - 5 + totalDim) % totalDim] +
                                t * dstStride[(dim - 6 + totalDim) % totalDim] +
                                k * dstStride[(dim - 7 + totalDim) % totalDim] +
                                j * dstStride[(dim - 8 + totalDim) % totalDim] +
                                i * dstStride[(dim - 9 + totalDim) % totalDim];
                        srcUb = srcUbTmp + p * srcStride[(dim - 5 + totalDim) % totalDim] +
                                t * srcStride[(dim - 6 + totalDim) % totalDim] +
                                k * srcStride[(dim - 7 + totalDim) % totalDim] +
                                j * srcStride[(dim - 8 + totalDim) % totalDim] +
                                i * srcStride[(dim - 9 + totalDim) % totalDim];
                        if constexpr (isNlast) {
                            constexpr uint16_t VF_LEN_HALF = GetVecLen() / 2 / sizeof(T);
                            if (sizeI[3] < VF_LEN_HALF && sizeof(T) != sizeof(uint8_t)) {
                                GatherWrapperForFourDim(dstUb, srcUb, dstShape + dim - 4, srcStride + dim - 4);
                            } else if (sizeI[3] <= VF_LEN) {
                                if (sizeI[3] % oneBlockElementNum == 0) {
                                    BrcNlastLessThanVLAligned<T>(
                                        dstUb, srcUb, sizeI[0], sizeI[1], sizeI[2], sizeI[3], stride[0], stride[1],
                                        stride[2]);
                                } else {
                                    BrcNlastLessThanVLUnaligned<T>(
                                        dstUb, srcUb, sizeI[0], sizeI[1], sizeI[2], sizeI[3], stride[0], stride[1],
                                        stride[2]);
                                }
                            } else {
                                if (sizeI[3] % oneBlockElementNum == 0) {
                                    BrcNlastLargerThanVLAlignedWithBlock<T>(
                                        dstUb, srcUb, sizeI[0], sizeI[1], sizeI[2], sizeI[3], stride[0], stride[1],
                                        stride[2]);
                                } else {
                                    BrcNlastLargerThanVLUnaligned<T>(
                                        dstUb, srcUb, sizeI[0], sizeI[1], sizeI[2], sizeI[3], stride[0], stride[1],
                                        stride[2]);
                                }
                            }
                        } else {
                            if (sizeI[3] <= VF_LEN) {
                                BrcLastLessThanVLUnaligned<T>(
                                    dstUb, srcUb, sizeI[0], sizeI[1], sizeI[2], sizeI[3], stride[0], stride[1],
                                    stride[2]);
                            } else {
                                BrcLastLargerThanVLUnaligned<T>(
                                    dstUb, srcUb, sizeI[0], sizeI[1], sizeI[2], sizeI[3], stride[0], stride[1],
                                    stride[2]);
                            }
                        }
                    }
                }
            }
        }
    }
}

template <typename T>
__aicore__ inline void BrcNlastWrapperForMoreDimDynamicShape(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, const uint32_t dim, const uint32_t* dstShape, const uint32_t* dstStride,
    const uint32_t* srcStride)
{
    BrcWrapperForMoreDimDynamicShapeImpl<T, true>(dstUb, srcUb, dim, dstShape, dstStride, srcStride);
}

template <typename T>
__aicore__ inline void BrcLastWrapperForMoreDimDynamicShape(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, const uint32_t dim, const uint32_t* dstShape, const uint32_t* dstStride,
    const uint32_t* srcStride)
{
    BrcWrapperForMoreDimDynamicShapeImpl<T, false>(dstUb, srcUb, dim, dstShape, dstStride, srcStride);
}
} // namespace BroadcastInternal
} // namespace AscendC
#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PAD_BROADCAST_BROADCAST_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PAD_BROADCAST_BROADCAST_C310_IMPL_H__
#endif
