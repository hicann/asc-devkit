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
    "impl/tensor_api/arch/vector/ub_to_gm/copy_impl/dn2dn.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file dn2dn.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_UB_TO_GM_COPY_IMPL_DN2DN_H
#define IMPL_TENSOR_API_ARCH_VECTOR_UB_TO_GM_COPY_IMPL_DN2DN_H

#include "impl/tensor_api/arch/vector/ub_to_gm/copy_impl/data_copy.h"

namespace AscendC {
namespace Te {

class CopyUbufToGmAlignV2DN : private CopyUbufToGmAlignV2Common {
public:
    template <const CopyUB2GMTrait& trait, typename T, typename U>
    __aicore__ inline static void Run(const T& dst, const U& src)
    {
        DataCopyImpl<trait, T, U>(dst, src);
    }

private:
    template <const CopyUB2GMTrait& trait, typename T, typename U>
    __aicore__ inline static constexpr void CheckTemplate()
    {
        CheckLayoutPattern<U, T>();
        CheckDataType::CheckUB2GMDataType<T, U>();
    }

    template <const CopyUB2GMTrait& trait, typename T, typename U>
    __aicore__ inline static void DataCopyImpl(const T& dst, const U& src)
    {
        using SrcType = typename U::elementType;
        using DstType = typename T::elementType;

        CheckTemplate<trait, T, U>();

        auto dstLayout = dst.Layout();
        auto srcLayout = src.Layout();

        uint16_t blockCount = GetTotalColumnShape(srcLayout);
        uint32_t blockLen =
            Std::min(GetTotalRowShape(srcLayout) * sizeof(SrcType), GetTotalRowShape(dstLayout) * sizeof(DstType));
        int64_t srcStride = GetElement<AttrInfo::Stride, AttrInfo::Column, 1>(srcLayout) * sizeof(SrcType);
        int64_t dstStride = GetElement<AttrInfo::Stride, AttrInfo::Column, 1>(dstLayout) * sizeof(DstType);

        EmitCopy(dst, src, blockCount, blockLen, srcStride, dstStride);
    }
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_VECTOR_UB_TO_GM_COPY_IMPL_DN2DN_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
