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
 * \file batch_scheduler_base.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/scheduler/batch/batch_scheduler_base.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BATCH_BATCH_SCHEDULER_BASE_H__
#endif

#ifndef IMPL_MATMUL_SCHEDULER_BATCH_BATCH_SCHEDULER_BASE_H
#define IMPL_MATMUL_SCHEDULER_BATCH_BATCH_SCHEDULER_BASE_H

#include "../../utils/matmul_module.h"
#include "../../utils/matmul_utils.h"

namespace AscendC {
namespace Impl {
namespace Detail {
/*
    BatchSchedulerBase is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    BatchSchedulerBase is only for internal usage, does not support extension or customized specialization!
*/
template <typename IMPL, class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG>
class BatchSchedulerBase {
    using TransAT = typename A_TYPE::T;
    using TransBT = typename decltype(GetTransBDataType<A_TYPE, B_TYPE, MM_CFG>())::T;
    using DstT = typename C_TYPE::T;
    using L0cT = typename GetMmDstType<TransAT>::Type;

    MATMUL_USE_MODULE(MLoop);
    MATMUL_USE_MODULE(NLoop);
    MATMUL_USE_MODULE(KLoop);
    MATMUL_USE_MODULE(BatchLoop);
    MATMUL_USE_MODULE(MatmulShapeTiling);
    MATMUL_USE_MODULE(MatmulShapeInfo);
    MATMUL_USE_MODULE(MatmulUnitFlag);
    MATMUL_USE_MODULE(CubeOutBuffer);
    MATMUL_USE_MODULE(CopyCubeOut);
    MATMUL_USE_MODULE(TBufPoolL0);
    MATMUL_USE_MODULE(MmadCompute);
    MATMUL_USE_MODULE(BatchCopyCubeInA);
    MATMUL_USE_MODULE(BatchCopyCubeInB);
    MATMUL_USE_MODULE(LoadToA2);
    MATMUL_USE_MODULE(LoadToB2);
    MATMUL_USE_MODULE(BiasScheduler);
    MATMUL_USE_MODULE(MatmulSubBlockInfo);
    MATMUL_USE_MODULE(MatmulQuantProcessor);

public:
    __aicore__ inline BatchSchedulerBase() = default;
    __aicore__ inline ~BatchSchedulerBase() = default;

    __aicore__ inline void Init(const TCubeTiling* __restrict cubeTiling, TPipe* tpipe)
    {
        MATMUL_MODULE(MatmulShapeTiling)->SetTiling(cubeTiling);
        MATMUL_MODULE(MatmulShapeTiling)->template CheckTiling<TransAT, L0cT>();
        auto& var = MATMUL_PARAM_VAR;
        var.tpipe_ = tpipe;

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 1001 || __NPU_ARCH__ == 2002)
        MATMUL_MODULE(MatmulSubBlockInfo)->SetSubBlockIdx(0);
#endif

        auto shapeInfo = MATMUL_MODULE(MatmulShapeInfo);
        shapeInfo->InitParams();

        MATMUL_MODULE(MLoop)->Init(shapeInfo->GetSingleCoreM());
        MATMUL_MODULE(NLoop)->Init(shapeInfo->GetSingleCoreN());
        MATMUL_MODULE(KLoop)->Init(shapeInfo->GetSingleCoreK());
        MATMUL_MODULE(BatchLoop)->Init();
        MATMUL_MODULE(TBufPoolL0)->Init();

        const auto& tiling = MATMUL_MODULE(MatmulShapeTiling)->GetTiling();
        uint32_t shareUbSize = static_cast<uint32_t>(tiling.GetShareUbSize());
        // shareL1Size, shareL0CSize, shareUbSize
        uint32_t shareLens[SHARE_LEN_SIZE] = {
            static_cast<uint32_t>(tiling.GetShareL1Size()), static_cast<uint32_t>(tiling.GetShareL0CSize()),
            shareUbSize};
        InitShareBufStart(
            var.tpipe_, tiling.GetShareMode(), shareLens, SHARE_LEN_SIZE,
            MATMUL_MODULE(MatmulSubBlockInfo)->GetSubBlockIdx());

        MATMUL_MODULE(BatchCopyCubeInA)->Init();
        MATMUL_MODULE(BatchCopyCubeInB)->Init();
        uint32_t lenFactor = 1;
        if constexpr (
            !MatmulFeatureTrait<MM_CFG>::IsNeedUB() &&
            ToMatmulConfig(MM_CFG).scheduleType == ScheduleType::OUTER_PRODUCT) {
            lenFactor = DOUBLE_SIZE;
        }

        if constexpr (ToMatmulConfig(MM_CFG).bmmOutMode != BatchOutMode::SINGLE_BATCH) {
            MATMUL_MODULE(CubeOutBuffer)->Init(tiling.GetBaseM() * tiling.GetBaseN() * nBatchOutNum_, lenFactor);
        } else {
            MATMUL_MODULE(CubeOutBuffer)->Init(tiling.GetBaseM() * tiling.GetBaseN(), lenFactor);
        }
        if constexpr (ToMatmulConfig(MM_CFG).batchMode == BatchMode::BATCH_LARGE_THAN_L1) {
            MATMUL_MODULE(BiasScheduler)->Init(MATMUL_MODULE(BatchLoop)->GetBatchNum());
        } else if constexpr (ToMatmulConfig(MM_CFG).batchMode == BatchMode::BATCH_LESS_THAN_L1) {
            MATMUL_MODULE(BiasScheduler)->Init(tiling.GetBatchNum());
        }

        InitShareBufEnd(var.tpipe_);
    }

    __aicore__ inline BatchSchedulerContext PrepareContext()
    {
        BatchSchedulerContext ctx;
        InitL0Params(ctx);    // init loop unrelated params
        CalcReduceGInfo(ctx); // will be used to calculate iBatch & offset in GetTensorCForBatch
        return ctx;
    }

    // PrepareOffset related
    __aicore__ inline void CalcBatchIterateAOffsetInfo(BatchOffsetInfo& batchOffsetInfo)
    {
        const auto tiling = MATMUL_MODULE(MatmulShapeTiling)->GetTiling();
        if constexpr (A_TYPE::layout == LayoutMode::NORMAL) {
            batchOffsetInfo.modA = 1;
            batchOffsetInfo.divisorA = MATMUL_MODULE(BatchLoop)->GetBatchNum() / MATMUL_MODULE(BatchLoop)->GetBatchA();
        } else {
            if (tiling.GetALayoutInfoG() == 1 && tiling.GetBLayoutInfoG() != 1) { // BRC for Gaxis
                ASSERT(
                    (tiling.GetBLayoutInfoG() > 0) && (tiling.GetALayoutInfoN() == tiling.GetBLayoutInfoN()) &&
                    (tiling.GetALayoutInfoB() == tiling.GetBLayoutInfoB()));
                batchOffsetInfo.modA = 1;
                batchOffsetInfo.divisorA = tiling.GetBLayoutInfoG();
            } else if (tiling.GetALayoutInfoN() == 1 && tiling.GetBLayoutInfoN() != 1) {
                // BRC for N axis = idx % BLayoutInfoG + idx / (BLayoutInfoG * BLayoutInfoN)
                ASSERT(
                    (tiling.GetBLayoutInfoN() > 0) && (tiling.GetALayoutInfoB() == tiling.GetBLayoutInfoB()) &&
                    (tiling.GetALayoutInfoG() == tiling.GetBLayoutInfoG()));
                batchOffsetInfo.modA = tiling.GetBLayoutInfoG();
                batchOffsetInfo.divisorA = tiling.GetBLayoutInfoG() * tiling.GetBLayoutInfoN();
            } else if (tiling.GetALayoutInfoB() == 1 && tiling.GetBLayoutInfoB() != 1) { // BRC for B axis
                ASSERT(
                    (tiling.GetBLayoutInfoB() > 0) &&
                    (tiling.GetALayoutInfoG() == tiling.GetBLayoutInfoG())); // multi axis BRC is not supported.
                batchOffsetInfo.modA = tiling.GetBLayoutInfoG() * tiling.GetBLayoutInfoN();
                batchOffsetInfo.divisorA =
                    tiling.GetBLayoutInfoG() * tiling.GetBLayoutInfoN() * tiling.GetBLayoutInfoB();
            } else {
                batchOffsetInfo.modA = 1;
                batchOffsetInfo.divisorA = 1;
            }
        }

        const auto matmulShapeInfo = MATMUL_MODULE(MatmulShapeInfo);
        if (MATMUL_MODULE(MatmulShapeInfo)->IsTransposeA()) {
            int32_t alignMa = CeilAlign(matmulShapeInfo->template GetSingleCoreM<false, IsBasic(MM_CFG)>(), c0Size_);
            constexpr int32_t alignSize = IsSameTypeV<TransAT, int8_t> ? c0Size_ : BLOCK_CUBE;
            int32_t alignKa = CeilAlign(matmulShapeInfo->template GetSingleCoreK<false, IsBasic(MM_CFG)>(), alignSize);
            batchOffsetInfo.alignA = alignMa * alignKa;
        } else {
            int32_t alignMa = CeilAlign(matmulShapeInfo->template GetSingleCoreM<false, IsBasic(MM_CFG)>(), BLOCK_CUBE);
            int32_t alignKa = CeilAlign(matmulShapeInfo->template GetSingleCoreK<false, IsBasic(MM_CFG)>(), c0Size_);
            batchOffsetInfo.alignA = alignMa * alignKa;
        }
    }

    __aicore__ inline void CalcBatchIterateBOffsetInfo(BatchOffsetInfo& batchOffsetInfo)
    {
        const auto tiling = MATMUL_MODULE(MatmulShapeTiling)->GetTiling();
        if constexpr (B_TYPE::layout == LayoutMode::NORMAL) {
            batchOffsetInfo.modB = 1;
            batchOffsetInfo.divisorB = MATMUL_MODULE(BatchLoop)->GetBatchNum() / MATMUL_MODULE(BatchLoop)->GetBatchB();
        } else {
            if (tiling.GetBLayoutInfoG() == 1 && tiling.GetALayoutInfoG() != 1) { // BRC for Gaxis
                ASSERT(
                    (tiling.GetALayoutInfoG() > 0) && (tiling.GetALayoutInfoN() == tiling.GetBLayoutInfoN()) &&
                    (tiling.GetALayoutInfoB() == tiling.GetBLayoutInfoB()));
                batchOffsetInfo.modB = 1;
                batchOffsetInfo.divisorB = tiling.GetALayoutInfoG();
            } else if (tiling.GetBLayoutInfoN() == 1 && tiling.GetALayoutInfoN() != 1) {
                // BRC for GN axis = idx % BLayoutInfoG + idx / (BLayoutInfoG * BLayoutInfoN)
                ASSERT(
                    (tiling.GetALayoutInfoN() > 0) && (tiling.GetALayoutInfoB() == tiling.GetBLayoutInfoB()) &&
                    (tiling.GetALayoutInfoG() == tiling.GetBLayoutInfoG()));
                batchOffsetInfo.modB = tiling.GetALayoutInfoG();
                batchOffsetInfo.divisorB = tiling.GetALayoutInfoG() * tiling.GetALayoutInfoN();
            } else if (tiling.GetBLayoutInfoB() == 1 && tiling.GetALayoutInfoB() != 1) { // BRC for B axis
                ASSERT(
                    (tiling.GetALayoutInfoB() > 0) && (tiling.GetALayoutInfoN() == tiling.GetBLayoutInfoN()) &&
                    (tiling.GetALayoutInfoG() == tiling.GetBLayoutInfoG())); // multi axis BRC is not supported.
                batchOffsetInfo.modB = tiling.GetALayoutInfoG() * tiling.GetALayoutInfoN();
                batchOffsetInfo.divisorB =
                    tiling.GetALayoutInfoG() * tiling.GetALayoutInfoN() * tiling.GetALayoutInfoB();
            } else {
                batchOffsetInfo.modB = 1;
                batchOffsetInfo.divisorB = 1;
            }
        }

        const auto matmulShapeInfo = MATMUL_MODULE(MatmulShapeInfo);
        if (MATMUL_MODULE(MatmulShapeInfo)->IsTransposeB()) {
            int32_t alignNb = CeilAlign(matmulShapeInfo->template GetSingleCoreN<false, IsBasic(MM_CFG)>(), BLOCK_CUBE);
            int32_t alignKb = CeilAlign(matmulShapeInfo->template GetSingleCoreK<false, IsBasic(MM_CFG)>(), c0SizeB_);
            batchOffsetInfo.alignB = alignNb * alignKb;
        } else {
            constexpr int32_t alignSize = IsSameTypeV<TransBT, int8_t> ? c0SizeB_ : BLOCK_CUBE;
            int32_t alignNb = CeilAlign(matmulShapeInfo->template GetSingleCoreN<false, IsBasic(MM_CFG)>(), c0SizeB_);
            int32_t alignKb = CeilAlign(matmulShapeInfo->template GetSingleCoreK<false, IsBasic(MM_CFG)>(), alignSize);
            batchOffsetInfo.alignB = alignNb * alignKb;
        }
    }

    __aicore__ inline bool MoveNext()
    {
        if (unlikely(isFirstIter_)) {
            return MoveOnFirstIterate();
        } else {
            if constexpr (ToMatmulConfig(MM_CFG).bmmOutMode == BatchOutMode::MULTI_BATCH) {
                return false;
            }
            if constexpr (ToMatmulConfig(MM_CFG).iterateOrder == IterateOrder::UNDEF) {
                if (likely(
                        MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetIterateOrder() ==
                        static_cast<int>(IterateOrder::ORDER_M))) {
                    return MoveOnIterateOrderM();
                } else {
                    ASCENDC_ASSERT(
                        (MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetIterateOrder() ==
                         static_cast<int>(IterateOrder::ORDER_N)),
                        {
                            KERNEL_LOG(
                                KERNEL_ERROR, "iterateOrder is %d , which should be ORDER_N",
                                MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetIterateOrder());
                        });
                    return MoveOnIterateOrderN();
                }
            } else if constexpr (ToMatmulConfig(MM_CFG).iterateOrder == IterateOrder::ORDER_N) {
                return MoveOnIterateOrderN();
            } else {
                return MoveOnIterateOrderM();
            }
        }
        return true;
    }

    // Init & Update SplitParams
    __aicore__ inline void InitSplitAParams(SplitParams& aL0Params)
    {
        aL0Params.axisL0Len = IsStaticPaddingEnable(MM_CFG) ? MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM() :
                                                              MATMUL_MODULE(MLoop)->GetBaseShape();
        if constexpr ((A_TYPE::format == CubeFormat::VECTOR) || (A_TYPE::format == CubeFormat::SCALAR)) {
            aL0Params.axisL0Len = 1;
        } else if (!MatmulFeatureTrait<MM_CFG>::IsSupportDisableGemvMode() && aL0Params.axisL0Len == 1) {
            aL0Params.axisL0Len = BLOCK_CUBE;
        }

        aL0Params.kAxisL1Offset = 0;
        aL0Params.axisL1Offset = IsBasic(MM_CFG) ? 0 :
                                                   MATMUL_MODULE(MLoop)->GetOuterIdx() *
                                                       MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM();
    }

    __aicore__ inline void InitSplitBParams(SplitParams& bL0Params)
    {
        bL0Params.axisL0Len = IsStaticPaddingEnable(MM_CFG) ? MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN() :
                                                              MATMUL_MODULE(NLoop)->GetBaseShape();

        bL0Params.kAxisL1Offset = 0;
        bL0Params.axisL1Offset = IsBasic(MM_CFG) ? 0 :
                                                   MATMUL_MODULE(NLoop)->GetOuterIdx() *
                                                       MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN();
    }

    __aicore__ inline void UpdateSplitParams(SplitParams& aL0Params, SplitParams& bL0Params)
    {
        if constexpr (!IsBasic(MM_CFG)) {
            aL0Params.kAxisL1Offset =
                MATMUL_MODULE(KLoop)->GetOuterIdx() * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK();
            bL0Params.kAxisL1Offset = aL0Params.kAxisL1Offset;
        }
    }

    __aicore__ inline void UpdateSplitParams(
        bool enPartialSum, SplitParams& aL0Params, SplitParams& bL0Params, int32_t& sL0CInit, int32_t& sL0CLast)
    {
        if constexpr (!IsBasic(MM_CFG)) {
            aL0Params.kAxisL1Offset =
                MATMUL_MODULE(KLoop)->GetOuterIdx() * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK();
            bL0Params.kAxisL1Offset = aL0Params.kAxisL1Offset;
        }

        if (unlikely(MATMUL_MODULE(KLoop)->GetOuterIdx() == 0)) {
            sL0CInit = enPartialSum ? 0 : 1;
        } else {
            sL0CInit = 0;
        }
        if constexpr (EnUnitFlag(MM_CFG)) {
            sL0CLast = (MATMUL_MODULE(KLoop)->GetOuterIdx() == MATMUL_MODULE(KLoop)->GetTotalIter() - 1) ? 1 : 0;
        } else {
            sL0CLast = 0;
        }
    }

    // Compute related
    __aicore__ inline void MacroCompute(
        LocalTensor<TransAT>& a1, LocalTensor<TransBT>& b1, BatchSchedulerContext& ctx, int32_t sL0CInit,
        int32_t sL0CLast)
    {
        // prepare for split
        bool isATranspose = MATMUL_MODULE(MatmulShapeInfo)->IsTransposeA();
        bool isBTranspose = MATMUL_MODULE(MatmulShapeInfo)->IsTransposeB();
        MATMUL_MODULE(LoadToA2)->Prepare(isATranspose, ctx.aL0Params.kAxisL1Len, ctx.aL0Params.axisL1Len);
        MATMUL_MODULE(LoadToB2)->Prepare(isBTranspose, ctx.bL0Params.kAxisL1Len);
        int32_t kL0Len = MATMUL_MODULE(KLoop)->GetTileShapeA();
        // allocate L0 buffer
        auto& bufferPool = MATMUL_MODULE(TBufPoolL0)->Allocate();
        LocalTensor<TransAT> a2 = bufferPool.template GetBuffer<TPosition::A2, TransAT>();
        LocalTensor<TransBT> b2 = bufferPool.template GetBuffer<TPosition::B2, TransBT>();
        // split
        MATMUL_MODULE(LoadToA2)->Load(
            a2, a1[ctx.offsetA], ctx.aL0Params.axisL1Len, ctx.aL0Params.kAxisL1Len, ctx.aL0Params.axisL0Len, kL0Len,
            ctx.aL0Params.axisL1Offset, ctx.aL0Params.kAxisL1Offset, isATranspose);
        MATMUL_MODULE(LoadToB2)->Load(
            b2, b1[ctx.offsetB], ctx.bL0Params.axisL1Len, ctx.bL0Params.kAxisL1Len, ctx.bL0Params.axisL0Len, kL0Len,
            ctx.bL0Params.axisL1Offset, ctx.bL0Params.kAxisL1Offset, isBTranspose);
        bufferPool.EnQue();
        bufferPool.DeQue();

        // prepare params and compute
        bool cmatrixSource;
        bool cmatrixInitVal;
        UpdateMmadComputeParams(sL0CInit, cmatrixSource, cmatrixInitVal);
        int32_t offsetC = 0;
        if constexpr (ToMatmulConfig(MM_CFG).bmmOutMode != BatchOutMode::SINGLE_BATCH) {
            offsetC = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM() *
                      MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN() *
                      (MATMUL_MODULE(BatchLoop)->GetBatchOutCacheNum() - 1);
        }
        MATMUL_MODULE(MmadCompute)
            ->Compute(
                MATMUL_MODULE(CubeOutBuffer)->GetTensor()[offsetC], a2, b2, ctx.aL0Params.axisL0Len, kL0Len,
                ctx.bL0Params.axisL0Len, isATranspose, isBTranspose,
                MATMUL_MODULE(MatmulUnitFlag)->GetUnitFlag(sL0CLast), cmatrixSource, cmatrixInitVal, false);
        bufferPool.Free();
        MATMUL_MODULE(BiasScheduler)->Free();
    }

    __aicore__ inline void UpdateMmadComputeParams(bool sL0CInit, bool& cmatrixSource, bool& cmatrixInitVal)
    {
        if constexpr (MatmulFeatureTrait<MM_CFG>::IsNeedUB()) {
            cmatrixSource = false; // version below v220 does not support cmatrixSource
        } else {
            cmatrixSource = sL0CInit && MATMUL_MODULE(BiasScheduler)->IsBias();
        }
        cmatrixInitVal = sL0CInit && !MATMUL_MODULE(BiasScheduler)->IsBias();
    }

    // Copyout related
    template <class T>
    __aicore__ inline void GetBatchResult(
        const T& dst, const BatchSchedulerContext& ctx, uint8_t enAtomic, bool enSequentialWriteIn)
    {
        // supports continuous, discontinuous and reduce transfer on the GM. (three layout types are supported)
        if constexpr (A_TYPE::layout != LayoutMode::NORMAL) {
            if (ctx.isReduceG) {
                SetAtomicAdd<DstT>();
            }
        }

        int32_t stride = 0;
        const auto tiling = MATMUL_MODULE(MatmulShapeTiling)->GetTiling();
        if constexpr (C_TYPE::layout == LayoutMode::BNGS1S2 || C_TYPE::layout == LayoutMode::NORMAL) {
            // Scenario 1: Continuous copy
            int32_t alignedSingleCoreN = CeilAlign(tiling.GetSingleCoreN(), AscendCUtils::GetC0Count(sizeof(DstT)));
            if constexpr (PhyPosIsGM(C_TYPE::pos)) {
                alignedSingleCoreN = tiling.GetSingleCoreN();
            }
            if constexpr (C_TYPE::format == CubeFormat::NZ && IsSupportB32<DstT>()) {
                alignedSingleCoreN = CeilAlign(tiling.GetSingleCoreN(), BLOCK_CUBE);
            }
            stride = tiling.GetSingleCoreM() * alignedSingleCoreN;
        } else if constexpr (C_TYPE::layout == LayoutMode::BSNGD || C_TYPE::layout == LayoutMode::SBNGD) {
            ASSERT(enSequentialWriteIn == false && "Layout BSNGD or SBNGD can not be SequentialWrite");
            // Scenario 2: disconsecutive copy
            stride = tiling.GetSingleCoreN();
            if constexpr (C_TYPE::format == CubeFormat::ND_ALIGN) {
                stride = CeilAlign(tiling.GetSingleCoreN(), AscendCUtils::GetC0Count(sizeof(DstT)));
            }
        } else {
            ASSERT(false && "Can not support other Layout");
        }

        if constexpr (ToMatmulConfig(MM_CFG).bmmOutMode != BatchOutMode::SINGLE_BATCH) {
            CopyOut(dst[MATMUL_MODULE(BatchLoop)->GetBatchOutOffsetNum() * stride], enAtomic, enSequentialWriteIn);
        } else {
            uint32_t iBatch = 0;
            if constexpr (A_TYPE::layout != LayoutMode::NORMAL) {
                iBatch = ctx.isReduceG ? (MATMUL_MODULE(BatchLoop)->GetBatchIndex() / ctx.reduceGNum) :
                                         MATMUL_MODULE(BatchLoop)->GetBatchIndex();
            } else {
                iBatch = MATMUL_MODULE(BatchLoop)->GetBatchIndex();
            }
            CopyOut(dst[iBatch * stride], enAtomic, enSequentialWriteIn);
        }

        if constexpr (A_TYPE::layout != LayoutMode::NORMAL) {
            if (ctx.isReduceG) {
                SetAtomicNone();
            }
        }
    }

    __aicore__ inline void End()
    {
        MATMUL_MODULE(BatchCopyCubeInA)->Destroy();
        MATMUL_MODULE(BatchCopyCubeInB)->Destroy();
        MATMUL_MODULE(BiasScheduler)->End();
        MATMUL_MODULE(CubeOutBuffer)->Destroy();
    }

    __aicore__ inline void SetNBatchOutNum(int32_t nBatchOutNum) { nBatchOutNum_ = nBatchOutNum; }

private:
    __aicore__ inline void InitL0Params(BatchSchedulerContext& ctx)
    {
        const auto matmulShapeInfo = MATMUL_MODULE(MatmulShapeInfo);
        ctx.aL0Params.axisL1Len = CeilAlign(matmulShapeInfo->GetSingleCoreM(), BLOCK_CUBE);
        ctx.aL0Params.kAxisL1Len = (matmulShapeInfo->IsTransposeA() && !IsSupportB8<TransAT>()) ?
                                       CeilAlign(matmulShapeInfo->GetSingleCoreK(), BLOCK_CUBE) :
                                       CeilAlign(matmulShapeInfo->GetSingleCoreK(), c0Size_);
        ctx.bL0Params.axisL1Len = CeilAlign(matmulShapeInfo->GetSingleCoreN(), BLOCK_CUBE);
        ctx.bL0Params.kAxisL1Len = (matmulShapeInfo->IsTransposeB() || IsSupportB8<TransBT>()) ?
                                       CeilAlign(matmulShapeInfo->GetSingleCoreK(), c0SizeB_) :
                                       CeilAlign(matmulShapeInfo->GetSingleCoreK(), BLOCK_CUBE);
    }

    __aicore__ inline void CalcReduceGInfo(BatchSchedulerContext& ctx)
    {
        const auto tiling = MATMUL_MODULE(MatmulShapeTiling)->GetTiling();
        ctx.isReduceG =
            ((tiling.GetCLayoutInfoG() == 1) && (tiling.GetALayoutInfoG() != 1 || tiling.GetBLayoutInfoG() != 1));
        if (ctx.isReduceG) {
            ctx.reduceGNum = tiling.GetALayoutInfoG() >= tiling.GetBLayoutInfoG() ? tiling.GetALayoutInfoG() :
                                                                                    tiling.GetBLayoutInfoG();
        } else {
            ctx.reduceGNum = tiling.GetCLayoutInfoG();
        }
    }

    __aicore__ inline bool MoveOnFirstIterate()
    {
        isFirstIter_ = false;
        if (ToMatmulConfig(MM_CFG).scheduleType == ScheduleType::OUTER_PRODUCT) {
            if constexpr (ToMatmulConfig(MM_CFG).iterateOrder == IterateOrder::ORDER_M) {
                MATMUL_MODULE(NLoop)->OuterStart();
                MATMUL_MODULE(MLoop)->InnerStart();
            } else {
                MATMUL_MODULE(MLoop)->OuterStart();
                MATMUL_MODULE(NLoop)->InnerStart();
            }
        } else {
            MATMUL_MODULE(MLoop)->OuterStart();
            MATMUL_MODULE(NLoop)->OuterStart();
        }
        return true;
    }

    __aicore__ inline bool MoveOnIterateOrderM()
    {
        if (!MATMUL_MODULE(NLoop)->OuterNext()) {
            if (!MATMUL_MODULE(MLoop)->InnerNext()) {
                return false;
            }
            MATMUL_MODULE(NLoop)->OuterStart();
        }
        return true;
    }

    __aicore__ inline bool MoveOnIterateOrderN()
    {
        if (!MATMUL_MODULE(MLoop)->OuterNext()) {
            if (!MATMUL_MODULE(NLoop)->InnerNext()) {
                return false;
            }
            MATMUL_MODULE(MLoop)->OuterStart();
        }
        return true;
    }

    template <class T>
    __aicore__ inline void CopyOut(const T& dst, uint8_t enAtomic, bool enSequentialWrite)
    {
        auto co1Local = MATMUL_MODULE(CubeOutBuffer)->GetTensor();
        MATMUL_MODULE(CubeOutBuffer)->EnQue(co1Local);
        MATMUL_MODULE(CubeOutBuffer)->DeQue();
        if constexpr (ToMatmulConfig(MM_CFG).scheduleType == ScheduleType::OUTER_PRODUCT) {
            GetTensorCImplL0Db(dst, co1Local, enAtomic, enSequentialWrite);
        } else {
            GetTensorCImpl(
                dst, co1Local, MATMUL_MODULE(MLoop)->GetOuterIdx(), MATMUL_MODULE(NLoop)->GetOuterIdx(),
                MATMUL_MODULE(MLoop)->GetBaseShape(), MATMUL_MODULE(NLoop)->GetBaseShape(),
                MATMUL_MODULE(MLoop)->GetBaseBlockShape(), MATMUL_MODULE(NLoop)->GetBaseBlockShape(), enAtomic,
                enSequentialWrite);
        }
        MATMUL_MODULE(CubeOutBuffer)->FreeTensor(co1Local);
    }

    template <class T>
    __aicore__ inline void GetTensorCImplL0Db(
        const T& dst, const LocalTensor<L0cT>& co1Local, uint8_t enAtomic, bool enSequentialWrite)
    {
        const auto mLoop = MATMUL_MODULE(MLoop);
        const auto nLoop = MATMUL_MODULE(NLoop);
        const auto& tiling = MATMUL_MODULE(MatmulShapeTiling)->GetTiling();
        if (mLoop->GetL0DBLoopNum() > 1) {
            GetTensorCImpl(
                dst, co1Local, mLoop->GetOuterIdx(), nLoop->GetOuterIdx(), mLoop->GetBaseShape(), nLoop->GetBaseShape(),
                mLoop->GetBaseBlockShape(), nLoop->GetBaseBlockShape(), enAtomic, enSequentialWrite);
            int32_t baseUseM =
                (mLoop->GetOuterIdx() + 2 == mLoop->GetTotalIter()) ? mLoop->GetTail() : tiling.GetBaseM();
            int32_t blockUseM = Ceil(baseUseM, BLOCK_CUBE);
            GetTensorCImpl(
                dst, co1Local[tiling.GetBaseM() * tiling.GetBaseN()], mLoop->GetOuterIdx() + 1, nLoop->GetOuterIdx(),
                baseUseM, nLoop->GetBaseShape(), blockUseM, nLoop->GetBaseBlockShape(), enAtomic, enSequentialWrite);
        } else if (nLoop->GetL0DBLoopNum() > 1) {
            GetTensorCImpl(
                dst, co1Local, mLoop->GetOuterIdx(), nLoop->GetOuterIdx(), mLoop->GetBaseShape(), nLoop->GetBaseShape(),
                mLoop->GetBaseBlockShape(), nLoop->GetBaseBlockShape(), enAtomic, enSequentialWrite);
            int32_t baseUseN =
                (nLoop->GetOuterIdx() + 2 == nLoop->GetTotalIter()) ? nLoop->GetTail() : tiling.GetBaseN();
            int32_t blockUseN = Ceil(baseUseN, BLOCK_CUBE);
            GetTensorCImpl(
                dst, co1Local[tiling.GetBaseM() * tiling.GetBaseN()], mLoop->GetOuterIdx(), nLoop->GetOuterIdx() + 1,
                mLoop->GetBaseShape(), baseUseN, mLoop->GetBaseBlockShape(), blockUseN, enAtomic, enSequentialWrite);
        } else {
            GetTensorCImpl(
                dst, co1Local, mLoop->GetOuterIdx(), nLoop->GetOuterIdx(), mLoop->GetBaseShape(), nLoop->GetBaseShape(),
                mLoop->GetBaseBlockShape(), nLoop->GetBaseBlockShape(), enAtomic, enSequentialWrite);
        }
    }

    __aicore__ inline void GetTensorCImpl(
        const GlobalTensor<DstT>& gm, const LocalTensor<L0cT>& co1Local, int curRow, int curCol, int32_t baseHeight,
        int32_t baseWidth, int32_t baseBlockHeight, int32_t baseBlockWidth, uint8_t enAtomic, bool enSequentialWrite)
    {
        SetAtomic(enAtomic); // remove dependency conflicts only for scene which is not db
        GetTensorCImplCore(
            gm, co1Local, curRow, curCol, baseHeight, baseWidth, baseBlockHeight, baseBlockWidth, enAtomic,
            enSequentialWrite);
        if (enAtomic != 0) {
            SetAtomicNone();
        }
    }

    __aicore__ inline void GetTensorCImpl(
        const LocalTensor<DstT>& co2Local, const LocalTensor<L0cT>& co1Local, int curRow, int curCol,
        int32_t baseHeight, int32_t baseWidth, int32_t baseBlockHeight, int32_t baseBlockWidth, uint8_t enAtomic,
        bool enSequentialWrite)
    {
        GetTensorCImplCore(
            co2Local, co1Local, curRow, curCol, baseHeight, baseWidth, baseBlockHeight, baseBlockWidth, enAtomic,
            enSequentialWrite);
    }

    template <class T>
    __aicore__ inline void GetTensorCImplCore(
        const T& dst, const LocalTensor<L0cT>& co1Local, int32_t curRow, int32_t curCol, int32_t baseHeight,
        int32_t baseWidth, int32_t baseBlockHeight, int32_t baseBlockWidth, uint8_t enAtomic, bool enSequentialWrite)
    {
        if (enSequentialWrite) {
            MATMUL_MODULE(CopyCubeOut)
                ->template Copy<true>(
                    dst, co1Local, curRow, curCol, baseHeight, baseWidth, baseBlockHeight, baseBlockWidth);
        } else {
            MATMUL_MODULE(CopyCubeOut)
                ->template Copy<false>(
                    dst, co1Local, curRow, curCol, baseHeight, baseWidth, baseBlockHeight, baseBlockWidth);
        }
    }

    __aicore__ inline void SetAtomic(uint8_t enAtomic)
    {
        if constexpr (MatmulFeatureTrait<MM_CFG>::IsNeedUB()) {
            if (enAtomic == ATOMIC_ADD) {
                SetAtomicAdd<DstT>();
            }
        } else {
            if (enAtomic == ATOMIC_ADD) {
                SetAtomicAdd<DstT>();
            } else if (enAtomic == ATOMIC_MAX) {
                SetAtomicMax<DstT>();
            } else if (enAtomic == ATOMIC_MIN) {
                SetAtomicMin<DstT>();
            }
        }
    }

public:
    bool isFirstIter_;

private:
    constexpr static int32_t c0Size_ = AuxGetC0Size<TransAT>();
    constexpr static int32_t c0SizeB_ = c0Size_;
    int32_t nBatchOutNum_;
};
} // namespace Detail
} // namespace Impl
} // namespace AscendC
#endif // IMPL_MATMUL_SCHEDULER_BATCH_BATCH_SCHEDULER_BASE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BATCH_BATCH_SCHEDULER_BASE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BATCH_BATCH_SCHEDULER_BASE_H__
#endif
