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
    "impl/tensor_api/arch/cube/l1_to_l0a/copy_impl/img2col.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file img2col.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0A_COPY_IMPL_IMG2COL_H
#define IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0A_COPY_IMPL_IMG2COL_H

#include "impl/tensor_api/arch/cube/l1_to_l0a/copy_impl/instruction.h"

namespace AscendC {
namespace Te {
class LoadDataL12L0AImg2Col {
public:
    template <const CopyL12L0ATrait& trait, typename DstTensor, typename SrcTensor, typename PadT>
    __aicore__ inline static void Run(const DstTensor& dst, const SrcTensor& src, const Img2ColParams<PadT>& params)
    {
        // PadT (the padValue type) should match the L0A dst element type so the padding register bit
        // pattern is correct; the caller picks it when constructing Img2ColParams<PadT>.
        auto dstLayout = dst.Layout();
        auto srcLayout = src.Layout();

        uint16_t kStartPt = 0;
        uint16_t mStartPt = 0;

        // src L1 NC1HWC0 (N, C1, H, W, C0): l1H/l1W are the feature-map dims, channelSize = C1*C0.
        uint16_t l1H = Get<2>(srcLayout.Shape());
        uint16_t l1W = Get<3>(srcLayout.Shape());
        uint16_t channelSize = Get<1>(srcLayout.Shape()) * Get<4>(srcLayout.Shape());

        LoadCbufToCaImg2Col::SetFMatrix(l1H, l1W, params.padList);
        LoadCbufToCaImg2Col::SetPadding(params.padValue);
        LoadCbufToCaImg2Col::SetRepeat(static_cast<uint16_t>(Std::ceil_division(params.mExtension, FRACTAL_FIXED)));
        LoadCbufToCaImg2Col::LoadData(
            dst, src, params.kExtension, params.mExtension, kStartPt, mStartPt, params.strideW, params.strideH,
            params.filterW, params.filterH, params.dilationFilterW, params.dilationFilterH, params.filterSizeW,
            params.filterSizeH, params.transpose, params.fMatrixCtrl, channelSize);
    }
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0A_COPY_IMPL_IMG2COL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
