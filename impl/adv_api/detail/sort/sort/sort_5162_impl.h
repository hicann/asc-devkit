/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file sort_5162_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/sort/sort/sort_5162_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/sort/sort.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SORT_SORT_SORT_5162_IMPL_H__
#endif

#ifndef IMPL_SORT_SORT_SORT_5162_IMPL_H
#define IMPL_SORT_SORT_SORT_5162_IMPL_H

#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../sort_common_utils.h"

namespace AscendC {
namespace Reg {
namespace internal {
__simd_callee__ inline void ConvertRegToWithShift(
    __ubuf__ uint32_t*& input, RegTensor<uint8_t>& dst, const int16_t offset)
{
    constexpr uint32_t eleCountPerVL = GetVecLen() / sizeof(uint32_t);
    MaskReg fullMask = CreateMask<uint32_t>();
    RegTensor<uint32_t> inputP0, inputP1, inputP2, inputP3;
    Reg::LoadAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(inputP0, input, eleCountPerVL);
    Reg::LoadAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(inputP1, input, eleCountPerVL);
    Reg::LoadAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(inputP2, input, eleCountPerVL);
    Reg::LoadAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(inputP3, input, eleCountPerVL);

    ShiftRights(inputP0, inputP0, offset, fullMask);
    ShiftRights(inputP1, inputP1, offset, fullMask);
    ShiftRights(inputP2, inputP2, offset, fullMask);
    ShiftRights(inputP3, inputP3, offset, fullMask);

    RegTensor<uint16_t> tmpU16Reg0, tmpU16Reg1, tmpU16Reg2;

    DeInterleave(tmpU16Reg0, tmpU16Reg1, (RegTensor<uint16_t>&)inputP0, (RegTensor<uint16_t>&)inputP1);
    DeInterleave(tmpU16Reg2, tmpU16Reg1, (RegTensor<uint16_t>&)inputP2, (RegTensor<uint16_t>&)inputP3);

    RegTensor<uint8_t> tmpReg;
    DeInterleave(dst, tmpReg, (RegTensor<uint8_t>&)tmpU16Reg0, (RegTensor<uint8_t>&)tmpU16Reg2);
}

__simd_callee__ inline void ConvertRegToWithShift(
    __ubuf__ uint64_t*& input, RegTensor<uint8_t>& dst, const int16_t offset)
{
    constexpr uint32_t eleCountPerVL = GetVecLen() / sizeof(uint64_t);
    MaskReg fullMask = CreateMask<uint64_t>();
    RegTensor<uint64_t> inputP0, inputP1, inputP2, inputP3, inputP4, inputP5, inputP6, inputP7;
    Reg::LoadAlign<uint64_t, PostLiteral::POST_MODE_UPDATE>(inputP0, input, eleCountPerVL);
    Reg::LoadAlign<uint64_t, PostLiteral::POST_MODE_UPDATE>(inputP1, input, eleCountPerVL);
    Reg::LoadAlign<uint64_t, PostLiteral::POST_MODE_UPDATE>(inputP2, input, eleCountPerVL);
    Reg::LoadAlign<uint64_t, PostLiteral::POST_MODE_UPDATE>(inputP3, input, eleCountPerVL);
    Reg::LoadAlign<uint64_t, PostLiteral::POST_MODE_UPDATE>(inputP4, input, eleCountPerVL);
    Reg::LoadAlign<uint64_t, PostLiteral::POST_MODE_UPDATE>(inputP5, input, eleCountPerVL);
    Reg::LoadAlign<uint64_t, PostLiteral::POST_MODE_UPDATE>(inputP6, input, eleCountPerVL);
    Reg::LoadAlign<uint64_t, PostLiteral::POST_MODE_UPDATE>(inputP7, input, eleCountPerVL);

    ShiftRights(inputP0, inputP0, offset, fullMask);
    ShiftRights(inputP1, inputP1, offset, fullMask);
    ShiftRights(inputP2, inputP2, offset, fullMask);
    ShiftRights(inputP3, inputP3, offset, fullMask);
    ShiftRights(inputP4, inputP4, offset, fullMask);
    ShiftRights(inputP5, inputP5, offset, fullMask);
    ShiftRights(inputP6, inputP6, offset, fullMask);
    ShiftRights(inputP7, inputP7, offset, fullMask);

    RegTensor<uint32_t> tmpU32Reg0, tmpU32Reg1, tmpU32Reg2, tmpU32Reg3, tmpU32Reg4;
    DeInterleave(tmpU32Reg0, tmpU32Reg4, (RegTensor<uint32_t>&)inputP0, (RegTensor<uint32_t>&)inputP1);
    DeInterleave(tmpU32Reg1, tmpU32Reg4, (RegTensor<uint32_t>&)inputP2, (RegTensor<uint32_t>&)inputP3);
    DeInterleave(tmpU32Reg2, tmpU32Reg4, (RegTensor<uint32_t>&)inputP4, (RegTensor<uint32_t>&)inputP5);
    DeInterleave(tmpU32Reg3, tmpU32Reg4, (RegTensor<uint32_t>&)inputP6, (RegTensor<uint32_t>&)inputP7);

    RegTensor<uint16_t> tmpU16Reg0, tmpU16Reg1, tmpU16Reg2;

    DeInterleave(tmpU16Reg0, tmpU16Reg1, (RegTensor<uint16_t>&)tmpU32Reg0, (RegTensor<uint16_t>&)tmpU32Reg1);
    DeInterleave(tmpU16Reg2, tmpU16Reg1, (RegTensor<uint16_t>&)tmpU32Reg2, (RegTensor<uint16_t>&)tmpU32Reg3);

    RegTensor<uint8_t> tmpReg;
    DeInterleave(dst, tmpReg, (RegTensor<uint8_t>&)tmpU16Reg0, (RegTensor<uint8_t>&)tmpU16Reg2);
}

__simd_callee__ inline void ConvertRegToWithShift(
    __ubuf__ uint16_t*& input, RegTensor<uint8_t>& dst, const int16_t offset)
{
    constexpr uint32_t eleCountPerVL = GetVecLen() / sizeof(uint16_t);
    MaskReg fullMask = CreateMask<uint16_t>();
    RegTensor<uint16_t> inputP0, inputP1;
    Reg::LoadAlign<uint16_t, PostLiteral::POST_MODE_UPDATE>(inputP0, input, eleCountPerVL);
    Reg::LoadAlign<uint16_t, PostLiteral::POST_MODE_UPDATE>(inputP1, input, eleCountPerVL);

    ShiftRights(inputP0, inputP0, offset, fullMask);
    ShiftRights(inputP1, inputP1, offset, fullMask);

    RegTensor<uint8_t> tmpReg;
    DeInterleave(dst, tmpReg, (RegTensor<uint8_t>&)inputP0, (RegTensor<uint8_t>&)inputP1);
}

__simd_callee__ inline void ConvertRegToWithShift(
    __ubuf__ uint8_t*& input, RegTensor<uint8_t>& dst, const int16_t offset)
{
    constexpr uint32_t eleCountPerVL = GetVecLen();
    Reg::LoadAlign<uint8_t, PostLiteral::POST_MODE_UPDATE>(dst, input, eleCountPerVL);
}

template <uint32_t K, uint32_t N>
__simd_callee__ inline void HistZeroCumAll(RegTensor<uint16_t> (&cum)[N], MaskReg& fullMaskB16)
{
    if constexpr (K < N) {
        Duplicate(cum[K], 0, fullMaskB16);
        HistZeroCumAll<K + 1, N>(cum, fullMaskB16);
    }
}

template <uint32_t K, uint32_t N>
__simd_callee__ inline void HistAccumAll(RegTensor<uint16_t> (&cum)[N], RegTensor<uint8_t>& key, MaskReg& maskRegB8)
{
    if constexpr (K < N) {
        Histograms<uint8_t, uint16_t, static_cast<HistogramsBinType>(K), HistogramsType::ACCUMULATE>(
            cum[K], key, maskRegB8);
        HistAccumAll<K + 1, N>(cum, key, maskRegB8);
    }
}

template <uint32_t K, uint32_t N>
__simd_callee__ inline void HistStoreAll(
    RegTensor<uint16_t> (&cum)[N], __ubuf__ uint16_t*& histPtr, MaskReg& fullMaskB16)
{
    if constexpr (K < N) {
        Reg::StoreAlign<uint16_t, PostLiteral::POST_MODE_UPDATE>(
            histPtr, cum[K], GetVecLen() / sizeof(uint16_t), fullMaskB16);
        HistStoreAll<K + 1, N>(cum, histPtr, fullMaskB16);
    }
}

constexpr CastTrait kU8ToU16PartZero = {
    RegLayout::ZERO, SatMode::UNKNOWN, MaskMergeMode::ZEROING, RoundMode::CAST_NONE};
constexpr CastTrait kU8ToU16PartOne = {RegLayout::ONE, SatMode::UNKNOWN, MaskMergeMode::ZEROING, RoundMode::CAST_NONE};

__simd_callee__ inline void GatherBinTableU16(
    RegTensor<uint16_t>& out0, RegTensor<uint16_t>& out1, __ubuf__ uint16_t* tableBase, RegTensor<uint16_t>& evenKeys,
    RegTensor<uint16_t>& oddKeys, MaskReg& fullMaskB16)
{
    RegTensor<uint16_t> evenRes, oddRes;
    Reg::Gather(evenRes, tableBase, evenKeys, fullMaskB16);
    Reg::Gather(oddRes, tableBase, oddKeys, fullMaskB16);
    // evenKeys/oddKeys hold the even/odd source lanes (Cast RegLayout ZERO/ONE), so Interleave (vintlv)
    // zips the two halves back into natural element order: out0 lane p = table[key[p]], out1 lane p =
    // table[key[u16PerVL + p]]. The caller must pair these with the natural-order position vectors.
    Interleave(out0, out1, evenRes, oddRes);
}

__aicore__ constexpr uint32_t Log2Pow2(uint32_t v)
{
    uint32_t r = 0;
    while (v > 1) {
        v >>= 1;
        ++r;
    }
    return r;
}

template <uint32_t K, uint32_t N>
__simd_callee__ inline void HistWindowFoldAll(
    __ubuf__ uint16_t* bucketBase, RegTensor<uint8_t>& key, RegTensor<uint16_t>& evenHi, RegTensor<uint16_t>& oddHi,
    RegTensor<uint16_t>& evenKey, RegTensor<uint16_t>& oddKey, RegTensor<uint16_t>& accEven,
    RegTensor<uint16_t>& accOdd, MaskReg& maskRegB8, MaskReg& fullMaskB16)
{
    if constexpr (K < N) {
        constexpr uint32_t u16PerVL = GetVecLen() / sizeof(uint16_t);
        RegTensor<uint16_t> dist, cum, excl, bucket;
        Duplicate(dist, 0, fullMaskB16);
        Duplicate(cum, 0, fullMaskB16);
        Histograms<uint8_t, uint16_t, static_cast<HistogramsBinType>(K), HistogramsType::FREQUENCY>(
            dist, key, maskRegB8);
        Histograms<uint8_t, uint16_t, static_cast<HistogramsBinType>(K), HistogramsType::ACCUMULATE>(
            cum, key, maskRegB8);
        Sub(excl, cum, dist, fullMaskB16);
        __ubuf__ uint16_t* bucketPtr = bucketBase + K * u16PerVL;
        Reg::LoadAlign(bucket, bucketPtr);
        Add(bucket, bucket, dist, fullMaskB16);
        Reg::StoreAlign(bucketPtr, bucket, fullMaskB16);
        // Fold the per-block local offset into registers: select the excl slice lane-wise (vselr)
        // instead of spilling the block prefix table to UB. vselr keeps only the low log2(u16PerVL)
        // index bits, which is exactly the in-bin lane of the key, so the key registers index directly.
        RegTensor<uint16_t> gEven, gOdd;
        MaskReg selMask;
        Reg::Gather(gEven, excl, evenKey);
        Reg::Gather(gOdd, excl, oddKey);
        Reg::CompareScalar<uint16_t, CMPMODE::EQ>(selMask, evenHi, static_cast<int16_t>(K), fullMaskB16);
        Reg::Select(accEven, gEven, accEven, selMask);
        Reg::CompareScalar<uint16_t, CMPMODE::EQ>(selMask, oddHi, static_cast<int16_t>(K), fullMaskB16);
        Reg::Select(accOdd, gOdd, accOdd, selMask);
        HistWindowFoldAll<K + 1, N>(
            bucketBase, key, evenHi, oddHi, evenKey, oddKey, accEven, accOdd, maskRegB8, fullMaskB16);
    }
}

__simd_vf__ inline void LocalSort(
    __ubuf__ uint8_t* srcU8, __ubuf__ uint16_t* tmpIndexU16, __ubuf__ uint16_t* exclusiveSumOrigin,
    __ubuf__ uint32_t* sortedLocalIndex, uint32_t count)
{
    uint16_t repeatTime = DivCeil(count, GetVecLen());
    constexpr uint32_t partNum = 512 / GetVecLen();
    constexpr uint32_t u16PerVL = GetVecLen() / sizeof(uint16_t);
    constexpr uint32_t binShift = Log2Pow2(u16PerVL);
    RegTensor<uint8_t> key;
    MaskReg fullMaskB16 = CreateMask<uint16_t>();
    MaskReg fullMaskB8 = CreateMask<uint8_t>();

    RegTensor<int16_t> bucketLocalIndex0, bucketLocalIndex1;
    Arange(bucketLocalIndex0, 0);
    Arange(bucketLocalIndex1, static_cast<int16_t>(GetVecLen() / 2));

    uint32_t currCount = count;
    uint32_t currCountU8 = count;
    for (uint16_t i = 0; i < repeatTime; i++) {
        MaskReg maskRegB8 = UpdateMask<uint8_t>(currCountU8);
        Reg::LoadAlign<uint8_t, PostLiteral::POST_MODE_UPDATE>(key, (__ubuf__ uint8_t*&)srcU8, GetVecLen());

        RegTensor<uint16_t> evenKeys, oddKeys;
        Cast<uint16_t, uint8_t, kU8ToU16PartZero>(evenKeys, key, maskRegB8);
        Cast<uint16_t, uint8_t, kU8ToU16PartOne>(oddKeys, key, maskRegB8);

        RegTensor<uint16_t> evenHi, oddHi;
        ShiftRights(evenHi, evenKeys, static_cast<int16_t>(binShift), fullMaskB16);
        ShiftRights(oddHi, oddKeys, static_cast<int16_t>(binShift), fullMaskB16);
        RegTensor<uint16_t> accEven, accOdd;
        Duplicate(accEven, 0, fullMaskB16);
        Duplicate(accOdd, 0, fullMaskB16);

        RegTensor<uint16_t> bucketOffsetKey0, bucketOffsetKey1;
        GatherBinTableU16(bucketOffsetKey0, bucketOffsetKey1, exclusiveSumOrigin, evenKeys, oddKeys, fullMaskB16);

        // The bucket table in UB is updated in place (G[v] += counts of the blocks already scanned).
        // It is scratch owned by this pass only: SortU8ElementsWithRound regenerates the full table
        // through GetExclusiveSum before every LocalSort call and nothing reads it afterwards.
        HistWindowFoldAll<0, partNum>(
            exclusiveSumOrigin, key, evenHi, oddHi, evenKeys, oddKeys, accEven, accOdd, maskRegB8, fullMaskB16);
        LocalMemBar<MemType::VEC_STORE, MemType::VEC_LOAD>();

        RegTensor<uint16_t> localOffsetKey0, localOffsetKey1;
        Interleave(localOffsetKey0, localOffsetKey1, accEven, accOdd);

        RegTensor<uint16_t> bucketRank0, bucketRank1;
        Add(bucketRank0, bucketOffsetKey0, (RegTensor<uint16_t>&)bucketLocalIndex0, fullMaskB16);
        Add(bucketRank1, bucketOffsetKey1, (RegTensor<uint16_t>&)bucketLocalIndex1, fullMaskB16);
        Sub(bucketRank0, bucketRank0, localOffsetKey0, fullMaskB16);
        Sub(bucketRank1, bucketRank1, localOffsetKey1, fullMaskB16);

        RegTensor<uint8_t> zeroReg;
        Duplicate(zeroReg, 0, fullMaskB8);
        RegTensor<uint32_t> rankP0, rankP1;
        Interleave(
            (RegTensor<uint16_t>&)rankP0, (RegTensor<uint16_t>&)rankP1, bucketRank0, (RegTensor<uint16_t>&)zeroReg);
        RegTensor<uint16_t> indexU16Bucket0;
        Reg::LoadAlign<uint16_t, PostLiteral::POST_MODE_UPDATE>(
            indexU16Bucket0, (__ubuf__ uint16_t*&)tmpIndexU16, GetVecLen() / sizeof(uint16_t));
        RegTensor<uint32_t> idxP0, idxP1;
        Interleave(
            (RegTensor<uint16_t>&)idxP0, (RegTensor<uint16_t>&)idxP1, indexU16Bucket0, (RegTensor<uint16_t>&)zeroReg);
        MaskReg u32Mask0 = UpdateMask<uint32_t>(currCount);
        Reg::Scatter(sortedLocalIndex, idxP0, rankP0, u32Mask0);
        MaskReg u32Mask1 = UpdateMask<uint32_t>(currCount);
        Reg::Scatter(sortedLocalIndex, idxP1, rankP1, u32Mask1);

        RegTensor<uint32_t> rankP2, rankP3;
        Interleave(
            (RegTensor<uint16_t>&)rankP2, (RegTensor<uint16_t>&)rankP3, bucketRank1, (RegTensor<uint16_t>&)zeroReg);
        RegTensor<uint16_t> indexU16Bucket1;
        Reg::LoadAlign<uint16_t, PostLiteral::POST_MODE_UPDATE>(
            indexU16Bucket1, (__ubuf__ uint16_t*&)tmpIndexU16, GetVecLen() / sizeof(uint16_t));
        RegTensor<uint32_t> idxP2, idxP3;
        Interleave(
            (RegTensor<uint16_t>&)idxP2, (RegTensor<uint16_t>&)idxP3, indexU16Bucket1, (RegTensor<uint16_t>&)zeroReg);
        MaskReg u32Mask2 = UpdateMask<uint32_t>(currCount);
        Reg::Scatter(sortedLocalIndex, idxP2, rankP2, u32Mask2);
        MaskReg u32Mask3 = UpdateMask<uint32_t>(currCount);
        Reg::Scatter(sortedLocalIndex, idxP3, rankP3, u32Mask3);
    }
}

template <uint32_t K, uint32_t N>
__simd_callee__ inline void HistExclShiftStoreAll(
    RegTensor<uint16_t> (&cum)[N], __ubuf__ uint16_t*& histPtr, RegTensor<uint16_t>& rotIdx,
    RegTensor<uint16_t>& carryIdx, RegTensor<uint16_t>& zeroReg, MaskReg& lane0Mask, MaskReg& fullMaskB16)
{
    if constexpr (K < N) {
        RegTensor<uint16_t> shifted, carry;
        Reg::Gather(shifted, cum[K], rotIdx);
        if constexpr (K == 0) {
            Reg::Select(shifted, zeroReg, shifted, lane0Mask);
        } else {
            Reg::Gather(carry, cum[K - 1], carryIdx);
            Reg::Select(shifted, carry, shifted, lane0Mask);
        }
        Reg::StoreAlign<uint16_t, PostLiteral::POST_MODE_UPDATE>(
            histPtr, shifted, GetVecLen() / sizeof(uint16_t), fullMaskB16);
        HistExclShiftStoreAll<K + 1, N>(cum, histPtr, rotIdx, carryIdx, zeroReg, lane0Mask, fullMaskB16);
    }
}

// Narrow-VL devices (5162 family) derive the exclusive prefix from the cumulative table alone,
// G[v] = C[v-1], so only one 16-entry table stays live (32-entry register file -> no spill).
template <typename T>
__simd_vf__ inline void GetExclusiveSum(
    __ubuf__ T* srcValue, __ubuf__ uint8_t* tmpSrc, __ubuf__ uint8_t* tmpSrcCopy, __ubuf__ uint16_t* exclusiveSum,
    const uint32_t count, const int32_t round)
{
    // Traverse all data to get a Exclusive Sum
    uint16_t repeatTime = DivCeil(count, GetVecLen());
    int32_t bitoffset = round * 8;
    constexpr uint32_t partNum = 512 / GetVecLen();
    constexpr uint32_t u16PerVL = GetVecLen() / sizeof(uint16_t);

    RegTensor<uint16_t> cumulativeHist[partNum];
    MaskReg b16FullMask = CreateMask<uint16_t>();
    HistZeroCumAll<0, partNum>(cumulativeHist, b16FullMask);

    uint32_t currCount = count;

    for (uint16_t i = 0; i < repeatTime; i++) {
        MaskReg maskReg = UpdateMask<uint8_t>(currCount);
        RegTensor<uint8_t> b8Reg;
        ConvertRegToWithShift(srcValue, b8Reg, bitoffset);

        Reg::StoreAlign<uint8_t, PostLiteral::POST_MODE_UPDATE>(tmpSrc, b8Reg, GetVecLen(), maskReg);
        Reg::StoreAlign<uint8_t, PostLiteral::POST_MODE_UPDATE>(tmpSrcCopy, b8Reg, GetVecLen(), maskReg);

        HistAccumAll<0, partNum>(cumulativeHist, b8Reg, maskReg);
    }

    __ubuf__ uint16_t* histPtr = exclusiveSum;
    RegTensor<int16_t> rotSeq, laneSeq;
    Arange(laneSeq, static_cast<int16_t>(0));
    Arange(rotSeq, static_cast<int16_t>(u16PerVL - 1));
    RegTensor<uint16_t> rotIdx = (RegTensor<uint16_t>&)rotSeq;
    RegTensor<uint16_t> laneIdx = (RegTensor<uint16_t>&)laneSeq;
    RegTensor<uint16_t> carryIdx, zeroReg;
    Duplicate(carryIdx, static_cast<int16_t>(u16PerVL - 1), b16FullMask);
    Duplicate(zeroReg, 0, b16FullMask);
    And(rotIdx, rotIdx, carryIdx, b16FullMask);
    MaskReg lane0Mask;
    CompareScalar<uint16_t, CMPMODE::EQ>(lane0Mask, laneIdx, static_cast<int16_t>(0), b16FullMask);
    HistExclShiftStoreAll<0, partNum>(cumulativeHist, histPtr, rotIdx, carryIdx, zeroReg, lane0Mask, b16FullMask);
}

} // namespace internal
} // namespace Reg
} // namespace AscendC
#endif // IMPL_SORT_SORT_SORT_5162_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SORT_SORT_SORT_5162_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SORT_SORT_SORT_5162_IMPL_H__
#endif
