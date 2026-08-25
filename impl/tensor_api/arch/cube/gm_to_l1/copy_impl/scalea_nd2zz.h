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
    "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/scalea_nd2zz.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file scalea_nd2zz.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_SCALEA_ND2ZZ_H
#define IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_SCALEA_ND2ZZ_H

#include "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/copy_common.h"

namespace AscendC {
namespace Te {

class CopyGmToCbufScaleAND2Zz {
public:
    template <const CopyGM2L1Trait& trait, typename T, typename U>
    __aicore__ inline static void Run(const T& dst, const U& src)
    {
        RunGmToL1Batched<trait, CopyGmToCbufScaleAND2Zz, T, U>(dst, src);
    }

    template <const CopyGM2L1Trait& trait, typename T, typename U>
    __aicore__ inline static constexpr void CheckTemplate()
    {
        CheckLayoutPattern<U, T>();
        CheckDataType::CheckGm2L1ScaleDataType<T, U>();
    }

    template <typename T, typename U, typename SrcLayout, typename DstLayout>
    __aicore__ inline static void EmitCopy(
        const T& dst, const U& src, const SrcLayout& srcLayout, const DstLayout& dstLayout, uint16_t dnNum,
        uint64_t srcDnMatrixStride, uint32_t dstNzMatrixStride)
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
        uint16_t dstBRowStride = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(dstLayout);

        uint16_t nValue = srcColShape >> 1; // use b16 for DN2NZ, so nValue = srcColShape / 2
        uint16_t dValue = srcRowShape;
        uint16_t dstNzNStride = 1;

        uint64_t loop1SrcStride = srcRowStride * sizeof(type);
        uint64_t loop4SrcStride = srcDnMatrixStride * sizeof(type);

        uint16_t loop2DstStride = dstNzNStride;                             // loop2_dst_stride = dst_nz_n_stride
        uint16_t loop3DstStride = dstBRowStride * sizeof(type) / C0_SIZE<>; // loop3_dst_stride = dst_nz_c0_Stride
        uint16_t loop4DstStride = dstNzMatrixStride * sizeof(type) / C0_SIZE<>;
        uint8_t cacheMode = src.Engine().GetCacheMode();
        // fp8 scale use b16 for movement
        CopyGmToCbufMultiDn2nzInstr::CopyGmToCbufMultiDn2nz(
            (__cbuf__ half*)(dst.Data().Get()), (__gm__ half*)(src.Data().Get()), dnNum, loop2DstStride, loop3DstStride,
            loop4DstStride, loop1SrcStride, cacheMode, nValue, dValue, loop4SrcStride, false);
    }
};

} // namespace Te
} // namespace AscendC

#endif

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
