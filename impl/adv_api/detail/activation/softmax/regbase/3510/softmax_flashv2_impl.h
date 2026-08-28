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
 * \file softmax_flashv2_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/activation/softmax/regbase/3510/softmax_flashv2_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/softmaxflashv2.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASHV2_IMPL_H__
#endif

#ifndef IMPL_ACTIVATION_SOFTMAX_C310_SOFTMAX_FLASHV2_IMPL_H
#define IMPL_ACTIVATION_SOFTMAX_C310_SOFTMAX_FLASHV2_IMPL_H

#include "softmax_common_impl.h"
#include "softmax_impl.h"
#include "../../../../common/common.h"

namespace AscendC {

template <typename T1, typename T2, bool isOutputReduceMax = false>
__simd_vf__ inline void SoftmaxFlashV2M1NDUpdateVFImpl(
    __ubuf__ T1* dstUb, __ubuf__ T2* reduceMaxUb, __ubuf__ T2* expSumUb, __ubuf__ T2* inExpSumUb, __ubuf__ T2* maxUb,
    __ubuf__ T2* inMaxUb, __ubuf__ T1* srcUb, __ubuf__ T1* expMaxUb, const LastAxisShapeND originalSrcShape,
    const SoftMaxTiling tiling)
{
    uint16_t srcM = originalSrcShape.m;
    uint16_t srcK = tiling.srcK;
    uint16_t originK = originalSrcShape.k;
    uint16_t repeatTimes = CeilDivision(originK, FLOAT_REPEAT_SIZE);
    NotNumUnion notNum;
    notNum.i = F32_NEG_INF;

    Reg::MaskReg pregFull = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    Reg::MaskReg pregOnePt = Reg::CreateMask<uint32_t, Reg::MaskPattern::VL1>();
    Reg::RegTensor<float> srcVreg;
    Reg::RegTensor<float> maxVreg;
    Reg::RegTensor<float> expMaxVreg;
    Reg::RegTensor<float> sumVreg;
    Reg::RegTensor<float> tmpVreg;
    Reg::RegTensor<float> dstVreg;

    for (uint16_t i = 0; i < srcM; ++i) {
        Duplicate(maxVreg, notNum.f);
        for (uint16_t j = 0; j < repeatTimes; ++j) {
            LoadIfNeedCast<T1>(srcVreg, srcUb + i * srcK + j * FLOAT_REPEAT_SIZE, pregFull);
            Reg::Max(maxVreg, maxVreg, srcVreg, pregFull);
        }
        Reg::ReduceMax(maxVreg, maxVreg, pregFull);
        if constexpr (isOutputReduceMax) {
            StoreIfNeedCastM1<T2>(reduceMaxUb + i, maxVreg, pregOnePt);
        }
        LoadIfNeedCastM1<T2>(tmpVreg, inMaxUb + i, pregOnePt);
        Reg::Max(maxVreg, maxVreg, tmpVreg, pregOnePt);
        StoreIfNeedCastM1<T2>(maxUb + i, maxVreg, pregOnePt);

        SoftmaxExpSub(expMaxVreg, tmpVreg, maxVreg, pregOnePt);
        StoreIfNeedCastM1<T1>(expMaxUb + i, expMaxVreg, pregOnePt);

        Duplicate(sumVreg, 0);
        Duplicate(maxVreg, maxVreg, pregFull);
        for (uint16_t j = 0; j < repeatTimes; ++j) {
            LoadIfNeedCast<T1>(srcVreg, srcUb + i * srcK + j * FLOAT_REPEAT_SIZE, pregFull);
            SoftmaxExpSub(tmpVreg, srcVreg, maxVreg, pregFull);
            StoreIfNeedCast<T1>(dstUb + i * srcK + j * FLOAT_REPEAT_SIZE, tmpVreg, pregFull);
            Reg::Add(sumVreg, sumVreg, tmpVreg, pregFull);
        }
        Reg::ReduceSum(sumVreg, sumVreg, pregFull);
        LoadIfNeedCastM1<T2>(tmpVreg, inExpSumUb + i, pregOnePt);
        Reg::Mul(tmpVreg, expMaxVreg, tmpVreg, pregOnePt);
        Reg::Add(sumVreg, sumVreg, tmpVreg, pregOnePt);
        StoreIfNeedCastM1<T2>(expSumUb + i, sumVreg, pregOnePt);
    }
}

template <typename T1, typename T2, bool isOutputReduceMax = false>
__aicore__ inline void SoftmaxFlashV2M1NDUpdateImpl(
    const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& outReduceMax, const LocalTensor<T2>& expSumTensor,
    const LocalTensor<T2>& maxTensor, const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& expMaxTensor,
    const LocalTensor<T2>& inExpSumTensor, const LocalTensor<T2>& inMaxTensor, const LocalTensor<float>& workLocal,
    const LastAxisShapeND& originalSrcShape, const SoftMaxTiling& tiling)
{
    __ubuf__ T1* dstUb = (__ubuf__ T1*)dstTensor.GetPhyAddr();
    __ubuf__ T2* reduceMaxUb = (__ubuf__ T2*)outReduceMax.GetPhyAddr();
    __ubuf__ T2* expSumUb = (__ubuf__ T2*)expSumTensor.GetPhyAddr();
    __ubuf__ T2* inExpSumUb = (__ubuf__ T2*)inExpSumTensor.GetPhyAddr();
    __ubuf__ T2* maxUb = (__ubuf__ T2*)maxTensor.GetPhyAddr();
    __ubuf__ T2* inMaxUb = (__ubuf__ T2*)inMaxTensor.GetPhyAddr();
    __ubuf__ T1* srcUb = (__ubuf__ T1*)srcTensor.GetPhyAddr();
    __ubuf__ T1* expMaxUb = (__ubuf__ T1*)expMaxTensor.GetPhyAddr();

    SoftmaxFlashV2M1NDUpdateVFImpl<T1, T2, isOutputReduceMax>(
        dstUb, reduceMaxUb, expSumUb, inExpSumUb, maxUb, inMaxUb, srcUb, expMaxUb, originalSrcShape, tiling);
}

template <typename T1, typename T2, bool isOutputReduceMax = false>
__simd_vf__ inline void SoftmaxFlashV2M1NDWithTailUpdateVFImpl(
    __ubuf__ T1* dstUb, __ubuf__ T2* reduceMaxUb, __ubuf__ T2* expSumUb, __ubuf__ T2* inExpSumUb, __ubuf__ T2* maxUb,
    __ubuf__ T2* inMaxUb, __ubuf__ T1* srcUb, __ubuf__ T1* expMaxUb, const LastAxisShapeND originalSrcShape,
    const SoftMaxTiling tiling)
{
    uint16_t srcM = originalSrcShape.m;
    uint16_t srcK = tiling.srcK;
    uint16_t originK = originalSrcShape.k;
    uint16_t repeatTimes = CeilDivision(originK, FLOAT_REPEAT_SIZE);
    NotNumUnion notNum;
    notNum.i = F32_NEG_INF;

    Reg::MaskReg pregCnt;
    Reg::MaskReg pregFull = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    Reg::MaskReg pregOnePt = Reg::CreateMask<uint32_t, Reg::MaskPattern::VL1>();
    Reg::RegTensor<float> srcVreg;
    Reg::RegTensor<float> maxVreg;
    Reg::RegTensor<float> expMaxVreg;
    Reg::RegTensor<float> sumVreg;
    Reg::RegTensor<float> tmpVreg;
    Reg::RegTensor<float> minVreg;
    Reg::RegTensor<float> dstVreg;

    Duplicate(minVreg, notNum.f);
    for (uint16_t i = 0; i < srcM; ++i) {
        uint32_t sreg = originK;
        Duplicate(maxVreg, notNum.f);
        for (uint16_t j = 0; j < static_cast<uint16_t>(repeatTimes - 1); ++j) {
            pregCnt = Reg::UpdateMask<uint32_t>(sreg);
            LoadIfNeedCast<T1>(srcVreg, srcUb + i * srcK + j * FLOAT_REPEAT_SIZE, pregFull);
            Reg::Max(maxVreg, maxVreg, srcVreg, pregCnt);
        }
        pregCnt = Reg::UpdateMask<uint32_t>(sreg);
        LoadIfNeedCast<T1>(srcVreg, srcUb + i * srcK + (repeatTimes - 1) * FLOAT_REPEAT_SIZE, pregFull);
        Reg::Select(srcVreg, srcVreg, minVreg, pregCnt);
        Reg::Max(maxVreg, maxVreg, srcVreg, pregFull);

        Reg::ReduceMax(maxVreg, maxVreg, pregFull);
        if constexpr (isOutputReduceMax) {
            StoreIfNeedCastM1<T2>(reduceMaxUb + i, maxVreg, pregOnePt);
        }
        LoadIfNeedCastM1<T2>(tmpVreg, inMaxUb + i, pregOnePt);
        Reg::Max(maxVreg, maxVreg, tmpVreg, pregOnePt);
        StoreIfNeedCastM1<T2>(maxUb + i, maxVreg, pregOnePt);

        SoftmaxExpSub(expMaxVreg, tmpVreg, maxVreg, pregOnePt);
        StoreIfNeedCastM1<T1>(expMaxUb + i, expMaxVreg, pregOnePt);

        Duplicate(sumVreg, 0);
        Duplicate(maxVreg, maxVreg, pregFull);
        sreg = originK;
        for (uint16_t j = 0; j < repeatTimes; ++j) {
            pregCnt = Reg::UpdateMask<uint32_t>(sreg);
            LoadIfNeedCast<T1>(srcVreg, srcUb + i * srcK + j * FLOAT_REPEAT_SIZE, pregFull);
            SoftmaxExpSub(tmpVreg, srcVreg, maxVreg, pregCnt);
            StoreIfNeedCast<T1>(dstUb + i * srcK + j * FLOAT_REPEAT_SIZE, tmpVreg, pregCnt);
            Reg::Add(sumVreg, sumVreg, tmpVreg, pregFull);
        }
        Reg::ReduceSum(sumVreg, sumVreg, pregFull);
        LoadIfNeedCastM1<T2>(tmpVreg, inExpSumUb + i, pregOnePt);
        Reg::Mul(tmpVreg, expMaxVreg, tmpVreg, pregOnePt);
        Reg::Add(sumVreg, sumVreg, tmpVreg, pregOnePt);
        StoreIfNeedCastM1<T2>(expSumUb + i, sumVreg, pregOnePt);
    }
}

template <typename T1, typename T2, bool isOutputReduceMax = false>
__aicore__ inline void SoftmaxFlashV2M1NDWithTailUpdateImpl(
    const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& outReduceMax, const LocalTensor<T2>& expSumTensor,
    const LocalTensor<T2>& maxTensor, const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& expMaxTensor,
    const LocalTensor<T2>& inExpSumTensor, const LocalTensor<T2>& inMaxTensor, const LocalTensor<float>& workLocal,
    const LastAxisShapeND& originalSrcShape, const SoftMaxTiling& tiling)
{
    __ubuf__ T1* dstUb = (__ubuf__ T1*)dstTensor.GetPhyAddr();
    __ubuf__ T2* reduceMaxUb = (__ubuf__ T2*)outReduceMax.GetPhyAddr();
    __ubuf__ T2* expSumUb = (__ubuf__ T2*)expSumTensor.GetPhyAddr();
    __ubuf__ T2* inExpSumUb = (__ubuf__ T2*)inExpSumTensor.GetPhyAddr();
    __ubuf__ T2* maxUb = (__ubuf__ T2*)maxTensor.GetPhyAddr();
    __ubuf__ T2* inMaxUb = (__ubuf__ T2*)inMaxTensor.GetPhyAddr();
    __ubuf__ T1* srcUb = (__ubuf__ T1*)srcTensor.GetPhyAddr();
    __ubuf__ T1* expMaxUb = (__ubuf__ T1*)expMaxTensor.GetPhyAddr();

    SoftmaxFlashV2M1NDWithTailUpdateVFImpl<T1, T2, isOutputReduceMax>(
        dstUb, reduceMaxUb, expSumUb, inExpSumUb, maxUb, inMaxUb, srcUb, expMaxUb, originalSrcShape, tiling);
}

template <typename T1, typename T2>
__simd_vf__ inline void SoftmaxFlashV2NZNoUpdateVFImpl(
    __ubuf__ T1* dstUb, __ubuf__ T2* expSumUb, __ubuf__ T2* maxUb, __ubuf__ T1* srcUb, __ubuf__ T2* tmpUb,
    __ubuf__ float* workUb, __ubuf__ float* expUb, const LastAxisShapeND originalSrcShape, const SoftMaxTiling tiling)
{
    uint16_t srcM = tiling.srcM;
    uint16_t originK = originalSrcShape.k;
    uint16_t originM = originalSrcShape.m;
    uint16_t dataBlock = srcM * SOFTMAX_SHAPE_NZ_BASIC_COUNT;
    uint16_t mRepeatTimes = CeilDivision(dataBlock, FLOAT_REPEAT_SIZE);
    uint16_t kRepeatTimes = originK / SOFTMAX_SHAPE_NZ_BASIC_COUNT;
    uint16_t VcgFoldRepeat = CeilDivision(srcM, FLOAT_REPEAT_SIZE);
    uint16_t e2bRep = srcM / DEFAULT_BLK_NUM;
    uint16_t dtypeRepStride = IsSameType<T2, half>::value ? HALF_REPEAT_SIZE : FLOAT_REPEAT_SIZE;
    uint16_t dtypeBlkStride = dtypeRepStride / DEFAULT_BLK_NUM;

    NotNumUnion notNum;
    notNum.i = F32_NEG_INF;

    Reg::MaskReg pregCnt;
    Reg::MaskReg pregFull = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    Reg::MaskReg pregOneBlk = Reg::CreateMask<uint32_t, Reg::MaskPattern::VL8>();
    Reg::RegTensor<float> srcVreg;
    Reg::RegTensor<float> maxVreg;
    Reg::RegTensor<float> sumVreg;
    Reg::RegTensor<float> tmpVreg;
    Reg::RegTensor<float> dstVreg;
    Reg::RegTensor<T2> castReg;

    // reducemax
    for (uint16_t i = 0; i < mRepeatTimes; ++i) {
        Duplicate(maxVreg, notNum.f);
        for (uint16_t j = 0; j < kRepeatTimes; ++j) {
            LoadIfNeedCast<T1>(srcVreg, srcUb + i * FLOAT_REPEAT_SIZE + j * dataBlock, pregFull);
            Reg::Max(maxVreg, maxVreg, srcVreg, pregFull);
        }
        Reg::ReduceMaxWithDataBlock(maxVreg, maxVreg, pregFull);
        Reg::StoreAlign<float, Reg::StoreDist::DIST_NORM>(workUb + i * DEFAULT_BLK_NUM, maxVreg, pregOneBlk);
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    for (uint16_t i = 0; i < VcgFoldRepeat; ++i) {
        Reg::LoadAlign<float, Reg::LoadDist::DIST_DINTLV_B32>(maxVreg, tmpVreg, workUb + i * HALF_REPEAT_SIZE);
        Reg::Max(maxVreg, maxVreg, tmpVreg, pregFull);
        StoreIfNeedCast<T2>(tmpUb + i * FLOAT_REPEAT_SIZE, maxVreg, pregFull);
        Reg::StoreAlign<float, Reg::StoreDist::DIST_INTLV_B32>(
            workUb + i * HALF_REPEAT_SIZE, maxVreg, maxVreg, pregFull);
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    uint32_t sreg = originM * dtypeBlkStride;
    for (uint16_t i = 0; i < e2bRep; ++i) {
        pregCnt = Reg::UpdateMask<T2>(sreg);
        LoadE2B<T2>(castReg, tmpUb + i * DEFAULT_BLK_NUM);
        Reg::StoreAlign(maxUb + i * dtypeRepStride, castReg, pregCnt);
    }

    for (uint16_t j = 0; j < kRepeatTimes; ++j) {
        sreg = originM * SOFTMAX_SHAPE_NZ_BASIC_COUNT;
        for (uint16_t i = 0; i < mRepeatTimes; ++i) {
            pregCnt = Reg::UpdateMask<uint32_t>(sreg);
            LoadE2B<float>(maxVreg, workUb + i * DEFAULT_BLK_NUM);
            LoadIfNeedCast<T1>(srcVreg, srcUb + i * FLOAT_REPEAT_SIZE + j * dataBlock, pregFull);
            SoftmaxExpSub(tmpVreg, srcVreg, maxVreg, pregFull);
            StoreIfNeedCast<T1>(dstUb + i * FLOAT_REPEAT_SIZE + j * dataBlock, tmpVreg, pregCnt);
            if constexpr (sizeof(T1) == 2) {
                Reg::StoreAlign(expUb + i * FLOAT_REPEAT_SIZE + j * dataBlock, tmpVreg, pregCnt);
            }
        }
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    // reducesum
    for (uint16_t i = 0; i < mRepeatTimes; ++i) {
        Duplicate(sumVreg, 0);
        for (uint16_t j = 0; j < kRepeatTimes; ++j) {
            if constexpr (sizeof(T1) == 2) {
                Reg::LoadAlign(srcVreg, expUb + i * FLOAT_REPEAT_SIZE + j * dataBlock);
            } else {
                Reg::LoadAlign(srcVreg, dstUb + i * FLOAT_REPEAT_SIZE + j * dataBlock);
            }
            Reg::Add(sumVreg, sumVreg, srcVreg, pregFull);
        }
        Reg::ReduceSumWithDataBlock(sumVreg, sumVreg, pregFull);
        Reg::StoreAlign<float, Reg::StoreDist::DIST_NORM>(workUb + i * DEFAULT_BLK_NUM, sumVreg, pregOneBlk);
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    for (uint16_t i = 0; i < VcgFoldRepeat; ++i) {
        Reg::LoadAlign<float, Reg::LoadDist::DIST_DINTLV_B32>(sumVreg, tmpVreg, workUb + i * HALF_REPEAT_SIZE);
        Reg::Add(sumVreg, sumVreg, tmpVreg, pregFull);
        StoreIfNeedCast<T2>(tmpUb + i * FLOAT_REPEAT_SIZE, sumVreg, pregFull);
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    sreg = originM * dtypeBlkStride;
    for (uint16_t i = 0; i < e2bRep; ++i) {
        pregCnt = Reg::UpdateMask<T2>(sreg);
        LoadE2B<T2>(castReg, tmpUb + i * DEFAULT_BLK_NUM);
        Reg::StoreAlign(expSumUb + i * dtypeRepStride, castReg, pregCnt);
    }
}

template <typename T1, typename T2>
__aicore__ inline void SoftmaxFlashV2NZNoUpdateImpl(
    const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& expSumTensor, const LocalTensor<T2>& maxTensor,
    const LocalTensor<T1>& srcTensor, const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape,
    const SoftMaxTiling& tiling)
{
    uint16_t srcM = tiling.srcM;
    uint16_t VcgFoldRepeat = CeilDivision(srcM, FLOAT_REPEAT_SIZE);
    uint16_t offset = CeilDivision(srcM * SOFTMAX_COMPUTE_DIM, HALF_REPEAT_SIZE) * HALF_REPEAT_SIZE;

    __ubuf__ T1* dstUb = (__ubuf__ T1*)dstTensor.GetPhyAddr();
    __ubuf__ T2* expSumUb = (__ubuf__ T2*)expSumTensor.GetPhyAddr();
    __ubuf__ T2* maxUb = (__ubuf__ T2*)maxTensor.GetPhyAddr();
    __ubuf__ T1* srcUb = (__ubuf__ T1*)srcTensor.GetPhyAddr();
    __ubuf__ T2* tmpUb = (__ubuf__ T2*)workLocal.GetPhyAddr();
    __ubuf__ float* workUb = (__ubuf__ float*)workLocal.GetPhyAddr(VcgFoldRepeat * FLOAT_REPEAT_SIZE);
    __ubuf__ float* expUb = (__ubuf__ float*)workLocal.GetPhyAddr(VcgFoldRepeat * FLOAT_REPEAT_SIZE + offset);

    SoftmaxFlashV2NZNoUpdateVFImpl<T1, T2>(
        dstUb, expSumUb, maxUb, srcUb, tmpUb, workUb, expUb, originalSrcShape, tiling);
}

template <typename T1, typename T2>
__simd_vf__ inline void SoftmaxFlashV2NZWithTailNoUpdateVFImpl(
    __ubuf__ T1* dstUb, __ubuf__ T2* expSumUb, __ubuf__ T2* maxUb, __ubuf__ T1* srcUb, __ubuf__ T2* tmpUb,
    __ubuf__ float* workUb, __ubuf__ float* expUb, const LastAxisShapeND originalSrcShape, const SoftMaxTiling tiling)
{
    uint16_t srcM = tiling.srcM;
    uint16_t originK = originalSrcShape.k;
    uint16_t originM = originalSrcShape.m;

    uint16_t dataBlock = srcM * SOFTMAX_SHAPE_NZ_BASIC_COUNT;
    uint16_t mRepeatTimes = CeilDivision(dataBlock, FLOAT_REPEAT_SIZE);
    uint16_t kRepeatTimes = originK / SOFTMAX_SHAPE_NZ_BASIC_COUNT;
    uint16_t VcgFoldRepeat = CeilDivision(srcM, FLOAT_REPEAT_SIZE);
    uint16_t e2bRep = srcM / DEFAULT_BLK_NUM;
    uint16_t dtypeRepStride = IsSameType<T2, half>::value ? HALF_REPEAT_SIZE : FLOAT_REPEAT_SIZE;
    uint16_t dtypeBlkStride = dtypeRepStride / DEFAULT_BLK_NUM;
    NotNumUnion notNum;
    notNum.i = F32_NEG_INF;

    Reg::MaskReg pregDst;
    Reg::MaskReg pregkTail = Reg::MoveMask<uint32_t>();
    Reg::MaskReg pregCnt;
    Reg::MaskReg pregFull = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    Reg::MaskReg pregOneBlk = Reg::CreateMask<uint32_t, Reg::MaskPattern::VL8>();
    Reg::RegTensor<float> srcVreg;
    Reg::RegTensor<float> maxVreg;
    Reg::RegTensor<float> sumVreg;
    Reg::RegTensor<float> tmpVreg;
    Reg::RegTensor<float> minVreg;
    Reg::RegTensor<float> dstVreg;
    Reg::RegTensor<T2> castReg;

    // reducemax
    Duplicate(minVreg, notNum.f);
    for (uint16_t i = 0; i < mRepeatTimes; ++i) {
        Duplicate(maxVreg, notNum.f);
        for (uint16_t j = 0; j < kRepeatTimes; ++j) {
            LoadIfNeedCast<T1>(srcVreg, srcUb + i * FLOAT_REPEAT_SIZE + j * dataBlock, pregFull);
            Reg::Max(maxVreg, maxVreg, srcVreg, pregFull);
        }
        LoadIfNeedCast<T1>(srcVreg, srcUb + i * FLOAT_REPEAT_SIZE + kRepeatTimes * dataBlock, pregFull);
        Reg::Select(srcVreg, srcVreg, minVreg, pregkTail);
        Reg::Max(maxVreg, maxVreg, srcVreg, pregFull);

        Reg::ReduceMaxWithDataBlock(maxVreg, maxVreg, pregFull);
        Reg::StoreAlign<float, Reg::StoreDist::DIST_NORM>(workUb + i * DEFAULT_BLK_NUM, maxVreg, pregOneBlk);
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    for (uint16_t i = 0; i < VcgFoldRepeat; ++i) {
        Reg::LoadAlign<float, Reg::LoadDist::DIST_DINTLV_B32>(maxVreg, tmpVreg, workUb + i * HALF_REPEAT_SIZE);
        Reg::Max(maxVreg, maxVreg, tmpVreg, pregFull);
        StoreIfNeedCast<T2>(tmpUb + i * FLOAT_REPEAT_SIZE, maxVreg, pregFull);
        Reg::StoreAlign<float, Reg::StoreDist::DIST_INTLV_B32>(
            workUb + i * HALF_REPEAT_SIZE, maxVreg, maxVreg, pregFull);
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    uint32_t sreg = originM * dtypeBlkStride;
    for (uint16_t i = 0; i < e2bRep; ++i) {
        pregCnt = Reg::UpdateMask<T2>(sreg);
        LoadE2B<T2>(castReg, tmpUb + i * DEFAULT_BLK_NUM);
        Reg::StoreAlign(maxUb + i * dtypeRepStride, castReg, pregCnt);
    }

    for (uint16_t j = 0; j < kRepeatTimes; ++j) {
        sreg = originM * SOFTMAX_SHAPE_NZ_BASIC_COUNT;
        for (uint16_t i = 0; i < mRepeatTimes; ++i) {
            pregCnt = Reg::UpdateMask<uint32_t>(sreg);
            LoadE2B<float>(maxVreg, workUb + i * DEFAULT_BLK_NUM);
            LoadIfNeedCast<T1>(srcVreg, srcUb + i * FLOAT_REPEAT_SIZE + j * dataBlock, pregFull);
            SoftmaxExpSub(tmpVreg, srcVreg, maxVreg, pregFull);
            StoreIfNeedCast<T1>(dstUb + i * FLOAT_REPEAT_SIZE + j * dataBlock, tmpVreg, pregCnt);
            if constexpr (sizeof(T1) == 2) {
                Reg::StoreAlign(expUb + i * FLOAT_REPEAT_SIZE + j * dataBlock, tmpVreg, pregCnt);
            }
        }
    }
    sreg = originM * SOFTMAX_SHAPE_NZ_BASIC_COUNT;
    for (uint16_t i = 0; i < mRepeatTimes; ++i) {
        pregCnt = Reg::UpdateMask<uint32_t>(sreg);
        LoadE2B<float>(maxVreg, workUb + i * DEFAULT_BLK_NUM);
        LoadIfNeedCast<T1>(srcVreg, srcUb + i * FLOAT_REPEAT_SIZE + kRepeatTimes * dataBlock, pregFull);
        Reg::MaskAnd(pregDst, pregkTail, pregCnt, pregFull);
        SoftmaxExpSub(tmpVreg, srcVreg, maxVreg, pregDst);
        StoreIfNeedCast<T1>(dstUb + i * FLOAT_REPEAT_SIZE + kRepeatTimes * dataBlock, tmpVreg, pregDst);
        if constexpr (sizeof(T1) == 2) {
            Reg::StoreAlign(expUb + i * FLOAT_REPEAT_SIZE + kRepeatTimes * dataBlock, tmpVreg, pregDst);
        }
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    // reducesum
    Duplicate(minVreg, 0);
    for (uint16_t i = 0; i < mRepeatTimes; ++i) {
        Duplicate(sumVreg, 0);
        for (uint16_t j = 0; j < kRepeatTimes; ++j) {
            if constexpr (sizeof(T1) == 2) {
                Reg::LoadAlign(srcVreg, expUb + i * FLOAT_REPEAT_SIZE + j * dataBlock);
            } else {
                Reg::LoadAlign(srcVreg, dstUb + i * FLOAT_REPEAT_SIZE + j * dataBlock);
            }
            Reg::Add(sumVreg, sumVreg, srcVreg, pregFull);
        }
        if constexpr (sizeof(T1) == 2) {
            Reg::LoadAlign(srcVreg, expUb + i * FLOAT_REPEAT_SIZE + kRepeatTimes * dataBlock);
        } else {
            Reg::LoadAlign(srcVreg, dstUb + i * FLOAT_REPEAT_SIZE + kRepeatTimes * dataBlock);
        }
        Reg::Select(srcVreg, srcVreg, minVreg, pregkTail);
        Reg::Add(sumVreg, sumVreg, srcVreg, pregFull);

        Reg::ReduceSumWithDataBlock(sumVreg, sumVreg, pregFull);
        Reg::StoreAlign<float, Reg::StoreDist::DIST_NORM>(workUb + i * DEFAULT_BLK_NUM, sumVreg, pregOneBlk);
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    for (uint16_t i = 0; i < VcgFoldRepeat; ++i) {
        Reg::LoadAlign<float, Reg::LoadDist::DIST_DINTLV_B32>(sumVreg, tmpVreg, workUb + i * HALF_REPEAT_SIZE);
        Reg::Add(sumVreg, sumVreg, tmpVreg, pregFull);
        StoreIfNeedCast<T2>(tmpUb + i * FLOAT_REPEAT_SIZE, sumVreg, pregFull);
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    sreg = originM * dtypeBlkStride;
    for (uint16_t i = 0; i < e2bRep; ++i) {
        pregCnt = Reg::UpdateMask<T2>(sreg);
        LoadE2B<T2>(castReg, tmpUb + i * DEFAULT_BLK_NUM);
        Reg::StoreAlign(expSumUb + i * dtypeRepStride, castReg, pregCnt);
    }
}

template <typename T1, typename T2>
__aicore__ inline void SoftmaxFlashV2NZWithTailNoUpdateImpl(
    const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& expSumTensor, const LocalTensor<T2>& maxTensor,
    const LocalTensor<T1>& srcTensor, const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape,
    const SoftMaxTiling& tiling)
{
    uint16_t srcM = tiling.srcM;
    uint16_t originK = originalSrcShape.k;
    uint16_t VcgFoldRepeat = CeilDivision(srcM, FLOAT_REPEAT_SIZE);
    uint16_t offset = CeilDivision(srcM * SOFTMAX_COMPUTE_DIM, HALF_REPEAT_SIZE) * HALF_REPEAT_SIZE;
    uint64_t mask[2] = {0, 0};
    CreateSpecialFormatMask(
        mask[0], originK % SOFTMAX_SHAPE_NZ_BASIC_COUNT, FLOAT_REPEAT_SIZE / SOFTMAX_SHAPE_NZ_BASIC_COUNT);
    SetVectorMask<uint32_t>(mask[1], mask[0]);

    __ubuf__ T1* dstUb = (__ubuf__ T1*)dstTensor.GetPhyAddr();
    __ubuf__ T2* expSumUb = (__ubuf__ T2*)expSumTensor.GetPhyAddr();
    __ubuf__ T2* maxUb = (__ubuf__ T2*)maxTensor.GetPhyAddr();
    __ubuf__ T1* srcUb = (__ubuf__ T1*)srcTensor.GetPhyAddr();
    __ubuf__ T2* tmpUb = (__ubuf__ T2*)workLocal.GetPhyAddr();
    __ubuf__ float* workUb = (__ubuf__ float*)workLocal.GetPhyAddr(VcgFoldRepeat * FLOAT_REPEAT_SIZE);
    __ubuf__ float* expUb = (__ubuf__ float*)workLocal.GetPhyAddr(VcgFoldRepeat * FLOAT_REPEAT_SIZE + offset);

    SoftmaxFlashV2NZWithTailNoUpdateVFImpl<T1, T2>(
        dstUb, expSumUb, maxUb, srcUb, tmpUb, workUb, expUb, originalSrcShape, tiling);
}

template <typename T1, typename T2>
__simd_vf__ inline void SoftmaxFlashV2NZUpdateVFImpl(
    __ubuf__ T1* dstUb, __ubuf__ T2* expSumUb, __ubuf__ T2* inExpSumUb, __ubuf__ T2* maxUb, __ubuf__ T2* inMaxUb,
    __ubuf__ T1* srcUb, __ubuf__ T1* expMaxUb, __ubuf__ float* expMaxF32Ub, __ubuf__ float* workUb,
    __ubuf__ float* tmpUb, __ubuf__ float* expUb, const LastAxisShapeND originalSrcShape, const SoftMaxTiling tiling)
{
    uint16_t srcM = tiling.srcM;
    uint16_t originK = originalSrcShape.k;
    uint16_t originM = originalSrcShape.m;

    uint16_t dataBlock = srcM * SOFTMAX_SHAPE_NZ_BASIC_COUNT;
    uint16_t mRepeatTimes = CeilDivision(dataBlock, FLOAT_REPEAT_SIZE);
    uint16_t kRepeatTimes = originK / SOFTMAX_SHAPE_NZ_BASIC_COUNT;
    uint16_t VcgFoldRepeat = CeilDivision(srcM, FLOAT_REPEAT_SIZE);
    uint16_t e2bRep = IsSameType<T2, float>::value ? srcM / DEFAULT_BLK_NUM : mRepeatTimes;
    uint16_t dtypeRepStride = IsSameType<T2, half>::value ? HALF_REPEAT_SIZE : FLOAT_REPEAT_SIZE;
    uint16_t dtypeBlkStride = dtypeRepStride / DEFAULT_BLK_NUM;
    uint32_t expMaxBlkStride = ONE_BLOCK_SIZE / sizeof(T1);

    NotNumUnion notNum;
    notNum.i = F32_NEG_INF;

    Reg::MaskReg pregCnt;
    Reg::MaskReg pregFull = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    Reg::MaskReg pregOneBlk = Reg::CreateMask<uint32_t, Reg::MaskPattern::VL8>();
    Reg::RegTensor<float> srcVreg;
    Reg::RegTensor<float> maxVreg;
    Reg::RegTensor<float> inMaxVreg;
    Reg::RegTensor<float> sumVreg;
    Reg::RegTensor<float> tmpVreg;
    Reg::RegTensor<float> dstVreg;
    Reg::RegTensor<T1> t1Reg;
    Reg::RegTensor<T1> dummyReg;

    // reducemax
    for (uint16_t i = 0; i < mRepeatTimes; ++i) {
        Duplicate(maxVreg, notNum.f);
        for (uint16_t j = 0; j < kRepeatTimes; ++j) {
            LoadIfNeedCast<T1>(srcVreg, srcUb + i * FLOAT_REPEAT_SIZE + j * dataBlock, pregFull);
            Reg::Max(maxVreg, maxVreg, srcVreg, pregFull);
        }
        Reg::ReduceMaxWithDataBlock(maxVreg, maxVreg, pregFull);
        Reg::StoreAlign<float, Reg::StoreDist::DIST_NORM>(workUb + i * DEFAULT_BLK_NUM, maxVreg, pregOneBlk);
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    for (uint16_t i = 0; i < VcgFoldRepeat; ++i) {
        Reg::LoadAlign<float, Reg::LoadDist::DIST_DINTLV_B32>(maxVreg, tmpVreg, workUb + i * HALF_REPEAT_SIZE);
        Reg::Max(maxVreg, maxVreg, tmpVreg, pregFull);
        if constexpr (sizeof(T2) == 4) {
            Reg::StoreAlign(workUb + i * FLOAT_REPEAT_SIZE, maxVreg, pregFull);
        } else {
            Reg::StoreAlign<float, Reg::StoreDist::DIST_INTLV_B32>(
                workUb + i * HALF_REPEAT_SIZE, maxVreg, maxVreg, pregFull);
        }
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    uint32_t sreg = originM * dtypeBlkStride;
    for (uint16_t i = 0; i < e2bRep; ++i) {
        pregCnt = Reg::UpdateMask<uint32_t>(sreg);
        LoadE2B<float>(maxVreg, workUb + i * DEFAULT_BLK_NUM);
        LoadIfNeedCast<T2>(inMaxVreg, inMaxUb + i * FLOAT_REPEAT_SIZE, pregCnt);
        Reg::Max(maxVreg, inMaxVreg, maxVreg, pregCnt);
        StoreIfNeedCast<T2>(maxUb + i * FLOAT_REPEAT_SIZE, maxVreg, pregCnt);
        if constexpr (sizeof(T2) == 4) {
            Reg::StoreAlign<float, Reg::StoreDist::DIST_INTLV_B32>(
                tmpUb + i * HALF_REPEAT_SIZE, maxVreg, maxVreg, pregFull);
        } else {
            Reg::StoreAlign(tmpUb + i * FLOAT_REPEAT_SIZE, maxVreg, pregFull);
        }
        SoftmaxExpSub(dstVreg, inMaxVreg, maxVreg, pregCnt);
        Reg::StoreAlign(expMaxF32Ub + i * FLOAT_REPEAT_SIZE, dstVreg, pregCnt);
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    for (uint16_t j = 0; j < kRepeatTimes; ++j) {
        sreg = originM * SOFTMAX_SHAPE_NZ_BASIC_COUNT;
        for (uint16_t i = 0; i < mRepeatTimes; ++i) {
            pregCnt = Reg::UpdateMask<uint32_t>(sreg);
            Reg::LoadAlign(maxVreg, tmpUb + i * FLOAT_REPEAT_SIZE);
            LoadIfNeedCast<T1>(srcVreg, srcUb + i * FLOAT_REPEAT_SIZE + j * dataBlock, pregFull);
            SoftmaxExpSub(tmpVreg, srcVreg, maxVreg, pregCnt);
            StoreIfNeedCast<T1>(dstUb + i * FLOAT_REPEAT_SIZE + j * dataBlock, tmpVreg, pregCnt);
            if constexpr (sizeof(T1) == 2) {
                Reg::StoreAlign(expUb + i * FLOAT_REPEAT_SIZE + j * dataBlock, tmpVreg, pregCnt);
            }
        }
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    // reducesum
    for (uint16_t i = 0; i < mRepeatTimes; ++i) {
        Duplicate(sumVreg, 0);
        for (uint16_t j = 0; j < kRepeatTimes; ++j) {
            if constexpr (sizeof(T1) == 2) {
                Reg::LoadAlign(tmpVreg, expUb + i * FLOAT_REPEAT_SIZE + j * dataBlock);
            } else {
                Reg::LoadAlign(tmpVreg, dstUb + i * FLOAT_REPEAT_SIZE + j * dataBlock);
            }
            Reg::Add(sumVreg, sumVreg, tmpVreg, pregFull);
        }
        Reg::ReduceSumWithDataBlock(sumVreg, sumVreg, pregFull);
        Reg::StoreAlign<float, Reg::StoreDist::DIST_NORM>(workUb + i * DEFAULT_BLK_NUM, sumVreg, pregOneBlk);
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    for (uint16_t i = 0; i < VcgFoldRepeat; ++i) {
        Reg::LoadAlign<float, Reg::LoadDist::DIST_DINTLV_B32>(sumVreg, tmpVreg, workUb + i * HALF_REPEAT_SIZE);
        Reg::Add(sumVreg, sumVreg, tmpVreg, pregFull);
        if constexpr (sizeof(T2) == 4) {
            Reg::StoreAlign(workUb + i * FLOAT_REPEAT_SIZE, sumVreg, pregFull);
        } else {
            Reg::StoreAlign<float, Reg::StoreDist::DIST_INTLV_B32>(
                workUb + i * HALF_REPEAT_SIZE, sumVreg, sumVreg, pregFull);
        }
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    sreg = originM * dtypeBlkStride;
    uint32_t sreg1 = originM * expMaxBlkStride;
    for (uint16_t i = 0; i < e2bRep; ++i) {
        pregCnt = Reg::UpdateMask<uint32_t>(sreg);
        LoadE2B<float>(sumVreg, workUb + i * DEFAULT_BLK_NUM);
        LoadIfNeedCast<T2>(tmpVreg, inExpSumUb + i * FLOAT_REPEAT_SIZE, pregCnt);
        Reg::LoadAlign(maxVreg, expMaxF32Ub + i * FLOAT_REPEAT_SIZE);
        Reg::Mul(dstVreg, maxVreg, tmpVreg, pregCnt);
        Reg::Add(sumVreg, sumVreg, dstVreg, pregCnt);
        StoreIfNeedCast<T2>(expSumUb + i * FLOAT_REPEAT_SIZE, sumVreg, pregCnt);
        if constexpr (sizeof(T1) == 2 && sizeof(T2) == 4) {
            pregOneBlk = Reg::UpdateMask<uint16_t>(sreg1);
            Reg::Cast<T1, float, Internal::castTraitB32ToB16>(t1Reg, maxVreg, pregCnt);
            Reg::Pack<uint16_t, uint32_t>((Reg::RegTensor<uint16_t>&)t1Reg, (Reg::RegTensor<uint32_t>&)t1Reg);
            Reg::Interleave(t1Reg, dummyReg, t1Reg, t1Reg);
            Reg::StoreAlign<T1>(expMaxUb + i * HALF_REPEAT_SIZE, t1Reg, pregOneBlk);
        } else if constexpr (sizeof(T1) == 2 && sizeof(T2) == 2) {
            StoreIfNeedCast<T1>(expMaxUb + i * FLOAT_REPEAT_SIZE, maxVreg, pregCnt);
        }
    }
}

template <typename T1, typename T2>
__aicore__ inline void SoftmaxFlashV2NZUpdateImpl(
    const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& expSumTensor, const LocalTensor<T2>& maxTensor,
    const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& expMaxTensor, const LocalTensor<T2>& inExpSumTensor,
    const LocalTensor<T2>& inMaxTensor, const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape,
    const SoftMaxTiling& tiling)
{
    uint16_t srcM = tiling.srcM;
    uint16_t originK = originalSrcShape.k;
    uint16_t offset;
    if constexpr (sizeof(T2) == 4) {
        offset = CeilDivision(srcM, FLOAT_REPEAT_SIZE) * FLOAT_REPEAT_SIZE;
    } else {
        offset = CeilDivision(srcM * SOFTMAX_COMPUTE_DIM, HALF_REPEAT_SIZE) * HALF_REPEAT_SIZE;
    }
    uint16_t offset1 = srcM * SOFTMAX_SHAPE_NZ_BASIC_COUNT;

    __ubuf__ T1* dstUb = (__ubuf__ T1*)dstTensor.GetPhyAddr();
    __ubuf__ T2* expSumUb = (__ubuf__ T2*)expSumTensor.GetPhyAddr();
    __ubuf__ T2* inExpSumUb = (__ubuf__ T2*)inExpSumTensor.GetPhyAddr();
    __ubuf__ T2* maxUb = (__ubuf__ T2*)maxTensor.GetPhyAddr();
    __ubuf__ T2* inMaxUb = (__ubuf__ T2*)inMaxTensor.GetPhyAddr();
    __ubuf__ T1* srcUb = (__ubuf__ T1*)srcTensor.GetPhyAddr();
    __ubuf__ T1* expMaxUb = (__ubuf__ T1*)expMaxTensor.GetPhyAddr();
    __ubuf__ float* expMaxF32Ub = (__ubuf__ float*)expMaxTensor.GetPhyAddr();
    __ubuf__ float* workUb = (__ubuf__ float*)workLocal.GetPhyAddr();
    __ubuf__ float* tmpUb = (__ubuf__ float*)workLocal.GetPhyAddr(offset);
    __ubuf__ float* expUb = (__ubuf__ float*)workLocal.GetPhyAddr(offset + offset1);
    if constexpr (sizeof(T1) == 2) {
        expMaxF32Ub = (__ubuf__ float*)workLocal.GetPhyAddr(offset + offset1 + srcM * originK);
    }

    SoftmaxFlashV2NZUpdateVFImpl<T1, T2>(
        dstUb, expSumUb, inExpSumUb, maxUb, inMaxUb, srcUb, expMaxUb, expMaxF32Ub, workUb, tmpUb, expUb,
        originalSrcShape, tiling);
}

template <typename T1, typename T2>
__simd_vf__ inline void SoftmaxFlashV2NZWithTailUpdateVFImpl(
    __ubuf__ T1* dstUb, __ubuf__ T2* expSumUb, __ubuf__ T2* inExpSumUb, __ubuf__ T2* maxUb, __ubuf__ T2* inMaxUb,
    __ubuf__ T1* srcUb, __ubuf__ T1* expMaxUb, __ubuf__ float* expMaxF32Ub, __ubuf__ float* workUb,
    __ubuf__ float* tmpUb, __ubuf__ float* expUb, const LastAxisShapeND originalSrcShape, const SoftMaxTiling tiling)
{
    uint16_t srcM = tiling.srcM;
    uint16_t originK = originalSrcShape.k;
    uint16_t originM = originalSrcShape.m;
    uint16_t dataBlock = srcM * SOFTMAX_SHAPE_NZ_BASIC_COUNT;
    uint16_t mRepeatTimes = CeilDivision(dataBlock, FLOAT_REPEAT_SIZE);
    uint16_t kRepeatTimes = originK / SOFTMAX_SHAPE_NZ_BASIC_COUNT;
    uint16_t VcgFoldRepeat = CeilDivision(srcM, FLOAT_REPEAT_SIZE);
    uint16_t e2bRep = IsSameType<T2, float>::value ? srcM / DEFAULT_BLK_NUM : mRepeatTimes;
    uint16_t dtypeRepStride = IsSameType<T2, half>::value ? HALF_REPEAT_SIZE : FLOAT_REPEAT_SIZE;
    uint16_t dtypeBlkStride = dtypeRepStride / DEFAULT_BLK_NUM;
    uint32_t expMaxBlkStride = ONE_BLOCK_SIZE / sizeof(T1);
    NotNumUnion notNum;
    notNum.i = F32_NEG_INF;

    Reg::MaskReg pregDst;
    Reg::MaskReg pregkTail = Reg::MoveMask<uint32_t>();
    Reg::MaskReg pregCnt;
    Reg::MaskReg pregFull = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    Reg::MaskReg pregOneBlk = Reg::CreateMask<uint32_t, Reg::MaskPattern::VL8>();
    Reg::RegTensor<float> srcVreg;
    Reg::RegTensor<float> maxVreg;
    Reg::RegTensor<float> inMaxVreg;
    Reg::RegTensor<float> sumVreg;
    Reg::RegTensor<float> tmpVreg;
    Reg::RegTensor<float> minVreg;
    Reg::RegTensor<float> dstVreg;
    Reg::RegTensor<T1> t1Reg;
    Reg::RegTensor<T1> dummyReg;

    // reducemax
    Duplicate(minVreg, notNum.f);
    for (uint16_t i = 0; i < mRepeatTimes; ++i) {
        Duplicate(maxVreg, notNum.f);
        for (uint16_t j = 0; j < kRepeatTimes; ++j) {
            LoadIfNeedCast<T1>(srcVreg, srcUb + i * FLOAT_REPEAT_SIZE + j * dataBlock, pregFull);
            Reg::Max(maxVreg, maxVreg, srcVreg, pregFull);
        }
        LoadIfNeedCast<T1>(srcVreg, srcUb + i * FLOAT_REPEAT_SIZE + kRepeatTimes * dataBlock, pregFull);
        Reg::Select(srcVreg, srcVreg, minVreg, pregkTail);
        Reg::Max(maxVreg, maxVreg, srcVreg, pregFull);

        Reg::ReduceMaxWithDataBlock(maxVreg, maxVreg, pregFull);
        Reg::StoreAlign<float, Reg::StoreDist::DIST_NORM>(workUb + i * DEFAULT_BLK_NUM, maxVreg, pregOneBlk);
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    for (uint16_t i = 0; i < VcgFoldRepeat; ++i) {
        Reg::LoadAlign<float, Reg::LoadDist::DIST_DINTLV_B32>(maxVreg, tmpVreg, workUb + i * HALF_REPEAT_SIZE);
        Reg::Max(maxVreg, maxVreg, tmpVreg, pregFull);
        if constexpr (sizeof(T2) == 4) {
            Reg::StoreAlign(workUb + i * FLOAT_REPEAT_SIZE, maxVreg, pregFull);
        } else {
            Reg::StoreAlign<float, Reg::StoreDist::DIST_INTLV_B32>(
                workUb + i * HALF_REPEAT_SIZE, maxVreg, maxVreg, pregFull);
        }
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    uint32_t sreg = originM * dtypeBlkStride;
    for (uint16_t i = 0; i < e2bRep; ++i) {
        pregCnt = Reg::UpdateMask<uint32_t>(sreg);
        LoadE2B<float>(maxVreg, workUb + i * DEFAULT_BLK_NUM);
        LoadIfNeedCast<T2>(inMaxVreg, inMaxUb + i * FLOAT_REPEAT_SIZE, pregCnt);
        Reg::Max(maxVreg, inMaxVreg, maxVreg, pregCnt);
        StoreIfNeedCast<T2>(maxUb + i * FLOAT_REPEAT_SIZE, maxVreg, pregCnt);
        if constexpr (sizeof(T2) == 4) {
            Reg::StoreAlign<float, Reg::StoreDist::DIST_INTLV_B32>(
                tmpUb + i * HALF_REPEAT_SIZE, maxVreg, maxVreg, pregFull);
        } else {
            Reg::StoreAlign(tmpUb + i * FLOAT_REPEAT_SIZE, maxVreg, pregFull);
        }
        SoftmaxExpSub(dstVreg, inMaxVreg, maxVreg, pregCnt);
        Reg::StoreAlign(expMaxF32Ub + i * FLOAT_REPEAT_SIZE, dstVreg, pregCnt);
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    for (uint16_t j = 0; j < kRepeatTimes; ++j) {
        sreg = originM * SOFTMAX_SHAPE_NZ_BASIC_COUNT;
        for (uint16_t i = 0; i < mRepeatTimes; ++i) {
            pregCnt = Reg::UpdateMask<uint32_t>(sreg);
            Reg::LoadAlign(maxVreg, tmpUb + i * FLOAT_REPEAT_SIZE);
            LoadIfNeedCast<T1>(srcVreg, srcUb + i * FLOAT_REPEAT_SIZE + j * dataBlock, pregFull);
            SoftmaxExpSub(tmpVreg, srcVreg, maxVreg, pregCnt);
            StoreIfNeedCast<T1>(dstUb + i * FLOAT_REPEAT_SIZE + j * dataBlock, tmpVreg, pregCnt);
            if constexpr (sizeof(T1) == 2) {
                Reg::StoreAlign(expUb + i * FLOAT_REPEAT_SIZE + j * dataBlock, tmpVreg, pregCnt);
            }
        }
    }
    sreg = originM * SOFTMAX_SHAPE_NZ_BASIC_COUNT;
    for (uint16_t i = 0; i < mRepeatTimes; ++i) {
        pregCnt = Reg::UpdateMask<uint32_t>(sreg);
        Reg::LoadAlign(maxVreg, tmpUb + i * FLOAT_REPEAT_SIZE);
        LoadIfNeedCast<T1>(srcVreg, srcUb + i * FLOAT_REPEAT_SIZE + kRepeatTimes * dataBlock, pregFull);
        Reg::MaskAnd(pregDst, pregkTail, pregCnt, pregFull);
        SoftmaxExpSub(tmpVreg, srcVreg, maxVreg, pregDst);
        StoreIfNeedCast<T1>(dstUb + i * FLOAT_REPEAT_SIZE + kRepeatTimes * dataBlock, tmpVreg, pregDst);
        if constexpr (sizeof(T1) == 2) {
            Reg::StoreAlign(expUb + i * FLOAT_REPEAT_SIZE + kRepeatTimes * dataBlock, tmpVreg, pregDst);
        }
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    // reducesum
    Duplicate(minVreg, 0);
    for (uint16_t i = 0; i < mRepeatTimes; ++i) {
        Duplicate(sumVreg, 0);
        for (uint16_t j = 0; j < kRepeatTimes; ++j) {
            if constexpr (sizeof(T1) == 2) {
                Reg::LoadAlign(tmpVreg, expUb + i * FLOAT_REPEAT_SIZE + j * dataBlock);
            } else {
                Reg::LoadAlign(tmpVreg, dstUb + i * FLOAT_REPEAT_SIZE + j * dataBlock);
            }
            Reg::Add(sumVreg, sumVreg, tmpVreg, pregFull);
        }
        if constexpr (sizeof(T1) == 2) {
            Reg::LoadAlign(tmpVreg, expUb + i * FLOAT_REPEAT_SIZE + kRepeatTimes * dataBlock);
        } else {
            Reg::LoadAlign(tmpVreg, dstUb + i * FLOAT_REPEAT_SIZE + kRepeatTimes * dataBlock);
        }
        Reg::Select(tmpVreg, tmpVreg, minVreg, pregkTail);
        Reg::Add(sumVreg, sumVreg, tmpVreg, pregFull);

        Reg::ReduceSumWithDataBlock(sumVreg, sumVreg, pregFull);
        Reg::StoreAlign<float, Reg::StoreDist::DIST_NORM>(workUb + i * DEFAULT_BLK_NUM, sumVreg, pregOneBlk);
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    for (uint16_t i = 0; i < VcgFoldRepeat; ++i) {
        Reg::LoadAlign<float, Reg::LoadDist::DIST_DINTLV_B32>(sumVreg, tmpVreg, workUb + i * HALF_REPEAT_SIZE);
        Reg::Add(sumVreg, sumVreg, tmpVreg, pregFull);
        if constexpr (sizeof(T2) == 4) {
            Reg::StoreAlign(workUb + i * FLOAT_REPEAT_SIZE, sumVreg, pregFull);
        } else {
            Reg::StoreAlign<float, Reg::StoreDist::DIST_INTLV_B32>(
                workUb + i * HALF_REPEAT_SIZE, sumVreg, sumVreg, pregFull);
        }
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    sreg = originM * dtypeBlkStride;
    uint32_t sreg1 = originM * expMaxBlkStride;
    for (uint16_t i = 0; i < e2bRep; ++i) {
        pregCnt = Reg::UpdateMask<uint32_t>(sreg);
        LoadE2B<float>(sumVreg, workUb + i * DEFAULT_BLK_NUM);
        LoadIfNeedCast<T2>(tmpVreg, inExpSumUb + i * FLOAT_REPEAT_SIZE, pregCnt);
        Reg::LoadAlign(maxVreg, expMaxF32Ub + i * FLOAT_REPEAT_SIZE);
        Reg::Mul(dstVreg, maxVreg, tmpVreg, pregCnt);
        Reg::Add(sumVreg, sumVreg, dstVreg, pregCnt);
        StoreIfNeedCast<T2>(expSumUb + i * FLOAT_REPEAT_SIZE, sumVreg, pregCnt);
        if constexpr (sizeof(T1) == 2 && sizeof(T2) == 4) {
            pregDst = Reg::UpdateMask<uint16_t>(sreg1);
            Reg::Cast<T1, float, Internal::castTraitB32ToB16>(t1Reg, maxVreg, pregCnt);
            Reg::Pack<uint16_t, uint32_t>((Reg::RegTensor<uint16_t>&)t1Reg, (Reg::RegTensor<uint32_t>&)t1Reg);
            Reg::Interleave(t1Reg, dummyReg, t1Reg, t1Reg);
            Reg::StoreAlign<T1>(expMaxUb + i * HALF_REPEAT_SIZE, t1Reg, pregDst);
        } else if constexpr (sizeof(T1) == 2 && sizeof(T2) == 2) {
            StoreIfNeedCast<T1>(expMaxUb + i * FLOAT_REPEAT_SIZE, maxVreg, pregCnt);
        }
    }
}

template <typename T1, typename T2>
__aicore__ inline void SoftmaxFlashV2NZWithTailUpdateImpl(
    const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& expSumTensor, const LocalTensor<T2>& maxTensor,
    const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& expMaxTensor, const LocalTensor<T2>& inExpSumTensor,
    const LocalTensor<T2>& inMaxTensor, const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape,
    const SoftMaxTiling& tiling)
{
    uint16_t srcM = tiling.srcM;
    uint16_t srcK = tiling.srcK;
    uint16_t originK = originalSrcShape.k;

    uint16_t offset;
    if constexpr (sizeof(T2) == 4) {
        offset = CeilDivision(srcM, FLOAT_REPEAT_SIZE) * FLOAT_REPEAT_SIZE;
    } else {
        offset = CeilDivision(srcM * SOFTMAX_COMPUTE_DIM, HALF_REPEAT_SIZE) * HALF_REPEAT_SIZE;
    }
    uint16_t offset1 = srcM * SOFTMAX_SHAPE_NZ_BASIC_COUNT;
    uint64_t mask[2] = {0, 0};
    CreateSpecialFormatMask(
        mask[0], originK % SOFTMAX_SHAPE_NZ_BASIC_COUNT, FLOAT_REPEAT_SIZE / SOFTMAX_SHAPE_NZ_BASIC_COUNT);
    SetVectorMask<uint32_t>(mask[1], mask[0]);

    __ubuf__ T1* dstUb = (__ubuf__ T1*)dstTensor.GetPhyAddr();
    __ubuf__ T2* expSumUb = (__ubuf__ T2*)expSumTensor.GetPhyAddr();
    __ubuf__ T2* inExpSumUb = (__ubuf__ T2*)inExpSumTensor.GetPhyAddr();
    __ubuf__ T2* maxUb = (__ubuf__ T2*)maxTensor.GetPhyAddr();
    __ubuf__ T2* inMaxUb = (__ubuf__ T2*)inMaxTensor.GetPhyAddr();
    __ubuf__ T1* srcUb = (__ubuf__ T1*)srcTensor.GetPhyAddr();
    __ubuf__ T1* expMaxUb = (__ubuf__ T1*)expMaxTensor.GetPhyAddr();
    __ubuf__ float* expMaxF32Ub = (__ubuf__ float*)expMaxTensor.GetPhyAddr();
    __ubuf__ float* workUb = (__ubuf__ float*)workLocal.GetPhyAddr();
    __ubuf__ float* tmpUb = (__ubuf__ float*)workLocal.GetPhyAddr(offset);
    __ubuf__ float* expUb = (__ubuf__ float*)workLocal.GetPhyAddr(offset + offset1);
    if constexpr (sizeof(T1) == 2) {
        expMaxF32Ub = (__ubuf__ float*)workLocal.GetPhyAddr(offset + offset1 + srcM * srcK);
    }
    SoftmaxFlashV2NZWithTailUpdateVFImpl<T1, T2>(
        dstUb, expSumUb, inExpSumUb, maxUb, inMaxUb, srcUb, expMaxUb, expMaxF32Ub, workUb, tmpUb, expUb,
        originalSrcShape, tiling);
}

template <typename T1, typename T2, bool isUpdate = false, bool isBasicBlock = false>
__aicore__ inline void SoftMaxFlashV2NZImpl(
    const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& expSumTensor, const LocalTensor<T2>& maxTensor,
    const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& expMaxTensor, const LocalTensor<T2>& inExpSumTensor,
    const LocalTensor<T2>& inMaxTensor, const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape,
    const SoftMaxTiling& tiling)
{
    static_assert(
        (SupportType<T1, float>() && SupportType<T2, float>()) ||
            (SupportType<T1, half>() && SupportType<T2, half>()) ||
            (SupportType<T1, half>() && SupportType<T2, float>()),
        "SoftMaxFlashV2 api only support half/float on current device");
    if constexpr (!isUpdate) {
        if (originalSrcShape.k != tiling.srcK) {
            SoftmaxFlashV2NZWithTailNoUpdateImpl<T1, T2>(
                dstTensor, expSumTensor, maxTensor, srcTensor, workLocal, originalSrcShape, tiling);
        } else {
            SoftmaxFlashV2NZNoUpdateImpl<T1, T2>(
                dstTensor, expSumTensor, maxTensor, srcTensor, workLocal, originalSrcShape, tiling);
        }
    } else {
        if (originalSrcShape.k != tiling.srcK) {
            SoftmaxFlashV2NZWithTailUpdateImpl<T1, T2>(
                dstTensor, expSumTensor, maxTensor, srcTensor, expMaxTensor, inExpSumTensor, inMaxTensor, workLocal,
                originalSrcShape, tiling);
        } else {
            SoftmaxFlashV2NZUpdateImpl<T1, T2>(
                dstTensor, expSumTensor, maxTensor, srcTensor, expMaxTensor, inExpSumTensor, inMaxTensor, workLocal,
                originalSrcShape, tiling);
        }
    }
}

template <typename T1, typename T2>
__simd_vf__ inline void SoftmaxFlashV2NDUpdateVFImpl(
    __ubuf__ T1* dstUb, __ubuf__ T2* expSumUb, __ubuf__ T2* inExpSumUb, __ubuf__ T2* maxUb, __ubuf__ T2* inMaxUb,
    __ubuf__ T1* srcUb, __ubuf__ T1* expMaxUb, const LastAxisShapeND originalSrcShape, const SoftMaxTiling tiling)
{
    uint16_t srcM = originalSrcShape.m;
    uint16_t srcK = tiling.srcK;
    uint16_t originK = originalSrcShape.k;
    uint16_t repeatTimes = CeilDivision(originK, FLOAT_REPEAT_SIZE);
    uint16_t blockStride = IsSameType<T2, half>::value ? B16_DATA_NUM_PER_BLOCK : B32_DATA_NUM_PER_BLOCK;
    NotNumUnion notNum;
    notNum.i = F32_NEG_INF;

    Reg::MaskReg pregFull = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    Reg::MaskReg pregOneBlk, pregCnt;
    if constexpr (IsSameType<T2, half>::value) {
        pregOneBlk = Reg::CreateMask<uint32_t, Reg::MaskPattern::VL16>();
    } else {
        pregOneBlk = Reg::CreateMask<uint32_t, Reg::MaskPattern::VL8>();
    }
    Reg::RegTensor<float> srcVreg;
    Reg::RegTensor<float> maxVreg;
    Reg::RegTensor<float> expMaxVreg;
    Reg::RegTensor<float> sumVreg;
    Reg::RegTensor<float> tmpVreg;
    Reg::RegTensor<float> dstVreg;

    for (uint16_t i = 0; i < srcM; ++i) {
        Duplicate(maxVreg, notNum.f);
        for (uint16_t j = 0; j < repeatTimes; ++j) {
            LoadIfNeedCast<T1>(srcVreg, srcUb + i * srcK + j * FLOAT_REPEAT_SIZE, pregFull);
            Reg::Max(maxVreg, maxVreg, srcVreg, pregFull);
        }
        Reg::ReduceMax(maxVreg, maxVreg, pregFull);
        Duplicate(maxVreg, maxVreg, pregOneBlk);
        LoadIfNeedCast<T2>(tmpVreg, inMaxUb + i * blockStride, pregOneBlk);
        Reg::Max(maxVreg, maxVreg, tmpVreg, pregOneBlk);
        StoreIfNeedCast<T2>(maxUb + i * blockStride, maxVreg, pregOneBlk);

        SoftmaxExpSub(expMaxVreg, tmpVreg, maxVreg, pregOneBlk);
        if constexpr (sizeof(T1) == 2 && sizeof(T2) == 4) {
            pregCnt = Reg::CreateMask<uint32_t, Reg::MaskPattern::VL16>();
            Reg::Interleave(tmpVreg, sumVreg, expMaxVreg, expMaxVreg);
            StoreIfNeedCast<T1>(expMaxUb + i * blockStride * 2, tmpVreg, pregCnt);
        } else {
            StoreIfNeedCast<T1>(expMaxUb + i * blockStride, expMaxVreg, pregOneBlk);
        }

        Duplicate(sumVreg, 0);
        Duplicate(maxVreg, maxVreg, pregFull);
        for (uint16_t j = 0; j < repeatTimes; ++j) {
            LoadIfNeedCast<T1>(srcVreg, srcUb + i * srcK + j * FLOAT_REPEAT_SIZE, pregFull);
            SoftmaxExpSub(tmpVreg, srcVreg, maxVreg, pregFull);
            StoreIfNeedCast<T1>(dstUb + i * srcK + j * FLOAT_REPEAT_SIZE, tmpVreg, pregFull);
            Reg::Add(sumVreg, sumVreg, tmpVreg, pregFull);
        }
        Reg::ReduceSum(sumVreg, sumVreg, pregFull);
        Duplicate(sumVreg, sumVreg, pregOneBlk);
        LoadIfNeedCast<T2>(tmpVreg, inExpSumUb + i * blockStride, pregOneBlk);
        Reg::Mul(tmpVreg, expMaxVreg, tmpVreg, pregOneBlk);
        Reg::Add(sumVreg, sumVreg, tmpVreg, pregOneBlk);
        StoreIfNeedCast<T2>(expSumUb + i * blockStride, sumVreg, pregOneBlk);
    }
}

template <typename T1, typename T2>
__aicore__ inline void SoftmaxFlashV2NDUpdateImpl(
    const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& expSumTensor, const LocalTensor<T2>& maxTensor,
    const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& expMaxTensor, const LocalTensor<T2>& inExpSumTensor,
    const LocalTensor<T2>& inMaxTensor, const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape,
    const SoftMaxTiling& tiling)
{
    __ubuf__ T1* dstUb = (__ubuf__ T1*)dstTensor.GetPhyAddr();
    __ubuf__ T2* expSumUb = (__ubuf__ T2*)expSumTensor.GetPhyAddr();
    __ubuf__ T2* inExpSumUb = (__ubuf__ T2*)inExpSumTensor.GetPhyAddr();
    __ubuf__ T2* maxUb = (__ubuf__ T2*)maxTensor.GetPhyAddr();
    __ubuf__ T2* inMaxUb = (__ubuf__ T2*)inMaxTensor.GetPhyAddr();
    __ubuf__ T1* srcUb = (__ubuf__ T1*)srcTensor.GetPhyAddr();
    __ubuf__ T1* expMaxUb = (__ubuf__ T1*)expMaxTensor.GetPhyAddr();

    SoftmaxFlashV2NDUpdateVFImpl<T1, T2>(
        dstUb, expSumUb, inExpSumUb, maxUb, inMaxUb, srcUb, expMaxUb, originalSrcShape, tiling);
}

template <typename T1, typename T2>
__simd_vf__ inline void SoftmaxFlashV2NDWithTailUpdateVFImpl(
    __ubuf__ T1* dstUb, __ubuf__ T2* expSumUb, __ubuf__ T2* inExpSumUb, __ubuf__ T2* maxUb, __ubuf__ T2* inMaxUb,
    __ubuf__ T1* srcUb, __ubuf__ T1* expMaxUb, const LastAxisShapeND originalSrcShape, const SoftMaxTiling tiling)
{
    uint16_t srcM = originalSrcShape.m;
    uint16_t srcK = tiling.srcK;
    uint16_t originK = originalSrcShape.k;
    uint16_t repeatTimes = CeilDivision(originK, FLOAT_REPEAT_SIZE);
    uint16_t blockStride = IsSameType<T2, half>::value ? B16_DATA_NUM_PER_BLOCK : B32_DATA_NUM_PER_BLOCK;
    NotNumUnion notNum;
    notNum.i = F32_NEG_INF;

    Reg::MaskReg pregCnt;
    Reg::MaskReg pregFull = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    Reg::MaskReg pregOneBlk;
    if constexpr (IsSameType<T2, half>::value) {
        pregOneBlk = Reg::CreateMask<uint32_t, Reg::MaskPattern::VL16>();
    } else {
        pregOneBlk = Reg::CreateMask<uint32_t, Reg::MaskPattern::VL8>();
    }
    Reg::RegTensor<float> srcVreg;
    Reg::RegTensor<float> maxVreg;
    Reg::RegTensor<float> expMaxVreg;
    Reg::RegTensor<float> sumVreg;
    Reg::RegTensor<float> tmpVreg;
    Reg::RegTensor<float> minVreg;
    Reg::RegTensor<float> dstVreg;

    Duplicate(minVreg, notNum.f);
    for (uint16_t i = 0; i < srcM; ++i) {
        uint32_t sreg = originK;
        Duplicate(maxVreg, notNum.f);
        for (uint16_t j = 0; j < static_cast<uint16_t>(repeatTimes - 1); ++j) {
            pregCnt = Reg::UpdateMask<uint32_t>(sreg);
            LoadIfNeedCast<T1>(srcVreg, srcUb + i * srcK + j * FLOAT_REPEAT_SIZE, pregFull);
            Reg::Max(maxVreg, maxVreg, srcVreg, pregCnt);
        }
        pregCnt = Reg::UpdateMask<uint32_t>(sreg);
        LoadIfNeedCast<T1>(srcVreg, srcUb + i * srcK + (repeatTimes - 1) * FLOAT_REPEAT_SIZE, pregFull);
        Reg::Select(srcVreg, srcVreg, minVreg, pregCnt);
        Reg::Max(maxVreg, maxVreg, srcVreg, pregFull);

        Reg::ReduceMax(maxVreg, maxVreg, pregFull);
        Duplicate(maxVreg, maxVreg, pregOneBlk);
        LoadIfNeedCast<T2>(tmpVreg, inMaxUb + i * blockStride, pregOneBlk);
        Reg::Max(maxVreg, maxVreg, tmpVreg, pregOneBlk);
        StoreIfNeedCast<T2>(maxUb + i * blockStride, maxVreg, pregOneBlk);

        SoftmaxExpSub(expMaxVreg, tmpVreg, maxVreg, pregOneBlk);
        if constexpr (sizeof(T1) == 2 && sizeof(T2) == 4) {
            pregCnt = Reg::CreateMask<uint32_t, Reg::MaskPattern::VL16>();
            Reg::Interleave(tmpVreg, sumVreg, expMaxVreg, expMaxVreg);
            StoreIfNeedCast<T1>(expMaxUb + i * blockStride * 2, tmpVreg, pregCnt);
        } else {
            StoreIfNeedCast<T1>(expMaxUb + i * blockStride, expMaxVreg, pregOneBlk);
        }

        Duplicate(sumVreg, 0);
        Duplicate(maxVreg, maxVreg, pregFull);
        sreg = originK;
        for (uint16_t j = 0; j < repeatTimes; ++j) {
            pregCnt = Reg::UpdateMask<uint32_t>(sreg);
            LoadIfNeedCast<T1>(srcVreg, srcUb + i * srcK + j * FLOAT_REPEAT_SIZE, pregFull);
            SoftmaxExpSub(tmpVreg, srcVreg, maxVreg, pregCnt);
            StoreIfNeedCast<T1>(dstUb + i * srcK + j * FLOAT_REPEAT_SIZE, tmpVreg, pregCnt);
            Reg::Add(sumVreg, sumVreg, tmpVreg, pregFull);
        }
        Reg::ReduceSum(sumVreg, sumVreg, pregFull);
        Duplicate(sumVreg, sumVreg, pregOneBlk);
        LoadIfNeedCast<T2>(tmpVreg, inExpSumUb + i * blockStride, pregOneBlk);
        Reg::Mul(tmpVreg, expMaxVreg, tmpVreg, pregOneBlk);
        Reg::Add(sumVreg, sumVreg, tmpVreg, pregOneBlk);
        StoreIfNeedCast<T2>(expSumUb + i * blockStride, sumVreg, pregOneBlk);
    }
}

template <typename T1, typename T2>
__aicore__ inline void SoftmaxFlashV2NDWithTailUpdateImpl(
    const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& expSumTensor, const LocalTensor<T2>& maxTensor,
    const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& expMaxTensor, const LocalTensor<T2>& inExpSumTensor,
    const LocalTensor<T2>& inMaxTensor, const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape,
    const SoftMaxTiling& tiling)
{
    __ubuf__ T1* dstUb = (__ubuf__ T1*)dstTensor.GetPhyAddr();
    __ubuf__ T2* expSumUb = (__ubuf__ T2*)expSumTensor.GetPhyAddr();
    __ubuf__ T2* inExpSumUb = (__ubuf__ T2*)inExpSumTensor.GetPhyAddr();
    __ubuf__ T2* maxUb = (__ubuf__ T2*)maxTensor.GetPhyAddr();
    __ubuf__ T2* inMaxUb = (__ubuf__ T2*)inMaxTensor.GetPhyAddr();
    __ubuf__ T1* srcUb = (__ubuf__ T1*)srcTensor.GetPhyAddr();
    __ubuf__ T1* expMaxUb = (__ubuf__ T1*)expMaxTensor.GetPhyAddr();

    SoftmaxFlashV2NDWithTailUpdateVFImpl<T1, T2>(
        dstUb, expSumUb, inExpSumUb, maxUb, inMaxUb, srcUb, expMaxUb, originalSrcShape, tiling);
}

template <typename T1, typename T2, bool isBasicBlock = false, bool outputBrc = true>
__aicore__ inline void SoftmaxFlashV2NDNoUpdateImpl(
    const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& expSumTensor, const LocalTensor<T2>& maxTensor,
    const LocalTensor<T1>& srcTensor, const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape,
    const SoftMaxTiling& tiling)
{
    if constexpr (isBasicBlock) {
        SoftMaxGenericNDImpl<T1, T2, true, false, outputBrc>(
            dstTensor, expSumTensor, maxTensor, srcTensor, workLocal, originalSrcShape, tiling);
    } else {
        if (tiling.srcK == B32_DATA_NUM_PER_BLOCK && IsSameType<T1, float>::value) {
            SingleSoftMaxGenericNDForBlkImpl<T1, T2, true, false, outputBrc>(
                dstTensor, expSumTensor, maxTensor, srcTensor, workLocal, originalSrcShape, tiling);
        } else if (tiling.srcK == B32_DATA_NUM_PER_BLOCK * 2) {
            SingleSoftMaxGenericNDAlignedWithBlkImpl<T1, T2, true, false, outputBrc>(
                dstTensor, expSumTensor, maxTensor, srcTensor, workLocal, originalSrcShape, tiling);
        } else if (originalSrcShape.k <= FLOAT_REPEAT_SIZE) {
            SingleSoftMaxGenericNDImpl<T1, T2, true, false, outputBrc>(
                dstTensor, expSumTensor, maxTensor, srcTensor, workLocal, originalSrcShape, tiling);
        } else if (originalSrcShape.k % FLOAT_REPEAT_SIZE != 0) {
            SoftMaxGenericNDWithTailImpl<T1, T2, true, false, outputBrc>(
                dstTensor, expSumTensor, maxTensor, srcTensor, workLocal, originalSrcShape, tiling);
        } else {
            SoftMaxGenericNDImpl<T1, T2, true, false, outputBrc>(
                dstTensor, expSumTensor, maxTensor, srcTensor, workLocal, originalSrcShape, tiling);
        }
    }
}

template <typename T1, typename T2, bool isUpdate = false, bool isBasicBlock = false, bool isOutputReduceMax = false>
__aicore__ inline void SoftmaxFlashV2M1PostProcess(
    const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& outReduceMax, const LocalTensor<T2>& expSumTensor,
    const LocalTensor<T2>& maxTensor, const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& expMaxTensor,
    const LocalTensor<T2>& inExpSumTensor, const LocalTensor<T2>& inMaxTensor, const LocalTensor<float>& workLocal,
    const LastAxisShapeND& originalSrcShape, const SoftMaxTiling& tiling)
{
    static_assert(
        (SupportType<T1, float>() && SupportType<T2, float>()) ||
            (SupportType<T1, half>() && SupportType<T2, half>()) ||
            (SupportType<T1, half>() && SupportType<T2, float>()),
        "SoftMaxFlashV2 api only support half/float on current device");
    if constexpr (isBasicBlock && isUpdate) {
        SoftmaxFlashV2M1NDUpdateImpl<T1, T2, isOutputReduceMax>(
            dstTensor, outReduceMax, expSumTensor, maxTensor, srcTensor, expMaxTensor, inExpSumTensor, inMaxTensor,
            workLocal, originalSrcShape, tiling);
    } else {
        if constexpr (!isUpdate) {
            SoftmaxFlashV2NDNoUpdateImpl<T1, T2, isBasicBlock, false>(
                dstTensor, expSumTensor, maxTensor, srcTensor, workLocal, originalSrcShape, tiling);
        } else {
            if (originalSrcShape.k % FLOAT_REPEAT_SIZE) {
                SoftmaxFlashV2M1NDWithTailUpdateImpl<T1, T2, isOutputReduceMax>(
                    dstTensor, outReduceMax, expSumTensor, maxTensor, srcTensor, expMaxTensor, inExpSumTensor,
                    inMaxTensor, workLocal, originalSrcShape, tiling);
            } else {
                SoftmaxFlashV2M1NDUpdateImpl<T1, T2, isOutputReduceMax>(
                    dstTensor, outReduceMax, expSumTensor, maxTensor, srcTensor, expMaxTensor, inExpSumTensor,
                    inMaxTensor, workLocal, originalSrcShape, tiling);
            }
        }
    }
}

template <typename T1, typename T2, bool isUpdate = false, bool isBasicBlock = false, const SoftmaxConfig& config>
__aicore__ inline void SoftmaxFlashV2PostProcess(
    const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& expSumTensor, const LocalTensor<T2>& maxTensor,
    const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& expMaxTensor, const LocalTensor<T2>& inExpSumTensor,
    const LocalTensor<T2>& inMaxTensor, const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape,
    const SoftMaxTiling& tiling)
{
    static_assert(
        (SupportType<T1, float>() && SupportType<T2, float>()) ||
            (SupportType<T1, half>() && SupportType<T2, half>()) ||
            (SupportType<T1, half>() && SupportType<T2, float>()),
        "SoftMaxFlashV2 api only support half/float on current device");
    if constexpr (isBasicBlock && isUpdate) {
        SoftmaxFlashV2NDUpdateImpl<T1, T2>(
            dstTensor, expSumTensor, maxTensor, srcTensor, expMaxTensor, inExpSumTensor, inMaxTensor, workLocal,
            originalSrcShape, tiling);
    } else {
        if constexpr (!isUpdate) {
            SoftmaxFlashV2NDNoUpdateImpl<T1, T2, isBasicBlock>(
                dstTensor, expSumTensor, maxTensor, srcTensor, workLocal, originalSrcShape, tiling);
        } else {
            if (originalSrcShape.k % FLOAT_REPEAT_SIZE) {
                SoftmaxFlashV2NDWithTailUpdateImpl<T1, T2>(
                    dstTensor, expSumTensor, maxTensor, srcTensor, expMaxTensor, inExpSumTensor, inMaxTensor, workLocal,
                    originalSrcShape, tiling);
            } else {
                SoftmaxFlashV2NDUpdateImpl<T1, T2>(
                    dstTensor, expSumTensor, maxTensor, srcTensor, expMaxTensor, inExpSumTensor, inMaxTensor, workLocal,
                    originalSrcShape, tiling);
            }
        }
    }
}
} // namespace AscendC
#endif // IMPL_ACTIVATION_SOFTMAX_C310_SOFTMAX_FLASHV2_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASHV2_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASHV2_IMPL_H__
#endif
