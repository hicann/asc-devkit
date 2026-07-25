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
    "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/nc1hwc02nc1hwc0.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file nc1hwc02nc1hwc0.h
 * \brief GM(NC1HWC0) -> L1(NC1HWC0) copy for conv feature maps.
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_NC1HWC02NC1HWC0_H
#define IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_NC1HWC02NC1HWC0_H

#include "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/instruction.h"

namespace AscendC {
namespace Te {

// GM(NC1HWC0) -> L1(NC1HWC0). Dispatched by CopyGM2L1Routing when both src/dst carry the
// NC1HWC0LayoutPtn tag. W is loaded fully when contiguous (one burst per C1 of H*W*C0);
// otherwise a per-H loop emits C1 bursts of W*C0 each.
class CopyGmToCbufNC1HWC02NC1HWC0 {
public:
    template <const CopyGM2L1Trait& trait, typename T, typename U>
    __aicore__ inline static void Run(const T& dst, const U& src)
    {
        using type = typename U::elementType;
        auto srcLayout = src.Layout();
        auto dstLayout = dst.Layout();

        // (N, C1, H, W, C0): C1 axis stride = H*W*C0, H axis stride = W*C0.
        uint8_t cacheMode = src.Engine().GetCacheMode();
        uint32_t c1 = Get<1>(dstLayout.Shape());
        uint32_t rowElems = Get<2>(dstLayout.Stride()); // W*C0 (dst packed H stride)

        uint64_t srcStride = Get<1>(srcLayout.Stride()) * sizeof(type); // burst_src_stride
        uint32_t dstStride = Get<1>(dstLayout.Stride()) * sizeof(type); // burst_dst_stride
        if constexpr (IsB4Type<type>) {
            srcStride = srcStride >> 1;
            dstStride = dstStride >> 1;
        }

        if (Get<2>(srcLayout.Stride()) == rowElems) {
            // W fully loaded: src H rows contiguous, one burst per C1 of H*W*C0.
            uint32_t blockLen = Get<1>(dstLayout.Stride()) * sizeof(type);
            if constexpr (IsB4Type<type>) {
                blockLen = blockLen >> 1;
            }
            CopyGmToCbufAlignV2Base::DataCopy(dst, src, c1, blockLen, 0, 0, cacheMode, srcStride, dstStride);
        } else {
            // W not fully loaded (sliced src): per-H loop, C1 bursts of W*C0 each.
            uint32_t h = Get<2>(dstLayout.Shape());
            uint32_t blockLen = rowElems * sizeof(type);
            if constexpr (IsB4Type<type>) {
                blockLen = blockLen >> 1;
            }
            for (uint32_t i = 0; i < h; ++i) {
                auto srcH = src(MakeCoord(0, 0, i, 0, 0));
                auto dstH = dst(MakeCoord(0, 0, i, 0, 0));
                CopyGmToCbufAlignV2Base::DataCopy(dstH, srcH, c1, blockLen, 0, 0, cacheMode, srcStride, dstStride);
            }
        }
    }
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_NC1HWC02NC1HWC0_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
