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
    "impl/tensor_api/arch/cube/l1_to_l0a/copy_impl/img2col3d.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file img2col3d.h
 * \brief conv3D img2col: L1(NDC1HWC0) -> L0A(NZ). Same hardware path as the conv2D img2col; the depth
 *        axis D is merged with C1 into the channel dimension via channelSize = D*C1*C0.
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0A_COPY_IMPL_IMG2COL3D_H
#define IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0A_COPY_IMPL_IMG2COL3D_H

#include "impl/tensor_api/arch/cube/l1_to_l0a/copy_impl/instruction.h"

namespace AscendC {
namespace Te {
// conv3D img2col. The depth axis D is merged with C1 into the channel dimension: NDC1HWC0's memory
// has the D axis (stride C1*H*W*C0) immediately outside C1 (stride H*W*C0), so the D*C1 blocks of C0
// are C0 groups spaced uniformly by H*W*C0 and can be treated as a single channelSize = D*C1*C0. The
// caller therefore passes the whole NDC1HWC0 tensor (no per-depth slicing) and the img2col reads one
// (H, W) feature map over the merged D*C1*C0 channels. The underlying LoadData instruction is
// unchanged; only channelSize differs from the conv2D path (C1*C0).
class LoadDataL12L0AImg2Col3D {
public:
    template <const CopyL12L0ATrait& trait, typename DstTensor, typename SrcTensor, typename PadT>
    __aicore__ inline static void Run(const DstTensor& dst, const SrcTensor& src, const Img2ColParams<PadT>& params)
    {
        // PadT (the padValue type) should match the L0A dst element type so the padding register bit
        // pattern is correct; the caller picks it when constructing Img2ColParams<PadT>.
        auto srcLayout = src.Layout();

        // Window start offsets come from params (default 0), same as the 2D img2col path.
        uint16_t kStartPt = params.kStartPt;
        uint16_t mStartPt = params.mStartPt;

        // src L1 NDC1HWC0 (N, D, C1, H, W, C0): l1H/l1W at index 3/4; D and C1 merge into the channel
        // axis -> channelSize = D*C1*C0 = Shape[1]*Shape[2]*Shape[5].
        uint16_t l1H = Get<3>(srcLayout.Shape());
        uint16_t l1W = Get<4>(srcLayout.Shape());
        uint16_t channelSize = Get<1>(srcLayout.Shape()) * Get<2>(srcLayout.Shape()) * Get<5>(srcLayout.Shape());

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

#endif // IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0A_COPY_IMPL_IMG2COL3D_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
