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
 * \file scheduler_norm_base.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/scheduler/base/scheduler_norm_base.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BASE_SCHEDULER_NORM_BASE_H__
#endif

#ifndef IMPL_MATMUL_SCHEDULER_BASE_SCHEDULER_NORM_BASE_H
#define IMPL_MATMUL_SCHEDULER_BASE_SCHEDULER_NORM_BASE_H

#include "scheduler_intf.h"
#include "scheduler_base.h"

namespace AscendC {
namespace Impl {
namespace Detail {
/*
    MatmulNormSchedulerBase is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    MatmulNormSchedulerBase is only for internal usage, does not support extension or customized specialization!
*/
template <
    typename IMPL, class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG,
    PolicyType POLICY_TYPE = PolicyType::MATMUL_DEFAULT>
class MatmulNormSchedulerBase
    : public MatmulSchedulerBase<IMPL, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, POLICY_TYPE> {
    MATMUL_USE_MODULE(MLoop);
    MATMUL_USE_MODULE(NLoop);
    MATMUL_USE_MODULE(KLoop);
    MATMUL_USE_MODULE(CopyCubeInA);
    MATMUL_USE_MODULE(CopyCubeInB);
    MATMUL_USE_MODULE(MatmulShapeTiling);
    MATMUL_USE_MODULE(MatmulShapeInfo);
    MATMUL_USE_MODULE(LoadToA2);
    MATMUL_USE_MODULE(LoadToB2);
    MATMUL_USE_MODULE(TBufPoolL0);
    MATMUL_USE_MODULE(MmadCompute);
    MATMUL_USE_MODULE(BiasScheduler);
    MATMUL_USE_MODULE(CubeOutBuffer);

    using TransBT = typename decltype(GetTransBDataType<A_TYPE, B_TYPE, MM_CFG>())::T;
    using BiasT = typename BIAS_TYPE::T;

public:
    using BASE_MODULE =
        AscendC::Impl::Detail::MatmulSchedulerBase<IMPL, A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, POLICY_TYPE>;

    __aicore__ inline bool ScheduleOnce(bool enPartialSum) { return false; }

    __aicore__ inline void Reset() { isFirstIter_ = true; }

protected:
    __aicore__ inline bool MoveNext()
    {
        bool ret = false;
        if constexpr (ToMatmulConfig(MM_CFG).singleCoreM != 0 && GetMIter(MM_CFG) == 1 && GetNIter(MM_CFG) == 1) {
            // only iterate once
            if (unlikely(isFirstIter_)) {
                ret = MoveOnFirstIterate();
            } else {
                ret = false;
            }
        } else {
            ret = MoveNextMulti();
        }
        return ret;
    }

    __aicore__ inline bool MoveOnFirstIterate()
    {
        // start MLoop and NLoop on the first iteration
        MATMUL_MODULE(MLoop)->OuterStart();
        MATMUL_MODULE(MLoop)->InnerStart();
        MATMUL_MODULE(NLoop)->OuterStart();
        MATMUL_MODULE(NLoop)->InnerStart();
        isFirstIter_ = false;
        return true;
    }

    __aicore__ inline bool MoveNextMulti()
    {
        if (unlikely(isFirstIter_)) {
            return MoveOnFirstIterate();
        } else {
            if constexpr (ToMatmulConfig(MM_CFG).iterateOrder == IterateOrder::ORDER_N) {
                return MoveOnIterateOrderN();
            } else if constexpr (ToMatmulConfig(MM_CFG).iterateOrder == IterateOrder::ORDER_M) {
                return MoveOnIterateOrderM();
            } else {
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
            }
        }
        return true;
    }

    __aicore__ inline bool MoveOnIterateOrderN()
    {
        if constexpr (DoMatmulIBShareNorm(MM_CFG) && A_TYPE::ibShare) {
            ASCENDC_ASSERT(
                (MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepM() >= MATMUL_MODULE(MLoop)->GetTotalIter()), {
                    KERNEL_LOG(
                        KERNEL_ERROR, "When iterateOrder is orderN and A is IBShare, stepM >= mIter is required");
                });
        }
        // when M inner loop is finished, clear right matrix's data in L1 buffer, and restart M inner loop
        if (!MATMUL_MODULE(MLoop)->InnerNext()) {
            MATMUL_MODULE(CopyCubeInB)->Reset();
            MATMUL_MODULE(MLoop)->InnerStart();
            // when N outer and inner loop both are finished, clear left matrix's data in L1 buffer,
            // and restart N outer and inner loop.
            if (!MATMUL_MODULE(NLoop)->InnerNext()) {
                if (!MATMUL_MODULE(NLoop)->OuterNext()) {
                    MATMUL_MODULE(NLoop)->OuterStart();
                    MATMUL_MODULE(NLoop)->InnerStart();
                    MATMUL_MODULE(CopyCubeInA)->Reset();
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

    __aicore__ inline bool MoveOnIterateOrderM()
    {
        if constexpr (DoMatmulIBShareNorm(MM_CFG) && B_TYPE::ibShare) {
            ASCENDC_ASSERT(
                (MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetStepN() >= MATMUL_MODULE(NLoop)->GetTotalIter()), {
                    KERNEL_LOG(
                        KERNEL_ERROR, "When iterateOrder is orderM and B is IBShare, stepN >= nIter is required");
                });
        }
        // when N inner loop is finished, clear left matrix's data in L1 buffer, and restart N inner loop
        if (!MATMUL_MODULE(NLoop)->InnerNext()) {
            MATMUL_MODULE(CopyCubeInA)->Reset();
            MATMUL_MODULE(NLoop)->InnerStart();
            // when M outer and inner loop both are finished, clear right matrix's data in L1 buffer,
            // and restart M outer and inner loop
            if (!MATMUL_MODULE(MLoop)->InnerNext()) {
                if (!MATMUL_MODULE(MLoop)->OuterNext()) {
                    MATMUL_MODULE(MLoop)->OuterStart();
                    MATMUL_MODULE(MLoop)->InnerStart();
                    MATMUL_MODULE(CopyCubeInB)->Reset();
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

    __aicore__ inline void UpdateComputeParams(const bool enPartialSum, bool& sL0CInit, bool& sL0CLast)
    {
        if (unlikely(MATMUL_MODULE(KLoop)->GetOuterIdx() == 0)) {
            sL0CInit = !enPartialSum;
        }
        if constexpr (EnUnitFlag(MM_CFG)) {
            sL0CLast = MATMUL_MODULE(KLoop)->GetOuterIdx() == MATMUL_MODULE(KLoop)->GetTotalIter() - 1;
        }
    }

    template <bool IS_INTRA_BLOCK = false>
    __aicore__ inline SplitParams InitSplitAParams()
    {
        SplitParams aL0Params;
        // if it's constant tiling sence, get params from tiling, else get params from loop
        if constexpr (IsStaticPaddingEnable(MM_CFG)) {
            aL0Params.axisL1Len = CeilAlign(MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM(), BLOCK_CUBE);
            aL0Params.axisL0Len = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM();
            aL0Params.kAxisL1Len = CeilAlign(MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK(), c0Size_);
        } else {
            if constexpr (IS_INTRA_BLOCK) {
                aL0Params.axisL1Len = MATMUL_MODULE(MLoop)->template GetTileBlockShape<true>() * BLOCK_CUBE;
                aL0Params.axisL0Len = MATMUL_MODULE(MLoop)->template GetBaseShape<true>();
            } else {
                aL0Params.axisL1Len = MATMUL_MODULE(MLoop)->GetTileBlockShape() * BLOCK_CUBE;
                aL0Params.axisL0Len = MATMUL_MODULE(MLoop)->GetBaseShape();
            }
        }
        aL0Params.axisL0Len = GetFixedMadM(aL0Params.axisL0Len);
        aL0Params.kAxisL1Offset = 0;
        // if input is from L1, update related params
        if constexpr (PhyPosIsL1OrUB<MM_CFG>(A_TYPE::pos)) {
            aL0Params.axisL1Len =
                CeilAlign(MATMUL_MODULE(MatmulShapeInfo)->template GetSingleCoreM<IS_INTRA_BLOCK>(), BLOCK_CUBE);
            if constexpr (IsBasic(MM_CFG)) {
                aL0Params.axisL1Offset = 0;
                if constexpr (MatmulFeatureTrait<MM_CFG>::IsSupportLoad2dV2()) {
                    aL0Params.kAxisL1Len =
                        IsSupportB8<typename A_TYPE::T>() ?
                            CeilAlign(MATMUL_MODULE(MatmulShapeInfo)->template GetSingleCoreK<false>(), c0Size_) :
                            CeilAlign(MATMUL_MODULE(MatmulShapeInfo)->template GetSingleCoreK<false>(), BLOCK_CUBE);
                    return aL0Params;
                }
            } else {
                aL0Params.axisL1Offset =
                    MATMUL_MODULE(MLoop)->GetInnerIdx() * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM();
            }
            if (MATMUL_MODULE(MatmulShapeInfo)->template IsTransposeA<IS_INTRA_BLOCK>()) {
                aL0Params.kAxisL1Len =
                    CeilAlign(MATMUL_MODULE(MatmulShapeInfo)->template GetSingleCoreK<IS_INTRA_BLOCK>(), BLOCK_CUBE);
            } else {
                aL0Params.kAxisL1Len =
                    CeilAlign(MATMUL_MODULE(MatmulShapeInfo)->template GetSingleCoreK<IS_INTRA_BLOCK>(), c0Size_);
            }
        } else {
            aL0Params.axisL1Offset = 0;
        }
        return aL0Params;
    }

    template <bool IS_INTRA_BLOCK = false>
    __aicore__ inline SplitParams InitSplitBParams()
    {
        SplitParams bL0Params;
        // if it's constant tiling sence, get params from tiling, else get params from loop
        if constexpr (IsStaticPaddingEnable(MM_CFG)) {
            bL0Params.axisL1Len = CeilAlign(MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN(), BLOCK_CUBE);
            bL0Params.axisL0Len = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN();
            bL0Params.kAxisL1Len = CeilAlign(MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK(), c0Size_);
        } else {
            if constexpr (IS_INTRA_BLOCK) {
                bL0Params.axisL1Len = MATMUL_MODULE(NLoop)->template GetTileBlockShape<IS_INTRA_BLOCK>() * BLOCK_CUBE;
                bL0Params.axisL0Len = MATMUL_MODULE(NLoop)->template GetBaseShape<IS_INTRA_BLOCK>();
            } else {
                bL0Params.axisL1Len = MATMUL_MODULE(NLoop)->GetTileBlockShape() * BLOCK_CUBE;
                bL0Params.axisL0Len = MATMUL_MODULE(NLoop)->GetBaseShape();
            }
        }
        bL0Params.kAxisL1Offset = 0;
        // if input is from L1, update related params
        if constexpr (PhyPosIsL1OrUB<MM_CFG>(B_TYPE::pos)) {
            bL0Params.axisL1Len =
                CeilAlign(MATMUL_MODULE(MatmulShapeInfo)->template GetSingleCoreN<IS_INTRA_BLOCK>(), BLOCK_CUBE);
            if constexpr (IsBasic(MM_CFG)) {
                bL0Params.axisL1Offset = 0;
                if constexpr (MatmulFeatureTrait<MM_CFG>::IsSupportLoad2dV2()) {
                    bL0Params.kAxisL1Len =
                        IsSupportB8<typename B_TYPE::T>() ?
                            CeilAlign(MATMUL_MODULE(MatmulShapeInfo)->template GetSingleCoreK<false>(), c0Size_) :
                            CeilAlign(MATMUL_MODULE(MatmulShapeInfo)->template GetSingleCoreK<false>(), BLOCK_CUBE);
                    return bL0Params;
                }
            } else {
                bL0Params.axisL1Offset =
                    MATMUL_MODULE(NLoop)->GetInnerIdx() * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN();
            }
            if (MATMUL_MODULE(MatmulShapeInfo)->template IsTransposeB<IS_INTRA_BLOCK>()) {
                bL0Params.kAxisL1Len =
                    CeilAlign(MATMUL_MODULE(MatmulShapeInfo)->template GetSingleCoreK<IS_INTRA_BLOCK>(), c0Size_);
            } else {
                bL0Params.kAxisL1Len =
                    CeilAlign(MATMUL_MODULE(MatmulShapeInfo)->template GetSingleCoreK<IS_INTRA_BLOCK>(), BLOCK_CUBE);
            }
        } else {
            bL0Params.axisL1Offset = 0;
        }
        return bL0Params;
    }

    __aicore__ inline void SplitPrepare(
        const bool isATranspose, const bool isBTranspose, SplitParams& aL0Params, SplitParams& bL0Params)
    {
        UpdateSplitParams(aL0Params, bL0Params);
        MATMUL_MODULE(LoadToA2)->Prepare(isATranspose, aL0Params.kAxisL1Len, aL0Params.axisL1Len);
        MATMUL_MODULE(LoadToB2)->Prepare(isBTranspose, bL0Params.kAxisL1Len);
    }

    template <bool IS_INTRA_BLOCK = false>
    __aicore__ inline void UpdateSplitParams(SplitParams& aL0Params, SplitParams& bL0Params)
    {
        // update Split params related to K loop
        if constexpr (PhyPosIsL1OrUB<MM_CFG>(A_TYPE::pos)) {
            aL0Params.kAxisL1Offset =
                MATMUL_MODULE(KLoop)->GetOuterIdx() * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK();
        } else if constexpr (!IsStaticPaddingEnable(MM_CFG)) {
            if constexpr (IS_INTRA_BLOCK) {
                aL0Params.kAxisL1Len = MATMUL_MODULE(KLoop)->template GetTileBlockShapeA<true>() * c0Size_;
            } else {
                aL0Params.kAxisL1Len = MATMUL_MODULE(KLoop)->GetTileBlockShapeA() * c0Size_;
            }
        }
        if constexpr (PhyPosIsL1OrUB<MM_CFG>(B_TYPE::pos)) {
            bL0Params.kAxisL1Offset =
                MATMUL_MODULE(KLoop)->GetOuterIdx() * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK();
        } else if constexpr (!IsStaticPaddingEnable(MM_CFG)) {
            if constexpr (IS_INTRA_BLOCK) {
                bL0Params.kAxisL1Len = MATMUL_MODULE(KLoop)->template GetTileBlockShapeB<true>() * c0Size_;
            } else {
                bL0Params.kAxisL1Len = MATMUL_MODULE(KLoop)->GetTileBlockShapeB() * c0Size_;
            }
        }
    }

    __aicore__ inline void UpdateBiasParams(
        bool enPartialSum, bool sL0CInit, bool& cmatrixSource, bool& cmatrixInitVal, bool& isBias)
    {
        // update params for Compute
        if constexpr (MatmulFeatureTrait<MM_CFG>::IsSupportCmatrixInitVal()) {
            cmatrixInitVal = false;
            cmatrixSource = false;
            if (sL0CInit) {
                if (MATMUL_MODULE(BiasScheduler)->IsBias()) {
                    isBias = true;
                } else {
                    isBias = enPartialSum;
                }
            } else {
                isBias = true;
            }
        } else if constexpr (MatmulFeatureTrait<MM_CFG>::IsNeedUB()) {
            cmatrixSource = false;
            isBias = false;
            if (MATMUL_MODULE(BiasScheduler)->IsBias()) {
                cmatrixInitVal = false;
            } else {
                cmatrixInitVal = sL0CInit;
            }
        } else {
            isBias = false;
            if (MATMUL_MODULE(BiasScheduler)->IsBias()) {
                cmatrixSource = sL0CInit;
                cmatrixInitVal = false;
            } else {
                cmatrixSource = false;
                cmatrixInitVal = sL0CInit;
            }
        }
    }

    __aicore__ inline int16_t GetFixedMadM(int madM)
    {
        // in GEMV mode, set axisL0Len to 1, else if axisL0Len is 1, manually align to 16
        if constexpr ((A_TYPE::format == CubeFormat::VECTOR) || (A_TYPE::format == CubeFormat::SCALAR)) {
            return 1;
        } else {
            if (!MatmulFeatureTrait<MM_CFG>::IsSupportDisableGemvMode() && madM == 1) {
                return BLOCK_CUBE;
            } else {
                return madM;
            }
        }
    }

    __aicore__ inline void SplitBias(const int32_t dataLen)
    {
        LocalTensor<BiasT> bias;
        if constexpr (IsBasic(MM_CFG)) {
            bias = MATMUL_MODULE(BiasScheduler)->CopyIn(MATMUL_MODULE(NLoop)->GetBaseShape());
        } else {
            bias =
                MATMUL_MODULE(BiasScheduler)
                    ->CopyIn(
                        MATMUL_MODULE(NLoop)->GetBaseShape(), 1,
                        MATMUL_MODULE(NLoop)->GetInnerIdx() * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN());
        }
        MATMUL_MODULE(BiasScheduler)->SplitLoad(bias, dataLen);
        MATMUL_MODULE(BiasScheduler)->Free(bias);
    }

    bool isFirstIter_ = true;
    constexpr static int32_t c0Size_ = AuxGetC0Size<typename A_TYPE::T>();
};

} // namespace Detail
} // namespace Impl
} // namespace AscendC

#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BASE_SCHEDULER_NORM_BASE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BASE_SCHEDULER_NORM_BASE_H__
#endif
