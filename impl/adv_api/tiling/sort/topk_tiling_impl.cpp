/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "adv_api/utils/types.h"
#include "adv_api/sort/topk_tilingdata.h"
#include "adv_api/sort/topk_tiling.h"

#include <set>
#include <map>
#include "adv_api/sort/sort_tiling_intf.h"
#include "../../detail/host_log.h"
#include "utils/tiling/platform/platform_ascendc.h"
#include "register/tilingdata_base.h"

namespace optiling {
REGISTER_TILING_DATA_CLASS(TopkTilingOpApi, TopkTiling)
}

namespace AscendC {
namespace {
constexpr uint32_t TOPK_CALC_FAC = 16;
constexpr uint32_t TOPK_HALF_SIZE = 2;
constexpr uint32_t TOPK_FLOAT_SIZE = 4;
constexpr uint32_t TOPK_INT32_SIZE = 4;
constexpr uint32_t BINARY_10 = 2;
constexpr uint32_t BINARY_0001 = 4;
constexpr uint32_t BINARY_THIRTYTWO = 2863311530;
constexpr uint32_t BINARY_SIXTEEN = 4369;
constexpr int32_t FIFTEEN = 15;
constexpr int32_t SIXTEEN = 16;
constexpr int32_t SEVENTEEN = 17;
constexpr int32_t THIRTYTWO = 32;
constexpr int32_t TWO = 2;
constexpr int32_t FOUR = 4;
constexpr int32_t FIVE = 5;
constexpr int32_t SIX = 6;
constexpr int32_t SEVEN = 7;
constexpr int32_t EIGHT = 8;
constexpr int32_t NINE = 9;
constexpr int32_t TWELVE = 12;
constexpr int32_t THIRTEEN = 13;
constexpr int32_t TWENTY = 20;
constexpr int32_t TWENTYONE = 21;
constexpr int32_t TWENTYFOUR = 24;
constexpr int32_t TWENTYFIVE = 25;
constexpr int32_t TWENTYEIGHT = 28;
constexpr int32_t TWENTYNINE = 29;
constexpr int32_t THIRTYTHREE = 33;
// (10 * sizeof(float) + 31) // 32 * 32
constexpr int32_t TOPKNSMALL_MASK_BYTES = 64;
constexpr int32_t SORT32_ONE_NUM_BYTES = 8;
constexpr int32_t ONE_BLK_SIZE = 32;
constexpr int32_t PROPOSAL_CONTAIN_NUMBER = 8;
constexpr int32_t MIN_RPSORT16_SIZE = 16;
// 1111 1111 1111 1111
constexpr uint32_t BINARY_SIXTEEN_ONE = 65535;
constexpr int32_t TOPK_RADIX_BUCKET_SIZE = 256;
constexpr int32_t TOPK_RADIX_LOAD_COUNT_PER_TIME = 256;
constexpr uint32_t TOPK_RADIX_B8_SIZE = 1;
constexpr uint32_t TOPK_RADIX_B16_SIZE = 2;
constexpr uint32_t TOPK_RADIX_B32_SIZE = 4;
constexpr uint32_t TOPK_RADIX_B64_SIZE = 8;

uint32_t GenerateBinary1(int32_t k)
{
    uint32_t m = 0;
    for (int i = 0; i < k; ++i) {
        m = (m << 1u) | 1u;
    }
    return m;
}

uint32_t GenerateBinary10(int32_t k)
{
    uint32_t m = BINARY_10; // 10
    for (int i = 1; i < k; ++i) {
        m = (m << BINARY_10) | m;
    }
    return m;
}

uint32_t GenerateBinary01(int32_t k)
{
    uint32_t m = 1; // 01
    for (int i = 1; i < k; ++i) {
        m = (m << BINARY_10) | m;
    }
    return m;
}

uint16_t GenerateBinary0001(int32_t k)
{
    uint16_t m = 1; // 0001
    for (int i = 1; i < k; ++i) {
        m = (m << BINARY_0001) | m;
    }
    return m;
}

void GetVreduceMask(const int32_t k, const uint32_t dataTypeSize, optiling::TopkTiling& topKTiling)
{
    if (k < SEVENTEEN) {
        topKTiling.set_vreduceIdxMask0(GenerateBinary10(k));
    }
    if (k >= SEVENTEEN && k <= THIRTYTWO) {
        topKTiling.set_vreduceIdxMask0(BINARY_THIRTYTWO);
        topKTiling.set_vreduceIdxMask1(GenerateBinary10(k - SIXTEEN));
    }
    if (dataTypeSize == TOPK_FLOAT_SIZE) {
        if (k < SEVENTEEN) {
            topKTiling.set_vreduceValMask0(GenerateBinary01(k));
        }
        if (k >= SEVENTEEN && k <= THIRTYTWO) {
            topKTiling.set_vreduceValMask0(GenerateBinary01(SIXTEEN));
            topKTiling.set_vreduceValMask1(GenerateBinary01(k - SIXTEEN));
        }
    } else {
        topKTiling.set_vreducehalfValMask0((k < FIVE) ? GenerateBinary0001(k) : BINARY_SIXTEEN);
        topKTiling.set_vreducehalfValMask1((k >= FIVE) ? (GenerateBinary0001((k >= NINE) ? FOUR : (k - FOUR))) : 0);
        topKTiling.set_vreducehalfValMask2(
            (k >= NINE) ? (GenerateBinary0001((k >= THIRTEEN) ? FOUR : (k - EIGHT))) : 0);
        topKTiling.set_vreducehalfValMask3(
            (k >= THIRTEEN) ? (GenerateBinary0001((k >= SEVENTEEN) ? FOUR : (k - TWELVE))) : 0);
        topKTiling.set_vreducehalfValMask4(
            (k >= SEVENTEEN) ? (GenerateBinary0001((k >= TWENTYONE) ? FOUR : (k - SIXTEEN))) : 0);
        topKTiling.set_vreducehalfValMask5(
            (k >= TWENTYONE) ? (GenerateBinary0001((k >= TWENTYFIVE) ? FOUR : (k - TWENTY))) : 0);
        topKTiling.set_vreducehalfValMask6(
            (k >= TWENTYFIVE) ? (GenerateBinary0001((k >= TWENTYNINE) ? FOUR : (k - TWENTYFOUR))) : 0);
        topKTiling.set_vreducehalfValMask7(
            (k >= TWENTYNINE) ? (GenerateBinary0001((k >= THIRTYTHREE) ? FOUR : (k - TWENTYEIGHT))) : 0);
    }
}

void GetTopKMaxMinTmpSize310(
    const int32_t inner, const int32_t outter, const bool isInitIndex, enum TopKMode mode, uint32_t& maxValue,
    uint32_t& minValue)
{
    // total：(Inner*8B)*2
    if (mode == TopKMode::TOPK_NORMAL) {
        maxValue = TOPK_CALC_FAC * static_cast<uint32_t>(inner);
        if (!isInitIndex) {
            // isInitIndex is false: inner * sizeof(int32_t)
            maxValue += inner * sizeof(int32_t);
        }
    } else if (mode == TopKMode::TOPK_NSMALL) {
        // Sort32: inner * outter * EIGHT
        maxValue = inner * outter * EIGHT;
        // isInitIndex is false: inner * outter * sizeof(int32_t)
        if (!isInitIndex) {
            maxValue += inner * outter * sizeof(int32_t);
        }
    }
    minValue = maxValue;
}

void GetTopKMaxMinTmpSize220(
    const int32_t inner, const int32_t outter, const bool isInitIndex, enum TopKMode mode, uint32_t& maxValue,
    uint32_t& minValue, const bool isLargest)
{
    // total：(Inner*4+inner*4)*2
    if ((mode == TopKMode::TOPK_NORMAL) && (isInitIndex)) {
        minValue = TOPK_CALC_FAC * static_cast<uint32_t>(inner);
        maxValue = minValue;
    }
    if ((mode == TopKMode::TOPK_NORMAL) && (!isInitIndex)) {
        minValue = (TOPK_CALC_FAC + TOPK_INT32_SIZE) * static_cast<uint32_t>(inner);
        maxValue = minValue;
    }

    if (!isLargest) {
        if ((mode == TopKMode::TOPK_NSMALL) && (isInitIndex)) {
            // Sort32: inner * outter * EIGHT
            // islargest is false: inner * outter * TOPK_FLOAT_SIZE
            // vreducev2 block bytes: TOPKNSMALL_MASK_BYTES
            minValue = static_cast<uint32_t>(
                inner * outter * EIGHT + inner * outter * TOPK_FLOAT_SIZE + TOPKNSMALL_MASK_BYTES);
            maxValue = minValue;
        }
        if ((mode == TopKMode::TOPK_NSMALL) && (!isInitIndex)) {
            // Sort32: inner * outter * EIGHT
            // islargest is false: inner * outter * TOPK_FLOAT_SIZE
            // vreducev2 block bytes: TOPKNSMALL_MASK_BYTES
            // isInitIndex is false: inner * outter * TOPK_FLOAT_SIZE
            minValue = static_cast<uint32_t>(
                inner * outter * EIGHT + inner * outter * TOPK_FLOAT_SIZE + TOPKNSMALL_MASK_BYTES +
                inner * outter * TOPK_FLOAT_SIZE);
            maxValue = minValue;
        }
    } else {
        if ((mode == TopKMode::TOPK_NSMALL) && (isInitIndex)) {
            minValue = static_cast<uint32_t>(inner * outter * EIGHT + TOPKNSMALL_MASK_BYTES);
            maxValue = minValue;
        }
        if ((mode == TopKMode::TOPK_NSMALL) && (!isInitIndex)) {
            minValue = static_cast<uint32_t>(
                inner * outter * EIGHT + TOPKNSMALL_MASK_BYTES + inner * outter * TOPK_FLOAT_SIZE);
            maxValue = minValue;
        }
    }
}

void GetTopKMaxMinTmpSize200(
    const int32_t inner, const int32_t outter, enum TopKMode mode, uint32_t& maxValue, uint32_t& minValue,
    const int32_t dataTypeSize)
{
    if (mode == TopKMode::TOPK_NORMAL) {
        // proposal space Bytes: inner * 8 * dataTypeSize
        // MrgSort4 space Bytes: inner * 8 * dataTypeSize * 2
        // index space Bytes: inner * sizeof(int32_t)
        maxValue = minValue =
            static_cast<uint32_t>((inner * PROPOSAL_CONTAIN_NUMBER * dataTypeSize) * TWO + inner * sizeof(int32_t));
    }
    if (mode == TopKMode::TOPK_NSMALL) {
        // proposal space Bytes: inner * outter * 8 * dataTypeSize
        // MrgSort4 space Bytes: inner * outter * 8 * dataTypeSize * 2
        maxValue = minValue = static_cast<uint32_t>(inner * outter * PROPOSAL_CONTAIN_NUMBER * dataTypeSize * TWO);
    }
}

void SetTopkNormalVal310(
    const int32_t inner, const int32_t outter, const int32_t k, const uint32_t dataTypeSize, const bool isInitIndex,
    optiling::TopkTiling& topKTiling)
{
    const int32_t kAlignFourBytesTmp = ((k + SEVEN) / EIGHT) * EIGHT;
    topKTiling.set_kAlignFourBytes(kAlignFourBytesTmp);
    const int32_t kAlignTwoBytesTmp = ((k + FIFTEEN) / SIXTEEN) * SIXTEEN;
    topKTiling.set_sortRepeat(static_cast<uint32_t>(inner / THIRTYTWO));
    topKTiling.set_innerDataSize(inner * (SORT32_ONE_NUM_BYTES / dataTypeSize));
    topKTiling.set_kAlignTwoBytes(kAlignTwoBytesTmp);
    const int32_t maskOffsetFloat = outter * kAlignFourBytesTmp;
    const int32_t maskOffsetHalf = outter * kAlignTwoBytesTmp;
    if (dataTypeSize == TOPK_HALF_SIZE) {
        topKTiling.set_maskOffset(maskOffsetHalf);
    } else {
        topKTiling.set_maskOffset(maskOffsetFloat);
    }
    topKTiling.set_maskVreducev2FourBytes(TWO * k);
    topKTiling.set_maskVreducev2TwoBytes(FOUR * k);
    // inner * 16 / sizeof(half) = inner * 8
    // inner * 16 / sizeof(float) = inner * 4
    topKTiling.set_tmpLocalSize(inner * SIXTEEN / dataTypeSize);
    if (!isInitIndex) {
        topKTiling.set_srcIndexOffset(inner * SIXTEEN / dataTypeSize);
        // Space required for generate indexs: inner * sizeof(int32_t) / dataTypeSize
        topKTiling.set_tmpLocalSize(inner * (SIXTEEN + sizeof(int32_t)) / dataTypeSize);
    }
}

void SetTopkNormalVal(
    const int32_t inner, const int32_t outter, const int32_t k, const uint32_t dataTypeSize, const bool isInitIndex,
    const int32_t kAlignFourBytesTmp, optiling::TopkTiling& topKTiling)
{
    const int32_t kAlignTwoBytesTmp = ((k + FIFTEEN) / SIXTEEN) * SIXTEEN;
    // inner * 16 / sizeof(float) = inner * 4
    topKTiling.set_tmpLocalSize(FOUR * inner);
    topKTiling.set_innerDataSize(inner * TWO);
    const uint32_t copyUbufToUbufBlockCount = static_cast<uint32_t>(inner * 2 * TOPK_FLOAT_SIZE / ONE_BLK_SIZE);
    topKTiling.set_copyUbToUbBlockCount(copyUbufToUbufBlockCount);
    topKTiling.set_sortRepeat(static_cast<uint32_t>(inner / THIRTYTWO));
    topKTiling.set_mrgSortRepeat(inner / FOUR);
    topKTiling.set_kAlignTwoBytes(kAlignTwoBytesTmp);
    const int32_t maskOffsetFloat = outter * kAlignFourBytesTmp;
    topKTiling.set_maskOffset(maskOffsetFloat);
    topKTiling.set_maskVreducev2FourBytes(TWO * k);
    topKTiling.set_maskVreducev2TwoBytes(FOUR * k);
    // float: 1 * 2
    topKTiling.set_mrgSortSrc1offset(TWO);
    // float: 2 * 2
    topKTiling.set_mrgSortSrc2offset(FOUR);
    // float: 3 * 2
    topKTiling.set_mrgSortSrc3offset(SIX);
    topKTiling.set_mrgSortTwoQueueSrc1Offset(TWO);
    topKTiling.set_mrgFourQueueTailPara1(inner * TWO);
    topKTiling.set_mrgFourQueueTailPara2(sizeof(float) / dataTypeSize);
    const int32_t maskOffsetHalf = outter * kAlignTwoBytesTmp;
    if (dataTypeSize == TOPK_HALF_SIZE) {
        // inner * 16 / sizeof(half) = inner * 8
        topKTiling.set_tmpLocalSize(EIGHT * inner);
        topKTiling.set_maskOffset(maskOffsetHalf);
        topKTiling.set_innerDataSize(inner * FOUR);
        // 2 * 2
        topKTiling.set_mrgSortSrc1offset(FOUR);
        // 2 * 2 * 2
        topKTiling.set_mrgSortSrc2offset(EIGHT);
        // 3 * 2 * 2
        topKTiling.set_mrgSortSrc3offset(TWELVE);
        topKTiling.set_mrgSortTwoQueueSrc1Offset(FOUR);
    }
    if (!isInitIndex) {
        topKTiling.set_tmpLocalSize(FOUR * inner + inner);
        topKTiling.set_srcIndexOffset(FOUR * inner);
        if (dataTypeSize == TOPK_HALF_SIZE) {
            topKTiling.set_tmpLocalSize(EIGHT * inner + inner * TWO);
            topKTiling.set_srcIndexOffset(EIGHT * inner);
        }
    }
}

void SetTopkNSmallVal310(
    const int32_t inner, const int32_t outter, const int32_t k, const uint32_t dataTypeSize, const bool isInitIndex,
    optiling::TopkTiling& topKTiling)
{
    const int32_t innerDataSize = inner * outter * (SORT32_ONE_NUM_BYTES / dataTypeSize);
    topKTiling.set_innerDataSize(innerDataSize);
    topKTiling.set_maskOffset(outter * k);
    topKTiling.set_tmpLocalSize(innerDataSize);
    if (!isInitIndex) {
        // Space required for generate indexs: inner * outter * sizeof(int32_t) / dataTypeSize
        topKTiling.set_tmpLocalSize(innerDataSize + inner * outter * sizeof(int32_t) / dataTypeSize);
    }
}

void SetTopkNSmallVal(
    const int32_t inner, const int32_t outter, const int32_t k, const uint32_t dataTypeSize, const bool isInitIndex,
    optiling::TopkTiling& topKTiling, const bool isLargest = false)
{
    int32_t innerDataSize = inner * outter * (SORT32_ONE_NUM_BYTES / dataTypeSize);
    topKTiling.set_innerDataSize(innerDataSize);
    topKTiling.set_maskOffset(outter * k);
    int32_t generateNegativeValSize = 0;
    if (!isLargest) {
        generateNegativeValSize = inner * outter;
    }
    int32_t mrgSrc1MaskSizeOffset =
        (innerDataSize) + generateNegativeValSize; // sort32 sort8B + asc(vmuls * -1) + mask space + generate index
    topKTiling.set_topkMrgSrc1MaskSizeOffset(mrgSrc1MaskSizeOffset);
    topKTiling.set_tmpLocalSize(mrgSrc1MaskSizeOffset + TOPKNSMALL_MASK_BYTES / dataTypeSize);
    if (!isInitIndex) {
        int32_t srcIndexLocalOffset =
            mrgSrc1MaskSizeOffset + TOPKNSMALL_MASK_BYTES / dataTypeSize; // the space required by mask
        // Space required for generate indexs: inner * outter * TOPK_FLOAT_SIZE / dataTypeSize
        topKTiling.set_tmpLocalSize(srcIndexLocalOffset + inner * outter * TOPK_FLOAT_SIZE / dataTypeSize);
        topKTiling.set_topkNSmallSrcIndexOffset(srcIndexLocalOffset);
    }
    GetVreduceMask(k, dataTypeSize, topKTiling);
}

void SetTopkNormalVal200(
    const int32_t inner, const int32_t outter, const int32_t k, const uint32_t dataTypeSize,
    optiling::TopkTiling& topKTiling)
{
    // proposal space contained elements: inner * 8
    // MrgSort4 space contained elements: inner * 8 * 2
    // index space contained elements: inner * sizeof(int32_t) / sizeof(T)
    int32_t mrgSort4Size = inner * PROPOSAL_CONTAIN_NUMBER * TWO;
    if (dataTypeSize != 0) {
        topKTiling.set_tmpLocalSize(mrgSort4Size + inner * sizeof(int32_t) / dataTypeSize);
    }
    topKTiling.set_srcIndexOffset(mrgSort4Size);
    int32_t innerProposalSize = inner * PROPOSAL_CONTAIN_NUMBER;
    topKTiling.set_innerDataSize(innerProposalSize);
    topKTiling.set_sortRepeat(inner / MIN_RPSORT16_SIZE);
    const uint32_t copyUbufToUbufBlockCount = static_cast<uint32_t>(innerProposalSize * dataTypeSize / ONE_BLK_SIZE);
    topKTiling.set_copyUbToUbBlockCount(copyUbufToUbufBlockCount);
    // float: 1 * 8
    topKTiling.set_mrgSortSrc1offset(PROPOSAL_CONTAIN_NUMBER);
    // float: 2 * 8
    topKTiling.set_mrgSortSrc2offset(2 * PROPOSAL_CONTAIN_NUMBER);
    // float: 3 * 8
    topKTiling.set_mrgSortSrc3offset(3 * PROPOSAL_CONTAIN_NUMBER);
    topKTiling.set_mrgSortRepeat(inner / FOUR);
    const int32_t kAlignFourBytesTmp = ((k + SEVEN) / EIGHT) * EIGHT;
    const int32_t kAlignTwoBytesTmp = ((k + FIFTEEN) / SIXTEEN) * SIXTEEN;
    const int32_t maskOffsetFloat = outter * kAlignFourBytesTmp;
    const int32_t maskOffsetHalf = outter * kAlignTwoBytesTmp;
    topKTiling.set_maskOffset(maskOffsetFloat);
    if (dataTypeSize == TOPK_HALF_SIZE) {
        topKTiling.set_maskOffset(maskOffsetHalf);
    }
    topKTiling.set_kAlignTwoBytes(kAlignTwoBytesTmp);
    topKTiling.set_kAlignFourBytes(kAlignFourBytesTmp);
}

void SetTopkNSmallVal200(
    const int32_t inner, const int32_t outter, const int32_t k, const uint32_t dataTypeSize,
    optiling::TopkTiling& topKTiling)
{
    // proposal space contained elements: inner * outter * 8
    // MrgSort4 space contained elements: inner * outter * 8 * 2
    int32_t proposalSize = inner * outter * PROPOSAL_CONTAIN_NUMBER;
    int32_t mrgSort4Size = proposalSize * TWO;
    topKTiling.set_tmpLocalSize(mrgSort4Size);
    int32_t topkNSmallSrcIndexOffset = proposalSize + inner * outter;
    topKTiling.set_topkNSmallSrcIndexOffset(topkNSmallSrcIndexOffset);
    topKTiling.set_innerDataSize(proposalSize);
    topKTiling.set_maskOffset(outter * k);
    topKTiling.set_allDataSize(inner * outter);
    topKTiling.set_sortRepeat(static_cast<uint32_t>(inner * outter / MIN_RPSORT16_SIZE));
    const uint32_t copyUbufToUbufBlockCount = static_cast<uint32_t>(proposalSize * dataTypeSize / ONE_BLK_SIZE);
    topKTiling.set_copyUbToUbBlockCount(copyUbufToUbufBlockCount);
    // mrgSortTwoQueueSrc1Offset: 16 * 8 * 1
    topKTiling.set_mrgSortTwoQueueSrc1Offset(MIN_RPSORT16_SIZE * PROPOSAL_CONTAIN_NUMBER);
    topKTiling.set_mrgSortSrc1offset(inner * PROPOSAL_CONTAIN_NUMBER);
    topKTiling.set_vreduceIdxMask0(GenerateBinary1(k));
    if (dataTypeSize == sizeof(float)) {
        // Cast: don't need half->float, so the source address and destination address can overlap
        topKTiling.set_srcIndexOffset(topkNSmallSrcIndexOffset);
        topKTiling.set_vreduceValMask0(GenerateBinary1(k));
    } else {
        // Cast: half->float, the source address and destination address cannot overlap
        topKTiling.set_srcIndexOffset(topkNSmallSrcIndexOffset + inner * outter);
        if (k <= SIXTEEN) {
            topKTiling.set_vreduceValMask0(GenerateBinary1(k));
        } else if (k <= THIRTYTWO) {
            topKTiling.set_vreduceValMask0(BINARY_SIXTEEN_ONE);
            topKTiling.set_vreduceValMask1(GenerateBinary1(k - SIXTEEN));
        }
    }
}

bool TopKTilingFunc(
    const int32_t inner, const int32_t outter, const int32_t k, const uint32_t dataTypeSize, const bool isInitIndex,
    enum TopKMode mode, optiling::TopkTiling& topKTiling, const bool isLargest, const NpuArch npuArch)
{
    if (dataTypeSize == 0) {
        return false;
    }
    if (npuArch == NpuArch::DAV_2002) {
        if (mode == TopKMode::TOPK_NORMAL) {
            SetTopkNormalVal200(inner, outter, k, dataTypeSize, topKTiling);
        } else {
            SetTopkNSmallVal200(inner, outter, k, dataTypeSize, topKTiling);
        }
    } else if (
        npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201 || npuArch == NpuArch::DAV_5102 ||
        npuArch == NpuArch::DAV_3003) {
        topKTiling.set_allDataSize(inner * outter);
        if (mode == TopKMode::TOPK_NORMAL) {
            SetTopkNormalVal310(inner, outter, k, dataTypeSize, isInitIndex, topKTiling);
        } else {
            SetTopkNSmallVal310(inner, outter, k, dataTypeSize, isInitIndex, topKTiling);
        }
    } else {
        topKTiling.set_allDataSize(inner * outter);
        const int32_t kAlignFourBytesTmp = ((k + SEVEN) / EIGHT) * EIGHT;
        topKTiling.set_kAlignFourBytes(kAlignFourBytesTmp);
        // TOPK_NORMAL = 0, TOPK_NSMALL = 1
        if (mode == TopKMode::TOPK_NORMAL) {
            SetTopkNormalVal(inner, outter, k, dataTypeSize, isInitIndex, kAlignFourBytesTmp, topKTiling);
        } else {
            SetTopkNSmallVal(inner, outter, k, dataTypeSize, isInitIndex, topKTiling, isLargest);
        }
    }
    return true;
}

void CheckTopKHostCommon(
    const char* apiName, const char* hostFuncName, const platform_ascendc::PlatformAscendC& ascendcPlatform,
    const int32_t inner, const int32_t outter, const bool isInitIndex, enum TopKMode mode, const uint32_t dataTypeSize)
{
    const auto npuArch = ascendcPlatform.GetCurNpuArch();
    ASCENDC_HOST_ASSERT(
        inner > 0, return, "[%s][%s] The length of the inner axis must be greater than 0!", apiName, hostFuncName);
    ASCENDC_HOST_ASSERT(
        inner % 32 == 0, return, "[%s][%s] The length of the inner axis must be a multiple of 32!", apiName,
        hostFuncName);
    if (mode == TopKMode::TOPK_NSMALL) {
        ASCENDC_HOST_ASSERT(
            inner == 32, return, "[%s][%s] In Small mode, the length of the inner axis must be 32!", apiName,
            hostFuncName);
    }
    if (npuArch == NpuArch::DAV_2002 && isInitIndex == false) {
        ASCENDC_HOST_ASSERT(
            inner <= 2048, return,
            "[%s][%s] In Atlas inference products, when the data type is half and the parameter isInitIndex "
            "is set to false, the inner axis length must be less than or equal to 2048!",
            apiName, hostFuncName);
    }
    ASCENDC_HOST_ASSERT(
        outter > 0, return, "[%s][%s] The length of the outter axis must be greater than 0!", apiName, hostFuncName);
    uint64_t ubSize = 0;
    ascendcPlatform.GetCoreMemSize(platform_ascendc::CoreMemType::UB, ubSize);
    ASCENDC_HOST_ASSERT(
        static_cast<uint64_t>(inner * outter * dataTypeSize) <= ubSize, return,
        "[%s][%s] The size of srcShape is %luB, should be less than UB size.", apiName, hostFuncName,
        static_cast<uint64_t>(inner * outter * dataTypeSize));
    ASCENDC_HOST_ASSERT(
        dataTypeSize == TOPK_HALF_SIZE || dataTypeSize == TOPK_FLOAT_SIZE, return,
        "[%s][%s] Type size %u is unsupported!", apiName, hostFuncName, dataTypeSize);
    return;
}
} // namespace

bool GetTopKMaxMinTmpSize(
    const platform_ascendc::PlatformAscendC& ascendcPlatform, const int32_t inner, const int32_t outter,
    const bool isReuseSource, const bool isInitIndex, enum TopKMode mode, const bool isLargest,
    const uint32_t dataTypeSize, uint32_t& maxValue, uint32_t& minValue)
{
    (void)isReuseSource;
    CheckTopKHostCommon(
        "TopK", "GetTopKMaxMinTmpSize", ascendcPlatform, inner, outter, isInitIndex, mode, dataTypeSize);
    const auto npuArch = ascendcPlatform.GetCurNpuArch();
    if (npuArch == NpuArch::DAV_2002) {
        GetTopKMaxMinTmpSize200(inner, outter, mode, maxValue, minValue, dataTypeSize);
    } else if (
        npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201 || npuArch == NpuArch::DAV_5102 ||
        npuArch == NpuArch::DAV_3003) {
        GetTopKMaxMinTmpSize310(inner, outter, isInitIndex, mode, maxValue, minValue);
    } else {
        GetTopKMaxMinTmpSize220(inner, outter, isInitIndex, mode, maxValue, minValue, isLargest);
    }
    return true;
}

bool GetTopKMaxMinTmpSize(
    const int32_t inner, const int32_t outter, const int32_t k, const bool isReuseSource, const bool isInitIndex,
    enum TopKMode mode, const bool isLargest, AscendC::TensorDataType dataType, const TopKConfig& config,
    uint32_t& maxValue, uint32_t& minValue)
{
    platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
    ASCENDC_HOST_ASSERT((platform != nullptr), return false, "Failed to get PlatformAscendC");

    auto npuArch = platform->GetCurNpuArch();
    ASCENDC_HOST_ASSERT(
        (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201 || npuArch == NpuArch::DAV_5102), return false,
        "Unsupported NpuArch of Topk radix select API.");

    ASCENDC_HOST_ASSERT((inner % 32 == 0), return false, "The value of inner must be an integer multiple of 32.");
    ASCENDC_HOST_ASSERT(
        (1 <= k) && (k <= inner), return false,
        "The value of k must be greater than or equal to 1 and less than or equal to inner.");

    std::map<AscendC::TensorDataType, uint32_t> supportTypes = {
        {ge::DT_INT8, TOPK_RADIX_B8_SIZE},   {ge::DT_UINT8, TOPK_RADIX_B8_SIZE},
        {ge::DT_INT16, TOPK_RADIX_B16_SIZE}, {ge::DT_UINT16, TOPK_RADIX_B16_SIZE},
        {ge::DT_INT32, TOPK_RADIX_B32_SIZE}, {ge::DT_UINT32, TOPK_RADIX_B32_SIZE},
        {ge::DT_INT64, TOPK_RADIX_B64_SIZE}, {ge::DT_UINT64, TOPK_RADIX_B64_SIZE},
        {ge::DT_FLOAT, TOPK_RADIX_B32_SIZE}, {ge::DT_FLOAT16, TOPK_RADIX_B16_SIZE},
        {ge::DT_BF16, TOPK_RADIX_B16_SIZE},
    };
    std::set<AscendC::TensorDataType> twiddleTypes = {ge::DT_INT8,  ge::DT_INT16,   ge::DT_INT32, ge::DT_INT64,
                                                      ge::DT_FLOAT, ge::DT_FLOAT16, ge::DT_BF16};

    auto typeSizeData = supportTypes.find(dataType);
    ASCENDC_HOST_ASSERT(
        typeSizeData != supportTypes.end(), return false, "Unsupported valueType of TopK radix select API.");

    auto dataTypeSize = typeSizeData->second;
    uint32_t tmpBufferSize =
        dataTypeSize * inner + std::max(FOUR * inner, static_cast<int32_t>(sizeof(uint16_t) * TOPK_RADIX_BUCKET_SIZE));
    if (dataTypeSize == EIGHT) {
        tmpBufferSize = dataTypeSize * std::max(inner, TOPK_RADIX_LOAD_COUNT_PER_TIME) +
                        std::max(FOUR * inner, static_cast<int32_t>(sizeof(uint16_t) * TOPK_RADIX_BUCKET_SIZE));
    }

    uint32_t twiddleBufferSize = 0;
    if (!isReuseSource && (twiddleTypes.find(dataType) != twiddleTypes.end() || !isLargest)) {
        twiddleBufferSize = dataTypeSize * inner;
    }

    uint32_t initBufferSize = 0;
    if (!isInitIndex) {
        if (mode == TopKMode::TOPK_NORMAL) {
            initBufferSize = FOUR * inner;
        } else if (mode == TopKMode::TOPK_NSMALL) {
            initBufferSize = FOUR * inner * outter;
        }
    }

    uint32_t minSortMem = 0;
    uint32_t maxSortMem = 0;
    if (config.sorted) {
        std::vector<int64_t> shapeDims = {1, k};
        auto srcShape = AscendC::TensorShape(shapeDims);
        auto indexType = ge::DT_INT32;
        SortConfig sortConfig;
        sortConfig.isDescend = true;
        sortConfig.hasSrcIndex = true;
        sortConfig.hasDstIndex = true;
        GetSortMaxMinTmpSize(srcShape, dataType, indexType, false, sortConfig, maxSortMem, minSortMem);
    }

    minValue = tmpBufferSize + twiddleBufferSize + initBufferSize + minSortMem;
    maxValue = tmpBufferSize + twiddleBufferSize + initBufferSize + maxSortMem;
    return true;
}

bool TopKTilingFunc(
    const platform_ascendc::PlatformAscendC& ascendcPlatform, const int32_t inner, const int32_t outter,
    const int32_t k, const uint32_t dataTypeSize, const bool isInitIndex, enum TopKMode mode, const bool isLargest,
    optiling::TopkTiling& topKTiling)
{
    CheckTopKHostCommon("TopK", "TopKTilingFunc", ascendcPlatform, inner, outter, isInitIndex, mode, dataTypeSize);
    ASCENDC_HOST_ASSERT(
        k >= 1 && k <= inner, continue, "[TopK][TopKTilingFunc] The range of value k is [1, %d]!", inner);
    const auto npuArch = ascendcPlatform.GetCurNpuArch();
    return TopKTilingFunc(inner, outter, k, dataTypeSize, isInitIndex, mode, topKTiling, isLargest, npuArch);
}

bool TopKTilingFunc(
    const platform_ascendc::PlatformAscendC& ascendcPlatform, const int32_t inner, const int32_t outter,
    const int32_t k, const uint32_t dataTypeSize, const bool isInitIndex, enum TopKMode mode, const bool isLargest,
    AscendC::tiling::TopkTiling& topKTiling)
{
    CheckTopKHostCommon("TopK", "TopKTilingFunc", ascendcPlatform, inner, outter, isInitIndex, mode, dataTypeSize);
    ASCENDC_HOST_ASSERT(
        k >= 1 && k <= inner, continue, "[TopK][TopKTilingFunc] The range of value k is [1, %d]!", inner);
    const auto npuArch = ascendcPlatform.GetCurNpuArch();
    optiling::TopkTiling tilingData;
    bool ret = TopKTilingFunc(inner, outter, k, dataTypeSize, isInitIndex, mode, tilingData, isLargest, npuArch);
    tilingData.SaveToBuffer(&topKTiling, sizeof(TopkTiling));
    return ret;
}
} // namespace AscendC
