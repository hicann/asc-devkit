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
 * \file bias_scheduler_v220.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/scheduler/bias/bias_scheduler_v220.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BIAS_BIAS_SCHEDULER_V220_H__
#endif

#ifndef IMPL_MATMUL_SCHEDULER_BIAS_BIAS_SCHEDULER_V220_H
#define IMPL_MATMUL_SCHEDULER_BIAS_BIAS_SCHEDULER_V220_H

#include "bias_scheduler_intf.h"
#include "bias_scheduler_base.h"

namespace AscendC {
namespace Impl {
namespace Detail {

/**
 * BiasScheduler: responsible for copy bias data management.
 * This module provides abilities to copy bias data in C2 or L0C.
 * We retain the freedom to make incompatible changes, but do not guarantee the stability.
 * BiasScheduler is only for internal usage, does not support extension or customized specialization!
 */
template <typename IMPL, class A_TYPE, class B_TYPE, class BIAS_TYPE, const auto& MM_CFG>
class BiasScheduler<
    IMPL, A_TYPE, B_TYPE, BIAS_TYPE, MM_CFG,
    enable_if_t<
        !MatmulFeatureTrait<MM_CFG>::IsNeedUB() && ToMatmulConfig(MM_CFG).enableSetBias &&
        (A_TYPE::layout == LayoutMode::NONE || ToMatmulConfig(MM_CFG).batchMode == BatchMode::SINGLE_LARGE_THAN_L1) &&
        (PhyPosIsUB(BIAS_TYPE::pos) || PhyPosIsGM(BIAS_TYPE::pos) || PhyPosIsL1(BIAS_TYPE::pos)) &&
        (DoMatmulMDL(MM_CFG) || isNormEnableScheduler<A_TYPE, MM_CFG> || IsBmmEnableScheduler<A_TYPE, MM_CFG> ||
         DoMatmulSpecialMDL(MM_CFG) || IsBasicBlockEnable<MM_CFG> || DoMatmulIBShareNorm(MM_CFG))>>
    : public BiasSchedulerBase<IMPL, A_TYPE, B_TYPE, BIAS_TYPE, MM_CFG> {
    MATMUL_USE_MODULE(KLoop);
    MATMUL_USE_MODULE(CopyBiasIn);
    MATMUL_USE_MODULE(LoadBias2C2);
    MATMUL_USE_MODULE(C1Buffer);
    MATMUL_USE_MODULE(C2Buffer);
    MATMUL_USE_MODULE(MatmulShapeTiling);

    using BiasT = typename BIAS_TYPE::T;

public:
    using BASE_MODULE = AscendC::Impl::Detail::BiasSchedulerBase<IMPL, A_TYPE, B_TYPE, BIAS_TYPE, MM_CFG>;

    __aicore__ inline BiasScheduler() = default;
    __aicore__ inline ~BiasScheduler() = default;

    __aicore__ inline void Init(int32_t batchNum = 0)
    {
        (void)batchNum;
        if (MATMUL_MODULE(MatmulShapeTiling)->GetTiling().IsBias()) {
            // bias UB in for david, because david supports UB->L1 path
            if constexpr (PhyPosIsUB(BIAS_TYPE::pos) && MatmulFeatureTrait<MM_CFG>::IsSupportUBToL1Singleshape()) {
                MATMUL_MODULE(C1Buffer)->Init(
                    CeilAlign(
                        MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetSingleCoreN() * sizeof(BiasT), ONE_BLK_SIZE) /
                    sizeof(BiasT));
            } else {
                if constexpr (isL0DBScene_) {
                    MATMUL_MODULE(C1Buffer)->Init(DB_NUM * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN());
                } else {
                    MATMUL_MODULE(C1Buffer)->Init(MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN());
                }
            }
            MATMUL_MODULE(C2Buffer)->Init();
        }
    }

    __aicore__ inline LocalTensor<BiasT> CopyIn(int32_t dataLen, int32_t dataNum = 1, int32_t srcOffset = 0)
    {
        LocalTensor<BiasT> biasC1;
        if constexpr (PhyPosIsL1(BIAS_TYPE::pos)) {
            biasC1 = (BASE_MODULE::srcTensor_)[srcOffset];
        } else if (BASE_MODULE::enableBias_ && MATMUL_MODULE(KLoop)->FirstOuterIter()) {
            biasC1 = MATMUL_MODULE(C1Buffer)->AllocTensor();
            // bias UB in for david, because david supports UB->L1 path
            if constexpr (PhyPosIsUB(BIAS_TYPE::pos) && MatmulFeatureTrait<MM_CFG>::IsSupportUBToL1Singleshape()) {
                biasC1 = biasC1[srcOffset];
            } else {
                MATMUL_MODULE(CopyBiasIn)
                    ->Copy(biasC1, BASE_MODULE::srcTensor_, dataLen, dataNum, srcOffset + BASE_MODULE::singleOffset_);
                MATMUL_MODULE(C1Buffer)->EnQue(biasC1);
                MATMUL_MODULE(C1Buffer)->DeQue();
            }
        }
        return biasC1;
    }

    __aicore__ inline void Free(LocalTensor<BiasT>& biasC1)
    {
        // biasC1 use end, free it
        if (!PhyPosIsL1(BIAS_TYPE::pos) && BASE_MODULE::enableBias_ && MATMUL_MODULE(KLoop)->FirstOuterIter()) {
            MATMUL_MODULE(C1Buffer)->FreeTensor(biasC1);
        }
    }

    __aicore__ inline void SplitLoad(LocalTensor<BiasT>& biasC1, int32_t dataLen, int32_t srcOffset = 0)
    {
        if (BASE_MODULE::enableBias_ && MATMUL_MODULE(KLoop)->FirstOuterIter()) {
            auto biasC2 = MATMUL_MODULE(C2Buffer)->Allocate();
            MATMUL_MODULE(LoadBias2C2)->Load(biasC2, biasC1[srcOffset], dataLen);
            MATMUL_MODULE(C2Buffer)->EnQue();
            MATMUL_MODULE(C2Buffer)->DeQue();
        }
    }

    __aicore__ inline void Free()
    {
        if (BASE_MODULE::enableBias_ && MATMUL_MODULE(KLoop)->FirstOuterIter() &&
            MATMUL_MODULE(KLoop)->FirstInnerIter()) {
            MATMUL_MODULE(C2Buffer)->Free();
        }
    }

    __aicore__ inline void End()
    {
        if (MATMUL_MODULE(MatmulShapeTiling)->GetTiling().IsBias()) {
            MATMUL_MODULE(C1Buffer)->Destroy();
        }
    }

private:
    constexpr static int32_t DB_NUM = 2;
    constexpr static bool isL0DBScene_ =
        (MatmulFeatureTrait<MM_CFG>().IsSupportMNL0DB() &&
         ToMatmulConfig(MM_CFG).iterateOrder == IterateOrder::ORDER_M);
};

} // namespace Detail
} // namespace Impl
} // namespace AscendC
#endif // _BIAS_SCHEDULER_V220_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BIAS_BIAS_SCHEDULER_V220_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BIAS_BIAS_SCHEDULER_V220_H__
#endif
