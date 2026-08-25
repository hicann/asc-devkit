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
    "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/nd2zn.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file nd2zn.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_ND2ZN_H
#define IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_ND2ZN_H

#include "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/copy_common.h"

namespace AscendC {
namespace Te {

class CopyGmToCbufMultiND2Zn {
public:
    template <const CopyGM2L1Trait& trait, typename T, typename U>
    __aicore__ inline static void Run(const T& dst, const U& src)
    {
        RunGmToL1Batched<trait, CopyGmToCbufMultiND2Zn, T, U>(dst, src);
    }

    template <const CopyGM2L1Trait& trait, typename T, typename U>
    __aicore__ inline static constexpr void CheckTemplate()
    {
        CheckLayoutPattern<U, T>();
        CheckDataType::CheckGm2L1DataType<T, U>();
    }

    // Extracts single-matrix parameters from the (batch-stripped) src/dst layouts and emits the
    // instruction. ndNum/srcNdMatrixStride/dstNzMatrixStride carry the batch dimension (1/0/0 when
    // there is no batch). The src/dst pattern is read from the original tensor type U/T.
    template <typename T, typename U, typename SrcLayout, typename DstLayout>
    __aicore__ inline static void EmitCopy(
        const T& dst, const U& src, const SrcLayout& srcLayout, const DstLayout& dstLayout, uint16_t ndNum,
        uint64_t srcNdMatrixStride, uint32_t dstNzMatrixStride)
    {
        using type = typename U::elementType;

        uint32_t srcRowShape;
        uint32_t srcColShape;
        uint32_t srcRowStride;
        if constexpr (IsSatisfiedPtnFormatV<U, NDLayoutPtn>) {
            srcRowShape = GetElement<AttrInfo::Shape, AttrInfo::Row>(srcLayout);
            srcColShape = GetElement<AttrInfo::Shape, AttrInfo::Column>(srcLayout);
            srcRowStride = GetElement<AttrInfo::Stride, AttrInfo::Row>(srcLayout);
        } else {
            srcRowShape = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(srcLayout);
            srcColShape = GetElement<AttrInfo::Shape, AttrInfo::Column, 1>(srcLayout);
            srcRowStride = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(srcLayout);
        }
        auto dstBRowStride = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(dstLayout);

        uint16_t nValue = srcColShape;
        uint32_t dValue = srcRowShape;
        uint64_t srcDValue = srcRowStride;

        uint16_t dstNzC0Stride = dstBRowStride / C0_ELEMENT<type>;
        uint16_t dstNzNStride = 1;

        uint64_t loop1SrcStride = srcDValue * sizeof(type);
        uint64_t loop4SrcStride = srcNdMatrixStride * sizeof(type);

        uint16_t loop2DstStride = dstNzNStride;  // loop2_dst_stride = dst_nz_n_stride
        uint16_t loop3DstStride = dstNzC0Stride; // loop3_dst_stride = dst_nz_c0_Stride
        // loop4_dst_stride: dst_nz_matrix_stride * size_of_dst_type / C0_size
        uint16_t loop4DstStride = static_cast<uint16_t>(dstNzMatrixStride / C0_ELEMENT<type>);

        uint8_t cacheMode = src.Engine().GetCacheMode();

        CopyGmToCbufMultiDn2nzInstr::DataCopy(
            dst, src, ndNum, loop2DstStride, loop3DstStride, loop4DstStride, loop1SrcStride, cacheMode, nValue, dValue,
            loop4SrcStride, false);
    }
};

} // namespace Te
} // namespace AscendC

#endif

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
