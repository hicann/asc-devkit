/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning \
    "impl/tensor_api/arch/cube/l1_to_l0a/copy_impl/zn2nzb8b4.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file zn2nzb8b4.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0A_COPY_IMPL_ZN2NZB8B4_H
#define IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0A_COPY_IMPL_ZN2NZB8B4_H

#include "impl/tensor_api/arch/cube/l1_to_l0a/copy_impl/instruction.h"

namespace AscendC {
namespace Te {
class LoadDataL12L0AZN2NZB8B4 {
public:
    template <const CopyL12L0ATrait& trait, typename T, typename U>
    __aicore__ inline static void Run(const T& dst, const U& src)
    {
        CheckTemplate<trait, T, U>();
        if constexpr (T::layoutType::depth == FIVE_DIM_DATA) {
            BatchLoadDataImpl<trait, T, U>(dst, src);
        } else if constexpr (T::layoutType::depth == FOUR_DIM_DATA) {
            LoadDataImpl<trait, T, U>(dst, src);
        } else {
            static_assert(
                T::layoutType::depth == FOUR_DIM_DATA || T::layoutType::depth == FIVE_DIM_DATA,
                "LoadDataL12L0AZN2NZB8B4 only supports the plain fractal layout "
                "((row0,row1),(col0,col1)) or the batch layout (B,((row0,row1),(col0,col1))).");
        }
    }

private:
    static constexpr uint8_t M_STEP_MIN_VAL_B4 = 4;
    static constexpr uint8_t M_STEP_MIN_VAL_B8 = 2;

    template <const CopyL12L0ATrait& trait, typename T, typename U>
    __aicore__ inline static constexpr void CheckTemplate()
    {
        CheckLayoutPattern<T, U>();
        CheckDataType::CheckL12L0ADataType<T, U>();
    }

    template <uint8_t mStepMinVal, typename T, typename U>
    __aicore__ inline static void LoadDataImplSplit(
        const T& dst, const U& src, uint16_t mStartPosition, uint16_t kStartPosition, uint8_t mStep, uint8_t kStep,
        int16_t srcStride, uint16_t dstStride)
    {
        uint16_t mLoop = mStep / mStepMinVal;
        mStep = mStepMinVal;
        for (uint16_t idx = 0; idx < mLoop; ++idx) {
            auto sliceDst = dst(MakeCoord(MakeCoord(0, 0), MakeCoord(0, idx)));
            LoadCbufToCa::LoadData<true>(
                sliceDst, src, mStartPosition, kStartPosition, mStep, kStep, srcStride, dstStride);
            mStartPosition += mStepMinVal;
        }
    }

    template <const CopyL12L0ATrait& trait, typename DstT, typename SrcT, typename DstLayoutT, typename SrcLayoutT>
    __aicore__ inline static void LoadDataFractal(
        const DstT& dst, const SrcT& src, const DstLayoutT& dstLayout, const SrcLayoutT& srcLayout)
    {
        using DstType = typename DstT::elementType;
        uint16_t mStartPosition = 0;
        uint16_t kStartPosition = 0;
        auto m1 = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(srcLayout) *
                      GetElement<AttrInfo::Shape, AttrInfo::Row, 0>(srcLayout) -
                  GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(dstLayout) *
                      GetElement<AttrInfo::Shape, AttrInfo::Row, 0>(dstLayout);
        auto mStep = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(dstLayout) *
                     GetElement<AttrInfo::Shape, AttrInfo::Column, 0>(dstLayout) / FRACTAL_FIXED;
        auto kStep = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(srcLayout);
        // Zn -> Nz
        constexpr uint32_t STRIDE_UNIT = C0_ELEMENT<DstType> * FRACTAL_FIXED;
        auto srcStride = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(srcLayout) / STRIDE_UNIT;
        auto dstStride = GetElement<AttrInfo::Stride, AttrInfo::Column, 1>(dstLayout) / STRIDE_UNIT;
        if (m1 < FRACTAL_FIXED) {
            LoadCbufToCa::LoadData<true>(dst, src, mStartPosition, kStartPosition, mStep, kStep, srcStride, dstStride);
        } else if constexpr (IsB4Type<DstType>) {
            LoadDataImplSplit<M_STEP_MIN_VAL_B4>(
                dst, src, mStartPosition, kStartPosition, mStep, kStep, srcStride, dstStride);
        } else {
            LoadDataImplSplit<M_STEP_MIN_VAL_B8>(
                dst, src, mStartPosition, kStartPosition, mStep, kStep, srcStride, dstStride);
        }
    }

    template <const CopyL12L0ATrait& trait, typename T, typename U>
    __aicore__ inline static void LoadDataImpl(const T& dst, const U& src)
    {
        LoadDataFractal<trait>(dst, src, dst.Layout(), src.Layout());
    }

    template <const CopyL12L0ATrait& trait, typename T, typename U>
    __aicore__ inline static void BatchLoadDataImpl(const T& dst, const U& src)
    {
        auto dstLayout = dst.Layout();
        auto srcLayout = src.Layout();
        auto dstNoBatchLayout = RemoveBatchDim(dstLayout);
        auto srcNoBatchLayout = RemoveBatchDim(srcLayout);
        auto batchNum = Get<0>(dstLayout.Shape());
        for (uint32_t i = 0; i < batchNum; ++i) {
            LoadDataFractal<trait>(
                MakeSingleBatchSubTensor(dst, i), MakeSingleBatchSubTensor(src, i), dstNoBatchLayout, srcNoBatchLayout);
        }
    }
};
} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0A_COPY_IMPL_ZN2NZB8B4_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
