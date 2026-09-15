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
 * \file batch_matmul_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/batch_matmul_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_BATCH_MATMUL_IMPL_H__
#endif

#ifndef IMPL_MATMUL_BATCH_MATMUL_IMPL_H
#define IMPL_MATMUL_BATCH_MATMUL_IMPL_H

#include "matmul_impl_base.h"

namespace AscendC {

// Match Policy with CallBack parameter
template <
    class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
class MatmulImpl<
    A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY, enable_if_t<A_TYPE::layout != LayoutMode::NONE>>
    : public MatmulImplBase<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>,
      MATMUL_IMPORT_MODULE(BatchScheduler),
      MATMUL_IMPORT_MODULE_PRIVATE(BatchCopyCubeInParamsA),
      MATMUL_IMPORT_MODULE_PRIVATE(BatchCopyCubeInParamsB),
      MATMUL_IMPORT_MODULE_PRIVATE(BatchCopyCubeInA),
      MATMUL_IMPORT_MODULE_PRIVATE(BatchCopyCubeInB),
      MATMUL_IMPORT_MODULE_PRIVATE(BatchLoop) {
private:
    using SrcAT = typename A_TYPE::T;
    using SrcBT = typename B_TYPE::T;
    using DstT = typename C_TYPE::T;
    using IMPL = MatmulImpl<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>;

public:
    MATMUL_ALLOW_USING(CopyCubeInA);
    MATMUL_ALLOW_USING(CopyCubeInB);
    MATMUL_ALLOW_USING(CopyCubeOut);
    MATMUL_ALLOW_USING(Scheduler);
    MATMUL_ALLOW_USING(BatchScheduler);
    MATMUL_ALLOW_USING_PRIVATE(BatchCopyCubeInParamsA);
    MATMUL_ALLOW_USING_PRIVATE(BatchCopyCubeInParamsB);
    MATMUL_ALLOW_USING_PRIVATE(BatchCopyCubeInA);
    MATMUL_ALLOW_USING_PRIVATE(BatchCopyCubeInB);
    MATMUL_ALLOW_USING_PRIVATE(BatchLoop);
    MATMUL_ALLOW_USING_PRIVATE(MatmulTensorInfoA);
    MATMUL_ALLOW_USING_PRIVATE(MatmulTensorInfoB);

    template <InputTypeTag TAG>
    using BatchCopyCubeInParams =
        typename AscendC::Conditional<TAG == InputTypeTag::A, BatchCopyCubeInParamsA, BatchCopyCubeInParamsB>::type;

    template <InputTypeTag TAG>
    using MatmulTensorInfo =
        typename AscendC::Conditional<TAG == InputTypeTag::A, MatmulTensorInfoA, MatmulTensorInfoB>::type;

private:
    MATMUL_USE_MODULE(CopyCubeInA);
    MATMUL_USE_MODULE(CopyCubeInB);
    MATMUL_USE_MODULE(Scheduler);
    MATMUL_USE_MODULE(BatchScheduler);
    MATMUL_USE_MODULE(BatchCopyCubeInA);
    MATMUL_USE_MODULE(BatchCopyCubeInB);
    MATMUL_USE_MODULE(BatchLoop);

    using ChosenCopyCubeInA = typename AscendC::Conditional<
        Impl::Detail::GetCopyCubeInType<MatmulInputAType<A_TYPE, typename A_TYPE::T>, MM_CFG>() !=
            Impl::Detail::CopyCubeInType::BMM,
        CopyCubeInA, BatchCopyCubeInA>::type;

    using ChosenCopyCubeInB = typename AscendC::Conditional<
        Impl::Detail::GetCopyCubeInType<MatmulInputBType<B_TYPE, typename B_TYPE::T>, MM_CFG>() !=
            Impl::Detail::CopyCubeInType::BMM,
        CopyCubeInB, BatchCopyCubeInB>::type;
    MATMUL_USE_MODULE(ChosenCopyCubeInA);
    MATMUL_USE_MODULE(ChosenCopyCubeInB);

public:
    using BASE_MODULE = MatmulImplBase<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>;
    __aicore__ inline MatmulImpl() {}

    __aicore__ inline void Init(const TCubeTiling* __restrict cubeTiling, TPipe* tpipe = nullptr)
    {
        auto tpipePtr = GetTPipePtr();
        MATMUL_MODULE(BatchScheduler)->Init(cubeTiling, tpipePtr);
    }

    __aicore__ inline void End() { MATMUL_MODULE(BatchScheduler)->End(); }

    __aicore__ inline void SetTensorA(const GlobalTensor<SrcAT>& gm, bool isTransposeA = false)
    {
        MATMUL_MODULE(ChosenCopyCubeInA)->SetInput(gm, isTransposeA);
        MATMUL_MODULE(Scheduler)->Reset();
    }

    __aicore__ inline void SetTensorA(const LocalTensor<SrcAT>& leftMatrix, bool isTransposeA = false)
    {
        BASE_MODULE::SetTensorA(leftMatrix, isTransposeA);
    }

    __aicore__ inline void SetTensorB(const GlobalTensor<SrcBT>& gm, bool isTransposeB = false)
    {
        MATMUL_MODULE(ChosenCopyCubeInB)->SetInput(gm, isTransposeB);
        MATMUL_MODULE(Scheduler)->Reset();
    }

    __aicore__ inline void SetTensorB(const LocalTensor<SrcBT>& rightMatrix, bool isTransposeB = false)
    {
        BASE_MODULE::SetTensorB(rightMatrix, isTransposeB);
    }

    __aicore__ inline void SetTensorA(SrcAT aScalar) { BASE_MODULE::SetTensorA(aScalar); }

    __aicore__ inline void SetTensorB(SrcBT bScalar) { BASE_MODULE::SetTensorB(bScalar); }

    __aicore__ inline void SetBatchNum(int32_t batchA, int32_t batchB)
    {
        MATMUL_MODULE(BatchLoop)->SetBatchNum(batchA, batchB);
    }

    __aicore__ inline void SetNBatchOutNum(int32_t nBatchOutNumIn)
    {
        int32_t nBatchOutNum = 1;
        if constexpr (ToMatmulConfig(MM_CFG).bmmOutMode != BatchOutMode::SINGLE_BATCH) {
            nBatchOutNum = nBatchOutNumIn;
        }
        MATMUL_MODULE(BatchScheduler)->SetNBatchOutNum(nBatchOutNum);
        MATMUL_MODULE(BatchLoop)->SetNBatchOutNum(nBatchOutNum);
    }

    __aicore__ inline void IterateBatch(
        const GlobalTensor<DstT>& gm, bool enPartialSum, uint8_t enAtomic, bool enSequentialWrite,
        const uint32_t matrixStrideA = 0, const uint32_t matrixStrideB = 0, const uint32_t matrixStrideC = 0)
    {
        MATMUL_MODULE(BatchScheduler)
            ->Schedule(gm, enPartialSum, enAtomic, enSequentialWrite, matrixStrideA, matrixStrideB, matrixStrideC);
    }

    __aicore__ inline void IterateBatch(
        const LocalTensor<DstT>& ubCmatrix, bool enPartialSum, uint8_t enAtomic, bool enSequentialWrite,
        const uint32_t matrixStrideA = 0, const uint32_t matrixStrideB = 0, const uint32_t matrixStrideC = 0)
    {
        MATMUL_MODULE(BatchScheduler)
            ->Schedule(
                ubCmatrix, enPartialSum, enAtomic, enSequentialWrite, matrixStrideA, matrixStrideB, matrixStrideC);
    }
};

} // namespace AscendC

#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_BATCH_MATMUL_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_BATCH_MATMUL_IMPL_H__
#endif
