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
    "impl/tensor_api/arch/cube/l1_to_l0scalea/copy_impl/scalea.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file scalea.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0SCALEA_COPY_IMPL_SCALEA_H
#define IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0SCALEA_COPY_IMPL_SCALEA_H

#include "impl/tensor_api/arch/cube/l1_to_l0scalea/copy_impl/instruction.h"

namespace AscendC {
namespace Te {

class LoadDataL12L0MxScaleA3510 {
public:
    template <const CopyL12L0ScaleATrait& trait, typename T, typename U>
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
                "LoadDataL12L0MxScaleA3510 only supports the plain fractal layout "
                "((row0,row1),(col0,col1)) or the batch layout (B,((row0,row1),(col0,col1))).");
        }
    }

private:
    template <const CopyL12L0ScaleATrait& trait, typename T, typename U>
    __aicore__ inline static constexpr void CheckTemplate()
    {
        CheckDataType::CheckL12L0ScaleADataType<T, U>();
        CheckLayoutPattern<T, U>();
    }

    template <typename DstT, typename SrcT, typename DstLayoutT, typename SrcLayoutT>
    __aicore__ inline static void LoadDataFractal(
        const DstT& dst, const SrcT& src, const DstLayoutT& dstLayout, const SrcLayoutT& srcLayout)
    {
        uint16_t mStartPosition = 0;
        uint16_t kStartPosition = 0;
        auto mStep = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(dstLayout);
        auto kStep = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(dstLayout);
        auto srcStride = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(srcLayout) >> 5;
        auto dstStride = kStep;
        uint64_t mxDstAddr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.Data().Get()));
        LoadCbufToL0MxScaleA3510::LoadData(
            mxDstAddr, src, mStartPosition, kStartPosition, mStep, kStep, srcStride, dstStride);
    }

    template <const CopyL12L0ScaleATrait& trait, typename T, typename U>
    __aicore__ inline static void LoadDataImpl(const T& dst, const U& src)
    {
        LoadDataFractal(dst, src, dst.Layout(), src.Layout());
    }

    template <const CopyL12L0ScaleATrait& trait, typename T, typename U>
    __aicore__ inline static void BatchLoadDataImpl(const T& dst, const U& src)
    {
        auto dstNoBatchLayout = RemoveBatchDim(dst.Layout());
        auto srcNoBatchLayout = RemoveBatchDim(src.Layout());
        auto batchNum = Get<0>(dst.Layout().Shape());
        for (uint32_t i = 0; i < batchNum; ++i) {
            LoadDataFractal(
                dst(MakeCoord(i, MakeCoord(MakeCoord(0, 0), MakeCoord(0, 0)))),
                src(MakeCoord(i, MakeCoord(MakeCoord(0, 0), MakeCoord(0, 0)))), dstNoBatchLayout, srcNoBatchLayout);
        }
    }
};
} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0SCALEA_COPY_IMPL_SCALEA_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
