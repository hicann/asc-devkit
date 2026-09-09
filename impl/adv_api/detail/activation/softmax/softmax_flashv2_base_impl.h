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
 * \file softmax_flashv2_base_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/activation/softmax/softmax_flashv2_base_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/softmaxflashv2.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASHV2_BASE_IMPL_H__
#endif

#ifndef IMPL_ACTIVATION_SOFTMAX_SOFTMAX_FLASHV2_BASE_IMPL_H
#define IMPL_ACTIVATION_SOFTMAX_SOFTMAX_FLASHV2_BASE_IMPL_H

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "regbase/3510/softmax_flashv2_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 3002
#include "regbase/v300/softmax_flashv2_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
#include "membase/v220/softmax_flashv2_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 2002
#include "membase/v200/softmax_flashv2_impl.h"
#endif
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/activation/softmax/softmax_flashv2/softmax_flashv2_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
__aicore__ inline constexpr SoftMaxTiling SoftMaxFlashV2TilingFuncImpl(
    const uint32_t srcM, const uint32_t srcK, const uint32_t dataTypeSize1, const uint32_t dataTypeSize2,
    const uint32_t localWorkSpaceSize, const bool isUpdate = false, const bool isBasicBlock = false,
    const bool isDataFormatNZ = false, const bool isFlashOutputBrc = false)
{
    SoftMaxTiling softmaxTiling;
    const uint32_t elementNumPerBlk = ONE_BLK_SIZE / dataTypeSize2;
    softmaxTiling.srcM = srcM;
    softmaxTiling.srcK = srcK;
    softmaxTiling.srcSize = srcM * srcK;

    softmaxTiling.outMaxM = srcM;
    softmaxTiling.outMaxK = elementNumPerBlk;
    softmaxTiling.outMaxSize = srcM * elementNumPerBlk;

    if (isDataFormatNZ) {
        softmaxTiling.reduceM =
            localWorkSpaceSize / (SOFTMAX_SHAPE_NZ_BASIC_COUNT * SOFTMAX_NZ_TILING_NEEDBLOCK + srcK);
    } else {
        if (isBasicBlock && srcK % FLOAT_REPEAT_SIZE == 0 && srcM % SOFTMAX_BASIC_TILE_NUM == 0) {
            softmaxTiling.reduceM =
                CalculateNDSplitM(localWorkSpaceSize, dataTypeSize1, elementNumPerBlk, {srcM, srcK}, isBasicBlock);
        } else {
            softmaxTiling.reduceM =
                (dataTypeSize1 == B16_BYTE_SIZE) ?
                    localWorkSpaceSize / (SOFTMAX_COMPUTE_DIM * elementNumPerBlk + srcK + FLOAT_REPEAT_SIZE) :
                    localWorkSpaceSize / (elementNumPerBlk + FLOAT_REPEAT_SIZE);
        }
    }

    uint32_t softmaxBasicTileNum = SOFTMAX_BASIC_TILE_NUM;
    if (isFlashOutputBrc && dataTypeSize1 == B16_BYTE_SIZE) {
        softmaxBasicTileNum = HALF_NUM_PER_BLK;
    }

    if (softmaxTiling.reduceM < srcM && softmaxTiling.reduceM > softmaxBasicTileNum) {
        softmaxTiling.reduceM = softmaxTiling.reduceM / softmaxBasicTileNum * softmaxBasicTileNum;
    }
    softmaxTiling.reduceM = softmaxTiling.reduceM < srcM ? softmaxTiling.reduceM : srcM;

    softmaxTiling.reduceK = elementNumPerBlk;
    softmaxTiling.reduceSize = softmaxTiling.reduceM * elementNumPerBlk;

    softmaxTiling.splitM = softmaxTiling.reduceM;
    softmaxTiling.splitK = srcK;
    softmaxTiling.splitSize = softmaxTiling.reduceM * srcK;

    softmaxTiling.rangeM = srcM / softmaxTiling.reduceM;
    softmaxTiling.tailM = srcM % softmaxTiling.reduceM;

    softmaxTiling.tailSplitSize = softmaxTiling.tailM * srcK;
    softmaxTiling.tailReduceSize = softmaxTiling.tailM * elementNumPerBlk;

    if (isFlashOutputBrc && (softmaxTiling.rangeM > MIN_BLOCK_LEN || softmaxTiling.tailM != 0)) {
        ASCENDC_ASSERT((softmaxTiling.reduceM % (ONE_BLK_SIZE / dataTypeSize1) == 0), {
            printf(
                "[ERROR] "
                "When dataTypeSize1(%d) is float(or half), softmaxTiling.reduceM(%d) must be a multiple of 8(or 16), "
                "Adjust the input parameter -> localWorkSpaceSize.\n",
                dataTypeSize1, softmaxTiling.reduceM);
        });
    }
    return softmaxTiling;
}

template <typename T1, typename T2, bool isUpdate, bool isBasicBlock, bool isDataFormatNZ, const SoftmaxConfig& config>
__aicore__ inline SoftMaxTiling SoftmaxFlashV2UpdateTilingImpl(
    const LocalTensor<T1>& srcTensor, const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling,
    const SoftMaxShapeInfo& softmaxShapeInfo)
{
    if constexpr (isDataFormatNZ) {
        LastAxisShapeND srcNDinfo = {softmaxShapeInfo.srcM, softmaxShapeInfo.srcK};
        if (softmaxShapeInfo.srcM == 0 || softmaxShapeInfo.srcK == 0) {
            ShapeInfo srcShape = srcTensor.GetShapeInfo();
            srcNDinfo = GetLastAxisShapeND(srcShape);
        }
        uint32_t workLocalSize = workLocal.GetSize();
        return SoftMaxFlashV2TilingFuncImpl(
            srcNDinfo.m, srcNDinfo.k, sizeof(T1), sizeof(T2), workLocalSize, isUpdate, false, true);
    } else {
        if constexpr (!config.isCheckTiling) {
            return tiling;
        }

        LastAxisShapeND srcNDinfo = {softmaxShapeInfo.srcM, softmaxShapeInfo.srcK};
        if (softmaxShapeInfo.srcM == 0 || softmaxShapeInfo.srcK == 0) {
            ShapeInfo srcShape = srcTensor.GetShapeInfo();
            srcNDinfo = GetLastAxisShapeND(srcShape);
        }

        if (srcNDinfo.m == tiling.srcM && srcNDinfo.k == tiling.srcK) {
            return tiling;
        }

        SoftMaxTiling softmaxTiling;
        uint32_t workLocalSize = workLocal.GetSize();

        if constexpr (config.mode == SoftmaxMode::SOFTMAX_OUTPUT_WITHOUT_BRC) {
            softmaxTiling = SoftMaxFlashV2TilingFuncImpl(
                srcNDinfo.m, srcNDinfo.k, sizeof(T1), sizeof(T2), workLocalSize, isUpdate, isBasicBlock, false, true);
        } else {
            softmaxTiling = SoftMaxFlashV2TilingFuncImpl(
                srcNDinfo.m, srcNDinfo.k, sizeof(T1), sizeof(T2), workLocalSize, isUpdate, isBasicBlock);
        }
        return softmaxTiling;
    }
}

template <
    typename T1, typename T2, bool isUpdate, bool isReuseSource, bool isBasicBlock, bool isDataFormatNZ,
    const SoftmaxConfig& config>
__aicore__ inline void SoftmaxFlashV2Impl(
    const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& sumTensor, const LocalTensor<T2>& maxTensor,
    const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& expMaxTensor, const LocalTensor<T2>& inSumTensor,
    const LocalTensor<T2>& inMaxTensor, const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling,
    const SoftMaxShapeInfo& softmaxShapeInfo)
{
    CHECK_FUNC_HIGHLEVEL_API(
        SoftmaxFlashV2, (T1, T2, isUpdate, isReuseSource, isBasicBlock, isDataFormatNZ, config),
        (dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor, workLocal, tiling,
         softmaxShapeInfo));

    SoftMaxTiling newTiling = SoftmaxFlashV2UpdateTilingImpl<T1, T2, isUpdate, isBasicBlock, isDataFormatNZ, config>(
        srcTensor, workLocal, tiling, softmaxShapeInfo);

    LastAxisShapeND originalSrcShape = {softmaxShapeInfo.oriSrcM, softmaxShapeInfo.oriSrcK};
    if (softmaxShapeInfo.srcM == 0 || softmaxShapeInfo.srcK == 0) {
        ShapeInfo srcShape = srcTensor.GetShapeInfo();
        originalSrcShape = GetLastAxisOriginShapeND(srcShape);
    }

    if constexpr (isDataFormatNZ) {
        SoftMaxFlashV2NZImpl<T1, T2, isUpdate, isBasicBlock>(
            dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor, workLocal,
            originalSrcShape, newTiling);
    } else if constexpr (config.mode == SoftmaxMode::SOFTMAX_OUTPUT_WITHOUT_BRC) {
        SoftmaxFlashV2M1PostProcess<T1, T2, isUpdate, isBasicBlock, false>(
            dstTensor, maxTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor, workLocal,
            originalSrcShape, newTiling);
    } else {
        SoftmaxFlashV2PostProcess<T1, T2, isUpdate, isBasicBlock, config>(
            dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor, workLocal,
            originalSrcShape, newTiling);
    }
}

template <
    typename T1, typename T2, bool isUpdate, bool isReuseSource, bool isBasicBlock, bool isDataFormatNZ,
    const SoftmaxConfig& config>
__aicore__ inline void SoftmaxFlashV2Impl(
    const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& sumTensor, const LocalTensor<T2>& maxTensor,
    const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& expMaxTensor, const LocalTensor<T2>& inSumTensor,
    const LocalTensor<T2>& inMaxTensor, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo)
{
    LocalTensor<float> workLocal;
    PopStackBuffer<float, TPosition::LCM>(workLocal);
    SoftmaxFlashV2Impl<T1, T2, isUpdate, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
        dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor, workLocal, tiling,
        softmaxShapeInfo);
}

template <
    typename T1, typename T2, bool isUpdate, bool isReuseSource, bool isBasicBlock, bool isDataFormatNZ,
    const SoftmaxConfig& config>
__aicore__ inline void SoftmaxFlashV2Impl(
    const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& sumTensor, const LocalTensor<T2>& maxTensor,
    const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& expMaxTensor, const LocalTensor<T2>& inSumTensor,
    const LocalTensor<T2>& inMaxTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling,
    const SoftMaxShapeInfo& softmaxShapeInfo)
{
    auto workLocal = sharedTmpBuffer.ReinterpretCast<float>();
    if constexpr (isDataFormatNZ) { // check nz format tmpbuffer size
        LastAxisShapeND srcNDinfo = {softmaxShapeInfo.srcM, softmaxShapeInfo.srcK};
        if (softmaxShapeInfo.srcM == 0 || softmaxShapeInfo.srcK == 0) {
            ShapeInfo srcShape = srcTensor.GetShapeInfo();
            srcNDinfo = GetLastAxisShapeND(srcShape);
        }
        uint32_t workLocalSize = workLocal.GetSize();
        if (workLocalSize < SOFTMAX_SHAPE_NZ_BASIC_COUNT * SOFTMAX_NZ_TILING_NEEDBLOCK + srcNDinfo.k) {
            PopStackBuffer<float, TPosition::LCM>(workLocal);
        }
    }

    SoftmaxFlashV2Impl<T1, T2, isUpdate, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
        dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor, workLocal, tiling,
        softmaxShapeInfo);
}

// outReduceMaxTensor
template <
    typename T1, typename T2, bool isUpdate, bool isReuseSource, bool isBasicBlock, bool isDataFormatNZ,
    const SoftmaxConfig& config>
__aicore__ inline void SoftmaxFlashV2MaxImpl(
    const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& outReduceMax, const LocalTensor<T2>& outSum,
    const LocalTensor<T2>& outMax, const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& outexpMax,
    const LocalTensor<T2>& inSum, const LocalTensor<T2>& inMax, const LocalTensor<float>& workLocal,
    const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo)
{
    CHECK_FUNC_HIGHLEVEL_API(
        SoftmaxFlashV2, (T1, T2, isUpdate, isReuseSource, isBasicBlock, isDataFormatNZ, config),
        (dstTensor, outSum, outMax, srcTensor, outexpMax, inSum, inMax, workLocal, tiling, softmaxShapeInfo));

    LastAxisShapeND originalSrcShape = {softmaxShapeInfo.oriSrcM, softmaxShapeInfo.oriSrcK};
    if constexpr (config.mode == SoftmaxMode::SOFTMAX_OUTPUT_WITHOUT_BRC) {
        SoftmaxFlashV2M1PostProcess<T1, T2, isUpdate, isBasicBlock, true>(
            dstTensor, outReduceMax, outSum, outMax, srcTensor, outexpMax, inSum, inMax, workLocal, originalSrcShape,
            tiling);
    }
}

template <
    typename T1, typename T2, bool isUpdate, bool isReuseSource, bool isBasicBlock, bool isDataFormatNZ,
    const SoftmaxConfig& config>
__aicore__ inline void SoftmaxFlashV2MaxImpl(
    const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& outReduceMax, const LocalTensor<T2>& outSum,
    const LocalTensor<T2>& outMax, const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& outexpMax,
    const LocalTensor<T2>& inSum, const LocalTensor<T2>& inMax, const SoftMaxTiling& tiling,
    const SoftMaxShapeInfo& softmaxShapeInfo)
{
    LocalTensor<float> workLocal;
    PopStackBuffer<float, TPosition::LCM>(workLocal);
    SoftmaxFlashV2MaxImpl<T1, T2, isUpdate, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
        dstTensor, outReduceMax, outSum, outMax, srcTensor, outexpMax, inSum, inMax, workLocal, tiling,
        softmaxShapeInfo);
}

template <
    typename T1, typename T2, bool isUpdate, bool isReuseSource, bool isBasicBlock, bool isDataFormatNZ,
    const SoftmaxConfig& config>
__aicore__ inline void SoftmaxFlashV2MaxImpl(
    const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& outReduceMax, const LocalTensor<T2>& outSum,
    const LocalTensor<T2>& outMax, const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& outexpMax,
    const LocalTensor<T2>& inSum, const LocalTensor<T2>& inMax, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo)
{
    auto workLocal = sharedTmpBuffer.ReinterpretCast<float>();
    SoftmaxFlashV2MaxImpl<T1, T2, isUpdate, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
        dstTensor, outReduceMax, outSum, outMax, srcTensor, outexpMax, inSum, inMax, workLocal, tiling,
        softmaxShapeInfo);
}

} // namespace AscendC
#endif // IMPL_ACTIVATION_SOFTMAX_SOFTMAX_FLASHV2_BASE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASHV2_BASE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASHV2_BASE_IMPL_H__
#endif
