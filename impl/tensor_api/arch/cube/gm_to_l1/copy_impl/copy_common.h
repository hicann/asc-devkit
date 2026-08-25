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
    "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/copy_common.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file copy_common.h
 * \brief Shared helpers for the gm_to_l1 copy implementations.
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_COPY_COMMON_H
#define IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_COPY_COMMON_H

#include "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/instruction.h"

namespace AscendC {
namespace Te {

// Shared GM->L1 batch dispatch for the four copy ops (ND2Nz/DN2Nz/ND2Zn/DN2Zn). The op-specific
// single-matrix parameter extraction lives in CopyOp::EmitCopy; this routine handles the parts that
// are identical across ops: depth-based batch detection, stripping the leading B axis, and reading
// the per-batch counts/strides. CopyOp must provide static CheckTemplate<trait,T,U>() and
// EmitCopy(dst, src, srcLayout, dstLayout, matrixNum, srcMatrixStride, dstMatrixStride).
//   - Non-batch (depth 2/4): pass the full layouts, matrixNum=1, strides=0.
//   - Batch (depth 3/5): strip B with RemoveBatchDim (keeps pattern/trait), matrixNum/strides from
//     the B axis. The GM stride comes straight from the layout, so both bmk-contiguous and
//     mbk-non-contiguous memory are covered.
template <const CopyGM2L1Trait& trait, typename CopyOp, typename T, typename U>
__aicore__ inline void RunGmToL1Batched(const T& dst, const U& src)
{
    CopyOp::template CheckTemplate<trait, T, U>();
    constexpr auto srcDepth = NestingDepthV<decltype(src.Layout().Shape())>;
    if constexpr (srcDepth == THREE_DIM_DATA || srcDepth == FIVE_DIM_DATA) {
        auto srcLayout = src.Layout();
        auto dstLayout = dst.Layout();
        uint16_t matrixNum = Get<0>(srcLayout.Shape());
        uint64_t srcMatrixStride = Get<0>(srcLayout.Stride());
        uint32_t dstMatrixStride = Get<0>(dstLayout.Stride());
        CopyOp::EmitCopy(
            dst, src, RemoveBatchDim(srcLayout), RemoveBatchDim(dstLayout), matrixNum, srcMatrixStride,
            dstMatrixStride);
    } else {
        CopyOp::EmitCopy(dst, src, src.Layout(), dst.Layout(), 1, 0, 0);
    }
}

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_COPY_COMMON_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
