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
    "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/zn2zn.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file zn2zn.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_ZN2ZN_H
#define IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_ZN2ZN_H

#include "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/copy_common.h"

namespace AscendC {
namespace Te {

class CopyGmToCbufAlignV2ZN {
public:
    template <const CopyGM2L1Trait& trait, typename T, typename U>
    __aicore__ inline static void Run(const T& dst, const U& src)
    {
        RunGmToL1Batched<trait, CopyGmToCbufAlignV2ZN, T, U>(dst, src);
    }

    template <const CopyGM2L1Trait& trait, typename T, typename U>
    __aicore__ inline static constexpr void CheckTemplate()
    {
        CheckLayoutPattern<U, T>();
        CheckDataType::CheckGm2L1AlignV2NDDataType<T, U>();
    }

    // Extracts single-matrix parameters from the (batch-stripped) src/dst layouts and emits the
    // copy. matrixNum carries the batch dimension (1 when there is no batch). The align_v2
    // instruction has no hardware batch loop, so the batch axis is folded into blockCount: ZN
    // matrices are laid out along the row (big-fractal) direction, so matrixNum matrices of
    // bigFractalSize big-fractals each become matrixNum * bigFractalSize contiguous blocks that
    // share the same per-big-fractal stride. srcNdMatrixStride/dstNzMatrixStride are unused here
    // because that contiguity makes the single srcStride/dstStride describe the whole sequence.
    template <typename T, typename U, typename SrcLayout, typename DstLayout>
    __aicore__ inline static void EmitCopy(const T& dst, const U& src, const SrcLayout& srcLayout,
                                           const DstLayout& dstLayout, uint16_t matrixNum,
                                           uint64_t srcNdMatrixStride, uint32_t dstNzMatrixStride)
    {
        using type = typename U::elementType;

        auto smallFractalSize = GetElement<AttrInfo::Shape, AttrInfo::Column, 0>(srcLayout) *
                                GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(srcLayout);
        auto bigFractalSize = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(srcLayout);
        auto srcStrideSize = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(srcLayout);
        auto dstStrideSize = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(dstLayout);

        uint8_t leftPaddingCnt = 0;
        uint8_t rightPaddingCnt = 0;
        uint8_t cacheMode = src.Engine().GetCacheMode();

        auto blockCount = bigFractalSize * matrixNum;
        auto blockLen = smallFractalSize * C0_SIZE<>;
        auto srcStride = srcStrideSize * sizeof(type);
        auto dstStride = dstStrideSize * sizeof(type);
        if constexpr (IsB4Type<type>) {
            // move fp4 as b8, need to be divided by 2
            srcStride = srcStride >> 1;
            dstStride = dstStride >> 1;
        }
        CopyGmToCbufAlignV2Base::DataCopy(
            dst, src, blockCount, blockLen, leftPaddingCnt, rightPaddingCnt, cacheMode, srcStride, dstStride);
    }
};
} // namespace Te
} // namespace AscendC

#endif

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
