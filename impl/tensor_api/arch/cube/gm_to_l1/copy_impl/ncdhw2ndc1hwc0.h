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
    "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/ncdhw2ndc1hwc0.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file ncdhw2ndc1hwc0.h
 * \brief GM(NCDHW) -> L1(NDC1HWC0) copy: one NCHW->NC1HWC0 (dn2nz) per depth slice.
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_NCDHW2NDC1HWC0_H
#define IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_NCDHW2NDC1HWC0_H

#include "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/instruction.h"

namespace AscendC {
namespace Te {

// GM(NCDHW) -> L1(NDC1HWC0). conv3D adds a depth axis D on top of conv2D's NCHW->NC1HWC0. Each depth
// slice d is an independent NCHW->NC1HWC0 (dn2nz path), so this loops over D and reuses the exact
// same per-slice parameters as CopyGmToCbufNCHW2NC1HWC0, only with indices shifted for the extra D
// dimension and a per-slice pointer offset on both src and dst.
//
//   src NCDHW  (N, C, D, H, W),        row-major stride (C*D*H*W, D*H*W, H*W, W, 1)
//   dst NDC1HWC0 (N, D, C1, H, W, C0), row-major stride (D*C1*H*W*C0, C1*H*W*C0, H*W*C0, W*C0, C0, 1)
//
// Per slice (fixed d): dnNum=H, nValue=W, dValue=C. Src C-axis step is Stride[1] (=D*H*W here, larger
// than conv2D's H*W because D sits between C and H) and src H-axis step is Stride[3] (=W). Dst nz
// strides: loop2=1, loop3=H*W, loop4=W. The depth step is Stride[2]=H*W on src and Stride[1]=C1*H*W*C0
// on dst.
class CopyGmToCbufNCDHW2NDC1HWC0 {
public:
    template <const CopyGM2L1Trait& trait, typename T, typename U>
    __aicore__ inline static void Run(const T& dst, const U& src)
    {
        using type = typename U::elementType;
        auto srcLayout = src.Layout();
        auto dstLayout = dst.Layout();

        // src NCDHW (N, C, D, H, W): dnNum=H, nValue=W, dValue=C.
        uint16_t depth = Get<2>(srcLayout.Shape());  // srcD (loop count)
        uint16_t dnNum = Get<3>(srcLayout.Shape());  // srcH
        uint16_t nValue = Get<4>(srcLayout.Shape()); // srcW
        uint32_t dValue = Get<1>(srcLayout.Shape()); // srcC
        // src stride (C*D*H*W, D*H*W, H*W, W, 1): Stride[1]=D*H*W (C step), Stride[3]=W (H step).
        uint64_t loop1SrcStride = static_cast<uint64_t>(Get<1>(srcLayout.Stride())) * sizeof(type); // C step
        uint64_t loop4SrcStride = static_cast<uint64_t>(Get<3>(srcLayout.Stride())) * sizeof(type); // H step

        // dst NDC1HWC0 (N, D, C1, H, W, C0): loop4DstStride=dstW=Shape[4]; loop3DstStride=dstH*dstW.
        uint16_t loop2DstStride = 1;
        uint16_t loop4DstStride = Get<4>(dstLayout.Shape());                             // dstW
        uint16_t loop3DstStride = Get<3>(dstLayout.Shape()) * Get<4>(dstLayout.Shape()); // dstH*dstW

        if constexpr (IsB4Type<type>) {
            dValue = dValue >> 1;
            loop4SrcStride = loop4SrcStride >> 1;
            loop1SrcStride = loop1SrcStride >> 1;
        }
        uint8_t cacheMode = src.Engine().GetCacheMode();

        // Per depth slice: index the slice with tensor(coord) to offset the address (engine moves by
        // Layout()(coord); pattern/trait preserved), same idiom as the batch-loop scenarios. The DMA
        // params are computed once above from the original layout and do not depend on d; only the
        // src/dst pointers move. src coord picks D at index 2 (NCDHW), dst at index 1 (NDC1HWC0).
        for (uint16_t d = 0; d < depth; ++d) {
            auto subSrc = src(MakeCoord(0, 0, d, 0, 0));    // NCDHW: engine += d*(H*W)
            auto subDst = dst(MakeCoord(0, d, 0, 0, 0, 0)); // NDC1HWC0: engine += d*(C1*H*W*C0)
            CopyGmToCbufMultiDn2nzInstr::DataCopy(
                subDst, subSrc, dnNum, loop2DstStride, loop3DstStride, loop4DstStride, loop1SrcStride, cacheMode,
                nValue, dValue, loop4SrcStride, false);
        }
    }
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_NCDHW2NDC1HWC0_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
