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
 * \file matmul_tiling_base.cpp
 * \brief
 */
#include "adv_api/matmul/matmul_tiling_base.h"

#include <iostream>
#include <algorithm>

#include "../../detail/host_log.h"
#include "math_util.h"

using namespace std;

namespace optiling {
REGISTER_TILING_DATA_CLASS(TCubeTilingOpApi, TCubeTiling);
}

namespace matmul_tiling {
constexpr int32_t MIN_MNK_SIZE = 16;
constexpr int32_t ALIGN_SIZE = 32;

// for ascend910b
constexpr int32_t L1_SIZE = 512 * 1024 - 256;
constexpr int32_t L0C_SIZE = 128 * 1024;
constexpr int32_t L0A_SIZE = 64 * 1024;
constexpr int32_t L0B_SIZE = 64 * 1024;
// ascend310B & ascend910B BT size
constexpr int32_t BT_SIZE = 1024;
constexpr int32_t UB_SIZE = 192 * 1024 - 256;

#define CHECK_TILING_PARAMETER(val, debugInfo)                                                           \
    TILING_LOG_DEBUG("%s: %d", debugInfo, val);                                                          \
    if ((val) <= 0) {                                                                                    \
        TILING_LOG_DEBUG("The input %s size should be greater than 0, currently, it is %d.", #val, val); \
    }

MatmulApiTilingBase::MatmulApiTilingBase()
{
    this->aType_.isDB = true;
    this->bType_.isDB = true;
    this->cType_.isDB = true;
    this->biasType_.isDB = true;

    this->aType_.isTrans = false;
    this->bType_.isTrans = false;

    this->isBias = false;
    this->madType_ = MatrixMadType::NORMAL;

    this->singleM = -1;
    this->singleN = -1;
    this->singleK = -1;
    this->singleCoreM = -1;
    this->singleCoreN = -1;
    this->singleCoreK = -1;
    this->orgM = -1;
    this->orgN = -1;
    this->orgKa = -1;
    this->orgKb = -1;
    this->baseM = -1;
    this->baseN = -1;
    this->baseK = -1;

    this->adjust_.maxBaseM = 0x7FFFFFFF;
    this->adjust_.maxBaseN = 0x7FFFFFFF;
    this->adjust_.maxBaseK = 0x7FFFFFFF;
    this->adjust_.minBaseM = MIN_MNK_SIZE;
    this->adjust_.minBaseN = MIN_MNK_SIZE;
    this->adjust_.minBaseK = MIN_MNK_SIZE;

    this->traverse_ = MatrixTraverse::NOSET;

    // based on hardware initial size, this part would be reset
    this->oriBufferPool_.l1Size = L1_SIZE;
    this->oriBufferPool_.l0CSize = L0C_SIZE;
    this->oriBufferPool_.ubSize = UB_SIZE;
    this->oriBufferPool_.l0ASize = L0A_SIZE;
    this->oriBufferPool_.l0BSize = L0B_SIZE;
    this->oriBufferPool_.btSize = BT_SIZE;

    TILING_LOG_INFO("l1Size = %d", this->oriBufferPool_.l1Size);
    TILING_LOG_INFO("l0CSize = %d", this->oriBufferPool_.l0CSize);
    TILING_LOG_INFO("ubSize = %d", this->oriBufferPool_.ubSize);
    TILING_LOG_INFO("l0ASize = %d", this->oriBufferPool_.l0ASize);
    TILING_LOG_INFO("l0BSize = %d", this->oriBufferPool_.l0BSize);
    TILING_LOG_INFO("btSize = %d", this->oriBufferPool_.btSize);

    this->oriBufferPool_.l1AlignSize = ALIGN_SIZE;
    this->oriBufferPool_.l0CAlignSize = ALIGN_SIZE;
    this->oriBufferPool_.l0AAlignSize = ALIGN_SIZE;
    this->oriBufferPool_.l0BAlignSize = ALIGN_SIZE;
    this->oriBufferPool_.ubAlignSize = ALIGN_SIZE;
    this->bufferPool_ = this->oriBufferPool_;

    this->blockDim = 1;
    this->batchM = 1;
    this->batchN = 1;
    this->singleBatchM = 1;
    this->singleBatchN = 1;
}

MatmulApiTilingBase::MatmulApiTilingBase(const platform_ascendc::PlatformAscendC& ascendcPlatform)
{
    this->aType_.isDB = true;
    this->bType_.isDB = true;
    this->cType_.isDB = true;
    this->biasType_.isDB = true;

    this->aType_.isTrans = false;
    this->bType_.isTrans = false;

    this->isBias = false;
    this->madType_ = MatrixMadType::NORMAL;

    this->singleM = -1;
    this->singleN = -1;
    this->singleK = -1;
    this->singleCoreM = -1;
    this->singleCoreN = -1;
    this->singleCoreK = -1;
    this->orgM = -1;
    this->orgN = -1;
    this->orgKa = -1;
    this->orgKb = -1;
    this->baseM = -1;
    this->baseN = -1;
    this->baseK = -1;

    this->adjust_.maxBaseM = 0x7FFFFFFF;
    this->adjust_.maxBaseN = 0x7FFFFFFF;
    this->adjust_.maxBaseK = 0x7FFFFFFF;
    this->adjust_.minBaseM = MIN_MNK_SIZE;
    this->adjust_.minBaseN = MIN_MNK_SIZE;
    this->adjust_.minBaseK = MIN_MNK_SIZE;

    this->traverse_ = MatrixTraverse::NOSET;

    // based on platformInfo initial size
    uint64_t l1Size = 0;
    uint64_t l0CSize = 0;
    uint64_t ubSize = 0;
    uint64_t l0ASize = 0;
    uint64_t l0BSize = 0;
    socVersion = ascendcPlatform.GetSocVersion();
    ascendcPlatform.GetCoreMemSize(platform_ascendc::CoreMemType::L1, l1Size);
    ascendcPlatform.GetCoreMemSize(platform_ascendc::CoreMemType::L0_C, l0CSize);
    ascendcPlatform.GetCoreMemSize(platform_ascendc::CoreMemType::UB, ubSize);
    ascendcPlatform.GetCoreMemSize(platform_ascendc::CoreMemType::L0_A, l0ASize);
    ascendcPlatform.GetCoreMemSize(platform_ascendc::CoreMemType::L0_B, l0BSize);
    this->oriBufferPool_.l1Size = l1Size;
    this->oriBufferPool_.l0CSize = static_cast<int32_t>(l0CSize);
    this->oriBufferPool_.ubSize = ubSize;
    this->oriBufferPool_.l0ASize = l0ASize;
    this->oriBufferPool_.l0BSize = l0BSize;
    this->oriBufferPool_.btSize = (socVersion == platform_ascendc::SocVersion::ASCEND910B ||
                                   socVersion == platform_ascendc::SocVersion::ASCEND310B) ?
                                      BT_SIZE :
                                      0;

    TILING_LOG_INFO("l1Size = %d", this->oriBufferPool_.l1Size);
    TILING_LOG_INFO("l0CSize = %d", this->oriBufferPool_.l0CSize);
    TILING_LOG_INFO("ubSize = %d", this->oriBufferPool_.ubSize);
    TILING_LOG_INFO("l0ASize = %d", this->oriBufferPool_.l0ASize);
    TILING_LOG_INFO("l0BSize = %d", this->oriBufferPool_.l0BSize);
    TILING_LOG_INFO("btSize = %d", this->oriBufferPool_.btSize);

    this->oriBufferPool_.l1AlignSize = ALIGN_SIZE;
    this->oriBufferPool_.l0CAlignSize = ALIGN_SIZE;
    this->oriBufferPool_.l0AAlignSize = ALIGN_SIZE;
    this->oriBufferPool_.l0BAlignSize = ALIGN_SIZE;
    this->oriBufferPool_.ubAlignSize = ALIGN_SIZE;
    this->bufferPool_ = this->oriBufferPool_;

    this->blockDim = 1;
    this->batchM = 1;
    this->batchN = 1;
    this->singleBatchM = 1;
    this->singleBatchN = 1;
}

MatmulApiTilingBase::MatmulApiTilingBase(const PlatformInfo& platform)
{
    this->aType_.isDB = true;
    this->bType_.isDB = true;
    this->cType_.isDB = true;
    this->biasType_.isDB = true;

    this->aType_.isTrans = false;
    this->bType_.isTrans = false;

    this->isBias = false;
    this->madType_ = MatrixMadType::NORMAL;

    this->singleM = -1;
    this->singleN = -1;
    this->singleK = -1;
    this->singleCoreM = -1;
    this->singleCoreN = -1;
    this->singleCoreK = -1;
    this->orgM = -1;
    this->orgN = -1;
    this->orgKa = -1;
    this->orgKb = -1;
    this->baseM = -1;
    this->baseN = -1;
    this->baseK = -1;

    this->adjust_.maxBaseM = 0x7FFFFFFF;
    this->adjust_.maxBaseN = 0x7FFFFFFF;
    this->adjust_.maxBaseK = 0x7FFFFFFF;
    this->adjust_.minBaseM = MIN_MNK_SIZE;
    this->adjust_.minBaseN = MIN_MNK_SIZE;
    this->adjust_.minBaseK = MIN_MNK_SIZE;

    this->traverse_ = MatrixTraverse::NOSET;

    // based on platformInfo initial size
    socVersion = platform.socVersion;
    this->oriBufferPool_.l1Size = platform.l1Size;
    this->oriBufferPool_.l0CSize = platform.l0CSize;
    this->oriBufferPool_.ubSize = platform.ubSize;
    this->oriBufferPool_.l0ASize = platform.l0ASize;
    this->oriBufferPool_.l0BSize = platform.l0BSize;
    this->oriBufferPool_.btSize = (socVersion == platform_ascendc::SocVersion::ASCEND910B ||
                                   socVersion == platform_ascendc::SocVersion::ASCEND310B) ?
                                      static_cast<int32_t>(BT_SIZE) :
                                      0;

    TILING_LOG_INFO("l1Size = %d", this->oriBufferPool_.l1Size);
    TILING_LOG_INFO("l0CSize = %d", this->oriBufferPool_.l0CSize);
    TILING_LOG_INFO("ubSize = %d", this->oriBufferPool_.ubSize);
    TILING_LOG_INFO("l0ASize = %d", this->oriBufferPool_.l0ASize);
    TILING_LOG_INFO("l0BSize = %d", this->oriBufferPool_.l0BSize);
    TILING_LOG_INFO("btSize = %d", this->oriBufferPool_.btSize);

    this->oriBufferPool_.l1AlignSize = ALIGN_SIZE;
    this->oriBufferPool_.l0CAlignSize = ALIGN_SIZE;
    this->oriBufferPool_.l0AAlignSize = ALIGN_SIZE;
    this->oriBufferPool_.l0BAlignSize = ALIGN_SIZE;
    this->oriBufferPool_.ubAlignSize = ALIGN_SIZE;
    this->bufferPool_ = this->oriBufferPool_;

    this->blockDim = 1;
    this->batchM = 1;
    this->batchN = 1;
    this->singleBatchM = 1;
    this->singleBatchN = 1;
}

MatmulApiTilingBase::~MatmulApiTilingBase() = default;

int32_t MatmulApiTilingBase::SetAType(TPosition pos, CubeFormat type, DataType dataType, bool isTrans)
{
    // need detection rationality of data, and data combinations
    TILING_LOG_DEBUG("A matrix TPosition: %d", static_cast<int32_t>(pos));
    TILING_LOG_DEBUG("A matrix CubeFormat: %d", static_cast<int32_t>(type));
    TILING_LOG_DEBUG("A matrix dataType: %d", static_cast<int32_t>(dataType));
    TILING_LOG_DEBUG("A matrix isTrans: %d", static_cast<int32_t>(isTrans));
    aType_.pos = pos;
    aType_.type = type;
    aType_.dataType = dataType;
    aType_.isTrans = isTrans;
    return 0;
}

int32_t MatmulApiTilingBase::SetBType(TPosition pos, CubeFormat type, DataType dataType, bool isTrans)
{
    TILING_LOG_DEBUG("B matrix TPosition: %d", static_cast<int32_t>(pos));
    TILING_LOG_DEBUG("B matrix CubeFormat: %d", static_cast<int32_t>(type));
    TILING_LOG_DEBUG("B matrix dataType: %d", static_cast<int32_t>(dataType));
    TILING_LOG_DEBUG("B matrix isTrans: %d", static_cast<int32_t>(isTrans));

    // need detection rationality of data, and data combinations
    bType_.pos = pos;
    bType_.type = type;
    bType_.dataType = dataType;
    bType_.isTrans = isTrans;
    return 0;
}

int32_t MatmulApiTilingBase::SetScaleAType(TPosition scalePos, CubeFormat scaleType, bool isScaleTrans)
{
    TILING_LOG_DEBUG("A scale TPosition: %d", static_cast<int32_t>(scalePos));
    TILING_LOG_DEBUG("A scale Type: %d", static_cast<int32_t>(scaleType));
    TILING_LOG_DEBUG("A scale isTrans: %d", static_cast<int32_t>(isScaleTrans));
    aType_.hasSetScaleType = true;
    aType_.scalePos = scalePos;
    aType_.scaleType = scaleType;
    aType_.isScaleTrans = isScaleTrans;
    return 0;
}

int32_t MatmulApiTilingBase::SetScaleBType(TPosition scalePos, CubeFormat scaleType, bool isScaleTrans)
{
    TILING_LOG_DEBUG("B scale TPosition: %d", static_cast<int32_t>(scalePos));
    TILING_LOG_DEBUG("B scale Type: %d", static_cast<int32_t>(scaleType));
    TILING_LOG_DEBUG("B scale isTrans: %d", static_cast<int32_t>(isScaleTrans));
    bType_.hasSetScaleType = true;
    bType_.scalePos = scalePos;
    bType_.scaleType = scaleType;
    bType_.isScaleTrans = isScaleTrans;
    return 0;
}

int32_t MatmulApiTilingBase::SetCType(TPosition pos, CubeFormat type, DataType dataType)
{
    TILING_LOG_DEBUG("C matrix TPosition: %d", static_cast<int32_t>(pos));
    TILING_LOG_DEBUG("C matrix CubeFormat: %d", static_cast<int32_t>(type));
    TILING_LOG_DEBUG("C matrix dataType: %d", static_cast<int32_t>(dataType));

    // need detection rationality of data, and data combinations
    cType_.pos = pos;
    cType_.type = type;
    cType_.dataType = dataType;
    return 0;
}

int32_t MatmulApiTilingBase::SetBiasType(TPosition pos, CubeFormat type, DataType dataType)
{
    TILING_LOG_DEBUG("Bias TPosition: %d", static_cast<int32_t>(pos));
    TILING_LOG_DEBUG("Bias CubeFormat: %d", static_cast<int32_t>(type));
    TILING_LOG_DEBUG("Bias dataType: %d", static_cast<int32_t>(dataType));

    // need detection rationality of data, and data combinations
    biasType_.pos = pos;
    biasType_.type = type;
    biasType_.dataType = dataType;
    return 0;
}

int32_t MatmulApiTilingBase::SetOrgShape(int32_t orgMIn, int32_t orgNIn, int32_t orgKIn)
{
    return SetOrgShape(orgMIn, orgNIn, orgKIn, orgKIn);
}

int32_t MatmulApiTilingBase::SetOrgShape(int32_t orgMIn, int32_t orgNIn, int32_t orgKaIn, int32_t orgKbIn)
{
    TILING_LOG_DEBUG("Set Original M: %d", orgMIn);
    TILING_LOG_DEBUG("Set Original N: %d", orgNIn);
    TILING_LOG_DEBUG("Set Original Ka: %d", orgKaIn);
    TILING_LOG_DEBUG("Set Original Kb: %d", orgKbIn);

    this->orgM = orgMIn;
    this->orgN = orgNIn;
    this->orgKa = orgKaIn;
    this->orgKb = orgKbIn;
    return 0;
}

int32_t MatmulApiTilingBase::SetALayout(int32_t b, int32_t s, int32_t n, int32_t g, int32_t d)
{
    TILING_LOG_DEBUG("Set ALayout B: %d", b);
    TILING_LOG_DEBUG("Set ALayout S: %d", s);
    TILING_LOG_DEBUG("Set ALayout N: %d", n);
    TILING_LOG_DEBUG("Set ALayout G: %d", g);
    TILING_LOG_DEBUG("Set ALayout D: %d", d);

    this->aLayoutInfoB = b;
    this->aLayoutInfoS = s;
    this->aLayoutInfoN = n;
    this->aLayoutInfoG = g;
    this->aLayoutInfoD = d;
    return 0;
}

int32_t MatmulApiTilingBase::SetBLayout(int32_t b, int32_t s, int32_t n, int32_t g, int32_t d)
{
    TILING_LOG_DEBUG("Set BLayout B: %d", b);
    TILING_LOG_DEBUG("Set BLayout S: %d", s);
    TILING_LOG_DEBUG("Set BLayout N: %d", n);
    TILING_LOG_DEBUG("Set BLayout G: %d", g);
    TILING_LOG_DEBUG("Set BLayout D: %d", d);

    this->bLayoutInfoB = b;
    this->bLayoutInfoS = s;
    this->bLayoutInfoN = n;
    this->bLayoutInfoG = g;
    this->bLayoutInfoD = d;
    return 0;
}

int32_t MatmulApiTilingBase::SetCLayout(int32_t b, int32_t s, int32_t n, int32_t g, int32_t d)
{
    TILING_LOG_DEBUG("Set CLayout B: %d", b);
    TILING_LOG_DEBUG("Set CLayout S: %d", s);
    TILING_LOG_DEBUG("Set CLayout N: %d", n);
    TILING_LOG_DEBUG("Set CLayout G: %d", g);
    TILING_LOG_DEBUG("Set CLayout D: %d", d);

    this->cLayoutInfoB = b;
    this->cLayoutInfoS1 = s;
    this->cLayoutInfoN = n;
    this->cLayoutInfoG = g;
    this->cLayoutInfoS2 = d;
    return 0;
}

int32_t MatmulApiTilingBase::SetBatchInfoForNormal(int32_t batchA, int32_t batchB, int32_t m, int32_t n, int32_t k)
{
    CHECK_TILING_PARAMETER(batchA, " Set Normal Layout BatchA");
    CHECK_TILING_PARAMETER(batchB, " Set Normal Layout BatchB");
    CHECK_TILING_PARAMETER(m, " Set Normal Layout M");
    CHECK_TILING_PARAMETER(n, " Set Normal Layout N");
    CHECK_TILING_PARAMETER(k, " Set Normal Layout K");
    if (this->batchNum > 0 && batchA > this->batchNum) {
        TILING_LOG_DEBUG(
            "The Input batchA size should be less than or equal to batchNum, currently, batchA is %d, batchNum is %d. ",
            batchA, this->batchNum);
    }
    if (this->batchNum > 0 && batchB > this->batchNum) {
        TILING_LOG_DEBUG(
            "The Input batchB size should be less than or equal to batchNum, currently, batchB is %d, batchNum is %d. ",
            batchB, this->batchNum);
    }
    this->aLayoutInfoB = batchA;
    this->aLayoutInfoS = m;
    this->aLayoutInfoN = 1;
    this->aLayoutInfoG = 1;
    this->aLayoutInfoD = k;

    this->bLayoutInfoB = batchB;
    this->bLayoutInfoS = n;
    this->bLayoutInfoN = 1;
    this->bLayoutInfoG = 1;
    this->bLayoutInfoD = k;

    this->cLayoutInfoB = batchA > batchB ? batchA : batchB;
    this->cLayoutInfoS1 = m;
    this->cLayoutInfoN = 1;
    this->cLayoutInfoG = 1;
    this->cLayoutInfoS2 = n;
    this->isBMNKBmm = true;
    return 0;
}

int32_t MatmulApiTilingBase::SetBatchNum(int32_t batch)
{
    CHECK_TILING_PARAMETER(batch, " Set BatchNum");
    if (this->aLayoutInfoB > 0 && batch < this->aLayoutInfoB) {
        TILING_LOG_DEBUG(
            "The Input batchNum size should be greater than or equal to batchA, currently, batchA is %d, batch is %d.",
            this->aLayoutInfoB, batch);
    }
    if (this->bLayoutInfoB > 0 && batch < this->bLayoutInfoB) {
        TILING_LOG_DEBUG(
            "The Input batchNum size should be greater than or equal to batchB, currently, batchB is %d, batch is %d.",
            this->bLayoutInfoB, batch);
    }
    this->batchNum = batch;
    return 0;
}

int32_t MatmulApiTilingBase::SetShape(int32_t m, int32_t n, int32_t k)
{
    TILING_LOG_DEBUG("Set Single M: %d", m);
    TILING_LOG_DEBUG("Set Single N: %d", n);
    TILING_LOG_DEBUG("Set Single K: %d", k);

    this->singleM = m;
    this->singleN = n;
    this->singleK = k;
    if (this->orgM == -1 && this->orgN == -1 && this->orgKa == -1 && this->orgKb == -1) {
        this->orgM = m;
        this->orgN = n;
        this->orgKa = k;
        this->orgKb = k;
    }
    return 0;
}

int32_t MatmulApiTilingBase::EnableBias(bool isBiasIn)
{
    TILING_LOG_DEBUG("Set bias: %d", static_cast<int32_t>(isBiasIn));
    this->isBias = isBiasIn;
    return 0;
}

int32_t MatmulApiTilingBase::SetBias(bool isBiasIn) { return EnableBias(isBiasIn); }

int32_t MatmulApiTilingBase::SetFixSplit(int32_t baseMIn, int32_t baseNIn, int32_t baseKIn)
{
    TILING_LOG_DEBUG("Set fixed split baseM: %d", baseMIn);
    TILING_LOG_DEBUG("Set fixed split baseN: %d", baseNIn);
    TILING_LOG_DEBUG("Set fixed split baseK: %d", baseKIn);
    if (baseMIn == 0 || baseNIn == 0 || baseKIn == 0) {
        return -1;
    }
    if (baseMIn != -1) {
        if (baseMIn % C0_SIZE != 0) {
            return -1;
        }
        this->baseM = baseMIn;
        this->adjust_.maxBaseM = baseMIn;
        this->adjust_.minBaseM = baseMIn;
    }
    if (baseNIn != -1) {
        if (baseNIn % C0_SIZE != 0) {
            return -1;
        }
        this->baseN = baseNIn;
        this->adjust_.maxBaseN = baseNIn;
        this->adjust_.minBaseN = baseNIn;
    }
    const int32_t k0 = C0_BYTE_SIZE / DTYPE_BIT_TAB.at(this->aType_.dataType) * BITS_PER_BYTE;
    if (baseKIn != -1) {
        if (k0 == 0 || baseKIn % k0 != 0) {
            return -1;
        }
        this->baseK = baseKIn;
        this->adjust_.maxBaseK = baseKIn;
        this->adjust_.minBaseK = baseKIn;
    }

    return 0;
}

int32_t MatmulApiTilingBase::SetDoubleBuffer(bool a, bool b, bool c, bool bias, bool transND2NZ, bool transNZ2ND)
{
    (void)a;
    (void)b;
    (void)c;
    (void)bias;
    (void)transND2NZ;
    (void)transNZ2ND;
    TILING_LOG_INFO("This is a reserved API.");
    return 0;
}

// set allowed used space, default using all space of chip
int32_t MatmulApiTilingBase::SetBufferSpace(int32_t l1Size, int32_t l0CSize, int32_t ubSize, int32_t btSize)
{
    TILING_LOG_DEBUG("Set Size L1: %d", l1Size);
    TILING_LOG_DEBUG("Set Size L0CSize: %d", l0CSize);
    TILING_LOG_DEBUG("Set Size UBSize: %d", ubSize);
    TILING_LOG_DEBUG("Set Size BtSize: %d", btSize);
    if (l1Size < -1 || (l1Size > this->bufferPool_.l1Size)) {
        TILING_LOG_INFO("Provided L1 size is out of range");
        return -1;
    }
    if (l0CSize < -1 || (l0CSize > this->bufferPool_.l0CSize)) {
        TILING_LOG_INFO("Provided L0c size is out of range");
        return -1;
    }
    if (ubSize < -1 || (ubSize > this->bufferPool_.ubSize)) {
        TILING_LOG_INFO("Provided Ub size is out of range");
        return -1;
    }
    if (btSize < -1 || (btSize > this->bufferPool_.btSize)) {
        TILING_LOG_INFO("Provided Bt size is out of range");
        return -1;
    }

    if (l1Size != -1) {
        l1Size = l1Size - (l1Size % this->bufferPool_.l1AlignSize);
        this->bufferPool_.l1Size = l1Size;
    }
    if (l0CSize != -1) {
        l0CSize = l0CSize - (l0CSize % this->bufferPool_.l0CAlignSize);
        this->bufferPool_.l0CSize = l0CSize;
    }
    if (ubSize != -1) {
        ubSize = ubSize - (ubSize % this->bufferPool_.ubAlignSize);
        this->bufferPool_.ubSize = ubSize;
    }
    // only V220 && V300 has bias table
    if (btSize != -1) {
        this->bufferPool_.btSize = btSize;
    }
    return 0;
}

int32_t MatmulApiTilingBase::SetTraverse(MatrixTraverse traverse)
{
    this->traverse_ = traverse;
    return 0;
}

int32_t MatmulApiTilingBase::SetMadType(MatrixMadType madType)
{
    this->madType_ = madType;
    return 0;
}

int32_t MatmulApiTilingBase::SetSplitRange(
    int32_t maxBaseM, int32_t maxBaseN, int32_t maxBaseK, int32_t minBaseM, int32_t minBaseN, int32_t minBaseK)
{
    if (this->baseM == -1) {
        if (maxBaseM != -1) {
            maxBaseM = maxBaseM - (maxBaseM % C0_SIZE);
            this->adjust_.maxBaseM = maxBaseM;
        }
        if (minBaseM != -1) {
            minBaseM = minBaseM - (minBaseM % C0_SIZE);
            this->adjust_.minBaseM = minBaseM;
        }
    }

    if (this->baseN == -1) {
        if (maxBaseN != -1) {
            maxBaseN = maxBaseN - (maxBaseN % C0_SIZE);
            this->adjust_.maxBaseN = maxBaseN;
        }
        if (minBaseN != -1) {
            minBaseN = minBaseN - (minBaseN % C0_SIZE);
            this->adjust_.minBaseN = minBaseN;
        }
    }

    if (this->baseK == -1) {
        if (maxBaseK != -1) {
            maxBaseK = maxBaseK - (maxBaseK % C0_SIZE);
            this->adjust_.maxBaseK = maxBaseK;
        }
        if (minBaseK != -1) {
            minBaseK = minBaseK - (minBaseK % C0_SIZE);
            this->adjust_.minBaseK = minBaseK;
        }
    }

    return 0;
}

int32_t MatmulApiTilingBase::SetSparse(bool isSparseIn)
{
    TILING_LOG_DEBUG("Set sparse: %d", static_cast<int32_t>(isSparseIn));
    this->isSparse_ = isSparseIn;
    return 0;
}

void MatmulApiTilingBase::SetMatmulConfigParams(
    int32_t mmConfigTypeIn, bool enableL1CacheUBIn, ScheduleType scheduleTypeIn, MatrixTraverse traverseIn,
    bool enVecND2NZIn)
{
    TILING_LOG_DEBUG("Set MatmulConfigType: %d", mmConfigTypeIn);
    TILING_LOG_DEBUG("Set EnableL1CacheUB: %d", static_cast<int32_t>(enableL1CacheUBIn));
    TILING_LOG_DEBUG("Set ScheduleType: %d", static_cast<int32_t>(scheduleTypeIn));
    TILING_LOG_DEBUG("Set Traverse: %d", static_cast<int32_t>(traverseIn));
    TILING_LOG_DEBUG("Set EnVecND2NZ: %d", static_cast<int32_t>(enVecND2NZIn));
    this->mmConfigType = mmConfigTypeIn;
    this->enableL1CacheUB = enableL1CacheUBIn;
    this->scheduleType = scheduleTypeIn;
    this->traverse_ = traverseIn;
    this->enVecND2NZ = enVecND2NZIn;
}

void MatmulApiTilingBase::SetMatmulConfigParams(const MatmulConfigParams& configParams)
{
    TILING_LOG_DEBUG("Set MatmulConfigType: %d", static_cast<int32_t>(configParams.mmConfigType));
    TILING_LOG_DEBUG("Set EnableL1CacheUB: %d", static_cast<int32_t>(configParams.enableL1CacheUB));
    TILING_LOG_DEBUG("Set ScheduleType: %d", static_cast<int32_t>(configParams.scheduleType));
    TILING_LOG_DEBUG("Set Traverse: %d", static_cast<int32_t>(configParams.traverse));
    TILING_LOG_DEBUG("Set EnVecND2NZ: %d", static_cast<int32_t>(configParams.enVecND2NZ));
    this->mmConfigType = configParams.mmConfigType;
    this->enableL1CacheUB = configParams.enableL1CacheUB;
    this->scheduleType = configParams.scheduleType;
    this->traverse_ = configParams.traverse;
    this->enVecND2NZ = configParams.enVecND2NZ;
}

bool MatmulApiTilingBase::CheckSetParam()
{
    if (socVersion == platform_ascendc::SocVersion::ASCEND910 ||
        socVersion == platform_ascendc::SocVersion::ASCEND310P) {
        if (isBias && biasType_.pos == TPosition::TSCM) {
            TILING_LOG_INFO("for ascend310p/ascend910, bias not support TSCM pos");
            return false;
        }
    }

    int32_t dataBits = DTYPE_BIT_TAB.at(aType_.dataType);
    if (this->baseM != -1 && this->baseK != -1) {
        // set baseM, baseK, L0A limited
        if (this->baseM * this->baseK * dataBits / BITS_PER_BYTE > this->bufferPool_.l0ASize) {
            TILING_LOG_INFO("baseM * baseK is larger than L0ASize");
            return false;
        }
    }

    dataBits = DTYPE_BIT_TAB.at(bType_.dataType);
    if (this->baseK != -1 && this->baseN != -1) {
        // set baseM, baseK, L0B limited
        if (this->baseK * this->baseN * dataBits / BITS_PER_BYTE > this->bufferPool_.l0BSize) {
            TILING_LOG_INFO("baseN * baseK is larger than l0BSize");
            return false;
        }
    }

    if (this->baseM != -1 && this->baseN != -1) {
        // set baseM, baseN, L0C limited
        if (this->baseM * this->baseN * FP32_BYTES > this->bufferPool_.l0CSize) {
            TILING_LOG_INFO("baseM * baseN is larger than L0CSize");
            return false;
        }
    }

    if (this->singleM == 0 || this->singleN == 0 || this->singleK == 0) {
        TILING_LOG_INFO("singleM/N/K can not be zero");
        return false;
    }
    return true;
}

void MatmulApiTilingBase::SetFinalTiling(optiling::TCubeTiling& tiling)
{
    tiling.set_usedCoreNum(this->tiling_.get_usedCoreNum());
    tiling.set_M(this->tiling_.get_M());
    tiling.set_N(this->tiling_.get_N());
    tiling.set_Ka(this->tiling_.get_Ka());
    tiling.set_Kb(this->tiling_.get_Kb());
    tiling.set_singleCoreM(this->tiling_.get_singleCoreM());
    tiling.set_singleCoreN(this->tiling_.get_singleCoreN());
    tiling.set_singleCoreK(this->tiling_.get_singleCoreK());
    tiling.set_baseM(this->tiling_.get_baseM());
    tiling.set_baseN(this->tiling_.get_baseN());
    tiling.set_baseK(this->tiling_.get_baseK());
    tiling.set_depthA1(this->tiling_.get_depthA1());
    tiling.set_depthB1(this->tiling_.get_depthB1());
    tiling.set_depthAL1CacheUB(this->tiling_.get_depthAL1CacheUB());
    tiling.set_depthBL1CacheUB(this->tiling_.get_depthBL1CacheUB());
    tiling.set_stepM(this->tiling_.get_stepM());
    tiling.set_stepN(this->tiling_.get_stepN());
    tiling.set_isBias(this->tiling_.get_isBias());
    tiling.set_transLength(this->tiling_.get_transLength());
    tiling.set_iterateOrder(this->tiling_.get_iterateOrder());
    tiling.set_shareMode(this->tiling_.get_shareMode());
    tiling.set_shareL1Size(this->tiling_.get_shareL1Size());
    tiling.set_shareL0CSize(this->tiling_.get_shareL0CSize());
    tiling.set_shareUbSize(this->tiling_.get_shareUbSize());
    tiling.set_batchM(this->tiling_.get_batchM());
    tiling.set_batchN(this->tiling_.get_batchN());
    tiling.set_singleBatchM(this->tiling_.get_singleBatchM());
    tiling.set_singleBatchN(this->tiling_.get_singleBatchN());
    tiling.set_stepKa(this->tiling_.get_stepKa());
    tiling.set_stepKb(this->tiling_.get_stepKb());
    tiling.set_dbL0A(this->tiling_.get_dbL0A());
    tiling.set_dbL0B(this->tiling_.get_dbL0B());
    tiling.set_dbL0C(this->tiling_.get_dbL0C());

    tiling.set_ALayoutInfoB(this->tiling_.get_ALayoutInfoB());
    tiling.set_ALayoutInfoS(this->tiling_.get_ALayoutInfoS());
    tiling.set_ALayoutInfoN(this->tiling_.get_ALayoutInfoN());
    tiling.set_ALayoutInfoG(this->tiling_.get_ALayoutInfoG());
    tiling.set_ALayoutInfoD(this->tiling_.get_ALayoutInfoD());
    tiling.set_BLayoutInfoB(this->tiling_.get_BLayoutInfoB());
    tiling.set_BLayoutInfoS(this->tiling_.get_BLayoutInfoS());
    tiling.set_BLayoutInfoN(this->tiling_.get_BLayoutInfoN());
    tiling.set_BLayoutInfoG(this->tiling_.get_BLayoutInfoG());
    tiling.set_BLayoutInfoD(this->tiling_.get_BLayoutInfoD());
    tiling.set_CLayoutInfoB(this->tiling_.get_CLayoutInfoB());
    tiling.set_CLayoutInfoS1(this->tiling_.get_CLayoutInfoS1());
    tiling.set_CLayoutInfoN(this->tiling_.get_CLayoutInfoN());
    tiling.set_CLayoutInfoG(this->tiling_.get_CLayoutInfoG());
    tiling.set_CLayoutInfoS2(this->tiling_.get_CLayoutInfoS2());
    tiling.set_BatchNum(this->tiling_.get_BatchNum());
    tiling.set_mxTypePara(this->tiling_.get_mxTypePara());
    return;
}

void MatmulApiTilingBase::SetFinalTiling(AscendC::tiling::TCubeTiling& tiling)
{
    optiling::TCubeTiling tCubeTiling;
    SetFinalTiling(tCubeTiling);
    tCubeTiling.SaveToBuffer(&tiling, sizeof(TCubeTiling));
}

void MatmulApiTilingBase::PrintTilingDataInfo(optiling::TCubeTiling& tiling) const
{
    TILING_LOG_INFO("MatmulTiling: M             = %d", tiling.get_M());
    TILING_LOG_INFO("MatmulTiling: N             = %d", tiling.get_N());
    TILING_LOG_INFO("MatmulTiling: Ka            = %d", tiling.get_Ka());
    TILING_LOG_INFO("MatmulTiling: Kb            = %d", tiling.get_Kb());
    TILING_LOG_INFO("MatmulTiling: singleCoreM   = %d", tiling.get_singleCoreM());
    TILING_LOG_INFO("MatmulTiling: singleCoreN   = %d", tiling.get_singleCoreN());
    TILING_LOG_INFO("MatmulTiling: singleCoreK   = %d", tiling.get_singleCoreK());
    TILING_LOG_INFO("MatmulTiling: baseM         = %d", tiling.get_baseM());
    TILING_LOG_INFO("MatmulTiling: baseN         = %d", tiling.get_baseN());
    TILING_LOG_INFO("MatmulTiling: baseK         = %d", tiling.get_baseK());
    TILING_LOG_INFO("MatmulTiling: depthA1       = %d", tiling.get_depthA1());
    TILING_LOG_INFO("MatmulTiling: depthB1       = %d", tiling.get_depthB1());
    TILING_LOG_INFO("MatmulTiling: depthAL1CacheUB     = %d", tiling.get_depthAL1CacheUB());
    TILING_LOG_INFO("MatmulTiling: depthBL1CacheUB     = %d", tiling.get_depthBL1CacheUB());
    TILING_LOG_INFO("MatmulTiling: stepM         = %d", tiling.get_stepM());
    TILING_LOG_INFO("MatmulTiling: stepN         = %d", tiling.get_stepN());
    TILING_LOG_INFO("MatmulTiling: isBias        = %d", tiling.get_isBias());
    TILING_LOG_INFO("MatmulTiling: transLength   = %d", tiling.get_transLength());
    TILING_LOG_INFO("MatmulTiling: iterateOrder  = %d", tiling.get_iterateOrder());
    TILING_LOG_INFO("MatmulTiling: shareMode     = %d", tiling.get_shareMode());
    TILING_LOG_INFO("MatmulTiling: usedL1Size    = %d", tiling.get_shareL1Size());
    TILING_LOG_INFO("MatmulTiling: usedL0CSize   = %d", tiling.get_shareL0CSize());
    TILING_LOG_INFO("MatmulTiling: usedUBSize    = %d", tiling.get_shareUbSize());
    TILING_LOG_INFO("MatmulTiling: batchM        = %d", tiling.get_batchM());
    TILING_LOG_INFO("MatmulTiling: batchN        = %d", tiling.get_batchN());
    TILING_LOG_INFO("MatmulTiling: singleBatchM  = %d", tiling.get_singleBatchM());
    TILING_LOG_INFO("MatmulTiling: singleBatchN  = %d", tiling.get_singleBatchN());
    TILING_LOG_INFO("MatmulTiling: stepKa        = %d", tiling.get_stepKa());
    TILING_LOG_INFO("MatmulTiling: stepKb        = %d", tiling.get_stepKb());
}

void MatmulApiTilingBase::PrintTilingDataInfo(AscendC::tiling::TCubeTiling& tiling) const
{
    TILING_LOG_INFO("MatmulTiling: M             = %d", tiling.M);
    TILING_LOG_INFO("MatmulTiling: N             = %d", tiling.N);
    TILING_LOG_INFO("MatmulTiling: Ka            = %d", tiling.Ka);
    TILING_LOG_INFO("MatmulTiling: Kb            = %d", tiling.Kb);
    TILING_LOG_INFO("MatmulTiling: singleCoreM   = %d", tiling.singleCoreM);
    TILING_LOG_INFO("MatmulTiling: singleCoreN   = %d", tiling.singleCoreN);
    TILING_LOG_INFO("MatmulTiling: singleCoreK   = %d", tiling.singleCoreK);
    TILING_LOG_INFO("MatmulTiling: baseM         = %d", tiling.baseM);
    TILING_LOG_INFO("MatmulTiling: baseN         = %d", tiling.baseN);
    TILING_LOG_INFO("MatmulTiling: baseK         = %d", tiling.baseK);
    TILING_LOG_INFO("MatmulTiling: depthA1       = %d", tiling.depthA1);
    TILING_LOG_INFO("MatmulTiling: depthB1       = %d", tiling.depthB1);
    TILING_LOG_INFO("MatmulTiling: depthAL1CacheUB     = %d", tiling.depthAL1CacheUB);
    TILING_LOG_INFO("MatmulTiling: depthBL1CacheUB     = %d", tiling.depthBL1CacheUB);
    TILING_LOG_INFO("MatmulTiling: stepM         = %d", tiling.stepM);
    TILING_LOG_INFO("MatmulTiling: stepN         = %d", tiling.stepN);
    TILING_LOG_INFO("MatmulTiling: isBias        = %d", tiling.isBias);
    TILING_LOG_INFO("MatmulTiling: transLength   = %d", tiling.transLength);
    TILING_LOG_INFO("MatmulTiling: iterateOrder  = %d", tiling.iterateOrder);
    TILING_LOG_INFO("MatmulTiling: shareMode     = %d", tiling.shareMode);
    TILING_LOG_INFO("MatmulTiling: usedL1Size    = %d", tiling.shareL1Size);
    TILING_LOG_INFO("MatmulTiling: usedL0CSize   = %d", tiling.shareL0CSize);
    TILING_LOG_INFO("MatmulTiling: usedUBSize    = %d", tiling.shareUbSize);
    TILING_LOG_INFO("MatmulTiling: batchM        = %d", tiling.batchM);
    TILING_LOG_INFO("MatmulTiling: batchN        = %d", tiling.batchN);
    TILING_LOG_INFO("MatmulTiling: singleBatchM  = %d", tiling.singleBatchM);
    TILING_LOG_INFO("MatmulTiling: singleBatchN  = %d", tiling.singleBatchN);
    TILING_LOG_INFO("MatmulTiling: stepKa        = %d", tiling.stepKa);
    TILING_LOG_INFO("MatmulTiling: stepKb        = %d", tiling.stepKb);
}

void MatmulApiTilingBase::PrintTilingData()
{
    std::cout << "tiling.usedCoreNum   = " << this->tiling_.get_usedCoreNum() << std::endl;
    std::cout << "tiling.M             = " << this->tiling_.get_M() << std::endl;
    std::cout << "tiling.N             = " << this->tiling_.get_N() << std::endl;
    std::cout << "tiling.Ka            = " << this->tiling_.get_Ka() << std::endl;
    std::cout << "tiling.Kb            = " << this->tiling_.get_Kb() << std::endl;
    std::cout << "tiling.singleCoreM   = " << this->tiling_.get_singleCoreM() << std::endl;
    std::cout << "tiling.singleCoreN   = " << this->tiling_.get_singleCoreN() << std::endl;
    std::cout << "tiling.singleCoreK   = " << this->tiling_.get_singleCoreK() << std::endl;
    std::cout << "tiling.baseM         = " << this->tiling_.get_baseM() << std::endl;
    std::cout << "tiling.baseN         = " << this->tiling_.get_baseN() << std::endl;
    std::cout << "tiling.baseK         = " << this->tiling_.get_baseK() << std::endl;
    std::cout << "tiling.depthA1       = " << this->tiling_.get_depthA1() << std::endl;
    std::cout << "tiling.depthB1       = " << this->tiling_.get_depthB1() << std::endl;
    std::cout << "tiling.depthAL1CacheUB     = " << this->tiling_.get_depthAL1CacheUB() << std::endl;
    std::cout << "tiling.depthBL1CacheUB     = " << this->tiling_.get_depthBL1CacheUB() << std::endl;
    std::cout << "tiling.stepM         = " << this->tiling_.get_stepM() << std::endl;
    std::cout << "tiling.stepN         = " << this->tiling_.get_stepN() << std::endl;
    std::cout << "tiling.isBias        = " << this->tiling_.get_isBias() << std::endl;
    std::cout << "tiling.transLength   = " << this->tiling_.get_transLength() << std::endl;
    std::cout << "tiling.iterateOrder  = " << this->tiling_.get_iterateOrder() << std::endl;
    std::cout << "tiling.shareMode     = " << this->tiling_.get_shareMode() << std::endl;
    std::cout << "tiling.usedL1Size    = " << this->tiling_.get_shareL1Size() << std::endl;
    std::cout << "tiling.usedL0CSize   = " << this->tiling_.get_shareL0CSize() << std::endl;
    std::cout << "tiling.usedUBSize    = " << this->tiling_.get_shareUbSize() << std::endl;
    std::cout << "tiling.batchM        = " << this->tiling_.get_batchM() << std::endl;
    std::cout << "tiling.batchN        = " << this->tiling_.get_batchN() << std::endl;
    std::cout << "tiling.singleBatchM  = " << this->tiling_.get_singleBatchM() << std::endl;
    std::cout << "tiling.singleBatchN  = " << this->tiling_.get_singleBatchN() << std::endl;
    std::cout << "tiling.stepKa        = " << this->tiling_.get_stepKa() << std::endl;
    std::cout << "tiling.stepKb        = " << this->tiling_.get_stepKb() << std::endl;
    std::cout << "tiling.dbL0A         = " << this->tiling_.get_dbL0A() << std::endl;
    std::cout << "tiling.dbL0B         = " << this->tiling_.get_dbL0B() << std::endl;
    std::cout << "tiling.dbL0C         = " << this->tiling_.get_dbL0C() << std::endl;

    std::cout << "tiling.ALayoutInfoB        = " << this->tiling_.get_ALayoutInfoB() << std::endl;
    std::cout << "tiling.ALayoutInfoS        = " << this->tiling_.get_ALayoutInfoS() << std::endl;
    std::cout << "tiling.ALayoutInfoN        = " << this->tiling_.get_ALayoutInfoN() << std::endl;
    std::cout << "tiling.ALayoutInfoG        = " << this->tiling_.get_ALayoutInfoG() << std::endl;
    std::cout << "tiling.ALayoutInfoD        = " << this->tiling_.get_ALayoutInfoD() << std::endl;
    std::cout << "tiling.BLayoutInfoB        = " << this->tiling_.get_BLayoutInfoB() << std::endl;
    std::cout << "tiling.BLayoutInfoS        = " << this->tiling_.get_BLayoutInfoS() << std::endl;
    std::cout << "tiling.BLayoutInfoN        = " << this->tiling_.get_BLayoutInfoN() << std::endl;
    std::cout << "tiling.BLayoutInfoG        = " << this->tiling_.get_BLayoutInfoG() << std::endl;
    std::cout << "tiling.BLayoutInfoD        = " << this->tiling_.get_BLayoutInfoD() << std::endl;
    std::cout << "tiling.CLayoutInfoB        = " << this->tiling_.get_CLayoutInfoB() << std::endl;
    std::cout << "tiling.CLayoutInfoS1        = " << this->tiling_.get_CLayoutInfoS1() << std::endl;
    std::cout << "tiling.CLayoutInfoN        = " << this->tiling_.get_CLayoutInfoN() << std::endl;
    std::cout << "tiling.CLayoutInfoG        = " << this->tiling_.get_CLayoutInfoG() << std::endl;
    std::cout << "tiling.CLayoutInfoS2        = " << this->tiling_.get_CLayoutInfoS2() << std::endl;
    std::cout << "tiling.BatchNum        = " << this->tiling_.get_BatchNum() << std::endl;
    std::cout << "tiling.mxTypePara        = " << this->tiling_.get_mxTypePara() << std::endl;

    std::cout << "tiling.L1Ratio       = " << (this->tiling_.get_shareL1Size() + 0.0) / this->oriBufferPool_.l1Size
              << std::endl;

    std::cout << "tiling.L0CRatio      = "
              << (static_cast<float>(this->tiling_.get_shareL0CSize()) + 0.0) /
                     static_cast<float>(this->oriBufferPool_.l0CSize)
              << std::endl;
    std::cout << "tiling.L0ARatio      = "
              << (this->tiling_.get_baseM() * this->tiling_.get_baseK() + 0.0) *
                     DTYPE_BIT_TAB.at(this->aType_.dataType) / BITS_PER_BYTE / this->oriBufferPool_.l0ASize
              << std::endl;
    std::cout << "tiling.L0BRatio      = "
              << (this->tiling_.get_baseN() * this->tiling_.get_baseK() + 0.0) *
                     DTYPE_BIT_TAB.at(this->bType_.dataType) / BITS_PER_BYTE / this->oriBufferPool_.l0BSize
              << std::endl;
}
} // namespace matmul_tiling
