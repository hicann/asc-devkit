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
    "impl/tensor_api/arch/vector/ub_to_ub/copy_impl/zn2zn.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file zn2zn.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_UB_TO_UB_COPY_IMPL_ZN2ZN_H
#define IMPL_TENSOR_API_ARCH_VECTOR_UB_TO_UB_COPY_IMPL_ZN2ZN_H

#include "impl/tensor_api/arch/vector/ub_to_ub/copy_impl/data_copy.h"

namespace AscendC {
namespace Te {

class CopyUbufToUbufAlignV2ZN : private CopyUbufToUbufAlignV2Common {
public:
    template <const CopyUB2UBTrait& trait, typename T, typename U>
    __aicore__ inline static void Run(const T& dst, const U& src)
    {
        DataCopyImpl<trait, T, U>(dst, src);
    }

private:
    template <const CopyUB2UBTrait& trait, typename T, typename U>
    __aicore__ inline static constexpr void CheckTemplate()
    {
        CheckLayoutPattern<U, T>();
        CheckDataType::CheckUB2UBDataType<T, U>();
    }

    template <const CopyUB2UBTrait& trait, typename T, typename U>
    __aicore__ inline static void DataCopyImpl(const T& dst, const U& src)
    {
        using SrcType = typename U::elementType;
        using DstType = typename T::elementType;

        CheckTemplate<trait, T, U>();

        auto dstLayout = dst.Layout();
        auto srcLayout = src.Layout();

        uint16_t blockCount = GetElement<AttrInfo::Shape, AttrInfo::Row, 1>(srcLayout);
        uint32_t blockLen =
            GetTotalColumnShape(srcLayout) * GetElement<AttrInfo::Shape, AttrInfo::Row, 0>(srcLayout) * sizeof(SrcType);
        int64_t srcStride = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(srcLayout) * sizeof(SrcType);
        int64_t dstStride = GetElement<AttrInfo::Stride, AttrInfo::Row, 1>(dstLayout) * sizeof(DstType);

        EmitCopy(dst, src, blockCount, blockLen, srcStride, dstStride);
    }
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_VECTOR_UB_TO_UB_COPY_IMPL_ZN2ZN_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
