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
 * \file sort_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/sort/sort/sort_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/sort/sort.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SORT_SORT_SORT_IMPL_H__
#endif

#ifndef IMPL_SORT_SORT_SORT_IMPL_H
#define IMPL_SORT_SORT_SORT_IMPL_H

#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../sort_common_utils.h"
#include "../../../../../include/adv_api/sort/sort_utils.h"

namespace AscendC {
constexpr auto singleSortElementCount = singleSortElementCountArch3510;

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

template <typename T>
__simd_vf__ inline void GetExclusiveSum(
    __ubuf__ T* srcValue, __ubuf__ uint8_t* tmpSrc, __ubuf__ uint8_t* tmpSrcCopy, __ubuf__ uint16_t* exclusiveSum,
    const uint32_t count, const int32_t round)
{
    // Traverse all data to get a Exclusive Sum
    uint16_t repeatTime = DivCeil(count, GetVecLen());
    int32_t bitoffset = round * 8;

    RegTensor<uint16_t> distributeHistP0;
    RegTensor<uint16_t> distributeHistP1;
    RegTensor<uint16_t> cumulativeHistP0;
    RegTensor<uint16_t> cumulativeHistP1;

    MaskReg b16FullMask = CreateMask<uint16_t>();

    Duplicate(distributeHistP0, 0, b16FullMask);
    Duplicate(distributeHistP1, 0, b16FullMask);
    Duplicate(cumulativeHistP0, 0, b16FullMask);
    Duplicate(cumulativeHistP1, 0, b16FullMask);

    uint32_t currCount = count;

    for (uint16_t i = 0; i < repeatTime; i++) {
        MaskReg maskReg = UpdateMask<uint8_t>(currCount);
        RegTensor<uint8_t> b8Reg;
        ConvertRegToWithShift(srcValue, b8Reg, bitoffset);

        Reg::StoreAlign<uint8_t, PostLiteral::POST_MODE_UPDATE>(tmpSrc, b8Reg, GetVecLen(), maskReg);
        Reg::StoreAlign<uint8_t, PostLiteral::POST_MODE_UPDATE>(tmpSrcCopy, b8Reg, GetVecLen(), maskReg);

        Histograms<uint8_t, uint16_t, HistogramsBinType::BIN0, HistogramsType::FREQUENCY>(
            distributeHistP0, b8Reg, maskReg);
        Histograms<uint8_t, uint16_t, HistogramsBinType::BIN1, HistogramsType::FREQUENCY>(
            distributeHistP1, b8Reg, maskReg);

        Histograms<uint8_t, uint16_t, HistogramsBinType::BIN0, HistogramsType::ACCUMULATE>(
            cumulativeHistP0, b8Reg, maskReg);
        Histograms<uint8_t, uint16_t, HistogramsBinType::BIN1, HistogramsType::ACCUMULATE>(
            cumulativeHistP1, b8Reg, maskReg);
    }

    RegTensor<uint16_t> exclusiveSumP0, exclusiveSumP1;
    Sub(exclusiveSumP0, cumulativeHistP0, distributeHistP0, b16FullMask);
    Sub(exclusiveSumP1, cumulativeHistP1, distributeHistP1, b16FullMask);

    Reg::StoreAlign<uint16_t, PostLiteral::POST_MODE_UPDATE>(
        exclusiveSum, exclusiveSumP0, GetVecLen() / sizeof(uint16_t), b16FullMask);
    Reg::StoreAlign<uint16_t, PostLiteral::POST_MODE_UPDATE>(
        exclusiveSum, exclusiveSumP1, GetVecLen() / sizeof(uint16_t), b16FullMask);
}

__simd_vf__ inline void EightBitsSort(
    __ubuf__ uint8_t* srcValueU8, __ubuf__ uint8_t* srcValueU8Back, __ubuf__ uint8_t* inputIndexAddr,
    __ubuf__ uint16_t* inputIndexU16Addr, uint32_t count)
{
    uint16_t repeatTime = DivCeil(count, GetVecLen());
    __ubuf__ uint8_t* srcAddr = srcValueU8;
    __ubuf__ uint8_t* srcAddrTmp = srcValueU8;
    __ubuf__ uint8_t* indexAddr = inputIndexAddr;
    __ubuf__ uint8_t* indexAddrTmp = inputIndexAddr;
    RegTensor<uint8_t> srcReg;
    RegTensor<uint8_t> indexReg;
    RegTensor<int8_t> shiftOffset;
    RegTensor<uint8_t> last2BitsMask;
    constexpr uint8_t last2BitMaskVal = 3;
    MaskReg fullMaskB8 = CreateMask<uint8_t>();

    RegTensor<int8_t> bucketIndex;
    Arange(bucketIndex, 0);

    __ubuf__ uint8_t* outputIndex = inputIndexAddr;
    uint32_t tmpCount = count;
    for (uint16_t j = 0; j < repeatTime; ++j) {
        MaskReg reg = UpdateMask<uint8_t>(tmpCount);
        Reg::StoreAlign<uint8_t, PostLiteral::POST_MODE_UPDATE>(
            outputIndex, (RegTensor<uint8_t>&)bucketIndex, GetVecLen(), reg);
    }

    LocalMemBar<MemType::VEC_STORE, MemType::VEC_LOAD>();
    ClearSpr<SpecialPurposeReg::AR>();

    Duplicate(shiftOffset, 0, fullMaskB8);
    Duplicate(last2BitsMask, last2BitMaskVal, fullMaskB8);
    srcAddr = srcValueU8;

    // Clear AR to make sure GatherMask not affected by other operations.
    ClearSpr<SpecialPurposeReg::AR>();
    // Makes 4 times 2 bits sort for 8 bits sort.
    for (uint16_t i = 0; i < 4; ++i) {
        indexAddr = inputIndexAddr;
        UnalignReg unalignIndex;
        uint32_t currCount = count;
        for (uint16_t j = 0; j < repeatTime; ++j) {
            MaskReg maskReg = UpdateMask<uint8_t>(currCount);

            Reg::LoadAlign<uint8_t, PostLiteral::POST_MODE_UPDATE>(srcReg, (__ubuf__ uint8_t*&)srcAddr, GetVecLen());
            Reg::LoadAlign<uint8_t, PostLiteral::POST_MODE_UPDATE>(
                indexReg, (__ubuf__ uint8_t*&)indexAddr, GetVecLen());
            RegTensor<uint8_t> shift2BitsReg;
            ShiftRight(shift2BitsReg, srcReg, shiftOffset, maskReg);

            And(shift2BitsReg, shift2BitsReg, last2BitsMask, maskReg);
            MaskReg cmpMask0, cmpMask1, cmpMask2, cmpMask3;
            CompareScalar<uint8_t, CMPMODE::EQ>(cmpMask0, shift2BitsReg, 0, maskReg);
            CompareScalar<uint8_t, CMPMODE::EQ>(cmpMask1, shift2BitsReg, 1, maskReg);
            CompareScalar<uint8_t, CMPMODE::EQ>(cmpMask2, shift2BitsReg, 2, maskReg);
            CompareScalar<uint8_t, CMPMODE::EQ>(cmpMask3, shift2BitsReg, 3, maskReg);
            RegTensor<uint8_t> gatherRes0;
            RegTensor<uint8_t> gatherRes1;
            RegTensor<uint8_t> gatherRes2;
            RegTensor<uint8_t> gatherRes3;

            GatherMask<uint8_t, GatherMaskMode::STORE_REG>(gatherRes0, indexReg, cmpMask0);
            Reg::StoreUnAlign(indexAddrTmp, gatherRes0, unalignIndex);
            GatherMask<uint8_t, GatherMaskMode::STORE_REG>(gatherRes1, indexReg, cmpMask1);
            Reg::StoreUnAlign(indexAddrTmp, gatherRes1, unalignIndex);
            GatherMask<uint8_t, GatherMaskMode::STORE_REG>(gatherRes2, indexReg, cmpMask2);
            Reg::StoreUnAlign(indexAddrTmp, gatherRes2, unalignIndex);
            GatherMask<uint8_t, GatherMaskMode::STORE_REG>(gatherRes3, indexReg, cmpMask3);
            Reg::StoreUnAlign(indexAddrTmp, gatherRes3, unalignIndex);
        }
        Reg::StoreUnAlignPost(indexAddrTmp, unalignIndex);
        LocalMemBar<MemType::VEC_STORE, MemType::VEC_LOAD>();
        indexAddr = inputIndexAddr;
        srcAddr = srcValueU8;
        srcAddrTmp = srcValueU8;
        __ubuf__ uint8_t* tmpSrcAddr = srcValueU8Back;
        currCount = count;
        for (uint16_t j = 0; j < repeatTime; ++j) {
            MaskReg maskReg = UpdateMask<uint8_t>(currCount);
            RegTensor<uint8_t> selectData, tmpIndexReg;
            Reg::LoadAlign<uint8_t, PostLiteral::POST_MODE_UPDATE>(srcReg, (__ubuf__ uint8_t*&)tmpSrcAddr, GetVecLen());
            Reg::LoadAlign<uint8_t, PostLiteral::POST_MODE_UPDATE>(
                tmpIndexReg, (__ubuf__ uint8_t*&)indexAddr, GetVecLen());
            Gather(selectData, srcReg, tmpIndexReg);
            Reg::StoreAlign<uint8_t, PostLiteral::POST_MODE_UPDATE>(srcAddrTmp, selectData, GetVecLen(), maskReg);
        }
        LocalMemBar<MemType::VEC_STORE, MemType::VEC_LOAD>();
        ClearSpr<SpecialPurposeReg::AR>();
        Adds(shiftOffset, shiftOffset, 2, fullMaskB8);
        srcAddr = srcValueU8;
    }

    // Obtain sorted local u8 index and src values.
    __ubuf__ uint8_t* tmpIndexLoad = inputIndexAddr;
    __ubuf__ uint16_t* tmpIndexStore = inputIndexU16Addr;

    MaskReg fullB16Mask = CreateMask<uint16_t>();

    RegTensor<uint8_t> zeroReg;
    Duplicate(zeroReg, 0, fullMaskB8);

    RegTensor<uint16_t> offset;
    Duplicate(offset, 0, fullB16Mask);

    uint32_t indexUpdateCount = count;
    // Sorts per block elements and convert index representing overall offset.
    for (uint16_t j = 0; j < repeatTime; ++j) {
        MaskReg maskReg0 = UpdateMask<uint16_t>(indexUpdateCount);
        MaskReg maskReg1 = UpdateMask<uint16_t>(indexUpdateCount);

        RegTensor<uint8_t> sortedRes;
        Reg::LoadAlign<uint8_t, PostLiteral::POST_MODE_UPDATE>(
            sortedRes, (__ubuf__ uint8_t*&)tmpIndexLoad, GetVecLen());
        // zero extend to u16
        RegTensor<uint8_t> sortedRes0, sortedRes1;
        Interleave(sortedRes0, sortedRes1, sortedRes, zeroReg);
        // add group offset
        RegTensor<uint16_t> sortedVal0, sortedVal1;
        Add(sortedVal0, (RegTensor<uint16_t>&)sortedRes0, offset, fullB16Mask);
        Add(sortedVal1, (RegTensor<uint16_t>&)sortedRes1, offset, fullB16Mask);
        Reg::StoreAlign<uint16_t, PostLiteral::POST_MODE_UPDATE>(
            tmpIndexStore, sortedVal0, GetVecLen() / sizeof(uint16_t), maskReg0);
        Reg::StoreAlign<uint16_t, PostLiteral::POST_MODE_UPDATE>(
            tmpIndexStore, sortedVal1, GetVecLen() / sizeof(uint16_t), maskReg1);
        Adds(offset, offset, GetVecLen(), fullB16Mask);
    }
}

__simd_vf__ inline void LocalSort(
    __ubuf__ uint8_t* srcU8, __ubuf__ uint16_t* tmpIndexU16, __ubuf__ uint16_t* exclusiveSumOrigin,
    __ubuf__ uint32_t* sortedLocalIndex, uint32_t count)
{
    uint16_t repeatTime = DivCeil(count, GetVecLen());
    RegTensor<uint8_t> key;
    RegTensor<uint16_t> indexU16Bucket0, indexU16Bucket1;
    MaskReg fullMaskB16 = CreateMask<uint16_t>();
    MaskReg fullMaskB8 = CreateMask<uint8_t>();

    RegTensor<int16_t> bucketLocalIndex0, bucketLocalIndex1;
    Arange(bucketLocalIndex0, 0);
    Arange(bucketLocalIndex1, 128);

    RegTensor<uint16_t> bucketOffset0, bucketOffset1;
    Reg::LoadAlign<uint16_t, PostLiteral::POST_MODE_UPDATE>(
        bucketOffset0, (__ubuf__ uint16_t*&)exclusiveSumOrigin, GetVecLen() / sizeof(uint16_t));
    Reg::LoadAlign<uint16_t, PostLiteral::POST_MODE_UPDATE>(
        bucketOffset1, (__ubuf__ uint16_t*&)exclusiveSumOrigin, GetVecLen() / sizeof(uint16_t));

    RegTensor<uint8_t> zeroReg;
    Duplicate(zeroReg, 0, fullMaskB8);

    uint32_t currCount = count;
    uint32_t currCountU8 = count;
    for (uint16_t i = 0; i < repeatTime; i++) {
        MaskReg maskRegB8 = UpdateMask<uint8_t>(currCountU8);
        Reg::LoadAlign<uint8_t, PostLiteral::POST_MODE_UPDATE>(key, (__ubuf__ uint8_t*&)srcU8, GetVecLen());
        Reg::LoadAlign<uint16_t, PostLiteral::POST_MODE_UPDATE>(
            indexU16Bucket0, (__ubuf__ uint16_t*&)tmpIndexU16, GetVecLen() / sizeof(uint16_t));
        Reg::LoadAlign<uint16_t, PostLiteral::POST_MODE_UPDATE>(
            indexU16Bucket1, (__ubuf__ uint16_t*&)tmpIndexU16, GetVecLen() / sizeof(uint16_t));

        RegTensor<uint8_t> bucketOffsetLow, bucketOffsetHigh;
        DeInterleave(
            bucketOffsetLow, bucketOffsetHigh, (RegTensor<uint8_t>&)bucketOffset0, (RegTensor<uint8_t>&)bucketOffset1);

        RegTensor<uint16_t> distributionHist0, distributionHist1;
        RegTensor<uint16_t> cumulativeHist0, cumulativeHist1;
        Duplicate(distributionHist0, 0, fullMaskB16);
        Duplicate(distributionHist1, 0, fullMaskB16);
        Duplicate(cumulativeHist0, 0, fullMaskB16);
        Duplicate(cumulativeHist1, 0, fullMaskB16);

        Histograms<uint8_t, uint16_t, HistogramsBinType::BIN0, HistogramsType::FREQUENCY>(
            distributionHist0, key, maskRegB8);
        Histograms<uint8_t, uint16_t, HistogramsBinType::BIN1, HistogramsType::FREQUENCY>(
            distributionHist1, key, maskRegB8);

        Histograms<uint8_t, uint16_t, HistogramsBinType::BIN0, HistogramsType::ACCUMULATE>(
            cumulativeHist0, key, maskRegB8);
        Histograms<uint8_t, uint16_t, HistogramsBinType::BIN1, HistogramsType::ACCUMULATE>(
            cumulativeHist1, key, maskRegB8);

        RegTensor<uint16_t> localOffset0, localOffset1;
        Sub(localOffset0, cumulativeHist0, distributionHist0, fullMaskB16);
        Sub(localOffset1, cumulativeHist1, distributionHist1, fullMaskB16);

        RegTensor<uint8_t> localOffsetLowbits, localOffsetHighbits;
        DeInterleave(
            localOffsetLowbits, localOffsetHighbits, (RegTensor<uint8_t>&)localOffset0,
            (RegTensor<uint8_t>&)localOffset1);

        // block_offset[key]
        RegTensor<uint8_t> bucketOffsetKeyLowbits, bucketOffsetKeyHighbits;
        Gather(bucketOffsetKeyLowbits, bucketOffsetLow, key);
        Gather(bucketOffsetKeyHighbits, bucketOffsetHigh, key);

        RegTensor<uint8_t> bucketOffsetKey0, bucketOffsetKey1;
        Interleave(bucketOffsetKey0, bucketOffsetKey1, bucketOffsetKeyLowbits, bucketOffsetKeyHighbits);

        // localOffset[key]
        // only select low bits, since count should less than 256
        RegTensor<uint8_t> localOffsetWriteKeyLowbits;
        Gather(localOffsetWriteKeyLowbits, localOffsetLowbits, key);

        RegTensor<uint8_t> localOffsetWriteKey0, localOffsetWriteKey1;
        Interleave(localOffsetWriteKey0, localOffsetWriteKey1, localOffsetWriteKeyLowbits, zeroReg);

        // block_rank = block_offset[key] + local_index - localOffset[key]
        RegTensor<uint16_t> bucketRank0, bucketRank1;
        Add(bucketRank0, (RegTensor<uint16_t>&)bucketOffsetKey0, (RegTensor<uint16_t>&)bucketLocalIndex0, fullMaskB16);
        Add(bucketRank1, (RegTensor<uint16_t>&)bucketOffsetKey1, (RegTensor<uint16_t>&)bucketLocalIndex1, fullMaskB16);

        Sub(bucketRank0, bucketRank0, (RegTensor<uint16_t>&)localOffsetWriteKey0, fullMaskB16);
        Sub(bucketRank1, bucketRank1, (RegTensor<uint16_t>&)localOffsetWriteKey1, fullMaskB16);

        RegTensor<uint32_t> bucketRankU32P0, bucketRankU32P1, bucketRankU32P2, bucketRankU32P3;
        Interleave(
            (RegTensor<uint16_t>&)bucketRankU32P0, (RegTensor<uint16_t>&)bucketRankU32P1, bucketRank0,
            (RegTensor<uint16_t>&)zeroReg);
        Interleave(
            (RegTensor<uint16_t>&)bucketRankU32P2, (RegTensor<uint16_t>&)bucketRankU32P3, bucketRank1,
            (RegTensor<uint16_t>&)zeroReg);

        RegTensor<uint32_t> indexU32P0, indexU32P1, indexU32P2, indexU32P3;
        Interleave(
            (RegTensor<uint16_t>&)indexU32P0, (RegTensor<uint16_t>&)indexU32P1, indexU16Bucket0,
            (RegTensor<uint16_t>&)zeroReg);
        Interleave(
            (RegTensor<uint16_t>&)indexU32P2, (RegTensor<uint16_t>&)indexU32P3, indexU16Bucket1,
            (RegTensor<uint16_t>&)zeroReg);

        // sortedLocalIndex[block_rank] = index
        MaskReg u32Mask0 = UpdateMask<uint32_t>(currCount);
        Reg::Scatter(sortedLocalIndex, indexU32P0, bucketRankU32P0, u32Mask0);
        MaskReg u32Mask1 = UpdateMask<uint32_t>(currCount);
        Reg::Scatter(sortedLocalIndex, indexU32P1, bucketRankU32P1, u32Mask1);
        MaskReg u32Mask2 = UpdateMask<uint32_t>(currCount);
        Reg::Scatter(sortedLocalIndex, indexU32P2, bucketRankU32P2, u32Mask2);
        MaskReg u32Mask3 = UpdateMask<uint32_t>(currCount);
        Reg::Scatter(sortedLocalIndex, indexU32P3, bucketRankU32P3, u32Mask3);

        // accumulate block offset
        Add(bucketOffset0, bucketOffset0, distributionHist0, fullMaskB16);
        Add(bucketOffset1, bucketOffset1, distributionHist1, fullMaskB16);
    }
}

// Gather B64 elements based on uint32_t offset and store at the corresponding memory.
__simd_callee__ inline void GatherAndStoreB64Elements(
    RegTensor<uint32_t>& localOffset, MaskReg& maskReg, __ubuf__ uint32_t* gatherIdxAddr, __ubuf__ uint32_t* storedAddr)
{
    MaskReg maskLow, maskHigh;
    RegTensor<uint32_t> indexLow;
    RegTensor<uint32_t> indexHigh;
    RegTensor<uint32_t> indexMergedLow;
    RegTensor<uint32_t> indexMergedHigh;
    RegTensor<uint32_t> localOffsetHigh;
    MaskInterleave<uint32_t>(maskLow, maskHigh, maskReg, maskReg);
    Muls(localOffsetHigh, localOffset, 2, maskReg);
    Reg::Gather(indexLow, gatherIdxAddr, localOffsetHigh, maskReg);
    Adds(localOffsetHigh, localOffsetHigh, 1, maskReg);
    Reg::Gather(indexHigh, gatherIdxAddr, localOffsetHigh, maskReg);
    Interleave(indexMergedLow, indexMergedHigh, indexLow, indexHigh);
    Reg::StoreAlign<uint32_t>(storedAddr, indexMergedLow, maskLow);
    Reg::StoreAlign<uint32_t>(storedAddr + GetVecLen() / sizeof(uint32_t), indexMergedHigh, maskHigh);
}

template <typename T>
__simd_vf__ inline void UpdateValueAndIndexByLocalIndex(
    __ubuf__ uint8_t* prevSortedValue, __ubuf__ T* prevSortedIndex, __ubuf__ uint32_t* sortedLocalIndex,
    __ubuf__ uint8_t* currSortedValue, __ubuf__ T* currSortedIndex, uint32_t count)
{
    uint16_t repeatTime = DivCeil(count, GetVecLen());
    uint32_t currCount = count;
    for (uint16_t i = 0; i < repeatTime; i++) {
        MaskReg maskReg = UpdateMask<uint8_t>(currCount);
        MaskReg maskRegP0, maskRegP1;
        MaskReg maskLowP0, maskHighP0;
        MaskReg maskLowP1, maskHighP1;
        MaskReg tmp = CreateMask<uint16_t, MaskPattern::ALLF>();
        MaskInterleave<uint8_t>(maskRegP0, maskRegP1, maskReg, tmp);
        MaskInterleave<uint16_t>(maskLowP0, maskHighP0, maskRegP0, tmp);
        MaskInterleave<uint16_t>(maskLowP1, maskHighP1, maskRegP1, tmp);

        RegTensor<uint32_t> localOffset0, localOffset1, localOffset2, localOffset3;
        RegTensor<uint16_t> indexP0, indexP1, indexTmp, resB8P0, resB8P1;
        RegTensor<uint8_t> resB8, tmpB8;

        Reg::LoadAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(
            localOffset0, (__ubuf__ uint32_t*&)sortedLocalIndex, GetVecLen() / sizeof(uint32_t));
        Reg::LoadAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(
            localOffset1, (__ubuf__ uint32_t*&)sortedLocalIndex, GetVecLen() / sizeof(uint32_t));
        Reg::LoadAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(
            localOffset2, (__ubuf__ uint32_t*&)sortedLocalIndex, GetVecLen() / sizeof(uint32_t));
        Reg::LoadAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(
            localOffset3, (__ubuf__ uint32_t*&)sortedLocalIndex, GetVecLen() / sizeof(uint32_t));
        DeInterleave(indexP0, indexTmp, (RegTensor<uint16_t>&)localOffset0, (RegTensor<uint16_t>&)localOffset1);
        DeInterleave(indexP1, indexTmp, (RegTensor<uint16_t>&)localOffset2, (RegTensor<uint16_t>&)localOffset3);

        Reg::Gather(resB8P0, prevSortedValue, indexP0, maskRegP0);
        Reg::Gather(resB8P1, prevSortedValue, indexP1, maskRegP1);
        DeInterleave(resB8, tmpB8, (RegTensor<uint8_t>&)resB8P0, (RegTensor<uint8_t>&)resB8P1);

        if constexpr (sizeof(T) == 8) {
            GatherAndStoreB64Elements(
                localOffset0, maskLowP0, (__ubuf__ uint32_t*)prevSortedIndex,
                (__ubuf__ uint32_t*)(currSortedIndex + 2 * i * GetVecLen() / sizeof(uint16_t)));
            GatherAndStoreB64Elements(
                localOffset1, maskHighP0, (__ubuf__ uint32_t*)prevSortedIndex,
                (__ubuf__ uint32_t*)(currSortedIndex + 2 * i * GetVecLen() / sizeof(uint16_t) +
                                     GetVecLen() / sizeof(uint32_t)));
            GatherAndStoreB64Elements(
                localOffset2, maskLowP1, (__ubuf__ uint32_t*)prevSortedIndex,
                (__ubuf__ uint32_t*)(currSortedIndex + (2 * i + 1) * GetVecLen() / sizeof(uint16_t)));
            GatherAndStoreB64Elements(
                localOffset3, maskHighP1, (__ubuf__ uint32_t*)prevSortedIndex,
                (__ubuf__ uint32_t*)(currSortedIndex + (2 * i + 1) * GetVecLen() / sizeof(uint16_t) +
                                     GetVecLen() / sizeof(uint32_t)));
        } else {
            RegTensor<uint32_t> vecIndex0;
            RegTensor<uint32_t> vecIndex1;
            RegTensor<uint32_t> vecIndex2;
            RegTensor<uint32_t> vecIndex3;
            Reg::Gather(vecIndex0, prevSortedIndex, localOffset0, maskLowP0);
            Reg::Gather(vecIndex1, prevSortedIndex, localOffset1, maskHighP0);
            Reg::Gather(vecIndex2, prevSortedIndex, localOffset2, maskLowP1);
            Reg::Gather(vecIndex3, prevSortedIndex, localOffset3, maskHighP1);
            Reg::StoreAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(
                (__ubuf__ uint32_t*&)currSortedIndex, vecIndex0, GetVecLen() / sizeof(uint32_t), maskLowP0);
            Reg::StoreAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(
                (__ubuf__ uint32_t*&)currSortedIndex, vecIndex1, GetVecLen() / sizeof(uint32_t), maskHighP0);
            Reg::StoreAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(
                (__ubuf__ uint32_t*&)currSortedIndex, vecIndex2, GetVecLen() / sizeof(uint32_t), maskLowP1);
            Reg::StoreAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(
                (__ubuf__ uint32_t*&)currSortedIndex, vecIndex3, GetVecLen() / sizeof(uint32_t), maskHighP1);
        }
        Reg::StoreAlign<uint8_t, PostLiteral::POST_MODE_UPDATE>(
            (__ubuf__ uint8_t*&)currSortedValue, resB8, GetVecLen(), maskReg);
    }
}

template <typename T>
__simd_vf__ inline void UpdateValueAndIndexByLocalIndex(
    __ubuf__ uint16_t* prevSortedValue, __ubuf__ T* prevSortedIndex, __ubuf__ uint32_t* sortedLocalIndex,
    __ubuf__ uint16_t* currSortedValue, __ubuf__ T* currSortedIndex, uint32_t count)
{
    uint16_t repeatTime = DivCeil(count, GetVecLen() / sizeof(uint16_t));
    uint32_t currCount = count;
    for (uint16_t i = 0; i < repeatTime; i++) {
        MaskReg maskReg = UpdateMask<uint16_t>(currCount);
        MaskReg maskLow, maskHigh;
        MaskReg tmp = CreateMask<uint16_t, MaskPattern::ALLF>();
        MaskInterleave<uint16_t>(maskLow, maskHigh, maskReg, tmp);
        RegTensor<uint32_t> localOffset0, localOffset1;
        RegTensor<uint16_t> indexP0;
        RegTensor<uint16_t> indexP1;
        Reg::LoadAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(
            localOffset0, (__ubuf__ uint32_t*&)sortedLocalIndex, GetVecLen() / sizeof(uint32_t));
        Reg::LoadAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(
            localOffset1, (__ubuf__ uint32_t*&)sortedLocalIndex, GetVecLen() / sizeof(uint32_t));

        DeInterleave(indexP1, indexP0, (RegTensor<uint16_t>&)localOffset0, (RegTensor<uint16_t>&)localOffset1);
        Reg::Gather(indexP0, prevSortedValue, indexP1, maskReg);
        if constexpr (sizeof(T) == 8) {
            GatherAndStoreB64Elements(
                localOffset0, maskLow, (__ubuf__ uint32_t*)prevSortedIndex,
                (__ubuf__ uint32_t*)(currSortedIndex + i * GetVecLen() / sizeof(uint16_t)));
            GatherAndStoreB64Elements(
                localOffset1, maskHigh, (__ubuf__ uint32_t*)prevSortedIndex,
                (__ubuf__ uint32_t*)(currSortedIndex + i * GetVecLen() / sizeof(uint16_t) +
                                     GetVecLen() / sizeof(uint32_t)));
        } else {
            RegTensor<uint32_t> indexU32P0;
            RegTensor<uint32_t> indexU32P1;
            Reg::Gather(indexU32P0, prevSortedIndex, localOffset0, maskLow);
            Reg::Gather(indexU32P1, prevSortedIndex, localOffset1, maskHigh);
            Reg::StoreAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(
                (__ubuf__ uint32_t*&)currSortedIndex, indexU32P0, GetVecLen() / sizeof(uint32_t), maskLow);
            Reg::StoreAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(
                (__ubuf__ uint32_t*&)currSortedIndex, indexU32P1, GetVecLen() / sizeof(uint32_t), maskHigh);
        }
        Reg::StoreAlign<uint16_t, PostLiteral::POST_MODE_UPDATE>(
            (__ubuf__ uint16_t*&)currSortedValue, indexP0, GetVecLen() / sizeof(uint16_t), maskReg);
    }
}

template <typename T>
__simd_vf__ inline void UpdateValueAndIndexByLocalIndex(
    __ubuf__ uint32_t* prevSortedValue, __ubuf__ T* prevSortedIndex, __ubuf__ uint32_t* sortedLocalIndex,
    __ubuf__ uint32_t* currSortedValue, __ubuf__ T* currSortedIndex, uint32_t count)
{
    uint16_t repeatTime = DivCeil(count, GetVecLen() / sizeof(uint32_t));
    uint32_t currCount = count;
    for (uint16_t i = 0; i < repeatTime; i++) {
        MaskReg maskReg = UpdateMask<uint32_t>(currCount);
        RegTensor<uint32_t> localOffset;
        RegTensor<uint32_t> index;
        Reg::LoadAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(
            localOffset, (__ubuf__ uint32_t*&)sortedLocalIndex, GetVecLen() / sizeof(uint32_t));
        Reg::Gather(index, prevSortedValue, localOffset, maskReg);
        Reg::StoreAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(
            (__ubuf__ uint32_t*&)currSortedValue, index, GetVecLen() / sizeof(uint32_t), maskReg);
        if constexpr (sizeof(T) == 8) {
            GatherAndStoreB64Elements(
                localOffset, maskReg, (__ubuf__ uint32_t*)prevSortedIndex,
                (__ubuf__ uint32_t*)(currSortedIndex + i * GetVecLen() / sizeof(uint32_t)));
        } else {
            RegTensor<uint32_t> indexU32;
            Reg::Gather(indexU32, prevSortedIndex, localOffset, maskReg);
            Reg::StoreAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(
                (__ubuf__ uint32_t*&)currSortedIndex, indexU32, GetVecLen() / sizeof(uint32_t), maskReg);
        }
    }
}

template <typename T>
__simd_vf__ inline void UpdateValueAndIndexByLocalIndex(
    __ubuf__ uint64_t* prevSortedValue, __ubuf__ T* prevSortedIndex, __ubuf__ uint32_t* sortedLocalIndex,
    __ubuf__ uint64_t* currSortedValue, __ubuf__ T* currSortedIndex, uint32_t count)
{
    uint16_t repeatTime = DivCeil(count, GetVecLen() / sizeof(uint64_t));
    uint32_t currCount = count;
    for (uint16_t i = 0; i < repeatTime; i++) {
        MaskReg maskReg = UpdateMask<uint32_t>(currCount);
        RegTensor<uint32_t> localOffset;
        RegTensor<uint32_t> index;
        Reg::LoadAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(
            localOffset, (__ubuf__ uint32_t*&)sortedLocalIndex, GetVecLen() / sizeof(uint32_t));
        GatherAndStoreB64Elements(
            localOffset, maskReg, (__ubuf__ uint32_t*)prevSortedValue,
            (__ubuf__ uint32_t*)(currSortedValue + i * GetVecLen() / sizeof(uint32_t)));
        if constexpr (sizeof(T) == 8) {
            GatherAndStoreB64Elements(
                localOffset, maskReg, (__ubuf__ uint32_t*)prevSortedIndex,
                (__ubuf__ uint32_t*)(currSortedIndex + i * GetVecLen() / sizeof(uint32_t)));
        } else {
            RegTensor<uint32_t> indexU32;
            Reg::Gather(indexU32, prevSortedIndex, localOffset, maskReg);
            Reg::StoreAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(
                (__ubuf__ uint32_t*&)currSortedIndex, indexU32, GetVecLen() / sizeof(uint32_t), maskReg);
        }
    }
}

template <typename T>
__simd_vf__ inline void UpdateValueByLocalIndex(
    __ubuf__ T* srcValue, __ubuf__ uint32_t* sortedLocalIndex, __ubuf__ T* currSortedValue, uint32_t count)
{
    uint16_t repeatTime = DivCeil(count, GetVecLen() / sizeof(T));
    uint32_t currCount = count;
    for (uint16_t i = 0; i < repeatTime; i++) {
        if constexpr (sizeof(T) == 1) {
            MaskReg maskReg = UpdateMask<uint8_t>(currCount);
            MaskReg maskRegP0, maskRegP1;
            MaskReg maskLowP0, maskHighP0;
            MaskReg maskLowP1, maskHighP1;
            MaskReg tmp = CreateMask<uint16_t, MaskPattern::ALLF>();
            MaskInterleave<uint8_t>(maskRegP0, maskRegP1, maskReg, tmp);
            MaskInterleave<uint16_t>(maskLowP0, maskHighP0, maskRegP0, tmp);
            MaskInterleave<uint16_t>(maskLowP1, maskHighP1, maskRegP1, tmp);

            RegTensor<uint32_t> localOffset0, localOffset1, localOffset2, localOffset3;
            RegTensor<uint16_t> indexP0, indexP1, indexTmp, resB8P0, resB8P1;
            RegTensor<uint8_t> resB8, tmpB8;

            Reg::LoadAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(
                localOffset0, (__ubuf__ uint32_t*&)sortedLocalIndex, GetVecLen() / sizeof(uint32_t));
            Reg::LoadAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(
                localOffset1, (__ubuf__ uint32_t*&)sortedLocalIndex, GetVecLen() / sizeof(uint32_t));
            Reg::LoadAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(
                localOffset2, (__ubuf__ uint32_t*&)sortedLocalIndex, GetVecLen() / sizeof(uint32_t));
            Reg::LoadAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(
                localOffset3, (__ubuf__ uint32_t*&)sortedLocalIndex, GetVecLen() / sizeof(uint32_t));
            DeInterleave(indexP0, indexTmp, (RegTensor<uint16_t>&)localOffset0, (RegTensor<uint16_t>&)localOffset1);
            DeInterleave(indexP1, indexTmp, (RegTensor<uint16_t>&)localOffset2, (RegTensor<uint16_t>&)localOffset3);
            Reg::Gather(resB8P0, srcValue, indexP0, maskRegP0);
            Reg::Gather(resB8P1, srcValue, indexP1, maskRegP1);
            DeInterleave(resB8, tmpB8, (RegTensor<uint8_t>&)resB8P0, (RegTensor<uint8_t>&)resB8P1);

            Reg::StoreAlign((__ubuf__ T*)(currSortedValue + i * (GetVecLen() / sizeof(T))), resB8, maskReg);
        } else if constexpr (sizeof(T) == 2) {
            MaskReg maskReg = UpdateMask<uint16_t>(currCount);
            MaskReg maskLow, maskHigh;
            MaskReg tmp = CreateMask<uint16_t, MaskPattern::ALLF>();
            MaskInterleave<uint16_t>(maskLow, maskHigh, maskReg, tmp);

            RegTensor<uint32_t> localOffset0, localOffset1;
            RegTensor<uint16_t> indexP0;
            RegTensor<uint16_t> indexP1;

            Reg::LoadAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(
                localOffset0, (__ubuf__ uint32_t*&)sortedLocalIndex, GetVecLen() / sizeof(uint32_t));
            Reg::LoadAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(
                localOffset1, (__ubuf__ uint32_t*&)sortedLocalIndex, GetVecLen() / sizeof(uint32_t));
            DeInterleave(indexP1, indexP0, (RegTensor<uint16_t>&)localOffset0, (RegTensor<uint16_t>&)localOffset1);
            Reg::Gather(indexP0, srcValue, indexP1, maskReg);

            Reg::StoreAlign((__ubuf__ T*)(currSortedValue + i * (GetVecLen() / sizeof(T))), indexP0, maskReg);
        } else {
            MaskReg maskReg = UpdateMask<uint32_t>(currCount);
            RegTensor<uint32_t> localOffset;
            RegTensor<uint32_t> indexP0;
            Reg::LoadAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(
                localOffset, (__ubuf__ uint32_t*&)sortedLocalIndex, GetVecLen() / sizeof(uint32_t));
            if constexpr (sizeof(T) == 8) {
                GatherAndStoreB64Elements(
                    localOffset, maskReg, (__ubuf__ uint32_t*)srcValue,
                    (__ubuf__ uint32_t*)(currSortedValue + i * GetVecLen() / sizeof(uint32_t)));
            } else {
                Reg::Gather(indexP0, srcValue, localOffset, maskReg);
                Reg::StoreAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(
                    (__ubuf__ uint32_t*&)currSortedValue, indexP0, GetVecLen() / sizeof(uint32_t), maskReg);
            }
        }
    }
}

template <typename T>
__simd_vf__ inline void SaveBufferTo(__ubuf__ T* srcBuffer, __ubuf__ T* dstBuffer, uint32_t count)
{
    uint16_t repeatTime = DivCeil(count, GetVecLen() / sizeof(T));
    for (uint16_t i = 0; i < repeatTime; i++) {
        MaskReg maskReg = UpdateMask<T>(count);
        RegTensor<T> reg;
        Reg::LoadAlign<T, PostLiteral::POST_MODE_UPDATE>(reg, srcBuffer, GetVecLen() / sizeof(T));
        Reg::StoreAlign<T, PostLiteral::POST_MODE_UPDATE>(dstBuffer, reg, GetVecLen() / sizeof(T), maskReg);
    }
}

__simd_vf__ inline void SaveBufferTo(__ubuf__ uint64_t* srcBuffer, __ubuf__ uint64_t* dstBuffer, uint32_t count)
{
    uint32_t calCount = count << 1;
    uint16_t repeatTime = DivCeil(calCount, GetVecLen() / sizeof(uint32_t));
    __ubuf__ uint32_t* tmpSrc = (__ubuf__ uint32_t*)srcBuffer;
    __ubuf__ uint32_t* tmpDst = (__ubuf__ uint32_t*)dstBuffer;
    for (uint16_t i = 0; i < repeatTime; i++) {
        MaskReg maskReg = UpdateMask<uint32_t>(calCount);
        RegTensor<uint32_t> reg;
        Reg::LoadAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(reg, tmpSrc, GetVecLen() / sizeof(uint32_t));
        Reg::StoreAlign<uint32_t, PostLiteral::POST_MODE_UPDATE>(tmpDst, reg, GetVecLen() / sizeof(uint32_t), maskReg);
    }
}

template <typename T>
__aicore__ inline void SwapBuffer(__ubuf__ T*& a, __ubuf__ T*& b)
{
    __ubuf__ T* tmp = a;
    a = b;
    b = tmp;
}

template <typename T, bool isDescend>
__aicore__ inline constexpr bool CheckDataProcess()
{
    return isDescend || SupportType<T, int8_t, int16_t, int32_t, float, half, bfloat16_t, int64_t>();
}

template <typename T>
__aicore__ inline void SortU8ElementsWithRound(
    __ubuf__ T* srcValue, __ubuf__ uint8_t* tmpSrc, __ubuf__ uint8_t* tmpSrcCopy, __ubuf__ uint16_t* exclusiveSum,
    __ubuf__ uint8_t* tmpIndexU8, __ubuf__ uint16_t* tmpIndexU16, __ubuf__ uint32_t* sortedLocalIndex,
    const uint32_t count, const int32_t round)
{
    GetExclusiveSum(srcValue, tmpSrc, tmpSrcCopy, exclusiveSum, count, round);
    EightBitsSort(tmpSrc, tmpSrcCopy, tmpIndexU8, tmpIndexU16, count);
    LocalSort(tmpSrc, tmpIndexU16, exclusiveSum, sortedLocalIndex, count);
}

__aicore__ inline void ArrangeCommonTmpBuffer(
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t alignCount, __ubuf__ uint16_t*& totalExclusiveSum,
    __ubuf__ uint8_t*& srcU8, __ubuf__ uint16_t*& tmpIndexU16, __ubuf__ uint32_t*& sortedLocalIndex,
    __ubuf__ uint8_t*& srcU8Copy, __ubuf__ uint8_t*& tmpIndexU8)
{
    constexpr uint32_t bucketBuffer = 512;
    __ubuf__ uint8_t* tmp = (__ubuf__ uint8_t*)sharedTmpBuffer.GetPhyAddr();
    totalExclusiveSum = (__ubuf__ uint16_t*)tmp;
    srcU8 = (__ubuf__ uint8_t*)(tmp + bucketBuffer);
    tmpIndexU16 = (__ubuf__ uint16_t*)(srcU8 + alignCount);
    sortedLocalIndex = (__ubuf__ uint32_t*)((__ubuf__ uint8_t*)tmpIndexU16 + sizeof(uint16_t) * alignCount);
    // Tmp u8 value and index, could be reused with sortedLocalIndex.
    srcU8Copy = (__ubuf__ uint8_t*)sortedLocalIndex;
    tmpIndexU8 = srcU8Copy + alignCount;
}

template <typename T>
__simd_vf__ inline void DescendProcess(__ubuf__ T* dst, uint32_t count)
{
    constexpr uint32_t halfTypeMask = 0x80008000;
    constexpr uint32_t floatTypeMask = 0x80000000;
    Reg::RegTensor<T> vreg;
    Reg::RegTensor<uint32_t> vDupReg;
    Reg::MaskReg mask;
    constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(T));
    uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(count, oneRepElm));
    if constexpr (IsSameType<T, float>::value) {
        Reg::Duplicate(vDupReg, floatTypeMask);
    } else {
        Reg::Duplicate(vDupReg, halfTypeMask);
    }
    for (uint16_t i = 0; i < repeatTime; ++i) {
        mask = Reg::UpdateMask<T>(count);
        Reg::LoadAlign(vreg, dst + i * oneRepElm);
        Reg::Xor((Reg::RegTensor<uint32_t>&)vreg, (Reg::RegTensor<uint32_t>&)vreg, vDupReg, mask);
        Reg::StoreAlign(dst + i * oneRepElm, vreg, mask);
    }
}

template <typename T, bool isReuseSource, const SortConfig& config>
__aicore__ inline void SortImpl(
    LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t count)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    constexpr SortType sortType = config.type;
    constexpr bool isDescend = config.isDescend;
    ASCENDC_ASSERT(
        (srcLocal.GetSize() * sizeof(T) % ONE_BLK_SIZE == 0 && dstLocal.GetSize() * sizeof(T) % ONE_BLK_SIZE == 0),
        { KERNEL_LOG(KERNEL_ERROR, "srcTensor and dstTensor size must be 32B aligned"); });
    ASCENDC_ASSERT((count > 0 && count <= srcLocal.GetSize() && count <= dstLocal.GetSize()), {
        KERNEL_LOG(
            KERNEL_ERROR, "count must be greater than 0 and less than or equals to srcTensor and dstTensor Size");
    });
    if constexpr (sortType == SortType::MERGE_SORT) {
        static_assert(
            SupportType<T, half, float>(),
            "Advanced Sort API MERGE_MODE only supports data value with half/float types.");
        ASCENDC_ASSERT((count % singleSortElementCount == 0), {
            KERNEL_LOG(KERNEL_ERROR, "calCount should be align to 32 in MERGE_MODE!");
        });
        ASCENDC_ASSERT((dstLocal.GetSize() * sizeof(T) >= count * 8), {
            KERNEL_LOG(KERNEL_ERROR, "dstLocal size should be greater equal to 8 * count!");
        });
        const int32_t repeatTime = static_cast<int32_t>(count / singleSortElementCount);
        if constexpr (!isDescend) {
            DescendProcess<T>((__ubuf__ T*)srcLocal.GetPhyAddr(), count);
        }
        Sort32(dstLocal, srcLocal, (const LocalTensor<uint32_t>&)srcLocal, repeatTime);
        DoFullSort(
            dstLocal, (const LocalTensor<T>&)sharedTmpBuffer, (const LocalTensor<uint32_t>&)sharedTmpBuffer,
            (LocalTensor<T>&)sharedTmpBuffer, repeatTime);
        Extract(dstLocal, (const LocalTensor<uint32_t>&)sharedTmpBuffer, dstLocal, repeatTime);
        if constexpr (!isDescend) {
            DescendProcess<T>((__ubuf__ T*)dstLocal.GetPhyAddr(), count);
        }
    } else {
        static_assert(
            SupportBytes<T, 2, 4, 8>() || SupportType<T, uint8_t, int8_t>(),
            "Advanced Sort API RADIX_MODE only supports data value with "
            "int8_t/uint8_t/half/bfloat16_t/int16_t/uint16_t/float/int32_t/uint32_t/int64_t/uint64_t types.");
        constexpr bool needProcess = CheckDataProcess<T, isDescend>();
        using ConvType = typename ::AscendC::Internal::ExtractTypeBySize<sizeof(T)>::T;
        uint32_t alignCount = AlignUp(count, ONE_BLK_SIZE);
        __ubuf__ ConvType* src = (__ubuf__ ConvType*)srcLocal.GetPhyAddr();
        __ubuf__ ConvType* dst = (__ubuf__ ConvType*)dstLocal.GetPhyAddr();
        __ubuf__ uint16_t *totalExclusiveSum, *tmpIndexU16;
        __ubuf__ uint8_t *srcU8, *srcU8Copy, *tmpIndexU8;
        __ubuf__ uint32_t* sortedLocalIndex;
        ArrangeCommonTmpBuffer(
            sharedTmpBuffer, alignCount, totalExclusiveSum, srcU8, tmpIndexU16, sortedLocalIndex, srcU8Copy,
            tmpIndexU8);
        __ubuf__ ConvType* srcForNextRound =
            (__ubuf__ ConvType*)((__ubuf__ uint8_t*)sortedLocalIndex + sizeof(uint32_t) * alignCount);
        __ubuf__ ConvType* currSortedValue = srcForNextRound;
        __ubuf__ ConvType* prevSortedValue = dst;
        __ubuf__ ConvType* initSrcValue = src;

        if constexpr (sizeof(T) == 1 && needProcess) {
            Internal::TwiddleInData<T, ConvType, isDescend>(src, dst, count);
            initSrcValue = dst;
        } else {
            if constexpr (needProcess) {
                Internal::TwiddleInData<T, ConvType, isDescend>(src, dst, count);
                initSrcValue = dst;
                if constexpr (isReuseSource) {
                    currSortedValue = src;
                }
            } else if constexpr (isReuseSource) {
                SaveBufferTo(src, dst, count);
                initSrcValue = dst;
                currSortedValue = src;
            }
        }

        if constexpr (sizeof(T) == 1) {
            SortU8ElementsWithRound(
                initSrcValue, srcU8, srcU8Copy, totalExclusiveSum, tmpIndexU8, tmpIndexU16, sortedLocalIndex, count, 0);
            UpdateValueByLocalIndex(src, sortedLocalIndex, dst, count);
        } else {
            SortU8ElementsWithRound(
                initSrcValue, srcU8, srcU8Copy, totalExclusiveSum, tmpIndexU8, tmpIndexU16, sortedLocalIndex, count, 0);
            UpdateValueByLocalIndex(initSrcValue, sortedLocalIndex, currSortedValue, count);

            SwapBuffer(currSortedValue, prevSortedValue);

            SortU8ElementsWithRound(
                prevSortedValue, srcU8, srcU8Copy, totalExclusiveSum, tmpIndexU8, tmpIndexU16, sortedLocalIndex, count,
                1);
            UpdateValueByLocalIndex(prevSortedValue, sortedLocalIndex, currSortedValue, count);

            if constexpr (sizeof(T) >= 4) {
                SwapBuffer(currSortedValue, prevSortedValue);

                SortU8ElementsWithRound(
                    prevSortedValue, srcU8, srcU8Copy, totalExclusiveSum, tmpIndexU8, tmpIndexU16, sortedLocalIndex,
                    count, 2);
                UpdateValueByLocalIndex(prevSortedValue, sortedLocalIndex, currSortedValue, count);

                SwapBuffer(currSortedValue, prevSortedValue);

                SortU8ElementsWithRound(
                    prevSortedValue, srcU8, srcU8Copy, totalExclusiveSum, tmpIndexU8, tmpIndexU16, sortedLocalIndex,
                    count, 3);
                UpdateValueByLocalIndex(prevSortedValue, sortedLocalIndex, currSortedValue, count);
            }

            if constexpr (sizeof(T) == 8) {
                SwapBuffer(currSortedValue, prevSortedValue);

                SortU8ElementsWithRound(
                    prevSortedValue, srcU8, srcU8Copy, totalExclusiveSum, tmpIndexU8, tmpIndexU16, sortedLocalIndex,
                    count, 4);
                UpdateValueByLocalIndex(prevSortedValue, sortedLocalIndex, currSortedValue, count);

                SwapBuffer(currSortedValue, prevSortedValue);

                SortU8ElementsWithRound(
                    prevSortedValue, srcU8, srcU8Copy, totalExclusiveSum, tmpIndexU8, tmpIndexU16, sortedLocalIndex,
                    count, 5);
                UpdateValueByLocalIndex(prevSortedValue, sortedLocalIndex, currSortedValue, count);

                SwapBuffer(currSortedValue, prevSortedValue);

                SortU8ElementsWithRound(
                    prevSortedValue, srcU8, srcU8Copy, totalExclusiveSum, tmpIndexU8, tmpIndexU16, sortedLocalIndex,
                    count, 6);
                UpdateValueByLocalIndex(prevSortedValue, sortedLocalIndex, currSortedValue, count);

                SwapBuffer(currSortedValue, prevSortedValue);

                SortU8ElementsWithRound(
                    prevSortedValue, srcU8, srcU8Copy, totalExclusiveSum, tmpIndexU8, tmpIndexU16, sortedLocalIndex,
                    count, 7);
                UpdateValueByLocalIndex(prevSortedValue, sortedLocalIndex, currSortedValue, count);
            }

            if constexpr (needProcess) {
                Internal::TwiddleOutData<T, ConvType, isDescend>(dst, dst, count);
            }
        }
    }
}

__simd_vf__ inline void GenSrcIndex(__ubuf__ uint32_t* dst, uint32_t count)
{
    Reg::RegTensor<uint32_t> vreg;
    Reg::MaskReg mask;
    constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(uint32_t));
    uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(count, oneRepElm));
    Reg::Arange((Reg::RegTensor<int32_t>&)vreg, 0u);
    for (uint16_t i = 0; i < repeatTime; ++i) {
        mask = Reg::UpdateMask<uint32_t>(count);
        Reg::StoreAlign(dst + i * oneRepElm, vreg, mask);
        Reg::Adds(vreg, vreg, oneRepElm, mask);
    }
}

template <typename T, bool isReuseSource, const SortConfig& config>
__aicore__ inline void SortImpl(
    LocalTensor<T>& dstLocal, LocalTensor<uint32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t count)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    constexpr SortType sortType = config.type;
    constexpr bool isDescend = config.isDescend;
    ASCENDC_ASSERT((srcLocal.GetSize() * sizeof(T) % ONE_BLK_SIZE == 0), {
        KERNEL_LOG(KERNEL_ERROR, "srcTensor size must be 32B aligned");
    });
    ASCENDC_ASSERT(
        (dstIndexLocal.GetSize() * sizeof(uint32_t) % ONE_BLK_SIZE == 0 &&
         dstLocal.GetSize() * sizeof(T) % ONE_BLK_SIZE == 0),
        { KERNEL_LOG(KERNEL_ERROR, "dstTensor and dstIndexTensor size must be 32B aligned"); });
    ASCENDC_ASSERT(
        (count > 0 && count <= srcLocal.GetSize() && count <= dstLocal.GetSize() && count <= dstIndexLocal.GetSize()), {
            KERNEL_LOG(
                KERNEL_ERROR, "count must be greater than 0 and less than or equals to srcTensor and dstTensor Size");
        });
    if constexpr (sortType == SortType::MERGE_SORT) {
        static_assert(
            SupportType<T, half, float>(),
            "Advanced Sort API MERGE_MODE only supports data value with half/float types.");
        ASCENDC_ASSERT((count % singleSortElementCount == 0), {
            KERNEL_LOG(KERNEL_ERROR, "calCount should be align to 32 in MERGE_MODE!");
        });
        ASCENDC_ASSERT((dstLocal.GetSize() * sizeof(T) >= count * 8), {
            KERNEL_LOG(KERNEL_ERROR, "dstLocal size should be greater equal to 8 * count!");
        });
        const int32_t repeatTime = static_cast<int32_t>(count / singleSortElementCount);
        if constexpr (!isDescend) {
            DescendProcess<T>((__ubuf__ T*)srcLocal.GetPhyAddr(), count);
        }
        GenSrcIndex((__ubuf__ uint32_t*)sharedTmpBuffer.GetPhyAddr(), count);
        Sort32(dstLocal, srcLocal, (const LocalTensor<uint32_t>&)sharedTmpBuffer, repeatTime);
        DoFullSort(
            dstLocal, (const LocalTensor<T>&)sharedTmpBuffer, (const LocalTensor<uint32_t>&)sharedTmpBuffer,
            (LocalTensor<T>&)sharedTmpBuffer, repeatTime);
        Extract(dstLocal, dstIndexLocal, dstLocal, repeatTime);
        if constexpr (!isDescend) {
            DescendProcess<T>((__ubuf__ T*)dstLocal.GetPhyAddr(), count);
        }
    } else {
        static_assert(
            SupportBytes<T, 2, 4, 8>() || SupportType<T, uint8_t, int8_t>(),
            "Advanced Sort API RADIX_MODE only supports data value with "
            "int8_t/uint8_t/half/bfloat16_t/int16_t/uint16_t/float/int32_t/uint32_t/int64_t/uint64_t types.");
        constexpr bool needProcess = CheckDataProcess<T, isDescend>();
        using ConvType = typename ::AscendC::Internal::ExtractTypeBySize<sizeof(T)>::T;
        uint32_t alignCount = AlignUp(count, ONE_BLK_SIZE);
        __ubuf__ ConvType* src = (__ubuf__ ConvType*)srcLocal.GetPhyAddr();
        __ubuf__ ConvType* dst = (__ubuf__ ConvType*)dstLocal.GetPhyAddr();
        __ubuf__ uint32_t* dstIndex = (__ubuf__ uint32_t*)dstIndexLocal.GetPhyAddr();
        __ubuf__ uint16_t *totalExclusiveSum, *tmpIndexU16;
        __ubuf__ uint8_t *srcU8, *srcU8Copy, *tmpIndexU8;
        __ubuf__ uint32_t* sortedLocalIndex;
        ArrangeCommonTmpBuffer(
            sharedTmpBuffer, alignCount, totalExclusiveSum, srcU8, tmpIndexU16, sortedLocalIndex, srcU8Copy,
            tmpIndexU8);
        __ubuf__ uint32_t* sortedIndexNextRound =
            (__ubuf__ uint32_t*)((__ubuf__ uint8_t*)sortedLocalIndex + sizeof(uint32_t) * alignCount);
        __ubuf__ ConvType* srcForNextRound =
            (__ubuf__ ConvType*)((__ubuf__ uint8_t*)sortedIndexNextRound + sizeof(uint32_t) * alignCount);
        __ubuf__ uint32_t* currSortedIndex = sortedIndexNextRound;
        __ubuf__ ConvType* currSortedValue = srcForNextRound;
        __ubuf__ uint32_t* prevSortedIndex = dstIndex;
        __ubuf__ ConvType* prevSortedValue = dst;
        __ubuf__ ConvType* initSrcValue = src;

        if constexpr (sizeof(T) == 1 && needProcess) {
            Internal::TwiddleInData<T, ConvType, isDescend>(src, dst, count);
            initSrcValue = dst;
        } else {
            if constexpr (needProcess) {
                Internal::TwiddleInData<T, ConvType, isDescend>(src, dst, count);
                initSrcValue = dst;
                if constexpr (isReuseSource) {
                    currSortedValue = src;
                }
            } else if constexpr (isReuseSource) {
                SaveBufferTo(src, dst, count);
                initSrcValue = dst;
                currSortedValue = src;
            }
        }

        if constexpr (sizeof(T) == 1) {
            SortU8ElementsWithRound(
                initSrcValue, srcU8, srcU8Copy, totalExclusiveSum, tmpIndexU8, tmpIndexU16, dstIndex, count, 0);
            UpdateValueByLocalIndex(src, dstIndex, dst, count);
        } else {
            SortU8ElementsWithRound(
                initSrcValue, srcU8, srcU8Copy, totalExclusiveSum, tmpIndexU8, tmpIndexU16, currSortedIndex, count, 0);
            UpdateValueByLocalIndex(initSrcValue, currSortedIndex, currSortedValue, count);

            SwapBuffer(currSortedIndex, prevSortedIndex);
            SwapBuffer(currSortedValue, prevSortedValue);

            SortU8ElementsWithRound(
                prevSortedValue, srcU8, srcU8Copy, totalExclusiveSum, tmpIndexU8, tmpIndexU16, sortedLocalIndex, count,
                1);
            UpdateValueAndIndexByLocalIndex(
                prevSortedValue, prevSortedIndex, sortedLocalIndex, currSortedValue, currSortedIndex, count);

            if constexpr (sizeof(T) >= 4) {
                SwapBuffer(currSortedIndex, prevSortedIndex);
                SwapBuffer(currSortedValue, prevSortedValue);

                SortU8ElementsWithRound(
                    prevSortedValue, srcU8, srcU8Copy, totalExclusiveSum, tmpIndexU8, tmpIndexU16, sortedLocalIndex,
                    count, 2);
                UpdateValueAndIndexByLocalIndex(
                    prevSortedValue, prevSortedIndex, sortedLocalIndex, currSortedValue, currSortedIndex, count);

                SwapBuffer(currSortedIndex, prevSortedIndex);
                SwapBuffer(currSortedValue, prevSortedValue);

                SortU8ElementsWithRound(
                    prevSortedValue, srcU8, srcU8Copy, totalExclusiveSum, tmpIndexU8, tmpIndexU16, sortedLocalIndex,
                    count, 3);
                UpdateValueAndIndexByLocalIndex(
                    prevSortedValue, prevSortedIndex, sortedLocalIndex, currSortedValue, currSortedIndex, count);
            }

            if constexpr (sizeof(T) == 8) {
                SwapBuffer(currSortedIndex, prevSortedIndex);
                SwapBuffer(currSortedValue, prevSortedValue);

                SortU8ElementsWithRound(
                    prevSortedValue, srcU8, srcU8Copy, totalExclusiveSum, tmpIndexU8, tmpIndexU16, sortedLocalIndex,
                    count, 4);
                UpdateValueAndIndexByLocalIndex(
                    prevSortedValue, prevSortedIndex, sortedLocalIndex, currSortedValue, currSortedIndex, count);

                SwapBuffer(currSortedIndex, prevSortedIndex);
                SwapBuffer(currSortedValue, prevSortedValue);

                SortU8ElementsWithRound(
                    prevSortedValue, srcU8, srcU8Copy, totalExclusiveSum, tmpIndexU8, tmpIndexU16, sortedLocalIndex,
                    count, 5);
                UpdateValueAndIndexByLocalIndex(
                    prevSortedValue, prevSortedIndex, sortedLocalIndex, currSortedValue, currSortedIndex, count);

                SwapBuffer(currSortedIndex, prevSortedIndex);
                SwapBuffer(currSortedValue, prevSortedValue);

                SortU8ElementsWithRound(
                    prevSortedValue, srcU8, srcU8Copy, totalExclusiveSum, tmpIndexU8, tmpIndexU16, sortedLocalIndex,
                    count, 6);
                UpdateValueAndIndexByLocalIndex(
                    prevSortedValue, prevSortedIndex, sortedLocalIndex, currSortedValue, currSortedIndex, count);

                SwapBuffer(currSortedIndex, prevSortedIndex);
                SwapBuffer(currSortedValue, prevSortedValue);

                SortU8ElementsWithRound(
                    prevSortedValue, srcU8, srcU8Copy, totalExclusiveSum, tmpIndexU8, tmpIndexU16, sortedLocalIndex,
                    count, 7);
                UpdateValueAndIndexByLocalIndex(
                    prevSortedValue, prevSortedIndex, sortedLocalIndex, currSortedValue, currSortedIndex, count);
            }

            if constexpr (needProcess) {
                Internal::TwiddleOutData<T, ConvType, isDescend>(dst, dst, count);
            }
        }
    }
}

template <typename T, typename U, bool isReuseSource, const SortConfig& config>
__aicore__ inline void SortImpl(
    const LocalTensor<T>& dstLocal, const LocalTensor<U>& dstIndexLocal, const LocalTensor<T>& srcLocal,
    const LocalTensor<U>& srcIndexLocal, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t count)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    constexpr SortType sortType = config.type;
    constexpr bool isDescend = config.isDescend;
    ASCENDC_ASSERT(
        (srcLocal.GetSize() * sizeof(T) % ONE_BLK_SIZE == 0 && srcIndexLocal.GetSize() * sizeof(U) % ONE_BLK_SIZE == 0),
        { KERNEL_LOG(KERNEL_ERROR, "srcTensor and srcIndexTensor size must be 32B aligned"); });
    ASCENDC_ASSERT(
        (dstIndexLocal.GetSize() * sizeof(U) % ONE_BLK_SIZE == 0 && dstLocal.GetSize() * sizeof(T) % ONE_BLK_SIZE == 0),
        { KERNEL_LOG(KERNEL_ERROR, "dstTensor and dstIndexTensor size must be 32B aligned"); });
    ASCENDC_ASSERT(
        (count > 0 && count <= srcLocal.GetSize() && count <= srcIndexLocal.GetSize() && count <= dstLocal.GetSize() &&
         count <= dstIndexLocal.GetSize()),
        {
            KERNEL_LOG(
                KERNEL_ERROR, "count must be greater than 0 and less than or equals to all src and dst tensors Size");
        });

    if constexpr (sortType == SortType::MERGE_SORT) {
        static_assert(
            SupportType<T, half, float>(),
            "Advanced Sort API MERGE_MODE only supports data value with half/float types.");
        static_assert(
            SupportType<U, uint32_t>(), "Advanced Sort API MERGE_MODE only supports src index with uint32_t types.");
        ASCENDC_ASSERT((count % singleSortElementCount == 0), {
            KERNEL_LOG(KERNEL_ERROR, "calCount should be align to 32 in MERGE_MODE!");
        });
        ASCENDC_ASSERT((dstLocal.GetSize() * sizeof(T) >= count * 8), {
            KERNEL_LOG(KERNEL_ERROR, "dstLocal size should be greater equal to 8 * count!");
        });
        const int32_t repeatTime = static_cast<int32_t>(count / singleSortElementCount);
        if constexpr (!isDescend) {
            DescendProcess<T>((__ubuf__ T*)srcLocal.GetPhyAddr(), count);
        }
        Sort32(dstLocal, srcLocal, srcIndexLocal, repeatTime);
        DoFullSort(
            dstLocal, (const LocalTensor<T>&)sharedTmpBuffer, (const LocalTensor<uint32_t>&)sharedTmpBuffer,
            (LocalTensor<T>&)sharedTmpBuffer, repeatTime);
        Extract(dstLocal, dstIndexLocal, dstLocal, repeatTime);
        if constexpr (!isDescend) {
            DescendProcess<T>((__ubuf__ T*)dstLocal.GetPhyAddr(), count);
        }
    } else {
        static_assert(
            SupportBytes<T, 2, 4, 8>() || SupportType<T, uint8_t, int8_t>(),
            "Advanced Sort API RADIX_MODE only supports data value with "
            "int8_t/uint8_t/half/bfloat16_t/int16_t/uint16_t/float/int32_t/uint32_t/int64_t/uint64_t types.");
        static_assert(
            SupportType<U, int32_t, uint32_t, int64_t, uint64_t>(),
            "Advanced Sort API RADIX_MODE only supports src index with int32_t/uint32_t/int64_t/uint64_t types.");
        constexpr bool needProcess = CheckDataProcess<T, isDescend>();
        using ConvTypeT = typename ::AscendC::Internal::ExtractTypeBySize<sizeof(T)>::T;
        using ConvTypeU = typename ::AscendC::Internal::ExtractTypeBySize<sizeof(U)>::T;
        uint32_t alignCount = AlignUp(count, ONE_BLK_SIZE);
        __ubuf__ ConvTypeT* src = (__ubuf__ ConvTypeT*)srcLocal.GetPhyAddr();
        __ubuf__ ConvTypeU* srcIndex = (__ubuf__ ConvTypeU*)srcIndexLocal.GetPhyAddr();
        __ubuf__ ConvTypeT* dst = (__ubuf__ ConvTypeT*)dstLocal.GetPhyAddr();
        __ubuf__ ConvTypeU* dstIndex = (__ubuf__ ConvTypeU*)dstIndexLocal.GetPhyAddr();
        __ubuf__ uint16_t *totalExclusiveSum, *tmpIndexU16;
        __ubuf__ uint8_t *srcU8, *srcU8Copy, *tmpIndexU8;
        __ubuf__ uint32_t* sortedLocalIndex;
        ArrangeCommonTmpBuffer(
            sharedTmpBuffer, alignCount, totalExclusiveSum, srcU8, tmpIndexU16, sortedLocalIndex, srcU8Copy,
            tmpIndexU8);
        __ubuf__ ConvTypeU* currSortedDstIndex =
            (__ubuf__ ConvTypeU*)((__ubuf__ uint8_t*)sortedLocalIndex + sizeof(uint32_t) * alignCount);
        __ubuf__ ConvTypeT* srcForNextRound =
            (__ubuf__ ConvTypeT*)((__ubuf__ uint8_t*)currSortedDstIndex + sizeof(ConvTypeU) * alignCount);
        __ubuf__ ConvTypeU* prevSortedDstIndex = dstIndex;
        __ubuf__ ConvTypeT* currSortedValue = srcForNextRound;
        __ubuf__ ConvTypeT* prevSortedValue = dst;
        __ubuf__ ConvTypeT* initSrcValue = src;
        __ubuf__ ConvTypeU* initSrcValueIndex = srcIndex;

        if constexpr (sizeof(T) == 1 && needProcess) {
            Internal::TwiddleInData<T, ConvTypeT, isDescend>(src, dst, count);
            initSrcValue = dst;
        } else {
            if constexpr (needProcess) {
                Internal::TwiddleInData<T, ConvTypeT, isDescend>(src, dst, count);
                initSrcValue = dst;
                if constexpr (isReuseSource) {
                    SaveBufferTo(srcIndex, dstIndex, count);
                    currSortedValue = src;
                    currSortedDstIndex = srcIndex;
                    initSrcValueIndex = dstIndex;
                }
            } else if constexpr (isReuseSource) {
                SaveBufferTo(src, dst, count);
                SaveBufferTo(srcIndex, dstIndex, count);
                initSrcValue = dst;
                initSrcValueIndex = dstIndex;
                currSortedValue = src;
                currSortedDstIndex = srcIndex;
            }
        }

        if constexpr (sizeof(T) == 1) {
            SortU8ElementsWithRound(
                initSrcValue, srcU8, srcU8Copy, totalExclusiveSum, tmpIndexU8, tmpIndexU16, sortedLocalIndex, count, 0);
            UpdateValueAndIndexByLocalIndex(src, srcIndex, sortedLocalIndex, dst, dstIndex, count);
        } else {
            SortU8ElementsWithRound(
                initSrcValue, srcU8, srcU8Copy, totalExclusiveSum, tmpIndexU8, tmpIndexU16, sortedLocalIndex, count, 0);
            UpdateValueAndIndexByLocalIndex(
                initSrcValue, initSrcValueIndex, sortedLocalIndex, currSortedValue, currSortedDstIndex, count);

            SwapBuffer(currSortedValue, prevSortedValue);
            SwapBuffer(currSortedDstIndex, prevSortedDstIndex);

            SortU8ElementsWithRound(
                prevSortedValue, srcU8, srcU8Copy, totalExclusiveSum, tmpIndexU8, tmpIndexU16, sortedLocalIndex, count,
                1);
            UpdateValueAndIndexByLocalIndex(
                prevSortedValue, prevSortedDstIndex, sortedLocalIndex, currSortedValue, currSortedDstIndex, count);

            if constexpr (sizeof(T) >= 4) {
                SwapBuffer(currSortedValue, prevSortedValue);
                SwapBuffer(currSortedDstIndex, prevSortedDstIndex);

                SortU8ElementsWithRound(
                    prevSortedValue, srcU8, srcU8Copy, totalExclusiveSum, tmpIndexU8, tmpIndexU16, sortedLocalIndex,
                    count, 2);
                UpdateValueAndIndexByLocalIndex(
                    prevSortedValue, prevSortedDstIndex, sortedLocalIndex, currSortedValue, currSortedDstIndex, count);

                SwapBuffer(currSortedValue, prevSortedValue);
                SwapBuffer(currSortedDstIndex, prevSortedDstIndex);

                SortU8ElementsWithRound(
                    prevSortedValue, srcU8, srcU8Copy, totalExclusiveSum, tmpIndexU8, tmpIndexU16, sortedLocalIndex,
                    count, 3);
                UpdateValueAndIndexByLocalIndex(
                    prevSortedValue, prevSortedDstIndex, sortedLocalIndex, currSortedValue, currSortedDstIndex, count);
            }

            if constexpr (sizeof(T) == 8) {
                SwapBuffer(currSortedValue, prevSortedValue);
                SwapBuffer(currSortedDstIndex, prevSortedDstIndex);

                SortU8ElementsWithRound(
                    prevSortedValue, srcU8, srcU8Copy, totalExclusiveSum, tmpIndexU8, tmpIndexU16, sortedLocalIndex,
                    count, 4);
                UpdateValueAndIndexByLocalIndex(
                    prevSortedValue, prevSortedDstIndex, sortedLocalIndex, currSortedValue, currSortedDstIndex, count);

                SwapBuffer(currSortedValue, prevSortedValue);
                SwapBuffer(currSortedDstIndex, prevSortedDstIndex);

                SortU8ElementsWithRound(
                    prevSortedValue, srcU8, srcU8Copy, totalExclusiveSum, tmpIndexU8, tmpIndexU16, sortedLocalIndex,
                    count, 5);
                UpdateValueAndIndexByLocalIndex(
                    prevSortedValue, prevSortedDstIndex, sortedLocalIndex, currSortedValue, currSortedDstIndex, count);

                SwapBuffer(currSortedValue, prevSortedValue);
                SwapBuffer(currSortedDstIndex, prevSortedDstIndex);

                SortU8ElementsWithRound(
                    prevSortedValue, srcU8, srcU8Copy, totalExclusiveSum, tmpIndexU8, tmpIndexU16, sortedLocalIndex,
                    count, 6);
                UpdateValueAndIndexByLocalIndex(
                    prevSortedValue, prevSortedDstIndex, sortedLocalIndex, currSortedValue, currSortedDstIndex, count);

                SwapBuffer(currSortedValue, prevSortedValue);
                SwapBuffer(currSortedDstIndex, prevSortedDstIndex);

                SortU8ElementsWithRound(
                    prevSortedValue, srcU8, srcU8Copy, totalExclusiveSum, tmpIndexU8, tmpIndexU16, sortedLocalIndex,
                    count, 7);
                UpdateValueAndIndexByLocalIndex(
                    prevSortedValue, prevSortedDstIndex, sortedLocalIndex, currSortedValue, currSortedDstIndex, count);
            }

            if constexpr (needProcess) {
                Internal::TwiddleOutData<T, ConvTypeT, isDescend>(dst, dst, count);
            }
        }
    }
}

template <typename T, bool isReuseSource, const SortConfig& config>
__aicore__ inline void SortImpl(LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }

    // Using the Stack Space to Allocate tmpBuffer
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    SortImpl<T, isReuseSource, config>(dstLocal, srcLocal, sharedTmpBuffer, calCount);
}

template <typename T, bool isReuseSource, const SortConfig& config>
__aicore__ inline void SortImpl(
    LocalTensor<T>& dstLocal, LocalTensor<uint32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal,
    const uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }

    // Using the Stack Space to Allocate tmpBuffer
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    SortImpl<T, isReuseSource, config>(dstLocal, dstIndexLocal, srcLocal, sharedTmpBuffer, calCount);
}

template <typename T, typename U, bool isReuseSource, const SortConfig& config>
__aicore__ inline void SortImpl(
    const LocalTensor<T>& dstLocal, const LocalTensor<U>& dstIndexLocal, const LocalTensor<T>& srcLocal,
    const LocalTensor<U>& srcIndexLocal, const uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }

    // Using the Stack Space to Allocate tmpBuffer
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    SortImpl<T, U, isReuseSource, config>(dstLocal, dstIndexLocal, srcLocal, srcIndexLocal, sharedTmpBuffer, calCount);
}
} // namespace internal
} // namespace Reg
}; // namespace AscendC
#endif // IMPL_SORT_SORT_SORT_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SORT_SORT_SORT_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SORT_SORT_SORT_IMPL_H__
#endif
