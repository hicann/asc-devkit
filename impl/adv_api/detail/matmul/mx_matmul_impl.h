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
 * \file mx_matmul_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/mx_matmul_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_MX_MATMUL_IMPL_H__
#endif

#ifndef IMPL_MATMUL_MX_MATMUL_IMPL_H
#define IMPL_MATMUL_MX_MATMUL_IMPL_H

#include "matmul_impl_base.h"

namespace AscendC {

// Match Policy with CallBack parameter
template <
    class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG, class MM_CB,
    MATMUL_POLICY_TEMPLATE_OF(MATMUL_POLICY)>
class MatmulImpl<
    A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY,
    enable_if_t<A_TYPE::layout == LayoutMode::NONE && isMxMatmul<A_TYPE, B_TYPE>>>
    : public MatmulImplBase<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>,
      MATMUL_IMPORT_MODULE(CopyCubeInScaleA),
      MATMUL_IMPORT_MODULE(CopyCubeInScaleB),
      MATMUL_IMPORT_MODULE(CubeInBufferScaleA),
      MATMUL_IMPORT_MODULE(CubeInBufferScaleB),
      MATMUL_IMPORT_MODULE_PRIVATE(MatmulTensorInfoScaleA),
      MATMUL_IMPORT_MODULE_PRIVATE(MatmulTensorInfoScaleB),
      MATMUL_IMPORT_MODULE_PRIVATE(CopyCubeInParamsScaleA),
      MATMUL_IMPORT_MODULE_PRIVATE(CopyCubeInParamsScaleB),
      MATMUL_IMPORT_MODULE_PRIVATE(DataCopyUtilsScaleA),
      MATMUL_IMPORT_MODULE_PRIVATE(DataCopyUtilsScaleB),
      MATMUL_IMPORT_MODULE_PRIVATE(DataCopyWrapperScaleA),
      MATMUL_IMPORT_MODULE_PRIVATE(DataCopyWrapperScaleB) {
private:
    using SrcScaleT = fp8_e8m0_t;
    using DstT = typename C_TYPE::T;
    using IMPL = MatmulImpl<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>;

public:
    MATMUL_ALLOW_USING(CubeInBufferA);
    MATMUL_ALLOW_USING(CubeInBufferB);
    MATMUL_ALLOW_USING(CopyCubeInScaleA);
    MATMUL_ALLOW_USING(CopyCubeInScaleB);
    MATMUL_ALLOW_USING(CubeInBufferScaleA);
    MATMUL_ALLOW_USING(CubeInBufferScaleB);
    MATMUL_ALLOW_USING(Scheduler);
    MATMUL_ALLOW_USING_PRIVATE(MatmulTensorInfoA);
    MATMUL_ALLOW_USING_PRIVATE(MatmulTensorInfoB);
    MATMUL_ALLOW_USING_PRIVATE(MatmulTensorInfoScaleA);
    MATMUL_ALLOW_USING_PRIVATE(MatmulTensorInfoScaleB);
    MATMUL_ALLOW_USING_PRIVATE(CopyCubeInParamsA);
    MATMUL_ALLOW_USING_PRIVATE(CopyCubeInParamsB);
    MATMUL_ALLOW_USING_PRIVATE(CopyCubeInParamsScaleA);
    MATMUL_ALLOW_USING_PRIVATE(CopyCubeInParamsScaleB);
    MATMUL_ALLOW_USING_PRIVATE(DataCopyUtilsA);
    MATMUL_ALLOW_USING_PRIVATE(DataCopyUtilsB);
    MATMUL_ALLOW_USING_PRIVATE(DataCopyUtilsScaleA);
    MATMUL_ALLOW_USING_PRIVATE(DataCopyUtilsScaleB);
    MATMUL_ALLOW_USING_PRIVATE(DataCopyWrapperA);
    MATMUL_ALLOW_USING_PRIVATE(DataCopyWrapperB);
    MATMUL_ALLOW_USING_PRIVATE(DataCopyWrapperScaleA);
    MATMUL_ALLOW_USING_PRIVATE(DataCopyWrapperScaleB);

private:
    MATMUL_USE_MODULE(CubeInBufferA);
    MATMUL_USE_MODULE(CubeInBufferB);
    MATMUL_USE_MODULE(CopyCubeInScaleA);
    MATMUL_USE_MODULE(CopyCubeInScaleB);
    MATMUL_USE_MODULE(CubeInBufferScaleA);
    MATMUL_USE_MODULE(CubeInBufferScaleB);
    MATMUL_USE_MODULE(Scheduler);
    MATMUL_USE_MODULE(MatmulCrossCoreSync);

public:
    template <InputTypeTag TAG>
    using CubeInBuffer = typename ConditionalMulti<
        TAG == InputTypeTag::A || TAG == InputTypeTag::B, TAG == InputTypeTag::A || TAG == InputTypeTag::scaleA,
        CubeInBufferA, CubeInBufferB, CubeInBufferScaleA, CubeInBufferScaleB>::type;

    template <InputTypeTag TAG>
    using CopyCubeInParams = typename ConditionalMulti<
        TAG == InputTypeTag::A || TAG == InputTypeTag::B, TAG == InputTypeTag::A || TAG == InputTypeTag::scaleA,
        CopyCubeInParamsA, CopyCubeInParamsB, CopyCubeInParamsScaleA, CopyCubeInParamsScaleB>::type;

    template <InputTypeTag TAG>
    using MatmulTensorInfo = typename ConditionalMulti<
        TAG == InputTypeTag::A || TAG == InputTypeTag::B, TAG == InputTypeTag::A || TAG == InputTypeTag::scaleA,
        MatmulTensorInfoA, MatmulTensorInfoB, MatmulTensorInfoScaleA, MatmulTensorInfoScaleB>::type;

    template <InputTypeTag TAG>
    using DataCopyUtils = typename ConditionalMulti<
        TAG == InputTypeTag::A || TAG == InputTypeTag::B, TAG == InputTypeTag::A || TAG == InputTypeTag::scaleA,
        DataCopyUtilsA, DataCopyUtilsB, DataCopyUtilsScaleA, DataCopyUtilsScaleB>::type;

    template <InputTypeTag TAG>
    using DataCopyWrapper = typename ConditionalMulti<
        TAG == InputTypeTag::A || TAG == InputTypeTag::B, TAG == InputTypeTag::A || TAG == InputTypeTag::scaleA,
        DataCopyWrapperA, DataCopyWrapperB, DataCopyWrapperScaleA, DataCopyWrapperScaleB>::type;

    using BASE_MODULE = MatmulImplBase<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>;
    __aicore__ inline MatmulImpl() {}

    __aicore__ inline void Init(const TCubeTiling* __restrict cubeTiling, TPipe* tpipe = nullptr)
    {
        static_assert(
            !PhyPosIsL1(A_TYPE::pos) || (PhyPosIsGM(A_TYPE::srcPos) || PhyPosIsUB(A_TYPE::srcPos)),
            "A_TYPE::srcPos only support GM or UB(VECOUT) when A_TYPE::pos is L1 Buffer(TSCM).");
        static_assert(
            !PhyPosIsL1(A_TYPE::scalePosition) || (PhyPosIsGM(A_TYPE::srcScalePos) || PhyPosIsUB(A_TYPE::srcScalePos)),
            "A_TYPE::srcScalePos only support GM or UB(VECOUT) when A_TYPE::scalePosition is L1 Buffer(TSCM).");
        static_assert(
            !PhyPosIsL1(B_TYPE::pos) || (PhyPosIsGM(B_TYPE::srcPos) || PhyPosIsUB(B_TYPE::srcPos)),
            "B_TYPE::srcPos only support GM or UB(VECOUT) when B_TYPE::pos is L1 Buffer(TSCM).");
        static_assert(
            !PhyPosIsL1(B_TYPE::scalePosition) || (PhyPosIsGM(B_TYPE::srcScalePos) || PhyPosIsUB(B_TYPE::srcScalePos)),
            "B_TYPE::srcScalePos only support GM or UB(VECOUT) when B_TYPE::scalePosition is L1 Buffer(TSCM).");
        BASE_MODULE::Init(cubeTiling, tpipe);
    }

    __aicore__ inline void SetTensorScaleA(const GlobalTensor<SrcScaleT>& gm, bool isTransposeScaleA = false)
    {
        MATMUL_MODULE(CopyCubeInScaleA)->SetInput(gm, isTransposeScaleA);
        MATMUL_MODULE(Scheduler)->Reset();
    }

    __aicore__ inline void SetTensorScaleA(const LocalTensor<SrcScaleT>& leftMatrix, bool isTransposeScaleA = false)
    {
        MATMUL_MODULE(CopyCubeInScaleA)->SetInput(leftMatrix, isTransposeScaleA);
        MATMUL_MODULE(Scheduler)->Reset();
    }

    __aicore__ inline void SetTensorScaleB(const GlobalTensor<SrcScaleT>& gm, bool isTransposeScaleB = true)
    {
        MATMUL_MODULE(CopyCubeInScaleB)->SetInput(gm, isTransposeScaleB);
        MATMUL_MODULE(Scheduler)->Reset();
    }

    __aicore__ inline void SetTensorScaleB(const LocalTensor<SrcScaleT>& rightMatrix, bool isTransposeScaleB = true)
    {
        MATMUL_MODULE(CopyCubeInScaleB)->SetInput(rightMatrix, isTransposeScaleB);
        MATMUL_MODULE(Scheduler)->Reset();
    }

    // v310
    template <bool sync = true>
    __aicore__ inline void IterateAll(
        const GlobalTensor<DstT>& gm, uint8_t enAtomic = 0, bool enSequentialWrite = false, bool waitIterateAll = false,
        bool fakeMsg = false)
    {
        ASCENDC_ASSERT((!ToMatmulConfig(MM_CFG).isPartialOutput), {
            KERNEL_LOG(KERNEL_ERROR, "IterateAll is not supported for PartialOutput.");
        });
        while (BASE_MODULE::Iterate()) {
            BASE_MODULE::GetTensorC(gm, enAtomic);
        }
    }

    // v310
    template <bool sync = true>
    __aicore__ inline void IterateAll(const LocalTensor<DstT>& ubCmatrix, uint8_t enAtomic = 0)
    {
        ASCENDC_ASSERT((!ToMatmulConfig(MM_CFG).isPartialOutput), {
            KERNEL_LOG(KERNEL_ERROR, "IterateAll is not supported for PartialOutput.");
        });
        int64_t dstOffset = 0;
        while (BASE_MODULE::Iterate(false, ubCmatrix[dstOffset])) {
            if constexpr (PhyPosIsL0C(C_TYPE::pos)) {
                dstOffset += MATMUL_MODULE(Scheduler)->GetL0cOffset();
            }
            BASE_MODULE::GetTensorC(ubCmatrix, enAtomic);
        }
    }

    __aicore__ inline MatrixL1Addr GetMatrixL1Addr()
    {
        struct MatrixL1Addr matrixL1Addr;
        matrixL1Addr = BASE_MODULE::GetMatrixL1Addr();
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
        if constexpr (PhyMxScalePosIsUB<A_TYPE>()) {
            matrixL1Addr.l1aScaleAddr = MATMUL_MODULE(CubeInBufferScaleA)->GetBufferHeadAddr();
        }
        if constexpr (PhyMxScalePosIsUB<B_TYPE>()) {
            matrixL1Addr.l1bScaleAddr = MATMUL_MODULE(CubeInBufferScaleB)->GetBufferHeadAddr();
        }
#endif
        return matrixL1Addr;
    }

    __aicore__ inline void SetIntraScaleAId(uint8_t intraId)
    {
        MATMUL_MODULE(MatmulCrossCoreSync)->SetIntraScaleAId(intraId);
    }

    __aicore__ inline void SetIntraScaleBId(uint8_t intraId)
    {
        MATMUL_MODULE(MatmulCrossCoreSync)->SetIntraScaleBId(intraId);
    }

    friend __aicore__ inline void KfcSetIntraScaleAId(
        MatmulImpl<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>& mm, uint8_t intraId)
    {
        mm.SetIntraScaleAId(intraId);
    }

    friend __aicore__ inline void KfcSetIntraScaleBId(
        MatmulImpl<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG, MM_CB, MATMUL_POLICY>& mm, uint8_t intraId)
    {
        mm.SetIntraScaleBId(intraId);
    }
};
} // namespace AscendC

#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_MX_MATMUL_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_MX_MATMUL_IMPL_H__
#endif
