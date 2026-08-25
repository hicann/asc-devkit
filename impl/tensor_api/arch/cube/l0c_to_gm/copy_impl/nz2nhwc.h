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
    "impl/tensor_api/arch/cube/l0c_to_gm/copy_impl/nz2nhwc.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file nz2nhwc.h
 * \brief L0C(NZ) -> GM(NHWC) copy via the nz2nd fixpipe path (no quant).
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_L0C_TO_GM_COPY_IMPL_NZ2NHWC_H
#define IMPL_TENSOR_API_ARCH_CUBE_L0C_TO_GM_COPY_IMPL_NZ2NHWC_H

#include "impl/tensor_api/arch/cube/l0c_to_gm/copy_impl/instruction.h"

namespace AscendC {
namespace Te {

class DataCopyL0C2GMNZ2NHWC {
public:
    template <const CopyL0C2GMTrait& trait, typename T, typename U>
    __aicore__ inline static void Run(const T& dst, const U& src, const FixpipeParams& params)
    {
        constexpr QuantMode_t quantPre = GetQuantMode<trait.roundMode, T, U>();
        CheckDataType::CheckL0C2GmDataType<quantPre, T, U>();
        SetRegisterImpl<T, U>(dst, src);

        auto dstLayout = dst.Layout();
        auto srcLayout = src.Layout();

        // dstStride = C = Stride[2] (row-major NHWC stride is (H*W*C, W*C, C, 1)).
        uint32_t nSize = Get<3>(dstLayout.Shape());
        uint32_t mSize = Get<1>(dstLayout.Shape()) * Get<2>(dstLayout.Shape());
        uint32_t srcStride = GetElement<AttrInfo::Stride, AttrInfo::Column, 1>(srcLayout) / FRACTAL_FIXED;
        uint32_t dstStride = Get<2>(dstLayout.Stride());

        uint8_t cacheMode = dst.Engine().GetCacheMode();
        bool reluEn = trait.enableRelu;
        uint8_t unitFlag = params.unitFlag;
        bool isChannelSplit = trait.enableChannelSplit;

        CopyMatrixCcToGmInstr::DataCopy<quantPre, T, U>(
            dst, src, nSize, mSize, srcStride, dstStride, cacheMode, reluEn, unitFlag, isChannelSplit, true, false);
    }
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_CUBE_L0C_TO_GM_COPY_IMPL_NZ2NHWC_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
