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
 * \file scheduler_mdl_base.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/scheduler/base/scheduler_mdl_base.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BASE_SCHEDULER_MDL_BASE_H__
#endif

#ifndef IMPL_MATMUL_SCHEDULER_BASE_SCHEDULER_MDL_BASE_H
#define IMPL_MATMUL_SCHEDULER_BASE_SCHEDULER_MDL_BASE_H

#include "scheduler_intf.h"
#include "scheduler_base.h"

namespace AscendC {
namespace Impl {
namespace Detail {
constexpr uint32_t PRELOAD_M = 1;
constexpr uint32_t PRELOAD_N = 2;
constexpr uint32_t PRELOAD_K = 3;
/*
    MatmulMDLSchedulerBase is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    MatmulSchedulerBase is only for internal usage, does not support extension or customized specialization!
*/

/*
    MatmulMDLSchedulerBase is the base class for other specialized MatmulScheduler,
    it implements the common methods for mdl and specialmdl.
*/
template <
    typename IMPL, class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG,
    PolicyType POLICY_TYPE = PolicyType::MATMUL_DEFAULT, typename = void>
class MatmulMDLSchedulerBase
    : public MatmulSchedulerBase<IMPL, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, POLICY_TYPE> {
    MATMUL_USE_MODULE(MLoop);
    MATMUL_USE_MODULE(NLoop);
    MATMUL_USE_MODULE(KLoop);
    MATMUL_USE_MODULE(CopyCubeInA);
    MATMUL_USE_MODULE(CopyCubeInB);
    MATMUL_USE_MODULE(MatmulShapeTiling);
    MATMUL_USE_MODULE(MatmulShapeInfo);
    MATMUL_USE_MODULE(MatmulUnitFlag);
    MATMUL_USE_MODULE(MatmulCrossCoreSync);
    MATMUL_USE_MODULE(LoadToA2);
    MATMUL_USE_MODULE(LoadToB2);
    MATMUL_USE_MODULE(TBufPoolL0);
    MATMUL_USE_MODULE(MmadCompute);
    MATMUL_USE_MODULE(BiasScheduler);

    using TransAT = typename A_TYPE::T;
    using TransBT = typename decltype(GetTransBDataType<A_TYPE, B_TYPE, MM_CFG>())::T;
    using L0cT = typename GetMmDstType<typename A_TYPE::T>::Type;

public:
    using BASE_MODULE =
        AscendC::Impl::Detail::MatmulSchedulerBase<IMPL, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, POLICY_TYPE>;
    __aicore__ inline void Init(const TCubeTiling* __restrict cubeTiling, TPipe* tpipe)
    {
        BASE_MODULE::Init(cubeTiling, tpipe);
        if constexpr (ToMatmulConfig(MM_CFG).doMTE2Preload > 0) {
            if constexpr (ToMatmulConfig(MM_CFG).doMTE2Preload == PRELOAD_M) {
                ASCENDC_ASSERT(
                    MATMUL_MODULE(KLoop)->IsAKL1FullLoad(), { KERNEL_LOG(KERNEL_ERROR, "MK must fullload"); });
            } else if constexpr (ToMatmulConfig(MM_CFG).doMTE2Preload == PRELOAD_N) {
                ASCENDC_ASSERT(
                    MATMUL_MODULE(KLoop)->IsBKL1FullLoad(), { KERNEL_LOG(KERNEL_ERROR, "NK must fullload"); });
            }
            const auto& tiling = MATMUL_MODULE(MatmulShapeTiling)->GetTiling();
            uint32_t cacheA1Size = tiling.GetStepM() * tiling.GetStepKa();
            cacheA1Factor_ = (tiling.GetDepthA1() / cacheA1Size - 1) & 1;
            uint32_t cacheB1Size = tiling.GetStepN() * tiling.GetStepKb();
            cacheB1Factor_ = (tiling.GetDepthB1() / cacheB1Size - 1) & 1;
        }
    }
#if !defined(ASCENDC_CPU_DEBUG) && defined(__CCE_IS_AICORE__)
    __aicore__ inline void Init(const __gm__ TCubeTiling* gmCubeTiling, TPipe* tpipe)
    {
        TCubeTiling cubeTiling;
        CopyTiling<A_TYPE, B_TYPE, MM_CFG>(gmCubeTiling, cubeTiling);
        Init(&cubeTiling, tpipe);
    }
#endif
    __aicore__ inline void Reset() { isFirstIter_ = true; }

protected:
    __aicore__ inline bool MoveNext()
    {
        if (unlikely(isFirstIter_)) {
            return MoveOnFirstIterate();
        } else {
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
            } else if constexpr (ToMatmulConfig(MM_CFG).iterateOrder == IterateOrder::ORDER_M) {
                return MoveOnIterateOrderM();
            } else {
                return MoveOnIterateOrderN();
            }
        }
    }

    __aicore__ inline bool MoveOnFirstIterate()
    {
        isFirstIter_ = false;
        MATMUL_MODULE(MLoop)->OuterStart();
        MATMUL_MODULE(MLoop)->InnerStart();
        MATMUL_MODULE(NLoop)->OuterStart();
        MATMUL_MODULE(NLoop)->InnerStart();
        return true;
    }

    __aicore__ inline bool MoveOnIterateOrderM()
    {
        // when N inner loop is finished, and restart N inner loop
        if (!MATMUL_MODULE(NLoop)->InnerNext()) {
            MATMUL_MODULE(NLoop)->InnerStart();
            // when M inner loop is finished, clear left matrix's data in L1 buffer
            if (!MATMUL_MODULE(MLoop)->InnerNext()) {
                if constexpr (!PhyPosIsL1OrUB<MM_CFG>(A_TYPE::pos)) {
                    if ((MATMUL_MODULE(KLoop)->IsAKL1FullLoad() && !MATMUL_MODULE(MLoop)->IsAML1FullLoad()) ||
                        (MATMUL_MODULE(MLoop)->IsLastOuterIter() && MATMUL_MODULE(NLoop)->IsLastOuterIter())) {
                        MATMUL_MODULE(CopyCubeInA)->ClearLoadData();
                    }
                }
                // when M outer loop is finished, clear right matrix's data in L1 buffer,
                // and restart M outer and inner loop
                if (!MATMUL_MODULE(MLoop)->OuterNext()) {
                    if constexpr (!PhyPosIsL1OrUB<MM_CFG>(B_TYPE::pos)) {
                        if ((MATMUL_MODULE(KLoop)->IsBKL1FullLoad() && !MATMUL_MODULE(NLoop)->IsBNL1FullLoad()) ||
                            (MATMUL_MODULE(MLoop)->IsLastOuterIter() && MATMUL_MODULE(NLoop)->IsLastOuterIter())) {
                            MATMUL_MODULE(CopyCubeInB)->ClearLoadData();
                        }
                    }
                    MATMUL_MODULE(MLoop)->OuterStart();
                    MATMUL_MODULE(MLoop)->InnerStart();
                    // when N outer loop is finished, all the iterations are finished, end process
                    if (!MATMUL_MODULE(NLoop)->OuterNext()) {
                        return false;
                    }
                    // M loop is restarted, N inner loop should be restarted
                    MATMUL_MODULE(NLoop)->InnerStart();
                }
            }
        }
        return true;
    }

    __aicore__ inline bool MoveOnIterateOrderN()
    {
        // when M inner loop is finished, and restart M inner loop
        if (!MATMUL_MODULE(MLoop)->InnerNext()) {
            MATMUL_MODULE(MLoop)->InnerStart();
            // when N inner loop is finished, clear right matrix's data in L1 buffer
            if (!MATMUL_MODULE(NLoop)->InnerNext()) {
                if constexpr (!PhyPosIsL1OrUB<MM_CFG>(B_TYPE::pos)) {
                    if ((MATMUL_MODULE(KLoop)->IsBKL1FullLoad() && !MATMUL_MODULE(NLoop)->IsBNL1FullLoad()) ||
                        (MATMUL_MODULE(MLoop)->IsLastOuterIter() && MATMUL_MODULE(NLoop)->IsLastOuterIter())) {
                        MATMUL_MODULE(CopyCubeInB)->ClearLoadData();
                    }
                }
                // when N outer loop is finished, clear left matrix's data in L1 buffer,
                // and restart N outer and inner loop
                if (!MATMUL_MODULE(NLoop)->OuterNext()) {
                    if constexpr (!PhyPosIsL1OrUB<MM_CFG>(A_TYPE::pos)) {
                        if ((MATMUL_MODULE(KLoop)->IsAKL1FullLoad() && !MATMUL_MODULE(MLoop)->IsAML1FullLoad()) ||
                            (MATMUL_MODULE(MLoop)->IsLastOuterIter() && MATMUL_MODULE(NLoop)->IsLastOuterIter())) {
                            MATMUL_MODULE(CopyCubeInA)->ClearLoadData();
                        }
                    }
                    MATMUL_MODULE(NLoop)->OuterStart();
                    MATMUL_MODULE(NLoop)->InnerStart();
                    // when M outer loop is finished, all the iterations are finished, end process
                    if (!MATMUL_MODULE(MLoop)->OuterNext()) {
                        return false;
                    }
                    // N loop is restarted, M inner loop should be restarted
                    MATMUL_MODULE(MLoop)->InnerStart();
                }
            }
        }
        return true;
    }

    __aicore__ inline void CopyIn(LocalTensor<TransAT>& a1, LocalTensor<TransBT>& b1)
    {
        a1 = MATMUL_MODULE(CopyCubeInA)
                 ->LoadData(
                     MATMUL_MODULE(MLoop)->GetInnerIdx(), MATMUL_MODULE(KLoop)->GetInnerStartIdx(),
                     MATMUL_MODULE(MLoop)->GetTileShape(), MATMUL_MODULE(KLoop)->GetTileShapeA());
        b1 = MATMUL_MODULE(CopyCubeInB)
                 ->LoadData(
                     MATMUL_MODULE(KLoop)->GetInnerStartIdx(), MATMUL_MODULE(NLoop)->GetInnerIdx(),
                     MATMUL_MODULE(KLoop)->GetTileShapeB(), MATMUL_MODULE(NLoop)->GetTileShape());

        if constexpr (MatmulFeatureTrait<MM_CFG>::IsSupportUBToL1Singleshape()) {
            MATMUL_MODULE(MatmulCrossCoreSync)->WaitL1Ready();
        }

        DoPreloadLoad();
    }

    __aicore__ inline void SplitPrepare(
        const bool enPartialSum, const bool isATranspose, const bool isBTranspose, SplitParams& aL0Params,
        SplitParams& bL0Params, bool& sL0CInit, bool& sL0CLast)
    {
        UpdateSplitParams(aL0Params, bL0Params);
        UpdateComputeParams(enPartialSum, sL0CInit, sL0CLast);
        MATMUL_MODULE(LoadToA2)->Prepare(isATranspose, aL0Params.kAxisL1Len, aL0Params.axisL1Len);
        MATMUL_MODULE(LoadToB2)->Prepare(isBTranspose, bL0Params.kAxisL1Len);
    }

    __aicore__ inline LocalTensor<TransAT> SplitA(
        const LocalTensor<TransAT>& a1, const SplitParams& aL0Params, const bool isATranspose)
    {
        if constexpr (DoMatmulSpecialMDL(MM_CFG) || MatmulFeatureTrait<MM_CFG>().IsSupportMNL0DB()) {
            LocalTensor<TransAT> a2 = MATMUL_MODULE(TBufPoolL0)->template GetBuffer<TPosition::A2, TransAT>();
            MATMUL_MODULE(LoadToA2)->Load(
                a2, a1, aL0Params.axisL1Len, aL0Params.kAxisL1Len, aL0Params.axisL0Len,
                MATMUL_MODULE(KLoop)->GetBaseShape(), aL0Params.axisL1Offset, aL0Params.kAxisL1Offset, isATranspose);
            return a2;
        } else {
            auto posA = MATMUL_MODULE(MLoop)->GetInnerIdx() * MATMUL_MODULE(KLoop)->GetTotalIter() +
                        MATMUL_MODULE(KLoop)->GetInnerIdx();
            int32_t kL0Len = MATMUL_MODULE(KLoop)->GetBaseShape();
            // Split
            if (!(MATMUL_MODULE(TBufPoolL0)->template Hit<TPosition::A2>(posA))) {
                LocalTensor<TransAT> a2 = MATMUL_MODULE(TBufPoolL0)->template GetBuffer<TPosition::A2, TransAT>();
                MATMUL_MODULE(LoadToA2)->Load(
                    a2, a1, aL0Params.axisL1Len, aL0Params.kAxisL1Len, aL0Params.axisL0Len, kL0Len,
                    aL0Params.axisL1Offset, aL0Params.kAxisL1Offset, isATranspose);
                return a2;
            } else {
                return MATMUL_MODULE(TBufPoolL0)->template GetBuffer<TPosition::A2, TransAT>();
            }
        }
    }

    __aicore__ inline LocalTensor<TransBT> SplitB(
        const LocalTensor<TransBT>& b1, const SplitParams& bL0Params, const bool isBTranspose)
    {
        if constexpr (DoMatmulSpecialMDL(MM_CFG) || MatmulFeatureTrait<MM_CFG>().IsSupportMNL0DB()) {
            LocalTensor<TransBT> b2 = MATMUL_MODULE(TBufPoolL0)->template GetBuffer<TPosition::B2, TransBT>();
            // Split b2
            MATMUL_MODULE(LoadToB2)->Load(
                b2, b1, bL0Params.axisL1Len, bL0Params.kAxisL1Len, bL0Params.axisL0Len,
                MATMUL_MODULE(KLoop)->GetBaseShape(), bL0Params.axisL1Offset, bL0Params.kAxisL1Offset, isBTranspose);
            return b2;
        } else {
            auto posB = MATMUL_MODULE(NLoop)->GetInnerIdx() * MATMUL_MODULE(KLoop)->GetTotalIter() +
                        MATMUL_MODULE(KLoop)->GetInnerIdx();
            int32_t kL0Len = MATMUL_MODULE(KLoop)->GetBaseShape();
            if constexpr (HasSparseIndex<B_TYPE>()) {
                if (!(MATMUL_MODULE(TBufPoolL0)->template Hit<TPosition::B2>(posB))) {
                    LocalTensor<TransBT> b2 = MATMUL_MODULE(TBufPoolL0)->template GetBuffer<TPosition::B2, TransBT>();
                    MATMUL_MODULE(LoadToB2)->Load(
                        b2, b1, bL0Params.axisL1Len, bL0Params.kAxisL1Len, bL0Params.axisL0Len,
                        MATMUL_MODULE(KLoop)->GetBaseShape(), bL0Params.axisL1Offset, bL0Params.kAxisL1Offset,
                        isBTranspose, MATMUL_MODULE(CopyCubeInB)->GetSparseIndex());
                    return b2;
                } else {
                    return MATMUL_MODULE(TBufPoolL0)->template GetBuffer<TPosition::B2, TransBT>();
                }
            } else {
                if (!(MATMUL_MODULE(TBufPoolL0)->template Hit<TPosition::B2>(posB))) {
                    LocalTensor<TransBT> b2 = MATMUL_MODULE(TBufPoolL0)->template GetBuffer<TPosition::B2, TransBT>();
                    MATMUL_MODULE(LoadToB2)->Load(
                        b2, b1, bL0Params.axisL1Len, bL0Params.kAxisL1Len, bL0Params.axisL0Len,
                        MATMUL_MODULE(KLoop)->GetBaseShape(), bL0Params.axisL1Offset, bL0Params.kAxisL1Offset,
                        isBTranspose);
                    return b2;
                } else {
                    return MATMUL_MODULE(TBufPoolL0)->template GetBuffer<TPosition::B2, TransBT>();
                }
            }
        }
    }

    __aicore__ inline void CubeCompute(
        const LocalTensor<L0cT>& cMatrix, const LocalTensor<TransAT>& a2, const LocalTensor<TransBT>& b2,
        const uint16_t madM, const uint16_t madN, const uint16_t madK, const bool isATranspose, const bool isBTranspose,
        const bool sL0CInit, const bool sL0CLast)
    {
        int32_t kInnerStartIdx = IsMDLKFullLoad() ? 0 : MATMUL_MODULE(KLoop)->GetInnerStartIdx();
        auto unitFlag = MATMUL_MODULE(MatmulUnitFlag)
                            ->GetUnitFlag(
                                sL0CLast && (MATMUL_MODULE(KLoop)->GetInnerIdx() ==
                                             kInnerStartIdx + MATMUL_MODULE(KLoop)->GetInnerIter() - 1));
        bool cmatrixSource;
        bool cmatrixInitVal;
        UpdateMmadComputeParams(kInnerStartIdx, sL0CInit, cmatrixSource, cmatrixInitVal);
        if constexpr (HasSparseIndex<B_TYPE>()) {
            MATMUL_MODULE(MmadCompute)
                ->template Compute<true>(
                    cMatrix, a2, b2, madM, madK, madN, isATranspose, isBTranspose, unitFlag, cmatrixSource,
                    cmatrixInitVal);
        } else {
            MATMUL_MODULE(MmadCompute)
                ->Compute(
                    cMatrix, a2, b2, madM, madK, madN, isATranspose, isBTranspose, unitFlag, cmatrixSource,
                    cmatrixInitVal);
        }
    }

    __aicore__ inline void ResetCopyInBuffer()
    {
        // clear L1 buffers
        if constexpr (!PhyPosIsL1OrUB<MM_CFG>(A_TYPE::pos)) {
            if (!MATMUL_MODULE(KLoop)->IsAKL1FullLoad()) {
                MATMUL_MODULE(CopyCubeInA)->Reset();
            }
        }
        if constexpr (!PhyPosIsL1OrUB<MM_CFG>(B_TYPE::pos)) {
            if (!MATMUL_MODULE(KLoop)->IsBKL1FullLoad()) {
                MATMUL_MODULE(CopyCubeInB)->Reset();
            }
        }
    }

    __aicore__ inline void UpdateComputeParams(const bool enPartialSum, bool& sL0CInit, bool& sL0CLast)
    {
        if (unlikely(MATMUL_MODULE(KLoop)->FirstOuterIter())) {
            sL0CInit = !enPartialSum;
        }
        if constexpr (EnUnitFlag(MM_CFG)) {
            sL0CLast = MATMUL_MODULE(KLoop)->LastOuterIter();
        }
    }

    __aicore__ inline constexpr bool IsMDLKFullLoad()
    {
        if constexpr (IsStaticTilingEnable(MM_CFG)) {
            const auto& tiling = MATMUL_MODULE(MatmulShapeTiling)->GetTiling();
            return (tiling.GetStepKa() < tiling.GetStepKb() ? tiling.GetStepKa() : tiling.GetStepKb()) *
                       tiling.GetBaseK() >=
                   tiling.GetSingleCoreK();
        }
        return false;
    }

    __aicore__ inline void ClearL1BufferCache(int32_t& curKaOuterIdx, int32_t& curKbOuterIdx)
    {
        if constexpr (
            !PhyPosIsL1OrUB<MM_CFG>(A_TYPE::pos) && ToMatmulConfig(MM_CFG).doMTE2Preload != PRELOAD_M &&
            ToMatmulConfig(MM_CFG).doMTE2Preload != PRELOAD_N) {
            int32_t curKaIdx = MATMUL_MODULE(KLoop)->GetNextOuterKaIdx();
            // if next outerKaIdx is not equal to curKaOuterIdx, clear left matrix's data in L1 buffer
            if (curKaIdx != curKaOuterIdx && !MATMUL_MODULE(KLoop)->IsAKL1FullLoad()) {
                MATMUL_MODULE(CopyCubeInA)->ClearLoadData();
                curKaOuterIdx = curKaIdx;
            }
        }
        if constexpr (
            !PhyPosIsL1OrUB<MM_CFG>(B_TYPE::pos) && ToMatmulConfig(MM_CFG).doMTE2Preload != PRELOAD_M &&
            ToMatmulConfig(MM_CFG).doMTE2Preload != PRELOAD_N) {
            // if next outerKbIdx is not equal to curKbOuterIdx, clear right matrix's data in L1 buffer
            int32_t curKbIdx = MATMUL_MODULE(KLoop)->GetNextOuterKbIdx();
            if (curKbIdx != curKbOuterIdx && !MATMUL_MODULE(KLoop)->IsBKL1FullLoad()) {
                MATMUL_MODULE(CopyCubeInB)->ClearLoadData();
                curKbOuterIdx = curKbIdx;
            }
        }
    }

    __aicore__ inline SplitParams InitSplitAParams()
    {
        SplitParams aL0Params;
        int32_t tilingBaseM = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM();
        if constexpr (PhyPosIsL1OrUB<MM_CFG>(A_TYPE::pos)) {
            aL0Params.axisL1Offset = MATMUL_MODULE(MLoop)->GetInnerIdx() * tilingBaseM;
            // ds && 82 mdl support multi singleshape in l1
            if constexpr (IsFullStaticTiling(MM_CFG) || MatmulFeatureTrait<MM_CFG>::IsSupportUBToL1Singleshape()) {
                aL0Params.axisL1Len = MATMUL_MODULE(MatmulShapeInfo)->GetOrgM() != -1 ?
                                          MATMUL_MODULE(MatmulShapeInfo)->GetOrgM() :
                                          MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreM();
                aL0Params.kAxisL1Len = MATMUL_MODULE(MatmulShapeInfo)->GetOrgKa() != -1 ?
                                           MATMUL_MODULE(MatmulShapeInfo)->GetOrgKa() :
                                           MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreK();
            } else {
                aL0Params.axisL1Len = MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreM();
                aL0Params.kAxisL1Len = MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreK();
            }
        } else if constexpr (IsStaticPaddingEnable(MM_CFG)) {
            aL0Params.axisL1Len = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM() * tilingBaseM;
            aL0Params.kAxisL1Len = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepKa() *
                                   MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK();
            aL0Params.axisL1Offset =
                (MATMUL_MODULE(MLoop)->GetInnerIdx() -
                 MATMUL_MODULE(MLoop)->GetOuterIdx() * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM()) *
                tilingBaseM;
        } else {
            aL0Params.axisL1Len = MATMUL_MODULE(MLoop)->GetTileBlockShape() * BLOCK_CUBE;
            aL0Params.axisL1Offset =
                (MATMUL_MODULE(MLoop)->GetInnerIdx() -
                 MATMUL_MODULE(MLoop)->GetOuterIdx() * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM()) *
                tilingBaseM;
        }
        // nbuffer 33 L1 offset is 0
        if constexpr (IMPL::POLICY::POLICY_TYPE == PolicyType::MATMUL_NBUFFER_33) {
            aL0Params.axisL1Offset = 0;
        }
        if constexpr (IsStaticPaddingEnable(MM_CFG)) {
            aL0Params.axisL0Len = tilingBaseM;
        } else {
            aL0Params.axisL0Len = MATMUL_MODULE(MLoop)->GetBaseShape();
        }
        if constexpr ((A_TYPE::format == CubeFormat::VECTOR) || (A_TYPE::format == CubeFormat::SCALAR)) {
            aL0Params.axisL0Len = 1;
        } else {
            if constexpr (!MatmulFeatureTrait<MM_CFG>::IsSupportDisableGemvMode()) {
                if (aL0Params.axisL0Len == 1) {
                    aL0Params.axisL0Len = BLOCK_CUBE;
                }
            }
        }
        return aL0Params;
    }

    __aicore__ inline SplitParams InitSplitBParams()
    {
        SplitParams bL0Params;
        int32_t tilingBaseN = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN();
        if constexpr (IsStaticPaddingEnable(MM_CFG)) {
            bL0Params.axisL1Len = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepN() * tilingBaseN;
            bL0Params.kAxisL1Len = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepKb() *
                                   MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK();
            bL0Params.axisL0Len = tilingBaseN;
        } else {
            bL0Params.axisL1Len = MATMUL_MODULE(NLoop)->GetTileBlockShape() * BLOCK_CUBE;
            bL0Params.axisL0Len = MATMUL_MODULE(NLoop)->GetBaseShape();
        }
        if constexpr (PhyPosIsL1OrUB<MM_CFG>(B_TYPE::pos)) {
            bL0Params.axisL1Offset = MATMUL_MODULE(NLoop)->GetInnerIdx() * tilingBaseN;
            // ds && 82 mdl support multi singleshape in l1
            if constexpr (IsFullStaticTiling(MM_CFG) || MatmulFeatureTrait<MM_CFG>::IsSupportUBToL1Singleshape()) {
                bL0Params.axisL1Len = MATMUL_MODULE(MatmulShapeInfo)->GetOrgN() != -1 ?
                                          MATMUL_MODULE(MatmulShapeInfo)->GetOrgN() :
                                          MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreN();
                bL0Params.kAxisL1Len = MATMUL_MODULE(MatmulShapeInfo)->GetOrgKb() != -1 ?
                                           MATMUL_MODULE(MatmulShapeInfo)->GetOrgKb() :
                                           MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreK();
            } else {
                bL0Params.axisL1Len = MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreN();
                bL0Params.kAxisL1Len = MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreK();
            }
        } else {
            if constexpr (!MatmulFeatureTrait<MM_CFG>::IsNeedUB()) {
                if constexpr (IsSameTypeV<TransBT, int8_t>) {
                    int32_t stepN = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepN();
                    if (tilingBaseN % c0Size_ == 0 || stepN == 1) {
                        bL0Params.axisL1Offset =
                            (MATMUL_MODULE(NLoop)->GetInnerIdx() - MATMUL_MODULE(NLoop)->GetOuterIdx() * stepN) *
                            tilingBaseN;
                    } else {
                        bL0Params.axisL1Offset =
                            (MATMUL_MODULE(NLoop)->GetInnerIdx() - MATMUL_MODULE(NLoop)->GetOuterIdx() * stepN) *
                            CeilAlign(tilingBaseN, c0Size_);
                    }
                } else {
                    bL0Params.axisL1Offset =
                        tilingBaseN * (MATMUL_MODULE(NLoop)->GetInnerIdx() -
                                       MATMUL_MODULE(NLoop)->GetOuterIdx() *
                                           MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepN());
                }
            } else {
                bL0Params.axisL1Offset =
                    (MATMUL_MODULE(NLoop)->GetInnerIdx() -
                     MATMUL_MODULE(NLoop)->GetOuterIdx() * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepN()) *
                    tilingBaseN;
            }
        }
        return bL0Params;
    }

    __aicore__ inline void UpdateSplitParams(SplitParams& aL0Params, SplitParams& bL0Params)
    {
        int32_t tilingBaseK = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK();
        int32_t kInnerIdx = MATMUL_MODULE(KLoop)->GetInnerStartIdx();
        if constexpr (PhyPosIsL1OrUB<MM_CFG>(A_TYPE::pos)) {
            aL0Params.kAxisL1Offset = kInnerIdx * tilingBaseK;
        } else {
            aL0Params.kAxisL1Len = MATMUL_MODULE(KLoop)->GetTileBlockShapeA() * c0Size_;
            int32_t tilingStepKa = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepKa();
            aL0Params.kAxisL1Offset = (kInnerIdx - kInnerIdx / tilingStepKa * tilingStepKa) * tilingBaseK;
        }
        if constexpr (PhyPosIsL1OrUB<MM_CFG>(B_TYPE::pos)) {
            bL0Params.kAxisL1Offset = kInnerIdx * tilingBaseK;
        } else {
            bL0Params.kAxisL1Len = MATMUL_MODULE(KLoop)->GetTileBlockShapeB() * c0Size_;
            int32_t tilingStepKb = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepKb();
            bL0Params.kAxisL1Offset = (kInnerIdx - kInnerIdx / tilingStepKb * tilingStepKb) * tilingBaseK;
        }
    }

    __aicore__ inline void UpdateMmadComputeParams(
        int32_t kInnerStartIdx, bool sL0CInit, bool& cmatrixSource, bool& cmatrixInitVal)
    {
        if constexpr (MatmulFeatureTrait<MM_CFG>::IsNeedUB()) {
            if (MATMUL_MODULE(BiasScheduler)->IsBias()) {
                cmatrixSource = false;
                cmatrixInitVal = false;
            } else {
                cmatrixSource = false;
                cmatrixInitVal = MATMUL_MODULE(KLoop)->GetInnerIdx() == kInnerStartIdx && sL0CInit;
            }
        } else {
            if constexpr (ToMatmulConfig(MM_CFG).isPartialOutput) {
                // if isPartialOutput, disable l0c accumulative
                cmatrixSource = MATMUL_MODULE(BiasScheduler)->IsBias();
                cmatrixInitVal = !MATMUL_MODULE(BiasScheduler)->IsBias();
            } else {
                bool isInit = (MATMUL_MODULE(KLoop)->GetInnerIdx() == kInnerStartIdx) && sL0CInit;
                cmatrixSource = MATMUL_MODULE(BiasScheduler)->IsBias() ? isInit : false;
                cmatrixInitVal = MATMUL_MODULE(BiasScheduler)->IsBias() ? false : isInit;
            }
        }
    }

    __aicore__ inline void DoPreloadLoad()
    {
        if (MATMUL_MODULE(KLoop)->FirstOuterIter()) {
            if constexpr (ToMatmulConfig(MM_CFG).doMTE2Preload == PRELOAD_M) {
                if (cacheA1Factor_ == 1 &&
                    (MATMUL_MODULE(NLoop)->GetInnerIdx() % MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepN() ==
                     0) &&
                    (MATMUL_MODULE(MLoop)->GetInnerIdx() + MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM() <
                     MATMUL_MODULE(MLoop)->GetTotalIter())) {
                    // preload B1
                    MATMUL_MODULE(CopyCubeInA)
                        ->AsyncLoadData(
                            (MATMUL_MODULE(MLoop)->GetInnerIdx() +
                             MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM()) %
                                MATMUL_MODULE(MLoop)->GetTotalIter(),
                            0, MATMUL_MODULE(MLoop)->GetTileShapeOf(MATMUL_MODULE(MLoop)->GetOuterIdx() + 1),
                            MATMUL_MODULE(KLoop)->GetTileShapeA());
                }
            } else if constexpr (ToMatmulConfig(MM_CFG).doMTE2Preload == PRELOAD_N) {
                if ((cacheB1Factor_ == 1) &&
                    (MATMUL_MODULE(MLoop)->GetInnerIdx() % MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM() ==
                     0) &&
                    (MATMUL_MODULE(NLoop)->GetInnerIdx() + MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepN() <
                     MATMUL_MODULE(NLoop)->GetTotalIter())) {
                    MATMUL_MODULE(CopyCubeInB)
                        ->AsyncLoadData(
                            0,
                            (MATMUL_MODULE(NLoop)->GetInnerIdx() +
                             MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepN()) %
                                MATMUL_MODULE(NLoop)->GetTotalIter(),
                            MATMUL_MODULE(KLoop)->GetTileShapeB(),
                            MATMUL_MODULE(NLoop)->GetTileShapeOf(MATMUL_MODULE(NLoop)->GetOuterIdx() + 1));
                }
            }
        }

        if constexpr (ToMatmulConfig(MM_CFG).doMTE2Preload == PRELOAD_K) {
            auto kLoop = MATMUL_MODULE(KLoop);
            const auto& tiling = MATMUL_MODULE(MatmulShapeTiling)->GetTiling();
            if (cacheB1Factor_ == 1 && !MATMUL_MODULE(KLoop)->IsBKL1FullLoad() &&
                (kLoop->GetInnerStartIdx() < kLoop->GetTotalIter() - tiling.GetStepKb())) {
                // preload B1
                int32_t nextKbIdx = kLoop->GetInnerStartIdx() + tiling.GetStepKb();
                MATMUL_MODULE(CopyCubeInB)
                    ->AsyncLoadData(
                        nextKbIdx, MATMUL_MODULE(NLoop)->GetInnerIdx(),
                        kLoop->GetTileShapeBOf(nextKbIdx / tiling.GetStepKb()), MATMUL_MODULE(NLoop)->GetTileShape());
            } else if (
                cacheB1Factor_ == 1 && !MATMUL_MODULE(KLoop)->IsBKL1FullLoad() &&
                (kLoop->GetInnerStartIdx() == kLoop->GetTotalIter() - tiling.GetStepKb())) {
                // preload B1
                MATMUL_MODULE(CopyCubeInB)
                    ->AsyncLoadData(
                        0, (MATMUL_MODULE(NLoop)->GetInnerIdx() + 1) % MATMUL_MODULE(NLoop)->GetTotalIter(),
                        kLoop->GetTileShapeBOf(1), MATMUL_MODULE(NLoop)->GetTileShape());
            }
        }
    }

    __aicore__ inline void DoPreloadAWait()
    {
        if constexpr (ToMatmulConfig(MM_CFG).doMTE2Preload == PRELOAD_M) {
            if ((cacheA1Factor_ == 1) &&
                (MATMUL_MODULE(NLoop)->GetInnerIdx() >=
                 MATMUL_MODULE(NLoop)->GetOuterIdx() * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepN() +
                     MATMUL_MODULE(NLoop)->GetInnerIter() - 1) &&
                (MATMUL_MODULE(MLoop)->GetInnerIdx() + MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM() <
                 MATMUL_MODULE(MLoop)->GetTotalIter())) {
                MATMUL_MODULE(CopyCubeInA)->AwaitLoadData();
            }
        } else if constexpr (ToMatmulConfig(MM_CFG).doMTE2Preload == PRELOAD_N) {
            if ((cacheB1Factor_ == 1) && (MATMUL_MODULE(MLoop)->IsLastInnerIter()) &&
                (MATMUL_MODULE(NLoop)->GetInnerIdx() + MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepN() <
                 MATMUL_MODULE(NLoop)->GetTotalIter())) {
                MATMUL_MODULE(CopyCubeInB)->AwaitLoadData();
            }
        } else if constexpr (ToMatmulConfig(MM_CFG).doMTE2Preload == PRELOAD_K) {
            if (cacheB1Factor_ == 1 && !MATMUL_MODULE(KLoop)->IsBKL1FullLoad()) {
                MATMUL_MODULE(CopyCubeInB)->AwaitLoadData();
            }
        }
    }

protected:
    bool isFirstIter_ = true;
    constexpr static int32_t c0Size_ = AuxGetC0Size<typename A_TYPE::T>();
    int32_t cacheA1Factor_, cacheB1Factor_;
};

} // namespace Detail
} // namespace Impl
} // namespace AscendC
#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BASE_SCHEDULER_MDL_BASE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BASE_SCHEDULER_MDL_BASE_H__
#endif
