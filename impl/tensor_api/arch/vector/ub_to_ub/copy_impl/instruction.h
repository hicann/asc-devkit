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
    "impl/tensor_api/arch/vector/ub_to_ub/copy_impl/instruction.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file instruction.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_UB_TO_UB_COPY_IMPL_INSTRUCTION_H
#define IMPL_TENSOR_API_ARCH_VECTOR_UB_TO_UB_COPY_IMPL_INSTRUCTION_H

#include "impl/tensor_api/tensor/pointer_pattern.h"
#include "impl/tensor_api/tensor/tensor_impl.h"
#include "impl/tensor_api/arch/utils/arch_utils.h"

namespace AscendC {
namespace Te {

class CopyUbufToUbufInstr {
public:
    template <typename T, typename U, typename... Params>
    __aicore__ inline static void DataCopy(const T& dst, const U& src, const Params&... params)
    {
        using srcType = typename U::elementType;
        if constexpr (sizeof(srcType) == 1) {
            CopyUbufToUbuf((__ubuf__ uint8_t*)(dst.Data().Get()), (__ubuf__ uint8_t*)(src.Data().Get()), params...);
        } else if constexpr (sizeof(srcType) == 2) {
            CopyUbufToUbuf((__ubuf__ uint16_t*)(dst.Data().Get()), (__ubuf__ uint16_t*)(src.Data().Get()), params...);
        } else if constexpr (sizeof(srcType) == 4) {
            CopyUbufToUbuf((__ubuf__ uint32_t*)(dst.Data().Get()), (__ubuf__ uint32_t*)(src.Data().Get()), params...);
        } else if constexpr (sizeof(srcType) == 8) {
            CopyUbufToUbuf((__ubuf__ uint32_t*)(dst.Data().Get()), (__ubuf__ uint32_t*)(src.Data().Get()), params...);
        } else {
            static_assert(
                sizeof(srcType) == 1 || sizeof(srcType) == 2 || sizeof(srcType) == 4 || sizeof(srcType) == 8,
                "Unsupported data type size for CopyUbufToUbuf");
        }
    }

private:
    template <typename T>
    __aicore__ inline static void CopyUbufToUbuf(
        __ubuf__ T* dst, __ubuf__ T* src, const uint16_t blockCount, const uint32_t blockLen, const int64_t srcStride,
        const int64_t dstStride)
    {
        if ASCEND_IS_AIC {
            return;
        }

        asc_copy_ub2ub(dst, src, blockCount, blockLen, srcStride, dstStride);
    }
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_VECTOR_UB_TO_UB_COPY_IMPL_INSTRUCTION_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
