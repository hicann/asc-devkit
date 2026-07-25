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
    "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/nchw2nc1hwc0.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file nchw2nc1hwc0.h
 * \brief GM(NCHW) -> L1(NC1HWC0) copy via the dn2nz DMA path.
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_NCHW2NC1HWC0_H
#define IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_NCHW2NC1HWC0_H

#include "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/instruction.h"

namespace AscendC {
namespace Te {

// GM(NCHW) -> L1(NC1HWC0). NCHW is the HW<->C transpose of NHWC, so it uses the dn2nz DMA path
// (column-major src) instead of nd2nz. Maps: dnNum=H, nValue=W, dValue=C. Src strides come straight
// from the NCHW layout (row-major (C*H*W, H*W, W, 1)): the H*W axis (Stride[1]) is the dnNum step,
// so loop1SrcStride = H*W*sizeof = Stride[1]*sizeof; the W axis (Stride[2]=W) is the nValue step,
// so loop4SrcStride = W*sizeof = Stride[2]*sizeof. Dst strides: loop2DstStride=1, loop3DstStride=H*W,
// loop4DstStride=W. H*W is the only value not directly readable from the dst stride (which carries
// C0), so it is the single product computed here.
class CopyGmToCbufNCHW2NC1HWC0 {
public:
    template <const CopyGM2L1Trait& trait, typename T, typename U>
    __aicore__ inline static void Run(const T& dst, const U& src)
    {
        using type = typename U::elementType;
        auto srcLayout = src.Layout();
        auto dstLayout = dst.Layout();

        // src NCHW (N, C, H, W): dnNum=H, nValue=W, dValue=C.
        uint16_t dnNum = Get<2>(srcLayout.Shape());  // srcH
        uint16_t nValue = Get<3>(srcLayout.Shape()); // srcW
        uint32_t dValue = Get<1>(srcLayout.Shape()); // srcC
        // NCHW row-major stride: (C*H*W, H*W, W, 1) -> Stride[1]=H*W (dnNum step), Stride[2]=W (nValue step).
        uint64_t loop1SrcStride = static_cast<uint64_t>(Get<1>(srcLayout.Stride())) * sizeof(type); // H*W*sizeof
        uint64_t loop4SrcStride = static_cast<uint64_t>(Get<2>(srcLayout.Stride())) * sizeof(type); // W*sizeof

        // dst NC1HWC0 (N, C1, H, W, C0): loop4DstStride = dstW = Shape[3]; loop3DstStride = dstH*dstW
        // = Shape[2]*Shape[3] (no stride carries the bare H*W product).
        uint16_t loop2DstStride = 1;
        uint16_t loop4DstStride = Get<3>(dstLayout.Shape());                             // dstW
        uint16_t loop3DstStride = Get<2>(dstLayout.Shape()) * Get<3>(dstLayout.Shape()); // dstH*dstW

        if constexpr (IsB4Type<type>) {
            dValue = dValue >> 1;
            loop4SrcStride = loop4SrcStride >> 1;
            loop1SrcStride = loop1SrcStride >> 1;
        }
        uint8_t cacheMode = src.Engine().GetCacheMode();

        CopyGmToCbufMultiDn2nzInstr::DataCopy(
            dst, src, dnNum, loop2DstStride, loop3DstStride, loop4DstStride, loop1SrcStride, cacheMode, nValue, dValue,
            loop4SrcStride, false);
    }
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_NCHW2NC1HWC0_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
