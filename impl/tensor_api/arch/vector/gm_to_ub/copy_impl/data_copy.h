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
    "impl/tensor_api/arch/vector/gm_to_ub/copy_impl/data_copy.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file data_copy.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_GM_TO_UB_COPY_IMPL_DATA_COPY_H
#define IMPL_TENSOR_API_ARCH_VECTOR_GM_TO_UB_COPY_IMPL_DATA_COPY_H

#include "impl/tensor_api/utils/utils_impl.h"
#include "impl/tensor_api/arch/vector/gm_to_ub/copy_impl/instruction.h"

namespace AscendC {
namespace Te {

struct CopyGM2UBTrait {};

class CopyGmToUbufAlignV2Common {
protected:
    template <typename T, typename U>
    __aicore__ inline static void EmitCopy(
        const T& dst, const U& src, uint16_t blockCount, uint32_t blockLen, int64_t srcStride, int64_t dstStride)
    {
        using SrcType = typename U::elementType;
        using DstType = typename T::elementType;

        if constexpr (IsB4Type<SrcType>) {
            blockLen = blockLen >> 1;
            srcStride = srcStride >> 1;
        }

        if constexpr (IsB4Type<DstType>) {
            dstStride = dstStride >> 1;
        }

        constexpr uint8_t leftPaddingCnt = 0;
        constexpr uint8_t rightPaddingCnt = 0;
        uint8_t cacheMode = src.Engine().GetCacheMode();
        CopyGmToUbufAlignV2Instr::DataCopy(
            dst, src, blockCount, blockLen, leftPaddingCnt, rightPaddingCnt, srcStride, dstStride, cacheMode);
    }
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_VECTOR_GM_TO_UB_COPY_IMPL_DATA_COPY_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
