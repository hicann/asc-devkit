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
    "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/nhwc2nc1hwc0.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file nhwc2nc1hwc0.h
 * \brief GM(NHWC) -> L1(NC1HWC0) copy via the nd2nz DMA path.
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_NHWC2NC1HWC0_H
#define IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_NHWC2NC1HWC0_H

#include "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/instruction.h"

namespace AscendC {
namespace Te {

class CopyGmToCbufNHWC2NC1HWC0 {
public:
    template <const CopyGM2L1Trait& trait, typename T, typename U>
    __aicore__ inline static void Run(const T& dst, const U& src)
    {
        using type = typename U::elementType;
        auto srcLayout = src.Layout();
        auto dstLayout = dst.Layout();

        // src NHWC (N, H, W, C): take the H/W/C axes. N is the leading batch axis (kept as N=1 here).
        uint16_t ndNum = Get<1>(srcLayout.Shape());  // srcH
        uint16_t nValue = Get<2>(srcLayout.Shape()); // srcW
        uint32_t dValue = Get<3>(srcLayout.Shape()); // srcC
        // NHWC row-major stride: (H*W*C, W*C, C, 1) -> stride[1]=W*C, stride[2]=C.
        uint64_t loop4SrcStride = static_cast<uint64_t>(Get<1>(srcLayout.Stride())) * sizeof(type); // srcW*srcC
        uint64_t loop1SrcStride = static_cast<uint64_t>(Get<2>(srcLayout.Stride())) * sizeof(type); // srcC

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

        CopyGmToCbufMultiNd2nzInstr::DataCopy(
            dst, src, ndNum, loop2DstStride, loop3DstStride, loop4DstStride, loop1SrcStride, cacheMode, nValue, dValue,
            loop4SrcStride, false);
    }
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_NHWC2NC1HWC0_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
